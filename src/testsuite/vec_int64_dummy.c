/*
 Copyright (c) [2018, 2023-2024] Steven Munroe.
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

#include <pveclib/vec_int128_ppc.h>

vui64_t
test_vec_expandm_doubleword (vui64_t vra)
{
  return vec_expandm_doubleword (vra);
}

vui64_t
test_vexpanddm_PWR (vui64_t vra)
{
  return vec_vexpanddm_PWR10 (vra);
}

vi64_t
test_vec_signextll_byte (vi8_t vra)
{
  return vec_signextll_byte (vra);
}

vi64_t
test_vec_vextsb2d (vi8_t vra)
{
  return vec_vextsb2d (vra);
}

vi64_t
test_vec_signextll_halfword (vi16_t vra)
{
  return vec_signextll_halfword (vra);
}

vi64_t
test_vec_vextsh2d (vi16_t vra)
{
  return vec_vextsh2d (vra);
}

vi64_t
test_vec_signextll_word (vi32_t vra)
{
  return vec_signextll_word (vra);
}

vi64_t
test_vec_vextsw2d (vi32_t vra)
{
  return vec_vextsw2d (vra);
}

vi64_t
test_vextsb2d_v0 (vi8_t vra)
{
  vi64_t tmp;

  tmp = (vi64_t) vec_sldi ((vui64_t) vra, 56);
  return vec_sradi (tmp, 56);
}

vi64_t
test_splat6_s64_1 ()
{
  return vec_splat6_s64 (1);
}

vi64_t
test_splat6_s64_1_V1 ()
{
  const vi32_t rword = vec_splat_s32 (1);
#if defined(_ARCH_PWR8)
  return (vi64_t) vec_unpackh ((vi32_t)rword);
#else
  return (vi64_t) vec_unpackh ((vi16_t)rword);
#endif
}

vi64_t
test_splat6_s64_1_V0 ()
{
  const vui32_t rword = vec_splat_u32 (1);
#if defined(_ARCH_PWR8)
  return (vi64_t) vec_vupklsw ((vi32_t)rword);
#else
  return (vi64_t) vec_vupklsh ((vi16_t)rword);
#endif
}

vui64_t
test_splat6_u64_1_V2 ()
{
  const vui32_t rword = vec_splat_u32 (1);
#if defined(_ARCH_PWR8)
  return (vui64_t) vec_vupklsw ((vi32_t)rword);
#else
  return (vui64_t) vec_vupklsh ((vi16_t)rword);
#endif
}

vui64_t
test_splat6_u64_1_V1 ()
{
  const vui32_t rword = vec_splat_u32 (1);
#if defined(_ARCH_PWR8)
  return (vui64_t) vec_unpackl ((vi32_t)rword);
#else
  return (vui64_t) vec_unpackl ((vi16_t)rword);
#endif
}

vui64_t
test_splat6_u64_1_V0 ()
{
  const vui32_t rword = vec_splat_u32 (1);
#if defined(_ARCH_PWR8)
  return (vui64_t) vec_vupklsw ((vi32_t)rword);
#else
  return (vui64_t) vec_vupklsh ((vi16_t)rword);
#endif
}

vi64_t
test_splat6_s64_15 ()
{
  return vec_splat6_s64 (15);
}

vi64_t
test_splat6_s64_m16 ()
{
  return vec_splat6_s64 (-16);
}

vi64_t
test_splat6_s64_m1 ()
{
  return vec_splat6_s64 (-1);
}

vui64_t
test_splat6_u64_1 ()
{
  return vec_splat6_u64 (1);
}

vui64_t
test_splat6_u64_15 ()
{
  return vec_splat6_u64 (15);
}

vui64_t
test_splat6_u64_16 ()
{
  return vec_splat6_u64 (16);
}

vui64_t
test_splat6_u64_30 ()
{
  return vec_splat6_u64 (30);
}

vui64_t
test_splat6_u64_31 ()
{
  return vec_splat6_u64 (31);
}

vui64_t
test_splat6_u64_32 ()
{
  return vec_splat6_u64 (32);
}

vui64_t
test_splat6_u64_33 ()
{
  return vec_splat6_u64 (33);
}

vui64_t
test_splat6_u64_34 ()
{
  return vec_splat6_u64 (34);
}

vui64_t
test_splat6_u64_46 ()
{
  return vec_splat6_u64 (46);
}

vui64_t
test_splat6_u64_47 ()
{
  return vec_splat6_u64 (47);
}

vui64_t
test_splat6_u64_48 ()
{
  return vec_splat6_u64 (48);
}

vui64_t
test_splat6_u64_52 ()
{
  return vec_splat6_u64 (52);
}

vui64_t
test_splat6_u64_63 ()
{
  return vec_splat6_u64 (63);
}

vui64_t
test_vec_clzd (vui64_t a)
{
  return vec_clzd (a);
}

vui64_t
test_vec_clzd_PWR7 (vui64_t a)
{
  return vec_clzd_PWR7 (a);
}

vui64_t
test_clzd_PWR7 (vui64_t vra)
{
  // generated const (vui32_t) {20, 20, 20, 20}
  const vui32_t v20 = vec_splat_u32 (20-32);
  // need a dword vector of 1086 which is the exponent for 2**63
  // f2_63 = (vector double) 2**63
  // const vf64_t f2_63 =  (vf64_t) {0x1.0p63D, 0x1.0p63D};
  // i2_63 = exponent for (vector double) 2**63
  const vui64_t i2_63 =  (vui64_t) {1086, 1086};
  // Generate mask = (vui32_t) {0, 0xffffffff, 0, 0xffffffff}
  const vui32_t zero = vec_splat_u32 (0);
  const vui32_t ones = vec_splat_u32 (-1);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  // Only for testing only, PWR7 is BE only
  const vui32_t mask = vec_mergel (ones, zero);
#else
  const vui32_t mask = vec_mergeh (zero, ones);
#endif
  // Generate const (vui32_t) {64, 64, 64, 64}
  const vui32_t v4 = vec_splat_u32 (4);
  const vui32_t v64 = vec_sl (v4, v4);
  vb32_t zmask;
  vui32_t k, n;
  vf64_t kf;

  // Avoid rounding in floating point conversion
  k = (vui32_t) vra & ~((vui32_t) vra >> 1);
  kf = vec_double ((vui64_t) k);
  // right justify exponent for long int
  // Which is tricky because we dont have DW shift.
  // clear extraneous data from words 1/3
  n = vec_andc ((vui32_t) kf, (vui32_t) mask);
  // Rotate right quadword 32-bits, exponents in words 1/3
  n = vec_sld (n, n, 12);
  // Shift exponent into low 11-bits of words 1, 3
  n = vec_vsrw(n, v20);
  // clzd == 1086 - exponent
  n = (vui32_t) i2_63 - (vui32_t) n;
  // Fix-up: DW[i]==0 case where the exponent is zero and diff is 1086
  zmask = vec_cmpgt (n, v64);
  n = vec_sel (n, v64, zmask);

  return ((vui64_t) n);
}

vui64_t
test_clzd_PWR7_V2 (vui64_t vra)
{
  const vui32_t zero = vec_splat_u32 (0);
  // generated const (vui32_t) {20, 20, 20, 20}
  const vui32_t v10 = vec_splat_u32 (10);
  const vui32_t v20 = vec_add (v10, v10);
  // need a dword vector of 1086 which is the exponent for 2**63
  // f2_63 = (vector double) 2**63
  const vf64_t f2_63 =  (vf64_t) {0x1.0p63, 0x1.0p63};
  // Generate expmask = (vui32_t) {0, 0xffffffff, 0, 0xffffffff}
  const vui32_t ones = vec_splat_u32 (-1);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  // Only for testing only, PWR7 is only BE
  const vui32_t expmask = vec_mergel (ones, zero);
#else
  const vui32_t expmask = vec_mergeh (zero, ones);
#endif
  // Generate const (vui32_t) {64, 64, 64, 64}
  const vui32_t v4 = vec_splat_u32 (4);
  const vui32_t v64 = vec_sl (v4, v4);
  vb32_t zmask;
  vui32_t k, n;
  vf64_t kf;

  // Avoid rounding in floating point conversion
  k = (vui32_t) vra & ~((vui32_t) vra >> 1);
  kf = vec_double ((vui64_t) k);
  // Exponent difference is the leading zeros count
  n = (vui32_t) f2_63 - (vui32_t) kf;
  // right justify exponent for long int
  n = vec_sld (n, n, 12);
  n = vec_vsrw(n, v20);
  // clear extraneous data from words 0/2
  n = vec_and (n, (vui32_t) expmask);
  // Fix-up: vra=0 case where the exponent is zero and diff is 1086
  zmask = vec_cmpgt (n, v64);
  n = vec_sel (n, v64, zmask);

  return ((vui64_t) n);
}

vui64_t
test_clzd_PWR7_V1 (vui64_t vra)
{
  const vui32_t zero = vec_splat_u32 (0);
  // const vui32_t vone = vec_splat_u32 (1);
  // const vui32_t ones = vec_splat_u32 (-1);
  // generated const (vui32_t) {20, 20, 20, 20}
  const vui32_t v10 = vec_splat_u32 (10);
  const vui32_t v20 = vec_add (v10, v10);
  // fp5 = 1.0 / 2.0 == 0.5
  //const vf64_t fp5 = (vf64_t) {0.5, 0.5};
  // need a dword vector of 1086 which is the exponent for 2**63
  // f2_63 = (vector double) 2**63
  const vf64_t f2_63 =  (vf64_t) {0x1.0p63, 0x1.0p63};
  vui64_t expmask;
#if 1
  const vui32_t ones = vec_splat_u32 (-1);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  expmask = (vui64_t) vec_mergel (ones, zero);
#else
  expmask = (vui64_t) vec_mergeh (zero, ones);
#endif
#else
  expmask =  (vui64_t) CONST_VINT64_DW (0xffffffff, 0xffffffff);
#endif

  vui32_t k, n;
  vf64_t kf;
  // Avoid rounding in floating point conversion
  k = (vui32_t) vra & ~((vui32_t) vra >> 1);
  // Handle all 0s case by insuring min exponent is 1066-64
  kf = vec_double ((vui64_t) k) /*+ fp5*/;
  // Exponent difference is the leading zeros count
  n = (vui32_t) f2_63 - (vui32_t) kf;
  // right justify exponent for long int
  n = vec_vsrw(n, v20);
  n = vec_sld (zero, n, 12);
  n = vec_and (n, (vui32_t) expmask);
