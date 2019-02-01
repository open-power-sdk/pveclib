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

/*!
 * \file  vec_bcd_ppc.h
 * \brief Header package containing a collection of Binary Coded
 * Decimal (<B>BCD</B>) computation functions implemented
 * with PowerISA VMX, VSX, and DFP instructions.
 *
 * \note BCD and DFP support requires at least PowerISA 2.05 (POWER6)
 * or later server level processor support.
 *
 * \note Plan to extend this header to cover the POWER9 Zoned decimal
 *  operations and conversions to / from BCD and DFP.
 */

/*! \brief vector signed BCD integer of up to 31 decimal digits.
 *  \note Currently the GCC implementation and the
 *  <A HREF="https://openpowerfoundation.org/?resource_lib=64-bit-elf-v2-abi-specification-power-architecture">
 *  OpenPOWER ELF V2 ABI</A>
 *  disagree on the vector type used (parameters and return values)
 *  by the BCD built-ins. Using vBCD_t insulates <B>pveclib</B> and
 *  applications while this is worked out.
 */
#define vBCD_t vui32_t

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
bcd_qauntize0 (_Decimal128 val)
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
  return (quantized128(t, 0DL));
#endif
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
  d_t = bcd_qauntize0 (d_a / d_b);
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
  d_t = bcd_qauntize0 (d_a * d_b);
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

/** \brief Vector Decimal Convert Binary Coded Decimal (BCD) digit
 *  pairs to binary integer bytes.
 *
 *  Vector Convert 2 adjacent BCD digits, in each byte,
 *  to the corresponding binary char int value.
 *  Input values should be valid BCD nibbles in the range 0-9.
 *  The result will be binary char int values in the range 0-99.
 *
 *  This can be used as the first stage operation in a wider BCD
 *  to binary conversions. Basically the result of this stage are
 *  binary coded 100s "digits".
 *
 *  @param vra a 128-bit vector treated as a vector unsigned char
 *  of BCD pairs.
 *  @return 128-bit vector unsigned char, For each byte, BCD digit
 *  pairs are converted to the equivalent binary representation
 *  in the range 0-99.
 */
static inline vui8_t
vec_bcdctb100s (vui8_t vra)
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

#endif /* VEC_BCD_PPC_H_ */
