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

 vec_common_ppc.h

 Contributors:
      IBM Corporation, Steven Munroe
 */

#ifndef VEC_COMMON_PPC_H_
#define VEC_COMMON_PPC_H_

#include <stdint.h>
#include <altivec.h>

/*!
 * \file  vec_common_ppc.h
 * \brief Common definitions and typedef used by the collection of
 * Power Vector Library (pveclib) headers.
 *
 * This includes:
 * - Typedefs as short names of common vector types.
 * - Union used to transfer 128-bit data between vector and
 * non-vector types.
 * - Helper macros that make declaring constants and accessing
 * elements a little easier.
 * - Common operations that are used to support operations across
 * multiple headers.
 *
 * \section common_type_naming_0_0 Consistent vector type naming
 *
 * Type names should be short, concise, and consistent. The ABI
 * defines the vector types as extensions of the existing C
 * Language types. So while <I>vector unsigned long long</I> is
 * consistent it is neither short or concise.
 * Pveclib uses the following naming convention for typedefs used in
 * its operations, function prototypes, and internal variables.
 * - Starting with the <B>v</B> prefix for vector.
 * - followed by one of the element classes:
 *   -  <B>i</B> for signed integer.
 *   -  <B>ui</B> for unsigned integer.
 *   -  <B>f</B> for floating-point.
 *   -  <B>b</B> for bool.
 * - followed by the element size in bits:
 *   - 8, 16, 32, 64, 128
 * - Ending with the <B>_t</B> suffix signifying a typedef.
 *
 * For example: \ref vi32_t is a vector int,
 * \ref vui32_t is a vector unsigned int,
 * \ref vb32_t is a vector bool int,
 * and \ref vf32_t is vector float.
 *
 * \section common_type_xfer_0_0 Transferring 128-bit types
 *
 * The OpenPOWER ABI and the GCC compiler define a number of 128-bit
 * scalar types that are not vector types:
 * - __int128 (a general purpose register pair)
 * - _Decimal128 (a floating-point even/odd register pair)
 * - __ibm128 (a floating-point register pair)
 * - __float128 (a vector register)
 *
 * These are not cast nor assignment compatible with any vector type.
 * However it may be useful to transfer to/from vector types for
 * conversion or manipulation within an operation.
 * For example:
 * - Conversions between __float128 and __int128, __ibm128,
 * and _Decimal128 types.
 * - Conversions between vector BCD integers and __int128
 * and _Decimal128 types.
 * - Conversions between vector __int128 and __float128, __ibm128,
 * and _Decimal128 types.
 *
 * Here we use the __VEC_U_128 union to affect the transfer between
 * the various types. We assume (fervently hope) that the compiler will
 * recognize and optimize these as registers to registers transfers
 * using the hardware instructions provided.
 *
 * The vector to/from __float128 transfer should be the simplest
 * as __float128 operations are defined over the vector register set.
 * However __float128 types are defined in the PowerISA and OpenPOWER
 * ABI, as scalars that just happens to use vector
 * registers for parameter passing and operations.
 * This distinction between scalars and vector prevents a direct cast
 * between types. The __VEC_U_128 union is the simplest work around
 * but in most cases no code should generated for this transfer.
 * For example: vec_xfer_bin128_2_vui128t() and vec_xfer_vui128t_2_bin128().
 *
 * Any vector to/from __int128 transfer requires a transfer between
 * vector and general purpose registers. POWER8 (PowerISA 2.07B) added
 * Move to/from Vector Scalar Register (mfvsr, mtvsr) instructions.
 * Again the __VEC_U_128 union is used to effect the transfer and the
 * compiler should leverage the move instructions in the generated code.
 *
 * Any vector to/from __ibm128 or _Decimal128 requires a transfer
 * between a pair of FPRs and a Vector Scalar Register (VSR).
 * Technically this is transfer between the upper doubleword of two
 * VSRs in the lower bank (VSR0-31) and another VSR.
 * POWER7 (PowerISA 2.06B) provides the
 * Permute Doubleword Immediate (xxpermdi) instruction.
 * Again the __VEC_U_128 union is used to effect the transfer and the
 * compiler should leverage the Permute Doubleword Immediate
 * instructions in the generate code.
 * For example: vec_BCD2DFP() and vec_DFP2BCD().
 *
 * \section common_endian_issues_0_0 Endian and vector constants
 *
 * Vector constants are often needed for: masking operations,
 * range checks, permute selection, and radix conversion.
 * Also compiler support for large integer and floating-point
 * constants may be limited by the compiler.
 * For example the GCC compilers support the (vector) __int128 type
 * but do not directly support __int128 (39 digit) decimal constants.
 * Another example is __float128 where the type and Q suffix constants
 * are recent additions.
 * In both cases we need to construct: large numeric constants,
 * special values (infinity and NaN), masks for manipulating the sign
 * bit and exponent bits.
 * Often these values will be constructed from vectors of word or
 * doubleword constants.
 *
 * \note GCC does not support expressing an integer constant of type
 * __int128 for targets where long long integer is less than 128
 * bits wide.  This applies to the PowerPC target as the long long
 * type is reserved for 64-bit integers. This was verified in GCC 8.2,
 *
 * \note GCC __float128 support for the PowerPC target began with
 * GCC 6. In GCC 6 __float128 support is off by default and has to be
 * explicitly enabled via the '-mfloat128' option. Starting with GCC 7,
 * __float128 is enabled by default with VSX support.
 *
 * Defining large constants for vectors is
 * complicated by <I>little-endian</I> (LE) support as
 * specified in the OpenPOWER ABI and as implemented in the compilers.
 * Little-endian changes the effective vector element numbering and
 * the order of constant elements in initializers.  But the __int128
 * numerical order of magnitude or floating-point format does not
 * change in registers. The high order bits are on the left and the
 * low order bits are on the right.
 *
 * So for example:
 * \code
  const vui32_t signmask = { 0x80000000, 0, 0, 0 };
  const vui32_t expmask = { 0x7fff0000, 0, 0, 0 };
 * \endcode
 * are correct sign and exponent masks for __float128 in big
 * endian (BE) but would be incorrect for little endian (LE).
 * To get correct results for both endians, one could code something
 * like this:
 * \code
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  const vui32_t signmask = { 0, 0, 0, 0x80000000 };
  const vui32_t expmask = { 0, 0, 0, 0x7fff0000 };
#else
  const vui32_t signmask = { 0x80000000, 0, 0, 0 };
  const vui32_t expmask = { 0x7fff0000, 0, 0, 0 };
#endif
 * \endcode
 * But this gets tedious after the first dozen times.
 * Also this can be confusing because it does not appear to the match
 * the floating-point format diagrams in the PowerISA. The sign-bit
 * and the exponent are always on the left.
 *
 * So this header provides endian sensitive macros that maintain
 * consistent "magnitude" order. For example:
 * \code
  const vui32_t signmask = CONST_VINT128_W (0x80000000, 0, 0, 0);
  const vui32_t expmask = CONST_VINT128_W (0x7fff0000, 0, 0, 0);
 * \endcode
 * This is always correct in either endian.
 *
 * Another example; the multiplicative inverse for __int128 10**32
 * is 211857340822306639531405861550393824741.
 * The GCC compiler will not accept this constant in a vector __int128
 * initializer. The next best thing would be
 * \code
  // The multiplicative inverse for 1 / 10**32 is
  // 211857340822306639531405861550393824741
  // or 0x9f623d5a8a732974cfbc31db4b0295e5
  const vui128_t mulinv_10to32 =
           (vui128_t) CONST_VINT128_DW128 ( 0x9f623d5a8a732974UL,
                                            0xcfbc31db4b0295e5UL );
 * \endcode
 * Here we use the CONST_VINT128_DW128 macro to maintain magnitude
 * order across endian. Again the high order bits are on the left
 * and the low order bits are on the right.
 *
 * \sa \ref i32_endian_issues_0_0
 * \sa \ref mainpage_endian_issues_1_1
 *
 * \section common_operation_issues_0_0 Issues with common operations.
 *
 * Some operations are common components of more complex operations.
 * It is common for wider element operations (doubleword/quadword) to
 * use narrower (word/halfword/byte) element operations within their
 * implementations. Especially when the target processor does
 * not support the operation directly.
 *
 * The PVECLIB implementation <I>stacks</I> the headers so that each
 * type and element width always starts by including the header for
 * the next smaller element width of that type.
 * So vec_int128_ppc.h includes vec_int64_ppc.h, which includes
 * vec_int32_ppc.h and so on down to vec_char_ppc.h.
 * The vec_char_ppc.h header includes this (vec_common_ppc.h) header
 * to pick up the type definitions required for all supported element
 * widths.
 *
 * Sometimes an operation implies an operation over a wider element
 * then parameters imply. For example the POWER10 Vector String
 * Isolate Left/Right-justified operations imply a quadword count
 * leading/trailing zeros (clz/ctz) operation to generate the required
 * vector char mask. Quadword count leading/trailing zeros are not
 * (yet) it the current PowerISA. The PVECLIB quadword operation
 * depends on the corresponding word/doubleword vec_clz()/Vec_ctz()
 * operations.
 *
 * While word/doubleword vec_clz() was introduced for POWER8,
 * vec_ctz() was not available until POWER9. So the _ARCH_PWR8
 * implementations of vec_ctzd() uses a trailing zeros conversion
 * (!vra & (vra - 1)) and vector population count (vec_popcntd()).
 * The vec_popcnt() intrinsic was also introduced for POWER8 along
 * with doubleword vec_add()/vec_sub() support.
 *
 * None of these vector intrinsics are available for POWER7. None of
 * corresponding word/doubleword PVECLIB operations are in scope
 * for vec_char_ppc.h implementations.
 * This quickly cascades into a large set of circular dependencies.
 *
 * This is becoming more common for newer (POWER9/10) operations
 * where the compile target is POWER7/8. In this case we find that the
 * desired operations is missing or out of scope for the required
 * element width.
 *
 * Implementations for specific targets
 * can be provided in this header and used as needed
 * at any other PVECLIB header.
 * These are intended for internal usage by PVECLIB operations
 * and must avoid conflicts with the intrinsics or PVECLIB public
 * name-space.
 *
 * \note Proposal: Common operations will use the form:
 * vec_<opcode>_<arch_suffix>.
 * The \<opcode\> will match the ISA mnemonic including the element width
 * suffix (ie. clzw, popcntb, ...).
 * The \<arch_suffix\> will match the suffix of compiler generated
 * \_ARCH\_ macros. For example; vec_clzd_PWR8.
 *
 * Implementations at this level can only use intrinsic operations
 * provided by altivec.h and/or in-line assembler instructions.
 * Any common implementation can use other implementations from
 * vec_common_ppc.h.
 * Any assembler codes or altivec.h intrinsic/element widths
 * introduced for POWER7/VSX (PowerISA 2.06 or later) must be guarded
 * by appropriate target conditionals.
 *
 * Implementation for different ISA levels should be isolated in
 * separate functions. The top level implementation will start with
 * the intrinsic or opcode that was introduced into the ISA and with
 * the appropriate \_ARCH suffix.
 * Additional operation implementations for previous ISA levels will
 * be separate functions for each \_ARCH level. Each level should
 * tail-call to next lower implementation for that operation.
 * For example:
 * \code
static inline vui32_t
vec_popcntw_PWR8 (vui32_t vra)
{
  vui32_t r;
#ifdef _ARCH_PWR8
#if defined (vec_vpopcntw)
  r = vec_vpopcntw (vra);
#elif defined (__clang__)
  r = vec_popcnt (vra);
#else
  __asm__(
      "vpopcntw %0,%1;"
      : "=v" (r)
      : "v" (vra)
      : );
#endif
#else
  r = vec_popcntw_PWR7 (vra);
#endif
  return (r);
}
 * \endcode
 * If the target is not at least -mcpu=power8 the implementation
 * tail-calls to vec_popcntw_PWR7().
 * For example:
 * \code
static inline vui32_t
vec_popcntw_PWR7 (vui32_t vra)
{
  const vui32_t vzero = vec_splat_u32 (0);
  vui8_t bytepop;
  // Generate byte pop counts
  bytepop = vec_popcntb_PWR7 ((vui8_t) vra);
  // sum across adjacent bytes into words for pop count
  return  vec_sum4s (bytepop, vzero);
}
 * \endcode
 * Which depends on the common implementation vec_popcntb_PWR7() and
 * intrinsic vec_sum4s(). The Vector Sum across Quarter Unsigned Byte
 * Saturate (vec_sum4s()) yields the word population counts.
 * \note vec_sum4s() is a saturating sum. Saturation is not needed for
 * this use case. But the range of values (0-32) is small and will
 * never saturate the 32-bit sum.
 *
 * And finally a vec_popcntb_PWR7() implementation is needed.
 * For example:
 * \code
static inline vui8_t
vec_popcntb_PWR7 (vui8_t vra)
{
  const vui8_t nibmask = vec_splat_u8 (15);
  const vui8_t nibshft = vec_splat_u8 (4);
  const vui8_t popc_perm = { 0, 1, 1, 2, 1, 2, 2, 3,
                             1, 2, 2, 3, 2, 3, 3, 4};
  vui8_t nib_h, nib_l; // High and low nibbles by byte.
  vui8_t rh, rl; // High and low nibble popcnt.

  nib_l = vec_and (vra, nibmask);
  nib_h = vec_sr  (vra, nibshft);
  rl = vec_perm (popc_perm, popc_perm, nib_l);
  rh = vec_perm (popc_perm, popc_perm, nib_h);
  return vec_add (rh, rl);
}
 * \endcode
 * Which depends only on original (PowerISA 2.03) <altivec.h>
 * intrinsics. Here we use vector permute as a table look-up to
 * convert nibbles into into a population count (0-4). The sum
 * (vec_add()) of the nibble population counts yields the byte wide
 * population counts.
 *
 *
 * (See vec_popcntb_PWR7(), vec_popcnth_PWR7(), vec_popcntw_PWR7(),
 * vec_popcntd_PWR7(), vec_popcntq_PWR7())
 *
 * \subsection common_const_issues_000 Generating small integer consts for vectors
 *
 * A number of common vector operations require small integer constants
 * as input. This includes simple arithmetic (+-*\/) and the
 * shift/rotate operations. Frequently these constants are common
 * (splatted) across the elements of one of the operands. As PowerISA
 * is a register-register architecture, these constants must be loaded
 * into vector register to be applied (via some operation) to a vector of
 * user data.
 *
 * Some examples using GCC extended inline vector syntax:
 * \code
// Convert 2 Binary Coded Decimal (BCD) digit (nibbles) to
// radix 100 binary integer bytes.
vui8_t
vec_rdxct100b (vui8_t bcdx2)
{
  vui8_t x6, high_digit;
  // Compute the high digit correction factor. For BCD to binary 100s
  // this is the isolated high digit multiplied by the radix difference
  // in binary.  For this stage we use 0x10 - 10 = 6.
  high_digit = bcdx2 >> 4;
  x6 = high_digit * 6;
  // Subtract the high digit correction bytes from the original
  // BCD bytes in binary.  This reduces byte range to 0-99.
  return (bcdx2 - x6);
}
 * \endcode
 *
 * \code
// Fixed point rounding for scaled integers. The constant parameter N
// defines the number of fractional binary digits
static inline vi32_t
vec_round_shift (vi32_t value, const unsigned int N)
{
  const vi32_t v1s = vec_splat_u32(1);
  return (((value) + (v1s << (N-1))) >> N);
}
 * \endcode
 *
 * \note vec_rdxct100b() is the first stage (Nibble to Byte) of the
 * Vector Parallel BCD to quadword conversion
 * (\ref bcd128_convert_0_2_2_2).
 * Additional stages (Byte to Halfword to Word to Doubleword to
 * Quadword) follow this pattern with appropriate changes to the
 * shift count and radix difference constants.
 *
 * Each example uses a number of (splatted) scalar constants that are
 * applied to each element of the vector. Perhaps it is clearer using
 * the equivalent vector instrinsic form. for example:
 * \code
vui8_t
vec_rdxct100b_V2 (vui8_t bcdx2)
{
  const vui8_t c4 = vec_splat_u8(4);
  const vui8_t c6 = vec_splat_u8(6);
  vui8_t x6, high_digit;

  high_digit = vec_vsrb (bcdx2, c4);
  x6 = vec_mul (high_digit, c6);
  return vec_vsububm (bcdx2, x6);
}
 * \endcode
 *
 * \code
vi32_t
vec_round_shift_14_v2 (vi32_t value)
{
  const vui32_t vN  = vec_splat_u32(14);
  const vui32_t vNm = vec_splat_u32(14-1);
  const vi32_t v1s = vec_splat_s32(1);
  vi32_t rndf, rnded;

  rndf = vec_sl (v1s, vNm);
  rnded = vec_add (value, rndf);
  return vec_sra (rnded, vN);
}
 * \endcode
 *
 * In the vector syntax, arithmetic and shift/rotate constants will be
 * treated the same by the language standards. The language will
 * normally require that the constant be promoted to the same type and
 * (element) size as the variable in the operation. For example:
 *
 * \code
  high_digit = bcdx2 >> 4;
  x6 = high_digit * 6;
 * \endcode
 * Requires that the constants are expanded to integer bytes then
 * splatted to the vector. This is based on the type of variable
 * bcdx2 and high_digit (vector char).
 *
 * While
 * \code
  const vui32_t v1s = vec_splat_s32(1);
  return (((value) + (v1s << (N-1))) >> N);
 * \endcode
 * Requires that the constants are expanded to integer words then
 * splatted to the vector. This is based on the type of variable value
 * (vector int).
 *
 * The compiler may also require a specific signness (signed or
 * unsigned) depending on the operator and which side of the operator.
 * For example shift counts are unsigned while values may be signed or
 * unsigned. This matters for shift right as signed values propagate
 * the sign-bit (vec_sra() vs vec_sr()).
 *
 * These language compliance issues have implications for efficiency
 * of the generated code. The GCC vector inline extension will expand
 * the shift count constants to full word/doubleword element. The
 * vector intrinsic tends to follow this and requires the shift count
 * operand has the same element size/type as the shifted value.
 * The PowerISA does not require this for shift/Rotate instructions.
 *
 * \note The PowerISA shift/Rotate instructions only require;
 * 3 to 7-bit shift counts (based on element size),
 * right justified in each element.
 * Element bits to the left of the shift count are ignored.
 *
 * The result is more complicate code (more instructions and greater
 * latency) then is strictly required for the function.
 * The complexity depends on:
 * - the availability and size/range of instruction immediate operands
 * - and in some cases sign extend (unpack) instructions
 * - For each version of the PowerISA
 *
 * If the compiler can not match the constant value:
 * - to a convenient splat immediate field
 * - that matches the required element size.
 * - or can be conveniently extended to the required element size.
 * - The compiler may give up and generate a vector constant that it
 *   can load from read-only storage (.rodata).
 *
 * For POWER8/9 these loads requires 3-5 instructions and 9-13 cycles
 * latency each. While a single splat immediate instruction has a
 * 2-3 cycle latency. So a short sequence:
 * - of (2/3) splat-immediate, arithmetic, shift, or extend instructions
 * - that generate the required constant
 * - is subject to common sub-expression optimizations
 * - and avoids the vector load
 * - is usually a performance win.
 *
 * \note The compiler <I>could</I> generate better code for loading
 * vector constants. See GCC PR 117718.
 *
 * This depends on the compile target (PowerISA version) and the
 * instructions available.
 * - PowerISA 2.03-2.06 (970/POWER6/7)
 *   - Vector Splat Immediate Signed Byte/Halfword/Word
 *     (5-bit Signed Immediate (SIM) field)
 *   - Vector Unpack High/Low Signed Halfword
 *     (can be use for word if constant is small (i.e. range of signed short))
 *   - Vector Merge High/Low Word
 *     (0 extent unsigned splatted word to DW, requires zero VR)
 * - PowerISA 2.07 (POWER8)
 *   - Vector Unpack High/Low Signed Word
 *     (sign extend splatted word to DW)
 *   - Vector Merge Even/Odd Word
 *     (0 extent unsigned splatted word to DW, requires zero VR)
 * - PowerISA 3.0 (POWER9)
 *   - VSX Vector Splat Immediate Byte (8-bit Immediate (IMM8) field)
 *   - Vector Extend Sign Byte To Word/Doubleword
 *   - Vector Extend Sign Halfword To Word/Doubleword
 *   - Vector Extend Sign Word To Doubleword
 * - PowerISA 3.1 (POWER10)
 *   - VSX Vector Splat Immediate Word (32-bit Immediate field)
 *   - VSX Vector Splat Immediate32 Doubleword Indexed
 *     (32-bit Immediate field, splatted to each DW.word[IX])
 *   - VSX Vector Splat Immediate Double-Precision
 *     (32-bit float Immediate field, expanded to double)
 *   - Vector Extend Sign Doubleword To Quadword
 *
 * \note Shift counts for byte and halfword shift/rotates are easy.
 * The Vector Splat Immediate Signed Byte/Halfword instructions can
 * match the element size and the 5-bit SIM can generate positive
 * integers for 3 and 4-bit shift counts. Net Byte/Halfword shift
 * counts can always be generated via a single instruction.
 *
 * \note The difference between Unpack High/Low Signed <I>element size</I>
 * and Extend Sign <I>element size X</I> To <I>element size Y</I> can be
 * confusing. Unpack High Signed Halfword could be called Extend Sign
 * High Halfwords To Words. And Extend Sign Halfword To Word could be
 * called Unpack Odd Signed Halfword. Unpack takes the high/low half
 * of a vector and sign extends the elements to the next larger element
 * in a full vector. Either can be used to sign extend splatted values.
 *
 * Now lets look at more complicated and less "tidy" example:
 * \code
vui32_t
test_slw_16_v0 (vui32_t vra)
{
  return vra << 16;
}
 * \endcode
 *
 * \note For POWER8 this should be the simplest example as the word
 * shift/rotates requires a 5-bit shift count and the Vector Splat
 * Immediate instructions includes a 5-bit Signed Immediate (SIM)
 * field. Which is appropriately justified into each word element.
 *
 * Which generates:
 * \code
     vspltisw v0,8
     vadduwm v0,v0,v0
     vslw    v2,v2,v0
 * \endcode
 *
 * The compiler <I>knows</I> that vspltisw can't generate a positive
 * 16 value. But as 16 is an even value it applies the following
 * transform:
 *
 * \code
  if (__builtin_constant_p (sim5) && (sim5 > 15) && (sim5 % 2 == 0))
	{
	  // For even values greater than 15.
	  // Divide by 2 (sim5 >> 1), SIM range is now 8 - 15
	  // generate the (signed) 5-bit splat immediate word,
	  // then double (*2) the word vector,
	  // This covers the even values 16 - 30.
	  tmp = vec_splat_u32(sim5 >> 1);
	  result = vec_add (tmp, tmp);
	}
 * \endcode
 *
 * \note This is a useful technique to fill in the middle range of 6
 * and 7-bit shift counts for POWER8.
 *
 * This is correct for arithmetic but unnecessary for word
 * element shift/rotate. For this case it is just additional
 * instructions and latency. This vector intrinsic implementation
 * does better:
 * \code
vector unsigned int test_slw_16_v1 (vector unsigned int vra)
{
  vector unsigned int shft = vec_splat_u32(16-32);
  return vec_sl (vra, shft);
}
 * \endcode
 * The generated code looks like:
 * \code
     vspltisw v0,-16
     vslw    v2,v2,v0
 * \endcode
 *
 * \note the intrinsic vec_splat_u32() has a input range of -16 to 15
 * (16 is not valid as input). But the 5-bit SIM value for -16 is
 * 0b10000, which is (in fact) 16, when applied as a word shift count.
 *
 * \subsubsection common_const_issues_000_0 Generating shift count constants for POWER8
 * The following transform can be used to cover the full range of
 * shift counts for word elements:
 *
 * \code
      if (__builtin_constant_p (sim5) && (sim5 < 16))
	    shft = vec_splat_u32(sim5);
      else
	    shft = vec_splat_u32(sim5 - 32);
 * \endcode
 * This pattern covers the full range of 5-bit (word) shift counts
 * from 0 to 31 (0 to 15 and -16 to -1) with a single instruction.
 * A similar pattern covers the lowest and highest shift counts for
 * 6 and 7-bit shift counts required for double/quadword shift/rotates.
 *
 * \paragraph common_const_issues_000_0_1 Generating 6 and 7-bit shift counts for POWER8
 * Before looking at the POWER9 target, I will cover techniques to
 * generate 6 and 7-bit shift counts for POWER8. This uses a
 * combinations of:
 * - Vector Splat Immedate.
 * - Vector Add/Subtract.
 * - Vector Shift Left.
 * - in 1, 2, or 3 instructions.
 *
 * We should generate the smallest code possible so the order we apply
 * these transforms matter. So start with specific ranges that can be
 * implemented in a single Vector Splat Immediate instruction. This
 * applies to 5/6/7 bit shift counts (and byte/halfword/word splats).
 *
 * The range 0-15 is always a single instruction. The negative SIM
 * range (-16 to -1) trivially extends to cover the highest range of
 * 6/7-bit shift counts (also a single instruction).
 * - A -16 SIM value generates 0b11110000 (240) in the low-order 8-bits.
 *  - The low-order 6-bit value is 0b110000 == 48.
 *  - The low-order 7-bit value is 0b1110000 == 112.
 * - A -1 SIM values generates 0b11111111 (255) in the low-order 8-bits.
 *  - The low-order 6-bit value is 0b111111 == 63.
 *  - The low-order 7-bit value is 0b1111111 == 127.
 *
 * Code examples for 6 and 7-bit shift counts:
 * \code
      if (__builtin_constant_p (sim6) && (sim6 < 16))
	    shft = vec_splat_u32(sim6);
      else if (__builtin_constant_p (sim6) && (sim6 >= 48))
	    shft = vec_splat_u32(sim6 - 64);
 * \endcode
 * And
 * \code
      if (__builtin_constant_p (sim7) && (sim7 < 16))
	    shft = vec_splat_u32(sim7);
      else if (__builtin_constant_p (sim7) && (sim7 >= 112))
	    shft = vec_splat_u32(sim7 - 128);
 * \endcode
 *
 * Next we need to extend 5-bit SIM values to 6/7-bits over the
 * middle ranges:
 * - 16 - 47 for 6 bit
 * - 16 -111 for 7 bit
 *
 * \paragraph common_const_issues_000_0_1_0 Generating mid-range values for 6 and 7-bit shift counts
 *
 * Some even values can be generate in 2 instruction so we should
 * handle those next.
 * Code examples for 6-bit shift counts:
 * \code
      // Already established sim6 > 15 && sim6 < 48
      // Generate even values 18 to 46
	  else if (__builtin_constant_p (sim6) && (sim6 % 2 == 0))
	    {
	      vector unsigned int tmp;
		  if (__builtin_constant_p (sim6) && (sim6 < 32))
		    tmp = vec_splat_u32(sim6 >> 1);
		  else
		    tmp = vec_splat_u32((sim6 >> 1) - 32);
          // (sim6 >> 1) * 2
		  result = vec_add (tmp, tmp);
	    }
 * \endcode
 * \note The compiler may do this for positive shift counts 16-30 but
 * seems unwilling to generate the negative (sign extended) variant
 * for shift counts 32-46. Extending the range using the negative SIM
 * values is not that difficult.
 *
 * By the numbers for 6-bits:
 * - shift count 16 is reduced to 8 and generates:
 *   - vspltisw v2,8; vadduwm v2,v2,v2
 *   - (0b00001000 + 0b00001000) = 0b00010000 = (sim6(0b010000)) = 16
 * - shift count 30 is reduced to 15 and generates:
 *   - vspltisw v2,15; vadduwm v2,v2,v2
 *   - (0b00001111 + 0b00001111) = 0b00011110 = (sim6(0b011110)) = 30
 * - Shift count 32 is reduced to (16-32 ==) -16 and generates:
 *   - vspltisw v2,-16; vadduwm v2,v2,v2
 *   - (0b11110000 + 0b11110000) = 0b11100000 = (sim6(0b100000)) = 32
 * - Shift count 46 is reduced to (23-32 ==) -9 and generates:
 *   - vspltisw v2,-9; vadduwm v2,v2,v2
 *   - (0b11110111 + 0b11110111) = 0b11101110 = (sim6(0b101110)) = 46
 *
 * Code examples for 7-bit shift counts:
 * \code
      // Already established sim7 > 15 && sim7 < 112
      // Generate even values 18 to 30 and 96 to 110
	  else if (__builtin_constant_p (sim7) && (sim7 % 2 == 0)
	       && (sim7 < 32) && (sim7 >= 96))
	    {
	      vector unsigned int tmp;
		  if (__builtin_constant_p (sim7) && (sim7 < 32))
		    tmp = vec_splat_u32(sim7 >> 1);
		  else
		    tmp = vec_splat_u32((sim7 >> 1) - 64);
          // (sim7 >> 1) * 2
		  result = vec_add (tmp, tmp);
	    }
 * \endcode
 * By the numbers for 7-bits:
 * - Even shift counts between 16 and 30 are same for sim6 and sim7
 * - Shift count 96 is reduced to (48-64 ==) -16 and generates:
 *   - vspltisw v2,-16; vadduwm v2,v2,v2
 *   - (0b11110000 + 0b11110000) = 0b11100000 = (sim7(0b1100000)) = 96
 * - Shift count 110 is reduced to (55-64 ==) -9 and generates:
 *   - vspltisw v2,-9; vadduwm v2,v2,v2
 *   - (0b11110111 + 0b11110111) = 0b11101110 = (sim7(0b1101110)) = 110
 *
 * There is a special case for 7-bit shift count 64:
 * \code
    else if (__builtin_constant_p (sim7) && sim7 == 64)
      // 7-bit shift count is 64
      { //  4 * 16
		vector unsigned int v4 = vec_splat_u32(4);
		result = vec_sl (v4, v4);
      }
 * \endcode
 * These 1 and 2 instruction examples cover the high-low 16 ranges
 * and even shift count ranges for 6-bit and the (30 < sim7 >= 96)
 * range for 7-bit.
 *
 * This leaves the sim6 odd shift counts (16 < sim6 < 48) and the
 * sim7 odd shift counts (16 < sim7 < 110) and all shift counts
 * (16 < sim7 < 64) and (64 < sim7 < 96). We can fill in (some of)
 * the range for odd number and more of the modulo 4 middle range
 * with 3 instructions sequences.
 *
 * Covering the odd shift counts in the gap require a extra constant
 * +-16 and a 3 instruction sequence. The SIM field can only generate
 * -16 but we can chose to add or subtract to get the effect of
 * + or -16. For example:
 * \code
	else if (__builtin_constant_p (sim6) && (sim6 > 16) && (sim6 < 32))
	  {
            const vector unsigned int v16 = vec_splat_u32(-16);
	    vector unsigned int tmp = vec_splat_u32(sim6 - 16);
	    result = vec_sub (tmp, v16);
	  }
	else if (__builtin_constant_p (sim6) && (sim6 > 32) && (sim6 < 48))
	  {
            const vector unsigned int v16 = vec_splat_u32(-16);
	    vector unsigned int tmp = vec_splat_u32(sim6 - 48);
	    result = vec_add (tmp, v16);
	  }
 * \endcode
 * By the numbers for 6-bits:
 * - shift count 17 is reduced to 1 and generates:
 *   - vspltisw v0,-16; vspltisw v2,1; vsubuwm v2,v2,v0
 *   - (0b00000001 - 0b11110000) = 0b00010001 = (sim6(0b010001)) = 17
 * - shift count 31 is reduced to 15 and generates:
 *   - vspltisw v0,-16; vspltisw v2,15; vsubuwm v2,v2,v0
 *   - (0b00001111 - 0b11110000) = 0b00011111 = (sim6(0b011111)) = 31
 * - Shift count 33 is reduced to (33-48 ==) -15 and generates:
 *   - vspltisw v0,-16; vspltisw v2,-15; vadduwm v2,v2,v0
 *   - (0b11110001 + 0b11110000) = 0b11100001 = (sim6(0b100001)) = 33
 * - Shift count 47 is reduced to (47-48 ==) -1 and generates:
 *   - vspltisw v0,-16; vspltisw v2,-1; vadduwm v2,v2,v0
 *   - (0b11111111 + 0b11110000) = 0b11101111 = (sim6(0b101111)) = 47
 *
 * \note: The vspltisw instructions are independent and can execute in
 * parallel on POWER7/8/9. Also the const -16 is a common
 * sub-expression and can be hoisted out of loops and/or into the
 * prologue. So latency will be in the 4-6 cycle range.
 *
 * A similar sequence can be used to cover the 7-bit shift counts in
 * the ranges 17-31 and 97-111. For example:
 * \code
	else if (__builtin_constant_p (sim7) && (sim7 > 16) && (sim7 < 32))
	  {
        const vector unsigned int v16 = vec_splat_u32(-16);
	    vector unsigned int tmp = vec_splat_u32(sim6 - 16);
	    result = vec_sub (tmp, v16);
	  }
	else if (__builtin_constant_p (sim7) && (sim7 > 96) && (sim7 < 112))
	  {
        const vector unsigned int v16 = vec_splat_u32(-16);
	    vector unsigned int tmp = vec_splat_u32(sim6 - 112);
	    result = vec_add (tmp, v16);
	  }
 * \endcode
 * By the numbers for 7-bits:
 * - Odd shift counts for the range 17 to 31 are the same as the 6-bit.
 * - Shift count 97 is reduced to (97-112 ==) -15 and generates:
 *   - vspltisw v0,-16; vspltisw v2,-15; vadduwm v2,v2,v0
 *   - (0b11110001 + 0b11110000) = 0b11100001 = (sim7(0b1100001)) = 97
 * - Shift count 111 is reduced to (111-112 ==) -1 and generates:
 *   - vspltisw v0,-16; vspltisw v2,-1; vadduwm v2,v2,v0
 *   - (0b11111111 + 0b11110000) = 0b11101111 = (sim7(0b1101111)) = 111
 *
 * \note The current versions of GCC really does not like this 3
 * instruction sequence and will always convert this to a load from
 * rodata. This is strange since the compilers do not convert the
 * modulo 4 three instruction sequence for 7-bit consts
 *
 * This leaves the sim7 shift counts (32 < sim7 < 64 and 64 < sim7 < 96).
 * We can fill in (some of) the range with multiples of 4 using 3
 * instructions sequences.
 * For sim7 using modulo 4 for example:
 * \code
      // Already established sim7 > 31 && sim7 < 96
	  else if ((sim7 % 4 == 0) && (sim7 < 64))
	  {
	    const vui32_t v2 = vec_splat_u32(2);
	    tmp = vec_splat_u32(sim7 >> 2);

	    result = vec_sl (tmp, v2);
	  }
	  else if ((sim7 % 4 == 0) && (sim7 > 64))
	  {
	    const vui8_t v2 = vec_splat_u32(2);
	    tmp = vec_splat_u32((sim7 >> 2) - 32);

	    result = vec_sl (tmp, v2);
	  }
 * \endcode
 * By the numbers for 7-bits, modulo 4:
 * - shift count 32 is reduced to 8 and generates:
 *   - vspltisw v0,2; vspltisw v2,8; vslw v2,v2,v0
 *   - (0b00001000 << 2) = 0b00100000 = (sim7(0b0100000)) = 32
 * - shift count 60 is reduced to 15 and generates:
 *   - vspltisw v0,2; vspltisw v2,15; vslw v2,v2,v0
 *   - (0b00001111 << 2) = 0b00111100 = (sim7(0b0111100)) = 60
 * - Shift count 68 is reduced to (17-32 ==) -15 and generates:
 *   - vspltisw v0,2; vspltisw v2,-15; vslw v2,v2,v0
 *   - (0b11110001 << 2) = 0b11000100 = (sim7(0b1000100)) = 68
 * - Shift count 92 is reduced to (23-32 ==) -9 and generates:
 *   - vspltisw v0,2; vspltisw v2,-9; vslw v2,v2,v0
 *   - (0b11110111 << 2) = 0b11011100 = (sim7(0b1011100)) = 92
 *
 * \note: The vspltisw instructions are independent and can execute in
 * parallel on POWER7/8/9. Also the shift count 2 is a common
 * sub-expression and can be hoisted out of loops and/or into the
 * prologue. So latency will be in the 4-6 cycle range.
 *
 * So far the examples above cover the ranges 0-31, 96-127, and
 * multiples of 4 between 32 and 92. The generated code runs 1-3
 * instructions and 2-6 cycles latency. More of the range between
 * 32 and 92 can be constructed as some additional complexity
 * (4-5 instruction). With current compiler versions this would be
 * wasted effort as the compiler is likely to replace the intended
 * code with constant value (in .rodata) and load sequence.
 *
 * \note The rationale is; replacing 3 instruction 9 cycle vector
 * load sequences with a 1 to 3 instruction 4-6 cycle
 * splat immediate/add/shift sequence is always a win.
 * A longer sequence (4 or more instructions) may be objectionable.
 *
 * The remaining gaps in the 7-bit shift counts can be filled in using
 * vec_splats (). For example:
 * \code
	  else
	    result = vec_splats ((unsigned char)sim7);
 * \endcode
 *
 * \note For POWER8 this will generate a vector constant in .rodate
 * and will generate the standard vector load sequence.
 * \sa vec_splat5_s8(), vec_splat5_u8(), vec_splat6_s8(),
 * vec_splat6_u8(), vec_splat7_u8().
 *
 * \paragraph common_const_issues_000_0_1_1 Special cases for 6 and 7-bit shift counts
 *
 * The examples above worked well for most 6-bit and some 7-bit shift
 * counts. But for GCC version 8 and later there are too many
 * conversions to .rodata that will impact performance for POWER8/9.
 *
 * Here the quadword shifts have to be implemented with some
 * combination of vsldoi, vslo, vsro, vsl, vsr (long, octet, and double
 * shifts).
 * The vec_splat7_u8() operations should generate xxspltib for POWER9
 * and mostly vspltisb/vaddubm sequences for POWER8. The compiler
 * should be happy passing these results to vec_slo() vec_sll() etc
 * which accept vector unsigned char shift counts (VRB). But no.
 *
 * So for GCC versions 8 or later, use:
 * \code
	// PVECLIB_AVOID_RODATA
	else if ((sim7 > 16) && (sim7 < 112))
	  {
	    const vui8_t v3 = vec_splat_u8(3);
	    const vui8_t vbyte = vec_splat_u8(sim7 / 8);
	    const vui8_t vbit = vec_splat_u8(sim7 % 8);
	    vui8_t voctet;
	    // To cover the odd numbers 16-112.
	    // Use splat immediates and shift left to generate the
	    // octet shift count (high 4-bits). Then splat immediate
	    // the byte bit shift count (low 3-bits). Then sum (add)
	    // to generate the 7-bit (quadword) shift count. Requires
	    // 5 instructions (or 3-4 with CSE) and 6 cycles latency.
	    // This matches vslo/vsl and vsro/vsr requirements.
	    // voctet = vbyte * 8
	    voctet = vec_sl(vbyte, v3);
	    // 7-bit shift count == voctet + vbit
	    result = vec_add (voctet, vbit);
	  }
 * \endcode
 *
 * This sequence runs 5 instructions or less if any of the 3 splat
 * values (v3, vbyte, vbit) constants are common. The latency
 * is still within 6 cycles (beats 9 cycles). While less
 * elegant than the code above it avoids negative values and
 * multiple dependent adds. This seems to get past GCC's
 * convert to .rodata filter. Also GCC will convert this to
 * xxspltib for POWER9.
 *
 * Additional optimizations are possible for quadword immediate
 * shift/rotate operations. For example:
 * \code
      vui8_t sl_a = ((vui8_t) vra);
      if (shb > 7)
	{ // Shift is more the 7-bits
	  vui8_t sl_0 = (vui8_t) vec_splat_u8(0);
	  // Vector Shift Left By Octet (shb/8). Shifting in 0's.
	  sl_a = vec_sld ((vui8_t) vra, sl_0, (shb / 8));
	}
      if ((shb & 7) != 0)
	{ // sub-octet bit shift is required
	  // Vector Shift Left by (shb%8) bits
	  vui8_t lshift = vec_splat_u8(shb % 8);
	  sl_a = vec_sll (sl_a, lshift);
	}
      result = (vui128_t) sl_a;
 * \endcode
 *
 * Splitting the 7-bit shift count into octet (high 4-bits) and
 * bit (low 3-bits) parts makes it easier to generate.
 * Then replace vec_slo() with vec_sld() which includes an
 * immediate octet shift count. So we don't have to generate
 * a separate octet shift count vector but do need an 0's vector
 * to mimic vec_slo using vec_sld.
 * If the low 3-bits of the shift count != 0 then splat immediate
 * the bit shift count and use vec_sll().
 *
 * \subsubsection common_const_issues_000_1 Generating integers for arithmetic for POWER8
 *
 * While the PowerISA shift/rotation instructions have relaxed
 * requirements for shift counts, integer values usually need to be
 * extended to the full element size for arithmetic. Fortunately the
 * PowerISA provides Vector Splat Immediate Signed Byte/Halfword/Word
 * to generate the splatted constant in a single instruction. The
 * limitation is that the constant has to fit within the 5-bit SIM
 * field. If not the compiler with generate a short sequence of
 * immediate/add/shift instructions or a constant load. Current
 * compilers have limited flexibility and default to constant load
 * from memory load for larger values.
 *
 * For performance critical operations avoiding a vector constant load
 * (and potential cache miss) can make a difference. Substituting a
 * short sequence of vector intrinisic operations as described in the
 * section above is an option. The vec_splat5/6/7 operations can be
 * used to generate most 2 digit constants in 1-3 instructions for
 * vector char/short/int operations.
 *
 * It gets trickier for doubleword and quadword integer (long long and
 * __int128) types. The compiler (vector inline extensions and vector
 * intrinsics) really wants any constant used with Double/Quadword
 * element types to be expanded to double/quadword before the
 * operations. The current PowerISA splat immediate instructions
 * limit:
 * - the splatted immediate values to 5/8/32-bits (POWER8/9/10 respectively)
 * - the splatted element size of byte/halfword/word
 * - depending on the PowerISA version
 *
 * \note the PowerISA 3.1C VSX Vector Splat Immediate32 Doubleword
 * Indexed and VSX Vector Splat Immediate Double-Precision are too
 * weird to discuss at this point.
 *
 * So some kind of <I>extend</I> operation is required before any
 * immediate constant value is used in double/quadword arithmetic.
 *
 * \paragraph common_const_issues_000_1_1 Generating immediate values for doubleword
 *
 * POWER8 (PowerISA 2.07) does not include any of new Vector Extend
 * Sign operations from PowerISA 3.0C. POWER8 does provides Vector
 * Unpack High/Low Signed Word. These instructions will sign extend
 * two word elements (from the high/low doubleword) into corresponding
 * doublewords of the result vector. This differs in detail from Vector
 * Extend Sign but is effective to extending spatted word values.
 * So any splatted word (or based sequence) value can be extended to
 * signed doubleword in a single instruction. If does not matter if we
 * unpack high or low as splatted elements are all the same value.
 *
 * This works for signed and unsigned constants less then 32-bits.
 * To zero extend arbitrary 32-bit values you need to use Vector Merge
 * Odd Word or Vector Merge High/low Word. This requires a separate
 * zero vector constant. The zero vector constant is a commonly used
 * and can be shared across multiple vector operations.
 *
 * For the small constants (6/7-bit), unpack adds one more instruction
 * to existing splat immediate sequences.
 * For examplevec_splat6_s64(const signed int sim6):
 * \code
  vi32_t rword = vec_splat6_s32 (sim6);
#if defined(_ARCH_PWR8)
  result = (vi64_t) vec_unpackh (rword);
#else //  defined(_ARCH_PWR7)
  result = (vi64_t) vec_unpackh (((vi16_t)rword);
#endif
 * \endcode
 * And vec_splat6_u64(const signed int sim6):
 * \code
  vui32_t rword = vec_splat6_u32 (sim6);
#if defined(_ARCH_PWR8)
  result = (vui64_t) vec_unpackh ((vi32_t)rword);
#else //  defined(_ARCH_PWR7)
  result = (vui64_t) vec_unpackh ((vi16_t)rword);
#endif
 * \endcode
 *
 * \note GCC has a bad opinion of vec_unpackh/l and converts every
 * sequence ending with it to a const vector (.rodata) and replaces
 * the original sequence with a const vector load sequence.
 * The PVECLIB implementation replaces the intrinsic vec_unpacki()
 * with inline assembler. This stops the constant propagation and
 * avoids the .rodata load.
 *
 * \paragraph common_const_issues_000_1_2 Generating immediate values for quadword
 *
 * POWER8/9 (PowerISA 2.07 and 3.0C) do not include any of the
 * Vector Extend Sign operations for quadword.
 * Nor Vector Shift Right Algebraic Quadword.
 * So some invention is required until POWER10.
 *
 * However it posible to splat the Byte/Word of interest which has
 * the effect of shifting the interesting bits into the high order
 * element of the vector. It is even better if the value of interest
 * has already splatted as originally generated.
 *
 * We also need to generate a quadword representing the zero or sign
 * extended constant. For the general case theequivalent of Vector
 * Expand Byte/Word Mask is signed compare to the zero vector.
 * For example:
 * \code
vi128_t test_vextsb2q_V1 (vi8_t vra)
{
  vi8_t zero = vec_splat_s8 ( 0 );
  vi8_t tmp, mask;
  tmp = vec_splat (vra, 0);
  // generate leading -1 if vra < 0 i.e. Expand Mask Byte
  mask = (vi8_t) vec_cmpgt (zero, tmp);
  return (vi128_t) vec_sld (mask, tmp, 1);
}
 * \endcode
 *
 * In the specific case of generating a quadword immediate value it is
 * possible to use a compile time compare to generate a 0 or -1
 * quadword constant. For example:
 * \code
    vi8_t signmask, tmp;
    if (__builtin_constant_p (sim7) && (sim7 < 0))
      signmask = vec_splat_s8(-1);
    else
      signmask = vec_splat_s8(0);

    tmp = vec_splat7_s8(sim7);
    return (vi128_t) vec_sld (signmask, tmp, 1);
 * \endcode
 * Once the double quadword composed of the 0/sign-mask and the left
 * justified element of interest. The double quadword shift aligns
 * elements of the 0/sign-mask and splatted constant element left by
 * one element. In this case use Vector Shift Left Double by Octet
 * Immediate.
 * For example if the constant is +15:
 * \code
 	vspltisb v0,15
 	vspltisb v2,0
 	vsldoi  v2,v2,v0,1
 * \endcode
 * If the constant is -15:
 * \code
 	vspltisb v0,-16
 	vspltisb v2,-1
 	vsldoi  v2,v2,v0,1
 * \endcode
 *
 * The result is a quadword with the zero/sign mask elements to the
 * left and the original constant in the right most element.
 * This sequence will execute with a 4-6 cycle latency.
 *
 * \subsubsection common_const_issues_000_2 Generating shift count constants for POWER9
 *
 * For power9 the compiler generates a different sequence using
 * VSX Vector Splat Immediate Byte. For example test_slw_16_v0 (above)
 * compiled for -mcpu=power9 generates:
 * \code
	xxspltib 32,16
	vextsb2w 0,0
	vslw 2,2,0
 * \endcode
 * The VSX Splat Immediate Byte has a 8-bit immediate field with a
 * range of 0-255 (-128 to 127). By itself only supports vector char.
 * As we are shifting a vector word value, the compiler feels
 * compelled to add the Extend Signed Byte to Word (vextsb2w)
 * instruction.
 *
 * This is not required for PowerISA shift/rotate instructions.
 * Specifically the shift word (vslw in this case) only needs the low
 * order 5-bits of each word element as the shift count and ignores
 * the high order 27-bits. This also applies to doubleword and quadword
 * shift/rotates which only require 6 and 7-bit shift counts.
 *
 * The VSX Vector Splat form has the advantage of generating to any of
 * the 64 VSX registers. This advantage is reduced as all existing
 * extend sign and shift/rotation instruction are restricted to the
 * original 32 vector registers. Also there is no:
 * - Extend sign byte to halfword (would have to use unpack)
 * - Extend sign (from any size) to quadword
 * - in POWER9 (doubleword to quadword added in POWER10).
 *
 * So Splat Immediate Byte is a good match for byte shift/rotate and
 * the shift left/right long/octet instructions. But there is little
 * utility for Halfword and Word shift/rotates when the original
 * vector splat immediate can generate the shift count and matching
 * element size in a single instruction. For example:
 * \code
 	vspltisw v0,-16
 	vslw    v2,v2,v0
 * \endcode
 *
 * The utility is mixed for Doubleword and Quadword shift/Rotate.
 * Technically the associated shift/rotate operations only require
 * 6 and 7-bit shift counts. So allowing vector char for the shift
 * counts would simplify the code generation. But one additional
 * (extend sign) instruction for doubleword shift/rotate is an
 * improvement over the situation for POWER8.
 *
 * \note To use the doubleword shift/rotate intrinsics POWER8 would
 * require Unpack Left/Right Signed word plus 1-3 instructions to
 * generate the 6-bit shift count.
 *
 * POWER9 does not support vector quadword shift/rotate directly.
 * So PVECLIB simulates those operations using existing shift
 * left/right long/octet instructions. The intrinsics for these
 * instructions do take vector char as the shift counts.
 *
 * \note It would be really helpful if the vector shift/rotate
 * Intrinsics accepted vector char as the shift count for cases.
 * This would avoid these extra instructions.
 * Especially so for Double/Quadword shifts
 *
 * \paragraph common_const_issues_000_2_2 Generating integers for arithmetic
 *
 * The extend sign operation is required to use VSX Vector Splat
 * Immediate Byte for constants used with vector halfword-quadword
 * arithmetic. For example:
 * \code
vector unsigned int
test_add_16_v0 (vector unsigned int vra)
{
  return vra + 16;
}
 * \endcode
 * Generates:
 * \code
	xxspltib 32,16
	vextsb2w 0,0
	vadduwm  2,2,0
 * \endcode
 *
 * This easily extends to vector long long using Extend Signed Byte to
 * Doubleword (vextsb2d) instruction. For example:
 * \code
	xxspltib 32,16
	vextsb2d 0,0
	vaddudm  2,2,0
 * \endcode
 *
 * Generating quadword constants for POWER9 is still challenging and
 * should use the same zero/sign extend techniques used for POWER8.
 *
 * \subsubsection common_const_issues_000_3 Generating shift count constants for POWER10
 *
 * POWER10  (PowerISA 3.1C) adds splat immediate instruction forms
 * with 32-bit immediate values:
 * - VSX Vector Splat Immediate Word
 * - VSX Vector Splat Immediate32 Doubleword Indexed
 *
 * \note VSX Vector Splat Immediate Double-Precision is not useful in
 * this context.
 *
 * These instructions simplify the generation of shift counts where the
 * language or intrinsic requires a word or doubleword shift count
 * parameter.
 *
 * \note So far only vec_splati() is defined to generate VSX Vector
 * Splat Immediate Word. And vec_splati_ins() is defined to generate
 * VSX Vector Splat Immediate32 Doubleword Indexed. These are useful
 * for generating vector int, float, long long, and double constant
 * values for arithmetic in addition to shift constants.
 * See \ref common_const_issues_000_3_1 for more details.
 *
 * POWER10 also adds Vector Extend Sign Doubleword to Quadword.
 * This instruction simplify the generation of shift counts where the
 * language or intrinsic requires a quadword shift count parameter.
 * This assumes that the immediate value has already been promoted to
 * doubleword.
 *
 * POWER10 also add quadword shift/rotate instructions. These are a bit
 * odd as they require the shift count in bits 57:63 of VRB. Also the
 * Vector Intrinsics are defined and implemented to require the shift
 * count in bits 122:127 of vector unsigned __int128. This requires the
 * compiler to generate a xxswapd before every quadword shift/rotate.
 *
 * \note The new quadword shift intrinsic seem to follow the precedent
 * of the shift left/right long/octet instructions. It would have been
 * more useful if the new quadword intrinsics also allowed vector
 * unsigned char for the VRB shift count.
 *
 * Finally POWER10 adds Vector Shift Left/Right Double by Bit
 * Immediate instructions. These complement the Vector Shift
 * Left Double by Octet Immediate operation to support 1-127 bit
 * shift/rotate. Here the shift counts are embedded in the shift
 * instruction and simplify the implementation of quadword
 * shift/rotate with constant shift counts.
 *
 * \paragraph common_const_issues_000_3_1 Generating integers for arithmetic
 *
 * POWER10 (PowerISA 3.1C) adds VSX Vector Splat Immediate Word,
 * VSX Vector Splat Immediate32 Doubleword Indexed and
 * VSX Vector Splat Immediate Double-Precision.
 * All are prefix form instructions with a 32-bit immediate field.
 * The Immediate Double-Precision form converts the single-precision
 * 32-bit immediate value to double-precision before splatting the
 * DW result. The Immediate32 Doubleword Indexed form can generate a
 * full 64-bit immediate value in two steps by manipulating the index
 * [IX] field. These new instructions enhance the range of splat
 * immediate constants from a single byte to word and doubleword.
 *
 * Generating quadword (vector __int128) constants is enhanced by
 * Vector Extend Sign Doubleword to Quadword. This requires a 3
 * instruction sequence to generate a quadword constant from any
 * word/doubleword immediate value. For example:
 * - Either
 *   - VSX Vector Splat Immediate Word, followed by
 *   - Vector Extend Sign Word To Doubleword (From PowerISA 3.0c), and
 *   - Then a final Vector Extend Sign Doubleword to Quadword
 * - Or
 *   - A pair of VSX Vector Splat Immediate32 Doubleword Indexed instructiona, and
 *   - Then a final Vector Extend Sign Doubleword to Quadword
 *
 * \subsubsection common_const_issues_000_4 A tactical responses for PVECLIB
 *
 * The implementation of POWER8 operations for vector double/quad
 * floating point instructions introduced for POWER9 was challenging.
 * This exposed a pressing need for fast generation of small integer
 * constants splatted to vectors. This was common for shift/rotate
 * counts and exponent arithmetic.
 *
 * The obvious solution of using vector constants is more expensive
 * then expected given the default medium model PIC addressing and how
 * the compiler generates TOC relative vector loads from read-only
 * sections (.rodata). For current GCC versions (8-14) and POWER8/9
 * targets this is 3 instructions and 9 cycles latency (minimum).
 * Improvements are possible and a PR (117718) has been posted to
 * the GCC community. But the best improvement we can expect is a 7
 * cycle latency for POWER8/9. POWER10 will be slightly better when
 * plxv is used (4-6 cycles).
 *
 * This is a high overhead to setup a 2/3 cycle shift/rotate/add
 * operation. On POWER8 a splat immediate instruction runs 2 cycles
 * latency (3 cycles POWER9). For POWER8 the 5-bit signed immediate
 * values can be extended to 6/7-bit values by custom 2/3 instruction
 * sequences generated from compile time constant values. These run
 * 4-6 cycles.
 *
 * For POWER9 the VSX Vector Splat Immediate Byte instruction followed
 * by Vector Extend Sign Byte to Word/Doubleword runs 5 cycles. So for
 * POWER8/9 the the splat immediate sequences (for small integer
 * values) beats the const vector load by a wide margin.
 *
 * An optimist might think that an optimizing compiler would spot
 * patterns of const vector values and replace those load sequence with
 * a shorter/faster splat immediate sequence. But instead the current
 * GCC versions frequently convert (hand optimized) splat immediate
 * sequences with larger/slower constant (.rodata) vector loads.
 *
 * \note This is new behavior for GCC starting with GCC 8. PRs
 * (117007, 117818) have been posted to the GCC community.
 *
 * Language standards tend to require values left/right of the operator
 * be the same (element) size. This extends to Vector Intrinsics that
 * mimic C operators, including shift and rotate. This often requires
 * additional instructions to zero/sign extend an easily generated
 * immediate shift count to the correct size. This is most problematic
 * for the new vector long long and __int128 (doubleword/quadword)
 * types.
 *
 * Current GCC versions do not offer much help for POWER8/9. GCC does
 * not support literal constants for __int128 and any attempt to extend
 * a word or doubleword constants to quadword is converted into .rodata
 * constant. Also attempts to extend vec_splat_s/u32() values to
 * doubleword using vec_unpackh/l() get the same treatment.
 *
 * While zero/sign extending constants to match the variable size is a
 * clear requirement for general arithmetic, it is not strictly
 * required for shift/rotate operations. The PowerISA vector
 * shift/Rotate instructions requires the 3-7-bit shift count be right
 * justified in the vector element and ignores the rest. So requiring
 * shift counts be extending to doubleword or quadword is is wasted
 * effort.
 *
 * The compiler also takes the idea that shift counts should be
 * unsigned way too seriously. The PowerISA shift/rotate instructions
 * will happily take a -16 signed immediate value as a:
 * - Shift/Rotate word 16 bits
 * - Shift/Rotate doubleword 48 bits
 * - Shift/Rotate quadword 112 bits
 *
 * Left to is own devices the compiler with convert the literal shift
 * count 16 into 8*2. It knows that Vector Splat Immediate Signed Word
 * can not generate 16. But it is wrong in this specific case and many
 * others.
 *
 * \paragraph common_const_issues_000_4_1 PVECLIB's problem
 *
 * The PVECLIB project has expended a lot of effort to optimize these
 * cases and attempts to work around what the compiler is doing. Most
 * of which has been futile. Much of POWER10 work has been blocked by
 * this and it is long past time to move on (PVECLIB release 1.0.5).
 * So a more tactical plan is needs to stabilize the exiting code and
 * move on the other topics within the PVECLIB project.
 *
 * The GCC PRs (117007, 117718, 117818) have not progressed
 * (still status: UNCONFIRMED). Even if these problems are resolve in
 * 2025, any fixes will be restricted to current GCC 15 and recent
 * releases (GCC 14, 13?). This leaves the problems unresolved for
 * GCC 8-12 (POWER8 versions on RHEL and Unbuntu 2020 LTS).
 *
 *
 * \paragraph common_const_issues_000_4_2 PVECLIB will help where it can
 *
 * PVECLIB can't easily address the const vector (.rodata) loads (PR 117718)
 * within PVECLIB. But PVECLIB should avoid or prevent cases where the
 * compiler transforms splat immediates and related sequences into
 * expensive const vectors loads.
 *
 * From test cases (PR 117007, 117818),
 * It appears that these transforms are triggered by;
 * - Longer sequences (4+ instructions)
 * - Immediate sequences involving two or more different values and add/subtract.
 * - Extending splatted values to larger element sizes (unpack).
 *
 * It seems like GCC is OK generating splatted immediate constants,
 * but does not like it with you (try to) use them!
 *
 * From experience, using inline assembler breaks the compilers
 * constant propagation phase. Also it seems stupid to generate
 * long long or __int128 shift counts when the PowerISA only requires
 * 6 or 7 bits (per element).
 *
 * So PVECLIB can introduce shift/rotate operations that accept vector
 * unsigned char for shift counts. This would cover vec_sl, vec_sr,
 * vec_sra, vec_rl for word/doubleword/quadword elements. The Vector
 * intrinsic reference does not (currently) allow this so PVECLIB would
 * use inline asm for the actual vector shift/rotate instructions.
 * For example:
 * \code
static inline vui64_t
vec_vsld_byte (vui64_t vra, vui8_t vrb)
{
  vui64_t result;

  __asm__(
      "vsld %0,%1,%2;"
      : "=v" (result)
      : "v" (vra),
      "v" (vrb)
      : );

  return (result);
}
 * \endcode
 *
 * This is not so wild as the existing shift long/octet Intrinsics
 * (vec_sll, vec_srl, vec_slo, vec_sro) already accept vector char as
 * (7-bit) shift counts for what are effectively quadword shifts.
 * These are used in PVECLIB to implement the quadword shift/rotate
 * (immediate) operations for POWER8/9 (anticipating POWER10 and
 * vrlq/vslq/vsrq/vsraq).
 *
 * PVECLIB will add operations for splatting 5/6/7-bit constants into
 * byte/word elements. These will be implemented via existing
 * splat/add/sub/shift intrinsics in vec_common_ppc.h. For example
 * vec_splat6_u8() would handle splatting constants (0-63) to vector
 * unsigned char. The result may be sign extended but the low-order
 * 6-bits of each bytes would be appropriate for shift/rotate
 * doubleword.
 *
 * The combination of vec_splat6_u8/vec_vsld_byte can then be used to
 * implement a shift left doubleword immediate. The use of inline asm
 * for the shifts will prevent CSE for the shifts but still allows for
 * commoning of the shift counts associated with shift immediate
 * operations.
 *
 * Similarly PVECLIB can provide operations like vec_splat_s64 ()
 * to generate small constants for vector long long. For POWER8 this
 * generates a splat immediate word (sequence) and unpack signed word,
 * The current compilers like to convert any sequence starting with a
 * splat immediate and ending in unpack into a .rodata constant and
 * load sequences. The result is usually more instructions and latency.
 * So again the PVECLIB implementation resorts to inline asm to
 * generate the trailing vupklsw.
 *
 * Similarly for vec_splat_s128 operations to generate small constants
 * for vector __int128. Here the implementation will generate the
 * splatted byte/word then shift/rotate zero/sign bits in using
 * vec_sld() (vsldio). This will be a minimum 3 instruction sequence
 * but the latency will run only 4-6 cycles.
 * Again better than the .rodata load.
 *
 * So in summary:
 * - Use inline asm to limit the compiler predilection to converting
 * immediate sequences into .rodata constants and loads.
 * - Provide shift/rotate by byte operations that accept vector
 * unsigned char as the shift count for word/doubleword/quadword.
 * - Provide vec_splat5/6/7_u8() operations to generate 5/6/7-bit
 * shift counts.
 *   - Use vec_splat5/6/7_u8() and the shift/rotate_byte operations to
 *   implement shift/rotate doubleword immediate.
 * - Update vec_splat_s64 to use vec_splat6_u32() and vec_unpackl().
 *   - Use inline asm vupklsw if the compiler does not behave
 * - Update vec_splat_s128 to use vec_splat7_u8() and expand mask pattern.
 *
 *
 */

