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

 vec_char_ppc.h

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Jul 2, 2015
      Steven Munroe, additional contributions for POWER9.
 */

#ifndef VEC_CHAR_PPC_H_
#define VEC_CHAR_PPC_H_

#include <pveclib/vec_common_ppc.h>

/*!
 * \file  vec_char_ppc.h
 * \brief Header package containing a collection of 128-bit SIMD
 * operations over 8-bit integer (char) elements.
 *
 * Most of these operations are implemented in a single VMX or VSX
 * instruction on newer (POWER6/POWER7/POWER8/POWER9) processors.
 * This header serves to fill in functional gaps for older
 * (POWER7, POWER8) processors and provides in-line assembler
 * implementations for older compilers that do not
 * provide the build-ins.
 *
 * Most vector char (8-bit integer) operations are are already covered
 * by the original VMX (AKA Altivec) instructions. VMX intrinsic
 * (compiler built-ins) operations are defined in <altivec.h> and
 * described in the compiler documentation.
 * PowerISA 2.07B (POWER8) added several useful byte operations
 * (count leading zeros, population count) not included in the
 * original VMX.
 * PowerISA 3.0B (POWER9) adds several more (absolute difference,
 * compare not equal, count trailing zeros, extend sign,
 * extract/insert, and reverse bytes).
 * Most of these intrinsic (compiler built-ins) operations are defined
 * in <altivec.h> and described in the compiler documentation.
 *
 * \note The compiler disables associated <altivec.h> built-ins if the
 * <B>mcpu</B> target does not enable the specific instruction.
 * For example if you compile with <B>-mcpu=power7</B>, vec_vclz and
 * vec_vclzb will not be defined.  But vec_clzb is always defined in
 * this header, will generate the minimum code, appropriate for the
 * target, and produce correct results.
 *
 * This header covers operations that are either:
 *
 * - Implemented in later processors and useful to programmers
 * if the same operations are available on slightly older processors.
 * This is required even if the operation is defined in the OpenPOWER
 * ABI or <altivec.h>, as the compiler disables the associated
 * built-ins if the <B>mcpu</B> target does not enable the instruction.
 * - Defined in the OpenPOWER ABI but <I>not</I> yet defined in
 * <altivec.n> provided by available compilers in common use.
 * Examples include Count Leading Zeros and Population Count.
 * - Commonly used operations, not covered by the ABI or
 * <altivec.h>, and require multiple instructions or
 * are not obvious.  Examples include the multiply high,
 * ASCII character tests, and shift immediate operations.
 *
 * \section i8_endian_issues_0_0 Endian problems with byte operations
 *
 * It would be useful to provide a vector multiply high byte
 * (return the high order 8-bits of the 16-bit product) operation.
 * This can be used for multiplicative inverse (effectively integer
 * divide) operations.  Neither integer multiply high nor divide
 * are available as vector instructions.  However the multiply high
 * byte operation can be composed from the existing multiply
 * even/odd byte operations followed by the vector merge even
 * byte operation.
 * Similarly a multiply low (modulo) byte operation can be
 * composed from the existing multiply even/odd byte
 * operations followed by the vector merge odd byte operation.
 *
 * As a prerequisite we need to provide the merge even/odd byte
 * operations.
 * While PowerISA has added these operations for word and doubleword,
 * instructions are not defined for byte and halfword.
 * Fortunately vector merge operations are
 * just a special case of vector permute.  So the vec_vmrgob() and
 * vec_vmrgeb() implementation can use vec_perm and appropriate
 * selection vectors to provide these merge operations.
 *
 * As described for other element sizes
 * this is complicated by <I>little-endian</I> (LE) support as
 * specified in the OpenPOWER ABI and as implemented in the compilers.
 * Little-endian changes the effective vector element numbering and
 * the location of even and odd elements.  This means that the vector
 * built-ins provided by altivec.h may not generate the instructions
 * you would expect.
 * \sa \ref i16_endian_issues_0_0
 * \sa \ref mainpage_endian_issues_1_1
 *
 * So this header defines endian independent byte operations
 * vec_vmrgeb() and vec_vmrgob().  These operations are used in the
 * implementation of the endian sensitive vec_mrgeb() and vec_mrgob().
 * These support the OpenPOWER ABI mandated merge even/odd semantic.
 *
 * We also provide the merge algebraic high/low operations vec_mrgahb()
 * and vec_mrgalb() to simplify extended precision arithmetic.
 * These implementations use vec_vmrgeb() and vec_vmrgob() as
 * extended precision byte order does not change with endian.
 * These operations are used in turn to implement multiply
 * byte high/low/modulo (vec_mulhsb(), vec_mulhub(), vec_mulubm()).
 *
 * These operations provide a basis for using the multiplicative
 * inverse as a alternative to integer divide.
 * \sa \ref int16_examples_0_1
 *
 * \section int8_perf_0_0 Performance data.
 *
 * The performance characteristics of the merge and multiply byte
 * operations are very similar to the halfword implementations.
 * (see \ref int16_perf_0_0).
 *
 * \subsection int8_perf_0_1 More information.
 * High level performance estimates are provided as an aid to function
 * selection when evaluating algorithms. For background on how
 * <I>Latency</I> and <I>Throughput</I> are derived see:
 * \ref perf_data
 *
 */

