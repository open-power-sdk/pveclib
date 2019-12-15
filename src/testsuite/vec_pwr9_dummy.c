/*
 Copyright (c) [2017] IBM Corporation.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 vec_pwr9_dummy.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: April 27, 2018
 */

//#pragma GCC push target
#pragma GCC target ("cpu=power9")
#include <stdint.h>
#include <stdio.h>
#include <fenv.h>
#include <float.h>
#include <math.h>

#include <pveclib/vec_int128_ppc.h>
#include <pveclib/vec_int512_ppc.h>
#include <pveclib/vec_f128_ppc.h>
#include <pveclib/vec_f32_ppc.h>
#include <pveclib/vec_bcd_ppc.h>

vui8_t
__test_absdub_PWR9 (vui8_t __A, vui8_t __B)
{
  return vec_absdub (__A, __B);
}

vui32_t
__test_absduw_PWR9 (vui32_t a, vui32_t b)
{
  return vec_absduw (a, b);
}

vui128_t
__test_msumudm_PWR9 (vui64_t a, vui64_t b, vui128_t c)
{
  return vec_msumudm ( a, b, c);
}

vui128_t
__test_muloud_PWR9 (vui64_t a, vui64_t b)
{
  return vec_muloud (a, b);
}

vui128_t
__test_muleud_PWR9 (vui64_t a, vui64_t b)
{
  return vec_muleud (a, b);
}

vui64_t
__test_mulhud_PWR9 (vui64_t a, vui64_t b)
{
  return vec_mulhud (a, b);
}

vui64_t
__test_muludm_PWR9 (vui64_t a, vui64_t b)
{
  return vec_muludm (a, b);
}

vui128_t
__test_vmuloud_PWR9 (vui64_t a, vui64_t b)
{
  return vec_vmuloud (a, b);
}

vui128_t
__test_vmuleud_PWR9 (vui64_t a, vui64_t b)
{
  return vec_vmuleud (a, b);
}

vui128_t
__test_mulhuq_PWR9 (vui128_t a, vui128_t b)
{
  return vec_mulhuq (a, b);
}

vui128_t
__test_mulluq_PWR9 (vui128_t a, vui128_t b)
{
  return vec_mulluq (a, b);
}

vui128_t
__test_muludq_PWR9 (vui128_t *mulh, vui128_t a, vui128_t b)
{
  return vec_muludq (mulh, a, b);
}

vui128_t
__test_mulhluq_PWR9 (vui128_t *mulh, vui128_t a, vui128_t b)
{
  *mulh = vec_mulhuq (a, b);
  return vec_mulluq (a, b);
}

vui128_t
__test_mulhuq2_PWR9 (vui128_t a, vui128_t b)
{
  vui128_t mq, r;
  r = vec_muludq (&mq, a, b);
  return mq;
}

vui128_t
test_vec_cmul10cuq_256_PWR9 (vui128_t *p, vui128_t a, vui128_t a2, vui128_t cin)
{
  vui128_t k, j;
  k = vec_cmul10ecuq (&j, a, cin);
  *p = vec_mul10euq ((vui128_t) a2, j);
  return k;
}

vui128_t
__test_cmul10cuq_PWR9 (vui128_t *cout, vui128_t a)
{
  return vec_cmul10cuq (cout, a);
}

vui128_t
__test_cmul10ecuq_PWR9 (vui128_t *cout, vui128_t a, vui128_t cin)
{
  return vec_cmul10ecuq (cout, a, cin);
}

vui128_t
__test_cmul100cuq_PWR9 (vui128_t *cout, vui128_t a)
{
  return vec_cmul100cuq (cout, a);
}

vui128_t
__test_cmul100ecuq_PWR9 (vui128_t *cout, vui128_t a, vui128_t cin)
{
  return vec_cmul100ecuq (cout, a, cin);
}

vui128_t
__test_mul10uq_c_PWR9 (vui128_t *p, vui128_t a)
{
  *p = vec_mul10uq (a);
  return vec_mul10uq (a);
}

vui128_t
__test_mul10uq_PWR9 (vui128_t a)
{
  return vec_mul10uq (a);
}

vui128_t
__test_mul10euq_PWR9 (vui128_t a, vui128_t cin)
{
  return vec_mul10euq (a, cin);
}

vui128_t
__test_mul10cuq_PWR9 (vui128_t a)
{
  return vec_mul10cuq (a);
}

vui128_t
__test_mul10ecuq_PWR9 (vui128_t a, vui128_t cin)
{
  return vec_mul10ecuq (a, cin);
}

vui64_t
__test_revbd_PWR9 (vui64_t a)
{
  return vec_revbd (a);
}

