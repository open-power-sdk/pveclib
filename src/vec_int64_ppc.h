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

 vec_int32_ppc.h

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Mar 29, 2018
 */

#ifndef VEC_INT64_PPC_H_
#define VEC_INT64_PPC_H_

#include <vec_int32_ppc.h>

/*!
 * \file  vec_int64_ppc.h
 * \brief Header package containing a collection of 128-bit SIMD
 * operations over 64-bit integer elements.
 *
 * The original VMX (AKA Altivec) did not define any doubleword element
 * (long long integer or double float) operations.
 * The VSX facility (introduced with POWER7) added vector double float
 * but did not add any integer doubleword (64-bit) operations.  However
 * it did add a useful doubleword permute immediate and word wise;
 * merge, shift, and splat immediate operations.
 * Otherwise vector long int (64-bit elements) operations
 * have to be implemented using VMX word and halfword element integer
 * operations for POWER7.
 *
 * POWER8 (PowerISA 2.07B) adds important doubleword integer (add,
 * subtract, compare, shift, rotate, ...) VMX operations. POWER8 also
 * added multiply word operations that produce the full doubleword
 * product and full quadword add / subtract (with carry extend).
 *
 * POWER9 (PowerISA 3.0B) adds the <B>Vector Multiply-Sum Unsigned
 * Doubleword Modulo</B> instruction. This is not the expected
 * multiply even/odd/modulo doubleword nor a full multiply modulo
 * quadword. But with a few extra (permutes and splat zero)
 * instructions you can get equivalent function.
 *
 * Most of these intrinsic (compiler built-in) operations are defined
 * in <altivec.h> and described in the compiler documentation.
 *
 * \note The compiler disables associated <altivec.h> built-ins if the
 * <B>mcpu</B> target does not enable the specific instruction.
 * For example if you compile with <B>-mcpu=power7</B>, vec_vclz and
 * vec_vclzd will not be defined.  But vec_clzd is always defined in
 * this header, will generate the minimum code, appropriate for the
 * target, and produce correct results.
 *
 * Most of these
 * operations are implemented in a single instruction on newer
 * (POWER8/POWER9) processors. So this header serves to fill in
 * functional gaps for older (POWER7, POWER8) processors and provides
 * a in-line assembler implementation for older compilers that do not
 * provide the build-ins.
 *
 * This header covers operations that are either:
 *
 * - Implemented in hardware instructions for later
 * processors and useful to programmers, on slightly older processors,
 * even if the equivalent function requires more instructions.
 * Examples include the multiply even/odd/modulo word operations.
 * - Defined in the OpenPOWER ABI but <I>not</I> yet defined in
 * <altivec.n> provided by available compilers in common use.
 * Examples include Count Leading Zeros, Population Count and Byte
 * Reverse.
 * - Commonly used operations, not covered by the ABI or
 * <altivec.h>, and require multiple instructions or
 * are not obvious.  Examples include the shift immediate operations.
 *
 * \note The Multiply even/odd doubleword operations are
 * currently implemented in <vec_int128_ppc.h> which resolves a
 * dependency on Add Quadword. These functions (vec_msumudm,
 * vec_muleud, vec_muloud) all produce a quadword results and need
 * vec_adduqm to sum partial products on earlier Power platforms.
 * \sa vec_adduqm, vec_muleud, vec_muloud, and vec_msumudm
 */

/** \brief Vector Add Unsigned Doubleword Modulo.
 *
 *  Add two vector long int values and return modulo 64-bits result.
 *
 *  @param a 128-bit vector long int.
 *  @param b 128-bit vector long int.
 *  @return vector long int sums of a and b.
 */
static inline
vui64_t
vec_addudm(vui64_t a, vui64_t b)
{
  vui32_t r;

#ifdef _ARCH_PWR8
#ifndef vec_vaddudm
  __asm__(
      "vaddudm %0,%1,%2;"
      : "=v" (r)
      : "v" (a),
      "v" (b)
      : );
#else
  r = (vui32_t) vec_vaddudm (a, b);
#endif
#else
  vui32_t c;
  vui32_t z= { 0,0,0,0};
  vui32_t cm= { 0,1,0,1};

  c = vec_vaddcuw ((vui32_t)a, (vui32_t)b);
  r = vec_vadduwm ((vui32_t)a, (vui32_t)b);
  c = vec_and (c, cm);
  c = vec_sld (c, z, 4);
  r = vec_vadduwm (r, c);
#endif
  return ((vui64_t) r);
}

/** \brief Count leading zeros for a vector unsigned long int.
 *
 *	Count leading zeros for a vector __int128 and return the count in a
 *	vector suitable for use with vector shift (left|right) and vector
 *	shift (left|right) by octet instructions.
 *
 *	@param vra a 128-bit vector treated a __int128.
 *	@return a 128-bit vector with bits 121:127 containing the count of
 *	leading zeros.
 */
