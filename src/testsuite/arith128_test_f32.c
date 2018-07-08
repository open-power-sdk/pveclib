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

 arith128_test_f32.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Jul 3, 2018
 */

#include <stdint.h>
#include <stdio.h>
#include <float.h>

#include "arith128.h"
#include <testsuite/arith128_print.h>
#include "vec_int32_ppc.h"
#include "vec_f32_ppc.h"
#include <testsuite/arith128_test_f32.h>
#include <testsuite/vec_perf_f32.h>

#define __FLOAT_INF (0x7f800000)
#define __FLOAT_NINF (0xff800000)
#define __FLOAT_NAN (0x7f800001)
#define __FLOAT_NNAN (0xff800001)
#define __FLOAT_SNAN (0x7fC00001)
#define __FLOAT_NSNAN (0xffC00001)
#define __FLOAT_TRUE (0xffffffff)
#define __FLOAT_NTRUE (0x00000000)


#ifdef __DEBUG_PRINT__
static inline vb32_t
db_vec_isnormalf32 (vf32_t vf32)
{
	vui32_t tmp, tmp2;
	const vui32_t expmask  = CONST_VINT128_W(0x7f800000, 0x7f800000, 0x7f800000, 0x7f800000);
	const vui32_t vec_denorm = CONST_VINT128_W(0x00800000, 0x00800000, 0x00800000, 0x00800000);
	vb32_t result;

	print_v4f32x ("db_vec_isnormalf32:", vf32);

#if _ARCH_PWR7
	/* Eliminate const load. */
	tmp2 = (vui32_t)vec_abs (vf32);
#else
	const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000, 0x80000000);
	tmp2 = vec_andc ((vui32_t)vf32, signmask);
#endif
	print_vint128x ("              tmp2=", (vui128_t)tmp2);
	tmp = vec_and ((vui32_t)vf32, expmask);
	print_vint128x ("              tmp =", (vui128_t)tmp);
	tmp2 = (vui32_t)vec_cmplt(tmp2, vec_denorm);
	print_vint128x ("              tmp2=", (vui128_t)tmp2);
	tmp = (vui32_t)vec_cmpeq(tmp, expmask);
	print_vint128x ("              tmp =", (vui128_t)tmp);
	result = (vb32_t)vec_nor (tmp, tmp2);
	print_v4b32x   ("            result=", result);

	return (result);
}
#endif

int
test_float_abs (void)
{
  vf32_t i, e;
  vf32_t k;
  int rc = 0;

  printf ("\n%s float absolute value\n", __FUNCTION__);

  i = (vf32_t) { 0.0, -0.0, 0.0, -0.0 };
  e = (vf32_t) { 0.0, 0.0, 0.0, 0.0 };
  k = vec_absf32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_absf32 i=", i);
  print_v4f32x ("           k=", k);
#endif
  rc += check_v4f32x ("vec_absf32 1:", k, e);

  i = (vf32_t) { -(__FLT_MAX__), __FLT_MIN__, __FLT_EPSILON__,
		  -(__FLT_DENORM_MIN__) };
  e = (vf32_t) { __FLT_MAX__, __FLT_MIN__, __FLT_EPSILON__,
		  __FLT_DENORM_MIN__ };
  k = vec_absf32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_absf32 i=", i);
  print_v4f32x ("           k=", k);
#endif
  rc += check_v4f32x ("vec_absf32 1:", k, e);

  i = (vf32_t) CONST_VINT128_W(__FLOAT_INF, __FLOAT_NINF, __FLOAT_INF,
			       __FLOAT_NINF);
  e = (vf32_t) CONST_VINT128_W(__FLOAT_INF, __FLOAT_INF, __FLOAT_INF,
			       __FLOAT_INF);
  k = vec_absf32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_absf32 i=", i);
  print_v4f32x ("           k=", k);
#endif
  rc += check_v4f32x ("vec_absf32 1:", k, e);

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, __FLOAT_NNAN, __FLOAT_SNAN,
			       __FLOAT_NSNAN);
  e = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, __FLOAT_NAN, __FLOAT_SNAN,
			       __FLOAT_SNAN);
  k = vec_absf32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_absf32 i=", i);
  print_v4f32x ("           k=", k);
#endif
  rc += check_v4f32x ("vec_absf32 1:", k, e);

  return (rc);
}

