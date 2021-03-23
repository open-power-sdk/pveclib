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

 vec_int32_dummy.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Mar 29, 2018
 */

#include <pveclib/vec_int128_ppc.h>

vb32_t
test_setb_sw (vi32_t vra)
{
  return vec_setb_sw (vra);
}

#if defined(_ARCH_PWR8)
vi64_t
test_vec_unpackh (vi32_t vra)
{
  return vec_unpackh (vra);
}

vi64_t
test_vec_unpackl (vi32_t vra)
{
  return vec_unpackl (vra);
}
#endif

vi64_t
test_vec_vupkhsw (vi32_t vra)
{
  return vec_vupkhsw (vra);
}

vi64_t
test_vec_vupklsw (vi32_t vra)
{
  return vec_vupklsw (vra);
}

vui64_t
test_vec_vupkhuw (vui32_t vra)
{
  return vec_vupkhuw (vra);
}

vui64_t
test_vec_vupkluw (vui32_t vra)
{
  return vec_vupkluw (vra);
}

vui32_t
test_vlsuwux (const signed long long ra, const unsigned int *rb)
{
  vui32_t xt;
  __VEC_U_128 t;

  unsigned int *p = (unsigned int *)((char *)rb + ra);
  t.ulong.upper = *p;
  xt = t.vx4;

  return xt;
}

#if !defined(_ARCH_PWR8)
vui64_t
test_vlxsiwx_PWR7 (const signed long long ra, const unsigned int *rb)
{
  const vui32_t zero = {0,0,0,0};
  vui32_t xte;
  vui8_t perm;

  perm = vec_lvsl (ra, rb);
  xte = vec_lde (ra, rb);
  xte = vec_perm (xte, xte, perm);
  return (vui64_t) vec_sld (zero, xte, 12);
}

vi64_t
test_vlxsiwax_PWR7 (const signed long long ra, const signed int *rb)
{
  vui32_t const shb = { 31, 0, 0 ,0 };
  vi32_t xte;
  vui8_t perm;

  perm = vec_lvsl (ra, rb);
  xte = vec_lde (ra, rb);
  perm = (vui8_t) vec_mergeh ((vui32_t) perm, (vui32_t) perm);
  xte = vec_perm (xte, xte, perm);
  return (vi64_t) vec_sra (xte, shb);
}
#endif

vui64_t
test_vec_vlxsiwx (const signed long long ra, const unsigned int *rb)
{
  return vec_vlxsiwzx (ra, rb);
}

vui64_t
test_vec_vlxsiwx_c0 (const unsigned int *rb)
{
  return vec_vlxsiwzx (0, rb);
}

vui64_t
test_vec_vlxsiwx_c1 (const unsigned int *rb)
{
  return vec_vlxsiwzx (8, rb);
}

vui64_t
test_vec_vlxsiwx_c2 (const unsigned int *rb)
{
  return vec_vlxsiwzx (32760, rb);
}

vui64_t
test_vec_vlxsiwx_c3 (const unsigned int *rb)
{
  return vec_vlxsiwzx (32768, rb);
}

vui64_t
test_vec_vlxsiwx_c4 (const unsigned int *rb)
{
  return vec_vlxsiwzx (-32768, rb);
}

vui64_t
test_vec_vgluwso (unsigned int *array, const long long offset0,
	     const long long offset1)
{
  return vec_vgluwso (array, offset0, offset1);
}

vui64_t
test_vec_vgluwdo (unsigned int *array, vi64_t offset)
{
  return vec_vgluwdo (array, offset);
}

vui64_t
test_vec_vgluwdsx (unsigned int *array, vi64_t offset)
{
  return vec_vgluwdsx (array, offset, 4);
}

vui64_t
test_vec_vgluwdx (unsigned int *array, vi64_t offset)
{
  return vec_vgluwdx (array, offset);
}

vi64_t
test_vec_vglswso (signed int *array, const long long offset0,
	     const long long offset1)
{
  return vec_vglswso (array, offset0, offset1);
}

vi64_t
test_vec_vglswdo (signed int *array, vi64_t offset)
{
  return vec_vglswdo (array, offset);
}

vi64_t
test_vec_vglswdx (signed int *array, vi64_t offset)
{
  return vec_vglswdx (array, offset);
}

vi64_t
test_vec_vglswdsx (signed int *array, vi64_t offset)
{
  return vec_vglswdsx (array, offset, 4);
}

vui32_t
test_vec_vgl4wso (unsigned int *array, const long long offset0,
	     const long long offset1, const long long offset2,
	     const long long offset3)
{
  return vec_vgl4wso (array, offset0, offset1, offset2, offset3);
}

