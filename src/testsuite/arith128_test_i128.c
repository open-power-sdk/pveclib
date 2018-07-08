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

 arith128_test_i128.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Oct 25, 2017
 */

#define __STDC_WANT_DEC_FP__    1

#include <stdint.h>
#include <stdio.h>
#if 0
#include <dfp/fenv.h>
#include <dfp/float.h>
#include <dfp/math.h>
#else
#include <fenv.h>
#include <float.h>
#include <math.h>
#endif

//#define __DEBUG_PRINT__
#include <vec_common_ppc.h>
#include <vec_int32_ppc.h>
#include <vec_int64_ppc.h>
#include <vec_int128_ppc.h>

#include "arith128.h"
#include <testsuite/arith128_print.h>

#include <testsuite/arith128_test_i128.h>
#include <testsuite/vec_perf_i128.h>

extern const vui128_t vtipowof10 [];

vui128_t
db_shift_rightq (vui128_t vra, vui128_t vrb)
{
  vui128_t result;

  result = vec_srq (vra, vrb);

  return (result);
}

vui128_t
db_shift_leftq (vui128_t vra, vui128_t vrb)
{
  vui128_t result;

  result = vec_slq (vra, vrb);

  return (result);
}

#ifdef __DEBUG_PRINT__
#ifdef _ARCH_PWR8
vui128_t
db_vec_clzq (vui128_t vra)
{
  __vector
  unsigned long long result, vt1, vt3;
  __vector
  unsigned long long vt2;
  const __vector
  unsigned long long vzero = { 0, 0 };
  const __vector
  unsigned long long v64 = { 64, 64 };

  printf ("db_vec_clzq\n");
  print_v2xint64 ("  a = ", (vui64_t) vra);
  print_v2xint64 ("  0 = ", vzero);
  print_v2xint64 (" 64 = ", v64);

  vt1 = vec_vclz ((__vector unsigned long long)vra);
  print_v2xint64 ("ctz = ", vt1);
#if 0
  vt2 = (__vector unsigned long long)vec_cmpeq ((__vector unsigned long long)vra, vzero);
#else
  vt2 = (__vector unsigned long long)vec_cmplt (vt1, v64);
#endif
  print_v2xint64 ("cmp = ", vt2);
  vt3 = (__vector unsigned long long)vec_sld ((__vector unsigned char)vzero, (__vector unsigned char)vt2, 8);
  print_v2xint64 ("slo8= ", vt3);
  result = vec_andc (vt1, vt3);
  print_v2xint64 ("andc= ", result);
  result = (__vector unsigned long long)vec_sums ((__vector int)result, (__vector int)vzero);
  print_v2xint64 ("sum = ", result);

  return ((vui128_t) result);
}
#endif


#ifdef _ARCH_PWR8
vui32_t
db_vec_mulluq (vui32_t a, vui32_t b)
{
  vui32_t t, tmq;
  /* compute the 256 bit product of two 128 bit values a, b.
   * The high 128 bits are accumulated in t and the low 128-bits
   * in tmq.
   *
   * We use the Vector Multiple Even/Odd Unsigned word to compute
   * the 128 x 32 partial (160-bit) product of value a with the
   * word splat of b. These instructions (vmuleum, vmuloum)
   * product four 64-bit 32 x 32 partial products where even
   * results are shifted 32-bit left from odd results. After
   * shifting the high 128 bits can be summed via Vector add
   * unsigned quadword.
   *
   *
   *
   */
#ifdef _ARCH_PWR8
  vui32_t tsw;
  vui32_t t_odd, t_even;
  vui32_t z=
    { 0,0,0,0};
  vui32_t sro_4 =
    { 0,0,0,4*8};

  tsw = vec_splat (b, 3);
  printf ("db_vec_mulluq\n");
  print_vint128 ("  a = ", (vui128_t)a);
  print_vint128 ("  b = ", (vui128_t)b);
  print_vint128 ("tsw = ", (vui128_t)tsw);
  /* t_even = vec_vmuleum(a, tsw) */
  __asm__(
      "vmuleuw %0,%1,%2;\n"
      : "=v" (t_even)
      : "v" (a),
      "v" (tsw)
      : );
  /* t_odd  = vec_vmuloum(a, tsw) */
  __asm__(
      "vmulouw %0,%1,%2;\n"
      : "=v" (t_odd)
      : "v" (a),
      "v" (tsw)
      : );
  /* Rotate the low 32-bits (right) into tmq. This is actually
   * implemented as 96-bit (12-byte) shift left. */
  tmq = vec_sld (t_odd, z, 12);
  print_vint128 (" t_even = ", (vui128_t)t_even);
  print_vint128 (" t_odd  = ", (vui128_t)t_odd);
  print_vint128 (" tmq    = ", (vui128_t)tmq);
  /* shift the low 128 bits of partial product right 32-bits */
#if 0
  t_odd = vec_sro (t_odd, (vui8_t)sro_4);
#else
  t_odd = vec_sld (z, t_odd, 12);
#endif
  print_vint128 (" t_odd  = ", (vui128_t)t_odd);
  /* add the high 128 bits of even / odd partial products */
  __asm__(
      "vadduqm %0,%1,%2;\n"
      : "=v" (t)
      : "v" (t_even),
      "v" (t_odd)
      : );

  print_vint128 (" t      = ", (vui128_t)t);
  tsw = vec_splat (b, 2);
  print_vint128 ("tsw = ", (vui128_t)tsw);
  /* t_even = vec_vmuleum(a, tsw) */
  __asm__(
      "vmuleuw %0,%1,%2;\n"
      : "=v" (t_even)
      : "v" (a),
      "v" (tsw)
      : );
  /* t_odd  = vec_vmuloum(a, tsw) */
  __asm__(
      "vmulouw %0,%1,%2;\n"
      : "=v" (t_odd)
      : "v" (a),
      "v" (tsw)
      : );
  print_vint128 (" t_even = ", (vui128_t)t_even);
  print_vint128 (" t_odd  = ", (vui128_t)t_odd);
  /* Sum the low 128 bits of odd previous partial products */
  /* todo is there a possible carry out of this */
  __asm__(
      "vadduqm %0,%0,%1;\n"
      : "+v" (t_odd)
      : "v" (t)
      : );
  print_vint128 (" t_odd+t= ", (vui128_t)t_odd);
  /* rotate right the low 32-bits into tmq */
  tmq = vec_sld (t_odd, tmq, 12);
  print_vint128 (" tmq    = ", (vui128_t)tmq);
  /* shift the low 128 bits of partial product right 32-bits */
  t_odd = vec_sro (t_odd, (vui8_t)sro_4);
  print_vint128 (" t_odd  = ", (vui128_t)t_odd);
  /* add the top 128 bits of even / odd partial products */
  __asm__(
      "vadduqm %0,%1,%2;\n"
      : "=v" (t)
      : "v" (t_even),
      "v" (t_odd)
      : );

  print_vint128 (" t      = ", (vui128_t)t);
  tsw = vec_splat (b, 1);
  /* t_even = vec_vmuleum(a, tsw) */
  __asm__(
      "vmuleuw %0,%1,%2;\n"
      : "=v" (t_even)
      : "v" (a),
      "v" (tsw)
      : );
  /* t_odd  = vec_vmuloum(a, tsw) */
  __asm__(
      "vmulouw %0,%1,%2;\n"
      : "=v" (t_odd)
      : "v" (a),
      "v" (tsw)
      : );
  print_vint128 (" t_even = ", (vui128_t)t_even);
  print_vint128 (" t_odd  = ", (vui128_t)t_odd);
  /* Sum the low 128 bits of odd previous partial products */
  /* todo is there a possible carry out of this */
  __asm__(
      "vadduqm %0,%0,%1;\n"
      : "+v" (t_odd)
      : "v" (t)
      : );
  print_vint128 (" t_odd+t= ", (vui128_t)t_odd);
  /* rotate right the low 32-bits into tmq */
  tmq = vec_sld (t_odd, tmq, 12);
  print_vint128 (" tmq    = ", (vui128_t)tmq);
  /* shift the low 128 bits of partial product right 32-bits */
  t_odd = vec_sro (t_odd, (vui8_t)sro_4);
  /* add the top 128 bits of even / odd partial products */
  __asm__(
      "vadduqm %0,%1,%2;\n"
      : "=v" (t)
      : "v" (t_even),
      "v" (t_odd)
      : );

  tsw = vec_splat (b, 0);
  /* t_even = vec_vmuleum(a, tsw) */
  __asm__(
      "vmuleuw %0,%1,%2;\n"
      : "=v" (t_even)
      : "v" (a),
      "v" (tsw)
      : );
  /* t_odd  = vec_vmuloum(a, tsw) */
  __asm__(
      "vmulouw %0,%1,%2;\n"
      : "=v" (t_odd)
      : "v" (a),
      "v" (tsw)
      : );
  print_vint128 (" t_even = ", (vui128_t)t_even);
  print_vint128 (" t_odd  = ", (vui128_t)t_odd);
  /* Sum the low 128 bits of odd previous partial products */
  /* todo is there a possible carry out of this */
  __asm__(
      "vadduqm %0,%0,%1;\n"
      : "+v" (t_odd)
      : "v" (t)
      : );
  print_vint128 (" t_odd+t= ", (vui128_t)t_odd);
  /* rotate right the low 32-bits into tmq */
  tmq = vec_sld (t_odd, tmq, 12);
  print_vint128 (" tmq    = ", (vui128_t)tmq);
  /* shift the low 128 bits of partial product right 32-bits */
  t_odd = vec_sro (t_odd, (vui8_t)sro_4);
  /* add the top 128 bits of even / odd partial products */
  __asm__(
      "vadduqm %0,%1,%2;\n"
      : "=v" (t)
      : "v" (t_even),
      "v" (t_odd)
      : );
#else
#warning Implememention pre power8
#endif
  return (tmq);
}
#endif