/*! \brief Macro to add platform suffix for static calls.
 * \sa \ref main_libary_issues_0_0_0_2
 * \sa \ref main_libary_issues_0_0 */
#ifdef _ARCH_PWR10
#define __VEC_PWR_IMP(FNAME) FNAME ## _PWR10
#else
#ifdef _ARCH_PWR9
#define __VEC_PWR_IMP(FNAME) FNAME ## _PWR9
#else
#ifdef _ARCH_PWR8
#define __VEC_PWR_IMP(FNAME) FNAME ## _PWR8
#else
#define __VEC_PWR_IMP(FNAME) FNAME ## _PWR7
#endif
#endif
#endif

/*! \brief vector of 8-bit unsigned char elements. */
typedef __vector unsigned char vui8_t;
/*! \brief vector of 16-bit unsigned short elements. */
typedef __vector unsigned short vui16_t;
/*! \brief vector of 32-bit unsigned int elements. */
typedef __vector unsigned int vui32_t;
/*! \brief vector of 64-bit unsigned long long elements. */
typedef __vector unsigned long long vui64_t;

/*! \brief vector of 8-bit signed char elements. */
typedef __vector signed char vi8_t;
/*! \brief vector of 16-bit signed short elements. */
typedef __vector short vi16_t;
/*! \brief vector of 32-bit signed int elements. */
typedef __vector int vi32_t;
/*! \brief vector of 64-bit signed long long elements. */
typedef __vector long long vi64_t;
/*! \brief vector of 32-bit float elements. */
typedef __vector float vf32_t;
/*! \brief vector of 64-bit double elements. */
typedef __vector double vf64_t;