vui16_t
__test_revbh_PWR9 (vui16_t a)
{
  return vec_revbh (a);
}

vui128_t
__test_revbq_PWR9 (vui128_t a)
{
  return vec_revbq (a);
}

vui32_t
__test_revbw_PWR9 (vui32_t a)
{
  return vec_revbw (a);
}

int
test_vec_all_finitef32_PWR9 (vf32_t value)
{
  return (vec_all_isfinitef32 (value));
}

int
test_vec_all_inff32_PWR9 (vf32_t value)
{
  return (vec_all_isinff32 (value));
}

int
test_vec_all_nanf32_PWR9 (vf32_t value)
{
  return (vec_all_isnanf32 (value));
}

int
test_vec_all_normalf32_PWR9 (vf32_t value)
{
  return (vec_all_isnormalf32 (value));
}

int
test_vec_all_subnormalf32_PWR9 (vf32_t value)
{
  return (vec_all_issubnormalf32 (value));
}

int
test_vec_all_zerof32_PWR9 (vf32_t value)
{
  return (vec_all_iszerof32 (value));
}
int
test_vec_any_finitef32_PWR9 (vf32_t value)
{
  return (vec_any_isfinitef32 (value));
}

int
test_vec_any_inff32_PWR9 (vf32_t value)
{
  return (vec_any_isinff32 (value));
}

int
test_vec_any_nanf32_PWR9 (vf32_t value)
{
  return (vec_any_isnanf32 (value));
}

int
test_vec_any_normalf32_PWR9 (vf32_t value)
{
  return (vec_any_isnormalf32 (value));
}

int
test_vec_any_subnormalf32_PWR9 (vf32_t value)
{
  return (vec_any_issubnormalf32 (value));
}

int
test_vec_any_zerof32_PWR9 (vf32_t value)
{
  return (vec_any_iszerof32 (value));
}

vb32_t
test_vec_isfinitef32_PWR9 (vf32_t value)
{
  return (vec_isfinitef32 (value));
}

vb32_t
test_vec_isinff32_PWR9 (vf32_t value)
{
  return (vec_isinff32 (value));
}

vb32_t
test_vec_isnanf32_PWR9 (vf32_t value)
{
  return (vec_isnanf32 (value));
}

vb32_t
test_vec_isnormalf32_PWR9 (vf32_t value)
{
  return (vec_isnormalf32 (value));
}

vb32_t
test_vec_issubnormalf32_PWR9 (vf32_t value)
{
  return (vec_issubnormalf32 (value));
}

vb32_t
test_vec_iszerof32_PWR9 (vf32_t value)
{
  return (vec_iszerof32 (value));
}

#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
int
test_vec_all_finitef64_PWR9 (vf64_t value)
{
  return (vec_all_isfinitef64 (value));
}

int
test_vec_all_inff64_PWR9 (vf64_t value)
{
  return (vec_all_isinff64 (value));
}

int
test_vec_all_nanf64_PWR9 (vf64_t value)
{
  return (vec_all_isnanf64 (value));
}

int
test_vec_all_normalf64_PWR9 (vf64_t value)
{
  return (vec_all_isnormalf64 (value));
}

int
test_vec_all_subnormalf64_PWR9 (vf64_t value)
{
  return (vec_all_issubnormalf64 (value));
}

int
test_vec_all_zerof64_PWR9 (vf64_t value)
{
  return (vec_all_iszerof64 (value));
}
int
test_vec_any_finitef64_PWR9 (vf64_t value)
{
  return (vec_any_isfinitef64 (value));
}

int
test_vec_any_inff64_PWR9 (vf64_t value)
{
  return (vec_any_isinff64 (value));
}

int
test_vec_any_nanf64_PWR9 (vf64_t value)
{
  return (vec_any_isnanf64 (value));
}

int
test_vec_any_normalf64_PWR9 (vf64_t value)
{
  return (vec_any_isnormalf64 (value));
}

int
test_vec_any_subnormalf64_PWR9 (vf64_t value)
{
  return (vec_any_issubnormalf64 (value));
}

int
test_vec_any_zerof64_PWR9 (vf64_t value)
{
  return (vec_any_iszerof64 (value));
}

vb64_t
test_vec_isfinitef64_PWR9 (vf64_t value)
{
  return (vec_isfinitef64 (value));
}

vb64_t
test_vec_isinff64_PWR9 (vf64_t value)
{
  return (vec_isinff64 (value));
}

vb64_t
test_vec_isnanf64_PWR9 (vf64_t value)
{
  return (vec_isnanf64 (value));
}

vb64_t
test_vec_isnormalf64_PWR9 (vf64_t value)
{
  return (vec_isnormalf64 (value));
}