static inline vui64_t
vec_clzd (vui64_t vra)
{
  vui64_t r;
#ifdef _ARCH_PWR8
#ifndef vec_vclzd
  __asm__(
      "vclzd %0,%1;"
      : "=v" (r)
      : "v" (vra)
      : );
#else
  r = vec_vclzd (vra);
#endif
#else
  //#warning Implememention pre power8
  vui32_t n, nt, y, x, m;
  vui32_t z = { 0, 0, 0, 0 };
  vui32_t dlwm = { 0, -1, 0, -1 };

  x = (vui32_t) vra;

  m = (vui32_t) vec_cmpgt (x, z);
  n = vec_sld (z, m, 12);
  y = vec_and (n, dlwm);
  nt = vec_or (x, y);

  n = vec_clzw (nt);
  r = (vui64_t)vec_sum2s ((vi32_t)n, (vi32_t)z);
#endif
  return (r);
}

/** \brief Vector Compare Equal Unsigned Doubleword.
 *
 *  Compare each unsigned long (64-bit) integer and return all '1's,
 *  if a[i] == b[i], otherwise all '0's.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the Vector Population
 *  Count DoubleWord (<B>vcmpequd</B>) instruction. Otherwise use
 *  boolean logic using word compares.
 *
 *  @param a 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @return 128-bit vector with each dword boolean reflecting compare
 *  equal result for each element.
 */
static inline
vui64_t
vec_cmpequd (vui64_t a, vui64_t b)
{
  vui64_t result;
#ifdef _ARCH_PWR8
#if __GNUC__ >= 7
  result = (vui64_t)vec_cmpeq((__vector bool long long)a, (__vector bool long long)b);
#else
  __asm__(
      "vcmpequd %0,%1,%2;\n"
      : "=v" (result)
      : "v" (a),
      "v" (b)
      : );
#endif
#else
  /*
   * Don't have vector compare equal unsigned doubleword until power8.
   * So we have to compare word and unless all_eq we need to do some
   * extra work, ie the words may have different truth values.  So we
   * rotate each doubleword by 32-bits (here we use permute as we don't
   * have rotate doubleword either). Then vand the original word
   * compare and rotated value to get the final value.
   */
  vui8_t permute =
    { 0x04,0x05,0x6,0x7, 0x00,0x01,0x2,0x03, 0x0C,0x0D,0x0E,0x0F, 0x08,0x09,0x0A,0x0B};
  vui32_t r, rr;
  r = (vui32_t)vec_cmpeq((vui32_t)a, (vui32_t)b);
  if (vec_any_ne((vui32_t)a, (vui32_t)b))
    {
       rr = vec_perm (r, r, permute);
       r= vec_and (r, rr);
    }
  result = (vui64_t)r;
#endif
  return (result);
}

/** \brief Vector Compare all Equal Unsigned Doubleword.
 *
 *  Compare each unsigned long (64-bit) integer and return true if all
 *  elements of a and b are equal.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the vec_all_eq built-in
 *  predicate directly.  Otherwise cast to unsigned word and use the
 *  same predicate generating (<B>vcmpequw</B>).
 *
 *  @param a 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @return boolean int for all 128-bits, true if equal, false otherwise.
 */
static inline
int
vec_cmpud_all_eq (vui64_t a, vui64_t b)
{
  int result;
#ifdef _ARCH_PWR8
#if __GNUC__ >= 7
  result = vec_all_eq((__vector bool long long)a, (__vector bool long long)b);
#else
  result = vec_all_eq((vui32_t)a, (vui32_t)b);
#endif
#else
  result = vec_all_eq((vui32_t)a, (vui32_t)b);
#endif
  return (result);
}

/** \brief Vector Compare Greater Than Unsigned Doubleword.
 *
 *  Compare each unsigned long (64-bit) integer and return all '1's,
 *  if a[i] > b[i], otherwise all '0's.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the Vector Population
 *  Count DoubleWord (<B>vcmpgtud</B>) instruction. Otherwise use
 *  boolean logic using word compares.
 *
 *  @param a 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @return 128-bit vector with each dword boolean reflecting compare
 *  greater result for each element.
 */
