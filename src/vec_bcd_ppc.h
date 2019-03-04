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

 vec_bcd_ppc.h

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Aug 12, 2015
 */

#ifndef VEC_BCD_PPC_H_
#define VEC_BCD_PPC_H_

#include <vec_common_ppc.h>
#include <vec_char_ppc.h>
#include <vec_int128_ppc.h>

/*!
 * \file  vec_bcd_ppc.h
 * \brief Header package containing a collection of Binary Coded
 * Decimal (<B>BCD</B>) and Zoned Character computation operations
 * on vector registers.
 *
 * Many of these operations are implemented in a single VMX or DFP
 * instruction on newer (POWER8/POWER9) processors.
 * This header serves to fill in functional gaps for older
 * (POWER7, POWER8) processors (using existing VMX, VSX, and DFP
 * instructions) and provides in-line assembler
 * implementations for older compilers that do not
 * provide the built-ins.
 *
 * <A HREF="https://en.wikipedia.org/wiki/Binary-coded_decimal">
 * Binary-coded decimal</A> (Also called <I>packed decimal</I>) and the
 * related <I>Zoned Decimal</I> are common representations of signed
 * decimal radix (base 10) numbers. BCD is more compact and usually
 * faster then zoned. Zoned format is more closely aligned
 * with human readable and printable character formats.
 * In both formats the sign indicator is associated (in the same
 * character or byte) with the low order digit.
 *
 * BCD and Zoned formats and operations were implemented for some of
 * the earliest computers. Then circuitry was costly and arithmetic
 * was often implemented as a digit (or bit) serial operation.
 * Modern computers have more circuitry with wider data paths and more
 * complex arithmetic/logic units. The current trend is for each
 * processor core implementation to include multiple computational
 * units that can operate in parallel.
 *
 * For POWER server class processors separate and multiple Fixed-Point
 * Units (FXU), (binary) Floating-point Units (FPU), and Vector
 * Processing Units (VPU) are the norm.
 * POWER6 introduced a Decimal Floating-point (<I>DFP</I>) Facility
 * implementing the
 * <A HREF="https://en.wikipedia.org/wiki/IEEE_754-2008_revision">
 * IEEE 754-2008 revision</A> standard. This is implemented in hardware
 * as an independent Decimal Floating-point Unit (<I>DFU</I>). This
 * is supported with ISO C/C++ language bindings and runtime libraries.
 *
 * The DFU supports a different data format
 * <A HREF="https://en.wikipedia.org/wiki/Densely_packed_decimal">
 * Densely packed decimal</A> (<I>DPD</I> and a more extensive set of
 * operations then BCD or Zoned. So DFP and the comprehensive C
 * language and runtime library support makes it a better target for
 * new business oriented applications.
 * As DFP is supported directly in the hardware and has extensive
 * language and runtime support, there is little that PVECLIB can
 * contribute to general decimal radix computation.
 *
 * \note BCD and DFP support requires at least PowerISA 2.05 (POWER6)
 * or later server level processor support.
 *
 * However the vector unit and recent BCD and Zoned extensions can
 * still be useful in areas include large order multiple precision
 * computation and conversions between binary and decimal radix.
 * Both are required to convert large decimal numeric or floating-point
 * values with extreme exponents for input or print.
 * And conventions between _Float128 and _Decimal128 types is even
 * more challenging. Basically both POSIX and IEEE 754-2008 require
 * that it possible to convert floating-point values to an external
 * character decimal representation, with the specified rounding, and
 * back recovering the original value.
 * This always requires more precision for the conversion then is
 * available in the given format and size.
 *
 * So what are the operations we need, what does the PowerISA provide,
 * and what does the ABI and/or compiler provide.
 * Some useful operations include:
 * - conversions between BCD and __int128
 *   - As intermediate step between external decimal/_Decimal128 and _Float128
 * - Conversions between BCD and Zoned (character)
 * - Conversions between BCD and DFP
 * - BCD add/subtract with carry/extend
 * - BCD compare equal, greater than, less than
 * - BCD copy sign and set bool from sign
 * - BCD digit shift left/right
 * - BCD multiply/divide
 *
 * The original VMX (AKA Altivec) only defined a few instructions that
 * operated on the 128-bit vector as a whole.
 * This included the vector shifts by bit and octet, and generalized
 * vector permute, general binary integer add, subtract and multiply
 * for byte/halfword/word. But no BCD or decimal character operations.
 *
 * POWER6 introduced the Decimal Floating-point Facility. DFP provides
 * a robust set of operations with 7 (_Decimal32), 16 (_Decimal64),
 * and 34 (_Decimal128) digit precision.
 * Arithmetic operations include add, subtract, multiply, divide,
 * and compare. Special operations insert/extract exponent, quantize,
 * and digit shift. Conversions to and from signed (31-digits)
 * and unsigned (32-digit) BCD. And conversions to and from binary
 * signed long (64-bit) integer.
 * DFP operations use the existing floating-point registers (FPRs).
 * The 128-bit DFP (quadword) instructions operate on even/odd
 * 64-bit Floating-point register pairs (FPRp).
 *
 * POWER6 also implemented the Vector Facility (VMX) instructions.
 * No additional vectors operations where added and the Vector
 * Registers (VRs) where separate from the GRPs and FPRs. The only
 * transfer data path between register sets is via storage.
 * So while the DFU could be used for BCD operations and conversions,
 * there was little synergy with the vector unit, in POWER6.
 *
 * POWER7 introduced the VSX facility providing 64x128-bit Vector
 * Scalar Registers (VSRs) that overlaid both the FPRs (VSRs 0-31) and
 * VRs (VSRs 32-63). It also added useful doubleword permute immediate
 * (xxpermdi) and logical/select operations with access to all 64 VSRs.
 * This greatly simplifies data transfers between VRs and FPRs (FPRps)
 * (see vec_pack_Decimal128(), vec_unpack_Decimal128()).
 * This makes it more practical to transfer vector contents to the DPU
 * for processing (see vec_BCD2DFP() and vec_DFP2BCD().
 *
 * \note All the BCD instructions and the quadword binary add/subtract
 * are defined as vector class and can only access vector registers
 * (VSRs 32-63). The DFP instructions can only access FPRs (VSRs 0-31).
 * So only a VSX instruction (like xxpermdi) can perform the transfer
 * without going through storage.
 *
 * POWER8 added vector add/subtract modulo/carry/extend unsigned
 * quadword for binary integer (vector [unsigned] __int128).
 * This combined with the wider (word) multiply greatly enhances
 * multiple precision operations on large (> 128-bit) binary numbers.
 * POWER8 also added signed BCD add/subtract instructions with up to
 * 31-digits.
 * While the PowerISA did not provide carry/extend forms of
 * bcdadd/bcdsub, it does set a condition code with bits for
 * GT/LT/EQ/OVF. This allows for implementations of BCD compare
 * and the overflow (OVF) bit supports carry/extend operations.
 * Also the lack of BCD multiply/divide in the vector unit is not a
 * problem because we can leverage DFP
 * (see vec_bcdmul(), vec_bcddiv()).
 *
 * POWER9 (PowerISA 3.0B) adds BCD copy sign, set sign, shift, round,
 * and truncate instructions. There are also unsigned (32-digit) forms
 * of the shift and truncate instructions. And instructions to convert
 * between signed BCD and quadword (__int128) and signed BCD and Zoned.
 * POWER9 also added quadword binary multiply 10 with carry extend
 * forms than can also help with decimal to binary conversion.
 *
 * \note The compiler disables associated <altivec.h> built-ins if the
 * <B>mcpu</B> target does not enable the specific instruction.
 * For example if you compile with <B>-mcpu=power7</B>,  __builtin_bcdadd and
 * __builtin_bcdsub are not supported.  But vec_bcdadd() is always defined in
 * this header, will generate the minimum code, appropriate for the
 * target, and produce correct results.
 *
 * This header covers operations that are either:
 *
 * - Operations implemented in hardware instructions for later
 * processors and useful to programmers, on slightly older processors,
 * even if the equivalent function requires more instructions.
 * Examples include quadword BCD add and subtract.
 * - Defined in the OpenPOWER ABI but <I>not</I> yet defined in
 * <altivec.n> or <bcd.h> provided by available compilers in common use.
 * Examples include bcd_add, bcd_cmpg and bcd_mul.
 * - Commonly used operations, not covered by the ABI or
 * <altivec.h>, and require multiple instructions or
 * are not obvious.  Examples include vec_pack_Decimal128() and
 * vec_unpack_Decimal128().
 *
 * See \ref mainpage_sub_1_3 for more background on extended quadword
 * computation.
 *
 * \section bcd128_endian_issues_0_0 Endian problems with quadword implementations
 *
 * Technically, operations on quadword elements should not require any
 * endian specific transformation. There is only one element so there
 * can be no confusion about element numbering or order. However
 * some of the more complex quadword operations are constructed from
 * operations on smaller elements. And those operations as provided by
 * <altivec.h> are required by the OpenPOWER ABI to be endian sensitive.
 * See \ref i64_endian_issues_0_0 for a more detailed discussion.
 *
 * In any case, the arithmetic (high to low) order of digit nibbles in
 * BCD or characters in Zoned are defined in the PowerISA.
 * In the vector register, high order digits are on the left
 * while low order digits and the sign are on the right.
 * (See vec_bcdadd() and vec_bcdsub()).
 * So pveclib implementations will need to either:
 * - Nullify little endian transforms of <altivec.h> operations.
 * The <altivec.h> built-ins vec_mule(), vec_mulo(),
 * and vec_pack() are endian sensitive and often require
 * nullification that restores the original operation.
 * - Use new operations that are specifically defined to be stable
 * across BE/LE implementations. The pveclib operations; vec_vmuleud()
 * and vec_mulubm() are defined to be endian stable.
 *
 * \section bcd128_extended_0_0 Extended Precision computation with BCD
 *
 * \todo Extended precision requires carry and extend forms of
 * bcdadd/sub and double quadword (62-digit) results for bcdmul.
 *
 * The PowerISA does not provide the extend and write-carry forms of
 * the bcdadd/sub instructions. But bcdadd/sub instructions do post
 * status to CR field 6 which includes:
 * - Result is less than zero (CR.bit[56])
 * - Result is greater than zero (CR.bit[57])
 * - Result is equal to zero (CR.bit[58])
 * - Result overflowed (CR.bit[59])
 *
 * which provides a basis for BCD comparison and the overflow may be
 * used for carry/extend logic. The GCC compiler provides built-ins to
 * generated the bcdadd/sub and test the resulting CR bits in if
 * statements.
 *
 * Unfortunately the Overflow flag generated by bcdadd/bcdsub, is not
 * a true carry/borrow. If the operands have the same sign for bcdadd
 * (different sign for bcdsub)
 * and there is a carry out of the high order digit, then:
 * - The sum is truncated to the low order 31 digits
 * - The sign code matches the operand signs
 * - The overflow flag (CR.bit[59]) is set.
 *
 * \note overflow is only set in conjunction with greater than zero
 * (positive) or less than zero (negative) results. This implies that
 * BCD carries are tri-state; +1, 0, or -1.
 *
 * This can be used to simulate a <B>Add and write-Carry</B> operation.
 * However if the operands have different signs the bcdadd
 * (same sign for bcdsub) the operation does the following:
 * - The smaller magnitude is subtracted from the larger magnitude.
 * - The sign code matches the sign of the larger magnitude.
 * - The ox_flag is NOT set.
 *
 * For a simple BCD add this is the desired result (overflow is
 * avoided and the borrow is recorded in the sign).
 * But for multiple precision BCD operation, this will delay
 * propagation of borrows to the higher order digits and the result a
 * mixture of signs across elements of the larger multiple precision
 * value. This would have to be corrected at some later stage.
 * For example the sum of 32 digits:
 * \code
 *   21000000000000000000000000000008
 * + 19000000000000000000000000000008
 * = 40000000000000000000000000000016
 * \endcode
 * This exceeds the 31-digit capacity of Vector signed BCD so we are
 * forced to represent each number as two or more BCD values.
 * For example:
 * \code
 *   0000000000000000000000000000002c 1000000000000000000000000000008c
 * + 0000000000000000000000000000001c 9000000000000000000000000000008c
 * = 0000000000000000000000000000004c 0000000000000000000000000000016c
 * \endcode
 * The sum of the low order operands will overflow, so we need to
 * detect this overflow and generate a carry that we can apply to
 * sum of the high order operands. For example the following code using
 * the GCC's __builtin_bcdadd_ov.
 * \code
static inline vBCD_t
vec_bcdaddcsq (vBCD_t a, vBCD_t b)
{
  vBCD_t c, sum_ab;
  c = _BCD_CONST_ZERO;
  // compute the sum of (a + b)
  sum_ab = (vBCD_t) __builtin_bcdadd ((vi128_t) a, (vi128_t) b, 0);;
  // Detect the overflow, which should be rare
  if (__builtin_expect (__builtin_bcdadd_ov ((vi128_t) a, (vi128_t) b, 0), 0))
    {
      // use copysign to generate a carry based on the sign of the sum_ab
      c = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, sum_ab);
    }
  return (c);
}
 * \endcode
 * \code
 *   0000000000000000000000000000002c   1000000000000000000000000000008c
 *   0000000000000000000000000000001c + 9000000000000000000000000000008c
 *                                    =
 *                                 1c   0000000000000000000000000000016c
 * + 0000000000000000000000000000002c
 * + 0000000000000000000000000000001c
 *
 * = 0000000000000000000000000000004c
 * \endcode
 * The higher operands requires a 3-way (a+b+c) sum to propagate the
 * carry.
 * \code
static inline vBCD_t
vec_bcdaddesqm (vBCD_t a, vBCD_t b, vBCD_t c)
{
  vBCD_t t;
  t = vec_bcdadd (vec_bcdadd (a, b), c);
  return (t);
}
 * \endcode
 * where vec_bcdadd is a pveclib wrapper around __builtin_bcdadd to
 * simplify the code. The simplified multiple precision BCD use case
 * looks like this:
 * \code
  // r_h|r_l = a_h|a_l + b_h|b_l
  r_l = vec_bcdadd (a_l, b_l);
  c_l = vec_bcdaddcsq (a_l, b_l);
  r_h = vec_bcdaddesqm (a_h, b_h, c_l)
 * \endcode
 *
 * But we should look at some more example before we assume we have a
 * complete solution. For example a subtract that requires a borrow:
 * \code
 *   21000000000000000000000000000008
 * - 19000000000000000000000000000008
 * = 02000000000000000000000000000000
 * \endcode
 * The multiple precision BCD would look like this:
 * \code
 *   0000000000000000000000000000002c 1000000000000000000000000000008c
 * + 0000000000000000000000000000001d 9000000000000000000000000000008d
 * \endcode
 * But the the example coide above we do expected result:
 * \code
 * = 0000000000000000000000000000000c 2000000000000000000000000000000c
 * \endcode
 * instead we see:
 * \code
 * = 0000000000000000000000000000001c 8000000000000000000000000000000d
 * \endcode
 *
 * The BCD overflow flag only captures carry/borrow when the bcdadd
 * operands have the same sign (or different signs for bcdsub).
 * In this case it looks like (1 - 9 = -8) which does not overflow.
 * \code
 *   0000000000000000000000000000002c   1000000000000000000000000000008c
 *   0000000000000000000000000000001d + 9000000000000000000000000000008d
 *                                    =
 *                                 0c   8000000000000000000000000000000d
 * + 0000000000000000000000000000002c
 * + 0000000000000000000000000000001d
 *
 * = 0000000000000000000000000000001c
 * \endcode
 * We need a way to detect the borrow and fix up the sum to look like
 * (11 - 9 = 2) and generate a carry digit (-1) to propagate the borrow
 * to the higher order digits.
 *
 * The secondary borrow is detected by comparing the sign of the
 * result to the sign of the first operand. Something like this:
 * \code
      t = _BCD_CONST_ZERO;
      sign_ab = vec_bcdcpsgn (sum_ab, a);
      if (!vec_all_eq(sign_ab, sum_ab))
	{
	// Borrow fix-up code
	}
 * \endcode
 * For multiple precision operations it would be better to retain the
 * sign from the first operand and generate a borrow digit (value of
 * '1' with the sign of the uncorrected result).
 *
 * This requires re-computing the sum/difference, while applying the
 * effect of borrow, and replacing the carry (currently 0) with a
 * signed borrow digit.
 * The corrected sum is the 10's complement (9's complement +1) of the
 * initial sum (like (10 - 8 = 2) or (9 - 8 + 1 = 2).
 * As we obviously don't know how to represent signed BCD with more
 * then 31-digits (10**32 is 32-digits), the 9's complement + 1 is
 * a better plan.
 * We know that initial sum has a different sign from
 * the original first operand. So adding 10**31 with the sign of the
 * first operand to the initial sum applies the borrow operation.
 *
 * \code
      c = _BCD_CONST_ZERO;
      sign_ab = vec_bcdcpsgn (sum_ab, a);
      if (!vec_all_eq(sign_ab, sum_ab))
	{
	  // 10**31 with the original sign of the first operand
	  vBCD_t nines = vec_bcdcpsgn (_BCD_CONST_PLUS_NINES, a);
	  vBCD_t c10s  = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, a);
	  // Generate the Borrow digit from the initial sum
	  c = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, sum_ab);
	  // Invert the sum using the 10s complement
	  sum_ab = vec_bcdaddesqm (nines, sum_ab, c10s);
	}
 * \endcode
 * \code
 *   0000000000000000000000000000002c   1000000000000000000000000000008c
 *   0000000000000000000000000000001d + 9000000000000000000000000000008d
 *                                    = ?
 *                                      8000000000000000000000000000000d
 *                                    + 9999999999999999999999999999999c
 *                                    + 0000000000000000000000000000001c
 *                                 1d   2000000000000000000000000000000c
 * + 0000000000000000000000000000002c
 * + 0000000000000000000000000000001d
 *
 * = 0000000000000000000000000000000c
 * \endcode
 *
 * This does not fit well into the separate
 * <I>add modulo</I> and <I>add and write-carry</I> operations commonly
 * used for fixed binary arithmetic.
 * Instead it requires a combined operation returning both the generated
 * borrow and a sum/difference result with a corrected sign code.
 * The combined add with carry looks like this:
 * \code
static inline vBCD_t
vec_cbcdaddcsq (vBCD_t *cout, vBCD_t a, vBCD_t b)
{
  vBCD_t t, c;
  vBCD_t sum_ab, sign_a, sign_ab;

  sum_ab = vec_bcdadd (a, b);
  if (__builtin_expect (__builtin_bcdadd_ov ((vi128_t) a, (vi128_t) b, 0), 0))
    {
      c = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, sum_ab);
    }
  else // (a + b) did not overflow, but did it borrow?
    {
      c = _BCD_CONST_ZERO;
      sign_ab = vec_bcdcpsgn (sum_ab, a);
      if (!vec_all_eq(sign_ab, sum_ab))
	{
	  // 10**31 with the original sign of the first operand
	  vBCD_t nines = vec_bcdcpsgn (_BCD_CONST_PLUS_NINES, a);
	  vBCD_t c10s  = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, a);
	  // Generate the Borrow digit from the initial sum
	  c = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, sum_ab);
	  // Invert the sum using the 10s complement
	  sum_ab = vec_bcdaddesqm (nines, sum_ab, c10s);
	}
    }
  *cout = c;
  return (sum_ab);
}
 * \endcode
 * and the usage example looks like this:
 * \code
  // r_h|r_l = a_h|a_l + b_h|b_l
  r_l = vec_cbcdaddcsq (&c_l, a_l, b_l);
  r_h = vec_bcdaddesqm (a_h, b_h, c_l)
 * \endcode
 *
 * \section bcd128_perf_0_0 Performance data.
 * High level performance estimates are provided as an aid to function
 * selection when evaluating algorithms. For background on how
 * <I>Latency</I> and <I>Throughput</I> are derived see:
 * \ref perf_data
 *
 */

