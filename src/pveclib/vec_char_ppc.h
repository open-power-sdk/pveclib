/*
 Copyright (c) [2017, 2018, 2024] IBM Corporation.

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
 * PowerISA 3.1C (POWER10) adds several more (blend, centrifuge,
 * clear first, clear last, count mask, count under mask,
 * expand mask, extract, extract mask, generate permute control,
 * insert, permute extended, string isolate)
 * Most of these intrinsic (compiler built-ins) operations are defined
 * in <altivec.h> and described in the compiler documentation.
 * \note The list of new (byte/char) operations for POWER9/10 is long.
 * Might take awhile to fill in the matching POWER8/9 implementations.
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
 * \section int8_P9_10_0_0 Implementations for POWER9/10 Intrinsics.
 *
 * PowerISA 3.0/3.1 (POWER9/10) added several useful character/byte
 * element vector instructions that are also specified in the
 * Power Vector Intrinsic Programming Reference.
 * In the spirit of PVECLIB equivalent operations should be provided as
 * in-line functions for both;
 * - older compilers that support the assembler instruction but not the
 *   intrinsic for POWER9/10 targets.
 * - functionally equivalent vector operations for POWER8 and earlier.
 *
 * Some examples follow.
 *
 * \subsection int8_P9_10_0_0_1 Vector Compare Not Equal or Zero Byte.
 *
 * This is one of the new vector string operations added in POWER9.
 * The intent is to accelerate comparing nul terminated strings of
 * up to 16 characters. The result is a vector bool char for i=0 to 15;
 * (src1[i] = 0) | (src2[i] = 0) | (src1[i] != src2[i]).
 *
 * The first section of code addresses older compilers that support
 * the -mcpu=power9 target but may not support the vec_cmpnez()
 * intrinsic. For example:
 * \code
vb8_t
test_vcmpnezb (vui8_t vra, vui8_t vrb)
{
  vb8_t result;
#ifdef _ARCH_PWR9
#ifdef vec_cmpnez
  result = vec_cmpnez (vra, vrb);
#else
  __asm__(
      "vcmpnezb %0,%1,%2;"
      : "=v" (result)
      : "v" (vra), "v" (vrb)
      : );
#endif
#else  // _ARCH_PWR8 and earlier
 ...
#endif
  return result;
}
 * \endcode
 *
 * For the mcpu=power8 and earlier targets we use an implementation
 * restricted to the original Altivec<SUP>tm</SUP> intrinsics
 * plus one POWER8 VMX extension (vector OR complement).
 * This insures compatibility across compilers and will run on
 * POWER7/8 processors.
 *
 * The logic requires a <I>not equal</I> compare which is not available
 * as an instruction or intrinsic until POWER9. So we have to use an
 * equal compare with a separate <I>not</I> or <I>compliment</I>
 * operation feeding the final <I>or</I>. For example:
 * \code
#else  // _ARCH_PWR8 and earlier
  const vui8_t VEOS = vec_splat_u8(0);
  vb8_t eosa, eosb, eosc, abne;
  // vcmpneb requires _ARCH_PWR9, so use cmpeq and orc
  abne = vec_cmpeq (vra, vrb);
  eosa = vec_cmpeq (vra, VEOS);
  eosb = vec_cmpeq (vrb, VEOS);
  eosc = vec_or (eosa, eosb);
  result = vec_orc (eosc, abne);
#endif
 * \endcode
 *
 * The generated code should look like this:
 * \code
 <test_vec_vcmpnezb>:
      vspltisw v0,0
      vcmpequb v1,v2,v0
      vcmpequb v0,v3,v0
      vcmpequb v2,v2,v3
      vor      v0,v1,v0
      vorc     v2,v0,v2
 * \endcode
 * The vspltisw is generated for the VEOS constant and can be shared
 * across invocations or hoisted out of loops or into the containing
 * functions prologue.
 *
 * For processors before POWER8 using the Vector OR complement
 * extension is a problem. The original Altivec<SUP>tm</SUP> only
 * supports vec_cmpeq, vec_nor and vec_or. The follow modification
 * extends this operation to POWER6/7 and 970 based processors.
 * \code
#ifdef _ARCH_PWR8
  result = vec_orc (eosc, abne);
#else // vorc requires _ARCH_PWR8, so use cmpeq, nor and or
  abne = vec_nor (abne, abne);
  result = vec_or (eosc, abne);
#endif
 * \endcode
 *
 * \subsection int8_P9_10_0_0_2 Vector Count Leading/Trailing Zero LSBB etc.
 *
 * Vector Count Leading Zero Least-Significant Bits Byte (LSBB)
 * and its opposite,
 * Vector Count Trailing Zero Least-Significant Bits Byte (LSBB),
 * forms the basis for a large set of string first/last match
 * intrinsic operations added to to Power Vector Intrinsic
 * Programming Reference.
 *
 * Counting only the <I>Least-Significant Bits Byte</I> compresses
 * leading/trailing bit counts into a byte element index.
 * The assumption is the input will be a vector bool generated from
 * a sequence of char element compare and logical operations.
 * Another novel aspect is the index is returned to a GPR.
 * This facilitates using the index to access character string in
 * storage.
 * Having both leading/trailing count operations supports first/last
 * searches. These operations also enables bi-endian vector support by
 * simply exchanging leading/trailing counts for the LE variant.
 *
 * PVECLIB's starts by implementing operations for each instruction
 * as defined in PowerISA 3.0B (See vec_vclzlsbb() and vec_vctzlsbb()).
 * This simplifies the coding for older (before POWER9) processors.
 * Then uses those operations to implement operations to match
 * bi-endian intrinsics defined by the Power Vector Intrinsic
 * Programming Reference (See vec_cntlz_lsbb_bi() and vec_cnttz_lsbb_bi()).
 * For example:
 * \code
static inline int
vec_cntlz_lsbb_bi (vui8_t vra)
{
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  return vec_vctzlsbb (vra);
#else
  return vec_vclzlsbb (vra);
#endif
}
 * \endcode
 * \note The vclzlsbb and vctzlsbb instructions are used in a number of
 * Power intrinsics including;
 * vec_cntlz_lsbb, vec_cnttz_lsbb,
 * vec_first_match_index, vec_first_match_or_eos_index,
 * vec_first_mismatch_index, vec_first_mismatch_or_eos_index.
 * The specific instruction used depends on the platform Endian.
 *
 * \subsubsection int8_P9_10_0_0_2_1 Vector Count Leading Zero LSBB
 *
 * The first section of code addresses compilers that support
 * the -mcpu=power9 target.
 * For this operation we want to generate the vclzlsbb (or equivalent)
 * instruction (independent of endian).
 * If the compiler supports the vec_cntlz_lsbb/vec_cnttz_lsbb intrinsics
 * use those but undo the compilers little-endian adjustment.
 * Otherwise use inline assembler to generate vclzlsbb.
 * For example:
 * \code
int test_vclzlsbb (vui8_t vra)
{
  int result;
#ifdef _ARCH_PWR9
#ifdef vec_cntlz_lsbb
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) && ( __GNUC__ >= 12)
  result = vec_cnttz_lsbb (vra);
#else
  result = vec_cntlz_lsbb (vra);
#endif
#else
  __asm__(
      "vclzlsbb %0,%1;"
      : "=r" (result)
      : "v" (vra)
      : );
#endif
#elif _ARCH_PWR8
  // ../
#endif
  return result;
}
 * \endcode
 *
 * For the mcpu=power8 target we need an implementation within the
 * existing PowerISA 2.07 instruction set.
 * This implementation must:
 * - isolate the Least-Significant Bits by Bytes,
 * - compress those bits into a contiguous 16-bit halfword,
 * - count the leading zeros of that halfword,
 * - return this count in a GPR.
 *
 * PowerISA 2.07 has a <I>Vector Gather Bits by Bytes by Doubleword</I>
 * (vec_gb()) instructions which seems like a good start.
 * It isolates the Least-Significant Bits into two bytes in separate
 * doublewords. A permute collects all 16 LSBB's into a single halfword
 * (in the high doubleword) for counting.
 * Then we can use <I>Vector Count Leading Zeros Halfword</I> to
 * count the leading zero LSBB's.
 * This result in the high-order doubleword ([VEC_DW_H]) of the vector
 * which is transfered to fixed-point (GPR) register as an integer.
 * \note This requires some halfword element operations that might be
 * sourced from vec_int16_ppc.h. But including vec_int16_ppc.h here
 * would be a circular dependency. So generic intrinsic operations
 * from the original Altivec<SUP>tm</SUP> are used.
 *
 * For example:
 * \code
#elif _ARCH_PWR8
  const vui8_t zeros = vec_splat_u8(0);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  const vui8_t pgbb = CONST_VINT128_B (0x10, 0x10, 0x10, 0x10,
				       0x10, 0x10, 0x08, 0x00,
				       0x10, 0x10, 0x10, 0x10,
				       0x10, 0x10, 0x10, 0x10);
#else
  const vui8_t pgbb = CONST_VINT128_B (0x10, 0x10, 0x10, 0x10,
				       0x10, 0x10, 0x07, 0x0F,
				       0x10, 0x10, 0x10, 0x10,
				       0x10, 0x10, 0x10, 0x10);
#endif
  vui8_t gbb;
  vui16_t lsbb, clzls;
  long long int dwres;

  gbb = vec_gb (vra);
  // Convert Gather Bits by Bytes by Doubleword
  // to Gather Bits by Halfwords by Quadword
  lsbb = (vui16_t) vec_perm (gbb, zeros, pgbb);
  clzls = vec_cntlz (lsbb);
  dwres = ((vui64_t) clzls) [VEC_DW_H];
  result = (unsigned short) dwres;
#else //  _ARCH_PWR7 and earlier
 * \endcode
 *
 * POWER7 (and earlier) processors don't support either
 * <I>Vector Gather Bits by Bytes by Doubleword</I> (vec_gb()) nor
 * <I>Vector Count Leading Zeros Halfword</I> (vec_cntlz ()).
 * The FXU does include a <I>Count Leading Zeros Word</I> instruction.
 * We can use this once we have isolated and compressed the LSBB bits
 * and transfered them to a GPR.
 *
 * Also the implementation needs to work within the original
 * Altivec<SUP>tm</SUP> vector intrinsics.
 * One implementation use vector bit-masks, shifts, and sum across
 * intrinsics to isolate the 16 LSBB bits.
 * \note This requires some word element operations that might be
 * sourced from vec_int32_ppc.h. But including vec_int32_ppc.h here
 * would be a circular dependency.
 *
 * For example:
 * \code
#else //  _ARCH_PWR7 and earlier
  const vui8_t zeros = vec_splat_u8(0);
  const vui8_t LSBBmask = vec_splat_u8(1);
  const vui8_t LSBBshl = CONST_VINT128_B (3, 2, 1, 0, 3, 2, 1, 0,
					  3, 2, 1, 0, 3, 2, 1, 0);
  const vui32_t LSBWshl = CONST_VINT128_W (12, 8, 4, 0);
  vui8_t gbb, gbbsb;
  vui32_t gbbsw;
  long long int dwres;
  // isolate LSBB bits
  gbb = vec_and (vra, LSBBmask);
  // merge lsbb into nibbles by word
  gbbsb = vec_sl (gbb, LSBBshl);
  gbbsw = vec_sum4s (gbbsb, (vui32_t) zeros);
  // merge lsbw into halfword by word
  gbbsw = vec_sl (gbbsw, LSBWshl);
  gbbsw = (vui32_t) vec_sums ((vi32_t) gbbsw, (vi32_t) zeros);
  // transfer from vector to GPR
  dwres = ((vui64_t) gbbsw) [VEC_DW_L];
  // Use GCC Builtin to get final leading zero count
  // with fake unsigned short clz
  result = __builtin_clz ((unsigned int) (dwres)) - 16;
#endif
 * \endcode
 *
 * \subsubsection int8_P9_10_0_0_2_2 Vector Count Trailing Zero LSBB
 *
 * The first section of code addresses compilers that support
 * the -mcpu=power9 target.
 * For this operation we want to generate the vctzlsbb (or equivalent)
 * instruction (independent of endian).
 * If the compiler supports the vec_cntlz_lsbb/vec_cnttz_lsbb intrinsics
 * use those but undo the compilers little-endian adjustment.
 * Otherwise use inline assembler to generate vctzlsbb.
 * For example:
 * \code
int test_vctzlsbb (vui8_t vra)
{
  int result;
#ifdef _ARCH_PWR9
#ifdef vec_cnttz_lsbb
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) && ( __GNUC__ >= 12)
  result = vec_cntlz_lsbb (vra);
#else
  result = vec_cnttz_lsbb (vra);
#endif
#else
  __asm__(
      "vctzlsbb %0,%1;"
      : "=r" (result)
      : "v" (vra)
      : );
#endif
#elif _ARCH_PWR8
  // ../
#endif
  return result;
}
 * \endcode
 *
 * For the mcpu=power8 target we need an implementation within the
 * existing PowerISA 2.07 instruction set.
 * This implementation must:
 * - isolate the Least-Significant Bits by Bytes,
 * - compress those bits into a contiguous 16-bit halfword,
 * - count the trailing zeros of that halfword,
 * - return this count in a GPR.
 *
 * PowerISA 2.07 has Vector Gather Bits by Bytes but no Count Trailing
 * Zeros. It does have Vector Population Count.
 * We use the formula <B>!(lsbb | -lsbb)</B> to convert the trailing
 * 0's into to trailing 1's and 0's elsewhere.
 * Then use <I>vec_popcnt()</I> to count the 1's, which is equivalent to
 * counting trailing 0's.
 * This result in the high-order doubleword ([VEC_DW_H]) of the vector
 * which is transfered to fixed-point (GPR) register as an integer.
 * \note Need to use altivec.h intrinsics here to avoid circular
 * dependencies. Older GCC compilers supporting power8 will provide
 * vec_vpopcntb/vec_vpopcnth but may not provide the generic vec_popcnt
 * for byte/halfword. Newer GCC compilers still support vec_vpopcntb,
 * vec_vpopcnth and the generic vec_popcnt. Clang compilers that
 * support power8 and vpopcntb/h instructions only support the generic
 * vec_popcnt intrinsic.
 *
 * For example:
 * \code
#elif _ARCH_PWR8
  const vui16_t zeros = vec_splat_u16 (0);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  const vui8_t pgbb = CONST_VINT128_B (0x10, 0x10, 0x10, 0x10,
				       0x10, 0x10, 0x08, 0x00,
				       0x10, 0x10, 0x10, 0x10,
				       0x10, 0x10, 0x10, 0x10);
#else
  const vui8_t pgbb = CONST_VINT128_B (0x10, 0x10, 0x10, 0x10,
				       0x10, 0x10, 0x07, 0x0F,
				       0x10, 0x10, 0x10, 0x10,
				       0x10, 0x10, 0x10, 0x10);
#endif
  vui8_t gbb;
  vui16_t lsbb, ctzls, tzmask;
  long long int dwres;

  gbb = vec_gb (vra);
  lsbb = (vui16_t) vec_perm (gbb, (vui8_t) zeros, pgbb);
  // tzmask = !(lsbb | -lsbb) -> tzmask = !(lsbb | (0-lsbb))
  tzmask = vec_nor (lsbb, vec_sub (zeros, lsbb));
  // return = vec_popcnt (!lsbb & (lsbb - 1))
  // GCC _ARCH_PWR8 supports vpopcnth
#if defined (vec_vpopcntb)
  ctzls = vec_vpopcnth (tzmask);
#else // clang
  ctzls = vec_popcnt (tzmask);
#endif
  dwres = ((vui64_t) ctzls) [VEC_DW_H];
  result = (unsigned short) dwres;
#else //  _ARCH_PWR7 and earlier
 * \endcode
 *
 * POWER7 (and earlier) processors don't support
 * <I>Vector Gather Bits by Bytes by Doubleword</I> (vec_gb()),
 * <I>Vector Count Leading Zeros Halfword</I> (vec_cntlz ())
 * or <I>Vector Population Count Halfword</I> (vec_popcnt()).
 * The FXU does include the <I>Population Count Byte/Word</I>
 * instruction and the GCC __builtin_ctz() uses those.
 * We use the same code as vclzlsbb (above) to isolate/compress
 * LSBB's and transfered them to a GPR.
 * Then use the GCC __builtin_ctz() to count the trailing 0's.
 *
 * For example:
 * \code
#else //  _ARCH_PWR7 and earlier
  const vui8_t zeros = vec_splat_u8(0);
  const vui8_t LSBBmask = vec_splat_u8(1);
  const vui8_t LSBBshl = CONST_VINT128_B (3, 2, 1, 0, 3, 2, 1, 0,
					  3, 2, 1, 0, 3, 2, 1, 0);
  const vui32_t LSBWshl = CONST_VINT128_W (12, 8, 4, 0);
  vui8_t gbb, gbbsb;
  vui32_t gbbsw;
  long long int dwres;

  // Mask the least significant bit of each byte
  gbb = vec_and (vra, LSBBmask);
  // merge lsbb into nibbles by word
  gbbsb = vec_sl (gbb, LSBBshl);
  gbbsw = vec_sum4s (gbbsb, (vui32_t) zeros);
  // merge lsbw into halfword by word
  gbbsw = vec_sl (gbbsw, LSBWshl);
  gbbsw = (vui32_t) vec_sums ((vi32_t) gbbsw, (vi32_t) zeros);
  // transfer from vector to GPR
  dwres = ((vui64_t) gbbsw) [VEC_DW_L];
  // Use GCC Builtin to get final trailing zero count
  // with fake unsigned short ctz
  result = __builtin_ctz ((unsigned int) (dwres+0x10000));
#endif
 * \endcode
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
static inline int vec_vclzlsbb (vui8_t vra);
static inline int vec_vctzlsbb (vui8_t vra);
static inline vb8_t vec_vcmpneb (vui8_t vra, vui8_t vrb);
static inline vb8_t vec_vcmpnezb (vui8_t vra, vui8_t vrb);
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
#ifdef vec_absdb
  result = vec_absdb (vra, vrb);
#else
  __asm__(
      "vabsdub %0,%1,%2;"
      : "=v" (result)
      : "v" (vra), "v" (vrb)
      : );
#endif
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
#else
  // For _ARCH_PWR8 and earlier. Generate 1's for the trailing zeros
  // and 0's otherwise. Then count (popcnt) the 1's. _ARCH_PWR8 uses
  // the hardware vpopcntb instruction.
  // _ARCH_PWR7 use the PVECLIB vec_common_ppc.h implementation
  // which runs ~6-13 cycles.
  const vui8_t ones = vec_splat_u8 (1);
  vui8_t tzmask;
  // tzmask = (!vra & (vra - 1))
  tzmask = vec_andc (vec_sub (vra, ones), vra);
  // return = vec_popcnt (!vra & (vra - 1))
  r = vec_popcntb (tzmask);
#endif
  return ((vui8_t) r);
}

/** \brief Vector Count Leading Zero Least-Significant Bits Byte.
 *
 *  Count the number of contiguous leading byte elements in the
 *  operand [vra] having a zero least-significant bit.
 *
 *  \note This operation implements the
 *  Power Bi-Endian Vector Programming Model specified in the
 *  Power Vector Intrinsic Programming Reference.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 11-22 | 2/cycle  |
 *  |power9   |   3   | 4/cycle  |
 *  |power10  |  3-4  | 4/cycle  |
 *
 *  @param vra vector of 16 unsigned char
 *  @return int value (0-16).
 */
