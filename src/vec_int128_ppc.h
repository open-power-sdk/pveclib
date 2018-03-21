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

 vec_int128_ppc.h

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: May 10, 2015
 */

#ifndef VEC_INT128_PPC_H_
#define VEC_INT128_PPC_H_

#include <vec_common_ppc.h>

/*!
 * \file  vec_int128_ppc.h
 * \brief Header package containing a collection of 128-bit computation
 * functions implemented with PowerISA VMX and VSX instructions.
 */

/*! \brief byte reverse quadword for a vector __int128.
 *
 *	Return the bytes / octets of a 128-bit vector in reverse order.
 *
 *	@param vra a 128-bit vector treated a __int128.
 *	@return a 128-bit vector with the bytes in reserve order.
 */
static inline vui128_t
vec_revq (vui128_t vra)
{
  vui128_t result;

#ifdef _ARCH_PWR9
  __asm__(
      "xxbrq %0,%1;"
      : "=wa" (result)
      : "wa" (vra)
      : );
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

/*! \brief byte reverse each doubleword for a vector unsigned long int.
 *
 *	For each doubleword of the input vector, reverse the order of
 *	bytes / octets within the doubleword.
 *
 *	@param vra a 128-bit vector unsigned long int.
 *	@return a 128-bit vector with the bytes of each doubleword
 *	reversed.
 */
static inline vui64_t
vec_revd (vui64_t vra)
{
  vui64_t result;

#ifdef _ARCH_PWR9
  __asm__(
      "xxbrd %0,%1;"
      : "=wa" (result)
      : "wa" (vra)
      : );
#else
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  const vui64_t vconstp = CONST_VINT64_DW(0x0706050403020100UL, 0x0F0E0D0C0B0A0908UL);
#else
  const vui64_t vconstp =
      CONST_VINT64_DW(0x08090A0B0C0D0E0FUL, 0x0001020304050607UL);
#endif
  result = (vui64_t) vec_perm ((vui8_t) vra, (vui8_t) vra, (vui8_t) vconstp);
#endif

  return (result);
}

/*! \brief byte reverse each word of a vector unsigned int.
 *
 *	For each word of the input vector, reverse the order of
 *	bytes / octets within the word.
 *
 *	@param vra a 128-bit vector unsigned int.
 *	@return a 128-bit vector with the bytes of each word
 *	reversed.
 */
static inline vui32_t
vec_revw (vui32_t vra)
{
  vui32_t result;

#ifdef _ARCH_PWR9
  __asm__(
      "xxbrw %0,%1;"
      : "=wa" (result)
      : "wa" (vra)
      : );
#else
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  const vui64_t vconstp = CONST_VINT64_DW(0x0302010007060504UL, 0x0B0A09080F0E0D0CUL);
#else
  const vui64_t vconstp =
      CONST_VINT64_DW(0x0C0D0E0F08090A0BUL, 0x0405060700010203UL);
#endif
  result = (vui32_t) vec_perm ((vui8_t) vra, (vui8_t) vra, (vui8_t) vconstp);
#endif

  return (result);
}

/*! \brief byte reverse each halfword of a vector unsigned short.
 *
 *	For each halfword of the input vector, reverse the order of
 *	bytes / octets within the halfword.
 *
 *	@param vra a 128-bit vector unsigned short.
 *	@return a 128-bit vector with the bytes of each halfword
 *	reversed.
 */
static inline vui16_t
vec_revh (vui16_t vra)
{
  vui16_t result;

#ifdef _ARCH_PWR9
  __asm__(
      "xxbrh %0,%1;"
      : "=wa" (result)
      : "wa" (vra)
      : );
#else
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  const vui64_t vconstp = CONST_VINT64_DW(0x0100030205040706UL, 0x09080B0A0D0C0F0EUL);
#else
  const vui64_t vconstp =
      CONST_VINT64_DW(0x0E0F0C0D0A0B0809UL, 0x0607040502030001UL);
#endif
  result = (vui16_t) vec_perm ((vui8_t) vra, (vui8_t) vra, (vui8_t) vconstp);
#endif

  return (result);
}

/** \brief Count leading zeros for a vector __int128.
 *
 *	Count leading zeros for a vector __int128 and return the count in a
 *	vector suitable for use with vector shift (left|right) and vector
 *	shift (left|right) by octet instructions.
 *
 *	@param vra a 128-bit vector treated a __int128.
 *	@return a 128-bit vector with bits 121:127 containing the count of
 *	leading zeros.
 */
static inline vui128_t
vec_clzq (vui128_t vra)
{
  __vector unsigned long long result;

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
  __vector unsigned long long vt1, vt2, vt3;
  const __vector unsigned long long vzero = { 0, 0 };
  const __vector unsigned long long v64 = { 64, 64 };

  vt1 = vec_vclz ((__vector unsigned long long) vra);
  vt2 = (__vector unsigned long long) vec_cmplt(vt1, v64);
  vt3 = (__vector unsigned long long) vec_sld ((__vector unsigned char) vzero,
                                               (__vector unsigned char) vt2, 8);
  result = vec_andc (vt1, vt3);
//	print_vint128x ("vec_clzq and:", (vui128_t)result);
  result = (__vector unsigned long long) vec_sums ((__vector int) result,
                                                   (__vector int) vzero);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  result = (__vector unsigned long long) vec_sld (
      (__vector unsigned char) result, (__vector unsigned char) result, 4);
#endif
//	print_vint128x ("vec_clzq rtn:", (vui128_t)result);
#else
#warning Implememention pre power8
  __VEC_U_128 xa, xb, xt;

  xa.vx1 = vra;
  xt.ulong.upper = 0;
  if (xa.ulong.upper == 0ULL)
  xt.ulong.lower = __builtin_clzl(xa.ulong.lower) + 64ULL;
  else
  xt.ulong.lower = __builtin_clzl(xa.ulong.upper);

  result = xt.vx2;
#endif

  return ((vui128_t) result);
}
/** \brief Population Count vector __int128.
 *
 *	Count the number of '1' bits within a vector __int128 and return
 *	the count (0-128) in a vector __int128.
 *
 *	@param vra a 128-bit vector treated a __int128.
 *	@return a 128-bit vector with bits 121:127 containing the
 *	population count.
 */
static inline vui128_t
vec_popcntq (vui128_t vra)
{
  __vector unsigned long long result;

#ifdef _ARCH_PWR8
  /*
   * Use the Vector Population Count Doubleword instruction to get
   * the count for the left and right vector halves.  Then sum across
   * the left and right counts to get the final 128-bit vector count
   * (0-128).
   */
  __vector unsigned long long vt1;
  const __vector unsigned long long vzero = { 0, 0 };

  vt1 = vec_vpopcnt ((__vector unsigned long long) vra);
  result = (__vector unsigned long long) vec_sums ((__vector int) vt1,
                                                   (__vector int) vzero);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  result = (__vector unsigned long long) vec_sld (
      (__vector unsigned char) result, (__vector unsigned char) result, 4);
#endif
#else
#warning Implememention pre power8
  __VEC_U_128 xa, xt;

  xa.vx1 = vra;
  xt.ulong.upper = 0;
  xt.ulong.lower = __builtin_popcountl(xa.ulong.lower) + __builtin_popcountl(xa.ulong.upper);

  result = xt.vx2;
#endif

  return ((vui128_t) result);
}

/** \brief Vector Shift Left double Quadword.
 *
 *	Vector Shift Left double Quadword 0-127 bits.
 *	Return a vector __int128 that is the left most 128-bits after
 *	shifting left 0-127-bits of the 32-byte double vector
 *	(vrw||vrx).  The shift amount is from bits 121:127 of vrb.
 *
 *	@param vrw upper 128-bits of the 256-bit double vector.
 *	@param vrx lower 128-bits of the 256-bit double vector.
 *	@param vrb Shift amount in bits 121:127.
 *	@return high 128-bits of left shifted double vector.
 */
static inline vui128_t
vec_sldq (vui128_t vrw, vui128_t vrx, vui128_t vrb)
{
  __vector unsigned char result, vt1, vt2, vt3, vbs;
  const __vector unsigned char vzero = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0 };

  vt1 = vec_slo ((__vector unsigned char) vrw, (__vector unsigned char) vrb);
  /* For some reason we let the processor jockies write they
   * hardware bug into the ISA.  The vsr instruction only works
   * correctly if the bit shift value is splatted to each byte
   * of the vector.  */
  vbs = vec_splat ((__vector unsigned char) vrb, VEC_BYTE_L);
  vt1 = vec_sll (vt1, vbs);
  vt3 = vec_sub (vzero, vbs);
  vt2 = vec_sro ((__vector unsigned char) vrx, vt3);
  vt2 = vec_srl (vt2, vt3);
  result = vec_or (vt1, vt2);

  return ((vui128_t) result);
}

