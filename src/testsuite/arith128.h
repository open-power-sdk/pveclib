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

 arith128.h

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: May 10, 2015
 */

#ifndef ARITH128_H_
#define ARITH128_H_

#ifdef __STDC_WANT_DEC_FP__
#else
#define __STDC_WANT_DEC_FP__    1
#endif

#include <stdint.h>
#include <stddef.h>
#include <float.h>

#include <pveclib/vec_int128_ppc.h>
#include <pveclib/vec_char_ppc.h>
#include <pveclib/vec_bcd_ppc.h>

static inline void
GROUP_ENDING_NOP (void)
{
	 __asm ( "ori 2,2,0;" : : : "memory");
}

static inline uint64_t
upper_128 (__int128 val)
{
  __VEC_U_128 t;
  t.i128 = val;
  return (t.ulong.upper);
}

static inline uint64_t
lower_128 (__int128 val)
{
  __VEC_U_128 t;
  t.i128 = val;
  return (t.ulong.lower);
}

static inline __int128
combine_128 (uint64_t u, uint64_t l)
{
  __VEC_U_128 t;
  t.ulong.lower = l;
  t.ulong.upper = u;
  return (t.i128);
}
/*
 * udiv_qrnnd(quotient, remainder, high_numerator, low_numerator,
 denominator) divides a UDWtype, composed by the UWtype integers
 HIGH_NUMERATOR and LOW_NUMERATOR, by DENOMINATOR and places the quotient
 in QUOTIENT and the remainder in REMAINDER.  HIGH_NUMERATOR must be less
 than DENOMINATOR for correct operation.
 */

static inline uint64_t
ppc_udiv_qrnnd (uint64_t *remainder, uint64_t high_num, uint64_t low_num,
                uint64_t denom)
{
  uint64_t q1, q2, r1, r2, Q, R;

  q1 = __builtin_divdeu (high_num, denom);
  r1 = -(q1 * denom);
  q2 = low_num / denom;
  r2 = low_num - (q2 * denom);
  Q = q1 + q2;
  R = r1 + r2;
  if ((R < r2) || (R >= denom))
    {
      Q += 1;
      R = R - denom;
    }
  *remainder = R;
  return (Q);
}

static inline uint64_t
div_qdu (uint64_t *remainder, unsigned __int128 numerator, uint64_t denom)
{
  uint64_t high_num, low_num, Q;
  high_num = upper_128 (numerator);
  low_num = lower_128 (numerator);
  Q = ppc_udiv_qrnnd (remainder, high_num, low_num, denom);
  return (Q);
}

static inline int64_t
ppc_sdiv_qrnnd (int64_t *remainder, int64_t high_num, int64_t low_num,
                int64_t denom)
{
  int64_t q1, q2, r1, r2, Q, R, abs_D;

  abs_D = __builtin_labs (denom);

  q1 = __builtin_divde (high_num, denom);
  r1 = -(q1 * denom);
  q2 = ((uint64_t) low_num / (uint64_t) denom);
  r2 = low_num - (q2 * denom);
  Q = q1 + q2;
  R = r1 + r2;
  if (((high_num >= 0) && (R >= abs_D)) || ((high_num < 0) && (R >= -abs_D)))
    {
      Q += 1;
      R = R - denom;
    }
  *remainder = R;
  return (Q);
}

extern int64_t
db_sdiv_qrnnd (int64_t *remainder, int64_t high_num, int64_t low_num,
               int64_t denom);

static inline int64_t
div_qd (int64_t *remainder, __int128 numerator, int64_t denom)
{
  int64_t high_num, low_num, Q;
  high_num = upper_128 (numerator);
  low_num = lower_128 (numerator);

#if 0
  Q = db_sdiv_qrnnd (remainder, high_num, low_num, denom);
#else
  Q = ppc_sdiv_qrnnd (remainder, high_num, low_num, denom);
#endif
  return (Q);
}

/** \brief Add two FXU Unsigned BCD 16 digit values.
 *
 * Two unsigned 16 digit values are added and lower 16 digits of the
 * sum are returned.  Overflow (carry-out) is ignored.
 *
 *	@param a a 64-bit vector treated a unsigned BCD 16 digit value.
 *	@param b a 64-bit vector treated a unsigned BCD 16 digit value.
 *	@return a 64-bit vector which is the lower 16 digits of (a + b).
 */
static inline uint64_t
fxu_bcdadd (uint64_t a, uint64_t b)
{
  uint64_t t;
  const uint64_t sixes = 0x6666666666666666UL;
#if 1
  __asm__(
      "add	0,%1,%3;\n"
      "\tadd	12,0,%2;\n"
      "\taddg6s	%0,0,%2;\n"
      "\tsubf	%0,%0,12;\n"
      : "=r" (t)
      : "r" (a),
      "r" (b),
      "r" (sixes)
      : "r0", "r12");
#else
  uint64_t x,y;
  x = a + sixes;
  y = x + b;
  t = __builtin_addg6s (x,b);
  t = y - t;
#endif
  return (t);
}

