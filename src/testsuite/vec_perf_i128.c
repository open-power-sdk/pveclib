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
static const vui32_t c_one = CONST_VINT32_W(0, 0, 0, 1);
static const vui32_t c_two = CONST_VINT32_W(0, 0, 0, 2);
static const vui32_t c_ten = CONST_VINT32_W(0, 0, 0, 10);


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
      rc += check_vuint128 ("vec_mul10uq:", k, vtipowof10[ii]);
      print_vint128 ("x * 10 ", k);
#endif
      i = k;
    }
  rc += check_vuint128 ("vec_mulluq:", k, vtipowof10[38]);

  return rc;
}