///@cond INTERNAL
#ifndef vec_popcntb
static inline vui8_t vec_popcntb (vui8_t vra);
#else
/* Work around for GCC PR85830.  */
#undef vec_popcntb
#define vec_popcntb __builtin_vec_vpopcntb
#endif
static inline vui8_t vec_vmrgeb (vui8_t vra, vui8_t vrb);
static inline vui8_t vec_vmrgob (vui8_t vra, vui8_t vrb);
///@endcond

/** \brief Vector Absolute Difference Unsigned byte.
 *
 * Compute the absolute difference for each byte.
 * For each unsigned byte, subtract B[i] from A[i] and return the
 * absolute value of the difference.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   4   | 1/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 * @param vra vector of 16 unsigned bytes
 * @param vrb vector of 16 unsigned bytes
 * @return vector of the absolute difference.
 */
static inline vui8_t
vec_absdub (vui8_t vra, vui8_t vrb)
{
  vui8_t result;
#ifdef _ARCH_PWR9
        __asm__(
            "vabsdub %0,%1,%2;"
            : "=v" (result)
            : "v" (vra), "v" (vrb)
            : );
#else
  vui8_t a, b;
  vui8_t vmin, vmax;

  a = (vui8_t) vra;
  b = (vui8_t) vrb;
  vmin = vec_min (a, b);
  vmax = vec_max (a, b);
  result = vec_sub (vmax, vmin);
#endif
  return (result);
}

/** \brief Vector Count Leading Zeros Byte for a unsigned char (byte)
 *  elements.
 *
 *  Count the number of leading '0' bits (0-7) within each byte
 *  element of a 128-bit vector.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the Vector Count Leading
 *  Zeros Byte instruction <B>vclzb</B>. Otherwise use sequence of pre
 *  2.07 VMX instructions.
 *  SIMDized count leading zeros inspired by:
 *
 *  Warren, Henry S. Jr and <I>Hacker's Delight</I>, 2nd Edition,
 *  Addison Wesley, 2013. Chapter 5 Counting Bits, Figure 5-12.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as 16 x 8-bit unsigned integer
 *  (byte) elements.
 *  @return 128-bit vector with the leading zeros count for each
 *  byte element.
 */