vui32_t
test_vec_vgl4wwo (unsigned int *array, vi32_t vra)
{
  return vec_vgl4wwo (array, vra);
}

vui32_t
test_vec_vgl4wwx (unsigned int *array, vi32_t vra)
{
  return vec_vgl4wwx (array, vra);
}

vui32_t
test_vec_vgl4wwsx (unsigned int *array, vi32_t vra)
{
  return vec_vgl4wwsx (array, vra, 4);
}

#ifdef _ARCH_PWR8
vui32_t
test_vec_vgl4wwo_1 (unsigned int *array, vi32_t vra)
{
  vui32_t r;

  r = vec_vgl4wso (array, vra[0], vra[1], vra[2], vra[3]);
  return  r;
}

vui32_t
test_vec_vgl4wwo_2 (unsigned int *array, vi32_t vra)
{
  vui32_t r;
  vi64_t off01, off23;

  off01 = vec_vupkhsw (vra);
  off23 = vec_vupklsw (vra);

  r = vec_vgl4wso (array, off01[0], off01[1], off23[0], off23[1]);
  return  r;
}
#endif

vi64_t
test_vec_vlxsiwax (const signed long long ra, const signed int *rb)
{
  return vec_vlxsiwax (ra, rb);
}

vi64_t
test_vec_vlxsiwax_c0 (const signed int *rb)
{
  return vec_vlxsiwax (0, rb);
}

vi64_t
test_vec_vlxsiwax_c1 (const signed int *rb)
{
  return vec_vlxsiwax (8, rb);
}

vi64_t
test_vec_vlxsiwax_c2 (const signed int *rb)
{
  return vec_vlxsiwax (32760, rb);
}

vi64_t
test_vec_vlxsiwax_c3 (const signed int *rb)
{
  return vec_vlxsiwax (32768, rb);
}

vi64_t
test_vec_vlxsiwax_c4 (const signed int *rb)
{
  return vec_vlxsiwax (-32768, rb);
}

void
test_vstsuwux (vui32_t xs, const signed long long ra, unsigned int *rb)
{
  __VEC_U_128 t;
  unsigned int *p = (unsigned int *)((char *)rb + ra);
  t.vx4 = xs;
  *p = t.ulong.upper;
}

#if !defined(_ARCH_PWR8)
void
test_vstsuwux_PWR7 (vui32_t xs, const signed long long ra, unsigned int *rb)
{
  vui32_t xss = vec_splat (xs, 1);
  vec_ste (xss, ra, rb);
}
#endif

void
test_vec_vstxsiwx (vui32_t xs, const signed long long ra, unsigned int *rb)
{
  vec_vstxsiwx (xs, ra, rb);
}

void
test_vec_vstxsiwx_c0 (vui32_t xs, unsigned int *rb)
{
  vec_vstxsiwx (xs, 0, rb);
}

void
test_vec_vstxsiwx_c1 (vui32_t xs, unsigned int *rb)
{
  vec_vstxsiwx (xs, 8, rb);
}

void
test_vec_vstxsiwx_c2 (vui32_t xs, unsigned int *rb)
{
  vec_vstxsiwx (xs, 32760, rb);
}

void
test_vec_vstxsiwx_c3 (vui32_t xs, unsigned int *rb)
{
  vec_vstxsiwx (xs, 32768, rb);
}

void
test_vec_vstxsiwx_c4 (vui32_t xs, unsigned int *rb)
{
  vec_vstxsiwx (xs, -32768, rb);
}

void
test_vec_vsst4wso (vui32_t xs, unsigned int *array,
		   const long long offset0, const long long offset1,
		   const long long offset2, const long long offset3)
{
  vec_vsst4wso (xs, array, offset0, offset1, offset2, offset3);
}

void
test_vec_vsst4wwo (vui32_t xs, unsigned int *array,
		   vi32_t vra)
{
  vec_vsst4wwo (xs, array, vra);
}

void
test_vec_vsst4wwx (vui32_t xs, unsigned int *array,
		   vi32_t vra)
{
  vec_vsst4wwx (xs, array, vra);
}

void
test_vec_vsstwso (vui64_t xs, unsigned int *array,
	      const long long offset0, const long long offset1)
{
  vec_vsstwso (xs, array, offset0, offset1);
}

void
test_vec_vsstwdo (vui64_t xs, unsigned int *array,
		  vi64_t offset)
{
  vec_vsstwdo (xs, array, offset);
}

void
test_vec_vsstwdx (vui64_t xs, unsigned int *array,
		  vi64_t offset)
{
  vec_vsstwdx (xs, array, offset);
}

