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

 vec_perf_i512.c

 Contributors:
      Steven Munroe
      Created on: Sep 9, 2019
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

//#define __DEBUG_PRINT__
#include <pveclib/vec_int128_ppc.h>
#include <pveclib/vec_int512_ppc.h>

//#include "arith128.h"
#include <testsuite/arith128_print.h>
#include <testsuite/arith128_test_i512.h>
#include <testsuite/vec_perf_i512.h>

/* 10^64th as a binary const requiring 256-bits.  */
static const vui128_t c_zero =  (vui128_t) ((unsigned __int128) 0);
static const vui128_t c_one =  (vui128_t) ((unsigned __int128) 1);
static const vui128_t c_ten =  (vui128_t) ((unsigned __int128) 10);
static const vui128_t c_hundred =  (vui128_t) ((unsigned __int128) 100);
static const vui128_t c_10k =  (vui128_t) ((unsigned __int128) 10000);
static const vui128_t c_100m =  (vui128_t) ((unsigned __int128) 100000000);
static const vui32_t ten_64h =
    CONST_VINT32_W(0x00000000, 0x00184f03, 0xe93ff9f4, 0xdaa797ed);
static const vui32_t ten_64l =
    CONST_VINT32_W(0x6e38ed64, 0xbf6a1f01, 0x00000000, 0x00000000);

