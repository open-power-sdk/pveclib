/*
 Copyright (c) [2017, 2018] IBM Corporation.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 arith128_test_f128.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Jul 31, 2017
      Commited on: Oct 10, 2018
 */

#include <stdint.h>
#include <stdio.h>
#include <fenv.h>
#include <float.h>
#ifdef __FLOAT128_TYPE__
#define __STDC_WANT_IEC_60559_TYPES_EXT__ 1
#define __STDC_WANT_IEC_60559_FUNCS_EXT__ 1
#endif
#ifndef PVECLIB_DISABLE_F128MATH
/* Disable <math.h> for __clang__ because of a bug involving <floatn.h>
   incombination with -mcpu=power9 -mfloat128. This means that ISO
   mandated <math.h> functions signbit(), isfinite(), isnormal(),
   isinf(), isnan(), etc are not available for __float128.  */
#include <math.h>
#endif

//#define __DEBUG_PRINT__
#include <pveclib/vec_f128_ppc.h>

int
test_vec_cmpqp_exp_eq (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_exp_eq ( vfa, vfb);
}

int
test_vec_cmpqp_exp_gt (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_exp_gt ( vfa, vfb);
}

int
test_vec_cmpqp_exp_lt (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_exp_lt ( vfa, vfb);
}

int
test_vec_cmpqp_exp_unordered (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_exp_unordered ( vfa, vfb);
}

int
test_vec_cmpqp_all_tone (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_all_tone (vfa, vfb);
}

int
test_vec_cmpqp_all_uzne (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_all_uzne (vfa, vfb);
}

int
test_vec_cmpqp_all_ne (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_all_ne (vfa, vfb);
}

int
test_vec_cmpqp_all_tole (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_all_tole (vfa, vfb);
}

int
test_vec_cmpqp_all_uzle (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_all_uzle (vfa, vfb);
}

int
test_vec_cmpqp_all_le (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_all_le (vfa, vfb);
}

int
test_vec_cmpqp_all_toge (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_all_toge (vfa, vfb);
}

int
test_vec_cmpqp_all_uzge (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_all_uzge (vfa, vfb);
}

int
test_vec_cmpqp_all_ge (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_all_ge (vfa, vfb);
}

int
test_vec_cmpqp_all_tolt (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_all_tolt (vfa, vfb);
}

int
test_vec_cmpqp_all_uzlt (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_all_uzlt (vfa, vfb);
}

int
test_vec_cmpqp_all_lt (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_all_lt (vfa, vfb);
}

int
test_vec_cmpqp_all_togt (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_all_togt (vfa, vfb);
}

int
test_vec_cmpqp_all_uzgt (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_all_uzgt (vfa, vfb);
}

int
test_vec_cmpqp_all_gt (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_all_gt (vfa, vfb);
}

int
test_vec_all_isunorderedf128 (__binary128 vfa, __binary128 vfb)
{
  return vec_all_isunorderedf128 (vfa, vfb);
}

vb128_t
test_vec_isunorderedf128 (__binary128 vfa, __binary128 vfb)
{
  return vec_isunorderedf128 (vfa, vfb);
}

int
test_vec_cmpqp_all_toeq (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_all_toeq (vfa, vfb);
}

int
test_vec_cmpqp_all_uzeq (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_all_uzeq (vfa, vfb);
}

int
test_vec_cmpqp_all_eq (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpqp_all_eq (vfa, vfb);
}

int
test_scalar_test_neg (__binary128 vfa)
{
  return vec_signbitf128 (vfa);
}

vf64_t
test_vec_xscvqpdpo (__binary128 f128)
{
  return vec_xscvqpdpo (f128);
}

vui64_t
test_vec_xscvqpudz (__binary128 f128)
{
  return vec_xscvqpudz (f128);
}

vui128_t
test_vec_xscvqpuqz (__binary128 f128)
{
  return vec_xscvqpuqz (f128);
}

// Convert Float DP to QP
__binary128
test_vec_xscvdpqp (vf64_t f64)
{
  return vec_xscvdpqp (f64);
}

// Convert Integer QW to QP
__binary128
test_vec_xscvsqqp (vi128_t int128)
{
  return vec_xscvsqqp (int128);
}

__binary128
test_vec_xscvuqqp (vui128_t int128)
{
  return vec_xscvuqqp (int128);
}

__binary128
test_convert_uqqpn (vui128_t int128)
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
  const vui128_t q_zero = (vui128_t) { 0 };
  const vui32_t lowmask = CONST_VINT128_W( 0, 0, 0, 1);

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
      // leading '0's. The 128-bit sig can have 0-127 leading '0's.
      vui64_t q_expm = (vui64_t) CONST_VINT64_DW(0, (0x3fff + 127));
      vui64_t i64_clz = (vui64_t) vec_clzq (q_sig);
      q_sig = vec_slq (q_sig, (vui128_t) i64_clz);
      q_exp = vec_subudm (q_expm, i64_clz);
      // This is the part that might require rounding.
#if 1
      // The Significand (including the L-bit) is right justified in
      // are in the high-order 113-bits of q_sig.
      // The guard, round, and sticky (GRX) bits are in the low-order
      // 15 bits.
      // The sticky-bits are the last 13 bits and are logically ORed
      // (or added to 0x1fff) to produce the X-bit.
      //
      // For "round to Nearest, ties to even".
      // GRX = 0b001 - 0b011; truncate
      // GRX = 0b100 and bit-112 is odd; round up, otherwise truncate
      // GRX = 0b100 - 0b111; round up
      // We can simplify by copying bit-112 and OR it with bit-X
      // Then add 0x3fff to q_sig will generate a carry into bit-112
      // if and only if GRX > 0b100 or (GRX == 0b100) && (bit-112 == 1)
      const vui32_t RXmask = CONST_VINT128_W( 0, 0, 0, 0x3fff);
      vui128_t q_carry, q_sigc;
      vb128_t qcmask;
      vui32_t q_odd;
      // Isolate bit-112 and OR into GRX bits if q_sig is odd
      q_odd = (vui32_t) vec_srhi ((vui16_t)q_sig, 15);
      q_odd = vec_and (q_odd, lowmask);
      q_sig = (vui128_t) vec_or ((vui32_t) q_sig, q_odd);
      // We add 0x3fff to GRX which may carry into low order sig-bit
      // This may result in a carry out of bit L into bit-C.
      q_carry = vec_addcuq (q_sig, (vui128_t) RXmask);
      q_sig = vec_adduqm (q_sig, (vui128_t) RXmask);
      // Generate a bool mask from the carry to use in the vsel
      qcmask = vec_setb_cyq (q_carry);
      // Two cases; 1) We did carry so shift (double) left 112 bits
      q_sigc = vec_sldqi (q_carry, q_sig, 112);
      // 2) no carry so shift left 15 bits
      q_sig = vec_srqi ((vui128_t) q_sig, 15);
      // Select which based on carry
      q_sig = (vui128_t) vec_sel ((vui32_t) q_sig, (vui32_t) q_sigc, (vui32_t) qcmask);
      // Increment the exponent based on the carry
      q_exp = vec_addudm (q_exp, (vui64_t) q_carry);
#else
      const vui32_t q_carry = CONST_VINT128_W(0x20000, 0, 0, 0);
      const vui32_t nlmask = CONST_VINT128_W( 0x7fffffff, -1, -1, -1);
      vui32_t q_GRX, q_low;
      vui128_t q_rnd;
      // We need to separate the Significand
      // from the guard, round, and sticky (GRX) bits
      // Left justify the GRX bits
      q_GRX = (vui32_t) vec_slqi ((vui128_t) q_sig, (128-15));
      // Pre-normalize the significand with the L (implicit) bit.
      q_sig = vec_srqi ((vui128_t) q_sig, 15);
      // Separate the low order significand (even/odd) bit.
      q_low = vec_and ((vui32_t)q_sig, lowmask);
      // And merge with sticky bits.
      q_GRX = vec_or (q_GRX, q_low);
      // Use Add write carry to force carry for rounding.
      q_rnd = vec_addcuq ((vui128_t) q_GRX, (vui128_t) nlmask);
      q_sig = vec_addeuqm (q_sig, q_zero,  q_rnd);
      // Check if rounding generated a carry (C-bit) and adjust
      if (vec_all_eq ((vui32_t) q_sig, q_carry))
        {
	  q_sig = vec_srqi ((vui128_t) q_sig, 1);
          q_exp = vec_addudm (q_exp, (vui64_t) lowmask);
        }
#endif
      q_exp = vec_swapd (q_exp);
      result = vec_xsiexpqp (q_sig, q_exp);
    }
#else
  result = int128[0];
#endif
  return result;
}