static inline int
vec_cntlz_lsbb_bi (vui8_t vra)
{
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  return vec_vctzlsbb (vra);
#else
  return vec_vclzlsbb (vra);
#endif
}

/** \brief Vector Expand Mask Byte.
 *
 *  Create byte element masks based on high-order (sign) bit of
 *  each byte element.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 2 - 4 | 2/cycle  |
 *  |power8   | 2 - 4 | 2/cycle  |
 *  |power9   | 3 - 6 | 2/cycle  |
 *  |power10  | 3 - 4 | 4/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned char.
 *  @return vector byte mask from the sign bit.
 */
static inline vui8_t
vec_expandm_byte (vui8_t vra)
{
  return vec_vexpandbm_PWR10 (vra);
}

/** \brief Vector Count Trailing Zero Least-Significant Bits Byte.
 *
 *  Count the number of contiguous trailing byte elements in the
 *  operand [vra] having a zero least-significant bit.
 *
 *  \note This operation implements the
 *  Power Bi-Endian Vector Programming Model specified in the
 *  Power Vector Intrinsic Programming Reference.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 15-26 | 2/cycle  |
 *  |power9   |   3   | 4/cycle  |
 *  |power10  |  3-4  | 4/cycle  |
 *
 *  @param vra vector of 16 unsigned char
 *  @return int value (0-16).
 */