/*! \brief vector of 8-bit bool char elements. */
typedef __vector __bool char vb8_t;
/*! \brief vector of 16-bit bool short elements. */
typedef __vector __bool short vb16_t;
/*! \brief vector of 32-bit bool int elements. */
typedef __vector __bool int vb32_t;
/*! \brief vector of 64-bit bool long long elements. */
typedef __vector __bool long long vb64_t;

/* did not get vector __int128 until GCC4.8.  */
#ifndef PVECLIB_DISABLE_INT128
/*! \brief vector of one 128-bit signed __int128 element. */
typedef __vector __int128 vi128_t;
/*! \brief vector of one 128-bit unsigned __int128 element. */
typedef __vector unsigned __int128 vui128_t;
#ifndef PVECLIB_DISABLE_BOOLINT128
/*! \brief vector of one 128-bit bool __int128 element. */
typedef __vector __bool __int128 vb128_t;
#else
/*! \brief vector of one 128-bit bool __int128 element. */
typedef __vector __bool int vb128_t;
#endif
#else
/*! \brief vector of one 128-bit signed __int128 element. */
typedef __vector int vi128_t;
/*! \brief vector of one 128-bit unsigned __int128 element. */
typedef __vector unsigned int vui128_t;
/*! \brief vector of one 128-bit bool __int128 element. */
typedef __vector __bool int vb128_t;
#endif

/*! \brief Union used to transfer 128-bit data between vector and
 * non-vector types. */
typedef union
{
  /*! \brief Signed 128-bit integer from pair of 64-bit GPRs.  */
  signed __int128 i128;
  /*! \brief Unsigned 128-bit integer from pair of 64-bit GPRs.  */
  unsigned __int128 ui128;
#ifndef PVECLIB_DISABLE_DFP
  /*! \brief 128 bit Decimal Float from pair of double float registers.  */
  _Decimal128 dpd128;
#endif
  /*! \brief IBM long double float from pair of double float registers.  */
  long double ldbl128;
  /*! \brief 128 bit Vector of 16 unsigned char elements.  */
  vui8_t vx16;
  /*! \brief 128 bit Vector of 8 unsigned short int elements.  */
  vui16_t vx8;
  /*! \brief 128 bit Vector of 4 unsigned int elements.  */
  vui32_t vx4;
  /*! \brief 128 bit Vector of 2 unsigned long int (64-bit) elements.  */
  vui64_t vx2;
  /*! \brief 128 bit Vector of 1 unsigned __int128 element.  */
  vui128_t vx1;
  /*! \brief 128 bit Vector of 2 double float elements.  */
  vf64_t vf2;
  /*! \brief Struct of two unsigned long int (64-bit GPR) fields.  */
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
} __VEC_U_128;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
/*! \brief Arrange elements of dword initializer in high->low order.  */
#define CONST_VINT64_DW(__dw0, __dw1) {__dw1, __dw0}
/*! \brief Initializer for 128-bits vector, as two unsigned long long
 * elements in high->low order.  May require an explicit cast. */
#define CONST_VINT128_DW(__dw0, __dw1) (vui64_t){__dw1, __dw0}
/*! \brief A vector unsigned __int128 initializer, as two unsigned
 * long long elements in high->low order.  */
#define CONST_VINT128_DW128(__dw0, __dw1) (vui128_t)((vui64_t){__dw1, __dw0})
/*! \brief Arrange word elements of a unsigned int initializer in
 * high->low order.  May require an explicit cast.  */
#define CONST_VINT128_W(__w0, __w1, __w2, __w3) (vui32_t){__w3, __w2, __w1, __w0}
/*! \brief Arrange elements of word initializer in high->low order.  */
#define CONST_VINT32_W(__w0, __w1, __w2, __w3) {__w3, __w2, __w1, __w0}
/*! \brief Arrange halfword elements of a unsigned int initializer in
 * high->low order.  May require an explicit cast.  */
#define CONST_VINT128_H(__hw0, __hw1, __hw2, __hw3, __hw4, __hw5, __hw6, __hw7) \
    (vui16_t){__hw7, __hw6, __hw5, __hw4, __hw3, __hw2, __hw1, __hw0}
/*! \brief Arrange elements of halfword initializer in high->low order.  */
#define CONST_VINT16_H(__hw0, __hw1, __hw2, __hw3, __hw4, __hw5, __hw6, __hw7) \
    {__hw7, __hw6, __hw5, __hw4, __hw3, __hw2, __hw1, __hw0}
/*! \brief Arrange byte elements of a unsigned int initializer in
 * high->low order.  May require an explicit cast.  */
#define CONST_VINT128_B(_b0, _b1, _b2, _b3, _b4, _b5, _b6, _b7, _b8, _b9, _b10, _b11, _b12, _b13, _b14, _b15) \
    (vui8_t){_b15, _b14, _b13, _b12, _b11, _b10, _b9, _b8, _b7, _b6, _b5, _b4, _b3, _b2, _b1, _b0}
/*! \brief Arrange elements of byte initializer in high->low order.  */
#define CONST_VINT8_B(_b0, _b1, _b2, _b3, _b4, _b5, _b6, _b7, _b8, _b9, _b10, _b11, _b12, _b13, _b14, _b15) \
    {_b15, _b14, _b13, _b12, _b11, _b10, _b9, _b8, _b7, _b6, _b5, _b4, _b3, _b2, _b1, _b0}
/*! \brief Element index for high order dword.  */
#define VEC_DW_H 1
/*! \brief Element index for low order dword.  */
#define VEC_DW_L 0
/*! \brief Element index for highest order word.  */
#define VEC_W_H 3
/*! \brief Element index for lowest order word.  */
#define VEC_W_L 0
/*! \brief Element index for vector splat word 0.  */
#define VEC_WE_0 3
/*! \brief Element index for vector splat word 1.  */
#define VEC_WE_1 2
/*! \brief Element index for vector splat word 2.  */
#define VEC_WE_2 1
/*! \brief Element index for vector splat word 3.  */
#define VEC_WE_3 0
/*! \brief Element index for highest order hword.  */
#define VEC_HW_H 7
/*! \brief Element index for lowest order hword of the high dword.  */
#define VEC_HW_L_DWH 4
/*! \brief Element index for lowest order hword.  */
#define VEC_HW_L 0
/*! \brief Element index for lowest order byte.  */
#define VEC_BYTE_L 0
/*! \brief Element index for lowest order byte of the high dword.  */
#define VEC_BYTE_L_DWH 8
/*! \brief Element index for lowest order byte of the low dword.  */
#define VEC_BYTE_L_DWL 0
/*! \brief Element index for highest order byte of the high dword.  */
#define VEC_BYTE_H_DWH 15
/*! \brief Element index for highest order byte of the low dword.  */
#define VEC_BYTE_H_DWL 7
/*! \brief Element index for highest order byte.  */
#define VEC_BYTE_H 15
/*! \brief Element index for second lowest order byte.  */
#define VEC_BYTE_HHW 14
#else
#define CONST_VINT64_DW(__dw0, __dw1) {__dw0, __dw1}
#define CONST_VINT128_DW(__dw0, __dw1) (vui64_t){__dw0, __dw1}
#define CONST_VINT128_DW128(__dw0, __dw1) (vui128_t)((vui64_t){__dw0, __dw1})
#define CONST_VINT128_W(__w0, __w1, __w2, __w3) (vui32_t){__w0, __w1, __w2, __w3}
#define CONST_VINT32_W(__w0, __w1, __w2, __w3) {__w0, __w1, __w2, __w3}
/*! \brief Arrange word elements of a unsigned int initializer in
 * high->low order.  May require an explicit cast.  */
#define CONST_VINT128_H(__hw0, __hw1, __hw2, __hw3, __hw4, __hw5, __hw6, __hw7) \
    (vui16_t){__hw0, __hw1, __hw2, __hw3, __hw4, __hw5, __hw6, __hw7}
/*! \brief Arrange elements of word initializer in high->low order.  */
#define CONST_VINT16_H(__hw0, __hw1, __hw2, __hw3, __hw4, __hw5, __hw6, __hw7) \
    {__hw0, __hw1, __hw2, __hw3, __hw4, __hw5, __hw6, __hw7}
/*! \brief Arrange byte elements of a unsigned int initializer in
 * high->low order.  May require an explicit cast.  */
#define CONST_VINT128_B(_b0, _b1, _b2, _b3, _b4, _b5, _b6, _b7, _b8, _b9, _b10, _b11, _b12, _b13, _b14, _b15) \
    (vui8_t){_b0, _b1, _b2, _b3, _b4, _b5, _b6, _b7, _b8, _b9, _b10, _b11, _b12, _b13, _b14, _b15}
/*! \brief Arrange elements of byte initializer in high->low order.  */
#define CONST_VINT8_B(_b0, _b1, _b2, _b3, _b4, _b5, _b6, _b7, _b8, _b9, _b10, _b11, _b12, _b13, _b14, _b15) \
    {_b0, _b1, _b2, _b3, _b4, _b5, _b6, _b7, _b8, _b9, _b10, _b11, _b12, _b13, _b14, _b15}
#define VEC_DW_H 0
#define VEC_DW_L 1
#define VEC_W_H 0
#define VEC_W_L 3
#define VEC_WE_0 0
#define VEC_WE_1 1
#define VEC_WE_2 2
#define VEC_WE_3 3
#define VEC_HW_H 0
/*! \brief Element index for lowest order hword of the high dword.  */
#define VEC_HW_L_DWH 3
#define VEC_HW_L 7
#define VEC_BYTE_L 15
/*! \brief Element index for lowest order byte of the high dword.  */
#define VEC_BYTE_L_DWH 7
/*! \brief Element index for lowest order byte of the low dword.  */
#define VEC_BYTE_L_DWL 15
/*! \brief Element index for highest order byte of the high dword.  */
#define VEC_BYTE_H_DWH 0
/*! \brief Element index for highest order byte of the low dword.  */
#define VEC_BYTE_H_DWL 8
#define VEC_BYTE_H 0
#define VEC_BYTE_HHW 1
#endif

/*! \brief table powers of 10 [0-38] in vector __int128 format.  */
extern const vui128_t vtipowof10[];
/*! \brief table used to verify 128-bit frexp operations for powers of 10.  */
extern const vui128_t vtifrexpof10[];

#ifndef PVECLIB_DISABLE_DFP
/*! \brief table powers of 2 [0-1077] in _Decimal128 format.  */
extern const _Decimal128 decpowof2[];
#endif

///@cond INTERNAL
static inline vui32_t vec_popcntw_PWR7 (vui32_t);
static inline vui32_t vec_popcntw_PWR8 (vui32_t);
static inline vui64_t vec_popcntd_PWR7 (vui64_t);
static inline vui64_t vec_popcntd_PWR8 (vui64_t);
static inline vui128_t vec_popcntq_PWR8 (vui128_t);
static inline vui128_t vec_popcntq_PWR9 (vui128_t);
static inline vui8_t vec_splat6_u8 (const unsigned int sim6);
static inline vui8_t vec_splat7_u8 (const unsigned int sim7);
static inline vui128_t vec_vexpandqm_PWR7 (vui128_t vra);
static inline vui128_t vec_vexpandqm_PWR10 (vui128_t vra);
static inline vui64_t vec_vrld_PWR8 (vui64_t vra, vui8_t vrb);
static inline vui64_t vec_vsld_PWR8 (vui64_t vra, vui8_t vrb);
static inline vui64_t vec_vsrd_PWR8 (vui64_t vra, vui8_t vrb);
static inline vui128_t vec_vrlq_PWR9 (vui128_t vra, vui8_t vrb);
static inline vui128_t vec_vrlq_PWR10 (vui128_t vra, vui8_t vrb);
static inline vui128_t vec_vslq_PWR9 (vui128_t vra, vui8_t vrb);
static inline vui128_t vec_vslq_PWR10 (vui128_t vra, vui8_t vrb);
static inline vui128_t vec_vsrq_PWR9 (vui128_t vra, vui8_t vrb);
static inline vui128_t vec_vsrq_PWR10 (vui128_t vra, vui8_t vrb);
static inline vi128_t vec_vsraq_PWR10 (vi128_t vra, vui8_t vrb);
///@endcond

/** \brief Vector Add Unsigned Doubleword Modulo for POWER7 and earlier.
 *
 *  Add two vector long int values and return modulo 64-bits result.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 8-16  |   NA     |
 *
 *  @param a 128-bit vector long long int.
 *  @param b 128-bit vector long long int.
 *  @return vector long long int sums of a and b.
 */
static inline vui64_t
vec_addudm_PWR7 (vui64_t a, vui64_t b)
{
  const vui32_t vone = vec_splat_u32 (1);
  const vui32_t vzero = vec_splat_u32 (0);
#if 1
  const vui32_t cm = vec_mergeh (vzero, vone);
#else
  const vui32_t cm= { 0,1,0,1};
  // const vui32_t cm = (vui32_t) vec_unpackh ((vi16_t) vone);
#endif
  vui32_t r;
  vui32_t c;

  // propagate carry from words 1/3 into words 0/2
  c = vec_vaddcuw ((vui32_t)a, (vui32_t)b);
  r = vec_vadduwm ((vui32_t)a, (vui32_t)b);
  // Ignore carry's from words 0/2
  c = vec_and (c, cm);
  // rotate Words 1/3 carry's into position for words 0/2 extend.
  c = vec_sld (c, c, 4);
  // Add/extend carry's to words 0/2
  return ((vui64_t) vec_vadduwm (r, c));
}

/** \brief Vector Add Unsigned Quadword Modulo for POWER7 and earlier.
 *
 *  Add two vector __int128 values and return result modulo 128-bits.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 14-16 |   NA     |
 *
 *  @param vra 128-bit vector treated as a __int128.
 *  @param vrb 128-bit vector treated as a __int128.
 *  @return __int128 sum of vra and vrb.
 */
static inline vui128_t
vec_adduqm_PWR7 (vui128_t vra, vui128_t vrb)
{
  const vui32_t z = vec_splat_u32 (0);
  vui32_t t;
  vui32_t c, c2;

  c = vec_vaddcuw ((vui32_t)vra, (vui32_t)vrb);
  t = vec_vadduwm ((vui32_t)vra, (vui32_t)vrb);
  c = vec_sld (c, z, 4);
  c2 = vec_vaddcuw (t, c);
  t = vec_vadduwm (t, c);
  c = vec_sld (c2, z, 4);
  c2 = vec_vaddcuw (t, c);
  t = vec_vadduwm (t, c);
  c = vec_sld (c2, z, 4);
  t = vec_vadduwm (t, c);
  return ((vui128_t) t);
}

/** \brief Vector Add Unsigned Quadword Modulo for POWER8 and earlier.
 *
 *  Add two vector __int128 values and return result modulo 128-bits.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 14-16 |   NA     |
 *  |power8   | 4     |2/2 cycles|
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as a __int128.
 *  @param b 128-bit vector treated as a __int128.
 *  @return __int128 sum of a and b.
 */
static inline vui128_t
vec_adduqm_PWR8 (vui128_t a, vui128_t b)
{
  vui32_t t;
#ifdef _ARCH_PWR8
#if defined (vec_vadduqm)
  t = (vui32_t) vec_vadduqm (a, b);
#elif defined (__clang__)
  t = (vui32_t) vec_add (a, b);
#else
  __asm__(
      "vadduqm %0,%1,%2;"
      : "=v" (t)
      : "v" (a),
      "v" (b)
      : );
#endif
#else
  t = (vui32_t) vec_adduqm_PWR7 (a, b);
#endif
  return ((vui128_t) t);
}

/** \brief Vector Count Leading Zeros word for POWER7 and earlier.
 *
 *  Count the number of leading '0' bits (0-32) within each word
 *  element of a 128-bit vector.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the Vector Count Leading
 *  Zeros Word instruction <B>vclzw</B>. Otherwise use sequence of pre
 *  2.07 VMX instructions.
 *  SIMDized count leading zeros inspired by:
 *
 *  Warren, Henry S. Jr and <I>Hacker's Delight</I>, 2nd Edition,
 *  Addison Wesley, 2013. Chapter 5 Counting Bits, Floating-Point
 *  Methods.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 16-26 |   NA     |
 *
 *  @param vra 128-bit vector treated as 4 x 32-bit integer (words)
 *  elements.
 *  @return 128-bit vector with the Leading Zeros count for each word
 *  element.
 */
static inline vui32_t
vec_clzw_PWR7 (vui32_t vra)
{
  // Updated to match vec-common-ppc.h vec_clzw_PWR7()
  // generated const (vui32_t) {23, 23, 23, 23}
  const vui32_t v23 = vec_splat_u32 (23-32);
  const vui32_t vone = vec_splat_u32 (1);
  // conservation of const, use -9 instead of -1
  const vui32_t v2_31 = vec_vslw (v23, v23);
  // Need const for SP exponents, let vFPU do the work
  // f2_31 = (vector float) ~2**31
  const vf32_t f2_31 =  vec_ctf (v2_31, 0);
  // fp5 = 1.0 / 2.0 == 0.5, for the clzw(0) case
  const vf32_t fp5 = vec_ctf (vone, 1);

  vui32_t k, n;
  vb32_t kb;
  vf32_t kf;
  // Avoid rounding in floating point conversion
  k = vra & ~(vra >> 1);
  // Handle all 0s case by insuring min exponent is 158-32
  // detect zero and substitute 0.5
  // conservation of const, cmplt 1 vs cmpeq 0
  kb = vec_cmplt (vra, vone);
  kf = vec_ctf (k, 0);
  kf = vec_sel (kf, fp5, kb);
  n = vec_vsrw((vui32_t) f2_31, v23);
  k = vec_vsrw((vui32_t) kf, v23);
  n = n - k;
  return n;
}

/** \brief Vector Count Leading Zeros Doubleword for POWER7 and earlier.
 *
 *  Count the number of leading '0' bits (0-64) within each doubleword
 *  element of a 128-bit vector.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the Vector Count Leading
 *  Zeros Doubleword instruction <B>vclzd</B>. Otherwise use sequence of
 *  pre 2.07 VMX instructions.
 *  SIMDized count leading zeros inspired by:
 *
 *  Warren, Henry S. Jr and <I>Hacker's Delight</I>, 2nd Edition,
 *  Addison Wesley, 2013. Chapter 5 Counting Bits, Floating-Point
 *  Methods.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 22-33 |   NA     |
 *
 *  @param vra a 128-bit vector treated as 2 x 64-bit unsigned
 *  long long int (doubleword) elements.
 *  @return 128-bit vector with the leading zeros count for each
 *  doubleword element.
 */
static inline vui64_t
vec_clzd_PWR7 (vui64_t vra)
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

/** \brief Vector Count Leading Zeros Doubleword for unsigned
 *  long long elements.
 *
 *  Count the number of leading '0' bits (0-64) within each doubleword
 *  element of a 128-bit vector.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the Vector Count Leading
 *  Zeros Doubleword instruction <B>vclzd</B>. Otherwise use
 *  vec_clzd_PWR7().
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 22-33 |   NA     |
 *  |power8   |   2   | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as 2 x 64-bit unsigned
 *  long long (doubleword) elements.
 *  @return 128-bit vector with the leading zeros count for each
 *  doubleword element.
 */
static inline vui64_t
vec_clzd_PWR8 (vui64_t vra)
{
  vui64_t r;
#ifdef _ARCH_PWR8
#if defined (vec_vclzd)
  r = vec_vclzd (vra);
#elif defined (__clang__)
  r = vec_cntlz (vra);
#else
  __asm__(
      "vclzd %0,%1;"
      : "=v" (r)
      : "v" (vra)
      : );
#endif
#else
  r = vec_clzd_PWR7 (vra);
#endif
  return (r);
}

/** \brief Vector Count Leading Zeros Quadword for POWER7 and earlier.
 *
 *  Count leading zeros for a vector __int128 and return the count in a
 *  vector suitable for use with vector shift (left|right) and vector
 *  shift (left|right) by octet instructions.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  8-10 | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @return a 128-bit vector with bits 121:127 containing the count of
 *  leading zeros.
 */
static inline vui128_t
vec_clzq_PWR7 (vui128_t vra)
{
  vui32_t result;
  /* vector clz instructions were introduced in power8. For power7 and
   * earlier, use the pveclib vec_clzw_PWR7 implementation.  For a quadword
   * clz, this requires pre-conditioning the input before computing the
   * the word clz and sum across.   */
  vui32_t c0, clz;
  vui32_t r32, gt32, gt32sr32, gt64sr64;

  c0 = vec_splat_u32 (0);
  gt32 = (vui32_t) vec_cmpgt ((vui32_t) vra, c0);
  gt32sr32 = vec_sld (c0, gt32, 12);
  gt64sr64 = vec_sld (c0, gt32, 8);
  gt32 = vec_sld (c0, gt32, 4);

  gt32sr32 = vec_or (gt32sr32, gt32);
  gt64sr64 = vec_or (gt64sr64, (vui32_t) vra);
  r32 = vec_or (gt32sr32, gt64sr64);

  clz = vec_clzw_PWR7 (r32);
  result = (vui32_t) vec_sums ((vi32_t) clz, (vi32_t) c0);

  return ((vui128_t) result);
}

/** \brief Vector Count Leading Zeros Quadword for POWER8.
 *
 *  Count leading zeros for a vector __int128 and return the count in a
 *  vector suitable for use with vector shift (left|right) and vector
 *  shift (left|right) by octet instructions.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  8-10 | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @return a 128-bit vector with bits 121:127 containing the count of
 *  leading zeros.
 */
static inline vui128_t
vec_clzq_PWR8 (vui128_t vra)
{
  vui32_t result;

#ifdef _ARCH_PWR8
  /*
   * Use the Vector Count Leading Zeros Double Word instruction to get
   * the count for the left and right vector halves.  If the left vector
   * doubleword of the input is nonzero then only the left count is
   * included and we need to mask off the right count.
   * Otherwise the left count is 64 and we need to add 64 to the right
   * count. After masking we sum across the left and right counts to
   * get the final 128-bit vector count (0-128).
   */
  vui32_t vt1, vt2, vt3, h64, l64;
  const vui32_t vzero = { 0, 0, 0, 0 };
  vt1 = (vui32_t) vec_clzd_PWR8 ((vui64_t) vra);
  vt2 = (vui32_t) vec_cmpeq((vui64_t) vra, (vui64_t) vzero);
  vt3 = vec_sld ((vui32_t)vzero, vt2, 8);
  h64 = vec_sld ((vui32_t)vzero, vt1, 8);
  l64 = vec_and (vt1, vt3);
  result = vec_add (h64, l64);
#else
  result = (vui32_t) vec_clzq_PWR7 (vra);
#endif
  return ((vui128_t) result);
}

/** \brief Vector Count Trailing Zeros Word for POWER7 and earlier.
 *
 *  Count the number of trailing '0' bits (0-32) within each word
 *  element of a 128-bit vector.
 *
 *  For POWER9 (PowerISA 3.0B) or later use the Vector Count Trailing
 *  Zeros Word instruction <B>vctzw</B>. Otherwise use a sequence of
 *  pre ISA 3.0 VMX instructions.
 *  SIMDized count Trailing zeros inspired by:
 *
 *  Warren, Henry S. Jr and <I>Hacker's Delight</I>, 2nd Edition,
 *  Addison Wesley, 2013. Chapter 5 Counting Bits, Section 5-4.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 17-26 |   NA     |
 *
 *  @param vra 128-bit vector treated as 4 x 32-bit integer (words)
 *  elements.
 *  @return 128-bit vector with the Trailng Zeros count for each word
 *  element.
 */
static inline vui32_t
vec_ctzw_PWR7 (vui32_t vra)
{
  vui32_t r;
// For _ARCH_PWR7 and earlier. Generate 1's for the trailing zeros
// and 0's otherwise. Then count (popcnt) the 1's. _ARCH_PWR7 and
// earlier use the PVECLIB vec_popcntw_PWR7 implementation.
  const vui32_t mone = vec_splat_u32 (-1);
  vui32_t tzmask;
  // tzmask = (!vra & (vra - 1))
  tzmask = vec_andc (vec_add (vra, mone), vra);
  // return = vec_popcnt (!vra & (vra - 1))
  r = vec_popcntw_PWR7 (tzmask);
  return ((vui32_t) r);
}

/** \brief Vector Count Trailing Zeros Word for POWER8 and earlier.
 *
 *  Count the number of trailing '0' bits (0-32) within each word
 *  element of a 128-bit vector.
 *
 *  For POWER9 (PowerISA 3.0B) or later use the Vector Count Trailing
 *  Zeros Word instruction <B>vctzw</B>. Otherwise use a sequence of
 *  pre ISA 3.0 VMX instructions.
 *  SIMDized count Trailing zeros inspired by:
 *
 *  Warren, Henry S. Jr and <I>Hacker's Delight</I>, 2nd Edition,
 *  Addison Wesley, 2013. Chapter 5 Counting Bits, Section 5-4.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 17-26 |   NA     |
 *  |power8   |  6-8  | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as 4 x 32-bit integer (words)
 *  elements.
 *  @return 128-bit vector with the Trailng Zeros count for each word
 *  element.
 */
static inline vui32_t
vec_ctzw_PWR8 (vui32_t vra)
{
  vui32_t r;
// For _ARCH_PWR8 and earlier. Generate 1's for the trailing zeros
// and 0's otherwise. Then count (popcnt) the 1's. _ARCH_PWR8 uses
// the hardware vpopcntw instruction. _ARCH_PWR7 and earlier use the
// PVECLIB vec_popcntw_PWR7 implementation which runs ~13-20 cycles.
  const vui32_t mone = vec_splat_u32 (-1);
  vui32_t tzmask;
  // tzmask = (!vra & (vra - 1))
  tzmask = vec_andc (vec_add (vra, mone), vra);
  // return = vec_popcnt (!vra & (vra - 1))
  r = vec_popcntw_PWR8 (tzmask);
  return ((vui32_t) r);
}

/** \brief Vector Count Trailing Zeros Word for POWER9 and earlier.
 *
 *  Count the number of trailing '0' bits (0-32) within each word
 *  element of a 128-bit vector.
 *
 *  For POWER9 (PowerISA 3.0B) or later use the Vector Count Trailing
 *  Zeros Word instruction <B>vctzw</B>. Otherwise use a sequence of
 *  pre ISA 3.0 VMX instructions.
 *  SIMDized count Trailing zeros inspired by:
 *
 *  Warren, Henry S. Jr and <I>Hacker's Delight</I>, 2nd Edition,
 *  Addison Wesley, 2013. Chapter 5 Counting Bits, Section 5-4.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 17-26 |   NA     |
 *  |power8   |  6-8  | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as 4 x 32-bit integer (words)
 *  elements.
 *  @return 128-bit vector with the Trailng Zeros count for each word
 *  element.
 */
static inline vui32_t
vec_ctzw_PWR9 (vui32_t vra)
{
  vui32_t r;
#ifdef _ARCH_PWR9
#if defined (vec_cnttz) || defined (__clang__)
  r = vec_cnttz (vra);
#else
  __asm__(
      "vctzw %0,%1;"
      : "=v" (r)
      : "v" (vra)
      : );
#endif
#else
  r = vec_ctzw_PWR8 (vra);
#endif
  return ((vui32_t) r);
}

/** \brief Vector Count Trailing Zeros Doubleword for POWER7 and earlier.
 *
 *  Count the number of trailing '0' bits (0-64) within each doubleword
 *  element of a 128-bit vector.
 *
 *  For POWER9 (PowerISA 3.0B) or later use the Vector Count Trailing
 *  Zeros Doubleword instruction <B>vctzd</B>. Otherwise use a sequence of
 *  pre ISA 3.0 VMX instructions leveraging the PVECLIB popcntd operation.
 *  SIMDized count Trailing zeros inspired by:
 *
 *  Warren, Henry S. Jr and <I>Hacker's Delight</I>, 2nd Edition,
 *  Addison Wesley, 2013. Chapter 5 Counting Bits, Section 5-4.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8-10  |2/2 cycles|
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as 2 x 64-bit integer
 *  (doublewords) elements.
 *  @return 128-bit vector with the trailng zeros count for each
 *  doubleword element.
 */
static inline vui64_t
vec_ctzd_PWR7 (vui64_t vra)
{
  vui64_t result;
  /* vector ctz instructions were introduced in power9. For power8 and
   * earlier, use the pveclib vec_popcntd_PWR8 implementation.  */
  const vui64_t mone = (vui64_t) vec_splat_u32 (-1);
  vui64_t tzmask;

  // This allows quadword ctz to be a simple sum across of the dwords
  // tzmask = (!r64 & (r64 - 1))
  tzmask = vec_andc (vec_addudm_PWR7 (vra, mone), vra);
  result = vec_popcntd_PWR7 (tzmask);

  return (result);
}

/** \brief Vector Count Trailing Zeros Doubleword for POWER8 and earlier.
 *
 *  Count the number of trailing '0' bits (0-64) within each doubleword
 *  element of a 128-bit vector.
 *
 *  For POWER9 (PowerISA 3.0B) or later use the Vector Count Trailing
 *  Zeros Doubleword instruction <B>vctzd</B>. Otherwise use a sequence of
 *  pre ISA 3.0 VMX instructions leveraging the PVECLIB popcntd operation.
 *  SIMDized count Trailing zeros inspired by:
 *
 *  Warren, Henry S. Jr and <I>Hacker's Delight</I>, 2nd Edition,
 *  Addison Wesley, 2013. Chapter 5 Counting Bits, Section 5-4.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8-10  |2/2 cycles|
 *
 *  @param vra 128-bit vector treated as 2 x 64-bit integer
 *  (doublewords) elements.
 *  @return 128-bit vector with the trailng zeros count for each
 *  doubleword element.
 */
static inline vui64_t
vec_ctzd_PWR8 (vui64_t vra)
{
  vui64_t result;

#ifdef _ARCH_PWR8
  /* vector ctz instructions were introduced in power9. For power8 and
   * earlier, use the pveclib vec_popcntd_PWR8 implementation.  */
  const vui64_t mone = (vui64_t) vec_splat_u32 (-1);
  vui64_t tzmask;
  // tzmask = (!r64 & (r64 - 1))
  tzmask = vec_andc (vec_add (vra, mone), vra);
  result = vec_popcntd_PWR8 (tzmask);
#else
  result = vec_ctzd_PWR7 (vra);
#endif

  return (result);
}

