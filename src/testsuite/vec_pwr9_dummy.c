/*
 Copyright (c) [2017] IBM Corporation.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 vec_pwr9_dummy.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: April 27, 2018
 */

//#pragma GCC push target
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
//#pragma GCC target ("cpu=power9")
#include <stdint.h>
#include <stdio.h>
#include <fenv.h>
#include <float.h>
#ifndef PVECLIB_DISABLE_F128MATH
/* Disable for __clang__ because of bug involving <math.h>
   incombination with -mcpu=power9 -mfloat128 */
#include <math.h>
#endif

#include <pveclib/vec_int128_ppc.h>
#include <pveclib/vec_int512_ppc.h>
#include <pveclib/vec_f128_ppc.h>
#include <pveclib/vec_f64_ppc.h>
#include <pveclib/vec_f32_ppc.h>
#include <pveclib/vec_bcd_ppc.h>

//const __float128 f128_e = 2.71828182845904523536028747135266249775724709369996Q;

__float128
test_scalarLib_exp_128_PWR9 (__float128 term1st, __float128 f128_fact[])
{
  const __float128 f128_one = 1.0Q;
#if 0
  const __float128 f128_fact0 = (1.0Q / 1.0Q);
  const __float128 f128_fact1 = (1.0Q / 2.0Q);
  const __float128 f128_fact2 = (1.0Q / 6.0Q);
  const __float128 f128_fact3 = (1.0Q / 24.0Q);
  const __float128 f128_fact4 = (1.0Q / 120.0Q);
  const __float128 f128_fact5 = (1.0Q / 720.0Q);
  const __float128 f128_fact6 = (1.0Q / 5040.0Q);
  const __float128 f128_fact7 = (1.0Q / 40320.0Q);
#endif
  __float128 term;

#if (__GNUC__ > 6)
  // 1st 8 terms of e = 1 + 1/1! + 1/2!+ 1/3! ...
  term = __builtin_fmaf128 (f128_one, f128_fact[0], term1st);
  term = __builtin_fmaf128 (f128_one, f128_fact[1], term);
  term = __builtin_fmaf128 (f128_one, f128_fact[2], term);
  term = __builtin_fmaf128 (f128_one, f128_fact[3], term);
  term = __builtin_fmaf128 (f128_one, f128_fact[4], term);
  term = __builtin_fmaf128 (f128_one, f128_fact[5], term);
  term = __builtin_fmaf128 (f128_one, f128_fact[6], term);

  return __builtin_fmaf128 (f128_one, f128_fact[7], term);
#else
  __float128 infrac;

  // 1st 8 terms of e = 1 + 1/1! + 1/2!+ 1/3! ...
  infrac = term1st * f128_fact[0];
  term =   term1st + infrac;
  infrac = term1st * f128_fact[1];
  term =   term + infrac;
  infrac = term1st * f128_fact[2];
  term =   term + infrac;
  infrac = term1st * f128_fact[3];
  term =   term + infrac;
  infrac = term1st * f128_fact[4];
  term =   term + infrac;
  infrac = term1st * f128_fact[5];
  term =   term + infrac;
  infrac = term1st * f128_fact[6];
  term =   term + infrac;
  infrac = term1st * f128_fact[7];
  return term + infrac;
#endif
}

vui32_t
test_mask128_f128sign_PWR9(void)
{
  return vec_mask128_f128sign ();
}

vui32_t
test_mask128_f128sign_V0_PWR9(void)
{
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  return signmask;
}

// Attempts at better code to splat small DW constants.
// Want to avoid addr calc and loads for what should be simple
// splat immediate and unpack.
vi64_t
__test_splatisd_12_PWR9 (void)
{
  return vec_splat_s64 (12);
}
vi64_t
__test_splatisd_16_PWR9 (void)
{
  return vec_splat_s64 (-16);
}

vi64_t
__test_splatisd_127_PWR9 (void)
{
  return vec_splat_s64 (127);
}

vi64_t
__test_splatiud_128_PWR9 (void)
{
  return vec_splat_s64 (128);
}

vui64_t
__test_splatiud_127_PWR9 (void)
{
  return vec_splat_u64 (127);
}
vui64_t
__test_splatisd_128_PWR9 (void)
{
  return vec_splat_u64 (128);
}

vui64_t
__test_splatudi_12_PWR9 (void)
{
  return vec_splats ((unsigned long long) 12);
}

// Attempts at better code to splat small QW constants.
// Want to avoid addr calc and loads for what should be simple
// splat immediate and sld.
vi128_t
__test_splatisq_16_PWR9 (void)
{
  return vec_splat_s128 (-16);
}

vi128_t
__test_splatisq_n1_PWR9 (void)
{
  return vec_splat_s128 (-1);
}

vi128_t
__test_splatisq_0_PWR9 (void)
{
  return vec_splat_s128 (0);
}

vi128_t
__test_splatisq_15_PWR9 (void)
{
  return vec_splat_s128 (15);
}

vi128_t
__test_splatisq_127_PWR9 (void)
{
  return vec_splat_s128 (127);
}

vi128_t
__test_splatisq_128_PWR9 (void)
{
  return vec_splat_s128 (128);
}

vui128_t
__test_splatiuq_0_PWR9 (void)
{
  return vec_splat_u128 (0);
}

vui128_t
__test_splatiuq_15_PWR9 (void)
{
  return vec_splat_u128 (15);
}

vui128_t
__test_splatiuq_127_PWR9 (void)
{
  return vec_splat_u128 (127);
}

vui128_t
__test_splatiuq_128_PWR9 (void)
{
  return vec_splat_u128 (128);
}

vui128_t
__test_splatiuq_255_PWR9 (void)
{
  return vec_splat_u128 (255);
}

vui128_t
__test_splatiuq_256_PWR9 (void)
{
  return vec_splat_u128 (256);
}

vui64_t
test_sldi_1_PWR9 (vui64_t a)
{
  return vec_sldi (a, 1);
}

vui64_t
test_sldi_15_PWR9 (vui64_t a)
{
  return vec_sldi (a, 15);
}

vui64_t
test_sldi_16_PWR9 (vui64_t a)
{
  return vec_sldi (a, 16);
}

__binary128
test_negqp_PWR9 (__binary128 vfa)
{
  return vec_negf128 (vfa);
}

__binary128
test_vec_maddqpo_PWR9 (__binary128 vfa, __binary128 vfb, __binary128 vfc)
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
#else
  result = vfa * vfb + vfc;
#endif
  return result;
}
__binary128
test_vec_msubqpo (__binary128 vfa, __binary128 vfb, __binary128 vfc)
{
  __binary128 result;
#if defined (_ARCH_PWR9) && (__GNUC__ > 7)
#if defined (__FLOAT128__) && (__GNUC__ > 8)
  /* There is no __builtin for msubqpo, but the compiler should convert
   * this fmaf128 to xsmsubqpo */
  result = __builtin_fmaf128_round_to_odd (vfa, vfb, vec_negf128 (vfc));
#else
  __asm__(
      "xsmsubqpo %0,%1,%2"
      : "+v" (vfc)
      : "v" (vfa), "v" (vfb)
      : );
  result = vfc;
#endif
  return result;
#else
  __binary128 nsrc3;

  nsrc3 = vec_self128 (vec_negf128 (vfc), vfc, vec_isnanf128(vfc));
  return vec_xsmaddqpo (vfa, vfb, nsrc3);
#endif
}

__binary128
test_mulqpo_PWR9 (__binary128 vfa, __binary128 vfb)
{
  __binary128 result;
#if defined (_ARCH_PWR9) && (__GNUC__ > 6)
#if defined (__FLOAT128__) && (__GNUC__ > 7)
  // Earlier GCC versions may not support this built-in.
  result = __builtin_mulf128_round_to_odd (vfa, vfb);
#else
  // If the compiler supports _ARCH_PWR9, must support mnemonics.
  __asm__(
      "xsmulqpo %0,%1,%2"
      : "=v" (result)
      : "v" (vfa), "v" (vfb)
      : );
#endif
#else
  // Call soft-float runtime
  result = vfa * vfb;
#endif
  return result;
}

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
int
test_gcc_cmpqpne_PWR9 (__float128 vfa, __float128 vfb)
{
  return (vfa != vfb);
}

int
test_gcc_cmpqpeq_PWR9 (__float128 vfa, __float128 vfb)
{
  return (vfa == vfb);
}

int
test_gcc_cmpqpneV2_PWR9 (__float128 vfa, __float128 vfb)
{
  return !(vfa == vfb);
}

void
test_gcc_udqp_f128_PWR9 (__binary128 * vf128,
		    vui64_t vf1, vui64_t vf2,
		    vui64_t vf3, vui64_t vf4,
		    vui64_t vf5)
{
  vf128[0] = vf1[VEC_DW_H];
  vf128[1] = vf1[VEC_DW_L];
  vf128[2] = vf2[VEC_DW_H];
  vf128[3] = vf2[VEC_DW_L];
  vf128[4] = vf3[VEC_DW_H];
  vf128[5] = vf3[VEC_DW_L];
  vf128[6] = vf4[VEC_DW_H];
  vf128[7] = vf4[VEC_DW_L];
  vf128[8] = vf5[VEC_DW_H];
  vf128[9] = vf5[VEC_DW_L];
}

void
test_gcc_dpqp_f128_PWR9 (__binary128 * vf128,
		    vf64_t vf1, vf64_t vf2,
		    vf64_t vf3, vf64_t vf4,
		    vf64_t vf5)
{
  vf128[0] = vf1[VEC_DW_H];
  vf128[1] = vf1[VEC_DW_L];
  vf128[2] = vf2[VEC_DW_H];
  vf128[3] = vf2[VEC_DW_L];
  vf128[4] = vf3[VEC_DW_H];
  vf128[5] = vf3[VEC_DW_L];
  vf128[6] = vf4[VEC_DW_H];
  vf128[7] = vf4[VEC_DW_L];
  vf128[8] = vf5[VEC_DW_H];
  vf128[9] = vf5[VEC_DW_L];
}
#endif

int
test_vec_cmpqp_exp_eq_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_exp_eq ( vfa, vfb);
}

int
test_vec_cmpqp_exp_gt_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_exp_gt ( vfa, vfb);
}

int
test_vec_cmpqp_exp_lt_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_exp_lt ( vfa, vfb);
}

int
test_vec_cmpqp_exp_unordered_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_exp_unordered ( vfa, vfb);
}

int
test_gcc_cmpdpne_PWR9 (double vfa, double vfb)
{
  return (vfa != vfb);
}

int
test_vec_cmpqp_all_tone_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_all_tone (vfa, vfb);
}

int
test_vec_cmpqp_all_uzne_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_all_uzne (vfa, vfb);
}

int
test_vec_cmpqp_all_ne_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_all_ne (vfa, vfb);
}

int
test_vec_cmpqp_all_toeq_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_all_toeq (vfa, vfb);
}

int
test_vec_cmpqp_all_uzeq_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_all_uzeq (vfa, vfb);
}

int
test_vec_cmpqp_all_eq_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_all_eq (vfa, vfb);
}

int
test_scalar_test_neg_PWR9 (__binary128 vfa)
{
  return vec_signbitf128 (vfa);
}

vf64_t
test_vec_xscvqpdpo_PWR9 (__binary128 f128)
{
  return vec_xscvqpdpo (f128);
}

vui64_t
test_vec_xscvqpudz_PWR9 (__binary128 f128)
{
  return vec_xscvqpudz (f128);
}

vui128_t
test_vec_xscvqpuqz_PWR9 (__binary128 f128)
{
  return vec_xscvqpuqz (f128);
}

// Convert Float DP to QP
__binary128
test_vec_xscvdpqp_PWR9 (vf64_t f64)
{
  return vec_xscvdpqp (f64);
}

// Convert Integer QW to QP
__binary128
test_vec_xscvsqqp_PWR9 (vi128_t int128)
{
  return vec_xscvsqqp (int128);
}

__binary128
test_vec_xscvuqqp_PWR9 (vui128_t int128)
{
  return vec_xscvuqqp (int128);
}