#if 1
  const vui32_t v4 = vec_splat_u32 (4);
  const vui32_t v64 = vec_sl (v4, v4);
  vb32_t mask;
  // Fix-up: vra=0 case where the exponent is zero and diff is 1086
  mask = vec_cmpgt (n, v64);
  n = vec_sel (n, v64, mask);
#else
  const vui32_t v9 = vec_splat_u32 (9);
  const vui64_t clzmask =  (vui64_t) CONST_VINT64_DW (63, 63);
  vui32_t n1, n2;
  n1 = vec_and (n, (vui32_t) clzmask);
  n2 = vec_vsrw (n, v9);
  n = vec_add (n1, n2);
#endif
  return ((vui64_t) n);
}

vui64_t
test_clzd_PWR7_V0 (vui64_t vra)
{
  const vui32_t zero = vec_splat_u32 (0);
  // const vui32_t vone = vec_splat_u32 (1);
  // const vui32_t ones = vec_splat_u32 (-1);
  // generated const (vui32_t) {20, 20, 20, 20}
  const vui32_t v10 = vec_splat_u32 (10);
  const vui32_t v20 = vec_add (v10, v10);
  // fp5 = 1.0 / 2.0 == 0.5
  //const vf64_t fp5 = (vf64_t) {0.5, 0.5};
  // need a dword vector of 1086 which is the exponent for 2**63
  // f2_63 = (vector double) 2**63
  const vf64_t f2_63 =  (vf64_t) {0x1.0p63, 0x1.0p63};
  vui64_t expmask;
#if 1
  const vui32_t ones = vec_splat_u32 (-1);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  expmask = (vui64_t) vec_mergel (ones, zero);
#else
  expmask = (vui64_t) vec_mergeh (zero, ones);
#endif
#else
  expmask =  (vui64_t) CONST_VINT64_DW (0x7ff, 0x7ff);
#endif

  vui32_t k, n;
  vf64_t kf;
  // Avoid rounding in floating point conversion
  k = (vui32_t) vra & ~((vui32_t) vra >> 1);
  // Handle all 0s case by insuring min exponent is 1066-64
  kf = vec_double ((vui64_t) k) /*+ fp5*/;
  // Exponent difference is the leading zeros count
  n = (vui32_t) f2_63 - (vui32_t) kf;
  // right justify exponent for long int
  n = vec_vsrw(n, v20);
#if 1
  n = vec_sld (zero, n, 12);
  n = vec_and (n, (vui32_t) expmask);
#if 1
  const vui32_t v4 = vec_splat_u32 (4);
  const vui32_t v64 = vec_sl (v4, v4);
  vb32_t mask;
  // Fix-up for 0 case where the exponent is zero and diff is 1086.
  mask = vec_cmpgt (n, v64);
  n = vec_sel (n, v64, mask);
#else
  const vui32_t v9 = vec_splat_u32 (9);
  const vui64_t clzmask =  (vui64_t) CONST_VINT64_DW (63, 63);
  vui32_t n1, n2;
  n1 = vec_and (n, (vui32_t) clzmask);
  n2 = vec_vsrw (n, v9);
  n = vec_add (n1, n2);
#endif
#else
  vui32_t nh, nl, nhl;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  // undo endian for PWR8 testing only, work around!
  // word0 in word0
  nh = n ; //vec_mergel (n, zero);
  // word2 in word1
  nl = vec_mergeh (n, zero);
  nhl = vec_or (nh, nl);
  n = vec_mergeh (zero, nhl);
  // {zero, word0, zero, word2}
  n = vec_mergel (nhl, zero);
#else
  // word0 in word0
  nh = n; //vec_mergeh (zero, n);
  // word2 in word1
  nl = vec_mergel (zero, n);
  nhl = vec_or (nh, nl);
  // {zero, word0, zero, word2}
  n = vec_mergeh (zero, nhl);
#endif
#endif
  return ((vui64_t) n);
}

vui64_t
test_vec_popcntd (vui64_t vra)
{
  return vec_popcntd (vra);
}

vui64_t
test_vec_popcntd_PWR7 (vui64_t vra)
{
  return vec_popcntd_PWR7 (vra);
}

vui64_t
__test_shift_cse (vui64_t a)
{
  vui64_t r;
  // Should generate the const vui64_t shift count once for both.
  // With small shift count ( < 16) should generate vspltisw.
  r = vec_srdi (a, 1);
  return vec_sldi (r, 1);
}

// Attempts at better code to splat small DW constants.
// Want to avoid addr calc and loads for what should be simple
// splat immediate and unpack/extend.
vi64_t
__test_splatisd_16 (void)
{
  return vec_splat_s64 (-16);
}

vi64_t
__test_splatisd_n1 (void)
{
  return vec_splat_s64 (-1);
}

vi64_t
__test_splatisd_0 (void)
{
  return vec_splat_s64 (0);
}

vi64_t
__test_splatisd_15 (void)
{
  return vec_splat_s64 (15);
}

vi64_t
__test_splatisd_127 (void)
{
  return vec_splat_s64 (127);
}

vui64_t
__test_splatiud_0 (void)
{
  return vec_splat_u64 (0);
}

vui64_t
__test_splatiud_15 (void)
{
  return vec_splat_u64 (15);
}

vui64_t
__test_splatiud_127 (void)
{
  return vec_splat_u64 (127);
}

#define COMPILE_FENCE __asm ("nop":::)
#define COMPILE_FENCE1 __asm ("ori 1,1,0":::)
// Avoid instructions that require power8
// Or LE specific compiler support
vui64_t
__test_splatudi_12_V3 (void)
{
  vi32_t vwi = vec_splat_s32 (12);
  // Unpack signed HW works here because the word immediate
  // value fits in a signed HW and high HW of word will unpacked
  // into 0's or 1's in the high word of DW.
  // And uppack low/high (or endian) will not change result.
  // COMPILE_FENCE1;
  return (vui64_t) vec_vupklsh ((vi16_t) vwi);
}
// Requires power8 for vec_vupklsw
#ifdef _ARCH_PWR8
vui64_t
__test_splatudi_12_V2 (void)
{
  vi32_t vwi = vec_splat_s32 (12);
  // vi32_t vwi = vec_splats ((int) 12);
  // Unpack signed Word expands two lower words to doublewords.
  // And uppack low/high (or endian) will not change result.
  return (vui64_t) vec_vupklsw (vwi);
}
#endif

// Unpackl/h from word requires GCC 7 for LE and 9 for BE
#if ((__GNUC__ > 6) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)) \
 || ((__GNUC__ > 8) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__))
vi64_t
__test_splatudi_12_V1 (void)
{
  vi32_t vwi = vec_splat_s32 (12);
  return vec_unpackl (vwi);
}
#endif

vui64_t
__test_splatudi_8_V0 (void)
{
  vi32_t vwi = vec_splat_s32 (8);
  return (vui64_t) vwi;
}

vui64_t
__test_splatudi_12_V0 (void)
{
  return vec_splats ((unsigned long long) 12);
}

vui64_t
__test_splatudi_15_V1 (void)
{
  return vec_splats ((unsigned long long) 12);
}

vui64_t
__test_splatudi_15_V0 (void)
{
  const vui64_t dw_15 = CONST_VINT64_DW(15, 15);
  return dw_15;
}

vui64_t
__test_splatudi_10_V0 (void)
{
  return vec_splats ((unsigned long long) 10);
}

vui64_t
__test_splatudi_0_V0 (void)
{
  return vec_splats ((unsigned long long) 0);
}

vui64_t
__test_splatudi_1_V1 (void)
{

  return vec_splats ((unsigned long long) 1);
}

vui64_t
__test_splatudi_1_V0 (void)
{
  const vui64_t dw_one = CONST_VINT64_DW(1, 1);
  return dw_one;
}

vui64_t
__test_splatudi_128_V1 (void)
{

  return vec_splats ((unsigned long long) 128);
}

vui64_t
__test_splatudi_128_V0 (void)
{
  const vui64_t dw_one = CONST_VINT64_DW(128, 128);
  return dw_one;
}

vui64_t
__test_splatudi_smask_V0 (void)
{
  const vui64_t dw_sign_mask = (vui64_t) CONST_VINT128_W(0x80000000, 0x0,
                                                         0x80000000, 0x0);
  return dw_sign_mask;
}

vi64_t
__test_splatudi_n1_V1 (void)
{
  return vec_splats ((signed long long) -1);
}

vi64_t
__test_splatudi_n1_V0 (void)
{
  const vi64_t dw_one = CONST_VINT64_DW(-1, -1);
  return dw_one;
}

vui64_t
__test_incud_V0 (vui64_t vra)
{
  return vra + 1;
}

vui32_t
__test_incuw_V0 (vui32_t vra)
{
  return vra + 1;
}

vi64_t
__test_incsd_V0 (vi64_t vra)
{
  return vra + 1;
}

vb64_t
test_vec_setb_sd (vi64_t vra)
{
  return vec_setb_sd (vra);
}

// Shift Left Double immediate for P7
vui64_t
test_slli_3 (vui64_t a)
{
  vui8_t sl_a, sl_m;
  vui8_t lshift;
  const vui8_t m1 = (vui8_t) vec_splat_s8(-1);

  // use vec_slqi/vec_rlqi for general shift!
  lshift = (vui8_t) vec_splat_s8(3);
  sl_a = vec_sll ((vui8_t) a, lshift);
  sl_m = vec_sll ((vui8_t) m1, lshift);
  sl_m = vec_xxpermdi (sl_m, sl_m, 2);
  return (vui64_t) vec_and (sl_a, sl_m);
}

unsigned __int128
test_unpack_dw (vui64_t vra)
{
  __VEC_U_128 t;
  unsigned __int128 result;
  t.vx2 = vra;
  result = t.ui128;
  return (result);
}

unsigned __int128
test_unpack_dw_2 (vui64_t vra)
{
  __VEC_U_128 t;
  t.ulong.upper = vec_extract (vra, 0);
  t.ulong.lower = vec_extract (vra, 1);
  return (t.ui128);
}

#if !(defined(__clang__) && __clang_major__ < 8)
unsigned __int128
test_unpack_dw_3 (vui64_t vra)
{
  __VEC_U_128 t;
  t.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) vra, 0);
  t.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) vra, 1);
  return (t.ui128);
}
#endif