/** \brief Subtract two FXU Unsigned BCD 16 digit values.
 *
 * Subtract unsigned 16 digit values and return the lower 16 digits of
 * the difference.  Overflow (borrow) is ignored.
 *
 *	@param a a 64-bit vector treated a unsigned BCD 16 digit value.
 *	@param b a 64-bit vector treated a unsigned BCD 16 digit value.
 *	@return a 64-bit vector which is the lower 16 digits of (a - b).
 */
static inline uint64_t
fxu_bcdsub (uint64_t a, uint64_t b)
{
  uint64_t t;
  const uint64_t sixes = 0x6666666666666666UL;
#if 1
  __asm__(
      "addi	0,%2,1;\n"
      "\tnor	12,%1,%1;\n"
      "\tadd	11,0,12;\n"
      "\taddg6s	%0,0,12;\n"
      "\tsubf	%0,%0,11;\n"
      : "=r" (t)
      : "r" (a),
      "r" (b),
      "r" (sixes)
      : "r0", "r11", "r12");
#else
  uint64_t x,y,z;
  x = b + 1;
  y = ~a;
  z = x + y;
  t = __builtin_addg6s (x,y);
  t = z - t;
#endif
  return (t);
}

static inline unsigned __int128
mul_hld (uint64_t a, uint64_t b)
{
  unsigned __int128 t, ex_a, ex_b;
  ex_a = a;
  ex_b = b;
  t = ex_a * ex_b;
  return (t);
}

static inline __int128
mul_shld (int64_t a, int64_t b)
{
  __int128 t, ex_a, ex_b;
  ex_a = a;
  ex_b = b;
  t = ex_a * ex_b;
  return (t);
}

/* Product a 256-bit sum of a 4 partial products of a 128-bit X 128-bit
 multiply.  The 128-bit partial products are produced as 64-bit X
 64-bit multiplies for the combinations of high and low 64-bits of
 the 128-bit multiplican and multiplier.
 */
static inline unsigned __int128
sumpprod (unsigned __int128 *hsum, unsigned __int128 h, unsigned __int128 m1,
          unsigned __int128 m2, unsigned __int128 l)
{
  __int128 t, hs, ls;
  __asm__(
      "addc %L2,%L4,%L5;\n"
      "\tadde %2,%4,%5;\n"
      "\taddze %0,%3;\n"
      "\taddc %1,%6,%L2;\n"
      "\tadde %L0,%L3,%2;\n"
      "\taddze %0,%0;\n"
      "\tmr   %L1,%L6;"
      : "=&r" (hs), /* 0 */
      "=r" (ls), /* 1 */
      "=&r" (t) /* 2 */
      : "r" (h), /* 3 */
      "r" (m1), /* 4 */
      "r" (m2), /* 5 */
      "r" (l) /* 6 */
      : );
  *hsum = hs;
  return (ls);
}

/* Product a 256-bit product of a unsigned 128-bit X 128-bit
 multiply.

 First 128-bit partial products are produced as 64-bit X
 64-bit multiplies for the combinations of high and low 64-bits of
 the 128-bit multiplican and multiplier.  Finally the partial
 products are summed to product the high/low 128-bits of the
 256-bit product.
 */
static inline unsigned __int128
mul_hlq (unsigned __int128 *h_prod, unsigned __int128 a, unsigned __int128 b)
{
  __VEC_U_128 ua, ub;
  unsigned __int128 h, m1, m2, l;
  ua.i128 = a;
  ub.i128 = b;

  m1 = mul_hld (ua.ulong.upper, ub.ulong.lower);
  m2 = mul_hld (ua.ulong.lower, ub.ulong.upper);
  l = mul_hld (ua.ulong.lower, ub.ulong.lower);
  h = mul_hld (ua.ulong.upper, ub.ulong.upper);
  l = sumpprod (h_prod, h, m1, m2, l);

  return (l);
}

static inline _Decimal128
vec2FRp (vui64_t val)
{
#ifdef _ARCH_PWR7
  _Decimal128 t;
  __asm__(
      "xxpermdi %0,%x1,%x1,0b00;\n"
      "\txxpermdi %L0,%x1,%x1,0b10;\n"
      : "=d" (t)
      : "v" (val)
      : );
  return (t);
#else
  U_128 t;
  t.vx2 = val;
  return (t.dpd128);
#endif
}