/*! \brief vector signed BCD integer of up to 31 decimal digits.
 *  \note Currently the GCC implementation and the
 *  <A HREF="https://openpowerfoundation.org/?resource_lib=64-bit-elf-v2-abi-specification-power-architecture">
 *  OpenPOWER ELF V2 ABI</A>
 *  disagree on the vector type
 *  (vector __int128 vs vector unsigned char) used
 *  (parameters and return values) by the BCD built-ins.
 *  Using vBCD_t insulates <B>pveclib</B> and
 *  applications while this is worked out.
 */
#define vBCD_t vui32_t

/*! \brief vector signed BCD constant +9s. */
#define _BCD_CONST_PLUS_NINES  ((vBCD_t) CONST_VINT128_DW128(0x9999999999999999, 0x999999999999999c))
/*! \brief vector signed BCD constant +1. */
#define _BCD_CONST_PLUS_ONE  ((vBCD_t) CONST_VINT128_DW128(0, 0x1c))
/*! \brief vector signed BCD constant -1. */
#define _BCD_CONST_MINUS_ONE ((vBCD_t) CONST_VINT128_DW128(0, 0x1d))
/*! \brief vector signed BCD constant +0. */
#define _BCD_CONST_ZERO  ((vBCD_t) CONST_VINT128_DW128(0, 0x0c))
/*! \brief vector BCD sign mask in bits 124:127. */
#define _BCD_CONST_SIGN_MASK  ((vBCD_t) CONST_VINT128_DW128(0, 0xf))

///@cond INTERNAL
static inline vBCD_t vec_bcdcpsgn (vBCD_t vra, vBCD_t vrb);
static inline vui128_t vec_bcdctuq (vBCD_t vra);
static inline vBCD_t vec_bcdsrqi (vBCD_t vra, const unsigned int _N);
static inline vBCD_t vec_bcdus (vBCD_t vra, vi8_t vrb);
static inline vf64_t vec_pack_Decimal128 (_Decimal128 lval);
static inline _Decimal128 vec_quantize0_Decimal128 (_Decimal128 val);
static inline vui8_t vec_rdxcfzt100b (vui8_t zone00, vui8_t zone16);
static inline vui8_t vec_rdxct100b (vui8_t vra);
static inline vui16_t vec_rdxct10kh (vui8_t vra);
static inline vui32_t vec_rdxct100mw (vui16_t vra);
static inline vui64_t vec_rdxct10E16d (vui32_t vra);
static inline vui128_t vec_rdxct10e32q (vui64_t vra);
static inline vb128_t vec_setbool_bcdsq (vBCD_t vra);
static inline int vec_signbit_bcdsq (vBCD_t vra);
static inline _Decimal128 vec_unpack_Decimal128 (vf64_t lval);
///@endcond

/** \brief Convert a Vector Signed BCD value to __Decimal128
 *
 * The BCD vector is permuted into a double float pair before
 * conversion to DPD format via the DFP Encode BCD To DPD Quad
 * instruction.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |  17   | 1/cycle  |
 * |power9   |  15   | 1/cycle  |
 *
 * @param val a 128-bit vector treated as a signed BCD 31 digit value.
 * @return a __Decimal128 in a double float pair.
 */
static inline _Decimal128
vec_BCD2DFP (vBCD_t val)
{
#ifdef _ARCH_PWR7
  _Decimal128 t;
#if (__GNUC__ < 5)
  __asm__(
      "xxpermdi %0,%x1,%x1,0b00;\n"
      "\txxpermdi %L0,%x1,%x1,0b10;\n"
      "\tdenbcdq 1,%0,%0;\n"
      : "=&d" (t)
      : "v" (val)
      : );
#else
  t = vec_unpack_Decimal128 ((vf64_t) val);
  t = __builtin_denbcdq (1, t);
#endif
  return (t);
#else
  // needs work for P6 without xxpermdi
  __VEC_U_128 t, x;
  x.vx4 = val;
  t.dpd128 = __builtin_denbcdq (1, x.dpd128);
  return (t.dpd128);
#endif
}

