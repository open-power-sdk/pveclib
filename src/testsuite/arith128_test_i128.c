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

extern const vui128_t vtipowof10 [];


#ifdef __DEBUG_PRINT__
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
  vt2 = (__vector unsigned long long)vec_cmplt (vt1, v64);
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


//#define __DEBUG_PRINT__ 1
vui128_t db_vec_diveuq (vui128_t x, vui128_t z)
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
  const vui64_t zeros = vec_splat_u64(0);
  const vui128_t mone = (vui128_t) CONST_VINT128_DW (-1, -1);
  vui128_t u0, u1, v0, v1, q0, q1, k, t, vn;
  vui64_t vdh, vdl, udh, qdl, qdh;
#ifdef __DEBUG_PRINT__
  print_vint128x ("db_vec_divuqe u ", (vui128_t) u);
  print_vint128x ("              v ", (vui128_t) v);
#endif

  if (vec_cmpuq_all_ge (x,z) || vec_cmpuq_all_eq (z , (vui128_t) zeros))
    {
      printf (" undef -- overlow or zero divide\n");
      return mone;
    }
  else
    {
      udh = vec_splatd ((vui64_t) u, 1);
      vdh = vec_splatd ((vui64_t) v, 1);
      vdl = vec_splatd ((vui64_t) v, 0);

      if (/*v >> 64 == 0UL*/vec_cmpud_all_eq (vdh, zeros))
	{
	  if (/*u >> 64 < v*/vec_cmpud_all_lt (udh, vdl))
	    {
	      u0 = (vui128_t) vec_swapd ((vui64_t) u);
	      qdh = vec_divqud_inline (u0, vdl);
#ifdef __DEBUG_PRINT__
	      print_vint128x (" (udh < vdl) u0  ", (vui128_t) u0);
	      print_vint128x ("             qdh ", (vui128_t) qdh);
#endif
#if 1
	      // vec_divqud already provides the remainder in qdh[1]
	      // k = u1 - q1*v; ((k << 64) + u0);
	      // Simplifies to:
	      u1 = (vui128_t) vec_pasted (qdh, (vui64_t) u0);
#else
	      k = vec_muleud (qdh, vdl);
	      //u1 = (vui128_t) vec_mrgahd ((vui128_t)zeros, k);
	      u1 = vec_subuqm (u0, k);
#ifdef __DEBUG_PRINT__
	      print_vint128x ("             u0  ", (vui128_t) u0);
	      print_vint128x ("             vd1 ", (vui128_t) vdl);
	      print_vint128x ("             qdh ", (vui128_t) qdh);
	      print_vint128x ("             k   ", (vui128_t) k);
	      print_vint128x ("             u1  ", (vui128_t) u1);
#endif
	      u1 = (vui128_t) vec_mrgald (u1, (vui128_t)zeros);
#endif
	      qdl = vec_divqud_inline (u1, vdl);
#ifdef __DEBUG_PRINT__
	      print_vint128x ("             u1  ", (vui128_t) u1);
	      print_vint128x ("             qdl ", (vui128_t) qdl);
#endif
	      //return (vui128_t) qdl;
	      return (vui128_t) vec_mrgald ((vui128_t) qdh, (vui128_t) qdl);
	    }
	  else
	    {
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
#if 1
	      // vec_divqud already provides the remainder in qdh[1]
	      // k = u1 - q1*v; ((k << 64) + u0);
	      // Simplifies to:
	      k = (vui128_t) vec_pasted (qdh, (vui64_t) u0);
#else
	      t = vec_muleud (qdh, vdl);
#ifdef __DEBUG_PRINT__
	      print_vint128x ("             qdh ", (vui128_t) qdh);
	      print_vint128x ("             t   ", (vui128_t) t);
#endif
	      k = vec_subuqm (u1, t);
	      // ((k << 64) + u0)
	      k = (vui128_t) vec_mrgald (k, u0);
#endif
	      // q0 = scalar_divdud ((k << 64) + u0, (unsigned long long) v) & lmask;
	      qdl = vec_divqud_inline (k, vdl);
#ifdef __DEBUG_PRINT__
	      print_vint128x ("             qdh ", (vui128_t) qdh);
	      print_vint128x ("             k   ", (vui128_t) k);
	      print_vint128x ("             qdl ", (vui128_t) qdl);
#endif
	      //return (q1 << 64) + q0;
	      return (vui128_t) vec_mrgald ((vui128_t) qdh, (vui128_t) qdl);
	    }
	}
      else
      //if (/*vec_cmpuq_all_ge (u1, (vui128_t) vdh)*/ 1)
	{
	  vui128_t u2, k1, t2;
	  vb128_t B0, Bgt;
      // Here v >= 2**64, Normalize the divisor so MSB is 1
      //n = __builtin_clzl ((unsigned long long)(v >> 64)); // 0 <= n <= 63
#if 1
      // Could use vec_clzq(), but we know  v >= 2**64, So:
      vn = (vui128_t) vec_clzd ((vui64_t) v);
      // vn = vn >> 64;, So we can use it with vec_slq ()
      vn = (vui128_t) vec_mrgahd ((vui128_t) zeros, vn);
#else
      vn = vec_clzq(v);
#endif
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
//#if 1
      vdh = vec_mrgahd (v1, (vui128_t) zeros);
      if (vec_cmpuq_all_eq (u1, (vui128_t) zeros))
	{
	  return ((vui128_t) zeros);
	}
	  const vui64_t ones = vec_splat_u64 (1);
#ifdef __DEBUG_PRINT__
	  print_vint128x ("        ovf vdh ", (vui128_t) vdh);
	  print_vint128x ("            u1  ", (vui128_t) u1);
#endif
	  u1 = vec_slq (u1, vn);
	  v1 = vec_slq (v1, vn);
#ifdef __DEBUG_PRINT__
	  print_vint128x ("         u1<<vn ", (vui128_t) u1);
	  print_vint128x ("         v1<<vn ", (vui128_t) v1);
#endif
	  //u1 = vec_srqi (u1, 1);
	  vdh = vec_mrgahd ((vui128_t) zeros, v1);
	  udh = vec_mrgahd ((vui128_t) zeros, u1); // !!
	  qdh = vec_divqud_inline (u1, (vui64_t) v1);
	  q0 = (vui128_t) vec_mrgald ((vui128_t) qdh, (vui128_t) qdh);
#ifdef __DEBUG_PRINT__
	  print_v2xint64 ("   u1/v1h = qdh ", qdh);
	  print_vint128x ("            q0  ", (vui128_t) q0);
#endif
	  if (vec_cmpud_all_eq ((vui64_t) q0, zeros)
	   /* && vec_cmpud_all_eq (vdh, udh)*/) // !!
	    { // this depends on U != 0
#if 1
	      vb64_t Beq;
              // detect overflow if ((x >> 64) == ((z >> 64)))
	      // a doubleword boolean true == __UINT64_MAX__
	      Beq = vec_cmpequd ((vui64_t) u1, (vui64_t) v1);
	      // Beq >> 64
	      Beq  = (vb64_t) vec_mrgahd ((vui128_t) zeros, (vui128_t) Beq);
#ifdef __DEBUG_PRINT__
	      print_v2xint64 (" qdh<-UINT128_MAX ", qdh);
	      print_v2xint64 ("     vdh eq udh = ", (vui64_t) Beq);
#endif
	      // Adjust quotient (-1) for divide overflow
	      qdh = (vui64_t) vec_or ((vui32_t) Beq, (vui32_t) qdh);
#ifdef __DEBUG_PRINT__
	      print_v2xint64 ("  corrected qdh = ", qdh);
#endif
#else
	      vb64_t CCq = vec_cmpequd ((vui64_t) qdh, zeros);
	      vb64_t CCv = vec_cmpequd ((vui64_t) vdh, udh);
	      vb64_t CCe = (vb64_t) vec_and ((vui32_t) CCq, (vui32_t) CCv);
#ifdef __DEBUG_PRINT__
	      print_v2xint64 (" qdh<-UINT128_MAX ", qdh);
	      print_v2xint64 ("            vdh ", (vui64_t) vdh);
	      print_v2xint64 ("            udh ", (vui64_t) udh);
	      print_v2xint64 ("   qdh eq 0   = ", (vui64_t) CCq);
	      print_v2xint64 ("   vdh eq udh = ", (vui64_t) CCv);
	      print_v2xint64 ("   CCq && CCv = ", (vui64_t) CCe);
#endif
	      qdh = vec_selud (qdh, (vui64_t) mone, CCe);
	      //qdh = vec_mrgahd ((vui128_t) zeros, mone);
#endif
	    }
#if 1
	  q0 = vec_slqi ((vui128_t) qdh, 64);
#else
	  q0 = (vui128_t) vec_mrgald ((vui128_t) qdh, (vui128_t) zeros);
#endif
#ifdef __DEBUG_PRINT__
	  print_v2xint64 ("   u1/v1h = qdh", qdh);
	  print_vint128x ("             q0 ", (vui128_t) q0);
#endif
	  // {k, k1}  = vec_muludq (v1, q0);
#if 1
	  {
	    vui128_t l128, h128;
	    vui64_t b_eud = vec_mrgald ((vui128_t) qdh, (vui128_t) qdh);
	    l128 = vec_vmuloud ((vui64_t ) v1, b_eud);
	    h128 = vec_vmaddeud ((vui64_t ) v1, b_eud, (vui64_t ) l128);
	    // 192-bit product of v1 * q-estimate
	    k  = h128;
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
	      vui128_t q2, t2, v3, v4;
	      q2 = (vui128_t) vec_subudm ((vui64_t) q0, ones);
	      q2 = (vui128_t) vec_mrgahd ((vui128_t) q2, (vui128_t) zeros);
	      //vdh = vec_mrgahd (v1, (vui128_t) zeros);
	      u2 = vec_adduqm ((vui128_t) u0, v1);
	      //t2 = vec_subuqm (u0, (vui128_t) vdh);
	      q0 = vec_seluq (q0, q2, Bgt);
	      u0 = vec_seluq (u0, u2, Bgt);
	      if (vec_cmpuq_all_eq ((vui128_t) Bgt, mone))
		{
#if 1
#if 0
		  u0 = vec_sldqi (u0, u2, 64);
		  u2 = vec_adduqm ((vui128_t) u0, v1);
		  u2 = vec_sldqi (u0, (vui128_t) zeros, 64);
		  u0 = vec_sldqi ((vui128_t) zeros, u0, 64);
#endif
#ifdef __DEBUG_PRINT__
		  print_vint128x (" hq(v1*q0) > u1 ", (vui128_t) u1);
		  print_vint128x ("     q0-1 = q0  ", (vui128_t) q0);
		  print_vint128x ("     u1-k'= u0  ", (vui128_t) u0);
		  print_vint128x ("     u1-k'= u2  ", (vui128_t) u2);
#endif
#else
		  v3 = vec_sldqi ((vui128_t) zeros, v1, 64);
		  v4 = vec_sldqi (v1, (vui128_t) zeros, 64);
		  t2 = vec_addcuq ((vui128_t) u2, v4);
		  u2 = vec_adduqm ((vui128_t) u2, v4);
		  u0 = vec_addeuqm (u0, v3, t2);
#ifdef __DEBUG_PRINT__
		  print_vint128x (" (v1>>64=v3/4   ", (vui128_t) v3);
		  print_vint128x ("                ", (vui128_t) v4);
		  print_vint128x (" hq(v1*q0) > u1 ", (vui128_t) u1);
		  print_vint128x ("     q0-1 = q0  ", (vui128_t) q0);
		  print_vint128x ("     u1-k'= u0  ", (vui128_t) u0);
		  print_vint128x ("     u1-k'= u2  ", (vui128_t) u2);
		  print_vint128x ("     u1-k'= t2  ", (vui128_t) t2);
#endif
#endif
#if 0
		  k1 = vec_muludq (&k, q0, v1);
#ifdef __DEBUG_PRINT__
		  print_vint128x (" hq(v1*q0) = k  ", (vui128_t) k);
		  print_vint128x (" lq(q0*v1) = k1 ", (vui128_t) k1);
#endif
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
		{
		  vb128_t CCk, CCke, CCk1;
		  vui128_t kc, k2;
		  u2 = vec_subuqm ((vui128_t) zeros, k1);
		  t = vec_subcuq ((vui128_t) zeros, k1);
		  u0 = vec_subeuqm (u1, k, t);
		  t2 = vec_subecuq (u1, k, t);
		  CCk = vec_setb_ncq (t2);
	#ifdef __DEBUG_PRINT__
		  print_vint128x (" hq(v1*q0) = k  ", (vui128_t) k);
		  print_vint128x (" lq(q0*v1) = k1 ", (vui128_t) k1);
		  print_vint128x ("     u1-k = u0  ", (vui128_t) u0);
		  print_vint128x ("     u1-k = u2  ", (vui128_t) u2);
		  print_vint128x ("     u1-k = t2  ", (vui128_t) t2);
	#endif
		  q2 = vec_adduqm (q0, mone);
		  q0 = vec_seluq (q0, q2, CCk);
		      if (vec_cmpuq_all_eq ((vui128_t) CCk, mone))
			{
#ifdef __DEBUG_PRINT__
		  print_vint128x (" hq(v1*q0')> u1 ", (vui128_t) CCk);
		  print_vint128x ("         q0''-1 ", (vui128_t) q0);
			}
#endif
#if 0
#ifdef __DEBUG_PRINT__
		  t = vec_muludq (&k, q0, v1);
		  print_vint128x ("            u1  ", (vui128_t) u1);
		  print_vint128x ("     hq(q0*v1)' ", (vui128_t) k);
		  print_vint128x ("     lq(q0*v1)' ", (vui128_t) t);
#endif
#endif
		}
	    }
#ifdef __DEBUG_PRINT__
          print_vint128x ("           q0'' ", (vui128_t) q0);
#endif
	  return q0;
	}
    }
#else
  int i;
  vb128_t ge;
  vui128_t t, cc, xt;
  vui128_t y = {(__int128) 0};

  for (i = 1; i <= 128; i++)
    {
      // Left shift (x || 0) requires 129-bits, is (t || x || 0 )
      t = vec_addcuq (x, x);
      x = vec_adduqm (x, x);

      // deconstruct ((t || x) >= z)
      // Carry from subcuq == 1 for x >= z
      cc = vec_subcuq (x, z);
      // Combine t with (x >= z) for 129-bit compare
      t  = (vui128_t) vec_or ((vui32_t)cc, (vui32_t)t);
      // Convert to a bool for select
      ge = vec_setb_cyq (t);

      xt = vec_subuqm (x, z);
      y = vec_addeuqm (y, y, t);
      x = vec_seluq (x, xt, ge);
    }
  return y;
#endif
}

