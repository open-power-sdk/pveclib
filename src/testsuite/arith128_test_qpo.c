/*
 Copyright (c) [2022, 2023] IBM Corporation.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 arith128_test_qpo.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Jul 26, 2023
 */


#include <stdint.h>
#include <stdio.h>
#include <float.h>

#include "arith128.h"
#include <testsuite/arith128_print.h>
#include <pveclib/vec_int128_ppc.h>
#include <pveclib/vec_f128_ppc.h>
#include <testsuite/arith128_test_f128.h>
#include <testsuite/arith128_test_qpo.h>

//#define __DEBUG_PRINT__ 1
//#define __DEBUG_PRINT__ 2
#if (__DEBUG_PRINT__ >= 2)
__binary128
db_vec_xsmulqpo (__binary128 vfa, __binary128 vfb)
{
  __binary128 result;
#if defined (_ARCH_PWR9) && (__GNUC__ > 7)
#if defined (__FLOAT128__) && (__GNUC__ > 8)
  // earlier GCC versions generate extra data moves for this.
  result = __builtin_mulf128_round_to_odd (vfa, vfb);
#else
  // No extra data moves here.
  __asm__(
      "xsmulqpo %0,%1,%2"
      : "=v" (result)
      : "v" (vfa), "v" (vfb)
      : );
#endif
#elif  defined (_ARCH_PWR8)
  vui64_t q_exp, a_exp, b_exp, x_exp;
  vui128_t q_sig, a_sig, b_sig, p_sig_h, p_sig_l, p_odd;
  vui32_t q_sign,  a_sign,  b_sign;
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  const vui64_t q_zero = { 0, 0 };
  const vui64_t q_ones = { -1, -1 };
  const vui64_t exp_bias = (vui64_t) CONST_VINT64_DW( 0x3fff, 0x3fff );
  const vi64_t exp_min = (vi64_t) CONST_VINT64_DW( 1, 1 );
  const vui64_t exp_dnrm = (vui64_t) CONST_VINT64_DW( 0, 0 );
  const vui64_t q_expnaninf = (vui64_t) CONST_VINT64_DW( 0x7fff, 0x7fff );
  const vui64_t q_expmax = (vui64_t) CONST_VINT64_DW( 0x7ffe, 0x7ffe );
  const vui32_t sigov = CONST_VINT128_W(0x0001ffff, -1, -1, -1);

  print_vfloat128x("db_vec_xsmulqpo vfa=  ", vfa);
  print_vfloat128x("                vfb=  ", vfb);

  a_exp = vec_xsxexpqp (vfa);
  a_sig = vec_xsxsigqp (vfa);
  a_sign = vec_and_bin128_2_vui32t (vfa, signmask);
  print_vint128x (" sign(vfa):", (vui128_t) a_sign);
  print_vint128x (" exp (vfa):", (vui128_t) a_exp);
  print_vint128x (" sig (vfa):", (vui128_t) a_sig);
  b_exp = vec_xsxexpqp (vfb);
  b_sig = vec_xsxsigqp (vfb);
  b_sign = vec_and_bin128_2_vui32t (vfb, signmask);
  print_vint128x (" sign(vfb):", (vui128_t) b_sign);
  print_vint128x (" exp (vfb):", (vui128_t) b_exp);
  print_vint128x (" sig (vfb):", (vui128_t) b_sig);
  x_exp = vec_mrgahd ((vui128_t) a_exp, (vui128_t) b_exp);
  q_sign = vec_xor (a_sign, b_sign);

//  if (vec_all_isfinitef128 (vfa) && vec_all_isfinitef128 (vfb))
//  The above can be optimized to the following
  if (vec_cmpud_all_lt (x_exp, q_expnaninf))
    {
      const vui32_t sigovt = CONST_VINT128_W(0x0000ffff, -1, -1, -1);
      const vui64_t q_one = { 1, 1 };
      vui128_t p_tmp;
      // Precondition the significands before multiply so that the
      // high-order 114-bits (C,L,FRACTION) of the product are right
      // adjusted in p_sig_h. And the low-order 112-bits are left
      // justified in p_sig_l.
      a_sig = vec_slqi (a_sig, 8);
      b_sig = vec_slqi (b_sig, 8);
      p_sig_l = vec_muludq (&p_sig_h, a_sig, b_sig);
      print_vint128x (" sig (p_h):", (vui128_t) p_sig_h);
      print_vint128x (" sig (p_l):", (vui128_t) p_sig_l);
      if (vec_cmpud_any_eq (x_exp, exp_dnrm))
	{ // Involves zeros or denormals
	  print_vint128x (" exps(x_exp):", (vui128_t) x_exp);
	  // check for zero significands in multiply
	  if (vec_cmpuq_all_eq (a_sig, (vui128_t) q_zero)
	      || vec_cmpuq_all_eq (b_sig, (vui128_t) q_zero))
	    { // Multiply by zero, return QP signed zero
	      print_vint128x (" zero (q_sign):", (vui128_t) q_sign);
	      result = vec_xfer_vui32t_2_bin128 (q_sign);
	      return result;
	    }
	  else
	    {
	      vb64_t exp_mask;
	      exp_mask = vec_cmpequd (x_exp, exp_dnrm);
	      x_exp = (vui64_t) vec_sel (x_exp, (vui64_t) exp_min, exp_mask);
	      print_vint128x (" adj (x_exp):", (vui128_t) x_exp);

	      a_exp = vec_splatd (x_exp, VEC_DW_H);
	      b_exp = vec_splatd (x_exp, VEC_DW_L);
	    }
	}
      else
	{
	  a_exp = vec_splatd (a_exp, VEC_DW_H);
	  b_exp = vec_splatd (b_exp, VEC_DW_H);
#if 1
	  // Check for carry and adjust
	  if (vec_cmpuq_all_gt (p_sig_h, (vui128_t) sigov))
	    {
	      print_vint128x (" OV  (p_sig):", (vui128_t) p_sig_h);
	      p_tmp = vec_sldqi (p_sig_h, p_sig_l, 120);
	      p_sig_h = vec_srqi (p_sig_h, 1);
	      p_sig_l = vec_slqi (p_tmp, 7);
	      print_vint128x (" sig (p_h):", (vui128_t) p_sig_h);
	      print_vint128x (" sig (p_l):", (vui128_t) p_sig_l);
	      a_exp = vec_addudm (a_exp, q_one);
	      print_vint128x (" adj (a_exp):", (vui128_t) a_exp);
	    }
#endif
	}
      // sum exponents
      q_exp = vec_addudm (a_exp, b_exp);
      print_vint128x (" exp (a+b):", (vui128_t) q_exp);
      q_exp = vec_subudm (q_exp, exp_bias);
      print_vint128x (" exp (q_exp):", (vui128_t) q_exp);
#if 0
      // Check for carry and adjust
      if (vec_cmpuq_all_gt (p_sig_h, (vui128_t) sigov))
      	{
      	  print_vint128x (" OV  (p_sig):", (vui128_t) p_sig_h);
      	  p_tmp = vec_sldqi (p_sig_h, p_sig_l, 120);
      	  p_sig_h = vec_srqi (p_sig_h, 1);
      	  p_sig_l = vec_slqi (p_tmp, 7);
      	  print_vint128x (" sig (p_h):", (vui128_t) p_sig_h);
      	  print_vint128x (" sig (p_l):", (vui128_t) p_sig_l);
      	  q_exp = vec_addudm (q_exp, q_one);
      	  print_vint128x (" adj (q_exp):", (vui128_t) q_exp);
      	}
      else if (vec_cmpuq_all_le (p_sig_h, (vui128_t) sigovt))
      	{
      	  print_vint128x (" OVT (p_sig):", (vui128_t) p_sig_h);
      	}
#endif

      // There are two cases for denormal
      // 1) The sum of unbiased exponents is less than E_min (tiny).
      // 2) The significand is less than 1.0 (C and L-bits are zero).
      //  2a) The exponent is greater than E_min
      //  2b) The exponent is equal to E_min
      //
      if (vec_cmpsd_all_lt ((vi64_t) q_exp, exp_min))
	{
	  print_vint128x (" tiny (q_exp):", (vui128_t) q_exp);
	  {
	    const vui64_t exp_tinyer = (vui64_t) CONST_VINT64_DW( 116, 116 );
	    const vui32_t xmask = CONST_VINT128_W(0x1fffffff, -1, -1, -1);
	    vui32_t tmp;
	    // Intermediate result is tiny, unbiased exponent < -16382
	    x_exp = vec_subudm ((vui64_t) exp_min, q_exp);

	    print_vint128x (" tiny (x_exp):", (vui128_t) x_exp);

	    if  (vec_cmpud_all_gt ((vui64_t) x_exp, exp_tinyer))
	      {
		// Intermediate result is too tiny, the shift will
		// zero the fraction and the GR-bit leaving only the
		// Sticky bit. The X-bit needs to include all bits
		// from p_sig_h and p_sig_l
		p_sig_l = vec_srqi (p_sig_l, 16);
		p_sig_l = (vui128_t) vec_or ((vui32_t) p_sig_l, (vui32_t) p_sig_h);
		print_vint128x (" sig (h|l):", (vui128_t) p_sig_l);
		// generate a carry into bit-2 for any nonzero bits 3-127
		p_sig_l = vec_adduqm (p_sig_l, (vui128_t) xmask);
		q_sig = (vui128_t) q_zero;
		p_sig_l = (vui128_t) vec_andc ((vui32_t) p_sig_l, xmask);
		print_vint128x (" sig (pxl):", (vui128_t) p_sig_l);
	      }
	    else
	      { // Normal tiny, right shift may lose low order bits
		// from p_sig_l. So collect any 1-bits below GRX and
		// OR them into the X-bit, before the right shift.
		vui64_t l_exp;
		const vui64_t exp_128 = (vui64_t) CONST_VINT64_DW( 128, 128 );
		// Propagate low order bits into the sticky bit
		// GRX left adjusted in p_sig_l
		// Isolate bits below GDX (bits 3-128).
		tmp = vec_and ((vui32_t) p_sig_l, xmask);
		// generate a carry into bit-2 for any nonzero bits 3-127
		tmp = (vui32_t) vec_adduqm ((vui128_t) tmp, (vui128_t) xmask);
		// Or this with the X-bit to propagate any sticky bits into X
		p_sig_l = (vui128_t) vec_or ((vui32_t) p_sig_l, tmp);
		p_sig_l = (vui128_t) vec_andc ((vui32_t) p_sig_l, xmask);
		print_vint128x (" sig (pxl):", (vui128_t) p_sig_l);

		l_exp = vec_subudm (exp_128, x_exp);

		print_vint128x (" tiny (l_exp):", (vui128_t) l_exp);
		p_sig_l = vec_sldq (p_sig_h, p_sig_l, (vui128_t) l_exp);
		p_sig_h = vec_srq (p_sig_h, (vui128_t) x_exp);
		print_vint128x (" srq (p_h):", (vui128_t) p_sig_h);
		print_vint128x (" srq (p_l):", (vui128_t) p_sig_l);
		q_sig = p_sig_h;
	      }
	    q_exp = q_zero;
	  }
	}
      else
	{
	  // Exponent is not tiny.
	  if (vec_cmpuq_all_le (p_sig_h, (vui128_t) sigovt))
	    {
	      // But the significand is below normal range.
	      // This can happen when multiplying a denormal by a
	      // normal.
	      const vui64_t exp_15 = { 15, 15 };
	      vui64_t c_exp, d_exp;
	      vui128_t c_sig;
	      print_vint128x (" UFt (p_sig):", (vui128_t) p_sig_h);
	      c_sig = vec_clzq (p_sig_h);
	      c_exp = vec_splatd ((vui64_t) c_sig, VEC_DW_L);
	      c_exp = vec_subudm (c_exp, exp_15);
	      print_vint128x (" clz (p_sig):", (vui128_t) c_exp);
	      d_exp = vec_subudm (q_exp, (vui64_t) exp_min);
	      print_vint128x (" exp (exp-1):", (vui128_t) d_exp);
	      d_exp = vec_minud (c_exp, d_exp);
	      print_vint128x (" exp (min-d):", (vui128_t) d_exp);

	      if (vec_cmpsd_all_gt ((vi64_t) q_exp, exp_min))
		{
		  p_sig_h = vec_sldq (p_sig_h, p_sig_l, (vui128_t) d_exp);
		  p_sig_l = vec_slq (p_sig_l, (vui128_t) d_exp);
		  print_vint128x (" sig (p_h):", (vui128_t) p_sig_h);
		  print_vint128x (" sig (p_l):", (vui128_t) p_sig_l);
		  if (vec_cmpud_all_le (q_exp, c_exp))
		    {
		      // Intermediate result == tiny, unbiased exponent == -16382
		      // Check if sig is denormal range (L-bit is 0).
		      q_exp = q_zero;
		      print_vint128x (" exp (q<=c):", (vui128_t) q_exp);
		    }
		  else
		    q_exp = vec_subudm (q_exp, d_exp);

		  print_vint128x (" adj (q_exp):", (vui128_t) q_exp);
		}
	      else
		{
		  // Intermediate result == tiny, unbiased exponent == -16382
		  // sig is denormal range (L-bit is 0).
		  print_vint128x (" UFt (p_sig):", (vui128_t) p_sig_h);
		  q_exp = q_zero;
		  print_vint128x (" adj (q_exp):", (vui128_t) q_exp);
		}
	    }
	  q_sig = p_sig_h;
	}

      // Round to odd from lower product bits
      p_odd = vec_addcuq (p_sig_l, (vui128_t) q_ones);
      q_sig = (vui128_t)  vec_or ((vui32_t) q_sig, (vui32_t) p_odd);

      // Check for exponent overflow -> __FLT128_INF__
      if  (vec_cmpud_all_gt ( q_exp, q_expmax))
      {
	// Intermediate result is huge, unbiased exponent > 16383
	print_vint128x (" OV  (q_exp):", (vui128_t) q_exp);
	q_exp = q_expmax;
	q_sig = (vui128_t) sigov;
      }
    }
  else
    { // One or both operands are NaN or Infinity
      if (vec_cmpuq_all_eq (a_sig, (vui128_t) q_zero)
	  && vec_cmpuq_all_eq (b_sig, (vui128_t) q_zero))
	{
	  // Both operands either infinity or zero
	  if (vec_cmpud_any_eq (x_exp, q_zero))
	    {
	      // Inifinty x Zero is Default Quiet NaN
	      return vec_const_nanf128 ();
	    }
	  else
	    {
	      // Infinity x Infinity == signed Infinity
	      q_sign = vec_xor (a_sign, b_sign);
	      q_exp = a_exp;
	      q_sig = a_sig;
	    }
	}
      else
	{
	  // One or both operands are NaN
	  const vui32_t q_nan = CONST_VINT128_W(0x00008000, 0, 0, 0);
	  if (vec_all_isnanf128 (vfa))
	    {
	      // vfa is NaN
	      q_sign = a_sign;
	      q_sig = (vui128_t) vec_or ((vui32_t) a_sig, q_nan);
	      q_exp = a_exp;
	    }
	  else if (vec_all_isnanf128 (vfb))
	    {
	      // vfb is NaN
	      q_sign = b_sign;
	      q_sig = (vui128_t) vec_or ((vui32_t) b_sig, q_nan);
	      q_exp = b_exp;
	    }
	  else  // OR an Infinity and a Nonzero finite number
	    {
	      q_sign = vec_xor (a_sign, b_sign);
	      q_exp = q_expnaninf;
	      q_sig = (vui128_t) q_zero;
	    }
	}
    }
  // Merge sign, significand, and exponent into final result
  q_sig = (vui128_t) vec_or ((vui32_t) q_sig, q_sign);
  result = vec_xsiexpqp (q_sig, q_exp);
#else // ! _ARCH_PWR8, use libgcc soft-float
  result = vfa * vfb;
#endif
  return result;
}

__binary128
db_vec_xsaddqpo (__binary128 vfa, __binary128 vfb)
{
  __binary128 result;
#if defined (_ARCH_PWR9) && (__GNUC__ > 7)
#if defined (__FLOAT128__) && (__GNUC__ > 8)
  // earlier GCC versions generate extra data moves for this.
  result = __builtin_addf128_round_to_odd (vfa, vfb);
#else
  // No extra data moves here.
  __asm__(
      "xsaddqpo %0,%1,%2"
      : "=v" (result)
      : "v" (vfa), "v" (vfb)
      : );
#endif
#elif  defined (_ARCH_PWR8)
  vui64_t q_exp, a_exp, b_exp, x_exp;
  vui128_t q_sig, a_sig, b_sig, p_tmp, p_odd;
  vui128_t a_mag, b_mag;
  vui128_t s_sig, x_bits;
  vui32_t q_sign,  a_sign,  b_sign;
  vb128_t a_lt_b;
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  const vui64_t q_zero = { 0, 0 };
  const vui64_t q_ones = { -1, -1 };
  const vi64_t exp_min = (vi64_t) CONST_VINT64_DW( 1, 1 );
  const vui64_t exp_dnrm = (vui64_t) CONST_VINT64_DW( 0, 0 );
  const vui64_t q_expnaninf = (vui64_t) CONST_VINT64_DW( 0x7fff, 0x7fff );
  const vui64_t q_expmax = (vui64_t) CONST_VINT64_DW( 0x7ffe, 0x7ffe );
  const vui32_t sigov = CONST_VINT128_W(0x000fffff, -1, -1, -1);
  const vui32_t sigovt = CONST_VINT128_W(0x0007ffff, -1, -1, -1);
  const vui32_t xbitmask = CONST_VINT128_W(0, 0, 0, 1);

  print_vfloat128x("db_vec_xsaddqpo vfa=  ", vfa);
  print_vfloat128x("                vfb=  ", vfb);

#if 0
  a_exp = (vui64_t) vec_sld ((vui32_t) q_zero, (vui32_t) a_mag, 2);
  a_exp = vec_splatd (a_exp, VEC_DW_L);
#else
  a_exp = vec_xsxexpqp (vfa);
#endif
  a_sig = vec_xsxsigqp (vfa);
  a_sign = vec_and_bin128_2_vui32t (vfa, signmask);
  print_vint128x (" sign(vfa):", (vui128_t) a_sign);
  print_vint128x (" exp (vfa):", (vui128_t) a_exp);
  print_vint128x (" sig (vfa):", (vui128_t) a_sig);
#if 0
  b_exp = (vui64_t) vec_sld ((vui32_t) q_zero, (vui32_t) b_mag, 2);
  b_exp = vec_splatd (b_exp, VEC_DW_L);
#else
  b_exp = vec_xsxexpqp (vfb);
#endif
  b_sig = vec_xsxsigqp (vfb);
  b_sign = vec_and_bin128_2_vui32t (vfb, signmask);
  print_vint128x (" sign(vfb):", (vui128_t) b_sign);
  print_vint128x (" exp (vfb):", (vui128_t) b_exp);
  print_vint128x (" sig (vfb):", (vui128_t) b_sig);
  x_exp = vec_mrgahd ((vui128_t) a_exp, (vui128_t) b_exp);

//  if (vec_all_isfinitef128 (vfa) && vec_all_isfinitef128 (vfb))
//  The above can be optimized to the following
  if (__builtin_expect (vec_cmpud_all_lt (x_exp, q_expnaninf), 1))
    {
      const vui64_t q_one = { 1, 1 };
      vui128_t add_sig, sub_sig;
      vui32_t diff_sign;

      q_sign = vec_xor (a_sign, b_sign);
#if 1
      diff_sign = (vui32_t) vec_setb_sq ((vi128_t) q_sign);
#else
      diff_sign = (vui32_t) vec_cmpneuq ((vui128_t) a_sign, (vui128_t) b_sign);
#endif

      a_mag = (vui128_t) vec_andc_bin128_2_vui32t (vfa, signmask);
      b_mag = (vui128_t) vec_andc_bin128_2_vui32t (vfb, signmask);

      // Precondition the significands before add so the GRX bits
      // are in the least significant 3 bit.
      a_sig = vec_slqi (a_sig, 3);
      b_sig = vec_slqi (b_sig, 3);
      print_vint128x (" sig (a  ):", (vui128_t) a_sig);
      print_vint128x (" sig (b  ):", (vui128_t) b_sig);

      // If magnitude(b) >  magnitude(a) will need to swap a/b, later
      a_lt_b = vec_cmpltuq (a_mag, b_mag);

//      if (vec_cmpud_any_eq (x_exp, exp_dnrm))
	{ // Involves zeros or denormals
	  print_vint128x (" exps(x_exp):", (vui128_t) x_exp);
#if 0
	  // check for zero significands in add
	  if (vec_cmpuq_all_eq (a_sig, (vui128_t) q_zero))
	    {
	      if (vec_cmpuq_all_eq (b_sig, (vui128_t) q_zero))
		{ // Add to zero, return vfa
		  print_vint128x (" zero+zero):", (vui128_t) a_sign);
		  print_vint128x ("          ):", (vui128_t) b_sign);

		  // Return Exact-zero-difference result.
		  q_sign = vec_sel (b_sign, (vui32_t) q_zero, diff_sign);
		  return vec_xfer_vui32t_2_bin128 (q_sign);
		}
	      else
		{
		  // Add to zero, return vfb
		  print_vint128x (" zero (+ b):", (vui128_t) b_sig);
		  return vfb;
		}
	    }
	  else if (vec_cmpuq_all_eq (b_sig, (vui128_t) q_zero))
	    { // Add to zero, return vfa
	      print_vint128x (" zero (+ a):", (vui128_t) a_sig);
	      return vfa;
	    }
	  else
#endif
	    {
	      vb64_t exp_mask;
	      exp_mask = vec_cmpequd (x_exp, exp_dnrm);
	      x_exp = (vui64_t) vec_sel (x_exp, (vui64_t) exp_min, exp_mask);
	      print_vint128x (" adj (x_exp):", (vui128_t) x_exp);
	    }
	}

      // Now swap a/b is necessary so a has greater magnitude.
	{
	  vui128_t a_tmp = a_sig;
	  vui128_t b_tmp = b_sig;
	  vui64_t  x_tmp = vec_swapd (x_exp);

	  q_sign = vec_sel (a_sign, b_sign, (vui32_t) a_lt_b);

	  x_exp  = vec_sel (x_exp, x_tmp, (vui64_t) a_lt_b);
	  a_exp = vec_splatd (x_exp, VEC_DW_H);
	  b_exp = vec_splatd (x_exp, VEC_DW_L);
	  q_exp = a_exp;

	  a_sig = vec_seluq (a_tmp, b_tmp, (vb128_t) a_lt_b);
	  b_sig = vec_seluq (b_tmp, a_tmp, (vb128_t) a_lt_b);
	  print_vint128x (" sign(q  ):", (vui128_t) q_sign);
	  print_vint128x (" exp (q  ):", (vui128_t) q_exp);
	  print_vint128x (" exps(a  ):", (vui128_t) a_exp);
	  print_vint128x (" exps(b  ):", (vui128_t) b_exp);
	  print_vint128x (" sigs(a  ):", (vui128_t) a_sig);
	  print_vint128x (" sigs(b  ):", (vui128_t) b_sig);
	}
      // At this point we can assume that:
      // The magnitude (vfa) >= magnitude (vfb)
      // 1) Exponents (a_exp, b_exp) in the range E_min -> E_max
      // 2) a_exp >= b_exp
      // 2a) If a_exp == b_exp then a_sig >= b_sig
      // 2b) If a_exp > b_exp then
      //     shift (b_sig) right by (a_exp-b_exp)

      x_bits = (vui128_t) q_zero;
      // If (b_exp < a_exp) then
      // Shift right b_sig by (a_exp - b_exp)
      // Collect any shift out of b_sig and or them into the X-bit
      if (vec_cmpud_all_lt (b_exp, a_exp))
	{
	  vui64_t d_exp, l_exp;
	  vui128_t t_sig;
	  const vui64_t exp_128 = (vui64_t) CONST_VINT64_DW( 128, 128 );

	  d_exp = vec_subudm (a_exp, b_exp);
	  print_vint128x (" delta (a_exp):", (vui128_t) d_exp);
	  if (vec_cmpud_all_lt (d_exp, exp_128))
	    {
	      l_exp = vec_subudm (exp_128, d_exp);
	      t_sig = vec_srq (b_sig, (vui128_t) d_exp);
	      x_bits = vec_slq (b_sig, (vui128_t) l_exp);
	    }
	  else
	    {
	      x_bits = b_sig;
	      t_sig = (vui128_t) q_zero;
	    }
	  print_vint128x (" srq (b_sig):", (vui128_t) t_sig);
	  print_vint128x (" slq (x_sig):", (vui128_t) x_bits);

	  p_odd = vec_addcuq (x_bits, (vui128_t) q_ones);
	  b_sig = (vui128_t) vec_or ((vui32_t) t_sig, (vui32_t) p_odd);
	  print_vint128x (" sig (p_odd):", (vui128_t) p_odd);
	  print_vint128x (" sig (b_sig):", (vui128_t) b_sig);
	//  b_sig = t_sig;
	}

      // Add/subtract significands
      add_sig = vec_adduqm (a_sig, b_sig);
      sub_sig = vec_subuqm (a_sig, b_sig);
      s_sig = vec_seluq (add_sig, sub_sig, (vb128_t) diff_sign);
      print_vint128x (" sig (s  ):", (vui128_t) s_sig);

      print_vint128x (" sum (sig):", (vui128_t) s_sig);
      if (vec_cmpuq_all_eq (s_sig, (vui128_t) q_zero))
	{ // Return Exact-zero-difference result.
	  print_vint128x (" zero (q_sign):", (vui128_t) q_sign);
#if 1
#if 1
#if 1
	  // Return Exact-zero-difference result.
	  q_sign = vec_sel (b_sign, (vui32_t) q_zero, diff_sign);
	  return vec_xfer_vui32t_2_bin128 (q_sign);
#else
	  if (vec_cmpuq_all_eq (a_sig, (vui128_t) q_zero)
	      && vec_cmpuq_all_eq (b_sig, (vui128_t) q_zero))
	    { // Add to zero, return vfa
		  print_vint128x (" zero+zero):", (vui128_t) a_sign);
		  print_vint128x ("          ):", (vui128_t) b_sign);

		  // Return Exact-zero-difference result.
		  q_sign = vec_sel (b_sign, (vui32_t) q_zero, diff_sign);
		  return vec_xfer_vui32t_2_bin128 (q_sign);
	    }
	  else
	    {
		  return vec_xfer_vui64t_2_bin128 (q_zero);
	    }
#endif
#else
	  // check for zero significands in add
	  if (vec_cmpuq_all_eq (a_sig, (vui128_t) q_zero))
	    {
	      if (vec_cmpuq_all_eq (b_sig, (vui128_t) q_zero))
		{ // Add to zero, return vfa
		  print_vint128x (" zero+zero):", (vui128_t) a_sign);
		  print_vint128x ("          ):", (vui128_t) b_sign);

		  // Return Exact-zero-difference result.
		  q_sign = vec_sel (b_sign, (vui32_t) q_zero, diff_sign);
		  return vec_xfer_vui32t_2_bin128 (q_sign);
		}
	      else
		{
		  // Add to zero, return vfb
		  print_vint128x (" zero (+ b):", (vui128_t) b_sig);
		  return vfb;
		}
	    }
	  else if (vec_cmpuq_all_eq (b_sig, (vui128_t) q_zero))
	    { // Add to zero, return vfa
	      print_vint128x (" zero (+ a):", (vui128_t) a_sig);
	      return vfa;
	    }
	  else
		  return vec_xfer_vui64t_2_bin128 (q_zero);
#endif
#else
	  return vec_xfer_vui64t_2_bin128 (q_zero);
#endif
	}
      else if (vec_cmpuq_all_gt (s_sig, (vui128_t) sigov))
	{ // Check for carry and adjust
	  print_vint128x (" OV  (s_sig):", (vui128_t) s_sig);
	  p_odd = (vui128_t) vec_and ((vui32_t) s_sig, xbitmask);
	  s_sig = vec_srqi (s_sig, 1);
	  s_sig = (vui128_t) vec_or ((vui32_t) s_sig, (vui32_t) p_odd);
	  //x_bits = (vui128_t) vec_or ((vui32_t) x_bits, (vui32_t) p_odd);
	  print_vint128x (" sig (s_xb ):", (vui128_t) s_sig);
	  q_exp = vec_addudm (q_exp, q_one);
	  print_vint128x (" adj (q_exp):", (vui128_t) q_exp);
	}
      else if (vec_cmpuq_all_le (s_sig, (vui128_t) sigovt))
	{
	  // Or the significand is below normal range.
	  // This can happen with subtraction.
	  const vui64_t exp_15 = { 12, 12 };
	  vui64_t c_exp, d_exp;
	  vui128_t c_sig;

	  print_vint128x (" UFt (s_sig):", (vui128_t) s_sig);
	  c_sig = vec_clzq (s_sig);
	  c_exp = vec_splatd ((vui64_t) c_sig, VEC_DW_L);
	  c_exp = vec_subudm (c_exp, exp_15);
	  print_vint128x (" clz (s_sig):", (vui128_t) c_exp);
	  d_exp = vec_subudm (q_exp, (vui64_t) exp_min);
	  print_vint128x (" exp (exp-1):", (vui128_t) d_exp);
	  d_exp = vec_minud (c_exp, d_exp);
	  print_vint128x (" exp (min-d):", (vui128_t) d_exp);

	  if (vec_cmpsd_all_gt ((vi64_t) q_exp, exp_min))
	    {
#if 1
	      vb64_t exp_mask;
	      s_sig = vec_slq (s_sig, (vui128_t) d_exp);
	      print_vint128x (" sig (s_h):", (vui128_t) s_sig);
	      exp_mask = vec_cmpgtud (q_exp, c_exp);
	      print_vint128x (" exp (Emsk)):", (vui128_t) exp_mask);
	      q_exp = vec_subudm (q_exp, d_exp);
	      print_vint128x (" exp (q-d )):", (vui128_t) q_exp);
	      q_exp = (vui64_t) vec_sel (q_zero, (vui64_t) q_exp, exp_mask);
	      print_vint128x (" exp (Emin)):", (vui128_t) q_exp);
#else
	      s_sig = vec_slq (s_sig, (vui128_t) d_exp);
	      print_vint128x (" sig (s_h):", (vui128_t) s_sig);
	      if (vec_cmpud_all_le (q_exp, c_exp))
		{
		  // Intermediate result == tiny, unbiased exponent == -16382
		  // Check if sig is denormal range (L-bit is 0).
		  q_exp = q_zero;
		  print_vint128x (" exp (q<=c):", (vui128_t) q_exp);
		}
	      else
		q_exp = vec_subudm (q_exp, d_exp);
#endif

	      print_vint128x (" adj (q_exp):", (vui128_t) q_exp);
	    }
	  else
	    {
	      // Intermediate result == tiny, unbiased exponent == -16382
	      // sig is denormal range (L-bit is 0).
	      print_vint128x (" UFt (s_sig):", (vui128_t) s_sig);
	      q_exp = q_zero;
	      print_vint128x (" adj (q_exp):", (vui128_t) q_exp);
	    }
	}
#if 0
      // Accumulate x_bits
      p_odd = vec_addcuq (x_bits, (vui128_t) q_ones);
      s_sig = (vui128_t) vec_or ((vui32_t) s_sig, (vui32_t) p_odd);
#endif
      // Round to odd from lower product bits
      p_tmp = vec_slqi (s_sig, 125);
      p_odd = vec_addcuq (p_tmp, (vui128_t) q_ones);
      q_sig = vec_srqi (s_sig, 3);
      q_sig = (vui128_t) vec_or ((vui32_t) q_sig, (vui32_t) p_odd);

      // Check for exponent overflow -> __FLT128_INF__
      if (vec_cmpud_all_gt (q_exp, q_expmax))
	{
	  // Intermediate result is huge, unbiased exponent > 16383
	  print_vint128x (" OV  (q_exp):", (vui128_t) q_exp);
	  q_exp = q_expmax;
	  q_sig = (vui128_t) sigov;
	}
    }
  else
    { // One or both operands are NaN or Infinity
      // One or both operands are NaN
      const vui32_t q_nan = CONST_VINT128_W(0x00008000, 0, 0, 0);
      if (vec_all_isnanf128 (vfa))
	{
	  // vfa is NaN
	  q_sign = a_sign;
	  q_sig = (vui128_t) vec_or ((vui32_t) a_sig, q_nan);
	  q_exp = a_exp;
	}
      else if (vec_all_isnanf128 (vfb))
	{
	  // vfb is NaN
	  q_sign = b_sign;
	  q_sig = (vui128_t) vec_or ((vui32_t) b_sig, q_nan);
	  q_exp = b_exp;
	}
      else  // One or both operands are infinity
	{
//	  a_exp = vec_splatd (a_exp, VEC_DW_H);
//	  b_exp = vec_splatd (b_exp, VEC_DW_H);
	  if (vec_cmpud_all_eq (x_exp, q_expnaninf)
	      && vec_cmpud_any_ne ((vui64_t) a_sign, (vui64_t) b_sign))
	    { // Both operands are infinity and opposite sign
	      print_vint128x (" Inf diff sign:", (vui128_t) b_sign);
	      // Inifinty + Infinity (opposite sign) is Default Quiet NaN
	      return vec_const_nanf128 ();
	    }
	  else
	    { // Either both operands infinity and same sign
	      // Or one infinity and one finite
	      if (vec_cmpud_any_eq (a_exp, q_expnaninf))
		{
		  print_vint128x (" Inf (vra):", (vui128_t) a_sign);
		  // return infinity
		  return vfa;
		}
	      else
		{
		  print_vint128x (" Inf (vrb):", (vui128_t) b_sign);
		  // return infinity
		  return vfb;
		}
	    }
	}
    }
  // Merge sign, significand, and exponent into final result
  q_sig = (vui128_t) vec_or ((vui32_t) q_sig, q_sign);
  result = vec_xsiexpqp (q_sig, q_exp);
#else // ! _ARCH_PWR8, use libgcc soft-float
  result = vfa + vfb;
#endif
  return result;
}