void
test_vec_vsstwdsx (vui64_t xs, unsigned int *array,
		  vi64_t offset)
{
  vec_vsstwdsx (xs, array, offset, 4);
}

vi32_t
test_vec_sum2s (vi32_t vra, vi32_t vrb)
{
  return vec_sum2s (vra, vrb);
}

vi32_t
test_vec_vsum2sw (vi32_t vra, vi32_t vrb)
{
  return vec_vsum2sw (vra, vrb);
}

vi32_t
test_vec_sums (vi32_t vra, vi32_t vrb)
{
  return vec_sums (vra, vrb);
}

vi32_t
test_vec_vsumsw (vi32_t vra, vi32_t vrb)
{
  return vec_vsumsw (vra, vrb);
}

vui32_t
test_ctz_v1 (vui32_t vra)
{
  const vui32_t ones = { 1, 1, 1, 1 };
  const vui32_t c32s = { 32, 32, 32, 32 };
  vui32_t term;
  // term = (!vra & (vra - 1))
  term = vec_andc (vec_sub (vra, ones), vra);
  // return = 32 - vec_clz (!vra & (vra - 1))
  return (c32s - vec_clzw (term));
}

vui32_t
test_ctz_v2 (vui32_t vra)
{
  const vui32_t ones = { 1, 1, 1, 1 };
  vui32_t term;
  // term = (!vra & (vra - 1))
  term = vec_andc (vec_sub (vra, ones), vra);
  // return = vec_popcnt (!vra & (vra - 1))
  return (vec_popcntw (term));
}

vui32_t
test_ctz_v3 (vui32_t vra)
{
  const vui32_t zeros = { 0, 0, 0, 0 };
  const vui32_t c32s = { 32, 32, 32, 32 };
  vui32_t term;
  // term = (vra | -vra))
  term = vec_or (vra, vec_sub (zeros, vra));
  // return = 32 - vec_poptcnt (vra & -vra)
  return (c32s - vec_popcntw (term));
}

vui32_t
test_vec_ctzw (vui32_t vra)
{
  return (vec_ctzw (vra));
}

#ifdef _ARCH_PWR8
#ifndef __clang__
// clang does not support specific built-ins for new (PWR8) operations.
// These are supported for GCC.
vui32_t
__test_mrgew (vui32_t a, vui32_t b)
{
  return vec_vmrgew (a, b);
}

vui32_t
__test_mrgow (vui32_t a, vui32_t b)
{
  return vec_vmrgow (a, b);
}
#endif
vui32_t
__test_mergeew (vui32_t a, vui32_t b)
{
  return vec_mergee (a, b);
}

vui32_t
__test_mergeow (vui32_t a, vui32_t b)
{
  return vec_mergeo (a, b);
}
#endif

vui32_t
__test_absduw (vui32_t a, vui32_t b)
{
  return vec_absduw (a, b);
}

vui32_t
__test_mrgahw (vui64_t a, vui64_t b)
{
  return vec_mrgahw (a, b);
}

vui32_t
__test_mrgalw (vui64_t a, vui64_t b)
{
  return vec_mrgalw (a, b);
}

vui32_t
test_mrgew (vui32_t a, vui32_t b)
{
  return vec_mrgew (a, b);
}

vui32_t
test_mrgow (vui32_t a, vui32_t b)
{
  return vec_mrgow (a, b);
}

vui64_t
__test_vmuleuw (vui32_t a, vui32_t b)
{
  return vec_vmuleuw (a, b);
}

vui64_t
__test_vmulouw (vui32_t a, vui32_t b)
{
  return vec_vmulouw (a, b);
}

vui64_t
__test_muleuw (vui32_t a, vui32_t b)
{
  return vec_muleuw (a, b);
}

vui64_t
__test_mulouw (vui32_t a, vui32_t b)
{
  return vec_mulouw (a, b);
}

vi64_t
__test_mulesw (vi32_t a, vi32_t b)
{
  return vec_mulesw (a, b);
}

vi64_t
__test_mulosw (vi32_t a, vi32_t b)
{
  return vec_mulosw (a, b);
}

vi32_t
__test_mulhsw (vi32_t a, vi32_t b)
{
  return vec_mulhsw (a, b);
}

vui32_t
__test_mulhuw (vui32_t a, vui32_t b)
{
  return vec_mulhuw (a, b);
}

vui32_t
__test_muluwm (vui32_t a, vui32_t b)
{
  return vec_muluwm (a, b);
}

vui32_t
test_slwi_1 (vui32_t a)
{
  return vec_slwi (a, 1);
}

vui32_t
test_slwi_15 (vui32_t a)
{
  return vec_slwi (a, 15);
}

