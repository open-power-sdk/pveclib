/*
 Copyright (c) [2017, 2018] IBM Corporation.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 vec_int128_ppc.h

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: May 10, 2015
      Steven Munroe, additional contributions for POWER9.
 */

#ifndef VEC_INT128_PPC_H_
#define VEC_INT128_PPC_H_

#include <vec_common_ppc.h>
#include <vec_int64_ppc.h>

/*!
 * \file  vec_int128_ppc.h
 * \brief Header package containing a collection of 128-bit computation
 * functions implemented with PowerISA VMX and VSX instructions.
 *
 * The original VMX (AKA Altivec) only defined a few instructions that
 * operated on the 128-bit vector as a whole.
 * This included the vector shift left/right (bit), vector shift
 * left/right by octet (byte), vector shift left double  by octet
 * (select a contiguous 16-bytes from 2 concatenated vectors)
 * 256-bit), and generalized vector permute (select any 16-bytes from
 * 2 concatenated vectors).
 * Use of these instructions can be complicated when;
 * - the shift amount is more than 8 bits,
 * - the shift amount is not a multiple of 8-bits (octet),
 * - the shift amount is a constant and needs to be generated/loaded
 * before use.
 *
 * These operations require pre-conditions to avoid multiple
 * instructions or require a combination of (bit and octet shift)
 * instructions to get the quadword result.  The compiler <altivec.h>
 * built-ins only supports individual instructions. So using these
 * operations quickly inspires a need for a header (like this) to
 * contain implementations of the common operations.
 *
 * The VSX facility (introduced with POWER7)  did not add any integer
 * doubleword (64-bit) or quadword (128-bit) operations.  However
 * it did add a useful doubleword permute immediate and word wise;
 * merge, shift, and splat immediate operations.
 * Otherwise vector __int128 (128-bit elements) operations
 * have to be implemented using VMX word and halfword element integer
 * operations for POWER7.
 *
 * POWER8
 * added multiply word operations that produce the full doubleword
 * product and full quadword add / subtract (with carry extend).
 * The add quadword is useful to sum the partial products for a full
 * 128 x 128-bit multiply.
 * The add quadword write carry and extend forms, simplify extending
 * arithmetic to 256-bits and beyond.
 *
 * While POWER8 provided quadword integer add and subtract operations,
 * it did not provide quadword Signed/Unsigned integer compare
 * operations. It is possible to implement quadword compare operations
 * using existing word / doubleword compares and the the new quadword
 * subtract, but this requires some study. Pveclib provides easy to
 * use quadword compare operations.
 *
 * POWER9 (PowerISA 3.0B) adds the <B>Vector Multiply-Sum unsigned
 * Doubleword Modulo</B> instruction. Aspects of this instruction mean
 * it needs to be used carefully as part of larger quadword multiply.
 * It performs only two of the four required doubleword multiplies.
 * The final quadword modulo sum will discard any overflow/carry from
 * the potential 130-bit result. With careful pre-conditioning of
 * doubleword inputs the results are can not overflow from 128-bits.
 * Then separate add quadword add/write carry operations can be used to
 * complete the sum of partial products.
 * These techniques are used in the POWER9 specific implementations of
 * vec_muleud, vec_muloud, vec_mulluq, and vec_muludq.
 *
 * PowerISA 3.0B also defined additional:
 * Binary Coded Decimal (BCD) and Zoned character format conversions.
 * String processing operations.
 * Vector Parity operations.
 * Integer Extend Sign Operations.
 * Integer Absolute Difference Operations.
 * All of these seem to useful additions to pveclib for older
 * (POWER7/8) processors and across element sizes (including
 * quadword elements).
 *
 * Most of these intrinsic (compiler built-in) operations are defined
 * in <altivec.h> and described in the compiler documentation.
 * However it took several compiler releases for all the new POWER8
 * 64-bit and 128-bit integer vector intrinsics to be added to
 * <B>altivec.h</B>. This support started with the GCC 4.9 but was not
 * complete across function/type and bug free until GCC 6.0.
 *
 * \note The compiler disables associated <altivec.h> built-ins if the
 * <B>mcpu</B> target does not enable the specific instruction.
 * For example if you compile with <B>-mcpu=power7</B>, vec_vadduqm and
 * vec_vsubudm will not be defined.  But vec_adduqm() and vec_subudm()
 * and always be defined in this header, will generate the minimum code,
 * appropriate for the target, and produce correct results.
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
 * - Operations implemented in hardware instructions for later
 * processors and useful to programmers, on slightly older processors,
 * even if the equivalent function requires more instructions.
 * Examples include quadword byte reverse, add and subtract.
 * - Defined in the OpenPOWER ABI but <I>not</I> yet defined in
 * <altivec.n> provided by available compilers in common use.
 * Examples include quadword byte reverse, add and subtract.
 * - Are commonly used operations, not covered by the ABI or
 * <altivec.h>, and require multiple instructions or
 * are not obvious.  Examples include quadword; Signed and Unsigned
 * compare, shift immediate, multiply, multiply by 10 immediate,
 * count leading zeros and population count.
 *
 * \note The Multiply sum/even/odd doubleword operations are
 * currently implemented here (in <vec_int128_ppc.h>) which resolves a
 * dependency on Add Quadword. These functions (vec_msumudm,
 * vec_muleud, vec_muloud) all produce a quadword results and may use
 * the vec_adduqm implementation to sum partial products.
 *
 * See \ref mainpage_sub_1_3 for more background on extended quadword
 * computation.
 *
 * \section int128_perf_0_0 Performance data.
 * High level performance estimates are provided as an aid to function
 * selection when evaluating algorithms. For background on how
 * <I>Latency</I> and <I>Throughput</I> are derived see:
 * \ref perf_data
 */

/** \brief Vector Add & write Carry Unsigned Quadword.
 *
 *  Add two vector __int128 values and return the carry out.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     |2/2 cycles|
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated a __int128.
 *  @param b 128-bit vector treated a __int128.
 *  @return __int128 carry of the sum of a and b.
 */
static inline vui128_t
vec_addcuq (vui128_t a, vui128_t b)
{
  vui32_t co;
#ifdef _ARCH_PWR8
#ifndef vec_vaddcuq
  __asm__(
      "vaddcuq %0,%1,%2;"
      : "=v" (co)
      : "v" (a),
      "v" (b)
      : );
#else
  co = (vui32_t) vec_vaddcuq (a, b);
#endif
#else
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
#endif
  return ((vui128_t) co);
}

 /** \brief Vector Add Extended & write Carry Unsigned Quadword.
  *
  *  Add two vector __int128 values plus a carry-in (0|1) and return
  *  the carry out bit.
  *
  *  |processor|Latency|Throughput|
  *  |--------:|:-----:|:---------|
  *  |power8   | 4     |2/2 cycles|
  *  |power9   | 3     | 2/cycle  |
  *
  *  @param a 128-bit vector treated a __int128.
  *  @param b 128-bit vector treated a __int128.
  *  @param ci Carry-in from vector bit[127].
  *  @return carry-out in bit[127] of the sum of a + b + c.
  */
 static inline vui128_t
 vec_addecuq (vui128_t a, vui128_t b, vui128_t ci)
 {
   vui32_t co;
 #ifdef _ARCH_PWR8
 #ifndef vec_vaddecuq
   __asm__(
       "vaddecuq %0,%1,%2,%3;"
       : "=v" (co)
       : "v" (a),
       "v" (b),
       "v" (ci)
       : );
 #else
   co = (vui32_t) vec_vaddecuq (a, b, ci);
 #endif
 #else
   vui32_t c, c2, t;
   vui32_t z = { 0, 0, 0, 0 };
   co = (vui32_t){ 1, 1, 1, 1 };

   c2 = vec_and ((vui32_t) ci, co);
   c2 = vec_sld ((vui32_t) c2, z, 12);
   co = vec_vaddcuw ((vui32_t) a, (vui32_t) b);
   t = vec_vadduwm ((vui32_t) a, (vui32_t) b);
   c = vec_sld (co, c2, 4);
   c2 = vec_vaddcuw (t, c);
   t = vec_vadduwm (t, c);
   co = vec_vor (co, c2);
   c = vec_sld (c2, z, 4);
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
 #endif
   return ((vui128_t) co);
 }

/** \brief Vector Add Extended Unsigned Quadword Modulo.
 *
 *  Add two vector __int128 values plus a carry (0|1) and return
 *  the modulo 128-bit result.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     |2/2 cycles|
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated a __int128.
 *  @param b 128-bit vector treated a __int128.
 *  @param ci Carry-in from vector bit[127].
 *  @return __int128 sum of a + b + c, modulo 128-bits.
 */
static inline vui128_t
vec_addeuqm (vui128_t a, vui128_t b, vui128_t ci)
{
  vui32_t t;
#ifdef _ARCH_PWR8
#ifndef vec_vaddeuqm
  __asm__(
      "vaddeuqm %0,%1,%2,%3;"
      : "=v" (t)
      : "v" (a),
      "v" (b),
      "v" (ci)
      : );
#else
  t = (vui32_t) vec_vaddeuqm (a, b, ci);
#endif
#else
  vui32_t c2, c;
  vui32_t z  = { 0,0,0,0};
  vui32_t co = { 1,1,1,1};

  c2 = vec_and ((vui32_t)ci, co);
  c2 = vec_sld ((vui32_t)ci, z, 12);
  co = vec_vaddcuw ((vui32_t)a, (vui32_t)b);
  t = vec_vadduwm ((vui32_t)a, (vui32_t)b);
  c = vec_sld (co, c2, 4);
  c2 = vec_vaddcuw (t, c);
  t = vec_vadduwm (t, c);
  c = vec_sld (c2, z, 4);
  c2 = vec_vaddcuw (t, c);
  t = vec_vadduwm (t, c);
  c = vec_sld (c2, z, 4);
  c2 = vec_vaddcuw (t, c);
  t = vec_vadduwm (t, c);
  c = vec_sld (c2, z, 4);
  t = vec_vadduwm (t, c);
#endif
  return ((vui128_t) t);
}

/** \brief Vector Add Unsigned Quadword Modulo.
 *
 *  Add two vector __int128 values and return result modulo 128-bits.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     |2/2 cycles|
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as a __int128.
 *  @param b 128-bit vector treated as a __int128.
 *  @return __int128 sum of a and b.
 */
static inline vui128_t
vec_adduqm (vui128_t a, vui128_t b)
{
  vui32_t t;
#ifdef _ARCH_PWR8
#ifndef vec_vadduqm
  __asm__(
      "vadduqm %0,%1,%2;"
      : "=v" (t)
      : "v" (a),
      "v" (b)
      : );
#else
  t = (vui32_t) vec_vadduqm (a, b);
#endif
#else
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
#endif
  return ((vui128_t) t);
}

/** \brief Vector Add with carry Unsigned Quadword.
 *
 *  Add two vector __int128 values and return sum and the carry out.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8     |1/2 cycles|
 *  |power9   | 6     | 2/cycle  |
 *
 *  @param *cout carry out from the sum of a and b.
 *  @param a 128-bit vector treated a __int128.
 *  @param b 128-bit vector treated a __int128.
 *  @return __int128 (lower 128-bits) sum of a and b.
 */
static inline vui128_t
vec_addcq (vui128_t *cout, vui128_t a, vui128_t b)
{
  vui32_t t, co;
#ifdef _ARCH_PWR8
#ifndef vec_vadduqm
  __asm__(
      "vadduqm %0,%2,%3;\n"
      "\tvaddcuq %1,%2,%3;"
      : "=v" (t),
      "=v" (co)
      : "v" (a),
      "v" (b)
      : );
#else
  t = (vui32_t) vec_vadduqm (a, b);
  co = (vui32_t) vec_vaddcuq (a, b);
#endif
#else
  vui32_t c, c2;
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
  t = vec_vadduwm (t, c);
  co = vec_vor (co, c2);
  co = vec_sld (z, co, 4);
#endif
  *cout = (vui128_t) co;
  return ((vui128_t) t);
}

/** \brief Vector Add Extend with carry Unsigned Quadword.
 *
 *  Add two vector __int128 values plus a carry-in (0|1)
 *  and return sum and the carry out.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8     |1/2 cycles|
 *  |power9   | 6     | 2/cycle  |
 *
 *  @param *cout carry out from the sum of a and b.
 *  @param a 128-bit vector treated a __int128.
 *  @param b 128-bit vector treated a __int128.
 *  @param ci Carry-in from vector bit[127].
 *  @return __int128 (lower 128-bits) sum of a + b + c.
 */
static inline vui128_t
vec_addeq (vui128_t *cout, vui128_t a, vui128_t b, vui128_t ci)
{
  vui32_t t, co;
#ifdef _ARCH_PWR8
#ifndef vec_vaddeuqm
  __asm__(
      "vaddeuqm %0,%2,%3,%4;\n"
      "\tvaddecuq %1,%2,%3,%4;"
      : "=v" (t),
      "=v" (co)
      : "v" (a),
      "v" (b),
      "v" (c)
      : );
#else
  t = (vui32_t) vec_vaddeuqm (a, b, ci);
  co = (vui32_t) vec_vaddecuq (a, b, ci);
#endif
#else
  vui32_t c, c2;
  vui32_t z= { 0,0,0,0};
  co = (vui32_t){ 1,1,1,1};

  c2 = vec_and ((vui32_t)ci, co);
  c2 = vec_sld ((vui32_t)c2, z, 12);
  co = vec_vaddcuw ((vui32_t)a, (vui32_t)b);
  t = vec_vadduwm ((vui32_t)a, (vui32_t)b);
  c = vec_sld (co, c2, 4);
  c2 = vec_vaddcuw (t, c);
  t = vec_vadduwm (t, c);
  co = vec_vor (co, c2);
  c = vec_sld (c2, z, 4);
  c2 = vec_vaddcuw (t, c);
  t = vec_vadduwm (t, c);
  co = vec_vor (co, c2);
  c = vec_sld (c2, z, 4);
  c2 = vec_vaddcuw (t, c);
  t = vec_vadduwm (t, c);
  co = vec_vor (co, c2);
  c = vec_sld (c2, z, 4);
  c2 = vec_vaddcuw (t, c);
  t = vec_vadduwm (t, c);
  co = vec_vor (co, c2);
  co = vec_sld (z, co, 4);
#endif
  *cout = (vui128_t) co;
  return ((vui128_t) t);
}

