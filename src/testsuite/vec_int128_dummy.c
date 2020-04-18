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

 vec_int128_dummy.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: May 9, 2017
 */

#include <stdint.h>
#include <stdio.h>
#include <pveclib/vec_int128_ppc.h>
#include "arith128.h"
#include <testsuite/arith128_print.h>

vui128_t
__test_msumudm (vui64_t a, vui64_t b, vui128_t c)
{
  return vec_msumudm ( a, b, c);
}

vui128_t
test_vec_add256_1 (vui128_t *__restrict__ out, vui128_t a0, vui128_t a1, vui128_t b0, vui128_t b1)
{
  vui128_t s0, s1, c0, c1;
  s1 = vec_adduqm (a1, b1);
  c1 = vec_addcuq (a1, b1);
  s0 = vec_addeuqm (a0, b0, c1);
  c0 = vec_addecuq (a0, b0, c1);

  out[1] = s1;
  out[0] = s0;
  return (c0);
}
vui128_t
test_vec_add256_2 (vui128_t *__restrict__ out, vui128_t a0, vui128_t a1, vui128_t b0, vui128_t b1)
{
  vui128_t s0, s1, c0, c1;
  s1 = vec_addcq (&c1, a1, b1);
  s0 = vec_addeq (&c0, a0, b0, c1);

  out[1] = s1;
  out[0] = s0;
  return (c0);
}

vui128_t
test_vec_add512_1 (vui128_t *__restrict__ out, vui128_t *__restrict__ a,
		   vui128_t *__restrict__ b)
{
  vui128_t s0, s1, s2, s3, c0, c1, c2, c3;
  s3 = vec_adduqm (a[3], b[3]);
  c3 = vec_addcuq (a[3], b[3]);
  s2 = vec_addeuqm (a[2], b[2], c3);
  c2 = vec_addecuq (a[2], b[2], c3);
  s1 = vec_addeuqm (a[1], b[1], c2);
  c1 = vec_addecuq (a[1], b[1], c2);
  s0 = vec_addeuqm (a[0], b[0], c1);
  c0 = vec_addecuq (a[0], b[0], c1);

  out[3] = s3;
  out[2] = s2;
  out[1] = s1;
  out[0] = s0;
  return (c0);
}

vui128_t
test_vec_add512_2 (vui128_t *__restrict__ out, vui128_t *__restrict__ a,
		   vui128_t *__restrict__ b)
{
  vui128_t s0, s1, s2, s3, c0, c1, c2, c3;
  s3 = vec_addcq (&c3, a[3], b[3]);
  s2 = vec_addeq (&c2, a[2], b[2], c3);
  s1 = vec_addeq (&c1, a[1], b[1], c2);
  s0 = vec_addeq (&c0, a[0], b[1], c1);

  out[3] = s3;
  out[2] = s2;
  out[1] = s1;
  out[0] = s0;
  return (c0);
}

vui128_t
test_vec_absduq (vui128_t vra, vui128_t vrb)
{
  return vec_absduq (vra, vrb);
}

vui128_t
test_vec_avguq (vui128_t vra, vui128_t vrb)
{
  return vec_avguq (vra, vrb);
}

vui128_t
test_vec_addeuqm_ecuq (vui128_t *p, vui128_t a, vui128_t b, vui128_t ci)
{
  *p = vec_addecuq (a, b, ci);
  return vec_addeuqm (a, b, ci);
}

vui128_t
test_vec_adduqm_cuq (vui128_t *p, vui128_t a, vui128_t b)
{
  *p = vec_addcuq (a, b);
   return vec_adduqm (a, b);
}

vui128_t
test_vec_subuqm (vui128_t a, vui128_t b)
{
  return (vec_subuqm (a, b));
}

vui128_t
test_vec_subcuq (vui128_t a, vui128_t b)
{
  return (vec_subcuq (a, b));
}

vui128_t
test_vec_subeuqm_ecuq (vui128_t *p, vui128_t a, vui128_t b, vui128_t ci)
{
  *p = vec_subecuq (a, b, ci);
  return vec_subeuqm (a, b, ci);
}

vui128_t
test_vec_subuqm_cuq (vui128_t *p, vui128_t a, vui128_t b)
{
  *p = vec_subcuq (a, b);
  return vec_subuqm (a, b);
}

vb128_t
test_vec_cmpeqsq (vi128_t a, vi128_t b)
{
  return (vec_cmpeqsq (a, b));
}

vb128_t
test_vec_cmpgesq (vi128_t a, vi128_t b)
{
  return (vec_cmpgesq (a, b));
}

vb128_t
test_vec_cmpgtsq (vi128_t a, vi128_t b)
{
  return (vec_cmpgtsq (a, b));
}