/** \brief Vector Shift right Quadword Immediate.
 *
 *	Vector Shift right Quadword 0-127 bits.
 *	The shift amount is a const unsigned int in the range 0-127.
 *	A shift count of 0 returns the original value of vra.
 *	Shift counts greater then 127 bits return zero.
 *
 *	@param vra a 128-bit vector treated as a __int128.
 *	@param shb Shift amount in the range 0-127.
 *	@return 128-bit vector shifted right shb bits.
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
	   require the shift amount is splated to all 16-bytes of
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
 *	Vector Shift Right Quadword 0-127 bits.
 *	The shift amount is from bits 121-127 of vrb.
 *
 *	@param vra a 128-bit vector treated as a __int128.
 *	@param vrb Shift amount in bits 121:127.
 *	@return Right shifted vector.
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

/** \brief Vector Shift left Quadword Immediate.
 *
 *	Vector Shift left Quadword 0-127 bits.
 *	The shift amount is a const unsigned int in the range 0-127.
 *	A shift count of 0 returns the original value of vra.
 *	Shift counts greater then 127 bits return zero.
 *
 *	@param vra a 128-bit vector treated as a __int128.
 *	@param shb Shift amount in the range 0-127.
 *	@return 128-bit vector shifted left shb bits.
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
	   require the shift amount is splated to all 16-bytes of
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
 *	Vector Shift Left Quadword 0-127 bits.
 *	The shift amount is from bits 121-127 of vrb.
 *
 *	@param vra a 128-bit vector treated as a __int128.
 *	@param vrb Shift amount in bits 121:127.
 *	@return Left shifted vector.
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

/** \brief Vector doubleword paste.
 *	Combine the high doubleword of the 1st vector with the
 *	low double word of the 2nd vector.
 *
 *	@param __VH a 128-bit vector as the source of the
 * 	high order doubleword.
 *	@param __VL a 128-bit vector as the source of the
 * 	low order doubleword.
 *	@return The combined 128-bit vector composed of the high order
 *	doubleword of __VH and the low order doubleword of __VL.
 */
