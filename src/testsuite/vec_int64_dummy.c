/*
 Copyright (c) [2018] IBM Corporation.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 vec_int64_dummy.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Mar 29, 2018
 */

#include <pveclib/vec_int128_ppc.h>

vui64_t
test_ctzd_v1 (vui64_t vra)
{
  const vui64_t ones = { -1, -1 };
  const vui64_t c64s = { 64, 64 };
  vui64_t term;
  // term = (!vra & (vra - 1))
  term = vec_andc (vec_addudm (vra, ones), vra);
  // return = 64 - vec_clz (!vra & (vra - 1))
  return (c64s - vec_clzd (term));
}

vui64_t
test_ctzd_v2 (vui64_t vra)
{
  const vui64_t ones = { -1, -1 };
  vui64_t term;
  // term = (!vra & (vra - 1))
  term = vec_andc (vec_addudm (vra, ones), vra);
  // return = vec_popcnt (!vra & (vra - 1))
  return (vec_popcntd (term));
}

vui64_t
test_ctzd_v3 (vui64_t vra)
{
  const vui64_t zeros = { 0, 0 };
  const vui64_t c64s = { 64, 64 };
  vui64_t term;
  // term = (vra | -vra))
  term = vec_or (vra, vec_subudm (zeros, vra));
  // return = 64 - vec_poptcnt (vra & -vra)
  return (c64s - vec_popcntd (term));
}

vui64_t
test_vec_ctzd (vui64_t vra)
{
  return (vec_ctzd (vra));
}

vui64_t
__test_vrld (vui64_t a, vui64_t b)
{
  return vec_vrld (a, b);
}

vui64_t
__test_vsld (vui64_t a, vui64_t b)
{
  return vec_vsld (a, b);
}

vui64_t
__test_vsrd (vui64_t a, vui64_t b)
{
  return vec_vsrd (a, b);
}

vi64_t
__test_vsrad (vi64_t a, vui64_t b)
{
  return vec_vsrad (a, b);
}

vui64_t
test_rldi_1 (vui64_t a)
{
  return vec_rldi (a, 1);
}

vui64_t
test_rldi_15 (vui64_t a)
{
  return vec_rldi (a, 15);
}

vui64_t
test_rldi_32 (vui64_t a)
{
  return vec_rldi (a, 32);
}

vui64_t
test_sldi_1 (vui64_t a)
{
  return vec_sldi (a, 1);
}

vui64_t
test_sldi_15 (vui64_t a)
{
  return vec_sldi (a, 15);
}

vui64_t
test_sldi_16 (vui64_t a)
{
  return vec_sldi (a, 16);
}

vui64_t
test_sldi_63 (vui64_t a)
{
  return vec_sldi (a, 63);
}

vui64_t
test_sldi_64 (vui64_t a)
{
  return vec_sldi (a, 64);
}

vui64_t
test_srdi_1 (vui64_t a)
{
  return vec_srdi (a, 1);
}

vui64_t
test_srdi_15 (vui64_t a)
{
  return vec_srdi (a, 15);
}

vui64_t
test_srdi_16 (vui64_t a)
{
  return vec_srdi (a, 16);
}

vui64_t
test_srdi_31 (vui64_t a)
{
  return vec_srdi (a, 31);
}

vui64_t
test_srdi_32 (vui64_t a)
{
  return vec_srdi (a, 32);
}

vui64_t
test_srdi_63 (vui64_t a)
{
  return vec_srdi (a, 63);
}

vui64_t
test_srdi_64 (vui64_t a)
{
  return vec_srdi (a, 64);
}

vi64_t
test_sradi_1 (vi64_t a)
{
  return vec_sradi (a, 1);
}

vi64_t
test_sradi_15 (vi64_t a)
{
  return vec_sradi (a, 15);
}

vi64_t
test_sradi_16 (vi64_t a)
{
  return vec_sradi (a, 16);
}

vi64_t
test_sradi_63 (vi64_t a)
{
  return vec_sradi (a, 63);
}

vi64_t
test_sradi_64 (vi64_t a)
{
  return vec_sradi (a, 64);
}

int
test_cmpud_all_eq (vui64_t a, vui64_t b)
{
  return vec_cmpud_all_eq (a, b);
}

int
test_cmpud_all_ge (vui64_t a, vui64_t b)
{
  return vec_cmpud_all_ge (a, b);
}