vb128_t
test_vec_cmplesq (vi128_t a, vi128_t b)
{
  return (vec_cmplesq (a, b));
}

vb128_t
test_vec_cmpltsq (vi128_t a, vi128_t b)
{
  return (vec_cmpltsq (a, b));
}

vb128_t
test_vec_cmpequq (vui128_t a, vui128_t b)
{
  return (vec_cmpequq (a, b));
}

vb128_t
test_vec_cmpgtuq (vui128_t a, vui128_t b)
{
  return (vec_cmpgtuq (a, b));
}

vb128_t
test_vec_cmpltuq (vui128_t a, vui128_t b)
{
  return (vec_cmpltuq (a, b));
}

vb128_t
test_vec_cmpgeuq (vui128_t a, vui128_t b)
{
  return (vec_cmpgeuq (a, b));
}

vb128_t
test_vec_cmpleuq (vui128_t a, vui128_t b)
{
  return (vec_cmpleuq (a, b));
}

vb128_t
test_vec_cmpneuq (vui128_t a, vui128_t b)
{
  return (vec_cmpneuq (a, b));
}

int
test_cmpsq_all_eq (vi128_t a, vi128_t b)
{
  return vec_cmpsq_all_eq (a, b);
}

int
test_cmpsq_all_ge (vi128_t a, vi128_t b)
{
  return vec_cmpsq_all_ge (a, b);
}

int
test_cmpsq_all_gt (vi128_t a, vi128_t b)
{
  return vec_cmpsq_all_gt (a, b);
}

int
test_cmpsq_all_le (vi128_t a, vi128_t b)
{
  return vec_cmpsq_all_le (a, b);
}

int
test_cmpsq_all_lt (vi128_t a, vi128_t b)
{
  return vec_cmpsq_all_lt (a, b);
}

int
test_cmpsq_all_ne (vi128_t a, vi128_t b)
{
  return vec_cmpsq_all_ne (a, b);
}

int
test_cmpuq_all_eq (vui128_t a, vui128_t b)
{
  return vec_cmpuq_all_eq (a, b);
}

int
test_cmpuq_all_ge (vui128_t a, vui128_t b)
{
  return vec_cmpuq_all_ge (a, b);
}

int
test_cmpuq_all_gt (vui128_t a, vui128_t b)
{
  return vec_cmpuq_all_gt (a, b);
}

int
test_cmpuq_all_le (vui128_t a, vui128_t b)
{
  return vec_cmpuq_all_le (a, b);
}

int
test_cmpuq_all_lt (vui128_t a, vui128_t b)
{
  return vec_cmpuq_all_lt (a, b);
}

int
test_cmpuq_all_ne (vui128_t a, vui128_t b)
{
  return vec_cmpuq_all_ne (a, b);
}

vui128_t
test_setb_cyq (vui128_t vcy)
{
  return vec_setb_cyq (vcy);
}

vui128_t
test_setb_ncq (vui128_t vcy)
{
  return vec_setb_ncq (vcy);
}

vui128_t
test_setb_sq (vi128_t vcy)
{
  return vec_setb_sq (vcy);
}

vi128_t
__test_maxsq (vi128_t __VH, vi128_t __VL)
{
  return vec_maxsq (__VH, __VL);
}

vui128_t
__test_maxuq (vui128_t __VH, vui128_t __VL)
{
  return vec_maxuq (__VH, __VL);
}

vi128_t
__test_minsq (vi128_t __VH, vi128_t __VL)
{
  return vec_minsq (__VH, __VL);
}

vui128_t
__test_minuq (vui128_t __VH, vui128_t __VL)
{
  return vec_minuq (__VH, __VL);
}

vi128_t
__test_minsq3 (vi128_t vra, vi128_t vrb, vi128_t vrc)
{
  return vec_minsq (vec_minsq (vra, vrb), vrc);
}

vui128_t
__test_minuq3 (vui128_t vra, vui128_t vrb, vui128_t vrc)
{
  return vec_minuq (vec_minuq (vra, vrb), vrc);
}

vui128_t
test_vec_mul10uq_cuq (vui128_t *p, vui128_t a)
{
  *p = vec_mul10cuq (a);
   return vec_mul10uq (a);
}

vui128_t
test_vec_cmul10uq_ecuq (vui128_t *p, vui128_t a, vui128_t a2, vui128_t cin)
{
  vui128_t k, j;
  k = vec_cmul10ecuq (&j, a, cin);
  *p = vec_mul10euq ((vui128_t) a2, j);
  return k;
}

vui128_t
test_vec_addeq (vui128_t *cout, vui128_t a, vui128_t b, vui128_t c)
{
  return (vec_addeq (cout, a, b, c));
}

vui128_t
test_vec_addcq (vui128_t *cout, vui128_t a, vui128_t b)
{
  return (vec_addcq (cout, a, b));
}

