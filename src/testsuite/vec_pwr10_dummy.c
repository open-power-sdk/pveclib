/*
 Copyright (c) [2020] IBM Corporation.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 vec_pwr10_dummy.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Nov. 24, 2020
 */

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
//#pragma GCC target ("cpu=power10")
#include <stdint.h>
#include <stdio.h>
#include <fenv.h>
#include <float.h>
#ifndef PVECLIB_DISABLE_F128MATH
/* Disable for __clang__ because of bug involving <math.h>
   incombination with -mcpu=power9 -mfloat128 */
#include <math.h>
#endif

#include <pveclib/vec_int128_ppc.h>
#include <pveclib/vec_int512_ppc.h>
#include <pveclib/vec_f128_ppc.h>
#include <pveclib/vec_f32_ppc.h>
#include <pveclib/vec_bcd_ppc.h>

int
test_vec_lsbb_all_ones_PWR10 (vui8_t vra)
{
  return vec_testlsbb_all_ones (vra);
}

int
test_vec_lsbb_all_zeros_PWR10 (vui8_t vra)
{
  return vec_testlsbb_all_zeros (vra);
}

int
test_lsbb_all_ones_PWR10 (vui8_t vra)
{
#if defined(_ARCH_PWR10) && (__GNUC__ > 9)
  int r;
  __asm__(
#if 0
      "xvtlsbb %0,%x2;\n"
      "setbc   %1,4*(%0)+lt;\n"
      : "=&y" (?), "=r" (r)
      : "wa" (vra)
      : "cc"
#else
      "xvtlsbb 6,%x1;\n"
      "setbc   %0,24;\n"
      : "=r" (r)
      : "wa" (vra)
      : "cr6"
#endif
	);
  return r;
#else
  const vui8_t ones = vec_splat_u8(1);
  vui8_t lsbb;

  lsbb = vec_and (vra, ones);
  return vec_all_eq (lsbb, ones);
#endif
}

int
test_lsbb_all_zeros_PWR10 (vui8_t vra)
{
#if defined(_ARCH_PWR10) && (__GNUC__ > 9)
  int r;
  __asm__(
#if 0
      "xvtlsbb %0,%x2;\n"
      "setbc   %1,4*(%0)+eq;\n"
      : "=&y" (cc), "=r" (r)
      : "wa" (vra)
      :
#else
      "xvtlsbb 6,%x1;\n"
      "setbc   %0,26;\n"
      : "=r" (r)
      : "wa" (vra)
      : "cr6"
#endif
	);
  return r;
#else
  const vui8_t ones = vec_splat_u8(1);
  vui8_t lsbb;

  lsbb = vec_and (vra, ones);
  return vec_all_ne (lsbb, ones);
#endif
}

#if defined(_ARCH_PWR10) && \
    ((__GNUC__ > 10) || (defined(__clang__) && (__clang_major__ > 12)))
int
test_intrn_test_lsbb_all_ones_PWR10 (vui8_t vra)
{
  return vec_test_lsbb_all_ones (vra);
}

int
test_intrn_test_lsbb_all_zeros_PWR10 (vui8_t vra)
{
  return vec_test_lsbb_all_zeros (vra);
}
#endif

vui128_t
test_diveuqo_PWR10 (vui128_t x, vui128_t z)
{
  return vec_diveuqo_inline (x, z);
}

vui128_t
test_vec_diveuqo_PWR10 (vui128_t x, vui128_t z)
{
  const vui128_t mone = (vui128_t) CONST_VINT128_DW(-1, -1);
  vui128_t y = {0};
  vui128_t Q, R;
  vui128_t Rt, r1, t;
  vb128_t CC;
  // Based on vec_divdqu with parm y = 0
  Q  = vec_vdiveuq_inline (x, z);
  // R = -(Q * z)
  r1 = vec_mulluq (Q, z);
  R  = vec_subuqm (y, r1);

  CC = vec_cmpgeuq (R, z);
  // Corrected Quotient before rounding.
  // if Q needs correction (Q+1), Bool CC is True, which is -1
  Q = vec_subuqm (Q, (vui128_t) CC);
  // Corrected Remainder
  Rt = vec_subuqm (R, z);
  R = vec_seluq (R, Rt, CC);
  // Convert nonzero remainder into a carry (=1).
  t = vec_addcuq (R, mone);
  Q = (vui128_t) vec_or ((vui32_t) Q, (vui32_t) t);
  return Q;
}

