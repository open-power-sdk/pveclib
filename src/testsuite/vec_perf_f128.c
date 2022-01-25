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

#if 0 // turn off until Round-to-odd implementation is ready
extern __float128
test_scalar_add128 (__float128 vra, __float128 vrb);

extern __float128
test_scalar_div128 (__float128 vra, __float128 vrb);

extern __float128
test_scalar_mul128 (__float128 vra, __float128 vrb);

extern __float128
test_scalar_sub128 (__float128 vra, __float128 vrb);
#endif

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

const double f64_one = 1.0;
const double f64_fact2 = (1.0 / 2.0);
const double f64_fact3 = (1.0 / 6.0);
const double f64_fact4 = (1.0 / 24.0);
const double f64_fact5 = (1.0 / 120.0);
const double f64_fact6 = (1.0 / 720.0);
const double f64_fact7 = (1.0 / 5040.0);
const double f64_fact8 = (1.0 / 40320.0);
const double f64_fact9 = (1.0 / 362880.0);
const double f64_fact10 = (1.0 / 3628800.0);

const vf64_t invfact1_2 = {1.0, 2.0};
const vf64_t invfact3_4 = {6.0, 24.0};
const vf64_t invfact5_6 = {120.0, 720.0};
const vf64_t invfact7_8 = {5040.0, 40320.0};
const vf64_t invfact9_10 = {362880.0, 3628800.0};

const __float128 qpfact1 = 1.0Q;
const __float128 qpfact2 = 2.0Q;
const __float128 qpfact3 = 6.0Q;
const __float128 qpfact4 = 24.0Q;
const __float128 qpfact5 = 120.0Q;
const __float128 qpfact6 = 720.0Q;
const __float128 qpfact7 = 5040.0Q;
const __float128 qpfact8 = 40320.0Q;

const vui128_t i128fact1 = {1};
const vui128_t i128fact2 = {2};
const vui128_t i128fact3 = {6};
const vui128_t i128fact4 = {24};
const vui128_t i128fact5 = {120};
const vui128_t i128fact6 = {720};
const vui128_t i128fact7 = {5040};
const vui128_t i128fact8 = {40320};

#if 0 // turn off until Round-to-odd implementation is ready
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
#endif

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

extern void
test_gcc_dpqp_f128 (__binary128 * vf128,
		    vf64_t vf1, vf64_t vf2,
		    vf64_t vf3, vf64_t vf4,
		    vf64_t vf5);

extern void
test_vec_dpqp_f128 (__binary128 * vf128,
		    vf64_t vf1, vf64_t vf2,
		    vf64_t vf3, vf64_t vf4,
		    vf64_t vf5);

extern void
test_vec_uqqp_f128 (__binary128 * vf128,
		    vui128_t vf1, vui128_t vf2,
		    vui128_t vf3, vui128_t vf4,
		    vui128_t vf5, vui128_t vf6,
		    vui128_t vf7, vui128_t vf8);

extern void
test_gcc_uqqp_f128 (__binary128 * vf128,
		    vui128_t vf1, vui128_t vf2,
		    vui128_t vf3, vui128_t vf4,
		    vui128_t vf5, vui128_t vf6,
		    vui128_t vf7, vui128_t vf8);

extern void
test_vec_qpuq_f128 (vui128_t * vf128,
		    __binary128 vf1, __binary128 vf2,
		    __binary128 vf3, __binary128 vf4,
		    __binary128 vf5, __binary128 vf6,
		    __binary128 vf7, __binary128 vf8);

extern void
test_gcc_qpuq_f128 (vui128_t * vf128,
		    __binary128 vf1, __binary128 vf2,
		    __binary128 vf3, __binary128 vf4,
		    __binary128 vf5, __binary128 vf6,
		    __binary128 vf7, __binary128 vf8);

extern void
test_vec_qpdpo_f128 (vf64_t * vf128,
		    __binary128 vf1, __binary128 vf2,
		    __binary128 vf3, __binary128 vf4,
		    __binary128 vf5, __binary128 vf6,
		    __binary128 vf7, __binary128 vf8);

