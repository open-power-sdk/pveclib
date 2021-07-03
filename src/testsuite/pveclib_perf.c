/*
 Copyright (c) [2021] IBM Corporation.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 pveclib_perf.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: May 26, 2021
 */

#define __STDC_WANT_DEC_FP__    1

#ifndef PVECLIB_DISABLE_F128MATH
/* Disable <stdlib.h> for __clang__ because of bug involving <floatn.h>
   incombination with -mcpu=power9 -mfloat128 */
#include <stdlib.h>
#else
#define EXIT_SUCCESS 0
#endif
#include <stdint.h>
#include <stdio.h>
#include <fenv.h>
#include <float.h>

//#define __DEBUG_PRINT__

#include <testsuite/arith128_print.h>
#include <testsuite/vec_perf_i128.h>
#include <testsuite/vec_perf_i512.h>
#include <testsuite/vec_perf_f32.h>
#include <testsuite/vec_perf_f64.h>
#include <testsuite/vec_perf_f128.h>

#define TIMING_ITERATIONS 10

int
test_time_i512 (void)
{
  long i;
  uint64_t t_start, t_end, t_delta;
  double delta_sec;
  int rc = 0;

  printf ("\n%s mul128x128 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_mul128x128 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s mul128x128 end", __FUNCTION__);
  printf ("\n%s mul128x128 delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

  printf ("\n%s mul256x256 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_mul256x256 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s mul256x256 end", __FUNCTION__);
  printf ("\n%s mul256x256 delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

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

  printf ("\n%s mul512x512by8 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_mul512x512by8 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s mul512x512by8 end", __FUNCTION__);
  printf ("\n%s mul512x512by8 delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
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

  printf ("\n%s mul1024x1024by8 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_mul1024x1024by8 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s mul1024x1024by8 end", __FUNCTION__);
  printf ("\n%s mul1024x1024by8 delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
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

  printf ("\n%s mul2048x2048by8 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_mul2048x2048by8 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s mul2048x2048by8 end", __FUNCTION__);
  printf ("\n%s mul2048x2048by8 delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

  printf ("\n%s timed_mul2048x2048_MN start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_mul2048x2048_MN ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s timed_mul2048x2048_MN end", __FUNCTION__);
  printf ("\n%s timed_mul2048x2048_MN delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

  printf ("\n%s timed_mul4096x4096_MN start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_mul4096x4096_MN ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s timed_mul4096x4096_MN end", __FUNCTION__);
  printf ("\n%s timed_mul4096x4096_MN delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

  return (rc);
}

int
test_time_i128 (void)
{
  long i;
  uint64_t t_start, t_end, t_delta;
  double delta_sec;
  int rc = 0;

  printf ("\n%s mul10uq start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_mul10uq ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s mul10uq end", __FUNCTION__);
  printf ("\n%s mul10uq  tb delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

  printf ("\n%s cmul10ecuq start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_cmul10ecuq ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s cmul10ecuq end", __FUNCTION__);
  printf ("\n%s cmul10ecuq  tb delta = %lu, sec = %10.6g\n", __FUNCTION__,
	  t_delta, delta_sec);

  printf ("\n%s mulluq start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_mulluq ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s mulluq end", __FUNCTION__);
  printf ("\n%s mulluq  tb delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

  printf ("\n%s muludq start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_muludq ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s muludq end", __FUNCTION__);
  printf ("\n%s muludq  tb delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

  printf ("\n%s muludqx start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_muludqx ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s muludqx end", __FUNCTION__);
  printf ("\n%s muludqx  tb delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

  printf ("\n%s longdiv_e32 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_longdiv_e32 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s longdiv_e32 end", __FUNCTION__);
  printf ("\n%s longdiv_e32 tb delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

#ifndef PVECLIB_DISABLE_DFP
  printf ("\n%s longbcdcf_10e32 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_longbcdcf_10e32 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s longbcdcf_10e32 end", __FUNCTION__);
  printf ("\n%s longbcdcf_10e32 tb delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

  printf ("\n%s longbcdct_10e32 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_longbcdct_10e32 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);
#endif

  printf ("\n%s longbcdct_10e32 end", __FUNCTION__);
  printf ("\n%s longbcdct_10e32 tb delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

  printf ("\n%s cfmaxdouble_10e32 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_cfmaxdouble_10e32 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s cfmaxdouble_10e32 end", __FUNCTION__);
  printf ("\n%s cfmaxdouble_10e32 delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

  printf ("\n%s ctmaxdouble_10e32 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_ctmaxdouble_10e32 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s ctmaxdouble_10e32 end", __FUNCTION__);
  printf ("\n%s ctmaxdouble_10e32 delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

  return (rc);
}

int
test_time_f32 (void)
{
  long i;
  uint64_t t_start, t_end, t_delta;
  double delta_sec;
  int rc = 0;

  printf ("\n%s is_f32 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_is_f32 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s is_f32 end", __FUNCTION__);
  printf ("\n%s is_f32  tb delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

  printf ("\n%s fpclassify_f32 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_fpclassify_f32 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s fpclassify_f32 end", __FUNCTION__);
  printf ("\n%s fpclassify_f32  tb delta = %lu, sec = %10.6g\n", __FUNCTION__,
	  t_delta, delta_sec);

  // initialize the test array to the Identity matrix
  timed_setup_f32_transpose  ();

  printf ("\n%s scalar_transpose_f32 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_scalar_f32_transpose ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s scalar_transpose_f32 end", __FUNCTION__);
  printf ("\n%s scalar_transpose_f32  tb delta = %lu, sec = %10.6g\n", __FUNCTION__,
	  t_delta, delta_sec);

  printf ("\n%s gather_transpose_f32 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_gather_f32_transpose ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s gather_transpose_f32 end", __FUNCTION__);
  printf ("\n%s gather_transpose_f32  tb delta = %lu, sec = %10.6g\n", __FUNCTION__,
	  t_delta, delta_sec);

  printf ("\n%s gatherx2_transpose_f32 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_gatherx2_f32_transpose ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s gatherx2_transpose_f32 end", __FUNCTION__);
  printf ("\n%s gatherx2_transpose_f32  tb delta = %lu, sec = %10.6g\n", __FUNCTION__,
	  t_delta, delta_sec);

  printf ("\n%s gatherx4_transpose_f32 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_gatherx4_f32_transpose ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s gatherx4_transpose_f32 end", __FUNCTION__);
  printf ("\n%s gatherx4_transpose_f32  tb delta = %lu, sec = %10.6g\n", __FUNCTION__,
	  t_delta, delta_sec);

  return (rc);
}

int
test_time_f64 (void)
{
  long i;
  uint64_t t_start, t_end, t_delta;
  double delta_sec;
  int rc = 0;

  printf ("\n%s is_f64 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_is_f64 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s is_f64 end", __FUNCTION__);
  printf ("\n%s is_f64  tb delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

  printf ("\n%s fpclassify_f64 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_fpclassify_f64 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s fpclassify_f64 end", __FUNCTION__);
  printf ("\n%s fpclassify_f64  tb delta = %lu, sec = %10.6g\n", __FUNCTION__,
	  t_delta, delta_sec);


  // initialize the test array to the Identity matrix
  timed_setup_f64_transpose  ();

  printf ("\n%s scalar_transpose_f64 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_scalar_f64_transpose ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s scalar_transpose_f64 end", __FUNCTION__);
  printf ("\n%s scalar_transpose_f64  tb delta = %lu, sec = %10.6g\n", __FUNCTION__,
	  t_delta, delta_sec);

  printf ("\n%s gather_transpose_f64 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_gather_f64_transpose ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s gather_transpose_f64 end", __FUNCTION__);
  printf ("\n%s gather_transpose_f64  tb delta = %lu, sec = %10.6g\n", __FUNCTION__,
	  t_delta, delta_sec);

  printf ("\n%s gatherx2_transpose_f64 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_gatherx2_f64_transpose ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s gatherx2_transpose_f64 end", __FUNCTION__);
  printf ("\n%s gatherx2_transpose_f64  tb delta = %lu, sec = %10.6g\n", __FUNCTION__,
	  t_delta, delta_sec);

  printf ("\n%s gatherx4_transpose_f64 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_gatherx4_f64_transpose ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s gatherx4_transpose_f64 end", __FUNCTION__);
  printf ("\n%s gatherx4_transpose_f64  tb delta = %lu, sec = %10.6g\n", __FUNCTION__,
	  t_delta, delta_sec);

  return (rc);
}

#ifndef PVECLIB_DISABLE_F128ARITH
int
test_time_f128 (void)
{
  long i;
  uint64_t t_start, t_end, t_delta;
  double delta_sec;
  int rc = 0;

#if 0
  printf ("\n%s f128 CC start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_expxsuba_v1_f128 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s f128 CC end", __FUNCTION__);
  printf ("\n%s f128 CC tb delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

  printf ("\n%s f128_LIB start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_expxsuba_v2_f128 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s f128_LIB end", __FUNCTION__);
  printf ("\n%s f128_LIB tb delta = %lu, sec = %10.6g\n", __FUNCTION__,
	  t_delta, delta_sec);
#endif
  printf ("\n%s cmpgtuqp_gcc start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_gcc_max8_f128 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s cmpgtuqp_gcc end", __FUNCTION__);
  printf ("\n%s cmpgtuqp_gcc tb delta = %lu, sec = %10.6g\n", __FUNCTION__,
	  t_delta, delta_sec);

  printf ("\n%s cmpgtuqp_lib start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_lib_max8_f128 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s cmpgtuqp_lib end", __FUNCTION__);
  printf ("\n%s cmpgtuqp_lib tb delta = %lu, sec = %10.6g\n", __FUNCTION__,
	  t_delta, delta_sec);

  printf ("\n%s cmpgtuqp_vec start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_vec_max8_f128 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s cmpgtuqp_vec end", __FUNCTION__);
  printf ("\n%s cmpgtuqp_vec tb delta = %lu, sec = %10.6g\n", __FUNCTION__,
	  t_delta, delta_sec);

  printf ("\n%s cmpgtuzqp_vec start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_vec_max8_f128uz ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s cmpgtuzqp_vec end", __FUNCTION__);
  printf ("\n%s cmpgtuzqp_vec tb delta = %lu, sec = %10.6g\n", __FUNCTION__,
	  t_delta, delta_sec);

  printf ("\n%s cvt_dpqp_gcc start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_gcc_dpqp_f128 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s cvt_dpqp_gcc end", __FUNCTION__);
  printf ("\n%s cvt_dpqp_gcc tb delta = %lu, sec = %10.6g\n", __FUNCTION__,
	  t_delta, delta_sec);

  printf ("\n%s cvt_dpqp_lib start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_lib_dpqp_f128 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s cvt_dpqp_lib end", __FUNCTION__);
  printf ("\n%s cvt_dpqp_lib tb delta = %lu, sec = %10.6g\n", __FUNCTION__,
	  t_delta, delta_sec);

  printf ("\n%s cvt_dpqp_vec start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_vec_dpqp_f128 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s cvt_dpqp_vec end", __FUNCTION__);
  printf ("\n%s cvt_dpqp_vec tb delta = %lu, sec = %10.6g\n", __FUNCTION__,
	  t_delta, delta_sec);

  return (rc);
}
#endif

int
main (void)
{
  int rc = EXIT_SUCCESS;

  puts ("Power Vector Performance testsuite");

#if 1
  rc += test_time_f32 ();
#endif
#if 1
  rc += test_time_f64 ();
#endif
#ifdef PVECLIB_DISABLE_F128ARITH
  puts ("\ntest_time_f128 disabled for PVECLIB_DISABLE_F128ARITH\n");
#else
  rc += test_time_f128 ();
#endif
#if 1
  rc += test_time_i128 ();
#endif
#if 1
  rc += test_time_i512 ();
#endif

  if (rc > 0)
    printf ("%d failures reported\n", rc);
  return (rc);
}