#if !defined(__clang__)
unsigned __int128
test_unpack_dw_4 (vui64_t vra)
{
  union
  {
    unsigned __int128 ui128;
    vui64_t vx2;
    struct
    {
  #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
      uint64_t lower;
      uint64_t upper;
  #else
      uint64_t upper;
      uint64_t lower;
  #endif
    } ulong;
  } t;

  unsigned __int128 result;
#ifdef  _ARCH_PWR8
  t.vx2 = vra;
  result = t.ui128;
#else
  vui64_t vra_l = vec_xxpermdi (vra, vra, 2);;

#ifdef  _ARCH_PWR7
  __asm__(
      "stxsdx %x2,%y0;"
      "stxsdx %x3,%y1;"
      "ori  2,2,0;"
      : "=Z" (t.ulong.lower),
	"=Z" (t.ulong.upper)
      : "wa" (vra_l), "wa" (vra)
      : );
#else //_ARCH_PWR6/970
  /* Just have to go through storage and let the hardware deal with
   * load/store ordering. */
  t.vx1 = vra;
#endif
  result = t.ui128;
#endif
  return (result);
}
#endif

void
test_vstsudux (vui64_t data, unsigned long long int *array, unsigned long offset)
{
  vec_vstsidx (data, offset, array);
}

void
test_vstsudux_c0 (vui64_t data, unsigned long long int *array)
{
  vec_vstsidx (data, 0, array);
}

void
test_vstsudux_c1 (vui64_t data, unsigned long long int *array)
{
  vec_vstsidx (data, 8, array);
}

void
test_vstsudux_c2 (vui64_t data, unsigned long long int *array)
{
  vec_vstsidx (data, 32760, array);
}

void
test_vstsudux_c3 (vui64_t data, unsigned long long int *array)
{
  vec_vstsidx (data, 32758, array);
}

void
test_vstsudux_c4 (vui64_t data, unsigned long long int *array)
{
  vec_vstsidx (data, 32768, array);
}

void
test_vstsudux_c5 (vui64_t data, unsigned long long int *array)
{
  vui64_t data1;

  data1 = vec_xxspltd (data, 1);
  vec_vstsidx (data, 16, array);
  vec_vstsidx (data1, 48, array);
}

void
test_stvsud_v1 (vui64_t xs, unsigned long long int *array,
		unsigned long offset0, unsigned long offset1)
{
  vui64_t rese1;

  rese1 = vec_xxspltd (xs, 1);
  vec_vstsidx (xs, offset0, array);
  vec_vstsidx (rese1, offset1, array);
}

void
test_stvsud_v2 (vui64_t data, unsigned long long int *array, vui64_t vra)
{
  vui64_t rese1;

  rese1 = vec_xxspltd (data, 1);
  vec_vstsidx (data, vra[VEC_DW_H], array);
  vec_vstsidx (rese1, vra[VEC_DW_L], array);
}

void
test_stvsud_v3 (vui64_t data, unsigned long long int *array, vui64_t vra)
{
  vui64_t rese1, offset;

  offset = vec_sldi (vra, 3);
  rese1 = vec_xxspltd (data, 1);
  vec_vstsidx (data, offset[VEC_DW_H], array);
  vec_vstsidx (rese1, offset[VEC_DW_L], array);
}

void
test_stvsudo (vui64_t data, unsigned long long int *array, vi64_t vra)
{
  vec_vsstuddo (data, array, vra);
}

void
test_stvsudx (vui64_t data, unsigned long long int *array, vi64_t vra)
{
  vec_vsstuddx (data, array, vra);
}

void
test_stvsudsx (vui64_t data, unsigned long long int *array, vi64_t vra)
{
  vec_vsstuddsx (data, array, vra, 4);
}

vui64_t
test_vslsudux (unsigned long long int *array, unsigned long offset)
{
  return vec_vlsidx (offset, array);
}

vui64_t
test_vslsudux_c0 (unsigned long long int *array)
{
  return vec_vlsidx (0, array);
}

vui64_t
test_vslsudux_c1 (unsigned long long int *array)
{
  return vec_vlsidx (8, array);
}

vui64_t
test_vslsudux_c2 (unsigned long long int *array)
{
  return vec_vlsidx (32768, array);
}

vui64_t
test_vslsudux_c3 (unsigned long long int *array)
{
  vui64_t rese0, rese1;

  rese0 = vec_vlsidx (8, array);
  rese1 = vec_vlsidx (40, array);
  return vec_permdi (rese0, rese1, 0);
}

vui64_t
test_lvgud_v1 (unsigned long long int *array, unsigned long offset0, unsigned long offset1)
{
  vui64_t rese0, rese1;

  rese0 = vec_vlsidx (offset0, array);
  rese1 = vec_vlsidx (offset1, array);
  return vec_permdi (rese0, rese1, 0);
}

vui64_t
test_lvgud_v2 (unsigned long long int *array, vui64_t vra)
{
  vui64_t rese0, rese1;

  rese0 = vec_vlsidx (vra[VEC_DW_H], array);
  rese1 = vec_vlsidx (vra[VEC_DW_L], array);
  return vec_xxpermdi (rese0, rese1, 0);
}

vui64_t
test_lvgud_v3 (unsigned long long int *array, vui64_t vra)
{
  vui64_t rese0, rese1, offset;
#if 0
  // This always loads the {3, 3} const vector from rodata.
  offset = vec_sldi (vra, 3);
#else
  {
    // This also loads the {3, 3} const vector from rodata.
    vui32_t lshift = vec_splats((unsigned)3);
    offset = vec_vsld (vra, (vui64_t) lshift);
  }
#endif
  rese0 = vec_vlsidx (offset[VEC_DW_H], array);
  rese1 = vec_vlsidx (offset[VEC_DW_L], array);
  return vec_xxpermdi (rese0, rese1, 0);
}

#if (__GNUC__ > 7)  && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
vui64_t
test_lvgud_v4 (unsigned long long int *array, vui64_t vra)
{
  vui64_t rese0, rese1;
  __VEC_U_128 t;
  t.vx2 = vra;

  rese0 = vec_xl (t.ulong.lower, array);
  rese1 = vec_xl (t.ulong.upper, array);
  return  vec_permdi (rese0, rese1, 0);
}
#endif

vui64_t
test_lvgud_v5 (unsigned long long int *array, vui64_t vra)
{
  unsigned char *arr = (unsigned char *)array;

  vui64_t r = { *(unsigned long long int *)(&arr[vra[VEC_DW_H]]),
                *(unsigned long long int *)(&arr[vra[VEC_DW_L]]) };
  return r;
}

#if !(defined(__clang__) && __clang_major__ < 8)
vui64_t
test_lvgud_v5b (unsigned long long int *array, vui64_t vra)
{
  unsigned char *arr = (unsigned char *)array;
  unsigned long long r0, r1;

  r0 = __builtin_unpack_vector_int128 ((vi128_t) vra, 0);
  r1 = __builtin_unpack_vector_int128 ((vi128_t) vra, 1);

  vui64_t r = { *(unsigned long long int *)(&arr[r0]),
                *(unsigned long long int *)(&arr[r1]) };
  return r;
}
#endif

vui64_t
test_lvgud_v6 (unsigned long long int *array, vui64_t vra)
{
  vui64_t r = { array[vra[VEC_DW_H]],
                array[vra[VEC_DW_L]] };
  return r;
}

#if !(defined(__clang__) && __clang_major__ < 8)
vui64_t
test_lvgud_v6b (unsigned long long int *array, vui64_t vra)
{
  unsigned long long r0, r1;

  r0 = __builtin_unpack_vector_int128 ((vi128_t) vra, 0);
  r1 = __builtin_unpack_vector_int128 ((vi128_t) vra, 1);
  vui64_t r = { array[r0], array[r1] };
  return r;
}
#endif

#if !defined(__clang__)
vui64_t
test_lvgud_v6c (unsigned long long int *array, vui64_t vra)
{
  union
  {
    unsigned __int128 ui128;
    vui64_t vx2;
    struct
    {
  #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
      uint64_t lower;
      uint64_t upper;
  #else
      uint64_t upper;
      uint64_t lower;
  #endif
    } ulong;
  } t;
  vui64_t vra_l = vec_xxpermdi (vra, vra, 2);
  unsigned long long r0, r1;

  __asm__(
      "stfd %2,%0;"
      "stfd %3,%1;"
      "ori  2,2,0;"
      : "=m" (t.ulong.lower),
	"=m" (t.ulong.upper)
      : "d" (vra_l), "d" (vra)
      : );

  r0 = t.ulong.upper;
  r1 = t.ulong.lower;
  vui64_t r = { array[r0], array[r1] };
  return r;
}
#endif

vui64_t
test_lvgud_v7 (unsigned long long int *array, vui64_t vra)
{
  vui64_t r = { array[(vra[VEC_DW_H] * 1 << 4)],
                array[(vra[VEC_DW_L] * 1 << 4)] };
  return r;
}

#if !(defined(__clang__) && __clang_major__ < 8)
vui64_t
test_lvgud_v7b (unsigned long long int *array, vui64_t vra)
{
  unsigned long long r0, r1;

  r0 = __builtin_unpack_vector_int128 ((vi128_t) vra, 0);
  r1 = __builtin_unpack_vector_int128 ((vi128_t) vra, 1);
  vui64_t r = { array[(r0 * 1 << 4)],
                array[(r1 * 1 << 4)] };
  return r;
}
#endif

vui64_t
test_vec_lvgudo (unsigned long long int *array, vi64_t vra)
{
  return vec_vgluddo (array, vra);
}

vui64_t
test_vec_lvgudx (unsigned long long int *array, vi64_t vra)
{
  return vec_vgluddx (array, vra);
}

vui64_t
test_vec_lvgudsx (unsigned long long int *array, vi64_t vra)
{
  return vec_vgluddsx (array, vra, 4);
}

vui64_t
test_ctzd_v1 (vui64_t vra)
{
  const vui64_t ones = { -1, -1 };
  const vui64_t c64s = { 64, 64 };
  vui64_t term;
  // term = (!vra & (vra - 1))
  term = vec_andc (vec_addudm (vra, ones), vra);
  // return = 64 - vec_clz (!vra & (vra - 1))
  return (c64s - vec_clzd (term));
}

vui64_t
test_ctzd_v2 (vui64_t vra)
{
  const vui64_t ones = { -1, -1 };
  vui64_t term;
  // term = (!vra & (vra - 1))
  term = vec_andc (vec_addudm (vra, ones), vra);
  // return = vec_popcnt (!vra & (vra - 1))
  return (vec_popcntd (term));
}

vui64_t
test_ctzd_v3 (vui64_t vra)
{
  const vui64_t zeros = { 0, 0 };
  const vui64_t c64s = { 64, 64 };
  vui64_t term;
  // term = (vra | -vra))
  term = vec_or (vra, vec_subudm (zeros, vra));
  // return = 64 - vec_poptcnt (vra & -vra)
  return (c64s - vec_popcntd (term));
}

vui64_t
test_vec_ctzd (vui64_t vra)
{
  return (vec_ctzd (vra));
}