/** \brief Convert a __Decimal128 value to Vector BCD
 *
 * The _Decimal128 value is converted to a signed BCD 31 digit value
 * via "DFP Decode DPD To BCD Quad".  The conversion result is still
 * in a double float register pair and so is permuted into single
 * vector register for use.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |  17   | 1/cycle  |
 * |power9   |  15   | 1/cycle  |
 *
 * @param val a __Decimal128 in a double float pair.
 * @return a 128-bit vector treated a signed BCD 31 digit value.
 */
static inline vBCD_t
vec_DFP2BCD (_Decimal128 val)
{
#ifdef _ARCH_PWR7
  vBCD_t t;
  _Decimal128 x;
#if (__GNUC__ < 5)
  __asm__(
      "ddedpdq 2,%1,%2;\n"
      "\txxpermdi %x0,%1,%L1,0b00;\n"
      : "=v" (t),
      "=&d" (x)
      : "d" (val)
      : );
#else
  x = __builtin_ddedpdq (2, val);
  t = (vBCD_t) vec_pack_Decimal128 (x);
#endif
  return (t);
#else
  // needs work for P6 without xxpermdi
  __VEC_U_128 t, x;
  t.dpd128 = __builtin_ddedpdq (1, val);
  return (t.vx4);
#endif
}

/** \brief Decimal Add Signed Modulo Quadword.
 *
 * Two Signed 31 digit values are added and lower 31 digits of the
 * sum are returned.  Overflow (carry-out) is ignored.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |  13   | 1/cycle  |
 * |power9   |   3   | 2/cycle  |
 *
 * @param a a 128-bit vector treated as a signed BCD 31 digit value.
 * @param b a 128-bit vector treated as a signed BCD 31 digit value.
 * @return a 128-bit vector which is the lower 31 digits of (a + b).
 */
static inline vBCD_t
vec_bcdadd (vBCD_t a, vBCD_t b)
{
  vBCD_t t;
#ifdef _ARCH_PWR8
#if (__GNUC__ < 7)
  __asm__(
      "bcdadd. %0,%1,%2,0;\n"
      : "=v" (t)
      : "v" (a),
      "v" (b)
      : "cr6" );
#else
  t = (vBCD_t) __builtin_bcdadd ((vi128_t) a, (vi128_t) b, 0);
#endif
#else
  const vui32_t mz = CONST_VINT128_W (0, 0, 0, 0x0000000d);
  _Decimal128 d_t;
  d_t = vec_BCD2DFP (a) + vec_BCD2DFP (b);
  t = vec_DFP2BCD(d_t);
  // fix up spurious negative zeros
  if (vec_all_eq((vui32_t) t, mz))
    t = _BCD_CONST_ZERO;
#endif
  return (t);
}

/** \brief Decimal Add & write Carry Signed Quadword.
 *
 * Two Signed 31 digit BCD values are added, and the carry-out
 * (the high order 32nd digit) of the sum is returned.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |  13   | 1/cycle  |
 * |power9   | 6-15  | 2/cycle  |
 *
 * @param a a 128-bit vector treated as a signed BCD 31 digit value.
 * @param b a 128-bit vector treated as a signed BCD 31 digit value.
 * @return a 128-bit vector with the carry digit.
 * Values are -1, 0, and +1.
 */
static inline vBCD_t
vec_bcdaddcsq (vBCD_t a, vBCD_t b)
{
  vBCD_t t;
#if defined ( _ARCH_PWR8) && (__GNUC__ > 6)
  t = _BCD_CONST_ZERO;
  if (__builtin_expect (__builtin_bcdadd_ov ((vi128_t) a, (vi128_t) b, 0), 0))
    {
#ifdef _ARCH_PWR9
      t = (vBCD_t) __builtin_bcdadd ((vi128_t) a, (vi128_t) b, 0);
      t = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, t);
#else
      if (__builtin_bcdadd_gt ((vi128_t) a, (vi128_t) b, 0))
      t = _BCD_CONST_PLUS_ONE;
      else
      t = _BCD_CONST_MINUS_ONE;
#endif
    }
  else
    {
#ifdef _ARCH_PWR9
      // Generate BCD zero from (a - a), which is 3 cycles on PWR9
      t = (vBCD_t) __builtin_bcdsub ((vi128_t) a, (vi128_t) a, 0);
#else // Else load a BCD const 0.
      t = _BCD_CONST_ZERO;
#endif
    }
#else
  _Decimal128 d_a, d_b, d_s, d_t;
  const vui32_t mz = CONST_VINT128_W (0, 0, 0, 0x0000000d);
  d_a = vec_BCD2DFP (a);
  d_b = vec_BCD2DFP (b);
  d_s = d_a + d_b;
  // Shift right 31 digits, leaving the carry.
  d_t = __builtin_dscriq (d_s, 31);
  t = vec_DFP2BCD (d_t);
  // fix up spurious negative zeros
  if (vec_all_eq((vui32_t) t, mz))
    t = _BCD_CONST_ZERO;
#endif
  return (t);
}

/** \brief Decimal Add Extended & write Carry Signed Quadword.
 *
 * Two Signed 31 digit values and a signed carry-in are added together
 * and the carry-out (the high order 32nd digit) of the sum is
 * returned.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |  13   | 1/cycle  |
 * |power9   | 9-18  | 2/cycle  |
 *
 * @param a a 128-bit vector treated as a signed BCD 31 digit value.
 * @param b a 128-bit vector treated as a signed BCD 31 digit value.
 * @param c a 128-bit vector treated as a signed BCD carry with values
 * -1, 0, or +1.
 * @return a 128-bit vector with the carry digit from the sum
 * (a + b +c). Carry values are -1, 0, and +1.
 */
static inline vBCD_t
vec_bcdaddecsq (vBCD_t a, vBCD_t b, vBCD_t c)
{
  vBCD_t t;
#ifdef _ARCH_PWR8
  vBCD_t a_b;
  a_b = vec_bcdadd (a, b);

  if (__builtin_expect (__builtin_bcdadd_ov ((vi128_t) a, (vi128_t) b, 0), 0))
    {
#ifdef _ARCH_PWR9
      t = (vBCD_t) __builtin_bcdadd ((vi128_t) a, (vi128_t) b, 0);
      t = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, t);
#else
      if (__builtin_bcdadd_gt ((vi128_t) a, (vi128_t) b, 0))
	t = _BCD_CONST_PLUS_ONE;
      else
	t = _BCD_CONST_MINUS_ONE;
#endif
    }
  else // (a + b) did not overflow, what about (a + b + c)
    {
      if (__builtin_bcdadd_ov ((vi128_t) a_b, (vi128_t) c, 0))
	{
#ifdef _ARCH_PWR9
	  t = (vBCD_t) __builtin_bcdadd ((vi128_t) a_b, (vi128_t) c, 0);
	  t = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, t);
#else
	  if (__builtin_bcdadd_gt ((vi128_t) a_b, (vi128_t) c, 0))
	    t = _BCD_CONST_PLUS_ONE;
	  else
	    t = _BCD_CONST_MINUS_ONE;
#endif
	}
      else
	{
#ifdef _ARCH_PWR9
	  // Generate BCD zero from (a - a), which is 3 cycles on PWR9
	  t = (vBCD_t) __builtin_bcdsub ((vi128_t) a, (vi128_t) a, 0);
#else // Else load a BCD const 0.
	  t = _BCD_CONST_ZERO;
#endif
	}
    }
#else
  const vui32_t mz = CONST_VINT128_W (0, 0, 0, 0x0000000d);
  _Decimal128 d_a, d_b, d_c, d_s, d_t;
  d_a = vec_BCD2DFP (a);
  d_b = vec_BCD2DFP (b);
  d_c = vec_BCD2DFP (c);
  d_s = d_a + d_b + d_c;
  // Shift right 31 digits, leaving the carry.
  d_t = __builtin_dscriq (d_s, 31);
  t = vec_DFP2BCD (d_t);
  // fix up spurious negative zeros
  if (vec_all_eq((vui32_t) t, mz))
    t = _BCD_CONST_ZERO;
#endif
  return (t);
}

/** \brief Decimal Add Extended Signed Modulo Quadword.
 *
 * Two Signed 31 digit values and a signed carry-in are added together
 * and lower 31 digits of the sum are returned.  Overflow (carry-out)
 * is ignored.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |  13   | 1/cycle  |
 * |power9   |   6   | 2/cycle  |
 *
 * @param a a 128-bit vector treated as a signed BCD 31 digit value.
 * @param b a 128-bit vector treated as a signed BCD 31 digit value.
 * @param c a 128-bit vector treated as a signed BCD carry with values
 * -1, 0, or +1.
 * @return a 128-bit vector which is the lower 31 digits of (a + b + c).
 */
static inline vBCD_t
vec_bcdaddesqm (vBCD_t a, vBCD_t b, vBCD_t c)
{
  vBCD_t t;
#ifdef _ARCH_PWR8
  t = vec_bcdadd (vec_bcdadd (a, b), c);
#else
  _Decimal128 d_t;
  d_t = vec_BCD2DFP (a) + vec_BCD2DFP (b) + vec_BCD2DFP (c);
  t = vec_DFP2BCD(d_t);
#endif
  return (t);
}

/** \brief Vector copy sign BCD.
 *
 * Given Two Signed BCD 31 digit values vra and vrb, return the
 * magnitude from vra (bits 0:123) and the sign (bits 124:127)
 * from vrb.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   | 2-11  | 1/cycle  |
 * |power9   |   3   | 2/cycle  |
 *
 * @param vra a 128-bit vector treated as a signed BCD 31 digit value.
 * @param vrb a 128-bit vector treated as a signed BCD 31 digit value.
 * @return a 128-bit BCD value with the magnitude from vra and the
 * sign copied from  vrb.
 */
static inline vBCD_t
vec_bcdcpsgn (vBCD_t vra, vBCD_t vrb)
{
  vBCD_t vrt;
#ifdef _ARCH_PWR9
  __asm__(
      "bcdcpsgn. %0,%1,%2;\n"
      : "=v" (vrt)
      : "v" (vra),
      "v" (vrb)
      : "cr6" );
#else
  const vui8_t sign_mask = (vui8_t) _BCD_CONST_SIGN_MASK;
  vrt = (vBCD_t) vec_sel ((vui8_t) vra, (vui8_t) vrb, sign_mask);
#endif
  return (vrt);
}

/** \brief Vector Decimal Convert to Signed Quadword.
 *
 *  Vector convert a BCD quadword containing signed 31 digits values
 *  to signed __int128, in the range
 *  +/- 0-9999999999999999999999999999999.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 80-95 | 1/cycle  |
 *  |power9   |  23   | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as a signed 31-digit BCD
 *  number.
 *  @return 128-bit vector signed __int128 in
 *  the range +/- 0-9999999999999999999999999999999.
 */
static inline vi128_t
vec_bcdctsq (vBCD_t vra)
{
  vui128_t result;
#ifdef _ARCH_PWR9
  __asm__(
      "bcdctsq. %0,%1;\n"
      : "=v" (result)
      : "v" (vra)
      : "cr6" );
#else
  const vui128_t zero = (vui128_t) vec_splats ((int) 0);
  vBCD_t ubcd;

  ubcd = (vBCD_t) vec_srqi ((vui128_t)vra, 4);
  result = vec_bcdctuq (ubcd);
  if (vec_signbit_bcdsq (vra))
    result = vec_subuqm (zero, result);
#endif
  return (vi128_t) result;
}

