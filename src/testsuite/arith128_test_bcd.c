/*
 * arith128_test_bcd.c
 *
 *  Created on: Apr 6, 2018
 *      Author: sjmunroe
 */
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

 arith128_test_bcd.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Apr 6, 2018
 */

#define __STDC_WANT_DEC_FP__    1

#include <stdint.h>
#include <stdio.h>
#include <fenv.h>
#include <float.h>
#include <math.h>

//#define __DEBUG_PRINT__
#include <vec_common_ppc.h>
#include <vec_bcd_ppc.h>

#include "arith128.h"
#include <testsuite/arith128_print.h>

#include <testsuite/arith128_test_bcd.h>

vui8_t
db_vec_BCD2i128 (vui8_t bcd32)
{
  vui8_t result = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  vui8_t e_perm =
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    { 0x00, 0x10, 0x02, 0x12, 0x04, 0x14, 0x06, 0x16, 0x08, 0x18, 0x0a, 0x1a,
	0x0c, 0x1c, 0x0e, 0x1e };
#else
    { 0x01, 0x11, 0x03, 0x13, 0x05, 0x15, 0x07, 0x17, 0x09, 0x19, 0x0b, 0x1b,
	0x0d, 0x1d, 0x0f, 0x1f };
#endif
  vui8_t /*i,*/ j, k, l, m/**/;

  print_vint8x ("32xBCD     ", bcd32);
  print_vint8d ("           ", bcd32);

  j = vec_srbi(bcd32, 4);
  print_vint8x ("BCD >> 4   ", j);
  print_vint8d (" high digit", j);

  k = vec_splat_u8 ((unsigned char)0x06);
  l = (vui8_t)vec_mule (j, k);
  m = (vui8_t)vec_mulo (j, k);
  print_vint8x (" hd*6 even ", l);
  print_vint8d ("           ", l);
  print_vint8x (" hd*6 odd  ", m);
  print_vint8d ("           ", m);
#if 0
  j = vec_vpkuhum ((vui16_t)l, (vui16_t)m);
  print_vint8x (" hd*6 pack ", j);
  print_vint8d ("           ", j);
#else
  j = vec_perm (l, m, e_perm);
  print_vint8x (" hd*6 perm ", j);
  print_vint8d ("           ", j);
#endif
  result = vec_sub (bcd32, j);
  print_vint8x ("BCD - hd*6 ", result);
  print_vint8d ("           ", result);

  return result;
}

vui16_t
db_vec_BC100s2i128 (vui8_t bc100s16)
{
  vui16_t result = { 0,0,0,0,0,0,0,0};
  vui8_t k;
  vui16_t l;

  print_vint8d  ("16xBC100s  ", bc100s16);
  print_vint16d ("           ", (vui16_t)bc100s16);

  k = vec_splats ((unsigned char)156);
  l = vec_vmuleub (bc100s16, k);
  print_vint16x (" hd*156 ev ", l);
  print_vint16d ("           ", l);

  result = vec_sub ((vui16_t)bc100s16, l);
  print_vint16x ("BCD-hd*156 ", result);
  print_vint16d ("           ", result);
  return result;
}

vui32_t
db_vec_BC10ks2i128 (vui16_t bc10k8)
{
  vui32_t result = { 0,0,0,0};
  vui16_t k;
  vui32_t l;

  print_vint16d ("8xBC10ks   ", bc10k8);
  print_vint32d ("           ", (vui32_t)bc10k8);

  k = vec_splats ((unsigned short)55536);
  l = vec_vmuleuh (bc10k8, k);
  print_vint32x (" hd*156 ev ", l);
  print_vint32d ("           ", l);

  result = vec_sub ((vui32_t)bc10k8, l);
  print_vint32x ("BCD-hd*10kc", result);
  print_vint32d ("           ", result);
  return result;
}

vui64_t
db_vec_BC100ms2i128 (vui32_t bc100m4)
{
  vui64_t result = { 0,0};
  vui32_t k;
  vui64_t l;

  print_vint32d ("4xBC100ms  ", bc100m4);
  print_v2int64 ("           ", (vui64_t)bc100m4);

  k = vec_splats ((unsigned int)4194967296);
#ifdef vec_vmuleuw
  l = vec_vmuleuw (bc100m4, k);
#else
  l = vec_muleuw (bc100m4, k);
#endif
  print_v2xint64 (" hd*100mc e", l);
  print_v2int64  ("           ", l);

  result = vec_subudm ((vui64_t)bc100m4, l);
  print_v2xint64 ("BCD-hd*10T ", result);
  print_v2int64  ("           ", result);
  return result;
}

