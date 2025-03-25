/*
 Copyright (c) [2017] IBM Corporation.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 vec_int128_dummy.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: May 9, 2017
 */

#include <stdint.h>
#include <stdio.h>
#include <pveclib/vec_int128_ppc.h>
#include "arith128.h"
#include <testsuite/arith128_print.h>

vi128_t
test_vec_signextq_byte (vi8_t vra)
{
  return vec_signextq_byte (vra);
}

vi128_t
test_vec_signextq_halfword (vi16_t vra)
{
  return vec_signextq_halfword (vra);
}

vi128_t
test_vec_signextq_word (vi32_t vra)
{
  return vec_signextq_word (vra);
}

vi128_t
test_vec_signextq_doubleword (vi64_t vra)
{
  return vec_signextq_doubleword (vra);
}

vi128_t
test_vec_vextsb2q (vi8_t vra)
{
  return vec_vextsb2q (vra);
}

vi128_t
test_vec_vextsh2q (vi16_t vra)
{
  return vec_vextsh2q (vra);
}

vi128_t
test_vec_vextsw2q (vi32_t vra)
{
  return vec_vextsw2q (vra);
}

vi128_t
test_vec_vextsd2q (vi64_t vra)
{
  return vec_vextsd2q (vra);
}

vui128_t
test_vec_expandm_quadword (vui128_t vra)
{
  return vec_expandm_quadword (vra);
}

vui128_t
test_vexpandqm_PWR (vui128_t vra)
{
  return vec_vexpandqm_PWR10 (vra);
}

#if 0 // Deprecated !
vui128_t
test_vec_rlq_byte  (vui128_t a, vui8_t b)
{
  return (vec_vrlq_byte (a, b));
}

vui128_t
test_vec_slq_byte  (vui128_t a, vui8_t b)
{
  return (vec_vslq_byte (a, b));
}

vui128_t
test_vec_srq_byte  (vui128_t a, vui8_t b)
{
  return (vec_vsrq_byte (a, b));
}

vi128_t
test_vec_sraq_byte  (vi128_t a, vui8_t b)
{
  return (vec_vsraq_byte (a, b));
}
#endif

vui128_t
test_vec_rlqi_PWR_1  (vui128_t a)
{
  return (vec_rlqi_PWR10 (a, 1));
}

vui128_t
test_vec_rlqi_PWR_15  (vui128_t a)
{
  return (vec_rlqi_PWR10 (a, 15));
}

vui128_t
test_vec_rlqi_PWR_16  (vui128_t a)
{
  return (vec_rlqi_PWR10 (a, 16));
}

vui128_t
test_vec_rlqi_PWR_31  (vui128_t a)
{
  return (vec_rlqi_PWR10 (a, 31));
}

vui128_t
test_vec_rlqi_PWR_64  (vui128_t a)
{
  return (vec_rlqi_PWR10 (a, 64));
}

vui128_t
test_vec_rlqi_PWR_112  (vui128_t a)
{
  return (vec_rlqi_PWR10 (a, 112));
}

vui128_t
test_vec_rlqi_PWR_127  (vui128_t a)
{
  return (vec_rlqi_PWR10 (a, 127));
}

vui128_t
test_vec_rlq_PWR  (vui128_t a, vui8_t sh)
{
  return (vec_rlq_PWR10 (a, sh));
}

vui128_t
test_vec_slqi_PWR_1  (vui128_t a)
{
  return (vec_slqi_PWR10 (a, 1));
}

vui128_t
test_vec_slqi_PWR_15  (vui128_t a)
{
  return (vec_slqi_PWR10 (a, 15));
}

vui128_t
test_vec_slqi_PWR_16  (vui128_t a)
{
  return (vec_slqi_PWR10 (a, 16));
}

vui128_t
test_vec_slqi_PWR_31  (vui128_t a)
{
  return (vec_slqi_PWR10 (a, 31));
}

vui128_t
test_vec_slqi_PWR_32  (vui128_t a)
{
  return (vec_slqi_PWR10 (a, 32));
}

vui128_t
test_vec_slqi_PWR_48  (vui128_t a)
{
  return (vec_slqi_PWR10 (a, 48));
}

vui128_t
test_vec_slqi_PWR_64  (vui128_t a)
{
  return (vec_slqi_PWR10 (a, 64));
}

vui128_t
test_vec_slqi_PWR_112  (vui128_t a)
{
  return (vec_slqi_PWR10 (a, 112));
}

vui128_t
test_vec_slqi_PWR_127  (vui128_t a)
{
  return (vec_slqi_PWR10 (a, 127));
}

vui128_t
test_vec_slq_PWR  (vui128_t a, vui8_t sh)
{
  return (vec_slq_PWR10 (a, sh));
}

vui128_t
test_vec_srqi_PWR_1  (vui128_t a)
{
  return (vec_srqi_PWR10 (a, 1));
}

vui128_t
test_vec_srqi_PWR_15  (vui128_t a)
{
  return (vec_srqi_PWR10 (a, 15));
}

vui128_t
test_vec_srqi_PWR_16  (vui128_t a)
{
  return (vec_srqi_PWR10 (a, 16));
}

vui128_t
test_vec_srqi_PWR_31  (vui128_t a)
{
  return (vec_srqi_PWR10 (a, 31));
}

vui128_t
test_vec_srqi_PWR_52  (vui128_t a)
{
  return (vec_srqi_PWR10 (a, 52));
}

vui128_t
test_vec_srqi_PWR_60  (vui128_t a)
{
  return (vec_srqi_PWR10 (a, 60));
}

vui128_t
test_vec_srqi_PWR_64  (vui128_t a)
{
  return (vec_srqi_PWR10 (a, 64));
}

vui128_t
test_vec_srqi_PWR_112  (vui128_t a)
{
  return (vec_srqi_PWR10 (a, 112));
}

vui128_t
test_vec_srqi_PWR_127  (vui128_t a)
{
  return (vec_srqi_PWR10 (a, 127));
}

vui128_t
test_vec_srq_PWR  (vui128_t a, vui8_t sh)
{
  return (vec_srq_PWR10 (a, sh));
}

vi128_t
test_vec_sraqi_PWR_1  (vi128_t a)
{
  return (vec_sraqi_PWR10 (a, 1));
}

vi128_t
test_vec_sraqi_PWR_15  (vi128_t a)
{
  return (vec_sraqi_PWR10 (a, 15));
}

vi128_t
test_vec_sraqi_PWR_31  (vi128_t a)
{
  return (vec_sraqi_PWR10 (a, 31));
}

vi128_t
test_vec_sraqi_PWR_32  (vi128_t a)
{
  return (vec_sraqi_PWR10 (a, 32));
}

vi128_t
test_vec_sraqi_PWR_63  (vi128_t a)
{
  return (vec_sraqi_PWR10 (a, 63));
}

vi128_t
test_vec_sraqi_PWR_64  (vi128_t a)
{
  return (vec_sraqi_PWR10 (a, 64));
}

vi128_t
test_vec_sraqi_PWR_112  (vi128_t a)
{
  return (vec_sraqi_PWR10 (a, 112));
}

vi128_t
test_vec_sraqi_PWR_127  (vi128_t a)
{
  return (vec_sraqi_PWR10 (a, 127));
}

vi128_t
test_vec_sraq_PWR  (vi128_t a, vui8_t sh)
{
  return (vec_sraq_PWR10 (a, sh));
}

vui128_t
test_vec_ctzq_PWR7 (vui128_t vra)
{
  return vec_ctzq_PWR7 (vra);
}

void
test_vec_ctzq_loop7 (vui128_t *vrt, vui128_t *vra, long int N)
{
  long int i;
  for (i=0; i < N; i++)
    {
      vrt[i] = vec_ctzq_PWR7 (vra[i]);
    }
}

vui128_t
test_vec_ctzq_PWR8 (vui128_t vra)
{
  return vec_ctzq_PWR8 (vra);
}

void
test_vec_ctzq_loop8 (vui128_t *vrt, vui128_t *vra, long int N)
{
  long int i;
  for (i=0; i < N; i++)
    {
      vrt[i] = vec_ctzq_PWR8 (vra[i]);
    }
}

vui128_t
test_vec_ctzq_PWR9 (vui128_t vra)
{
  return vec_ctzq_PWR9 (vra);
}

vui128_t
test_ctzq_PWR7 (vui128_t vra)
{
  vui32_t result;
  /* vector ctz instructions were introduced in power9. For power7 and
   * earlier, use the pveclib vec_ctzw_PWR7 implementation.  For a quadword
   * clz, this requires pre-conditioning the input before computing the
   * the word ctz and sum across.   */
  const vui32_t vzero = vec_splat_u32 (0);
  vui32_t clz;
  vui32_t r32, gt32, gt32sr32, gt64sr64;

  // From right to left, any word to the left of the 1st nonzero word
  // is set to 0xffffffff (ctz(0xffffffff) = 0)
  gt32 = (vui32_t) vec_cmpgt ((vui32_t) vra, vzero);
  gt32sr32 = vec_sld (gt32, vzero, 12);
  gt64sr64 = vec_sld (gt32, vzero, 8);
  gt32 = vec_sld (gt32, vzero, 4);

  gt32sr32 = vec_or (gt32sr32, gt32);
  gt64sr64 = vec_or (gt64sr64, (vui32_t) vra);
  r32 = vec_or (gt32sr32, gt64sr64);

  // This allows quadword ctz to be a simple sum across the words
  clz = vec_ctzw_PWR7 (r32);
  result = (vui32_t) vec_sums ((vi32_t) clz, (vi32_t) vzero);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  // undo endian for PWR8 testing only, work around!
  result = vec_sld (result, result, 4);
#endif

  return ((vui128_t) result);
}

vui128_t
test_ctzq_PWR8 (vui128_t vra)
{
  vui64_t result;

#ifdef _ARCH_PWR8
#if 1
  const vui128_t ones = (vui128_t) vec_splat_s32(-1);
  vui128_t tzmask;

  // tzmask = (!vra & (vra - 1))
  tzmask = (vui128_t) vec_andc ((vui64_t) vec_adduqm_PWR8 (vra, ones),
				(vui64_t) vra);
  // return = vec_popcnt (!vra & (vra - 1))
  return vec_popcntq_PWR8 (tzmask);
#else
  /* vector ctz instructions were introduced in power9. For power8 and
   * earlier, use the pveclib vec_popcntd_PWR8 implementation.  For a
   * quadword clz, this requires pre-conditioning the input before
   * computing the the dword ctz and sum across.   */
  const vui64_t vzero = (vui64_t) vec_splat_u32 (0);
  const vui64_t vones = (vui64_t) vec_splat_u32 (-1);
  vui64_t ctz;
  vui64_t r64, gt64, gt64sr64;
  vui64_t tzmask;

  // From right to left, any dword to the left of the 1st nonzero
  // dword is set to 0xffffffffffffffff (for which ctz returns 0).
  gt64 = (vui64_t) vec_cmpgt ((vui64_t) vra, vzero);
  gt64sr64 = vec_sld (gt64, vzero, 8);

  r64 = vec_or (gt64sr64, (vui64_t) vra);

  // This allows quadword ctz to be a simple sum across of the dwords
  // tzmask = (!r64 & (r64 - 1))
  tzmask = vec_andc (vec_add ((vui64_t) r64, vones), (vui64_t) r64);
  ctz = vec_popcntd_PWR8 (tzmask);
#if 1
  vui64_t h64, l64;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  // Undo the endian transform
  h64 = vec_mergel (ctz, vzero);
  l64 = vec_mergeh (ctz, vzero);
#else
  h64 = vec_mergeh (vzero, ctz);
  l64 = vec_mergel (vzero, ctz);
#endif
  result = vec_add (h64, l64);
#else
  result = (vui64_t) vec_sums ((vi32_t) clz, (vi32_t) vzero);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  // undo endian for PWR8 testing only, work around!
  result = vec_sld (result, result, 4);
#endif
#endif
#endif
#else
  result = (vui64_t) test_ctzq_PWR7 (vra);
#endif

  return ((vui128_t) result);
}

vui128_t
test_ctzq_PWR9 (vui128_t vra)
{
  vui64_t result;

#ifdef _ARCH_PWR9
#if 1
  const vui128_t ones = (vui128_t) vec_splat_s32(-1);
  vui128_t tzmask;

  // tzmask = (!vra & (vra - 1))
  tzmask = (vui128_t) vec_andc ((vui64_t) vec_adduqm_PWR8 (vra, ones),
				(vui64_t) vra);
  // return = vec_popcnt (!vra & (vra - 1))
  return vec_popcntq_PWR9 (tzmask);
#else
  /* vector ctz instructions were introduced in power9. For power8 and
   * earlier, use the pveclib vec_popcntd_PWR8 implementation.  For a
   * quadword clz, this requires pre-conditioning the input before
   * computing the the dword ctz and sum across.   */
  const vui64_t vzero = (vui64_t) vec_splat_u32 (0);
  const vui64_t vones = (vui64_t) vec_splat_u32 (-1);
  vui64_t ctz;
  vui64_t r64, gt64, gt64sr64;
  vui64_t h64, l64;

  // From right to left, any dword to the left of the 1st nonzero
  // dword is set to 0xffffffffffffffff (for which ctz returns 0).
  gt64 = (vui64_t) vec_cmpgt ((vui64_t) vra, vzero);
  gt64sr64 = vec_sld (gt64, vzero, 8);

  r64 = vec_or (gt64sr64, (vui64_t) vra);

  // This allows quadword ctz to be a simple sum across of the dwords
  ctz = vec_ctzd_PWR9 (r64);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  // Undo the endian transform
  h64 = vec_mergel (ctz, vzero);
  l64 = vec_mergeh (ctz, vzero);
#else
  h64 = vec_mergeh (vzero, ctz);
  l64 = vec_mergel (vzero, ctz);
#endif
  result = vec_add (h64, l64);
#endif
#else
  result = (vui64_t) test_ctzq_PWR8 (vra);
#endif

  return ((vui128_t) result);
}

vui128_t
test_vec_clzq (vui128_t vra)
{
  return vec_clzq (vra);
}

vui128_t
test_vec_clzq_PWR7 (vui128_t vra)
{
  return vec_clzq_PWR7 (vra);
}

vui128_t
__test_clzq_PWR7 (vui128_t vra)
{
  /*
   * Use the Vector Count Leading Zeros Double Word instruction to get
   * the count for the left and right vector halves.  If the left vector
   * doubleword of the input is nonzero then only the left count is
   * included and we need to mask off the right count.
   * Otherwise the left count is 64 and we need to add 64 to the right
   * count.
   * After masking we sum across the left and right counts to
   * get the final 128-bit vector count (0-128).
   *
   * Did not use this version as the version based on clzw is more
   * compact. Turns out the doubleword compare is expensive.
   */
  vui32_t result;
  vui32_t vt1, vt2, vt3, h64, l64;
  const vui64_t vzero = { 0, 0 };

#if 1
  {
    vui32_t v2, v2t, v2x;
    v2 = (vui32_t) vec_cmpeq((vui32_t) vra, (vui32_t)vzero);
    v2t = vec_sld (v2, v2, 4);
    v2x = vec_and (v2, v2t);
    // DW0 bool vmpeq in DW0
    vt2 = vec_mergeh (v2x, v2x);
    // DW1 bool vmpeq in DW0 -< vec_mergel (v2x, v2x)
  }
#else
  vt2 = (vui32_t) vec_cmpequd((vui64_t) vra, vzero);
#endif
  vt1 = (vui32_t) vec_clzd_PWR7 ((vui64_t) vra);
  vt3 = vec_sld ((vui32_t)vzero, vt2, 8);
  h64 = vec_sld ((vui32_t)vzero, vt1, 8);
  l64 = vec_and (vt1, vt3);
  result = vec_add (h64, l64);

  return ((vui128_t) result);
}

vui128_t
test_vec_popcntq (vui128_t vra)
{
  return vec_popcntq (vra);
}

vui128_t
test_vec_popcntq_PWR7 (vui128_t vra)
{
  return vec_popcntq_PWR7 (vra);
}

vui128_t
test_vec_popcntq_PWR8 (vui128_t vra)
{
  return vec_popcntq_PWR8 (vra);
}

vui128_t
test_popcntq_PWR8 (vui128_t vra)
{
  vui128_t result;

#if defined(_ARCH_PWR8)
  /*
   * Use the Vector Population Count Doubleword instruction to get the
   * count for each DW.  Then sum across the DWs to get the final
   * 128-bit vector count (0-128). For P8 popcntw is 2 cycles faster
   * then popcntd but requires vsumsws (7 cycles). Using popcntd
   *  allows a faster sum and saves a cycle over all.
   */
  vui32_t vt1, h64, l64;
  const vui32_t vzero = { 0, 0, 0, 0 };

  vt1 = (vui32_t) vec_popcntd_PWR8 ((vui64_t)  vra);
  // Note high words 0,1,2 of vt1 are zero
  // h64 = { vzero [0] | vt1 [0] | vzero [1] | vt1 [1] }
  // l64 = { vzero [2] | vt1 [2] | vzero [3] | vt1 [3] }
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  // Undo the endian transform
  h64 = vec_mergel (vt1, vzero);
  l64 = vec_mergeh (vt1, vzero);
#else
  h64 = vec_mergeh (vzero, vt1);
  l64 = vec_mergel (vzero, vt1);
#endif
  result = (vui128_t) vec_add (h64, l64);

#else
  result = vec_popcntq_PWR7 (vra);
#endif
  return result;
}