vb64_t
test_vec_issubnormalf64_PWR9 (vf64_t value)
{
  return (vec_issubnormalf64 (value));
}

vb64_t
test_vec_iszerof64_PWR9 (vf64_t value)
{
  return (vec_iszerof64 (value));
}
#endif

vb128_t
test_vec_isfinitef128_PWR9 (__binary128 f128)
{
  return vec_isfinitef128 (f128);
}

vb128_t
test_vec_isinff128_PWR9 (__binary128 f128)
{
  return vec_isinff128 (f128);
}

vb128_t
test_vec_isnanf128_PWR9 (__binary128 f128)
{
  return vec_isnanf128 (f128);
}

vb128_t
test_vec_isnormalf128_PWR9 (__binary128 f128)
{
  return vec_isnormalf128 (f128);
}

vb128_t
test_vec_issubnormalf128_PWR9 (__binary128 f128)
{
  return vec_issubnormalf128 (f128);
}

vb128_t
test_vec_iszerof128_PWR9 (__binary128 f128)
{
  return vec_iszerof128 (f128);
}

int
test_vec_all_normalf128_PWR9 (__binary128 value)
{
  return (vec_all_isnormalf128 (value));
}

int
test_vec_all_finitef128_PWR9 (__binary128 value)
{
  return (vec_all_isfinitef128 (value));
}

int
test_vec_all_subnormalf128_PWR9 (__binary128 value)
{
  return (vec_all_issubnormalf128 (value));
}

int
test_vec_all_inff128_PWR9 (__binary128 value)
{
  return (vec_all_isinff128 (value));
}

int
test_vec_isinf_signf128_PWR9 (__binary128 value)
{
  return (vec_isinf_signf128 (value));
}

int
test_vec_all_nanf128_PWR9 (__binary128 value)
{
  return (vec_all_isnanf128 (value));
}

int
test_vec_all_zerof128_PWR9 (__binary128 value)
{
  return (vec_all_iszerof128 (value));
}

vf32_t
test_f32_zero_demorm_PWR9 (vf32_t value)
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
test_fpclassify_f32_PWR9 (vf32_t value)
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
test_fpclassify_f32loop_PWR9 (vui32_t *out, vf32_t *in, int count)
{
  int i;

  for (i=0; i<count; i++)
    {
      out[i] = test_fpclassify_f32_PWR9 (in[i]);
    }
}

/* dummy sinf32 example. From Posix:
 * If value is NaN then return a NaN.
 * If value is +-0.0 then return value.
 * If value is subnormal then return value.
 * If value is +-Inf then return a NaN.
 * Otherwise compute and return sin(value).
 */
vf32_t
test_vec_sinf32_PWR9 (vf32_t value)
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
test_vec_cosf32_PWR9 (vf32_t value)
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

#ifdef __FLOAT128_TYPE__
/* dummy sinf128 example. From Posix:
 * If value is NaN then return a NaN.
 * If value is +-0.0 then return value.
 * If value is subnormal then return value.
 * If value is +-Inf then return a NaN.
 * Otherwise compute and return sin(value).
 */
__binary128
test_sinf128_PWR9 (__binary128 value)
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
test_cosf128_PWR9 (__binary128 value)
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

#ifdef vec_cmpne
vb64_t
__test_cmpnedp_PWR9 (vf64_t a, vf64_t b)
{
  return vec_cmpne (a, b);
}

vb64_t
__test_cmpneud_PWR9 (vui64_t a, vui64_t b)
{
  return vec_cmpne (a, b);
}
#endif

#ifdef scalar_test_data_class
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
int
__test_scalar_test_data_class_f128 (__binary128 val)
{
  return scalar_test_data_class (val, 0x7f);
}
#endif
int
__test_scalar_test_data_class_f64 (double val)
{
  return scalar_test_data_class (val, 0x7f);
}

int
__test_scalar_test_data_class_f32 (float val)
{
  return scalar_test_data_class (val, 0x7f);
}
#endif

#ifdef scalar_test_neg
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
int
__test_scalar_test_neg (__ieee128 val)
{
  return scalar_test_neg (val);
}
#endif
#endif

#ifdef scalar_extract_exp
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
long long int
__test_scalar_extract_exp_f128 (__ieee128 val)
{
  return scalar_extract_exp (val);
}
#endif
int
__test_scalar_extract_exp_f64 (double val)
{
  return scalar_extract_exp (val);
}
#endif

#ifdef scalar_extract_sig
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
__int128
__test_scalar_extract_sig_f128 (__ieee128 val)
{
  return scalar_extract_sig (val);
}
#endif
long long int
__test_scalar_extract_sig_f64 (double val)
{
  return scalar_extract_sig (val);
}
#endif

