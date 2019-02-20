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

#include <testsuite/arith128_print.h>
#include <testsuite/arith128_test_bcd.h>

vui8_t
db_vec_ZN2i128 (vui8_t zone00, vui8_t zone16)
{
  vui8_t result = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  const vui8_t dmask = vec_splat_u8 (15);
  const vui8_t dx10 = vec_splat_u8 (10);
  vui8_t znd00, znd16;
  vui8_t ones, tens;
  vui16_t ten00, ten16;
  vui8_t e_perm =
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    { 0x00, 0x10, 0x02, 0x12, 0x04, 0x14, 0x06, 0x16, 0x08, 0x18, 0x0a, 0x1a,
	0x0c, 0x1c, 0x0e, 0x1e };
#else
    { 0x01, 0x11, 0x03, 0x13, 0x05, 0x15, 0x07, 0x17, 0x09, 0x19, 0x0b, 0x1b,
	0x0d, 0x1d, 0x0f, 0x1f };
#endif
  vui8_t /*i,*/ j, k, l, m/**/;

  print_vint8x ("32xZoned   ", zone00);
  print_vint8x ("           ", zone16);
  print_vint8c ("32xZoned  c", zone00);
  print_vint8c ("           ", zone16);

  znd00 = vec_and (zone00, dmask);
  znd16 = vec_and (zone16, dmask);
  print_vint8x ("32xZD      ", znd00);
  print_vint8x ("           ", znd16);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  ones = vec_pack ((vui16_t) znd16, (vui16_t) znd00);
#else
  ones = vec_pack ((vui16_t) znd00, (vui16_t) znd16);
#endif
  print_vint8x ("ones       ", ones);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  ten00 = vec_mulo (znd00, dx10);
  ten16 = vec_mulo (znd16, dx10);
  tens = vec_pack (ten16, ten00);
#else
  ten00 = vec_mule (znd00, dx10);
  ten16 = vec_mule (znd16, dx10);
  tens = vec_pack (ten00, ten16);
#endif
  print_vint8x ("tenx10     ", (vui8_t) ten00);
  print_vint8x ("           ", (vui8_t) ten16);
  print_vint8x ("tenxpack   ", tens);

  result = vec_add (tens, ones);
  print_vint8x ("Zone 100s  ", result);
  print_vint8d ("           ", result);

  return result;
}

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
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  l = vec_vmuloub (bc100s16, k);
#else
  l = vec_vmuleub (bc100s16, k);
#endif
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
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  l = vec_vmulouh (bc10k8, k);
#else
  l = vec_vmuleuh (bc10k8, k);
#endif
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
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  l = vec_mulouw (bc100m4, k);
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
db_vec_BC10es2i128 (vui64_t bc10t2)
{
  vui128_t result;
  vui64_t k;
  vui128_t l;
//  vui64_t cvt10t = {0xFFDC790D903F0000UL, 0xFFDC790D903F0000UL};

  print_v2int64 ("2xBC10ts   ", (vui64_t) bc10t2);
  print_v2xint64("           ", (vui64_t) bc10t2);
  print_vint128 ("           ", (vui128_t) bc10t2);

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
  /* k = 18436744073709551616UL */
  k = vec_splats ((unsigned long)0xFFDC790D903F0000UL);
#if 1
  l = vec_vmuleud (bc10t2, k);
#else
  print_vint128x (" cvt*10t k ", (vui128_t) k);

  l = vec_srqi ((vui128_t)bc10t2, 64);

  print_vint128x (" bc10t2>>64", l);
  print_vint128  ("           ", l);
  l = vec_mulluq (l, (vui128_t) k);
#endif
  print_vint128x (" hd*10t ev ", l);
  print_vint128  ("           ", l);

  result = vec_subuqm ((vui128_t) bc10t2, l);
  print_vint128x ("BCD-hd*10x ", result);
  print_vint128  ("           ", result);
  return result;
}

//#define __DEBUG_PRINT__ 1
#ifdef __DEBUG_PRINT__
#define test_vec_bcdctb100s(_l)	db_vec_BCD2i128(_l)
#else
#define test_vec_bcdctb100s(_l)	vec_rdxct100b(_l)
#endif