vui128_t
test_divdqu_rto_PWR10 (vui128_t x, vui128_t z)
{
  const vui128_t mone = (vui128_t) CONST_VINT128_DW(-1, -1);
  vui128_t y = {0};
  vui128_t Q, R;
  vui128_t Rt, t2;
  vui128_t r1, r2, q1;
  vb128_t CC, c2;
  // Based on the PowerISA, Programming Note for
  // Divide Word Extended [Unsigned] but vectorized
  // for vector __int128

  // Based on vec_divdqu with y = 0
  q1 = vec_vdiveuq_inline (x, z);
  // q2 = vec_vdivuq_inline  (y, z);
  r1 = vec_mulluq (q1, z);

  // r2 = vec_mulluq (q2, z) == 0;
  r2 = y; //vec_subuqm (y, r2);
  Q  = q1; //vec_adduqm (q1, q2);
  R  = vec_subuqm (r2, r1);

#if 0
  c1 = (vb128_t) y; //vec_cmpltuq (R, r2);
  c2 = vec_cmpgtuq (z, R);
  CC = (vb128_t) vec_orc ((vb32_t)c1, (vb32_t)c2);
#else
  c2 = vec_cmpgeuq (R, z);
  CC = c2; //(vb128_t) vec_or ((vb32_t)c1, (vb32_t)c2);
#endif
  // Corrected Quotient returned for divduq.
  // if Q needs correction (Q+1), Bool CC is True, which is -1
  Q = vec_subuqm (Q, (vui128_t) CC);
  //result.Q = Q;
// Corrected Remainder returned for modduq.
  Rt = vec_subuqm (R, z);
  R = vec_seluq (R, Rt, CC);
  // Convert nonzero remainder into a carry (=1).
  t2 = vec_addcuq (R, mone);
  Q = (vui128_t) vec_or ((vui32_t) Q, (vui32_t) t2);
  return Q;
}

vui128_t test_diveuq_PWR10 (vui128_t x, vui128_t z)
{
  return vec_vdiveuq_inline (x, z);
}

vui128_t test_divuq_PWR10 (vui128_t y, vui128_t z)
{
  return vec_vdivuq_inline (y, z);
}

vui128_t test_moduq_PWR10 (vui128_t y, vui128_t z)
{
  return vec_vmoduq_inline (y, z);
}

vui128_t test_divduq_PWR10 (vui128_t x, vui128_t y, vui128_t z)
{
  return vec_divduq (x, y, z);
}

vui128_t test_modduq_PWR10 (vui128_t x, vui128_t y, vui128_t z)
{
  return vec_modduq (x, y, z);
}

__VEC_U_128RQ test_divdqu_PWR10 (vui128_t x, vui128_t y, vui128_t z)
{
  return vec_divdqu_inline (x, y, z);
}

vui128_t
test_vec_mulqud_PWR10 (vui128_t *mulu, vui128_t a, vui128_t b)
{
  vui128_t l128, h128;
  vui64_t b_eud = vec_mrgahd ((vui128_t) b, (vui128_t)b);
  l128 = vec_vmuloud ((vui64_t ) a, b_eud);
  h128 = vec_vmaddeud ((vui64_t ) a, b_eud, (vui64_t ) l128);
  l128 = vec_slqi (l128, 64);

  *mulu = (vui128_t) h128;
  return ((vui128_t) l128);
}

// Attempts at better code to splat small DW constants.
// Want to avoid addr calc and loads for what should be simple
// splat immediate and unpack/extend.
vi64_t
__test_splatisd_12_PWR10 (void)
{
  return vec_splat_s64 (12);
}

// vec_splati from word requires GCC 12 and PWR10
#if defined (_ARCH_PWR10) && ((__GNUC__ > 11) \
    && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__))
vi64_t
__test_splatudi_12_PWR10 (void)
{
  vi32_t vwi = vec_splati (12);
  return vec_unpackl (vwi);
}
#endif

vui64_t
__test_splatudi_12_PWR10_v0 (void)
{
  return vec_splats ((unsigned long long) 12);
}

#if defined (_ARCH_PWR10) && (__GNUC__ > 11)
// New support defined in Power Vector Intrinsic Programming Reference.
int
test_gcc_cmpsq_all_gt_PWR10 (vi128_t a, vi128_t b)
{
  return vec_all_gt (a, b);
}

vb128_t
test_gcc_cmpsq_gt_PWR10 (vi128_t a, vi128_t b)
{
  return vec_cmpgt (a, b);
}
#endif

vi128_t
__test_vec_abssq_PWR10 (vi128_t vra)
{
  return vec_abssq (vra);
}


// Convert QP to Unsigned Integer QW
vui128_t
test_vec_xscvqpuqz_PWR10 (__binary128 f128)
{
  return vec_xscvqpuqz (f128);
}

// Convert Integer QW to QP
__binary128
test_vec_xscvuqqp_PWR10 (vui128_t int128)
{
  return vec_xscvuqqp (int128);
}

vb128_t
test_vec_cmpeqsq_PWR10 (vi128_t a, vi128_t b)
{
  return (vec_cmpeqsq (a, b));
}

vb128_t
test_vec_cmpgesq_PWR10 (vi128_t a, vi128_t b)
{
  return (vec_cmpgesq (a, b));
}