/** \brief Vector Decimal Convert Binary Coded Decimal (BCD) digit
 *  pairs to binary unsigned bytes .
 *
 *  Vector convert 16 bytes each containing 2 BCD digits to the
 *  equivalent unsigned char, in the range 0-99.
 *  Input values should be valid 2 x BCD nibbles in the range 0-9.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 13-22 | 1/cycle  |
 *  |power9   | 14-23 | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as 16 unsigned 2-digit BCD
 *  numbers.
 *  @return 128-bit vector unsigned char, For each byte in the range
 *  0-99.
 */
static inline vui8_t
vec_bcdctub (vBCD_t vra)
{
  return vec_rdxct100b ((vui8_t) vra);
}

/** \brief Vector Decimal Convert groups of 4 BCD digits
 *  to binary unsigned halfwords.
 *
 *  Vector convert 8 halfwords each containing 4 BCD digits to the
 *  equivalent unsigned short, in the range 0-9999.
 *  Input values should be valid 4 x BCD nibbles in the range 0-9.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 22-31 | 1/cycle  |
 *  |power9   | 23-32 | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as 8 unsigned 4-digit BCD
 *  numbers.
 *  @return 128-bit vector unsigned short, For each halfword in the
 *  range 0-9999.
 */
static inline vui16_t
vec_bcdctuh (vBCD_t vra)
{
  vui8_t d100;
  d100 = vec_rdxct100b ((vui8_t) vra);
  return vec_rdxct10kh (d100);
}

/** \brief Vector Decimal Convert groups of 8 BCD digits
 *  to binary unsigned words.
 *
 *  Vector convert 4 words each containing 8 BCD digits to the
 *  equivalent unsigned int, in the range 0-99999999.
 *  Input values should be valid 8 x BCD nibbles in the range 0-9.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 31-42 | 1/cycle  |
 *  |power9   | 32-43 | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as 4 unsigned 8-digit BCD
 *  numbers.
 *  @return 128-bit vector unsigned int, For each word in the
 *  range 0-99999999.
 */
static inline vui32_t
vec_bcdctuw (vBCD_t vra)
{
  vui8_t d100;
  vui16_t d10k;
  d100 = vec_rdxct100b ((vui8_t) vra);
  d10k = vec_rdxct10kh (d100);
  return vec_rdxct100mw (d10k);
}

/** \brief Vector Decimal Convert groups of 16 BCD digits
 *  to binary unsigned doublewords.
 *
 *  Vector convert 2 doublewords each containing 16 BCD digits to the
 *  equivalent unsigned long int, in the range 0-9999999999999999.
 *  Input values should be valid 16 x BCD nibbles in the range 0-9.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 40-51 | 1/cycle  |
 *  |power9   | 41-52 | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as 2 unsigned 16-digit BCD
 *  numbers.
 *  @return 128-bit vector unsigned long int, For each doubleword in
 *  the range 0-9999999999999999.
 */
static inline vui64_t
vec_bcdctud (vBCD_t vra)
{
  vui8_t d100;
  vui16_t d10k;
  vui32_t d100m;
  d100 = vec_rdxct100b ((vui8_t) vra);
  d10k = vec_rdxct10kh (d100);
  d100m = vec_rdxct100mw (d10k);
  return vec_rdxct10E16d (d100m);
}

/** \brief Vector Decimal Convert groups of 32 BCD digits
 *  to binary unsigned quadword.
 *
 *  Vector convert quadwords each containing 32 BCD digits to the
 *  equivalent unsigned __int128, in the range
 *  0-99999999999999999999999999999999.
 *  Input values should be valid 32 x BCD nibbles in the range 0-9.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 65-78 | 1/cycle  |
 *  |power9   | 52-65 | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as an unsigned 32-digit BCD
 *  number.
 *  @return 128-bit vector unsigned __int128 in
 *  the range 0-99999999999999999999999999999999.
 */
static inline vui128_t
vec_bcdctuq (vBCD_t vra)
{
  vui8_t d100;
  vui16_t d10k;
  vui32_t d100m;
  vui64_t d10e;
  d100 = vec_rdxct100b ((vui8_t) vra);
  d10k = vec_rdxct10kh (d100);
  d100m = vec_rdxct100mw (d10k);
  d10e = vec_rdxct10E16d (d100m);
  return vec_rdxct10e32q (d10e);
}

/** \brief Divide a Vector Signed BCD 31 digit value by another BCD value.
 *
 * One Signed 31 digit value is divided by a second 31 digit value
 * and the quotient is returned.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |102-238| 1/cycle  |
 * |power9   | 96-228| 1/cycle  |
 *
 * @param a a 128-bit vector treated as a signed BCD 31 digit value.
 * @param b a 128-bit vector treated as a signed BCD 31 digit value.
 * @return a 128-bit vector which is the lower 31 digits of (a / b).
 */
static inline vBCD_t
vec_bcddiv (vBCD_t a, vBCD_t b)
{
  vBCD_t t;
  _Decimal128 d_t, d_a, d_b;
  d_a = vec_BCD2DFP (a);
  d_b = vec_BCD2DFP (b);
  d_t = vec_quantize0_Decimal128 (d_a / d_b);
  t = vec_DFP2BCD (d_t);
  return (t);
}

/** \brief Multiply two Vector Signed BCD 31 digit values.
 *
 * Two Signed 31 digit values are multiplied and the lower 31 digits
 * of the product are returned.  Overflow is ignored.
 *
 * The vector unit does not have a BCD multiply, so we convert the
 * operands to _Decimal128 format and use the DFP quadword multiply.
 * This gets tricky as the product can be up to 62 digits, and
 * _Decimal128 format can only hold 34 digits.
 *
 * To avoid overflow in the DFU, we split each BCD operand into 15
 * upper and 16 lower digit halves. This requires up to four decimal
 * multiplies and produces up to four 30-32 digit partial products.
 * These are aligned appropriately (via DFP decimal shift) and summed
 * (via DFP Decimal add) to generate the high and low (31-digit)
 * parts of the 62 digit product.
 *
 * In this case we only need the lower 31-digits of the product.
 * So only 3 (not 4) DFP multiplies are required. Also we can
 * discard any high digits above 31.
 *
 * \note There is early exit case if both operands are
 * 16-digits or less. Here the product can not exceed 32-digits and
 * requires only a single DFP multiply. The DFP2BCD conversion
 * will discard any extra (32th) digit.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   | 94-194| 1/cycle  |
 * |power9   | 88-227| 1/cycle  |
 *
 * @param a a 128-bit vector treated as a signed BCD 31 digit value.
 * @param b a 128-bit vector treated as a signed BCD 31 digit value.
 * @return a 128-bit vector which is the lower 31 digits of (a * b).
 */
static inline vBCD_t
vec_bcdmul (vBCD_t a, vBCD_t b)
{
  const vui32_t mz = CONST_VINT128_W (0, 0, 0, 0x0000000d);
  const vBCD_t dword_mask = (vBCD_t) CONST_VINT128_DW(15, -1);
  vBCD_t t, low_a, low_b, high_a, high_b;
  _Decimal128 d_p, d_t, d_a, d_b;

  low_a = vec_and (a, dword_mask);
  low_b = vec_and (b, dword_mask);
  d_a = vec_BCD2DFP (low_a);
  d_b = vec_BCD2DFP (low_b);
  d_p = d_a * d_b;
  if (__builtin_expect ((vec_cmpuq_all_eq ((vui128_t) low_a, (vui128_t) a)
      && vec_cmpuq_all_eq ((vui128_t) low_b, (vui128_t) b)), 1))
    {
      d_t = d_p;
    }
  else
    {
      _Decimal128 d_ah, d_bh, d_hl, d_lh, d_h;

      high_a = vec_bcdsrqi (a, 16);
      high_b = vec_bcdsrqi (b, 16);

      d_ah = vec_BCD2DFP (high_a);
      d_bh = vec_BCD2DFP (high_b);

      d_hl = d_ah * d_b;
      d_lh = d_a * d_bh;

      d_h = d_hl + d_lh;
      d_h = __builtin_dscliq (d_h, 17);
      d_h = __builtin_dscriq (d_h, 1);

      d_t = d_p + d_h;
    }
  t = vec_DFP2BCD (d_t);
  // fix up spurious negative zeros
#ifdef _ARCH_PWR9
  t = vec_bcdadd (t, _BCD_CONST_ZERO);
#else
  if (vec_all_eq((vui32_t) t, mz))
    t = _BCD_CONST_ZERO;
#endif
  return (t);
}

/** \brief Vector Signed BCD Multiply High.
 *
 * Two Signed 31 digit values are multiplied and the higher 31 digits
 * of the product are returned.
 *
 * The vector unit does not have a BCD multiply, so we convert the
 * operands to _Decimal128 format and use the DFP quadword multiply.
 * This gets tricky as the product can be up to 62 digits, and
 * _Decimal128 format can only hold 34 digits.
 *
 * To avoid overflow in the DFU, we split each BCD operand into 15
 * upper and 16 lower digit halves. This requires up four decimal
 * multiplies and produces four 30-32 digit partial products.
 * These are aligned appropriately (via DFP decimal shift) and summed
 * (via DFP Decimal add) to generate the high and low (31-digit)
 * parts of the 62 digit product.
 *
 * In this case we only need the upper 31-digits of the product.
 * The lower 31-digits are discarded.
 *
 * \note There is early exit case if both operands are
 * 16-digits or less. Here the product can not exceed 32-digits and
 * requires only a single DFP multiply. We use the DFP Decimal shift
 * to discard the lower 31-digits and return the single (32nd) digit.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |106-361| 1/cycle  |
 * |power9   | 99-271| 1/cycle  |
 *
 * @param a a 128-bit vector treated as a signed BCD 31 digit value.
 * @param b a 128-bit vector treated as a signed BCD 31 digit value.
 * @return a 128-bit vector which is the higher 31 digits of (a * b).
 */
static inline vBCD_t
vec_bcdmulh (vBCD_t a, vBCD_t b)
{
  const vBCD_t dword_mask = (vBCD_t) CONST_VINT128_DW(15, -1);
  const vui32_t mz = CONST_VINT128_W (0, 0, 0, 0x0000000d);
  vBCD_t t, low_a, low_b, high_a, high_b;
  _Decimal128 d_p, d_t, d_al, d_bl;

  low_a = vec_and (a, dword_mask);
  low_b = vec_and (b, dword_mask);
  d_al = vec_BCD2DFP (low_a);
  d_bl = vec_BCD2DFP (low_b);
  d_p = d_al * d_bl;
  if (__builtin_expect ((vec_cmpuq_all_eq ((vui128_t) low_a, (vui128_t) a)
      && vec_cmpuq_all_eq ((vui128_t) low_b, (vui128_t) b)), 1))
    {
      d_t = __builtin_dscriq (d_p, 31);
    }
  else
    {
      _Decimal128 d_ah, d_bh, d_hl, d_lh, d_h, d_ll, d_m;

      high_a = vec_bcdsrqi (a, 16);
      high_b = vec_bcdsrqi (b, 16);
      d_ah = vec_BCD2DFP (high_a);
      d_bh = vec_BCD2DFP (high_b);

      d_hl = d_ah * d_bl;
      d_lh = d_al * d_bh;
      d_ll = __builtin_dscriq (d_p, 16);

      d_m = d_hl + d_lh + d_ll;
      d_m = __builtin_dscriq (d_m, 15);

      d_h = d_ah * d_bh;
      d_h = __builtin_dscliq (d_h, 1);
      d_t = d_m + d_h;
    }
  t = vec_DFP2BCD (d_t);
  // fix up spurious negative zeros
#ifdef _ARCH_PWR9
  t = vec_bcdadd (t, _BCD_CONST_ZERO);
#else
  if (vec_all_eq((vui32_t) t, mz))
    t = _BCD_CONST_ZERO;
#endif
  return (t);
}

