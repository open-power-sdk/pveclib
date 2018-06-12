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

#include <vec_int64_ppc.h>

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
test_cmpud_all_gt (vui64_t a, vui64_t b)
{
  return vec_cmpud_all_gt (a, b);
}

vui64_t
test_cmpud_all_gt2 (vui64_t a, vui64_t b, vui64_t c, vui64_t d)
{
  if (vec_cmpud_all_gt (a, b))
    return c;
  else
    return d;
}

int
test_cmpud_all_eq (vui64_t a, vui64_t b)
{
  return vec_cmpud_all_eq (a, b);
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
  return (vec_subudm (a, b));
}

vui64_t
test_addudm (vui64_t a, vui64_t b)
{
  return (vec_addudm (a, b));
}

vui64_t
test_cmpgtud (vui64_t a, vui64_t b)
{
  return (vec_cmpgtud (a, b));
}

vui64_t
__test_popcntd (vui64_t a)
{
  return (vec_popcntd (a));
}

vui64_t
__test_clzd (vui64_t a)
{
  return (vec_clzd (a));
}

vui64_t
__test_revbd (vui64_t vra)
{
  return vec_revbd (vra);
}

vui64_t
__test_mrghd (vui64_t __VH, vui64_t __VL)
{
	return (vec_mrghd(__VH, __VL));
}

vui64_t
__test_mrgld (vui64_t __VH, vui64_t __VL)
{
	return (vec_mrgld(__VH, __VL));
}

vui64_t
__test_permdi_0 (vui64_t __VH, vui64_t __VL)
{
	return (vec_permdi(__VH, __VL, 0));
}

vui64_t
__test_permdi_1 (vui64_t __VH, vui64_t __VL)
{
	return (vec_permdi(__VH, __VL, 1));
}

vui64_t
__test_permdi_2 (vui64_t __VH, vui64_t __VL)
{
	return (vec_permdi(__VH, __VL, 2));
}

vui64_t
__test_permdi_3 (vui64_t __VH, vui64_t __VL)
{
	return (vec_permdi(__VH, __VL, 3));
}

vui64_t
__test_vpaste (vui64_t __VH, vui64_t __VL)
{
	return (vec_pasted(__VH, __VL));
}

vui64_t
__test_spltd_0 (vui64_t __VH)
{
	return (vec_spltd(__VH, 0));
}

vui64_t
__test_spltd_1 (vui64_t __VH)
{
	return (vec_spltd(__VH, 1));
}

vui64_t
__test_swapd (vui64_t __VH)
{
	return (vec_swapd(__VH));
}