static inline vui64_t
vec_pasted (vui64_t __VH, vui64_t __VL)
{
  vui64_t result;
  /*
   result[1] = __VH[1];
   result[0] = __VL[0];
   */
  result = vec_xxpermdi ((vui64_t) __VH, (vui64_t) __VL, 1);

  return ((vui64_t) result);
}

/** \brief Vector multiply odd unsigned words.
 *
 *	Multiple the odd words of two vector unsigned int values and return
 *	the unsigned long product of the odd words..
 *
 *	@param a 128-bit vector unsigned int.
 *	@param b 128-bit vector unsigned int.
 *	@return vector unsigned long product of the odd words of a and b.
 */
static inline vui64_t
vec_mulouw (vui32_t a, vui32_t b)
{
  vui64_t res;
#if 0
  /* Not supported in GCC yet.  ETA GCC-8.  */
  res = vec_mulo (a, b);
#else
  __asm__(
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
      "vmuleuw %0,%1,%2;\n"
#else
      "vmulouw %0,%1,%2;\n"
#endif
      : "=v" (res)
      : "v" (a),
      "v" (b)
      : );
#endif
  return (res);
}

/** \brief Vector multiply even unsigned words.
 *
 * Multiple the even words of two vector unsigned int values and return
 * the unsigned long product of the even words.
 *
 * @param a 128-bit vector unsigned int.
 * @param b 128-bit vector unsigned int.
 * @return vector unsigned long product of the even words of a and b.
 */