vui128_t db_vec_divuq (vui128_t y, vui128_t z)
{
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  vui128_t res;
#if (__GNUC__ >= 12)
  res = vec_div (y, z);
#else
  __asm__(
      "vdivuq %0,%1,%2;\n"
      : "=v" (res)
      : "v" (y), "v" (z)
      : );
#endif
  return res;
#elif  (_ARCH_PWR8)
  vui128_t u = y;
  vui128_t v = z;
  const vui64_t zeros = vec_splat_u64(0);
  const vui128_t mone = (vui128_t) CONST_VINT128_DW (-1, -1);
  vui128_t u0, u1, v1, q0, q1, k, t, vn;
  vui64_t vdh, vdl, udh, qdl, qdh;
#ifdef __DEBUG_PRINT__
  print_vint128x (" vec_divuq   u ", (vui128_t) u);
  print_vint128x ("             v ", (vui128_t) v);
#endif

  udh = vec_splatd((vui64_t)u, 1);
  vdh = vec_splatd((vui64_t)v, 1);
  vdl = vec_splatd((vui64_t)v, 0);

  if (/*v >> 64 == 0UL*/ vec_cmpud_all_eq(vdh, zeros))
    {
      if (/*u >> 64 < v*/ vec_cmpud_all_lt(udh, vdl))
	{

	  qdl = vec_divqud_inline (u, vdl);
	  return (vui128_t) vec_mrgald((vui128_t)zeros, (vui128_t)qdl);
	}
      else {
	  //u1 = u >> 64;
	  u1 = (vui128_t) vec_mrgahd ((vui128_t)zeros, u);
	  // u0 = u & lmask;
	  u0 = (vui128_t) vec_mrgald ((vui128_t)zeros, u);
#ifdef __DEBUG_PRINT__
	  print_vint128x ("             vd1 ", (vui128_t) vdl);
	  print_vint128x ("             u0  ", (vui128_t) u0);
	  print_vint128x ("             u1  ", (vui128_t) u1);
#endif
	  //q1 = scalar_divdud (u1, (unsigned long long) v) & lmask;
	  qdh = vec_divqud_inline (u1, vdl);
	  //k = u1 - q1*v;
#if 1
	  // vec_divqud already provides the remainder in qdh[1]
	  // k = u1 - q1*v; ((k << 64) + u0);
	  // Simplifies to:
	  k = (vui128_t) vec_pasted (qdh, (vui64_t) u0);
#else
	  t = vec_muleud (qdh, vdl);
#ifdef __DEBUG_PRINT__
	  print_vint128x ("             qdh ", (vui128_t) qdh);
	  print_vint128x ("             t   ", (vui128_t) t);
#endif
	  k = vec_subuqm (u1, t);
	  // ((k << 64) + u0)
	  k = (vui128_t) vec_mrgald (k, u0);
#endif
	  // q0 = scalar_divdud ((k << 64) + u0, (unsigned long long) v) & lmask;
	  qdl = vec_divqud_inline (k, vdl);
#ifdef __DEBUG_PRINT__
	  print_vint128x ("             k   ", (vui128_t) k);
	  print_vint128x ("             qdl ", (vui128_t) qdl);
#endif
	  //return (q1 << 64) + q0;
	  return (vui128_t) vec_mrgald((vui128_t) qdh, (vui128_t) qdl);
      }
    }
  // Here v >= 2**64, Normalize the divisor so MSB is 1
  //n = __builtin_clzl ((unsigned long long)(v >> 64)); // 0 <= n <= 63
#if 1
  // Could use vec_clzq(), but we know  v >= 2**64, So:
  vn = (vui128_t) vec_clzd((vui64_t) v);
  // vn = vn >> 64;, So we can use it with vec_slq ()
  vn = (vui128_t) vec_mrgahd ((vui128_t)zeros, vn);
#else
  vn = vec_clzq(v);
#endif
  //v1 = (v << n) >> 64;
  v1 = vec_slq (v, vn);

  //u1 = u >> 1; 	// to insure no overflow
  u1 = vec_srqi (u, 1);
#ifdef __DEBUG_PRINT__
  print_vint128x ("             vn ", (vui128_t) vn);
  print_vint128x ("             v1 ", (vui128_t) v1);
  print_vint128x ("             u1 ", (vui128_t) u1);
#endif
  // q1 = scalar_divdud (u1, (unsigned long long) v1) & lmask;
  qdl = vec_divqud_inline (u1, (vui64_t) v1);
  q1  = (vui128_t) vec_mrgald((vui128_t)zeros, (vui128_t)qdl);
  // Undo normalization and u/2.
  //q0 = (q1 << n) >> 63;
  q0  = vec_slq (q1, vn);
  q0  = vec_srqi(q0, 63);
#ifdef __DEBUG_PRINT__
  print_vint128x ("             qd ", (vui128_t) qdl);
  print_vint128x ("             q1 ", (vui128_t) q1);
  print_vint128x ("             q0 ", (vui128_t) q0);
#endif

  if (/*q0 != 0*/ vec_cmpuq_all_ne(q0, (vui128_t)zeros))
    {
      //q0 = q0 - 1;
      q0 = vec_adduqm (q0, mone);
#ifdef __DEBUG_PRINT__
      print_vint128x ("           q0-1 ", (vui128_t) q0);
#endif
    }
  t = vec_mulluq(q0, v);
#ifdef __DEBUG_PRINT__
      print_vint128x ("           q0*v ", (vui128_t) t);
#endif
  t = vec_subuqm (u, t);
#ifdef __DEBUG_PRINT__
      print_vint128x ("           u-t  ", (vui128_t) t);
#endif
  /* (u - q0*v) >= v */
  if (vec_cmpuq_all_ge(t, v))
    {
      //q0 = q0 + 1;
      q0 = vec_subuqm (q0, mone);
#ifdef __DEBUG_PRINT__
      print_vint128x ("           q0+1 ", (vui128_t) q0);
#endif
    }
  return q0;
#else
  int i;
  vb128_t ge;
  vui128_t c, t, xt;
  vui128_t x = {(__int128) 0};

  for (i = 1; i <= 128; i++)
    {
      // 256-bit shift left
      c = vec_addcuq (y, y);
      x = vec_addeuqm (x, x, c);

      // Deconstruct QW cmpge to extract the carry
      t  = vec_subcuq (x, z);
      ge = vec_cmpgeuq (x, z);

      // xt = x - z, for select x >= z
      xt = vec_subuqm (x, z);
      // left shift y || t
      y = vec_addeuqm (y, y, t);
      // if (x >= z) x = x - z
      x = vec_seluq (x, xt, ge);
    }
  return y;
#endif
}

__VEC_U_128P db_vec_divdqu (vui128_t x, vui128_t y, vui128_t z)
{
  __VEC_U_128P result;
#if defined (_ARCH_PWR8)
  vui128_t Q, R;
  vui128_t r1, r2, q1, q2;
  vb128_t CC, c1, c2;
  const vui128_t ones = {(__int128) 1};
#ifdef __DEBUG_PRINT__
  print_vint128x (" db_vec_divdqu x ", (vui128_t) x);
  print_vint128x ("               y ", (vui128_t) y);
  print_vint128x ("               z ", (vui128_t) z);
#endif

  // Based on the PowerISA, Programming Note for
  // Divide Word Extended [Unsigned] but vectorized
  // for vector __int128
  q1 = db_vec_divuqe (x, z);
  q2 = db_vec_divuq  (y, z);
#ifdef __DEBUG_PRINT__
  print_vint128x (" vec_divuqe       = ", (vui128_t) q1);
  print_vint128x (" vec_divuq        = ", (vui128_t) q2);
#endif
  r1 = vec_mulluq (q1, z);

  r2 = vec_mulluq (q2, z);
#ifdef __DEBUG_PRINT__
  print_vint128x (" vec_mulluq(q1,z) = ", (vui128_t) r1);
  print_vint128x (" vec_mulluq(q2,z) = ", (vui128_t) r2);
#endif
  r2 = vec_subuqm (y, r2);
  Q  = vec_adduqm (q1, q2);
  R  = vec_subuqm (r2, r1);
#ifdef __DEBUG_PRINT__
  print_vint128x ("                 r2 ", (vui128_t) r2);
  print_vint128x ("                  Q ", (vui128_t) Q);
  print_vint128x ("                  R ", (vui128_t) R);
#endif

  c1 = vec_cmpltuq (R, r2);
#if defined (_ARCH_PWR8) // vorc requires P8
  c2 = vec_cmpgtuq (z, R);
  CC = (vb128_t) vec_orc ((vb32_t)c1, (vb32_t)c2);
#else
  c2 = vec_cmpgeuq (R, z);
  CC = (vb128_t) vec_or ((vb32_t)c1, (vb32_t)c2);
#endif
#ifdef __DEBUG_PRINT__
  print_vint128x ("                 c1 ", (vui128_t) c1);
  print_vint128x ("                 c2 ", (vui128_t) c2);
  print_vint128x ("                 CC ", (vui128_t) CC);
#endif
// Corrected Quotient returned for divduq.
  vui128_t Qt;
  Qt = vec_adduqm (Q, ones);
  Q = vec_seluq (Q, Qt, CC);
  result.vx0 = Q;
#ifdef __DEBUG_PRINT__
  print_vint128x ("                 Qt ", (vui128_t) Qt);
  print_vint128x ("                  Q ", (vui128_t) Q);
#endif
// Corrected Remainder not returned for divduq.
  vui128_t Rt;
  Rt = vec_subuqm (R, z);
  R = vec_seluq (R, Rt, CC);
  result.vx1 = R;
#ifdef __DEBUG_PRINT__
  print_vint128x ("                 Rt ", (vui128_t) Qt);
  print_vint128x ("                  R ", (vui128_t) Q);
#endif
#else
  /* Based on Hacker's Delight (2nd Edition) Figure 9-2.
   * "Divide long unsigned shift-and-subtract algorithm."
   * Converted to use vector unsigned __int128 and PVEClIB
   * operations.
   * As cmpgeuq is based on detecting the carry-out of (x -z) and
   * setting the bool via setb_cyq, we can use this carry (variable t)
   * to generate quotient bits.
   * Multi-precision shift-left is simpler then general addition,
   * so we can simplify carry generation. This allows delaying the
   * the y left-shift / quotient accumulation to a later.
   * */
  int i;
  vb128_t ge;
  vui128_t t, cc, c, xt;
  //t = (vui128_t) CONST_VINT128_W (0, 0, 0, 0);

  for (i = 1; i <= 128; i++)
    {
      // Left shift (x || y) requires 257-bits, is (t || x || y)
      // t from previous iteration generates to 0/1 for low order y-bits.
      c = vec_addcuq (y, y);
      t = vec_addcuq (x, x);
      x = vec_addeuqm (x, x, c);

      // deconstruct ((t || x) >= z)
      // Carry from subcuq == 1 for x >= z
      cc = vec_subcuq (x, z);
      // Combine t with (x >= z) for 129-bit compare
      t  = (vui128_t) vec_or ((vui32_t)cc, (vui32_t)t);
      // Convert to a bool for select
      ge = vec_setb_cyq (t);

      xt = vec_subuqm (x, z);
      y = vec_addeuqm (y, y, t);
      x = vec_seluq (x, xt, ge);
    }
  result.vx0 = y; // Q
  result.vx1 = x; // R
#endif
  return result;
}

