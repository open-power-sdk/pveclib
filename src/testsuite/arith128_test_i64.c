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

 arith128_test_i64.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Apr 5, 2018
 */

#include <stdint.h>
#include <stdio.h>
#include <fenv.h>
#include <float.h>
#include <math.h>

//#define __DEBUG_PRINT__
#include <testsuite/arith128_print.h>
#include <vec_common_ppc.h>
#include <vec_int32_ppc.h>
#include <vec_int64_ppc.h>

//#include "arith128.h"
#include <testsuite/arith128_test_i64.h>

//#define __DEBUG_PRINT__ 1

#ifdef __DEBUG_PRINT__
vui64_t
db_vec_vsld (vui64_t vra, vui64_t vrb)
{
  vui64_t result;

#ifdef _ARCH_PWR8
  __asm__(
      "vsld %0,%1,%2;"
      : "=v" (result)
      : "v" (vra),
      "v" (vrb)
      : );
#else
  vui8_t  vsh_h, vsh_l;
  vui8_t  vr_h, vr_l;
  vui64_t sel_mask = CONST_VINT128_DW(0, -1LL);
  vui64_t shft_mask = CONST_VINT128_DW(63, 63);
  print_v2xint64 ("db_vec_vsld (", vra);
  print_v2xint64 ("             ", vrb);

  /* constrain the dword shift amounts to 0-63.  */
  vsh_l = vec_and ((vui8_t)vrb, (vui8_t)shft_mask);
  /* Isolate the low dword so that bits from the high dword,
   * do not contaminate the result.  */
  vr_h = vec_andc ((vui8_t)vra, (vui8_t)sel_mask);
  vr_l  = vec_and ((vui8_t)vra, (vui8_t)sel_mask);
  /* The vsr instruction only works correctly if the bit shift
   * value is splatted to each byte of the vector.  */
  vsh_h = vec_splat (vsh_l, VEC_BYTE_L_DWH);
  vsh_l = vec_splat (vsh_l, VEC_BYTE_L_DWL);
  print_v2xint64 ("      vsh_h (", (vui64_t)vsh_h);
  print_v2xint64 ("      vsh_l (", (vui64_t)vsh_l);
//  vsht_splat = vec_splat ((vui8_t) vrb, VEC_BYTE_L);
  /* Shift the high dword by vsh_h.  */
  vr_h = vec_vslo (vr_h,  vsh_h);
  vr_h = vec_vsl  (vr_h, vsh_h);
  print_v2xint64 ("       vr_h (", (vui64_t)vr_h);
  /* Shift the low dword by vsh_l.  */
  vr_l = vec_vslo (vr_l,  vsh_l);
  vr_l = vec_vsl  (vr_l, vsh_l);
  print_v2xint64 ("       vr_l (", (vui64_t)vr_l);
  /* Merge the dwords after shift.  */
  result = (vui64_t)vec_sel (vr_h, vr_l, (vui8_t)sel_mask);
#endif
  return ((vui64_t) result);
}

vui64_t
db_vec_vsrd (vui64_t vra, vui64_t vrb)
{
  vui64_t result;

#ifdef _ARCH_PWR8
  __asm__(
      "vsrd %0,%1,%2;"
      : "=v" (result)
      : "v" (vra),
      "v" (vrb)
      : );
#else
  vui8_t  vsh_h, vsh_l;
  vui8_t  vr_h, vr_l;
  vui64_t sel_mask = CONST_VINT128_DW(0, -1LL);
  vui64_t shft_mask = CONST_VINT128_DW(63, 63);
  print_v2xint64 ("db_vec_vsrd (", vra);
  print_v2xint64 ("             ", vrb);

  /* constrain the dword shift amounts to 0-63.  */
  vsh_l = vec_and ((vui8_t)vrb, (vui8_t)shft_mask);
  /* Isolate the low dword so that bits from the high dword,
   * do not contaminate the result.  */
  vr_l  = vec_and ((vui8_t)vra, (vui8_t)sel_mask);
  /* The vsr instruction only works correctly if the bit shift
   * value is splatted to each byte of the vector.  */
  vsh_h = vec_splat (vsh_l, VEC_BYTE_L_DWH);
  vsh_l = vec_splat (vsh_l, VEC_BYTE_L_DWL);
  print_v2xint64 ("      vsh_h (", (vui64_t)vsh_h);
  print_v2xint64 ("      vsh_l (", (vui64_t)vsh_l);
//  vsht_splat = vec_splat ((vui8_t) vrb, VEC_BYTE_L);
  /* Shift the high dword by vsh_h.  */
  vr_h = vec_vsro ((vui8_t)vra,  vsh_h);
  vr_h = vec_vsr  (vr_h, vsh_h);
  print_v2xint64 ("       vr_h (", (vui64_t)vr_h);
  /* Shift the low dword by vsh_l.  */
  vr_l = vec_vsro (vr_l,  vsh_l);
  vr_l = vec_vsr  (vr_l, vsh_l);
  print_v2xint64 ("       vr_l (", (vui64_t)vr_l);
  /* Merge the dwords after shift.  */
  result = (vui64_t)vec_sel (vr_h, vr_l, (vui8_t)sel_mask);
#endif
  return ((vui64_t) result);
}