// Convert Float DP to QP
__binary128
test_convert_dpqp_PWR9 (vf64_t f64)
{
  __binary128 result;
#if defined (_ARCH_PWR9) && (__GNUC__ > 7)
#if defined (__FLOAT128__) && (__GNUC__ > 9)
  // earlier GCC versions generate extra data moves for this.
  result = f64[VEC_DW_H];
#else
  // No extra data moves here.
  __asm__(
      "xscvdpqp %0,%1"
      : "=v" (result)
      : "v" (f64)
      : );
#endif
#else // likely call to libgcc concert.
#ifndef __clang__
  result = f64[VEC_DW_H];
#endif
#endif
  return result;
}

vf64_t
test_convert_qpdpo_PWR9 (__binary128 f128)
{
  vf64_t result;
#if defined (_ARCH_PWR9) && (__GNUC__ > 7)
#if defined (__FLOAT128__) && (__GNUC__ > 9)
  // GCC runtime does not convert/round directly from __float128 to
  // vector double. So convert scalar double then copy to vector double.
  result = (vf64_t) { 0.0, 0.0 };
  result [VEC_DW_H] = __builtin_truncf128_round_to_odd (f128);
#else
  // No extra data moves here.
  __asm__(
      "xscvqpdpo %0,%1"
      : "=v" (result)
      : "v" (f128)
      : );
#endif
#else //  defined (_ARCH_PWR8)
  vui64_t d_exp, d_sig;
  vui64_t q_exp, q_delta;
  vui128_t q_sig;
  vui32_t q_sign;
  const vui128_t q_zero = { 0 };
//  const vui128_t q_ones = (vui128_t) vec_splat_s32 (-1);
//  const vui64_t exp_low = (vui64_t) CONST_VINT64_DW( (0x3fff), 0 );
  const vui64_t exp_delta = (vui64_t) CONST_VINT64_DW( (0x3fff - 0x3ff), 0 );
  const vui64_t exp_tiny = (vui64_t) CONST_VINT64_DW( (0x3fff-1022), 0 );
  const vui64_t exp_high = (vui64_t) CONST_VINT64_DW( (0x3fff+1023), 0 );
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  const vui64_t q_naninf = (vui64_t) CONST_VINT64_DW( 0x7fff, 0 );
  const vui64_t d_naninf = (vui64_t) CONST_VINT64_DW( 0x7ff, 0 );

  q_exp = vec_xsxexpqp (f128);
  q_sig = vec_xsxsigqp (f128);
  q_sign = vec_and_bin128_2_vui32t (f128, signmask);
  if (__builtin_expect (!vec_cmpuq_all_eq ((vui128_t) q_exp, (vui128_t) q_naninf), 1))
    {
      vui64_t d_X;

      if (vec_cmpuq_all_ge ((vui128_t) q_exp, (vui128_t) exp_tiny))
	{ // Greater than or equal to 2**-1022
	  if (vec_cmpuq_all_lt ((vui128_t) q_exp, (vui128_t) exp_high))
	    { // Less than or equal to 2**+1023
	      // vui64_t d_X;
	      // Convert the significand to double with left shift 4
	      q_sig = vec_slqi ((vui128_t) q_sig, 4);
	      // The GRX round bits are now in bits 64-127 (DW element 1)
	      // For round-to-odd just test for any GRX bits nonzero
	      d_X = (vui64_t) vec_cmpgtud ((vui64_t) q_sig, (vui64_t) q_zero);
	      d_X = vec_mrgald ((vui128_t) d_X, q_zero);
	      d_X = vec_srdi (d_X, 63);
	      d_sig = (vui64_t) vec_or ((vui32_t) q_sig, (vui32_t) d_X);
	      d_exp = vec_subudm (q_exp, exp_delta);
	    }
	  else
	    { // To high so return infinity
	      d_sig = (vui64_t) {0, 0};
	      d_exp = (vui64_t) {0x3ff, 0x3ff};
	    }
	}
      else
	{ // tiny
	  q_delta = vec_subudm (exp_tiny, q_exp);
	  // Convert the significand to double with left shift 4
	  // The GRX round bits are now in bits 64-127 (DW element 1)
	  q_sig = vec_slqi ((vui128_t) q_sig, 4);
	  d_X = (vui64_t) vec_cmpgtud ((vui64_t) q_sig, (vui64_t)q_zero);
	  d_sig = (vui64_t) vec_srq (q_sig, (vui128_t) q_delta);
	  d_exp = (vui64_t) {0, 0};
	}
    }
  else
    { // isinf or isnan.
      d_sig = (vui64_t)vec_slqi (q_sig, 4);
      d_exp = d_naninf;
    }
 // q_exp = vec_swapd (q_exp);
  d_sig [VEC_DW_L] = 0UL;
  d_sig = (vui64_t) vec_or ((vui32_t) d_sig, q_sign);
  result = vec_xviexpdp (d_sig, d_exp);
#endif
  return result;
}


// TBD PWR9 version of qpuqz
// Work in progress, needs a number of things including
vui128_t
test_convert_qpuqz_PWR9 (__binary128 f128)
{
  vui128_t result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 9)
//  vui64_t q_exp, q_delta;
//  vui32_t q_sign;
  const vui128_t q_zero = { 0 };
  const vui128_t q_ones = (vui128_t) vec_splat_s32 (-1);
//  const vui64_t exp_low = (vui64_t) CONST_VINT64_DW( (0x3fff), 0 );
//  const vui64_t exp_high = (vui64_t) CONST_VINT64_DW( (0x3fff+128), 0 );
//  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
//  const vui64_t q_naninf = (vui64_t) CONST_VINT64_DW( 0x7fff, 0 );
  const __binary128 qp_low = 0x1.0p-1;
  const __binary128 qp_high = 0x1.0p+128;

  result = q_zero;
//  q_exp = vec_xsxexpqp (f128);
//  q_sign = vec_and_bin128_2_vui32t (f128, signmask);
  if (__builtin_expect (vec_all_isfinitef128 (f128), 1))
    {
      if (vec_cmpqp_exp_gt (f128, qp_low))
	{ // Greater than or equal to 1.0
	  if (vec_cmpqp_exp_lt (f128, qp_high))
	    { // Less than 2**128-1e
	      const __binary128 qp_shr64 = 0x1.0p-64;
	      const __binary128 qp_shl64 = 0x1.0p+64;
	      __binary128 tmp128, hi128;
	      vui64_t ull_low, ull_high;
	      // TBD xscvqpudz to proceed.
	      tmp128 = f128 * qp_shr64;
	      ull_high = vec_xscvqpudz (tmp128);
	      //hi128 = ull_high [VEC_DW_H];
	      hi128 = vec_xscvudqp (ull_high);
	      hi128 = hi128 * qp_shl64;
	      tmp128 = tmp128 - hi128;
	      ull_low = vec_xscvqpudz (tmp128);
	      result = (vui128_t) vec_mrgahd ((vui128_t) ull_high, (vui128_t) ull_low);
	    }
	  else
	    { // set result to 2**128-1
	      result = (vui128_t) q_ones;
	    }
	}
      else
	{ // less than 1.0
	  result = (vui128_t) q_zero;
	}
    }
  else
    { // isinf or isnan.
      vb128_t is_neg;
      if (vec_all_isnanf128(f128))
        result = q_zero;
      else// is inf
	{
	  is_neg = vec_setb_qp (f128);
	  result = (vui128_t) vec_nor ((vui32_t) is_neg, (vui32_t) is_neg);
	}
      //  else NaN or -Infinity returns zero
    }

#else
  __VEC_U_128 xxx;
  xxx.ui128 = f128;
  result = xxx.vx1;
#endif
  return result;
}

__binary128
test_convert_uqqp_PWR9 (vui128_t int128)
{
  __binary128 result;
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  __asm__(
      "xscvuqqp %0,%1"
      : "=v" (result)
      : "v" (int128)
      : );
#elif defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  vui64_t int64 = (vui64_t) int128;
  __binary128 hi64, lo64;
  __binary128 two64 = 0x1.0p64;
  hi64 = int64[VEC_DW_H];
  lo64 = int64[VEC_DW_L];
  result = (hi64 * two64) + lo64;
#elif  defined (_ARCH_PWR8)
  vui64_t q_exp;
  vui128_t q_sig;
//  vui32_t q_sign;
  const vui128_t q_zero = (vui128_t) { 0 };
//  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);

//  int64[VEC_DW_L] = 0UL; // clear the right most element to zero.
  q_sig = int128;
  // Quick test for 0UL as this case requires a special exponent.
  if (vec_cmpuq_all_eq (q_sig, q_zero))
    {
      result = vec_xfer_vui128t_2_bin128 (q_zero);
    }
  else
    { // We need to produce a normal QP, so we treat the integer like a
      // denormal, then normalize it.
      // Start with the quad exponent bias + 127 then subtract the count of
      // leading '0's. The 64-bit sig can have 0-127 leading '0's.
      vui64_t q_expm = (vui64_t) CONST_VINT64_DW(0, (0x3fff + 127));
      vui64_t i64_clz = (vui64_t) vec_clzq (q_sig);
      q_sig = vec_slq (q_sig, (vui128_t) i64_clz);
      q_exp = vec_subudm (q_expm, i64_clz);
      // This is the part that might require rounding. As is we truncate.
      q_sig = vec_srqi ((vui128_t) q_sig, 15);
      result = vec_xsiexpqp (q_sig, q_exp);
    }
#else
  result = int128[0];
#endif
  return result;
}

// Convert Float SD to QP
__binary128
test_convert_sdqp_PWR9 (vi64_t i64)
{
  __binary128 result;
#if defined (_ARCH_PWR9) && (__GNUC__ > 7)
#if defined (__FLOAT128__) && (__GNUC__ > 9)
  // earlier GCC versions generate extra data moves for this.
  result = i64[VEC_DW_H];
#else
  // No extra data moves here.
  __asm__(
      "xscvsdqp %0,%1"
      : "=v" (result)
      : "v" (i64)
      : );
#endif
#else // likely call to libgcc concert.
#ifndef __clang__
  result = i64[VEC_DW_H];
#endif
#endif
  return result;
}

// Convert Float UD to QP
__binary128
test_convert_udqp_PWR9 (vui64_t i64)
{
  __binary128 result;
#if defined (_ARCH_PWR9) && (__GNUC__ > 7)
#if defined (__FLOAT128__) && (__GNUC__ > 9)
  // earlier GCC versions generate extra data moves for this.
  result = i64[VEC_DW_H];
#else
  // No extra data moves here.
  __asm__(
      "xscvudqp %0,%1"
      : "=v" (result)
      : "v" (i64)
      : );
#endif
#else // likely call to libgcc concert.
#ifndef __clang__
  result = i64[VEC_DW_H];
#endif
#endif
  return result;
}

int
test_scalar_cmpto_exp_gt_PWR9 (__binary128 vfa, __binary128 vfb)
{
#if defined (_ARCH_PWR9) && defined (scalar_cmp_exp_gt) \
  && defined (__FLOAT128__) && (__GNUC__ >= 9)
  return scalar_cmp_exp_gt (vfa, vfb);
#else
  vui32_t vra, vrb;
//  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  const vui32_t expmask = CONST_VINT128_W(0x7fff0000, 0, 0, 0);

  vra = vec_and_bin128_2_vui32t (vfa, expmask);
  vrb = vec_and_bin128_2_vui32t (vfb, expmask);
  return vec_any_gt (vra, vrb);
#endif
}

int
test_scalar_cmp_exp_unordered_PWR9 (__binary128 vfa, __binary128 vfb)
{
#if defined (_ARCH_PWR9) && defined (scalar_cmp_exp_gt) \
  && defined (__FLOAT128__) && (__GNUC__ >= 9)
  return scalar_cmp_exp_unordered (vfa, vfb);
#else
  return (vec_all_isnanf128 (vfa) || vec_all_isnanf128 (vfb));
#endif
}