//#define __DEBUG_PRINT__ 1
int
timed_mul1024x1024by8 (void)
{
  __VEC_U_2048x512 k1, k2;
  __VEC_U_1024x512 m1;
  int rc = 0;

#ifdef __DEBUG_PRINT__
  printf ("\ntest_mul1024x1024 vector multiply quadword, 2048-bit product\n");
#endif

  m1.x2.v1x512 = vec512_zeros;
  m1.x2.v0x512.vx3 = c_zero;
  m1.x2.v0x512.vx2 = c_zero;
  m1.x2.v0x512.vx1 = c_zero;
  m1.x2.v0x512.vx0 = c_hundred;
  // 10^4 <-
  __VEC_PWR_IMP (vec_mul1024x1024)(&k1.x2048, &m1.x1024, &m1.x1024);
  // 10^8 <-
  __VEC_PWR_IMP (vec_mul1024x1024)(&k2.x2048, &k1.x2.v0x1024, &k1.x2.v0x1024);
  // 10^16 <-
  __VEC_PWR_IMP (vec_mul1024x1024)(&k1.x2048, &k2.x2.v0x1024, &k2.x2.v0x1024);
  // 10^32 <-
  __VEC_PWR_IMP (vec_mul1024x1024)(&k2.x2048, &k1.x2.v0x1024, &k1.x2.v0x1024);
  // 10^64 <-
  __VEC_PWR_IMP (vec_mul1024x1024)(&k1.x2048, &k2.x2.v0x1024, &k2.x2.v0x1024);
  // 10^128 <-
  __VEC_PWR_IMP (vec_mul1024x1024)(&k2.x2048, &k1.x2.v0x1024, &k1.x2.v0x1024);
  // 10^256 <-
  __VEC_PWR_IMP (vec_mul1024x1024)(&k1.x2048, &k2.x2.v0x1024, &k2.x2.v0x1024);
  // 10^512 <-
  __VEC_PWR_IMP (vec_mul1024x1024)(&k2.x2048, &k1.x2.v0x1024, &k1.x2.v0x1024);

  rc += check_vint512 ("vec_mul1024x1024 10a:", k2.x4.v3x512, vec512_ten512_3);
  rc += check_vint512 ("vec_mul1024x1024 10b:", k2.x4.v2x512, vec512_ten512_2);
  rc += check_vint512 ("vec_mul1024x1024 10c:", k2.x4.v1x512, vec512_ten512_1);
  rc += check_vint512 ("vec_mul1024x1024 10d:", k2.x4.v0x512, vec512_ten512_0);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
timed_mul2048x2048by8 (void)
{
  __VEC_U_4096x512 k1, k2;
  __VEC_U_2048x512 m1;

  int rc = 0;

#ifdef __DEBUG_PRINT__
  printf ("\ntimed_mul2048x2048 vector multiply quadword, 4096-bit product\n");
#endif
  m1.x4.v3x512 = vec512_zeros;
  m1.x4.v2x512 = vec512_zeros;
  m1.x4.v1x512 = vec512_zeros;
  m1.x4.v0x512.vx3 = c_zero;
  m1.x4.v0x512.vx2 = c_zero;
  m1.x4.v0x512.vx1 = c_zero;
  m1.x4.v0x512.vx0 = c_10k;
  // 10^8 <-
  __VEC_PWR_IMP (vec_mul2048x2048)(&k1.x4096, &m1.x2048, &m1.x2048);
  // 10^16 <-
  __VEC_PWR_IMP (vec_mul2048x2048)(&k2.x4096, &k1.x2.v0x2048, &k1.x2.v0x2048);
  // 10^32 <-
  __VEC_PWR_IMP (vec_mul2048x2048)(&k1.x4096, &k2.x2.v0x2048, &k2.x2.v0x2048);
  // 10^64 <-
  __VEC_PWR_IMP (vec_mul2048x2048)(&k2.x4096, &k1.x2.v0x2048, &k1.x2.v0x2048);
  // 10^128 <-
  __VEC_PWR_IMP (vec_mul2048x2048)(&k1.x4096, &k2.x2.v0x2048, &k2.x2.v0x2048);
  // 10^256 <-
  __VEC_PWR_IMP (vec_mul2048x2048)(&k2.x4096, &k1.x2.v0x2048, &k1.x2.v0x2048);
  // 10^512 <-
  __VEC_PWR_IMP (vec_mul2048x2048)(&k1.x4096, &k2.x2.v0x2048, &k2.x2.v0x2048);
  // 10^1024 <-
  __VEC_PWR_IMP (vec_mul2048x2048)(&k2.x4096, &k1.x2.v0x2048, &k1.x2.v0x2048);

#ifdef __DEBUG_PRINT__
  rc += check_vint512 ("vec_mul2048x2048 10a:", k2.x8.v7x512, vec512_ten1024_0);
#endif
  rc += check_vint512 ("vec_mul2048x2048 10b:", k2.x8.v6x512, vec512_ten1024_6);
  rc += check_vint512 ("vec_mul2048x2048 10c:", k2.x8.v5x512, vec512_ten1024_5);
  rc += check_vint512 ("vec_mul2048x2048 10d:", k2.x8.v4x512, vec512_ten1024_4);
  rc += check_vint512 ("vec_mul2048x2048 10e:", k2.x8.v3x512, vec512_ten1024_3);
  rc += check_vint512 ("vec_mul2048x2048 10f:", k2.x8.v2x512, vec512_ten1024_2);
#ifdef __DEBUG_PRINT__
  rc += check_vint512 ("vec_mul2048x2048 10g:", k2.x8.v1x512, vec512_ten1024_1);
  rc += check_vint512 ("vec_mul2048x2048 10h:", k2.x8.v0x512, vec512_ten1024_0);
#endif

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
timed_mul2048x2048_MN (void)
{
  __VEC_U_4096x512 k1, k2;
  __VEC_U_2048x512 m1;
  __VEC_U_512 *kp1, *kp2, *ip, *jp1, *jp2;

  int rc = 0;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  kp1 = &k1.x8.v0x512;
  kp2 = &k2.x8.v0x512;
  ip = &m1.x4.v0x512;
  // Low order 1024-bits of k1/k2
  jp1 = &k1.x8.v0x512;
  jp2 = &k2.x8.v0x512;

#else
  kp1 = &k1.x8.v7x512;
  kp2 = &k2.x8.v7x512;
  ip = &m1.x4.v3x512;
  // Low order 1024-bits of k1/k2
  jp1 = &k1.x8.v3x512;
  jp2 = &k2.x8.v3x512;
#endif

#ifdef __DEBUG_PRINT__
  printf ("\ntimed_mulx2048_MN vector multiply quadword, 4096-bit product\n");
#endif
  m1.x4.v3x512 = vec512_zeros;
  m1.x4.v2x512 = vec512_zeros;
  m1.x4.v1x512 = vec512_zeros;
  m1.x4.v0x512.vx3 = c_zero;
  m1.x4.v0x512.vx2 = c_zero;
  m1.x4.v0x512.vx1 = c_zero;
  m1.x4.v0x512.vx0 = c_10k;
  // 10^8 <-
  __VEC_PWR_IMP (vec_mul512_byMN)(kp1, ip, ip, 4, 4);
  // 10^16 <-
  __VEC_PWR_IMP (vec_mul512_byMN)(kp2, jp1, jp1, 4, 4);
  // 10^32 <-
  __VEC_PWR_IMP (vec_mul512_byMN)(kp1, jp2, jp2, 4, 4);
  // 10^64 <-
  __VEC_PWR_IMP (vec_mul512_byMN)(kp2, jp1, jp1, 4, 4);
  // 10^128 <-
  __VEC_PWR_IMP (vec_mul512_byMN)(kp1, jp2, jp2, 4, 4);
  // 10^256 <-
  __VEC_PWR_IMP (vec_mul512_byMN)(kp2, jp1, jp1, 4, 4);
  // 10^512 <-
  __VEC_PWR_IMP (vec_mul512_byMN)(kp1, jp2, jp2, 4, 4);
  // 10^1024 <-
  __VEC_PWR_IMP (vec_mul512_byMN)(kp2, jp1, jp1, 4, 4);

#ifdef __DEBUG_PRINT__
  rc += check_vint512 ("vec_mulx2048_MN 10a:", k2.x8.v7x512, vec512_ten1024_0);
#endif
  rc += check_vint512 ("vec_mulx2048_MN 10b:", k2.x8.v6x512, vec512_ten1024_6);
  rc += check_vint512 ("vec_mulx2048_MN 10c:", k2.x8.v5x512, vec512_ten1024_5);
  rc += check_vint512 ("vec_mulx2048_MN 10d:", k2.x8.v4x512, vec512_ten1024_4);
  rc += check_vint512 ("vec_mulx2048_MN 10e:", k2.x8.v3x512, vec512_ten1024_3);
  rc += check_vint512 ("vec_mulx2048_MN 10f:", k2.x8.v2x512, vec512_ten1024_2);
#ifdef __DEBUG_PRINT__
  rc += check_vint512 ("vec_mulx2048_MN 10g:", k2.x8.v1x512, vec512_ten1024_1);
  rc += check_vint512 ("vec_mulx2048_MN 10h:", k2.x8.v0x512, vec512_ten1024_0);
#endif

  return (rc);
}

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define __NDX16(__index) (__index)
#else
#define __NDX16(__index) ((16 - 1) - (__index))
#endif
//#define __DEBUG_PRINT__ 1
int
timed_mul4096x4096_MN (void)
{
  __VEC_U_512 k1[16], k2[16];
  __VEC_U_4096x512 m1;
  __VEC_U_512 *kp1, *kp2, *ip, *jp1, *jp2;

  int rc = 0;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  kp1 = &k1[0];
  kp2 = &k2[0];
  ip = &m1.x8.v0x512;
  // Low order 2048-bits of k1/k2
  jp1 = &k1[0];
  jp2 = &k2[0];

#else
  kp1 = &k1[0];
  kp2 = &k2[0];
  ip = &m1.x8.v7x512;
  // Low order 2048-bits of k1/k2
  jp1 = &k1[8];
  jp2 = &k2[8];
#endif

#ifdef __DEBUG_PRINT__
  printf ("\ntimed_mulx4096_MN vector multiply quadword, 8192-bit product\n");
#endif
  m1.x8.v7x512 = vec512_zeros;
  m1.x8.v6x512 = vec512_zeros;
  m1.x8.v5x512 = vec512_zeros;
  m1.x8.v4x512 = vec512_zeros;
  m1.x8.v3x512 = vec512_zeros;
  m1.x8.v2x512 = vec512_zeros;
  m1.x8.v1x512 = vec512_zeros;
  m1.x8.v0x512.vx3 = c_zero;
  m1.x8.v0x512.vx2 = c_zero;
  m1.x8.v0x512.vx1 = c_zero;
  m1.x8.v0x512.vx0 = c_100m;
  // 10^8 <-
  __VEC_PWR_IMP (vec_mul512_byMN)(kp1, ip, ip, 8, 8);
  // 10^16 <-
  __VEC_PWR_IMP (vec_mul512_byMN)(kp2, jp1, jp1, 8, 8);
  // 10^32 <-
  __VEC_PWR_IMP (vec_mul512_byMN)(kp1, jp2, jp2, 8, 8);
  // 10^64 <-
  __VEC_PWR_IMP (vec_mul512_byMN)(kp2, jp1, jp1, 8, 8);
  // 10^128 <-
  __VEC_PWR_IMP (vec_mul512_byMN)(kp1, jp2, jp2, 8, 8);
  // 10^256 <-
  __VEC_PWR_IMP (vec_mul512_byMN)(kp2, jp1, jp1, 8, 8);
  // 10^512 <-
  __VEC_PWR_IMP (vec_mul512_byMN)(kp1, jp2, jp2, 8, 8);
  // 10^1024 <-
  __VEC_PWR_IMP (vec_mul512_byMN)(kp2, jp1, jp1, 8, 8);

#ifdef __DEBUG_PRINT__
  rc += check_vint512 ("vec_mulx4096_MN 14a:", kp2[__NDX16(15)], vec512_zeros);
  rc += check_vint512 ("vec_mulx4096_MN 14b:", kp2[__NDX16(14)], vec512_zeros);
#endif
  rc += check_vint512 ("vec_mulx4096_MN 14c:", kp2[__NDX16(13)], vec512_ten2048_13);
  rc += check_vint512 ("vec_mulx4096_MN 14d:", kp2[__NDX16(12)], vec512_ten2048_12);
  rc += check_vint512 ("vec_mulx4096_MN 14e:", kp2[__NDX16(11)], vec512_ten2048_11);
  rc += check_vint512 ("vec_mulx4096_MN 14f:", kp2[__NDX16(10)], vec512_ten2048_10);
  rc += check_vint512 ("vec_mulx4096_MN 14g:", kp2[__NDX16(9)], vec512_ten2048_9);
  rc += check_vint512 ("vec_mulx4096_MN 14h:", kp2[__NDX16(8)], vec512_ten2048_8);
  rc += check_vint512 ("vec_mulx4096_MN 14i:", kp2[__NDX16(7)], vec512_ten2048_7);
  rc += check_vint512 ("vec_mulx4096_MN 14j:", kp2[__NDX16(6)], vec512_ten2048_6);
  rc += check_vint512 ("vec_mulx4096_MN 14k:", kp2[__NDX16(5)], vec512_ten2048_5);
  rc += check_vint512 ("vec_mulx4096_MN 14l:", kp2[__NDX16(4)], vec512_ten2048_4);
  rc += check_vint512 ("vec_mulx4096_MN 14m:", kp2[__NDX16(3)], vec512_ten2048_3);
  rc += check_vint512 ("vec_mulx4096_MN 14n:", kp2[__NDX16(2)], vec512_ten2048_2);
#ifdef __DEBUG_PRINT__
  rc += check_vint512 ("vec_mulx4096_MN 14o:", kp2[__NDX16(1)], vec512_zeros);
  rc += check_vint512 ("vec_mulx4096_MN 14p:", kp2[__NDX16(0)], vec512_zeros);
#endif

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
timed_mul1024x1024 (void)
{
  __VEC_U_2048x512 k, e;
  __VEC_U_1024x512 m1, m2;
  int rc = 0;

#ifdef __DEBUG_PRINT__
  printf ("\ntest_mul1024x1024 vector multiply quadword, 2048-bit product\n");
#endif

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
#ifdef __DEBUG_PRINT__
  e.x4.v2x512 = vec512_zeros;
  e.x4.v3x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul1024x1024 1a:", k.x4.v3x512, e.x4.v3x512);
  rc += check_vint512 ("vec_mul1024x1024 1b:", k.x4.v2x512, e.x4.v2x512);
#endif
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
#ifdef __DEBUG_PRINT__
  e.x4.v3x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul1024x1024 2a:", k.x4.v3x512, e.x4.v3x512);
#endif
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
#ifdef __DEBUG_PRINT__
  rc += check_vint512 ("vec_mul1024x1024 3a:", k.x4.v3x512, e.x4.v3x512);
#endif
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
#ifdef __DEBUG_PRINT__
  e.x4.v0x512 = vec512_zeros;
  e.x4.v1x512 = vec512_zeros;
#endif
  e.x4.v2x512 = vec512_one;
  e.x4.v3x512 = vec512_foxeasy;
  rc += check_vint512 ("vec_mul1024x1024 4a:", k.x4.v3x512, e.x4.v3x512);
  rc += check_vint512 ("vec_mul1024x1024 4b:", k.x4.v2x512, e.x4.v2x512);
#ifdef __DEBUG_PRINT__
  rc += check_vint512 ("vec_mul1024x1024 4c:", k.x4.v1x512, e.x4.v1x512);
  rc += check_vint512 ("vec_mul1024x1024 4d:", k.x4.v0x512, e.x4.v0x512);
#endif

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
#ifdef __DEBUG_PRINT__
  e.x4.v2x512 = vec512_zeros;
  e.x4.v3x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul1024x1024 6a:", k.x4.v3x512, e.x4.v3x512);
  rc += check_vint512 ("vec_mul1024x1024 6b:", k.x4.v2x512, e.x4.v2x512);
#endif
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
#ifdef __DEBUG_PRINT__
  e.x4.v3x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul1024x1024 8a:", k.x4.v3x512, e.x4.v3x512);
#endif
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
#ifdef __DEBUG_PRINT__
  rc += check_vint512 ("vec_mul1024x1024 9a:", k.x4.v3x512, e.x4.v3x512);
#endif
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

//#define __DEBUG_PRINT__ 1
int
timed_mul2048x2048 (void)
{
  __VEC_U_4096x512 k, e;
  __VEC_U_2048x512 m1, m2;

  int rc = 0;

#ifdef __DEBUG_PRINT__
  printf ("\ntimed_mul2048x2048 vector multiply quadword, 4096-bit product\n");
#endif

  m1.x4.v3x512 = vec512_zeros;
  m1.x4.v2x512 = vec512_zeros;
  m1.x4.v1x512 = vec512_zeros;
  m1.x4.v0x512 = vec512_foxes;
  m2.x4.v3x512 = vec512_zeros;
  m2.x4.v2x512 = vec512_zeros;
  m2.x4.v1x512 = vec512_zeros;
  m2.x4.v0x512 = vec512_foxes;
  __VEC_PWR_IMP (vec_mul2048x2048)(&k.x4096, &m1.x2048, &m2.x2048);

  e.x8.v0x512 = vec512_one;
  e.x8.v1x512 = vec512_foxeasy;
#ifdef __DEBUG_PRINT__
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
#endif
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

  e.x8.v0x512 = vec512_zeros;
  e.x8.v1x512 = vec512_one;
  e.x8.v2x512 = vec512_foxeasy;
#ifdef __DEBUG_PRINT__
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
#endif
  rc += check_vint512 ("vec_mul2048x2048 2f:", k.x8.v2x512, e.x8.v2x512);
  rc += check_vint512 ("vec_mul2048x2048 2g:", k.x8.v1x512, e.x8.v1x512);
  rc += check_vint512 ("vec_mul2048x2048 2h:", k.x8.v0x512, e.x8.v0x512);

  m1.x4.v3x512 = vec512_zeros;
  m1.x4.v2x512 = vec512_foxes;
  m1.x4.v1x512 = vec512_zeros;
  m1.x4.v0x512 = vec512_zeros;
  m2.x4.v3x512 = vec512_zeros;
  m2.x4.v2x512 = vec512_zeros;
  m2.x4.v1x512 = vec512_zeros;
  m2.x4.v0x512 = vec512_foxes;
  __VEC_PWR_IMP (vec_mul2048x2048)(&k.x4096, &m1.x2048, &m2.x2048);

  e.x8.v0x512 = vec512_zeros;
  e.x8.v1x512 = vec512_zeros;
  e.x8.v2x512 = vec512_one;
  e.x8.v3x512 = vec512_foxeasy;
#ifdef __DEBUG_PRINT__
  e.x8.v4x512 = vec512_zeros;
  e.x8.v5x512 = vec512_zeros;
  e.x8.v6x512 = vec512_zeros;
  e.x8.v7x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul2048x2048 4a:", k.x8.v7x512, e.x8.v7x512);
  rc += check_vint512 ("vec_mul2048x2048 4b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul2048x2048 4c:", k.x8.v5x512, e.x8.v5x512);
  rc += check_vint512 ("vec_mul2048x2048 4d:", k.x8.v4x512, e.x8.v4x512);
#endif
  rc += check_vint512 ("vec_mul2048x2048 4e:", k.x8.v3x512, e.x8.v3x512);
  rc += check_vint512 ("vec_mul2048x2048 4f:", k.x8.v2x512, e.x8.v2x512);
#ifdef __DEBUG_PRINT__
  rc += check_vint512 ("vec_mul2048x2048 4g:", k.x8.v1x512, e.x8.v1x512);
  rc += check_vint512 ("vec_mul2048x2048 4h:", k.x8.v0x512, e.x8.v0x512);
#endif

  m1.x4.v3x512 = vec512_foxes;
  m1.x4.v2x512 = vec512_zeros;
  m1.x4.v1x512 = vec512_zeros;
  m1.x4.v0x512 = vec512_zeros;
  m2.x4.v3x512 = vec512_zeros;
  m2.x4.v2x512 = vec512_zeros;
  m2.x4.v1x512 = vec512_zeros;
  m2.x4.v0x512 = vec512_foxes;
  __VEC_PWR_IMP (vec_mul2048x2048)(&k.x4096, &m1.x2048, &m2.x2048);

  e.x8.v0x512 = vec512_zeros;
  e.x8.v1x512 = vec512_zeros;
  e.x8.v2x512 = vec512_zeros;
  e.x8.v3x512 = vec512_one;
  e.x8.v4x512 = vec512_foxeasy;
#ifdef __DEBUG_PRINT__
  e.x8.v5x512 = vec512_zeros;
  e.x8.v6x512 = vec512_zeros;
  e.x8.v7x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul2048x2048 6a:", k.x8.v7x512, e.x8.v7x512);
  rc += check_vint512 ("vec_mul2048x2048 6b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul2048x2048 6c:", k.x8.v5x512, e.x8.v5x512);
#endif
  rc += check_vint512 ("vec_mul2048x2048 6d:", k.x8.v4x512, e.x8.v4x512);
  rc += check_vint512 ("vec_mul2048x2048 6e:", k.x8.v3x512, e.x8.v3x512);
#ifdef __DEBUG_PRINT__
  rc += check_vint512 ("vec_mul2048x2048 6f:", k.x8.v2x512, e.x8.v2x512);
  rc += check_vint512 ("vec_mul2048x2048 6g:", k.x8.v1x512, e.x8.v1x512);
  rc += check_vint512 ("vec_mul2048x2048 6h:", k.x8.v0x512, e.x8.v0x512);
#endif

  m1.x4.v3x512 = vec512_foxes;
  m1.x4.v2x512 = vec512_foxes;
  m1.x4.v1x512 = vec512_foxes;
  m1.x4.v0x512 = vec512_foxes;
  m2.x4.v3x512 = vec512_foxes;
  m2.x4.v2x512 = vec512_foxes;
  m2.x4.v1x512 = vec512_foxes;
  m2.x4.v0x512 = vec512_foxes;
  __VEC_PWR_IMP (vec_mul2048x2048)(&k.x4096, &m1.x2048, &m2.x2048);

  e.x8.v0x512 = vec512_one;
  e.x8.v1x512 = vec512_zeros;
  e.x8.v2x512 = vec512_zeros;
  e.x8.v3x512 = vec512_zeros;
  e.x8.v4x512 = vec512_foxeasy;
  e.x8.v5x512 = vec512_foxes;
  e.x8.v6x512 = vec512_foxes;
  e.x8.v7x512 = vec512_foxes;
  rc += check_vint512 ("vec_mul2048x2048 8a:", k.x8.v7x512, e.x8.v7x512);
#ifdef __DEBUG_PRINT__
  rc += check_vint512 ("vec_mul2048x2048 8b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul2048x2048 8c:", k.x8.v5x512, e.x8.v5x512);
#endif
  rc += check_vint512 ("vec_mul2048x2048 8d:", k.x8.v4x512, e.x8.v4x512);
#ifdef __DEBUG_PRINT__
  rc += check_vint512 ("vec_mul2048x2048 8e:", k.x8.v3x512, e.x8.v3x512);
  rc += check_vint512 ("vec_mul2048x2048 8f:", k.x8.v2x512, e.x8.v2x512);
  rc += check_vint512 ("vec_mul2048x2048 8g:", k.x8.v1x512, e.x8.v1x512);
#endif
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

  e.x8.v0x512 = vec512_one;
  e.x8.v1x512 = vec512_zeros;
  e.x8.v2x512 = vec512_foxeasy;
  e.x8.v3x512 = vec512_foxes;
#ifdef __DEBUG_PRINT__
  e.x8.v4x512 = vec512_zeros;
  e.x8.v5x512 = vec512_zeros;
  e.x8.v6x512 = vec512_zeros;
  e.x8.v7x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul2048x2048 9a:", k.x8.v7x512, e.x8.v7x512);
  rc += check_vint512 ("vec_mul2048x2048 9b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul2048x2048 9c:", k.x8.v5x512, e.x8.v5x512);
  rc += check_vint512 ("vec_mul2048x2048 9d:", k.x8.v4x512, e.x8.v4x512);
#endif
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

  e.x8.v0x512 = vec512_one;
  e.x8.v1x512 = vec512_foxes;
  e.x8.v2x512 = vec512_foxes;
  e.x8.v3x512 = vec512_foxes;
  e.x8.v4x512 = vec512_foxeasy;
#ifdef __DEBUG_PRINT__
  e.x8.v5x512 = vec512_zeros;
  e.x8.v6x512 = vec512_zeros;
  e.x8.v7x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul2048x2048 10a:", k.x8.v7x512, e.x8.v7x512);
  rc += check_vint512 ("vec_mul2048x2048 10b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul2048x2048 10c:", k.x8.v5x512, e.x8.v5x512);
#endif
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

  e.x8.v0x512 = vec512_ten256_l;
  e.x8.v1x512 = vec512_ten256_h;
#ifdef __DEBUG_PRINT__
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
#endif
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

  e.x8.v0x512 = vec512_zeros;
  e.x8.v1x512 = vec512_ten512_1;
  e.x8.v2x512 = vec512_ten512_2;
  e.x8.v3x512 = vec512_ten512_3;
#ifdef __DEBUG_PRINT__
  e.x8.v4x512 = vec512_zeros;
  e.x8.v5x512 = vec512_zeros;
  e.x8.v6x512 = vec512_zeros;
  e.x8.v7x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul2048x2048 12a:", k.x8.v7x512, e.x8.v7x512);
  rc += check_vint512 ("vec_mul2048x2048 12b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul2048x2048 12c:", k.x8.v5x512, e.x8.v5x512);
  rc += check_vint512 ("vec_mul2048x2048 12d:", k.x8.v4x512, e.x8.v4x512);
#endif
  rc += check_vint512 ("vec_mul2048x2048 12e:", k.x8.v3x512, e.x8.v3x512);
  rc += check_vint512 ("vec_mul2048x2048 12f:", k.x8.v2x512, e.x8.v2x512);
  rc += check_vint512 ("vec_mul2048x2048 12g:", k.x8.v1x512, e.x8.v1x512);
#ifdef __DEBUG_PRINT__
  rc += check_vint512 ("vec_mul2048x2048 12h:", k.x8.v0x512, e.x8.v0x512);
#endif

  m1.x4.v3x512 = e.x8.v3x512;
  m1.x4.v2x512 = e.x8.v2x512;
  m1.x4.v1x512 = e.x8.v1x512;
  m1.x4.v0x512 = e.x8.v0x512;
  m2.x4.v3x512 = e.x8.v3x512;
  m2.x4.v2x512 = e.x8.v2x512;
  m2.x4.v1x512 = e.x8.v1x512;
  m2.x4.v0x512 = e.x8.v0x512;
  __VEC_PWR_IMP (vec_mul2048x2048)(&k.x4096, &m1.x2048, &m2.x2048);

  e.x8.v0x512 = vec512_zeros;
  e.x8.v1x512 = vec512_zeros;
  e.x8.v2x512 = vec512_ten1024_2;
  e.x8.v3x512 = vec512_ten1024_3;
  e.x8.v4x512 = vec512_ten1024_4;
  e.x8.v5x512 = vec512_ten1024_5;
  e.x8.v6x512 = vec512_ten1024_6;
#ifdef __DEBUG_PRINT__
  e.x8.v7x512 = vec512_zeros;
  rc += check_vint512 ("vec_mul2048x2048 13a:", k.x8.v7x512, e.x8.v7x512);
#endif
  rc += check_vint512 ("vec_mul2048x2048 13b:", k.x8.v6x512, e.x8.v6x512);
  rc += check_vint512 ("vec_mul2048x2048 13c:", k.x8.v5x512, e.x8.v5x512);
  rc += check_vint512 ("vec_mul2048x2048 13d:", k.x8.v4x512, e.x8.v4x512);
  rc += check_vint512 ("vec_mul2048x2048 13e:", k.x8.v3x512, e.x8.v3x512);
  rc += check_vint512 ("vec_mul2048x2048 13f:", k.x8.v2x512, e.x8.v2x512);
#ifdef __DEBUG_PRINT__
  rc += check_vint512 ("vec_mul2048x2048 13g:", k.x8.v1x512, e.x8.v1x512);
  rc += check_vint512 ("vec_mul2048x2048 13h:", k.x8.v0x512, e.x8.v0x512);
#endif


  return (rc);
}

int
timed_mul128x128 (void)
{
  vui128_t i, j;
  __VEC_U_256 k, k2;
  int rc = 0;

  i = (vui128_t) c_ten;
  j = (vui128_t) c_one;
  k2 = __VEC_PWR_IMP (vec_mul128x128) (j, j);
  j = k2.vx0;
  k  = __VEC_PWR_IMP (vec_mul128x128) (i, j);
  i = k.vx0;
  k2 = __VEC_PWR_IMP (vec_mul128x128) (i, i);
  i = k2.vx0;
  k  = __VEC_PWR_IMP (vec_mul128x128) (i, i);
  i = k.vx0;
  k2 = __VEC_PWR_IMP (vec_mul128x128) (i, i);
  i = k2.vx0;
  k  = __VEC_PWR_IMP (vec_mul128x128) (i, i);
  i = k.vx0;
  k2  = __VEC_PWR_IMP (vec_mul128x128) (i, i);
  i = k2.vx0;
  k  =  __VEC_PWR_IMP (vec_mul128x128) (i, i);

  rc += check_vint256 ("vec_mul128x128 1:", k.vx1, k.vx0, (vui128_t)ten_64h, (vui128_t)ten_64l);

  return rc;
}

int
timed_mul256x256 (void)
{
  __VEC_U_256 i, j;
  __VEC_U_512 k, k2;
  int rc = 0;

  i.vx0 = (vui128_t) c_ten;
  i.vx1 = (vui128_t) c_zero;
  j.vx0 = (vui128_t) c_one;
  j.vx1 = (vui128_t) c_zero;

  k2 = __VEC_PWR_IMP (vec_mul256x256) (i, j);
  i.vx0 = k2.vx0;
  i.vx1 = k2.vx1;
  k  = __VEC_PWR_IMP (vec_mul256x256) (i, i);
  i.vx0 = k.vx0;
  i.vx1 = k.vx1;
  k2 = __VEC_PWR_IMP (vec_mul256x256) (i, i);
  i.vx0 = k2.vx0;
  i.vx1 = k2.vx1;
  k  = __VEC_PWR_IMP (vec_mul256x256) (i, i);
  i.vx0 = k.vx0;
  i.vx1 = k.vx1;
  k2 = __VEC_PWR_IMP (vec_mul256x256) (i, i);
  i.vx0 = k2.vx0;
  i.vx1 = k2.vx1;
  k  = __VEC_PWR_IMP (vec_mul256x256) (i, i);
  i.vx0 = k.vx0;
  i.vx1 = k.vx1;
  k2  = __VEC_PWR_IMP (vec_mul256x256) (i, i);
  i.vx0 = k2.vx0;
  i.vx1 = k2.vx1;
  k  =  __VEC_PWR_IMP (vec_mul256x256) (i, i);

  rc += check_vint512 ("vec_mul256x256:", k, vec512_ten128th);

  return rc;
}

int
timed_mul512x512by8 (void)
{
  __VEC_U_512 i;
  __VEC_U_1024x512 k, k2;
  int rc = 0;

//  i.vx0 = (vui128_t) c_hundred;
  i.vx0 = (vui128_t) c_ten;
  i.vx1 = (vui128_t) c_zero;
  i.vx2 = (vui128_t) c_zero;
  i.vx3 = (vui128_t) c_zero;

  k2.x1024 = __VEC_PWR_IMP (vec_mul512x512) (i, i);
  k.x1024 = __VEC_PWR_IMP (vec_mul512x512) (k2.x2.v0x512, k2.x2.v0x512);
  k2.x1024 = __VEC_PWR_IMP (vec_mul512x512) (k.x2.v0x512, k.x2.v0x512);
  k.x1024 = __VEC_PWR_IMP (vec_mul512x512) (k2.x2.v0x512, k2.x2.v0x512);
  k2.x1024 = __VEC_PWR_IMP (vec_mul512x512) (k.x2.v0x512, k.x2.v0x512);
  k.x1024 = __VEC_PWR_IMP (vec_mul512x512) (k2.x2.v0x512, k2.x2.v0x512);
  k2.x1024 = __VEC_PWR_IMP (vec_mul512x512) (k.x2.v0x512, k.x2.v0x512);
  k.x1024 = __VEC_PWR_IMP (vec_mul512x512) (k2.x2.v0x512, k2.x2.v0x512);

  rc += check_vint512 ("vec_mul512x512a:", k.x2.v1x512, vec512_ten256_h);
  rc += check_vint512 ("vec_mul512x512b:", k.x2.v0x512, vec512_ten256_l);

  return rc;
}

int
timed_mul512x512 (void)
{
  __VEC_U_1024 k;
  __VEC_U_1024x512 kp, ep;
  __VEC_U_512 i, j;
  int rc = 0;

#ifdef __DEBUG_PRINT__
  printf ("\ntest_mul512x512 vector multiply quadword, 1024-bit product\n");
#endif

  i.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  i.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  i.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  i.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx0 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  k = __VEC_PWR_IMP (vec_mul512x512)(i, j);

  kp.x1024 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
#ifdef __DEBUG_PRINT__
  ep.x2.v1x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint512 ("vec_mul512x128 1a:", kp.x2.v1x512, ep.x2.v1x512);
#endif
  rc += check_vint512 ("vec_mul512x128 1b:", kp.x2.v0x512, ep.x2.v0x512);

  j.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx1 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  k = __VEC_PWR_IMP (vec_mul512x512)(i, j);

  kp.x1024 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
#ifdef __DEBUG_PRINT__
  ep.x2.v1x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint512 ("vec_mul512x128 2a:", kp.x2.v1x512, ep.x2.v1x512);
#endif
  rc += check_vint512 ("vec_mul512x128 2b:", kp.x2.v0x512, ep.x2.v0x512);
  k = __VEC_PWR_IMP (vec_mul512x512)(j, i);

  kp.x1024 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
#ifdef __DEBUG_PRINT__
  ep.x2.v1x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint512 ("vec_mul512x128 2a:", kp.x2.v1x512, ep.x2.v1x512);
#endif
  rc += check_vint512 ("vec_mul512x128 2b:", kp.x2.v0x512, ep.x2.v0x512);

  j.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx2 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  k = __VEC_PWR_IMP (vec_mul512x512)(i, j);

  kp.x1024 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
#ifdef __DEBUG_PRINT__
  ep.x2.v1x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint512 ("vec_mul512x128 3a:", kp.x2.v1x512, ep.x2.v1x512);
#endif
  rc += check_vint512 ("vec_mul512x128 3b:", kp.x2.v0x512, ep.x2.v0x512);
  k = __VEC_PWR_IMP (vec_mul512x512)(j, i);

  kp.x1024 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
#ifdef __DEBUG_PRINT__
  ep.x2.v1x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint512 ("vec_mul512x128 4a:", kp.x2.v1x512, ep.x2.v1x512);
#endif
  rc += check_vint512 ("vec_mul512x128 4b:", kp.x2.v0x512, ep.x2.v0x512);

  j.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  j.vx3 = (vui128_t) CONST_VINT128_W (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  k = __VEC_PWR_IMP (vec_mul512x512)(i, j);

#ifdef __DEBUG_PRINT__
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

  kp.x1024 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x6e38ed64, 0xbf6a1f01, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00184f03, 0xe93ff9f4, 0xdaa797ed);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
#ifdef __DEBUG_PRINT__
  ep.x2.v1x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint512 ("vec_mul512x128 7a:", kp.x2.v1x512, ep.x2.v1x512);
#endif
  rc += check_vint512 ("vec_mul512x128 7b:", kp.x2.v0x512, ep.x2.v0x512);

  kp.x1024 = k;
  i = kp.x2.v0x512;
  j = kp.x2.v0x512;
  k = __VEC_PWR_IMP (vec_mul512x512)(i, j);

  kp.x1024 = k;
  ep.x2.v0x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v0x512.vx1 = (vui128_t) CONST_VINT128_W (0x2374e42f, 0x0f1538fd, 0x03df9909, 0x2e953e01);
  ep.x2.v0x512.vx2 = (vui128_t) CONST_VINT128_W (0xa6337f19, 0xbccdb0da, 0xc404dc08, 0xd3cff5ec);
  ep.x2.v0x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x0000024e, 0xe91f2603);
#ifdef __DEBUG_PRINT__
  ep.x2.v1x512.vx0 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx1 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx2 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ep.x2.v1x512.vx3 = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint512 ("vec_mul512x128 8a:", kp.x2.v1x512, ep.x2.v1x512);
#endif
  rc += check_vint512 ("vec_mul512x128 8b:", kp.x2.v0x512, ep.x2.v0x512);

  kp.x1024 = k;
  i = kp.x2.v0x512;
  j = kp.x2.v0x512;
  k = __VEC_PWR_IMP (vec_mul512x512)(i, j);

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