static inline int
vec_cnttz_lsbb_bi (vui8_t vra)
{
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  return vec_vclzlsbb (vra);
#else
  return vec_vctzlsbb (vra);
#endif
}

/** \brief Vector First Match Byte Index.
 *
 *  Comparison of equality for each of the corresponding elements of
 *  vra and vrb, and returns the byte index of the first position of
 *  equality.
 *
 *  \note This operation implements the
 *  Power Bi-Endian Vector Programming Model specified in the
 *  Power Vector Intrinsic Programming Reference.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 17-28 |    NA    |
 *  |power9   |   6   | 2/cycle  |
 *  |power10  |  2-6  | 4/cycle  |
 *
 *  @param vra vector of 16 unsigned char
 *  @param vrb vector of 16 unsigned char
 *  @return int value (0-16).
 */
static inline int
vec_first_match_byte_index (vui8_t vra, vui8_t vrb)
{
#if defined(_ARCH_PWR9) && (__GNUC__ > 10)
  return vec_first_match_index (vra, vrb);
#else
  vui8_t abeq;
  int result;

  abeq = (vui8_t) vec_cmpeq(vra, vrb);
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  result = vec_vctzlsbb (abeq);
#else
  result = vec_vclzlsbb (abeq);
#endif
  return result;
#endif
}

