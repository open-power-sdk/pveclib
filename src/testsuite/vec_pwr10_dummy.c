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
__test_vmsumoud_PWR9 (vui64_t a, vui64_t b, vui128_t c)
{
  return vec_vmsumoud (a, b, c);
}

vui128_t
__test_vmsumeud_PWR9 (vui64_t a, vui64_t b, vui128_t c)
{
  return vec_vmsumeud (a, b, c);
}

vui128_t
__test_vmaddoud_PWR9 (vui64_t a, vui64_t b, vui64_t c)
{
  return vec_vmaddoud (a, b, c);
}

vui128_t
__test_vmaddeud_PWR9 (vui64_t a, vui64_t b, vui64_t c)
{
  return vec_vmaddeud (a, b, c);
}

vui128_t
__test_vmadd2oud_PWR9 (vui64_t a, vui64_t b, vui64_t c, vui64_t d)
{
  return vec_vmadd2oud (a, b, c, d);
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
#endif