vui64_t
__test_vrld_builtin (vui64_t a, vui64_t b)
{
  return vec_vrld (a, b);
}

vui64_t
__test_vsld_builtin (vui64_t a, vui64_t b)
{
  return vec_vsld (a, b);
}

vui64_t
__test_vsrd_builtin (vui64_t a, vui64_t b)
{
  return vec_vsrd (a, b);
}

vi64_t
__test_vsrad_builtin (vi64_t a, vui64_t b)
{
  return vec_vsrad (a, b);
}

vui64_t
__test_vrld_V1 (vui64_t a, vui8_t b)
{
  return vec_vrld_PWR8 (a, b);
}

vui64_t
test_rldi_1 (vui64_t a)
{
  return vec_rldi_PWR8 (a, 1);
}

vui64_t
test_rldi_15 (vui64_t a)
{
  return vec_rldi_PWR8 (a, 15);
}

vui64_t
test_vec_rldi_31 (vui64_t a)
{
  return vec_rldi_PWR8 (a, 31);
}

vui64_t
test_vec_rldi_32 (vui64_t a)
{
  return vec_rldi_PWR8 (a, 32);
}

vui64_t
test_vec_rldi_33 (vui64_t a)
{
  return vec_rldi_PWR8 (a, 33);
}

vui64_t
test_vec_rldi_48 (vui64_t a)
{
  return vec_rldi_PWR8 (a, 48);
}

vui64_t
test_vec_rldi_52 (vui64_t a)
{
  return vec_rldi_PWR8 (a, 52);
}

vui64_t
test_vec_rldi_63 (vui64_t a)
{
  return vec_rldi_PWR8 (a, 63);
}

vui64_t
test_rldi_32_V0 (vui64_t a)
{
  vui8_t shft = vec_splat6_u8 (32);
  return vec_vrld_PWR8 (a, shft);
}

vui64_t
test_rldi_52_V0 (vui64_t a)
{
  vui8_t shft = vec_splat6_u8 (52);
  return vec_vrld_PWR8 (a, shft);
}

vui64_t
test_rldi_63_V0 (vui64_t a)
{
  vui8_t shft = vec_splat6_u8 (63);
  return vec_vrld_PWR8 (a, shft);
}

vui64_t
__test_vsld_PWR7 (vui64_t a, vui8_t b)
{
  return vec_vsld_PWR7 (a, b);
}

