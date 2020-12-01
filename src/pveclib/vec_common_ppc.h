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
 */

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
  unsigned __int128 i128;
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