__binary128
db_vec_xssubqpo (__binary128 vfa, __binary128 vfb)
{
  __binary128 result;
#if defined (_ARCH_PWR9) && (__GNUC__ > 7)
#if defined (__FLOAT128__) && (__GNUC__ > 8)
  // earlier GCC versions generate extra data moves for this.
  result = __builtin_subf128_round_to_odd (vfa, vfb);
#else
  // No extra data moves here.
  __asm__(
      "xssubqpo %0,%1,%2"
      : "=v" (result)
      : "v" (vfa), "v" (vfb)
      : );
#endif
#else  // defined (_ARCH_PWR8)
#if 1
  vui64_t q_exp, a_exp, b_exp, x_exp;
  vui128_t q_sig, a_sig, b_sig, p_tmp, p_odd;
  vui128_t a_mag, b_mag;
  vui128_t s_sig, x_bits;
  vui32_t q_sign,  a_sign,  b_sign;
  vb128_t a_lt_b;
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  const vui64_t q_zero = { 0, 0 };
  const vui64_t q_ones = { -1, -1 };
  const vi64_t exp_min = (vi64_t) CONST_VINT64_DW( 1, 1 );
  const vui64_t exp_dnrm = (vui64_t) CONST_VINT64_DW( 0, 0 );
  const vui64_t q_expnaninf = (vui64_t) CONST_VINT64_DW( 0x7fff, 0x7fff );
  const vui64_t q_expmax = (vui64_t) CONST_VINT64_DW( 0x7ffe, 0x7ffe );
  const vui32_t sigov = CONST_VINT128_W(0x000fffff, -1, -1, -1);
  const vui32_t sigovt = CONST_VINT128_W(0x0007ffff, -1, -1, -1);
  const vui32_t xbitmask = CONST_VINT128_W(0, 0, 0, 1);

  print_vfloat128x("db_vec_xssubqpo vfa=  ", vfa);
  print_vfloat128x("                vfb=  ", vfb);

  a_exp = vec_xsxexpqp (vfa);
  a_sig = vec_xsxsigqp (vfa);
  a_sign = vec_and_bin128_2_vui32t (vfa, signmask);
  print_vint128x (" sign(vfa):", (vui128_t) a_sign);
  print_vint128x (" exp (vfa):", (vui128_t) a_exp);
  print_vint128x (" sig (vfa):", (vui128_t) a_sig);
  b_exp = vec_xsxexpqp (vfb);
  b_sig = vec_xsxsigqp (vfb);
  b_sign = vec_and_bin128_2_vui32t (vfb, signmask);
  print_vint128x (" sign(vfb):", (vui128_t) b_sign);
  print_vint128x (" exp (vfb):", (vui128_t) b_exp);
  print_vint128x (" sig (vfb):", (vui128_t) b_sig);
  x_exp = vec_mrgahd ((vui128_t) a_exp, (vui128_t) b_exp);

//  if (vec_all_isfinitef128 (vfa) && vec_all_isfinitef128 (vfb))
//  The above can be optimized to the following
  if (__builtin_expect (vec_cmpud_all_lt (x_exp, q_expnaninf), 1))
    {
      const vui64_t q_one = { 1, 1 };
      vui128_t add_sig, sub_sig;
      vui32_t diff_sign;
      // Negate sign for subtract, then use add logic
      b_sign = vec_xor (signmask, b_sign);
      q_sign = vec_xor (a_sign, b_sign);
      diff_sign = (vui32_t) vec_setb_sq ((vi128_t) q_sign);
      print_vint128x (" sign(b  ):", (vui128_t) b_sign);
      print_vint128x (" sign(msk):", (vui128_t) diff_sign);

      a_mag = (vui128_t) vec_andc_bin128_2_vui32t (vfa, signmask);
      b_mag = (vui128_t) vec_andc_bin128_2_vui32t (vfb, signmask);

      // Precondition the significands before add so the GRX bits
      // are in the least significant 3 bit.
      a_sig = vec_slqi (a_sig, 3);
      b_sig = vec_slqi (b_sig, 3);
      print_vint128x (" sig (a  ):", (vui128_t) a_sig);
      print_vint128x (" sig (b  ):", (vui128_t) b_sig);

      // If magnitude(b) >  magnitude(a) will need to swap a/b, later
      a_lt_b = vec_cmpltuq (a_mag, b_mag);

//      if (vec_cmpud_any_eq (x_exp, exp_dnrm))
	{ // Involves zeros or denormals
	  print_vint128x (" exps(x_exp):", (vui128_t) x_exp);
	    {
	      vb64_t exp_mask;
	      exp_mask = vec_cmpequd (x_exp, exp_dnrm);
	      x_exp = (vui64_t) vec_sel (x_exp, (vui64_t) exp_min, exp_mask);
	      print_vint128x (" adj (x_exp):", (vui128_t) x_exp);
	    }
	}

      // Now swap a/b is necessary so a has greater magnitude.
	{
	  vui128_t a_tmp = a_sig;
	  vui128_t b_tmp = b_sig;
	  vui64_t  x_tmp = vec_swapd (x_exp);

	  q_sign = vec_sel (a_sign, b_sign, (vui32_t) a_lt_b);

	  x_exp  = vec_sel (x_exp, x_tmp, (vui64_t) a_lt_b);
	  a_exp = vec_splatd (x_exp, VEC_DW_H);
	  b_exp = vec_splatd (x_exp, VEC_DW_L);
	  q_exp = a_exp;

	  a_sig = vec_seluq (a_tmp, b_tmp, (vb128_t) a_lt_b);
	  b_sig = vec_seluq (b_tmp, a_tmp, (vb128_t) a_lt_b);
	  print_vint128x (" sign(q  ):", (vui128_t) q_sign);
	  print_vint128x (" exp (q  ):", (vui128_t) q_exp);
	  print_vint128x (" exps(a  ):", (vui128_t) a_exp);
	  print_vint128x (" exps(b  ):", (vui128_t) b_exp);
	  print_vint128x (" sigs(a  ):", (vui128_t) a_sig);
	  print_vint128x (" sigs(b  ):", (vui128_t) b_sig);
	}
      // At this point we can assume that:
      // The magnitude (vfa) >= magnitude (vfb)
      // 1) Exponents (a_exp, b_exp) in the range E_min -> E_max
      // 2) a_exp >= b_exp
      // 2a) If a_exp == b_exp then a_sig >= b_sig
      // 2b) If a_exp > b_exp then
      //     shift (b_sig) right by (a_exp-b_exp)

      x_bits = (vui128_t) q_zero;
      // If (b_exp < a_exp) then
      // Shift right b_sig by (a_exp - b_exp)
      // Collect any shift out of b_sig and or them into the X-bit
      if (vec_cmpud_all_lt (b_exp, a_exp))
	{
	  vui64_t d_exp, l_exp;
	  vui128_t t_sig;
	  const vui64_t exp_128 = (vui64_t) CONST_VINT64_DW( 128, 128 );

	  d_exp = vec_subudm (a_exp, b_exp);
	  print_vint128x (" delta (a_exp):", (vui128_t) d_exp);
	  if (vec_cmpud_all_lt (d_exp, exp_128))
	    {
	      l_exp = vec_subudm (exp_128, d_exp);
	      t_sig = vec_srq (b_sig, (vui128_t) d_exp);
	      x_bits = vec_slq (b_sig, (vui128_t) l_exp);
	    }
	  else
	    {
	      x_bits = b_sig;
	      t_sig = (vui128_t) q_zero;
	    }
	  print_vint128x (" srq (b_sig):", (vui128_t) t_sig);
	  print_vint128x (" slq (x_sig):", (vui128_t) x_bits);

	  p_odd = vec_addcuq (x_bits, (vui128_t) q_ones);
	  b_sig = (vui128_t) vec_or ((vui32_t) t_sig, (vui32_t) p_odd);
	  print_vint128x (" sig (p_odd):", (vui128_t) p_odd);
	  print_vint128x (" sig (b_sig):", (vui128_t) b_sig);
	//  b_sig = t_sig;
	}

      // Add/subtract significands
      add_sig = vec_adduqm (a_sig, b_sig);
      sub_sig = vec_subuqm (a_sig, b_sig);
      s_sig = vec_seluq (add_sig, sub_sig, (vb128_t) diff_sign);
      print_vint128x (" sig (s  ):", (vui128_t) s_sig);

      print_vint128x (" sum (sig):", (vui128_t) s_sig);
      if (vec_cmpuq_all_eq (s_sig, (vui128_t) q_zero))
	{ // Return Exact-zero-difference result.
	  print_vint128x (" zero (q_sign):", (vui128_t) q_sign);
#if 1
#if 1
#if 1
	  // Return Exact-zero-difference result.
	  q_sign = vec_sel (b_sign, (vui32_t) q_zero, diff_sign);
	  return vec_xfer_vui32t_2_bin128 (q_sign);
#else
#endif
#else
#endif
#else
#endif
	}
      else if (vec_cmpuq_all_gt (s_sig, (vui128_t) sigov))
	{ // Check for carry and adjust
	  print_vint128x (" OV  (s_sig):", (vui128_t) s_sig);
	  p_odd = (vui128_t) vec_and ((vui32_t) s_sig, xbitmask);
	  s_sig = vec_srqi (s_sig, 1);
	  s_sig = (vui128_t) vec_or ((vui32_t) s_sig, (vui32_t) p_odd);
	  //x_bits = (vui128_t) vec_or ((vui32_t) x_bits, (vui32_t) p_odd);
	  print_vint128x (" sig (s_xb ):", (vui128_t) s_sig);
	  q_exp = vec_addudm (q_exp, q_one);
	  print_vint128x (" adj (q_exp):", (vui128_t) q_exp);
	}
      else if (vec_cmpuq_all_le (s_sig, (vui128_t) sigovt))
	{
	  // Or the significand is below normal range.
	  // This can happen with subtraction.
	  const vui64_t exp_15 = { 12, 12 };
	  vui64_t c_exp, d_exp;
	  vui128_t c_sig;

	  print_vint128x (" UFt (s_sig):", (vui128_t) s_sig);
	  c_sig = vec_clzq (s_sig);
	  c_exp = vec_splatd ((vui64_t) c_sig, VEC_DW_L);
	  c_exp = vec_subudm (c_exp, exp_15);
	  print_vint128x (" clz (s_sig):", (vui128_t) c_exp);
	  d_exp = vec_subudm (q_exp, (vui64_t) exp_min);
	  print_vint128x (" exp (exp-1):", (vui128_t) d_exp);
	  d_exp = vec_minud (c_exp, d_exp);
	  print_vint128x (" exp (min-d):", (vui128_t) d_exp);

	  if (vec_cmpsd_all_gt ((vi64_t) q_exp, exp_min))
	    {
	      s_sig = vec_slq (s_sig, (vui128_t) d_exp);
	      print_vint128x (" sig (s_h):", (vui128_t) s_sig);
	      if (vec_cmpud_all_le (q_exp, c_exp))
		{
		  // Intermediate result == tiny, unbiased exponent == -16382
		  // Check if sig is denormal range (L-bit is 0).
		  q_exp = q_zero;
		  print_vint128x (" exp (q<=c):", (vui128_t) q_exp);
		}
	      else
		q_exp = vec_subudm (q_exp, d_exp);

	      print_vint128x (" adj (q_exp):", (vui128_t) q_exp);
	    }
	  else
	    {
	      // Intermediate result == tiny, unbiased exponent == -16382
	      // sig is denormal range (L-bit is 0).
	      print_vint128x (" UFt (s_sig):", (vui128_t) s_sig);
	      q_exp = q_zero;
	      print_vint128x (" adj (q_exp):", (vui128_t) q_exp);
	    }
	}
#if 0
#endif
      // Round to odd from lower product bits
      p_tmp = vec_slqi (s_sig, 125);
      p_odd = vec_addcuq (p_tmp, (vui128_t) q_ones);
      q_sig = vec_srqi (s_sig, 3);
      q_sig = (vui128_t) vec_or ((vui32_t) q_sig, (vui32_t) p_odd);

      // Check for exponent overflow -> __FLT128_INF__
      if (vec_cmpud_all_gt (q_exp, q_expmax))
	{
	  // Intermediate result is huge, unbiased exponent > 16383
	  print_vint128x (" OV  (q_exp):", (vui128_t) q_exp);
	  q_exp = q_expmax;
	  q_sig = (vui128_t) sigov;
	}
    }
  else
    { // One or both operands are NaN or Infinity
      // One or both operands are NaN
      const vui32_t q_nan = CONST_VINT128_W(0x00008000, 0, 0, 0);
      if (vec_all_isnanf128 (vfa))
	{
	  // vfa is NaN
	  q_sign = a_sign;
	  q_sig = (vui128_t) vec_or ((vui32_t) a_sig, q_nan);
	  q_exp = a_exp;
	}
      else if (vec_all_isnanf128 (vfb))
	{
	  // vfb is NaN
	  q_sign = b_sign;
	  q_sig = (vui128_t) vec_or ((vui32_t) b_sig, q_nan);
	  q_exp = b_exp;
	}
      else  // One or both operands are infinity
	{
//	  a_exp = vec_splatd (a_exp, VEC_DW_H);
//	  b_exp = vec_splatd (b_exp, VEC_DW_H);

//	  b_sign = vec_xor (signmask, b_sign);
	  if (vec_cmpud_all_eq (x_exp, q_expnaninf)
	      && vec_cmpud_all_eq ((vui64_t) a_sign, (vui64_t) b_sign))
	    { // Both operands are infinity and opposite sign
	      print_vint128x (" Inf diff sign:", (vui128_t) b_sign);
	      // Inifinty + Infinity (opposite sign) is Default Quiet NaN
	      return vec_const_nanf128 ();
	    }
	  else
	    { // Either both operands infinity and same sign
	      // Or one infinity and one finite
	      if (vec_cmpud_any_eq (a_exp, q_expnaninf))
		{
		  print_vint128x (" Inf (vra):", (vui128_t) a_sign);
		  // return infinity
		  return vfa;
		}
	      else
		{
		  print_vint128x (" Inf (vrb):", (vui128_t) b_sign);
		  // return infinity
		  return vec_negf128(vfb);
		}
	    }
	}
    }
  // Merge sign, significand, and exponent into final result
  q_sig = (vui128_t) vec_or ((vui32_t) q_sig, q_sign);
  result = vec_xsiexpqp (q_sig, q_exp);
#else
  __binary128 nvfb;
  nvfb = vec_self128 (vec_negf128 (vfb), vfb, vec_isnanf128(vfb));
  result = db_vec_xsaddqpo (vfa, nvfb);
#endif
#endif
  return result;
}