vb128_t
test_vec_cmpgtsq_PWR10 (vi128_t a, vi128_t b)
{
  return (vec_cmpgtsq (a, b));
}

vb128_t
test_vec_cmpltsq_PWR10 (vi128_t a, vi128_t b)
{
  return (vec_cmpltsq (a, b));
}

vb128_t
test_vec_cmpequq_PWR10 (vui128_t a, vui128_t b)
{
  return (vec_cmpequq (a, b));
}

vb128_t
test_vec_cmpgtuq_PWR10 (vui128_t a, vui128_t b)
{
  return (vec_cmpgtuq (a, b));
}

vb128_t
test_vec_cmpgeuq_PWR10 (vui128_t a, vui128_t b)
{
  return (vec_cmpgeuq (a, b));
}

vb128_t
test_vec_cmpltuq_PWR10 (vui128_t a, vui128_t b)
{
  return (vec_cmpltuq (a, b));
}

vb128_t
test_vec_cmpneuq_PWR10 (vui128_t a, vui128_t b)
{
  return (vec_cmpneuq (a, b));
}

vi128_t
test_min_ui128_PWR10  (vi128_t a, vi128_t b)
{
  if (vec_cmpsq_all_lt (a, b))
    return a;
  else
    return b;
}

vi128_t
test_max_ui128_PWR10  (vi128_t a, vi128_t b)
{
  if (vec_cmpsq_all_gt (a, b))
    return a;
  else
    return b;
}

int
test_cmpsq_all_lt_PWR10 (vi128_t a, vi128_t b)
{
  return vec_cmpsq_all_lt (a, b);
}

int
test_cmpsq_all_gt_PWR10 (vi128_t a, vi128_t b)
{
  return vec_cmpsq_all_gt (a, b);
}

vb128_t
test_vec_cmpnetoqp_PWR10 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpnetoqp (vfa, vfb);
}

vb128_t
test_vec_cmpneuzqp_PWR10 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpneuzqp (vfa, vfb);
}

vb128_t
test_vec_cmpneuqp_PWR10 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpneuqp (vfa, vfb);
}

vb128_t
test_vec_cmpletoqp_PWR10 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpletoqp (vfa, vfb);
}

vb128_t
test_vec_cmpleuzqp_PWR10 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpleuzqp (vfa, vfb);
}

vb128_t
test_vec_cmpleuqp_PWR10 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpleuqp (vfa, vfb);
}

vb128_t
test_vec_cmpgetoqp_PWR10 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpgetoqp (vfa, vfb);
}

vb128_t
test_vec_cmpgeuzqp_PWR10 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpgeuzqp (vfa, vfb);
}

vb128_t
test_vec_cmpgeuqp_PWR10 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpgeuqp (vfa, vfb);
}

vb128_t
test_vec_cmplttoqp_PWR10 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmplttoqp (vfa, vfb);
}

vb128_t
test_vec_cmpltuzqp_PWR10 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpltuzqp (vfa, vfb);
}

vb128_t
test_vec_cmpltuqp_PWR10 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpltuqp (vfa, vfb);
}

vb128_t
test_vec_cmpgttoqp_PWR10 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpgttoqp (vfa, vfb);
}

vb128_t
test_vec_cmpgtuzqp_PWR10 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpgtuzqp (vfa, vfb);
}

vb128_t
test_vec_cmpgtuqp_PWR10 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpgtuqp (vfa, vfb);
}

vb128_t
test_vec_cmpeqtoqp_PWR10 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpeqtoqp (vfa, vfb);
}

vb128_t
test_vec_cmpequzqp_PWR10 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpequzqp (vfa, vfb);
}

vb128_t
test_vec_cmpequqp_PWR10 (__binary128 vfa, __binary128 vfb)
{
  return vec_cmpequqp (vfa, vfb);
}

vb128_t
test_setb_qp_PWR10 (__binary128 f128)
{
  return vec_setb_qp (f128);
}

vb8_t
test_setb_sb_PWR10 (vi8_t vra)
{
  return vec_setb_sb (vra);
}

vb16_t
test_setb_sh_PWR10 (vi16_t vra)
{
  return vec_setb_sh (vra);
}

vb32_t
test_setb_sw_PWR10 (vi32_t vra)
{
  return vec_setb_sw (vra);
}

vb64_t
test_setb_sd_PWR10 (vi64_t vra)
{
  return vec_setb_sd (vra);
}

vb128_t
test_setb_sq_PWR10 (vi128_t vcy)
{
  return vec_setb_sq (vcy);
}

vui128_t
test_vec_sldqi_7_PWR10 (vui128_t a, vui128_t b)
{
  return (vec_sldqi (a, b, 7));
}

