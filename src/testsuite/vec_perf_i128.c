/*
 Copyright (c) [2018] IBM Corporation.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 vec_perf_i128.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Jun 21, 2018
 */


#include <stdint.h>
#include <stdio.h>
#include <fenv.h>
#include <float.h>
#include <math.h>

//#define __DEBUG_PRINT__
#include <vec_int128_ppc.h>

//#include "arith128.h"
#include <testsuite/arith128_print.h>
#include <testsuite/vec_perf_i128.h>

extern const vui128_t vtipowof10 [];
static const vui32_t c_zero = CONST_VINT32_W(0, 0, 0, 0);
static const vui32_t c_one = CONST_VINT32_W(0, 0, 0, 1);
static const vui32_t c_two = CONST_VINT32_W(0, 0, 0, 2);
static const vui32_t c_nine = CONST_VINT32_W(0, 0, 0, 9);
static const vui32_t c_ten = CONST_VINT32_W(0, 0, 0, 10);

/* 10^64th as a binary const requiring 256-bits.  */
static const vui32_t ten_64h =
    CONST_VINT32_W(0x00000000, 0x00184f03, 0xe93ff9f4, 0xdaa797ed);
static const vui32_t ten_64l =
    CONST_VINT32_W(0x6e38ed64, 0xbf6a1f01, 0x00000000, 0x00000000);


int timed_mul10uq (void)
{
  vui128_t i, k;
  int ii;
  int rc = 0;

#ifdef __DEBUG_PRINT__
  print_vint128 ("init i=1", (vui128_t)i);
#endif

  i = (vui128_t)c_one;
  for (ii = 1; ii < 39; ii++)
    {
      k = vec_mul10uq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
      rc += check_vuint128 ("vec_mul10uq:", k, vtipowof10[ii]);
      print_vint128 ("x * 10 ", k);
#endif
      i = k;
    }
  rc += check_vuint128 ("vec_mul10uq:", k, vtipowof10[38]);

  return rc;
}

int timed_mul10uq2x (void)
{
  vui128_t i, k;
  vui128_t i2, k2, ks;
  int ii;
  int rc = 0;

#ifdef __DEBUG_PRINT__
  print_vint128 ("init i=1", (vui128_t)i);
#endif

  i = (vui128_t)c_one;
  i2 = (vui128_t)c_two;
  for (ii = 1; ii < 39; ii++)
    {
      k = vec_mul10uq ((vui128_t) i);
      k2 = vec_mul10uq ((vui128_t) i2);

#ifdef __DEBUG_PRINT__
      rc += check_vuint128 ("vec_mul10uq:", k, vtipowof10[ii]);
      print_vint128 ("x * 10 ", k);
#endif
      i = k;
      i2 = k2;
    }
  rc += check_vuint128 ("vec_mul10uq:", k, vtipowof10[38]);
  ks = vec_adduqm (k,k);
  rc += check_vuint128 ("vec_mul10uq2x:", k2, ks);

  return rc;
}

//#define __DEBUG_PRINT__
int timed_cmul10ecuq (void)
{
  vui128_t i, k, j;
  vui128_t i2, k2, ks;
  int ii;
  int rc = 0;

  i = (vui128_t)c_zero;
  i2 = (vui128_t)c_zero;
  for (ii = 0; ii < 64; ii++)
    {
      k = vec_cmul10ecuq (&j, i, (vui128_t)c_nine);
      k2 = vec_mul10euq ((vui128_t) i2, j);
      i = k;
      i2 = k2;
    }
  /* k2|k should be 10^64-1.  */
  ks = vec_adduqm (k, (vui128_t)c_one);
  rc += check_vuint128x ("vec_cmul10ecuq:", ks, (vui128_t) ten_64l);
  rc += check_vuint128x ("vec_mul10euq:", k2, (vui128_t) ten_64h);

  return rc;
}
//#undef __DEBUG_PRINT__
//#define __DEBUG_PRINT__
int timed_mulluq (void)
{
  vui128_t i, k;
  int ii;
  int rc = 0;

#ifdef __DEBUG_PRINT__
  print_vint128 ("init i=1", (vui128_t)i);
#endif

  i = (vui128_t)c_one;
  for (ii = 1; ii < 39; ii++)
    {
      k = vec_mulluq ((vui128_t) i, (vui128_t) c_ten);

#ifdef __DEBUG_PRINT__
      rc += check_vuint128 ("vec_mulluq:", k, vtipowof10[ii]);
      print_vint128 ("x * 10 ", k);
#endif
      i = k;
    }
  rc += check_vuint128 ("vec_mulluq:", k, vtipowof10[38]);

  return rc;
}

#pragma GCC optimize ("no-unroll-loops")
//#define __DEBUG_PRINT__
int
timed_muludq (void)
{
  vui128_t i, j, k;
  int ii, iii;
  int rc = 0;

  i = (vui128_t) c_ten;
  iii = 2;
#ifdef __DEBUG_PRINT__
  print_vint128 ("init i=10", (vui128_t) i);
#endif

  for (ii = 1; ii < 7; ii++)
    {
      k = vec_muludq (&j, (vui128_t) i, (vui128_t) i);

#ifdef __DEBUG_PRINT__
      if (ii < 6)
	{
	  printf ("iii=%d ", iii);
	  print_vint128 ("10Eiii=", k);
	  rc += check_vuint128 ("vec_muludq:", k, vtipowof10[iii]);
	}
#endif
      i = k;
      iii = iii + iii;
    }
  rc += check_vuint128x ("vec_muludqx k:", k, (vui128_t) ten_64l);
  rc += check_vuint128x ("vec_muludqx j:", j, (vui128_t) ten_64h);

  return rc;
}

//#define __DEBUG_PRINT__
/* Older (GCC6) compilers ignore the "no-unroll-loops" pragma and make
   it hard to isolate each iteration of vec_muludq.  This version
   calls __test_muludq from vec_int128_dummy.c as a work around.  */
extern vui128_t
__test_muludq (vui128_t *mh, vui128_t a, vui128_t b);

int
timed_muludqx (void)
{
  vui128_t i, j, k;
  int ii, iii;
  int rc = 0;

  i = (vui128_t) c_ten;
  iii = 2;
#ifdef __DEBUG_PRINT__
  print_vint128 ("init i=10", (vui128_t) i);
#endif

  for (ii = 1; ii < 7; ii++)
    {
      k = __test_muludq (&j, (vui128_t) i, (vui128_t) i);

#ifdef __DEBUG_PRINT__
      if (ii < 6)
	{
	  printf ("iii=%d ", iii);
	  print_vint128 ("10Eiii=", k);
	  rc += check_vuint128 ("vec_muludq:", k, vtipowof10[iii]);
	}
#endif
      i = k;
      iii = iii + iii;
    }
  rc += check_vuint128x ("vec_muludqx k:", k, (vui128_t) ten_64l);
  rc += check_vuint128x ("vec_muludqx j:", j, (vui128_t) ten_64h);

  return rc;
}