/** \brief Vector Count Trailing Zeros Doubleword for POWER9 and earlier.
 *
 *  Count the number of trailing '0' bits (0-64) within each doubleword
 *  element of a 128-bit vector.
 *
 *  For POWER9 (PowerISA 3.0B) or later use the Vector Count Trailing
 *  Zeros Doubleword instruction <B>vctzd</B>. Otherwise use a sequence of
 *  pre ISA 3.0 VMX instructions leveraging the PVECLIB popcntd operation.
 *  SIMDized count Trailing zeros inspired by:
 *
 *  Warren, Henry S. Jr and <I>Hacker's Delight</I>, 2nd Edition,
 *  Addison Wesley, 2013. Chapter 5 Counting Bits, Section 5-4.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8-10  |2/2 cycles|
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as 2 x 64-bit integer
 *  (doublewords) elements.
 *  @return 128-bit vector with the trailng zeros count for each
 *  doubleword element.
 */
static inline vui64_t
vec_ctzd_PWR9 (vui64_t vra)
{
  vui64_t result;
#ifdef _ARCH_PWR9
#if defined (vec_cnttz) || defined (__clang__)
  result = vec_cnttz (vra);
#else
  __asm__(
      "vctzd %0,%1;"
      : "=v" (result)
      : "v" (vra)
      : );
#endif
#else
  result = vec_ctzd_PWR8 (vra);
#endif
  return (result);
}

/** \brief Vector Count Trailing Zeros Quadword for POWER7 and earlier.
 *
 *  Count trailing zeros for a vector __int128 and return the count in a
 *  vector suitable for use with vector shift (left|right) and vector
 *  shift (left|right) by octet instructions.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 32-41 | 1/cycle  |
 *  |power8   | 15-17 | 1/cycle  |
 *  |power9   | 13-16 | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @return a 128-bit vector with bits 121:127 containing the count of
 *  trailing zeros.
 */
static inline vui128_t
vec_ctzq_PWR7 (vui128_t vra)
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

/** \brief Vector Count Trailing Zeros Quadword for POWER7 and earlier.
 *
 *  Count trailing zeros for a vector __int128 and return the count in a
 *  vector suitable for use with vector shift (left|right) and vector
 *  shift (left|right) by octet instructions.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 14-16 | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @return a 128-bit vector with bits 121:127 containing the count of
 *  trailing zeros.
 */
static inline vui128_t
vec_ctzq_PWR8 (vui128_t vra)
{
#ifdef _ARCH_PWR8
  const vui128_t ones = (vui128_t) vec_splat_s32(-1);
  vui128_t tzmask;

  // tzmask = (!vra & (vra - 1))
  tzmask = (vui128_t) vec_andc ((vui64_t) vec_adduqm_PWR8 (vra, ones),
				(vui64_t) vra);
  // return = vec_popcnt (!vra & (vra - 1))
  return vec_popcntq_PWR8 (tzmask);
#else
  return vec_ctzq_PWR7 (vra);
#endif
}

/** \brief Vector Count Trailing Zeros Quadword for POWER9 and earlier.
 *
 *  Count trailing zeros for a vector __int128 and return the count in a
 *  vector suitable for use with vector shift (left|right) and vector
 *  shift (left|right) by octet instructions.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power9   | 13-16 | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @return a 128-bit vector with bits 121:127 containing the count of
 *  trailing zeros.
 */
static inline vui128_t
vec_ctzq_PWR9 (vui128_t vra)
{
  vui64_t result;

#ifdef _ARCH_PWR9
  const vui128_t ones = (vui128_t) vec_splat_s32(-1);
  vui128_t tzmask;

  // tzmask = (!vra & (vra - 1))
  tzmask = (vui128_t) vec_andc ((vui64_t) vec_adduqm_PWR8 (vra, ones),
				(vui64_t) vra);
  // return = vec_popcnt (!vra & (vra - 1))
  return vec_popcntq_PWR9 (tzmask);
#else
  result = (vui64_t) vec_ctzq_PWR8 (vra);
#endif

  return ((vui128_t) result);
}

/** \brief Vector doubleword paste.
 *  Concatenate the high doubleword of the 1st vector with the
 *  low double word of the 2nd vector.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   |   2   | 2/cycle  |
 *
 *  @param __VH a 128-bit vector as the source of the
 *  high order doubleword.
 *  @param __VL a 128-bit vector as the source of the
 *  low order doubleword.
 *  @return The combined 128-bit vector composed of the high order
 *  doubleword of __VH and the low order doubleword of __VL.
 */
static inline vui64_t
vec_pasted_PWR7 (vui64_t __VH, vui64_t __VL)
{
  // result[1] = __VH[1];
  // result[0] = __VL[0];
  vui64_t result;
  // BE only
  result = vec_xxpermdi (__VH, __VL, 1);

  return (result);
}

/** \brief Vector doubleword paste.
 *  Concatenate the high doubleword of the 1st vector with the
 *  low double word of the 2nd vector.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param __VH a 128-bit vector as the source of the
 *  high order doubleword.
 *  @param __VL a 128-bit vector as the source of the
 *  low order doubleword.
 *  @return The combined 128-bit vector composed of the high order
 *  doubleword of __VH and the low order doubleword of __VL.
 */
static inline vui64_t
vec_pasted_PWR8 (vui64_t __VH, vui64_t __VL)
{
  // result[1] = __VH[1];
  // result[0] = __VL[0];
  vui64_t result;
#if defined (_ARCH_PWR8)  && (__GNUC__ >= 6)
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  // Undo LE adjustment
  result = vec_xxpermdi (__VL, __VH, 1);
#else
  result = vec_xxpermdi (__VH, __VL, 1);
#endif
#else
  result = vec_pasted_PWR7 (__VH, __VL);
#endif
  return (result);
}

/** \brief Vector Population Count byte for POWER7 and earlier.
 *
 *  Count the number of '1' bits (0-8) within each byte element of
 *  a 128-bit vector.
 *
 *  For POWER8 (PowerISA 2.07B) or later use vec_popcntb()
 *  operation. Otherwise use this implementation requiring
 *  only simple Vector (VMX) instructions.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 6-13  |   NA     |
 *
 *  @param vra 128-bit vector treated as 16 x 8-bit integers (byte)
 *  elements.
 *  @return 128-bit vector with the population count for each byte
 *  element.
 */
static inline vui8_t
vec_popcntb_PWR7 (vui8_t vra)
{
  const vui8_t nibmask = vec_splat_u8 (15);
  const vui8_t nibshft = vec_splat_u8 (4);
  const vui8_t popc_perm = { 0, 1, 1, 2, 1, 2, 2, 3,
                             1, 2, 2, 3, 2, 3, 3, 4};
  vui8_t nib_h, nib_l;
  vui8_t rh, rl;

  nib_l = vec_and (vra, nibmask);
  nib_h = vec_sr  (vra, nibshft);
  rl = vec_perm (popc_perm, popc_perm, nib_l);
  rh = vec_perm (popc_perm, popc_perm, nib_h);
  return  vec_add (rh, rl);
}

/** \brief Vector Population Count halfword for POWER7 and earlier.
 *
 *  Count the number of '1' bits (0-16) within each byte element of
 *  a 128-bit vector.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the vec_popcnth()
 *  operation. Otherwise use this implementation requiring
 *  only simple Vector (VMX) instructions.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 13-22 |   NA     |
 *
 *  @param vra 128-bit vector treated as 8 x 16-bit integers (halfword)
 *  elements.
 *  @return 128-bit vector with the population count for each halfword
 *  element.
 */
static inline vui16_t
vec_popcnth_PWR7 (vui16_t vra)
{
  const vui8_t vone = vec_splat_u8 (1);
  vui16_t result, rl, rh;
  vui8_t bytepop;
  // Generate byte pop counts
  bytepop = vec_popcntb_PWR7 ((vui8_t) vra);
  // unpack and sum adjacent bytes into halfwords for pop count
  rl = vec_mulo (bytepop, vone);
  rh = vec_mule (bytepop, vone);
  result = vec_add (rh, rl);

  return result;
}

/** \brief Vector Population Count word for POWER7 and earlier.
 *
 *  Count the number of '1' bits (0-32) within each word element of
 *  a 128-bit vector.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the vec_popcntw()
 *  operation. Otherwise use this implementation requiring
 *  only simple Vector (VMX) instructions.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 13-20 |   NA     |
 *
 *  @param vra 128-bit vector treated as 4 x 32-bit integer (words)
 *  elements.
 *  @return 128-bit vector with the population count for each word
 *  element.
 */
static inline vui32_t
vec_popcntw_PWR7 (vui32_t vra)
{
  const vui32_t vzero = vec_splat_u32 (0);
  vui8_t bytepop;
  // Generate byte pop counts
  bytepop = vec_popcntb_PWR7 ((vui8_t) vra);
  // unpack and sum adjacent bytes into words for pop count
  return  vec_sum4s (bytepop, vzero);
}

/** \brief Vector Population Count word for POWER7/8 and earlier.
 *
 *  Count the number of '1' bits (0-32) within each word element of
 *  a 128-bit vector.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the Vector Population
 *  Count Word instruction. Otherwise use the pveclib vec_popcntb_PWR7
 *  to count each byte then sum across with Vector Sum across Quarter
 *  Unsigned Byte Saturate.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 13-20 |   NA     |
 *  |power8   |   2   | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as 4 x 32-bit integer (words)
 *  elements.
 *  @return 128-bit vector with the population count for each word
 *  element.
 */
static inline vui32_t
vec_popcntw_PWR8 (vui32_t vra)
{
  vui32_t r;
#ifdef _ARCH_PWR8
#if defined (vec_vpopcntw)
  r = vec_vpopcntw (vra);
#elif defined (__clang__)
  r = vec_popcnt (vra);
#else
  __asm__(
      "vpopcntw %0,%1;"
      : "=v" (r)
      : "v" (vra)
      : );
#endif
#else
  r = vec_popcntw_PWR7 (vra);
#endif
  return (r);
}

/** \brief Vector Population Count doubleword for POWER7 and earlier.
 *
 *  Count the number of '1' bits (0-64) within each doubleword element
 *  of a 128-bit vector.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the vec_popcntd()
 *  operation. Otherwise use this implementation
 *  requiring only simple Vector (VMX) instructions.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 20-27 |   NA     |
 *
 *  @param vra 128-bit vector treated as 2 x 64-bit integer (dwords)
 *  elements.
 *  @return 128-bit vector with the population count for each dword
 *  element.
 */
static inline vui64_t
vec_popcntd_PWR7 (vui64_t vra)
{
  const vui32_t vzero = vec_splat_u32 (0);
  vui8_t bytepop;
  vui32_t wordpop;
  vi32_t dwordpop;
  // Generate byte pop counts
  bytepop = vec_popcntb_PWR7 ((vui8_t) vra);
  // unpack and sum adjacent bytes into words for pop count
  wordpop = vec_sum4s (bytepop, vzero);
  // unpack and sum adjacent words into dwords for pop count
  dwordpop = vec_sum2s ((vi32_t) wordpop, (vi32_t) vzero);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  // undo endian for PWR8 testing only, work around!
  dwordpop = vec_sld (dwordpop, dwordpop, 12);
#endif
  return ((vui64_t) dwordpop);
}

/** \brief Vector Population Count doublewordfor POWER7 and earlier.
 *
 *  Count the number of '1' bits (0-64) within each doubleword element
 *  of a 128-bit vector.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   4   |2/2 cycles|
 *  |power9   |   3   | 2/cycle  |
 *
 *  For POWER8 (PowerISA 2.07B) or later use the Vector Population
 *  Count DoubleWord (<B>vpopcntd</B>) instruction. Otherwise use the
 *  pveclib vec_popcntd_PWR7.
 *
 *  @param vra 128-bit vector treated as 2 x 64-bit integer (dwords)
 *  elements.
 *  @return 128-bit vector with the population count for each dword
 *  element.
 */
static inline vui64_t
vec_popcntd_PWR8 (vui64_t vra)
{
  vui64_t r;
#ifdef _ARCH_PWR8
#if defined (vec_vpopcntd)
  r = vec_vpopcntd (vra);
#elif defined (__clang__)
  r = vec_popcnt (vra);
#else
  __asm__(
      "vpopcntd %0,%1;"
      : "=v" (r)
      : "v" (vra)
      : );
#endif
#else
  r = vec_popcntd_PWR7 (vra);
#endif
  return (r);
}

/** \brief Vector Population Count Quadword for POWER7 and earlier.
 *
 *  Count the number of '1' bits within a vector unsigned __int128
 *  and return the count (0-128) in a vector unsigned __int128.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the vec_popcntq()
 *  operation. Otherwise use this implementation
 *  requiring only simple Vector (VMX) instructions.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 20-27 |   NA     |
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @return a 128-bit vector with bits 121:127 containing the
 *  population count.
 */
static inline vui128_t
vec_popcntq_PWR7 (vui128_t vra)
{
  const vui32_t vzero = vec_splat_u32 (0);
  vui8_t bytepop;
  vui32_t wordpop;
  vi32_t qwordpop;
  // Generate byte pop counts
  bytepop = vec_popcntb_PWR7 ((vui8_t) vra);
  // unpack and sum adjacent bytes into words for pop count
  wordpop = vec_sum4s (bytepop, vzero);
  // unpack and sum adjacent words into quadword for pop count
  qwordpop = vec_sums ((vi32_t) wordpop, (vi32_t) vzero);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  // undo endian for PWR8 testing only, work around!
  qwordpop = vec_sld (qwordpop, qwordpop, 4);
#endif
  return ((vui128_t) qwordpop);
}

/** \brief Vector Population Count Quadword for POWER8 and earlier.
 *
 *  Count the number of '1' bits within a vector unsigned __int128
 *  and return the count (0-128) in a vector unsigned __int128.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 20-27 |   NA     |
 *  |power8   |  8-10 | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @return a 128-bit vector with bits 121:127 containing the
 *  population count.
 */
static inline vui128_t
vec_popcntq_PWR8 (vui128_t vra)
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

/** \brief Vector Population Count Quadword for POWER8 and earlier.
 *
 *  Count the number of '1' bits within a vector unsigned __int128
 *  and return the count (0-128) in a vector unsigned __int128.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  8-10 | 1/cycle  |
 *  |power9   |  9-12 | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @return a 128-bit vector with bits 121:127 containing the
 *  population count.
 */
static inline vui128_t
vec_popcntq_PWR9 (vui128_t vra)
{
  vui128_t result;

#ifdef _ARCH_PWR9
  /*
   * Use the Vector Population Count Doubleword instruction to get
   * the count for the left and right vector halves.  Then sum across
   * the left and right counts to get the final 128-bit vector count
   * (0-128).
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
  result = (vui128_t) vec_popcntq_PWR8 (vra);
#endif
  return ((vui128_t) result);
}

/** \brief Vector Rotate Left Doubleword Immediate for POWER7 and earlier.
 *
 *  Vector Rotate Left Doubleword 0-63 bits.
 *  Shift left each doubleword element [0-1], 0-63 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-63.
 *  Shift counts greater then 63 bits return vra.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   |  6-10 | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as 2 x unsigned long integers.
 *  @param shb shift amount in the range 0-63.
 *  @return Left shifted vector unsigned long.
 */
static inline vui64_t
vec_rldi_PWR7 (vui64_t vra, const unsigned int shb)
{
  vui64_t r = vra;

  /* constrain the dword shift amounts to 0-63.  */
  if (__builtin_constant_p (shb) && (shb < 64))
    {
      /*
       * POWER7 and earlier do not have vrld. So use the vector shift
       * left long/octet instructions. But these may shift bits from
       * element 1 in the low bits of element 0. So splat each DW into
       * a separate vectors then shift left long/octet both vectors by
       * the 6-bit shift count. Then merge high DW for the result.
       *
       * Also split the 6-bit shift count into octet (high 3-bits) and
       * bit (low 3-bits) to make it easier to generate.
       * Finally replace vec_slo() with vec_sld() which includes an
       * immediate octet shift count. So we don't have to generate
       * a separate octet shift count vector .
       */

      vui64_t hd, ld;
      vui8_t t1, t2;

      // Splat high/low DWs into two vectors for the rotate via shift
      hd = vec_xxpermdi (vra, vra, 0);
      ld = vec_xxpermdi (vra, vra, 3);
      // convert to vector char of this part
      t1 = (vui8_t) hd;
      t2 = (vui8_t) ld;
      if (shb > 7)
	{ // Shift is more the 7-bits
	  // Vector Rotate Left By Octet (shb/8).
	  t1 = vec_sld (t1, t1, (shb / 8));
	  t2 = vec_sld (t2, t2, (shb / 8));
	}
      if ((shb & 7) != 0)
	{ // sub-octet bit shift is required
	  // Vector Shift Left by (shb%8) bits
	  vui8_t lshift = vec_splat_u8(shb % 8);
	  t1 = vec_sll (t1, lshift);
	  t2 = vec_sll (t2, lshift);
	}
      // Convert back to vector long long and merge high
      r = vec_xxpermdi ((vui64_t) t1, (vui64_t) t2, 0);
    }
  return (r);
}

/** \brief Vector Rotate Left Doubleword Immediate for POWER7 and earlier.
 *
 *  Vector Rotate Left Doubleword 0-63 bits.
 *  Shift left each doubleword element [0-1], 0-63 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-63.
 *  Shift counts greater then 63 bits return vra.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   |  6-10 | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as 2 x unsigned long integers.
 *  @param shb shift amount in the range 0-63.
 *  @return Left shifted vector unsigned long.
 */
static inline vui64_t
vec_rldi_PWR8 (vui64_t vra, const unsigned int shb)
{
  vui64_t result = vra;

#if defined (_ARCH_PWR8)  && (__GNUC__ >= 6)
  /* constrain the dword shift amounts to 0-63.  */
  result = vec_vrld_PWR8 (vra, vec_splat6_u8 (shb % 64));
#else
  result = vec_rldi_PWR7 (vra, (shb%64));
#endif
  return (result);
}

/** \brief Vector Shift left Doubleword Immediate for POWER7 and earlier.
 *
 *  Shift left each doubleword element [0-1], 0-63 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-63.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 63 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   |  6-12 | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned long int.
 *  @param shb shift amount in the range 0-63.
 *  @return 128-bit vector unsigned long long int, shifted left shb bits.
 */
static inline vui64_t
vec_sldi_PWR7 (vui64_t vra, const unsigned int shb)
{
  vui64_t result = vra;

  if (__builtin_constant_p (shb) && (shb < 64))
    {
      /*
       * POWER7 and earlier do not have vsld. So use the vector shift
       * left long/octet instructions. But these may shift bits from
       * element 1 in the low bits of element 0. So generate a mask of
       * '1's, shifted left by the same shb and rotated into the
       * element 0 position.
       * Also split the 6-bit shift count into octet (high 3-bits) and
       * bit (low 3-bits) to make it easier to generate.
       * Finally replace vec_slo() with vec_sld() which includes an
       * immediate octet shift count. So we don't have to generate
       * a separate octet shift count vector but do need an 0's vector
       * to mimic vec_slo using vec_sld.
       */
      vui8_t sl_m = (vui8_t) vec_splat_u8(-1);
      vui8_t sl_a = ((vui8_t) vra);
      if (shb > 7)
	{ // Shift is more the 7-bits
	  vui8_t sl_0 = (vui8_t) vec_splat_u8(0);
	  // Vector Shift Left By Octet (shb/8). Shifting in 0's.
	  sl_m = vec_sld (sl_m, sl_0, (shb / 8));
	  sl_a = vec_sld ((vui8_t) vra, sl_0, (shb / 8));
	}
      if ((shb & 7) != 0)
	{ // sub-octet bit shift is required
	  // Vector Shift Left by (shb%8) bits
	  vui8_t lshift = vec_splat_u8(shb % 8);
	  sl_m = vec_sll (sl_m, lshift);
	  sl_a = vec_sll (sl_a, lshift);
	}
      /* Rotate mask and clear low order bits of Element 0. */
      sl_m = vec_sld (sl_m, sl_m, 8);
      result = (vui64_t) vec_and (sl_a, sl_m);
    }

  return (vui64_t) result;
}

/** \brief Vector Shift Left Doubleword Immediate for POWER7 and earlier.
 *
 *  Vector Rotate Left Doubleword 0-63 bits.
 *  Shift left each doubleword element [0-1], 0-63 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-63.
 *  Shift counts greater then 63 bits return vra.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   |  6-10 | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as 2 x unsigned long integers.
 *  @param shb shift amount in the range 0-63.
 *  @return Left shifted vector unsigned long.
 */
static inline vui64_t
vec_sldi_PWR8 (vui64_t vra, const unsigned int shb)
{
  vui64_t result = vra;

  if (shb < 64)
    {
#if defined (_ARCH_PWR8)  && (__GNUC__ >= 6)
      /* constrain the dword shift amounts to 0-63.  */
      result = vec_vsld_PWR8 (vra, vec_splat6_u8 (shb % 64));
#else
      result = vec_sldi_PWR7 (vra, (shb%64));
#endif
    }
  else
    { /* shifts greater then 63 bits return zeros.  */
      result = vec_xor ((vui64_t) vra, (vui64_t) vra);
    }
  return (result);
}

/** \brief Vector Shift Right Algebraic Doubleword by Byte for POWER7 and earlier.
 *
 *  Vector Shift Right Algebraic Doubleword 0-63 bits.
 *  The shift amount is from bits 58-63 and 122-127 of vrb.
 *
 *  \note The PowerISA only requires the low order 6-bits of each
 *  doubleword as the shift count. So there is no reason to force
 *  the shift count to conform to be a unsigned long long. Also it is
 *  much easier to splat byte constants than doubleword constants.
 *
 *  @param vra a 128-bit vector treated as 2 x unsigned long integers.
 *  @param vrb shift amount in bits 58:63 and 122:127.
 *  @return Right shifted vector unsigned long.
 */
static inline vi64_t
vec_vsrad_PWR7 (vi64_t vra, vui8_t vrb)
{
  vi64_t result;
  vui8_t  vsh_h, vsh_l;
  vui8_t  vr_h, vr_l;
  vi32_t exsa;
  vui64_t exsah, exsal;
  vui64_t shft_mask = CONST_VINT128_DW(63, 63);

  /* Need to extend each signed long int to __int128. So the unsigned
   * (128-bit) shift right behaves as a arithmetic (64-bit) shift.  */
  vi32_t vs0 = vec_splat_s32(0);
  exsa = (vi32_t) vec_cmpgt (vs0, (vi32_t) vra);
  exsah = (vui64_t) vec_vmrghw (exsa, exsa);
  exsal = (vui64_t) vec_vmrglw (exsa, exsa);
  /* constrain the dword shift amounts to 0-63.  */
  vsh_l = vec_and ((vui8_t) vrb, (vui8_t) shft_mask);
  /* The vsr instruction only works correctly if the bit shift
   * value is splatted to each byte of the vector.  */
  vsh_h = vec_splat (vsh_l, VEC_BYTE_L_DWH);
  vsh_l = vec_splat (vsh_l, VEC_BYTE_L_DWL);
  /* Merge the extended sign with high dword.  */
  exsah = vec_xxpermdi (exsah, (vui64_t) vra, 0);
  /* Shift the high dword by vsh_h.  */
  vr_h = vec_vsro ((vui8_t) exsah,  vsh_h);
  vr_h = vec_vsr  (vr_h, vsh_h);
  /* Merge the extended sign with high dword.  */
  //exsal = vec_xxpermdi (exsal, (vui64_t) vra, 1);
  exsal = vec_pasted_PWR7 (exsal, (vui64_t) vra);
  /* Shift the low dword by vsh_l.  */
  vr_l = vec_vsro ((vui8_t) exsal, vsh_l);
  vr_l = vec_vsr  (vr_l, vsh_l);
  /* Merge the dwords after shift.  */
  result = (vi64_t) vec_xxpermdi ((vui64_t) vr_h, (vui64_t) vr_l, 3);

  return ((vi64_t) result);
}

/** \brief Vector Shift Right Algebraic Doubleword by Byte for POWER8 and earlier.
 *
 *  Vector Shift Right Algebraic Doubleword 0-63 bits.
 *  The shift amount is from bits 58-63 and 122-127 of vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   2   | 2/cycle  |
 *  |power10  |  1-3  | 4/cycle  |
 *
 *  \note The PowerISA only requires the low order 6-bits of each
 *  doubleword as the shift count. So there is no reason to force
 *  the shift count to conform to be a unsigned long long. Also it is
 *  much easier to splat byte constants than doubleword constants.
 *
 *  @param vra a 128-bit vector treated as 2 x unsigned long integers.
 *  @param vrb shift amount in bits 58:63 and 122:127.
 *  @return Right shifted vector unsigned long.
 */
static inline vi64_t
vec_vsrad_PWR8 (vi64_t vra, vui8_t vrb)
{
  vi64_t result;

#ifdef _ARCH_PWR8
#ifdef __clang__bad
// clang8/9 has code gen bug here, disabled for now
  result = vec_sra (vra, (vui64_t) vrb);
#else
  __asm__(
      "vsrad %0,%1,%2;"
      : "=v" (result)
      : "v" (vra),
      "v" (vrb)
      : );
#endif
#else
  result = vec_vsrad_PWR7 (vra, vrb);
#endif
  return ((vi64_t) result);
}

/** \brief Vector Shift Right Algebraic Doubleword Immediate for POWER7 and earlier.
 *
 *  Vector Shift Right Algebraic Doubleword 0-63 bits.
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-63.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 63 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 12-18 | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as 2 x unsigned long integers.
 *  @param shb shift amount in the range 0-63.
 *  @return Right shifted vector unsigned long.
 */
static inline vi64_t
vec_sradi_PWR7 (vi64_t vra, const unsigned int shb)
{
  vi64_t result = vra;

  if (__builtin_constant_p (shb) && (shb < 64))
    {
      /*
       * POWER7 and earlier do not have vsrad. So use the vector shift
       * left long/octet instructions. But these may shift bits from
       * element 1 in the low bits of element 0. Also need to propagate
       * the sign-bit for the right shift.
       * So split the high/low DWs into separate vectors with the sign
       * bit extend into he high DW.
       * Also split the 6-bit shift count into octet (high 3-bits) and
       * bit (low 3-bits) to make it easier to generate.
       * Finally replace vec_slo() with vec_sld() which includes an
       * immediate octet shift count. So we don't have to generate
       * a separate octet shift count vector but use the rotate form
       * of vec_sld to mimic vec_rlo.
       */
      vui8_t vr_h, vr_l;
      vi32_t exsa;
      vui64_t exsah, exsal;

      /* Need to extend each signed long int to __int128. So the unsigned
       * (128-bit) shift right behaves as a arithmetic (64-bit) shift.  */
      vi32_t vs0 = vec_splat_s32(0);
      exsa = (vi32_t) vec_cmpgt (vs0, (vi32_t) vra);
      exsah = (vui64_t) vec_vmrghw (exsa, exsa);
      exsal = (vui64_t) vec_vmrglw (exsa, exsa);
      /* Merge the extended sign with high dword.  */
      exsah = vec_xxpermdi (exsah, (vui64_t) vra, 0);
      /* Merge the extended sign with high dword.  */
      // exsal = vec_xxpermdi (exsal, (vui64_t) vra, 1);
      exsal = vec_pasted_PWR7 (exsal, (vui64_t) vra);

      vr_h = (vui8_t) exsah;
      vr_l = (vui8_t) exsal;

      if (shb > 7)
	{ // Shift is more the 7-bits
	  // Vector rotate  By Octet (shb/8). Shifting in exsa's.
	  vr_h = vec_sld ((vui8_t) exsah, (vui8_t) exsah, (16 - (shb / 8)));
	  vr_l = vec_sld ((vui8_t) exsal, (vui8_t) exsal, (16 - (shb / 8)));
	}
      if ((shb & 7) != 0)
	{ // sub-octet bit shift is required
	  // Vector Shift Right by (shb%8) bits
	  vui8_t rshift = vec_splat_u8(shb % 8);
	  vr_h = vec_srl (vr_h, rshift);
	  vr_l = vec_srl (vr_l, rshift);
	}
      /* Merge the dwords after shift.  */
      result = (vi64_t) vec_xxpermdi ((vui64_t) vr_h, (vui64_t) vr_l, 3);
    }

  return ((vi64_t) result);
}

/** \brief Vector Shift Right Algebraic Doubleword Immediate for POWER8 and earlier.
 *
 *  Vector Shift Right Algebraic Doubleword 0-63 bits.
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-63.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 63 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power    | 4 - 8 | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as 2 x unsigned long integers.
 *  @param shb shift amount in the range 0-63.
 *  @return Right shifted vector unsigned long.
 */
static inline vi64_t
vec_sradi_PWR8 (vi64_t vra, const unsigned int shb)
{
  vi64_t result = vra;

  if (shb < 64)
    {
#if defined (_ARCH_PWR8)  && (__GNUC__ >= 6)
      /* constrain the dword shift amounts to 0-63.  */
      result = vec_vsrad_PWR8 (vra, vec_splat6_u8 (shb));
#else
      result = vec_sradi_PWR7 (vra, (shb));
#endif
    }
  else
    { /* shifts greater then 63 bits return sign mask.  */
      result = vec_vsrad_PWR8 (vra, vec_splat6_u8 (63));
    }
  return (result);
}

/** \brief Vector Shift Right Doubleword Immediate for POWER7 and earlier.
 *
 *  Shift Right each doubleword element [0-1], 0-63 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-63.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 63 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   |  6-12 | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned long int.
 *  @param shb shift amount in the range 0-63.
 *  @return 128-bit vector unsigned long long int, shifted left shb bits.
 */
static inline vui64_t
vec_srdi_PWR7 (vui64_t vra, const unsigned int shb)
{
  vui64_t result = vra;

  if (__builtin_constant_p (shb) && (shb < 64))
    {
      /*
       * POWER7 and earlier do not have vsrd. So use the vector shift
       * right long/octet instructions. But these may shift bits from
       * element 1 in the low bits of element 0. So generate a mask of
       * '1's, shifted right by the same shb and rotated into the
       * element 0 position.
       * Also split the 6-bit shift count into octet (high 3-bits) and
       * bit (low 3-bits) to make it easier to generate.
       * Finally replace vec_slo() with vec_sld() which includes an
       * immediate octet shift count. So we don't have to generate
       * a separate octet shift count vector but do need an 0's vector
       * to mimic vec_slo using vec_sld.
       */
      vui8_t sr_m = (vui8_t) vec_splat_u8(-1);
      vui8_t sr_a = ((vui8_t) vra);
      if (shb > 7)
	{ // Shift is more the 7-bits
	  vui8_t sr_0 = (vui8_t) vec_splat_u8(0);
	  // Vector Shift Left By Octet (shb/8). Shifting in 0's.
	  sr_m = vec_sld (sr_0, sr_m, (16-(shb / 8)));
	  sr_a = vec_sld (sr_0, (vui8_t) vra, (16-(shb / 8)));
	}
      if ((shb & 7) != 0)
	{ // sub-octet bit shift is required
	  // Vector Shift Left by (shb%8) bits
	  vui8_t rshift = vec_splat_u8(shb % 8);
	  sr_m = vec_srl (sr_m, rshift);
	  sr_a = vec_srl (sr_a, rshift);
	}
      /* Rotate mask and clear low order bits of Element 0. */
      sr_m = vec_sld (sr_m, sr_m, 8);
      result = (vui64_t) vec_and (sr_a, sr_m);
    }

  return (vui64_t) result;
}

/** \brief Vector Shift Right Doubleword Immediate for POWER7 and earlier.
 *
 *  Vector Shift Right Doubleword 0-63 bits.
 *  Shift left each doubleword element [0-1], 0-63 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-63.
 *  Shift counts greater then 63 bits return vra.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   |  6-10 | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as 2 x unsigned long long integers.
 *  @param shb shift amount in the range 0-63.
 *  @return Right shifted vector unsigned long.
 */
static inline vui64_t
vec_srdi_PWR8 (vui64_t vra, const unsigned int shb)
{
  vui64_t result = vra;

  if (shb < 64)
    {
#if defined (_ARCH_PWR8)  && (__GNUC__ >= 6)
      /* constrain the dword shift amounts to 0-63.  */
      result = vec_vsrd_PWR8 (vra, vec_splat6_u8 (shb));
#else
      result = vec_srdi_PWR7 (vra, (shb));
#endif
    }
  else
    { /* shifts greater then 63 bits return zeros.  */
      result = vec_xor ((vui64_t) vra, (vui64_t) vra);
    }
  return (result);
}

/** \brief Vector Shift Left Double Quadword by Bit Immediate.
 *
 *  Return a vector __int128 that is bits shb:shb+127
 *  from the (256-bit) double quadword (vra || vrb).
 *  The shift amount is constant immediate value in the range 0-7.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   8   | 1 cycle  |
 *  |power9   |  11   | 1/cycle  |
 *
 *  @param vra upper 128-bits of the 256-bit double quadword vector.
 *  @param vrb lower 128-bits of the 256-bit double quadword vector.
 *  @param shb Shift amount in the range 0-7.
 *  @return 128-bits from bits shb:shb+127.
 */
static inline vui8_t
vec_sldbi_PWR9 (vui8_t vra, vui8_t vrb, const unsigned int shb)
{
  vui8_t result;

  if (__builtin_constant_p (shb) && (shb < 8))
    {
      /* For Power7/8/9 the quadword bit shift left/right instructions
       * only handle 128-bits.
       * So shift vra and vrb separately then combine those into
       * a single 128-bit result.
       */
     if (shb > 0)
	{
	  const vui8_t vshl = vec_splat_u8 (shb);
	  const vui8_t vshr = vec_splat_u8 (8 - shb);
	  const vui8_t zero = vec_splat_u8 (0);
	  vui8_t lowbits, highbits;

	  /* Shift left double (vra || 'zero') by 15 octet  to isolate
	   * the high order byte of vrb in to the low 8-bits. Then right
	   * shift this (8-shb) bits. This provides (128-shb) bits of
	   * leading '0's. */
	  lowbits = vec_sld (zero, (vui8_t) vrb, 1);
	  lowbits = vec_vsrb (lowbits, vshr);
	  /* Left shift the quadword vra shifting in shb '0' bits.  */
	  highbits = vec_sll ((vui8_t) vra, vshl);
	  /* Combine left shifted bits from vra, vrb.  */
	  result = vec_or (highbits, lowbits);
	}
     else
	result = vra;
    }
  else
    {
      result = vra;
    }

  return ((vui8_t) result);
}

/** \brief Vector Shift Left Double Quadword by Bit Immediate.
 *
 *  Return a vector __int128 that is bits shb:shb+127
 *  from the (256-bit) double quadword (vra || vrb).
 *  The shift amount is constant immediate value in the range 0-7.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   8   | 1 cycle  |
 *  |power9   |  11   | 1/cycle  |
 *  |power10  |  3-4  | 4/cycle  |
 *
 *  @param vra upper 128-bits of the 256-bit double quadword vector.
 *  @param vrb lower 128-bits of the 256-bit double quadword vector.
 *  @param shb Shift amount in the range 0-7.
 *  @return 128-bits from bits shb:shb+127.
 */
static inline vui8_t
vec_sldbi_PWR10 (vui8_t vra, vui8_t vrb, const unsigned int shb)
{
  vui8_t result;

  if (__builtin_constant_p (shb) && (shb < 8))
    {
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
#if (__GNUC__ >= 12)
      // GCC PR 111645
      result = (vui8_t) vec_sldb ((vui8_t) vra, (vui8_t) vrb, shb);
#else
      __asm__(
	  "vsldbi %0,%1,%2,%3;\n"
	  : "=v" (result)
	  : "v" (vra), "v" (vrb), "K" (shb)
	  : );
#endif
#else
      result = vec_sldbi_PWR9 (vra, vrb, shb);
#endif
    }
  else
    {
      result = vra;
    }

  return ((vui8_t) result);
}

/** \brief Vector Shift Right Double Quadword by Bit Immediate for POWER9 and earlier.
 *
 *  Return a vector __int128 that is bits 128-shb:255-shb
 *  from the (256-bit) double quadword (vra || vrb).
 *  The shift amount is constant immediate value in the range 0-7.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   8   | 1 cycle  |
 *  |power9   |  11   | 1/cycle  |
 *  |power10  |  3-4  | 4/cycle  |
 *
 *  @param vra upper 128-bits of the 256-bit double quadword vector.
 *  @param vrb lower 128-bits of the 256-bit double quadword vector.
 *  @param shb Shift amount in the range 0-7.
 *  @return 128-bits from bits 128-shb:255-shb.
 */
static inline vui8_t
vec_srdbi_PWR9 (vui8_t vra, vui8_t vrb, const unsigned int shb)
{
  vui8_t result;

  if (__builtin_constant_p (shb) && (shb < 8))
    {
      /* For Power7/8/9 the quadword bit shift left/right instructions
       * only handle 128-bits.
       * So shift vra and vrb separately then combine those into
       * a single 128-bit result.
       */
      if (shb > 0)
	{
	  const vui8_t vshl = vec_splat_u8 (8 - shb);
	  const vui8_t vshr = vec_splat_u8 (shb);
	  const vui8_t zero = vec_splat_u8 (0);
	  vui8_t lowbits, highbits;

	  /* Shift left double (vra || 'zero') by 15 octet to isolate
	   * the low order byte of vra in to the high 8-bits. Then left
	   * shift this (8-shb) bits. This provides (128-shb) bits of
	   * trailing '0's. */
	  highbits = vec_sld ((vui8_t) vra, zero, 15);
	  highbits = vec_vslb (highbits, vshl);
	  /* right shift the quadword vrb shifting in shb '0' bits.  */
	  lowbits = vec_srl ((vui8_t) vrb, vshr);
	  /* Combine right shifted bits from vra, vrb.  */
	  result = vec_or (highbits, lowbits);
	}
      else
	result = vrb;
    }
  else
    {
      result = vra;
    }

  return ((vui8_t) result);
}

/** \brief Vector Shift Right Double Quadword by Bit Immediate.
 *
 *  Return a vector __int128 that is bits 128-shb:255-shb
 *  from the (256-bit) double quadword (vra || vrb).
 *  The shift amount is constant immediate value in the range 0-7.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   8   | 1 cycle  |
 *  |power9   |  11   | 1/cycle  |
 *  |power10  |  3-4  | 4/cycle  |
 *
 *  @param vra upper 128-bits of the 256-bit double quadword vector.
 *  @param vrb lower 128-bits of the 256-bit double quadword vector.
 *  @param shb Shift amount in the range 0-7.
 *  @return 128-bits from bits 128-shb:255-shb.
 */
static inline vui8_t
vec_srdbi_PWR10 (vui8_t vra, vui8_t vrb, const unsigned int shb)
{
  vui8_t result;

  if (__builtin_constant_p (shb) && (shb < 8))
    {
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
#if (__GNUC__ >= 12)
      // GCC PR 111645
      result = vec_srdb (vra, vrb, shb);
#else
      __asm__(
	  "vsrdbi %0,%1,%2,%3;\n"
	  : "=v" (result)
	  : "v" (vra), "v" (vrb), "K" (shb)
	  : );
#endif
#else
      result = vec_srdbi_PWR9 (vra, vrb, shb);
#endif
    }
  else
    {
#if defined (__clang__) && (__clang_major__ < 6)
      // A workaround for a constant propagation bug in clang-5
      if (shb == 0)
        result = vrb;
      else
#endif
    }

  return ((vui8_t) result);
}

/** \brief Vector Shift Left Double Quadword.
 *
 *  Vector Shift Left double Quadword 0-127 bits.
 *  Return a vector __int128 that is the left most 128-bits after
 *  shifting left 0-127-bits of the 256-bit double vector
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
static inline vui8_t
vec_sldq_PWR7 (vui8_t vrw, vui8_t vrx, vui8_t vrb)
{
  vui8_t result, vt1, vt2, vt3, vbs;
  const vui8_t vzero = vec_splat_u8 (0);

  vt1 = vec_slo (vrw, vrb);
  /* The vsl/vsr instruction only works correctly if the bit shift
     value is splatted to each byte of the vector.  */
  vbs = vec_splat ((vui8_t) vrb, VEC_BYTE_L);
  vt1 = vec_sll (vt1, vbs);
  vt3 = vec_sub (vzero, vbs);
  vt2 = vec_sro ((vui8_t) vrx, vt3);
  vt2 = vec_srl (vt2, vt3);
  result = vec_or (vt1, vt2);

  return result;
}