__binary128
db_vec_maddqpo (__binary128 vfa, __binary128 vfb, __binary128 vfc)
{
  __binary128 result;
#if defined (_ARCH_PWR9) && (__GNUC__ > 7)
#if defined (__FLOAT128__) && (__GNUC__ > 8)
  // earlier GCC versions generate extra data moves for this.
  result = __builtin_fmaf128_round_to_odd (vfa, vfb, vfc);
#else
  // No extra data moves here.
  __asm__(
      "xsmaddqpo %0,%1,%2"
      : "+v" (vfc)
      : "v" (vfa), "v" (vfb)
      : );
  result = vfc;
#endif
  return result;
#elif  defined (_ARCH_PWR7)
  vui64_t q_exp, a_exp, b_exp, c_exp, x_exp;
  vui128_t q_sig, a_sig, b_sig, c_sig, c_sig_l, p_sig_h, p_sig_l, p_odd;
  vui32_t q_sign, a_sign, b_sign, c_sign;
  vui128_t a_mag, b_mag, c_mag;
  const vui32_t q_zero = CONST_VINT128_W (0, 0, 0, 0);
  const vui32_t q_ones = CONST_VINT128_W (-1, -1, -1, -1);
  //const vui64_t exp_naninf = (vui64_t) { 0x7fff, 0x7fff };
  const vui64_t exp_naninf = vec_mask64_f128exp();
  const vui32_t magmask = vec_mask128_f128mag();

  print_vfloat128x("db_vec_xsmaddqpo vfa=  ", vfa);
  print_vfloat128x("                 vfb=  ", vfb);
  print_vfloat128x("                 vfc=  ", vfc);

  a_exp = vec_xsxexpqp (vfa);
  b_exp = vec_xsxexpqp (vfb);

  // Vector extract the exponents from vfa, vfb
  x_exp = vec_xxxexpqpp (vfa, vfb);
  c_exp = vec_xsxexpqp (vfc);
  // Mask off sign bits so can use integers for magnitude compare.
  a_mag = (vui128_t) vec_and_bin128_2_vui32t (vfa, magmask);
  b_mag = (vui128_t) vec_and_bin128_2_vui32t (vfb, magmask);
  c_mag = (vui128_t) vec_and_bin128_2_vui32t (vfc, magmask);
  a_sign = vec_andc_bin128_2_vui32t (vfa, magmask);
  b_sign = vec_andc_bin128_2_vui32t (vfb, magmask);
  c_sign = vec_andc_bin128_2_vui32t (vfc, magmask);
  print_vint128x (" sign(vfa):", (vui128_t) a_sign);
  print_vint128x (" exp (vfa):", (vui128_t) a_exp);
  print_vint128x (" mag (vfa):", (vui128_t) a_mag);
  print_vint128x (" sign(vfb):", (vui128_t) b_sign);
  print_vint128x (" exp (vfb):", (vui128_t) b_exp);
  print_vint128x (" mag (vfb):", (vui128_t) b_mag);
  print_vint128x (" sign(vfc):", (vui128_t) c_sign);
  print_vint128x (" exp (vfc):", (vui128_t) c_exp);
  print_vint128x (" mag (vfc):", (vui128_t) c_mag);
  q_sign = vec_xor (a_sign, b_sign);
  c_exp = vec_mrgahd ((vui128_t) c_exp, (vui128_t) c_exp);

//  if (vec_all_isfinitef128 (vfa) && vec_all_isfinitef128 (vfb))
  if (__builtin_expect ((vec_cmpud_all_lt (x_exp, exp_naninf)
      && vec_cmpud_all_lt (c_exp, exp_naninf)), 1))
    {
      const vui64_t exp_dnrm = (vui64_t) q_zero;
      vui64_t exp_min, exp_one, exp_bias;
      vui32_t diff_sign;
      vui128_t p_tmp;
      vb128_t a_lt_b;
#if 1
      // check for zero significands in multiply
      if (__builtin_expect (
	    (vec_all_eq((vui32_t ) a_mag, (vui32_t ) q_zero)
	  || vec_all_eq((vui32_t ) b_mag, (vui32_t ) q_zero)),
	  0))
	{ // Multiply by zero + zero, return QP signed zero
	  if (vec_all_eq((vui32_t ) c_mag, (vui32_t ) q_zero))
		{ // Special case of both zero with different sign
		  vui32_t diff_sign = vec_xor (q_sign, c_sign);
		  diff_sign = (vui32_t) vec_setb_sq ((vi128_t) diff_sign);
		  q_sign = vec_sel (q_sign, (vui32_t) q_zero, diff_sign);
		  result = vec_xfer_vui32t_2_bin128 (q_sign);
		}
	  else
	    result = vfc;

	  return result;
	}
#endif
      // const vui64_t exp_min, exp_one = { 1, 1 };
      // exp_min = exp_one = vec_splat_u64 (1);
      { // Extract the significands and insert the Hidden bit
	//const vui32_t q_zero = CONST_VINT128_W(0, 0, 0, 0);
	const vui32_t sigmask = vec_mask128_f128sig ();
	vui32_t a_s32, b_s32, c_s32;
	vui16_t a_e16, b_e16, c_e16, x_hidden;
	vb16_t a_norm, b_norm, c_norm;

	//const vui32_t hidden = vec_mask128_f128Lbit();
	x_hidden = vec_splat_u16(1);
	// Assume that the operands are finite magnitudes
	// Mask off the significands
	// Applying sigmask to orignal inputs can save 2 cycles here
	a_s32 = vec_and_bin128_2_vui32t (vfa, sigmask);
	b_s32 = vec_and_bin128_2_vui32t (vfb, sigmask);
	c_s32 = vec_and_bin128_2_vui32t (vfc, sigmask);
	// But still need a/b_mag for exp extract to clear sign-bit
	// Mask off the exponents in high halfword
	a_e16 = (vui16_t) vec_andc ((vui32_t) a_mag, sigmask);
	b_e16 = (vui16_t) vec_andc ((vui32_t) b_mag, sigmask);
	c_e16 = (vui16_t) vec_andc ((vui32_t) c_mag, sigmask);
	// Compare exponents for finite i.e. > denomal (q_zero)
	a_norm = vec_cmpgt (a_e16, (vui16_t) q_zero);
	b_norm = vec_cmpgt (b_e16, (vui16_t) q_zero);
	c_norm = vec_cmpgt (c_e16, (vui16_t) q_zero);
	// For Normal QP insert (hidden) L-bit into significand
	a_sig = (vui128_t) vec_sel ((vui16_t) a_s32, x_hidden, a_norm);
	b_sig = (vui128_t) vec_sel ((vui16_t) b_s32, x_hidden, b_norm);
	c_sig = (vui128_t) vec_sel ((vui16_t) c_s32, x_hidden, c_norm);
      }
      print_vint128x (" sig (vfa):", (vui128_t) a_sig);
      print_vint128x (" sig (vfb):", (vui128_t) b_sig);
      print_vint128x (" sig (vfc):", (vui128_t) c_sig);
#if 0
      // check for zero significands in multiply
      if (__builtin_expect (
	    (vec_all_eq((vui32_t ) a_mag, (vui32_t ) q_zero)
	  || vec_all_eq((vui32_t ) b_mag, (vui32_t ) q_zero)),
	  0))
	{ // Multiply by zero + zero, return QP signed zero
	  if (vec_all_eq((vui32_t ) c_mag, (vui32_t ) q_zero))
		{ // Special case of both zero with different sign
		  vui32_t diff_sign = vec_xor (q_sign, c_sign);
		  diff_sign = (vui32_t) vec_setb_sq ((vi128_t) diff_sign);
		  q_sign = vec_sel (q_sign, (vui32_t) q_zero, diff_sign);
		  result = vec_xfer_vui32t_2_bin128 (q_sign);
		}
	  else
	    result = vfc;

	  return result;
	}
#endif

      // Precondition the significands before multiply so that the
      // high-order 114-bits (C,L,FRACTION) of the product are right
      // adjusted in p_sig_h. And the Low-order 112-bits are left
      // justified in p_sig_l.
      // Logically this (multiply) step could be moved after the zero
      // test. But this uses a lot of registers and the compiler may
      // see this as register pressure and decide to spill and reload
      // unrelated data around this block.
      // The zero multiply is rare so on average performance is better
      // if we get this started now.
      a_sig = vec_slqi (a_sig, 8);
      b_sig = vec_slqi (b_sig, 8);
      p_sig_l = vec_muludq (&p_sig_h, a_sig, b_sig);
#if 0
      // check for zero significands in multiply
      if (__builtin_expect (
	    (vec_all_eq((vui32_t ) a_sig, (vui32_t ) q_zero)
	  || vec_all_eq((vui32_t ) b_sig, (vui32_t ) q_zero)),
	  0))
	{ // Multiply by zero + zero, return QP signed zero
	  if (vec_all_eq((vui32_t ) c_sig, (vui32_t ) q_zero))
		{ // Special case of both zero with different sign
		  vui32_t diff_sign = vec_xor (q_sign, c_sign);
		  diff_sign = (vui32_t) vec_setb_sq ((vi128_t) diff_sign);
		  q_sign = vec_sel (q_sign, (vui32_t) q_zero, diff_sign);
		  result = vec_xfer_vui32t_2_bin128 (q_sign);
		}
	  else
	    result = vfc;

	  return result;
	}
#endif
      // const vui64_t exp_min, exp_one = { 1, 1 };
      exp_min = exp_one = vec_splat_u64 (1);
      //const vui64_t exp_bias = (vui64_t) { 0x3fff, 0x3fff };
      exp_bias = (vui64_t) vec_srhi ((vui16_t) exp_naninf, 1);
	{ // Compute product exponent q_exp
	  // Operand exponents should >= Emin for computation
	  vb64_t exp_mask;
	  exp_mask = vec_cmpequd (x_exp, exp_dnrm);
	  x_exp = vec_selud (x_exp, exp_min, (vb64_t) exp_mask);
	  print_vint128x (" exp (x_exp):", (vui128_t) x_exp);
	  // sum exponents across x_exp
	  q_exp = vec_addudm (x_exp, vec_swapd (x_exp));
	  // Sum includes 2 x exp_bias, So subtract 1 x exp_bias
	  print_vint128x (" exp (p_exp):", (vui128_t) q_exp);
	  q_exp = vec_subudm (q_exp, exp_bias);
	  print_vint128x (" exp (p_exp):", (vui128_t) q_exp);
	  exp_mask = vec_cmpequd (c_exp, exp_dnrm);
	  c_exp = vec_selud (c_exp, exp_min, (vb64_t) exp_mask);
	  print_vint128x (" exp (vfc ):", (vui128_t) c_exp);
	}
#if 1
      // Check for carry; shift right 1 and adjust exp +1
	{
	  vb128_t carry_mask;
	  vui128_t sig_h, sig_l;
	  // Test Carry-bit (greater than L-bit)
	  vui16_t sig_l_mask = vec_splat_u16(1);
	  vui16_t t_sig = vec_splat ((vui16_t) p_sig_h, VEC_HW_H);
	  carry_mask = (vb128_t) vec_cmpgt (t_sig, sig_l_mask);
	  // Shift double quadword right 1 bit
	  p_tmp = vec_sldqi (p_sig_h, p_sig_l, 120);
	  sig_h = vec_srqi (p_sig_h, 1);
	  sig_l = vec_slqi (p_tmp, 7);
	  // Increment the exponent
	  x_exp = vec_addudm (q_exp, exp_one);
	  // Select original or normalized exp/sig
	  p_sig_h = vec_seluq (p_sig_h, sig_h, carry_mask);
	  p_sig_l = vec_seluq (p_sig_l, sig_l, carry_mask);
	  q_exp = vec_selud (q_exp, x_exp, (vb64_t) carry_mask);
	}
#endif
      print_vint128x (" sign(prod):", (vui128_t) q_sign);
      print_vint128x (" exp (prod):", (vui128_t) q_exp);
      print_vint128x (" sig (prod):", (vui128_t) p_sig_h);
      print_vint128x ("     (prod):", (vui128_t) p_sig_l);
      // We have the product in q_sign, q_exp, p_sig_h/p_sig_l
      // And the addend (vfc) in c_sign, c_exp,  c_sig/c_sig_l
      c_sig_l = (vui128_t) q_zero;
      q_sig = p_sig_h;
      // Generation sign difference for signed 0.0

      // If sign(vfa) != sign(vfb) will need to:
      // 1) Subtract instead of add significands
      // 2) Generate signed zeros

      // If magnitude(b) >  magnitude(a) will need to swap a/b, later
      a_lt_b = vec_cmpltuq (q_sig, c_sig);
      diff_sign = (vui32_t) vec_cmpneuq ((vui128_t) q_sign, (vui128_t) c_sign);
      print_vint128x (" sign(diff):", (vui128_t) diff_sign);
      print_vint128x (" sig ( lt ):", (vui128_t) a_lt_b);
      // Simply vfc == +-0.0. Treat as p_exp == c_exp
      if (vec_cmpud_all_eq (q_exp, c_exp)
       || vec_all_eq((vui32_t ) c_sig, (vui32_t ) q_zero))
	{
	  //vui128_t add_sig, sub_sig, s_sig;
	  vui128_t carry;
	  print_vint128x (" exp ( = ):", (vui128_t) q_exp);

	  // Simply vfc == +-0.0. Treat as p_sign == c_sign
	  if (vec_all_eq (q_sign, c_sign)
	   || vec_all_eq((vui32_t ) c_sig, (vui32_t ) q_zero))
	    { // Same sign, simple add
	      q_sig = vec_adduqm (p_sig_h, c_sig);
	      p_sig_h = q_sig;
	      // q_sign = q_sign;
	      print_vint128x (" sig (add):", (vui128_t) p_sig_h);
	      print_vint128x ("     (add):", (vui128_t) p_sig_l);
	    }
	  else
	    { // different sign, subtract smallest from largest magnitude
	      const vui64_t exp_112 = vec_splats ((unsigned long long) 112);

	      print_vint128x (" sig (vfc):", (vui128_t) c_sig);
	      print_vint128x ("     (vfc):", (vui128_t) c_sig_l);
	      if (vec_cmpuq_all_lt (q_sig, c_sig))
		{
		  carry = vec_subcuq (c_sig_l, p_sig_l);
		  p_sig_l = vec_subuqm (c_sig_l, p_sig_l);
		      print_vint128x (" sig (sub):", (vui128_t) p_sig_l);
		      print_vint128x (" brw (sub):", (vui128_t) carry);
		  q_sig = vec_subeuqm (c_sig, p_sig_h, carry);
		  p_sig_h = q_sig;
		  q_sign = c_sign;
		}
	      else
		{
		  q_sig = vec_subuqm (q_sig, c_sig);
		  p_sig_h = q_sig;
		  // q_sign = q_sign;
		}
	      print_vint128x (" sig (sub):", (vui128_t) p_sig_h);
	      print_vint128x ("     (sub):", (vui128_t) p_sig_l);

	      if (__builtin_expect (vec_cmpuq_all_eq (q_sig, (vui128_t) q_zero),
				    0))
		{
		  if (vec_cmpuq_all_eq (p_sig_l, (vui128_t) q_zero))
		    { // Special case of both zero with different sign
		      q_sign = vec_sel (q_sign, (vui32_t) q_zero, diff_sign);
		      //q_sign = vec_xor (q_sign, c_sign);
		      return vec_xfer_vui32t_2_bin128 (q_sign);
		    }

		  // If p_sig_h == 0 after subtract and p_sig_l != 0
		  // Need to pre-normalize if we can, because the code that
		  // follows is not expecting delta exp > 112
		  // But must maintain q_exp >= E_min
		  if (vec_cmpud_all_gt ((vui64_t) q_exp, exp_112))
		    {
		      vui128_t t_sig;
		      t_sig = (vui128_t) vec_sld ((vui32_t) p_sig_h,
						  (vui32_t) p_sig_l, 14);
		      p_sig_l = vec_slqi (p_sig_l, 112);
		      p_sig_h = t_sig;
		      q_exp = vec_subudm (q_exp, exp_112);
		      print_vint128x (" exp (adj ):", (vui128_t) q_exp);
		      print_vint128x (" sig (lsht):", (vui128_t) p_sig_h);
		      print_vint128x ("     (lsht):", (vui128_t) p_sig_l);
		      q_sig = p_sig_h;
		    }
		}
	    }
	  //q_sig = s_sig;
	  //p_sig_h = s_sig;
	      print_vint128x (" sign(sum):", (vui128_t) q_sign);
	      print_vint128x (" exp (sum):", (vui128_t) q_exp);
	      print_vint128x (" sig (sum):", (vui128_t) p_sig_h);
	      print_vint128x ("     (sum):", (vui128_t) p_sig_l);
	}
      else
	{
	  // 2a) If p_exp > c_exp then
	  //if (vec_cmpuq_all_ne (c_sig, (vui128_t) q_zero))
	    { // and c_sig != zero
	      //     any bits shifted out of b_sig are ORed into the X-bit
	      if (vec_cmpud_all_lt (c_exp, q_exp))
		{ //     shift (c_sig) right by (q_exp - c_exp)
		  vui64_t d_exp, l_exp;
		  vui128_t t_sig, carry;
		  vb128_t exp_mask;
		  const vui64_t exp_128 = vec_const64_f128_128 ();
		  const vui64_t exp_256 = vec_addudm (exp_128, exp_128);

		  d_exp = vec_subudm (q_exp, c_exp);
		  exp_mask = (vb128_t) vec_cmpltud (d_exp, exp_128);
		  print_vint128x (" exp2a(diff):", (vui128_t) d_exp);
		  print_vint128x (" exp2a(mask):", (vui128_t) exp_mask);
		  if (vec_cmpud_all_lt (d_exp, exp_128))
		    {
		      l_exp = vec_subudm (exp_128, d_exp);
		      t_sig = vec_srq (c_sig, (vui128_t) d_exp);
		      c_sig_l = vec_slq (c_sig, (vui128_t) l_exp);
		      t_sig = vec_seluq ((vui128_t) q_zero, t_sig, exp_mask);
		      //c_sig_l = vec_seluq (c_sig, c_sig_l, exp_mask);
		    }
		  else
		    {
		      if (vec_cmpud_all_lt (d_exp, exp_256))
			{
			  d_exp = vec_subudm (d_exp, exp_128);
			  print_vint128x (" exp (diff):", (vui128_t) d_exp);
			  if (vec_cmpud_all_eq (d_exp, (vui64_t) q_zero))
			    {
			      // if (d_exp == 128) then shift 128 not 0 bits
			      c_sig_l = c_sig;
			      c_sig = (vui128_t) q_zero;
			      p_odd = (vui128_t) q_zero;
			    }
			  else
			    {
			      l_exp = vec_subudm (exp_128, d_exp);
			      print_vint128x (" exp (ldif):", (vui128_t) l_exp);
			      t_sig = vec_srq (c_sig, (vui128_t) d_exp);
			      c_sig_l = vec_slq (c_sig, (vui128_t) l_exp);
			      print_vint128x (" sig (c srq):",
					      (vui128_t) t_sig);
			      print_vint128x ("     (c slq):",
					      (vui128_t) c_sig_l);
			      p_odd = vec_addcuq (c_sig_l, (vui128_t) q_ones);
			      c_sig_l = (vui128_t) vec_or ((vui32_t) t_sig,
							   (vui32_t) p_odd);
			      c_sig = (vui128_t) q_zero;
			    }
			}
		      else
			{
			  p_odd = vec_addcuq (c_sig, (vui128_t) q_ones);
			  c_sig = (vui128_t) q_zero;
			  c_sig_l = p_odd;
			}
		      t_sig = c_sig;
		      print_vint128x (" sig ( odd ):", (vui128_t) p_odd);
		    }
		  print_vint128x (" sig (shift):", (vui128_t) t_sig);
		  print_vint128x ("     (shift):", (vui128_t) c_sig_l);

		  //if (vec_cmpuq_all_ne (c_sig, (vui128_t) q_zero))
		    {
		  if (vec_all_eq(q_sign, c_sign))
		    { // Same sign, simple add
		      carry = vec_addcuq (c_sig_l, p_sig_l);
		      p_sig_l = vec_adduqm (c_sig_l, p_sig_l);
		      q_sig = vec_addeuqm (t_sig, p_sig_h, carry);
		      p_sig_h = q_sig;
		      // q_sign = q_sign;
		      print_vint128x (" sig (add):", (vui128_t) p_sig_h);
		      print_vint128x ("     (add):", (vui128_t) p_sig_l);
		    }
		  else
		    {
		      carry = vec_subcuq (p_sig_l, c_sig_l);
		      p_sig_l = vec_subuqm (p_sig_l, c_sig_l);
		      q_sig = vec_subeuqm (p_sig_h, t_sig, carry);
		      p_sig_h = q_sig;
		      // q_sign = q_sign;
		      print_vint128x (" sig (sub):", (vui128_t) p_sig_h);
		      print_vint128x ("     (sub):", (vui128_t) p_sig_l);
		    }
		    }

		}
	      else
		{
		  // 2b) If q_exp < c_exp
		  // Shift p_sig_h | p_sig_l right by (c_exp - q_exp)
		  vui64_t d_exp, l_exp;
		  vui128_t t_sig, carry;
		  vb128_t exp_mask;
		  const vui64_t exp_128 = vec_const64_f128_128 ();
		  const vui64_t exp_256 = vec_addudm (exp_128, exp_128);

		  d_exp = vec_subudm (c_exp, q_exp);
		  exp_mask = (vb128_t) vec_cmpltud (d_exp, exp_128);
		  print_vint128x (" exp2b(diff):", (vui128_t) d_exp);
		  print_vint128x (" exp2b(mask):", (vui128_t) exp_mask);
		  if (vec_cmpud_all_lt (d_exp, exp_128))
		    {
		      // if (d_exp < 128) then shift right 1-127 bits
		      vui128_t p_sig_x;
		      l_exp = vec_subudm (exp_128, d_exp);
		      // Isolate bits shifted out of P_sig_l for x-bit
		      p_sig_x = vec_slq (p_sig_l, (vui128_t) l_exp);
		      t_sig = vec_srq (p_sig_h, (vui128_t) d_exp);
		      p_sig_l = vec_sldq (p_sig_h, p_sig_l, (vui128_t) l_exp);
		      p_odd = vec_addcuq (p_sig_x, (vui128_t) q_ones);
		      p_sig_l = (vui128_t) vec_or ((vui32_t) p_sig_l,
						   (vui32_t) p_odd);
		      p_sig_h = t_sig;
		    }
		  else
		    {
		      if (vec_cmpud_all_lt (d_exp, exp_256))
			{
			  d_exp = vec_subudm (d_exp, exp_128);
			  print_vint128x (" exp (diff):", (vui128_t) d_exp);
			  if (vec_cmpud_all_eq (d_exp, (vui64_t) q_zero))
			    {
			      // if (d_exp == 128) then shift 128 not 0 bits
			      p_odd = vec_addcuq (p_sig_l, (vui128_t) q_ones);
			      p_sig_l = (vui128_t) vec_or ((vui32_t) p_sig_h,
							   (vui32_t) p_odd);
			      p_sig_h = (vui128_t) q_zero;
			    }
			  else
			    {
			      // if (d_exp > 128) then shift right 128+ bits
			      vui128_t p_sig_x;
			      l_exp = vec_subudm (exp_128, d_exp);
			      print_vint128x (" exp (ldif):", (vui128_t) l_exp);
			      t_sig = vec_srq (p_sig_h, (vui128_t) d_exp);
			      p_sig_x = vec_slq (p_sig_h, (vui128_t) l_exp);
			      print_vint128x (" sig (p srq):",
					      (vui128_t) t_sig);
			      print_vint128x ("     (p slq):",
					      (vui128_t) p_sig_x);
			      p_sig_x = (vui128_t) vec_or ((vui32_t) p_sig_x,
							   (vui32_t) p_sig_l);
			      p_odd = vec_addcuq (p_sig_x, (vui128_t) q_ones);
			      p_sig_l = (vui128_t) vec_or ((vui32_t) t_sig,
							   (vui32_t) p_odd);
			      p_sig_h = (vui128_t) q_zero;
			    }
			}
		      else
			{ // Shift >= 256, Accumulate all bits from p_sig_h | p_sig_l into x-bit
			  t_sig = (vui128_t) vec_or ((vui32_t) p_sig_h,
						     (vui32_t) p_sig_l);
			  p_odd = vec_addcuq (t_sig, (vui128_t) q_ones);
			  print_vint128x (" sig (t srq):", (vui128_t) t_sig);
			  print_vint128x ("     (p odd):", (vui128_t) p_odd);
			  // Zero all product bits (includeing GR) while preserving X-bit
			  p_sig_h = (vui128_t) q_zero;
			  p_sig_l = p_odd;
			}
		      //t_sig = c_sig;
		      print_vint128x (" sig ( odd ):", (vui128_t) p_odd);
		    }
		  print_vint128x (" sig (shift):", (vui128_t) p_sig_h);
		  print_vint128x ("     (shift):", (vui128_t) p_sig_l);

		  c_sig_l = (vui128_t) q_zero;
		 // if (vec_cmpuq_all_ne (c_sig, (vui128_t) q_zero))
		    {
		  if (vec_all_eq(q_sign, c_sign))
		    { // Same sign, simple add
		      carry = vec_addcuq (c_sig_l, p_sig_l);
		      p_sig_l = vec_adduqm (c_sig_l, p_sig_l);
		      p_sig_h = vec_addeuqm (c_sig, p_sig_h, carry);
		      //q_sig = p_sig_h;
		      q_sign = c_sign;
		      print_vint128x (" sig (add):", (vui128_t) p_sig_h);
		      print_vint128x ("     (add):", (vui128_t) p_sig_l);
		    }
		  else
		    {
		      carry = vec_subcuq (c_sig_l, p_sig_l);
		      p_sig_l = vec_subuqm (c_sig_l, p_sig_l);
		      p_sig_h = vec_subeuqm (c_sig, p_sig_h, carry);
		      //q_sig = p_sig_h;
		      q_sign = c_sign;
		      print_vint128x (" sig (sub):", (vui128_t) p_sig_h);
		      print_vint128x ("     (sub):", (vui128_t) p_sig_l);
		    }
		    }
		  q_sig = p_sig_h;
		  q_exp = c_exp;
		}
	    }

	}

      // check for significand overfloaw again
	{
	  vb128_t carry_mask;
	  vui128_t sig_h, sig_l;
	  // Test Carry-bit (greater than L-bit)
	  vui16_t sig_l_mask = vec_splat_u16(1);
	  vui16_t t_sig = vec_splat ((vui16_t) p_sig_h, VEC_HW_H);
	  carry_mask = (vb128_t) vec_cmpgt (t_sig, sig_l_mask);
	  // Shift double quadword right 1 bit
	  p_tmp = vec_sldqi (p_sig_h, p_sig_l, 120);
	  sig_h = vec_srqi (p_sig_h, 1);
	  sig_l = vec_slqi (p_tmp, 7);
	  // Increment the exponent
	  x_exp = vec_addudm (q_exp, exp_one);
	  // Select original or normalized exp/sig
	  p_sig_h = vec_seluq (p_sig_h, sig_h, carry_mask);
	  p_sig_l = vec_seluq (p_sig_l, sig_l, carry_mask);
	  q_exp = vec_selud (q_exp, x_exp, (vb64_t) carry_mask);
	}
      // There are two cases for denormal
      // 1) The sum of unbiased exponents is less the E_min (tiny).
      // 2) The significand is less then 1.0 (C and L-bits are zero).
      //  2a) The exponent is > E_min
      //  2b) The exponent is == E_min
      //
      // Check for Tiny exponent
      if (__builtin_expect (
	  (vec_cmpsd_all_lt ((vi64_t) q_exp, (vi64_t) exp_min)), 0))
	{
	  //const vui64_t exp_128 = (vui64_t) { 128, 128 };
	  const vui64_t exp_128 = vec_const64_f128_128();
	  const vui64_t too_tiny = (vui64_t) { 116, 116 };
	  // const vui32_t xmask = CONST_VINT128_W(0x1fffffff, -1, -1, -1);
	  vui32_t xmask = (vui32_t) vec_srqi ((vui128_t) q_ones, 3);
	  vui32_t tmp;

	  // Intermediate result is tiny, unbiased exponent < -16382
	  //x_exp = vec_subudm ((vui64_t) exp_tiny, q_exp);
	  x_exp = vec_subudm (exp_min, q_exp);

	  if (vec_cmpud_all_gt ((vui64_t) x_exp, too_tiny))
	    {
	      // Intermediate result is too tiny, the shift will
	      // zero the fraction and the GR-bit leaving only the
	      // Sticky bit. The X-bit needs to include all bits
	      // from p_sig_h and p_sig_l
	      p_sig_l = vec_srqi (p_sig_l, 8);
	      p_sig_l = (vui128_t) vec_or ((vui32_t) p_sig_l,
					   (vui32_t) p_sig_h);
	      // generate a carry into bit-2 for any nonzero bits 3-127
	      p_sig_l = vec_adduqm (p_sig_l, (vui128_t) xmask);
	      q_sig = (vui128_t) q_zero;
	      p_sig_l = (vui128_t) vec_andc ((vui32_t) p_sig_l, xmask);
	    }
	  else
	    { // Normal tiny, right shift may loose low order bits
	      // from p_sig_l. So collect any 1-bits below GRX and
	      // OR them into the X-bit, before the right shift.
	      vui64_t l_exp;

	      // Propagate low order bits into the sticky bit
	      // GRX left adjusted in p_sig_l
	      // Issolate bits below GDX (bits 3-128).
	      tmp = vec_and ((vui32_t) p_sig_l, xmask);
	      // generate a carry into bit-2 for any nonzero bits 3-127
	      tmp = (vui32_t) vec_adduqm ((vui128_t) tmp, (vui128_t) xmask);
	      // Or this with the X-bit to propagate any sticky bits into X
	      p_sig_l = (vui128_t) vec_or ((vui32_t) p_sig_l, tmp);
	      p_sig_l = (vui128_t) vec_andc ((vui32_t) p_sig_l, xmask);

	      l_exp = vec_subudm (exp_128, x_exp);
	      p_sig_l = vec_sldq (p_sig_h, p_sig_l, (vui128_t) l_exp);
	      p_sig_h = vec_srq (p_sig_h, (vui128_t) x_exp);
	      q_sig = p_sig_h;
	    }
	  // Set the exponent for denormal
	  q_exp = exp_dnrm;
	}
      // Exponent is not tiny but significand may be denormal
      // Isolate sig CL bits and compare
      vui16_t t_sig = vec_splat ((vui16_t) p_sig_h, VEC_HW_H);
      if (__builtin_expect ((vec_all_eq(t_sig, (vui16_t ) q_zero)), 0))
	{
	  // Is below normal range. This can happen when
	  // multiplying a denormal by a normal.
	  // So try to normalize the significand.
	  //const vui64_t exp_15 = { 15, 15 };
	  const vui64_t exp_15 = vec_splat_u64 (15);
	  //const vui64_t exp_112 = vec_splats ((unsigned long long) 112);
	  vui64_t c_exp, d_exp;
	  vui128_t c_sig;
	  vb64_t exp_mask;

	  print_vint128x (" exp (norm):", (vui128_t) q_exp);
	  print_vint128x (" sig (norm):", (vui128_t) p_sig_h);
	  print_vint128x ("     (norm):", (vui128_t) p_sig_l);
#if 0
	  // Not sure this test is needed here
	  if (vec_cmpuq_all_eq (p_sig_h, (vui128_t) q_zero)
	      && vec_cmpud_all_gt ((vui64_t) q_exp, exp_112))
	    {
	      vui128_t t_sig;
	      t_sig = (vui128_t) vec_sld ((vui32_t) p_sig_h, (vui32_t) p_sig_l, 14);
	      p_sig_l = vec_slqi (p_sig_l, 112);
	      p_sig_h = t_sig;
	      q_exp = vec_subudm (q_exp, exp_112);
	      print_vint128x (" exp (adj?):", (vui128_t) q_exp);
	      print_vint128x (" sig (lsht):", (vui128_t) p_sig_h);
	      print_vint128x ("     (lsht):", (vui128_t) p_sig_l);
	      q_sig = p_sig_h;
	    }
#endif
	  c_sig = vec_clzq (p_sig_h);
	  print_vint128x (" exp (clzq):", (vui128_t) c_sig);
	  c_exp = vec_splatd ((vui64_t) c_sig, VEC_DW_L);
	  c_exp = vec_subudm (c_exp, exp_15);
	  print_vint128x (" exp (c-15):", (vui128_t) c_exp);
	  d_exp = vec_subudm (q_exp, exp_min);
	  print_vint128x (" exp (diff):", (vui128_t) d_exp);
	  d_exp = vec_minud (c_exp, d_exp);
	  print_vint128x (" exp (min ):", (vui128_t) d_exp);
	  exp_mask = vec_cmpgtud (q_exp, c_exp);

	  // Intermediate result <= tiny, unbiased exponent <= -16382
	  if (vec_cmpsd_all_gt ((vi64_t) q_exp, (vi64_t) exp_min))
	    {
	      // Try to normalize the significand.
	      p_sig_h = vec_sldq (p_sig_h, p_sig_l, (vui128_t) d_exp);
	      p_sig_l = vec_slq (p_sig_l, (vui128_t) d_exp);
	      q_sig = p_sig_h;
	      // Compare computed exp to shift count to normalize.
	      //exp_mask = vec_cmpgtud (q_exp, c_exp);
	      q_exp = vec_subudm (q_exp, d_exp);
	      q_exp = vec_selud (exp_dnrm, q_exp, exp_mask);
	    }
	  else
	    { // sig is denormal range (L-bit is 0). Set exp to zero.
	      q_exp = exp_dnrm;
	    }
	}
      // Merge sign early will not effect rounding for this mode
      // q_ssig = vec_or ((vui32_t) q_sig, q_sign);
      // Round to odd from lower product bits
      p_odd = vec_addcuq (p_sig_l, (vui128_t) q_ones);
      q_sig = (vui128_t) vec_or ((vui32_t) q_sig, (vui32_t) p_odd);

      // Check for exponent overflow -> __FLT128_MAX__ (round to odd)
      if (__builtin_expect ((vec_cmpud_all_ge ( q_exp, exp_naninf)), 0))
	{
	  // Intermediate result is huge, unbiased exponent > 16383
	  // so return __FLT128_MAX__ with the appropriate sign.
	  const vui32_t f128_max = CONST_VINT128_W(0x7ffeffff, -1, -1, -1);
	  vui32_t f128_smax = vec_or ((vui32_t) f128_max, q_sign);
	  return vec_xfer_vui32t_2_bin128 (f128_smax);
	}
      else // combine sign, exp, and significand for return
	{
	  // Merge sign, significand, and exponent into final result
	  q_sig = (vui128_t) vec_or ((vui32_t) q_sig, q_sign);
	  vui32_t tmp, t128;
	  // convert DW exp_naninf to QW expmask
	  vui32_t expmask = vec_sld ((vui32_t) exp_naninf, q_zero, 14);
	  // convert q_exp from DW to QW for QP format
	  tmp = vec_sld ((vui32_t) q_exp, q_zero, 14);
	  t128 = vec_sel ((vui32_t) q_sig, tmp, expmask);
	  result = vec_xfer_vui32t_2_bin128 (t128);
	  return result;
	}
    }
  else
    { // One, two, or three operands are NaN or Infinity
      //const vui32_t q_nan = CONST_VINT128_W(0x00008000, 0, 0, 0);
      vui32_t q_nan = vec_mask128_f128Qbit ();
      vui32_t q_inf = vec_mask128_f128exp ();
      // the product operands are NaN or Infinity
      if (vec_cmpud_any_eq (x_exp, exp_naninf))
	{
	  // One or both operands are NaN
	  if (vec_all_isnanf128 (vfa))
	    {
	      // vfa is NaN, Convert vfa to QNaN and return
	      vui32_t vf128 = vec_or_bin128_2_vui32t (vfa, q_nan);
	      return vec_xfer_vui32t_2_bin128 (vf128);
	    }
	  else if (vec_all_isnanf128 (vfb))
	    {
	      if (vec_all_isnanf128 (vfc))
		{
		  // vfb is NaN, Convert vfc to QNaN and return
		  vui32_t vf128 = vec_or_bin128_2_vui32t (vfc, q_nan);
		  return vec_xfer_vui32t_2_bin128 (vf128);
		}
	      else
		{
		  // vfb is NaN, Convert vfb to QNaN and return
		  vui32_t vf128 = vec_or_bin128_2_vui32t (vfb, q_nan);
		  return vec_xfer_vui32t_2_bin128 (vf128);
		}
	    }
	  else  // Or one or both operands are Infinity
	    {
	      if (vec_cmpud_all_eq (x_exp, (vui64_t) exp_naninf))
		{
		  // Infinity x Infinity == signed Infinity
		  q_sig = (vui128_t) q_inf;
		}
	      else
		{
		  // One each Infinity/Finite value, check for 0.0
		  if (vec_cmpuq_all_eq (a_mag, (vui128_t) q_zero)
		      || vec_cmpuq_all_eq (b_mag, (vui128_t) q_zero))
		    {
		      if (vec_all_isnanf128 (vfc))
			{
			      // vfb is NaN, Convert vfb to QNaN and return
			      vui32_t vf128 = vec_or_bin128_2_vui32t (vfc, q_nan);
			      return vec_xfer_vui32t_2_bin128 (vf128);
			}
		      else
		      // Inifinty x Zero is Default Quiet NaN
		        return vec_const_nanf128 ();
		    }
		  else // an Infinity and a Nonzero finite number
		    {
		      // Return Infinity with product sign.
		      q_sig = (vui128_t) q_inf;
		    }
		}
	      // At this point we have a signed infinity product
	      // Need to check addend for nan/infinity
	      if (vec_all_isnanf128 (vfc))
		{
		  // vfc is NaN, Convert vfc to QNaN and return
		  vui32_t vf128 = vec_or_bin128_2_vui32t (vfc, q_nan);
		  return vec_xfer_vui32t_2_bin128 (vf128);
		}
	      else  // Or addend is also Infinity
		{
		  if (vec_cmpud_all_eq (c_exp, exp_naninf)
		      && vec_cmpud_any_ne ((vui64_t) q_sign, (vui64_t) c_sign))
		    { // Both operands infinity and opposite sign
		      // Inifinty + Infinity (opposite sign) is Default Quiet NaN
		      return vec_const_nanf128 ();
		    }
		    }
	      // Merge sign, exp/sig into final result
	      q_sig = (vui128_t) vec_or ((vui32_t) q_sig, q_sign);
	      return vec_xfer_vui128t_2_bin128 (q_sig);
	    }
	}
      else // The product is finite but the addend is not
	{
	      if (vec_all_isnanf128 (vfc))
		{
		  // vfa is NaN, Convert vfa to QNaN and return
		  vui32_t vf128 = vec_or_bin128_2_vui32t (vfc, q_nan);
		  return vec_xfer_vui32t_2_bin128 (vf128);
		}
	      else  // Or addend is Infinity
		{
		  // return infinity
		  return vfc;
		}
	}
    }
#else
  result = vfa * vfb;
#endif
  return result;
}
#endif