extern void
test_gcc_qpdpo_f128 (vf64_t * vf128,
		    __binary128 vf1, __binary128 vf2,
		    __binary128 vf3, __binary128 vf4,
		    __binary128 vf5, __binary128 vf6,
		    __binary128 vf7, __binary128 vf8);

extern void
test_gcc_mulqpo_f128 (__binary128 * vf128,
		    __binary128 vf1, __binary128 vf2,
		    __binary128 vf3, __binary128 vf4,
		    __binary128 vf5, __binary128 vf6,
		    __binary128 vf7, __binary128 vf8);

extern void
test_gcc_addqpn_f128 (__binary128 * vf128,
		    __binary128 vf1, __binary128 vf2,
		    __binary128 vf3, __binary128 vf4,
		    __binary128 vf5, __binary128 vf6,
		    __binary128 vf7, __binary128 vf8);

extern void
test_gcc_subqpn_f128 (__binary128 * vf128,
		    __binary128 vf1, __binary128 vf2,
		    __binary128 vf3, __binary128 vf4,
		    __binary128 vf5, __binary128 vf6,
		    __binary128 vf7, __binary128 vf8);

extern vb128_t
test_vec_cmpgtuqp (__binary128 vfa, __binary128 vfb);

extern __binary128
test_vec_xscvdpqp (vf64_t f64);

extern vf64_t
test_vec_xscvqpdpo (__binary128 f128);

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

void
test_lib_qpdpo_f128 (vf64_t * vx64,
		    __binary128 vf1, __binary128 vf2,
		    __binary128 vf3, __binary128 vf4,
		    __binary128 vf5, __binary128 vf6,
		    __binary128 vf7, __binary128 vf8)
{
  vf64_t vxf1, vxf2, vxf3, vxf4;

  vxf1 = test_vec_xscvqpdpo (vf1);
  vxf2 = test_vec_xscvqpdpo (vf2);
  vxf3 = test_vec_xscvqpdpo (vf3);
  vxf4 = test_vec_xscvqpdpo (vf4);

  vxf1[VEC_DW_L] = vxf2[VEC_DW_H];
  vxf3[VEC_DW_L] = vxf4[VEC_DW_H];

  vx64[0] = vxf1;
  vx64[1] = vxf3;

  vxf1 = test_vec_xscvqpdpo (vf5);
  vxf2 = test_vec_xscvqpdpo (vf6);
  vxf3 = test_vec_xscvqpdpo (vf7);
  vxf4 = test_vec_xscvqpdpo (vf8);

  vxf1[VEC_DW_L] = vxf2[VEC_DW_H];
  vxf3[VEC_DW_L] = vxf4[VEC_DW_H];

  vx64[2] = vxf1;
  vx64[3] = vxf3;
}

void
test_lib_dpqp_f128 (__binary128 * vf128,
		    vf64_t vf1, vf64_t vf2,
		    vf64_t vf3, vf64_t vf4,
		    vf64_t vf5)
{
  vf128[0] = test_vec_xscvdpqp (vf1);
  vf1[VEC_DW_H] = vf1[VEC_DW_L];
  vf128[1] = test_vec_xscvdpqp (vf1);

  vf128[2] = test_vec_xscvdpqp (vf2);
  vf2[VEC_DW_H] = vf2[VEC_DW_L];
  vf128[3] = test_vec_xscvdpqp (vf2);

  vf128[4] = test_vec_xscvdpqp (vf3);
  vf3[VEC_DW_H] = vf3[VEC_DW_L];
  vf128[5] = test_vec_xscvdpqp (vf3);

  vf128[6] = test_vec_xscvdpqp (vf4);
  vf4[VEC_DW_H] = vf4[VEC_DW_L];
  vf128[7] = test_vec_xscvdpqp (vf4);

  vf128[8] = test_vec_xscvdpqp (vf5);
  vf5[VEC_DW_H] = vf5[VEC_DW_L];
  vf128[8] = test_vec_xscvdpqp (vf5);
}

extern vui128_t
test_vec_xscvqpuqz (__binary128 f128);