#ifdef scalar_insert_exp
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
__ieee128
__test_scalar_insert_exp_f128 (__ieee128 sig, unsigned long long int exp)
{
  return scalar_insert_exp (sig, exp);
}
#endif
double
__test_scalar_insert_exp_f64 (double sig, unsigned long long int exp)
{
  return scalar_insert_exp (sig, exp);
}
#endif

#ifdef scalar_cmp_exp_eq
#if 0
/* there is an instruction for this, but is not supported in
   GCC (8.2) yet.  */
int
__test_scalar_cmp_exp_eq_f128 (__ieee128 vra, __ieee128 vrb)
{
  return scalar_cmp_exp_eq (vra, vrb);
}
#endif
int
__test_scalar_cmp_exp_eq_f64 (double vra, double vrb)
{
  return scalar_cmp_exp_eq (vra, vrb);
}
#endif

#ifdef vec_insert_exp
vf64_t
__test_vec_insert_exp_f64b (vui64_t sig, vui64_t exp)
{
  return vec_insert_exp (sig, exp);
}
vf64_t
__test_vec_insert_exp_f64 (vf64_t sig, vui64_t exp)
{
  return vec_insert_exp (sig, exp);
}

vf32_t
__test_vec_insert_exp_f32b (vui32_t sig, vui32_t exp)
{
  return vec_insert_exp (sig, exp);
}
vf32_t
__test_vec_insert_exp_f32 (vf32_t sig, vui32_t exp)
{
  return vec_insert_exp (sig, exp);
}
#endif

#ifdef vec_test_data_class
vb64_t
__test_vec_test_data_class_f64 (vf64_t val)
{
  return vec_test_data_class (val, 0x7f);
}

vb32_t
__test_vec_test_data_class_f32 (vf32_t val)
{
  return vec_test_data_class (val, 0x7f);
}
#endif

#ifdef vec_extract_exp
vui64_t
__test_vec_extract_exp_f64 (vf64_t val)
{
  return vec_extract_exp (val);
}
vui32_t
__test_vec_extract_exp_f32 (vf32_t val)
{
  return vec_extract_exp (val);
}
#endif

#ifdef vec_extract_sig
vui64_t
__test_vec_extract_sig_f64 (vf64_t val)
{
  return vec_extract_sig (val);
}
vui32_t
__test_vec_extract_sig_f32 (vf32_t val)
{
  return vec_extract_sig (val);
}
#endif

vBCD_t
test_vec_bcdmulh_PWR9 (vBCD_t a, vBCD_t b)
{
  return vec_bcdmulh (a, b);
}

vBCD_t
test_vec_bcdmul_PWR9 (vBCD_t a, vBCD_t b)
{
  return vec_bcdmul (a, b);
}

vBCD_t
test_vec_cbcdmul_PWR9 (vBCD_t *p, vBCD_t a, vBCD_t b)
{
  return vec_cbcdmul (p, a, b);
}

vi128_t
test_vec_bcdctsq_PWR9 (vBCD_t a)
{
  return (vec_bcdctsq (a));
}

vBCD_t
test_vec_bcdcpsgn_PWR9 (vBCD_t a, vBCD_t b)
{
  return vec_bcdcpsgn (a, b);
}

#if 1
vBCD_t
test_vec_bcds_PWR9 (vBCD_t vra, vi8_t vrb)
{
  return vec_bcds (vra, vrb);
}

vBCD_t
test_vec_bcdsr_PWR9 (vBCD_t vra, vi8_t vrb)
{
  return vec_bcdsr (vra, vrb);
}

vBCD_t
test_vec_bcdsrrqi_PWR9 (vBCD_t vra)
{
  return vec_bcdsrrqi (vra, 15);
}

vBCD_t
test_vec_bcdtrunc_PWR9 (vBCD_t vra, vui16_t vrb)
{
  return vec_bcdtrunc (vra, vrb);
}

vBCD_t
test_vec_bcdtruncqi_PWR9 (vBCD_t vra)
{
  return vec_bcdtruncqi (vra, 15);
}

vBCD_t
test_vec_bcdutrunc_PWR9 (vBCD_t vra, vui16_t vrb)
{
  return vec_bcdutrunc (vra, vrb);
}

vBCD_t
test_vec_bcdutruncqi_PWR9 (vBCD_t vra)
{
  return vec_bcdutruncqi (vra, 15);
}

vBCD_t
test_vec_bcdus_PWR9 (vBCD_t vra, vi8_t vrb)
{
  return vec_bcdus (vra, vrb);
}

vBCD_t
test_vec_bcdsrqi_1_PWR9 (vBCD_t vra)
{
  return vec_bcdsrqi (vra, 1);
}

