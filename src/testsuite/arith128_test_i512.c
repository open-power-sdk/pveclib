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
#include <testsuite/vec_perf_i512.h>


const __VEC_U_512 vec512_zeros =
    {
	(vui128_t) ((unsigned __int128) 0),
	(vui128_t) ((unsigned __int128) 0),
	(vui128_t) ((unsigned __int128) 0),
	(vui128_t) ((unsigned __int128) 0)
    };

const __VEC_U_512 vec512_one = CONST_VINT512_Q
    (
      (vui128_t) ((unsigned __int128) 0x00000000),
      (vui128_t) ((unsigned __int128) 0x00000000),
      (vui128_t) ((unsigned __int128) 0x00000000),
      (vui128_t) ((unsigned __int128) 0x00000001)
    );

const __VEC_U_512 vec512_foxes = CONST_VINT512_Q
    (
      // FFFF...FFFF
      (vui128_t) ((__int128) -1),
      (vui128_t) ((__int128) -1),
      (vui128_t) ((__int128) -1),
      (vui128_t) ((__int128) -1)
    );

const __VEC_U_512 vec512_foxeasy = CONST_VINT512_Q
    (
	// FFFF...FFFE
	(vui128_t) ((__int128) -1),
	(vui128_t) ((__int128) -1),
	(vui128_t) ((__int128) -1),
	(vui128_t) ((__int128) -2)
    );

const __VEC_U_512  vec512_ten128th = CONST_VINT512_Q
    (
      CONST_VUINT128_QxW (0x00000000, 0x00000000, 0x0000024e, 0xe91f2603),
      CONST_VUINT128_QxW (0xa6337f19, 0xbccdb0da, 0xc404dc08, 0xd3cff5ec),
      CONST_VUINT128_QxW (0x2374e42f, 0x0f1538fd, 0x03df9909, 0x2e953e01),
      CONST_VUINT128_QxW (0x00000000, 0x00000000, 0x00000000, 0x00000000)
    );

const __VEC_U_512 vec512_ten256_h = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0x00000000, 0x00000000, 0x00000000, 0x00000000),
	CONST_VUINT128_QxW (0x00000000, 0x000553f7, 0x5fdcefce, 0xf46eeddc),
	CONST_VUINT128_QxW (0x80dcc7f7, 0x55bc28f2, 0x65f9ef17, 0xcc5573c0),
	CONST_VUINT128_QxW (0x63ff540e, 0x3c42d35a, 0x1d153624, 0xadc666b0)
    );

const __VEC_U_512 vec512_ten256_l = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0x26b2716e, 0xd595d80f, 0xcf4a6e70, 0x6bde50c6),
	CONST_VUINT128_QxW (0x12152f87, 0xd8d99f72, 0xbed3875b, 0x982e7c01),
	CONST_VUINT128_QxW (0x00000000, 0x00000000, 0x00000000, 0x00000000),
	CONST_VUINT128_QxW (0x00000000, 0x00000000, 0x00000000, 0x00000000)
    );

const __VEC_U_512 vec512_ten384_h = CONST_VINT512_Q
    (
        CONST_VUINT128_QxW (0x00000000, 0x00000000, 0x00000000, 0x00000000),
	CONST_VUINT128_QxW (0x00000000, 0x00000000, 0x00000000, 0x00000000),
	CONST_VUINT128_QxW (0x0c4c5e31, 0x0aef8aa1, 0x71027fff, 0x56784f44),
	CONST_VUINT128_QxW (0x4e117bef, 0xa6fab7d1, 0x99c34cce, 0x922010ba)
    );

const __VEC_U_512 vec512_ten384_m = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0xcba53571, 0xac20aee4, 0xe9cd9bd7, 0xf1b92377),
	CONST_VUINT128_QxW( 0xfc363385, 0x867b782d, 0x6e921a29, 0x949e1b66),
	CONST_VUINT128_QxW( 0x23303863, 0xb92d6f55, 0x34a8e6d6, 0x997d980d),
	CONST_VUINT128_QxW( 0x61c2dbb1, 0x09169857, 0x4cff3c6c, 0x0892aa8f)
    );

const __VEC_U_512 vec512_ten384_l = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0x3347f75d, 0xe297de35, 0x60e19896, 0x34cbba01),
	CONST_VUINT128_QxW( 0x00000000, 0x00000000, 0x00000000, 0x00000000),
	CONST_VUINT128_QxW( 0x00000000, 0x00000000, 0x00000000, 0x00000000),
	CONST_VUINT128_QxW( 0x00000000, 0x00000000, 0x00000000, 0x00000000)
    );


