/*
 Copyright (c) [2021] Steven Munroe

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 vec_perf_f128.c

 Contributors: Steven Munroe
 Created on: Apr 2, 2021
 */

#include <stdint.h>
#include <stdio.h>
#include <float.h>

#include <pveclib/vec_int128_ppc.h>
#include <pveclib/vec_f128_ppc.h>
#include <testsuite/arith128_print.h>
#include <testsuite/vec_perf_f128.h>

#ifdef __FLOAT128__
#ifndef PVECLIB_DISABLE_F128ARITH

extern __float128
test_scalar_add128 (__float128 vra, __float128 vrb);

extern __float128
test_scalar_div128 (__float128 vra, __float128 vrb);

extern __float128
test_scalar_mul128 (__float128 vra, __float128 vrb);

extern __float128
test_scalar_sub128 (__float128 vra, __float128 vrb);

extern __float128
test_scalarCC_expxsuba_128 (__float128 x, __float128 a, __float128 expa);

const __float128 f128_one = 1.0Q;
const __float128 f128_e = 2.71828182845904523536028747135266249775724709369996Q;
const __float128 inv_fact2 = (1.0Q / 2.0Q);
const __float128 inv_fact3 = (1.0Q / 6.0Q);
const __float128 inv_fact4 = (1.0Q / 24.0Q);
const __float128 inv_fact5 = (1.0Q / 120.0Q);
const __float128 inv_fact6 = (1.0Q / 720.0Q);
const __float128 inv_fact7 = (1.0Q / 5040.0Q);
const __float128 inv_fact8 = (1.0Q / 40320.0Q);

__float128
test_scalarLib_expxsuba_128 (__float128 x, __float128 a, __float128 expa)
{
  __float128 term, xma, xma2, xmaf2;
  __float128 xma3, xmaf3, xma4, xmaf4, xma5, xmaf5;
  __float128 xma6, xmaf6, xma7, xmaf7, xma8, xmaf8;

  // 1st 8 terms of e**x = e**a * taylor( x-a )
  xma = test_scalar_sub128 (x, a);
  term = test_scalar_add128 (f128_one, xma);
  xma2 = test_scalar_mul128 (xma, xma);
  xmaf2 = test_scalar_mul128 (xma2, inv_fact2);
  term = test_scalar_add128 (term, xmaf2);
  xma3 = test_scalar_mul128 (xma2, xma);
  xmaf3 = test_scalar_mul128 (xma3, inv_fact3);
  term = test_scalar_add128 (term, xmaf3);
  xma4 = test_scalar_mul128 (xma3, xma);
  xmaf4 = test_scalar_mul128 (xma4, inv_fact4);
  term = test_scalar_add128 (term, xmaf4);
  xma5 = test_scalar_mul128 (xma4, xma);
  xmaf5 = test_scalar_mul128 (xma5, inv_fact5);
  term = test_scalar_add128 (term, xmaf5);
  xma6 = test_scalar_mul128 (xma5, xma);
  xmaf6 = test_scalar_mul128 (xma6, inv_fact6);
  term = test_scalar_add128 (term, xmaf6);
  xma7 = test_scalar_mul128 (xma6, xma);
  xmaf7 = test_scalar_mul128 (xma7, inv_fact7);
  term = test_scalar_add128 (term, xmaf7);
  xma8 = test_scalar_mul128 (xma7, xma);
  xmaf8 = test_scalar_mul128 (xma8, inv_fact8);
  term = test_scalar_add128 (term, xmaf8);
  return test_scalar_mul128(expa, term);;
}

#define N 10
#define TIMING_ITERATIONS 10

extern __binary128
test_vec_max8_f128 (__binary128 vf1, __binary128 vf2,
		    __binary128 vf3, __binary128 vf4,
		    __binary128 vf5, __binary128 vf6,
		    __binary128 vf7, __binary128 vf8);