static inline vui8_t
vec_clzb (vui8_t vra)
{
  vui8_t r;
#ifdef _ARCH_PWR8
#if defined (vec_vclzb)
  r = vec_vclzb (vra);
#elif defined (__clang__)
  r = vec_cntlz (vra);
#else
  __asm__(
      "vclzb %0,%1;"
      : "=v" (r)
      : "v" (vra)
      : );
#endif
#else
//#warning Implememention pre power8
  __vector unsigned char n, nt, y, x, s, m;
  __vector unsigned char z= { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
  __vector unsigned char one = { 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1};

  /* n = 8 s = 4 */
  s = vec_splat_u8(4);
  n = vec_splat_u8(8);
  x = vra;

  /* y=x>>4 if (y!=0) (n=n-4 x=y)  */
  y = vec_sr(x, s);
  nt = vec_sub(n,s);
  m = (__vector unsigned char)vec_cmpgt(y, z);
  s = vec_sr(s,one);
  x = vec_sel (x , y, m);
  n = vec_sel (n , nt, m);

  /* y=x>>2 if (y!=0) (n=n-2 x=y)  */
  y = vec_sr(x, s);
  nt = vec_sub(n,s);
  m = (__vector unsigned char)vec_cmpgt(y, z);
  s = vec_sr(s,one);
  x = vec_sel (x , y, m);
  n = vec_sel (n , nt, m);

  /* y=x>>1 if (y!=0) return (n=n-2)   */
  y = vec_sr(x, s);
  nt = vec_sub(n,s);
  nt = vec_sub(nt,s);
  m = (__vector unsigned char)vec_cmpgt(y, z);
  n = vec_sel (n , nt, m);

  /* else return (x-n)  */
  nt = vec_sub (n, x);
  n = vec_sel (nt , n, m);
  r = n;
#endif

  return (r);
}

/** \brief Vector Count Trailing Zeros Byte for a unsigned char (byte)
 *  elements.
 *
 *  Count the number of trailing '0' bits (0-8) within each byte
 *  element of a 128-bit vector.
 *
 *  For POWER9 (PowerISA 3.0B) or later use the Vector Count Trailing
 *  Zeros Byte instruction <B>vctzb</B>. Otherwise use a sequence of
 *  pre ISA 3.0 VMX instructions.
 *  SIMDized count trailing zeros inspired by:
 *
 *  Warren, Henry S. Jr and <I>Hacker's Delight</I>, 2nd Edition,
 *  Addison Wesley, 2013. Chapter 5 Counting Bits, Section 5-4.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  6-8  | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as 16 x 8-bit unsigned char
 *  (byte) elements.
 *  @return 128-bit vector with the trailing zeros count for each
 *  byte element.
 */
static inline vui8_t
vec_ctzb (vui8_t vra)
{
  vui8_t r;
#ifdef _ARCH_PWR9
#if defined (vec_cnttz) || defined (__clang__)
  r = vec_cnttz (vra);
#else
  __asm__(
      "vctzb %0,%1;"
      : "=v" (r)
      : "v" (vra)
      : );
#endif
#elif _ARCH_PWR8
// For _ARCH_PWR8. Generate 1's for the trailing zeros
// and 0's otherwise. Then count (popcnt) the 1's.
// _ARCH_PWR8 uses the hardware vpopcntb instruction.
  const vui8_t ones = vec_splat_u8 (1);
  vui8_t tzmask;
  // tzmask = (!vra & (vra - 1))
  tzmask = vec_andc (vec_sub (vra, ones), vra);
  // return = vec_popcnt (!vra & (vra - 1))
  r = vec_popcntb (tzmask);
#else
  // For _ARCH_PWR7 and earlier (without hardware clz or popcnt).
  // Generate 1's for the trailing zeros and 0's otherwise.
  // Then count leading 0's using the PVECLIB vec_clzb implementation
  // which minimizes the number of constant loads (vs popcntb).
  // Finally subtract this count from 8.
  const vui8_t ones = vec_splat_u8 (1);
  const vui8_t c8s = vec_splat_u8 (8);
  vui8_t term;
  // term = (!vra & (vra - 1))
  term = vec_andc (vec_sub (vra, ones), vra);
  // return = 8 - vec_clz (!vra & (vra - 1))
  return vec_sub (c8s, vec_clzb (term));
#endif
  return ((vui8_t) r);
}

/** \brief Vector isalpha.
 *
 * Return a vector boolean char with a true indicator for any character
 * that is either Lower Case Alpha ASCII or Upper Case ASCII.
 * False otherwise.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-20 | 1/cycle  |
 *  |power9   | 11-21 | 1/cycle  |
 *
 * @param vec_str vector of 16 ASCII characters
 * @return vector bool char of the isalpha operation applied to each
 * character of vec_str. For each byte 0xff indicates true (isalpha),
 * 0x00 indicates false.
 */
static inline vui8_t
vec_isalnum (vui8_t vec_str)
{
  vui8_t result;
  const vui8_t UC_FIRST =
    { 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x40 };
  const vui8_t UC_LAST =
    { 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a,
	0x5a, 0x5a, 0x5a, 0x5a };
  const vui8_t LC_FIRST =
    { 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60,
	0x60, 0x60, 0x60, 0x60 };
  const vui8_t LC_LAST =
    { 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a,
	0x7a, 0x7a, 0x7a, 0x7a };
  const vui8_t DG_FIRST =
    { 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
	0x2f, 0x2f, 0x2f, 0x2f };
  const vui8_t DG_LAST =
    { 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x5a,
	0x39, 0x39, 0x39, 0x39 };

  vui8_t cmp1, cmp2, cmp3, cmp4, cmp5, cmp6, cmask1, cmask2, cmask3;

  cmp1 = (vui8_t) vec_cmpgt (vec_str, LC_FIRST);
  cmp2 = (vui8_t) vec_cmpgt (vec_str, LC_LAST);

  cmp3 = (vui8_t) vec_cmpgt (vec_str, UC_FIRST);
  cmp4 = (vui8_t) vec_cmpgt (vec_str, UC_LAST);

  cmp5 = (vui8_t) vec_cmpgt (vec_str, DG_FIRST);
  cmp6 = (vui8_t) vec_cmpgt (vec_str, DG_LAST);

  cmask1 = vec_andc (cmp1, cmp2);
  cmask2 = vec_andc (cmp3, cmp4);
  cmask3 = vec_andc (cmp5, cmp6);

  result = vec_or (vec_or (cmask1, cmask2), cmask3);

  return (result);
}

/** \brief Vector isalnum.
 *
 * Return a vector boolean char with a true indicator for any character
 * that is either Lower Case Alpha ASCII, Upper Case ASCII, or numeric ASCII.
 * False otherwise.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 9-18  | 1/cycle  |
 *  |power9   | 10-19 | 1/cycle  |
 *
 * @param vec_str vector of 16 ASCII characters
 * @return vector bool char of the isalnum operation applied to each
 * character of vec_str. For each byte 0xff indicates true (isalpha),
 * 0x00 indicates false.
 */
static inline vui8_t
vec_isalpha (vui8_t vec_str)
{
  vui8_t result;
  const vui8_t UC_FIRST =
    { 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x40 };
  const vui8_t UC_LAST =
    { 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a,
	0x5a, 0x5a, 0x5a, 0x5a };
  const vui8_t LC_FIRST =
    { 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60,
	0x60, 0x60, 0x60, 0x60 };
  const vui8_t LC_LAST =
    { 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a,
	0x7a, 0x7a, 0x7a, 0x7a };

  vui8_t cmp1, cmp2, cmp3, cmp4, cmask1, cmask2;

  cmp1 = (vui8_t) vec_cmpgt (vec_str, LC_FIRST);
  cmp2 = (vui8_t) vec_cmpgt (vec_str, LC_LAST);

  cmp3 = (vui8_t) vec_cmpgt (vec_str, UC_FIRST);
  cmp4 = (vui8_t) vec_cmpgt (vec_str, UC_LAST);

  cmask1 = vec_andc (cmp1, cmp2);
  cmask2 = vec_andc (cmp3, cmp4);

  result = vec_or (cmask1, cmask2);

  return (result);
}

/** \brief Vector isdigit.
 *
 * Return a vector boolean char with a true indicator for any character
 * that is ASCII decimal digit.
 * False otherwise.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-13  | 1/cycle  |
 *  |power9   | 5-14  | 1/cycle  |
 *
 * @param vec_str vector of 16 ASCII characters
 * @return vector bool char of the isdigit operation applied to each
 * character of vec_str. For each byte 0xff indicates true (isdigit),
 * 0x00 indicates false.
 */
static inline vui8_t
vec_isdigit (vui8_t vec_str)
{
  vui8_t result;
  const vui8_t DG_FIRST =
    { 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
	0x2f, 0x2f, 0x2f, 0x2f };
  const vui8_t DG_LAST =
    { 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x5a,
	0x39, 0x39, 0x39, 0x39 };

  vui8_t cmp1, cmp2;

  cmp1 = (vui8_t) vec_cmpgt (vec_str, DG_FIRST);
  cmp2 = (vui8_t) vec_cmpgt (vec_str, DG_LAST);

  result = vec_andc (cmp1, cmp2);

  return (result);
}

/** \brief Vector Merge Algebraic High Byte operation.
 *
 * Merge only the high byte from 16 x Algebraic halfwords
 * across vectors vra and vrb. This is effectively the Vector Merge
 * Even Byte operation that is not modified for Endian.
 *
 * For example merge the high 8-bits from each of 16 x 16-bit products
 * as generated by vec_muleub/vec_muloub. This result is effectively
 * a vector multiply high unsigned byte.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-13  | 2/cycle  |
 *  |power9   | 3-14  | 2/cycle  |
 *
 * @param vra 128-bit vector unsigned short.
 * @param vrb 128-bit vector unsigned short.
 * @return A vector merge from only the high bytes of the 16 x
 * Algebraic halfwords across vra and vrb.
 */
static inline vui8_t
vec_mrgahb  (vui16_t vra, vui16_t vrb)
{
  return vec_vmrgeb ((vui8_t) vra, (vui8_t) vrb);
}

/** \brief Vector Merge Algebraic Low Byte operation.
 *
 * Merge only the low bytes from 16 x Algebraic halfwords
 * across vectors vra and vrb. This is effectively the Vector Merge
 * Odd Bytes operation that is not modified for Endian.
 *
 * For example merge the low 8-bits from each of 16 x 16-bit products
 * as generated by vec_muleub/vec_muloub. This result is effectively
 * a vector multiply low unsigned byte.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-13  | 2/cycle  |
 *  |power9   | 3-14  | 2/cycle  |
 *
 * @param vra 128-bit vector unsigned int.
 * @param vrb 128-bit vector unsigned int.
 * @return A vector merge from only the high halfwords of the 8 x
 * Algebraic words across vra and vrb.
 */
static inline vui8_t
vec_mrgalb  (vui16_t vra, vui16_t vrb)
{
  return vec_vmrgob ((vui8_t) vra, (vui8_t) vrb);
}

/** \brief Vector Merge Even Bytes operation.
 *
 * Merge the even byte elements from the concatenation of 2 x
 * vectors (vra and vrb).
 *
 * \note The element numbering changes between Big and Little Endian.
 * So the compiler and this implementation adjusts the generated code
 * to reflect this.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-13  | 2/cycle  |
 *  |power9   | 3-14  | 2/cycle  |
 *
 * @param vra 128-bit vector unsigned char.
 * @param vrb 128-bit vector unsigned char.
 * @return A vector merge from only the even bytes of vra and vrb.
 */
static inline vui8_t
vec_mrgeb  (vui8_t vra, vui8_t vrb)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return vec_vmrgob ((vui8_t) vrb, (vui8_t) vra);
#else
  return vec_vmrgeb ((vui8_t) vra, (vui8_t) vrb);
#endif
}