const __VEC_U_512 vec512_ten512_3 = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0x00000000, 0x00000000, 0x00000000, 0x00000000),
	CONST_VUINT128_QxW (0x00000000, 0x00000000, 0x00000000, 0x00000000),
	CONST_VUINT128_QxW (0x00000000, 0x00000000, 0x0000001C, 0x633415D4),
	CONST_VUINT128_QxW (0xC1D238D9, 0x8CAB8A97, 0x8A0B1F13, 0x8CB07303)
    );

const __VEC_U_512 vec512_ten512_2 = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0xA2699748, 0x45A71D46, 0xB099BC81, 0x7343AFAC),
	CONST_VUINT128_QxW (0x69BE5B0E, 0x9449775C, 0x1366732A, 0x93ABADE4),
	CONST_VUINT128_QxW (0xB2908EE0, 0xF95F635E, 0x85A91924, 0xC3FC0695),
	CONST_VUINT128_QxW (0xE7FC7153, 0x329C57AE, 0xBFA3EDAC, 0x96E14F5D)
    );

const __VEC_U_512 vec512_ten512_1 = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0xBC51FB2E, 0xB21A2F22, 0x1E25CFEA, 0x703ED321),
	CONST_VUINT128_QxW (0xAA1DA1BF, 0x28F8733B, 0x4475B579, 0xC88976C1),
	CONST_VUINT128_QxW (0x94E65747, 0x46C40513, 0xC31E1AD9, 0xB83A8A97),
	CONST_VUINT128_QxW (0x5D96976F, 0x8F9546DC, 0x77F27267, 0xFC6CF801)
    );

const __VEC_U_512 vec512_ten512_0 = CONST_VINT512_Q
    (
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0)
    );

const __VEC_U_512 vec512_ten1024_6 = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0x00000000, 0x00000000, 0x00000000, 0x00000000),
	CONST_VUINT128_QxW (0x00000000, 0x00000325, 0xD9D61A05, 0xD4305D94),
	CONST_VUINT128_QxW (0x34F4A3C6, 0x2D433949, 0xAE6209D4, 0x926C3F5B),
	CONST_VUINT128_QxW (0xD2DB49EF, 0x47187094, 0xC1A6970C, 0xA7E6BD2A)
    );

const __VEC_U_512 vec512_ten1024_5 = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0x73C55349, 0x36A8DE06, 0x1E8D4649, 0xF4F3235E),
	CONST_VUINT128_QxW (0x005B8041, 0x1640114A, 0x88BC491B, 0x9FC4ED52),
	CONST_VUINT128_QxW (0x0190FBA0, 0x35FAABA6, 0xC356E38A, 0x31B5653F),
	CONST_VUINT128_QxW (0x44597583, 0x6CB0B6C9, 0x75A351A2, 0x8E4262CE)
    );

const __VEC_U_512 vec512_ten1024_4 = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0x3CE3A0B8, 0xDF68368A, 0xE26A7B7E, 0x976A3310),
	CONST_VUINT128_QxW (0xFC8F1F90, 0x31EB0F66, 0x9A202882, 0x80BDA5A5),
	CONST_VUINT128_QxW (0x80D98089, 0xDC1A47FE, 0x6B7595FB, 0x101A3616),
	CONST_VUINT128_QxW (0xB6F4654B, 0x31FB6BFD, 0xF56DEEEC, 0xB1B896BC)
    );

const __VEC_U_512 vec512_ten1024_3 = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0x8FC51A16, 0xBF3FDEB3, 0xD814B505, 0xBA34C411),
	CONST_VUINT128_QxW (0x8AD822A5, 0x1ABE1DE3, 0x045B7A74, 0x8E1042C4),
	CONST_VUINT128_QxW (0x62BE695A, 0x9F9F2A07, 0xA7E89431, 0x922BBB9F),
	CONST_VUINT128_QxW (0xC9635986, 0x1C5CD134, 0xF451218B, 0x65DC60D7)
    );

const __VEC_U_512 vec512_ten1024_2 = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0x233E55C7, 0x231D2B9C, 0x9FCE837D, 0x1E43F61F),
	CONST_VUINT128_QxW (0x7DE16CFB, 0x896634EE, 0x0ED1440E, 0xCC2CD819),
	CONST_VUINT128_QxW (0x4C7D1E1A, 0x140AC535, 0x15C51A88, 0x991C4E87),
	CONST_VUINT128_QxW (0x1EC29F86, 0x6E7C215B, 0xF55B2B72, 0x2919F001)
    );

