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
#include <stdlib.h>
#include <fenv.h>
#include <float.h>
#include <math.h>

//#define __DEBUG_PRINT__
#include <pveclib/vec_int128_ppc.h>
#include <pveclib/vec_int512_ppc.h>

//#include "arith128.h"
#include <testsuite/arith128_print.h>
#include <testsuite/arith128_test_i512.h>
#include <testsuite/vec_perf_i512.h>

//#define __DEBUG_PRINT__ 1
int
timed_mul1024x1024 (void)
{
  __VEC_U_2048x512 k, e;
  __VEC_U_1024x512 m1, m2;
  __VEC_U_512 i, j;
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
  __VEC_U_512 i, j;

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
timed_mul512x512 (void)
{
  __VEC_U_1024 k, e;
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