static inline vui64_t
vec_muleuw (vui32_t a, vui32_t b)
{
  vui64_t res;
#if 0
  /* Not supported in GCC yet.  ETA GCC-8.  */
  res = vec_mule (a, b);
#else
  __asm__(
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
      "vmulouw %0,%1,%2;\n"
#else
      "vmuleuw %0,%1,%2;\n"
#endif
      : "=v" (res)
      : "v" (a),
      "v" (b)
      : );
#endif
  return (res);
}

/** \brief Vector multiply odd signed words.
 *
 * Multiple the odd words of two vector signed int values and return
 * the signed long product of the odd words..
 *
 * @param a 128-bit vector signed int.
 * @param b 128-bit vector signed int.
 * @return vector signed long product of the odd words of a and b.
 */
static inline vi64_t
vec_mulosw (vi32_t a, vi32_t b)
{
  vi64_t res;
#if 0
  /* Not supported in GCC yet.  ETA GCC-8.  */
  res = vec_mulo (a, b);
#else
  __asm__(
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
      "vmulesw %0,%1,%2;\n"
#else
      "vmulosw %0,%1,%2;\n"
#endif
      : "=v" (res)
      : "v" (a),
      "v" (b)
      : );
#endif
  return (res);
}

/** \brief Vector multiply even signed words.
 *
 * Multiple the even words of two vector signed int values and return
 * the signed long product of the even words.
 *
 * @param a 128-bit vector signed int.
 * @param b 128-bit vector signed int.
 * @return vector signed long product of the even words of a and b.
 */
static inline vi64_t
vec_mulesw (vi32_t a, vi32_t b)
{
  vi64_t res;
#if 0
  /* Not supported in GCC yet.  ETA GCC-8.  */
  res = vec_mule (a, b);
#else
  __asm__(
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
      "vmulosw %0,%1,%2;\n"
#else
      "vmulesw %0,%1,%2;\n"
#endif
      : "=v" (res)
      : "v" (a),
      "v" (b)
      : );
#endif
  return (res);
}

/** \brief Vector Add Unsigned Quadword Modulo.
 *
 *	Add two vector __int128 values and return result modulo 128-bits.
 *
 *	@param a 128-bit vector treated a __int128.
 *	@param b 128-bit vector treated a __int128.
 *	@return __int128 sum of a and b.
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

/** \brief Vector Add & write Carry Unsigned Quadword.
 *
 *	Add two vector __int128 values and return the carry out.
 *
 *	@param a 128-bit vector treated a __int128.
 *	@param b 128-bit vector treated a __int128.
 *	@return __int128 carry of the sum of a and b.
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

/** \brief Vector Add with carry Unsigned Quadword.
 *
 *	Add two vector __int128 values and return sum and the carry out.
 *
 *	@param *cout carry out from the sum of a and b.
 *	@param a 128-bit vector treated a __int128.
 *	@param b 128-bit vector treated a __int128.
 *	@return __int128 (lower 128-bits) sum of a and b.
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

/** \brief Vector Add Extended Unsigned Quadword Modulo.
 *
 *	Add two vector __int128 values plus a carry (0|1) and return
 *	the modulo 128-bit result.
 *
 *	@param a 128-bit vector treated a __int128.
 *	@param b 128-bit vector treated a __int128.
 *	@param ci Carry-in from vector bit[127].
 *	@return __int128 sum of a + b + c, modulo 128-bits.
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

/** \brief Vector Add Extended & write Carry Unsigned Quadword.
 *
 *	Add two vector __int128 values plus a carry-in (0|1) and return
 *	the carry out bit.
 *
 *	@param a 128-bit vector treated a __int128.
 *	@param b 128-bit vector treated a __int128.
 *	@param ci Carry-in from vector bit[127].
 *	@return carry-out in bit[127] of the sum of a + b + c.
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

/** \brief Vector Add Extend with carry Unsigned Quadword.
 *
 *	Add two vector __int128 values plus a carry-in (0|1)
 *	and return sum and the carry out.
 *
 *	@param *cout carry out from the sum of a and b.
 *	@param a 128-bit vector treated a __int128.
 *	@param b 128-bit vector treated a __int128.
 *	@param ci Carry-in from vector bit[127].
 *	@return __int128 (lower 128-bits) sum of a + b + c.
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

/** \brief Vector multiply odd unsigned doublewords.
 *
 * Multiple the odd 64-bit doublewords of two vector unsigned long values and return
 * the unsigned __int128 product of the odd doublewords..
 *
 * @param a 128-bit vector unsigned long.
 * @param b 128-bit vector unsigned long.
 * @return vector unsigned __int128 product of the odd double words of a and b.
 */
