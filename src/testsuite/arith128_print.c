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

 arith128_print.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Apr 25, 2014
 */

#include <stdint.h>
#include <stdio.h>
#include <float.h>
#define __DEBUG_PRINT__
#include "vec_f128_ppc.h"
#include <testsuite/arith128_print.h>
//#include "math_private.h"
#include "ieee754.h"

long tcount;

void
print_dfp128 (char *prefix, _Decimal128 val128)
{
  printf ("%s %DDf\n", prefix, val128);
}

void
print_dfp128e (char *prefix, _Decimal128 val128)
{
  printf ("%s %36.33DDe\n", prefix, val128);
}

void
print_dfp64 (char *prefix, _Decimal64 val64)
{
  printf ("%s %Df\n", prefix, val64);
}

void
print_dfp32 (char *prefix, _Decimal32 val32)
{
  printf ("%s %Hf\n", prefix, val32);
}

int
check_dfp32 (char *prefix, _Decimal32 val32, _Decimal32 shouldbe)
{
  int rc = 0;
  if (val32 != shouldbe)
    {
      rc = 1;
      printf ("%s should be %Hf, is %Hf\n", prefix, shouldbe, val32);
    }

  return (rc);
}

int
check_dfp64 (char *prefix, _Decimal64 val64, _Decimal64 shouldbe)
{
  int rc = 0;
  if (val64 != shouldbe)
    {
      rc = 1;
      printf ("%s should be %Df, is %Df\n", prefix, shouldbe, val64);
    }

  return (rc);
}

int
check_dfp128 (char *prefix, _Decimal128 val128, _Decimal128 shouldbe)
{
  int rc = 0;
  if (val128 != shouldbe)
    {
      rc = 1;
      printf ("%s should be %DDf, is %DDf\n", prefix, shouldbe, val128);
    }

  return (rc);
}

void
print_int128 (char *prefix, __int128 val128)
{
  __int128 ten_17 = (__int128 ) 100000000000000000UL;
  /* Generating large __int128 constants is tricky.  There is no
   * support in GCC for expressing an integer constant of type
   * __int128 for targets with long long integer less than 128 bits
   * wide. */
  __int128 mtwo_127 = (((__int128 ) 0x8000000000000000L) << 64);
  __int128 x, y, z;
  unsigned long long t_low, t_mid, t_high;
//	long long t_low, t_mid, t_high;
  char sign = '+';

  if (val128 == mtwo_127)
    {
      /* The maximum negative value just wraps back to the same
       * value.  Just print it.  */
      printf ("%s-170141183460469231731687303715884105728\n", prefix);
    }
  else
    {
      if (val128 < (__int128 ) 0LL)
        {
          sign = '-';
          val128 = -val128;
        }
      /* Else convert the absolute (unsigned) value to Decimal and
       * add the sign later.  */
      y = val128 / ten_17;
      z = val128 % ten_17;
      t_low = (unsigned long long) z;
      x = y / ten_17;
      z = y % ten_17;
      t_mid = (unsigned long long) z;
      t_high = (unsigned long long) x;
      printf ("%s%c%05lld%017lld%017lld\n", prefix, sign, t_high, t_mid, t_low);
    }
}

void
print_uint128 (char *prefix, unsigned __int128 val128)
  {
    unsigned __int128 ten_17 = (unsigned __int128)100000000000000000UL;
    unsigned __int128 x, y, z;
    unsigned long long t_low, t_mid, t_high;

    /* Else convert the absolute (unsigned) value to Decimal and
     * add the sign later.  */
    y = val128 / ten_17;
    z = val128 % ten_17;
    t_low = (unsigned long long)z;
    x = y / ten_17;
    z = y % ten_17;
    t_mid = (unsigned long long)z;
    t_high = (unsigned long long)x;
    printf ("%s %05lld%017lld%017lld\n", prefix, t_high, t_mid, t_low);
  }

void
print_int64 (char *prefix, long val64)
{
  printf ("%s %18ld\n", prefix, val64);
}

void
print_uint64 (char *prefix, unsigned long val64)
{
  printf ("%s %18lu\n", prefix, val64);
}

void
print_int64x (char *prefix, unsigned long val64)
{
  printf ("%s %016lx\n", prefix, val64);
}

void
print_int128x (char *prefix, unsigned __int128 val128)
  {
    __VEC_U_128 val;
    val.ui128 = val128;
#if __LITTLE_ENDIAN__
    printf ("%s%08x%08x%08x%08x\n", prefix, val.vx4[3], val.vx4[2], val.vx4[1], val.vx4[0]);
#else
    printf ("%s%08x%08x%08x%08x\n", prefix, val.vx4[0], val.vx4[1], val.vx4[2],
        val.vx4[3]);
#endif
  }