static inline
vui64_t
vec_cmpgtud (vui64_t a, vui64_t b)
{
  vui64_t result;
#ifdef _ARCH_PWR8
#if __GNUC__ >= 7
  result = (vui64_t)vec_cmpgt(a, b);
#else
  __asm__(
      "vcmpgtud %0,%1,%2;\n"
      : "=v" (result)
      : "v" (a),
      "v" (b)
      : );
#endif
#else
  /*
   * Don't have vector compare greater than unsigned doubleword until
   * power8.  So we have to use compare word and logic to compute the
   * doubleword truth values.
   */
  __vector unsigned int r, x, y;
  __vector unsigned int c0, c1, c01;
  __vector unsigned int eq, gt, a32, b32;

  /* c10 = {0, -1, 0, -1}  */
  c0 = vec_splat_u32 (0);
  c1 = vec_splat_u32 (-1);
  c01 = vec_mergeh (c0, c1);

  a32 = (__vector unsigned int)a;
  b32 = (__vector unsigned int)b;

  gt = (__vector unsigned int)vec_cmpgt (a32, b32);
  eq = (__vector unsigned int)vec_cmpeq (a32, b32);
  /* GTxw = GThw | (EQhw & GTlw)  */
  x = vec_sld (gt, c0, 4);
  y = vec_and (eq, x);
  x = vec_or  (gt, y);
  /* Duplicate result word to dword width.  */
  y = vec_sld (c0, x, 12);
  r = vec_sel (x, y, c01);
  result = (vui64_t)r;
#endif
  return (result);
}

/** \brief Vector Compare all Greater Than Unsigned Doubleword.
 *
 *  Compare each unsigned long (64-bit) integer and return true if all
 *  elements of a > b.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the vec_all_eq built-in predicate directly.
 *  Otherwise case to unsigned word and use the same predicate generating (<B>vcmpequw</B>).
 *
 *  @param a 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @return boolean int for all 128-bits, true if all Greater Than,
 *  false otherwise.
 */
static inline
int
vec_cmpud_any_gt (vui64_t a, vui64_t b)
{
  int result;
#ifdef _ARCH_PWR8
#if __GNUC__ >= 7
  result = vec_any_gt(a, b);
#else
  vui32_t wt = { -1, -1, -1, -1};
  vui64_t gt_bool = vec_cmpgtud (a, b);
  result = vec_any_eq((vui32_t)gt_bool, wt);
#endif
#else
  vui32_t wt= { -1, -1, -1, -1};
  vui64_t gt_bool= vec_cmpgtud (a, b);
  result = vec_any_eq((vui32_t)gt_bool, wt);
#endif
  return (result);
}

/** \brief Vector Compare all Greater Than Unsigned Doubleword.
 *
 *  Compare each unsigned long (64-bit) integer and return true if all
 *  elements of a > b.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the vec_all_eq built-in
 *  predicate directly.  Otherwise cast to unsigned word and use the
 *  same predicate generating (<B>vcmpequw</B>).
 *
 *  @param a 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @return boolean int for all 128-bits, true if all Greater Than,
 *  false otherwise.
 */
static inline
int
vec_cmpud_all_gt (vui64_t a, vui64_t b)
{
  int result;
#ifdef _ARCH_PWR8
#if __GNUC__ >= 7
  result = vec_all_gt(a, b);
#else
  vui32_t wt = { -1, -1, -1, -1};
  vui64_t gt_bool = vec_cmpgtud (a, b);
  result = vec_all_eq((vui32_t)gt_bool, wt);
#endif
#else
  vui32_t wt= { -1, -1, -1, -1};
  vui64_t gt_bool= vec_cmpgtud (a, b);
  result = vec_all_eq((vui32_t)gt_bool, wt);
#endif
  return (result);
}


/** \brief Vector Compare Less Than Equal Unsigned Doubleword.
 *
 *  Compare each unsigned long (64-bit) integer and return all '1's,
 *  if a[i] > b[i], otherwise all '0's.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the Vector Population
 *  Count DoubleWord (<B>vcmpgtud</B>) instruction. Otherwise use
 *  boolean logic using word compares.
 *
 *  @param a 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @return 128-bit vector with each dword boolean reflecting compare
 *  greater result for each element.
 */
static inline
vui64_t
vec_cmpleud (vui64_t a, vui64_t b)
{
  vui64_t result;
#ifdef _ARCH_PWR8
#if __GNUC__ >= 7
  result = (vui64_t)vec_cmple(a, b);
#else
  __asm__(
      "vcmpgtud %0,%1,%2;\n"
      "xxlnor %0,%0,%0;\n"
      : "=&v" (result)
      : "v" (a),
      "v" (b)
      : );
#endif
#else
  vui64_t r = vec_cmpgtud (a, b);
  result = vec_nor(r, r);;
#endif
  return (result);
}

/** \brief Vector Compare all Less than equal Unsigned Doubleword.
 *
 *  Compare each unsigned long (64-bit) integer and return true if all
 *  elements of a > b.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the vec_all_eq built-in
 *  predicate directly.  Otherwise cast to unsigned word and use the
 *  same predicate generating (<B>vcmpequw</B>).
 *
 *  @param a 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @return boolean int for all 128-bits, true if all Greater Than,
 *  false otherwise.
 */