/** \brief Count leading zeros for a vector __int128.
 *
 *  Count leading zeros for a vector __int128 and return the count in a
 *  vector suitable for use with vector shift (left|right) and vector
 *  shift (left|right) by octet instructions.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 19-28 | 1/cycle  |
 *  |power9   | 25-36 | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated a __int128.
 *  @return a 128-bit vector with bits 121:127 containing the count of
 *  leading zeros.
 */
static inline vui128_t
vec_clzq (vui128_t vra)
{
  vui64_t result;

#ifdef _ARCH_PWR8
  /*
   * Use the Vector Count Leading Zeros Double Word instruction to get
   * the count for the left and right vector halves.  If the left vector
   * doubleword of the input is nonzero then only the left count is
   * included and we need to mask off the right count.
   * Otherwise the left count is 64 and we need to add 64 to the right
   * count.
   * After masking we sum across the left and right counts to
   * get the final 128-bit vector count (0-128).
   */
  vui64_t vt1, vt2, vt3;
  vui64_t vzero = { 0, 0 };
  vui64_t v64 = { 64, 64 };

  vt1 = vec_vclz ((vui64_t) vra);
  vt2 = (vui64_t) vec_cmplt(vt1, v64);
  vt3 = (vui64_t) vec_sld ((vui8_t) vzero, (vui8_t) vt2, 8);
  result = vec_andc (vt1, vt3);
  result = (vui64_t) vec_sums ((vi32_t) result, (vi32_t) vzero);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  result = (vui64_t) vec_sld ((vui8_t) result, (vui8_t) result, 4);
#endif
#else
  /* vector clz instructions were introduced in power8. For power7 and
   * earlier, use the pveclib vec_clzw implementation.  For a quadword
   * clz, this requires pre-conditioning the input before computing the
   * the word clz and sum across.   */
  vui32_t c0, clz;
  vui32_t r32, gt32, gt32sr32, gt64sr64;

  c0 = vec_splat_u32 (0);
  gt32 = (vui32_t)vec_cmpgt((vui32_t)vra, c0);
  gt32sr32 = vec_sld (c0, gt32, 12);
  gt64sr64 = vec_sld (c0, gt32, 8);
  gt32 = vec_sld (c0, gt32, 4);

  gt32sr32 = vec_or (gt32sr32, gt32);
  gt64sr64 = vec_or (gt64sr64, (vui32_t)vra);
  r32 = vec_or (gt32sr32, gt64sr64);

  clz = vec_clzw (r32);
  result = (vui64_t)vec_sums ((vi32_t)clz, (vi32_t)c0);
#endif

  return ((vui128_t) result);
}
///@cond INTERNAL
static inline vui128_t vec_cmpequq (vui128_t vra, vui128_t vrb);
static inline vui128_t vec_cmpgeuq (vui128_t vra, vui128_t vrb);
static inline vui128_t vec_cmpgtuq (vui128_t vra, vui128_t vrb);
static inline vui128_t vec_cmpleuq (vui128_t vra, vui128_t vrb);
static inline vui128_t vec_cmpltuq (vui128_t vra, vui128_t vrb);
static inline vui128_t vec_cmpneuq (vui128_t vra, vui128_t vrb);
static inline vui128_t vec_muleud (vui64_t a, vui64_t b);
static inline vui128_t vec_muloud (vui64_t a, vui64_t b);
static inline vb128_t vec_setb_cyq (vui128_t vcy);
static inline vb128_t vec_setb_ncq (vui128_t vcy);
static inline vb128_t vec_setb_sq (vi128_t vra);
static inline vui128_t vec_subcuq (vui128_t vra, vui128_t vrb);
static inline vui128_t vec_subuqm (vui128_t vra, vui128_t vrb);
///@endcond

/** \brief Vector Compare Equal Signed Quadword.
 *
 *  Compare signed __int128 (128-bit) integers and return all '1's,
 *  if vra == vrb, otherwise all '0's.  We use
 *  vec_cmpequq as it works for both signed and unsigned compares.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6     | 2/cycle  |
 *  |power9   | 7     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an signed __int128.
 *  @param vrb 128-bit vector treated as an signed __int128.
 *  @return 128-bit vector boolean reflecting vector signed __int128
 *  compare equal.
 */
static inline vi128_t
vec_cmpeqsq (vi128_t vra, vi128_t vrb)
{
  /* vec_cmpequq works for both signed and unsigned compares.  */
  return (vi128_t)vec_cmpequq ((vui128_t) vra, (vui128_t) vrb);
}

/** \brief Vector Compare Equal Unsigned Quadword.
 *
 *  Compare unsigned __int128 (128-bit) integers and return all '1's,
 *  if vra == vrb, otherwise all '0's.
 *
 *  For POWER8 (PowerISA 2.07B) or later, use the Vector Compare
 *  Equal Unsigned DoubleWord (<B>vcmpequd</B>) instruction.
 *  To get the correct quadword result, the doubleword element equal
 *  truth values are swapped, then <I>anded</I> with the
 *  original compare results.
 *  Otherwise use vector word compare and additional boolean logic to
 *  insure all word elements are equal.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6     | 2/cycle  |
 *  |power9   | 7     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an unsigned __int128s.
 *  @param vrb 128-bit vector treated as an unsigned __int128.
 *  @return 128-bit vector boolean reflecting vector unsigned __int128
 *  compare equal.
 */
static inline vui128_t
vec_cmpequq (vui128_t vra, vui128_t vrb)
{
#ifdef _ARCH_PWR8
  vui64_t equd, swapd;

  equd = (vui64_t) vec_cmpequd ((vui64_t) vra, (vui64_t) vrb);
  swapd = vec_swapd (equd);
  return (vui128_t) vec_and (equd, swapd);
#else
  vui32_t equw, equ1, equ2, equ3;

  equw = (vui32_t) vec_cmpeq ((vui32_t) vra,
      (vui32_t) vrb);
  equ1 = vec_sld (equw, equw, 4);
  equ2 = vec_sld (equw, equw, 8);
  equ3 = vec_sld (equw, equw, 12);
  equw = vec_and (equw, equ1);
  equ2 = vec_and (equ2, equ3);
  return (vui128_t) vec_and (equw, equ2);
#endif
}

/** \brief Vector Compare Greater Than or Equal Signed Quadword.
 *
 *  Compare signed __int128 (128-bit) integers and return all '1's,
 *  if vra >= vrb, otherwise all '0's.
 *
 *  Flip the operand sign bits and use vec_cmpgeuq for signed compare.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-16 |1/ 2cycles|
 *  |power9   | 8-14  | 1/cycle  |
 *
 *  @param vra 128-bit vector treated as an signed __int128.
 *  @param vrb 128-bit vector treated as an signed __int128.
 *  @return 128-bit vector boolean reflecting vector signed __int128
 *  compare greater than.
 */
static inline vi128_t
vec_cmpgesq (vi128_t vra, vi128_t vrb)
{
  const vui32_t signbit = CONST_VINT128_W (0x80000000, 0, 0, 0);
  vui32_t _a, _b;

  _a = vec_xor ((vui32_t)vra, signbit);
  _b = vec_xor ((vui32_t)vrb, signbit);
  return (vi128_t)vec_cmpgeuq ((vui128_t)_a, (vui128_t)_b);
}

/** \brief Vector Compare Greater Than or Equal Unsigned Quadword.
 *
 *  Compare unsigned __int128 (128-bit) integers and return all '1's,
 *  if vra >= vrb, otherwise all '0's.
 *
 *  For POWER8 (PowerISA 2.07B) or later, use the Vector Subtract &
 *  write Carry QuadWord (<B>vsubcuq</B>) instruction.
 *  This generates a carry for greater than or equal
 *  and NOT carry for less than.
 *  Then use vec_setb_cyq ro convert the carry into a vector bool.
 *  Here we use the pveclib implementations (vec_subcuq() and
 *  vec_setb_cyq()), instead of <altivec.h> intrinsics,
 *  to address older compilers and POWER7.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8     |2/ 2cycles|
 *  |power9   | 6     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an unsigned __int128.
 *  @param vrb 128-bit vector treated as an unsigned __int128.
 *  @return 128-bit vector boolean reflecting vector unsigned __int128
 *  compare greater than.
 */
static inline vui128_t
vec_cmpgeuq (vui128_t vra, vui128_t vrb)
{
  vui128_t a_b;

  a_b = vec_subcuq (vra, vrb);
  return vec_setb_cyq (a_b);
}

/** \brief Vector Compare Greater Than Signed Quadword.
 *
 *  Compare signed __int128 (128-bit) integers and return all '1's,
 *  if vra > vrb, otherwise all '0's.
 *
 *  Flip the operand sign bits and use vec_cmpgtuq for signed compare.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-16 |1/ 2cycles|
 *  |power9   | 8-14  | 1/cycle  |
 *
 *  @param vra 128-bit vector treated as an signed __int128.
 *  @param vrb 128-bit vector treated as an signed __int128.
 *  @return 128-bit vector boolean reflecting vector signed __int128
 *  compare greater than.
 */
static inline vi128_t
vec_cmpgtsq (vi128_t vra, vi128_t vrb)
{
  const vui32_t signbit = CONST_VINT128_W (0x80000000, 0, 0, 0);
  vui32_t _a, _b;

  _a = vec_xor ((vui32_t)vra, signbit);
  _b = vec_xor ((vui32_t)vrb, signbit);
  return (vi128_t)vec_cmpgtuq ((vui128_t)_a, (vui128_t)_b);
}

/** \brief Vector Compare Greater Than Unsigned Quadword.
 *
 *  Compare unsigned __int128 (128-bit) integers and return all '1's,
 *  if vra > vrb, otherwise all '0's.
 *
 *  For POWER8 (PowerISA 2.07B) or later, use the Vector Subtract &
 *  write Carry QuadWord (<B>vsubcuq</B>) instruction with the
 *  parameters reversed.  This generates a carry for less than or equal
 *  and NOT carry for greater than.
 *  Then use vec_setb_ncq ro convert the carry into a vector bool.
 *  Here we use the pveclib implementations (vec_subcuq() and
 *  vec_setb_ncq()), instead of <altivec.h> intrinsics,
 *  to address older compilers and POWER7.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8     |2/ 2cycles|
 *  |power9   | 6     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an unsigned __int128.
 *  @param vrb 128-bit vector treated as an unsigned __int128.
 *  @return 128-bit vector boolean reflecting vector unsigned __int128
 *  compare greater than.
 */
static inline vui128_t
vec_cmpgtuq (vui128_t vra, vui128_t vrb)
{
  vui128_t b_a;

  b_a = vec_subcuq (vrb, vra);
  return vec_setb_ncq (b_a);
}

/** \brief Vector Compare Less Than or Equal Signed Quadword.
 *
 *  Compare signed __int128 (128-bit) integers and return all '1's,
 *  if vra <= vrb, otherwise all '0's.
 *
 *  Flip the operand sign bits and use vec_cmpleuq for signed compare.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-16 |1/ 2cycles|
 *  |power9   | 8-14  | 1/cycle  |
 *
 *  @param vra 128-bit vector treated as an signed __int128.
 *  @param vrb 128-bit vector treated as an signed __int128.
 *  @return 128-bit vector boolean reflecting vector signed __int128
 *  compare less than or equal.
 */
static inline vi128_t
vec_cmplesq (vi128_t vra, vi128_t vrb)
{
  const vui32_t signbit = CONST_VINT128_W (0x80000000, 0, 0, 0);
  vui32_t _a, _b;

  _a = vec_xor ((vui32_t)vra, signbit);
  _b = vec_xor ((vui32_t)vrb, signbit);
  return (vi128_t)vec_cmpleuq ((vui128_t)_a, (vui128_t)_b);
}

/** \brief Vector Compare Less Than or Equal Unsigned Quadword.
 *
 *  Compare unsigned __int128 (128-bit) integers and return all '1's,
 *  if vra <= vrb, otherwise all '0's.
 *
 *  For POWER8 (PowerISA 2.07B) or later, use the Vector Subtract &
 *  write Carry QuadWord (<B>vsubcuq</B>) instruction.
 *  This generates a carry for greater than or equal
 *  and NOT carry for less than.
 *  Then use vec_setb_ncq ro convert the carry into a vector bool.
 *  Here we use the pveclib implementations (vec_subcuq() and
 *  vec_setb_cyq()), instead of <altivec.h> intrinsics,
 *  to address older compilers and POWER7.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8     |2/ 2cycles|
 *  |power9   | 6     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an unsigned __int128.
 *  @param vrb 128-bit vector treated as an unsigned __int128.
 *  @return 128-bit vector boolean reflecting vector unsigned __int128
 *  compare less than or equal.
 */
static inline vui128_t
vec_cmpleuq (vui128_t vra, vui128_t vrb)
{
  vui128_t b_a;

  b_a = vec_subcuq (vrb, vra);
  return vec_setb_cyq (b_a);
}


/** \brief Vector Compare Less Than Signed Quadword.
 *
 *  Compare signed __int128 (128-bit) integers and return all '1's,
 *  if vra < vrb, otherwise all '0's.
 *
 *  Flip the operand sign bits and use vec_cmpltuq for signed compare.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-16 |1/ 2cycles|
 *  |power9   | 8-14  | 1/cycle  |
 *
 *  @param vra 128-bit vector treated as an signed __int128.
 *  @param vrb 128-bit vector treated as an signed __int128.
 *  @return 128-bit vector boolean reflecting vector unsigned __int128
 *  compare less than.
 */
