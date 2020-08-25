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

//#define __DEBUG_PRINT__
#include <pveclib/vec_common_ppc.h>
#include <pveclib/vec_int32_ppc.h>
#include <pveclib/vec_int64_ppc.h>
#include <pveclib/vec_int128_ppc.h>

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

  co = (vui32_t) { 0, 0, 0, 1 };

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

//#define __DEBUG_PRINT__ 1
vui128_t
db_vec_divudq_10e31 (vui128_t *qh, vui128_t vra, vui128_t vrb)
{
  /* ten31  = +100000000000000000000000000000000UQ  */
  const vui128_t ten31 = (vui128_t)
	{ (__int128) 1000000000000000UL
        * (__int128) 10000000000000000UL };
  /* Magic numbers for multiplicative inverse to divide by 10**31
   are 4804950418589725908363185682083061167, corrective add,
   and shift right 103 bits.  */
  const vui128_t mul_invs_ten31 = (vui128_t) CONST_VINT128_DW(
      0x039d66589687f9e9UL, 0x01d59f290ee19dafUL);
  const int shift_ten31 = 103;
  vui128_t result, r2, t, q, q1, q2, c;
#ifdef __DEBUG_PRINT__
  print_vint128x (" divudq 31 ", (vui128_t) vra);
  print_vint128x ("           ", (vui128_t) vrb);
#endif
  if (vec_cmpuq_all_lt (vra, ten31))
    {
      // Multiply high [vra||vrb] * mul_invs_ten31
      q = vec_mulhuq (vrb, mul_invs_ten31);
      q1 = vec_muludq (&t, vra, mul_invs_ten31);

#ifdef __DEBUG_PRINT__
      print_vint128x (" mulhuq    ", (vui128_t) q);
      print_vint128x (" muludq    ", (vui128_t) t);
      print_vint128x ("           ", (vui128_t) q1);
#endif
      c = vec_addcuq (q1, q);
      q = vec_adduqm (q1, q);
      q1 = vec_adduqm (t, c);
#ifdef __DEBUG_PRINT__
      print_vint128x (" q         ", (vui128_t) q1);
      print_vint128x ("           ", (vui128_t) q);
#endif
      // corrective add [q2||q1||q] = [q1||q] + [vra||vrb]
      c = vec_addcuq (vrb, q);
      q = vec_adduqm (vrb, q);
      // q2 is the carry-out from the corrective add
      q2 = vec_addecuq (q1, vra, c);
      q1 = vec_addeuqm (q1, vra, c);
#ifdef __DEBUG_PRINT__
      print_vint128x (" add       ", (vui128_t) q2);
      print_vint128x ("           ", (vui128_t) q1);
      print_vint128x ("           ", (vui128_t) q);
#endif
      // shift 384-bits (including the carry) right 107 bits
      // Using shift left double quadword shift by (128-103)-bits
      r2 = vec_sldqi (q2, q1, (128 - shift_ten31));
      result = vec_sldqi (q1, q, (128 - shift_ten31));
#ifdef __DEBUG_PRINT__
      print_vint128x (" divudq31= ", (vui128_t) r2);
      print_vint128x ("           ", (vui128_t) result);
#endif
    }
  else
    {
      // Dividend is too large for multiplicative inverse
      // Have to use long division
      vui128_t qh,rh;
      qh = vec_divuq_10e31 (vra);
      rh = vec_moduq_10e31 (vra, qh);
#ifdef __DEBUG_PRINT__
      print_vint128x (" div qh    ", (vui128_t) qh);
      print_vint128x (" mod rh    ", (vui128_t) rh);
#endif
      result = db_vec_divudq_10e31 (&r2, rh, vrb);
      r2 = qh;
#ifdef __DEBUG_PRINT__
      print_vint128x (" result  = ", (vui128_t) r2);
      print_vint128x ("           ", (vui128_t) result);
#endif
    }

  // return 256-bit quotient
  *qh = r2;
  return result;
}

vui128_t
db_vec_modudq_10e31 (vui128_t vra, vui128_t vrb, vui128_t *ql)
{
  /* ten31  = +100000000000000000000000000000000UQ  */
  const vui128_t ten31 = (vui128_t)
	  { (__int128) 1000000000000000UL
         * (__int128) 10000000000000000UL };
  vui128_t result;
#if 1
  const vui128_t minus_one = (vui128_t) { (__int128) -1L };
  const vui128_t zero = (vui128_t) { (__int128) 0UL };
  vui128_t t, th, c;
#ifdef __DEBUG_PRINT__
  print_vint128x (" modudq 31 ", (vui128_t) vra);
  print_vint128x ("           ", (vui128_t) vrb);
  print_vint128x ("           ", (vui128_t) *ql);
#endif

  if (vec_cmpuq_all_ne (vra, zero) || vec_cmpuq_all_ge (vrb, ten31))
    {
      t = vec_muludq (&th, *ql, ten31);
      c = vec_subcuq (vrb, t);
      t = vec_subuqm (vrb, t);
      th = vec_subeuqm (vra, th, c);
#ifdef __DEBUG_PRINT__
      print_vint128x (" vr - t  = ", (vui128_t) th);
      print_vint128x ("           ", (vui128_t) t);
#endif
      // The remainder should be less than the divisor
      if (vec_cmpuq_all_ne (th, zero) && vec_cmpuq_all_ge (t, ten31))
	{
	  // If not the estimated quotient is off by 1
	  *ql = vec_adduqm (*ql, minus_one);
	  // And the remainder is negative, so add the divisor
	  t = vec_adduqm (t, ten31);
#ifdef __DEBUG_PRINT__
      print_vint128x (" fixup 31  ", (vui128_t) *ql);
      print_vint128x ("           ", (vui128_t) t);
#endif
	}
      result = t;
    }
  else
    result = vrb;
#ifdef __DEBUG_PRINT__
  print_vint128x (" modudq31= ", (vui128_t) result);
#endif
#endif

  return result;
}

