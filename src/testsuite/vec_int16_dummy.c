/*
 Copyright (c) [2018] Steven Munroe.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 vec_int16_dummy.c

 Contributors:
      Steven Munroe
      Created on: Nov 7, 2018
 */

#include <pveclib/vec_int16_ppc.h>

vui16_t
__test_absduh (vui16_t a, vui16_t b)
{
  return vec_absduh (a, b);
}

vui16_t
__test_mrgahh (vui32_t a, vui32_t b)
{
  return vec_mrgahh (a, b);
}

vui16_t
__test_mrgalh (vui32_t a, vui32_t b)
{
  return vec_mrgalh (a, b);
}

vui16_t
test_mrgeh (vui16_t a, vui16_t b)
{
  return vec_mrgeh (a, b);
}

vui16_t
test_mrgoh (vui16_t a, vui16_t b)
{
  return vec_mrgoh (a, b);
}

vi16_t
__test_mulhsh (vi16_t a, vi16_t b)
{
  return vec_mulhsh (a, b);
}

vui16_t
__test_mulhuh (vui16_t a, vui16_t b)
{
  return vec_mulhuh (a, b);
}

vui16_t
__test_muluhm (vui16_t a, vui16_t b)
{
  return vec_muluhm (a, b);
}

vui32_t
__test_vmaddeuh (vui16_t a, vui16_t b, vui16_t c)
{
  return vec_vmaddeuh (a, b, c);
}

vui32_t
__test_vmaddouh (vui16_t a, vui16_t b, vui16_t c)
{
  return vec_vmaddouh (a, b, c);
}

vui32_t
__test_vmaddouh_alt (vui16_t a, vui16_t b, vui16_t c)
{
  const vui16_t zero = { 0, 0, 0, 0,  0, 0, 0, 0 };
  vui16_t b_oud = vec_mrgalh ((vui32_t) zero, (vui32_t) b);
  vui16_t c_oud = vec_mrgalh ((vui32_t) zero, (vui32_t) c);
  return vec_vmsumuhm(a, b_oud, (vui32_t) c_oud);
}

vui32_t
__test_vadduqm_alt7 (vui32_t a, vui32_t b)
{
  vui32_t t;
  vui32_t c, c2;
  vui32_t z= { 0,0,0,0};

  c = vec_vaddcuw ((vui32_t)a, (vui32_t)b);
  t = vec_vadduwm ((vui32_t)a, (vui32_t)b);
  c = vec_sld (c, z, 4);
  c2 = vec_vaddcuw (t, c);
  t = vec_vadduwm (t, c);
  c = vec_sld (c2, z, 4);
  c2 = vec_vaddcuw (t, c);
  t = vec_vadduwm (t, c);
  c = vec_sld (c2, z, 4);
  t = vec_vadduwm (t, c);
  return (t);
}

vui32_t
__test_vaddcuq_alt7 (vui32_t a, vui32_t b)
{
  vui32_t co;
  vui32_t c, c2, t;
  vui32_t z= { 0,0,0,0};

  co = vec_vaddcuw ((vui32_t)a, (vui32_t)b);
  t = vec_vadduwm ((vui32_t)a, (vui32_t)b);
  c = vec_sld (co, z, 4);
  c2 = vec_vaddcuw (t, c);
  t = vec_vadduwm (t, c);
  co = vec_vor (co, c2);
  c = vec_sld (c2, z, 4);
  c2 = vec_vaddcuw (t, c);
  t = vec_vadduwm (t, c);
  co = vec_vor (co, c2);
  c = vec_sld (c2, z, 4);
  c2 = vec_vaddcuw (t, c);
  co = vec_vor (co, c2);
  co = vec_sld (z, co, 4);
  return ((vui32_t) co);
}

