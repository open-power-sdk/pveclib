/*
 * vec_perf_f32.c
 *
 *  Created on: Jul 4, 2018
 *      Author: sjmunroe
 */
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

//#define __DEBUG_PRINT__
#include <pveclib/vec_f32_ppc.h>

#include <testsuite/arith128_print.h>
#include <testsuite/vec_perf_f32.h>
#include <testsuite/arith128_test_f32.h>

#define N 10
static const vf32_t data0 =
  { __FLT_MAX__, __FLT_MIN__, __FLT_EPSILON__, __FLT_DENORM_MIN__ };

extern __vector bool int
test_pred_f32_inf (vf32_t value);
extern __vector bool int
test_pred_f32_nan (vf32_t value);
extern __vector bool int
test_pred_f32_normal (vf32_t value);
extern __vector bool int
test_pred_f32_subnormal (vf32_t value);
extern __vector bool int
test_pred_f32_zero (vf32_t value);

#ifndef PVECLIB_DISABLE_F128MATH
extern vui32_t
test_fpclassify_f32 (vf32_t value);
#endif

int timed_is_f32 (void)
{
  vb32_t accum = {0,0,0,0};
  int i;

  for (i=0; i<N; i++)
    {
      accum |= test_pred_f32_inf (data0);
      accum |= test_pred_f32_nan (data0);
      accum |= test_pred_f32_normal (data0);
      accum |= test_pred_f32_subnormal (data0);
      accum |= test_pred_f32_zero (data0);
    }
   return 0;
}

int timed_fpclassify_f32 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  vb32_t accum = {0,0,0,0};
  int i;

  for (i=0; i<N; i++)
    {
      accum |= test_fpclassify_f32 (data0);
    }
#endif
   return 0;
}

int
timed_scalar_f32_transpose ()
{
  float tmatrix[MN][MN] __attribute__ ((aligned (128)));
  int rc = 0;

  test_f32_matrix_transpose (&tmatrix[0][0], &matrix_f32[0][0]);

  return rc;
}

int
timed_gather_f32_transpose ()
{
  float tmatrix[MN][MN] __attribute__ ((aligned (128)));
  int rc = 0;

  test_f32_matrix_gather_transpose (&tmatrix[0][0], &matrix_f32[0][0]);

  return rc;
}

int
timed_gatherx2_f32_transpose ()
{
  float tmatrix[MN][MN] __attribute__ ((aligned (128)));
  int rc = 0;

  test_f32_matrix_gatherx2_transpose (&tmatrix[0][0], &matrix_f32[0][0]);

  return rc;
}

#if 1
int
timed_gatherx4_f32_transpose ()
{
  float tmatrix[MN][MN] __attribute__ ((aligned (128)));
  int rc = 0;

  test_f32_matrix_gatherx4_transpose (&tmatrix[0][0], &matrix_f32[0][0]);

  return rc;
}
#endif
