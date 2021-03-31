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
#include <pveclib/vec_int32_ppc.h>
#include <pveclib/vec_f32_ppc.h>
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
  printf ("\n%s float is all finite\n", __FUNCTION__);

  i = (vf32_t) { 0.0, -0.0, 0.0, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_isfinite i=", i);
#endif
  if (vec_all_isfinitef32 (i))
    {
    } else {
      rc += 1;
      print_v4f32x ("vec_all_isfinite fail 1", i);
    }

  i = (vf32_t) { __FLT_MAX__, __FLT_MIN__, __FLT_EPSILON__,
		  __FLT_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_isfinite i=", i);
#endif
  if (vec_all_isfinitef32 (i))
    {
    } else {
      rc += 1;
      print_v4f32x ("vec_all_isfinite fail 2", i);
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_INF, __FLOAT_NINF, 0,
			       __FLOAT_NINF);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_isfinite i=", i);
#endif
  if (vec_all_isfinitef32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_isfinite fail 3", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_INF, __FLOAT_NINF, __FLOAT_INF,
			       __FLOAT_NINF);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_isfinite i=", i);
#endif
  if (vec_all_isfinitef32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_isfinite fail 4", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, __FLOAT_NAN, __FLOAT_SNAN,
			       __FLOAT_SNAN);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_isfinite i=", i);
#endif
  if (vec_all_isfinitef32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_isfinite fail 5", i);
    } else {
    }

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
      print_v4f32x ("vec_all_isnan fail 1", i);
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
      print_v4f32x ("vec_all_isnan fail 2", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, __FLOAT_NNAN, 0,
			       __FLOAT_SNAN);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_isnan i=", i);
#endif

  if (vec_all_isnanf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_isnan fail 3", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_INF, __FLOAT_NINF, __FLOAT_INF,
			       __FLOAT_NINF);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_all_isnan i=", i);
#endif
  if (vec_all_isnanf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_all_isnan fail 4", i);
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
      print_v4f32x ("vec_all_isnan fail 5", i);
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
  printf ("\n%s float is any finite\n", __FUNCTION__);

  i = (vf32_t) { 0.0, -0.0, 0.0, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_isfinite i=", i);
#endif
  if (vec_any_isfinitef32 (i))
    {
    } else {
      rc += 1;
      print_v4f32x ("vec_any_isfinite fail 1", i);
    }

  i = (vf32_t) { __FLT_MAX__, __FLT_MIN__, __FLT_EPSILON__,
		  __FLT_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_isfinite i=", i);
#endif
  if (vec_any_isfinitef32 (i))
    {
    } else {
      rc += 1;
      print_v4f32x ("vec_any_isfinite fail 2", i);
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_INF, __FLOAT_NINF, 0,
			       __FLOAT_NINF);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_isfinite i=", i);
#endif
  if (vec_any_isfinitef32 (i))
    {
    } else {
      rc += 1;
      print_v4f32x ("vec_any_isfinite fail 3", i);
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_INF, __FLOAT_NINF, __FLOAT_INF,
			       __FLOAT_NINF);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_isfinite i=", i);
#endif
  if (vec_any_isfinitef32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_any_isfinite fail 4", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, __FLOAT_NAN, __FLOAT_SNAN,
			       __FLOAT_SNAN);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_isfinite i=", i);
#endif
  if (vec_any_isfinitef32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_any_isfinite fail 5", i);
    } else {
    }

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
      print_v4f32x ("vec_any_isnormal fail 1", i);
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
      print_v4f32x ("vec_any_isnormal fail 2", i);
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
      print_v4f32x ("vec_any_isnormal fail 3", i);
    }

  i = (vf32_t) { __FLT_DENORM_MIN__, __FLT_DENORM_MIN__, __FLT_DENORM_MIN__,
		  __FLT_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_isnormal i=", i);
#endif
  if (vec_any_isnormalf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_any_isnormal fail 4", i);
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
      print_v4f32x ("vec_any_isnormal fail 5", i);
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
      print_v4f32x ("vec_any_isnormal fail 6", i);
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_INF, __FLOAT_NINF, __FLOAT_INF,
			       __FLOAT_NINF);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_isnormal i=", i);
#endif
  if (vec_any_isnormalf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_any_isnormal fail 7", i);
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
      print_v4f32x ("vec_any_isnormal fail 8", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, __FLOAT_INF, 0x7ffff,
			       1);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_isnormal i=", i);
#endif
  if (vec_any_isnormalf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_any_isnormal fail 9", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, __FLOAT_INF, 0,
			       1);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_isnormal i=", i);
#endif
  if (vec_any_isnormalf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_any_isnormal fail 10", i);
    } else {
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, __FLOAT_INF, 0x00800000,
			       1);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_isnormal i=", i);
#endif
  if (vec_any_isnormalf32 (i))
    {
    } else {
      rc += 1;
      print_v4f32x ("vec_any_isnormal fail 11", i);
    }

  printf ("\n%s float is any Subnormal\n", __FUNCTION__);

  i = (vf32_t) { 0.0, -0.0, 0.0, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_issubnormal i=", i);
#endif
  if (vec_any_issubnormalf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_any_issubnormal fail 1", i);
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
      print_v4f32x ("vec_any_issubnormal fail 2", i);
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
      print_v4f32x ("vec_any_issubnormal fail 3", i);
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
      print_v4f32x ("vec_any_issubnormal fail 4", i);
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
      print_v4f32x ("vec_any_issubnormal fail 5", i);
    }

  i = (vf32_t) CONST_VINT128_W(__FLOAT_INF, __FLOAT_NINF, __FLOAT_INF,
			       __FLOAT_NINF);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_any_issubnormal i=", i);
#endif
  if (vec_any_issubnormalf32 (i))
    {
      rc += 1;
      print_v4f32x ("vec_any_issubnormal fail 6", i);
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
      print_v4f32x ("vec_any_issubnormal fail 7", i);
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

int
test_float_isfinite (void)
{
  vf32_t i;
  vb32_t e, k;
  int rc = 0;

  printf ("\n%s float isfinite\n", __FUNCTION__);

  i = (vf32_t) { 0.0, -0.0, 0.0, -0.0 };
  e = (vb32_t) CONST_VINT128_W(__FLOAT_TRUE, __FLOAT_TRUE, __FLOAT_TRUE,
			       __FLOAT_TRUE);
  k = vec_isfinitef32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_isfinitef32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_isfinitef32 2:", k, e);

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, __FLOAT_NNAN, __FLOAT_NSNAN,
			       __FLOAT_SNAN);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_NTRUE, __FLOAT_NTRUE, __FLOAT_NTRUE,
			       __FLOAT_NTRUE);
  k = vec_isfinitef32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_isfinitef32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_isfinitef32 3:", k, e);

  i = (vf32_t) CONST_VINT128_W(0x7f7fffff, 0x00800000, 0x3f800000,
		0x80000001);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_TRUE, __FLOAT_TRUE, __FLOAT_TRUE,
			       __FLOAT_TRUE);
  k = vec_isfinitef32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_isfinitef32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_isfinitef32 4:", k, e);

  i = (vf32_t) CONST_VINT128_W(__FLOAT_NAN, 0x00800000, 0x3f800000,
		0x00000001);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_NTRUE, __FLOAT_TRUE, __FLOAT_TRUE,
			       __FLOAT_TRUE);
  k = vec_isfinitef32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_isfinitef32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_isfinitef32 5:", k, e);

  i = (vf32_t) CONST_VINT128_W(0x7f7fffff, __FLOAT_NNAN, 0x3f800000,
		0x80000001);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_TRUE, __FLOAT_NTRUE, __FLOAT_TRUE,
			       __FLOAT_TRUE);
  k = vec_isfinitef32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_isfinitef32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_isfinitef32 6:", k, e);

  i = (vf32_t) CONST_VINT128_W(__FLOAT_SNAN, 0x00800000, __FLOAT_NINF,
		0x80000001);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_NTRUE, __FLOAT_TRUE, __FLOAT_NTRUE,
			       __FLOAT_TRUE);
  k = vec_isfinitef32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_isfinitef32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_isfinitef32 7:", k, e);

  i = (vf32_t) CONST_VINT128_W(__FLOAT_INF, __FLOAT_NINF, 0,
			       __FLOAT_NINF);
  e = (vb32_t) CONST_VINT128_W(__FLOAT_NTRUE, __FLOAT_NTRUE, __FLOAT_TRUE,
			       __FLOAT_NTRUE);
  k = vec_isfinitef32 (i);

#ifdef __DEBUG_PRINT__
  print_v4f32x ("vec_isfinitef32 i=", i);
  print_v4b32c ("             k=", k);
  print_v4b32x ("             k=", k);
#endif
  rc += check_v4b32c ("vec_isfinitef32 8:", k, e);

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

static float test_f32[] =
    {
	0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0,
	8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0,
	0.0, -1.0, -2.0, -3.0, -4.0, -5.0, -6.0, -7.0,
	-8.0, -9.0, -10.0, -11.0, -12.0, -13.0, -14.0, -15.0
    };

int
test_lvgfsx (void)
{
  vi32_t i1;
  vf32_t e, j;
  vi64_t id1;
  vf64_t ed;
  vf64_t jd, jd0, jd1;
  int rc = 0;

  printf ("\ntest_Vector Gather-Load Single Float\n");

  ed =  (vf64_t) CONST_VINT64_DW ( 0.0, -2.0);
  jd0 = vec_vlxsspx (0, test_f32);
  jd =  (vf64_t) vec_permdi ((vui64_t) jd0, (vui64_t)ed, 1);

  rc += check_v2f64 ("vec_vlxsspx 1:", (vf64_t) jd, (vf64_t) ed);

  ed =  (vf64_t) CONST_VINT64_DW ( 1.0, -2.0 );
  jd0 = vec_vlxsspx (4, test_f32);
  jd =  (vf64_t) vec_permdi ((vui64_t) jd0, (vui64_t)ed, 1);

  rc += check_v2f64 ("vec_vlxsspx 2:", (vf64_t) jd, (vf64_t) ed);

  ed =  (vf64_t) CONST_VINT64_DW ( 15.0, -2.0 );
  jd0 = vec_vlxsspx (60, test_f32);
  jd =  (vf64_t) vec_permdi ((vui64_t) jd0, (vui64_t)ed, 1);

  rc += check_v2f64 ("vec_vlxsspx 3:", (vf64_t) jd, (vf64_t) ed);

  i1 = (vi32_t) { 4, 60, 8, 56 };
  ed =  (vf64_t) CONST_VINT64_DW ( 1.0, -2.0 );
  jd0 = vec_vlxsspx (i1[0], test_f32);
  jd =  (vf64_t) vec_permdi ((vui64_t) jd0, (vui64_t)ed, 1);

  rc += check_v2f64 ("vec_vlxsspx 4:", (vf64_t) jd, (vf64_t) ed);

  ed = (vf64_t) CONST_VINT64_DW ( 15.0, -2.0 );
  jd1 = vec_vlxsspx (i1[1], test_f32);
  jd = (vf64_t) vec_permdi ((vui64_t) jd1, (vui64_t)ed, 1);

  rc += check_v2f64 ("vec_vlxsspx 5:", (vf64_t) jd, (vf64_t) ed);

  // This test depends on the merge of scalars from lxsiwzx 4/5

  ed =  (vf64_t) { 1.0, 15.0 };
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  jd = (vf64_t) vec_permdi ((vui64_t) jd1, (vui64_t) jd0, 0);
#else
  jd = (vf64_t) vec_permdi ((vui64_t) jd0, (vui64_t) jd1, 0);
#endif
  rc += check_v2f64 ("vec_vlxsspx2 :", (vf64_t) jd, (vf64_t) ed);

  // This test replicates the results of the last 3 tests in single op.
  ed =  (vf64_t) { 1.0, 15.0 };
  jd = vec_vglfsso (test_f32, 4, 60);

  rc += check_v2f64 ("vec_vglfsso :", (vf64_t) jd, (vf64_t) ed);

  // This test replicates the results of the last tests with vector offsets.
  id1 = (vi64_t)  {  4, 60 };
  jd = vec_vglfsdo (test_f32, id1);

  rc += check_v2f64 ("vec_vglfsdo :", (vf64_t) jd, (vf64_t) ed);

  // This test replicates the results of the last tests with vector indexes.
  id1 = (vi64_t)  {  1, 15 };
  jd = vec_vglfsdx (test_f32, id1);

  rc += check_v2f64 ("vec_vglfsdx :", (vf64_t) jd, (vf64_t) ed);

  // This test replicates the results of the last tests with vector
  // scaled indexes.
  id1 = (vi64_t)  { 1, 3 };
  ed =  (vf64_t) { 4.0, 12.0 };
  jd = vec_vglfsdsx (test_f32, id1, 2);

  rc += check_v2f64 ("vec_vglfsdsx :", (vf64_t) jd, (vf64_t) ed);


  // This test replicates the results of the last 3 tests in single op.
  e =  (vf32_t) { 1.0, 15.0, 2.0, -2.0 };
  j = vec_vgl4fsso (test_f32, 4, 60, 8, 72);

  rc += check_v4f32 ("vec_vgl4fsso :", j, e);

  // This test replicates the results of the last 3 tests in single op.
  i1 = (vi32_t) { 4, 60, 8, 72 };
  e =  (vf32_t) { 1.0, 15.0, 2.0, -2.0 };
  j = vec_vgl4fswo (test_f32, i1);

  rc += check_v4f32 ("vec_vgl4fswo :", j, e);

  // This test replicates the results of the last tests with indexed op.
  i1 = (vi32_t) { 1, 15, 2, 18 };
  e =  (vf32_t) { 1.0, 15.0, 2.0, -2.0 };
  j = vec_vgl4fswx (test_f32, i1);

  rc += check_v4f32 ("vec_vgl4fswx :", j, e);

  // This test replicates the results of the last tests with scaled indexed op.
  i1 = (vi32_t) { 1, 15, 2, 9 };
  e =  (vf32_t) { 2.0, -14.0, 4.0, -2.0 };
  j = vec_vgl4fswsx (test_f32, i1, 1);

  rc += check_v4f32 ("vec_vgl4fswsx :", j, e);
  return (rc);
}

static float test_stf32[16];

int
test_stvgfsx (void)
{
  vi32_t i1, i2;
  vf32_t e, *mem;
  vf32_t j, j1;
  vf64_t jd, jd1, jd2;
  vi64_t id1;
  int rc = 0;
  int i;

  mem = (vf32_t *)& test_stf32;

  for (i=0; i<16; i++)
    test_stf32[i] = test_f32[i];

  printf ("\ntest_Vector Scatter-Store Single Float\n");

  jd1 = (vf64_t) CONST_VINT64_DW ( 16.0, 1616.0 );
  jd2 = (vf64_t) CONST_VINT64_DW ( 31.0, 3131.0 );
  vec_vstxsspx (jd1, 0, test_stf32);
  vec_vstxsspx (jd2, 60, test_stf32);

  j = mem [0];
  e = (vf32_t) { 16.0, 1.0, 2.0, 3.0 };

  rc += check_v4f32 ("vec_vstxsspx 1:", j, e);

  j = mem [3];
  e = (vf32_t) { 12.0, 13.0, 14.0, 31.0 };

  rc += check_v4f32 ("vec_vstxsspx 2:", j, e);

  jd = (vf64_t) { 0.5, 31.5 };
  vec_vsstfsso (jd, test_stf32, 0, 60);

  j = mem [0];
  e = (vf32_t) { 0.5, 1.0, 2.0, 3.0 };

  rc += check_v4f32 ("vec_vsstfsso 1:", j, e);

  j = mem [3];
  e = (vf32_t) { 12.0, 13.0, 14.0, 31.5 };

  rc += check_v4f32 ("vec_vsstfsso 2:", j, e);

  jd = (vf64_t) { 0.75, 31.75 };
  id1 = (vi64_t) { 0, 60 };
  vec_vsstfsdo (jd, test_stf32, id1);

  j = mem [0];
  e = (vf32_t) { 0.75, 1.0, 2.0, 3.0 };

  rc += check_v4f32 ("vec_vsstfsdo 1:", j, e);

  j = mem [3];
  e = (vf32_t) { 12.0, 13.0, 14.0, 31.75 };

  rc += check_v4f32 ("vec_vsstfsdo 2:", j, e);

  jd = (vf64_t) { 0.25, 31.25 };
  id1 = (vi64_t) { 0, 15 };
  vec_vsstfsdx (jd, test_stf32, id1);

  j = mem [0];
  e = (vf32_t) { 0.25, 1.0, 2.0, 3.0 };

  rc += check_v4f32 ("vec_vsstfsdx 1:", j, e);

  j = mem [3];
  e = (vf32_t) { 12.0, 13.0, 14.0, 31.25 };

  rc += check_v4f32 ("vec_vsstfsdx 2:", j, e);

  jd = (vf64_t) { 0.125, 31.125 };
  id1 = (vi64_t) { 0, 15 };
  vec_vsstfsdsx (jd, test_stf32, id1, 0);

  j = mem [0];
  e = (vf32_t) { 0.125, 1.0, 2.0, 3.0 };

  rc += check_v4f32 ("vec_vsstfsdsx 1:", j, e);

  j = mem [3];
  e = (vf32_t) { 12.0, 13.0, 14.0, 31.125 };

  rc += check_v4f32 ("vec_vsstfsdsx 2:", j, e);


  j1 = (vf32_t) { 16.0, 31.0, 17.0, 30.0 };
  vec_vsst4fsso (j1, test_stf32, 0, 60, 4, 56);

  j = mem [0];
  e = (vf32_t) { 16.0, 17.0, 2.0, 3.0 };

  rc += check_v4f32 ("vec_vsst4fsso 1:", j, e);

  j = mem [3];
  e = (vf32_t) { 12.0, 13.0, 30.0, 31.0 };

  rc += check_v4f32 ("vec_vsst4fsso 2:", j, e);


  j1 = (vf32_t) { 16.5, 31.5, 17.5, 30.5 };
  i1 = (vi32_t) { 0, 60, 4, 56 };
  vec_vsst4fswo (j1, test_stf32, i1);

  j = mem [0];
  e = (vf32_t) { 16.5, 17.5, 2.0, 3.0 };

  rc += check_v4f32 ("vec_vsst4fswo 1:", j, e);

  j = mem [3];
  e = (vf32_t) { 12.0, 13.0, 30.5, 31.5 };

  rc += check_v4f32 ("vec_vsst4fswo 2:", j, e);

  j1 = (vf32_t) { 16.75, 31.75, 17.75, 30.75 };
  i2 = (vi32_t) { 0, 15, 1, 14 };
  vec_vsst4fswx (j1, test_stf32, i2);

  j = mem [0];
  e = (vf32_t) { 16.75, 17.75, 2.0, 3.0 };


  rc += check_v4f32 ("vec_vsst4fswx 1:", j, e);

  j = mem [3];
  e = (vf32_t) { 12.0, 13.0, 30.75, 31.75 };

  rc += check_v4f32 ("vec_vsst4fswx 2:", j, e);

  j1 = (vf32_t) { 0.0, 7.0, 1.0, 6.0 };
  i2 = (vi32_t) { 0, 7, 1, 6 };
  vec_vsst4fswsx (j1, test_stf32, i2, 1);

  j = mem [0];
  e = (vf32_t) { 0.0, 17.75, 1.0, 3.0 };

  rc += check_v4f32 ("vec_vsst4fswsx 1:", j, e);

  j = mem [3];
  e = (vf32_t) { 6.0, 13.0, 7.0, 31.75 };

  rc += check_v4f32 ("vec_vsst4fswsx 2:", j, e);

  return (rc);
}

float matrix_f32 [MN][MN] __attribute__ ((aligned (128)));

void
test_f32_Imatrix_init (float * array)
{
  long i, j, k;
  long rows, columns;

  rows = columns = MN;

#ifdef __DEBUG_PRINT__
  printf ("init_indentity array[%d,%d]\n",
	  rows, columns);
#endif

  for ( i=0; i<rows; i++ )
  {
    for ( j=0; j<columns; j++ )
      {
	k = (i * columns) + j;
	if (i == j)
	  {
	    array [k] = 1.0;
#ifdef __DEBUG_PRINT__
	    printf ("init_indentity array[%d,%d] is %f\n",
			i, j, array [k]);
#endif
	  }
	else
	  {
	    array [k] = 0.0;
	  }
      }
  }
}

int
#if !defined(__clang__)
__attribute__ ((optimize ("unroll-loops")))
#endif
test_f32_Imatrix_check (float * array)
{
  long i, j, k;
  long rows, columns;
  int rc = 0;

  rows = columns = MN;

  for ( i=0; i<rows; i++ )
  {
    for ( j=0; j<columns; j++ )
      {
	k = (i * columns) + j;
	if (i == j)
	  {
	    if ( array [k] != 1.0 )
	      {
		printf ("check_indentity array[%ld,%ld] !=1.0 is %f\n",
			i, j, array [k]);
		rc++;
	      }
	  }
	else
	  {
	    if ( array [k] != 0.0 )
	      {
		printf ("check_indentity array[%ld,%ld] !=0.0 is %f\n",
			i, j, array [k]);
		rc++;
	      }
	  }
      }
  }
  if (rc)
    {
      printf ("check_indentity array failed rc=%d\n",
		rc);

    }
  return rc;
}

void
#if !defined(__clang__)
__attribute__ ((optimize ("unroll-loops")))
#endif
test_f32_matrix_transpose (float * tm, float * m)
{
  long i, j, k, l;
  long rows, columns;

  rows = columns = MN;

  for ( i=0; i<rows; i++ )
  {
    for ( j=0; j<columns; j++ )
      {
	k = (i * columns) + j;
	l = (j * columns) + i;
	tm[l] = m[k];
      }
  }
}

void
//__attribute__ ((optimize ("unroll-loops")))
test_f32_matrix_gather_transpose (float * tm, float * m)
{
  vi32_t vra_init = { 0, MN*4, (MN*2)*4, (MN*3)*4 };
  vi32_t vra;
  vi32_t stride = { MN * 4 * 4, MN * 4 * 4, MN * 4 * 4, MN * 4 * 4 };
  long i, j;
  long rows, columns;

  rows = columns = MN;
  vra = vra_init;
#ifdef __DEBUG_PRINT__
  printf ("test_f32_matrix_gather_transpose (%p, %p)\n", tm, m);
  printf ("vra    = {%d, %d, %d, %d}\n",
	  vra[0], vra[1], vra[2], vra[3]);
  printf ("stride = {%d, %d, %d, %d}\n",
	  stride[0], stride[1], stride[2], stride[3]);
#endif
  for (i = 0; i < rows; i++)
    {
      float *cadr = &m[i];
      vf32_t *radr = (vf32_t*)&tm[(i * columns)];
      vra = vra_init;
      for (j = 0; j < columns/4; j++)
	{
	  radr[j] = vec_vgl4fswo (cadr, vra);
	  vra = vec_add ( vra, stride);
	}
    }
}

void
//__attribute__ ((optimize ("unroll-loops")))
test_f32_matrix_gatherx2_transpose (float * tm, float * m)
{
  vi32_t vra_init = { 0, MN*4, (MN*2)*4, (MN*3)*4 };
  vi32_t vra;
  vi32_t stride = { MN * 4 * 4, MN * 4 * 4, MN * 4 * 4, MN * 4 * 4 };
  long i, j;
  long rows, columns;

  rows = columns = MN;

  for (i = 0; i < rows; i+=2)
    {
      float *cadr = &m[i];
      float *cadr1 = &m[i+1];
      vf32_t *radr = (vf32_t*)&tm[(i * columns)];
      vf32_t *radr1 = (vf32_t*)&tm[((i+1) * columns)];

      vra = vra_init;
      for (j = 0; j < columns/4; j++)
	{
	  vf32_t vrow0, vrow1;
	  vrow0 = vec_vgl4fswo (cadr, vra);
	  vrow1 = vec_vgl4fswo (cadr1, vra);
	  radr[j] = vrow0;
	  radr1[j] = vrow1;
	  vra = vec_add ( vra, stride);
	}
    }
}

#if 1
void
//__attribute__ ((optimize ("unroll-loops")))
test_f32_matrix_gatherx4_transpose (float * tm, float * m)
{
  vi32_t vra_init = { 0, MN*4, (MN*2)*4, (MN*3)*4 };
  vi32_t vra;
  vi32_t stride = { MN * 4 * 4, MN * 4 * 4, MN * 4 * 4, MN * 4 * 4 };
  long i, j;
  long rows, columns;

  rows = columns = MN;

  for (i = 0; i < rows; i+=4)
    {
      float *cadr = &m[i];
      float *cadr1 = &m[i+1];
      float *cadr2 = &m[i+2];
      float *cadr3 = &m[i+3];
      vf32_t *radr = (vf32_t*)&tm[(i * columns)];
      vf32_t *radr1 = (vf32_t*)&tm[((i+1) * columns)];
      vf32_t *radr2 = (vf32_t*)&tm[((i+2) * columns)];
      vf32_t *radr3 = (vf32_t*)&tm[((i+3) * columns)];

      vra = vra_init;
      for (j = 0; j < columns/4; j++)
	{
	  vf32_t vrow0, vrow1, vrow2, vrow3;
	  vrow0 = vec_vgl4fswo (cadr, vra);
	  vrow1 = vec_vgl4fswo (cadr1, vra);
	  vrow2 = vec_vgl4fswo (cadr2, vra);
	  vrow3 = vec_vgl4fswo (cadr3, vra);
	  radr[j] = vrow0;
	  radr1[j] = vrow1;
	  radr2[j] = vrow2;
	  radr3[j] = vrow3;
	  vra = vec_add ( vra, stride);
	}
    }
}
#endif
int
test_f32_indentity_array ()
{
  float tmatrix[MN][MN] __attribute__ ((aligned (128)));
  int rc = 0;

  printf ("\ntest_indentity_array\n");

  test_f32_Imatrix_init  (&matrix_f32[0][0]);

  rc += test_f32_Imatrix_check (&matrix_f32[0][0]);

  test_f32_matrix_transpose (&tmatrix[0][0], &matrix_f32[0][0]);

  rc += test_f32_Imatrix_check (&tmatrix[0][0]);

  test_f32_matrix_gather_transpose (&tmatrix[0][0], &matrix_f32[0][0]);

  rc += test_f32_Imatrix_check (&tmatrix[0][0]);

  test_f32_matrix_gatherx2_transpose (&tmatrix[0][0], &matrix_f32[0][0]);

  rc += test_f32_Imatrix_check (&tmatrix[0][0]);

#if 1
  test_f32_matrix_gatherx4_transpose (&tmatrix[0][0], &matrix_f32[0][0]);

  rc += test_f32_Imatrix_check (&tmatrix[0][0]);
#endif
  return rc;
}

#define TIMING_ITERATIONS 10

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
  test_f32_Imatrix_init  (&matrix_f32[0][0]);

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
test_setb_sp (void)
{
  const vf32_t f32_zero_one = (vf32_t)
      CONST_VINT32_W(0.0, -0.0, 1.0, -1.0);

  const vf32_t f32_max_min = (vf32_t)
      CONST_VINT128_W(0x7f000000, 0xff000000,
                      0x00400000, 0x80400000);

  const vf32_t f32_sub_inf = (vf32_t)
      CONST_VINT128_W(0x007fffff, 0x807fffff,
		       __FLOAT_INF, __FLOAT_NINF);

  const vf32_t f32_nan_snan = (vf32_t)
      CONST_VINT128_W(__FLOAT_NAN, __FLOAT_NNAN,
		       __FLOAT_SNAN, __FLOAT_NSNAN);

  const vb32_t f32_FTFT = (vb32_t)
      CONST_VINT128_W(0, -1, 0, -1);

  vf32_t x;
  vb32_t test, expt;
  long tests_count = 0;
  int rc = 0;

  printf ("\ntest_setb_sp f32 -> vector bool , ...\n");

#if 1
  tests_count++;
  x = (vf32_t) f32_zero_one;
#ifdef __DEBUG_PRINT__
  print_v4f32 (" x=  ", x);
  print_v4b32x(" t=  ", test);
#endif
  test = vec_setb_sp (x);
  expt = f32_FTFT;
  rc += check_v4b32x ("check vec_setb_sp 1", test, expt);
#endif
#if 1
  tests_count++;
  x = (vf32_t) f32_max_min;
#ifdef __DEBUG_PRINT__
  print_v4f32 (" x=  ", x);
  print_v4b32x(" t=  ", test);
#endif
  test = vec_setb_sp (x);
  expt = f32_FTFT;
  rc += check_v4b32x ("check vec_setb_sp 2", test, expt);
#endif
#if 1
  tests_count++;
  x = (vf32_t) f32_sub_inf;
  test = vec_setb_sp (x);
#ifdef __DEBUG_PRINT__
  print_v4f32 (" x=  ", x);
  print_v4b32x(" t=  ", test);
#endif
  expt = f32_FTFT;
  rc += check_v4b32x ("check vec_setb_sp 3", test, expt);
#endif
#if 1
  tests_count++;
  x = (vf32_t) f32_nan_snan;
  test = vec_setb_sp (x);
#ifdef __DEBUG_PRINT__
  print_v4f32 (" x=  ", x);
  print_v4b32x(" t=  ", test);
#endif
  expt = f32_FTFT;
  rc += check_v4b32x ("check vec_setb_sp 4", test, expt);
#endif

  /* accumulate the number of values tested, in case we are doing
   * detail timing and want to compute function averages.  */
  tcount += tests_count;
  printf ("\ntest_setb_sp, tests=%ld fails=%d\n", tests_count, rc);

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
  rc += test_float_isfinite ();
  rc += test_setb_sp ();
  rc += test_lvgfsx ();
  rc += test_stvgfsx ();
  rc += test_f32_indentity_array ();

  rc += test_time_f32 ();

  return (rc);
}