static inline vui128_t
vec_muloud (vui64_t a, vui64_t b)
{
  const vui64_t zero = { 0, 0 };
  vui64_t res;
  vui64_t p0, p1, pp10, pp01;
  vui32_t m0, m1;

  m0 = vec_mergel ((vui32_t) b, (vui32_t) b);
  m1 = (vui32_t) vec_splat ((vui64_t) a, 1);
  p0 = vec_muleuw (m1, m0);
  p1 = vec_mulouw (m1, m0);

  /* res[1] = p1[1];  res[0] = p0[0];  */
  res = vec_pasted (p1, p0);
  /*
   pp10[0] = p1[0]; pp10[1] = 0;
   pp01[0] = p0[1]; pp01[1] = 0;
   */
  pp10 = (vui64_t) vec_mergeh ((vui64_t) p1, (vui64_t) zero);
  pp01 = (vui64_t) vec_mergel ((vui64_t) p0, (vui64_t) zero);

  pp01 = (vui64_t) vec_adduqm ((vui128_t) pp01, (vui128_t) pp10);

  pp01 = (vui64_t) vec_sld ((vi32_t) pp01, (vi32_t) pp01, 4);

  res = (vui64_t) vec_adduqm ((vui128_t) pp01, (vui128_t) res);

  return ((vui128_t) res);
}

/** \brief Vector multiply even unsigned doublewords.
 *
 * Multiple the even 64-bit doublewords of two vector unsigned long values and return
 * the unsigned __int128 product of the even doublewords..
 *
 * @param a 128-bit vector unsigned long.
 * @param b 128-bit vector unsigned long.
 * @return vector unsigned __int128 product of the even double words of a and b.
 */
static inline vui128_t
vec_muleud (vui64_t a, vui64_t b)
{
  const vui64_t zero = { 0, 0 };
  vui64_t res;
  vui64_t p0, p1, pp10, pp01;
  vui32_t m0, m1;

  m0 = vec_mergeh ((vui32_t) b, (vui32_t) b);
  m1 = (vui32_t) vec_splat ((vui64_t) a, 0);
  p0 = vec_muleuw (m1, m0);
  p1 = vec_mulouw (m1, m0);
  /* res[1] = p1[1];  res[0] = p0[0];  */
  res = vec_pasted (p1, p0);
  /*
   pp10[0] = p1[0]; pp10[1] = 0;
   pp01[0] = p0[1]; pp01[1] = 0;
   */
  pp10 = (vui64_t) vec_mergeh ((vui64_t) p1, (vui64_t) zero);
  pp01 = (vui64_t) vec_mergel ((vui64_t) p0, (vui64_t) zero);
  /* pp01 = pp01 + pp10.  */
  pp01 = (vui64_t) vec_adduqm ((vui128_t) pp01, (vui128_t) pp10);

  /* res = res + (pp01 >> 32)  */
  pp01 = (vui64_t) vec_sld ((vi32_t) pp01, (vi32_t) pp01, 4);
  res = (vui64_t) vec_adduqm ((vui128_t) pp01, (vui128_t) res);

  return ((vui128_t) res);
}

/** \brief Vector Multiply Low Unsigned Quadword.
 *
 *	compute the 256 bit product of two 128 bit values a, b.
 *	Only the low order 128 bits of the product are returned.
 *
 *	@param a 128-bit vector treated a __int128.
 *	@param b 128-bit vector treated a __int128.
 *	@return __int128 (lower 128-bits) a * b.
 */
