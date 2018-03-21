/*
 Copyright [2017] IBM Corporation.

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
 * Power Vector Library headers.
 *
 *  \section includes
 *  - Typedefs as short names of common vector types.
 *  - Union used to transfer 128-bit data between vector and
 *  non-vector types.
 *  - Helper macros that make declaring constants and accessing
 *  elements, a little easier.
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

/* did not get vector __int128 until GCC4.8.  */
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)
typedef __vector __int128 vi128_t;
typedef __vector unsigned __int128 vui128_t;
#else
/*! \brief vector of one 128-bit signed __int128 element. */
typedef __vector int vi128_t;
/*! \brief vector of one 128-bit unsigned __int128 element. */
typedef __vector unsigned int vui128_t;
#endif

/*! \brief Union used to transfer 128-bit data between vector and
 * non-vector types. */
typedef union
{
  /*! \brief Signed 128-bit integer from pair of 64-bit GPRs.  */
  unsigned __int128 i128;
  /*! \brief Unsigned 128-bit integer from pair of 64-bit GPRs.  */
  unsigned __int128 ui128;
  /*! \brief 128 bit Decimal Float from pair of double float registers.  */
  _Decimal128 dpd128;
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
/*! \brief Element index for lowest order hword.  */
#define VEC_HW_L 0
/*! \brief Element index for lowest order byte.  */
#define VEC_BYTE_L 0
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
#define VEC_DW_H 0
#define VEC_DW_L 1
#define VEC_W_H 0
#define VEC_W_L 3
#define VEC_WE_0 0
#define VEC_WE_1 1
#define VEC_WE_2 2
#define VEC_WE_3 3
#define VEC_HW_H 0
#define VEC_HW_L 7
#define VEC_BYTE_L 15
#define VEC_BYTE_H 0
#define VEC_BYTE_HHW 1
#endif

#endif /* VEC_COMMON_PPC_H_ */