static inline vui64_t
FRp2vec (_Decimal128 val)
{
#ifdef _ARCH_PWR7
  vui64_t vx2;
  __asm__(
      "xxpermdi %x0,%1,%L1,0b00;\n"
      : "=v" (vx2)
      : "d" (val)
      : );
  return (vx2);
#else
  U_128 t;
  t.dpd128 = val;
  return (t.vx2);
#endif
}

static inline __int128
adduqm (__int128 a, __int128 b)
{
  __int128 t;
  __asm__(
      "addc %L0,%L1,%L2;\n"
      "\tadde %0,%1,%2;"
      : "=r" (t)
      : "r" (a),
      "r" (b)
      : );
  return (t);
}

static inline __int128
addcuq (__int128 a, __int128 b)
{
  __int128 t, x;
  __asm__(
      "addc %L1,%L2,%L3;\n"
      "\tadde %1,%2,%3;\n"
      "\tli   %0,0;\n"
      "\taddze %L0,%0;"
      : "=r" (t),
      "=&r" (x)
      : "r" (a),
      "r" (b)
      : );
  return (t);
}

static inline __int128
addeuqm (__int128 a, __int128 b, __int128 c)
{
  __int128 t;
  __asm__(
      "addic 0,%L3,-1;\n"
      "\tadde %L0,%L1,%L2;\n"
      "\tadde %0,%1,%2;"
      : "=r" (t)
      : "r" (a),
      "r" (b),
      "r" (c)
      : "r0");
  return (t);
}

static inline __int128
addecuq (__int128 a, __int128 b, __int128 c)
{
  __int128 t;
  __asm__(
      "addic 0,%L3,-1;\n"
      "\tadde %L0,%L1,%L2;\n"
      "\tadde %0,%1,%2;\n"
      "\tli   %0,0;\n"
      "\taddze %L0,%0;"
      : "=r" (t)
      : "r" (a),
      "r" (b),
      "r" (c)
      : "r0");
  return (t);
}

static inline __int128
subuqm (__int128 a, __int128 b)
{
  __int128 t;
  __asm__(
      "subfc %L0,%L2,%L1;\n"
      "\tsubfe %0,%2,%1;"
      : "=r" (t)
      : "r" (a),
      "r" (b)
      : );
  return (t);
}

static inline __int128
subcuq (__int128 a, __int128 b)
{
  __int128 t, x;
  __asm__(
      "subfc %L1,%L3,%L2;\n"
      "\tsubfe %1,%3,%2;\n"
      "\tli   %0,0;\n"
      "\taddze %L0,%0;"
      : "=r" (t),
      "=r" (x)
      : "r" (a),
      "r" (b)
      : );
  return (t);
}

static inline __int128
subeuqm (__int128 a, __int128 b, __int128 c)
{
  __int128 t;
  __asm__(
      "addic 0,%L3,-1;\n"
      "\tsubfe %L0,%L2,%L1;\n"
      "\tsubfe %0,%2,%1;"
      : "=r" (t)
      : "r" (a),
      "r" (b),
      "r" (c)
      : "r0");
  return (t);
}

static inline __int128
subecuq (__int128 a, __int128 b, __int128 c)
{
  __int128 t;
  __asm__(
      "addic 0,%L3,-1;\n"
      "\tsubfe %L0,%L2,%L1;\n"
      "\tsubfe %0,%2,%1;\n"
      "\tli   %0,0;\n"
      "\taddze %L0,%0;"
      : "=r" (t)
      : "r" (a),
      "r" (b),
      "r" (c)
      : "r0");
  return (t);
}

static inline __int128
mullq (__int128 a, __int128 b)
{
  __int128 t, x;
  __asm__(
      "mulld %0,%L2,%3;\n"
      "\tmulld %L1,%2,%L3;\n"
      "\tmulhdu %1,%L2,%L3;\n"
      "\tmulld %L0,%L2,%L3;\n"
      "\tadd %0,%0,%L1;\n"
      "\tadd %0,%0,%1;"
      : "+r" (t),
      "=&r" (x)
      : "r" (a),
      "r" (b)
      : );
  return (t);
}

static inline unsigned __int128
mullqu (unsigned __int128 a, unsigned __int128 b)
{
  unsigned __int128 t, x;
  __asm__(
      "mulld %0,%L2,%3;\n"
      "\tmulld %L1,%2,%L3;\n"
      "\tmulhdu %1,%L2,%L3;\n"
      "\tmulld %L0,%L2,%L3;\n"
      "\tadd %0,%0,%L1;\n"
      "\tadd %0,%0,%1;"
      : "=&r" (t),
      "=&r" (x)
      : "r" (a),
      "r" (b)
      : );
  return (t);
}

static inline __int128
fixtfti (long double a)
{
  __int128 r;
  r = a;
  return (r);
}

static inline unsigned __int128
fixunstfti (long double a)
{
  unsigned __int128 r;
  r = a;
  return (r);
}


#endif /* ARITH128_H_ */
