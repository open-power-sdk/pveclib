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
      Created on: Feb 7, 2024
 */

#include <pveclib/vec_int128_ppc.h>

vui128_t
__VEC_PWR_IMP (vec_diveuq) (vui128_t x, vui128_t z)
{
  return vec_vdiveuq_inline (x, z);
}

vui128_t
__VEC_PWR_IMP (vec_divuq) (vui128_t x, vui128_t z)
{
  return vec_vdivuq_inline (x, z);
}

vui128_t
__VEC_PWR_IMP (vec_moduq) (vui128_t x, vui128_t z)
{
  return vec_vmoduq_inline (x, z);
}

__VEC_U_128RQ
__VEC_PWR_IMP (vec_divdqu) (vui128_t x, vui128_t y, vui128_t z)
{
  return vec_divdqu_inline (x, y, z);
}

vui128_t
__VEC_PWR_IMP (vec_divduq) (vui128_t x, vui128_t y, vui128_t z)
{
    __VEC_U_128RQ result = vec_divdqu_inline (x, y, z);
    return result.Q;
}

vui128_t
__VEC_PWR_IMP (vec_modduq) (vui128_t x, vui128_t y, vui128_t z)
{
    __VEC_U_128RQ result = vec_divdqu_inline (x, y, z);
    return result.R;
}