int
check_sdiv128_64 (__int128 numerator, int64_t divisor, int64_t exp_q,
                  int64_t exp_r)
{
  __int128 x;
  int64_t Q, R;
  int rc = 0;

  print_int128 ("check_sdiv128_64 N ", numerator);
  print_int128 ("check_sdiv128_64 D ", divisor);
  Q = div_qd (&R, numerator, divisor);

  if (Q != exp_q)
    {
      rc = 1;
      print_int128 ("check_sdiv128_64 expect ", exp_q);
      print_int128 ("check_sdiv128_64 Q is   ", Q);
    }

  if (R != exp_r)
    {
      rc = 1;
      print_int128 ("check_sdiv128_64 expect ", exp_r);
      print_int128 ("check_sdiv128_64 R is   ", R);
    }

  x = (unsigned __int128)Q * (unsigned __int128)divisor;
  x = x + R;

  if (x != numerator)
    {
      rc = 1;
      print_int128 ("check_sdiv128_64 expect ", numerator);
      print_int128 ("check_sdiv128_64 Q * D + R is ", x);
    }

  return (rc);
}

int
check_udiv128_64 (unsigned __int128 numerator, uint64_t divisor, uint64_t exp_q, uint64_t exp_r)
  {
    unsigned __int128 x;
    uint64_t Q, R;
    int rc = 0;

    print_uint128 ("check_udiv128_64 N ", numerator);
    print_uint128 ("check_udiv128_64 D ", divisor);
    Q = div_qdu (&R, numerator, divisor);

    if (Q != exp_q)
      {
        rc = 1;
        print_uint128 ("check_udiv128_64 expect ", exp_q);
        print_uint128 ("check_udiv128_64 Q is   ", Q);
      }

    if (R != exp_r)
      {
        rc = 1;
        print_uint128 ("check_udiv128_64 expect ", exp_r);
        print_uint128 ("check_udiv128_64 R is   ", R);
      }

    x = (unsigned __int128)Q * (unsigned __int128)divisor;
    x = x + R;

    if (x != numerator)
      {
        rc = 1;
        print_uint128 ("check_udiv128_64 expect ", numerator);
        print_uint128 ("check_udiv128_64 Q * D + R is ", x);
      }

    return (rc);
  }

int
check_sdiv128_64x (__int128 numerator, int64_t divisor, int64_t exp_q,
                   int64_t exp_r)
{
  __int128 x;
  int64_t Q, R;
  int rc = 0;

  print_int128x ("check_sdiv128_64 N ", numerator);
  print_int128x ("check_sdiv128_64 D ", divisor);
  Q = div_qd (&R, numerator, divisor);

  if (Q != exp_q)
    {
      rc = 1;
      print_int128x ("check_sdiv128_64 expect ", exp_q);
      print_int128x ("check_sdiv128_64 Q is   ", Q);
    }

  if (R != exp_r)
    {
      rc = 1;
      print_int128x ("check_sdiv128_64 expect ", exp_r);
      print_int128x ("check_sdiv128_64 R is   ", R);
    }

  x = (unsigned __int128)Q * (unsigned __int128)divisor;
  x = x + R;

  if (x != numerator)
    {
      rc = 1;
      print_int128x ("check_sdiv128_64 expect ", numerator);
      print_int128x ("check_sdiv128_64 Q * D + R is ", x);
    }

  return (rc);
}

int
check_udiv128_64x (unsigned __int128 numerator, uint64_t divisor, uint64_t exp_q, uint64_t exp_r)
  {
    unsigned __int128 x;
    uint64_t Q, R;
    int rc = 0;

    print_int128x ("check_udiv128_64 N ", numerator);
    print_int128x ("check_udiv128_64 D ", divisor);
    Q = div_qdu (&R, numerator, divisor);

    if (Q != exp_q)
      {
        rc = 1;
        print_int128x ("check_udiv128_64 expect ", exp_q);
        print_int128x ("check_udiv128_64 Q is   ", Q);
      }

    if (R != exp_r)
      {
        rc = 1;
        print_int128x ("check_udiv128_64 expect ", exp_r);
        print_int128x ("check_udiv128_64 R is   ", R);
      }

    x = (unsigned __int128)Q * (unsigned __int128)divisor;
    x = x + R;

    if (x != numerator)
      {
        rc = 1;
        print_int128x ("check_udiv128_64 expect ", numerator);
        print_int128x ("check_udiv128_64 Q * D + R is ", x);
      }

    return (rc);
  }

