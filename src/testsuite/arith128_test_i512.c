/*
 Copyright (c) [2019] Steven Munroe.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 arith128_test_i512.c

 Contributors:
      Steven Munroe
      Created on: Sep 9, 2019
 */

#define __STDC_WANT_DEC_FP__    1

#include <stdint.h>
#include <stdio.h>

//#define __DEBUG_PRINT__
#include <pveclib/vec_common_ppc.h>
#include <pveclib/vec_int128_ppc.h>
#include <pveclib/vec_int512_ppc.h>

#include "arith128.h"
#include <testsuite/arith128_print.h>
#include <testsuite/arith128_test_i512.h>

int
test_mul128x128 (void)
{
  __VEC_U_256 k, e;
  vui128_t i, j;
  int rc = 0;

  printf ("\ntest_mul128x128 vector multiply quadword, 256-bit product\n");

  i = (vui128_t) CONST_VINT128_W (0, 0xffffffff, 0xffffffff, 0xffffffff);
  j = (vui128_t) CONST_VINT128_W (0, 0xffffffff, 0xffffffff, 0xffffffff);
  k = __VEC_PWR_IMP (vec_mul128x128)(i, j);

#ifdef __DEBUG_PRINT__
  print_vint256_prod ("2**96-1 * 2**96-1 ", k, i, j);
#endif
  e.vx0 = (vui128_t) CONST_VINT128_W (0xfffffffe, 0x00000000, 0x00000000, 0x00000001);
  e.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0xffffffff, 0xffffffff);
  rc += check_vint256 ("vec_mul128x128 1:", k.vx1, k.vx0, e.vx1, e.vx0);

  i = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  k = __VEC_PWR_IMP (vec_mul128x128)(i, j);

#ifdef __DEBUG_PRINT__
  print_vint256_prod ("2**128-1 * 2**128-1 ", k, i, j);
#endif
  e.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  e.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  rc += check_vint256 ("vec_mul128x128 2:", k.vx1, k.vx0, e.vx1, e.vx0);

  i = (vui128_t) CONST_VINT128_W (0, 0, 0, 100000000);
  j = (vui128_t) CONST_VINT128_W (0, 0, 0, 100000000);
  k = __VEC_PWR_IMP (vec_mul128x128)(i, j);

#ifdef __DEBUG_PRINT__
  print_vint256_prod ("10**8 * 10**8 ", k, i, j);
#endif
  e.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x002386f2, 0x6fc10000);
  e.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint256 ("vec_mul128x128 3:", k.vx1, k.vx0, e.vx1, e.vx0);

  i = k.vx0;
  j = k.vx0;
  k = __VEC_PWR_IMP (vec_mul128x128)(i, j);

#ifdef __DEBUG_PRINT__
  print_vint256_prod ("10**16 * 10**16 ", k, i, j);
#endif
  e.vx0 = (vui128_t) CONST_VINT128_W (0x000004ee, 0x2d6d415b, 0x85acef81, 0x00000000);
  e.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint256 ("vec_mul128x128 4:", k.vx1, k.vx0, e.vx1, e.vx0);

  i = k.vx0;
  j = k.vx0;
  k = __VEC_PWR_IMP (vec_mul128x128)(i, j);

#ifdef __DEBUG_PRINT__
  print_vint256_prod ("10**32 * 10**32 ", k, i, j);
#endif
  e.vx0 = (vui128_t) CONST_VINT128_W (0x6e38ed64, 0xbf6a1f01, 0x00000000, 0x00000000);
  e.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00184f03, 0xe93ff9f4, 0xdaa797ed);
  rc += check_vint256 ("vec_mul128x128 5:", k.vx1, k.vx0, e.vx1, e.vx0);

  return (rc);
}

int
test_mul256x256 (void)
{
  __VEC_U_512 k, e;
  __VEC_U_256 i, j;
  int rc = 0;

  printf ("\ntest_mul256x256 vector multiply quadword, 512-bit product\n");

  i.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  k = __VEC_PWR_IMP (vec_mul256x256)(i, j);

#ifdef __DEBUG_PRINT__
  print_vint512x_prod ("2**256-1 * 2**256-1 ", k, i, j);
#endif
  e.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  e.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  e.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  e.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint512 ("vec_mul256x256 1:", k, e);

  i.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  k = __VEC_PWR_IMP (vec_mul256x256)(i, j);

#ifdef __DEBUG_PRINT__
  print_vint512x_prod ("2**256-1 * 2**256-1 ", k, i, j);
#endif
  e.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  e.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  e.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  e.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint512 ("vec_mul256x256 2:", k, e);

  i.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  k = __VEC_PWR_IMP (vec_mul256x256)(i, j);

#ifdef __DEBUG_PRINT__
  print_vint512x_prod ("2**256-1 * 2**256-1 ", k, i, j);
#endif
  e.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  e.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  e.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  e.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint512 ("vec_mul256x256 3:", k, e);

  i.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  k = __VEC_PWR_IMP (vec_mul256x256)(i, j);

#ifdef __DEBUG_PRINT__
  print_vint512x_prod ("2**256-1 * 2**256-1 ", k, i, j);
#endif
  e.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  e.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  e.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  e.vx3 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  rc += check_vint512 ("vec_mul256x256 4:", k, e);

  i.vx0 = (vui128_t) CONST_VINT128_W (0x000004ee, 0x2d6d415b, 0x85acef81, 0x00000000);
  i.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx0 = (vui128_t) CONST_VINT128_W (0x000004ee, 0x2d6d415b, 0x85acef81, 0x00000000);
  j.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  k = __VEC_PWR_IMP (vec_mul256x256)(i, j);

#ifdef __DEBUG_PRINT__
  print_vint512x_prod ("10**32 * 10**32 ", k, i, j);
#endif
  e.vx0 = (vui128_t) CONST_VINT128_W (0x6e38ed64, 0xbf6a1f01, 0x00000000, 0x00000000);
  e.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00184f03, 0xe93ff9f4, 0xdaa797ed);
  e.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  e.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint512 ("vec_mul256x256 5:", k, e);

  i.vx0 = e.vx0;
  i.vx1 = e.vx1;
  j.vx0 = e.vx0;
  j.vx1 = e.vx1;
  k = __VEC_PWR_IMP (vec_mul256x256)(i, j);

#ifdef __DEBUG_PRINT__
  print_vint512x_prod ("10**64 * 10**64 ", k, i, j);
#endif
  e.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  e.vx1 = (vui128_t) CONST_VINT128_W (0x2374e42f, 0x0f1538fd, 0x03df9909, 0x2e953e01);
  e.vx2 = (vui128_t) CONST_VINT128_W (0xa6337f19, 0xbccdb0da, 0xc404dc08, 0xd3cff5ec);
  e.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x0000024e, 0xe91f2603);
  rc += check_vint512 ("vec_mul256x256 6:", k, e);

  return (rc);
}


int
test_mul512x128_MN (void)
{
  __VEC_U_640 k;
  __VEC_U_512x1 ke, ep;
  __VEC_U_512 i;
  vui128_t *kp, *ip, *jp;
  vui128_t j;
  int rc = 0;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  kp = &k.vx0;
  ip = &i.vx0;
  jp = &j;
#else
  kp = &k.vx4;
  ip = &i.vx3;
  jp = &j;
#endif

  printf ("\ntest_mul512x128_MN vector multiply quadword, 640-bit product\n");

  i.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  i.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  i.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j     = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  __VEC_PWR_IMP (vec_mul128_byMN) (kp, ip, jp, 4, 1);

#ifdef __DEBUG_PRINT__
  print_vint512x (" 2**128-1   ", i);
  print_vint128x (" * 2**128-1 ", j);
  print_vint640x ("          = ", k);
#endif
  ke.x640 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x128 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_mul512x128_MN 1a:", ke.x2.v1x128, ep.x2.v1x128);
  rc += check_vint512   ("vec_mul512x128_MN 1b:", ke.x2.v0x512, ep.x2.v0x512);

  __VEC_PWR_IMP (vec_mul128_byMN) (kp, ip, jp, 4, 1);

#ifdef __DEBUG_PRINT__
  print_vint512x (" 2**128-1   ", i);
  print_vint128x (" * 2**128-1 ", j);
  print_vint640x ("          = ", k);
#endif
  ke.x640 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x128 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_mul512x128_MN 2a:", ke.x2.v1x128, ep.x2.v1x128);
  rc += check_vint512   ("vec_mul512x128_MN 2b:", ke.x2.v0x512, ep.x2.v0x512);

  i.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  i.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  i.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  i.vx3 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j     = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);

  __VEC_PWR_IMP (vec_mul128_byMN) (kp, ip, jp, 4, 1);

#ifdef __DEBUG_PRINT__
  kp.x640 = k;
  print_vint512x (" 2**512-2**396-1 ", i);
  print_vint128x ("      * 2**128-1 ", j);
  print_vint640x ("               = ", k);
#endif
  ke.x640 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ep.x2.v1x128 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  rc += check_vuint128x ("vec_mul512x128_MN 3a:", ke.x2.v1x128, ep.x2.v1x128);
  rc += check_vint512   ("vec_mul512x128_MN 3b:", ke.x2.v0x512, ep.x2.v0x512);

  i.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx3 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j     = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);

  __VEC_PWR_IMP (vec_mul128_byMN) (kp, ip, jp, 4, 1);

#ifdef __DEBUG_PRINT__
  print_vint512x ("  2**512-1 ", i);
  print_vint128x ("* 2**128-1 ", j);
  print_vint640x ("         = ", k);
#endif
  ke.x640 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  ep.x2.v1x128 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  rc += check_vuint128x ("vec_mul512x128_MN 4a:", ke.x2.v1x128, ep.x2.v1x128);
  rc += check_vint512   ("vec_mul512x128_MN 4b:", ke.x2.v0x512, ep.x2.v0x512);

  return (rc);
}

int
test_mul512x128 (void)
{
  __VEC_U_640 k;
  __VEC_U_512x1 kp, ep;
  __VEC_U_512 i;
  vui128_t j;
  int rc = 0;

  printf ("\ntest_mul512x128 vector multiply quadword, 512-bit product\n");

  i.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  i.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  i.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j     = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  k = __VEC_PWR_IMP (vec_mul512x128)(i, j);

#ifdef __DEBUG_PRINT__
  print_vint512x (" 2**128-1   ", i);
  print_vint128x (" * 2**128-1 ", j);
  print_vint640x ("          = ", k);
#endif
  kp.x640 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x128 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_mul512x128 1a:", kp.x2.v1x128, ep.x2.v1x128);
  rc += check_vint512   ("vec_mul512x128 1b:", kp.x2.v0x512, ep.x2.v0x512);

  i.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  i.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  i.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j     = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  k = __VEC_PWR_IMP (vec_mul512x128)(i, j);

#ifdef __DEBUG_PRINT__
  print_vint512x (" 2**256-2**128-1 ", i);
  print_vint128x ("      * 2**128-1 ", j);
  print_vint640x ("               = ", k);
#endif
  kp.x640 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x128 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_mul512x128 2a:", kp.x2.v1x128, ep.x2.v1x128);
  rc += check_vint512   ("vec_mul512x128 2b:", kp.x2.v0x512, ep.x2.v0x512);

  i.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  i.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  i.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j     = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  k = __VEC_PWR_IMP (vec_mul512x128)(i, j);

#ifdef __DEBUG_PRINT__
  print_vint512x (" 2**396-2**256-1 ", i);
  print_vint128x ("      * 2**128-1 ", j);
  print_vint640x ("               = ", k);
#endif
  kp.x640 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  ep.x2.v1x128 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_mul512x128 3a:", kp.x2.v1x128, ep.x2.v1x128);
  rc += check_vint512   ("vec_mul512x128 3b:", kp.x2.v0x512, ep.x2.v0x512);

  i.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  i.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  i.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  i.vx3 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j     = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  k = __VEC_PWR_IMP (vec_mul512x128)(i, j);

#ifdef __DEBUG_PRINT__
  kp.x640 = k;
  print_vint512x (" 2**512-2**396-1 ", i);
  print_vint128x ("      * 2**128-1 ", j);
  print_vint640x ("               = ", k);
#endif
  kp.x640 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ep.x2.v1x128 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  rc += check_vuint128x ("vec_mul512x128 4a:", kp.x2.v1x128, ep.x2.v1x128);
  rc += check_vint512   ("vec_mul512x128 4b:", kp.x2.v0x512, ep.x2.v0x512);

  i.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx3 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j     = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  k = __VEC_PWR_IMP (vec_mul512x128)(i, j);

#ifdef __DEBUG_PRINT__
  print_vint512x ("  2**512-1 ", i);
  print_vint128x ("* 2**128-1 ", j);
  print_vint640x ("         = ", k);
#endif
  kp.x640 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  ep.x2.v1x128 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  rc += check_vuint128x ("vec_mul512x128 5a:", kp.x2.v1x128, ep.x2.v1x128);
  rc += check_vint512   ("vec_mul512x128 5b:", kp.x2.v0x512, ep.x2.v0x512);

  return (rc);
}

