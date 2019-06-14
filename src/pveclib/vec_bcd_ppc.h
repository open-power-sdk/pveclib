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

#include <pveclib/vec_common_ppc.h>
#include <pveclib/vec_char_ppc.h>
#include <pveclib/vec_int128_ppc.h>

/*!
 * \file  vec_bcd_ppc.h
 * \brief Header package containing a collection of Binary Coded
 * Decimal (<B>BCD</B>) computation and Zoned Character conversion
 * operations on vector registers.
 *
 * Many of these operations are implemented in a single VMX or DFP
 * instruction on newer (POWER8/POWER9) processors.
 * This header serves to fill in functional gaps for older
 * (POWER7, POWER8) processors (using existing VMX, VSX, and DFP
 * instructions) and provides in-line assembler
 * implementations for older compilers that do not
 * provide the built-ins.
 *
 * Starting with POWER6 introduced a Decimal Floating-point
 * (<I>DFP</I>) Facility implementing the
 * <A HREF="https://en.wikipedia.org/wiki/IEEE_754-2008_revision">
 * IEEE 754-2008 revision</A> standard. This is implemented in hardware
 * as an independent Decimal Floating-point Unit (<I>DFU</I>). This
 * is supported with ISO C/C++ language bindings and runtime libraries.
 *
 * The DFP Facility supports a different data format
 * <A HREF="https://en.wikipedia.org/wiki/Densely_packed_decimal">
 * Densely packed decimal</A> (<I>DPD</I> and a more extensive set of
 * operations then BCD or Zoned. So DFP and the comprehensive C
 * language and runtime library support makes it a better target for
 * new business oriented applications. As the DFP Facility supports conversions
 * between DPD and BCD, existing DFP operations can be used to emulate
 * BCD operations on older processors and fill in operational gaps in
 * the vector BCD instruction set.
 *
 * As DFP is supported directly in the hardware and has extensive
 * language and runtime support, there is little that PVECLIB can
 * contribute to general decimal radix computation.
 * However the vector unit and recent BCD and Zoned extensions can
 * still be useful in areas include large order multiple precision
 * computation and conversions between binary and decimal radix.
 * Both are required to convert large decimal numeric or floating-point
 * values with extreme exponents for input or print.
 *
 * So what operations are needed, what does the PowerISA provide,
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
 * So while the DFP Facility could be used for BCD operations and conversions,
 * there was little synergy with the vector unit, in POWER6.
 *
 * POWER7 introduced the VSX facility providing 64x128-bit Vector
 * Scalar Registers (VSRs) that overlaid both the FPRs (VSRs 0-31) and
 * VRs (VSRs 32-63). It also added useful doubleword permute immediate
 * (xxpermdi) and logical/select operations with access to all 64 VSRs.
 * This greatly simplifies data transfers between VRs and FPRs (FPRps)
 * (see vec_pack_Decimal128(), vec_unpack_Decimal128()).
 * This makes it more practical to transfer vector contents to the DFP
 * Facility for processing (see vec_BCD2DFP() and vec_DFP2BCD().
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
 * The
 * <A HREF="https://openpowerfoundation.org/?resource_lib=64-bit-elf-v2-abi-specification-power-architecture">
 * OpenPOWER ABI</A> does have an
 * <I>Appendix B. Binary-Coded Decimal Built-In Functions</I>
 * and proposes that compilers provide a <B>bcd.h</B> header file.
 * At this time no compiler provides this header.
 * GCC does provides compiler built-ins to generate the bcdadd/bcdsub
 * instructions and access the associated condition codes in
 * <I>if</I> statements.
 * GCC also provides built-ins to generate the DFP instruction
 * encode/decode to and from BCD.
 *
 * \note The compiler disables built-ins if the
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
 * \section bcd128_details_0_0 Some details of BCD computation
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
 * operations then BCD or Zoned. So hardware DFP and the comprehensive
 * C language and runtime library support makes it a better target for
 * new business oriented applications.
 * As DFP is supported directly in the hardware and has extensive
 * language and runtime support, there is little that PVECLIB can
 * contribute to general decimal radix computation.
 *
 * \note BCD and DFP support requires at least PowerISA 2.05 (POWER6)
 * or later server level processor support.
 *
 * However the vector unit and recent BCD and Zoned extensions can
 * still be useful in areas including large order multiple precision
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
 * \subsection bcd128_extended_0_1 Preferred sign, zone, and zero.
 *
 * BCD and Zoned Decimal have a long history with multiple computer
 * manufacturers, and this is reflected as multiple encodings of the
 * same basic concept. This is in turn reflected in the PowerISA as
 * Preferred Sign <B>PS</B> immediate operand on BCD instructions.
 *
 * This header implementation assumes that users of PVECLIB are not
 * interested in this detail and just want access to BCD computation
 * with consistent results. So PVECLIB does not expose preferred sign
 * at the API and provides reasonable defaults in the implementation.
 *
 * PVECLIB is targeted at the Linux ecosystem with ASCII character
 * encoding, so the implementation defaults for:
 * - preferred zone nibble 0x3.
 * ASCII encodes decimal characters as 0x30 - 0x39.
 * - preferred sign code nibbles 0xC and 0xD.
 * Historically accounting refers to <I><B>C</B>redit</I> as positive
 * and <I><B>D</B>edit</I> for negative.
 *
 * The PowerISA implementation is permissive of sign encoding of input
 * values and will accept four (0xA, 0xC, 0xE, 0xF) encodings of
 * positive and two (0xB, 0xD) for negative. But the sign code of the
 * result is always set to the preferred sign.
 *
 * The BCD encoding allows for signed zeros (-0, +0) but the PowerISA
 * implementation prefers the positive encoding for zero results.
 * Again the implementation is permissive of both encodings for input
 * operands.
 * Usually this is not an issue but can be when dealing with
 * conversions from other formats (DFP also allows signed 0.0)
 * and implementations of BCD operations for older (POWER7/8)
 * processors.
 *
 * This is most likely to effect user code in comparisons of BCD
 * values for 0.
 * One might expect the following vector binary word compare all
 * \code
  if (vec_all_eq((vui32_t) t, (vui32_t) _BCD_CONST_ZERO))
 * \endcode
 * to give the same result as
 * \code
  if (vec_bcdcmpeq (t, _BCD_CONST_ZERO))
 * \endcode
 * The vector binary compare is likely to have lower latency
 * (on POWER7/8), but will miss compare on <I>-0</I>.
 * The BCD compare operation (i.e. vec_bcdcmpeq ()) is recommended,
 * unless the programs knows the details for the source operands
 * generation, and have good (performance and latency) reasons to
 * to use the alternative compare.  Pveclib strives to provide
 * correct preferred zeros results in its implementation of BCD
 * operations.
 *
 * \subsection bcd128_extended_0_2 Extended Precision computation with BCD
 *
 * Extended precision requires carry and extend forms of bcdadd/sub.
 * Also BCD multiply with multiply high and and double quadword
 * (62-digit) forms. The vector unit does not support BCD multiply
 * so pveclib leverages the DFP Facility to implement these operations.
 * Finally algorithms and extended precision conversions require
 * BCD divide and divide extended. Again leveraging the DPU to
 * implement these operations.
 *
 * \subsubsection bcd128_extended_0_2_0 Vector Add/Subtrace with Carry/Extend example
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
 * generate the bcdadd/sub and test the resulting CR bits in if
 * statements.
 *
 * Unfortunately, the Overflow flag generated by bcdadd/bcdsub is not
 * a true carry/borrow. If the operands have the same sign for bcdadd
 * (different sign for bcdsub)
 * and there is a carry out of the high order digit, then:
 * - The sum is truncated to the low order 31 digits
 * - The sum's sign matches the operands signs
 * - The overflow flag (CR.bit[59]) is set.
 *
 * \note overflow is only set in conjunction with greater than zero
 * (positive) or less than zero (negative) results. This implies that
 * BCD carries are tri-state; +1, 0, or -1.
 *
 * This can be used to simulate a <B>Add and Write-Carry</B> operation.
 * However if the operands have different signs the bcdadd
 * (same sign for bcdsub) the operation does the following:
 * - The smaller magnitude is subtracted from the larger magnitude.
 * - The sign matches the sign of the larger magnitude.
 * - The ox_flag (CR.bit[59]) is NOT set.
 *
 * For a simple BCD add this is the desired result (overflow is
 * avoided and the borrow is recorded in the sign).
 * But for multiple precision BCD operation, this will delay
 * propagation of borrows to the higher order digits and the result is
 * a mixture of signs across elements of the larger multiple precision
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
 * But we should look at some more examples before we assume we have a
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
 * But with the example code above we expected result:
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
      if (!vec_all_eq(sign_ab, t) && !vec_all_eq(_BCD_CONST_ZERO, t))
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
      if (!vec_all_eq(sign_ab, sum_ab) && !vec_all_eq(_BCD_CONST_ZERO, t))
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
 * \todo The BCD add/subtract extend/carry story is not complete.
 * The carry extend operations based only on the <B>OV</B> condition
 * codes only works as expected for bcdadd operands with the same sign
 * and bcdsub with different signs.
 * See vec_bcdaddcsq() and vec_bcdaddecsq().
 * Extended BCD difference (or subtract the same sign or add with
 * different signs) is more complicated.
 * See vec_bcdsubcsq() and vec_bcdsubecsq().
 * Generating a true borrow seems to require
 * looking one (31-digit) column ahead or behind.
 * The first attempt at generating correct borrowing is implemented
 * in vec_cbcdaddcsq() and vec_cbcdaddecsq().
 * There are still cases where these operation will generate a borrow
 * and invert (10s complement) incorrectly.
 * The net seems to be that for BCD multiple precision difference to
 * work correctly, the larger magnitude must be the first
 * operand.
 *
 * \subsubsection bcd128_muldiv_0_2_1 Vector BCD Multiply/Divide Quadword example
 *
 * BCD multiply and divide operations are not directly supported
 * in the current PowerISA. Decimal multiply and divide are
 * supported in the Decimal Floating-point (DFP) Facility, as well
 * as conversion to and from signed (unsigned) BCD.
 *
 * So BCD multiply and divide operations can be routed through the
 * DFP Facility with a few caveats.
 * - DFP Extended format supports up to 34 digits precision
 * - DFP significand represent digits to the <I>left</I> of the
 * implied decimal point.
 * - DFP finite number are not normalized.
 *
 * This allows DFP to represent decimal integer and fixed point
 * decimal values with a preferred exponent of 0.
 * The DFP Facility will maintain this preferred exponent for DPF arithmetic
 * operations until:
 * - An arithmetic operation involves a operand with a non-zero
 * exponent.
 * - A divide operation generates a result with fractional digits
 * - A multiply operation generates a result that exceeds 34 digits.
 *
 * The implementation can insure that input operands are derived from
 * 31-digit BCD values. The results of any divide operations can be
 * truncated back to decimal integer with the preferred 0 exponent.
 * This can be achieved with the DFP Quantize Immediate instruction,
 * specifying the ideal exponent of 0 and a rounding mode of
 * <I>round toward 0</I> (see vec_quantize0_Decimal128()).
 * This allows the following implementation:
 * \code
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
 * \endcode
 *
 * The multiply case is bit more complicated as we need to produce up
 * to 62 digit results without losing precision and DFP only supports
 * 34 digits. This requires splitting the input operands into groups
 * of digits where partial products of any combination of these groups
 * is guaranteed not exceed 34 digits.
 *
 * One way to do this is split each 31-digit operand into two 16-digit
 * chunks (actually 15 and 16-digits). These chunks are converted to
 * DFP extended format and multiplied to produce four 32-digit partial
 * products. These partial products can be aligned and summed to
 * produce the high and low 31-digits of the full 62-digit product.
 * This is the basis for vec_bcd_mul(), vec_bcdmulh(), and
 * vec_cbcdmul().
 *
 * A simple vec_and() can be used to isolate the low order 16 BCD
 * digits. It is simple at this point to detect if both operands are
 * 16-digits or less by comparing the original operand to the isolate
 * value. In this case the product can not exceed 32 digits and we
 * can short circuit the product to a single multiply.
 * Here we can safely use binary compare all.
 *
 * \code
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
    ...
    }
  t = vec_DFP2BCD (d_t);
 * \endcode
 * This is a case where negative 0 can be generated in the DFP multiply
 * and converted unchanged to BCD. This is handled with the following
 * fix up code:
 * \code
 *
  // Minus zero
  const vui32_t mz = CONST_VINT128_W (0, 0, 0, 0x0000000d);
  ...
#ifdef _ARCH_PWR9
  t = vec_bcdadd (t, _BCD_CONST_ZERO);
#else
  if (vec_all_eq((vui32_t) t, mz))
    t = _BCD_CONST_ZERO;
#endif
  return t;
 * \endcode
 * From here the code diverges for multiply low and multiply high
 * (and full combined multiply). Multiply low only needs the 3 lower
 * order partial products. The highest order partial product does not
 * impact the lower order 31-digits and is not needed.
 * Multiply high requires the generation and summation of all 4
 * partial products.
 * Following code completes the implementation of BCD multiply low:
 * \code
  ...
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
 * \endcode
 * Here we know that there are higher order digits in one or both operands.
 * First use vec_bcdsrqi() to isolate the high 15-digits of operands a
 * and b. Both Vector unit and DFP Facility have decimal shift operations,
 * but the vector shift operation is faster.
 *
 * Then convert to DFP and
 * multiply (high_a * low_b and high_b * low_a) for the two middle
 * order partial products which are summed. This sum represents the
 * high 32-digits (the 31-digit sum can carry) of a 48-digit product.
 * Only the lower 16-digits of this sum is needed for the final sum and
 * this needs to be aligned with the high 16 digits of the original
 * lower order partial product.
 *
 * For this case use
 * <B>DFP Shift Significand Left Immediate</B> and
 * <B>DFP Shift Significand Right Immediate</B>. All the data is in
 * the DFP Facility and the high cost of the DFP Facility shift is offset by avoiding
 * extra format conversions. We use shift left 17 followed by shift
 * right 1 to clear the highest order DFP digit and avoid any overflow.
 * A final DFP add produces the low order 32 digits of the product
 * which will be truncated to 31-digits in the conversion to BCD.
 *
 * How we can look at the BCD multiply high (generate the full 62-digit
 * product returning the high 31 digits) and point out the differences.
 * Multiply high also starts by isolating the low order 16 BCD
 * digits, performing the low order multiply (low_a * low_b),
 * and testing for the short circuit (all higher order digits are 0).
 * The first difference (from multiply low) is that in this case
 * only the high digit of the potential 32-digit product is returned.
 *
 * \code
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
      d_t = __builtin_dscriq (d_p, 31);
    }
  else
    {
    ...
    }
  t = vec_DFP2BCD (d_t);
 * \endcode
 * So the short circuit code shifts the low partial product right 31
 * digits and returns that value.
 *
 * If we can not short circuit, Multiply high requires the generation
 * and summation of all four partial products.
 * Following code completes the implementation of BCD multiply high:
 * \code
  ...
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
 * \endcode
 * Again we know that there are higher order digits in one or both
 * operands and use vec_bcdsrqi() to isolate the high 15-digits of
 * operands a and b. Then convert to DFP and multiply (high_a * low_b
 * and high_b * low_a) for the two middle order partial products
 * (d_hl and d_lh).
 *
 * The low order partial product (d_p) was generated above but we need
 * only the high order 15 digits for summation. Shift the low partial
 * product right 16 digits then sum (d_hl + d_lh + d_ll) the low and
 * middle order partial products. This produces the high 32 digits of
 * the lower 48 digit partial sum. Shift this right 15 digits to align
 * with the high order 31 digits for the product.
 *
 * Then multiply (high_a * high_b) to generate the high order partial
 * product. This represents the high 30 digits of a 62 digits. Shift
 * this left 1 digit to correct the alignment. The sum of the adjusted
 * high and middle order partials gives the high order 31 digits of the
 * 62-digit product.
 *
 * \subsubsection bcd128_convert_0_2_2 Vector BCD to/from Binary conversion
 *
 * Conversions between Decimal (BCD, Zoned, or string) and binary is
 * another topic which is more complicated that it first appears.
 * Everyone that takes computer science should have learned about
 * <I>atoi</I> and <I>itoa</I> for conversions between strings of
 * decimal character and binary integers.
 *
 * ASCII to integer is basically;
 * - initialize a integer accumulator to 0
 * - loop
 *   - multiply the accumulator by 10
 *   - load the next character and convert to a binary decimal digit
 *   - Add this digit to the accumulator
 * - repeat until end of string.
 *
 * Integer to ASCII is basically;
 * - initialize a temp variable with the integer number
 * - loop
 *   - compute the remainder/modulo of temp by 10
 *   - convert this binary digit to a character and store as the next
 *     char
 *   - divide temp by 10 and use that for the next iteration
 * - repeat until temp is zero.
 *
 * You may have noticed that the algorithms above are not exactly
 * vector ready. Both are serialized on expensive multiply and
 * divide operations. This is not so bad for 9 digit (32-bit)
 * integers but will be noticeable when converting between 128-bit
 * binary and 31-digit BCD.
 *
 * For the vector BCD equivalent of <B><I>atoi</I></B> we
 * could use the PVECLIB implementation of
 * <B>Vector Multiply by 10 Extended Unsigned Quadword</B>. For POWER8,
 * vec_mul10euq() uses; multiple even/odd, a couple of shift left octet
 * immediates, and add quadword. This sequence runs 5-7 instructions
 * and has a minimum latency of 13 cycles.
 * To convert from BCD to binary we need to shift and isolate,
 * one BCD digit at time, then feed that into vec_mul10euq().
 * Ignoring for now the latency associated with shifting the BCD digits,
 * we can quickly estimate 13 * 32 = 416 cycles to convert 32 digits.
 *
 * For the vector BCD equivalent of <B><I>itoa</I></B> we could use the
 * POWER8 <B>Decimal Add Modulo</B> instruction.
 * For POWER8 vec_bcdadd() has a latency of 13 cycles.
 * But the conversion would be one bit at a time.
 * Use vec_bcdadd() to multiply by 2 then shift / issolate a bit from
 * the binary value, format / convert that bit to BCD 0/1. and
 * vec_bcdadd() again.
 * So a quick estimate for this conversion is 13 * 2 * 128 = 3328
 * cycles.
 *
 * \paragraph bcd128_convert_0_2_2_1 Vector Parallel conversion
 *
 * Clearly just using bigger registers for bigger numbers is not
 * helping. So we want to think about algorithms that do
 * more in parallel and leverage the vector unit we have.
 *
 * For POWER9 we have
 * Decimal Convert From/To Signed Quadword
 * and Decimal Convert From/To Zoned
 * (See vec_bcdcfsq(), vec_bcdctsq(), vec_bcdcfz(), vec_bcdctz()).
 * These provide direct conversion between quadword binary and signed
 * BCD and between signed BCD and zoned characters.
 *
 * The BCD convert from/to Zoned are simple operation that run 3 cycles
 * latency on POWER9 and 14-27 cycles for the POWER8 implementation.
 * For POWER8 there is some additional complexity verify and converting
 * the preferred <I>sign code</I> between BCD and Zoned
 * (of course they are different).
 *
 * But the BCD convert from/to Signed Quadword operations are a bit
 * heavier, running 37 and 23 cycles latency on POWER9. These
 * instructions execute in the DFU and so are single issue. They also
 * keeps the DFU pipeline busy (for 25 and 11 cycles) and block
 * execution of the next DFU operation for a while.  Still this is
 * better than the serial conversion examples described above.
 *
 * But part of the value of PVECLIB is to provide support across
 * POWER7/8/9 and across compiler versions.
 * The convert instructions above are not supported in current
 * compilers with built-ins so PVECLIB provides in-line
 * assembler implementations for these operations.
 * Now we need look into better algorithms for implementing
 * these operations on POWER7/8.
 *
 * The Vector unit can multiply, add, or subtract integer elements in
 * parallel. The conversion process is basically multiply and add/sub
 * as we can replace divide operations with the multiplicative inverse.
 * So if we are looking for a way to break the conversion down into
 * steps that can be performed in parallel on elements of the larger
 * value and require fewer steps.
 *
 * For now we can simplify the problem to unsigned radix conversion
 * and deal with signed conversion as a later cleanup step
 * based on the complete unsigned conversion.
 *
 * \paragraph bcd128_convert_0_2_2_2 Vector Parallel BCD to quadword conversion
 *
 * Starting with BCD (Radix 10) to Binary (Radix 2) conversion. The
 * data is represented as 32 BCD digits encoded as 4-bit <I>nibbles</I>
 * starting with high orders digits on the left,
 * to low order digits on the right.
 *
 * Said differently, unsigned BCD vectors are represented as 16-bytes
 * each containing a pair of BCD digits, each in the range 00-99.
 * This is helpful because the PowerISA has instructions that multiply
 * and add integer bytes, in parallel.
 * So it seems possible to convert bytes containing even/odd pairs of
 * BCD digits to integer bytes, each in the range 0-99:
 * simply multiply the even digit by 10 and add the odd digit.
 *
 * The result is a vector of 16 x radix-100 bytes (binary integers in
 * the range 0-99). Said differently a radix 100 vector represented as
 * 8 halfwords each containing a pair of radix 100 digits, each in the
 * range 0-99. Again these pairs of digits (bytes) can be converted by
 * multiply and add to radix 10,000 halfwords.
 *
 * Repeat the process three more times:
 * - convert 8 halfwords pairwise into 4 words
 * each containing values in the range 0-99999999
 * (radix 10**8 digits).
 * - convert 4 words pairwise into 2 doublewords,
 * each containing values in the range 0-9999999999999999
 * (radix 10*16 digits).
 * - convert 2 doublewords pairwise into a quadword integer in the range
 * 0-99999999999999999999999999999999.
 *
 * So in 5 steps, each only using vector multiply and add, we convert
 * 32 BCD digits to a quadword integer.
 *
 * \note Actually 10**32 can be represented in 107 bits, but who is
 * counting.
 *
 * Actually, it is a little more complicated than multiply and add.
 * The digits of the digit pair must be isolated and shifted into
 * alignment before the multiply and add.
 * Looking something like this:
 *
 * \code
vui8_t
test_vec_rdxct100b_0 (vui8_t vra)
{
  vui8_t x10, c10, high_digit, low_digit;
  // Isolate the low_digit
  low_digit = vec_slbi (vra, 4);
  low_digit = vec_srbi (low_digit, 4);
  // Shift the high digit into the units position
  high_digit = vec_srbi (vra, 4);
  // multiply the high digit by 10
  c10 = vec_splats ((unsigned char) 10);
  x10 = vec_mulubm (high_digit, c10);
  // add the low_digit to high_digit * 10.
  return vec_add (x10, low_digit);
}
 * \endcode
 * The PowerISA does not provide general <I>nibble</I> arithmetic,
 * only byte. So the first operations involve isolating each nibble
 * into separate (high_digit and low_digit) bytes. The high_digit
 * shift also aligns the binary for the multiply and add.
 *
 * The Multiply Unsigned Byte Modulo (vec_mulubm()) generates
 * vmuleub/vmuloub then loads a permute control vector and permutes
 * the low order bytes of the halfword (even/odd) products into a
 * single vector. Finally, add the x10 product and low_digit to get
 * the binary value in the range 0-99.
 *
 * This sequence runs 6-10 instructions and 13-22 cycles latency. The
 * lower values assume the shift control and permute control vectors
 * are commoned with other operations.
 *
 * This is a case where the process on paper is much simpler than the
 * reality of programming computers. The operation is actually
 * (bcd_byte / 16 * 10) + (bcd_byte * 16 / 16) where 16 is the
 * <I>alignment</I> radix and 10 is the <I>decimal</I> radix at this step.
 * The alignment radix operations are (fortunately) strength reduced
 * to vector byte shift left/right.
 *
 * Let's use a little algebra to eliminate some of these steps. One
 * approach is to generate a correction factor from the high_digit and
 * the difference between the <I>alignment</I>  and decimal radix. This
 * correction factor is subtracted directly from the original BCD byte
 * and reduces the operation to
 * (bcd_byte - ((bcd_byte / 16) x (16 - 10))
 * Which looks something like:
 * \code
vui8_t
test_vec_rdxct100b_1 (vui8_t vra)
{
  vui8_t x6, c6, high_digit;
  // Compute the high digit correction factor. For BCD to binary 100s
  // this is the isolated high digit multiplied by the radix difference
  // in binary.  For this stage we use 0x10 - 10 = 6.
  high_digit = vec_srbi (vra, 4);
  c6 = vec_splats ((unsigned char) (16-10));
  x6 = vec_mulubm (high_digit, c6);
  // Subtract the high digit correction bytes from the original
  // BCD bytes in binary.  This reduces byte range to 0-99.
  return vec_sub (vra, x6);
}
 * \endcode
 * Another opportunity is to let the compiler strength reduce the
 * multiply to shift and add. Newer versions of GCC will perform
 * this optimization when using the generic vec_mul built-in for vector
 * integer elements.
 * \note Previous to GCC 8, vec_mul() was only supported for vector
 * float and double.
 *
 * \code
#if (__GNUC__ > 7)
  x6 = vec_mul (high_digit, c6);
#else
  x6 = vec_mulubm (high_digit, c6);
#endif
 * \endcode
 * This eliminates vector multiply even/odd, the permute, and the load
 * associated with the permute. The final sequence runs 5-7
 * instructions and 10-12 cycles latency and looks something like this:
 * \code
     vspltisb v1,4
     vspltisb v13,1
     vsrb    v1,v2,v1
     vslb    v0,v1,v13
     vaddubm v0,v0,v1
     vslb    v0,v0,v13
     vsububm v2,v2,v0
 * \endcode
 *
 * The next step converts adjacent byte pairs to halfwords. We use
 * the same basic formula but adjust the radix constants to;
 * (rdx_hword - ((rdx_hword / 256) x (256 - 100)).
 * Here we need a byte multiply producing a halfword correction factor.
 * No shifts are needed as the vmuleub multiply will access the
 * high byte of each halfword directly.
 *
 * \code
static inline vui16_t
vec_rdxct10kh (vui8_t vra)
{
  vui8_t c156;
  vui16_t x156;
  // Compute the high digit correction factor. For 100s to binary 10ks
  // this is the isolated high digit multiplied by the radix difference
  // in binary.  For this stage we use 256 - 100 = 156.
  c156 = vec_splats ((unsigned char) 156);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  x156 = vec_mulo ((vui8_t) vra, c156);
#else
  x156 = vec_mule ((vui8_t) vra, c156);
#endif
  // Subtract the high digit correction halfword from the original
  // 100s byte pair in binary.  This reduces the range to 0-9999.
  return vec_sub ((vui16_t) vra, x156);
}
 * \endcode
 * This requires: a constant load, a multiply even byte and subtract
 * halfword. The final sequence runs 2-5 instructions and 9-18 cycles
 * latency and looks something like this:
 * \code
     addis   r9,r2,.rodata.cst16+0x90@ha
     addi    r9,r9,.rodata.cst16+0x90@l
     lvx     v0,0,r9
     vmuleub v0,v2,v0
     vsubuhm v2,v2,v0
 * \endcode
 *
 * This pattern continues for converting halfwords to words, words to
 * doublewords, and doublewords to quadwords. For POWER8 the first 4
 * steps are supported by vector multiply and subtract instructions.
 * The last step requires a vec_vmuleud() operation implemented in
 * vec_int128_ppc.h, based on vec_muleuw(), vec_mulouw() and
 * vec_adduqm(). The vec_adduqm() operation is single instruction for
 * POWER8. For POWER7 we will need to leverage more operations
 * implemented in vec_int64_ppc.h and vec_int128_ppc.h for the last
 * two steps.
 *
 * The complete set of steps for converting 32 BCD digits to quadword
 * __int128 binary looks like this:
 * \code
vui128_t
example_vec_bcdctuq (vBCD_t vra)
{
  vui8_t d100;
  vui16_t d10k;
  vui32_t d100m;
  vui64_t d10e;
  vui128_t result;

  d100 = vec_rdxct100b ((vui8_t) vra);
  d10k = vec_rdxct10kh (d100);
  d100m = vec_rdxct100mw (d10k);
  d10e = vec_rdxct10E16d (d100m);
  result = vec_rdxct10e32q (d10e);

  return result;
}
 * \endcode
 * For POWER8 the whole sequence runs 24-36 instructions and 65-78
 * cycles latency. For POWER9 the whole sequence runs 17-26
 * instructions and 52-65 cycles latency.
 *
 * \note POWER9 has a Decimal Convert to Signed Quadword instruction,
 * but no unsigned (32-digit) convert.
 *
 * However we can leverage the POWER9
 * <B>Vector Multiply by 10 Extended Unsigned Quadword</B> instruction
 * to extend the 31-digit convert to a full 32-digits.
 * Basically use the <B>bcdctsq</B> to convert the high 31-digits and
 * then multiply by 10 and add the last digit.
 * See example below:
 *
 * \code
vui128_t
example_vec_bcdctuq_2 (vBCD_t vra)
{
  vui128_t vrt;
#ifdef  _ARCH_PWR9
  const vui32_t bcd_one = (vui32_t) _BCD_CONST_PLUS_ONE;
  const vui32_t sign_mask = (vui32_t) _BCD_CONST_SIGN_MASK;
  vui128_t vrd;
  vBCD_t sbcd;
  // Need to convert BCD unsigned to signed for bcdctsq
  // But can't use bcdcpsgn as the unit digit is not a sign code
  // So use vec_and/sel to extract unit digit and insert sign
  vrd = (vui128_t) vec_and ((vui32_t) vra, sign_mask);
  sbcd = (vBCD_t) vec_sel ((vui32_t) vra, bcd_one, sign_mask);
  // Convert top 31 digits to binary
  vrt = (vui128_t) vec_bcdctsq (sbcd);
  // Then X 10 plus the unit digit to complete 32-digit convert
  vrt = vec_mul10euq (vrt, vrd);
#else
  // P7/P8 implementation as above
#endif
  return vrt;
}
 * \endcode
 * This adds a few more cycles to split the high digits from the low
 * digit and insert a positive sign code. This requires loading some
 * vector constants which may be commoned with loads from other
 * operations. This adds 2-11 cycles. The <B>mul10euq</B> only adds 3
 * cycles latency to complete the BCD to Binary conversion.
 * This is adds only a 21% to 60% latency over the base
 * <B>bcdctsq</B> instruction.
 *
 * \note This process can be extended to 256, 512, 1024-bits, etc by
 * widening the BCD to binary conversion appropriately to blocks of
 * 31 or 32 digits. Then use the basic <I>atoi</I> algorithm using
 * extended quadword multiply / add operations from vec_int128_ppc.h
 * (\ref bcd128_convert_0_2_3).
 *
 * \paragraph bcd128_convert_0_2_2_3 Vector Parallel quadword to BCD conversion
 *
 * \note Binary to BCD conversions are challenging a in a number of
 * ways. First any conversion requires division by non powers of 2.
 * Second, for the same element size binary representation holds more
 * equivalent decimal digits then BCD. If the binary value is too large
 * for the BCD target's element size, the results are often undefined.
 * For example vec_bcdcfsq(). So it is important to constrain
 * the magnitude of the binary to fit the BCD target before conversion.
 * See \ref int128_examples_0_1_2 for details.
 *
 * In most senses, binary to BCD is the reverse of BCD to binary.
 * The radix number in the conversion formula exchange places and the
 * conversion starts with the largest element size (quadword) and works
 * it's way down to the smallest (4-bit nibble).
 *
 * Let's take a look at the conversion formula. For BCD to Binary we used:
 * - bin_byte <- (bcd_byte - ((bcd_byte / 16) x (16 - 10))
 * - bin_byte <- (bcd_byte - ((bcd_byte >> 4) x 6)
 *
 * So after swapping the conversion (to / from) radix constants we see:
 * - bcd_byte <-(bin_byte - ((bin_byte / 10) x (10 - 16))
 * - bcd_byte <-(bin_byte - ((bin_byte / 10) x (-6))
 * - bcd_byte <-(bin_byte + ((bin_byte / 10) x 6)
 *
 * The effect is to divide vector elements of 4*2N bits by 10**N and
 * return the quotient in the high half of the element (in 4*N bits),
 * and the remainder of this divide in the low half of the element
 * (in 4*N bits), Where N is a power of 2<SUP>n</SUP> and <I>n</I>
 * ranges from 0 to 4 (5 steps again).
 *
 * \note So why doesn't PVECLIB provide these steps as operations.
 * For example: divide a vector unsigned __int128 by 10<SUP>16</SUP>
 * and return the quotient in the high doubleword and the remainder
 * in the low doubleword of a vector unsigned long?
 * Because if the input quadword is not less than 10<SUP>32</SUP>
 * the result is undefined (the quotient will overflow).
 *
 * This is good news and bad news. It is good that the correction
 * subtract became a simple add. This allows the uses of multiply sum
 * instruction (where PowerISA has such instructions for the element
 * size).
 * The bad news is that the radix divisor is not a power of two.
 * And since the PowerISA does not have vector integer divide
 * instructions, we use the multiplicative inverse.
 * So in effect, each step of the binary to BCD conversion requires,
 * two multiplies and an add.
 *
 * So let's look at the first and last step of the conversion (the two
 * extremes). The first step (after verifying that the quadword value
 * is less than 10<SUP>32<</SUP>-1) looks like this:
 * \code
static inline vui64_t
vec_rdxcf10e32q (vui128_t vra)
{
  // Compute the high digit correction factor. For binary 10**32 to
  // 10**16, this is  0x10000000000000000 - 10000000000000000
  // = 18436744073709551616.
  const vui64_t c = CONST_VINT128_DW (0, 18436744073709551616UL);

  // Magic numbers for multiplicative inverse to divide by 10**16
  // are 76624777043294442917917351357515459181, no corrective add,
  // and shift right 51 bits.
  const vui128_t mul_invs_ten16 = (vui128_t) CONST_VINT128_DW(
      0x39a5652fb1137856UL, 0xd30baf9a1e626a6dUL);
  const int shift_ten16 = 51;

  vui64_t result;
  vui128_t x, high_digit;

  // high_digit = vra / 10000000000000000;
  high_digit = vec_mulhuq (vra, mul_invs_ten16);
  high_digit = vec_srqi (high_digit, shift_ten16);

  // multiply high_digit by the radix difference c and add vra
  // This separates the high/low 16 digits into doublewords.
#ifdef _ARCH_PWR9
  // 0 in the high dword of const c reduces vmsumudm to vmuloud
  // but with a qword add included.
  result = (vui64_t) vec_msumudm ((vui64_t) high_digit, c, vra);
#else
  x = vec_vmuloud ((vui64_t) high_digit, c);
  result = (vui64_t) vec_adduqm (vra, x);
#endif
  return result;
}
 * \endcode
 * The first multiply is an expensive (40 to 60 cycles) operation as
 * it requires a full Multiply High Unsigned Quadword. The next
 * operation requires a Multiply Odd Unsigned Doubleword then
 * Add Unsigned Quadword Modulo. For POWER9 we can replace these two
 * operations with a single Multiply Sum Unsigned Doubleword Modulo.
 * The latency of this single step is in the same order at the
 * complete BCD to Binary conversion (vec_bcdctuq()).
 *
 * The conversion steps continue with doubleword to word,
 * word to halfword, halfword to byte, byte to BCD (nibbles).
 * The final step is simple by comparison to the first step.
 * \code
static inline vui8_t
vec_rdxcf100b (vui8_t vra)
{
  vui8_t x6, c6, high_digit;
  // Let the compiler generate the multiplicative inverse code
  high_digit = vra / 10;
  // This separates two digit values into BCD Nibbles.
  // multiply high_digit by the radix difference c and
  x6 = high_digit * 6;
  // add bytes the high digit correction to the original
  // (radix 100) bytes in binary.
  return (vra + x6);
}
 * \endcode
 * The GCC vector extensions support dividing a vector
 * char / short / int by a constant. So we can let the compiler
 * generate the multiplicative inverse code for the last three steps.
 * This is not supported (yet) for long and __int128 so the first two
 * steps must explicitly code the multiplicative inverse.
 *
 * Using GCC vector extensions for the following multiply and add
 * works well in this case as it allows the compiler to perform
 * strength reduction. It is not as useful in the other steps as
 * the programmer knows more about the value ranges then the compiler
 * can or should assume. We know the the quotient and corrective
 * constant always fit into the lower half of the element.
 * This allows the use of the half sized vector multiply odd unsigned
 * while compiler will assume it needs to generate a multiply modulo
 * for the full element size.
 *
 * For example the third step (word to halfword) we can use Multiply
 * Sum Unsigned Halfword Modulo to replace the multiply odd and add.
 * This is similar to the multiply sum usage in the first step and it
 * is a case not recognized by the compiler.
 *
 * The full binary to BCD conversion requires all 5 steps to complete
 * the operations and this adds up to 200+ cycles.
 * So this is worth another look.
 *
 * Initially using the DFP Facility for this binary to BCD conversion was
 * rejected because:
 * - The DFP Facility only supports signed fixed doubleword conversions
 * (no fixed quadword conversion)
 * - Fixed binary to DFP conversions are expensive operations
 *   - For POWER8, 32 cycles latency and 1 per 19 cycles throughput
 * - The DFP Facility does support DFP to BCD conversions for double
 * and quadword
 *   - For POWER8, 13 cycle latency and 1 per cycle throughput
 *
 * Perhaps we can use the vec_rdxcf10e32q() operation we defined above
 * as the first step (factoring quadwords into the 16 digit doublewords).
 * Then use the DFP Facility to convert binary doublewords to BCD.
 * In this case we are not concerned with signed conversion as 10**16
 * fits in 54-bits binary and guarantees positive binary values.
 * We still have to deal with the VR to/from FPR transfers but
 * that mechanism is already defined and at a reasonable cost
 * (2-4 cycles each way).
 *
 * \code
static inline vBCD_t
vec_BIN2BCD (vui64_t val)
{
#ifdef _ARCH_PWR6
  vBCD_t t;
  _Decimal128 x, y, z;
  // unpack the vector into a FPRp
  z = vec_unpack_Decimal128 ((vf64_t) val);
  // Convert 2 long int values into 2 _Decimal64 values
  // Then convert each _Decimal64 value into 16-digit BCD
  __asm__(
      "dcffix %1,%2;\n"
      "dcffix %L1,%L2;\n"
      "ddedpd 0,%0,%1;\n"
      "ddedpd 0,%L0,%L1;\n"
      : "=d" (x),
      "=&d" (y)
      : "d" (z)
      : );
  // Pack the FPRp back into a vector
  t = (vBCD_t) vec_pack_Decimal128 (x);
  return (t);
#else
  // no solution before P6
#endif
}
 * \endcode
 * If we assume that the second Decimal Convert From Fixed (dcffix) is
 * independent and issues 19 cycles after the first, we get 32+19 = 51
 * cycles to complete. Then another 13+1 cycles to convert back to BCD.
 * Add a few cycles for the unpack and pack operations and we estimate
 * 69 cycles for POWER8 and 58 cycles for POWER9. The totals for
 * vec_rdxcf10e32q() plus vec_BIN2BCD() come to 154-164 for POWER8 and
 * 114-124 for POWER9.
 * This is a 30-60% improvement over the previous (all vector) attempt.
 * So the final unsigned binary to BCD conversion looks like this:
 * \code
static inline vBCD_t
vec_bcdcfuq (vui128_t vra)
{
  vui64_t d10e;
  d10e =vec_rdxcf10e32q (vra);
#ifdef _ARCH_PWR7
  return (vBCD_t) vec_BIN2BCD (d10e);
#else
  vui8_t d100;
  vui16_t d10k;
  vui32_t d100m;
  d100m = vec_rdxcf10E16d (d10e);
  d10k = vec_rdxcf100mw (d100m);
  d100 = vec_rdxcf10kh (d10k);
  return (vBCD_t) vec_rdxcf100b (d10e);
#endif
}
 * \endcode
 * \note This process can be extended to 256, 512, 1024-bits, etc by
 * widening the first 5 steps appropriately and adding steps using
 * extended quadword multiply and add operations from vec_int128_ppc.h
 * (\ref int128_examples_0_1_3_1).
 *
 * \subsubsection bcd128_convert_0_2_3 Multiple precision BCD to/from Binary conversion
 *
 * The simplest case is converting a vector unsigned __int128 to BCD.
 * This requires up to 39 digits across two vectors. This can either be
 * split into 8 and 31 digits for signed conversion or 7 and 32 for
 * unsigned. Signed conversion is preferred where extended BCD result
 * will be input to additional BCD arithmetic. Unsigned is preferred
 * for conversion to Zoned characters for decimal display.
 *
 * From \ref int128_examples_0_1_2 we see the divide / modulo quadword
 * by constant operations which can used to factor binary quadwords
 * into high and low digit groups for conversion. For example:
 * \code
  q = vec_divuq_10e32 (a);
  r = vec_moduq_10e32 (a, q);
  // high 7 digits
  dh = vec_bcdcfuq (q);
  // lower 32 digits
  dl = vec_bcdcfuq (r);

  printf ("%07lld%016lld%016lld", (vui64_t) dh[VEC_DW_L],
          (vui64_t) dl[VEC_DW_H], (vui64_t) dl[VEC_DW_L]);
 * \endcode
 *
 * \paragraph bcd128_convert_0_2_3_0 Multiple precision BCD from Binary conversion
 *
 * The general multiple precision binary to BCD conversion requires
 * quadword long division as described in \ref int128_examples_0_1_3_1.
 * After each long division the remainder is in a range for conversion
 * to BCD. In the example below the remainder is converted to 32 digit
 * BCD as the last step.
 *
 * \code
// Convert extended quadword binary to BCD 32-digits at a time.
vBCD_t
example_longbcdcf_10e32 (vui128_t *q, vui128_t *d, long int _N)
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
  // convert to BCD and return the remainder for this step
  return vec_bcdcfuq (rh);
}
 * \endcode
 * Each call to example_longbcdcf_10e32 () produces the next 32-digit
 * group. Repeated calls where the previous iterations quotient is
 * passed as the dividend to the next step, produce additional 32-digit
 * groups. This continues until the quotient is less then the divisor
 * (in this case 10<SUP>32</SUP>). This final quadword quotient
 * provides the highest order 32-digit group for the conversion.
 * The digit groups are produced in order from lowest to highest
 * significance.
 *
 * As the conversion process continues the number of quadwords in the
 * extended dividend/quotient shrinks. The divide / modulo quadword
 * by constant operations test for leading zeros and skip over them.
 *
 * \paragraph bcd128_convert_0_2_3_1 Multiple precision BCD to Binary conversion
 *
 * The general multiple precision binary from BCD conversion only
 * requires extended quadword multiply as described in
 * \ref int128_examples_0_1_3_0.
 * Starting with the high order BCD (32 or 31) digit group, multiply by
 * 10<SUP>32</SUP> (or 10<SUP>31</SUP>) then add the next digit group
 * to the extended product. Continue until the low order digit group is
 * added. For example:
 *
 * \code
// Convert extended quadword BCD to binary 32-digits at a time.
long int
example_longbcdct_10e32 (vui128_t *d, vBCD_t decimal,
			  long int _C , long int _N)
{
  // ten32  = +100000000000000000000000000000000UQ
  const vui128_t ten32 = (vui128_t)
	  { (__int128) 10000000000000000UL * (__int128) 10000000000000000UL };
  const vui128_t zero = (vui128_t) { (__int128) 0UL };
  vui128_t dn, ph, pl, cn, c;
  long int i, cnt;

  cnt = _C;

  dn = zero;
  cn  = zero;
  // case _C == 0 is the initialization step and no multiply required
  if ( cnt == 0 )
    {
      // if the decimal is 0, no conversion is required
      if (vec_cmpuq_all_ne ((vui128_t) decimal, zero))
	{
	  cnt++;
	  dn = vec_bcdctuq (decimal);
	}
      // But it is a good time to initialize d[]
      for ( i = 0; i < (_N - 1); i++ )
	{
	  d[i] = zero;
	}
      d[_N - cnt] = dn;
    }
  else // case _C > 0
    {
      // convert decimal group to binary.
      if (vec_cmpuq_all_ne ((vui128_t) decimal, zero))
	{
	  dn = vec_bcdctuq (decimal);
	}
      // Compute extended product, plus the decimal group
      for ( i = (_N - 1); i >= (_N - cnt); i--)
	{
	  pl = vec_muludq (&ph, d[i], ten32);

	  c = vec_addecuq (pl, dn, cn);
	  d[i] = vec_addeuqm (pl, dn, cn);
	  cn = c;
	  dn = ph;
	}
      // If the product exceeds the current quadword count, extend
      if (vec_cmpuq_all_ne (dn, zero) || vec_cmpuq_all_ne (cn, zero))
	{
	  cnt++;
	  dn = vec_adduqm (dn, cn);
	  d[_N - cnt] = dn;
	}
    }

  return cnt;
}
 * \endcode
 * This process starts with a single quadword (the converted high order
 * digit group). As additional digit groups are converted, the extended
 * binary value is multiplied by 10<SUP>32</SUP> before adding the
 * converted digit group.  The number of quadwords in the array
 * <I>d[]</I> expand as needed to hold the binary value.
 *
 * The interface includes:
 * - A pointer to an array of quadwords which accumulates the
 * converted binary value.
 * - A BCD decimal value to be converted and added to the accumulated
 * binary.
 * - A current quadword count. The number of nonzero quadwords
 * accumulated so far. Should be 0 on the initial call.
 * - A maximum quadword count.
 * - Return the updated quadword count. Passed back as current
 * quadword count on the next iteration.
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
/*! \brief vector vector bool from 128-bit signed BCD integer. */
#define vbBCD_t vb32_t

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
static inline vBCD_t vec_bcdcfuq (vui128_t vra);
static inline vui128_t vec_bcdctuq (vBCD_t vra);
static inline vBCD_t vec_bcdsrqi (vBCD_t vra, const unsigned int _N);
static inline vBCD_t vec_bcdsub (vBCD_t a, vBCD_t b);
static inline vBCD_t vec_bcdus (vBCD_t vra, vi8_t vrb);
static inline vf64_t vec_pack_Decimal128 (_Decimal128 lval);
static inline _Decimal128 vec_quantize0_Decimal128 (_Decimal128 val);
static inline vui8_t vec_rdxcf100b (vui8_t vra);
static inline vui8_t vec_rdxcf10kh (vui16_t vra);
static inline vui16_t vec_rdxcf100mw (vui32_t vra);
static inline vui32_t vec_rdxcf10E16d (vui64_t vra);
static inline vui64_t vec_rdxcf10e32q (vui128_t vra);
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

