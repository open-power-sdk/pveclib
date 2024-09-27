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
  // vui32_t cm= { 0,1,0,1};
  const vui32_t cm = (vui32_t) vec_unpackh ((vi16_t) vone);
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
 *  @return __int128 sum of a and b.
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
  // generated const (vui32_t) {23, 23, 23, 23}
  const vui32_t v15 = vec_splat_u32 (15);
  const vui32_t v8 = vec_splat_u32 (8);
  const vui32_t v23 = vec_add (v15, v8);
  // fp5 = 1.0 / 2.0 == 0.5
  const vui32_t vone = vec_splat_u32 (1);
  const vf32_t fp5 = vec_ctf (vone, 1);
  // need a word vector of 158 which is the exponent for 2**31
  // const vui32_t v2_31 = ones & ~(ones>>1);
  const vui32_t ones = vec_splat_u32 (-1);
  const vui32_t v2_31 = vec_vslw (ones, ones);
  // f2_31 = (vector float) 2**31
  const vf32_t f2_31 =  vec_ctf (v2_31, 0);
  vui32_t k, n;
  vf32_t kf;
  // Avoid rounding in floating point conversion
  k = vra & ~(vra >> 1);
  // Handle all 0s case by insuring min exponent is 158-32
  kf = vec_ctf (k, 0) + fp5;
  // Exponent difference is the leading zeros count
  n = (vui32_t) f2_31 - (vui32_t) kf;
  // right justify exponent for int
  n = vec_vsrw(n, v23);
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
  // Only for testing only, PWR7 is BE only
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
  // Sum across adjacent bytes into words for pop count
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

/** \brief Vector Subtract Unsigned Doubleword Modulo for POWER7 and earlier.
 *
 *  For each unsigned long (64-bit) integer element c[i] = a[i] +
 *  NOT(b[i]) + 1.
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
 *  @return  vector unsigned long int sum of a[0] + NOT(b[0]) + 1
 *  and a[1] + NOT(b[1]) + 1.
 */
static inline vui64_t
vec_subudm_PWR7 (vui64_t vra, vui64_t vrb)
{
  const vui32_t vone = vec_splat_u32 (1);
  // vui32_t cm= { 0,1,0,1};
  const vui32_t cm = (vui32_t) vec_unpackh ((vi16_t) vone);
  vui32_t r;
  vui32_t c;

  // propagate carry/borrow from words 1/3 into words 0/2
  c = vec_vsubcuw ((vui32_t)vra, (vui32_t)vrb);
  r = vec_vsubuwm ((vui32_t)vra, (vui32_t)vrb);
  // Ignore carry's from words 0/2
  c = vec_and (c, cm);
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