/** \brief Vector Rotate left Quadword Immediate for POWER9 and earlier.
 *
 *  Shift left each quadword element [0], 0-127 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-127.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 63 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   |  4-6  | 1/cycle  |
 *  |power8   |  4-6  | 1/cycle  |
 *  |power9   |  6-9  | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned __int128.
 *  @param shb shift amount in the range 0-127.
 *  @return 128-bit vector unsigned __int128, shifted left shb bits.
 */
static inline vui128_t
vec_rlqi_PWR8 (vui128_t vra, const unsigned int shb)
{
  vui128_t result = vra;

  if (__builtin_constant_p (shb) && (shb < 128))
    {
      /*
       * POWER8 and earlier has splat immediate with a 5-bit SIM.
       * Hard to make a 7-bit shift count (0-127). Also there is no
       * Rotate long vector (immediate or otherwise.
       * We do have vector shift left double octet immediate which
       * simulates a rotate by doubling vra. But no vec_rll() or
       * vector shift left double bit immediate equivalents.
       *
       * So split the 7-bit shift count into octet (high 4-bits) and
       * bit (low 3-bits) to make it easier to generate. Then use
       * with vec_sld() which includes an immediate octet shift count
       * to simulate vec_rlo(). Then use the vec_sldbi_PWR9 operation
       * to simulate vec_rll().
       */
      vui8_t sl_a = ((vui8_t) vra);
      if (shb > 7)
	{ // Shift is more the 7-bits
	  // Vector Shift Left By Octet (shb/8). Shifting in 0's.
	  sl_a = vec_sld (sl_a, sl_a, (shb / 8));
	}
      if ((shb & 7) != 0)
	{ // sub-octet bit shift is required
	  // Vector Shift Left by (shb%8) bits
	  sl_a = vec_sldbi_PWR9 (sl_a, sl_a, (shb % 8));
	}
      result = (vui128_t) sl_a;
    }

  return (vui128_t) result;
}

/** \brief Vector Rotate left Quadword Immediate for POWER9 and earlier.
 *
 *  Shift left each quadword element [0], 0-127 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-127.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 63 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   |  2-10 | 1/cycle  |
 *  |power8   |  2-10 | 1/cycle  |
 *  |power9   |  3-15 | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned __int128.
 *  @param shb shift amount in the range 0-127.
 *  @return 128-bit vector unsigned __int128, shifted left shb bits.
 */
static inline vui128_t
vec_rlqi_PWR9 (vui128_t vra, const unsigned int shb)
{
  vui128_t result = vra;

#if defined (_ARCH_PWR9)
  if (__builtin_constant_p (shb) && (shb < 128))
    {
      /*
       * POWER9 and earlier has no Rotate long/octet vector
       * (immediate or otherwise).
       * We do have vector shift left double octet immediate which
       * simulates a rotate by doubling vra. But no vec_rll() or
       * vector shift left double bit immediate equivalents.
       *
       * So split the 7-bit shift count into octet (high 4-bits) and
       * bit (low 3-bits) to make it easier to generate. Then use
       * with vec_sld() which includes an immediate octet shift count
       * to simulate vec_rlo(). Then use the vec_sldbi_PWR9 operation
       * to simulate vec_rll().
       */
      vui8_t sl_a = ((vui8_t) vra);
      if (shb > 7)
	{ // Shift is more the 7-bits
	  // Vector Rotate Left By Octet (shb/8). Shifting in 0's.
	  sl_a = vec_sld (sl_a, sl_a, (shb / 8));
	}
      if ((shb & 7) != 0)
	{ // sub-octet bit shift is required
	  // Vector Rotate Left by (shb%8) bits
	  sl_a = vec_sldbi_PWR9 (sl_a, sl_a, (shb % 8));
	}
      result = (vui128_t) sl_a;
    }
#else
  result =  vec_rlqi_PWR8 (vra, shb);
#endif

  return (vui128_t) result;
}

/** \brief Vector Rotate left Quadword Immediate for POWER10 and earlier.
 *
 *  Shift left each quadword element [0], 0-127 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-127.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 63 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   |  2-10 | 1/cycle  |
 *  |power8   |  2-10 | 1/cycle  |
 *  |power9   |  3-15 | 1/cycle  |
 *  |power10  |  3-8  | 4/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned __int128.
 *  @param shb shift amount in the range 0-127.
 *  @return 128-bit vector unsigned __int128, rotated left shb bits.
 */
static inline vui128_t
vec_rlqi_PWR10 (vui128_t vra, const unsigned int shb)
{
  vui128_t result = vra;

#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  if (__builtin_constant_p (shb) && (shb < 128))
    {
      /*
       * POWER10 has xxspltib and vrlq. But the Intrinsic Reference
       * insists the shift count should be __int128. Current
       * compilers will not reliably generate xxspltib and the only
       * way to use a vector char as shift count requires inline asm.
       * POWER10 also has vsldbi which nicely works with vsldoi.
       * Also vrlq is cracked into 2 IOPs so the timing is the same
       * as the vsldoi/vsldbi combination. We must choose wisely.
       *
       * So split the 7-bit shift count into octet (high 4-bits) and
       * bit (low 3-bits) to make it easier to generate. Then use
       * with vec_sld() which includes an immediate octet shift count
       * to simulate vec_rlo(). Then use the vec_sldbi_PWR10 operation
       * (vsldbi instruction) to simulate vec_rll().
       */
      vui8_t sl_a = ((vui8_t) vra);
      if (shb > 7)
	{ // Shift is more the 7-bits
	  // Vector Shift Left By Octet (shb/8). Shifting in 0's.
	  sl_a = vec_sld (sl_a, sl_a, (shb / 8));
	}
      if ((shb & 7) != 0)
	{ // sub-octet bit shift is required
	  // Vector Shift Left by (shb%8) bits
	  sl_a = vec_sldbi_PWR10 (sl_a, sl_a, (shb % 8));
	}
      result = (vui128_t) sl_a;
    }
#else
  result =  vec_rlqi_PWR9 (vra, shb);
#endif

  return (vui128_t) result;
}

/** \brief Vector Rotate Left Quadword by Byte.
 *
 *  Vector Rotate Left Quadword 0-127 bits.
 *  The 7-bit shift count is splatted across the 16-bytes of vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10    | 1 cycle  |
 *  |power9   | 14    | 1/cycle  |
 *  |power10  |  6-8  | 4/cycle  |
 *
 *  \note The PowerISA only requires the low order 7-bits of each
 *  quadword as the shift count. So there is no reason to force
 *  the shift count to conform to be a unsigned __int128. Also it is
 *  much easier to splat byte constants than quadword constants.
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @param vrb Shift count in bits 1:7, splatted across bytes 0-15.
 *  @return Left rotated 128-bit vector.
 */
static inline vui128_t
vec_rlq_PWR9 (vui128_t vra, vui8_t vrb)
{
  return vec_vrlq_PWR9 (vra, vrb);
}

/** \brief Vector Rotate Left Quadword by Byte.
 *
 *  Vector Rotate Left Quadword 0-127 bits.
 *  The 7-bit shift count is splatted across the 16-bytes of vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10    | 1 cycle  |
 *  |power9   | 14    | 1/cycle  |
 *  |power10  |  6-8  | 4/cycle  |
 *
 *  \note The PowerISA only requires the low order 7-bits of each
 *  quadword as the shift count. So there is no reason to force
 *  the shift count to conform to be a unsigned __int128. Also it is
 *  much easier to splat byte constants than quadword constants.
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @param vrb Shift count in bits 1:7, splatted across bytes 0-15.
 *  @return Left rotated 128-bit vector.
 */
static inline vui128_t
vec_rlq_PWR10 (vui128_t vra, vui8_t vrb)
{
  vui128_t result;

#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
#ifdef __clang__
  result = vec_rl (vra, (vui128_t) vrb);
#else
  __asm__(
      "vrlq %0,%1,%2;\n"
      : "=v" (result)
      : "v" (vra), "v" (vrb)
      : );
#endif
#else
  result = vec_rlq_PWR9 (vra, vrb);
#endif
  return ((vui128_t) result);
}

/** \brief Vector Shift left Quadword Immediate for POWER9 and earlier.
 *
 *  Shift left each quadword element [0], 0-127 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-127.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 63 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   |  4-6  | 1/cycle  |
 *  |power8   |  4-6  | 1/cycle  |
 *  |power9   |  6-9  | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned long int.
 *  @param shb shift amount in the range 0-63.
 *  @return 128-bit vector unsigned long long int, shifted left shb bits.
 */
static inline vui128_t
vec_slqi_PWR8 (vui128_t vra, const unsigned int shb)
{
  vui128_t result = vra;

  if (__builtin_constant_p (shb) && (shb < 128))
    {
      /*
       * POWER8 and earlier has splat immediate with a 5-bit SIM.
       * Hard to make a 7-bit shift count (0-127).
       * So split the 7-bit shift count into octet (high 4-bits) and
       * bit (low 3-bits) to make it easier to generate.
       * Then replace vec_slo() with vec_sld() which includes an
       * immediate octet shift count. So we don't have to generate
       * a separate octet shift count vector but do need an 0's vector
       * to mimic vec_slo using vec_sld.
       * If the low 3-bits of the shift count != 0 then splat immediate
       * the bit shift count and use vec_sll().
       */
      vui8_t sl_a = ((vui8_t) vra);
      if (shb > 7)
	{ // Shift is more the 7-bits
	  vui8_t sl_0 = (vui8_t) vec_splat_u8(0);
	  // Vector Shift Left By Octet (shb/8). Shifting in 0's.
	  sl_a = vec_sld ((vui8_t) vra, sl_0, (shb / 8));
	}
      if ((shb & 7) != 0)
	{ // sub-octet bit shift is required
	  // Vector Shift Left by (shb%8) bits
	  vui8_t lshift = vec_splat_u8(shb % 8);
	  sl_a = vec_sll (sl_a, lshift);
	}
      result = (vui128_t) sl_a;
    }

  return (vui128_t) result;
}

/** \brief Vector Shift left Quadword Immediate for POWER9 and earlier.
 *
 *  Shift left each quadword element [0], 0-127 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-127.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 63 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   |  4-6  | 1/cycle  |
 *  |power8   |  4-6  | 1/cycle  |
 *  |power9   |  6-9  | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned long int.
 *  @param shb shift amount in the range 0-63.
 *  @return 128-bit vector unsigned long long int, shifted left shb bits.
 */
static inline vui128_t
vec_slqi_PWR9 (vui128_t vra, const unsigned int shb)
{
  vui128_t result = vra;

#if defined (_ARCH_PWR9)
  if (__builtin_constant_p (shb) && (shb < 128))
    {
      /*
       * POWER9 has xxspltib and vslo/vsl which together perform a
       * quadword shift left. POWER9 also has vsldoi which
       * includes an immediate (octet) shift count.
       * But the rotate (vsldoi) implementation of slq requires
       * the quadword zero extend of vra (xxspltib 0). So using vslo/vsl
       * might pay off if we can generate the 7 bit shift count in one
       * xxspltib instruction.
       *
       * But if only vsldoi ((shb % 8) == 0) or vsl (shb < 8) are
       * needed then xxspltib shb is not needed and can be replaced by
       * xxspltib 0. We must choose wisely.
       */
      if ((shb > 7) && ((shb % 8) != 0))
	{
          result = vec_vslq_PWR9 (vra, vec_splats ((unsigned char) shb));
	}
      else
	{
	  vui8_t sl_a = ((vui8_t) vra);
	  vui8_t lshift = vec_splats ((unsigned char)shb);
	  if (shb > 7)
	    {
	      // Shift is more the 7-bits
	      vui8_t sl_0 = vec_splat_u8(0);
	      // Vector Shift Left By Octet (shb/8). Shifting in 0's.
	      sl_a = vec_sld ((vui8_t) vra, sl_0, (shb / 8));
	    }
	  if ((shb & 7) != 0)
	    { // sub-octet bit shift is required
	      // Vector Shift Left by (shb%8) bits
	      sl_a = vec_sll (sl_a, lshift);
	    }
	  result = (vui128_t) sl_a;
	}
    }
#else
  result =  vec_slqi_PWR8 (vra, shb);
#endif
  return (vui128_t) result;
}

/** \brief Vector Shift left Quadword Immediate for POWER7 and earlier.
 *
 *  Shift left each quadword element [0], 0-127 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-127.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 63 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  4-6  | 1/cycle  |
 *  |power9   |  6-9  | 1/cycle  |
 *  |power10  |  6-12 | 4/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned long int.
 *  @param shb shift amount in the range 0-63.
 *  @return 128-bit vector unsigned long long int, shifted left shb bits.
 */
static inline vui128_t
vec_slqi_PWR10 (vui128_t vra, const unsigned int shb)
{
  vui128_t result = vra;

#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  if (__builtin_constant_p (shb) && (shb < 128))
    {
      /*
       * POWER10 has xxspltib and vslq. But the Intrinsic Reference
       * insists the shift count should be __int128. Current
       * compilers will not reliably generate xxspltib and the only
       * way to use a vector char as shift count requires inline asm.
       * POWER10 also has vsldbi which nicely works with vsldoi.
       * Also vslq is cracked into 2 IOPs so the timing is the same
       * as the vsldoi/vsldbi combination. We must choose wisely.
       *
       * So split the 7-bit shift count into octet (high 4-bits) and
       * bit (low 3-bits) to make it easier to generate. Then use
       * the vector shift left double bit/octet immediate instructions.
       * I.E. replace vec_slo() with vec_sld() which includes an
       * immediate octet shift count. So we don't have to generate
       * a separate octet shift count vector but do need an 0's vector
       * to mimic vec_slo using vec_sld.
       * Then replace vec_sll with vec_sldb which also includes an
       * immediate bit shift count.
       */
      if ((shb > 7) && ((shb % 8) != 0))
	{
          result = vec_vslq_PWR10 (vra, vec_splats ((unsigned char) shb));
	}
      else
	{
	  vui8_t sl_0 = (vui8_t) vec_splat_u8(0);
	  vui8_t sl_a = ((vui8_t) vra);
	  if (shb > 7)
	    { // Shift is more the 7-bits
	      // Vector Shift Left By Octet (shb/8). Shifting in 0's.
	      sl_a = vec_sld ((vui8_t) vra, sl_0, (shb / 8));
	    }
	  if ((shb & 7) != 0)
	    { // sub-octet bit shift is required
	      // Vector Shift Left by (shb%8) bits
	      sl_a = vec_sldbi_PWR10 (sl_a, sl_0, (shb % 8));
	    }
	  result = (vui128_t) sl_a;
	}
    }
#else
  result =  vec_slqi_PWR9 (vra, shb);
#endif

  return (vui128_t) result;
}

/** \brief Vector Shift Left Quadword by Byte for POWER9 and earlier.
 *
 *  Vector Shift Left Quadword 0-127 bits.
 *  The 7-bit shift count is splatted across the 16-bytes of vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-6   | 1/cycle  |
 *  |power9   | 6-9   | 1/cycle  |
 *  |power10  | 3-4   | 4/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @param vrb Shift count in bits 1:7, splatted across bytes 0-15.
 *  @return Left shifted vector.
 */
static inline vui128_t
vec_slq_PWR9 (vui128_t vra, vui8_t vrb)
{
  vui8_t result;
  result = vec_slo ((vui8_t) vra, vrb);
  result = vec_sll (result, vrb);
  return ((vui128_t) result);
}

/** \brief Vector Shift Left Quadword by Byte for POWER10 and earlier.
 *
 *  Vector Shift Left Quadword 0-127 bits.
 *  The 7-bit shift count is splatted across the 16-bytes of vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     | 1/cycle  |
 *  |power9   | 6     | 1/cycle  |
 *  |power10  | 3-4   | 4/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @param vrb Shift count in bits 1:7, splatted across bytes 0-15.
 *  @return Left shifted vector.
 */
static inline vui128_t
vec_slq_PWR10 (vui128_t vra, vui8_t vrb)
{
  vui128_t result;

#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
#ifdef __clang__
  result = vec_sl (vra, (vui128_t) vrb);
#else
  __asm__(
      "vslq %0,%1,%2;\n"
      : "=v" (result)
      : "v" (vra), "v" (vrb)
      : );
#endif
#else
  result =  vec_slq_PWR9 (vra, vrb);
#endif
  return ((vui128_t) result);
}

/** \brief Vector Shift Right Quadword Immediate for POWER9 and earlier.
 *
 *  Shift Right each quadword element [0], 0-127 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-127.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 63 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   |  4-6  | 1/cycle  |
 *  |power8   |  4-6  | 1/cycle  |
 *  |power9   |  6-9  | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned long int.
 *  @param shb shift amount in the range 0-127.
 *  @return 128-bit vector unsigned long long int, shifted Right shb bits.
 */
static inline vui128_t
vec_srqi_PWR8 (vui128_t vra, const unsigned int shb)
{
  vui128_t result = vra;

  if (__builtin_constant_p (shb) && (shb < 128))
    {
      /*
       * POWER8 and earlier has splat immediate with a 5-bit SIM.
       * Hard to make a 7-bit shift count (0-127).
       * So split the 7-bit shift count into octet (high 4-bits) and
       * bit (low 3-bits) to make it easier to generate.
       * Then replace vec_sro() with vec_sld() which includes an
       * immediate octet shift count. So we don't have to generate
       * a separate octet shift count vector but do need an 0's vector
       * to mimic vec_slo using vec_sld.
       * If the low 3-bits of the shift count != 0 then splat immediate
       * the 3-bit shift count and use vec_srl().
       */
      vui8_t sl_a = ((vui8_t) vra);
      if (shb > 7)
	{ // Shift is more the 7-bits
	  vui8_t sl_0 = (vui8_t) vec_splat_u8(0);
	  // Vector Shift Left By Octet (shb/8). Shifting in 0's.
	  sl_a = vec_sld (sl_0, (vui8_t) vra, (16-(shb / 8)));
	}
      if ((shb & 7) != 0)
	{ // sub-octet bit shift is required
	  // Vector Shift Left by (shb%8) bits
	  vui8_t lshift = vec_splat_u8(shb % 8);
	  sl_a = vec_srl (sl_a, lshift);
	}
      result = (vui128_t) sl_a;
    }

  return (vui128_t) result;
}

/** \brief Vector Shift Right Quadword Immediate for POWER9 and earlier.
 *
 *  Shift Right each quadword element [0], 0-127 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-127.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 63 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   |  4-6  | 1/cycle  |
 *  |power8   |  4-6  | 1/cycle  |
 *  |power9   |  6-9  | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned long int.
 *  @param shb shift amount in the range 0-127.
 *  @return 128-bit vector unsigned long long int, shifted Right shb bits.
 */
static inline vui128_t
vec_srqi_PWR9 (vui128_t vra, const unsigned int shb)
{
  vui128_t result = vra;

#if defined (_ARCH_PWR9)
  if (__builtin_constant_p (shb) && (shb < 128))
    {
      /*
       * POWER9 has xxspltib and vsro/vsr which together perform a
       * quadword shift right. POWER9 also has vsldoi which
       * includes an immediate (octet) shift count.
       * But the rotate (vsldoi) implementation of srq requires
       * the quadword zero extend of vra (xxspltib 0). So using vsro/vsr
       * might pay off if we can generate the 7 bit shift count in one
       * xxspltib instruction.
       *
       * But if only vsldoi ((shb % 8) == 0) or vsrdbi (shb < 8) are
       * needed then xxspltib shb is not needed and can be replaced by
       * xxspltib 0. We must choose wisely.
       */
      if ((shb > 7) && ((shb % 8) != 0))
	{
          result = vec_vsrq_PWR9 (vra, vec_splats ((unsigned char) shb));
	}
      else
	{
	  vui8_t sr_a = ((vui8_t) vra);
	  if (shb > 7)
	    { // Shift is more the 7-bits
	      vui8_t sr_0 = vec_splat_u8(0);
	      // Vector Shift Left By Octet (shb/8). Shifting in 0's.
	      sr_a = vec_sld (sr_0, (vui8_t) vra, (16-(shb / 8)));
	    }
	  if ((shb & 7) != 0)
	    { // sub-octet bit shift is required
	      // Vector Shift Left by (shb%8) bits
	      vui8_t rshift = vec_splat_u8(shb % 8);
	      sr_a = vec_srl (sr_a, rshift);
	    }
      result = (vui128_t) sr_a;
	}
    }
#else
  result =  vec_srqi_PWR8 (vra, shb);
#endif

  return (vui128_t) result;
}

/** \brief Vector Shift Right Quadword Immediate for POWER9 and earlier.
 *
 *  Shift Right each quadword element [0], 0-127 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-127.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 63 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   |  4-6  | 1/cycle  |
 *  |power8   |  4-6  | 1/cycle  |
 *  |power9   |  6-9  | 1/cycle  |
 *  |power10  |  6-12 | 4/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned long int.
 *  @param shb shift amount in the range 0-127.
 *  @return 128-bit vector unsigned long long int, shifted Right shb bits.
 */
static inline vui128_t
vec_srqi_PWR10 (vui128_t vra, const unsigned int shb)
{
  vui128_t result = vra;

#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)

  if (__builtin_constant_p (shb) && (shb < 128))
    {
      /*
       * POWER10 has xxspltib and vsrq. But the Intrinsic Reference
       * insists the shift count should be __int128. For current
       * compilers the only way to use a vector char as shift count
       * requires inline asm for vsrq.
       *
       * POWER10 also has vsrdbi which nicely works with vsldoi and
       * include immediate (octet/bit) shift counts.
       * Also vsrq is cracked into 2 IOPs so the timing is the same
       * as the vsldoi/vsrdbi combination.
       *
       * But the rotate (vsldoi/vsrdbi) implementation of srq requires
       * the quadword zero extend of vra (xxspltib 0). So using vsrq
       * might pay off if we can generate the 7 bit shift count in one
       * xxspltib instruction.
       *
       * But if only vsldoi ((shb % 8) == 0) or vsrdbi (shb < 8) are
       * needed then xxspltib is not needed and can be replaced by
       * xxspltib 0. We must choose wisely.
       */
      if ((shb > 7) && ((shb % 8) != 0))
	{
          result = vec_vsrq_PWR10 (vra, vec_splats ((unsigned char) shb));
	}
      else
	{
	  vui8_t sl_a = ((vui8_t) vra);
	  vui8_t vzero = vec_splat_u8(0);
	  if (shb > 7)
	    { // Shift is more the 7-bits
	      // Vector Shift Left double By Octet (16-(shb / 8)) after
	      // pre-pending the sign-mask.
	      sl_a = vec_sld (vzero, (vui8_t) vra, (16-(shb / 8)));
	    }
	  if ((shb & 7) != 0)
	    { // sub-octet bit shift is required
	      // Vector Shift Right Double Bit by (shb%8).
	      sl_a = vec_srdbi_PWR10 (vzero, sl_a, (shb % 8));
	    }
	  result = (vui128_t) sl_a;
	}
    }
#else
  result =  vec_srqi_PWR9 (vra, shb);
#endif

  return (vui128_t) result;
}