/** \brief Convert vector of 2 x unsigned 16-digit BCD values
 * to vector 2 x doubleword binary values.
 *
 * Convert a vector of 16-digit unsigned BCD doublewords to a
 * vector of unsigned long int doublewords.
 * The vector unsigned long int doublewords are in the range
 * 0-9999999999999999.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |  55   |1/51 cycle|
 * |power9   |  59   |1/53 cycle|
 *
 * @param val a vector treated a 2 unsigned BCD 16 digit values.
 * @return a 128-bit vector unsigned long int.
 */
static inline vui64_t
vec_BCD2BIN (vBCD_t val)
{
#ifdef _ARCH_PWR6
  vui64_t t;
  _Decimal128 x, y, z;
  // unpack the vector into a FPRp
  z = vec_unpack_Decimal128 ((vf64_t) val);
  // Convert 2 long int values into 2 _Decimal64 values
  // Then convert each _Decimal64 value into 16-digit BCD
  __asm__(
      "denbcd 0,%1,%2;\n"
      "denbcd 0,%L1,%L2;\n"
      "dctfix %0,%1;\n"
      "dctfix %L0,%L1;\n"
      : "=d" (x),
      "=&d" (y)
      : "d" (z)
      : );
  // Pack the FPRp back into a vector
  t = (vui64_t) vec_pack_Decimal128 (x);
  return (t);
#else
  // todo no solution before P6
#endif
}

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