/** \brief Decimal Shift.
 * Shift a vector signed BCD value, left or right a variable
 * amount of digits (nibbles). The sign nibble is preserved.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   | 14-25 | 1/cycle  |
 * |power9   |   3   | 2/cycle  |
 *
 * @param vra 128-bit vector treated as a signed BCD 31 digit value.
 * @param vrb Digit shift count in vector byte 7.
 * @return a 128-bit vector BCD value shifted right digits.
 */
static inline vBCD_t
vec_bcds (vBCD_t vra, vi8_t vrb)
{
  vBCD_t vrt;
#ifdef _ARCH_PWR9
  __asm__(
      "bcds. %0,%1,%2,0;\n"
      : "=v" (vrt)
      : "v" (vrb),
      "v" (vra)
      : "cr6" );
#else
  const vi8_t zero = vec_splat_s8(0);
  vi8_t shd = vec_splat (vrb, VEC_BYTE_L_DWH);
  vui128_t t;
  // Multiply digit shift by 4 to get bit shift count
  shd = vec_add (shd, shd);
  shd = vec_add (shd, shd);
  // Clear sign nibble before shift.
  t = (vui128_t) vec_andc ((vui32_t) vra, (vui32_t) _BCD_CONST_SIGN_MASK);
  // Compare shift positive or negative
  if (vec_all_ge(shd, zero))
    {
      // Positive, shift left
      t = vec_slq (t, (vui128_t) shd);
    }
  else
    {
      // Negative, shift right by absolute value
      shd = vec_sub (zero, shd);
      t = vec_srq (t, (vui128_t) shd);
    }
  // restore original sign nibble
  vrt = vec_bcdcpsgn ((vBCD_t) t, vra);
#endif
  return (vrt);
}

/** \brief Vector BCD Shift Right Signed Quadword
 *
 * Shift a vector signed BCD value right _N digits.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |  6-15 | 2/cycle  |
 * |power9   |  3-6  | 2/cycle  |
 *
 * @param vra 128-bit vector signed BCD 31 digit value.
 * @param _N int constant for the number of digits to shift right.
 * @return a 128-bit vector BCD value shifted right _N digits.
 */
static inline vBCD_t
vec_bcdslqi (vBCD_t vra, const unsigned int _N)
{
  vBCD_t vrt;
#ifdef _ARCH_PWR9
  vi8_t shd = vec_splats ((const signed char) (_N));
  vrt = vec_bcds (vra, shd);
#else
  vui128_t t;

  t = (vui128_t) vec_andc ((vui32_t) vra, (vui32_t) _BCD_CONST_SIGN_MASK);
  t = vec_slqi (t, (_N*4));
  vrt = vec_bcdcpsgn ((vBCD_t) t, vra);
#endif
  return (vrt);
}

/** \brief Vector BCD Shift Right unsigned Quadword
 *
 * Shift a vector unsigned BCD value right _N digits.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |  6-15 | 2/cycle  |
 * |power9   |  3-6  | 2/cycle  |
 *
 * @param vra 128-bit vector unsigned BCD 32 digit value.
 * @param _N int constant for the number of digits to shift right.
 * @return a 128-bit vector BCD value shifted right _N digits.
 */
static inline vBCD_t
vec_bcdsluqi (vBCD_t vra, const unsigned int _N)
{
#ifdef _ARCH_PWR9
  vi8_t shd = vec_splats ((const signed char) (_N));
  return vec_bcdus (vra, shd);
#else
  return (vBCD_t) vec_slqi ((vui128_t) vra, (_N*4));
#endif
}

/** \brief Vector BCD Shift Right Signed Quadword
 *
 * Shift a vector signed BCD value right _N digits.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |  6-15 | 2/cycle  |
 * |power9   |  3-6  | 2/cycle  |
 *
 * @param vra 128-bit vector signed BCD 31 digit value.
 * @param _N int constant for the number of digits to shift right.
 * @return a 128-bit vector BCD value shifted right _N digits.
 */
static inline vBCD_t
vec_bcdsrqi (vBCD_t vra, const unsigned int _N)
{
  vBCD_t vrt;
#ifdef _ARCH_PWR9
  vi8_t shd = vec_splats ((const signed char) (-_N));
  vrt = vec_bcds (vra, shd);
#else
  vui128_t t;

  t = vec_srqi ((vui128_t) vra, (_N*4));
  vrt = vec_bcdcpsgn ((vBCD_t) t, vra);
#endif
  return (vrt);
}

/** \brief Vector BCD Shift Right Unsigned Quadword
 *
 * Shift a vector unsigned BCD value right _N digits.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |  6-15 | 2/cycle  |
 * |power9   |  3-6  | 2/cycle  |
 *
 * @param vra 128-bit vector unsigned BCD 32 digit value.
 * @param _N int constant for the number of digits to shift right.
 * @return a 128-bit vector BCD value shifted right _N digits.
 */
static inline vBCD_t
vec_bcdsruqi (vBCD_t vra, const unsigned int _N)
{
#ifdef _ARCH_PWR9
  vi8_t shd = vec_splats ((const signed char) (-_N));
  return vec_bcdus (vra, shd);
#else
  return (vBCD_t) vec_srqi ((vui128_t) vra, (_N*4));
#endif
}

/** \brief Subtract two Vector Signed BCD 31 digit values.
 *
 * Subtract Signed 31 digit values and return the lower 31 digits of
 * of the result.  Overflow (carry-out/barrow) is ignored.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |  13   | 1/cycle  |
 * |power9   |   3   | 2/cycle  |
 *
 * @param a a 128-bit vector treated a signed BCD 31 digit value.
 * @param b a 128-bit vector treated a signed BCD 31 digit value.
 * @return a 128-bit vector which is the lower 31 digits of (a - b).
 */
static inline vBCD_t
vec_bcdsub (vBCD_t a, vBCD_t b)
{
  vBCD_t t;
#ifdef _ARCH_PWR8
#if (__GNUC__ < 7)
  __asm__(
      "bcdsub. %0,%1,%2,0;\n"
      : "=v" (t)
      : "v" (a),
      "v" (b)
      : "cr6" );
#else
  t = (vBCD_t) __builtin_bcdsub ((vi128_t) a, (vi128_t) b, 0);
#endif
#else
  const vui32_t mz = CONST_VINT128_W (0, 0, 0, 0x0000000d);
  _Decimal128 d_t, d_a, d_b;
  d_a = vec_BCD2DFP (a);
  d_b = vec_BCD2DFP (b);
  d_t = d_a - d_b;
  t = vec_DFP2BCD(d_t);
#endif
  return (t);
}

/** \brief Decimal Sudtract & write Carry Signed Quadword.
 *
 * Two Signed 31 digit BCD values are subtracted, and the carry-out
 * (the high order 32nd digit) of the difference is returned.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   | 13-21 | 1/cycle  |
 * |power9   | 6-15  | 2/cycle  |
 *
 * @param a a 128-bit vector treated as a signed BCD 31 digit value.
 * @param b a 128-bit vector treated as a signed BCD 31 digit value.
 * @return a 128-bit vector with the carry digit.
 * Values are -1, 0, and +1.
 */
static inline vBCD_t
vec_bcdsubcsq (vBCD_t a, vBCD_t b)
{
  vBCD_t t;
#if defined (_ARCH_PWR8) && (__GNUC__ > 6)
  t = _BCD_CONST_ZERO;
  if (__builtin_expect (__builtin_bcdsub_ov ((vi128_t) a, (vi128_t) b, 0), 0))
    {
      if (__builtin_bcdsub_gt ((vi128_t) a, (vi128_t) b, 0))
	t = _BCD_CONST_PLUS_ONE;
      else
	t = _BCD_CONST_MINUS_ONE;
    }
#else
  const vui32_t mz = CONST_VINT128_W (0, 0, 0, 0x0000000d);
  _Decimal128 d_a, d_b, d_s, d_t;
  d_a = vec_BCD2DFP (a);
  d_b = vec_BCD2DFP (b);
  d_s = d_a - d_b;
  // Shift right 31 digits, leaving the carry.
  d_t = __builtin_dscriq (d_s, 31);
  t = vec_DFP2BCD(d_t);
  // fix up spurious negative zeros
  if (vec_all_eq((vui32_t) t, mz))
    t = _BCD_CONST_ZERO;
#endif
  return (t);
}

/** \brief Decimal Add Extended & write Carry Signed Quadword.
 *
 * Two Signed 31 digit values and a signed carry-in are added together
 * and the carry-out (the high order 32nd digit) of the sum is
 * returned.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |  39   | 1/cycle  |
 * |power9   | 9-18  | 2/cycle  |
 *
 * @param a a 128-bit vector treated as a signed BCD 31 digit value.
 * @param b a 128-bit vector treated as a signed BCD 31 digit value.
 * @param c a 128-bit vector treated as a signed BCD carry with values
 * -1, 0, or +1.
 * @return a 128-bit vector with the carry digit from the sum
 * (a + b +c). Carry values are -1, 0, and +1.
 */
static inline vBCD_t
vec_bcdsubecsq (vBCD_t a, vBCD_t b, vBCD_t c)
{
  vBCD_t t;
#ifdef _ARCH_PWR8
  vBCD_t a_b;
  a_b = vec_bcdsub (a, b);

  if (__builtin_expect (__builtin_bcdsub_ov ((vi128_t) a, (vi128_t) b, 0), 0))
    {
#ifdef _ARCH_PWR9
      t = (vBCD_t) __builtin_bcdsub ((vi128_t) a, (vi128_t) b, 0);
      t = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, t);
#else
      if (__builtin_bcdadd_gt ((vi128_t) a, (vi128_t) b, 0))
	t = _BCD_CONST_PLUS_ONE;
      else
	t = _BCD_CONST_MINUS_ONE;
#endif
    }
  else // (a - b) did not overflow, what about (a - b - c)
    {
      if (__builtin_bcdsub_ov ((vi128_t) a_b, (vi128_t) c, 0))
	{
#ifdef _ARCH_PWR9
	  t = (vBCD_t) __builtin_bcdsub ((vi128_t) a_b, (vi128_t) c, 0);
	  t = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, t);
#else
	  if (__builtin_bcdsub_gt ((vi128_t) a_b, (vi128_t) c, 0))
	    t = _BCD_CONST_PLUS_ONE;
	  else
	    t = _BCD_CONST_MINUS_ONE;
#endif
	}
      else
	{
#ifdef _ARCH_PWR9
	  // Generate BCD zero from (a - a), which is 3 cycles on PWR9
	  t = (vBCD_t) __builtin_bcdsub ((vi128_t) a, (vi128_t) a, 0);
#else // Else load a BCD const 0.
	  t = _BCD_CONST_ZERO;
#endif
	}
    }
#else
  const vui32_t mz = CONST_VINT128_W (0, 0, 0, 0x0000000d);
  _Decimal128 d_a, d_b, d_c, d_s, d_t;
  d_a = vec_BCD2DFP (a);
  d_b = vec_BCD2DFP (b);
  d_c = vec_BCD2DFP (c);
  d_s = d_a - d_b - d_c;
  // Shift right 31 digits, leaving the carry.
  d_t = __builtin_dscriq (d_s, 31);
  t = vec_DFP2BCD (d_t);
  // fix up spurious negative zeros
  if (vec_all_eq((vui32_t) t, mz))
    t = _BCD_CONST_ZERO;
#endif
  return (t);
}

/** \brief Decimal Subtract Extended Signed Modulo Quadword.
 *
 * Two Signed 31 digit values and a signed carry-in are subtracted
 * (a - b- c) and lower 31 digits of the subtraction is returned.
 * Overflow (carry-out) is ignored.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |  26   | 1/cycle  |
 * |power9   |   6   | 2/cycle  |
 *
 * @param a a 128-bit vector treated as a signed BCD 31 digit value.
 * @param b a 128-bit vector treated as a signed BCD 31 digit value.
 * @param c a 128-bit vector treated as a signed BCD carry with values
 * -1, 0, or +1.
 * @return a 128-bit vector which is the lower 31 digits of (a + b + c).
 */