vui32_t
db_vec_muludq (vui32_t *mulu, vui32_t a, vui32_t b)
{
  vui32_t t, tmq;
  /* compute the 256 bit product of two 128 bit values a, b.
   * The high 128 bits are accumulated in t and the low 128-bits
   * in tmq. The high 128-bits of the product are returned to the
   * address of the 1st parm. The low 128-bits are the return
   * value.
   *
   * We use the Vector Multiple Even/Odd Unsigned word to compute
   * the 128 x 32 partial (160-bit) product of value a with the
   * word splat of b. These instructions (vmuleum, vmuloum)
   * product four 64-bit 32 x 32 partial products where even
   * results are shifted 32-bit left from odd results. After
   * shifting the high 128 bits can be summed via Vector add
   * unsigned quadword.
   *
   *
   *
   */
#ifdef _ARCH_PWR8
  vui32_t tsw;
  vui32_t tc;
  vui32_t t_odd, t_even;
  vui32_t z=
    { 0,0,0,0};

  tsw = vec_splat (b, 3);
  printf ("db_vec_muluq\n");
  print_vint128 ("  a = ", (vui128_t)a);
  print_vint128 ("  b = ", (vui128_t)b);
  print_vint128 ("tsw = ", (vui128_t)tsw);
  /* t_even = vec_vmuleum(a, tsw) */
  __asm__(
      "vmuleuw %0,%1,%2;\n"
      : "=v" (t_even)
      : "v" (a),
      "v" (tsw)
      : );
  /* t_odd  = vec_vmuloum(a, tsw) */
  __asm__(
      "vmulouw %0,%1,%2;\n"
      : "=v" (t_odd)
      : "v" (a),
      "v" (tsw)
      : );
  /* Rotate the low 32-bits (right) into tmq. This is actually
   * implemented as 96-bit (12-byte) shift left. */
  tmq = vec_sld (t_odd, z, 12);
  print_vint128 (" t_even = ", (vui128_t)t_even);
  print_vint128 (" t_odd  = ", (vui128_t)t_odd);
  print_vint128 (" tmq    = ", (vui128_t)tmq);
  /* shift the low 128 bits of partial product right 32-bits */
  t_odd = vec_sld (z, t_odd, 12);
  print_vint128 (" t_odd  = ", (vui128_t)t_odd);
  /* add the high 128 bits of even / odd partial products */
  __asm__(
      "vadduqm %0,%1,%2;\n"
      : "=v" (t)
      : "v" (t_even),
      "v" (t_odd)
      : );

  print_vint128 (" t      = ", (vui128_t)t);
  tsw = vec_splat (b, 2);
  print_vint128 ("tsw = ", (vui128_t)tsw);
  /* t_even = vec_vmuleum(a, tsw) */
  __asm__(
      "vmuleuw %0,%1,%2;\n"
      : "=v" (t_even)
      : "v" (a),
      "v" (tsw)
      : );
  /* t_odd  = vec_vmuloum(a, tsw) */
  __asm__(
      "vmulouw %0,%1,%2;\n"
      : "=v" (t_odd)
      : "v" (a),
      "v" (tsw)
      : );
  print_vint128 (" t_even = ", (vui128_t)t_even);
  print_vint128 (" t_odd  = ", (vui128_t)t_odd);
  /* Sum the low 128 bits of odd previous partial products */
  __asm__(
      "vaddcuq %0,%1,%2;\n"
      : "=v" (tc)
      : "v" (t_odd),
      "v" (t)
      : );
  __asm__(
      "vadduqm %0,%0,%1;\n"
      : "+v" (t_odd)
      : "v" (t)
      : );
  print_vint128 (" t_odd+t= ", (vui128_t)t_odd);
  print_vint128 ("  carry = ", (vui128_t)tc);
  /* rotate right the low 32-bits into tmq */
  tmq = vec_sld (t_odd, tmq, 12);
  print_vint128 (" tmq    = ", (vui128_t)tmq);
  /* shift the low 128 bits of partial product right 32-bits */
  t_odd = vec_sld (tc, t_odd, 12);
  print_vint128 (" t_odd  = ", (vui128_t)t_odd);
  /* add the top 128 bits of even / odd partial products */
  __asm__(
      "vadduqm %0,%1,%2;\n"
      : "=v" (t)
      : "v" (t_even),
      "v" (t_odd)
      : );

  print_vint128 (" t      = ", (vui128_t)t);
  tsw = vec_splat (b, 1);
  /* t_even = vec_vmuleum(a, tsw) */
  __asm__(
      "vmuleuw %0,%1,%2;\n"
      : "=v" (t_even)
      : "v" (a),
      "v" (tsw)
      : );
  /* t_odd  = vec_vmuloum(a, tsw) */
  __asm__(
      "vmulouw %0,%1,%2;\n"
      : "=v" (t_odd)
      : "v" (a),
      "v" (tsw)
      : );
  print_vint128 (" t_even = ", (vui128_t)t_even);
  print_vint128 (" t_odd  = ", (vui128_t)t_odd);
  /* Sum the low 128 bits of odd previous partial products */
  __asm__(
      "vaddcuq %0,%1,%2;\n"
      : "=v" (tc)
      : "v" (t_odd),
      "v" (t)
      : );
  __asm__(
      "vadduqm %0,%0,%1;\n"
      : "+v" (t_odd)
      : "v" (t)
      : );
  print_vint128 (" t_odd+t= ", (vui128_t)t_odd);
  print_vint128 ("  carry = ", (vui128_t)tc);
  /* rotate right the low 32-bits into tmq */
  tmq = vec_sld (t_odd, tmq, 12);
  print_vint128 (" tmq    = ", (vui128_t)tmq);
  /* shift the low 128 bits of partial product right 32-bits */
  t_odd = vec_sld (tc, t_odd, 12);
  print_vint128 (" t_odd  = ", (vui128_t)t_odd);
  /* add the top 128 bits of even / odd partial products */
  __asm__(
      "vadduqm %0,%1,%2;\n"
      : "=v" (t)
      : "v" (t_even),
      "v" (t_odd)
      : );
  print_vint128 (" t      = ", (vui128_t)t);

  tsw = vec_splat (b, 0);
  /* t_even = vec_vmuleum(a, tsw) */
  __asm__(
      "vmuleuw %0,%1,%2;\n"
      : "=v" (t_even)
      : "v" (a),
      "v" (tsw)
      : );
  /* t_odd  = vec_vmuloum(a, tsw) */
  __asm__(
      "vmulouw %0,%1,%2;\n"
      : "=v" (t_odd)
      : "v" (a),
      "v" (tsw)
      : );
  print_vint128 (" t_even = ", (vui128_t)t_even);
  print_vint128 (" t_odd  = ", (vui128_t)t_odd);
  /* Sum the low 128 bits of odd previous partial products */
  __asm__(
      "vaddcuq %0,%1,%2;\n"
      : "=v" (tc)
      : "v" (t_odd),
      "v" (t)
      : );
  __asm__(
      "vadduqm %0,%0,%1;\n"
      : "+v" (t_odd)
      : "v" (t)
      : );
  print_vint128 (" t_odd+t= ", (vui128_t)t_odd);
  print_vint128 ("  carry = ", (vui128_t)tc);
  /* rotate right the low 32-bits into tmq */
  tmq = vec_sld (t_odd, tmq, 12);
  print_vint128 (" tmq    = ", (vui128_t)tmq);
  /* shift the low 128 bits of partial product right 32-bits */
  t_odd = vec_sld (tc, t_odd, 12);
  print_vint128 (" t_odd  = ", (vui128_t)t_odd);
  /* add the top 128 bits of even / odd partial products */
  __asm__(
      "vadduqm %0,%1,%2;\n"
      : "=v" (t)
      : "v" (t_even),
      "v" (t_odd)
      : );
  print_vint128 (" t      = ", (vui128_t)t);
#else
//#warning Implememention pre power8 missing
#endif
  *mulu = t;
  return (tmq);
}

vui32_t
db_vec_addeuqm (vui32_t a, vui32_t b, vui32_t c)
{
  vui32_t t;
#ifdef _ARCH_PWR8
  __asm__(
      "vaddeuqm %0,%1,%2,%3;"
      : "=v" (t)
      : "v" (a),
      "v" (b),
      "v" (c)
      : );
#else
  vui32_t c2;
  vui32_t z = { 0, 0, 0, 0 };
  vui32_t m = { 0, 0, 0, 1 };
  printf ("db_vec_addeuqm\n");
  print_vint128 ("  a = ", (vui128_t)a);
  print_vint128 ("  b = ", (vui128_t)b);
  print_vint128 ("  c = ", (vui128_t)c);
  print_vint128 ("  z = ", (vui128_t)z);
  print_vint128 ("  m = ", (vui128_t)m);
  __asm__(
      "vand %2,%1,%6;\n"
      "\tvaddcuw %1,%3,%4;\n"
      "\tvsldoi %2,%2,%5,12;\n"
      "\tvsldoi %1,%1,%2,4;\n"
      "\tvadduwm %0,%3,%4;\n"
      "\tvaddcuw %2,%0,%1;\n"
      "\tvadduwm %0,%0,%1;\n"
      "\tvsldoi %1,%2,%5,4;\n"
      "\tvaddcuw %2,%0,%1;\n"
      "\tvadduwm %0,%0,%1;\n"
      "\tvsldoi %1,%2,%5,4;\n"
      "\tvaddcuw %2,%0,%1;\n"
      "\tvadduwm %0,%0,%1;\n"
      "\tvsldoi %1,%2,%5,4;\n"
      "\tvadduwm %0,%0,%1;\n"
      : "=&v" (t), /* 0 */
      "+&v" (c), /* 1 */
      "=&v" (c2) /* 2 */
      : "v" (a), /* 3 */
      "v" (b), /* 4 */
      "v" (z), /* 5 */
      "v" (m) /* 6 */
      : );
  print_vint128 ("  t = ", (vui128_t)t);
  print_vint128 ("  c = ", (vui128_t)c);
  print_vint128 ("  c2= ", (vui128_t)c2);
#endif
  return (t);
}

vui32_t
db_vec_addeq (vui32_t *cout, vui32_t a, vui32_t b, vui32_t c)
{
  vui32_t t, co;
#ifdef _ARCH_PWR8
  __asm__(
      "vaddeuqm %0,%2,%3,%4;\n"
      "\tvaddecuq %1,%2,%3,%4;"
      : "=v" (t),
      "=v" (co)
      : "v" (a),
      "v" (b),
      "v" (c)
      : );
#else
  vui32_t c2;
  vui32_t z = { 0, 0, 0, 0 };

  co = (vui32_t ) { 0, 0, 0, 1 };

  printf ("db_vec_addeq\n");
  print_vint128 ("  a = ", (vui128_t)a);
  print_vint128 ("  b = ", (vui128_t)b);
  print_vint128 ("  c = ", (vui128_t)c);
  print_vint128 ("  z = ", (vui128_t)z);
  print_vint128 ("  co= ", (vui128_t)co);
  __asm__(
      "vand %3,%1,%2;\n"
      "\tvaddcuw %2,%4,%5;\n"
      "\tvsldoi %3,%3,%5,12;\n"
      "\tvsldoi %1,%2,%3,4;\n"
      "\tvadduwm %0,%4,%5;\n"
      "\tvaddcuw %3,%0,%1;\n"
      "\tvadduwm %0,%0,%1;\n"
      "\tvor %2,%2,%3;\n"
      "\tvsldoi %1,%3,%6,4;\n"
      "\tvaddcuw %3,%0,%1;\n"
      "\tvadduwm %0,%0,%1;\n"
      "\tvor %2,%2,%3;\n"
      "\tvsldoi %1,%3,%6,4;\n"
      "\tvaddcuw %3,%0,%1;\n"
      "\tvadduwm %0,%0,%1;\n"
      "\tvor %2,%2,%3;\n"
      "\tvsldoi %1,%3,%6,4;\n"
      "\tvaddcuw %3,%0,%1;\n"
      "\tvadduwm %0,%0,%1;\n"
      "\tvor %2,%2,%3;\n"
      "\tvsldoi %2,%6,%2,4;\n"
      : "=&v" (t), /* 0 */
      "+&v" (c), /* 1 */
      "+&v" (co), /* 2 */
      "=&v" (c2) /* 3 */
      : "v" (a), /* 4 */
      "v" (b), /* 5 */
      "v" (z) /* 6 */
      : );
  print_vint128 ("  t = ", (vui128_t)t);
  print_vint128 ("  c = ", (vui128_t)c);
  print_vint128 ("  co= ", (vui128_t)co);
  print_vint128 ("  c2= ", (vui128_t)c2);
#endif
  *cout = co;
  return (t);
}

