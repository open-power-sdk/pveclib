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
#ifndef PVECLIB_DISABLE_F128MATH
/* Disable for __clang__ because of bug involving <math.h>
   in combination with -mcpu=power9 -mfloat128 */
#include <math.h>
#endif

//#define __DEBUG_PRINT__
#include <pveclib/vec_f32_ppc.h>

#if (__GNUC__ > 7) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
vf32_t
test_vec_pack_dpsp (vf64_t a, vf64_t b)
{
  return vec_pack (a,b);
}
#endif

void
test_vec_vsst4fsso (vf32_t xs, float *array,
		   const long long offset0, const long long offset1,
		   const long long offset2, const long long offset3)
{
  vec_vsst4fsso (xs, array, offset0, offset1, offset2, offset3);
}

void
test_vec_vsstfswwo (vf32_t xs, float *array,
		   vi32_t vra)
{
  vec_vsst4fswo (xs, array, vra);
}

void
test_vec_vsst4fswsx (vf32_t xs, float *array,
		   vi32_t vra)
{
  vec_vsst4fswsx (xs, array, vra, 4);
}

void
test_vec_vsst4fswx (vf32_t xs, float *array,
		   vi32_t vra)
{
  vec_vsst4fswx (xs, array, vra);
}

vf32_t
test_vec_vgl4fswsx (float *array, vi32_t vra)
{
  return vec_vgl4fswsx (array, vra, 4);
}

vf32_t
test_vec_vgl4fswx (float *array, vi32_t vra)
{
  return vec_vgl4fswx (array, vra);
}

vf32_t
test_vec_vgl4fswo (float *array, vi32_t vra)
{
  return vec_vgl4fswo (array, vra);
}

vf32_t
test_vec_vgl4fsso (float *array, const long long offset0,
	     const long long offset1, const long long offset2,
	     const long long offset3)
{
  return vec_vgl4fsso (array, offset0, offset1, offset2, offset3);
}

void
test_vec_vsstfsdsx (vf64_t xs, float *array, vi64_t vra)
{
  vec_vsstfsdsx (xs, array, vra, 4);
}

void
test_vec_vsstfsdx (vf64_t xs, float *array, vi64_t vra)
{
  vec_vsstfsdx (xs, array, vra);
}

void
test_vec_vsstfsdo (vf64_t xs, float *array, vi64_t vra)
{
  vec_vsstfsdo (xs, array, vra);
}

void
test_vec_vsstfsso (vf64_t xs, float *array,
	      const long long offset0, const long long offset1)
{
  vec_vsstfsso (xs, array, offset0, offset1);
}

void
test_vec_vsstfsso_032 (vf64_t xs, float *array)
{
  vec_vsstfsso (xs, array, 0, 32);
}

vf64_t
test_vec_vglfsdsx (float *array, vi64_t vra)
{
  return vec_vglfsdsx (array, vra, 4);
}

vf64_t
test_vec_vglfsdx (float *array, vi64_t vra)
{
  return vec_vglfsdx (array, vra);
}

vf64_t
test_vec_vglfsdo (float *array, vi64_t vra)
{
  return vec_vglfsdo (array, vra);
}

vf64_t
test_vec_vglfsso (float *array, const long long offset0,
		     const long long offset1)
{
  return vec_vglfsso (array, offset0, offset1);
}

vf64_t
test_vec_vglfsso_032 (float *array)
{
  return vec_vglfsso (array, 0, 32);
}

vf64_t
test_vec_vlxsspx (const signed long long ra, const float *rb)
{
  return vec_vlxsspx (ra, rb);
}

vf64_t
test_vec_vlxsspx_c0 (const float *rb)
{
  return vec_vlxsspx (0, rb);
}

vf64_t
test_vec_vlxsspx_c1 (const float *rb)
{
  return vec_vlxsspx (8, rb);
}