vui128_t
test_vec_addecuq (vui128_t a, vui128_t b, vui128_t c)
{
  return (vec_addecuq (a, b, c));
}

vui128_t
test_vec_addeuqm (vui128_t a, vui128_t b, vui128_t ci)
{
  return (vec_addeuqm (a, b, ci));
}

vui128_t
test_vec_adduqm (vui128_t a, vui128_t b)
{
  return (vec_adduqm (a, b));
}

vui128_t
test_vec_addcuq (vui128_t a, vui128_t b)
{
  return (vec_addcuq (a, b));
}

vui128_t
test_vec_slqi_0 (vui128_t __A)
{
  return vec_slqi (__A, 0);
}

vui128_t
test_vec_slqi_4 (vui128_t __A)
{
  return vec_slqi (__A, 4);
}

vui128_t
test_vec_slqi_7 (vui128_t __A)
{
  return vec_slqi (__A, 7);
}

vui128_t
test_vec_slqi_8 (vui128_t __A)
{
  return vec_slqi (__A, 8);
}

vui128_t
test_vec_slqi_14 (vui128_t __A)
{
  return vec_slqi (__A, 14);
}

vui128_t
test_vec_slqi_16 (vui128_t __A)
{
  return vec_slqi (__A, 16);
}
#if 1

vui128_t
test_vec_slqi_17 (vui128_t __A)
{
  return vec_slqi (__A, 17);
}
vui128_t
test_vec_slqi_31 (vui128_t __A)
{
  return vec_slqi (__A, 31);
}

vui128_t
test_vec_slqi_48 (vui128_t __A)
{
  return vec_slqi (__A, 48);
}

vui128_t
test_vec_slqi_120 (vui128_t __A)
{
  return vec_slqi (__A, 120);
}

vui128_t
test_vec_slqi_128 (vui128_t __A)
{
  return vec_slqi (__A, 128);
}

vui128_t
test_vec_slqi_129 (vui128_t __A)
{
  return vec_slqi (__A, 129);
}
#endif

vui128_t
test_vec_srqi_0 (vui128_t __A)
{
  return vec_srqi (__A, 0);
}

vui128_t
test_vec_srqi_4 (vui128_t __A)
{
  return vec_srqi (__A, 4);
}

vui128_t
test_vec_srqi_7 (vui128_t __A)
{
  return vec_srqi (__A, 7);
}

vui128_t
test_vec_srqi_8 (vui128_t __A)
{
  return vec_srqi (__A, 8);
}

vui128_t
test_vec_srqi_14 (vui128_t __A)
{
  return vec_srqi (__A, 14);
}

vui128_t
test_vec_srqi_16 (vui128_t __A)
{
  return vec_srqi (__A, 16);
}
#if 1
vui128_t
test_vec_srqi_31 (vui128_t __A)
{
  return vec_srqi (__A, 31);
}

vui128_t
test_vec_srqi_48 (vui128_t __A)
{
  return vec_srqi (__A, 48);
}

vui128_t
test_vec_srqi_120 (vui128_t __A)
{
  return vec_srqi (__A, 120);
}

vui128_t
test_vec_srqi_128 (vui128_t __A)
{
  return vec_srqi (__A, 128);
}

vui128_t
test_vec_srqi_129 (vui128_t __A)
{
  return vec_srqi (__A, 129);
}
#endif

vi128_t
test_vec_sraqi_0 (vi128_t __A)
{
  return vec_sraqi (__A, 0);
}

vi128_t
test_vec_sraqi_4 (vi128_t __A)
{
  return vec_sraqi (__A, 4);
}

vi128_t
test_vec_sraqi_7 (vi128_t __A)
{
  return vec_sraqi (__A, 7);
}

vi128_t
test_vec_sraqi_8 (vi128_t __A)
{
  return vec_sraqi (__A, 8);
}

vi128_t
test_vec_sraqi_14 (vi128_t __A)
{
  return vec_sraqi (__A, 14);
}

vi128_t
test_vec_sraqi_16 (vi128_t __A)
{
  return vec_sraqi (__A, 16);
}
#if 1
vi128_t
test_vec_sraqi_31 (vi128_t __A)
{
  return vec_sraqi (__A, 31);
}

vi128_t
test_vec_sraqi_48 (vi128_t __A)
{
  return vec_sraqi (__A, 48);
}

vi128_t
test_vec_sraqi_68 (vi128_t __A)
{
  return vec_sraqi (__A, 68);
}

vi128_t
test_vec_sraqi_120 (vi128_t __A)
{
  return vec_sraqi (__A, 120);
}

vi128_t
test_vec_sraqi_128 (vi128_t __A)
{
  return vec_sraqi (__A, 128);
}