int
test_cmpud_all_gt (vui64_t a, vui64_t b)
{
  return vec_cmpud_all_gt (a, b);
}

int
test_cmpud_all_le (vui64_t a, vui64_t b)
{
  return vec_cmpud_all_le (a, b);
}

int
test_cmpud_all_lt (vui64_t a, vui64_t b)
{
  return vec_cmpud_all_lt (a, b);
}

int
test_cmpud_all_ne (vui64_t a, vui64_t b)
{
  return vec_cmpud_all_ne (a, b);
}

int
test_cmpud_any_eq (vui64_t a, vui64_t b)
{
  return vec_cmpud_any_eq (a, b);
}

int
test_cmpud_any_ge (vui64_t a, vui64_t b)
{
  return vec_cmpud_any_ge (a, b);
}

int
test_cmpud_any_gt (vui64_t a, vui64_t b)
{
  return vec_cmpud_any_gt (a, b);
}

int
test_cmpud_any_le (vui64_t a, vui64_t b)
{
  return vec_cmpud_any_le (a, b);
}

int
test_cmpud_any_lt (vui64_t a, vui64_t b)
{
  return vec_cmpud_any_lt (a, b);
}

int
test_cmpud_any_ne (vui64_t a, vui64_t b)
{
  return vec_cmpud_any_ne (a, b);
}

int
test_cmpsd_all_eq (vi64_t a, vi64_t b)
{
  return vec_cmpsd_all_eq (a, b);
}

int
test_cmpsd_all_ge (vi64_t a, vi64_t b)
{
  return vec_cmpsd_all_ge (a, b);
}

int
test_cmpsd_all_gt (vi64_t a, vi64_t b)
{
  return vec_cmpsd_all_gt (a, b);
}

int
test_cmpsd_all_le (vi64_t a, vi64_t b)
{
  return vec_cmpsd_all_le (a, b);
}

int
test_cmpsd_all_lt (vi64_t a, vi64_t b)
{
  return vec_cmpsd_all_lt (a, b);
}

int
test_cmpsd_all_ne (vi64_t a, vi64_t b)
{
  return vec_cmpsd_all_ne (a, b);
}

int
test_cmpsd_any_eq (vi64_t a, vi64_t b)
{
  return vec_cmpsd_any_eq (a, b);
}

int
test_cmpsd_any_ge (vi64_t a, vi64_t b)
{
  return vec_cmpsd_any_ge (a, b);
}

int
test_cmpsd_any_gt (vi64_t a, vi64_t b)
{
  return vec_cmpsd_any_gt (a, b);
}

int
test_cmpsd_any_le (vi64_t a, vi64_t b)
{
  return vec_cmpsd_any_le (a, b);
}

int
test_cmpsd_any_lt (vi64_t a, vi64_t b)
{
  return vec_cmpsd_any_lt (a, b);
}

int
test_cmpsd_any_ne (vi64_t a, vi64_t b)
{
  return vec_cmpsd_any_ne (a, b);
}

vui64_t
test_cmpud_all_gt2 (vui64_t a, vui64_t b, vui64_t c, vui64_t d)
{
  if (vec_cmpud_all_gt (a, b))
    return c;
  else
    return d;
}

vui64_t
test_cmpud_all_eq2 (vui64_t a, vui64_t b, vui64_t c, vui64_t d)
{
  if (vec_cmpud_all_eq (a, b))
    return c;
  else
    return d;
}

vui64_t
test_subudm (vui64_t a, vui64_t b)
{
  return vec_subudm (a, b);
}

vui64_t
test_addudm (vui64_t a, vui64_t b)
{
  return vec_addudm (a, b);
}

vui64_t
test_absdud (vui64_t a, vui64_t b)
{
  return vec_absdud (a, b);
}

vb64_t
test_cmpequd (vui64_t a, vui64_t b)
{
  return vec_cmpequd (a, b);
}

vb64_t
test_cmpneud (vui64_t a, vui64_t b)
{
  return vec_cmpneud (a, b);
}

vb64_t
test_cmpgtud (vui64_t a, vui64_t b)
{
  return vec_cmpgtud (a, b);
}

vb64_t
test_cmpltud (vui64_t a, vui64_t b)
{
  return vec_cmpltud (a, b);
}

vb64_t
test_cmpgeud (vui64_t a, vui64_t b)
{
  return vec_cmpgeud (a, b);
}

vb64_t
test_cmpleud (vui64_t a, vui64_t b)
{
  return vec_cmpleud (a, b);
}