void
test_lib_qpuq_f128 (vui128_t * vf128,
		    __binary128 vf1, __binary128 vf2,
		    __binary128 vf3, __binary128 vf4,
		    __binary128 vf5, __binary128 vf6,
		    __binary128 vf7, __binary128 vf8)
{
  vf128[0] = test_vec_xscvqpuqz (vf1);
  vf128[1] = test_vec_xscvqpuqz (vf2);
  vf128[2] = test_vec_xscvqpuqz (vf3);
  vf128[3] = test_vec_xscvqpuqz (vf4);
  vf128[4] = test_vec_xscvqpuqz (vf5);
  vf128[5] = test_vec_xscvqpuqz (vf6);
  vf128[6] = test_vec_xscvqpuqz (vf7);
  vf128[7] = test_vec_xscvqpuqz (vf8);
}

extern __binary128
test_vec_xscvuqqp (vui128_t int128);

void
test_lib_uqqp_f128 (__binary128 * vf128,
		    vui128_t vf1, vui128_t vf2,
		    vui128_t vf3, vui128_t vf4,
		    vui128_t vf5, vui128_t vf6,
		    vui128_t vf7, vui128_t vf8)
{
  vf128[0] = test_vec_xscvuqqp (vf1);
  vf128[1] = test_vec_xscvuqqp (vf2);
  vf128[2] = test_vec_xscvuqqp (vf3);
  vf128[3] = test_vec_xscvuqqp (vf4);
  vf128[4] = test_vec_xscvuqqp (vf5);
  vf128[5] = test_vec_xscvuqqp (vf6);
  vf128[6] = test_vec_xscvuqqp (vf7);
  vf128[7] = test_vec_xscvuqqp (vf8);
}

extern __binary128 test_vec_addqpo (__binary128 vfa, __binary128 vfb);


void
test_lib_addqpo_f128 (__binary128 * vf128,
		      __binary128 vf1, __binary128 vf2,
		      __binary128 vf3, __binary128 vf4,
		      __binary128 vf5, __binary128 vf6,
		      __binary128 vf7, __binary128 vf8)
{
  __binary128 result;
  result = test_vec_addqpo (qpfact1, vf1);
  result = test_vec_addqpo (result, vf2);
  result = test_vec_addqpo (result, vf3);
  result = test_vec_addqpo (result, vf4);
  result = test_vec_addqpo (result, vf5);
  result = test_vec_addqpo (result, vf6);
  result = test_vec_addqpo (result, vf7);
  result = test_vec_addqpo (result, vf8);
  *vf128 = result;
}

extern __binary128 test_vec_subqpo (__binary128 vfa, __binary128 vfb);


void
test_lib_subqpo_f128 (__binary128 * vf128,
		      __binary128 vf1, __binary128 vf2,
		      __binary128 vf3, __binary128 vf4,
		      __binary128 vf5, __binary128 vf6,
		      __binary128 vf7, __binary128 vf8)
{
  __binary128 result;
  result = test_vec_subqpo (qpfact1, vf1);
  result = test_vec_subqpo (result, vf2);
  result = test_vec_subqpo (result, vf3);
  result = test_vec_subqpo (result, vf4);
  result = test_vec_subqpo (result, vf5);
  result = test_vec_subqpo (result, vf6);
  result = test_vec_subqpo (result, vf7);
  result = test_vec_subqpo (result, vf8);
  *vf128 = result;
}

extern __binary128 test_vec_mulqpo (__binary128 vfa, __binary128 vfb);


void
test_lib_mulqpo_f128 (__binary128 * vf128,
		      __binary128 vf1, __binary128 vf2,
		      __binary128 vf3, __binary128 vf4,
		      __binary128 vf5, __binary128 vf6,
		      __binary128 vf7, __binary128 vf8)
{
  __binary128 result;
  result = test_vec_mulqpo (qpfact1, vf1);
  result = test_vec_mulqpo (result, vf2);
  result = test_vec_mulqpo (result, vf3);
  result = test_vec_mulqpo (result, vf4);
  result = test_vec_mulqpo (result, vf5);
  result = test_vec_mulqpo (result, vf6);
  result = test_vec_mulqpo (result, vf7);
  result = test_vec_mulqpo (result, vf8);
  *vf128 = result;
}