//#define __DEBUG_PRINT__ 1
vui128_t
db_example_longdiv_10e31 (vui128_t *q, vui128_t *d, long int _N)
{
  vui128_t dn, qh, ql, rh;
  long int i;

  // init step for the top digits
  dn = d[0];
  qh = vec_divuq_10e31 (dn);
  rh = vec_moduq_10e31 (dn, qh);
#ifdef __DEBUG_PRINT__
  print_vint128x ("    ldiv31 ", (vui128_t) dn);
  print_vint128x ("         q ", (vui128_t) qh);
  print_vint128x ("         r ", (vui128_t) rh);
#endif
  q[0] = qh;

  // now we know the remainder is less than the divisor.
  for (i=1; i<_N; i++)
    {
      dn = d[i];
      ql = db_vec_divudq_10e31 (&qh, rh, dn);
#ifdef __DEBUG_PRINT__
      print_vint128x ("       dh= ", (vui128_t) rh);
      print_vint128x ("       dn= ", (vui128_t) dn);
      print_vint128x ("       qh= ", (vui128_t) qh);
      print_vint128x ("       ql= ", (vui128_t) ql);
#endif
      rh = db_vec_modudq_10e31 (rh, dn, &ql);
#ifdef __DEBUG_PRINT__
      print_vint128x ("       rh= ", (vui128_t) rh);
#endif
      q[i] = ql;
    }

#ifdef __DEBUG_PRINT__
      print_vint128x (" ldiv31=rh ", (vui128_t) rh);
#endif
  return rh;
}
#undef __DEBUG_PRINT__

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

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  l = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addeq ((vui128_t*) &m, (vui128_t) i, (vui128_t) j,
                           (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_extend ("extent 2E32-1 + 2E32-1 + c=1", k, m, i, j, l);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000001, 0xffffffff);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint256 ("vec_addeq:", (vui128_t) m, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  k = (vui32_t) vec_addeq ((vui128_t*) &m, (vui128_t) i, (vui128_t) j,
                           (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_extend ("extend 1 + 2E128-1 + c=0", k, m, i, j, l);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vint256 ("vec_addeq:", (vui128_t) m, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  j = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addeq ((vui128_t*) &m, (vui128_t) i, (vui128_t) j,
                           (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_extend ("extend 0 + 2E128-1 + c=1", k, m, i, j, l);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vint256 ("vec_addeq:", (vui128_t) m, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 2);
  j = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addeq ((vui128_t*) &m, (vui128_t) i, (vui128_t) j,
                           (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_extend ("extend 2 + 2E128-1 + c=1", k, m, i, j, l);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000002);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vint256 ("vec_addeq:", (vui128_t) m, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 1, 0);
  j = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0);
  l = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addeq ((vui128_t*) &m, (vui128_t) i, (vui128_t) j,
                           (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_extend ("extend 2E32 + 2E128-1-2E32-1 + c=1", k, m, i, j, l);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vint256 ("vec_addeq:", (vui128_t) m, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  j = (vui32_t)
          CONST_VINT32_W(0xfffeffff, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addeq ((vui128_t*) &m, (vui128_t) i, (vui128_t) j,
                           (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_extend ("extend 0 + 2E128-1-2E102 + c=1", k, m, i, j, l);
#endif
  e = (vui32_t)CONST_VINT32_W(0xffff0000, 0x00000000, 0x00000000, 0x00000000);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint256 ("vec_addeq:", (vui128_t) m, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t)
          CONST_VINT32_W(0xfffeffff, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addeq ((vui128_t*) &m, (vui128_t) i, (vui128_t) j,
                           (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_extend ("extend 1 + 2E128-1-2E102 + c=1", k, m, i, j, l);
#endif
  e = (vui32_t)CONST_VINT32_W(0xffff0000, 0x00000000, 0x00000000, 0x00000001);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
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
  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  k = (vui32_t) vec_adduqm ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E32-1", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000001, 0x00000000);
  rc += check_vuint128x ("vec_adduqm:", (vui128_t) k, (vui128_t) e);

#ifdef __DEBUG_PRINT__
  a = __UINT32_MAX__;
  b = __UINT32_MAX__;
  c = a + b;

  print_int128_sum ("2E32-1 + 2E32-1", c, a, b);
#endif
  i = (vui32_t)CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  k = (vui32_t) vec_adduqm ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2E32-1 + 2E32-1", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000001, 0xfffffffe);
  rc += check_vuint128x ("vec_adduqm:", (vui128_t) k, (vui128_t) e);

#ifdef __DEBUG_PRINT__
  a = 1UL;
  b = __UINTMAX_MAX__;
  c = a + b;

  print_int128_sum ("1 + 2E64-1", c, a, b);
#endif
  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t)CONST_VINT32_W(0, 0, __UINT32_MAX__, __UINT32_MAX__);
  k = (vui32_t) vec_adduqm ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E64-1", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000001, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_adduqm:", (vui128_t) k, (vui128_t) e);

#ifdef __DEBUG_PRINT__
  a = 1UL;
  b = __UINTMAX_MAX__;
  b = b << 32;
  b = b + __UINT32_MAX__;
  c = a + b;

  print_int128_sum ("1 + 2E96-1", c, a, b);
#endif
  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t)
          CONST_VINT32_W(0, __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__);
  k = (vui32_t) vec_adduqm ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E96-1", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000001, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_adduqm:", (vui128_t) k, (vui128_t) e);

#ifdef __DEBUG_PRINT__
  a = 1UL;
  b = __UINTMAX_MAX__;
  b = b << 64;
  b = b + __UINTMAX_MAX__;
  c = a + b;

  print_int128_sum ("1 + 2E128-1", c, a, b);
#endif
  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  k = (vui32_t) vec_adduqm ((vui128_t) i, (vui128_t) j);
  l = (vui32_t) vec_addcuq ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E128-1", k, i, j);
  print_vint128x     ("  c = ", (vui128_t)l);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_adduqm:", (vui128_t) k, (vui128_t) e);
  rc += check_vuint128x ("vec_addcuq:", (vui128_t) l, (vui128_t) ec);

  i = (vui32_t)CONST_VINT32_W(0, 0, 1, 0);
  j = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0);
  k = (vui32_t) vec_adduqm ((vui128_t) i, (vui128_t) j);
  l = (vui32_t) vec_addcuq ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E128-1", k, i, j);
  print_vint128x     ("  c = ", (vui128_t)l);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_adduqm:", (vui128_t) k, (vui128_t) e);
  rc += check_vuint128x ("vec_addcuq:", (vui128_t) l, (vui128_t) ec);

  i = (vui32_t)CONST_VINT32_W(0, 1, 0, 0);
  j = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, 0,
                         0);
  k = (vui32_t) vec_adduqm ((vui128_t) i, (vui128_t) j);
  l = (vui32_t) vec_addcuq ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E128-1", k, i, j);
  print_vint128x     ("  c = ", (vui128_t)l);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_adduqm:", (vui128_t) k, (vui128_t) e);
  rc += check_vuint128x ("vec_addcuq:", (vui128_t) l, (vui128_t) ec);

  i = (vui32_t)CONST_VINT32_W(1, 0, 0, 0);
  j = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, 0, 0,
                         0);
  k = (vui32_t) vec_adduqm ((vui128_t) i, (vui128_t) j);
  l = (vui32_t) vec_addcuq ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E128-1", k, i, j);
  print_vint128x     ("  c = ", (vui128_t)l);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_adduqm:", (vui128_t) k, (vui128_t) e);
  rc += check_vuint128x ("vec_addcuq:", (vui128_t) l, (vui128_t) ec);

  i = (vui32_t)CONST_VINT32_W(1, 1, 1, 1);
  j = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, 0, 0,
                         0);
  k = (vui32_t) vec_adduqm ((vui128_t) i, (vui128_t) j);
  l = (vui32_t) vec_addcuq ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E128-1", k, i, j);
  print_vint128x     ("  c = ", (vui128_t)l);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000001, 0x00000001, 0x00000001);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_adduqm:", (vui128_t) k, (vui128_t) e);
  rc += check_vuint128x ("vec_addcuq:", (vui128_t) l, (vui128_t) ec);

  i = (vui32_t)CONST_VINT32_W(1, __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__);
  j = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, 0, 0,
                         0);
  k = (vui32_t) vec_adduqm ((vui128_t) i, (vui128_t) j);
  l = (vui32_t) vec_addcuq ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E128-1", k, i, j);
  print_vint128x     ("  c = ", (vui128_t)l);
#endif
  e = (vui32_t)CONST_VINT32_W(0, __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_adduqm:", (vui128_t) k, (vui128_t) e);
  rc += check_vuint128x ("vec_addcuq:", (vui128_t) l, (vui128_t) ec);

  i = (vui32_t)CONST_VINT32_W(0, __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__);
  j = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, 0, 0,
                         0);
  k = (vui32_t) vec_adduqm ((vui128_t) i, (vui128_t) j);
  l = (vui32_t) vec_addcuq ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E128-1", k, i, j);
  print_vint128x     ("  c = ", (vui128_t)l);
#endif
  e = (vui32_t)CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_adduqm:", (vui128_t) k, (vui128_t) e);
  rc += check_vuint128x ("vec_addcuq:", (vui128_t) l, (vui128_t) ec);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  l = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addeuqm ((vui128_t) i, (vui128_t) j, (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_carry ("1 + 2E32-1 + c=1", k, i, j, l);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000001, 0x00000001);
  rc += check_vuint128x ("vec_addeuqm:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  l = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addeuqm ((vui128_t) i, (vui128_t) j, (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_carry ("2E32-1 + 2E32-1 + c=1", k, i, j, l);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000001, 0xffffffff);
  rc += check_vuint128x ("vec_addeuqm:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  l = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addecuq ((vui128_t) i, (vui128_t) j, (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_carry ("carry 2E32-1 + 2E32-1 + c=1", k, i, j, l);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_addecuq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  k = (vui32_t) vec_addeuqm ((vui128_t) i, (vui128_t) j, (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_carry ("1 + 2E128-1 + c=0", k, i, j, l);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_addeuqm:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  k = (vui32_t) vec_addecuq ((vui128_t) i, (vui128_t) j, (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_carry ("carry 1 + 2E128-1 + c=0", k, i, j, l);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_addecuq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  j = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addeuqm ((vui128_t) i, (vui128_t) j, (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_carry ("0 + 2E128-1 + c=1", k, i, j, l);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_addeuqm:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  j = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addecuq ((vui128_t) i, (vui128_t) j, (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_carry ("carry 0 + 2E128-1 + c=1", k, i, j, l);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_addecuq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addeuqm ((vui128_t) i, (vui128_t) j, (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_carry ("1 + 2E128-1 + c=1", k, i, j, l);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_addeuqm:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 2);
  j = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addecuq ((vui128_t) i, (vui128_t) j, (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_carry ("carry 2 + 2E128-1 + c=1", k, i, j, l);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
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

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 10);
  k = vec_mul10uq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("1 * 10 ", k);
  print_vint128x("1 * 10 ", k);
#endif
  rc += check_vuint128 ("vec_mul10uq:", k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 65535);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 655350);
  k = vec_mul10uq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("65535 * 10 ", k);
  print_vint128x("65535 * 10 ", (vui128_t)e);
#endif
  rc += check_vuint128 ("vec_mul10uq:", k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 65536);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 0xa0000U);
  k = vec_mul10uq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("65536 * 10 ", k);
  print_vint128x("65536 * 10 ", (vui128_t)e);
#endif
  rc += check_vuint128 ("vec_mul10uq:", k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 65537);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 0xa000aU);
  k = vec_mul10uq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("65537 * 10 ", k);
  print_vint128x("65537 * 10 ", (vui128_t)e);
#endif
  rc += check_vuint128 ("vec_mul10uq:", k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0x00000009U, 0xfffffff6U);
  k = vec_mul10uq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("2E32-1 * 10 ", k);
  print_vint128x("2E32-1 * 10 ", (vui128_t)e);
#endif
  rc += check_vuint128 ("vec_mul10uq:", k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 65535, __UINT32_MAX__);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0x9ffffU, 0xfffffff6U);
  k = vec_mul10uq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("2E48-1 * 10 ", k);
  print_vint128x("2E48-1 * 10 ", (vui128_t)e);
#endif
  rc += check_vuint128 ("vec_mul10uq:", k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, __UINT32_MAX__, __UINT32_MAX__);
  e = (vui32_t)CONST_VINT32_W(0, 0x00000009U, __UINT32_MAX__, 0xfffffff6U);
  k = vec_mul10uq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("2E64-1 * 10 ", k);
  print_vint128x("2E64-1 * 10 ", (vui128_t)e);
#endif
  rc += check_vuint128 ("vec_mul10uq:", k, (vui128_t) e);

  i = (vui32_t)
          CONST_VINT32_W(0, __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__);
  e = (vui32_t)
          CONST_VINT32_W(0x00000009U, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffff6U);
  k = vec_mul10uq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("2E96-1 * 10 ", k);
  print_vint128x("2E96-1 * 10 ", (vui128_t)e);
#endif
  rc += check_vuint128 ("vec_mul10uq:", k, (vui128_t) e);

  i = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  e = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffff6U);
  k = vec_mul10uq ((vui128_t) i);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1 * 10 ", k);
  print_vint128x("2E128-1 * 10 ", (vui128_t)e);
#endif
  rc += check_vuint128 ("vec_mul10uq:", k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);

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
  i = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  e = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffff6);
  ec = (vui32_t)CONST_VINT32_W(0, 0, 0, 9);

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

  i = (vui32_t)CONST_VINT32_W(0x19999999, 0x99999999, 0x99999999, 0x99999999);
  e = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffffa);
  ec = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);

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

  i = (vui32_t)CONST_VINT32_W(0x19999999, 0x99999999, 0x99999999, 0x9999999a);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 4);
  ec = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);

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

  i = (vui32_t)
          CONST_VINT32_W(0x7fffffff, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  e = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffff6);
  ec = (vui32_t)CONST_VINT32_W(0, 0, 0, 4);

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

  i = (vui32_t)
          CONST_VINT32_W(0x7fffffff, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  j = (vui128_t) (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  e = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffff6);
  k = vec_mul10euq ((vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
#endif
  rc += check_vuint128 ("vec_mul10euq 4:", k, (vui128_t) e);

  i = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  j = (vui128_t) (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  e = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffff6);
  k = vec_mul10euq ((vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
#endif
  rc += check_vuint128 ("vec_mul10euq 5:", k, (vui128_t) e);
  i = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  j = (vui128_t) (vui32_t)CONST_VINT32_W(0, 0, 0, 4);
  e = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffffa);
  k = vec_mul10euq ((vui128_t) i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
#endif
  rc += check_vuint128 ("vec_mul10euq 6:", k, (vui128_t) e);

  i = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  j = (vui128_t) (vui32_t)CONST_VINT32_W(0, 0, 0, 9);
  e = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  k = vec_mul10euq ((vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
#endif
  rc += check_vuint128 ("vec_mul10euq 7:", k, (vui128_t) e);

  i = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  j = (vui128_t) (vui32_t) { 0, 0, 0, 0 };
  e = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffff6);
  ec = (vui32_t)CONST_VINT32_W(0, 0, 0, 9);
  k = vec_mul10euq ((vui128_t) i, j);
  l = vec_mul10ecuq ((vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
  print_vint128 ("2E128-1 *10c ", l);
  print_vint128x("2E128-1 *10c ", l);
#endif
  rc += check_vint256 ("vec_mul10ecuq 8:", l, k, (vui128_t) ec, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0x19999999, 0x99999999, 0x99999999, 0x99999999);
  j = (vui128_t) (vui32_t) { 0, 0, 0, 0 };
  e = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffffa);
  ec = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);

  k = vec_mul10euq ((vui128_t) i, j);
  l = vec_mul10ecuq ((vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
  print_vint128 ("2E128-1 *10c ", l);
  print_vint128x("2E128-1 *10c ", l);
#endif
  rc += check_vint256 ("vec_mul10ecuq 9:", l, k, (vui128_t) ec, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0x19999999, 0x99999999, 0x99999999, 0x99999999);
  j = (vui128_t) ((vui32_t)CONST_VINT32_W(0, 0, 0, 1));
  e = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffffb);
  ec = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);

  k = vec_mul10euq ((vui128_t) i, j);
  l = vec_mul10ecuq ((vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
  print_vint128 ("2E128-1 *10c ", l);
  print_vint128x("2E128-1 *10c ", l);
#endif
  rc += check_vint256 ("vec_mul10ecuq 10:", l, k, (vui128_t) ec, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0x19999999, 0x99999999, 0x99999999, 0x99999999);
  j = (vui128_t) ((vui32_t)CONST_VINT32_W(0, 0, 0, 6));
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  ec = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);

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
  i = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  k = vec_cmul100cuq (&j, (vui128_t) i);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1 * 100 ", k);
  print_vint128 ("2E128-1 *100c ", j);
#endif
  e = (vui32_t)CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffff9c);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000063);
  rc += check_vint256 ("vec_cmul100cuq 1:", (vui128_t) j, (vui128_t) k,
                       (vui128_t) ec, (vui128_t) e);

  i = (vui32_t)
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  j = (vui128_t) (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  k = vec_cmul100ecuq (&l, (vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*100+e ", k);
  print_vint128 ("2E128-1 *100c ", l);
#endif
  e = (vui32_t)CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffff9c);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000063);
  rc += check_vint256 ("vec_cmul100ecuq 2:", (vui128_t) l, (vui128_t) k,
                       (vui128_t) ec, (vui128_t) e);

  i = (vui32_t) { __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
          __UINT32_MAX__ };
  j = (vui128_t) (vui32_t)CONST_VINT32_W(0, 0, 0, 4);
  k = vec_cmul100ecuq (&l, (vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*100+e ", k);
  print_vint128 ("2E128-1 *100c ", l);
#endif
  e = (vui32_t)CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffa0);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000063);
  rc += check_vint256 ("vec_cmul100ecuq:", (vui128_t) l, (vui128_t) k,
                       (vui128_t) ec, (vui128_t) e);

  i = (vui32_t) { __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
          __UINT32_MAX__ };
  j = (vui128_t) (vui32_t)CONST_VINT32_W(0, 0, 0, 9);
  k = vec_cmul100ecuq (&l, (vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*100+e ", k);
  print_vint128 ("2E128-1 *100c ", l);
#endif
  e = (vui32_t)CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffa5);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000063);
  rc += check_vint256 ("vec_cmul100ecuq 3:", (vui128_t) l, (vui128_t) k,
                       (vui128_t) ec, (vui128_t) e);

  i = (vui32_t) { __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
          __UINT32_MAX__ };
  j = (vui128_t) (vui32_t)CONST_VINT32_W(0, 0, 0, 10);
  k = vec_cmul100ecuq (&l, (vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*100+e ", k);
  print_vint128 ("2E128-1 *100c ", l);
#endif
  e = (vui32_t)CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffa6);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000063);
  rc += check_vint256 ("vec_cmul100ecuq 4:", (vui128_t) l, (vui128_t) k,
                       (vui128_t) ec, (vui128_t) e);

  i = (vui32_t) { __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
          __UINT32_MAX__ };
  j = (vui128_t) (vui32_t)CONST_VINT32_W(0, 0, 0, 99);
  k = vec_cmul100ecuq (&l, (vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*100+e ", k);
  print_vint128 ("2E128-1 *100c ", l);
#endif
  e = (vui32_t)CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000063);
  rc += check_vint256 ("vec_cmul100ecuq 5:", (vui128_t) l, (vui128_t) k,
                       (vui128_t) ec, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 100);
  m = (vui128_t) (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  e = (vui32_t)CONST_VINT32_W(0x7775a5f1, 0x71951000, 0x00000000, 0x00000000);
  ec = (vui32_t)CONST_VINT32_W(0x161bcca7, 0x119915b5, 0x0764b4ab, 0xe8652979);

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

  n = (vui128_t) (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  e = (vui32_t)CONST_VINT32_W (0xd1640000, 0000000000, 0x00000000, 0x00000000);
  em = (vui32_t)CONST_VINT32_W(0x1db2332b, 0x93f1e94f, 0x637a5429, 0x0fe2a55c);
  ec = (vui32_t)CONST_VINT32_W(0x067f43fb, 0xe77a37f8, 0xb7309320, 0xc32b3cd3);

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

//#define __DEBUG_PRINT__ 1
int
test_mulhuq (void)
{
  vui128_t i, j, k /*, l, m*/;
  vui128_t e;
  int rc = 0;

  printf ("\ntest_mulhuq vector multiply high unsigned quad\n");

  i = (vui128_t) CONST_VINT128_DW (0, __UINT64_MAX__);
  j = (vui128_t) CONST_VINT128_DW (0, __UINT64_MAX__);
  k = vec_mulhuq (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x ("mulhuq( ", i);
  print_vint128x ("       ,", j);
  print_vint128x ("      )=", k);
#endif
  e = (vui128_t) CONST_VINT128_DW (0, 0);;
  rc += check_vuint128 ("vec_mulhuq 1:", k, e);

  i = (vui128_t) CONST_VINT128_DW (1, 0);
  j = (vui128_t) CONST_VINT128_DW (1, 0);
  k = vec_mulhuq (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x ("mulhuq( ", i);
  print_vint128x ("       ,", j);
  print_vint128x ("      )=", k);
#endif
  e = (vui128_t) CONST_VINT128_DW (0, 1);;
  rc += check_vuint128 ("vec_mulhuq 2:", k, e);

  i = (vui128_t) CONST_VINT128_DW (1, __UINT64_MAX__);
  j = (vui128_t) CONST_VINT128_DW (1, 0);
  k = vec_mulhuq (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x ("mulhuq( ", i);
  print_vint128x ("       ,", j);
  print_vint128x ("      )=", k);
#endif
  e = (vui128_t) CONST_VINT128_DW (0, 1);;
  rc += check_vuint128 ("vec_mulhuq 3:", k, e);

  i = (vui128_t) CONST_VINT128_DW (1, __UINT64_MAX__);
  j = (vui128_t) CONST_VINT128_DW (1, __UINT64_MAX__);
  k = vec_mulhuq (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x ("mulhuq( ", i);
  print_vint128x ("       ,", j);
  print_vint128x ("      )=", k);
#endif
  e = (vui128_t) CONST_VINT128_DW (0, 3);;
  rc += check_vuint128 ("vec_mulhuq 4:", k, e);

  i = (vui128_t) CONST_VINT128_DW (__UINT64_MAX__, __UINT64_MAX__);
  j = (vui128_t) CONST_VINT128_DW (0, 3402824);
  k = vec_mulhuq (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x ("mulhuq( ", i);
  print_vint128x ("       ,", j);
  print_vint128x ("      )=", k);
#endif
  e = (vui128_t) CONST_VINT128_DW (0, 3402823);;
  rc += check_vuint128 ("vec_mulhuq 5:", k, e);

  i = (vui128_t) CONST_VINT128_DW (__INT64_MAX__, __UINT64_MAX__);
  j = (vui128_t) CONST_VINT128_DW (0, 3402824);
  k = vec_mulhuq (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x ("mulhuq( ", i);
  print_vint128x ("       ,", j);
  print_vint128x ("      )=", k);
#endif
  e = (vui128_t) CONST_VINT128_DW (0, 1701411);;
  rc += check_vuint128 ("vec_mulhuq 6:", k, e);

  i = (vui128_t) CONST_VINT128_DW (__INT64_MAX__, __UINT64_MAX__);
  j = (vui128_t) CONST_VINT128_DW (0x9f623d5a8a732974UL, 0xcfbc31db4b0295e5UL);
  k = vec_mulhuq (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x ("mulhuq( ", i);
  print_vint128x ("       ,", j);
  print_vint128x ("      )=", k);
#endif
  e = (vui128_t) CONST_VINT128_DW (0x4fb11ead453994baUL, 0x67de18eda5814af1UL);;
  rc += check_vuint128 ("vec_mulhuq 7:", k, e);

  return (rc);
}
#undef __DEBUG_PRINT__

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

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) test_vec_mulluq((vui128_t)i, (vui128_t)j);

#ifdef __DEBUG_PRINT__
  print_vint128_sum ("1 * 1 ", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  rc += check_vuint128 ("vec_mulluq 1:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 100);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0, 100);
  k = (vui32_t) test_vec_mulluq((vui128_t)i, (vui128_t)j);

#ifdef __DEBUG_PRINT__
  print_vint128_sum ("100 * 100 ", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 10000);
  rc += check_vuint128 ("vec_mulluq 2:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 0x10000);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0, 0x10000);
  k = (vui32_t) test_vec_mulluq((vui128_t)i, (vui128_t)j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("65536 * 65536 ", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0, 0, 0x00000001, 0x00000000);
  rc += check_vuint128x ("vec_mulluq 3:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 0xffffffff);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0, 0xffffffff);
  k = (vui32_t) test_vec_mulluq((vui128_t)i, (vui128_t)j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2**32-1 * 2**32-1 ", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0, 0, 0xfffffffe, 0x00000001);
  rc += check_vuint128x ("vec_mulluq 4:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0xffff, 0xffffffff);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0xffff, 0xffffffff);
  k = (vui32_t) test_vec_mulluq((vui128_t)i, (vui128_t)j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2**48-1 * 2**48-1 ", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0, 0xffffffff, 0xfffe0000, 0x00000001);
  rc += check_vuint128x ("vec_mulluq 5:", (vui128_t) k, (vui128_t) e);

#if 1
  i = (vui32_t)CONST_VINT32_W(0, 0, 0x1, 0x2);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0x3, 0x5);
  k = (vui32_t) test_vec_mulluq((vui128_t)i, (vui128_t)j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("0X100000002 * 0X300000005 ", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0, 0x00000003, 0x0000000b, 0x0000000a);
  rc += check_vuint128x ("vec_mulluq 6:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0xffffffff, 0xffffffff);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0xffffffff, 0xffffffff);
  k = (vui32_t) test_vec_mulluq((vui128_t)i, (vui128_t)j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2**64-1 * 2**64-1 ", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0xffffffff, 0xfffffffe, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_mulluq 7:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0, 0xffffffff);
  k = (vui32_t) test_vec_mulluq((vui128_t)i, (vui128_t)j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2**96-1 * 2**32-1 ", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0xfffffffe, 0xffffffff, 0xffffffff, 0x00000001);
  rc += check_vuint128x ("vec_mulluq 8:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 0xffffffff);
  j = (vui32_t)CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  k = (vui32_t) test_vec_mulluq((vui128_t)i, (vui128_t)j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2**32-1 * 2**96-1 ", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0xfffffffe, 0xffffffff, 0xffffffff, 0x00000001);
  rc += check_vuint128x ("vec_mulluq 9:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  j = (vui32_t)CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  k = (vui32_t) test_vec_mulluq((vui128_t)i, (vui128_t)j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2**96-1 * 2**96-1 ", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0xfffffffe, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_mulluq 10:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 100000000);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0, 100000000);
  k = (vui32_t) test_vec_mulluq((vui128_t)i, (vui128_t)j);

#ifdef __DEBUG_PRINT__
  print_vint128_sum ("10**8 * 10**8 ", k, i, j);
  print_vint128x_sum ("10**8 * 10**8 ", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0, 0, 0x002386f2, 0x6fc10000);
  rc += check_vuint128x ("vec_mulluq 11:", (vui128_t) k, (vui128_t) e);

  i = k;
  j = k;
  k = (vui32_t) test_vec_mulluq((vui128_t)i, (vui128_t)j);

#ifdef __DEBUG_PRINT__
  print_vint128_sum ("10**16 * 10**16 ", k, i, j);
  print_vint128x_sum ("10**16 * 10**16 ", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0x000004ee, 0x2d6d415b, 0x85acef81, 0x00000000);
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

  i = (vui32_t)CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  j = (vui32_t)CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  k = (vui32_t) test_vec_muludq((vui128_t* )&l, (vui128_t)i, (vui128_t)j);

#ifdef __DEBUG_PRINT__
  print_vint128_prod ("2**96-1 * 2**96-1 ", k, i, j, l);
#endif
  e = (vui32_t)CONST_VINT32_W(0xfffffffe, 0x00000000, 0x00000000, 0x00000001);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0xffffffff, 0xffffffff);
  rc += check_vint256 ("vec_muludq 1:", (vui128_t) l, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);
#if 1
  i = (vui32_t)CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j = (vui32_t)CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  k = (vui32_t) test_vec_muludq((vui128_t* )&l, (vui128_t)i, (vui128_t)j);

#ifdef __DEBUG_PRINT__
  print_vint128_prod ("2**128-1 * 2**128-1 ", k, i, j, l);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ec = (vui32_t)CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  rc += check_vint256 ("vec_muludq 2:", (vui128_t) l, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 100000000);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0, 100000000);
  k = (vui32_t) test_vec_muludq((vui128_t* )&l, (vui128_t)i, (vui128_t)j);

#ifdef __DEBUG_PRINT__
  print_vint128_prod ("10**8 * 10**8 ", k, i, j, l);
#endif
  e = (vui32_t)CONST_VINT32_W (0x00000000, 0x00000000, 0x002386f2, 0x6fc10000);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint256 ("vec_muludq 3:", (vui128_t) l, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = k;
  j = k;
  k = (vui32_t) test_vec_muludq((vui128_t* )&l, (vui128_t)i, (vui128_t)j);

#ifdef __DEBUG_PRINT__
  print_vint128_prod ("10**16 * 10**16 ", k, i, j, l);
#endif
  e = (vui32_t)CONST_VINT32_W (0x000004ee, 0x2d6d415b, 0x85acef81, 0x00000000);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint256 ("vec_muludq 4:", (vui128_t) l, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = k;
  j = k;
  k = (vui32_t) test_vec_muludq((vui128_t* )&l, (vui128_t)i, (vui128_t)j);

#ifdef __DEBUG_PRINT__
  print_vint128_prod ("10**32 * 10**32 ", k, i, j, l);
#endif
  e = (vui32_t)CONST_VINT32_W (0x6e38ed64, 0xbf6a1f01, 0x00000000, 0x00000000);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00184f03, 0xe93ff9f4, 0xdaa797ed);
  rc += check_vint256 ("vec_muludq 5:", (vui128_t) l, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);
#endif
  return (rc);
}

#define test_vec_madduq(_l, _i, _j, _m)	vec_madduq(_l, _i, _j, _m)
#define test_vec_madd2uq(_l, _i, _j, _m, _n)	vec_madd2uq(_l, _i, _j, _m, _n)

int
test_madduq (void)
{
  vui32_t i, j, k, l, m, n;
  vui32_t e, ec;
  int rc = 0;

  printf ("\ntest_madduq Vector Multiply-Add Unsigned Quadword, 256-bit Sum\n");

  i = (vui32_t)CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  j = (vui32_t)CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  m = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  k = (vui32_t) test_vec_madduq((vui128_t* )&l, (vui128_t)i, (vui128_t)j, (vui128_t)m);

#ifdef __DEBUG_PRINT__
  print_vint128_prod ("2**96-1 * 2**96-1 ", k, i, j, l);
#endif
  e = (vui32_t)CONST_VINT32_W(0xfffffffe, 0x00000000, 0x00000000, 0x00000001);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0xffffffff, 0xffffffff);
  rc += check_vint256 ("vec_madduq 1:", (vui128_t) l, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  j = (vui32_t)CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  m = (vui32_t)CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  k = (vui32_t) test_vec_madduq((vui128_t* )&l, (vui128_t)i, (vui128_t)j, (vui128_t)m);

#ifdef __DEBUG_PRINT__
  print_vint128_prod ("2**96-1 * 2**96-1 ", k, i, j, l);
#endif
  e = (vui32_t)CONST_VINT32_W(0xffffffff, 0x00000000, 0x00000000, 0x00000000);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0xffffffff, 0xffffffff);
  rc += check_vint256 ("vec_madduq 2:", (vui128_t) l, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  j = (vui32_t)CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  m = (vui32_t)CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  k = (vui32_t) test_vec_madduq((vui128_t* )&l, (vui128_t)i, (vui128_t)j, (vui128_t)m);

#ifdef __DEBUG_PRINT__
  print_vint128_prod ("2**96-1 * 2**96-1 ", k, i, j, l);
#endif
  e = (vui32_t)CONST_VINT32_W(0xfffffffe, 0x00000000, 0x00000000, 0x00000000);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000001, 0x00000000, 0x00000000);
  rc += check_vint256 ("vec_madduq 3:", (vui128_t) l, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j = (vui32_t)CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  m = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  k = (vui32_t) test_vec_madduq((vui128_t* )&l, (vui128_t)i, (vui128_t)j, (vui128_t)m);

#ifdef __DEBUG_PRINT__
  print_vint128_prod ("2**128-1 * 2**128-1 ", k, i, j, l);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ec = (vui32_t)CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  rc += check_vint256 ("vec_madduq 4:", (vui128_t) l, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j = (vui32_t)CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  m = (vui32_t)CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  k = (vui32_t) test_vec_madduq((vui128_t* )&l, (vui128_t)i, (vui128_t)j, (vui128_t)m);

#ifdef __DEBUG_PRINT__
  print_vint128_prod ("2**128-1 * 2**128-1 ", k, i, j, l);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ec = (vui32_t)CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  rc += check_vint256 ("vec_madduq 5:", (vui128_t) l, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t)
	CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j = (vui32_t)
	CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  m = (vui32_t)
	CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  n = (vui32_t)
	CONST_VINT32_W(0, 0, 0, 0);
  k = (vui32_t) test_vec_madd2uq((vui128_t* )&l, (vui128_t )i, (vui128_t )j,
				 (vui128_t )m, (vui128_t )n);

#ifdef __DEBUG_PRINT__
  print_vint128_prod ("2**128-1 * 2**128-1 ", k, i, j, l);
#endif
  e = (vui32_t)
	CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ec = (vui32_t)
	CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  rc += check_vint256 ("vec_madd2uq 1:", (vui128_t) l, (vui128_t) k,
		       (vui128_t) ec, (vui128_t) e);
  i = (vui32_t)
	CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j = (vui32_t)
	CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  m = (vui32_t)
	CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  n = (vui32_t)
	CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  k = (vui32_t) test_vec_madd2uq((vui128_t* )&l, (vui128_t )i, (vui128_t )j,
				 (vui128_t )m, (vui128_t )n);

#ifdef __DEBUG_PRINT__
  print_vint128_prod ("2**128-1 * 2**128-1 ", k, i, j, l);
#endif
  e = (vui32_t)
	CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  ec = (vui32_t)
	CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  rc += check_vint256 ("vec_madd2uq 2:", (vui128_t) l, (vui128_t) k,
		       (vui128_t) ec, (vui128_t) e);

  i = (vui32_t)
	CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  j = (vui32_t)
	CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  m = (vui32_t)
	CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  n = (vui32_t)
	CONST_VINT32_W(0, 0, 0, 0);
  k = (vui32_t) test_vec_madd2uq((vui128_t* )&l, (vui128_t )i, (vui128_t )j,
				 (vui128_t )m, (vui128_t )n);

#ifdef __DEBUG_PRINT__
  print_vint128_prod ("2**96-1 * 2**96-1 ", k, i, j, l);
#endif
  e = (vui32_t)
	CONST_VINT32_W(0xffffffff, 0x00000000, 0x00000000, 0x00000000);
  ec = (vui32_t)
	CONST_VINT32_W(0x00000000, 0x00000000, 0xffffffff, 0xffffffff);
  rc += check_vint256 ("vec_madd2uq 3:", (vui128_t) l, (vui128_t) k,
		       (vui128_t) ec, (vui128_t) e);

  i = (vui32_t)
	CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  j = (vui32_t)
	CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  m = (vui32_t)
	CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  n = (vui32_t)
	CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  k = (vui32_t) test_vec_madd2uq((vui128_t* )&l, (vui128_t )i, (vui128_t )j,
				 (vui128_t )m, (vui128_t )n);

#ifdef __DEBUG_PRINT__
  print_vint128_prod ("2**96-1 * 2**96-1 ", k, i, j, l);
#endif
  e = (vui32_t)
	CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  ec = (vui32_t)
	CONST_VINT32_W(0x00000000, 0x00000000, 0xffffffff, 0xffffffff);
  rc += check_vint256 ("vec_madd2uq 4:", (vui128_t) l, (vui128_t) k,
		       (vui128_t) ec, (vui128_t) e);

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

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  j = vec_revbq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("revbq(0) ", j);
#endif
  rc += check_vuint128x ("vec_revbq 1:", j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0xffffffff, 0, 0, 0);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 0xffffffff);
  j = vec_revbq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("revbq(0xffffffff,0,0,0) ", j);
#endif
  rc += check_vuint128x ("vec_revbq 2:", j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0xffffffff, 0, 0);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0xffffffff, 0);
  j = vec_revbq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("revbq(0,0xffffffff,0,0) ", j);
#endif
  rc += check_vuint128x ("vec_revbq 3:", j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 0xffffffff);
  e = (vui32_t)CONST_VINT32_W(0xffffffff, 0, 0, 0);
  j = vec_revbq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("revbq(0,0,0,0xffffffff) ", j);
#endif
  rc += check_vuint128x ("vec_revbq 4:", j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0xffffffff, 0);
  e = (vui32_t)CONST_VINT32_W(0, 0xffffffff, 0, 0);
  j = vec_revbq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("rev(0,0,0xffffffff,0) ", j);
#endif
  rc += check_vuint128x ("vec_revbq 5:", j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0xffff, 0);
  e = (vui32_t)CONST_VINT32_W(0, 0xffff0000, 0, 0);
  j = vec_revbq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("revbq(0,0,0xffff,0) ", j);
#endif
  rc += check_vuint128x ("vec_revbq 6:", j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0xffffffff, 0, 0xffffffff);
  e = (vui32_t)CONST_VINT32_W(0xffffffff, 0, 0xffffffff, 0);
  j = vec_revbq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("revbq(0,0xffffffff,0,0xffffffff) ", j);
#endif
  rc += check_vuint128x ("vec_revbq 7:", j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0xffff, 0, 0xffffffff);
  e = (vui32_t)CONST_VINT32_W(0xffffffff, 0, 0xffff0000, 0);
  j = vec_revbq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("revbq(0,0xffff,0,0xffffffff) ", j);
#endif
  rc += check_vuint128x ("vec_revbq 8:", j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0xf, 0, 0xffffffff);
  e = (vui32_t)CONST_VINT32_W(0xffffffff, 0, 0x0f000000, 0);
  j = vec_revbq((vui128_t)i);

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

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 128);
  j = test_vec_clzq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0) ", j);
#endif
  rc += check_vuint128x ("vec_clzq:", j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0xffffffff, 0, 0, 0);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  j = test_vec_clzq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0xffffffff,0,0,0) ", j);
#endif
  rc += check_vuint128x ("vec_clzq:", j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0xffffffff, 0, 0);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 32);
  j = test_vec_clzq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0xffffffff,0,0) ", j);
#endif
  rc += check_vuint128x ("vec_clzq:", j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 0xffffffff);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 96);
  j = test_vec_clzq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0,0,0xffffffff) ", j);
#endif
  rc += check_vuint128x ("vec_clzq:", j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0xffffffff, 0);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 64);
  j = test_vec_clzq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0,0xffffffff,0) ", j);
#endif
  rc += check_vuint128x ("vec_clzq:", j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0xffff, 0);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 80);
  j = test_vec_clzq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0,0xffff,0) ", j);
#endif
  rc += check_vuint128x ("vec_clzq:", j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0xffffffff, 0, 0xffffffff);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 32);
  j = test_vec_clzq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0xffffffff,0,0xffffffff) ", j);
#endif
  rc += check_vuint128x ("vec_clzq:", j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0xffff, 0, 0xffffffff);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 48);
  j = test_vec_clzq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0xffff,0,0xffffffff) ", j);
#endif
  rc += check_vuint128x ("vec_clzq:", j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0xf, 0, 0xffffffff);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 60);
  j = test_vec_clzq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0xf,0,0xffffffff) ", j);
#endif
  rc += check_vuint128x ("vec_clzq:", j, (vui128_t) e);

  return (rc);
}

#define test_vec_ctzq(_l)	vec_ctzq(_l)
//#define __DEBUG_PRINT__ 1
int
test_ctzq (void)
{
  vui32_t i, e;
  vui128_t j;
  int rc = 0;

  printf ("\ntest_ctzq Vector Count Trailing Zeros in quadwords\n");

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 128);
  j = test_vec_ctzq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0) ", j);
#endif
  rc += check_vuint128x ("vec_ctzq:", j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0xffffffff, 0, 0, 0);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 96);
  j = test_vec_ctzq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0xffffffff,0,0,0) ", j);
#endif
  rc += check_vuint128x ("vec_ctzq:", j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0xffffffff, 0, 0);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 64);
  j = test_vec_ctzq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0xffffffff,0,0) ", j);
#endif
  rc += check_vuint128x ("vec_ctzq:", j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 0xffffffff);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  j = test_vec_ctzq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0,0,0xffffffff) ", j);
#endif
  rc += check_vuint128x ("vec_ctzq:", j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0xffffffff, 0);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 32);
  j = test_vec_ctzq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0,0xffffffff,0) ", j);