vui8_t
test_vstribl_V0 (vui8_t vra)
{
#if defined(_ARCH_PWR10)
#if ((__GNUC__ > 10) || (defined(__clang__) && (__clang_major__ > 12)))
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  return vec_strir (vra);
#else
  return vec_stril (vra);
#endif
#else
  vui8_t result;
  __asm__(
      "vstribl %0,%1;"
      : "=v" (result)
      : "v" (vra)
      : );
  return result;
#endif
#else
  const vui8_t zeros = vec_splat_u8(0);
  vui8_t nulchr, clrmask, clrcnt;

  nulchr = (vui8_t) vec_cmpeq (vra, zeros);
  // clrmask == ones unless nullchr == zeros
  clrmask = (vui8_t) vec_cmpneuq ((vui128_t) nulchr, (vui128_t) zeros);
  clrcnt = (vui8_t) vec_clzq ((vui128_t) nulchr);
  // Shift clrmask right by quadword clz of nulchar
  // leaving 0x00 bytes from byte 0 to first null char in vra
  clrmask = vec_sro (clrmask, clrcnt);
  // And compliment to clear trailing bytes after first nulchr
  return vec_andc (vra, clrmask);
#endif
}

vui128_t test_diveuq (vui128_t x, vui128_t z)
{
  return vec_vdiveuq_inline (x, z);
}

vui128_t test_divuq (vui128_t y, vui128_t z)
{
  return vec_vdivuq_inline (y, z);
}

vui128_t test_moduq (vui128_t y, vui128_t z)
{
  return vec_vmoduq_inline (y, z);
}

vui128_t
test_divduq (vui128_t x, vui128_t y, vui128_t z)
{
  return vec_divduq_inline (x, y, z);
}

vui128_t
test_modduq (vui128_t x, vui128_t y, vui128_t z)
{
  return vec_modduq_inline (x, y, z);
}

__VEC_U_128RQ
test_divdqu (vui128_t x, vui128_t y, vui128_t z)
{
  return vec_divdqu_inline (x, y, z);
}

vui128_t
test_vec_mulqud (vui128_t *mulu, vui128_t a, vui128_t b)
{
  vui128_t l128, h128;
  /* compute the 256 bit product of two 128 bit values a, b.
   * The low order DW of b are assumed 0 and ignored.
   * So effectively a 128x64-bit multiply shifted left 64-bits.
   * The high 128 bits are accumulated in t and the low 128-bits
   * in tmq. The high 128-bits of the product are returned to the
   * address of the 1st parm. The low 128-bits are the return
   * value.
   */
  vui64_t b_eud = vec_mrgahd ((vui128_t) b, (vui128_t)b);
  l128 = vec_vmuloud ((vui64_t ) a, b_eud);
  h128 = vec_vmaddeud ((vui64_t ) a, b_eud, (vui64_t ) l128);
  l128 = vec_slqi (l128, 64);

  *mulu = (vui128_t) h128;
  return ((vui128_t) l128);
}

  // The double QW {k || k1} is the produce of QWs estimated
  // Quotient (q0) and divisor (v1).
  // If Double QW {k || k1} is greater than {u1 || 0} then the
  // estimated quotient is high and should be reduced by 1
  // the remainder should be recomputed for the next step.
  // So the QW compare is (k > u1) || ((k == u1) && (k1 != 0))
  // The following is study in simplifying this compare

vui128_t test_vec_xxx (vui128_t k, vui128_t k1, vui128_t u1, vui128_t vdh, vui128_t q0)
{
  vui128_t q2, t2, t3;
  const vui64_t zeros = vec_splat_u64 (0);
  const vui128_t mone = (vui128_t) CONST_VINT128_DW(-1, -1);
  vb128_t CCgt;
  //
  // If a remainder is needed would like to integrate the compare into
  // remainder calculation.
  // double QW remainder is {u1 || 0} - {k || k1} which is:
  t3 = vec_subcuq ((vui128_t) zeros, k1);
  // u3 = vec_subuqm ((vui128_t) zeros, k1);
  t2 = vec_subecuq (u1, k, t3);
  // u2 = vec_subeuqm (u1, k, t3);
  // Technically we don't need the vec_subecuq() but the carry can be
  // used for the compare. The boolean compare vec_cmpgtuq (k, u1)
  // generates:
  //   t2 = vec_subcuq (u1, k);
  //   return vec_setb_ncq (t2);
  // So we can use the result of t2 = vec_subecuq (u1, k, t3) to set
  // the boolean CCgt via vec_setb_ncq (t2).
  // This provides a select boolean from double QW compare greater then
  // in addition to the remainder.
  // And if we only need the double QW compare,
  // the subuqm/subeuqm can be eliminated.

  // NOT carry of (u - k) -> k gt u
  CCgt = vec_setb_ncq (t2);
  // q0 = q0 -1
  q2 = vec_adduqm (q0, mone);
  q0 = vec_seluq (q0, q2, CCgt);
  // adjust remainder
  //u2 = vec_subuqm (u2, (vui128_t) vdh);
  //u1 = vec_seluq (u1, u2, CCgt);

  return q0;
}

vui128_t test_vec_xxx_V2 (vui128_t k, vui128_t k1, vui128_t u1, vui128_t q0)
{
  vui128_t q2, kc, k2;
  const vui128_t mone = (vui128_t) CONST_VINT128_DW(-1, -1);
  vb128_t CCk;
  //
  // Would like to reduce this test to a single QW compare.
  // Observe that for max unsigned QW * QW the produce is:
  // double QW {(UINT128_MAX-1), 1}
  // So, for any double QW product, we can add 0 or 1 to the high QW,
  // without overflow.
  // So a generated carry from a nonzero k1 (carry (k1+UINT128_MAX))
  // can be safely add to k without overflow.
  // And the sum of k + carry (k1+UINT128_MAX) will test greater than
  // divisor (u1), If and only if the product {k || k1} is greater than
  // the dividend {u1 || 0}
  kc = vec_addcuq (k1, mone);
  k2 = vec_adduqm (k, kc);
  CCk = vec_cmpgtuq (k2, u1);

  q2 = vec_adduqm (q0, mone);
  q0 = vec_seluq (q0, q2, CCk);

  return q0;
}

vui128_t test_vec_xxx_V1 (vui128_t k, vui128_t k1, vui128_t u1, vui128_t q0)
{
  vui128_t q2;
  const vui64_t zeros = vec_splat_u64 (0);
  const vui128_t mone = (vui128_t) CONST_VINT128_DW(-1, -1);
  vb128_t CCk, CCke, CCk1;

  CCk = vec_cmpgtuq (k, u1);
  CCke = vec_cmpequq (k, u1);
  CCk1 = vec_cmpequq (k1, (vui128_t) zeros);
  CCk1 = (vb128_t) vec_andc ((vui32_t) CCke, (vui32_t) CCk1);
  CCk = (vb128_t) vec_or ((vui32_t) CCk, (vui32_t) CCk1);

  q2 = vec_adduqm (q0, mone);
  q0 = vec_seluq (q0, q2, CCk);

  return q0;
}

vui128_t test_vec_xxx_V0 (vui128_t k, vui128_t k1, vui128_t u1, vui128_t q0)
{
  const vui64_t zeros = vec_splat_u64 (0);
  const vui128_t mone = (vui128_t) CONST_VINT128_DW(-1, -1);
  if (vec_cmpuq_all_gt (k, u1)
	  || (vec_cmpuq_all_eq (k, u1)
	      && vec_cmpuq_all_ne (k1, (vui128_t) zeros)))
	{
	  q0 = vec_adduqm (q0, mone);
	}
  return q0;
}

vui128_t test_vec_diveuq (vui128_t x, vui128_t z)
{
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  vui128_t res;
#if (__GNUC__ >= 12)
  res = vec_dive (x, z);
#else
  __asm__(
      "vdiveuq %0,%1,%2;\n"
      : "=v" (res)
      : "v" (x), "v" (z)
      : );
#endif
  return res;
#elif  (_ARCH_PWR8)
  // See "Hacker's Delight, 2nd Edition,"
  // Henry S. Warren, Jr, Addison Wesley, 2013.
  // Chapter 9, Section 9-5 Doubleword Division from Long Division.
  //
  // Here we will use long division by doubleword to compute the
  // quadword division. We use the 128 by 64 division operation
  // vec_divqud_inline() for 2 cases.
  // - divisor < 2**64
  // - divisor >= 2**64
  // This also allows the use of doubleword operations for permutes,
  // compares, and count leading zeros.
  // It does require some quadword shifts, add/subtract, and  multiply.
  // TBD explain diff from divuq and clean up if/then/else where possible
  // The difference from divuq is:
  // - The dividend is really 256-bits
  //   - with x as the high order 128-bits
  //   - with 0s implied for the low order 128-bits.
  // - The divisor is 128-bits
  // - We treat this like a 4-digit by 2 digit long division,
  //   where a digit is a 64-bit DW.
  // - Use vec_divqud_inline() to do the 2-digit by 1-digit divide step
  //   to generate a provisional quotient digit.
  // - First check for overflow (x >= z) and zero divide (z == 0).
  //   - return __UINT128_MAX__
  // - For the case; divisor < 2**64
  //   - zdh == 0 and can be ignored.
  //   - Divide x by zdl, the 1st quotient digit (qdh) is exact.
  //   - Shift the remainder left 64-bits and divide again by zdl,
  //     the 2nd quotient digit is exact.
  //   - Combine 1st/2nd quotient digits and return the 128-bit result.
  // - For the case divisor >= 2**64
  //   - So check for z == 0 first and return 0 as the quotient
  //   - We know that x < z, but we don't know if x < {zdh || 0},
  //     vec_divqud_inline can only divide by one digit (zdh) to
  //     compute 1st quotient digit estimate.
  //     - We can normalize the dividend and divisor by shifting
  //       left by vec_clzd ((vui64_t) z). This makes zdh a more
  //       reasonable divisor.
  //     - Then we use vec_divqud_inline to compute the 1st quotient
  //       digit estimate (DW qdh).
  //     - if (xdh >= zdh) the quotient (qdh) will overflow, which
  //       looks like quotient of 0 but is really (vui64_t) {1, 0}
  //     - For this case we force qdh to __UINT64_MAX__, which a
  //       reasonable estimate for the 1st quotient digit.
  //   - Use the 1st quotient digit estimate to compute the
  //     1st quotient remainder. For divide extended this requires
  //     a QW x DW multiply returning a 192-bit product (z1*qdh).
  //     This product is right justified in a double QW {k, k1}.
  //     - The remainder is the double quadword {x1, 0} - {k, k1} and
  //       the carry-out (t2==0) gives the boolean (Bgt) for
  //       double quadword {k,k1} > {x1,0}.
  //     - if ({k,k1} > {x1.0}) then the quotient estimate is too high
  //       and we need to reduce the quotient by 1 and adjust the
  //       remainder by adding double quadword (z1>>64).
  //     - The resulting remainder has a DW of leading zeros,
  //       2 x DWs of remainder, followed by another DW of zeros.
  //     - For the next step we need to normalize this by shifting this
  //       double QW left 64-bits for the 2nd stage QW dividend
  //       (using vec_sldqi()).
  //   - Now we have the corrected 1st DW (qdh) quotient digit and the
  //     corrected QW remainder.
  //     - Use vec_divqud_inline to compute the 2nd digit estimate.
  //     - Here We don't really need the remainder but we do need to
  //       verify the 2nd DW (qdl) guess is correct.
  //     - Again generate a 192-bit product (z1*qdl) right justified
  //       in a double QW {k, k1} and compute the remainder and
  //       the carry-out (t2==0) which gives the boolean (Bgt)
  //       - If ({k,k1} > {x0,0}) then subtract 1 from qdl.
  //     - Then combine the 1st and 2nd quotient digits to form the
  //       final QW quotient result.

  const vui64_t zeros = vec_splat_u64 (0);
  const vui128_t mone = (vui128_t) CONST_VINT128_DW(-1, -1);
  vui128_t x0, x1, z1, q0, k, t, zn;
  vui64_t zdh, zdl, qdl, qdh;

  // Check for overflow (x >= z) where the quotient can not be
  // represented in 128-bits, or zero divide
  if (__builtin_expect (
      vec_cmpuq_all_lt (x, z) && vec_cmpuq_all_ne (z, (vui128_t) zeros), 1))
    {
      // Check for x != 0
      if (__builtin_expect (vec_cmpuq_all_ne (x, (vui128_t) zeros), 1))
	{
	  zdh = vec_splatd ((vui64_t) z, VEC_DW_H);
	  zdl = vec_splatd ((vui64_t) z, VEC_DW_L);

          // (z < 2**64) simplifies to z >> 64 == 0UL
	  if (vec_cmpud_all_eq (zdh, zeros))
	    {
	      x0 = (vui128_t) vec_swapd ((vui64_t) x);
	      qdh = vec_divqud_inline (x0, zdl);
	      // vec_divqud already provides the remainder in qdh[1]
	      // k = x1 - q1*z; ((k << 64) + x0);
	      // Simplifies to:
	      x1 = (vui128_t) vec_pasted (qdh, (vui64_t) x0);
	      qdl = vec_divqud_inline (x1, zdl);
	      //return (vui128_t) {qlh, qdl};
	      return (vui128_t) vec_mrgald ((vui128_t) qdh, (vui128_t) qdl);
	    }
	  else
	    {
	      const vui64_t ones = vec_splat_u64 (1);
	      vui128_t k1, x2, t2, q2;
	      vb128_t Bgt;
	      vb64_t Beq;
	      // Here z >= 2**64, Normalize the divisor so MSB is 1
	      // Could use vec_clzq(), but we know  z >= 2**64, So:
	      zn = (vui128_t) vec_clzd ((vui64_t) z);
	      // zn = zn >> 64;, So we can use it with vec_slq ()
	      zn = (vui128_t) vec_mrgahd ((vui128_t) zeros, zn);

	      // Normalize dividend and divisor
	      x1 = vec_slq (x, zn);
	      z1 = vec_slq (z, zn);
	      // estimate the 1st quotient digit
	      qdh = vec_divqud_inline (x1, (vui64_t) z1);
	      // Long division with multi-digit divisor
	      // divqud by zdh might overflow the estimated quotient
              // detect overflow if ((x >> 64) == ((z >> 64)))
	      // a doubleword boolean true == __UINT64_MAX__
	      Beq = vec_cmpequd ((vui64_t) x1, (vui64_t) z1);
	      // Beq >> 64
	      Beq  = (vb64_t) vec_mrgahd ((vui128_t) zeros, (vui128_t) Beq);
	      // Adjust quotient (-1) for divide overflow
	      qdh = (vui64_t) vec_or ((vui32_t) Beq, (vui32_t) qdh);

	      // Compute 1st digit remainder
	      // {k, k1}  = vec_muludq (z1, q0);
	      // simplify to 128x64 bit product as only have 64-bit qdh
	      {
		vui128_t l128, h128;
		vui64_t b_eud = vec_mrgald ((vui128_t) qdh, (vui128_t) qdh);
		l128 = vec_vmuloud ((vui64_t ) z1, b_eud);
		h128 = vec_vmaddeud ((vui64_t ) z1, b_eud, (vui64_t ) l128);
		// 192-bit product of z1 * qdh-estimate
		k  = h128;
		k1 = vec_slqi (l128, 64);
	      }
	      // Calc double QW remainder {x1||0} - {k||k1} = {x0||x2}
	      // Also a double QW compare for {x1||0} > {k||k1}
	      x2 = vec_subuqm ((vui128_t) zeros, k1);
	      t = vec_subcuq ((vui128_t) zeros, k1);
	      x0 = vec_subeuqm (x1, k, t);
	      t2 = vec_subecuq (x1, k, t);
	      // NOT carry of (x - k) -> k gt x
	      Bgt = vec_setb_ncq (t2);
	      // Correct 1st remainder/quotient if negative
	      // Remainder will fit into 128-bits
	      x0 = vec_sldqi (x0, x2, 64);
	      x2 = vec_adduqm ((vui128_t) x0, z1);
	      x0 = vec_seluq (x0, x2, Bgt);
	      // Correct qdh estimate
	      q2 = (vui128_t) vec_subudm (qdh, ones);
	      qdh = (vui64_t) vec_seluq ((vui128_t) qdh, q2, Bgt);

	      // estimate the 2nd quotient digit
	      qdl = vec_divqud_inline (x0, (vui64_t) z1);
	      // Compute 2nd digit remainder
	      // {k, k1}  = vec_muludq (z1, qdl);
	      // simplify to 128x64 bit product with 64-bit qdl
	      x1 = x0;
	      {
		vui128_t l128, h128;
		vui64_t b_eud = vec_mrgald ((vui128_t) qdl, (vui128_t) qdl);
		l128 = vec_vmuloud ((vui64_t ) z1, b_eud);
		h128 = vec_vmaddeud ((vui64_t ) z1, b_eud, (vui64_t ) l128);
		// 192-bit product of v1 * qdl estimate
		k  = h128;
		k1 = vec_slqi (l128, 64);
	      }
	      // A double QW compare for {x1||0} > {k||k1}
	      // NOT carry of (x - k) -> k gt x
	      t = vec_subcuq ((vui128_t) zeros, k1);
	      t2 = vec_subecuq (x1, k, t);
	      Bgt = vec_setb_ncq (t2);
#if 0 // only if corrected remainder is required
	      x2 = vec_subuqm ((vui128_t) zeros, k1);
	      x0 = vec_subeuqm (x1, k, t);
	      // Remainder will fit into 128-bits
	      x0 = vec_sldqi (x0, x2, 64);
	      x2 = vec_adduqm ((vui128_t) x0, z1);
	      x0 = vec_seluq (x0, x2, Bgt);
#endif
	      // Correct combined quotient if 2nd remainder negative
	      q0 = (vui128_t) vec_mrgald ((vui128_t) qdh, (vui128_t) qdl);
	      q2 = vec_adduqm (q0, mone);
	      q0 = vec_seluq (q0, q2, Bgt);
	      return q0;
	    }
	}
      else  // if (x == 0) return 0 as Quotient
	{
	  return ((vui128_t) zeros);
	}
    }
  else
    { //  undef -- overlow or zero divide
      // If the quotient cannot be represented in 128 bits, or if
      // an attempt is made divide any value by 0
      // then the results are undefined. We use __UINT128_MAX__.
      return mone;
    }
#endif
}

