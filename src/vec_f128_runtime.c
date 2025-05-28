/*
 Copyright (c) [2023] Steven Munroe.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 vec_int512_runtime.c

 Contributors:
      Steven Munroe
      Created on: Aug 1, 2023
 */

#include <pveclib/vec_f128_ppc.h>

__binary128
__VEC_PWR_IMP (vec_xsaddqpo) (__binary128 vfa, __binary128 vfb)
{
  return vec_xsaddqpo_inline (vfa, vfb);
}

__binary128
__VEC_PWR_IMP (vec_xssubqpo) (__binary128 vfa, __binary128 vfb)
{
  return vec_xssubqpo_inline (vfa, vfb);
}

__binary128
__VEC_PWR_IMP (vec_xsdivqpo) (__binary128 vfa, __binary128 vfb)
{
  return vec_xsdivqpo_inline (vfa, vfb);
}

__binary128
__VEC_PWR_IMP (vec_xsmulqpo) (__binary128 vfa, __binary128 vfb)
{
  return vec_xsmulqpo_inline (vfa, vfb);
}

__binary128
__VEC_PWR_IMP (vec_xsmaddqpo) (__binary128 vfa, __binary128 vfb, __binary128 vfc)
{
  return vec_xsmaddqpo_inline (vfa, vfb, vfc);
}

__binary128
__VEC_PWR_IMP (vec_xsmsubqpo) (__binary128 vfa, __binary128 vfb, __binary128 vfc)
{
  return vec_xsmsubqpo_inline (vfa, vfb, vfc);
}

__binary128
__VEC_PWR_IMP (vec_xscvdpqp) (vf64_t vfa)
{
  return vec_xscvdpqp_inline (vfa);
}

vf64_t
__VEC_PWR_IMP (vec_xscvqpdpo) (__binary128 vfa)
{
  return vec_xscvqpdpo_inline (vfa);
}

vui64_t
__VEC_PWR_IMP (vec_xscvqpudz) (__binary128 vfa)
{
  return vec_xscvqpudz_inline (vfa);
}

vui128_t
__VEC_PWR_IMP (vec_xscvqpuqz) (__binary128 vfa)
{
  return vec_xscvqpuqz_inline (vfa);
}

__binary128
__VEC_PWR_IMP (vec_xscvsdqp) (vi64_t vfa)
{
  return vec_xscvsdqp_inline (vfa);
}

__binary128
__VEC_PWR_IMP (vec_xscvsqqp) (vi128_t vfa)
{
  return vec_xscvsqqp_inline (vfa);
}

__binary128
__VEC_PWR_IMP (vec_xscvudqp) (vui64_t vfa)
{
  return vec_xscvudqp_inline (vfa);
}

__binary128
__VEC_PWR_IMP (vec_xscvuqqp) (vui128_t vfa)
{
  return vec_xscvuqqp_inline (vfa);
}


