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

 arith128_test_i16.c

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

#include <testsuite/arith128_test_i16.h>

int
test_clzh (void)
{
  vui16_t i, e, j;
  int rc = 0;

  printf ("\ntest_clzh Vector Count Leading Zeros in halfwords\n");

  i = (vui16_t )CONST_VINT16_H(0, 0, 0, 0, 0, 0, 0, 0);
  e = (vui16_t )CONST_VINT16_H(16, 16, 16, 16, 16, 16, 16, 16);
  j = vec_clzh(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0) ", j);
#endif
  rc += check_vuint128x ("vec_clzh:", (vui128_t)j, (vui128_t) e);

  i = (vui16_t )CONST_VINT16_H(0x8000, 0xc000, 0xe000, 0xf000, 0xf800, 0xfc00, 0xfe00, 0xff00);
  e = (vui16_t )CONST_VINT16_H(0, 0, 0, 0, 0, 0, 0, 0);
  j = vec_clzh(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0x8000, 0xc000, 0xe000, 0xf000, 0xf800, 0xfc00, 0xfe00, 0xff00) ", j);
#endif
  rc += check_vuint128x ("vec_clzh:", (vui128_t)j, (vui128_t) e);

  i = (vui16_t )CONST_VINT16_H(0, 1, 2, 4, 8, 16, 32, 64);
  e = (vui16_t )CONST_VINT16_H(16, 15, 14, 13, 12, 11, 10, 9);
  j = vec_clzh(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0, 1, 2, 4, 8, 16, 32, 64) ", j);
#endif
  rc += check_vuint128x ("vec_clzh:", (vui128_t)j, (vui128_t) e);

  i = (vui16_t )CONST_VINT16_H(128, 256, 256, 512, 1024, 2048, 4096, 8192);
  e = (vui16_t )CONST_VINT16_H(8, 7, 7, 6, 5, 4, 3, 2);
  j = vec_clzh(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(128, 256, 256, 512, 1024, 2048, 4096, 8192) ", j);
#endif
  rc += check_vuint128x ("vec_clzh:", (vui128_t)j, (vui128_t) e);

  i = (vui16_t )CONST_VINT16_H(16384, 32768, 61440, 65280, 65520, 65535, 43690, 21845);
  e = (vui16_t )CONST_VINT16_H(1, 0, 0, 0, 0, 0, 0, 1);
  j = vec_clzh(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(16384, 32768, 61440, 65280, 65520, 65535, 43690, 21845) ", j);
#endif
  rc += check_vuint128x ("vec_clzh:", (vui128_t)j, (vui128_t) e);

  return (rc);
}

int
test_popcnth (void)
{
  vui16_t i, e;
  vui16_t j;
  int rc = 0;

  printf ("\ntest_popcnth Vector Pop Count halfword\n");

  i = (vui16_t){0, 1, 2, 4, 8, 16, 32, 64};
  e = (vui16_t){0, 1, 1, 1, 1, 1, 1, 1};
  j = vec_popcnth(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcnth({0, 1, 2, 4, 8, 16, 32, 64}) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcnth:", (vui128_t)j, (vui128_t) e);

  i = (vui16_t){128, 256, 256, 512, 1024, 2048, 4096, 8192};
  e = (vui16_t){1, 1, 1, 1, 1, 1, 1, 1};
  j = vec_popcnth(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcnth({128, 256, 256, 512, 1024, 2048, 4096, 8192}) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcnth:", (vui128_t)j, (vui128_t) e);

  i = (vui16_t){16384, 32768, 61440, 65280, 65520, 65535, 43690, 21845};
  e = (vui16_t){1, 1, 4, 8, 12, 16, 8, 8};
  j = vec_popcnth(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcnth({16384, 32768, 61440, 65280, 65520, 65535, 43690, 21845}) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcnth:", (vui128_t)j, (vui128_t) e);

  i = (vui16_t){1, 516, 2064, 8256, 32904, 8721, 61695, 43605};
  e = (vui16_t){1, 2, 2, 2, 3, 4, 12, 8};
  j = vec_popcnth(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcnth({1, 516, 2064, 8256, 32904, 8721, 61695, 43605}) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcnth:", (vui128_t)j, (vui128_t) e);

  i = (vui16_t){2016, 43141, 4080, 57770, 8184, 20453, 16373, 60350};
  e = (vui16_t){6, 6, 8, 8, 10, 10, 12, 12};
  j = vec_popcnth(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcnth({2016, 43141, 4080, 57770, 8184, 20453, 16373, 60350}) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcnth:", (vui128_t)j, (vui128_t) e);

  i = (vui16_t){32763, 57327, 65280, 65280, 6274, 35504, 12709, 5948};
  e = (vui16_t){14, 14, 8, 8, 4, 6, 7, 8};
  j = vec_popcnth(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcnth({32763, 57327, 65280, 65280, 6274, 35504, 12709, 5948}) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcnth:", (vui128_t)j, (vui128_t) e);

  return (rc);
}

int
test_revbh (void)
{
  vui32_t i, e, *ip;
  vui128_t k;
  unsigned char mem[16] __attribute__ ((aligned (16))) = { 0xf0, 0xf1, 0xf2,
      0xf3, 0xe0, 0xe1, 0xe2, 0xe3, 0xd0, 0xd1, 0xd2, 0xd3, 0xc0, 0xc1, 0xc2,
      0xc3 };
  int rc = 0;

  printf ("\ntest_revbh Reverse Bytes in halfwords\n");

  i = (vui32_t ) { 0, 1, 2, 3 };
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  e = (vui32_t)CONST_VINT32_W(0x00000000, 0x00000100, 0x00000200, 0x00000300);
#else
  e = (vui32_t)CONST_VINT32_W(0x00000300, 0x00000200, 0x00000100, 0x00000000);
#endif
  k = (vui128_t) vec_revbh ((vui16_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_revbh i=", (vui128_t)i);
  print_vint128x ("         k=", k);
#endif
  rc += check_vuint128x ("vec_revbh 1:", k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0x01020304, 0x11121314, 0x21222324, 0x31323334);
  e = (vui32_t)CONST_VINT32_W(0x02010403, 0x12111413, 0x22212423, 0x32313433);
  k = (vui128_t) vec_revbh ((vui16_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_revbh i=", (vui128_t)i);
  print_vint128x ("         k=", k);
#endif
  rc += check_vuint128x ("vec_revbh 2:", k, (vui128_t) e);

  ip = (vui32_t*) mem;
  i = *ip;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  e = (vui32_t)CONST_VINT32_W(0xf1f0f3f2, 0xe1e0e3e2, 0xd1d0d3d2, 0xc1c0c3c2);
#else
  e = (vui32_t)CONST_VINT32_W(0xc2c3c0c1, 0xd2d3d0d1, 0xe2e3e0e1, 0xf2f3f0f1);
#endif
  k = (vui128_t) vec_revbh ((vui16_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_revbh i=", (vui128_t)i);
  print_vint128x ("         k=", k);
#endif
  rc += check_vuint128x ("vec_revbh 3:", k, (vui128_t) e);

  return (rc);
}

int
test_vec_i16 (void)
{
  int rc = 0;

  printf ("\n%s\n", __FUNCTION__);
#if 1
  rc += test_revbh ();
  rc += test_clzh ();
  rc += test_popcnth();
#endif
  return (rc);
}