int
test_madd512x128 (void)
{
  __VEC_U_640 k;
  __VEC_U_512x1 kp, ep;
  __VEC_U_512 i, m;
  vui128_t j, n;
  int rc = 0;

  printf ("\ntest_madd512x128 vector multiply-add quadword, 512-bit product\n");

  i.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  i.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  i.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j     = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  n     = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  k = __VEC_PWR_IMP (vec_madd512x128a128)(i, j, n);

#ifdef __DEBUG_PRINT__
  print_vint512x (" 2**128-1   ", i);
  print_vint128x (" * 2**128-1 ", j);
  print_vint128x (" * 2**128-1 ", n);
  print_vint640x ("          = ", k);
#endif
  kp.x640 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x128 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_madd512x128a128 1a:", kp.x2.v1x128, ep.x2.v1x128);
  rc += check_vint512   ("vec_madd512x128a128 1b:", kp.x2.v0x512, ep.x2.v0x512);

  i.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  i.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  i.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j     = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  n     = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  k = __VEC_PWR_IMP (vec_madd512x128a128)(i, j, n);

#ifdef __DEBUG_PRINT__
  print_vint512x (" 2**128-1   ", i);
  print_vint128x (" * 2**128-1 ", j);
  print_vint128x (" + 2**128-1 ", n);
  print_vint640x ("          = ", k);
#endif
  kp.x640 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x128 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_madd512x128a128 2a:", kp.x2.v1x128, ep.x2.v1x128);
  rc += check_vint512   ("vec_madd512x128a128 2b:", kp.x2.v0x512, ep.x2.v0x512);

  i.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  i.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  i.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j     = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  n     = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  k = __VEC_PWR_IMP (vec_madd512x128a128)(i, j, n);

#ifdef __DEBUG_PRINT__
  print_vint512x (" 2**256-2**128-1 ", i);
  print_vint128x ("      * 2**128-1 ", j);
  print_vint128x ("      + 2**128-1 ", n);
  print_vint640x ("               = ", k);
#endif
  kp.x640 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x128 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_madd512x128a128 3a:", kp.x2.v1x128, ep.x2.v1x128);
  rc += check_vint512   ("vec_madd512x128a128 3b:", kp.x2.v0x512, ep.x2.v0x512);

  i.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx3 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j     = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  n     = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  k = __VEC_PWR_IMP (vec_madd512x128a128)(i, j, n);

#ifdef __DEBUG_PRINT__
  print_vint512x ("  2**512-1 ", i);
  print_vint128x ("* 2**128-1 ", j);
  print_vint128x ("+ 2**128-1 ", n);
  print_vint640x ("         = ", k);
#endif
  kp.x640 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x128 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  rc += check_vuint128x ("vec_madd512x128a128 4a:", kp.x2.v1x128, ep.x2.v1x128);
  rc += check_vint512   ("vec_madd512x128a128 4b:", kp.x2.v0x512, ep.x2.v0x512);

  i.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx3 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j     = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  m.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  m.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  m.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  m.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  n     = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  k = __VEC_PWR_IMP (vec_madd512x128a512)(i, j, m);

#ifdef __DEBUG_PRINT__
  print_vint512x ("  2**512-1 ", i);
  print_vint128x ("* 2**128-1 ", j);
  print_vint128x ("+ 2**128-1 ", m);
  print_vint640x ("         = ", k);
#endif
  kp.x640 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x128 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  rc += check_vuint128x ("vec_madd512x128a512 5a:", kp.x2.v1x128, ep.x2.v1x128);
  rc += check_vint512   ("vec_madd512x128a512 5b:", kp.x2.v0x512, ep.x2.v0x512);

  i.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx3 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j     = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  m.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  m.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  m.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  m.vx3 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  n     = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  k = __VEC_PWR_IMP (vec_madd512x128a512)(i, j, m);

#ifdef __DEBUG_PRINT__
  print_vint512x ("  2**512-1 ", i);
  print_vint128x ("* 2**128-1 ", j);
  print_vint128x ("+ 2**128-1 ", m);
  print_vint640x ("         = ", k);
#endif
  kp.x640 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  ep.x2.v1x128 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  rc += check_vuint128x ("vec_madd512x128a512 6a:", kp.x2.v1x128, ep.x2.v1x128);
  rc += check_vint512   ("vec_madd512x128a512 6b:", kp.x2.v0x512, ep.x2.v0x512);

  i.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx3 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j     = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  m.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  m.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  m.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  m.vx3 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  n     = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  k = __VEC_PWR_IMP (vec_madd512x128a128a512)(i, j, n, m);

#ifdef __DEBUG_PRINT__
  print_vint512x ("  2**512-1 ", i);
  print_vint128x ("* 2**128-1 ", j);
  print_vint128x ("+ 2**128-1 ", m);
  print_vint640x ("         = ", k);
#endif
  kp.x640 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  ep.x2.v1x128 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  rc += check_vuint128x ("vec_madd512x128a128a512 7a:", kp.x2.v1x128, ep.x2.v1x128);
  rc += check_vint512   ("vec_madd512x128a128a512 7b:", kp.x2.v0x512, ep.x2.v0x512);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_mul512x512_MN (void)
{
  __VEC_U_1024 k;
  __VEC_U_1024x512 ke, ep;
  __VEC_U_512 i, j;
  vui128_t *kp, *ip, *jp;
  int rc = 0;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  kp = &k.vx0;
  ip = &i.vx0;
  jp = &j.vx0;
#else
  kp = &k.vx7;
  ip = &i.vx3;
  jp = &j.vx3;
#endif

  printf ("\ntest_mul512x512_MN vector multiply quadword, 1024-bit product\n");

  i.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  i.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  i.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  __VEC_PWR_IMP (vec_mul128_byMN) (kp, ip, jp, 4, 4);

#ifdef __DEBUG_PRINT__
  kp.x1024 = k;
  print_vint512x (" 2**128-1   ", i);
  print_vint512x (" * 2**128-1 ", j);
  print_vint512x ("     = h512 ", kp.x2.v1x512);
  print_vint512x ("       l512 ", kp.x2.v0x512);
#endif
  ke.x1024 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint512 ("vec_mul512x512_MN 1a:", ke.x2.v1x512, ep.x2.v1x512);
  rc += check_vint512 ("vec_mul512x512_MN 1b:", ke.x2.v0x512, ep.x2.v0x512);

  i.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx3 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  __VEC_PWR_IMP (vec_mul128_byMN) (kp, ip, jp, 4, 4);

#ifdef __DEBUG_PRINT__
  kp.x1024 = k;
  print_vint512x (" 2**128-1   ", i);
  print_vint512x (" * 2**128-1 ", j);
  print_vint512x ("     = h512 ", kp.x2.v1x512);
  print_vint512x ("       l512 ", kp.x2.v0x512);
#endif
  ke.x1024 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  ep.x2.v1x512.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  ep.x2.v1x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint512 ("vec_mul512x512_MN 2a:", ke.x2.v1x512, ep.x2.v1x512);
  rc += check_vint512 ("vec_mul512x512_MN 2b:", ke.x2.v0x512, ep.x2.v0x512);

  i.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx3 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  __VEC_PWR_IMP (vec_mul128_byMN) (kp, ip, jp, 4, 4);

#ifdef __DEBUG_PRINT__
  kp.x1024 = k;
  print_vint512x (" 2**128-1   ", i);
  print_vint512x (" * 2**128-1 ", j);
  print_vint512x ("     = h512 ", kp.x2.v1x512);
  print_vint512x ("       l512 ", kp.x2.v0x512);
#endif
  ke.x1024 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  ep.x2.v1x512.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  ep.x2.v1x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint512 ("vec_mul512x512_MN 3a:", ke.x2.v1x512, ep.x2.v1x512);
  rc += check_vint512 ("vec_mul512x512_MN 3b:", ke.x2.v0x512, ep.x2.v0x512);

  i.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx3 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j.vx3 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  __VEC_PWR_IMP (vec_mul128_byMN) (kp, ip, jp, 4, 4);

#ifdef __DEBUG_PRINT__
  kp.x1024 = k;
  print_vint512x (" 2**128-1   ", i);
  print_vint512x (" * 2**128-1 ", j);
  print_vint512x ("     = h512 ", kp.x2.v1x512);
  print_vint512x ("       l512 ", kp.x2.v0x512);
#endif
  ke.x1024 = k;
  rc += check_vint512 ("vec_mul512x512_MN 4a:", ke.x2.v1x512, vec512_foxeasy);
  rc += check_vint512 ("vec_mul512x512_MN 4b:", ke.x2.v0x512, vec512_one);

  i = vec512_ten128th;
  j = vec512_ten128th;
  __VEC_PWR_IMP (vec_mul128_byMN) (kp, ip, jp, 4, 4);

#ifdef __DEBUG_PRINT__
  kp.x1024 = k;
  print_vint512x (" 2**128-1   ", i);
  print_vint512x (" * 2**128-1 ", j);
  print_vint512x ("     = h512 ", kp.x2.v1x512);
  print_vint512x ("       l512 ", kp.x2.v0x512);
#endif
  ke.x1024 = k;
  rc += check_vint512 ("vec_mul512x512_MN 5a:", ke.x2.v1x512, vec512_ten256_h);
  rc += check_vint512 ("vec_mul512x512_MN 5b:", ke.x2.v0x512, vec512_ten256_l);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_mul512x512 (void)
{
  __VEC_U_1024 k;
  __VEC_U_1024x512 kp, ep;
  __VEC_U_512 i, j;
  int rc = 0;

  printf ("\ntest_mul512x512 vector multiply quadword, 1024-bit product\n");

  i.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  i.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  i.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  k = __VEC_PWR_IMP (vec_mul512x512)(i, j);

#ifdef __DEBUG_PRINT__
  kp.x1024 = k;
  print_vint512x (" 2**128-1   ", i);
  print_vint512x (" * 2**128-1 ", j);
  print_vint512x ("     = h512 ", kp.x2.v1x512);
  print_vint512x ("       l512 ", kp.x2.v0x512);
#endif
  kp.x1024 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint512 ("vec_mul512x512 1a:", kp.x2.v1x512, ep.x2.v1x512);
  rc += check_vint512 ("vec_mul512x512 1b:", kp.x2.v0x512, ep.x2.v0x512);

  j.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  k = __VEC_PWR_IMP (vec_mul512x512)(i, j);

#ifdef __DEBUG_PRINT__
  kp.x1024 = k;
  print_vint512x (" 2**256-2**128-1 ", i);
  print_vint512x (" * 2**128-1      ", j);
  print_vint512x ("     = h512 ", kp.x2.v1x512);
  print_vint512x ("       l512 ", kp.x2.v0x512);
#endif
  kp.x1024 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint512 ("vec_mul512x512 2a:", kp.x2.v1x512, ep.x2.v1x512);
  rc += check_vint512 ("vec_mul512x512 2b:", kp.x2.v0x512, ep.x2.v0x512);
  k = __VEC_PWR_IMP (vec_mul512x512)(j, i);

#ifdef __DEBUG_PRINT__
  kp.x1024 = k;
  print_vint512x (" 2**256-2**128-1 ", j);
  print_vint512x (" * 2**128-1      ", i);
  print_vint512x ("     = h512 ", kp.x2.v1x512);
  print_vint512x ("       l512 ", kp.x2.v0x512);
#endif
  kp.x1024 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint512 ("vec_mul512x512 2a:", kp.x2.v1x512, ep.x2.v1x512);
  rc += check_vint512 ("vec_mul512x512 2b:", kp.x2.v0x512, ep.x2.v0x512);

  j.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  k = __VEC_PWR_IMP (vec_mul512x512)(i, j);

#ifdef __DEBUG_PRINT__
  kp.x1024 = k;
  print_vint512x (" 2**396-2**256-1 ", i);
  print_vint512x (" * 2**128-1      ", j);
  print_vint512x ("     = h512 ", kp.x2.v1x512);
  print_vint512x ("       l512 ", kp.x2.v0x512);
#endif
  kp.x1024 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  ep.x2.v1x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint512 ("vec_mul512x512 3a:", kp.x2.v1x512, ep.x2.v1x512);
  rc += check_vint512 ("vec_mul512x512 3b:", kp.x2.v0x512, ep.x2.v0x512);
  k = __VEC_PWR_IMP (vec_mul512x512)(j, i);

#ifdef __DEBUG_PRINT__
  kp.x1024 = k;
  print_vint512x (" 2**396-2**256-1 ", j);
  print_vint512x (" * 2**128-1      ", i);
  print_vint512x ("     = h512 ", kp.x2.v1x512);
  print_vint512x ("       l512 ", kp.x2.v0x512);
#endif
  kp.x1024 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  ep.x2.v1x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint512 ("vec_mul512x512 4a:", kp.x2.v1x512, ep.x2.v1x512);
  rc += check_vint512 ("vec_mul512x512 4b:", kp.x2.v0x512, ep.x2.v0x512);

  j.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx3 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  k = __VEC_PWR_IMP (vec_mul512x512)(i, j);

#ifdef __DEBUG_PRINT__
  kp.x1024 = k;
  print_vint512x (" 2**512-2**396-1 ", i);
  print_vint512x (" * 2**128-1      ", j);
  print_vint512x ("     = h512 ", kp.x2.v1x512);
  print_vint512x ("       l512 ", kp.x2.v0x512);
#endif
  kp.x1024 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ep.x2.v1x512.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  ep.x2.v1x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint512 ("vec_mul512x512 5a:", kp.x2.v1x512, ep.x2.v1x512);
  rc += check_vint512 ("vec_mul512x512 5b:", kp.x2.v0x512, ep.x2.v0x512);
  k = __VEC_PWR_IMP (vec_mul512x512)(j, i);

#ifdef __DEBUG_PRINT__
  kp.x1024 = k;
  print_vint512x (" 2**512-2**396-1 ", j);
  print_vint512x (" * 2**128-1      ", i);
  print_vint512x ("     = h512 ", kp.x2.v1x512);
  print_vint512x ("       l512 ", kp.x2.v0x512);
#endif
  kp.x1024 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ep.x2.v1x512.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  ep.x2.v1x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint512 ("vec_mul512x512 6a:", kp.x2.v1x512, ep.x2.v1x512);
  rc += check_vint512 ("vec_mul512x512 6b:", kp.x2.v0x512, ep.x2.v0x512);

  i.vx0 = (vui128_t) CONST_VINT128_W (0x000004ee, 0x2d6d415b, 0x85acef81, 0x00000000);
  i.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  i.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  i.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx0 = (vui128_t) CONST_VINT128_W (0x000004ee, 0x2d6d415b, 0x85acef81, 0x00000000);
  j.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  k = __VEC_PWR_IMP (vec_mul512x512)(i, j);

#ifdef __DEBUG_PRINT__
  kp.x1024 = k;
  print_vint512x (" 10**32   ", i);
  print_vint512x (" * 10**32 ", j);
  print_vint512x ("   = h512 ", kp.x2.v1x512);
  print_vint512x ("     l512 ", kp.x2.v0x512);
#endif
  kp.x1024 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x6e38ed64, 0xbf6a1f01, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00184f03, 0xe93ff9f4, 0xdaa797ed);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint512 ("vec_mul512x512 7a:", kp.x2.v1x512, ep.x2.v1x512);
  rc += check_vint512 ("vec_mul512x512 7b:", kp.x2.v0x512, ep.x2.v0x512);

  kp.x1024 = k;
  i = kp.x2.v0x512;
  j = kp.x2.v0x512;
  k = __VEC_PWR_IMP (vec_mul512x512)(i, j);

#ifdef __DEBUG_PRINT__
  kp.x1024 = k;
  print_vint512x (" 10**64   ", i);
  print_vint512x (" * 10**64 ", j);
  print_vint512x ("   = h512 ", kp.x2.v1x512);
  print_vint512x ("     l512 ", kp.x2.v0x512);
#endif
  kp.x1024 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0x2374e42f, 0x0f1538fd, 0x03df9909, 0x2e953e01);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0xa6337f19, 0xbccdb0da, 0xc404dc08, 0xd3cff5ec);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x0000024e, 0xe91f2603);
  ep.x2.v1x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint512 ("vec_mul512x512 8a:", kp.x2.v1x512, ep.x2.v1x512);
  rc += check_vint512 ("vec_mul512x512 8b:", kp.x2.v0x512, ep.x2.v0x512);

  kp.x1024 = k;
  i = kp.x2.v0x512;
  j = kp.x2.v0x512;
  k = __VEC_PWR_IMP (vec_mul512x512)(i, j);

#ifdef __DEBUG_PRINT__
  kp.x1024 = k;
  print_vint512x (" 10**128  ", i);
  print_vint512x (" *10**128 ", j);
  print_vint512x ("   = h512 ", kp.x2.v1x512);
  print_vint512x ("     l512 ", kp.x2.v0x512);