vi128_t
test_vec_sraqi_129 (vi128_t __A)
{
  return vec_sraqi (__A, 129);
}
#endif

vui128_t
test_vec_vsumsws (vui128_t vra)
{
  const __vector unsigned long long vzero =
    { 0, 0 };
  return (vui128_t) __builtin_altivec_vsumsws ((__vector int)vra, (__vector int)vzero);
}

vui128_t
test_vec_clzq (vui128_t vra)
{
  return vec_clzq (vra);
}

vui128_t
test_vec_popcntq (vui128_t vra)
{
  return vec_popcntq (vra);
}

vui128_t
test_cmul10cuq (vui128_t *cout, vui128_t a)
{
  return vec_cmul10cuq (cout, a);
}

vui128_t
test_cmul10ecuq (vui128_t *cout, vui128_t a, vui128_t cin)
{
  return vec_cmul10ecuq (cout, a, cin);
}

vui128_t
test_cmul100cuq (vui128_t *cout, vui128_t a)
{
  return vec_cmul100cuq (cout, a);
}

vui128_t
test_cmul100ecuq (vui128_t *cout, vui128_t a, vui128_t cin)
{
  return vec_cmul100ecuq (cout, a, cin);
}

vui128_t
test_vec_mul10uq_c (vui128_t *p, vui128_t a)
{
  *p = vec_mul10cuq (a);
  return vec_mul10uq (a);
}

/* This does not work because the target is applied after the CPP
 * platform specific have been applied based on the command line
 * =mcpu=<target>.
 * Keep this example as a warning. */
vui128_t __attribute__((__target__("cpu=power9")))
test_vec_mul10uq (vui128_t a)
{
	return vec_mul10uq (a);
}

vui128_t
test_vec_mul10euq (vui128_t a, vui128_t cin)
{
	return vec_mul10euq (a, cin);
}

vui128_t
test_vec_mul10cuq (vui128_t a)
{
	return vec_mul10cuq (a);
}

vui128_t
test_vec_mul10ecuq (vui128_t a, vui128_t cin)
{
	return vec_mul10ecuq (a, cin);
}

vui128_t
test_vec_revbq (vui128_t a)
{
	return vec_revbq(a);
}

void
test_vec_load_store (vui128_t *a, vui128_t *b)
  {
    vui64_t temp;

    temp = vec_ld (0, (vui64_t *)a);
    vec_st (temp, 0, (vui64_t *)b);
  }

vui64_t
test_vpaste_x (vui64_t __VH, vui64_t __VL)
{
  vui64_t result;
  result[1] = __VH[1];
  result[0] = __VL[0];
  return (result);
}

vui128_t
test_vsl4 (vui128_t a)
{
	return (vec_slq4(a));
}

vui128_t
test_vsr4 (vui128_t a)
{
	return (vec_srq4(a));
}

vui128_t
test_vec_sldq (vui128_t a, vui128_t b, vui128_t sh)
{
  return (vec_sldq (a, b, sh));
}

vui128_t
test_vec_sldqi_0 (vui128_t a, vui128_t b)
{
  return (vec_sldqi (a, b, 0));
}

vui128_t
test_vec_sldqi_15 (vui128_t a, vui128_t b)
{
  return (vec_sldqi (a, b, 15));
}

vui128_t
test_vec_sldqi_48 (vui128_t a, vui128_t b)
{
  return (vec_sldqi (a, b, 48));
}

vui128_t
test_vec_sldqi_52 (vui128_t a, vui128_t b)
{
  return (vec_sldqi (a, b, 52));
}

vui128_t
test_vec_rlq  (vui128_t a, vui128_t sh)
{
  return (vec_rlq (a, sh));
}

vui128_t
test_vec_srq  (vui128_t a, vui128_t sh)
{
  return (vec_srq (a, sh));
}

vi128_t
test_vec_sraq  (vi128_t a, vui128_t sh)
{
  return (vec_sraq (a, sh));
}

vui128_t
test_vec_slq  (vui128_t a, vui128_t sh)
{
  return (vec_slq (a, sh));
}

vui128_t
__test_vec_msumudm (vui64_t a, vui64_t b, vui128_t c)
{
  return (vec_msumudm (a, b, c));
}

vui128_t
__test_muloud (vui64_t a, vui64_t b)
{
  return (vec_muloud (a, b));
}

vui128_t
__test_muleud (vui64_t a, vui64_t b)
{
  return (vec_muleud (a, b));
}

vui64_t
__test_muludm (vui64_t a, vui64_t b)
{
  return (vec_muludm (a, b));
}

vui64_t
__test_mulhud (vui64_t a, vui64_t b)
{
  return (vec_mulhud (a, b));
}

vui128_t
__test_vmuloud (vui64_t a, vui64_t b)
{
  return (vec_vmuloud (a, b));
}