vui128_t test_vec_diveuq_V0 (vui128_t x, vui128_t z)
{
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  vui128_t res;
#if (__GNUC__ >= 12)
  res = vec_dive (x, z);
#else
  __asm__(
      "vdiveuq %0,%1,%2;\n"
      : "=v" (res)
      : "v" (x), "v" (z)
      : );
#endif
  return res;
#elif  (_ARCH_PWR8)
  // See "Hacker's Delight, 2nd Edition,"
  // Henry S. Warren, Jr, Addison Wesley, 2013.
  // Chapter 9, Section 9-5 Doubleword Division from Long Division.
  //
  // Here we will use long division by doubleword to compute the
  // quadword division. We use the 128 by 64 division operation
  // vec_divqud_inline() for 2 cases.
  // - divisor < 2**64
  // - divisor >= 2**64
  // This also allows the use of doubleword operations for permutes,
  // compares, and count leading zeros.
  // It does require some quadword shifts, add/subtract, and  multiply.
  // TBD explain diff from divuq and clean up if/then/else where possible
  // The difference from divuq is:
  // - The dividend is really 256-bits
  //   - with x as the high order 128-bits
  //   - with 0s implied for the low order 128-bits.
  // - The divisor is 128-bits
  // - We treat this like a 4-digit by 2 digit long division,
  //   where a digit is a 64-bit DW.
  // - Use vec_divqud_inline() to do the 2-digit by 1-digit divide step
  //   to generate a provisional quotient digit.
  // - First check for overflow (x >= z) and zero divide (z == 0).
  //   - return __UINT128_MAX__
  // - For the case; divisor < 2**64
  //   - Divide x by zdl, the 1st quotient digit (qdh) is exact.
  //   - Shift the remainder left 64-bits and divide again by zdl,
  //     the 2nd quotient digit is exact.
  //   - Combine 1st/2nd quotient digits and return the 128-bit result.
  // - For the case divisor >= 2**64
  //   - So check for z == 0 first and return 0 as the quotient
  //   - We know that x < z, but we don't know if x < {zdh || 0},
  //     vec_divqud_inline can only divide by one digit (zdh) to
  //     compute 1st quotient digit estimate.
  //     - We can normalize the dividend and divisor by shifting
  //       left by vec_clzd ((vui64_t) z). This makes zdh a more
  //       reasonable divisor.
  //     - Then we use vec_divqud_inline to compute the 1st quotient
  //       digit estimate (DW qdh).
  //     - if (xdh >= zdh) the quotient (qdh) will overflow, which
  //       looks like quotient of 0 but is really (vui64_t) {1, 0}
  //     - For this case we force qdh to __UINT64_MAX__, which a
  //       reasonable estimate for the 1st quotient digit.
  //   - Use the 1st quotient digit estimate to compute the
  //     1st quotient remainder. For divide extended this requires
  //     a full QW x QW multiply (vec_muludq()) returning a double
  //     QW product (vui128_t) {k, k1} and double QW subtract from
  //     (vui128_t) {x1, 0} to generate the remainder.
  //     - We use subtract QW with carry/extent.
  //     - The double QW carry/borrow can used to effect the compare
  //       {k,k1} gt {x1.0}.
  //     - if ({k,k1} gt {x1.0}) then the quotient estimate was too
  //       and we need to reduce the quotient by 1 and adjust the
  //       remainder by subtracting zdh.
  //     - The resulting remainder has a DW of leading zeros,
  //       2 x DWs of remainder, followed another DW or zeros.
  //     - For the next step we need to normalize this by shifting this
  //       double QW left 64-bits for 2nd stage QW dividend
  //       (using vec_sldqi()).
  //   - Now we have the corrected 1st quotient digit and the corrected
  //     remainder.
  //     - Use vec_divqud_inline to compute the 2nd digit estimate.
  //     - Then combine the 1st and 2nd quotient digits to form the
  //       QW quotient estimate q0.
  //     - Multiply QW divisor by the QW quotient estimate and compute
  //       the double QW remainder.
  //       - Here We dont really need the remainder but we do need to
  //         verify the q0 guess is correct.
  //       - If ({k,k1} gt {x0.0}) then subtract 1 from q0 before
  //         returning the quotient.

  const vui64_t zeros = vec_splat_u64 (0);
  const vui128_t mone = (vui128_t) CONST_VINT128_DW(-1, -1);
  vui128_t x0, x1, z1, q0, k, t, zn;
  vui64_t zdh, zdl, qdl, qdh;

  // Check for overflow (x >= z) where the quotient can not be
  // represented in 128-bits, or zero divide
  if (__builtin_expect (
      vec_cmpuq_all_lt (x, z) && vec_cmpuq_all_ne (z, (vui128_t) zeros), 1))
    {
      // Check for x != 0
      if (__builtin_expect (vec_cmpuq_all_ne (x, (vui128_t) zeros), 1))
	{
	  zdh = vec_splatd ((vui64_t) z, VEC_DW_H);
	  zdl = vec_splatd ((vui64_t) z, VEC_DW_L);

          // (z < 2**64) simplifies to z >> 64 == 0UL
	  if (vec_cmpud_all_eq (zdh, zeros))
	    {
	      x0 = (vui128_t) vec_swapd ((vui64_t) x);
	      qdh = vec_divqud_inline (x0, zdl);
	      // vec_divqud already provides the remainder in qdh[1]
	      // k = x1 - q1*z; ((k << 64) + x0);
	      // Simplifies to:
	      x1 = (vui128_t) vec_pasted (qdh, (vui64_t) x0);
	      qdl = vec_divqud_inline (x1, zdl);
	      //return (vui128_t) {qlh, qdl};
	      return (vui128_t) vec_mrgald ((vui128_t) qdh, (vui128_t) qdl);
	    }
	  else
	    {
	      const vui64_t ones = vec_splat_u64 (1);
	      vui128_t k1, x2, t2, q2;
	      vb128_t Bgt;
	      vb64_t Beq;
	      // Here z >= 2**64, Normalize the divisor so MSB is 1
	      // Could use vec_clzq(), but we know  z >= 2**64, So:
	      zn = (vui128_t) vec_clzd ((vui64_t) z);
	      // zn = zn >> 64;, So we can use it with vec_slq ()
	      zn = (vui128_t) vec_mrgahd ((vui128_t) zeros, zn);

	      // Normalize dividend and divisor
	      x1 = vec_slq (x, zn);
	      z1 = vec_slq (z, zn);
#if 1
#else
	      xdh = vec_mrgahd ((vui128_t) zeros, x1); // !!
	      zdh = vec_mrgahd ((vui128_t) zeros, z1); // !!
#endif
	      // estimate the quotient 1st digit
	      qdh = vec_divqud_inline (x1, (vui64_t) z1);
#if 1
              // detect overflow if ((x >> 64) == ((z >> 64)))
	      // a doubleword boolean true == __UINT64_MAX__
	      Beq = vec_cmpequd ((vui64_t) x1, (vui64_t) z1);
	      // Beq >> 64
	      Beq  = (vb64_t) vec_mrgahd ((vui128_t) zeros, (vui128_t) Beq);
	      // Adjust quotient (-1) for divide overflow
	      qdh = (vui64_t) vec_or ((vui32_t) Beq, (vui32_t) qdh);
#else
	      // divqud might overflow the estimated quotient
	      // If X != 0 then quotient == 0 implies overflow
	      // If so use UINT64_MAX as the estimated quotient
	      Beq = vec_cmpequd (qdh, zeros);
	      Beq2 = vec_cmpequd (xdh, zdh); // !!
	      Beq = (vb64_t) vec_and ((vui32_t) Beq, (vui32_t) Beq2);  // !!
	      qdh = vec_selud (qdh, (vui64_t) mone, Beq);
#endif
	      // q0 = qdh << 64
	      q0 = (vui128_t) vec_mrgald ((vui128_t) qdh, (vui128_t) zeros);

	      // Compute 1st digit remainder
	      // {k, k1}  = vec_muludq (z1, q0);
#if 1
	      {
		vui128_t l128, h128;
		vui64_t b_eud = vec_mrgald ((vui128_t) qdh, (vui128_t) qdh);
		l128 = vec_vmuloud ((vui64_t ) z1, b_eud);
		h128 = vec_vmaddeud ((vui64_t ) z1, b_eud, (vui64_t ) l128);
		// 192-bit product of v1 * q-estimate
		k  = h128;
		k1 = vec_slqi (l128, 64);
	      }
#else
	      k1 = vec_muludq (&k, z1, q0);
#endif
	      // Also a double QW compare for {x1 || 0} > {k || k1}
	      x2 = vec_subuqm ((vui128_t) zeros, k1);
	      t = vec_subcuq ((vui128_t) zeros, k1);
	      x0 = vec_subeuqm (x1, k, t);
	      t2 = vec_subecuq (x1, k, t);
	      // NOT carry of (x - k) -> k gt x
	      Bgt = vec_setb_ncq (t2);

	      x0 = vec_sldqi (x0, x2, 64);
	      q2 = (vui128_t) vec_subudm ((vui64_t) q0, ones);
	      //t2 = vec_subuqm (x0, (vui128_t) zdh);
	      x2 = vec_adduqm ((vui128_t) x0, z1);
	      q0 = vec_seluq (q0, q2, Bgt);
	      x0 = vec_seluq (x0, x2, Bgt);

	      qdh = (vui64_t) vec_mrgahd ((vui128_t) zeros, (vui128_t) q0);
	      //x0 = vec_sldqi (x0, x2, 64);

	      qdl = vec_divqud_inline (x0, (vui64_t) z1);
	      q0 = (vui128_t) vec_mrgald ((vui128_t) qdh, (vui128_t) qdl);
	      k1 = vec_muludq (&k, q0, z1);
	      // NOT carry of (x - k) -> k gt x
	      t = vec_subcuq ((vui128_t) zeros, k1);
	      //x2 = vec_subuqm ((vui128_t) zeros, k1);
	      t2 = vec_subecuq (x1, k, t);
	      //x0 = vec_subeuqm (x1, k, t);
	      Bgt = vec_setb_ncq (t2);
	      q2 = vec_adduqm (q0, mone);
	      q0 = vec_seluq (q0, q2, Bgt);
	      return q0;
	    }
	}
      else  // if (x == 0) return 0 as Quotient
	{
	  return ((vui128_t) zeros);
	}
    }
  else
    { //  undef -- overlow or zero divide
      // If the quotient cannot be represented in 128 bits, or if
      // an attempt is made divide any value by 0
      // then the results are undefined. We use __UINT128_MAX__.
      return mone;
    }
#else
/* Based on Hacker's Delight (2nd Edition) Figure 9-2.
 * "Divide long unsigned shift-and-subtract algorithm."
 * Simplified for y = 0 case. Carry from y to x
 * is not possible so; eliminate the vec_addcuq (y, y) and replace
 * vec_addeuqm (x, x, c) with vec_adduqm (x, x).
 * As cmpgeuq is based on detecting the carry-out of (x - z) and
 * setting the bool via setb_cyq, we can use this carry (variable t)
 * to generate quotient bits.
 * */
  int i;
  vb128_t ge;
  vui128_t t, cc, xt;
  vui128_t y = {(__int128) 0};

  for (i = 1; i <= 128; i++)
    {
      // Left shift (x || 0) requires 129-bits, is (t || x || 0 )
      t = vec_addcuq (x, x);
      x = vec_adduqm (x, x);

      // deconstruct ((t || x) >= z)
      // Carry from subcuq == 1 for x >= z
      cc = vec_subcuq (x, z);
      // Combine t with (x >= z) for 129-bit compare
      t  = (vui128_t) vec_or ((vui32_t)cc, (vui32_t)t);
      // Convert to a bool for select
      ge = vec_setb_cyq (t);

      xt = vec_subuqm (x, z);
      y = vec_addeuqm (y, y, t);
      x = vec_seluq (x, xt, ge);
    }
  return y;
#endif
}

/* Based on Hacker's Delight (2nd Edition) Figure 9-2.
 * "Divide long unsigned shift-and-subtract algorithm."
 * Simplified for x = 0 case. Carry out of high order (257th bit)
 * is not possible so the compare becomes x >= z.
 * As cmpgeuq is based on detecting the carry-out of (x -z) and
 * setting the bool via setb_cyq, we can use this carry (variable t)
 * to generate quotient bits (variable y).
 * */
vui128_t test_vec_divuq (vui128_t y, vui128_t z)
{
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  vui128_t res;
#if (__GNUC__ >= 12)
  res = vec_div (y, z);
#else
  __asm__(
      "vdivuq %0,%1,%2;\n"
      : "=v" (res)
      : "v" (y), "v" (z)
      : );
#endif
  return res;
#elif (_ARCH_PWR8)
 // See "Hacker's Delight, 2nd Edition,"
 // Henry S. Warren, Jr, Addison Wesley, 2013.
 // Chapter 9, Section 9-5 Doubleword Division from Long Division.
 //
 // Here we will use long division by doubleword to compute the
 // quadword division. We use the 128 by 64 division operation
 // vec_divqud_inline() for 3 distinct cases.
 // - divisor < 2**64 and
 //   - dividend < 2**64
 //   - dividend >= 2**64
 // - divisor >= 2**64
 // This also allows the use of doubleword operations for permutes,
 // compares, and count leading zeros.
 // It does require some quadword shifts, add/subtract, and in one case
 // multiply.
  const vui64_t zeros = vec_splat_u64(0);
  const vui128_t mone = (vui128_t) CONST_VINT128_DW (-1, -1);
  vui128_t y0, y1, z1, q0, q1, k, t, zn;
  vui64_t zdh, zdl, ydh, qdl, qdh;

  ydh = vec_splatd((vui64_t)y, VEC_DW_H);
  zdh = vec_splatd((vui64_t)z, VEC_DW_H);
  zdl = vec_splatd((vui64_t)z, VEC_DW_L);

  if (vec_cmpud_all_eq (zdh, zeros)) // (z >> 64) == 0UL
    {
      if (vec_cmpud_all_lt (ydh, zdl)) // (y >> 64) < z
	{
	  // Here qdl = {(y % z) || (y / z)}
	  qdl = vec_divqud_inline (y, zdl);
	  // return the quotient
	  return (vui128_t) vec_mrgald ((vui128_t) zeros, (vui128_t) qdl);
	}
      else
	{
	  //y1 = y >> 64;
	  y1 = (vui128_t) vec_mrgahd ((vui128_t) zeros, y);
	  // y0 = y & lmask;
	  y0 = (vui128_t) vec_mrgald ((vui128_t) zeros, y);
	  //q1 = scalar_divqud (y1, (unsigned long long) z) & lmask;
	  // Here qdh = {(y1 % z) || (y1 / z)}
	  qdh = vec_divqud_inline (y1, zdl);
	  // vec_divqud already provides the remainder in qdh[1]
	  // So; k = y1 - q1*z; ((k << 64) + y0);
	  // Simplifies to:
	  k = (vui128_t) vec_pasted (qdh, (vui64_t) y0);
	  // q0 = scalar_divqud ((k << 64) + y0, (unsigned long long) z) & lmask;
	  // Here qdl = (vui64_t) {(k % z) || (k / z)}
	  qdl = vec_divqud_inline (k, zdl);
	  //return (q1 << 64) + q0;
	  return (vui128_t) vec_mrgald ((vui128_t) qdh, (vui128_t) qdl);
	}
    }
  else
    {
      // Here z >= 2**64, Normalize the divisor so MSB is 1
      // Could use vec_clzq(), but we know  z >= 2**64, So:
      zn = (vui128_t) vec_clzd ((vui64_t) z);
      // zn = zn >> 64, So we can use it with vec_slq ()
      zn = (vui128_t) vec_mrgahd ((vui128_t) zeros, zn);
      //z1 = (z << n) >> 64;
      z1 = vec_slq (z, zn);

      //y1 = y >> 1; 	// to insure no overflow
      y1 = vec_srqi (y, 1);
      // q1 = scalar_divdud (y1, (unsigned long long) z1) & lmask;
      qdl = vec_divqud_inline (y1, (vui64_t) z1);
      q1 = (vui128_t) vec_mrgald ((vui128_t) zeros, (vui128_t) qdl);
      // Undo normalization and y/2.
      //q0 = (q1 << n) >> 63;
      q0 = vec_slq (q1, zn);
      q0 = vec_srqi (q0, 63);

      // if (q0 != 0) q0 = q0 - 1;
	{
	  vb128_t QB;
	  QB = vec_cmpequq (q0, (vui128_t) zeros);
	  q1 = vec_adduqm (q0, mone);
	  q0 = vec_seluq (q1, q0, QB);
	}
      t = vec_mulluq (q0, z);
      t = vec_subuqm (y, t);
      // if ((y - q0*z) >= z) q0 = q0 + 1;
	{
	  vb128_t QB;
	  QB = vec_cmpgtuq (z, t);
	  q1 = vec_subuqm (q0, mone);
	  q0 = vec_seluq (q1, q0, QB);
	}
      return q0;
    }
#else
  int i;
  vb128_t ge;
  vui128_t c, t, xt;
  vui128_t x = {(__int128) 0};

  for (i = 1; i <= 128; i++)
    {
      // 256-bit shift left
      c = vec_addcuq (y, y);
      x = vec_addeuqm (x, x, c);

      // Deconstruct QW cmpge to extract the carry
      t  = vec_subcuq (x, z);
      ge = vec_cmpgeuq (x, z);

      // xt = x - z, for select x >= z
      xt = vec_subuqm (x, z);
      // left shift y || t
      y = vec_addeuqm (y, y, t);
      // if (x >= z) x = x - z
      x = vec_seluq (x, xt, ge);
    }
  return y;
#endif
}

