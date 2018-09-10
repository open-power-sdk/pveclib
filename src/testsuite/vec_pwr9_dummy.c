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