/** \brief Vector Merge Odd Halfwords operation.
 *
 * Merge the odd halfword elements from the concatenation of 2 x
 * vectors (vra and vrb).
 *
 * \note The element numbering changes between Big and Little Endian.
 * So the compiler and this implementation adjusts the generated code
 * to reflect this.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-13  | 2/cycle  |
 *  |power9   | 3-14  | 2/cycle  |
 *
 * @param vra 128-bit vector unsigned char.
 * @param vrb 128-bit vector unsigned char.
 * @return A vector merge from only the odd bytes of vra and vrb.
 */
static inline vui8_t
vec_mrgob  (vui8_t vra, vui8_t vrb)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return vec_vmrgeb ((vui8_t) vrb, (vui8_t) vra);
#else
  return vec_vmrgob ((vui8_t) vra, (vui8_t) vrb);
#endif
}

/** \brief Vector Multiply High Signed Bytes.
 *
 *  Multiple the corresponding byte elements of two vector signed
 *  char values and return the high order 8-bits, for each
 *  16-bit product element.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 9-13  | 1/cycle  |
 *  |power9   | 10-14 | 1/cycle  |
 *
 *  @param vra 128-bit vector signed char.
 *  @param vrb 128-bit vector signed char.
 *  @return vector of the high order 8-bits of the product of the
 *  byte elements from vra and vrb.
 */