vui32_t
test_slwi_16 (vui32_t a)
{
  return vec_slwi (a, 16);
}

vui32_t
test_slwi_31 (vui32_t a)
{
  return vec_slwi (a, 31);
}

vui32_t
test_slwi_32 (vui32_t a)
{
  return vec_slwi (a, 32);
}

vui32_t
test_srwi_1 (vui32_t a)
{
  return vec_srwi (a, 1);
}

vui32_t
test_srwi_15 (vui32_t a)
{
  return vec_srwi (a, 15);
}

vui32_t
test_srwi_16 (vui32_t a)
{
  return vec_srwi (a, 16);
}

vui32_t
test_srwi_31 (vui32_t a)
{
  return vec_srwi (a, 31);
}

vui32_t
test_srwi_32 (vui32_t a)
{
  return vec_srwi (a, 32);
}

vi32_t
test_srawi_1 (vi32_t a)
{
  return vec_srawi (a, 1);
}

vi32_t
test_srawi_15 (vi32_t a)
{
  return vec_srawi (a, 15);
}

vi32_t
test_srawi_16 (vi32_t a)
{
  return vec_srawi (a, 16);
}

vi32_t
test_srawi_31 (vi32_t a)
{
  return vec_srawi (a, 31);
}

vi32_t
test_srawi_32 (vi32_t a)
{
  return vec_srawi (a, 32);
}

vui32_t
test_cmpgtuw (vui32_t a, vui32_t b)
{
  return (vui32_t)vec_cmpgt (a, b);
}

vui32_t
test_cmpleuw (vui32_t a, vui32_t b)
{
  return (vui32_t)vec_cmple (a, b);
}

int
test_cmpuw_all_gt (vui32_t a, vui32_t b)
{
  return vec_all_gt (a, b);
}

int
test_cmpuw_all_le (vui32_t a, vui32_t b)
{
  return vec_all_le (a, b);
}

vui32_t
__test_popcntw (vui32_t a)
{
  return (vec_popcntw (a));
}

vui32_t
__test_clzw (vui32_t a)
{
  return (vec_clzw (a));
}

vui32_t
__test_revbw (vui32_t vra)
{
  return vec_revbw (vra);
}

void
example_convert_timebase (vui32_t *tb, vui32_t *timespec, int n)
{
  const vui32_t rnd_512 =
    { (256-1), (256-1), (256-1), (256-1) };
  /* Magic numbers for multiplicative inverse to divide by 1,000,000
     are 1125899907 and shift right 18 bits.  */
  const vui32_t mul_invs_1m =
    { 1125899907, 1125899907, 1125899907, 1125899907 };
  const int shift_1m = 18;
  /* Need const for microseconds/second to extract remainder.  */
  const vui32_t usec_sec =
    { 1000000, 1000000, 1000000, 1000000 };
  vui32_t tmp, tb_usec, seconds, useconds;
  vui32_t timespec1, timespec2;
  int i;

  for (i = 0; i < n; i++)
    {
      /* Convert 512MHz timebase to microseconds with rounding.  */
      tmp = vec_avg (*tb++, rnd_512);
      tb_usec = vec_srwi (tmp, 8);
      /* extract integer seconds from tb_usec.  */
      tmp = vec_mulhuw (tb_usec, mul_invs_1m);
      seconds = vec_srwi (tmp, shift_1m);
      /* Extract remainder microseconds. */
      tmp = vec_muluwm (seconds, usec_sec);
      useconds = vec_sub (tb_usec, tmp);
      /* Use merge high/low to interleave seconds and useconds in timespec.  */
      timespec1 = vec_mergeh (seconds, useconds);
      timespec2 = vec_mergel (seconds, useconds);
      /* Store timespec.  */
      *timespec++ = timespec1;
      *timespec++ = timespec2;
    }
}

/* these are test to see exactly what the compilers will generate for
   specific built-ins.  */

#if defined _ARCH_PWR8 && (__GNUC__ > 7)
vui64_t
__test_mulew (vui32_t vra, vui32_t vrb)
{
  return vec_mule (vra, vrb);
}

vui64_t
__test_mulow (vui32_t vra, vui32_t vrb)
{
  return vec_mulo (vra, vrb);
}
#endif

#if defined _ARCH_PWR7 && (__GNUC__ > 6)
vi32_t
__test_abssw (vi32_t vra)
{
  return vec_abs (vra);
}

vui32_t
__test_mulluw (vui32_t vra, vui32_t vrb)
{
  return vec_mul (vra, vrb);
}

vi32_t
__test_mullsw (vi32_t vra, vi32_t vrb)
{
  return vec_mul (vra, vrb);
}
#endif