vBCD_t
test_vec_bcdsrqi_31_PWR9 (vBCD_t vra)
{
  return vec_bcdsrqi (vra, 31);
}

vBCD_t
test_vec_bcdsruqi_1_PWR9 (vBCD_t vra)
{
  return vec_bcdsruqi (vra, 1);
}

vBCD_t
test_vec_bcdsruqi_31_PWR9 (vBCD_t vra)
{
  return vec_bcdsruqi (vra, 31);
}

vBCD_t
test_vec_bcdslqi_1_PWR9 (vBCD_t vra)
{
  return vec_bcdslqi (vra, 1);
}

vBCD_t
test_vec_bcdslqi_31_PWR9 (vBCD_t vra)
{
  return vec_bcdslqi (vra, 31);
}

vBCD_t
test_vec_bcdsluqi_1_PWR9 (vBCD_t vra)
{
  return vec_bcdsluqi (vra, 1);
}

vBCD_t
test_vec_bcdsluqi_31_PWR9 (vBCD_t vra)
{
  return vec_bcdsluqi (vra, 31);
}
#endif


vBCD_t
test_vec_cbcdaddcsq_PWR9 (vBCD_t *c, vBCD_t a, vBCD_t b)
{
  return vec_cbcdaddcsq (c, a, b);
}

vBCD_t
test_vec_cbcdaddcsq2_PWR9 (vBCD_t *c, vBCD_t a, vBCD_t b)
{
  *c = vec_bcdaddcsq (a, b);
  return vec_bcdadd (a, b);
}

vBCD_t
test_vec_cbcdaddecsq_PWR9 (vBCD_t *co, vBCD_t a, vBCD_t b, vBCD_t c)
{
  return vec_cbcdaddecsq (co, a, b, c);
}

vBCD_t
test_vec_cbcdaddecsq2_PWR9 (vBCD_t *co, vBCD_t a, vBCD_t b, vBCD_t c)
{
  *co = vec_bcdaddecsq (a, b, c);
  return vec_bcdaddesqm (a, b, c);
}

vui128_t
test_vec_bcdctuq_PWR9 (vBCD_t vra)
{
  return vec_bcdctuq (vra);
}

vui128_t
example_vec_bcdctuq_PWR9 (vui8_t vra)
{
  vui8_t d100;
  vui16_t d10k;
  vui32_t d100m;
  vui64_t d10e;
  vui128_t result;

  d100 = vec_rdxct100b ((vui8_t) vra);
  d10k = vec_rdxct10kh (d100);
  d100m = vec_rdxct100mw (d10k);
  d10e = vec_rdxct10E16d (d100m);
  return vec_rdxct10e32q (d10e);

  return result;
}

void
example_vec_cbcdecsq_loop_PWR9 (vBCD_t *cout, vBCD_t* out, vBCD_t* a, vBCD_t* b,
				long cnt)
{
  vBCD_t c, cn;
  long i;

  out[cnt - 1] = vec_cbcdaddcsq (&c, a[cnt - 1], b[cnt - 1]);

  cn = _BCD_CONST_ZERO;
  for (i = (cnt - 2); i >= 0; i--)
    {
      out[i] = vec_cbcdaddecsq (&cn, a[i], b[i], c);
      c = cn;
    }
  *cout = cn;
}

vBCD_t
test_vec_bcdaddcsq_PWR9 (vBCD_t a, vBCD_t b)
{
  return vec_bcdaddcsq (a, b);
}

vBCD_t
test_vec_bcdaddesqm_PWR9 (vBCD_t a, vBCD_t b, vBCD_t c)
{
  return vec_bcdaddesqm (a, b, c);
}

vBCD_t
test_vec_bcdaddecsq_PWR9 (vBCD_t a, vBCD_t b, vBCD_t c)
{
  return vec_bcdaddecsq (a, b, c);
}

vBCD_t
test__vec_bcdaddcsq_PWR9 (vBCD_t a, vBCD_t b)
{
  return vec_bcdaddcsq (a, b);
}

vi128_t
test__vec_bcdaddcsq2_PWR9 (vi128_t a, vi128_t b)
{
  vi128_t t;
  t = (vi128_t) _BCD_CONST_ZERO;
  if (__builtin_expect (__builtin_bcdadd_ov ( a, b, 0), 0))
    {
      t = __builtin_bcdadd (a, b, 0);
      t = (vi128_t) vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, (vBCD_t) t);
    }
  return (t);
}

vui64_t
test_vec_rdxct10E16d_PWR9 (vui32_t vra)
{
  return vec_rdxct10E16d (vra);
}