static inline vi8_t
vec_mulhsb (vi8_t vra, vi8_t vrb)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return (vi8_t) vec_mrgahb ((vui16_t)vec_mulo (vra, vrb),
			     (vui16_t)vec_mule (vra, vrb));
#else
  return (vi8_t) vec_mrgahb ((vui16_t)vec_mule (vra, vrb),
			     (vui16_t)vec_mulo (vra, vrb));
#endif
}

/** \brief Vector Multiply High Unsigned Bytes.
 *
 *  Multiple the corresponding byte elements of two vector unsigned
 *  char values and return the high order 8-bits, for each
 *  16-bit product element.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 9-13  | 1/cycle  |
 *  |power9   | 10-14 | 1/cycle  |
 *
 *  @param vra 128-bit vector unsigned char.
 *  @param vrb 128-bit vector unsigned char.
 *  @return vector of the high order 8-bits of the product of the
 *  byte elements from vra and vrb.
 */
static inline vui8_t
vec_mulhub (vui8_t vra, vui8_t vrb)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return vec_mrgahb (vec_mulo (vra, vrb), vec_mule (vra, vrb));
#else
  return vec_mrgahb (vec_mule (vra, vrb), vec_mulo (vra, vrb));
#endif
}

/** \brief Vector Multiply Unsigned Byte Modulo.
 *
 *  Multiple the corresponding byte elements of two vector unsigned
 *  char values and return the low order 8-bits of the 16-bit product
 *  for each element.
 *
 *  \note vec_mulubm can be used for unsigned or signed char integers.
 *  It is the vector equivalent of Multiply Low Byte.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 9-13  | 1/cycle  |
 *  |power9   | 10-14 | 1/cycle  |
 *
 *  @param vra 128-bit vector unsigned char.
 *  @param vrb 128-bit vector unsigned char.
 *  @return vector of the low order 8-bits of the unsigned product
 *  of the byte elements from vra and vrb.
 */
static inline vui8_t
vec_mulubm (vui8_t vra, vui8_t vrb)
{
#if __GNUC__ >= 7
/* Generic vec_mul not supported for vector char until GCC 7.  */
  return vec_mul (vra, vrb);
#else
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return vec_mrgalb (vec_mulo (vra, vrb), vec_mule (vra, vrb));
#else
  return vec_mrgalb (vec_mule (vra, vrb), vec_mulo (vra, vrb));
#endif
#endif
}

/** \brief Vector Population Count byte.
 *
 *  Count the number of '1' bits (0-8) within each byte element of
 *  a 128-bit vector.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the Vector Population
 *  Count Byte instruction. Otherwise use simple Vector (VMX)
 *  instructions to count bits in bytes in parallel.
 *  SIMDized population count inspired by:
 *
 *  Warren, Henry S. Jr and <I>Hacker's Delight</I>, 2nd Edition,
 *  Addison Wesley, 2013. Chapter 5 Counting Bits, Figure 5-2.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as 16 x 8-bit integers (byte)
 *  elements.
 *  @return 128-bit vector with the population count for each byte
 *  element.
 */