__binary128
test_vec_max8_f128_PWR9 (__binary128 vf1, __binary128 vf2,
		    __binary128 vf3, __binary128 vf4,
		    __binary128 vf5, __binary128 vf6,
		    __binary128 vf7, __binary128 vf8)
{
  __binary128 maxres;
  vb128_t bool;

  bool = vec_cmpgtuqp (vf2, vf1);
  maxres = vec_self128 (vf1, vf2, bool);
  bool = vec_cmpgtuqp (vf3, maxres);
  maxres = vec_self128 (vf3, maxres, bool);
  bool = vec_cmpgtuqp (vf4, maxres);
  maxres = vec_self128 (vf4, maxres, bool);
  bool = vec_cmpgtuqp (vf5, maxres);
  maxres = vec_self128 (vf5, maxres, bool);
  bool = vec_cmpgtuqp (vf6, maxres);
  maxres = vec_self128 (vf6, maxres, bool);
  bool = vec_cmpgtuqp (vf7, maxres);
  maxres = vec_self128 (vf7, maxres, bool);
  bool = vec_cmpgtuqp (vf8, maxres);
  maxres = vec_self128 (vf8, maxres, bool);

  return maxres;
}

vb128_t
test_vec_cmpnetoqp_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpnetoqp (vfa, vfb);
}

vb128_t
test_vec_cmpneuzqp_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpneuzqp (vfa, vfb);
}

vb128_t
test_vec_cmpneuqp_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpneuqp (vfa, vfb);
}

vb128_t
test_vec_cmpletoqp_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpletoqp (vfa, vfb);
}

vb128_t
test_vec_cmpleuzqp_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpleuzqp (vfa, vfb);
}

vb128_t
test_vec_cmpleuqp_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpleuqp (vfa, vfb);
}

vb128_t
test_vec_cmpgetoqp_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpgetoqp (vfa, vfb);
}

vb128_t
test_vec_cmpgeuzqp_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpgeuzqp (vfa, vfb);
}

vb128_t
test_vec_cmpgeuqp_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpgeuqp (vfa, vfb);
}

vb128_t
test_vec_cmplttoqp_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmplttoqp (vfa, vfb);
}

vb128_t
test_vec_cmpltuzqp_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpltuzqp (vfa, vfb);
}

vb128_t
test_vec_cmpltuqp_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpltuqp (vfa, vfb);
}

vb128_t
test_vec_cmpgttoqp_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpgttoqp (vfa, vfb);
}

vb128_t
test_vec_cmpgtuzqp_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpgtuzqp (vfa, vfb);
}

vb128_t
test_vec_cmpgtuqp_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpgtuqp (vfa, vfb);
}

vb128_t
test_vec_cmpeqtoqp_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpeqtoqp (vfa, vfb);
}

vb128_t
test_vec_cmpequzqp_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpequzqp (vfa, vfb);
}

vb128_t
test_vec_cmpequqp_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpequqp (vfa, vfb);
}

__binary128
test_vec_absf128_PWR9 (__binary128 f128)
{
  return vec_absf128 (f128);
}

__binary128
test_vec_nabsf128_PWR9 (__binary128 f128)
{
  return vec_nabsf128 (f128);
}

__float128
test_absdiff_PWR9 (__float128 vra, __float128 vrb)
{
  __float128 result;
  if (vra > vrb)
    result = vra - vrb;
  else
    result = vrb - vra;
  return result;
}

vf32_t
test_vec_xviexpsp_PWR9 (vui32_t sig, vui32_t exp)
{
  return vec_xviexpsp (sig, exp);
}

vui32_t
test_vec_xvxexpsp_PWR9 (vf32_t f32)
{
  return vec_xvxexpsp (f32);
}

vui32_t
test_vec_xvxsigsp_PWR9 (vf32_t f32)
{
  return vec_xvxsigsp (f32);
}

vf64_t
test_vec_xviexpdp_PWR9 (vui64_t sig, vui64_t exp)
{
  return vec_xviexpdp (sig, exp);
}

vui64_t
test_vec_xvxexpdp_PWR9 (vf64_t f64)
{
  return vec_xvxexpdp (f64);
}

vui64_t
test_vec_xvxsigdp_PWR9 (vf64_t f64)
{
  return vec_xvxsigdp (f64);
}

__binary128
test_sel_bin128_2_bin128_PWR9 (__binary128 vfa, __binary128 vfb, vb128_t mask)
{
  return vec_sel_bin128_2_bin128 (vfa, vfb, mask);
}

vui32_t
test_and_bin128_2_vui32t_PWR9 (__binary128 f128, vui32_t mask)
{
  return vec_and_bin128_2_vui32t (f128, mask);
}

vui32_t
test_andc_bin128_2_vui32t_PWR9 (__binary128 f128, vui32_t mask)
{
  return vec_andc_bin128_2_vui32t (f128, mask);
}

vui32_t
test_xor_bin128_2_vui32t_PWR9 (__binary128 f128, vui32_t mask)
{
  return vec_xor_bin128_2_vui32t (f128, mask);
}

vui32_t
test_xfer_bin128_2_vui32t_V0_PWR9 (__binary128 f128)
{
  __VF_128 vunion;

  vunion.vf1 = f128;
  return vunion.vx4;
}

vui64_t
test_mrgh_bin128_2_vui64t_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_mrgh_bin128_2_vui64t (vfa, vfb) ;
}

vui64_t
test_mrgl_bin128_2_vui64t_PWR9 (__binary128 vfa, __binary128 vfb)
{
  return vec_mrgl_bin128_2_vui64t (vfa, vfb) ;
}

vui32_t
test_xfer_bin128_2_vui32t_PWR9 (__binary128 f128)
{
  return vec_xfer_bin128_2_vui32t (f128);
}

vui128_t
test_xfer_bin128_2_vui128t_PWR9 (__binary128 f128)
{
  return vec_xfer_bin128_2_vui128t (f128);
}

__binary128
test_xfer_vui32t_2_bin128_PWR9 (vui32_t f128)
{
  return vec_xfer_vui32t_2_bin128 (f128);
}

vui64_t
test_vec_xxxexpqpp_PWR9 (__binary128 f128a, __binary128 f128b)
{
  return vec_xxxexpqpp (f128a, f128b);
}

__binary128
test_vec_xsiexpqp_PWR9 (vui128_t sig, vui64_t exp)
{
  return vec_xsiexpqp (sig, exp);
}

vui64_t
test_vec_xsxexpqp_PWR9 (__binary128 f128)
{
  return vec_xsxexpqp (f128);
}

vui128_t
test_vec_xsxsigqp_PWR9 (__binary128 f128)
{
  return vec_xsxsigqp (f128);
}

vb128_t
test_setb_qp_PWR9 (__binary128 f128)
{
  return vec_setb_qp (f128);
}

vb8_t
test_setb_sb_PWR9 (vi8_t vra)
{
  return vec_setb_sb (vra);
}

vb16_t
test_setb_sh_PWR9 (vi16_t vra)
{
  return vec_setb_sh (vra);
}

vb32_t
test_setb_sw_PWR9 (vi32_t vra)
{
  return vec_setb_sw (vra);
}

vb64_t
test_setb_sd_PWR9 (vi64_t vra)
{
  return vec_setb_sd (vra);
}

vui128_t
__test_msumcud_PWR9 (vui64_t a, vui64_t b, vui128_t c)
{
  return vec_msumcud ( a, b, c);
}

vui128_t
__test_cmsumudm_PWR9 (vui128_t * carry, vui64_t a, vui64_t b, vui128_t c)
{
  *carry = vec_msumcud ( a, b, c);
  return vec_msumudm ( a, b, c);
}

#ifndef PVECLIB_DISABLE_F128MATH
#if defined (_ARCH_PWR9) &&(__GNUC__ > 7)

__binary128
test_gcc_max8_f128_PWR9 (__binary128 vf1, __binary128 vf2,
		    __binary128 vf3, __binary128 vf4,
		    __binary128 vf5, __binary128 vf6,
		    __binary128 vf7, __binary128 vf8)
{
  __binary128 maxres = vf1;

  if (vf2 > vf1)
    maxres = vf2;
  if (vf3 > maxres)
    maxres = vf3;
  if (vf4 > maxres)
    maxres = vf4;
  if (vf5 > maxres)
    maxres = vf5;
  if (vf6 > maxres)
    maxres = vf6;
  if (vf7 > maxres)
    maxres = vf7;
  if (vf8 > maxres)
    maxres = vf8;

  return maxres;
}

__float128
test_scalar_add128_PWR9 (__float128 vra, __float128 vrb)
{
  return __builtin_addf128_round_to_odd (vra, vrb);
}

__float128
test_scalar_mul128_PWR9 (__float128 vra, __float128 vrb)
{
  return __builtin_mulf128_round_to_odd (vra, vrb);
}
#endif
#endif

vui64_t
test_vslsudux_PWR9 (unsigned long long int *array, unsigned long offset)
{
  return vec_vlsidx (offset, array);
}

vui64_t
test_vslsudux_c0_PWR9 (unsigned long long int *array)
{
  return vec_vlsidx (0, array);
}

vui64_t
test_vslsudux_c1_PWR9 (unsigned long long int *array)
{
  return vec_vlsidx (8, array);
}

vui64_t
test_vslsudux_c6_PWR9 (unsigned long long int *array)
{
  return vec_vlsidx (6, array);
}

vui64_t
test_vec_lvgudo_PWR9 (unsigned long long int *array, vi64_t vra)
{
  return vec_vgluddo (array, vra);
}

vui64_t
test_vec_lvgudx_PWR9 (unsigned long long int *array, vi64_t vra)
{
  return vec_vgluddx (array, vra);
}

vui64_t
test_vec_lvgudsx_PWR9 (unsigned long long int *array, vi64_t vra)
{
  return vec_vgluddsx (array, vra, 4);
}

void
test_vstsudux_c0_PWR9 (vui64_t data, unsigned long long int *array)
{
  vec_vstsidx (data, 0, array);
}

void
test_vstsudux_c1_PWR9 (vui64_t data, unsigned long long int *array)
{
  vec_vstsidx (data, 8, array);
}

void
test_stvsudo_PWR9 (vui64_t data, unsigned long long int *array, vi64_t vra)
{
  vec_vsstuddo (data, array, vra);
}

void
test_stvsudx_PWR9 (vui64_t data, unsigned long long int *array, vi64_t vra)
{
  vec_vsstuddx (data, array, vra);
}

void
test_stvsudsx_PWR9 (vui64_t data, unsigned long long int *array, vi64_t vra)
{
  vec_vsstuddsx (data, array, vra, 4);
}

vui8_t
__test_absdub_PWR9 (vui8_t __A, vui8_t __B)
{
  return vec_absdub (__A, __B);
}

vui32_t
__test_absduw_PWR9 (vui32_t a, vui32_t b)
{
  return vec_absduw (a, b);
}

vui128_t
__test_msumudm_PWR9 (vui64_t a, vui64_t b, vui128_t c)
{
  return vec_msumudm ( a, b, c);
}

vui128_t
__test_muloud_PWR9 (vui64_t a, vui64_t b)
{
  return vec_muloud (a, b);
}

vui128_t
__test_muleud_PWR9 (vui64_t a, vui64_t b)
{
  return vec_muleud (a, b);
}

vui64_t
__test_mulhud_PWR9 (vui64_t a, vui64_t b)
{
  return vec_mulhud (a, b);
}

vui64_t
__test_muludm_PWR9 (vui64_t a, vui64_t b)
{
  return vec_muludm (a, b);
}

vui128_t
__test_vmuloud_PWR9 (vui64_t a, vui64_t b)
{
  return vec_vmuloud (a, b);
}

vui128_t
__test_vmuleud_PWR9 (vui64_t a, vui64_t b)
{
  return vec_vmuleud (a, b);
}

vui128_t
__test_vmsumoud_PWR9 (vui64_t a, vui64_t b, vui128_t c)
{
  return vec_vmsumoud (a, b, c);
}