vui128_t
test_vec_rdxct10e32q_PWR9 (vui64_t vra)
{
  return vec_rdxct10e32q (vra);
}

vui32_t
test_vec_rdxcf10E16d_PWR9 (vui64_t vra)
{
  return vec_rdxcf10E16d (vra);
}

vui64_t
test_vec_rdxcf10e32q_PWR9 (vui128_t vra)
{
  return vec_rdxcf10e32q (vra);
}

vBCD_t
test_vec_bcdcfsq_PWR9 (vi128_t vra)
{
  return vec_bcdcfsq (vra);
}

vBCD_t
test_vec_bcdcfuq_PWR9 (vui128_t vra)
{
  return vec_bcdcfuq (vra);
}

vi128_t
__test_remsq_10e31_PWR9  (vi128_t a, vi128_t b)
{
  vi128_t q;
  q = vec_divsq_10e31 (a);
  return vec_modsq_10e31 (a, q);
}

vui128_t
__test_remuq_10e31_PWR9  (vui128_t a, vui128_t b)
{
  vui128_t q;
  q = vec_divuq_10e31 (a);
  return vec_moduq_10e31 (a, q);
}

vui128_t
__test_remuq_10e32_PWR9  (vui128_t a, vui128_t b)
{
  vui128_t q;
  q = vec_divuq_10e32 (a);
  return vec_moduq_10e32 (a, q);
}

vui128_t
__test_divudq_10e31_PWR9  (vui128_t *qh, vui128_t a, vui128_t b)
{
  return vec_divudq_10e31 (qh, a, b);
}

vui128_t
__test_divudq_10e32_PWR9  (vui128_t *qh, vui128_t a, vui128_t b)
{
  return vec_divudq_10e32 (qh, a, b);
}

vui128_t
__test_remudq_10e31_PWR9  (vui128_t a, vui128_t b)
{
  vui128_t q, qh;
  q = vec_divudq_10e31 (&qh, a, b);
  return vec_modudq_10e31 (a, b, &q);
}

vui128_t
__test_remudq_10e32_PWR9  (vui128_t a, vui128_t b)
{
  vui128_t q, qh;
  q = vec_divudq_10e32 (&qh, a, b);
  return vec_modudq_10e32 (a, b, &q);
}

void
example_qw_convert_decimal_PWR9 (vui64_t *ten_16, vui128_t value)
{
  /* Magic numbers for multiplicative inverse to divide by 10**32
   are 211857340822306639531405861550393824741, corrective add,
   and shift right 107 bits.  */
  const vui128_t mul_invs_ten32 = (vui128_t) CONST_VINT128_DW(
      0x9f623d5a8a732974UL, 0xcfbc31db4b0295e5UL);
  const int shift_ten32 = 107;
  /* Magic numbers for multiplicative inverse to divide by 10**16
   are 76624777043294442917917351357515459181, no corrective add,
   and shift right 51 bits.  */
  const vui128_t mul_invs_ten16 = (vui128_t) CONST_VINT128_DW(
      0x39a5652fb1137856UL, 0xd30baf9a1e626a6dUL);
  const int shift_ten16 = 51;

  const vui128_t mul_ten32 = (vui128_t) (__int128) 100000000000000ll
      * (__int128) 1000000000000000000ll;
  const vui128_t mul_ten16 = (vui128_t) CONST_VINT128_DW(0UL,
							 10000000000000000UL);

  vui128_t tmpq, tmpr, tmpc, tmp;

  // First divide/modulo by 10**32 to separate the top 7 digits from
  // the lower 32 digits
  // tmpq = floor ( M * value / 2**128)
  tmpq = vec_mulhuq (value, mul_invs_ten32);
  // Corrective add may overflow, generate carry
  tmpq = vec_adduqm (tmpq, value);
  tmpc = vec_addcuq (tmpq, value);
  // Shift right with carry bit
  tmpq = vec_sldqi (tmpc, tmpq, (128 - shift_ten32));
  // Compute remainder of value / 10**32
  // tmpr = value - (tmpq * 10**32)
  tmp = vec_mulluq (tmpq, mul_ten32);
  tmpr = vec_subuqm (value, tmp);

  // return top 16 digits
  ten_16[0] = (vui64_t) tmpq[VEC_DW_L];

  // Next divide/modulo the remaining 32 digits by 10**16.
  // This separates the middle and low 16 digits into doublewords.
  tmpq = vec_mulhuq (tmpr, mul_invs_ten16);
  tmpq = vec_srqi (tmpq, shift_ten16);
  // Compute remainder of tmpr / 10**16
  // tmpr = tmpr - (tmpq * 10**16)
  // Here we know tmpq and mul_ten16 are less then 64-bits
  // so can use vec_vmuloud insted of vec_mulluq
  tmp = vec_vmuloud ((vui64_t) tmpq, (vui64_t) mul_ten16);
  tmpr = vec_subuqm (value, tmp);

  // return middle 16 digits
  ten_16[1] = (vui64_t) tmpq[VEC_DW_L];
  // return low 16 digits
  ten_16[2] = (vui64_t) tmpr[VEC_DW_L];
}