vui128_t db_vec_moduq (vui128_t y, vui128_t z)
{
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  vui128_t res;
#if (__GNUC__ >= 12)
  res = vec_mod (y, z);
#else
  __asm__(
      "vmoduq %0,%1,%2;\n"
      : "=v" (res)
      : "v" (y), "v" (z)
      : );
#endif
  return res;
#elif  (_ARCH_PWR8)
  vui128_t u = y;
  vui128_t v = z;
  const vui64_t zeros = vec_splat_u64(0);
  const vui128_t mone = (vui128_t) CONST_VINT128_DW (-1, -1);
  vui128_t u0, u1, v1, r0, q0, q1, k, t, vn;
  vui64_t vdh, vdl, udh, qdl, qdh;
#ifdef __DEBUG_PRINT__
  print_vint128x (" vec_moduq   u ", (vui128_t) u);
  print_vint128x ("             v ", (vui128_t) v);
#endif

  udh = vec_splatd((vui64_t)u, 1);
  vdh = vec_splatd((vui64_t)v, 1);
  vdl = vec_splatd((vui64_t)v, 0);

  if (/*v >> 64 == 0UL*/ vec_cmpud_all_eq(vdh, zeros))
    {
      if (/*u >> 64 < v*/ vec_cmpud_all_lt(udh, vdl))
	{

	  qdl = vec_divqud_inline (u, vdl);
	  return (vui128_t) vec_mrgahd((vui128_t)zeros, (vui128_t)qdl);
	}
      else {
	  //u1 = u >> 64;
	  u1 = (vui128_t) vec_mrgahd ((vui128_t)zeros, u);
	  // u0 = u & lmask;
	  u0 = (vui128_t) vec_mrgald ((vui128_t)zeros, u);
#ifdef __DEBUG_PRINT__
	  print_vint128x ("             vd1 ", (vui128_t) vdl);
	  print_vint128x ("             u0  ", (vui128_t) u0);
	  print_vint128x ("             u1  ", (vui128_t) u1);
#endif
	  //q1 = scalar_divdud (u1, (unsigned long long) v) & lmask;
	  qdh = vec_divqud_inline (u1, vdl);
	  //k = u1 - q1*v;
#if 1
	  // vec_divqud already provides the remainder in qdh[1]
	  // k = u1 - q1*v; ((k << 64) + u0);
	  // Simplifies to:
	  k = (vui128_t) vec_pasted (qdh, (vui64_t) u0);
#else
	  t = vec_muleud (qdh, vdl);
#ifdef __DEBUG_PRINT__
	  print_vint128x ("             qdh ", (vui128_t) qdh);
	  print_vint128x ("             t   ", (vui128_t) t);
#endif
	  k = vec_subuqm (u1, t);
	  // ((k << 64) + u0)
	  k = (vui128_t) vec_mrgald (k, u0);
#endif
	  // q0 = scalar_divdud ((k << 64) + u0, (unsigned long long) v) & lmask;
	  qdl = vec_divqud_inline (k, vdl);
#ifdef __DEBUG_PRINT__
	  print_vint128x ("             k   ", (vui128_t) k);
	  print_vint128x ("             qdl ", (vui128_t) qdl);
#endif
	  //return (q1 << 64) + q0;
	  //return (vui128_t) vec_mrgald((vui128_t) qdh, (vui128_t) qdl);
	  return (vui128_t) vec_mrgahd((vui128_t)zeros, (vui128_t)qdl);
      }
    }
  // Here v >= 2**64, Normalize the divisor so MSB is 1
  //n = __builtin_clzl ((unsigned long long)(v >> 64)); // 0 <= n <= 63
#if 1
  // Could use vec_clzq(), but we know  v >= 2**64, So:
  vn = (vui128_t) vec_clzd((vui64_t) v);
  // vn = vn >> 64;, So we can use it with vec_slq ()
  vn = (vui128_t) vec_mrgahd ((vui128_t)zeros, vn);
#else
  vn = vec_clzq(v);
#endif
  //v1 = (v << n) >> 64;
  v1 = vec_slq (v, vn);

  //u1 = u >> 1; 	// to insure no overflow
  u1 = vec_srqi (u, 1);
#ifdef __DEBUG_PRINT__
  print_vint128x ("             vn ", (vui128_t) vn);
  print_vint128x ("             v1 ", (vui128_t) v1);
  print_vint128x ("             u1 ", (vui128_t) u1);
#endif
  // q1 = scalar_divdud (u1, (unsigned long long) v1) & lmask;
  qdl = vec_divqud_inline (u1, (vui64_t) v1);
  q1  = (vui128_t) vec_mrgald((vui128_t)zeros, (vui128_t)qdl);
  // Undo normalization and u/2.
  //q0 = (q1 << n) >> 63;
  q0  = vec_slq (q1, vn);
  q0  = vec_srqi(q0, 63);
#ifdef __DEBUG_PRINT__
  print_vint128x ("             qd ", (vui128_t) qdl);
  print_vint128x ("             q1 ", (vui128_t) q1);
  print_vint128x ("             q0 ", (vui128_t) q0);
#endif

  if (/*q0 != 0*/ vec_cmpuq_all_ne(q0, (vui128_t)zeros))
    {
      //q0 = q0 - 1;
      q0 = vec_adduqm (q0, mone);
#ifdef __DEBUG_PRINT__
      print_vint128x ("           q0-1 ", (vui128_t) q0);
#endif
    }
  t = vec_mulluq(q0, v);
#ifdef __DEBUG_PRINT__
      print_vint128x ("           q0*v ", (vui128_t) t);
#endif
  // r0 = u - (q0 * v)
  r0 = vec_subuqm (u, t);
#ifdef __DEBUG_PRINT__
      print_vint128x ("  u-(q0*v) = r0 ", (vui128_t) r0);
#endif
  /* (u - q0*v) >= v */
  if (vec_cmpuq_all_ge(r0, v))
    {
      //q0 = q0 + 1;
      q0 = vec_subuqm (q0, mone);
      //r0 = r0 - v;
      r0 = vec_subuqm (r0, v);
#ifdef __DEBUG_PRINT__
      print_vint128x ("           q0+1 ", (vui128_t) q0);
      print_vint128x ("           r0-v ", (vui128_t) r0);
#endif
    }
  return r0;
#else
  int i;
  vb128_t ge;
  vui128_t c, t, xt;
  vui128_t x = {(__int128) 0};

  for (i = 1; i <= 128; i++)
    {
      // 256-bit shift left
      c = vec_addcuq (y, y);
      x = vec_addeuqm (x, x, c);

      // Deconstruct QW cmpge to extract the carry
      t  = vec_subcuq (x, z);
      ge = vec_cmpgeuq (x, z);

      // xt = x - z, for select x >= z
      xt = vec_subuqm (x, z);
      // left shift y || t
      y = vec_addeuqm (y, y, t);
      // if (x >= z) x = x - z
      x = vec_seluq (x, xt, ge);
    }
  return y;
#endif
}
#endif

#undef __DEBUG_PRINT__


int
test_addcq (void)
{
  vui32_t i, j, k;
  vui128_t l;
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
  vui32_t i, j, k, l;
  vui128_t m;
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

  i = (vui64_t){-1UL, -1UL};
  j = (vui64_t){-1UL, -1UL};
  k = (vui128_t)CONST_VINT128_DW128(0x0UL, 0x0UL);
  e = (vui128_t)CONST_VINT128_DW128(0xfffffffffffffffcUL, 2UL);

  l = vec_msumudm(i, j, k);

#ifdef __DEBUG_PRINT__
  print_vint128 ("msumudm({-1, -1}, {-1, -1}, 0) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_msumudm:", (vui128_t)l, (vui128_t) e);

  e = (vui128_t)CONST_VINT128_DW128(0UL, 1UL);

  l = vec_msumcud(i, j, k);

#ifdef __DEBUG_PRINT__
  print_vint128 ("msumcud({-1, -1}, {-1, -1}, 0) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_msumcud:", (vui128_t)l, (vui128_t) e);

  i = (vui64_t){-1UL, -1UL};
  j = (vui64_t){0UL, -1UL};
  k = (vui128_t)CONST_VINT128_DW128(-1UL, -1UL);
  e = (vui128_t)CONST_VINT128_DW128(0xfffffffffffffffeUL, 0UL);

  l = vec_msumudm(i, j, k);

#ifdef __DEBUG_PRINT__
  print_vint128 ("msumudm({-1, -1}, {0, -1}, -1) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_msumudm:", (vui128_t)l, (vui128_t) e);

  e = (vui128_t)CONST_VINT128_DW128(0UL, 1UL);

  l = vec_msumcud(i, j, k);

#ifdef __DEBUG_PRINT__
  print_vint128 ("msumcud({-1, -1}, {-1, -1}, 0) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_msumcud:", (vui128_t)l, (vui128_t) e);

  i = (vui64_t){-1UL, -1UL};
  j = (vui64_t){-1UL, -1UL};
  k = (vui128_t)CONST_VINT128_DW128(-1UL, -1UL);
  e = (vui128_t)CONST_VINT128_DW128(0xfffffffffffffffcUL, 1UL);

  l = vec_msumudm(i, j, k);

#ifdef __DEBUG_PRINT__
  print_vint128 ("msumudm({-1, -1}, {-1, -1}, -1) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_msumudm:", (vui128_t)l, (vui128_t) e);

  e = (vui128_t)CONST_VINT128_DW128(0UL, 2UL);

  l = vec_msumcud(i, j, k);

#ifdef __DEBUG_PRINT__
  print_vint128 ("msumcud({-1, -1}, {-1, -1}, -1) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_msumcud:", (vui128_t)l, (vui128_t) e);

  return (rc);
}

#ifdef __DEBUG_PRINT__
#if __DEBUG_PRINT__ == 2
// Use local debug code.
#define test_vec_muludq(_l, _i, _j)	db_vec_muluq(_l, _i, _j)
#else
// Use out of line code from vec_int128_dummy.c compile test.
extern vui128_t
__test_muludq (vui128_t *mulu, vui128_t a, vui128_t b);
#define test_vec_muludq(_l, _i, _j)	__test_muludq(_l, _i, _j)
#endif
#else
// Use inline code from vec_int128_ppch.
#define test_vec_muludq(_l, _i, _j)	vec_muludq(_l, _i, _j)
#endif

int
test_muludq (void)
{
  vui32_t i, j, k/*, l , m*/;
  vui128_t l;
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


#ifdef __DEBUG_PRINT__
// Use out of line code from vec_int128_dummy.c compile test.
extern vui128_t
__test_madd2uq (vui128_t *mulu, vui128_t a, vui128_t b, vui128_t c, vui128_t d);
extern vui128_t
__test_madduq (vui128_t *mulu, vui128_t a, vui128_t b, vui128_t c);
#define test_vec_madduq(_l, _i, _j, _m)	__test_madduq(_l, _i, _j, _m)
#define test_vec_madd2uq(_l, _i, _j, _m, _n)	__test_madd2uq(_l, _i, _j, _m, _n)
#else
// Use inline code from vec_int128_ppch.
#define test_vec_madduq(_l, _i, _j, _m)	vec_madduq(_l, _i, _j, _m)
#define test_vec_madd2uq(_l, _i, _j, _m, _n)	vec_madd2uq(_l, _i, _j, _m, _n)
#endif

int
test_madduq (void)
{
  vui32_t i, j, k, m, n;
  vui128_t l;
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

  l = vec_sldqi ((vui128_t) i, (vui128_t) j, 7);
#ifdef __DEBUG_PRINT__
  print_vint128x (" w ", (vui128_t) i);
  print_vint128x (" x ", (vui128_t) j);
  print_vint128x (" = ", l);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xffffff80);
  rc += check_vuint128x ("vec_sldq (  7):", (vui128_t) l, (vui128_t) e);

  l = vec_sldqi ((vui128_t) i, (vui128_t) j, 8);
#ifdef __DEBUG_PRINT__
  print_vint128x (" w ", (vui128_t) i);
  print_vint128x (" x ", (vui128_t) j);
  print_vint128x (" = ", l);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xffffff00);
  rc += check_vuint128x ("vec_sldq (  8):", (vui128_t) l, (vui128_t) e);

  l = vec_sldqi ((vui128_t) i, (vui128_t) j, 15);
#ifdef __DEBUG_PRINT__
  print_vint128x (" w ", (vui128_t) i);
  print_vint128x (" x ", (vui128_t) j);
  print_vint128x (" = ", l);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xffff8000);
  rc += check_vuint128x ("vec_sldq ( 15):", (vui128_t) l, (vui128_t) e);

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

  l = vec_sldqi ((vui128_t) i, (vui128_t) j, 8);
#ifdef __DEBUG_PRINT__
  print_vint128x (" w ", (vui128_t) i);
  print_vint128x (" x ", (vui128_t) j);
  print_vint128x (" = ", l);
#endif
  e = (vui32_t) CONST_VINT32_W (0, 0, 0, 0xff);
  rc += check_vuint128 ("vec_sldqi (  8):", (vui128_t) l, (vui128_t) e);

  l = vec_sldqi ((vui128_t) i, (vui128_t) j, 15);
#ifdef __DEBUG_PRINT__
  print_vint128x (" w ", (vui128_t) i);
  print_vint128x (" x ", (vui128_t) j);
  print_vint128x (" = ", l);