__binary128
test_convert_uqqpn_V0 (vui128_t int128)
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
  const vui128_t q_zero = (vui128_t) { 0 };
  const vui32_t q_carry = CONST_VINT128_W(0x20000, 0, 0, 0);
  const vui32_t lowmask = CONST_VINT128_W( 0, 0, 0, 1);
  const vui32_t nlmask = CONST_VINT128_W( 0x7fffffff, -1, -1, -1);

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
      // leading '0's. The 128-bit sig can have 0-127 leading '0's.
      vui64_t q_expm = (vui64_t) CONST_VINT64_DW(0, (0x3fff + 127));
      vui64_t i64_clz = (vui64_t) vec_clzq (q_sig);
      vui32_t q_GRX, q_low;
      vui128_t q_rnd;
      q_sig = vec_slq (q_sig, (vui128_t) i64_clz);
      q_exp = vec_subudm (q_expm, i64_clz);
      // This is the part that might require rounding.
      // For "round to Nearest, ties to even".
      // We need to separate the Significand (including the L-bit)
      // from the guard, round, and sticky (GRX) bits
      // Left justify the GRX bits
      q_GRX = (vui32_t) vec_slqi ((vui128_t) q_sig, (128-15));
      // Pre-normalize the significand with the L (implicit) bit.
      q_sig = vec_srqi ((vui128_t) q_sig, 15);
      // Separate the low order significand (even/odd) bit.
      q_low = vec_and ((vui32_t)q_sig, lowmask);
      // And merge with sticky bits.
      q_GRX = vec_or (q_GRX, q_low);
      // Use Add write carry to force carry for rounding.
      q_rnd = vec_addcuq ((vui128_t) q_GRX, (vui128_t) nlmask);
      q_sig = vec_addeuqm (q_sig, q_zero,  q_rnd);
      // Check if rounding generated a carry (C-bit) and adjust
      if (vec_all_eq ((vui32_t) q_sig, q_carry))
        {
	  q_sig = vec_srqi ((vui128_t) q_sig, 1);
          q_exp = vec_addudm (q_exp, (vui64_t) lowmask);
        }
      q_exp = vec_swapd (q_exp);
      result = vec_xsiexpqp (q_sig, q_exp);
    }
#else
  result = int128[0];
#endif
  return result;
}

__binary128
test_convert_uqqpz (vui128_t int128)
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
  const vui128_t q_zero = (vui128_t) { 0 };

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
      // leading '0's. The 128-bit sig can have 0-127 leading '0's.
      vui64_t q_expm = (vui64_t) CONST_VINT64_DW(0, (0x3fff + 127));
      vui64_t i64_clz = (vui64_t) vec_clzq (q_sig);
      q_sig = vec_slq (q_sig, (vui128_t) i64_clz);
      q_exp = vec_subudm (q_expm, i64_clz);
      // This is the part that might require rounding.
      // The simplest is we "round toward zero" (truncate).
      q_sig = vec_srqi ( q_sig, 15 );
      q_exp = vec_swapd (q_exp);
      result = vec_xsiexpqp (q_sig, q_exp);
    }
#else
  result = int128[0];
#endif
  return result;
}

__binary128
test_convert_uqqpo (vui128_t int128)
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
  const vui128_t q_zero = (vui128_t) { 0 };
  const vui32_t oddmask = CONST_VINT128_W(0, 0, 0, 0x7fff);

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
      // leading '0's. The 128-bit sig can have 0-127 leading '0's.
      vui64_t q_expm = (vui64_t) CONST_VINT64_DW(0, (0x3fff + 127));
      vui64_t i64_clz = (vui64_t) vec_clzq (q_sig);
      vui32_t q_odd;

      q_sig = vec_slq (q_sig, (vui128_t) i64_clz);
      q_exp = vec_subudm (q_expm, i64_clz);
      // This is the part that might require rounding.
      // Here we "round to odd". So need to detect if any
      // GRX bits (lower 15-bits) are not zero.
      // And then add with the oddmask will set the odd-bit position.
      q_odd = vec_and ((vui32_t) q_sig, oddmask);
      q_odd = vec_add (q_odd, oddmask);
      // Then or the odd-bit into the low order bit of the significand.
      q_sig = (vui128_t) vec_or  ((vui32_t) q_sig, q_odd);
      // Shift the significand into the correct position for xsiexpqp
      q_sig = vec_srqi ( q_sig, 15 );
      q_exp = vec_swapd (q_exp);
      result = vec_xsiexpqp (q_sig, q_exp);
    }
#else
  result = int128[0];
#endif
  return result;
}

__binary128
test_convert_udqp (vui64_t int64)
{
  __binary128 result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 9)
  result = int64[VEC_DW_H];
#elif  defined (_ARCH_PWR8)
  vui64_t d_sig, q_exp;
  vui128_t q_sig;
  const vui64_t d_zero = (vui64_t) CONST_VINT64_DW( 0, 0 );

  int64[VEC_DW_L] = 0UL; // clear the right most element to zero.int64
  // Quick test for 0UL as this case requires a special exponent.
  if (vec_cmpud_all_eq (int64, d_zero))
    {
      result = vec_xfer_vui64t_2_bin128 (d_zero);
    }
  else
    { // We need to produce a normal QP, so we treat the integer like a
      // denormal, then normalize it.
      // Start with the quad exponent bias + 63 then subtract the count of
      // leading '0's. The 64-bit sig can have 0-63 leading '0's.
      vui64_t q_expm = (vui64_t) CONST_VINT64_DW((0x3fff + 63), 0 );
      vui64_t i64_clz = vec_clzd (int64);
      d_sig = vec_vsld (int64, i64_clz);
      q_exp = vec_subudm (q_expm, i64_clz);
      q_sig = vec_srqi ((vui128_t) d_sig, 15);
      result = vec_xsiexpqp (q_sig, q_exp);
    }
  // Insert exponent into significand to complete conversion to QP
  // result = vec_xsiexpqp (q_sig, q_exp);
#else
  result = int64[VEC_DW_H];
#endif
  return result;
}

__binary128
test_convert_sdqp (vi64_t int64)
{
  __binary128 result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 9)
  result = int64[VEC_DW_H];
#elif  defined (_ARCH_PWR8)
  vui64_t d_sig, q_exp, d_sign, d_inv;
  vui128_t q_sig;
  vui32_t q_sign;
  const vui64_t d_zero = (vui64_t) CONST_VINT64_DW( 0, 0 );
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);

  int64[VEC_DW_L] = 0UL; // clear the right most element to zero.

  if (vec_cmpud_all_eq ((vui64_t) int64, d_zero))
    {
      result = vec_xfer_vui64t_2_bin128 (d_zero);
    }
  else
    {
      q_sign = vec_and ((vui32_t) int64, signmask);
      d_inv  = vec_subudm (d_zero, (vui64_t)int64);
      d_sign = (vui64_t) vec_cmpequd ((vui64_t) q_sign, (vui64_t) signmask);
      d_sig = (vui64_t) vec_sel ((vui32_t) int64, (vui32_t) d_inv, (vui32_t) d_sign);
      // We need to produce a normal QP, so we treat the integer as
      // denormal, Then normalize it.
      // Start with the quad exponent bias + 63 then subtract the count of
      // leading '0's. The 64-bit sig will have at 0-63 leading '0's
      vui64_t q_expm = (vui64_t) CONST_VINT64_DW((0x3fff + 63), 0 );
      vui64_t i64_clz = vec_clzd (d_sig);
      d_sig = vec_vsld (d_sig, i64_clz);
      q_exp = vec_subudm (q_expm, i64_clz);
      q_sig = vec_srqi ((vui128_t) d_sig, 15);
      // Copy Sign-bit to QP significand before insert.
      q_sig = (vui128_t) vec_or ((vui32_t) q_sig, q_sign);
      // Insert exponent into significand to complete conversion to QP
      result = vec_xsiexpqp (q_sig, q_exp);
    }
#else
  result = int64[VEC_DW_H];
#endif
  return result;
}

vui128_t
test_convert_qpuqz (__binary128 f128)
{
  vui128_t result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 9)
  // GCC runtime does not convert directly from F128 to vector _int128
  // So convert to __int128 then xfer via union.
  __VEC_U_128 xxx;
  xxx.ui128 = f128;
  result = xxx.vx1;