vui128_t
__test_vmsumeud_PWR9 (vui64_t a, vui64_t b, vui128_t c)
{
  return vec_vmsumeud (a, b, c);
}

vui128_t
__test_vmaddoud_PWR9 (vui64_t a, vui64_t b, vui64_t c)
{
  return vec_vmaddoud (a, b, c);
}

vui128_t
__test_vmaddeud_PWR9 (vui64_t a, vui64_t b, vui64_t c)
{
  return vec_vmaddeud (a, b, c);
}

vui128_t
__test_vmadd2oud_PWR9 (vui64_t a, vui64_t b, vui64_t c, vui64_t d)
{
  return vec_vmadd2oud (a, b, c, d);
}

vui128_t
__test_vmadd2oud_x_PWR9 (vui64_t a, vui64_t b, vui64_t c, vui64_t d)
{
  /* msumudm provides 2 x DW muliplies and 3 way 128-bit sum of the
   * two products and the addend c.
   * We want the 3 way 128-bit sum of one DW multiply and two DW
   * addends c and d ((A.odd * B.odd) + D.odd + C.odd).
   * So we convert the inputs to compute:
   * (A.odd * B.odd) + (1 * D.odd) + C.odd which executes
   * in the single msumudm.
   */
  const vui64_t one =  CONST_VINT64_DW ( 0, 1 );
  vui64_t admrg = vec_mrgald ((vui128_t) d, (vui128_t) a);
  vui64_t b1mrg = vec_mrgald ((vui128_t) one, (vui128_t) b);
  /* The high (even) DW of one is zero so only 1 QW const is needed.
   * Effectively vec_mrgald ((vui128_t) zero, (vui128_t) c). */
  vui64_t c_oud = vec_pasted ( one, c);
  return vec_msumudm(admrg, b1mrg, (vui128_t) c_oud);
}

vui128_t
__test_vmadd2eud_PWR9 (vui64_t a, vui64_t b, vui64_t c, vui64_t d)
{
  return vec_vmadd2eud (a, b, c, d);
}

vui128_t
__test_vmadd2eud_x_PWR9 (vui64_t a, vui64_t b, vui64_t c, vui64_t d)
{
  /* msumudm provides 2 x DW muliplies and 3 way 128-bit sum of the
   * two products and the addend c.
   * We want the 3 way 128-bit sum of one DW multiply and two DW
   * addends c and d ((A.even * B,even) + D.even + C.even).
   * So we convert the inputs to compute:
   * (A.even * B.even) + (D.even * 1) + C.even which executes
   * in the single msumudm.
   */
  const vui64_t one =  CONST_VINT64_DW ( 0, 1 );
  vui64_t admrg = vec_mrgahd ((vui128_t) a, (vui128_t) d);
  vui64_t b1mrg = vec_pasted ( b, one);
  vui64_t c_eud = vec_mrgahd ((vui128_t) one, (vui128_t) c);
  return vec_msumudm(admrg, b1mrg, (vui128_t) c_eud);
}

vui128_t
__test_vmadd2ud_PWR9 (vui64_t a, vui64_t b, vui64_t c, vui64_t d)
{
  vui128_t e, o;
  e = vec_vmadd2eud (a, b, c, d);
  o = vec_vmadd2oud (a, b, c, d);
  return vec_adduqm (e, o);
}

vui128_t
__test_vmadd2ud_x_PWR9 (vui64_t a, vui64_t b, vui64_t c, vui64_t d)
{
  vui128_t e, o;
  e = __test_vmadd2eud_x_PWR9 (a, b, c, d);
  o = __test_vmadd2oud_x_PWR9 (a, b, c, d);
  return vec_adduqm (e, o);
}

vui128_t
__test_mulhuq_PWR9 (vui128_t a, vui128_t b)
{
  return vec_mulhuq (a, b);
}

vui128_t
__test_mulluq_PWR9 (vui128_t a, vui128_t b)
{
  return vec_mulluq (a, b);
}

vui128_t
__test_muludq_PWR9 (vui128_t *mulh, vui128_t a, vui128_t b)
{
  return vec_muludq (mulh, a, b);
}

vui128_t
__test_muludq_x_PWR9 (vui128_t *mulu, vui128_t a, vui128_t b)
{
  /* compute the 256 bit product of two 128 bit values a, b.
   * The high 128 bits are accumulated in thq and the low 128-bits
   * in tlq. The high 128-bits of the product are returned to the
   * address of the 1st parm. The low 128-bits are the return
   * value.
   */
  const vui64_t zero = { 0, 0 };
  vui64_t a_swap = vec_swapd ((vui64_t) a);
  vui128_t thq, tlq, tx;
  vui128_t t0l, tc1;
  vui128_t thh, thl, tlh, tll;
  /* multiply the low 64-bits of a and b.  For PWR9 this is just
   * vmsumudm with conditioned inputs.  */
  tll = vec_vmuloud ((vui64_t)a, (vui64_t)b);
  thh = vec_vmuleud ((vui64_t)a, (vui64_t)b);
  thl = vec_vmuloud (a_swap, (vui64_t)b);
  tlh = vec_vmuleud (a_swap, (vui64_t)b);
  /* sum the two middle products (plus the high 64-bits of the low
   * product).  This will generate a carry that we need to capture.  */
  t0l   = (vui128_t) vec_mrgahd ( (vui128_t) zero, tll);
  tc1 = vec_addcuq (thl, tlh);
  tx   = vec_adduqm (thl, tlh);
  tx   = vec_adduqm (tx, t0l);
  /* result = t[l] || tll[l].  */
  tlq = (vui128_t) vec_mrgald ((vui128_t) tx, (vui128_t) tll);
  /* Sum the high product plus the high sum (with carry) of middle
   * partial products.  This can't overflow.  */
  thq = (vui128_t) vec_permdi ((vui64_t) tc1, (vui64_t) tx, 2);
  thq = vec_adduqm ( thh, thq);

  *mulu = (vui128_t) thq;
  return ((vui128_t) tlq);
}

vui128_t
__test_muludq_y_PWR9 (vui128_t *mulu, vui128_t a, vui128_t b)
{
  vui32_t t, tmq;
  /* compute the 256 bit product of two 128 bit values a, b.
   * The high 128 bits are accumulated in t and the low 128-bits
   * in tmq. The high 128-bits of the product are returned to the
   * address of the 1st parm. The low 128-bits are the return
   * value.
   */
  vui64_t a_swap = vec_swapd ((vui64_t) a);
  vui128_t tab, tba, tc1;
  /* multiply the low 64-bits of a and b.  For PWR9 this is just
   * vmsumudm with conditioned inputs.  */
  tmq = (vui32_t) vec_vmuloud ((vui64_t)a, (vui64_t)b);
  /* compute the 2 middle partial projects.  Use vmaddeud to add the
   * high 64-bits of the low product to one of the middle products.
   * This can not overflow.  */
  tab = vec_vmuloud (a_swap, (vui64_t) b);
  tba = vec_vmaddeud (a_swap, (vui64_t) b, (vui64_t) tmq);
  /* sum the two middle products (plus the high 64-bits of the low
   * product.  This will generate a carry that we need to capture.  */
  t   = (vui32_t) vec_adduqm (tab, tba);
  tc1 = vec_addcuq (tab, tba);
  /* result = t[l] || tmq[l].  */
  tmq = (vui32_t) vec_mrgald ((vui128_t) t, (vui128_t) tmq);
  /* we can use multiply sum here because the high product plus the
   * high sum of middle partial products can't overflow.  */
  t   = (vui32_t) vec_permdi ((vui64_t) tc1, (vui64_t) t, 2);
  // This is equivalent to vec_vmadd2eud(a, b, tab, tba)
  // were (tab_even + tba_even) was pre-computed including the carry,
  // so no masking is required.
  t   = (vui32_t) vec_vmsumeud ((vui64_t) a, (vui64_t) b, (vui128_t) t);

  *mulu = (vui128_t) t;
  return ((vui128_t) tmq);
}

vui128_t
__test_muludq_z_PWR9 (vui128_t *mulu, vui128_t a, vui128_t b)
{
  vui32_t t, tmq;
  /* compute the 256 bit product of two 128 bit values a, b.
   * The high 128 bits are accumulated in t and the low 128-bits
   * in tmq. The high 128-bits of the product are returned to the
   * address of the 1st parm. The low 128-bits are the return
   * value.
   */
  const vui64_t zero = { 0, 0 };
  vui64_t b_splto = vec_spltd ((vui64_t) b, 1);
  vui64_t b_splte = vec_spltd ((vui64_t) b, 0);
  vui128_t tb0, tb1;
  // multiply a by the odd 64-bits of b.  For PWR9 this is just
  // vmsumudm with conditioned inputs.
  tb1 = vec_vmuloud ((vui64_t)a, (vui64_t)b_splto);
  tb0 = vec_vmuleud ((vui64_t)a, (vui64_t)b_splto);
  // save the low dw of a_low * b_low in tmq
  tmq = (vui32_t) tb1;
  // shift a_odd * b_odd right 64-bits
  tb1 = (vui128_t) vec_mrgahd ((vui128_t) zero, (vui128_t) tb1);
  // Sum (a_even * b_odd) + ((a_odd * b_odd) >> 64))
  t   = (vui32_t) vec_adduqm (tb0, tb1);

  // multiply a by the even 64-bits of b sum with t.
  // For PWR9 this is just vmsumudm with conditioned inputs.
  tb1 = vec_vmaddoud ((vui64_t)a, (vui64_t)b_splte, (vui64_t)t);
  tb0 = vec_vmaddeud ((vui64_t)a, (vui64_t)b_splte, (vui64_t)t);
  // merge low dw of (a_odd * b_even + t_odd) with low dw of (a_low * b_low)
  tmq = (vui32_t) vec_mrgald ((vui128_t) tb1, (vui128_t) tmq);
  // shift a_odd * b_even + t_odd right 64-bits
  tb1 = (vui128_t) vec_mrgahd ((vui128_t) zero, (vui128_t) tb1);
  t   = (vui32_t) vec_adduqm (tb0, tb1);

  *mulu = (vui128_t) t;
  return ((vui128_t) tmq);
}

vui128_t
__test_madduq_PWR9 (vui128_t *mulu, vui128_t a, vui128_t b, vui128_t c)
{
  return vec_madduq (mulu, a, b, c);
}

vui128_t
__test_madduq_x_PWR9 (vui128_t *mulu, vui128_t a, vui128_t b, vui128_t c)
{
  // compute the 256 bit sum of product of two 128 bit values a, b
  // plus the quadword addend c.
  const vui64_t zero = { 0, 0 };
  vui64_t a_swap = vec_swapd ((vui64_t) a);
  vui128_t thq, tlq, tx;
  vui128_t t0l, tc1;
  vui128_t thh, thl, tlh, tll;
  // multiply the low 64-bits of a and b.  For PWR9 this is just
  // vmsumudm with conditioned inputs.
  tll = vec_vmaddoud ((vui64_t)a, (vui64_t)b, (vui64_t)c);
  thh = vec_vmuleud ((vui64_t)a, (vui64_t)b);
  thl = vec_vmuloud (a_swap, (vui64_t)b);
  tlh = vec_vmaddeud (a_swap, (vui64_t)b, (vui64_t)c);
  // sum the two middle products (plus the high 64-bits of the low
  // product).  This will generate a carry that we need to capture.
  t0l = (vui128_t) vec_mrgahd ( (vui128_t) zero, tll);
  tc1 = vec_addcuq (thl, tlh);
  tx  = vec_adduqm (thl, tlh);
  tx  = vec_adduqm (tx, t0l);
  // result = t[l] || tll[l].
  tlq = (vui128_t) vec_mrgald ((vui128_t) tx, (vui128_t) tll);
  // Sum the high product plus the high sum (with carry) of middle
  // partial products.  This can't overflow.
  thq = (vui128_t) vec_permdi ((vui64_t) tc1, (vui64_t) tx, 2);
  thq = vec_adduqm ( thh, thq);

  *mulu = (vui128_t) thq;
  return ((vui128_t) tlq);
}