const __VEC_U_512 vec512_ten1024_1 = CONST_VINT512_Q
    (
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0)
    );

const __VEC_U_512 vec512_ten1024_0 = CONST_VINT512_Q
    (
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0)
    );

#define TIMING_ITERATIONS 10

int
test_time_i512 (void)
{
  long i;
  uint64_t t_start, t_end, t_delta;
  double delta_sec;
  int rc = 0;

  printf ("\n%s mul512x512 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_mul512x512 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s mul512x512 end", __FUNCTION__);
  printf ("\n%s mul512x512 delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

  printf ("\n%s mul1024x1024 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_mul1024x1024 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s mul1024x1024 end", __FUNCTION__);
  printf ("\n%s mul1024x1024 delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

  printf ("\n%s mul2048x2048 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_mul2048x2048 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s mul2048x2048 end", __FUNCTION__);
  printf ("\n%s mul2048x2048 delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
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
test_mul512x128 (void)
{
  __VEC_U_640 k, e;
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

//#define __DEBUG_PRINT__ 1
int
test_mul512x512 (void)
{
  __VEC_U_1024 k, e;
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
  rc += check_vint512 ("vec_mul512x128 1a:", kp.x2.v1x512, ep.x2.v1x512);
  rc += check_vint512 ("vec_mul512x128 1b:", kp.x2.v0x512, ep.x2.v0x512);

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
  rc += check_vint512 ("vec_mul512x128 2a:", kp.x2.v1x512, ep.x2.v1x512);
  rc += check_vint512 ("vec_mul512x128 2b:", kp.x2.v0x512, ep.x2.v0x512);
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
  rc += check_vint512 ("vec_mul512x128 2a:", kp.x2.v1x512, ep.x2.v1x512);
  rc += check_vint512 ("vec_mul512x128 2b:", kp.x2.v0x512, ep.x2.v0x512);

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
  rc += check_vint512 ("vec_mul512x128 3a:", kp.x2.v1x512, ep.x2.v1x512);
  rc += check_vint512 ("vec_mul512x128 3b:", kp.x2.v0x512, ep.x2.v0x512);
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
  rc += check_vint512 ("vec_mul512x128 4a:", kp.x2.v1x512, ep.x2.v1x512);
  rc += check_vint512 ("vec_mul512x128 4b:", kp.x2.v0x512, ep.x2.v0x512);

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
  rc += check_vint512 ("vec_mul512x128 5a:", kp.x2.v1x512, ep.x2.v1x512);
  rc += check_vint512 ("vec_mul512x128 5b:", kp.x2.v0x512, ep.x2.v0x512);
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
  rc += check_vint512 ("vec_mul512x128 6a:", kp.x2.v1x512, ep.x2.v1x512);
  rc += check_vint512 ("vec_mul512x128 6b:", kp.x2.v0x512, ep.x2.v0x512);

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
  rc += check_vint512 ("vec_mul512x128 7a:", kp.x2.v1x512, ep.x2.v1x512);
  rc += check_vint512 ("vec_mul512x128 7b:", kp.x2.v0x512, ep.x2.v0x512);

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
  rc += check_vint512 ("vec_mul512x128 8a:", kp.x2.v1x512, ep.x2.v1x512);
  rc += check_vint512 ("vec_mul512x128 8b:", kp.x2.v0x512, ep.x2.v0x512);

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
  rc += check_vint512 ("vec_mul512x128 8a:", kp.x2.v1x512, ep.x2.v1x512);
  rc += check_vint512 ("vec_mul512x128 8b:", kp.x2.v0x512, ep.x2.v0x512);

  return (rc);
}


//#define __DEBUG_PRINT__ 1
int
test_mul1024x1024 (void)
{
  __VEC_U_2048x512 k, e;
  __VEC_U_1024x512 m1, m2;
  __VEC_U_512 i, j;

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

//#define __DEBUG_PRINT__ 1
int
test_mul2048x2048 (void)
{
  __VEC_U_4096x512 k, e;
  __VEC_U_2048x512 m1, m2;
  __VEC_U_512 i, j;

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
#if 1
  rc += test_mul128x128 ();
  rc += test_mul256x256 ();
  rc += test_mul512x128 ();
  rc += test_mul512x512 ();
  rc += test_mul1024x1024 ();
  rc += test_mul2048x2048 ();
#endif
#if 1
  rc += test_time_i512();
#endif
  return (rc);
}