vui128_t
__test_vmuleud (vui64_t a, vui64_t b)
{
  return (vec_vmuleud (a, b));
}

vui128_t
__test_vmsumoud (vui64_t a, vui64_t b, vui128_t c)
{
  return vec_vmsumoud (a, b, c);
}

vui128_t
__test_vmsumeud (vui64_t a, vui64_t b, vui128_t c)
{
  return vec_vmsumeud (a, b, c);
}

vui128_t
__test_vmaddoud (vui64_t a, vui64_t b, vui64_t c)
{
  return vec_vmaddoud (a, b, c);
}

vui128_t
__test_vmadd2oud (vui64_t a, vui64_t b, vui64_t c, vui64_t d)
{
  return vec_vmadd2oud (a, b, c, d);
}

vui128_t
__test_vmaddeud (vui64_t a, vui64_t b, vui64_t c)
{
  return vec_vmaddeud (a, b, c);
}

vui128_t
__test_vmadd2eud (vui64_t a, vui64_t b, vui64_t c, vui64_t d)
{
  return vec_vmadd2eud (a, b, c, d);
}

vui128_t
__test_muludq (vui128_t *mulu, vui128_t a, vui128_t b)
{
  return (vec_muludq (mulu, a, b));
}

vui128_t
__test_madduq (vui128_t *mulu, vui128_t a, vui128_t b, vui128_t c)
{
  return vec_madduq (mulu, a, b, c);
}

vui128_t
__test_madd2uq (vui128_t *mulu, vui128_t a, vui128_t b, vui128_t c, vui128_t d)
{
  return vec_madd2uq (mulu, a, b, c, d);
}

vui128_t
__test_madduq_x (vui128_t *mulu, vui128_t a, vui128_t b, vui128_t c)
{
  vui128_t ph, pl, cl;
  pl = vec_muludq (&ph, a, b);
  pl = vec_adduqm (pl, c);
  cl = vec_addcuq (pl, c);
  *mulu = vec_adduqm (ph, cl);
  return (pl);
}

vui128_t
__test_mulluq (vui128_t a, vui128_t b)
{
  return (vec_mulluq (a, b));
}

vui128_t
__test_mulhuq (vui128_t a, vui128_t b)
{
  return vec_mulhuq (a, b);;
}

vui128_t
__test_mulhuq2 (vui128_t a, vui128_t b)
{
  vui128_t mq, r;
  r = vec_muludq (&mq, a, b);
  return mq;
}

vi128_t
__test_modsq_10e31  (vi128_t a, vi128_t b)
{
  return vec_modsq_10e31 (a, b);
}

vi128_t
__test_remsq_10e31  (vi128_t a, vi128_t b)
{
  vi128_t q;
  q = vec_divsq_10e31 (a);
  return vec_modsq_10e31 (a, q);
}

vui128_t
__test_remuq_10e31  (vui128_t a, vui128_t b)
{
  vui128_t q;
  q = vec_divuq_10e31 (a);
  return vec_moduq_10e31 (a, q);
}

vui128_t
__test_remuq_10e32  (vui128_t a, vui128_t b)
{
  vui128_t q;
  q = vec_divuq_10e32 (a);
  return vec_moduq_10e32 (a, q);
}

vui128_t
__test_divudq_10e31  (vui128_t *qh, vui128_t a, vui128_t b)
{
  return vec_divudq_10e31 (qh, a, b);
}

vui128_t
__test_divudq_10e32  (vui128_t *qh, vui128_t a, vui128_t b)
{
  return vec_divudq_10e32 (qh, a, b);
}

vui128_t
__test_remudq_10e31  (vui128_t a, vui128_t b)
{
  vui128_t q, qh;
  q = vec_divudq_10e31 (&qh, a, b);
  return vec_modudq_10e31 (a, b, &q);
}

vui128_t
__test_remudq_10e32  (vui128_t a, vui128_t b)
{
  vui128_t q, qh;
  q = vec_divudq_10e32 (&qh, a, b);
  return vec_modudq_10e32 (a, b, &q);
}

#ifdef _ARCH_PWR8
vui128_t
test_vec_subq (vui128_t a, vui128_t b)
{
  return (vec_sub(a,b));
}
#endif