vui64_t
__test_vsld_PWR8 (vui64_t a, vui8_t b)
{
  return vec_vsld_PWR8 (a, b);
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
test_sldi_24 (vui64_t a)
{
  return vec_sldi (a, 24);
}

vui64_t
test_sldi_31 (vui64_t a)
{
  return vec_sldi (a, 31);
}

vui64_t
test_vec_sldi_32 (vui64_t a)
{
  return vec_sldi (a, 32);
}

vui64_t
test_sldi_33 (vui64_t a)
{
  return vec_sldi (a, 33);
}

vui64_t
test_sldi_46 (vui64_t a)
{
  return vec_sldi (a, 46);
}

vui64_t
test_sldi_47 (vui64_t a)
{
  return vec_sldi (a, 47);
}

vui64_t
test_sldi_48 (vui64_t a)
{
  return vec_sldi (a, 48);
}

vui64_t
test_vec_sldi_52 (vui64_t a)
{
  return vec_sldi (a, 52);
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
__test_sldi_24_V0 (vui64_t a)
{
  vui8_t shft = vec_splat6_u8 (24);
  return vec_vsld_PWR8 (a, shft);
}

vui64_t
__test_sldi_31_V0 (vui64_t a)
{
  vui8_t shft = vec_splat6_u8 (31);
  return vec_vsld_PWR8 (a, shft);
}

vui64_t
test_sldi_32_V0 (vui64_t a)
{
  vui8_t shft = vec_splat6_u8 (32);
  return vec_vsld_PWR8(a, shft);
}

vui64_t
test_sldi_52_V0 (vui64_t a)
{
  vui8_t shft = vec_splat6_u8 (52);
  return vec_vsld_PWR8 (a, shft);
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
test_srdi_31 (vui64_t a)
{
  return vec_srdi (a, 31);
}

vui64_t
test_vec_srdi_32 (vui64_t a)
{
  return vec_srdi (a, 32);
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

vui64_t
__test_vsrd_b (vui64_t a, vui8_t b)
{
  return vec_vsrd_PWR8 (a, b);
}

vui64_t
test_srdi_32_V0 (vui64_t a)
{
  vui8_t shft = vec_splat6_u8 (32);
  return vec_vsrd_PWR8 (a, shft);
}

vui64_t
test_srdi_52_V0 (vui64_t a)
{
  vui8_t shft = vec_splat6_u8 (52);
  return vec_vsrd_PWR8 (a, shft);
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

vi64_t
__test_vsrad_b (vi64_t a, vui8_t b)
{
  return vec_vsrad_PWR8 (a, b);
}

vi64_t
test_sradi_32 (vi64_t a)
{
  vui8_t shft = vec_splat6_u8 (32);
  return vec_vsrad_PWR8 (a, shft);
}

vi64_t
test_sradi_52 (vi64_t a)
{
  vui8_t shft = vec_splat6_u8 (52);
  return vec_vsrad_PWR8 (a, shft);
}

int
test_cmpud_all_eq (vui64_t a, vui64_t b)
{
  return vec_cmpud_all_eq (a, b);
}

int
test_cmpud_all_ge (vui64_t a, vui64_t b)
{
  return vec_cmpud_all_ge (a, b);
}

int
test_cmpud_all_gt (vui64_t a, vui64_t b)
{
  return vec_cmpud_all_gt (a, b);
}

int
test_cmpud_all_le (vui64_t a, vui64_t b)
{
  return vec_cmpud_all_le (a, b);
}

int
test_cmpud_all_lt (vui64_t a, vui64_t b)
{
  return vec_cmpud_all_lt (a, b);
}

int
test_cmpud_all_ne (vui64_t a, vui64_t b)
{
  return vec_cmpud_all_ne (a, b);
}

int
test_cmpud_any_eq (vui64_t a, vui64_t b)
{
  return vec_cmpud_any_eq (a, b);
}

int
test_cmpud_any_ge (vui64_t a, vui64_t b)
{
  return vec_cmpud_any_ge (a, b);
}

int
test_cmpud_any_gt (vui64_t a, vui64_t b)
{
  return vec_cmpud_any_gt (a, b);
}

int
test_cmpud_any_le (vui64_t a, vui64_t b)
{
  return vec_cmpud_any_le (a, b);
}

int
test_cmpud_any_lt (vui64_t a, vui64_t b)
{
  return vec_cmpud_any_lt (a, b);
}

int
test_cmpud_any_ne (vui64_t a, vui64_t b)
{
  return vec_cmpud_any_ne (a, b);
}

int
test_cmpsd_all_eq (vi64_t a, vi64_t b)
{
  return vec_cmpsd_all_eq (a, b);
}

int
test_cmpsd_all_ge (vi64_t a, vi64_t b)
{
  return vec_cmpsd_all_ge (a, b);
}

int
test_cmpsd_all_gt (vi64_t a, vi64_t b)
{
  return vec_cmpsd_all_gt (a, b);
}

int
test_cmpsd_all_le (vi64_t a, vi64_t b)
{
  return vec_cmpsd_all_le (a, b);
}

int
test_cmpsd_all_lt (vi64_t a, vi64_t b)
{
  return vec_cmpsd_all_lt (a, b);
}

int
test_cmpsd_all_ne (vi64_t a, vi64_t b)
{
  return vec_cmpsd_all_ne (a, b);
}

int
test_cmpsd_any_eq (vi64_t a, vi64_t b)
{
  return vec_cmpsd_any_eq (a, b);
}

int
test_cmpsd_any_ge (vi64_t a, vi64_t b)
{
  return vec_cmpsd_any_ge (a, b);
}

int
test_cmpsd_any_gt (vi64_t a, vi64_t b)
{
  return vec_cmpsd_any_gt (a, b);
}

int
test_cmpsd_any_le (vi64_t a, vi64_t b)
{
  return vec_cmpsd_any_le (a, b);
}

int
test_cmpsd_any_lt (vi64_t a, vi64_t b)
{
  return vec_cmpsd_any_lt (a, b);
}

int
test_cmpsd_any_ne (vi64_t a, vi64_t b)
{
  return vec_cmpsd_any_ne (a, b);
}

vui64_t
test_cmpud_all_gt2 (vui64_t a, vui64_t b, vui64_t c, vui64_t d)
{
  if (vec_cmpud_all_gt (a, b))
    return c;
  else
    return d;
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
  return vec_subudm (a, b);
}

vui64_t
test_addudm (vui64_t a, vui64_t b)
{
  return vec_addudm (a, b);
}

vui64_t
test_addudm_PWR7 (vui64_t a, vui64_t b)
{
  return vec_addudm_PWR7 (a, b);
}

vui64_t
test_absdud (vui64_t a, vui64_t b)
{
  return vec_absdud (a, b);
}

vb64_t
test_cmpequd (vui64_t a, vui64_t b)
{
  return vec_cmpequd (a, b);
}

vb64_t
test_cmpneud (vui64_t a, vui64_t b)
{
  return vec_cmpneud (a, b);
}

vb64_t
test_cmpgtud (vui64_t a, vui64_t b)
{
  return vec_cmpgtud (a, b);
}

vb64_t
test_cmpltud (vui64_t a, vui64_t b)
{
  return vec_cmpltud (a, b);
}

vb64_t
test_cmpgeud (vui64_t a, vui64_t b)
{
  return vec_cmpgeud (a, b);
}

vb64_t
test_cmpleud (vui64_t a, vui64_t b)
{
  return vec_cmpleud (a, b);
}

vb64_t
test_cmpeqsd (vi64_t a, vi64_t b)
{
  return vec_cmpeqsd (a, b);
}

vb64_t
test_cmpnesd (vi64_t a, vi64_t b)
{
  return vec_cmpnesd (a, b);
}

vb64_t
test_cmpgtsd (vi64_t a, vi64_t b)
{
  return vec_cmpgtsd (a, b);
}

vb64_t
test_cmpltsd (vi64_t a, vi64_t b)
{
  return vec_cmpltsd (a, b);
}

vb64_t
test_cmpgesd (vi64_t a, vi64_t b)
{
  return vec_cmpgesd (a, b);
}

vb64_t
test_cmplesd (vi64_t a, vi64_t b)
{
  return vec_cmplesd (a, b);
}

vui64_t
__test_popcntd (vui64_t a)
{
  return vec_popcntd (a);
}

vui64_t
__test_clzd (vui64_t a)
{
  return vec_clzd (a);
}

vui64_t
__test_revbd (vui64_t vra)
{
  return vec_revbd (vra);
}

vi64_t
__test_maxsd (vi64_t __VH, vi64_t __VL)
{
  return vec_maxsd (__VH, __VL);
}

vui64_t
__test_maxud (vui64_t __VH, vui64_t __VL)
{
  return vec_maxud (__VH, __VL);
}

vi64_t
__test_minsd (vi64_t __VH, vi64_t __VL)
{
  return vec_minsd (__VH, __VL);
}

vui64_t
__test_minud (vui64_t __VH, vui64_t __VL)
{
  return vec_minud (__VH, __VL);
}

vui64_t
__test_mrgahd (vui128_t __VH, vui128_t __VL)
{
  return vec_mrgahd (__VH, __VL);
}

vui64_t
__test_mrgald (vui128_t __VH, vui128_t __VL)
{
  return vec_mrgald (__VH, __VL);
}

vui64_t
__test_mrghd (vui64_t __VH, vui64_t __VL)
{
  return vec_mrghd (__VH, __VL);
}

vui64_t
__test_mrgld (vui64_t __VH, vui64_t __VL)
{
  return vec_mrgld (__VH, __VL);
}

vui64_t
__test_permdi_0 (vui64_t __VH, vui64_t __VL)
{
  return vec_permdi (__VH, __VL, 0);
}

vui64_t
__test_permdi_1 (vui64_t __VH, vui64_t __VL)
{
  return vec_permdi (__VH, __VL, 1);
}

vui64_t
__test_permdi_2 (vui64_t __VH, vui64_t __VL)
{
  return vec_permdi (__VH, __VL, 2);
}

vui64_t
__test_permdi_3 (vui64_t __VH, vui64_t __VL)
{
  return vec_permdi (__VH, __VL, 3);
}

vui64_t
__test_vpaste (vui64_t __VH, vui64_t __VL)
{
  return vec_pasted (__VH, __VL);
}

vui64_t
__test_spltd_0 (vui64_t __VH)
{
  return vec_xxspltd (__VH, 0);
}

vui64_t
__test_spltd_1 (vui64_t __VH)
{
  return vec_xxspltd (__VH, 1);
}

vui64_t
__test_swapd (vui64_t __VH)
{
  return vec_swapd (__VH);
}

#ifdef _ARCH_PWR8
/* POWER 64-bit (vector long long) compiler tests.  */

vb64_t
__test_cmpequd (vui64_t a, vui64_t b)
{
  return vec_cmpeq (a, b);
}

vb64_t
__test_cmpgtud (vui64_t a, vui64_t b)
{
  return vec_cmpgt (a, b);
}

vb64_t
__test_cmpltud (vui64_t a, vui64_t b)
{
  return vec_cmplt (a, b);
}

vb64_t
__test_cmpgeud (vui64_t a, vui64_t b)
{
  return vec_cmpge (a, b);
}

vb64_t
__test_cmpleud (vui64_t a, vui64_t b)
{
  return vec_cmple (a, b);
}

vui64_t
__test_vmaddeuw (vui32_t a, vui32_t b, vui32_t c)
{
  return vec_vmaddeuw (a, b, c);
}

vui64_t
__test_vmaddouw (vui32_t a, vui32_t b, vui32_t c)
{
  return vec_vmaddouw (a, b, c);
}

vui64_t
__test_vmadd2euw (vui32_t a, vui32_t b, vui32_t c, vui32_t d)
{
  return vec_vmadd2euw (a, b, c, d);
}

vui64_t
__test_vmadd2ouw (vui32_t a, vui32_t b, vui32_t c, vui32_t d)
{
  return vec_vmadd2ouw (a, b, c, d);
}

vui64_t
__test_vmadduw (vui32_t a, vui32_t b, vui32_t c)
{
  vui64_t ep, op;
  ep = vec_vmaddeuw (a, b, c);
  op = vec_vmaddouw (a, b, c);
  return vec_addudm (ep, op);
}

vui64_t
__test_vmuludm (vui64_t vra, vui64_t vrb)
{
  vui64_t s32 = (vui64_t) { 32, 32 };
  vui64_t z = (vui64_t) { 0, 0 };
  vui64_t t4;
  vui64_t t2, t3;
  vui32_t t1;

  t1 = (vui32_t)vec_vrld (vrb, s32);
  t2 = vec_mulouw ((vui32_t)vra, (vui32_t)vrb);
  t3 = vec_vmsumuwm ((vui32_t)vra, t1, z);
  t4 = vec_vsld (t3, s32);
  return (vui64_t)vec_addudm (t4, t2);
}
#endif

#ifdef _ARCH_PWR7
/* POWER 64-bit (vector long long) compiler tests.  */

vui64_t
__test_vrld_PWR7 (vui64_t a, vui64_t b)
{
  vui64_t r, hd, ld;
  vui32_t t1, t2;
  vui8_t shh, shl;

  shh = vec_splat ((vui8_t)b, 7);
  shl = vec_splat ((vui8_t)b, 15);
  hd = vec_xxspltd (a, 0);
  ld = vec_xxspltd (a, 1);
  t1 = vec_vslo ((vui32_t)hd, shh);
  t1 = vec_vsl (t1, shh);
  t2 = vec_vslo ((vui32_t)ld, shl);
  t2 = vec_vsl (t2, shl);
  r = vec_mrghd ((vui64_t)t1, (vui64_t)t2);

  return r;
}

vui64_t
__test_xxpermdi_0 (vui64_t vra, vui64_t vrb)
{
  return vec_xxpermdi (vra, vrb, 0);
}

vui64_t
__test_xxpermdi_1 (vui64_t vra, vui64_t vrb)
{
  return vec_xxpermdi (vra, vrb, 1);
}

vui64_t
__test_xxpermdi_2 (vui64_t vra, vui64_t vrb)
{
  return vec_xxpermdi (vra, vrb, 2);
}

vui64_t
__test_xxpermdi_3 (vui64_t vra, vui64_t vrb)
{
  return vec_xxpermdi (vra, vrb, 3);
}

vui64_t
__test_mergeh (vui64_t a, vui64_t b)
{
  return vec_mergeh (a, b);
}

vui64_t
__test_mergel (vui64_t a, vui64_t b)
{
  return vec_mergel (a, b);
}

#if (__GNUC__ > 6)

vui64_t
__test_vmsumuwm (vui32_t vra, vui32_t vrb, vui64_t vrc)
{
  return vec_vmsumuwm (vra, vrb, vrc);
}

vui64_t
__test_splatd0 (vui64_t a)
{
  return vec_splat (a, 0);
}

vui64_t
__test_splatd1 (vui64_t a)
{
  return vec_splat (a, 1);
}

#if (__GNUC__ > 7)  && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)

vui64_t
__test_mergee (vui64_t a, vui64_t b)
{
  return vec_mergee (a, b);
}

vui64_t
__test_mergeo (vui64_t a, vui64_t b)
{
  return vec_mergeo (a, b);
}

vui64_t
__test_mul (vui64_t a, vui64_t b)
{
  return vec_mul (a, b);
}

vui64_t
__test_vmuludm_PWR7 (vui64_t vra, vui64_t vrb)
{
  vui64_t s32 = (vui64_t) { 32, 32 };
  vui64_t z = (vui64_t) { 0, 0 };
  vui64_t t4;
  vui64_t t2, t3;
  vui32_t t1;

  t1 = (vui32_t)vec_vrld (vrb, s32);
  t2 = vec_mulouw ((vui32_t)vra, (vui32_t)vrb);
  t3 = vec_vmsumuwm ((vui32_t)vra, t1, z);
  t4 = vec_vsld (t3, s32);
  return (vui64_t)vec_addudm (t4, t2);
}
#endif
#endif
#endif

vui64_t test_divdud (vui64_t x, vui64_t y, vui64_t z)
{
  return vec_divdud_inline (x, y, z);
}

vui64_t test_moddud (vui64_t x, vui64_t y, vui64_t z)
{
  return vec_moddud_inline (x, y, z);
}

vui64_t test_divqud (vui128_t x_y, vui64_t z)
{
  return vec_divqud_inline (x_y, z);
}

vui64_t test_divud (vui64_t y, vui64_t z)
{
  return vec_vdivud_inline (y, z);
}

vui64_t test_divude (vui64_t x, vui64_t z)
{
  return vec_vdiveud_inline (x, z);
}

vui64_t test_modud (vui64_t y, vui64_t z)
{
  return vec_vmodud_inline (y, z);
}

vui64_t test_divmodud (vui64_t *r, vui64_t y, vui64_t z)
{
  *r = vec_vmodud_inline (y, z);
  return vec_vdivud_inline (y, z);
}

vui64_t test_divmoddud (vui64_t *r, vui64_t x, vui64_t y, vui64_t z)
{
  *r = vec_moddud_inline (x, y, z);
  return vec_divdud_inline (x, y, z);
}

vui64_t test_vec_divud (vui64_t y, vui64_t z)
{
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  vui64_t res;
#if (__GNUC__ >= 13)
  res = vec_div (y, z);
#else
  __asm__(
      "vdivud %0,%1,%2;\n"
      : "=v" (res)
      : "v" (y), "v" (z)
      : );
#endif
  return res;
#elif defined (_ARCH_PWR8)
  // POWER8/9 Do not have vector integer divide, but do have
  // Move To/From Vector-Scalar Register Instructions
  // So we can use the scalar hardware divide instructions
  __VEC_U_128 qu, yu, zu;
#if (__GNUC__ <= 10)
  yu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) y, 1);
  yu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) y, 0);
  zu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) z, 1);
  zu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) z, 0);
#else
  yu.vx2 = y;
  zu.vx2 = z;
#endif

  qu.ulong.lower = yu.ulong.lower / zu.ulong.lower;
  qu.ulong.upper = yu.ulong.upper / zu.ulong.upper;

  return qu.vx2;