static inline
int
vec_cmpud_all_le (vui64_t a, vui64_t b)
{
  int result;
#ifdef _ARCH_PWR8
#if __GNUC__ >= 7
  result = vec_all_le(a, b);
#else
  vui32_t wt = { -1, -1, -1, -1};
  vui64_t gt_bool = vec_cmpleud (a, b);
  result = vec_all_eq((vui32_t)gt_bool, wt);
#endif
#else
  vui32_t wt= { -1, -1, -1, -1};
  vui64_t gt_bool= vec_cmpleud (a, b);
  result = vec_all_eq((vui32_t)gt_bool, wt);
#endif
  return (result);
}

static inline vui64_t
vec_permdi (vui64_t vra, vui64_t vrb, const int ctl);

/** \brief Vector Merge High Doubleword.
 *  Merge the high doubleword elements from two vectors into the high
 *  and low doubleword elements of the result.
 *
 *  @param __VA a 128-bit vector as the source of the
 *  high order doubleword.
 *  @param __VB a 128-bit vector as the source of the
 *  low order doubleword.
 *  @return The original vector with the doubleword elements swapped.
 */
static inline vui64_t
vec_mrghd (vui64_t __VA, vui64_t __VB)
{
  vui64_t result;
  /*
   result[0] = __VA[0];
   result[1] = __VB[0];
   */
  result = vec_permdi (__VA, __VB, 0);

  return (result);
}

/** \brief Vector Merge Low Doubleword.
 *  Merge the low doubleword elements from two vectors into the high
 *  and low doubleword elements of the result.
 *
 *  @param __VA a 128-bit vector as the source of the
 *  high order doubleword.
 *  @param __VB a 128-bit vector as the source of the
 *  low order doubleword.
 *  @return The original vector with the doubleword elements swapped.
 */
static inline vui64_t
vec_mrgld (vui64_t __VA, vui64_t __VB)
{
  vui64_t result;
  /*
   result[0] = __VA[1];
   result[1] = __VB[1];
   */
  result = vec_permdi (__VA, __VB, 3);

  return (result);
}

/** \brief Vector doubleword paste.
 *	Concatenate the high doubleword of the 1st vector with the
 *	low double word of the 2nd vector.
 *
 *	@param __VH a 128-bit vector as the source of the
 * 	high order doubleword.
 *	@param __VL a 128-bit vector as the source of the
 * 	low order doubleword.
 *	@return The combined 128-bit vector composed of the high order
 *	doubleword of __VH and the low order doubleword of __VL.
 */
static inline vui64_t
vec_pasted (vui64_t __VH, vui64_t __VL)
{
  vui64_t result;
  /*
   result[1] = __VH[1];
   result[0] = __VL[0];
   */
  result = vec_permdi (__VH, __VL, 1);

  return (result);
}

/** \brief Vector Permute Doubleword Immediate.
 *  Combine a doubleword selected from the 1st (vra) vector with
 *  a doubleword selected from the 2nd (vrb) vector. The 2-bit control
 *  operand (ctl) selects which doubleword from the 1st and 2nd
 *  vector operands are transfered to the result vector.
 *
 *  ctl |  vrt[0:63]  | vrt[64:127]
 *  :-: | :---------: | :----------:
 *   0  |  vra[0:63]  | vrb[0:63]
 *   1  |  vra[0:63]  | vrb[64:127]
 *   2  | vra[64:127] | vrb[0:63]
 *   3  | vra[64:127] | vrb[64:127]
 *
 *  @param vra a 128-bit vector as the source of the
 *  high order doubleword of the result.
 *  @param vrb a 128-bit vector as the source of the
 *  low order doubleword of the result.
 *  @param ctl const integer where the low order 2 bits control the
 *  selection of doublewords from input vector vra and vrb.
 *  @return The combined 128-bit vector composed of the high order
 *  doubleword of vra and the low order doubleword of vrb.
 */
static inline vui64_t
vec_permdi (vui64_t vra, vui64_t vrb, const int ctl)
{
  vui64_t result;
#ifdef _ARCH_PWR7
  switch (ctl & 3)
    {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    case 0:
      result = vec_xxpermdi (vra, vrb, 0);
      break;
    case 1:
      result = vec_xxpermdi (vra, vrb, 1);
      break;
    case 2:
      result = vec_xxpermdi (vra, vrb, 2);
      break;
    case 3:
      result = vec_xxpermdi (vra, vrb, 3);
      break;
#else
    case 0:
      result = vec_xxpermdi (vrb, vra, 3);
      break;
    case 1:
      result = vec_xxpermdi (vrb, vra, 1);
      break;
    case 2:
      result = vec_xxpermdi (vrb, vra, 2);
      break;
    case 3:
      result = vec_xxpermdi (vrb, vra, 0);
      break;
#endif
    default:
      result = (vui64_t){ 0, 0 };
    }
#else
  /* Current compilers don't accept vector unsigned long int as vector
   * parms to vec_sld, so use vector unsigned int.  The vsldoi
   * instruction does not care).  */
  vui32_t temp;
  switch (ctl & 3)
    {
      case 0:
      temp = vec_sld ((vui32_t) vra, (vui32_t) vra, 8);
      result = (vui64_t) vec_sld (temp, (vui32_t) vrb, 8);
      break;
      case 1:
      temp = vec_sld ((vui32_t) vrb, (vui32_t) vra, 8);
      result = (vui64_t) vec_sld (temp, temp, 8);
      break;
      case 2:
      result = (vui64_t) vec_sld ((vui32_t) vra, (vui32_t) vrb, 8);
      break;
      case 3:
      temp = vec_sld ((vui32_t) vrb, (vui32_t) vrb, 8);
      result = (vui64_t) vec_sld ((vui32_t) vra, temp, 8);
      break;
    }
#endif
  return (result);
}