vui128_t
test_vec_sldqi_15_PWR10 (vui128_t a, vui128_t b)
{
  return (vec_sldqi (a, b, 15));
}

vui128_t
test_vec_sldqi_16_PWR10 (vui128_t a, vui128_t b)
{
  return (vec_sldqi (a, b, 16));
}

vui128_t
test_vec_sldqi_127_PWR10 (vui128_t a, vui128_t b)
{
  return (vec_sldqi (a, b, 127));
}

vui128_t
__test_msumcud_PWR10 (vui64_t a, vui64_t b, vui128_t c)
{
  return vec_msumcud ( a, b, c);
}

vui128_t
__test_cmsumudm_PWR10 (vui128_t * carry, vui64_t a, vui64_t b, vui128_t c)
{
  *carry = vec_msumcud ( a, b, c);
  return vec_msumudm ( a, b, c);
}

vui128_t
__test_cmsumudm_V2_PWR10 (vui128_t * carry, vui64_t a, vui64_t b, vui128_t c)
{
  *carry = vec_msumcud ( a, b, c);
  return vec_msumudm ( a, b, c);
}

vui128_t
test_vec_srdbi_PWR10  (vui128_t a, vui128_t b, const unsigned int  sh)
{
  return (vec_vsrdbi (a, b, sh));
}

vui128_t
test_vec_srdbi_PWR10_0  (vui128_t a, vui128_t b)
{
  return (vec_vsrdbi (a, b, 0));
}

vui128_t
test_vec_srdbi_PWR10_7  (vui128_t a, vui128_t b)
{
  return (vec_vsrdbi (a, b, 7));
}

vui128_t
test_vec_sldbi_PWR10  (vui128_t a, vui128_t b, const unsigned int  sh)
{
  return (vec_vsldbi (a, b, sh));
}

vui128_t
test_vec_sldbi_PWR10_0  (vui128_t a, vui128_t b)
{
  return (vec_vsldbi (a, b, 0));
}

vui128_t
test_vec_sldbi_PWR10_7  (vui128_t a, vui128_t b)
{
  return (vec_vsldbi (a, b, 7));
}

vui128_t
test_vec_sldbi_PWR10_8  (vui128_t a, vui128_t b)
{
  return (vec_vsldbi (a, b, 8));
}

vui128_t
test_vec_rlqi_PWR10  (vui128_t a, const unsigned int  sh)
{
  return (vec_rlqi (a, sh));
}

vui128_t
test_vec_rlqi_7_PWR10  (vui128_t a)
{
  return (vec_rlqi (a, 7));
}

vui128_t
test_vec_rlqi_15_PWR10  (vui128_t a)
{
  return (vec_rlqi (a, 15));
}

vui128_t
test_vec_slqi_PWR10  (vui128_t a, const unsigned int  sh)
{
  return (vec_slqi (a, sh));
}

vui128_t
test_vec_slqi_7_PWR10  (vui128_t a)
{
  return (vec_slqi (a, 7));
}

vui128_t
test_vec_slqi_8_PWR10  (vui128_t a)
{
  return (vec_slqi (a, 8));
}

vi128_t
test_vec_sraqi_PWR10  (vi128_t a, const unsigned int  sh)
{
  return (vec_sraqi (a, sh));
}

vi128_t
test_vec_sraqi_7_PWR10  (vi128_t a)
{
  return (vec_sraqi (a, 7));
}

vui128_t
test_vec_srqi_PWR10  (vui128_t a, const unsigned int  sh)
{
  return (vec_srqi (a, sh));
}

vui128_t
test_vec_srqi_7_PWR10  (vui128_t a)
{
  return (vec_srqi (a, 7));
}

vui128_t
test_vec_rlq_PWR10  (vui128_t a, vui128_t sh)
{
  return (vec_rlq (a, sh));
}

vui128_t
test_vec_slq_PWR10  (vui128_t a, vui128_t sh)
{
  return (vec_slq (a, sh));
}

vi128_t
test_vec_sraq_PWR10  (vi128_t a, vui128_t sh)
{
  return (vec_sraq (a, sh));
}

vui128_t
test_vec_srq_PWR10  (vui128_t a, vui128_t sh)
{
  return (vec_srq (a, sh));
}

vui128_t
__test_muloud_PWR10 (vui64_t a, vui64_t b)
{
  return vec_muloud (a, b);
}

vui128_t
__test_muleud_PWR10 (vui64_t a, vui64_t b)
{
  return vec_muleud (a, b);
}

vui64_t
__test_mulhud_PWR10 (vui64_t a, vui64_t b)
{
  return vec_mulhud (a, b);
}

vui64_t
__test_muludm_PWR10 (vui64_t a, vui64_t b)
{
  return vec_muludm (a, b);
}

vui128_t
__test_vmuloud_PWR10 (vui64_t a, vui64_t b)
{
  return vec_vmuloud (a, b);
}

