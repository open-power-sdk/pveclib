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

//#define __DEBUG_PRINT__
#include <testsuite/arith128_print.h>
#include <pveclib/vec_common_ppc.h>
#include <pveclib/vec_int32_ppc.h>

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
test_ctzh (void)
{
  vui16_t i, e, j;
  int rc = 0;

  printf ("\ntest_ctzh Vector Count Trailing Zeros in halfwords\n");

  i = (vui16_t )CONST_VINT16_H(0, 0, 0, 0, 0, 0, 0, 0);
  e = (vui16_t )CONST_VINT16_H(16, 16, 16, 16, 16, 16, 16, 16);
  j = vec_ctzh(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0) ", j);
#endif
  rc += check_vuint128x ("vec_ctzh:", (vui128_t)j, (vui128_t) e);

  i = (vui16_t )CONST_VINT16_H(0x8000, 0xc000, 0xe000, 0xf000, 0xf800, 0xfc00, 0xfe00, 0xff00);
  e = (vui16_t )CONST_VINT16_H(15, 14, 13, 12, 11, 10, 9, 8);
  j = vec_ctzh(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0x8000, 0xc000, 0xe000, 0xf000, 0xf800, 0xfc00, 0xfe00, 0xff00) ", j);
#endif
  rc += check_vuint128x ("vec_ctzh:", (vui128_t)j, (vui128_t) e);

  i = (vui16_t )CONST_VINT16_H(0, 1, 2, 4, 8, 16, 32, 64);
  e = (vui16_t )CONST_VINT16_H(16, 0, 1, 2, 3, 4, 5, 6);
  j = vec_ctzh(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0, 1, 2, 4, 8, 16, 32, 64) ", j);
#endif
  rc += check_vuint128x ("vec_ctzh:", (vui128_t)j, (vui128_t) e);

  i = (vui16_t )CONST_VINT16_H(128, 256, 256, 512, 1024, 2048, 4096, 8192);
  e = (vui16_t )CONST_VINT16_H(7, 8, 8, 9, 10, 11, 12, 13);
  j = vec_ctzh(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(128, 256, 256, 512, 1024, 2048, 4096, 8192) ", j);
#endif
  rc += check_vuint128x ("vec_ctzh:", (vui128_t)j, (vui128_t) e);

  i = (vui16_t )CONST_VINT16_H(16384, 32768, 61440, 65280, 65520, 65535, 43690, 21845);
  e = (vui16_t )CONST_VINT16_H(14, 15, 12, 8, 4, 0, 1, 0);
  j = vec_ctzh(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(16384, 32768, 61440, 65280, 65520, 65535, 43690, 21845) ", j);
#endif
  rc += check_vuint128x ("vec_ctzh:", (vui128_t)j, (vui128_t) e);

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

//#define __DEBUG_PRINT__
int
test_muluhm (void)
{
  vui16_t i, j, k, e;
  int rc = 0;

  printf ("\ntest_muluhm Vector Multiply Unsigned Halfword Modulo\n");

  i = (vui16_t) {1, 2, 3, 4, 5, 6, 7, 8};
  j = (vui16_t) {10, 20, 30, 40, 50, 60, 70, 80};
  e = (vui16_t) {10, 40, 90, 160, 250, 360, 490, 640};
  k = vec_muluhm (i, j);

#ifdef __DEBUG_PRINT__
  print_vint16d ("muluhm(\t{1, 2, 3, 4, ....},\n\t {10, 20, 30, 40, ...})\n\t ", (vui16_t)k);
#endif
  rc += check_vuint128x ("vec_muluhm:", (vui128_t)k, (vui128_t) e);

  i = (vui16_t ){-1, -2, -3, -4, -5, -6, -7, -8};
  j = (vui16_t) {10, 20, 30, 40, 50, 60, 70, 80};
  e = (vui16_t) {-10, -40, -90, -160, -250, -360, -490, -640};
  k = vec_muluhm (i, j);

#ifdef __DEBUG_PRINT__
  print_vint16d ("muluhm(\t{-1, -2, -3, -4, ....},\n\t {10, 20, 30, 40, ...})\n\t ", (vui16_t)k);
#endif
  rc += check_vuint128x ("vec_muluhm:", (vui128_t)k, (vui128_t) e);

  i = (vui16_t) {1, 2, 3, 4, 5, 6, 7, 8};
  j = (vui16_t) {-10, -20, -30, -40, -50,- 60, -70, -80};
  e = (vui16_t) {-10, -40, -90, -160, -250, -360, -490, -640};
  k = vec_muluhm (i, j);

#ifdef __DEBUG_PRINT__
  print_vint16d ("muluhm(\t{1, 2, 3, 4, ....},\n\t {-10, -20, -30, -40, ...})\n\t ", (vui16_t)k);
#endif
  rc += check_vuint128x ("vec_muluhm:", (vui128_t)k, (vui128_t) e);

  i = (vui16_t) {-1, -2, -3, -4, -5, -6, -7, -8};
  j = (vui16_t) {-10, -20, -30, -40, -50,- 60, -70, -80};
  e = (vui16_t) {10, 40, 90, 160, 250, 360, 490, 640};
  k = vec_muluhm (i, j);

#ifdef __DEBUG_PRINT__
  print_vint16x ("muluhm(\t{", i);
  print_vint16x ("\t\t{", j);
  print_vint16x ("\t\t{", k);
#endif
  rc += check_vuint128x ("vec_muluhm:", (vui128_t)k, (vui128_t) e);

  return (rc);
}

int
test_mulhsh (void)
{
  vi16_t i, j;
  vi16_t k, e;
  int rc = 0;

  printf ("\ntest_mmulhsh Vector Multiply High Signed Halfwords\n");

  i = (vi16_t) CONST_VINT128_H(-1, -1, -1, -1, -1, -1, -1, -1);
  j = (vi16_t) CONST_VINT128_H(1, 2, 3, 4, 5, 6, 7, 8);
  e = (vi16_t) CONST_VINT128_H(-1, -1, -1, -1, -1, -1, -1, -1);
  k = vec_mulhsh (i, j);

#ifdef __DEBUG_PRINT__
  print_vint16d ("mulhsh(\t{-1,-1,-1,-1,-1,-1,-1,-1},\n\t {1,2,3,4,5,6,7,8})\n\t ", (vui16_t)k);
#endif
  rc += check_vuint128x ("vec_mulhsh:", (vui128_t)k, (vui128_t) e);

  i = (vi16_t) CONST_VINT128_H(-1, -1, -1, -1, -1, -1, -1, -1);
  j = (vi16_t) CONST_VINT128_H(-1, -2, -3, -4, -5, -6, -7, -8);
  e = (vi16_t) CONST_VINT128_H(0, 0, 0, 0, 0, 0, 0, 0);
  k = vec_mulhsh (i, j);

#ifdef __DEBUG_PRINT__
  print_vint16d ("mulhsh(\t{-1,-1,-1,-1,-1,-1,-1,-1},\n\t {-1,-2,-3,-4,-5,-6,-7,-8})\n\t ", (vui16_t)k);
#endif
  rc += check_vuint128x ("vec_mulhsh:", (vui128_t)k, (vui128_t) e);

  i = (vi16_t) CONST_VINT128_H(256, 256, 256, 256, 256, 256, 256, 256);
  j = (vi16_t) CONST_VINT128_H(256, 512, 768, 1024, 1280, 1536, 1792, 2048);
  e = (vi16_t) CONST_VINT128_H(1, 2, 3, 4, 5, 6, 7, 8);
  k = vec_mulhsh (i, j);

#ifdef __DEBUG_PRINT__
  print_vint16d ("mulhsh(\t{256,256, ...},\n\t {2^8,2^9,3*2^8,2^10, ...})\n\t ", (vui16_t)k);
#endif
  rc += check_vuint128x ("vec_mulhsh:", (vui128_t)k, (vui128_t) e);

  i = (vi16_t) CONST_VINT128_H(-256, -256, -256, -256, -256, -256, -256, -256);
  j = (vi16_t) CONST_VINT128_H(256, 512, 768, 1024, 1280, 1536, 1792, 2048);
  e = (vi16_t) CONST_VINT128_H(-1, -2, -3, -4, -5, -6, -7, -8);
  k = vec_mulhsh (i, j);

#ifdef __DEBUG_PRINT__
  print_vint16d ("mulhsh(\t{-256,-256, ...},\n\t {2^8,2^9,3*2^8,2^10, ...})\n\t ", (vui16_t)k);
#endif
  rc += check_vuint128x ("vec_mulhsh:", (vui128_t)k, (vui128_t) e);

  i = (vi16_t) CONST_VINT128_H(-256, -256, -256, -256, -256, -256, -256, -256);
  j = (vi16_t) CONST_VINT128_H(-256, -512, -768, -1024, -1280, -1536, -1792, -2048);
  e = (vi16_t) CONST_VINT128_H(1, 2, 3, 4, 5, 6, 7, 8);
  k = vec_mulhsh (i, j);

#ifdef __DEBUG_PRINT__
  print_vint16d ("mulhsh(\t{-256,-256, ...},\n\t {-2^8,-2^9,-3*2^8,-2^10, ...})\n\t ", (vui16_t)k);
#endif
  rc += check_vuint128x ("vec_mulhsh:", (vui128_t)k, (vui128_t) e);

  i = (vi16_t) CONST_VINT128_H(-32768,-32768,-32768,-32768, 32767, 32767, 32767, 32767);
  j = (vi16_t) CONST_VINT128_H(-32768, 32767,-16384, 16384,-32768, 32767,-16384, 16384);
  e = (vi16_t) CONST_VINT128_H( 16384,-16384,  8192, -8192,-16384, 16383, -8192,  8191);
  k = vec_mulhsh (i, j);

#ifdef __DEBUG_PRINT__
  print_vint16x ("mulhsh(\t{", (vui16_t) i);
  print_vint16x ("\t\t{", (vui16_t) j);
  print_vint16x ("\t\t{", (vui16_t) k);
#endif
  rc += check_vuint128x ("vec_mulhsh:", (vui128_t)k, (vui128_t) e);

  return (rc);
}

int
test_mulhuh (void)
{
  vui16_t i, j;
  vui16_t k, e;
  int rc = 0;

  printf ("\ntest_mmulhuh Vector Multiply High unsigned Halfwords\n");

  i = (vui16_t) CONST_VINT128_H(-1, -1, -1, -1, -1, -1, -1, -1);
  j = (vui16_t) CONST_VINT128_H(1, 2, 3, 4, 5, 6, 7, 8);
  e = (vui16_t) CONST_VINT128_H(0, 1, 2, 3, 4, 5, 6, 7);
  k = vec_mulhuh (i, j);

#ifdef __DEBUG_PRINT__
  print_vint16d ("mulhuh(\t{-1,-1,-1,-1,-1,-1,-1,-1},\n\t {1,2,3,4,5,6,7,8})\n\t ", k);
#endif
  rc += check_vuint128x ("vec_mulhuh:", (vui128_t)k, (vui128_t) e);

  i = (vui16_t) CONST_VINT128_H(0x100, 0x100, 0x100, 0x100, 0x100, 0x100, 0x100, 0x100);
  j = (vui16_t) CONST_VINT128_H(0x100, 0x200, 0x300, 0x400, 0x500, 0x600, 0x700, 0x800);
  e = (vui16_t) CONST_VINT128_H(1, 2, 3, 4, 5, 6, 7, 8);
  k = vec_mulhuh (i, j);

#ifdef __DEBUG_PRINT__
  print_vint16d ("mulhuh(\t{2^8,2^8,2^8,2^8, ...},\n\t {2^8,2^9,3*2^8,2^10,...})\n\t ", k);
#endif
  rc += check_vuint128x ("vec_mulhuh:", (vui128_t)k, (vui128_t) e);

  i = (vui16_t) CONST_VINT128_H(10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000);
  j = (vui16_t) CONST_VINT128_H(7, 68, 661, 6554, 7, 68, 661, 6554);
  e = (vui16_t) CONST_VINT128_H(1, 10, 100, 1000, 1, 10, 100, 1000);
  k = vec_mulhuh (i, j);

#ifdef __DEBUG_PRINT__
  print_vint16d ("mulhuh(\t{10^4,10^4,10^4,10^4, ...},\n\t {10^-4,10^-3,10^-2,10^-1})\n\t ", k);
#endif
  rc += check_vuint128x ("vec_mulhuh:", (vui128_t)k, (vui128_t) e);

  i = (vui16_t) CONST_VINT128_H(65535, 65535, 65535, 65535, 32768, 32767, 16384, 16383);
  j = (vui16_t) CONST_VINT128_H(65535, 32768, 16384, 16383, 32768, 16384, 16384, 16383);
  e = (vui16_t) CONST_VINT128_H(65534, 32767, 16383, 16382, 16384,  8191,  4096,  4095);
  k = vec_mulhuh (i, j);

#ifdef __DEBUG_PRINT__
  print_vint16x ("mulhuh(\t{", i);
  print_vint16x ("\t\t{", j);
  print_vint16d ("\t\t{", k);
#endif
  rc += check_vuint128x ("vec_mulhuh:", (vui128_t)k, (vui128_t) e);

  return (rc);
}

int
test_mrgeoh (void)
{
  vui16_t i, j;
  vui16_t k, e;
  int rc = 0;

  printf ("\ntest_mrgeoh Vector Merge Even/Odd Halfwords\n");

  i = (vui16_t ) {100, 1, 200, 2, 300, 3, 400, 4};
  j = (vui16_t ) {500, 5, 600, 6, 700, 7, 800, 8};
  e = (vui16_t ) {100, 500, 200, 600, 300, 700, 400, 800};
  k = vec_mrgeh (i, j);

#ifdef __DEBUG_PRINT__
  print_vint16d ("mrgeh(\t{100, 1, 200, 2, 300, 3, 400, 4},\n\t    {500, 5, 600, 6, 700, 7, 800, 8})\n\t", k);
  print_vint16x ("\t", k);
#endif
  rc += check_vuint128x ("vec_mrgeh:", (vui128_t)k, (vui128_t) e);

  e = (vui16_t ) {1, 5, 2, 6, 3, 7, 4, 8};
  k = vec_mrgoh (i, j);

#ifdef __DEBUG_PRINT__
  print_vint16d ("mrgoh(\t{100, 1, 200, 2, 300, 3, 400, 4},\n\t    {500, 5, 600, 6, 700, 7, 800, 8})\n\t", k);
  print_vint16x ("\t", k);
#endif
  rc += check_vuint128x ("vec_mrgoh:", (vui128_t)k, (vui128_t) e);

  return (rc);
}

int
test_mrgahlh (void)
{
  vui32_t i, j;
  vui16_t k, e;
  int rc = 0;

  printf ("\ntest_mrgahlh Vector Merge Algebraic High/Low Halfwords\n");

  i = (vui32_t) CONST_VINT32_W (0x00f10001, 0x00f20002, 0x00f30003, 0x00f40004);
  j = (vui32_t) CONST_VINT32_W (0x00f50005, 0x00f60006, 0x00f70007, 0x00f80008);
  e = (vui16_t) CONST_VINT128_H (0xf1, 0xf5, 0xf2, 0xf6, 0xf3, 0xf7, 0xf4, 0xf8);
  k = vec_mrgahh(i, j);

#ifdef __DEBUG_PRINT__
  print_vint16x ("mrgahh( {00f10001 00f20002 00f30003 00f40004},\n\t{00f50005 00f60006 00f70007 00f80008})\n\t", k);
#endif
  rc += check_vuint128x ("vec_mrgahh:", (vui128_t)k, (vui128_t) e);

  e = (vui16_t) CONST_VINT128_H (0x1, 0x5, 0x2, 0x6, 0x3, 0x7, 0x4, 0x8);
  k = vec_mrgalh (i, j);

#ifdef __DEBUG_PRINT__
  print_vint16x ("mrgalh( {00f10001 00f20002 00f30003 00f40004},\n\t{00f50005 00f60006 00f70007 00f80008})\n\t", k);
#endif
  rc += check_vuint128x ("vec_mrgalh:", (vui128_t)k, (vui128_t) e);

  return (rc);
}

int
test_vmadduh (void)
{
  vui16_t i, j, k;
  vui32_t l, e;
  int rc = 0;

  printf ("\ntest_vmadduh Vector Multiply-Add Even/Odd Halfwords\n");

  i = (vui16_t) CONST_VINT16_H(0xffff, 0xffff, 0xffff, 0xffff,
			       10000, 10000, 9999, 9999);
  j = (vui16_t) CONST_VINT16_H(0xffff, 10000, 9999, 0xffff,
			       0xffff, 9999, 10000, 0xffff);
  k = (vui16_t) { 0, 0, 0, 0, 0, 0, 0, 0 };
  l = vec_vmaddeuh (i, j, k);

#ifdef __DEBUG_PRINT__
  print_vint16x (" vmaddeuh ", i);
  print_vint16x ("        * ", j);
  print_vint16x ("        + ", k);
  print_vint32x ("        = ", l);
#endif
  e = (vui32_t) CONST_VINT32_W (0xfffe0001, 0x270ed8f1,
				0x270fd8f0, 99990000);
  rc += check_vuint128x ("vec_vmaddeuh 1:", (vui128_t) l, (vui128_t) e);

  l = vec_vmaddouh (i, j, k);

#ifdef __DEBUG_PRINT__
  print_vint16x (" vmaddouh ", i);
  print_vint16x ("        * ", j);
  print_vint16x ("        + ", k);
  print_vint32x ("        = ", l);
#endif
  e = (vui32_t) CONST_VINT32_W (0x270fd8f0, 0xfffe0001,
				99990000, 0x270ed8f1);
  rc += check_vuint128x ("vec_vmaddouh 2:", (vui128_t) l, (vui128_t) e);

  k = (vui16_t) { 0xffff, 10000, 9999, 0x0000,
                  0x0000, 9999, 10000, 0xffff };
  l = vec_vmaddeuh (i, j, k);

#ifdef __DEBUG_PRINT__
  print_vint16x (" vmaddeuh ", i);
  print_vint16x ("        * ", j);
  print_vint16x ("        + ", k);
  print_vint32x ("        = ", l);
#endif
  e = (vui32_t) CONST_VINT32_W (0xffff0000, 0x270f0000,
				0x270fd8f0, 100000000);
  rc += check_vuint128x ("vec_vmaddeuh 3:", (vui128_t) l, (vui128_t) e);

  l = vec_vmaddouh (i, j, k);

#ifdef __DEBUG_PRINT__
  print_vint16x (" vmaddouh ", i);
  print_vint16x ("        * ", j);
  print_vint16x ("        + ", k);
  print_vint32x ("        = ", l);
#endif
  e = (vui32_t) CONST_VINT32_W (0x27100000, 0xfffe0001,
				99999999, 0x270fd8f0);
  rc += check_vuint128x ("vec_vmaddouh 4:", (vui128_t) l, (vui128_t) e);

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
  rc += test_ctzh ();
  rc += test_popcnth();
  rc += test_mrgeoh();
  rc += test_mrgahlh();
  rc += test_mulhuh();
  rc += test_mulhsh();
  rc += test_muluhm();
  rc += test_vmadduh();
#endif
  return (rc);
}