void
print_ibm128 (char *prefix, long double ldbl)
{
  double high, low;

  printf ("%s:% .34" "Le" " % .28" "La" "\n", prefix, ldbl, ldbl);

  high = __builtin_unpack_longdouble (ldbl, 0);
  low = __builtin_unpack_longdouble (ldbl, 1);
  printf ("   %s:% .14" "a" " % .14" "a" "\n", prefix, high, low);
}

void
print_ibm128x (char *prefix, long double ldbl)
{
  double high, low;
  high = __builtin_unpack_longdouble (ldbl, 0);
  low = __builtin_unpack_longdouble (ldbl, 1);
  printf ("%s:% .28La, /* %Lg */\n", prefix, ldbl, ldbl);
  printf ("\t:% .13a, % .13a*/\n", high, low);
}

void
print_int128_sum (char *prefix,
    unsigned __int128 r, unsigned __int128 a, unsigned __int128 b)
  {
    printf ("%s\n", prefix);
    print_int128 ("  a = ", a);
    print_int128 ("  b = ", b);
    print_int128 ("  r = ", r);
  }

void
print_int128_carry (char *prefix,
    unsigned __int128 r, unsigned __int128 a, unsigned __int128 b,
    unsigned __int128 c)
  {
    printf ("%s\n", prefix);
    print_int128 ("  a = ", a);
    print_int128 ("  b = ", b);
    print_int128 ("  c = ", c);
    print_int128 ("  r = ", r);
}

void
print_vint32d (char *prefix, vui32_t val)
{
  printf ("%s%10d,%10d,%10d,%10d\n", prefix, val[0], val[1], val[2], val[3]);
}

void
print_vint32x (char *prefix, vui32_t val)
{
  printf ("%s%08x,%08x,%08x,%08x\n", prefix, val[0], val[1], val[2], val[3]);
}

void
print_vint16d (char *prefix, vui16_t val)
{
  printf ("%s%5d,%5d,%5d,%5d,%5d,%5d,%5d,%5d\n", prefix, val[0], val[1], val[2],
	  val[3], val[4], val[5], val[6], val[7]);
}

void
print_vint16x (char *prefix, vui16_t val)
{
  printf ("%s%04x,%4x,%4x,%4x,%4x,%4x,%4x,%4x\n", prefix, val[0], val[1],
	  val[2], val[3], val[4], val[5], val[6], val[7]);
}

void
print_vint8 (char *prefix, vui8_t val)
{
  printf (
      "%s%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\n",
      prefix, val[0], val[1], val[2], val[3], val[4], val[5], val[6], val[7],
      val[8], val[9], val[10], val[11], val[12], val[13], val[14], val[15]);
}

void
print_vint8c (char *prefix, vui8_t val)
{
  printf ("%s%c,%c,%c,%c,%c,%c,%c,%c,%c,%c,%c,%c,%c,%c,%c,%c\n", prefix, val[0],
	  val[1], val[2], val[3], val[4], val[5], val[6], val[7], val[8],
	  val[9], val[10], val[11], val[12], val[13], val[14], val[15]);
}

void
print_vint8x (char *prefix, vui8_t val)
{
  printf (
      "%s%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\n",
      prefix, val[0], val[1], val[2], val[3], val[4], val[5], val[6], val[7],
      val[8], val[9], val[10], val[11], val[12], val[13], val[14], val[15]);
}

void
print_vint8d (char *prefix, vui8_t val)
{
  printf ("%s%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d\n",
	  prefix, val[0], val[1], val[2], val[3], val[4], val[5], val[6],
	  val[7], val[8], val[9], val[10], val[11], val[12], val[13], val[14],
	  val[15]);
}

void
print_vchar (char *prefix, vui8_t val)
{
  printf ("%s%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", prefix, val[0], val[1],
	  val[2], val[3], val[4], val[5], val[6], val[7], val[8], val[9],
	  val[10], val[11], val[12], val[13], val[14], val[15]);
}

void
print_vbool8 (char *prefix, vui8_t val)
{
  const vui8_t UC_TRUE =
    { 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54,
	0x54, 0x54, 0x54, 0x54 };
  const vui8_t UC_FALSE =
    { 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46,
	0x46, 0x46, 0x46, 0x46 };

  vui8_t tpart, fpart, merge;

  tpart = vec_and (UC_TRUE, val);
  fpart = vec_andc (UC_FALSE, val);
  merge = vec_or (tpart, fpart);

  printf ("%s%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", prefix, merge[0], merge[1],
	  merge[2], merge[3], merge[4], merge[5], merge[6], merge[7], merge[8],
	  merge[9], merge[10], merge[11], merge[12], merge[13], merge[14],
	  merge[15]);
}