vui128_t
__test_vmuleud_PWR10 (vui64_t a, vui64_t b)
{
  return vec_vmuleud (a, b);
}

vui128_t
__test_vmsumoud_PWR10 (vui64_t a, vui64_t b, vui128_t c)
{
  return vec_vmsumoud (a, b, c);
}

vui128_t
__test_vmsumeud_PWR10 (vui64_t a, vui64_t b, vui128_t c)
{
  return vec_vmsumeud (a, b, c);
}

vui128_t
__test_vmaddoud_PWR10 (vui64_t a, vui64_t b, vui64_t c)
{
  return vec_vmaddoud (a, b, c);
}

vui128_t
__test_vmaddeud_PWR10 (vui64_t a, vui64_t b, vui64_t c)
{
  return vec_vmaddeud (a, b, c);
}

vui128_t
__test_vmadd2oud_PWR10 (vui64_t a, vui64_t b, vui64_t c, vui64_t d)
{
  return vec_vmadd2oud (a, b, c, d);
}

vui128_t
__test_mulhuq_PWR10 (vui128_t a, vui128_t b)
{
  return vec_mulhuq (a, b);
}

vui128_t
__test_mulluq_PWR10 (vui128_t a, vui128_t b)
{
  return vec_mulluq (a, b);
}

vui128_t
__test_mulluq_V1_PWR10 (vui128_t a, vui128_t b)
{
  vui64_t t, tmq;
  /* compute the 256 bit product of two 128 bit values a, b.
   * The high 128 bits are accumulated in t and the low 128-bits
   * in tmq.  Only the low order 128 bits of the product are
   * returned.
   */
#ifdef _ARCH_PWR9
  const vui64_t zero = { 0, 0 };
  vui64_t b_swap = vec_swapd ((vui64_t) b);
  /* multiply the low 64-bits of a and b.  For PWR9 this is just
   * vmsumudm with conditioned inputs.  */
  tmq = (vui64_t) vec_vmuloud ((vui64_t) a, (vui64_t) b);
  /* we can use multiply sum here because we only need the low 64-bits
   * and don't care if we lose the carry / overflow.  */
  /* sum = (a[h] * b[l]) + (a[l] * b[h])) + zero).  */
  t   = (vui64_t) vec_msumudm ((vui64_t) a, b_swap, (vui128_t) zero);
  /* result = sum ({tmq[h] + t[l]} , {tmq[l] + zero}).  */
  /* Shift t left 64-bits and use doubleword add. */
  t   = (vui64_t) vec_mrgald ((vui128_t) t, (vui128_t) zero);
  tmq = (vui64_t) vec_addudm ((vui64_t) t, (vui64_t) tmq);
#else
#endif
  return ((vui128_t) tmq);
}

vui128_t
__test_muludq_PWR10 (vui128_t *mulh, vui128_t a, vui128_t b)
{
  return vec_muludq (mulh, a, b);
}

vui128_t
__test_muludq_V1_PWR10 (vui128_t *mulu, vui128_t a, vui128_t b)
{
  vui32_t t, tmq;
  /* compute the 256 bit product of two 128 bit values a, b.
   * The high 128 bits are accumulated in t and the low 128-bits
   * in tmq. The high 128-bits of the product are returned to the
   * address of the 1st parm. The low 128-bits are the return
   * value.
   */
#ifdef _ARCH_PWR10
  const vui64_t zero = { 0, 0 };
  vui64_t a_swap = vec_swapd ((vui64_t) a);
  vui128_t thq, tlq, tx;
  vui128_t txl, txh, tc1;
  vui128_t thh, tll;
  /* multiply the high and low 64-bits of a and b.  */
  tll = vec_vmuloud ((vui64_t)a, (vui64_t)b);
  thh = vec_vmuleud ((vui64_t)a, (vui64_t)b);
  /* multiply and sum the middle products with carry-out */
  tx  = vec_vmsumudm_inline  ((vui64_t)a_swap, (vui64_t)b, (vui128_t)zero);
  tc1 = vec_vmsumcud_inline  ((vui64_t)a_swap, (vui64_t)b, (vui128_t)zero);
  /* Align the middle product and carry-out for double quadword sum */
#if 0
  txl = (vui128_t) vec_mrgald ( tx,  (vui128_t) zero);
#else
  txl = vec_sldqi ( tx,  tc1, 64);
#endif
  txh = vec_sldqi ( tc1, tx,  64);
  /* Double quadword sum for 256-bit product */
  tc1 = vec_addcuq (tll, txl);
  tlq  = vec_adduqm (tll, txl);
  thq  = vec_addeuqm (thh, txh, tc1);

  t = (vui32_t) thq;
  tmq = (vui32_t) tlq;
#else
  const vui64_t zero = { 0, 0 };
  vui64_t a_swap = vec_swapd ((vui64_t) a);
  vui128_t thq, tlq, tx;
  vui128_t t0l, tc1;
  vui128_t thh, thl, tlh, tll;
  /* multiply the low 64-bits of a and b.  For PWR9 this is just
   * vmsumudm with conditioned inputs.  */
  tll = vec_vmuloud ((vui64_t)a, (vui64_t)b);
  thh = vec_vmuleud ((vui64_t)a, (vui64_t)b);
  thl = vec_vmuloud (a_swap, (vui64_t)b);
#if 0
  tlh = vec_vmuleud (a_swap, (vui64_t)b);
  /* sum the two middle products (plus the high 64-bits of the low
   * product.  This will generate a carry that we need to capture.  */
  t0l   = (vui128_t) vec_mrgahd ( (vui128_t) zero, tll);
#else
  tlh = vec_vmaddeud (a_swap, (vui64_t)b, (vui64_t)tll);
#endif
  tc1 = vec_addcuq (thl, tlh);
  tx   = vec_adduqm (thl, tlh);
#if 0
  tx   = vec_adduqm (tx, t0l);
#endif
  /* result = t[l] || tll[l].  */
  tlq = (vui128_t) vec_mrgald ((vui128_t) tx, (vui128_t) tll);
  /* Sum the high product plus the high sum (with carry) of middle
   * partial products.  This can't overflow.  */
  thq = (vui128_t) vec_permdi ((vui64_t) tc1, (vui64_t) tx, 2);
  thq = vec_adduqm ( thh, thq);

  t = (vui32_t) thq;
  tmq = (vui32_t) tlq;
#endif
  *mulu = (vui128_t) t;
  return ((vui128_t) tmq);
}