void
test_mul4uq (vui128_t *__restrict__ mulu, vui128_t m1h, vui128_t m1l,
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

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define __NDX(__index) (__index)
#define __MDX(__index) (__index)
#define __PDX(__index) (__index)
#else
#define __NDX(__index) ((nx - 1) - (__index))
#define __MDX(__index) ((mx - 1) - (__index))
#define __PDX(__index) ((px - 1) - (__index))
#endif
void
test_mul128_MN (vui128_t *p, vui128_t *m1, vui128_t *m2,
		  unsigned long M, unsigned long N)
{
  vui128_t *mp = m1;
  vui128_t *np = m2;
  unsigned long mx = M;
  unsigned long nx = N;
  unsigned long px = M+N;
  unsigned long i, j;
  vui128_t mpx0, mqx0, mpx1, mqx1;

  /* sizeof(m1) < sizeof(m2) swap the pointers and quadword counts.
   * This allows for early exit when size of either is 1. */
  if (mx < nx)
    {
      vui128_t *xp;
      unsigned long x;

      x = mx;
      xp = mp;
      mx = nx;
      mp = np;
      nx = x;
      np = xp;
    }

//  printf ("MXD(0)=%lu, NDX(0)=%lu, PDX(0)=%lu\n", __MDX(0), __NDX(0), __PDX(0));
  mpx0 = vec_muludq (&mqx0, mp[__MDX(0)], np[__NDX(0)]);
  p[__PDX(0)] = mpx0;
  for ( i=1; i < mx; i++ )
    {
      mpx1 = vec_madduq (&mqx1, mp[__MDX(i)], np[__NDX(0)], mqx0);
      p[__PDX(i)] = mpx1;
      mqx0 = mqx1;
//      printf ("MXD(i)=%lu, NDX(0)=%lu, PDX(i)=%lu\n", __MDX(i), __NDX(0), __PDX(i));
    }
  p[__PDX(mx)] = mqx0;
//  printf ("PDX(mx)=%lu\n", __PDX(mx));

  for  ( j=1; j < nx; j++ )
    {
      mpx0 = vec_madduq (&mqx0, mp[__MDX(0)], np[__NDX(j)], p[__PDX(j)]);
      p[__PDX(j)] = mpx0;
      for ( i=1; i < mx; i++ )
        {
          mpx1 = vec_madd2uq (&mqx1, mp[__MDX(i)], np[__NDX(j)], mqx0, p[__PDX(i+j)]);
          p[__PDX(i+j)] = mpx1;
          mqx0 = mqx1;
        }
      p[__PDX(mx+j)] = mqx0;
    }

}

void
example_qw_convert_decimal (vui64_t *ten_16, vui128_t value)
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


void
example_print_vint128 (vi128_t value)
{
  const vi128_t zero128 = (vi128_t) CONST_VINT128_DW(
      0x0L, 0UL);
  const vui128_t mul_ten16 = (vui128_t) CONST_VINT128_DW(
      0UL, 10000000000000000UL);
  // Magic numbers for multiplicative inverse to divide by 10**16
  // are 76624777043294442917917351357515459181, no corrective add,
  // and shift right 51 bits.
  const vui128_t mul_invs_ten16 = (vui128_t) CONST_VINT128_DW(
      0x39a5652fb1137856UL, 0xd30baf9a1e626a6dUL);
  const int shift_ten16 = 51;

  vui128_t tmpq, tmp;
  vui64_t t_low, t_mid, t_high;
  vui128_t val128;
  char sign;

  if (vec_cmpsq_all_ge (value, zero128))
    {
      sign = ' ';
      val128 = (vui128_t) value;
    }
  else
    {
      sign = '-';
      val128 = vec_subuqm ((vui128_t) zero128, (vui128_t) value);
    }
  // Convert the absolute (unsigned) value to Decimal and
  // prefix the sign.

  // first divide/modulo the 39 digits __int128 by 10**16.
  // This separates the high/middle 23 digits (tmpq) and low 16 digits.
  tmpq = vec_mulhuq (val128, mul_invs_ten16);
  tmpq = vec_srqi (tmpq, shift_ten16);
  // Compute remainder of val128 / 10**16
  // t_low = val128 - (tmpq * 10**16)
  // Here we know tmpq and mul_ten16 are less then 64-bits
  // so can use vec_vmuloud instead of vec_mulluq
  tmp = vec_vmuloud ((vui64_t) tmpq, (vui64_t) mul_ten16);
  t_low = (vui64_t) vec_subuqm (val128, tmp);

  // Next divide/modulo the high/middle digits by 10**16.
  // This separates the high 7 and middle 16 digits.
  val128 = tmpq;
  tmpq = vec_mulhuq (tmpq, mul_invs_ten16);
  t_high = (vui64_t) vec_srqi (tmpq, shift_ten16);
  tmp = vec_vmuloud (t_high, (vui64_t) mul_ten16);
  t_mid = (vui64_t) vec_subuqm (val128, tmp);

  printf ("%c%07ld%016ld%016ld", sign, t_high[VEC_DW_L],
	  t_mid[VEC_DW_L], t_low[VEC_DW_L]);
}

void
example_dw_convert_timebase (vui64_t *tb, vui32_t *timespec, int n)
{
  /* Magic numbers for multiplicative inverse to divide by 512,000,000
     are 4835703278458516699 and shift right 27 bits.  */
  const vui64_t mul_invs_clock =
    { 4835703278458516699UL, 4835703278458516699UL };
  const int shift_clock = 27;
  /* Need const for TB clocks/second to extract remainder.  */
  const vui64_t tb_clock_sec =
    { 512000000, 512000000};
  const int shift_512 = 9;
  const vui64_t nano_512 =
    { 1000, 1000};
  vui64_t tb_v, tmp, tb_clocks, seconds, nseconds;
  vui64_t timespec1, timespec2;
  int i;

  for (i = 0; i < n; i++)
    {
      tb_v = *tb++;
      /* extract integer seconds from timebase vector.  */
      tmp = vec_mulhud (tb_v, mul_invs_clock);
      seconds = vec_srdi (tmp, shift_clock);
      /* Extract remainder in tb clocks. */
      tmp = vec_muludm (seconds, tb_clock_sec);
      tb_clocks = vec_subudm (tb_v, tmp);
      /* Convert 512MHz timebase to nanoseconds.  */
      /* nseconds = tb_clocks * 1000000000 / 512000000 */
      /* reduces to (tb_clocks * 1000) >> 9 */
      tmp = vec_muludm (tb_clocks, nano_512);
      nseconds = vec_srdi (tmp, shift_512);
      /* Use merge high/low to interleave seconds and nseconds
       * into timespec.  */
      timespec1 = vec_mergeh (seconds, nseconds);
      timespec2 = vec_mergel (seconds, nseconds);
      /* seconds and nanoseconds fit int 32-bits after conversion.
       * So pack results and store the timespec.  */
      *timespec++ = vec_vpkudum (timespec1, timespec2);
    }
}

vui128_t
example_longdiv_10e31 (vui128_t *q, vui128_t *d, long int _N)
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
  // return the final remainder
  return rh;
}

