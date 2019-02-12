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
 * provide the build-ins.
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
 * BCD and Zoned formats and operations where implemented for some of
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
 * new business oriented applications requiring decimal arithmetic.
 * As DFP is supported directly in the hardware and has has extensive
 * language and runtime support, there is little that PVECLIB can
 * contribute to general decimal radix computation.
 *
 * \note BCD and DFP support requires at least PowerISA 2.05 (POWER6)
 * or later server level processor support.
 *
 * However the vector unit and recent BCD and Zoned extensions can
 * still be useful. Areas include large order multiple precision
 * computation and conversions between binary and Decimal radix.
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
 * - BCD add/subtract with carry/extend.
 * - BCD compare equal, greater than, less than
 * - BCD copy sign and set bool from sign
 * - BCD digit shift left/right
 * - BCD multiply/divide
 *
 * The original VMX (AKA Altivec) only defined a few instructions that
 * operated on the 128-bit vector as a whole.
 * This included the vector shifts by bit and octet, and generalized
 * vector permute. General binary integer add, subtract and multiply
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
 * The 128-bit DPF (quadword) instructions operate on even/odd
 * 64-bit Floating-point register pairs (FPRp).
 *
 * POWER6 also implemented the Vector Facility (VMX) instructions.
 * No additional vectors operations where added and the Vector
 * Registers (VRs) where separate from the GRPs and FPRs. The only
 * transfer data path between register sets is via storage.
 * So while the DFU could be used for BCD operations and conversions
 * there was little synergy with the vector unit.
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
 * GT/LT/EQ/OVF. This allows for implementations of BCD compare and
 * and the overflow (OVF) bit supports carry/extend operations.
 * Also the lack of BCD multiply/divide in the vector unit is not a
 * problem we can leverage DFP (see vec_bcdmul(), vec_bcddiv()).
 *
 * POWER9 (PowerISA 3.0B) adds BCD copy sign, set sign, shift, round,
 * and truncate instructions. There are also unsigned (32-digit) forms
 * of the shift and truncate instructions. And convert instructions
 * between signed BCD and quadword (__int128) and signed BCD and Zoned.
 * POWER9 also added quadword binary multiply 10 with carry extend
 * forms. Theses can also help with decimal to binary conversion.
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
 * - Are commonly used operations, not covered by the ABI or
 * <altivec.h>, and require multiple instructions or
 * are not obvious.  Examples include vec_pack_Decimal128() and
 * vec_unpack_Decimal128().
 *
 * See \ref mainpage_sub_1_3 for more background on extended quadword
 * computation.
 *
 * \section bcd128_endian_issues_0_0 Endian problems with quadword implementations
 *
 * Technically operations on quadword elements should not require any
 * endian specific transformation. There is only one element so there
 * can be no confusion about element numbering or order. However
 * some of the more complex quadword operations are constructed from
 * operations on smaller elements. And those operations as provided by
 * <altivec.h> are required by the OpenPOWER ABI to be endian sensitive.
 * See \ref i64_endian_issues_0_0 for a more detailed discussion.
 *
 * In any case the arithmetic (high to low) order of digit nibbles in
 * BCD or characters in Zoned are defined in the PowerISA.
 * In the vector register high order digits are on the left
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

///@cond INTERNAL
static inline vf64_t vec_pack_Decimal128 (_Decimal128 lval);
static inline _Decimal128 vec_qauntize0_Decimal128 (_Decimal128 val);
static inline vui8_t vec_rdxcfzt100b (vui8_t zone00, vui8_t zone16);
static inline vui8_t vec_rdxct100b (vui8_t vra);
static inline vui16_t vec_rdxct10kh (vui8_t vra);
static inline vui32_t vec_rdxct100mw (vui16_t vra);
static inline vui64_t vec_rdxct10E16d (vui32_t vra);
static inline vui128_t vec_rdxct10e32q (vui64_t vra);
static inline _Decimal128 vec_unpack_Decimal128 (vf64_t lval);
///@endcond

/** \brief Convert a Vector Signed BCD value to __Decimal128
 *
 * The BCD vector is permuted into a double float pair before
 * conversion to DPD format via the DFP Encode BCD To DPD Quad
 * instruction.
 *
 * @param val a 128-bit vector treated a signed BCD 31 digit value.
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

/* \todo To be useful for number conversion will need to implement
 * carry and extend forms of bcdadd/sub. This will require conditional
 * logic based on CR6.
 *
 */