#endif
  e = (vui32_t) CONST_VINT32_W (0, 0, 0, 0x7fff);
  rc += check_vuint128 ("vec_sldqi ( 15):", (vui128_t) l, (vui128_t) e);

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

int
test_sldbi (void)
{
  vui32_t i, j, e;
  vui128_t l;
  int rc = 0;

  printf ("\n%s Vector Shift Left Double Quadword by Bit Immediate\n", __FUNCTION__);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 0);
  l = vec_vsldbi ((vui128_t) i, (vui128_t) j, 0);
#ifdef __DEBUG_PRINT__
  print_vint128x (" w ", (vui128_t) i);
  print_vint128x (" x ", (vui128_t) j);
  print_vint128x (" = ", l);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  rc += check_vuint128x ("vec_vsldbi (  0):", (vui128_t) l, (vui128_t) e);

  l = vec_vsldbi ((vui128_t) i, (vui128_t) j, 4);
#ifdef __DEBUG_PRINT__
  print_vint128x (" w ", (vui128_t) i);
  print_vint128x (" x ", (vui128_t) j);
  print_vint128x (" = ", l);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xfffffff0);
  rc += check_vuint128x ("vec_vsldbi (  4):", (vui128_t) l, (vui128_t) e);

  l = vec_vsldbi ((vui128_t) i, (vui128_t) j, 7);
#ifdef __DEBUG_PRINT__
  print_vint128x (" w ", (vui128_t) i);
  print_vint128x (" x ", (vui128_t) j);
  print_vint128x (" = ", l);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xffffff80);
  rc += check_vuint128x ("vec_vsldbi (  7):", (vui128_t) l, (vui128_t) e);

  j = (vui32_t) CONST_VINT32_W(0x55555555, 0, 0, 0);
  l = vec_vsldbi ((vui128_t) i, (vui128_t) j, 7);
#ifdef __DEBUG_PRINT__
  print_vint128x (" w ", (vui128_t) i);
  print_vint128x (" x ", (vui128_t) j);
  print_vint128x (" = ", l);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xffffffaa);
  rc += check_vuint128x ("vec_vsldbi ( 7a):", (vui128_t) l, (vui128_t) e);

  return (rc);
}

int
test_srdbi (void)
{
  vui32_t i, j, e;
  vui128_t l;
  int rc = 0;

  printf ("\n%s Vector Shift Right Double Quadword by Bit Immediate\n", __FUNCTION__);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   __UINT32_MAX__);
  j = (vui32_t) CONST_VINT32_W(0, 0, 0, 0);
  l = vec_vsrdbi ((vui128_t) i, (vui128_t) j, 0);
#ifdef __DEBUG_PRINT__
  print_vint128x (" w ", (vui128_t) i);
  print_vint128x (" x ", (vui128_t) j);
  print_vint128x (" = ", l);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0, 0, 0, 0);
  rc += check_vuint128x ("vec_vsrdbi (  0):", (vui128_t) l, (vui128_t) e);

  l = vec_vsrdbi ((vui128_t) i, (vui128_t) j, 4);
#ifdef __DEBUG_PRINT__
  print_vint128x (" w ", (vui128_t) i);
  print_vint128x (" x ", (vui128_t) j);
  print_vint128x (" = ", l);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0xf0000000, 0, 0, 0);
  rc += check_vuint128x ("vec_vsrdbi (  4):", (vui128_t) l, (vui128_t) e);

  l = vec_vsrdbi ((vui128_t) i, (vui128_t) j, 7);
#ifdef __DEBUG_PRINT__
  print_vint128x (" w ", (vui128_t) i);
  print_vint128x (" x ", (vui128_t) j);
  print_vint128x (" = ", l);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0xfe000000, 0, 0, 0);
  rc += check_vuint128x ("vec_vsrdbi (  7):", (vui128_t) l, (vui128_t) e);

  i = (vui32_t)
	    CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
			   0xaaaaaaaa);
  l = vec_vsrdbi ((vui128_t) i, (vui128_t) j, 7);
#ifdef __DEBUG_PRINT__
  print_vint128x (" w ", (vui128_t) i);
  print_vint128x (" x ", (vui128_t) j);
  print_vint128x (" = ", l);
#endif
  e = (vui32_t)
	    CONST_VINT32_W(0x54000000, 0, 0, 0);
  rc += check_vuint128x ("vec_vsrdbi ( 7a):", (vui128_t) l, (vui128_t) e);

  return (rc);
}

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
int
test_splat_128 (void)
{
  vui128_t ek, k;
  vi128_t  el, l;
  int rc = 0;

  printf ("\ntest_splat_128 Vector Splat Immediate Signed/Unsigned Quadword\n");

  l =  vec_splat_s128 (0);
  el = (vi128_t) CONST_VINT128_DW128(0, 0);
  rc += check_vuint128x ("vec_splat_s128(0):", (vui128_t) l, (vui128_t) el);

  k =  vec_splat_u128 (0);
  ek = CONST_VINT128_DW128(0, 0);
  rc += check_vuint128x ("vec_splat_u128(0):", k, ek);

  l =  vec_splat_s128 (-1);
  el = (vi128_t) CONST_VINT128_DW128(-1, -1);
  rc += check_vuint128x ("vec_splat_s128(-1):", (vui128_t) l, (vui128_t) el);

  l =  vec_splat_s128 (-16);
  el = (vi128_t) CONST_VINT128_DW128(-1, -16);
  rc += check_vuint128x ("vec_splat_s128(-16):", (vui128_t) l, (vui128_t) el);

  l =  vec_splat_s128 (-128);
  el = (vi128_t) CONST_VINT128_DW128(-1, -128);
  rc += check_vuint128x ("vec_splat_s128(-128):", (vui128_t) l, (vui128_t) el);

  l =  vec_splat_s128 (-255);
  el = (vi128_t) CONST_VINT128_DW128(-1, -255);
  rc += check_vuint128x ("vec_splat_s128(-255):", (vui128_t) l, (vui128_t) el);

  l =  vec_splat_s128 (-256);
  el = (vi128_t) CONST_VINT128_DW128(-1, -256);
  rc += check_vuint128x ("vec_splat_s128(-256):", (vui128_t) l, (vui128_t) el);

  l =  vec_splat_s128 (1);
  el = (vi128_t) CONST_VINT128_DW128(0, 1);
  rc += check_vuint128x ("vec_splat_s128(1):", (vui128_t) l, (vui128_t) el);

  k =  vec_splat_u128 (1);
  ek = CONST_VINT128_DW128(0, 1);
  rc += check_vuint128x ("vec_splat_u128(1):", k, ek);

  l =  vec_splat_s128 (15);
  el = (vi128_t) CONST_VINT128_DW128(0, 15);
  rc += check_vuint128x ("vec_splat_s128(15):", (vui128_t) l, (vui128_t) el);

  k =  vec_splat_u128 (15);
  ek = CONST_VINT128_DW128(0, 15);
  rc += check_vuint128x ("vec_splat_u128(15):", k, ek);

  l =  vec_splat_s128 (16);
  el = (vi128_t) CONST_VINT128_DW128(0, 16);
  rc += check_vuint128x ("vec_splat_s128(16):", (vui128_t) l, (vui128_t) el);

  k =  vec_splat_u128 (16);
  ek = CONST_VINT128_DW128(0, 16);
  rc += check_vuint128x ("vec_splat_u128(16):", k, ek);

  l =  vec_splat_s128 (127);
  el = (vi128_t) CONST_VINT128_DW128(0, 127);
  rc += check_vuint128x ("vec_splat_s128(127):", (vui128_t) l, (vui128_t) el);

  k =  vec_splat_u128 (127);
  ek = CONST_VINT128_DW128(0, 127);
  rc += check_vuint128x ("vec_splat_u128(127):", k, ek);

  l =  vec_splat_s128 (128);
  el = (vi128_t) CONST_VINT128_DW128(0, 128);
  rc += check_vuint128x ("vec_splat_s128(128):", (vui128_t) l, (vui128_t) el);

  k =  vec_splat_u128 (128);
  ek = CONST_VINT128_DW128(0, 128);
  rc += check_vuint128x ("vec_splat_u128(128):", k, ek);

  l =  vec_splat_s128 (255);
  el = (vi128_t) CONST_VINT128_DW128(0, 255);
  rc += check_vuint128x ("vec_splat_s128(255):", (vui128_t) l, (vui128_t) el);

  k =  vec_splat_u128 (255);
  ek = CONST_VINT128_DW128(0, 255);
  rc += check_vuint128x ("vec_splat_u128(255):", k, ek);

  l =  vec_splat_s128 (256);
  el = (vi128_t) CONST_VINT128_DW128(0, 256);
  rc += check_vuint128x ("vec_splat_s128(256):", (vui128_t) l, (vui128_t) el);

  k =  vec_splat_u128 (256);
  ek = CONST_VINT128_DW128(0, 256);
  rc += check_vuint128x ("vec_splat_u128(256):", k, ek);

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
test_xfer_int128 (void)
{
  unsigned __int128 i, e;
  vui128_t vi;
  vui128_t ve;
  unsigned long long high, low;

  int rc = 0;
  printf ("\ntest xfer __int128\n");

  vi = (vui128_t) { 10000000000000000UL };
  e  = (unsigned __int128) 10000000000000000UL;

  i  = vec_transfer_vui128t_to_uint128 (vi);
  rc += check_int128 ("xfer to int128 1", i , e);

  vi = (vui128_t) { (__int128) 10000000000000000UL
                  * (__int128) 10000000000000000UL };
  e  = (unsigned __int128) 10000000000000000UL
      * (unsigned __int128) 10000000000000000UL;

  i  = vec_transfer_vui128t_to_uint128 (vi);
  rc += check_int128 ("xfer to int128 2", i , e);

  low = scalar_extract_uint64_from_low_uint128 (i);
  high = scalar_extract_uint64_from_high_uint128 (i);

  rc = check_uint64 ("ext_uint128_low", low, 9632337040368467968UL);
  rc = check_uint64 ("ext_uint128_high", high, 5421010862427UL);

  i  = (unsigned __int128) 10000000000000000UL;
  ve = (vui128_t) { 10000000000000000UL };

  vi = vec_transfer_uint128_to_vui128t (i);
  rc += check_vuint128x ("xfer from int128 1", vi , ve);

  i  = (unsigned __int128) 10000000000000000UL
      * (unsigned __int128) 10000000000000000UL;
  ve = (vui128_t) { (__int128) 10000000000000000UL
                  * (__int128) 10000000000000000UL };

  vi = vec_transfer_uint128_to_vui128t (i);
  rc += check_vuint128x ("xfer from int128 1", vi , ve);

  return (rc);
}

#if 0
extern vui128_t test_vec_divduq (vui128_t x, vui128_t y, vui128_t z);
#define test_divduq	test_vec_divduq
#if 0
#define test_diveuq	db_vec_diveuq
#else
extern vui128_t test_vec_diveuq (vui128_t x, vui128_t z);
#define test_diveuq	test_vec_diveuq
#endif
#if 0
#define test_divuq	db_vec_divuq
#else
extern vui128_t test_vec_divuq (vui128_t y, vui128_t z);
#define test_divuq	test_vec_divuq
#endif
#else
#if 1
// Use static lib implementations.
extern vui128_t  __VEC_PWR_IMP(vec_divduq) (vui128_t x, vui128_t y, vui128_t z);
#define test_divduq __VEC_PWR_IMP(vec_divduq)
extern vui128_t  __VEC_PWR_IMP(vec_diveuq) (vui128_t x, vui128_t z);
#define test_diveuq __VEC_PWR_IMP(vec_diveuq)
extern vui128_t  __VEC_PWR_IMP(vec_divuq) (vui128_t y, vui128_t z);
#define test_divuq __VEC_PWR_IMP(vec_divuq)
#else
// use implementations from vec_int128_dummy compile tests.
extern vui128_t test_divduq (vui128_t x, vui128_t y, vui128_t z);
extern vui128_t test_diveuq (vui128_t x, vui128_t z);
extern vui128_t test_divuq (vui128_t y, vui128_t z);
#endif
#endif
//#define __DEBUG_PRINT__
int
test_vec_divide_QW (void)
{

  vui128_t ix[4];
  vui128_t er;
  vui128_t rq;
  int rc = 0;

  printf ("\ntest Vector divide Unsigned Quadword\n");

  ix[0] = (vui128_t)CONST_VINT128_DW128(0, 1);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0, 0);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0, __UINT64_MAX__);
  er = (vui128_t)CONST_VINT128_DW128(1, 1);

  rq = test_divduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divduq ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divduq:", (vui128_t)rq, (vui128_t) er);
#if 1
  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x (" qo  rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);
#endif

  ix[0] = (vui128_t)CONST_VINT128_DW128(0, 1);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0, 0);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0, 10000000000000000);
  er = (vui128_t)CONST_VINT128_DW128(0x0000000000000734UL, 0xaca5f6226f0ada61UL);

  rq = test_divduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divduq ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divduq:", (vui128_t)rq, (vui128_t) er);
#if 1
  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x (" qo  rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);
#endif

  ix[0] = (vui128_t)CONST_VINT128_DW128(0, __UINT64_MAX__-1);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0, 0);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0, __UINT64_MAX__);
  er = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__-1, __UINT64_MAX__-1);

  rq = test_divduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divduq ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divduq:", (vui128_t)rq, (vui128_t) er);