/** \brief Vector Population Count doubleword.
 *
 *  Count the number of '1' bits (0-64) within each doubleword element
 *  of a 128-bit vector.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the Vector Population
 *  Count DoubleWord (<B>vpopcntd</B>) instruction. Otherwise use the
 *  pveclib vec_popcntw to count each word then sum across with Vector
 *  Sum across Half Signed Word Saturate (<B>vsum2sws</B>).
 *
 *  @param vra 128-bit vector treated as 2 x 64-bit integer (dwords)
 *  elements.
 *  @return 128-bit vector with the population count for each dword
 *  element.
 */
#ifndef vec_popcntd
static inline vui64_t
vec_popcntd (vui64_t vra)
{
  vui64_t r;
#ifdef _ARCH_PWR8
#ifndef vec_vpopcntd
  __asm__(
      "vpopcntd %0,%1;"
      : "=v" (r)
      : "v" (vra)
      : );
#else
  r = vec_vpopcntd (vra);
#endif
#else
  //#warning Implememention pre power8
  vui32_t z= { 0,0,0,0};
  vui32_t x;
  x = vec_popcntw ((vui32_t)vra);
  r = (vui64_t)vec_sum2s ((vi32_t)x, (vi32_t)z);
#endif
  return (r);
}
#else
/* Work around for GCC PR85830.  */
#undef vec_popcntd
#define vec_popcntd __builtin_vec_vpopcntd
#endif

/*! \brief byte reverse each doubleword for a vector unsigned long int.
 *
 *	For each doubleword of the input vector, reverse the order of
 *	bytes / octets within the doubleword.
 *
 *	@param vra a 128-bit vector unsigned long int.
 *	@return a 128-bit vector with the bytes of each doubleword
 *	reversed.
 */
static inline vui64_t
vec_revbd (vui64_t vra)
{
  vui64_t result;

#ifdef _ARCH_PWR9
#ifndef vec_revb
  __asm__(
      "xxbrd %x0,%x1;"
      : "=wa" (result)
      : "wa" (vra)
      : );
#else
  result = vec_revb (vra);
#endif
#else
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  const vui64_t vconstp = CONST_VINT64_DW(0x0706050403020100UL, 0x0F0E0D0C0B0A0908UL);
#else
  const vui64_t vconstp =
      CONST_VINT64_DW(0x08090A0B0C0D0E0FUL, 0x0001020304050607UL);
#endif
  result = (vui64_t) vec_perm ((vui8_t) vra, (vui8_t) vra, (vui8_t) vconstp);
#endif

  return (result);
}

#ifndef vec_vsld
static inline vui64_t vec_vsld (vui64_t vra, vui64_t vrb);
static inline vui64_t vec_vsrd (vui64_t vra, vui64_t vrb);
static inline vi64_t vec_vsrad (vi64_t vra, vui64_t vrb);
#endif


/** \brief Vector Shift left Doubleword Immediate.
 *
 *	Vector Shift left Doublewords each element [0-1], 0-63 bits,
 *	as specified by an immediate value.
 *	The shift amount is a const unsigned long int in the range 0-63.
 *	A shift count of 0 returns the original value of vra.
 *	Shift counts greater then 63 bits return zero.
 *
 *	@param vra a 128-bit vector treated as a vector unsigned long int.
 *	@param shb shift amount in the range 0-63.
 *	@return 128-bit vector unsigned long int, shifted left shb bits.
 */
static inline vui64_t
vec_sldi (vui64_t vra, const unsigned int shb)
{
  vui64_t lshift;
  vui64_t result;

  if (shb < 64)
    {
      /* Load the shift const in a vector.  The element shifts require
         a shift amount for each element. For the immediate form the
         shift constant is splatted to all elements of the
         shift control.  */
      if (__builtin_constant_p (shb) && (shb < 16))
	lshift = (vui64_t) vec_splat_s32(shb);
      else
	lshift = vec_splats ((unsigned long) shb);

      /* Vector Shift right bytes based on the lower 6-bits of
         corresponding element of lshift.  */
      result = vec_vsld (vra, lshift);
    }
  else
    { /* shifts greater then 31 bits return zeros.  */
      result = vec_xor ((vui64_t) vra, (vui64_t) vra);
    }

  return (vui64_t) result;
}