#elif  defined (_ARCH_PWR8)
  vui64_t q_exp, q_delta;
  vui128_t q_sig;
  vui32_t q_sign;
  const vui128_t q_zero = { 0 };
  const vui128_t q_ones = (vui128_t) vec_splat_s32 (-1);
  const vui64_t exp_low = (vui64_t) CONST_VINT64_DW( (0x3fff), 0 );
  const vui64_t exp_high = (vui64_t) CONST_VINT64_DW( (0x3fff+128), 0 );
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  const vui64_t q_naninf = (vui64_t) CONST_VINT64_DW( 0x7fff, 0 );

  result = q_zero;
  q_exp = vec_xsxexpqp (f128);
  q_sig = vec_xsxsigqp (f128);
  q_sign = vec_and_bin128_2_vui32t (f128, signmask);
  if (__builtin_expect (!vec_cmpuq_all_eq ((vui128_t) q_exp, (vui128_t) q_naninf), 1))
    {
      if (vec_cmpuq_all_ge ((vui128_t) q_exp, (vui128_t) exp_low))
	{ // Greater than or equal to 1.0
	  if (vec_cmpuq_all_lt ((vui128_t) q_exp, (vui128_t) exp_high))
	    { // Less than 2**128-1
#if 1
	      const vui64_t exp_127 = (vui64_t) CONST_VINT64_DW( (0x3fff+127), 0 );
	      q_sig = vec_slqi ((vui128_t) q_sig, 15);
	      q_delta = vec_subudm (exp_127, q_exp);
	      q_delta = vec_swapd (q_delta);
	      q_sig = vec_srq (q_sig, (vui128_t) q_delta);
#else
	      const vui64_t exp_112 = (vui64_t) CONST_VINT64_DW( (0x3fff+112), 0 );
	      if (vec_cmpuq_all_lt ((vui128_t) q_exp, (vui128_t) exp_112))
		{ // Less than 2**112
		  q_delta = vec_subudm (exp_112, q_exp);
		  q_sig = vec_srq (q_sig, (vui128_t) q_delta);
		}
	      else
		{ // greater then 2**112
		  q_delta = vec_subudm (q_exp, exp_112);
		  q_sig = vec_slq (q_sig, (vui128_t) q_delta);
		}
#endif
	      result = q_sig;
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
      vb128_t is_inf, is_pos;
      is_inf = vec_cmpequq (q_sig, q_zero);
      is_pos = vec_cmpneuq ((vui128_t) q_sign, (vui128_t) signmask);
      result = (vui128_t) vec_and ((vui32_t) is_inf, (vui32_t) is_pos);
      //  else NaN or -Infinity returns zero
    }

#else
  // GCC runtime does not convert directly from F128 to vector _int128
  // So convert to __int128 then xfer via union.
  __VEC_U_128 xxx;
  xxx.ui128 = f128;
  result = xxx.vx1;
#endif
  return result;
}

vf64_t
test_convert_qpdpo_v2 (__binary128 f128)
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
  vui64_t d_exp, d_sig, x_exp;
  vui64_t q_exp;
  vui128_t q_sig;
  vui32_t q_sign;
  const vui128_t q_zero = { 0 };
  const vui128_t q_ones = (vui128_t) vec_splat_s32 (-1);
//  const vui64_t exp_low = (vui64_t) CONST_VINT64_DW( (0x3fff), 0 );
  const vui64_t qpdp_delta = (vui64_t) CONST_VINT64_DW( (0x3fff - 0x3ff), 0 );
  const vui64_t exp_tiny = (vui64_t) CONST_VINT64_DW( (0x3fff - 1022), (0x3fff - 1022) );
  const vui64_t exp_high = (vui64_t) CONST_VINT64_DW( (0x3fff + 1023), (0x3fff + 1023));
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  const vui64_t q_naninf = (vui64_t) CONST_VINT64_DW( 0x7fff, 0x7fff );
  const vui64_t d_naninf = (vui64_t) CONST_VINT64_DW( 0x7ff, 0 );

  q_exp = vec_xsxexpqp (f128);
  x_exp = vec_splatd (q_exp, VEC_DW_H);
  q_sig = vec_xsxsigqp (f128);
  q_sign = vec_and_bin128_2_vui32t (f128, signmask);
  if (__builtin_expect (!vec_cmpud_all_eq (x_exp, q_naninf), 1))
    {
      if (vec_cmpud_all_ge (x_exp, exp_tiny))
	{ // Greater than or equal to 2**-1022
	  if (vec_cmpud_all_le (x_exp, exp_high))
	    { // Less than or equal to 2**+1023
	      vui64_t d_X;
	      // Convert the significand to double with left shift 4
	      q_sig = vec_slqi ((vui128_t) q_sig, 4);
	      // The GRX round bits are now in bits 64-127 (DW element 1)
	      // For round-to-odd just test for any GRX bits nonzero
	      d_X = (vui64_t) vec_cmpgtud ((vui64_t) q_sig, (vui64_t) q_zero);
#if 1
	      d_X = vec_mrgald (q_zero, (vui128_t) d_X);
	      d_X = (vui64_t) vec_slqi ((vui128_t) d_X, 1);
#else
	      d_X = vec_mrgald ((vui128_t) d_X, q_zero);
	      d_X = vec_srdi (d_X, 63);
#endif
	      d_sig = (vui64_t) vec_or ((vui32_t) q_sig, (vui32_t) d_X);
	      d_exp = vec_subudm (q_exp, qpdp_delta);
	    }
	  else
	    { // To high so return infinity OR double max???
	      d_sig = (vui64_t) CONST_VINT64_DW (0x001fffffffffffff, 0);
	      d_exp = (vui64_t) CONST_VINT64_DW (0x7fe, 0);
	    }
	}
      else
	{ // tiny
	  vui64_t d_X;
	  vui64_t q_delta;
	  const vui64_t exp_tinyr = (vui64_t)
	      CONST_VINT64_DW( (0x3fff-(1022+53)), (0x3fff-(1022+53)));
	  q_delta = vec_subudm (exp_tiny, x_exp);
	  // Set double exp to denormal
	  d_exp = (vui64_t) q_zero;
	  if (vec_cmpud_all_gt (x_exp, exp_tinyr))
	    {
	      // Convert the significand to double with left shift 4
	      // The GRX round bits are now in bits 64-127 (DW element 1)
	      q_sig = vec_slqi ((vui128_t) q_sig, 4);
	      d_sig = (vui64_t) vec_srq (q_sig, (vui128_t) q_delta);
	      // For round-to-odd just test for any nonzero GRX bits.
	      d_X = (vui64_t) vec_cmpgtud ((vui64_t) d_sig, (vui64_t) q_zero);
#if 1
	      // Generate a low order 0b1 in DW[0]
	      d_X = vec_mrgald (q_zero, (vui128_t) d_X);
	      d_X = (vui64_t) vec_slqi ((vui128_t) d_X, 1);
#else
	      d_X = vec_mrgald ((vui128_t) d_X, q_zero);
	      d_X = vec_srdi (d_X, 63);
#endif
	      d_sig = (vui64_t) vec_or ((vui32_t) d_sig, (vui32_t) d_X);
	    }
	  else
	    { // tinyr
	      // For round-to-odd just test for any nonzero GRX bits.
	      d_X = (vui64_t) vec_addcuq (q_sig, q_ones);
	      d_sig = (vui64_t) vec_swapd (d_X);
	    }
	}
    }
  else
    { // isinf or isnan.
      const vui64_t q_quiet   = CONST_VINT64_DW(0x0000800000000000, 0);
#if 1
      vb128_t is_inf;
      vui128_t x_sig;
      is_inf = vec_cmpequq ((vui128_t) q_sig, (vui128_t) q_zero);
      x_sig = (vui128_t) vec_or ((vui32_t) q_sig, (vui32_t) q_quiet);
      q_sig = (vui128_t) vec_sel ((vui32_t)x_sig, (vui32_t)q_sig, (vui32_t)is_inf);
#else
      if (vec_cmpuq_all_ne ((vui128_t) q_sig, (vui128_t) q_zero))
	{
	  q_sig = (vui128_t) vec_or ((vui32_t) q_sig, (vui32_t) q_quiet);
	}
#endif
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

vf64_t
test_convert_qpdpo (__binary128 f128)
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
  const vui128_t q_ones = (vui128_t) vec_splat_s32 (-1);
//  const vui64_t exp_low = (vui64_t) CONST_VINT64_DW( (0x3fff), 0 );
  const vui64_t exp_delta = (vui64_t) CONST_VINT64_DW( (0x3fff - 0x3ff), 0 );
  const vui64_t exp_tiny = (vui64_t) CONST_VINT64_DW( (0x3fff - 1022), 0 );
  const vui64_t exp_high = (vui64_t) CONST_VINT64_DW( (0x3fff + 1023), 0 );
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  const vui64_t q_naninf = (vui64_t) CONST_VINT64_DW( 0x7fff, 0 );
  const vui64_t d_naninf = (vui64_t) CONST_VINT64_DW( 0x7ff, 0 );

  q_exp = vec_xsxexpqp (f128);
  q_sig = vec_xsxsigqp (f128);
  q_sign = vec_and_bin128_2_vui32t (f128, signmask);
  if (__builtin_expect (!vec_cmpuq_all_eq ((vui128_t) q_exp, (vui128_t) q_naninf), 1))
    {
      if (vec_cmpuq_all_ge ((vui128_t) q_exp, (vui128_t) exp_tiny))
	{ // Greater than or equal to 2**-1022
	  if (vec_cmpuq_all_le ((vui128_t) q_exp, (vui128_t) exp_high))
	    { // Less than or equal to 2**+1023
	      vui64_t d_X;
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
	    { // To high so return infinity OR double max???
	      d_sig = (vui64_t) CONST_VINT64_DW (0x001fffffffffffff, 0);
	      d_exp = (vui64_t) CONST_VINT64_DW (0x7fe, 0);
	    }
	}
      else
	{ // tiny
	  vui64_t d_X;
	  const vui64_t exp_tinyr = (vui64_t)
	      CONST_VINT64_DW( (0x3fff-(1022+53) ), 0);
	  q_delta = vec_subudm (exp_tiny, q_exp);
	  q_delta = vec_swapd (q_delta);
	  if (vec_cmpuq_all_gt ((vui128_t) q_exp, (vui128_t) exp_tinyr))
	    {
	      // Convert the significand to double with left shift 4
	      // The GRX round bits are now in bits 64-127 (DW element 1)
	      q_sig = vec_slqi ((vui128_t) q_sig, 4);
	      d_sig = (vui64_t) vec_srq (q_sig, (vui128_t) q_delta);
	      d_X = (vui64_t) vec_cmpgtud ((vui64_t) d_sig, (vui64_t) q_zero);
	      d_X = vec_mrgald ((vui128_t) d_X, q_zero);
	      d_X = vec_srdi (d_X, 63);
	      d_sig = (vui64_t) vec_or ((vui32_t) d_sig, (vui32_t) d_X);
	      d_exp = (vui64_t) { 0, 0 };
	    }
	  else
	    { // tinyr
	      d_X = (vui64_t) vec_addcuq (q_sig, q_ones);
	      d_sig = (vui64_t) vec_swapd (d_X);
	      d_exp = (vui64_t) q_zero;
	    }
	}
    }
  else
    { // isinf or isnan.
      if (vec_cmpuq_all_ne ((vui128_t) q_sig, (vui128_t) q_zero))
	{
	  const vui64_t q_quiet   = CONST_VINT64_DW(0x0000800000000000, 0);
	  q_sig = (vui128_t) vec_or ((vui32_t) q_sig, (vui32_t) q_quiet);
	}
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

__binary128
test_convert_dpqp_v3 (vf64_t f64)
{
  __binary128 result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 9)
  result = f64[VEC_DW_H];
#elif  defined (_ARCH_PWR8)
  vui64_t d_exp, d_sig, q_exp;
  vui128_t q_sig;
  vui32_t q_sign;
  const vui64_t exp_delta = (vui64_t) CONST_VINT64_DW( (0x3fff - 0x3ff), 0 );
  const vui64_t d_naninf = (vui64_t) CONST_VINT64_DW( 0x7ff, 0 );
  const vui64_t d_denorm = (vui64_t) CONST_VINT64_DW( 0, 0 );
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);

  f64[VEC_DW_L] = 0.0; // clear the right most element to zero.
  // Extract the exponent, significand, and sign bit.
  d_exp = vec_xvxexpdp (f64);
  d_sig = vec_xvxsigdp (f64);
  q_sign = vec_and ((vui32_t) f64, signmask);
  // The extract sig operation has already tested for finite/subnormal.
  // So avoid testing isfinite/issubnormal again by simply testing
  // the extracted exponent.
  if (__builtin_expect (!vec_cmpud_all_eq (d_exp, d_naninf), 1))
    {
      if (__builtin_expect (!vec_cmpud_all_eq (d_exp, d_denorm), 1))
	{
	  q_sig = vec_srqi ((vui128_t) d_sig, 4);
	  q_exp = vec_addudm (d_exp, exp_delta);
	}
      else
	{ // We can simplify iszero by comparing the sig to 0
	  if (vec_cmpud_all_eq (d_sig, d_denorm))
	    {
	      q_sig = (vui128_t) d_sig;
	      q_exp = (vui64_t) d_exp;
	    }
	  else
	    { // Must be subnormal but we need to produce a normal QP
	      // Need to adjust the quad exponent by the f64 denormal exponent
	      // (-1023) and that the f64 sig will have at least 12 leading '0's
	      vui64_t q_denorm = (vui64_t) CONST_VINT64_DW( (0x3fff - (1023 -12)), 0 );
	      vui64_t f64_clz;
	      //d_sig = vec_sldi (d_sig, 12);
	      f64_clz = vec_clzd (d_sig);
	      d_sig = vec_vsld (d_sig, f64_clz);
	      q_exp = vec_subudm (q_denorm, f64_clz);
	      q_sig = vec_srqi ((vui128_t) d_sig, 15);
	    }
	}
    }
  else
    { // isinf or isnan.
      q_sig = vec_srqi ((vui128_t) d_sig, 4);
      q_exp = (vui64_t) CONST_VINT64_DW(0x7fff, 0);
    }

  // Copy Sign-bit to QP significand before insert.
  q_sig = (vui128_t) vec_or ((vui32_t) q_sig, q_sign);
  // Insert exponent into significand to complete conversion to QP
  result = vec_xsiexpqp (q_sig, q_exp);
#else
  result = f64[VEC_DW_H];
#endif
  return result;
}

__binary128
test_convert_dpqp_v2 (vf64_t f64)
{
  __binary128 result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 9)
  result = f64[VEC_DW_H];
#elif  defined (_ARCH_PWR8)
  f64[VEC_DW_L] = 0.0;
  vui64_t d_exp, d_sig, q_exp;
  vui128_t q_sig;
  const vui64_t exp_delta = (vui64_t) CONST_VINT64_DW( (0x3fff - 0x3ff), 0 );
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);

  d_exp = vec_xvxexpdp (f64);
  d_sig = vec_xvxsigdp (f64);
  if (__builtin_expect (vec_all_isfinitef64 (f64), 1))
    {
      if (__builtin_expect (vec_all_isnormalf64 (vec_splat (f64, VEC_DW_H)), 1))
	{
	  q_sig = vec_srqi ((vui128_t) d_sig, 4);
	  q_exp = vec_addudm (d_exp, exp_delta);
	}
      else
	{
	  if (vec_all_iszerof64 (f64))
	    {
	      q_sig = (vui128_t) d_sig;
	      q_exp = (vui64_t) d_exp;
	    }
	  else
	    { // Must be subnormal
	      vui64_t q_denorm = (vui64_t) CONST_VINT64_DW( (0x3fff - 1023), 0 );
	      vui64_t f64_clz;
	      d_sig = vec_sldi (d_sig, 12);
	      f64_clz = vec_clzd (d_sig);
	      d_sig = vec_sl (d_sig, f64_clz);
	      q_exp = vec_subudm (q_denorm, f64_clz);
	      q_sig = vec_srqi ((vui128_t) d_sig, 15);
	    }
	}
    }
  else
    { // isinf or isnan.
      q_sig = vec_srqi ((vui128_t) d_sig, 4);
      q_exp = (vui64_t) CONST_VINT64_DW(0x7fff, 0);
    }

  q_sig = (vui128_t) vec_sel ((vui32_t) q_sig, (vui32_t) f64, signmask);
  result = vec_xsiexpqp (q_sig, q_exp);
#else
  result = f64[VEC_DW_H];
#endif
  return result;
}