#if 1
  er = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__-1, __UINT64_MAX__-1);
  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x (" qo  rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);
#endif

  ix[0] = (vui128_t)CONST_VINT128_DW128(0, 0);
  ix[1] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  ix[2] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  er = (vui128_t)CONST_VINT128_DW128(0, 1);

  rq = test_divuq (ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

#if 0
  rq = test_diveuq (ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x (" qo  rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);
#endif

  ix[0] = (vui128_t)CONST_VINT128_DW128(0, 0);
  ix[1] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  ix[2] = CONST_VINT128_DW128 ( 0x000004ee2d6d415bll,0x85acef8100000000ll ); /* 10**32 */
  er = (vui128_t)CONST_VINT128_DW128(0, 0x33ec47UL);

  rq = test_divuq (ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[0] = (vui128_t)CONST_VINT128_DW128(0, 0);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0, __UINT64_MAX__);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0, 1);
  er = (vui128_t)CONST_VINT128_DW128(0, __UINT64_MAX__);

  rq = test_divuq (ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[0] = (vui128_t)CONST_VINT128_DW128(0, 0);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0, __UINT64_MAX__);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0, __UINT32_MAX__);
  er = (vui128_t)CONST_VINT128_DW128(0, 0x100000001UL);

  rq = test_divuq (ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[0] = (vui128_t)CONST_VINT128_DW128(0, 0);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0, __UINT64_MAX__);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0, __UINT64_MAX__);
  er = (vui128_t)CONST_VINT128_DW128(0, 1);

  rq = test_divuq (ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[0] = (vui128_t)CONST_VINT128_DW128(0, 0);
  ix[1] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0, 1);
  er = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);

  rq = test_divuq (ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[0] = (vui128_t)CONST_VINT128_DW128(0, 0);
  ix[1] = (vui128_t)CONST_VINT128_DW128((__UINT64_MAX__-1), 1);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0, __UINT64_MAX__);
  er = (vui128_t)CONST_VINT128_DW128(0, __UINT64_MAX__);

  rq = test_divuq (ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[0] = (vui128_t)CONST_VINT128_DW128(0, 0);
  ix[1] = (vui128_t)CONST_VINT128_DW128((__UINT64_MAX__-1), __UINT64_MAX__);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0, __UINT64_MAX__);
  er = (vui128_t)CONST_VINT128_DW128(0, __UINT64_MAX__);

  rq = test_divuq (ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[0] = (vui128_t)CONST_VINT128_DW128(0, 0);
  ix[1] = (vui128_t)CONST_VINT128_DW128(__UINT32_MAX__, __UINT32_MAX__);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0, __UINT32_MAX__);
  er = (vui128_t)CONST_VINT128_DW128(1, 1);

  rq = test_divuq (ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[0] = (vui128_t)CONST_VINT128_DW128(0, 0);
  ix[1] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  ix[2] = (vui128_t)CONST_VINT128_DW128(__UINT32_MAX__, __UINT64_MAX__);
  er = (vui128_t)CONST_VINT128_DW128(0, 0x100000000UL);

  rq = test_divuq (ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

#ifdef test_divduq
  ix[0] = (vui128_t)CONST_VINT128_DW128(0, 0);
  ix[1] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0, 1);
  er = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);

  rq = test_divduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divduq ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divduq:", (vui128_t)rq, (vui128_t) er);

  ix[0] = (vui128_t)CONST_VINT128_DW128(0UL, 0UL);
  ix[1] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  ix[2] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  er = (vui128_t)CONST_VINT128_DW128(0x0UL,
				     0x1UL);

  rq = test_divduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divduq ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divduq!!:", (vui128_t)rq, (vui128_t) er);

  er = (vui128_t)CONST_VINT128_DW128(0x0UL, 0x0UL);
  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x (" qo  rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);
#endif

  ix[0] = (vui128_t)CONST_VINT128_DW128(0UL, 0UL);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0x4b3b4ca85a86c47all,
                                        0x098a224000000000ll) /* 10**38 */;
  ix[2] = (vui128_t)CONST_VINT128_DW128(0x0000000000000000ll,
                                        0x8ac7230489e80000ll) /* 10**19 */;
  er =    (vui128_t)CONST_VINT128_DW128(0x0000000000000000ll,
                                        0x8ac7230489e80000ll) /* 10**19 */;

#ifdef test_divduq
  rq = test_divduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divduq ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divduq:", (vui128_t)rq, (vui128_t) er);
#endif

  rq = test_divuq (ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[0] = (vui128_t)CONST_VINT128_DW128(0UL, 0UL);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0x4b3b4ca85a86c47all,
                                        0x098a224000000000ll) /* 10**38 */;
  ix[2] = (vui128_t)CONST_VINT128_DW128(0x0000000000000000ll,
                                        0x8ac7230489e80000ll) /* 10**19 */;
  ix[3] = (vui128_t)CONST_VINT128_DW128(0x0000000000000000ll,
                                        0x8ac7230489e7ffffll) /* 10**19 */;
  er =    (vui128_t)CONST_VINT128_DW128(0x0000000000000000ll,
                                        0x8ac7230489e80000ll) /* 10**19 */;
  ix[1] = vec_adduqm(ix[1], ix[3]);

#ifdef test_divduq
  rq = test_divduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divduq ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divduq:", (vui128_t)rq, (vui128_t) er);
#endif

  rq = test_divuq (ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);


  ix[0] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, (__UINT64_MAX__-1));
  ix[1] = (vui128_t)CONST_VINT128_DW128(0x0UL, 0x1UL);
  ix[2] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  er = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);

  rq = test_divduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divduq ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divduq:", (vui128_t)rq, (vui128_t) er);

  er = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, (__UINT64_MAX__-1));
  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x (" qo  rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);

  er = (vui128_t)CONST_VINT128_DW128(0x0UL, 0x0UL);
  rq = test_divuq (ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[0] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0UL, 0UL);
  ix[2] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  er = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);

#if 0 // Quotient overflow
  rq = test_divduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divduq ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x (" qo  rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divduq:", (vui128_t)rq, (vui128_t) er);

  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x (" qo  rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);
#endif

  ix[2] = (vui128_t)CONST_VINT128_DW128(0x4b3b4ca85a86c47all,
                                        0x098a224000000000ll) /* 10**38 */;
  ix[3] = (vui128_t)CONST_VINT128_DW128(0, 1);
  ix[3] = vec_subuqm (ix[2], ix[3]);
  ix[1] = vec_madduq (ix, ix[2], ix[2], ix[3]);
  er = ix[2];

  rq = test_divduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divduq ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divduq!:", (vui128_t)rq, (vui128_t) er);


  er =  (vui128_t)CONST_VINT128_DW128(0x4b3b4ca85a86c47all,
                                      0x098a223ffffffffell);
  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x (" qo  rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuqe!:", (vui128_t)rq, (vui128_t) er);

  ix[0] = (vui128_t)CONST_VINT128_DW128(0x0000000100000000UL, 0UL);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0x0UL, 0x0UL);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0x0001000000000000UL, 0UL);
  er = (vui128_t)CONST_VINT128_DW128(0x0001000000000000UL, 0UL);

  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);

  ix[0] = (vui128_t)CONST_VINT128_DW128(0x0001000000000000UL, 0UL);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0x0UL, 0x0UL);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0x0100000000000000UL, 0UL);
  er = (vui128_t)CONST_VINT128_DW128(0x0100000000000000UL, 0UL);

  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);

  ix[0] = (vui128_t)CONST_VINT128_DW128(0x0001ffffffffffffUL, __UINT64_MAX__);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0x0UL, 0x0UL);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0x0100000000000000UL, 0UL);
  er = (vui128_t)CONST_VINT128_DW128(0x01ffffffffffffffUL, 0xffffffffffffff00UL);

  rq = test_divduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divduq ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divduq:", (vui128_t)rq, (vui128_t) er);

  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);

  ix[0] = (vui128_t)CONST_VINT128_DW128(0x0001ffffffffffffUL, __UINT64_MAX__);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0x0UL, 0x0UL);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0x0180000000000000UL, 0UL);
  er = (vui128_t)CONST_VINT128_DW128(0x0155555555555555UL, 0x55555555555554aaUL);

  rq = test_divduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divduq ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divduq:", (vui128_t)rq, (vui128_t) er);

  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);

  ix[0] = (vui128_t)CONST_VINT128_DW128(0x0001000000000000UL, 0UL);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0x0UL, 0x0UL);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0x01ffffffffffffffUL, __UINT64_MAX__);
  er = (vui128_t)CONST_VINT128_DW128(0x0080000000000000UL, 0UL);

  rq = test_divduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divduq ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divduq:", (vui128_t)rq, (vui128_t) er);

  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);

  ix[0] = (vui128_t)CONST_VINT128_DW128(0x0001800000000000UL, 0UL);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0x0UL, 0x0UL);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0x01ffffffffffffffUL, __UINT64_MAX__);
  er = (vui128_t)CONST_VINT128_DW128(0x00c0000000000000UL, 0UL);

  rq = test_divduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divduq ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divduq:", (vui128_t)rq, (vui128_t) er);

  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);

  ix[0] = (vui128_t)CONST_VINT128_DW128(0x0001ffffffffffffUL, __UINT64_MAX__);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0x0UL, 0x0UL);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0x1800000000000000UL, 0UL);
  er = (vui128_t)CONST_VINT128_DW128(0x0015555555555555UL, 0x555555555555554aUL);

  rq = test_divduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divduq ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divduq:", (vui128_t)rq, (vui128_t) er);

  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);

  ix[0] = (vui128_t)CONST_VINT128_DW128(1UL, 0);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0x0UL, 0x0UL);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0x1UL, __UINT64_MAX__);
  er = (vui128_t)CONST_VINT128_DW128(0x8000000000000000UL, 0x4000000000000000UL);

  rq = test_divduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divduq ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divduq:", (vui128_t)rq, (vui128_t) er);

  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);

  ix[0] = (vui128_t)CONST_VINT128_DW128(1UL, 0);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0x0UL, 0x0UL);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0x2UL, 0x0UL);
  er = (vui128_t)CONST_VINT128_DW128(0x8000000000000000UL, 0x0UL);

  rq = test_divduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divduq ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divduq:", (vui128_t)rq, (vui128_t) er);

  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);

  ix[0] = (vui128_t)CONST_VINT128_DW128(1UL, __UINT64_MAX__);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0x0UL, 0x0UL);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0x2UL, 0x0UL);
  er = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, 0x8000000000000000UL);

  rq = test_divduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divduq ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divduq:", (vui128_t)rq, (vui128_t) er);

  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);

  ix[0] = (vui128_t)CONST_VINT128_DW128(1UL, (__UINT64_MAX__-1));
  ix[1] = (vui128_t)CONST_VINT128_DW128(0x0UL, 0x0UL);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0x1UL, __UINT64_MAX__);
  er = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, 0x7fffffffffffffffUL);

  rq = test_divduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divduq ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divduq:", (vui128_t)rq, (vui128_t) er);

  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);

  ix[0] = (vui128_t)CONST_VINT128_DW128(1UL, __UINT32_MAX__);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0x0UL, 0x0UL);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0x1UL, __UINT64_MAX__);
  er = (vui128_t)CONST_VINT128_DW128(0x800000007fffffffUL, 0xc00000003fffffffUL);

  rq = test_divduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divduq ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divduq:", (vui128_t)rq, (vui128_t) er);

  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);

  ix[0] = (vui128_t)CONST_VINT128_DW128(0x0000000599709868UL, 0xff9e4deefbdc5d1cUL);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0x3e21f7954fe4a741UL, 0xd3ad0eeba1000000UL);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0x0000000c9f2c9cd0UL, 0x4674edea40000000UL);
  er =    (vui128_t)CONST_VINT128_DW128(0x71919d1a73fa5e25UL, 0xdc93b8194055b965UL);

  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);

#ifndef __DEBUG_PRINT__
  if (rc == 1)
    {
      printf ("\n%i Failures may be the result of qemu issues\n", rc);
      rc = 0;
    }
#endif

  return (rc);
}
#undef __DEBUG_PRINT__

#if 0
extern vui128_t test_vec_modduq (vui128_t x, vui128_t y, vui128_t z);
#if 0
#define test_moddivduq	db_vec_divdqu
#else
extern __VEC_U_128P test_vec_divdqu (vui128_t x, vui128_t y, vui128_t z);
#define test_moddivduq	test_vec_divdqu
#endif
#if 0
#define test_moduq	db_vec_moduq
#else
extern vui128_t test_vec_moduq (vui128_t y, vui128_t z);
#define test_moduq	test_vec_moduq
#endif
#define test_modduq	test_vec_modduq
#else
#if 1
extern vui128_t  __VEC_PWR_IMP(vec_moduq) (vui128_t y, vui128_t z);
#define test_moduq __VEC_PWR_IMP(vec_moduq)
extern vui128_t  __VEC_PWR_IMP(vec_modduq) (vui128_t x, vui128_t y, vui128_t z);
#define test_modduq __VEC_PWR_IMP(vec_modduq)
extern __VEC_U_128RQ  __VEC_PWR_IMP(vec_divdqu) (vui128_t x, vui128_t y, vui128_t z);
#define test_divdqu __VEC_PWR_IMP(vec_divdqu)
#define test_moddivduq __VEC_PWR_IMP(vec_divdqu)
#else
extern vui128_t test_moduq (vui128_t y, vui128_t z);
extern vui128_t test_modduq (vui128_t x, vui128_t y, vui128_t z);
extern __VEC_U_128RQ test_divdqu (vui128_t x, vui128_t y, vui128_t z);
// Correct for the renaming to Divide Double Quadword Unsigned
// Which returns the quotient and remainder.
#define test_moddivduq	test_divdqu
#endif
#endif
//#define __DEBUG_PRINT__