static inline vi128_t
vec_cmpltsq (vi128_t vra, vi128_t vrb)
{
  const vui32_t signbit = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vui32_t _a, _b;

  _a = vec_xor ((vui32_t) vra, signbit);
  _b = vec_xor ((vui32_t) vrb, signbit);
  return (vi128_t) vec_cmpltuq ((vui128_t) _a, (vui128_t) _b);
}

/** \brief Vector Compare Less Than Unsigned Quadword.
 *
 *  Compare unsigned __int128 (128-bit) integers and return all '1's,
 *  if vra < vrb, otherwise all '0's.
 *
 *  For POWER8 (PowerISA 2.07B) or later, use the Vector Subtract &
 *  write Carry QuadWord (<B>vsubcuq</B>) instruction.
 *  This generates a carry for greater than or equal
 *  and NOT carry for less than.
 *  Then use vec_setb_ncq ro convert the carry into a vector bool.
 *  Here we use the pveclib implementations (vec_subcuq() and
 *  vec_setb_ncq()), instead of <altivec.h> intrinsics,
 *  to address older compilers and POWER7.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8     |2/ 2cycles|
 *  |power9   | 6     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an unsigned __int128.
 *  @param vrb 128-bit vector treated as an unsigned __int128.
 *  @return 128-bit vector boolean reflecting vector unsigned __int128
 *  compare less than.
 */
static inline vui128_t
vec_cmpltuq (vui128_t vra, vui128_t vrb)
{
  vui128_t  a_b;

  a_b = vec_subcuq (vra, vrb);
  return vec_setb_ncq (a_b);
}

/** \brief Vector Compare Equal Signed Quadword.
 *
 *  Compare signed __int128 (128-bit) integers and return all '1's,
 *  if vra != vrb, otherwise all '0's.  We use
 *  vec_cmpequq as it works for both signed and unsigned compares.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6     | 2/cycle  |
 *  |power9   | 7     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an signed __int128.
 *  @param vrb 128-bit vector treated as an signed __int128.
 *  @return 128-bit vector boolean reflecting vector signed __int128
 *  compare not equal.
 */
static inline vi128_t
vec_cmpnesq (vi128_t vra, vi128_t vrb)
{
  /* vec_cmpneuq works for both signed and unsigned compares.  */
  return (vi128_t)vec_cmpneuq ((vui128_t) vra, (vui128_t) vrb);
}

/** \brief Vector Compare Not Equal Unsigned Quadword.
 *
 *  Compare unsigned __int128 (128-bit) integers and return all '1's,
 *  if vra != vrb, otherwise all '0's.
 *
 *  For POWER8 (PowerISA 2.07B) or later, use the Vector Compare
 *  Equal Unsigned DoubleWord (<B>vcmpequd</B>) instruction.
 *  To get the correct quadword result, the doubleword element equal
 *  truth values are swapped, then <I>not anded</I> with the
 *  original compare results.
 *  Otherwise use vector word compare and additional boolean logic to
 *  insure all word elements are equal.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6     | 2/cycle  |
 *  |power9   | 7     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @param vrb 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @return 128-bit vector boolean reflecting vector unsigned __int128
 *  compare equal.
 */
static inline vui128_t
vec_cmpneuq (vui128_t vra, vui128_t vrb)
{
#ifdef _ARCH_PWR8
  __vector unsigned long equd, swapd;

  equd = (vui64_t) vec_cmpequd ((vui64_t) vra,
      (vui64_t) vrb);
  swapd = vec_swapd (equd);
  return (vui128_t) vec_nand (equd, swapd);
#else
  vui32_t equw, equ1, equ2, equ3;

  equw = (vui32_t) vec_cmpeq ((vui32_t) vra, (vui32_t) vrb);
  equ1 = vec_sld (equw, equw, 4);
  equ2 = vec_sld (equw, equw, 8);
  equ3 = vec_sld (equw, equw, 12);
  equw = vec_and (equw, equ1);
  equ2 = vec_and (equ2, equ3);
  /* POWER7 does not have vnand nor xxlnand, so requires an extra vnor
     after the final vand.  */
  equw = vec_and (equw, equ2);
  return (vui128_t) vec_nor (equw, equw);
#endif
}

/** \brief Vector Compare all Equal Signed Quadword.
 *
 *  Compare vector signed __int128 values and return true if
 *  vra and vrb are equal.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector signed
 *  __int128 (qword) element.
 *  @param vrb 128-bit vector treated as an vector signed
 *  __int128 (qword) element.
 *  @return boolean int for all 128-bits, true if equal,
 *  false otherwise.
 */
static inline
int
vec_cmpsq_all_eq (vi128_t vra, vi128_t vrb)
{
  int result;
#if defined (_ARCH_PWR8) && (__GNUC__ >= 6)
  result = vec_all_eq((vui64_t)vra, (vui64_t)vrb);
#else
  result = vec_all_eq((vui32_t)vra, (vui32_t)vrb);
#endif
  return (result);
}

/** \brief Vector Compare any Greater Than or Equal Signed Quadword.
 *
 *  Compare vector unsigned __int128 values and return true if
 *  vra >= vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-15 |1/ 2cycles|
 *  |power9   | 8     | 1/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector signed
 *  __int128 (qword) element.
 *  @param vrb 128-bit vector treated as an vector signed
 *  __int128 (qword) element.
 *  @return boolean int for all 128-bits, true if Greater Than or Equal,
 *  false otherwise.
 */
static inline int
vec_cmpsq_all_ge (vi128_t vra, vi128_t vrb)
{
  const vui32_t carry128 = CONST_VINT128_W (0, 0, 0, 1);
  const vui32_t signbit = CONST_VINT128_W (0x80000000, 0, 0, 0);
  vui128_t a_b, _a, _b;

  _a = (vui128_t)vec_xor ((vui32_t)vra, signbit);
  _b = (vui128_t)vec_xor ((vui32_t)vrb, signbit);

  a_b = vec_subcuq (_a, _b);
  return vec_all_eq((vui32_t)a_b, carry128);
}

/** \brief Vector Compare any Greater Than Signed Quadword.
 *
 *  Compare vector signed __int128 values and return true if
 *  vra > vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-15 |1/ 2cycles|
 *  |power9   | 8     | 1/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector signed
 *  __int128 (qword) element.
 *  @param vrb 128-bit vector treated as an vector signed
 *  __int128 (qword) element.
 *  @return boolean int for all 128-bits, true if Greater Than,
 *  false otherwise.
 */
static inline int
vec_cmpsq_all_gt (vi128_t vra, vi128_t vrb)
{
  const vui32_t ncarry128 = CONST_VINT128_W (0, 0, 0, 0);
  const vui32_t signbit = CONST_VINT128_W (0x80000000, 0, 0, 0);
  vui128_t b_a, _a, _b;

  _a = (vui128_t)vec_xor ((vui32_t)vra, signbit);
  _b = (vui128_t)vec_xor ((vui32_t)vrb, signbit);

  b_a = vec_subcuq (_b, _a);
  return vec_all_eq((vui32_t)b_a, ncarry128);
}

/** \brief Vector Compare any Less Than or Equal Signed Quadword.
 *
 *  Compare vector signed __int128 values and return true if
 *  vra <= vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-15 |1/ 2cycles|
 *  |power9   | 8     | 1/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector signed
 *  __int128 (qword) element.
 *  @param vrb 128-bit vector treated as an vector signed
 *  __int128 (qword) element.
 *  @return boolean int for all 128-bits, true if Less Than or Equal,
 *  false otherwise.
 */
static inline int
vec_cmpsq_all_le (vi128_t vra, vi128_t vrb)
{
  const vui32_t carry128 = CONST_VINT128_W (0, 0, 0, 1);
  const vui32_t signbit = CONST_VINT128_W (0x80000000, 0, 0, 0);
  vui128_t b_a, _a, _b;

  _a = (vui128_t)vec_xor ((vui32_t)vra, signbit);
  _b = (vui128_t)vec_xor ((vui32_t)vrb, signbit);

  b_a = vec_subcuq (_b, _a);
  return vec_all_eq((vui32_t)b_a, carry128);
}

/** \brief Vector Compare any Less Than Signed Quadword.
 *
 *  Compare vector signed __int128 values and return true if
 *  vra < vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-15 |1/ 2cycles|
 *  |power9   | 8     | 1/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector signed
 *  __int128 (qword) element.
 *  @param vrb 128-bit vector treated as an vector signed
 *  __int128 (qword) element.
 *  @return boolean int for all 128-bits, true if Less Than,
 *  false otherwise.
 */
static inline int
vec_cmpsq_all_lt (vi128_t vra, vi128_t vrb)
{
  const vui32_t ncarry128 = CONST_VINT128_W (0, 0, 0, 0);
  const vui32_t signbit = CONST_VINT128_W (0x80000000, 0, 0, 0);
  vui128_t a_b, _a, _b;

  _a = (vui128_t)vec_xor ((vui32_t)vra, signbit);
  _b = (vui128_t)vec_xor ((vui32_t)vrb, signbit);

  a_b = vec_subcuq (_a, _b);
  return vec_all_eq((vui32_t)a_b, ncarry128);
}

/** \brief Vector Compare all Not Equal Signed Quadword.
 *
 *  Compare vector signed __int128 values and return true if
 *  vra and vrb are not equal.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector signed
 *  __int128 (qword) element.
 *  @param vrb 128-bit vector treated as an vector signed
 *  __int128 (qword) element.
 *  @return boolean __int128 for all 128-bits, true if equal,
 *  false otherwise.
 */
static inline
int
vec_cmpsq_all_ne (vi128_t vra, vi128_t vrb)
{
  int result;
#if defined (_ARCH_PWR8) && (__GNUC__ >= 6)
  result = !vec_all_eq((vui64_t)vra, (vui64_t)vrb);
#else
  result = !vec_all_eq((vui32_t)vra, (vui32_t)vrb);
#endif
  return (result);
}

/** \brief Vector Compare all Equal Unsigned Quadword.
 *
 *  Compare vector unsigned __int128 values and return true if
 *  vra and vrb are equal.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector unsigned
 *  __int128 (qword) element.
 *  @param vrb 128-bit vector treated as an vector unsigned
 *  __int128 (qword) element.
 *  @return boolean int for all 128-bits, true if equal,
 *  false otherwise.
 */
static inline
int
vec_cmpuq_all_eq (vui128_t vra, vui128_t vrb)
{
  int result;
#if defined (_ARCH_PWR8) && (__GNUC__ >= 6)
  result = vec_all_eq((vui64_t)vra, (vui64_t)vrb);
#else
  result = vec_all_eq((vui32_t)vra, (vui32_t)vrb);
#endif
  return (result);
}

/** \brief Vector Compare any Greater Than or Equal Unsigned Quadword.
 *
 *  Compare vector unsigned __int128 values and return true if
 *  vra >= vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8-13  |2/ 2cycles|
 *  |power9   | 6     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector unsigned
 *  __int128 (qword) element.
 *  @param vrb 128-bit vector treated as an vector unsigned
 *  __int128 (qword) element.
 *  @return boolean int for all 128-bits, true if Greater Than or Equal,
 *  false otherwise.
 */
static inline int
vec_cmpuq_all_ge (vui128_t vra, vui128_t vrb)
{
  const vui32_t carry128 = CONST_VINT128_W (0, 0, 0, 1);
  vui128_t a_b;

  a_b = vec_subcuq (vra, vrb);
  return vec_all_eq((vui32_t)a_b, carry128);
}

/** \brief Vector Compare any Greater Than Unsigned Quadword.
 *
 *  Compare vector unsigned __int128 values and return true if
 *  vra > vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8-13  |2/ 2cycles|
 *  |power9   | 6     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector unsigned
 *  __int128 (qword) element.
 *  @param vrb 128-bit vector treated as an vector unsigned
 *  __int128 (qword) element.
 *  @return boolean int for all 128-bits, true if Greater Than,
 *  false otherwise.
 */
static inline int
vec_cmpuq_all_gt (vui128_t vra, vui128_t vrb)
{
  const vui32_t ncarry128 = CONST_VINT128_W (0, 0, 0, 0);
  vui128_t b_a;

  b_a = vec_subcuq (vrb, vra);
  return vec_all_eq((vui32_t)b_a, ncarry128);
}

/** \brief Vector Compare any Less Than or Equal Unsigned Quadword.
 *
 *  Compare vector unsigned __int128 values and return true if
 *  vra <= vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8-13  |2/ 2cycles|
 *  |power9   | 6     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector unsigned
 *  __int128 (qword) element.
 *  @param vrb 128-bit vector treated as an vector unsigned
 *  __int128 (qword) element.
 *  @return boolean int for all 128-bits, true if Less Than or Equal,
 *  false otherwise.
 */
static inline int
vec_cmpuq_all_le (vui128_t vra, vui128_t vrb)
{
  const vui32_t carry128 = CONST_VINT128_W (0, 0, 0, 1);
  vui128_t b_a;

  b_a = vec_subcuq (vrb, vra);
  return vec_all_eq((vui32_t)b_a, carry128);
}

/** \brief Vector Compare any Less Than Unsigned Quadword.
 *
 *  Compare vector unsigned __int128 values and return true if
 *  vra < vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8-13  |2/ 2cycles|
 *  |power9   | 6     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector unsigned
 *  __int128 (qword) element.
 *  @param vrb 128-bit vector treated as an vector unsigned
 *  __int128 (qword) element.
 *  @return boolean int for all 128-bits, true if Less Than,
 *  false otherwise.
 */
static inline int
vec_cmpuq_all_lt (vui128_t vra, vui128_t vrb)
{
  const vui32_t ncarry128 = CONST_VINT128_W (0, 0, 0, 0);
  vui128_t  a_b;

  a_b = vec_subcuq (vra, vrb);
  return vec_all_eq((vui32_t)a_b, ncarry128);
}

/** \brief Vector Compare all Not Equal Unsigned Quadword.
 *
 *  Compare vector unsigned __int128 values and return true if
 *  vra and vrb are not equal.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector unsigned
 *  __int128 (qword) element.
 *  @param vrb 128-bit vector treated as an vector unsigned
 *  __int128 (qword) element.
 *  @return boolean __int128 for all 128-bits, true if equal,
 *  false otherwise.
 */