int
test_float_all_is (void)
{
  vf32_t i;
  int rc = 0;

  printf ("\n%s float is all infinity\n", __FUNCTION__);

  i = (vf32_t) { 0.0, -0.0, 0.0, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_isinf i=", i);
#endif
  if (vec_all_isinff32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_isinf fail", i);
    } else {
    }

  i = (vf32_t) { __FLT_MAX__, __FLT_MIN__, __FLT_EPSILON__,
		  __FLT_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_isinf i=", i);
#endif
  if (vec_all_isinff32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_isinf fail", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_INF, __FLOAT_NINF, 0,
			       __FLOAT_NINF);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_isinf i=", i);
#endif
  if (vec_all_isinff32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_isinf fail", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_INF, __FLOAT_NINF, __FLOAT_INF,
			       __FLOAT_NINF);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_isinf i=", i);
#endif
  if (vec_all_isinff32 (i))
    {
    } else {
      rc += 1;
      print_v4f32x ("vec_all_isinf fail", i);
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, __FLOAT_NAN, __FLOAT_SNAN,
			       __FLOAT_SNAN);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_isinf i=", i);
#endif
  if (vec_all_isinff32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_isinf fail", i);
    } else {
    }

  printf ("\n%s float is all NaN\n", __FUNCTION__);

  i = (vf32_t) { 0.0, -0.0, 0.0, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_isnan i=", i);
#endif
  if (vec_all_isnanf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_isnan fail", i);
    } else {
    }

  i = (vf32_t) { __FLT_MAX__, __FLT_MIN__, __FLT_EPSILON__,
		  __FLT_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_isnan i=", i);
#endif
  if (vec_all_isnanf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_isnan fail", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, __FLOAT_NNAN, 0,
			       __FLOAT_SNAN);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_isnan i=", i);
#endif
  if (vec_all_isnanf32 (i))
    {
    } else {
      rc += 1;
      print_v4f32x ("vec_all_isnan fail", i);
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_INF, __FLOAT_NINF, __FLOAT_INF,
			       __FLOAT_NINF);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_isnan i=", i);
#endif
  if (vec_all_isnanf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_isnan fail", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, __FLOAT_NNAN, __FLOAT_NSNAN,
			       __FLOAT_SNAN);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_isnan i=", i);
#endif
  if (vec_all_isnanf32 (i))
    {
    } else {
	      rc += 1;
	      print_v4f32x ("vec_all_isnan fail", i);
    }

  printf ("\n%s float is all Normal\n", __FUNCTION__);

  i = (vf32_t) { 0.0, -0.0, 0.0, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_isnormal i=", i);
#endif
  if (vec_all_isnormalf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_isnormal fail", i);
    } else {
    }

  i = (vf32_t) { __FLT_MAX__, __FLT_MIN__, __FLT_EPSILON__,
		  -0.0 };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_isnormal i=", i);
#endif
  if (vec_all_isnormalf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_isnormal fail", i);
    } else {
    }

  i = (vf32_t) { __FLT_MAX__, __FLT_MIN__, __FLT_EPSILON__,
		  -1.0 };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_isnormal i=", i);
#endif
  if (vec_all_isnormalf32 (i))
    {
    } else {
      rc += 1;
      print_v4f32x ("vec_all_isnormal fail", i);
    }

  i = (vf32_t) { __FLT_MAX__, __FLT_MIN__, __FLT_EPSILON__,
		  __FLT_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_isnormal i=", i);
#endif
  if (vec_all_isnormalf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_isnormal fail", i);
    } else {
    }

  i = (vf32_t) { __FLT_DENORM_MIN__, __FLT_DENORM_MIN__, __FLT_DENORM_MIN__,
		  __FLT_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_isnormal i=", i);
#endif
  if (vec_all_isnormalf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_isnormal fail", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(0, 0xff000000, 1,
		__FLOAT_INF );

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_isnormal i=", i);
#endif
  if (vec_all_isnormalf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_isnormal fail", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NNAN, 0xff000000, 1,
		0x80000000);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_isnormal i=", i);
#endif
  if (vec_all_isnormalf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_isnormal fail", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_INF, __FLOAT_NINF, __FLOAT_INF,
			       __FLOAT_NINF);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_isnormal i=", i);
#endif
  if (vec_all_isnormalf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_isnormal fail", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, __FLOAT_NNAN, __FLOAT_NSNAN,
			       __FLOAT_SNAN);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_isnormal i=", i);
#endif
  if (vec_all_isnormalf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_isnormal fail", i);
    } else {
    }

  printf ("\n%s float is all Subnormal\n", __FUNCTION__);

  i = (vf32_t) { 0.0, -0.0, 0.0, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_issubnormal i=", i);
#endif
  if (vec_all_issubnormalf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_issubnormal fail", i);
    } else {
    }

  i = (vf32_t) { __FLT_MAX__, __FLT_MIN__, __FLT_EPSILON__,
		  -0.0 };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_issubnormal i=", i);
#endif
  if (vec_all_issubnormalf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_issubnormal fail", i);
    } else {
    }

  i = (vf32_t) { __FLT_MAX__, __FLT_MIN__, __FLT_EPSILON__,
		  __FLT_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_issubnormal i=", i);
#endif
  if (vec_all_issubnormalf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_issubnormal fail", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(0x80000001, 0x007fffff, 1,
			       0x803fffff );

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_issubnormal i=", i);
#endif
  if (vec_all_issubnormalf32 (i))
    {
    } else {
      rc += 1;
      print_v4f32x ("vec_all_issubnormal fail", i);
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NNAN, 0xff000000, 1,
		0x80000000);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_issubnormal i=", i);
#endif
  if (vec_all_issubnormalf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_issubnormal fail", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_INF, __FLOAT_NINF, __FLOAT_INF,
			       __FLOAT_NINF);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_issubnormal i=", i);
#endif
  if (vec_all_issubnormalf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_issubnormal fail", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, __FLOAT_NNAN, __FLOAT_NSNAN,
			       __FLOAT_SNAN);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_issubnormal i=", i);
#endif
  if (vec_all_issubnormalf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_issubnormal fail", i);
    } else {
    }

  printf ("\n%s float is all Zero\n", __FUNCTION__);

  i = (vf32_t) { 0.0, -0.0, 0.0, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_iszero i=", i);
#endif
  if (vec_all_iszerof32 (i))
    {
    } else {
      rc += 1;
      print_v4f32x ("vec_all_iszero fail", i);
    }

  i = (vf32_t) { __FLT_MAX__, __FLT_MIN__, __FLT_EPSILON__,
		  -0.0 };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_iszero i=", i);
#endif
  if (vec_all_iszerof32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_iszero fail", i);
    } else {
    }

  i = (vf32_t) { __FLT_MAX__, __FLT_MIN__, __FLT_EPSILON__,
		  __FLT_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_iszero i=", i);
#endif
  if (vec_all_iszerof32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_iszero fail", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(0x80000001, 0x007fffff, 1,
			       0x803fffff );

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_iszero i=", i);
#endif
  if (vec_all_iszerof32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_iszero fail", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NNAN, 0xff000000, 1,
		0x80000000);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_iszero i=", i);
#endif
  if (vec_all_iszerof32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_iszero fail", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_INF, __FLOAT_NINF, __FLOAT_INF,
			       __FLOAT_NINF);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_iszero i=", i);
#endif
  if (vec_all_iszerof32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_iszero fail", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, __FLOAT_NNAN, __FLOAT_NSNAN,
			       __FLOAT_SNAN);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_iszero i=", i);
#endif
  if (vec_all_iszerof32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_iszero fail", i);
    } else {
    }

  return (rc);
}

int
test_float_any_is (void)
{
  vf32_t i;
  int rc = 0;

  printf ("\n%s float is any infinity\n", __FUNCTION__);

  i = (vf32_t) { 0.0, -0.0, 0.0, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_isinf i=", i);
#endif
  if (vec_any_isinff32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_any_isinf fail", i);
    } else {
    }

  i = (vf32_t) { __FLT_MAX__, __FLT_MIN__, __FLT_EPSILON__,
		  __FLT_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_isinf i=", i);
#endif
  if (vec_any_isinff32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_any_isinf fail", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_INF, __FLOAT_NINF, 0,
			       __FLOAT_NINF);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_isinf i=", i);
#endif
  if (vec_any_isinff32 (i))
    {
    } else {
      rc += 1;
      print_v4f32x ("vec_any_isinf fail", i);
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_INF, __FLOAT_NINF, __FLOAT_INF,
			       __FLOAT_NINF);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_isinf i=", i);
#endif
  if (vec_any_isinff32 (i))
    {
    } else {
      rc += 1;
      print_v4f32x ("vec_any_isinf fail", i);
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, __FLOAT_NAN, __FLOAT_SNAN,
			       __FLOAT_SNAN);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_isinf i=", i);
#endif
  if (vec_any_isinff32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_any_isinf fail", i);
    } else {
    }

  printf ("\n%s float is any NaN\n", __FUNCTION__);

  i = (vf32_t) { 0.0, -0.0, 0.0, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_isnan i=", i);
#endif
  if (vec_any_isnanf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_any_isnan fail", i);
    } else {
    }

  i = (vf32_t) { __FLT_MAX__, __FLT_MIN__, __FLT_EPSILON__,
		  __FLT_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_isnan i=", i);
#endif
  if (vec_any_isnanf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_any_isnan fail", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, __FLOAT_NNAN, 0,
			       __FLOAT_SNAN);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_isnan i=", i);
#endif
  if (vec_any_isnanf32 (i))
    {
    } else {
      rc += 1;
      print_v4f32x ("vec_any_isnan fail", i);
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_INF, __FLOAT_NINF, __FLOAT_INF,
			       __FLOAT_NINF);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_isnan i=", i);
#endif
  if (vec_any_isnanf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_any_isnan fail", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, __FLOAT_NNAN, __FLOAT_NSNAN,
			       __FLOAT_SNAN);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_isnan i=", i);
#endif
  if (vec_any_isnanf32 (i))
    {
    } else {
	      rc += 1;
	      print_v4f32x ("vec_any_isnan fail", i);
    }

  printf ("\n%s float is any Normal\n", __FUNCTION__);

  i = (vf32_t) { 0.0, -0.0, 0.0, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_isnormal i=", i);
#endif
  if (vec_any_isnormalf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_any_isnormal fail", i);
    } else {
    }

  i = (vf32_t) { __FLT_MAX__, __FLT_MIN__, __FLT_EPSILON__,
		  -0.0 };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_isnormal i=", i);
#endif
  if (vec_any_isnormalf32 (i))
    {
    } else {
      rc += 1;
      print_v4f32x ("vec_any_isnormal fail", i);
    }

  i = (vf32_t) { __FLT_MAX__, __FLT_MIN__, __FLT_EPSILON__,
		  __FLT_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_isnormal i=", i);
#endif
  if (vec_any_isnormalf32 (i))
    {
    } else {
      rc += 1;
      print_v4f32x ("vec_any_isnormal fail", i);
    }

  i = (vf32_t) { __FLT_DENORM_MIN__, __FLT_DENORM_MIN__, __FLT_DENORM_MIN__,
		  __FLT_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_isnormal i=", i);
#endif
  if (vec_any_isnormalf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_any_isnormal fail", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(0, 0xff000000, 1,
		__FLOAT_INF );

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_isnormal i=", i);
#endif
  if (vec_any_isnormalf32 (i))
    {
    } else {
      rc += 1;
      print_v4f32x ("vec_any_isnormal fail", i);
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NNAN, 0xff000000, 1,
		0x80000000);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_isnormal i=", i);
#endif
  if (vec_any_isnormalf32 (i))
    {
    } else {
      rc += 1;
      print_v4f32x ("vec_any_isnormal fail", i);
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_INF, __FLOAT_NINF, __FLOAT_INF,
			       __FLOAT_NINF);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_isnormal i=", i);
#endif
  if (vec_any_isnormalf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_any_isnormal fail", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, __FLOAT_NNAN, __FLOAT_NSNAN,
			       __FLOAT_SNAN);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_isnormal i=", i);
#endif
  if (vec_any_isnormalf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_any_isnormal fail", i);
    } else {
    }

  printf ("\n%s float is any Subnormal\n", __FUNCTION__);

  i = (vf32_t) { 0.0, -0.0, 0.0, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_issubnormal i=", i);
#endif
  if (vec_any_issubnormalf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_any_issubnormal fail", i);
    } else {
    }

  i = (vf32_t) { __FLT_MAX__, __FLT_MIN__, __FLT_EPSILON__,
		  -0.0 };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_issubnormal i=", i);
#endif
  if (vec_any_issubnormalf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_any_issubnormal fail", i);
    } else {
    }

  i = (vf32_t) { __FLT_MAX__, __FLT_MIN__, __FLT_EPSILON__,
		  __FLT_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_issubnormal i=", i);
#endif
  if (vec_any_issubnormalf32 (i))
    {
    } else {
      rc += 1;
      print_v4f32x ("vec_any_issubnormal fail", i);
    }

  i = (vf32_t) CONST_VINT128_W(0x80000001, 0x007fffff, 1,
			       0x803fffff );

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_issubnormal i=", i);
#endif
  if (vec_any_issubnormalf32 (i))
    {
    } else {
      rc += 1;
      print_v4f32x ("vec_any_issubnormal fail", i);
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NNAN, 0xff000000, 1,
		0x80000000);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_issubnormal i=", i);
#endif
  if (vec_any_issubnormalf32 (i))
    {
    } else {
      rc += 1;
      print_v4f32x ("vec_any_issubnormal fail", i);
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_INF, __FLOAT_NINF, __FLOAT_INF,
			       __FLOAT_NINF);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_issubnormal i=", i);
#endif
  if (vec_any_issubnormalf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_any_issubnormal fail", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, __FLOAT_NNAN, __FLOAT_NSNAN,
			       __FLOAT_SNAN);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_issubnormal i=", i);
#endif
  if (vec_any_issubnormalf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_any_issubnormal fail", i);
    } else {
    }

  printf ("\n%s float is any Zero\n", __FUNCTION__);

  i = (vf32_t) { 0.0, -0.0, 0.0, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_iszero i=", i);
#endif
  if (vec_any_iszerof32 (i))
    {
    } else {
      rc += 1;
      print_v4f32x ("vec_any_iszero fail", i);
    }

  i = (vf32_t) { __FLT_MAX__, __FLT_MIN__, __FLT_EPSILON__,
		  -0.0 };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_iszero i=", i);
#endif
  if (vec_any_iszerof32 (i))
    {
    } else {
      rc += 1;
      print_v4f32x ("vec_any_iszero fail", i);
    }

  i = (vf32_t) { __FLT_MAX__, __FLT_MIN__, __FLT_EPSILON__,
		  __FLT_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_iszero i=", i);
#endif
  if (vec_any_iszerof32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_any_iszero fail", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(0x80000001, 0x007fffff, 1,
			       0x803fffff );

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_iszero i=", i);
#endif
  if (vec_any_iszerof32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_any_iszero fail", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NNAN, 0xff000000, 1,
		0x80000000);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_iszero i=", i);
#endif
  if (vec_any_iszerof32 (i))
    {
    } else {
      rc += 1;
      print_v4f32x ("vec_any_iszero fail", i);
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_INF, __FLOAT_NINF, __FLOAT_INF,
			       __FLOAT_NINF);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_iszero i=", i);
#endif
  if (vec_any_iszerof32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_any_iszero fail", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, __FLOAT_NNAN, __FLOAT_NSNAN,
			       __FLOAT_SNAN);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_iszero i=", i);
#endif
  if (vec_any_iszerof32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_any_iszero fail", i);
    } else {
    }

  return (rc);
}

int
test_float_cpsgn (void)
{
  vf32_t i, j, e;
  vf32_t k;
  int rc = 0;

  printf ("\n%s float Copy Sign\n", __FUNCTION__);

  i = (vf32_t) { 0.0, -0.0, 0.0, -0.0 };
  j = (vf32_t) {-0.0, 0.0, -0.0, 0.0 };
  e = (vf32_t) {-0.0, 0.0, -0.0, 0.0 };
  k = vec_copysignf32 (i, j);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_copysignf32 i=", i);
  print_v4f32x ("                j=", j);
  print_v4f32x ("                k=", k);
#endif
  rc += check_v4f32x ("vec_copysignf32 1:", k, e);

  i = (vf32_t) { __FLT_MAX__, __FLT_MIN__, __FLT_EPSILON__,
		  __FLT_DENORM_MIN__ };
  j = (vf32_t) {-0.0, 0.0, -0.0, 0.0 };
  e = (vf32_t) { -(__FLT_MAX__), __FLT_MIN__, -(__FLT_EPSILON__),
		  __FLT_DENORM_MIN__ };
  k = vec_copysignf32 (i, j);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_copysignf32 i=", i);
  print_v4f32x ("                j=", j);
  print_v4f32x ("                k=", k);
#endif
  rc += check_v4f32x ("vec_copysignf32 2:", k, e);

  i = (vf32_t) CONST_VINT128_W(__FLOAT_INF, __FLOAT_NINF, __FLOAT_INF,
			       __FLOAT_NINF);
  j = (vf32_t) CONST_VINT32_W(0.0, -0.0, 0.0, -0.0);
  e = (vf32_t) CONST_VINT128_W(__FLOAT_INF, __FLOAT_NINF, __FLOAT_INF,
			       __FLOAT_NINF);
  k = vec_copysignf32 (i, j);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_copysignf32 i=", i);
  print_v4f32x ("                j=", j);
  print_v4f32x ("                k=", k);
#endif
  rc += check_v4f32x ("vec_copysignf32 3:", k, e);

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, __FLOAT_NNAN, __FLOAT_NSNAN,
			       __FLOAT_SNAN);
  j = (vf32_t) {-0.0, 0.0, 0.0, -0.0 };
  e = (vf32_t) CONST_VINT128_W(__FLOAT_NNAN, __FLOAT_NAN, __FLOAT_SNAN,
			       __FLOAT_NSNAN);
  k = vec_copysignf32 (i, j);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_copysignf32 i=", i);
  print_v4f32x ("                j=", j);
  print_v4f32x ("                k=", k);
#endif
  rc += check_v4f32x ("vec_copysignf32 4:", k, e);

  return (rc);
}

int
test_float_isinf (void)
{
  vf32_t i;
  vb32_t e, k;
  int rc = 0;

  printf ("\n%s float isinf\n", __FUNCTION__);

  i = (vf32_t) { 0.0, -0.0, 0.0, -0.0 };
  e = (vb32_t) CONST_VINT128_W(__FLOAT_NTRUE, __FLOAT_NTRUE, __FLOAT_NTRUE,
			       __FLOAT_NTRUE);
  k = vec_isinff32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_isinff32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_isinff32 2:", k, e);

  i = (vf32_t) CONST_VINT128_W(__FLOAT_INF, __FLOAT_NINF, __FLOAT_INF,
			       __FLOAT_NINF);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_TRUE, __FLOAT_TRUE, __FLOAT_TRUE,
			       __FLOAT_TRUE);
  k = vec_isinff32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_isinff32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_isinff32 3:", k, e);

  i = (vf32_t) CONST_VINT128_W(0x7f7fffff, 0x00800000, 0x3f800000,
		0x80000001);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_NTRUE, __FLOAT_NTRUE, __FLOAT_NTRUE,
			       __FLOAT_NTRUE);
  k = vec_isinff32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_isinff32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_isinff32 4:", k, e);

  i = (vf32_t) CONST_VINT128_W(__FLOAT_INF, 0x00800000, 0x3f800000,
		0x80000001);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_TRUE, __FLOAT_NTRUE, __FLOAT_NTRUE,
			       __FLOAT_NTRUE);
  k = vec_isinff32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_isinff32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_isinff32 5:", k, e);

  i = (vf32_t) CONST_VINT128_W(0x7f7fffff, __FLOAT_NINF, 0x3f800000,
		0x80000001);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_NTRUE, __FLOAT_TRUE, __FLOAT_NTRUE,
			       __FLOAT_NTRUE);
  k = vec_isinff32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_isinff32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_isinff32 6:", k, e);

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NINF, 0x00800000, __FLOAT_INF,
		0x80000001);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_TRUE, __FLOAT_NTRUE, __FLOAT_TRUE,
			       __FLOAT_NTRUE);
  k = vec_isinff32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_isinff32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_isinff32 7:", k, e);

  return (rc);
}

int
test_float_isnan (void)
{
  vf32_t i;
  vb32_t e, k;
  int rc = 0;

  printf ("\n%s float isnan\n", __FUNCTION__);

  i = (vf32_t) { 0.0, -0.0, 0.0, -0.0 };
  e = (vb32_t) CONST_VINT128_W(__FLOAT_NTRUE, __FLOAT_NTRUE, __FLOAT_NTRUE,
			       __FLOAT_NTRUE);
  k = vec_isnanf32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_isnanf32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_isnanf32 2:", k, e);

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, __FLOAT_NNAN, __FLOAT_NSNAN,
			       __FLOAT_SNAN);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_TRUE, __FLOAT_TRUE, __FLOAT_TRUE,
			       __FLOAT_TRUE);
  k = vec_isnanf32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_isnanf32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_isnanf32 3:", k, e);

  i = (vf32_t) CONST_VINT128_W(0x7f7fffff, 0x00800000, 0x3f800000,
		0x80000001);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_NTRUE, __FLOAT_NTRUE, __FLOAT_NTRUE,
			       __FLOAT_NTRUE);
  k = vec_isnanf32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_isnanf32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_isnanf32 4:", k, e);

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, 0x00800000, 0x3f800000,
		0x80000001);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_TRUE, __FLOAT_NTRUE, __FLOAT_NTRUE,
			       __FLOAT_NTRUE);
  k = vec_isnanf32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_isnanf32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_isnanf32 5:", k, e);

  i = (vf32_t) CONST_VINT128_W(0x7f7fffff, __FLOAT_NNAN, 0x3f800000,
		0x80000001);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_NTRUE, __FLOAT_TRUE, __FLOAT_NTRUE,
			       __FLOAT_NTRUE);
  k = vec_isnanf32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_isnanf32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_isnanf32 6:", k, e);

  i = (vf32_t) CONST_VINT128_W(__FLOAT_SNAN, 0x00800000, __FLOAT_NNAN,
		0x80000001);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_TRUE, __FLOAT_NTRUE, __FLOAT_TRUE,
			       __FLOAT_NTRUE);
  k = vec_isnanf32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_isnanf32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_isnanf32 7:", k, e);

  return (rc);
}
//#define __DEBUG_PRINT__ 1
#ifdef __DEBUG_PRINT__
#define test_vec_isnormalf32(_i)	db_vec_isnormalf32(_i)
#else
#define test_vec_isnormalf32(_i)	vec_isnormalf32(_i)
#endif
int
test_float_isnormal (void)
{
  vf32_t i;
  vb32_t e, k;
  int rc = 0;

  printf ("\n%s float isnormal\n", __FUNCTION__);

  i = (vf32_t) { 0.0, -0.0, 0.0, -0.0 };
  e = (vb32_t) CONST_VINT128_W(__FLOAT_NTRUE, __FLOAT_NTRUE, __FLOAT_NTRUE,
			       __FLOAT_NTRUE);
  k = test_vec_isnormalf32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_isnormalf32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_isnormalf32 2:", k, e);

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, __FLOAT_NNAN, __FLOAT_NSNAN,
			       __FLOAT_SNAN);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_NTRUE, __FLOAT_NTRUE, __FLOAT_NTRUE,
			       __FLOAT_NTRUE);
  k = test_vec_isnormalf32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_isnormalf32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_isnormalf32 3:", k, e);

  i = (vf32_t) CONST_VINT128_W(0x7f7fffff, 0x00800000, 0x3f800000,
		0x80000001);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_TRUE, __FLOAT_TRUE, __FLOAT_TRUE,
			       __FLOAT_NTRUE);
  k = test_vec_isnormalf32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_isnormalf32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_isnormalf32 4:", k, e);

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, 0x00800000, 0x3f800000,
		0x00000001);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_NTRUE, __FLOAT_TRUE, __FLOAT_TRUE,
			       __FLOAT_NTRUE);
  k = test_vec_isnormalf32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_isnormalf32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_isnormalf32 5:", k, e);

  i = (vf32_t) CONST_VINT128_W(0x7f7fffff, __FLOAT_NNAN, 0x3f800000,
		0x80000001);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_TRUE, __FLOAT_NTRUE, __FLOAT_TRUE,
			       __FLOAT_NTRUE);
  k = test_vec_isnormalf32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_isnormalf32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_isnormalf32 6:", k, e);

  i = (vf32_t) CONST_VINT128_W(__FLOAT_SNAN, 0x00800000, __FLOAT_NINF,
		0x80000001);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_NTRUE, __FLOAT_TRUE, __FLOAT_NTRUE,
			       __FLOAT_NTRUE);
  k = test_vec_isnormalf32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_isnormalf32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_isnormalf32 7:", k, e);

  return (rc);
}
#undef __DEBUG_PRINT__
int
test_float_issubnormal (void)
{
  vf32_t i;
  vb32_t e, k;
  int rc = 0;

  printf ("\n%s float issubnormal\n", __FUNCTION__);

  i = (vf32_t) { 0.0, -0.0, 0.0, -0.0 };
  e = (vb32_t) CONST_VINT128_W(__FLOAT_NTRUE, __FLOAT_NTRUE, __FLOAT_NTRUE,
			       __FLOAT_NTRUE);
  k = vec_issubnormalf32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_issubnormalf32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_issubnormalf32 2:", k, e);

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, __FLOAT_NNAN, __FLOAT_NSNAN,
			       __FLOAT_SNAN);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_NTRUE, __FLOAT_NTRUE, __FLOAT_NTRUE,
			       __FLOAT_NTRUE);
  k = vec_issubnormalf32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_issubnormalf32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_issubnormalf32 3:", k, e);

  i = (vf32_t) CONST_VINT128_W(0x7f7fffff, 0x00400000, 0x3f800000,
		0x80000001);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_NTRUE, __FLOAT_TRUE, __FLOAT_NTRUE,
			       __FLOAT_TRUE);
  k = vec_issubnormalf32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_issubnormalf32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_issubnormalf32 4:", k, e);

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, 0x00800000, 0x3f800000,
		0x00000001);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_NTRUE, __FLOAT_NTRUE, __FLOAT_NTRUE,
			       __FLOAT_TRUE);
  k = vec_issubnormalf32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_issubnormalf32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_issubnormalf32 5:", k, e);

  i = (vf32_t) CONST_VINT128_W(0x7f7fffff, __FLOAT_NNAN, 0x3f800000,
		0x80000001);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_NTRUE, __FLOAT_NTRUE, __FLOAT_NTRUE,
			       __FLOAT_TRUE);
  k = vec_issubnormalf32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_issubnormalf32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_issubnormalf32 6:", k, e);

  i = (vf32_t) CONST_VINT128_W(__FLOAT_SNAN, 0x00800000, __FLOAT_NINF,
		0x80000001);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_NTRUE, __FLOAT_NTRUE, __FLOAT_NTRUE,
			       __FLOAT_TRUE);
  k = vec_issubnormalf32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_issubnormalf32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_issubnormalf32 7:", k, e);

  return (rc);
}

