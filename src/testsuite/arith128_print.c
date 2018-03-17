/*
 Copyright [2017] IBM Corporation.

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
#include "arith128_print.h"
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
print_vint8 (char *prefix, vui8_t val)
{
  printf (
      "%s%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\n",
      prefix, val[0], val[1], val[2], val[3], val[4], val[5], val[6], val[7],
      val[8], val[9], val[10], val[11], val[12], val[13], val[14], val[15]);
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
  const vui8_t UC_TRUE = { 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54,
      0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54 };
  const vui8_t UC_FALSE = { 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46,
      0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46 };

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
  printf ("%s%08x%08x%08x%08x %08x%08x%08x%08x\n", prefix, val0[0], val0[1],
          val0[2], val0[3], val1[0], val1[1], val1[2], val1[3]);
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

/*
 * udiv_qrnnd(quotient, remainder, high_numerator, low_numerator,
 denominator) divides a UDWtype, composed by the UWtype integers
 HIGH_NUMERATOR and LOW_NUMERATOR, by DENOMINATOR and places the quotient
 in QUOTIENT and the remainder in REMAINDER.  HIGH_NUMERATOR must be less
 than DENOMINATOR for correct operation.
 */

int64_t
db_sdiv_qrnnd (int64_t *remainder, int64_t high_num, int64_t low_num,
               int64_t denom)
{
  int64_t q1, q2, r1, r2, Q, R, abs_D;

  abs_D = __builtin_labs (denom);

  printf ("db_sdiv_qrnnd N= (%ld,%ld) (%016lx%016lx)\n", high_num, low_num,
          high_num, low_num);
  printf ("db_sdiv_qrnnd D= (%ld) (%016lx)\n", denom, denom);
  q1 = __builtin_divde (high_num, denom);
  r1 = -(q1 * denom);
#if 0
  q2 = low_num / denom;
#else
  q2 = ((uint64_t) low_num / (uint64_t) denom);
#endif
  r2 = low_num - (q2 * denom);
  Q = q1 + q2;
  R = r1 + r2;
  printf ("db_sdiv_qrnnd q= (%ld,%ld)\n", q1, q2);
  printf ("db_sdiv_qrnnd Q= (%ld) (%016lx)\n", Q, Q);
  printf ("db_sdiv_qrnnd r= (%ld,%ld)\n", r1, r2);
  printf ("db_sdiv_qrnnd R= (%ld) (%016lx)\n", R, R);
  printf ("db_sdiv_qrnnd (high_num >=0) (%d)\n", (int) (high_num >= 0));
  printf ("db_sdiv_qrnnd (R >= abs_D) (%d)\n", (int) (R >= abs_D));
  printf ("db_sdiv_qrnnd (R >= -abs_D) (%d)\n", (int) (R >= -abs_D));
  printf ("db_sdiv_qrnnd (R <= -abs_D) (%d)\n", (int) (R <= -abs_D));
  printf ("db_sdiv_qrnnd (N^R >>63) (%d)\n", (int) ((high_num ^ R) >> 63));
  printf ("db_sdiv_qrnnd (abs_R >= abs_D) (%d)\n",
          (int) (__builtin_labs (R) >= abs_D));
#if 0
  printf ("db_sdiv_qrnnd (R > -abs_D) (%d)\n", (int)(R > -abs_D));
  printf ("db_sdiv_qrnnd (r2 > -abs_D) (%d)\n", (int)(r2 > -abs_D));
#endif
  if (((high_num >= 0) && (R >= abs_D)) || ((high_num < 0) && (R >= -abs_D)))
    {
      Q += 1;
      R = R - denom;
      printf ("db_sdiv_qrnnd Q' (%ld) (%016lx)\n", Q, Q);
      printf ("db_sdiv_qrnnd R' (%ld) (%016lx)\n", R, R);
    }
  *remainder = R;
  return (Q);
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

/*
 * Return a vector boolean char with a true indicator for any character
 * that is either Lower Case Alpha ASCII or Upper Case ASCII.
 * False otherwise.
 */
vui8_t
db_vec_isalpha (vui8_t vec_str)
{
  vui8_t result;
  const vui8_t UC_FIRST = { 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
      0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40 };
  const vui8_t UC_LAST = { 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a,
      0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a };
  const vui8_t LC_FIRST = { 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60,
      0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60 };
  const vui8_t LC_LAST = { 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a,
      0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a };

  vui8_t cmp1, cmp2, cmp3, cmp4, cmask1, cmask2;

  printf ("db_vec_isalpha\n");
  print_vint8 ("vec_str = ", vec_str);

  cmp1 = (vui8_t) vec_cmpgt (vec_str, LC_FIRST);
  cmp2 = (vui8_t) vec_cmpgt (vec_str, LC_LAST);

  print_vint8 ("cmp1    = ", cmp1);
  print_vint8 ("cmp2    = ", cmp2);

  cmp3 = (vui8_t) vec_cmpgt (vec_str, UC_FIRST);
  cmp4 = (vui8_t) vec_cmpgt (vec_str, UC_LAST);

  print_vint8 ("cmp3    = ", cmp3);
  print_vint8 ("cmp4    = ", cmp4);

  cmask1 = vec_andc (cmp1, cmp2);
  cmask2 = vec_andc (cmp3, cmp4);

  print_vint8 ("lcmask1 = ", cmask1);
  print_vint8 ("ucmask2 = ", cmask2);

  result = vec_or (cmask1, cmask2);

  print_vint8 ("result  = ", result);

  return (result);
}

/*
 * Convert any Lower Case Alpha ASCII characters within a vector
 * unsigned char into the equivalent Upper Case character.
 * Return the result as a vector unsigned char.
 */
vui8_t
db_vec_toupper (vui8_t vec_str)
{
  vui8_t result;
  const vui8_t UC_MASK = { 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
      0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20 };
  const vui8_t LC_FIRST = { 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60,
      0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60 };
  const vui8_t LC_LAST = { 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a,
      0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a };

  vui8_t cmp1, cmp2, cmask;

  printf ("db_vec_tolower\n");
  print_vint8 ("vec_str = ", vec_str);

  cmp1 = (vui8_t) vec_cmpgt (vec_str, LC_FIRST);
  cmp2 = (vui8_t) vec_cmpgt (vec_str, LC_LAST);

  print_vint8 ("cmp1    = ", cmp1);
  print_vint8 ("cmp2    = ", cmp2);

  cmask = vec_andc (cmp1, cmp2);
  cmask = vec_and (cmask, UC_MASK);

  print_vint8 ("cmask   = ", cmask);

  result = vec_andc (vec_str, cmask);

  print_vint8 ("result  = ", result);

  return (result);
}
/*
 * Convert any Upper Case Alpha ASCII characters within a vector
 * unsigned char into the equivalent Lower Case character.
 * Return the result as a vector unsigned char.
 */
vui8_t
db_vec_tolower (vui8_t vec_str)
{
  vui8_t result;
  const vui8_t UC_MASK = { 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
      0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20 };
  const vui8_t UC_FIRST = { 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
      0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40 };
  const vui8_t UC_LAST = { 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a,
      0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a };

  vui8_t cmp1, cmp2, cmask;

  printf ("db_vec_tolower\n");
  print_vint8 ("vec_str = ", vec_str);

  cmp1 = (vui8_t) vec_cmpgt (vec_str, UC_FIRST);
  cmp2 = (vui8_t) vec_cmpgt (vec_str, UC_LAST);

  print_vint8 ("cmp1    = ", cmp1);
  print_vint8 ("cmp2    = ", cmp2);

  cmask = vec_andc (cmp1, cmp2);
  cmask = vec_and (cmask, UC_MASK);

  print_vint8 ("cmask   = ", cmask);

  result = vec_or (vec_str, cmask);

  print_vint8 ("result  = ", result);

  return (result);
}

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
#warning Implememention pre power8 missing
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
  vui32_t t_odd, t_even, t_carry, t_high;
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