vui128_t
example_longdiv_10e31_PWR9 (vui128_t *q, vui128_t *d, long int _N)
{
  vui128_t dn, qh, ql, rh;
  long int i;

  // init step for the top digits
  dn = d[0];
  qh = vec_divuq_10e31 (dn);
  rh = vec_moduq_10e31 (dn, qh);
  q[0] = qh;

  // now we know the remainder is less than the divisor.
  for (i=1; i<_N; i++)
    {
      dn = d[i];
      ql = vec_divudq_10e31 (&qh, rh, dn);
      rh = vec_modudq_10e31 (rh, dn, &ql);
      q[i] = ql;
    }

  return rh;
}

long int
example_longbcdct_10e32_PWR9 (vui128_t *d, vBCD_t decimal, long int _C , long int _N)
{
  /* ten32  = +100000000000000000000000000000000UQ  */
  const vui128_t ten32 = (vui128_t)
	  { (__int128) 10000000000000000UL * (__int128) 10000000000000000UL };
  const vui128_t zero = (vui128_t) { (__int128) 0UL };
  vui128_t dn, ph, pl, cn, c;
  long int i, cnt;

  cnt = _C;

  dn = zero;
  cn  = zero;
  if ( cnt == 0 )
    {
      if (vec_cmpuq_all_ne ((vui128_t) decimal, zero))
	{
	  cnt++;
	  dn = vec_bcdctuq (decimal);
	}

      for ( i = 0; i < (_N - 1); i++ )
	{
	  d[i] = zero;
	}
      d[_N - cnt] = dn;
    }
  else
    {
      if (vec_cmpuq_all_ne ((vui128_t) decimal, zero))
	{
	  dn = vec_bcdctuq (decimal);
	}
      for ( i = (_N - 1); i >= (_N - cnt); i--)
	{
	  pl = vec_muludq (&ph, d[i], ten32);

	  c = vec_addecuq (pl, dn, cn);
	  d[i] = vec_addeuqm (pl, dn, cn);
	  cn = c;
	  dn = ph;
	}
      if (vec_cmpuq_all_ne (dn, zero) || vec_cmpuq_all_ne (cn, zero))
	{
	  cnt++;
	  dn = vec_adduqm (dn, cn);
	  d[_N - cnt] = dn;
	}
    }

  return cnt;
}

long int
example_longbcdct_10e31_PWR9 (vui128_t *d, vBCD_t decimal, long int _C,
			      long int _N)
{
  const vui128_t ten31 = (vui128_t)
	  { (__int128) 1000000000000000UL * (__int128) 10000000000000000UL };
  const vui128_t zero = (vui128_t) { (__int128) 0UL };
  vui128_t dn, ph, pl, cn, c;
  long int i, cnt;

  cnt = _C;

  dn = zero;
  cn = zero;
  if (cnt == 0)
    {
      if (vec_cmpuq_all_ne ((vui128_t) decimal, zero))
	{
	  cnt++;
	  dn = (vui128_t) vec_bcdctsq (decimal);
	}

      for (i = 0; i < (_N - 1); i++)
	{
	  d[i] = zero;
	}
      d[_N - cnt] = dn;
    }
  else
    {
      if (vec_cmpuq_all_ne ((vui128_t) decimal, zero))
	{
	  dn = (vui128_t) vec_bcdctsq (decimal);
	}
      for ( i = (_N - 1); i >= (_N - cnt); i--)
	{
	  pl = vec_muludq (&ph, d[i], ten31);

	  c = vec_addecuq (pl, dn, cn);
	  d[i] = vec_addeuqm (pl, dn, cn);
	  cn = c;
	  dn = ph;
	}
      if (vec_cmpuq_all_ne (dn, zero) || vec_cmpuq_all_ne (cn, zero))
	{
	  cnt++;
	  dn = vec_adduqm (dn, cn);
	  d[_N - cnt] = dn;
	}
    }

  return cnt;
}