vui128_t
__test_madduq_y_PWR9 (vui128_t *mulu, vui128_t a, vui128_t b, vui128_t c)
{
  // compute the 256 bit sum of product of two 128 bit values a, b
  // plus the quadword addend c.
  vui64_t a_swap = vec_swapd ((vui64_t) a);
  vui128_t thq, tlq, tx;
  vui128_t t0l, tc1, tcl;
  vui128_t thh, thl, tlh, tll;
  // multiply the four combinations of a_odd/a_even by b_odd/b_even.
  tll = vec_vmuloud ((vui64_t)a, (vui64_t)b);
  thh = vec_vmuleud ((vui64_t)a, (vui64_t)b);
  thl = vec_vmuloud (a_swap, (vui64_t)b);
  tlh = vec_vmuleud (a_swap, (vui64_t)b);
  /* Add c to lower 128-bits of the partial product. */
  tcl = vec_addcuq (tll, c);
  tll = vec_adduqm (tll, c);
  t0l = (vui128_t) vec_permdi ((vui64_t) tcl, (vui64_t) tll, 2);
  // sum the two middle products (plus the high 65-bits of the low
  // product-sum.
  tc1 = vec_addcuq (thl, tlh);
  tx  = vec_adduqm (thl, tlh);
  tx  = vec_adduqm (tx, t0l);
  // result = tx[l]_odd || tll[l]_odd.
  tlq = (vui128_t) vec_mrgald ((vui128_t) tx, (vui128_t) tll);
  // Sum the high product plus the high sum (with carry) of middle
  // partial products.  This can't overflow.
  thq = (vui128_t) vec_permdi ((vui64_t) tc1, (vui64_t) tx, 2);
  thq = vec_adduqm ( thh, thq);

  *mulu = (vui128_t) thq;
  return ((vui128_t) tlq);
}

vui128_t
__test_madd2uq_PWR9 (vui128_t *mulu, vui128_t a, vui128_t b, vui128_t c, vui128_t d)
{
  return vec_madd2uq (mulu, a, b, c, d);
}

vui128_t
__test_mulhluq_PWR9 (vui128_t *mulh, vui128_t a, vui128_t b)
{
  *mulh = vec_mulhuq (a, b);
  return vec_mulluq (a, b);
}

vui128_t
__test_mulhuq2_PWR9 (vui128_t a, vui128_t b)
{
  vui128_t mq;
  vec_muludq (&mq, a, b);
  return mq;
}

vui128_t
__test_muludq_karatsuba_PWR9 (vui128_t *mulu, vui128_t a, vui128_t b)
{
  vui64_t tax, tbx;
  vui128_t tc1;
  vui128_t thh, thl, tlh, tll;
  vui128_t t, tmq;
#if 1
  // Karatsuba Step A, B, C
  thh = vec_vmuleud ((vui64_t) a, (vui64_t) b);
  tll = vec_vmuloud ((vui64_t) a, (vui64_t) b);
  // Step D add a/b across
  tax = vec_swapd ((vui64_t) a);
  tbx = vec_swapd ((vui64_t) b);
  tax = vec_addudm ((vui64_t) a, tax);
  tbx = vec_addudm ((vui64_t) b, tbx);
  // Step E
  thl = vec_vmuloud (tax, tbx);
  // Step F
  tlh = vec_adduqm (thh, tll);
  tlh = vec_subuqm (thl, tlh);
  // Step G
  t = vec_srqi (tlh, 64);
  tmq = vec_slqi (tlh, 64);
  tc1 = vec_addcuq (tmq, tll);
  tmq = vec_adduqm (tmq, tll);
  t = vec_addeuqm (t, thh, tc1);

#else
  /* multiply the low 64-bits of a and b.  For PWR9 this is just
   * vmsumudm with conditioned inputs.  */
  tll = vec_vmuloud ((vui64_t)a, (vui64_t)b);
  thh = vec_vmuleud ((vui64_t)a, (vui64_t)b);
  thl = vec_vmuloud (a_swap, (vui64_t)b);
  tlh = vec_vmuleud (a_swap, (vui64_t)b);
  /* sum the two middle products (plus the high 64-bits of the low
   * product.  This will generate a carry that we need to capture.  */
  t0l   = (vui128_t) vec_mrgahd ( (vui128_t) zero, tll);
  tc1 = vec_addcuq (thl, tlh);
  tx   = vec_adduqm (thl, tlh);
  tx   = vec_adduqm (tx, t0l);
  /* result = t[l] || tll[l].  */
  tlq = (vui128_t) vec_mrgald ((vui128_t) tx, (vui128_t) tll);
  /* Sum the high product plus the high sum (with carry) of middle
   * partial products.  This can't overflow.  */
  thq = (vui128_t) vec_permdi ((vui64_t) tc1, (vui64_t) tx, 2);
  thq = vec_adduqm ( thh, thq);

  t = (vui32_t) thq;
  tmq = (vui32_t) tlq;
#endif

  *mulu = (vui128_t) t;
  return ((vui128_t) tmq);
}

vui128_t
test_vec_cmul10cuq_256_PWR9 (vui128_t *p, vui128_t a, vui128_t a2, vui128_t cin)
{
  vui128_t k, j;
  k = vec_cmul10ecuq (&j, a, cin);
  *p = vec_mul10euq ((vui128_t) a2, j);
  return k;
}

vui128_t
__test_cmul10cuq_PWR9 (vui128_t *cout, vui128_t a)
{
  return vec_cmul10cuq (cout, a);
}

vui128_t
__test_cmul10ecuq_PWR9 (vui128_t *cout, vui128_t a, vui128_t cin)
{
  return vec_cmul10ecuq (cout, a, cin);
}

vui128_t
__test_cmul100cuq_PWR9 (vui128_t *cout, vui128_t a)
{
  return vec_cmul100cuq (cout, a);
}

vui128_t
__test_cmul100ecuq_PWR9 (vui128_t *cout, vui128_t a, vui128_t cin)
{
  return vec_cmul100ecuq (cout, a, cin);
}

vui128_t
__test_mul10uq_c_PWR9 (vui128_t *p, vui128_t a)
{
  *p = vec_mul10uq (a);
  return vec_mul10uq (a);
}

vui128_t
__test_mul10uq_PWR9 (vui128_t a)
{
  return vec_mul10uq (a);
}

vui128_t
__test_mul10euq_PWR9 (vui128_t a, vui128_t cin)
{
  return vec_mul10euq (a, cin);
}

vui128_t
__test_mul10cuq_PWR9 (vui128_t a)
{
  return vec_mul10cuq (a);
}

vui128_t
__test_mul10ecuq_PWR9 (vui128_t a, vui128_t cin)
{
  return vec_mul10ecuq (a, cin);
}

vui64_t
__test_revbd_PWR9 (vui64_t a)
{
  return vec_revbd (a);
}

vui16_t
__test_revbh_PWR9 (vui16_t a)
{
  return vec_revbh (a);
}

vui128_t
__test_revbq_PWR9 (vui128_t a)
{
  return vec_revbq (a);
}

vui32_t
__test_revbw_PWR9 (vui32_t a)
{
  return vec_revbw (a);
}

int
test_vec_all_finitef32_PWR9 (vf32_t value)
{
  return (vec_all_isfinitef32 (value));
}

int
test_vec_all_inff32_PWR9 (vf32_t value)
{
  return (vec_all_isinff32 (value));
}

int
test_vec_all_nanf32_PWR9 (vf32_t value)
{
  return (vec_all_isnanf32 (value));
}

int
test_vec_all_normalf32_PWR9 (vf32_t value)
{
  return (vec_all_isnormalf32 (value));
}

int
test_vec_all_subnormalf32_PWR9 (vf32_t value)
{
  return (vec_all_issubnormalf32 (value));
}

int
test_vec_all_zerof32_PWR9 (vf32_t value)
{
  return (vec_all_iszerof32 (value));
}
int
test_vec_any_finitef32_PWR9 (vf32_t value)
{
  return (vec_any_isfinitef32 (value));
}

int
test_vec_any_inff32_PWR9 (vf32_t value)
{
  return (vec_any_isinff32 (value));
}

int
test_vec_any_nanf32_PWR9 (vf32_t value)
{
  return (vec_any_isnanf32 (value));
}

int
test_vec_any_normalf32_PWR9 (vf32_t value)
{
  return (vec_any_isnormalf32 (value));
}

int
test_vec_any_subnormalf32_PWR9 (vf32_t value)
{
  return (vec_any_issubnormalf32 (value));
}

int
test_vec_any_zerof32_PWR9 (vf32_t value)
{
  return (vec_any_iszerof32 (value));
}

vb32_t
test_vec_isfinitef32_PWR9 (vf32_t value)
{
  return (vec_isfinitef32 (value));
}

vb32_t
test_vec_isinff32_PWR9 (vf32_t value)
{
  return (vec_isinff32 (value));
}

vb32_t
test_vec_isnanf32_PWR9 (vf32_t value)
{
  return (vec_isnanf32 (value));
}

vb32_t
test_vec_isnormalf32_PWR9 (vf32_t value)
{
  return (vec_isnormalf32 (value));
}

vb32_t
test_vec_issubnormalf32_PWR9 (vf32_t value)
{
  return (vec_issubnormalf32 (value));
}

vb32_t
test_vec_iszerof32_PWR9 (vf32_t value)
{
  return (vec_iszerof32 (value));
}

#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
int
test_vec_all_finitef64_PWR9 (vf64_t value)
{
  return (vec_all_isfinitef64 (value));
}

int
test_vec_all_inff64_PWR9 (vf64_t value)
{
  return (vec_all_isinff64 (value));
}

int
test_vec_all_nanf64_PWR9 (vf64_t value)
{
  return (vec_all_isnanf64 (value));
}

int
test_vec_all_normalf64_PWR9 (vf64_t value)
{
  return (vec_all_isnormalf64 (value));
}

int
test_vec_all_subnormalf64_PWR9 (vf64_t value)
{
  return (vec_all_issubnormalf64 (value));
}

int
test_vec_all_zerof64_PWR9 (vf64_t value)
{
  return (vec_all_iszerof64 (value));
}
int
test_vec_any_finitef64_PWR9 (vf64_t value)
{
  return (vec_any_isfinitef64 (value));
}

int
test_vec_any_inff64_PWR9 (vf64_t value)
{
  return (vec_any_isinff64 (value));
}

int
test_vec_any_nanf64_PWR9 (vf64_t value)
{
  return (vec_any_isnanf64 (value));
}

int
test_vec_any_normalf64_PWR9 (vf64_t value)
{
  return (vec_any_isnormalf64 (value));
}

int
test_vec_any_subnormalf64_PWR9 (vf64_t value)
{
  return (vec_any_issubnormalf64 (value));
}

int
test_vec_any_zerof64_PWR9 (vf64_t value)
{
  return (vec_any_iszerof64 (value));
}

vb64_t
test_vec_isfinitef64_PWR9 (vf64_t value)
{
  return (vec_isfinitef64 (value));
}

vb64_t
test_vec_isinff64_PWR9 (vf64_t value)
{
  return (vec_isinff64 (value));
}

vb64_t
test_vec_isnanf64_PWR9 (vf64_t value)
{
  return (vec_isnanf64 (value));
}

vb64_t
test_vec_isnormalf64_PWR9 (vf64_t value)
{
  return (vec_isnormalf64 (value));
}

vb64_t
test_vec_issubnormalf64_PWR9 (vf64_t value)
{
  return (vec_issubnormalf64 (value));
}

vb64_t
test_vec_iszerof64_PWR9 (vf64_t value)
{
  return (vec_iszerof64 (value));
}
#endif

vb128_t
test_vec_isfinitef128_PWR9 (__binary128 f128)
{
  return vec_isfinitef128 (f128);
}

vb128_t
test_vec_isinff128_PWR9 (__binary128 f128)
{
  return vec_isinff128 (f128);
}

vb128_t
test_vec_isnanf128_PWR9 (__binary128 f128)
{
  return vec_isnanf128 (f128);
}