/** \brief Vector Shift Right Quadword by Byte for POWER9 and earlier.
 *
 *  Vector Shift Right Quadword 0-127 bits.
 *  The 7-bit shift count is splatted across the 16-bytes of vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     | 1/cycle  |
 *  |power9   | 6     | 1/cycle  |
 *  |power10  | 3-4   | 4/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @param vrb Shift count in bits 1:7, splatted across bytes 0-15.
 *  @return Right shifted vector.
 */
static inline vui128_t
vec_srq_PWR9 (vui128_t vra, vui8_t vrb)
{
  vui8_t result;
  result = vec_sro ((vui8_t) vra, vrb);
  result = vec_srl (result, vrb);
  return ((vui128_t) result);
}

/** \brief Vector Shift Right Quadword by Byte for POWER10 and earlier.
 *
 *  Vector Shift Right Quadword 0-127 bits.
 *  The 7-bit shift count is splatted across the 16-bytes of vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     | 1/cycle  |
 *  |power9   | 6     | 1/cycle  |
 *  |power10  | 3-4   | 4/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @param vrb Shift count in bits 1:7, splatted across bytes 0-15.
 *  @return Right shifted vector.
 */
static inline vui128_t
vec_srq_PWR10 (vui128_t vra, vui8_t vrb)
{
  vui128_t result;

#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
#ifdef __clang__
  result = vec_sr (vra, (vui128_t) vrb);
#else
  __asm__(
      "vsrq %0,%1,%2;\n"
      : "=v" (result)
      : "v" (vra), "v" (vrb)
      : );
#endif
#else
  result =  vec_srq_PWR9 (vra, vrb);
#endif
  return ((vui128_t) result);
}

/** \brief Vector Shift Right Algebraic Quadword Immediate for POWER9 and earlier.
 *
 *  Shift Right each quadword element [0], 0-127 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-127.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 63 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   |  12   | 1/cycle  |
 *  |power8   |  12   | 1/cycle  |
 *  |power9   | 9-16  | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned long int.
 *  @param shb shift amount in the range 0-127.
 *  @return 128-bit vector unsigned long long int, shifted Right shb bits.
 */
static inline vi128_t
vec_sraqi_PWR7 (vi128_t vra, const unsigned int shb)
{
  vi128_t result = vra;

  if (__builtin_constant_p (shb) && (shb < 128))
    {
	{
	  /*
	   * POWER9 and earlier has vsldoi but not vsldbi. So use the vector shift
	   * left double bit/octet immediate instructions.
	   * Split the 7-bit shift count into octet (high 4-bits) and
	   * bit (low 3-bits) to make it easier to generate.
	   * Also need to extend sign vra to double quadword to propagate
	   * the sign bit into the right shift.
	   * If shb > 7  then use vec_sld((16-(shb / 8))) to right shift
	   * octets (including the extended sign).
	   * If the low 3-bits of the shift count != 0 then splat immediate
	   * the bit shift count and use vec_sll().
	   */
	  vui8_t vsgn = (vui8_t) vec_vexpandqm_PWR7 ((vui128_t) vra);
	  vui8_t sl_a = ((vui8_t) vra);
	  if (shb > 7)
	    { // Shift is more the 7-bits
	      // Vector Shift Left By Octet (shb/8). Shifting in 0's.
	      sl_a = vec_sld (vsgn, (vui8_t) vra, (16 - (shb / 8)));
	    }
	  if ((shb & 7) != 0)
	    { // sub-octet bit shift is required
	      // Vector Shift Left by (shb%8) bits
	      sl_a = vec_srdbi_PWR9 (vsgn, sl_a, (shb % 8));
	    }
	  result = (vi128_t) sl_a;
	}
    }

  return (vi128_t) result;
}

/** \brief Vector Shift Right Algebraic Quadword Immediate for POWER9 and earlier.
 *
 *  Shift Right each quadword element [0], 0-127 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-127.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 63 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   |  12   | 1/cycle  |
 *  |power8   |  12   | 1/cycle  |
 *  |power9   | 9-16  | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned long int.
 *  @param shb shift amount in the range 0-127.
 *  @return 128-bit vector unsigned long long int, shifted Right shb bits.
 */
static inline vi128_t
vec_sraqi_PWR8 (vi128_t vra, const unsigned int shb)
{
  vi128_t result = vra;

#if defined (_ARCH_PWR8)  && (__GNUC__ >= 6)
  if (__builtin_constant_p (shb) && (shb < 128))
    {
      // This trick requires Vector Shift Right Algebraic Doubleword
      // and VSX Vector Permute Doubleword Immediate
      if (__builtin_constant_p (shb) && (shb < 64))
	{
	  vui128_t vrshq;
	  vi64_t vrshd;
	  vui8_t rshift = vec_splat7_u8 (shb);
	  {
		  vui8_t sl_a = ((vui8_t) vra);
		  if (shb > 7)
		    { // Shift is more the 7-bits
		      // Vector Shift Left By Octet (shb/8). Shifting in 0's.
		      sl_a = vec_sld ((vui8_t) vra, (vui8_t) vra, (16 - (shb / 8)));
		    }
		  if ((shb & 7) != 0)
		    { // sub-octet bit shift is required
		      // Vector Shift Left by (shb%8) bits
		      sl_a = vec_srl (sl_a, (rshift));
		    }
		  vrshq = (vui128_t) sl_a;
	  }
	  vrshd = vec_vsrad_PWR8 ((vi64_t) vra, rshift);
	  result = (vi128_t) vec_pasted_PWR8 ((vui64_t) vrshd, (vui64_t) vrshq);
	}
      else
	{
	  /*
	   * POWER9 and earlier has vsldoi but not vsldbi. So use the vector shift
	   * left double bit/octet immediate instructions.
	   * Split the 7-bit shift count into octet (high 4-bits) and
	   * bit (low 3-bits) to make it easier to generate.
	   * Also need to extend sign vra to double quadword to propagate
	   * the sign bit into the right shift.
	   * If shb > 7  then use vec_sld((16-(shb / 8))) to right shift
	   * octets (including the extended sign).
	   * If the low 3-bits of the shift count != 0 then splat immediate
	   * the bit shift count and use vec_sll().
	   */
	  vui8_t vsgn = (vui8_t) vec_vexpandqm_PWR7 ((vui128_t) vra);
	  vui8_t sl_a = ((vui8_t) vra);
	  if (shb > 7)
	    { // Shift is more the 7-bits
	      // Vector Shift Left By Octet (shb/8). Shifting in 0's.
	      sl_a = vec_sld (vsgn, (vui8_t) vra, (16 - (shb / 8)));
	    }
	  if ((shb & 7) != 0)
	    { // sub-octet bit shift is required
	      // Vector Shift Left by (shb%8) bits
	      if (__builtin_constant_p (shb) && (shb > 64))
		{
		  sl_a = (vui8_t) vec_srqi_PWR8 ((vui128_t)sl_a, (shb % 8));
		  sl_a = (vui8_t) vec_pasted_PWR8 ((vui64_t) vsgn,(vui64_t) sl_a);
		}
	      else
	        sl_a = vec_srdbi_PWR9 (vsgn, sl_a, (shb % 8));
	    }
	  result = (vi128_t) sl_a;
	}
    }
#else
  result =  vec_sraqi_PWR7 (vra, shb);
#endif

  return (vi128_t) result;
}

/** \brief Vector Shift Right Algebraic Quadword Immediate for POWER9 and earlier.
 *
 *  Shift Right each quadword element [0], 0-127 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-127.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 63 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   |  12   | 1/cycle  |
 *  |power8   |  12   | 1/cycle  |
 *  |power9   | 9-16  | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned long int.
 *  @param shb shift amount in the range 0-127.
 *  @return 128-bit vector unsigned long long int, shifted Right shb bits.
 */
static inline vi128_t
vec_sraqi_PWR9 (vi128_t vra, const unsigned int shb)
{
  vi128_t result = vra;

#if defined (_ARCH_PWR9)
  if (__builtin_constant_p (shb) && (shb < 128))
    {
      // This trick requires Vector Shift Right Algebraic Doubleword
      // and VSX Vector Permute Doubleword Immediate
      if (__builtin_constant_p (shb) && (shb < 64))
	{
	  vui128_t vrshq;
	  vi64_t vrshd;

	  vui8_t rshift = vec_splat7_u8 (shb);
	  vrshq = vec_vsrq_PWR9 ((vui128_t) vra, rshift);
	  vrshd = vec_vsrad_PWR8 ((vi64_t) vra, rshift);
	  result = (vi128_t) vec_pasted_PWR8 ((vui64_t) vrshd, (vui64_t) vrshq);
	}
      else
	{
	  /*
	   * POWER9 and earlier has vsldoi but not vsldbi. So use the vector shift
	   * left double bit/octet immediate instructions.
	   * Split the 7-bit shift count into octet (high 4-bits) and
	   * bit (low 3-bits) to make it easier to generate.
	   * Also need to extend sign vra to double quadword to propagate
	   * the sign bit into the right shift.
	   * If shb > 7  then use vec_sld((16-(shb / 8))) to right shift
	   * octets (including the extended sign).
	   * If the low 3-bits of the shift count != 0 then splat immediate
	   * the bit shift count and use vec_sll().
	   */
	  vui8_t vsgn = (vui8_t) vec_vexpandqm_PWR7 ((vui128_t) vra);
	  vui8_t sl_a = ((vui8_t) vra);
	  if (shb > 7)
	    { // Shift is more the 7-bits
	      // Vector Shift Left By Octet (shb/8). Shifting in 0's.
	      sl_a = vec_sld (vsgn, (vui8_t) vra, (16 - (shb / 8)));
	    }
	  if ((shb & 7) != 0)
	    { // sub-octet bit shift is required
	      // Vector Shift Left by (shb%8) bits
	      if (__builtin_constant_p (shb) && (shb > 64))
		{
		  sl_a = (vui8_t) vec_srqi_PWR9 ((vui128_t)sl_a, (shb % 8));
		  sl_a = (vui8_t) vec_pasted_PWR8 ((vui64_t) vsgn,(vui64_t) sl_a);
		}
	      else
	        sl_a = vec_srdbi_PWR9 (vsgn, sl_a, (shb % 8));
	    }
	  result = (vi128_t) sl_a;
	}
    }
#else
  result =  vec_sraqi_PWR8 (vra, shb);
#endif

  return (vi128_t) result;
}

/** \brief Vector Shift Right Algebraic Quadword Immediate for POWER10 and earlier.
 *
 *  Shift Right each quadword element [0], 0-127 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-127.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 63 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 6-12  | 1/cycle  |
 *  |power8   | 6-12  | 1/cycle  |
 *  |power9   | 9-16  | 1/cycle  |
 *  |power10  | 6-12  | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned long int.
 *  @param shb shift amount in the range 0-127.
 *  @return 128-bit vector unsigned long long int, shifted Right shb bits.
 */
static inline vi128_t
vec_sraqi_PWR10 (vi128_t vra, const unsigned int shb)
{
  vi128_t result = vra;

#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)

  if (__builtin_constant_p (shb) && (shb < 128))
    {
      /*
       * POWER10 has xxspltib and vsraq. But the Intrinsic Reference
       * insists the shift count should be __int128. For current
       * compilers the only way to use a vector char as shift count
       * requires inline asm for vsraq.
       *
       * POWER10 also has vsrdbi which nicely works with vsldoi and
       * include immediate (octet/bit) shift counts.
       * Also vsraq is cracked into 2 IOPs so the timing is the same
       * as the vsldoi/vsrdbi combination.
       *
       * But the rotate (vsldoi/vsrdbi) implementation of sraq requires
       * the quadword sign extend of vra (vexpandqm). So using vsraq
       * might pay off if we can generate the 7 bit shift count in one
       * xxspltib instruction.
       *
       * But if only vsldoi ((shb % 8) == 0) or vsrdbi (shb < 8) are
       * needed then xxspltib is not needed and can be replaced by
       * vexpandqm. We must choose wisely.
       */
      if ((shb > 7) && ((shb % 8) != 0))
	{
          result = vec_vsraq_PWR10 (vra, vec_splats ((unsigned char) shb));
	}
      else
	{
	  vui8_t sl_a = ((vui8_t) vra);
	  vui8_t vsgn = (vui8_t) vec_vexpandqm_PWR10 ((vui128_t) vra);
	  if (shb > 7)
	    { // Shift is more the 7-bits
	      // Vector Shift Left double By Octet (16-(shb / 8)) after
	      // pre-pending the sign-mask.
	      sl_a = vec_sld (vsgn, (vui8_t) vra, (16-(shb / 8)));
	    }
	  if ((shb & 7) != 0)
	    { // sub-octet bit shift is required
	      // Vector Shift Right Double Bit by (shb%8).
	      sl_a = vec_srdbi_PWR10 (vsgn, sl_a, (shb % 8));
	    }
	  result = (vi128_t) sl_a;
	}
    }
#else
  result =  vec_sraqi_PWR9 (vra, shb);
#endif

  return (vi128_t) result;
}

/** \brief Vector Shift Right Algebraic Quadword for POWER9 and earlier.
 *
 *  Vector Shift Right Algebraic Quadword 0-127 bits.
 *  The 7-bit shift count is splatted across the 16-bytes of vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10    | 1 cycle  |
 *  |power9   | 14    | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as signed __int128.
 *  @param vrb Shift count in bits 1:7, splatted across bytes 0-15.
 *  @return Right algebraic shifted vector.
 */
static inline vi128_t
vec_sraq_PWR9 (vi128_t vra, vui8_t vrb)
{
  vui8_t result;
  vui8_t vsht;
  vui8_t vsgn;
  const vui8_t zero = vec_splat_u8 (0);

  /* For some reason the vsr instruction only works
   * correctly if the bit shift value is splatted to each byte
   * of the vector.  */
  vsgn = (vui8_t) vec_vexpandqm_PWR7 ((vui128_t) vra);
  vsht = vec_sub (zero, (vui8_t) vrb);
  result = (vui8_t) vec_sldq_PWR7 (vsgn, (vui8_t) vra, (vui8_t) vsht);

  return ((vi128_t) result);
}

/** \brief Vector Shift Right Algebraic Quadword for POWER10 and earlier.
 *
 *  Vector Shift Right Algebraic Quadword 0-127 bits.
 *  The 7-bit shift count is splatted across the 16-bytes of vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10    | 1 cycle  |
 *  |power9   | 14    | 1/cycle  |
 *  |power10  | 6 - 8 | 4/cycle  |
 *
 *
 *  @param vra a 128-bit vector treated as signed __int128.
 *  @param vrb Shift count in bits 1:7, splatted across bytes 0-15.
 *  @return Right algebraic shifted vector.
 */
static inline vi128_t
vec_sraq_PWR10 (vi128_t vra, vui8_t vrb)
{
  vui8_t result;
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  __asm__(
      "vsraq %0,%1,%2;\n"
      : "=v" (result)
      : "v" (vra), "v" (vrb)
      : );
#else
  result = (vui8_t) vec_sraq_PWR9 (vra, vrb);
#endif
  return ((vi128_t) result);
}

/** \brief Vector Splat (5-bit)Immediate Signed Byte.
 *
 *  Small constants are need for shift/rotate operations which require
 *  5/6/7-bit shift counts. The sim6 constant value signed extended
 *  and splatted to each Byte.
 *
 *  \note Before PowerISA 3.0 (power9) only Vector Splat Immediate
 *  Signed Byte/Halfword/Word with a 5-bit Signed Immediate (SIM)
 *  or loading a vector constant from storage are available.
 *  The load is at least 3 instructions and 9 cycles latency.
 *  So a short sequence of (2 cycle) vector splat immediate and
 *  integer arithmetic operations can provide better performance
 *  (less then 9 cycles latency).
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *  |power10  | 3-4   | 4/cycle  |
 *
 *  @param sim5 shift amount in the range -16 to 15.
 *  @return 128-bit vector unsigned char, where the 5-bit sim5 is
 *  signed extend.
 */
static inline vi8_t
vec_splat5_s8 (const signed int sim5)
{
  vi8_t result;
#if defined(_ARCH_PWR9)
  if (__builtin_constant_p (sim5) && (sim5 >= -16) && (sim5 < 16))
    result = vec_splats ((signed char)sim5);
#else // defined(_ARCH_PWR8)
  if (__builtin_constant_p (sim5) && (sim5 >= -16) && (sim5 < 16))
    result = vec_splat_s8(sim5);
  else
    result = vec_splats ((signed char)sim5);
#endif
  return result;
}

/** \brief Vector Splat (5-bit)Immediate Unsigned Byte.
 *
 *  Small constants are need for shift/rotate operations which require
 *  5/6/7-bit shift counts. The sim6 constant value signed extended
 *  and splatted to each Byte.
 *
 *  The high-order 3-bits of each Byte are <B>Boundedly Undefined</B>.
 *  They are either sign or 0 extended based on the target ISA and
 *  the whim of the developer. This value is replicated across
 *  the 16 byte elements.
 *
 *  \note Before PowerISA 3.0 (power9) only Vector Splat Immediate
 *  Signed Byte/Halfword/Word with a 5-bit Signed Immediate (SIM)
 *  or loading a vector constant from storage are available.
 *  The load is at least 3 instructions and 9 cycles latency.
 *  So a short sequence of (2 cycle) vector splat immediate and
 *  integer arithmetic operations can provide better performance
 *  (less then 9 cycles latency).
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *  |power10  | 3-4   | 4/cycle  |
 *
 *  @param sim5 shift amount in the range -16 to 15.
 *  @return 128-bit vector unsigned char, where the 5-bit sim5 is
 *  signed extend.
 */
static inline vui8_t
vec_splat5_u8 (const unsigned int sim5)
{
  vui8_t result;
#if defined(_ARCH_PWR9)
  if (__builtin_constant_p (sim5) && (sim5 < 32))
    result = vec_splats ((unsigned char)sim5);
#else // defined(_ARCH_PWR8)
  if (__builtin_constant_p (sim5) && (sim5 < 16))
    result = vec_splat_u8(sim5);
  else if (__builtin_constant_p (sim5) && (sim5 < 32))
    result = vec_splat_u8(sim5-32);
  else
    result = vec_splats ((unsigned char)sim5);
#endif
  return result;
}

/** \brief Vector Splat (5-bit)Immediate Signed Byte.
 *
 *  Small constants are need for shift/rotate operations which require
 *  5/6/7-bit shift counts. The sim5 constant value signed extended
 *  and splatted to each Byte.
 *
 *  \note Before PowerISA 3.0 (power9) only Vector Splat Immediate
 *  Signed Byte/Halfword/Word with a 5-bit Signed Immediate (SIM)
 *  or loading a vector constant from storage are available.
 *  The load is at least 3 instructions and 9 cycles latency.
 *  So a short sequence of (2 cycle) vector splat immediate and
 *  integer arithmetic operations can provide better performance
 *  (less then 9 cycles latency).
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-6   | 2/cycle  |
 *  |power9   | 3-5   | 2/cycle  |
 *  |power10  | 3-4   | 4/cycle  |
 *
 *  @param sim6 shift amount in the range -32 to 31.
 *  @return 128-bit vector unsigned char, where the 6-bit sim6 is
 *  signed extend.
 */
static inline vi8_t
vec_splat6_s8 (const signed int sim6)
{
  vi8_t result;
#if defined(_ARCH_PWR9)
  if (__builtin_constant_p (sim6) && (sim6 >= -32) && (sim6 < 32))
    result = vec_splats ((signed char)sim6);
#else // defined(_ARCH_PWR8)
  if (__builtin_constant_p (sim6) && (sim6 >= -16) && (sim6 < 16))
    result = vec_splat_s8(sim6);
  else
    { // 6-bit sim6 in the range (sim6 < -16) || (sim6 > 15)
      vi8_t tmp;
      if (__builtin_constant_p (sim6) && (sim6 % 2 == 0))
	{
	  /* For even values less then -16 or greater than 15.
	   * Divide sim6 by 2 (sim6 >> 1), range is -16 to 15
	   * generate the (signed) 5-bit splat immediate word,
	   * then double (*2) the word vector,
	   * to generate the 6-bit sim6.
	   * This is one less instruction then the general case. */
	  tmp = vec_splat_s8(sim6 >> 1);
	  result = vec_add (tmp, tmp);
	}
      else
	{
	  const vi8_t v16 = vec_splat_s8(-16);
	  /* To cover the range less then -16 or greater than 15.
	   * Add/Subtract 16 (sim6+-16) to reduce the range,
	   * generate the (signed) 5-bit splat immediate word,
	   * then vec_add the 16 back,
	   * to generate the 6-bit immediate. */
	  if (__builtin_constant_p (sim6) && (sim6 > 15))
	    {
	      tmp = vec_splat_s8(sim6 - 16);
	      result = vec_sub (tmp, v16);
	    }
	  else
	    {
	      tmp = vec_splat_s8(sim6 + 16);
	      result = vec_add (tmp, v16);
	    }
	}
    }
#endif

  return result;
}

/** \brief Vector Splat (6-bit)Immediate Byte.
 *
 *  Small constants are need for shift/rotate operations which require
 *  5/6/7-bit shift counts. The sim6 constant value is splatted to
 *  the low-order 6-bits for each Byte. This exactly matches the
 *  requirements of the Doubleword vector shift/rotate instructions
 *  as only the low-order 6-bits of each Doubleword is used.
 *
 *  The high-order 2-bits of each Byte are <B>Boundedly Undefined</B>.
 *  They are either sign or 0 extended based on the target ISA and
 *  the whim of the developer. This value is replicated across
 *  the 16 byte elements.
 *
 *  \note Before PowerISA 3.0 (power9) only Vector Splat Immediate
 *  Signed Byte/Halfword/Word with a 5-bit Signed Immediate (SIM)
 *  or loading a vector constant from storage are available.
 *  The load is at least 3 instructions and 9 cycles latency.
 *  So a short sequence of (2 cycle) vector splat immediate and
 *  integer arithmetic operations can provide better performance
 *  (less then 9 cycles latency).
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-6   | 2/cycle  |
 *  |power9   | 3-5   | 2/cycle  |
 *  |power10  | 3-4   | 4/cycle  |
 *
 *  @param sim6 shift amount in the range 0-63.
 *  @return 128-bit vector unsigned char, with const sim6 placed
 *  in the low-order 6 bits of each byte element.
 */
static inline vui8_t
vec_splat6_u8 (const unsigned int sim6)
{
  vui8_t result;

  if (__builtin_constant_p (sim6) && (sim6 < 64))
#if defined(_ARCH_PWR9)
    result = vec_splats ((unsigned char)sim6);
#else // defined(_ARCH_PWR8)
    if (__builtin_constant_p (sim6) && (sim6 < 16))
      result = vec_splat_u8(sim6);
    else if (__builtin_constant_p (sim6) && (sim6 > 47))
      // 6-bit shift count in the range 48-63
      result = vec_splat_u8(sim6 - 64);
    else
      { // 6-bit shift count in the range 16-47
	vui8_t tmp;
	if (__builtin_constant_p (sim6) && (sim6 % 2 == 0))
	  {
	    /* To cover even values for range 16-46.
	     * Divide sim6 by 2 (sim6 >> 1), range is 8-23
	     * generate the (signed) 5-bit splat immediate word,
	     * then double (*2) the byte vector,
	     * to generate the 6-bit sim6.
	     * This is one less instruction then the general case. */
	    if (/* __builtin_constant_p (sim6) && */(sim6 < 32))
	      tmp = vec_splat_u8(sim6 >> 1);
	    else
	      tmp = vec_splat_u8((sim6 >> 1) - 32);

	    result = vec_add (tmp, tmp);
	  }
	else
	  {
#if (defined (__GNUC__) && (__GNUC__ < 8))
	    /* GCC v8+ will convert the following sequences to constant
	     * vector in .rodata and replace this sequence with a load.
	     * While these sequences run 4-5 instruction they complete
	     * in 6-8 (dual issue) cycles. This is still better than a
	     * 7-13 cycle vector load from .rodata. */
	    // PVECLIB_AVOID_RODATA
	    /* To cover the odd shift counts in the range 17-47.
	     * Subtract 16 (sim6-16) to reduce the range,
	     * generate the (signed) 5-bit splat immediate word,
	     * then vec_add (word) the 16 back,
	     * to generate the 6-bit shift count. */
	    if (__builtin_constant_p (sim6) && (sim6 > 16) && (sim6 < 32))
	      {
		const vui8_t v16 = vec_splat_u8(-16);
		tmp = vec_splat_u8(sim6 - 16);
		result = vec_sub (tmp, v16);
	      }
	    else if (__builtin_constant_p (sim6) && (sim6 > 32) && (sim6 < 48))
	      {
		const vui8_t v16 = vec_splat_u8(-16);
		tmp = vec_splat_u8(sim6 - 48);
		result = vec_add (tmp, v16);
	      }
#else
	    /* This sequence runs 5 instructions or less if any of the 3
	     * splat (v3, vbyte, vbit) constants are common. The latency
	     * is still within 6 cycles (beats 9 cycles). While less
	     * elegant then the code above it avoids negative values and
	     * multiple dependent adds. This seems to get past GCC's
	     * convert to .rodata filter. Also GCC will convert this to
	     * xxspltib for POWER9. */
	    if (__builtin_constant_p (sim6) && (sim6 > 16) && (sim6 < 48))
	      {
		    const vui8_t v2 = vec_splat_u8(2);
		    const vui8_t vbyte = vec_splat_u8((sim6 / 4));
		    const vui8_t vbit = vec_splat_u8((sim6 % 4));
		    /* To cover the odd numbers 16-48.
		     * Use splat immediates and shift left to generate
		     * the high 4-bits. Then splat immediate the low
		     * 2-bits. Then sum (add) to generate the 6-bit
		     * (doubleword) shift count. Requires 5
		     * instructions (or 3-4 with CSE) and 6 cycles
		     * latency. */
		    // vbyte * 4
		    tmp = vec_sl(vbyte, v2);
		    // 6-bit shift count == (vbyte * 4) + vbit
		    result = vec_add (tmp, vbit);
	      }
#endif
	  }
      }
#endif
  else
    vec_splats ((unsigned char) sim6);

  return result;
}

/** \brief Vector Splat (7-bit)Immediate Byte.
 *
 *  Small constants are need for shift/rotate operations which require
 *  5/6/7-bit shift counts. The sim6 constant value is splatted to
 *  the low-order 6-bits for each Byte. This exactly matches the
 *  requirements of the Doubleword vector shift/rotate instructions
 *  as only the low-order 6-bits of each Doubleword is used.
 *
 *  The high-order bit of each Byte is <B>Boundedly Undefined</B>.
 *  They are either sign or 0 extended based on the target ISA and
 *  the whim of the developer. This value is replicated across
 *  the 16 byte elements.
 *
 *  \note Before PowerISA 3.0 (power9) only Vector Splat Immediate
 *  Signed Byte/Halfword/Word with a 5-bit Signed Immediate (SIM)
 *  or loading a vector constant from storage are available.
 *  The load is at least 3 instructions and 9 cycles latency.
 *  So a short sequence of (2 cycle) vector splat immediate and
 *  integer arithmetic operations can provide better performance
 *  (less then 9 cycles latency).
 *  For POWER8 this code should generate 1-5 instructions
 *  and 2 - 6 cycles.
 *  If the splat immediate sequence would require more then 3
 *  instructions it should generate a vector load from .rodata.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-9   | 2/cycle  |
 *  |power9   | 3-5   | 2/cycle  |
 *  |power10  | 3-4   | 4/cycle  |
 *
 *  @param sim7 shift amount in the range 0-127.
 *  @return 128-bit vector unsigned char, with const sim7 placed
 *  in the low-order 7 bits of each byte element.
 */
