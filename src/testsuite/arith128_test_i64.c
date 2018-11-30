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
  vui64_t sel_mask = CONST_VINT128_DW (0, -1LL);
  vui64_t shft_mask = CONST_VINT128_DW (63, 63);
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
  vui64_t sel_mask = CONST_VINT128_DW (0, -1LL);
  vui64_t shft_mask = CONST_VINT128_DW (63, 63);
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
//  vui64_t sel_mask = CONST_VINT128_DW (0, -1LL);
  vui64_t shft_mask = CONST_VINT128_DW (63, 63);
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

int
test_vrld (void)
{
  vui64_t i1, i2, e;
  vui64_t j;
  int rc = 0;

  printf ("\ntest_vrld Vector Rotate left doubleword\n");

  i1 = (vui64_t) CONST_VINT64_DW (1, 2);
  i2 = (vui64_t) CONST_VINT64_DW (2, 1);
  e = (vui64_t) CONST_VINT64_DW (4, 4);
  j = vec_vrld(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vrld   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vrld:", (vui128_t)j, (vui128_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x0800000000000010UL);
  i2 = (vui64_t) CONST_VINT64_DW (1, 2);
  e = (vui64_t) CONST_VINT64_DW (0x0000000000000003UL, 0x2000000000000040UL);
  j = vec_vrld(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vrld   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vrld:", (vui128_t)j, (vui128_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x0800000000000010UL);
  i2 = (vui64_t) CONST_VINT64_DW (2, 4);
  e = (vui64_t) CONST_VINT64_DW (0x0000000000000006UL, 0x8000000000000100UL);
  j = vec_vrld(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vrld   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vrld:", (vui128_t)j, (vui128_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x0800000000000010UL);
  i2 = (vui64_t) CONST_VINT64_DW (32, 32);
  e = (vui64_t) CONST_VINT64_DW (0x0000000180000000UL, 0x0000001008000000UL);
  j = vec_vrld(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vrld   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vrld:", (vui128_t)j, (vui128_t) e);

  return (rc);
}

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

  i1 = (vui64_t) CONST_VINT64_DW (1, 2);
  i2 = (vui64_t) CONST_VINT64_DW (2, 1);
  e = (vui64_t) CONST_VINT64_DW (4, 4);
  j = test_vec_vsld(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsld   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vsld:", (vui128_t)j, (vui128_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vui64_t) CONST_VINT64_DW (33, 31);
  e = (vui64_t) CONST_VINT64_DW (0xfffffffe00000000, 0xffffffff80000000);
  j = test_vec_vsld(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsld   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vsld:", (vui128_t)j, (vui128_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vui64_t) CONST_VINT64_DW (63, 60);
  e = (vui64_t) CONST_VINT64_DW (0x8000000000000000, 0xf000000000000000);
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

  i1 = (vui64_t) CONST_VINT64_DW (4, 2);
  i2 = (vui64_t) CONST_VINT64_DW (2, 1);
  e = (vui64_t) CONST_VINT64_DW (1, 1);
  j = test_vec_vsrd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrd   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vsrd:", (vui128_t)j, (vui128_t) e);
#if 1
  i1 = (vui64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vui64_t) CONST_VINT64_DW (33, 31);
  e = (vui64_t) CONST_VINT64_DW (0x000000007fffffff, 0x00000001ffffffff);
  j = test_vec_vsrd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrd   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vsrd:", (vui128_t)j, (vui128_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vui64_t) CONST_VINT64_DW (63, 60);
  e = (vui64_t) CONST_VINT64_DW (0x1, 0xf);
  j = test_vec_vsrd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrd   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vsrd:", (vui128_t)j, (vui128_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (100000000000, 100000000000);
  i2 = (vui64_t) CONST_VINT64_DW (33, 31);
  e = (vui64_t) CONST_VINT64_DW (11, 46);
  j = test_vec_vsrd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrd   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vsrd:", (vui128_t)j, (vui128_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (100000000000, 100000000000);
  i2 = (vui64_t) CONST_VINT64_DW (48, 40);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
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

  i1 = (vi64_t) CONST_VINT64_DW (4, 2);
  i2 = (vui64_t) CONST_VINT64_DW (2, 1);
  e = (vi64_t) CONST_VINT64_DW (1, 1);
  j = test_vec_vsrad(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrad  ( ", (vui64_t)i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_vsrad:", (vui128_t)j, (vui128_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-4, -2);
  i2 = (vui64_t) CONST_VINT64_DW (2, 1);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = test_vec_vsrad(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrad  ( ", (vui64_t)i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_vsrad:", (vui128_t)j, (vui128_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000, 0x7fffffffffffffff);
  i2 = (vui64_t) CONST_VINT64_DW (63, 63);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = test_vec_vsrad(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrad  ( ", (vui64_t)i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_vsrad:", (vui128_t)j, (vui128_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0x7fffffffffffffff, 0x8000000000000000);
  i2 = (vui64_t) CONST_VINT64_DW (63, 63);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
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
test_mrghld (void)
{
  vui64_t i1, i2, e;
  vui64_t j;
  int rc = 0;

  printf ("\ntest_merge Vector Merge High/Low Doubleword\n");

  i1 = (vui64_t) { 1, 2 };
  i2 = (vui64_t) { 101, 102 };

  e = (vui64_t) { 1, 101 };
  j = vec_mrghd (i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("mrghd  ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_mrghd :", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) { 2, 102 };
  j = vec_mrgld (i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("mrgld  ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_mrgld :", (vui128_t) j, (vui128_t) e);

  return (rc);
}

int
test_mrgeod (void)
{
  vui64_t i1, i2, e;
  vui64_t j;
  int rc = 0;

  printf ("\ntest_merge Vector Merge Even/Odd Doubleword\n");

  i1 = (vui64_t) { 1, 2 };
  i2 = (vui64_t) { 101, 102 };

  e = (vui64_t) { 1, 101 };
  j = vec_mrged (i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("mrged  ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_mrged :", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) { 2, 102 };
  j = vec_mrgod (i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("mrgod  ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_mrgod :", (vui128_t) j, (vui128_t) e);

  return (rc);
}

int
test_mrgahld (void)
{
  vui64_t i1, i2, e;
  vui64_t j;
  int rc = 0;

  printf ("\ntest_permdi Vector Merge Algebraic High/Low Doubleword\n");

  i1 = (vui64_t) CONST_VINT64_DW (1, 2);
  i2 = (vui64_t) CONST_VINT64_DW (101, 102);

  e = (vui64_t) CONST_VINT64_DW (1, 101);
  j = vec_mrgahd((vui128_t)i1, (vui128_t)i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("mrgahd  ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_mrgahd :", (vui128_t)j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (2, 102);
  j = vec_mrgald((vui128_t)i1, (vui128_t)i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("mrgald  ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_mrgald :", (vui128_t)j, (vui128_t) e);

  return (rc);
}

int
test_splatd (void)
{
  vui64_t i1, e;
  vui64_t j;
  int rc = 0;

  printf ("\ntest_splatd Vector splat doubleword immediate\n");

  i1 = (vui64_t) { 1, 2 };

  e = (vui64_t) { 1, 1 };
  j = vec_splatd(i1, 0);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("splatd  ( ", i1);
  print_v2xint64 ("     ,0)=", j);
#endif
  rc += check_vuint128x ("vec_splatd (x,0):", (vui128_t)j, (vui128_t) e);

  e = (vui64_t) { 2, 2 };
  j = vec_splatd(i1, 1);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("splatd  ( ", i1);
  print_v2xint64 ("     ,1)=", j);
#endif
  rc += check_vuint128x ("vec_splatd (x,1):", (vui128_t)j, (vui128_t) e);

  return (rc);
}

int
test_xxspltd (void)
{
  vui64_t i1, e;
  vui64_t j;
  int rc = 0;

  printf ("\ntest_permdi Vector splat doubleword immediate\n");

  i1 = (vui64_t) CONST_VINT64_DW (1, 2);

  e = (vui64_t) CONST_VINT64_DW (1, 1);
  j = vec_xxspltd(i1, 0);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("spltd  ( ", i1);
  print_v2xint64 ("     ,0)=", j);
#endif
  rc += check_vuint128x ("vec_xxspltd (x,0):", (vui128_t)j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (2, 2);
  j = vec_xxspltd(i1, 1);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("spltd  ( ", i1);
  print_v2xint64 ("     ,1)=", j);
#endif
  rc += check_vuint128x ("vec_xxspltd (x,1):", (vui128_t)j, (vui128_t) e);

  return (rc);
}

int
test_permdi (void)
{
  vui64_t i1, i2, e;
  vui64_t j;
  int rc = 0;

  printf ("\ntest_permdi Vector permute doubleword immediate\n");

  i1 = (vui64_t) CONST_VINT64_DW (1, 2);
  i2 = (vui64_t) CONST_VINT64_DW (101, 102);
  e = (vui64_t) CONST_VINT64_DW (1, 101);
  j = vec_permdi(i1, i2, 0);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("permdi ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("     ,0)=", j);
#endif
  rc += check_vuint128x ("vec_permdi:", (vui128_t)j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (1, 102);
  j = vec_permdi(i1, i2, 1);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("permdi ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("     ,1)=", j);
#endif
  rc += check_vuint128x ("vec_permdi:", (vui128_t)j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (2, 101);
  j = vec_permdi(i1, i2, 2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("permdi ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("     ,2)=", j);
#endif
  rc += check_vuint128x ("vec_permdi:", (vui128_t)j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (2, 102);
  j = vec_permdi(i1, i2, 3);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("permdi ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("     ,3)=", j);
#endif
  rc += check_vuint128x ("vec_permdi:", (vui128_t)j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (2, 1);
  j = vec_swapd(i1);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("swapd  ( ", i1);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_swapd (x):", (vui128_t)j, (vui128_t) e);

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

  i = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({0, 0) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (0, -1);
  e = (vui64_t) CONST_VINT64_DW (0, 64);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({0, -1) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (-1, 0);
  e = (vui64_t) CONST_VINT64_DW (64, 0);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({-1, 0) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (-1, -1);
  e = (vui64_t) CONST_VINT64_DW (64, 64);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({-1, -1) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (1, 8589934596);
  e = (vui64_t) CONST_VINT64_DW (1, 2);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({1, 8589934596) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (34359738384, 137438953536);
  e = (vui64_t) CONST_VINT64_DW (2, 2);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({34359738384, 137438953536) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (549755814144, 2199023256576);
  e = (vui64_t) CONST_VINT64_DW (2, 2);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({549755814144, 2199023256576) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (8796093026304, 35184372105216);
  e = (vui64_t) CONST_VINT64_DW (2, 2);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({8796093026304, 35184372105216) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (140737488420864, 562949953683456);
  e = (vui64_t) CONST_VINT64_DW (2, 2);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({140737488420864, 562949953683456) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (2251799814733824, 9007199258935296);
  e = (vui64_t) CONST_VINT64_DW (2, 2);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({2251799814733824, 9007199258935296) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (36028797035741184, 144115188142964736);
  e = (vui64_t) CONST_VINT64_DW (2, 2);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({36028797035741184, 144115188142964736) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (576460752571858944, 2305843010287435776);
  e = (vui64_t) CONST_VINT64_DW (2, 2);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({576460752571858944, 2305843010287435776) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (429496725405032703, 429496725405032703);
  e = (vui64_t) CONST_VINT64_DW (38, 38);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({429496725405032703, 429496725405032703) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (381774867026695736, 381774867026695736);
  e = (vui64_t) CONST_VINT64_DW (24, 24);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({381774867026695736, 381774867026695736) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (1000000000, 1000000000);
  e = (vui64_t) CONST_VINT64_DW (13, 13);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({1000000000, 1000000000) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (10000000000, 1000000000);
  e = (vui64_t) CONST_VINT64_DW (11, 13);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({10000000000, 1000000000) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (1000000000, 10000000000);
  e = (vui64_t) CONST_VINT64_DW (13, 11);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({1000000000, 10000000000) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (1000000000000000000, 0);
  e = (vui64_t) CONST_VINT64_DW (24, 0);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({1000000000000000000, 0) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (0, 1000000000000000000);
  e = (vui64_t) CONST_VINT64_DW (0, 24);
  j = vec_popcntd(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({0, 1000000000000000000) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (0, 1000000000000000000);
  e = (vui64_t) CONST_VINT64_DW (0, 24);
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

  i = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (64, 64);
  j = vec_clzd((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0, 0) ", j);
#endif
  rc += check_vuint128x ("vec_clzd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (-1, -1);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_clzd((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(-1, -1) ", j);
#endif
  rc += check_vuint128x ("vec_clzd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (0, -1);
  e = (vui64_t) CONST_VINT64_DW (64, 0);
  j = vec_clzd((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0, -1) ", j);
#endif
  rc += check_vuint128x ("vec_clzd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (-1, 0);
  e = (vui64_t) CONST_VINT64_DW (0, 64);
  j = vec_clzd((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(-1, 0) ", j);
#endif
  rc += check_vuint128x ("vec_clzd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (1, 8589934596);
  e = (vui64_t) CONST_VINT64_DW (63, 30);
  j = vec_clzd((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(1, 8589934596) ", j);
#endif
  rc += check_vuint128x ("vec_clzd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (34359738384, 137438953536);
  e = (vui64_t) CONST_VINT64_DW (28, 26);
  j = vec_clzd((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(34359738384, 137438953536) ", j);
#endif
  rc += check_vuint128x ("vec_clzd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (549755814144, 2199023256576);
  e = (vui64_t) CONST_VINT64_DW (24, 22);
  j = vec_clzd((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(549755814144, 2199023256576) ", j);
#endif
  rc += check_vuint128x ("vec_clzd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (8796093026304, 35184372105216);
  e = (vui64_t) CONST_VINT64_DW (20, 18);
  j = vec_clzd((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(8796093026304, 35184372105216) ", j);
#endif
  rc += check_vuint128x ("vec_clzd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (140737488420864, 562949953683456);
  e = (vui64_t) CONST_VINT64_DW (16, 14);
  j = vec_clzd((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(140737488420864, 562949953683456) ", j);
#endif
  rc += check_vuint128x ("vec_clzd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (2251799814733824, 9007199258935296);
  e = (vui64_t) CONST_VINT64_DW (12, 10);
  j = vec_clzd((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(2251799814733824, 9007199258935296) ", j);
#endif
  rc += check_vuint128x ("vec_clzd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (36028797035741184, 144115188142964736);
  e = (vui64_t) CONST_VINT64_DW (8, 6);
  j = vec_clzd((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(36028797035741184, 144115188142964736) ", j);
#endif
  rc += check_vuint128x ("vec_clzd:", (vui128_t)j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (576460752571858944, 2305843010287435776);
  e = (vui64_t) CONST_VINT64_DW (4, 2);
  j = vec_clzd((vui64_t)i);

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
  i = (vui64_t){1, 2};
  j = (vui64_t){101, 102};
  e = (vui128_t) CONST_VINT128_DW128(0, 101);
  k = vec_muleud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({1, 2, 3, 4}, {10, 20, 30, 40}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){1000000000, 1000000000};
  j = (vui64_t){1000000000, 1000000000};
  e = (vui128_t) CONST_VINT128_DW128(0, 1000000000000000000UL);
  k = vec_muleud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({1000000000, 1000000000}, {1000000000, 1000000000}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){10000000000, 1000000000};
  j = (vui64_t){10000000000, 1000000000};
  e = (vui128_t) CONST_VINT128_DW128(0x5, 0x6bc75e2d63100000UL);
  k = vec_muleud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({10000000000, 1000000000}, {10000000000, 1000000000}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){1000000000000000000UL, 0};
  j = (vui64_t){1000000000000000000UL, 0};
  e = (vui128_t) CONST_VINT128_DW128(0x00c097ce7bc90715UL, 0xb34b9f1000000000UL);
  k = vec_muleud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({10**18, 0}, {10**18, 0}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){0, 1000000000000000000UL};
  j = (vui64_t){0, 1000000000000000000UL};
  e = (vui128_t) CONST_VINT128_DW128(0x0UL, 0x0UL);
  k = vec_muleud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({0, 10**18}, {0, 10**18}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){100000000000UL, 100000000000UL};
  j = (vui64_t){100000000000UL, 100000000000UL};
  e = (vui128_t) CONST_VINT128_DW128(0x021eUL, 0x19e0c9bab2400000UL);
  k = vec_muleud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({10**11, 10**11}, {10**11, 10**11}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){0x00000000ffffffffUL, 0x00000000ffffffffUL};
  j = (vui64_t){0x00000000ffffffffUL, 0x00000000ffffffffUL};
  e = (vui128_t) CONST_VINT128_DW128(0x0UL, 0xfffffffe00000001UL);
  k = vec_muleud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({2**32-1, 2**32-1}, {2**32-1, 2**32-1}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){0xffffffff00000000UL, 0xffffffff00000000UL};
  j = (vui64_t){0xffffffff00000000UL, 0xffffffff00000000UL};
  e = (vui128_t) CONST_VINT128_DW128(0xfffffffe00000001UL, 0x0UL);
  k = vec_muleud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({2**64-2**32 , 2**64-2**32}, {2**64-2**32 , 2**64-2**32}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){0xffffffffffffffffUL, 0x0000000000000000UL};
  j = (vui64_t){0xffffffffffffffffUL, 0x0000000000000000UL};
  e = (vui128_t) CONST_VINT128_DW128(0xfffffffffffffffeUL, 0x0000000000000001UL);
  k = vec_muleud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({2**64-1 , 0}, {2**64-1 , 0}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){2000000000000000000UL, 1000000000000000000UL};
  j = (vui64_t){1UL, -1UL};
  e = (vui128_t) CONST_VINT128_DW128(0UL, 2000000000000000000UL);
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
  i = (vui64_t){1, 2};
  j = (vui64_t){101, 102};
  e = (vui128_t) CONST_VINT128_DW128(0, 204);
  k = vec_muloud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({1, 2, 3, 4}, {10, 20, 30, 40}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){1000000000, 1000000000};
  j = (vui64_t){1000000000, 1000000000};
  e = (vui128_t) CONST_VINT128_DW128(0, 1000000000000000000UL);
  k = vec_muloud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({1000000000, 1000000000}, {1000000000, 1000000000}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){10000000000, 1000000000};
  j = (vui64_t){10000000000, 1000000000};
  e = (vui128_t) CONST_VINT128_DW128(0x0, 0x0de0b6b3a7640000UL);
  k = vec_muloud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({10000000000, 1000000000}, {10000000000, 1000000000}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){1000000000000000000UL, 0};
  j = (vui64_t){1000000000000000000UL, 0};
  e = (vui128_t) CONST_VINT128_DW128(0, 0);
  k = vec_muloud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({10**18, 0}, {10**18, 0}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){0, 1000000000000000000UL};
  j = (vui64_t){0, 1000000000000000000UL};
  e = (vui128_t) CONST_VINT128_DW128(0x00c097ce7bc90715UL, 0xb34b9f1000000000UL);
  k = vec_muloud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({0, 10**18}, {0, 10**18}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){100000000000UL, 100000000000UL};
  j = (vui64_t){100000000000UL, 100000000000UL};
  e = (vui128_t) CONST_VINT128_DW128(0x021eUL, 0x19e0c9bab2400000UL);
  k = vec_muloud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({10**11, 10**11}, {10**11, 10**11}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){0x00000000ffffffffUL, 0x00000000ffffffffUL};
  j = (vui64_t){0x00000000ffffffffUL, 0x00000000ffffffffUL};
  e = (vui128_t) CONST_VINT128_DW128(0x0UL, 0xfffffffe00000001UL);
  k = vec_muloud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({2**32-1, 2**32-1}, {2**32-1, 2**32-1}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){0xffffffff00000000UL, 0xffffffff00000000UL};
  j = (vui64_t){0xffffffff00000000UL, 0xffffffff00000000UL};
  e = (vui128_t) CONST_VINT128_DW128(0xfffffffe00000001UL, 0x0UL);
  k = vec_muloud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({2**64-2**32 , 2**64-2**32}, {2**64-2**32 , 2**64-2**32}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){ 0x0000000000000000UL, 0xffffffffffffffffUL};
  j = (vui64_t){ 0x0000000000000000UL, 0xffffffffffffffffUL};
  e = (vui128_t) CONST_VINT128_DW128(0xfffffffffffffffeUL, 0x0000000000000001UL);
  k = vec_muloud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({2**64-1 , 0}, {2**64-1 , 0}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){2000000000000000000UL, 1000000000000000000UL};
  j = (vui64_t){1UL, -1L};
  e = (vui128_t) CONST_VINT128_DW128(0x0de0b6b3a763ffffUL, -1000000000000000000L);
  k = vec_muloud(i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({2**64-1 , 0}, {2**64-1 , 0}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  return (rc);
}

int
test_mulhud (void)
{
  vui64_t i, j;
  vui64_t k, e;
  int rc = 0;

  printf ("\ntest_mulhud Vector Multiply High Unsigned doublewords\n");

  i = (vui64_t) { 1, 2 };
  j = (vui64_t) { 101, 102 };
  e = (vui64_t) { 0, 0 };
  k = vec_mulhud( i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("mulhud  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulhud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) {-1, -1 };
  j = (vui64_t) { 1, 2 };
  e = (vui64_t) { 0, 1 };
  k = vec_mulhud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("mulhud  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulhud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){ -1, -1 };
  j = (vui64_t){ 3, 4 };
  e = (vui64_t){ 2, 3 };
  k = vec_mulhud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("mulhud  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulhud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){ 0x100000000, 0x100000000 };
  j = (vui64_t){ 0x100000000, 0x200000000 };
  e = (vui64_t){ 0x1, 0x2 };
  k = vec_mulhud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("mulhud  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulhud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){ 0x200000000, 0x200000000 };
  j = (vui64_t){ 0x300000000, 0x400000000 };
  e = (vui64_t){ 0x6, 0x8 };
  k = vec_mulhud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("mulhud  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulhud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){ 1000000000000000000, 1000000000000000000 };
  j = (vui64_t){ 19, 18447 };
  e = (vui64_t){ 1, 1000 };
  k = vec_mulhud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("mulhud  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulhud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){ 1000000000000000000, 1000000000000000000 };
  j = (vui64_t){ 18446745, 18446744074 };
  e = (vui64_t){ 1000000, 1000000000 };
  k = vec_mulhud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("mulhud  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulhud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){ __UINT64_MAX__, __UINT64_MAX__ };
  j = (vui64_t){ 19, 18447 };
  e = (vui64_t){ 18, 18446 };
  k = vec_mulhud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("mulhud  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulhud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){ __UINT64_MAX__, __UINT64_MAX__ };
  j = (vui64_t){ 18446745, 18446744074 };
  e = (vui64_t){ 18446744, 18446744073 };
  k = vec_mulhud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("mulhud  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulhud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){ __UINT64_MAX__, __UINT64_MAX__ };
  j = (vui64_t){ __UINT64_MAX__, __UINT64_MAX__ };
  e = (vui64_t){ __UINT64_MAX__-1, __UINT64_MAX__-1 };
  k = vec_mulhud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("mulhud  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulhud:", (vui128_t)k, (vui128_t) e);

  return (rc);
}

int
test_muludm (void)
{
  vui64_t i, j;
  vui64_t k, e;
  int rc = 0;

  printf ("\ntest_muludm Vector Multiply Unsigned doubleword Modulo\n");
  i = (vui64_t){1, 2};
  j = (vui64_t){101, 102};
  e = (vui64_t){101, 204};
  k = vec_muludm(i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("muludm  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muludm:", (vui128_t)k, (vui128_t) e);
  i = (vui64_t){0x8000000000000000, 0x4000000000000000};
  j = (vui64_t){0x2000000000000000, 0x4000000000000000};
  e = (vui64_t){0x0000000000000000, 0x0000000000000000};
  k = vec_muludm(i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("muludm  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muludm:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){ __UINT64_MAX__, __INT64_MAX__ };
  j = (vui64_t){ __UINT64_MAX__, __UINT64_MAX__ };
  e = (vui64_t){ 1, -(__INT64_MAX__)};
  k = vec_muludm(i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("muludm  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muludm:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){ __UINT64_MAX__, __INT64_MAX__ };
  j = (vui64_t){ __INT64_MAX__, __INT64_MAX__ };
  e = (vui64_t){ -(__INT64_MAX__), 1 };
  k = vec_muludm(i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("muludm  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muludm:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){1, 20};
  j = (vui64_t){4000, 300};
  e = (vui64_t){4000, 6000};
  k = vec_muludm(i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("muludm  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muludm:", (vui128_t)k, (vui128_t) e);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_vmuleud (void)
{
  vui64_t i, j;
  vui128_t k, e;
  int rc = 0;

  printf ("\ntest_vmuleud Vector Multiply Even Unsigned doublewords\n");
  i = (vui64_t) CONST_VINT64_DW (1, 2);
  j = (vui64_t) CONST_VINT64_DW (101, 102);
  e = (vui128_t) CONST_VINT128_DW128(0, 101);
  k = vec_vmuleud(i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuleud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128  ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (1000000000, 1000000000 );
  j = (vui64_t) CONST_VINT64_DW (1000000000, 1000000000 );
  e = (vui128_t) CONST_VINT128_DW128 (0, 1000000000000000000UL);
  k = vec_vmuleud(i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuleud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (10000000000, 1000000000 );
  j = (vui64_t) CONST_VINT64_DW (10000000000, 1000000000 );
  e = (vui128_t) CONST_VINT128_DW128 (0x5, 0x6bc75e2d63100000UL);
  k = vec_vmuleud(i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuleud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 1000000000000000000UL, 0 );
  j = (vui64_t) CONST_VINT64_DW ( 1000000000000000000UL, 0 );
  e = (vui128_t) CONST_VINT128_DW128 ( 0x00c097ce7bc90715UL, 0xb34b9f1000000000UL );
  k = vec_vmuleud(i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuleud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 0, 1000000000000000000UL );
  j = (vui64_t) CONST_VINT64_DW ( 0, 1000000000000000000UL );
  e = (vui128_t) CONST_VINT128_DW128 ( 0x0UL, 0x0UL );
  k = vec_vmuleud(i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuleud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 100000000000UL, 100000000000UL );
  j = (vui64_t) CONST_VINT64_DW ( 100000000000UL, 100000000000UL );
  e = (vui128_t) CONST_VINT128_DW128 ( 0x021eUL, 0x19e0c9bab2400000UL );
  k = vec_vmuleud(i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuleud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 0x00000000ffffffffUL, 0x00000000ffffffffUL );
  j = (vui64_t) CONST_VINT64_DW ( 0x00000000ffffffffUL, 0x00000000ffffffffUL );
  e = (vui128_t) CONST_VINT128_DW128 ( 0x0UL, 0xfffffffe00000001UL );
  k = vec_muleud(i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuleud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 0xffffffff00000000UL, 0xffffffff00000000UL );
  j = (vui64_t) CONST_VINT64_DW ( 0xffffffff00000000UL, 0xffffffff00000000UL );
  e = (vui128_t) CONST_VINT128_DW128 ( 0xfffffffe00000001UL, 0x0UL );
  k = vec_vmuleud(i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuleud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 0xffffffffffffffffUL, 0x0000000000000000UL );
  j = (vui64_t) CONST_VINT64_DW ( 0xffffffffffffffffUL, 0x0000000000000000UL );
  e = (vui128_t) CONST_VINT128_DW128 (0xfffffffffffffffeUL, 0x0000000000000001UL );
  k = vec_vmuleud(i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuleud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 2000000000000000000UL, 1000000000000000000UL );
  j = (vui64_t) CONST_VINT64_DW ( 1UL, -1UL );
  e = (vui128_t) CONST_VINT128_DW128 ( 0UL, 2000000000000000000UL );
  k = vec_vmuleud(i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuleud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuleud:", (vui128_t)k, (vui128_t) e);

  return (rc);
}

int
test_vmuloud (void)
{
  vui64_t i, j;
  vui128_t k, e;
  int rc = 0;

  printf ("\ntest_vmuloud Vector Multiply Odd Unsigned doublewords\n");
  i = (vui64_t) CONST_VINT64_DW ( 1, 2 );
  j = (vui64_t) CONST_VINT64_DW ( 101, 102 );
  e = (vui128_t) CONST_VINT128_DW128 (0, 204);
  k = vec_vmuloud(i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuloud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 1000000000, 1000000000 );
  j = (vui64_t) CONST_VINT64_DW ( 1000000000, 1000000000 );
  e = (vui128_t) CONST_VINT128_DW128 ( 0, 1000000000000000000UL );
  k = vec_vmuloud(i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuloud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 10000000000, 1000000000 );
  j = (vui64_t) CONST_VINT64_DW ( 10000000000, 1000000000 );
  e = (vui128_t) CONST_VINT128_DW128 ( 0x0, 0x0de0b6b3a7640000UL );
  k = vec_vmuloud(i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuloud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 1000000000000000000UL, 0 );
  j = (vui64_t) CONST_VINT64_DW ( 1000000000000000000UL, 0 );
  e = (vui128_t) CONST_VINT128_DW128 ( 0, 0 );
  k = vec_vmuloud(i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuloud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 0, 1000000000000000000UL );
  j = (vui64_t) CONST_VINT64_DW ( 0, 1000000000000000000UL );
  e = (vui128_t) CONST_VINT128_DW128 ( 0x00c097ce7bc90715UL, 0xb34b9f1000000000UL );
  k = vec_vmuloud(i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuloud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 100000000000UL, 100000000000UL );
  j = (vui64_t) CONST_VINT64_DW ( 100000000000UL, 100000000000UL );
  e = (vui128_t) CONST_VINT128_DW128 ( 0x021eUL, 0x19e0c9bab2400000UL );
  k = vec_vmuloud(i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuloud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 0x00000000ffffffffUL, 0x00000000ffffffffUL );
  j = (vui64_t) CONST_VINT64_DW ( 0x00000000ffffffffUL, 0x00000000ffffffffUL );
  e = (vui128_t) CONST_VINT128_DW128 ( 0x0UL, 0xfffffffe00000001UL );
  k = vec_vmuloud(i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuloud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 0xffffffff00000000UL, 0xffffffff00000000UL );
  j = (vui64_t) CONST_VINT64_DW ( 0xffffffff00000000UL, 0xffffffff00000000UL );
  e = (vui128_t) CONST_VINT128_DW128 ( 0xfffffffe00000001UL, 0x0UL );
  k = vec_vmuloud(i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuloud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 0x0000000000000000UL, 0xffffffffffffffffUL );
  j = (vui64_t) CONST_VINT64_DW ( 0x0000000000000000UL, 0xffffffffffffffffUL );
  e = (vui128_t) CONST_VINT128_DW128 ( 0xfffffffffffffffeUL, 0x0000000000000001UL );
  k = vec_vmuloud(i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuloud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 2000000000000000000UL, 1000000000000000000UL );
  j = (vui64_t) CONST_VINT64_DW ( 1UL, -1L );
  e = (vui128_t) CONST_VINT128_DW128 ( 0x0de0b6b3a763ffffUL, -1000000000000000000L );
  k = vec_vmuloud(i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuloud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuloud:", (vui128_t)k, (vui128_t) e);

  return (rc);
}
#undef __DEBUG_PRINT__

//#define __DEBUG_PRINT__ 1
int
test_maxud (void)
{
  vui64_t i1, i2, j, e;
  int rc = 0;

  printf ("\ntest_maxud Vector Maximum Unsigned doubleword\n");

  i1 = (vui64_t) { 0, 0 };
  i2 = (vui64_t) { 0, 0 };
  e =  (vui64_t) { 0, 0 };
  j = vec_maxud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vmaxud( ", i1);
  print_v2xint64 ("       ,", i2);
  print_v2xint64 ("      )=", j);
#endif
  rc += check_vuint128x ("vec_maxud:", (vui128_t)j, (vui128_t) e);

  i1 = (vui64_t) { 1, 2 };
  i2 = (vui64_t) { 0, 1 };
  e =  (vui64_t) { 1, 2 };
  j = vec_maxud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vmaxud( ", i1);
  print_v2xint64 ("       ,", i2);
  print_v2xint64 ("      )=", j);
#endif
  rc += check_vuint128x ("vec_maxud:", (vui128_t)j, (vui128_t) e);

  i1 = (vui64_t) { 0, 1 };
  i2 = (vui64_t) { 1, 2 };
  e =  (vui64_t) { 1, 2 };
  j = vec_maxud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vmaxud( ", i1);
  print_v2xint64 ("       ,", i2);
  print_v2xint64 ("      )=", j);
#endif
  rc += check_vuint128x ("vec_maxud:", (vui128_t)j, (vui128_t) e);

  i1 = (vui64_t) { __UINT64_MAX__, __INT64_MAX__ };
  i2 = (vui64_t) { __INT64_MAX__, __UINT64_MAX__ };
  e =  (vui64_t) { __UINT64_MAX__, __UINT64_MAX__ };
  j = vec_maxud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vmaxud( ", i1);
  print_v2xint64 ("       ,", i2);
  print_v2xint64 ("      )=", j);
#endif
  rc += check_vuint128x ("vec_maxud:", (vui128_t)j, (vui128_t) e);

  return (rc);
}

int
test_maxsd (void)
{
  vi64_t i1, i2, j, e;
  int rc = 0;

  printf ("\ntest_maxsd Vector Maximum Unsigned doubleword\n");

  i1 = (vi64_t) { 0, 0 };
  i2 = (vi64_t) { 0, 0 };
  e =  (vi64_t) { 0, 0 };
  j = vec_maxsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vmaxsd( ", (vui64_t) i1);
  print_v2xint64 ("       ,", (vui64_t) i2);
  print_v2xint64 ("      )=", (vui64_t) j);
#endif
  rc += check_vuint128x ("vec_maxsd:", (vui128_t)j, (vui128_t) e);

  i1 = (vi64_t) { 1, 2 };
  i2 = (vi64_t) { 0, 1 };
  e =  (vi64_t) { 1, 2 };
  j = vec_maxsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vmaxsd( ", (vui64_t) i1);
  print_v2xint64 ("       ,", (vui64_t) i2);
  print_v2xint64 ("      )=", (vui64_t) j);
#endif
  rc += check_vuint128x ("vec_maxsd:", (vui128_t)j, (vui128_t) e);

  i1 = (vi64_t) { -1, -2 };
  i2 = (vi64_t) { 0, 1 };
  e =  (vi64_t) { 0, 1 };
  j = vec_maxsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vmaxsd( ", (vui64_t) i1);
  print_v2xint64 ("       ,", (vui64_t) i2);
  print_v2xint64 ("      )=", (vui64_t) j);
#endif
  rc += check_vuint128x ("vec_maxsd:", (vui128_t)j, (vui128_t) e);

  i1 = (vi64_t) { 0, 1 };
  i2 = (vi64_t) { 1, 2 };
  e =  (vi64_t) { 1, 2 };
  j = vec_maxsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vmaxsd( ", (vui64_t) i1);
  print_v2xint64 ("       ,", (vui64_t) i2);
  print_v2xint64 ("      )=", (vui64_t) j);
#endif
  rc += check_vuint128x ("vec_maxsd:", (vui128_t)j, (vui128_t) e);

  i1 = (vi64_t) { 0, 1 };
  i2 = (vi64_t) { -1, -2 };
  e =  (vi64_t) { 0, 1 };
  j = vec_maxsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vmaxsd( ", (vui64_t) i1);
  print_v2xint64 ("       ,", (vui64_t) i2);
  print_v2xint64 ("      )=", (vui64_t) j);
#endif
  rc += check_vuint128x ("vec_maxsd:", (vui128_t)j, (vui128_t) e);

  i1 = (vi64_t) { __UINT64_MAX__, __INT64_MAX__ };
  i2 = (vi64_t) { __INT64_MAX__, __UINT64_MAX__ };
  e =  (vi64_t) { __INT64_MAX__, __INT64_MAX__ };
  j = vec_maxsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vmaxsd( ", (vui64_t) i1);
  print_v2xint64 ("       ,", (vui64_t) i2);
  print_v2xint64 ("      )=", (vui64_t) j);
#endif
  rc += check_vuint128x ("vec_maxsd:", (vui128_t)j, (vui128_t) e);

  return (rc);
}

int
test_minud (void)
{
  vui64_t i1, i2, j, e;
  int rc = 0;

  printf ("\ntest_minud Vector Minimum Unsigned doubleword\n");

  i1 = (vui64_t) { 0, 0 };
  i2 = (vui64_t) { 0, 0 };
  e =  (vui64_t) { 0, 0 };
  j = vec_minud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vminud( ", i1);
  print_v2xint64 ("       ,", i2);
  print_v2xint64 ("      )=", j);
#endif
  rc += check_vuint128x ("vec_minud:", (vui128_t)j, (vui128_t) e);

  i1 = (vui64_t) { 1, 2 };
  i2 = (vui64_t) { 0, 1 };
  e =  (vui64_t) { 0, 1 };
  j = vec_minud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vminud( ", i1);
  print_v2xint64 ("       ,", i2);
  print_v2xint64 ("      )=", j);
#endif
  rc += check_vuint128x ("vec_minud:", (vui128_t)j, (vui128_t) e);

  i1 = (vui64_t) { 0, 1 };
  i2 = (vui64_t) { 1, 2 };
  e =  (vui64_t) { 0, 1 };
  j = vec_minud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vminud( ", i1);
  print_v2xint64 ("       ,", i2);
  print_v2xint64 ("      )=", j);
#endif
  rc += check_vuint128x ("vec_minud:", (vui128_t)j, (vui128_t) e);

  i1 = (vui64_t) { __UINT64_MAX__, __INT64_MAX__ };
  i2 = (vui64_t) { __INT64_MAX__, __UINT64_MAX__ };
  e =  (vui64_t) { __INT64_MAX__, __INT64_MAX__ };
  j = vec_minud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vminud( ", i1);
  print_v2xint64 ("       ,", i2);
  print_v2xint64 ("      )=", j);
#endif
  rc += check_vuint128x ("vec_minud:", (vui128_t)j, (vui128_t) e);

  return (rc);
}

int
test_minsd (void)
{
  vi64_t i1, i2, j, e;
  int rc = 0;

  printf ("\ntest_minsd Vector Minimum Unsigned doubleword\n");

  i1 = (vi64_t) { 0, 0 };
  i2 = (vi64_t) { 0, 0 };
  e =  (vi64_t) { 0, 0 };
  j = vec_minsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vminsd( ", (vui64_t) i1);
  print_v2xint64 ("       ,", (vui64_t) i2);
  print_v2xint64 ("      )=", (vui64_t) j);
#endif
  rc += check_vuint128x ("vec_minsd:", (vui128_t)j, (vui128_t) e);

  i1 = (vi64_t) { 1, 2 };
  i2 = (vi64_t) { 0, 1 };
  e =  (vi64_t) { 0, 1 };
  j = vec_minsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vminsd( ", (vui64_t) i1);
  print_v2xint64 ("       ,", (vui64_t) i2);
  print_v2xint64 ("      )=", (vui64_t) j);
#endif
  rc += check_vuint128x ("vec_minsd:", (vui128_t)j, (vui128_t) e);

  i1 = (vi64_t) { -1, -2 };
  i2 = (vi64_t) { 0, 1 };
  e =  (vi64_t) { -1, -2 };
  j = vec_minsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vminsd( ", (vui64_t) i1);
  print_v2xint64 ("       ,", (vui64_t) i2);
  print_v2xint64 ("      )=", (vui64_t) j);
#endif
  rc += check_vuint128x ("vec_minsd:", (vui128_t)j, (vui128_t) e);

  i1 = (vi64_t) { 0, 1 };
  i2 = (vi64_t) { 1, 2 };
  e =  (vi64_t) { 0, 1 };
  j = vec_minsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vminsd( ", (vui64_t) i1);
  print_v2xint64 ("       ,", (vui64_t) i2);
  print_v2xint64 ("      )=", (vui64_t) j);
#endif
  rc += check_vuint128x ("vec_minsd:", (vui128_t)j, (vui128_t) e);

  i1 = (vi64_t) { 0, 1 };
  i2 = (vi64_t) { -1, -2 };
  e =  (vi64_t) { -1, -2 };
  j = vec_minsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vminsd( ", (vui64_t) i1);
  print_v2xint64 ("       ,", (vui64_t) i2);
  print_v2xint64 ("      )=", (vui64_t) j);
#endif
  rc += check_vuint128x ("vec_minsd:", (vui128_t)j, (vui128_t) e);

  i1 = (vi64_t) { __UINT64_MAX__, __INT64_MAX__ };
  i2 = (vi64_t) { __INT64_MAX__, __UINT64_MAX__ };
  e =  (vi64_t) { __UINT64_MAX__, __UINT64_MAX__ };
  j = vec_minsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vminsd( ", (vui64_t) i1);
  print_v2xint64 ("       ,", (vui64_t) i2);
  print_v2xint64 ("      )=", (vui64_t) j);
#endif
  rc += check_vuint128x ("vec_minsd:", (vui128_t)j, (vui128_t) e);

  return (rc);
}
#undef __DEBUG_PRINT__

//#define __DEBUG_PRINT__ 1
int
test_cmpud (void)
{
  vui64_t i1, i2, e;
  vb64_t j;
  int rc = 0;

  printf ("\ntest_cmpud Vector Compare Unsigned doubleword\n");

  printf ("test_cmpequd\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpequd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpequd:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x200000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x200000000);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpequd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpequd:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (2, 2);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpequd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpequd:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 1);
  i2 = (vui64_t) CONST_VINT64_DW (2, 2);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpequd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpequd:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (2, 0x200000000);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpequd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpequd:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpequd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpequd:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpequd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpequd:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 0xffffffff00000000);
  i2 = (vui64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpequd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpequd:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpequd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpequd:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0xffffffff00000000, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpequd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpequd:", (vb64_t)j, (vb64_t) e);

  printf ("test_cmpneud\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpneud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpneud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x200000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x200000000);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpneud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpneud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (2, 2);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpneud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpneud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 1);
  i2 = (vui64_t) CONST_VINT64_DW (2, 2);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpneud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpneud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (2, 0x200000000);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpneud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpneud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpneud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpneud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpneud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpneud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 0xffffffff00000000);
  i2 = (vui64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpneud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpneud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpneud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpneud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0xffffffff00000000, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpneud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpneud:", (vb64_t)j, (vb64_t) e);

  printf ("test_cmpgtud\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpgtud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgtud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgtud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgtud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgtud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgtud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpgtud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgtud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x100000000);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpgtud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgtud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 2);
  i2 = (vui64_t) CONST_VINT64_DW (2, 0x100000000);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpgtud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgtud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgtud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgtud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgtud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgtud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 2);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgtud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgtud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpgtud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgtud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (2, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000000);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpgtud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgtud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000001);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpgtud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgtud:", (vb64_t)j, (vb64_t) e);

  printf ("test_cmpgeud\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpgeud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgeud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgeud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgeud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgeud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgeud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgeud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgeud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgeud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpgeud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgeud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgeud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x100000000);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpgeud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgeud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgeud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 2);
  i2 = (vui64_t) CONST_VINT64_DW (2, 0x100000000);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpgeud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgeud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgeud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpgeud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgeud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgeud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpgeud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgeud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgeud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 2);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpgeud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgeud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgeud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgeud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgeud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgeud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (2, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000000);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgeud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgeud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgeud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000001);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpgeud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgeud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgeud:", (vb64_t)j, (vb64_t) e);

  printf ("test_cmpltud\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpltud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpltud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpltud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpltud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpltud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpltud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpltud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpltud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x100000000);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpltud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpltud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 2);
  i2 = (vui64_t) CONST_VINT64_DW (2, 0x100000000);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpltud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpltud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpltud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpltud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpltud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpltud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 2);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpltud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpltud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpltud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpltud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (2, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000000);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpltud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpltud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000001);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpltud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpltud:", (vb64_t)j, (vb64_t) e);

  printf ("test_cmpleud\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpleud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpleud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpleud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpleud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpleud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpleud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpleud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpleud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpleud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpleud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpleud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpleud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x100000000);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpleud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpleud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpleud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 2);
  i2 = (vui64_t) CONST_VINT64_DW (2, 0x100000000);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpleud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpleud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpleud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpleud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpleud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpleud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpleud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpleud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpleud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 2);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpleud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpleud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpleud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpleud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpleud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpleud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (2, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000000);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpleud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpleud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpleud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000001);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpleud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpleud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpleud:", (vb64_t)j, (vb64_t) e);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_cmpud_all (void)
{
  vui64_t i1, i2;
#ifdef __DEBUG_PRINT__
  vb64_t j;
#endif
  int rc = 0;

  printf ("\ntest_cmpud_all Vector Compare Unsigned doubleword\n");

  printf ("test_cmp_all_eq\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_all_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_all_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_all_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_all_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpud_all_eq (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpequd(i1, i2);
    print_v2xint64 ("vec_cmpud_all_eq( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpud_all_eq (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpequd(i1, i2);
    print_v2xint64 ("vec_cmpud_all_eq( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_all_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpequd(i1, i2);
    print_v2xint64 ("vec_cmpud_all_eq( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_all_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_all_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_all_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_all_eq (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpequd(i1, i2);
    print_v2xint64 ("vec_cmpud_all_eq( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_all_eq (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpequd(i1, i2);
    print_v2xint64 ("vec_cmpud_all_eq( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_all_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpequd(i1, i2);
    print_v2xint64 ("vec_cmpud_all_eq( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpud_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_all_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpud_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_all_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  printf ("test_cmp_all_ne\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpud_all_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpneud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_ne( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpud_all_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpneud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_ne( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_all_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpneud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_ne( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_all_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpneud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_ne( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_all_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpneud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_ne( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_all_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpneud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_ne( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpud_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpud_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  printf ("test_cmp_all_gt\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpud_all_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_gt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpud_all_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_gt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_all_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_gt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_all_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_gt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_all_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_gt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_all_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_gt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpud_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpud_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_all_gt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_gt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  printf ("test_cmp_all_ge\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpud_all_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgeud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_ge( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpud_all_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgeud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_ge( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_all_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgeud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_ge( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_all_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgeud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_ge( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_all_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgeud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_ge( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_all_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgeud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_ge( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpud_all_ge (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpud_all_ge (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_all_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgeud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_ge( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  printf ("test_cmp_all_lt\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_all_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpud_all_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpud_all_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_all_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_all_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_all_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_all_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_all_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpud_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpud_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);

  if (vec_cmpud_all_lt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_all_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  printf ("test_cmp_all_le\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpud_all_le (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpud_all_le (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_all_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_all_le (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_all_le (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_all_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpud_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpud_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);

  if (vec_cmpud_all_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_all_le (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_cmpud_any (void)
{
  vui64_t i1, i2;
#ifdef __DEBUG_PRINT__
  vb64_t j;
#endif
  int rc = 0;

  printf ("\ntest_cmpud_any Vector Compare Unsigned doubleword\n");

  printf ("test_cmp_any_eq\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_any_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_any_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_any_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_any_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpud_any_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpequd(i1, i2);
    print_v2xint64 ("vec_cmpud_any_eq( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpud_any_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpequd(i1, i2);
    print_v2xint64 ("vec_cmpud_any_eq( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_any_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpequd(i1, i2);
    print_v2xint64 ("vec_cmpud_any_eq( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_any_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_any_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_any_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_any_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpequd(i1, i2);
    print_v2xint64 ("vec_cmpud_any_eq( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_any_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpequd(i1, i2);
    print_v2xint64 ("vec_cmpud_any_eq( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_any_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpequd(i1, i2);
    print_v2xint64 ("vec_cmpud_any_eq( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpud_any_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_any_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpud_any_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_any_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  printf ("test_cmp_any_ne\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpud_any_ne (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpneud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_ne( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpud_any_ne (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpneud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_ne( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_any_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpneud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_ne( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_any_ne (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpneud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_ne( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_any_ne (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpneud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_ne( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_any_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpneud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_ne( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpud_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpud_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  printf ("test_cmp_any_gt\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpud_any_gt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_gt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpud_any_gt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_gt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_any_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_gt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_any_gt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_gt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_any_gt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_gt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_any_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_gt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpud_any_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpud_any_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_any_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_any_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_any_gt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_gt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  printf ("test_cmp_any_ge\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpud_any_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgeud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_ge( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpud_any_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgeud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_ge( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_any_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgeud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_ge( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_any_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgeud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_ge( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_any_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgeud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_ge( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_any_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgeud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_ge( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpud_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpud_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_any_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgeud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_ge( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  printf ("test_cmp_any_lt\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_any_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpud_any_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpud_any_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_any_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_any_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_any_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_any_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_any_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpud_any_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpud_any_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);

  if (vec_cmpud_any_lt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_any_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  printf ("test_cmp_any_le\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpud_any_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpud_any_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_any_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_any_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_any_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_any_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpud_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpud_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);

  if (vec_cmpud_any_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_any_le (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  return (rc);
}
//#undef __DEBUG_PRINT__
//#define __DEBUG_PRINT__ 1
int
test_cmpsd (void)
{
  vi64_t i1, i2, e;
  vb64_t j;
  int rc = 0;

  printf ("\ntest_cmpsd Vector Compare Signed doubleword\n");

  printf ("test_cmpeqsd\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpeqsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpeqsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpeqsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpeqsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpeqsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpeqsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpeqsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpeqsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, -1);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpeqsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpeqsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpeqsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpeqsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1,-1);
  i2 = (vi64_t) CONST_VINT64_DW (0, -1);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpeqsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpeqsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpeqsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpeqsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1,-1);
  i2 = (vi64_t) CONST_VINT64_DW (-1, -1);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpeqsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpeqsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0xffffffff00000000);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpeqsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpeqsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff00000001, 0xffffffff00000000);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpeqsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpeqsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff00000001, 0xffffffffffffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpeqsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpeqsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffffffffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff7fffffff);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpeqsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpeqsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff7fffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffffffffffff);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpeqsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpeqsd:", (vb64_t)j, (vb64_t) e);

  printf ("test_cmpgtsd\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, -1);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1,-1);
  i2 = (vi64_t) CONST_VINT64_DW (0, -1);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vi64_t) CONST_VINT64_DW (-1, -1);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vi64_t) CONST_VINT64_DW (-1, -2);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vi64_t) CONST_VINT64_DW (-2, -1);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0xffffffff00000000);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff00000001, 0xffffffff00000000);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff00000001, 0xffffffffffffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffffffffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff7fffffff);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff7fffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffffffffffff);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  printf ("test_cmpgesd\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, -1);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1,-1);
  i2 = (vi64_t) CONST_VINT64_DW (0, -1);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vi64_t) CONST_VINT64_DW (-1, -1);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vi64_t) CONST_VINT64_DW (-1, -2);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vi64_t) CONST_VINT64_DW (-2, -1);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0xffffffff00000000);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff00000001, 0xffffffff00000000);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff00000001, 0xffffffffffffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffffffffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff7fffffff);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff7fffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffffffffffff);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  printf ("test_cmpltsd\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, -1);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1,-1);
  i2 = (vi64_t) CONST_VINT64_DW (0, -1);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vi64_t) CONST_VINT64_DW (-1, -1);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vi64_t) CONST_VINT64_DW (-1, -2);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vi64_t) CONST_VINT64_DW (-2, -1);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0xffffffff00000000);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff00000001, 0xffffffff00000000);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff00000001, 0xffffffffffffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffffffffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff7fffffff);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff7fffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffffffffffff);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  printf ("test_cmplesd\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, -1);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1,-1);
  i2 = (vi64_t) CONST_VINT64_DW (0, -1);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vi64_t) CONST_VINT64_DW (-1, -1);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vi64_t) CONST_VINT64_DW (-1, -2);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vi64_t) CONST_VINT64_DW (-2, -1);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0xffffffff00000000);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff00000001, 0xffffffff00000000);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff00000001, 0xffffffffffffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffffffffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff7fffffff);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff7fffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffffffffffff);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  printf ("test_cmpnesd\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpnesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpnesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpnesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpnesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpnesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpnesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpnesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpnesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, -1);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpnesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpnesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpnesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpnesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1,-1);
  i2 = (vi64_t) CONST_VINT64_DW (0, -1);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpnesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpnesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpnesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpnesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vi64_t) CONST_VINT64_DW (-1, -1);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpnesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpnesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0xffffffff00000000);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpnesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpnesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff00000001, 0xffffffff00000000);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpnesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpnesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff00000001, 0xffffffffffffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpnesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpnesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffffffffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff7fffffff);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpnesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpnesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff7fffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffffffffffff);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpnesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpnesd:", (vb64_t)j, (vb64_t) e);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_cmpsd_all (void)
{
  vi64_t i1, i2;
#ifdef __DEBUG_PRINT__
  vb64_t j;
#endif
  int rc = 0;

  printf ("\ntest_cmpsd_all Vector Compare Unsigned doubleword\n");

  printf ("test_cmp_all_eq\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpsd_all_eq (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpeqsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpsd_all_eq (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpeqsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_all_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpeqsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_all_eq (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpeqsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_all_eq (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpeqsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_all_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpeqsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpsd_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpsd_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  printf ("test_cmp_all_ne\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpsd_all_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpnesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpsd_all_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpnesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_all_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpnesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_all_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpnesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_all_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpnesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_all_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpnesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpsd_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpsd_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  printf ("test_cmp_all_gt\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpsd_all_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpsd_all_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_all_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_all_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_all_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_all_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpsd_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpsd_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_all_gt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  printf ("test_cmp_all_ge\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpsd_all_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpsd_all_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_all_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_ge (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_ge (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_ge (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_all_ge (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_all_ge (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_all_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpsd_all_ge (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpsd_all_ge (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_all_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  printf ("test_cmp_all_lt\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_all_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpsd_all_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpsd_all_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_all_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_all_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_all_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_all_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpsd_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpsd_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);

  if (vec_cmpsd_all_lt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_all_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  printf ("test_cmp_all_le\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpsd_all_le (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpsd_all_le (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_all_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_all_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_all_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_all_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpsd_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpsd_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);

  if (vec_cmpsd_all_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_all_le (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_cmpsd_any (void)
{
  vi64_t i1, i2;
#ifdef __DEBUG_PRINT__
  vb64_t j;
#endif
  int rc = 0;

  printf ("\ntest_cmpsd_any Vector Compare Unsigned doubleword\n");

  printf ("test_cmp_any_eq\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpsd_any_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpeqsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpsd_any_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpeqsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_any_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpeqsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_any_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpeqsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_any_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpeqsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_any_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpeqsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpsd_any_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpsd_any_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  printf ("test_cmp_any_ne\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpsd_any_ne (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpnesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpsd_any_ne (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpnesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_any_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpnesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_any_ne (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpnesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_any_ne (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpnesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_any_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpnesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpsd_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpsd_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  printf ("test_cmp_any_gt\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpsd_any_gt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpsd_any_gt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_any_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_any_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_any_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_any_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpsd_any_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpsd_any_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_any_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_any_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_any_gt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  printf ("test_cmp_any_ge\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpsd_any_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpsd_any_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_any_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_ge (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_any_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_any_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_any_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpsd_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpsd_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_any_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  printf ("test_cmp_any_lt\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_any_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpsd_any_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpsd_any_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_any_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_any_lt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_any_lt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_any_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpsd_any_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpsd_any_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);

  if (vec_cmpsd_any_lt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_any_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  printf ("test_cmp_any_le\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpsd_any_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpsd_any_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_any_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_any_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_any_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_any_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_any_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpsd_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpsd_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);

  if (vec_cmpsd_any_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_any_le (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  return (rc);
}

int
test_vec_i64 (void)
{
  int rc = 0;

  printf ("\n%s\n", __FUNCTION__);

  rc += test_permdi ();
  rc += test_xxspltd ();
  rc += test_mrgahld ();
  rc += test_mrghld ();
  rc += test_mrgeod ();
  rc += test_splatd ();
  rc += test_revbd ();
  rc += test_clzd ();
  rc += test_popcntd ();
  rc += test_vrld ();
  rc += test_vsld ();
  rc += test_vsrd ();
  rc += test_vsrad ();
  rc += test_muleud ();
  rc += test_muloud ();
  rc += test_mulhud ();
  rc += test_muludm ();
  rc += test_cmpud ();
  rc += test_cmpud_all ();
  rc += test_cmpud_any ();
  rc += test_cmpsd ();
  rc += test_cmpsd_all ();
  rc += test_cmpsd_any ();
  rc += test_maxud ();
  rc += test_maxsd ();
  rc += test_minud ();
  rc += test_minsd ();
  rc += test_vmuleud ();
  rc += test_vmuloud ();

  return (rc);
}