static inline vui128_t
vec_mulluq (vui128_t a, vui128_t b)
{
  vui32_t t, tmq;
  /* compute the 256 bit product of two 128 bit values a, b.
   * The high 128 bits are accumulated in t and the low 128-bits
   * in tmq.  Only the low order 128 bits of the product are
   * returned.
   *
   * We use the Vector Multiple Even/Odd Unsigned word to compute
   * the 128 x 32 partial (160-bit) product of value a with the
   * word splat of b. These instructions (vmuleum, vmuloum)
   * product four 64-bit 32 x 32 partial products where even
   * results are shifted 32-bit left from odd results. After
   * shifting the high 128 bits can be summed via Vector add
   * unsigned quadword.
   *
   */
#ifdef _ARCH_PWR8
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
#warning Implememention pre power8
  __VEC_U_128 xa, xb, xt;

  xa.vx1 = a;
  xb.vx1 = b;
  xt.ui128 = xa.ui128 * xa.ui128;

  tmq = xt.vx4;
#endif
  return ((vui128_t) tmq);
}

/** \brief Vector Multiply Unsigned double Quadword.
 *
 *	compute the 256 bit product of two 128 bit values a, b.
 *	Only the low order 128 bits of the product are returned.
 *
 *	@param *mulu pointer to upper 128-bits of the product.
 *	@param a 128-bit vector treated a __int128.
 *	@param b 128-bit vector treated a __int128.
 *	@return __int128 (lower 128-bits) a * b.
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
   *
   * We use the Vector Multiple Even/Odd Unsigned word to compute
   * the 128 x 32 partial (160-bit) product of value a with the
   * word splat of b. These instructions (vmuleum, vmuloum)
   * product four 64-bit 32 x 32 partial products where even
   * results are shifted 32-bit left from odd results. After
   * shifting the high 128 bits can be summed via Vector add
   * unsigned quadword.
   */
#ifdef _ARCH_PWR8
  vui32_t tsw;
  vui32_t tc;
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
#else
#warning Implememention pre power8 missing vector support
#endif
  *mulu = (vui128_t) t;
  return ((vui128_t) tmq);
}

/** \brief Vector Multiply by 10 Unsigned Quadword.
 *
 *	compute the product of a 128 bit value a * 10.
 *	Only the low order 128 bits of the product are returned.
 *
 *	@param a 128-bit vector treated as a __int128.
 *	@return __int128 (lower 128-bits) a * 10.
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

/** \brief Vector Multiply by 10 extended Unsigned Quadword.
 *
 *	compute the product of a 128 bit value a * 10 + digit(cin).
 *	Only the low order 128 bits of the extended product are returned.
 *
 *	@param a 128-bit vector treated as a unsigned __int128.
 *	@param cin values 0-9 in bits 124:127 of a vector.
 *	@return __int128 (lower 128-bits) a * 10.
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

/** \brief Vector Multiply by 10 & write Carry Unsigned Quadword.
 *
 *	compute the product of a 128 bit value a * 10.
 *	Only the high order 128 bits of the product are returned.
 *	This will be binary coded decimal value 0-9 in bits 124-127,
 *	Bits 0-123 will be '0'.
 *
 *	@param a 128-bit vector treated as a __int128.
 *	@return __int128 (upper 128-bits of the 256-bit product) a * 10 >> 128.
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
 *	Compute the product of a 128 bit value a * 10 + digit(cin).
 *	Only the low order 128 bits of the extended product are returned.
 *
 *	@param a 128-bit vector treated as a unsigned __int128.
 *	@param cin values 0-9 in bits 124:127 of a vector.
 *	@return __int128 (upper 128-bits of the 256-bit product) a * 10 >> 128.
 */
