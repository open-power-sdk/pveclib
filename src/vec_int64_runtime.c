/*
 Copyright (c) [2024] Steven Munroe.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 vec_int64_runtime.c

 Contributors:
      Steven Munroe
      Created on: Jan 27, 2024
 */

#include <pveclib/vec_int128_ppc.h>
#include <pveclib/vec_int64_ppc.h>

vui64_t
__VEC_PWR_IMP (vec_diveud) (vui64_t x, vui64_t z)
{
  return vec_vdiveud_inline (x, z);
}

vui64_t
__VEC_PWR_IMP (vec_divud) (vui64_t x, vui64_t z)
{
  return vec_vdivud_inline (x, z);
}

vui64_t
__VEC_PWR_IMP (vec_modud) (vui64_t x, vui64_t z)
{
  return vec_vmodud_inline (x, z);
}

vui64_t
__VEC_PWR_IMP (vec_divdud) (vui64_t x, vui64_t y, vui64_t z)
{
  return vec_divdud_inline (x, y, z);
}

vui64_t
__VEC_PWR_IMP (vec_moddud) (vui64_t x, vui64_t y, vui64_t z)
{
  return vec_moddud_inline (x, y, z);
}

vui64_t
__VEC_PWR_IMP (vec_divqud) (vui128_t x_y, vui64_t z)
{
  return vec_divqud_inline (x_y, z);
}