#else
  int i;
  vb64_t ge;
  vui64_t c, xt;
  const vui64_t ones = vec_splat_u64(1);
  vui64_t x = vec_splat_u64(0);

  for (i = 1; i <= 64; i++)
    {
      /* Left shift (x || y) requires 129-bits, -> (t || x || y) */
      /* capture high bits of x and y into t and c. */
#if 1
#if 1
      c = vec_vrld (y, ones);
#else
      c = vec_rldi (y, 1);
#endif
#else
      c = vec_srdi (y, 63);
#endif
#if 0
      /* capture high bit of x as bool */
      t = (vui64_t) vec_sradi ((vi64_t)x, 63);
#endif
      y = vec_addudm (y, y); /* Shift left 1, x and y */
      x = vec_addudm (x, x);
      /* Propagate carry from y to x */
#if 1
      x = vec_selud (x, c, (vb64_t) ones);
#else
      x = vec_addudm (x, c);
#endif

      // deconstruct ((t || x) >= z)
      ge = vec_cmpgeud (x, z);
#if 0
      // Combine t with (x >= z) for 65-bit compare
      ge = (vb64_t) vec_or ((vui32_t)t, (vui32_t)ge);
      // Convert bool to carry-bit for conditional y+1
#if 0
      t  = vec_srdi ((vui64_t)ge, 63);
#endif
#endif

      /* if (x >= z) x = x - z ; y++ */
      xt = vec_subudm (x, z);
      /* if ((t || x) >= z) {x = xt; y++} */
#if 1
      /* Instead of add, OR the boolean ge into bit_0 of y */
      y = vec_selud (y, (vui64_t) ge, (vb64_t) ones);
#else
      y = (vui64_t) vec_or ((vui32_t)y, (vui32_t)t);
#endif
      x = vec_selud (x, xt, ge);
    }
  return y;
#endif
}

vui64_t test_vec_divude (vui64_t x, vui64_t z)
{
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  vui64_t res;
#if (__GNUC__ >= 13)
  res = vec_dive (x, z);
#else
  __asm__(
      "vdiveud %0,%1,%2;\n"
      : "=v" (res)
      : "v" (x), "v" (z)
      : );
#endif
  return res;
#elif defined (_ARCH_PWR8)
  // POWER8/9 Do not have vector integer divide, but do have
  // Move To/From Vector-Scalar Register Instructions
  // So we can use the scalar hardware divide instructions
  __VEC_U_128 qu, yu, zu;
#if (__GNUC__ <= 10)
  yu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) x, 1);
  yu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) x, 0);
  zu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) z, 1);
  zu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) z, 0);
#else
  // Looks like AT16 handles this but what about 15/14 ...
  // AT10 does not.
  yu.vx2 = x;
  zu.vx2 = z;
#endif

  qu.ulong.lower = __builtin_divdeu (yu.ulong.lower, zu.ulong.lower);
  qu.ulong.upper = __builtin_divdeu (yu.ulong.upper, zu.ulong.upper);

  return qu.vx2;
#else
  int i;
  vb64_t ge;
  vui64_t t, xt;
  const vui64_t ones = vec_splat_u64(1);
#if defined (_ARCH_PWR8)
  const vui64_t zeros = vec_splat_u64(0);
#endif
  vui64_t y = vec_splat_u64(0);

  for (i = 1; i <= 64; i++)
    {
      /* Left shift (x || y) requires 129-bits, -> (t || x || y) */
      /* capture high bits of x and y into t and c. */
#if 0
#if 1
      c = vec_rldi (y, 1);
#else
      c = vec_srdi (y, 63);
#endif
#endif
      /* capture high bit of x as bool */
#if defined (_ARCH_PWR8)
      /* capture high bit of x as bool t */
      t = (vui64_t) vec_cmpltsd ((vi64_t) x, (vi64_t) zeros);
#else
      /* P7 vec_cmpltsd is complicated, vec_setb_sd() is simpler */
      t = (vui64_t) vec_setb_sd ((vi64_t) x);
#endif
      y = vec_addudm (y, y); /* Shift left 1, x and y */
      x = vec_addudm (x, x);
#if 0
#if 1
      x = vec_sel (x, c, ones);
#else
      x = vec_add (x, c); /* Propagate carry from y to x */
#endif
#endif

      // deconstruct ((t || x) >= z)
      ge = vec_cmpgeud (x, z);
      // Combine t with (x >= z) for 65-bit compare
      ge = (vb64_t) vec_or ((vui32_t)t, (vui32_t)ge);
#if 0
      // Convert bool to carry-bit for conditional y+1
      t  = vec_srdi ((vui64_t)ge, 63);
#endif

      /* if (x >= z) x = x - z ; y++ */
      xt = vec_subudm (x, z);
      /* if ((t || x) >= z) {x = xt; y++} */
#if 1
      /* Instead of add, OR the boolean ge into bit_0 of y */
      y = vec_selud (y, (vui64_t) ge, (vb64_t) ones);
#else
      y = (vui64_t) vec_or ((vui32_t)y, (vui32_t)t);
#endif
      x = vec_selud (x, xt, ge);
    }
  return y;
#endif
}

vui64_t test_vec_modud (vui64_t y, vui64_t z)
{
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  vui64_t res;
#if (__GNUC__ >= 13)
  res = vec_mod (y, z);
#else
  __asm__(
      "vmodud %0,%1,%2;\n"
      : "=v" (res)
      : "v" (y), "v" (z)
      : );
#endif
  return res;
#elif defined (_ARCH_PWR8)
  // POWER8/9 Do not have vector integer divide, but do have
  // Move To/From Vector-Scalar Register Instructions
  // So we can use the scalar hardware divide instructions
  __VEC_U_128 qu, yu, zu;
#if (__GNUC__ <= 10)
  yu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) y, 1);
  yu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) y, 0);
  zu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) z, 1);
  zu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) z, 0);
#else
  yu.vx2 = y;
  zu.vx2 = z;
#endif

  qu.ulong.lower = yu.ulong.lower % zu.ulong.lower;
  qu.ulong.upper = yu.ulong.upper % zu.ulong.upper;

  return qu.vx2;
#else
  int i;
  vb64_t ge;
  vui64_t c, xt;
  const vui64_t ones = vec_splat_u64(1);
  vui64_t x = vec_splat_u64(0);

  for (i = 1; i <= 64; i++)
    {
      /* Left shift (x || y) requires 129-bits, -> (t || x || y) */
      /* capture high bits of x and y into t and c. */
#if 1
#if 1
      c = vec_vrld (y, ones);
#else
      c = vec_rldi (y, 1);
#endif
#else
      c = vec_srdi (y, 63);
#endif
#if 0
      /* capture high bit of x as bool */
      t = (vui64_t) vec_sradi ((vi64_t)x, 63);
#endif
      y = vec_addudm (y, y); /* Shift left 1, x and y */
      x = vec_addudm (x, x);
      /* Propagate carry from y to x */
#if 1
      x = vec_selud (x, c, (vb64_t) ones);
#else
      x = vec_addudm (x, c);
#endif

      // deconstruct ((t || x) >= z)
      ge = vec_cmpgeud (x, z);
#if 0
      // Combine t with (x >= z) for 65-bit compare
      ge = (vb64_t) vec_or ((vui32_t)t, (vui32_t)ge);
      // Convert bool to carry-bit for conditional y+1
#if 0
      t  = vec_srdi ((vui64_t)ge, 63);
#endif
#endif

      /* if (x >= z) x = x - z ; y++ */
      xt = vec_subudm (x, z);
      /* if ((t || x) >= z) {x = xt; y++} */
#if 1
      /* Instead of add, OR the boolean ge into bit_0 of y */
      y = vec_selud (y, (vui64_t) ge, (vb64_t) ones);
#else
      y = (vui64_t) vec_or ((vui32_t)y, (vui32_t)t);
#endif
      x = vec_selud (x, xt, ge);
    }
  return x;
#endif
}

vui64_t test_vec_divmodud_V1 (vui64_t *r, vui64_t y, vui64_t z)
{
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  vui64_t res, rem;
#if (__GNUC__ >= 13)
  res = vec_div (y, z);
  rem = vec_div (y, z);
#else
  __asm__(
      "vdivud %0,%1,%2;\n"
      : "=v" (res)
      : "v" (y), "v" (z)
      : );
  __asm__(
      "vmodud %0,%1,%2;\n"
      : "=v" (rem)
      : "v" (y), "v" (z)
      : );
#endif
  *r = rem;
  return res;
#elif defined (_ARCH_PWR8)
  // POWER8/9 Do not have vector integer divide, but do have
  // Move To/From Vector-Scalar Register Instructions
  // So we can use the scalar hardware divide instructions
  __VEC_U_128 ru, qu, yu, zu;
#if (__GNUC__ <= 10)
  yu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) y, 1);
  yu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) y, 0);
  zu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) z, 1);
  zu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) z, 0);
#else
  yu.vx2 = y;
  zu.vx2 = z;
#endif

  qu.ulong.lower = yu.ulong.lower / zu.ulong.lower;
  qu.ulong.upper = yu.ulong.upper / zu.ulong.upper;

  ru.ulong.lower = yu.ulong.lower % zu.ulong.lower;
  ru.ulong.upper = yu.ulong.upper % zu.ulong.upper;

  *r = ru.vx2;
  return qu.vx2;
#else
  int i;
  vb64_t ge;
  vui64_t c, xt;
  const vui64_t ones = vec_splat_u64(1);
  vui64_t x = vec_splat_u64(0);

  for (i = 1; i <= 64; i++)
    {
      /* Left shift (x || y) requires 129-bits, -> (t || x || y) */
      /* capture high bits of x and y into t and c. */
#if 1
#if 1
      c = vec_vrld (y, ones);
#else
      c = vec_rldi (y, 1);
#endif
#else
      c = vec_srdi (y, 63);
#endif
#if 0
      /* capture high bit of x as bool */
      t = (vui64_t) vec_sradi ((vi64_t)x, 63);
#endif
      y = vec_addudm (y, y); /* Shift left 1, x and y */
      x = vec_addudm (x, x);
      /* Propagate carry from y to x */
#if 1
      x = vec_selud (x, c, (vb64_t) ones);
#else
      x = vec_addudm (x, c);
#endif

      // deconstruct ((t || x) >= z)
      ge = vec_cmpgeud (x, z);
#if 0
      // Combine t with (x >= z) for 65-bit compare
      ge = (vb64_t) vec_or ((vui32_t)t, (vui32_t)ge);
      // Convert bool to carry-bit for conditional y+1
#if 0
      t  = vec_srdi ((vui64_t)ge, 63);
#endif
#endif

      /* if (x >= z) x = x - z ; y++ */
      xt = vec_subudm (x, z);
      /* if ((t || x) >= z) {x = xt; y++} */
#if 1
      /* Instead of add, OR the boolean ge into bit_0 of y */
      y = vec_selud (y, (vui64_t) ge, (vb64_t) ones);
#else
      y = (vui64_t) vec_or ((vui32_t)y, (vui32_t)t);
#endif
      x = vec_selud (x, xt, ge);
    }
  *r = x;
  return y;
#endif
}