vui128_t test_vec_divduq (vui128_t x, vui128_t y, vui128_t z)
{
#if defined (_ARCH_PWR8)
  vui128_t Q, R;
  vui128_t r1, r2, q1, q2;
  vb128_t CC, c1, c2;

  // Based on the PowerISA, Programming Note for
  // Divide Word Extended [Unsigned] but vectorized
  // for vector __int128
  q1 = test_vec_diveuq (x, z);
  q2 = test_vec_divuq  (y, z);
  r1 = vec_mulluq (q1, z);

  r2 = vec_mulluq (q2, z);
  r2 = vec_subuqm (y, r2);
  Q  = vec_adduqm (q1, q2);
  R  = vec_subuqm (r2, r1);

  c1 = vec_cmpltuq (R, r2);
#if defined (_ARCH_PWR8) // vorc requires P8
  c2 = vec_cmpgtuq (z, R);
  CC = (vb128_t) vec_orc ((vb32_t)c1, (vb32_t)c2);
#else
  c2 = vec_cmpgeuq (R, z);
  CC = (vb128_t) vec_or ((vb32_t)c1, (vb32_t)c2);
#endif
#if 1 // Corrected Quotient returned for divduq.
#if 1
  // if Q needs correction (Q+1), Bool CC is True, which is -1
  Q = vec_subuqm (Q, (vui128_t) CC);
#else
  vui128_t Qt;
  Qt = vec_adduqm (Q, ones);
  Q = vec_seluq (Q, Qt, CC);
#endif
  return Q;
#else // Corrected Remainder not returned for divduq.
  vui128_t Rt;
  Rt = vec_subuqm (R, z);
  R = vec_seluq (R, Rt, CC);
  return R;
#endif
#else
  /* Based on Hacker's Delight (2nd Edition) Figure 9-2.
   * "Divide long unsigned shift-and-subtract algorithm."
   * Converted to use vector unsigned __int128 and PVEClIB
   * operations.
   * As cmpgeuq is based on detecting the carry-out of (x -z) and
   * setting the bool via setb_cyq, we can use this carry (variable t)
   * to generate quotient bits.
   * Multi-precision shift-left is simpler then general addition,
   * so we can simplify carry generation. This allows delaying the
   * the y left-shift / quotient accumulation to a later.
   * */
  int i;
  vb128_t ge;
  vui128_t t, cc, c, xt;
  //t = (vui128_t) CONST_VINT128_W (0, 0, 0, 0);

  for (i = 1; i <= 128; i++)
    {
      // Left shift (x || y) requires 257-bits, is (t || x || y)
      c = vec_addcuq (y, y);
      t = vec_addcuq (x, x);
      x = vec_addeuqm (x, x, c);

      // deconstruct ((t || x) >= z) to ((x >= z) || t), then
      // deconstruct vec_cmpgeuq() to vec_subcuq and vec_setb_cyq ()
      // If (x >= z) cc == 1
      cc = vec_subcuq (x, z);
      // Combine t with (x >= z) for 129-bit compare
      t  = (vui128_t) vec_or ((vui32_t)cc, (vui32_t)t);
      // Convert t to a 128-bit bool for select
      ge = vec_setb_cyq (t);

      xt = vec_subuqm (x, z);
      // Delay the shift left of y to here so we can conveniently shift
      // t into the low order bits to accumulate the quotient.
      y = vec_addeuqm (y, y, t);
      x = vec_seluq (x, xt, ge);
    }
  return y;
#endif
}

__VEC_U_128RQ test_vec_divdqu (vui128_t x, vui128_t y, vui128_t z)
{
  __VEC_U_128RQ result;
#if defined (_ARCH_PWR8)
  vui128_t Q, R;
  vui128_t r1, r2, q1, q2;
  vb128_t CC, c1, c2;
  const vui128_t ones = {(__int128) 1};

  // Based on the PowerISA, Programming Note for
  // Divide Word Extended [Unsigned] but vectorized
  // for vector __int128
  q1 = test_vec_diveuq (x, z);
  q2 = test_vec_divuq  (y, z);
  r1 = vec_mulluq (q1, z);

  r2 = vec_mulluq (q2, z);
  r2 = vec_subuqm (y, r2);
  Q  = vec_adduqm (q1, q2);
  R  = vec_subuqm (r2, r1);

  c1 = vec_cmpltuq (R, r2);
#if defined (_ARCH_PWR8) // vorc requires P8
  c2 = vec_cmpgtuq (z, R);
  CC = (vb128_t) vec_orc ((vb32_t)c1, (vb32_t)c2);
#else
  c2 = vec_cmpgeuq (R, z);
  CC = (vb128_t) vec_or ((vb32_t)c1, (vb32_t)c2);
#endif
// Corrected Quotient returned for divduq.
  vui128_t Qt;
  Qt = vec_adduqm (Q, ones);
  Q = vec_seluq (Q, Qt, CC);
  result.Q = Q;
// Corrected Remainder not returned for divduq.
  vui128_t Rt;
  Rt = vec_subuqm (R, z);
  R = vec_seluq (R, Rt, CC);
  result.R = R;
#else
  /* Based on Hacker's Delight (2nd Edition) Figure 9-2.
   * "Divide long unsigned shift-and-subtract algorithm."
   * Converted to use vector unsigned __int128 and PVEClIB
   * operations.
   * As cmpgeuq is based on detecting the carry-out of (x -z) and
   * setting the bool via setb_cyq, we can use this carry (variable t)
   * to generate quotient bits.
   * Multi-precision shift-left is simpler then general addition,
   * so we can simplify carry generation. This allows delaying the
   * the y left-shift / quotient accumulation to a later.
   * */
  int i;
  vb128_t ge;
  vui128_t t, cc, c, xt;
  //t = (vui128_t) CONST_VINT128_W (0, 0, 0, 0);

  for (i = 1; i <= 128; i++)
    {
      // Left shift (x || y) requires 257-bits, is (t || x || y)
      // t from previous iteration generates to 0/1 for low order y-bits.
      c = vec_addcuq (y, y);
      t = vec_addcuq (x, x);
      x = vec_addeuqm (x, x, c);

      // deconstruct ((t || x) >= z)
      // Carry from subcuq == 1 for x >= z
      cc = vec_subcuq (x, z);
      // Combine t with (x >= z) for 129-bit compare
      t  = (vui128_t) vec_or ((vui32_t)cc, (vui32_t)t);
      // Convert to a bool for select
      ge = vec_setb_cyq (t);

      xt = vec_subuqm (x, z);
      y = vec_addeuqm (y, y, t);
      x = vec_seluq (x, xt, ge);
    }
  result.Q = y; // Q
  result.R = x; // R
#endif
  return result;
}

/*
 * Here we will use long division by doubleword to compute the
 * quadword division. We use the 128 by 64 division operation
 * vec_divqud_inline() for 3 distinct cases.
 * - divisor < 2**64 and
 *   - dividend < 2**64
 *   - dividend >= 2**64
 * - divisor >= 2**64
 * This also allows the use of doubleword operations for permutes,
 * compares, and count leading zeros.
 * I does require some quadword shifts, add/subtrac, and in one case
 * multiply.
* \see "Hacker's Delight, 2nd Edition,"
* Henry S. Warren, Jr, Addison Wesley, 2013.
* Chapter 9, Section 9-5 Doubleword Division from Long Division.
*/
//#define __DEBUG_PRINT__
vui128_t test_vec_udivqi3 (vui128_t u, vui128_t v)
{
  const vui64_t zeros = vec_splat_u64(0);
  const vui128_t mone = (vui128_t) CONST_VINT128_DW (-1, -1);
  vui128_t u0, u1, v1, q0, q1, k, t, vn;
  vui64_t vdh, vdl, udh, qdl, qdh;

  udh = vec_splatd((vui64_t)u, VEC_DW_H);
  vdh = vec_splatd((vui64_t)v, VEC_DW_H);
  vdl = vec_splatd((vui64_t)v, VEC_DW_L);

  if (/*v >> 64 == 0UL*/ vec_cmpud_all_eq(vdh, zeros))
    {
      if (/*u >> 64 < v*/ vec_cmpud_all_lt(udh, vdl))
	{

	  qdl = vec_divqud_inline (u, vdl);
	  return (vui128_t) vec_mrgald((vui128_t)zeros, (vui128_t)qdl);
	}
      else {
	  //u1 = u >> 64;
	  u1 = (vui128_t) vec_mrgahd ((vui128_t)zeros, u);
	  // u0 = u & lmask;
	  u0 = (vui128_t) vec_mrgald ((vui128_t)zeros, u);
	  //q1 = scalar_divdud (u1, (unsigned long long) v) & lmask;
	  qdh = vec_divqud_inline (u1, vdl);
	  // vec_divqud already provides the remainder in qdh[1]
	  // So; k = u1 - q1*v; ((k << 64) + u0);
	  // Simplifies to:
	  k = (vui128_t) vec_pasted (qdh, (vui64_t) u0);
	  // q0 = scalar_divdud ((k << 64) + u0, (unsigned long long) v) & lmask;
	  qdl = vec_divqud_inline (k, vdl);
	  //return (q1 << 64) + q0;
	  return (vui128_t) vec_mrgald((vui128_t) qdh, (vui128_t) qdl);
      }
    }
  // Here v >= 2**64, Normalize the divisor so MSB is 1
  //n = __builtin_clzl ((unsigned long long)(v >> 64)); // 0 <= n <= 63
#if 1
  // Could use vec_clzq(), but we know  v >= 2**64, So:
  vn = (vui128_t) vec_clzd((vui64_t) v);
  // vn = vn >> 64, So we can use it with vec_slq ()
  vn = (vui128_t) vec_mrgahd ((vui128_t)zeros, vn);
#else
#endif
  //v1 = (v << n) >> 64;
  v1 = vec_slq (v, vn);

  //u1 = u >> 1; 	// to insure no overflow
  u1 = vec_srqi (u, 1);
  // q1 = scalar_divdud (u1, (unsigned long long) v1) & lmask;
  qdl = vec_divqud_inline (u1, (vui64_t) v1);
  q1  = (vui128_t) vec_mrgald((vui128_t)zeros, (vui128_t)qdl);
  // Undo normalization and u/2.
  //q0 = (q1 << n) >> 63;
  q0  = vec_slq (q1, vn);
  q0  = vec_srqi(q0, 63);

  if (/*q0 != 0*/ vec_cmpuq_all_ne(q0, (vui128_t)zeros))
    {
      //q0 = q0 - 1;
      q0 = vec_adduqm (q0, mone);
    }
  t = vec_mulluq(q0, v);
  t = vec_subuqm (u, t);
  /* (u - q0*v) >= v */
  if (vec_cmpuq_all_ge(t, v))
    {
      //q0 = q0 + 1;
      q0 = vec_subuqm (q0, mone);
    }
  return q0;
}

/* Based on Hacker's Delight (2nd Edition) Figure 9-2.
 * "Divide long unsigned shift-and-subtract algorithm."
 * Simplified for x = 0 case. Carry out of high order (257th bit)
 * is not possible so the compare becomes x >= z.
 * As cmpgeuq is based on detecting the carry-out of (x -z) and
 * setting the bool via setb_cyq. But we don't return the quotient and
 * so don't need to shift t into low order y.
 * */
vui128_t test_vec_moduq (vui128_t y, vui128_t z)
{
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  vui128_t res;
#if (__GNUC__ >= 12)
  res = vec_mod (y, z);
#else
  __asm__(
      "vmoduq %0,%1,%2;\n"
      : "=v" (res)
      : "v" (y), "v" (z)
      : );
#endif
  return res;
#elif defined (_ARCH_PWR7)
#if 1
  // inspired by:
  // See "Hacker's Delight, 2nd Edition,"
  // Henry S. Warren, Jr, Addison Wesley, 2013.
  // Chapter 9, Section 9-5 Doubleword Division from Long Division.
  // basically perform the long division as in vec_divuq but return
  // the remainder.
   const vui64_t zeros = vec_splat_u64(0);
   const vui128_t mone = (vui128_t) CONST_VINT128_DW (-1, -1);
   vui128_t y0, y1, z1, r0, q0, q1, k, t, zn;
   vui64_t zdh, zdl, ydh, qdl, qdh;

   ydh = vec_splatd((vui64_t)y, VEC_DW_H);
   zdh = vec_splatd((vui64_t)z, VEC_DW_H);
   zdl = vec_splatd((vui64_t)z, VEC_DW_L);

   if (vec_cmpud_all_eq (zdh, zeros)) // (z >> 64) == 0UL
     {
       if (vec_cmpud_all_lt (ydh, zdl)) // (y >> 64) < z
 	{
 	  // Here qdl = {(y % z) || (y / z)}
 	  qdl = vec_divqud_inline (y, zdl);
 	  // return the quotient
 	  //return (vui128_t) vec_mrgald ((vui128_t) zeros, (vui128_t) qdl);
 	  // return the remainder
	  return (vui128_t) vec_mrgahd((vui128_t)zeros, (vui128_t)qdl);
 	}
       else
 	{
 	  //y1 = y >> 64;
 	  y1 = (vui128_t) vec_mrgahd ((vui128_t) zeros, y);
 	  // y0 = y & lmask;
 	  y0 = (vui128_t) vec_mrgald ((vui128_t) zeros, y);
 	  //q1 = scalar_divqud (y1, (unsigned long long) z) & lmask;
 	  // Here qdh = {(y1 % z) || (y1 / z)}
 	  qdh = vec_divqud_inline (y1, zdl);
 	  // vec_divqud already provides the remainder in qdh[1]
 	  // So; k = y1 - q1*z; ((k << 64) + y0);
 	  // Simplifies to:
 	  k = (vui128_t) vec_pasted (qdh, (vui64_t) y0);
 	  // q0 = scalar_divqud ((k << 64) + y0, (unsigned long long) z) & lmask;
 	  // Here qdl = (vui64_t) {(k % z) || (k / z)}
 	  qdl = vec_divqud_inline (k, zdl);
 	  //return (q1 << 64) + q0;
 	  //return (vui128_t) vec_mrgald ((vui128_t) qdh, (vui128_t) qdl);
 	  // return the remainder
	  return (vui128_t) vec_mrgahd((vui128_t)zeros, (vui128_t)qdl);
 	}
     }
   else
     {
       // Here z >= 2**64, Normalize the divisor so MSB is 1
       // Could use vec_clzq(), but we know  z >= 2**64, So:
       zn = (vui128_t) vec_clzd ((vui64_t) z);
       // zn = zn >> 64, So we can use it with vec_slq ()
       zn = (vui128_t) vec_mrgahd ((vui128_t) zeros, zn);
       //z1 = (z << n) >> 64;
       z1 = vec_slq (z, zn);

       //y1 = y >> 1; 	// to insure no overflow
       y1 = vec_srqi (y, 1);
       // q1 = scalar_divdud (y1, (unsigned long long) z1) & lmask;
       qdl = vec_divqud_inline (y1, (vui64_t) z1);
       q1 = (vui128_t) vec_mrgald ((vui128_t) zeros, (vui128_t) qdl);
       // Undo normalization and y/2.
       //q0 = (q1 << n) >> 63;
       q0 = vec_slq (q1, zn);
       q0 = vec_srqi (q0, 63);

       // if (q0 != 0) q0 = q0 - 1;
 	{
 	  vb128_t QB;
 	  QB = vec_cmpequq (q0, (vui128_t) zeros);
 	  q1 = vec_adduqm (q0, mone);
 	  q0 = vec_seluq (q1, q0, QB);
 	}
       t = vec_mulluq (q0, z);
       r0 = vec_subuqm (y, t);
       // if ((y - q0*z) >= z) q0 = q0 + 1;
 	{
 	  vb128_t QB;
 	  QB = vec_cmpgtuq (z, r0);
 	  //q1 = vec_subuqm (q0, mone);
 	  //q0 = vec_seluq (q1, q0, QB);
 	  t  = vec_subuqm (r0, z);
 	  r0 = vec_seluq (t, r0, QB);
 	}
       return r0;
     }
#else
  vui128_t R;
  vui128_t r2, q2;
  const vui128_t ones = {(__int128) 1};
  q2 = test_vec_divuq  (y, z);

  r2 = vec_mulluq (q2, z);
  R  = vec_subuqm (y, r2);
  return R;
#endif
#else
  int i;
  vb128_t ge;
  vui128_t c, xt;
  vui128_t x = {(__int128) 0};

  for (i = 1; i <= 128; i++)
    {
      // 256-bit shift left
      c = vec_addcuq (y, y);
      y = vec_adduqm (y, y);
      x = vec_addeuqm (x, x, c);
      ge = vec_cmpgeuq (x, z);

      // xt = x - z, for select x >= z
      xt = vec_subuqm (x, z);
      // if (x >= z) x = x - z
      x = vec_seluq (x, xt, ge);
    }
  return x;
#endif
}

