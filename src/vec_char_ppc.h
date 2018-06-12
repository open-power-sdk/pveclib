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

 vec_char_ppc.h

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Jul 2, 2015
      Steven Munroe, additional contributions for POWER9.
 */

#ifndef VEC_CHAR_PPC_H_
#define VEC_CHAR_PPC_H_

#include <vec_common_ppc.h>

/*!
 * \file  vec_char_ppc.h
 * \brief Header package containing a collection of char and 8-bit
 * integer computation functions implemented
 * with PowerISA VMX, VSX, and DFP instructions.
 *
 * Most vector char (8-bit integer) operations are are already covered
 * by the original VMX (AKA Altivec) instructions. VMX intrinsic
 * (compiler built-ins) operations are defined in <altivec.h> and
 * described in the compiler documentation.
 *
 * \note The compiler disables associated <altivec.h> built-ins if the
 * <B>mcpu</B> target does not enable the specific instruction.
 * For example if you compile with <B>-mcpu=power7</B>, vec_vclz and
 * vec_vclzb will not be defined.  But vec_clzb is always defined in
 * this header, will generate the minimum code, appropriate for the
 * target, and produce correct results.
 *
 * This header serves to fill in functional gaps for older
 * (POWER7, POWER8) processors and provides a in-line assembler
 * implementation for older compilers that do not
 * provide the build-ins.
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
 * are not obvious.
 * Examples include the shift immediate operations and the common
 * ASCII character tests and case conversions.
 *
 */

/** \brief Vector Absolute Difference Unsigned byte.
 *
 * Compute the absolute difference for each byte.
 * For each unsigned byte, subtract B[i] from A[i] and return the
 * absolute value of the difference.
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
        __asm__(
            "vabsdub %0,%1,%2;"
            : "=v" (result)
            : "v" (vra), "v" (vrb)
            : );
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

/** \brief Count leading zeros for a vector unsigned char (byte)
 *  elements.
 *
 *  Count the number of leading '0' bits (0-7) within each byte
 *  element of a 128-bit vector.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the Vector Count Leading
 *  Zeros byte instruction <B>vclzb</B>. Otherwise use sequence of pre
 *  2.07 VMX instructions.
 *  SIMDized count leading zeros inspired by:
 *
 *  Warren, Henry S. Jr and <I>Hacker's Delight</I>, 2nd Edition,
 *  Addison Wesley, 2013. Chapter 5 Counting Bits, Figure 5-12.
 *
 *  @param vra 128-bit vector treated as 16 x 8-bit integer (byte)
 *  elements.
 *  @return 128-bit vector with the Leading Zeros count for each
 *  byte element.
 */
static inline vui8_t
vec_clzb (vui8_t vra)
{
  vui8_t r;
#ifdef _ARCH_PWR8
#ifndef vec_vclzb
  __asm__(
      "vclzb %0,%1;"
      : "=v" (r)
      : "v" (vra)
      : );
#else
  r = vec_vclzb (vra);
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

/** \brief Vector isalpha.
 *
 * Return a vector boolean char with a true indicator for any character
 * that is either Lower Case Alpha ASCII or Upper Case ASCII.
 * False otherwise.
 *
 * @param vec_str vector of 16 ASCII characters
 * @return vector bool char of the isalpha operation applied to each
 * character of vec_str. For each byte 0xff indicates true (isalpha),
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

/** \brief Vector isalnum.
 *
 * Return a vector boolean char with a true indicator for any character
 * that is either Lower Case Alpha ASCII, Upper Case ASCII, or numeric ASCII.
 * False otherwise.
 *
 * @param vec_str vector of 16 ASCII characters
 * @return vector bool char of the isalnum operation applied to each
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
#ifndef vec_vpopcntb
  __asm__(
      "vpopcntb %0,%1;"
      : "=v" (r)
      : "v" (vra)
      : );
#else
  r = vec_vpopcntb (vra);
#endif
#else
//#warning Implememention pre power8
  vui8_t n, x1, x2, x, s;
  vui8_t ones = { 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1};
  vui8_t fives =
      {0x55,0x55,0x55,0x55, 0x55,0x55,0x55,0x55,
	  0x55,0x55,0x55,0x55, 0x55,0x55,0x55,0x55};
  vui8_t threes =
      {0x33,0x33,0x33,0x33, 0x33,0x33,0x33,0x33,
	  0x33,0x33,0x33,0x33, 0x33,0x33,0x33,0x33};
  vui8_t fs =
      {0x0f,0x0f,0x0f,0x0f, 0x0f,0x0f,0x0f,0x0f,
	  0x0f,0x0f,0x0f,0x0f, 0x0f,0x0f,0x0f,0x0f};
  /* n = 8 s = 4 */
  s = ones;
  x = vra;
  /* x = x - ((x >> 1) & 0x55)  */
  x2 = vec_and (vec_sr (x, s), fives);
  n = vec_sub (x, x2);
  s = vec_add (s, s);
  /* x = (x & 0x33) + ((x & 0xcc) >> 2)  */
  x1 = vec_and (n, threes);
  x2 = vec_andc (n, threes);
  n = vec_add (x1, vec_sr (x2, s));
  s = vec_add (s, s);
  /* x = (x + (x >> 4)) & 0x0f)  */
  x1 = vec_add (n, vec_sr (n, s));
  n  = vec_and (x1, fs);
  r = n;
#endif
  return (r);
}
#else
/* Work around for GCC PR85830.  */
#undef vec_popcntb
#define vec_popcntb __builtin_vec_vpopcntb
#endif