vui64_t test_vec_divmodud_V0 (vui64_t *r, vui64_t y, vui64_t z)
{
  vui64_t Q;
  vui64_t R;

  Q = test_vec_divud (y, z);
  R = test_vec_modud (y, z);

  *r = R;
  return Q;
}

vui64_t test_vec_divdud (vui64_t x, vui64_t y, vui64_t z)
{
#if defined (_ARCH_PWR8)
  vui64_t Q, R, Qt /*, Rt*/;
  vui64_t r1, r2, q1, q2;
  vb64_t CC, c1, c2;
  const vui64_t ones = vec_splat_u64(1);

  // Based on the PowerISA, Programming Note for
  // Divide Word Extended [Unsigned] but vectorized
  // for vector long long int
  q1 = test_vec_divude (x, z);
  q2 = test_vec_divud  (y, z);
  r1 = vec_muludm (q1, z);

  r2 = vec_muludm (q2, z);
  r2 = vec_subudm (y, r2);
  Q  = vec_addudm (q1, q2);
  R  = vec_subudm (r2, r1);

  c1 = vec_cmpltud (R, r2);
  c2 = vec_cmpgeud (R, z);
  CC = vec_or (c1, c2);
#if 1
  Qt = vec_addudm (Q, ones);
  Q = vec_selud (Q, Qt, CC);
#else // Corrected Remainder not returned for divdud.
  Rt = vec_subudm (R, z);
  R = vec_selud (R, Rt, CC);
#endif
  return Q;
#elif defined (_ARCH_PWR7)
  // P7 Missing some DW operations, so use divqud to avoid them.
  vui128_t xy_h, xy_l;
  vui64_t QQ, RQ_h, RQ_l, z_l;

  xy_h = (vui128_t) vec_mrghd (x, y);
  xy_l = (vui128_t) vec_mrgld (x, y);
  z_l  = vec_swapd (z);
  RQ_h = test_divqud (xy_h, z);
  RQ_l = test_divqud (xy_l, z_l);
  QQ   = vec_mrgld (RQ_h, RQ_l);
  return QQ;
#else
  /* Based on Hacker's Delight (2nd Edition) Figure 9-2.
   * "Vector Divide unsigned doubleword, using shift-and-subtract algorithm."
   * Converted to use vector unsigned long long and PVEClIB
   * operations.
   * Requires a 129-bit shift left (((x || y) << 1) -> (t || x || y))
   * and 65-bit compare (t || x) > z).
   * */
  int i;
  vb64_t ge;
  vui64_t t, c, xt;
  const vui64_t ones = vec_splat_u64(1);
  const vui64_t zeros = vec_splat_u64(0);

  for (i = 1; i <= 64; i++)
    {
      /* Left shift (x || y) requires 129-bits, -> (t || x || y) */
      /* capture high bits of x and y into t and c. */
      c = vec_vrld (y, ones);
      /* capture high bit of x as bool t */
#if defined (_ARCH_PWR8)
      /* capture high bit of x as bool t */
      t = (vui64_t) vec_cmpltsd ((vi64_t) x, (vi64_t) zeros);
#else
      /* P7 vec_cmpltsd is complicated, vec_setb_sd() is simpler */
      t = (vui64_t) vec_setb_sd ((vi64_t) x);
#endif
      y = vec_addudm (y, y); /* Shift left 1, x and y */
      x = vec_addudm (x, x);
      /* Propagate carry from y to x */
      x = vec_selud (x, c, (vb64_t) ones);

#if defined (_ARCH_PWR8)
      // vec_cmpge (x_y,z) is NOT vec_cmpgt (z, x_y)
      ge = (vui64_t) vec_cmpgtud (z, (vui64_t)x_y);
      // Combine t with (x >= z) for 129-bit compare
      ge = (vui64_t) vec_orc ((vui32_t)t, (vui32_t)ge);
#else
      // deconstruct ((t || x) >= z) to (t || (x >= z))
      ge = vec_cmpgeud (x, z);
      // Combine t with (x >= z) for 65-bit compare
      ge = (vb64_t) vec_or ((vui32_t)t, (vui32_t)ge);
#endif

      // if (x >= z) x = x - z ; y++
      xt = vec_subudm (x, z);
      // if ((t || x) >= z) {x = xt; y++}
      // Instead of add, OR the boolean ge into bit_0 of y
      y = vec_selud (y, (vui64_t) ge, (vb64_t) ones);
      // Select next x value
      x = vec_selud (x, xt, ge);
    }
  return y;
#endif
}

vui64_t test_vec_moddud (vui64_t x, vui64_t y, vui64_t z)
{
#if defined (_ARCH_PWR8)
  vui64_t Q, R, Qt, Rt;
  vui64_t r1, r2, q1, q2;
  vb64_t CC, c1, c2;
  const vui64_t ones = vec_splat_u64(1);

  // Based on the PowerISA, Programming Note for
  // Divide Word Extended [Unsigned] but vectorized
  // for vector long long int
  q1 = test_vec_divude (x, z);
  q2 = test_vec_divud  (y, z);
  r1 = vec_muludm (q1, z);

  r2 = vec_muludm (q2, z);
  r2 = vec_subudm (y, r2);
  Q  = vec_addudm (q1, q2);
  R  = vec_subudm (r2, r1);

  c1 = vec_cmpltud (R, r2);
  c2 = vec_cmpgeud (R, z);
  CC = vec_or (c1, c2);

  Qt = vec_addudm (Q, ones);
  Q = vec_selud (Q, Qt, CC);
  Rt = vec_subudm (R, z);
  R = vec_selud (R, Rt, CC);

  return R;
#elif defined (_ARCH_PWR7)
  // P7 Missing some DW operations, so use divqud to avoid them.
  vui128_t xy_h, xy_l;
  vui64_t RR, RQ_h, RQ_l, z_l;

  xy_h = (vui128_t) vec_mrghd (x, y);
  xy_l = (vui128_t) vec_mrgld (x, y);
  z_l  = vec_swapd (z);
  RQ_h = test_divqud (xy_h, z);
  RQ_l = test_divqud (xy_l, z_l);
  RR   = vec_mrghd (RQ_h, RQ_l);
  return RR;
#else
  /* Based on Hacker's Delight (2nd Edition) Figure 9-2.
   * "Vector Divide unsigned doubleword, using shift-and-subtract algorithm."
   * Converted to use vector unsigned long long and PVEClIB
   * operations.
   * Requires a 129-bit shift left (((x || y) << 1) -> (t || x || y))
   * and 65-bit compare (t || x) > z).
   * */
  int i;
  vb64_t ge;
  vui64_t t, c, xt;
  const vui64_t ones = vec_splat_u64(1);
  const vui64_t zeros = vec_splat_u64(0);

  for (i = 1; i <= 64; i++)
    {
      /* Left shift (x || y) requires 129-bits, -> (t || x || y) */
      /* capture high bits of x and y into t and c. */
      c = vec_vrld (y, ones);
      /* capture high bit of x as bool t */
#if defined (_ARCH_PWR8)
      /* capture high bit of x as bool t */
      t = (vui64_t) vec_cmpltsd ((vi64_t) x, (vi64_t) zeros);
#else
      /* P7 vec_cmpltsd is complicated, vec_setb_sd() is simpler */
      t = (vui64_t) vec_setb_sd ((vi64_t) x);
#endif
      y = vec_addudm (y, y); /* Shift left 1, x and y */
      x = vec_addudm (x, x);
      /* Propagate carry from y to x */
      x = vec_selud (x, c, (vb64_t) ones);

      // deconstruct ((t || x) >= z) to (t || (x >= z))
      ge = vec_cmpgeud (x, z);
      // Combine t with (x >= z) for 65-bit compare
      ge = (vb64_t) vec_or ((vui32_t)ge, (vui32_t)t);

      // if (x >= z) x = x - z ; y++
      xt = vec_subudm (x, z);
      // if ((t || x) >= z) {x = xt; y++}
      // Instead of add, OR the boolean ge into bit_0 of y
      y = vec_selud (y, (vui64_t) ge, (vb64_t) ones);
      // Select next x value
      x = vec_selud (x, xt, ge);
    }
  return x;
#endif
}

vui64_t test_vec_divdud_V1 (vui64_t x, vui64_t y, vui64_t z)
{
#if defined (_ARCH_PWR9)
  vui64_t Q, R, Qt /*, Rt*/;
  vui64_t r1, r2, q1, q2;
  vb64_t CC, c1, c2;
  const vui64_t ones = vec_splat_u64(1);

  // Based on the PowerISA, Programming Note for
  // Divide Word Extended [Unsigned] but vectorized
  // for vector long long int
  q1 = test_vec_divude (x, z);
  q2 = test_vec_divud  (y, z);
  r1 = vec_muludm (q1, z);

  r2 = vec_muludm (q2, z);
  r2 = vec_subudm (y, r2);
  Q  = vec_addudm (q1, q2);
  R  = vec_subudm (r2, r1);

  c1 = vec_cmpltud (R, r2);
  c2 = vec_cmpgeud (R, z);
  CC = vec_or (c1, c2);

  Qt = vec_addudm (Q, ones);
  Q = vec_selud (Q, Qt, CC);
#if 0 // Corrected Remainder not returned
  Rt = vec_subudm (R, z);
  R = vec_selud (R, Rt, CC);
#endif
  return Q;
#else
  int i;
  vb64_t ge;
  vui64_t t, c, xt;
  const vui64_t ones = vec_splat_u64(1);
#if defined (_ARCH_PWR8)
  const vui64_t zeros = vec_splat_u64(0);
#endif

  for (i = 1; i <= 64; i++)
    {
      /* Left shift (x || y) requires 129-bits, -> (t || x || y) */
      /* capture high bits of x and y into t and c. */
      c = vec_vrld (y, ones);
#if defined (_ARCH_PWR8)
      /* capture high bit of x as bool t */
      t = (vui64_t) vec_cmpltsd ((vi64_t) x, (vi64_t) zeros);
#else
      t = (vui64_t) vec_setb_sd ((vi64_t) x);
#endif
      y = vec_addudm (y, y); /* Shift left 1, x and y */
      x = vec_addudm (x, x);
      /* Propagate carry from y to x */
      x = vec_selud (x, c, (vb64_t) ones);

#if defined (_ARCH_PWR8)
      // vec_cmpge (x_y,z) is NOT vec_cmpgt (z, x)
      ge = vec_cmpgtud (z, x);
      // Combine t with (x >= z) for 129-bit compare
      ge = (vb64_t) vec_orc ((vui32_t)t, (vui32_t)ge);
#else
      // deconstruct ((t || x) >= z) to (t || (x >= z))
      ge = vec_cmpgeud (x, z);
      // Combine t with (x >= z) for 65-bit compare
      ge = (vb64_t) vec_or ((vui32_t)t, (vui32_t)ge);
#endif

      // if (x >= z) x = x - z ; y++
      xt = vec_subudm (x, z);
      // if ((t || x) >= z) {x = xt; y++}
      // Instead of add, OR the boolean ge into bit_0 of y
      y = vec_selud (y, (vui64_t) ge, (vb64_t) ones);
      // Select next x value
      x = vec_selud (x, xt, ge);
    }
  return y;
#endif
}