vui128_t test_vec_modduq (vui128_t x, vui128_t y, vui128_t z)
{
#if defined (_ARCH_PWR8)
  vui128_t R;
  vui128_t r1, r2, q1, q2;
  vb128_t CC, c1, c2;

  // Based on the PowerISA, Programming Note for
  // Divide Word Extended [Unsigned] but vectorized
  // for vector  __int128
  q1 = test_vec_diveuq (x, z);
  q2 = test_vec_divuq  (y, z);
  r1 = vec_mulluq (q1, z);

  r2 = vec_mulluq (q2, z);
  r2 = vec_subuqm (y, r2);
  R  = vec_subuqm (r2, r1);

  c1 = vec_cmpltuq (R, r2);
#if defined (_ARCH_PWR8) // vorc requires P8
  c2 = vec_cmpgtuq (z, R);
  CC = (vb128_t) vec_orc ((vb32_t)c1, (vb32_t)c2);
#else
  c2 = vec_cmpgeuq (R, z);
  CC = (vb128_t) vec_or ((vb32_t)c1, (vb32_t)c2);
#endif
#if 0 // Corrected Quotient not returned for modduq.
  vui128_t Q, Qt;
  const vui128_t ones = {(__int128) 1};
  Q  = vec_adduqm (q1, q2);
  Qt = vec_adduqm (Q, ones);
  Q = vec_seluq (Q, Qt, CC);
  return Q;
#else // Corrected Remainder returned for modduq.
  vui128_t Rt;
  Rt = vec_subuqm (R, z);
  R = vec_seluq (R, Rt, CC);
  return R;
#endif
#else // defined (_ARCH_PWR7)
  int i;
  vb128_t ge;
  vui128_t t, cc, c, xt;
  //t = (vui128_t) CONST_VINT128_W (0, 0, 0, 0);

  for (i = 1; i <= 128; i++)
    {
      // Left shift (x || y) requires 257-bits, is (t || x || y)
      // t from previous iteration generates to 0/1 for low order y-bits.
      c = vec_addcuq (y, y);
      t = vec_addcuq (x, x);
      x = vec_addeuqm (x, x, c);

      // deconstruct ((t || x) >= z)
      // Carry from subcuq == 1 for x >= z
      cc = vec_subcuq (x, z);
      // Combine t with (x >= z) for 129-bit compare
      t  = (vui128_t) vec_or ((vui32_t)cc, (vui32_t)t);
      // Convert to a bool for select
      ge = vec_setb_cyq (t);

      xt = vec_subuqm (x, z);
      y = vec_addeuqm (y, y, t);
      x = vec_seluq (x, xt, ge);
    }
  return x;
#endif
}

// Attempts at better code to splat small QW constants.
// Want to avoid addr calc and loads for what should be simple
// splat immediate and sld.

vi128_t
__test_splatisq_256 (void)
{
  return vec_splat_s128 (-256);
}

vi128_t
__test_splatisq_192 (void)
{
  return vec_splat_s128 (-192);
}

vi128_t
__test_splatisq_128 (void)
{
  return vec_splat_s128 (-128);
}

vi128_t
__test_splatisq_96 (void)
{
  return vec_splat_s128 (-96);
}

vi128_t
__test_splatisq_64 (void)
{
  return vec_splat_s128 (-64);
}

vi128_t
__test_splatisq_32 (void)
{
  return vec_splat_s128 (-32);
}

vi128_t
__test_splatisq_33 (void)
{
  return vec_splat_s128 (-33);
}

vi128_t
__test_splatisq_24 (void)
{
  return vec_splat_s128 (-24);
}

vi128_t
__test_splatisq_16 (void)
{
  return vec_splat_s128 (-16);
}

vi128_t
__test_splatisq_n1 (void)
{
  return vec_splat_s128 (-1);
}

vi128_t
__test_splatisq_0 (void)
{
  return vec_splat_s128 (0);
}

vi128_t
__test_splatisq_15 (void)
{
  return vec_splat_s128 (15);
}

vi128_t
__test_splatisq_27 (void)
{
  return vec_splat_s128 (27);
}

vi128_t
__test_splatisq_31 (void)
{
  return vec_splat_s128 (31);
}

vi128_t
__test_splatisq_p32 (void)
{
  return vec_splat_s128 (32);
}

vi128_t
__test_splatisq_p33 (void)
{
  return vec_splat_s128 (33);
}

vi128_t
__test_splatisq_p56 (void)
{
  return vec_splat_s128 (56);
}

vi128_t
__test_splatisq_p60 (void)
{
  return vec_splat_s128 (60);
}

vi128_t
__test_splatisq_p63 (void)
{
  return vec_splat_s128 (63);
}

vi128_t
__test_splatisq_p64 (void)
{
  return vec_splat_s128 (64);
}

vi128_t
__test_splatisq_127 (void)
{
  return vec_splat_s128 (127);
}

vi128_t
__test_splatisq_p128 (void)
{
  return vec_splat_s128 (128);
}

vi128_t
__test_splatisq_p192 (void)
{
  return vec_splat_s128 (192);
}

vi128_t
__test_splatisq_p255 (void)
{
  return vec_splat_s128 (255);
}

vi128_t
__test_splatisq_n1_V1 (void)
{
  const vui64_t q_ones = {-1, -1};
  return (vi128_t) q_ones;
}

vi128_t
__test_splatisq_n1_V0 (void)
{
  const vui32_t q_ones = {-1, -1, -1, -1};
  return (vi128_t) q_ones;
}

vi128_t
__test_splatisq_0_V0 (void)
{
  const vui32_t q_zero = {0, 0, 0, 0};
  return (vi128_t) q_zero;
}

vui32_t
__test_splatisq_signmask_V0 (void)
{
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  return signmask;
}

vi128_t
__test_splatisq_15_V2 (void)
{
  //  const vui32_t qw_15 = CONST_VINT128_W(0, 0, 0, 15);
  const vui32_t q_zero = CONST_VINT128_W (0, 0, 0, 0);
  vui32_t qw_15 = (vui32_t) vec_splat_s32(15);
  return (vi128_t) vec_sld (q_zero, qw_15, 4);
}

vi128_t
__test_splatisq_15_V1 (void)
{
  const vui128_t qw_15 = {15};
  return (vi128_t) qw_15;
}

vi128_t
__test_splatisq_15_V0 (void)
{
  const vui32_t qw_15 = CONST_VINT128_W(0, 0, 0, 15);
  return (vi128_t) qw_15;
}

vui128_t
__test_splatiuq_0 (void)
{
  return vec_splat_u128 (0);
}

vui128_t
__test_splatiuq_15 (void)
{
  return vec_splat_u128 (15);
}

vui128_t
__test_splatiuq_16 (void)
{
  return vec_splat_u128 (16);
}

vui128_t
__test_splatiuq_20 (void)
{
  return vec_splat_u128 (20);
}

vui128_t
__test_splatiuq_24 (void)
{
  return vec_splat_u128 (24);
}

vui128_t
__test_splatiuq_29 (void)
{
  return vec_splat_u128 (29);
}

vui128_t
__test_splatiuq_30 (void)
{
  return vec_splat_u128 (30);
}

vui128_t
__test_splatiuq_31 (void)
{
  return vec_splat_u128 (31);
}

vui128_t
__test_splatiuq_31_V1 (void)
{
  const int sim = 0x1f;
  // latency PWR8 6-8
  const vui32_t q_zero = CONST_VINT128_W (0, 0, 0, 0);
  const vui32_t q_ones = CONST_VINT128_W (-1, -1, -1, -1);
  vui32_t tmp = vec_srwi (q_ones, (32-5));
  return (vui128_t) vec_sld (q_zero, tmp, 4);
}

vui128_t
__test_splatiuq_32 (void)
{
  return vec_splat_u128 (32);
}

vui128_t
__test_splatiuq_33 (void)
{
  return vec_splat_u128 (33);
}

vui128_t
__test_splatiuq_48 (void)
{
  return vec_splat_u128 (48);
}

vui128_t
__test_splatiuq_55 (void)
{
  return vec_splat_u128 (55);
}

vui128_t
__test_splatiuq_56 (void)
{
  return vec_splat_u128 (56);
}

vui128_t
__test_splatiuq_60 (void)
{
  return vec_splat_u128 (60);
}

vui128_t
__test_splatiuq_63 (void)
{
  return vec_splat_u128 (63);
}

vui128_t
__test_splatiuq_63_V1 (void)
{
  const int sim = 0x3f;
  // latency PWR8 6-8
  const vui32_t q_zero = CONST_VINT128_W (0, 0, 0, 0);
  const vui32_t q_ones = CONST_VINT128_W (-1, -1, -1, -1);
  vui32_t tmp = vec_srwi (q_ones, (32-6));
  return (vui128_t) vec_sld (q_zero, tmp, 4);
}

vui128_t
__test_splatiuq_64 (void)
{
  return vec_splat_u128 (64);
}

vui128_t
__test_splatiuq_65 (void)
{
  return vec_splat_u128 (65);
}

vui128_t
__test_splatiuq_72 (void)
{
  return vec_splat_u128 (72);
}

vui128_t
__test_splatiuq_112 (void)
{
  return vec_splat_u128 (112);
}

vui128_t
__test_splatiuq_120 (void)
{
  return vec_splat_u128 (120);
}

vui128_t
__test_splatiuq_127 (void)
{
  return vec_splat_u128 (127);
}

vui128_t
__test_splatiuq_127_V1 (void)
{
  const int sim = 0x7f;
  // latency PWR8 6-8
  const vui32_t q_zero = CONST_VINT128_W (0, 0, 0, 0);
  const vui32_t q_ones = CONST_VINT128_W (-1, -1, -1, -1);
  vui32_t tmp = vec_srwi (q_ones, (32-7));
  return (vui128_t) vec_sld (q_zero, tmp, 4);
}

vui128_t
__test_splatiuq_128 (void)
{
  return vec_splat_u128 (128);
}

vui128_t
__test_splatiuq_129 (void)
{
  return vec_splat_u128 (129);
}

vui128_t
__test_splatiuq_136 (void)
{
  return vec_splat_u128 (136);
}

vui128_t
__test_splatiuq_143 (void)
{
  return vec_splat_u128 (143);
}

vui128_t
__test_splatiuq_144 (void)
{
  return vec_splat_u128 (144);
}

vui128_t
__test_splatiuq_159 (void)
{
  return vec_splat_u128 (159);
}

vui128_t
__test_splatiuq_192 (void)
{
  return vec_splat_u128 (192);
}

vui128_t
__test_splatiuq_255 (void)
{
  return vec_splat_u128 (255);
}

vui128_t
__test_splatiuq_V3 (void)
  {
    const int sim = 63;
    vui128_t result;
    /* ((sim >= 16) && (sim < 64))
     * Use the Vector Sum across Signed Word Saturate (vsumsws)
     * instructions. Combining the sum across with word splat
     * generates A * 4 + B word constant in word 3. Words 0-2 are
     * filled with 0x00000000 which eliminates the normal splat 0
     * and terminating vsldoi required for quadword constants.
     * There is special case for ((sim % 5) == 0)) where A == B.
     * Vsumsws is an expensive instructions (7 cycles latency)
     * but is does a lot of work. Requires 3 instructions
     * (or 2 with CSE) and 9 cycles latency total.
     */
    if (__builtin_constant_p (sim) && ((sim % 5) == 0))
	{
	  const vi32_t vai = vec_splat_s32 (sim/5);
	  result = (vui128_t) vec_vsumsws_PWR7 (vai, vai);
	}
    else
	{
	  const vi32_t vai = vec_splat_s32 (sim/4);
	  const vi32_t vbi = vec_splat_s32 (sim%4);
	  // need inline asm to avoid unnecessary LE correction.
	  result = (vui128_t) vec_vsumsws_PWR7 (vai, vbi);
	}
    return result;
  }

vui128_t
__test_splatiuq_V2 (void)
{
  // 7-bit const ((sim > 16) && (sim < 255))
      const int sim = 159;
      const vui32_t q_zero = vec_splat_u32(0);
      const vui32_t v4 = vec_splat_u32(4);
      const vui32_t vhnib = vec_splat_u32(sim / 16);
      vui32_t tmp;
      /* 8-bit pattern to cover constants 16-255.
       * Use splat immediates and shift left to generate the
       * high nibble (high 4-bits). Then splat immediate
       * the low nibble (low 4-bits). Then combine (ADD or OR)
       * to generate the 8-bit const. Skip this if ((sim % 16) != 0).
       * Then shift in (vsldoi) 96-bits of zeros to complete the
       * unsigned __int128 const. Requires 5-7 instructions
       * (or 3-5 with CSE) and 6-12 cycles latency.
       *  */
      // v8bit = vhnib * 16
      tmp = vec_sl (vhnib, v4);
      if ((sim % 16) != 0)
	{ // generate low nibble 0-15
	  const vui32_t vlnib = vec_splat_u32((sim % 16));
	  // 7-bit shift count == voctet + vbit
	  tmp = vec_add (tmp, vlnib);
	}
      return (vui128_t) vec_sld (q_zero, tmp, 4);
    }

vui128_t
__test_splatiuq_V1 (void)
{
  // 7-bit const ((sim > 64) && (sim < 128))
      const int sim = 76;
      const vui32_t q_zero = vec_splat_u32(0);
      const vui32_t v3 = vec_splat_u32(3);
      const vui32_t vbyte = vec_splat_u32(sim / 8);
      vui32_t tmp;
      /* 7-bit shift-count pattern to cover constants 65-127.
       * Use splat immediates and shift left to generate the
       * octet shift count (high 4-bits). Then splat immediate
       * the byte bit shift count (low 3-bits). Then sum (add)
       * to generate the 7-bit const. Skip this if ((sim % 8) != 0).
       * Then shift in (vsldoi) 96-bits of zeros to complete the
       * unsigned __int128 const. Requires 5-7 instructions
       * __int128 const. Requires 5-7 instructions
       * (or 3-5 with CSE) and 6-12 cycles latency.
       *  */
      // voctet = vbyte * 8
      tmp = vec_sl (vbyte, v3);
      if ((sim % 8) != 0)
	{
	  const vui32_t vbit = vec_splat_u32((sim % 8));
	  // 7-bit shift count == voctet + vbit
	  tmp = vec_add (tmp, vbit);
	}
      return (vui128_t) vec_sld (q_zero, tmp, 4);
    }

vui128_t
__test_splatiuq_V0 (void)
{
  // For (((sim % 2) == 0) && (sim < 32))) use vec_splat6_u32
  // from vec_common_ppc.
  const int sim = 24;
  const vui32_t q_zero = vec_splat_u32(0);
  vui32_t vwi = vec_splat6_u32 (sim);
  return (vui128_t) vec_sld (q_zero, vwi, 4);
}

vui128_t
__test_splatiuq_32_V0 (void)
{
  /* Special case for constant 64.
   * For PWR8 we can use Vector Count Leading Zeros Word.
   * For a value of zero returns a bit count of 64.
   * We need to zero extent the doubleword for a quadword result.
   * We use vsldoi with the zero const.
   * This runs 3 instructions (2 with CSE) and 4-6 cycles.
   * Otherwise use ((4 << 3) == 32). See also __test_splatiuq_V1
   * or __test_splatiuq_V3.
   */
#ifdef _ARCH_PWR8
  const vui32_t q_zero = vec_splat_u32(0);
  vui32_t v32 = vec_clzw (q_zero);
  return (vui128_t) vec_sld (q_zero, v32, 4);
#else
  const vui32_t q_zero = vec_splat_u32(0);
  vui32_t v3 = vec_splat_u32(3);
  vui32_t v4 = vec_splat_u32(4);
  vui32_t tmp = vec_sl (v4, v3);
  return (vui128_t) vec_sld (q_zero, tmp, 4);
#endif
}

