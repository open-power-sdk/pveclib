/*
 Copyright (c) [2017-18] IBM Corporation.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 vec_int32_dummy.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Jul 31, 2017
 */

#include <stdint.h>
#include <stdio.h>
#include <fenv.h>
#include <float.h>
#include <math.h>

//#define __DEBUG_PRINT__
#include "vec_f32_ppc.h"

vf32_t
test_vec_f32_abs (vf32_t value)
{
  return (vec_absf32 (value));
}

vf32_t
test_f32_zero_demorm (vf32_t value)
{
  vf32_t result = value;
  vui32_t mask;
  vf32_t fzero = {0.0, 0.0, 0.0, 0.0};

  mask = (vui32_t)vec_issubnormalf32 (value);
  if (vec_any_issubnormalf32 (value))
    {
      result = vec_sel (value, fzero, mask);
    }

  return result;
}

vui32_t
test_fpclassify_f32 (vf32_t value)
{
  const vui32_t VFP_NAN =
    { FP_NAN, FP_NAN, FP_NAN, FP_NAN };
  const vui32_t VFP_INFINITE =
    { FP_INFINITE, FP_INFINITE,
    FP_INFINITE, FP_INFINITE };
  const vui32_t VFP_ZERO =
    { FP_ZERO, FP_ZERO, FP_ZERO, FP_ZERO };
  const vui32_t VFP_SUBNORMAL =
    { FP_SUBNORMAL, FP_SUBNORMAL,
    FP_SUBNORMAL, FP_SUBNORMAL };
  const vui32_t VFP_NORMAL =
    { FP_NORMAL, FP_NORMAL, FP_NORMAL,
    FP_NORMAL };
  /* FP_NAN should be 0.  */
  vui32_t result = VFP_NAN;
  vui32_t mask;

  mask = (vui32_t)vec_isinff32 (value);
  result = vec_sel (result, VFP_INFINITE, mask);
  mask = (vui32_t)vec_iszerof32 (value);
  result = vec_sel (result, VFP_ZERO, mask);
  mask = (vui32_t)vec_issubnormalf32 (value);
  result = vec_sel (result, VFP_SUBNORMAL, mask);
  mask = (vui32_t)vec_isnormalf32 (value);
  result = vec_sel (result, VFP_NORMAL, mask);

  return result;
}

int
test512_any_f32_subnorm (vf32_t val0, vf32_t val1, vf32_t val2, vf32_t val3)
{
  const vb32_t alltrue = {-1,-1,-1,-1};
  vb32_t sub0, sub1, sub2, sub3;

  sub0 = vec_issubnormalf32(val0);
  sub1 = vec_issubnormalf32(val1);
  sub2 = vec_issubnormalf32(val2);
  sub3 = vec_issubnormalf32(val3);

  sub0 = vec_or (sub0, sub1);
  sub2 = vec_or (sub2, sub3);
  sub0 = vec_or (sub2, sub0);

  return vec_any_eq (sub0, alltrue);
}

int
test512_any_f32_subnorm2 (vf32_t val0, vf32_t val1, vf32_t val2, vf32_t val3)
{
  return vec_any_issubnormalf32 (val0)
      || vec_any_issubnormalf32 (val1)
      || vec_any_issubnormalf32 (val2)
      || vec_any_issubnormalf32 (val3);
}

int
test512_all_f32_nan (vf32_t val0, vf32_t val1, vf32_t val2, vf32_t val3)
{
  const vb32_t alltrue = { -1, -1, -1, -1 };
  vb32_t nan0, nan1, nan2, nan3;

  nan0 = vec_isnanf32 (val0);
  nan1 = vec_isnanf32 (val1);
  nan2 = vec_isnanf32 (val2);
  nan3 = vec_isnanf32 (val3);

  nan0 = vec_and (nan0, nan1);
  nan2 = vec_and (nan2, nan3);
  nan0 = vec_and (nan2, nan0);

  return vec_all_eq(nan0, alltrue);
}

int
test_all_f32_inf (vf32_t value)
{
  return (vec_all_isinff32 (value));
}

int
test_any_f32_inf (vf32_t value)
{
	return (vec_any_isinff32 (value));
}

__vector bool int
test_pred_f32_inf (vf32_t value)
{
	return (vec_isinff32 (value));
}

int
test_all_f32_nan (vf32_t value)
{
	return (vec_all_isnanf32 (value));
}

int
test_any_f32_nan (vf32_t value)
{
	return (vec_any_isnanf32 (value));
}

__vector bool int
test_pred_f32_nan (vf32_t value)
{
	return (vec_isnanf32 (value));
}

int
test_all_f32_norm (vf32_t value)
{
	return (vec_all_isnormalf32 (value));
}

int
test_any_f32_norm (vf32_t value)
{
	return (vec_any_isnormalf32 (value));
}

__vector bool int
test_pred_f32_normal (vf32_t value)
{
	return (vec_isnormalf32 (value));
}

int
test_all_f32_subnorm (vf32_t value)
{
	return (vec_all_issubnormalf32 (value));
}

int
test_any_f32_subnorm (vf32_t value)
{
	return (vec_any_issubnormalf32 (value));
}

__vector bool int
test_pred_f32_subnormal (vf32_t value)
{
	return (vec_issubnormalf32 (value));
}

int
test_all_f32_zero (vf32_t value)
{
	return (vec_all_iszerof32 (value));
}

int
test_any_f32_zero (vf32_t value)
{
	return (vec_any_iszerof32 (value));
}

__vector bool int
test_pred_f32_zero (vf32_t value)
{
	return (vec_iszerof32 (value));
}
/* Compiler generation tests.  */
#ifdef vec_float
vf32_t
test_vec_float_int (vi32_t __A)
  {
    return vec_float (__A);
  }

vf32_t
test_vec_float_uint (vui32_t __A)
  {
    return vec_float (__A);
  }
#endif
#ifdef vec_float2
vf32_t
test_vec_float2_long (vi64_t __A, vi64_t __B)
  {
    return vec_float2 (__A, __B);
  }

vf32_t
test_vec_float2_ulong (vui64_t __A, vui64_t __B)
  {
    return vec_float2 (__A, __B);
  }

vf32_t
test_vec_float2_double (vf64_t __A, vf64_t __B)
  {
    return vec_float2 (__A, __B);
  }
#endif
#ifdef vec_floate
vf32_t
test_vec_floate_long (vi64_t __A)
  {
    return vec_floate (__A);
  }

vf32_t
test_vec_floate_ulong (vui64_t __A)
  {
    return vec_floate (__A);
  }

vf32_t
test_vec_floate_double (vf64_t __A)
  {
    return vec_floate (__A);
  }
#endif
#ifdef vec_floato
vf32_t
test_vec_floato_long (vi64_t __A)
  {
    return vec_floato (__A);
  }

vf32_t
test_vec_floato_ulong (vui64_t __A)
  {
    return vec_floato (__A);
  }

vf32_t
test_vec_floato_double (vf64_t __A)
  {
    return vec_floato (__A);
  }
#endif