static inline vui128_t
vec_mul10ecuq (vui128_t a, vui128_t cin)
{
//        vui32_t  t;
  vui32_t t_carry;
#ifdef _ARCH_PWR9
  __asm__(
      "vmul10ecuq %0,%1,%2;\n"
      : "=v" (t_carry)
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
//        t = (vui32_t)vec_vadduqm ((vui128_t)t_even, (vui128_t)t_odd);
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

/** \brief Vector combined Multiply by 10 Extended & write Carry Unsigned Quadword.
 *
 *	Compute the product of a 128 bit value a * 10 + digit(cin).
 *	Only the low order 128 bits of the extended product are returned.
 *
 *	@param *cout pointer to upper 128-bits of the product.
 *	@param a 128-bit vector treated as a unsigned __int128.
 *	@param cin values 0-9 in bits 124:127 of a vector.
 *	@return vector __int128 (upper 128-bits of the 256-bit product) a * 10.
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
      : "=v" (t_carry),
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

/** \brief Vector combined Multiply by 10 & write Carry Unsigned Quadword.
 *
 *	compute the product of a 128 bit values a * 10.
 *	Only the low order 128 bits of the product are returned.
 *
 *	@param *cout pointer to upper 128-bits of the product.
 *	@param a 128-bit vector treated as a __int128.
 *	@return vector __int128 (lower 128-bits of the 256-bit product) a * 10.
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
      : "=v" (t_carry),
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

/** \brief Vector combined Multiply by 100 & write Carry Unsigned Quadword.
 *
 *	compute the product of a 128 bit values a * 100.
 *	Only the low order 128 bits of the product are returned.
 *
 *	@param *cout pointer to upper 128-bits of the product.
 *	@param a 128-bit vector treated as a __int128.
 *	@return vector __int128 (lower 128-bits of the 256-bit product) a * 100.
 */
static inline vui128_t
vec_cmul100cuq (vui128_t *cout, vui128_t a)
{
  vui32_t t;
  vui32_t t_carry;
#ifdef _ARCH_PWR9
  vui32_t t0, t1, tc0, tc1;
  /* Times 10 with 1st carry.  */
  __asm__(
      "vmul10cuq %0,%2;\n"
      "\tvmul10uq %1,%2;\n"
      : "=v" (tc0),
      "=v" (t0)
      : "v" (a)
      : );
  /* Times 10 again with 2nd carry.  */
  __asm__(
      "vmul10cuq %0,%2;\n"
      "\tvmul10uq %1,%2;\n"
      : "=v" (tc1),
      "=v" (t1)
      : "v" (t0)
      : );
  /* 1st carry times 10 plus 2nd carry.  */
  __asm__(
      "vmul10euq %0,%1,%2;\n"
      : "=v" (t_carry)
      : "v" (tc0),
      "v" (tc1)
      : );
  t = t0;
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
 *	Compute the product of a 128 bit value a * 100 + digit(cin).
 *	The function return its low order 128 bits of the extended product.
 *	The first parameter (*cout) it the address of the vector to receive
 *	the generated carry out in the range 0-99.
 *
 *	@param *cout pointer to upper 128-bits of the product.
 *	@param a 128-bit vector treated as a unsigned __int128.
 *	@param cin values 0-99 in bits 120:127 of a vector.
 *	@return vector __int128 (lower 128-bits of the 256-bit product) a * 100.
 */
static inline vui128_t
vec_cmul100ecuq (vui128_t *cout, vui128_t a, vui128_t cin)
{
  vui32_t t;
  vui32_t t_carry;
#ifdef _ARCH_PWR9
  vui32_t t0, t1, tc0, tc1;
  /* Times 10 with 1st carry.  */
  __asm__(
      "vmul10cuq %0,%2;\n"
      "\tvmul10uq %1,%2;\n"
      : "=v" (tc0),
      "=v" (t0)
      : "v" (a)
      : );
  /* Times 10 again with 2nd carry.  */
  __asm__(
      "vmul10cuq %0,%2;\n"
      "\tvmul10uq %1,%2;\n"
      : "=v" (tc1),
      "=v" (t1)
      : "v" (t0)
      : );
  /* 1st carry times 10 plus 2nd carry.  */
  __asm__(
      "vmul10euq %0,%1,%2;\n"
      : "=v" (t_carry)
      : "v" (tc0),
      "v" (tc1)
      : );
  /* Add cin to the low bits of a * 100.  If cin is in valid range
   * (0-99) then can not generate carry out of low 128-bits.  */
  t = (vui32_t)vec_vadduqm ((vui128_t)t0, cin);
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
#endif /* VEC_INT128_PPC_H_ */