vb64_t
test_cmpeqsd (vi64_t a, vi64_t b)
{
  return vec_cmpeqsd (a, b);
}

vb64_t
test_cmpnesd (vi64_t a, vi64_t b)
{
  return vec_cmpnesd (a, b);
}

vb64_t
test_cmpgtsd (vi64_t a, vi64_t b)
{
  return vec_cmpgtsd (a, b);
}

vb64_t
test_cmpltsd (vi64_t a, vi64_t b)
{
  return vec_cmpltsd (a, b);
}

vb64_t
test_cmpgesd (vi64_t a, vi64_t b)
{
  return vec_cmpgesd (a, b);
}

vb64_t
test_cmplesd (vi64_t a, vi64_t b)
{
  return vec_cmplesd (a, b);
}

vui64_t
__test_popcntd (vui64_t a)
{
  return vec_popcntd (a);
}

vui64_t
__test_clzd (vui64_t a)
{
  return vec_clzd (a);
}

vui64_t
__test_revbd (vui64_t vra)
{
  return vec_revbd (vra);
}

vi64_t
__test_maxsd (vi64_t __VH, vi64_t __VL)
{
  return vec_maxsd (__VH, __VL);
}

vui64_t
__test_maxud (vui64_t __VH, vui64_t __VL)
{
  return vec_maxud (__VH, __VL);
}

vi64_t
__test_minsd (vi64_t __VH, vi64_t __VL)
{
  return vec_minsd (__VH, __VL);
}

vui64_t
__test_minud (vui64_t __VH, vui64_t __VL)
{
  return vec_minud (__VH, __VL);
}

vui64_t
__test_mrgahd (vui128_t __VH, vui128_t __VL)
{
  return vec_mrgahd (__VH, __VL);
}

vui64_t
__test_mrgald (vui128_t __VH, vui128_t __VL)
{
  return vec_mrgald (__VH, __VL);
}

vui64_t
__test_mrghd (vui64_t __VH, vui64_t __VL)
{
  return vec_mrghd (__VH, __VL);
}

vui64_t
__test_mrgld (vui64_t __VH, vui64_t __VL)
{
  return vec_mrgld (__VH, __VL);
}

vui64_t
__test_permdi_0 (vui64_t __VH, vui64_t __VL)
{
  return vec_permdi (__VH, __VL, 0);
}

vui64_t
__test_permdi_1 (vui64_t __VH, vui64_t __VL)
{
  return vec_permdi (__VH, __VL, 1);
}

vui64_t
__test_permdi_2 (vui64_t __VH, vui64_t __VL)
{
  return vec_permdi (__VH, __VL, 2);
}

vui64_t
__test_permdi_3 (vui64_t __VH, vui64_t __VL)
{
  return vec_permdi (__VH, __VL, 3);
}

vui64_t
__test_vpaste (vui64_t __VH, vui64_t __VL)
{
  return vec_pasted (__VH, __VL);
}

vui64_t
__test_spltd_0 (vui64_t __VH)
{
  return vec_xxspltd (__VH, 0);
}

vui64_t
__test_spltd_1 (vui64_t __VH)
{
  return vec_xxspltd (__VH, 1);
}

vui64_t
__test_swapd (vui64_t __VH)
{
  return vec_swapd (__VH);
}

#ifdef _ARCH_PWR8
/* POWER 64-bit (vector long long) compiler tests.  */

vb64_t
__test_cmpequd (vui64_t a, vui64_t b)
{
  return vec_cmpeq (a, b);
}

vb64_t
__test_cmpgtud (vui64_t a, vui64_t b)
{
  return vec_cmpgt (a, b);
}

vb64_t
__test_cmpltud (vui64_t a, vui64_t b)
{
  return vec_cmplt (a, b);
}

vb64_t
__test_cmpgeud (vui64_t a, vui64_t b)
{
  return vec_cmpge (a, b);
}

vb64_t
__test_cmpleud (vui64_t a, vui64_t b)
{
  return vec_cmple (a, b);
}

vui64_t
__test_vmaddeuw (vui32_t a, vui32_t b, vui32_t c)
{
  return vec_vmaddeuw (a, b, c);
}

vui64_t
__test_vmaddouw (vui32_t a, vui32_t b, vui32_t c)
{
  return vec_vmaddouw (a, b, c);
}

vui64_t
__test_vmadd2euw (vui32_t a, vui32_t b, vui32_t c, vui32_t d)
{
  return vec_vmadd2euw (a, b, c, d);
}