__binary128
test_convert_dpqp (vf64_t f64)
{
  __binary128 result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 9)
  result = f64[VEC_DW_H];
#elif  defined (_ARCH_PWR8)
  f64[VEC_DW_L] = 0.0;
  vui64_t d_exp, d_sig, q_exp;
  vui128_t q_sig;
  const vui64_t exp_delta = {(0x3fff - 0x3ff), (0x3fff - 0x3ff)};

  d_exp = vec_xvxexpdp (f64);
  d_sig = vec_xvxsigdp (f64);
  if (vec_any_isnormalf64 (vec_splat (f64, VEC_DW_H)))
    {
      q_sig = vec_srqi ((vui128_t) d_sig, 4);
      q_exp = vec_addudm (d_exp, exp_delta);
    } else {
	if (vec_all_iszerof64 (vec_splat (f64, VEC_DW_H)))
	  {
	    q_sig = (vui128_t) d_sig;
	    q_exp = (vui64_t) d_exp;
	  }
	else
	  {
	    if (vec_all_issubnormalf64 (vec_splat (f64, VEC_DW_H)))
	      {
		vui64_t q_denorm = { (0x3fff - 1023), 0 };
		vui64_t f64_clz;
		d_sig = vec_sldi ( d_sig, 12);
		f64_clz = vec_clzd (d_sig);
		d_sig = vec_sl (d_sig, f64_clz);
		q_exp = vec_subudm (q_denorm, f64_clz);
		q_sig = vec_srqi ((vui128_t) d_sig, 15);
	      } else {
		q_sig = vec_srqi ((vui128_t) d_sig, 4);
		q_exp = (vui64_t) CONST_VINT64_DW(0x7fff, 0);
	      }
	  }
    }
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);

  q_sig = (vui128_t) vec_sel ((vui32_t) q_sig, (vui32_t) f64, signmask);
  result = vec_xsiexpqp (q_sig, q_exp);