static inline vui8_t
vec_splat7_u8 (const unsigned int sim7)
{
  vui8_t result;

  if (__builtin_constant_p (sim7) && (sim7 < 128))
#if defined(_ARCH_PWR9)
    result = vec_splats ((unsigned char)sim7);
#else // defined(_ARCH_PWR8)
    if (sim7 < 16)
      result = vec_splat_u8(sim7);
    else if (sim7 > 111)
      // 7-bit shift count in the range 112-127
      result = vec_splat_u8(sim7 - 128);
    else if (sim7 == 64)
    // 7-bit shift count is 64
      {
	vui8_t v4 = vec_splat_u8(4);
	result = vec_sl (v4, v4);
      }
    else
      { // 7-bit shift count in the range 65-111
	vui8_t tmp;
	if ((sim7 % 2 == 0) && ((sim7 < 32) || (sim7 >= 96)))
	  {
	    /* To cover even values for range 96-110.
	     * Divide sim7 by 2 (sim7 >> 1), range is 48-55
	     * generate the (signed) 5-bit splat immediate word,
	     * then double (*2) the word vector,
	     * to generate the 7-bit sim7.
	     * This is one less instruction then the general case. */
	    tmp = vec_splat6_u8 ((sim7 >> 1));
	    result = vec_add (tmp, tmp);
	  }
	else if ((sim7 % 4 == 0) && (sim7 < 64))
	  {
	    /* To cover modulo 4 values for range 32-60.
	     * Divide sim7 by 4 (sim6 >> 1), range is 8-15
	     * generate the (signed) 5-bit splat immediate word,
	     * then double (*2) the word vector,
	     * to generate the 7-bit sim7.
	     * This is one less instruction then the general case. */
	    const vui8_t v2 = vec_splat_u8(2);
	    tmp = vec_splat_u8(sim7 >> 2);

	    result = vec_sl (tmp, v2);
	  }
	else if ((sim7 % 4 == 0) && (sim7 > 64))
	  {
	    /* To cover modulo 4 values for range 68-92.
	     * Divide sim7 by 4 (sim6 >> 2), range is 17-23 (-15 to -9)
	     * generate the (signed) 5-bit splat immediate word,
	     * then double (*2) the word vector,
	     * to generate the 7-bit sim7.
	     * This is one less instruction then the general case. */
	    const vui8_t v2 = vec_splat_u8(2);
	    tmp = vec_splat_u8((sim7 >> 2) - 32);

	    result = vec_sl (tmp, v2);
	  }
#if (defined (__GNUC__) && (__GNUC__ < 8))
	/* GCC v8+ will convert the following sequences to constant
	 * vector in .rodata and replace this sequence with a load.
	 * While these sequences run 4-5 instruction they complete
	 * in 6-8 (dual issue) cycles. This is still better than a
	 * 7-13 cycle vector load from .rodata. */
	// PVECLIB_AVOID_RODATA
	else if ((sim7 > 16) && (sim7 < 32))
	  {
	    const vui8_t v16 = vec_splat_u8(-16);
	    /* To cover the odd numbers 97-111.
	     * Subtract 112 (sim7-112) to reduce the range,
	     * generate the (signed) 5-bit splat immediate word,
	     * then vec_add (byte) the 112  (-16) back,
	     * to generate the 7-bit shift count. */
	    tmp = vec_splat_u8(sim7 - 16);
	    result = vec_sub (tmp, v16);
	  }
	else if ((sim7 > 96) && (sim7 < 112))
	  {
	    const vui8_t v16 = vec_splat_u8(-16);
	    /* To cover the odd numbers 97-111.
	     * Subtract 112 (sim7-112) to reduce the range,
	     * generate the (signed) 5-bit splat immediate word,
	     * then vec_add (byte) the 112  (-16) back,
	     * to generate the 7-bit shift count. */
	    tmp = vec_splat_u8(sim7 - 112);
	    result = vec_add (tmp, v16);
	  }
	else if ((sim7 > 32) && (sim7 < 48))
	  {
	    const vui8_t v16 = vec_splat_u8(-16);
	    const vui8_t v32 = vec_add (v16, v16);
	    /* To cover the odd numbers 33-47.
	     * Subtract 32 (sim7-32) to reduce the range,
	     * generate the (signed) 5-bit splat immediate word,
	     * then vec_sub (byte) the   (-32) back,
	     * to generate the 7-bit shift count. */
	    tmp = vec_splat_u8(sim7 - 32);
	    result = vec_sub (tmp, v32);
	  }
	else if ((sim7 > 80) && (sim7 < 96))
	  {
	    const vui8_t v16 = vec_splat_u8(-16);
	    const vui8_t v32 = vec_add (v16, v16);
	    /* To cover the odd numbers 81-95.
	     * Subtract 96 (sim7-96) to reduce the range,
	     * generate the (signed) 5-bit splat immediate word,
	     * then vec_add (byte) the 96  (-32) back,
	     * to generate the 7-bit shift count. */
	    tmp = vec_splat_u8(sim7 - 96);
	    result = vec_add (tmp, v32);
	  }
	else if ((sim7 > 48) && (sim7 < 64))
	  {
	    const vui8_t v16 = vec_splat_u8(-16);
	    const vui8_t v32 = vec_add (v16, v16);
	    const vui8_t v48 = vec_add (v32, v16);
	    /* To cover the odd numbers 49-63.
	     * Subtract 48 (sim7-48) to reduce the range,
	     * generate the (signed) 5-bit splat immediate word,
	     * then vec_sub (byte) the   (-48) back,
	     * to generate the 7-bit shift count. */
	    tmp = vec_splat_u8(sim7 - 48);
	    result = vec_sub (tmp, v16);
	    result = vec_sub (result, v32);
	  }
	else if ((sim7 > 64) && (sim7 < 80))
	  {
	    const vui8_t v16 = vec_splat_u8(-16);
	    const vui8_t v32 = vec_add (v16, v16);
	    const vui8_t v48 = vec_add (v32, v16);
	    /* To cover the odd numbers 81-95.
	     * Subtract 96 (sim7-96) to reduce the range,
	     * generate the (signed) 5-bit splat immediate word,
	     * then vec_add (byte) the 96  (-32) back,
	     * to generate the 7-bit shift count. */
	    tmp = vec_splat_u8(sim7 - 80);
	    result = vec_add (tmp, v16);
	    result = vec_add (result, v32);
	  }
#else
	/* This sequence runs 5 instructions or less if any of the 3
	 * splat (v3, vbyte, vbit) constants are common. The latency
	 * is still within 6 cycles (beats 9 cycles). While less
	 * elegant then the code above it avoids negative values and
	 * multiple dependent adds. This seems to get past GCC's
	 * convert to .rodata filter. Also GCC will convert this to
	 * xxspltib for POWER9. */
	// PVECLIB_AVOID_RODATA
	else if ((sim7 > 16) && (sim7 < 112))
	  {
	    const vui8_t v3 = vec_splat_u8(3);
	    const vui8_t vbyte = vec_splat_u8(sim7 / 8);
	    const vui8_t vbit = vec_splat_u8(sim7 % 8);
	    /* To cover the odd numbers 16-112.
	     * Use splat immediates and shift left to generate the
	     * octet shift count (high 4-bits). Then splat immediate
	     * the byte bit shift count (low 3-bits). Then sum (add)
	     * to generate the 7-bit (quadword) shift count. Requires
	     * 5 instructions (or 3-4 with CSE) and 6 cycles latency.
	     * This matches vslo/vsl and vsro/vsr requirements. */
	    // voctet = vbyte * 8
	    tmp = vec_sl(vbyte, v3);
	    // 7-bit shift count == voctet + vbit
	    result = vec_add (tmp, vbit);
	  }
#endif
	else
	  /* The rest not handled above
	   * Odd values 33 to 95
	   * Even values 34 to 94, not divisible by 4.
	   * Require a 4 instruction sequences or a load.*/
	  result = vec_splats ((unsigned char) sim7);
      }
#endif
  else
    vec_splats ((unsigned char) sim7);

  return result;
}

/** \brief Vector Splat (6-bit)Immediate Word.
 *
 *  Small constants are need for shift/rotate operations which require
 *  5/6/7-bit shift counts. The sim6 constant value is splatted to
 *  the low-order 6-bits for each Word.
 *
 *  \note Before PowerISA 3.0 (power9) only Vector Splat Immediate
 *  Signed Byte/Halfword/Word with a 5-bit Signed Immediate (SIM)
 *  or loading a vector constant from storage are available.
 *  The load is at least 3 instructions and 9 cycles latency.
 *  So a short sequence of (2 cycle) vector splat immediate and
 *  integer arithmetic operations can provide better performance
 *  (less then 9 cycles latency).
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-8   | 2/cycle  |
 *  |power9   | 2-5   | 2/cycle  |
 *  |power10  | 3-4   | 4/cycle  |
 *
 *  @param sim6 shift amount in the range -32 to 31.
 *  @return 128-bit vector unsigned int, where the 6-bit sim6 is
 *  signed extend for each word.
 */
static inline vi32_t
vec_splat6_s32 (const signed int sim6)
{
  vi32_t result;
#if defined (_ARCH_PWR10) && (defined (__GNUC__) && (__GNUC__ >= 11))
  result = vec_splati ((signed int)sim6);
#elif defined(_ARCH_PWR9) && (__GNUC__ > 10)
  result = vec_splats ((signed int)sim6);
#else // defined(_ARCH_PWR8)
  if (__builtin_constant_p (sim6) && (sim6 >= -16) && (sim6 < 16))
    result = vec_splat_s32(sim6);
  else
    { // 6-bit sim6 in the range (sim6 < -16) || (sim6 > 15)
      vi32_t tmp;
      if (__builtin_constant_p (sim6) && (sim6 % 2 == 0))
	{
	  /* For even values less then -16 or greater than 15.
	   * Divide sim6 by 2 (sim6 >> 1), range is -16 to 15
	   * generate the (signed) 5-bit splat immediate word,
	   * then double (*2) the word vector,
	   * to generate the 6-bit sim6.
	   * This is one less instruction then the general case. */
	  tmp = vec_splat_s32(sim6 >> 1);
	  result = vec_add (tmp, tmp);
	}
      else
	{
	  vi32_t v16 = vec_splat_s32(-16);
	  /* To cover the range less then -16 or greater than 15.
	   * Add/Subtract 16 (sim6+-16) to reduce the range,
	   * generate the (signed) 5-bit splat immediate word,
	   * then vec_add the 16 back,
	   * to generate the 6-bit immediate. */
	  if (__builtin_constant_p (sim6) && (sim6 > 15))
	    {
	      tmp = vec_splat_s32(sim6 - 16);
	      result = vec_sub (tmp, v16);
	    }
	  else
	    {
	      tmp = vec_splat_s32(sim6 + 16);
	      result = vec_add (tmp, v16);
	    }
	}
    }
#endif

  return result;
}

/** \brief Vector Splat (6-bit)Immediate Word.
 *
 *  Small constants are need for shift/rotate operations which require
 *  5/6/7-bit shift counts. The sim6 constant value is splatted to
 *  the low-order 6-bits for each Word. This exactly matches the
 *  requirements of the Doubleword vector shift/rotate instructions as
 *  only the low-order 6-bits of each Doubleword shift count is used.
 *
 *  The high-order 26-bits of each Word are <B>Boundedly Undefined</B>.
 *  They are either sign or 0 extended based on the target ISA and
 *  the whim of the developer. This value is replicated across
 *  the 4 word elements.
 *
 *  \note Before PowerISA 3.0 (power9) only Vector Splat Immediate
 *  Signed Byte/Halfword/Word with a 5-bit Signed Immediate (SIM)
 *  or loading a vector constant from storage are available.
 *  The load is at least 3 instructions and 9 cycles latency.
 *  So a short sequence of (2 cycle) vector splat immediate and
 *  integer arithmetic operations can provide better performance
 *  (less then 9 cycles latency).
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-8   | 2/cycle  |
 *  |power9   | 2-5   | 2/cycle  |
 *  |power10  | 3-4   | 4/cycle  |
 *
 *  @param sim6 shift amount in the range 0-63.
 *  @return 128-bit vector unsigned int, with const sim6 placed
 *  in the low-order 6 bits of each Word element.
 */
static inline vui32_t
vec_splat6_u32 (const unsigned int sim6)
{
  vui32_t result;
#if defined (_ARCH_PWR10) && (defined (__GNUC__) && (__GNUC__ >= 11))
  result = (vui32_t) vec_splati ((signed int)sim6);
#elif defined(_ARCH_PWR9) && (__GNUC__ > 10)
  result = vec_splats ((unsigned int)sim6);
#else // defined(_ARCH_PWR8)
  if (__builtin_constant_p (sim6) && (sim6 < 16))
    result = vec_splat_u32(sim6);
  else if (__builtin_constant_p (sim6) && (sim6 > 47))
    // 6-bit shift count in the range 48-63
    result = vec_splat_u32(sim6 - 64);
  else
    { // 6-bit shift count in the range 16-47
      vui32_t tmp;
      if (__builtin_constant_p (sim6) && (sim6 % 2 == 0))
	{
	  /* To cover even values for range 16-46.
	   * Divide sim6 by 2 (sim6 >> 1), range is 8-23
	   * generate the (signed) 5-bit splat immediate word,
	   * then double (*2) the word vector,
	   * to generate the 6-bit sim6.
	   * This is one less instruction then the general case. */
	  if (/* __builtin_constant_p (sim6) && */(sim6 < 32))
	    tmp = vec_splat_u32(sim6 >> 1);
	  else
	    tmp = vec_splat_u32((sim6 >> 1) - 32);

	  result = vec_add (tmp, tmp);
	}
      else
	{
	  vui32_t v16 = vec_splat_u32(-16);
	  /* To cover the range 17-47.
	   * Subtract 16 (sim6-16) to reduce the range,
	   * generate the (signed) 5-bit splat immediate word,
	   * then vec_add (word) the 16 back,
	   * to generate the 6-bit shift count. */
	  if (__builtin_constant_p (sim6) && (sim6 < 32))
	    {
	      tmp = vec_splat_u32(sim6 - 16);
	      result = vec_sub (tmp, v16);
	    }
	  else
	    {
	      tmp = vec_splat_u32(sim6 - 48);
	      result = vec_add (tmp, v16);
	    }
	}
    }
#endif

  return result;
}

/** \brief Vector Splat (7-bit) Immediate Word.
 *
 *  Small constants are need for shift/rotate operations which require
 *  5/6/7-bit shift counts. The sim7 constant value is splatted to
 *  the low-order 7-bits for each Word. This exactly matches the
 *  requirements of the Qaudword vector shift/rotate instructions
 *  as only the low-order 7-bits of each Doubleword is used.
 *
 *  The high-order bits of each Word is <B>Boundedly Undefined</B>.
 *  They are either sign or 0 extended based on the target ISA and
 *  the whim of the developer. This value is replicated across
 *  the 4 word elements.
 *
 *  \note Before PowerISA 3.0 (power9) only Vector Splat Immediate
 *  Signed Byte/Halfword/Word with a 5-bit Signed Immediate (SIM)
 *  or loading a vector constant from storage are available.
 *  The load is at least 3 instructions and 9 cycles latency.
 *  So a short sequence of (2 cycle) vector splat immediate and
 *  integer arithmetic operations can provide better performance
 *  (less then 9 cycles latency).
 *  For POWER8 this code should generate 1-3 instructions
 *  and 2 - 6 cycles.
 *  If the splat immediate sequence would require more then 3
 *  instructions it should generate a vector load from .rodata.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-9   | 2/cycle  |
 *  |power9   | 3-5   | 2/cycle  |
 *  |power10  | 3-4   | 4/cycle  |
 *
 *  @param sim7 shift amount in the range 0-127.
 *  @return 128-bit vector unsigned char, with const sim7 placed
 *  in the low-order 7 bits of each byte element.
 */
static inline vui32_t
vec_splat7_u32 (const unsigned int sim7)
{
  vui32_t result;

  if (__builtin_constant_p (sim7) && (sim7 < 128))
#if defined (_ARCH_PWR10) && (defined (__GNUC__) && (__GNUC__ >= 11))
    result = (vui32_t) vec_splati ((signed int)sim7);
#elif defined(_ARCH_PWR9)
    result = vec_splats ((unsigned int)sim7);
#else // defined(_ARCH_PWR8)
    if (sim7 < 16)
      result = vec_splat_u32(sim7);
    else if (sim7 > 111)
      // 7-bit shift count in the range 112-127
      result = vec_splat_u32(sim7 - 128);
    else if (sim7 == 64)
    // 7-bit shift count is 64
      {
	vui32_t v4 = vec_splat_u32(4);
	result = vec_sl (v4, v4);
      }
    else
      { // 7-bit shift count in the range 65-111
	  vui32_t tmp;
    if ((sim7 % 2 == 0) && ((sim7 < 32) || (sim7 >= 96)))
      {
	/* To cover even values for range 96-110.
	 * Divide sim7 by 2 (sim7 >> 1), range is 48-55
	 * generate the (signed) 5-bit splat immediate word,
	 * then double (*2) the word vector,
	 * to generate the 7-bit sim6.
	 * This is one less instruction then the general case. */
	tmp = vec_splat6_u32 ((sim7 >> 1));
	result = vec_add (tmp, tmp);
      }
    else if ((sim7 % 4 == 0) && (sim7 < 64))
      {
	/* To cover modulo 4 values for range 32-60.
	 * Divide sim7 by 4 (sim7 >> 1), range is 8-15
	 * generate the (signed) 5-bit splat immediate word,
	 * then multiple (*4) the word vector,
	 * to generate the 7-bit sim7.
	 * This is one less instruction then the general case. */
	const vui32_t v2 = vec_splat_u32(2);
	tmp = vec_splat_u32(sim7 >> 2);

	result = vec_sl (tmp, v2);
      }
    else if ((sim7 % 4 == 0) && (sim7 > 64))
      {
	/* To cover modulo 4 values for range 68-92.
	 * Divide sim7 by 4 (sim7 >> 2), range is 17-23 (-15 to -9)
	 * generate the (signed) 5-bit splat immediate word,
	 * then muliply (*4) the word vector,
	 * to generate the 7-bit sim7.
	 * This is one less instruction then the general case. */
	const vui32_t v2 = vec_splat_u32(2);
	tmp = vec_splat_u32((sim7 >> 2) - 32);

	result = vec_sl (tmp, v2);
      }
    // PVECLIB_AVOID_RODATA
#if (defined (__GNUC__) && (__GNUC__ > 7))
    else if ((sim7 > 16) && (sim7 < 64))
      {
	const vui32_t vm4w = vec_splat_u32(sim7 % 4);
	const vui8_t vd4b = vec_splat_u8(sim7 / 4);
	// result == ((sim7 / 4) * 4) + (sim7 % 4)
	return vec_sum4s (vd4b, vm4w);
      }
    else if ((sim7 > 96) && (sim7 < 112))
      {
	const vui32_t vm4w = vec_splat_u32(sim7-112);
	const vui8_t vd4b = vec_splat_u8(-4);
	// result == ((-4) * 4) + (sim7 - 112)
	return vec_sum4s (vd4b, vm4w);
      }
    else if ((sim7 > 80) && (sim7 < 96))
      {
	const vui32_t vm4w = vec_splat_u32(sim7-96);
	const vui8_t vd4b = vec_splat_u8(-8);
	// result == ((-8) * 4) + (sim7 - 96)
	return vec_sum4s (vd4b, vm4w);
      }
    else if ((sim7 > 64) && (sim7 < 80))
      {
	const vui32_t vm4w = vec_splat_u32(sim7-80);
	const vui8_t vd4b = vec_splat_u8(-12);
	// result == ((-12) * 4) + (sim7 - 80)
	return vec_sum4s (vd4b, vm4w);
      }
#else
    else if ((sim7 > 16) && (sim7 < 32))
      {
	const vui32_t v16 = vec_splat_u32(-16);
	/* To cover the odd numbers 97-111.
	 * Subtract 112 (sim7-112) to reduce the range,
	 * generate the (signed) 5-bit splat immediate word,
	 * then vec_add (byte) the 112  (-16) back,
	 * to generate the 7-bit shift count. */
	tmp = vec_splat_u32(sim7 - 16);
	result = vec_sub (tmp, v16);
      }
    else if ((sim7 > 96) && (sim7 < 112))
      {
	const vui32_t v16 = vec_splat_u32(-16);
	/* To cover the odd numbers 97-111.
	 * Subtract 112 (sim7-112) to reduce the range,
	 * generate the (signed) 5-bit splat immediate word,
	 * then vec_add (byte) the 112  (-16) back,
	 * to generate the 7-bit shift count. */
	tmp = vec_splat_u32(sim7 - 112);
	result = vec_add (tmp, v16);
      }
#if (defined (__GNUC__) && (__GNUC__ < 8))
    /* GCC v8+ will convert the following sequences to constant
     * vector in .rodata and replace this sequence with a load.
     * While these sequences run 4-5 instruction they complete
     * in 6-8 (dual issue) cycles. This is still better than a
     * 7-13 cycle vector load from .rodata. */
    // PVECLIB_AVOID_RODATA
    else if ((sim7 > 32) && (sim7 < 48))
      {
	const vui32_t v16 = vec_splat_u32(-16);
	const vui32_t v32 = vec_add (v16, v16);
	/* To cover the odd numbers 33-47.
	 * Subtract 32 (sim7-32) to reduce the range,
	 * generate the (signed) 5-bit splat immediate word,
	 * then vec_sub (byte) the   (-32) back,
	 * to generate the 7-bit shift count. */
	tmp = vec_splat_u32(sim7 - 32);
	result = vec_sub (tmp, v32);
      }
    else if ((sim7 > 80) && (sim7 < 96))
      {
	const vui32_t v16 = vec_splat_u32(-16);
	const vui32_t v32 = vec_add (v16, v16);
	/* To cover the odd numbers 81-95.
	 * Subtract 96 (sim7-96) to reduce the range,
	 * generate the (signed) 5-bit splat immediate word,
	 * then vec_add (byte) the 96  (-32) back,
	 * to generate the 7-bit shift count. */
	tmp = vec_splat_u32(sim7 - 96);
	result = vec_add (tmp, v32);
      }
    else if ((sim7 > 48) && (sim7 < 64))
      {
	const vui32_t v16 = vec_splat_u32(-16);
	const vui32_t v32 = vec_add (v16, v16);
	/* To cover the odd numbers 49-63.
	 * Subtract 48 (sim7-48) to reduce the range,
	 * generate the (signed) 5-bit splat immediate word,
	 * then vec_sub (byte) the   (-48) back,
	 * to generate the 7-bit shift count. */
	tmp = vec_splat_u32(sim7 - 48);
	// Allow the v32 and 1st result adds execute in parallel
	result = vec_sub (tmp, v16);
	result = vec_sub (result, v32);
      }
    else if ((sim7 > 64) && (sim7 < 80))
      {
	const vui32_t v16 = vec_splat_u32(-16);
	const vui32_t v32 = vec_add (v16, v16);
	/* To cover the odd numbers 81-95.
	 * Subtract 96 (sim7-96) to reduce the range,
	 * generate the (signed) 5-bit splat immediate word,
	 * then vec_add (byte) the 96  (-32) back,
	 * to generate the 7-bit shift count. */
	tmp = vec_splat_u32(sim7 - 80);
	// Allow the v32 and 1st result adds execute in parallel
	result = vec_add (tmp, v16);
	result = vec_add (result, v32);
      }
#endif
#endif
    else
      /* The rest including:
       * Odd values 33 to 95
       * Even values 34 to 94, not divisible by 4.
       * Require a 4-5 instruction sequences or a load.*/
      result = vec_splats ((unsigned int) sim7);
  }
#endif
  else
    vec_splats ((unsigned char) sim7);

  return result;
}

/** \brief Vector Splat (6-bit)Immediate Doubleword .
 *
 *  Small constants are need for shift/rotate operations which require
 *  5/6/7-bit shift counts. The sim6 constant value is splatted to
 *  the low-order 6-bits for each DW. This exactly matches the
 *  requirements of the Doubleword vector shift/rotate instructions
 *  as only the low-order 6-bits of each Doubleword is used.
 *
 *  The high-order 58-bits of each Byte are <B>Boundedly Undefined</B>.
 *  They are either sign or 0 extended based on the target ISA and
 *  the whim of the developer. This value is replicated across
 *  the 2 doubleword elements.
 *
 *  \note Before PowerISA 3.0 (power9) only Vector Splat Immediate
 *  Signed Byte/Halfword/Word with a 5-bit Signed Immediate (SIM)
 *  or loading a vector constant from storage are available.
 *  The load is at least 3 instructions and 9 cycles latency.
 *  So a short sequence of (2 cycle) vector splat immediate and
 *  integer arithmetic operations can provide better performance
 *  (less then 9 cycles latency).
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-8   | 2/cycle  |
 *  |power9   | 5-5   | 2/cycle  |
 *
 *  @param sim6 shift amount in the range 0-63.
 *  @return 128-bit vector unsigned long int, with const sim6 placed
 *  in the low-order 6 bits of each Doubleword element.
 */
static inline vui64_t
vec_splat6_u64 (const unsigned int sim6)
{
  vui64_t result;
#if defined (_ARCH_PWR10) && (defined (__GNUC__) && (__GNUC__ >= 11))
  result = (vui64_t) vec_splati ((signed int)sim6);
#elif defined(_ARCH_PWR9) && (__GNUC__ > 10)
  result = (vui64_t) vec_splats ((unsigned int)sim6);
#else // defined(_ARCH_PWR8)
  vui32_t rword;
  if (__builtin_constant_p (sim6) && (sim6 < 16))
    rword = vec_splat_u32(sim6);
  else
    {
      if (__builtin_constant_p (sim6) && (sim6 > 47))
	// 6-bit shift count in the range 48-63
	rword = vec_splat_u32(sim6 - 64);
      else
	{ // 6-bit shift count in the range 16-47
	  vui32_t tmp;
	  if (__builtin_constant_p (sim6) && (sim6 % 2 == 0))
	    {
	      /* To cover even values for range 16-46.
	       * Divide sim6 by 2 (sim6 >> 1), range is 8-23
	       * generate the (signed) 5-bit splat immediate word,
	       * then double (*2) the word vector,
	       * to generate the 6-bit sim6.
	       * This is one less instruction then the general case. */
	      if (__builtin_constant_p (sim6) && (sim6 < 32))
		tmp = vec_splat_u32(sim6 >> 1);
	      else
		tmp = vec_splat_u32((sim6 >> 1) - 32);

	      rword = vec_add (tmp, tmp);
	    }
	  else
	    {
	      vui32_t v16 = vec_splat_u32(-16);
	      /* To cover the range 17-47.
	       * Subtract 16 (sim6-16) to reduce the range,
	       * generate the (signed) 5-bit splat immediate word,
	       * then vec_add (word) the 16 back,
	       * to generate the 6-bit shift count. */
	      if (__builtin_constant_p (sim6) && (sim6 < 32))
		{
		  tmp = vec_splat_u32(sim6 - 16);
		  rword = vec_sub (tmp, v16);
		}
	      else
		{
		  tmp = vec_splat_u32(sim6 - 48);
		  rword = vec_add (tmp, v16);
		}
	    }
	}
    }
#if 0
  result = (vui64_t) rword;
#else
#if defined(_ARCH_PWR8)
//  result = (vui64_t) vec_unpackh ((vi32_t)rword);
#if defined (__GNUC__) && ((__GNUC__ == 8))
	  // GCC 8 (AT12) handle this correctly.
	  result = (vui64_t) vec_vupklsw ((vi32_t)rword);
#else
	  // But GCC 9-13 optimized the above to be load from .rodata.
	  // With a little register pressure it adds some gratuitous store/reloads.
	  // So the following work-around is required.
	  __asm__(
	      "vupklsw %0,%1;"
	      : "=v" (result)
	      : "v" (rword)
	      : );
#endif
#else //  defined(_ARCH_PWR7)
  result = (vui64_t) vec_unpackh ((vi16_t)rword);
#endif
#endif

#endif

  return result;
}

/** \brief Vector Splat (6-bit) Signed Immediate Doubleword .
 *
 *  Small constants are need for shift/rotate operations which require
 *  5/6/7-bit shift counts. The sim6 constant value is splatted to
 *  the low-order 6-bits for each DW. This exactly matches the
 *  requirements of the Doubleword vector shift/rotate instructions
 *  as only the low-order 6-bits of each Doubleword is used.
 *
 *  \note Before PowerISA 3.0 (power9) only Vector Splat Immediate
 *  Signed Byte/Halfword/Word with a 5-bit Signed Immediate (SIM)
 *  or loading a vector constant from storage are available.
 *  The load is at least 3 instructions and 9 cycles latency.
 *  So a short sequence of (2 cycle) vector splat immediate and
 *  integer arithmetic operations can provide better performance
 *  (less then 9 cycles latency).
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-8   | 2/cycle  |
 *  |power9   | 5-5   | 2/cycle  |
 *
 *  @param sim6 shift amount in the range 0-63.
 *  @return 128-bit vector unsigned long int, with const sim6 placed
 *  in the low-order 6 bits of each Doubleword element.
 */
static inline vi64_t
vec_splat6_s64 (const signed int sim6)
{
  vi64_t result;
#if defined (_ARCH_PWR10) && (defined (__GNUC__) && (__GNUC__ >= 11))
  result = (vi64_t) vec_splati ((signed int)sim6);
#elif defined(_ARCH_PWR9) && (__GNUC__ > 10)
  result = (vi64_t) vec_splats ((signed long long)sim6);
#else // defined(_ARCH_PWR8)
  vi32_t rword;
  rword = vec_splat6_s32 (sim6);
#if defined(_ARCH_PWR8)
#if defined (__GNUC__) && ((__GNUC__ == 8))
	  // GCC 8 (AT12) handle this correctly.
	  result = (vi64_t) vec_vupklsw ((vi32_t)rword);
#else
	  // But GCC 9-13 optimized the above to be load from .rodata.
	  // With a little register pressure it adds some gratuitous store/reloads.
	  // So the following work-around is required.
	  __asm__(
	      "vupklsw %0,%1;"
	      : "=v" (result)
	      : "v" (rword)
	      : );
#endif
#else //  defined(_ARCH_PWR7)
  result = (vi64_t) vec_unpackh ((vi16_t)rword);
#endif
#endif
  return result;
}

/** \brief Vector Subtract Unsigned Doubleword Modulo for POWER7 and earlier.
 *
 *  For each unsigned long (64-bit) integer element r[i] = vra[i] +
 *  NOT(vrb[i]) + 1.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the Vector Subtract
 *  Unsigned Doubleword Modulo (<B>vsubudm</B>) instruction.
 *  Otherwise use this implementation
 *  requiring only simple Vector (VMX) instructions.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 20-27 |   NA     |
 *
 *  @param vra 128-bit vector treated as 2 X unsigned long long int.
 *  @param vrb 128-bit vector treated as 2 X unsigned long long int.
 *  @return  vector unsigned long int sum of vra[0] + NOT(vrb[0]) + 1
 *  and vra[1] + NOT(vrb[1]) + 1.
 */
static inline vui64_t
vec_subudm_PWR7 (vui64_t vra, vui64_t vrb)
{
  const vui32_t vone = vec_splat_u32 (1);
  const vui32_t vzero = vec_splat_u32 (0);
#if 1
  const vui32_t cm = vec_mergeh (vzero, vone);
#else
  const vui32_t cm= { 0,1,0,1};
  // const vui32_t cm = (vui32_t) vec_unpackh ((vi16_t) vone);
#endif
  vui32_t r;
  vui32_t c;

  // propagate carry/borrow from words 1/3 into words 0/2
  c = vec_vsubcuw ((vui32_t)vra, (vui32_t)vrb);
  r = vec_vsubuwm ((vui32_t)vra, (vui32_t)vrb);
  // Ignore carry's from words 0/2
  c = vec_andc (cm, c);
  // rotate Words 1/3 carry's into position for words 0/2 extend.
  c = vec_sld (c, c, 4);
  // Add/extend carry's to words 0/2
  return ((vui64_t) vec_vsubuwm (r, c));
}

/** \brief Vector Subtract Unsigned Quadword Modulo for POWER7 and earlier.
 *
 *  Subtract two vector __int128 values and return result modulo 128-bits.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the Vector Subtract
 *  Unsigned Doubleword Modulo (<B>vsubuqm</B>) instruction.
 *  Otherwise use this implementation
 *  requiring only simple Vector (VMX) instructions.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 24-31 |   NA     |
 *
 *  @param vra 128-bit vector treated as unsigned __int128.
 *  @param vrb 128-bit vector treated as unsigned __int128.
 *  @return __int128 unsigned difference of vra minus vrb.
 */
static inline vui128_t
vec_subuqm_PWR7 (vui128_t vra, vui128_t vrb)
{
  vui32_t t;
  vui32_t c2, c;
  vui32_t z  = { 0,0,0,0};
  vui32_t co = { 1,1,1,1};
  vui32_t _b = vec_nor ((vui32_t) vrb, (vui32_t) vrb);

  /* vsubuqm is defined as vra + NOT(vrb) + 1.
  // or vec_addeuqm (vra, (vui128_t) _b, (vui128_t) {1}); */

  c2 = vec_vaddcuw ((vui32_t)vra, (vui32_t)_b);
  t = vec_vadduwm ((vui32_t)vra, (vui32_t)_b);
  // inject the extend carry.
  c = vec_sld (c2, co, 4);
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

  return ((vui128_t) t);
}

/** \brief Vector Expand Byte Mask for POWER9/8/7 and earlier.
 *
 *  Create word element masks based on high-order (sign) bit of
 *  the input byte elements.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 2 - 4 | 2/cycle  |
 *  |power8   | 2 - 4 | 2/cycle  |
 *  |power9   | 3 - 6 | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned char.
 *  @return vector word mask from the sign bit.
 */
static inline vui8_t
vec_vexpandbm_PWR7 (vui8_t vra)
{
  const vi8_t zero = vec_splat_s8( 0 );
  vb8_t vsgn;

  // expand mask word (using signed compare)
  vsgn = vec_cmpgt (zero, (vi8_t) vra);

  return ((vui8_t) vsgn);
}

/** \brief Vector Expand Byte Mask for POWER10.
 *
 *  Create word element masks based on high-order (sign) bit of
 *  the input byte elements.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 2 - 4 | 2/cycle  |
 *  |power8   | 2 - 4 | 2/cycle  |
 *  |power9   | 3 - 6 | 2/cycle  |
 *  |power10  | 3 - 4 | 4/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned char.
 *  @return vector word mask from the sign bit.
 */
static inline vui8_t
vec_vexpandbm_PWR10 (vui8_t vra)
{
  vui8_t result;
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
#if (__GNUC__ >= 11)
      result = vec_expandm (vra);
#else
  __asm__(
      "vexpandbm %0,%1;\n"
      : "=v" (result)
      : "v" (vra)
      : );
#endif
#else
  result = vec_vexpandbm_PWR7 (vra);
#endif
  return (result);
}