vi64_t
db_vec_vsrad (vi64_t vra, vui64_t vrb)
{
  vi64_t result;

#ifdef _ARCH_PWR8
  __asm__(
      "vsrad %0,%1,%2;"
      : "=v" (result)
      : "v" (vra),
      "v" (brb)
      : );
#else
  vui8_t  vsh_h, vsh_l;
  vui8_t  vr_h, vr_l;
  vi32_t exsa;
  vui32_t shw31 = CONST_VINT128_W (31, 31, 31, 31);
  vui64_t exsah, exsal;
//  vui64_t sel_mask = CONST_VINT128_DW(0, -1LL);
  vui64_t shft_mask = CONST_VINT128_DW(63, 63);
  print_v2xint64 ("db_vec_vsrad (", (vui64_t)vra);
  print_v2xint64 ("              ", vrb);

  /* Need to extend each signed long int to __int128. So the unsigned
   * (128-bit) shift right behave as a arithmetic (64-bit) shift.  */
  exsa = vec_vsraw ((vi32_t)vra, shw31);
  exsah = (vui64_t)vec_vmrghw (exsa, exsa);
  exsal = (vui64_t)vec_vmrglw (exsa, exsa);
  print_v2xint64 ("      exsah (", (vui64_t)exsah);
  print_v2xint64 ("      exsal (", (vui64_t)exsal);
  /* constrain the dword shift amounts to 0-63.  */
  vsh_l = vec_and ((vui8_t)vrb, (vui8_t)shft_mask);
  /* Isolate the low dword so that bits from the high dword,
   * do not contaminate the result.  */
//  vr_l  = vec_and ((vui8_t)vra, (vui8_t)sel_mask);
  /* The vsr instruction only works correctly if the bit shift
   * value is splatted to each byte of the vector.  */
  vsh_h = vec_splat (vsh_l, VEC_BYTE_L_DWH);
  vsh_l = vec_splat (vsh_l, VEC_BYTE_L_DWL);
  print_v2xint64 ("      vsh_h (", (vui64_t)vsh_h);
  print_v2xint64 ("      vsh_l (", (vui64_t)vsh_l);
//  vsht_splat = vec_splat ((vui8_t) vrb, VEC_BYTE_L);
  /* Merge the extended sign with high dword.  */
  exsah = vec_mrghd (exsah, (vui64_t)vra);
  print_v2xint64 ("exsd-vra[0] (", (vui64_t)exsah);
  /* Shift the high dword by vsh_h.  */
  vr_h = vec_vsro ((vui8_t)exsah,  vsh_h);
  vr_h = vec_vsr  (vr_h, vsh_h);
  print_v2xint64 ("       vr_h (", (vui64_t)vr_h);
  /* Merge the extended sign with high dword.  */
  exsal = vec_pasted (exsal, (vui64_t)vra);
  print_v2xint64 ("exsd-vra[1] (", (vui64_t)exsal);
  /* Shift the low dword by vsh_l.  */
  vr_l = vec_vsro ((vui8_t)exsal,  vsh_l);
  vr_l = vec_vsr  (vr_l, vsh_l);
  print_v2xint64 ("       vr_l (", (vui64_t)vr_l);
  /* Merge the dwords after shift.  */
//  result = (vi64_t)vec_sel (vr_h, vr_l, (vui8_t)sel_mask);
  result = (vi64_t)vec_mrgld ((vui64_t)vr_h, (vui64_t)vr_l);
#endif
  return ((vi64_t) result);
}
#endif