vui32_t
__test_vmadduh_alt7 (vui32_t *mulh, vui16_t a, vui16_t b, vui16_t c)
{
  vui32_t t, tmq;// _ARCH_PWR7 or earlier and Big Endian only.  */

  /* We use Vector Multiply Even/Odd Unsigned Halfword to compute
   * the 128 x 16 partial (144-bit) product of vector a with a
   * halfword element of b. The (for each halfword of vector b)
   * 8 X 144-bit partial products are  summed to produce the full
   * 256-bit product. */
  vui16_t tsw;
  vui16_t tc;
  vui16_t t_odd, t_even;
  vui16_t z = { 0, 0, 0, 0, 0, 0, 0, 0 };

  tsw = vec_splat ((vui16_t) b, 7);
#if 1
  t_even = (vui16_t)vec_vmuleuh((vui16_t)a, tsw);
  t_odd = (vui16_t)vec_vmulouh((vui16_t)a, tsw);
#else
  t_even = (vui16_t)vec_vmaddeuh((vui16_t)a, tsw, (vui16_t)c);
  t_odd = (vui16_t)vec_vmaddouh((vui16_t)a, tsw, (vui16_t)c);
#endif

  /* Rotate the low 16-bits (right) into tmq. This is actually
   * implemented as 112-bit (14-byte) shift left. */
  tmq = (vui32_t)vec_sld (t_odd, z, 14);
  /* shift the low 128 bits of partial product right 16-bits */
  t_odd = vec_sld (z, t_odd, 14);
  /* add the high 128 bits of even / odd partial products */
  t = (vui32_t) __test_vadduqm_alt7 ((vui32_t) t_even, (vui32_t) t_odd);

  tsw = vec_splat ((vui16_t) b, 6);
  t_even = (vui16_t)vec_vmaddeuh((vui16_t)a, tsw, (vui16_t)t);
  t_odd = (vui16_t)vec_vmaddouh((vui16_t)a, tsw, (vui16_t)t);

  /* rotate right the low 16-bits into tmq */
  tmq = (vui32_t)vec_sld (t_odd, (vui16_t)tmq, 14);
  /* shift the low 128 bits (with carry) of partial product right
   * 16-bits */
  t_odd = vec_sld (tc, t_odd, 14);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) __test_vadduqm_alt7 ((vui32_t) t_even, (vui32_t) t_odd);

  tsw = vec_splat ((vui16_t) b, 5);
  t_even = (vui16_t)vec_vmaddeuh((vui16_t)a, tsw, (vui16_t)t);
  t_odd = (vui16_t)vec_vmaddouh((vui16_t)a, tsw, (vui16_t)t);

  /* rotate right the low 16-bits into tmq */
  tmq = (vui32_t)vec_sld (t_odd, (vui16_t)tmq, 14);
  /* shift the low 128 bits (with carry) of partial product right
   * 16-bits */
  t_odd = vec_sld (tc, t_odd, 14);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) __test_vadduqm_alt7 ((vui32_t) t_even, (vui32_t) t_odd);

  tsw = vec_splat ((vui16_t) b, 4);
  t_even = (vui16_t)vec_vmaddeuh((vui16_t)a, tsw, (vui16_t)t);
  t_odd = (vui16_t)vec_vmaddouh((vui16_t)a, tsw, (vui16_t)t);

  /* rotate right the low 16-bits into tmq */
  tmq = (vui32_t)vec_sld (t_odd, (vui16_t)tmq, 14);
  /* shift the low 128 bits (with carry) of partial product right
   * 16-bits */
  t_odd = vec_sld (tc, t_odd, 14);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) __test_vadduqm_alt7 ((vui32_t) t_even, (vui32_t) t_odd);

  tsw = vec_splat ((vui16_t) b, 3);
  t_even = (vui16_t)vec_vmaddeuh((vui16_t)a, tsw, (vui16_t)t);
  t_odd = (vui16_t)vec_vmaddouh((vui16_t)a, tsw, (vui16_t)t);

  /* rotate right the low 16-bits into tmq */
  tmq = (vui32_t)vec_sld (t_odd, (vui16_t)tmq, 14);
  /* shift the low 128 bits (with carry) of partial product right
   * 16-bits */
  t_odd = vec_sld (tc, t_odd, 14);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) __test_vadduqm_alt7 ((vui32_t) t_even, (vui32_t) t_odd);


  tsw = vec_splat ((vui16_t) b, 2);
  t_even = (vui16_t)vec_vmaddeuh((vui16_t)a, tsw, (vui16_t)t);
  t_odd = (vui16_t)vec_vmaddouh((vui16_t)a, tsw, (vui16_t)t);

  /* rotate right the low 16-bits into tmq */
  tmq = (vui32_t)vec_sld (t_odd, (vui16_t)tmq, 14);
  /* shift the low 128 bits (with carry) of partial product right
   * 16-bits */
  t_odd = vec_sld (tc, t_odd, 14);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) __test_vadduqm_alt7 ((vui32_t) t_even, (vui32_t) t_odd);

  tsw = vec_splat ((vui16_t) b, 1);
  t_even = (vui16_t)vec_vmaddeuh((vui16_t)a, tsw, (vui16_t)t);
  t_odd = (vui16_t)vec_vmaddouh((vui16_t)a, tsw, (vui16_t)t);

  /* rotate right the low 16-bits into tmq */
  tmq = (vui32_t)vec_sld (t_odd, (vui16_t)tmq, 14);
  /* shift the low 128 bits (with carry) of partial product right
   * 16-bits */
  t_odd = vec_sld (tc, t_odd, 14);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) __test_vadduqm_alt7 ((vui32_t) t_even, (vui32_t) t_odd);


  tsw = vec_splat ((vui16_t) b, 0);
  t_even = (vui16_t)vec_vmaddeuh((vui16_t)a, tsw, (vui16_t)t);
  t_odd = (vui16_t)vec_vmaddouh((vui16_t)a, tsw, (vui16_t)t);

  /* rotate right the low 16-bits into tmq */
  tmq = (vui32_t)vec_sld (t_odd, (vui16_t)tmq, 14);
  /* shift the low 128 bits (with carry) of partial product right
   * 16-bits */
  t_odd = vec_sld (tc, t_odd, 14);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) __test_vadduqm_alt7 ((vui32_t) t_even, (vui32_t) t_odd);

  *mulh = t;
  return ((vui32_t) tmq);
}

