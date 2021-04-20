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
#ifndef PVECLIB_DISABLE_F128MATH
/* Disable <math.h> for __clang__ because of a bug involving <floatn.h>
   incombination with -mcpu=power9 -mfloat128. This means that ISO
   mandated <math.h> functions signbit(), isfinite(), isnormal(),
   isinf(), isnan(), etc are not available for __float128.  */
#include <math.h>
#endif

//#define __DEBUG_PRINT__
#include <pveclib/vec_f128_ppc.h>

vui32_t
test_and_bin128_2_vui32t (__binary128 f128, vui32_t mask)
{
  return vec_and_bin128_2_vui32t (f128, mask);
}

vui32_t
test_andc_bin128_2_vui32t (__binary128 f128, vui32_t mask)
{
  return vec_andc_bin128_2_vui32t (f128, mask);
}

vui32_t
test_xfer_bin128_2_vui32t (__binary128 f128)
{
  return vec_xfer_bin128_2_vui32t (f128);
}

vui128_t
test_xfer_bin128_2_vui128t (__binary128 f128)
{
  return vec_xfer_bin128_2_vui128t (f128);
}

__binary128
test_xfer_vui32t_2_bin128 (vui32_t f128)
{
  return vec_xfer_vui32t_2_bin128 (f128);
}

#ifndef PVECLIB_DISABLE_F128MATH
#ifdef __FLOAT128__
// TBD will sub-in pveclib softfloat for P8 when available

__float128
test_scalar_add128 (__float128 vra, __float128 vrb)
{
  if (__builtin_cpu_supports("ieee128"))
    {
      __float128 result;
      __asm__(
	      "xsaddqp %0,%1,%2"
	      : "=v" (result)
	      : "v" (vra), "v" (vrb)
	      : );
      return result;
    }
  else
    // Generate call to __addkf3
    return (vra + vrb);
}

__float128
test_scalar_div128 (__float128 vra, __float128 vrb)
{
  if (__builtin_cpu_supports("ieee128"))
    {
      __float128 result;
      __asm__(
	      "xsdivqp %0,%1,%2"
	      : "=v" (result)
	      : "v" (vra), "v" (vrb)
	      : );
      return result;
    }
  else
    // Generate call to __divkf3
    return (vra / vrb);
}

__float128
test_scalar_mul128 (__float128 vra, __float128 vrb)
{
  if (__builtin_cpu_supports("ieee128"))
    {
      __float128 result;
      __asm__(
	      "xsmulqp %0,%1,%2"
	      : "=v" (result)
	      : "v" (vra), "v" (vrb)
	      : );
      return result;
    }
  else
    // Generate call to __mulkf3
    return (vra * vrb);
}

__float128
test_scalar_sub128 (__float128 vra, __float128 vrb)
{
  if (__builtin_cpu_supports("ieee128"))
    {
      __float128 result;
      __asm__(
	      "xssubqp %0,%1,%2"
	      : "=v" (result)
	      : "v" (vra), "v" (vrb)
	      : );
      return result;
    }
  else
    // Generate call to __subkf3
    return (vra - vrb);
}

__float128
test_scalarCC_expxsuba_128 (__float128 x, __float128 a, __float128 expa)
{
  const __float128 f128_one = 1.0Q;
  const __float128 inv_fact2 = (1.0Q / 2.0Q);
  const __float128 inv_fact3 = (1.0Q / 6.0Q);
  const __float128 inv_fact4 = (1.0Q / 24.0Q);
  const __float128 inv_fact5 = (1.0Q / 120.0Q);
  const __float128 inv_fact6 = (1.0Q / 720.0Q);
  const __float128 inv_fact7 = (1.0Q / 5040.0Q);
  const __float128 inv_fact8 = (1.0Q / 40320.0Q);

  __float128 term, xma, xma2, xmaf2;
  __float128 xma3, xmaf3, xma4, xmaf4, xma5, xmaf5;
  __float128 xma6, xmaf6, xma7, xmaf7, xma8, xmaf8;

  // 1st 8 terms of e**x = e**a * taylor( x-a )
  xma = (x - a);
  term = (f128_one + xma);
  xma2 = (xma * xma);
  xmaf2 = (xma2 * inv_fact2);
  term = (term + xmaf2);
  xma3 = (xma2 * xma);
  xmaf3 = (xma3 * inv_fact3);
  term =  (term + xmaf3);
  xma4 = (xma3 * xma);
  xmaf4 = (xma4 * inv_fact4);
  term = (term + xmaf4);
  xma5 = (xma4 * xma);
  xmaf5 = (xma5 * inv_fact5);
  term = (term + xmaf5);
  xma6 = (xma5 * xma);
  xmaf6 = (xma6 * inv_fact6);
  term = (term + xmaf6);
  xma7 = (xma6 * xma);
  xmaf7 = (xma7 * inv_fact7);
  term = (term + xmaf7);
  xma8 = (xma7 * xma);
  xmaf8 = (xma8 * inv_fact8);
  term = (term + xmaf8);
  return (expa * term);
}
#endif
#endif

__binary128
test_vec_xsiexpqp (vui128_t sig, vui64_t exp)
{
  return vec_xsiexpqp (sig, exp);
}

vui64_t
test_vec_xsxexpqp (__binary128 f128)
{
  return vec_xsxexpqp (f128);
}

vui128_t
test_vec_xsxsigqp (__binary128 f128)
{
  return vec_xsxsigqp (f128);
}

vb128_t
__test_setb_qp (__binary128 f128)
{
  return vec_setb_qp (f128);
}

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

#ifndef PVECLIB_DISABLE_F128MATH
// Enable only if math.h supports generic C11 functions for __float128
// __clang__ has a bug whenever -mfloat128 is enabled, maybe clang 10.0.1
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
int
test_gcc_f128_signbit (__Float128 value)
  {
    return (signbit(value));
  }

int
test_gcc_f128_isinf (__Float128 value)
  {
    return (isinf(value));
  }

int
test_gcc_float128_isnan (__Float128 value)
  {
    return (isnan(value));
  }

__Float128
test_gcc_f128_copysign (__Float128 valx, __Float128 valy)
  {
    return (__builtin_copysignf128(valx, valy));
  }

int
test_glibc_f128_classify (__Float128 value)
  {
    if (isfinite(value))
    return 1;

    if (isinf(value))
    return 2;

    if (isnan(value))
    return 3;
    /* finite */
    return 0;
  }
#endif
#endif
#endif