void
test_muluq_4x1_PWR9 (vui128_t *__restrict__ mulu, vui128_t m10, vui128_t m11,
		     vui128_t m12, vui128_t m13, vui128_t m2)
{
  vui128_t mq2, mq1, mq0, mq, mc;
  vui128_t mpx0, mpx1, mpx2, mpx3;
  mpx3 = vec_muludq (&mq2, m13, m2);
  mpx2 = vec_muludq (&mq1, m12, m2);
  mpx2 = vec_addcq (&mc, mpx2, mq2);
  mpx1 = vec_muludq (&mq0, m11, m2);
  mpx1 = vec_addeq (&mc, mpx1, mq1, mc);
  mpx0 = vec_muludq (&mq, m10, m2);
  mpx0 = vec_addeq (&mc, mpx0, mq0, mc);
  mq = vec_adduqm (mc, mq);

  mulu[0] = mq;
  mulu[1] = mpx0;
  mulu[2] = mpx1;
  mulu[3] = mpx2;
  mulu[4] = mpx3;
}

void
test_mul4uq_PWR9 (vui128_t *__restrict__ mulu, vui128_t m1h, vui128_t m1l,
		  vui128_t m2h, vui128_t m2l)
{
  vui128_t mc, mp, mq, mqhl;
  vui128_t mphh, mphl, mplh, mpll;
  mpll = vec_muludq (&mplh, m1l, m2l);
  mp = vec_muludq (&mphl, m1h, m2l);
  mplh = vec_addcq (&mc, mplh, mp);
  mphl = vec_adduqm (mphl, mc);
  mp = vec_muludq (&mqhl, m2h, m1l);
  mplh = vec_addcq (&mq, mplh, mp);
  mphl = vec_addeq (&mc, mphl, mqhl, mq);
  mp = vec_muludq (&mphh, m2h, m1h);
  mphl = vec_addcq (&mq, mphl, mp);
  mphh = vec_addeuqm (mphh, mq, mc);

  mulu[0] = mpll;
  mulu[1] = mplh;
  mulu[2] = mphl;
  mulu[3] = mphh;
}

void __attribute__((flatten ))
#if 0
test_vec_mul1024x1024_PWR9 (__VEC_U_2048* restrict r2048,
				  __VEC_U_1024* restrict m1_1024,
				  __VEC_U_1024* restrict m2_1024)
#else
test_vec_mul1024x1024_PWR9 (__VEC_U_2048* r2048,
				  __VEC_U_1024* m1_1024,
				  __VEC_U_1024* m2_1024)
#endif
{
  __VEC_U_1024x512 subp0, subp1, subp2, subp3;
  __VEC_U_512x1 sum0, sum1, sum2, sum3, sumx;
  __VEC_U_512 temp[3];
#if 0
  __VEC_U_1024x512 * restrict pm1 = (__VEC_U_1024x512 *) m1_1024;
  __VEC_U_1024x512 * restrict pm2 = (__VEC_U_1024x512 *) m2_1024;
  __VEC_U_2048x512 * restrict pm2048 = (__VEC_U_2048x512 *) r2048;
#else
  __VEC_U_1024x512 * pm1 = (__VEC_U_1024x512 *) m1_1024;
  __VEC_U_1024x512 * pm2 = (__VEC_U_1024x512 *) m2_1024;
  __VEC_U_2048x512 * pm2048 = (__VEC_U_2048x512 *) r2048;
#endif

  subp0.x1024 = vec_mul512x512_inline (pm1->x2.v0x512, pm2->x2.v0x512);
  pm2048->x4.v0x512 = subp0.x2.v0x512;

  subp1.x1024 = vec_mul512x512_inline (pm1->x2.v1x512, pm2->x2.v0x512);
  sum1.x640 = vec_add512cu (subp1.x2.v0x512, subp0.x2.v1x512);

  temp[0] = sum1.x2.v0x512;
  temp[1] = vec_add512ze (subp1.x2.v1x512, sum1.x2.v1x128);
  COMPILE_FENCE;

  subp2.x1024 = vec_mul512x512_inline (pm1->x2.v0x512, pm2->x2.v1x512);
  sum2.x640 = vec_add512cu (temp[0], subp2.x2.v0x512);
  temp[2] = sum2.x2.v0x512;
  pm2048->x4.v1x512 = temp[2];
  sumx.x640 = vec_add512ecu (temp[1], subp2.x2.v1x512, sum2.x2.v1x128);
  temp[1] = sumx.x2.v0x512;
  COMPILE_FENCE;

  subp3.x1024 = vec_mul512x512_inline (pm1->x2.v1x512, pm2->x2.v1x512);
  sum3.x640 = vec_add512cu (sumx.x2.v0x512, subp3.x2.v0x512);
  pm2048->x4.v2x512 = sum3.x2.v0x512;
  pm2048->x4.v3x512 = vec_add512ze2 (subp3.x2.v1x512, sumx.x2.v1x128,
				     sum3.x2.v1x128);
}

//#pragma GCC pop target