vui128_t
__test_splatiuq_64_V0 (void)
{
  /* Special case for constant 64.
   * For PWR8 we can use Vector Count Leading Zeros Doubleword.
   * For a value of zero returns a bit count of 64.
   * We need to zero extent the doubleword for a quadword result.
   * We can use either vsldoi or xxpermdi using the zero const.
   * This runs 4 instructions (2 with CSE) and 4-6 cycles.
   * Otherwise use ((4 << 4) == 64).
   * This also needs zero extent of the byte/word to quadword.
   * This runs 4 instructions (2 with CSE) and 4-6 cycles.
   */
#ifdef _ARCH_PWR8
  // This should generate vspltisw vt,0
  // But the compiler may separate vspltisw for word/doubleword const
  const vui64_t q_zero = { 0, 0 };
  vui64_t v64 = vec_clzd (q_zero);
  return (vui128_t) vec_sld ((vui32_t) q_zero, (vui32_t) v64, 8);
#else
  const vui32_t q_zero = vec_splat_u32(0);
  vui32_t v4 = vec_splat_u32(4);
  vui32_t tmp = vec_sl (v4, v4);
  return (vui128_t) vec_sld (q_zero, tmp, 4);
#endif
}

vui128_t
__test_splatiuq_128_V0 (void)
{
  // Expect the compiler to generate vspltisw/vslb here.
  vui8_t vbi = vec_splats ((unsigned char) 128);
  const vui32_t q_zero = {0, 0, 0, 0};
  return (vui128_t) vec_sld ((vui8_t) q_zero, vbi, 1);;
}

vi128_t
__test_vec_abssq (vi128_t vra)
{
  return vec_abssq (vra);
}

vi128_t
__test_vec_negsq (vi128_t int128)
{
  return vec_negsq (int128);
}

vui128_t
__test_vec_neguq (vui128_t int128)
{
  return vec_neguq (int128);
}

vui128_t
__test_msumcud (vui64_t a, vui64_t b, vui128_t c)
{
  return vec_msumcud ( a, b, c);
}

vui128_t
__test_cmsumudm (vui128_t * carry, vui64_t a, vui64_t b, vui128_t c)
{
  *carry = vec_msumcud ( a, b, c);
  return vec_msumudm ( a, b, c);
}

vui128_t
__test_cmsumudm_V2 (vui128_t * carry, vui64_t a, vui64_t b, vui128_t c)
{
  *carry = vec_vmsumcud_inline ( a, b, c);
  return vec_vmsumudm_inline ( a, b, c);
}

vui128_t
test_vec_sldbi_0  (vui128_t a, vui128_t b)
{
  return (vec_sldb_quadword (a, b, 0));
}

vui128_t
test_vec_srdbi_0  (vui128_t a, vui128_t b)
{
  return (vec_sldb_quadword (a, b, 0));
}

vui128_t
test_vec_sldbi_7  (vui128_t a, vui128_t b)
{
  return (vec_sldb_quadword (a, b, 7));
}

vui128_t
test_vec_srdbi_7  (vui128_t a, vui128_t b)
{
  return (vec_srdb_quadword (a, b, 7));
}

unsigned __int128
test_xfer_vui128t_2_uint128 (vui128_t vra)
{
  return vec_transfer_vui128t_to_uint128 (vra);
}

unsigned long long
test_xfer_vui128t_2_LL (vui128_t vra)
{
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 (vra);
  return scalar_extract_uint64_from_low_uint128 (gprp);
}

vui128_t
test_xfer_uint128_2_vui128t (unsigned __int128 gprp)
{
  return vec_transfer_uint128_to_vui128t (gprp);
}

vui128_t
test_xfer_LL_2_vui128t (unsigned long long high, unsigned long long low)
{
  unsigned __int128 gprp = scalar_insert_uint64_to_uint128 (high, low);
  return vec_transfer_uint128_to_vui128t (gprp);
}

vui128_t
test_ctzq_v1 (vui128_t vra)
{
  const vui128_t ones = (vui128_t) vec_splat_s32 (-1);
  const vui128_t c128s = CONST_VUINT128_QxD ( 0, 128 );
  vui128_t term;
  // term = (!vra & (vra - 1))
  term = (vui128_t)vec_andc ((vui64_t)vec_adduqm (vra, ones), (vui64_t)vra);
  // return = 128 - vec_clz (!vra & (vra - 1))
  return vec_subuqm (c128s, vec_clzq (term));
}

// This Looks like the overall winner with the shortest
// instruction sequence and the smallest latency across P8/P9.
vui128_t
test_ctzq_v2 (vui128_t vra)
{
  const vui128_t ones = (vui128_t) vec_splat_s32 (-1);
  vui128_t term;
  // term = (!vra & (vra - 1))
  term = (vui128_t)vec_andc ((vui64_t)vec_adduqm (vra, ones), (vui64_t)vra);
  // return = vec_popcnt (!vra & (vra - 1))
  return (vec_popcntq (term));
}

vui128_t
test_ctzq_v3 (vui128_t vra)
{
  const vui128_t zeros = (vui128_t) vec_splat_s32 (0);
  const vui128_t c128s = CONST_VUINT128_QxD ( 0, 128 );
  vui128_t term;
  // term = (vra | -vra))
  term = (vui128_t)vec_or ((vui64_t)vra, (vui64_t)vec_subuqm (zeros, vra));
  // return = 128 - vec_poptcnt (vra & -vra)
  return vec_subuqm (c128s, vec_popcntq (term));
}

vui128_t
test_ctzq_v4 (vui128_t vra)
{
  vui64_t result;
  vui64_t vt1, vt2, vt3;
  const vui128_t vones = (vui128_t) vec_splat_s32 (-1);
  const vui128_t vzero = (vui128_t) vec_splat_s32 (0);
  vui64_t v64 = { 64, 64 };

  vt1 = vec_ctzd ((vui64_t) vra);
  vt2 = (vui64_t) vec_cmpequd(vt1, v64);
  vt3 = (vui64_t) vec_sld ((vui8_t) vt2, (vui8_t) vones, 8);
  result = vec_and (vt1, vt3);
  result = (vui64_t) vec_vsumsw ((vi32_t) result, (vi32_t) vzero);
  return ((vui128_t) result);
}

#ifdef _ARCH_PWR8
vui128_t
test_ctzq_v5 (vui128_t vra)
{
  vui64_t result;
  const vui128_t vones = (vui128_t) vec_splat_s32 (-1);
  const vui128_t vzero = (vui128_t) vec_splat_s32 (0);
  vui64_t gt64, gt64sl64, rt64, h64, l64;

  // precondition high dword to return ctz()=0 if low dword != 0
  gt64 = (vui64_t) vec_cmpequd((vui64_t) vra, (vui64_t) vzero);
  gt64sl64 = (vui64_t)vec_sld ((vui8_t)gt64, (vui8_t)vones, 8);
  gt64sl64 = vec_orc ((vui64_t) vra, gt64sl64);
  // CTZ high/low halves and sum across dwords
  rt64 = vec_ctzd ((vui64_t) gt64sl64);
  // The following alternative to vsumsms requires more instructions
  // (3 vs 1) but executes with less latency (4-5 vs 7)
  h64 = vec_mrgahd (vzero, (vui128_t)rt64);
  l64 = vec_mrgald (vzero, (vui128_t)rt64);
  result = vec_addudm (h64, l64);

  return ((vui128_t) result);
}
#endif

vui128_t
test_vec_ctzq (vui128_t vra)
{
  return vec_ctzq (vra);
}

vui128_t
__test_msumudm (vui64_t a, vui64_t b, vui128_t c)
{
  return vec_msumudm ( a, b, c);
}

vui128_t
test_vec_add256_1 (vui128_t *__restrict__ out, vui128_t a0, vui128_t a1, vui128_t b0, vui128_t b1)
{
  vui128_t s0, s1, c0, c1;
  s1 = vec_adduqm (a1, b1);
  c1 = vec_addcuq (a1, b1);
  s0 = vec_addeuqm (a0, b0, c1);
  c0 = vec_addecuq (a0, b0, c1);

  out[1] = s1;
  out[0] = s0;
  return (c0);
}
vui128_t
test_vec_add256_2 (vui128_t *__restrict__ out, vui128_t a0, vui128_t a1, vui128_t b0, vui128_t b1)
{
  vui128_t s0, s1, c0, c1;
  s1 = vec_addcq (&c1, a1, b1);
  s0 = vec_addeq (&c0, a0, b0, c1);

  out[1] = s1;
  out[0] = s0;
  return (c0);
}

vui128_t
test_vec_add512_1 (vui128_t *__restrict__ out, vui128_t *__restrict__ a,
		   vui128_t *__restrict__ b)
{
  vui128_t s0, s1, s2, s3, c0, c1, c2, c3;
  s3 = vec_adduqm (a[3], b[3]);
  c3 = vec_addcuq (a[3], b[3]);
  s2 = vec_addeuqm (a[2], b[2], c3);
  c2 = vec_addecuq (a[2], b[2], c3);
  s1 = vec_addeuqm (a[1], b[1], c2);
  c1 = vec_addecuq (a[1], b[1], c2);
  s0 = vec_addeuqm (a[0], b[0], c1);
  c0 = vec_addecuq (a[0], b[0], c1);

  out[3] = s3;
  out[2] = s2;
  out[1] = s1;
  out[0] = s0;
  return (c0);
}

vui128_t
test_vec_add512_2 (vui128_t *__restrict__ out, vui128_t *__restrict__ a,
		   vui128_t *__restrict__ b)
{
  vui128_t s0, s1, s2, s3, c0, c1, c2, c3;
  s3 = vec_addcq (&c3, a[3], b[3]);
  s2 = vec_addeq (&c2, a[2], b[2], c3);
  s1 = vec_addeq (&c1, a[1], b[1], c2);
  s0 = vec_addeq (&c0, a[0], b[1], c1);

  out[3] = s3;
  out[2] = s2;
  out[1] = s1;
  out[0] = s0;
  return (c0);
}

vui128_t
test_vec_absduq (vui128_t vra, vui128_t vrb)
{
  return vec_absduq (vra, vrb);
}

vui128_t
test_vec_avguq (vui128_t vra, vui128_t vrb)
{
  return vec_avguq (vra, vrb);
}

vui128_t
test_vec_addeuqm_ecuq (vui128_t *p, vui128_t a, vui128_t b, vui128_t ci)
{
  *p = vec_addecuq (a, b, ci);
  return vec_addeuqm (a, b, ci);
}

vui128_t
test_vec_adduqm_cuq (vui128_t *p, vui128_t a, vui128_t b)
{
  *p = vec_addcuq (a, b);
   return vec_adduqm (a, b);
}

vui128_t
test_vec_subuqm (vui128_t a, vui128_t b)
{
  return (vec_subuqm (a, b));
}

vui128_t
test_vec_subcuq (vui128_t a, vui128_t b)
{
  return (vec_subcuq (a, b));
}

vui128_t
test_vec_subeuqm_ecuq (vui128_t *p, vui128_t a, vui128_t b, vui128_t ci)
{
  *p = vec_subecuq (a, b, ci);
  return vec_subeuqm (a, b, ci);
}

vui128_t
test_vec_subuqm_cuq (vui128_t *p, vui128_t a, vui128_t b)
{
  *p = vec_subcuq (a, b);
  return vec_subuqm (a, b);
}

vb128_t
test_vec_cmpeqsq (vi128_t a, vi128_t b)
{
  return (vec_cmpeqsq (a, b));
}

vb128_t
test_vec_cmpgesq (vi128_t a, vi128_t b)
{
  return (vec_cmpgesq (a, b));
}

vb128_t
test_vec_cmpgtsq (vi128_t a, vi128_t b)
{
  return (vec_cmpgtsq (a, b));
}

vb128_t
test_vec_cmplesq (vi128_t a, vi128_t b)
{
  return (vec_cmplesq (a, b));
}

vb128_t
test_vec_cmpltsq (vi128_t a, vi128_t b)
{
  return (vec_cmpltsq (a, b));
}

vb128_t
test_vec_cmpequq (vui128_t a, vui128_t b)
{
  return (vec_cmpequq (a, b));
}

vb128_t
test_vec_cmpgtuq (vui128_t a, vui128_t b)
{
  return (vec_cmpgtuq (a, b));
}

vb128_t
test_vec_cmpltuq (vui128_t a, vui128_t b)
{
  return (vec_cmpltuq (a, b));
}

vb128_t
test_vec_cmpgeuq (vui128_t a, vui128_t b)
{
  return (vec_cmpgeuq (a, b));
}

vb128_t
test_vec_cmpleuq (vui128_t a, vui128_t b)
{
  return (vec_cmpleuq (a, b));
}

vb128_t
test_vec_cmpneuq (vui128_t a, vui128_t b)
{
  return (vec_cmpneuq (a, b));
}

int
test_cmpsq_all_eq (vi128_t a, vi128_t b)
{
  return vec_cmpsq_all_eq (a, b);
}

int
test_cmpsq_all_ge (vi128_t a, vi128_t b)
{
  return vec_cmpsq_all_ge (a, b);
}

int
test_cmpsq_all_gt (vi128_t a, vi128_t b)
{
  return vec_cmpsq_all_gt (a, b);
}

int
test_cmpsq_all_le (vi128_t a, vi128_t b)
{
  return vec_cmpsq_all_le (a, b);
}

int
test_cmpsq_all_lt (vi128_t a, vi128_t b)
{
  return vec_cmpsq_all_lt (a, b);
}

int
test_cmpsq_all_ne (vi128_t a, vi128_t b)
{
  return vec_cmpsq_all_ne (a, b);
}

int
test_cmpuq_all_eq (vui128_t a, vui128_t b)
{
  return vec_cmpuq_all_eq (a, b);
}

int
test_cmpuq_all_ge (vui128_t a, vui128_t b)
{
  return vec_cmpuq_all_ge (a, b);
}

int
test_cmpuq_all_gt (vui128_t a, vui128_t b)
{
  return vec_cmpuq_all_gt (a, b);
}

int
test_cmpuq_all_le (vui128_t a, vui128_t b)
{
  return vec_cmpuq_all_le (a, b);
}

int
test_cmpuq_all_lt (vui128_t a, vui128_t b)
{
  return vec_cmpuq_all_lt (a, b);
}

int
test_cmpuq_all_ne (vui128_t a, vui128_t b)
{
  return vec_cmpuq_all_ne (a, b);
}

vb128_t
test_setb_cyq (vui128_t vcy)
{
  return vec_setb_cyq (vcy);
}

vb128_t
test_setb_ncq (vui128_t vcy)
{
  return vec_setb_ncq (vcy);
}

vb128_t
test_setb_sq (vi128_t vcy)
{
  return vec_setb_sq (vcy);
}

vi128_t
__test_maxsq (vi128_t __VH, vi128_t __VL)
{
  return vec_maxsq (__VH, __VL);
}

vui128_t
__test_maxuq (vui128_t __VH, vui128_t __VL)
{
  return vec_maxuq (__VH, __VL);
}

vi128_t
__test_minsq (vi128_t __VH, vi128_t __VL)
{
  return vec_minsq (__VH, __VL);
}

vui128_t
__test_minuq (vui128_t __VH, vui128_t __VL)
{
  return vec_minuq (__VH, __VL);
}

vi128_t
__test_minsq3 (vi128_t vra, vi128_t vrb, vi128_t vrc)
{
  return vec_minsq (vec_minsq (vra, vrb), vrc);
}

vui128_t
__test_minuq3 (vui128_t vra, vui128_t vrb, vui128_t vrc)
{
  return vec_minuq (vec_minuq (vra, vrb), vrc);
}

vui128_t
test_vec_mul10uq_cuq (vui128_t *p, vui128_t a)
{
  *p = vec_mul10cuq (a);
   return vec_mul10uq (a);
}

vui128_t
test_vec_cmul10uq_ecuq (vui128_t *p, vui128_t a, vui128_t a2, vui128_t cin)
{
  vui128_t k, j;
  k = vec_cmul10ecuq (&j, a, cin);
  *p = vec_mul10euq ((vui128_t) a2, j);
  return k;
}

vui128_t
test_vec_addeq (vui128_t *cout, vui128_t a, vui128_t b, vui128_t c)
{
  return (vec_addeq (cout, a, b, c));
}

vui128_t
test_vec_addcq (vui128_t *cout, vui128_t a, vui128_t b)
{
  return (vec_addcq (cout, a, b));
}

vui128_t
test_vec_addecuq (vui128_t a, vui128_t b, vui128_t c)
{
  return (vec_addecuq (a, b, c));
}

vui128_t
test_vec_addeuqm (vui128_t a, vui128_t b, vui128_t ci)
{
  return (vec_addeuqm (a, b, ci));
}

vui128_t
test_vec_adduqm (vui128_t a, vui128_t b)
{
  return (vec_adduqm (a, b));
}

vui128_t
test_vec_addcuq (vui128_t a, vui128_t b)
{
  return (vec_addcuq (a, b));
}

vui128_t
test_vec_rlqi_0 (vui128_t __A)
{
  return vec_rlqi (__A, 0);
}