void
print_v4f32 (char *prefix, vf32_t val)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  printf ("%s %8.3f,%8.3f,%8.3f,%8.3f\n", prefix, val[3], val[2], val[1], val[0]);
#else
  printf ("%s %8.3f,%8.3f,%8.3f,%8.3f\n", prefix, val[0], val[1], val[2], val[3]);
#endif
}

void
print_v4f32x (char *prefix, vf32_t val)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  printf ("%s %16.6a,%16.6a,%16.6a,%16.6a\n", prefix, val[3], val[2], val[1], val[0]);
#else
  printf ("%s %16.6a,%16.6a,%16.6a,%16.6a\n", prefix, val[0], val[1], val[2], val[3]);
#endif
}

void
print_v4b32c (char *prefix, vb32_t val)
{
  const vui32_t true =  { 'T', 'T', 'T', 'T' };
  const vui32_t false = { 'F', 'F', 'F', 'F' };
  vui32_t text;

  text = vec_sel (false, true, val);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  printf ("%s %c,%c,%c,%c\n", prefix, text[3], text[2], text[1], text[0]);
#else
  printf ("%s %c,%c,%c,%c\n", prefix, text[0], text[1], text[2], text[3]);
#endif
}

void
print_v4b32x (char *prefix, vb32_t boolval)
{
  vui32_t val = (vui32_t)boolval;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  printf ("%s %08x,%08x,%08x,%08x\n", prefix, val[3], val[2], val[1], val[0]);
#else
  printf ("%s %08x,%08x,%08x,%08x\n", prefix, val[0], val[1], val[2], val[3]);
#endif
}

void
print_v2f64 (char *prefix, vf64_t val)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  printf ("%s %16.6f,%16.6f\n", prefix, val[1], val[0]);
#else
  printf ("%s %16.6f,%16.6f\n", prefix, val[0], val[1]);
#endif
}

void
print_v2f64x (char *prefix, vf64_t val)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  printf ("%s %22.13a,%22.13a\n", prefix, val[1], val[0]);
#else
  printf ("%s %22.13a,%22.13a\n", prefix, val[0], val[1]);
#endif
}

void
print_v2b64c (char *prefix, vb64_t val)
{
  const vui64_t true =  { 'T', 'T' };
  const vui64_t false = { 'F', 'F' };
  vui64_t text;

  text = vec_sel (false, true, val);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  printf ("%s %c,%c\n", prefix, (int)text[1], (int)text[0]);
#else
  printf ("%s %c,%c\n", prefix, (int)text[0], (int)text[1]);
#endif
}

void
print_v2b64x (char *prefix, vb64_t boolval)
{
  vui64_t val = (vui64_t)boolval;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  printf ("%s %016lx,%016lx\n", prefix, val[1], val[0]);
#else
  printf ("%s %016lx,%016lx\n", prefix, val[0], val[1]);
#endif
}

void
print_v2int64 (char *prefix, vui64_t val128)
{
  vui64_t val = (vui64_t) val128;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  printf ("%s %ld,%ld\n", prefix, val[1], val[0]);
#else
  printf ("%s %ld,%ld\n", prefix, val[0], val[1]);
#endif
}

void
print_v2xint64 (char *prefix, vui64_t val128)
{
  vui64_t val = (vui64_t) val128;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  printf ("%s %016lx,%016lx\n", prefix, val[1], val[0]);
#else
  printf ("%s %016lx,%016lx\n", prefix, val[0], val[1]);
#endif
}

void
print_vint128 (char *prefix, vui128_t val128)
{
  __VEC_U_128 val;
  unsigned __int128
  val_x;
  val.vx1 = val128;
  val_x = val.i128;
  print_uint128 (prefix, val_x);
}

void
print_vint128x (char *prefix, vui128_t val128)
{
  vui32_t val = (vui32_t) val128;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  printf ("%s%08x%08x%08x%08x\n", prefix, val[3], val[2], val[1], val[0]);
#else
  printf("%s%08x%08x%08x%08x\n", prefix, val[0], val[1], val[2], val[3]);
#endif
}