vui128_t
example_longdiv_10e32 (vui128_t *q, vui128_t *d, long int _N)
{
  vui128_t dn, qh, ql, rh;
  long int i;

  // init step for the top digits
  dn = d[0];
  qh = vec_divuq_10e32 (dn);
  rh = vec_moduq_10e32 (dn, qh);
  q[0] = qh;

  // now we know the remainder is less than the divisor.
  for (i=1; i<_N; i++)
    {
      dn = d[i];
      ql = vec_divudq_10e32 (&qh, rh, dn);
      rh = vec_modudq_10e32 (rh, dn, &ql);
      q[i] = ql;
    }
  // return the final remainder
  return rh;
}

/* alternative algorithms tested and not selected due to code size
   and cycle latency.  */
vi128_t
__test_vec_cmpgesq (vi128_t vra, vi128_t vrb)
{
#ifdef _ARCH_PWR8
  vi128_t tand;
  vui128_t a_b;
  vui64_t axorb, torc, torc2;

  /* vra >=vrb: (vrb | NOT(vra)) & ((vrb ~ vra) | NOT(vra - vrb)) */
  a_b = vec_subuqm ((vui128_t)vra, (vui128_t)vrb);
  axorb = vec_xor ((vui64_t)vra, (vui64_t)vrb);
  torc = vec_orc (axorb, (vui64_t)a_b);
  torc2 = vec_orc ((vui64_t)vrb, (vui64_t)vra);
  tand = (vi128_t)vec_and (torc2, torc);
  return (vi128_t)vec_setb_sq (tand);
#else
  const vui32_t signbit = CONST_VINT128_W (0x80000000, 0, 0, 0);
  vui32_t _a, _b;

  _a = vec_xor ((vui32_t)vra, signbit);
  _b = vec_xor ((vui32_t)vrb, signbit);
  return (vi128_t)vec_cmpgeuq ((vui128_t)_a, (vui128_t)_b);
#endif
}

vi128_t
__test_vec_cmpgtsq (vi128_t vra, vi128_t vrb)
{
#ifdef _ARCH_PWR8
  vi128_t tor;
  vui128_t b_a;
  vui64_t aeqvb, tand, tandc;

  /* vra >vrb: (vrb & NOT(vra)) | (NOT(vrb ~ vra) & (vrb - vra)) */
  b_a = vec_subuqm ((vui128_t)vrb, (vui128_t)vra);
  /* NOT(a XOR b) == (a EQV b).  */
  aeqvb = vec_eqv ((vui64_t)vra, (vui64_t)vrb);
  tand = vec_and (aeqvb, (vui64_t)b_a);
  tandc = vec_andc ((vui64_t)vrb, (vui64_t)vra);
  tor = (vi128_t)vec_or (tandc, tand);
  return (vi128_t)vec_setb_sq (tor);
#else
  const vui32_t signbit = CONST_VINT128_W (0x80000000, 0, 0, 0);
  vui32_t _a, _b;

  _a = vec_xor ((vui32_t)vra, signbit);
  _b = vec_xor ((vui32_t)vrb, signbit);
  return (vi128_t)vec_cmpgtuq ((vui128_t)_a, (vui128_t)_b);
#endif
}