/** \brief Vector splat doubleword.
 *  Duplicate the selected doubleword element across the doubleword
 *  elements of the result.
 *
 *  ctl |  vrt[0:63]  | vrt[64:127]
 *  :-: | :---------: | :----------:
 *   0  |  vra[0:63]  | vra[0:63]
 *   1  | vra[64:127] | vra[64:127]
 *
 *  @param vra a 128-bit vector.
 *  @param ctl a const integer encoding the source doubleword.
 *  @return The original vector with the doubleword elements swapped.
 */
static inline vui64_t
vec_spltd (vui64_t vra, const int ctl)
{
  vui64_t result;
  /* Don't need to reverse the cases for LE because vec_permdi handles
     that.  */
  switch (ctl & 1)
    {
    case 0:
      /*
       result[1] = vra[0];
       result[0] = vra[0];
       */
      result = vec_permdi (vra, vra, 0);
      break;
    case 1:
      /*
       result[1] = vra[1];
       result[0] = vra[1];
       */
      result = vec_permdi (vra, vra, 3);
      break;
    }

  return (result);
}

/** \brief Vector Shift Right Doubleword Immediate.
 *
 *	Vector Shift Right Doublewords each element [0-1], 0-63 bits,
 *	as specified by an immediate value.
 *	The shift amount is a const unsigned int in the range 0-63.
 *	A shift count of 0 returns the original value of vra.
 *	Shift counts greater then 63 bits return zero.
 *
 *	@param vra a 128-bit vector treated as a vector unsigned long int.
 *	@param shb shift amount in the range 0-63.
 *	@return 128-bit vector unsigned long int, shifted right shb bits.
 */
static inline vui64_t
vec_srdi (vui64_t vra, const unsigned int shb)
{
  vui64_t lshift;
  vui64_t result;

  if (shb < 32)
    {
      /* Load the shift const in a vector.  The element shifts require
         a shift amount for each element. For the immediate form the
         shift constant is splatted to all elements of the
         shift control.  */
      if (__builtin_constant_p (shb) && (shb < 16))
	lshift = (vui64_t) vec_splat_s32(shb);
      else
	lshift = vec_splats ((unsigned long) shb);

      /* Vector Shift right bytes based on the lower 6-bits of
         corresponding element of lshift.  */
      result = vec_vsrd (vra, lshift);
    }
  else
    { /* shifts greater then 63 bits return zeros.  */
      result = vec_xor ((vui64_t) vra, (vui64_t) vra);
    }
  return (vui64_t) result;
}

/** \brief Vector Shift Right Algebraic Doubleword Immediate.
 *
 *  Vector Shift Right Algebraic Doublewords each element [0-1],
 *  0-63 bits, as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-63.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 63 bits return the sign bit
 *  propagated to each bit of each element.
 *
 *  @param vra a 128-bit vector treated as a vector signed long int.
 *  @param shb shift amount in the range 0-63.
 *  @return 128-bit vector signed long int, shifted right shb bits.
 */
static inline vi64_t
vec_sradi (vi64_t vra, const unsigned int shb)
{
  vui64_t lshift;
  vi64_t result;

  if (shb < 64)
    {
      /* Load the shift const in a vector.  The element shifts require
         a shift amount for each element. For the immediate form the
         shift constant is splatted to all elements of the
         shift control.  */
      if (__builtin_constant_p (shb) && (shb < 16))
	lshift = (vui64_t) vec_splat_s32(shb);
      else
	lshift = vec_splats ((unsigned long) shb);

      /* Vector Shift Right Algebraic Doublewords based on the lower 6-bits
         of corresponding element of lshift.  */
      result = vec_vsrad (vra, lshift);
    }
  else
    { /* shifts greater then 63 bits returns the sign bit propagated to
         all bits.   This is equivalent to shift Right Algebraic of
         63 bits.  */
      lshift = (vui64_t) vec_splats(63);
      result = vec_vsrad (vra, lshift);
    }

  return (vi64_t) result;
}

/** \brief Vector Subtract Unsigned Doubleword Modulo.
 *
 *  For each unsigned long (64-bit) integer element c[i] = a[i] +
 *  NOT(b[i]) + 1.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the Vector Subtract
 *  Unsigned Doubleword Modulo (<B>vsubudm</B>) instruction. Otherwise
 *  use vector add word modulo forms and propagate the carry bits.
 *
 *  @param a 128-bit vector treated as 2 X unsigned long int.
 *  @param b 128-bit vector treated as 2 X unsigned long int.
 *  @return  vector unsigned long int sum of a[0] + NOT(b[0]) + 1
 *  and a[1] + NOT(b[1]) + 1.
 */