vui128_t
test_vec_rlqi_1 (vui128_t __A)
{
  return vec_rlqi (__A, 1);
}

vui128_t
test_vec_rlqi_15 (vui128_t __A)
{
  return vec_rlqi (__A, 15);
}

vui128_t
test_vec_rlqi_16 (vui128_t __A)
{
  return vec_rlqi (__A, 16);
}

vui128_t
test_vec_rlqi_17 (vui128_t __A)
{
  return vec_rlqi (__A, 17);
}

vui128_t
test_vec_rlqi_31 (vui128_t __A)
{
  return vec_rlqi (__A, 31);
}

vui128_t
test_vec_rlqi_32 (vui128_t __A)
{
  return vec_rlqi (__A, 32);
}

vui128_t
test_vec_rlqi_33 (vui128_t __A)
{
  return vec_rlqi (__A, 33);
}

vui128_t
test_vec_slqi_0 (vui128_t __A)
{
  return vec_slqi (__A, 0);
}

vui128_t
test_vec_slqi_4 (vui128_t __A)
{
  return vec_slqi (__A, 4);
}

vui128_t
test_vec_slqi_7 (vui128_t __A)
{
  return vec_slqi (__A, 7);
}

vui128_t
test_vec_slqi_8 (vui128_t __A)
{
  return vec_slqi (__A, 8);
}

vui128_t
test_vec_slqi_14 (vui128_t __A)
{
  return vec_slqi (__A, 14);
}

vui128_t
test_vec_slqi_16 (vui128_t __A)
{
  return vec_slqi (__A, 16);
}
#if 1

vui128_t
test_vec_slqi_17 (vui128_t __A)
{
  return vec_slqi (__A, 17);
}

vui128_t
test_vec_slqi_18 (vui128_t __A)
{
  return vec_slqi (__A, 18);
}

vui128_t
test_vec_slqi_30 (vui128_t __A)
{
  return vec_slqi (__A, 30);
}

vui128_t
test_vec_slqi_31 (vui128_t __A)
{
  return vec_slqi (__A, 31);
}

vui128_t
test_vec_slqi_32 (vui128_t __A)
{
  return vec_slqi (__A, 32);
}

vui128_t
test_vec_slqi_34 (vui128_t __A)
{
  return vec_slqi (__A, 34);
}

vui128_t
test_vec_slqi_36 (vui128_t __A)
{
  return vec_slqi (__A, 36);
}

vui128_t
test_vec_slqi_48 (vui128_t __A)
{
  return vec_slqi (__A, 48);
}

vui128_t
test_vec_slqi_60 (vui128_t __A)
{
  return vec_slqi (__A, 60);
}

vui128_t
test_vec_slqi_64 (vui128_t __A)
{
  return vec_slqi (__A, 64);
}

vui128_t
test_vec_slqi_68 (vui128_t __A)
{
  return vec_slqi (__A, 68);
}

vui128_t
test_vec_slqi_96 (vui128_t __A)
{
  return vec_slqi (__A, 96);
}

vui128_t
test_vec_slqi_98 (vui128_t __A)
{
  return vec_slqi (__A, 98);
}

vui128_t
test_vec_slqi_110 (vui128_t __A)
{
  return vec_slqi (__A, 110);
}

vui128_t
test_vec_slqi_112 (vui128_t __A)
{
  return vec_slqi (__A, 112);
}

vui128_t
test_vec_slqi_114 (vui128_t __A)
{
  return vec_slqi (__A, 114);
}

vui128_t
test_vec_slqi_120 (vui128_t __A)
{
  return vec_slqi (__A, 120);
}

vui128_t
test_vec_slqi_127 (vui128_t __A)
{
  return vec_slqi (__A, 127);
}

vui128_t
test_vec_slqi_128 (vui128_t __A)
{
  return vec_slqi (__A, 128);
}

vui128_t
test_vec_slqi_129 (vui128_t __A)
{
  return vec_slqi (__A, 129);
}
#endif

vui128_t
test_vec_srqi_0 (vui128_t __A)
{
  return vec_srqi (__A, 0);
}

vui128_t
test_vec_srqi_4 (vui128_t __A)
{
  return vec_srqi (__A, 4);
}

vui128_t
test_vec_srqi_7 (vui128_t __A)
{
  return vec_srqi (__A, 7);
}

vui128_t
test_vec_srqi_8 (vui128_t __A)
{
  return vec_srqi (__A, 8);
}

vui128_t
test_vec_srqi_14 (vui128_t __A)
{
  return vec_srqi (__A, 14);
}

vui128_t
test_vec_srqi_16 (vui128_t __A)
{
  return vec_srqi (__A, 16);
}
#if 1
vui128_t
test_vec_srqi_31 (vui128_t __A)
{
  return vec_srqi (__A, 31);
}

vui128_t
test_vec_srqi_48 (vui128_t __A)
{
  return vec_srqi (__A, 48);
}

vui128_t
test_vec_srqi_120 (vui128_t __A)
{
  return vec_srqi (__A, 120);
}

vui128_t
test_vec_srqi_128 (vui128_t __A)
{
  return vec_srqi (__A, 128);
}

vui128_t
test_vec_srqi_129 (vui128_t __A)
{
  return vec_srqi (__A, 129);
}
#endif

vi128_t
test_vec_sraqi_0 (vi128_t __A)
{
  return vec_sraqi (__A, 0);
}

vi128_t
test_vec_sraqi_4 (vi128_t __A)
{
  return vec_sraqi (__A, 4);
}

vi128_t
test_vec_sraqi_7 (vi128_t __A)
{
  return vec_sraqi (__A, 7);
}

vi128_t
test_vec_sraqi_8 (vi128_t __A)
{
  return vec_sraqi (__A, 8);
}

vi128_t
test_vec_sraqi_14 (vi128_t __A)
{
  return vec_sraqi (__A, 14);
}

vi128_t
test_vec_sraqi_16 (vi128_t __A)
{
  return vec_sraqi (__A, 16);
}
#if 1
vi128_t
test_vec_sraqi_31 (vi128_t __A)
{
  return vec_sraqi (__A, 31);
}

vi128_t
test_vec_sraqi_48 (vi128_t __A)
{
  return vec_sraqi (__A, 48);
}

vi128_t
test_vec_sraqi_68 (vi128_t __A)
{
  return vec_sraqi (__A, 68);
}

vi128_t
test_vec_sraqi_120 (vi128_t __A)
{
  return vec_sraqi (__A, 120);
}

vi128_t
test_vec_sraqi_128 (vi128_t __A)
{
  return vec_sraqi (__A, 128);
}

vi128_t
test_vec_sraqi_129 (vi128_t __A)
{
  return vec_sraqi (__A, 129);
}
#endif

vui128_t
test_vec_vsumsws (vui128_t vra)
{
  const __vector unsigned long long vzero =
    { 0, 0 };
  return (vui128_t) __builtin_altivec_vsumsws ((__vector int)vra, (__vector int)vzero);
}

vui128_t
test_cmul10cuq (vui128_t *cout, vui128_t a)
{
  return vec_cmul10cuq (cout, a);
}

vui128_t
test_cmul10ecuq (vui128_t *cout, vui128_t a, vui128_t cin)
{
  return vec_cmul10ecuq (cout, a, cin);
}

vui128_t
test_cmul100cuq (vui128_t *cout, vui128_t a)
{
  return vec_cmul100cuq (cout, a);
}

vui128_t
test_cmul100ecuq (vui128_t *cout, vui128_t a, vui128_t cin)
{
  return vec_cmul100ecuq (cout, a, cin);
}

vui128_t
test_vec_mul10uq_c (vui128_t *p, vui128_t a)
{
  *p = vec_mul10cuq (a);
  return vec_mul10uq (a);
}

/* This does not work because the target is applied after the CPP
 * platform specific have been applied based on the command line
 * =mcpu=<target>.
 * Keep this example as a warning. */
vui128_t
#ifndef __clang__
// clang does not either, generates a warning
__attribute__((__target__("cpu=power9")))
#endif
test_vec_mul10uq (vui128_t a)
{
	return vec_mul10uq (a);
}

vui128_t
test_vec_mul10euq (vui128_t a, vui128_t cin)
{
	return vec_mul10euq (a, cin);
}

vui128_t
test_vec_mul10cuq (vui128_t a)
{
	return vec_mul10cuq (a);
}

vui128_t
test_vec_mul10ecuq (vui128_t a, vui128_t cin)
{
	return vec_mul10ecuq (a, cin);
}

vui128_t
test_vec_revbq (vui128_t a)
{
	return vec_revbq(a);
}

void
test_vec_load_store (vui128_t *a, vui128_t *b)
  {
#ifdef __clang__
    vui32_t temp;

    temp = vec_ld (0, (vui32_t *)a);
    vec_st (temp, 0, (vui32_t *)b);
#else
    vui64_t temp;

    temp = vec_ld (0, (vui64_t *)a);
    vec_st (temp, 0, (vui64_t *)b);
#endif
  }

vui64_t
test_vpaste_x (vui64_t __VH, vui64_t __VL)
{
  vui64_t result;
  result[1] = __VH[1];
  result[0] = __VL[0];
  return (result);
}

#if 0 // Deprecated!
vui128_t
test_vsl4 (vui128_t a)
{
	return (vec_slq4(a));
}

vui128_t
test_vsr4 (vui128_t a)
{
	return (vec_srq4(a));
}
#endif

vui128_t
test_vec_sldq (vui128_t a, vui128_t b, vui128_t sh)
{
  return (vec_sldq (a, b, sh));
}

vui128_t
test_vec_sldqi_0 (vui128_t a, vui128_t b)
{
  return (vec_sldqi (a, b, 0));
}

vui128_t
test_vec_sldqi_7 (vui128_t a, vui128_t b)
{
  return (vec_sldqi (a, b, 7));
}

vui128_t
test_vec_sldqi_15 (vui128_t a, vui128_t b)
{
  return (vec_sldqi (a, b, 15));
}

vui128_t
test_vec_sldqi_48 (vui128_t a, vui128_t b)
{
  return (vec_sldqi (a, b, 48));
}

vui128_t
test_vec_sldqi_52 (vui128_t a, vui128_t b)
{
  return (vec_sldqi (a, b, 52));
}

vui128_t
test_vec_rlq  (vui128_t a, vui128_t sh)
{
  return (vec_rlq (a, sh));
}

vui128_t
test_vec_srq  (vui128_t a, vui128_t sh)
{
  return (vec_srq (a, sh));
}

vi128_t
test_vec_sraq  (vi128_t a, vui128_t sh)
{
  return (vec_sraq (a, sh));
}

vui128_t
test_vec_slq  (vui128_t a, vui128_t sh)
{
  return (vec_slq (a, sh));
}

vui128_t
__test_vec_msumudm (vui64_t a, vui64_t b, vui128_t c)
{
  return (vec_msumudm (a, b, c));
}

vui128_t
__test_muloud (vui64_t a, vui64_t b)
{
  return (vec_muloud (a, b));
}

vui128_t
__test_muleud (vui64_t a, vui64_t b)
{
  return (vec_muleud (a, b));
}

vui64_t
__test_muludm (vui64_t a, vui64_t b)
{
  return (vec_muludm (a, b));
}

vui64_t
__test_mulhud (vui64_t a, vui64_t b)
{
  return (vec_mulhud (a, b));
}

vui128_t
__test_vmuloud (vui64_t a, vui64_t b)
{
  return (vec_vmuloud (a, b));
}

vui128_t
__test_vmuleud (vui64_t a, vui64_t b)
{
  return (vec_vmuleud (a, b));
}

vui128_t
__test_vmsumoud (vui64_t a, vui64_t b, vui128_t c)
{
  return vec_vmsumoud (a, b, c);
}

vui128_t
__test_vmsumeud (vui64_t a, vui64_t b, vui128_t c)
{
  return vec_vmsumeud (a, b, c);
}

vui128_t
__test_vmaddoud (vui64_t a, vui64_t b, vui64_t c)
{
  return vec_vmaddoud (a, b, c);
}

vui128_t
__test_vmadd2oud (vui64_t a, vui64_t b, vui64_t c, vui64_t d)
{
  return vec_vmadd2oud (a, b, c, d);
}

vui128_t
__test_vmaddeud (vui64_t a, vui64_t b, vui64_t c)
{
  return vec_vmaddeud (a, b, c);
}

vui128_t
__test_vmadd2eud (vui64_t a, vui64_t b, vui64_t c, vui64_t d)
{
  return vec_vmadd2eud (a, b, c, d);
}

vui128_t
__test_muludq (vui128_t *mulu, vui128_t a, vui128_t b)
{
  return (vec_muludq (mulu, a, b));
}

vui128_t
__test_madduq (vui128_t *mulu, vui128_t a, vui128_t b, vui128_t c)
{
  return vec_madduq (mulu, a, b, c);
}

vui128_t
__test_madd2uq (vui128_t *mulu, vui128_t a, vui128_t b, vui128_t c, vui128_t d)
{
  return vec_madd2uq (mulu, a, b, c, d);
}

vui128_t
__test_madduq_x (vui128_t *mulu, vui128_t a, vui128_t b, vui128_t c)
{
  vui128_t ph, pl, cl;
  pl = vec_muludq (&ph, a, b);
  pl = vec_adduqm (pl, c);
  cl = vec_addcuq (pl, c);
  *mulu = vec_adduqm (ph, cl);
  return (pl);
}

vui128_t
__test_mulluq (vui128_t a, vui128_t b)
{
  return (vec_mulluq (a, b));
}

vui128_t
__test_mulhuq (vui128_t a, vui128_t b)
{
  return vec_mulhuq (a, b);;
}

vui128_t
__test_mulhuq2 (vui128_t a, vui128_t b)
{
  vui128_t mq;
  vec_muludq (&mq, a, b);
  return mq;
}

vi128_t
__test_modsq_10e31  (vi128_t a, vi128_t b)
{
  return vec_modsq_10e31 (a, b);
}

vi128_t
__test_remsq_10e31  (vi128_t a, vi128_t b)
{
  vi128_t q;
  q = vec_divsq_10e31 (a);
  return vec_modsq_10e31 (a, q);
}

vui128_t
__test_remuq_10e31  (vui128_t a, vui128_t b)
{
  vui128_t q;
  q = vec_divuq_10e31 (a);
  return vec_moduq_10e31 (a, q);
}

vui128_t
__test_remuq_10e32  (vui128_t a, vui128_t b)
{
  vui128_t q;
  q = vec_divuq_10e32 (a);
  return vec_moduq_10e32 (a, q);
}

vui128_t
__test_divudq_10e31  (vui128_t *qh, vui128_t a, vui128_t b)
{
  return vec_divudq_10e31 (qh, a, b);
}

vui128_t
__test_divudq_10e32  (vui128_t *qh, vui128_t a, vui128_t b)
{
  return vec_divudq_10e32 (qh, a, b);
}

vui128_t
__test_remudq_10e31  (vui128_t a, vui128_t b)
{
  vui128_t q, qh;
  q = vec_divudq_10e31 (&qh, a, b);
  return vec_modudq_10e31 (a, b, &q);
}

vui128_t
__test_remudq_10e32  (vui128_t a, vui128_t b)
{
  vui128_t q, qh;
  q = vec_divudq_10e32 (&qh, a, b);
  return vec_modudq_10e32 (a, b, &q);
}

#ifdef _ARCH_PWR8
vui128_t
test_vec_subq (vui128_t a, vui128_t b)
{
  return (vec_sub(a,b));
}
#endif

void
test_mul4uq (vui128_t *__restrict__ mulu, vui128_t m1h, vui128_t m1l,
	     vui128_t m2h, vui128_t m2l)
{
  vui128_t mc, mp, mq, mqhl;
  vui128_t mphh, mphl, mplh, mpll;
  mpll = vec_muludq (&mplh, m1l, m2l);
  mp = vec_muludq (&mphl, m1h, m2l);
  mplh = vec_addcq (&mc, mplh, mp);
  mphl = vec_adduqm (mphl, mc);
  mp = vec_muludq (&mqhl, m2h, m1l);
  mplh = vec_addcq (&mq, mplh, mp);
  mphl = vec_addeq (&mc, mphl, mqhl, mq);
  mp = vec_muludq (&mphh, m2h, m1h);
  mphl = vec_addcq (&mq, mphl, mp);
  mphh = vec_addeuqm (mphh, mq, mc);

  mulu[0] = mpll;
  mulu[1] = mplh;
  mulu[2] = mphl;
  mulu[3] = mphh;
}

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define __NDX(__index) (__index)
#define __MDX(__index) (__index)
#define __PDX(__index) (__index)
#else
#define __NDX(__index) ((nx - 1) - (__index))
#define __MDX(__index) ((mx - 1) - (__index))
#define __PDX(__index) ((px - 1) - (__index))
#endif
void
test_mul128_MN (vui128_t *p, vui128_t *m1, vui128_t *m2,
		  unsigned long M, unsigned long N)
{
  vui128_t *mp = m1;
  vui128_t *np = m2;
  unsigned long mx = M;
  unsigned long nx = N;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  unsigned long px = M + N;
#endif
  unsigned long i, j;
  vui128_t mpx0, mqx0, mpx1, mqx1;

  /* sizeof(m1) < sizeof(m2) swap the pointers and quadword counts.
   * This allows for early exit when size of either is 1. */
  if (mx < nx)
    {
      vui128_t *xp;
      unsigned long x;

      x = mx;
      xp = mp;
      mx = nx;
      mp = np;
      nx = x;
      np = xp;
    }

//  printf ("MXD(0)=%lu, NDX(0)=%lu, PDX(0)=%lu\n", __MDX(0), __NDX(0), __PDX(0));
  mpx0 = vec_muludq (&mqx0, mp[__MDX(0)], np[__NDX(0)]);
  p[__PDX(0)] = mpx0;
  for ( i=1; i < mx; i++ )
    {
      mpx1 = vec_madduq (&mqx1, mp[__MDX(i)], np[__NDX(0)], mqx0);
      p[__PDX(i)] = mpx1;
      mqx0 = mqx1;
//      printf ("MXD(i)=%lu, NDX(0)=%lu, PDX(i)=%lu\n", __MDX(i), __NDX(0), __PDX(i));
    }
  p[__PDX(mx)] = mqx0;
//  printf ("PDX(mx)=%lu\n", __PDX(mx));

  for  ( j=1; j < nx; j++ )
    {
      mpx0 = vec_madduq (&mqx0, mp[__MDX(0)], np[__NDX(j)], p[__PDX(j)]);
      p[__PDX(j)] = mpx0;
      for ( i=1; i < mx; i++ )
        {
          mpx1 = vec_madd2uq (&mqx1, mp[__MDX(i)], np[__NDX(j)], mqx0, p[__PDX(i+j)]);
          p[__PDX(i+j)] = mpx1;
          mqx0 = mqx1;
        }
      p[__PDX(mx+j)] = mqx0;
    }

}