void
print_vint256 (char *prefix, vui128_t val0_128, vui128_t val1_128)
{
  vui32_t val0 = (vui32_t) val0_128;
  vui32_t val1 = (vui32_t) val1_128;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  printf ("%s%08x%08x%08x%08x %08x%08x%08x%08x\n", prefix,
	  val0[3], val0[2], val0[1], val0[0],
	  val1[3], val1[2], val1[1], val1[0]);
#else
  printf ("%s%08x%08x%08x%08x %08x%08x%08x%08x\n", prefix, val0[0], val0[1],
          val0[2], val0[3], val1[0], val1[1], val1[2], val1[3]);
#endif
}

void
print_vint384 (char *prefix, vui128_t val0_128, vui128_t val1_128,
               vui128_t val2_128)
{
  vui32_t val0 = (vui32_t) val0_128;
  vui32_t val1 = (vui32_t) val1_128;
  vui32_t val2 = (vui32_t) val2_128;
  printf ("%s%08x%08x%08x%08x %08x%08x%08x%08x %08x%08x%08x%08x\n", prefix,
          val0[0], val0[1], val0[2], val0[3], val1[0], val1[1], val1[2],
          val1[3], val2[0], val2[1], val2[2], val2[3]);
}

void
print_vint128x_sum (char *prefix, vui32_t r, vui32_t a, vui32_t b)
{
  printf ("vec %s\n", prefix);
  print_vint128x ("  a = ", (vui128_t) a);
  print_vint128x ("  b = ", (vui128_t) b);
  print_vint128x ("  r = ", (vui128_t) r);
}

void
print_vint128_sum (char *prefix, vui32_t r, vui32_t a, vui32_t b)
{
  printf ("vec %s\n", prefix);
  print_vint128 ("  a = ", (vui128_t) a);
  print_vint128 ("  b = ", (vui128_t) b);
  print_vint128 ("  r = ", (vui128_t) r);
}

void
print_vint128x_carry (char *prefix, vui32_t r, vui32_t a, vui32_t b, vui32_t c)
{
  printf ("vec %s\n", prefix);
  print_vint128x ("  a = ", (vui128_t) a);
  print_vint128x ("  b = ", (vui128_t) b);
  print_vint128x ("  c = ", (vui128_t) c);
  print_vint128x ("  r = ", (vui128_t) r);
}

void
print_vint128_carry (char *prefix, vui32_t r, vui32_t a, vui32_t b, vui32_t c)
{
  printf ("vec %s\n", prefix);
  print_vint128 ("  a = ", (vui128_t) a);
  print_vint128 ("  b = ", (vui128_t) b);
  print_vint128 ("  c = ", (vui128_t) c);
  print_vint128 ("  r = ", (vui128_t) r);
}

void
print_vint128_prod (char *prefix, vui32_t r, vui32_t a, vui32_t b, vui32_t c)
{
  printf ("vec %s\n", prefix);
  print_vint128 ("  a = ", (vui128_t) a);
  print_vint128 ("  b = ", (vui128_t) b);
  print_vint128 (" *u = ", (vui128_t) c);
  print_vint128 (" *l = ", (vui128_t) r);
}

void
print_vint128_extend (char *prefix, vui32_t r, vui32_t co, vui32_t a, vui32_t b,
                      vui32_t c)
{
  printf ("vec %s\n", prefix);
  print_vint128 ("  a = ", (vui128_t) a);
  print_vint128 ("  b = ", (vui128_t) b);
  print_vint128 (" ci = ", (vui128_t) c);
  print_vint128 (" co = ", (vui128_t) co);
  print_vint128 ("  r = ", (vui128_t) r);
}

void
print_vint128x_extend (char *prefix, vui32_t r, vui32_t co, vui32_t a,
                       vui32_t b, vui32_t c)
{
  printf ("vec %s\n", prefix);
  print_vint128 ("  a = ", (vui128_t) a);
  print_vint128 ("  b = ", (vui128_t) b);
  print_vint128 (" ci = ", (vui128_t) c);
  print_vint128 (" co = ", (vui128_t) co);
  print_vint128 ("  r = ", (vui128_t) r);
}