extern __binary128 test_vec_mulqpn (__binary128 vfa, __binary128 vfb);


void
test_lib_mulqpn_f128 (__binary128 * vf128,
		      __binary128 vf1, __binary128 vf2,
		      __binary128 vf3, __binary128 vf4,
		      __binary128 vf5, __binary128 vf6,
		      __binary128 vf7, __binary128 vf8)
{
  __binary128 result;
  result = test_vec_mulqpn (qpfact1, vf1);
  result = test_vec_mulqpn (result, vf2);
  result = test_vec_mulqpn (result, vf3);
  result = test_vec_mulqpn (result, vf4);
  result = test_vec_mulqpn (result, vf5);
  result = test_vec_mulqpn (result, vf6);
  result = test_vec_mulqpn (result, vf7);
  result = test_vec_mulqpn (result, vf8);
  *vf128 = result;
}

int timed_lib_addqpo_f128 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  __binary128 tbl[10];
  int i;

  for (i=0; i<N; i++)
    {
      test_lib_addqpo_f128 (tbl,
			  qpfact1, qpfact2,
			  qpfact3, qpfact4,
			  qpfact5, qpfact6,
			  qpfact7, qpfact8);
    }
#endif
   return 0;
}

int timed_lib_subqpo_f128 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  __binary128 tbl[10];
  int i;

  for (i=0; i<N; i++)
    {
      test_lib_subqpo_f128 (tbl,
			  qpfact1, qpfact2,
			  qpfact3, qpfact4,
			  qpfact5, qpfact6,
			  qpfact7, qpfact8);
    }
#endif
   return 0;
}

int timed_lib_mulqpo_f128 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  __binary128 tbl[10];
  int i;

  for (i=0; i<N; i++)
    {
      test_lib_mulqpo_f128 (tbl,
			  qpfact1, qpfact2,
			  qpfact3, qpfact4,
			  qpfact5, qpfact6,
			  qpfact7, qpfact8);
    }
#endif
   return 0;
}

int timed_lib_mulqpn_f128 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  __binary128 tbl[10];
  int i;

  for (i=0; i<N; i++)
    {
      test_lib_mulqpn_f128 (tbl,
			  qpfact1, qpfact2,
			  qpfact3, qpfact4,
			  qpfact5, qpfact6,
			  qpfact7, qpfact8);
    }
#endif
   return 0;
}

int timed_gcc_addqpn_f128 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  __binary128 tbl[10];
  int i;

  for (i=0; i<N; i++)
    {
      test_gcc_addqpn_f128 (tbl,
			  qpfact1, qpfact2,
			  qpfact3, qpfact4,
			  qpfact5, qpfact6,
			  qpfact7, qpfact8);
    }
#endif
   return 0;
}

int timed_gcc_subqpn_f128 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  __binary128 tbl[10];
  int i;

  for (i=0; i<N; i++)
    {
      test_gcc_subqpn_f128 (tbl,
			  qpfact1, qpfact2,
			  qpfact3, qpfact4,
			  qpfact5, qpfact6,
			  qpfact7, qpfact8);
    }
#endif
   return 0;
}

int timed_gcc_mulqpo_f128 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  __binary128 tbl[10];
  int i;

  for (i=0; i<N; i++)
    {
      test_gcc_mulqpo_f128 (tbl,
			  qpfact1, qpfact2,
			  qpfact3, qpfact4,
			  qpfact5, qpfact6,
			  qpfact7, qpfact8);
    }
#endif
   return 0;
}

int timed_lib_qpdpo_f128 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  vf64_t tbl[10];
  int i;

  for (i=0; i<N; i++)
    {
      test_lib_qpdpo_f128 (tbl,
			  qpfact1, qpfact2,
			  qpfact3, qpfact4,
			  qpfact5, qpfact6,
			  qpfact7, qpfact8);
    }