/** \brief Add two Vector Signed BCD 31 digit values.
 *
 * Two Signed 31 digit values are added and lower 31 digits of the
 * sum are returned.  Overflow (carry-out) is ignored.
 *
 * @param a a 128-bit vector treated a signed BCD 31 digit value.
 * @param b a 128-bit vector treated a signed BCD 31 digit value.
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
  _Decimal128 d_t;
  d_t = vec_BCD2DFP (a) + vec_BCD2DFP (b);
  t = vec_DFP2BCD(d_t);
#endif
  return (t);
}

/** \brief Vector Decimal Convert Binary Coded Decimal (BCD) digit
 *  pairs to binary unsigned bytes .
 *
 *  Vector convert 16 bytes each containing 2 BCD digits to the
 *  equivalent unsigned char, in the range 0-99.
 *  Input values should be valid 2 x BCD nibbles in the range 0-9.
 *
 *  @param vra a 128-bit vector treated as a 16 unsigned 2-digit BCD
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
 *  @param vra a 128-bit vector treated as a 8 unsigned 4-digit BCD
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
 *  @param vra a 128-bit vector treated as a 4 unsigned 8-digit BCD
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
 *  @param vra a 128-bit vector treated as a 2 unsigned 16-digit BCD
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
 *  @param vra a 128-bit vector treated as unsigned 32-digit BCD
 *  numbers.
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
 * @param a a 128-bit vector treated a signed BCD 31 digit value.
 * @param b a 128-bit vector treated a signed BCD 31 digit value.
 * @return a 128-bit vector which is the lower 31 digits of (a / b).
 */
static inline vBCD_t
vec_bcddiv (vBCD_t a, vBCD_t b)
{
  vBCD_t t;
  _Decimal128 d_t, d_a, d_b;
  d_a = vec_BCD2DFP (a);
  d_b = vec_BCD2DFP (b);
  d_t = vec_qauntize0_Decimal128 (d_a / d_b);
  t = vec_DFP2BCD (d_t);
  return (t);
}

/** \brief Multiply two Vector Signed BCD 31 digit values.
 *
 * Two Signed 31 digit values are multiplied and the lower 31 digits
 * of the product are returned.  Overflow is ignored.
 *
 * \todo This is tricky as the product can be up to 62 digits and
 * _Decimal128 can only hold 34 digits. So products that are more
 * than 34 digits will loose precision on the right during the
 * conversion back to BCD.
 * Need a different implementation that will implement the truncation
 * semantics of multiply low. Also need to implement the corresponding
 * multiply high operation.
 *
 * @param a a 128-bit vector treated a signed BCD 31 digit value.
 * @param b a 128-bit vector treated a signed BCD 31 digit value.
 * @return a 128-bit vector which is the lower 31 digits of (a * b).
 */
static inline vBCD_t
vec_bcdmul (vBCD_t a, vBCD_t b)
{
  vBCD_t t;
  _Decimal128 d_t, d_a, d_b;
  d_a = vec_BCD2DFP (a);
  d_b = vec_BCD2DFP (b);
  d_t = vec_qauntize0_Decimal128 (d_a * d_b);
  t = vec_DFP2BCD (d_t);
  return (t);
}

/** \brief Subtract two Vector Signed BCD 31 digit values.
 *
 * Subtract Signed 31 digit values and return the lower 31 digits of
 * of the result.  Overflow (carry-out/barrow) is ignored.
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
  _Decimal128 d_t, d_a, d_b;
  d_a = vec_BCD2DFP (a);
  d_b = vec_BCD2DFP (b);
  d_t = d_a - d_b;
  t = vec_DFP2BCD(d_t);
#endif
  return (t);
}

/** \brief Pack a FPR pair (_Decimal128) to a doubleword vector
 *  (vector double).
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
 * The truncate (round toward 0) and justify right the input
 * _Decimal128 value so that the unit digit is in the right most
 * position.  This supports BCD multiply and divide using DFP
 * instructions by truncating fractional digits before conversion
 * back to BCD.
 *
 * @param val a _Decimal128 value.
 * @return The quantized __Decimal128 value in a double float pair.
 */
static inline _Decimal128
vec_qauntize0_Decimal128 (_Decimal128 val)
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
 *  Century format is adjacent digit pairs converter to a binary
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
 *  Century format is adjacent digit pairs converter to a binary
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
 *  @param vra a 128-bit vector treated as a vector unsigned char
 *  of BCD pairs.
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
 *  @param vra a 128-bit vector treated as a vector unsigned char
 *  of century digit pairs.
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
 *  @param vra a 128-bit vector treated as a vector unsigned short
 *  of 10k digit pairs.
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
 *  @param vra a 128-bit vector treated as a vector unsigned int
 *  of 100m digit pairs.
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
 *  @param vra a 128-bit vector treated as a vector unsigned long
 *  of 10e16-1 digit pairs.
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

/** \brief Unpack a doubleword vector (vector double) into a FPR pair.
 * (_Decimal128).
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