#ifndef vec_popcntb
static inline vui8_t
vec_popcntb (vui8_t vra)
{
  vui8_t r;
#ifdef _ARCH_PWR8
#if defined (vec_vpopcntb)
  r = vec_vpopcntb (vra);
#elif defined (__clang__)
  r = vec_popcnt (vra);
#else
  __asm__(
      "vpopcntb %0,%1;"
      : "=v" (r)
      : "v" (vra)
      : );
#endif
#else
//#warning Implememention pre power8
  vui8_t n, x1, x2, x, s;
  vui8_t ones = { 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1};
  vui8_t fives =
      {0x55,0x55,0x55,0x55, 0x55,0x55,0x55,0x55,
	  0x55,0x55,0x55,0x55, 0x55,0x55,0x55,0x55};
  vui8_t threes =
      {0x33,0x33,0x33,0x33, 0x33,0x33,0x33,0x33,
	  0x33,0x33,0x33,0x33, 0x33,0x33,0x33,0x33};
  vui8_t fs =
      {0x0f,0x0f,0x0f,0x0f, 0x0f,0x0f,0x0f,0x0f,
	  0x0f,0x0f,0x0f,0x0f, 0x0f,0x0f,0x0f,0x0f};
  /* n = 8 s = 4 */
  s = ones;
  x = vra;
  /* x = x - ((x >> 1) & 0x55)  */
  x2 = vec_and (vec_sr (x, s), fives);
  n = vec_sub (x, x2);
  s = vec_add (s, s);
  /* x = (x & 0x33) + ((x & 0xcc) >> 2)  */
  x1 = vec_and (n, threes);
  x2 = vec_andc (n, threes);
  n = vec_add (x1, vec_sr (x2, s));
  s = vec_add (s, s);
  /* x = (x + (x >> 4)) & 0x0f)  */
  x1 = vec_add (n, vec_sr (n, s));
  n  = vec_and (x1, fs);
  r = n;
#endif
  return (r);
}
#else
/* Work around for GCC PR85830.  */
#undef vec_popcntb
#define vec_popcntb __builtin_vec_vpopcntb
#endif

/*! \brief Vector Set Bool from Signed Byte.
 *
 *  For each byte, propagate the sign bit, to all 8-bits of that
 *  byte. The result is vector bool char reflecting the sign
 *  bit of each 8-bit byte.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-4   | 2/cycle  |
 *  |power9   | 2-5   | 2/cycle  |
 *
 *  @param vra Vector signed char.
 *  @return vector bool char reflecting the sign bit of each
 *  byte.
 */

static inline vb8_t
vec_setb_sb (vi8_t vra)
{
  vb8_t result;

#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  __asm__(
      "vexpandbm %0,%1;\n"
      : "=v" (result)
      : "v" (vra)
      : );
#else
  const vui8_t rshift =  vec_splat_u8( 7 );
  // Vector Shift Right Algebraic Bytes 7-bits.
  result = (vb8_t) vec_sra (vra, rshift);
#endif
  return result;
}

/** \brief Vector Shift left Byte Immediate.
 *
 *  Shift left each byte element [0-15], 0-7 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-7.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 7 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-11  | 2/cycle  |
 *  |power9   | 5-11  | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned char.
 *  @param shb Shift amount in the range 0-7.
 *  @return 128-bit vector unsigned char, shifted left shb bits.
 */
static inline vui8_t
vec_slbi (vui8_t vra, const unsigned int shb)
{
  vui8_t lshift;
  vui8_t result;

  if (shb < 8)
    {
      /* Load the shift const in a vector.  The element shifts require
         a shift amount for each element. For the immediate form the
         shift constant is splatted to all elements of the
         shift control.  */
      if (__builtin_constant_p(shb))
	lshift = (vui8_t) vec_splat_s8(shb);
      else
	lshift = vec_splats ((unsigned char) shb);

      /* Vector Shift right bytes based on the lower 3-bits of
         corresponding element of lshift.  */
      result = vec_vslb (vra, lshift);
    }
  else
    { /* shifts greater then 7 bits return zeros.  */
      result = vec_xor ((vui8_t) vra, (vui8_t) vra);
    }

  return (vui8_t) result;
}

/** \brief Vector Shift Right Algebraic Byte Immediate.
 *
 *  Shift right each byte element [0-15], 0-7 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-7.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 7 bits return the sign bit
 *  propagated to each bit of each element.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-11  | 2/cycle  |
 *  |power9   | 5-11  | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector signed char.
 *  @param shb Shift amount in the range 0-7.
 *  @return 128-bit vector signed char, shifted right shb bits.
 */