static inline vBCD_t
vec_bcdsubesqm (vBCD_t a, vBCD_t b, vBCD_t c)
{
  vBCD_t t;
#ifdef _ARCH_PWR8
  t = vec_bcdsub (vec_bcdsub (a, b), c);
#else
  const vui32_t mz = CONST_VINT128_W (0, 0, 0, 0x0000000d);
  _Decimal128 d_t;
  d_t = vec_BCD2DFP (a) - vec_BCD2DFP (b) - vec_BCD2DFP (c);
  t = vec_DFP2BCD(d_t);
  // fix up spurious negative zeros
  if (vec_all_eq((vui32_t) t, mz))
    t = _BCD_CONST_ZERO;
#endif
  return (t);
}

/** \brief Decimal Unsigned Shift.
 * Shift a vector unsigned BCD value, left or right a variable
 * amount of digits (nibbles).
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   | 12-14 | 1/cycle  |
 * |power9   |   3   | 2/cycle  |
 *
 * @param vra 128-bit vector treated as a signed BCD 32 digit value.
 * @param vrb Digit shift count in vector byte 7.
 * @return a 128-bit vector BCD value shifted right digits.
 */
static inline vBCD_t
vec_bcdus (vBCD_t vra, vi8_t vrb)
{
  vBCD_t vrt;
#ifdef _ARCH_PWR9
  __asm__(
      "bcdus. %0,%1,%2;\n"
      : "=v" (vrt)
      : "v" (vrb),
      "v" (vra)
      : "cr6" );
#else
  const vi8_t zero = vec_splat_s8(0);
  vi8_t shd = vec_splat (vrb, VEC_BYTE_L_DWH);
  vui128_t t;
  // Multiply digit shift by 4 to get bit shift count
  shd = vec_add (shd, shd);
  shd = vec_add (shd, shd);
  t = (vui128_t) vra;
  // Compare shift positive or negative
  if (vec_all_ge(shd, zero))
    {
      // Positive, shift left
      t = vec_slq (t, (vui128_t) shd);
    }
  else
    {
      // Negative, shift right by absolute value
      shd = vec_sub (zero, shd);
      t = vec_srq (t, (vui128_t) shd);
    }
  vrt = (vBCD_t) t;
#endif
  return (vrt);
}

/** \brief Combined Decimal Add & write Carry Signed Quadword.
 *
 * Two Signed 31 digit BCD values are added, and the carry-out
 * (the high order 32nd digit) of the sum is generated.
 * Botht the sum and the carry are returned.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   | 15-24 | 1/cycle  |
 * |power9   | 6-15  | 2/cycle  |
 *
 * @param *cout a pointer to a 128-bit vector to recieve the BCD carry-out.
 * @param a a 128-bit vector treated as a signed BCD 31 digit value.
 * @param b a 128-bit vector treated as a signed BCD 31 digit value.
 * @return a 128-bit vector with the low order 31-digits of the sum (a+b).
 * Values are -1, 0, and +1.
 */
static inline vBCD_t
vec_cbcdaddcsq (vBCD_t *cout, vBCD_t a, vBCD_t b)
{
  vBCD_t t, c;
#ifdef _ARCH_PWR8
  vBCD_t sum_ab, sign_a, sign_ab;

  sum_ab = vec_bcdadd (a, b);
  if (__builtin_expect (__builtin_bcdadd_ov ((vi128_t) a, (vi128_t) b, 0), 0))
    {
      c = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, sum_ab);
    }
  else // (a + b) did not overflow, but did it borrow?
    {
      c = _BCD_CONST_ZERO;
      sign_ab = vec_bcdcpsgn (sum_ab, a);
      if (!vec_all_eq(sign_ab, sum_ab))
	{
	  vBCD_t nines = vec_bcdcpsgn (_BCD_CONST_PLUS_NINES, a);
	  vBCD_t tensc = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, a);
	  c = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, sum_ab);
	  sum_ab = vec_bcdaddesqm (nines, sum_ab, tensc);
	}
    }
  t = sum_ab;
#else
  vBCD_t sign_ab;
  _Decimal128 d_a, d_b, d_s, d_t;
  const vui32_t mz = CONST_VINT128_W(0, 0, 0, 0x0000000d);
  d_a = vec_BCD2DFP (a);
  d_b = vec_BCD2DFP (b);
  d_s = d_a + d_b;
  t = vec_DFP2BCD (d_s);
  // Shift right 31 digits, leaving the carry.
  d_t = __builtin_dscriq (d_s, 31);
  c = vec_DFP2BCD (d_t);
  // fix up spurious negative zeros
  if (vec_all_eq((vui32_t ) c, mz))
    c = _BCD_CONST_ZERO;
  // (a + b) did not overflow, but did it borrow?
  sign_ab = vec_bcdcpsgn (t, a);
  if (!vec_all_eq(sign_ab, t))
    {
      vBCD_t nines = vec_bcdcpsgn (_BCD_CONST_PLUS_NINES, a);
      vBCD_t tensc = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, a);
      c = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, t);
      t = vec_bcdaddesqm (nines, t, tensc);
    }
#endif
  *cout = c;
  return (t);
}

/** \brief Combined Decimal Add Extended & write Carry Signed Quadword.
 *
 * Two Signed 31 digit values and a signed carry-in are added together
 * and the carry-out (the high order 32nd digit) of the sum is
 * returned.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   | 54-63 | 1/cycle  |
 * |power9   | 15-24 | 2/cycle  |
 *
 * @param *cout a pointer to a 128-bit vector to recieve the BCD carry-out.
 * @param a a 128-bit vector treated as a signed BCD 31 digit value.
 * @param b a 128-bit vector treated as a signed BCD 31 digit value.
 * @param cin a 128-bit vector treated as a signed BCD carry with values
 * -1, 0, or +1.
 * @return a 128-bit vector with the low order 31-digits of the sum (a+b).
 */
static inline vBCD_t
vec_cbcdaddecsq (vBCD_t *cout, vBCD_t a, vBCD_t b, vBCD_t cin)
{
  vBCD_t t, c;
#ifdef _ARCH_PWR8
  vBCD_t a_b;
  vBCD_t sum_ab, sum_abc, sign_abc;

  sum_ab = vec_bcdadd (a, b);

  if (__builtin_expect (__builtin_bcdadd_ov ((vi128_t) a, (vi128_t) b, 0), 0))
    {
      sum_abc = vec_bcdadd (sum_ab, cin);
      c = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, sum_abc);
    }
  else // (a + b) did not overflow, but did (a + b + c) overflow?
    {
      sum_abc = vec_bcdadd (sum_ab, cin);
      if (__builtin_expect (__builtin_bcdadd_ov ((vi128_t) sum_ab, (vi128_t) cin, 0), 0))
	{
	  c = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, sum_abc);
	}
      else // (a + b + c) did not overflow, but did it borrow?
	{
	  c = _BCD_CONST_ZERO;
	  sign_abc = vec_bcdcpsgn (sum_abc, a);
	  if (!vec_all_eq(sign_abc, sum_abc))
	    {
	      vBCD_t nines = vec_bcdcpsgn (_BCD_CONST_PLUS_NINES, a);
	      vBCD_t tensc = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, a);
	      c = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, sum_abc);
	      sum_abc = vec_bcdaddesqm (nines, sum_abc, tensc);
	    }
	}
    }
  t = sum_abc;
#else
  const vui32_t mz = CONST_VINT128_W (0, 0, 0, 0x0000000d);
  vBCD_t sign_abc;
  _Decimal128 d_a, d_b, d_c, d_s, d_t;
  d_a = vec_BCD2DFP (a);
  d_b = vec_BCD2DFP (b);
  d_c = vec_BCD2DFP (cin);
  d_s = d_a + d_b + d_c;
  t = vec_DFP2BCD (d_s);
  // Shift right 31 digits, leaving the carry.
  d_t = __builtin_dscriq (d_s, 31);
  c = vec_DFP2BCD (d_t);
  // fix up spurious negative zeros
  if (vec_all_eq((vui32_t) c, mz))
  c = _BCD_CONST_ZERO;
  // (a + b + c) did not overflow, but did it borrow?
  sign_abc = vec_bcdcpsgn (t, a);
  if (!vec_all_eq(sign_abc, t))
    {
      vBCD_t nines = vec_bcdcpsgn (_BCD_CONST_PLUS_NINES, a);
      vBCD_t tensc = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, a);
      c = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, t);
      t = vec_bcdaddesqm (nines, t, tensc);
    }
#endif
  *cout = c;
  return (t);
}

/** \brief Combined Vector Signed BCD Multiply High/Low.
 *
 * Two Signed 31 digit values are multiplied and generates the 62 digit
 * product.
 *
 * The vector unit does not have a BCD multiply, so we convert the
 * operands to _Decimal128 format and use the DFP quadword multiply.
 * This gets tricky as the product can be up to 62 digits, and
 * _Decimal128 format can only hold 34 digits.
 *
 * To avoid overflow in the DFU, we split each BCD operand into 15
 * upper and 16 lower digit halves. This requires up four decimal
 * multiplies and produces four 30-32 digit partial products.
 * These are aligned appropriately (via DFP decimal shift) and summed
 * (via DFP Decimal add) to generate the high and low (31-digit)
 * parts of the 62 digit product.
 *
 * In this case we compute and return the whole 62-digit product
 * split into two 31-digit BCD vectors.
 *
 * \note There is early exit case if both operands are
 * 16-digits or less. Here the product can not exceed 32-digits and
 * requires only a single DFP multiply.
 * The DFP2BCD conversion will extract the lower 31-digits.
 * Then DFP Decimal shift will isolate 32nd digit.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |107-413| 1/cycle  |
 * |power9   |115-294| 1/cycle  |
 *
 * @param a a 128-bit vector treated as a signed BCD 31 digit value.
 * @param b a 128-bit vector treated as a signed BCD 31 digit value.
 * @param *p_high pointer 128-bit vector to receive the high 31-digits
 * of the product (a * b).
 * @return a 128-bit vector which is the lower 31 digits of (a * b).
 */
static inline vBCD_t
vec_cbcdmul (vBCD_t *p_high, vBCD_t a, vBCD_t b)
{
  const vBCD_t dword_mask = (vBCD_t) CONST_VINT128_DW(15, -1);
  const vui32_t mz = CONST_VINT128_W (0, 0, 0, 0x0000000d);
  vBCD_t t, ph, low_a, low_b, high_a, high_b;
  _Decimal128 d_p, d_t, d_al, d_bl;

  low_a = vec_and (a, dword_mask);
  low_b = vec_and (b, dword_mask);
  d_al = vec_BCD2DFP (low_a);
  d_bl = vec_BCD2DFP (low_b);
  d_p = d_al * d_bl;
  if (__builtin_expect ((vec_cmpuq_all_eq ((vui128_t) low_a, (vui128_t) a)
      && vec_cmpuq_all_eq ((vui128_t) low_b, (vui128_t) b)), 1))
    {
      d_t = __builtin_dscriq (d_p, 31);
      ph = vec_DFP2BCD (d_t);
      d_t = d_p;
    }
  else
    {
      _Decimal128 d_ah, d_bh, d_hl, d_lh, d_h, d_hh, d_ll, d_m, d_mp;
      high_a = vec_bcdsrqi (a, 16);
      high_b = vec_bcdsrqi (b, 16);
      d_ah = vec_BCD2DFP (high_a);
      d_bh = vec_BCD2DFP (high_b);
      d_hl = d_ah * d_bl;
      d_ll = __builtin_dscriq (d_p, 16);

      d_lh = d_al * d_bh;

      d_mp = d_hl + d_lh;
      d_m = d_mp + d_ll;
      d_m = __builtin_dscriq (d_m, 15);

      d_hh = d_ah * d_bh;
      d_hh = __builtin_dscliq (d_hh, 1);

      d_t = d_m + d_hh;
      ph = vec_DFP2BCD (d_t);

      d_h = __builtin_dscliq (d_mp, 17);
      d_h = __builtin_dscriq (d_h, 1);

      d_t = d_p + d_h;
    }
  t = vec_DFP2BCD (d_t);
  // fix up spurious negative zeros
#ifdef _ARCH_PWR9
  ph = vec_bcdadd (ph, _BCD_CONST_ZERO);
#else
  if (vec_all_eq((vui32_t) ph, mz))
    ph = _BCD_CONST_ZERO;
#endif
  *p_high = ph;

  // fix up spurious negative zeros
#ifdef _ARCH_PWR9
  t = vec_bcdadd (t, _BCD_CONST_ZERO);
#else
  if (vec_all_eq((vui32_t) t, mz))
    t = _BCD_CONST_ZERO;
#endif
  return (t);
}