#else
  result = f64[VEC_DW_H];
#endif
  return result;
}

int
test_scalar_cmpto_exp_gt (__binary128 vfa, __binary128 vfb)
{
#if defined (_ARCH_PWR9) && defined (scalar_cmp_exp_gt) && defined (__FLOAT128__) && (__GNUC__ > 9)
  return scalar_cmp_exp_gt (vfa, vfb);
#else
  vui32_t vra, vrb;
  const vui32_t expmask = CONST_VINT128_W(0x7fff0000, 0, 0, 0);

  vra = vec_and_bin128_2_vui32t (vfa, expmask);
  vrb = vec_and_bin128_2_vui32t (vfb, expmask);
  return vec_any_gt (vra, vrb);
#endif
}

int
test_scalar_cmp_exp_gt (__binary128 vfa, __binary128 vfb)
{
#if defined (_ARCH_PWR9) && defined (scalar_cmp_exp_gt) && defined (__FLOAT128__) && (__GNUC__ > 9)
  return scalar_cmp_exp_gt (vfa, vfb);
#else
  vui32_t vra, vrb;
  const vui32_t expmask = CONST_VINT128_W(0x7fff0000, 0, 0, 0);

  if (__builtin_expect ((vec_all_isnanf128 (vfa) || vec_all_isnanf128 (vfb)), 0))
    return 0;

  vra = vec_and_bin128_2_vui32t (vfa, expmask);
  vrb = vec_and_bin128_2_vui32t (vfb, expmask);
  return vec_any_gt (vra, vrb);
#endif
}

int
test_scalar_cmp_exp_unordered (__binary128 vfa, __binary128 vfb)
{
#if defined (_ARCH_PWR9) && defined (scalar_cmp_exp_gt) && defined (__FLOAT128__) && (__GNUC__ > 9)
  return scalar_cmp_exp_unordered (vfa, vfb);
#else
  return (vec_all_isnanf128 (vfa) || vec_all_isnanf128 (vfb));
#endif
}

vb128_t
test_bool_cmp_exp_unordered (__binary128 vfa, __binary128 vfb)
{
  return (vb128_t) vec_or ((vui32_t) vec_isnanf128 (vfa),
			   (vui32_t) vec_isnanf128 (vfb));
}

vb128_t
test_bool_cmp_exp_unordered_v2 (__binary128 vfa, __binary128 vfb)
{
  vb128_t result;

  result = (vb128_t) vec_splat_u32 (0);
  if (vec_all_isnanf128 (vfa) || vec_all_isnanf128 (vfb))
    result = (vb128_t) vec_splat_s32 (-1);

  return result;
}

__binary128
test_sel_bin128_2_bin128 (__binary128 vfa, __binary128 vfb, vb128_t mask)
{
  return vec_sel_bin128_2_bin128 (vfa, vfb, mask);
}

vui32_t
test_and_bin128_2_vui32t (__binary128 f128, vui32_t mask)
{
  return vec_and_bin128_2_vui32t (f128, mask);
}

vui32_t
test_andc_bin128_2_vui32t (__binary128 f128, vui32_t mask)
{
  return vec_andc_bin128_2_vui32t (f128, mask);
}

vui32_t
test_xor_bin128_2_vui32t (__binary128 f128, vui32_t mask)
{
  return vec_xor_bin128_2_vui32t (f128, mask);
}

vui32_t
test_xfer_bin128_2_vui32t (__binary128 f128)
{
  return vec_xfer_bin128_2_vui32t (f128);
}

vui128_t
test_xfer_bin128_2_vui128t (__binary128 f128)
{
  return vec_xfer_bin128_2_vui128t (f128);
}

__binary128
test_xfer_vui32t_2_bin128 (vui32_t f128)
{
  return vec_xfer_vui32t_2_bin128 (f128);
}

vb128_t
test_cmpltf128_v1 (vi128_t vfa128, vi128_t vfb128)
{
  const vui32_t zero = CONST_VINT128_W(0, 0, 0, 0);
  vb128_t age0, altb, alt0, agtb;
  vui32_t andp, andn;
  vb128_t result;
  age0 = vec_cmpgesq (vfa128, (vi128_t) zero);
  altb = vec_cmpltsq (vfa128, vfb128);
  andp = vec_and ((vui32_t) age0, (vui32_t) altb);
  alt0 = vec_cmpltsq (vfa128, (vi128_t) zero);
  agtb = vec_cmpgeuq ((vui128_t) vfa128, (vui128_t) vfb128);
  andn = vec_and ((vui32_t) alt0, (vui32_t) agtb);
  result = (vb128_t) vec_or (andp, andn);
  return result;
}

vb128_t
test_cmpltf128_v1b (vi128_t vfa128, vi128_t vfb128)
{
  const vui32_t zero = CONST_VINT128_W(0, 0, 0, 0);
  vb128_t age0, altb, agtb;
  vui32_t andp, andn;
  vb128_t result;
  age0 = vec_cmpgesq (vfa128, (vi128_t) zero);
  altb = vec_cmpltsq (vfa128, vfb128);
  andp = vec_and ((vui32_t) altb, (vui32_t) age0);
  agtb = vec_cmpgeuq ((vui128_t) vfa128, (vui128_t) vfb128);
  andn = vec_andc ((vui32_t) agtb, (vui32_t) age0);
  result = (vb128_t) vec_or (andp, andn);
  return result;
}

vb128_t
test_cmpltf128_v1c (vi128_t vfa128, vi128_t vfb128)
{
  vb128_t altb, agtb;
  vb128_t signbool;

  // a >= 0
  // signbool = vec_setb_qp;
  const vui8_t shift = vec_splat_u8 (7);
  vui8_t splat = vec_splat ((vui8_t) vfa128, VEC_BYTE_H);
  signbool = (vb128_t) vec_sra (splat, shift);

  altb = vec_cmpltsq (vfa128, vfb128);
  agtb = vec_cmpgtuq ((vui128_t) vfa128, (vui128_t) vfb128);
  return (vb128_t) vec_sel ((vui32_t)agtb, (vui32_t)altb, (vui32_t)signbool);
}

vb128_t
test_cmpltf128_v2 (vi128_t vfa128, vi128_t vfb128)
{
  const vui32_t zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vb128_t age0, altb, alt0, agtb, nesm;
  vui32_t andp, andn, or_ab;
  vb128_t result;
  age0 = vec_cmpgesq (vfa128, (vi128_t) zero);
  altb = vec_cmpltsq (vfa128, vfb128);
  andp = vec_and ((vui32_t) age0, (vui32_t) altb);
  alt0 = vec_cmpltsq (vfa128, (vi128_t) zero);
  agtb = vec_cmpgeuq ((vui128_t) vfa128, (vui128_t) vfb128);
  andn = vec_and ((vui32_t) alt0, (vui32_t) agtb);

  or_ab = vec_or ((vui32_t) vfa128, (vui32_t) vfb128 );
  nesm = vec_cmpneuq ((vui128_t) or_ab, (vui128_t) signmask);
  andn = vec_and ((vui32_t) andn, (vui32_t) nesm);

  result = (vb128_t) vec_or (andp, andn);
  return result;
}

vb128_t
test_cmpltf128_v2b (vi128_t vfa128, vi128_t vfb128)
{
  const vui32_t zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vb128_t age0, altb, alt0, agtb, nesm;
  vui32_t andp, andn, or_ab;
  vb128_t result;
  age0 = vec_cmpgesq (vfa128, (vi128_t) zero);
  altb = vec_cmpltsq (vfa128, vfb128);
  andp = vec_and ((vui32_t) age0, (vui32_t) altb);
  alt0 = vec_cmpltsq (vfa128, (vi128_t) zero);
  agtb = vec_cmpgeuq ((vui128_t) vfa128, (vui128_t) vfb128);
  andn = vec_and ((vui32_t) alt0, (vui32_t) agtb);

  or_ab = vec_or ((vui32_t) vfa128, (vui32_t) vfb128 );
  nesm = vec_cmpequq ((vui128_t) or_ab, (vui128_t) signmask);
  andn = vec_andc ((vui32_t) andn, (vui32_t) nesm);

  result = (vb128_t) vec_or (andp, andn);
  return result;
}