vb128_t
test_vec_isnormalf128_PWR9 (__binary128 f128)
{
  return vec_isnormalf128 (f128);
}

vb128_t
test_vec_issubnormalf128_PWR9 (__binary128 f128)
{
  return vec_issubnormalf128 (f128);
}

vb128_t
test_vec_iszerof128_PWR9 (__binary128 f128)
{
  return vec_iszerof128 (f128);
}

int
test_vec_all_normalf128_PWR9 (__binary128 value)
{
  return (vec_all_isnormalf128 (value));
}

int
test_vec_all_finitef128_PWR9 (__binary128 value)
{
  return (vec_all_isfinitef128 (value));
}

int
test_vec_all_subnormalf128_PWR9 (__binary128 value)
{
  return (vec_all_issubnormalf128 (value));
}

int
test_vec_all_inff128_PWR9 (__binary128 value)
{
  return (vec_all_isinff128 (value));
}

int
test_vec_isinf_signf128_PWR9 (__binary128 value)
{
  return (vec_isinf_signf128 (value));
}

int
test_vec_all_nanf128_PWR9 (__binary128 value)
{
  return (vec_all_isnanf128 (value));
}

int
test_vec_all_zerof128_PWR9 (__binary128 value)
{
  return (vec_all_iszerof128 (value));
}

vf32_t
test_f32_zero_demorm_PWR9 (vf32_t value)
{
  vf32_t result = value;
  vui32_t mask;
  vf32_t fzero = {0.0, 0.0, 0.0, 0.0};

  mask = (vui32_t)vec_issubnormalf32 (value);
  if (vec_any_issubnormalf32 (value))
    {
      result = vec_sel (value, fzero, mask);
    }

  return result;
}

#ifndef PVECLIB_DISABLE_F128MATH
vui32_t
test_fpclassify_f32_PWR9 (vf32_t value)
{
  const vui32_t VFP_NAN =
    { FP_NAN, FP_NAN, FP_NAN, FP_NAN };
  const vui32_t VFP_INFINITE =
    { FP_INFINITE, FP_INFINITE,
    FP_INFINITE, FP_INFINITE };
  const vui32_t VFP_ZERO =
    { FP_ZERO, FP_ZERO, FP_ZERO, FP_ZERO };
  const vui32_t VFP_SUBNORMAL =
    { FP_SUBNORMAL, FP_SUBNORMAL,
    FP_SUBNORMAL, FP_SUBNORMAL };
  const vui32_t VFP_NORMAL =
    { FP_NORMAL, FP_NORMAL, FP_NORMAL,
    FP_NORMAL };
  /* FP_NAN should be 0.  */
  vui32_t result = VFP_NAN;
  vui32_t mask;

  mask = (vui32_t)vec_isinff32 (value);
  result = vec_sel (result, VFP_INFINITE, mask);
  mask = (vui32_t)vec_iszerof32 (value);
  result = vec_sel (result, VFP_ZERO, mask);
  mask = (vui32_t)vec_issubnormalf32 (value);
  result = vec_sel (result, VFP_SUBNORMAL, mask);
  mask = (vui32_t)vec_isnormalf32 (value);
  result = vec_sel (result, VFP_NORMAL, mask);

  return result;
}

void
test_fpclassify_f32loop_PWR9 (vui32_t *out, vf32_t *in, int count)
{
  int i;

  for (i=0; i<count; i++)
    {
      out[i] = test_fpclassify_f32_PWR9 (in[i]);
    }
}
#endif

/* dummy sinf32 example. From Posix:
 * If value is NaN then return a NaN.
 * If value is +-0.0 then return value.
 * If value is subnormal then return value.
 * If value is +-Inf then return a NaN.
 * Otherwise compute and return sin(value).
 */
vf32_t
test_vec_sinf32_PWR9 (vf32_t value)
{
  const vf32_t vec_f0 =
    { 0.0, 0.0, 0.0, 0.0 };
  const vui32_t vec_f32_qnan =
    { 0x7f800001, 0x7fc00000, 0x7fc00000, 0x7fc00000 };
  vf32_t result;
  vb32_t normmask, infmask;

  normmask = vec_isnormalf32 (value);
  if (vec_any_isnormalf32 (value))
    {
      /* replace non-normal input values with safe values.  */
      vf32_t safeval = vec_sel (vec_f0, value, normmask);
      /* body of vec_sin(safeval) computation elided for this example.  */
      result = vec_mul (safeval, safeval);
    }
  else
    result = value;

  /* merge non-normal input values back into result */
  result = vec_sel (value, result, normmask);
  /* Inf input value elements return quiet-nan.  */
  infmask = vec_isinff32 (value);
  result = vec_sel (result, (vf32_t) vec_f32_qnan, infmask);

  return result;
}

/* dummy cosf32 example. From Posix:
 * If value is NaN then return a NaN.
 * If value is +-0.0 then return 1.0.
 * If value is +-Inf then return a NaN.
 * Otherwise compute and return sin(value).
 */
vf32_t
test_vec_cosf32_PWR9 (vf32_t value)
{
  vf32_t result;
  const vf32_t vec_f0 =
    { 0.0, 0.0, 0.0, 0.0 };
  const vf32_t vec_f1 =
    { 1.0, 1.0, 1.0, 1.0 };
  const vui32_t vec_f32_qnan =
    { 0x7f800001, 0x7fc00000, 0x7fc00000, 0x7fc00000 };
  vb32_t finitemask, infmask, zeromask;

  finitemask = vec_isfinitef32 (value);
  if (vec_any_isfinitef32 (value))
    {
      /* replace non-finite input values with safe values.  */
      vf32_t safeval = vec_sel (vec_f0, value, finitemask);
      /* body of vec_sin(safeval) computation elided for this example.  */
      result = vec_mul (safeval, safeval);
    }
  else
    result = value;

  /* merge non-finite input values back into result */
  result = vec_sel (value, result, finitemask);
  /* Set +-0.0 input elements to exactly 1.0 in result.  */
  zeromask = vec_iszerof32 (value);
  result = vec_sel (result, vec_f1, zeromask);
  /* Set Inf input elements to quiet-nan in result.  */
  infmask = vec_isinff32 (value);
  result = vec_sel (result, (vf32_t) vec_f32_qnan, infmask);

  return result;
}

#ifdef __FLOAT128_TYPE__
/* dummy sinf128 example. From Posix:
 * If value is NaN then return a NaN.
 * If value is +-0.0 then return value.
 * If value is subnormal then return value.
 * If value is +-Inf then return a NaN.
 * Otherwise compute and return sin(value).
 */
__binary128
test_sinf128_PWR9 (__binary128 value)
  {
    __binary128 result;
#ifdef __FLOAT128__
    // requires -mfloat128 to use Q const
    const __binary128 zeroF128 = 0.0Q;
#else
    const __binary128 zeroF128 = (__binary128)CONST_VINT128_W(0, 0, 0, 0);
#endif

    if (vec_all_isnormalf128 (value))
      {
	/* body of vec_sin() computation elided for this example.  */
	result = zeroF128;
      }
    else
      {
	if (vec_all_isinff128 (value))
	  result = vec_const_nanf128 ();
	else
	  result = value;
      }

    return result;
  }

/* dummy cosf128 example. From Posix:
 * If value is NaN then return a NaN.
 * If value is +-0.0 then return 1.0.
 * If value is +-Inf then return a NaN.
 * Otherwise compute and return sin(value).
 */
__binary128
test_cosf128_PWR9 (__binary128 value)
  {
    __binary128 result;
#ifdef __FLOAT128__
    // requires -mfloat128 to use Q const
    const __binary128 zeroF128 = 0.0Q;
    const __binary128 oneF128 = 1.0Q;
#else
    const __binary128 zeroF128 = (__binary128)CONST_VINT128_W(0, 0, 0, 0);
    const __binary128 oneF128 = (__binary128)CONST_VINT128_W(0x3fff0000, 0, 0, 0);
#endif

    if (vec_all_isfinitef128 (value))
      {
	if (vec_all_iszerof128 (value))
	  result = oneF128;
	else
	  {
	    /* body of vec_cos() computation elided for this example.  */
            result = zeroF128;
	  }
      }
    else
      {
	if (vec_all_isinff128 (value))
	  result = vec_const_nanf128 ();
	else
	  result = value;
      }

    return result;
  }
#endif

#ifdef vec_cmpne
vb64_t
__test_cmpnedp_PWR9 (vf64_t a, vf64_t b)
{
  return vec_cmpne (a, b);
}

vb64_t
__test_cmpneud_PWR9 (vui64_t a, vui64_t b)
{
  return vec_cmpne (a, b);
}
#endif

#ifdef scalar_test_data_class
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
int
__test_scalar_test_data_class_f128 (__binary128 val)
{
  return scalar_test_data_class (val, 0x7f);
}
#endif
int
__test_scalar_test_data_class_f64 (double val)
{
  return scalar_test_data_class (val, 0x7f);
}

int
__test_scalar_test_data_class_f32 (float val)
{
  return scalar_test_data_class (val, 0x7f);
}
#endif

#ifdef scalar_test_neg
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
int
__test_scalar_test_neg (__binary128 val)
{
  return scalar_test_neg (val);
}
#endif
#endif

#ifdef scalar_extract_exp
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
long long int
__test_scalar_extract_exp_f128 (__binary128 val)
{
  return scalar_extract_exp (val);
}
#endif
int
__test_scalar_extract_exp_f64 (double val)
{
  return scalar_extract_exp (val);
}
#endif

#ifdef scalar_extract_sig
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
__int128
__test_scalar_extract_sig_f128 (__binary128 val)
{
  return scalar_extract_sig (val);
}
#endif
long long int
__test_scalar_extract_sig_f64 (double val)
{
  return scalar_extract_sig (val);
}
#endif

#ifdef scalar_insert_exp
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
__binary128
__test_scalar_insert_exp_f128 (__binary128 sig, unsigned long long int exp)
{
  return scalar_insert_exp (sig, exp);
}
#endif
double
__test_scalar_insert_exp_f64 (double sig, unsigned long long int exp)
{
  return scalar_insert_exp (sig, exp);
}
#endif

#ifdef scalar_cmp_exp_eq
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 8)
/* there is an instruction for this, but is not supported in
   GCC (8.2) yet.  */
int
__test_scalar_cmp_exp_eq_f128 (__binary128 vra, __binary128 vrb)
{
  return scalar_cmp_exp_eq (vra, vrb);
}
#endif
int
__test_scalar_cmp_exp_eq_f64 (double vra, double vrb)
{
  return scalar_cmp_exp_eq (vra, vrb);
}
#endif

#ifdef vec_insert_exp
vf64_t
__test_vec_insert_exp_f64b (vui64_t sig, vui64_t exp)
{
  return vec_insert_exp (sig, exp);
}
vf64_t
__test_vec_insert_exp_f64 (vf64_t sig, vui64_t exp)
{
  return vec_insert_exp (sig, exp);
}

vf32_t
__test_vec_insert_exp_f32b (vui32_t sig, vui32_t exp)
{
  return vec_insert_exp (sig, exp);
}
vf32_t
__test_vec_insert_exp_f32 (vf32_t sig, vui32_t exp)
{
  return vec_insert_exp (sig, exp);
}
#endif

#ifdef vec_test_data_class
vb64_t
__test_vec_test_data_class_f64 (vf64_t val)
{
  return vec_test_data_class (val, 0x7f);
}

vb32_t
__test_vec_test_data_class_f32 (vf32_t val)
{
  return vec_test_data_class (val, 0x7f);
}
#endif

#ifdef vec_extract_exp
vui64_t
__test_vec_extract_exp_f64 (vf64_t val)
{
  return vec_extract_exp (val);
}
vui32_t
__test_vec_extract_exp_f32 (vf32_t val)
{
  return vec_extract_exp (val);
}
#endif

