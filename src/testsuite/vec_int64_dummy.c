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

#include <pveclib/vec_int128_ppc.h>


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
__test_vrld (vui64_t a, vui64_t b)
{
  return vec_vrld (a, b);
}

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
test_rldi_1 (vui64_t a)
{
  return vec_rldi (a, 1);
}

vui64_t
test_rldi_15 (vui64_t a)
{
  return vec_rldi (a, 15);
}

vui64_t
test_rldi_32 (vui64_t a)
{
  return vec_rldi (a, 32);
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
test_srdi_31 (vui64_t a)
{
  return vec_srdi (a, 31);
}

vui64_t
test_srdi_32 (vui64_t a)
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