#endif
  kp.x1024 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0x12152f87, 0xd8d99f72, 0xbed3875b, 0x982e7c01);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0x26b2716e, 0xd595d80f, 0xcf4a6e70, 0x6bde50c6);
  ep.x2.v1x512.vx0 = (vui128_t) CONST_VINT128_W (0x63ff540e, 0x3c42d35a, 0x1d153624, 0xadc666b0);
  ep.x2.v1x512.vx1 = (vui128_t) CONST_VINT128_W (0x80dcc7f7, 0x55bc28f2, 0x65f9ef17, 0xcc5573c0);
  ep.x2.v1x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x000553f7, 0x5fdcefce, 0xf46eeddc);
  ep.x2.v1x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint512 ("vec_mul512x512 8a:", kp.x2.v1x512, ep.x2.v1x512);
  rc += check_vint512 ("vec_mul512x512 8b:", kp.x2.v0x512, ep.x2.v0x512);

  return (rc);
}


//#define __DEBUG_PRINT__ 1
int
test_mul1024x1024 (void)
{
  __VEC_U_2048x512 k, e;
  __VEC_U_1024x512 m1, m2;

  int rc = 0;

  printf ("\ntest_mul1024x1024 vector multiply quadword, 2048-bit product\n");

  m1.x2.v1x512 = vec512_zeros;
  m1.x2.v0x512 = vec512_foxes;
  m2.x2.v1x512 = vec512_zeros;
  m2.x2.v0x512 = vec512_foxes;
  __VEC_PWR_IMP (vec_mul1024x1024)(&k.x2048, &m1.x1024, &m2.x1024);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[1]   ", m1.x2.v1x512);
  print_vint512x (" m1[0] * ", m1.x2.v0x512);
  print_vint512x (" m2[1]   ", m2.x2.v1x512);
  print_vint512x (" m2[0] = ", m2.x2.v0x512);
  print_vint512x (" k [3]   ", k.x4.v3x512);
  print_vint512x (" k [2]   ", k.x4.v2x512);
  print_vint512x (" k [1]   ", k.x4.v1x512);
  print_vint512x (" k [0] = ", k.x4.v0x512);
#endif
  e.x4.v0x512 = vec512_one;
  e.x4.v1x512 = vec512_foxeasy;
  e.x4.v2x512 = vec512_zeros;
  e.x4.v3x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul1024x1024 1a:", k.x4.v3x512, e.x4.v3x512);
  rc += check_vint512 ("vec_mul1024x1024 1b:", k.x4.v2x512, e.x4.v2x512);
  rc += check_vint512 ("vec_mul1024x1024 1c:", k.x4.v1x512, e.x4.v1x512);
  rc += check_vint512 ("vec_mul1024x1024 1d:", k.x4.v0x512, e.x4.v0x512);

  m1.x2.v1x512 = vec512_foxes;
  m1.x2.v0x512 = vec512_zeros;
  m2.x2.v1x512 = vec512_zeros;
  m2.x2.v0x512 = vec512_foxes;
  __VEC_PWR_IMP (vec_mul1024x1024)(&k.x2048, &m1.x1024, &m2.x1024);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[1]   ", m1.x2.v1x512);
  print_vint512x (" m1[0] * ", m1.x2.v0x512);
  print_vint512x (" m2[1]   ", m2.x2.v1x512);
  print_vint512x (" m2[0] = ", m2.x2.v0x512);
  print_vint512x (" k [3]   ", k.x4.v3x512);
  print_vint512x (" k [2]   ", k.x4.v2x512);
  print_vint512x (" k [1]   ", k.x4.v1x512);
  print_vint512x (" k [0]   ", k.x4.v0x512);
#endif
  e.x4.v0x512 = vec512_zeros;
  e.x4.v1x512 = vec512_one;
  e.x4.v2x512 = vec512_foxeasy;
  e.x4.v3x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul1024x1024 2a:", k.x4.v3x512, e.x4.v3x512);
  rc += check_vint512 ("vec_mul1024x1024 2b:", k.x4.v2x512, e.x4.v2x512);
  rc += check_vint512 ("vec_mul1024x1024 2c:", k.x4.v1x512, e.x4.v1x512);
  rc += check_vint512 ("vec_mul1024x1024 2d:", k.x4.v0x512, e.x4.v0x512);

  __VEC_PWR_IMP (vec_mul1024x1024)(&k.x2048, &m1.x1024, &m2.x1024);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[1]   ", m2.x2.v1x512);
  print_vint512x (" m1[0] * ", m2.x2.v0x512);
  print_vint512x (" m2[1]   ", m1.x2.v1x512);
  print_vint512x (" m2[0] = ", m1.x2.v0x512);
  print_vint512x (" k [3]   ", k.x4.v3x512);
  print_vint512x (" k [2]   ", k.x4.v2x512);
  print_vint512x (" k [1]   ", k.x4.v1x512);
  print_vint512x (" k [0]   ", k.x4.v0x512);
#endif
  rc += check_vint512 ("vec_mul1024x1024 3a:", k.x4.v3x512, e.x4.v3x512);
  rc += check_vint512 ("vec_mul1024x1024 3b:", k.x4.v2x512, e.x4.v2x512);
  rc += check_vint512 ("vec_mul1024x1024 3c:", k.x4.v1x512, e.x4.v1x512);
  rc += check_vint512 ("vec_mul1024x1024 3d:", k.x4.v0x512, e.x4.v0x512);

  m1.x2.v1x512 = vec512_foxes;
  m1.x2.v0x512 = vec512_zeros;
  m2.x2.v1x512 = vec512_foxes;
  m2.x2.v0x512 = vec512_zeros;
  __VEC_PWR_IMP (vec_mul1024x1024)(&k.x2048, &m1.x1024, &m2.x1024);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[1]   ", m1.x2.v1x512);
  print_vint512x (" m1[0] * ", m1.x2.v0x512);
  print_vint512x (" m2[1]   ", m2.x2.v1x512);
  print_vint512x (" m2[0] = ", m2.x2.v0x512);
  print_vint512x (" k [3]   ", k.x4.v3x512);
  print_vint512x (" k [2]   ", k.x4.v2x512);
  print_vint512x (" k [1]   ", k.x4.v1x512);
  print_vint512x (" k [0]   ", k.x4.v0x512);
#endif
  e.x4.v0x512 = vec512_zeros;
  e.x4.v1x512 = vec512_zeros;
  e.x4.v2x512 = vec512_one;
  e.x4.v3x512 = vec512_foxeasy;
  rc += check_vint512 ("vec_mul1024x1024 4a:", k.x4.v3x512, e.x4.v3x512);
  rc += check_vint512 ("vec_mul1024x1024 4b:", k.x4.v2x512, e.x4.v2x512);
  rc += check_vint512 ("vec_mul1024x1024 4c:", k.x4.v1x512, e.x4.v1x512);
  rc += check_vint512 ("vec_mul1024x1024 4d:", k.x4.v0x512, e.x4.v0x512);

  m1.x2.v1x512 = vec512_foxes;
  m1.x2.v0x512 = vec512_foxes;
  m2.x2.v1x512 = vec512_foxes;
  m2.x2.v0x512 = vec512_foxes;
  __VEC_PWR_IMP (vec_mul1024x1024)(&k.x2048, &m1.x1024, &m2.x1024);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[1]   ", m1.x2.v1x512);
  print_vint512x (" m1[0] * ", m1.x2.v0x512);
  print_vint512x (" m2[1]   ", m2.x2.v1x512);
  print_vint512x (" m2[0] = ", m2.x2.v0x512);
  print_vint512x (" k [3]   ", k.x4.v3x512);
  print_vint512x (" k [2]   ", k.x4.v2x512);
  print_vint512x (" k [1]   ", k.x4.v1x512);
  print_vint512x (" k [0]   ", k.x4.v0x512);
#endif
  e.x4.v0x512 = vec512_one;
  e.x4.v1x512 = vec512_zeros;
  e.x4.v2x512 = vec512_foxeasy;
  e.x4.v3x512 = vec512_foxes;
  rc += check_vint512 ("vec_mul1024x1024 5a:", k.x4.v3x512, e.x4.v3x512);
  rc += check_vint512 ("vec_mul1024x1024 5b:", k.x4.v2x512, e.x4.v2x512);
  rc += check_vint512 ("vec_mul1024x1024 5c:", k.x4.v1x512, e.x4.v1x512);
  rc += check_vint512 ("vec_mul1024x1024 5d:", k.x4.v0x512, e.x4.v0x512);

  m1.x2.v1x512 = vec512_zeros;
  m1.x2.v0x512 = vec512_ten128th;
  m2.x2.v1x512 = vec512_zeros;
  m2.x2.v0x512 = vec512_ten128th;
  __VEC_PWR_IMP (vec_mul1024x1024)(&k.x2048, &m1.x1024, &m2.x1024);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[1]   ", m1.x2.v1x512);
  print_vint512x (" m1[0] * ", m1.x2.v0x512);
  print_vint512x (" m2[1]   ", m2.x2.v1x512);
  print_vint512x (" m2[0] = ", m2.x2.v0x512);
  print_vint512x (" k [3]   ", k.x4.v3x512);
  print_vint512x (" k [2]   ", k.x4.v2x512);
  print_vint512x (" k [1]   ", k.x4.v1x512);
  print_vint512x (" k [0]   ", k.x4.v0x512);
#endif
  e.x4.v0x512 = vec512_ten256_l;
  e.x4.v1x512 = vec512_ten256_h;
  e.x4.v2x512 = vec512_zeros;
  e.x4.v3x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul1024x1024 6a:", k.x4.v3x512, e.x4.v3x512);
  rc += check_vint512 ("vec_mul1024x1024 6b:", k.x4.v2x512, e.x4.v2x512);
  rc += check_vint512 ("vec_mul1024x1024 6c:", k.x4.v1x512, e.x4.v1x512);
  rc += check_vint512 ("vec_mul1024x1024 6d:", k.x4.v0x512, e.x4.v0x512);

  m1.x2.v1x512 = e.x4.v1x512;
  m1.x2.v0x512 = e.x4.v0x512;
  m2.x2.v1x512 = e.x4.v1x512;
  m2.x2.v0x512 = e.x4.v0x512;
  __VEC_PWR_IMP (vec_mul1024x1024)(&k.x2048, &m1.x1024, &m2.x1024);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[1]   ", m1.x2.v1x512);
  print_vint512x (" m1[0] * ", m1.x2.v0x512);
  print_vint512x (" m2[1]   ", m2.x2.v1x512);
  print_vint512x (" m2[0] = ", m2.x2.v0x512);
  print_vint512x (" k [3]   ", k.x4.v3x512);
  print_vint512x (" k [2]   ", k.x4.v2x512);
  print_vint512x (" k [1]   ", k.x4.v1x512);
  print_vint512x (" k [0]   ", k.x4.v0x512);
#endif
  e.x4.v0x512 = vec512_zeros;
  e.x4.v1x512 = vec512_ten512_1;
  e.x4.v2x512 = vec512_ten512_2;
  e.x4.v3x512 = vec512_ten512_3;
  rc += check_vint512 ("vec_mul1024x1024 7a:", k.x4.v3x512, e.x4.v3x512);
  rc += check_vint512 ("vec_mul1024x1024 7b:", k.x4.v2x512, e.x4.v2x512);
  rc += check_vint512 ("vec_mul1024x1024 7c:", k.x4.v1x512, e.x4.v1x512);
  rc += check_vint512 ("vec_mul1024x1024 7d:", k.x4.v0x512, e.x4.v0x512);

  m1.x2.v1x512 = vec512_zeros;
  m1.x2.v0x512 = vec512_ten128th;
  m2.x2.v1x512 = vec512_ten256_h;
  m2.x2.v0x512 = vec512_ten256_l;
  __VEC_PWR_IMP (vec_mul1024x1024)(&k.x2048, &m1.x1024, &m2.x1024);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[1]   ", m1.x2.v1x512);
  print_vint512x (" m1[0] * ", m1.x2.v0x512);
  print_vint512x (" m2[1]   ", m2.x2.v1x512);
  print_vint512x (" m2[0] = ", m2.x2.v0x512);
  print_vint512x (" k [3]   ", k.x4.v3x512);
  print_vint512x (" k [2]   ", k.x4.v2x512);
  print_vint512x (" k [1]   ", k.x4.v1x512);
  print_vint512x (" k [0]   ", k.x4.v0x512);
#endif
  e.x4.v0x512 = vec512_ten384_l;
  e.x4.v1x512 = vec512_ten384_m;
  e.x4.v2x512 = vec512_ten384_h;
  e.x4.v3x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul1024x1024 8a:", k.x4.v3x512, e.x4.v3x512);
  rc += check_vint512 ("vec_mul1024x1024 8b:", k.x4.v2x512, e.x4.v2x512);
  rc += check_vint512 ("vec_mul1024x1024 8c:", k.x4.v1x512, e.x4.v1x512);
  rc += check_vint512 ("vec_mul1024x1024 8d:", k.x4.v0x512, e.x4.v0x512);

  __VEC_PWR_IMP (vec_mul1024x1024)(&k.x2048, &m1.x1024, &m2.x1024);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[1]   ", m2.x2.v1x512);
  print_vint512x (" m1[0] * ", m2.x2.v0x512);
  print_vint512x (" m2[1]   ", m1.x2.v1x512);
  print_vint512x (" m2[0] = ", m1.x2.v0x512);
  print_vint512x (" k [3]   ", k.x4.v3x512);
  print_vint512x (" k [2]   ", k.x4.v2x512);
  print_vint512x (" k [1]   ", k.x4.v1x512);
  print_vint512x (" k [0]   ", k.x4.v0x512);
#endif
  rc += check_vint512 ("vec_mul1024x1024 9a:", k.x4.v3x512, e.x4.v3x512);
  rc += check_vint512 ("vec_mul1024x1024 9b:", k.x4.v2x512, e.x4.v2x512);
  rc += check_vint512 ("vec_mul1024x1024 9c:", k.x4.v1x512, e.x4.v1x512);
  rc += check_vint512 ("vec_mul1024x1024 9d:", k.x4.v0x512, e.x4.v0x512);

  m1.x2.v1x512 = vec512_foxes;
  m1.x2.v0x512 = vec512_foxes;
  m2.x2.v1x512 = vec512_foxes;
  m2.x2.v0x512 = vec512_zeros;
  __VEC_PWR_IMP (vec_mul1024x1024)(&k.x2048, &m1.x1024, &m2.x1024);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[1]   ", m1.x2.v1x512);
  print_vint512x (" m1[0] * ", m1.x2.v0x512);
  print_vint512x (" m2[1]   ", m2.x2.v1x512);
  print_vint512x (" m2[0] = ", m2.x2.v0x512);
  print_vint512x (" k [3]   ", k.x4.v3x512);
  print_vint512x (" k [2]   ", k.x4.v2x512);
  print_vint512x (" k [1]   ", k.x4.v1x512);
  print_vint512x (" k [0]   ", k.x4.v0x512);
#endif
  e.x4.v0x512 = vec512_zeros;
  e.x4.v1x512 = vec512_one;
  e.x4.v2x512 = vec512_foxes;
  e.x4.v3x512 = vec512_foxeasy;
  rc += check_vint512 ("vec_mul1024x1024 10a:", k.x4.v3x512, e.x4.v3x512);
  rc += check_vint512 ("vec_mul1024x1024 10b:", k.x4.v2x512, e.x4.v2x512);
  rc += check_vint512 ("vec_mul1024x1024 10c:", k.x4.v1x512, e.x4.v1x512);
  rc += check_vint512 ("vec_mul1024x1024 10d:", k.x4.v0x512, e.x4.v0x512);

  return (rc);
}
//#undef __DEBUG_PRINT__

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define __NDX16(__index) (__index)
#else
#define __NDX16(__index) ((16 - 1) - (__index))
#endif
//#define __DEBUG_PRINT__ 1
int
test_mul2048x2048_MN (void)
{
  __VEC_U_512 k1[16];
  __VEC_U_4096x512 k, e;
  __VEC_U_2048x512 m1, m2;
  __VEC_U_512 *kp, *ip, *jp;
  int rc = 0;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  kp = &k.x8.v0x512;
  ip = &m1.x4.v0x512;
  jp = &m2.x4.v0x512;
#else
  kp = &k.x8.v7x512;
  ip = &m1.x4.v3x512;
  jp = &m2.x4.v3x512;
#endif

  printf ("\ntest_mul512_MN vector multiply quadwords, 4096-bit product\n");

  m1.x4.v3x512 = vec512_zeros;
  m1.x4.v2x512 = vec512_zeros;
  m1.x4.v1x512 = vec512_zeros;
  m1.x4.v0x512 = vec512_foxes;
  m2.x4.v3x512 = vec512_zeros;
  m2.x4.v2x512 = vec512_zeros;
  m2.x4.v1x512 = vec512_zeros;
  m2.x4.v0x512 = vec512_foxes;
  __VEC_PWR_IMP (vec_mul512_byMN)(kp, ip, jp, 4, 4);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[3]   ", m1.x4.v3x512);
  print_vint512x (" m1[2]   ", m1.x4.v2x512);
  print_vint512x (" m1[1]   ", m1.x4.v1x512);
  print_vint512x (" m1[0] * ", m1.x4.v0x512);
  print_vint512x (" m2[3]   ", m2.x4.v3x512);
  print_vint512x (" m2[2]   ", m2.x4.v2x512);
  print_vint512x (" m2[1]   ", m2.x4.v1x512);
  print_vint512x (" m2[0] = ", m2.x4.v0x512);
  print_vint512x (" k [7]   ", k.x8.v7x512);
  print_vint512x (" k [6]   ", k.x8.v6x512);
  print_vint512x (" k [5]   ", k.x8.v5x512);
  print_vint512x (" k [4] = ", k.x8.v4x512);
  print_vint512x (" k [3]   ", k.x8.v3x512);
  print_vint512x (" k [2]   ", k.x8.v2x512);
  print_vint512x (" k [1]   ", k.x8.v1x512);
  print_vint512x (" k [0] = ", k.x8.v0x512);
#endif
  e.x8.v0x512 = vec512_one;
  e.x8.v1x512 = vec512_foxeasy;
  e.x8.v2x512 = vec512_zeros;
  e.x8.v3x512 = vec512_zeros;
  e.x8.v4x512 = vec512_zeros;
  e.x8.v5x512 = vec512_zeros;
  e.x8.v6x512 = vec512_zeros;
  e.x8.v7x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul512_byMN 1a:", k.x8.v7x512, e.x8.v7x512);
  rc += check_vint512 ("vec_mul512_byMN 1b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul512_byMN 1c:", k.x8.v5x512, e.x8.v5x512);
  rc += check_vint512 ("vec_mul512_byMN 1d:", k.x8.v4x512, e.x8.v4x512);
  rc += check_vint512 ("vec_mul512_byMN 1e:", k.x8.v3x512, e.x8.v3x512);
  rc += check_vint512 ("vec_mul512_byMN 1f:", k.x8.v2x512, e.x8.v2x512);
  rc += check_vint512 ("vec_mul512_byMN 1g:", k.x8.v1x512, e.x8.v1x512);
  rc += check_vint512 ("vec_mul512_byMN 1h:", k.x8.v0x512, e.x8.v0x512);

  m1.x4.v3x512 = vec512_zeros;
  m1.x4.v2x512 = vec512_zeros;
  m1.x4.v1x512 = vec512_foxes;
  m1.x4.v0x512 = vec512_zeros;
  m2.x4.v3x512 = vec512_zeros;
  m2.x4.v2x512 = vec512_zeros;
  m2.x4.v1x512 = vec512_zeros;
  m2.x4.v0x512 = vec512_foxes;
  __VEC_PWR_IMP (vec_mul512_byMN)(kp, ip, jp, 4, 4);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[3]   ", m1.x4.v3x512);
  print_vint512x (" m1[2]   ", m1.x4.v2x512);
  print_vint512x (" m1[1]   ", m1.x4.v1x512);
  print_vint512x (" m1[0] * ", m1.x4.v0x512);
  print_vint512x (" m2[3]   ", m2.x4.v3x512);
  print_vint512x (" m2[2]   ", m2.x4.v2x512);
  print_vint512x (" m2[1]   ", m2.x4.v1x512);
  print_vint512x (" m2[0] = ", m2.x4.v0x512);
  print_vint512x (" k [7]   ", k.x8.v7x512);
  print_vint512x (" k [6]   ", k.x8.v6x512);
  print_vint512x (" k [5]   ", k.x8.v5x512);
  print_vint512x (" k [4]   ", k.x8.v4x512);
  print_vint512x (" k [3]   ", k.x8.v3x512);
  print_vint512x (" k [2]   ", k.x8.v2x512);
  print_vint512x (" k [1]   ", k.x8.v1x512);
  print_vint512x (" k [0]   ", k.x8.v0x512);
#endif
  e.x8.v0x512 = vec512_zeros;
  e.x8.v1x512 = vec512_one;
  e.x8.v2x512 = vec512_foxeasy;
  e.x8.v3x512 = vec512_zeros;
  e.x8.v4x512 = vec512_zeros;
  e.x8.v5x512 = vec512_zeros;
  e.x8.v6x512 = vec512_zeros;
  e.x8.v7x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul512_byMN 2a:", k.x8.v7x512, e.x8.v7x512);
  rc += check_vint512 ("vec_mul512_byMN 2b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul512_byMN 2c:", k.x8.v5x512, e.x8.v5x512);
  rc += check_vint512 ("vec_mul512_byMN 2d:", k.x8.v4x512, e.x8.v4x512);
  rc += check_vint512 ("vec_mul512_byMN 2e:", k.x8.v3x512, e.x8.v3x512);
  rc += check_vint512 ("vec_mul512_byMN 2f:", k.x8.v2x512, e.x8.v2x512);
  rc += check_vint512 ("vec_mul512_byMN 2g:", k.x8.v1x512, e.x8.v1x512);
  rc += check_vint512 ("vec_mul512_byMN 2h:", k.x8.v0x512, e.x8.v0x512);

  __VEC_PWR_IMP (vec_mul512_byMN)(kp, ip, jp, 4, 4);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[3]   ", m2.x4.v3x512);
  print_vint512x (" m1[2]   ", m2.x4.v2x512);
  print_vint512x (" m1[1]   ", m2.x4.v1x512);
  print_vint512x (" m1[0] * ", m2.x4.v0x512);
  print_vint512x (" m2[3]   ", m1.x4.v3x512);
  print_vint512x (" m2[2]   ", m1.x4.v2x512);
  print_vint512x (" m2[1]   ", m1.x4.v1x512);
  print_vint512x (" m2[0] = ", m1.x4.v0x512);
  print_vint512x (" k [7]   ", k.x8.v7x512);
  print_vint512x (" k [6]   ", k.x8.v6x512);
  print_vint512x (" k [5]   ", k.x8.v5x512);
  print_vint512x (" k [4]   ", k.x8.v4x512);
  print_vint512x (" k [3]   ", k.x8.v3x512);
  print_vint512x (" k [2]   ", k.x8.v2x512);
  print_vint512x (" k [1]   ", k.x8.v1x512);
  print_vint512x (" k [0]   ", k.x8.v0x512);
#endif
  rc += check_vint512 ("vec_mul512_byMN 3a:", k.x8.v7x512, e.x8.v7x512);
  rc += check_vint512 ("vec_mul512_byMN 3b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul512_byMN 3c:", k.x8.v5x512, e.x8.v5x512);
  rc += check_vint512 ("vec_mul512_byMN 3d:", k.x8.v4x512, e.x8.v4x512);
  rc += check_vint512 ("vec_mul512_byMN 3e:", k.x8.v3x512, e.x8.v3x512);
  rc += check_vint512 ("vec_mul512_byMN 3f:", k.x8.v2x512, e.x8.v2x512);
  rc += check_vint512 ("vec_mul512_byMN 3g:", k.x8.v1x512, e.x8.v1x512);
  rc += check_vint512 ("vec_mul512_byMN 3h:", k.x8.v0x512, e.x8.v0x512);

  m1.x4.v3x512 = vec512_zeros;
  m1.x4.v2x512 = vec512_foxes;
  m1.x4.v1x512 = vec512_zeros;
  m1.x4.v0x512 = vec512_zeros;
  m2.x4.v3x512 = vec512_zeros;
  m2.x4.v2x512 = vec512_zeros;
  m2.x4.v1x512 = vec512_zeros;
  m2.x4.v0x512 = vec512_foxes;
  __VEC_PWR_IMP (vec_mul512_byMN)(kp, ip, jp, 4, 4);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[3]   ", m1.x4.v3x512);
  print_vint512x (" m1[2]   ", m1.x4.v2x512);
  print_vint512x (" m1[1]   ", m1.x4.v1x512);
  print_vint512x (" m1[0] * ", m1.x4.v0x512);
  print_vint512x (" m2[3]   ", m2.x4.v3x512);
  print_vint512x (" m2[2]   ", m2.x4.v2x512);
  print_vint512x (" m2[1]   ", m2.x4.v1x512);
  print_vint512x (" m2[0] = ", m2.x4.v0x512);
  print_vint512x (" k [7]   ", k.x8.v7x512);
  print_vint512x (" k [6]   ", k.x8.v6x512);
  print_vint512x (" k [5]   ", k.x8.v5x512);
  print_vint512x (" k [4]   ", k.x8.v4x512);
  print_vint512x (" k [3]   ", k.x8.v3x512);
  print_vint512x (" k [2]   ", k.x8.v2x512);
  print_vint512x (" k [1]   ", k.x8.v1x512);
  print_vint512x (" k [0]   ", k.x8.v0x512);
#endif
  e.x8.v0x512 = vec512_zeros;
  e.x8.v1x512 = vec512_zeros;
  e.x8.v2x512 = vec512_one;
  e.x8.v3x512 = vec512_foxeasy;
  e.x8.v4x512 = vec512_zeros;
  e.x8.v5x512 = vec512_zeros;
  e.x8.v6x512 = vec512_zeros;
  e.x8.v7x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul512_byMN 4a:", k.x8.v7x512, e.x8.v7x512);
  rc += check_vint512 ("vec_mul512_byMN 4b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul512_byMN 4c:", k.x8.v5x512, e.x8.v5x512);
  rc += check_vint512 ("vec_mul512_byMN 4d:", k.x8.v4x512, e.x8.v4x512);
  rc += check_vint512 ("vec_mul512_byMN 4e:", k.x8.v3x512, e.x8.v3x512);
  rc += check_vint512 ("vec_mul512_byMN 4f:", k.x8.v2x512, e.x8.v2x512);
  rc += check_vint512 ("vec_mul512_byMN 4g:", k.x8.v1x512, e.x8.v1x512);
  rc += check_vint512 ("vec_mul512_byMN 4h:", k.x8.v0x512, e.x8.v0x512);

  __VEC_PWR_IMP (vec_mul512_byMN)(kp, ip, jp, 4, 4);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[3]   ", m2.x4.v3x512);
  print_vint512x (" m1[2]   ", m2.x4.v2x512);
  print_vint512x (" m1[1]   ", m2.x4.v1x512);
  print_vint512x (" m1[0] * ", m2.x4.v0x512);
  print_vint512x (" m2[3]   ", m1.x4.v3x512);
  print_vint512x (" m2[2]   ", m1.x4.v2x512);
  print_vint512x (" m2[1]   ", m1.x4.v1x512);
  print_vint512x (" m2[0] = ", m1.x4.v0x512);
  print_vint512x (" k [7]   ", k.x8.v7x512);
  print_vint512x (" k [6]   ", k.x8.v6x512);
  print_vint512x (" k [5]   ", k.x8.v5x512);
  print_vint512x (" k [4]   ", k.x8.v4x512);
  print_vint512x (" k [3]   ", k.x8.v3x512);
  print_vint512x (" k [2]   ", k.x8.v2x512);
  print_vint512x (" k [1]   ", k.x8.v1x512);
  print_vint512x (" k [0]   ", k.x8.v0x512);
#endif
  rc += check_vint512 ("vec_mul512_byMN 5a:", k.x8.v7x512, e.x8.v7x512);
  rc += check_vint512 ("vec_mul512_byMN 5b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul512_byMN 5c:", k.x8.v5x512, e.x8.v5x512);
  rc += check_vint512 ("vec_mul512_byMN 5d:", k.x8.v4x512, e.x8.v4x512);
  rc += check_vint512 ("vec_mul512_byMN 5e:", k.x8.v3x512, e.x8.v3x512);
  rc += check_vint512 ("vec_mul512_byMN 5f:", k.x8.v2x512, e.x8.v2x512);
  rc += check_vint512 ("vec_mul512_byMN 5g:", k.x8.v1x512, e.x8.v1x512);
  rc += check_vint512 ("vec_mul512_byMN 5h:", k.x8.v0x512, e.x8.v0x512);

  m1.x4.v3x512 = vec512_foxes;
  m1.x4.v2x512 = vec512_zeros;
  m1.x4.v1x512 = vec512_zeros;
  m1.x4.v0x512 = vec512_zeros;
  m2.x4.v3x512 = vec512_zeros;
  m2.x4.v2x512 = vec512_zeros;
  m2.x4.v1x512 = vec512_zeros;
  m2.x4.v0x512 = vec512_foxes;
  __VEC_PWR_IMP (vec_mul512_byMN)(kp, ip, jp, 4, 4);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[3]   ", m1.x4.v3x512);
  print_vint512x (" m1[2]   ", m1.x4.v2x512);
  print_vint512x (" m1[1]   ", m1.x4.v1x512);
  print_vint512x (" m1[0] * ", m1.x4.v0x512);
  print_vint512x (" m2[3]   ", m2.x4.v3x512);
  print_vint512x (" m2[2]   ", m2.x4.v2x512);
  print_vint512x (" m2[1]   ", m2.x4.v1x512);
  print_vint512x (" m2[0] = ", m2.x4.v0x512);
  print_vint512x (" k [7]   ", k.x8.v7x512);
  print_vint512x (" k [6]   ", k.x8.v6x512);
  print_vint512x (" k [5]   ", k.x8.v5x512);
  print_vint512x (" k [4]   ", k.x8.v4x512);
  print_vint512x (" k [3]   ", k.x8.v3x512);
  print_vint512x (" k [2]   ", k.x8.v2x512);
  print_vint512x (" k [1]   ", k.x8.v1x512);
  print_vint512x (" k [0]   ", k.x8.v0x512);
#endif
  e.x8.v0x512 = vec512_zeros;
  e.x8.v1x512 = vec512_zeros;
  e.x8.v2x512 = vec512_zeros;
  e.x8.v3x512 = vec512_one;
  e.x8.v4x512 = vec512_foxeasy;
  e.x8.v5x512 = vec512_zeros;
  e.x8.v6x512 = vec512_zeros;
  e.x8.v7x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul512_byMN 6a:", k.x8.v7x512, e.x8.v7x512);
  rc += check_vint512 ("vec_mul512_byMN 6b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul512_byMN 6c:", k.x8.v5x512, e.x8.v5x512);
  rc += check_vint512 ("vec_mul512_byMN 6d:", k.x8.v4x512, e.x8.v4x512);
  rc += check_vint512 ("vec_mul512_byMN 6e:", k.x8.v3x512, e.x8.v3x512);
  rc += check_vint512 ("vec_mul512_byMN 6f:", k.x8.v2x512, e.x8.v2x512);
  rc += check_vint512 ("vec_mul512_byMN 6g:", k.x8.v1x512, e.x8.v1x512);
  rc += check_vint512 ("vec_mul512_byMN 6h:", k.x8.v0x512, e.x8.v0x512);

  __VEC_PWR_IMP (vec_mul512_byMN)(kp, ip, jp, 4, 4);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[3]   ", m2.x4.v3x512);
  print_vint512x (" m1[2]   ", m2.x4.v2x512);
  print_vint512x (" m1[1]   ", m2.x4.v1x512);
  print_vint512x (" m1[0] * ", m2.x4.v0x512);
  print_vint512x (" m2[3]   ", m1.x4.v3x512);
  print_vint512x (" m2[2]   ", m1.x4.v2x512);
  print_vint512x (" m2[1]   ", m1.x4.v1x512);
  print_vint512x (" m2[0] = ", m1.x4.v0x512);
  print_vint512x (" k [7]   ", k.x8.v7x512);
  print_vint512x (" k [6]   ", k.x8.v6x512);
  print_vint512x (" k [5]   ", k.x8.v5x512);
  print_vint512x (" k [4]   ", k.x8.v4x512);
  print_vint512x (" k [3]   ", k.x8.v3x512);
  print_vint512x (" k [2]   ", k.x8.v2x512);
  print_vint512x (" k [1]   ", k.x8.v1x512);
  print_vint512x (" k [0]   ", k.x8.v0x512);
#endif
  rc += check_vint512 ("vec_mul512_byMN 7a:", k.x8.v7x512, e.x8.v7x512);
  rc += check_vint512 ("vec_mul512_byMN 7b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul512_byMN 7c:", k.x8.v5x512, e.x8.v5x512);
  rc += check_vint512 ("vec_mul512_byMN 7d:", k.x8.v4x512, e.x8.v4x512);
  rc += check_vint512 ("vec_mul512_byMN 7e:", k.x8.v3x512, e.x8.v3x512);
  rc += check_vint512 ("vec_mul512_byMN 7f:", k.x8.v2x512, e.x8.v2x512);
  rc += check_vint512 ("vec_mul512_byMN 7g:", k.x8.v1x512, e.x8.v1x512);
  rc += check_vint512 ("vec_mul512_byMN 7h:", k.x8.v0x512, e.x8.v0x512);

  m1.x4.v3x512 = vec512_foxes;
  m1.x4.v2x512 = vec512_foxes;
  m1.x4.v1x512 = vec512_foxes;
  m1.x4.v0x512 = vec512_foxes;
  m2.x4.v3x512 = vec512_foxes;
  m2.x4.v2x512 = vec512_foxes;
  m2.x4.v1x512 = vec512_foxes;
  m2.x4.v0x512 = vec512_foxes;
  __VEC_PWR_IMP (vec_mul512_byMN)(kp, ip, jp, 4, 4);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[3]   ", m1.x4.v3x512);
  print_vint512x (" m1[2]   ", m1.x4.v2x512);
  print_vint512x (" m1[1]   ", m1.x4.v1x512);
  print_vint512x (" m1[0] * ", m1.x4.v0x512);
  print_vint512x (" m2[3]   ", m2.x4.v3x512);
  print_vint512x (" m2[2]   ", m2.x4.v2x512);
  print_vint512x (" m2[1]   ", m2.x4.v1x512);
  print_vint512x (" m2[0] = ", m2.x4.v0x512);
  print_vint512x (" k [7]   ", k.x8.v7x512);
  print_vint512x (" k [6]   ", k.x8.v6x512);
  print_vint512x (" k [5]   ", k.x8.v5x512);
  print_vint512x (" k [4]   ", k.x8.v4x512);
  print_vint512x (" k [3]   ", k.x8.v3x512);
  print_vint512x (" k [2]   ", k.x8.v2x512);
  print_vint512x (" k [1]   ", k.x8.v1x512);
  print_vint512x (" k [0]   ", k.x8.v0x512);
#endif
  e.x8.v0x512 = vec512_one;
  e.x8.v1x512 = vec512_zeros;
  e.x8.v2x512 = vec512_zeros;
  e.x8.v3x512 = vec512_zeros;
  e.x8.v4x512 = vec512_foxeasy;
  e.x8.v5x512 = vec512_foxes;
  e.x8.v6x512 = vec512_foxes;
  e.x8.v7x512 = vec512_foxes;
  rc += check_vint512 ("vec_mul512_byMN 8a:", k.x8.v7x512, e.x8.v7x512);
  rc += check_vint512 ("vec_mul512_byMN 8b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul512_byMN 8c:", k.x8.v5x512, e.x8.v5x512);
  rc += check_vint512 ("vec_mul512_byMN 8d:", k.x8.v4x512, e.x8.v4x512);
  rc += check_vint512 ("vec_mul512_byMN 8e:", k.x8.v3x512, e.x8.v3x512);
  rc += check_vint512 ("vec_mul512_byMN 8f:", k.x8.v2x512, e.x8.v2x512);
  rc += check_vint512 ("vec_mul512_byMN 8g:", k.x8.v1x512, e.x8.v1x512);
  rc += check_vint512 ("vec_mul512_byMN 8h:", k.x8.v0x512, e.x8.v0x512);

  m1.x4.v3x512 = vec512_zeros;
  m1.x4.v2x512 = vec512_zeros;
  m1.x4.v1x512 = vec512_foxes;
  m1.x4.v0x512 = vec512_foxes;
  m2.x4.v3x512 = vec512_zeros;
  m2.x4.v2x512 = vec512_zeros;
  m2.x4.v1x512 = vec512_foxes;
  m2.x4.v0x512 = vec512_foxes;
  __VEC_PWR_IMP (vec_mul512_byMN)(kp, ip, jp, 4, 4);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[3]   ", m1.x4.v3x512);
  print_vint512x (" m1[2]   ", m1.x4.v2x512);
  print_vint512x (" m1[1]   ", m1.x4.v1x512);
  print_vint512x (" m1[0] * ", m1.x4.v0x512);
  print_vint512x (" m2[3]   ", m2.x4.v3x512);
  print_vint512x (" m2[2]   ", m2.x4.v2x512);
  print_vint512x (" m2[1]   ", m2.x4.v1x512);
  print_vint512x (" m2[0] = ", m2.x4.v0x512);
  print_vint512x (" k [7]   ", k.x8.v7x512);
  print_vint512x (" k [6]   ", k.x8.v6x512);
  print_vint512x (" k [5]   ", k.x8.v5x512);
  print_vint512x (" k [4]   ", k.x8.v4x512);
  print_vint512x (" k [3]   ", k.x8.v3x512);
  print_vint512x (" k [2]   ", k.x8.v2x512);
  print_vint512x (" k [1]   ", k.x8.v1x512);
  print_vint512x (" k [0]   ", k.x8.v0x512);
#endif
  e.x8.v0x512 = vec512_one;
  e.x8.v1x512 = vec512_zeros;
  e.x8.v2x512 = vec512_foxeasy;
  e.x8.v3x512 = vec512_foxes;
  e.x8.v4x512 = vec512_zeros;
  e.x8.v5x512 = vec512_zeros;
  e.x8.v6x512 = vec512_zeros;
  e.x8.v7x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul512_byMN 9a:", k.x8.v7x512, e.x8.v7x512);
  rc += check_vint512 ("vec_mul512_byMN 9b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul512_byMN 9c:", k.x8.v5x512, e.x8.v5x512);
  rc += check_vint512 ("vec_mul512_byMN 9d:", k.x8.v4x512, e.x8.v4x512);
  rc += check_vint512 ("vec_mul512_byMN 9e:", k.x8.v3x512, e.x8.v3x512);
  rc += check_vint512 ("vec_mul512_byMN 9f:", k.x8.v2x512, e.x8.v2x512);
  rc += check_vint512 ("vec_mul512_byMN 9g:", k.x8.v1x512, e.x8.v1x512);
  rc += check_vint512 ("vec_mul512_byMN 9h:", k.x8.v0x512, e.x8.v0x512);

  m1.x4.v3x512 = vec512_foxes;
  m1.x4.v2x512 = vec512_foxes;
  m1.x4.v1x512 = vec512_foxes;
  m1.x4.v0x512 = vec512_foxes;
  m2.x4.v3x512 = vec512_zeros;
  m2.x4.v2x512 = vec512_zeros;
  m2.x4.v1x512 = vec512_zeros;
  m2.x4.v0x512 = vec512_foxes;
  __VEC_PWR_IMP (vec_mul512_byMN)(kp, ip, jp, 4, 4);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[3]   ", m1.x4.v3x512);
  print_vint512x (" m1[2]   ", m1.x4.v2x512);
  print_vint512x (" m1[1]   ", m1.x4.v1x512);
  print_vint512x (" m1[0] * ", m1.x4.v0x512);
  print_vint512x (" m2[3]   ", m2.x4.v3x512);
  print_vint512x (" m2[2]   ", m2.x4.v2x512);
  print_vint512x (" m2[1]   ", m2.x4.v1x512);
  print_vint512x (" m2[0] = ", m2.x4.v0x512);
  print_vint512x (" k [7]   ", k.x8.v7x512);
  print_vint512x (" k [6]   ", k.x8.v6x512);
  print_vint512x (" k [5]   ", k.x8.v5x512);
  print_vint512x (" k [4]   ", k.x8.v4x512);
  print_vint512x (" k [3]   ", k.x8.v3x512);
  print_vint512x (" k [2]   ", k.x8.v2x512);
  print_vint512x (" k [1]   ", k.x8.v1x512);
  print_vint512x (" k [0]   ", k.x8.v0x512);
#endif
  e.x8.v0x512 = vec512_one;
  e.x8.v1x512 = vec512_foxes;
  e.x8.v2x512 = vec512_foxes;
  e.x8.v3x512 = vec512_foxes;
  e.x8.v4x512 = vec512_foxeasy;
  e.x8.v5x512 = vec512_zeros;
  e.x8.v6x512 = vec512_zeros;
  e.x8.v7x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul512_byMN 10a:", k.x8.v7x512, e.x8.v7x512);
  rc += check_vint512 ("vec_mul512_byMN 10b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul512_byMN 10c:", k.x8.v5x512, e.x8.v5x512);
  rc += check_vint512 ("vec_mul512_byMN 10d:", k.x8.v4x512, e.x8.v4x512);
  rc += check_vint512 ("vec_mul512_byMN 10e:", k.x8.v3x512, e.x8.v3x512);
  rc += check_vint512 ("vec_mul512_byMN 10f:", k.x8.v2x512, e.x8.v2x512);
  rc += check_vint512 ("vec_mul512_byMN 10g:", k.x8.v1x512, e.x8.v1x512);
  rc += check_vint512 ("vec_mul512_byMN 10h:", k.x8.v0x512, e.x8.v0x512);

  m1.x4.v3x512 = vec512_zeros;
  m1.x4.v2x512 = vec512_zeros;
  m1.x4.v1x512 = vec512_zeros;
  m1.x4.v0x512 = vec512_ten128th;
  m2.x4.v3x512 = vec512_zeros;
  m2.x4.v2x512 = vec512_zeros;
  m2.x4.v1x512 = vec512_zeros;
  m2.x4.v0x512 = vec512_ten128th;
  __VEC_PWR_IMP (vec_mul512_byMN)(kp, ip, jp, 4, 4);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[3]   ", m1.x4.v3x512);
  print_vint512x (" m1[2]   ", m1.x4.v2x512);
  print_vint512x (" m1[1]   ", m1.x4.v1x512);
  print_vint512x (" m1[0] * ", m1.x4.v0x512);
  print_vint512x (" m2[3]   ", m2.x4.v3x512);
  print_vint512x (" m2[2]   ", m2.x4.v2x512);
  print_vint512x (" m2[1]   ", m2.x4.v1x512);
  print_vint512x (" m2[0] = ", m2.x4.v0x512);
  print_vint512x (" k [7]   ", k.x8.v7x512);
  print_vint512x (" k [6]   ", k.x8.v6x512);
  print_vint512x (" k [5]   ", k.x8.v5x512);
  print_vint512x (" k [4]   ", k.x8.v4x512);
  print_vint512x (" k [3]   ", k.x8.v3x512);
  print_vint512x (" k [2]   ", k.x8.v2x512);
  print_vint512x (" k [1]   ", k.x8.v1x512);
  print_vint512x (" k [0]   ", k.x8.v0x512);
#endif
  e.x8.v0x512 = vec512_ten256_l;
  e.x8.v1x512 = vec512_ten256_h;
  e.x8.v2x512 = vec512_zeros;
  e.x8.v3x512 = vec512_zeros;
  e.x8.v4x512 = vec512_zeros;
  e.x8.v5x512 = vec512_zeros;
  e.x8.v6x512 = vec512_zeros;
  e.x8.v7x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul512_byMN 11a:", k.x8.v7x512, vec512_zeros);
  rc += check_vint512 ("vec_mul512_byMN 11b:", k.x8.v6x512, vec512_zeros);
  rc += check_vint512 ("vec_mul512_byMN 11c:", k.x8.v5x512, vec512_zeros);
  rc += check_vint512 ("vec_mul512_byMN 11d:", k.x8.v4x512, vec512_zeros);
  rc += check_vint512 ("vec_mul512_byMN 11e:", k.x8.v3x512, vec512_zeros);
  rc += check_vint512 ("vec_mul512_byMN 11f:", k.x8.v2x512, vec512_zeros);
  rc += check_vint512 ("vec_mul512_byMN 11g:", k.x8.v1x512, vec512_ten256_h);
  rc += check_vint512 ("vec_mul512_byMN 11h:", k.x8.v0x512, vec512_ten256_l);

  m1.x4.v3x512 = vec512_zeros;
  m1.x4.v2x512 = vec512_zeros;
  m1.x4.v1x512 = e.x8.v1x512;
  m1.x4.v0x512 = e.x8.v0x512;
  m2.x4.v3x512 = vec512_zeros;
  m2.x4.v2x512 = vec512_zeros;
  m2.x4.v1x512 = e.x8.v1x512;
  m2.x4.v0x512 = e.x8.v0x512;
  __VEC_PWR_IMP (vec_mul512_byMN)(kp, ip, jp, 4, 4);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[3]   ", m1.x4.v3x512);
  print_vint512x (" m1[2]   ", m1.x4.v2x512);
  print_vint512x (" m1[1]   ", m1.x4.v1x512);
  print_vint512x (" m1[0] * ", m1.x4.v0x512);
  print_vint512x (" m2[3]   ", m2.x4.v3x512);
  print_vint512x (" m2[2]   ", m2.x4.v2x512);
  print_vint512x (" m2[1]   ", m2.x4.v1x512);
  print_vint512x (" m2[0] = ", m2.x4.v0x512);
  print_vint512x (" k [7]   ", k.x8.v7x512);
  print_vint512x (" k [6]   ", k.x8.v6x512);
  print_vint512x (" k [5]   ", k.x8.v5x512);
  print_vint512x (" k [4]   ", k.x8.v4x512);
  print_vint512x (" k [3]   ", k.x8.v3x512);
  print_vint512x (" k [2]   ", k.x8.v2x512);
  print_vint512x (" k [1]   ", k.x8.v1x512);
  print_vint512x (" k [0]   ", k.x8.v0x512);
#endif
  rc += check_vint512 ("vec_mul512_byMN 12a:", k.x8.v7x512, vec512_zeros);
  rc += check_vint512 ("vec_mul512_byMN 12b:", k.x8.v6x512, vec512_zeros);
  rc += check_vint512 ("vec_mul512_byMN 12c:", k.x8.v5x512, vec512_zeros);
  rc += check_vint512 ("vec_mul512_byMN 12d:", k.x8.v4x512, vec512_zeros);
  rc += check_vint512 ("vec_mul512_byMN 12e:", k.x8.v3x512, vec512_ten512_3);
  rc += check_vint512 ("vec_mul512_byMN 12f:", k.x8.v2x512, vec512_ten512_2);
  rc += check_vint512 ("vec_mul512_byMN 12g:", k.x8.v1x512, vec512_ten512_1);
  rc += check_vint512 ("vec_mul512_byMN 12h:", k.x8.v0x512, vec512_zeros);

  m1.x4.v3x512 = vec512_ten512_3;
  m1.x4.v2x512 = vec512_ten512_2;
  m1.x4.v1x512 = vec512_ten512_1;
  m1.x4.v0x512 = vec512_zeros;
  m2.x4.v3x512 = vec512_ten512_3;
  m2.x4.v2x512 = vec512_ten512_2;
  m2.x4.v1x512 = vec512_ten512_1;
  m2.x4.v0x512 = vec512_zeros;
  __VEC_PWR_IMP (vec_mul512_byMN)(kp, ip, jp, 4, 4);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[3]   ", m1.x4.v3x512);
  print_vint512x (" m1[2]   ", m1.x4.v2x512);
  print_vint512x (" m1[1]   ", m1.x4.v1x512);
  print_vint512x (" m1[0] * ", m1.x4.v0x512);
  print_vint512x (" m2[3]   ", m2.x4.v3x512);
  print_vint512x (" m2[2]   ", m2.x4.v2x512);
  print_vint512x (" m2[1]   ", m2.x4.v1x512);
  print_vint512x (" m2[0] = ", m2.x4.v0x512);
  print_vint512x (" k [7]   ", k.x8.v7x512);
  print_vint512x (" k [6]   ", k.x8.v6x512);
  print_vint512x (" k [5]   ", k.x8.v5x512);
  print_vint512x (" k [4]   ", k.x8.v4x512);
  print_vint512x (" k [3]   ", k.x8.v3x512);
  print_vint512x (" k [2]   ", k.x8.v2x512);
  print_vint512x (" k [1]   ", k.x8.v1x512);
  print_vint512x (" k [0]   ", k.x8.v0x512);
#endif
  rc += check_vint512 ("vec_mul512_byMN 13a:", k.x8.v7x512, vec512_zeros);
  rc += check_vint512 ("vec_mul512_byMN 13b:", k.x8.v6x512, vec512_ten1024_6);
  rc += check_vint512 ("vec_mul512_byMN 13c:", k.x8.v5x512, vec512_ten1024_5);
  rc += check_vint512 ("vec_mul512_byMN 13d:", k.x8.v4x512, vec512_ten1024_4);
  rc += check_vint512 ("vec_mul512_byMN 13e:", k.x8.v3x512, vec512_ten1024_3);
  rc += check_vint512 ("vec_mul512_byMN 13f:", k.x8.v2x512, vec512_ten1024_2);
  rc += check_vint512 ("vec_mul512_byMN 13g:", k.x8.v1x512, vec512_zeros);
  rc += check_vint512 ("vec_mul512_byMN 13h:", k.x8.v0x512, vec512_zeros);


  __VEC_PWR_IMP (vec_mul512_byMN)(k1, kp, kp, 8, 8);

#ifdef __DEBUG_PRINT__
  print_vint512x (" k [7]^2 ", k.x8.v7x512);
  print_vint512x (" k [6]   ", k.x8.v6x512);
  print_vint512x (" k [5]   ", k.x8.v5x512);
  print_vint512x (" k [4]   ", k.x8.v4x512);
  print_vint512x (" k [3]   ", k.x8.v3x512);
  print_vint512x (" k [2]   ", k.x8.v2x512);
  print_vint512x (" k [1]   ", k.x8.v1x512);
  print_vint512x (" k [0] = ", k.x8.v0x512);
  print_vint512x (" k1 [15] ", k1[15]);
  print_vint512x (" k1 [14] ", k1[14]);
  print_vint512x (" k1 [13] ", k1[13]);
  print_vint512x (" k1 [12] ", k1[12]);
  print_vint512x (" k1 [11] ", k1[11]);
  print_vint512x (" k1 [10] ", k1[10]);
  print_vint512x (" k1 [9]  ", k1[9]);
  print_vint512x (" k1 [8]  ", k1[8]);
  print_vint512x (" k1 [7]  ", k1[7]);
  print_vint512x (" k1 [6]  ", k1[6]);
  print_vint512x (" k1 [5]  ", k1[5]);
  print_vint512x (" k1 [4]  ", k1[4]);
  print_vint512x (" k1 [3]  ", k1[3]);
  print_vint512x (" k1 [2]  ", k1[2]);
  print_vint512x (" k1 [1]  ", k1[1]);
  print_vint512x (" k1 [0]  ", k1[0]);
#endif

  rc += check_vint512 ("vec_mulx4096_MN 14a:", k1[__NDX16(15)], vec512_zeros);
  rc += check_vint512 ("vec_mulx4096_MN 14b:", k1[__NDX16(14)], vec512_zeros);
  rc += check_vint512 ("vec_mulx4096_MN 14c:", k1[__NDX16(13)], vec512_ten2048_13);
  rc += check_vint512 ("vec_mulx4096_MN 14d:", k1[__NDX16(12)], vec512_ten2048_12);
  rc += check_vint512 ("vec_mulx4096_MN 14e:", k1[__NDX16(11)], vec512_ten2048_11);
  rc += check_vint512 ("vec_mulx4096_MN 14f:", k1[__NDX16(10)], vec512_ten2048_10);
  rc += check_vint512 ("vec_mulx4096_MN 14g:", k1[__NDX16(9)], vec512_ten2048_9);
  rc += check_vint512 ("vec_mulx4096_MN 14h:", k1[__NDX16(8)], vec512_ten2048_8);
  rc += check_vint512 ("vec_mulx4096_MN 14i:", k1[__NDX16(7)], vec512_ten2048_7);
  rc += check_vint512 ("vec_mulx4096_MN 14j:", k1[__NDX16(6)], vec512_ten2048_6);
  rc += check_vint512 ("vec_mulx4096_MN 14k:", k1[__NDX16(5)], vec512_ten2048_5);
  rc += check_vint512 ("vec_mulx4096_MN 14l:", k1[__NDX16(4)], vec512_ten2048_4);
  rc += check_vint512 ("vec_mulx4096_MN 14m:", k1[__NDX16(3)], vec512_ten2048_3);
  rc += check_vint512 ("vec_mulx4096_MN 14n:", k1[__NDX16(2)], vec512_ten2048_2);
  rc += check_vint512 ("vec_mulx4096_MN 14o:", k1[__NDX16(1)], vec512_zeros);
  rc += check_vint512 ("vec_mulx4096_MN 14p:", k1[__NDX16(0)], vec512_zeros);

  return (rc);
}
//#undef __DEBUG_PRINT__

