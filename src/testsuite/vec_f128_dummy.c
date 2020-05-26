/*
 Copyright (c) [2017, 2018] IBM Corporation.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 arith128_test_f128.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Jul 31, 2017
      Commited on: Oct 10, 2018
 */

#include <stdint.h>
#include <stdio.h>
#include <fenv.h>
#include <float.h>
#ifdef __FLOAT128_TYPE__
#define __STDC_WANT_IEC_60559_TYPES_EXT__ 1
#define __STDC_WANT_IEC_60559_FUNCS_EXT__ 1
#endif
#ifndef __clang__
/* Disable <math.h> for __clang__ because of a bug involving <floatn.h>
   incombination with -mcpu=power9 -mfloat128. This means that ISO
   mandated <math.h> functions __finitef128, __isinff128, etc are not
   available.  */ 
#include <math.h>
#endif

//#define __DEBUG_PRINT__
#include <pveclib/vec_f128_ppc.h>

__binary128
test_vec_absf128 (__binary128 f128)
{
  return vec_absf128 (f128);
}

__binary128
test_const_vec_huge_valf128 (void)
{
  return vec_const_huge_valf128 ();
}

__binary128
test_const_vec_nanf128 (void)
{
  return vec_const_nanf128 ();
}

__binary128
test_const_vec_nansf128 (void)
{
  return vec_const_nansf128 ();
}

__binary128
test_const_vec_inff128 (void)
{
  return vec_const_inff128 ();
}

__binary128
test_vec_copysignf128 (__binary128 f128x , __binary128 f128y)
{
  return vec_copysignf128 (f128x , f128y);
}

vb128_t
test_vec_isfinitef128 (__binary128 f128)
{
  return vec_isfinitef128 (f128);
}

vb128_t
test_vec_isinff128 (__binary128 value)
{
  return (vec_isinff128 (value));
}

vb128_t
test_vec_isnanf128 (__binary128 value)
{
  return (vec_isnanf128 (value));
}

vb128_t
test_vec_isnormalf128 (__binary128 f128)
{
  return vec_isnormalf128 (f128);
}

vb128_t
test_vec_issubnormalf128 (__binary128 f128)
{
  return vec_issubnormalf128 (f128);
}

vb128_t
test_vec_iszerof128 (__binary128 f128)
{
  return vec_iszerof128 (f128);
}

int
test_vec_all_finitef128 (__binary128 value)
{
  return (vec_all_isfinitef128 (value));
}

int
test_vec_all_inff128 (__binary128 value)
{
  return (vec_all_isinff128 (value));
}

int
test_vec_all_nanf128 (__binary128 value)
{
  return (vec_all_isnanf128 (value));
}

int
test_vec_all_normalf128 (__binary128 value)
{
  return (vec_all_isnormalf128 (value));
}

int
test_vec_all_subnormalf128 (__binary128 value)
{
  return (vec_all_issubnormalf128 (value));
}

int
test_vec_all_zerof128 (__binary128 value)
{
  return (vec_all_iszerof128 (value));
}
#ifdef __FLOAT128_TYPE__
/* dummy sinf128 example. From Posix:
 * If value is NaN then return a NaN.
 * If value is +-0.0 then return value.
 * If value is subnormal then return value.
 * If value is +-Inf then return a NaN.
 * Otherwise compute and return sin(value).
 */
__binary128
test_sinf128 (__binary128 value)
  {
    __binary128 result;
#ifdef __FLOAT128__
    // requires -mfloat128 to use Q const
    const __binary128 zeroF128 = 0.0Q;
#else
    const __binary128 zeroF128 = (__binary128)CONST_VINT128_W(0, 0, 0, 0);
#endif

    if (vec_all_isnormalf128 (value))
      {
	/* body of vec_sin() computation elided for this example.  */
	result = zeroF128;
      }
    else
      {
	if (vec_all_isinff128 (value))
	  result = vec_const_nanf128 ();
	else
	  result = value;
      }

    return result;
  }

/* dummy cosf128 example. From Posix:
 * If value is NaN then return a NaN.
 * If value is +-0.0 then return 1.0.
 * If value is +-Inf then return a NaN.
 * Otherwise compute and return sin(value).
 */
__binary128
test_cosf128 (__binary128 value)
  {
    __binary128 result;
#ifdef __FLOAT128__
    // requires -mfloat128 to use Q const
    const __binary128 zeroF128 = 0.0Q;
    const __binary128 oneF128 = 1.0Q;
#else
    const __binary128 zeroF128 = (__binary128)CONST_VINT128_W(0, 0, 0, 0);
    const __binary128 oneF128 = (__binary128)CONST_VINT128_W(0x3fff0000, 0, 0, 0);
#endif

    if (vec_all_isfinitef128 (value))
      {
	if (vec_all_iszerof128 (value))
	  result = oneF128;
	else
	  {
	    /* body of vec_cos() computation elided for this example.  */
            result = zeroF128;
	  }
      }
    else
      {
	if (vec_all_isinff128 (value))
	  result = vec_const_nanf128 ();
	else
	  result = value;
      }

    return result;
  }
#endif

vb128_t
_test_f128_isinff128 (__Float128 value)
{
  return (vec_isinff128 (value));
}

int
_test_f128_isinf_sign (__Float128 value)
{
  return (vec_isinf_signf128 (value));
}

vb128_t
_test_f128_isnan (__Float128 value)
{
  return (vec_isnanf128 (value));
}

vb128_t
_test_pred_f128_finite (__Float128 value)
{
  return (vec_isfinitef128 (value));
}

vb128_t
_test_pred_f128_normal (__Float128 value)
{
  return (vec_isnormalf128 (value));
}

vb128_t
_test_pred_f128_subnormal (__Float128 value)
{
  return (vec_issubnormalf128 (value));
}

vui16_t
_test_xfer_bin128_2_vui16t (__binary128 f128)
{
  return vec_xfer_bin128_2_vui16t (f128);
}

#ifdef __FLOAT128__
/* Mostly compiler and library tests follow to see what the various
 * compilers will do. */

#ifndef __clang__ // to disable __clang__ because <math.h> fails
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
int
test_gcc_f128_signbit (__Float128 value)
  {
    return (__signbitf128(value));
  }

int
test_gcc_f128_isinf (__Float128 value)
  {
    return (__isinff128(value));
  }

int
test_gcc_float128_isnan (__Float128 value)
  {
    return (__isnanf128(value));
  }

__Float128
test_gcc_f128_copysign (__Float128 valx, __Float128 valy)
  {
    return (__copysignf128(valx, valy));
  }

int
test_glibc_f128_classify (__Float128 value)
  {
    if (__finitef128(value))
    return 1;

    if (__isinff128(value))
    return 2;

    if (__isnanf128(value))
    return 3;
    /* finite */
    return 0;
  }
#endif
#endif
#endif