static inline
vui64_t
vec_subudm(vui64_t a, vui64_t b)
{
  vui32_t r;

#ifdef _ARCH_PWR8
#ifndef vec_vsubudm
  __asm__(
      "vsubudm %0,%1,%2;"
      : "=v" (r)
      : "v" (a),
      "v" (b)
      : );
#else
  r = (vui32_t) vec_vsubudm (a, b);
#endif
#else
  vui32_t c;
  vui32_t z= { 0,0,0,0};
  vui32_t cm= { 0,1,0,1};

  c = vec_vsubcuw ((vui32_t)a, (vui32_t)b);
  r = vec_vsubuwm ((vui32_t)a, (vui32_t)b);
  c = vec_andc (cm, c);
  c = vec_sld (c, z, 4);
  r = vec_vsubuwm (r, c);
#endif
  return ((vui64_t) r);
}

/** \brief Vector doubleword swap.
 *  Exchange the high and low doubleword elements of a vector.
 *
 *  @param vra a 128-bit vector.
 *  @return The original vector with the doubleword elements swapped.
 */
static inline vui64_t
vec_swapd (vui64_t vra)
{
  vui64_t result;
  /*
   result[1] = vra[0];
   result[0] = vra[1];
   */
  result = vec_permdi (vra, vra, 2);

  return (result);
}

/** \brief Vector Shift Left Doubleword.
 *
 *  Vector Shift Left Doubleword 0-63 bits.
 *  The shift amount is from bits 58-63 and 122-127 of vrb.
 *
 *  \note Can not use vec_sld naming here as that would conflict
 *  with the generic Shift Left Double Vector. Use vec_vsld but only
 *  if the compiler does not define it in <altivec.h>.
 *
 *  @param vra a 128-bit vector treated as 2 x unsigned long integers.
 *  @param vrb shift amount in bits 58:63 and 122:127.
 *  @return Left shifted vector unsigned long.
 */
#ifndef vec_vsld
static inline vui64_t
vec_vsld (vui64_t vra, vui64_t vrb)
{
  vui64_t result;

#ifdef _ARCH_PWR8
  __asm__(
      "vsld %0,%1,%2;"
      : "=v" (result)
      : "v" (vra),
      "v" (vrb)
      : );
#else
  vui8_t  vsh_h, vsh_l;
  vui8_t  vr_h, vr_l;
  vui64_t sel_mask = CONST_VINT128_DW(0, -1LL);
  vui64_t shft_mask = CONST_VINT128_DW(63, 63);

  /* constrain the dword shift amounts to 0-63.  */
  vsh_l = vec_and ((vui8_t)vrb, (vui8_t)shft_mask);
  /* Isolate the high dword so that bits from the low dword
   * do not contaminate the result.  */
  vr_h = vec_andc ((vui8_t)vra, (vui8_t)sel_mask);
  /* The low dword is just vra as the 128-bit shift left generates
   * '0's on the right and the final merge (vec_sel)
   * cleans up 64-bit overflow on the left.  */
  vr_l  = (vui8_t)vra;
  /* The vsr instruction only works correctly if the bit shift
   * value is splatted to each byte of the vector.  */
  vsh_h = vec_splat (vsh_l, VEC_BYTE_L_DWH);
  vsh_l = vec_splat (vsh_l, VEC_BYTE_L_DWL);
//  vsht_splat = vec_splat ((vui8_t) vrb, VEC_BYTE_L);
  /* Shift the high dword by vsh_h.  */
  vr_h = vec_vslo (vr_h,  vsh_h);
  vr_h = vec_vsl  (vr_h, vsh_h);
  /* Shift the low dword by vsh_l.  */
  vr_l = vec_vslo (vr_l,  vsh_l);
  vr_l = vec_vsl  (vr_l, vsh_l);
  /* Merge the dwords after shift.  */
  result = (vui64_t)vec_sel (vr_h, vr_l, (vui8_t)sel_mask);
#endif
  return ((vui64_t) result);
}
#endif

/** \brief Vector Shift Right Algebraic Doubleword.
 *
 *  Vector Shift Right Algebraic Doubleword 0-63 bits.
 *  The shift amount is from bits 58-63 and 122-127 of vrb.
 *
 *  \note Use the vec_vsrad for consistency with vec_vsld above.
 *  Define vec_vsrad only if the compiler does not define it in
 *  <altivec.h>.
 *
 *  @param vra a 128-bit vector treated as 2 x unsigned long integers.
 *  @param vrb shift amount in bits 58:63 and 122:127.
 *  @return Right shifted vector unsigned long.
 */
