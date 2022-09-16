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

 arith128_test_f64.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Jul 3, 2018
 */

#include <stdint.h>
#include <stdio.h>
#include <float.h>

#include "arith128.h"
#include <testsuite/arith128_print.h>
#include <pveclib/vec_int64_ppc.h>
#include <pveclib/vec_f64_ppc.h>
#include <testsuite/arith128_test_f64.h>
#include <testsuite/vec_perf_f64.h>

#define __DOUBLE_INF (0x7ff0000000000000UL)
#define __DOUBLE_NINF (0xfff0000000000000UL)
#define __DOUBLE_NAN (0x7ff0000000000001UL)
#define __DOUBLE_NNAN (0xfff0000000000001UL)
#define __DOUBLE_SNAN (0x7ff8000000000001UL)
#define __DOUBLE_NSNAN (0xfff8000000000001UL)
#define __DOUBLE_TRUE (0xffffffffffffffffUL)
#define __DOUBLE_NTRUE (0x0000000000000000UL)


#ifdef __DEBUG_PRINT__
static inline vb64_t
db_vec_isnormalf64 (vf64_t vf64)
  {
    vui64_t tmp, tmp2;
    const vui64_t expmask = CONST_VINT128_DW(0x7ff0000000000000UL, 0x7ff0000000000000UL);
    const vui64_t vec_denorm = CONST_VINT128_DW(0x0010000000000000UL, 0x0010000000000000UL);
    vb64_t result;

    print_v2f64x ("db_vec_isnormalf64:", vf64);

#if _ARCH_PWR7
    /* Eliminate const load. */
    tmp2 = (vui64_t)vec_abs (vf64);
#else
    const vui64_t signmask = CONST_VINT128_DW(0x8000000000000000UL, 0x8000000000000000UL);
    tmp2 = vec_andc ((vui64_t)vf64, signmask);
#endif
    print_vint128x ("              tmp2=", (vui128_t)tmp2);
    tmp = vec_and ((vui64_t)vf64, expmask);
    print_vint128x ("              tmp =", (vui128_t)tmp);
    tmp2 = (vui64_t)vec_cmplt(tmp2, vec_denorm);
    print_vint128x ("              tmp2=", (vui128_t)tmp2);
    tmp = (vui64_t)vec_cmpeq(tmp, expmask);
    print_vint128x ("              tmp =", (vui128_t)tmp);
    result = (vb64_t)vec_nor (tmp, tmp2);
    print_v2b64x ("            result=", result);

    return (result);
  }
#endif