static inline
int
vec_cmpuq_all_ne (vui128_t vra, vui128_t vrb)
{
  int result;
#if defined (_ARCH_PWR8) && (__GNUC__ >= 6)
  result = !vec_all_eq((vui64_t)vra, (vui64_t)vrb);
#else
  result = !vec_all_eq((vui32_t)vra, (vui32_t)vrb);
#endif
  return (result);
}

/** \brief Vector combined Multiply by 10 Extended & write Carry Unsigned Quadword.
 *
 *  Compute the product of a 128 bit value a * 10 + digit(cin).
 *  Only the low order 128 bits of the extended product are returned.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 13-15 | 1/cycle  |
 *  |power9   | 3     |1/ 2cycles|
 *
 *  @param *cout pointer to upper 128-bits of the product.
 *  @param a 128-bit vector treated as a unsigned __int128.
 *  @param cin values 0-9 in bits 124:127 of a vector.
 *  @return vector __int128 (upper 128-bits of the 256-bit product) a * 10.
 */
static inline vui128_t
vec_cmul10ecuq (vui128_t *cout, vui128_t a, vui128_t cin)
{
  vui32_t t;
  vui32_t t_carry;
#ifdef _ARCH_PWR9
  __asm__(
      "vmul10ecuq %0,%2,%3;\n"
      "vmul10euq %1,%2,%3;\n"
      : "=&v" (t_carry),
      "=v" (t)
      : "v" (a),
      "v" (cin)
      : );
#else
  vui16_t ts = (vui16_t) a;
  vui32_t tc;
  vui16_t t10;
  vui32_t t_odd, t_even, t_high;
  vui32_t z = { 0, 0, 0, 0 };
  t10 = vec_splat_u16(10);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = vec_vmulouh (ts, t10);
  t_odd = vec_vmuleuh (ts, t10);
#else
  t_even = vec_vmuleuh(ts, t10);
  t_odd = vec_vmulouh(ts, t10);
#endif
  /* Shift t_even left 16-bits (right 112-bits) for the partial carry.  */
  t_high = vec_sld (z, t_even, 2);
  /* Shift cin left 112 bits.  */
  tc = vec_sld ((vui32_t) cin, z, 14);
  /* Shift t_even left 16 bits, merging the carry into the low bits.  */
  t_even = vec_sld (t_even, tc, 2);
  /* then add the even/odd sub-products to generate the final product */
#ifdef _ARCH_PWR8
  /* Any compiler that supports ARCH_PWR8 should support these builtins.  */
  t_carry = t_high; /* there is not carry into high */
  t = (vui32_t) vec_vadduqm ((vui128_t) t_even, (vui128_t) t_odd);
#else
  t_carry = t_high; /* there is no carry into high */
  /* Use pveclib adduqm implementation for pre _ARCH_PWR8.  */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);
#endif
#endif
  *cout = (vui128_t) t_carry;
  return ((vui128_t) t);
}

/** \brief Vector combined Multiply by 10 & write Carry Unsigned Quadword.
 *
 *  compute the product of a 128 bit values a * 10.
 *  Only the low order 128 bits of the product are returned.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 13-15 | 1/cycle  |
 *  |power9   | 3     |1/ 2cycles|
 *
 *  @param *cout pointer to upper 128-bits of the product.
 *  @param a 128-bit vector treated as a __int128.
 *  @return vector __int128 (lower 128-bits of the 256-bit product) a * 10.
 */
static inline vui128_t
vec_cmul10cuq (vui128_t *cout, vui128_t a)
{
  vui32_t t;
  vui32_t t_carry;
#ifdef _ARCH_PWR9
  __asm__(
      "vmul10cuq %0,%2;\n"
      "vmul10uq %1,%2;\n"
      : "=&v" (t_carry),
      "=v" (t)
      : "v" (a)
      : );
#else
  vui16_t ts = (vui16_t) a;
  vui16_t t10;
  vui32_t t_odd, t_even, t_high;
  vui32_t z = { 0, 0, 0, 0 };
  t10 = vec_splat_u16(10);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = vec_vmulouh (ts, t10);
  t_odd = vec_vmuleuh (ts, t10);
#else
  t_even = vec_vmuleuh(ts, t10);
  t_odd = vec_vmulouh(ts, t10);
#endif
  /* Shift t_even left 16-bits (right 112-bits) for the partial carry.  */
  t_high = vec_sld (z, t_even, 2);
  /* Shift t_even left 16 bits to align for lower 128-bits. */
  t_even = vec_sld (t_even, z, 2);
  /* then add the even/odd sub-products to generate the final product */
#ifdef _ARCH_PWR8
  /* Any compiler that supports ARCH_PWR8 should support these builtins.  */
  t_carry = t_high; /* there is no carry into high */
  t = (vui32_t) vec_vadduqm ((vui128_t) t_even, (vui128_t) t_odd);
#else
  t_carry = t_high; /* there is no carry into high */
  /* Use pveclib adduqm implementation for pre _ARCH_PWR8.  */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);
#endif
#endif
  *cout = (vui128_t) t_carry;
  return ((vui128_t) t);
}

/** \brief Vector Multiply by 10 & write Carry Unsigned Quadword.
 *
 *  compute the product of a 128 bit value a * 10.
 *  Only the high order 128 bits of the product are returned.
 *  This will be binary coded decimal value 0-9 in bits 124-127,
 *  Bits 0-123 will be '0'.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 13-15 | 1/cycle  |
 *  |power9   | 3     | 1/cycle  |
 *
 *  @param a 128-bit vector treated as a __int128.
 *  @return __int128 (upper 128-bits of the 256-bit product) a * 10 >> 128.
 */
static inline vui128_t
vec_mul10cuq (vui128_t a)
{
  vui32_t t_carry;
#ifdef _ARCH_PWR9
  __asm__(
      "vmul10cuq %0,%1;\n"
      : "=v" (t_carry)
      : "v" (a)
      : );
#else
  vui16_t ts = (vui16_t) a;
  vui16_t t10;
  vui32_t t_even, t_odd, t_high;
  vui32_t z = { 0, 0, 0, 0 };
  t10 = vec_splat_u16(10);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = vec_vmulouh (ts, t10);
  t_odd = vec_vmuleuh (ts, t10);
#else
  t_even = vec_vmuleuh(ts, t10);
  t_odd = vec_vmulouh(ts, t10);
#endif
  /* Shift t_even left 16-bits (right 112-bits) for the partial carry.  */
  t_high = vec_sld (z, t_even, 2);
  /* Shift t_even left 16 bits to align for lower 128-bits. */
  t_even = vec_sld (t_even, z, 2);
  /* then add the even/odd sub-products to generate the final product */
#ifdef _ARCH_PWR8
  /* Any compiler that supports ARCH_PWR8 should support these builtins.  */
  t_carry = (vui32_t) vec_vaddcuq ((vui128_t) t_even, (vui128_t) t_odd);
  t_carry = (vui32_t) vec_vadduqm ((vui128_t) t_carry, (vui128_t) t_high);
#else
  /* Use pveclib addcuq implementation for pre _ARCH_PWR8.  */
  t_carry = (vui32_t) vec_addcuq ((vui128_t) t_even, (vui128_t) t_odd);
  /* The final carry is small (0-9) so use word add, ignore carries.  */
  t_carry = vec_vadduwm (t_carry, t_high);
#endif
#endif
  return ((vui128_t) t_carry);
}

/** \brief Vector Multiply by 10 Extended & write Carry Unsigned Quadword.
 *
 *  Compute the product of a 128 bit value a * 10 + digit(cin).
 *  Only the low order 128 bits of the extended product are returned.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 15-17 | 1/cycle  |
 *  |power9   | 3     | 1/cycle  |
 *
 *  @param a 128-bit vector treated as a unsigned __int128.
 *  @param cin values 0-9 in bits 124:127 of a vector.
 *  @return __int128 (upper 128-bits of the 256-bit product) a * 10 >> 128.
 */
static inline vui128_t
vec_mul10ecuq (vui128_t a, vui128_t cin)
{
//        vui32_t  t;
  vui32_t t_carry;
#ifdef _ARCH_PWR9
  __asm__(
      "vmul10ecuq %0,%1,%2;\n"
      : "=&v" (t_carry)
      : "v" (a),
      "v" (cin)
      : );
#else
  vui16_t ts = (vui16_t) a;
  vui32_t tc;
  vui16_t t10;
  vui32_t t_odd;
  vui32_t t_even, t_high;
  vui32_t z = { 0, 0, 0, 0 };
  t10 = vec_splat_u16(10);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = vec_vmulouh (ts, t10);
  t_odd = vec_vmuleuh (ts, t10);
#else
  t_even = vec_vmuleuh(ts, t10);
  t_odd = vec_vmulouh(ts, t10);
#endif
  /* Shift t_even left 16-bits (right 112-bits) for the partial carry.  */
  t_high = vec_sld (z, t_even, 2);
  /* Shift cin left 112 bits.  */
  tc = vec_sld ((vui32_t) cin, z, 14);
  /* Shift t_even left 16 bits, merging the carry into the low bits.  */
  t_even = vec_sld (t_even, tc, 2);
  /* then add the even/odd sub-products to generate the final product */
#ifdef _ARCH_PWR8
  /* Any compiler that supports ARCH_PWR8 should support these builtins.  */
  t_carry = (vui32_t) vec_vaddcuq ((vui128_t) t_even, (vui128_t) t_odd);
  t_carry = (vui32_t) vec_vadduqm ((vui128_t) t_carry, (vui128_t) t_high);
#else
  /* Use pveclib addcuq implementation for pre _ARCH_PWR8.  */
  t_carry = (vui32_t) vec_addcuq ((vui128_t) t_even, (vui128_t) t_odd);
  /* The final carry is small (0-9) so use word add, ignore carries.  */
  t_carry = vec_vadduwm (t_carry, t_high);
#endif
#endif
  return ((vui128_t) t_carry);
}

/** \brief Vector Multiply by 10 extended Unsigned Quadword.
 *
 *  compute the product of a 128 bit value a * 10 + digit(cin).
 *  Only the low order 128 bits of the extended product are returned.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 13-15 | 1/cycle  |
 *  |power9   | 3     | 1/cycle  |
 *
 *  @param a 128-bit vector treated as a unsigned __int128.
 *  @param cin values 0-9 in bits 124:127 of a vector.
 *  @return __int128 (lower 128-bits) a * 10.
 */
static inline vui128_t
vec_mul10euq (vui128_t a, vui128_t cin)
{
  vui32_t t;
#ifdef _ARCH_PWR9
  __asm__(
      "vmul10euq %0,%1,%2;\n"
      : "=v" (t)
      : "v" (a),
      "v" (cin)
      : );
#else
  vui16_t ts = (vui16_t) a;
  vui32_t tc;
  vui16_t t10;
  vui32_t t_odd, t_even;
  vui32_t z = { 0, 0, 0, 0 };
  t10 = vec_splat_u16(10);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = vec_vmulouh (ts, t10);
  t_odd = vec_vmuleuh (ts, t10);
#else
  t_even = vec_vmuleuh(ts, t10);
  t_odd = vec_vmulouh(ts, t10);
#endif
  /* Shift cin left 112 bits.  */
  tc = vec_sld ((vui32_t) cin, z, 14);
  /* Shift t_even left 16 bits, merging the carry into the low bits.  */
  t_even = vec_sld (t_even, tc, 2);
  /* then add the even/odd sub-products to generate the final product.  */
#ifdef _ARCH_PWR8
  t = (vui32_t) vec_vadduqm ((vui128_t) t_even, (vui128_t) t_odd);
#else
  /* Use pveclib addcuq implementation for pre _ARCH_PWR8.  */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);
#endif
#endif
  return ((vui128_t) t);
}

/** \brief Vector Multiply by 10 Unsigned Quadword.
 *
 *  compute the product of a 128 bit value a * 10.
 *  Only the low order 128 bits of the product are returned.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 13-15 | 1/cycle  |
 *  |power9   | 3     | 1/cycle  |
 *
 *  @param a 128-bit vector treated as a __int128.
 *  @return __int128 (lower 128-bits) a * 10.
 */
static inline vui128_t
vec_mul10uq (vui128_t a)
{
  vui32_t t;
#ifdef _ARCH_PWR9
  __asm__(
      "vmul10uq %0,%1;\n"
      : "=v" (t)
      : "v" (a)
      : );
#else
  vui16_t ts = (vui16_t) a;
  vui16_t t10;
  vui32_t t_odd, t_even;
  vui32_t z = { 0, 0, 0, 0 };
  t10 = vec_splat_u16(10);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = vec_vmulouh (ts, t10);
  t_odd = vec_vmuleuh (ts, t10);
#else
  t_even = vec_vmuleuh(ts, t10);
  t_odd = vec_vmulouh(ts, t10);
#endif
  /* Shift t_even left 16 bits */
  t_even = vec_sld (t_even, z, 2);
  /* then add the even/odd sub-products to generate the final product */
#ifdef _ARCH_PWR8
  t = (vui32_t) vec_vadduqm ((vui128_t) t_even, (vui128_t) t_odd);
#else
  /* Use pveclib addcuq implementation for pre _ARCH_PWR8.  */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);
#endif
#endif
  return ((vui128_t) t);
}

/** \brief Vector combined Multiply by 100 & write Carry Unsigned Quadword.
 *
 *  compute the product of a 128 bit values a * 100.
 *  Only the low order 128 bits of the product are returned.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 13-15 | 1/cycle  |
 *  |power9   | 6     | 1/cycle  |
 *
 *  @param *cout pointer to upper 128-bits of the product.
 *  @param a 128-bit vector treated as a __int128.
 *  @return vector __int128 (lower 128-bits of the 256-bit product) a * 100.
 */