vui128_t
test_vec_rlqi_128_PWR10  (vui128_t a)
{
  return (vec_rlqi (a, 128));
}

vui128_t
test_vec_slqi_128_PWR10  (vui128_t a)
{
  return (vec_slqi (a, 128));
}

vi128_t
test_vec_sraqi_128_PWR10  (vi128_t a)
{
  return (vec_sraqi (a, 128));
}

vui128_t
test_vec_srqi_128_PWR10  (vui128_t a)
{
  return (vec_srqi (a, 128));
}

vui128_t
test_vec_rlqi_127_PWR10  (vui128_t a)
{
  return (vec_rlqi (a, 127));
}

vui128_t
test_vec_slqi_127_PWR10  (vui128_t a)
{
  return (vec_slqi (a, 127));
}

vi128_t
test_vec_sraqi_127_PWR10  (vi128_t a)
{
  return (vec_sraqi (a, 127));
}

vui128_t
test_vec_srqi_127_PWR10  (vui128_t a)
{
  return (vec_srqi (a, 127));
}

vui128_t
test_vec_rlqi_64_PWR10  (vui128_t a)
{
  return (vec_rlqi (a, 64));
}

vui128_t
test_vec_slqi_64_PWR10  (vui128_t a)
{
  return (vec_slqi (a, 64));
}

vi128_t
test_vec_sraqi_64_PWR10  (vi128_t a)
{
  return (vec_sraqi (a, 64));
}

vui128_t
test_vec_srqi_64_PWR10  (vui128_t a)
{
  return (vec_srqi (a, 64));
}

vui128_t
test_vec_slqi_15_PWR10  (vui128_t a)
{
  return (vec_slqi (a, 15));
}

vi128_t
test_vec_sraqi_15_PWR10  (vi128_t a)
{
  return (vec_sraqi (a, 15));
}

vui128_t
test_vec_srqi_15_PWR10  (vui128_t a)
{
  return (vec_srqi (a, 15));
}

vui128_t
test_vec_rlqi_0_PWR10  (vui128_t a)
{
  return (vec_rlqi (a, 0));
}

vui128_t
test_vec_slqi_0_PWR10  (vui128_t a)
{
  return (vec_slqi (a, 0));
}

vi128_t
test_vec_sraqi_0_PWR10  (vi128_t a)
{
  return (vec_sraqi (a, 0));
}

vui128_t
test_vec_srqi_0_PWR10  (vui128_t a)
{
  return (vec_srqi (a, 0));
}

vui64_t test_divmodud_PWR10 (vui64_t *r, vui64_t y, vui64_t z)
{
  *r = vec_vmodud_inline (y, z);
  return vec_vdivud_inline (y, z);
}

vui64_t
test_divqud_PWR10 (vui128_t x_y, vui64_t z)
{
  return vec_divqud_inline (x_y, z);
}