void
example_qw_convert_decimal (uint64_t *ten_16, vui128_t value)
{
  /* Magic numbers for multiplicative inverse to divide by 10**32
   are 211857340822306639531405861550393824741, corrective add,
   and shift right 107 bits.  */
  const vui128_t mul_invs_ten32 = (vui128_t) CONST_VINT128_DW(
      0x9f623d5a8a732974UL, 0xcfbc31db4b0295e5UL);
  const int shift_ten32 = 107;
  /* Magic numbers for multiplicative inverse to divide by 10**16
   are 76624777043294442917917351357515459181, no corrective add,
   and shift right 51 bits.  */
  const vui128_t mul_invs_ten16 = (vui128_t) CONST_VINT128_DW(
      0x39a5652fb1137856UL, 0xd30baf9a1e626a6dUL);
  const int shift_ten16 = 51;

  const vui128_t mul_ten32 = (vui128_t) (__int128) 100000000000000ll
      * (__int128) 1000000000000000000ll;
  const vui128_t mul_ten16 = (vui128_t) CONST_VINT128_DW(0UL,
							 10000000000000000UL);

  vui128_t tmpq, tmpr, tmpc, tmp;
  __VEC_U_128 tmp_16;

  // First divide/modulo by 10**32 to separate the top 7 digits from
  // the lower 32 digits
  // tmpq = floor ( M * value / 2**128)
  tmpq = vec_mulhuq (value, mul_invs_ten32);
  // Corrective add may overflow, generate carry
  tmpq = vec_adduqm (tmpq, value);
  tmpc = vec_addcuq (tmpq, value);
  // Shift right with carry bit
  tmpq = vec_sldqi (tmpc, tmpq, (128 - shift_ten32));
  // Compute remainder of value / 10**32
  // tmpr = value - (tmpq * 10**32)
  tmp = vec_mulluq (tmpq, mul_ten32);
  tmpr = vec_subuqm (value, tmp);

  // return top 16 digits
  tmp_16.vx1 = tmpq;
  ten_16[0] = tmp_16.ulong.lower;

  // Next divide/modulo the remaining 32 digits by 10**16.
  // This separates the middle and low 16 digits into doublewords.
  tmpq = vec_mulhuq (tmpr, mul_invs_ten16);
  tmpq = vec_srqi (tmpq, shift_ten16);
  // Compute remainder of tmpr / 10**16
  // tmpr = tmpr - (tmpq * 10**16)
  // Here we know tmpq and mul_ten16 are less then 64-bits
  // so can use vec_vmuloud insted of vec_mulluq
  tmp = vec_vmuloud ((vui64_t) tmpq, (vui64_t) mul_ten16);
  tmpr = vec_subuqm (value, tmp);

  // return middle 16 digits
  tmp_16.vx1 = tmpq;
  ten_16[1] = tmp_16.ulong.lower;
  // return low 16 digits
  tmp_16.vx1 = tmpr;
  ten_16[2] = tmp_16.ulong.lower;
}


void
example_print_vint128 (vi128_t value)
{
  const vi128_t zero128 = (vi128_t) CONST_VINT128_DW(
      0x0L, 0UL);
  const vui128_t mul_ten16 = (vui128_t) CONST_VINT128_DW(
      0UL, 10000000000000000UL);
  // Magic numbers for multiplicative inverse to divide by 10**16
  // are 76624777043294442917917351357515459181, no corrective add,
  // and shift right 51 bits.
  const vui128_t mul_invs_ten16 = (vui128_t) CONST_VINT128_DW(
      0x39a5652fb1137856UL, 0xd30baf9a1e626a6dUL);
  const int shift_ten16 = 51;

  vui128_t tmpq, tmp;
  vui64_t t_low, t_mid, t_high;
  vui128_t val128;
  char sign;

  if (vec_cmpsq_all_ge (value, zero128))
    {
      sign = ' ';
      val128 = (vui128_t) value;
    }
  else
    {
      sign = '-';
      val128 = vec_subuqm ((vui128_t) zero128, (vui128_t) value);
    }
  // Convert the absolute (unsigned) value to Decimal and
  // prefix the sign.

  // first divide/modulo the 39 digits __int128 by 10**16.
  // This separates the high/middle 23 digits (tmpq) and low 16 digits.
  tmpq = vec_mulhuq (val128, mul_invs_ten16);
  tmpq = vec_srqi (tmpq, shift_ten16);
  // Compute remainder of val128 / 10**16
  // t_low = val128 - (tmpq * 10**16)
  // Here we know tmpq and mul_ten16 are less then 64-bits
  // so can use vec_vmuloud instead of vec_mulluq
  tmp = vec_vmuloud ((vui64_t) tmpq, (vui64_t) mul_ten16);
  t_low = (vui64_t) vec_subuqm (val128, tmp);

  // Next divide/modulo the high/middle digits by 10**16.
  // This separates the high 7 and middle 16 digits.
  val128 = tmpq;
  tmpq = vec_mulhuq (tmpq, mul_invs_ten16);
  t_high = (vui64_t) vec_srqi (tmpq, shift_ten16);
  tmp = vec_vmuloud (t_high, (vui64_t) mul_ten16);
  t_mid = (vui64_t) vec_subuqm (val128, tmp);

  // Work-around for GCC PR 96139
#if defined (__clang__) || (__GNUC__ > 9)
  printf ("%c%07llu%016llu%016llu", sign, t_high[VEC_DW_L],
	  t_mid[VEC_DW_L], t_low[VEC_DW_L]);
#else
  printf ("%c%07lu%016lu%016lu", sign, t_high[VEC_DW_L],
	  t_mid[VEC_DW_L], t_low[VEC_DW_L]);
#endif
}

void
example_dw_convert_timebase (vui64_t *tb, vui32_t *timespec, int n)
{
  /* Magic numbers for multiplicative inverse to divide by 512,000,000
     are 4835703278458516699 and shift right 27 bits.  */
  const vui64_t mul_invs_clock =
    { 4835703278458516699UL, 4835703278458516699UL };
  const int shift_clock = 27;
  /* Need const for TB clocks/second to extract remainder.  */
  const vui64_t tb_clock_sec =
    { 512000000, 512000000};
  const int shift_512 = 9;
  const vui64_t nano_512 =
    { 1000, 1000};
  vui64_t tb_v, tmp, tb_clocks, seconds, nseconds;
  vui64_t timespec1, timespec2;
  int i;

  for (i = 0; i < n; i++)
    {
      tb_v = *tb++;
      /* extract integer seconds from timebase vector.  */
      tmp = vec_mulhud (tb_v, mul_invs_clock);
      seconds = vec_srdi (tmp, shift_clock);
      /* Extract remainder in tb clocks. */
      tmp = vec_muludm (seconds, tb_clock_sec);
      tb_clocks = vec_subudm (tb_v, tmp);
      /* Convert 512MHz timebase to nanoseconds.  */
      /* nseconds = tb_clocks * 1000000000 / 512000000 */
      /* reduces to (tb_clocks * 1000) >> 9 */
      tmp = vec_muludm (tb_clocks, nano_512);
      nseconds = vec_srdi (tmp, shift_512);
      /* Use merge high/low to interleave seconds and nseconds
       * into timespec.  */
      timespec1 = vec_mergeh (seconds, nseconds);
      timespec2 = vec_mergel (seconds, nseconds);
      /* seconds and nanoseconds fit int 32-bits after conversion.
       * So pack results and store the timespec.  */
      *timespec++ = vec_vpkudum (timespec1, timespec2);
    }
}

vui128_t
example_longdiv_10e31 (vui128_t *q, vui128_t *d, long int _N)
{
  vui128_t dn, qh, ql, rh;
  long int i;

  // init step for the top digits
  dn = d[0];
  qh = vec_divuq_10e31 (dn);
  rh = vec_moduq_10e31 (dn, qh);
  q[0] = qh;

  // now we know the remainder is less than the divisor.
  for (i=1; i<_N; i++)
    {
      dn = d[i];
      ql = vec_divudq_10e31 (&qh, rh, dn);
      rh = vec_modudq_10e31 (rh, dn, &ql);
      q[i] = ql;
    }
  // return the final remainder
  return rh;
}

vui128_t
example_longdiv_10e32 (vui128_t *q, vui128_t *d, long int _N)
{
  vui128_t dn, qh, ql, rh;
  long int i;

  // init step for the top digits
  dn = d[0];
  qh = vec_divuq_10e32 (dn);
  rh = vec_moduq_10e32 (dn, qh);
  q[0] = qh;

  // now we know the remainder is less than the divisor.
  for (i=1; i<_N; i++)
    {
      dn = d[i];
      ql = vec_divudq_10e32 (&qh, rh, dn);
      rh = vec_modudq_10e32 (rh, dn, &ql);
      q[i] = ql;
    }
  // return the final remainder
  return rh;
}

/* alternative algorithms tested and not selected due to code size
   and cycle latency.  */
vi128_t
__test_vec_cmpgesq (vi128_t vra, vi128_t vrb)
{
#ifdef _ARCH_PWR8
  vi128_t tand;
  vui128_t a_b;
  vui64_t axorb, torc, torc2;

  /* vra >=vrb: (vrb | NOT(vra)) & ((vrb ~ vra) | NOT(vra - vrb)) */
  a_b = vec_subuqm ((vui128_t)vra, (vui128_t)vrb);
  axorb = vec_xor ((vui64_t)vra, (vui64_t)vrb);
  torc = vec_orc (axorb, (vui64_t)a_b);
  torc2 = vec_orc ((vui64_t)vrb, (vui64_t)vra);
  tand = (vi128_t)vec_and (torc2, torc);
  return (vi128_t)vec_setb_sq (tand);
#else
  const vui32_t signbit = CONST_VINT128_W (0x80000000, 0, 0, 0);
  vui32_t _a, _b;

  _a = vec_xor ((vui32_t)vra, signbit);
  _b = vec_xor ((vui32_t)vrb, signbit);
  return (vi128_t)vec_cmpgeuq ((vui128_t)_a, (vui128_t)_b);
#endif
}

vi128_t
__test_vec_cmpgtsq (vi128_t vra, vi128_t vrb)
{
#ifdef _ARCH_PWR8
  vi128_t tor;
  vui128_t b_a;
  vui64_t aeqvb, tand, tandc;

  /* vra >vrb: (vrb & NOT(vra)) | (NOT(vrb ~ vra) & (vrb - vra)) */
  b_a = vec_subuqm ((vui128_t)vrb, (vui128_t)vra);
  /* NOT(a XOR b) == (a EQV b).  */
  aeqvb = vec_eqv ((vui64_t)vra, (vui64_t)vrb);
  tand = vec_and (aeqvb, (vui64_t)b_a);
  tandc = vec_andc ((vui64_t)vrb, (vui64_t)vra);
  tor = (vi128_t)vec_or (tandc, tand);
  return (vi128_t)vec_setb_sq (tor);
#else
  const vui32_t signbit = CONST_VINT128_W (0x80000000, 0, 0, 0);
  vui32_t _a, _b;

  _a = vec_xor ((vui32_t)vra, signbit);
  _b = vec_xor ((vui32_t)vrb, signbit);
  return (vi128_t)vec_cmpgtuq ((vui128_t)_a, (vui128_t)_b);
#endif
}

vi128_t
__test_vec_cmplesq (vi128_t vra, vi128_t vrb)
{
#ifdef _ARCH_PWR8
  vi128_t tand;
  vui128_t b_a;
  vui64_t axorb, torc, torc2;

  /* vra < vrb: (vra | NOT(vrb)) & ((vra ~ vrb) | NOT(vrb - vra)) */
  b_a = vec_subuqm ((vui128_t)vrb, (vui128_t)vra);
  axorb = vec_xor ((vui64_t)vra, (vui64_t)vrb);
  torc = vec_orc (axorb, (vui64_t)b_a);
  torc2 = vec_orc ((vui64_t)vra, (vui64_t)vrb);
  tand = (vi128_t)vec_and (torc2, torc);
  return (vi128_t)vec_setb_sq (tand);
#else
  const vui32_t signbit = CONST_VINT128_W (0x80000000, 0, 0, 0);
  vui32_t _a, _b;

  _a = vec_xor ((vui32_t)vra, signbit);
  _b = vec_xor ((vui32_t)vrb, signbit);
  return (vi128_t)vec_cmpleuq ((vui128_t)_a, (vui128_t)_b);
#endif
}

vi128_t
__test_vec_cmpltsq (vi128_t vra, vi128_t vrb)
{
#ifdef _ARCH_PWR8
  vi128_t tor;
  vui128_t a_b;
  vui64_t aeqvb, tand, tandc;

  /* vra < vrb: (vra & NOT(vrb)) | (NOT(vra ~ vrb) & (vra - vrb)) */
  a_b = vec_subuqm ((vui128_t)vra, (vui128_t)vrb);
  /* NOT(a XOR b) == (a EQV b).  */
  aeqvb = vec_eqv ((vui64_t)vra, (vui64_t)vrb);
  tand = vec_and (aeqvb, (vui64_t)a_b);
  tandc = vec_andc ((vui64_t)vra, (vui64_t)vrb);
  tor = (vi128_t)vec_or (tandc, tand);
  return (vi128_t)vec_setb_sq (tor);
#else
  const vui32_t signbit = CONST_VINT128_W (0x80000000, 0, 0, 0);
  vui32_t _a, _b;

  _a = vec_xor ((vui32_t)vra, signbit);
  _b = vec_xor ((vui32_t)vrb, signbit);
  return (vi128_t)vec_cmpltuq ((vui128_t)_a, (vui128_t)_b);
#endif
}

#ifdef _ARCH_PWR8
vui128_t
__test_clzq (vui128_t vra)
{
	__vector unsigned long long result, vt1, vt3;
	__vector unsigned long long vt2;
	const __vector unsigned long long vzero = {0,0};
	const __vector unsigned long long v64 = {64, 64};

	vt1 = vec_clzd ((__vector unsigned long long)vra);
	vt2 = (__vector unsigned long long)vec_cmpeq (vt1, v64);
	vt3 = (__vector unsigned long long)vec_sld ((__vector unsigned char)vzero, (__vector unsigned char)vt2, 8);
	result = vec_andc(vt1, vt3);
	result = (__vector unsigned long long)vec_sums ((__vector int)result, (__vector int)vzero);

	return ((vui128_t)result);
}
#endif
vui128_t
test_shift_leftdo (vui128_t vrw, vui128_t vrx, vui128_t vrb)
{
	__vector unsigned char result, vt1, vt2, vt3;
	const __vector unsigned char vzero = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	vt1 = vec_slo ((__vector unsigned char)vrw, (__vector unsigned char)vrb);
	vt3 = vec_sub (vzero, (__vector unsigned char)vrb);
	vt2 = vec_sro ((__vector unsigned char)vrx, vt3);
	result = vec_or (vt1, vt2);

	return ((vui128_t)result);
}

vui128_t
test_shift_leftdq (vui128_t vrw, vui128_t vrx, vui128_t vrb)
{
	__vector unsigned char result, vt1, vt2, vt3;
	const __vector unsigned char vzero = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	vt1 = vec_slo ((__vector unsigned char)vrw, (__vector unsigned char)vrb);
	vt1 = vec_sll (vt1, (__vector unsigned char)vrb);
	vt3 = vec_sub (vzero, (__vector unsigned char)vrb);
	vt2 = vec_sro ((__vector unsigned char)vrx, vt3);
	vt2 = vec_srl (vt2, vt3);
	result = vec_or (vt1, vt2);

	return ((vui128_t)result);
}