#ifdef vec_extract_sig
vui64_t
__test_vec_extract_sig_f64 (vf64_t val)
{
  return vec_extract_sig (val);
}
vui32_t
__test_vec_extract_sig_f32 (vf32_t val)
{
  return vec_extract_sig (val);
}
#endif

#ifndef PVECLIB_DISABLE_DFP
vBCD_t
test_vec_bcdmulh_PWR9 (vBCD_t a, vBCD_t b)
{
  return vec_bcdmulh (a, b);
}

vBCD_t
test_vec_bcdmul_PWR9 (vBCD_t a, vBCD_t b)
{
  return vec_bcdmul (a, b);
}

vBCD_t
test_vec_cbcdmul_PWR9 (vBCD_t *p, vBCD_t a, vBCD_t b)
{
  return vec_cbcdmul (p, a, b);
}

vi128_t
test_vec_bcdctsq_PWR9 (vBCD_t a)
{
  return (vec_bcdctsq (a));
}

vBCD_t
test_vec_bcdcpsgn_PWR9 (vBCD_t a, vBCD_t b)
{
  return vec_bcdcpsgn (a, b);
}

#if 1
vBCD_t
test_vec_bcds_PWR9 (vBCD_t vra, vi8_t vrb)
{
  return vec_bcds (vra, vrb);
}

vBCD_t
test_vec_bcdsr_PWR9 (vBCD_t vra, vi8_t vrb)
{
  return vec_bcdsr (vra, vrb);
}

vBCD_t
test_vec_bcdsrrqi_PWR9 (vBCD_t vra)
{
  return vec_bcdsrrqi (vra, 15);
}

vBCD_t
test_vec_bcdtrunc_PWR9 (vBCD_t vra, vui16_t vrb)
{
  return vec_bcdtrunc (vra, vrb);
}

vBCD_t
test_vec_bcdtruncqi_PWR9 (vBCD_t vra)
{
  return vec_bcdtruncqi (vra, 15);
}

vBCD_t
test_vec_bcdutrunc_PWR9 (vBCD_t vra, vui16_t vrb)
{
  return vec_bcdutrunc (vra, vrb);
}

vBCD_t
test_vec_bcdutruncqi_PWR9 (vBCD_t vra)
{
  return vec_bcdutruncqi (vra, 15);
}

vBCD_t
test_vec_bcdus_PWR9 (vBCD_t vra, vi8_t vrb)
{
  return vec_bcdus (vra, vrb);
}

vBCD_t
test_vec_bcdsrqi_1_PWR9 (vBCD_t vra)
{
  return vec_bcdsrqi (vra, 1);
}

vBCD_t
test_vec_bcdsrqi_31_PWR9 (vBCD_t vra)
{
  return vec_bcdsrqi (vra, 31);
}

vBCD_t
test_vec_bcdsruqi_1_PWR9 (vBCD_t vra)
{
  return vec_bcdsruqi (vra, 1);
}

vBCD_t
test_vec_bcdsruqi_31_PWR9 (vBCD_t vra)
{
  return vec_bcdsruqi (vra, 31);
}

vBCD_t
test_vec_bcdslqi_1_PWR9 (vBCD_t vra)
{
  return vec_bcdslqi (vra, 1);
}

vBCD_t
test_vec_bcdslqi_31_PWR9 (vBCD_t vra)
{
  return vec_bcdslqi (vra, 31);
}

vBCD_t
test_vec_bcdsluqi_1_PWR9 (vBCD_t vra)
{
  return vec_bcdsluqi (vra, 1);
}

vBCD_t
test_vec_bcdsluqi_31_PWR9 (vBCD_t vra)
{
  return vec_bcdsluqi (vra, 31);
}
#endif


vBCD_t
test_vec_cbcdaddcsq_PWR9 (vBCD_t *c, vBCD_t a, vBCD_t b)
{
  return vec_cbcdaddcsq (c, a, b);
}

vBCD_t
test_vec_cbcdaddcsq2_PWR9 (vBCD_t *c, vBCD_t a, vBCD_t b)
{
  *c = vec_bcdaddcsq (a, b);
  return vec_bcdadd (a, b);
}

vBCD_t
test_vec_cbcdaddecsq_PWR9 (vBCD_t *co, vBCD_t a, vBCD_t b, vBCD_t c)
{
  return vec_cbcdaddecsq (co, a, b, c);
}

vBCD_t
test_vec_cbcdaddecsq2_PWR9 (vBCD_t *co, vBCD_t a, vBCD_t b, vBCD_t c)
{
  *co = vec_bcdaddecsq (a, b, c);
  return vec_bcdaddesqm (a, b, c);
}

vui128_t
test_vec_bcdctuq_PWR9 (vBCD_t vra)
{
  return vec_bcdctuq (vra);
}

vui128_t
example_vec_bcdctuq_PWR9 (vui8_t vra)
{
  vui8_t d100;
  vui16_t d10k;
  vui32_t d100m;
  vui64_t d10e;
  vui128_t result;

  d100 = vec_rdxct100b ((vui8_t) vra);
  d10k = vec_rdxct10kh (d100);
  d100m = vec_rdxct100mw (d10k);
  d10e = vec_rdxct10E16d (d100m);
  return vec_rdxct10e32q (d10e);

  return result;
}

void
example_vec_cbcdecsq_loop_PWR9 (vBCD_t *cout, vBCD_t* out, vBCD_t* a, vBCD_t* b,
				long cnt)
{
  vBCD_t c, cn;
  long i;

  out[cnt - 1] = vec_cbcdaddcsq (&c, a[cnt - 1], b[cnt - 1]);

  cn = _BCD_CONST_ZERO;
  for (i = (cnt - 2); i >= 0; i--)
    {
      out[i] = vec_cbcdaddecsq (&cn, a[i], b[i], c);
      c = cn;
    }
  *cout = cn;
}

vBCD_t
test_vec_bcdaddcsq_PWR9 (vBCD_t a, vBCD_t b)
{
  return vec_bcdaddcsq (a, b);
}

vBCD_t
test_vec_bcdaddesqm_PWR9 (vBCD_t a, vBCD_t b, vBCD_t c)
{
  return vec_bcdaddesqm (a, b, c);
}

vBCD_t
test_vec_bcdaddecsq_PWR9 (vBCD_t a, vBCD_t b, vBCD_t c)
{
  return vec_bcdaddecsq (a, b, c);
}

vBCD_t
test__vec_bcdaddcsq_PWR9 (vBCD_t a, vBCD_t b)
{
  return vec_bcdaddcsq (a, b);
}

vi128_t
test__vec_bcdaddcsq2_PWR9 (vi128_t a, vi128_t b)
{
  vi128_t t;
  t = (vi128_t) _BCD_CONST_ZERO;
  if (__builtin_expect (__builtin_bcdadd_ov ( a, b, 0), 0))
    {
      t = __builtin_bcdadd (a, b, 0);
      t = (vi128_t) vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, (vBCD_t) t);
    }
  return (t);
}

vui64_t
test_vec_rdxct10E16d_PWR9 (vui32_t vra)
{
  return vec_rdxct10E16d (vra);
}

vui128_t
test_vec_rdxct10e32q_PWR9 (vui64_t vra)
{
  return vec_rdxct10e32q (vra);
}

vui32_t
test_vec_rdxcf10E16d_PWR9 (vui64_t vra)
{
  return vec_rdxcf10E16d (vra);
}

vui64_t
test_vec_rdxcf10e32q_PWR9 (vui128_t vra)
{
  return vec_rdxcf10e32q (vra);
}

vBCD_t
test_vec_bcdcfsq_PWR9 (vi128_t vra)
{
  return vec_bcdcfsq (vra);
}

vBCD_t
test_vec_bcdcfuq_PWR9 (vui128_t vra)
{
  return vec_bcdcfuq (vra);
}
#endif

vi128_t
__test_remsq_10e31_PWR9  (vi128_t a, vi128_t b)
{
  vi128_t q;
  q = vec_divsq_10e31 (a);
  return vec_modsq_10e31 (a, q);
}

vui128_t
__test_remuq_10e31_PWR9  (vui128_t a, vui128_t b)
{
  vui128_t q;
  q = vec_divuq_10e31 (a);
  return vec_moduq_10e31 (a, q);
}

vui128_t
__test_remuq_10e32_PWR9  (vui128_t a, vui128_t b)
{
  vui128_t q;
  q = vec_divuq_10e32 (a);
  return vec_moduq_10e32 (a, q);
}

vui128_t
__test_divudq_10e31_PWR9  (vui128_t *qh, vui128_t a, vui128_t b)
{
  return vec_divudq_10e31 (qh, a, b);
}

vui128_t
__test_divudq_10e32_PWR9  (vui128_t *qh, vui128_t a, vui128_t b)
{
  return vec_divudq_10e32 (qh, a, b);
}

vui128_t
__test_remudq_10e31_PWR9  (vui128_t a, vui128_t b)
{
  vui128_t q, qh;
  q = vec_divudq_10e31 (&qh, a, b);
  return vec_modudq_10e31 (a, b, &q);
}

vui128_t
__test_remudq_10e32_PWR9  (vui128_t a, vui128_t b)
{
  vui128_t q, qh;
  q = vec_divudq_10e32 (&qh, a, b);
  return vec_modudq_10e32 (a, b, &q);
}

void
example_qw_convert_decimal_PWR9 (uint64_t *ten_16, vui128_t value)
{
  /* Magic numbers for multiplicative inverse to divide by 10**32
   are 211857340822306639531405861550393824741, corrective add,
   and shift right 107 bits.  */
  const vui128_t mul_invs_ten32 = (vui128_t) CONST_VINT128_DW(
      0x9f623d5a8a732974UL, 0xcfbc31db4b0295e5UL);
  const int shift_ten32 = 107;
  /* Magic numbers for multiplicative inverse to divide by 10**16
   are 76624777043294442917917351357515459181, no corrective add,
   and shift right 51 bits.  */
  const vui128_t mul_invs_ten16 = (vui128_t) CONST_VINT128_DW(
      0x39a5652fb1137856UL, 0xd30baf9a1e626a6dUL);
  const int shift_ten16 = 51;

  const vui128_t mul_ten32 = (vui128_t) (__int128) 100000000000000ll
      * (__int128) 1000000000000000000ll;
  const vui128_t mul_ten16 = (vui128_t) CONST_VINT128_DW(0UL,
							 10000000000000000UL);

  vui128_t tmpq, tmpr, tmpc, tmp;
  __VEC_U_128 tmp_16;

  // First divide/modulo by 10**32 to separate the top 7 digits from
  // the lower 32 digits
  // tmpq = floor ( M * value / 2**128)
  tmpq = vec_mulhuq (value, mul_invs_ten32);
  // Corrective add may overflow, generate carry
  tmpq = vec_adduqm (tmpq, value);
  tmpc = vec_addcuq (tmpq, value);
  // Shift right with carry bit
  tmpq = vec_sldqi (tmpc, tmpq, (128 - shift_ten32));
  // Compute remainder of value / 10**32
  // tmpr = value - (tmpq * 10**32)
  tmp = vec_mulluq (tmpq, mul_ten32);
  tmpr = vec_subuqm (value, tmp);

  // return top 16 digits
  tmp_16.vx1 = tmpq;
  ten_16[0] = tmp_16.ulong.lower;

  // Next divide/modulo the remaining 32 digits by 10**16.
  // This separates the middle and low 16 digits into doublewords.
  tmpq = vec_mulhuq (tmpr, mul_invs_ten16);
  tmpq = vec_srqi (tmpq, shift_ten16);
  // Compute remainder of tmpr / 10**16
  // tmpr = tmpr - (tmpq * 10**16)
  // Here we know tmpq and mul_ten16 are less then 64-bits
  // so can use vec_vmuloud insted of vec_mulluq
  tmp = vec_vmuloud ((vui64_t) tmpq, (vui64_t) mul_ten16);
  tmpr = vec_subuqm (value, tmp);

  // return middle 16 digits
  tmp_16.vx1 = tmpq;
  ten_16[1] = tmp_16.ulong.lower;
  // return low 16 digits
  tmp_16.vx1 = tmpr;
  ten_16[2] = tmp_16.ulong.lower;
}