/** \brief Convert vector unsigned doubleword binary values to
 * Vector unsigned 16-digit BCD values
 *
 * Convert a vector of 2 unsigned long int doubleword to 2 16-digit
 * unsigned BCD doublewords.
 * Input doublewords should each be in the range
 * 0-9999999999999999.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |  69   |1/19 cycle|
 * |power9   |  58   |1/21 cycle|
 *
 * @param val a vector unsigned long int.
 * @return a 128-bit vector treated a 2 unsigned BCD 16 digit values.
 */
static inline vBCD_t
vec_BIN2BCD (vui64_t val)
{
#ifdef _ARCH_PWR6
  vBCD_t t;
  _Decimal128 x, y, z;
  // unpack the vector into a FPRp
  z = vec_unpack_Decimal128 ((vf64_t) val);
  // Convert 2 long int values into 2 _Decimal64 values
  // Then convert each _Decimal64 value into 16-digit BCD
  __asm__(
      "dcffix %1,%2;\n"
      "dcffix %L1,%L2;\n"
      "ddedpd 0,%0,%1;\n"
      "ddedpd 0,%L0,%L1;\n"
      : "=d" (x),
      "=&d" (y)
      : "d" (z)
      : );
  // Pack the FPRp back into a vector
  t = (vBCD_t) vec_pack_Decimal128 (x);
  return (t);
#else
  // todo no solution before P6
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
 * \note This operation will only detect overflows where the operand
 * signs match. It will not detect a borrow if the signs differ.
 * So this operation should only be used if matching signs are
 * guaranteed. Otherwise vec_cbcdaddcsq() should be used.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   | 15-21 | 1/cycle  |
 * |power9   | 6-18  | 2/cycle  |
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
#ifdef _ARCH_PWR9
  // Generate BCD zero from (a - a), which is 3 cycles on PWR9
  t = vec_bcdsub (a,  a);
#else // Else load a BCD const 0.
  t = _BCD_CONST_ZERO;
#endif
  if (__builtin_expect (__builtin_bcdadd_ov ((vi128_t) a, (vi128_t) b, 0), 0))
    {
      vBCD_t a_b;
      a_b = vec_bcdadd (a, b);
      t = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, a_b);
    }