//#define __DEBUG_PRINT__ 1
#ifdef __DEBUG_PRINT__
vui128_t db_vec_diveuqo_V0 (vui128_t x, vui128_t z)
{
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  vui128_t res;
#if (__GNUC__ >= 12)
  res = vec_dive (x, z);
#else
  __asm__(
      "vdiveuq %0,%1,%2;\n"
      : "=v" (res)
      : "v" (x), "v" (z)
      : );
#endif
  return res;
#elif  (_ARCH_PWR8)
  vui128_t u = x;
  vui128_t v = z;
  const vui64_t zeros = vec_splat_u64 (0);
  const vui128_t mone = (vui128_t) CONST_VINT128_DW(-1, -1);
  vui128_t u0, u1, v1, q0, k, s, t, vn;
  vui64_t vdh, vdl, qdl, qdh;
#ifdef __DEBUG_PRINT__
  print_vint128x ("db_vec_divuqe u ", (vui128_t) u);
  print_vint128x ("              v ", (vui128_t) v);
#endif

  if (vec_cmpuq_all_ge (x, z) || vec_cmpuq_all_eq (z, (vui128_t) zeros))
    {
      printf (" undef -- overlow or zero divide\n");
      return mone;
    }
  else
    {
      //udh = vec_splatd ((vui64_t) u, 1);
      vdh = vec_splatd ((vui64_t) v, VEC_DW_H);
      vdl = vec_splatd ((vui64_t) v, VEC_DW_L);

      // if (/*v >> 64 == 0UL*/vec_cmpud_all_eq (vdh, zeros))
      if (/*v >> 64 == 0UL*/vec_cmpud_all_eq (vdl, zeros))
	{
	  //if (/*u >> 64 < v*/vec_cmpud_all_lt (udh, /*vdl*/vdh))
	    {
	      u0 = (vui128_t) vec_swapd ((vui64_t) u);
	      qdh = vec_divqud_inline (u/*0*/, vdh);
#ifdef __DEBUG_PRINT__
	      print_vint128x (" (udh < vdh) u  ", (vui128_t) u);
	      print_vint128x ("            qdh ", (vui128_t) qdh);
#endif
	      // vec_divqud already provides the remainder in qdh[1]
	      // k = u1 - q1*v; ((k << 64) + u0);
	      // Simplifies to:
	      u1 = (vui128_t) vec_pasted (qdh, (vui64_t) zeros /*u0*/);
	      qdl = vec_divqud_inline (u1, vdh);
#ifdef __DEBUG_PRINT__
	      print_vint128x ("             u1 ", (vui128_t) u1);
	      print_vint128x ("            qdl ", (vui128_t) qdl);
#endif
	      //return (vui128_t) qdl;
	      q0 = (vui128_t) vec_mrgald ((vui128_t) qdh, (vui128_t) qdl);
	      s = (vui128_t) vec_mrgahd ((vui128_t) qdl, (vui128_t) zeros);
#ifdef __DEBUG_PRINT__
	      print_vint128x (" (qdh !! qdl) q ", (vui128_t) q0);
	      print_vint128x (" (rdl !! 0)   s ", (vui128_t) s);
#endif
	      // Convert nonzero remainder into a carry (=1).
	      t = vec_addcuq (s, mone);
	      q0 = (vui128_t) vec_or ((vui32_t) q0, (vui32_t) t);
#if 1 //def __DEBUG_PRINT__
	      print_vint128x ("     r != 0 ->c ", (vui128_t) t);
	      print_vint128x ("     q0||c ->q' ", (vui128_t) q0);
#endif
	      return q0;
	    }
#if 0
	  else
	    {
#ifdef __DEBUG_PRINT__
	      printf ("db_vec_divuqe !!Should not happen! \n");
#endif
	      // TODO 1st test for overflow means this can not happen?!
	      //u1 = u >> 64;
	      u1 = (vui128_t) vec_mrgahd ((vui128_t) zeros, u);
	      // u0 = u & lmask;
	      u0 = (vui128_t) vec_mrgald ((vui128_t) zeros, u);
#ifdef __DEBUG_PRINT__
	      print_vint128x (" (udh >= vdl) u0 ", (vui128_t) u0);
	      print_vint128x ("             u1  ", (vui128_t) u1);
	      print_vint128x ("             vd1 ", (vui128_t) vdl);
#endif
	      //q1 = scalar_divdud (u1, (unsigned long long) v) & lmask;
	      qdh = vec_divqud_inline (u1, vdl);
	      //k = u1 - q1*v;
	      // vec_divqud already provides the remainder in qdh[1]
	      // k = u1 - q1*v; ((k << 64) + u0);
	      // Simplifies to:
	      k = (vui128_t) vec_pasted (qdh, (vui64_t) u0);
	      // q0 = scalar_divdud ((k << 64) + u0, (unsigned long long) v) & lmask;
	      qdl = vec_divqud_inline (k, vdl);
#ifdef __DEBUG_PRINT__
	      print_vint128x ("             qdh ", (vui128_t) qdh);
	      print_vint128x ("             k   ", (vui128_t) k);
	      print_vint128x ("             qdl ", (vui128_t) qdl);
#endif
	      //return (q1 << 64) + q0;
	      q0 = (vui128_t) vec_mrgald ((vui128_t) qdh, (vui128_t) qdl);
	      s = (vui128_t) vec_mrgahd ((vui128_t) qdl, (vui128_t) zeros);
		  // Convert nonzero remainder into a carry (=1).
	      t = vec_addcuq (s, mone);
	      q0 = (vui128_t) vec_or ((vui32_t) q0, (vui32_t) t);
#if 1 //def __DEBUG_PRINT__
	      print_vint128x ("     u?!!=0 ->c ", (vui128_t) t);
	      print_vint128x ("    q0''|c ->q0 ", (vui128_t) q0);
#endif
	      return q0;
	    }
#endif
	}
      else
      //if (/*vec_cmpuq_all_ge (u1, (vui128_t) vdh)*/ 1)
	{
	  const vui64_t ones = vec_splat_u64 (1);
	  vui128_t u2, k1, t2;
	  vb128_t Bgt;
	  // Here v >= 2**64, Normalize the divisor so MSB is 1
	  //n = __builtin_clzl ((unsigned long long)(v >> 64)); // 0 <= n <= 63
	  // Could use vec_clzq(), but we know  v >= 2**64, So:
	  vn = (vui128_t) vec_clzd ((vui64_t) v);
	  // vn = vn >> 64;, So we can use it with vec_slq ()
	  vn = (vui128_t) vec_mrgahd ((vui128_t) zeros, vn);
	  //v1 = (v << n) >> 64;
	  //v1 = vec_slq (v, vn);

	  //u1 = u >> 1; 	// to insure no overflow
	  //u1 = vec_srqi (u, 1);
	  v1 = v;
	  u1 = u;
#ifdef __DEBUG_PRINT__
	  print_vint128x ("             vn ", (vui128_t) vn);
	  print_vint128x ("             v1 ", (vui128_t) v1);
	  print_vint128x ("             u1 ", (vui128_t) u1);
#endif
	  // vdh = vec_mrgahd (v1, (vui128_t) zeros);
	  if (vec_cmpuq_all_eq (u1, (vui128_t) zeros))
	    {
	      return ((vui128_t) zeros);
	    }

	  u1 = vec_slq (u1, vn);
	  v1 = vec_slq (v1, vn);
#ifdef __DEBUG_PRINT__
	  print_vint128x ("         u1<<vn ", (vui128_t) u1);
	  print_vint128x ("         v1<<vn ", (vui128_t) v1);
#endif
	  //u1 = vec_srqi (u1, 1);
	  vdh = vec_mrgahd ((vui128_t) zeros, v1);
	  //udh = vec_mrgahd ((vui128_t) zeros, u1); // !!
	  qdh = vec_divqud_inline (u1, (vui64_t) v1);
#if 0
	  q0 = (vui128_t) vec_mrgald ((vui128_t) qdh, (vui128_t) qdh);
#ifdef __DEBUG_PRINT__
	  print_v2xint64 ("   u1/v1h = qdh ", qdh);
	  print_vint128x ("            q0  ", (vui128_t) q0);
#endif
	  if (vec_cmpud_all_eq ((vui64_t) q0, zeros)
	  /* && vec_cmpud_all_eq (vdh, udh)*/) // !!
	    { // this depends on U != 0
	      vb64_t Beq;
	      // detect overflow if ((x >> 64) == ((z >> 64)))
	      // a doubleword boolean true == __UINT64_MAX__
	      Beq = vec_cmpequd ((vui64_t) u1, (vui64_t) v1);
	      // Beq >> 64
	      Beq = (vb64_t) vec_mrgahd ((vui128_t) zeros, (vui128_t) Beq);
#ifdef __DEBUG_PRINT__
	      print_v2xint64 (" qdh<-UINT128_MAX ", qdh);
	      print_v2xint64 ("     vdh eq udh = ", (vui64_t) Beq);
#endif
	      // Adjust quotient (-1) for divide overflow
	      qdh = (vui64_t) vec_or ((vui32_t) Beq, (vui32_t) qdh);
#ifdef __DEBUG_PRINT__
	      print_v2xint64 ("  corrected qdh = ", qdh);
#endif
	    }
#endif
	  q0 = vec_slqi ((vui128_t) qdh, 64);
#ifdef __DEBUG_PRINT__
	  print_v2xint64 ("   u1/v1h = qdh", qdh);
	  print_vint128x ("             q0 ", (vui128_t) q0);
#endif
	  // {k, k1}  = vec_muludq (v1, q0);
#if 1
	    {
	      vui128_t l128, h128;
	      vui64_t b_eud = vec_mrgald ((vui128_t) qdh, (vui128_t) qdh);
	      l128 = vec_vmuloud ((vui64_t) v1, b_eud);
	      h128 = vec_vmaddeud ((vui64_t) v1, b_eud, (vui64_t) l128);
	      // 192-bit product of v1 * q-estimate
	      k = h128;
	      k1 = vec_slqi (l128, 64);
	    }
#else
	  k1 = vec_muludq (&k, v1, q0);
#endif
	  u2 = vec_subuqm ((vui128_t) zeros, k1);
	  t = vec_subcuq ((vui128_t) zeros, k1);
	  u0 = vec_subeuqm (u1, k, t);
	  t2 = vec_subecuq (u1, k, t);
	  Bgt = vec_setb_ncq (t2);
#ifdef __DEBUG_PRINT__
	  print_vint128x (" hq(v1*q0) = k  ", (vui128_t) k);
	  print_vint128x (" lq(q0*v1) = k1 ", (vui128_t) k1);
	  print_vint128x ("     u1-k = u0  ", (vui128_t) u0);
	  print_vint128x ("     u1-k = u2  ", (vui128_t) u2);
	  print_vint128x ("     u1-k = t2  ", (vui128_t) t2);
	  print_vint128x ("     k>u1 = Bgt ", (vui128_t) Bgt);
#endif
	  u0 = vec_sldqi (u0, u2, 64);
	  //if (vec_cmpuq_all_gt (k, u1))
	    {
	      vui128_t q2;
	      q2 = (vui128_t) vec_subudm ((vui64_t) q0, ones);
	      q2 = (vui128_t) vec_mrgahd ((vui128_t) q2, (vui128_t) zeros);
	      //vdh = vec_mrgahd (v1, (vui128_t) zeros);
	      u2 = vec_adduqm ((vui128_t) u0, v1);
	      //t2 = vec_subuqm (u0, (vui128_t) vdh);
	      q0 = vec_seluq (q0, q2, Bgt);
	      u0 = vec_seluq (u0, u2, Bgt);
	      if (vec_cmpuq_all_eq ((vui128_t) Bgt, mone))
		{
#ifdef __DEBUG_PRINT__
		  print_vint128x (" hq(v1*q0) > u1 ", (vui128_t) u1);
		  print_vint128x ("     q0-1 = q0  ", (vui128_t) q0);
		  print_vint128x ("     u1-k'= u0  ", (vui128_t) u0);
		  print_vint128x ("     u1-k'= u2  ", (vui128_t) u2);
#endif
		}
	    }
	  qdh = (vui64_t) vec_mrgahd ((vui128_t) zeros, (vui128_t) q0);
	  //u0 = vec_sldqi (u0, u2, 64);
#ifdef __DEBUG_PRINT__
	  print_vint128x (" (u0|u2)<<64=u0 ", (vui128_t) u0);
	  print_vint128x ("   q0>>64 = qdh ", (vui128_t) qdh);
#endif
	  qdl = vec_divqud_inline (u0, (vui64_t) v1);
#ifdef __DEBUG_PRINT__
	  print_vint128x ("            v1  ", (vui128_t) v1);
	  print_vint128x ("            u0  ", (vui128_t) u0);
	  print_vint128x ("    u0/v1 = qdl ", (vui128_t) qdl);
#endif
	  q0 = (vui128_t) vec_mrgald ((vui128_t) qdh, (vui128_t) qdl);
#ifdef __DEBUG_PRINT__
	  print_vint128x ("            q0' ", (vui128_t) q0);
#endif
	    {
	      vui128_t q2, u2, k1;
	      vui128_t s, s0, s1;
	      k1 = vec_muludq (&k, q0, v1);
#ifdef __DEBUG_PRINT__
	      print_vint128x ("            u1  ", (vui128_t) u1);
	      print_vint128x ("      hq(q0*v1) ", (vui128_t) k);
	      print_vint128x ("      lq(q0*v1) ", (vui128_t) k1);
#endif
#if 0
	      if (vec_cmpuq_all_gt (k, u1)
		  || (vec_cmpuq_all_eq (k, u1)
		      && vec_cmpuq_all_ne (k1, (vui128_t) zeros)))
#endif
		// need to verify that double quadword product (q0*v1)
		// is less than u1 || qw0. This is effectively a
		// double quadword remainder calculation, where 0 carry
		// indicates that the initial Q estimate is too high.
		{
		  vb128_t CCk;
		  u2 = vec_subuqm ((vui128_t) zeros, k1);
		  t = vec_subcuq ((vui128_t) zeros, k1);
		  u0 = vec_subeuqm (u1, k, t);
		  t2 = vec_subecuq (u1, k, t);
		  CCk = vec_setb_ncq (t2);
		  // The remainder should fit into u2 (u0 == 0) and
		  // should be less than v1.
		  // If not it will be after correction.
		  // Collect initial remainder for sticky-bits.
		  // s0 = (vui128_t) vec_or ((vui32_t) u0, (vui32_t) u2);
		  s0 = u2;
#ifdef __DEBUG_PRINT__
		  print_vint128x (" hq(q0*v1) = k  ", (vui128_t) k);
		  print_vint128x (" lq(q0*v1) = k1 ", (vui128_t) k1);
		  print_vint128x ("     u1-k = u0  ", (vui128_t) u0);
		  print_vint128x ("     u1-k = u2  ", (vui128_t) u2);
		  print_vint128x ("      CCk mask  ", (vui128_t) CCk);
		  print_vint128x ("      X-sticky  ", (vui128_t) s0);
#endif
		  // If remainder (u2) >= divisor (v1) implies Q + 1
		  // So Correct Q and adjust remainder
		  q2 = vec_adduqm (q0, mone);
		  q0 = vec_seluq (q0, q2, CCk);
		  // Subtract divisor (v1) from initial remainder (u2)
		  // for corrected remainder.
		  t = vec_subcuq (u2, v1);
		  u2 = vec_subuqm (u2, v1);
		  u0 = vec_subeuqm (u0, (vui128_t) zeros, t);
		  // Collect corrected remainder for sticky-bits.
		  // s1 = (vui128_t) vec_or ((vui32_t) u0, (vui32_t) u2);
		  s1 = u2;

		  if (vec_cmpuq_all_eq ((vui128_t) CCk, mone))
		    {
#if 1 //def __DEBUG_PRINT__
		      print_vint128x ("         q0''-1 ", (vui128_t) q0);
		      print_vint128x ("     u0-v1 = u0 ", (vui128_t) u0);
		      print_vint128x ("     u2-v1 = u2 ", (vui128_t) u2);
		      print_vint128x ("      X-sticky' ", (vui128_t) s1);
#endif
		    }
		  // Test for nonzero remainder and round to odd
		  // Select from initial or corrected sticky bits
		  s  = vec_seluq (s0, s1, CCk);
		  // Convert nonzero remainder into a carry (=1).
		  t2 = vec_addcuq (s, mone);
		  q0 = (vui128_t) vec_or ((vui32_t) q0, (vui32_t) t2);
#if 1 //def __DEBUG_PRINT__
		  print_vint128x ("     u?!!=0 ->c ", (vui128_t) t2);
		  print_vint128x ("    q0''|c ->q0 ", (vui128_t) q0);
#endif
		}
	    }
#ifdef __DEBUG_PRINT__
	  print_vint128x ("           q0'' ", (vui128_t) q0);
#endif
	  return q0;
	}
    }
#endif
}
#endif