extern __binary128
test_vec_max8_f128uz (__binary128 vf1, __binary128 vf2,
		    __binary128 vf3, __binary128 vf4,
		    __binary128 vf5, __binary128 vf6,
		    __binary128 vf7, __binary128 vf8);

extern __binary128
test_gcc_max8_f128 (__binary128 vf1, __binary128 vf2,
		    __binary128 vf3, __binary128 vf4,
		    __binary128 vf5, __binary128 vf6,
		    __binary128 vf7, __binary128 vf8);

extern vb128_t
test_vec_cmpgtuqp (__binary128 vfa, __binary128 vfb);

__binary128
test_lib_max8_f128 (__binary128 vf1, __binary128 vf2,
		    __binary128 vf3, __binary128 vf4,
		    __binary128 vf5, __binary128 vf6,
		    __binary128 vf7, __binary128 vf8)
{
  __binary128 maxres;
  vb128_t bool;

  bool = test_vec_cmpgtuqp (vf2, vf1);
  maxres = vec_self128 (vf1, vf2, bool);
  bool = test_vec_cmpgtuqp (vf3, maxres);
  maxres = vec_self128 (vf3, maxres, bool);
  bool = test_vec_cmpgtuqp (vf4, maxres);
  maxres = vec_self128 (vf4, maxres, bool);
  bool = test_vec_cmpgtuqp (vf5, maxres);
  maxres = vec_self128 (vf5, maxres, bool);
  bool = test_vec_cmpgtuqp (vf6, maxres);
  maxres = vec_self128 (vf6, maxres, bool);
  bool = test_vec_cmpgtuqp (vf7, maxres);
  maxres = vec_self128 (vf7, maxres, bool);
  bool = test_vec_cmpgtuqp (vf8, maxres);
  maxres = vec_self128 (vf8, maxres, bool);

  return maxres;
}

int timed_lib_max8_f128 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  __float128 accum = 0.0Q;
  int i;

  for (i=0; i<N; i++)
    {
      accum += test_lib_max8_f128 (f128_e, inv_fact2,
				   inv_fact3, inv_fact4,
				   inv_fact5, inv_fact6,
				   inv_fact7, inv_fact8);
    }
#endif
   return 0;
}

int timed_gcc_max8_f128 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  __float128 accum = 0.0Q;
  int i;

  for (i=0; i<N; i++)
    {
      accum += test_gcc_max8_f128 (f128_e, inv_fact2,
				   inv_fact3, inv_fact4,
				   inv_fact5, inv_fact6,
				   inv_fact7, inv_fact8);
    }
#endif
   return 0;
}

int timed_vec_max8_f128 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  __float128 accum = 0.0Q;
  int i;

  for (i=0; i<N; i++)
    {
      accum += test_vec_max8_f128 (f128_e, inv_fact2,
				   inv_fact3, inv_fact4,
				   inv_fact5, inv_fact6,
				   inv_fact7, inv_fact8);
    }
#endif
   return 0;
}

int timed_vec_max8_f128uz (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  __float128 accum = 0.0Q;
  int i;

  for (i=0; i<N; i++)
    {
      accum += test_vec_max8_f128uz (f128_e, inv_fact2,
				   inv_fact3, inv_fact4,
				   inv_fact5, inv_fact6,
				   inv_fact7, inv_fact8);
    }
#endif
   return 0;
}

int timed_expxsuba_v1_f128 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  __float128 accum = 0.0Q;
  int i;

  for (i=0; i<N; i++)
    {
      accum += test_scalarCC_expxsuba_128 (f128_one, f128_one, f128_e);
    }
#endif
   return 0;
}

int timed_expxsuba_v2_f128 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  __float128 accum = 0.0Q;
  int i;

  for (i=0; i<N; i++)
    {
      accum += test_scalarLib_expxsuba_128 (f128_one, f128_one, f128_e);
    }
#endif
   return 0;
}

#endif
#endif