/** \brief Vector Expand Halfword Mask for POWER9/8/7 and earlier.
 *
 *  Create word element masks based on high-order (sign) bit of
 *  the input halfword elements.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 2 - 4 | 2/cycle  |
 *  |power8   | 2 - 4 | 2/cycle  |
 *  |power9   | 3 - 6 | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned short.
 *  @return vector word mask from the sign bit.
 */
static inline vui16_t
vec_vexpandhm_PWR7 (vui16_t vra)
{
  const vi16_t zero = vec_splat_s16( 0 );
  vb16_t vsgn;

  // expand mask word (using signed compare)
  vsgn = vec_cmpgt (zero, (vi16_t) vra);

  return ((vui16_t) vsgn);
}

/** \brief Vector Expand Halfword Mask for POWER10.
 *
 *  Create word element masks based on high-order (sign) bit of
 *  the input halfword elements.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 2 - 4 | 2/cycle  |
 *  |power8   | 2 - 4 | 2/cycle  |
 *  |power9   | 3 - 6 | 2/cycle  |
 *  |power10  | 3 - 4 | 4/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned short.
 *  @return vector word mask from the sign bit.
 */
static inline vui16_t
vec_vexpandhm_PWR10 (vui16_t vra)
{
  vui16_t result;
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
#if (__GNUC__ >= 11)
      result = vec_expandm (vra);
#else
  __asm__(
      "vexpandhm %0,%1;\n"
      : "=v" (result)
      : "v" (vra)
      : );
#endif
#else
  result = vec_vexpandhm_PWR7 (vra);
#endif
  return (result);
}

/** \brief Vector Expand Word Mask for POWER9/8/7 and earlier.
 *
 *  Create word element masks based on high-order (sign) bit of
 *  the input word elements.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 2 - 4 | 2/cycle  |
 *  |power8   | 2 - 4 | 2/cycle  |
 *  |power9   | 3 - 6 | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned int.
 *  @return vector word mask from the sign bit.
 */
static inline vui32_t
vec_vexpandwm_PWR7 (vui32_t vra)
{
  const vi32_t zero = vec_splat_s32( 0 );
  vb32_t vsgn;

  // expand mask word (using signed compare)
  vsgn = vec_cmpgt (zero, (vi32_t) vra);

  return ((vui32_t) vsgn);
}

/** \brief Vector Expand Word Mask for POWER10.
 *
 *  Create word element masks based on high-order (sign) bit of
 *  the input word elements.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 2 - 4 | 2/cycle  |
 *  |power8   | 2 - 4 | 2/cycle  |
 *  |power9   | 3 - 6 | 2/cycle  |
 *  |power10  | 3 - 4 | 4/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned int.
 *  @return vector word mask from the sign bit.
 */
static inline vui32_t
vec_vexpandwm_PWR10 (vui32_t vra)
{
  vui32_t result;
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
#if (__GNUC__ >= 11)
      result = vec_expandm (vra);
#else
  __asm__(
      "vexpandwm %0,%1;\n"
      : "=v" (result)
      : "v" (vra)
      : );
#endif
#else
  result = vec_vexpandwm_PWR7 (vra);
#endif
  return (result);
}

/** \brief Vector Expand Doubleword Mask for POWER7 and earlier.
 *
 *  Create doubleword element masks based on high-order (sign) bit of
 *  the input doubleword elements.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 6 - 8 | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned long long.
 *  @return vector doubleword mask from the sign bit.
 */
static inline vui64_t
vec_vexpanddm_PWR7 (vui64_t vra)
{
  const vi8_t zero = vec_splat_s8 (0);
  // Splat the doubleword sign byte across the quadwords
  vi8_t vra_sign_h = vec_splat ((vi8_t) vra, VEC_BYTE_H_DWH);
  vi8_t vra_sign_l = vec_splat ((vi8_t) vra, VEC_BYTE_L_DWH);
  vi8_t vra_sign = (vi8_t) vec_xxpermdi ((vui64_t)vra_sign_h, (vui64_t)vra_sign_l, 0);

  vb8_t vsgn;

  // expand mask bytes (using signed compare)
  vsgn = vec_cmpgt ((vi8_t) zero, vra_sign);

  return ((vui64_t) vsgn);
}

/** \brief Vector Expand Doubleword Mask for POWER9/8.
 *
 *  Create doubleword element masks based on high-order (sign) bit of
 *  the input doubleword elements.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2 - 4 | 2/cycle  |
 *  |power9   | 3 - 6 | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned long long.
 *  @return vector doubleword mask from the sign bit.
 */
static inline vui64_t
vec_vexpanddm_PWR8 (vui64_t vra)
{
#if defined (_ARCH_PWR8)  && (__GNUC__ >= 6)
  const vi64_t zero = (vi64_t) {0, 0};
  vb64_t vsgn;

  // expand mask doubleword (using signed compare)
  vsgn = vec_cmpgt (zero, (vi64_t) vra);

  return ((vui64_t) vsgn);
#else
  return vec_vexpanddm_PWR7 (vra);
#endif
}

/** \brief Vector Expand Doubleword Mask for POWER10.
 *
 *  Create an doubleword element masks based on high-order (sign) bit of
 *  the input doubleword elements.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 6 - 8 | 2/cycle  |
 *  |power8   | 2 - 4 | 2/cycle  |
 *  |power9   | 3 - 6 | 2/cycle  |
 *  |power10  | 3 - 4 | 4/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned long long.
 *  |processor|Latency|Throughput|
 *  @return vector doubleword mask from the sign bit.
 */
static inline vui64_t
vec_vexpanddm_PWR10 (vui64_t vra)
{
  vui64_t result;
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
#if (__GNUC__ >= 11)
      result = vec_expandm (vra);
#else
  __asm__(
      "vexpanddm %0,%1;\n"
      : "=v" (result)
      : "v" (vra)
      : );
#endif
#else
  result = vec_vexpanddm_PWR8 (vra);
#endif
  return (result);
}

/** \brief Vector Expand Quadword Mask for POWER9/8/7 and earlier.
 *
 *  Create an quadword element mask based on high-order (sign) bit of
 *  the input quadword.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   | 4 - 6 | 2/cycle  |
 *  |power8   | 4 - 6 | 2/cycle  |
 *  |power9   | 6 - 9 | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @return vector quadword mask from the sign bit.
 */
static inline vui128_t
vec_vexpandqm_PWR7 (vui128_t vra)
{
  // Splat the sign byte across the quadword
#if 1
  vui8_t vra_sign = vec_splat ((vui8_t) vra, VEC_BYTE_H);
  return (vui128_t) vec_vexpandbm_PWR7 (vra_sign);
#else
  vi8_t vra_sign = vec_splat ((vi8_t) vra, VEC_BYTE_H);
  const vi8_t zero = vec_splat_s8 (0);
  vb8_t vsgn;

  // expand mask byte (using signed compare)
  vsgn = vec_cmpgt ((vi8_t) zero, vra_sign);

  return ((vui128_t) vsgn);
#endif
}

/** \brief Vector Expand Quadword Mask for POWER10.
 *
 *  Create an quadword element mask based on high-order (sign) bit of
 *  the input quadword.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power10  | 3 - 4 | 4/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @return vector quadword mask from the sign bit.
 */
static inline vui128_t
vec_vexpandqm_PWR10 (vui128_t vra)
{
  vui128_t result;
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
#if (__GNUC__ >= 11)
      result = vec_expandm (vra);
#else
  __asm__(
      "vexpandqm %0,%1;\n"
      : "=v" (result)
      : "v" (vra)
      : );
#endif
#else
  result = vec_vexpandqm_PWR7 (vra);
#endif
  return (result);
}

/** \brief Vector Rotate Left Doubleword by Byte for POWER7 and earlier.
 *
 *  Vector Rotate Left Doubleword 0-63 bits.
 *  The shift amount is from bits 58-63 and 122-127 of vrb.
 *
 *  \note The PowerISA only requires the low order 6-bits of each
 *  doubleword as the shift count. So there is no reason to force
 *  the shift count to conform to be a unsigned long long. Also it is
 *  much easier to splat byte constants than doubleword constants.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   |  8-10 | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as 2 x unsigned long integers.
 *  @param vrb shift amount in bits 58:63 and 122:127.
 *  @return Left shifted vector unsigned long long.
 */
static inline vui64_t
vec_vrld_PWR7 (vui64_t vra, vui8_t vrb)
{
  vui8_t vb2 = vec_splat_u8(2);
  vui8_t vones =  vec_splat_u8(-1);
  // shft_mask == (63, ... }
  vui8_t shft_mask = vec_sr (vones, vb2);
  vui64_t r;
  vui64_t hd, ld;
  vui32_t t1, t2;
  vui8_t shh, shl;

  /* constrain the dword shift amounts to 0-63.  */
  vrb = vec_and ((vui8_t) vrb, (vui8_t) shft_mask);
  shh = vec_splat ((vui8_t) vrb, VEC_BYTE_L_DWH);
  shl = vec_splat ((vui8_t) vrb, VEC_BYTE_L_DWL);

  //hd = vec_xxspltd (vra, VEC_DW_H);
  hd = vec_xxpermdi (vra, vra, 0);
  //ld = vec_xxspltd (vra, VEC_DW_L);
  ld = vec_xxpermdi (vra, vra, 3);
  t1 = vec_vslo ((vui32_t)hd, shh);
  t2 = vec_vslo ((vui32_t)ld, shl);
  t1 = vec_vsl (t1, shh);
  t2 = vec_vsl (t2, shl);
  //r = vec_mrghd ((vui64_t)t1, (vui64_t)t2);
  r = vec_xxpermdi ((vui64_t)t1, (vui64_t)t2, 0);
  return (r);
}

/** \brief Vector Rotate Left Doubleword by Byte for POWER8 and earlier.
 *
 *  Vector Rotate Left Doubleword 0-63 bits.
 *  The shift amount is from bits 58-63 and 122-127 of vrb.
 *
 *  \note The PowerISA only requires the low order 6-bits of each
 *  doubleword as the shift count. So there is no reason to force
 *  the shift count to conform to be a unsigned long long. Also it is
 *  much easier to splat byte constants than doubleword constants.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power7   |  8-10 | 1/cycle  |
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   2   | 2/cycle  |
 *  |power10  |  1-3  | 4/cycle  |
 *
 *  @param vra a 128-bit vector treated as 2 x unsigned long integers.
 *  @param vrb shift amount in bits 58:63 and 122:127.
 *  @return Left shifted vector unsigned long long.
 */
static inline vui64_t
vec_vrld_PWR8 (vui64_t vra, vui8_t vrb)
{
  vui64_t r;
#ifdef _ARCH_PWR8
#ifdef __clang__
  r = vec_rl (vra, (vui64_t) vrb);
#else
  __asm__(
      "vrld %0,%1,%2;"
      : "=v" (r)
      : "v" (vra),
	"v" (vrb)
      : );
#endif
#else
  r = vec_vrld_PWR7 (vra, vrb);
#endif
  return (r);
}

/** \brief Vector Shift Left Doubleword by Byte for POWER7 and earlier.
 *
 *  Vector Shift Left Doubleword 0-63 bits.
 *  The shift amount is from bits 58-63 and 122-127 of vrb.
 *
 *  \note The PowerISA only requires the low order 6-bits of each
 *  doubleword as the shift count. So there is no reason to force
 *  the shift count to conform to be a unsigned long long. Also it is
 *  much easier to splat byte constants than doubleword constants.
 *
 *  @param vra a 128-bit vector treated as 2 x unsigned long integers.
 *  @param vrb shift amount in bits 58:63 and 122:127.
 *  @return Left shifted vector unsigned long long.
 */
static inline vui64_t
vec_vsld_PWR7 (vui64_t vra, vui8_t vrb)
{
  vui8_t vzero =  vec_splat_u8(0);
  vui8_t vb2 = vec_splat_u8(2);
  vui8_t vones =  vec_splat_u8(-1);
  // shft_mask == (63, ... }
  vui8_t shft_mask = vec_sr (vones, vb2);
  // vui64_t sel_mask = CONST_VINT128_DW(0, -1LL);
  vui8_t sel_mask = vec_sld (vzero, vones, 8);
  vui64_t result;

  vui8_t  vsh_h, vsh_l;
  vui8_t  vr_h, vr_l;

  /* constrain the dword shift amounts to 0-63.  */
  vsh_l = vec_and ((vui8_t) vrb, (vui8_t) shft_mask);
  /* Isolate the high dword so that bits from the low dword
   * do not contaminate the result.  */
  vr_h = vec_andc ((vui8_t) vra, (vui8_t) sel_mask);
  /* The low dword is just vra as the 128-bit shift left generates
   * '0's on the right and the final merge (vec_sel)
   * cleans up 64-bit overflow on the left.  */
  vr_l  = (vui8_t) vra;
  /* The vsr instruction only works correctly if the bit shift
   * value is splatted to each byte of the vector.  */
  vsh_h = vec_splat (vsh_l, VEC_BYTE_L_DWH);
  vsh_l = vec_splat (vsh_l, VEC_BYTE_L_DWL);
  /* Shift the high dword by vsh_h.  */
  vr_h = vec_vslo (vr_h,  vsh_h);
  vr_h = vec_vsl  (vr_h, vsh_h);
  /* Shift the low dword by vsh_l.  */
  vr_l = vec_vslo (vr_l,  vsh_l);
  vr_l = vec_vsl  (vr_l, vsh_l);
  /* Merge the dwords after shift.  */
  result = (vui64_t) vec_sel (vr_h, vr_l, (vui8_t) sel_mask);

  return ((vui64_t) result);
}

/** \brief Vector Shift Left Doubleword by Byte for POWER8 and earlier.
 *
 *  Vector Shift Left Doubleword 0-63 bits.
 *  The shift amount is from bits 58-63 and 122-127 of vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   2   | 2/cycle  |
 *  |power10  |  1-3  | 4/cycle  |
 *
 *  \note The PowerISA only requires the low order 6-bits of each
 *  doubleword as the shift count. So there is no reason to force
 *  the shift count to conform to be a unsigned long long. Also it is
 *  much easier to splat byte constants than doubleword constants.
 *
 *  @param vra a 128-bit vector treated as 2 x unsigned long integers.
 *  @param vrb shift amount in bits 58:63 and 122:127.
 *  @return Left shifted vector unsigned long long.
 */
static inline vui64_t
vec_vsld_PWR8 (vui64_t vra, vui8_t vrb)
{
  vui64_t result;

#ifdef _ARCH_PWR8
#ifdef __clang__
  result = vec_sl (vra, (vui64_t) vrb);
#else
  __asm__(
      "vsld %0,%1,%2;"
      : "=v" (result)
      : "v" (vra),
      "v" (vrb)
      : );
#endif
#else
  result = vec_vsld_PWR7 (vra, vrb);
#endif
  return ((vui64_t) result);
}

/** \brief Vector Shift Right Doubleword by Byte for POWER7 and earlier.
 *
 *  Vector Shift Right Doubleword 0-63 bits.
 *  The shift amount is from bits 58-63 and 122-127 of vrb.
 *
 *  \note The PowerISA only requires the low order 6-bits of each
 *  doubleword as the shift count. So there is no reason to force
 *  the shift count to conform to be a unsigned long long. Also it is
 *  much easier to splat byte constants than doubleword constants.
 *
 *  @param vra a 128-bit vector treated as 2 x unsigned long integers.
 *  @param vrb shift amount in bits 58:63 and 122:127.
 *  @return Right shifted vector unsigned long.
 */
static inline vui64_t
vec_vsrd_PWR7 (vui64_t vra, vui8_t vrb)
{
  vui64_t result;
  vui8_t  vsh_h, vsh_l;
  vui8_t  vr_h, vr_l;
  vui64_t sel_mask = CONST_VINT128_DW(0, -1LL);
  vui64_t shft_mask = CONST_VINT128_DW(63, 63);

  /* constrain the dword shift amounts to 0-63.  */
  vsh_l = vec_and ((vui8_t) vrb, (vui8_t) shft_mask);
  /* Isolate the low dword so that bits from the high dword,
   * do not contaminate the result.  */
  vr_l  = vec_and ((vui8_t) vra, (vui8_t) sel_mask);
  /* The vsr instruction only works correctly if the bit shift
   * value is splatted to each byte of the vector.  */
  vsh_h = vec_splat (vsh_l, VEC_BYTE_L_DWH);
  vsh_l = vec_splat (vsh_l, VEC_BYTE_L_DWL);
  /* Shift the high dword by vsh_h.  */
  vr_h = vec_vsro ((vui8_t) vra,  vsh_h);
  vr_h = vec_vsr  (vr_h, vsh_h);
  /* Shift the low dword by vsh_l.  */
  vr_l = vec_vsro (vr_l,  vsh_l);
  vr_l = vec_vsr  (vr_l, vsh_l);
  /* Merge the dwords after shift.  */
  result = (vui64_t) vec_sel (vr_h, vr_l, (vui8_t) sel_mask);

  return ((vui64_t) result);
}

/** \brief Vector Shift Right Doubleword by Byte for POWER8 and earlier.
 *
 *  Vector Shift Right Doubleword 0-63 bits.
 *  The shift amount is from bits 58-63 and 122-127 of vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   2   | 2/cycle  |
 *  |power10  |  1-3  | 4/cycle  |
 *
 *  \note The PowerISA only requires the low order 6-bits of each
 *  doubleword as the shift count. So there is no reason to force
 *  the shift count to conform to be a unsigned long long. Also it is
 *  much easier to splat byte constants than doubleword constants.
 *
 *  @param vra a 128-bit vector treated as 2 x unsigned long integers.
 *  @param vrb shift amount in bits 58:63 and 122:127.
 *  @return Right shifted vector unsigned long.
 */
static inline vui64_t
vec_vsrd_PWR8 (vui64_t vra, vui8_t vrb)
{
  vui64_t result;

#ifdef _ARCH_PWR8
#ifdef __clang__
  result = vec_sr (vra, (vui64_t) vrb);
#else
  __asm__(
      "vsrd %0,%1,%2;"
      : "=v" (result)
      : "v" (vra),
      "v" (vrb)
      : );
#endif
#else
  result = vec_vsrd_PWR7 (vra, vrb);
#endif
  return ((vui64_t) result);
}

/** \brief Vector Rotate Left Quadword by Byte for POWER9 and earlier.
 *
 *  Vector Shift Right Quadword 0-127 bits. The shift count is splatted
 *  into the low-order 7 bits of each byte of vrb.
 *
 *  \note xxspltib is the simplest way to generate a shift count for
 *  vslq. Or vspltb from byte 0 if the shift count originated in VRB
 *  bits 121:127. The PowerISA only looks at bits 121:124 for vslo and
 *  bits 5:7 of each byte for vsl. All other bits in VRB are ignored.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  10   | 1/cycle  |
 *  |power9   |  14   | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @param vrb Shift amount in bits 1:7 of each byte.
 *  @return vector quadword shifted left.
 */
static inline vui128_t
vec_vrlq_PWR9 (vui128_t vra, vui8_t vrb)
{
  vui8_t result, vt1, vt2, rsh;
  vui8_t vr8 = (vui8_t) vra;
  const vui8_t vzero = vec_splat_u8 (0);
  // Assume that vrb is already byte splatted with the shift count
  // Compute the right shift count
  rsh = vec_sub (vzero, vrb);

  vt1 = vec_slo (vr8, vrb);
  vt1 = vec_sll (vt1, vrb);
  vt2 = vec_sro ((vui8_t) vr8, rsh);
  vt2 = vec_srl (vt2, rsh);
  result = vec_or (vt1, vt2);

  return (vui128_t) result;
}

/** \brief Vector Rotate Left Quadword by Byte for POWER10 and earlier.
 *
 *  Vector Rotate Left Quadword 0-127 bits.
 *  The shift amount is from bits 57:63 of vrb.
 *
 *  \note xxspltib is the simplest way to generate a shift count for
 *  vrlq. A xxspltiw works as well. Or vspltb from byte 0 if the
 *  shift count is in VRB bits 121:127. The PowerISA only looks at
 *  bits 57:63, all other bits in VRB are ignored.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  10   | 1/cycle  |
 *  |power9   |  14   | 1/cycle  |
 *  |power10  | 6 - 8 | 4/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @param vrb Shift amount in bits 57:63.
 *  @return vector quadword shifted left.
 */
static inline vui128_t
vec_vrlq_PWR10 (vui128_t vra, vui8_t vrb)
{
  vui128_t result;

#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
#ifdef __clang__
  result = vec_vrlq_PWR9 (vra, vrb);
#else
  // vrlq takes the shift count from bits 57:63
  __asm__(
      "vrlq %0,%1,%2;\n"
      : "=v" (result)
      : "v" (vra), "v" (vrb)
      : );
#endif
#else
  result =  vec_vrlq_PWR9 (vra, vrb);
#endif
  return ((vui128_t) result);
}

/** \brief Vector Shift Left Quadword by Byte for POWER9 and earlier.
 *
 *  Vector Shift Right Quadword 0-127 bits. The shift count is splatted
 *  into the low-order 7 bits of each byte of vrb.
 *
 *  \note xxspltib is the simplest way to generate a shift count for
 *  vslq. Or vspltb from byte 0 if the shift count originated in VRB
 *  bits 121:127. The PowerISA only looks at bits 121:124 for vslo and
 *  bits 5:7 of each byte for vsl. All other bits in VRB are ignored.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     | 1/cycle  |
 *  |power9   | 6     | 1/cycle  |
 *  |power10  | 6 - 8 | 4/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @param vrb Shift amount in bits 1:7 of each byte.
 *  @return vector quadword shifted left.
 */
static inline vui128_t
vec_vslq_PWR9 (vui128_t vra, vui8_t vrb)
{
  vui128_t result;

#ifdef __clang__
  {
    vui8_t tmp;
    tmp = vec_slo ((vui8_t)vra,  vrb);
    result = (vui128_t) vec_sll (tmp, vrb);
  }
#else
  vui128_t tmp;
  __asm__(
      "vslo %1,%2,%3;\n"
      "vsl  %0,%1,%3;\n"
      : "=v" (result), "=&v" (tmp)
      : "v" (vra), "v" (vrb)
      : );
#endif
  return ((vui128_t) result);
}

/** \brief Vector Shift Left Quadword by Byte for POWER10 and earlier.
 *
 *  Vector Shift Left Quadword 0-127 bits.
 *  The shift amount is from bits 57:63 of vrb.
 *
 *  \note xxspltib is the simplest way to generate a shift count for
 *  vsrq. A xxspltiw works as well. Or vspltb from byte 0 if the
 *  shift count is in VRB bits 121:127. The PowerISA only looks at
 *  bits 57:63, all other bits in VRB are ignored.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     | 1/cycle  |
 *  |power9   | 6     | 1/cycle  |
 *  |power10  | 6 - 8 | 4/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @param vrb Shift amount in bits 57:63.
 *  @return vector quadword shifted left.
 */
static inline vui128_t
vec_vslq_PWR10 (vui128_t vra, vui8_t vrb)
{
  vui128_t result;

#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
#ifdef __clang__
  result = vec_vslq_PWR9 (vra, vrb);
#else
  // vsrq takes the shift count from bits 57:63
  __asm__(
      "vslq %0,%1,%2;\n"
      : "=v" (result)
      : "v" (vra), "v" (vrb)
      : );
#endif
#else
  result =  vec_vslq_PWR9 (vra, vrb);
#endif
  return ((vui128_t) result);
}

/** \brief Vector Shift Right Quadword by Byte for POWER9 and earlier.
 *
 *  Vector Shift Right Quadword 0-127 bits. The shift count is splatted
 *  into the low-order 7 bits of each byte of vrb.
 *
 *  \note xxspltib is the simplest way to generate a shift count for
 *  vsrq. Or vspltb from byte 0 if the shift count originated in VRB
 *  bits 121:127. The PowerISA only looks at bits 121:124 for vsro and
 *  bits 5:7 of each byte for vsr. All other bits in VRB are ignored.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     | 1/cycle  |
 *  |power9   | 6     | 1/cycle  |
 *  |power10  | 6 - 8 | 4/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @param vrb Shift amount in bits 1:7 of each byte.
 *  @return vector quadword shifted right.
 */
static inline vui128_t
vec_vsrq_PWR9 (vui128_t vra, vui8_t vrb)
{
  vui128_t result;

#ifdef __clang__
  {
    vui8_t tmp;
    tmp = vec_sro ((vui8_t)vra,  vrb);
    result = (vui128_t) vec_srl (tmp, vrb);
  }
#else
  vui128_t tmp;
  __asm__(
      "vsro %1,%2,%3;\n"
      "vsr  %0,%1,%3;\n"
      : "=v" (result), "=&v" (tmp)
      : "v" (vra), "v" (vrb)
      : );
#endif
  return ((vui128_t) result);
}

/** \brief Vector Shift Right Quadword by Byte for POWER10 and earlier.
 *
 *  Vector Shift Right Quadword 0-127 bits.
 *  The shift amount is from bits 57:63 of vrb.
 *
 *  \note xxspltib is the simplest way to generate a shift count for
 *  vsrq. A xxspltiw works as well. Or vspltb from byte 0 if the
 *  shift count is in VRB bits 121:127. The PowerISA only looks at
 *  bits 57:63, all other bits in VRB are ignored.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     | 1/cycle  |
 *  |power9   | 6     | 1/cycle  |
 *  |power10  | 6 - 8 | 4/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @param vrb Shift amount in bits 57:63.
 *  @return vector quadword shifted right.
 */
static inline vui128_t
vec_vsrq_PWR10 (vui128_t vra, vui8_t vrb)
{
  vui128_t result;

#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
#ifdef __clang__
  result = vec_vsrq_PWR9 (vra, vrb);
#else
  // vsrq takes the shift count from bits 57:63
  __asm__(
      "vsrq %0,%1,%2;\n"
      : "=v" (result)
      : "v" (vra), "v" (vrb)
      : );
#endif
#else
  result =  vec_vsrq_PWR9 (vra, vrb);
#endif
  return ((vui128_t) result);
}

/** \brief Vector Shift Right Algebraic Quadword by Byte for POWER10.
 *
 *  Vector Shift Right Algebraic Quadword 0-127 bits.
 *  The shift amount is from bits 57:63 of vrb.
 *
 *  \note xxspltib is the simplest way to generate a shift count for
 *  vsraq. A xxspltiw works as well. Or vspltb from byte 0 if the
 *  shift count is in VRB bits 121:127. The PowerISA only looks at
 *  bits 57:63, all other bits in VRB are ignored.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power10  | 6 - 8 | 4/cycle  |
 *
 *  @param vra a 128-bit vector treated as signed __int128.
 *  @param vrb Shift amount in bits 57:63.
 *  @return vector quadword shifted right algebraic.
 */
static inline vi128_t
vec_vsraq_PWR10 (vi128_t vra, vui8_t vrb)
{
  vui8_t result;
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
#ifdef __clang__
  result = vec_sraq_PWR9 (vra, vrb);
#else
  // vsraq takes the shift count from bits 57:63
  __asm__(
      "vsraq %0,%1,%2;\n"
      : "=v" (result)
      : "v" (vra), "v" (vrb)
      : );
#endif
#else
  result = (vui8_t) vec_sraq_PWR9 (vra, vrb);
#endif
  return ((vi128_t) result);
}

/** \brief Transfer a vector unsigned __int128 to __int128 scalar.
*
*  The compiler does not allow direct transfer (assignment or type
*  cast) between __int128 scalars and vector types. Vectors are held
*  in 128-bit VRs (VSRs) and __int128 scalars are held in pair of
*  64-bit GPRs. So this operation requires a transfer between
*  registers of different types/sizes.
*
*  |processor|Latency|Throughput|
*  |--------:|:-----:|:---------|
*  |power8   |  6-7  | 1/cycle  |
*  |power9   |  5-6  | 2/cycle  |
*
*  @param vra a vector unsigned __int128 value.
*  @return The original value returned as a __int128 scalar.
*/
static inline unsigned __int128
vec_transfer_vui128t_to_uint128 (vui128_t vra)
{
  __VEC_U_128 t;
  unsigned __int128 result;
#if defined(_ARCH_PWR8) || defined (__clang__)
  // PWR8/9 should generate Move From VSR Doubleword instructions.
  t.vx1 = vra;
  result = t.ui128;
#else
#ifdef  _ARCH_PWR7
  /* PWR7 and earlier must transfer through storage.  This requires
   * care as we want to avoid load-hit-store flushes in the pipeline.
   * First split the vector into a pair of dword FPRs (vra_u, vra_l). */
  vui64_t vra_u = (vui64_t) vra;
  vui64_t vra_l = vec_xxpermdi ((vui64_t) vra, (vui64_t) vra, 2);
  /* Store this pair as adjacent dwords, followed by a group ending
   * nop. This prevents the hardware from dispatching the stores in the
   * same cycle as the following loads (a guaranteed pipeline flush).
   * Also the load addresses and data size will match these stores and
   * increase the possibility of store forwarding from the store queue.
   */
  __asm__(
      "stxsdx %x2,%y0;"
      "stxsdx %x3,%y1;"
      "ori  2,2,0;"
      : "=Z" (t.ulong.lower),
	"=Z" (t.ulong.upper)
      : "wa" (vra_l), "wa" (vra_u)
      : );
#else //_ARCH_PWR6/970
  /* Just have to go through storage and let the hardware deal with
   * load/store ordering. */
  t.vx1 = vra;
#endif
  // Load the dwords into a pair of GPRs for the __int128 result.
  result = t.ui128;
#endif
  return (result);
}

/** \brief Transfer a __int128 scalar to vector unsigned __int128.
*
*  The compiler does not allow direct transfer (assignment or type
*  cast) between __int128 scalars and vector types. Vectors are held
*  in 128-bit VRs (VSRs) and __int128 scalars are held in pair of
*  64-bit GPRs. So this operation requires a transfer between
*  registers of different types/sizes.
*
*  |processor|Latency|Throughput|
*  |--------:|:-----:|:---------|
*  |power8   |   7   | 1/cycle  |
*  |power9   |   5   | 1/cycle  |
*
*  @param gprp a unsigned __int128 value.
*  @return The original value returned as a vector unsigned__int128.
*/
static inline vui128_t
vec_transfer_uint128_to_vui128t (unsigned __int128 gprp)
{
  __VEC_U_128 t;
  t.ui128 = gprp;
  return t.vx1;
}

/** \brief Extract the low doubleword from a __int128 scalar.
*
*  @param gprp a unsigned __int128 value.
*  @return The low doubleword of __int128.
*/
static inline unsigned long long
scalar_extract_uint64_from_low_uint128 (unsigned __int128 gprp)
{
  __VEC_U_128 t;
  t.ui128 = gprp;
  return t.ulong.lower;
}

/** \brief Extract the high doubleword from a __int128 scalar.
*
*  @param gprp a unsigned __int128 value.
*  @return The high doubleword of __int128.
*/
static inline unsigned long long
scalar_extract_uint64_from_high_uint128 (unsigned __int128 gprp)
{
  __VEC_U_128 t;
  t.ui128 = gprp;
  return t.ulong.upper;
}

/** \brief Insert High/low doublewords into a __int128 scalar.
*
*  @param high doubleword of a __int128.
*  @param low doubleword of a __int128.
*  @return The combined quadword as a __int128 scalar.
*/
static inline unsigned __int128
scalar_insert_uint64_to_uint128 (unsigned long long high,
				 unsigned long long low)
{
  __VEC_U_128 t;
  t.ulong.lower = low;
  t.ulong.upper = high;
  return t.ui128;
}

#endif /* VEC_COMMON_PPC_H_ */