static inline vi8_t
vec_srabi (vi8_t vra, const unsigned int shb)
{
  vui8_t lshift;
  vi8_t result;

  if (shb < 8)
    {
      /* Load the shift const in a vector.  The element shifts require
         a shift amount for each element. For the immediate form the
         shift constant is splatted to all elements of the
         shift control.  */
      if (__builtin_constant_p(shb))
	lshift = (vui8_t) vec_splat_s8(shb);
      else
	lshift = vec_splats ((unsigned char) shb);

      /* Vector Shift Right Algebraic Bytes based on the lower 3-bits
         of corresponding element of lshift.  */
      result = vec_vsrab (vra, lshift);
    }
  else
    { /* shifts greater then 7 bits returns the sign bit propagated to
         all bits.   This is equivalent to shift Right Algebraic of
         7 bits.  */
      lshift = (vui8_t) vec_splat_s8(7);
      result = vec_vsrab (vra, lshift);
    }

  return (vi8_t) result;
}

/** \brief Vector Shift Right Byte Immediate.
 *
 *  Shift right each byte element [0-15], 0-7 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-7.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 7 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-11  | 2/cycle  |
 *  |power9   | 5-11  | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned char.
 *  @param shb Shift amount in the range 0-7.
 *  @return 128-bit vector unsigned char, shifted right shb bits.
 */
static inline vui8_t
vec_srbi (vui8_t vra, const unsigned int shb)
{
  vui8_t lshift;
  vui8_t result;

  if (shb < 8)
    {
      /* Load the shift const in a vector.  The element shifts require
         a shift amount for each element. For the immediate form the
         shift constant is splatted to all elements of the
         shift control.  */
      if (__builtin_constant_p(shb))
	lshift = (vui8_t) vec_splat_s8(shb);
      else
	lshift = vec_splats ((unsigned char) shb);

      /* Vector Shift right bytes based on the lower 3-bits of
         corresponding element of lshift.  */
      result = vec_vsrb (vra, lshift);
    }
  else
    { /* shifts greater then 7 bits return zeros.  */
      result = vec_xor ((vui8_t) vra, (vui8_t) vra);
    }

  return (vui8_t) result;
}

/** \brief Shift left double quadword by octet.
 * Return a vector unsigned char that is the left most 16 chars after
 * shifting left 0-15 octets (chars) of the 32 char double vector
 * (vrw||vrx).  The octet shift amount is from bits 121:124 of vrb.
 *
 * This sequence can be used to align a unaligned 16 char substring
 * based on the result of a vector count leading zero of of the compare
 * boolean.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  6-8  | 1/cycle  |
 *  |power9   |  8-9  | 1/cycle  |
 *
 * @param vrw upper 16-bytes of the 32-byte double vector.
 * @param vrx lower 16-bytes of the 32-byte double vector.
 * @param vrb Shift amount in bits 121:124.
 * @return upper 16-bytes of left shifted double vector.
 */
static inline vui8_t
vec_shift_leftdo (vui8_t vrw, vui8_t vrx, vui8_t vrb)
{
	vui8_t result, vt1, vt2, vt3;
	const vui8_t vzero = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	vt1 = vec_slo (vrw, vrb);
	vt3 = vec_sub (vzero, vrb);
	vt2 = vec_sro (vrx, vt3);
	result = vec_or (vt1, vt2);

	return (result);
}

/** \brief Vector toupper.
 *
 *  Convert any Lower Case Alpha ASCII characters within a vector
 *  unsigned char into the equivalent Upper Case character.
 *  Return the result as a vector unsigned char.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8-17  | 1/cycle  |
 *  |power9   | 9-18  | 1/cycle  |
 *
 *  @param vec_str vector of 16 ASCII characters
 *  @return vector char converted to upper case.
 */
static inline vui8_t
vec_toupper (vui8_t vec_str)
{
  vui8_t result;
  const vui8_t UC_MASK =
    { 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20 };
  const vui8_t LC_FIRST =
    { 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60,
	0x60, 0x60, 0x60, 0x60 };
  const vui8_t LC_LAST =
    { 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a,
	0x7a, 0x7a, 0x7a, 0x7a };

  vui8_t cmp1, cmp2, cmask;

  cmp1 = (vui8_t) vec_cmpgt (vec_str, LC_FIRST);
  cmp2 = (vui8_t) vec_cmpgt (vec_str, LC_LAST);

  cmask = vec_andc (cmp1, cmp2);
  cmask = vec_and (cmask, UC_MASK);

  result = vec_andc (vec_str, cmask);

  return (result);
}

/** \brief Vector tolower.
 *
 *  Convert any Upper Case Alpha ASCII characters within a vector
 *  unsigned char into the equivalent Lower Case character.
 *  Return the result as a vector unsigned char.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8-17  | 1/cycle  |
 *  |power9   | 9-18  | 1/cycle  |
 *
 *  @param vec_str vector of 16 ASCII characters
 *  @return vector char converted to lower case.
 */