//#define __DEBUG_PRINT__ 1
#ifdef __DEBUG_PRINT__
vui128_t
db_vec_diveuqo (vui128_t x, vui128_t z)
{
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  vui128_t res;
#if (__GNUC__ >= 12)
  res = vec_dive (x, z);
#else
  __asm__(
      "vdiveuq %0,%1,%2;\n"
      : "=v" (res)
      : "v" (x), "v" (z)
      : );
#endif
  return res;
#elif  (_ARCH_PWR8)
  vui128_t u = x;
  vui128_t v = z;
  const vui64_t zeros = vec_splat_u64 (0);
  const vui128_t mone = (vui128_t) CONST_VINT128_DW(-1, -1);
  vui128_t u0, u1, v1, q0, k, s, t, vn;
  vui64_t vdh, vdl, qdl, qdh;
#ifdef __DEBUG_PRINT__
  print_vint128x ("db_vec_divuqe u ", (vui128_t) u);
  print_vint128x ("              v ", (vui128_t) v);
#endif

  if (vec_cmpuq_all_ge (x, z) || vec_cmpuq_all_eq (z, (vui128_t) zeros))
    {
      printf (" undef -- overlow or zero divide\n");
      return mone;
    }
  else
    {
      //udh = vec_splatd ((vui64_t) u, 1);
      vdh = vec_splatd ((vui64_t) v, VEC_DW_H);
      vdl = vec_splatd ((vui64_t) v, VEC_DW_L);

      // if (/*v >> 64 == 0UL*/vec_cmpud_all_eq (vdh, zeros))
      if (/*v >> 64 == 0UL*/vec_cmpud_all_eq (vdl, zeros))
	{
	  //if (/*u >> 64 < v*/vec_cmpud_all_lt (udh, /*vdl*/vdh))
	    {
	      u0 = (vui128_t) vec_swapd ((vui64_t) u);
	      qdh = vec_divqud_inline (u/*0*/, vdh);
#ifdef __DEBUG_PRINT__
	      print_vint128x (" (udh < vdh) u  ", (vui128_t) u);
	      print_vint128x ("            qdh ", (vui128_t) qdh);
#endif
	      // vec_divqud already provides the remainder in qdh[1]
	      // k = u1 - q1*v; ((k << 64) + u0);
	      // Simplifies to:
	      u1 = (vui128_t) vec_pasted (qdh, (vui64_t) zeros /*u0*/);
	      qdl = vec_divqud_inline (u1, vdh);
#ifdef __DEBUG_PRINT__
	      print_vint128x ("             u1 ", (vui128_t) u1);
	      print_vint128x ("            qdl ", (vui128_t) qdl);
#endif
	      //return (vui128_t) qdl;
	      q0 = (vui128_t) vec_mrgald ((vui128_t) qdh, (vui128_t) qdl);
	      s = (vui128_t) vec_mrgahd ((vui128_t) qdl, (vui128_t) zeros);
#ifdef __DEBUG_PRINT__
	      print_vint128x (" (qdh !! qdl) q ", (vui128_t) q0);
	      print_vint128x (" (rdl !! 0)   s ", (vui128_t) s);
#endif
	      // Convert nonzero remainder into a carry (=1).
	      t = vec_addcuq (s, mone);
	      q0 = (vui128_t) vec_or ((vui32_t) q0, (vui32_t) t);
#if 1 //def __DEBUG_PRINT__
	      print_vint128x ("     r != 0 ->c ", (vui128_t) t);
	      print_vint128x ("     q0||c ->q' ", (vui128_t) q0);
#endif
	      return q0;
	    }
	}
      else
      //if (/*vec_cmpuq_all_ge (u1, (vui128_t) vdh)*/ 1)
	{
	  const vui64_t ones = vec_splat_u64 (1);
	  vui128_t u2, k1, t2;
	  vb128_t Bgt;
	  // Here v >= 2**64, Normalize the divisor so MSB is 1
	  //n = __builtin_clzl ((unsigned long long)(v >> 64)); // 0 <= n <= 63
	  // Could use vec_clzq(), but we know  v >= 2**64, So:
	  vn = (vui128_t) vec_clzd ((vui64_t) v);
	  // vn = vn >> 64;, So we can use it with vec_slq ()
	  vn = (vui128_t) vec_mrgahd ((vui128_t) zeros, vn);
	  //v1 = (v << n) >> 64;
	  //v1 = vec_slq (v, vn);

	  //u1 = u >> 1; 	// to insure no overflow
	  //u1 = vec_srqi (u, 1);
	  v1 = v;
	  u1 = u;
#ifdef __DEBUG_PRINT__
	  print_vint128x ("             vn ", (vui128_t) vn);
	  print_vint128x ("             v1 ", (vui128_t) v1);
	  print_vint128x ("             u1 ", (vui128_t) u1);
#endif
	  // vdh = vec_mrgahd (v1, (vui128_t) zeros);
	  if (vec_cmpuq_all_eq (u1, (vui128_t) zeros))
	    {
	      return ((vui128_t) zeros);
	    }

	  u1 = vec_slq (u1, vn);
	  v1 = vec_slq (v1, vn);
#ifdef __DEBUG_PRINT__
	  print_vint128x ("         u1<<vn ", (vui128_t) u1);
	  print_vint128x ("         v1<<vn ", (vui128_t) v1);
#endif
	  //u1 = vec_srqi (u1, 1);
	  vdh = vec_mrgahd ((vui128_t) zeros, v1);
	  //udh = vec_mrgahd ((vui128_t) zeros, u1); // !!
	  qdh = vec_divqud_inline (u1, (vui64_t) v1);

	  q0 = vec_slqi ((vui128_t) qdh, 64);
#ifdef __DEBUG_PRINT__
	  print_v2xint64 ("   u1/v1h = qdh", qdh);
	  print_vint128x ("             q0 ", (vui128_t) q0);
#endif
	  // {k, k1}  = vec_muludq (v1, q0);
#if 1
	    {
	      vui128_t l128, h128;
	      vui64_t b_eud = vec_mrgald ((vui128_t) qdh, (vui128_t) qdh);
	      l128 = vec_vmuloud ((vui64_t) v1, b_eud);
	      h128 = vec_vmaddeud ((vui64_t) v1, b_eud, (vui64_t) l128);
	      // 192-bit product of v1 * q-estimate
	      k = h128;
	      k1 = vec_slqi (l128, 64);
	    }
#else
	  k1 = vec_muludq (&k, v1, q0);
#endif
	  u2 = vec_subuqm ((vui128_t) zeros, k1);
	  t = vec_subcuq ((vui128_t) zeros, k1);
	  u0 = vec_subeuqm (u1, k, t);
	  t2 = vec_subecuq (u1, k, t);
	  Bgt = vec_setb_ncq (t2);
#ifdef __DEBUG_PRINT__
	  print_vint128x (" hq(v1*q0) = k  ", (vui128_t) k);
	  print_vint128x (" lq(q0*v1) = k1 ", (vui128_t) k1);
	  print_vint128x ("     u1-k = u0  ", (vui128_t) u0);
	  print_vint128x ("     u1-k = u2  ", (vui128_t) u2);
	  print_vint128x ("     u1-k = t2  ", (vui128_t) t2);
	  print_vint128x ("     k>u1 = Bgt ", (vui128_t) Bgt);
#endif
	  u0 = vec_sldqi (u0, u2, 64);
	  //if (vec_cmpuq_all_gt (k, u1))
	    {
	      vui128_t q2;
	      q2 = (vui128_t) vec_subudm ((vui64_t) q0, ones);
	      q2 = (vui128_t) vec_mrgahd ((vui128_t) q2, (vui128_t) zeros);
	      //vdh = vec_mrgahd (v1, (vui128_t) zeros);
	      u2 = vec_adduqm ((vui128_t) u0, v1);
	      //t2 = vec_subuqm (u0, (vui128_t) vdh);
	      q0 = vec_seluq (q0, q2, Bgt);
	      u0 = vec_seluq (u0, u2, Bgt);
	      if (vec_cmpuq_all_eq ((vui128_t) Bgt, mone))
		{
#ifdef __DEBUG_PRINT__
		  print_vint128x (" hq(v1*q0) > u1 ", (vui128_t) u1);
		  print_vint128x ("     q0-1 = q0  ", (vui128_t) q0);
		  print_vint128x ("     u1-k'= u0  ", (vui128_t) u0);
		  print_vint128x ("     u1-k'= u2  ", (vui128_t) u2);
#endif
		}
	    }
	  qdh = (vui64_t) vec_mrgahd ((vui128_t) zeros, (vui128_t) q0);

	  //u0 = vec_sldqi (u0, u2, 64);
#ifdef __DEBUG_PRINT__
	  print_vint128x (" (u0|u2)<<64=u0 ", (vui128_t) u0);
	  print_vint128x ("   q0>>64 = qdh ", (vui128_t) qdh);
#endif
	  qdl = vec_divqud_inline (u0, (vui64_t) v1);
#ifdef __DEBUG_PRINT__
	    {
	      print_vint128x ("            u0  ", (vui128_t) u0);
	      print_v2xint64 ("            v1 ", (vui64_t) v1);
	      print_v2xint64 (" u0/v1[h] = qdl", (vui64_t) qdl);
	    }
#endif

	  printf (" qdl 192-bit mul\n");
	  u1 = u0;
	    {
	      vui128_t l128, h128;
	      vui64_t b_eud = vec_mrgald ((vui128_t) qdl, (vui128_t) qdl);
	      l128 = vec_vmuloud ((vui64_t) v1, b_eud);
	      h128 = vec_vmaddeud ((vui64_t) v1, b_eud, (vui64_t) l128);
	      // 192-bit product of v1 * qdl
	      k = h128;
	      k1 = vec_slqi (l128, 64);
	    }
	  u2 = vec_subuqm ((vui128_t) zeros, k1);
	  t = vec_subcuq ((vui128_t) zeros, k1);
	  u0 = vec_subeuqm (u1, k, t);
	  t2 = vec_subecuq (u1, k, t);
	  Bgt = vec_setb_ncq (t2);
#ifdef __DEBUG_PRINT__
	    {
	      print_vint128x ("            u1  ", (vui128_t) u1);
	      print_vint128x (" hq(v1*q0) = k  ", (vui128_t) k);
	      print_vint128x (" lq(v1*q0) = k1 ", (vui128_t) k1);
	      print_vint128x ("    'u1-k = u0  ", (vui128_t) u0);
	      print_vint128x ("    'u1-k1= u2  ", (vui128_t) u2);
	      print_vint128x ("    'u1-k = t2  ", (vui128_t) t2);
	      print_vint128x ("    'k>u1 = Bgt ", (vui128_t) Bgt);
	    }
#endif
	  u0 = vec_sldqi (u0, u2, 64);
	  q0 = (vui128_t) vec_mrgald ((vui128_t) qdh, (vui128_t) qdl);
#ifdef __DEBUG_PRINT__
	  print_vint128x (" (u0|u2)<<64=u0 ", (vui128_t) u0);
	  print_vint128x (" [qdh,qdl] = q0 ", (vui128_t) q0);
#endif
	  //if (vec_cmpuq_all_gt (k, u1))
	    {
	      vui128_t q2;
	      q2 = (vui128_t) vec_adduqm (q0, mone);
	      u2 = vec_adduqm ((vui128_t) u0, v1);
	      q0 = vec_seluq (q0, q2, Bgt);
	      u0 = vec_seluq (u0, u2, Bgt);
#ifdef __DEBUG_PRINT__
	      if (vec_cmpuq_all_eq ((vui128_t) Bgt, mone))
		{
		  print_vint128x (" lq(u0*ql) > u1 ", (vui128_t) u1);
		  print_vint128x ("     'q0-1 = q0 ", (vui128_t) q0);
		  print_vint128x ("     'u1-k'= u0 ", (vui128_t) u0);
		}
#endif
	    }
	  printf (" qdl end\n");
	  // Collect corrected remainder for sticky-bits.
	  // Test for nonzero remainder and round to odd
	  // Select from initial or corrected sticky bits
	  // Convert nonzero remainder into a carry (=1).
	  t2 = vec_addcuq (u0, mone);
	  q0 = (vui128_t) vec_or ((vui32_t) q0, (vui32_t) t2);
#ifdef __DEBUG_PRINT__
	  print_vint128x ("     u?!!=0 ->c ", (vui128_t) t2);
	  print_vint128x ("    q0''|c ->q0 ", (vui128_t) q0);
	  print_vint128x ("           q0'' ", (vui128_t) q0);
	  printf ("db_vec_divuqe end\n");
#endif
	  return q0;
	}
    }
#endif
}

__binary128
db_vec_xsdivqpo (__binary128 vfa, __binary128 vfb)
{
  __binary128 result;
#if defined (_ARCH_PWR9) && (__GNUC__ > 7)
#if defined (__FLOAT128__) && (__GNUC__ > 8)
  // earlier GCC versions generate extra data moves for this.
  result = __builtin_divf128_round_to_odd (vfa, vfb);
#else
  // No extra data moves here.
  __asm__(
      "xsdivqpo %0,%1,%2"
      : "=v" (result)
      : "v" (vfa), "v" (vfb)
      : );
#endif
#elif  defined (_ARCH_PWR8)
  vui64_t q_exp, a_exp, b_exp, x_exp;
  vui128_t q_sig, a_sig, b_sig, p_sig_h, p_sig_l, p_odd;
  vui32_t q_sign,  a_sign,  b_sign;
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  const vui64_t q_zero = { 0, 0 };
  const vui64_t q_ones = { -1, -1 };
  const vui64_t exp_bias = (vui64_t) CONST_VINT64_DW( 0x3fff, 0x3fff );
  const vi64_t exp_min = (vi64_t) CONST_VINT64_DW( 1, 1 );
  const vui64_t exp_dnrm = (vui64_t) CONST_VINT64_DW( 0, 0 );
  const vui64_t q_expnaninf = (vui64_t) CONST_VINT64_DW( 0x7fff, 0x7fff );
  const vui64_t q_expmax = (vui64_t) CONST_VINT64_DW( 0x7ffe, 0x7ffe );
  const vui32_t sigov = CONST_VINT128_W(0x0001ffff, -1, -1, -1);

  print_vfloat128x("db_vec_xsdivqpo vfa=  ", vfa);
  print_vfloat128x("                vfb=  ", vfb);

  a_exp = vec_xsxexpqp (vfa);
  a_sig = vec_xsxsigqp (vfa);
  a_sign = vec_and_bin128_2_vui32t (vfa, signmask);
  print_vint128x (" sign(vfa):", (vui128_t) a_sign);
  print_vint128x (" exp (vfa):", (vui128_t) a_exp);
  print_vint128x (" sig (vfa):", (vui128_t) a_sig);
  b_exp = vec_xsxexpqp (vfb);
  b_sig = vec_xsxsigqp (vfb);
  b_sign = vec_and_bin128_2_vui32t (vfb, signmask);
  print_vint128x (" sign(vfb):", (vui128_t) b_sign);
  print_vint128x (" exp (vfb):", (vui128_t) b_exp);
  print_vint128x (" sig (vfb):", (vui128_t) b_sig);
  x_exp = vec_mrgahd ((vui128_t) a_exp, (vui128_t) b_exp);
  q_sign = vec_xor (a_sign, b_sign);

//  if (vec_all_isfinitef128 (vfa) && vec_all_isfinitef128 (vfb))
//  The above can be optimized to the following
  if (vec_cmpud_all_lt (x_exp, q_expnaninf))
    {
      const vui32_t sigovt = CONST_VINT128_W(0x0000ffff, -1, -1, -1);
      if (vec_cmpud_any_eq (x_exp, exp_dnrm))
	{ // Involves zeros or denormals
	  print_vint128x (" exps(x_exp): ", (vui128_t) x_exp);
	  // check for zero significands in Divide
	  if (vec_cmpuq_all_eq (b_sig, (vui128_t) q_zero))
	    { // Divide by zero, return QP Infinity OR QNAN
	      if (vec_cmpuq_all_eq (a_sig, (vui128_t) q_zero))
		{ // Divide by zero, return QP Infinity
		  result = vec_const_nanf128 ();
		}
	      else
		{ // Divide by zero, return QP Infinity
		  q_sign = vec_xor (a_sign, b_sign);
		  q_exp = q_expnaninf;
		  q_sig = (vui128_t) q_zero;
		  q_sig = (vui128_t) vec_or ((vui32_t) q_sig, q_sign);
		  result = vec_xsiexpqp (q_sig, q_exp);
		}
	      print_vint128x (" Div zero (q_sign):", (vui128_t) q_sign);
	      return result;
	    }
	  else if (vec_cmpuq_all_eq (a_sig, (vui128_t) q_zero))
	    { // Multiply by zero, return QP signed zero
	      print_vint128x (" Zero Div (q_sign):", (vui128_t) q_sign);
	      result = vec_xfer_vui32t_2_bin128 (q_sign);
	      return result;
	    }
	  else
	    {
	      // need to Normalize Denormals before divide
	      vui128_t a_tmp, b_tmp;
	      vui64_t a_adj = {0, 0};
	      vui64_t b_adj = {0, 0};
	      vui64_t x_adj;

	      vb64_t exp_mask;
	      exp_mask = vec_cmpequd (x_exp, exp_dnrm);
	      x_exp = (vui64_t) vec_sel (x_exp, (vui64_t) exp_min, exp_mask);
	      print_vint128x (" adj (x_exp): ", (vui128_t) x_exp);

	      if  (vec_cmpud_all_eq (a_exp, exp_dnrm))
		{
		  a_tmp = vec_slqi (a_sig, 15);
		  a_adj = (vui64_t) vec_clzq (a_tmp);
		  a_sig = vec_slq (a_sig, (vui128_t) a_adj);
		  print_vint128x (" slqi(a_exp): ", (vui128_t) a_tmp);
		  print_vint128x (" clzq(a_exp): ", (vui128_t) a_adj);
		  print_vint128x (" slq (clz_a): ", (vui128_t) a_sig);
		}
	      if  (vec_cmpud_all_eq (b_exp, exp_dnrm))
		{
		  b_tmp = vec_slqi (b_sig, 15);
		  b_adj = (vui64_t) vec_clzq (b_tmp);
		  b_sig = vec_slq (b_sig, (vui128_t) b_adj);
		  print_vint128x (" slqi(b_exp): ", (vui128_t) b_tmp);
		  print_vint128x (" clzq(b_exp): ", (vui128_t) b_adj);
		  print_vint128x (" slq (clz_b): ", (vui128_t) b_sig);
		}
	      x_adj = vec_mrgald ((vui128_t) a_adj, (vui128_t) b_adj);
	      x_exp = vec_subudm (x_exp, x_adj);
	      print_vint128x (" exp (adj  ): ", (vui128_t) x_adj);
	      print_vint128x (" exp (x_exp): ", (vui128_t) x_exp);
	      a_exp = vec_splatd (x_exp, VEC_DW_H);
	      b_exp = vec_splatd (x_exp, VEC_DW_L);
	    }
	}
      else
	{
	  a_exp = vec_splatd (a_exp, VEC_DW_H);
	  b_exp = vec_splatd (b_exp, VEC_DW_H);
	}
      // Precondition the significands before multiply so that the
      // high-order 114-bits (C,L,FRACTION) of the product are right
      // adjusted in p_sig_h. And the low-order 112-bits are left
      // justified in p_sig_l.
      // Using Divide extended we are effective performing a 256-bit
      // by 128-bit divide.
      b_sig = vec_slqi (b_sig, 8);
#if 0 //defined (_ARCH_PWR9)
      p_sig_l = vec_diveuqo_inline (a_sig, b_sig);
#else
      p_sig_l = db_vec_diveuqo (a_sig, b_sig);
#endif
      print_vint128x (" sig (slqi):", (vui128_t) b_sig);
      print_vint128x (" sig (dive):", (vui128_t) p_sig_l);
      p_sig_h = (vui128_t) vec_sld ((vui8_t) q_zero, (vui8_t) p_sig_l, 15);
      p_sig_l = (vui128_t) vec_sld ((vui8_t) p_sig_l, (vui8_t) q_zero, 15);
      print_vint128x (" sig (p_h):", (vui128_t) p_sig_h);
      print_vint128x (" sig (p_l):", (vui128_t) p_sig_l);
      // sum exponents
      q_exp = vec_subudm (a_exp, b_exp);
      print_vint128x (" exp (a-b):", (vui128_t) q_exp);
      q_exp = vec_addudm (q_exp, exp_bias);
      print_vint128x (" exp (q_exp):", (vui128_t) q_exp);
#if 0
      // Check for carry and adjust
      if (vec_cmpuq_all_gt (p_sig_h, (vui128_t) sigov))
      	{
      	  print_vint128x (" OV  (p_sig):", (vui128_t) p_sig_h);
      	  p_tmp = vec_sldqi (p_sig_h, p_sig_l, 120);
      	  p_sig_h = vec_srqi (p_sig_h, 1);
      	  p_sig_l = vec_slqi (p_tmp, 7);
      	  print_vint128x (" sig (p_h):", (vui128_t) p_sig_h);
      	  print_vint128x (" sig (p_l):", (vui128_t) p_sig_l);
      	  q_exp = vec_addudm (q_exp, q_one);
      	  print_vint128x (" adj (q_exp):", (vui128_t) q_exp);
      	}
      else if (vec_cmpuq_all_le (p_sig_h, (vui128_t) sigovt))
      	{
      	  print_vint128x (" OVT (p_sig):", (vui128_t) p_sig_h);
      	}
#endif

      // There are two cases for denormal
      // 1) The sum of unbiased exponents is less than E_min (tiny).
      // 2) The significand is less than 1.0 (C and L-bits are zero).
      //  2a) The exponent is greater than E_min
      //  2b) The exponent is equal to E_min
      //
      if (vec_cmpsd_all_lt ((vi64_t) q_exp, exp_min))
	{
	  print_vint128x (" tiny (q_exp):", (vui128_t) q_exp);
	  {
	    const vui64_t exp_tinyer = (vui64_t) CONST_VINT64_DW( 116, 116 );
	    const vui32_t xmask = CONST_VINT128_W(0x1fffffff, -1, -1, -1);
	    vui32_t tmp;
	    // Intermediate result is tiny, unbiased exponent < -16382
	    x_exp = vec_subudm ((vui64_t) exp_min, q_exp);

	    print_vint128x (" tiny (x_exp):", (vui128_t) x_exp);

	    if  (vec_cmpud_all_gt ((vui64_t) x_exp, exp_tinyer))
	      {
		// Intermediate result is too tiny, the shift will
		// zero the fraction and the GR-bit leaving only the
		// Sticky bit. The X-bit needs to include all bits
		// from p_sig_h and p_sig_l
		p_sig_l = vec_srqi (p_sig_l, 16);
		p_sig_l = (vui128_t) vec_or ((vui32_t) p_sig_l, (vui32_t) p_sig_h);
		print_vint128x (" sig (h|l):", (vui128_t) p_sig_l);
		// generate a carry into bit-2 for any nonzero bits 3-127
		p_sig_l = vec_adduqm (p_sig_l, (vui128_t) xmask);
		q_sig = (vui128_t) q_zero;
		p_sig_l = (vui128_t) vec_andc ((vui32_t) p_sig_l, xmask);
		print_vint128x (" sig (pxl):", (vui128_t) p_sig_l);
	      }
	    else
	      { // Normal tiny, right shift may lose low order bits
		// from p_sig_l. So collect any 1-bits below GRX and
		// OR them into the X-bit, before the right shift.
		vui64_t l_exp;
		const vui64_t exp_128 = (vui64_t) CONST_VINT64_DW( 128, 128 );
		// Propagate low order bits into the sticky bit
		// GRX left adjusted in p_sig_l
		// Isolate bits below GDX (bits 3-128).
		tmp = vec_and ((vui32_t) p_sig_l, xmask);
		// generate a carry into bit-2 for any nonzero bits 3-127
		tmp = (vui32_t) vec_adduqm ((vui128_t) tmp, (vui128_t) xmask);
		// Or this with the X-bit to propagate any sticky bits into X
		p_sig_l = (vui128_t) vec_or ((vui32_t) p_sig_l, tmp);
		p_sig_l = (vui128_t) vec_andc ((vui32_t) p_sig_l, xmask);
		print_vint128x (" sig (pxl):", (vui128_t) p_sig_l);

		l_exp = vec_subudm (exp_128, x_exp);

		print_vint128x (" tiny (l_exp):", (vui128_t) l_exp);
		p_sig_l = vec_sldq (p_sig_h, p_sig_l, (vui128_t) l_exp);
		p_sig_h = vec_srq (p_sig_h, (vui128_t) x_exp);
		print_vint128x (" srq (p_h):", (vui128_t) p_sig_h);
		print_vint128x (" srq (p_l):", (vui128_t) p_sig_l);
		q_sig = p_sig_h;
	      }
	    q_exp = q_zero;
	  }
	}
      else
	{
	  // Exponent is not tiny.
	  if (vec_cmpuq_all_le (p_sig_h, (vui128_t) sigovt))
	    {
	      // But the significand is below normal range.
	      // This can happen when multiplying a denormal by a
	      // normal.
	      const vui64_t exp_15 = { 15, 15 };
	      vui64_t c_exp, d_exp;
	      vui128_t c_sig;
	      print_vint128x (" UFt (p_sig):", (vui128_t) p_sig_h);
	      c_sig = vec_clzq (p_sig_h);
	      c_exp = vec_splatd ((vui64_t) c_sig, VEC_DW_L);
	      c_exp = vec_subudm (c_exp, exp_15);
	      print_vint128x (" clz (p_sig):", (vui128_t) c_exp);
	      d_exp = vec_subudm (q_exp, (vui64_t) exp_min);
	      print_vint128x (" exp (exp-1):", (vui128_t) d_exp);
	      d_exp = vec_minud (c_exp, d_exp);
	      print_vint128x (" exp (min-d):", (vui128_t) d_exp);

	      if (vec_cmpsd_all_gt ((vi64_t) q_exp, exp_min))
		{
		  p_sig_h = vec_sldq (p_sig_h, p_sig_l, (vui128_t) d_exp);
		  p_sig_l = vec_slq (p_sig_l, (vui128_t) d_exp);
		  print_vint128x (" sig (p_h):", (vui128_t) p_sig_h);
		  print_vint128x (" sig (p_l):", (vui128_t) p_sig_l);
		  if (vec_cmpud_all_le (q_exp, c_exp))
		    {
		      // Intermediate result == tiny, unbiased exponent == -16382
		      // Check if sig is denormal range (L-bit is 0).
		      q_exp = q_zero;
		      print_vint128x (" exp (q<=c):", (vui128_t) q_exp);
		    }
		  else
		    q_exp = vec_subudm (q_exp, d_exp);

		  print_vint128x (" adj (q_exp):", (vui128_t) q_exp);
		}
	      else
		{
		  // Intermediate result == tiny, unbiased exponent == -16382
		  // sig is denormal range (L-bit is 0).
		  print_vint128x (" UFt (p_sig):", (vui128_t) p_sig_h);
		  q_exp = q_zero;
		  print_vint128x (" adj (q_exp):", (vui128_t) q_exp);
		}
	    }
	  q_sig = p_sig_h;
	}

      // Round to odd from lower product bits
      p_odd = vec_addcuq (p_sig_l, (vui128_t) q_ones);
      q_sig = (vui128_t)  vec_or ((vui32_t) q_sig, (vui32_t) p_odd);

      // Check for exponent overflow -> __FLT128_INF__
      if  (vec_cmpud_all_gt ( q_exp, q_expmax))
      {
	// Intermediate result is huge, unbiased exponent > 16383
	print_vint128x (" OV  (q_exp):", (vui128_t) q_exp);
	q_exp = q_expmax;
	q_sig = (vui128_t) sigov;
      }
    }
  else
    { // One or both operands are NaN or Infinity
      if (vec_cmpuq_all_eq (a_sig, (vui128_t) q_zero)
	  && vec_cmpuq_all_eq (b_sig, (vui128_t) q_zero))
	{
	  // Both operands either infinity or zero
	  if (vec_cmpud_any_eq (x_exp, q_zero))
	    {
	      // Inifinty / Zero is Inifinty
	      q_sign = vec_xor (a_sign, b_sign);
	      q_exp = a_exp;
	      q_sig = a_sig;
	    }
	  else
	    {
	      // Infinity / Infinity == Quiet NAN
	      return vec_const_nanf128 ();
	    }
	}
      else
	{
	  // One or both operands are NaN
	  const vui32_t q_nan = CONST_VINT128_W(0x00008000, 0, 0, 0);
	  if (vec_all_isnanf128 (vfa))
	    {
	      // vfa is NaN
	      q_sign = a_sign;
	      q_sig = (vui128_t) vec_or ((vui32_t) a_sig, q_nan);
	      q_exp = a_exp;
	    }
	  else if (vec_all_isnanf128 (vfb))
	    {
	      // vfb is NaN
	      q_sign = b_sign;
	      q_sig = (vui128_t) vec_or ((vui32_t) b_sig, q_nan);
	      q_exp = b_exp;
	    }
	  else  // OR an Infinity and a Nonzero finite number
	    {
	      if (vec_cmpuq_all_eq (a_sig, (vui128_t) q_zero))
		{ // vfa is a Infinity, return signed Infinity
		  q_sign = vec_xor (a_sign, b_sign);
		  q_exp = q_expnaninf;
		  q_sig = (vui128_t) q_zero;
		}
	      else
		{  // vfb is a Infinity, return signed zero
		  q_sign = vec_xor (a_sign, b_sign);
		  q_exp = (vui64_t) {0, 0};
		  q_sig = (vui128_t) q_zero;
		}
	    }
	}
    }
  // Merge sign, significand, and exponent into final result
  q_sig = (vui128_t) vec_or ((vui32_t) q_sig, q_sign);
  result = vec_xsiexpqp (q_sig, q_exp);
#else // ! _ARCH_PWR8, use libgcc soft-float
  result = vfa / vfb;
#endif
  return result;
}
#endif

//#define __DEBUG_PRINT__ 1
//#define __DEBUG_PRINT__ 2
#ifdef __DEBUG_PRINT__
#if (__DEBUG_PRINT__ == 2)
#define test_xsaddqpo(_l,_k)	db_vec_xsaddqpo(_l,_k)
#else
// Test implementation from vec_f128_dummy.c
extern __binary128 test_vec_addqpo (__binary128 vfa, __binary128 vfb);
#define test_xsaddqpo(_l,_k)	test_vec_addqpo(_l,_k)
#endif
#else
#if 1
// Test implementation from libpvecstatic
extern __binary128 __VEC_PWR_IMP (vec_xsaddqpo) (__binary128 vfa, __binary128 vfb);
#define test_xsaddqpo(_l,_k)	__VEC_PWR_IMP (vec_xsaddqpo)(_l,_k)
#else
// Test implementation expanded from vec_f128_ppc.h
extern __binary128 test_vec_xsaddqpo (__binary128 vfa, __binary128 vfb);
#define test_xsaddqpo(_l,_k)	test_vec_xsaddqpo(_l,_k)
#endif
#endif