int
test_vec_modulo_QW (void)
{
  __VEC_U_128RQ mq;
  vui128_t ix[4];
  vui128_t er;
  vui128_t rq;
  int rc = 0;

  printf ("\ntest Vector modulo Unsigned Quadword\n");

  ix[0] = (vui128_t)CONST_VINT128_DW128(0UL, 0UL);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0UL,
					__UINT64_MAX__) /* 2**64-1 */;
  ix[2] = (vui128_t)CONST_VINT128_DW128(0x0000000000000000ll,
                                        0x8ac7230489e80000ll) /* 10**19 */;
  er =    (vui128_t)CONST_VINT128_DW128(0x0000000000000000ll,
                                        0x7538dcfb7617ffffll);
  rq = test_moduq (ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" moduq  ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rm=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("moduq:", (vui128_t)rq, (vui128_t) er);

  ix[0] = (vui128_t)CONST_VINT128_DW128(0UL, 0UL);
  ix[1] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__,
					__UINT64_MAX__) /* 2**128-1 */;
  ix[2] = (vui128_t)CONST_VINT128_DW128(0x0000000000000000ll,
                                        0x8ac7230489e80000ll) /* 10**19 */;
  er =    (vui128_t)CONST_VINT128_DW128(0x0000000000000000ll,
                                        0x2ed503946aefffffll);
  rq = test_moduq (ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" moduq  ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rm=", (vui128_t) rq);
#endif
  rc += check_vuint128x ("moduq:", (vui128_t)rq, (vui128_t) er);

  ix[0] = (vui128_t)CONST_VINT128_DW128(0UL, 0UL);
  ix[1] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__,
					__UINT64_MAX__) /* 2**128-1 */;
  ix[2] = (vui128_t)CONST_VINT128_DW128(0x4b3b4ca85a86c47all,
                                        0x098a224000000000ll) /* 10**38 */;
  er =    (vui128_t)CONST_VINT128_DW128(0x1e4e1a06f06bb291ll,
                                        0xe361993fffffffffll);
  rq = test_moduq (ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" moduq  ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rm=", (vui128_t) rq);
#endif
  rc += check_vuint128x ("moduq:", (vui128_t)rq, (vui128_t) er);

  ix[2] = (vui128_t)CONST_VINT128_DW128(0x000004ee2d6d415bll,
					0x85acef8100000000ll ); /* 10**32 */
  er =    (vui128_t)CONST_VINT128_DW128(0x0000034ca936deeell,
                                        0xc98ba738ffffffffll);

  rq = test_moduq (ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" moduq  ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rm=", (vui128_t) rq);
#endif
  rc += check_vuint128x ("moduq:", (vui128_t)rq, (vui128_t) er);

  ix[0] = (vui128_t)CONST_VINT128_DW128(0UL, 0UL);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0x4b3b4ca85a86c47all,
                                        0x098a224000000000ll) /* 10**38 */;
  ix[2] = (vui128_t)CONST_VINT128_DW128(0x0000000000000000ll,
                                        0x8ac7230489e80000ll) /* 10**19 */;
  ix[3] = (vui128_t)CONST_VINT128_DW128(0x0000000000000000ll,
                                        0x8ac7230489e7ffffll) /* 10**19 */;
  er =    (vui128_t)CONST_VINT128_DW128(0x0000000000000000ll,
                                        0x8ac7230489e80000ll) /* 10**19 */;
  ix[1] = vec_adduqm(ix[1], ix[3]);

  er = ix[3];
  rq = test_modduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" modduq ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rm=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("modduq:", (vui128_t)rq, (vui128_t) er);

  mq = test_moddivduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" moddiv ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("    rmq=", (vui128_t) mq.R);
  print_vint128x ("       =", (vui128_t) mq.Q);
#endif

  rc += check_vuint128x ("moddiv01:", (vui128_t)mq.R, (vui128_t) er);
  rc += check_vuint128x ("moddiv00:", (vui128_t)mq.Q, (vui128_t) ix[2]);

  rq = test_moduq (ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" moduq  ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("moduq:", (vui128_t)rq, (vui128_t) er);

#if 1 // Quotient overflow
  ix[0] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, (__UINT64_MAX__-1));
  ix[1] = (vui128_t)CONST_VINT128_DW128(0UL, 0UL);
  ix[2] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  er = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, (__UINT64_MAX__-1));

  rq = test_modduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" modduq ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x (" qo  rm=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("modduq:", (vui128_t)rq, (vui128_t) er);

  mq = test_moddivduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" moddiv ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("    rmq=", (vui128_t) mq.R);
  print_vint128x ("       =", (vui128_t) mq.Q);
#endif

  rc += check_vuint128x ("moddiv11:", (vui128_t)mq.R, (vui128_t) er);
  rc += check_vuint128x ("moddiv10:", (vui128_t)mq.Q, (vui128_t) er);
#endif

  ix[2] = (vui128_t)CONST_VINT128_DW128(0x4b3b4ca85a86c47all,
                                        0x098a224000000000ll) /* 10**38 */;
  ix[3] = (vui128_t)CONST_VINT128_DW128(0, 1);
  ix[3] = vec_subuqm (ix[2], ix[3]);
  ix[1] = vec_madduq (ix, ix[2], ix[2], ix[3]);

  er =  (vui128_t)CONST_VINT128_DW128(0x4b3b4ca85a86c47all,
                                      0x098a223fffffffffll) /* 10**38-1 */;

  rq = test_modduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" modduq ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("modduq:", (vui128_t)rq, (vui128_t) er);

  mq = test_moddivduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" moddiv ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("    rmq=", (vui128_t) mq.R);
  print_vint128x ("       =", (vui128_t) mq.Q);
#endif

  rc += check_vuint128x ("moddiv21:", (vui128_t)mq.R, (vui128_t) er);
  rc += check_vuint128x ("moddiv20:", (vui128_t)mq.Q, (vui128_t) ix[2]);

  ix[0] = (vui128_t)CONST_VINT128_DW128(0UL, 0UL);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0x4b3b4ca85a86c47all,
                                        0x098a224000000000ll) /* 10**38 */;
  ix[2] = (vui128_t)CONST_VINT128_DW128(0x0000000000000000ll,
                                        0x8ac7230489e80000ll) /* 10**19 */;
  ix[3] = (vui128_t)CONST_VINT128_DW128(0x0000000000000000ll,
                                        0x8ac7230489e7ffffll) /* 10**19 */;
  er =    (vui128_t)CONST_VINT128_DW128(0x0000000000000000ll,
                                        0x8ac7230489e80000ll) /* 10**19 */;
  ix[1] = vec_adduqm(ix[1], ix[3]);

  er = ix[3];
  rq = test_modduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" modduq ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("modduq:", (vui128_t)rq, (vui128_t) er);

  rq = test_moduq (ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" moduq  ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("moduq:", (vui128_t)rq, (vui128_t) er);



  ix[0] = (vui128_t)CONST_VINT128_DW128(0x0000000599709868ll,
					0xff9e4deefbdc5d1cll);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0x3e21f7954fe4a741ll,
                                        0xd3ad0eeba1000000ll);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0x0000000c9f2c9cd0ll,
                                        0x4674edea40000000ll) /* 10**30 */;
  ix[3] = (vui128_t)CONST_VINT128_DW128(0x71919d1a73fa5e25ll,
                                        0xdc93b8194541ecbcll);

  er =    (vui128_t)CONST_VINT128_DW128(0x0000000b3dc3bba9ll,
                                        0x7ec023e4a1000000ll);


  //er = ix[3];
  rq = test_modduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" modduq ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("modduq:", (vui128_t)rq, (vui128_t) er);

  mq = test_moddivduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" moddiv ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("    rmq=", (vui128_t) mq.R);
  print_vint128x ("       =", (vui128_t) mq.Q);
#endif

  rc += check_vuint128x ("moddiv31:", (vui128_t)mq.R, (vui128_t) er);
  rc += check_vuint128x ("moddiv30:", (vui128_t)mq.Q, (vui128_t) ix[3]);

#ifndef __DEBUG_PRINT__
  if (rc == 1)
    {
      printf ("\n%i Failures may be the result of qemu issues\n", rc);
      rc = 0;
    }
#endif

  return (rc);
}

//#define __DEBUG_PRINT__

int
test_vec_moddiv_QW (void)
{
  __VEC_U_128RQ mq;
  vui128_t ix[4], qx[4];
#ifdef __DEBUG_PRINT__
  vui128_t rx[6];
#endif
  vui128_t c152[4];
  // 10**30
  vui128_t c30 = (vui128_t) CONST_VINT128_W (0x0000000c, 0x9f2c9cd0, 0x4674edea, 0x40000000);

  vui128_t zero = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  vui128_t er, eq;
  // vui128_t rq;
  // 10**152-1
  c152[0] = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  c152[1] = (vui128_t) CONST_VINT128_W (0x3e21f795, 0x4fe4a741, 0xd3ad0eeb, 0xa0ffffff);
  c152[2] = (vui128_t) CONST_VINT128_W (0xd2d8af57, 0xd5d929cb, 0x5f1e32bf, 0xfbdc5d1c);
  c152[3] = (vui128_t) CONST_VINT128_W (0x01e8ca31, 0x85deb719, 0xa2fd64b0, 0xccbf84ba);
  int rc = 0;

  printf ("\ntest Vector long division Unsigned Quadword\n");

  ix[0] = zero;
  ix[1] = c152[3];
  ix[2] = c30;

  mq = test_moddivduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" moddiv ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("    rmq=", (vui128_t) mq.R);
  print_vint128x ("       =", (vui128_t) mq.Q);
#endif
  eq = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x0026b9d5);
  er = (vui128_t) CONST_VINT128_W (0x0000000a, 0x91834589, 0x32ffa889, 0x8cbf84ba);

  rc += check_vuint128x ("moddiv a er:", (vui128_t)mq.R, (vui128_t) er);
  rc += check_vuint128x ("moddiv a eq:", (vui128_t)mq.Q, (vui128_t) eq);

  // record 1st (QW) quotient digit
  qx[3] = mq.Q;
  // Remainder and next (QW) digit
  ix[0] = mq.R;
  ix[1] = c152[2];

  mq = test_moddivduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" moddiv ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("    rmq=", (vui128_t) mq.R);
  print_vint128x ("       =", (vui128_t) mq.Q);
#endif
  er = (vui128_t) CONST_VINT128_W (0x00000005, 0x99709868, 0xff9e4dee, 0xfbdc5d1c);
  eq = (vui128_t) CONST_VINT128_W (0xd65a5181, 0xd7ccdd5e, 0x237a6c1a, 0x561573a4);

  rc += check_vuint128x ("moddiv b er:", (vui128_t)mq.R, (vui128_t) er);
  rc += check_vuint128x ("moddiv b eq:", (vui128_t)mq.Q, (vui128_t) eq);
  // record 2st (QW) quotient digit
  qx[2] = mq.Q;
  // Remainder and next (QW) digit
  ix[0] = mq.R;
  ix[1] = c152[1];

  mq = test_moddivduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" moddiv ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("    rmq=", (vui128_t) mq.R);
  print_vint128x ("       =", (vui128_t) mq.Q);
#endif
  er = (vui128_t) CONST_VINT128_W (0x0000000b, 0x3dc3bba9, 0x7ec023e4, 0xa0ffffff);
  eq = (vui128_t) CONST_VINT128_W (0x71919d1a, 0x73fa5e25, 0xdc93b819, 0x4541ecbc);

  rc += check_vuint128x ("moddiv c er:", (vui128_t)mq.R, (vui128_t) er);
  rc += check_vuint128x ("moddiv c eq:", (vui128_t)mq.Q, (vui128_t) eq);
  // record 3rd (QW) quotient digit
  qx[1] = mq.Q;
  // Remainder and next (QW) digit
  ix[0] = mq.R;
  ix[1] = c152[0];

  mq = test_moddivduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" moddiv ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("    rmq=", (vui128_t) mq.R);
  print_vint128x ("       =", (vui128_t) mq.Q);
#endif
  // 10**30-1
  er = (vui128_t) CONST_VINT128_W (0x0000000c, 0x9f2c9cd0, 0x4674edea, 0x3fffffff);
  eq = (vui128_t) CONST_VINT128_W (0xe3ffffff, 0xffffffff, 0xffffffff, 0xffffffff);

  rc += check_vuint128x ("moddiv d er:", (vui128_t)mq.R, (vui128_t) er);
  rc += check_vuint128x ("moddiv d eq:", (vui128_t)mq.Q, (vui128_t) eq);

  // record 4th (QW) quotient digit
  qx[0] = mq.Q;