vi128_t
__test_vec_cmplesq (vi128_t vra, vi128_t vrb)
{
#ifdef _ARCH_PWR8
  vi128_t tand;
  vui128_t b_a;
  vui64_t axorb, torc, torc2;

  /* vra < vrb: (vra | NOT(vrb)) & ((vra ~ vrb) | NOT(vrb - vra)) */
  b_a = vec_subuqm ((vui128_t)vrb, (vui128_t)vra);
  axorb = vec_xor ((vui64_t)vra, (vui64_t)vrb);
  torc = vec_orc (axorb, (vui64_t)b_a);
  torc2 = vec_orc ((vui64_t)vra, (vui64_t)vrb);
  tand = (vi128_t)vec_and (torc2, torc);
  return (vi128_t)vec_setb_sq (tand);
#else
  const vui32_t signbit = CONST_VINT128_W (0x80000000, 0, 0, 0);
  vui32_t _a, _b;

  _a = vec_xor ((vui32_t)vra, signbit);
  _b = vec_xor ((vui32_t)vrb, signbit);
  return (vi128_t)vec_cmpleuq ((vui128_t)_a, (vui128_t)_b);
#endif
}

vi128_t
__test_vec_cmpltsq (vi128_t vra, vi128_t vrb)
{
#ifdef _ARCH_PWR8
  vi128_t tor;
  vui128_t a_b;
  vui64_t aeqvb, tand, tandc;

  /* vra < vrb: (vra & NOT(vrb)) | (NOT(vra ~ vrb) & (vra - vrb)) */
  a_b = vec_subuqm ((vui128_t)vra, (vui128_t)vrb);
  /* NOT(a XOR b) == (a EQV b).  */
  aeqvb = vec_eqv ((vui64_t)vra, (vui64_t)vrb);
  tand = vec_and (aeqvb, (vui64_t)a_b);
  tandc = vec_andc ((vui64_t)vra, (vui64_t)vrb);
  tor = (vi128_t)vec_or (tandc, tand);
  return (vi128_t)vec_setb_sq (tor);
#else
  const vui32_t signbit = CONST_VINT128_W (0x80000000, 0, 0, 0);
  vui32_t _a, _b;

  _a = vec_xor ((vui32_t)vra, signbit);
  _b = vec_xor ((vui32_t)vrb, signbit);
  return (vi128_t)vec_cmpltuq ((vui128_t)_a, (vui128_t)_b);
#endif
}

#ifdef _ARCH_PWR8
vui128_t
__test_clzq (vui128_t vra)
{
	__vector unsigned long long result, vt1, vt3;
	__vector unsigned long long vt2;
	const __vector unsigned long long vzero = {0,0};
	const __vector unsigned long long v64 = {64, 64};

	vt1 = vec_vclz ((__vector unsigned long long)vra);
	vt2 = (__vector unsigned long long)vec_cmpeq (vt1, v64);
	vt3 = (__vector unsigned long long)vec_sld ((__vector unsigned char)vzero, (__vector unsigned char)vt2, 8);
	result = vec_andc(vt1, vt3);
	result = (__vector unsigned long long)vec_sums ((__vector int)result, (__vector int)vzero);

	return ((vui128_t)result);
}
#endif
vui128_t
test_shift_leftdo (vui128_t vrw, vui128_t vrx, vui128_t vrb)
{
	__vector unsigned char result, vt1, vt2, vt3;
	const __vector unsigned char vzero = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	vt1 = vec_slo ((__vector unsigned char)vrw, (__vector unsigned char)vrb);
	vt3 = vec_sub (vzero, (__vector unsigned char)vrb);
	vt2 = vec_sro ((__vector unsigned char)vrx, vt3);
	result = vec_or (vt1, vt2);

	return ((vui128_t)result);
}

vui128_t
test_shift_leftdq (vui128_t vrw, vui128_t vrx, vui128_t vrb)
{
	__vector unsigned char result, vt1, vt2, vt3;
	const __vector unsigned char vzero = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	vt1 = vec_slo ((__vector unsigned char)vrw, (__vector unsigned char)vrb);
	vt1 = vec_sll (vt1, (__vector unsigned char)vrb);
	vt3 = vec_sub (vzero, (__vector unsigned char)vrb);
	vt2 = vec_sro ((__vector unsigned char)vrx, vt3);
	vt2 = vec_srl (vt2, vt3);
	result = vec_or (vt1, vt2);

	return ((vui128_t)result);
}