vf64_t
test_vec_vlxsspx_c2 (const float *rb)
{
  return vec_vlxsspx (32760, rb);
}

vf64_t
test_vec_vlxsspx_c3 (const float *rb)
{
  return vec_vlxsspx (32768, rb);
}

vf64_t
test_vec_vlxsspx_c4 (const float *rb)
{
  return vec_vlxsspx (-32768, rb);
}

vf64_t
test_vlxsspx_v0 (const signed long long ra, const float *rb)
{
  vf64_t xt;
  __VEC_U_128 t;

  float *p = (float *)((char *)rb + ra);
  t.vf2[0] = t.vf2[1] = *p;
  xt = t.vf2;
  return xt;
}

void
test_vstxsspx (vf64_t data, float *array, signed long offset)
{
  vec_vstxsspx (data, offset, array);
}

void
test_vstxsspx_c0 (vf64_t data, float *array)
{
  vec_vstxsspx (data, 0, array);
}

void
test_vstxsspx_c1 (vf64_t data, float *array)
{
  vec_vstxsspx (data, 8, array);
}

void
test_vstxsspx_c2 (vf64_t data, float *array)
{
  vec_vstxsspx (data, 32760, array);
}

void
test_vstxsspx_c3 (vf64_t data, float *array)
{
  vec_vstxsspx (data, 32768, array);
}

void
test_vstxsspx_c4 (vf64_t data, float *array)
{
  vec_vstxsspx (data, -32768, array);
}

void
test_vstxsspx_v1 (vf64_t data, float *array, signed long offset)
{
  __VEC_U_128 t;
  float *p = (float *)((char *)array + offset);
  t.vf2 = data;
  *p = t.vf2[0];
}

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
vf32_t
test_vlxsspx_v1 (float *array, unsigned long offset)
{
  vf32_t res, rese;
  vui8_t resp;

  rese = vec_lvewx (offset, array);
  resp = vec_lvsr (offset, array);
  res = vec_perm (rese, rese, resp);
  return res;
}

vf32_t
test_vglssfso_v2 (float *array, unsigned long offset0, unsigned long offset1)
{
  vf32_t res, rese0, rese1;
  vui8_t resp0, resp1;

  rese0 = vec_lvewx (offset0, array);
  rese1 = vec_lvewx (offset1, array);
  resp0 = vec_lvsl (offset0, array);
  resp1 = vec_lvsl (offset1, array);
  rese0 = vec_perm (rese0, rese0, resp0);
  rese1 = vec_perm (rese1, rese1, resp1);
  res = vec_mergeh (rese0, rese1);
  return res;
}
#endif

vf32_t
test_veclfgux_v1 (float *array, vui32_t idx)
{
  vf32_t res;
  res[0] = array[idx[0]];
  res[1] = array[idx[1]];
  res[2] = array[idx[2]];
  res[3] = array[idx[3]];
  return res;
}

vf32_t
test_veclfgux_v2 (float *array, vui32_t idx)
{
  __VEC_U_128 vidx, vflt;
  vidx.vx4 = idx;
  vflt.vf4[0] = array[vidx.uint.ix0];
  vflt.vf4[1] = array[vidx.uint.ix1];
  vflt.vf4[2] = array[vidx.uint.ix2];
  vflt.vf4[3] = array[vidx.uint.ix3];
  return vflt.vf4;
}