/* Based on Hacker's Delight (2nd Edition) Figure 9-2.
 * "Vector Divide unsigned doubleword, using shift-and-subtract algorithm."
 * Converted to use vector unsigned long long and PVEClIB
 * operations.
 * Requires a 129-bit shift left (((x || y) << 1) -> (t || x || y))
 * and 65-bit compare (t || x) > z).
 * */
vui64_t test_vec_divdud_V0 (vui64_t x, vui64_t y, vui64_t z)
{
  int i;
  vb64_t ge;
  vui64_t t, c, xt;
  //const vui64_t ones = vec_splat_u64(1);
  //const vui64_t zeros = vec_splat_u64(0);

  for (i = 1; i <= 64; i++)
    {
      /* Left shift (x || y) requires 129-bits, -> (t || x || y) */
      /* capture high bits of x and y into t and c. */
      c = vec_srdi (y, 63);
      /* capture high bit of x as bool t */
      t = (vui64_t) vec_sradi ((vi64_t)x, 63);
      y = vec_addudm (y, y); /* Shift left 1, x and y */
      x = vec_addudm (x, x);
      /* Propagate carry from y to x */
      x = vec_addudm (x, c);

      // deconstruct ((t || x) >= z) to (t || (x >= z))
      ge = vec_cmpgeud (x, z);
      // Combine t with (x >= z) for 65-bit compare
      ge = (vb64_t) vec_or ((vui32_t)t, (vui32_t)ge);
      // Convert bool to carry-bit for conditional y+1
      t  = vec_srdi ((vui64_t)ge, 63);

      /* if (x >= z) x = x - z ; y++ */
      xt = vec_subudm (x, z);
      /* if ((t || x) >= z) {x = xt; y++} */
      y = (vui64_t) vec_or ((vui32_t)y, (vui32_t)t);
      x = vec_selud (x, xt, ge);
    }
  return y;
}

vui64_t test_vec_divqud (vui128_t x_y, vui64_t z)
{
#if defined (_ARCH_PWR10) && (__GNUC__ >= 12)
  // Circular dependency between int64 and int128, thinking
  vui128_t Dv, Q, R, t;

#if 1
  // Use compiler intrinsics to avoid dependency on vec_int128-ppc.h
  const vui64_t zero = { 0, 0 };
  Dv = (vui128_t) vec_mrgahd ((vui128_t) zero, (vui128_t) z);
  Q  = vec_div (x_y, Dv);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t  = vec_mule ((vui64_t)Q, (vui64_t) Dv);
#else
  t  = vec_mulo ((vui64_t)Q, (vui64_t) Dv);
#endif
  R = vec_sub (x_y, t);
#else
  // This would require vec_int128_ppc.h to be completely compiled
  // before vec_int64_ppc.h. Nope!
  Dv = vec_srqi ((vui128_t) z, 64);
  Q  = vec_vdivuq_inline (x_y, Dv);
  t  = vec_vmuloud ((vui64_t)Q, (vui64_t) Dv);
  R  = vec_subuqm (x_y, t);
#endif

  return vec_mrgald (R, Q);
#elif defined (_ARCH_PWR7)
  // POWER8/9 Do not have vector integer divide, but do have
  // Move To/From Vector-Scalar Register Instructions
  // So we can use the scalar hardware divide/divide extended
  __VEC_U_128 qu, xy, zu;
#if (__GNUC__ <= 10)
  xy.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) x_y, 1);
  xy.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) x_y, 0);
  zu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) z, 1);
  zu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) z, 0);
#else
  // Looks like AT16 handles this but what about 15/14 ...
  // AT10 does not.
  xy.vx1 = x_y;
  zu.vx2 = z;
#endif
  unsigned long long Dh = xy.ulong.upper;
  unsigned long long Dl = xy.ulong.lower;
  unsigned long long Dv = zu.ulong.upper;
  unsigned long long q1, q2, Q;
  unsigned long long r1, r2, R;

  // Transfer to GPUs and use scalar divide/divide extended
  // Based on the PowerISA, Programming Note for
  // Divide Word Extended [Unsigned]
  q1 = __builtin_divdeu (Dh, Dv);
  //r1 = -(q1 * Dv);
  r1 = (q1 * Dv);
  q2 = Dl / Dv;
  r2 = Dl - (q2 * Dv);
  Q = q1 + q2;
  //R = r1 + r2;
  R = r2 - r1;
  if ((R < r2) | (R >= Dv))
    {
      Q++;
      R = R - Dv;
    }

  // Transfer R|Q back to VRs and return
  qu.ulong.upper = R;
  qu.ulong.lower = Q;
  return qu.vx2;
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
  vui64_t ge;
  //vui128_t cc, c;
  vui64_t t, xt, mone;
  const vui64_t zeros = vec_splat_u64(0);
  // t = (vui64_t) CONST_VINT128_DW (0, 0);
  mone = (vui64_t) CONST_VINT128_DW (-1, -1);
  /* Here only using the high DW of z, generated z as {z'', -1} */
  z = vec_pasted (z, mone);

  for (i = 1; i <= 64; i++)
    {
      // Left shift (x || y) requires 129-bits, is (t || x || y)
      /* capture high bit of x_y as bool t */
#if defined (_ARCH_PWR8)
      t = (vui64_t) vec_cmpltsd ((vi64_t) x_y, (vi64_t) zeros);
#else
      { // P7 and earlier did not support DW int.
	// But only need to convert the sign-bit into a bool
	vui32_t lts;
	lts = (vui32_t) vec_cmplt ((vi32_t) x_y, (vi32_t) zeros);
	t = (vui64_t) vec_splat (lts, VEC_W_H);
      }
#endif
      // Then shift left Quadword x_y by 1 bit;
      x_y = vec_slqi (x_y, 1);
      /* We only need the high DW of t and ge */
      /* deconstruct ((t || x) >= z) to (t || (x >= z)) */
#if defined (_ARCH_PWR8)
      // vec_cmpge (x_y,z) is NOT vec_cmpgt (z, x_y)
      ge = (vui64_t) vec_cmpgtud (z, (vui64_t)x_y);
      /* Combine t with (x >= z) for 129-bit compare */
      ge = (vui64_t) vec_orc ((vui32_t)t, (vui32_t)ge);
#else // P7 and earlier did not support OR Complement
      ge = (vui64_t) vec_cmpgeud ((vui64_t)x_y, z);
      /* Combine t with (x >= z) for 129-bit compare */
      ge = (vui64_t) vec_or ((vui32_t)t, (vui32_t)ge);
#endif
      /* Splat the high ge DW to both DWs for select */
      ge = vec_splatd (ge, VEC_DW_H);

      /* xt <- {(x - z), (y - ( -1)} */
      xt = vec_subudm ((vui64_t)x_y, z);
      x_y = (vui128_t)vec_selud ((vui64_t)x_y, xt, (vb64_t)ge);
    }
  return (vui64_t)x_y;
#endif
}

vui64_t test_vec_divmoddud (vui64_t *r, vui64_t x, vui64_t y, vui64_t z)
{
#if defined (_ARCH_PWR8)
  vui64_t Q, R, Qt, Rt;
  vui64_t r1, r2, q1, q2;
  vb64_t CC, c1, c2;
  const vui64_t ones = vec_splat_u64(1);

  // Based on the PowerISA, Programming Note for
  // Divide Word Extended [Unsigned] but vectorized
  // for vector long long int
  q1 = test_vec_divude (x, z);
  q2 = test_vec_divud  (y, z);
  r1 = vec_muludm (q1, z);

  r2 = vec_muludm (q2, z);
  r2 = vec_subudm (y, r2);
  Q  = vec_addudm (q1, q2);
  R  = vec_subudm (r2, r1);

  c1 = vec_cmpltud (R, r2);
  c2 = vec_cmpgeud (R, z);
  CC = vec_or (c1, c2);

  Qt = vec_addudm (Q, ones);
  Q = vec_selud (Q, Qt, CC);
  Rt = vec_subudm (R, z);
  R = vec_selud (R, Rt, CC);
  *r = R;
  return Q;

#else
  /* Based on Hacker's Delight (2nd Edition) Figure 9-2.
   * "Vector Divide unsigned doubleword, using shift-and-subtract algorithm."
   * Converted to use vector unsigned long long and PVEClIB
   * operations.
   * Requires a 129-bit shift left (((x || y) << 1) -> (t || x || y))
   * and 65-bit compare (t || x) > z).
   * */
  int i;
  vb64_t ge;
  vui64_t t, c, xt;
  const vui64_t ones = vec_splat_u64(1);
  const vui64_t zeros = vec_splat_u64(0);

  for (i = 1; i <= 64; i++)
    {
      /* Left shift (x || y) requires 129-bits, -> (t || x || y) */
      /* capture high bits of x and y into t and c. */
      c = vec_vrld (y, ones);
      /* capture high bit of x as bool t */
      t = (vui64_t) vec_cmpltsd ((vi64_t) x, (vi64_t) zeros);
      y = vec_addudm (y, y); /* Shift left 1, x and y */
      x = vec_addudm (x, x);
      /* Propagate carry from y to x */
      x = vec_selud (x, c, (vb64_t) ones);

      // deconstruct ((t || x) >= z) to (t || (x >= z))
      ge = vec_cmpgeud (x, z);
      // Combine t with (x >= z) for 65-bit compare
      ge = (vb64_t) vec_or ((vui32_t)t, (vui32_t)ge);

      // if (x >= z) x = x - z ; y++
      xt = vec_subudm (x, z);
      // if ((t || x) >= z) {x = xt; y++}
      // Instead of add, OR the boolean ge into bit_0 of y
      y = vec_selud (y, (vui64_t) ge, (vb64_t) ones);
      // Select next x value
      x = vec_selud (x, xt, ge);
    }
  *r = x;
  return y;
#endif
}

vui64_t test_vec_divmoddud_V0 (vui64_t *r, vui64_t x, vui64_t y, vui64_t z)
{
  vui64_t Q;
  vui64_t R;

  Q = test_vec_divdud (x, y, z);
  R = test_vec_moddud (x, y, z);

  *r = R;
  return Q;
}