/** \brief Vector Shift left Byte Immediate.
 *
 *	Shift left each byte element [0-15], 0-7 bits,
 *	as specified by an immediate value.
 *	The shift amount is a const unsigned int in the range 0-7.
 *	A shift count of 0 returns the original value of vra.
 *	Shift counts greater then 7 bits return zero.
 *
 *	@param vra a 128-bit vector treated as a vector unsigned char.
 *	@param shb Shift amount in the range 0-7.
 *	@return 128-bit vector unsigned char, shifted left shb bits.
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
	lshift = (vui8_t) vec_splat_s8(shb);
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
 *	Shift right each byte element [0-15], 0-7 bits,
 *	as specified by an immediate value.
 *	The shift amount is a const unsigned int in the range 0-7.
 *	A shift count of 0 returns the original value of vra.
 *	Shift counts greater then 7 bits return the sign bit
 *	propagated to each bit of each element.
 *
 *	@param vra a 128-bit vector treated as a vector signed char.
 *	@param shb Shift amount in the range 0-7.
 *	@return 128-bit vector signed char, shifted right shb bits.
 */
static inline vi8_t
vec_srabi (vi8_t vra, const unsigned int shb)
{
  vui8_t lshift;
  vi8_t result;

  if (shb < 8)
    {
      /* Load the shift const in a vector.  The element shifts require
         a shift amount for each element. For the immediate form the
         shift constant is splatted to all elements of the
         shift control.  */
      if (__builtin_constant_p(shb))
	lshift = (vui8_t) vec_splat_s8(shb);
      else
	lshift = vec_splats ((unsigned char) shb);

      /* Vector Shift Right Algebraic Bytes based on the lower 3-bits
         of corresponding element of lshift.  */
      result = vec_vsrab (vra, lshift);
    }
  else
    { /* shifts greater then 7 bits returns the sign bit propagated to
         all bits.   This is equivalent to shift Right Algebraic of
         7 bits.  */
      lshift = (vui8_t) vec_splat_s8(7);
      result = vec_vsrab (vra, lshift);
    }

  return (vi8_t) result;
}

/** \brief Vector Shift Right Byte Immediate.
 *
 *	Shift right each byte element [0-15], 0-7 bits,
 *	as specified by an immediate value.
 *	The shift amount is a const unsigned int in the range 0-7.
 *	A shift count of 0 returns the original value of vra.
 *	Shift counts greater then 7 bits return zero.
 *
 *	@param vra a 128-bit vector treated as a vector unsigned char.
 *	@param shb Shift amount in the range 0-7.
 *	@return 128-bit vector unsigned char, shifted right shb bits.
 */
static inline vui8_t
vec_srbi (vui8_t vra, const unsigned int shb)
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
	lshift = (vui8_t) vec_splat_s8(shb);
      else
	lshift = vec_splats ((unsigned char) shb);

      /* Vector Shift right bytes based on the lower 3-bits of
         corresponding element of lshift.  */
      result = vec_vsrb (vra, lshift);
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

/** \brief Vector toupper.
 *
 * Convert any Lower Case Alpha ASCII characters within a vector
 * unsigned char into the equivalent Upper Case character.
 * Return the result as a vector unsigned char.
 *
 * @param vec_str vector of 16 ASCII characters
 * @return vector char converted to upper case.
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

/** \brief Vector tolower.
 *
 * Convert any Upper Case Alpha ASCII characters within a vector
 * unsigned char into the equivalent Lower Case character.
 * Return the result as a vector unsigned char.
 *
 * @param vec_str vector of 16 ASCII characters
 * @return vector char converted to lower case.
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

#endif /* VEC_CHAR_PPC_H_ */