int
test_48 (void)
{
  vui8_t i, j;
  vui16_t k;
  vui32_t l;
  vui64_t m;
  vui128_t n;
  int rc = 0;

  i = (vui8_t) {0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99};
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

  n = db_vec_BC10es2i128 (m);
  print_vint128 ("BCD10x     ", n);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_cvtbcd2c100 (void)
{
  vui8_t i, j;
  vui8_t e;
  int rc = 0;

  printf ("\n%s Vector BCD convert\n", __FUNCTION__);

  i = (vui8_t) { 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
	         0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99 };
  e = (vui8_t) { 99, 99, 99, 99, 99, 99, 99, 99,
                 99, 99, 99, 99, 99, 99, 99, 99 };
  j = test_vec_bcdctb100s(i);

#ifdef __DEBUG_PRINT__
  print_vint8x ("BCD 9s     ", i);
  print_vint8d ("           ", i);
  print_vint8x ("BC100s     ", j);
  print_vint8d ("           ", j);
#endif
  rc += check_vuint128x ("vec_rdxct100b:", (vui128_t) j, (vui128_t) e);

  i = (vui8_t) { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  e = (vui8_t) { 00, 00, 00, 00, 00, 00, 00, 00,
                 00, 00, 00, 00, 00, 00, 00, 00 };
  j = test_vec_bcdctb100s(i);

#ifdef __DEBUG_PRINT__
  print_vint8x ("BCD 0s     ", i);
  print_vint8d ("           ", i);
  print_vint8x ("BC100s     ", j);
  print_vint8d ("           ", j);
#endif
  rc += check_vuint128x ("vec_rdxct100b:", (vui128_t) j, (vui128_t) e);

  i = (vui8_t) { 0x01, 0x09, 0x10, 0x19, 0x20, 0x29, 0x30, 0x39,
                 0x40, 0x49, 0x50, 0x59, 0x60, 0x69, 0x70, 0x79 };
  e = (vui8_t) { 1, 9, 10, 19, 20, 29, 30, 39,
                40, 49, 50, 59, 60, 69, 70, 79 };
  j = test_vec_bcdctb100s(i);

#ifdef __DEBUG_PRINT__
  print_vint8x ("BCD 01-79  ", i);
  print_vint8d ("           ", i);
  print_vint8x ("BC100s     ", j);
  print_vint8d ("           ", j);
#endif
  rc += check_vuint128x ("vec_rdxct100b:", (vui128_t) j, (vui128_t) e);

  i = (vui8_t) { 0x80, 0x81, 0x82, 0x83, 0x86, 0x87, 0x88, 0x89,
                 0x90, 0x91, 0x92, 0x93, 0x95, 0x96, 0x97, 0x98 };
  e = (vui8_t) { 80, 81, 82, 83, 86, 87, 88, 89,
                 90, 91, 92, 93, 95, 96, 97, 98 };
  j = test_vec_bcdctb100s(i);

#ifdef __DEBUG_PRINT__
  print_vint8x ("BCD 80-98  ", i);
  print_vint8d ("           ", i);
  print_vint8x ("BC100s     ", j);
  print_vint8d ("           ", j);
#endif
  rc += check_vuint128x ("vec_rdxct100b:", (vui128_t) j, (vui128_t) e);

  return (rc);
}
//#undef __DEBUG_PRINT__

int
test_bcd_addsub (void)
{
  vBCD_t i, j, k;
  vBCD_t e, ex;
  int rc = 0;

  printf ("\n%s Vector BCD +-\n", __FUNCTION__);

  i = (vBCD_t) CONST_VINT128_W (0, 0, 0, 0x0000001c);
  j = (vBCD_t) CONST_VINT128_W (0, 0, 0, 0x0000001c);
  k = vec_bcdadd (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (1+1)", k, i, j);
#endif
  e = (vBCD_t) CONST_VINT128_W(0, 0, 0, 0x2c);
  rc += check_vuint128x ("vec_bcdadd:", (vui128_t) k, (vui128_t) e);

  i = (vBCD_t) CONST_VINT128_W(0, 0, 0, 0x9999999c);
  j = (vBCD_t) CONST_VINT128_W(0, 0, 0, 0x0000001c);
  k = vec_bcdadd (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (9999999+1)", k, i, j);
#endif
  e = (vBCD_t) CONST_VINT128_W(0, 0, 0x1, 0x0000000c);
  rc += check_vuint128x ("vec_bcdadd:", (vui128_t) k, (vui128_t) e);

  i = (vBCD_t) CONST_VINT128_W(0, 0, 0, 0x0000001c);
  j = (vBCD_t) CONST_VINT128_W(0, 0, 0, 0x0000001c);
  k = vec_bcdsub (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (1-1)", k, i, j);
#endif
  e =  (vBCD_t) CONST_VINT128_W (0, 0, 0, 0x0000000c);
  ex = (vBCD_t) CONST_VINT128_W (0, 0, 0, 0x0000000d);
  if (vec_all_eq (k, ex))
    {
      printf ("vec_bcdsub: ignore negative zero. Likely QEMU artifact\n");
      k = e;
    }
  rc += check_vuint128x ("vec_bcdsub:", (vui128_t) k, (vui128_t) e);

  i = (vBCD_t) CONST_VINT128_W (0, 0, 0, 0x9999999c);
  j = (vBCD_t) CONST_VINT128_W (0, 0, 0, 0x0000001c);
  k = vec_bcdsub (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (9999999-1)", k, i, j);
#endif
  e = (vBCD_t) CONST_VINT128_W (0, 0, 0, 0x9999998c);
  rc += check_vuint128x ("vec_bcdsub:", (vui128_t) k, (vui128_t) e);

  i = (vBCD_t) CONST_VINT128_W (0, 0, 0, 0x0000001c);
  j = (vBCD_t) CONST_VINT128_W (0, 0, 0, 0x9999999c);
  k = vec_bcdsub (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (1-9999999)", k, i, j);
#endif
  e = (vBCD_t) CONST_VINT128_W (0, 0, 0, 0x9999998d);
  rc += check_vuint128x ("vec_bcdsub:", (vui128_t) k, (vui128_t) e);

  return rc;
}


 int
 test_bcd_muldiv (void)
 {
   vBCD_t i, j, k;
   vBCD_t e;
  int rc = 0;

  printf ("\n%s Vector BCD */\n", __FUNCTION__);

  i = (vBCD_t) CONST_VINT128_W (0, 0, 0, 0x0000001c);
  j = (vBCD_t) CONST_VINT128_W (0, 0, 0, 0x9999999c);
  k = vec_bcdmul (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (1*9999999)", k, i, j);
#endif
  e = (vBCD_t) CONST_VINT128_W (0, 0, 0, 0x9999999c);
  rc += check_vuint128x ("vec_bcdmul:", (vui128_t) k, (vui128_t) e);

  i = (vBCD_t) CONST_VINT128_W (0, 0, 0, 0x9999999c);
  j = (vBCD_t) CONST_VINT128_W (0, 0, 0, 0x9999999c);
  k = vec_bcdmul (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (9999999*9999999)", k, i, j);
#endif
  e = (vBCD_t) CONST_VINT128_W (0, 0, 0x9999998, 0x0000001c);
  rc += check_vuint128x ("vec_bcdmul:", (vui128_t) k, (vui128_t) e);

  i = (vBCD_t) CONST_VINT128_W (0, 0, 0x99999999, 0x9999999c);
  j = (vBCD_t) CONST_VINT128_W (0, 0, 0x99999999, 0x9999999c);
  k = vec_bcdmul (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (999999999999999*999999999999999)", k, i, j);
#endif
  e = (vBCD_t) CONST_VINT128_W (0x09999999, 0x99999998,
				0x00000000, 0x0000001c);
  rc += check_vuint128x ("vec_bcdmul:", (vui128_t) k, (vui128_t) e);

  i = (vBCD_t) CONST_VINT128_W (0x09999999, 0x99999998,
				0x00000000, 0x0000001c);
  j = (vBCD_t) CONST_VINT128_W (0, 0, 0x99999999, 0x9999999c);
  k = vec_bcddiv (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (999999999999998000000000000001/999999999999999)", k, i, j);
#endif
  e = (vBCD_t) CONST_VINT128_W (0x00000000, 0x00000000,
				0x99999999, 0x9999999c);
  rc += check_vuint128x ("vec_bcddiv:", (vui128_t) k, (vui128_t) e);

  i = (vBCD_t) CONST_VINT128_W (0, 0, 0x99999999, 0x9999999c);
  j = (vBCD_t) CONST_VINT128_W (0, 0x99999999, 0x99999999, 0x9999999c);
  k = vec_bcdmul (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (999999999999999*99999999999999999999999)", k, i, j);
#endif
  e = (vBCD_t) CONST_VINT128_W (0x00000000, 0x00000000,
				0x00000000, 0x0000000c);
  rc += check_vuint128x ("vec_bcdmul:", (vui128_t) k, (vui128_t) e);

  i = (vBCD_t) CONST_VINT128_W (0, 0, 0x00000001, 0x0000000c);
  j = (vBCD_t) CONST_VINT128_W (0, 0, 0, 0x3c);
  k = vec_bcddiv (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (10000000/3)", k, i, j);
#endif
  e = (vBCD_t) CONST_VINT128_W (0x00000000, 0x00000000,
				0x00000000, 0x3333333c);
  rc += check_vuint128x ("vec_bcddiv:", (vui128_t) k, (vui128_t) e);

  return rc;
}

 //#define __DEBUG_PRINT__ 1
 #ifdef __DEBUG_PRINT__
 #define test_vec_BC10ks2i128(_l)	db_vec_BC100s2i128(_l)
 #else
 #define test_vec_BC10ks2i128(_l)	vec_rdxct10kh(_l)
 #endif
 int
 test_cvtbcd2c10k (void)
 {
   vui8_t i;
   vui16_t j, e;
   int rc = 0;

   printf ("\n%s Vector BCD 100s convert\n", __FUNCTION__);

   i = (vui8_t) { 99, 99, 99, 99, 99, 99, 99, 99,
 	          99, 99, 99, 99, 99, 99, 99, 99 };
   e = (vui16_t) { 9999, 9999, 9999, 9999,
                  9999, 9999, 9999, 9999 };
   j = test_vec_BC10ks2i128 (i);

 #ifdef __DEBUG_PRINT__
   print_vint8x ("BCD 9s     ", i);
   print_vint8d ("           ", i);
   print_vint16x ("BC10ks     ", j);
   print_vint16d ("           ", j);
 #endif
   rc += check_vuint128x ("vec_rdxct10kh:", (vui128_t) j, (vui128_t) e);

   i = (vui8_t) CONST_VINT8_B (  1,  9, 10, 19, 20, 29, 30, 39,
                  40, 49, 50, 59, 60, 69, 70, 79 );
   e = (vui16_t) CONST_VINT16_H ( 109, 1019, 2029, 3039,
                   4049, 5059, 6069, 7079 );
   j = test_vec_BC10ks2i128 (i);

#ifdef __DEBUG_PRINT__
  print_vint8x ("BCD 9s     ", i);
  print_vint8d ("           ", i);
  print_vint16x ("BC10ks     ", j);
  print_vint16d ("           ", j);
#endif
  rc += check_vuint128x ("vec_rdxct10kh:", (vui128_t) j, (vui128_t) e);

  i = (vui8_t) CONST_VINT8_B ( 80, 81, 82, 83, 86, 87, 88, 89,
                 90, 91, 92, 93, 95, 96, 97, 98 );
  e = (vui16_t) CONST_VINT16_H ( 8081, 8283, 8687, 8889,
                 9091, 9293, 9596, 9798 );

  j = test_vec_BC10ks2i128 (i);

#ifdef __DEBUG_PRINT__
 print_vint8x ("BCD 9s     ", i);
 print_vint8d ("           ", i);
 print_vint16x ("BC10ks     ", j);
 print_vint16d ("           ", j);
#endif
 rc += check_vuint128x ("vec_rdxct10kh:", (vui128_t) j, (vui128_t) e);

  return rc;
}

 //#define __DEBUG_PRINT__ 1
 #ifdef __DEBUG_PRINT__
 #define test_vec_BC100ms2i128(_l)	db_vec_BC10ks2i128(_l)
 #else
 #define test_vec_BC100ms2i128(_l)	vec_rdxct100mw(_l)
 #endif
 int
 test_cvtbcd2c100m (void)
 {
   vui16_t i;
   vui32_t j, e;
   int rc = 0;

   printf ("\n%s Vector BCD 10Ks convert\n", __FUNCTION__);

   i = (vui16_t) { 9999, 9999, 9999, 9999,
 	          9999, 9999, 9999, 9999 };
   e = (vui32_t) { 99999999, 99999999,
                  99999999, 99999999 };
   j = test_vec_BC100ms2i128 (i);

 #ifdef __DEBUG_PRINT__
   print_vint16x ("BC10ks     ", i);
   print_vint16d ("           ", i);
   print_vint32x ("BC100ms    ", j);
   print_vint32d ("           ", j);
 #endif
   rc += check_vuint128x ("vec_rdxct100mw:", (vui128_t) j, (vui128_t) e);

   i = (vui16_t) CONST_VINT16_H (  109, 1019, 2029, 3039,
                                  4049, 5059, 6069, 7079 );
   e = (vui32_t) CONST_VINT32_W ( 1091019, 20293039,
                                 40495059, 60697079 );
   j = test_vec_BC100ms2i128 (i);

 #ifdef __DEBUG_PRINT__
   print_vint16x ("BC10ks     ", i);
   print_vint16d ("           ", i);
   print_vint32x ("BC100ms    ", j);
   print_vint32d ("           ", j);
 #endif
   rc += check_vuint128x ("vec_rdxct100mw:", (vui128_t) j, (vui128_t) e);

   i = (vui16_t) CONST_VINT16_H ( 8081, 8283, 8687, 8889,
			          9091, 9293, 9596, 9798 );
   e = (vui32_t) CONST_VINT32_W ( 80818283, 86878889,
			          90919293, 95969798 );
   j = test_vec_BC100ms2i128 (i);

 #ifdef __DEBUG_PRINT__
   print_vint16x ("BC10ks     ", i);
   print_vint16d ("           ", i);
   print_vint32x ("BC100ms    ", j);
   print_vint32d ("           ", j);
 #endif
   rc += check_vuint128x ("vec_rdxct100mw:", (vui128_t) j, (vui128_t) e);

   return rc;
 }

 //#define __DEBUG_PRINT__ 1
 #ifdef __DEBUG_PRINT__
 #define test_vec_BC10es2i128(_l)	db_vec_BC100ms2i128(_l)
 #else
 #define test_vec_BC10es2i128(_l)	vec_rdxct10E16d(_l)
 #endif
 int
 test_cvtbcd2c10e (void)
 {
   vui32_t i;
   vui64_t j, e;
   int rc = 0;

   printf ("\n%s Vector BCD 100Ms convert\n", __FUNCTION__);

   i = (vui32_t) { 99999999, 99999999,
 	           99999999, 99999999 };
   e = (vui64_t) { 9999999999999999UL,
                   9999999999999999UL };
   j = test_vec_BC10es2i128 (i);

 #ifdef __DEBUG_PRINT__
   print_vint32x  ("BC100ms    ", i);
   print_vint32d  ("           ", i);
   print_v2xint64 ("BC10es     ", j);
   print_v2int64  ("           ", j);
 #endif
   rc += check_vuint128x ("vec_rdxct10E16d:", (vui128_t) j, (vui128_t) e);

   i = (vui32_t) CONST_VINT32_W (  1091019, 20293039,
	                          40495059, 60697079 );
   e = (vui64_t) CONST_VINT64_DW ( 109101920293039UL,
	                          4049505960697079UL );
   j = test_vec_BC10es2i128 (i);

 #ifdef __DEBUG_PRINT__
   print_vint32x  ("BC100ms    ", i);
   print_vint32d  ("           ", i);
   print_v2xint64 ("BC10es     ", j);
   print_v2int64  ("           ", j);
 #endif
   rc += check_vuint128x ("vec_rdxct10E16d:", (vui128_t) j, (vui128_t) e);

   i = (vui32_t) CONST_VINT32_W ( 80818283, 86878889,
			          90919293, 95969798 );
   e = (vui64_t) CONST_VINT64_DW ( 8081828386878889UL,
				   9091929395969798UL );
   j = test_vec_BC10es2i128 (i);

 #ifdef __DEBUG_PRINT__
   print_vint32x  ("BC100ms    ", i);
   print_vint32d  ("           ", i);
   print_v2xint64 ("BC10es     ", j);
   print_v2int64  ("           ", j);
 #endif
   rc += check_vuint128x ("vec_rdxct10E16d:", (vui128_t) j, (vui128_t) e);

   return rc;
 }

 //#define __DEBUG_PRINT__ 1
 #ifdef __DEBUG_PRINT__
 #define test_vec_BC10e32i128(_l)	db_vec_BC10es2i128(_l)
 #else
 #define test_vec_BC10e32i128(_l)	vec_rdxct10e32q(_l)
 #endif
 int
 test_cvtbcd2c10e32 (void)
 {
   vui64_t i;
   vui128_t j, e;
   int rc = 0;

   printf ("\n%s Vector BCD 10es convert\n", __FUNCTION__);

   i = (vui64_t) { 9999999999999999UL,
 	           9999999999999999UL };
   /* e = 999999999999999999999999999999UQ  */
   e = (vui128_t) { (__int128 ) 9999999999999999ll
                  * (__int128 ) 10000000000000000ll
                  + (__int128 ) 9999999999999999ll };
   j = test_vec_BC10e32i128 (i);

 #ifdef __DEBUG_PRINT__
   print_v2xint64 ("BC10es    ", i);
   print_v2int64  ("          ", i);
   print_vint128x ("BC10e32   ", j);
   print_vint128  ("          ", j);
 #endif
   rc += check_vuint128x ("vec_rdxct10e32q:", (vui128_t) j, (vui128_t) e);

   i = (vui64_t) CONST_VINT64_DW (  109101920293039UL,
		                   4049505960697079UL );
   /* e = 1091019202930394049505960697079UQ  */
   e = (vui128_t) { (__int128 ) 109101920293039ll
                  * (__int128 ) 10000000000000000ll
                  + (__int128 ) 4049505960697079ll };
   j = test_vec_BC10e32i128 (i);

 #ifdef __DEBUG_PRINT__
   print_v2xint64 ("BC10es    ", i);
   print_v2int64  ("          ", i);
   print_vint128x ("BC10e32   ", j);
   print_vint128  ("          ", j);
 #endif
   rc += check_vuint128x ("vec_rdxct10e32q:", (vui128_t) j, (vui128_t) e);

   i = (vui64_t) CONST_VINT64_DW ( 8081828386878889UL,
				   9091929395969798UL );
   /* e = 80818283868788899091929395969798UQ  */
   e = (vui128_t) { (__int128 ) 8081828386878889ll
                  * (__int128 ) 10000000000000000ll
                  + (__int128 ) 9091929395969798ll };
   j = test_vec_BC10e32i128 (i);

 #ifdef __DEBUG_PRINT__
   print_v2xint64 ("BC10es    ", i);
   print_v2int64  ("          ", i);
   print_vint128x ("BC10e32   ", j);
   print_vint128  ("          ", j);
 #endif
   rc += check_vuint128x ("vec_rdxct10e32q:", (vui128_t) j, (vui128_t) e);

   return rc;
 }

 //#define __DEBUG_PRINT__ 1
 #ifdef __DEBUG_PRINT__
 #define test_vec_bcdcfz100s(_l, _m)	db_vec_ZN2i128(_l, _m)
 #else
 #define test_vec_bcdcfz100s(_l, _m)	vec_rdxcfzt100b(_l, _m)
 #endif
 int
 test_cvtbcfz2c100 (void)
 {
   vui8_t i00, i16, j;
   vui8_t e;
   int rc = 0;

   printf ("\n%s Vector Zoned convert\n", __FUNCTION__);

   i00 = (vui8_t) { '9', '9', '9', '9', '9', '9', '9', '9',
                    '9', '9', '9', '9', '9', '9', '9', '9' };
   i16 = (vui8_t) { '9', '9', '9', '9', '9', '9', '9', '9',
                    '9', '9', '9', '9', '9', '9', '9', '9' };
   e = (vui8_t) { 99, 99, 99, 99, 99, 99, 99, 99,
                  99, 99, 99, 99, 99, 99, 99, 99 };
   j = test_vec_bcdcfz100s (i00, i16);

 #ifdef __DEBUG_PRINT__
   print_vint8c ("Zoned      ", i00);
   print_vint8c ("           ", i16);
   print_vint8x ("Zoned 100s ", j);
   print_vint8d ("           ", j);
 #endif
   rc += check_vuint128x ("vec_rdxcfzt100b:", (vui128_t) j, (vui128_t) e);

   i00 = (vui8_t) CONST_VINT8_B ( '0', '1', '0', '9', '1', '0', '1', '9',
                                  '2', '0', '2', '9', '3', '0', '3', '9' );
   i16 = (vui8_t) CONST_VINT8_B ( '4', '0', '4', '9', '5', '0', '5', '9',
                                  '6', '0', '6', '9', '7', '0', '7', '9' );
   e =   (vui8_t) CONST_VINT8_B (  1,  9, 10, 19, 20, 29, 30, 39,
                                  40, 49, 50, 59, 60, 69, 70, 79 );
   j = test_vec_bcdcfz100s (i00, i16);

 #ifdef __DEBUG_PRINT__
   print_vint8c ("Zoned      ", i00);
   print_vint8c ("           ", i16);
   print_vint8x ("Zoned 100s ", j);
   print_vint8d ("           ", j);
 #endif
   rc += check_vuint128x ("vec_rdxcfzt100b:", (vui128_t) j, (vui128_t) e);

   i00 = (vui8_t) CONST_VINT8_B ( '8', '0', '8', '1', '8', '2', '8', '3',
                                  '8', '6', '8', '7', '8', '8', '8', '9' );
   i16 = (vui8_t) CONST_VINT8_B ( '9', '0', '9', '1', '9', '2', '9', '3',
                                  '9', '5', '9', '6', '9', '7', '9', '8' );
   e =   (vui8_t) CONST_VINT8_B ( 80, 81, 82, 83, 86, 87, 88, 89,
		                  90, 91, 92, 93, 95, 96, 97, 98 );
   j = test_vec_bcdcfz100s (i00, i16);

 #ifdef __DEBUG_PRINT__
   print_vint8c ("Zoned      ", i00);
   print_vint8c ("           ", i16);
   print_vint8x ("Zoned 100s ", j);
   print_vint8d ("           ", j);
 #endif
   rc += check_vuint128x ("vec_rdxcfzt100b:", (vui128_t) j, (vui128_t) e);

   return rc;
 }

 //#define __DEBUG_PRINT__ 1
 int
 test_bcdctub (void)
 {
   vui8_t i;
   vui8_t j;
   vui8_t e;
   int rc = 0;

   printf ("\n%s Vector BCD convert\n", __FUNCTION__);

   i = (vui8_t) { 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
 	         0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99 };
   e = (vui8_t) { 99, 99, 99, 99, 99, 99, 99, 99,
                  99, 99, 99, 99, 99, 99, 99, 99 };
   j = vec_bcdctub ((vBCD_t) i);

 #ifdef __DEBUG_PRINT__
   print_vint8x ("BCD bytes  ", i);
   print_vint8d ("           ", i);
   print_vint8x ("char bytes ", j);
   print_vint8d ("           ", j);
 #endif
   rc += check_vuint128x ("vec_bcdctub:", (vui128_t) j, (vui128_t) e);

   i = (vui8_t) { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
   e = (vui8_t) { 00, 00, 00, 00, 00, 00, 00, 00,
                  00, 00, 00, 00, 00, 00, 00, 00 };
   j = vec_bcdctub ((vBCD_t) i);

 #ifdef __DEBUG_PRINT__
   print_vint8x ("BCD bytes  ", i);
   print_vint8d ("           ", i);
   print_vint8x ("char bytes ", j);
   print_vint8d ("           ", j);
 #endif
   rc += check_vuint128x ("vec_bcdctub:", (vui128_t) j, (vui128_t) e);

   i = (vui8_t) { 0x01, 0x09, 0x10, 0x19, 0x20, 0x29, 0x30, 0x39,
                  0x40, 0x49, 0x50, 0x59, 0x60, 0x69, 0x70, 0x79 };
   e = (vui8_t) { 1, 9, 10, 19, 20, 29, 30, 39,
                 40, 49, 50, 59, 60, 69, 70, 79 };
   j = vec_bcdctub ((vBCD_t) i);

 #ifdef __DEBUG_PRINT__
   print_vint8x ("BCD bytes  ", i);
   print_vint8d ("           ", i);
   print_vint8x ("char bytes ", j);
   print_vint8d ("           ", j);
 #endif
   rc += check_vuint128x ("vec_bcdctub:", (vui128_t) j, (vui128_t) e);

   i = (vui8_t) { 0x80, 0x81, 0x82, 0x83, 0x86, 0x87, 0x88, 0x89,
                  0x90, 0x91, 0x92, 0x93, 0x95, 0x96, 0x97, 0x98 };
   e = (vui8_t) { 80, 81, 82, 83, 86, 87, 88, 89,
                  90, 91, 92, 93, 95, 96, 97, 98 };
   j = vec_bcdctub ((vBCD_t) i);

 #ifdef __DEBUG_PRINT__
   print_vint8x ("BCD bytes  ", i);
   print_vint8d ("           ", i);
   print_vint8x ("char bytes ", j);
   print_vint8d ("           ", j);
 #endif
   rc += check_vuint128x ("vec_bcdctub:", (vui128_t) j, (vui128_t) e);

   return (rc);
 }
 //#undef __DEBUG_PRINT__

 //#define __DEBUG_PRINT__ 1
 int
 test_bcdctuh (void)
 {
   vui16_t i;
   vui16_t j, e;
   int rc = 0;

   printf ("\n%s Vector BCD halfword convert\n", __FUNCTION__);

   i = (vui16_t) CONST_VINT16_H ( 0x9999, 0x9999, 0x9999, 0x9999,
 	                          0x9999, 0x9999, 0x9999, 0x9999 );
   e = (vui16_t) CONST_VINT16_H ( 9999, 9999, 9999, 9999,
                                  9999, 9999, 9999, 9999 );
   j = vec_bcdctuh ((vBCD_t) i);

 #ifdef __DEBUG_PRINT__
   print_vint16x ("BCD halfwords ", i);
   print_vint16d ("              ", i);
   print_vint16x ("short int     ", j);
   print_vint16d ("              ", j);
 #endif
   rc += check_vuint128x ("vec_bcdctuh:", (vui128_t) j, (vui128_t) e);

   i = (vui16_t) CONST_VINT16_H ( 0x0109, 0x1019, 0x2029, 0x3039,
                                  0x4049, 0x5059, 0x6069, 0x7079 );
   e = (vui16_t) CONST_VINT16_H ( 109, 1019, 2029, 3039,
                                 4049, 5059, 6069, 7079 );
   j = vec_bcdctuh ((vBCD_t) i);

#ifdef __DEBUG_PRINT__
   print_vint16x ("BCD halfwords ", i);
   print_vint16d ("              ", i);
   print_vint16x ("short int     ", j);
   print_vint16d ("              ", j);
#endif
  rc += check_vuint128x ("vec_bcdctuh:", (vui128_t) j, (vui128_t) e);

  i = (vui16_t) CONST_VINT16_H ( 0x8081, 0x8283, 0x8687, 0x8889,
                                 0x9091, 0x9293, 0x9596, 0x9798 );
  e = (vui16_t) CONST_VINT16_H ( 8081, 8283, 8687, 8889,
                                 9091, 9293, 9596, 9798 );

  j = vec_bcdctuh ((vBCD_t) i);

#ifdef __DEBUG_PRINT__
  print_vint16x ("BCD halfwords ", i);
  print_vint16d ("              ", i);
  print_vint16x ("short int     ", j);
  print_vint16d ("              ", j);
#endif
 rc += check_vuint128x ("vec_bcdctuh:", (vui128_t) j, (vui128_t) e);

  return rc;
}
 //#undef __DEBUG_PRINT__

 //#define __DEBUG_PRINT__ 1
 int
 test_bcdctuw (void)
 {
   vui32_t i;
   vui32_t j, e;
   int rc = 0;

   printf ("\n%s Vector BCD word convert\n", __FUNCTION__);

   i = (vui32_t) CONST_VINT32_W ( 0x99999999, 0x99999999,
 	                          0x99999999, 0x99999999 );
   e = (vui32_t) CONST_VINT32_W  ( 99999999, 99999999,
                                   99999999, 99999999 );
   j = vec_bcdctuw ((vBCD_t) i);

 #ifdef __DEBUG_PRINT__
   print_vint32x ("BCD words ", i);
   print_vint32d ("          ", i);
   print_vint32x ("int       ", j);
   print_vint32d ("          ", j);
 #endif
   rc += check_vuint128x ("vec_bcdctuw:", (vui128_t) j, (vui128_t) e);

   i = (vui32_t) CONST_VINT32_W ( 0x01091019, 0x20293039,
                                  0x40495059, 0x60697079 );
   e = (vui32_t) CONST_VINT32_W ( 1091019, 20293039,
                                 40495059, 60697079 );
   j = vec_bcdctuw ((vBCD_t) i);

#ifdef __DEBUG_PRINT__
   print_vint32x ("BCD words ", i);
   print_vint32d ("          ", i);
   print_vint32x ("int       ", j);
   print_vint32d ("          ", j);
#endif
  rc += check_vuint128x ("vec_bcdctuw:", (vui128_t) j, (vui128_t) e);

  i = (vui32_t) CONST_VINT32_W ( 0x80818283, 0x86878889,
                                 0x90919293, 0x95969798 );
  e = (vui32_t) CONST_VINT32_W ( 80818283, 86878889,
                                 90919293, 95969798 );

  j = vec_bcdctuw ((vBCD_t) i);

#ifdef __DEBUG_PRINT__
  print_vint32x ("BCD words ", i);
  print_vint32d ("          ", i);
  print_vint32x ("int       ", j);
  print_vint32d ("          ", j);
#endif
 rc += check_vuint128x ("vec_bcdctuw:", (vui128_t) j, (vui128_t) e);

  return rc;
}
 //#undef __DEBUG_PRINT__

 //#define __DEBUG_PRINT__ 1
 int
 test_bcdctud (void)
 {
   vui64_t i;
   vui64_t j, e;
   int rc = 0;

   printf ("\n%s Vector BCD dword convert\n", __FUNCTION__);

   i = (vui64_t) CONST_VINT64_DW ( 0x9999999999999999UL,
 	                           0x9999999999999999UL );
   e = (vui64_t) CONST_VINT64_DW ( 9999999999999999UL,
                                   9999999999999999UL );
   j = vec_bcdctud ((vBCD_t) i);

 #ifdef __DEBUG_PRINT__
   print_v2xint64 ("BCD dwords ", i);
   print_v2xint64 ("long int   ", j);
   print_v2int64  ("           ", j);
 #endif
   rc += check_vuint128x ("vec_bcdctud:", (vui128_t) j, (vui128_t) e);

   i = (vui64_t) CONST_VINT64_DW ( 0x0109101920293039UL,
                                   0x4049505960697079UL );
   e = (vui64_t) CONST_VINT64_DW ( 109101920293039UL,
                                  4049505960697079UL );
   j = vec_bcdctud ((vBCD_t) i);

#ifdef __DEBUG_PRINT__
   print_v2xint64 ("BCD dwords ", i);
   print_v2xint64 ("long int   ", j);
   print_v2int64  ("           ", j);
#endif
  rc += check_vuint128x ("vec_bcdctud:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 0x8081828386878889UL,
                                  0x9091929395969798UL );
  e = (vui64_t) CONST_VINT64_DW ( 8081828386878889UL,
                                  9091929395969798UL );

  j = vec_bcdctud ((vBCD_t) i);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("BCD dwords ", i);
  print_v2xint64 ("long int   ", j);
  print_v2int64  ("           ", j);
#endif
 rc += check_vuint128x ("vec_bcdctud:", (vui128_t) j, (vui128_t) e);

  return rc;
}
 //#undef __DEBUG_PRINT__

 //#define __DEBUG_PRINT__ 1
 int
 test_bcdctuq (void)
 {
   vui128_t i;
   vui128_t j, e;
   int rc = 0;

   printf ("\n%s Vector BCD qword convert\n", __FUNCTION__);

   i = CONST_VINT128_DW128 ( 0x9999999999999999UL,
 	                     0x9999999999999999UL );
   /* e = 999999999999999999999999999999UQ  */
   e = (vui128_t) { (__int128 ) 9999999999999999ll
                  * (__int128 ) 10000000000000000ll
                  + (__int128 ) 9999999999999999ll };
   j = vec_bcdctuq ((vBCD_t) i);

 #ifdef __DEBUG_PRINT__
   print_vint128x ("BCD qwords ", i);
   print_vint128x ("__int128   ", j);
   print_vint128  ("           ", j);
 #endif
   rc += check_vuint128x ("vec_bcdctuq:", (vui128_t) j, (vui128_t) e);

   i = CONST_VINT128_DW128 ( 0x0109101920293039UL,
                             0x4049505960697079UL );
   /* e = 1091019202930394049505960697079UQ  */
   e = (vui128_t) { (__int128 ) 109101920293039ll
                  * (__int128 ) 10000000000000000ll
                  + (__int128 ) 4049505960697079ll };
   j = vec_bcdctuq ((vBCD_t) i);

#ifdef __DEBUG_PRINT__
   print_vint128x ("BCD qwords ", i);
   print_vint128x ("__int128   ", j);
   print_vint128  ("           ", j);
#endif
  rc += check_vuint128x ("vec_bcdctuq:", (vui128_t) j, (vui128_t) e);

  i = CONST_VINT128_DW128 ( 0x8081828386878889UL,
                            0x9091929395969798UL );
  /* e = 80818283868788899091929395969798UQ  */
  e = (vui128_t) { (__int128 ) 8081828386878889ll
                 * (__int128 ) 10000000000000000ll
                 + (__int128 ) 9091929395969798ll };

  j = vec_bcdctuq ((vBCD_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("BCD qwords ", i);
  print_vint128x ("__int128   ", j);
  print_vint128  ("           ", j);
#endif
 rc += check_vuint128x ("vec_bcdctuq:", (vui128_t) j, (vui128_t) e);

  return rc;
}

 //#define __DEBUG_PRINT__ 1
 int
 test_zndctuq (void)
 {
   vui8_t i00, i16;
   vui128_t j, e;
   int rc = 0;

   printf ("\n%s Vector Zoned qword convert\n", __FUNCTION__);

   i00 = (vui8_t) { '9', '9', '9', '9', '9', '9', '9', '9',
                    '9', '9', '9', '9', '9', '9', '9', '9' };
   i16 = (vui8_t) { '9', '9', '9', '9', '9', '9', '9', '9',
                    '9', '9', '9', '9', '9', '9', '9', '9' };
   /* e = 999999999999999999999999999999UQ  */
   e = (vui128_t) { (__int128 ) 9999999999999999ll
                  * (__int128 ) 10000000000000000ll
                  + (__int128 ) 9999999999999999ll };
   j = vec_zndctuq (i00, i16);

 #ifdef __DEBUG_PRINT__
   print_vint8c   ("Zoned      ", i00);
   print_vint8c   ("           ", i16);
   print_vint128x ("__int128   ", j);
   print_vint128  ("           ", j);
 #endif
   rc += check_vuint128x ("vec_zndctuq:", (vui128_t) j, (vui128_t) e);

   i00 = (vui8_t) CONST_VINT8_B ( '0', '1', '0', '9', '1', '0', '1', '9',
                                  '2', '0', '2', '9', '3', '0', '3', '9' );
   i16 = (vui8_t) CONST_VINT8_B ( '4', '0', '4', '9', '5', '0', '5', '9',
                                  '6', '0', '6', '9', '7', '0', '7', '9' );
   /* e = 1091019202930394049505960697079UQ  */
   e = (vui128_t) { (__int128 ) 109101920293039ll
                  * (__int128 ) 10000000000000000ll
                  + (__int128 ) 4049505960697079ll };
   j = vec_zndctuq (i00, i16);

#ifdef __DEBUG_PRINT__
   print_vint8c   ("Zoned      ", i00);
   print_vint8c   ("           ", i16);
   print_vint128x ("__int128   ", j);
   print_vint128  ("           ", j);
#endif
  rc += check_vuint128x ("vec_zndctuq:", (vui128_t) j, (vui128_t) e);

  i00 = (vui8_t) CONST_VINT8_B ( '8', '0', '8', '1', '8', '2', '8', '3',
                                 '8', '6', '8', '7', '8', '8', '8', '9' );
  i16 = (vui8_t) CONST_VINT8_B ( '9', '0', '9', '1', '9', '2', '9', '3',
                                 '9', '5', '9', '6', '9', '7', '9', '8' );
  /* e = 80818283868788899091929395969798UQ  */
  e = (vui128_t) { (__int128 ) 8081828386878889ll
                 * (__int128 ) 10000000000000000ll
                 + (__int128 ) 9091929395969798ll };

  j = vec_zndctuq (i00, i16);

#ifdef __DEBUG_PRINT__
  print_vint8c   ("Zoned      ", i00);
  print_vint8c   ("           ", i16);
  print_vint128x ("__int128   ", j);
  print_vint128  ("           ", j);
#endif
 rc += check_vuint128x ("vec_zndctuq:", (vui128_t) j, (vui128_t) e);

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

  rc += test_cvtbcd2c10k ();

  rc += test_cvtbcd2c100m ();

  rc += test_cvtbcd2c10e ();

  rc += test_cvtbcd2c10e32 ();

  rc += test_cvtbcfz2c100 ();

  rc += test_bcdctub ();
  rc += test_bcdctuh ();
  rc += test_bcdctuw ();
  rc += test_bcdctud ();
  rc += test_bcdctuq ();

  rc += test_zndctuq ();

//  rc += test_48 ();

  return (rc);
}