void
print_vfloat128x (char *prefix, __float128 val128)
{
  vui32_t val;
  vui16_t vh;
  const vui32_t vzero = { 0, 0, 0, 0 };
  int sign, bexp, exp, hidden;
  char signc;

  vh = vec_xfer_bin128_2_vui16t (val128);
  bexp = vh[VEC_HW_H];
  vh[VEC_HW_H] = 0;
  val = (vui32_t) vh;

  sign = bexp >> 15;
  signc = sign ? '-' : '+';
  bexp = bexp & 0x7fff;
  exp = bexp - 16383;
  hidden = 0;
#if 0
  printf ("\tsign=%d, bexp=%04x, exp=%d\n", sign, bexp, exp);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  printf("\t%08x%08x%08x%08x\n", val[3], val[2], val[1], val[0]);
#else
  printf("\t%08x%08x%08x%08x\n", val[0], val[1], val[2], val[3]);
#endif
#endif

  if (bexp != 0)
    {
      hidden = 1;
    }
  else
    {
      if (vec_any_gt (val, vzero))
        {
          exp = -16382;
        }
      else
        {
          /* float 0.0  */
          exp = 0;
        }
    }

  if (bexp != 0x7fff)
    {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
      printf ("%s %c%d.%04x%08x%08x%08xp%+d\n", prefix, signc, hidden, val[3],
              val[2], val[1], val[0], exp);
#else
      printf("%s %c%d.%04x%08x%08x%08xp%+d\n", prefix, signc, hidden,
          val[0], val[1], val[2], val[3], exp);
#endif
    }
  else
    {
      if (val[0] | val[1] | val[2] | val[3])
        {
          if (val[VEC_W_H] & 0x00008000)
            {
              printf ("%s qNaN\n", prefix);
            }
          else
            {
              printf ("%s sNaN\n", prefix);
            }
        }
      else
        {
          printf ("%s %cinf\n", prefix, signc);
        }
    }
}

int
check_isf128_priv (char *prefix, __float128 val128, int val, int shouldbe)
{
  int rc = 0;
  if (val != shouldbe)
    {
      rc = 1;
      print_vfloat128x (prefix, val128);
      printf ("\tshould be: %d\n", shouldbe);
      printf ("\t       is: %d\n", val);
    }

  return (rc);
}

int
check_vuint128c_priv (char *prefix, vui128_t val128, vui128_t carry128,
                      vui128_t shouldbe, vui128_t shouldcarry)
{
  int rc = 0;
  if (vec_any_ne ((vui32_t) val128, (vui32_t) shouldbe)
      || vec_any_ne ((vui32_t) carry128, (vui32_t) shouldcarry))
    {
      rc = 1;
      printf ("%s\n", prefix);
      print_vint128 ("\tshould be: ", shouldbe);
      print_vint128 ("\tshould c : ", shouldcarry);
      print_vint128 ("\t       is: ", val128);
      print_vint128 ("\t is carry: ", carry128);
    }

  return (rc);
}

int
check_vint256_priv (char *prefix, vui128_t val0_128, vui128_t val1_128,
                    vui128_t sb0_128, vui128_t sb1_128)
{
  int rc = 0;
  if (vec_any_ne ((vui32_t) val0_128, (vui32_t) sb0_128)
      || vec_any_ne ((vui32_t) val1_128, (vui32_t) sb1_128))
    {
      rc = 1;
      printf ("%s\n", prefix);
      print_vint256 ("\tshould be: ", sb0_128, sb1_128);
      print_vint256 ("\t       is: ", val0_128, val1_128);
    }

  return (rc);
}

int
check_vint384_priv (char *prefix, vui128_t val0_128, vui128_t val1_128,
                    vui128_t val2_128, vui128_t sb0_128, vui128_t sb1_128,
                    vui128_t sb2_128)
{
  int rc = 0;
  if (vec_any_ne ((vui32_t) val0_128, (vui32_t) sb0_128)
      || vec_any_ne ((vui32_t) val1_128, (vui32_t) sb1_128)
      || vec_any_ne ((vui32_t) val2_128, (vui32_t) sb2_128))
    {
      rc = 1;
      printf ("%s\n", prefix);
      print_vint384 ("\tshould be: ", sb0_128, sb1_128, sb2_128);
      print_vint384 ("\t       is: ", val0_128, val1_128, val2_128);
    }

  return (rc);
}

int
check_vui8_priv (char *prefix, vui8_t val128, vui8_t shouldbe)
{
  int rc = 0;
  if (vec_any_ne((vui8_t )val128, (vui8_t )shouldbe))
    {
      rc = 1;
      printf ("%s\n", prefix);
      print_vint8x ("\tshould be: ", shouldbe);
      print_vint8x ("\t       is: ", val128);
    }

  return (rc);
}

int
check_v4b32c_priv (char *prefix, vb32_t val128, vb32_t shouldbe)
{
  int rc = 0;


  if (vec_any_ne ((vui32_t) val128, (vui32_t) shouldbe))
    {
      rc = 1;
      printf ("%s\n", prefix);
      print_v4b32c ("\tshould be: ", shouldbe);
      print_v4b32c ("\t       is: ", val128);
    }

  return (rc);
}