vui128_t
db_vec_mul10uq (vui128_t a)
{
  vui32_t t;
#ifdef _ARCH_PWR9
  __asm__(
      "vmul10uq %0,%1;\n"
      : "=v" (t)
      : "v" (a)
      : );
#else
  vui16_t ts = (vui16_t) a;
  vui16_t t10;
  vui32_t t_odd, t_even;
  vui32_t z = { 0, 0, 0, 0 };
  t10 = vec_splat_u16 (10);

  print_vint128 ("vec_mul10uq: ts     ", (vui128_t) ts);
  print_vint128 ("vec_mul10uq: t10    ", (vui128_t) t10);

  t_even = vec_vmuleuh (ts, t10);
  t_odd = vec_vmulouh (ts, t10);

  print_vint128 ("vec_mul10uq: t_odd  ", (vui128_t) t_odd);
  print_vint128 ("vec_mul10uq: t_even ", (vui128_t) t_even);

  /* Just using t10 to supply zeros into the low 16 bits of t_odd */
  t_even = vec_sld (t_even, z, 2);

  print_vint128 ("vec_mul10uq: t_even ", (vui128_t) t_even);
#ifdef _ARCH_PWR8
  t = (vui32_t)vec_vadduqm ((vui128_t)t_even, (vui128_t)t_odd);
#else
  vui32_t c, c2;
  __asm__(
      "vaddcuw %1,%3,%4;\n"
      "\tvadduwm %0,%3,%4;\n"
      "\tvsldoi %1,%1,%5,4;\n"
      "\tvaddcuw %2,%0,%1;\n"
      "\tvadduwm %0,%0,%1;\n"
      "\tvsldoi %1,%2,%5,4;\n"
      "\tvaddcuw %2,%0,%1;\n"
      "\tvadduwm %0,%0,%1;\n"
      "\tvsldoi %1,%2,%5,4;\n"
      "\tvadduwm %0,%0,%1;\n"
      : "=&v" (t),
      "=&v" (c),
      "=&v" (c2)
      : "v" (t_even),
      "v" (t_odd),
      "v" (z)
      : );
#endif
#endif
  return ((vui128_t) t);
}

vui128_t
db_vec_mul10cuq (vui128_t a)
{
  vui32_t t, t_carry;
#ifdef _ARCH_PWR9
  __asm__(
      "vmul10cuq %0,%1;\n"
      : "=v" (t_carry)
      : "v" (a)
      : );
#else
  vui16_t ts = (vui16_t) a;
  vui16_t t10;
  vui32_t t_odd, t_even, t_high;
  vui32_t z = { 0, 0, 0, 0 };
  t10 = vec_splat_u16 (10);

  print_vint128 ("vec_mul10cuq: ts     ", (vui128_t) ts);
  print_vint128 ("vec_mul10cuq: t10    ", (vui128_t) t10);

  t_even = vec_vmuleuh (ts, t10);
  t_odd = vec_vmulouh (ts, t10);

  print_vint128 ("vec_mul10cuq: t_odd  ", (vui128_t) t_odd);
  print_vint128 ("vec_mul10cuq: t_even ", (vui128_t) t_even);

  /* Shift t_even left 16-bits (right 112-bits) for the partial carry.  */
  t_high = vec_sld (z, t_even, 2);
  /* Shift t_even left 16 bits to align for lower 128-bits. */
  t_even = vec_sld (t_even, z, 2);

  print_vint128 ("vec_mul10cuq: t_high ", (vui128_t) t_high);
  print_vint128 ("vec_mul10cuq: t_even ", (vui128_t) t_even);
#ifdef _ARCH_PWR8
  t = (vui32_t)vec_vadduqm ((vui128_t)t_even, (vui128_t)t_odd);
  t_carry = (vui32_t)vec_vaddcuq ((vui128_t)t_even, (vui128_t)t_odd);
  t_carry = (vui32_t)vec_vadduqm ((vui128_t)t_carry, (vui128_t)t_high);
  print_vint128 ("vec_mul10cuq: t       ", (vui128_t)t);
  print_vint128 ("vec_mul10cuq: t_carry ", (vui128_t)t_carry);
#else
  vui32_t c, c2;
  __asm__(
      "vaddcuw %1,%3,%4;\n"
      "\tvadduwm %0,%3,%4;\n"
      "\tvsldoi %2,%5,%1,4;\n"
      "\tvsldoi %1,%1,%5,4;\n"
      "\tvadduwm %0,%0,%1;\n"
      : "=&v" (t),
      "=&v" (c),
      "=&v" (c2)
      : "v" (t_even),
      "v" (t_odd),
      "v" (z)
      : );
  __asm__(
      "vadduwm %0,%1,%2;\n"
      : "=v" (t_carry)
      : "v" (t_high),
      "v" (c2)
      : );
#endif
#endif
  return ((vui128_t) t_carry);
}

#endif