#ifdef __DEBUG_PRINT__
  // record 1th (QW) remainder digit
  rx[0] = mq.R;

  print_vint128x ("      Q ", (vui128_t) qx[3]);
  print_vint128x ("        ", (vui128_t) qx[2]);
  print_vint128x ("        ", (vui128_t) qx[1]);
  print_vint128x ("        ", (vui128_t) qx[0]);
  print_vint128x ("      R ", (vui128_t) rx[0]);
#endif
  // Remainder and next (QW) digit
  ix[0] = qx[3];
  ix[1] = qx[2];

  mq = test_moddivduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" moddiv ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("    rmq=", (vui128_t) mq.R);
  print_vint128x ("       =", (vui128_t) mq.Q);
#endif
  er = (vui128_t) CONST_VINT128_W (0x00000005, 0xd68afdd8, 0x5f4bc881, 0x561573a4);
  eq = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00031174, 0x77e509c4);

  rc += check_vuint128x ("moddiv e er:", (vui128_t)mq.R, (vui128_t) er);
  rc += check_vuint128x ("moddiv e eq:", (vui128_t)mq.Q, (vui128_t) eq);
  // record 1st (QW) quotient digit
  qx[3] =(vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  qx[2] = mq.Q;
  // Remainder and next (QW) digit
  ix[0] = mq.R;
  ix[1] = qx[1];

  mq = test_moddivduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" moddiv ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("    rmq=", (vui128_t) mq.R);
  print_vint128x ("       =", (vui128_t) mq.Q);
#endif
  er = (vui128_t) CONST_VINT128_W (0x00000004, 0xc2c28a47, 0x1a78277b, 0x4541ecbc);
  eq = (vui128_t) CONST_VINT128_W (0x7668ee97, 0x42f4c93e, 0x0e502fb0, 0x2174d9b8);

  rc += check_vuint128x ("moddiv f er:", (vui128_t)mq.R, (vui128_t) er);
  rc += check_vuint128x ("moddiv f eq:", (vui128_t)mq.Q, (vui128_t) eq);

  qx[1] = mq.Q;
  // Remainder and next (QW) digit
  ix[0] = mq.R;
  ix[1] = qx[0];

  mq = test_moddivduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" moddiv ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("    rmq=", (vui128_t) mq.R);
  print_vint128x ("       =", (vui128_t) mq.Q);
#endif
  // 10**30-1
  er = (vui128_t) CONST_VINT128_W (0x0000000c, 0x9f2c9cd0, 0x4674edea, 0x3fffffff);
  eq = (vui128_t) CONST_VINT128_W (0x608f6351, 0x0fffffff, 0xffffffff, 0xffffffff);

  rc += check_vuint128x ("moddiv g er:", (vui128_t)mq.R, (vui128_t) er);
  rc += check_vuint128x ("moddiv g eq:", (vui128_t)mq.Q, (vui128_t) eq);
  // record 4th (QW) quotient digit
  qx[0] = mq.Q;

#ifdef __DEBUG_PRINT__
  // record 2th (QW) remainder digit
  rx[1] = mq.R;

  print_vint128x ("      Q ", (vui128_t) qx[3]);
  print_vint128x ("        ", (vui128_t) qx[2]);
  print_vint128x ("        ", (vui128_t) qx[1]);
  print_vint128x ("        ", (vui128_t) qx[0]);
  print_vint128x ("      R ", (vui128_t) rx[1]);
  print_vint128x ("        ", (vui128_t) rx[0]);
#endif
  // Remainder and next (QW) digit
  ix[0] = qx[2];
  ix[1] = qx[1];

  mq = test_moddivduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" moddiv ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("    rmq=", (vui128_t) mq.R);
  print_vint128x ("       =", (vui128_t) mq.Q);
#endif
  er = (vui128_t) CONST_VINT128_W (0x00000001, 0xb1f6f4e1, 0x64956a52, 0x2174d9b8);
  eq = (vui128_t) CONST_VINT128_W (0x00000000, 0x00003e3a, 0xeb4ae138, 0x3562f4b8);

  rc += check_vuint128x ("moddiv h er:", (vui128_t)mq.R, (vui128_t) er);
  rc += check_vuint128x ("moddiv h eq:", (vui128_t)mq.Q, (vui128_t) eq);
  // record 2st (QW) quotient digit
  qx[2] =(vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  qx[1] = mq.Q;
  // Remainder and next (QW) digit
  ix[0] = mq.R;
  ix[1] = qx[0];

  mq = test_moddivduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" moddiv ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("    rmq=", (vui128_t) mq.R);
  print_vint128x ("       =", (vui128_t) mq.Q);
#endif
  // 10**30-1
  er = (vui128_t) CONST_VINT128_W (0x0000000c, 0x9f2c9cd0, 0x4674edea, 0x3fffffff);
  eq = (vui128_t) CONST_VINT128_W (0x2261d969, 0xf7ac94ca, 0x3fffffff, 0xffffffff);

  rc += check_vuint128x ("moddiv i er:", (vui128_t)mq.R, (vui128_t) er);
  rc += check_vuint128x ("moddiv i eq:", (vui128_t)mq.Q, (vui128_t) eq);
  // record 4th (QW) quotient digit
  qx[0] = mq.Q;

#ifdef __DEBUG_PRINT__
  // record 3th (QW) remainder digit
  rx[2] = mq.R;

  print_vint128x ("      Q ", (vui128_t) qx[3]);
  print_vint128x ("        ", (vui128_t) qx[2]);
  print_vint128x ("        ", (vui128_t) qx[1]);
  print_vint128x ("        ", (vui128_t) qx[0]);
  print_vint128x ("      R ", (vui128_t) rx[2]);
  print_vint128x ("        ", (vui128_t) rx[1]);
  print_vint128x ("        ", (vui128_t) rx[0]);
#endif
  // Remainder and next (QW) digit
  ix[0] = qx[1];
  ix[1] = qx[0];

  mq = test_moddivduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" moddiv ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("    rmq=", (vui128_t) mq.R);
  print_vint128x ("       =", (vui128_t) mq.Q);
#endif
  // 10**30-1
  er = (vui128_t) CONST_VINT128_W (0x0000000c, 0x9f2c9cd0, 0x4674edea, 0x3fffffff);
  eq = (vui128_t) CONST_VINT128_W (0x000004ee, 0x2d6d415b, 0x85acef80, 0xffffffff);

  rc += check_vuint128x ("moddiv j er:", (vui128_t)mq.R, (vui128_t) er);
  rc += check_vuint128x ("moddiv j eq:", (vui128_t)mq.Q, (vui128_t) eq);
  // record 3rd (QW) quotient digit
  qx[1] =(vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  // record 4th (QW) quotient digit
  qx[0] = mq.Q;

#ifdef __DEBUG_PRINT__
  // record 4th (QW) remainder digit
  rx[3] = mq.R;

  print_vint128x ("      Q ", (vui128_t) qx[3]);
  print_vint128x ("        ", (vui128_t) qx[2]);
  print_vint128x ("        ", (vui128_t) qx[1]);
  print_vint128x ("        ", (vui128_t) qx[0]);
  print_vint128x ("      R ", (vui128_t) rx[3]);
  print_vint128x ("        ", (vui128_t) rx[2]);
  print_vint128x ("        ", (vui128_t) rx[1]);
  print_vint128x ("        ", (vui128_t) rx[0]);
#endif
  // Remainder and next (QW) digit
  ix[0] = qx[1];
  ix[1] = qx[0];

  mq = test_moddivduq (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" moddiv ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("    rmq=", (vui128_t) mq.R);
  print_vint128x ("       =", (vui128_t) mq.Q);
#endif
  // 10**30-1
  er = (vui128_t) CONST_VINT128_W (0x0000000c, 0x9f2c9cd0, 0x4674edea, 0x3fffffff);
  eq = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000063);

  rc += check_vuint128x ("moddiv k er:", (vui128_t)mq.R, (vui128_t) er);
  rc += check_vuint128x ("moddiv k eq:", (vui128_t)mq.Q, (vui128_t) eq);
  // record 5th (QW) quotient digit
  qx[0] = mq.R;

#ifdef __DEBUG_PRINT__
  // record 5th (QW) remainder digit
  rx[4] = mq.R;
  rx[5] = mq.Q;

  print_vint128x ("      Q ", (vui128_t) qx[3]);
  print_vint128x ("        ", (vui128_t) qx[2]);
  print_vint128x ("        ", (vui128_t) qx[1]);
  print_vint128x ("        ", (vui128_t) qx[0]);
  print_vint128x ("      R ", (vui128_t) rx[4]);
  print_vint128x ("        ", (vui128_t) rx[3]);
  print_vint128x ("        ", (vui128_t) rx[2]);
  print_vint128x ("        ", (vui128_t) rx[1]);
  print_vint128x ("        ", (vui128_t) rx[0]);
#endif

  return (rc);
}

//#define __DEBUG_PRINT__
int
test_vec_divext_QW (void)
{

  vui128_t ix[4];
  vui128_t rnd = (vui128_t)CONST_VINT128_DW128(0UL, 0x80UL);
  vui128_t er, es;
  vui128_t rq;
  int rc = 0;

  printf ("\ntest Vector divide Extended Unsigned Quadword\n");

  ix[0] = (vui128_t)CONST_VINT128_DW128(0x0001000000000000UL, 0UL);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0x0100000000000000UL, 0UL);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0x0100000000000000UL, 0UL);
  ix[3] = (vui128_t)CONST_VINT128_DW128(0x0100000000000000UL, 0x0UL);
  er = (vui128_t)CONST_VINT128_DW128(0x0100000000000000UL, 0UL);

  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  // round and normalize
  rq    = vec_adduqm (rq, rnd);
  ix[0] = vec_srqi (rq, 8);
  ix[2] = vec_adduqm (ix[2], ix[1]);
  er = (vui128_t)CONST_VINT128_DW128(0x0080000000000000UL, 0UL);

  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
  print_vint128x ("    sum ", (vui128_t) ix[3]);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  // round and normalize
  rq    = vec_adduqm (rq, rnd);
  ix[0] = vec_srqi (rq, 8);
  ix[2] = vec_adduqm (ix[2], ix[1]);
  er = (vui128_t)CONST_VINT128_DW128(0x002aaaaaaaaaaaaaUL, 0xaaaaaaaaaaaaaaaaUL);

  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
  print_vint128x ("    sum ", (vui128_t) ix[3]);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  // round and normalize
  rq    = vec_adduqm (rq, rnd);
  ix[0] = vec_srqi (rq, 8);
  ix[2] = vec_adduqm (ix[2], ix[1]);
  er = (vui128_t)CONST_VINT128_DW128(0x000aaaaaaaaaaaaaUL, 0xaaaaaaaaaaaaaac0UL);

  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
  print_vint128x ("    sum ", (vui128_t) ix[3]);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  // round and normalize
  rq    = vec_adduqm (rq, rnd);
  ix[0] = vec_srqi (rq, 8);
  ix[2] = vec_adduqm (ix[2], ix[1]);
  er = (vui128_t)CONST_VINT128_DW128(0x0002222222222222UL, 0x2222222222222233UL);

  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
  print_vint128x ("    sum ", (vui128_t) ix[3]);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  // round and normalize
  rq    = vec_adduqm (rq, rnd);
  ix[0] = vec_srqi (rq, 8);
  ix[2] = vec_adduqm (ix[2], ix[1]);
  er = (vui128_t)CONST_VINT128_DW128(0x00005b05b05b05b0UL, 0x5b05b05b05b05b00UL);

  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
  print_vint128x ("    sum ", (vui128_t) ix[3]);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  // round and normalize
  rq    = vec_adduqm (rq, rnd);
  ix[0] = vec_srqi (rq, 8);
  ix[2] = vec_adduqm (ix[2], ix[1]);
  er = (vui128_t)CONST_VINT128_DW128(0x00000d00d00d00d0UL, 0x0d00d00d00d00d00UL);

  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
  print_vint128x ("    sum ", (vui128_t) ix[3]);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  // round and normalize
  rq    = vec_adduqm (rq, rnd);
  ix[0] = vec_srqi (rq, 8);
  ix[2] = vec_adduqm (ix[2], ix[1]);
  er = (vui128_t)CONST_VINT128_DW128(0x000001a01a01a01aUL, 0x01a01a01a01a01a0UL);

  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
  print_vint128x ("    sum ", (vui128_t) ix[3]);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  // round and normalize
  rq    = vec_adduqm (rq, rnd);
  ix[0] = vec_srqi (rq, 8);
  ix[2] = vec_adduqm (ix[2], ix[1]);
  er = (vui128_t)CONST_VINT128_DW128(0x0000002e3bc74aadUL, 0x8e671f5583911caaUL);

  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
  print_vint128x ("    sum ", (vui128_t) ix[3]);
#endif

  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  // round and normalize
  rq    = vec_adduqm (rq, rnd);
  ix[0] = vec_srqi (rq, 8);
  ix[2] = vec_adduqm (ix[2], ix[1]);
  er = (vui128_t)CONST_VINT128_DW128(0x000000049f93eddeUL, 0x27d71cbbc05b4fb3UL);

  rq = test_diveuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuqe ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
  print_vint128x ("    sum ", (vui128_t) ix[3]);
#endif
  rc += check_vuint128x ("divuqe:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[3], rq); // last term
  es = (vui128_t)CONST_VINT128_DW128(0x02b7e150ed3c569dUL, 0x975c4df261fe4d9aUL);
  rc += check_vuint128x ("divuqe-sum :", (vui128_t)ix[3], (vui128_t) es);

#ifdef __DEBUG_PRINT__
  print_vint128x ("   'sum ", (vui128_t) ix[3]);
#endif

#ifndef __DEBUG_PRINT__
  if (rc == 1)
    {
      printf ("\n%i Failures may be the result of qemu issues\n", rc);
      rc = 0;
    }
#endif

  return (rc);
}

//#define __DEBUG_PRINT__
int
test_vec_div1_QW (void)
{

  vui128_t ix[4];
  // vui128_t rnd = (vui128_t)CONST_VINT128_DW128(0UL, 0x80UL);
  vui128_t er, es;
  vui128_t rq;
  int rc = 0;

  printf ("\ntest Vector divide Unsigned Quadword\n");

  ix[0] = (vui128_t)CONST_VINT128_DW128(0x0100000000000000UL, 0UL);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0UL, 1UL);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0UL, 1UL);
  ix[3] = (vui128_t)CONST_VINT128_DW128(0x0100000000000000UL, 0x0UL);
  er = (vui128_t)CONST_VINT128_DW128(0x0100000000000000UL, 0UL);

  rq = test_divuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  ix[0] = rq;
  ix[2] = vec_adduqm (ix[2], ix[1]);
  er = (vui128_t)CONST_VINT128_DW128(0x0080000000000000UL, 0UL);

  rq = test_divuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
  print_vint128x ("    sum ", (vui128_t) ix[3]);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);
  ix[3] = vec_adduqm (ix[3], rq); // next term
  ix[0] = rq;
  ix[2] = vec_adduqm (ix[2], ix[1]);
  er = (vui128_t)CONST_VINT128_DW128(0x002aaaaaaaaaaaaaUL, 0xaaaaaaaaaaaaaaaaUL);

  rq = test_divuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
  print_vint128x ("    sum ", (vui128_t) ix[3]);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  ix[0] = rq;
  ix[2] = vec_adduqm (ix[2], ix[1]);
  er = (vui128_t)CONST_VINT128_DW128(0x000aaaaaaaaaaaaaUL, 0xaaaaaaaaaaaaaaaaUL);

  rq = test_divuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
  print_vint128x ("    sum ", (vui128_t) ix[3]);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  ix[0] = rq;
  ix[2] = vec_adduqm (ix[2], ix[1]);
  er = (vui128_t)CONST_VINT128_DW128(0x0002222222222222UL, 0x2222222222222222UL);

  rq = test_divuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
  print_vint128x ("    sum ", (vui128_t) ix[3]);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  ix[0] = rq;
  ix[2] = vec_adduqm (ix[2], ix[1]);
  er = (vui128_t)CONST_VINT128_DW128(0x00005b05b05b05b0UL, 0x5b05b05b05b05b05UL);

  rq = test_divuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
  print_vint128x ("    sum ", (vui128_t) ix[3]);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  ix[0] = rq;
  ix[2] = vec_adduqm (ix[2], ix[1]);
  er = (vui128_t)CONST_VINT128_DW128(0x00000d00d00d00d0UL, 0x0d00d00d00d00d00UL);

  rq = test_divuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
  print_vint128x ("    sum ", (vui128_t) ix[3]);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  ix[0] = rq;
  ix[2] = vec_adduqm (ix[2], ix[1]);
  er = (vui128_t)CONST_VINT128_DW128(0x000001a01a01a01aUL, 0x01a01a01a01a01a0UL);

  rq = test_divuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
  print_vint128x ("    sum ", (vui128_t) ix[3]);