static inline vui128_t
vec_cmul100cuq (vui128_t *cout, vui128_t a)
{
  vui32_t t;
  vui32_t t_carry;
#ifdef _ARCH_PWR9
  vui128_t t0, t1, tc0, tc1;
  /* Times 10 with 1st carry.  */
  tc0 = vec_mul10cuq (a);
  t0  = vec_mul10uq (a);
  /* Times 10 again with 2nd carry.  */
  tc1 = vec_mul10cuq (t0);
  t1  = vec_mul10uq (t0);
  /* 1st carry times 10 plus 2nd carry.  */
  t_carry  = (vui32_t)vec_mul10euq (tc0, tc1);
  t = (vui32_t)t1;
#else
  vui16_t ts = (vui16_t) a;
  vui16_t t100 = (vui16_t ) { 100, 100, 100, 100, 100, 100, 100, 100 };
  vui32_t t_odd, t_even, t_high;
  vui32_t z = { 0, 0, 0, 0 };
  //t100 = vec_splat_u16 (100);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = vec_vmulouh (ts, t100);
  t_odd = vec_vmuleuh (ts, t100);
#else
  t_even = vec_vmuleuh(ts, t100);
  t_odd = vec_vmulouh(ts, t100);
#endif
  /* Shift t_even left 16-bits (right 112-bits) for the partial carry.  */
  t_high = vec_sld (z, t_even, 2);
  /* Shift t_even left 16 bits to align for lower 128-bits. */
  t_even = vec_sld (t_even, z, 2);
  /* then add the even/odd sub-products to generate the final product */
#ifdef _ARCH_PWR8
  /* Any compiler that supports ARCH_PWR8 should support these builtins.  */
  t_carry = t_high; /* there is no carry into high */
  t = (vui32_t) vec_vadduqm ((vui128_t) t_even, (vui128_t) t_odd);
#else
  t_carry = t_high; /* there is no carry into high */
  /* Use pveclib adduqm implementation for pre _ARCH_PWR8.  */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);
#endif
#endif
  *cout = (vui128_t) t_carry;
  return ((vui128_t) t);
}

/** \brief Vector combined Multiply by 100 Extended & write Carry Unsigned Quadword.
 *
 *  Compute the product of a 128 bit value a * 100 + digit(cin).
 *  The function return its low order 128 bits of the extended product.
 *  The first parameter (*cout) it the address of the vector to receive
 *  the generated carry out in the range 0-99.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 15-17 | 1/cycle  |
 *  |power9   | 9     | 1/cycle  |
 *
 *  @param *cout pointer to upper 128-bits of the product.
 *  @param a 128-bit vector treated as a unsigned __int128.
 *  @param cin values 0-99 in bits 120:127 of a vector.
 *  @return vector __int128 (lower 128-bits of the 256-bit product) a * 100.
 */
static inline vui128_t
vec_cmul100ecuq (vui128_t *cout, vui128_t a, vui128_t cin)
{
  vui32_t t;
  vui32_t t_carry;
#ifdef _ARCH_PWR9
  vui128_t t0, t1, tc0, tc1;
  /* Times 10 with 1st carry.  */
  tc0 = vec_mul10cuq (a);
  t0  = vec_mul10uq (a);
  /* Times 10 again with 2nd carry.  No carry in yet.  */
  tc1 = vec_mul10cuq (t0);
  t1  = vec_mul10uq (t0);
  /* 1st carry times 10 plus 2nd carry.  */
  t_carry  = (vui32_t)vec_mul10euq (tc0, tc1);
  /* Add cin to the low bits of a * 100.  If cin is in valid range
   * (0-99) then can not generate carry out of low 128-bits.  */
  t = (vui32_t)vec_vadduqm ((vui128_t)t1, cin);
#else
  vui16_t ts = (vui16_t) a;
  vui32_t tc;
  vui16_t t100 = (vui16_t ) { 100, 100, 100, 100, 100, 100, 100, 100 };
  vui32_t t_odd, t_even, t_high;
  vui32_t z = { 0, 0, 0, 0 };
  //t100 = vec_splat_u16 (100);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = vec_vmulouh (ts, t100);
  t_odd = vec_vmuleuh (ts, t100);
#else
  t_even = vec_vmuleuh(ts, t100);
  t_odd = vec_vmulouh(ts, t100);
#endif
  /* Shift t_even left 16-bits (right 112-bits) for the partial carry.  */
  t_high = vec_sld (z, t_even, 2);
  /* Shift cin left 112 bits.  */
  tc = vec_sld ((vui32_t) cin, z, 14);
  /* Shift t_even left 16 bits, merging the carry into the low bits.  */
  t_even = vec_sld (t_even, tc, 2);
  /* then add the even/odd sub-products to generate the final product */
#ifdef _ARCH_PWR8
  /* Any compiler that supports ARCH_PWR8 should support these builtins.  */
  t_carry = t_high; /* there is no carry into high */
  t = (vui32_t) vec_vadduqm ((vui128_t) t_even, (vui128_t) t_odd);
#else
  t_carry = t_high; /* there is no carry into high */
  /* Use pveclib adduqm implementation for pre _ARCH_PWR8.  */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);
#endif
#endif
  *cout = (vui128_t) t_carry;
  return ((vui128_t) t);
}

/** \brief Vector Multiply-Sum Unsigned Doubleword Modulo.
 *
 *  compute the even and odd produ256 bit product of two 128 bit values a, b.
 *  Only the low order 128 bits of the product are returned.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 44    | 1/cycle  |
 *  |power9   | 5-7   | 2/cycle  |
 *
 *  @param a 128-bit vector treated as __vector unsigned long int.
 *  @param b 128-bit vector treated as __vector unsigned long int.
 *  @param c 128-bit vector treated as __vector unsigned __int128.
 *  @return __vector unsigned Modulo Sum of the 128-bit even / odd
 *  products of operands a and b plus the unsigned __int128
 *  operand c.
 */
static inline vui128_t
vec_msumudm (vui64_t a, vui64_t b, vui128_t c)
{
  vui128_t res;
#ifdef _ARCH_PWR9
  __asm__(
      "vmsumudm %0,%1,%2,%3;\n"
      : "=v" (res)
      : "v" (a), "v" (b), "v" (c)
      : );
#else
  vui128_t p_even, p_odd, p_sum;

  p_even = vec_muleud (a, b);
  p_odd  = vec_muloud (a, b);
  p_sum  = vec_adduqm (p_even, p_odd);
  res    = vec_adduqm (p_sum, c);
#endif

  return (res);
}

/** \brief Vector multiply even unsigned doublewords.
 *
 *  Multiple the even 64-bit doublewords of two vector unsigned long
 *  values and return the unsigned __int128 product of the even
 *  doublewords.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 21-23 | 1/cycle  |
 *  |power9   | 8-10  | 2/cycle  |
 *
 *  @param a 128-bit vector unsigned long.
 *  @param b 128-bit vector unsigned long.
 *  @return vector unsigned __int128 product of the even double words of a and b.
 */
static inline vui128_t
vec_muleud (vui64_t a, vui64_t b)
{
  vui64_t res;

#ifdef _ARCH_PWR9
  const vui64_t zero = { 0, 0 };
  vui64_t b_eud = vec_mergeh (b, zero);
  __asm__(
      "vmsumudm %0,%1,%2,%3;\n"
      : "=v" (res)
      : "v" (a), "v" (b_eud), "v" (zero)
      : );
#else
#ifdef _ARCH_PWR8
  const vui64_t zero = { 0, 0 };
  vui64_t p0, p1, pp10, pp01;
  vui32_t m0, m1;

  m0 = vec_mergeh ((vui32_t) b, (vui32_t) b);
  m1 = (vui32_t) vec_splat ((vui64_t) a, 0);

  p0 = vec_muleuw (m1, m0);
  p1 = vec_mulouw (m1, m0);
  /* res[1] = p1[1];  res[0] = p0[0];  */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  res = vec_pasted (p1, p0);
#else
  res = vec_pasted (p0, p1);
#endif
  /*
   pp10[1] = p1[0]; pp10[0] = 0;
   pp01[1] = p0[1]; pp01[0] = 0;
   */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  pp10 = (vui64_t) vec_mergeh ((vui64_t) p1, (vui64_t) zero);
  pp01 = (vui64_t) vec_mergel ((vui64_t) p0, (vui64_t) zero);
#else
  pp10 = (vui64_t) vec_mergeh ((vui64_t) zero, (vui64_t) p1);
  pp01 = (vui64_t) vec_mergel ((vui64_t) zero, (vui64_t) p0);
#endif
  /* pp01 = pp01 + pp10.  */
  pp01 = (vui64_t) vec_adduqm ((vui128_t) pp01, (vui128_t) pp10);

  /* res = res + (pp01 << 32)  */
  pp01 = (vui64_t) vec_sld ((vi32_t) pp01, (vi32_t) pp01, 4);
  res = (vui64_t) vec_adduqm ((vui128_t) pp01, (vui128_t) res);
#else
  const vui32_t zero = {0,0,0,0};
//  const vui32_t ones = {-1,-1,-1,-1};
//  const vui32_t cd01 = {0,0,-1,-1};
  vui32_t p0, p1;
  vui32_t resw;
  vui16_t m0, m1, mm;

  m0 = (vui16_t)vec_mergeh (a, (vui64_t)zero);
  mm = (vui16_t)vec_mergeh (b, (vui64_t)zero);

  m1 = vec_splat (mm, 3);

  p0 = vec_vmuleuh (m0, m1);
  p1 = vec_vmulouh (m0, m1);

  resw = vec_sld (zero, p1, 14);
  {
    vui32_t c;
    c    = vec_vaddcuw (resw, p0);
    resw = vec_vadduwm (resw, p0);
    c    = vec_sld (c, c, 4);
    resw = vec_vadduwm (resw, c);
  }

  m1 = vec_splat (mm, 2);
  p0 = vec_vmuleuh (m0, m1);
  p1 = vec_vmulouh (m0, m1);

  {
    vui32_t c;
    c    = vec_vaddcuw (resw, p1);
    resw = vec_vadduwm (resw, p1);
    c    = vec_sld (c, c, 4);
    resw = vec_vadduwm (resw, c);
    resw = vec_sld (c, resw, 14);
  }

  {
    vui32_t c;
    c    = vec_vaddcuw (resw, p0);
    resw = vec_vadduwm (resw, p0);
    c    = vec_sld (c, c, 4);
    resw = vec_vadduwm (resw, c);
  }

  m1 = vec_splat (mm, 1);
  p0 = vec_vmuleuh (m0, m1);
  p1 = vec_vmulouh (m0, m1);

  {
    vui32_t c;
    c    = vec_vaddcuw (resw, p1);
    resw = vec_vadduwm (resw, p1);
    c    = vec_sld (c, c, 4);
    resw = vec_vadduwm (resw, c);
    resw = vec_sld (c, resw, 14);
  }

  {
    vui32_t c;
    c    = vec_vaddcuw (resw, p0);
    resw = vec_vadduwm (resw, p0);
    c    = vec_sld (c, c, 4);
    resw = vec_vadduwm (resw, c);
  }

  m1 = vec_splat (mm, 0);
  p0 = vec_vmuleuh (m0, m1);
  p1 = vec_vmulouh (m0, m1);

  {
    vui32_t c;
    c    = vec_vaddcuw (resw, p1);
    resw = vec_vadduwm (resw, p1);
    c    = vec_sld (c, c, 4);
    resw = vec_vadduwm (resw, c);
    resw = vec_sld (c, resw, 14);
  }

  {
    vui32_t c;
    c    = vec_vaddcuw (resw, p0);
    resw = vec_vadduwm (resw, p0);
    c    = vec_sld (c, c, 4);
    resw = vec_vadduwm (resw, c);
  }

  res = (vui64_t)resw;
#endif
#endif
  return ((vui128_t) res);
}

/** \brief Vector multiply odd unsigned doublewords.
 *
 *  Multiple the odd 64-bit doublewords of two vector unsigned long values and return
 *  the unsigned __int128 product of the odd doublewords.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 21-23 | 1/cycle  |
 *  |power9   | 5-7   | 2/cycle  |
 *
 *  @param a 128-bit vector unsigned long.
 *  @param b 128-bit vector unsigned long.
 * @ return vector unsigned __int128 product of the odd double words of a and b.
 */