vui128_t
db_vec_BC10ts2i128 (vui64_t bc10t2)
{
  vui128_t result;
  vui64_t k;
  vui128_t l;
//  vui64_t cvt10t = {0xFFDC790D903F0000UL, 0xFFDC790D903F0000UL};

  print_v2int64 ("2xBC10ts   ", (vui64_t)bc10t2);
  print_v2xint64("           ", (vui64_t)bc10t2);
  print_vint128 ("           ", (vui128_t)bc10t2);

#if 0
  vui128_t v10_16 = (vui128_t)CONST_VINT128_DW (0, 10000000000000000UL);
  vui128_t v2_64 = (vui128_t)CONST_VINT128_DW (1, 0);
  print_vint128x (" v2_64     ", v2_64);
  print_vint128  ("           ", v2_64);
  print_vint128x (" v10_16    ", v10_16);
  print_vint128  ("           ", v10_16);

  l = vec_sub (v2_64, v10_16);
  print_vint128x (" cvt*10t   ", l);
  print_vint128  ("           ", l);
#endif
  k = vec_splats ((unsigned long)0xFFDC790D903F0000UL);
#if 0
#ifdef vec_vmuleud
  l = vec_vmuleud (bc10t2, k);
#else
  l = vec_muleud (bc10t2, k);
  k = (vui64_t)l;
#endif
#else
  print_vint128x (" cvt*10t k ", (vui128_t)k);

  l = vec_srqi ((vui128_t)bc10t2, 64);

  print_vint128x (" bc10t2>>64", l);
  print_vint128  ("           ", l);
  l = vec_mulluq (l, (vui128_t)k);
#endif
  print_vint128x (" hd*10t ev ", l);
  print_vint128  ("           ", l);

  result = vec_subuqm ((vui128_t)bc10t2, l);
  print_vint128x ("BCD-hd*10x ", result);
  print_vint128  ("           ", result);
  return result;
}

//#define __DEBUG_PRINT__ 1
#ifdef __DEBUG_PRINT__
#define test_vec_bcdctb100s(_l)	db_vec_BCD2i128(_l)
#else
#define test_vec_bcdctb100s(_l)	vec_bcdctb100s(_l)
#endif

int
test_48 (void)
{
  vui8_t i, j/*, k, l, m*/;
  vui16_t /*i, j,*/ k/*, l, m*/;
  vui32_t l;
  vui64_t m;
  vui128_t n;
  int rc = 0;

  i = (vui8_t){0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99};
  j = test_vec_bcdctb100s (i);
  print_vint8x ("BCD 9s     ", i);
  print_vint8d ("           ", i);
  print_vint8x ("BC100s     ", j);
  print_vint8d ("           ", j);

  k = db_vec_BC100s2i128 (j);
  print_vint16d ("BC10Ks     ", k);

  l = db_vec_BC10ks2i128 (k);
  print_vint32d ("BC100Ms    ", l);

  m = db_vec_BC100ms2i128 (l);
  print_v2int64 ("BC10Ts     ", m);

  n = db_vec_BC10ts2i128 (m);
  print_vint128 ("BCD10x     ", n);

  return (rc);
}