#ifndef vec_vsrad
static inline vi64_t
vec_vsrad (vi64_t vra, vui64_t vrb)
{
  vi64_t result;

#ifdef _ARCH_PWR8
  __asm__(
      "vsrad %0,%1,%2;"
      : "=v" (result)
      : "v" (vra),
      "v" (brb)
      : );
#else
  vui8_t  vsh_h, vsh_l;
  vui8_t  vr_h, vr_l;
  vi32_t exsa;
  vui32_t shw31 = CONST_VINT128_W (-1, -1, -1, -1);
  vui64_t exsah, exsal;
  vui64_t shft_mask = CONST_VINT128_DW(63, 63);

  /* Need to extend each signed long int to __int128. So the unsigned
   * (128-bit) shift right behaves as a arithmetic (64-bit) shift.  */
  exsa = vec_vsraw ((vi32_t)vra, shw31);
  exsah = (vui64_t)vec_vmrghw (exsa, exsa);
  exsal = (vui64_t)vec_vmrglw (exsa, exsa);
  /* constrain the dword shift amounts to 0-63.  */
  vsh_l = vec_and ((vui8_t)vrb, (vui8_t)shft_mask);
  /* The vsr instruction only works correctly if the bit shift
   * value is splatted to each byte of the vector.  */
  vsh_h = vec_splat (vsh_l, VEC_BYTE_L_DWH);
  vsh_l = vec_splat (vsh_l, VEC_BYTE_L_DWL);
  /* Merge the extended sign with high dword.  */
  exsah = vec_mrghd (exsah, (vui64_t)vra);
  /* Shift the high dword by vsh_h.  */
  vr_h = vec_vsro ((vui8_t)exsah,  vsh_h);
  vr_h = vec_vsr  (vr_h, vsh_h);
  /* Merge the extended sign with high dword.  */
  exsal = vec_pasted (exsal, (vui64_t)vra);
  /* Shift the low dword by vsh_l.  */
  vr_l = vec_vsro ((vui8_t)exsal, vsh_l);
  vr_l = vec_vsr  (vr_l, vsh_l);
  /* Merge the dwords after shift.  */
  result = (vi64_t)vec_mrgld ((vui64_t)vr_h, (vui64_t)vr_l);
#endif
  return ((vi64_t) result);
}
#endif

/** \brief Vector Shift Right Doubleword.
 *
 *  Vector Shift Right Doubleword 0-63 bits.
 *  The shift amount is from bits 58-63 and 122-127 of vrb.
 *
 *  \note Use the vec_vsrd for consistency with vec_vsld above.
 *  Define vec_vsrd only if the compiler does not define it in
 *  <altivec.h>.
 *
 *  @param vra a 128-bit vector treated as 2 x unsigned long integers.
 *  @param vrb shift amount in bits 58:63 and 122:127.
 *  @return Right shifted vector unsigned long.
 */
#ifndef vec_vsrd
static inline vui64_t
vec_vsrd (vui64_t vra, vui64_t vrb)
{
  vui64_t result;

#ifdef _ARCH_PWR8
  __asm__(
      "vsrd %0,%1,%2;"
      : "=v" (result)
      : "v" (vra),
      "v" (brb)
      : );
#else
  vui8_t  vsh_h, vsh_l;
  vui8_t  vr_h, vr_l;
  vui64_t sel_mask = CONST_VINT128_DW(0, -1LL);
  vui64_t shft_mask = CONST_VINT128_DW(63, 63);

  /* constrain the dword shift amounts to 0-63.  */
  vsh_l = vec_and ((vui8_t)vrb, (vui8_t)shft_mask);
  /* Isolate the low dword so that bits from the high dword,
   * do not contaminate the result.  */
  vr_l  = vec_and ((vui8_t)vra, (vui8_t)sel_mask);
  /* The vsr instruction only works correctly if the bit shift
   * value is splatted to each byte of the vector.  */
  vsh_h = vec_splat (vsh_l, VEC_BYTE_L_DWH);
  vsh_l = vec_splat (vsh_l, VEC_BYTE_L_DWL);
//  vsht_splat = vec_splat ((vui8_t) vrb, VEC_BYTE_L);
  /* Shift the high dword by vsh_h.  */
  vr_h = vec_vsro ((vui8_t)vra,  vsh_h);
  vr_h = vec_vsr  (vr_h, vsh_h);
  /* Shift the low dword by vsh_l.  */
  vr_l = vec_vsro (vr_l,  vsh_l);
  vr_l = vec_vsr  (vr_l, vsh_l);
  /* Merge the dwords after shift.  */
  result = (vui64_t)vec_sel (vr_h, vr_l, (vui8_t)sel_mask);
#endif
  return ((vui64_t) result);
}
#endif

#endif /* VEC_INT64_PPC_H_ */