int
test_double_abs (void)
{
  vf64_t i, e;
  vf64_t k;
  int rc = 0;

  printf ("\n%s double absolute value\n", __FUNCTION__);

  i = (vf64_t) { 0.0, -0.0};
  e = (vf64_t) { 0.0, 0.0};
  k = vec_absf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_absf64 i=", i);
  print_v2f64x ("           k=", k);
#endif
  rc += check_v2f64x ("vec_absf64 1:", k, e);

  i = (vf64_t) { -(__DBL_MAX__), __DBL_MIN__ };
  e = (vf64_t) { __DBL_MAX__, __DBL_MIN__ };
  k = vec_absf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_absf64 i=", i);
  print_v2f64x ("           k=", k);
#endif
  rc += check_v2f64x ("vec_absf64 2:", k, e);

  i = (vf64_t) { __DBL_EPSILON__, -(__DBL_DENORM_MIN__) };
  e = (vf64_t) { __DBL_EPSILON__, __DBL_DENORM_MIN__ };
  k = vec_absf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_absf64 i=", i);
  print_v2f64x ("           k=", k);
#endif
  rc += check_v2f64x ("vec_absf64 3:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, __DOUBLE_NINF);
  e = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, __DOUBLE_INF);
  k = vec_absf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_absf64 i=", i);
  print_v2f64x ("           k=", k);
#endif
  rc += check_v2f64x ("vec_absf64 4:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NAN, __DOUBLE_NNAN);
  e = (vf64_t) CONST_VINT128_DW(__DOUBLE_NAN, __DOUBLE_NAN);
  k = vec_absf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_absf64 i=", i);
  print_v2f64x ("           k=", k);
#endif
  rc += check_v2f64x ("vec_absf64 5:", k, e);

  i = (vf64_t) CONST_VINT128_DW( __DOUBLE_SNAN, __DOUBLE_NSNAN);
  e = (vf64_t) CONST_VINT128_DW( __DOUBLE_SNAN, __DOUBLE_SNAN);
  k = vec_absf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_absf64 i=", i);
  print_v2f64x ("           k=", k);
#endif
  rc += check_v2f64x ("vec_absf64 1:", k, e);

  return (rc);
}
#if 1
int
test_double_all_is (void)
{
  vf64_t i;
  int rc = 0;
  printf ("\n%s double is all finite\n", __FUNCTION__);

  i = (vf64_t) { 0.0, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isfinite i=", i);
#endif
  if (vec_all_isfinitef64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_all_isfinite fail", i);
    }

  i = (vf64_t) { __DBL_MAX__, __DBL_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isfinite i=", i);
#endif
  if (vec_all_isfinitef64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_all_isfinite fail", i);
    }

  i = (vf64_t) { __DBL_EPSILON__, __DBL_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isfinite i=", i);
#endif
  if (vec_all_isfinitef64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_all_isfinite fail", i);
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, 0);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isfinite i=", i);
#endif
  if (vec_all_isfinitef64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_isfinite fail", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW( 0, __DOUBLE_NINF);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isfinite i=", i);
#endif
  if (vec_all_isfinitef64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_isfinite fail", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, __DOUBLE_NINF);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isfinite i=", i);
#endif
  if (vec_all_isfinitef64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_isfinite fail", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NAN, __DOUBLE_SNAN);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isfinite i=", i);
#endif
  if (vec_all_isfinitef64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_isfinite fail", i);
    } else {
    }

  printf ("\n%s double is all infinity\n", __FUNCTION__);

  i = (vf64_t) { 0.0, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isinf i=", i);
#endif
  if (vec_all_isinff64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_isinf fail", i);
    } else {
    }

  i = (vf64_t) { __DBL_MAX__, __DBL_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isinf i=", i);
#endif
  if (vec_all_isinff64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_isinf fail", i);
    } else {
    }

  i = (vf64_t) { __DBL_EPSILON__, __DBL_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isinf i=", i);
#endif
  if (vec_all_isinff64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_isinf fail", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, 0);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isinf i=", i);
#endif
  if (vec_all_isinff64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_isinf fail", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW( 0, __DOUBLE_NINF);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isinf i=", i);
#endif
  if (vec_all_isinff64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_isinf fail", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, __DOUBLE_NINF);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isinf i=", i);
#endif
  if (vec_all_isinff64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_all_isinf fail", i);
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NAN, __DOUBLE_SNAN);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isinf i=", i);
#endif
  if (vec_all_isinff64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_isinf fail", i);
    } else {
    }

  printf ("\n%s double is all NaN\n", __FUNCTION__);

  i = (vf64_t) { 0.0, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isnan i=", i);
#endif
  if (vec_all_isnanf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_isnan fail 1", i);
    } else {
    }

  i = (vf64_t) { __DBL_MAX__, __DBL_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isnan i=", i);
#endif
  if (vec_all_isnanf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_isnan fail 2", i);
    } else {
    }

  i = (vf64_t) { __DBL_EPSILON__, __DBL_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isnan i=", i);
#endif
  if (vec_all_isnanf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_isnan fail 2", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW( __DOUBLE_NNAN, 0);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isnan i=", i);
#endif

  if (vec_all_isnanf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_isnan fail 3", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW( 0, __DOUBLE_SNAN);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isnan i=", i);
#endif

  if (vec_all_isnanf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_isnan fail 3", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, __DOUBLE_NINF);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isnan i=", i);
#endif
  if (vec_all_isnanf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_isnan fail 4", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NAN, __DOUBLE_NNAN);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isnan i=", i);
#endif
  if (vec_all_isnanf64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_all_isnan fail 5", i);
    }

  i = (vf64_t) CONST_VINT128_DW( __DOUBLE_NSNAN, __DOUBLE_SNAN );

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isnan i=", i);
#endif
  if (vec_all_isnanf64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_all_isnan fail 5", i);
    }

  printf ("\n%s double is all Normal\n", __FUNCTION__);

  i = (vf64_t) { 0.0, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isnormal i=", i);
#endif
  if (vec_all_isnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_isnormal fail", i);
    } else {
    }

  i = (vf64_t) { __DBL_MAX__, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isnormal i=", i);
#endif
  if (vec_all_isnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_isnormal fail", i);
    } else {
    }

  i = (vf64_t) { __DBL_MIN__, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isnormal i=", i);
#endif
  if (vec_all_isnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_isnormal fail", i);
    } else {
    }

  i = (vf64_t) { __DBL_EPSILON__, -1.0 };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isnormal i=", i);
#endif
  if (vec_all_isnormalf64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_all_isnormal fail", i);
    }

  i = (vf64_t) { __DBL_MIN__, __DBL_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isnormal i=", i);
#endif
  if (vec_all_isnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_isnormal fail", i);
    } else {
    }

  i = (vf64_t) { __DBL_DENORM_MIN__, __DBL_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isnormal i=", i);
#endif
  if (vec_all_isnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_isnormal fail", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW(0, 1 );

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isnormal i=", i);
#endif
  if (vec_all_isnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_isnormal fail", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NNAN, 0xff000000);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isnormal i=", i);
#endif
  if (vec_all_isnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_isnormal fail", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, __DOUBLE_NINF);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isnormal i=", i);
#endif
  if (vec_all_isnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_isnormal fail", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NAN, __DOUBLE_NNAN);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isnormal i=", i);
#endif
  if (vec_all_isnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_isnormal fail", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW( __DOUBLE_NSNAN, __DOUBLE_SNAN) ;

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_isnormal i=", i);
#endif
  if (vec_all_isnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_isnormal fail", i);
    } else {
    }

  printf ("\n%s double is all Subnormal\n", __FUNCTION__);

  i = (vf64_t) { 0.0, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_issubnormal i=", i);
#endif
  if (vec_all_issubnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_issubnormal fail", i);
    } else {
    }

  i = (vf64_t) { __DBL_MAX__, __DBL_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_issubnormal i=", i);
#endif
  if (vec_all_issubnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_issubnormal fail", i);
    } else {
    }

  i = (vf64_t) { __DBL_EPSILON__, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_issubnormal i=", i);
#endif
  if (vec_all_issubnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_issubnormal fail", i);
    } else {
    }

  i = (vf64_t) { __DBL_MAX__, __DBL_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_issubnormal i=", i);
#endif
  if (vec_all_issubnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_issubnormal fail", i);
    } else {
    }

  i = (vf64_t) {__DBL_MIN__, __DBL_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_issubnormal i=", i);
#endif
  if (vec_all_issubnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_issubnormal fail", i);
    } else {
    }

  i = (vf64_t) { __DBL_EPSILON__, __DBL_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_issubnormal i=", i);
#endif
  if (vec_all_issubnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_issubnormal fail", i);
    } else {
    }

  i = (vf64_t) { __DBL_DENORM_MIN__, __DBL_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_issubnormal i=", i);
#endif
  if (vec_all_issubnormalf64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_all_issubnormal fail", i);
    }

  i = (vf64_t) { -0.0, __DBL_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_issubnormal i=", i);
#endif
  if (vec_all_issubnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_issubnormal fail", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NNAN, 1);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_issubnormal i=", i);
#endif
  if (vec_all_issubnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_issubnormal fail", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, __DOUBLE_NINF);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_issubnormal i=", i);
#endif
  if (vec_all_issubnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_issubnormal fail", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NAN, __DOUBLE_NNAN);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_issubnormal i=", i);
#endif
  if (vec_all_issubnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_issubnormal fail", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW( __DOUBLE_NSNAN, __DOUBLE_SNAN);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_issubnormal i=", i);
#endif
  if (vec_all_issubnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_issubnormal fail", i);
    } else {
    }

  printf ("\n%s double is all Zero\n", __FUNCTION__);

  i = (vf64_t) { 0.0, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_iszero i=", i);
#endif
  if (vec_all_iszerof64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_all_iszero fail", i);
    }

  i = (vf64_t) { __DBL_MAX__, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_iszero i=", i);
#endif
  if (vec_all_iszerof64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_iszero fail", i);
    } else {
    }

  i = (vf64_t) { __DBL_MIN__, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_iszero i=", i);
#endif
  if (vec_all_iszerof64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_iszero fail", i);
    } else {
    }

  i = (vf64_t) { __DBL_EPSILON__, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_iszero i=", i);
#endif
  if (vec_all_iszerof64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_iszero fail", i);
    } else {
    }

  i = (vf64_t) { __DBL_DENORM_MIN__, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_iszero i=", i);
#endif
  if (vec_all_iszerof64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_iszero fail", i);
    } else {
    }

  i = (vf64_t) { __DBL_MAX__, __DBL_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_iszero i=", i);
#endif
  if (vec_all_iszerof64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_iszero fail", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NNAN, 0x8000000000000000UL);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_iszero i=", i);
#endif
  if (vec_all_iszerof64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_iszero fail", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, __DOUBLE_NINF);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_iszero i=", i);
#endif
  if (vec_all_iszerof64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_iszero fail", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NAN, __DOUBLE_NNAN);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_iszero i=", i);
#endif
  if (vec_all_iszerof64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_iszero fail", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW( __DOUBLE_NSNAN, __DOUBLE_SNAN );

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_all_iszero i=", i);
#endif
  if (vec_all_iszerof64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_all_iszero fail", i);
    } else {
    }

  return (rc);
}
#endif
#if 1
int
test_double_any_is (void)
{
  vf64_t i;
  int rc = 0;

  printf ("\n%s double is any finite\n", __FUNCTION__);

  i = (vf64_t) { 0.0, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isfinite i=", i);
#endif
  if (vec_any_isfinitef64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_isfinite fail", i);
    }

  i = (vf64_t) { __DBL_MAX__, __DBL_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isfinite i=", i);
#endif
  if (vec_any_isfinitef64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_isfinite fail", i);
    }

  i = (vf64_t) { __DBL_EPSILON__, __DBL_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isfinite i=", i);
#endif
  if (vec_any_isfinitef64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_isfinite fail", i);
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, 0 );

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isfinite i=", i);
#endif
  if (vec_any_isfinitef64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_isfinite fail", i);
    }

  i = (vf64_t) CONST_VINT128_DW( __DOUBLE_NINF, 0 );

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isfinite i=", i);
#endif
  if (vec_any_isfinitef64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_isfinite fail", i);
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, __DOUBLE_NINF);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isfinite i=", i);
#endif
  if (vec_any_isfinitef64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_isfinite fail", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NAN, __DOUBLE_SNAN);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isfinite i=", i);
#endif
  if (vec_any_isfinitef64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_isfinite fail", i);
    } else {
    }

  printf ("\n%s double is any infinity\n", __FUNCTION__);

  i = (vf64_t) { 0.0, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isinf i=", i);
#endif
  if (vec_any_isinff64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_isinf fail", i);
    } else {
    }

  i = (vf64_t) { __DBL_MAX__, __DBL_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isinf i=", i);
#endif
  if (vec_any_isinff64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_isinf fail", i);
    } else {
    }

  i = (vf64_t) { __DBL_EPSILON__, __DBL_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isinf i=", i);
#endif
  if (vec_any_isinff64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_isinf fail", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, 0 );

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isinf i=", i);
#endif
  if (vec_any_isinff64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_isinf fail", i);
    }

  i = (vf64_t) CONST_VINT128_DW( __DOUBLE_NINF, 0 );

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isinf i=", i);
#endif
  if (vec_any_isinff64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_isinf fail", i);
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, __DOUBLE_NINF);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isinf i=", i);
#endif
  if (vec_any_isinff64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_isinf fail", i);
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NAN, __DOUBLE_SNAN);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isinf i=", i);
#endif
  if (vec_any_isinff64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_isinf fail", i);
    } else {
    }

  printf ("\n%s double is any NaN\n", __FUNCTION__);

  i = (vf64_t) { 0.0, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isnan i=", i);
#endif
  if (vec_any_isnanf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_isnan fail", i);
    } else {
    }

  i = (vf64_t) { __DBL_MAX__, __DBL_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isnan i=", i);
#endif
  if (vec_any_isnanf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_isnan fail", i);
    } else {
    }

  i = (vf64_t) { __DBL_EPSILON__, __DBL_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isnan i=", i);
#endif
  if (vec_any_isnanf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_isnan fail", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW( __DOUBLE_NNAN, 0 );

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isnan i=", i);
#endif
  if (vec_any_isnanf64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_isnan fail", i);
    }

  i = (vf64_t) CONST_VINT128_DW( __DOUBLE_NAN, 0 );

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isnan i=", i);
#endif
  if (vec_any_isnanf64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_isnan fail", i);
    }

  i = (vf64_t) CONST_VINT128_DW( 0, __DOUBLE_SNAN );

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isnan i=", i);
#endif
  if (vec_any_isnanf64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_isnan fail", i);
    }

  i = (vf64_t) CONST_VINT128_DW( 0, __DOUBLE_NSNAN );

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isnan i=", i);
#endif
  if (vec_any_isnanf64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_isnan fail", i);
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, __DOUBLE_NINF);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isnan i=", i);
#endif
  if (vec_any_isnanf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_isnan fail", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NAN, __DOUBLE_NNAN);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isnan i=", i);
#endif
  if (vec_any_isnanf64 (i))
    {
    } else {
	      rc += 1;
	      print_v2f64x ("vec_any_isnan fail", i);
    }

  i = (vf64_t) CONST_VINT128_DW( __DOUBLE_NSNAN, __DOUBLE_SNAN);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isnan i=", i);
#endif
  if (vec_any_isnanf64 (i))
    {
    } else {
	      rc += 1;
	      print_v2f64x ("vec_any_isnan fail", i);
    }

  printf ("\n%s double is any Normal\n", __FUNCTION__);

  i = (vf64_t) { 0.0, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isnormal i=", i);
#endif
  if (vec_any_isnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_isnormal fail 1", i);
    } else {
    }

  i = (vf64_t) { __DBL_MAX__, __DBL_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isnormal i=", i);
#endif
  if (vec_any_isnormalf64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_isnormal fail 2", i);
    }

  i = (vf64_t) { __DBL_EPSILON__, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isnormal i=", i);
#endif
  if (vec_any_isnormalf64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_isnormal fail 3", i);
    }

  i = (vf64_t) { __DBL_MAX__, __DBL_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isnormal i=", i);
#endif
  if (vec_any_isnormalf64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_isnormal fail 4", i);
    }

  i = (vf64_t) { __DBL_DENORM_MIN__, __DBL_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isnormal i=", i);
#endif
  if (vec_any_isnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_isnormal fail 5", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW(0, 0xff00000000000000UL );

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isnormal i=", i);
#endif
  if (vec_any_isnormalf64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_isnormal fail 6", i);
    }

  i = (vf64_t) CONST_VINT128_DW( 1, __DOUBLE_INF );

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isnormal i=", i);
#endif
  if (vec_any_isnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_isnormal fail 7", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NNAN, 0xff00000000000000UL);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isnormal i=", i);
#endif
  if (vec_any_isnormalf64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_isnormal fail 8", i);
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, __DOUBLE_NINF);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isnormal i=", i);
#endif
  if (vec_any_isnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_isnormal fail 9", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NAN, __DOUBLE_NNAN );

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isnormal i=", i);
#endif
  if (vec_any_isnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_isnormal fail 10", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW( __DOUBLE_NSNAN, __DOUBLE_SNAN);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isnormal i=", i);
#endif
  if (vec_any_isnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_isnormal fail 11", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW( 0x7ffff, 1);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isnormal i=", i);
#endif
  if (vec_any_isnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_isnormal fail 12", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NAN, __DOUBLE_INF);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isnormal i=", i);
#endif
  if (vec_any_isnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_isnormal fail 13", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW( 0x0010000000000000UL,
			       1);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_isnormal i=", i);
#endif
  if (vec_any_isnormalf64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_isnormal fail 14", i);
    }

  printf ("\n%s double is any Subnormal\n", __FUNCTION__);

  i = (vf64_t) { 0.0, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_issubnormal i=", i);
#endif
  if (vec_any_issubnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_issubnormal fail 1", i);
    } else {
    }

  i = (vf64_t) { __DBL_MAX__, __DBL_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_issubnormal i=", i);
#endif

  if (vec_any_issubnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_issubnormal fail 2", i);
    } else {
    }

  i = (vf64_t) { __DBL_DENORM_MIN__, __DBL_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_issubnormal i=", i);
#endif

  if (vec_any_issubnormalf64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_issubnormal fail 3", i);
    }

  i = (vf64_t) { __DBL_MAX__, __DBL_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_issubnormal i=", i);
#endif

  if (vec_any_issubnormalf64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_issubnormal fail 4", i);
    }

  i = (vf64_t) { __DBL_EPSILON__, __DBL_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_issubnormal i=", i);
#endif
  if (vec_any_issubnormalf64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_issubnormal fail 5", i);
    }

  i = (vf64_t) CONST_VINT128_DW(0x8000000000000001UL, 0x000fffffffffffffUL );

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_issubnormal i=", i);
#endif
  if (vec_any_issubnormalf64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_issubnormal fail 6", i);
    }

  i = (vf64_t) CONST_VINT128_DW( 1, 0x800fffffffffffffUL );

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_issubnormal i=", i);
#endif
  if (vec_any_issubnormalf64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_issubnormal fail 7", i);
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NNAN, 1);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_issubnormal i=", i);
#endif
  if (vec_any_issubnormalf64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_issubnormal fail 8", i);
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, __DOUBLE_NINF);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_issubnormal i=", i);
#endif
  if (vec_any_issubnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_issubnormal fail 9", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW( __DOUBLE_NAN, __DOUBLE_NNAN );

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_issubnormal i=", i);
#endif
  if (vec_any_issubnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_issubnormal fail 10", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW( __DOUBLE_NSNAN, __DOUBLE_SNAN);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_issubnormal i=", i);
#endif
  if (vec_any_issubnormalf64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_issubnormal fail 11", i);
    } else {
    }

  printf ("\n%s double is any Zero\n", __FUNCTION__);

  i = (vf64_t) { 0.0, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_iszero i=", i);
#endif
  if (vec_any_iszerof64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_iszero fail", i);
    }

  i = (vf64_t) { __DBL_MAX__ -0.0 };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_iszero i=", i);
#endif
  if (vec_any_iszerof64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_iszero fail", i);
    }

  i = (vf64_t) { __DBL_MIN__, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_iszero i=", i);
#endif
  if (vec_any_iszerof64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_iszero fail", i);
    }

  i = (vf64_t) { __DBL_EPSILON__, -0.0 };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_iszero i=", i);
#endif
  if (vec_any_iszerof64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_iszero fail", i);
    }

  i = (vf64_t) { __DBL_MAX__, __DBL_DENORM_MIN__ };

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_iszero i=", i);
#endif
  if (vec_any_iszerof64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_iszero fail", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW( 0x800000000001UL, 0x000fffffffffffffUL );

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_iszero i=", i);
#endif
  if (vec_any_iszerof64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_iszero fail", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW( 1UL, 0x800fffffffffffffUL );

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_iszero i=", i);
#endif
  if (vec_any_iszerof64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_iszero fail", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW( __DOUBLE_NNAN, 0x8000000000000000UL );

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_iszero i=", i);
#endif
  if (vec_any_iszerof64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_iszero fail", i);
    }

  i = (vf64_t) CONST_VINT128_DW( __DOUBLE_INF, 0x8000000000000000UL );

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_iszero i=", i);
#endif
  if (vec_any_iszerof64 (i))
    {
    } else {
      rc += 1;
      print_v2f64x ("vec_any_iszero fail", i);
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, __DOUBLE_NINF);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_iszero i=", i);
#endif
  if (vec_any_iszerof64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_iszero fail", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NAN, __DOUBLE_NNAN);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_iszero i=", i);
#endif
  if (vec_any_iszerof64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_iszero fail", i);
    } else {
    }

  i = (vf64_t) CONST_VINT128_DW( __DOUBLE_NSNAN, __DOUBLE_SNAN );

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_any_iszero i=", i);
#endif
  if (vec_any_iszerof64 (i))
    {
      rc += 1;
      print_v2f64x ("vec_any_iszero fail", i);
    } else {
    }

  return (rc);
}
#endif
int
test_double_cpsgn (void)
{
  vf64_t i, j, e;
  vf64_t k;
  int rc = 0;

  printf ("\n%s double Copy Sign\n", __FUNCTION__);

  i = (vf64_t) { 0.0, -0.0 };
  j = (vf64_t) { -0.0, 0.0 };
  e = (vf64_t) { 0.0, -0.0 };
  k = vec_copysignf64 (i, j);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_copysignf64 i=", i);
  print_v2f64x ("                j=", j);
  print_v2f64x ("                k=", k);
#endif
  rc += check_v2f64x ("vec_copysignf64 1:", k, e);

  i = (vf64_t) { -0.0, 0.0 };
  j = (vf64_t) { __DBL_MAX__, __DBL_MIN__ };
  e = (vf64_t) { -(__DBL_MAX__), __DBL_MIN__ };
  k = vec_copysignf64 (i, j);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_copysignf64 i=", i);
  print_v2f64x ("                j=", j);
  print_v2f64x ("                k=", k);
#endif
  rc += check_v2f64x ("vec_copysignf64 2:", k, e);

  i = (vf64_t) { -0.0, 0.0 };
  j = (vf64_t) { __DBL_EPSILON__, __DBL_DENORM_MIN__ };
  e = (vf64_t) { -(__DBL_EPSILON__), __DBL_DENORM_MIN__ };
  k = vec_copysignf64 (i, j);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_copysignf64 i=", i);
  print_v2f64x ("                j=", j);
  print_v2f64x ("                k=", k);
#endif
  rc += check_v2f64x ("vec_copysignf64 3:", k, e);

  i = (vf64_t) CONST_VINT64_DW(0.0, -0.0);
  j = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, __DOUBLE_NINF);
  e = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, __DOUBLE_NINF);
  k = vec_copysignf64 (i, j);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_copysignf64 i=", i);
  print_v2f64x ("                j=", j);
  print_v2f64x ("                k=", k);
#endif
  rc += check_v2f64x ("vec_copysignf64 4:", k, e);

  i = (vf64_t) CONST_VINT64_DW(0.0, -0.0);
  j = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, __DOUBLE_NINF);
  e = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, __DOUBLE_NINF);
  k = vec_copysignf64 (i, j);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_copysignf64 i=", i);
  print_v2f64x ("                j=", j);
  print_v2f64x ("                k=", k);
#endif
  rc += check_v2f64x ("vec_copysignf64 5:", k, e);

  i = (vf64_t) CONST_VINT64_DW( -0.0, 0.0 );
  j = (vf64_t) CONST_VINT128_DW(__DOUBLE_NAN, __DOUBLE_NNAN);
  e = (vf64_t) CONST_VINT128_DW(__DOUBLE_NNAN, __DOUBLE_NAN);
  k = vec_copysignf64 (i, j);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_copysignf64 i=", i);
  print_v2f64x ("                j=", j);
  print_v2f64x ("                k=", k);
#endif
  rc += check_v2f64x ("vec_copysignf64 6:", k, e);

  i = (vf64_t) CONST_VINT64_DW ( 0.0, -0.0 );
  j = (vf64_t) CONST_VINT128_DW(__DOUBLE_NSNAN, __DOUBLE_SNAN);
  e = (vf64_t) CONST_VINT128_DW(__DOUBLE_SNAN, __DOUBLE_NSNAN);
  k = vec_copysignf64 (i, j);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_copysignf64 i=", i);
  print_v2f64x ("                j=", j);
  print_v2f64x ("                k=", k);
#endif
  rc += check_v2f64x ("vec_copysignf64 7:", k, e);

  return (rc);
}

int
test_double_isfinite (void)
{
  vf64_t i;
  vb64_t e, k;
  int rc = 0;

  printf ("\n%s double isfinite\n", __FUNCTION__);

  i = (vf64_t){ 0.0, -0.0 };
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_TRUE, __DOUBLE_TRUE);
  k = vec_isfinitef64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isfinitef64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isfinitef64 1:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, __DOUBLE_NINF);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_NTRUE);
  k = vec_isfinitef64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isfinitef64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isfinitef64 2:", k, e);

  i = (vf64_t) CONST_VINT128_DW(0x7fefffffffffffffUL, 0x0001000000000000UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_TRUE, __DOUBLE_TRUE);
  k = vec_isfinitef64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isfinitef64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isfinitef64 3:", k, e);

  i = (vf64_t) CONST_VINT128_DW(0x3ff0000000000000UL, 0x8000000000000001UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_TRUE, __DOUBLE_TRUE);
  k = vec_isfinitef64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isfinitef64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isfinitef64 4:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, 0x0001000000000000UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_TRUE);
  k = vec_isfinitef64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isfinitef64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isfinitef64 5:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, 0x3ff0000000000000UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_TRUE);
  k = vec_isfinitef64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isfinitef64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isfinitef64 6:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, 0x8000000000000001UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_TRUE);
  k = vec_isfinitef64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isfinitef64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isfinitef64 7:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NINF, 0x3ff0000000000000UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_TRUE);
  k = vec_isfinitef64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isfinitef64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isfinitef64 8:", k, e);

  i = (vf64_t) CONST_VINT128_DW(0x3ff0000000000000UL, __DOUBLE_NINF);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_TRUE, __DOUBLE_NTRUE);
  k = vec_isfinitef64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isfinitef64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isfinitef64 9:", k, e);

  return (rc);
}

int
test_double_isinf (void)
{
  vf64_t i;
  vb64_t e, k;
  int rc = 0;

  printf ("\n%s double isinf\n", __FUNCTION__);

  i = (vf64_t){ 0.0, -0.0 };
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_NTRUE);
  k = vec_isinff64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isinff64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isinff64 2:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, __DOUBLE_NINF);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_TRUE, __DOUBLE_TRUE);
  k = vec_isinff64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isinff64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isinff64 3:", k, e);

  i = (vf64_t) CONST_VINT128_DW(0x7fefffffffffffffUL, 0x0001000000000000UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_NTRUE);
  k = vec_isinff64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isinff64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isinff64 4:", k, e);

  i = (vf64_t) CONST_VINT128_DW(0x3ff0000000000000UL, 0x8000000000000001UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_NTRUE);
  k = vec_isinff64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isinff64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isinff64 4:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, 0x0001000000000000UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_TRUE, __DOUBLE_NTRUE);
  k = vec_isinff64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isinff64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isinff64 5:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, 0x3ff0000000000000UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_TRUE, __DOUBLE_NTRUE);
  k = vec_isinff64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isinff64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isinff64 5:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_INF, 0x8000000000000001UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_TRUE, __DOUBLE_NTRUE);
  k = vec_isinff64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isinff64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isinff64 5:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NINF, 0x3ff0000000000000UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_TRUE, __DOUBLE_NTRUE);
  k = vec_isinff64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isinff64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isinff64 6:", k, e);

  i = (vf64_t) CONST_VINT128_DW(0x3ff0000000000000UL, __DOUBLE_NINF);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_TRUE);
  k = vec_isinff64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isinff64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isinff64 7:", k, e);

  return (rc);
}
//#undefine __DEBUG_PRINT__

//#define __DEBUG_PRINT__ 1
int
test_double_isnan (void)
{
  vf64_t i;
  vb64_t e, k;
  int rc = 0;

  printf ("\n%s double isnan\n", __FUNCTION__);

  i = (vf64_t){ 0.0, -0.0 };
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_NTRUE);
  k = vec_isnanf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isnanf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isnanf64 1:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NAN, __DOUBLE_NNAN);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_TRUE, __DOUBLE_TRUE);
  k = vec_isnanf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isnanf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isnanf64 2:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NSNAN, __DOUBLE_SNAN);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_TRUE, __DOUBLE_TRUE);
  k = vec_isnanf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isnanf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isnanf64 3:", k, e);

  i = (vf64_t) CONST_VINT128_DW(0x7fEfffffffffffffUL, 0x0001000000000000UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_NTRUE);
  k = vec_isnanf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isnanf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isnanf64 4:", k, e);

  i = (vf64_t) CONST_VINT128_DW(0x3ff0000000000000UL, 0x8000000000000001UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_NTRUE);
  k = vec_isnanf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isnanf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isnanf64 5:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NAN, 0x0001000000000000UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_TRUE, __DOUBLE_NTRUE);
  k = vec_isnanf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isnanf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isnanf64 6:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NAN, 0x3ff0000000000000UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_TRUE, __DOUBLE_NTRUE);
  k = vec_isnanf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isnanf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isnanf64 7:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NAN, 0x8000000000000001UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_TRUE, __DOUBLE_NTRUE);
  k = vec_isnanf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isnanf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isnanf64 8:", k, e);

  i = (vf64_t) CONST_VINT128_DW(0x7fefffffffffffffUL, __DOUBLE_NNAN);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_TRUE);
  k = vec_isnanf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isnanf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isnanf64 9:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NNAN, 0x3ff0000000000000UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_TRUE, __DOUBLE_NTRUE);
  k = vec_isnanf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isnanf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isnanf64 10:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NNAN, 0x8000000000000001UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_TRUE, __DOUBLE_NTRUE);
  k = vec_isnanf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isnanf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isnanf64 11:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_SNAN, __DOUBLE_NNAN);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_TRUE, __DOUBLE_TRUE);
  k = vec_isnanf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isnanf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isnanf64 12:", k, e);

  return (rc);
}
//#undef __DEBUG_PRINT__

//#define __DEBUG_PRINT__ 1
#ifdef __DEBUG_PRINT__
#define test_vec_isnormalf64(_i)	db_vec_isnormalf64(_i)
#else
#define test_vec_isnormalf64(_i)	vec_isnormalf64(_i)
#endif
int
test_double_isnormal (void)
{
  vf64_t i;
  vb64_t e, k;
  int rc = 0;

  printf ("\n%s double isnormal\n", __FUNCTION__);

  i = (vf64_t){ 0.0, -0.0 };
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_NTRUE);
  k = test_vec_isnormalf64(i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isnormalf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isnormalf64 1:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NSNAN, __DOUBLE_SNAN);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_NTRUE);
  k = test_vec_isnormalf64(i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isnormalf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isnormalf64 2:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NAN, __DOUBLE_NNAN);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_NTRUE);
  k = test_vec_isnormalf64(i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isnormalf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isnormalf64 3:", k, e);

  i = (vf64_t) CONST_VINT128_DW(0x7fefffffffffffffUL, 0x0010000000000000UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_TRUE, __DOUBLE_TRUE);
  k = test_vec_isnormalf64(i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isnormalf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isnormalf64 4:", k, e);

  i = (vf64_t) CONST_VINT128_DW(0x3ff0000000000000UL, 0x8000000000000001UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_TRUE, __DOUBLE_NTRUE);
  k = test_vec_isnormalf64(i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isnormalf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isnormalf64 5:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NAN, 0x0010000000000000UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_TRUE);
  k = test_vec_isnormalf64(i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isnormalf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isnormalf64 6:", k, e);

  i = (vf64_t) CONST_VINT128_DW(0x3ff0000000000000UL, 0x0000000000000001UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_TRUE, __DOUBLE_NTRUE);
  k = test_vec_isnormalf64(i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isnormalf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isnormalf64 7:", k, e);

  i = (vf64_t) CONST_VINT128_DW(0x7fefffffffffffffUL, __DOUBLE_NNAN);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_TRUE, __DOUBLE_NTRUE);
  k = test_vec_isnormalf64(i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isnormalf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isnormalf64 8:", k, e);

  i = (vf64_t) CONST_VINT128_DW(0x3ff0000000000000UL, 0x8000000000000001UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_TRUE, __DOUBLE_NTRUE);
  k = test_vec_isnormalf64(i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isnormalf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isnormalf64 9:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_SNAN, 0x0010000000000000UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_TRUE);
  k = test_vec_isnormalf64(i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isnormalf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isnormalf64 10:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NINF, 0x8000000000000001UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_NTRUE);
  k = test_vec_isnormalf64(i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_isnormalf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_isnormalf64 11:", k, e);

  return (rc);
}
//#undef __DEBUG_PRINT__

int
test_double_issubnormal (void)
{
  vf64_t i;
  vb64_t e, k;
  int rc = 0;

  printf ("\n%s double issubnormal\n", __FUNCTION__);

  i = (vf64_t) { 0.0, -0.0 };
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_NTRUE);
  k = vec_issubnormalf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_issubnormalf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_issubnormalf64 1:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NAN, __DOUBLE_NNAN);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_NTRUE);
  k = vec_issubnormalf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_issubnormalf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_issubnormalf64 2:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NSNAN, __DOUBLE_SNAN);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_NTRUE);
  k = vec_issubnormalf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_issubnormalf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_issubnormalf64 3:", k, e);

  i = (vf64_t) CONST_VINT128_DW(0x7fefffffffffffffUL, 0x0008000000000000UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_TRUE);
  k = vec_issubnormalf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_issubnormalf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_issubnormalf64 4:", k, e);

  i = (vf64_t) CONST_VINT128_DW(0x3ff0000000000000UL, 0x8000000000000001UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_TRUE);
  k = vec_issubnormalf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_issubnormalf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_issubnormalf64 5:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NAN, 0x0010000000000000UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_NTRUE);
  k = vec_issubnormalf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_issubnormalf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_issubnormalf64 6:", k, e);

  i = (vf64_t) CONST_VINT128_DW(0x3ff0000000000000UL, 0x0000000000000001UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_TRUE);
  k = vec_issubnormalf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_issubnormalf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_issubnormalf64 7:", k, e);

  i = (vf64_t) CONST_VINT128_DW(0x7fefffffffffffffUL, __DOUBLE_NNAN);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_NTRUE);
  k = vec_issubnormalf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_issubnormalf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_issubnormalf64 8:", k, e);

  i = (vf64_t) CONST_VINT128_DW(0x3ff0000000000000UL, 0x8000000000000001UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_TRUE);
  k = vec_issubnormalf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_issubnormalf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_issubnormalf64 9:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_SNAN, 0x0010000000000000UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_NTRUE);
  k = vec_issubnormalf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_issubnormalf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_issubnormalf64 10:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NINF, 0x8000000000000001UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_TRUE);
  k = vec_issubnormalf64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_issubnormalf64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_issubnormalf64 11:", k, e);

  return (rc);
}

int
test_double_iszero (void)
{
  vf64_t i;
  vb64_t e, k;
  int rc = 0;

  printf ("\n%s double iszero\n", __FUNCTION__);

  i = (vf64_t){ 0.0, -0.0 };
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_TRUE, __DOUBLE_TRUE);
  k = vec_iszerof64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_iszerof64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_iszerof64 1:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NAN, __DOUBLE_NNAN);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_NTRUE);
  k = vec_iszerof64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_iszerof64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_iszerof64 2:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NINF, __DOUBLE_SNAN);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_NTRUE);
  k = vec_iszerof64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_iszerof64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_iszerof64 3:", k, e);

  i = (vf64_t) CONST_VINT128_DW(0x7fefffffffffffffUL, 0x0008000000000000UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_NTRUE);
  k = vec_iszerof64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_iszerof64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_iszerof64 4:", k, e);

  i = (vf64_t) CONST_VINT128_DW(0x3ff0000000000000UL, 0x8000000000000000UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_TRUE);
  k = vec_iszerof64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_iszerof64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_iszerof64 4:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NAN, 0x8000000000000000UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_TRUE);
  k = vec_iszerof64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_iszerof64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_iszerof64 5:", k, e);

  i = (vf64_t) CONST_VINT128_DW(0x3ff0000000000000UL, 0x0000000000000001UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_NTRUE);
  k = vec_iszerof64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_iszerof64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_iszerof64 5:", k, e);

  i = (vf64_t) CONST_VINT128_DW(0x3ff0000000000000UL, 0x8000000000000001UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_NTRUE);
  k = vec_iszerof64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_iszerof64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_iszerof64 6:", k, e);

  i = (vf64_t) CONST_VINT128_DW(0x7fefffffffffffffUL, __DOUBLE_NNAN);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_NTRUE);
  k = vec_iszerof64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_iszerof64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_iszerof64 6:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_SNAN, 0x0010000000000000UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_NTRUE);
  k = vec_iszerof64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_iszerof64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_iszerof64 7:", k, e);

  i = (vf64_t) CONST_VINT128_DW(__DOUBLE_NINF, 0x8000000000000001UL);
  e = (vb64_t) CONST_VINT128_DW(__DOUBLE_NTRUE, __DOUBLE_NTRUE);
  k = vec_iszerof64 (i);

#ifdef __DEBUG_PRINT__
  print_v2f64x ("vec_iszerof64 i=", i);
  print_v2b64c ("             k=", k);
  print_v2b64x ("             k=", k);
#endif
  rc += check_v2b64c ("vec_iszerof64 7:", k, e);

  return (rc);
}
//#undef __DEBUG_PRINT__


#define TIMING_ITERATIONS 10
#if 1
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

  return (rc);
}
#endif
int
test_vec_f64 (void)
{
  int rc = 0;

  printf ("\n%s\n", __FUNCTION__);

  rc += test_double_abs ();
  rc += test_double_all_is ();
  rc += test_double_any_is ();
  rc += test_double_cpsgn ();
  rc += test_double_isinf ();
  rc += test_double_isnan ();
  rc += test_double_isnormal ();
  rc += test_double_issubnormal ();
  rc += test_double_iszero ();
  rc += test_double_isfinite ();

  rc += test_time_f64 ();

  return (rc);
}