vb128_t
test_cmpltf128_v2c (vi128_t vfa128, vi128_t vfb128)
{
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vb128_t altb, agtb, nesm;
  vui32_t or_ab;
  vb128_t signbool;

  // a >= 0
  // signbool = vec_setb_qp;
  const vui8_t shift = vec_splat_u8 (7);
  vui8_t splat = vec_splat ((vui8_t) vfa128, VEC_BYTE_H);
  signbool = (vb128_t) vec_sra (splat, shift);

  altb = vec_cmpltsq (vfa128, vfb128);
  agtb = vec_cmpgtuq ((vui128_t) vfa128, (vui128_t) vfb128);

  or_ab = vec_or ((vui32_t) vfa128, (vui32_t) vfb128 );
  // For ne compare eq then and compliment
  nesm = vec_cmpequq ((vui128_t) or_ab, (vui128_t) signmask);
  agtb = (vb128_t) vec_andc ((vui32_t) agtb, (vui32_t) nesm);

  return (vb128_t) vec_sel ((vui32_t)agtb, (vui32_t)altb, (vui32_t)signbool);
}

vb128_t
test_cmpltf128_v3 (vi128_t vfa128, vi128_t vfb128)
{
  const vui32_t zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);

  vb128_t result;
  vb128_t age0, bge0;
  vi128_t vra, vrap, vran;
  vi128_t vrb, vrbp, vrbn;

  age0 = vec_cmpltsq (vfa128, (vi128_t) zero);
  vrap = (vi128_t) vec_adduqm ((vui128_t) vfa128, (vui128_t) signmask);
  vran = (vi128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vfa128);
  vra  = (vi128_t) vec_sel ((vui32_t)vrap, (vui32_t)vran, (vui32_t)age0);

  bge0 = vec_cmpltsq (vfb128, (vi128_t) zero);
  vrbp = (vi128_t) vec_adduqm ((vui128_t) vfb128, (vui128_t) signmask);
  vrbn = (vi128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vfb128);
  vrb  = (vi128_t) vec_sel ((vui32_t)vrbp, (vui32_t)vrbn, (vui32_t)bge0);

  result = vec_cmpltuq ((vui128_t) vra, (vui128_t) vrb);

  return result;
}

vb128_t
test_cmpltf128_v3b (vui128_t vfa128, vui128_t vfb128)
{
  const vui32_t zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);

  vb128_t result;
  vb128_t age0, bge0;
  vui128_t vra, vrap, vran;
  vui128_t vrb, vrbp, vrbn;

  age0 = vec_cmpltuq (vfa128, (vui128_t) signmask);
  vrap = (vui128_t) vec_adduqm ((vui128_t) vfa128, (vui128_t) signmask);
  vran = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vfa128);
  vra  = (vui128_t) vec_sel ((vui32_t)vran, (vui32_t)vrap, (vui32_t)age0);

  bge0 = vec_cmpltuq (vfb128, (vui128_t) signmask);
  vrbp = (vui128_t) vec_adduqm ((vui128_t) vfb128, (vui128_t) signmask);
  vrbn = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vfb128);
  vrb  = (vui128_t) vec_sel ((vui32_t)vrbn, (vui32_t)vrbp, (vui32_t)bge0);

  result = vec_cmpltuq ((vui128_t) vra, (vui128_t) vrb);

  return result;
}

vb128_t
test_cmpltf128_v3c (vui128_t vfa128, vui128_t vfb128)
{
  const vui32_t zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);

  vb128_t result;
  vb128_t age0, bge0;
  vui128_t vra, vrap, vran;
  vui128_t vrb, vrbp, vrbn;

  age0 = vec_cmpleuq (vfa128, (vui128_t) signmask);
  vrap = (vui128_t) vec_adduqm ((vui128_t) vfa128, (vui128_t) signmask);
  vran = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vfa128);
  vra  = (vui128_t) vec_sel ((vui32_t)vran, (vui32_t)vrap, (vui32_t)age0);

  bge0 = vec_cmpleuq (vfb128, (vui128_t) signmask);
  vrbp = (vui128_t) vec_adduqm ((vui128_t) vfb128, (vui128_t) signmask);
  vrbn = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vfb128);
  vrb  = (vui128_t) vec_sel ((vui32_t)vrbn, (vui32_t)vrbp, (vui32_t)bge0);

  result = vec_cmpltuq ((vui128_t) vra, (vui128_t) vrb);

  return result;
}

vb128_t
test_cmpltf128_v3d (vui128_t vfa128, vui128_t vfb128)
{
  const vui32_t zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  const vui8_t shift = vec_splat_u8 (7);

  vb128_t result;
  vb128_t age0, bge0;
  vui128_t vra, vrap, vran;
  vui128_t vrb, vrbp, vrbn;
  vui8_t splta, spltb;

  // signbool = vec_setb_qp;
  splta = vec_splat ((vui8_t) vfa128, VEC_BYTE_H);
  age0 = (vb128_t) vec_sra (splta, shift);

  vrap = (vui128_t) vec_xor ((vui32_t) vfa128, signmask);
  vran = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vfa128);
  vra  = (vui128_t) vec_sel ((vui32_t)vrap, (vui32_t)vran, (vui32_t)age0);

  spltb = vec_splat ((vui8_t) vfb128, VEC_BYTE_H);
  bge0 = (vb128_t) vec_sra (spltb, shift);

  vrbp = (vui128_t) vec_xor ((vui32_t) vfb128, signmask);
  vrbn = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vfb128);
  vrb  = (vui128_t) vec_sel ((vui32_t)vrbp, (vui32_t)vrbn, (vui32_t)bge0);

  result = vec_cmpltuq (vra, vrb);

  return result;
}

vb128_t
test_cmpeqf128_v1 (vui128_t vfa128, vui128_t vfb128)
{
  return vec_cmpequq (vfa128, vfb128);
}

vb128_t
test_cmpeqf128_v2 (vi128_t vfa128, vi128_t vfb128)
{
  const vui32_t zero = CONST_VINT128_W(0, 0, 0, 0);
  vi128_t _a = (vi128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vfa128);
  vi128_t _b = (vi128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vfb128);
  vb128_t eq_a, eq_b, and_eq, cmps;
  vb128_t result;

  eq_a = vec_cmpeqsq (vfa128, _a);
  eq_b = vec_cmpeqsq (vfb128, _b);
  and_eq = (vb128_t) vec_and ((vui32_t) eq_a, (vui32_t) eq_b );
  cmps = vec_cmpeqsq (vfa128, vfb128);
  result = (vb128_t) vec_or ((vui32_t) cmps, (vui32_t) and_eq);
  return result;
}

vb128_t
test_cmpeqf128_v3 (vi128_t vfa128, vi128_t vfb128)
{
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vb128_t cmps, or_ab, eq_s;
  vb128_t result;

  or_ab = (vb128_t) vec_or ((vui32_t) vfa128, (vui32_t) vfb128 );
  eq_s = vec_cmpequq ((vui128_t) or_ab, (vui128_t) signmask);
  cmps = vec_cmpeqsq (vfa128, vfb128);
  result = (vb128_t) vec_or ((vui32_t) cmps, (vui32_t) eq_s);
  return result;
}

vb128_t
test_cmpeqf128_v4 (vi128_t vfa128, vi128_t vfb128)
{
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  const vui32_t zero = CONST_VINT128_W(0, 0, 0, 0);
  vb128_t cmps, or_ab, andc, eq_s;
  vb128_t result;

  or_ab = (vb128_t) vec_or ((vui32_t) vfa128, (vui32_t) vfb128 );
  andc  = (vb128_t) vec_andc ((vui32_t) or_ab, (vui32_t) signmask);
  eq_s = vec_cmpequq ((vui128_t) andc, (vui128_t) zero);
  cmps = vec_cmpeqsq (vfa128, vfb128);
  result = (vb128_t) vec_or ((vui32_t) cmps, (vui32_t) eq_s);
  return result;
}

__binary128
test_vec_max8_f128uz (__binary128 vf1, __binary128 vf2,
		    __binary128 vf3, __binary128 vf4,
		    __binary128 vf5, __binary128 vf6,
		    __binary128 vf7, __binary128 vf8)
{
  __binary128 maxres;
  vb128_t bool;

  bool = vec_cmpgtuzqp (vf2, vf1);
  maxres = vec_self128 (vf1, vf2, bool);
  bool = vec_cmpgtuzqp (vf3, maxres);
  maxres = vec_self128 (vf3, maxres, bool);
  bool = vec_cmpgtuzqp (vf4, maxres);
  maxres = vec_self128 (vf4, maxres, bool);
  bool = vec_cmpgtuzqp (vf5, maxres);
  maxres = vec_self128 (vf5, maxres, bool);
  bool = vec_cmpgtuzqp (vf6, maxres);
  maxres = vec_self128 (vf6, maxres, bool);
  bool = vec_cmpgtuzqp (vf7, maxres);
  maxres = vec_self128 (vf7, maxres, bool);
  bool = vec_cmpgtuzqp (vf8, maxres);
  maxres = vec_self128 (vf8, maxres, bool);

  return maxres;
}

