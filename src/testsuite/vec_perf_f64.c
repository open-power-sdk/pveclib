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

 vec_perf_f64.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Jun 31, 2018
 */


#include <stdint.h>
#include <stdio.h>
#include <fenv.h>
#include <float.h>

//#define __DEBUG_PRINT__
#include <pveclib/vec_f64_ppc.h>

#include <testsuite/arith128_print.h>
#include <testsuite/vec_perf_f64.h>
#include <testsuite/arith128_test_f64.h>

#define N 10
static const vf64_t data0 = { __DBL_MAX__, __DBL_MIN__ };
static const vf64_t data1 = { __DBL_EPSILON__, __DBL_DENORM_MIN__ };

extern __vector bool long long
test_pred_f64_inf (vf64_t value);
extern __vector bool long long
test_pred_f64_nan (vf64_t value);
extern __vector bool long long
test_pred_f64_normal (vf64_t value);
extern __vector bool long long
test_pred_f64_subnormal (vf64_t value);
extern __vector bool long long
test_pred_f64_zero (vf64_t value);

#ifndef  PVECLIB_DISABLE_F128MATH
extern vui64_t
test_fpclassify_f64 (vf64_t value);
#endif

int timed_is_f64 (void)
{
  vb64_t accum = {0,0};
  int i;

  for (i=0; i<N; i++)
    {
      accum |= test_pred_f64_inf (data0);
      accum |= test_pred_f64_inf (data1);
      accum |= test_pred_f64_nan (data0);
      accum |= test_pred_f64_nan (data1);
      accum |= test_pred_f64_normal (data0);
      accum |= test_pred_f64_normal (data1);
      accum |= test_pred_f64_subnormal (data0);
      accum |= test_pred_f64_subnormal (data1);
      accum |= test_pred_f64_zero (data0);
      accum |= test_pred_f64_zero (data1);
    }
   return 0;
}

int timed_fpclassify_f64 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  vui64_t accum = {0,0};
  int i;

  for (i=0; i<N; i++)
    {
      accum |= test_fpclassify_f64 (data0);
    }
#endif
   return 0;
}

int
timed_setup_f64_transpose ()
{
  int rc = 0;

  test_f64_Imatrix_init (&matrix_f64[0][0]);

  return rc;
}

int
timed_scalar_f64_transpose ()
{
  double tmatrix[MN][MN] __attribute__ ((aligned (128)));
  int rc = 0;

  test_f64_matrix_transpose (&tmatrix[0][0], &matrix_f64[0][0]);

  return rc;
}

int
timed_gather_f64_transpose ()
{
  double tmatrix[MN][MN] __attribute__ ((aligned (128)));
  int rc = 0;

  test_f64_matrix_gather_transpose (&tmatrix[0][0], &matrix_f64[0][0]);

  return rc;
}

int
timed_gatherx2_f64_transpose ()
{
  double tmatrix[MN][MN] __attribute__ ((aligned (128)));
  int rc = 0;

  test_f64_matrix_gatherx2_transpose (&tmatrix[0][0], &matrix_f64[0][0]);

  return rc;
}

int
timed_gatherx4_f64_transpose ()
{
  double tmatrix[MN][MN] __attribute__ ((aligned (128)));
  int rc = 0;

  test_f64_matrix_gatherx4_transpose (&tmatrix[0][0], &matrix_f64[0][0]);

  return rc;
}