vui64_t test_vec_divqud_PWR10 (vui128_t x_y, vui64_t z)
{
#if defined (_ARCH_PWR10) && (__GNUC__ >= 12)
  // Circular dependency between int64 and int128, thinking
  vui128_t Dv, Q, R, t;

#if 1
  // Use compiler intrinsics to avoid dependency on vec_int128-ppc.h
  const vui64_t zero = { 0, 0 };
  Dv = (vui128_t) vec_mrgahd ((vui128_t) zero, (vui128_t) z);
  Q  = vec_div (x_y, Dv);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t  = vec_mule ((vui64_t)Q, (vui64_t) Dv);
#else
  t  = vec_mulo ((vui64_t)Q, (vui64_t) Dv);
#endif
  R = vec_sub (x_y, t);
#else
  // This would require vec_int128_ppc.h to be completely compiled
  // before vec_int64_ppc.h. Nope!
  Dv = vec_srqi ((vui128_t) z, 64);
  Q  = vec_vdivuq_inline (x_y, Dv);
  t  = vec_vmuloud ((vui64_t)Q, (vui64_t) Dv);
  R  = vec_subuqm (x_y, t);
#endif
  return vec_mrgald (R, Q);
#else //  defined (_ARCH_PWR7)
  // POWER8/9 Do not have vector integer divide, but do have
  // Move To/From Vector-Scalar Register Instructions
  // So we can use the scalar hardware divide/divide extended
  __VEC_U_128 qu, xy, zu;
#if (__GNUC__ <= 10)
  xy.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) x_y, 1);
  xy.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) x_y, 0);
  zu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) z, 1);
  zu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) z, 0);
#else
  // Looks like AT16 handles this but what about 15/14 ...
  // AT10 does not.
  xy.vx1 = x_y;
  zu.vx2 = z;
#endif
  unsigned long long Dh = xy.ulong.upper;
  unsigned long long Dl = xy.ulong.lower;
  unsigned long long Dv = zu.ulong.upper;
  unsigned long long q1, q2, Q;
  unsigned long long r1, r2, R;

  // Transfer to GPUs and use scalar divide/divide extended
  // Based on the PowerISA, Programming Note for
  // Divide Word Extended [Unsigned]
  q1 = __builtin_divdeu (Dh, Dv);
  //r1 = -(q1 * Dv);
  r1 = (q1 * Dv);
  q2 = Dl / Dv;
  r2 = Dl - (q2 * Dv);
  Q = q1 + q2;
  //R = r1 + r2;
  R = r2 - r1;
  if ((R < r2) | (R >= Dv))
    {
      Q++;
      R = R - Dv;
    }

  // Transfer R|Q back to VRs and return
  qu.ulong.upper = R;
  qu.ulong.lower = Q;
  return qu.vx2;
#endif
}

vui64_t test_vec_divud_PWR10 (vui64_t y, vui64_t z)
{
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  vui64_t res;
#if (__GNUC__ >= 13)
  res = vec_div (y, z);
#else
  __asm__(
      "vdivud %0,%1,%2;\n"
      : "=v" (res)
      : "v" (y), "v" (z)
      : );
#endif
  return res;
#elif defined (_ARCH_PWR8)
  __VEC_U_128 qu, yu, zu;
#if 1
  yu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) y, 1);
  yu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) y, 0);
  zu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) z, 1);
  zu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) z, 0);
#else
  yu.vx2 = y;
  zu.vx2 = z;
#endif

  qu.ulong.lower = yu.ulong.lower / zu.ulong.lower;
  qu.ulong.upper = yu.ulong.upper / zu.ulong.upper;

  return qu.vx2;
#endif
}

vui64_t test_vec_divude_PWR10 (vui64_t x, vui64_t z)
{
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  vui64_t res;
#if (__GNUC__ >= 13)
  res = vec_dive (x, z);
#else
  __asm__(
      "vdiveud %0,%1,%2;\n"
      : "=v" (res)
      : "v" (x), "v" (z)
      : );
#endif
  return res;
#elif defined (_ARCH_PWR8)
  __VEC_U_128 qu, yu, zu;
#if (__GNUC__ <= 10)
  yu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) x, 1);
  yu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) x, 0);
  zu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) z, 1);
  zu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) z, 0);
#else
  // Looks like AT16 handles this but what about 15/14 ...
  // AT10 does not.
  yu.vx2 = x;
  zu.vx2 = z;
#endif

  qu.ulong.lower = __builtin_divdeu (yu.ulong.lower, zu.ulong.lower);
  qu.ulong.upper = __builtin_divdeu (yu.ulong.upper, zu.ulong.upper);

  return qu.vx2;
#endif
}

vui64_t test_vec_modud_PWR10 (vui64_t y, vui64_t z)
{
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  vui64_t res;
#if (__GNUC__ >= 13)
  res = vec_mod (y, z);
#else
  __asm__(
      "vmodud %0,%1,%2;\n"
      : "=v" (res)
      : "v" (y), "v" (z)
      : );
#endif
  return res;
#elif defined (_ARCH_PWR8)
  __VEC_U_128 qu, yu, zu;
#if (__GNUC__ <= 10)
  yu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) y, 1);
  yu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) y, 0);
  zu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) z, 1);
  zu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) z, 0);