vui64_t
__test_vmadd2ouw (vui32_t a, vui32_t b, vui32_t c, vui32_t d)
{
  return vec_vmadd2ouw (a, b, c, d);
}

vui64_t
__test_vmadduw (vui32_t a, vui32_t b, vui32_t c)
{
  vui64_t ep, op;
  ep = vec_vmaddeuw (a, b, c);
  op = vec_vmaddouw (a, b, c);
  return vec_addudm (ep, op);
}

vui64_t
__test_vmuludm (vui64_t vra, vui64_t vrb)
{
  vui64_t s32 = (vui64_t) { 32, 32 };
  vui64_t z = (vui64_t) { 0, 0 };
  vui64_t t4;
  vui64_t t2, t3;
  vui32_t t1;

  t1 = (vui32_t)vec_vrld (vrb, s32);
  t2 = vec_mulouw ((vui32_t)vra, (vui32_t)vrb);
  t3 = vec_vmsumuwm ((vui32_t)vra, t1, z);
  t4 = vec_vsld (t3, s32);
  return (vui64_t)vec_addudm (t4, t2);
}
#endif

#ifdef _ARCH_PWR7
/* POWER 64-bit (vector long long) compiler tests.  */

vui64_t
__test_vrld_PWR7 (vui64_t a, vui64_t b)
{
  vui64_t r, hd, ld;
  vui32_t t1, t2;
  vui8_t shh, shl;

  shh = vec_splat ((vui8_t)b, 7);
  shl = vec_splat ((vui8_t)b, 15);
  hd = vec_xxspltd (a, 0);
  ld = vec_xxspltd (a, 1);
  t1 = vec_vslo ((vui32_t)hd, shh);
  t1 = vec_vsl (t1, shh);
  t2 = vec_vslo ((vui32_t)ld, shl);
  t2 = vec_vsl (t2, shl);
  r = vec_mrghd ((vui64_t)t1, (vui64_t)t2);

  return r;
}

vui64_t
__test_xxpermdi_0 (vui64_t vra, vui64_t vrb)
{
  return vec_xxpermdi (vra, vrb, 0);
}

vui64_t
__test_xxpermdi_1 (vui64_t vra, vui64_t vrb)
{
  return vec_xxpermdi (vra, vrb, 1);
}

vui64_t
__test_xxpermdi_2 (vui64_t vra, vui64_t vrb)
{
  return vec_xxpermdi (vra, vrb, 2);
}

vui64_t
__test_xxpermdi_3 (vui64_t vra, vui64_t vrb)
{
  return vec_xxpermdi (vra, vrb, 3);
}

vui64_t
__test_mergeh (vui64_t a, vui64_t b)
{
  return vec_mergeh (a, b);
}

vui64_t
__test_mergel (vui64_t a, vui64_t b)
{
  return vec_mergel (a, b);
}

#if (__GNUC__ > 6)

vui64_t
__test_vmsumuwm (vui32_t vra, vui32_t vrb, vui64_t vrc)
{
  return vec_vmsumuwm (vra, vrb, vrc);
}

vui64_t
__test_splatd0 (vui64_t a)
{
  return vec_splat (a, 0);
}

vui64_t
__test_splatd1 (vui64_t a)
{
  return vec_splat (a, 1);
}

#if (__GNUC__ > 7)  && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)

vui64_t
__test_mergee (vui64_t a, vui64_t b)
{
  return vec_mergee (a, b);
}

vui64_t
__test_mergeo (vui64_t a, vui64_t b)
{
  return vec_mergeo (a, b);
}

vui64_t
__test_mul (vui64_t a, vui64_t b)
{
  return vec_mul (a, b);
}

vui64_t
__test_vmuludm_PWR7 (vui64_t vra, vui64_t vrb)
{
  vui64_t s32 = (vui64_t) { 32, 32 };
  vui64_t z = (vui64_t) { 0, 0 };
  vui64_t t4;
  vui64_t t2, t3;
  vui32_t t1;

  t1 = (vui32_t)vec_vrld (vrb, s32);
  t2 = vec_mulouw ((vui32_t)vra, (vui32_t)vrb);
  t3 = vec_vmsumuwm ((vui32_t)vra, t1, z);
  t4 = vec_vsld (t3, s32);
  return (vui64_t)vec_addudm (t4, t2);
}
#endif
#endif
#endif