//#define __DEBUG_PRINT__ 1
int
test_mul2048x2048 (void)
{
  __VEC_U_4096x512 k, e;
  __VEC_U_2048x512 m1, m2;

  int rc = 0;

  printf ("\ntest_mul2048x2048 vector multiply quadword, 1024-bit product\n");

  m1.x4.v3x512 = vec512_zeros;
  m1.x4.v2x512 = vec512_zeros;
  m1.x4.v1x512 = vec512_zeros;
  m1.x4.v0x512 = vec512_foxes;
  m2.x4.v3x512 = vec512_zeros;
  m2.x4.v2x512 = vec512_zeros;
  m2.x4.v1x512 = vec512_zeros;
  m2.x4.v0x512 = vec512_foxes;
  __VEC_PWR_IMP (vec_mul2048x2048)(&k.x4096, &m1.x2048, &m2.x2048);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[3]   ", m1.x4.v3x512);
  print_vint512x (" m1[2]   ", m1.x4.v2x512);
  print_vint512x (" m1[1]   ", m1.x4.v1x512);
  print_vint512x (" m1[0] * ", m1.x4.v0x512);
  print_vint512x (" m2[3]   ", m2.x4.v3x512);
  print_vint512x (" m2[2]   ", m2.x4.v2x512);
  print_vint512x (" m2[1]   ", m2.x4.v1x512);
  print_vint512x (" m2[0] = ", m2.x4.v0x512);
  print_vint512x (" k [7]   ", k.x8.v7x512);
  print_vint512x (" k [6]   ", k.x8.v6x512);
  print_vint512x (" k [5]   ", k.x8.v5x512);
  print_vint512x (" k [4] = ", k.x8.v4x512);
  print_vint512x (" k [3]   ", k.x8.v3x512);
  print_vint512x (" k [2]   ", k.x8.v2x512);
  print_vint512x (" k [1]   ", k.x8.v1x512);
  print_vint512x (" k [0] = ", k.x8.v0x512);
#endif
  e.x8.v0x512 = vec512_one;
  e.x8.v1x512 = vec512_foxeasy;
  e.x8.v2x512 = vec512_zeros;
  e.x8.v3x512 = vec512_zeros;
  e.x8.v4x512 = vec512_zeros;
  e.x8.v5x512 = vec512_zeros;
  e.x8.v6x512 = vec512_zeros;
  e.x8.v7x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul2048x2048 1a:", k.x8.v7x512, e.x8.v7x512);
  rc += check_vint512 ("vec_mul2048x2048 1b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul2048x2048 1c:", k.x8.v5x512, e.x8.v5x512);
  rc += check_vint512 ("vec_mul2048x2048 1d:", k.x8.v4x512, e.x8.v4x512);
  rc += check_vint512 ("vec_mul2048x2048 1e:", k.x8.v3x512, e.x8.v3x512);
  rc += check_vint512 ("vec_mul2048x2048 1f:", k.x8.v2x512, e.x8.v2x512);
  rc += check_vint512 ("vec_mul2048x2048 1g:", k.x8.v1x512, e.x8.v1x512);
  rc += check_vint512 ("vec_mul2048x2048 1h:", k.x8.v0x512, e.x8.v0x512);

  m1.x4.v3x512 = vec512_zeros;
  m1.x4.v2x512 = vec512_zeros;
  m1.x4.v1x512 = vec512_foxes;
  m1.x4.v0x512 = vec512_zeros;
  m2.x4.v3x512 = vec512_zeros;
  m2.x4.v2x512 = vec512_zeros;
  m2.x4.v1x512 = vec512_zeros;
  m2.x4.v0x512 = vec512_foxes;
  __VEC_PWR_IMP (vec_mul2048x2048)(&k.x4096, &m1.x2048, &m2.x2048);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[3]   ", m1.x4.v3x512);
  print_vint512x (" m1[2]   ", m1.x4.v2x512);
  print_vint512x (" m1[1]   ", m1.x4.v1x512);
  print_vint512x (" m1[0] * ", m1.x4.v0x512);
  print_vint512x (" m2[3]   ", m2.x4.v3x512);
  print_vint512x (" m2[2]   ", m2.x4.v2x512);
  print_vint512x (" m2[1]   ", m2.x4.v1x512);
  print_vint512x (" m2[0] = ", m2.x4.v0x512);
  print_vint512x (" k [7]   ", k.x8.v7x512);
  print_vint512x (" k [6]   ", k.x8.v6x512);
  print_vint512x (" k [5]   ", k.x8.v5x512);
  print_vint512x (" k [4]   ", k.x8.v4x512);
  print_vint512x (" k [3]   ", k.x8.v3x512);
  print_vint512x (" k [2]   ", k.x8.v2x512);
  print_vint512x (" k [1]   ", k.x8.v1x512);
  print_vint512x (" k [0]   ", k.x8.v0x512);
#endif
  e.x8.v0x512 = vec512_zeros;
  e.x8.v1x512 = vec512_one;
  e.x8.v2x512 = vec512_foxeasy;
  e.x8.v3x512 = vec512_zeros;
  e.x8.v4x512 = vec512_zeros;
  e.x8.v5x512 = vec512_zeros;
  e.x8.v6x512 = vec512_zeros;
  e.x8.v7x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul2048x2048 2a:", k.x8.v7x512, e.x8.v7x512);
  rc += check_vint512 ("vec_mul2048x2048 2b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul2048x2048 2c:", k.x8.v5x512, e.x8.v5x512);
  rc += check_vint512 ("vec_mul2048x2048 2d:", k.x8.v4x512, e.x8.v4x512);
  rc += check_vint512 ("vec_mul2048x2048 2e:", k.x8.v3x512, e.x8.v3x512);
  rc += check_vint512 ("vec_mul2048x2048 2f:", k.x8.v2x512, e.x8.v2x512);
  rc += check_vint512 ("vec_mul2048x2048 2g:", k.x8.v1x512, e.x8.v1x512);
  rc += check_vint512 ("vec_mul2048x2048 2h:", k.x8.v0x512, e.x8.v0x512);

  __VEC_PWR_IMP (vec_mul2048x2048)(&k.x4096, &m1.x2048, &m2.x2048);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[3]   ", m2.x4.v3x512);
  print_vint512x (" m1[2]   ", m2.x4.v2x512);
  print_vint512x (" m1[1]   ", m2.x4.v1x512);
  print_vint512x (" m1[0] * ", m2.x4.v0x512);
  print_vint512x (" m2[3]   ", m1.x4.v3x512);
  print_vint512x (" m2[2]   ", m1.x4.v2x512);
  print_vint512x (" m2[1]   ", m1.x4.v1x512);
  print_vint512x (" m2[0] = ", m1.x4.v0x512);
  print_vint512x (" k [7]   ", k.x8.v7x512);
  print_vint512x (" k [6]   ", k.x8.v6x512);
  print_vint512x (" k [5]   ", k.x8.v5x512);
  print_vint512x (" k [4]   ", k.x8.v4x512);
  print_vint512x (" k [3]   ", k.x8.v3x512);
  print_vint512x (" k [2]   ", k.x8.v2x512);
  print_vint512x (" k [1]   ", k.x8.v1x512);
  print_vint512x (" k [0]   ", k.x8.v0x512);
#endif
  rc += check_vint512 ("vec_mul2048x2048 3a:", k.x8.v7x512, e.x8.v7x512);
  rc += check_vint512 ("vec_mul2048x2048 3b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul2048x2048 3c:", k.x8.v5x512, e.x8.v5x512);
  rc += check_vint512 ("vec_mul2048x2048 3d:", k.x8.v4x512, e.x8.v4x512);
  rc += check_vint512 ("vec_mul2048x2048 3e:", k.x8.v3x512, e.x8.v3x512);
  rc += check_vint512 ("vec_mul2048x2048 3f:", k.x8.v2x512, e.x8.v2x512);
  rc += check_vint512 ("vec_mul2048x2048 3g:", k.x8.v1x512, e.x8.v1x512);
  rc += check_vint512 ("vec_mul2048x2048 3h:", k.x8.v0x512, e.x8.v0x512);

  m1.x4.v3x512 = vec512_zeros;
  m1.x4.v2x512 = vec512_foxes;
  m1.x4.v1x512 = vec512_zeros;
  m1.x4.v0x512 = vec512_zeros;
  m2.x4.v3x512 = vec512_zeros;
  m2.x4.v2x512 = vec512_zeros;
  m2.x4.v1x512 = vec512_zeros;
  m2.x4.v0x512 = vec512_foxes;
  __VEC_PWR_IMP (vec_mul2048x2048)(&k.x4096, &m1.x2048, &m2.x2048);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[3]   ", m1.x4.v3x512);
  print_vint512x (" m1[2]   ", m1.x4.v2x512);
  print_vint512x (" m1[1]   ", m1.x4.v1x512);
  print_vint512x (" m1[0] * ", m1.x4.v0x512);
  print_vint512x (" m2[3]   ", m2.x4.v3x512);
  print_vint512x (" m2[2]   ", m2.x4.v2x512);
  print_vint512x (" m2[1]   ", m2.x4.v1x512);
  print_vint512x (" m2[0] = ", m2.x4.v0x512);
  print_vint512x (" k [7]   ", k.x8.v7x512);
  print_vint512x (" k [6]   ", k.x8.v6x512);
  print_vint512x (" k [5]   ", k.x8.v5x512);
  print_vint512x (" k [4]   ", k.x8.v4x512);
  print_vint512x (" k [3]   ", k.x8.v3x512);
  print_vint512x (" k [2]   ", k.x8.v2x512);
  print_vint512x (" k [1]   ", k.x8.v1x512);
  print_vint512x (" k [0]   ", k.x8.v0x512);
#endif
  e.x8.v0x512 = vec512_zeros;
  e.x8.v1x512 = vec512_zeros;
  e.x8.v2x512 = vec512_one;
  e.x8.v3x512 = vec512_foxeasy;
  e.x8.v4x512 = vec512_zeros;
  e.x8.v5x512 = vec512_zeros;
  e.x8.v6x512 = vec512_zeros;
  e.x8.v7x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul2048x2048 4a:", k.x8.v7x512, e.x8.v7x512);
  rc += check_vint512 ("vec_mul2048x2048 4b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul2048x2048 4c:", k.x8.v5x512, e.x8.v5x512);
  rc += check_vint512 ("vec_mul2048x2048 4d:", k.x8.v4x512, e.x8.v4x512);
  rc += check_vint512 ("vec_mul2048x2048 4e:", k.x8.v3x512, e.x8.v3x512);
  rc += check_vint512 ("vec_mul2048x2048 4f:", k.x8.v2x512, e.x8.v2x512);
  rc += check_vint512 ("vec_mul2048x2048 4g:", k.x8.v1x512, e.x8.v1x512);
  rc += check_vint512 ("vec_mul2048x2048 4h:", k.x8.v0x512, e.x8.v0x512);

  __VEC_PWR_IMP (vec_mul2048x2048)(&k.x4096, &m1.x2048, &m2.x2048);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[3]   ", m2.x4.v3x512);
  print_vint512x (" m1[2]   ", m2.x4.v2x512);
  print_vint512x (" m1[1]   ", m2.x4.v1x512);
  print_vint512x (" m1[0] * ", m2.x4.v0x512);
  print_vint512x (" m2[3]   ", m1.x4.v3x512);
  print_vint512x (" m2[2]   ", m1.x4.v2x512);
  print_vint512x (" m2[1]   ", m1.x4.v1x512);
  print_vint512x (" m2[0] = ", m1.x4.v0x512);
  print_vint512x (" k [7]   ", k.x8.v7x512);
  print_vint512x (" k [6]   ", k.x8.v6x512);
  print_vint512x (" k [5]   ", k.x8.v5x512);
  print_vint512x (" k [4]   ", k.x8.v4x512);
  print_vint512x (" k [3]   ", k.x8.v3x512);
  print_vint512x (" k [2]   ", k.x8.v2x512);
  print_vint512x (" k [1]   ", k.x8.v1x512);
  print_vint512x (" k [0]   ", k.x8.v0x512);
#endif
  rc += check_vint512 ("vec_mul2048x2048 5a:", k.x8.v7x512, e.x8.v7x512);
  rc += check_vint512 ("vec_mul2048x2048 5b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul2048x2048 5c:", k.x8.v5x512, e.x8.v5x512);
  rc += check_vint512 ("vec_mul2048x2048 5d:", k.x8.v4x512, e.x8.v4x512);
  rc += check_vint512 ("vec_mul2048x2048 5e:", k.x8.v3x512, e.x8.v3x512);
  rc += check_vint512 ("vec_mul2048x2048 5f:", k.x8.v2x512, e.x8.v2x512);
  rc += check_vint512 ("vec_mul2048x2048 5g:", k.x8.v1x512, e.x8.v1x512);
  rc += check_vint512 ("vec_mul2048x2048 5h:", k.x8.v0x512, e.x8.v0x512);

  m1.x4.v3x512 = vec512_foxes;
  m1.x4.v2x512 = vec512_zeros;
  m1.x4.v1x512 = vec512_zeros;
  m1.x4.v0x512 = vec512_zeros;
  m2.x4.v3x512 = vec512_zeros;
  m2.x4.v2x512 = vec512_zeros;
  m2.x4.v1x512 = vec512_zeros;
  m2.x4.v0x512 = vec512_foxes;
  __VEC_PWR_IMP (vec_mul2048x2048)(&k.x4096, &m1.x2048, &m2.x2048);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[3]   ", m1.x4.v3x512);
  print_vint512x (" m1[2]   ", m1.x4.v2x512);
  print_vint512x (" m1[1]   ", m1.x4.v1x512);
  print_vint512x (" m1[0] * ", m1.x4.v0x512);
  print_vint512x (" m2[3]   ", m2.x4.v3x512);
  print_vint512x (" m2[2]   ", m2.x4.v2x512);
  print_vint512x (" m2[1]   ", m2.x4.v1x512);
  print_vint512x (" m2[0] = ", m2.x4.v0x512);
  print_vint512x (" k [7]   ", k.x8.v7x512);
  print_vint512x (" k [6]   ", k.x8.v6x512);
  print_vint512x (" k [5]   ", k.x8.v5x512);
  print_vint512x (" k [4]   ", k.x8.v4x512);
  print_vint512x (" k [3]   ", k.x8.v3x512);
  print_vint512x (" k [2]   ", k.x8.v2x512);
  print_vint512x (" k [1]   ", k.x8.v1x512);
  print_vint512x (" k [0]   ", k.x8.v0x512);
#endif
  e.x8.v0x512 = vec512_zeros;
  e.x8.v1x512 = vec512_zeros;
  e.x8.v2x512 = vec512_zeros;
  e.x8.v3x512 = vec512_one;
  e.x8.v4x512 = vec512_foxeasy;
  e.x8.v5x512 = vec512_zeros;
  e.x8.v6x512 = vec512_zeros;
  e.x8.v7x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul2048x2048 6a:", k.x8.v7x512, e.x8.v7x512);
  rc += check_vint512 ("vec_mul2048x2048 6b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul2048x2048 6c:", k.x8.v5x512, e.x8.v5x512);
  rc += check_vint512 ("vec_mul2048x2048 6d:", k.x8.v4x512, e.x8.v4x512);
  rc += check_vint512 ("vec_mul2048x2048 6e:", k.x8.v3x512, e.x8.v3x512);
  rc += check_vint512 ("vec_mul2048x2048 6f:", k.x8.v2x512, e.x8.v2x512);
  rc += check_vint512 ("vec_mul2048x2048 6g:", k.x8.v1x512, e.x8.v1x512);
  rc += check_vint512 ("vec_mul2048x2048 6h:", k.x8.v0x512, e.x8.v0x512);

  __VEC_PWR_IMP (vec_mul2048x2048)(&k.x4096, &m1.x2048, &m2.x2048);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[3]   ", m2.x4.v3x512);
  print_vint512x (" m1[2]   ", m2.x4.v2x512);
  print_vint512x (" m1[1]   ", m2.x4.v1x512);
  print_vint512x (" m1[0] * ", m2.x4.v0x512);
  print_vint512x (" m2[3]   ", m1.x4.v3x512);
  print_vint512x (" m2[2]   ", m1.x4.v2x512);
  print_vint512x (" m2[1]   ", m1.x4.v1x512);
  print_vint512x (" m2[0] = ", m1.x4.v0x512);
  print_vint512x (" k [7]   ", k.x8.v7x512);
  print_vint512x (" k [6]   ", k.x8.v6x512);
  print_vint512x (" k [5]   ", k.x8.v5x512);
  print_vint512x (" k [4]   ", k.x8.v4x512);
  print_vint512x (" k [3]   ", k.x8.v3x512);
  print_vint512x (" k [2]   ", k.x8.v2x512);
  print_vint512x (" k [1]   ", k.x8.v1x512);
  print_vint512x (" k [0]   ", k.x8.v0x512);
#endif
  rc += check_vint512 ("vec_mul2048x2048 7a:", k.x8.v7x512, e.x8.v7x512);
  rc += check_vint512 ("vec_mul2048x2048 7b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul2048x2048 7c:", k.x8.v5x512, e.x8.v5x512);
  rc += check_vint512 ("vec_mul2048x2048 7d:", k.x8.v4x512, e.x8.v4x512);
  rc += check_vint512 ("vec_mul2048x2048 7e:", k.x8.v3x512, e.x8.v3x512);
  rc += check_vint512 ("vec_mul2048x2048 7f:", k.x8.v2x512, e.x8.v2x512);
  rc += check_vint512 ("vec_mul2048x2048 7g:", k.x8.v1x512, e.x8.v1x512);
  rc += check_vint512 ("vec_mul2048x2048 7h:", k.x8.v0x512, e.x8.v0x512);

  m1.x4.v3x512 = vec512_foxes;
  m1.x4.v2x512 = vec512_foxes;
  m1.x4.v1x512 = vec512_foxes;
  m1.x4.v0x512 = vec512_foxes;
  m2.x4.v3x512 = vec512_foxes;
  m2.x4.v2x512 = vec512_foxes;
  m2.x4.v1x512 = vec512_foxes;
  m2.x4.v0x512 = vec512_foxes;
  __VEC_PWR_IMP (vec_mul2048x2048)(&k.x4096, &m1.x2048, &m2.x2048);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[3]   ", m1.x4.v3x512);
  print_vint512x (" m1[2]   ", m1.x4.v2x512);
  print_vint512x (" m1[1]   ", m1.x4.v1x512);
  print_vint512x (" m1[0] * ", m1.x4.v0x512);
  print_vint512x (" m2[3]   ", m2.x4.v3x512);
  print_vint512x (" m2[2]   ", m2.x4.v2x512);
  print_vint512x (" m2[1]   ", m2.x4.v1x512);
  print_vint512x (" m2[0] = ", m2.x4.v0x512);
  print_vint512x (" k [7]   ", k.x8.v7x512);
  print_vint512x (" k [6]   ", k.x8.v6x512);
  print_vint512x (" k [5]   ", k.x8.v5x512);
  print_vint512x (" k [4]   ", k.x8.v4x512);
  print_vint512x (" k [3]   ", k.x8.v3x512);
  print_vint512x (" k [2]   ", k.x8.v2x512);
  print_vint512x (" k [1]   ", k.x8.v1x512);
  print_vint512x (" k [0]   ", k.x8.v0x512);
#endif
  e.x8.v0x512 = vec512_one;
  e.x8.v1x512 = vec512_zeros;
  e.x8.v2x512 = vec512_zeros;
  e.x8.v3x512 = vec512_zeros;
  e.x8.v4x512 = vec512_foxeasy;
  e.x8.v5x512 = vec512_foxes;
  e.x8.v6x512 = vec512_foxes;
  e.x8.v7x512 = vec512_foxes;
  rc += check_vint512 ("vec_mul2048x2048 8a:", k.x8.v7x512, e.x8.v7x512);
  rc += check_vint512 ("vec_mul2048x2048 8b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul2048x2048 8c:", k.x8.v5x512, e.x8.v5x512);
  rc += check_vint512 ("vec_mul2048x2048 8d:", k.x8.v4x512, e.x8.v4x512);
  rc += check_vint512 ("vec_mul2048x2048 8e:", k.x8.v3x512, e.x8.v3x512);
  rc += check_vint512 ("vec_mul2048x2048 8f:", k.x8.v2x512, e.x8.v2x512);
  rc += check_vint512 ("vec_mul2048x2048 8g:", k.x8.v1x512, e.x8.v1x512);
  rc += check_vint512 ("vec_mul2048x2048 8h:", k.x8.v0x512, e.x8.v0x512);

  m1.x4.v3x512 = vec512_zeros;
  m1.x4.v2x512 = vec512_zeros;
  m1.x4.v1x512 = vec512_foxes;
  m1.x4.v0x512 = vec512_foxes;
  m2.x4.v3x512 = vec512_zeros;
  m2.x4.v2x512 = vec512_zeros;
  m2.x4.v1x512 = vec512_foxes;
  m2.x4.v0x512 = vec512_foxes;
  __VEC_PWR_IMP (vec_mul2048x2048)(&k.x4096, &m1.x2048, &m2.x2048);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[3]   ", m1.x4.v3x512);
  print_vint512x (" m1[2]   ", m1.x4.v2x512);
  print_vint512x (" m1[1]   ", m1.x4.v1x512);
  print_vint512x (" m1[0] * ", m1.x4.v0x512);
  print_vint512x (" m2[3]   ", m2.x4.v3x512);
  print_vint512x (" m2[2]   ", m2.x4.v2x512);
  print_vint512x (" m2[1]   ", m2.x4.v1x512);
  print_vint512x (" m2[0] = ", m2.x4.v0x512);
  print_vint512x (" k [7]   ", k.x8.v7x512);
  print_vint512x (" k [6]   ", k.x8.v6x512);
  print_vint512x (" k [5]   ", k.x8.v5x512);
  print_vint512x (" k [4]   ", k.x8.v4x512);
  print_vint512x (" k [3]   ", k.x8.v3x512);
  print_vint512x (" k [2]   ", k.x8.v2x512);
  print_vint512x (" k [1]   ", k.x8.v1x512);
  print_vint512x (" k [0]   ", k.x8.v0x512);
#endif
  e.x8.v0x512 = vec512_one;
  e.x8.v1x512 = vec512_zeros;
  e.x8.v2x512 = vec512_foxeasy;
  e.x8.v3x512 = vec512_foxes;
  e.x8.v4x512 = vec512_zeros;
  e.x8.v5x512 = vec512_zeros;
  e.x8.v6x512 = vec512_zeros;
  e.x8.v7x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul2048x2048 9a:", k.x8.v7x512, e.x8.v7x512);
  rc += check_vint512 ("vec_mul2048x2048 9b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul2048x2048 9c:", k.x8.v5x512, e.x8.v5x512);
  rc += check_vint512 ("vec_mul2048x2048 9d:", k.x8.v4x512, e.x8.v4x512);
  rc += check_vint512 ("vec_mul2048x2048 9e:", k.x8.v3x512, e.x8.v3x512);
  rc += check_vint512 ("vec_mul2048x2048 9f:", k.x8.v2x512, e.x8.v2x512);
  rc += check_vint512 ("vec_mul2048x2048 9g:", k.x8.v1x512, e.x8.v1x512);
  rc += check_vint512 ("vec_mul2048x2048 9h:", k.x8.v0x512, e.x8.v0x512);

  m1.x4.v3x512 = vec512_foxes;
  m1.x4.v2x512 = vec512_foxes;
  m1.x4.v1x512 = vec512_foxes;
  m1.x4.v0x512 = vec512_foxes;
  m2.x4.v3x512 = vec512_zeros;
  m2.x4.v2x512 = vec512_zeros;
  m2.x4.v1x512 = vec512_zeros;
  m2.x4.v0x512 = vec512_foxes;
  __VEC_PWR_IMP (vec_mul2048x2048)(&k.x4096, &m1.x2048, &m2.x2048);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[3]   ", m1.x4.v3x512);
  print_vint512x (" m1[2]   ", m1.x4.v2x512);
  print_vint512x (" m1[1]   ", m1.x4.v1x512);
  print_vint512x (" m1[0] * ", m1.x4.v0x512);
  print_vint512x (" m2[3]   ", m2.x4.v3x512);
  print_vint512x (" m2[2]   ", m2.x4.v2x512);
  print_vint512x (" m2[1]   ", m2.x4.v1x512);
  print_vint512x (" m2[0] = ", m2.x4.v0x512);
  print_vint512x (" k [7]   ", k.x8.v7x512);
  print_vint512x (" k [6]   ", k.x8.v6x512);
  print_vint512x (" k [5]   ", k.x8.v5x512);
  print_vint512x (" k [4]   ", k.x8.v4x512);
  print_vint512x (" k [3]   ", k.x8.v3x512);
  print_vint512x (" k [2]   ", k.x8.v2x512);
  print_vint512x (" k [1]   ", k.x8.v1x512);
  print_vint512x (" k [0]   ", k.x8.v0x512);
#endif
  e.x8.v0x512 = vec512_one;
  e.x8.v1x512 = vec512_foxes;
  e.x8.v2x512 = vec512_foxes;
  e.x8.v3x512 = vec512_foxes;
  e.x8.v4x512 = vec512_foxeasy;
  e.x8.v5x512 = vec512_zeros;
  e.x8.v6x512 = vec512_zeros;
  e.x8.v7x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul2048x2048 10a:", k.x8.v7x512, e.x8.v7x512);
  rc += check_vint512 ("vec_mul2048x2048 10b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul2048x2048 10c:", k.x8.v5x512, e.x8.v5x512);
  rc += check_vint512 ("vec_mul2048x2048 10d:", k.x8.v4x512, e.x8.v4x512);
  rc += check_vint512 ("vec_mul2048x2048 10e:", k.x8.v3x512, e.x8.v3x512);
  rc += check_vint512 ("vec_mul2048x2048 10f:", k.x8.v2x512, e.x8.v2x512);
  rc += check_vint512 ("vec_mul2048x2048 10g:", k.x8.v1x512, e.x8.v1x512);
  rc += check_vint512 ("vec_mul2048x2048 10h:", k.x8.v0x512, e.x8.v0x512);

  m1.x4.v3x512 = vec512_zeros;
  m1.x4.v2x512 = vec512_zeros;
  m1.x4.v1x512 = vec512_zeros;
  m1.x4.v0x512 = vec512_ten128th;
  m2.x4.v3x512 = vec512_zeros;
  m2.x4.v2x512 = vec512_zeros;
  m2.x4.v1x512 = vec512_zeros;
  m2.x4.v0x512 = vec512_ten128th;
  __VEC_PWR_IMP (vec_mul2048x2048)(&k.x4096, &m1.x2048, &m2.x2048);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[3]   ", m1.x4.v3x512);
  print_vint512x (" m1[2]   ", m1.x4.v2x512);
  print_vint512x (" m1[1]   ", m1.x4.v1x512);
  print_vint512x (" m1[0] * ", m1.x4.v0x512);
  print_vint512x (" m2[3]   ", m2.x4.v3x512);
  print_vint512x (" m2[2]   ", m2.x4.v2x512);
  print_vint512x (" m2[1]   ", m2.x4.v1x512);
  print_vint512x (" m2[0] = ", m2.x4.v0x512);
  print_vint512x (" k [7]   ", k.x8.v7x512);
  print_vint512x (" k [6]   ", k.x8.v6x512);
  print_vint512x (" k [5]   ", k.x8.v5x512);
  print_vint512x (" k [4]   ", k.x8.v4x512);
  print_vint512x (" k [3]   ", k.x8.v3x512);
  print_vint512x (" k [2]   ", k.x8.v2x512);
  print_vint512x (" k [1]   ", k.x8.v1x512);
  print_vint512x (" k [0]   ", k.x8.v0x512);
#endif
  e.x8.v0x512 = vec512_ten256_l;
  e.x8.v1x512 = vec512_ten256_h;
  e.x8.v2x512 = vec512_zeros;
  e.x8.v3x512 = vec512_zeros;
  e.x8.v4x512 = vec512_zeros;
  e.x8.v5x512 = vec512_zeros;
  e.x8.v6x512 = vec512_zeros;
  e.x8.v7x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul2048x2048 11a:", k.x8.v7x512, e.x8.v7x512);
  rc += check_vint512 ("vec_mul2048x2048 11b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul2048x2048 11c:", k.x8.v5x512, e.x8.v5x512);
  rc += check_vint512 ("vec_mul2048x2048 11d:", k.x8.v4x512, e.x8.v4x512);
  rc += check_vint512 ("vec_mul2048x2048 11e:", k.x8.v3x512, e.x8.v3x512);
  rc += check_vint512 ("vec_mul2048x2048 11f:", k.x8.v2x512, e.x8.v2x512);
  rc += check_vint512 ("vec_mul2048x2048 11g:", k.x8.v1x512, e.x8.v1x512);
  rc += check_vint512 ("vec_mul2048x2048 11h:", k.x8.v0x512, e.x8.v0x512);

  m1.x4.v3x512 = vec512_zeros;
  m1.x4.v2x512 = vec512_zeros;
  m1.x4.v1x512 = e.x8.v1x512;
  m1.x4.v0x512 = e.x8.v0x512;
  m2.x4.v3x512 = vec512_zeros;
  m2.x4.v2x512 = vec512_zeros;
  m2.x4.v1x512 = e.x8.v1x512;
  m2.x4.v0x512 = e.x8.v0x512;
  __VEC_PWR_IMP (vec_mul2048x2048)(&k.x4096, &m1.x2048, &m2.x2048);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[3]   ", m1.x4.v3x512);
  print_vint512x (" m1[2]   ", m1.x4.v2x512);
  print_vint512x (" m1[1]   ", m1.x4.v1x512);
  print_vint512x (" m1[0] * ", m1.x4.v0x512);
  print_vint512x (" m2[3]   ", m2.x4.v3x512);
  print_vint512x (" m2[2]   ", m2.x4.v2x512);
  print_vint512x (" m2[1]   ", m2.x4.v1x512);
  print_vint512x (" m2[0] = ", m2.x4.v0x512);
  print_vint512x (" k [7]   ", k.x8.v7x512);
  print_vint512x (" k [6]   ", k.x8.v6x512);
  print_vint512x (" k [5]   ", k.x8.v5x512);
  print_vint512x (" k [4]   ", k.x8.v4x512);
  print_vint512x (" k [3]   ", k.x8.v3x512);
  print_vint512x (" k [2]   ", k.x8.v2x512);
  print_vint512x (" k [1]   ", k.x8.v1x512);
  print_vint512x (" k [0]   ", k.x8.v0x512);
#endif
  e.x8.v0x512 = vec512_zeros;
  e.x8.v1x512 = vec512_ten512_1;
  e.x8.v2x512 = vec512_ten512_2;
  e.x8.v3x512 = vec512_ten512_3;
  e.x8.v4x512 = vec512_zeros;
  e.x8.v5x512 = vec512_zeros;
  e.x8.v6x512 = vec512_zeros;
  e.x8.v7x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul2048x2048 12a:", k.x8.v7x512, e.x8.v7x512);
  rc += check_vint512 ("vec_mul2048x2048 12b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul2048x2048 12c:", k.x8.v5x512, e.x8.v5x512);
  rc += check_vint512 ("vec_mul2048x2048 12d:", k.x8.v4x512, e.x8.v4x512);
  rc += check_vint512 ("vec_mul2048x2048 12e:", k.x8.v3x512, e.x8.v3x512);
  rc += check_vint512 ("vec_mul2048x2048 12f:", k.x8.v2x512, e.x8.v2x512);
  rc += check_vint512 ("vec_mul2048x2048 12g:", k.x8.v1x512, e.x8.v1x512);
  rc += check_vint512 ("vec_mul2048x2048 12h:", k.x8.v0x512, e.x8.v0x512);

  m1.x4.v3x512 = e.x8.v3x512;
  m1.x4.v2x512 = e.x8.v2x512;
  m1.x4.v1x512 = e.x8.v1x512;
  m1.x4.v0x512 = e.x8.v0x512;
  m2.x4.v3x512 = e.x8.v3x512;
  m2.x4.v2x512 = e.x8.v2x512;
  m2.x4.v1x512 = e.x8.v1x512;
  m2.x4.v0x512 = e.x8.v0x512;
  __VEC_PWR_IMP (vec_mul2048x2048)(&k.x4096, &m1.x2048, &m2.x2048);

#ifdef __DEBUG_PRINT__
  print_vint512x (" m1[3]   ", m1.x4.v3x512);
  print_vint512x (" m1[2]   ", m1.x4.v2x512);
  print_vint512x (" m1[1]   ", m1.x4.v1x512);
  print_vint512x (" m1[0] * ", m1.x4.v0x512);
  print_vint512x (" m2[3]   ", m2.x4.v3x512);
  print_vint512x (" m2[2]   ", m2.x4.v2x512);
  print_vint512x (" m2[1]   ", m2.x4.v1x512);
  print_vint512x (" m2[0] = ", m2.x4.v0x512);
  print_vint512x (" k [7]   ", k.x8.v7x512);
  print_vint512x (" k [6]   ", k.x8.v6x512);
  print_vint512x (" k [5]   ", k.x8.v5x512);
  print_vint512x (" k [4]   ", k.x8.v4x512);
  print_vint512x (" k [3]   ", k.x8.v3x512);
  print_vint512x (" k [2]   ", k.x8.v2x512);
  print_vint512x (" k [1]   ", k.x8.v1x512);
  print_vint512x (" k [0]   ", k.x8.v0x512);
#endif
  e.x8.v0x512 = vec512_zeros;
  e.x8.v1x512 = vec512_zeros;
  e.x8.v2x512 = vec512_ten1024_2;
  e.x8.v3x512 = vec512_ten1024_3;
  e.x8.v4x512 = vec512_ten1024_4;
  e.x8.v5x512 = vec512_ten1024_5;
  e.x8.v6x512 = vec512_ten1024_6;
  e.x8.v7x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul2048x2048 13a:", k.x8.v7x512, e.x8.v7x512);
  rc += check_vint512 ("vec_mul2048x2048 13b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul2048x2048 13c:", k.x8.v5x512, e.x8.v5x512);
  rc += check_vint512 ("vec_mul2048x2048 13d:", k.x8.v4x512, e.x8.v4x512);
  rc += check_vint512 ("vec_mul2048x2048 13e:", k.x8.v3x512, e.x8.v3x512);
  rc += check_vint512 ("vec_mul2048x2048 13f:", k.x8.v2x512, e.x8.v2x512);
  rc += check_vint512 ("vec_mul2048x2048 13g:", k.x8.v1x512, e.x8.v1x512);
  rc += check_vint512 ("vec_mul2048x2048 13h:", k.x8.v0x512, e.x8.v0x512);


  return (rc);
}
#undef __DEBUG_PRINT__

int
test_vec_i512 (void)
{
  int rc = 0;

  printf ("\n%s\n", __FUNCTION__);

  rc += test_mul128x128 ();
  rc += test_mul256x256 ();
  rc += test_mul512x128 ();
  rc += test_mul512x512 ();
  rc += test_mul1024x1024 ();
  rc += test_mul2048x2048 ();
  rc += test_madd512x128 ();
  rc += test_mul512x128_MN ();
  rc += test_mul512x512_MN ();
  rc += test_mul2048x2048_MN ();

  return (rc);
}