#else
  yu.vx2 = y;
  zu.vx2 = z;
#endif

  qu.ulong.lower = yu.ulong.lower % zu.ulong.lower;
  qu.ulong.upper = yu.ulong.upper % zu.ulong.upper;

  return qu.vx2;
#endif
}

vui64_t test_vec_divdud_PWR10 (vui64_t x, vui64_t y, vui64_t z)
{
  vui64_t Q, R, Qt /*, Rt*/;
  vui64_t r1, r2, q1, q2;
  vb64_t CC, c1, c2;
  const vui64_t ones = vec_splat_u64(1);

  q1 = test_vec_divude_PWR10 (x, z);
  r1 = vec_muludm (q1, z);
  q2 = test_vec_divud_PWR10 (y, z);

  r2 = vec_muludm (q2, z);
  r2 = vec_subudm (y, r2);
  Q  = vec_addudm (q1, q2);
  R  = vec_subudm (r2, r1);

  c1 = vec_cmpltud (R, r2);
  c2 = vec_cmpgeud (R, z);
  CC = vec_or (c1, c2);

  Qt = vec_addudm (Q, ones);
  Q = vec_selud (Q, Qt, CC);
#if 0 // Corrected Remainder not returned
  Rt = vec_subudm (R, z);
  R = vec_selud (R, Rt, CC);
#endif
  return Q;
}

__VEC_U_128RQ test_vec_moddivduq_PWR10 (vui128_t x, vui128_t y, vui128_t z)
{
  __VEC_U_128RQ result;
#if defined (_ARCH_PWR8)
  vui128_t Q, R;
  vui128_t r1, r2, q1, q2;
  vb128_t CC, c1, c2;
  const vui128_t ones = {(__int128) 1};

  // Based on the PowerISA, Programming Note for
  // Divide Word Extended [Unsigned] but vectorized
  // for vector __int128
  q1 = vec_vdiveuq_inline (x, z);
  q2 = vec_vdivuq_inline  (y, z);
  r1 = vec_mulluq (q1, z);

  r2 = vec_mulluq (q2, z);
  r2 = vec_subuqm (y, r2);
  Q  = vec_adduqm (q1, q2);
  R  = vec_subuqm (r2, r1);

  c1 = vec_cmpltuq (R, r2);
#if defined (_ARCH_PWR8) // vorc requires P8
  c2 = vec_cmpgtuq (z, R);
  CC = (vb128_t) vec_orc ((vb32_t)c1, (vb32_t)c2);
#else
  c2 = vec_cmpgeuq (R, z);
  CC = (vb128_t) vec_or ((vb32_t)c1, (vb32_t)c2);
#endif
// Corrected Quotient returned for divduq.
  vui128_t Qt;
  Qt = vec_adduqm (Q, ones);
  Q = vec_seluq (Q, Qt, CC);
  result.Q = Q;
// Corrected Remainder not returned for divduq.
  vui128_t Rt;
  Rt = vec_subuqm (R, z);
  R = vec_seluq (R, Rt, CC);
  result.R = R;
#else
  /* Based on Hacker's Delight (2nd Edition) Figure 9-2.
   * "Divide long unsigned shift-and-subtract algorithm."
   * Converted to use vector unsigned __int128 and PVEClIB
   * operations.
   * As cmpgeuq is based on detecting the carry-out of (x -z) and
   * setting the bool via setb_cyq, we can use this carry (variable t)
   * to generate quotient bits.
   * Multi-precision shift-left is simpler then general addition,
   * so we can simplify carry generation. This allows delaying the
   * the y left-shift / quotient accumulation to a later.
   * */
  int i;
  vb128_t ge;
  vui128_t t, cc, c, xt;
  //t = (vui128_t) CONST_VINT128_W (0, 0, 0, 0);

  for (i = 1; i <= 128; i++)
    {
      // Left shift (x || y) requires 257-bits, is (t || x || y)
      // t from previous iteration generates to 0/1 for low order y-bits.
      c = vec_addcuq (y, y);
      t = vec_addcuq (x, x);
      x = vec_addeuqm (x, x, c);

      // deconstruct ((t || x) >= z)
      // Carry from subcuq == 1 for x >= z
      cc = vec_subcuq (x, z);
      // Combine t with (x >= z) for 129-bit compare
      t  = (vui128_t) vec_or ((vui32_t)cc, (vui32_t)t);
      // Convert to a bool for select
      ge = vec_setb_cyq (t);

      xt = vec_subuqm (x, z);
      y = vec_addeuqm (y, y, t);
      x = vec_seluq (x, xt, ge);
    }
  result.vx0 = y; // Q
  result.vx1 = x; // R
#endif
  return result;
}
#endif