static inline vui128_t
vec_muloud (vui64_t a, vui64_t b)
{
  vui64_t res;

#ifdef _ARCH_PWR9
  const vui64_t zero = { 0, 0 };
  vui64_t b_oud = vec_mergel (zero, b);
  __asm__(
      "vmsumudm %0,%1,%2,%3;\n"
      : "=v" (res)
      : "v" (a), "v" (b_oud), "v" (zero)
      : );
#else
#ifdef _ARCH_PWR8
  const vui64_t zero = { 0, 0 };
  vui64_t p0, p1, pp10, pp01;
  vui32_t m0, m1;

  m0 = vec_mergel ((vui32_t) b, (vui32_t) b);
  m1 = (vui32_t) vec_splat ((vui64_t) a, 1);
  p0 = vec_muleuw (m1, m0);
  p1 = vec_mulouw (m1, m0);

  /* res[1] = p1[1];  res[0] = p0[0];  */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  res = vec_pasted (p1, p0);
#else
  res = vec_pasted (p0, p1);
#endif
  /*
   pp10[0] = p1[0]; pp10[1] = 0;
   pp01[0] = p0[1]; pp01[1] = 0;
   */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  pp10 = (vui64_t) vec_mergeh ((vui64_t) p1, (vui64_t) zero);
  pp01 = (vui64_t) vec_mergel ((vui64_t) p0, (vui64_t) zero);
#else
  pp10 = (vui64_t)vec_mergeh ((vui64_t)zero, (vui64_t)p1);
  pp01 = (vui64_t)vec_mergel ((vui64_t)zero, (vui64_t)p0);
#endif

  pp01 = (vui64_t) vec_adduqm ((vui128_t) pp01, (vui128_t) pp10);

  pp01 = (vui64_t) vec_sld ((vi32_t) pp01, (vi32_t) pp01, 4);

  res = (vui64_t) vec_adduqm ((vui128_t) pp01, (vui128_t) res);
#else

  const vui32_t zero = {0,0,0,0};
//  const vui32_t ones = {-1,-1,-1,-1};
//  const vui32_t cd01 = {0,0,-1,-1};
  vui32_t p0, p1;
  vui32_t resw;
  vui16_t m0, m1, mm;

  m0 = (vui16_t)vec_mergel (a, (vui64_t)zero);
  mm = (vui16_t)vec_mergel (b, (vui64_t)zero);

  m1 = vec_splat (mm, 3);

  p0 = vec_vmuleuh (m0, m1);
  p1 = vec_vmulouh (m0, m1);

  resw = vec_sld (zero, p1, 14);

  {
    vui32_t c;
    c    = vec_vaddcuw (resw, p0);
    resw = vec_vadduwm (resw, p0);
    c    = vec_sld (c, c, 4);
    resw = vec_vadduwm (resw, c);
  }

  m1 = vec_splat (mm, 2);

  p0 = vec_vmuleuh (m0, m1);
  p1 = vec_vmulouh (m0, m1);
  {
    vui32_t c;
    c    = vec_vaddcuw (resw, p1);
    resw = vec_vadduwm (resw, p1);

    c    = vec_sld (c, c, 4);
    resw = vec_vadduwm (resw, c);
    resw = vec_sld (c, resw, 14);
  }

  {
    vui32_t c;
    c    = vec_vaddcuw (resw, p0);
    resw = vec_vadduwm (resw, p0);
    c    = vec_sld (c, c, 4);
    resw = vec_vadduwm (resw, c);
  }

  m1 = vec_splat (mm, 1);

  p0 = vec_vmuleuh (m0, m1);
  p1 = vec_vmulouh (m0, m1);

  {
    vui32_t c;
    c    = vec_vaddcuw (resw, p1);
    resw = vec_vadduwm (resw, p1);

    c    = vec_sld (c, c, 4);
    resw = vec_vadduwm (resw, c);
    resw = vec_sld (c, resw, 14);
  }

  {
    vui32_t c;
    c    = vec_vaddcuw (resw, p0);
    resw = vec_vadduwm (resw, p0);
    c    = vec_sld (c, c, 4);
    resw = vec_vadduwm (resw, c);
  }

  m1 = vec_splat (mm, 0);

  p0 = vec_vmuleuh (m0, m1);
  p1 = vec_vmulouh (m0, m1);

  {
    vui32_t c;
    c    = vec_vaddcuw (resw, p1);
    resw = vec_vadduwm (resw, p1);

    c    = vec_sld (c, c, 4);
    resw = vec_vadduwm (resw, c);
    resw = vec_sld (c, resw, 14);
  }

  {
    vui32_t c;
    c    = vec_vaddcuw (resw, p0);
    resw = vec_vadduwm (resw, p0);
    c    = vec_sld (c, c, 4);
    resw = vec_vadduwm (resw, c);
  }

  res = (vui64_t)resw;
#endif
#endif
  return ((vui128_t) res);
}

/** \brief Vector Multiply Unsigned double Quadword.
 *
 *  compute the 256 bit product of two 128 bit values a, b.
 *  The low order 128 bits of the product are returned, while
 *  the high order 128-bits are "stored" via the mulu pointer.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 56-64 | 1/cycle  |
 *  |power9   | 33-39 | 1/cycle  |
 *
 *  @param *mulu pointer to upper 128-bits of the product.
 *  @param a 128-bit vector treated a __int128.
 *  @param b 128-bit vector treated a __int128.
 *  @return vector unsigned __int128 (lower 128-bits) of a * b.
 */
static inline vui128_t
vec_muludq (vui128_t *mulu, vui128_t a, vui128_t b)
{
  vui32_t t, tmq;
  /* compute the 256 bit product of two 128 bit values a, b.
   * The high 128 bits are accumulated in t and the low 128-bits
   * in tmq. The high 128-bits of the product are returned to the
   * address of the 1st parm. The low 128-bits are the return
   * value.
   */
#ifdef _ARCH_PWR9
  const vui64_t zero = { 0, 0 };
  vui64_t b_swap = vec_swapd ((vui64_t)b);
  vui128_t tmh, tab, tba, tb0, tc1, tc2;
  /* multiply the low 64-bits of a and b.  For PWR9 this is just
   * vmsumudm with conditioned inputs.  */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  tmq = (vui32_t)vec_muleud ((vui64_t)a, (vui64_t)b);
#else
  tmq = (vui32_t)vec_muloud ((vui64_t)a, (vui64_t)b);
#endif
  /* compute the 2 middle partial projects.  Can't directly use
   * vmsumudm here because the sum of partial products can overflow.  */
  tab = vec_muloud ((vui64_t)a, b_swap);
  tba = vec_muleud ((vui64_t)a, b_swap);
  t   = (vui32_t)vec_addcq  (&tc1, tab, tba);
  tmh   = (vui128_t)vec_sld ((vui32_t)zero, tmq, 8);
  t   = (vui32_t)vec_addcq  (&tc2, (vui128_t)t, tmh);
  tc1 = (vui128_t)vec_vadduwm ((vui32_t)tc1, (vui32_t)tc2);
  /* result = t[l] || tmq[l].  */
  tmq = (vui32_t)vec_mrgld ((vui64_t)t, (vui64_t)tmq);
  /* we can use multiply sum here because the high product plus the
   * high sum of middle partial products can't overflow.  */
  t   = (vui32_t)vec_sld ((vui32_t)tc1, (vui32_t)t, 8);
  tb0 = (vui128_t)vec_mrghd ((vui64_t)b, zero);
  /* sum = (a[h] * b[h]) + (a[l] * 0) + (tc1[l] || t[h]).  */
  t   = (vui32_t)vec_msumudm ((vui64_t)a, (vui64_t)tb0, (vui128_t)t);
#else
#ifdef _ARCH_PWR8
  vui32_t tsw;
  vui32_t tc;
  vui32_t t_odd, t_even;
  vui32_t z = { 0, 0, 0, 0 };
  /* We use the Vector Multiple Even/Odd Unsigned word to compute
   * the 128 x 32 partial (160-bit) product of value a with the
   * word splat of b. These instructions (vmuleum, vmuloum)
   * product four 64-bit 32 x 32 partial products where even
   * results are shifted 32-bit left from odd results. After
   * shifting the high 128 bits can be summed via Vector add
   * unsigned quadword.
   */

  tsw = vec_splat ((vui32_t) b, VEC_WE_3);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = (vui32_t) vec_mulouw ((vui32_t) a, tsw);
  t_odd = (vui32_t) vec_muleuw ((vui32_t) a, tsw);
#else
  t_even = (vui32_t)vec_muleuw((vui32_t)a, tsw);
  t_odd = (vui32_t)vec_mulouw((vui32_t)a, tsw);
#endif
  /* Rotate the low 32-bits (right) into tmq. This is actually
   * implemented as 96-bit (12-byte) shift left. */
  tmq = vec_sld (t_odd, z, 12);
  /* shift the low 128 bits of partial product right 32-bits */
  t_odd = vec_sld (z, t_odd, 12);
  /* add the high 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui32_t) b, VEC_WE_2);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = (vui32_t) vec_mulouw ((vui32_t) a, tsw);
  t_odd = (vui32_t) vec_muleuw ((vui32_t) a, tsw);
#else
  t_even = (vui32_t)vec_muleuw((vui32_t)a, tsw);
  t_odd = (vui32_t)vec_mulouw((vui32_t)a, tsw);
#endif
  /* Sum the low 128 bits of odd previous partial products */
  tc = (vui32_t) vec_addcuq ((vui128_t) t_odd, (vui128_t) t);
  t_odd = (vui32_t) vec_adduqm ((vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 32-bits into tmq */
  tmq = vec_sld (t_odd, tmq, 12);
  /* shift the low 128 bits (with carry) of partial product right
   * 32-bits */
  t_odd = vec_sld (tc, t_odd, 12);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui32_t) b, VEC_WE_1);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = (vui32_t) vec_mulouw ((vui32_t) a, tsw);
  t_odd = (vui32_t) vec_muleuw ((vui32_t) a, tsw);
#else
  t_even = (vui32_t)vec_muleuw((vui32_t)a, tsw);
  t_odd = (vui32_t)vec_mulouw((vui32_t)a, tsw);
#endif
  /* Sum the low 128 bits of odd previous partial products */
  tc = (vui32_t) vec_addcuq ((vui128_t) t_odd, (vui128_t) t);
  t_odd = (vui32_t) vec_adduqm ((vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 32-bits into tmq */
  tmq = vec_sld (t_odd, tmq, 12);
  /* shift the low 129 bits (with carry) of partial product right
   * 32-bits */
  t_odd = vec_sld (tc, t_odd, 12);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui32_t) b, VEC_WE_0);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = (vui32_t) vec_mulouw ((vui32_t) a, tsw);
  t_odd = (vui32_t) vec_muleuw ((vui32_t) a, tsw);
#else
  t_even = (vui32_t)vec_muleuw((vui32_t)a, tsw);
  t_odd = (vui32_t)vec_mulouw((vui32_t)a, tsw);
#endif
  /* Sum the low 128 bits of odd previous partial products,
   * generate the carry.  */
  tc = (vui32_t) vec_addcuq ((vui128_t) t_odd, (vui128_t) t);
  t_odd = (vui32_t) vec_adduqm ((vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 32-bits into tmq */
  tmq = vec_sld (t_odd, tmq, 12);
  /* shift the low 129 bits (with carry) of partial product right
   * 32-bits */
  t_odd = vec_sld (tc, t_odd, 12);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);
#else // _ARCH_PWR7 or earlier and Big Endian only.  */
  /* We use the Vector Multiple Even/Odd Unsigned haldword to compute
   * the 128 x 16 partial (144-bit) product of value a with the
   * halfword splat of b. These instructions (vmuleuh, vmulouh)
   * product 8 32-bit 16 x 16 partial products where even
   * results are shifted 16-bit left from odd results. After
   * shifting the high 128 bits can be summed via Vector add
   * unsigned quadword equivalent.
   */
  vui16_t tsw;
  vui16_t tc;
  vui16_t t_odd, t_even;
  vui16_t z = { 0, 0, 0, 0, 0, 0, 0, 0 };

  tsw = vec_splat ((vui16_t) b, 7);
  t_even = (vui16_t)vec_vmuleuh((vui16_t)a, tsw);
  t_odd = (vui16_t)vec_vmulouh((vui16_t)a, tsw);

  /* Rotate the low 16-bits (right) into tmq. This is actually
   * implemented as 112-bit (14-byte) shift left. */
  tmq = (vui32_t)vec_sld (t_odd, z, 14);
  /* shift the low 128 bits of partial product right 16-bits */
  t_odd = vec_sld (z, t_odd, 14);
  /* add the high 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui16_t) b, 6);

  t_even = (vui16_t)vec_vmuleuh((vui16_t)a, tsw);
  t_odd = (vui16_t)vec_vmulouh((vui16_t)a, tsw);

  /* Sum the low 128 bits of odd previous partial products,
   * generate the carry.  */
  t_odd = (vui16_t) vec_addcq ((vui128_t*) &tc, (vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 16-bits into tmq */
  tmq = (vui32_t)vec_sld (t_odd, (vui16_t)tmq, 14);
  /* shift the low 128 bits (with carry) of partial product right
   * 16-bits */
  t_odd = vec_sld (tc, t_odd, 14);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui16_t) b, 5);

  t_even = (vui16_t)vec_vmuleuh((vui16_t)a, tsw);
  t_odd = (vui16_t)vec_vmulouh((vui16_t)a, tsw);

  /* Sum the low 128 bits of odd previous partial products,
   * generate the carry.  */
  t_odd = (vui16_t) vec_addcq ((vui128_t*) &tc, (vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 16-bits into tmq */
  tmq = (vui32_t)vec_sld (t_odd, (vui16_t)tmq, 14);
  /* shift the low 128 bits (with carry) of partial product right
   * 16-bits */
  t_odd = vec_sld (tc, t_odd, 14);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui16_t) b, 4);

  t_even = (vui16_t)vec_vmuleuh((vui16_t)a, tsw);
  t_odd = (vui16_t)vec_vmulouh((vui16_t)a, tsw);

  /* Sum the low 128 bits of odd previous partial products,
   * generate the carry.  */
  t_odd = (vui16_t) vec_addcq ((vui128_t*) &tc, (vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 16-bits into tmq */
  tmq = (vui32_t)vec_sld (t_odd, (vui16_t)tmq, 14);
  /* shift the low 128 bits (with carry) of partial product right
   * 16-bits */
  t_odd = vec_sld (tc, t_odd, 14);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui16_t) b, 3);

  t_even = (vui16_t)vec_vmuleuh((vui16_t)a, tsw);
  t_odd = (vui16_t)vec_vmulouh((vui16_t)a, tsw);

  /* Sum the low 128 bits of odd previous partial products,
   * generate the carry.  */
  t_odd = (vui16_t) vec_addcq ((vui128_t*) &tc, (vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 16-bits into tmq */
  tmq = (vui32_t)vec_sld (t_odd, (vui16_t)tmq, 14);
  /* shift the low 128 bits (with carry) of partial product right
   * 16-bits */
  t_odd = vec_sld (tc, t_odd, 14);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui16_t) b, 2);

  t_even = (vui16_t)vec_vmuleuh((vui16_t)a, tsw);
  t_odd = (vui16_t)vec_vmulouh((vui16_t)a, tsw);

  /* Sum the low 128 bits of odd previous partial products,
   * generate the carry.  */
  t_odd = (vui16_t) vec_addcq ((vui128_t*) &tc, (vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 16-bits into tmq */
  tmq = (vui32_t)vec_sld (t_odd, (vui16_t)tmq, 14);
  /* shift the low 128 bits (with carry) of partial product right
   * 16-bits */
  t_odd = vec_sld (tc, t_odd, 14);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui16_t) b, 1);

  t_even = (vui16_t)vec_vmuleuh((vui16_t)a, tsw);
  t_odd = (vui16_t)vec_vmulouh((vui16_t)a, tsw);

  /* Sum the low 128 bits of odd previous partial products,
   * generate the carry.  */
  t_odd = (vui16_t) vec_addcq ((vui128_t*) &tc, (vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 16-bits into tmq */
  tmq = (vui32_t)vec_sld (t_odd, (vui16_t)tmq, 14);
  /* shift the low 128 bits (with carry) of partial product right
   * 16-bits */
  t_odd = vec_sld (tc, t_odd, 14);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui16_t) b, 0);

  t_even = (vui16_t)vec_vmuleuh((vui16_t)a, tsw);
  t_odd = (vui16_t)vec_vmulouh((vui16_t)a, tsw);

  /* Sum the low 128 bits of odd previous partial products,
   * generate the carry.  */
  t_odd = (vui16_t) vec_addcq ((vui128_t*) &tc, (vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 16-bits into tmq */
  tmq = (vui32_t)vec_sld (t_odd, (vui16_t)tmq, 14);
  /* shift the low 128 bits (with carry) of partial product right
   * 16-bits */
  t_odd = vec_sld (tc, t_odd, 14);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);
#endif
#endif
  *mulu = (vui128_t) t;
  return ((vui128_t) tmq);
}

/** \brief Vector Multiply Low Unsigned Quadword.
 *
 *  compute the 256 bit product of two 128 bit values a, b.
 *  Only the low order 128 bits of the product are returned.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 42-48 | 1/cycle  |
 *  |power9   | 16-20 | 2/cycle  |
 *
 *  @param a 128-bit vector treated a __int128.
 *  @param b 128-bit vector treated a __int128.
 *  @return __int128 (lower 128-bits) a * b.
 */
static inline vui128_t
vec_mulluq (vui128_t a, vui128_t b)
{
  vui32_t t, tmq;
  /* compute the 256 bit product of two 128 bit values a, b.
   * The high 128 bits are accumulated in t and the low 128-bits
   * in tmq.  Only the low order 128 bits of the product are
   * returned.
   */
#ifdef _ARCH_PWR9
  const vui64_t zero = { 0, 0 };
  vui64_t b_swap = vec_swapd ((vui64_t)b);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  tmq = (vui32_t)vec_muleud ((vui64_t)a, (vui64_t)b);
  t   = (vui32_t)vec_sld ((vui32_t)zero, tmq, 8);
  t   = (vui32_t)vec_msumudm ((vui64_t)a, b_swap, (vui128_t)t);
  tmq = (vui32_t)vec_mergeh ((vui64_t)tmq, (vui64_t)t);
#else
  /* multiply the low 64-bits of a and b.  For PWR9 this is just
   * vmsumudm with conditioned inputs.  */
  tmq = (vui32_t)vec_muloud ((vui64_t)a, (vui64_t)b);
  /* we can use multiply sum here because we only need the low 64-bits
   * and don't care if we lose the carry / overflow.  */
  t   = (vui32_t)vec_mergeh (zero, (vui64_t)tmq);
  /* sum = (a[h] * b[l]) + (a[l] * b[h]) + (zero || tmq[h]).  */
  t   = (vui32_t)vec_msumudm ((vui64_t)a, b_swap, (vui128_t)t);
  /* result = t[l] || tmq[l].  */
  tmq = (vui32_t)vec_mergel ((vui64_t)t, (vui64_t)tmq);
#endif
#else
#ifdef _ARCH_PWR8
  /*
   * We use the Vector Multiple Even/Odd Unsigned word to compute
   * the 128 x 32 partial (160-bit) product of value a with the
   * word splat of b. These instructions (vmuleum, vmuloum)
   * product four 64-bit 32 x 32 partial products where even
   * results are shifted 32-bit left from odd results. After
   * shifting the high 128 bits can be summed via Vector add
   * unsigned quadword.
   *
   */
  vui32_t tsw;
  vui32_t t_odd, t_even;
  vui32_t z = { 0, 0, 0, 0 };

  tsw = vec_splat ((vui32_t) b, VEC_WE_3);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = (vui32_t) vec_mulouw ((vui32_t) a, tsw);
  t_odd = (vui32_t) vec_muleuw ((vui32_t) a, tsw);
#else
  t_even = (vui32_t)vec_muleuw((vui32_t)a, tsw);
  t_odd = (vui32_t)vec_mulouw((vui32_t)a, tsw);
#endif
  /* Rotate the low 32-bits (right) into tmq. This is actually
   * implemented as 96-bit (12-byte) shift left. */
  tmq = vec_sld (t_odd, z, 12);
  /* shift the low 128 bits of partial product right 32-bits */
  t_odd = vec_sld (z, t_odd, 12);
  /* add the high 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui32_t) b, VEC_WE_2);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = (vui32_t) vec_mulouw ((vui32_t) a, tsw);
  t_odd = (vui32_t) vec_muleuw ((vui32_t) a, tsw);
#else
  t_even = (vui32_t)vec_muleuw((vui32_t)a, tsw);
  t_odd = (vui32_t)vec_mulouw((vui32_t)a, tsw);
#endif
  /* Sum the low 128 bits of odd previous partial products */
  t_odd = (vui32_t) vec_adduqm ((vui128_t) t_odd, (vui128_t) t);
  /* rotate right the low 32-bits into tmq */
  tmq = vec_sld (t_odd, tmq, 12);
  /* shift the low 128 bits of partial product right 32-bits */
  t_odd = vec_sld (z, t_odd, 12);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui32_t) b, VEC_WE_1);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = (vui32_t) vec_mulouw ((vui32_t) a, tsw);
  t_odd = (vui32_t) vec_muleuw ((vui32_t) a, tsw);
#else
  t_even = (vui32_t)vec_muleuw((vui32_t)a, tsw);
  t_odd = (vui32_t)vec_mulouw((vui32_t)a, tsw);
#endif
  /* add the low 128 bits of odd / previous partial products */
  t_odd = (vui32_t) vec_adduqm ((vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 32-bits into tmq */
  tmq = vec_sld (t_odd, tmq, 12);
  /* shift the low 128 bits of partial product right 32-bits */
  t_odd = vec_sld (z, t_odd, 12);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui32_t) b, VEC_WE_0);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = (vui32_t) vec_mulouw ((vui32_t) a, tsw);
  t_odd = (vui32_t) vec_muleuw ((vui32_t) a, tsw);
#else
  t_even = (vui32_t)vec_muleuw((vui32_t)a, tsw);
  t_odd = (vui32_t)vec_mulouw((vui32_t)a, tsw);
#endif
  /* add the low 128 bits of odd / previous partial products */
  t_odd = (vui32_t) vec_adduqm ((vui128_t) t_odd, (vui128_t) t);
  /* rotate right the low 32-bits into tmq */
  tmq = vec_sld (t_odd, tmq, 12);
  /* shift the low 128 bits of partial product right 32-bits */
  t_odd = vec_sld (z, t_odd, 12);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);