/** \brief Pack a FPR pair (_Decimal128) to a doubleword vector
 *  (vector double).
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param lval FPR pair containing a _Decimal128.
 *  @return vector double containing the doublewords of the FPR pair.
 */
static inline vf64_t
vec_pack_Decimal128 (_Decimal128 lval)
{
#ifdef _ARCH_PWR7
  vf64_t  t;
  __asm__(
      "\txxpermdi %x0,%1,%L1,0b00;\n"
      : "=v" (t)
      : "d" (lval)
      : );
  return (t);
#else
  //needs to work for P6 without xxpermdi
  __VEC_U_128   t;
  t.dpd128 = lval;
  return (t.vf2);
#endif
}

/** \brief Quantize (truncate) a _Decimal128 value before convert to
 * BCD.
 *
 * Truncate (round toward 0) and justify right the input
 * _Decimal128 value so that the unit digit is in the right most
 * position.  This supports BCD multiply and divide using DFP
 * instructions by truncating fractional digits before conversion
 * back to BCD.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |  15   | 1/cycle  |
 * |power9   |  12   | 1/cycle  |
 *
 * @param val a _Decimal128 value.
 * @return The quantized __Decimal128 value in a double float pair.
 */
static inline _Decimal128
vec_quantize0_Decimal128 (_Decimal128 val)
{
#ifdef _ARCH_PWR7
  _Decimal128 t;
  __asm__(
      "dquaiq 0,%0,%1,0b01;\n"
      : "=d" (t)
      : "d" (val)
      : );
  return (t);
#else
  return (quantized128(val, 0DL));
#endif
}

/** \brief Vector Decimal Convert Zoned Decimal digit
 *  pairs to to radix 100 binary integer bytes..
 *
 *  Convert 32 decimal digits from Zoned Format (one character
 *  per digit, in 2 vectors) to Binary coded century format.
 *  Century format is adjacent digit pairs converted to a binary
 *  integer in the range 0-99. Each century digit is stored in a byte.
 *  Input values should be valid decimal characters in the range 0-9.
 *
 *  \note Zoned numbers are character strings with the high order
 *  digit on the left.
 *  \note The high to low digit order is effectively big endian.
 *  This matches the digit order precedence of Decimal Add/Subtract.
 *
 *  This can be used as the first stage operation in wider decimal
 *  to binary conversions. Basically the result of this stage are
 *  binary coded 100s "digits" that can be passed to vec_bcdctb10ks().
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 15-17 | 1/cycle  |
 *  |power9   | 17-20 | 1/cycle  |
 *
 *  @param zone00 a 128-bit vector char containing the high order 16
 *  digits of a 32-digit number.
 *  @param zone16 a 128-bit vector char containing the low order 16
 *  digits of a 32-digit number.
 *  @return 128-bit vector unsigned char. For each byte, 2 adjacent
 *  zoned digits are converted to the equivalent binary representation
 *  in the range 0-99.
 */
static inline vui8_t
vec_rdxcfzt100b (vui8_t zone00, vui8_t zone16)
{
  const vui8_t dmask = vec_splat_u8 (15);
  const vui8_t dx10 = vec_splat_u8 (10);
  vui8_t znd00, znd16;
  vui8_t ones, tens;
  vui16_t ten00, ten16;

  /* Isolate the BCD digit from each zoned character. */
  znd00 = vec_and (zone00, dmask);
  znd16 = vec_and (zone16, dmask);
  /* Pack the odd zone digits into a single vector.
     This is the unit digit of each zoned digit pair. */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  ones = vec_pack ((vui16_t) znd16, (vui16_t) znd00);
#else
  ones = vec_pack ((vui16_t) znd00, (vui16_t) znd16);
#endif
  /* Multiply the even zone digits by 10 before packing
     them into a single vector.
     This is the tens digit of each zoned digit pair. */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  ten00 = vec_mulo (znd00, dx10);
  ten16 = vec_mulo (znd16, dx10);
  tens = vec_pack (ten16, ten00);
#else
  ten00 = vec_mule (znd00, dx10);
  ten16 = vec_mule (znd16, dx10);
  tens = vec_pack (ten00, ten16);
#endif
  /* sum adjacent tens and unit digit pairs, into a single
   * binary value in the range 0-99.  */
  return vec_add (tens, ones);
}

/** \brief Vector Decimal Convert Binary Coded Decimal (BCD) digit
 *  pairs to radix 100 binary integer bytes.
 *
 *  Convert 32 decimal digits from BCD Format (one 4-bit nibble
 *  per digit) to Binary coded century format.
 *  Century format is adjacent digit pairs converted to a binary
 *  integer in the range 0-99. Each century digit is stored in a byte.
 *  Input values should be valid BCD nibbles in the range 0-9.
 *
 *  This can be used as the first stage operation in wider decimal
 *  to binary conversions. Basically the result of this stage are
 *  binary coded Century "digits" that can be passed to
 *  vec_bcdctb10ks().
 *
 *  \note the nibble high to low digit word is effectively big endian.
 *  This matches the digit order precedence of Decimal Add/Subtract.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 13-22 | 1/cycle  |
 *  |power9   | 14-23 | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned char
 *  of BCD nibble pairs.
 *  @return 128-bit vector unsigned char, For each byte, BCD digit
 *  pairs are converted to the equivalent binary representation
 *  in the range 0-99.
 */
static inline vui8_t
vec_rdxct100b (vui8_t vra)
{
  vui8_t x6, c6, high_digit;
  /* Compute the high digit correction factor. For BCD to binary 100s
   * this is the isolated high digit multiplied by the radix difference
   * in binary.  For this stage we use 0x10 - 10 = 6.  */
  high_digit = vec_srbi (vra, 4);
  c6 = vec_splat_u8 ((unsigned char) 0x06);
  x6 = vec_mulubm (high_digit, c6);
  /* Subtract the high digit correction bytes from the original
   * BCD bytes in binary.  This reduces byte range to 0-99. */
  return vec_sub (vra, x6);
}

/** \brief Vector Decimal Convert radix 100 digit
 *  pairs to radix 10,000 binary integer halfwords.
 *
 *  Convert from 16 century digit Format (one century
 *  per byte) to 8 Binary coded 10k (one per halfword) format.
 *  10K format is adjacent century digit pairs converted to a binary
 *  integer in the range 0-9999 .
 *  Input byte values should be valid 100s in the range 0-99.
 *  The result vector will be 8 short int values in the range 0-9999.
 *
 *  This can be used as the intermediate stage operation in wider
 *  BCD to binary conversions. Basically the result of this stage are
 *  binary coded 10,000s "digits" which can be passed to
 *  vec_bcdctb100ms().
 *
 *  \note the 100s digit high to low order is effectively big endian.
 *  This matches the digit order precedence of Decimal Add/Subtract.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  9-18 | 1/cycle  |
 *  |power9   |  9-18 | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned char
 *  of radix 100 digit pairs.
 *  @return 128-bit vector unsigned short. For each halfword,
 *  adjacent pairs of century digits pairs are converted to the
 *  equivalent binary halfword representation in the range 0-9999.
 */
static inline vui16_t
vec_rdxct10kh (vui8_t vra)
{
  vui8_t c156;
  vui16_t x156;
  /* Compute the high digit correction factor. For 100s to binary 10ks
   * this is the isolated high digit multiplied by the radix difference
   * in binary.  For this stage we use 0x100 - 100 = 156.  */
  c156 = vec_splats ((unsigned char) 156);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  x156 = vec_mulo ((vui8_t) vra, c156);
#else
  x156 = vec_mule ((vui8_t) vra, c156);
#endif
  /* Subtract the high digit correction halfword from the original
   * 100s byte pair in binary.  This reduces the range to 0-9999. */
  return vec_sub ((vui16_t) vra, x156);
}

/** \brief Vector Decimal Convert radix 10,000 digit
 *  halfword pairs to radix 100,000,000 binary integer words.
 *
 *  Convert from 10k digit Format (one 10k per halfword)
 *  to Binary coded 100m (one per word) format.
 *  100m format is adjacent 10k digit pairs converted to a binary
 *  integer in the range 0-99999999.
 *  Input halfword values should be valid 10Ks in the range 0-9999.
 *  The result will be binary int values in the range 0-99999999.
 *
 *  This can be used as the intermediate stage operation in a wider
 *  BCD to binary conversions. Basically the result of this stage are
 *  binary coded 100,000,000s "digit" words which can be passed to
 *  vec_bcdctb10es().
 *
 *  \note the 10k digit high to low order is effectively big endian.
 *  This matches the digit order precedence of Decimal Add/Subtract.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  9-18 | 1/cycle  |
 *  |power9   |  9-18 | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned short
 *  of radix 10k digit pairs.
 *  @return 128-bit vector unsigned int. For each halfword, adjacent
 *  10k digit pairs are converted to the equivalent binary word
 *  integer representation in the range 0-99999999.
 */
static inline vui32_t
vec_rdxct100mw (vui16_t vra)
{
  vui16_t c55536;
  vui32_t x55536;
  /* Compute the high digit correction factor. For 10ks to binary 100ms
   * this is the isolated high digit multiplied by the radix difference
   * in binary.  For this stage we use 0x10000 - 10000 = 55536.  */
  c55536 = vec_splats ((unsigned short) 55536);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  x55536 = vec_mulo ((vui16_t) vra, c55536);
#else
  x55536 = vec_mule ((vui16_t) vra, c55536);
#endif
  /* Subtract the high digit correction word from the original
   * 10ks byte pair in binary.  This reduces the range to
   * 0-99999999. */
  return vec_sub ((vui32_t) vra, x55536);
}

/** \brief Vector Decimal Convert radix 100,000,000 digit
 *  word pairs to radix 10E16 binary integer doublewords.
 *
 *  Convert from 100m digit format (one 100m digit per word)
 *  to Binary coded 10p (one per doubleword) format.
 *  10p format is adjacent 100m digit pairs converted to a binary
 *  long integer in the range 0-9999999999999999 (10 quadrillion).
 *  Input word values should be valid 100m in the range 0-99999999.
 *
 *  This can be used as the intermediate stage operation in a wider
 *  BCD to binary conversions. Basically the result of this stage are
 *  binary coded 10,000,000,000,000,000s "digits" doublewords which
 *  can be passed to vec_bcdctb10e32().
 *
 *  \note the 100m digit high to low order is effectively big endian.
 *  This matches the digit order precedence of Decimal Add/Subtract.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  9-18 | 1/cycle  |
 *  |power9   |  9-18 | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned int
 *  of radix 100m digit pairs.
 *  @return 128-bit vector unsigned long. For each word pair,
 *  containing 8 digit equivalent value each,
 *  adjacent 100m digits are converted to the equivalent binary
 *  doubleword representation in the range 0-9999999999999999.
 */