/** \brief Vector First Match Byte or EOS Index.
 *
 *  Comparison of equality for each of the corresponding elements of
 *  vra and vrb, and returns the byte index of the first position of
 *  equality, or the zero string terminator.
 *
 *  \note This operation implements the
 *  Power Bi-Endian Vector Programming Model specified in the
 *  Power Vector Intrinsic Programming Reference.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 21-32 |    NA    |
 *  |power9   | 12-15 | 2/cycle  |
 *  |power10  |  4-5  | 4/cycle  |
 *
 *  @param vra vector of 16 unsigned char
 *  @param vrb vector of 16 unsigned char
 *  @return int value (0-16).
 */
static inline int
vec_first_match_byte_or_eos_index (vui8_t vra, vui8_t vrb)
{
#if defined(_ARCH_PWR9) && (__GNUC__ > 13)
  return vec_first_match_or_eos_index (vra, vrb);
#else
  const vui8_t VEOS = vec_splat_u8(0);
  vui8_t abeqz;
  vb8_t abeq, eosa, eosb, eosc;
  int result;

  eosa = vec_cmpeq (vra, VEOS);
  eosb = vec_cmpeq (vrb, VEOS);
  abeq = vec_cmpeq (vra, vrb);
  eosc = vec_or (eosa, eosb);
  abeqz = (vui8_t) vec_or (abeq, eosc);
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  result = vec_vctzlsbb (abeqz);
#else
  result = vec_vclzlsbb (abeqz);
#endif
  return result;
#endif
}

/** \brief Vector First Mismatch Byte Index.
 *
 *  Comparison of inequality for each of the corresponding elements of
 *  vra and vrb, and returns the byte index of the first position of
 *  inequality.
 *
 *  \note This operation implements the
 *  Power Bi-Endian Vector Programming Model specified in the
 *  Power Vector Intrinsic Programming Reference.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 19-30 |    NA    |
 *  |power9   |   6   | 2/cycle  |
 *  |power10  |  2-6  | 4/cycle  |
 *
 *  @param vra vector of 16 unsigned char
 *  @param vrb vector of 16 unsigned char
 *  @return int value (0-16).
 */
static inline int
vec_first_mismatch_byte_index (vui8_t vra, vui8_t vrb)
{
#if defined(_ARCH_PWR9) && (__GNUC__ > 10)
  return vec_first_mismatch_index (vra, vrb);
#else
  vui8_t abeq;
  int result;

  abeq = (vui8_t) vec_vcmpneb(vra, vrb);
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  result = vec_vctzlsbb (abeq);
#else
  result = vec_vclzlsbb (abeq);
#endif
  return result;
#endif
}

/** \brief Vector First Mismatch Byte or EOS Index.
 *
 *  Comparison of equality for each of the corresponding elements of
 *  vra and vrb, and returns the byte index of the first position of
 *  equality, or the zero string terminator.
 *
 *  \note This operation implements the
 *  Power Bi-Endian Vector Programming Model specified in the
 *  Power Vector Intrinsic Programming Reference.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 21-32 |    NA    |
 *  |power9   |   6   | 2/cycle  |
 *  |power10  |  2-6  | 4/cycle  |
 *
 *  @param vra vector of 16 unsigned char
 *  @param vrb vector of 16 unsigned char
 *  @return int value (0-16).
 */
static inline int
vec_first_mismatch_byte_or_eos_index (vui8_t vra, vui8_t vrb)
{
#if defined (_ARCH_PWR9) && defined (__VSX__) && (__GNUC__ > 7)
#if (__GNUC__ > 13)
  return vec_first_mismatch_or_eos_index (vra, vrb);
#else
  vui8_t abnez;
  abnez  = (vui8_t) vec_cmpnez (vra, vrb);
  return vec_cntlz_lsbb_bi (abnez);
#endif
#else // _ARCH_PWR8
  const vui8_t VEOS = vec_splat_u8(0);
  vui8_t abnez;
  vb8_t ab_b, eosa, eosb, eosc;
  int result;

  eosa = vec_cmpeq (vra, VEOS);
  eosb = vec_cmpeq (vrb, VEOS);
  // vcmpneb requires _ARCH_PWR9, so use NOT cmpeq
  ab_b = vec_cmpeq (vra, vrb);
  eosc = vec_or (eosa, eosb);
#ifdef _ARCH_PWR8
  abnez = (vui8_t) vec_orc (eosc, ab_b);
#else // vorc requires _ARCH_PWR8, so use cmpeq, nor and or
  abnez = (vui8_t) vec_nor (ab_b, ab_b);
  abnez = vec_or ((vui8_t) eosc, abnez);
#endif
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  result = vec_vctzlsbb (abnez);
#else
  result = vec_vclzlsbb (abnez);
#endif
  return result;
#endif
}

/** \brief Vector isalnum.
 *
 * Return a vector boolean char with a true indicator for any character
 * that is either Lower Case Alpha ASCII, Upper Case ASCII, or numeric ASCII.
 * False otherwise.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-20 | 1/cycle  |
 *  |power9   | 11-21 | 1/cycle  |
 *
 * @param vec_str vector of 16 ASCII characters
 * @return vector bool char of the isalnum operation applied to each
 * character of vec_str. For each byte 0xff indicates true (isalnum),
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

/** \brief Vector isalpha.
 *
 * Return a vector boolean char with a true indicator for any character
 * that is either Lower Case Alpha ASCII or Upper Case ASCII.
 * False otherwise.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 9-18  | 1/cycle  |
 *  |power9   | 10-19 | 1/cycle  |
 *
 * @param vec_str vector of 16 ASCII characters
 * @return vector bool char of the isalpha operation applied to each
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
  r = vec_popcntb_PWR7 (vra);
#endif
  return (r);
}
#else
/* Work around for GCC PR85830.  */
#undef vec_popcntb
#define vec_popcntb __builtin_vec_vpopcntb
#endif

/** \brief Vector Rotate left Byte Immediate.
 *
 *  Rotate left each word element [0-15], 0-7 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-7.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 31 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-11  | 2/cycle  |
 *  |power9   | 5-11  | 2/cycle  |
 *  |power10  | 4-7   | 4/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned char.
 *  @param shb shift amount in the range 0-7.
 *  @return 128-bit vector unsigned char, rotated left shb bits.
 */