int
check_v4b32x_priv (char *prefix, vb32_t val128, vb32_t shouldbe)
{
  int rc = 0;


  if (vec_any_ne ((vui32_t) val128, (vui32_t) shouldbe))
    {
      rc = 1;
      printf ("%s\n", prefix);
      print_v4b32x ("\tshould be: ", shouldbe);
      print_v4b32x ("\t       is: ", val128);
    }

  return (rc);
}

int
check_v4f32_priv (char *prefix, vf32_t val128, vf32_t shouldbe)
{
  int rc = 0;


  if (vec_any_ne ((vui32_t) val128, (vui32_t) shouldbe))
    {
      rc = 1;
      printf ("%s\n", prefix);
      print_v4f32 ("\tshould be: ", shouldbe);
      print_v4f32 ("\t       is: ", val128);
    }

  return (rc);
}

int
check_v4f32x_priv (char *prefix, vf32_t val128, vf32_t shouldbe)
{
  int rc = 0;


  if (vec_any_ne ((vui32_t) val128, (vui32_t) shouldbe))
    {
      rc = 1;
      printf ("%s\n", prefix);
      print_v4f32x ("\tshould be: ", shouldbe);
      print_v4f32x ("\t       is: ", val128);
    }

  return (rc);
}

int
check_v2b64c_priv (char *prefix, vb64_t val128, vb64_t shouldbe)
{
  int rc = 0;


  if (vec_any_ne ((vui32_t) val128, (vui32_t) shouldbe))
    {
      rc = 1;
      printf ("%s\n", prefix);
      print_v2b64c ("\tshould be: ", shouldbe);
      print_v2b64c ("\t       is: ", val128);
    }

  return (rc);
}

int
check_v2b64x_priv (char *prefix, vb64_t val128, vb64_t shouldbe)
{
  int rc = 0;


  if (vec_any_ne ((vui32_t) val128, (vui32_t) shouldbe))
    {
      rc = 1;
      printf ("%s\n", prefix);
      print_v2b64x ("\tshould be: ", shouldbe);
      print_v2b64x ("\t       is: ", val128);
    }

  return (rc);
}

int
check_v2f64_priv (char *prefix, vf64_t val128, vf64_t shouldbe)
{
  int rc = 0;


  if (vec_any_ne ((vui32_t) val128, (vui32_t) shouldbe))
    {
      rc = 1;
      printf ("%s\n", prefix);
      print_v2f64 ("\tshould be: ", shouldbe);
      print_v2f64 ("\t       is: ", val128);
    }

  return (rc);
}

int
check_v2f64x_priv (char *prefix, vf64_t val128, vf64_t shouldbe)
{
  int rc = 0;


  if (vec_any_ne ((vui32_t) val128, (vui32_t) shouldbe))
    {
      rc = 1;
      printf ("%s\n", prefix);
      print_v2f64x ("\tshould be: ", shouldbe);
      print_v2f64x ("\t       is: ", val128);
    }

  return (rc);
}

int
check_vuint128_priv (char *prefix, vui128_t val128, vui128_t shouldbe)
{
  int rc = 0;
  if (vec_any_ne ((vui32_t) val128, (vui32_t) shouldbe))
    {
      rc = 1;
      printf ("%s\n", prefix);
      print_vint128 ("\tshould be: ", shouldbe);
      print_vint128 ("\t       is: ", val128);
    }

  return (rc);
}

int
check_vuint128x_priv (char *prefix, vui128_t val128, vui128_t shouldbe)
{
  int rc = 0;
  if (vec_any_ne ((vui32_t) val128, (vui32_t) shouldbe))
    {
      rc = 1;
      printf ("%s\n", prefix);
      print_vint128x ("\tshould be: ", shouldbe);
      print_vint128x ("\t       is: ", val128);
    }

  return (rc);
}

int
check_uint128_priv (char *prefix, unsigned __int128 val128, unsigned __int128 shouldbe)
  {
    int rc = 0;
    if (val128 != shouldbe)
      {
        rc = 1;
        printf ("%s\n", prefix);
        print_uint128 ("\tshould be: ", shouldbe);
        print_uint128 ("\t       is: ", val128);
      }

    return (rc);
  }

int
check_int128_priv (char *prefix, __int128 val128, __int128 shouldbe)
{
  int rc = 0;
  if (val128 != shouldbe)
    {
      rc = 1;
      printf ("%s\n", prefix);
      print_int128 ("\tshould be: ", shouldbe);
      print_int128 ("\t       is: ", val128);
    }

  return (rc);
}