#else
//#warning implementation pre power8

  tmq = (vui32_t)vec_muludq ((vui128_t*)&t, a, b);
#endif
#endif
  return ((vui128_t) tmq);
}

/** \brief Population Count vector __int128.
 *
 *  Count the number of '1' bits within a vector __int128 and return
 *  the count (0-128) in a vector __int128.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 15    |2/2 cycles|
 *  |power9   | 16    | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated a __int128.
 *  @return a 128-bit vector with bits 121:127 containing the
 *  population count.
 */
static inline vui128_t
vec_popcntq (vui128_t vra)
{
  vui64_t result;

#ifdef _ARCH_PWR8
  /*
   * Use the Vector Population Count Doubleword instruction to get
   * the count for the left and right vector halves.  Then sum across
   * the left and right counts to get the final 128-bit vector count
   * (0-128).
   */
  vui64_t vt1;
  const vui64_t vzero = { 0, 0 };

  vt1 = vec_vpopcnt ((__vector unsigned long long) vra);
  result = (vui64_t) vec_sums ((__vector int) vt1,
                               (__vector int) vzero);
#else
  //#warning Implememention pre power8
  vui32_t z= { 0,0,0,0};
  vui32_t x;
  x = vec_popcntw ((vui32_t)vra);
  result = (vui64_t)vec_sums ((vi32_t)x, (vi32_t)z);
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  result = (vui64_t) vec_sld (
      (__vector unsigned char) result, (__vector unsigned char) result, 4);
#endif
  return ((vui128_t) result);
}

/*! \brief byte reverse quadword for a vector __int128.
 *
 *  Return the bytes / octets of a 128-bit vector in reverse order.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-13  | 2 cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated a __int128.
 *  @return a 128-bit vector with the bytes in reserve order.
 */
static inline vui128_t
vec_revbq (vui128_t vra)
{
  vui128_t result;

#ifdef _ARCH_PWR9
#ifndef vec_revb
  __asm__(
      "xxbrq %x0,%x1;"
      : "=wa" (result)
      : "wa" (vra)
      : );
#else
  result = vec_revb (vra);
#endif
#else
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  const vui64_t vconstp = CONST_VINT64_DW(0x0F0E0D0C0B0A0908UL, 0x0706050403020100UL);
#else
  const vui64_t vconstp =
      CONST_VINT64_DW(0x0001020304050607UL, 0x08090A0B0C0D0E0FUL);
#endif
  result = (vui128_t) vec_perm ((vui8_t) vra, (vui8_t) vra, (vui8_t) vconstp);
#endif

  return (result);
}

/*! \brief Vector Set Bool from Quadword Carry.
 *
 *  If the vector quadword carry bit (vcy.bit[127]) is '1'
 *  then return a vector bool __int128 that is all '1's.
 *  Otherwise return all '0's.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4 - 6 |2/2 cycles|
 *  |power9   | 3 - 5 | 2/cycle  |
 *
 *  Vector quadword carries are normally the result of a
 *  <I>write-Carry</I> operation. For example; vec_addcuq(),
 *  vec_addecuq(), vec_subcuq(), vec_subecuq(), vec_addcq(),
 *  vec_addeq().
 *
 *  @param vcy a 128-bit vector generated from a <I>write-Carry</I>
 *  operation.
 *  @return a 128-bit vector bool of all '1's if the carry bit is '1'.
 *  Otherwise all '0's.
 */
static inline vb128_t
vec_setb_cyq (vui128_t vcy)
{
#ifdef _ARCH_PWR8
  const vui128_t zero = (vui128_t) vec_splat_u32(0);

  return (vb128_t) vec_vsubuqm (zero, vcy);
#else
  const vui32_t ones =  vec_splat_u32(1);
  vui32_t rcy;

  rcy = vec_and ((vui32_t)vcy, ones);
  rcy = (vui32_t)vec_cmpeq (rcy, ones);
  rcy = vec_splat (rcy, VEC_W_L);

  return (vb128_t) rcy;
#endif
}

/*! \brief Vector Set Bool from Quadword not Carry.
 *
 *  If the vector quadword carry bit (vcy.bit[127]) is '1'
 *  then return a vector bool __int128 that is all '0's.
 *  Otherwise return all '1's.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4 - 6 |2/2 cycles|
 *  |power9   | 3 - 5 | 2/cycle  |
 *
 *  Vector quadword carries are normally the result of a
 *  <I>write-Carry</I> operation. For example; vec_addcuq(),
 *  vec_addecuq(), vec_subcuq(), vec_subecuq(), vec_addcq(),
 *  vec_addeq().
 *
 *  @param vcy a 128-bit vector generated from a <I>write-Carry</I>
 *  operation.
 *  @return a 128-bit vector bool of all '1's if the carry bit is '0'.
 *  Otherwise all '0's.
 */
static inline vb128_t
vec_setb_ncq (vui128_t vcy)
{
#ifdef _ARCH_PWR8
  const vui128_t zero = (vui128_t) vec_splat_u32(0);

  return (vb128_t) vec_vsubeuqm (zero, zero, vcy);
#else
  const vui32_t ones =  vec_splat_u32(1);
  vui32_t rcy;

  rcy = vec_and ((vui32_t)vcy, ones);
  rcy = (vui32_t)vec_cmplt (rcy, ones);
  rcy = vec_splat (rcy, VEC_W_L);

  return (vb128_t) rcy;
#endif
}

/*! \brief Vector Set Bool from Signed Quadword.
 *
 *  If the quadword's sign bit is '1' then return a vector bool
 *  __int128 that is all '1's. Otherwise return all '0's.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4 - 6 | 2/cycle  |
 *  |power9   | 5 - 8 | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated a signed __int128.
 *  @return a 128-bit vector bool of all '1's if the sign bit is '1'.
 *  Otherwise all '0's.
 */
static inline vb128_t
vec_setb_sq (vi128_t vra)
{
  const vui8_t shift = vec_splat_u8 (7);
  vui8_t splat = vec_splat ((vui8_t) vra, VEC_BYTE_H);

  return (vb128_t) vec_sra (splat, shift);
}

/** \brief Vector Shift Left double Quadword.
 *
 *  Vector Shift Left double Quadword 0-127 bits.
 *  Return a vector __int128 that is the left most 128-bits after
 *  shifting left 0-127-bits of the 32-byte double vector
 *  (vrw||vrx).  The shift amount is from bits 121:127 of vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10    | 1 cycle  |
 *  |power9   | 14    | 1/cycle  |
 *
 *  @param vrw upper 128-bits of the 256-bit double vector.
 *  @param vrx lower 128-bits of the 256-bit double vector.
 *  @param vrb Shift amount in bits 121:127.
 *  @return high 128-bits of left shifted double vector.
 */
static inline vui128_t
vec_sldq (vui128_t vrw, vui128_t vrx, vui128_t vrb)
{
  __vector unsigned char result, vt1, vt2, vt3, vbs;
  const __vector unsigned char vzero = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0 };

  vt1 = vec_slo ((__vector unsigned char) vrw, (__vector unsigned char) vrb);
  /* The vsr instruction only works correctly if the bit shift value
     is splatted to each byte of the vector.  */
  vbs = vec_splat ((__vector unsigned char) vrb, VEC_BYTE_L);
  vt1 = vec_sll (vt1, vbs);
  vt3 = vec_sub (vzero, vbs);
  vt2 = vec_sro ((__vector unsigned char) vrx, vt3);
  vt2 = vec_srl (vt2, vt3);
  result = vec_or (vt1, vt2);

  return ((vui128_t) result);
}