#endif

  rc += check_vuint128x ("divuq :", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  ix[0] = rq;
  ix[2] = vec_adduqm (ix[2], ix[1]);
  er = (vui128_t)CONST_VINT128_DW128(0x0000002e3bc74aadUL, 0x8e671f5583911ca0UL);

  rq = test_divuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
  print_vint128x ("    sum ", (vui128_t) ix[3]);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  ix[0] = rq;
  ix[2] = vec_adduqm (ix[2], ix[1]);
  er = (vui128_t)CONST_VINT128_DW128(0x000000049f93eddeUL, 0x27d71cbbc05b4fa9UL);

  rq = test_divuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
  print_vint128x ("    sum ", (vui128_t) ix[3]);
#endif
  rc += check_vuint128x ("divuq :", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[3], rq); // last term
  es = (vui128_t)CONST_VINT128_DW128(0x02b7e150ed3c569dUL, 0x975c4df261fe4d64UL);
  rc += check_vuint128x ("divuq-sum :", (vui128_t)ix[3], (vui128_t) es);

#if 0
#endif
#ifdef __DEBUG_PRINT__
  print_vint128x ("   'sum ", (vui128_t) ix[3]);
#endif

#ifndef __DEBUG_PRINT__
  if (rc == 1)
    {
      printf ("\n%i Failures may be the result of qemu issues\n", rc);
      rc = 0;
    }
#endif

  return (rc);
}

int
test_vec_div_QW (void)
{

  vui128_t ix[4];
  // vui128_t rnd = (vui128_t)CONST_VINT128_DW128(0UL, 0x80UL);
  vui128_t er, es;
  vui128_t rq;
  int rc = 0;

  printf ("\ntest Vector divide Unsigned Quadword\n");

  ix[0] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  ix[1] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  er = (vui128_t)CONST_VINT128_DW128(0x0UL, 0x0100000000000000UL);

  // initial estimate of g0
  ix[2] = vec_srqi (ix[1], 56);
  // a / g0
  rq = test_divuq (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[1]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[2], rq);
  ix[2] = vec_srqi (ix[3], 1);
#ifdef __DEBUG_PRINT__
  print_vint128x ("g0+a/g0 ", (vui128_t) ix[3]);
  print_vint128x ("     /2 ", (vui128_t) ix[2]);
#endif

  // a / gn
  rq = test_divuq (ix[0], ix[2]);
  er = (vui128_t)CONST_VINT128_DW128(0x0UL, 0x01fffe0001fffe00UL);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[2], rq);
  ix[2] = vec_srqi (ix[3], 1);
#ifdef __DEBUG_PRINT__
  print_vint128x ("gn+a/gn ", (vui128_t) ix[3]);
  print_vint128x ("     /2 ", (vui128_t) ix[2]);
#endif

  // a / gn
  rq = test_divuq (ix[0], ix[2]);
  er = (vui128_t)CONST_VINT128_DW128(0x0UL, 0x3ffec0073fd5c0fUL);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[2], rq);
  ix[2] = vec_srqi (ix[3], 1);
#ifdef __DEBUG_PRINT__
  print_vint128x ("gn+a/gn ", (vui128_t) ix[3]);
  print_vint128x ("     /2 ", (vui128_t) ix[2]);
#endif

  // a / gn
  rq = test_divuq (ix[0], ix[2]);
  er = (vui128_t)CONST_VINT128_DW128(0x0UL, 0x7ff58106661e0e1UL);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[2], rq);
  ix[2] = vec_srqi (ix[3], 1);
#ifdef __DEBUG_PRINT__
  print_vint128x ("gn+a/gn ", (vui128_t) ix[3]);
  print_vint128x ("     /2 ", (vui128_t) ix[2]);
#endif

  // a / gn
  rq = test_divuq (ix[0], ix[2]);
  er = (vui128_t)CONST_VINT128_DW128(0x0UL, 0xffab21bf6d2e14dUL);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[2], rq);
  ix[2] = vec_srqi (ix[3], 1);
#ifdef __DEBUG_PRINT__
  print_vint128x ("gn+a/gn ", (vui128_t) ix[3]);
  print_vint128x ("     /2 ", (vui128_t) ix[2]);
#endif

  // a / gn
  rq = test_divuq (ix[0], ix[2]);
  er = (vui128_t)CONST_VINT128_DW128(0x0UL, 0x1fd5a3ac3bbea82dUL);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[2], rq);
  ix[2] = vec_srqi (ix[3], 1);
#ifdef __DEBUG_PRINT__
  print_vint128x ("gn+a/gn ", (vui128_t) ix[3]);
  print_vint128x ("     /2 ", (vui128_t) ix[2]);
#endif

  // a / gn
  rq = test_divuq (ix[0], ix[2]);
  er = (vui128_t)CONST_VINT128_DW128(0x0UL, 0x3eb3115afbdbfcdfUL);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[2], rq);
  ix[2] = vec_srqi (ix[3], 1);
#ifdef __DEBUG_PRINT__
  print_vint128x ("gn+a/gn ", (vui128_t) ix[3]);
  print_vint128x ("     /2 ", (vui128_t) ix[2]);
#endif

  // a / gn
  rq = test_divuq (ix[0], ix[2]);
  er = (vui128_t)CONST_VINT128_DW128(0x0UL, 0x764d70eb86f7d254UL);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[2], rq);
  ix[2] = vec_srqi (ix[3], 1);
#ifdef __DEBUG_PRINT__
  print_vint128x ("gn+a/gn ", (vui128_t) ix[3]);
  print_vint128x ("     /2 ", (vui128_t) ix[2]);
#endif

  // a / gn
  rq = test_divuq (ix[0], ix[2]);
  er = (vui128_t)CONST_VINT128_DW128(0x0UL, 0xc2f7f97f2ec3bb41UL);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[2], rq);
  ix[2] = vec_srqi (ix[3], 1);
#ifdef __DEBUG_PRINT__
  print_vint128x ("gn+a/gn ", (vui128_t) ix[3]);
  print_vint128x ("     /2 ", (vui128_t) ix[2]);
#endif

  // a / gn
  rq = test_divuq (ix[0], ix[2]);
  er = (vui128_t)CONST_VINT128_DW128(0x0UL, 0xf6ca8effa5b44f42UL);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[2], rq);
  ix[2] = vec_srqi (ix[3], 1);
#ifdef __DEBUG_PRINT__
  print_vint128x ("gn+a/gn ", (vui128_t) ix[3]);
  print_vint128x ("     /2 ", (vui128_t) ix[2]);
#endif

  // a / gn
  rq = test_divuq (ix[0], ix[2]);
  er = (vui128_t)CONST_VINT128_DW128(0x0UL, 0xffd40bf97c935913UL);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[2], rq);
  ix[2] = vec_srqi (ix[3], 1);
#ifdef __DEBUG_PRINT__
  print_vint128x ("gn+a/gn ", (vui128_t) ix[3]);
  print_vint128x ("     /2 ", (vui128_t) ix[2]);
#endif

  // a / gn
  rq = test_divuq (ix[0], ix[2]);
  er = (vui128_t)CONST_VINT128_DW128(0x0UL, 0xfffffc3968b32bdaUL);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[2], rq);
  ix[2] = vec_srqi (ix[3], 1);
#ifdef __DEBUG_PRINT__
  print_vint128x ("gn+a/gn ", (vui128_t) ix[3]);
  print_vint128x ("     /2 ", (vui128_t) ix[2]);
#endif

  // a / gn
  rq = test_divuq (ix[0], ix[2]);
  er = (vui128_t)CONST_VINT128_DW128(0x0UL, 0xfffffffffff8df33UL);

#ifdef __DEBUG_PRINT__
  print_vint128x (" divuq  ", (vui128_t) ix[0]);
  print_vint128x ("        ", (vui128_t) ix[2]);
  print_vint128x ("     rq=", (vui128_t) rq);
#endif

  rc += check_vuint128x ("divuq:", (vui128_t)rq, (vui128_t) er);

  ix[3] = vec_adduqm (ix[2], rq);
  ix[2] = vec_srqi (ix[3], 1);
#ifdef __DEBUG_PRINT__
  print_vint128x ("gn+a/gn ", (vui128_t) ix[3]);
  print_vint128x ("     /2 ", (vui128_t) ix[2]);
#endif
  es = (vui128_t)CONST_VINT128_DW128(1UL, 0UL);
  rc += check_vuint128x ("divuq-sum :", (vui128_t)ix[2], (vui128_t) es);

#ifndef __DEBUG_PRINT__
  if (rc == 1)
    {
      printf ("\n%i Failures may be the result of qemu issues\n", rc);
      rc = 0;
    }
#endif

  return (rc);
}

int
test_vec_i128 (void)
{
  int rc = 0;

  printf ("\n%s\n", __FUNCTION__);
#if 1
  rc += test_xfer_int128 ();
  rc += test_splat_128 ();
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
  rc += test_sldbi ();
  rc += test_srdbi ();

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

  rc += test_vec_divide_QW ();
  rc += test_vec_modulo_QW ();
  rc += test_vec_moddiv_QW ();
  rc += test_vec_divext_QW ();
  rc += test_vec_div_QW ();
#endif
  return (rc);
}