#ifdef __DEBUG_PRINT__
#define test_vec_vsld(_i, _j)	db_vec_vsld(_i, _j)
#else
#define test_vec_vsld(_i, _j)	vec_vsld(_i, _j)
#endif
int
test_vsld (void)
{
  vui64_t i1, i2, e;
  vui64_t j;
  int rc = 0;

  printf ("\ntest_vsld Vector shift left doubleword\n");

  i1 = (vui64_t )CONST_VINT64_DW(1, 2);
  i2 = (vui64_t )CONST_VINT64_DW(2, 1);
  e = (vui64_t )CONST_VINT64_DW(4, 4);
  j = test_vec_vsld(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsld   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vsld:", (vui128_t)j, (vui128_t) e);

  i1 = (vui64_t )CONST_VINT64_DW(-1, -1);
  i2 = (vui64_t )CONST_VINT64_DW(33, 31);
  e = (vui64_t )CONST_VINT64_DW(0xfffffffe00000000, 0xffffffff80000000);
  j = test_vec_vsld(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsld   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vsld:", (vui128_t)j, (vui128_t) e);

  i1 = (vui64_t )CONST_VINT64_DW(-1, -1);
  i2 = (vui64_t )CONST_VINT64_DW(63, 60);
  e = (vui64_t )CONST_VINT64_DW(0x8000000000000000, 0xf000000000000000);
  j = test_vec_vsld(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsld   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vsld:", (vui128_t)j, (vui128_t) e);

  return (rc);
}

#ifdef __DEBUG_PRINT__
#define test_vec_vsrd(_i, _j)	db_vec_vsrd(_i, _j)
#else
#define test_vec_vsrd(_i, _j)	vec_vsrd(_i, _j)
#endif
int
test_vsrd (void)
{
  vui64_t i1, i2, e;
  vui64_t j;
  int rc = 0;

  printf ("\ntest_vsrd Vector shift right doubleword\n");

  i1 = (vui64_t )CONST_VINT64_DW(4, 2);
  i2 = (vui64_t )CONST_VINT64_DW(2, 1);
  e = (vui64_t )CONST_VINT64_DW(1, 1);
  j = test_vec_vsrd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrd   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vsrd:", (vui128_t)j, (vui128_t) e);
#if 1
  i1 = (vui64_t )CONST_VINT64_DW(-1, -1);
  i2 = (vui64_t )CONST_VINT64_DW(33, 31);
  e = (vui64_t )CONST_VINT64_DW(0x000000007fffffff, 0x00000001ffffffff);
  j = test_vec_vsrd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrd   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vsrd:", (vui128_t)j, (vui128_t) e);

  i1 = (vui64_t )CONST_VINT64_DW(-1, -1);
  i2 = (vui64_t )CONST_VINT64_DW(63, 60);
  e = (vui64_t )CONST_VINT64_DW(0x1, 0xf);
  j = test_vec_vsrd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrd   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vsrd:", (vui128_t)j, (vui128_t) e);

  i1 = (vui64_t )CONST_VINT64_DW(100000000000, 100000000000);
  i2 = (vui64_t )CONST_VINT64_DW(33, 31);
  e = (vui64_t )CONST_VINT64_DW(11, 46);
  j = test_vec_vsrd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrd   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vsrd:", (vui128_t)j, (vui128_t) e);

  i1 = (vui64_t )CONST_VINT64_DW(100000000000, 100000000000);
  i2 = (vui64_t )CONST_VINT64_DW(48, 40);
  e = (vui64_t )CONST_VINT64_DW(0, 0);
  j = test_vec_vsrd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrd   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vsrd:", (vui128_t)j, (vui128_t) e);
#endif
  return (rc);
}

#ifdef __DEBUG_PRINT__
#define test_vec_vsrad(_i, _j)	db_vec_vsrad(_i, _j)
#else
#define test_vec_vsrad(_i, _j)	vec_vsrad(_i, _j)
#endif
int
test_vsrad (void)
{
  vi64_t i1, e;
  vui64_t i2;
  vi64_t j;
  int rc = 0;

  printf ("\ntest_vsrd Vector shift right arithmetic doubleword\n");

  i1 = (vi64_t )CONST_VINT64_DW(4, 2);
  i2 = (vui64_t )CONST_VINT64_DW(2, 1);
  e = (vi64_t )CONST_VINT64_DW(1, 1);
  j = test_vec_vsrad(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrad  ( ", (vui64_t)i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_vsrad:", (vui128_t)j, (vui128_t) e);

  i1 = (vi64_t )CONST_VINT64_DW(-4, -2);
  i2 = (vui64_t )CONST_VINT64_DW(2, 1);
  e = (vi64_t )CONST_VINT64_DW(-1, -1);
  j = test_vec_vsrad(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrad  ( ", (vui64_t)i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_vsrad:", (vui128_t)j, (vui128_t) e);

  i1 = (vi64_t )CONST_VINT64_DW(0x8000000000000000, 0x7fffffffffffffff);
  i2 = (vui64_t )CONST_VINT64_DW(63, 63);
  e = (vi64_t )CONST_VINT64_DW(-1, 0);
  j = test_vec_vsrad(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrad  ( ", (vui64_t)i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_vsrad:", (vui128_t)j, (vui128_t) e);

  i1 = (vi64_t )CONST_VINT64_DW(0x7fffffffffffffff, 0x8000000000000000);
  i2 = (vui64_t )CONST_VINT64_DW(63, 63);
  e = (vi64_t )CONST_VINT64_DW(0, -1);
  j = test_vec_vsrad(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrad  ( ", (vui64_t)i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_vsrad:", (vui128_t)j, (vui128_t) e);

  return (rc);
}
#undef __DEBUG_PRINT__

int
test_permdi (void)
{
  vui64_t i1, i2, e;
  vui64_t j;
  int rc = 0;

  printf ("\ntest_permdi Vector permute doubleword immediate\n");

  i1 = (vui64_t )CONST_VINT64_DW(1, 2);
  i2 = (vui64_t )CONST_VINT64_DW(101, 102);
  e = (vui64_t )CONST_VINT64_DW(1, 101);
  j = vec_permdi(i1, i2, 0);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("permdi ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("     ,0)=", j);
#endif
  rc += check_vuint128x ("vec_permdi:", (vui128_t)j, (vui128_t) e);

  e = (vui64_t )CONST_VINT64_DW(1, 102);
  j = vec_permdi(i1, i2, 1);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("permdi ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("     ,1)=", j);
#endif
  rc += check_vuint128x ("vec_permdi:", (vui128_t)j, (vui128_t) e);

  e = (vui64_t )CONST_VINT64_DW(2, 101);
  j = vec_permdi(i1, i2, 2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("permdi ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("     ,2)=", j);
#endif
  rc += check_vuint128x ("vec_permdi:", (vui128_t)j, (vui128_t) e);

  e = (vui64_t )CONST_VINT64_DW(2, 102);
  j = vec_permdi(i1, i2, 3);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("permdi ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("     ,3)=", j);
#endif
  rc += check_vuint128x ("vec_permdi:", (vui128_t)j, (vui128_t) e);

  e = (vui64_t )CONST_VINT64_DW(1, 1);
  j = vec_spltd(i1, 0);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("spltd  ( ", i1);
  print_v2xint64 ("     ,0)=", j);
#endif
  rc += check_vuint128x ("vec_spltd (x,0):", (vui128_t)j, (vui128_t) e);

  e = (vui64_t )CONST_VINT64_DW(2, 2);
  j = vec_spltd(i1, 1);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("spltd  ( ", i1);
  print_v2xint64 ("     ,1)=", j);
#endif
  rc += check_vuint128x ("vec_spltd (x,1):", (vui128_t)j, (vui128_t) e);

  e = (vui64_t )CONST_VINT64_DW(2, 1);
  j = vec_swapd(i1);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("swapd  ( ", i1);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_swapd (x):", (vui128_t)j, (vui128_t) e);

  e = (vui64_t )CONST_VINT64_DW(1, 101);
  j = vec_mrghd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("mrghd  ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_mrghd :", (vui128_t)j, (vui128_t) e);

  e = (vui64_t )CONST_VINT64_DW(2, 102);
  j = vec_mrgld(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("mrgld  ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_mrgld :", (vui128_t)j, (vui128_t) e);

  return (rc);
}

int
test_revbd (void)
{
  vui32_t i, e, *ip;
  vui128_t k;
  unsigned char mem[16] __attribute__ ((aligned (16))) = { 0xf0, 0xf1, 0xf2,
      0xf3, 0xe0, 0xe1, 0xe2, 0xe3, 0xd0, 0xd1, 0xd2, 0xd3, 0xc0, 0xc1, 0xc2,
      0xc3 };
  int rc = 0;

  printf ("\ntest_revbd Reverse Bytes in doublewords\n");

  i = (vui32_t ) { 0, 1, 2, 3 };
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  e = (vui32_t )CONST_VINT32_W(0x01000000, 0x00000000, 0x03000000, 0x02000000);
#else
  e = (vui32_t)CONST_VINT32_W(0x02000000, 0x03000000, 0x00000000, 0x01000000);
#endif
  k = (vui128_t) vec_revbd ((vui64_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_revbd i=", (vui128_t)i);
  print_vint128x ("         k=", k);
#endif
  rc += check_vuint128x ("vec_revbd 1:", k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0x01020304, 0x11121314, 0x21222324, 0x31323334);
  e = (vui32_t )CONST_VINT32_W(0x14131211, 0x04030201, 0x34333231, 0x24232221);
  k = (vui128_t) vec_revbd ((vui64_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_revbd i=", (vui128_t)i);
  print_vint128x ("         k=", k);
#endif
  rc += check_vuint128x ("vec_revbd 2:", k, (vui128_t) e);

  ip = (vui32_t*) mem;
  i = *ip;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  e = (vui32_t )CONST_VINT32_W(0xe3e2e1e0, 0xf3f2f1f0, 0xc3c2c1c0, 0xd3d2d1d0);
#else
  e = (vui32_t)CONST_VINT32_W(0xd0d1d2d3, 0xc0c1c2c3, 0xf0f1f2f3, 0xe0e1e2e3);
#endif
  k = (vui128_t) vec_revbd ((vui64_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_revbd i=", (vui128_t)i);
  print_vint128x ("         k=", k);
#endif
  rc += check_vuint128x ("vec_revbd 3:", k, (vui128_t) e);

  return (rc);
}

int
test_popcntd (void)
{
  vui64_t i, e;
  vui64_t j;
  int rc = 0;

  printf ("\ntest_popcntd Vector Pop Count doubleword\n");

  i = (vui64_t )CONST_VINT64_DW(0, 0);
  e = (vui64_t )CONST_VINT64_DW(0, 0);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({0, 0) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(0, -1);
  e = (vui64_t )CONST_VINT64_DW(0, 64);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({0, -1) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(-1, 0);
  e = (vui64_t )CONST_VINT64_DW(64, 0);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({-1, 0) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(-1, -1);
  e = (vui64_t )CONST_VINT64_DW(64, 64);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({-1, -1) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(1, 8589934596);
  e = (vui64_t )CONST_VINT64_DW(1, 2);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({1, 8589934596) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(34359738384, 137438953536);
  e = (vui64_t )CONST_VINT64_DW(2, 2);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({34359738384, 137438953536) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(549755814144, 2199023256576);
  e = (vui64_t )CONST_VINT64_DW(2, 2);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({549755814144, 2199023256576) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(8796093026304, 35184372105216);
  e = (vui64_t )CONST_VINT64_DW(2, 2);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({8796093026304, 35184372105216) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(140737488420864, 562949953683456);
  e = (vui64_t )CONST_VINT64_DW(2, 2);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({140737488420864, 562949953683456) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(2251799814733824, 9007199258935296);
  e = (vui64_t )CONST_VINT64_DW(2, 2);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({2251799814733824, 9007199258935296) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(36028797035741184, 144115188142964736);
  e = (vui64_t )CONST_VINT64_DW(2, 2);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({36028797035741184, 144115188142964736) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(576460752571858944, 2305843010287435776);
  e = (vui64_t )CONST_VINT64_DW(2, 2);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({576460752571858944, 2305843010287435776) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(429496725405032703, 429496725405032703);
  e = (vui64_t )CONST_VINT64_DW(38, 38);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({429496725405032703, 429496725405032703) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(381774867026695736, 381774867026695736);
  e = (vui64_t )CONST_VINT64_DW(24, 24);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({381774867026695736, 381774867026695736) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(1000000000, 1000000000);
  e = (vui64_t )CONST_VINT64_DW(13, 13);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({1000000000, 1000000000) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(10000000000, 1000000000);
  e = (vui64_t )CONST_VINT64_DW(11, 13);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({10000000000, 1000000000) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(1000000000, 10000000000);
  e = (vui64_t )CONST_VINT64_DW(13, 11);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({1000000000, 10000000000) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(1000000000000000000, 0);
  e = (vui64_t )CONST_VINT64_DW(24, 0);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({1000000000000000000, 0) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(0, 1000000000000000000);
  e = (vui64_t )CONST_VINT64_DW(0, 24);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({0, 1000000000000000000) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(0, 1000000000000000000);
  e = (vui64_t )CONST_VINT64_DW(0, 24);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({0, 1000000000000000000) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  return (rc);
}

//#undef __DEBUG_PRINT__
int
test_clzd (void)
{
  vui64_t i, e;
  vui64_t j;
  int rc = 0;

  printf ("\ntest_clzd Vector Count Leading Zeros in doublewords\n");

  i = (vui64_t )CONST_VINT64_DW(0, 0);
  e = (vui64_t )CONST_VINT64_DW(64, 64);
  j = vec_clzd((vui64_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0, 0) ", j);
#endif
  rc += check_vuint128x ("vec_clzd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(-1, -1);
  e = (vui64_t )CONST_VINT64_DW(0, 0);
  j = vec_clzd((vui64_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(-1, -1) ", j);
#endif
  rc += check_vuint128x ("vec_clzd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(0, -1);
  e = (vui64_t )CONST_VINT64_DW(64, 0);
  j = vec_clzd((vui64_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0, -1) ", j);
#endif
  rc += check_vuint128x ("vec_clzd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(-1, 0);
  e = (vui64_t )CONST_VINT64_DW(0, 64);
  j = vec_clzd((vui64_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(-1, 0) ", j);
#endif
  rc += check_vuint128x ("vec_clzd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(1, 8589934596);
  e = (vui64_t )CONST_VINT64_DW(63, 30);
  j = vec_clzd((vui64_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(1, 8589934596) ", j);
#endif
  rc += check_vuint128x ("vec_clzd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(34359738384, 137438953536);
  e = (vui64_t )CONST_VINT64_DW(28, 26);
  j = vec_clzd((vui64_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(34359738384, 137438953536) ", j);
#endif
  rc += check_vuint128x ("vec_clzd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(549755814144, 2199023256576);
  e = (vui64_t )CONST_VINT64_DW(24, 22);
  j = vec_clzd((vui64_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(549755814144, 2199023256576) ", j);
#endif
  rc += check_vuint128x ("vec_clzd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(8796093026304, 35184372105216);
  e = (vui64_t )CONST_VINT64_DW(20, 18);
  j = vec_clzd((vui64_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(8796093026304, 35184372105216) ", j);
#endif
  rc += check_vuint128x ("vec_clzd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(140737488420864, 562949953683456);
  e = (vui64_t )CONST_VINT64_DW(16, 14);
  j = vec_clzd((vui64_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(140737488420864, 562949953683456) ", j);
#endif
  rc += check_vuint128x ("vec_clzd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(2251799814733824, 9007199258935296);
  e = (vui64_t )CONST_VINT64_DW(12, 10);
  j = vec_clzd((vui64_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(2251799814733824, 9007199258935296) ", j);
#endif
  rc += check_vuint128x ("vec_clzd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(36028797035741184, 144115188142964736);
  e = (vui64_t )CONST_VINT64_DW(8, 6);
  j = vec_clzd((vui64_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(36028797035741184, 144115188142964736) ", j);
#endif
  rc += check_vuint128x ("vec_clzd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t )CONST_VINT64_DW(576460752571858944, 2305843010287435776);
  e = (vui64_t )CONST_VINT64_DW(4, 2);
  j = vec_clzd((vui64_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(576460752571858944, 2305843010287435776) ", j);
#endif
  rc += check_vuint128x ("vec_clzd:", (vui128_t)j, (vui128_t) e);

  return (rc);
}

int
test_muleud (void)
{
  vui64_t i, j;
  vui128_t k, e;
  int rc = 0;

  printf ("\ntest_muleud Vector Multiply Even Unsigned doublewords\n");
#if 0
  i = (vui64_t )CONST_VINT32_W(1, 2, 3, 4);
  j = (vui64_t )CONST_VINT32_W(10, 20, 30, 40);
  e = (vui128_t )CONST_VINT128_DW128(10, 90);
#else
  i = (vui64_t ){1, 2};
  j = (vui64_t ){101, 102};
  e = (vui128_t )CONST_VINT128_DW128(0, 101);
#endif
  k = vec_muleud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({1, 2, 3, 4}, {10, 20, 30, 40}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t ){1000000000, 1000000000};
  j = (vui64_t ){1000000000, 1000000000};
  e = (vui128_t )CONST_VINT128_DW128(0, 1000000000000000000UL);
  k = vec_muleud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({1000000000, 1000000000}, {1000000000, 1000000000}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t ){10000000000, 1000000000};
  j = (vui64_t ){10000000000, 1000000000};
  e = (vui128_t )CONST_VINT128_DW128(0x5, 0x6bc75e2d63100000UL);
  k = vec_muleud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({10000000000, 1000000000}, {10000000000, 1000000000}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t ){1000000000000000000UL, 0};
  j = (vui64_t ){1000000000000000000UL, 0};
  e = (vui128_t )CONST_VINT128_DW128(0x00c097ce7bc90715UL, 0xb34b9f1000000000UL);
  k = vec_muleud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({10**18, 0}, {10**18, 0}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t ){0, 1000000000000000000UL};
  j = (vui64_t ){0, 1000000000000000000UL};
  e = (vui128_t )CONST_VINT128_DW128(0x0UL, 0x0UL);
  k = vec_muleud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({0, 10**18}, {0, 10**18}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t ){100000000000UL, 100000000000UL};
  j = (vui64_t ){100000000000UL, 100000000000UL};
  e = (vui128_t )CONST_VINT128_DW128(0x021eUL, 0x19e0c9bab2400000UL);
  k = vec_muleud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({10**11, 10**11}, {10**11, 10**11}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t ){0x00000000ffffffffUL, 0x00000000ffffffffUL};
  j = (vui64_t ){0x00000000ffffffffUL, 0x00000000ffffffffUL};
  e = (vui128_t )CONST_VINT128_DW128(0x0UL, 0xfffffffe00000001UL);
  k = vec_muleud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({2**32-1, 2**32-1}, {2**32-1, 2**32-1}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t ){0xffffffff00000000UL, 0xffffffff00000000UL};
  j = (vui64_t ){0xffffffff00000000UL, 0xffffffff00000000UL};
  e = (vui128_t )CONST_VINT128_DW128(0xfffffffe00000001UL, 0x0UL);
  k = vec_muleud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({2**64-2**32 , 2**64-2**32}, {2**64-2**32 , 2**64-2**32}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t ){0xffffffffffffffffUL, 0x0000000000000000UL};
  j = (vui64_t ){0xffffffffffffffffUL, 0x0000000000000000UL};
  e = (vui128_t )CONST_VINT128_DW128(0xfffffffffffffffeUL, 0x0000000000000001UL);
  k = vec_muleud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({2**64-1 , 0}, {2**64-1 , 0}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t ){2000000000000000000UL, 1000000000000000000UL};
  j = (vui64_t ){1UL, -1UL};
  e = (vui128_t )CONST_VINT128_DW128(0UL, 2000000000000000000UL);
  k = vec_muleud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({2**64-1 , 0}, {2**64-1 , 0}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  return (rc);
}

int
test_muloud (void)
{
  vui64_t i, j;
  vui128_t k, e;
  int rc = 0;

  printf ("\ntest_muloud Vector Multiply Odd Unsigned doublewords\n");
#if 0
  i = (vui64_t )CONST_VINT32_W(1, 2, 3, 4);
  j = (vui64_t )CONST_VINT32_W(10, 20, 30, 40);
  e = (vui128_t )CONST_VINT128_DW128(10, 90);
#else
  i = (vui64_t ){1, 2};
  j = (vui64_t ){101, 102};
  e = (vui128_t )CONST_VINT128_DW128(0, 204);
#endif
  k = vec_muloud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({1, 2, 3, 4}, {10, 20, 30, 40}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t ){1000000000, 1000000000};
  j = (vui64_t ){1000000000, 1000000000};
  e = (vui128_t )CONST_VINT128_DW128(0, 1000000000000000000UL);
  k = vec_muloud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({1000000000, 1000000000}, {1000000000, 1000000000}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t ){10000000000, 1000000000};
  j = (vui64_t ){10000000000, 1000000000};
  e = (vui128_t )CONST_VINT128_DW128(0x0, 0x0de0b6b3a7640000UL);
  k = vec_muloud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({10000000000, 1000000000}, {10000000000, 1000000000}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t ){1000000000000000000UL, 0};
  j = (vui64_t ){1000000000000000000UL, 0};
  e = (vui128_t )CONST_VINT128_DW128(0, 0);
  k = vec_muloud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({10**18, 0}, {10**18, 0}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t ){0, 1000000000000000000UL};
  j = (vui64_t ){0, 1000000000000000000UL};
  e = (vui128_t )CONST_VINT128_DW128(0x00c097ce7bc90715UL, 0xb34b9f1000000000UL);
  k = vec_muloud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({0, 10**18}, {0, 10**18}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t ){100000000000UL, 100000000000UL};
  j = (vui64_t ){100000000000UL, 100000000000UL};
  e = (vui128_t )CONST_VINT128_DW128(0x021eUL, 0x19e0c9bab2400000UL);
  k = vec_muloud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({10**11, 10**11}, {10**11, 10**11}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t ){0x00000000ffffffffUL, 0x00000000ffffffffUL};
  j = (vui64_t ){0x00000000ffffffffUL, 0x00000000ffffffffUL};
  e = (vui128_t )CONST_VINT128_DW128(0x0UL, 0xfffffffe00000001UL);
  k = vec_muloud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({2**32-1, 2**32-1}, {2**32-1, 2**32-1}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t ){0xffffffff00000000UL, 0xffffffff00000000UL};
  j = (vui64_t ){0xffffffff00000000UL, 0xffffffff00000000UL};
  e = (vui128_t )CONST_VINT128_DW128(0xfffffffe00000001UL, 0x0UL);
  k = vec_muloud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({2**64-2**32 , 2**64-2**32}, {2**64-2**32 , 2**64-2**32}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t ){ 0x0000000000000000UL, 0xffffffffffffffffUL};
  j = (vui64_t ){ 0x0000000000000000UL, 0xffffffffffffffffUL};
  e = (vui128_t )CONST_VINT128_DW128(0xfffffffffffffffeUL, 0x0000000000000001UL);
  k = vec_muloud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({2**64-1 , 0}, {2**64-1 , 0}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t ){2000000000000000000UL, 1000000000000000000UL};
  j = (vui64_t ){1UL, -1L};
  e = (vui128_t )CONST_VINT128_DW128(0x0de0b6b3a763ffffUL, -1000000000000000000L);
  k = vec_muloud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({2**64-1 , 0}, {2**64-1 , 0}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  return (rc);
}

int
test_vec_i64 (void)
{
  int rc = 0;

  printf ("\n%s\n", __FUNCTION__);

  rc += test_permdi ();
  rc += test_revbd ();
  rc += test_clzd ();
  rc += test_popcntd();
  rc += test_vsld ();
  rc += test_vsrd ();
  rc += test_vsrad ();
  rc += test_muleud ();
  rc += test_muloud ();

  return (rc);
}
