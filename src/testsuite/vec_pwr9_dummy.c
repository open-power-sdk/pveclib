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

#include <vec_int128_ppc.h>
#include <vec_f128_ppc.h>

vui8_t
__test_absdub_PWR9 (vui8_t __A, vui8_t __B)
{
  return vec_absdub (__A, __B);
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
__test_mulhuq_PWR9 (vui128_t a, vui128_t b)
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

    if (vec_all_isnormalf128 (value))
      {
	/* body of vec_sin() computation elided for this example.  */
	result = 0.0q;
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

    if (vec_all_isfinitef128 (value))
      {
	if (vec_all_iszerof128 (value))
	  result = 1.0q;
	else
	  {
	    /* body of vec_cos() computation elided for this example.  */
	    result = 0.0q;
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
int
__test_scalar_test_data_class_f128 (__binary128 val)
{
  return scalar_test_data_class (val, 0x7f);
}

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
int
__test_scalar_test_neg (__ieee128 val)
{
  return scalar_test_neg (val);
}
#endif

#ifdef scalar_extract_exp
long long int
__test_scalar_extract_exp_f128 (__ieee128 val)
{
  return scalar_extract_exp (val);
}

int
__test_scalar_extract_exp_f64 (double val)
{
  return scalar_extract_exp (val);
}
#endif

#ifdef scalar_extract_sig
__int128
__test_scalar_extract_sig_f128 (__ieee128 val)
{
  return scalar_extract_sig (val);
}

long long int
__test_scalar_extract_sig_f64 (double val)
{
  return scalar_extract_sig (val);
}
#endif

#ifdef scalar_insert_exp
__ieee128
__test_scalar_insert_exp_f128 (__ieee128 sig, unsigned long long int exp)
{
  return scalar_insert_exp (sig, exp);
}

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

void
test_muluq_4x1_PWR9 (vui128_t *__restrict__ mulu, vui128_t m10, vui128_t m11, vui128_t m12, vui128_t m13, vui128_t m2)
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
  mq   = vec_adduqm (mc, mq);

  mulu[0] = mpx0;
  mulu[1] = mpx1;
  mulu[2] = mpx2;
  mulu[3] = mpx3;
}

void
test_mul4uq_PWR9 (vui128_t *__restrict__ mulu, vui128_t m1h, vui128_t m1l, vui128_t m2h, vui128_t m2l)
{
  vui128_t mc, mp, mq;
  vui128_t mphh, mphl, mplh, mpll;
  mpll = vec_muludq (&mplh, m1l, m2l);
  mp = vec_muludq (&mphl, m1h, m2l);
  mplh = vec_addcq (&mc, mplh, mp);
  mphl = vec_addcuq (mphl, mc);
  mp = vec_muludq (&mc, m2h, m1l);
  mplh = vec_addcq (&mq, mplh, mp);
  mphl = vec_addcq (&mc, mphl, mq);
  mp = vec_muludq (&mphh, m2h, m1h);
  mplh = vec_addcq (&mc, mplh, mp);
  mphl = vec_addcuq (mphh, mc);

  mulu[0] = mpll;
  mulu[1] = mplh;
  mulu[2] = mphl;
  mulu[3] = mphh;
}

//#pragma GCC pop target
