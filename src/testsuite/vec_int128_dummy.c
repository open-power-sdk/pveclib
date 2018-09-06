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

#include <vec_int128_ppc.h>

vui128_t
__test_msumudm (vui64_t a, vui64_t b, vui128_t c)
{
  return vec_msumudm ( a, b, c);
}

vui128_t
test_vec_add256_1 (vui128_t *out, vui128_t a0, vui128_t a1, vui128_t b0, vui128_t b1)
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
test_vec_add256_2 (vui128_t *out, vui128_t a0, vui128_t a1, vui128_t b0, vui128_t b1)
{
  vui128_t s0, s1, c0, c1;
  s1 = vec_addcq (&c1, a1, b1);
  s0 = vec_addeq (&c0, a0, b0, c1);

  out[1] = s1;
  out[0] = s0;
  return (c0);
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

vi128_t
test_vec_cmpeqsq (vi128_t a, vi128_t b)
{
  return (vec_cmpeqsq (a, b));
}

vi128_t
test_vec_cmpgesq (vi128_t a, vi128_t b)
{
  return (vec_cmpgesq (a, b));
}

vi128_t
test_vec_cmpgtsq (vi128_t a, vi128_t b)
{
  return (vec_cmpgtsq (a, b));
}

vi128_t
test_vec_cmplesq (vi128_t a, vi128_t b)
{
  return (vec_cmplesq (a, b));
}

vi128_t
test_vec_cmpltsq (vi128_t a, vi128_t b)
{
  return (vec_cmpltsq (a, b));
}

vui128_t
test_vec_cmpequq (vui128_t a, vui128_t b)
{
  return (vec_cmpequq (a, b));
}

vui128_t
test_vec_cmpgtuq (vui128_t a, vui128_t b)
{
  return (vec_cmpgtuq (a, b));
}

vui128_t
test_vec_cmpltuq (vui128_t a, vui128_t b)
{
  return (vec_cmpltuq (a, b));
}

vui128_t
test_vec_cmpgeuq (vui128_t a, vui128_t b)
{
  return (vec_cmpgeuq (a, b));
}

vui128_t
test_vec_cmpleuq (vui128_t a, vui128_t b)
{
  return (vec_cmpleuq (a, b));
}

vui128_t
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
  *p = vec_mul10uq (a);
  return vec_mul10uq (a);
}

vui128_t
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
test_vec_srq  (vui128_t a, vui128_t sh)
{
  return (vec_srq (a, sh));
}

vui128_t
test_vec_slq  (vui128_t a, vui128_t sh)
{
  return (vec_slq (a, sh));
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

vui128_t
__test_muludq (vui128_t *mulu, vui128_t a, vui128_t b)
{
  return (vec_muludq (mulu, a, b));
}

vui128_t
__test_mulluq (vui128_t a, vui128_t b)
{
  return (vec_mulluq (a, b));
}

vui128_t
__test_mulhuq (vui128_t a, vui128_t b)
{
  vui128_t mq, r;
  r = vec_muludq (&mq, a, b);
  return mq;
}

#ifdef _ARCH_PWR8
vui128_t
test_vec_subq (vui128_t a, vui128_t b)
{
  return (vec_sub(a,b));
}
#endif

void
test_mul4uq (vui128_t *__restrict__ mulu, vui128_t m1h, vui128_t m1l, vui128_t m2h, vui128_t m2l)
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
