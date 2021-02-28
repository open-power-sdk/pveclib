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

vui128_t
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
__test_muludq_PWR10 (vui128_t *mulh, vui128_t a, vui128_t b)
{
  return vec_muludq (mulh, a, b);
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
#endif