static inline vui64_t
vec_rdxct10E16d (vui32_t vra)
{
  vui32_t c4194967296;
  vui64_t x4194967296;
  /* Compute the high digit correction factor. For 100ms to binary 10ts
   * this is the isolated high digit multiplied by the radix difference
   * in binary.  For this stage we use 0x100000000 - 100000000 =
   * 4194967296.  */
  c4194967296 = vec_splats ((unsigned int) 4194967296);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  x4194967296 = vec_mulouw ((vui32_t) vra, c4194967296);
#else
  x4194967296 = vec_muleuw ((vui32_t) vra, c4194967296);
#endif
  /* Subtract the high digit correction doubleword from the original
   * 100m word pair in binary.  This reduces the range to
   * 0-9999999999999999. */
  return vec_subudm ((vui64_t) vra, x4194967296);
}

/** \brief Vector Decimal Convert radix 10E16 digit
 *  pairs to radix 10E32 __int128 quadwords.
 *
 *  Convert from 10p digit format (one 10p digit per doubleword)
 *  to binary __int128 (one per quadword) format.
 *  Input doubleword values should be valid long integers in the
 *  range 0-9999999999999999.
 *  The result will be a binary _int128 value in the range
 *  0-99999999999999999999999999999999.
 *
 *  This can be used as the final stage operation in a 32-digit
 *  BCD to binary __int128 conversion.
 *
 *  \note the 10e16-1 digit high to low order is effectively big endian.
 *  This matches the digit order precedence of Decimal Add/Subtract.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 25-32 | 1/cycle  |
 *  |power9   | 11-20 | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned long
 *  of radix 10e16 digit pairs.
 *  @return 128-bit vector unsigned __int128. The doubleword pair, of
 *  16 equivalent digits each, are converted to the equivalent binary
 *  quadword representation in the range
 *  0-99999999999999999999999999999999.
 */
static inline vui128_t
vec_rdxct10e32q (vui64_t vra)
{
  vui64_t c18436744073709551616;
  vui128_t x18436744073709551616;
  /* Compute the high digit correction factor for 10ts to binary 10e32s
   * This is the isolated high digit multiplied by the radix difference
   * in binary.  For this stage we use
   * 0x10000000000000000 - 10000000000000000 = 18436744073709551616.  */
  c18436744073709551616 = vec_splats ((unsigned long) 18436744073709551616UL);
  x18436744073709551616 = vec_vmuleud ((vui64_t) vra, c18436744073709551616);

  /* Subtract the high digit correction quadword from the original
   * 10e doubleword pair in binary.  This reduces the range to
   * 0-99999999999999999999999999999999. */
  return vec_subuqm ((vui128_t) vra, x18436744073709551616);
}

/*! \brief Vector Set Bool from Signed BCD Quadword if invalid.
 *
 *  If the quadword's sign nibble is 0xB, 0xD, 0xA, 0xC, 0xE, or 0xF
 *  and all 31 digit nibbles 0-9 then return a vector
 *  bool __int128 that is all '0's. Otherwise return all '1's.
 *
 *  |processor| Latency |Throughput|
 *  |--------:|:-------:|:---------|
 *  |power8   | 15 - 39 | 1/cycle  |
 *  |power9   | 3 - 15  | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as signed BCD quadword.
 *  @return a 128-bit vector bool of all '0's if the BCD digits and
 *  sign are valid. Otherwise all '1's.
 */
static inline vb128_t
vec_setbool_bcdinv (vBCD_t vra)
{
  vb128_t result;
#if defined (_ARCH_PWR8) && (__GNUC__ > 6)
  // The OV and INV status results overlay CR.bit[59] for bcdadd/sub.
  // For valid inputs OV will never be set for both bcdadd/sub.
  // So if both bcdadd/bcdsub return OV then must be invalid data.
  if (__builtin_bcdadd_ov ((vi128_t) vra, (vi128_t) _BCD_CONST_ZERO, 0)
   && __builtin_bcdsub_ov ((vi128_t) vra, (vi128_t) _BCD_CONST_ZERO, 0))
    result = (vb128_t) vec_splat_s8 (-1);
  else
    result = (vb128_t) vec_splat_s8 (0);
#else
  const vui16_t sign_mask = vec_splat_u16(15);
  // Load all 6 valid sign nibble values into a vector unsigned short
  // After splatting the sign, we can compare any of six in one op
  const vui16_t sign_vals = CONST_VINT128_H(0x0b, 0x0d, 0x0b, 0x0d, 0x0a, 0x0c,
					    0x0e, 0x0f);
  const vui8_t max_digit = vec_splat_u8(9);
  const vui8_t msk_digit = vec_splat_u8(15);
  vui16_t sign_splat;
  vui16_t sign_code;
  vui8_t even, odd;

  // Replicate the halfword containing the sign nibble
  sign_splat = vec_splat ((vui16_t) vra, VEC_HW_L);
  // Apply the sign nibble mask
  sign_code = vec_and (sign_splat, sign_mask);
  // SIMD compare for match to any positive/negative sign code
  if (vec_any_eq(sign_code, sign_vals))
    {
      // Split even/odd digits out so there only one digit per byte.
      // This insures the binary compare can detect any digits > 9
      even = vec_andc ((vui8_t) vra, msk_digit);
      odd = vec_and ((vui8_t) vra, msk_digit);
      // Align the compare digits with max_digit
      even = (vui8_t) vec_srqi ((vui128_t) even, 4);
      // And eliminate the sign nibble
      odd = (vui8_t) vec_srqi ((vui128_t) odd, 8);
      if (vec_any_gt (even, max_digit) || vec_any_gt(odd, max_digit))
	result = (vb128_t) vec_splat_s8(-1);
      else
	result = (vb128_t) vec_splat_s8(0);
    }
  else
    result = (vb128_t) vec_splat_s8(-1);
#endif
  return (result);
}

/*! \brief Vector Set Bool from Signed BCD Quadword.
 *
 *  If the quadword's sign nibble is 0xB or 0xD then return a vector
 *  bool __int128 that is all '1's. Otherwise if the sign nibble is
 *  0xA, 0xC, 0xE, or 0xF then return all '0's.
 *
 *  /note For _ARCH_PWR7 and earlier (No vector BCD instructions),
 *
 *  this implementation only tests for a valid plus sign nibble.
 *  Otherwise the BCD value is assumed to be negative.
 *
 *  |processor| Latency |Throughput|
 *  |--------:|:-------:|:---------|
 *  |power8   | 17 - 26 | 2/cycle  |
 *  |power9   | 5 - 14  | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as signed BCD quadword.
 *  @return a 128-bit vector bool of all '1's if the sign is negative.
 *  Otherwise all '0's.
 */
static inline vb128_t
vec_setbool_bcdsq (vBCD_t vra)
{
  vb128_t result;
#if defined (_ARCH_PWR8) && (__GNUC__ > 6)
  if (__builtin_bcdsub_gt ((vi128_t) vra, (vi128_t) _BCD_CONST_MINUS_ONE, 0))
    result = (vb128_t) vec_splat_s8 (0);
  else
    result = (vb128_t) vec_splat_s8 (-1);
#else
  const vui32_t sign_mask = vec_splat_u32(15);
//  const vui32_t neg_sign = (vui32_t) CONST_VINT128_W(0x0b, 0x0d, 0x0b, 0x0d);
  const vui32_t plus_sign = (vui32_t) CONST_VINT128_W(0x0a, 0x0c, 0x0e, 0x0f);
  vui32_t sign_splat;
  vui32_t sign_code;

  // Replicate the byte containing the sign to words
  sign_splat = vec_splat ((vui32_t) vra, VEC_W_L);
  // Apply the code mask
  sign_code = vec_and (sign_splat, sign_mask);
  // SIMD compare for match to any positive sign code
  if (vec_any_eq(sign_code, plus_sign))
    result = (vb128_t) vec_splat_s8(0);
  else
    result = (vb128_t) vec_splat_s8(-1);
#endif
  return (result);
}

/*! \brief Vector Sign bit from Signed BCD Quadword.
 *
 *  If the quadword's sign nibble is 0xB or 0xD then return a
 *  none-zero value. Otherwise if the sign nibble is
 *  0xA, 0xC, 0xE, or 0xF then return all '0's.
 *
 *  /note For _ARCH_PWR7 and earlier (No vector BCD instructions),
 *  this implementation only tests for a valid minus sign nibble.
 *  Otherwise the BCD value is assumed to be positive.
 *
 *  |processor| Latency |Throughput|
 *  |--------:|:-------:|:---------|
 *  |power8   | 15 - 26 | 2/cycle  |
 *  |power9   | 5 - 14  | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as signed BCD quadword.
 *  @return a none-zero value if the sign is negative.
 *  Otherwise return '0's.
 */
static inline int
vec_signbit_bcdsq (vBCD_t vra)
{
  int result;
#if defined (_ARCH_PWR8) && (__GNUC__ > 6)
  result = __builtin_bcdsub_lt ((vi128_t) vra, (vi128_t) _BCD_CONST_ZERO, 0);
#else
  const vui32_t sign_mask = vec_splat_u32(15);
  const vui32_t minus_sign = (vui32_t) CONST_VINT128_W(0x0b, 0x0d, 0x0b, 0x0d);
//  const vui32_t plus_sign = (vui32_t) CONST_VINT128_W(0x0a, 0x0c, 0x0e, 0x0f);
  vui32_t sign_splat;
  vui32_t sign_code;

  // Replicate the byte containing the sign to words
  sign_splat = vec_splat ((vui32_t) vra, VEC_W_L);
  // Apply the code mask
  sign_code = vec_and (sign_splat, sign_mask);
  // SIMD compare for match to any negative sign code
  result = vec_any_eq(sign_code, minus_sign);
#endif
  return (result);
}

/** \brief Unpack a doubleword vector (vector double) into a FPR pair.
 * (_Decimal128).
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 1/cycle  |
 *  |power9   |   3   | 1/cycle  |
 *
 *  @param lval Vector of doublewords (long int).
 *  @param lval FPR pair containing a _Decimal128.
 *  @return FPR pair containing a _Decimal128.
 */
static inline _Decimal128
vec_unpack_Decimal128 (vf64_t lval)
{
#ifdef _ARCH_PWR7
  _Decimal128 t;
  __asm__(
      "xxpermdi %0,%x1,%x1,0b00;\n"
      "\txxpermdi %L0,%x1,%x1,0b10;\n"
      : "=&d" (t)
      : "v" (lval)
      : );
  return (t);
#else
  // needs to work for P6 without xxpermdi
  __VEC_U_128   t;
  t.vf2 = lval;
  return (t.dpd128);
#endif
}

/** \brief Vector Zoned Decimal Convert 32 digits
 *  to binary unsigned quadword.
 *
 *  Vector convert 2x quadwords each containing 16 digits to the
 *  equivalent unsigned __int128, in the range
 *  0-99999999999999999999999999999999.
 *  Input values should be valid 32 zoned digits in the range '0'-'9'.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 67-73 | 1/cycle  |
 *  |power9   | 55-62 | 1/cycle  |
 *
 *  @param zone00 a 128-bit vector char containing the high order 16
 *  digits of a 32-digit number.
 *  @param zone16 a 128-bit vector char containing the low order 16
 *  digits of a 32-digit number.
 *  @return 128-bit vector unsigned __int128 in
 *  the range 0-99999999999999999999999999999999.
 */
static inline vui128_t
vec_zndctuq (vui8_t zone00, vui8_t zone16)
{
  vui8_t d100;
  vui16_t d10k;
  vui32_t d100m;
  vui64_t d10e;
  d100 = vec_rdxcfzt100b (zone00, zone16);
  d10k = vec_rdxct10kh (d100);
  d100m = vec_rdxct100mw (d10k);
  d10e = vec_rdxct10E16d (d100m);
  return vec_rdxct10e32q (d10e);
}

#endif /* VEC_BCD_PPC_H_ */