int
test_add_qpo (void)
{
  __binary128 x, y;
  __binary128 t, e;
  vui64_t xui;
  int rc = 0;
  printf ("\n%s\n", __FUNCTION__);

  x = vec_xfer_vui64t_2_bin128 (vf128_one);
  y = vec_xfer_vui64t_2_bin128 (vf128_ntwo);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);
  e = vec_xfer_vui64t_2_bin128 (vf128_none);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 (vf128_zero);
  y = vec_xfer_vui64t_2_bin128 (vf128_zero);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);
  e = vec_xfer_vui64t_2_bin128 (vf128_zero);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 (vf128_zero);
  y = vec_xfer_vui64t_2_bin128 (vf128_one);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);
  e = vec_xfer_vui64t_2_bin128 (vf128_one);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 (vf128_one);
  y = vec_xfer_vui64t_2_bin128 (vf128_one);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);
  e = vec_xfer_vui64t_2_bin128 (vf128_two);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  //  x = 1.0Q;
  xui = CONST_VINT128_DW(0x3fff000000000000, 1);
  x = vec_xfer_vui64t_2_bin128 (xui);
  //  y = 0.125Q;
  xui = CONST_VINT128_DW(0x3ffc000000000000, 1);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);

  //  e = 1.175Q;
  xui = CONST_VINT128_DW(0x3fff200000000000, 1);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  //  x = 1.0Q;
  xui = CONST_VINT128_DW(0x3fff000000000000, 1);
  x = vec_xfer_vui64t_2_bin128 (xui);
  //  y = -0.125Q;
  xui = CONST_VINT128_DW(0xbffc000000000000, 1);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);

  //  e = 0.875Q;
  xui = CONST_VINT128_DW(0x3ffec00000000000, 1);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  //  x = 1.0Q;
  xui = CONST_VINT128_DW(0x3fff000000000000, 0);
  x = vec_xfer_vui64t_2_bin128 (xui);
  //  y = -0.125Q;
  xui = CONST_VINT128_DW(0xbffc000000000000, 1);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);

  //  e = 0.875Q;
  xui = CONST_VINT128_DW(0x3ffebfffffffffff, -1);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

//  x = 3.0Q;
  xui = CONST_VINT128_DW(0x4000800000000000, 0);
  x = vec_xfer_vui64t_2_bin128 (xui);
//  y = 3.0Q;
  xui = CONST_VINT128_DW(0x4000800000000000, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);

//  e = 6.0Q;
  xui = CONST_VINT128_DW(0x4001800000000000, 0);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  //  x = 3.0Q;
  xui = CONST_VINT128_DW(0x4000800000000000, 0);
  x = vec_xfer_vui64t_2_bin128 (xui);
  y = vec_xfer_vui64t_2_bin128 (vf128_none);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);

  //  e = 6.0Q;
  xui = CONST_VINT128_DW(0x4000000000000000, 0);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  // x = +1.c000000000000000000000000000p-16381Q;
  xui = CONST_VINT128_DW(0x0002400000000000, 0);
  x = vec_xfer_vui64t_2_bin128 (xui);
  // y = -1.8000000000000000000000000000p-16381Q
  xui = CONST_VINT128_DW(0x8002000000000000, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);

  //  e = 0.8000000000000000000000000000p-16382Q;
  xui = CONST_VINT128_DW(0x0000800000000000, 0);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  // x = +1.c000000000000000000000000000p-16380Q;
  xui = CONST_VINT128_DW(0x0003400000000000, 0);
  x = vec_xfer_vui64t_2_bin128 (xui);
  // y = -1.8000000000000000000000000000p-16380Q
  xui = CONST_VINT128_DW(0x8003000000000000, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);

  //  e = 1.000000000000000000000000000p-16381Q;
  xui = CONST_VINT128_DW(0x0001000000000000, 0);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  //  x = +1.c000000000000000000000000000p-16382Q;
  xui = CONST_VINT128_DW(0x0001c00000000000, 0);
  x = vec_xfer_vui64t_2_bin128 (xui);
  // y = -1.8000000000000000000000000000p-16382Q
  xui = CONST_VINT128_DW(0x8001800000000000, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);

  //  e = 0.4000000000000000000000000000p-16382Q;
  xui = CONST_VINT128_DW(0x0000400000000000, 0);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  //  x = +1.c000000000000000000000000000p-16381Q;
  xui = CONST_VINT128_DW(0x0002c00000000000, 0);
  x = vec_xfer_vui64t_2_bin128 (xui);
  // y = -1.8000000000000000000000000000p-16381Q
  xui = CONST_VINT128_DW(0x8002800000000000, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);

  //  e = 0.8000000000000000000000000000p-16382Q;
  xui = CONST_VINT128_DW(0x0000800000000000, 0);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  //  x = 3.0Q;
  xui = CONST_VINT128_DW(0x4000800000000000, 0);
  x = vec_xfer_vui64t_2_bin128 (xui);
  y = vec_xfer_vui64t_2_bin128 (vf128_ntwo);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);

  //  e = 1.0Q;
  e = vec_xfer_vui64t_2_bin128 (vf128_one);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  //  x = 1.0Q;
  x = vec_xfer_vui64t_2_bin128 (vf128_one);
  // y = 3.0p-113Q;
  xui = CONST_VINT128_DW(0x3f8e800000000000, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);

  //  e = 1.0Q odd;
  xui = CONST_VINT128_DW(0x3fff000000000000, 1);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  //  x = 1.0Q;
  x = vec_xfer_vui64t_2_bin128 (vf128_one);
  // y = -3.0p-113Q;
  xui = CONST_VINT128_DW(0xbf8e800000000000, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);

  //  e = 1.fffffQ odd;
  xui = CONST_VINT128_DW(0x3ffeffffffffffff, -1);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  //  x = 1.0Q;
  x = vec_xfer_vui64t_2_bin128 (vf128_one);
  // y = 3.0p-115Q;
  xui = CONST_VINT128_DW(0x3f8c800000000000, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);

  //  e = 1.0Q odd;
  xui = CONST_VINT128_DW(0x3fff000000000000, 1);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  //  x = 1.0Q;
  x = vec_xfer_vui64t_2_bin128 (vf128_one);
  // y = -3.0p-115Q;
  xui = CONST_VINT128_DW(0xbf8c800000000000, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);

  //  e = 1.fffffQ odd;
  xui = CONST_VINT128_DW(0x3ffeffffffffffff, -1);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  //  x = 1.0Q;
  x = vec_xfer_vui64t_2_bin128 (vf128_one);
  // y = 3.0p-117Q;
  xui = CONST_VINT128_DW(0x3f8a800000000000, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);

  //  e = 1.0Q odd;
  xui = CONST_VINT128_DW(0x3fff000000000000, 1);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  //  x = 1.0Q;
  x = vec_xfer_vui64t_2_bin128 (vf128_one);
  // y = -3.0p-117Q;
  xui = CONST_VINT128_DW(0xbf8a800000000000, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);

  //  e = 1.fffffQ odd;
  xui = CONST_VINT128_DW(0x3ffeffffffffffff, -1);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 (vf128_sub);
  y = vec_xfer_vui64t_2_bin128 (vf128_sub);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);
  xui = CONST_VINT128_DW(0x0001ffffffffffff, 0xfffffffffffffffe);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 (vf128_sub);
  y = vec_xfer_vui64t_2_bin128 (vf128_nsub);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);
  e = vec_xfer_vui64t_2_bin128 (vf128_zero);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  // y = 1.0p-16382Q;
  xui = CONST_VINT128_DW(0, 1);
  x = vec_xfer_vui64t_2_bin128 (xui);
  // y = 1.0pp-16382Q;
  xui = CONST_VINT128_DW(0, 1);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);
  xui = CONST_VINT128_DW(0x0, 2);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  // x = 0.ffffffp-16382QQ;
  x = vec_xfer_vui64t_2_bin128 (vf128_sub);
  // y = 0.0000001p-16382Q;
  xui = CONST_VINT128_DW(0, 1);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);

  //  e = 1.fffffQ odd;
  xui = CONST_VINT128_DW(0x0001000000000000, 0);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  // x = 0.ffffffp-16382QQ;
  x = vec_xfer_vui64t_2_bin128 (vf128_sub);
  // y = -0.0000001p-16382Q;
  xui = CONST_VINT128_DW(0x8000000000000000, 1);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);

  //  e = 0.fffff...eQ;
  xui = CONST_VINT128_DW(0x0000ffffffffffff, 0xfffffffffffffffe);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  //  x = 1.875Q;
  xui = CONST_VINT128_DW(0x3fffe00000000000, 0);
  x = vec_xfer_vui64t_2_bin128 (xui);
  // y = 1.0p-3Q;
  xui = CONST_VINT128_DW(0x3ffc000000000000, 1);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);

  //  e = 2.0000Q odd;
  xui = CONST_VINT128_DW(0x4000000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  //  x = 1.875Q;
  xui = CONST_VINT128_DW(0x3fffe00000000000, 0);
  x = vec_xfer_vui64t_2_bin128 (xui);
  // y = -1.0p-3Q;
  xui = CONST_VINT128_DW(0xbffc000000000000, 1);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);

  //  e = 1.7421875....Q odd;
  xui = CONST_VINT128_DW(0x3fffbfffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  //  x = 1.0Q;
  x = vec_xfer_vui64t_2_bin128 (vf128_one);
  // y = -1.0p-3Q;
  xui = CONST_VINT128_DW(0xbffc000000000000, 1);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);

  //  e = 1.7421875....Q odd;
  xui = CONST_VINT128_DW(0x3ffebfffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  //  x = 1.9999Q;
  xui = CONST_VINT128_DW(0x3fffffffffffffff, 0xffffffffffffffff);
  x = vec_xfer_vui64t_2_bin128 (xui);
  // y = -1.9998Q;
  xui = CONST_VINT128_DW(0xbfffffffffffffff, 0xfffffffffffffffe);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);

  //  e = 1.7421875....Q odd;
  xui = CONST_VINT128_DW(0x3f8f000000000000, 0x0000000000000000);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 (vf128_max);
  y = vec_xfer_vui64t_2_bin128 (vf128_max);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);
  // e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  e = vec_xfer_vui64t_2_bin128 (vf128_max);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 (vf128_nmax);
  y = vec_xfer_vui64t_2_bin128 (vf128_nmax);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsaddqpo(x, y);
  // e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  e = vec_xfer_vui64t_2_bin128 (vf128_nmax);
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  return (rc);
}