static inline vui8_t
vec_rlbi (vui8_t vra, const unsigned  shb)
{
  vui8_t lshift;
  vui8_t result;
  /* Load the shift const in a vector.  The element shifts require
     a shift amount for each element. For the immediate form the
     shift constant is splatted to all elements of the
     shift control.  */
  if (__builtin_constant_p (shb) && (shb < 8))
    lshift = vec_splat_u8(shb);
  else
    lshift = vec_splats ((unsigned char) shb);

  /* Vector Rotate Left halfword based on the lower 3-bits of
     corresponding element of lshift.  */
  result = vec_vrlb (vra, lshift);

  return result;
}

/*! \brief Vector Set Bool from Signed Byte.
 *
 *  For each byte, propagate the sign bit to all 8-bits of that
 *  byte. The result is vector bool char reflecting the sign
 *  bit of each 8-bit byte.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 2 - 4 | 2/cycle  |
 *  |power8   | 2 - 4 | 2/cycle  |
 *  |power9   | 3 - 6 | 2/cycle  |
 *  |power10  | 3 - 4 | 4/cycle  |
 *
 *  @param vra Vector signed char.
 *  @return vector bool char reflecting the sign bit of each
 *  byte.
 */

static inline vb8_t
vec_setb_sb (vi8_t vra)
{
  return (vb8_t) vec_expandm_byte ((vui8_t) vra);
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
 *  |power10  | 4-7   | 4/cycle  |
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
	lshift = vec_splat_u8(shb);
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
 *  |power10  | 4-7   | 4/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector signed char.
 *  @param shb Shift amount in the range 0-7.
 *  @return 128-bit vector signed char, shifted right shb bits.
 */
static inline vi8_t
vec_srabi (vi8_t vra, const unsigned int shb)
{
  vui8_t rshift;
  vi8_t result;

  if (shb < 8)
    {
      /* Load the shift const in a vector.  The element shifts require
         a shift amount for each element. For the immediate form the
         shift constant is splatted to all elements of the
         shift control.  */
      if (__builtin_constant_p(shb))
	rshift = vec_splat_u8(shb);
      else
	rshift = vec_splats ((unsigned char) shb);

      /* Vector Shift Right Algebraic Bytes based on the lower 3-bits
         of corresponding element of lshift.  */
      result = vec_vsrab (vra, rshift);
    }
  else
    { /* shifts greater then 7 bits returns the sign bit propagated to
         all bits.   This is equivalent to shift Right Algebraic of
         7 bits.  */
      rshift = vec_splat_u8(7);
      result = vec_vsrab (vra, rshift);
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
 *  |power10  | 4-7   | 4/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned char.
 *  @param shb Shift amount in the range 0-7.
 *  @return 128-bit vector unsigned char, shifted right shb bits.
 */
static inline vui8_t
vec_srbi (vui8_t vra, const unsigned int shb)
{
  vui8_t rshift;
  vui8_t result;

  if (shb < 8)
    {
      /* Load the shift const in a vector.  The element shifts require
         a shift amount for each element. For the immediate form the
         shift constant is splatted to all elements of the
         shift control.  */
      if (__builtin_constant_p(shb))
	rshift = vec_splat_u8(shb);
      else
	rshift = vec_splats ((unsigned char) shb);

      /* Vector Shift right bytes based on the lower 3-bits of
         corresponding element of lshift.  */
      result = vec_vsrb (vra, rshift);
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

/** \brief Vector Test Least-Significant Bit by Byte for All Ones.
 *
 *  Test whether the least-significant bit of all bytes of the input
 *  operand are equal to one.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  4-6  | 2/cycle  |
 *  |power9   |  6-9  | 4/cycle  |
 *  |power10  |  3-4  | 4/cycle  |
 *
 *  @param vra vector of 16 unsigned char
 *  @return int value (0-1).
 */
static inline int
vec_testlsbb_all_ones (vui8_t vra)
{
#if defined(_ARCH_PWR10) && (__GNUC__ > 9)
#if defined (vec_test_lsbb_all_ones)
  return vec_test_lsbb_all_ones (vra);
#else
  int r;
  __asm__(
      "xvtlsbb 0,%x1;\n"
      "setbc   %0,lt;\n"
      : "=r" (r)
      : "wa" (vra)
      : "cr0"
	);
  return r;
#endif
#else
  const vui8_t ones = vec_splat_u8(1);
  vui8_t lsbb;

  lsbb = vec_and (vra, ones);
  return vec_all_eq (lsbb, ones);
#endif
}

/** \brief Vector Test Least-Significant Bit by Byte for All Zeros.
 *
 *  Test whether the least-significant bit of all bytes of the input
 *  operand are not equal to one.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  4-6  | 2/cycle  |
 *  |power9   |  6-9  | 4/cycle  |
 *  |power10  |  3-4  | 4/cycle  |
 *
 *  @param vra vector of 16 unsigned char
 *  @return int value (0-1).
 */
static inline int
vec_testlsbb_all_zeros (vui8_t vra)
{
#if defined(_ARCH_PWR10) && (__GNUC__ > 9)
#if defined (vec_test_lsbb_all_ones)
  return vec_test_lsbb_all_zeros (vra);
#else
  int r;
  __asm__(
      "xvtlsbb 0,%x1;\n"
      "setbc   %0,eq;\n"
      : "=r" (r)
      : "wa" (vra)
      : "cr0"
	);
  return r;
#endif
#else
  const vui8_t ones = vec_splat_u8(1);
  vui8_t lsbb;

  lsbb = vec_and (vra, ones);
  return vec_all_ne (lsbb, ones);
#endif
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

/** \brief Vector Clear Leftmost Bytes.
 *
 *  Clear the leftmost N - rb bytes of vra to 0x00.
 *  If rb >= 16 then vra is returned unchanged.
 *  If rb == 0 then the zeros vector is returned.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-21  | 2/cycle  |
 *  |power9   | 5-21  | 2/cycle  |
 *  |power10  |  3-4  | 4/cycle  |
 *
 *  \note For POWER8/9, best performance is when rb is a constant.
 *
 *  @param vra Vector of 16 byte elements.
 *  @param rb Byte count.
 *  @return Vector char with rb bytes cleared from vra.
 */
static inline vui8_t
vec_vclrlb (vui8_t vra, unsigned int rb)
{
#if defined(_ARCH_PWR10)
#if (__GNUC__ > 10)
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  return vec_clrr (vra, rb);
#else
  return vec_clrl (vra, rb);
#endif
#elif defined(__clang__) && (__clang_major__ > 12)
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  return vec_clr_last (vra, rb);
#else
  return vec_clr_first (vra, rb);
#endif
#else
  vui8_t result;
  __asm__(
      "vclrlb %0,%1,%2;"
      : "=v" (result)
      : "v" (vra), "r" (rb)
      : );
  return result;
#endif
#else
  const vui8_t zeros = vec_splat_u8(0);
  const vui8_t ones = (vui8_t) vec_splat_s8(-1);
  vui8_t clrmask;
  // In case rb==0 clear all 16 bytes.
  vui8_t result = vec_splat_u8(0);
  unsigned int N;
  // rb == 0 clears 16 bytes
  if (rb != 0)
    {
      // rb >= 16 clears no bytes
      clrmask = ones;
      if (rb < 16)
	{
	  // Clear N bytes in the range 15-1
	  N = 16 - rb;
	  if (__builtin_constant_p(rb))
	    {
	      // If rb/N is const use vsldoi
	      clrmask = vec_sld (zeros, clrmask, rb);
	    }
	  else
	    { // otherwise xfer N*8 to VR and use vslo
#if defined(_ARCH_PWR8)
	      // Take advantage of P8 VSX and direct move.
	      vui64_t shfcnt;
	      shfcnt[VEC_DW_L] = N * 8;
#else
	      vui32_t shfcnt;
	      shfcnt [VEC_W_L] = N * 8;
#endif
	      clrmask = vec_sro (clrmask, (vui8_t) shfcnt);
	    }
	}
      return vec_and (vra, clrmask);
    }
  return result;
#endif
}

/** \brief Vector Clear Rightmost Bytes.
 *
 *  Clear the rightmost N - rb bytes of vra to 0x00.
 *  If rb >= 16 then vra is returned unchanged.
 *  If rb == 0 then the zeros vector is returned.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-21  | 2/cycle  |
 *  |power9   | 5-21  | 2/cycle  |
 *  |power10  |  3-4  | 4/cycle  |
 *
 *  \note For POWER8/9, best performance is when rb is a constant.
 *
 *  @param vra Vector of 16 byte elements.
 *  @param rb Byte count.
 *  @return Vector char with rb bytes cleared from vra.
 */
static inline vui8_t
vec_vclrrb (vui8_t vra, unsigned int rb)
{
#if defined(_ARCH_PWR10)
#if (__GNUC__ > 10)
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  return vec_clrl (vra, rb);
#else
  return vec_clrr (vra, rb);
#endif
#elif defined(__clang__) && (__clang_major__ > 12)
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  return vec_clr_first (vra, rb);
#else
  return vec_clr_last (vra, rb);
#endif
#else
  vui8_t result;
  __asm__(
      "vclrrb %0,%1,%2;"
      : "=v" (result)
      : "v" (vra), "r" (rb)
      : );
  return result;
#endif
#else
  const vui8_t zeros = vec_splat_u8(0);
  const vui8_t ones = (vui8_t) vec_splat_s8(-1);
  vui8_t clrmask;
  vui8_t result = vec_splat_u8(0);
  unsigned int N;

  if ( rb != 0)
    {
      // rb >= 16 clears no bytes
      clrmask = ones;
      if (rb < 16)
	{
	  // Clear N bytes in the range 15-1
	  N = 16 - rb;
	  if (__builtin_constant_p(rb))
	    {
	      // If rb/N is const use vsldoi
	      clrmask = vec_sld (clrmask, zeros, N);
	    }
	  else
	    { // otherwise xfer N*8 to VR and use vslo
#if defined(_ARCH_PWR8)
	      // Take advantage of P8 VSX and direct move.
	      vui64_t shfcnt;
	      shfcnt[VEC_DW_L] = N * 8;
#else
	      vui32_t shfcnt;
	      shfcnt [VEC_W_L] = N * 8;
#endif
	      clrmask = vec_slo (clrmask, (vui8_t) shfcnt);
	    }
	}
      result = vec_and (vra, clrmask);
    }
  return result;
#endif
}

/** \brief Vector Count Leading Zero Least-Significant Bits Byte.
 *
 *  Count the number of contiguous leading byte elements in the
 *  operand [vra] having a zero least-significant bit.
 *
 *  \note This operation implements the equivalent of the PowerISA
 *  3.0 instruction vclzlsbb. It does not implement the
 *  Power Bi-Endian Vector Programming Model specified in the
 *  Power Vector Intrinsic Programming Reference.
 *  See vec_cntlz_lsbb_bi().
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 11-22 | 2/cycle  |
 *  |power9   |   3   | 4/cycle  |
 *  |power10  |  3-4  | 4/cycle  |
 *
 *  @param vra vector of 16 unsigned char
 *  @return int value (0-16).
 */
static inline int
vec_vclzlsbb (vui8_t vra)
{
  int result;
#ifdef _ARCH_PWR9
#ifdef vec_cntlz_lsbb
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) && ( __GNUC__ >= 12)
  result = vec_cnttz_lsbb (vra);
#else
  result = vec_cntlz_lsbb (vra);
#endif
#else
  __asm__(
      "vclzlsbb %0,%1;"
      : "=r" (result)
      : "v" (vra)
      : );
#endif
#elif _ARCH_PWR8
  const vui8_t zeros = vec_splat_u8(0);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  const vui8_t pgbb = CONST_VINT128_B (0x10, 0x10, 0x10, 0x10,
				       0x10, 0x10, 0x08, 0x00,
				       0x10, 0x10, 0x10, 0x10,
				       0x10, 0x10, 0x10, 0x10);
#else
  const vui8_t pgbb = CONST_VINT128_B (0x10, 0x10, 0x10, 0x10,
				       0x10, 0x10, 0x07, 0x0F,
				       0x10, 0x10, 0x10, 0x10,
				       0x10, 0x10, 0x10, 0x10);
#endif
  vui8_t gbb;
  vui16_t lsbb, clzls;
  long long int dwres;
  // PowerISA 2.07 (P8) has:
  // Vector Gather Bits by Bytes and Count Leading Zeros
  // So use vec_gb to collect the Least-Significant Bits by Byte
  // Vec_gb is Gather Bits by Bytes by Doubleword
  // so the lsbb's are in bytes 7 and 15.
  // Use vec_perm to collect all 16 lsbb's into a single Halfword.
  // Then vec_cntlz to count the leading zeros within lsbb's
  gbb = vec_gb (vra);
  lsbb = (vui16_t) vec_perm (gbb, zeros, pgbb);
  clzls = vec_cntlz (lsbb);
  dwres = ((vui64_t) clzls) [VEC_DW_H];
  result = (unsigned short) dwres;
#else
  const vui8_t zeros = vec_splat_u8(0);
  const vui8_t LSBBmask = vec_splat_u8(1);
  const vui8_t LSBBshl = CONST_VINT128_B (3, 2, 1, 0, 3, 2, 1, 0,
					  3, 2, 1, 0, 3, 2, 1, 0);
  const vui32_t LSBWshl = CONST_VINT128_W (12, 8, 4, 0);
  vui8_t gbb, gbbsb;
  vui32_t gbbsw;
  long long int dwres;

  gbb = vec_and (vra, LSBBmask);
  // merge lsbb into nibbles by word
  gbbsb = vec_sl (gbb, LSBBshl);
  gbbsw = vec_sum4s (gbbsb, (vui32_t) zeros);
  // merge lsbw into halfword by word
  gbbsw = vec_sl (gbbsw, LSBWshl);
  gbbsw = (vui32_t) vec_sums ((vi32_t) gbbsw, (vi32_t) zeros);
  // transfer from vector to GPR
  dwres = ((vui64_t) gbbsw) [VEC_DW_L];
  // Use GCC Builtin to get final leading zero count
  // with fake unsigned short clz
  result = __builtin_clz ((unsigned int) (dwres)) - 16;
#endif
  return result;
}

/** \brief Vector Count Trailing Zero Least-Significant Bits Byte.
 *
 *  Count the number of contiguous trailing byte elements in the
 *  operand [vra] having a zero least-significant bit.
 *
 *  \note This operation implements the equivalent of the PowerISA
 *  3.0 instruction vctzlsbb. It does not implement the
 *  Power Bi-Endian Vector Programming Model specified in the
 *  Power Vector Intrinsic Programming Reference.
 *  See vec_cnttz_lsbb_bi().
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 15-26 | 2/cycle  |
 *  |power9   |   3   | 4/cycle  |
 *  |power10  |  3-4  | 4/cycle  |
 *
 *  @param vra vector of 16 unsigned char
 *  @return int value (0-16).
 */
static inline int
vec_vctzlsbb (vui8_t vra)
{
  int result;
#ifdef _ARCH_PWR9
#ifdef vec_cnttz_lsbb
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) && ( __GNUC__ >= 12)
  result = vec_cntlz_lsbb (vra);
#else
  result = vec_cnttz_lsbb (vra);
#endif
#else
  __asm__(
      "vctzlsbb %0,%1;"
      : "=r" (result)
      : "v" (vra)
      : );
#endif
#elif _ARCH_PWR8
  const vui16_t zeros = vec_splat_u16 (0);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  const vui8_t pgbb = CONST_VINT128_B (0x10, 0x10, 0x10, 0x10,
				       0x10, 0x10, 0x08, 0x00,
				       0x10, 0x10, 0x10, 0x10,
				       0x10, 0x10, 0x10, 0x10);
#else
  const vui8_t pgbb = CONST_VINT128_B (0x10, 0x10, 0x10, 0x10,
				       0x10, 0x10, 0x07, 0x0F,
				       0x10, 0x10, 0x10, 0x10,
				       0x10, 0x10, 0x10, 0x10);
#endif
  vui8_t gbb;
  vui16_t lsbb, ctzls, tzmask;
  long long int dwres;
  // PowerISA 2.07 (P8) has:
  // Vector Gather Bits by Bytes but not Count Trailing Zeros.
  // It does have Vector Population Count.
  // So after collecting the lsbb's into a Halfword we can use the
  // formula !(lsbb | -lsbb) to Generate 1's for the trailing
  // zeros and 0's otherwise. Then count the 1's (via vec_popcnt())
  // to generate the count of trailing zeros
  gbb = vec_gb (vra);
  lsbb = (vui16_t) vec_perm (gbb, (vui8_t) zeros, pgbb);
  // tzmask = !(lsbb | -lsbb)
  // tzmask = !(lsbb | (0-lsbb))
  tzmask = vec_nor (lsbb, vec_sub (zeros, lsbb));
  // return = vec_popcnt (!lsbb & (lsbb - 1))
  // _ARCH_PWR8 supports vpopcnth
#if defined (vec_vpopcntb)
  ctzls = vec_vpopcnth (tzmask);
#else // clang
  ctzls = vec_popcnt (tzmask);
#endif
  dwres = ((vui64_t) ctzls) [VEC_DW_H];
  result = (unsigned short) dwres;
#else
  const vui8_t zeros = vec_splat_u8(0);
  const vui8_t LSBBmask = vec_splat_u8(1);
  const vui8_t LSBBshl = CONST_VINT128_B (3, 2, 1, 0, 3, 2, 1, 0,
					  3, 2, 1, 0, 3, 2, 1, 0);
  const vui32_t LSBWshl = CONST_VINT128_W (12, 8, 4, 0);
  vui8_t gbb, gbbsb;
  vui32_t gbbsw;
  long long int dwres;

  // Mask the least significant bit of each byte
  gbb = vec_and (vra, LSBBmask);
  // merge lsbb into nibbles by word
  gbbsb = vec_sl (gbb, LSBBshl);
  gbbsw = vec_sum4s (gbbsb, (vui32_t) zeros);
  // merge lsbw into halfword by word
  gbbsw = vec_sl (gbbsw, LSBWshl);
  gbbsw = (vui32_t) vec_sums ((vi32_t) gbbsw, (vi32_t) zeros);
  // transfer from vector to GPR
  dwres = ((vui64_t) gbbsw) [VEC_DW_L];
  // Use GCC Builtin to get final trailing zero count
  // with fake unsigned short ctz
  result = __builtin_ctz ((unsigned int) (dwres+0x10000));
#endif

  return result;
}

/** \brief Vector Compare Not Equal Byte.
 *
 * For each byte element of vra and vrb compare for (vra[i] != vrb[i].
 * For each byte where any of these conditions are true set that byte
 * to all 1s (0xff).
 *
 * \note This operation can be used for unsigned or signed char.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   4   | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *  |power10  |  1-3  | 4/cycle  |
 *
 * @param vra vector of 16 unsigned bytes
 * @param vrb vector of 16 unsigned bytes
 * @return vector bool char.
 */
static inline vb8_t
vec_vcmpneb (vui8_t vra, vui8_t vrb)
{
  vb8_t result;
#ifdef _ARCH_PWR9
#ifdef vec_cmpne
  result = vec_cmpne (vra, vrb);
#else
  __asm__(
      "vcmpneb %0,%1,%2;"
      : "=v" (result)
      : "v" (vra), "v" (vrb)
      : );
#endif
#else
  vb8_t abne;
  abne = vec_cmpeq (vra, vrb);
  result = vec_nor (abne, abne);
#endif
  return result;
}

/** \brief Vector Compare Not Equal or Zero Byte.
 *
 * For each byte element of vra and vrb compare for (vra[i] != vrb[i]
 * or (vra[i] == 0) or (vrb[i] == 0).
 * For each byte where any of these conditions are true set that byte
 * to all 1s (0xff).
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  6-8  | 1/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *  |power10  |  1-3  | 4/cycle  |
 *
 * @param vra vector of 16 unsigned bytes
 * @param vrb vector of 16 unsigned bytes
 * @return vector bool char.
 */
static inline vb8_t
vec_vcmpnezb (vui8_t vra, vui8_t vrb)
{
  vb8_t result;
#ifdef _ARCH_PWR9
#ifdef vec_cmpnez
  result = vec_cmpnez (vra, vrb);
#else
  __asm__(
      "vcmpnezb %0,%1,%2;"
      : "=v" (result)
      : "v" (vra), "v" (vrb)
      : );
#endif
#else  // _ARCH_PWR8 and earlier
  const vui8_t VEOS = vec_splat_u8(0);
  vb8_t eosa, eosb, eosc, abne;
  // vcmpneb requires _ARCH_PWR9, so use cmpeq and orc
  eosa = vec_cmpeq (vra, VEOS);
  eosb = vec_cmpeq (vrb, VEOS);
  abne = vec_cmpeq (vra, vrb);
  eosc = vec_or (eosa, eosb);
#ifdef _ARCH_PWR8
  result = vec_orc (eosc, abne);
#else // vorc requires _ARCH_PWR8, so use cmpeq, nor and or
  abne = vec_nor (abne, abne);
  result = vec_or (eosc, abne);
#endif
#endif
  return result;
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

/** \brief Vector String Isolate Byte Left-justified.
 *
 *  Copy a null-terminated string of elements from the input vector
 *  to the output vector, replacing all elements following the first
 *  (left most) zero element with zeros.
 *
 *  For POWER10 (PowerISA 3.1C) or later use the Vector String Isolate
 *  Byte Left-justified instruction <B>vstribl</B>. Otherwise use a
 *  sequence of pre ISA 3.1 VMX instructions.
 *
 *  \note This operation implements the equivalent of the PowerISA
 *  3.1 instruction vstribl. It does not implement the
 *  Power Bi-Endian Vector Programming Model specified in the
 *  Power Vector Intrinsic Programming Reference.
 *  See vec_stril().
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 16-20 | 2/cycle  |
 *  |power9   | 24-30 | 2/cycle  |
 *  |power10  |  3-4  | 4/cycle  |
 *
 *  @param vra 128-bit vector treated as string of 16 unsigned char
 *  (byte) elements.
 *  @return 128-bit vector String isolated and Left-justified.
 */
static inline vui8_t
vec_vstribl (vui8_t vra)
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
  const vui8_t ones = (vui8_t) vec_splat_s8(-1);
  vui8_t nulchr, clrmask, clrcnt, result;

  result = vra;
  if (vec_any_eq(vra, zeros))
    {
      nulchr = (vui8_t) vec_cmpeq (vra, zeros);
      // clrmask == ones unless nullchr == zeros
      clrmask = ones;
      // No Quadword Vector Count Leading Zeros yet. So
      // for POWER7/8/9 we can use the PVECLIB vec_clzq_PWR8/7
      // Quadword operations from vec_common_ppc.h.
      clrcnt = (vui8_t) vec_clzq_PWR8 ((vui128_t) nulchr);
      // Shift clrmask right by quadword clz of nulchar
      // leaving 0x00 bytes from byte 0 to first null char in vra
      clrmask = vec_sro (clrmask, clrcnt);
      // And compliment to clear trailing bytes after first nulchr
      result = vec_andc (vra, clrmask);
    }
  return result;
#endif
}

/** \brief Vector String Isolate Byte Left-justified (Predicate).
 *
 *  Tests whether the input vector contains a zero element.
 *
 *  For POWER10 (PowerISA 3.1C) or later use the Vector String Isolate
 *  Byte Left-justified (record form) instruction <B>vstribl.</B>.
 *  Otherwise use vec_any_eq (vra, zeros).
 *
 *  \note This operation implements the equivalent of the PowerISA
 *  3.1 instruction sequence; vstribl.,setbc. If the predicate is used
 *  to guard the string isolate operations the compile should common
 *  instructions across both operations.
 *  See vec_stril().
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 15-21 | 1/cycle  |
 *  |power9   | 11-14 | 2/cycle  |
 *  |power10  |  5-10 | 4/cycle  |
 *
 *  @param vra 128-bit vector treated as string of 16 unsigned char
 *  (byte) elements.
 *  @return int value (0-1).
 */
static inline int
vec_vstribl_p (vui8_t vra)
{
#if defined(_ARCH_PWR10)
#if ((__GNUC__ > 10) || (defined(__clang__) && (__clang_major__ > 12)))
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  return vec_strir_p (vra);
#else
  return vec_stril_p (vra);
#endif
#else
  vui8_t tmp;
  int result;
  __asm__(
      "vstribl. %1,%2;"
      "setbc %0,26;"
      : "=r" (result), "=v" (tmp)
      : "v" (vra)
      : "cr6");
  return result;
#endif
#else
  const vui8_t zeros = vec_splat_u8(0);
  return vec_any_eq (vra, zeros);
#endif
}

/** \brief Vector String Isolate Byte Right-justified.
 *
 *  Copy a null-terminated string of elements from the input vector
 *  to the output vector, replacing all elements following the first
 *  (right most) zero element with zeros.
 *
 *  For POWER10 (PowerISA 3.1C) or later use the Vector String Isolate
 *  Byte Left-justified instruction <B>vstribr</B>. Otherwise use a
 *  sequence of pre ISA 3.1 VMX instructions.
 *
 *  \note This operation implements the equivalent of the PowerISA
 *  3.1 instruction vstribr. It does not implement the
 *  Power Bi-Endian Vector Programming Model specified in the
 *  Power Vector Intrinsic Programming Reference.
 *  See vec_stril().
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 16-20 | 1/cycle  |
 *  |power9   | 24-30 | 1/cycle  |
 *  |power10  |  3-4  | 4/cycle  |
 *
 *  @param vra 128-bit vector treated as string of 16 unsigned char
 *  (byte) elements.
 *  @return 128-bit vector String isolated and right-justified.
 */
static inline vui8_t
vec_vstribr (vui8_t vra)
{
#if defined(_ARCH_PWR10)
#if ((__GNUC__ > 10) || (defined(__clang__) && (__clang_major__ > 12)))
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  return vec_stril (vra);
#else
  return vec_strir (vra);
#endif
#else
  vui8_t result;
  __asm__(
      "vstribr %0,%1;"
      : "=v" (result)
      : "v" (vra)
      : );
  return result;
#endif
#else
  const vui8_t zeros = vec_splat_u8(0);
  const vui8_t ones = (vui8_t) vec_splat_s8(-1);
  vui8_t nulchr, clrmask, clrcnt, result;

  result = vra;
  if (vec_any_eq(vra, zeros))
    {
      nulchr = (vui8_t) vec_cmpeq (vra, zeros);
      // clrmask == ones unless nullchr == zeros
      clrmask = ones;
      // No Quadword Vector Count Trailing Zeros yet. So
      // for POWER7/8/9 we can use the PVECLIB vec_ctzq_PWR9/8/7
      // Quadword operations from vec_common_ppc.h.
      clrcnt = (vui8_t) vec_ctzq_PWR9 ((vui128_t) nulchr);
      // Shift clrmask left by quadword ctz of nulchar
      // leaving 0x00 bytes from the first null char to byte 15 in vra
      clrmask = vec_slo (clrmask, clrcnt);
      // And compliment to clear trailing bytes before first nulchr
      result = vec_andc (vra, clrmask);
    }
  return result;
#endif
}

/** \brief Vector String Isolate Byte Right-justified (Predicate).
 *
 *  Tests whether the input vector contains a zero element.
 *
 *  For POWER10 (PowerISA 3.1C) or later use the Vector String Isolate
 *  Byte Right-justified (record form) instruction <B>vstribr.</B>.
 *  Otherwise use vec_any_eq (vra, zeros).
 *
 *  \note This operation implements the equivalent of the PowerISA
 *  3.1 instruction sequence; vstribr.,setbc. If the predicate is used
 *  to guard string isolate operations the compile should common
 *  instructions across both operations.
 *  See vec_strir().
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 15-21 | 1/cycle  |
 *  |power9   | 11-14 | 2/cycle  |
 *  |power10  |  5-10 | 4/cycle  |
 *
 *  @param vra 128-bit vector treated as string of 16 unsigned char
 *  (byte) elements.
 *  @return int value (0-1).
 */
static inline int
vec_vstribr_p (vui8_t vra)
{
#if defined(_ARCH_PWR10)
#if ((__GNUC__ > 10) || (defined(__clang__) && (__clang_major__ > 12)))
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  return vec_stril_p (vra);
#else
  return vec_strir_p (vra);
#endif
#else
  vui8_t tmp;
  int result;
  __asm__(
      "vstribr. %1,%2;"
      "setbc %0,26;"
      : "=r" (result), "=v" (tmp)
      : "v" (vra)
      : "cr6");
  return result;
#endif
#else
  const vui8_t zeros = vec_splat_u8(0);
  return vec_any_eq (vra, zeros);
#endif
}

#endif /* VEC_CHAR_PPC_H_ */