__binary128
test_vec_max8_f128 (__binary128 vf1, __binary128 vf2,
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

#ifndef PVECLIB_DISABLE_F128ARITH
#ifdef __FLOAT128__

void
test_vec_qpdpo_f128 (vf64_t * vx64,
		    __binary128 vf1, __binary128 vf2,
		    __binary128 vf3, __binary128 vf4,
		    __binary128 vf5, __binary128 vf6,
		    __binary128 vf7, __binary128 vf8)
{
  vf64_t vxf1, vxf2, vxf3, vxf4;

  vxf1 = vec_xscvqpdpo (vf1);
  vxf2 = vec_xscvqpdpo (vf2);
  vxf3 = vec_xscvqpdpo (vf3);
  vxf4 = vec_xscvqpdpo (vf4);

  vxf1[VEC_DW_L] = vxf2[VEC_DW_H];
  vxf3[VEC_DW_L] = vxf4[VEC_DW_H];

  vx64[0] = vxf1;
  vx64[1] = vxf3;

  vxf1 = vec_xscvqpdpo (vf5);
  vxf2 = vec_xscvqpdpo (vf6);
  vxf3 = vec_xscvqpdpo (vf7);
  vxf4 = vec_xscvqpdpo (vf8);

  vxf1[VEC_DW_L] = vxf2[VEC_DW_H];
  vxf3[VEC_DW_L] = vxf4[VEC_DW_H];

  vx64[2] = vxf1;
  vx64[3] = vxf3;
}

void
test_gcc_qpdpo_f128 (vf64_t * vx64,
		    __binary128 vf1, __binary128 vf2,
		    __binary128 vf3, __binary128 vf4,
		    __binary128 vf5, __binary128 vf6,
		    __binary128 vf7, __binary128 vf8)
{
  vf64_t vxf1, vxf2, vxf3, vxf4;

  vxf1[0] = vf1;
  vxf1[1] = vf2;
  vxf2[0] = vf3;
  vxf2[1] = vf4;
  vxf3[0] = vf5;
  vxf3[1] = vf6;
  vxf4[0] = vf7;
  vxf4[1] = vf8;

  vx64[0] = vxf1;
  vx64[1] = vxf2;
  vx64[2] = vxf3;
  vx64[3] = vxf4;
}

void
test_vec_qpuq_f128 (vui128_t * vf128,
		    __binary128 vf1, __binary128 vf2,
		    __binary128 vf3, __binary128 vf4,
		    __binary128 vf5, __binary128 vf6,
		    __binary128 vf7, __binary128 vf8)
{
  vf128[0] = vec_xscvqpuqz (vf1);
  vf128[1] = vec_xscvqpuqz (vf2);
  vf128[2] = vec_xscvqpuqz (vf3);
  vf128[3] = vec_xscvqpuqz (vf4);
  vf128[4] = vec_xscvqpuqz (vf5);
  vf128[5] = vec_xscvqpuqz (vf6);
  vf128[6] = vec_xscvqpuqz (vf7);
  vf128[7] = vec_xscvqpuqz (vf8);
}

void
test_gcc_qpuq_f128 (vui128_t * vf128,
		    __binary128 vf1, __binary128 vf2,
		    __binary128 vf3, __binary128 vf4,
		    __binary128 vf5, __binary128 vf6,
		    __binary128 vf7, __binary128 vf8)
{
  unsigned __int128 *vx128 = (unsigned __int128 *)vf128;
  unsigned __int128 vi1, vi2, vi3, vi4, vi5, vi6, vi7, vi8;

  vi1 = vf1;
  vi2 = vf2;
  vi3 = vf3;
  vi4 = vf4;
  vi5 = vf5;
  vi6 = vf6;
  vi7 = vf7;
  vi8 = vf8;

  vx128[0] = vi1;
  vx128[1] = vi2;
  vx128[2] = vi3;
  vx128[3] = vi4;
  vx128[4] = vi5;
  vx128[5] = vi6;
  vx128[6] = vi7;
  vx128[7] = vi8;
}

void
test_vec_uqqp_f128 (__binary128 * vf128,
		    vui128_t vf1, vui128_t vf2,
		    vui128_t vf3, vui128_t vf4,
		    vui128_t vf5, vui128_t vf6,
		    vui128_t vf7, vui128_t vf8)
{
  vf128[0] = vec_xscvuqqp (vf1);
  vf128[1] = vec_xscvuqqp (vf2);
  vf128[2] = vec_xscvuqqp (vf3);
  vf128[3] = vec_xscvuqqp (vf4);
  vf128[4] = vec_xscvuqqp (vf5);
  vf128[5] = vec_xscvuqqp (vf6);
  vf128[6] = vec_xscvuqqp (vf7);
  vf128[7] = vec_xscvuqqp (vf8);
}

void
test_gcc_uqqp_f128 (__binary128 * vf128,
		    vui128_t vf1, vui128_t vf2,
		    vui128_t vf3, vui128_t vf4,
		    vui128_t vf5, vui128_t vf6,
		    vui128_t vf7, vui128_t vf8)
{
  vf128[0] = vf1[0];
  vf128[1] = vf2[0];
  vf128[2] = vf3[0];
  vf128[3] = vf4[0];
  vf128[4] = vf5[0];
  vf128[5] = vf6[0];
  vf128[6] = vf7[0];
  vf128[7] = vf8[0];
}

void
test_vec_dpqp_f128 (__binary128 * vf128,
		    vf64_t vf1, vf64_t vf2,
		    vf64_t vf3, vf64_t vf4,
		    vf64_t vf5)
{
  vf128[0] = vec_xscvdpqp (vf1);
  vf1[VEC_DW_H] = vf1[VEC_DW_L];
  vf128[1] = vec_xscvdpqp (vf1);

  vf128[2] = vec_xscvdpqp (vf2);
  vf2[VEC_DW_H] = vf2[VEC_DW_L];
  vf128[3] = vec_xscvdpqp (vf2);

  vf128[4] = vec_xscvdpqp (vf3);
  vf3[VEC_DW_H] = vf3[VEC_DW_L];
  vf128[5] = vec_xscvdpqp (vf3);

  vf128[6] = vec_xscvdpqp (vf4);
  vf4[VEC_DW_H] = vf4[VEC_DW_L];
  vf128[7] = vec_xscvdpqp (vf4);

  vf128[8] = vec_xscvdpqp (vf5);
  vf5[VEC_DW_H] = vf5[VEC_DW_L];
  vf128[8] = vec_xscvdpqp (vf5);
}

void
test_gcc_dpqp_f128 (__binary128 * vf128,
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

__binary128
test_gcc_max8_f128 (__binary128 vf1, __binary128 vf2,
		    __binary128 vf3, __binary128 vf4,
		    __binary128 vf5, __binary128 vf6,
		    __binary128 vf7, __binary128 vf8)
{
  __binary128 maxres = vf1;

#ifndef __clang__
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
#endif

  return maxres;
}

vb128_t
test_vec_cmpequqp (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpequqp (vfa, vfb);
}

vb128_t
test_vec_cmpequzqp (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpequzqp (vfa, vfb);
}

vb128_t
test_vec_cmpeqtoqp (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpeqtoqp (vfa, vfb);
}

vb128_t
test_vec_cmpneuqp (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpneuqp (vfa, vfb);
}

vb128_t
test_vec_cmpneuzqp (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpneuzqp (vfa, vfb);
}

vb128_t
test_vec_cmpnetoqp (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpnetoqp (vfa, vfb);
}

vb128_t
test_vec_cmpleuzqp (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpleuzqp (vfa, vfb);
}

vb128_t
test_vec_cmpleuqp (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpleuqp (vfa, vfb);
}

vb128_t
test_vec_cmpletoqp (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpletoqp (vfa, vfb);
}

vb128_t
test_vec_cmpgeuzqp (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpgeuzqp (vfa, vfb);
}

vb128_t
test_vec_cmpgeuqp (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpgeuqp (vfa, vfb);
}

vb128_t
test_vec_cmpgetoqp (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpgetoqp (vfa, vfb);
}

vb128_t
test_vec_cmpltuzqp (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpltuzqp (vfa, vfb);
}

vb128_t
test_vec_cmpltuqp (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpltuqp (vfa, vfb);
}

vb128_t
test_vec_cmplttoqp (__binary128 vfa, __binary128 vfb)
{
  return vec_cmplttoqp (vfa, vfb);
}

vb128_t
test_vec_cmpgtuzqp (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpgtuzqp (vfa, vfb);
}

vb128_t
test_vec_cmpgtuqp (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpgtuqp (vfa, vfb);
}

vb128_t
test_vec_cmpgttoqp (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpgttoqp (vfa, vfb);
}

__float128
test_absdiff (__float128 vra, __float128 vrb)
{
#ifndef __clang__
  __float128 result;
  if (vra > vrb)
    result = vra - vrb;
  else
    result = vrb - vra;
  return result;
#else
  return vra;
#endif
}

// TBD will sub-in pveclib softfloat for P8 when available

__float128
test_scalar_add128 (__float128 vra, __float128 vrb)
{
#ifndef __clang__
  if (__builtin_cpu_supports("ieee128"))
    {
      __float128 result;
      __asm__(
	      "xsaddqp %0,%1,%2"
	      : "=v" (result)
	      : "v" (vra), "v" (vrb)
	      : );
      return result;
    }
  else
    // Generate call to __addkf3
    return (vra + vrb);
#else
  return vra;
#endif
}

__float128
test_scalar_div128 (__float128 vra, __float128 vrb)
{
#ifndef __clang__
  if (__builtin_cpu_supports("ieee128"))
    {
      __float128 result;
      __asm__(
	      "xsdivqp %0,%1,%2"
	      : "=v" (result)
	      : "v" (vra), "v" (vrb)
	      : );
      return result;
    }
  else
    // Generate call to __divkf3
    return (vra / vrb);
#else
  return vra;
#endif
}

__float128
test_scalar_mul128 (__float128 vra, __float128 vrb)
{
#ifndef __clang__
  if (__builtin_cpu_supports("ieee128"))
    {
      __float128 result;
      __asm__(
	      "xsmulqp %0,%1,%2"
	      : "=v" (result)
	      : "v" (vra), "v" (vrb)
	      : );
      return result;
    }
  else
    // Generate call to __mulkf3
    return (vra * vrb);
#else
  return vra;
#endif
}

__float128
test_scalar_sub128 (__float128 vra, __float128 vrb)
{
#ifndef __clang__
  if (__builtin_cpu_supports("ieee128"))
    {
      __float128 result;
      __asm__(
	      "xssubqp %0,%1,%2"
	      : "=v" (result)
	      : "v" (vra), "v" (vrb)
	      : );
      return result;
    }
  else
    // Generate call to __subkf3
    return (vra - vrb);
#else
  return vra;
#endif
}

__float128
test_scalarCC_expxsuba_128 (__float128 x, __float128 a, __float128 expa)
{
#ifndef __clang__
  const __float128 f128_one = 1.0Q;
  const __float128 inv_fact2 = (1.0Q / 2.0Q);
  const __float128 inv_fact3 = (1.0Q / 6.0Q);
  const __float128 inv_fact4 = (1.0Q / 24.0Q);
  const __float128 inv_fact5 = (1.0Q / 120.0Q);
  const __float128 inv_fact6 = (1.0Q / 720.0Q);
  const __float128 inv_fact7 = (1.0Q / 5040.0Q);
  const __float128 inv_fact8 = (1.0Q / 40320.0Q);

  __float128 term, xma, xma2, xmaf2;
  __float128 xma3, xmaf3, xma4, xmaf4, xma5, xmaf5;
  __float128 xma6, xmaf6, xma7, xmaf7, xma8, xmaf8;

  // 1st 8 terms of e**x = e**a * taylor( x-a )
  xma = (x - a);
  term = (f128_one + xma);
  xma2 = (xma * xma);
  xmaf2 = (xma2 * inv_fact2);
  term = (term + xmaf2);
  xma3 = (xma2 * xma);
  xmaf3 = (xma3 * inv_fact3);
  term =  (term + xmaf3);
  xma4 = (xma3 * xma);
  xmaf4 = (xma4 * inv_fact4);
  term = (term + xmaf4);
  xma5 = (xma4 * xma);
  xmaf5 = (xma5 * inv_fact5);
  term = (term + xmaf5);
  xma6 = (xma5 * xma);
  xmaf6 = (xma6 * inv_fact6);
  term = (term + xmaf6);
  xma7 = (xma6 * xma);
  xmaf7 = (xma7 * inv_fact7);
  term = (term + xmaf7);
  xma8 = (xma7 * xma);
  xmaf8 = (xma8 * inv_fact8);
  term = (term + xmaf8);
  return (expa * term);
#else
  return x;
#endif
}
#endif
#endif

__binary128
test_vec_xsiexpqp (vui128_t sig, vui64_t exp)
{
  return vec_xsiexpqp (sig, exp);
}

vui64_t
test_vec_xsxexpqp (__binary128 f128)
{
  return vec_xsxexpqp (f128);
}

vui128_t
test_vec_xsxsigqp (__binary128 f128)
{
  return vec_xsxsigqp (f128);
}

vb128_t
__test_setb_qp (__binary128 f128)
{
  return vec_setb_qp (f128);
}

__binary128
test_vec_absf128 (__binary128 f128)
{
  return vec_absf128 (f128);
}

__binary128
test_vec_nabsf128 (__binary128 f128)
{
  return vec_nabsf128 (f128);
}

__binary128
test_const_vec_huge_valf128 (void)
{
  return vec_const_huge_valf128 ();
}

__binary128
test_const_vec_nanf128 (void)
{
  return vec_const_nanf128 ();
}

__binary128
test_const_vec_nansf128 (void)
{
  return vec_const_nansf128 ();
}

__binary128
test_const_vec_inff128 (void)
{
  return vec_const_inff128 ();
}

__binary128
test_vec_copysignf128 (__binary128 f128x , __binary128 f128y)
{
  return vec_copysignf128 (f128x , f128y);
}

vb128_t
test_vec_isfinitef128 (__binary128 f128)
{
  return vec_isfinitef128 (f128);
}

vb128_t
test_vec_isinff128 (__binary128 value)
{
  return (vec_isinff128 (value));
}

vb128_t
test_vec_isnanf128 (__binary128 value)
{
  return (vec_isnanf128 (value));
}

vb128_t
test_vec_isnormalf128 (__binary128 f128)
{
  return vec_isnormalf128 (f128);
}

vb128_t
test_vec_issubnormalf128 (__binary128 f128)
{
  return vec_issubnormalf128 (f128);
}

vb128_t
test_vec_iszerof128 (__binary128 f128)
{
  return vec_iszerof128 (f128);
}

int
test_vec_all_finitef128 (__binary128 value)
{
  return (vec_all_isfinitef128 (value));
}

int
test_vec_all_inff128 (__binary128 value)
{
  return (vec_all_isinff128 (value));
}

int
test_vec_all_nanf128 (__binary128 value)
{
  return (vec_all_isnanf128 (value));
}

int
test_vec_all_normalf128 (__binary128 value)
{
  return (vec_all_isnormalf128 (value));
}

int
test_vec_all_subnormalf128 (__binary128 value)
{
  return (vec_all_issubnormalf128 (value));
}

int
test_vec_all_zerof128 (__binary128 value)
{
  return (vec_all_iszerof128 (value));
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
test_sinf128 (__binary128 value)
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
test_cosf128 (__binary128 value)
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

vb128_t
_test_f128_isinff128 (__Float128 value)
{
  return (vec_isinff128 (value));
}

int
_test_f128_isinf_sign (__Float128 value)
{
  return (vec_isinf_signf128 (value));
}

vb128_t
_test_f128_isnan (__Float128 value)
{
  return (vec_isnanf128 (value));
}

vb128_t
_test_pred_f128_finite (__Float128 value)
{
  return (vec_isfinitef128 (value));
}

vb128_t
_test_pred_f128_normal (__Float128 value)
{
  return (vec_isnormalf128 (value));
}

vb128_t
_test_pred_f128_subnormal (__Float128 value)
{
  return (vec_issubnormalf128 (value));
}

vui16_t
_test_xfer_bin128_2_vui16t (__binary128 f128)
{
  return vec_xfer_bin128_2_vui16t (f128);
}

#ifdef __FLOAT128__
/* Mostly compiler and library tests follow to see what the various
 * compilers will do. */

#ifndef PVECLIB_DISABLE_F128MATH
// Enable only if math.h supports generic C11 functions for __float128
// __clang__ has a bug whenever -mfloat128 is enabled, maybe clang 10.0.1
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
int
test_gcc_f128_signbit (__Float128 value)
  {
    return (signbit(value));
  }

int
test_gcc_f128_isinf (__Float128 value)
  {
    return (isinf(value));
  }

int
test_gcc_float128_isnan (__Float128 value)
  {
    return (isnan(value));
  }

__Float128
test_gcc_f128_copysign (__Float128 valx, __Float128 valy)
  {
    return (__builtin_copysignf128(valx, valy));
  }

int
test_glibc_f128_classify (__Float128 value)
  {
    if (isfinite(value))
    return 1;

    if (isinf(value))
    return 2;

    if (isnan(value))
    return 3;
    /* finite */
    return 0;
  }
#endif
#endif
#endif

