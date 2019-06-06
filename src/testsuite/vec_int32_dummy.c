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

#include <pveclib/vec_int32_ppc.h>

#ifdef _ARCH_PWR8
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
__test_muleuw (vui32_t a, vui32_t b)
{
  return vec_muleuw (a, b);
}

vui64_t
__test_mulouw (vui32_t a, vui32_t b)
{
  return vec_mulouw (a, b);
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
vui32_t
__test_mulluw (vui32_t vra, vui32_t vrb)
{
  return vec_mul (vra, vrb);
}
#endif