static inline vui8_t
vec_tolower (vui8_t vec_str)
{
  vui8_t result;
  const vui8_t UC_MASK =
    { 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20 };
  const vui8_t UC_FIRST =
    { 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x40 };
  const vui8_t UC_LAST =
    { 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a,
	0x5a, 0x5a, 0x5a, 0x5a };

  vui8_t cmp1, cmp2, cmask;

  cmp1 = (vui8_t) vec_cmpgt (vec_str, UC_FIRST);
  cmp2 = (vui8_t) vec_cmpgt (vec_str, UC_LAST);

  cmask = vec_andc (cmp1, cmp2);
  cmask = vec_and (cmask, UC_MASK);

  result = vec_or (vec_str, cmask);

  return (result);
}

/** \brief Vector Merge Even Bytes.
 *
 * Merge the even byte elements from the concatenation of 2 x
 * vectors (vra and vrb).
 *
 * \note This function implements the operation of a Vector Merge Even
 * Bytes instruction, if the PowerISA included such an instruction.
 * This implementation is NOT Endian sensitive and the function is
 * stable across BE/LE implementations. Using Big Endian element
 * numbering:
 * - res[0] = vra[0];
 * - res[1] = vrb[0];
 * - res[2] = vra[2];
 * - res[3] = vrb[2];
 * - res[4] = vra[4];
 * - res[5] = vrb[4];
 * - res[6] = vra[6];
 * - res[7] = vrb[6];
 * - res[8] = vra[8];
 * - res[9] = vrb[8];
 * - res[10] = vra[10];
 * - res[11] = vrb[10];
 * - res[12] = vra[12];
 * - res[13] = vrb[12];
 * - res[14] = vra[14];
 * - res[15] = vrb[14];
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-13  | 2/cycle  |
 *  |power9   | 3-14  | 2/cycle  |
 *
 * @param vra 128-bit vector unsigned char.
 * @param vrb 128-bit vector unsigned char.
 * @return A vector merge from only the even bytes of vra and vrb.
 */
static inline vui8_t
vec_vmrgeb (vui8_t vra, vui8_t vrb)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  const vui8_t permute =
    { 0x01, 0x11, 0x03, 0x13, 0x05, 0x15, 0x07, 0x17, 0x09, 0x19, 0x0B, 0x1B,
	0x0D, 0x1D, 0x0F, 0x1F };

  return vec_perm (vrb, vra, (vui8_t) permute);
#else
  const vui8_t permute =
    { 0x00, 0x10, 0x02, 0x12, 0x04, 0x14, 0x06, 0x16, 0x08, 0x18, 0x0A, 0x1A,
	0x0C, 0x1C, 0x0E, 0x1E};

  return vec_perm (vra, vrb, (vui8_t)permute);
#endif
}

/** \brief Vector Merge Odd Byte.
 *
 * Merge the odd byte elements from the concatenation of 2 x
 * vectors (vra and vrb).
 *
 * \note This function implements the operation of a Vector Merge Odd
 * Bytes instruction, if the PowerISA included such an instruction.
 * This implementation is NOT Endian sensitive and the function is
 * stable across BE/LE implementations. Using Big Endian element
 * numbering:
 * - res[0] = vra[1];
 * - res[1] = vrb[1];
 * - res[2] = vra[3];
 * - res[3] = vrb[3];
 * - res[4] = vra[5];
 * - res[5] = vrb[5];
 * - res[6] = vra[7];
 * - res[7] = vrb[7];
 * - res[8] = vra[9];
 * - res[9] = vrb[9];
 * - res[10] = vra[11];
 * - res[11] = vrb[11];
 * - res[12] = vra[13];
 * - res[13] = vrb[13];
 * - res[14] = vra[15];
 * - res[15] = vrb[15];
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-13  | 2/cycle  |
 *  |power9   | 3-14  | 2/cycle  |
 *
 * @param vra 128-bit vector unsigned char.
 * @param vrb 128-bit vector unsigned char.
 * @return A vector merge from only the odd bytes of vra and vrb.
 */
static inline vui8_t
vec_vmrgob (vui8_t vra, vui8_t vrb)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  const vui8_t permute =
      { 0x00, 0x10, 0x02, 0x12, 0x04, 0x14, 0x06, 0x16, 0x08, 0x18, 0x0A, 0x1A,
  	0x0C, 0x1C, 0x0E, 0x1E};
  return vec_perm (vrb, vra, (vui8_t)permute);
#else
  const vui8_t permute =
      { 0x01, 0x11, 0x03, 0x13, 0x05, 0x15, 0x07, 0x17, 0x09, 0x19, 0x0B, 0x1B,
  	0x0D, 0x1D, 0x0F, 0x1F };
  return vec_perm (vra, vrb, (vui8_t)permute);
#endif
}

#endif /* VEC_CHAR_PPC_H_ */