int
test_add_qpo_xtra (void)
{
  __binary128 x, y;
  __binary128 t, e;
  vui64_t xui;
  int rc = 0;
  printf ("\n%s\n", __FUNCTION__);

  // PowerISA 3.1 Table 65 Actions for xsaddqp[o]
  // Line -infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  // e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  e = y;
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  // Line -NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ntwo );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  // Line -zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  // Line +zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  // Line +NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_two );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  // Line +infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  // Line QNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  // Line SNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_snan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsaddqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsaddqpo", e, t, e);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
//#define __DEBUG_PRINT__ 2
#ifdef __DEBUG_PRINT__
#if (__DEBUG_PRINT__ == 2)
#define test_xsdivqpo(_l,_k)	db_vec_xsdivqpo(_l,_k)
#else
// Test implementation from vec_f128_dummy.c
extern __binary128 test_vec_divqpo (__binary128 vfa, __binary128 vfb);
#define test_xsdivqpo(_l,_k)	test_vec_divqpo(_l,_k)
#endif
#else
#if 1
// Test implementation from libpvecstatic
extern __binary128 __VEC_PWR_IMP (vec_xsdivqpo) (__binary128 vfa, __binary128 vfb);
#define test_xsdivqpo(_l,_k)	__VEC_PWR_IMP (vec_xsdivqpo)(_l,_k)
#else
// Test implementation expanded from vec_f128_ppc.h
extern __binary128 test_vec_xsdivqpo (__binary128 vfa, __binary128 vfb);
#define test_xsdivqpo(_l,_k)	test_vec_xsdivqpo(_l,_k)
#endif
#endif

int
test_div_qpo (void)
{
  __binary128 x, y;
  __binary128 t, e;
  vui64_t xui;
  int rc = 0;
  printf ("\n%s\n", __FUNCTION__);

  x = vec_xfer_vui64t_2_bin128 (vf128_zero);
  y = vec_xfer_vui64t_2_bin128 (vf128_zero);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsdivqpo(x, y);
  e = vec_xfer_vui64t_2_bin128 (vf128_nan);
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 (vf128_zero);
  y = vec_xfer_vui64t_2_bin128 (vf128_one);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsdivqpo(x, y);
  e = vec_xfer_vui64t_2_bin128 (vf128_zero);
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 (vf128_one);
  y = vec_xfer_vui64t_2_bin128 (vf128_one);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsdivqpo(x, y);
  e = vec_xfer_vui64t_2_bin128 (vf128_one);
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  //  x = 1.ffffffffffffffffffp-0Q;
    xui = CONST_VINT128_DW(0x3fffffffffffffff, 0);
    x = vec_xfer_vui64t_2_bin128 (xui);
  //  y = 1.fffffffffff0000000p-0Q;
    xui = CONST_VINT128_DW(0x3fffffffffffffff, 0xffffffffffffffff);
    y = vec_xfer_vui64t_2_bin128 (xui);

  #ifdef __DEBUG_PRINT__
    print_vfloat128x (" x=  ", x);
    print_vfloat128x (" y=  ", y);
  #endif
    t = test_xsdivqpo(x, y);
    // is:                   3ffeffffffffffff0000000000000001
    xui = CONST_VINT128_DW(0x3ffeffffffffffff, 0x0000000000000001);
    e = vec_xfer_vui64t_2_bin128 (xui);
    rc += check_f128 ("check vec_xsdivqpo", e, t, e);

    //  x = 1.fffffffffff0000000p-0Q;
      xui = CONST_VINT128_DW(0x3fffffffffffffff, 0xffffffffffffffff);
      x = vec_xfer_vui64t_2_bin128 (xui);
    //  y = 1.ffffffffffffffffffp-0Q;
      xui = CONST_VINT128_DW(0x3fffffffffffffff, 0);
      y = vec_xfer_vui64t_2_bin128 (xui);

    #ifdef __DEBUG_PRINT__
      print_vfloat128x (" x=  ", x);
      print_vfloat128x (" y=  ", y);
    #endif
      t = test_xsdivqpo(x, y);
      // is:                   3fff0000000000008000000000003fff
      xui = CONST_VINT128_DW(0x3fff000000000000, 0x8000000000003fff);
      e = vec_xfer_vui64t_2_bin128 (xui);
      rc += check_f128 ("check vec_xsdivqpo", e, t, e);

      //  x = 1.0Q;
      x = vec_xfer_vui64t_2_bin128 (vf128_one);
      //  y = 1.ffffffffffffffffffp-0Q;
        xui = CONST_VINT128_DW(0x3fffffffffffffff, 0);
        y = vec_xfer_vui64t_2_bin128 (xui);

      #ifdef __DEBUG_PRINT__
        print_vfloat128x (" x=  ", x);
        print_vfloat128x (" y=  ", y);
      #endif
        t = test_xsdivqpo(x, y);
        // is: 3ffe0000000000008000000000004001
        xui = CONST_VINT128_DW(0x3ffe000000000000, 0x8000000000004001);
        e = vec_xfer_vui64t_2_bin128 (xui);
        rc += check_f128 ("check vec_xsdivqpo", e, t, e);

//  x = 3.0Q;
  xui = CONST_VINT128_DW(0x4000800000000000, 0);
  x = vec_xfer_vui64t_2_bin128 (xui);
//  y = 3.0Q;
  xui = CONST_VINT128_DW(0x4000800000000000, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsdivqpo(x, y);
  e = vec_xfer_vui64t_2_bin128 (vf128_one);
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  //  x = 1.0Q;
  x = vec_xfer_vui64t_2_bin128 (vf128_one);
  //  y = 3.0Q;
  xui = CONST_VINT128_DW(0x4000800000000000, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsdivqpo(x, y);
  //  e = 1.0Q / 3.0Q;
  xui = CONST_VINT128_DW(0x3ffd555555555555, 0x5555555555555555);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  //  x = 1.0Q;
  x = vec_xfer_vui64t_2_bin128 (vf128_one);
  //  y = 1.0Q / 3.0Q;
  xui = CONST_VINT128_DW(0x3ffd555555555555, 0x5555555555555555);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsdivqpo(x, y);
  //  e = 3.0Q;
  xui = CONST_VINT128_DW(0x4000800000000000, 1);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  //  x = 1.0Q;
  x = vec_xfer_vui64t_2_bin128 (vf128_one);
  //  y = 9.0Q;
  xui = CONST_VINT128_DW(0x4002200000000000, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsdivqpo(x, y);
  //  e = 1.0Q / 9.0Q;
  xui = CONST_VINT128_DW(0x3ffbc71c71c71c71, 0xc71c71c71c71c71d);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  //  x = 1.0Q;
  x = vec_xfer_vui64t_2_bin128 (vf128_one);
  //  y = 1.0Q / 9.0Q;
  xui = CONST_VINT128_DW(0x3ffbc71c71c71c71, 0xc71c71c71c71c71c);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsdivqpo(x, y);

  //  e = 9.0Q;
  xui = CONST_VINT128_DW(0x4002200000000000, 1);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  //  x = 1.0Q;
  x = vec_xfer_vui64t_2_bin128 (vf128_one);
  //  y = 10.0Q;
  xui = CONST_VINT128_DW(0x4002400000000000, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsdivqpo(x, y);
  xui = CONST_VINT128_DW(0x3ffb999999999999, 0x9999999999999999);

  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  //  x = 1.0Q;
  x = vec_xfer_vui64t_2_bin128 (vf128_one);
  //  y = 257.0Q;
  xui = CONST_VINT128_DW(0x4007010000000000, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsdivqpo(x, y);
  // is:                   3ff6fe01fe01fe01fe01fe01fe01fe01
  xui = CONST_VINT128_DW(0x3ff6fe01fe01fe01, 0xfe01fe01fe01fe01);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  //  x = 1.0p-8192Q;
  xui = CONST_VINT128_DW(0x1fff000000000000, 0);
  x = vec_xfer_vui64t_2_bin128 (xui);
  //  y = 1.0p-8192Q;
  xui = CONST_VINT128_DW(0x1fff000000000000, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsdivqpo(x, y);
  e = vec_xfer_vui64t_2_bin128 (vf128_one);
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  //  x = 1.0p-16382Q;
  xui = CONST_VINT128_DW(0x0001000000000000, 0);
  x = vec_xfer_vui64t_2_bin128 (xui);
  //  y = 1.0p-16382Q;
  xui = CONST_VINT128_DW(0x0001000000000000, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsdivqpo(x, y);
  e = vec_xfer_vui64t_2_bin128 (vf128_one);
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  //  x = 1.0p-16383Q;
  xui = CONST_VINT128_DW(0x0000800000000000, 0);
  x = vec_xfer_vui64t_2_bin128 (xui);
  //  y = 1.0p-16383Q;
  xui = CONST_VINT128_DW(0x0000800000000000, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsdivqpo(x, y);
  e = vec_xfer_vui64t_2_bin128 (vf128_one);
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  //  x = 1.0p-16494Q;
  xui = CONST_VINT128_DW(0, 1);
  x = vec_xfer_vui64t_2_bin128 (xui);
  //  y = 1.0p-16494Q;
  xui = CONST_VINT128_DW(0, 1);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsdivqpo(x, y);
  e = vec_xfer_vui64t_2_bin128 (vf128_one);
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  //  x = 1.0p-16494Q;
  xui = CONST_VINT128_DW(0, 1);
  x = vec_xfer_vui64t_2_bin128 (xui);
  //  y = 9.0p-16494Q;
  xui = CONST_VINT128_DW(0, 9);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsdivqpo(x, y);
  xui = CONST_VINT128_DW(0x3ffbc71c71c71c71, 0xc71c71c71c71c71d);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  //  x = 1.0p-16494Q;
  xui = CONST_VINT128_DW(0, 1);
  x = vec_xfer_vui64t_2_bin128 (xui);
  //  y = 99.0p-16494Q;
  xui = CONST_VINT128_DW(0, 99);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsdivqpo(x, y);
  xui = CONST_VINT128_DW(0x3ff84afd6a052bf5, 0xa814afd6a052bf5b);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  //  x = 1.0p-16494Q;
  xui = CONST_VINT128_DW(0, 1);
  x = vec_xfer_vui64t_2_bin128 (xui);
  //  y = 127.0p-16494Q;
  xui = CONST_VINT128_DW(0, 127);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsdivqpo(x, y);
  xui = CONST_VINT128_DW(0x3ff8020408102040, 0x8102040810204081);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  //  x = 1.0p-16494Q;
  xui = CONST_VINT128_DW(0, 1);
  x = vec_xfer_vui64t_2_bin128 (xui);
  //  y = 257.0p-16494Q;
  xui = CONST_VINT128_DW(0, 257);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsdivqpo(x, y);
  xui = CONST_VINT128_DW(0x3ff6fe01fe01fe01, 0xfe01fe01fe01fe01);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  //  x = 1.0p-16430Q;
  xui = CONST_VINT128_DW(1, 0);
  x = vec_xfer_vui64t_2_bin128 (xui);
  //  y = 257.0p-16430Q;
  xui = CONST_VINT128_DW(257, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsdivqpo(x, y);
  xui = CONST_VINT128_DW(0x3ff6fe01fe01fe01, 0xfe01fe01fe01fe01);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  //  x = 1.0p-16430Q;
  xui = CONST_VINT128_DW(0x100, 0);
  x = vec_xfer_vui64t_2_bin128 (xui);
  //  y = 257.0p-16430Q;
  xui = CONST_VINT128_DW(0x1010, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsdivqpo(x, y);
  xui = CONST_VINT128_DW(0x3ffafe01fe01fe01, 0xfe01fe01fe01fe01);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  //  x = 1.0p-35Q;
  xui = CONST_VINT128_DW(0x3fdc000000000000, 0);
  x = vec_xfer_vui64t_2_bin128 (xui);
  //  y = 257.0p-16430Q;
  xui = CONST_VINT128_DW(0x1010, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsdivqpo(x, y);
  xui = CONST_VINT128_DW(0x7ffdfe01fe01fe01, 0xfe01fe01fe01fe01);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  //  x = 1.0p-34Q;
  xui = CONST_VINT128_DW(0x3fdd000000000000, 0);
  x = vec_xfer_vui64t_2_bin128 (xui);
  //  y = 257.0p-16430Q;
  xui = CONST_VINT128_DW(0x1010, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsdivqpo(x, y);
  xui = CONST_VINT128_DW(0x7ffefe01fe01fe01, 0xfe01fe01fe01fe01);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  //  x = 1.0p-33Q;
  xui = CONST_VINT128_DW(0x3fde000000000000, 0);
  x = vec_xfer_vui64t_2_bin128 (xui);
  //  y = 257.0p-16430Q;
  xui = CONST_VINT128_DW(0x1010, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsdivqpo(x, y);
  // e == f128_MAX
  xui = CONST_VINT128_DW(0x7ffeffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  //  x = 1.0p-16382Q; __F128_MIN__
  xui = CONST_VINT128_DW(0x0001000000000000, 0x0000000000000000);
  x = vec_xfer_vui64t_2_bin128 (xui);
  //  y = 257.0Q;
  xui = CONST_VINT128_DW(0x3fff010000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsdivqpo(x, y);
  xui = CONST_VINT128_DW(0x0000ff00ff00ff00, 0xff00ff00ff00ff01);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  //  x = 1.0p-16382Q; __F128_MIN__
  xui = CONST_VINT128_DW(0x0001000000000000, 0x0000000000000000);
  x = vec_xfer_vui64t_2_bin128 (xui);
  //  y = 257.0p4Q;
  xui = CONST_VINT128_DW(0x4003010000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsdivqpo(x, y);
  xui = CONST_VINT128_DW(0x00000ff00ff00ff0, 0x0ff00ff00ff00ff1);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  //  x = 1.0p-16382Q; __F128_MIN__
  xui = CONST_VINT128_DW(0x0001000000000000, 0x0000000000000000);
  x = vec_xfer_vui64t_2_bin128 (xui);
  //  y = 257.0p100Q;
  xui = CONST_VINT128_DW(0x4063010000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsdivqpo(x, y);
  xui = CONST_VINT128_DW(0x0000000000000000, 0x0000000000000ff1);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  //  x = 1.0p-16382Q; __F128_MIN__
  xui = CONST_VINT128_DW(0x0001000000000000, 0x0000000000000000);
  x = vec_xfer_vui64t_2_bin128 (xui);
  //  y = 257.0p116Q;
  xui = CONST_VINT128_DW(0x4073010000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xsdivqpo(x, y);
  xui = CONST_VINT128_DW(0x0000000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  return (rc);
}

#undef __DEBUG_PRINT__
int
test_div_qpo_xtra (void)
{
  __binary128 x, y;
  __binary128 t, e;
  vui64_t xui;
  int rc = 0;
  printf ("\n%s\n", __FUNCTION__);

  // PowerISA 3.1 Table 81 Actions for xsdivqp[o]
  // Line -infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  // Line -NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  // Line -zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  // Line +zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  // Line +NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  // Line +infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  // Line QNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  // Line SNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_snan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsdivqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsdivqpo", e, t, e);

  return (rc);
}
#undef __DEBUG_PRINT__

//#define __DEBUG_PRINT__ 1
//#define __DEBUG_PRINT__ 2
#ifdef __DEBUG_PRINT__
#if (__DEBUG_PRINT__ == 2)
#define test_xsmulqpo(_l,_k)	db_vec_xsmulqpo(_l,_k)
#else
// Test implementation from vec_f128_dummy.c
extern __binary128 test_vec_mulqpo (__binary128 vfa, __binary128 vfb);
#define test_xsmulqpo(_l,_k)	test_vec_mulqpo(_l,_k)
#endif
#else
#if 1
// Test implementation from libpvecstatic
extern __binary128 __VEC_PWR_IMP (vec_xsmulqpo) (__binary128 vfa, __binary128 vfb);
#define test_xsmulqpo(_l,_k)	__VEC_PWR_IMP (vec_xsmulqpo)(_l,_k)
#else
// Test implementation expanded from vec_f128_ppc.h
extern __binary128 test_vec_xsmulqpo (__binary128 vfa, __binary128 vfb);
#define test_xsmulqpo(_l,_k)	test_vec_xsmulqpo(_l,_k)
#endif
#endif

int
test_mul_qpo (void)
{
  __binary128 x, y;
  __binary128 t, e;
  vui64_t xui;
  int rc = 0;
  printf ("\n%s\n", __FUNCTION__);

  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

//  x = 3.0Q;
  xui = CONST_VINT128_DW ( 0x4000800000000000, 0 );
  x = vec_xfer_vui64t_2_bin128 ( xui );
//  y = 3.0Q;
  xui = CONST_VINT128_DW ( 0x4000800000000000, 0 );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);

//  e = 9.0Q;
  xui = CONST_VINT128_DW ( 0x4002200000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  //  x = 3.0Q;
    xui = CONST_VINT128_DW ( 0x4000800000000000, 1 );
    x = vec_xfer_vui64t_2_bin128 ( xui );
  //  y = 3.0Q;
    xui = CONST_VINT128_DW ( 0x4000800000000000, 0 );
    y = vec_xfer_vui64t_2_bin128 ( xui );

  #ifdef __DEBUG_PRINT__
    print_vfloat128x(" x=  ", x);
    print_vfloat128x(" y=  ", y);
  #endif
    t = test_xsmulqpo (x, y);

  //  e = 9.0Q;
    xui = CONST_VINT128_DW ( 0x4002200000000000, 1 );
    e = vec_xfer_vui64t_2_bin128 ( xui );
    rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);
  //  x = 1.0p-8192Q;
  xui = CONST_VINT128_DW ( 0x1fff000000000000, 0 );
  x = vec_xfer_vui64t_2_bin128 ( xui );
  //  y = 1.0p-8192Q;
  xui = CONST_VINT128_DW ( 0x1fff000000000000, 0 );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  xui = CONST_VINT128_DW (0x0000400000000000, 0x0000000000000000);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  //  x = 1.0p-8191Q;
  xui = CONST_VINT128_DW ( 0x2000000000000000, 0 );
  x = vec_xfer_vui64t_2_bin128 ( xui );
  //  y = 1.0p-8192Q;
  xui = CONST_VINT128_DW ( 0x1fff000000000000, 0 );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  xui = CONST_VINT128_DW (0x0000800000000000, 0x0000000000000000);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  //  x = 0.5Q;
  xui = CONST_VINT128_DW ( 0x3ffe000000000000, 0 );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  xui = CONST_VINT128_DW (0x00007fffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  //  x = 1.0p-111Q;
  xui = CONST_VINT128_DW ( 0x3f90000000000000, 0 );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  xui = CONST_VINT128_DW (0, 1);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  //  x = 1.5Q;
  xui = CONST_VINT128_DW ( 0x3fff800000000000, 0 );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  xui = CONST_VINT128_DW (0x00017fffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  //  x = 0.75Q;
  xui = CONST_VINT128_DW ( 0x3ffe800000000000, 0 );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  xui = CONST_VINT128_DW (0x0000bfffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  //  x = 1.999999Q;
  xui = CONST_VINT128_DW ( 0x3fffffffffffffff, 0xffffffffffffffff );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  xui = CONST_VINT128_DW (0x0001ffffffffffff, 0xfffffffffffffffd);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  //  x = 0.75Q;
  xui = CONST_VINT128_DW ( 0x3ffeffffffffffff, 0xffffffffffffffff );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  xui = CONST_VINT128_DW (0x0000ffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  //  x = 0.75Q;
  xui = CONST_VINT128_DW ( 0x3f8fffffffffffff, 0xffffffffffffffff );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  xui = CONST_VINT128_DW (0, 1);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  //  x = 0.75Q;
  xui = CONST_VINT128_DW ( 0x3f8effffffffffff, 0xffffffffffffffff );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  xui = CONST_VINT128_DW (0, 1);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  //  x = 0.75Q;
  xui = CONST_VINT128_DW ( 0x3f87ffffffffffff, 0xffffffffffffffff );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  xui = CONST_VINT128_DW (0, 1);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_max );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  //  x = 1.5Q;
  xui = CONST_VINT128_DW ( 0x3fff800000000000, 0 );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  // e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  e = vec_xfer_vui64t_2_bin128 ( vf128_max );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  //  x = 2.0Q;
  xui = CONST_VINT128_DW ( 0x4000000000000000, 0 );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  // e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  e = vec_xfer_vui64t_2_bin128 ( vf128_max );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  // e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  e = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  return (rc);
}

int
test_mul_qpo_xtra (void)
{
  __binary128 x, y;
  __binary128 t, e;
  vui64_t xui;
  int rc = 0;
  printf ("\n%s\n", __FUNCTION__);

  // PowerISA 3.1 Table 84 Actions for xsmulqp[o]
  // Line -infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  // Line -NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  // Line -zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  // Line +zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  // Line +NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  // Line +infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  // Line QNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  // Line SNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_snan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmulqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmulqpo", e, t, e);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
//#define __DEBUG_PRINT__ 2
#ifdef __DEBUG_PRINT__
#if (__DEBUG_PRINT__ == 2)
#define test_xssubqpo(_l,_k)	db_vec_xssubqpo(_l,_k)
#else
// Test implementation from vec_f128_dummy.c
extern __binary128 test_vec_subqpo (__binary128 vfa, __binary128 vfb);
#define test_xssubqpo(_l,_k)	test_vec_subqpo(_l,_k)
#endif
#else
#if 1
// Test implementation from libpvecstatic
extern __binary128 __VEC_PWR_IMP (vec_xssubqpo) (__binary128 vfa, __binary128 vfb);
#define test_xssubqpo(_l,_k)	__VEC_PWR_IMP (vec_xssubqpo)(_l,_k)
#else
// Test implementation expanded from vec_f128_ppc.h
extern __binary128 test_vec_xssubqpo (__binary128 vfa, __binary128 vfb);
#define test_xssubqpo(_l,_k)	test_vec_xssubqpo(_l,_k)
#endif
#endif

int
test_sub_qpo (void)
{
  __binary128 x, y;
  __binary128 t, e;
  vui64_t xui;
  int rc = 0;
  printf ("\n%s\n", __FUNCTION__);

  x = vec_xfer_vui64t_2_bin128 (vf128_one);
  y = vec_xfer_vui64t_2_bin128 (vf128_two);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xssubqpo(x, y);
  e = vec_xfer_vui64t_2_bin128 (vf128_none);
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 (vf128_zero);
  y = vec_xfer_vui64t_2_bin128 (vf128_nzero);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xssubqpo(x, y);
  e = vec_xfer_vui64t_2_bin128 (vf128_zero);
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 (vf128_zero);
  y = vec_xfer_vui64t_2_bin128 (vf128_none);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xssubqpo(x, y);
  e = vec_xfer_vui64t_2_bin128 (vf128_one);
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 (vf128_one);
  y = vec_xfer_vui64t_2_bin128 (vf128_none);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xssubqpo(x, y);
  e = vec_xfer_vui64t_2_bin128 (vf128_two);
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

//  x = 3.0Q;
  xui = CONST_VINT128_DW(0x4000800000000000, 0);
  x = vec_xfer_vui64t_2_bin128 (xui);
//  y = 3.0Q;
  xui = CONST_VINT128_DW(0xc000800000000000, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xssubqpo(x, y);

//  e = 6.0Q;
  xui = CONST_VINT128_DW(0x4001800000000000, 0);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  //  x = 3.0Q;
  xui = CONST_VINT128_DW(0x4000800000000000, 0);
  x = vec_xfer_vui64t_2_bin128 (xui);
  y = vec_xfer_vui64t_2_bin128 (vf128_one);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xssubqpo(x, y);

  //  e = 2.0Q;
  e = vec_xfer_vui64t_2_bin128 (vf128_two);
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  //  x = 3.0Q;
  xui = CONST_VINT128_DW(0x4000800000000000, 0);
  x = vec_xfer_vui64t_2_bin128 (xui);
  y = vec_xfer_vui64t_2_bin128 (vf128_two);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xssubqpo(x, y);

  //  e = 1.0Q;
  e = vec_xfer_vui64t_2_bin128 (vf128_one);
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  //  x = 1.0Q;
  x = vec_xfer_vui64t_2_bin128 (vf128_one);
  // y = 3.0p-113Q;
  xui = CONST_VINT128_DW(0xbf8e800000000000, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xssubqpo(x, y);

  //  e = 1.0Q odd;
  xui = CONST_VINT128_DW(0x3fff000000000000, 1);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  //  x = 1.0Q;
  x = vec_xfer_vui64t_2_bin128 (vf128_one);
  // y = -3.0p-113Q;
  xui = CONST_VINT128_DW(0x3f8e800000000000, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xssubqpo(x, y);

  //  e = 1.fffffQ odd;
  xui = CONST_VINT128_DW(0x3ffeffffffffffff, -1);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  //  x = 1.0Q;
  x = vec_xfer_vui64t_2_bin128 (vf128_one);
  // y = 3.0p-115Q;
  xui = CONST_VINT128_DW(0xbf8c800000000000, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xssubqpo(x, y);

  //  e = 1.0Q odd;
  xui = CONST_VINT128_DW(0x3fff000000000000, 1);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  //  x = 1.0Q;
  x = vec_xfer_vui64t_2_bin128 (vf128_one);
  // y = -3.0p-115Q;
  xui = CONST_VINT128_DW(0x3f8c800000000000, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xssubqpo(x, y);

  //  e = 1.fffffQ odd;
  xui = CONST_VINT128_DW(0x3ffeffffffffffff, -1);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  //  x = 1.0Q;
  x = vec_xfer_vui64t_2_bin128 (vf128_one);
  // y = 3.0p-117Q;
  xui = CONST_VINT128_DW(0xbf8a800000000000, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xssubqpo(x, y);

  //  e = 1.0Q odd;
  xui = CONST_VINT128_DW(0x3fff000000000000, 1);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  //  x = 1.0Q;
  x = vec_xfer_vui64t_2_bin128 (vf128_one);
  // y = -3.0p-117Q;
  xui = CONST_VINT128_DW(0x3f8a800000000000, 0);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xssubqpo(x, y);

  //  e = 1.fffffQ odd;
  xui = CONST_VINT128_DW(0x3ffeffffffffffff, -1);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 (vf128_sub);
  y = vec_xfer_vui64t_2_bin128 (vf128_nsub);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xssubqpo(x, y);
  xui = CONST_VINT128_DW(0x0001ffffffffffff, 0xfffffffffffffffe);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 (vf128_sub);
  y = vec_xfer_vui64t_2_bin128 (vf128_sub);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xssubqpo(x, y);
  e = vec_xfer_vui64t_2_bin128 (vf128_zero);
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  // y = 1.0p-16382Q;
  xui = CONST_VINT128_DW(0, 1);
  x = vec_xfer_vui64t_2_bin128 (xui);
  // y = 1.0pp-16382Q;
  xui = CONST_VINT128_DW(0x8000000000000000, 1);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xssubqpo(x, y);
  xui = CONST_VINT128_DW(0x0, 2);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  // x = 0.ffffffp-16382QQ;
  x = vec_xfer_vui64t_2_bin128 (vf128_sub);
  // y = 0.0000001p-16382Q;
  xui = CONST_VINT128_DW(0x8000000000000000, 1);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xssubqpo(x, y);

  //  e = 1.fffffQ odd;
  xui = CONST_VINT128_DW(0x0001000000000000, 0);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  // x = 0.ffffffp-16382QQ;
  x = vec_xfer_vui64t_2_bin128 (vf128_sub);
  // y = -0.0000001p-16382Q;
  xui = CONST_VINT128_DW(0x0000000000000000, 1);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xssubqpo(x, y);

  //  e = 0.fffff...eQ;
  xui = CONST_VINT128_DW(0x0000ffffffffffff, 0xfffffffffffffffe);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  //  x = 1.875Q;
  xui = CONST_VINT128_DW(0x3fffe00000000000, 0);
  x = vec_xfer_vui64t_2_bin128 (xui);
  // y = 1.0p-3Q;
  xui = CONST_VINT128_DW(0xbffc000000000000, 1);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xssubqpo(x, y);

  //  e = 2.0000Q odd;
  xui = CONST_VINT128_DW(0x4000000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  //  x = 1.875Q;
  xui = CONST_VINT128_DW(0x3fffe00000000000, 0);
  x = vec_xfer_vui64t_2_bin128 (xui);
  // y = -1.0p-3Q;
  xui = CONST_VINT128_DW(0x3ffc000000000000, 1);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xssubqpo(x, y);

  //  e = 1.7421875....Q odd;
  xui = CONST_VINT128_DW(0x3fffbfffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  //  x = 1.0Q;
  x = vec_xfer_vui64t_2_bin128 (vf128_one);
  // y = -1.0p-3Q;
  xui = CONST_VINT128_DW(0x3ffc000000000000, 1);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xssubqpo(x, y);

  //  e = 1.7421875....Q odd;
  xui = CONST_VINT128_DW(0x3ffebfffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  //  x = 1.9999Q;
  xui = CONST_VINT128_DW(0x3fffffffffffffff, 0xffffffffffffffff);
  x = vec_xfer_vui64t_2_bin128 (xui);
  // y = -1.9998Q;
  xui = CONST_VINT128_DW(0x3fffffffffffffff, 0xfffffffffffffffe);
  y = vec_xfer_vui64t_2_bin128 (xui);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xssubqpo(x, y);

  //  e = 1.7421875....Q odd;
  xui = CONST_VINT128_DW(0x3f8f000000000000, 0x0000000000000000);
  e = vec_xfer_vui64t_2_bin128 (xui);
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 (vf128_max);
  y = vec_xfer_vui64t_2_bin128 (vf128_nmax);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xssubqpo(x, y);
  // e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  e = vec_xfer_vui64t_2_bin128 (vf128_max);
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 (vf128_nmax);
  y = vec_xfer_vui64t_2_bin128 (vf128_max);

#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
  print_vfloat128x (" y=  ", y);
#endif
  t = test_xssubqpo(x, y);
  // e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  e = vec_xfer_vui64t_2_bin128 (vf128_nmax);
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  return (rc);
}

int
test_sub_qpo_xtra (void)
{
  __binary128 x, y;
  __binary128 t, e;
  vui64_t xui;
  int rc = 0;
  printf ("\n%s\n", __FUNCTION__);

  // PowerISA 3.1 Table 65 Actions for xssubqp[o]
  // Line -infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  // e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  e = y;
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  // Line -NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ntwo );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  // Line -zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  // Line +zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  // Line +NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_two );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  // Line +infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  // Line QNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  // Line SNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_snan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xssubqpo (x, y);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xssubqpo", e, t, e);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
//#define __DEBUG_PRINT__ 2
#ifdef __DEBUG_PRINT__
#if (__DEBUG_PRINT__ == 2)
#define test_xsmaddqpo(_l,_k,_j)	db_vec_maddqpo(_l,_k,_j)
#else
// Test implementation from vec_f128_dummy.c
extern __binary128 test_vec_maddqpo (__binary128 vfa, __binary128 vfb, __binary128 vfc);
#define test_xsmaddqpo(_l,_k,_j)	test_vec_maddqpo(_l,_k,_j)
#endif
#else
#if 1
// Test implementation from libpvecstatic
extern __binary128 __VEC_PWR_IMP (vec_xsmaddqpo) (__binary128 vfa, __binary128 vfb, __binary128 vfc);
#define test_xsmaddqpo(_l,_k,_j)	__VEC_PWR_IMP (vec_xsmaddqpo)(_l,_k,_j)
#else
// Test implementation expanded from vec_f128_ppc.h
extern __binary128 test_vec_xsmaddqpo (__binary128 vfa, __binary128 vfb, __binary128 vfc);
#define test_xsmaddqpo(_l,_k,_j)	test_vec_xsmaddqpo(_l,_k,_j)
#endif
#endif

int
test_madd_qpo_xtra_c1 (void)
{
  __binary128 x, y, z;
  __binary128 t, e;
  vui64_t xui;
  int rc = 0;
  printf ("\n%s\n", __FUNCTION__);

  // PowerISA 3.0 Table 71 Actions for xsmaddqp[o], VTR = -0.0
  // Line -infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  z = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" z=  ", z);
#endif

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line -NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line -zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line +zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line +NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line +infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line QNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line SNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_snan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  return (rc);
}

int
test_madd_qpo_xtra_c2 (void)
{
  __binary128 x, y, z;
  __binary128 t, e;
  vui64_t xui;
  int rc = 0;
  printf ("\n%s\n", __FUNCTION__);

  // PowerISA 3.0 Table 71 Actions for xsmaddqp[o], VTR = -1.0
  // Line -infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  z = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" z=  ", z);
#endif

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line -NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ntwo );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line -zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line +zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line +NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ntwo );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line +infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line QNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line SNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_snan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  return (rc);
}

int
test_madd_qpo_xtra_c3 (void)
{
  __binary128 x, y, z;
  __binary128 t, e;
  vui64_t xui;
  int rc = 0;
  printf ("\n%s\n", __FUNCTION__);

  // PowerISA 3.0 Table 71 Actions for xsmaddqp[o], VTR = -0.0
  // Line -infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  z = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" z=  ", z);
#endif

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line -NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line -zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line +zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line +NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line +infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line QNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line SNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_snan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  return (rc);
}

int
test_madd_qpo_xtra_c4 (void)
{
  __binary128 x, y, z;
  __binary128 t, e;
  vui64_t xui;
  int rc = 0;
  printf ("\n%s\n", __FUNCTION__);

  // PowerISA 3.0 Table 71 Actions for xsmaddqp[o], VTR = 0.0
  // Line -infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  z = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" z=  ", z);
#endif

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line -NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line -zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line +zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line +NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line +infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line QNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line SNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_snan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  return (rc);
}

int
test_madd_qpo_xtra_c5 (void)
{
  __binary128 x, y, z;
  __binary128 t, e;
  vui64_t xui;
  int rc = 0;
  printf ("\n%s\n", __FUNCTION__);

  // PowerISA 3.0 Table 71 Actions for xsmaddqp[o], VTR = 1.0
  // Line -infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  z = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" z=  ", z);
#endif

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line -NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_two );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line -zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line +zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line +NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_two );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line +infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line QNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line SNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_snan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  return (rc);
}
//#undef __DEBUG_PRINT__

int
test_madd_qpo_xtra_c6 (void)
{
  __binary128 x, y, z;
  __binary128 t, e;
  vui64_t xui;
  int rc = 0;
  printf ("\n%s\n", __FUNCTION__);

  // PowerISA 3.0 Table 71 Actions for xsmaddqp[o], VTR = -0.0
  // Line -infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  z = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" z=  ", z);
#endif

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line -NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line -zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line +zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line +NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line +infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line QNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line SNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_snan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  return (rc);
}

int
test_madd_qpo_xtra_c7 (void)
{
  __binary128 x, y, z;
  __binary128 t, e;
  vui64_t xui;
  int rc = 0;
  printf ("\n%s\n", __FUNCTION__);

  // PowerISA 3.0 Table 71 Actions for xsmaddqp[o], VTR = -0.0
  // Line -infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  z = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" z=  ", z);
#endif

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
//  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
//  e = vec_xfer_vui64t_2_bin128 ( xui );
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line -NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
//  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
//  e = vec_xfer_vui64t_2_bin128 ( xui );
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line -zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
//  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
//  e = vec_xfer_vui64t_2_bin128 ( xui );
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line +zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan);
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
//  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
//  e = vec_xfer_vui64t_2_bin128 ( xui );
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line +NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
//  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
//  e = vec_xfer_vui64t_2_bin128 ( xui );
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line +infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
//  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
//  e = vec_xfer_vui64t_2_bin128 ( xui );
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line QNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line SNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_snan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  return (rc);
}

int
test_madd_qpo_xtra_c8 (void)
{
  __binary128 x, y, z;
  __binary128 t, e;
  vui64_t xui;
  int rc = 0;
  printf ("\n%s\n", __FUNCTION__);

  // PowerISA 3.0 Table 71 Actions for xsmaddqp[o], VTR = -0.0
  // Line -infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  z = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" z=  ", z);
#endif

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line -NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line -zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line +zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line +NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line +infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line QNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  // Line SNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_snan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  return (rc);
}

int
test_madd_qpo_xtra (void)
{
  int rc = 0;

  printf ("\n%s\n", __FUNCTION__);

  rc += test_madd_qpo_xtra_c1 ();

  rc += test_madd_qpo_xtra_c2 ();

  rc += test_madd_qpo_xtra_c3 ();

  rc += test_madd_qpo_xtra_c4 ();

  rc += test_madd_qpo_xtra_c5 ();

  rc += test_madd_qpo_xtra_c6 ();

  rc += test_madd_qpo_xtra_c7 ();

  rc += test_madd_qpo_xtra_c8 ();

  return (rc);
}

int
test_madd_qpo_zero_c (void)
{
  __binary128 x, y, z, nz;
  __binary128 t, e;
  vui64_t xui;
  int rc = 0;
  printf ("\n%s\n", __FUNCTION__);

  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  z = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  nz = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y,z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y,z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y,z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

//  x = 3.0Q;
  xui = CONST_VINT128_DW ( 0x4000800000000000, 0 );
  x = vec_xfer_vui64t_2_bin128 ( xui );
//  y = 3.0Q;
  xui = CONST_VINT128_DW ( 0x4000800000000000, 0 );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y,z);

//  e = 9.0Q;
  xui = CONST_VINT128_DW ( 0x4002200000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  x = 3.0Q;
    xui = CONST_VINT128_DW ( 0x4000800000000000, 1 );
    x = vec_xfer_vui64t_2_bin128 ( xui );
  //  y = 3.0Q;
    xui = CONST_VINT128_DW ( 0x4000800000000000, 0 );
    y = vec_xfer_vui64t_2_bin128 ( xui );

  #ifdef __DEBUG_PRINT__
    print_vfloat128x(" x=  ", x);
    print_vfloat128x(" y=  ", y);
  #endif
    t = test_xsmaddqpo (x, y,z);

  //  e = 9.0Q;
    xui = CONST_VINT128_DW ( 0x4002200000000000, 1 );
    e = vec_xfer_vui64t_2_bin128 ( xui );
    rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y,z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);
  //  x = 1.0p-8192Q;
  xui = CONST_VINT128_DW ( 0x1fff000000000000, 0 );
  x = vec_xfer_vui64t_2_bin128 ( xui );
  //  y = 1.0p-8192Q;
  xui = CONST_VINT128_DW ( 0x1fff000000000000, 0 );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x0000400000000000, 0x0000000000000000);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  x = 1.0p-8191Q;
  xui = CONST_VINT128_DW ( 0x2000000000000000, 0 );
  x = vec_xfer_vui64t_2_bin128 ( xui );
  //  y = 1.0p-8192Q;
  xui = CONST_VINT128_DW ( 0x1fff000000000000, 0 );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x0000800000000000, 0x0000000000000000);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y,nz);
  xui = CONST_VINT128_DW (0x0000800000000000, 0x0000000000000000);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  //  x = 0.5Q;
  xui = CONST_VINT128_DW ( 0x3ffe000000000000, 0 );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x00007fffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  //  x = 1.0p-111Q;
  xui = CONST_VINT128_DW ( 0x3f90000000000000, 0 );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0, 1);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  //  x = 1.5Q;
  xui = CONST_VINT128_DW ( 0x3fff800000000000, 0 );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x00017fffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  //  x = 0.75Q;
  xui = CONST_VINT128_DW ( 0x3ffe800000000000, 0 );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x0000bfffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  //  x = 1.999999Q;
  xui = CONST_VINT128_DW ( 0x3fffffffffffffff, 0xffffffffffffffff );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x0001ffffffffffff, 0xfffffffffffffffd);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  //  x = 0.75Q;
  xui = CONST_VINT128_DW ( 0x3ffeffffffffffff, 0xffffffffffffffff );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x0000ffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  //  x = 0.75Q;
  xui = CONST_VINT128_DW ( 0x3f8fffffffffffff, 0xffffffffffffffff );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0, 1);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  //  x = 0.75Q;
  xui = CONST_VINT128_DW ( 0x3f8effffffffffff, 0xffffffffffffffff );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0, 1);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  //  x = 0.75Q;
  xui = CONST_VINT128_DW ( 0x3f87ffffffffffff, 0xffffffffffffffff );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0, 1);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y,z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_max );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  //  x = 1.5Q;
  xui = CONST_VINT128_DW ( 0x3fff800000000000, 0 );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y,z);
  // e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  e = vec_xfer_vui64t_2_bin128 ( vf128_max );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  //  x = 2.0Q;
  xui = CONST_VINT128_DW ( 0x4000000000000000, 0 );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y,z);
  // e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  e = vec_xfer_vui64t_2_bin128 ( vf128_max );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y,z);
  // e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  e = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  return (rc);
}

int
test_madd_qpo (void)
{
  __binary128 x, y, z;
  __binary128 t, e;
  vui64_t xui;
  int rc = 0;
  printf ("\n%s\n", __FUNCTION__);

  x = vec_xfer_vui64t_2_bin128 ( vf128_two );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );
  z = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  //  e = 3.0Q;
  xui = CONST_VINT128_DW (0x4000800000000000, 0x0000000000000000);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  z = 1.0p-111;
  xui = CONST_VINT128_DW (0x3f90000000000000, 0x0000000000000000);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x4000000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  z = 1.0p-112;
  xui = CONST_VINT128_DW (0x3f8f000000000000, 0x0000000000000000);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x4000000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  z = 1.0p-127;
  xui = CONST_VINT128_DW (0x3f80000000000000, 0x0000000000000000);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x4000000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  z = 1.0p-128;
  xui = CONST_VINT128_DW (0x3f7f000000000000, 0x0000000000000000);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x4000000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  z = 1.0p-239;
  xui = CONST_VINT128_DW (0x3f10000000000000, 0x0000000000000000);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x4000000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  z = 1.0p-240;
  xui = CONST_VINT128_DW (0x3f0f000000000000, 0x0000000000000000);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x4000000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  z = 1.0p-241;
  xui = CONST_VINT128_DW (0x3f0e000000000000, 0x0000000000000000);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x4000000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  z = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y,z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  z = -1.0p-111;
  xui = CONST_VINT128_DW (0xbf90000000000000, 0x0000000000000000);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x3fffffffffffffff, 0xfffffffffffffffe);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  z = -1.0p-112;
  xui = CONST_VINT128_DW (0xbf8f000000000000, 0x0000000000000000);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x3fffffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  z = -1.0p-127;
  xui = CONST_VINT128_DW (0xbf80000000000000, 0x0000000000000000);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x3fffffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  z = -1.0p-128;
  xui = CONST_VINT128_DW (0xbf7f000000000000, 0x0000000000000000);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x3fffffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  z = -1.0p-239;
  xui = CONST_VINT128_DW (0xbf10000000000000, 0x0000000000000000);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x3fffffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  z = -1.0p-240;
  xui = CONST_VINT128_DW (0xbf0f000000000000, 0x0000000000000000);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x3fffffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  z = -1.0p-241;
  xui = CONST_VINT128_DW (0xbf0e000000000000, 0x0000000000000000);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x3fffffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  //  y = 0.5Q;
  xui = CONST_VINT128_DW (0x3ffe000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );
  z = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0x3fff800000000000, 0x0000000000000000);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  y = 1.0p-112;
  xui = CONST_VINT128_DW (0x3f8f000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0x3fff000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  y = 1.0p-113;
  xui = CONST_VINT128_DW (0x3f8e000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0x3fff000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  y = 1.0p-127;
  xui = CONST_VINT128_DW (0x3f80000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0x3fff000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  y = 1.0p-128;
  xui = CONST_VINT128_DW (0x3f7f000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0x3fff000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  z = 1.0p-240;
  xui = CONST_VINT128_DW (0x3f0f000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmaddqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0x3fff000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  z = 1.0p-241;
  xui = CONST_VINT128_DW (0x3f0e000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0x3fff000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  z = 1.0p-256;
  xui = CONST_VINT128_DW (0x3eff000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0x3fff000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  //  y = 0.5Q;
  xui = CONST_VINT128_DW (0x3ffe000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );
  z = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0xbffe000000000000, 0x0000000000000000);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  y = 1.0p-112;
  xui = CONST_VINT128_DW (0x3f8f000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0xbffeffffffffffff, 0xfffffffffffffffe);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  y = 1.0p-113;
  xui = CONST_VINT128_DW (0x3f8e000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0xbffeffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  y = 1.0p-127;
  xui = CONST_VINT128_DW (0x3f80000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0xbffeffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  y = 1.0p-128;
  xui = CONST_VINT128_DW (0x3f7f000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0xbffeffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  z = 1.0p-240;
  xui = CONST_VINT128_DW (0x3f0f000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0xbffeffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  z = 1.0p-241;
  xui = CONST_VINT128_DW (0x3f0e000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0xbffeffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  z = 1.0p-256;
  xui = CONST_VINT128_DW (0x3eff000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0xbffeffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  y = 1.99999999999998Q;
  xui = CONST_VINT128_DW (0x3fffffffffffffff, 0xffffffffffffffff);
  x = vec_xfer_vui64t_2_bin128 ( xui );
  y = vec_xfer_vui64t_2_bin128 ( xui );
  xui = CONST_VINT128_DW (0xc000ffffffffffff, 0xfffffffffffffffe);
  z = vec_xfer_vui64t_2_bin128 ( xui );
  // z = vec_xfer_vui64t_2_bin128 ( vf128_zero);

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x3f1f000000000000, 0x0000000000000000);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  xui = CONST_VINT128_DW (0xc000ffffffffffff, 0xffffffffffffffff);
  z = vec_xfer_vui64t_2_bin128 ( xui );
  // z = vec_xfer_vui64t_2_bin128 ( vf128_zero);

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0xbf8fffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  y = 1.99999999999998Q;
  xui = CONST_VINT128_DW (0x3fffffffffffffff, 0xffffffffffffffff);
  x = vec_xfer_vui64t_2_bin128 ( xui );
  //  y = 1.99999999999998p-16271Q;
  xui = CONST_VINT128_DW (0x0070ffffffffffff, 0xffffffffffffffff);
  y = vec_xfer_vui64t_2_bin128 ( xui );
  //  z = -1.99999999999998p-16272Q;
  xui = CONST_VINT128_DW (0x8071ffffffffffff, 0xfffffffffffffffe);
  z = vec_xfer_vui64t_2_bin128 ( xui );
  //z = vec_xfer_vui64t_2_bin128 ( vf128_zero);

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x0000000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  y = 1.99999999999998Q;
  xui = CONST_VINT128_DW (0x3fffffffffffffff, 0xffffffffffffffff);
  x = vec_xfer_vui64t_2_bin128 ( xui );
  //  y = 1.99999999999998p-16271Q;
  xui = CONST_VINT128_DW (0x0070ffffffffffff, 0xffffffffffffffff);
  y = vec_xfer_vui64t_2_bin128 ( xui );
  //  z = -1.99999999999999p-16272Q;
  xui = CONST_VINT128_DW (0x8071ffffffffffff, 0xffffffffffffffff);
  z = vec_xfer_vui64t_2_bin128 ( xui );
  //z = vec_xfer_vui64t_2_bin128 ( vf128_zero);

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x8000ffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  y = 1.99999999999998Q;
  xui = CONST_VINT128_DW (0x3fffffffffffffff, 0xffffffffffffffff);
  x = vec_xfer_vui64t_2_bin128 ( xui );
  //  y = 1.99999999999998p-16273Q;
  xui = CONST_VINT128_DW (0x006fffffffffffff, 0xffffffffffffffff);
  y = vec_xfer_vui64t_2_bin128 ( xui );
  //  z = -1.99999999999998p-16272Q;
  xui = CONST_VINT128_DW (0x8070ffffffffffff, 0xfffffffffffffffe);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x0000000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  y = 1.99999999999998Q;
  xui = CONST_VINT128_DW (0x3fffffffffffffff, 0xffffffffffffffff);
  x = vec_xfer_vui64t_2_bin128 ( xui );
  //  y = 1.99999999999998p-16272Q;
  xui = CONST_VINT128_DW (0x006effffffffffff, 0xffffffffffffffff);
  y = vec_xfer_vui64t_2_bin128 ( xui );
  //  z = -1.99999999999998p-16273Q;
  xui = CONST_VINT128_DW (0x806fffffffffffff, 0xfffffffffffffffe);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x0000000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //  x = 1.99999999999998Q;
  xui = CONST_VINT128_DW (0x3fffffffffffffff, 0xffffffffffffffff);
  x = vec_xfer_vui64t_2_bin128 ( xui );
  //  y = 0.99999999999998p-16382Q;
  xui = CONST_VINT128_DW (0x0000ffffffffffff, 0xffffffffffffffff);
  y = vec_xfer_vui64t_2_bin128 ( xui );
  //  z = -1.99999999999998p-16382Q;
  xui = CONST_VINT128_DW (0x8001ffffffffffff, 0xfffffffffffffffe);
  z = vec_xfer_vui64t_2_bin128 ( xui );
  //z = vec_xfer_vui64t_2_bin128 ( vf128_zero);

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x8000000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //x = 1.41421356237309504880168872420969807856967187537695Q;
  xui = CONST_VINT128_DW (0x3fff6a09e667f3bc, 0xc908b2fb1366ea96);
  x = vec_xfer_vui64t_2_bin128 ( xui );
  //y = 1.41421356237309504880168872420969807856967187537695Q;
  xui = CONST_VINT128_DW (0x3fff6a09e667f3bc, 0xc908b2fb1366ea96);
  y = vec_xfer_vui64t_2_bin128 ( xui );
  //  z = -1.99999999999999Q;
  xui = CONST_VINT128_DW (0xbfffffffffffffff, 0xffffffffffffffff);
  z = vec_xfer_vui64t_2_bin128 ( xui );
  //z = vec_xfer_vui64t_2_bin128 ( vf128_zero);

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x3f9038eb0f40202c, 0x3070852b053f47f2);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  //x = 1.41421356237309504880168872420969807856967187537695Q;
  xui = CONST_VINT128_DW (0x3fff6a09e667f3bc, 0xc908b2fb1366ea96);
  x = vec_xfer_vui64t_2_bin128 ( xui );
  //y = 1.41421356237309504880168872420969807856967187537695Q;
  xui = CONST_VINT128_DW (0x3fff6a09e667f3bc, 0xc908b2fb1366ea96);
  y = vec_xfer_vui64t_2_bin128 ( xui );
  //  z = -2.0Q;
  z = vec_xfer_vui64t_2_bin128 ( vf128_ntwo);

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmaddqpo (x, y,z);
  xui = CONST_VINT128_DW (0x3f8f71d61e804058, 0x60e10a560a7e8fe4);

  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmaddqpo", e, t, e);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
#ifdef __DEBUG_PRINT__
#if (__DEBUG_PRINT__ == 2)
//TODO Debug not implemented, using maddqpo with negqpo
#define test_xsmsubqpo(_l,_k)	db_vec_xsmsubqpo(_l,_k)
#else
// Test implementation from vec_f128_dummy.c
extern __binary128 test_vec_msubqpo (__binary128 vfa, __binary128 vfb, __binary128 vfc);
#define test_xsmsubqpo(_l,_k,_m)	test_vec_msubqpo(_l,_k,_m)
#endif
#else
#if 1
// Test implementation from libpvecstatic
extern __binary128 __VEC_PWR_IMP (vec_xsmsubqpo) (__binary128 vfa, __binary128 vfb, __binary128 vfc);
#define test_xsmsubqpo(_l,_k,_j)	__VEC_PWR_IMP (vec_xsmsubqpo)(_l,_k,_j)
#else
// Test implementation expanded from vec_f128_ppc.h
extern __binary128 test_vec_xsmsubqpo (__binary128 vfa, __binary128 vfb, __binary128 vfc);
#define test_xsmsubqpo(_l,_k,_m)	test_vec_xsmsubqpo(_l,_k,_m)
#endif
#endif

int
test_msub_qpo_xtra_c1 (void)
{
  __binary128 x, y, z;
  __binary128 t, e;
  vui64_t xui;
  int rc = 0;
  printf ("\n%s\n", __FUNCTION__);

  // PowerISA 3.0 Table 80 Actions for xsmsubqp[o], VTR = -0.0
  // Line -infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  z = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" z=  ", z);
#endif

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line -NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line -zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line +zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line +NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line +infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line QNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line SNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_snan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  return (rc);
}

int
test_msub_qpo_xtra_c2 (void)
{
  __binary128 x, y, z;
  __binary128 t, e;
  vui64_t xui;
  int rc = 0;
  printf ("\n%s\n", __FUNCTION__);

  // PowerISA 3.0 Table 80 Actions for xsmsubqp[o], VTR = -1.0
  // Line -infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  z = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" z=  ", z);
#endif

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line -NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_two );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line -zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line +zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line +NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_two );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line +infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line QNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line SNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_snan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  return (rc);
}

int
test_msub_qpo_xtra_c3 (void)
{
  __binary128 x, y, z;
  __binary128 t, e;
  vui64_t xui;
  int rc = 0;
  printf ("\n%s\n", __FUNCTION__);

  // PowerISA 3.0 Table 80 Actions for xsmsubqp[o], VTR = -0.0
  // Line -infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  z = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" z=  ", z);
#endif

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line -NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line -zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line +zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line +NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line +infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line QNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line SNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_snan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  return (rc);
}

int
test_msub_qpo_xtra_c4 (void)
{
  __binary128 x, y, z;
  __binary128 t, e;
  vui64_t xui;
  int rc = 0;
  printf ("\n%s\n", __FUNCTION__);

  // PowerISA 3.0 Table 80 Actions for xsmsubqp[o], VTR = 0.0
  // Line -infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  z = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" z=  ", z);
#endif

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line -NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line -zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line +zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line +NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line +infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line QNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line SNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_snan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  return (rc);
}

int
test_msub_qpo_xtra_c5 (void)
{
  __binary128 x, y, z;
  __binary128 t, e;
  vui64_t xui;
  int rc = 0;
  printf ("\n%s\n", __FUNCTION__);

  // PowerISA 3.0 Table 80 Actions for xsmsubqp[o], VTR = 1.0
  // Line -infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  z = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" z=  ", z);
#endif

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line -NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ntwo );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line -zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line +zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line +NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ntwo );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line +infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line QNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line SNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_snan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  return (rc);
}
// #undef __DEBUG_PRINT__

int
test_msub_qpo_xtra_c6 (void)
{
  __binary128 x, y, z;
  __binary128 t, e;
  vui64_t xui;
  int rc = 0;
  printf ("\n%s\n", __FUNCTION__);

  // PowerISA 3.0 Table 80 Actions for xsmsubqp[o], VTR = -0.0
  // Line -infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  z = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" z=  ", z);
#endif

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line -NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line -zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line +zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line +NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line +infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line QNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line SNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_snan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  return (rc);
}

int
test_msub_qpo_xtra_c7 (void)
{
  __binary128 x, y, z;
  __binary128 t, e;
  vui64_t xui;
  int rc = 0;
  printf ("\n%s\n", __FUNCTION__);

  // PowerISA 3.0 Table 80 Actions for xsmsubqp[o], VTR = -0.0
  // Line -infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  z = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" z=  ", z);
#endif

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
//  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
//  e = vec_xfer_vui64t_2_bin128 ( xui );
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line -NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
//  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
//  e = vec_xfer_vui64t_2_bin128 ( xui );
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line -zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
//  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
//  e = vec_xfer_vui64t_2_bin128 ( xui );
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line +zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan);
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
//  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
//  e = vec_xfer_vui64t_2_bin128 ( xui );
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line +NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
//  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
//  e = vec_xfer_vui64t_2_bin128 ( xui );
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line +infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
//  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
//  e = vec_xfer_vui64t_2_bin128 ( xui );
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line QNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line SNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_snan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  return (rc);
}

int
test_msub_qpo_xtra_c8 (void)
{
  __binary128 x, y, z;
  __binary128 t, e;
  vui64_t xui;
  int rc = 0;
  printf ("\n%s\n", __FUNCTION__);

  // PowerISA 3.0 Table 80 Actions for xsmsubqp[o], VTR = -0.0
  // Line -infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  z = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" z=  ", z);
#endif

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line -NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line -zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line +zero
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line +NZF
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line +infinity
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line QNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  // Line SNaN
  x = vec_xfer_vui64t_2_bin128 ( vf128_snan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_ninf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  y = vec_xfer_vui64t_2_bin128 ( vf128_snan );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, z);
  xui = CONST_VINT128_DW ( 0x7fffc00000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  return (rc);
}

int
test_msub_qpo_xtra (void)
{
  int rc = 0;

  printf ("\n%s\n", __FUNCTION__);

  rc += test_msub_qpo_xtra_c1 ();

  rc += test_msub_qpo_xtra_c2 ();

  rc += test_msub_qpo_xtra_c3 ();

  rc += test_msub_qpo_xtra_c4 ();

  rc += test_msub_qpo_xtra_c5 ();

  rc += test_msub_qpo_xtra_c6 ();

  rc += test_msub_qpo_xtra_c7 ();

  rc += test_msub_qpo_xtra_c8 ();

  return (rc);
}

int
test_msub_qpo_zero_c (void)
{
  __binary128 x, y, z, nz;
  __binary128 t, e;
  vui64_t xui;
  int rc = 0;
  printf ("\n%s\n", __FUNCTION__);

  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  z = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  nz = vec_xfer_vui64t_2_bin128 ( vf128_nzero );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y,z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y,z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y,z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

//  x = 3.0Q;
  xui = CONST_VINT128_DW ( 0x4000800000000000, 0 );
  x = vec_xfer_vui64t_2_bin128 ( xui );
//  y = 3.0Q;
  xui = CONST_VINT128_DW ( 0x4000800000000000, 0 );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y,z);

//  e = 9.0Q;
  xui = CONST_VINT128_DW ( 0x4002200000000000, 0 );
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  x = 3.0Q;
    xui = CONST_VINT128_DW ( 0x4000800000000000, 1 );
    x = vec_xfer_vui64t_2_bin128 ( xui );
  //  y = 3.0Q;
    xui = CONST_VINT128_DW ( 0x4000800000000000, 0 );
    y = vec_xfer_vui64t_2_bin128 ( xui );

  #ifdef __DEBUG_PRINT__
    print_vfloat128x(" x=  ", x);
    print_vfloat128x(" y=  ", y);
  #endif
    t = test_xsmsubqpo (x, y,z);

  //  e = 9.0Q;
    xui = CONST_VINT128_DW ( 0x4002200000000000, 1 );
    e = vec_xfer_vui64t_2_bin128 ( xui );
    rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y,z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);
  //  x = 1.0p-8192Q;
  xui = CONST_VINT128_DW ( 0x1fff000000000000, 0 );
  x = vec_xfer_vui64t_2_bin128 ( xui );
  //  y = 1.0p-8192Q;
  xui = CONST_VINT128_DW ( 0x1fff000000000000, 0 );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x0000400000000000, 0x0000000000000000);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  x = 1.0p-8191Q;
  xui = CONST_VINT128_DW ( 0x2000000000000000, 0 );
  x = vec_xfer_vui64t_2_bin128 ( xui );
  //  y = 1.0p-8192Q;
  xui = CONST_VINT128_DW ( 0x1fff000000000000, 0 );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x0000800000000000, 0x0000000000000000);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y, nz);
  xui = CONST_VINT128_DW (0x0000800000000000, 0x0000000000000000);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  //  x = 0.5Q;
  xui = CONST_VINT128_DW ( 0x3ffe000000000000, 0 );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x00007fffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  //  x = 1.0p-111Q;
  xui = CONST_VINT128_DW ( 0x3f90000000000000, 0 );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0, 1);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  //  x = 1.5Q;
  xui = CONST_VINT128_DW ( 0x3fff800000000000, 0 );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x00017fffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  //  x = 0.75Q;
  xui = CONST_VINT128_DW ( 0x3ffe800000000000, 0 );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x0000bfffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  //  x = 1.999999Q;
  xui = CONST_VINT128_DW ( 0x3fffffffffffffff, 0xffffffffffffffff );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x0001ffffffffffff, 0xfffffffffffffffd);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  //  x = 0.75Q;
  xui = CONST_VINT128_DW ( 0x3ffeffffffffffff, 0xffffffffffffffff );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x0000ffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  //  x = 0.75Q;
  xui = CONST_VINT128_DW ( 0x3f8fffffffffffff, 0xffffffffffffffff );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0, 1);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  //  x = 0.75Q;
  xui = CONST_VINT128_DW ( 0x3f8effffffffffff, 0xffffffffffffffff );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0, 1);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  //  x = 0.75Q;
  xui = CONST_VINT128_DW ( 0x3f87ffffffffffff, 0xffffffffffffffff );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0, 1);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y,z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_max );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  //  x = 1.5Q;
  xui = CONST_VINT128_DW ( 0x3fff800000000000, 0 );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y,z);
  // e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  e = vec_xfer_vui64t_2_bin128 ( vf128_max );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  //  x = 2.0Q;
  xui = CONST_VINT128_DW ( 0x4000000000000000, 0 );
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y,z);
  // e = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  e = vec_xfer_vui64t_2_bin128 ( vf128_max );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y,z);
  // e = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  e = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  return (rc);
}
int
test_msub_qpo (void)
{
  __binary128 x, y, z;
  __binary128 t, e;
  vui64_t xui;
  int rc = 0;
  printf ("\n%s\n", __FUNCTION__);

  x = vec_xfer_vui64t_2_bin128 ( vf128_two );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );
  z = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  //  e = 3.0Q;
  xui = CONST_VINT128_DW (0x4000800000000000, 0x0000000000000000);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  z = 1.0p-111;
  xui = CONST_VINT128_DW (0xbf90000000000000, 0x0000000000000000);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x4000000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  z = 1.0p-112;
  xui = CONST_VINT128_DW (0xbf8f000000000000, 0x0000000000000000);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x4000000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  z = 1.0p-127;
  xui = CONST_VINT128_DW (0xbf80000000000000, 0x0000000000000000);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x4000000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  z = 1.0p-128;
  xui = CONST_VINT128_DW (0xbf7f000000000000, 0x0000000000000000);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x4000000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  z = 1.0p-239;
  xui = CONST_VINT128_DW (0xbf10000000000000, 0x0000000000000000);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x4000000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  z = 1.0p-240;
  xui = CONST_VINT128_DW (0xbf0f000000000000, 0x0000000000000000);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x4000000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  z = 1.0p-241;
  xui = CONST_VINT128_DW (0xbf0e000000000000, 0x0000000000000000);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x4000000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  z = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y,z);
  e = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  z = -1.0p-111;
  xui = CONST_VINT128_DW (0x3f90000000000000, 0x0000000000000000);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x3fffffffffffffff, 0xfffffffffffffffe);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  z = -1.0p-112;
  xui = CONST_VINT128_DW (0x3f8f000000000000, 0x0000000000000000);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x3fffffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  z = -1.0p-127;
  xui = CONST_VINT128_DW (0x3f80000000000000, 0x0000000000000000);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x3fffffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  z = -1.0p-128;
  xui = CONST_VINT128_DW (0x3f7f000000000000, 0x0000000000000000);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x3fffffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  z = -1.0p-239;
  xui = CONST_VINT128_DW (0x3f10000000000000, 0x0000000000000000);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x3fffffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  z = -1.0p-240;
  xui = CONST_VINT128_DW (0x3f0f000000000000, 0x0000000000000000);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x3fffffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  z = -1.0p-241;
  xui = CONST_VINT128_DW (0x3f0e000000000000, 0x0000000000000000);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x3fffffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  //  y = 0.5Q;
  xui = CONST_VINT128_DW (0x3ffe000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );
  z = vec_xfer_vui64t_2_bin128 ( vf128_none );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0x3fff800000000000, 0x0000000000000000);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  y = 1.0p-112;
  xui = CONST_VINT128_DW (0x3f8f000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0x3fff000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  y = 1.0p-113;
  xui = CONST_VINT128_DW (0x3f8e000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0x3fff000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  y = 1.0p-127;
  xui = CONST_VINT128_DW (0x3f80000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0x3fff000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  y = 1.0p-128;
  xui = CONST_VINT128_DW (0x3f7f000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0x3fff000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  y = 1.0p-240;
  xui = CONST_VINT128_DW (0x3f0f000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = test_xsmsubqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0x3fff000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  z = 1.0p-241;
  xui = CONST_VINT128_DW (0x3f0e000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0x3fff000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  z = 1.0p-256;
  xui = CONST_VINT128_DW (0x3eff000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0x3fff000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  //  y = 0.5Q;
  xui = CONST_VINT128_DW (0x3ffe000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );
  z = vec_xfer_vui64t_2_bin128 ( vf128_one );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0xbffe000000000000, 0x0000000000000000);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  y = 1.0p-112;
  xui = CONST_VINT128_DW (0x3f8f000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0xbffeffffffffffff, 0xfffffffffffffffe);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  y = 1.0p-113;
  xui = CONST_VINT128_DW (0x3f8e000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0xbffeffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  y = 1.0p-127;
  xui = CONST_VINT128_DW (0x3f80000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0xbffeffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  y = 1.0p-128;
  xui = CONST_VINT128_DW (0x3f7f000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0xbffeffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  z = 1.0p-240;
  xui = CONST_VINT128_DW (0x3f0f000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0xbffeffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  z = 1.0p-241;
  xui = CONST_VINT128_DW (0x3f0e000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0xbffeffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  z = 1.0p-256;
  xui = CONST_VINT128_DW (0x3eff000000000000, 0x0000000000000000);
  y = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  //  e = 1.5Q;
  xui = CONST_VINT128_DW (0xbffeffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  y = 1.99999999999998Q;
  xui = CONST_VINT128_DW (0x3fffffffffffffff, 0xffffffffffffffff);
  x = vec_xfer_vui64t_2_bin128 ( xui );
  y = vec_xfer_vui64t_2_bin128 ( xui );
  xui = CONST_VINT128_DW (0x4000ffffffffffff, 0xfffffffffffffffe);
  z = vec_xfer_vui64t_2_bin128 ( xui );
  // z = vec_xfer_vui64t_2_bin128 ( vf128_zero);

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x3f1f000000000000, 0x0000000000000000);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  xui = CONST_VINT128_DW (0x4000ffffffffffff, 0xffffffffffffffff);
  z = vec_xfer_vui64t_2_bin128 ( xui );
  // z = vec_xfer_vui64t_2_bin128 ( vf128_zero);

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0xbf8fffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  y = 1.99999999999998Q;
  xui = CONST_VINT128_DW (0x3fffffffffffffff, 0xffffffffffffffff);
  x = vec_xfer_vui64t_2_bin128 ( xui );
  //  y = 1.99999999999998p-16271Q;
  xui = CONST_VINT128_DW (0x0070ffffffffffff, 0xffffffffffffffff);
  y = vec_xfer_vui64t_2_bin128 ( xui );
  //  z = -1.99999999999998p-16272Q;
  xui = CONST_VINT128_DW (0x0071ffffffffffff, 0xfffffffffffffffe);
  z = vec_xfer_vui64t_2_bin128 ( xui );
  //z = vec_xfer_vui64t_2_bin128 ( vf128_zero);

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x0000000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  y = 1.99999999999998Q;
  xui = CONST_VINT128_DW (0x3fffffffffffffff, 0xffffffffffffffff);
  x = vec_xfer_vui64t_2_bin128 ( xui );
  //  y = 1.99999999999998p-16271Q;
  xui = CONST_VINT128_DW (0x0070ffffffffffff, 0xffffffffffffffff);
  y = vec_xfer_vui64t_2_bin128 ( xui );
  //  z = -1.99999999999999p-16272Q;
  xui = CONST_VINT128_DW (0x0071ffffffffffff, 0xffffffffffffffff);
  z = vec_xfer_vui64t_2_bin128 ( xui );
  //z = vec_xfer_vui64t_2_bin128 ( vf128_zero);

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x8000ffffffffffff, 0xffffffffffffffff);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  y = 1.99999999999998Q;
  xui = CONST_VINT128_DW (0x3fffffffffffffff, 0xffffffffffffffff);
  x = vec_xfer_vui64t_2_bin128 ( xui );
  //  y = 1.99999999999998p-16273Q;
  xui = CONST_VINT128_DW (0x006fffffffffffff, 0xffffffffffffffff);
  y = vec_xfer_vui64t_2_bin128 ( xui );
  //  z = -1.99999999999998p-16272Q;
  xui = CONST_VINT128_DW (0x0070ffffffffffff, 0xfffffffffffffffe);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x0000000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  y = 1.99999999999998Q;
  xui = CONST_VINT128_DW (0x3fffffffffffffff, 0xffffffffffffffff);
  x = vec_xfer_vui64t_2_bin128 ( xui );
  //  y = 1.99999999999998p-16272Q;
  xui = CONST_VINT128_DW (0x006effffffffffff, 0xffffffffffffffff);
  y = vec_xfer_vui64t_2_bin128 ( xui );
  //  z = -1.99999999999998p-16273Q;
  xui = CONST_VINT128_DW (0x006fffffffffffff, 0xfffffffffffffffe);
  z = vec_xfer_vui64t_2_bin128 ( xui );

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x0000000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  // e = vec_xfer_vui64t_2_bin128 ( vf128_three );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //  x = 1.99999999999998Q;
  xui = CONST_VINT128_DW (0x3fffffffffffffff, 0xffffffffffffffff);
  x = vec_xfer_vui64t_2_bin128 ( xui );
  //  y = 0.99999999999998p-16382Q;
  xui = CONST_VINT128_DW (0x0000ffffffffffff, 0xffffffffffffffff);
  y = vec_xfer_vui64t_2_bin128 ( xui );
  //  z = -1.99999999999998p-16382Q;
  xui = CONST_VINT128_DW (0x0001ffffffffffff, 0xfffffffffffffffe);
  z = vec_xfer_vui64t_2_bin128 ( xui );
  //z = vec_xfer_vui64t_2_bin128 ( vf128_zero);

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x8000000000000000, 0x0000000000000001);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //x = 1.41421356237309504880168872420969807856967187537695Q;
  xui = CONST_VINT128_DW (0x3fff6a09e667f3bc, 0xc908b2fb1366ea96);
  x = vec_xfer_vui64t_2_bin128 ( xui );
  //y = 1.41421356237309504880168872420969807856967187537695Q;
  xui = CONST_VINT128_DW (0x3fff6a09e667f3bc, 0xc908b2fb1366ea96);
  y = vec_xfer_vui64t_2_bin128 ( xui );
  //  z = -1.99999999999999Q;
  xui = CONST_VINT128_DW (0x3fffffffffffffff, 0xffffffffffffffff);
  z = vec_xfer_vui64t_2_bin128 ( xui );
  //z = vec_xfer_vui64t_2_bin128 ( vf128_zero);

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x3f9038eb0f40202c, 0x3070852b053f47f2);
  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  //x = 1.41421356237309504880168872420969807856967187537695Q;
  xui = CONST_VINT128_DW (0x3fff6a09e667f3bc, 0xc908b2fb1366ea96);
  x = vec_xfer_vui64t_2_bin128 ( xui );
  //y = 1.41421356237309504880168872420969807856967187537695Q;
  xui = CONST_VINT128_DW (0x3fff6a09e667f3bc, 0xc908b2fb1366ea96);
  y = vec_xfer_vui64t_2_bin128 ( xui );
  //  z = -2.0Q;
  z = vec_xfer_vui64t_2_bin128 ( vf128_two);

#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
  print_vfloat128x(" z=  ", z);
#endif
  t = test_xsmsubqpo (x, y,z);
  xui = CONST_VINT128_DW (0x3f8f71d61e804058, 0x60e10a560a7e8fe4);

  e = vec_xfer_vui64t_2_bin128 ( xui );
  rc += check_f128 ("check vec_xsmsubqpo", e, t, e);

  return (rc);
}