int
test_float_iszero (void)
{
  vf32_t i;
  vb32_t e, k;
  int rc = 0;

  printf ("\n%s float iszero\n", __FUNCTION__);

  i = (vf32_t) { 0.0, -0.0, 0.0, -0.0 };
  e = (vb32_t) CONST_VINT128_W(__FLOAT_TRUE, __FLOAT_TRUE, __FLOAT_TRUE,
			       __FLOAT_TRUE);
  k = vec_iszerof32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_iszerof32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_iszerof32 2:", k, e);

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, __FLOAT_NNAN, __FLOAT_NINF,
			       __FLOAT_SNAN);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_NTRUE, __FLOAT_NTRUE, __FLOAT_NTRUE,
			       __FLOAT_NTRUE);
  k = vec_iszerof32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_iszerof32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_iszerof32 3:", k, e);

  i = (vf32_t) CONST_VINT128_W(0x7f7fffff, 0x00400000, 0x3f800000,
		0x80000000);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_NTRUE, __FLOAT_NTRUE, __FLOAT_NTRUE,
			       __FLOAT_TRUE);
  k = vec_iszerof32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_iszerof32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_iszerof32 4:", k, e);

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, 0x80000000, 0x3f800000,
		0x00000001);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_NTRUE, __FLOAT_TRUE, __FLOAT_NTRUE,
			       __FLOAT_NTRUE);
  k = vec_iszerof32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_iszerof32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_iszerof32 5:", k, e);

  i = (vf32_t) CONST_VINT128_W(0x7f7fffff, __FLOAT_NNAN, 0x3f800000,
		0x80000001);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_NTRUE, __FLOAT_NTRUE, __FLOAT_NTRUE,
			       __FLOAT_NTRUE);
  k = vec_iszerof32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_iszerof32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_iszerof32 6:", k, e);

  i = (vf32_t) CONST_VINT128_W(__FLOAT_SNAN, 0x00800000, __FLOAT_NINF,
		0x80000001);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_NTRUE, __FLOAT_NTRUE, __FLOAT_NTRUE,
			       __FLOAT_NTRUE);
  k = vec_iszerof32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_iszerof32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_iszerof32 7:", k, e);

  return (rc);
}
#define __DEBUG_PRINT__ 1
#undef __DEBUG_PRINT__


#define TIME_10_ITERATION 10

int
test_time_f32 (void)
{
  long i;
  uint64_t t_start, t_end, t_delta;
  double delta_sec;
  int rc = 0;

  printf ("\n%s is_f32 start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIME_10_ITERATION; i++)
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
  for (i = 0; i < TIME_10_ITERATION; i++)
    {
      rc += timed_fpclassify_f32 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s fpclassify_f32 end", __FUNCTION__);
  printf ("\n%s fpclassify_f32  tb delta = %lu, sec = %10.6g\n", __FUNCTION__,
	  t_delta, delta_sec);

  return (rc);
}

int
test_vec_f32 (void)
{
  int rc = 0;

  printf ("\n%s\n", __FUNCTION__);

  rc += test_float_abs ();
  rc += test_float_all_is ();
  rc += test_float_any_is ();
  rc += test_float_cpsgn ();
  rc += test_float_isinf ();
  rc += test_float_isnan ();
  rc += test_float_isnormal ();
  rc += test_float_issubnormal ();
  rc += test_float_iszero ();

  rc += test_time_f32 ();

  return (rc);
}