int
test_cvtbcd2c100 (void)
{
  vui8_t i, j/*, k, l, m*/;
  vui8_t e;
  int rc = 0;

  printf ("\n%s Vector BCD convert\n", __FUNCTION__);

  i = (vui8_t){0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99};
  e = (vui8_t){99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99};
  j = test_vec_bcdctb100s (i);

#ifdef __DEBUG_PRINT__
  print_vint8x ("BCD 9s     ", i);
  print_vint8d ("           ", i);
  print_vint8x ("BC100s     ", j);
  print_vint8d ("           ", j);
#endif
  rc += check_vuint128x ("vec_bcdctb100s:", (vui128_t) j, (vui128_t) e);

  i = (vui8_t){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  e = (vui8_t){00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00};
  j = test_vec_bcdctb100s (i);

#ifdef __DEBUG_PRINT__
  print_vint8x ("BCD 0s     ", i);
  print_vint8d ("           ", i);
  print_vint8x ("BC100s     ", j);
  print_vint8d ("           ", j);
#endif
  rc += check_vuint128x ("vec_bcdctb100s:", (vui128_t) j, (vui128_t) e);

  i = (vui8_t){0x01, 0x09, 0x10, 0x19, 0x20, 0x29, 0x30, 0x39, 0x40, 0x49, 0x50, 0x59, 0x60, 0x69, 0x70, 0x79};
  e = (vui8_t){1, 9, 10, 19, 20, 29, 30, 39, 40, 49, 50, 59, 60, 69, 70, 79};
  j = test_vec_bcdctb100s (i);

#ifdef __DEBUG_PRINT__
  print_vint8x ("BCD 01-79  ", i);
  print_vint8d ("           ", i);
  print_vint8x ("BC100s     ", j);
  print_vint8d ("           ", j);
#endif
  rc += check_vuint128x ("vec_bcdctb100s:", (vui128_t) j, (vui128_t) e);

  i = (vui8_t){0x80, 0x81, 0x82, 0x83, 0x86, 0x87, 0x88, 0x89, 0x90, 0x91, 0x92, 0x93, 0x95, 0x96, 0x97, 0x98};
  e = (vui8_t){80, 81, 82, 83, 86, 87, 88, 89, 90, 91, 92, 93, 95, 96, 97, 98};
  j = test_vec_bcdctb100s (i);

#ifdef __DEBUG_PRINT__
  print_vint8x ("BCD 80-98  ", i);
  print_vint8d ("           ", i);
  print_vint8x ("BC100s     ", j);
  print_vint8d ("           ", j);
#endif
  rc += check_vuint128x ("vec_bcdctb100s:", (vui128_t) j, (vui128_t) e);

  return (rc);
}
//#undef __DEBUG_PRINT__

int
test_bcd_addsub (void)
{
  vui32_t i, j, k /*, l, m*/;
  vui32_t e, ex;
  int rc = 0;

  printf ("\n%s Vector BCD +-\n", __FUNCTION__);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x0000001c);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x0000001c);
  k = vec_bcdadd (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (1+1)", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x2c);
  rc += check_vuint128x ("vec_bcdadd:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x9999999c);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x0000001c);
  k = vec_bcdadd (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (9999999+1)", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0, 0, 0x1, 0x0000000c);
  rc += check_vuint128x ("vec_bcdadd:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x0000001c);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x0000001c);
  k = vec_bcdsub (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (1-1)", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x0000000c);
  ex = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x0000000d);
  if (vec_all_eq (k, ex))
    {
      printf ("vec_bcdsub: ignore negative zero. Likely QEMU artifact\n");
      k = e;
    }
  rc += check_vuint128x ("vec_bcdsub:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x9999999c);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x0000001c);
  k = vec_bcdsub (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (9999999-1)", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x9999998c);
  rc += check_vuint128x ("vec_bcdsub:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x0000001c);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x9999999c);
  k = vec_bcdsub (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (1-9999999)", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x9999998d);
  rc += check_vuint128x ("vec_bcdsub:", (vui128_t) k, (vui128_t) e);

  return rc;
}


 int
 test_bcd_muldiv (void)
 {
  vui32_t i, j, k /*, l, m*/;
  vui32_t e;
  int rc = 0;

  printf ("\n%s Vector BCD */\n", __FUNCTION__);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x0000001c);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x9999999c);
  k = vec_bcdmul (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (1*9999999)", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x9999999c);
  rc += check_vuint128x ("vec_bcdmul:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x9999999c);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x9999999c);
  k = vec_bcdmul (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (9999999*9999999)", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0, 0, 0x9999998, 0x0000001c);
  rc += check_vuint128x ("vec_bcdmul:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0x99999999, 0x9999999c);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0x99999999, 0x9999999c);
  k = vec_bcdmul (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (999999999999999*999999999999999)", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0x09999999, 0x99999998, 0x00000000, 0x0000001c);
  rc += check_vuint128x ("vec_bcdmul:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0x09999999, 0x99999998, 0x00000000, 0x0000001c);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0x99999999, 0x9999999c);
  k = vec_bcddiv (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (999999999999998000000000000001/999999999999999)", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x99999999, 0x9999999c);
  rc += check_vuint128x ("vec_bcddiv:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0x99999999, 0x9999999c);
  j = (vui32_t )CONST_VINT32_W(0, 0x99999999, 0x99999999, 0x9999999c);
  k = vec_bcdmul (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (999999999999999*99999999999999999999999)", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x0000000c);
  rc += check_vuint128x ("vec_bcdmul:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0x00000001, 0x0000000c);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x3c);
  k = vec_bcddiv (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (10000000/3)", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x3333333c);
  rc += check_vuint128x ("vec_bcddiv:", (vui128_t) k, (vui128_t) e);

  return rc;
}

int
test_vec_bcd (void)
{
  int rc = 0;

  printf ("\n%s\n", __FUNCTION__);

  rc += test_bcd_addsub ();

  rc += test_bcd_muldiv ();

  rc += test_cvtbcd2c100 ();

//  rc += test_48 ();

  return (rc);
}