int
check_uint64 (char *prefix, unsigned long val64, unsigned long shouldbe)
{
  int rc = 0;
  if (val64 != shouldbe)
    {
      rc = 1;
      printf ("%s", prefix);
      printf ("\tshould be: %20lu", shouldbe);
      printf ("\t       is: %20lu\n", val64);
    }

  return (rc);
}

int
check_int64 (char *prefix, long val64, long shouldbe)
{
  int rc = 0;
  if (val64 != shouldbe)
    {
      rc = 1;
      printf ("%s", prefix);
      printf ("\tshould be: %20ld", shouldbe);
      printf ("\t       is: %20ld\n", val64);
    }

  return (rc);
}
// TODO make this hidden so we can use TOC relative
extern const _Decimal128 decpowof2[];

void
print_dfp128p2 (char *prefix, _Decimal128 val128, long exp)
{
  _Decimal128 x = val128;
  if (exp > 0)
    {
      x = val128 * decpowof2[exp];
    }
  else
    {
      if (exp < 0)
        {
          x = val128 / decpowof2[-exp];
        }
    }
  printf ("%s % .33DDe\n", prefix, x);
}

int
check_f128bool_priv (char *prefix, __float128 val128, __f128_bool boolis,
                     __f128_bool shouldbe)
{
  int rc = 0;
  if (!vec_all_eq (boolis, shouldbe))
    {
      rc = 1;
      print_vfloat128x (prefix, val128);
      print_vint128x ("\tshould be: ", (vui128_t) shouldbe);
      print_vint128x ("\t       is: ", (vui128_t) boolis);
    }

  return (rc);
}

int
check_frexptftd_priv (char *prefix, _Decimal128 val128, long exp,
                      _Decimal128 shouldbe, long shouldexp)
{
  int rc = 0;
  if ((val128 != shouldbe) || (exp != shouldexp))
    {
      rc = 1;
      printf ("%s\n", prefix);
      print_dfp128 ("\tshould be: ", shouldbe);
      printf ("\t      exp: %ld\n", shouldexp);
      print_dfp128 ("\t       is: ", val128);
      printf ("\t      exp: %ld\n", exp);

      print_dfp128p2 ("\t  decpow2: ", val128, exp);
    }

  return (rc);
}

int
check_frexptfkf_priv (char *prefix, vui128_t val128, long exp,
                      vui128_t shouldbe, long shouldexp)
{
  int rc = 0;
  if ((!vec_all_eq ((vui32_t) val128, (vui32_t) shouldbe))
      || (exp != shouldexp))
    {
      rc = 1;
      printf ("%s\n", prefix);
      print_vint128 ("\tshould be: ", shouldbe);
      printf ("\t      exp: %ld\n", shouldexp);
      print_vint128 ("\t       is: ", val128);
      printf ("\t      exp: %ld\n", exp);
    }

  return (rc);
}

int
check_kf_priv (char *prefix, vui128_t val128, vui128_t shouldbe)
{
  int rc = 0;
  if (!vec_all_eq ((vui32_t) val128, (vui32_t) shouldbe))
    {
      rc = 1;
      printf ("%s\n", prefix);
      print_vint128 ("\tshould be: ", shouldbe);
      print_vint128 ("\t       is: ", val128);
    }

  return (rc);
}

int
check_frexptfvti_priv (char *prefix, vui128_t val128, long exp,
                       vui128_t shouldbe, long shouldexp)
{
  int rc = 0;
  if ((!vec_all_eq ((vui32_t) val128, (vui32_t) shouldbe))
      || (exp != shouldexp))
    {
      rc = 1;
      printf ("%s\n", prefix);
      print_vint128 ("\tshould be: ", shouldbe);
      printf ("\t      exp: %ld\n", shouldexp);
      print_vint128 ("\t       is: ", val128);
      printf ("\t      exp: %ld\n", exp);
    }

  return (rc);
}

int
check_frexptfti_priv (char *prefix, __int128 val128, long exp,
                      __int128 shouldbe, long shouldexp)
{
  int rc = 0;
  if ((val128 != shouldbe) || (exp != shouldexp))
    {
      rc = 1;
      printf ("%s\n", prefix);
      print_int128x ("\tshould be: ", shouldbe);
      printf ("\t      exp: %ld\n", shouldexp);
      print_int128x ("\t       is: ", val128);
      printf ("\t      exp: %ld\n", exp);
    }

  return (rc);
}