vui16_t
__test_mrgeh2mrgoh (vui16_t a, vui16_t b, vui16_t c, vui16_t d)
{
  return vec_mrgoh (vec_mrgeh (a, b), vec_mrgeh (c, d));
}

// Divide vector integer by constant divisor
// Using multiplicative inverse
static vui16_t
__test_mulinvuh (vui16_t n, const unsigned short M, const unsigned short a,
		 const unsigned short s)
{
  vui16_t result, q;
  vui16_t magic = vec_splats (M);

  q = vec_mulhuh (n, magic);
  if (a)
    {
      const vui16_t vec_ones =
	{ 1, 1, 1, 1, 1, 1, 1, 1 };
      vui16_t n_1 = vec_sub (n, vec_ones);
      q = vec_avg (q, n_1);
      if (s > 1)
	result = vec_srhi (q, (s - 1));
    }
  else
    result = vec_srhi (q, s);
  return result;
}

// Examples of Divide vector integer by constant divisor

vui16_t
__test_div10 (vui16_t n)
{
  vui16_t result, q;
  /* M= 52429, a=0, s=3 */
  const vui16_t magic = vec_splats ((unsigned short) 52429);
  const int s = 3;

  q = vec_mulhuh (magic, n);
  result = vec_srhi (q, s);
  return result;
}

vui16_t
__test_mod10 (vui16_t n)
{
  vui16_t result, q;
  /* M= 52429, a=0, s=3 */
  vui16_t magic = vec_splats ((unsigned short) 52429);
  vui16_t c_10 = vec_splats ((unsigned short) 10);
  const int s = 3;
  vui16_t tmp, q_10;

  q = vec_mulhuh (magic, n);
  q_10 = vec_srhi (q, s);
  tmp = vec_muluhm (q_10, c_10);
  result = vec_sub (n, tmp);
  return result;
}

vui16_t
__test_div10000 (vui16_t n)
{
  vui16_t result, q;
  /* M= 41839, a=1, s=14 */
  vui16_t magic = vec_splats ((unsigned short) 41839);
  const int s = 14;

  q = vec_mulhuh (magic, n);
    {
    // In this case the perfect multiplier is too large (>= 2**16)
    // So the magic multiplier is reduced by 2**16
    // Then correct for this by adding n to the product
    // The add may generate a carry that must be part of the shift
    // Here vec_avg handles the 17-bit sum internally before shifting right 1
    // But vec_avg adds an extra +1 (for rounding) that we don't want
    // So we use (n-1) for the product correction
    // Then complete the operation with shift right (s-1)
      const vui16_t vec_ones = vec_splat_u16 ( 1 );
      vui16_t n_1 = vec_sub (n, vec_ones);
      // avg = (q + (n-1) + 1) >> 1
      q = vec_avg (q, n_1);
      result = vec_srhi (q, (s - 1));
    }
  return result;
}

vui16_t
__test_mod10000 (vui16_t n)
{
  vui16_t result, q;
  /* M= 41839, a=1, s=14 */
  vui16_t magic = vec_splats ((unsigned short) 41839);
  vui16_t c_10K = vec_splats ((unsigned short) 10000);
  const int s = 14;
  vui16_t tmp, q_10K;

  q = vec_mulhuh (magic, n);
    {
      const vui16_t vec_ones = vec_splat_u16(1);
      vui16_t n_1 = vec_sub (n, vec_ones);
      // avg = (q + (n-1) + 1) >> 1
      q = vec_avg (q, n_1);
      q_10K = vec_srhi (q, (s - 1));
    }
  tmp = vec_muluhm (q_10K, c_10K);
  result = vec_sub (n, tmp);
  return result;
}

vui16_t
__test_div_rem_10k (vui16_t n, vui16_t *rem_10K)
{
  const vui16_t c_10K = {10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000};
  vui16_t d_10K;
  vui16_t tmp, rem;
  d_10K = __test_mulinvuh (n, 41839, 1, 14);
  tmp = vec_muluhm (d_10K, c_10K);
  rem = vec_sub (n, tmp);
  *rem_10K = rem;
  return d_10K;
}


