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

 arith128_test_i32.c

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
#include <pveclib/vec_common_ppc.h>
#include <pveclib/vec_int32_ppc.h>

#include <testsuite/arith128_test_i32.h>

int
test_revbw (void)
{
  vui32_t i, e, *ip;
  vui128_t k;
  unsigned char mem[16] __attribute__ ((aligned (16))) = { 0xf0, 0xf1, 0xf2,
      0xf3, 0xe0, 0xe1, 0xe2, 0xe3, 0xd0, 0xd1, 0xd2, 0xd3, 0xc0, 0xc1, 0xc2,
      0xc3 };
  int rc = 0;

  printf ("\ntest_revbw Reverse Bytes in words\n");

  i = (vui32_t)CONST_VINT32_W(0, 1, 2, 3);
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x01000000, 0x02000000, 0x03000000);
  k = (vui128_t) vec_revbw ((vui32_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_revbw i=", (vui128_t)i);
  print_vint128x ("         k=", k);
#endif
  rc += check_vuint128x ("vec_revbw 1:", k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0x01020304, 0x11121314, 0x21222324, 0x31323334);
  e = (vui32_t)CONST_VINT32_W(0x04030201, 0x14131211, 0x24232221, 0x34333231);
  k = (vui128_t) vec_revbw ((vui32_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_revbw i=", (vui128_t)i);
  print_vint128x ("         k=", k);
#endif
  rc += check_vuint128x ("vec_revbw 2:", k, (vui128_t) e);

  ip = (vui32_t*) mem;
  i = *ip;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  e = (vui32_t)CONST_VINT32_W(0xf3f2f1f0, 0xe3e2e1e0, 0xd3d2d1d0, 0xc3c2c1c0);
#else
  e = (vui32_t)CONST_VINT32_W(0xc0c1c2c3, 0xd0d1d2d3, 0xe0e1e2e3, 0xf0f1f2f3);
#endif
  k = (vui128_t) vec_revbw ((vui32_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_revbw i=", (vui128_t)i);
  print_vint128x ("         k=", k);
#endif
  rc += check_vuint128x ("vec_revbw 3:", k, (vui128_t) e);

  return (rc);
}

int
test_popcntw (void)
{
  vui32_t i, e, j;
  int rc = 0;

  printf ("\ntest_popcntw Vector Pop Count word\n");

  i = (vui32_t)CONST_VINT32_W(0, 1, 2, 4);
  e = (vui32_t)CONST_VINT32_W(0, 1, 1, 1);
  j = vec_popcntw(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntw({0, 1, 2, 4}) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntw:", (vui128_t)j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(8, 16, 32, 64);
  e = (vui32_t)CONST_VINT32_W(1, 1, 1, 1);
  j = vec_popcntw(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntw({8, 16, 32, 64}) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntw:", (vui128_t)j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(128, 256, 512, 1024);
  e = (vui32_t)CONST_VINT32_W(1, 1, 1, 1);
  j = vec_popcntw(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntw({128, 256, 512, 1024}) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntw:", (vui128_t)j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(2048, 4096, 8192, 16384);
  e = (vui32_t)CONST_VINT32_W(1, 1, 1, 1);
  j = vec_popcntw(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntw({2048, 4096, 8192, 16384}) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntw:", (vui128_t)j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(32768, 65536, 131072, 262144);
  e = (vui32_t)CONST_VINT32_W(1, 1, 1, 1);
  j = vec_popcntw(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntw({32768, 65536, 131072, 262144}) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntw:", (vui128_t)j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(134217728, 268435456, 536870912, 1073741824);
  e = (vui32_t)CONST_VINT32_W(1, 1, 1, 1);
  j = vec_popcntw(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntw({134217728, 268435456, 536870912, 1073741824}) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntw:", (vui128_t)j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(-2147483648, -268435456, -16777216, -1048576);
  e = (vui32_t)CONST_VINT32_W(1, 4, 8, 12);
  j = vec_popcntw(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntw({-2147483648, -268435456, -16777216, -1048576}) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntw:", (vui128_t)j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(-2132741936, 521078531, -219024136, 1311448462);
  e = (vui32_t)CONST_VINT32_W(11, 14, 20, 16);
  j = vec_popcntw(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntw({-2132741936, 521078531, -219024136, 1311448462}) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntw:", (vui128_t)j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(-2143281136, 270549120, 540037232, 1081127120);
  e = (vui32_t)CONST_VINT32_W(4, 4, 8, 10);
  j = vec_popcntw(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntw({-2143281136, 270549120, 540037232, 1081127120}) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntw:", (vui128_t)j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(-1177168454, -1789281621, -1700158118, -1044208472);
  e = (vui32_t)CONST_VINT32_W(20, 18, 16, 12);
  j = vec_popcntw(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntw({-1177168454, -1789281621, -1700158118, -1044208472}) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntw:", (vui128_t)j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(-1143095845, -1957965135, -2008800751, 134480385);
  e = (vui32_t)CONST_VINT32_W(24, 16, 8, 4);
  j = vec_popcntw(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntw({-1143095845, -1957965135, -2008800751, 134480385}) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntw:", (vui128_t)j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(-1, -1, -1, -1);
  e = (vui32_t)CONST_VINT32_W(32, 32, 32, 32);
  j = vec_popcntw(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntw({-1, -1, -1, -1}) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntw:", (vui128_t)j, (vui128_t) e);

  return (rc);
}

int
test_clzw (void)
{
  vui32_t i, e, j;
  int rc = 0;

  printf ("\ntest_clzw Vector Count Leading Zeros in words\n");

  i = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  e = (vui32_t)CONST_VINT32_W(32, 32, 32, 32);
  j = vec_clzw(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0) ", j);
#endif
  rc += check_vuint128x ("vec_clzw:", (vui128_t)j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(-1, 0, -1, 0);
  e = (vui32_t)CONST_VINT32_W(0, 32, 0, 32);
  j = vec_clzw(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(-1, 0, -1, 0) ", j);
#endif
  rc += check_vuint128x ("vec_clzw:", (vui128_t)j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0, 1, 2, 4);
  e = (vui32_t)CONST_VINT32_W(32, 31, 30, 29);
  j = vec_clzw(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0, 1, 2, 4) ", j);
#endif
  rc += check_vuint128x ("vec_clzw:", (vui128_t)j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(8, 16, 32, 64);
  e = (vui32_t)CONST_VINT32_W(28, 27, 26, 25);
  j = vec_clzw(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(8, 16, 32, 64) ", j);
#endif
  rc += check_vuint128x ("vec_clzw:", (vui128_t)j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(128, 256, 512, 1024);
  e = (vui32_t)CONST_VINT32_W(24, 23, 22, 21);
  j = vec_clzw(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(128, 256, 512, 1024) ", j);
#endif
  rc += check_vuint128x ("vec_clzw:", (vui128_t)j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(2048, 4096, 8192, 16384);
  e = (vui32_t)CONST_VINT32_W(20, 19, 18, 17);
  j = vec_clzw(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(2048, 4096, 8192, 16384) ", j);
#endif
  rc += check_vuint128x ("vec_clzw:", (vui128_t)j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(32768, 65536, 131072, 262144);
  e = (vui32_t)CONST_VINT32_W(16, 15, 14, 13);
  j = vec_clzw(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(32768, 65536, 131072, 262144) ", j);
#endif
  rc += check_vuint128x ("vec_clzw:", (vui128_t)j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(524288, 1048576, 2097152, 4194304);
  e = (vui32_t)CONST_VINT32_W(12, 11, 10, 9);
  j = vec_clzw(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(524288, 1048576, 2097152, 4194304) ", j);
#endif
  rc += check_vuint128x ("vec_clzw:", (vui128_t)j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(8388608, 16777216, 33554432, 67108864);
  e = (vui32_t)CONST_VINT32_W(8, 7, 6, 5);
  j = vec_clzw(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(8388608, 16777216, 33554432, 67108864) ", j);
#endif
  rc += check_vuint128x ("vec_clzw:", (vui128_t)j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(134217728, 268435456, 536870912, 1073741824);
  e = (vui32_t)CONST_VINT32_W(4, 3, 2, 1);
  j = vec_clzw(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(134217728, 268435456, 536870912, 1073741824) ", j);
#endif
  rc += check_vuint128x ("vec_clzw:", (vui128_t)j, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(-2147483648, -268435456, -16777216, -1048576);
  e = (vui32_t)CONST_VINT32_W(0, 0, 0, 0);
  j = vec_clzw(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(-2147483648, -268435456, -16777216, -1048576) ", j);
#endif
  rc += check_vuint128x ("vec_clzw:", (vui128_t)j, (vui128_t) e);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_mulhuw (void)
{
  vui32_t i, j;
  vui32_t k, e;
  int rc = 0;

  printf ("\ntest_mmulhuw Vector Multiply High unsigned Words\n");

  i = (vui32_t) CONST_VINT32_W (-1, -1, -1, -1);
  j = (vui32_t) CONST_VINT32_W (1, 2, 3, 4);
  e = (vui32_t) CONST_VINT32_W (0, 1, 2, 3);
  k = vec_mulhuw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d ("mulhuw({-1, -1, -1, -1}, {1, 2, 3, 4}) ", k);
#endif
  rc += check_vuint128x ("vec_mulhuw:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t) CONST_VINT32_W (0x10000, 0x10000, 0x10000, 0x10000);
  j = (vui32_t) CONST_VINT32_W (0x10000, 0x20000, 0x30000, 0x40000);
  e = (vui32_t) CONST_VINT32_W (1, 2, 3, 4);
  k = vec_mulhuw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d ("mulhuw({2^16,2^16,2^16,2^16}, {2^16,2^17,3*2^16,2^18}) ", k);
#endif
  rc += check_vuint128x ("vec_mulhuw:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t) CONST_VINT32_W (1000000000, 1000000000, 1000000000, 1000000000);
  j = (vui32_t) CONST_VINT32_W (5, 4295, 4294968, 429496730);
  e = (vui32_t) CONST_VINT32_W (1, 1000, 1000000, 100000000);
  k = vec_mulhuw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d ("mulhuw({10^9,10^9,10^9,10^9}, {10^-9,10^-6,10^-3,10^-1}) ", k);
#endif
  rc += check_vuint128x ("vec_mulhuw:", (vui128_t) k, (vui128_t) e);

  return (rc);
}

int
test_mrgahlw (void)
{
  vui64_t i, j;
  vui32_t k, e;
  int rc = 0;

  printf ("\ntest_mrgahw Vector Merge Algebraic High/Low Words\n");

  i = (vui64_t) (vui32_t) CONST_VINT32_W (100, 1, 200, 2);
  j = (vui64_t) (vui32_t) CONST_VINT32_W (300, 3, 400, 4);
  e = (vui32_t) CONST_VINT32_W (100, 300, 200, 400);
  k = vec_mrgahw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d ("mrgahw({100, 1, 200, 2}, {300, 3, 400, 4}) ", k);
#endif
  rc += check_vuint128x ("vec_mrgahw:", (vui128_t) k, (vui128_t) e);

  e = (vui32_t) CONST_VINT32_W (1, 3, 2, 4);
  k = vec_mrgalw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d ("mrgalw({100, 1, 200, 2}, {300, 3, 400, 4}) ", k);
#endif
  rc += check_vuint128x ("vec_mrgalw:", (vui128_t) k, (vui128_t) e);

  return (rc);
}

int
test_mrgeow (void)
{
  vui32_t i, j;
  vui32_t k, e;
  int rc = 0;

  printf ("\ntest_mrgeow Vector Merge Even/Odd Words\n");

  i = (vui32_t) { 100, 1, 200, 2 };
  j = (vui32_t) { 300, 3, 400, 4 };
  e = (vui32_t) { 100, 300, 200, 400 };
  k = vec_mrgew (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d ("mrgew({100, 1, 200, 2}, {300, 3, 400, 4}) ", k);
#endif
  rc += check_vuint128x ("vec_mrgew:", (vui128_t) k, (vui128_t) e);

  e = (vui32_t) { 1, 3, 2, 4 };
  k = vec_mrgow (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d ("mrgow({100, 1, 200, 2}, {300, 3, 400, 4}) ", k);
#endif
  rc += check_vuint128x ("vec_mrgew:", (vui128_t) k, (vui128_t) e);

  return (rc);
}
//#undef __DEBUG_PRINT__

//#define __DEBUG_PRINT__
int
test_mulesw (void)
{
  vi32_t i, j;
  vi64_t k, e;
  int rc = 0;

  printf ("\ntest_mulesw Vector Multiply Even Signed words\n");

  i = (vi32_t) {1, 2, 3, 4};
  j = (vi32_t) {10, 20, 30, 40};
  e = (vi64_t) {10, 90};
  k = vec_mulesw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d  ("mulesw  ( ", (vui32_t)i);
  print_vint32d  ("         ,", (vui32_t)j);
  print_v2xint64 ("        )=", (vui64_t)k);
#endif
  rc += check_vuint128x ("vec_mulesw:", (vui128_t)k, (vui128_t) e);

  i = (vi32_t) {-1, -2, 3, 4};
  j = (vi32_t) {10, -20, 30, -40};
  e = (vi64_t) {-10, 90};
  k = vec_mulesw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d  ("mulesw  ( ", (vui32_t)i);
  print_vint32d  ("         ,", (vui32_t)j);
  print_v2xint64 ("        )=", (vui64_t)k);
#endif
  rc += check_vuint128x ("vec_mulesw:", (vui128_t)k, (vui128_t) e);

  i = (vi32_t) {-1, -2, 3, 4};
  j = (vi32_t) {-10, -20, -30, -40};
  e = (vi64_t) {10, -90};
  k = vec_mulesw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d  ("mulesw  ( ", (vui32_t)i);
  print_vint32d  ("         ,", (vui32_t)j);
  print_v2xint64 ("        )=", (vui64_t)k);
#endif
  rc += check_vuint128x ("vec_mulesw:", (vui128_t)k, (vui128_t) e);

#ifdef __DEBUG_PRINT__
  print_vint32d  ("mulesw  ( ", (vui32_t)i);
  print_vint32d  ("         ,", (vui32_t)j);
  print_v2xint64 ("        )=", (vui64_t)k);
#endif
  rc += check_vuint128x ("vec_mulesw:", (vui128_t)k, (vui128_t) e);

  i = (vi32_t) {-1, -1, -1, -1};
  j = (vi32_t) {-1, -1, -1, -1};
  e = (vi64_t) {1, 1};
  k = vec_mulesw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d  ("mulesw  ( ", (vui32_t)i);
  print_vint32d  ("         ,", (vui32_t)j);
  print_v2xint64 ("        )=", (vui64_t)k);
#endif
  rc += check_vuint128x ("vec_mulesw:", (vui128_t)k, (vui128_t) e);

  i = (vi32_t) {-1, -1, -1, -1};
  j = (vi32_t) {-1,  1, -1,  1};
  e = (vi64_t) {1, 1};
  k = vec_mulesw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d  ("mulesw  ( ", (vui32_t)i);
  print_vint32d  ("         ,", (vui32_t)j);
  print_v2xint64 ("        )=", (vui64_t)k);
#endif
  rc += check_vuint128x ("vec_mulesw:", (vui128_t)k, (vui128_t) e);

  i = (vi32_t) {-1, -1, -1, -1};
  j = (vi32_t) { 1, -1,  1, -1};
  e = (vi64_t) {-1, -1};
  k = vec_mulesw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d  ("mulesw  ( ", (vui32_t)i);
  print_vint32d  ("         ,", (vui32_t)j);
  print_v2xint64 ("        )=", (vui64_t)k);
#endif
  rc += check_vuint128x ("vec_mulesw:", (vui128_t)k, (vui128_t) e);

  i = (vi32_t) {99999999, 99999999, 99999999, 99999999};
  j = (vi32_t) {99999999, -99999999, -99999999, 99999999};
  e = (vi64_t) {9999999800000001LL, -9999999800000001LL};
  k = vec_mulesw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d  ("mulesw  ( ", (vui32_t)i);
  print_vint32d  ("         ,", (vui32_t)j);
  print_v2xint64 ("        )=", (vui64_t)k);
#endif
  rc += check_vuint128x ("vec_mulesw:", (vui128_t)k, (vui128_t) e);

  i = (vi32_t) {88888888, 88888888, 88888888, 88888888};
  j = (vi32_t) {88888888, -88888888, -88888888, 88888888};
  e = (vi64_t) {7901234409876544LL, -7901234409876544LL};
  k = vec_mulesw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d  ("mulesw  ( ", (vui32_t)i);
  print_vint32d  ("         ,", (vui32_t)j);
  print_v2xint64 ("        )=", (vui64_t)k);
#endif
  rc += check_vuint128x ("vec_mulesw:", (vui128_t)k, (vui128_t) e);

  i = (vi32_t) {99999999, 99999999, 99999999, 99999999};
  j = (vi32_t) {88888888, -88888888, -88888888, 88888888};
  e = (vi64_t) {8888888711111112LL, -8888888711111112LL};
  k = vec_mulesw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d  ("mulesw  ( ", (vui32_t)i);
  print_vint32d  ("         ,", (vui32_t)j);
  print_v2xint64 ("        )=", (vui64_t)k);
#endif
  rc += check_vuint128x ("vec_mulesw:", (vui128_t)k, (vui128_t) e);

  i = (vi32_t) {99999999, 0, 99999999, 0};
  j = (vi32_t) {0, 99999999, 0, 99999999};
  e = (vi64_t) {0UL, 0UL};
  k = vec_mulesw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d  ("mulesw  ( ", (vui32_t)i);
  print_vint32d  ("         ,", (vui32_t)j);
  print_v2xint64 ("        )=", (vui64_t)k);
#endif
  rc += check_vuint128x ("vec_mulesw:", (vui128_t)k, (vui128_t) e);

  i = (vi32_t) {__INT_MAX__, __INT_MAX__, __INT_MAX__, __INT_MAX__};
  j = (vi32_t) {__INT_MAX__, __INT_MAX__, -__INT_MAX__, __INT_MAX__};
  e = (vi64_t) {4611686014132420609LL, -4611686014132420609LL};
  k = vec_mulesw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d  ("mulesw  ( ", (vui32_t)i);
  print_vint32d  ("         ,", (vui32_t)j);
  print_v2xint64 ("        )=", (vui64_t)k);
#endif
  rc += check_vuint128x ("vec_mulesw:", (vui128_t)k, (vui128_t) e);

  i = (vi32_t) { 0x80000000, 0x80000000, 0x80000000, 0x80000000};
  j = (vi32_t) { 0x80000000, 0x80000000, __INT_MAX__, __INT_MAX__};
  e = (vi64_t) { 0x4000000000000000LL, 0xc000000080000000LL};
  k = vec_mulesw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d  ("mulesw  ( ", (vui32_t)i);
  print_vint32d  ("         ,", (vui32_t)j);
  print_v2xint64 ("        )=", (vui64_t)k);
#endif
  rc += check_vuint128x ("vec_mulesw:", (vui128_t)k, (vui128_t) e);

  return (rc);
}
//#undef __DEBUG_PRINT__

//#define __DEBUG_PRINT__
int
test_mulosw (void)
{
  vi32_t i, j;
  vi64_t k, e;
  int rc = 0;

  printf ("\ntest_mulosw Vector Multiply Odd Signed words\n");

  i = (vi32_t) {1, 2, 3, 4};
  j = (vi32_t) {10, 20, 30, 40};
  e = (vi64_t) {40, 160};
  k = vec_mulosw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d  ("mulosw  ( ", (vui32_t)i);
  print_vint32d  ("         ,", (vui32_t)j);
  print_v2xint64 ("        )=", (vui64_t)k);
#endif
  rc += check_vuint128x ("vec_mulosw:", (vui128_t)k, (vui128_t) e);

  i = (vi32_t) {-1, -2, 3, 4};
  j = (vi32_t) {10, -20, 30, -40};
  e = (vi64_t) {40, -160};
  k = vec_mulosw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d  ("mulosw  ( ", (vui32_t)i);
  print_vint32d  ("         ,", (vui32_t)j);
  print_v2xint64 ("        )=", (vui64_t)k);
#endif
  rc += check_vuint128x ("vec_mulosw:", (vui128_t)k, (vui128_t) e);

  i = (vi32_t) {-1, -2, 3, 4};
  j = (vi32_t) {-10, -20, -30, -40};
  e = (vi64_t) {40, -160};
  k = vec_mulosw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d  ("mulosw  ( ", (vui32_t)i);
  print_vint32d  ("         ,", (vui32_t)j);
  print_v2xint64 ("        )=", (vui64_t)k);
#endif
  rc += check_vuint128x ("vec_mulosw:", (vui128_t)k, (vui128_t) e);

#ifdef __DEBUG_PRINT__
  print_vint32d  ("mulosw  ( ", (vui32_t)i);
  print_vint32d  ("         ,", (vui32_t)j);
  print_v2xint64 ("        )=", (vui64_t)k);
#endif
  rc += check_vuint128x ("vec_mulosw:", (vui128_t)k, (vui128_t) e);

  i = (vi32_t) {-1, -1, -1, -1};
  j = (vi32_t) {-1, -1, -1, -1};
  e = (vi64_t) {1, 1};
  k = vec_mulosw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d  ("mulosw  ( ", (vui32_t)i);
  print_vint32d  ("         ,", (vui32_t)j);
  print_v2xint64 ("        )=", (vui64_t)k);
#endif
  rc += check_vuint128x ("vec_mulosw:", (vui128_t)k, (vui128_t) e);

  i = (vi32_t) {-1, -1, -1, -1};
  j = (vi32_t) {-1,  1, -1,  1};
  e = (vi64_t) {-1, -1};
  k = vec_mulosw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d  ("mulosw  ( ", (vui32_t)i);
  print_vint32d  ("         ,", (vui32_t)j);
  print_v2xint64 ("        )=", (vui64_t)k);
#endif
  rc += check_vuint128x ("vec_mulosw:", (vui128_t)k, (vui128_t) e);

  i = (vi32_t) {-1, -1, -1, -1};
  j = (vi32_t) { 1, -1,  1, -1};
  e = (vi64_t) {1, 1};
  k = vec_mulosw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d  ("mulosw  ( ", (vui32_t)i);
  print_vint32d  ("         ,", (vui32_t)j);
  print_v2xint64 ("        )=", (vui64_t)k);
#endif
  rc += check_vuint128x ("vec_mulosw:", (vui128_t)k, (vui128_t) e);

  i = (vi32_t) {99999999, 99999999, 99999999, 99999999};
  j = (vi32_t) {99999999, -99999999, -99999999, 99999999};
  e = (vi64_t) {-9999999800000001LL, 9999999800000001LL};
  k = vec_mulosw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d  ("mulosw  ( ", (vui32_t)i);
  print_vint32d  ("         ,", (vui32_t)j);
  print_v2xint64 ("        )=", (vui64_t)k);
#endif
  rc += check_vuint128x ("vec_mulosw:", (vui128_t)k, (vui128_t) e);

  i = (vi32_t) {88888888, 88888888, 88888888, 88888888};
  j = (vi32_t) {88888888, -88888888, -88888888, 88888888};
  e = (vi64_t) {-7901234409876544LL, 7901234409876544LL};
  k = vec_mulosw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d  ("mulosw  ( ", (vui32_t)i);
  print_vint32d  ("         ,", (vui32_t)j);
  print_v2xint64 ("        )=", (vui64_t)k);
#endif
  rc += check_vuint128x ("vec_mulosw:", (vui128_t)k, (vui128_t) e);

  i = (vi32_t) {99999999, 99999999, 99999999, 99999999};
  j = (vi32_t) {88888888, -88888888, -88888888, 88888888};
  e = (vi64_t) {-8888888711111112LL, 8888888711111112LL};
  k = vec_mulosw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d  ("mulosw  ( ", (vui32_t)i);
  print_vint32d  ("         ,", (vui32_t)j);
  print_v2xint64 ("        )=", (vui64_t)k);
#endif
  rc += check_vuint128x ("vec_mulosw:", (vui128_t)k, (vui128_t) e);

  i = (vi32_t) {99999999, 0, 99999999, 0};
  j = (vi32_t) {0, 99999999, 0, 99999999};
  e = (vi64_t) {0UL, 0UL};
  k = vec_mulosw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d  ("mulosw  ( ", (vui32_t)i);
  print_vint32d  ("         ,", (vui32_t)j);
  print_v2xint64 ("        )=", (vui64_t)k);
#endif
  rc += check_vuint128x ("vec_mulosw:", (vui128_t)k, (vui128_t) e);

  i = (vi32_t) {__INT_MAX__, __INT_MAX__, __INT_MAX__, __INT_MAX__};
  j = (vi32_t) {__INT_MAX__, -__INT_MAX__, -__INT_MAX__, __INT_MAX__};
  e = (vi64_t) { -4611686014132420609LL, 4611686014132420609LL};
  k = vec_mulosw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d  ("mulosw  ( ", (vui32_t)i);
  print_vint32d  ("         ,", (vui32_t)j);
  print_v2xint64 ("        )=", (vui64_t)k);
#endif
  rc += check_vuint128x ("vec_mulosw:", (vui128_t)k, (vui128_t) e);

  i = (vi32_t) { 0x80000000, 0x80000000, 0x80000000, 0x80000000};
  j = (vi32_t) { 0x80000000, 0x80000000, __INT_MAX__, __INT_MAX__};
  e = (vi64_t) { 0x4000000000000000LL, 0xc000000080000000LL};
  k = vec_mulosw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint32d  ("mulosw  ( ", (vui32_t)i);
  print_vint32d  ("         ,", (vui32_t)j);
  print_v2xint64 ("        )=", (vui64_t)k);
#endif
  rc += check_vuint128x ("vec_mulosw:", (vui128_t)k, (vui128_t) e);

  return (rc);
}
//#undef __DEBUG_PRINT__

int
test_muleuw (void)
{
  vui32_t i, j;
  vui64_t k, e;
  int rc = 0;

  printf ("\ntest_muleuw Vector Multiply Even Unsigned words\n");

  i = (vui32_t) {1, 2, 3, 4};
  j = (vui32_t) {10, 20, 30, 40};
  e = (vui64_t) {10, 90};
  k = vec_muleuw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleuw({1, 2, 3, 4}, {10, 20, 30, 40}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleuw:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {1, 2, 3, 4};
  j = (vui32_t) {3, 4, 1, 2};
  e = (vui64_t) {3, 3};
  k = vec_muleuw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleuw({1, 2, 3, 4}, {3, 4, 1, 2}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleuw:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {1, 2, 3, 4};
  j = (vui32_t) {4, 3, 2, 1};
  e = (vui64_t) {4, 6};
  k = vec_muleuw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleuw({1, 2, 3, 4}, {4, 3, 2, 1}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleuw:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {1, 2, 3, 4};
  j = (vui32_t) {10, 10, 30, 30};
  e = (vui64_t) {10, 90};
  k = vec_muleuw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleuw({1, 2, 3, 4}, {10, 10, 30, 30}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleuw:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {-1, -1, -1, -1};
  j = (vui32_t) {-1, -1, -1, -1};
  e = (vui64_t) {-8589934591, -8589934591};
  k = vec_muleuw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleuw({-1, -1, -1, -1}, {-1, -1, -1, -1}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleuw:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {99999999, 99999999, 99999999, 99999999};
  j = (vui32_t) {99999999, 99999999, 99999999, 99999999};
  e = (vui64_t) {9999999800000001UL, 9999999800000001UL};
  k = vec_muleuw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleuw({9s, 9s, 9s, 9s}, {9s, 9s, 9s, 9s}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleuw:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {88888888, 88888888, 88888888, 88888888};
  j = (vui32_t) {88888888, 88888888, 88888888, 88888888};
  e = (vui64_t) {7901234409876544UL, 7901234409876544UL};
  k = vec_muleuw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleuw({8s, 8s, 8s, 8s}, {8s, 8s, 8s, 8s}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleuw:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {99999999, 99999999, 99999999, 99999999};
  j = (vui32_t) {88888888, 88888888, 88888888, 88888888};
  e = (vui64_t) {8888888711111112UL, 8888888711111112UL};
  k = vec_muleuw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleuw({9s, 9s, 9s, 9s}, {8s, 8s, 8s, 8s}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleuw:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {0, 99999999, 0, 99999999};
  j = (vui32_t) {0, 99999999, 0, 99999999};
  e = (vui64_t) {0UL, 0UL};
  k = vec_muleuw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleuw({0, 99999999, 0, 99999999}, {0, 99999999, 0, 99999999}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleuw:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {0, 9999, 0, 9999};
  j = (vui32_t) {0, 9999, 0, 9999};
  e = (vui64_t) {0UL, 0UL};
  k = vec_muleuw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleuw({0, 9999, 0, 9999}, {0, 9999, 0, 9999}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleuw:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {0, 196612, 0, 65538};
  j = (vui32_t) {0, 196612, 0, 65538};
  e = (vui64_t) {0UL, 0UL};
  k = vec_muleuw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleuw({0, 196612, 0, 65538}, {0, 196612, 0, 65538}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleuw:", (vui128_t)k, (vui128_t) e);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_mulouw (void)
{
  vui32_t i, j;
  vui64_t k, e;
  int rc = 0;

  printf ("\ntest_mulouw Vector Multiply Odd Unsigned words\n");
  i = (vui32_t) {1, 2, 3, 4};
  j = (vui32_t) {10, 20, 30, 40};
  e = (vui64_t) {40, 160};
  k = vec_mulouw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("mulouw({1, 2, 3, 4}, {10, 20, 30, 40}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulouw:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {1, 2, 3, 4};
  j = (vui32_t) {3, 4, 1, 2};
  e = (vui64_t) {8, 8};
  k = vec_mulouw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("mulouw({1, 2, 3, 4}, {3, 4, 1, 2}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulouw:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {1, 2, 3, 4};
  j = (vui32_t) {4, 3, 2, 1};
  e = (vui64_t) {6, 4};
  k = vec_mulouw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("mulouw({1, 2, 3, 4}, {4, 3, 2, 1}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulouw:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {1, 2, 3, 4};
  j = (vui32_t) {10, 10, 30, 30};
  e = (vui64_t) {20, 120};
  k = vec_mulouw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("mulouw({1, 2, 3, 4}, {10, 10, 30, 30}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulouw:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {-1, -1, -1, -1};
  j = (vui32_t) {-1, -1, -1, -1};
  e = (vui64_t) {-8589934591, -8589934591};
  k = vec_mulouw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("mulouw({-1, -1, -1, -1}, {-1, -1, -1, -1}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulouw:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {99999999, 99999999, 99999999, 99999999};
  j = (vui32_t) {99999999, 99999999, 99999999, 99999999};
  e = (vui64_t) {9999999800000001UL, 9999999800000001UL};
  k = vec_mulouw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("mulouw({9s, 9s, 9s, 9s}, {9s, 9s, 9s, 9s}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulouw:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {88888888, 88888888, 88888888, 88888888};
  j = (vui32_t) {88888888, 88888888, 88888888, 88888888};
  e = (vui64_t) {7901234409876544UL, 7901234409876544UL};
  k = vec_mulouw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("mulouw({8s, 8s, 8s, 8s}, {8s, 8s, 8s, 8s}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulouw:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {99999999, 99999999, 99999999, 99999999};
  j = (vui32_t) {88888888, 88888888, 88888888, 88888888};
  e = (vui64_t) {8888888711111112UL, 8888888711111112UL};
  k = vec_mulouw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("mulouw({9s, 9s, 9s, 9s}, {8s, 8s, 8s, 8s}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulouw:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {0, 99999999, 0, 99999999};
  j = (vui32_t) {0, 99999999, 0, 99999999};
  e = (vui64_t) {9999999800000001UL, 9999999800000001UL};
  k = vec_mulouw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("mulouw({0, 99999999, 0, 99999999}, {0, 99999999, 0, 99999999}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulouw:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {0, 9999, 0, 9999};
  j = (vui32_t) {0, 9999, 0, 9999};
  e = (vui64_t) {99980001UL, 99980001UL};
  k = vec_mulouw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("mulouw({0, 9999, 0, 9999}, {0, 9999, 0, 9999}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulouw:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {0, 196612, 0, 65538};
  j = (vui32_t) {0, 196612, 0, 65538};
  e = (vui64_t) {38656278544UL, 4295229444UL};
  k = vec_mulouw (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("mulouw({0, 196612, 0, 65538}, {0, 196612, 0, 65538}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulouw:", (vui128_t)k, (vui128_t) e);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_muluwm (void)
{
  vui32_t i, j, k, e;
  int rc = 0;

  printf ("\ntest_muluwm Vector Multiply Unsigned Word Modulo\n");
  i = (vui32_t) {1, 2, 3, 4};
  j = (vui32_t) {10, 20, 30, 40};
  e = (vui32_t) {10, 40, 90, 160};
  k = vec_muluwm (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muluwm({1, 2, 3, 4}, {10, 20, 30, 40}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muluwm:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {1, 2, 3, 4};
  j = (vui32_t) {3, 4, 1, 2};
  e = (vui32_t) {3, 8, 3, 8};
  k = vec_muluwm (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muluwm({1, 2, 3, 4}, {3, 4, 1, 2}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muluwm:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {1, 2, 3, 4};
  j = (vui32_t) {4, 3, 2, 1};
  e = (vui32_t) {4, 6, 6, 4};
  k = vec_muluwm (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muluwm({1, 2, 3, 4}, {4, 3, 2, 1}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muluwm:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {1, 2, 3, 4};
  j = (vui32_t) {10, 10, 30, 30};
  e = (vui32_t) {10, 20, 90, 120};
  k = vec_muluwm (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muluwm({1, 2, 3, 4}, {10, 10, 30, 30}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muluwm:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {-1, -1, -1, -1};
  j = (vui32_t) {-1, -1, -1, -1};
  e = (vui32_t) {1, 1, 1, 1};
  k = vec_muluwm (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muluwm({-1, -1, -1, -1}, {-1, -1, -1, -1}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muluwm:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {99999999, 99999999, 99999999, 99999999};
  j = (vui32_t) {99999999, 99999999, 99999999, 99999999};
  e = (vui32_t) {0x63d53e01, 0x63d53e01, 0x63d53e01, 0x63d53e01};
  k = vec_muluwm (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muluwm({9s, 9s, 9s, 9s}, {9s, 9s, 9s, 9s}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muluwm:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {88888888, 88888888, 88888888, 88888888};
  j = (vui32_t) {88888888, 88888888, 88888888, 88888888};
  e = (vui32_t) {0x7e49ac40, 0x7e49ac40, 0x7e49ac40, 0x7e49ac40};
  k = vec_muluwm (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muluwm({8s, 8s, 8s, 8s}, {8s, 8s, 8s, 8s}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muluwm:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {99999999, 99999999, 99999999, 99999999};
  j = (vui32_t) {88888888, 88888888, 88888888, 88888888};
  e = (vui32_t) {0xae12e1c8, 0xae12e1c8, 0xae12e1c8, 0xae12e1c8};
  k = vec_muluwm (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muluwm({9s, 9s, 9s, 9s}, {8s, 8s, 8s, 8s}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muluwm:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {0, 99999999, 0, 99999999};
  j = (vui32_t) {0, 99999999, 0, 99999999};
  e = (vui32_t) {0, 0x63d53e01, 0, 0x63d53e01};
  k = vec_muluwm (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muluwm({0, 99999999, 0, 99999999}, {0, 99999999, 0, 99999999}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muluwm:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {0, 9999, 0, 9999};
  j = (vui32_t) {0, 9999, 0, 9999};
  e = (vui32_t) {0, 99980001, 0, 99980001};
  k = vec_muluwm (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muluwm({0, 9999, 0, 9999}, {0, 9999, 0, 9999}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muluwm:", (vui128_t)k, (vui128_t) e);

  i = (vui32_t) {0, 196612, 0, 65538};
  j = (vui32_t) {0, 196612, 0, 65538};
  e = (vui32_t) {0, 0x00180010, 0, 0x00040004};
  k = vec_muluwm (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muluwm({0, 196612, 0, 65538}, {0, 196612, 0, 65538}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muluwm:", (vui128_t)k, (vui128_t) e);

  return (rc);
}

int
test_vec_i32 (void)
{
  int rc = 0;

  printf ("\n%s\n", __FUNCTION__);

  rc += test_revbw ();
  rc += test_clzw ();
  rc += test_popcntw();
  rc += test_mulesw();
  rc += test_mulosw();
  rc += test_muleuw();
  rc += test_mulouw();
  rc += test_muluwm();
  rc += test_mrgahlw();
  rc += test_mrgeow();
  rc += test_mulhuw();

  return (rc);
}