#endif
  rc += check_vuint128x ("vec_ctzq:", j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0xffff, 0);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 32);
  j = test_vec_ctzq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0,0xffff,0) ", j);
#endif
  rc += check_vuint128x ("vec_ctzq:", j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0xffff0000, 0);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 48);
  j = test_vec_ctzq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0,0xffff0000,0) ", j);
#endif
  rc += check_vuint128x ("vec_ctzq:", j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0xffffffff, 0, 0xffffffff);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  j = test_vec_ctzq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0xffffffff,0,0xffffffff) ", j);
#endif
  rc += check_vuint128x ("vec_ctzq:", j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0xffffffff,0xffffffff, 0);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 32);
  j = test_vec_ctzq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0xffffffff,0xffffffff,0) ", j);
#endif
  rc += check_vuint128x ("vec_ctzq:", j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0xffff, 0xffff0000, 0);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 48);
  j = test_vec_ctzq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0xffff,0xffff0000,0) ", j);
#endif
  rc += check_vuint128x ("vec_ctzq:", j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0xf, 0xf0000000, 0);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 60);
  j = test_vec_ctzq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0xf,0xf0000000,0) ", j);
#endif
  rc += check_vuint128x ("vec_ctzq:", j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0xf0000000, 0, 0, 0);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0,124);
  j = test_vec_ctzq((vui128_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0xf0000000,0,0,0) ", j);
#endif
  rc += check_vuint128x ("vec_ctzq:", j, (vui128_t) e);

  return (rc);
}
#undef __DEBUG_PRINT__

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