int
test_addcq (void)
{
  vui32_t i, j, k, l;
  vui32_t e, ec;
  int rc = 0;

  printf ("\ntest_addcq Vector add carry int128\n");

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t)
	CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
	    __UINT32_MAX__);
  k = (vui32_t) vec_addcq ((vui128_t*)&l, (vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E128-1", k, i, j);
  print_vint128x ("  c = ", (vui128_t) l);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_addcq=:", (vui128_t) k, (vui128_t) e);
  rc += check_vuint128x ("       co :", (vui128_t) l, (vui128_t) ec);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t)
	CONST_VINT32_W(0xfffeffff, __UINT32_MAX__, __UINT32_MAX__,
	    __UINT32_MAX__);
  k = (vui32_t) vec_addcq ((vui128_t*)&l, (vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E128-2E102", k, i, j);
  print_vint128x ("  c = ", (vui128_t) l);
#endif
  e = (vui32_t)CONST_VINT32_W(0xffff0000, 0x00000000, 0x00000000, 0x00000000);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_addcq=:", (vui128_t) k, (vui128_t) e);
  rc += check_vuint128x ("       co :", (vui128_t) l, (vui128_t) ec);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t)
	CONST_VINT32_W(0xfffffffe, __UINT32_MAX__, __UINT32_MAX__,
	    __UINT32_MAX__);
  k = (vui32_t) vec_addcq ((vui128_t*)&l, (vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E128-2E96", k, i, j);
  print_vint128x ("  c = ", (vui128_t) l);
#endif
  e = (vui32_t
	)CONST_VINT32_W(__UINT32_MAX__, 0x00000000, 0x00000000, 0x00000000);
  ec = (vui32_t
	)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_addcq=:", (vui128_t) k, (vui128_t) e);
  rc += check_vuint128x ("       co :", (vui128_t) l, (vui128_t) ec);

  i = (vui32_t)CONST_VINT32_W(0, 0, 1, 0);
  j = (vui32_t)
	CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
	    0);
  k = (vui32_t) vec_addcq ((vui128_t*)&l, (vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E128-2E32-1", k, i, j);
  print_vint128x ("  c = ", (vui128_t) l);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_addcq=:", (vui128_t) k, (vui128_t) e);
  rc += check_vuint128x ("       co :", (vui128_t) l, (vui128_t) ec);

  i = (vui32_t)CONST_VINT32_W(0, 1, 2, 3);
  j = (vui32_t)
	CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, 0, 0);
  k = (vui32_t) vec_addcq ((vui128_t*)&l, (vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E128-2E64-1", k, i, j);
  print_vint128x ("  c = ", (vui128_t) l);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 2, 3);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_addcq=:", (vui128_t) k, (vui128_t) e);
  rc += check_vuint128x ("       co :", (vui128_t) l, (vui128_t) ec);

  i = (vui32_t
	)CONST_VINT32_W(1, 2, 3, 4);
  j = (vui32_t
	)
	CONST_VINT32_W(__UINT32_MAX__, 0, 0,
	    0);
  k = (vui32_t) vec_addcq ((vui128_t*)&l, (vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E128-2E96-1", k, i, j);
  print_vint128x ("  c = ", (vui128_t) l);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 2, 3, 4);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_addcq=:", (vui128_t) k, (vui128_t) e);
  rc += check_vuint128x ("       co :", (vui128_t) l, (vui128_t) ec);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_addeq (void)
{
  vui32_t i, j, k, l, m;
  vui32_t e, ec;
  int rc = 0;

  printf ("\ntest_addeq Vector add extend carry int128\n");

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addeq ((vui128_t*) &m, (vui128_t) i, (vui128_t) j,
                           (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_extend ("extent 2E32-1 + 2E32-1 + c=1", k, m, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000001, 0xffffffff);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint256 ("vec_addeq:", (vui128_t) m, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  k = (vui32_t) vec_addeq ((vui128_t*) &m, (vui128_t) i, (vui128_t) j,
                           (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_extend ("extend 1 + 2E128-1 + c=0", k, m, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vint256 ("vec_addeq:", (vui128_t) m, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  j = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addeq ((vui128_t*) &m, (vui128_t) i, (vui128_t) j,
                           (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_extend ("extend 0 + 2E128-1 + c=1", k, m, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vint256 ("vec_addeq:", (vui128_t) m, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 2);
  j = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addeq ((vui128_t*) &m, (vui128_t) i, (vui128_t) j,
                           (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_extend ("extend 2 + 2E128-1 + c=1", k, m, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000002);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vint256 ("vec_addeq:", (vui128_t) m, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 1, 0);
  j = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addeq ((vui128_t*) &m, (vui128_t) i, (vui128_t) j,
                           (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_extend ("extend 2E32 + 2E128-1-2E32-1 + c=1", k, m, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vint256 ("vec_addeq:", (vui128_t) m, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  j = (vui32_t )
          CONST_VINT32_W(0xfffeffff, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addeq ((vui128_t*) &m, (vui128_t) i, (vui128_t) j,
                           (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_extend ("extend 0 + 2E128-1-2E102 + c=1", k, m, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0xffff0000, 0x00000000, 0x00000000, 0x00000000);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint256 ("vec_addeq:", (vui128_t) m, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t )
          CONST_VINT32_W(0xfffeffff, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addeq ((vui128_t*) &m, (vui128_t) i, (vui128_t) j,
                           (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_extend ("extend 1 + 2E128-1-2E102 + c=1", k, m, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0xffff0000, 0x00000000, 0x00000000, 0x00000001);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint256 ("vec_addeq:", (vui128_t) m, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  return (rc);
}
//#undef __DEBUG_PRINT__

int
test_addq (void)
{
#ifdef __DEBUG_PRINT__
  unsigned __int128 a, b, c;
#endif
  vui32_t i, j, k, l;
  vui32_t e, ec;
  int rc = 0;

  rc += test_addcq ();

  rc += test_addeq ();

  printf ("\ntest_addq Vector add unsigned quadword modulo\n");

#ifdef __DEBUG_PRINT__
  a = 1UL;
  b = __UINT32_MAX__;
  c = a + b;

  print_int128_sum ("1 + 2E32-1", c, a, b);
#endif
  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  k = (vui32_t) vec_adduqm ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E32-1", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000001, 0x00000000);
  rc += check_vuint128x ("vec_adduqm:", (vui128_t) k, (vui128_t) e);

#ifdef __DEBUG_PRINT__
  a = __UINT32_MAX__;
  b = __UINT32_MAX__;
  c = a + b;

  print_int128_sum ("2E32-1 + 2E32-1", c, a, b);
#endif
  i = (vui32_t )CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  k = (vui32_t) vec_adduqm ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2E32-1 + 2E32-1", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000001, 0xfffffffe);
  rc += check_vuint128x ("vec_adduqm:", (vui128_t) k, (vui128_t) e);

#ifdef __DEBUG_PRINT__
  a = 1UL;
  b = __UINTMAX_MAX__;
  c = a + b;

  print_int128_sum ("1 + 2E64-1", c, a, b);
#endif
  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t )CONST_VINT32_W(0, 0, __UINT32_MAX__, __UINT32_MAX__);
  k = (vui32_t) vec_adduqm ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E64-1", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000001, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_adduqm:", (vui128_t) k, (vui128_t) e);

#ifdef __DEBUG_PRINT__
  a = 1UL;
  b = __UINTMAX_MAX__;
  b = b << 32;
  b = b + __UINT32_MAX__;
  c = a + b;

  print_int128_sum ("1 + 2E96-1", c, a, b);
#endif
  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t )
          CONST_VINT32_W(0, __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__);
  k = (vui32_t) vec_adduqm ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E96-1", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000001, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_adduqm:", (vui128_t) k, (vui128_t) e);

#ifdef __DEBUG_PRINT__
  a = 1UL;
  b = __UINTMAX_MAX__;
  b = b << 64;
  b = b + __UINTMAX_MAX__;
  c = a + b;

  print_int128_sum ("1 + 2E128-1", c, a, b);
#endif
  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  k = (vui32_t) vec_adduqm ((vui128_t) i, (vui128_t) j);
  l = (vui32_t) vec_addcuq ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E128-1", k, i, j);
  print_vint128x     ("  c = ", (vui128_t)l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_adduqm:", (vui128_t) k, (vui128_t) e);
  rc += check_vuint128x ("vec_addcuq:", (vui128_t) l, (vui128_t) ec);

  i = (vui32_t )CONST_VINT32_W(0, 0, 1, 0);
  j = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0);
  k = (vui32_t) vec_adduqm ((vui128_t) i, (vui128_t) j);
  l = (vui32_t) vec_addcuq ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E128-1", k, i, j);
  print_vint128x     ("  c = ", (vui128_t)l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_adduqm:", (vui128_t) k, (vui128_t) e);
  rc += check_vuint128x ("vec_addcuq:", (vui128_t) l, (vui128_t) ec);

  i = (vui32_t )CONST_VINT32_W(0, 1, 0, 0);
  j = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, 0,
                         0);
  k = (vui32_t) vec_adduqm ((vui128_t) i, (vui128_t) j);
  l = (vui32_t) vec_addcuq ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E128-1", k, i, j);
  print_vint128x     ("  c = ", (vui128_t)l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_adduqm:", (vui128_t) k, (vui128_t) e);
  rc += check_vuint128x ("vec_addcuq:", (vui128_t) l, (vui128_t) ec);

  i = (vui32_t )CONST_VINT32_W(1, 0, 0, 0);
  j = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, 0, 0,
                         0);
  k = (vui32_t) vec_adduqm ((vui128_t) i, (vui128_t) j);
  l = (vui32_t) vec_addcuq ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E128-1", k, i, j);
  print_vint128x     ("  c = ", (vui128_t)l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_adduqm:", (vui128_t) k, (vui128_t) e);
  rc += check_vuint128x ("vec_addcuq:", (vui128_t) l, (vui128_t) ec);

  i = (vui32_t )CONST_VINT32_W(1, 1, 1, 1);
  j = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, 0, 0,
                         0);
  k = (vui32_t) vec_adduqm ((vui128_t) i, (vui128_t) j);
  l = (vui32_t) vec_addcuq ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E128-1", k, i, j);
  print_vint128x     ("  c = ", (vui128_t)l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000001, 0x00000001, 0x00000001);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_adduqm:", (vui128_t) k, (vui128_t) e);
  rc += check_vuint128x ("vec_addcuq:", (vui128_t) l, (vui128_t) ec);

  i = (vui32_t )CONST_VINT32_W(1, __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__);
  j = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, 0, 0,
                         0);
  k = (vui32_t) vec_adduqm ((vui128_t) i, (vui128_t) j);
  l = (vui32_t) vec_addcuq ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E128-1", k, i, j);
  print_vint128x     ("  c = ", (vui128_t)l);
#endif
  e = (vui32_t )CONST_VINT32_W(0, __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_adduqm:", (vui128_t) k, (vui128_t) e);
  rc += check_vuint128x ("vec_addcuq:", (vui128_t) l, (vui128_t) ec);

  i = (vui32_t )CONST_VINT32_W(0, __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__);
  j = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, 0, 0,
                         0);
  k = (vui32_t) vec_adduqm ((vui128_t) i, (vui128_t) j);
  l = (vui32_t) vec_addcuq ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E128-1", k, i, j);
  print_vint128x     ("  c = ", (vui128_t)l);
#endif
  e = (vui32_t )CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_adduqm:", (vui128_t) k, (vui128_t) e);
  rc += check_vuint128x ("vec_addcuq:", (vui128_t) l, (vui128_t) ec);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addeuqm ((vui128_t) i, (vui128_t) j, (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_carry ("1 + 2E32-1 + c=1", k, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000001, 0x00000001);
  rc += check_vuint128x ("vec_addeuqm:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addeuqm ((vui128_t) i, (vui128_t) j, (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_carry ("2E32-1 + 2E32-1 + c=1", k, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000001, 0xffffffff);
  rc += check_vuint128x ("vec_addeuqm:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addecuq ((vui128_t) i, (vui128_t) j, (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_carry ("carry 2E32-1 + 2E32-1 + c=1", k, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_addecuq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  k = (vui32_t) vec_addeuqm ((vui128_t) i, (vui128_t) j, (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_carry ("1 + 2E128-1 + c=0", k, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_addeuqm:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  k = (vui32_t) vec_addecuq ((vui128_t) i, (vui128_t) j, (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_carry ("carry 1 + 2E128-1 + c=0", k, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_addecuq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  j = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addeuqm ((vui128_t) i, (vui128_t) j, (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_carry ("0 + 2E128-1 + c=1", k, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_addeuqm:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  j = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addecuq ((vui128_t) i, (vui128_t) j, (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_carry ("carry 0 + 2E128-1 + c=1", k, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_addecuq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addeuqm ((vui128_t) i, (vui128_t) j, (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_carry ("1 + 2E128-1 + c=1", k, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_addeuqm:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 2);
  j = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addecuq ((vui128_t) i, (vui128_t) j, (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_carry ("carry 2 + 2E128-1 + c=1", k, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_addecuq:", (vui128_t) k, (vui128_t) e);

  return (rc);
}


int
test_mul10uq (void)
{
  vui32_t i, e /*j, l, m*/;
  vui128_t k;
  int ii;
  int rc = 0;

  printf ("\ntest_mul10uq Vector Multiply by 10 unsigned quadword\n");

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 10);
  k = vec_mul10uq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("1 * 10 ", k);
  print_vint128x("1 * 10 ", k);
#endif
  rc += check_vuint128 ("vec_mul10uq:", k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 65535);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 655350);
  k = vec_mul10uq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("65535 * 10 ", k);
  print_vint128x("65535 * 10 ", (vui128_t)e);
#endif
  rc += check_vuint128 ("vec_mul10uq:", k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 65536);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 0xa0000U);
  k = vec_mul10uq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("65536 * 10 ", k);
  print_vint128x("65536 * 10 ", (vui128_t)e);
#endif
  rc += check_vuint128 ("vec_mul10uq:", k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 65537);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 0xa000aU);
  k = vec_mul10uq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("65537 * 10 ", k);
  print_vint128x("65537 * 10 ", (vui128_t)e);
#endif
  rc += check_vuint128 ("vec_mul10uq:", k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0x00000009U, 0xfffffff6U);
  k = vec_mul10uq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("2E32-1 * 10 ", k);
  print_vint128x("2E32-1 * 10 ", (vui128_t)e);
#endif
  rc += check_vuint128 ("vec_mul10uq:", k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 65535, __UINT32_MAX__);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0x9ffffU, 0xfffffff6U);
  k = vec_mul10uq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("2E48-1 * 10 ", k);
  print_vint128x("2E48-1 * 10 ", (vui128_t)e);
#endif
  rc += check_vuint128 ("vec_mul10uq:", k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, __UINT32_MAX__, __UINT32_MAX__);
  e = (vui32_t )CONST_VINT32_W(0, 0x00000009U, __UINT32_MAX__, 0xfffffff6U);
  k = vec_mul10uq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("2E64-1 * 10 ", k);
  print_vint128x("2E64-1 * 10 ", (vui128_t)e);
#endif
  rc += check_vuint128 ("vec_mul10uq:", k, (vui128_t) e);

  i = (vui32_t )
          CONST_VINT32_W(0, __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__);
  e = (vui32_t )
          CONST_VINT32_W(0x00000009U, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffff6U);
  k = vec_mul10uq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("2E96-1 * 10 ", k);
  print_vint128x("2E96-1 * 10 ", (vui128_t)e);
#endif
  rc += check_vuint128 ("vec_mul10uq:", k, (vui128_t) e);

  i = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  e = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffff6U);
  k = vec_mul10uq ((vui128_t) i);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1 * 10 ", k);
  print_vint128x("2E128-1 * 10 ", (vui128_t)e);
#endif
  rc += check_vuint128 ("vec_mul10uq:", k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);

#ifdef __DEBUG_PRINT__
  print_vint128 ("init i=1", (vui128_t)i);
#endif
  for (ii = 1; ii < 39; ii++)
    {
      k = vec_mul10uq ((vui128_t) i);

      rc += check_vuint128 ("vec_mul10uq:", k, vtipowof10[ii]);
#ifdef __DEBUG_PRINT__
      print_vint128 ("x * 10 ", k);
#endif
      i = (vui32_t) k;
    }

  if (rc)
    printf ("\n%s Vector Multiply by 10 %d errors\n", __FUNCTION__, rc);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_mul10ecuq (void)
{
  vui32_t i, e, ec;
  vui128_t j, k, l;
  int rc = 0;

  printf ("\ntest_mul10ecuq Vector Multiply by 10 unsigned quadword carry/extend\n");
  i = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  e = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffff6);
  ec = (vui32_t )CONST_VINT32_W(0, 0, 0, 9);

  k = vec_mul10uq ((vui128_t) i);
#if 1
  j = vec_mul10cuq ((vui128_t) i);
#else
  j = db_vec_mul10cuq ((vui128_t)i);
#endif

#ifdef __DEBUG_PRINT__
  print_vint128 ("\n2E128-1 = ec ", (vui128_t)ec);
  print_vint128 ("2E128-1 * 10 ", k);
  print_vint128 ("2E128-1 *10c ", j);
  print_vint128x("2E128-1 * 10 ", k);
  print_vint128x("2E128-1 *10c ", j);
#endif
  rc += check_vint256 ("vec_mul10cuq:", j, k, (vui128_t) ec, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0x19999999, 0x99999999, 0x99999999, 0x99999999);
  e = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffffa);
  ec = (vui32_t )CONST_VINT32_W(0, 0, 0, 0);

  k = vec_mul10uq ((vui128_t) i);
  j = vec_mul10cuq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("\n2E128-1 = ec ", (vui128_t)ec);
  print_vint128 ("2E128-1 * 10 ", k);
  print_vint128 ("2E128-1 *10c ", j);
  print_vint128x("2E128-1 * 10 ", k);
  print_vint128x("2E128-1 *10c ", j);
#endif
  rc += check_vint256 ("vec_mul10cuq 1:", j, k, (vui128_t) ec, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0x19999999, 0x99999999, 0x99999999, 0x9999999a);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 4);
  ec = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);

  k = vec_mul10uq ((vui128_t) i);
  j = vec_mul10cuq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("\n2E128-1 = ec ", (vui128_t)ec);
  print_vint128 ("2E128-1 * 10 ", k);
  print_vint128 ("2E128-1 *10c ", j);
  print_vint128x("2E128-1 * 10 ", k);
  print_vint128x("2E128-1 *10c ", j);
#endif
  rc += check_vint256 ("vec_mul10cuq 2:", j, k, (vui128_t) ec, (vui128_t) e);

  i = (vui32_t )
          CONST_VINT32_W(0x7fffffff, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  e = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffff6);
  ec = (vui32_t )CONST_VINT32_W(0, 0, 0, 4);

  k = vec_mul10uq ((vui128_t) i);
  j = vec_mul10cuq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("\n2E128-1 = ec ", (vui128_t)ec);
  print_vint128 ("2E128-1 * 10 ", k);
  print_vint128 ("2E128-1 *10c ", j);
  print_vint128x("2E128-1 * 10 ", k);
  print_vint128x("2E128-1 *10c ", j);
#endif
  rc += check_vint256 ("vec_mul10cuq 3:", j, k, (vui128_t) ec, (vui128_t) e);

  i = (vui32_t )
          CONST_VINT32_W(0x7fffffff, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  j = (vui128_t) (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  e = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffff6);
  k = vec_mul10euq ((vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
#endif
  rc += check_vuint128 ("vec_mul10euq 4:", k, (vui128_t) e);

  i = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  j = (vui128_t) (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  e = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffff6);
  k = vec_mul10euq ((vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
#endif
  rc += check_vuint128 ("vec_mul10euq 5:", k, (vui128_t) e);
  i = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  j = (vui128_t) (vui32_t )CONST_VINT32_W(0, 0, 0, 4);
  e = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffffa);
  k = vec_mul10euq ((vui128_t) i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
#endif
  rc += check_vuint128 ("vec_mul10euq 6:", k, (vui128_t) e);

  i = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  j = (vui128_t) (vui32_t )CONST_VINT32_W(0, 0, 0, 9);
  e = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  k = vec_mul10euq ((vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
#endif
  rc += check_vuint128 ("vec_mul10euq 7:", k, (vui128_t) e);

  i = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  j = (vui128_t) (vui32_t ) { 0, 0, 0, 0 };
  e = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffff6);
  ec = (vui32_t )CONST_VINT32_W(0, 0, 0, 9);
  k = vec_mul10euq ((vui128_t) i, j);
  l = vec_mul10ecuq ((vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
  print_vint128 ("2E128-1 *10c ", l);
  print_vint128x("2E128-1 *10c ", l);
#endif
  rc += check_vint256 ("vec_mul10ecuq 8:", l, k, (vui128_t) ec, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0x19999999, 0x99999999, 0x99999999, 0x99999999);
  j = (vui128_t) (vui32_t ) { 0, 0, 0, 0 };
  e = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffffa);
  ec = (vui32_t )CONST_VINT32_W(0, 0, 0, 0);

  k = vec_mul10euq ((vui128_t) i, j);
  l = vec_mul10ecuq ((vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
  print_vint128 ("2E128-1 *10c ", l);
  print_vint128x("2E128-1 *10c ", l);
#endif
  rc += check_vint256 ("vec_mul10ecuq 9:", l, k, (vui128_t) ec, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0x19999999, 0x99999999, 0x99999999, 0x99999999);
  j = (vui128_t) ((vui32_t )CONST_VINT32_W(0, 0, 0, 1));
  e = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffffb);
  ec = (vui32_t )CONST_VINT32_W(0, 0, 0, 0);

  k = vec_mul10euq ((vui128_t) i, j);
  l = vec_mul10ecuq ((vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
  print_vint128 ("2E128-1 *10c ", l);
  print_vint128x("2E128-1 *10c ", l);
#endif
  rc += check_vint256 ("vec_mul10ecuq 10:", l, k, (vui128_t) ec, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0x19999999, 0x99999999, 0x99999999, 0x99999999);
  j = (vui128_t) ((vui32_t )CONST_VINT32_W(0, 0, 0, 6));
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  ec = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);

  k = vec_mul10euq ((vui128_t) i, j);
  l = vec_mul10ecuq ((vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
  print_vint128 ("2E128-1 *10c ", l);
  print_vint128x("2E128-1 *10c ", l);
#endif
  rc += check_vint256 ("vec_mul10ecuq 11:", l, k, (vui128_t) ec, (vui128_t) e);

  return (rc);
}

#undef __DEBUG_PRINT__
//#define __DEBUG_PRINT__ 1
int
test_cmul100 (void)
{
  vui32_t i;
  vui32_t e, em, ec;
  vui128_t j, k, l, m, n;
  int ii;
  int rc = 0;

  printf ("\ntest_cmul100 Vector Multiply by 100\n");
  i = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  k = vec_cmul100cuq (&j, (vui128_t) i);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1 * 100 ", k);
  print_vint128 ("2E128-1 *100c ", j);
#endif
  e = (vui32_t )CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffff9c);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000063);
  rc += check_vint256 ("vec_cmul100cuq 1:", (vui128_t) j, (vui128_t) k,
                       (vui128_t) ec, (vui128_t) e);

  i = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  j = (vui128_t) (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  k = vec_cmul100ecuq (&l, (vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*100+e ", k);
  print_vint128 ("2E128-1 *100c ", l);
#endif
  e = (vui32_t )CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffff9c);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000063);
  rc += check_vint256 ("vec_cmul100ecuq 2:", (vui128_t) l, (vui128_t) k,
                       (vui128_t) ec, (vui128_t) e);

  i = (vui32_t ) { __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
          __UINT32_MAX__ };
  j = (vui128_t) (vui32_t )CONST_VINT32_W(0, 0, 0, 4);
  k = vec_cmul100ecuq (&l, (vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*100+e ", k);
  print_vint128 ("2E128-1 *100c ", l);
#endif
  e = (vui32_t )CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffa0);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000063);
  rc += check_vint256 ("vec_cmul100ecuq:", (vui128_t) l, (vui128_t) k,
                       (vui128_t) ec, (vui128_t) e);

  i = (vui32_t ) { __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
          __UINT32_MAX__ };
  j = (vui128_t) (vui32_t )CONST_VINT32_W(0, 0, 0, 9);
  k = vec_cmul100ecuq (&l, (vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*100+e ", k);
  print_vint128 ("2E128-1 *100c ", l);
#endif
  e = (vui32_t )CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffa5);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000063);
  rc += check_vint256 ("vec_cmul100ecuq 3:", (vui128_t) l, (vui128_t) k,
                       (vui128_t) ec, (vui128_t) e);

  i = (vui32_t ) { __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
          __UINT32_MAX__ };
  j = (vui128_t) (vui32_t )CONST_VINT32_W(0, 0, 0, 10);
  k = vec_cmul100ecuq (&l, (vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*100+e ", k);
  print_vint128 ("2E128-1 *100c ", l);
#endif
  e = (vui32_t )CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffa6);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000063);
  rc += check_vint256 ("vec_cmul100ecuq 4:", (vui128_t) l, (vui128_t) k,
                       (vui128_t) ec, (vui128_t) e);

  i = (vui32_t ) { __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
          __UINT32_MAX__ };
  j = (vui128_t) (vui32_t )CONST_VINT32_W(0, 0, 0, 99);
  k = vec_cmul100ecuq (&l, (vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*100+e ", k);
  print_vint128 ("2E128-1 *100c ", l);
#endif
  e = (vui32_t )CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000063);
  rc += check_vint256 ("vec_cmul100ecuq 5:", (vui128_t) l, (vui128_t) k,
                       (vui128_t) ec, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 100);
  m = (vui128_t) (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  e = (vui32_t )CONST_VINT32_W(0x7775a5f1, 0x71951000, 0x00000000, 0x00000000);
  ec = (vui32_t )CONST_VINT32_W(0x161bcca7, 0x119915b5, 0x0764b4ab, 0xe8652979);

  for (ii = 0; ii < 37; ii++)
    {
      k = vec_cmul100cuq (&j, (vui128_t) i);
      m = vec_cmul100ecuq (&l, m, j);
#ifdef __DEBUG_PRINT__
      print_vint256 ("x *=100 ", m, k);
#endif
      i = (vui32_t) k;
    }
  rc += check_vint256 ("vec_cmul100euq 6:", m, k, (vui128_t) ec, (vui128_t) e);

  n = (vui128_t) (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  e = (vui32_t )CONST_VINT32_W (0xd1640000, 0000000000, 0x00000000, 0x00000000);
  em = (vui32_t )CONST_VINT32_W(0x1db2332b, 0x93f1e94f, 0x637a5429, 0x0fe2a55c);
  ec = (vui32_t )CONST_VINT32_W(0x067f43fb, 0xe77a37f8, 0xb7309320, 0xc32b3cd3);

  for (ii = 37; ii < 56; ii++)
    {
      vui128_t x;
      k = vec_cmul100cuq (&j, (vui128_t) i);
      m = vec_cmul100ecuq (&l, m, j);
      n = vec_cmul100ecuq (&x, n, l);
#ifdef __DEBUG_PRINT__
      print_vint384 ("x *=10 ", n, m, k);
#endif
      i = (vui32_t) k;
    }
  rc += check_vint384 ("vec_cmul100ecuq 7:", n, m, k, (vui128_t) ec, (vui128_t) em,
                       (vui128_t) e);

  return (rc);
}

#ifdef __DEBUG_PRINT__
#define test_vec_mulluq(_i, _j)	db_vec_mulluq(_i, _j)
#else
#define test_vec_mulluq(_i, _j)	vec_mulluq(_i, _j)
#endif

int
test_mulluq (void)
{
  vui32_t i, j, k /*, l, m*/;
  vui32_t e;
  int rc = 0;

  printf ("\ntest_mulluq vector multiply low unsigned quad\n");

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) test_vec_mulluq((vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128_sum ("1 * 1 ", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  rc += check_vuint128 ("vec_mulluq 1:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 100);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 100);
  k = (vui32_t) test_vec_mulluq((vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128_sum ("100 * 100 ", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 10000);
  rc += check_vuint128 ("vec_mulluq 2:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x10000);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x10000);
  k = (vui32_t) test_vec_mulluq((vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("65536 * 65536 ", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0, 0, 0x00000001, 0x00000000);
  rc += check_vuint128x ("vec_mulluq 3:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0xffffffff);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 0xffffffff);
  k = (vui32_t) test_vec_mulluq((vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2**32-1 * 2**32-1 ", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0, 0, 0xfffffffe, 0x00000001);
  rc += check_vuint128x ("vec_mulluq 4:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0xffff, 0xffffffff);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0xffff, 0xffffffff);
  k = (vui32_t) test_vec_mulluq((vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2**48-1 * 2**48-1 ", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0, 0xffffffff, 0xfffe0000, 0x00000001);
  rc += check_vuint128x ("vec_mulluq 5:", (vui128_t) k, (vui128_t) e);

#if 1
  i = (vui32_t )CONST_VINT32_W(0, 0, 0x1, 0x2);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0x3, 0x5);
  k = (vui32_t) test_vec_mulluq((vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("0X100000002 * 0X300000005 ", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0, 0x00000003, 0x0000000b, 0x0000000a);
  rc += check_vuint128x ("vec_mulluq 6:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0xffffffff, 0xffffffff);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0xffffffff, 0xffffffff);
  k = (vui32_t) test_vec_mulluq((vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2**64-1 * 2**64-1 ", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0xffffffff, 0xfffffffe, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_mulluq 7:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 0xffffffff);
  k = (vui32_t) test_vec_mulluq((vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2**96-1 * 2**32-1 ", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0xfffffffe, 0xffffffff, 0xffffffff, 0x00000001);
  rc += check_vuint128x ("vec_mulluq 8:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0xffffffff);
  j = (vui32_t )CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  k = (vui32_t) test_vec_mulluq((vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2**32-1 * 2**96-1 ", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0xfffffffe, 0xffffffff, 0xffffffff, 0x00000001);
  rc += check_vuint128x ("vec_mulluq 9:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  j = (vui32_t )CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  k = (vui32_t) test_vec_mulluq((vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2**96-1 * 2**96-1 ", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0xfffffffe, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_mulluq 10:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 100000000);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 100000000);
  k = (vui32_t) test_vec_mulluq((vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128_sum ("10**8 * 10**8 ", k, i, j);
  print_vint128x_sum ("10**8 * 10**8 ", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0, 0, 0x002386f2, 0x6fc10000);
  rc += check_vuint128x ("vec_mulluq 11:", (vui128_t) k, (vui128_t) e);

  i = k;
  j = k;
  k = (vui32_t) test_vec_mulluq((vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128_sum ("10**16 * 10**16 ", k, i, j);
  print_vint128x_sum ("10**16 * 10**16 ", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0x000004ee, 0x2d6d415b, 0x85acef81, 0x00000000);
  rc += check_vuint128x ("vec_mulluq 12:", (vui128_t) k, (vui128_t) e);
#endif
  return (rc);
}

int
test_msumudm (void)
{
  vui64_t i, j;
  vui128_t k, l, e;
  int rc = 0;

  printf ("\ntest_msumudm Vector Multiply-Sum Unsigned Doubleword Modulo\n");

  i = (vui64_t){1, 2};
  j = (vui64_t){101, 102};
  k = (vui128_t)CONST_VINT128_DW128(0, 0x1000);
  e = (vui128_t)CONST_VINT128_DW128(0, 0x1131);

  l = vec_msumudm(i, j, k);

#ifdef __DEBUG_PRINT__
  print_vint128 ("msumudm({1, 2}, {101, 102}, 0x1000) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mmsumudm:", (vui128_t)l, (vui128_t) e);

  i = (vui64_t){1000000000, 1000000000};
  j = (vui64_t){1000000000, 1000000000};
  k = (vui128_t)CONST_VINT128_DW128(0, 1000000000000000000UL);
  e = (vui128_t)CONST_VINT128_DW128(0, 3000000000000000000UL);

  l = vec_msumudm(i, j, k);

#ifdef __DEBUG_PRINT__
  print_vint128 ("msumudm({10**9, 10**9}, {10**9, 10**9}, 10**18) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mmsumudm:", (vui128_t)l, (vui128_t) e);

  i = (vui64_t){0, 1000000000000000000UL};
  j = (vui64_t){0, 1000000000000000000UL};
  k = (vui128_t)CONST_VINT128_DW128(0, 0);
  e = (vui128_t)CONST_VINT128_DW128(0x00c097ce7bc90715UL, 0xb34b9f1000000000UL);

  l = vec_msumudm(i, j, k);

#ifdef __DEBUG_PRINT__
  print_vint128 ("msumudm({0, 10**18}, {0, 10**18}, 0) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mmsumudm:", (vui128_t)l, (vui128_t) e);

  i = (vui64_t){1000000000000000000UL, 0};
  j = (vui64_t){1000000000000000000UL, 0};
  k = (vui128_t)CONST_VINT128_DW128(0, 0);
  e = (vui128_t)CONST_VINT128_DW128(0x00c097ce7bc90715UL, 0xb34b9f1000000000UL);

  l = vec_msumudm(i, j, k);

#ifdef __DEBUG_PRINT__
  print_vint128 ("msumudm({10**18, 0}, {10**18, 0}, 0) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mmsumudm:", (vui128_t)l, (vui128_t) e);

  i = (vui64_t){1000000000000000000UL, 0UL};
  j = (vui64_t){1000000000000000000UL, 0UL};
  k = (vui128_t)CONST_VINT128_DW128(0x00c097ce7bc90715UL, 0xb34b9f1000000000UL);
  e = (vui128_t)CONST_VINT128_DW128(0x01812f9cf7920e2bUL, 0x66973e2000000000UL);

  l = vec_msumudm(i, j, k);

#ifdef __DEBUG_PRINT__
  print_vint128 ("msumudm({10**18, 0}, {10**18, 0}, 10**36) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mmsumudm:", (vui128_t)l, (vui128_t) e);

  i = (vui64_t){1000000000000000000UL, 1000000000000000000UL};
  j = (vui64_t){1000000000000000000UL, 1000000000000000000UL};
  k = (vui128_t)CONST_VINT128_DW128(0, 0);
  e = (vui128_t)CONST_VINT128_DW128(0x01812f9cf7920e2bUL, 0x66973e2000000000UL);

  l = vec_msumudm(i, j, k);

#ifdef __DEBUG_PRINT__
  print_vint128 ("msumudm({10**18, 10**18}, {10**18, 10**18}, 0) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mmsumudm:", (vui128_t)l, (vui128_t) e);

  i = (vui64_t){1000000000000000000UL, 1000000000000000000UL};
  j = (vui64_t){1000000000000000000UL, 1000000000000000000UL};
  k = (vui128_t)CONST_VINT128_DW128(0x00c097ce7bc90715UL, 0xb34b9f1000000000UL);
  e = (vui128_t)CONST_VINT128_DW128(0x0241c76b735b1541UL, 0x19e2dd3000000000UL);

  l = vec_msumudm(i, j, k);

#ifdef __DEBUG_PRINT__
  print_vint128 ("msumudm({10**18, 10**18}, {10**18, 10**18}, 10**36) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mmsumudm:", (vui128_t)l, (vui128_t) e);

  i = (vui64_t){1000000000000000000UL, 2000000000000000000UL};
  j = (vui64_t){1UL, 1UL};
  k = (vui128_t)CONST_VINT128_DW128(0x0UL, 0x0UL);
  e = (vui128_t)CONST_VINT128_DW128(0, 3000000000000000000UL);

  l = vec_msumudm(i, j, k);

#ifdef __DEBUG_PRINT__
  print_vint128 ("msumudm({10**18, 2*10**18}, {1, 1}, 0x1000) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mmsumudm:", (vui128_t)l, (vui128_t) e);

  i = (vui64_t){2000000000000000000UL, 1000000000000000000UL};
  j = (vui64_t){1UL, -1UL};
  k = (vui128_t)CONST_VINT128_DW128(0x0UL, 0x0UL);
  e = (vui128_t)CONST_VINT128_DW128(1000000000000000000UL, 1000000000000000000UL);

  l = vec_msumudm(i, j, k);

#ifdef __DEBUG_PRINT__
  print_vint128 ("msumudm({2*10**18, 2*10**18}, {1, -1}, 0) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mmsumudm:", (vui128_t)l, (vui128_t) e);

  return (rc);
}

#ifdef __DEBUG_PRINT__
#define test_vec_muludq(_l, _i, _j)	db_vec_muluq(_l, _i, _j)
#else
#define test_vec_muludq(_l, _i, _j)	vec_muludq(_l, _i, _j)
#endif

int
test_muludq (void)
{
  vui32_t i, j, k, l /*, m*/;
  vui32_t e, ec;
  int rc = 0;

  printf ("\ntest_muludq vector multiply quadword, 256-bit product\n");

  i = (vui32_t )CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  j = (vui32_t )CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  k = (vui32_t) test_vec_muludq((vui128_t* )&l, (vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128_prod ("2**96-1 * 2**96-1 ", k, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0xfffffffe, 0x00000000, 0x00000000, 0x00000001);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0xffffffff, 0xffffffff);
  rc += check_vint256 ("vec_muludq 1:", (vui128_t) l, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);
#if 1
  i = (vui32_t )CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j = (vui32_t )CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  k = (vui32_t) test_vec_muludq((vui128_t* )&l, (vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128_prod ("2**128-1 * 2**128-1 ", k, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ec = (vui32_t )CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  rc += check_vint256 ("vec_muludq 2:", (vui128_t) l, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 100000000);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 100000000);
  k = (vui32_t) test_vec_muludq((vui128_t* )&l, (vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128_prod ("10**8 * 10**8 ", k, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W (0x00000000, 0x00000000, 0x002386f2, 0x6fc10000);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint256 ("vec_muludq 3:", (vui128_t) l, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = k;
  j = k;
  k = (vui32_t) test_vec_muludq((vui128_t* )&l, (vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128_prod ("10**16 * 10**16 ", k, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W (0x000004ee, 0x2d6d415b, 0x85acef81, 0x00000000);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint256 ("vec_muludq 4:", (vui128_t) l, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = k;
  j = k;
  k = (vui32_t) test_vec_muludq((vui128_t* )&l, (vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128_prod ("10**32 * 10**32 ", k, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W (0x6e38ed64, 0xbf6a1f01, 0x00000000, 0x00000000);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00184f03, 0xe93ff9f4, 0xdaa797ed);
  rc += check_vint256 ("vec_muludq 5:", (vui128_t) l, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);
#endif
  return (rc);
}


//#define __DEBUG_PRINT__ 2
int
test_revbq (void)
{
  vui32_t i, e;
  vui128_t j;
  int rc = 0;

  printf ("\ntest_revbq Reverse Bytes in quadwords\n");

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  j = vec_revbq((vui128_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("revbq(0) ", j);
#endif
  rc += check_vuint128x ("vec_revbq 1:", j, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0xffffffff, 0, 0, 0);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 0xffffffff);
  j = vec_revbq((vui128_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("revbq(0xffffffff,0,0,0) ", j);
#endif
  rc += check_vuint128x ("vec_revbq 2:", j, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0xffffffff, 0, 0);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0xffffffff, 0);
  j = vec_revbq((vui128_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("revbq(0,0xffffffff,0,0) ", j);
#endif
  rc += check_vuint128x ("vec_revbq 3:", j, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0xffffffff);
  e = (vui32_t )CONST_VINT32_W(0xffffffff, 0, 0, 0);
  j = vec_revbq((vui128_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("revbq(0,0,0,0xffffffff) ", j);
#endif
  rc += check_vuint128x ("vec_revbq 4:", j, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0xffffffff, 0);
  e = (vui32_t )CONST_VINT32_W(0, 0xffffffff, 0, 0);
  j = vec_revbq((vui128_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("rev(0,0,0xffffffff,0) ", j);
#endif
  rc += check_vuint128x ("vec_revbq 5:", j, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0xffff, 0);
  e = (vui32_t )CONST_VINT32_W(0, 0xffff0000, 0, 0);
  j = vec_revbq((vui128_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("revbq(0,0,0xffff,0) ", j);
#endif
  rc += check_vuint128x ("vec_revbq 6:", j, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0xffffffff, 0, 0xffffffff);
  e = (vui32_t )CONST_VINT32_W(0xffffffff, 0, 0xffffffff, 0);
  j = vec_revbq((vui128_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("revbq(0,0xffffffff,0,0xffffffff) ", j);
#endif
  rc += check_vuint128x ("vec_revbq 7:", j, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0xffff, 0, 0xffffffff);
  e = (vui32_t )CONST_VINT32_W(0xffffffff, 0, 0xffff0000, 0);
  j = vec_revbq((vui128_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("revbq(0,0xffff,0,0xffffffff) ", j);
#endif
  rc += check_vuint128x ("vec_revbq 8:", j, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0xf, 0, 0xffffffff);
  e = (vui32_t )CONST_VINT32_W(0xffffffff, 0, 0x0f000000, 0);
  j = vec_revbq((vui128_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("rev(0,0xf,0,0xffffffff) ", j);
#endif
  rc += check_vuint128x ("vec_revbq:", j, (vui128_t) e);

  return (rc);
}

int
test_popcntq (void)
{
  vui128_t i, e;
  vui128_t j;
  int rc = 0;

  printf ("\ntest_popcntq Vector Pop Count quadword\n");

  i = CONST_VINT128_DW128(0, 0);
  e = CONST_VINT128_DW128(0, 0);
  j = vec_popcntq(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntq({0, 0) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntq:", (vui128_t)j, (vui128_t) e);

  i = CONST_VINT128_DW128(0, -1);
  e = CONST_VINT128_DW128(0, 64);
  j = vec_popcntq(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntq({0, -1) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntq:", (vui128_t)j, (vui128_t) e);

  i = CONST_VINT128_DW128(-1, 0);
  e = CONST_VINT128_DW128(0, 64);
  j = vec_popcntq(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntq({-1, 0) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntq:", (vui128_t)j, (vui128_t) e);

  i = CONST_VINT128_DW128(-1, -1);
  e = CONST_VINT128_DW128(0, 128);
  j = vec_popcntq(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntq({-1, -1) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntq:", (vui128_t)j, (vui128_t) e);

  i = CONST_VINT128_DW128(10000000000, 1000000000);
  e = CONST_VINT128_DW128(0, 24);
  j = vec_popcntq(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntq({10000000000, 1000000000) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntq:", (vui128_t)j, (vui128_t) e);

  i = CONST_VINT128_DW128(1000000000, 10000000000);
  e = CONST_VINT128_DW128(0, 24);
  j = vec_popcntq(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntq({1000000000, 10000000000) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntq:", (vui128_t)j, (vui128_t) e);

  return (rc);
}

#ifdef __DEBUG_PRINT__
#define test_vec_clzq(_l)	db_vec_clzq(_l)
#else
#define test_vec_clzq(_l)	vec_clzq(_l)
#endif
//#undef __DEBUG_PRINT__
int
test_clzq (void)
{
  vui32_t i, e;
  vui128_t j;
  int rc = 0;

  printf ("\ntest_clzq Vector Count Leading Zeros in quadwords\n");

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 128);
  j = test_vec_clzq((vui128_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0) ", j);
#endif
  rc += check_vuint128x ("vec_clzq:", j, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0xffffffff, 0, 0, 0);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  j = test_vec_clzq((vui128_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0xffffffff,0,0,0) ", j);
#endif
  rc += check_vuint128x ("vec_clzq:", j, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0xffffffff, 0, 0);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 32);
  j = test_vec_clzq((vui128_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0xffffffff,0,0) ", j);
#endif
  rc += check_vuint128x ("vec_clzq:", j, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0xffffffff);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 96);
  j = test_vec_clzq((vui128_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0,0,0xffffffff) ", j);
#endif
  rc += check_vuint128x ("vec_clzq:", j, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0xffffffff, 0);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 64);
  j = test_vec_clzq((vui128_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0,0xffffffff,0) ", j);
#endif
  rc += check_vuint128x ("vec_clzq:", j, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0xffff, 0);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 80);
  j = test_vec_clzq((vui128_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0,0xffff,0) ", j);
#endif
  rc += check_vuint128x ("vec_clzq:", j, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0xffffffff, 0, 0xffffffff);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 32);
  j = test_vec_clzq((vui128_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0xffffffff,0,0xffffffff) ", j);
#endif
  rc += check_vuint128x ("vec_clzq:", j, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0xffff, 0, 0xffffffff);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 48);
  j = test_vec_clzq((vui128_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0xffff,0,0xffffffff) ", j);
#endif
  rc += check_vuint128x ("vec_clzq:", j, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0xf, 0, 0xffffffff);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 60);
  j = test_vec_clzq((vui128_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0xf,0,0xffffffff) ", j);
#endif
  rc += check_vuint128x ("vec_clzq:", j, (vui128_t) e);

  return (rc);
}
//#undef __DEBUG_PRINT__

//#define __DEBUG_PRINT__ 1
int
test_vsrqi (void)
{
  vui32_t i, e;
  vui128_t k;
  int rc = 0;

  printf ("\n%s Vector shift right quadword immediate\n", __FUNCTION__);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 0);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 0   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 0 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_srqi (  0):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 8);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 8   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 8 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00ffffff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128x ("vec_srqi (  8):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 16);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 16   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 16 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x0000ffff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128x ("vec_srqi ( 16):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 24);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 24   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 24 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x000000ff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128x ("vec_srqi ( 24):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 32);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 32   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 32 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128x ("vec_srqi ( 32):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 40);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 40   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 40 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x00ffffff, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128x ("vec_srqi ( 40):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 48);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 48   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 48 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x0000ffff, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128x ("vec_srqi ( 48):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 56);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 56   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 56 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x000000ff, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128x ("vec_srqi ( 56):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 64);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 64   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 64 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x00000000, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128x ("vec_srqi ( 64):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 72);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 72   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 72 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x00000000, 0x00ffffff,
			   __UINT32_MAX__);
  rc += check_vuint128x ("vec_srqi ( 72):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 80);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 80   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 80 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x00000000, 0x0000ffff,
			   __UINT32_MAX__);
  rc += check_vuint128x ("vec_srqi ( 80):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 88);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 88   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 88 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x00000000, 0x000000ff,
			   __UINT32_MAX__);
  rc += check_vuint128x ("vec_srqi ( 88):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 96);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 96   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 96 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000,
			   __UINT32_MAX__);
  rc += check_vuint128x ("vec_srqi ( 96):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 104);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 104   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 104 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000,
			   0x00ffffff);
  rc += check_vuint128x ("vec_srqi (104):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 112);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 112   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 112 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000,
			   0x0000ffff);
  rc += check_vuint128x ("vec_srqi (112):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 120);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 120   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 120 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000,
			   0x000000ff);
  rc += check_vuint128x ("vec_srqi (120):", (vui128_t) k, (vui128_t) e);
#if 1
  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 128);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 128   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 128 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000,
			   0x00000000);
  rc += check_vuint128x ("vec_srqi (128):", (vui128_t) k, (vui128_t) e);
#endif

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 1);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 1   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 1 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x7fffffff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_srqi (  1):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 2);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 2   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 2 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x3fffffff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_srqi (  2):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 3);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 3   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 3 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x1fffffff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_srqi (  3):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 4);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 4   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 4 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x0fffffff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_srqi (  4):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 5);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 5   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 5 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x07ffffff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_srqi (  5):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 6);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 6   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 6 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x03ffffff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_srqi (  6):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 7);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 7   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 7 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x01ffffff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_srqi (  7):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 9);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 9   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 9 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x007fffff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_srqi (  9):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 12);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 12   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 12 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x000fffff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_srqi ( 12):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 15);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 15   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 15 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x0001ffff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_srqi ( 15):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 17);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 17   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 17 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00007fff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_srqi ( 17):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 20);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 20   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 20 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000fff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_srqi ( 20):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 23);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 23   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 23 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x000001ff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_srqi ( 23):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 123);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 123   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 123 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000,
			   0x0000001f);
  rc += check_vuint128 ("vec_srqi (123):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 127);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 127   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 127 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000,
			   0x00000001);
  rc += check_vuint128 ("vec_srqi (127):", (vui128_t) k, (vui128_t) e);
#if 1
  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_srqi ((vui128_t) i, 129);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 >> 129   ", (vui128_t) i);
  print_vint128x ("2E128-1 >> 129 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000,
			   0x00000000);
  rc += check_vuint128x ("vec_srqi (129):", (vui128_t) k, (vui128_t) e);
#endif

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_vslqi (void)
{
  vui32_t i, e;
  vui128_t k;
  int rc = 0;

  printf ("\n%s Vector shift left quadword immediate\n", __FUNCTION__);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 0);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 0   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 0 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_slqi (  0):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 8);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 8   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 8 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xffffff00);
  rc += check_vuint128x ("vec_slqi (  8):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 16);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 16   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 16 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xffff0000);
  rc += check_vuint128x ("vec_slqi ( 16):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 24);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 24   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 24 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xff000000);
  rc += check_vuint128x ("vec_slqi ( 24):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 32);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 32   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 32 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0x00000000);
  rc += check_vuint128x ("vec_slqi ( 32):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 40);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 40   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 40 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, 0xffffff00,
			   0x00000000);
  rc += check_vuint128x ("vec_slqi ( 40):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 48);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 48   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 48 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, 0xffff0000,
			   0x00000000);
  rc += check_vuint128x ("vec_slqi ( 48):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 56);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 56   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 56 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, 0xff000000,
			   0x00000000);
  rc += check_vuint128x ("vec_slqi ( 56):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 64);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 64   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 64 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, 0x00000000,
			   0x00000000);
  rc += check_vuint128x ("vec_slqi ( 64):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 72);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 72   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 72 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, 0xffffff00, 0x00000000,
			   0x00000000);
  rc += check_vuint128x ("vec_slqi ( 72):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 80);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 80   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 80 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, 0xffff0000, 0x00000000,
			   0x00000000);
  rc += check_vuint128x ("vec_slqi ( 80):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 88);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 88   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 88 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, 0xff000000, 0x00000000,
			   0x00000000);
  rc += check_vuint128x ("vec_slqi ( 88):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 96);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 96   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 96 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, 0x00000000, 0x00000000,
			   0x00000000);
  rc += check_vuint128x ("vec_slqi ( 96):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 104);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 104   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 104 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0xffffff00, 0x00000000, 0x00000000,
			   0x00000000);
  rc += check_vuint128x ("vec_slqi (104):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 112);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 112   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 112 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0xffff0000, 0x00000000, 0x00000000,
			   0x00000000);
  rc += check_vuint128x ("vec_slqi (112):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 120);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 120   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 120 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0xff000000, 0x00000000, 0x00000000,
			   0x00000000);
  rc += check_vuint128x ("vec_slqi (120):", (vui128_t) k, (vui128_t) e);
#if 1
  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 128);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 128   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 128 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000,
			   0x00000000);
  rc += check_vuint128x ("vec_slqi (128):", (vui128_t) k, (vui128_t) e);
#endif

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 1);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 1   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 1 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xfffffffe);
  rc += check_vuint128 ("vec_slqi (  1):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 2);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 2   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 2 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xfffffffc);
  rc += check_vuint128 ("vec_slqi (  2):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 3);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 3   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 3 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xfffffff8);
  rc += check_vuint128 ("vec_slqi (  3):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 4);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 4   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 4 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xfffffff0);
  rc += check_vuint128 ("vec_slqi (  4):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 5);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 5   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 5 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xffffffe0);
  rc += check_vuint128 ("vec_slqi (  5):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 6);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 6   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 6 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xffffffc0);
  rc += check_vuint128 ("vec_slqi (  6):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 7);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 7   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 7 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xffffff80);
  rc += check_vuint128 ("vec_slqi (  7):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 9);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 9   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 9 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xfffffe00);
  rc += check_vuint128 ("vec_slqi (  9):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 12);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 12   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 12 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xfffff000);
  rc += check_vuint128 ("vec_slqi ( 12):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 15);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 15   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 15 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xffff8000);
  rc += check_vuint128 ("vec_slqi ( 15):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 17);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 17   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 17 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xfffe0000);
  rc += check_vuint128 ("vec_slqi ( 17):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 20);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 20   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 20 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xfff00000);
  rc += check_vuint128 ("vec_slqi ( 20):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 23);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 23   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 23 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xff800000);
  rc += check_vuint128 ("vec_slqi ( 23):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 123);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 123   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 123 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0xf8000000, 0x00000000, 0x00000000,
			   0x00000000);
  rc += check_vuint128 ("vec_slqi (123):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 127);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 127   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 127 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x80000000, 0x00000000, 0x00000000,
			   0x00000000);
  rc += check_vuint128 ("vec_slqi (127):", (vui128_t) k, (vui128_t) e);
#if 1
  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_slqi ((vui128_t) i, 129);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1 << 129   ", (vui128_t) i);
  print_vint128x ("2E128-1 << 129 = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000,
			   0x00000000);
  rc += check_vuint128x ("vec_slqi (129):", (vui128_t) k, (vui128_t) e);
#endif

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_vsrq (void)
{
  vui32_t i, j, e;
  vui128_t k;
  int rc = 0;

  printf ("\n%s Vector shift right quadword\n", __FUNCTION__);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t)
	    CONST_VINT32_W(0, 0, 0, 0);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_srq (  0):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 8);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00ffffff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128x ("vec_srq (  8):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 16);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x0000ffff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128x ("vec_srq ( 16):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 24);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x000000ff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128x ("vec_srq ( 24):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 32);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128x ("vec_srq ( 32):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 40);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x00ffffff, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128x ("vec_srq ( 40):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 48);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x0000ffff, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128x ("vec_srq ( 48):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 56);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x000000ff, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128x ("vec_srq ( 56):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 64);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x00000000, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128x ("vec_srq ( 64):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 72);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x00000000, 0x00ffffff,
			   __UINT32_MAX__);
  rc += check_vuint128x ("vec_srq ( 72):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 80);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x00000000, 0x0000ffff,
			   __UINT32_MAX__);
  rc += check_vuint128x ("vec_srq ( 80):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 88);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x00000000, 0x000000ff,
			   __UINT32_MAX__);
  rc += check_vuint128x ("vec_srq ( 88):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 96);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000,
			   __UINT32_MAX__);
  rc += check_vuint128x ("vec_srq ( 96):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 104);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000,
			   0x00ffffff);
  rc += check_vuint128x ("vec_srq (104):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 112);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000,
			   0x0000ffff);
  rc += check_vuint128x ("vec_srq (112):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 120);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000,
			   0x000000ff);
  rc += check_vuint128x ("vec_srq (120):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 128);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128x ("vec_srq (128):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 1);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x7fffffff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_srq (  1):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 2);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x3fffffff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_srq (  2):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 3);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x1fffffff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_srq (  3):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 4);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x0fffffff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_srq (  4):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 5);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x07ffffff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_srq (  5):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 6);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x03ffffff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_srq (  6):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 7);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x01ffffff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_srq (  7):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 9);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x007fffff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_srq (  9):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 12);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x000fffff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_srq ( 12):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 15);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x0001ffff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_srq ( 15):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 17);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00007fff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_srq ( 17):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 20);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000fff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_srq ( 20):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 23);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x000001ff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_srq ( 23):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 123);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000,
			   0x0000001f);
  rc += check_vuint128 ("vec_srq (123):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 127);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000,
			   0x00000001);
  rc += check_vuint128 ("vec_srq (127):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 129);
  k = vec_srq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        >> ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x7fffffff, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128x ("vec_srq (129):", (vui128_t) k, (vui128_t) e);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_vslq (void)
{
  vui32_t i, j, e;
  vui128_t k;
  int rc = 0;

  printf ("\n%s Vector shift left quadword\n", __FUNCTION__);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 0);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_slq (  0):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 8);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xffffff00);
  rc += check_vuint128x ("vec_slq (  8):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 16);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xffff0000);
  rc += check_vuint128x ("vec_slq ( 16):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 24);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xff000000);
  rc += check_vuint128x ("vec_slq ( 24):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 32);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0x00000000);
  rc += check_vuint128x ("vec_slq ( 32):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 40);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, 0xffffff00,
			   0x00000000);
  rc += check_vuint128x ("vec_slq ( 40):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 48);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, 0xffff0000,
			   0x00000000);
  rc += check_vuint128x ("vec_slq ( 48):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 56);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, 0xff000000,
			   0x00000000);
  rc += check_vuint128x ("vec_slq ( 56):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 64);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, 0x00000000,
			   0x00000000);
  rc += check_vuint128x ("vec_slq ( 64):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 72);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, 0xffffff00, 0x00000000,
			   0x00000000);
  rc += check_vuint128x ("vec_slq ( 72):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 80);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, 0xffff0000, 0x00000000,
			   0x00000000);
  rc += check_vuint128x ("vec_slq ( 80):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 88);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, 0xff000000, 0x00000000,
			   0x00000000);
  rc += check_vuint128x ("vec_slq ( 88):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 96);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, 0x00000000, 0x00000000,
			   0x00000000);
  rc += check_vuint128x ("vec_slq ( 96):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 104);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0xffffff00, 0x00000000, 0x00000000,
			   0x00000000);
  rc += check_vuint128x ("vec_slq (104):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 112);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0xffff0000, 0x00000000, 0x00000000,
			   0x00000000);
  rc += check_vuint128x ("vec_slq (112):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 120);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0xff000000, 0x00000000, 0x00000000,
			   0x00000000);
  rc += check_vuint128x ("vec_slq (120):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 128);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128x ("vec_slq (128):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 1);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xfffffffe);
  rc += check_vuint128 ("vec_slq (  1):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 2);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xfffffffc);
  rc += check_vuint128 ("vec_slq (  2):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 3);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xfffffff8);
  rc += check_vuint128 ("vec_slq (  3):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 4);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xfffffff0);
  rc += check_vuint128 ("vec_slq (  4):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 5);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xffffffe0);
  rc += check_vuint128 ("vec_slq (  5):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 6);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xffffffc0);
  rc += check_vuint128 ("vec_slq (  6):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 7);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xffffff80);
  rc += check_vuint128 ("vec_slq (  7):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 9);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xfffffe00);
  rc += check_vuint128 ("vec_slq (  9):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 12);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xfffff000);
  rc += check_vuint128 ("vec_slq ( 12):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 15);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xffff8000);
  rc += check_vuint128 ("vec_slq ( 15):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 17);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xfffe0000);
  rc += check_vuint128 ("vec_slq ( 17):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 20);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xfff00000);
  rc += check_vuint128 ("vec_slq ( 20):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 23);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xff800000);
  rc += check_vuint128 ("vec_slq ( 23):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 123);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0xf8000000, 0x00000000, 0x00000000,
			   0x00000000);
  rc += check_vuint128 ("vec_slq (123):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 127);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x80000000, 0x00000000, 0x00000000,
			   0x00000000);
  rc += check_vuint128 ("vec_slq (127):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 129);
  k = vec_slq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x ("2E128-1    ", (vui128_t) i);
  print_vint128x ("        << ", (vui128_t) j);
  print_vint128x ("         = ", k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xfffffffe);
  rc += check_vuint128x ("vec_slq (129):", (vui128_t) k, (vui128_t) e);

  return (rc);
}

#define TIME_10_ITERATION 10

int
test_time_i128 (void)
{
  long i;
  uint64_t t_start, t_end, t_delta;
  double delta_sec;
  int rc = 0;

  printf ("\n%s mul10uq start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIME_10_ITERATION; i++)
    {
      rc += timed_mul10uq ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s mul10uq end", __FUNCTION__);
  printf ("\n%s mul10uq  tb delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

  printf ("\n%s mul10uq2x start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIME_10_ITERATION; i++)
    {
      rc += timed_mul10uq2x ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s mul10uq2x end", __FUNCTION__);
  printf ("\n%s mul10uq2x  tb delta = %lu, sec = %10.6g\n", __FUNCTION__,
	  t_delta, delta_sec);

  printf ("\n%s mulluq start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIME_10_ITERATION; i++)
    {
      rc += timed_mulluq ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s mulluq end", __FUNCTION__);
  printf ("\n%s mulluq  tb delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);
  return (rc);
}

int
test_vec_i128 (void)
{
  int rc = 0;

  printf ("\n%s\n", __FUNCTION__);

  rc += test_revbq ();
  rc += test_clzq ();
  rc += test_popcntq();

  rc += test_vsrq();
  rc += test_vslq();
  rc += test_vsrqi();
  rc += test_vslqi();

  rc += test_addq();

  rc += test_mul10uq ();
  rc += test_mul10ecuq ();
  rc += test_cmul100 ();

  rc += test_mulluq ();
  rc += test_muludq ();

  rc += test_msumudm ();

  rc += test_time_i128();

  return (rc);
}