#if defined __GNUC__ && (__GNUC__ > 7) && defined(_ARCH_PWR8)
vf32_t
test_veclfgux_v3 (float *array, vui32_t idx)
{
  __VEC_U_128 vidx0, vidx1, vflt0, vflt1;
  vui64_t idxh, idxl;
  const vui32_t vzero = { 0, 0, 0, 0 };
  const vui64_t vsl2 = { 2, 2 };
  vf32_t res0, res1, res;
  idxh = (vui64_t) vec_mergeh (vzero, idx);
  idxl = (vui64_t) vec_mergel (vzero, idx);
  idxh = vec_sl (idxh, vsl2);
  idxl = vec_sl (idxl, vsl2);

  vidx0.vx2 = idxh;
  vidx1.vx2 = idxl;
  vflt0.vf2[0] = array[vidx0.ulong.lower];
  vflt0.vf2[1] = array[vidx0.ulong.upper];
  vflt1.vf2[0] = array[vidx1.ulong.lower];
  vflt1.vf2[1] = array[vidx1.ulong.upper];
  res0 = vec_floate (vflt0.vf2);
  res1 = vec_floato (vflt1.vf2);
  res  = vec_mergee (res0, res1);
  return res;
}
#endif

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

#ifndef PVECLIB_DISABLE_F128MATH
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

void
test_fpclassify_f32loop (vui32_t *out, vf32_t *in, int count)
{
  int i;

  for (i=0; i<count; i++)
    {
      out[i] = test_fpclassify_f32 (in[i]);
    }
}
#endif

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

vf32_t
test_vec_sinf32 (vf32_t value)
{
  const vf32_t vec_f0 =
    { 0.0, 0.0, 0.0, 0.0 };
  const vui32_t vec_f32_qnan =
    { 0x7f800001, 0x7fc00000, 0x7fc00000, 0x7fc00000 };
  vf32_t result;
  vb32_t normmask, infmask;

  normmask = vec_isnormalf32 (value);
  if (vec_any_isnormalf32 (value))
    {
      /* replace non-normal input values with safe values.  */
      vf32_t safeval = vec_sel (vec_f0, value, normmask);
      /* body of vec_sin(safeval) computation elided for this example.  */
      result = vec_mul (safeval, safeval);
    }
  else
    result = value;

  /* merge non-normal input values back into result */
  result = vec_sel (value, result, normmask);
  /* Inf input value elements return quiet-nan.  */
  infmask = vec_isinff32 (value);
  result = vec_sel (result, (vf32_t) vec_f32_qnan, infmask);

  return result;
}

/* dummy cosf32 example. From Posix:
 * If value is NaN then return a NaN.
 * If value is +-0.0 then return 1.0.
 * If value is +-Inf then return a NaN.
 * Otherwise compute and return sin(value).
 */
vf32_t
test_vec_cosf32 (vf32_t value)
{
  vf32_t result;
  const vf32_t vec_f0 =
    { 0.0, 0.0, 0.0, 0.0 };
  const vf32_t vec_f1 =
    { 1.0, 1.0, 1.0, 1.0 };
  const vui32_t vec_f32_qnan =
    { 0x7f800001, 0x7fc00000, 0x7fc00000, 0x7fc00000 };
  vb32_t finitemask, infmask, zeromask;

  finitemask = vec_isfinitef32 (value);
  if (vec_any_isfinitef32 (value))
    {
      /* replace non-finite input values with safe values.  */
      vf32_t safeval = vec_sel (vec_f0, value, finitemask);
      /* body of vec_sin(safeval) computation elided for this example.  */
      result = vec_mul (safeval, safeval);
    }
  else
    result = value;

  /* merge non-finite input values back into result */
  result = vec_sel (value, result, finitemask);
  /* Set +-0.0 input elements to exactly 1.0 in result.  */
  zeromask = vec_iszerof32 (value);
  result = vec_sel (result, vec_f1, zeromask);
  /* Set Inf input elements to quiet-nan in result.  */
  infmask = vec_isinff32 (value);
  result = vec_sel (result, (vf32_t) vec_f32_qnan, infmask);

  return result;
}

int
test_all_f32_finite (vf32_t value)
{
  return (vec_all_isfinitef32 (value));
}

int
test_any_f32_finite (vf32_t value)
{
  return (vec_any_isfinitef32 (value));
}

vb32_t
test_pred_f32_finite (vf32_t value)
{
  return (vec_isfinitef32 (value));
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

vb32_t
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
#if defined vec_float2  && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
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