/** \brief Vector Shift left Quadword Immediate.
 *
 *  Shift left Quadword 0-127 bits.
 *  The shift amount is a const unsigned int in the range 0-127.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 127 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-13  | 2 cycle  |
 *  |power9   | 3-15  | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as a __int128.
 *  @param shb Shift amount in the range 0-127.
 *  @return 128-bit vector shifted left shb bits.
 */
static inline vui128_t
vec_slqi (vui128_t vra, const unsigned int shb)
{
  vui8_t lshift;
  vui8_t result;

  if (shb < 128)
    {
      if (__builtin_constant_p (shb) && ((shb % 8) == 0))
	{
	  /* When shifting an multiple of 8 bits (octet), use Vector
	   Shift Left Double By Octet Immediate.  This eliminates
	   loading the shift const into a VR, but requires an
	   explicit vector of zeros.  */
	  vui8_t zero =
	    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	  if (shb > 0)
	    result = vec_sld ((vui8_t) vra, zero, (shb / 8));
	  else
	    result = (vui8_t) vra;
	}
      else
	{
	  /* Load the shift const in a vector.  The bit level shifts
	   require the shift amount is splatted to all 16-bytes of
	   the shift control.  */
	  if (__builtin_constant_p (shb) && (shb < 16))
	    lshift = (vui8_t) vec_splat_s8(shb);
	  else
	    lshift = vec_splats ((unsigned char) shb);

	  if (shb > 7)
	    /* Vector Shift Left By Octet by bits 121-124 of lshift.  */
	    result = vec_slo ((vui8_t) vra, lshift);
	  else
	    result = ((vui8_t) vra);

	  /* Vector Shift Left by bits 125-127 of lshift.  */
	  result = vec_sll (result, lshift);
	}
    }
  else
    { /* shifts greater then 127 bits return zeros.  */
      result = vec_xor ((vui8_t) vra, (vui8_t) vra);
    }
  return (vui128_t) result;
}

/** \brief Vector Shift Left Quadword.
 *
 *  Vector Shift Left Quadword 0-127 bits.
 *  The shift amount is from bits 121-127 of vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     | 1/cycle  |
 *  |power9   | 6     | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as a __int128.
 *  @param vrb Shift amount in bits 121:127.
 *  @return Left shifted vector.
 */
static inline vui128_t
vec_slq (vui128_t vra, vui128_t vrb)
{
  __vector unsigned char result, vshift_splat;

  /* For some reason we let the processor jockies write they
   * hardware bug into the ISA.  The vsl instruction only works
   * correctly if the bit shift value is splatted to each byte
   * of the vector.  */
  vshift_splat = vec_splat ((__vector unsigned char) vrb, VEC_BYTE_L);
  result = vec_slo ((__vector unsigned char) vra, (__vector unsigned char) vrb);
  result = vec_sll (result, vshift_splat);

  return ((vui128_t) result);
}

/** \brief Vector Shift right Quadword Immediate.
 *
 *  Shift right Quadword 0-127 bits.
 *  The shift amount is a const unsigned int in the range 0-127.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 127 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-13  | 2 cycle  |
 *  |power9   | 3-15  | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as a __int128.
 *  @param shb Shift amount in the range 0-127.
 *  @return 128-bit vector shifted right shb bits.
 */
static inline vui128_t
vec_srqi (vui128_t vra, const unsigned int shb)
{
  vui8_t lshift;
  vui8_t result;

  if (shb < 128)
    {
      if (__builtin_constant_p (shb) && ((shb % 8)) == 0)
	{
	  /* When shifting an multiple of 8 bits (octet), use Vector
	   Shift Left Double By Octet Immediate.  This eliminates
	   loading the shift const into a VR, but requires an
	   explicit vector of zeros.  */
	  vui8_t zero =
	    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	  /* The compiler needs to know at compile time that
	     0 < shb < 128 is true to insure the constraint (4 bit
	     immediate field) of vsldoi is meet.  So the following if
	     is required but should not generate any branch code.  */
	  if (shb > 0)
	    result = vec_sld (zero, (vui8_t) vra, (16 - (shb / 8)));
	  else
	    result = (vui8_t) vra;
	}
      else
	{
	  /* Load the shift const in a vector.  The bit level shifts
	   require the shift amount is splatted to all 16-bytes of
	   the shift control.  */
	  if ((__builtin_constant_p (shb) && (shb < 16)))
	    lshift = (vui8_t) vec_splat_s8(shb);
	  else
	    lshift = vec_splats ((unsigned char) shb);

	  if (shb > 7)
	    /* Vector Shift right By Octet based on the bits 121-124 of
	     lshift.  */
	    result = vec_sro ((vui8_t) vra, lshift);
	  else
	    result = ((vui8_t) vra);

	  /* Vector Shift right based on the lower 3-bits of lshift.  */
	  result = vec_srl (result, lshift);
	}
    }
  else
    { /* shifts greater then 127 bits return zeros.  */
      result = vec_xor ((vui8_t) vra, (vui8_t) vra);
    }
  return (vui128_t) result;
}

/** \brief Vector Shift right Quadword.
 *
 *  Vector Shift Right Quadword 0-127 bits.
 *  The shift amount is from bits 121-127 of vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     | 1/cycle  |
 *  |power9   | 6     | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as a __int128.
 *  @param vrb Shift amount in bits 121:127.
 *  @return Right shifted vector.
 */
static inline vui128_t
vec_srq (vui128_t vra, vui128_t vrb)
{
  __vector unsigned char result, vsht_splat;

  /* For some reason we let the processor jockies write they
   * hardware bug into the ISA.  The vsr instruction only works
   * correctly if the bit shift value is splatted to each byte
   * of the vector.  */
  vsht_splat = vec_splat ((__vector unsigned char) vrb, VEC_BYTE_L);
  result = vec_sro ((__vector unsigned char) vra, (__vector unsigned char) vrb);
  result = vec_srl (result, vsht_splat);

  return ((vui128_t) result);
}

/** \deprecated Vector Shift Left 4-bits Quadword.
 * Replaced by vec_slqi with shb param = 4.
 *
 * Vector Shift Left Quadword 0-127 bits.
 * The shift amount is from bits 121-127 of vrb.
 *
 * @param vra a 128-bit vector treated a __int128.
 * @return Left shifted vector.
 */
static inline vui128_t
vec_slq4 (vui128_t vra)
{
  __vector unsigned char result, vsht_splat;

  /* The vsl instruction only works correctly if the bit shift value
   * is splatted to each byte of the vector.  */
  vsht_splat = vec_splat_u8(4);
  result = vec_sll ((__vector unsigned char) vra, vsht_splat);

  return ((vui128_t) result);
}

/** \deprecated Vector Shift Left 5-bits Quadword.
 * Replaced by vec_slqi with shb param = 5.
 *
 * Vector Shift Left Quadword 0-127 bits.
 * The shift amount is from bits 121-127 of vrb.
 *
 *	@param vra a 128-bit vector treated a __int128.
 *	@return Left shifted vector.
 */
static inline vui128_t
vec_slq5 (vui128_t vra)
{
  __vector unsigned char result, vsht_splat;

  /* The vsl instruction only works correctly if the bit shift value
   * is splatted to each byte of the vector.  */
  vsht_splat = vec_splat_u8(5);
  result = vec_sll ((__vector unsigned char) vra, vsht_splat);

  return ((vui128_t) result);
}

/** \deprecated Vector Shift right 4-bits Quadword.
 * Replaced by vec_srqi with shb param = 4.
 *
 * Vector Shift Right Quadword 0-127 bits.
 * The shift amount is from bits 121-127 of vrb.
 *
 * @param vra a 128-bit vector treated as a __int128.
 * @return Right shifted vector.
 */
static inline vui128_t
vec_srq4 (vui128_t vra)
{
  __vector unsigned char result, vsht_splat;

  /* The vsr instruction only works correctly if the bit shift value
   * is splatted to each byte of the vector.  */
  vsht_splat = vec_splat_u8(4);
  result = vec_srl ((__vector unsigned char) vra, vsht_splat);

  return ((vui128_t) result);
}

/** \deprecated Vector Shift right 5-bits Quadword.
 * Replaced by vec_srqi with shb param = 5.
 *
 * Vector Shift Right Quadword 0-127 bits.
 * The shift amount is from bits 121-127 of vrb.
 *
 * @param vra a 128-bit vector treated a __int128.
 * @return Right shifted vector.
 */
static inline vui128_t
vec_srq5 (vui128_t vra)
{
  __vector unsigned char result, vsht_splat;

  /* The vsr instruction only works correctly if the bit shift value
   * is splatted to each byte of the vector.  */
  vsht_splat = vec_splat_u8(5);
  result = vec_srl ((__vector unsigned char) vra, vsht_splat);

  return ((vui128_t) result);
}

/** \brief Vector Subtract and Write Carry Unsigned Quadword.
 *
 *  Generate the carry-out of the sum (vra + NOT(vrb) + 1).
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     |2/2 cycles|
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated a unsigned __int128.
 *  @param vrb 128-bit vector treated a unsigned __int128.
 *  @return __int128 carry from the unsigned difference vra - vrb.
 */
static inline vui128_t
vec_subcuq (vui128_t vra, vui128_t vrb)
{
  vui32_t t;
#ifdef _ARCH_PWR8
#ifndef vec_vsubcuq
  __asm__(
      "vsubcuq %0,%1,%2;"
      : "=v" (t)
      : "v" (vra),
      "v" (vrb)
      : );
#else
  t = (vui32_t) vec_vsubcuq (vra, vrb);
#endif
#else
  /* vsubcuq is defined as (vra + NOT(vrb) + 1) >> 128.  */
  vui32_t _b = vec_nor ((vui32_t)vrb, (vui32_t)vrb);
  const vui32_t ci= { 0,0,0,1 };

  t = (vui32_t)vec_addecuq (vra, (vui128_t)_b, (vui128_t)ci);
#endif
  return ((vui128_t) t);
}

/** \brief Vector Subtract Extended and Write Carry Unsigned Quadword.
 *
 *  Generate the carry-out of the sum (vra + NOT(vrb) + vrc.bit[127]).
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     |2/2 cycles|
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated a unsigned __int128.
 *  @param vrb 128-bit vector treated a unsigned __int128.
 *  @param vrc 128-bit vector carry-in from bit 127.
 *  @return __int128 carry from the extended __int128 difference.
 */
static inline vui128_t
vec_subecuq (vui128_t vra, vui128_t vrb, vui128_t vrc)
{
  vui32_t t;
#ifdef _ARCH_PWR8
#ifndef vec_vsubcuq
  __asm__(
      "vsubecuq %0,%1,%2,%3;"
      : "=v" (t)
      : "v" (vra),
	"v" (vrb),
        "v" (vrc)
      : );
#else
  t = (vui32_t) vec_vsubecuq (vra, vrb, vrc);
#endif
#else
  /* vsubcuq is defined as (vra + NOT(vrb) + vrc.bit[127]) >> 128.  */
  vui32_t _b = vec_nor ((vui32_t)vrb, (vui32_t)vrb);

  t = (vui32_t)vec_addecuq (vra, (vui128_t)_b, vrc);
#endif
  return ((vui128_t) t);
}

/** \brief Vector Subtract Extended Unsigned Quadword Modulo.
 *
 *  Subtract two vector __int128 values and return result modulo 128-bits.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     |2/2 cycles|
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated an unsigned __int128.
 *  @param vrb 128-bit vector treated an unsigned __int128.
 *  @param vrc 128-bit vector carry-in from bit 127.
 *  @return __int128 unsigned difference of vra minus vrb.
 */
static inline vui128_t
vec_subeuqm (vui128_t vra, vui128_t vrb, vui128_t vrc)
{
  vui32_t t;
#ifdef _ARCH_PWR8
#ifndef vec_vsubuqm
  __asm__(
      "vsubeuqm %0,%1,%2,%3;"
      : "=v" (t)
      : "v" (vra),
	"v" (vrb),
        "v" (vrc)
      : );
#else
  t = (vui32_t) vec_vsubeuqm (vra, vrb, vrc);
#endif
#else
  /* vsubeuqm is defined as vra + NOT(vrb) + vrc.bit[127].  */
  vui32_t _b = vec_nor ((vui32_t)vrb, (vui32_t)vrb);

  t = (vui32_t)vec_addeuqm (vra, (vui128_t)_b, vrc);
#endif
  return ((vui128_t) t);
}

/** \brief Vector subtract Unsigned Quadword Modulo.
 *
 *  Subtract two vector __int128 values and return result modulo 128-bits.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     |2/2 cycles|
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated an unsigned __int128.
 *  @param vrb 128-bit vector treated an unsigned __int128.
 *  @return __int128 unsigned difference of vra minus vrb.
 */
static inline vui128_t
vec_subuqm (vui128_t vra, vui128_t vrb)
{
  vui32_t t;
#ifdef _ARCH_PWR8
#ifndef vec_vsubuqm
  __asm__(
      "vsubuqm %0,%1,%2;"
      : "=v" (t)
      : "v" (vra),
      "v" (vrb)
      : );
#else
  t = (vui32_t) vec_vsubuqm (vra, vrb);
#endif
#else
  /* vsubuqm is defined as vra + NOT(vrb) + 1.  */
  vui32_t _b = vec_nor ((vui32_t)vrb, (vui32_t)vrb);
  const vui32_t ci= { 0,0,0,1 };

  t = (vui32_t)vec_addeuqm (vra, (vui128_t)_b, (vui128_t)ci);
#endif
  return ((vui128_t) t);
}
#endif /* VEC_INT128_PPC_H_ */