//#define __DEBUG_PRINT__ 1
int
test_vsraq (void)
{
  vui32_t i, j, e;
  vi128_t k;
  int rc = 0;

  printf ("\n%s Vector shift right algebraic quadword\n", __FUNCTION__);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t)
	    CONST_VINT32_W(0, 0, 0, 0);
  k = vec_sraq ((vi128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a ", (vui128_t) i);
  print_vint128x (" b ", (vui128_t) j);
  print_vint128x (" = ", (vui128_t) k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_sraq (  0):", (vui128_t) k, (vui128_t) e);

  j = (vui32_t)
	    CONST_VINT32_W(0, 0, 0, 127);
  k = vec_sraq ((vi128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a ", (vui128_t) i);
  print_vint128x (" b ", (vui128_t) j);
  print_vint128x (" = ", (vui128_t) k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_sraq (127):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(0x00010203, 0x04050607, 0x08090a0b,
			   0x0c0d0e0f);
  j = (vui32_t)
	    CONST_VINT32_W(0, 0, 0, 4);
  k = vec_sraq ((vi128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a ", (vui128_t) i);
  print_vint128x (" b ", (vui128_t) j);
  print_vint128x (" = ", (vui128_t) k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00001020, 0x30405060, 0x708090a0,
			   0xb0c0d0e0);
  rc += check_vuint128 ("vec_sraq (  4):", (vui128_t) k, (vui128_t) e);

  j = (vui32_t)
	    CONST_VINT32_W(0, 0, 0, 32);
  k = vec_sraq ((vi128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a ", (vui128_t) i);
  print_vint128x (" b ", (vui128_t) j);
  print_vint128x (" = ", (vui128_t) k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0, 0x000010203, 0x04050607, 0x08090a0b);
  rc += check_vuint128 ("vec_sraq ( 32):", (vui128_t) k, (vui128_t) e);

  j = (vui32_t)
	    CONST_VINT32_W(0, 0, 0, 60);
  k = vec_sraq ((vi128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a ", (vui128_t) i);
  print_vint128x (" b ", (vui128_t) j);
  print_vint128x (" = ", (vui128_t) k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0, 0, 0x0000102030, 0x40506070);
  rc += check_vuint128 ("vec_sraq ( 60):", (vui128_t) k, (vui128_t) e);

  j = (vui32_t)
	    CONST_VINT32_W(0, 0, 0, 68);
  k = vec_sraq ((vi128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a ", (vui128_t) i);
  print_vint128x (" b ", (vui128_t) j);
  print_vint128x (" = ", (vui128_t) k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0, 0, 0x00001020, 0x30405060);
  rc += check_vuint128 ("vec_sraq ( 68):", (vui128_t) k, (vui128_t) e);

  j = (vui32_t)
	    CONST_VINT32_W(0, 0, 0, 120);
  k = vec_sraq ((vi128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a ", (vui128_t) i);
  print_vint128x (" b ", (vui128_t) j);
  print_vint128x (" = ", (vui128_t) k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x0, 0x0, 0x0,
			   0x0);
  rc += check_vuint128 ("vec_sraq (120):", (vui128_t) k, (vui128_t) e);

  j = (vui32_t)
	    CONST_VINT32_W(0, 0, 0, 127);
  k = vec_sraq ((vi128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a ", (vui128_t) i);
  print_vint128x (" b ", (vui128_t) j);
  print_vint128x (" = ", (vui128_t) k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x0, 0x0, 0x0,
			   0x0);
  rc += check_vuint128 ("vec_sraq (127):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(0xf0010203, 0x04050607, 0x08090a0b,
			   0x0c0d0e0f);
  j = (vui32_t)
	    CONST_VINT32_W(0, 0, 0, 4);
  k = vec_sraq ((vi128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a ", (vui128_t) i);
  print_vint128x (" b ", (vui128_t) j);
  print_vint128x (" = ", (vui128_t) k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0xff001020, 0x30405060, 0x708090a0,
			   0xb0c0d0e0);
  rc += check_vuint128 ("vec_sraq (  4):", (vui128_t) k, (vui128_t) e);

  j = (vui32_t)
	    CONST_VINT32_W(0, 0, 0, 32);
  k = vec_sraq ((vi128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a ", (vui128_t) i);
  print_vint128x (" b ", (vui128_t) j);
  print_vint128x (" = ", (vui128_t) k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0xffffffff, 0xf0010203, 0x04050607,
			   0x08090a0b);
  rc += check_vuint128 ("vec_sraq ( 32):", (vui128_t) k, (vui128_t) e);

  j = (vui32_t)
	    CONST_VINT32_W(0, 0, 0, 60);
  k = vec_sraq ((vi128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a ", (vui128_t) i);
  print_vint128x (" b ", (vui128_t) j);
  print_vint128x (" = ", (vui128_t) k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0xffffffff, 0xffffffff, 0x00102030,
			   0x40506070);
  rc += check_vuint128 ("vec_sraq ( 60):", (vui128_t) k, (vui128_t) e);

  j = (vui32_t)
	    CONST_VINT32_W(0, 0, 0, 68);
  k = vec_sraq ((vi128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a ", (vui128_t) i);
  print_vint128x (" b ", (vui128_t) j);
  print_vint128x (" = ", (vui128_t) k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0xffffffff, 0xffffffff, 0xff001020,
			   0x30405060);
  rc += check_vuint128 ("vec_sraq ( 68):", (vui128_t) k, (vui128_t) e);

  j = (vui32_t)
	    CONST_VINT32_W(0, 0, 0, 120);
  k = vec_sraq ((vi128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a ", (vui128_t) i);
  print_vint128x (" b ", (vui128_t) j);
  print_vint128x (" = ", (vui128_t) k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff,
			   0xfffffff0);
  rc += check_vuint128 ("vec_sraq (120):", (vui128_t) k, (vui128_t) e);

  j = (vui32_t)
	    CONST_VINT32_W(0, 0, 0, 127);
  k = vec_sraq ((vi128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a ", (vui128_t) i);
  print_vint128x (" b ", (vui128_t) j);
  print_vint128x (" = ", (vui128_t) k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff,
			   0xffffffff);
  rc += check_vuint128 ("vec_sraq (127):", (vui128_t) k, (vui128_t) e);

  return (rc);
}
#undef __DEBUG_PRINT__

//#define __DEBUG_PRINT__ 1
int
test_vsraqi (void)
{
  vui32_t i, e;
  vi128_t k;
  int rc = 0;

  printf ("\n%s Vector shift right algebraic quadword  immediate\n", __FUNCTION__);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  k = vec_sraqi ((vi128_t) i, 0);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a,  0 ", (vui128_t) i);
  print_vint128x (" =     ", (vui128_t) k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_sraqi (  0):", (vui128_t) k, (vui128_t) e);

  k = vec_sraqi ((vi128_t) i, 127);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a,127 ", (vui128_t) i);
  print_vint128x (" =     ", (vui128_t) k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_sraqi (127):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(0x00010203, 0x04050607, 0x08090a0b,
			   0x0c0d0e0f);
  k = vec_sraqi ((vi128_t) i, 4);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a,  4 ", (vui128_t) i);
  print_vint128x (" =     ", (vui128_t) k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x00001020, 0x30405060, 0x708090a0,
			   0xb0c0d0e0);
  rc += check_vuint128 ("vec_sraqi (  4):", (vui128_t) k, (vui128_t) e);

  k = vec_sraqi ((vi128_t) i, 32);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a, 32 ", (vui128_t) i);
  print_vint128x (" =     ", (vui128_t) k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0, 0x000010203, 0x04050607, 0x08090a0b);
  rc += check_vuint128 ("vec_sraqi ( 32):", (vui128_t) k, (vui128_t) e);

  k = vec_sraqi ((vi128_t) i, 60);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a, 60 ", (vui128_t) i);
  print_vint128x (" =     ", (vui128_t) k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0, 0, 0x0000102030, 0x40506070);
  rc += check_vuint128 ("vec_sraqi ( 60):", (vui128_t) k, (vui128_t) e);

  k = vec_sraqi ((vi128_t) i, 68);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a, 68 ", (vui128_t) i);
  print_vint128x (" =     ", (vui128_t) k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0, 0, 0x00001020, 0x30405060);
  rc += check_vuint128 ("vec_sraqi ( 68):", (vui128_t) k, (vui128_t) e);

  k = vec_sraqi ((vi128_t) i, 120);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a,120 ", (vui128_t) i);
  print_vint128x (" =     ", (vui128_t) k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x0, 0x0, 0x0,
			   0x0);
  rc += check_vuint128 ("vec_sraqi (120):", (vui128_t) k, (vui128_t) e);

  k = vec_sraqi ((vi128_t) i, 127);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a,127 ", (vui128_t) i);
  print_vint128x (" =     ", (vui128_t) k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x0, 0x0, 0x0,
			   0x0);
  rc += check_vuint128 ("vec_sraqi (127):", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(0xf0010203, 0x04050607, 0x08090a0b,
			   0x0c0d0e0f);
  k = vec_sraqi ((vi128_t) i, 4);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a,  4 ", (vui128_t) i);
  print_vint128x (" =     ", (vui128_t) k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0xff001020, 0x30405060, 0x708090a0,
			   0xb0c0d0e0);
  rc += check_vuint128 ("vec_sraqi (  4):", (vui128_t) k, (vui128_t) e);

  k = vec_sraqi ((vi128_t) i, 32);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a, 32 ", (vui128_t) i);
  print_vint128x (" =     ", (vui128_t) k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0xffffffff, 0xf0010203, 0x04050607,
			   0x08090a0b);
  rc += check_vuint128 ("vec_sraqi ( 32):", (vui128_t) k, (vui128_t) e);

  k = vec_sraqi ((vi128_t) i, 60);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a, 60 ", (vui128_t) i);
  print_vint128x (" =     ", (vui128_t) k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0xffffffff, 0xffffffff, 0x00102030,
			   0x40506070);
  rc += check_vuint128 ("vec_sraqi ( 60):", (vui128_t) k, (vui128_t) e);

  k = vec_sraqi ((vi128_t) i, 68);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a, 68 ", (vui128_t) i);
  print_vint128x (" =     ", (vui128_t) k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0xffffffff, 0xffffffff, 0xff001020,
			   0x30405060);
  rc += check_vuint128 ("vec_sraqi ( 68):", (vui128_t) k, (vui128_t) e);

  k = vec_sraqi ((vi128_t) i, 120);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a,120 ", (vui128_t) i);
  print_vint128x (" =     ", (vui128_t) k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff,
			   0xfffffff0);
  rc += check_vuint128 ("vec_sraqi (120):", (vui128_t) k, (vui128_t) e);

  k = vec_sraqi ((vi128_t) i, 127);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a,127 ", (vui128_t) i);
  print_vint128x (" =     ", (vui128_t) k);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff,
			   0xffffffff);
  rc += check_vuint128 ("vec_sraqi (127):", (vui128_t) k, (vui128_t) e);

  return (rc);
}
#undef __DEBUG_PRINT__

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
test_vrlq (void)
{
  vui32_t i, j, e;
  vui128_t k;
  int rc = 0;

  printf ("\n%s Vector Rotate left quadword\n", __FUNCTION__);

  i = (vui32_t)
	CONST_VINT32_W(__UINT32_MAX__, 0, __UINT32_MAX__, 0);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 0);
  k = vec_rlq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a ", (vui128_t) i);
  print_vint128x (" b ", (vui128_t) j);
  print_vint128x (" = ", k);
#endif
  e = (vui32_t)
	CONST_VINT32_W(__UINT32_MAX__, 0, __UINT32_MAX__, 0);
  rc += check_vuint128 ("vec_rlq (  0):", (vui128_t) k, (vui128_t) e);

  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 1);
  k = vec_rlq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a ", (vui128_t) i);
  print_vint128x (" b ", (vui128_t) j);
  print_vint128x (" = ", k);
#endif
  e = (vui32_t)
	CONST_VINT32_W(__UINT32_MAX__-1, 1, __UINT32_MAX__-1, 1);
  rc += check_vuint128 ("vec_rlq (  1):", (vui128_t) k, (vui128_t) e);

  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 127);
  k = vec_rlq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a ", (vui128_t) i);
  print_vint128x (" b ", (vui128_t) j);
  print_vint128x (" = ", k);
#endif
  e = (vui32_t)
	CONST_VINT32_W(__INT32_MAX__, 0x80000000, __INT32_MAX__,
		       0x80000000);
  rc += check_vuint128 ("vec_rlq (127):", (vui128_t) k, (vui128_t) e);

  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 120);
  k = vec_rlq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a ", (vui128_t) i);
  print_vint128x (" b ", (vui128_t) j);
  print_vint128x (" = ", k);
#endif
  e = (vui32_t)
	CONST_VINT32_W(0x00ffffff, 0xff000000, 0x00ffffff,
		       0xff000000);
  rc += check_vuint128 ("vec_rlq (120):", (vui128_t) k, (vui128_t) e);

  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 8);
  k = vec_rlq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a ", (vui128_t) i);
  print_vint128x (" b ", (vui128_t) j);
  print_vint128x (" = ", k);
#endif
  e = (vui32_t)
	CONST_VINT32_W(0xffffff00, 0x000000ff, 0xffffff00,
		       0x000000ff);
  rc += check_vuint128 ("vec_rlq (  8):", (vui128_t) k, (vui128_t) e);

  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 4);
  k = vec_rlq ((vui128_t) i, (vui128_t) j);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a ", (vui128_t) i);
  print_vint128x (" b ", (vui128_t) j);
  print_vint128x (" = ", k);
#endif
  e = (vui32_t)
	CONST_VINT32_W(0xfffffff0, 0x0000000f, 0xfffffff0,
		       0x0000000f);
  rc += check_vuint128 ("vec_rlq (  4):", (vui128_t) k, (vui128_t) e);

  return (rc);
}
#undef __DEBUG_PRINT__

//#define __DEBUG_PRINT__ 1
int
test_vrlqi (void)
{
  vui32_t i, e;
  vui128_t k;
  int rc = 0;

  printf ("\n%s Vector rotate left quadword immediate\n", __FUNCTION__);

  i = (vui32_t)
	CONST_VINT32_W(__UINT32_MAX__, 0, __UINT32_MAX__, 0);
  k = vec_rlqi ((vui128_t) i, 0);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a ", (vui128_t) i);
  print_vint128x (" = ", k);
#endif
  e = (vui32_t)
	CONST_VINT32_W(__UINT32_MAX__, 0, __UINT32_MAX__, 0);
  rc += check_vuint128 ("vec_rlq (  0):", (vui128_t) k, (vui128_t) e);

  k = vec_rlqi ((vui128_t) i, 1);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a ", (vui128_t) i);
  print_vint128x (" = ", k);
#endif
  e = (vui32_t)
	CONST_VINT32_W(__UINT32_MAX__-1, 1, __UINT32_MAX__-1, 1);
  rc += check_vuint128 ("vec_rlq (  1):", (vui128_t) k, (vui128_t) e);

  k = vec_rlqi ((vui128_t) i, 127);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a ", (vui128_t) i);
  print_vint128x (" = ", k);
#endif
  e = (vui32_t)
	CONST_VINT32_W(__INT32_MAX__, 0x80000000, __INT32_MAX__,
		       0x80000000);
  rc += check_vuint128 ("vec_rlq (127):", (vui128_t) k, (vui128_t) e);

  k = vec_rlqi ((vui128_t) i, 120);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a ", (vui128_t) i);
  print_vint128x (" = ", k);
#endif
  e = (vui32_t)
	CONST_VINT32_W(0x00ffffff, 0xff000000, 0x00ffffff,
		       0xff000000);
  rc += check_vuint128 ("vec_rlq (120):", (vui128_t) k, (vui128_t) e);

  k = vec_rlqi ((vui128_t) i, 8);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a ", (vui128_t) i);
  print_vint128x (" = ", k);
#endif
  e = (vui32_t)
	CONST_VINT32_W(0xffffff00, 0x000000ff, 0xffffff00,
		       0x000000ff);
  rc += check_vuint128 ("vec_rlq (  8):", (vui128_t) k, (vui128_t) e);

  k = vec_rlqi ((vui128_t) i, 4);
#ifdef __DEBUG_PRINT__
  print_vint128x (" a ", (vui128_t) i);
  print_vint128x (" = ", k);
#endif
  e = (vui32_t)
	CONST_VINT32_W(0xfffffff0, 0x0000000f, 0xfffffff0,
		       0x0000000f);
  rc += check_vuint128 ("vec_rlq (  4):", (vui128_t) k, (vui128_t) e);

  return (rc);
}
#undef __DEBUG_PRINT__

//#define __DEBUG_PRINT__ 1
int
test_vsldq (void)
{
  vui32_t i, j, k, e;
  vui128_t l;
  int rc = 0;

  printf ("\n%s Vector shift left double quadword\n", __FUNCTION__);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 0);
  k = (vui32_t) CONST_VINT32_W(0, 0, 0, 0);
  l = vec_sldq ((vui128_t) i, (vui128_t) j, (vui128_t) k);
#ifdef __DEBUG_PRINT__
  print_vint128x (" w ", (vui128_t) i);
  print_vint128x (" x ", (vui128_t) j);
  print_vint128x (" s ", (vui128_t) k);
  print_vint128x (" = ", l);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_sldq (  0):", (vui128_t) l, (vui128_t) e);

  k = (vui32_t) CONST_VINT32_W(0, 0, 0, 1);
  l = vec_sldq ((vui128_t) i, (vui128_t) j, (vui128_t) k);
#ifdef __DEBUG_PRINT__
  print_vint128x (" w ", (vui128_t) i);
  print_vint128x (" x ", (vui128_t) j);
  print_vint128x (" s ", (vui128_t) k);
  print_vint128x (" = ", l);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xfffffffe);
  rc += check_vuint128 ("vec_sldq (  1):", (vui128_t) l, (vui128_t) e);

  k = (vui32_t) CONST_VINT32_W(0, 0, 0, 127);
  l = vec_sldq ((vui128_t) i, (vui128_t) j, (vui128_t) k);
#ifdef __DEBUG_PRINT__
  print_vint128x (" w ", (vui128_t) i);
  print_vint128x (" x ", (vui128_t) j);
  print_vint128x (" s ", (vui128_t) k);
  print_vint128x (" = ", l);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x80000000, 0, 0, 0);
  rc += check_vuint128 ("vec_sldq (127):", (vui128_t) l, (vui128_t) e);

  i = (vui32_t) CONST_VINT32_W(0, 0, 0, 0);
  j = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);

  k = (vui32_t) CONST_VINT32_W(0, 0, 0, 1);
  l = vec_sldq ((vui128_t) i, (vui128_t) j, (vui128_t) k);
#ifdef __DEBUG_PRINT__
  print_vint128x (" w ", (vui128_t) i);
  print_vint128x (" x ", (vui128_t) j);
  print_vint128x (" s ", (vui128_t) k);
  print_vint128x (" = ", l);
#endif
  e = (vui32_t) CONST_VINT32_W (0, 0, 0, 1);
  rc += check_vuint128 ("vec_sldq (  1):", (vui128_t) l, (vui128_t) e);

  k = (vui32_t) CONST_VINT32_W(0, 0, 0, 127);
  l = vec_sldq ((vui128_t) i, (vui128_t) j, (vui128_t) k);
#ifdef __DEBUG_PRINT__
  print_vint128x (" w ", (vui128_t) i);
  print_vint128x (" x ", (vui128_t) j);
  print_vint128x (" s ", (vui128_t) k);
  print_vint128x (" = ", l);
#endif
  e = (vui32_t)
	CONST_VINT32_W( 0x7fffffff, __UINT32_MAX__, __UINT32_MAX__,
	    __UINT32_MAX__);
  rc += check_vuint128 ("vec_sldq (127):", (vui128_t) l, (vui128_t) e);

  k = (vui32_t) CONST_VINT32_W(0, 0, 0, 8);
  l = vec_sldq ((vui128_t) i, (vui128_t) j, (vui128_t) k);
#ifdef __DEBUG_PRINT__
  print_vint128x (" w ", (vui128_t) i);
  print_vint128x (" x ", (vui128_t) j);
  print_vint128x (" s ", (vui128_t) k);
  print_vint128x (" = ", l);
#endif
  e = (vui32_t) CONST_VINT32_W (0, 0, 0, 0xff);
  rc += check_vuint128 ("vec_sldq (  8):", (vui128_t) l, (vui128_t) e);

  k = (vui32_t) CONST_VINT32_W(0, 0, 0, 120);
  l = vec_sldq ((vui128_t) i, (vui128_t) j, (vui128_t) k);
#ifdef __DEBUG_PRINT__
  print_vint128x (" w ", (vui128_t) i);
  print_vint128x (" x ", (vui128_t) j);
  print_vint128x (" s ", (vui128_t) k);
  print_vint128x (" = ", l);
#endif
  e = (vui32_t)
	CONST_VINT32_W( 0x00ffffff, __UINT32_MAX__, __UINT32_MAX__,
	    __UINT32_MAX__);
  rc += check_vuint128 ("vec_sldq (120):", (vui128_t) l, (vui128_t) e);

  return (rc);
}
#undef __DEBUG_PRINT__

//#define __DEBUG_PRINT__ 1
int
test_vsldqi (void)
{
  vui32_t i, j, e;
  vui128_t l;
  int rc = 0;

  printf ("\n%s Vector shift left double quadword\n", __FUNCTION__);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 0);
  l = vec_sldqi ((vui128_t) i, (vui128_t) j, 0);
#ifdef __DEBUG_PRINT__
  print_vint128x (" w ", (vui128_t) i);
  print_vint128x (" x ", (vui128_t) j);
  print_vint128x (" = ", l);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128 ("vec_sldqi (  0):", (vui128_t) l, (vui128_t) e);

  l = vec_sldqi ((vui128_t) i, (vui128_t) j, 1);
#ifdef __DEBUG_PRINT__
  print_vint128x (" w ", (vui128_t) i);
  print_vint128x (" x ", (vui128_t) j);
  print_vint128x (" = ", l);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xfffffffe);
  rc += check_vuint128 ("vec_sldq (  1):", (vui128_t) l, (vui128_t) e);

  l = vec_sldqi ((vui128_t) i, (vui128_t) j, 127);
#ifdef __DEBUG_PRINT__
  print_vint128x (" w ", (vui128_t) i);
  print_vint128x (" x ", (vui128_t) j);
  print_vint128x (" = ", l);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x80000000, 0, 0, 0);
  rc += check_vuint128 ("vec_sldq (127):", (vui128_t) l, (vui128_t) e);

  i = (vui32_t) CONST_VINT32_W(0, 0, 0, 0);
  j = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);

  l = vec_sldqi ((vui128_t) i, (vui128_t) j, 1);
#ifdef __DEBUG_PRINT__
  print_vint128x (" w ", (vui128_t) i);
  print_vint128x (" x ", (vui128_t) j);
  print_vint128x (" = ", l);
#endif
  e = (vui32_t) CONST_VINT32_W (0, 0, 0, 1);
  rc += check_vuint128 ("vec_sldqi (  1):", (vui128_t) l, (vui128_t) e);

  l = vec_sldqi ((vui128_t) i, (vui128_t) j, 127);
#ifdef __DEBUG_PRINT__
  print_vint128x (" w ", (vui128_t) i);
  print_vint128x (" x ", (vui128_t) j);
  print_vint128x (" = ", l);
#endif
  e = (vui32_t)
	CONST_VINT32_W( 0x7fffffff, __UINT32_MAX__, __UINT32_MAX__,
	    __UINT32_MAX__);
  rc += check_vuint128 ("vec_sldqi (127):", (vui128_t) l, (vui128_t) e);

  l = vec_sldqi ((vui128_t) i, (vui128_t) j, 8);
#ifdef __DEBUG_PRINT__
  print_vint128x (" w ", (vui128_t) i);
  print_vint128x (" x ", (vui128_t) j);
  print_vint128x (" = ", l);
#endif
  e = (vui32_t) CONST_VINT32_W (0, 0, 0, 0xff);
  rc += check_vuint128 ("vec_sldqi (  8):", (vui128_t) l, (vui128_t) e);

  l = vec_sldqi ((vui128_t) i, (vui128_t) j, 120);
#ifdef __DEBUG_PRINT__
  print_vint128x (" w ", (vui128_t) i);
  print_vint128x (" x ", (vui128_t) j);
  print_vint128x (" = ", l);
#endif
  e = (vui32_t)
	CONST_VINT32_W( 0x00ffffff, __UINT32_MAX__, __UINT32_MAX__,
	    __UINT32_MAX__);
  rc += check_vuint128 ("vec_sldqi (120):", (vui128_t) l, (vui128_t) e);

  return (rc);
}
#undef __DEBUG_PRINT__

//#define __DEBUG_PRINT__ 1
int
test_maxuq (void)
{
  vui128_t i1, i2, j, e;
  int rc = 0;

  printf ("\ntest_maxuq Vector Maximum Unsigned Quadword\n");

  i1 = (vui128_t) CONST_VINT128_DW ( 0, 0 );
  i2 = (vui128_t) CONST_VINT128_DW ( 0, 0 );
  e =  (vui128_t) CONST_VINT128_DW ( 0, 0 );
  j = vec_maxuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vmaxuq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_maxuq:", (vui128_t)j, (vui128_t) e);

  i1 = (vui128_t) CONST_VINT128_DW ( 0, 1 );
  i2 = (vui128_t) CONST_VINT128_DW ( 0, 2 );
  e =  (vui128_t) CONST_VINT128_DW ( 0, 2 );
  j = vec_maxuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vmaxuq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_maxuq:", (vui128_t)j, (vui128_t) e);

  i1 = (vui128_t) CONST_VINT128_DW ( 0, 2 );
  i2 = (vui128_t) CONST_VINT128_DW ( 0, 1 );
  e =  (vui128_t) CONST_VINT128_DW ( 0, 2 );
  j = vec_maxuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vmaxuq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_maxuq:", (vui128_t)j, (vui128_t) e);

  i1 = (vui128_t) CONST_VINT128_DW ( 1, 0 );
  i2 = (vui128_t) CONST_VINT128_DW ( 2, 0 );
  e =  (vui128_t) CONST_VINT128_DW ( 2, 0 );
  j = vec_maxuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vmaxuq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_maxuq:", (vui128_t)j, (vui128_t) e);

  i1 = (vui128_t) CONST_VINT128_DW ( 2, 0 );
  i2 = (vui128_t) CONST_VINT128_DW ( 1, 0 );
  e =  (vui128_t) CONST_VINT128_DW ( 2, 0 );
  j = vec_maxuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vmaxuq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_maxuq:", (vui128_t)j, (vui128_t) e);

  i1 = (vui128_t) CONST_VINT128_DW ( __UINT64_MAX__, __INT64_MAX__ );
  i2 = (vui128_t) CONST_VINT128_DW ( __UINT64_MAX__, __UINT64_MAX__ );
  e =  (vui128_t) CONST_VINT128_DW ( __UINT64_MAX__, __UINT64_MAX__ );
  j = vec_maxuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vmaxuq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_maxuq:", (vui128_t)j, (vui128_t) e);

  i1 = (vui128_t) CONST_VINT128_DW ( __UINT64_MAX__, __UINT64_MAX__ );
  i2 = (vui128_t) CONST_VINT128_DW ( __UINT64_MAX__, __INT64_MAX__ );
  e =  (vui128_t) CONST_VINT128_DW ( __UINT64_MAX__, __UINT64_MAX__ );
  j = vec_maxuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vmaxuq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_maxuq:", (vui128_t)j, (vui128_t) e);

  return (rc);
}
#undef __DEBUG_PRINT__

//#define __DEBUG_PRINT__ 1
int
test_maxsq (void)
{
  vi128_t i1, i2, j, e;
  int rc = 0;

  printf ("\ntest_maxsq Vector Maximum Signed Quadword\n");

  i1 = (vi128_t) CONST_VINT128_DW ( 0, 0 );
  i2 = (vi128_t) CONST_VINT128_DW ( 0, 0 );
  e =  (vi128_t) CONST_VINT128_DW ( 0, 0 );
  j = vec_maxsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vmaxsq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_maxsq:", (vui128_t)j, (vui128_t) e);

  i1 = (vi128_t) CONST_VINT128_DW ( 0, 1 );
  i2 = (vi128_t) CONST_VINT128_DW ( 0, 2 );
  e =  (vi128_t) CONST_VINT128_DW ( 0, 2 );
  j = vec_maxsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vmaxsq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_maxsq:", (vui128_t)j, (vui128_t) e);

  i1 = (vi128_t) CONST_VINT128_DW ( 0, 2 );
  i2 = (vi128_t) CONST_VINT128_DW ( 0, 1 );
  e =  (vi128_t) CONST_VINT128_DW ( 0, 2 );
  j = vec_maxsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vmaxsq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_maxsq:", (vui128_t)j, (vui128_t) e);

  i1 = (vi128_t) CONST_VINT128_DW ( 1, 0 );
  i2 = (vi128_t) CONST_VINT128_DW ( 2, 0 );
  e =  (vi128_t) CONST_VINT128_DW ( 2, 0 );
  j = vec_maxsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vmaxsq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_maxsq:", (vui128_t)j, (vui128_t) e);

  i1 = (vi128_t) CONST_VINT128_DW ( 2, 0 );
  i2 = (vi128_t) CONST_VINT128_DW ( 1, 0 );
  e =  (vi128_t) CONST_VINT128_DW ( 2, 0 );
  j = vec_maxsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vmaxsq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_maxsq:", (vui128_t)j, (vui128_t) e);

  i1 = (vi128_t) CONST_VINT128_DW ( __UINT64_MAX__, __INT64_MAX__ );
  i2 = (vi128_t) CONST_VINT128_DW ( __UINT64_MAX__, __UINT64_MAX__ );
  e =  (vi128_t) CONST_VINT128_DW ( __UINT64_MAX__, __UINT64_MAX__ );
  j = vec_maxsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vmaxsq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_maxsq:", (vui128_t)j, (vui128_t) e);

  i1 = (vi128_t) CONST_VINT128_DW ( __UINT64_MAX__, __UINT64_MAX__ );
  i2 = (vi128_t) CONST_VINT128_DW ( __UINT64_MAX__, __INT64_MAX__ );
  e =  (vi128_t) CONST_VINT128_DW ( __UINT64_MAX__, __UINT64_MAX__ );
  j = vec_maxsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vmaxsq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_maxsq:", (vui128_t)j, (vui128_t) e);

  i1 = (vi128_t) CONST_VINT128_DW ( __INT64_MAX__, __INT64_MAX__ );
  i2 = (vi128_t) CONST_VINT128_DW ( __UINT64_MAX__, __UINT64_MAX__ );
  e =  (vi128_t) CONST_VINT128_DW ( __INT64_MAX__, __INT64_MAX__ );
  j = vec_maxsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vmaxsq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_maxsq:", (vui128_t)j, (vui128_t) e);

  i1 = (vi128_t) CONST_VINT128_DW ( __UINT64_MAX__, __UINT64_MAX__ );
  i2 = (vi128_t) CONST_VINT128_DW ( __INT64_MAX__, __INT64_MAX__ );
  e =  (vi128_t) CONST_VINT128_DW ( __INT64_MAX__, __INT64_MAX__ );
  j = vec_maxsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vmaxsq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_maxsq:", (vui128_t)j, (vui128_t) e);

  return (rc);
}
#undef __DEBUG_PRINT__

//#define __DEBUG_PRINT__ 1
int
test_minuq (void)
{
  vui128_t i1, i2, j, e;
  int rc = 0;

  printf ("\ntest_minuq Vector Minimum Unsigned Quadword\n");

  i1 = (vui128_t) CONST_VINT128_DW ( 0, 0 );
  i2 = (vui128_t) CONST_VINT128_DW ( 0, 0 );
  e =  (vui128_t) CONST_VINT128_DW ( 0, 0 );
  j = vec_minuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vminuq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_minuq:", (vui128_t)j, (vui128_t) e);

  i1 = (vui128_t) CONST_VINT128_DW ( 0, 1 );
  i2 = (vui128_t) CONST_VINT128_DW ( 0, 2 );
  e =  (vui128_t) CONST_VINT128_DW ( 0, 1 );
  j = vec_minuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vminuq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_minuq:", (vui128_t)j, (vui128_t) e);

  i1 = (vui128_t) CONST_VINT128_DW ( 0, 2 );
  i2 = (vui128_t) CONST_VINT128_DW ( 0, 1 );
  e =  (vui128_t) CONST_VINT128_DW ( 0, 1 );
  j = vec_minuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vminuq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_minuq:", (vui128_t)j, (vui128_t) e);

  i1 = (vui128_t) CONST_VINT128_DW ( 1, 0 );
  i2 = (vui128_t) CONST_VINT128_DW ( 2, 0 );
  e =  (vui128_t) CONST_VINT128_DW ( 1, 0 );
  j = vec_minuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vminuq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_minuq:", (vui128_t)j, (vui128_t) e);

  i1 = (vui128_t) CONST_VINT128_DW ( 2, 0 );
  i2 = (vui128_t) CONST_VINT128_DW ( 1, 0 );
  e =  (vui128_t) CONST_VINT128_DW ( 1, 0 );
  j = vec_minuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vminuq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_minuq:", (vui128_t)j, (vui128_t) e);

  i1 = (vui128_t) CONST_VINT128_DW ( __UINT64_MAX__, __INT64_MAX__ );
  i2 = (vui128_t) CONST_VINT128_DW ( __UINT64_MAX__, __UINT64_MAX__ );
  e =  (vui128_t) CONST_VINT128_DW ( __UINT64_MAX__, __INT64_MAX__ );
  j = vec_minuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vminuq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_minuq:", (vui128_t)j, (vui128_t) e);

  i1 = (vui128_t) CONST_VINT128_DW ( __UINT64_MAX__, __UINT64_MAX__ );
  i2 = (vui128_t) CONST_VINT128_DW ( __UINT64_MAX__, __INT64_MAX__ );
  e =  (vui128_t) CONST_VINT128_DW ( __UINT64_MAX__, __INT64_MAX__ );
  j = vec_minuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vminuq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_minuq:", (vui128_t)j, (vui128_t) e);

  return (rc);
}
#undef __DEBUG_PRINT__

//#define __DEBUG_PRINT__ 1
int
test_minsq (void)
{
  vi128_t i1, i2, j, e;
  int rc = 0;

  printf ("\ntest_minsq Vector Minimum Signed Quadword\n");

  i1 = (vi128_t) CONST_VINT128_DW ( 0, 0 );
  i2 = (vi128_t) CONST_VINT128_DW ( 0, 0 );
  e =  (vi128_t) CONST_VINT128_DW ( 0, 0 );
  j = vec_minsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vminsq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_minsq:", (vui128_t)j, (vui128_t) e);

  i1 = (vi128_t) CONST_VINT128_DW ( 0, 1 );
  i2 = (vi128_t) CONST_VINT128_DW ( 0, 2 );
  e =  (vi128_t) CONST_VINT128_DW ( 0, 1 );
  j = vec_minsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vminsq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_minsq:", (vui128_t)j, (vui128_t) e);

  i1 = (vi128_t) CONST_VINT128_DW ( 0, 2 );
  i2 = (vi128_t) CONST_VINT128_DW ( 0, 1 );
  e =  (vi128_t) CONST_VINT128_DW ( 0, 1 );
  j = vec_minsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vminsq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_minsq:", (vui128_t)j, (vui128_t) e);

  i1 = (vi128_t) CONST_VINT128_DW ( 1, 0 );
  i2 = (vi128_t) CONST_VINT128_DW ( 2, 0 );
  e =  (vi128_t) CONST_VINT128_DW ( 1, 0 );
  j = vec_minsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vminsq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_minsq:", (vui128_t)j, (vui128_t) e);

  i1 = (vi128_t) CONST_VINT128_DW ( 2, 0 );
  i2 = (vi128_t) CONST_VINT128_DW ( 1, 0 );
  e =  (vi128_t) CONST_VINT128_DW ( 1, 0 );
  j = vec_minsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vminsq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_minsq:", (vui128_t)j, (vui128_t) e);

  i1 = (vi128_t) CONST_VINT128_DW ( __UINT64_MAX__, __INT64_MAX__ );
  i2 = (vi128_t) CONST_VINT128_DW ( __UINT64_MAX__, __UINT64_MAX__ );
  e =  (vi128_t) CONST_VINT128_DW ( __UINT64_MAX__, __INT64_MAX__ );
  j = vec_minsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vminsq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_minsq:", (vui128_t)j, (vui128_t) e);

  i1 = (vi128_t) CONST_VINT128_DW ( __UINT64_MAX__, __UINT64_MAX__ );
  i2 = (vi128_t) CONST_VINT128_DW ( __UINT64_MAX__, __INT64_MAX__ );
  e =  (vi128_t) CONST_VINT128_DW ( __UINT64_MAX__, __INT64_MAX__ );
  j = vec_minsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vminsq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_minsq:", (vui128_t)j, (vui128_t) e);

  i1 = (vi128_t) CONST_VINT128_DW ( __INT64_MAX__, __INT64_MAX__ );
  i2 = (vi128_t) CONST_VINT128_DW ( __UINT64_MAX__, __UINT64_MAX__ );
  e =  (vi128_t) CONST_VINT128_DW ( __UINT64_MAX__, __UINT64_MAX__ );
  j = vec_minsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vminsq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_minsq:", (vui128_t)j, (vui128_t) e);

  i1 = (vi128_t) CONST_VINT128_DW ( __UINT64_MAX__, __UINT64_MAX__ );
  i2 = (vi128_t) CONST_VINT128_DW ( __INT64_MAX__, __INT64_MAX__ );
  e =  (vi128_t) CONST_VINT128_DW ( __UINT64_MAX__, __UINT64_MAX__ );
  j = vec_minsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vminsq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_minsq:", (vui128_t)j, (vui128_t) e);

  return (rc);
}
#undef __DEBUG_PRINT__

//#define __DEBUG_PRINT__ 1
int
test_absduq (void)
{
  vui128_t i1, i2, j, e;
  int rc = 0;

  printf ("\ntest_absduq Vector Absolute Difference Unsigned Quadword\n");

  i1 = (vui128_t) CONST_VINT128_DW ( 0, 0 );
  i2 = (vui128_t) CONST_VINT128_DW ( 0, 0 );
  e =  (vui128_t) CONST_VINT128_DW ( 0, 0 );
  j = vec_absduq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("absduq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_absduq:", (vui128_t)j, (vui128_t) e);

  i1 = (vui128_t) CONST_VINT128_DW ( 0, 3 );
  i2 = (vui128_t) CONST_VINT128_DW ( 0, 5 );
  e =  (vui128_t) CONST_VINT128_DW ( 0, 2 );
  j = vec_absduq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("absduq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_absduq:", (vui128_t)j, (vui128_t) e);

  i1 = (vui128_t) CONST_VINT128_DW ( 0, 5 );
  i2 = (vui128_t) CONST_VINT128_DW ( 0, 3 );
  e =  (vui128_t) CONST_VINT128_DW ( 0, 2 );
  j = vec_absduq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("absduq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_absduq:", (vui128_t)j, (vui128_t) e);

  i1 = (vui128_t) CONST_VINT128_DW ( 7, 5 );
  i2 = (vui128_t) CONST_VINT128_DW ( 5, 3 );
  e =  (vui128_t) CONST_VINT128_DW ( 2, 2 );
  j = vec_absduq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("absduq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_absduq:", (vui128_t)j, (vui128_t) e);

  i1 = (vui128_t) CONST_VINT128_DW ( 5, 3 );
  i2 = (vui128_t) CONST_VINT128_DW ( 7, 5 );
  e =  (vui128_t) CONST_VINT128_DW ( 2, 2 );
  j = vec_absduq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("absduq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_absduq:", (vui128_t)j, (vui128_t) e);

  i1 = (vui128_t) CONST_VINT128_DW ( __UINT64_MAX__, __INT64_MAX__ );
  i2 = (vui128_t) CONST_VINT128_DW ( __UINT64_MAX__, __UINT64_MAX__ );
  e =  (vui128_t) CONST_VINT128_DW ( 0UL, 0x8000000000000000UL);
  j = vec_absduq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("absduq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_absduq:", (vui128_t)j, (vui128_t) e);

  i1 = (vui128_t) CONST_VINT128_DW ( __UINT64_MAX__, __UINT64_MAX__ );
  i2 = (vui128_t) CONST_VINT128_DW ( __UINT64_MAX__, __INT64_MAX__ );
  e =  (vui128_t) CONST_VINT128_DW ( 0UL, 0x8000000000000000UL);
  j = vec_absduq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("absduq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_absduq:", (vui128_t)j, (vui128_t) e);

  i1 = (vui128_t) CONST_VINT128_DW ( __INT64_MAX__, __UINT64_MAX__ );
  i2 = (vui128_t) CONST_VINT128_DW ( __INT64_MAX__, __INT64_MAX__ );
  e =  (vui128_t) CONST_VINT128_DW ( 0UL, 0x8000000000000000UL);
  j = vec_absduq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("absduq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_absduq:", (vui128_t)j, (vui128_t) e);

  i1 = (vui128_t) CONST_VINT128_DW ( __UINT64_MAX__, __UINT64_MAX__ );
  i2 = (vui128_t) CONST_VINT128_DW ( __INT64_MAX__, __INT64_MAX__ );
  e =  (vui128_t) CONST_VINT128_DW ( 0x8000000000000000UL, 0x8000000000000000UL);
  j = vec_absduq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("absduq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_absduq:", (vui128_t)j, (vui128_t) e);

  return (rc);
}
#undef __DEBUG_PRINT__

//#define __DEBUG_PRINT__ 1
int
test_avguq (void)
{
  vui128_t i1, i2, j, e;
  int rc = 0;

  printf ("\ntest_avguq Vector Average Unsigned Quadword\n");

  i1 = (vui128_t) CONST_VINT128_DW ( 0, 0 );
  i2 = (vui128_t) CONST_VINT128_DW ( 0, 0 );
  e =  (vui128_t) CONST_VINT128_DW ( 0, 0 );
  j = vec_avguq (i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vavguq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_avguq:", (vui128_t)j, (vui128_t) e);

  i1 = (vui128_t) CONST_VINT128_DW ( 0, 3 );
  i2 = (vui128_t) CONST_VINT128_DW ( 0, 7 );
  e =  (vui128_t) CONST_VINT128_DW ( 0, 5 );
  j = vec_avguq (i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vavguq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_avguq:", (vui128_t)j, (vui128_t) e);

  i1 = (vui128_t) CONST_VINT128_DW ( 0, 9 );
  i2 = (vui128_t) CONST_VINT128_DW ( 0, 5 );
  e =  (vui128_t) CONST_VINT128_DW ( 0, 7 );
  j = vec_avguq (i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vavguq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_avguq:", (vui128_t)j, (vui128_t) e);

  i1 = (vui128_t) CONST_VINT128_DW ( 0, 2 );
  i2 = (vui128_t) CONST_VINT128_DW ( 0, 7 );
  e =  (vui128_t) CONST_VINT128_DW ( 0, 5 );
  j = vec_avguq (i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vavguq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_avguq:", (vui128_t)j, (vui128_t) e);

  i1 = (vui128_t) CONST_VINT128_DW ( 0, 9 );
  i2 = (vui128_t) CONST_VINT128_DW ( 0, 4 );
  e =  (vui128_t) CONST_VINT128_DW ( 0, 7 );
  j = vec_avguq (i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vavguq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_avguq:", (vui128_t)j, (vui128_t) e);

  i1 = (vui128_t) CONST_VINT128_DW ( __INT64_MAX__, __UINT64_MAX__ );
  i2 = (vui128_t) CONST_VINT128_DW ( __INT64_MAX__, __INT64_MAX__ );
  e =  (vui128_t) CONST_VINT128_DW ( __INT64_MAX__, 0xbfffffffffffffffUL);
  j = vec_avguq (i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vavguq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_avguq:", (vui128_t)j, (vui128_t) e);

  i1 = (vui128_t) CONST_VINT128_DW ( __UINT64_MAX__, __UINT64_MAX__ );
  i2 = (vui128_t) CONST_VINT128_DW ( __UINT64_MAX__, __INT64_MAX__ );
  e =  (vui128_t) CONST_VINT128_DW ( __UINT64_MAX__, 0xbfffffffffffffffUL);
  j = vec_avguq (i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vavguq( ", i1);
  print_vint128x ("       ,", i2);
  print_vint128x ("      )=", j);
#endif
  rc += check_vuint128x ("vec_avguq:", (vui128_t)j, (vui128_t) e);

  return (rc);
}
#undef __DEBUG_PRINT__

#define TIMING_ITERATIONS 10

int
test_time_i128 (void)
{
  long i;
  uint64_t t_start, t_end, t_delta;
  double delta_sec;
  int rc = 0;

  printf ("\n%s mul10uq start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_mul10uq ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s mul10uq end", __FUNCTION__);
  printf ("\n%s mul10uq  tb delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

  printf ("\n%s cmul10ecuq start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_cmul10ecuq ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s cmul10ecuq end", __FUNCTION__);
  printf ("\n%s cmul10ecuq  tb delta = %lu, sec = %10.6g\n", __FUNCTION__,
	  t_delta, delta_sec);

  printf ("\n%s mulluq start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_mulluq ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s mulluq end", __FUNCTION__);
  printf ("\n%s mulluq  tb delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

  printf ("\n%s muludq start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_muludq ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s muludq end", __FUNCTION__);
  printf ("\n%s muludq  tb delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

  printf ("\n%s muludqx start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_muludqx ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s muludqx end", __FUNCTION__);
  printf ("\n%s muludqx  tb delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

  printf ("\n%s longdiv_e32 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_longdiv_e32 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s longdiv_e32 end", __FUNCTION__);
  printf ("\n%s longdiv_e32 tb delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

#ifndef PVECLIB_DISABLE_DFP
  printf ("\n%s longbcdcf_10e32 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_longbcdcf_10e32 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s longbcdcf_10e32 end", __FUNCTION__);
  printf ("\n%s longbcdcf_10e32 tb delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

  printf ("\n%s longbcdct_10e32 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_longbcdct_10e32 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);
#endif

  printf ("\n%s longbcdct_10e32 end", __FUNCTION__);
  printf ("\n%s longbcdct_10e32 tb delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

  printf ("\n%s cfmaxdouble_10e32 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_cfmaxdouble_10e32 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s cfmaxdouble_10e32 end", __FUNCTION__);
  printf ("\n%s cfmaxdouble_10e32 delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

  printf ("\n%s ctmaxdouble_10e32 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_ctmaxdouble_10e32 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s ctmaxdouble_10e32 end", __FUNCTION__);
  printf ("\n%s ctmaxdouble_10e32 delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_subcuq (void)
{
  vui32_t i, j, k;
  vui32_t e;
  int rc = 0;

  printf ("\ntest_subuq Vector subtract & Write Carry unsigned quadword\n");

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  k = (vui32_t) vec_subcuq ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("0 - 0", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  rc += check_vuint128x ("vec_subcuq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  k = (vui32_t) vec_subcuq ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 - 0", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  rc += check_vuint128x ("vec_subcuq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_subcuq ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("0 - 1", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  rc += check_vuint128x ("vec_subcuq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_subcuq ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2E32-1 - 1", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  rc += check_vuint128x ("vec_subcuq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_subcuq ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2E128-1 - 1", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  rc += check_vuint128x ("vec_subcuq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t)CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__);
  k = (vui32_t) vec_subcuq ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 - 2E128-1", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  rc += check_vuint128x ("vec_subcuq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__);
  j = (vui32_t)CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__);
  k = (vui32_t) vec_subcuq ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2E128-1 - 2E128-1", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  rc += check_vuint128x ("vec_subcuq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__-1);
  j = (vui32_t)CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__);
  k = (vui32_t) vec_subcuq ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2E128-2 - 2E128-1", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  rc += check_vuint128x ("vec_subcuq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__);
  j = (vui32_t)CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__-1);
  k = (vui32_t) vec_subcuq ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2E128-1 - 2E128-2", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  rc += check_vuint128x ("vec_subcuq:", (vui128_t) k, (vui128_t) e);

  return (rc);
}
#undef  __DEBUG_PRINT__
//#define __DEBUG_PRINT__ 1
int
test_subecuq (void)
{
  vui32_t i, j, k, l, m;
  vui32_t e, ec;
  int rc = 0;

  printf ("\ntest_subecuq Vector subtract extended/carry unsigned quadword modulo\n");

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  k = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  l = (vui32_t) vec_subeuqm ((vui128_t) i, (vui128_t) j, (vui128_t) k);
  m = (vui32_t) vec_subecuq ((vui128_t) i, (vui128_t) j, (vui128_t) k);

#ifdef __DEBUG_PRINT__
  print_vint128x_extend ("extent 0 - 0 + c=0", l, m, i, j, k);
#endif
  e = (vui32_t)CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint256 ("vec_subeuqm/ecuq:", (vui128_t) m, (vui128_t) l, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  k = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  l = (vui32_t) vec_subeuqm ((vui128_t) i, (vui128_t) j, (vui128_t) k);
  m = (vui32_t) vec_subecuq ((vui128_t) i, (vui128_t) j, (vui128_t) k);

#ifdef __DEBUG_PRINT__
  print_vint128x_extend ("extent 0 - 0 + c=1", l, m, i, j, k);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vint256 ("vec_subeuqm/ecuq:", (vui128_t) m, (vui128_t) l, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  k = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  l = (vui32_t) vec_subeuqm ((vui128_t) i, (vui128_t) j, (vui128_t) k);
  m = (vui32_t) vec_subecuq ((vui128_t) i, (vui128_t) j, (vui128_t) k);

#ifdef __DEBUG_PRINT__
  print_vint128x_extend ("extent 1 - 0 + c=0", l, m, i, j, k);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vint256 ("vec_subeuqm/ecuq:", (vui128_t) m, (vui128_t) l, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  l = (vui32_t) vec_subeuqm ((vui128_t) i, (vui128_t) j, (vui128_t) k);
  m = (vui32_t) vec_subecuq ((vui128_t) i, (vui128_t) j, (vui128_t) k);

#ifdef __DEBUG_PRINT__
  print_vint128x_extend ("extent 0 - 1 + c=0", l, m, i, j, k);
#endif
  e = (vui32_t)CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint256 ("vec_subeuqm/ecuq:", (vui128_t) m, (vui128_t) l, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  k = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  l = (vui32_t) vec_subeuqm ((vui128_t) i, (vui128_t) j, (vui128_t) k);
  m = (vui32_t) vec_subecuq ((vui128_t) i, (vui128_t) j, (vui128_t) k);

#ifdef __DEBUG_PRINT__
  print_vint128x_extend ("extent 1 - 0 + c=1", l, m, i, j, k);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vint256 ("vec_subeuqm/ecuq:", (vui128_t) m, (vui128_t) l, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  l = (vui32_t) vec_subeuqm ((vui128_t) i, (vui128_t) j, (vui128_t) k);
  m = (vui32_t) vec_subecuq ((vui128_t) i, (vui128_t) j, (vui128_t) k);

#ifdef __DEBUG_PRINT__
  print_vint128x_extend ("extent 0 - 1 + c=1", l, m, i, j, k);
#endif
  e = (vui32_t)CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  ec = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint256 ("vec_subeuqm/ecuq:", (vui128_t) m, (vui128_t) l, (vui128_t) ec,
                       (vui128_t) e);

  return (rc);
}
#undef  __DEBUG_PRINT__
//#define __DEBUG_PRINT__ 1
int
test_subuq (void)
{
#ifdef __DEBUG_PRINT__
  unsigned __int128 a, b, c;
#endif
  vui32_t i, j, k;
  vui32_t e;
  int rc = 0;

  rc += test_subcuq ();

  rc += test_subecuq ();

  printf ("\ntest_subuq Vector subtract unsigned quadword modulo\n");

#ifdef __DEBUG_PRINT__
  a = __UINT32_MAX__;
  b = 1UL;
  c = a - b;

  print_int128_sum ("2E32-1 - 1", c, a, b);
#endif
  i = (vui32_t)CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_subuqm ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2E32-1 - 1", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0xfffffffe);
  rc += check_vuint128x ("vec_subuqm:", (vui128_t) k, (vui128_t) e);

#ifdef __DEBUG_PRINT__
  a = 1UL;
  b = __UINT32_MAX__;
  c = a - b;

  print_int128_sum ("1 - 2E32-1", c, a, b);
#endif
  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  k = (vui32_t) vec_subuqm ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 - 2E32-1", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0x00000002);
  rc += check_vuint128x ("vec_subuqm:", (vui128_t) k, (vui128_t) e);

#ifdef __DEBUG_PRINT__
  a = __UINT64_MAX__;
  b = 1UL;
  c = a - b;

  print_int128_sum ("2E64-1 - 1", c, a, b);
#endif
  i = (vui32_t)CONST_VINT32_W(0, 0, __UINT32_MAX__, __UINT32_MAX__);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_subuqm ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2E64-1 - 1", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0xffffffff, 0xfffffffe);
  rc += check_vuint128x ("vec_subuqm:", (vui128_t) k, (vui128_t) e);

#ifdef __DEBUG_PRINT__
  a = 1UL;
  a = a << 96;
  b = 1UL;
  c = a - b;

  print_int128_sum ("2E96 - 1", c, a, b);
#endif
  i = (vui32_t)CONST_VINT32_W(1, 0, 0, 0);
  j = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_subuqm ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2E96 - 1", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0xffffffff, 0xffffffff, 0xffffffff);
  rc += check_vuint128x ("vec_subuqm:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j = (vui32_t)CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  k = (vui32_t) vec_subuqm ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2E128-1 - 2E128-1", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_subuqm:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j = (vui32_t)CONST_VINT32_W(0x7fffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  k = (vui32_t) vec_subuqm ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2E128-1 - 2E127-1", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0x80000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_subuqm:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0x7fffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j = (vui32_t)CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  k = (vui32_t) vec_subuqm ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2E127-1 - 2E128-1", k, i, j);
#endif
  e = (vui32_t)CONST_VINT32_W(0x80000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_subuqm:", (vui128_t) k, (vui128_t) e);

  return (rc);
}
#undef  __DEBUG_PRINT__
//#define __DEBUG_PRINT__ 1
int
test_setbq (void)
{
  vui32_t i, k;
  vui32_t e;
  int rc = 0;

  printf ("\ntest_setbq Vector set bool quadword\n");

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  k = (vui32_t) vec_setb_cyq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_setb ", (vui128_t) i);
  print_vint128x ("        =", (vui128_t) k);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_setb_cyq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_setb_cyq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_setb ", (vui128_t) i);
  print_vint128x ("        =", (vui128_t) k);
#endif
  e = (vui32_t)CONST_VINT32_W(-1, -1, -1, -1);
  rc += check_vuint128x ("vec_setb_cyq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  k = (vui32_t) vec_setb_ncq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_setb ", (vui128_t) i);
  print_vint128x ("        =", (vui128_t) k);
#endif
  e = (vui32_t)CONST_VINT32_W(-1, -1, -1, -1);
  rc += check_vuint128x ("vec_setb_ncq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_setb_ncq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_setb ", (vui128_t) i);
  print_vint128x ("        =", (vui128_t) k);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_setb_ncq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  k = (vui32_t) vec_setb_sq ((vi128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_setb ", (vui128_t) i);
  print_vint128x ("        =", (vui128_t) k);
#endif
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_setb_sq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0x80000000, 0, 0, 1);
  k = (vui32_t) vec_setb_sq ((vi128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_setb ", (vui128_t) i);
  print_vint128x ("        =", (vui128_t) k);
#endif
  e = (vui32_t)CONST_VINT32_W(-1, -1, -1, -1);
  rc += check_vuint128x ("vec_setb_sq:", (vui128_t) k, (vui128_t) e);
  rc += check_vuint128x ("vec_setb_sq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(-1, -1, -1, -1);
  k = (vui32_t) vec_setb_sq ((vi128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_setb ", (vui128_t) i);
  print_vint128x ("        =", (vui128_t) k);
#endif
  e = (vui32_t)CONST_VINT32_W(-1, -1, -1, -1);
  rc += check_vuint128x ("vec_setb_sq:", (vui128_t) k, (vui128_t) e);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_cmpuq_p2 (void)
{
  vui128_t i1, i2, e;
  vb128_t j;
  int rc = 0;
  printf ("test_cmpltuq\n");

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0, 0);
  e =  CONST_VINT128_DW128(0, 0);
  j = vec_cmpltuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpltuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpltuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0, 1);
  i2 = CONST_VINT128_DW128(0, 0);
  e =  CONST_VINT128_DW128(0, 0);
  j = vec_cmpltuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpltuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpltuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0, 1);
  e =  CONST_VINT128_DW128(-1, -1);
  j = vec_cmpltuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpltuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpltuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0, 1);
  i2 = CONST_VINT128_DW128(0, 1);
  e =  CONST_VINT128_DW128(0, 0);
  j = vec_cmpltuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpltuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpltuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = CONST_VINT128_DW128(0, 0);
  e =  CONST_VINT128_DW128(0, 0);
  j = vec_cmpltuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpltuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpltuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0x8000000000000000, 0);
  e =  CONST_VINT128_DW128(-1, -1);
  j = vec_cmpltuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpltuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpltuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = CONST_VINT128_DW128(0x8000000000000000, 0);
  e =  CONST_VINT128_DW128(0, 0);
  j = vec_cmpltuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpltuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpltuq:", (vb128_t)j, (vb128_t) e);

  printf ("test_cmpleuq\n");

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0, 0);
  e =  CONST_VINT128_DW128(-1, -1);
  j = vec_cmpleuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpleuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpleuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0, 1);
  i2 = CONST_VINT128_DW128(0, 0);
  e =  CONST_VINT128_DW128(0, 0);
  j = vec_cmpleuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpleuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpleuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0, 1);
  e =  CONST_VINT128_DW128(-1, -1);
  j = vec_cmpleuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpleuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpleuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0, 1);
  i2 = CONST_VINT128_DW128(0, 1);
  e =  CONST_VINT128_DW128(-1, -1);
  j = vec_cmpleuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpleuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpleuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = CONST_VINT128_DW128(0, 0);
  e =  CONST_VINT128_DW128(0, 0);
  j = vec_cmpleuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpleuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpleuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0x8000000000000000, 0);
  e =  CONST_VINT128_DW128(-1, -1);
  j = vec_cmpleuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpleuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpleuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = CONST_VINT128_DW128(0x8000000000000000, 0);
  e =  CONST_VINT128_DW128(-1, -1);
  j = vec_cmpleuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpleuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpleuq:", (vb128_t)j, (vb128_t) e);

  return (rc);
}

int
test_cmpuq (void)
{
  vui128_t i1, i2, e;
  vb128_t j;
  int rc = 0;

  printf ("\ntest_cmpuq Vector Compare Unsigned Quadword\n");

  printf ("test_cmpequq\n");

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0, 0);
  e =  CONST_VINT128_DW128(-1, -1);
  j = vec_cmpequq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpequq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpequq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0, 1);
  i2 = CONST_VINT128_DW128(0, 0);
  e =  CONST_VINT128_DW128(0, 0);
  j = vec_cmpequq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpequq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpequq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0, 1);
  e =  CONST_VINT128_DW128(0, 0);
  j = vec_cmpequq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpequq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpequq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0, 1);
  i2 = CONST_VINT128_DW128(0, 1);
  e =  CONST_VINT128_DW128(-1, -1);
  j = vec_cmpequq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpequq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpequq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = CONST_VINT128_DW128(0, 0);
  e =  CONST_VINT128_DW128(0, 0);
  j = vec_cmpequq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpequq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpequq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0x8000000000000000, 0);
  e =  CONST_VINT128_DW128(0, 0);
  j = vec_cmpequq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpequq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpequq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = CONST_VINT128_DW128(0x8000000000000000, 0);
  e =  CONST_VINT128_DW128(-1, -1);
  j = vec_cmpequq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpequq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpequq:", (vb128_t)j, (vb128_t) e);

  printf ("test_cmpneuq\n");

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0, 0);
  e =  CONST_VINT128_DW128(0, 0);
  j = vec_cmpneuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpneuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpneuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0, 1);
  i2 = CONST_VINT128_DW128(0, 0);
  e =  CONST_VINT128_DW128(-1, -1);
  j = vec_cmpneuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpneuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpneuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0, 1);
  e =  CONST_VINT128_DW128(-1, -1);
  j = vec_cmpneuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpneuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpneuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0, 1);
  i2 = CONST_VINT128_DW128(0, 1);
  e =  CONST_VINT128_DW128(0, 0);
  j = vec_cmpneuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpneuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpneuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = CONST_VINT128_DW128(0, 0);
  e =  CONST_VINT128_DW128(-1, -1);
  j = vec_cmpneuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpneuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpneuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0x8000000000000000, 0);
  e =  CONST_VINT128_DW128(-1, -1);
  j = vec_cmpneuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpneuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpneuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = CONST_VINT128_DW128(0x8000000000000000, 0);
  e =  CONST_VINT128_DW128(0, 0);
  j = vec_cmpneuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpneuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpneuq:", (vb128_t)j, (vb128_t) e);

  printf ("test_cmpgtuq\n");

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0, 0);
  e =  CONST_VINT128_DW128(0, 0);
  j = vec_cmpgtuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpgtuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpgtuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0, 1);
  i2 = CONST_VINT128_DW128(0, 0);
  e =  CONST_VINT128_DW128(-1, -1);
  j = vec_cmpgtuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpgtuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpgtuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0, 1);
  e =  CONST_VINT128_DW128(0, 0);
  j = vec_cmpgtuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpgtuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpgtuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0, 1);
  i2 = CONST_VINT128_DW128(0, 1);
  e =  CONST_VINT128_DW128(0, 0);
  j = vec_cmpgtuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpgtuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpgtuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = CONST_VINT128_DW128(0, 0);
  e =  CONST_VINT128_DW128(-1, -1);
  j = vec_cmpgtuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpgtuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpgtuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0x8000000000000000, 0);
  e =  CONST_VINT128_DW128(0, 0);
  j = vec_cmpgtuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpgtuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpgtuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = CONST_VINT128_DW128(0x8000000000000000, 0);
  e =  CONST_VINT128_DW128(0, 0);
  j = vec_cmpgtuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpgtuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpgtuq:", (vb128_t)j, (vb128_t) e);

  printf ("test_cmpgeuq\n");

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0, 0);
  e =  CONST_VINT128_DW128(-1, -1);
  j = vec_cmpgeuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpgeuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpgeuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0, 1);
  i2 = CONST_VINT128_DW128(0, 0);
  e =  CONST_VINT128_DW128(-1, -1);
  j = vec_cmpgeuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpgeuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpgeuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0, 1);
  e =  CONST_VINT128_DW128(0, 0);
  j = vec_cmpgeuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpgeuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpgeuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0, 1);
  i2 = CONST_VINT128_DW128(0, 1);
  e =  CONST_VINT128_DW128(-1, -1);
  j = vec_cmpgeuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpgeuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpgeuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = CONST_VINT128_DW128(0, 0);
  e =  CONST_VINT128_DW128(-1, -1);
  j = vec_cmpgeuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpgeuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpgeuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0x8000000000000000, 0);
  e =  CONST_VINT128_DW128(0, 0);
  j = vec_cmpgeuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpgeuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpgeuq:", (vb128_t)j, (vb128_t) e);

  i1 = CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = CONST_VINT128_DW128(0x8000000000000000, 0);
  e =  CONST_VINT128_DW128(-1, -1);
  j = vec_cmpgeuq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpgeuq( ", i1);
  print_vint128x ("         ,", i2);
  print_vint128x ("        )=", j);
#endif
  rc += check_vb128c ("vec_cmpgeuq:", (vb128_t)j, (vb128_t) e);

#if 1
  rc += test_cmpuq_p2 ();
#endif
  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_cmpsq_p2 (void)
{
  vi128_t i1, i2, e;
  vb128_t j;
  int rc = 0;
  printf ("test_cmpltsq\n");

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  e =  (vi128_t)CONST_VINT128_DW128(0, 0);
  j = vec_cmpltsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpltsq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpltsq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0, 1);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  e =  (vi128_t)CONST_VINT128_DW128(0, 0);
  j = vec_cmpltsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpltsq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpltsq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 1);
  e =  (vi128_t)CONST_VINT128_DW128(-1, -1);
  j = vec_cmpltsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpltsq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpltsq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0, 1);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 1);
  e =  (vi128_t)CONST_VINT128_DW128(0, 0);
  j = vec_cmpltsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpltsq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpltsq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  e =  (vi128_t)CONST_VINT128_DW128(-1, -1);
  j = vec_cmpltsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpltsq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpltsq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  e =  (vi128_t)CONST_VINT128_DW128(0, 0);
  j = vec_cmpltsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpltsq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpltsq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  e =  (vi128_t)CONST_VINT128_DW128(0, 0);
  j = vec_cmpltsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpltsq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpltsq:", (vb128_t)j, (vb128_t) e);

  printf ("test_cmplesq\n");

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  e =  (vi128_t)CONST_VINT128_DW128(-1, -1);
  j = vec_cmplesq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmplesq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmplesq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0, 1);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  e =  (vi128_t)CONST_VINT128_DW128(0, 0);
  j = vec_cmplesq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmplesq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmplesq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 1);
  e =  (vi128_t)CONST_VINT128_DW128(-1, -1);
  j = vec_cmplesq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmplesq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmplesq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0, 1);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 1);
  e =  (vi128_t)CONST_VINT128_DW128(-1, -1);
  j = vec_cmplesq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmplesq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmplesq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  e =  (vi128_t)CONST_VINT128_DW128(-1, -1);
  j = vec_cmplesq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmplesq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmplesq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  e =  (vi128_t)CONST_VINT128_DW128(0, 0);
  j = vec_cmplesq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmplesq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmplesq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  e =  (vi128_t)CONST_VINT128_DW128(-1, -1);
  j = vec_cmplesq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmplesq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmplesq:", (vb128_t)j, (vb128_t) e);

  return (rc);
}

int
test_cmpsq (void)
{
  vi128_t i1, i2, e;
  vb128_t j;
  int rc = 0;

  printf ("\ntest_cmpsq Vector Compare Unsigned Quadword\n");

  printf ("test_cmpeqsq\n");

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  e =  (vi128_t)CONST_VINT128_DW128(-1, -1);
  j = vec_cmpeqsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpeqsq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpeqsq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0, 1);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  e =  (vi128_t)CONST_VINT128_DW128(0, 0);
  j = vec_cmpeqsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpeqsq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpeqsq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 1);
  e =  (vi128_t)CONST_VINT128_DW128(0, 0);
  j = vec_cmpeqsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpeqsq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpeqsq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0, 1);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 1);
  e =  (vi128_t)CONST_VINT128_DW128(-1, -1);
  j = vec_cmpeqsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpeqsq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpeqsq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  e =  (vi128_t)CONST_VINT128_DW128(0, 0);
  j = vec_cmpeqsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpeqsq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpeqsq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  e =  (vi128_t)CONST_VINT128_DW128(0, 0);
  j = vec_cmpeqsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpeqsq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpeqsq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  e =  (vi128_t)CONST_VINT128_DW128(-1, -1);
  j = vec_cmpeqsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpeqsq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpeqsq:", (vb128_t)j, (vb128_t) e);

  printf ("test_cmpnesq\n");

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  e =  (vi128_t)CONST_VINT128_DW128(0, 0);
  j = vec_cmpnesq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpnesq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpnesq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0, 1);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  e =  (vi128_t)CONST_VINT128_DW128(-1, -1);
  j = vec_cmpnesq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpnesq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpnesq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 1);
  e =  (vi128_t)CONST_VINT128_DW128(-1, -1);
  j = vec_cmpnesq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpnesq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpnesq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0, 1);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 1);
  e =  (vi128_t)CONST_VINT128_DW128(0, 0);
  j = vec_cmpnesq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpnesq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpnesq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  e =  (vi128_t)CONST_VINT128_DW128(-1, -1);
  j = vec_cmpnesq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpnesq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpnesq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  e =  (vi128_t)CONST_VINT128_DW128(-1, -1);
  j = vec_cmpnesq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpnesq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpnesq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  e =  (vi128_t)CONST_VINT128_DW128(0, 0);
  j = vec_cmpnesq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpnesq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpnesq:", (vb128_t)j, (vb128_t) e);

  printf ("test_cmpgtsq\n");

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  e =  (vi128_t)CONST_VINT128_DW128(0, 0);
  j = vec_cmpgtsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpgtsq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpgtsq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0, 1);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  e =  (vi128_t)CONST_VINT128_DW128(-1, -1);
  j = vec_cmpgtsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpgtsq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpgtsq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 1);
  e =  (vi128_t)CONST_VINT128_DW128(0, 0);
  j = vec_cmpgtsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpgtsq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpgtsq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0, 1);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 1);
  e =  (vi128_t)CONST_VINT128_DW128(0, 0);
  j = vec_cmpgtsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpgtsq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpgtsq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  e =  (vi128_t)CONST_VINT128_DW128(0, 0);
  j = vec_cmpgtsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpgtsq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpgtsq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  e =  (vi128_t)CONST_VINT128_DW128(-1, -1);
  j = vec_cmpgtsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpgtsq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpgtsq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  e =  (vi128_t)CONST_VINT128_DW128(0, 0);
  j = vec_cmpgtsq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpgtsq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpgtsq:", (vb128_t)j, (vb128_t) e);

  printf ("test_cmpgesq\n");

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  e =  (vi128_t)CONST_VINT128_DW128(-1, -1);
  j = vec_cmpgesq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpgesq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpgesq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0, 1);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  e =  (vi128_t)CONST_VINT128_DW128(-1, -1);
  j = vec_cmpgesq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpgesq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpgesq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 1);
  e =  (vi128_t)CONST_VINT128_DW128(0, 0);
  j = vec_cmpgesq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpgesq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpgesq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0, 1);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 1);
  e =  (vi128_t)CONST_VINT128_DW128(-1, -1);
  j = vec_cmpgesq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpgesq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpgesq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  e =  (vi128_t)CONST_VINT128_DW128(0, 0);
  j = vec_cmpgesq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpgesq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpgesq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  e =  (vi128_t)CONST_VINT128_DW128(-1, -1);
  j = vec_cmpgesq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpgesq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpgesq:", (vb128_t)j, (vb128_t) e);

  i1 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  e =  (vi128_t)CONST_VINT128_DW128(-1, -1);
  j = vec_cmpgesq(i1, i2);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vcmpgesq( ", (vui128_t)i1);
  print_vint128x ("         ,", (vui128_t)i2);
  print_vint128x ("        )=", (vui128_t)j);
#endif
  rc += check_vb128c ("vec_cmpgesq:", (vb128_t)j, (vb128_t) e);

#if 1
  rc += test_cmpsq_p2 ();
#endif
  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_cmpuq_all (void)
{
  vui128_t i1, i2;
#ifdef __DEBUG_PRINT__
  vb128_t j;
#endif
  int rc = 0;

  printf ("\ntest_cmpuq_all Vector Compare Unsigned quadword\n");

  printf ("test_cmpuq_all_eq\n");

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0, 0);

  if (vec_cmpuq_all_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpuq_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequq(i1, i2);
      print_vint128x ("vec_cmpuq_all_eq( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    }

  i1 = CONST_VINT128_DW128(0, 1);
  i2 = CONST_VINT128_DW128(0, 0);
  if (vec_cmpuq_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpuq_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequq(i1, i2);
      print_vint128x ("vec_cmpuq_all_eq( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    } else {
    }

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0, 1);
  if (vec_cmpuq_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpuq_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequq(i1, i2);
      print_vint128x ("vec_cmpuq_all_eq( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    } else {
    }

  i1 = CONST_VINT128_DW128(0, 1);
  i2 = CONST_VINT128_DW128(0, 1);
  if (vec_cmpuq_all_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpuq_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequq(i1, i2);
      print_vint128x ("vec_cmpuq_all_eq( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    }

  i1 = CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = CONST_VINT128_DW128(0, 0);
  if (vec_cmpuq_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpuq_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequq(i1, i2);
      print_vint128x ("vec_cmpuq_all_eq( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    } else {
    }

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0x8000000000000000, 0);
  if (vec_cmpuq_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpuq_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequq(i1, i2);
      print_vint128x ("vec_cmpuq_all_eq( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    } else {
    }

  i1 = CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = CONST_VINT128_DW128(0x8000000000000000, 0);
  if (vec_cmpuq_all_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpuq_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequq(i1, i2);
      print_vint128x ("vec_cmpuq_all_eq( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    }

  printf ("test_cmpuq_all_ne\n");

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0, 0);
  if (vec_cmpuq_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpuq_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_ne( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    } else {
    }

  i1 = CONST_VINT128_DW128(0, 1);
  i2 = CONST_VINT128_DW128(0, 0);
  if (vec_cmpuq_all_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpuq_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_ne( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    }

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0, 1);
  if (vec_cmpuq_all_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpuq_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_ne( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    }

  i1 = CONST_VINT128_DW128(0, 1);
  i2 = CONST_VINT128_DW128(0, 1);
  if (vec_cmpuq_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpuq_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_ne( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    } else {
    }

  i1 = CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = CONST_VINT128_DW128(0, 0);
  if (vec_cmpuq_all_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpuq_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_ne( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    }

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0x8000000000000000, 0);
  if (vec_cmpuq_all_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpuq_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_ne( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    }

  i1 = CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = CONST_VINT128_DW128(0x8000000000000000, 0);
  if (vec_cmpuq_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpuq_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_ne( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    } else {
    }

  printf ("test_cmpuq_all_gt\n");

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0, 0);
  if (vec_cmpuq_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpuq_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_gt( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    } else {
    }

  i1 = CONST_VINT128_DW128(0, 1);
  i2 = CONST_VINT128_DW128(0, 0);
  if (vec_cmpuq_all_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpuq_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_gt( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    }

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0, 1);
  if (vec_cmpuq_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpuq_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_gt( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    } else {
    }

  i1 = CONST_VINT128_DW128(0, 1);
  i2 = CONST_VINT128_DW128(0, 1);
  if (vec_cmpuq_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpuq_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_gt( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    } else {
    }

  i1 = CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = CONST_VINT128_DW128(0, 0);
  if (vec_cmpuq_all_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpuq_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_gt( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    }

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0x8000000000000000, 0);
  if (vec_cmpuq_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpuq_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_gt( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    } else {
    }

  i1 = CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = CONST_VINT128_DW128(0x8000000000000000, 0);
  if (vec_cmpuq_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpuq_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_gt( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    } else {
    }

  printf ("test_cmpuq_all_ge\n");

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0, 0);
  if (vec_cmpuq_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpuq_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_ge( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    }

  i1 = CONST_VINT128_DW128(0, 1);
  i2 = CONST_VINT128_DW128(0, 0);
  if (vec_cmpuq_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpuq_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_ge( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    }

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0, 1);
  if (vec_cmpuq_all_ge (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpuq_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_ge( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    } else {
    }

  i1 = CONST_VINT128_DW128(0, 1);
  i2 = CONST_VINT128_DW128(0, 1);
  if (vec_cmpuq_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpuq_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_ge( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    }

  i1 = CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = CONST_VINT128_DW128(0, 0);
  if (vec_cmpuq_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpuq_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_ge( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    }

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0x8000000000000000, 0);
  if (vec_cmpuq_all_ge (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpuq_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_ge( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    } else {
    }

  i1 = CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = CONST_VINT128_DW128(0x8000000000000000, 0);
  if (vec_cmpuq_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpuq_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_ge( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    }

  printf ("test_cmpuq_all_lt\n");

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0, 0);
  if (vec_cmpuq_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpuq_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_lt( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    } else {
    }

  i1 = CONST_VINT128_DW128(0, 1);
  i2 = CONST_VINT128_DW128(0, 0);
  if (vec_cmpuq_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpuq_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_lt( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    } else {
    }

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0, 1);
  if (vec_cmpuq_all_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpuq_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_lt( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    }

  i1 = CONST_VINT128_DW128(0, 1);
  i2 = CONST_VINT128_DW128(0, 1);
  if (vec_cmpuq_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpuq_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_lt( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    } else {
    }

  i1 = CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = CONST_VINT128_DW128(0, 0);
  if (vec_cmpuq_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpuq_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_lt( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    } else {
    }

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0x8000000000000000, 0);
  if (vec_cmpuq_all_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpuq_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_lt( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    }

  i1 = CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = CONST_VINT128_DW128(0x8000000000000000, 0);
  if (vec_cmpuq_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpuq_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_lt( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    } else {
    }

  printf ("test_cmpuq_all_le\n");

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0, 0);
  if (vec_cmpuq_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpuq_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_le( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    }

  i1 = CONST_VINT128_DW128(0, 1);
  i2 = CONST_VINT128_DW128(0, 0);
  if (vec_cmpuq_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpuq_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_le( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    } else {
    }

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0, 1);
  if (vec_cmpuq_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpuq_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_le( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    }

  i1 = CONST_VINT128_DW128(0, 1);
  i2 = CONST_VINT128_DW128(0, 1);
  if (vec_cmpuq_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpuq_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_le( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    }

  i1 = CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = CONST_VINT128_DW128(0, 0);
  if (vec_cmpuq_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpuq_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_le( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    } else {
    }

  i1 = CONST_VINT128_DW128(0, 0);
  i2 = CONST_VINT128_DW128(0x8000000000000000, 0);
  if (vec_cmpuq_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpuq_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_le( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    }

  i1 = CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = CONST_VINT128_DW128(0x8000000000000000, 0);
  if (vec_cmpuq_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpuq_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltuq(i1, i2);
      print_vint128x ("vec_cmpuq_all_le( ", i1);
      print_vint128x ("                 ,", i2);
      print_vint128x ("                )=", j);
#endif
    }

  return (rc);
}
//#define __DEBUG_PRINT__ 1
int
test_cmpsq_all (void)
{
  vi128_t i1, i2;
#ifdef __DEBUG_PRINT__
  vb128_t j;
#endif
  int rc = 0;

  printf ("\ntest_cmpsq_all Vector Compare Signed quadword\n");

  printf ("test_cmpsq_all_eq\n");

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);

  if (vec_cmpsq_all_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsq_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_eq( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0, 1);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  if (vec_cmpsq_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsq_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_eq( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    } else {
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 1);
  if (vec_cmpsq_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsq_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_eq( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    } else {
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0, 1);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 1);
  if (vec_cmpsq_all_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsq_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_eq( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  if (vec_cmpsq_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsq_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_eq( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    } else {
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  if (vec_cmpsq_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsq_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_eq( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    } else {
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  if (vec_cmpsq_all_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsq_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_eq( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    }

  printf ("test_cmpsq_all_ne\n");

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  if (vec_cmpsq_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsq_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesq(i1, i2);
      print_vint128x ("vec_cmpsq_all_ne( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    } else {
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0, 1);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  if (vec_cmpsq_all_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsq_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesq(i1, i2);
      print_vint128x ("vec_cmpsq_all_ne( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 1);
  if (vec_cmpsq_all_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsq_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesq(i1, i2);
      print_vint128x ("vec_cmpsq_all_ne( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0, 1);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 1);
  if (vec_cmpsq_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsq_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesq(i1, i2);
      print_vint128x ("vec_cmpsq_all_ne( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    } else {
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  if (vec_cmpsq_all_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsq_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesq(i1, i2);
      print_vint128x ("vec_cmpsq_all_ne( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  if (vec_cmpsq_all_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsq_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesq(i1, i2);
      print_vint128x ("vec_cmpsq_all_ne( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  if (vec_cmpsq_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsq_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesq(i1, i2);
      print_vint128x ("vec_cmpsq_all_ne( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    } else {
    }

  printf ("test_cmpsq_all_gt\n");

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  if (vec_cmpsq_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsq_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_gt( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    } else {
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0, 2);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 1);
  if (vec_cmpsq_all_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsq_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_gt( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0, 1);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  if (vec_cmpsq_all_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsq_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_gt( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 1);
  if (vec_cmpsq_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsq_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_gt( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    } else {
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0, 1);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 1);
  if (vec_cmpsq_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsq_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_gt( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    } else {
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  if (vec_cmpsq_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsq_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_gt( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    } else {
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  if (vec_cmpsq_all_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsq_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_gt( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  if (vec_cmpsq_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsq_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_gt( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    } else {
    }

  printf ("test_cmpsq_all_ge\n");

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  if (vec_cmpsq_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsq_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesq(i1, i2);
      print_vint128x ("vec_cmpsq_all_ge( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0, 2);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 1);
  if (vec_cmpsq_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsq_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesq(i1, i2);
      print_vint128x ("vec_cmpsq_all_ge( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0, 1);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  if (vec_cmpsq_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsq_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesq(i1, i2);
      print_vint128x ("vec_cmpsq_all_ge( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 1);
  if (vec_cmpsq_all_ge (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsq_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesq(i1, i2);
      print_vint128x ("vec_cmpsq_all_ge( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    } else {
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0, 1);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 1);
  if (vec_cmpsq_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsq_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesq(i1, i2);
      print_vint128x ("vec_cmpsq_all_ge( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  if (vec_cmpsq_all_ge (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsq_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesq(i1, i2);
      print_vint128x ("vec_cmpsq_all_ge( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    } else {
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  if (vec_cmpsq_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsq_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesq(i1, i2);
      print_vint128x ("vec_cmpsq_all_ge( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  if (vec_cmpsq_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsq_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesq(i1, i2);
      print_vint128x ("vec_cmpsq_all_ge( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    }

  printf ("test_cmpsq_all_lt\n");

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  if (vec_cmpsq_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsq_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_lt( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    } else {
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0, 1);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  if (vec_cmpsq_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsq_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_lt( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    } else {
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 1);
  if (vec_cmpsq_all_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsq_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_lt( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0, 1);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 1);
  if (vec_cmpsq_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsq_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_lt( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    } else {
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  if (vec_cmpsq_all_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsq_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_lt( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  if (vec_cmpsq_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsq_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_lt( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    } else {
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  if (vec_cmpsq_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsq_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_lt( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    } else {
    }

  printf ("test_cmpsq_all_le\n");

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  if (vec_cmpsq_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsq_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_le( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0, 1);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  if (vec_cmpsq_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsq_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_le( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    } else {
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 1);
  if (vec_cmpsq_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsq_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_le( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0, 1);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 1);
  if (vec_cmpsq_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsq_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_le( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0, 0);
  if (vec_cmpsq_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsq_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_le( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  if (vec_cmpsq_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsq_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_le( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    } else {
    }

  i1 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  i2 = (vi128_t)CONST_VINT128_DW128(0x8000000000000000, 0);
  if (vec_cmpsq_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsq_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsq(i1, i2);
      print_vint128x ("vec_cmpsq_all_le( ", (vui128_t)i1);
      print_vint128x ("                 ,", (vui128_t)i2);
      print_vint128x ("                )=", (vui128_t)j);
#endif
    }

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_div_moduq_e32 (void)
{
  vui128_t i;
  vui128_t k, l, e;
  int rc = 0;

  printf ("\ntest Vector divide/modulo Unsigned Quadword\n");

  i = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  e = (vui128_t)CONST_VINT128_DW128(0, 3402823);

  k = vec_divuq_10e32 (i);

#ifdef __DEBUG_PRINT__
  print_vint128 (" divuq_10e32 ", (vui128_t)i);
  print_vint128 ("           = ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_divuq_10e32:", (vui128_t)k, (vui128_t) e);

  e = (vui128_t) CONST_VINT128_DW128 ( 0x0000034ca936deeeUL,
				       0xc98ba738ffffffffUL );
  l = vec_moduq_10e32 (i, k);

#ifdef __DEBUG_PRINT__
  print_vint128 (" moduq_10e32 ", (vui128_t)i);
  print_vint128 ("           q ", (vui128_t)k);
  print_vint128 ("           = ", (vui128_t)l);
  print_vint128x("           = 0x", (vui128_t)l);
#endif
  rc += check_vuint128x ("vec_moduq_10e32:", (vui128_t)l, (vui128_t) e);

  i = (vui128_t) { (__int128 ) 10000000000000000UL
                 * (__int128 ) 10000000000000000UL };;
  e = (vui128_t)CONST_VINT128_DW128(0, 1);

  k = vec_divuq_10e32 (i);

#ifdef __DEBUG_PRINT__
  print_vint128 (" divuq_10e32 ", (vui128_t)i);
  print_vint128 ("           = ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_divuq_10e32:", (vui128_t)k, (vui128_t) e);

  e = (vui128_t) CONST_VINT128_DW128 ( 0UL,
				       0UL );
  l = vec_moduq_10e32 (i, k);

#ifdef __DEBUG_PRINT__
  print_vint128 (" moduq_10e32 ", (vui128_t)i);
  print_vint128 ("           q ", (vui128_t)k);
  print_vint128 ("           = ", (vui128_t)l);
  print_vint128x("           = 0x", (vui128_t)l);
#endif
  rc += check_vuint128x ("vec_moduq_10e32:", (vui128_t)l, (vui128_t) e);

  i = (vui128_t) { (__int128 )  9999999999999999UL
                 * (__int128 ) 10000000000000000UL
		 + (__int128 )  9999999999999999UL};
  e = (vui128_t)CONST_VINT128_DW128(0, 0);

  k = vec_divuq_10e32 (i);

#ifdef __DEBUG_PRINT__
  print_vint128 (" divuq_10e32 ", (vui128_t)i);
  print_vint128 ("           = ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_divuq_10e32:", (vui128_t)k, (vui128_t) e);

  e = (vui128_t) { (__int128 )  9999999999999999UL
                 * (__int128 ) 10000000000000000UL
		 + (__int128 )  9999999999999999UL};
  l = vec_moduq_10e32 (i, k);

#ifdef __DEBUG_PRINT__
  print_vint128 (" moduq_10e32 ", (vui128_t)i);
  print_vint128 ("           q ", (vui128_t)k);
  print_vint128 ("           = ", (vui128_t)l);
  print_vint128x("           = 0x", (vui128_t)l);
#endif
  rc += check_vuint128x ("vec_moduq_10e32:", (vui128_t)l, (vui128_t) e);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_div_moduq_e31 (void)
{
  vui128_t i;
  vui128_t k, l, e;
  int rc = 0;

  printf ("\ntest Vector divide/modulo Unsigned Quadword\n");

  i = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  e = (vui128_t)CONST_VINT128_DW128(0, 34028236);

  k = vec_divuq_10e31 (i);

#ifdef __DEBUG_PRINT__
  print_vint128 (" divuq_10e31 ", (vui128_t)i);
  print_vint128 ("           = ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_divuq_10e31:", (vui128_t)k, (vui128_t) e);

  e = (vui128_t) CONST_VINT128_DW128 ( 0x000000575ac21e1eUL,
				       0x4623e451ffffffffUL );
  l = vec_moduq_10e31 (i, k);

#ifdef __DEBUG_PRINT__
  print_vint128 (" moduq_10e31 ", (vui128_t)i);
  print_vint128 ("           q ", (vui128_t)k);
  print_vint128 ("           = ", (vui128_t)l);
  print_vint128x("           = 0x", (vui128_t)l);
#endif
  rc += check_vuint128x ("vec_moduq_10e31:", (vui128_t)l, (vui128_t) e);

  i = (vui128_t) { (__int128 ) 1000000000000000UL
                 * (__int128 ) 10000000000000000UL };;
  e = (vui128_t)CONST_VINT128_DW128(0, 1);

  k = vec_divuq_10e31 (i);

#ifdef __DEBUG_PRINT__
  print_vint128 (" divuq_10e31 ", (vui128_t)i);
  print_vint128 ("           = ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_divuq_10e31:", (vui128_t)k, (vui128_t) e);

  e = (vui128_t) CONST_VINT128_DW128 ( 0UL,
				       0UL );
  l = vec_moduq_10e31 (i, k);

#ifdef __DEBUG_PRINT__
  print_vint128 (" moduq_10e31 ", (vui128_t)i);
  print_vint128 ("           q ", (vui128_t)k);
  print_vint128 ("           = ", (vui128_t)l);
  print_vint128x("           = 0x", (vui128_t)l);
#endif
  rc += check_vuint128x ("vec_moduq_10e31:", (vui128_t)l, (vui128_t) e);

  i = (vui128_t) { (__int128 )   999999999999999UL
                 * (__int128 ) 10000000000000000UL
		 + (__int128 )  9999999999999999UL};
  e = (vui128_t)CONST_VINT128_DW128(0, 0);

  k = vec_divuq_10e31 (i);

#ifdef __DEBUG_PRINT__
  print_vint128 (" divuq_10e31 ", (vui128_t)i);
  print_vint128 ("           = ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_divuq_10e31:", (vui128_t)k, (vui128_t) e);

  e = (vui128_t) { (__int128 )   999999999999999UL
                 * (__int128 ) 10000000000000000UL
		 + (__int128 )  9999999999999999UL};
  l = vec_moduq_10e31 (i, k);

#ifdef __DEBUG_PRINT__
  print_vint128 (" moduq_10e31 ", (vui128_t)i);
  print_vint128 ("           q ", (vui128_t)k);
  print_vint128 ("           = ", (vui128_t)l);
  print_vint128x("           = 0x", (vui128_t)l);
#endif
  rc += check_vuint128x ("vec_moduq_10e31:", (vui128_t)l, (vui128_t) e);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_div_modsq_e31 (void)
{
  vi128_t i;
  vi128_t k, l, e;
  int rc = 0;

  printf ("\ntest Vector divide/modulo Unsigned Quadword\n");

  i = (vi128_t)CONST_VINT128_DW128(__INT64_MAX__, __UINT64_MAX__);
  e = (vi128_t)CONST_VINT128_DW128(0, 17014118);

  k = vec_divsq_10e31 (i);

#ifdef __DEBUG_PRINT__
  print_vint128s(" divsq_10e31 ", (vi128_t)i);
  print_vint128s("           = ", (vi128_t)k);
#endif
  rc += check_vuint128x ("vec_divsq_10e31:", (vui128_t)k, (vui128_t) e);

  e = (vi128_t) CONST_VINT128_DW128 ( 0x0000002bad610f0fUL,
				      0x2311f228ffffffffUL );
  l = vec_modsq_10e31 (i, k);

#ifdef __DEBUG_PRINT__
  print_vint128s(" modsq_10e31 ", (vi128_t)i);
  print_vint128s("           q ", (vi128_t)k);
  print_vint128s("           = ", (vi128_t)l);
  print_vint128x("           = 0x", (vui128_t)l);
#endif
  rc += check_vuint128x ("vec_modsq_10e31:", (vui128_t)l, (vui128_t) e);

  i = (vi128_t) { (__int128 ) 1000000000000000UL
                 * (__int128 ) 10000000000000000UL };
  e = (vi128_t)CONST_VINT128_DW128(0, 1);

  k = vec_divsq_10e31 (i);

#ifdef __DEBUG_PRINT__
  print_vint128s(" divsq_10e31 ", (vi128_t)i);
  print_vint128s("           = ", (vi128_t)k);
#endif
  rc += check_vuint128x ("vec_divsq_10e31:", (vui128_t)k, (vui128_t) e);

  e = (vi128_t) CONST_VINT128_DW128 ( 0UL,
				       0UL );
  l = vec_modsq_10e31 (i, k);

#ifdef __DEBUG_PRINT__
  print_vint128s(" modsq_10e31 ", (vi128_t)i);
  print_vint128s("           q ", (vi128_t)k);
  print_vint128s("           = ", (vi128_t)l);
  print_vint128x("           = 0x", (vui128_t)l);
#endif
  rc += check_vuint128x ("vec_modsq_10e31:", (vui128_t)l, (vui128_t) e);

  i = (vi128_t) { (__int128 )   999999999999999UL
                 * (__int128 ) 10000000000000000UL
		 + (__int128 )  9999999999999999UL};
  e = (vi128_t)CONST_VINT128_DW128(0, 0);

  k = vec_divsq_10e31 (i);

#ifdef __DEBUG_PRINT__
  print_vint128s(" divsq_10e31 ", (vi128_t)i);
  print_vint128s("           = ", (vi128_t)k);
#endif
  rc += check_vuint128x ("vec_divsq_10e31:", (vui128_t)k, (vui128_t) e);

  e = (vi128_t) { (__int128 )   999999999999999UL
                 * (__int128 ) 10000000000000000UL
		 + (__int128 )  9999999999999999UL};
  l = vec_modsq_10e31 (i, k);

#ifdef __DEBUG_PRINT__
  print_vint128s(" modsq_10e31 ", (vi128_t)i);
  print_vint128s("           q ", (vi128_t)k);
  print_vint128s("           = ", (vi128_t)l);
  print_vint128x("           = 0x", (vui128_t)l);
#endif
  rc += check_vuint128x ("vec_modsq_10e31:", (vui128_t)l, (vui128_t) e);

  i = (vi128_t)CONST_VINT128_DW128(0x8000000000000000UL, 0);
  e = (vi128_t)CONST_VINT128_DW128(__UINT64_MAX__, -17014118);

  k = vec_divsq_10e31 (i);

#ifdef __DEBUG_PRINT__
  print_vint128s(" divsq_10e31 ", (vi128_t)i);
  print_vint128s("           = ", (vi128_t)k);
#endif
  rc += check_vuint128x ("vec_divsq_10e31:", (vui128_t)k, (vui128_t) e);

  e = (vi128_t) CONST_VINT128_DW128 ( 0xffffffd4529ef0f0UL,
				      0xdcee0dd700000000UL );
  l = vec_modsq_10e31 (i, k);

#ifdef __DEBUG_PRINT__
  print_vint128s(" modsq_10e31 ", (vi128_t)i);
  print_vint128s("           q ", (vi128_t)k);
  print_vint128s("           = ", (vi128_t)l);
  print_vint128x("           = 0x", (vui128_t)l);
#endif
  rc += check_vuint128x ("vec_modsq_10e31:", (vui128_t)l, (vui128_t) e);

  i = (vi128_t)CONST_VINT128_DW128(0x8000000000000000UL, 1);
  e = (vi128_t)CONST_VINT128_DW128(__UINT64_MAX__, -17014118);

  k = vec_divsq_10e31 (i);

#ifdef __DEBUG_PRINT__
  print_vint128s(" divsq_10e31 ", (vi128_t)i);
  print_vint128s("           = ", (vi128_t)k);
#endif
  rc += check_vuint128x ("vec_divsq_10e31:", (vui128_t)k, (vui128_t) e);

  e = (vi128_t) CONST_VINT128_DW128 ( 0xffffffd4529ef0f0UL,
				      0xdcee0dd700000001UL );
  l = vec_modsq_10e31 (i, k);

#ifdef __DEBUG_PRINT__
  print_vint128s(" modsq_10e31 ", (vi128_t)i);
  print_vint128s("           q ", (vi128_t)k);
  print_vint128s("           = ", (vi128_t)l);
  print_vint128x("           = 0x", (vui128_t)l);
#endif
  rc += check_vuint128x ("vec_modsq_10e31:", (vui128_t)l, (vui128_t) e);

  i = (vi128_t) { -((__int128 )  1000000000000000UL
                 * (__int128 ) 10000000000000000UL) };
  e = (vi128_t) { -((__int128 ) 1UL) };

  k = vec_divsq_10e31 (i);

#ifdef __DEBUG_PRINT__
  print_vint128s(" divsq_10e31 ", (vi128_t)i);
  print_vint128s("           = ", (vi128_t)k);
#endif
  rc += check_vuint128x ("vec_divsq_10e31:", (vui128_t)k, (vui128_t) e);

  e = (vi128_t) CONST_VINT128_DW128 ( 0UL,
				       0UL );
  l = vec_modsq_10e31 (i, k);

#ifdef __DEBUG_PRINT__
  print_vint128s(" modsq_10e31 ", (vi128_t)i);
  print_vint128s("           q ", (vi128_t)k);
  print_vint128s("           = ", (vi128_t)l);
  print_vint128x("           = 0x", (vui128_t)l);
#endif
  rc += check_vuint128x ("vec_modsq_10e31:", (vui128_t)l, (vui128_t) e);

  i = (vi128_t) { -((__int128 )   999999999999999UL
                  * (__int128 ) 10000000000000000UL
		  + (__int128 )  9999999999999999UL)};
  e = (vi128_t)CONST_VINT128_DW128(0, 0);

  k = vec_divsq_10e31 (i);

#ifdef __DEBUG_PRINT__
  print_vint128s(" divsq_10e31 ", (vi128_t)i);
  print_vint128s("           = ", (vi128_t)k);
#endif
  rc += check_vuint128x ("vec_divsq_10e31:", (vui128_t)k, (vui128_t) e);

  e = (vi128_t) { -((__int128 )   999999999999999UL
                  * (__int128 ) 10000000000000000UL
		  + (__int128 )  9999999999999999UL)};
  l = vec_modsq_10e31 (i, k);

#ifdef __DEBUG_PRINT__
  print_vint128s(" modsq_10e31 ", (vi128_t)i);
  print_vint128s("           q ", (vi128_t)k);
  print_vint128s("           = ", (vi128_t)l);
  print_vint128x("           = 0x", (vui128_t)l);
#endif
  rc += check_vuint128x ("vec_modsq_10e31:", (vui128_t)l, (vui128_t) e);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
#ifdef __DEBUG_PRINT__
#define test_vec_divudq_10e31(_i, _j, _k)	db_vec_divudq_10e31(_i, _j, _k)
#define test_vec_modudq_10e31(_i, _j, _k)	db_vec_modudq_10e31(_i, _j, _k)
#else
#define test_vec_divudq_10e31(_i, _j, _k)	vec_divudq_10e31(_i, _j, _k)
#define test_vec_modudq_10e31(_i, _j, _k)	vec_modudq_10e31(_i, _j, _k)
#endif
int
test_div_modudq_e31 (void)
{
  /* ten32  = +100000000000000000000000000000000UQ  */
  const vui128_t ten32 = (vui128_t)
	  { (__int128) 10000000000000000UL * (__int128) 10000000000000000UL };
  /* ten31  = +10000000000000000000000000000000UQ  */
  const vui128_t ten31 = (vui128_t)
	  { (__int128) 1000000000000000UL * (__int128) 10000000000000000UL };
  const vui128_t ten1 = (vui128_t) { (__int128) 10UL };
  const vui128_t ten2 = (vui128_t) { (__int128) 100UL };
  const vui128_t zero = (vui128_t) { (__int128) 0UL };
  vui128_t ih, il;
  vui128_t kh, kl, eh, el, er;
  vui128_t rl, rm;
  int rc = 0;

  printf ("\ntest Vector divide/modulo Unsigned Double Quadword\n");
#ifdef __DEBUG_PRINT__
  print_vint128x (" 10**31 ", ten31);
  print_vint128x (" 10**32 ", ten32);
#endif

  il = vec_muludq (&ih, ten31, ten31);
#ifdef __DEBUG_PRINT__
  print_vint128x ("\n    value= ", (vui128_t) ih);
  print_vint128x ("           ", (vui128_t) il);
#endif
  eh = (vui128_t)CONST_VINT128_DW128(0, 0);
  el = ten31;
  er = zero;
  kl = test_vec_divudq_10e31 (&kh, ih, il);
  rl = test_vec_modudq_10e31 (ih, il, &kl);

#ifdef __DEBUG_PRINT__
  print_vint128x (" div/mod31 ", (vui128_t) ih);
  print_vint128x ("           ", (vui128_t) il);
  print_vint128x ("         = ", (vui128_t) kh);
  print_vint128x ("           ", (vui128_t) kl);
  print_vint128x ("       rl= ", (vui128_t) rl);
  print_vint128  ("       rl=", (vui128_t) rl);
#endif
  rc += check_vint256 ("dive 10e62:", (vui128_t) kh, (vui128_t) kl,
		       (vui128_t) eh, (vui128_t) el);
  rc += check_vuint128x ("modq_10e62:", (vui128_t)rl, (vui128_t) er);

  ih = kh;
  il = kl;
  eh = (vui128_t)CONST_VINT128_DW128(0, 0);
  el = (vui128_t)CONST_VINT128_DW128(0, 1UL);
  kl = test_vec_divudq_10e31 (&kh, ih, il);
  rm = test_vec_modudq_10e31 (ih, il, &kl);

#ifdef __DEBUG_PRINT__
  print_vint128x (" div/mod31 ", (vui128_t) ih);
  print_vint128x ("           ", (vui128_t) il);
  print_vint128x ("         = ", (vui128_t) kh);
  print_vint128x ("           ", (vui128_t) kl);
  print_vint128x ("       rm= ", (vui128_t) rm);
  print_vint128  ("       rm=", (vui128_t) rm);
#endif
  rc += check_vint256 ("dive 10e31:", (vui128_t) kh, (vui128_t) kl,
		       (vui128_t) eh, (vui128_t) el);
  rc += check_vuint128x ("modq_10e31:", (vui128_t)rm, (vui128_t) er);


  il = vec_muludq (&ih, ten31, ten32);
#ifdef __DEBUG_PRINT__
  print_vint128x ("\n    value= ", (vui128_t) ih);
  print_vint128x ("           ", (vui128_t) il);
#endif
  eh = (vui128_t)CONST_VINT128_DW128(0, 0);
  el = vec_mulluq (ten31, ten1);
  kl = test_vec_divudq_10e31 (&kh, ih, il);
  rl = test_vec_modudq_10e31 (ih, il, &kl);

#ifdef __DEBUG_PRINT__
  print_vint128x (" div/mod31 ", (vui128_t) ih);
  print_vint128x ("           ", (vui128_t) il);
  print_vint128x ("         = ", (vui128_t) kh);
  print_vint128x ("           ", (vui128_t) kl);
  print_vint128x ("       rl= ", (vui128_t) rl);
  print_vint128  ("       rl=", (vui128_t) rl);
#endif
  rc += check_vint256 ("dive 10e63:", (vui128_t) kh, (vui128_t) kl,
		       (vui128_t) eh, (vui128_t) el);
  rc += check_vuint128x ("modq_10e63:", (vui128_t)rl, (vui128_t) er);

  ih = kh;
  il = kl;
  eh = (vui128_t)CONST_VINT128_DW128(0, 0);
  el = (vui128_t)CONST_VINT128_DW128(0, 10UL);
  kl = test_vec_divudq_10e31 (&kh, ih, il);
  rm = test_vec_modudq_10e31 (ih, il, &kl);

#ifdef __DEBUG_PRINT__
  print_vint128x (" div/mod31 ", (vui128_t) ih);
  print_vint128x ("           ", (vui128_t) il);
  print_vint128x ("         = ", (vui128_t) kh);
  print_vint128x ("           ", (vui128_t) kl);
  print_vint128x ("       rm= ", (vui128_t) rm);
  print_vint128  ("       rm=", (vui128_t) rm);
#endif
  rc += check_vint256 ("dive 10e32:", (vui128_t) kh, (vui128_t) kl,
		       (vui128_t) eh, (vui128_t) el);
  rc += check_vuint128x ("modq_10e32:", (vui128_t)rm, (vui128_t) er);


  il = vec_muludq (&ih, ten32, ten32);
#ifdef __DEBUG_PRINT__
  print_vint128x ("\n    value= ", (vui128_t) ih);
  print_vint128x ("           ", (vui128_t) il);
#endif
  eh = (vui128_t)CONST_VINT128_DW128(0, 0);
  el = vec_mulluq (ten31, ten2);
  kl = test_vec_divudq_10e31 (&kh, ih, il);
  rl = test_vec_modudq_10e31 (ih, il, &kl);

#ifdef __DEBUG_PRINT__
  print_vint128x (" div/mod31 ", (vui128_t) ih);
  print_vint128x ("           ", (vui128_t) il);
  print_vint128x ("         = ", (vui128_t) kh);
  print_vint128x ("           ", (vui128_t) kl);
  print_vint128x ("       rl= ", (vui128_t) rl);
  print_vint128  ("       rl=", (vui128_t) rl);
#endif
  rc += check_vint256 ("dive 10e64:", (vui128_t) kh, (vui128_t) kl,
		       (vui128_t) eh, (vui128_t) el);
  rc += check_vuint128x ("modq_10e64:", (vui128_t)rl, (vui128_t) er);

  ih = kh;
  il = kl;
  eh = (vui128_t)CONST_VINT128_DW128(0, 0);
  el = (vui128_t)CONST_VINT128_DW128(0, 100UL);
  kl = test_vec_divudq_10e31 (&kh, ih, il);
  rm = test_vec_modudq_10e31 (ih, il, &kl);

#ifdef __DEBUG_PRINT__
  print_vint128x (" div/mod31 ", (vui128_t) ih);
  print_vint128x ("           ", (vui128_t) il);
  print_vint128x ("         = ", (vui128_t) kh);
  print_vint128x ("           ", (vui128_t) kl);
  print_vint128x ("       rm= ", (vui128_t) rm);
  print_vint128  ("       rm=", (vui128_t) rm);
#endif
  rc += check_vint256 ("dive 10e32:", (vui128_t) kh, (vui128_t) kl,
		       (vui128_t) eh, (vui128_t) el);
  rc += check_vuint128x ("modq_10e32:", (vui128_t)rm, (vui128_t) er);


  ih = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  il = vec_muludq (&ih, ten31, ih);
#ifdef __DEBUG_PRINT__
  print_vint128x ("\n    value= ", (vui128_t) ih);
  print_vint128x ("           ", (vui128_t) il);
#endif
  eh = (vui128_t)CONST_VINT128_DW128(0, 0);
  el = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  kl = test_vec_divudq_10e31 (&kh, ih, il);
  rl = test_vec_modudq_10e31 (ih, il, &kl);

#ifdef __DEBUG_PRINT__
  print_vint128x (" div/mod31 ", (vui128_t) ih);
  print_vint128x ("           ", (vui128_t) il);
  print_vint128x ("         = ", (vui128_t) kh);
  print_vint128x ("           ", (vui128_t) kl);
  print_vint128x ("       rl= ", (vui128_t) rl);
  print_vint128  ("       rl=", (vui128_t) rl);
#endif
  rc += check_vint256 ("dive 10e31*2128:", (vui128_t) kh, (vui128_t) kl,
		       (vui128_t) eh, (vui128_t) el);
  rc += check_vuint128x ("modq_10e31*2128:", (vui128_t)rl, (vui128_t) er);

  ih = kh;
  il = kl;
  eh = (vui128_t)CONST_VINT128_DW128(0, 0);
  el = (vui128_t)CONST_VINT128_DW128(0, 34028236UL);
  er = (vui128_t)CONST_VINT128_DW128(0x000000575ac21e1eUL,
				     0x4623e451ffffffffUL);
  kl = test_vec_divudq_10e31 (&kh, ih, il);
  rm = test_vec_modudq_10e31 (ih, il, &kl);

#ifdef __DEBUG_PRINT__
  print_vint128x (" div/mod31 ", (vui128_t) ih);
  print_vint128x ("           ", (vui128_t) il);
  print_vint128x ("         = ", (vui128_t) kh);
  print_vint128x ("           ", (vui128_t) kl);
  print_vint128x ("       rm= ", (vui128_t) rm);
  print_vint128  ("       rm=", (vui128_t) rm);
#endif
  rc += check_vint256 ("dive 10e31*2128:", (vui128_t) kh, (vui128_t) kl,
		       (vui128_t) eh, (vui128_t) el);
  rc += check_vuint128x ("modq_10e31*2128:", (vui128_t)rm, (vui128_t) er);


  ih = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  il = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
#ifdef __DEBUG_PRINT__
  print_vint128x ("\n    value= ", (vui128_t) ih);
  print_vint128x ("           ", (vui128_t) il);
#endif
  eh = (vui128_t)CONST_VINT128_DW128(0, 34028236);
  el = (vui128_t)CONST_VINT128_DW128(0xb12d0ff3d203ab3eUL,
				     0x521dc33b5d1c63c2UL);
  er = (vui128_t)CONST_VINT128_DW128(0x0000004731a537a1UL,
				     0x2d1f2952ffffffffUL);
  if (vec_cmpuq_all_lt (ih, ten31))
    {
      kl = test_vec_divudq_10e31 (&kh, ih, il);
    }
  else
    {
      // Dividend is too large for multiplicative inverse
      // Have to use long division
      vui128_t qh, rh, r2;
      qh = vec_divuq_10e31 (ih);
      rh = vec_moduq_10e31 (ih, qh);
#ifdef __DEBUG_PRINT__
      print_vint128x (" div qh    ", (vui128_t) qh);
      print_vint128x (" mod rh    ", (vui128_t) rh);
#endif
      kl = test_vec_divudq_10e31 (&r2, rh ,il);
      kh = qh;
#ifdef __DEBUG_PRINT__
      print_vint128x (" result  = ", (vui128_t) r2);
      print_vint128x ("           ", (vui128_t) kl);
#endif
    }

  rl = test_vec_modudq_10e31 (ih, il, &kl);

#ifdef __DEBUG_PRINT__
  print_vint128x (" div/mod31 ", (vui128_t) ih);
  print_vint128x ("           ", (vui128_t) il);
  print_vint128x ("         = ", (vui128_t) kh);
  print_vint128x ("           ", (vui128_t) kl);
  print_vint128x ("       rl= ", (vui128_t) rl);
  print_vint128  ("       rl=", (vui128_t) rl);
#endif
  rc += check_vint256 ("dive 2e256:", (vui128_t) kh, (vui128_t) kl,
		       (vui128_t) eh, (vui128_t) el);
  rc += check_vuint128x ("modq_2e256:", (vui128_t)rl, (vui128_t) er);

  ih = kh;
  il = kl;
  eh = (vui128_t)CONST_VINT128_DW128(0, 0);
  el = (vui128_t)CONST_VINT128_DW128(0,
				     0x41d1f7777c8a9UL);
  er = (vui128_t)CONST_VINT128_DW128(0x000000787104ca39UL,
				     0x5b1792d0dd1c63c2UL);
  kl = test_vec_divudq_10e31 (&kh, ih, il);
  rm = test_vec_modudq_10e31 (ih, il, &kl);

#ifdef __DEBUG_PRINT__
  print_vint128x (" div/mod31 ", (vui128_t) ih);
  print_vint128x ("           ", (vui128_t) il);
  print_vint128x ("         = ", (vui128_t) kh);
  print_vint128x ("           ", (vui128_t) kl);
  print_vint128x ("       rm= ", (vui128_t) rm);
  print_vint128  ("       rm=", (vui128_t) rm);
#endif
  rc += check_vint256 ("dive 2e256r:", (vui128_t) kh, (vui128_t) kl,
		       (vui128_t) eh, (vui128_t) el);
  rc += check_vuint128x ("modq_2e256r:", (vui128_t)rm, (vui128_t) er);

  return (rc);
}

#ifdef __DEBUG_PRINT__
#define test_example_longdiv_10e31(_i, _j, _k)	db_example_longdiv_10e31(_i, _j, _k)
#else
extern vui128_t
example_longdiv_10e31 (vui128_t *q, vui128_t *d, long int _N);
#define test_example_longdiv_10e31(_i, _j, _k)	example_longdiv_10e31(_i, _j, _k)
#endif

int
test_longdiv_e31 (void)
{

  vui128_t ix[4], qx[4], qy[4], qz[4], qw[4];
  vui128_t eh, el, er;
  vui128_t rl, rm;
  int rc = 0;

  printf ("\ntest Vector long divide Unsigned Quadword\n");

  ix[0] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  ix[1] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  er = (vui128_t)CONST_VINT128_DW128(0x0000004731a537a1UL,
				     0x2d1f2952ffffffffUL);

  rl = test_example_longdiv_10e31 (qx, ix, 2UL);

#ifdef __DEBUG_PRINT__
  print_vint128x (" longdiv31 ", (vui128_t) ix[0]);
  print_vint128x ("           ", (vui128_t) ix[1]);
  print_vint128x ("         = ", (vui128_t) qx[0]);
  print_vint128x ("           ", (vui128_t) qx[1]);
  print_vint128x ("       rm= ", (vui128_t) rl);
  print_vint128  ("       rm=", (vui128_t) rl);
#endif

  rc += check_vuint128x ("modq_2e256r:", (vui128_t)rl, (vui128_t) er);
  er = (vui128_t)CONST_VINT128_DW128(0x000000787104ca39UL,
				     0x5b1792d0dd1c63c2UL);
  eh = (vui128_t)CONST_VINT128_DW128(0,
				     0x41d1f7777c8a9UL);

  rm = test_example_longdiv_10e31 (qy, qx, 2UL);

#ifdef __DEBUG_PRINT__
  print_vint128x (" longdiv31 ", (vui128_t) qx[0]);
  print_vint128x ("           ", (vui128_t) qx[1]);
  print_vint128x ("         = ", (vui128_t) qy[0]);
  print_vint128x ("           ", (vui128_t) qy[1]);
  print_vint128x ("       rm= ", (vui128_t) rm);
  print_vint128  ("       rm=", (vui128_t) rm);
#endif

  rc += check_vuint128x ("modq_2e256r:", (vui128_t)rm, (vui128_t) er);
  rc += check_vuint128x ("modq_2e256r:", (vui128_t)qy[1], (vui128_t) eh);

  printf ("\ntest Vector long divide Unsigned Quadword x 4\n");

  ix[0] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  ix[1] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  ix[2] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  ix[3] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  er = (vui128_t)CONST_VINT128_DW128(0x00000031020ac3afUL,
				     0x30b873467fffffffUL);

  rm = test_example_longdiv_10e31 (qx, ix, 4UL);

#ifdef __DEBUG_PRINT__
  print_vint128x (" longdiv31 ", (vui128_t) qx[0]);
  print_vint128x ("           ", (vui128_t) qx[1]);
  print_vint128x ("           ", (vui128_t) qx[2]);
  print_vint128x ("           ", (vui128_t) qx[3]);
  print_vint128x ("       rm= ", (vui128_t) rm);
  print_vint128  ("       rm=", (vui128_t) rm);
#endif
  rc += check_vuint128x ("modq_2e512r:", (vui128_t)rm, (vui128_t) er);

  er = (vui128_t)CONST_VINT128_DW128(0x0000005471A9D471UL,
				     0x4C0649AC298BCEBFUL);

  rm = test_example_longdiv_10e31 (qy, qx, 4UL);

#ifdef __DEBUG_PRINT__
  print_vint128x (" longdiv31 ", (vui128_t) qy[0]);
  print_vint128x ("           ", (vui128_t) qy[1]);
  print_vint128x ("           ", (vui128_t) qy[2]);
  print_vint128x ("           ", (vui128_t) qy[3]);
  print_vint128x ("       rm= ", (vui128_t) rm);
  print_vint128  ("       rm=", (vui128_t) rm);
#endif
  rc += check_vuint128x ("modq_2e512r:", (vui128_t)rm, (vui128_t) er);

  er = (vui128_t)CONST_VINT128_DW128(0x00000038016C67DEUL,
				     0x40C7C81EB3FBCEC5UL);

  rm = test_example_longdiv_10e31 (qz, qy, 4UL);

#ifdef __DEBUG_PRINT__
  print_vint128x (" longdiv31 ", (vui128_t) qz[0]);
  print_vint128x ("           ", (vui128_t) qz[1]);
  print_vint128x ("           ", (vui128_t) qz[2]);
  print_vint128x ("           ", (vui128_t) qz[3]);
  print_vint128x ("       rm= ", (vui128_t) rm);
  print_vint128  ("       rm=", (vui128_t) rm);
#endif
  rc += check_vuint128x ("modq_2e512r:", (vui128_t)rm, (vui128_t) er);

  er = (vui128_t)CONST_VINT128_DW128(0x00000049C9DB4F37UL,
				     0xC80C108D269EB4A9UL);

  el = (vui128_t)CONST_VINT128_DW128(0x00000010EC4BE0ADUL,
				     0x8F89516228E39AECUL);

  rm = test_example_longdiv_10e31 (qw, qz, 4UL);

#ifdef __DEBUG_PRINT__
  print_vint128x (" longdiv31 ", (vui128_t) qw[0]);
  print_vint128x ("           ", (vui128_t) qw[1]);
  print_vint128x ("           ", (vui128_t) qw[2]);
  print_vint128x ("           ", (vui128_t) qw[3]);
  print_vint128x ("       rm= ", (vui128_t) rm);
  print_vint128  ("       rm=", (vui128_t) rm);
#endif
  rc += check_vuint128x ("modq_2e512r:", (vui128_t)rm, (vui128_t) er);
  rc += check_vuint128x ("   q_2e512r:", (vui128_t)qw[3], (vui128_t) el);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
extern vui128_t
example_longdiv_10e32 (vui128_t *q, vui128_t *d, long int _N);
#ifdef __DEBUG_PRINT__
#define test_example_longdiv_10e32(_i, _j, _k)	example_longdiv_10e32(_i, _j, _k)
#else
#define test_example_longdiv_10e32(_i, _j, _k)	example_longdiv_10e32(_i, _j, _k)
#endif

int
test_longdiv_e32 (void)
{
#ifdef __DEBUG_PRINT__
  /* ten32  = +100000000000000000000000000000000UQ  */
  const vui128_t ten32 = (vui128_t)
	  { (__int128) 10000000000000000UL * (__int128) 10000000000000000UL };
#endif
  vui128_t ix[4], qx[4], qy[4], qz[4], qw[4];
  vui128_t el, er;
  vui128_t rm;
  int rc = 0;

  printf ("\ntest Vector long divide Unsigned Quadword\n");
#ifdef __DEBUG_PRINT__
  print_vint128x (" 10**32 ", ten32);
#endif

  printf ("\ntest Vector long divide Unsigned Quadword x 4\n");

  ix[0] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  ix[1] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  ix[2] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  ix[3] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  er = (vui128_t)CONST_VINT128_DW128(0x000002A818C1645CUL,
				     0xF38EEB06FFFFFFFFUL);

  rm = test_example_longdiv_10e32 (qx, ix, 4UL);

#ifdef __DEBUG_PRINT__
  print_vint128x (" longdiv32 ", (vui128_t) qx[0]);
  print_vint128x ("           ", (vui128_t) qx[1]);
  print_vint128x ("           ", (vui128_t) qx[2]);
  print_vint128x ("           ", (vui128_t) qx[3]);
  print_vint128x ("       rm= ", (vui128_t) rm);
  print_vint128  ("       rm=", (vui128_t) rm);
#endif
  rc += check_vuint128x ("modq_2e512r:", (vui128_t)rm, (vui128_t) er);

  er = (vui128_t)CONST_VINT128_DW128(0x00000406CEE1CCBEUL,
				     0x6C331B4910F46179UL);

  rm = test_example_longdiv_10e32 (qy, qx, 4UL);

#ifdef __DEBUG_PRINT__
  print_vint128x (" longdiv32 ", (vui128_t) qy[0]);
  print_vint128x ("           ", (vui128_t) qy[1]);
  print_vint128x ("           ", (vui128_t) qy[2]);
  print_vint128x ("           ", (vui128_t) qy[3]);
  print_vint128x ("       rm= ", (vui128_t) rm);
  print_vint128  ("       rm=", (vui128_t) rm);
#endif
  rc += check_vuint128x ("modq_2e512r:", (vui128_t)rm, (vui128_t) er);

  er = (vui128_t)CONST_VINT128_DW128(0x000002C4A4402B14UL,
				     0x8ADF2CB703147025UL);

  rm = test_example_longdiv_10e32 (qz, qy, 4UL);

#ifdef __DEBUG_PRINT__
  print_vint128x (" longdiv32 ", (vui128_t) qz[0]);
  print_vint128x ("           ", (vui128_t) qz[1]);
  print_vint128x ("           ", (vui128_t) qz[2]);
  print_vint128x ("           ", (vui128_t) qz[3]);
  print_vint128x ("       rm= ", (vui128_t) rm);
  print_vint128  ("       rm=", (vui128_t) rm);
#endif
  rc += check_vuint128x ("modq_2e512r:", (vui128_t)rm, (vui128_t) er);

  er = (vui128_t)CONST_VINT128_DW128(0x000004D78833F4BDUL,
				     0x83B9B4EFF89D57BBUL);

  el = (vui128_t)CONST_VINT128_DW128(0x00000000006EE823UL,
				     0x3E325E7250563E59UL);

  rm = test_example_longdiv_10e32 (qw, qz, 4UL);

#ifdef __DEBUG_PRINT__
  print_vint128x (" longdiv32 ", (vui128_t) qw[0]);
  print_vint128x ("           ", (vui128_t) qw[1]);
  print_vint128x ("           ", (vui128_t) qw[2]);
  print_vint128x ("           ", (vui128_t) qw[3]);
  print_vint128x ("       rm= ", (vui128_t) rm);
  print_vint128  ("       rm=", (vui128_t) rm);
#endif
  rc += check_vuint128x ("modq_2e512r:", (vui128_t)rm, (vui128_t) er);
  rc += check_vuint128x ("   q_2e512r:", (vui128_t)qw[3], (vui128_t) el);

  return (rc);
}

int
test_vec_i128 (void)
{
  int rc = 0;

  printf ("\n%s\n", __FUNCTION__);
#if 1
  rc += test_revbq ();
  rc += test_clzq ();
  rc += test_ctzq ();
  rc += test_popcntq();

  rc += test_vsrq ();
  rc += test_vslq ();
  rc += test_vsrqi ();
  rc += test_vslqi ();

  rc += test_vrlq ();
  rc += test_vrlqi ();

  rc += test_vsraq ();
  rc += test_vsraqi ();
  rc += test_vsldq ();
  rc += test_vsldqi ();

  rc += test_addq();

  rc += test_mul10uq ();
  rc += test_mul10ecuq ();
  rc += test_cmul100 ();

  rc += test_mulluq ();
  rc += test_mulhuq ();
  rc += test_muludq ();
  rc += test_madduq ();

  rc += test_msumudm ();

  rc += test_subuq();
  rc += test_setbq();
  rc += test_cmpuq();
  rc += test_cmpsq();
  rc += test_cmpuq_all();
  rc += test_cmpsq_all();
  rc += test_maxuq ();
  rc += test_maxsq ();
  rc += test_minuq ();
  rc += test_minsq ();
  rc += test_absduq ();
  rc += test_avguq ();

  rc += test_div_moduq_e32 ();
  rc += test_div_moduq_e31 ();
  rc += test_div_modsq_e31 ();
  rc += test_div_modudq_e31 ();
  rc += test_longdiv_e31 ();
  rc += test_longdiv_e32 ();
#endif
#if 1
  rc += test_time_i128();
#endif
  return (rc);
}