#endif
   return 0;
}

int timed_gcc_qpdpo_f128 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  vf64_t tbl[10];
  int i;

  for (i=0; i<N; i++)
    {
      test_gcc_qpdpo_f128 (tbl,
			  qpfact1, qpfact2,
			  qpfact3, qpfact4,
			  qpfact5, qpfact6,
			  qpfact7, qpfact8);
    }
#endif
   return 0;
}

int timed_vec_qpdpo_f128 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  vf64_t tbl[10];
  int i;

  for (i=0; i<N; i++)
    {
      test_vec_qpdpo_f128 (tbl,
			  qpfact1, qpfact2,
			  qpfact3, qpfact4,
			  qpfact5, qpfact6,
			  qpfact7, qpfact8);
    }
#endif
   return 0;
}

int timed_vec_qpuq_f128 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  vui128_t tbl[10];
  int i;

  for (i=0; i<N; i++)
    {
      test_vec_qpuq_f128 (tbl,
			  qpfact1, qpfact2,
			  qpfact3, qpfact4,
			  qpfact5, qpfact6,
			  qpfact7, qpfact8);
    }
#endif
   return 0;
}

int timed_lib_qpuq_f128 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  vui128_t tbl[10];
  int i;

  for (i=0; i<N; i++)
    {
      test_lib_qpuq_f128 (tbl,
			  qpfact1, qpfact2,
			  qpfact3, qpfact4,
			  qpfact5, qpfact6,
			  qpfact7, qpfact8);
    }
#endif
   return 0;
}

int timed_gcc_qpuq_f128 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  vui128_t tbl[10];
  int i;

  for (i=0; i<N; i++)
    {
      test_gcc_qpuq_f128 (tbl,
			  qpfact1, qpfact2,
			  qpfact3, qpfact4,
			  qpfact5, qpfact6,
			  qpfact7, qpfact8);
    }
#endif
   return 0;
}

int timed_vec_uqqp_f128 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  __float128 tbl[10];
  int i;

  for (i=0; i<N; i++)
    {
      test_vec_uqqp_f128 (tbl,
			  i128fact1, i128fact2,
			  i128fact3, i128fact4,
			  i128fact5, i128fact6,
			  i128fact7, i128fact8);
    }
#endif
   return 0;
}

int timed_lib_uqqp_f128 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  __float128 tbl[10];
  int i;

  for (i=0; i<N; i++)
    {
      test_lib_uqqp_f128 (tbl,
			  i128fact1, i128fact2,
			  i128fact3, i128fact4,
			  i128fact5, i128fact6,
			  i128fact7, i128fact8);
    }
#endif
   return 0;
}

int timed_gcc_uqqp_f128 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  __float128 tbl[10];
  int i;

  for (i=0; i<N; i++)
    {
      test_gcc_uqqp_f128 (tbl,
			  i128fact1, i128fact2,
			  i128fact3, i128fact4,
			  i128fact5, i128fact6,
			  i128fact7, i128fact8);
    }
#endif
   return 0;
}

int timed_vec_dpqp_f128 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  __float128 tbl[10];
  int i;

  for (i=0; i<N; i++)
    {
      test_vec_dpqp_f128 (tbl, invfact1_2,
			  invfact3_4, invfact5_6,
			  invfact7_8, invfact9_10);
    }
#endif
   return 0;
}

int timed_lib_dpqp_f128 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  __float128 tbl[10];
  int i;

  for (i=0; i<N; i++)
    {
      test_lib_dpqp_f128 (tbl, invfact1_2,
			  invfact3_4, invfact5_6,
			  invfact7_8, invfact9_10);
    }
#endif
   return 0;
}

int timed_gcc_dpqp_f128 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  __float128 tbl[10];
  int i;

  for (i=0; i<N; i++)
    {
      test_gcc_dpqp_f128 (tbl, invfact1_2,
			  invfact3_4, invfact5_6,
			  invfact7_8, invfact9_10);
    }
#endif
   return 0;
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

#if 0 // turn off until Round-to-odd implementation is ready
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
#endif