vui128_t
example_longdiv_10e31_PWR9 (vui128_t *q, vui128_t *d, long int _N)
{
  vui128_t dn, qh, ql, rh;
  long int i;

  // init step for the top digits
  dn = d[0];
  qh = vec_divuq_10e31 (dn);
  rh = vec_moduq_10e31 (dn, qh);
  q[0] = qh;

  // now we know the remainder is less than the divisor.
  for (i=1; i<_N; i++)
    {
      dn = d[i];
      ql = vec_divudq_10e31 (&qh, rh, dn);
      rh = vec_modudq_10e31 (rh, dn, &ql);
      q[i] = ql;
    }

  return rh;
}

#ifndef PVECLIB_DISABLE_DFP
long int
example_longbcdct_10e32_PWR9 (vui128_t *d, vBCD_t decimal, long int _C , long int _N)
{
  /* ten32  = +100000000000000000000000000000000UQ  */
  const vui128_t ten32 = (vui128_t)
	  { (__int128) 10000000000000000UL * (__int128) 10000000000000000UL };
  const vui128_t zero = (vui128_t) { (__int128) 0UL };
  vui128_t dn, ph, pl, cn, c;
  long int i, cnt;

  cnt = _C;

  dn = zero;
  cn  = zero;
  if ( cnt == 0 )
    {
      if (vec_cmpuq_all_ne ((vui128_t) decimal, zero))
	{
	  cnt++;
	  dn = vec_bcdctuq (decimal);
	}

      for ( i = 0; i < (_N - 1); i++ )
	{
	  d[i] = zero;
	}
      d[_N - cnt] = dn;
    }
  else
    {
      if (vec_cmpuq_all_ne ((vui128_t) decimal, zero))
	{
	  dn = vec_bcdctuq (decimal);
	}
      for ( i = (_N - 1); i >= (_N - cnt); i--)
	{
	  pl = vec_muludq (&ph, d[i], ten32);

	  c = vec_addecuq (pl, dn, cn);
	  d[i] = vec_addeuqm (pl, dn, cn);
	  cn = c;
	  dn = ph;
	}
      if (vec_cmpuq_all_ne (dn, zero) || vec_cmpuq_all_ne (cn, zero))
	{
	  cnt++;
	  dn = vec_adduqm (dn, cn);
	  d[_N - cnt] = dn;
	}
    }

  return cnt;
}

long int
example_longbcdct_10e31_PWR9 (vui128_t *d, vBCD_t decimal, long int _C,
			      long int _N)
{
  const vui128_t ten31 = (vui128_t)
	  { (__int128) 1000000000000000UL * (__int128) 10000000000000000UL };
  const vui128_t zero = (vui128_t) { (__int128) 0UL };
  vui128_t dn, ph, pl, cn, c;
  long int i, cnt;

  cnt = _C;

  dn = zero;
  cn = zero;
  if (cnt == 0)
    {
      if (vec_cmpuq_all_ne ((vui128_t) decimal, zero))
	{
	  cnt++;
	  dn = (vui128_t) vec_bcdctsq (decimal);
	}

      for (i = 0; i < (_N - 1); i++)
	{
	  d[i] = zero;
	}
      d[_N - cnt] = dn;
    }
  else
    {
      if (vec_cmpuq_all_ne ((vui128_t) decimal, zero))
	{
	  dn = (vui128_t) vec_bcdctsq (decimal);
	}
      for ( i = (_N - 1); i >= (_N - cnt); i--)
	{
	  pl = vec_muludq (&ph, d[i], ten31);

	  c = vec_addecuq (pl, dn, cn);
	  d[i] = vec_addeuqm (pl, dn, cn);
	  cn = c;
	  dn = ph;
	}
      if (vec_cmpuq_all_ne (dn, zero) || vec_cmpuq_all_ne (cn, zero))
	{
	  cnt++;
	  dn = vec_adduqm (dn, cn);
	  d[_N - cnt] = dn;
	}
    }

  return cnt;
}
#endif

void
test_muluq_4x1_PWR9 (vui128_t *__restrict__ mulu, vui128_t m10, vui128_t m11,
		     vui128_t m12, vui128_t m13, vui128_t m2)
{
  vui128_t mq2, mq1, mq0, mq, mc;
  vui128_t mpx0, mpx1, mpx2, mpx3;
  mpx3 = vec_muludq (&mq2, m13, m2);
  mpx2 = vec_muludq (&mq1, m12, m2);
  mpx2 = vec_addcq (&mc, mpx2, mq2);
  mpx1 = vec_muludq (&mq0, m11, m2);
  mpx1 = vec_addeq (&mc, mpx1, mq1, mc);
  mpx0 = vec_muludq (&mq, m10, m2);
  mpx0 = vec_addeq (&mc, mpx0, mq0, mc);
  mq = vec_adduqm (mc, mq);

  mulu[0] = mq;
  mulu[1] = mpx0;
  mulu[2] = mpx1;
  mulu[3] = mpx2;
  mulu[4] = mpx3;
}

void
test_mul4uq_PWR9 (vui128_t *__restrict__ mulu, vui128_t m1h, vui128_t m1l,
		  vui128_t m2h, vui128_t m2l)
{
  vui128_t mc, mp, mq, mqhl;
  vui128_t mphh, mphl, mplh, mpll;
  mpll = vec_muludq (&mplh, m1l, m2l);
  mp = vec_muludq (&mphl, m1h, m2l);
  mplh = vec_addcq (&mc, mplh, mp);
  mphl = vec_adduqm (mphl, mc);
  mp = vec_muludq (&mqhl, m2h, m1l);
  mplh = vec_addcq (&mq, mplh, mp);
  mphl = vec_addeq (&mc, mphl, mqhl, mq);
  mp = vec_muludq (&mphh, m2h, m1h);
  mphl = vec_addcq (&mq, mphl, mp);
  mphh = vec_addeuqm (mphh, mq, mc);

  mulu[0] = mpll;
  mulu[1] = mplh;
  mulu[2] = mphl;
  mulu[3] = mphh;
}

__VEC_U_1024  __attribute__((flatten ))
__test_madd512x512a512_PWR9 (__VEC_U_512 m1, __VEC_U_512 m2, __VEC_U_512 a1)
{
  __VEC_U_1024 result;
#ifndef __clang__
// Clang does not support register variables.
  register vui128_t t0 asm("vs10");
  register vui128_t t1 asm("vs11");
  register vui128_t t2 asm("vs12");
#else
  vui128_t t0, t1, t2;
#endif
  __VEC_U_512x1 mp3, mp2, mp1, mp0;

  mp0.x640 = vec_madd512x128a512_inline (m1, m2.vx0, a1);
  t0 = mp0.x3.v1x128;
  COMPILE_FENCE;
  mp1.x640 = vec_madd512x128a512_inline (m1, m2.vx1, mp0.x3.v0x512);
  t1 = mp1.x3.v1x128;
  COMPILE_FENCE;
  mp2.x640 = vec_madd512x128a512_inline (m1, m2.vx2, mp1.x3.v0x512);
  t2 = mp2.x3.v1x128;
  COMPILE_FENCE;
  mp3.x640 = vec_madd512x128a512_inline (m1, m2.vx3, mp2.x3.v0x512);

  result.vx0 = t0;
  result.vx1 = t1;
  result.vx2 = t2;
  result.vx3 = mp3.x3.v1x128;
  result.vx4 = mp3.x3.v0x512.vx0;
  result.vx5 = mp3.x3.v0x512.vx1;
  result.vx6 = mp3.x3.v0x512.vx2;
  result.vx7 = mp3.x3.v0x512.vx3;
  return result;
}

void
test_mul128_MN_PWR9 (vui128_t *p, vui128_t *m1, vui128_t *m2, unsigned long M,
		     unsigned long N)
{
  vui128_t *mp = m1;
  vui128_t *np = m2;
  unsigned long mx = M;
  unsigned long nx = N;
  unsigned long i, j;
  vui128_t mpx0, mqx0, mpx1, mqx1;

  /* sizeof(m1) < sizeof(m2) swap the pointers and quadword counts.
   * This allows for early exit when size of either is 1. */
  if (mx < nx)
    {
      vui128_t *xp;
      unsigned long x;

      x = mx;
      xp = mp;
      mx = nx;
      mp = np;
      nx = x;
      np = xp;
    }

  mpx0 = vec_muludq (&mqx0, mp[0], np[0]);
  p[0] = mpx0;
  for (i = 1; i < mx; i++)
    {
      mpx1 = vec_madduq (&mqx1, mp[i], np[0], mqx0);
      p[i] = mpx1;
      mqx0 = mqx1;
    }
  p[mx] = mqx0;

  for (j = 1; j < nx; j++)
    {
      mpx0 = vec_madduq (&mqx0, mp[0], np[j], p[j]);
      p[j] = mpx0;
      for (i = 1; i < mx; i++)
	{
	  mpx1 = vec_madd2uq (&mqx1, mp[i], np[j], mqx0, p[i + j]);
	  p[i + j] = mpx1;
	  mqx0 = mqx1;
	}
      p[mx + j] = mqx0;
    }
}

void __attribute__((flatten ))
#if 0
test_vec_mul1024x1024_PWR9 (__VEC_U_2048* restrict r2048,
				  __VEC_U_1024* restrict m1_1024,
				  __VEC_U_1024* restrict m2_1024)
#else
test_vec_mul1024x1024_PWR9 (__VEC_U_2048* r2048,
				  __VEC_U_1024* m1_1024,
				  __VEC_U_1024* m2_1024)
#endif
{
  __VEC_U_1024x512 subp0, subp1, subp2, subp3;
  __VEC_U_512x1 sum1, sum2, sum3, sumx;
  __VEC_U_512 temp[3];
#if 0
  __VEC_U_1024x512 * restrict pm1 = (__VEC_U_1024x512 *) m1_1024;
  __VEC_U_1024x512 * restrict pm2 = (__VEC_U_1024x512 *) m2_1024;
  __VEC_U_2048x512 * restrict pm2048 = (__VEC_U_2048x512 *) r2048;
#else
  __VEC_U_1024x512 * pm1 = (__VEC_U_1024x512 *) m1_1024;
  __VEC_U_1024x512 * pm2 = (__VEC_U_1024x512 *) m2_1024;
  __VEC_U_2048x512 * pm2048 = (__VEC_U_2048x512 *) r2048;
#endif

  subp0.x1024 = vec_mul512x512_inline (pm1->x2.v0x512, pm2->x2.v0x512);
  pm2048->x4.v0x512 = subp0.x2.v0x512;

  subp1.x1024 = vec_mul512x512_inline (pm1->x2.v1x512, pm2->x2.v0x512);
  sum1.x640 = vec_add512cu (subp1.x2.v0x512, subp0.x2.v1x512);

  temp[0] = sum1.x2.v0x512;
  temp[1] = vec_add512ze (subp1.x2.v1x512, sum1.x2.v1x128);
  COMPILE_FENCE;

  subp2.x1024 = vec_mul512x512_inline (pm1->x2.v0x512, pm2->x2.v1x512);
  sum2.x640 = vec_add512cu (temp[0], subp2.x2.v0x512);
  temp[2] = sum2.x2.v0x512;
  pm2048->x4.v1x512 = temp[2];
  sumx.x640 = vec_add512ecu (temp[1], subp2.x2.v1x512, sum2.x2.v1x128);
  temp[1] = sumx.x2.v0x512;
  COMPILE_FENCE;

  subp3.x1024 = vec_mul512x512_inline (pm1->x2.v1x512, pm2->x2.v1x512);
  sum3.x640 = vec_add512cu (sumx.x2.v0x512, subp3.x2.v0x512);
  pm2048->x4.v2x512 = sum3.x2.v0x512;
  pm2048->x4.v3x512 = vec_add512ze2 (subp3.x2.v1x512, sumx.x2.v1x128,
				     sum3.x2.v1x128);
}
#endif
//#pragma GCC pop target