#else
  _Decimal128 d_a, d_b, d_s, d_t;
  const vui32_t mz = CONST_VINT128_W(0, 0, 0, 0x0000000d);
  d_a = vec_BCD2DFP (a);
  d_b = vec_BCD2DFP (b);
  d_s = d_a + d_b;
  // Shift right 31 digits, leaving the carry.
  d_t = __builtin_dscriq (d_s, 31);
  t = vec_DFP2BCD (d_t);
  // fix up spurious negative zeros
  if (vec_all_eq ((vui32_t) t, mz))
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
 * \note This operation will only detect overflows where the operand
 * signs match. It will not detect a borrow if the signs differ.
 * So this operation should only be used if matching signs are
 * guaranteed. Otherwise vec_cbcdaddecsq() should be used.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   | 28-37 | 1/cycle  |
 * |power9   | 9-21  | 2/cycle  |
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
  vBCD_t a_b, a_b_c;

  a_b = vec_bcdadd (a, b);
  if (__builtin_expect (__builtin_bcdadd_ov ((vi128_t) a, (vi128_t) b, 0), 0))
    {
      t = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, a_b);
    }
  else // (a + b) did not overflow, what about (a + b + c)
    {
      a_b_c = (vBCD_t) vec_bcdadd (a_b, c);
      if (__builtin_bcdadd_ov ((vi128_t) a_b, (vi128_t) c, 0))
	{
	  t = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, a_b_c);
	}
      else
	{
#ifdef _ARCH_PWR9
	  // Generate BCD zero from (a - a), which is 3 cycles on PWR9
	  t = vec_bcdsub (a, a);
#else     // Else load a BCD const 0.
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
  if (vec_all_eq ((vui32_t) t, mz))
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

/** \brief Vector Decimal Convert From Signed Quadword
 *  returning up to 31 BCD digits.
 *
 *  Vector convert a quadword containing a signed __int128 in the
 *  range -9999999999999999999999999999999 to
 *  +9999999999999999999999999999999 to the equivalent
 *  signed BCD value with up to 31 digits.
 *
 *  \note If the signed value of vrb is less then -(10**31-1)
 *  or greater than 10**31-1 the result is too large for the BCD format
 *  and the result is undefined.
 *  See \ref int128_examples_0_1_2 for details.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |166-176|1/19 cycle|
 *  |power9   |  37   |1/26 cycle|
 *
 *  @param vrb a 128-bit vector as an signed __int128
 *  number in the range -9999999999999999999999999999999 to
 *  +9999999999999999999999999999999.
 *  @return 128-bit vector signed BCD value in
 *  the range -9999999999999999999999999999999 to
 *  +9999999999999999999999999999999.
 */
static inline vBCD_t
vec_bcdcfsq (vi128_t vrb)
{
  vBCD_t result;
#ifdef _ARCH_PWR9
  __asm__(
      "bcdcfsq. %0,%1,0;\n"
      : "=v" (result)
      : "v" (vrb)
      : "cr6" );
#else
  const vui128_t zero = (vui128_t) vec_splats ((int) 0);
  vBCD_t ubcd, bcdsign;
  vui128_t uvrb;
  vb128_t negbool;

  bcdsign = _BCD_CONST_PLUS_ONE;
  negbool = vec_setb_sq (vrb);

  uvrb = (vui128_t) vec_sel ((vui32_t) vrb,
			     (vui32_t) vec_subuqm (zero, (vui128_t) vrb),
			     (vb32_t) negbool);
  bcdsign = (vBCD_t) vec_sel ((vui32_t) bcdsign, (vui32_t) _BCD_CONST_MINUS_ONE,
			      (vb32_t) negbool);

  ubcd = vec_bcdcfuq (uvrb);

  result = (vBCD_t) vec_slqi ((vui128_t) ubcd, 4);
  result = vec_bcdcpsgn (result, bcdsign);
#endif
  return (vBCD_t) result;
}

/** \brief Vector Decimal Convert From Unsigned doubleword
 *  returning up to 2x16 BCD digits.
 *
 *  Vector convert doubleword containing a unsigned long int
 *  each in the range 0-9999999999999999 to the equivalent
 *  unsigned BCD doubleword value each up to 16 digits.
 *
 *  \note If either doubleword of vrb is greater than 10**16-1
 *  the result is too large for the BCD format
 *  and the result is undefined.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  69   |1/19 cycle|
 *  |power9   |  58   |1/21 cycle|
 *
 *  @param vrb a 128-bit vector of unsigned long int
 *  numbers, each in the range 0-9999999999999999.
 *  @return 128-bit vector doublewords of unsigned BCD values each in
 *  the range 0-9999999999999999.
 */
static inline vBCD_t
vec_bcdcfud (vui64_t vrb)
{
#ifdef _ARCH_PWR7
  return vec_BIN2BCD (vrb);
#else
  vui8_t d100;
  vui16_t d10k;
  vui32_t d100m;
  d100m = vec_rdxcf10E16d (vrb);
  d10k = vec_rdxcf100mw (d100m);
  d100 = vec_rdxcf10kh (d10k);
  return (vBCD_t) vec_rdxcf100b (d100);
#endif
}

/** \brief Vector Decimal Convert From Unsigned Quadword
 *  returning up to 32 BCD digits.
 *
 *  Vector convert a quadword containing a unsigned __int128 in the
 *  range 0-99999999999999999999999999999999 to the equivalent
 *  unsigned BCD value with up to 32 digits.
 *
 *  \note If the value of vrb is greater than 10**32-1 the result is
 *  too large for the unsigned BCD format and the result is undefined.
 *  See \ref int128_examples_0_1_2 for details.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |154-164|1/19 cycle|
 *  |power9   |117-128|1/21 cycle|
 *
 *  @param vra a 128-bit vector as an unsigned __int128
 *  number in the range 0-99999999999999999999999999999999.
 *  @return 128-bit vector unsigned BCD value in
 *  the range 0-99999999999999999999999999999999.
 */
static inline vBCD_t
vec_bcdcfuq (vui128_t vra)
{
  vui64_t d10e;
  d10e =vec_rdxcf10e32q (vra);
#ifdef _ARCH_PWR7
  return (vBCD_t) vec_BIN2BCD (d10e);
#else
  vui8_t d100;
  vui16_t d10k;
  vui32_t d100m;
  d100m = vec_rdxcf10E16d (d10e);
  d10k = vec_rdxcf100mw (d100m);
  d100 = vec_rdxcf10kh (d10k);
  return (vBCD_t) vec_rdxcf100b (d10e);
#endif
}

/** \brief Vector Decimal Convert From Zoned.
 *
 * Given a Signed 16-digit signed Zoned value vrb,
 * return equivalent Signed BCD value.
 * For Zoned (PS=0) the sign code is in bits 0:3 of byte 15.
 * - Positive sign codes are: 0x0, 0x1, 0x2, 0x3, 0x8, 0x9, 0xa, 0xb.
 * - Negative sign codes are: 0x4, 0x5, 0x6, 0x7, 0xc, 0xd, 0xe, 0xf.
 *
 * The resulting BCD value with up to 16 digits magnitude and
 * set to the preferred BCD sign (0xc or 0xd).
 *
 * \note The POWER9 bcdcfz instruction gives
 * undefined results if given invalid input.
 * In this implementation for older processors there is no checking
 * for Zone (bits 0:3) or digit (bits 4:7) range.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   | 14-27 | 1/cycle  |
 * |power9   |   3   | 2/cycle  |
 *
 * @param vrb a 128-bit vector treated as a signed Zoned 16 digit value.
 * @return a 128-bit BCD value with the magnitude and sign from the
 * Zoned value in vrb.
 */
static inline vBCD_t
vec_bcdcfz (vui8_t vrb)
{
  vBCD_t vrt;
#ifdef _ARCH_PWR9
  __asm__(
      "bcdcfz. %0,%1,0;\n"
      : "=v" (vrt)
      : "v" (vrb)
      : "cr6" );
#else
  const vui8_t dmask = vec_splat_u8(15);
  const vui8_t dx0 = vec_splat_u8(0);
  vui8_t znd_s;
  vui8_t znd_d, znd_t;
  vui8_t bcd, bcd_h, bcd_l;
  // Isolate the BCD digit from each zoned character.
  znd_d = vec_and (vrb, dmask);
  znd_t = (vui8_t) vec_srqi ((vui128_t) znd_d, 4);
  // Isolate the bit (1) that matters in the Zoned sign code.
  znd_s = vec_slbi (vrb, 1);
  znd_s = vec_srbi (znd_s, 7);
  // Convert to BCD preferred sign code 0xC or 0xD
  znd_s = vec_or (znd_s, (vui8_t) _BCD_CONST_ZERO);
  // Pack the even/odd zone digits into a single vector.
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  bcd = vec_pack ((vui16_t) znd_d, (vui16_t) znd_t);
#else
  bcd = vec_pack ((vui16_t) znd_t, (vui16_t) znd_d);
#endif
  // Swap even/odd DWs to low half and OR to get unsigned 16-digit BCD.
  bcd_l = (vui8_t) vec_mrgald ((vui128_t) dx0, (vui128_t) bcd);
  bcd_h = (vui8_t) vec_mrgahd ((vui128_t) dx0, (vui128_t) bcd);
  bcd = vec_or (bcd_h, bcd_l);
  // Shift left to make room for sign code
  vrt = (vBCD_t) vec_slqi ((vui128_t) bcd, 4);
  // Merge sign code from zone code.
  vrt = vec_bcdcpsgn (vrt, (vBCD_t) znd_s);
#endif
  return (vrt);
}

/** \brief Vector Compare Signed BCD Quadword for equal.
 *
 *  Compare vector signed BCD values and return vector bool true if
 *  vra and vrb are equal.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 15-17 | 1/cycle  |
 *  |power9   | 6-9   | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector signed
 *  BCD (qword) element.
 *  @param vrb 128-bit vector treated as an vector signed
 *  BCD (qword) element.
 *  @return 128-bit vector boolean reflecting vector signed BCD
 *  compare equal.
 */
static inline vbBCD_t
vec_bcdcmp_eqsq (vBCD_t vra, vBCD_t vrb)
{
  vbBCD_t result = (vbBCD_t) vec_splat_s32 (0);
#ifdef _ARCH_PWR8
  if (__builtin_bcdsub_eq ((vi128_t) vra, (vi128_t) vrb, 0))
    result = (vbBCD_t) vec_splat_s32 (-1);
#else
  _Decimal128 d_a, d_b;
  d_a = vec_BCD2DFP (vra);
  d_b = vec_BCD2DFP (vrb);
  if (d_a == d_b)
    result = (vbBCD_t) vec_splat_s32 (-1);
#endif
  return result;
}

/** \brief Vector Compare Signed BCD Quadword for greater than or equal.
 *
 *  Compare vector signed BCD values and return vector bool true if
 *  vra and vrb are greater than or equal.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 15-17 | 1/cycle  |
 *  |power9   | 6-9   | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector signed
 *  BCD (qword) element.
 *  @param vrb 128-bit vector treated as an vector signed
 *  BCD (qword) element.
 *  @return 128-bit vector boolean reflecting vector signed BCD
 *  compare greater than or equal.
 */
static inline vbBCD_t
vec_bcdcmp_gesq (vBCD_t vra, vBCD_t vrb)
{
  vbBCD_t result = (vbBCD_t) vec_splat_s32 (-1);
#ifdef _ARCH_PWR8
  if (__builtin_bcdsub_lt ((vi128_t) vra, (vi128_t) vrb, 0))
    result = (vbBCD_t) vec_splat_s32 (0);
#else
  _Decimal128 d_a, d_b;
  d_a = vec_BCD2DFP (vra);
  d_b = vec_BCD2DFP (vrb);
  if (d_a < d_b)
    result = (vbBCD_t) vec_splat_s32 (0);
#endif
  return result;
}

/** \brief Vector Compare Signed BCD Quadword for greater than.
 *
 *  Compare vector signed BCD values and return vector bool true if
 *  vra and vrb are greater than.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 15-17 | 1/cycle  |
 *  |power9   | 6-9   | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector signed
 *  BCD (qword) element.
 *  @param vrb 128-bit vector treated as an vector signed
 *  BCD (qword) element.
 *  @return 128-bit vector boolean reflecting vector signed BCD
 *  compare greater than.
 */
static inline vbBCD_t
vec_bcdcmp_gtsq (vBCD_t vra, vBCD_t vrb)
{
  vbBCD_t result = (vbBCD_t) vec_splat_s32 (0);
#ifdef _ARCH_PWR8
  if (__builtin_bcdsub_gt ((vi128_t) vra, (vi128_t) vrb, 0))
    result = (vbBCD_t) vec_splat_s32 (-1);
#else
  _Decimal128 d_a, d_b;
  d_a = vec_BCD2DFP (vra);
  d_b = vec_BCD2DFP (vrb);
  if (d_a > d_b)
    result = (vbBCD_t) vec_splat_s32 (-1);
#endif
  return result;
}

/** \brief Vector Compare Signed BCD Quadword for less than or equal.
 *
 *  Compare vector signed BCD values and return vector bool true if
 *  vra and vrb are less than or equal.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 15-17 | 1/cycle  |
 *  |power9   | 6-9   | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector signed
 *  BCD (qword) element.
 *  @param vrb 128-bit vector treated as an vector signed
 *  BCD (qword) element.
 *  @return 128-bit vector boolean reflecting vector signed BCD
 *  compare less than or equal.
 */
static inline vbBCD_t
vec_bcdcmp_lesq (vBCD_t vra, vBCD_t vrb)
{
  vbBCD_t result = (vbBCD_t) vec_splat_s32 (-1);
#ifdef _ARCH_PWR8
  if (__builtin_bcdsub_gt ((vi128_t) vra, (vi128_t) vrb, 0))
    result = (vbBCD_t) vec_splat_s32 (0);
#else
  _Decimal128 d_a, d_b;
  d_a = vec_BCD2DFP (vra);
  d_b = vec_BCD2DFP (vrb);
  if (d_a > d_b)
    result = (vbBCD_t) vec_splat_s32 (0);
#endif
  return result;
}

/** \brief Vector Compare Signed BCD Quadword for less than.
 *
 *  Compare vector signed BCD values and return vector bool true if
 *  vra and vrb are less than.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 15-17 | 1/cycle  |
 *  |power9   | 6-9   | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector signed
 *  BCD (qword) element.
 *  @param vrb 128-bit vector treated as an vector signed
 *  BCD (qword) element.
 *  @return 128-bit vector boolean reflecting vector signed BCD
 *  compare less than.
 */
static inline vbBCD_t
vec_bcdcmp_ltsq (vBCD_t vra, vBCD_t vrb)
{
  vbBCD_t result = (vbBCD_t) vec_splat_s32 (0);
#ifdef _ARCH_PWR8
  if (__builtin_bcdsub_lt ((vi128_t) vra, (vi128_t) vrb, 0))
    result = (vbBCD_t) vec_splat_s32 (-1);
#else
  _Decimal128 d_a, d_b;
  d_a = vec_BCD2DFP (vra);
  d_b = vec_BCD2DFP (vrb);
  if (d_a < d_b)
    result = (vbBCD_t) vec_splat_s32 (-1);
#endif
  return result;
}

/** \brief Vector Compare Signed BCD Quadword for not equal.
 *
 *  Compare vector signed BCD values and return vector bool true if
 *  vra and vrb are not equal.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 15-17 | 1/cycle  |
 *  |power9   | 6-9   | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector signed
 *  BCD (qword) element.
 *  @param vrb 128-bit vector treated as an vector signed
 *  BCD (qword) element.
 *  @return 128-bit vector boolean reflecting vector signed BCD
 *  compare not equal.
 */
static inline vbBCD_t
vec_bcdcmp_nesq (vBCD_t vra, vBCD_t vrb)
{
#ifdef _ARCH_PWR8
  vbBCD_t result = (vbBCD_t) vec_splat_s32 (-1);
  if (__builtin_bcdsub_eq ((vi128_t) vra, (vi128_t) vrb, 0))
    result = (vbBCD_t) vec_splat_s32 (0);

  return result;
#else
  /* vec_cmpneuq works for both signed and unsigned compares.  */
  return (vbBCD_t) vec_cmpneuq ((vui128_t) vra, (vui128_t) vrb);
#endif
}

/** \brief Vector Compare Signed BCD Quadword for equal.
 *
 *  Compare vector signed BCD values and return boolean true if
 *  vra and vrb are equal.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   13  | 1/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector signed
 *  BCD (qword) element.
 *  @param vrb 128-bit vector treated as an vector signed
 *  BCD (qword) element.
 *  @return boolean int for BCD compare, true if equal,
 *  false otherwise.
 */
static inline int
vec_bcdcmpeq (vBCD_t vra, vBCD_t vrb)
{
#ifdef _ARCH_PWR8
  return __builtin_bcdsub_eq ((vi128_t) vra, (vi128_t) vrb, 0);
#else
  _Decimal128 d_a, d_b;
  d_a = vec_BCD2DFP (vra);
  d_b = vec_BCD2DFP (vrb);
  return (d_a == d_b);
#endif
}

/** \brief Vector Compare Signed BCD Quadword for greater than or equal.
 *
 *  Compare vector signed BCD values and return boolean true if
 *  vra and vrb are greater than or equal.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   13  | 1/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector signed
 *  BCD (qword) element.
 *  @param vrb 128-bit vector treated as an vector signed
 *  BCD (qword) element.
 *  @return boolean int for BCD compare, true if greater than or equal,
 *  false otherwise.
 */
static inline int
vec_bcdcmpge (vBCD_t vra, vBCD_t vrb)
{
#ifdef _ARCH_PWR8
  return !__builtin_bcdsub_lt ((vi128_t) vra, (vi128_t) vrb, 0);
#else
  _Decimal128 d_a, d_b;
  d_a = vec_BCD2DFP (vra);
  d_b = vec_BCD2DFP (vrb);
  return (d_a >= d_b);
#endif
}

/** \brief Vector Compare Signed BCD Quadword for greater than.
 *
 *  Compare vector signed BCD values and return boolean true if
 *  vra and vrb are greater than.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   13  | 1/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector signed
 *  BCD (qword) element.
 *  @param vrb 128-bit vector treated as an vector signed
 *  BCD (qword) element.
 *  @return boolean int for BCD compare, true if greater than,
 *  false otherwise.
 */
static inline int
vec_bcdcmpgt (vBCD_t vra, vBCD_t vrb)
{
#ifdef _ARCH_PWR8
  return __builtin_bcdsub_gt ((vi128_t) vra, (vi128_t) vrb, 0);
#else
  _Decimal128 d_a, d_b;
  d_a = vec_BCD2DFP (vra);
  d_b = vec_BCD2DFP (vrb);
  return (d_a > d_b);
#endif
}

/** \brief Vector Compare Signed BCD Quadword for less than or equal.
 *
 *  Compare vector signed BCD values and return boolean true if
 *  vra and vrb are less than or equal.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   13  | 1/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector signed
 *  BCD (qword) element.
 *  @param vrb 128-bit vector treated as an vector signed
 *  BCD (qword) element.
 *  @return boolean int for BCD compare, true if less than or equal,
 *  false otherwise.
 */
static inline int
vec_bcdcmple (vBCD_t vra, vBCD_t vrb)
{
#ifdef _ARCH_PWR8
  return !__builtin_bcdsub_gt ((vi128_t) vra, (vi128_t) vrb, 0);
#else
  _Decimal128 d_a, d_b;
  d_a = vec_BCD2DFP (vra);
  d_b = vec_BCD2DFP (vrb);
  return (d_a <= d_b);
#endif
}

/** \brief Vector Compare Signed BCD Quadword for less than.
 *
 *  Compare vector signed BCD values and return boolean true if
 *  vra and vrb are less than.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   13  | 1/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector signed
 *  BCD (qword) element.
 *  @param vrb 128-bit vector treated as an vector signed
 *  BCD (qword) element.
 *  @return boolean int for BCD compare, true if less than,
 *  false otherwise.
 */
static inline int
vec_bcdcmplt (vBCD_t vra, vBCD_t vrb)
{
#ifdef _ARCH_PWR8
  return __builtin_bcdsub_lt ((vi128_t) vra, (vi128_t) vrb, 0);
#else
  _Decimal128 d_a, d_b;
  d_a = vec_BCD2DFP (vra);
  d_b = vec_BCD2DFP (vrb);
  return (d_a < d_b);
#endif
}

/** \brief Vector Compare Signed BCD Quadword for not equal.
 *
 *  Compare vector signed BCD values and return boolean true if
 *  vra and vrb are not equal.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   13  | 1/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector signed
 *  BCD (qword) element.
 *  @param vrb 128-bit vector treated as an vector signed
 *  BCD (qword) element.
 *  @return boolean int for BCD compare, true if not equal,
 *  false otherwise.
 */
static inline int
vec_bcdcmpne (vBCD_t vra, vBCD_t vrb)
{
#ifdef _ARCH_PWR8
  return !__builtin_bcdsub_eq ((vi128_t) vra, (vi128_t) vrb, 0);
#else
  return vec_cmpuq_all_ne ((vui128_t) vra, (vui128_t) vrb);
#endif
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
  const vui32_t sign_mask = (vui32_t) _BCD_CONST_SIGN_MASK;
  vrt = (vBCD_t) vec_sel ((vui32_t) vra, (vui32_t) vrb, sign_mask);
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
 *  |power9   |  23   |1/12 cycle|
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
#ifdef _ARCH_PWR7
  return vec_BCD2BIN (vra);
#else
  vui8_t d100;
  vui16_t d10k;
  vui32_t d100m;
  d100 = vec_rdxct100b ((vui8_t) vra);
  d10k = vec_rdxct10kh (d100);
  d100m = vec_rdxct100mw (d10k);
  return vec_rdxct10E16d (d100m);
#endif
}

/** \brief Vector Decimal Convert groups of 32 BCD digits
 *  to binary unsigned quadword.
 *
 *  Vector convert a quadword containing 32 BCD digits to the
 *  equivalent unsigned __int128, in the range
 *  0-99999999999999999999999999999999.
 *  Input values should be valid 32 x BCD nibbles in the range 0-9.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 65-78 | 1/cycle  |
 *  |power9   | 28-37 |1/12 cycle|
 *
 *  @param vra a 128-bit vector treated as an unsigned 32-digit BCD
 *  number.
 *  @return 128-bit vector unsigned __int128 in
 *  the range 0-99999999999999999999999999999999.
 */
static inline vui128_t
vec_bcdctuq (vBCD_t vra)
{
  vui128_t vrt;
#ifdef  _ARCH_PWR9
  const vui32_t bcd_one = (vui32_t) _BCD_CONST_PLUS_ONE;
  const vui32_t sign_mask = (vui32_t) _BCD_CONST_SIGN_MASK;
  vui128_t vrd;
  vBCD_t sbcd;
  // Need to convert BCD unsigned to signed for bcdctsq
  // But can't use bcdcpsgn as the unit digit is not a sign code
  // So use vec_and/sel to extract unit digit and insert sign
  vrd = (vui128_t) vec_and ((vui32_t) vra, sign_mask);
  sbcd = (vBCD_t) vec_sel ((vui32_t) vra, bcd_one, sign_mask);
  // Convert top 31 digits to binary
  vrt = (vui128_t) vec_bcdctsq (sbcd);
  // Then X 10 plus the unit digit to complete 32-digit convert
  vrt = vec_mul10euq (vrt, vrd);
#else
  vui64_t d10e;
#ifdef _ARCH_PWR7
  d10e = vec_BCD2BIN (vra);
#else
  vui8_t d100;
  vui16_t d10k;
  vui32_t d100m;
  d100 = vec_rdxct100b ((vui8_t) vra);
  d10k = vec_rdxct10kh (d100);
  d100m = vec_rdxct100mw (d10k);
  d10e = vec_rdxct10E16d (d100m);
#endif
  vrt = vec_rdxct10e32q (d10e);
#endif
  return vrt;
}

/** \brief Vector Decimal Convert To Zoned.
 *
 * Given a Signed 16-digit signed BCD value vrb,
 * return equivalent Signed Zoned value.
 * For Zoned (PS=0) the sign code is in bits 0:3 of byte 15.
 * - Positive sign Zone is: 0x30.
 * - Negative sign Zone is: 0x70.
 *
 * The resulting Zone value will up to 16 digits magnitude and
 * set to the preferred Zoned sign codes (0x30 or 0x70).
 *
 * \note The POWER9 bcdctz instruction gives
 * undefined results if given invalid input.
 * In this implementation for older processors there is no checking
 * for BCD digit (bits 4:7) range.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   | 14-27 | 1/cycle  |
 * |power9   |   3   | 2/cycle  |
 *
 * @param vrb a 128-bit vector treated as a signed BCD 16 digit value.
 * @return a 128-bit Zoned value with the magnitude (low order
 * 16-digits) and sign from the value in vrb.
 */
static inline vui8_t
vec_bcdctz (vBCD_t vrb)
{
  vui8_t vrt;
#ifdef _ARCH_PWR9
  __asm__(
      "bcdctz. %0,%1,0;\n"
      : "=v" (vrt)
      : "v" (vrb)
      : "cr6" );
#else
  const vui8_t dmask = vec_splat_u8(15);
  const vui8_t zone_minus = CONST_VINT128_B ( '0', '0', '0', '0',
					      '0', '0', '0', '0',
					      '0', '0', '0', '0',
					      '0', '0', '0', 0x70 );
//  const vui32_t minus_sign = (vui32_t) CONST_VINT128_W(0x0b, 0x0d, 0x0b, 0x0d);
  const vui32_t plus_sign = (vui32_t) CONST_VINT128_W(0x0a, 0x0c, 0x0e, 0x0f);
  vui32_t sign_splat;
  const vui32_t bcd_sign_mask = vec_splat_u32(15);
  vui8_t znd_s, znd_d, znd_t;
  vui8_t bcd_s, bcd_u;
  vui8_t zone_code;
  // Isolate the BCD Sign code
  bcd_s = vec_and ((vui8_t) vrb, (vui8_t) bcd_sign_mask);
  // Replicate the byte containing the sign to words
  sign_splat = vec_splat ((vui32_t) bcd_s, VEC_W_L);
  // Isolate the low 16 digits as unsigned BCD
  bcd_u = (vui8_t) vec_srqi ((vui128_t) vrb, 4);
  // Isolate the even/odd nibbles and merge low bytes for zoned
  znd_d = vec_and (bcd_u, dmask);
  znd_t = vec_srbi (bcd_u, 4);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  znd_s = vec_mergeh (znd_d, znd_t);
#else
  znd_s = vec_mergel (znd_t, znd_d);
#endif
  // Initialize the zone_code with negative zone mask.
  zone_code = zone_minus;
  // SIMD compare for match to any positive sign code
  if (vec_any_eq(sign_splat, plus_sign))
    // Convert to positive zone mask.
    zone_code = (vui8_t) vec_xxspltd ((vui64_t) zone_code, 0);

  // Merge the zone nibbles with the digit nibble to
  vrt = vec_or (znd_s, zone_code);
#endif
  return (vrt);
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

/** \brief Decimal Divide Extended.
 *
 * The dividend <I>a</I> is a Signed BCD 31 digit value extended to
 * right internally with 31 decimal 0s.
 * The divisor <I>b</I> is Signed BCD 31 digit value.
 * The quotient of <I>a || 0<SUP>31</SUP></I> / <I>b</I> is truncated
 * to a Decimal integer and returned in Signed BCD format.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |102-238| 1/cycle  |
 * |power9   | 96-228| 1/cycle  |
 *
 * @param a a 128-bit vector treated as the high 31-digits of a
 * 62-digit value extended with 0's.
 * @param b a 128-bit vector treated as a signed BCD 31 digit value.
 * @return a 128-bit vector quotient of (a / b).
 */
static inline vBCD_t
vec_bcddive (vBCD_t a, vBCD_t b)
{
  vBCD_t t;
  _Decimal128 d_t, d_a, d_b;
  d_a = vec_BCD2DFP (a);
  d_b = vec_BCD2DFP (b);
  // Look into using DFP Insert Biased Exponent here.
  d_a = d_a * 10E31DL;
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
 * To avoid overflow in the DFP Facility, we split each BCD operand
 * into 15 upper and 16 lower digit halves. This requires up to four
 * decimal multiplies and produces up to four 30-32 digit partial
 * products. These are aligned appropriately (via DFP decimal shift)
 * and summed (via DFP Decimal add) to generate the high and low
 * (31-digit) parts of the 62 digit product.
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
#ifndef _ARCH_PWR9
  const vui32_t mz = CONST_VINT128_W (0, 0, 0, 0x0000000d);
#endif
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
 * To avoid overflow in the DFP Facility, we split each BCD operand
 * into 15 upper and 16 lower digit halves. This requires up four
 * decimal multiplies and produces four 30-32 digit partial products.
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
#ifndef _ARCH_PWR9
  const vui32_t mz = CONST_VINT128_W (0, 0, 0, 0x0000000d);
#endif
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

/** \brief Vector Set preferred BCD Sign.
 *
 * Given a Signed BCD 31 digit value vrb, return the
 * magnitude from vrb (bits 0:123) and the sign (bits 124:127)
 * set to the preferred sign (0xc or 0xd).
 * Valid positive sign codes are; 0xA, 0xC, 0xE, or 0xF.
 * Valid negative sign codes are; 0xB or 0xD.
 *
 * \note The POWER9 bcdsetsgn instruction gives
 * undefined results if given invalid input.
 * In this implementation for older processors only the sign code is
 * checked. In this case, if the sign code is invalid the vrb input
 * value is returned unchanged.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   | 6-26  | 1/cycle  |
 * |power9   |   3   | 2/cycle  |
 *
 * @param vrb a 128-bit vector treated as a signed BCD 31 digit value.
 * @return a 128-bit BCD value with the magnitude from vra and the
 * sign copied from  vrb.
 */
static inline vBCD_t
vec_bcdsetsgn (vBCD_t vrb)
{
  vBCD_t vrt;
#ifdef _ARCH_PWR9
  __asm__(
      "bcdsetsgn. %0,%1,0;\n"
      : "=v" (vrt)
      : "v" (vrb)
      : "cr6" );
#else
  const vui32_t match_mask = vec_splat_u32(15);
  // The preferred sign is in the correct position for vec_bcdcpsgn
  const vui32_t minus_sign = (vui32_t) CONST_VINT128_W(0x0b, 0x0d, 0x0b, 0x0d);
  const vui32_t plus_sign = (vui32_t) CONST_VINT128_W(0x0a, 0x0f, 0x0e, 0x0c);
  vui32_t sign_splat;
  vui32_t sign_code;

  // Replicate the byte containing the sign to words
  sign_splat = vec_splat ((vui32_t) vrb, VEC_W_L);
  // Apply the code match mask
  sign_code = vec_and (sign_splat, match_mask);
  // SIMD compare for match to any positive sign code
  if (vec_any_eq (sign_code, plus_sign))
    vrt = vec_bcdcpsgn (vrb, (vBCD_t) plus_sign);
  else
    {
      // SIMD compare for match to any negative sign code
      if (vec_any_eq (sign_code, minus_sign))
	vrt = vec_bcdcpsgn (vrb, (vBCD_t) minus_sign);
      else
	vrt = vrb;
    }
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

/** \brief Decimal Shift and Round.
 * Shift a vector signed BCD value, left or right a variable
 * amount of digits (nibbles). The sign nibble is preserved.
 * If byte element 7 of the shift count is negative (right shift),
 * and the last digit shifted out is greater then or equal to 5,
 * then increment the shifted magnitude by 1.
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
vec_bcdsr (vBCD_t vra, vi8_t vrb)
{
  vBCD_t vrt;
#ifdef _ARCH_PWR9
  __asm__(
      "bcdsr. %0,%1,%2,0;\n"
      : "=v" (vrt)
      : "v" (vrb),
      "v" (vra)
      : "cr6" );
#else
  const vi8_t zero = vec_splat_s8(0);
  vi8_t shd = vec_splat (vrb, VEC_BYTE_L_DWH);
  vui128_t t;
  vui32_t r_d;
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
      // restore original sign nibble
      vrt = vec_bcdcpsgn ((vBCD_t) t, vra);
    }
  else
    {
      const vui32_t rnd6 = CONST_VINT128_W (0, 0, 0, (5+6));
      vBCD_t rnd_d;
      // Negative, shift right by absolute value
      shd = vec_sub (zero, shd);
      t = vec_srq (t, (vui128_t) shd);
      // extract the last digit shifted out for rounding.
      r_d = (vui32_t) vec_and ((vui32_t) t, (vui32_t) _BCD_CONST_SIGN_MASK);
      // Add decimal 6's +5 to generate rounding digit
      r_d = vec_add (r_d, rnd6);
      // Set the sign from original value
      rnd_d = vec_bcdcpsgn (r_d, vra);
      // restore original sign nibble
      vrt = vec_bcdcpsgn ((vBCD_t) t, vra);
      // round the last digit
      vrt = vec_bcdadd (vrt, rnd_d);
#ifdef _ARCH_PWR7
      // Special fixup for P7 via DFP. But in case of shift right
      // resulting in 0, the bcdadd above will return the preferred
      // +0, while bcdsr should not change the sign.
      vrt = vec_bcdcpsgn (vrt, vra);
#endif
    }
#endif
  return (vrt);
}

/** \brief Vector BCD Shift Right Signed Quadword Immediate
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

/** \brief Vector BCD Shift Right and Round Signed Quadword Immediate
 *
 * Shift and round a vector signed BCD value right _N digits.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   | 25-34 | 2/cycle  |
 * |power9   |  3-6  | 2/cycle  |
 *
 * @param vra 128-bit vector signed BCD 31 digit value.
 * @param _N int constant for the number of digits to shift right.
 * @return a 128-bit vector BCD value shifted right _N digits.
 */
static inline vBCD_t
vec_bcdsrrqi (vBCD_t vra, const unsigned int _N)
{
  vBCD_t vrt;
#ifdef _ARCH_PWR9
  vi8_t shd = vec_splats ((const signed char) (-_N));
  vrt = vec_bcdsr (vra, shd);
#else
  vui128_t t;
  vui32_t r_d;
  // Compare shift positive or negative
  if (_N < 32)
    {
      const vui32_t rnd6 = CONST_VINT128_W(0, 0, 0, (5 + 6));
      vBCD_t rnd_d;
      // Clear sign nibble before shift.
      t = (vui128_t) vec_andc ((vui32_t) vra, (vui32_t) _BCD_CONST_SIGN_MASK);
      t = vec_srqi (t, (_N * 4));
      // extract the last digit shifted out for rounding.
      r_d = (vui32_t) vec_and ((vui32_t) t, (vui32_t) _BCD_CONST_SIGN_MASK);
      // Add decimal 6's +5 to generate rounding digit
      r_d = vec_add (r_d, rnd6);
      // Set the sign from original value
      rnd_d = vec_bcdcpsgn (r_d, vra);
      // restore original sign nibble
      vrt = vec_bcdcpsgn ((vBCD_t) t, vra);
      // round the last digit
      vrt = vec_bcdadd (vrt, rnd_d);
#ifdef _ARCH_PWR7
      // Special fixup for P7 via DFP. But in case of shift right
      // resulting in 0, the bcdadd above will return the preferred
      // +0, while bcdsr should not change the sign.
      vrt = vec_bcdcpsgn (vrt, vra);
#endif
    }
  else
    {
      vrt = vra;
    }
#endif
  return (vrt);
}

/** \brief Vector BCD Shift Right Unsigned Quadword immediate
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
  // fix up spurious negative zeros
  if (vec_all_eq((vui32_t) t, mz))
    t = _BCD_CONST_ZERO;
#endif
  return (t);
}

/** \brief Decimal Sudtract & write Carry Signed Quadword.
 *
 * Two Signed 31 digit BCD values are subtracted, and the carry-out
 * (the high order 32nd digit) of the difference is returned.
 *
 * \note This operation will only detect overflows where the operand
 * signs differ. It will not detect a borrow if the signs match.
 * So this operation should only be used if differing signs are
 * guaranteed.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   | 15-21 | 1/cycle  |
 * |power9   | 6-18  | 2/cycle  |
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
  vBCD_t a_b;
#ifdef _ARCH_PWR9
  // Generate BCD zero from (a - a), which is 3 cycles on PWR9
  t = vec_bcdsub (a,  a);
#else // Else load a BCD const 0.
  t = _BCD_CONST_ZERO;
#endif
  if (__builtin_expect (__builtin_bcdsub_ov ((vi128_t) a, (vi128_t) b, 0), 0))
    {
      a_b = vec_bcdsub (a, b);
      t = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, a_b);
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
  if (vec_all_eq ((vui32_t) t, mz))
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
 * \note This operation will only detect overflows where the operand
 * signs differ. It will not detect a borrow if the signs match.
 * So this operation should only be used if differing signs are
 * guaranteed.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   | 28-37 | 1/cycle  |
 * |power9   | 9-21  | 2/cycle  |
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
  vBCD_t a_b, a_b_c;

  a_b = vec_bcdsub (a, b);
  if (__builtin_expect (__builtin_bcdsub_ov ((vi128_t) a, (vi128_t) b, 0), 0))
    {
      t = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, a_b);
    }
  else // (a - b) did not overflow, what about (a - b + c)
    {
      a_b_c = vec_bcdadd (a_b, c);
      if (__builtin_bcdadd_ov ((vi128_t) a_b, (vi128_t) c, 0))
	{
	  t = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, a_b_c);
	}
      else
	{
#ifdef _ARCH_PWR9
	  // Generate BCD zero from (a - a), which is 3 cycles on PWR9
	  t = vec_bcdsub (a, a);
#else     // Else load a BCD const 0.
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
  d_s = d_a - d_b + d_c;
  // Shift right 31 digits, leaving the carry.
  d_t = __builtin_dscriq (d_s, 31);
  t = vec_DFP2BCD (d_t);
  // fix up spurious negative zeros
  if (vec_all_eq ((vui32_t) t, mz))
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
  t = vec_bcdadd (vec_bcdsub (a, b), c);
#else
  const vui32_t mz = CONST_VINT128_W (0, 0, 0, 0x0000000d);
  _Decimal128 d_t;
  d_t = vec_BCD2DFP (a) - vec_BCD2DFP (b) + vec_BCD2DFP (c);
  t = vec_DFP2BCD(d_t);
  // fix up spurious negative zeros
  if (vec_all_eq ((vui32_t) t, mz))
    t = _BCD_CONST_ZERO;
#endif
  return (t);
}

/** \brief Decimal Truncate.
 * Truncate a vector signed BCD value vra to N-digits,
 * where N is the unsigned integer value in bits 48-63 of vrb.
 * The first 31-N digits are set to 0 and the result returned.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   | 18-27 | 1/cycle  |
 * |power9   |   3   | 2/cycle  |
 *
 * @param vra 128-bit vector treated as a signed BCD 31 digit value.
 * @param vrb Digit truncate count in vector halfword 3 (bits 48:63).
 * @return a 128-bit vector BCD value with the first 31-count digits
 * set to 0.
 */
static inline vBCD_t
vec_bcdtrunc (vBCD_t vra, vui16_t vrb)
{
  vBCD_t vrt;
#ifdef _ARCH_PWR9
  __asm__(
      "bcdtrunc. %0,%1,%2,0;\n"
      : "=v" (vrt)
      : "v" (vrb),
      "v" (vra)
      : "cr6" );
#else
  const vui16_t c124 = vec_splats ((unsigned short) 124);
  const vui16_t c4 = vec_splats ((unsigned short) 4);
  vui16_t shd = vec_splat (vrb, VEC_HW_L_DWH);
  vui128_t t;
  // Multiply digit shift by 4 to get bit shift count
  shd = vec_add (shd, shd);
  shd = vec_add (shd, shd);
  vui16_t one_s;
  // compensate for the sign nibble
  shd = vec_add (shd, c4);
  // generation all ones if in range, zeros if greater than
  one_s = (vui16_t) vec_cmple (shd, c124);
  // Generate a mask for the digits we will clear
  t = vec_slq ((vui128_t) one_s, (vui128_t) shd);
  // Clear the digits we are truncating
  vrt = (vBCD_t) vec_andc ((vui32_t)vra, (vui32_t) t);
#endif
  return (vrt);
}

/** \brief Decimal Truncate Quadword Immediate.
 * Truncate a vector signed BCD value vra to N-digits,
 * where N is a unsigned short integer constant.
 * The first 31-N digits are set to 0 and the result returned.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   | 6-17  | 1/cycle  |
 * |power9   |   6   | 2/cycle  |
 *
 * @param vra 128-bit vector treated as a signed BCD 31 digit value.
 * @param _N a unsigned short integer constant truncate count.
 * @return a 128-bit vector BCD value with the first 31-count digits
 * set to 0.
 */
static inline vBCD_t
vec_bcdtruncqi (vBCD_t vra, const unsigned short _N)
{
  vBCD_t vrt;
#ifdef _ARCH_PWR9
  vui16_t shd = vec_splats ((const unsigned short) (_N));
  vrt = vec_bcdtrunc (vra, shd);
#else
  vui128_t t;
  const vui16_t ones = vec_splat_u16(-1);
  // Compare shift < 32 (128-bits)
  if (_N < 31)
    {
      // Generate a mask for the digits we will keep
      t = vec_srqi ((vui128_t) ones, ((31 -_N) * 4));
      // Clear the digits we are truncating
      vrt = (vBCD_t) vec_and ((vui32_t) t, (vui32_t)vra);
    }
  else
    vrt = vra;
#endif
  return (vrt);
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

/** \brief Decimal Unsigned Truncate.
 * Truncate a vector unsigned BCD value vra to N-digits,
 * where N is the unsigned integer value in bits 48-63 of vrb.
 * The first 32-N digits are set to 0 and the result returned.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   | 16-25 | 1/cycle  |
 * |power9   |   3   | 2/cycle  |
 *
 * @param vra 128-bit vector treated as an unsigned BCD 32 digit value.
 * @param vrb Digit truncate count in vector halfword 3 (bits 48:63).
 * @return a 128-bit vector BCD value with the first 32-count digits
 * set to 0.
 */
static inline vBCD_t
vec_bcdutrunc (vBCD_t vra, vui16_t vrb)
{
  vBCD_t vrt;
#ifdef _ARCH_PWR9
  __asm__(
      "bcdutrunc. %0,%1,%2;\n"
      : "=v" (vrt)
      : "v" (vrb),
      "v" (vra)
      : "cr6" );
#else
  const vui16_t c128 = vec_splats ((unsigned short) 128);
  vui16_t shd = vec_splat (vrb, VEC_HW_L_DWH);
  vui16_t one_s;
  vui128_t t;
  // Multiply digit shift by 4 to get bit shift count
  shd = vec_add (shd, shd);
  shd = vec_add (shd, shd);
  // generation all ones if in range, zeros if greater than
  one_s = (vui16_t) vec_cmplt (shd, c128);
  // Generate a mask for the digits we will clear
  t = vec_slq ((vui128_t) one_s, (vui128_t) shd);
  // Clear the digits we are truncating
  vrt = (vBCD_t) vec_andc ((vui32_t)vra, (vui32_t) t);
#endif
  return (vrt);
}

/** \brief Decimal Unsigned Truncate Quadword Immediate.
 * Truncate a vector unsigned BCD value vra to N-digits,
 * where N is a unsigned short integer constant.
 * The first 32-N digits are set to 0 and the result returned.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   | 6-17  | 1/cycle  |
 * |power9   |   6   | 2/cycle  |
 *
 * @param vra 128-bit vector treated as a signed BCD 31 digit value.
 * @param _N a unsigned short integer constant truncate count.
 * @return a 128-bit vector BCD value with the first 32-count digits
 * set to 0.
 */
static inline vBCD_t
vec_bcdutruncqi (vBCD_t vra, const unsigned short _N)
{
  vBCD_t vrt;
#ifdef _ARCH_PWR9
  vui16_t shd = vec_splats ((const unsigned short) (_N));
  vrt = vec_bcdutrunc (vra, shd);
#else
  vui128_t t;
  const vui16_t ones = vec_splat_u16(-1);
  // Compare shift < 32 (128-bits)
  if (_N < 32)
    {
      // Generate a mask for the digits we will keep
      t = vec_srqi ((vui128_t) ones, ((32 -_N) * 4));
      // Clear the digits we are truncating
      vrt = (vBCD_t) vec_and ((vui32_t) t, (vui32_t)vra);
    }
  else
    vrt = vra;
#endif
  return (vrt);
}

/** \brief Combined Decimal Add & Write Carry Signed Quadword.
 *
 * Two Signed 31 digit BCD values are added, and the carry-out
 * (the high order 32nd digit) of the sum is generated.
 * Alternatively if the intermediate sum changes sign we need to,
 * borrow '1' from the magnitude of the higher BCD value and correct
 * (invert by subtracting from 10**31) the intermediate sum.
 * Both the sum and the carry/borrow are returned.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   | 15-24 | 1/cycle  |
 * |power9   | 6-15  | 2/cycle  |
 *
 * @param cout a pointer to a 128-bit vector to recieve the BCD carry-out.
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
  vBCD_t sum_ab, sign_ab;

  sum_ab = vec_bcdadd (a, b);
  if (__builtin_expect (__builtin_bcdadd_ov ((vi128_t) a, (vi128_t) b, 0), 0))
    {
      c = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, sum_ab);
    }
  else // (a + b) did not overflow, but did it borrow?
    {
      c = _BCD_CONST_ZERO;
      sign_ab = vec_bcdcpsgn (sum_ab, a);
      if (!vec_all_eq(sign_ab, sum_ab) && !vec_all_eq(_BCD_CONST_ZERO, sum_ab))
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
  if (!vec_all_eq(sign_ab, t) && !vec_all_eq(_BCD_CONST_ZERO, t))
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
 * generated.
 * Alternatively if the intermediate sum changes sign we need to,
 * borrow '1' from the magnitude of the next higher BCD value and
 * correct (invert by subtracting from 10**31) the intermediate sum.
 * Both the sum and the carry/borrow are returned.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   | 54-63 | 1/cycle  |
 * |power9   | 15-24 | 2/cycle  |
 *
 * @param cout a pointer to a 128-bit vector to recieve the BCD carry-out.
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
	  if (!vec_all_eq(sign_abc, sum_abc) && !vec_all_eq(_BCD_CONST_ZERO, sum_abc))
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
  if (!vec_all_eq(sign_abc, t) && !vec_all_eq(_BCD_CONST_ZERO, t))
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
 * To avoid overflow in the DFP Facility, we split each BCD operand
 * into 15 upper and 16 lower digit halves. This requires up four
 * decimal multiplies and produces four 30-32 digit partial products.
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
 * Then DFP Decimal shift will isolate the high (32nd) digit.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |107-413| 1/cycle  |
 * |power9   |115-294| 1/cycle  |
 *
 * @param a a 128-bit vector treated as a signed BCD 31 digit value.
 * @param b a 128-bit vector treated as a signed BCD 31 digit value.
 * @param p_high a pointer to a 128-bit vector to receive the
 * high 31-digits of the product (a * b).
 * @return a 128-bit vector which is the lower 31 digits of (a * b).
 */
static inline vBCD_t
vec_cbcdmul (vBCD_t *p_high, vBCD_t a, vBCD_t b)
{
  const vBCD_t dword_mask = (vBCD_t) CONST_VINT128_DW(15, -1);
#ifndef _ARCH_PWR9
  const vui32_t mz = CONST_VINT128_W (0, 0, 0, 0x0000000d);
#endif
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

/** \brief Combined Decimal Subtract & Write Carry Signed Quadword.
 *
 * Subtract (a -b) Signed 31 digit BCD values and detect the
 * carry/barrow (the high order 32nd digit).
 * If the intermediate sum changes sign we need to,
 * borrow '1' from the magnitude of the higher BCD value and correct
 * (invert by subtracting from 10**31) the intermediate sum.
 * Both the sum and the carry/borrow are returned.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   | 15-24 | 1/cycle  |
 * |power9   | 6-15  | 2/cycle  |
 *
 * @param cout a pointer to a 128-bit vector to recieve the BCD
 * carry-out (alues are -1, 0, and +1).
 * @param a a 128-bit vector treated as a signed BCD 31 digit value.
 * @param b a 128-bit vector treated as a signed BCD 31 digit value.
 * @return a 128-bit vector with the low order 31-digits of the
 * difference (a+b).
 */
static inline vBCD_t
vec_cbcdsubcsq (vBCD_t *cout, vBCD_t a, vBCD_t b)
{
  vBCD_t t, c;
#ifdef _ARCH_PWR8
  vBCD_t sum_ab, sign_ab;

  sum_ab = vec_bcdsub (a, b);
  if (__builtin_expect (__builtin_bcdsub_ov ((vi128_t) a, (vi128_t) b, 0), 0))
    {
      c = vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, sum_ab);
    }
  else // (a + b) did not overflow, but did it borrow?
    {
      c = _BCD_CONST_ZERO;
      sign_ab = vec_bcdcpsgn (sum_ab, a);
      if (!vec_all_eq(sign_ab, sum_ab) && !vec_all_eq(_BCD_CONST_ZERO, sum_ab))
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
  d_s = d_a - d_b;
  t = vec_DFP2BCD (d_s);
  // Shift right 31 digits, leaving the carry.
  d_t = __builtin_dscriq (d_s, 31);
  c = vec_DFP2BCD (d_t);
  // fix up spurious negative zeros
  if (vec_all_eq((vui32_t ) c, mz))
    c = _BCD_CONST_ZERO;
  // (a + b) did not overflow, but did it borrow?
  sign_ab = vec_bcdcpsgn (t, a);
  if (!vec_all_eq(sign_ab, t) && !vec_all_eq(_BCD_CONST_ZERO, t))
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

/** \brief Vector Decimal Convert Binary Coded Decimal (BCD) digit
 *  pairs from radix 100 binary integer bytes.
 *
 *  Convert 16 radix 100 digits to 32 BCD Format decimal digits.
 *  Input is radix 100 digits as binary bytes in the range 0-99.
 *  Each byte converted to the equivalent BCD digit pair in adjacent
 *  nibbles.
 *
 *  This can be used as the last stage operation in wider binary to
 *  decimal conversions.
 *
 *  \note the nibble high to low digit word is effectively big endian.
 *  This matches the digit order precedence of Decimal Add/Subtract.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 24-34 | 1/cycle  |
 *  |power9   | 27-37 | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned char of
 *  radix 100 digits.
 *  @return 128-bit vector unsigned char of BCD nibble pairs in the
 *  range 0-9.
 */
static inline vui8_t
vec_rdxcf100b (vui8_t vra)
{
  vui8_t result;
  vui8_t x6, high_digit;
  /* Compute the high digit correction factor. For binary 100s to BCD
   * this is the radix 100 value divided by 10 times by the radix
   * difference in binary.  For this stage we use 0x10 - 10 = 6.  */
  high_digit = vra / 10;
#if (__GNUC__ > 6)
  // Allow the compiler to do strength reduction for const 6 multiplier
  x6 = high_digit * 6;
  result = vra + x6;
#else
    {
      vui8_t c6;
      c6 = vec_splats ((unsigned char) 0x06);
      x6 = vec_mulubm (high_digit, c6);
      /* Add the high digit correction bytes to the original
       * radix 100 bytes in binary. */
      result = vec_add (vra, x6);
    }
#endif
  return result;
}

/** \brief Vector Decimal Convert radix 10,000 Binary halfwords to
 *  pairs of radix 100 binary bytes.
 *
 *  Convert 8 radix 10,000 digits to 16 adjacent radix 100 digits.
 *  Input is radix 10,000 digits as binary halfwords in the range
 *  0-9999.
 *  Each halfword converted to the equivalent radix 100 pair in
 *  adjacent bytes.
 *
 *  This can be used as a intermediate stage operation in wider binary
 *  to decimal conversions.
 *
 *  \note The high to low digit order is effectively big endian.
 *  This matches the digit order precedence of Decimal Add/Subtract.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 24-34 | 1/cycle  |
 *  |power9   | 27-37 | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned short of
 *  radix 10,000 digits.
 *  @return 128-bit vector unsigned char radix 100 pairs in the
 *  range 0-99.
 */
static inline vui8_t
vec_rdxcf10kh (vui16_t vra)
{
  vui8_t result;
  vui16_t x156, c156, high_digit;
  /* Compute the high digit correction factor. For binary 10**4 to 100s
   * this is the radix 10000 value divided by 100 times by the radix
   * difference in binary.  For this stage we use 0x100 - 100 = 156.  */
  high_digit = vra / 100;
  c156 = vec_splats ((unsigned short) 156);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  x156 = vec_vmuleub ((vui8_t) high_digit, (vui8_t) c156);
#else
  x156 = vec_vmuloub ((vui8_t) high_digit, (vui8_t) c156);
#endif
  /* Add the high digit correction bytes from the original
   * radix 10000 hword in binary. */
  result = (vui8_t) vec_add (vra, x156);
  return result;
}

/** \brief Vector Decimal Convert radix 10**8 Binary words to
 *  pairs of radix 10,000 binary halfwords.
 *
 *  Convert 4 radix 10**8 digits to 8 adjacent radix 10,000 digits.
 *  Input is radix 10**8 digits as binary words in the range
 *  0-99999999.
 *  Each word converted to the equivalent radix 10,000 pair in adjacent
 *  halfword.
 *
 *  This can be used as a intermediate stage operation in wider binary
 *  to decimal conversions.
 *
 *  \note The high to low digit order is effectively big endian.
 *  This matches the digit order precedence of Decimal Add/Subtract.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 18-25 | 1/cycle  |
 *  |power9   | 19-26 | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned int of
 *  radix 10**8 digits.
 *  @return 128-bit vector unsigned short radix 10,000 pairs in the
 *  range 0-9999.
 */
static inline vui16_t
vec_rdxcf100mw (vui32_t vra)
{
  vui16_t result;
  vui32_t high_digit;
  /* Compute the high digit correction factor. For binary 10**8 to 10**4
   * this is the radix 100000000 value divided by 10000 times by the radix
   * difference in binary.  For this stage we use 0x10000 - 10000 = 55536.  */
  const vui32_t c = vec_splats ((unsigned int) 55536);

  high_digit = vra / 10000;
#ifdef _ARCH_PWR8
  // 0 in the even hword of const c reduces vmsumuhm to vmulouh
  result = (vui16_t) vec_msum ((vui16_t) high_digit, (vui16_t) c, vra);
#else
    {
      vui32_t x;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
      x = vec_vmuleuh ((vui16_t) high_digit, (vui16_t) c);
#else
      x = vec_vmulouh ((vui16_t) high_digit, (vui16_t) c);
#endif
      /* Add the high digit correction word to the original
       * radix 10**8 word in binary. */
      result = (vui16_t) vec_add (vra, x);
    }
#endif
  return result;
}

/** \brief Vector Decimal Convert radix 10**16 Binary doublewords to
 *  pairs of radix 10**8 binary words.
 *
 *  Convert 2 radix 10**16 digits to 4 adjacent radix 10**8 digits.
 *  Input is radix 10**16 digits as binary doublewords in the range
 *  0-9999999999999999.
 *  Each doubleword converted to the equivalent radix 10**8 pair in
 *  adjacent words.
 *
 *  This can be used as a intermediate stage operation in wider binary
 *  to decimal conversions.
 *
 *  \note The high to low digit order is effectively big endian.
 *  This matches the digit order precedence of Decimal Add/Subtract.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 51-61 | 1/cycle  |
 *  |power9   | 30-40 | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned long of
 *  radix 10**16 digits.
 *  @return 128-bit vector unsigned short radix 10**8 pairs in the
 *  range 0-99999999.
 */
static inline vui32_t
vec_rdxcf10E16d (vui64_t vra)
{
  /* Magic numbers for multiplicative inverse to divide by 10**8
   are 12379400392853802749, no corrective add,
   and shift right 26 bits.  */
  const vui64_t mul_invs_ten8 = CONST_VINT128_DW(
      12379400392853802749UL, 12379400392853802749UL);
  const int shift_ten8 = 26;
  vui32_t result;
  vui64_t x, c, high_digit;
  /* Compute the high digit correction factor. For binary 10**16 to
   * 10**8 this is the radix 10000000000000000 value divided by
   * 100000000 times by the radix difference in binary.  For this
   * stage we use 0x100000000 - 100000000 = 4194967296.  */

  // high_digit = vra / 100000000;
  // Next divide the 16 digits by 10**8.
  // This separates the high 8 digits into words.
  high_digit = vec_mulhud (vra, mul_invs_ten8);
  high_digit = vec_srdi (high_digit, shift_ten8);
  c = vec_splats ((unsigned long)4194967296);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  x = vec_muleuw ((vui32_t) high_digit, (vui32_t) c);
#else
  x = vec_mulouw ((vui32_t) high_digit, (vui32_t) c);
#endif
  /* Add the high digit correction dword to the original
   * radix 10**16 dword in binary. */
  result = (vui32_t) vec_addudm (vra, x);
  return result;
}

/* Convert radix 10**16 binary dwords to radix 10**8 words */

/** \brief Vector Decimal Convert radix 10**32 Binary quadword to
 *  pairs of radix 10**16 binary doublewords.
 *
 *  Convert a  binary quadword to 2 adjacent radix 10**16 digits.
 *  Input is a binary quadwords in the range
 *  0-99999999999999999999999999999999.
 *  The quadword converted to the equivalent radix 10**18 pair in
 *  adjacent doublewords.
 *
 *  This can be used as a first stage operation in binary
 *  to decimal conversions.
 *
 *  \note Results are undefined if the input value is greater than
 *  10**32 - 1. See \ref int128_examples_0_1_2 for details.
 *  \note The high to low digit order is effectively big endian.
 *  This matches the digit order precedence of Decimal Add/Subtract.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 85-95 | 1/cycle  |
 *  |power9   | 56-66 | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned __int128
 *  in the range 0-99999999999999999999999999999999.
 *  @return 128-bit vector unsigned long radix 10**16 pairs in the
 *  range 0-9999999999999999.
 */
static inline vui64_t
vec_rdxcf10e32q (vui128_t vra)
{
  // Compute the high digit correction factor. For binary 10**32 to
  // 10**16, this is (16**16 - 10**16) = 18436744073709551616.
  const vui64_t c = CONST_VINT128_DW (0, 18436744073709551616UL);

  /* Magic numbers for multiplicative inverse to divide by 10**16
   are 76624777043294442917917351357515459181, no corrective add,
   and shift right 51 bits.  */
  const vui128_t mul_invs_ten16 = (vui128_t) CONST_VINT128_DW(
      0x39a5652fb1137856UL, 0xd30baf9a1e626a6dUL);
  const int shift_ten16 = 51;

  vui64_t result;
  vui128_t high_digit;

  // high_digit = vra / 10000000000000000;
  // Next divide the 32 digits by 10**16.
  // This separates the high 16 digits into doublewords.
  high_digit = vec_mulhuq (vra, mul_invs_ten16);
  high_digit = vec_srqi (high_digit, shift_ten16);

  // multiply high_digit by the radix difference c and add vra
#ifdef _ARCH_PWR9
  // 0 in the high dword of const c reduces vmsumudm to vmuloud
  result = (vui64_t) vec_msumudm ((vui64_t) high_digit, c, vra);
#else
    {
      vui128_t x;
      x = vec_vmuloud ((vui64_t) high_digit, c);
      /* Add the high digit correction qword to the original
       * radix 10**32 qword in binary. */
      result = (vui64_t) vec_adduqm (vra, x);
    }
#endif
  return result;
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
  c6 = vec_splats ((unsigned char) 0x06);
#if (__GNUC__ > 7)
  // Allow the compiler to do strength reduction for const 6 multiplier
  x6 = vec_mul (high_digit, c6);
#else
  x6 = vec_mulubm (high_digit, c6);
#endif
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
 *  |power9   | 10-19 | 2/cycle  |
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
  const vui64_t c18436744073709551616 = CONST_VINT128_DW (18436744073709551616UL, 0);
  vui128_t x18436744073709551616;
  /* Compute the high digit correction factor for 10ts to binary 10e32s
   * This is the isolated high digit multiplied by the radix difference
   * in binary.  For this stage we use
   * 0x10000000000000000 - 10000000000000000 = 18436744073709551616.  */
//  c18436744073709551616 = vec_splats ((unsigned long) 18436744073709551616UL);
#ifdef _ARCH_PWR9
  const vui128_t zero = CONST_VINT128_DW128 (0, 0);
  // 0 in the low dword of const c reduces vmsumudm to vmuleud
  x18436744073709551616 = (vui128_t) vec_msumudm ((vui64_t) vra, c18436744073709551616, zero);
#else
  x18436744073709551616 = vec_vmuleud ((vui64_t) vra, c18436744073709551616);
#endif

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
 *  non-zero value. Otherwise if the sign nibble is
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
