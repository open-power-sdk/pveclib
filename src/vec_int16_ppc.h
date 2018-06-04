/*
 Copyright (c) [2018] IBM Corporation.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 vec_int32_ppc.h

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Apr 06, 2018
 */

#ifndef VEC_INT16_PPC_H_
#define VEC_INT16_PPC_H_

#include <vec_char_ppc.h>

/*!
 * \file  vec_int16_ppc.h
 * \brief Header package containing a collection of 128-bit SIMD
 * operations over 16-bit integer elements.
 *
 * Most vector short (16-bit integer) operations are already
 * covered by the original VMX (AKA Altivec) instructions.
 * VMX intrinsic (compiler built-ins) operations are defined in
 * <altivec.h> and described in the compiler documentation.
 *
 * \note The compiler disables associated <altivec.h> built-ins if the
 * <B>mcpu</B> target does not enable the specific instruction.
 * For example if you compile with <B>-mcpu=power7</B>, vec_vclz and
 * vec_vclzh will not be defined.  But vec_clzh is always defined in
 * this header, will generate the minimum code, appropriate for the
 * target, and produce correct results.
 *
 * This header serves to fill in functional gaps for older
 * (POWER7, POWER8) processors and provides an in-line assembler
 * implementation for older compilers that do not
 * provide the build-ins.
 *
 * This header covers operations that are either:
 *
 * - Implemented in hardware instructions for later
 * processors and useful to programmers, on slightly older processors,
 * even if the equivalent function requires more instructions.
 * Examples include Count Leading Zeros, Population Count and Byte
 * Reverse.
 * - Defined in the OpenPOWER ABI but <I>not</I> yet defined in
 * <altivec.n> provided by available compilers in common use.
 * Examples include Count Leading Zeros, Population Count and Byte
 * Reverse.
 * - Commonly used operations, not covered by the ABI or
 * <altivec.h>, and require multiple instructions or
 * are not obvious.
 * Examples include the shift immediate operations.
 *
 */


/** \brief Count leading zeros for a vector unsigned short (halfword)
 *  elements.
 *
 *  Count the number of leading '0' bits (0-16) within each halfword
 *  element of a 128-bit vector.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the Vector Count Leading
 *  Zeros Halfword instruction <B>vclzh</B>. Otherwise use sequence of pre
 *  2.07 VMX instructions.
 *  SIMDized count leading zeros inspired by:
 *
 *  Warren, Henry S. Jr and <I>Hacker's Delight</I>, 2nd Edition,
 *  Addison Wesley, 2013. Chapter 5 Counting Bits, Figure 5-12.
 *
 *  @param vra 128-bit vector treated as 8 x 16-bit integer (halfword)
 *  elements.
 *  @return 128-bit vector with the Leading Zeros count for each
 *  halfword element.
 */
static inline vui16_t
vec_clzh (vui16_t vra)
{
  vui16_t r;
#ifdef _ARCH_PWR8
#ifndef vec_vclzh
  __asm__(
      "vclzh %0,%1;"
      : "=v" (r)
      : "v" (vra)
      : );
#else
  r = vec_vclzh (vra);
#endif
#else
//#warning Implememention pre power8
  vui16_t n, nt, y, x, s, m;
  vui16_t z= { 0,0,0,0, 0,0,0,0};
  vui16_t one = { 1,1,1,1, 1,1,1,1};

  /* n = 16 s = 8 */
  s = vec_splat_u16(8);
  n = vec_add (s, s);
  x = vra;

  /* y=x>>8 if (y!=0) (n=n-8 x=y)  */
  y = vec_sr(x, s);
  nt = vec_sub(n,s);
  m = (vui16_t)vec_cmpgt(y, z);
  s = vec_sr(s,one);
  x = vec_sel (x , y, m);
  n = vec_sel (n , nt, m);

  /* y=x>>4 if (y!=0) (n=n-4 x=y)  */
  y = vec_sr(x, s);
  nt = vec_sub(n,s);
  m = (vui16_t)vec_cmpgt(y, z);
  s = vec_sr(s,one);
  x = vec_sel (x , y, m);
  n = vec_sel (n , nt, m);

  /* y=x>>2 if (y!=0) (n=n-2 x=y)  */
  y = vec_sr(x, s);
  nt = vec_sub(n,s);
  m = (vui16_t)vec_cmpgt(y, z);
  s = vec_sr(s,one);
  x = vec_sel (x , y, m);
  n = vec_sel (n , nt, m);

  /* y=x>>1 if (y!=0) return (n=n-2)   */
  y = vec_sr(x, s);
  nt = vec_sub(n,s);
  nt = vec_sub(nt,s);
  m = (vui16_t)vec_cmpgt(y, z);
  n = vec_sel (n , nt, m);

  /* else return (x-n)  */
  nt = vec_sub (n, x);
  r = vec_sel (nt , n, m);
#endif

  return (r);
}

/** \brief Vector Population Count halfword.
 *
 *  Count the number of '1' bits (0-16) within each byte element of
 *  a 128-bit vector.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the Vector Population
 *  Count Halfword instruction. Otherwise use simple Vector (VMX)
 *  instructions to count bits in bytes in parallel.
 *  SIMDized population count inspired by:
 *
 *  Warren, Henry S. Jr and <I>Hacker's Delight</I>, 2nd Edition,
 *  Addison Wesley, 2013. Chapter 5 Counting Bits, Figure 5-2.
 *
 *  @param vra 128-bit vector treated as 8 x 16-bit integers (halfword)
 *  elements.
 *  @return 128-bit vector with the population count for each halfword
 *  element.
 */
#ifndef vec_popcnth
static inline vui16_t
vec_popcnth (vui16_t vra)
{
  vui16_t r;
#ifdef _ARCH_PWR8
#ifndef vec_vpopcnth
  __asm__(
      "vpopcnth %0,%1;"
      : "=v" (r)
      : "v" (vra)
      : );
#else
  r = vec_vpopcnth (vra);
#endif
#else
  //#warning Implememention pre power8
    __vector unsigned short n, x1, x2, x, s;
    __vector unsigned short ones = { 1,1,1,1, 1,1,1,1};
    __vector unsigned short fives =
        {0x5555,0x5555,0x5555,0x5555, 0x5555,0x5555,0x5555,0x5555};
    __vector unsigned short threes =
        {0x3333,0x3333,0x3333,0x3333, 0x3333,0x3333,0x3333,0x3333};
    __vector unsigned short fs =
        {0x0f0f,0x0f0f,0x0f0f,0x0f0f, 0x0f0f,0x0f0f,0x0f0f,0x0f0f};
    /* n = 8 s = 4 */
    s = ones;
    x = vra;

    /* x = x - ((x >> 1) & 0x5555)  */
    x2 = vec_and (vec_sr (x, s), fives);
    n = vec_sub (x, x2);
    s = vec_add (s, s);

    /* x = (x & 0x3333) + ((x & 0xcccc) >> 2)  */
    x1 = vec_and (n, threes);
    x2 = vec_andc (n, threes);
    n = vec_add (x1, vec_sr (x2, s));
    s = vec_add (s, s);

    /* x = (x + (x >> 4)) & 0x0f0f)  */
    x1 = vec_add (n, vec_sr (n, s));
    n  = vec_and (x1, fs);
    s = vec_add (s, s);

    /* This avoids the extra load const.  */
    /* x = (x + (x << 8)) >> 8)  */
    x1 = vec_add (n, vec_sl (n, s));
    r  = vec_sr (x1, s);
#endif
  return (r);
}
#else
/* Work around for GCC PR85830.  */
#undef vec_popcnth
#define vec_popcnth __builtin_vec_vpopcnth
#endif

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
vec_revbh (vui16_t vra)
{
  vui16_t result;

#ifdef _ARCH_PWR9
#ifndef vec_revb
  __asm__(
      "xxbrh %x0,%x1;"
      : "=wa" (result)
      : "wa" (vra)
      : );
#else
  result = vec_revb (vra);
#endif
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

/** \brief Vector Shift left Halfword Immediate.
 *
 *	Vector Shift left Halfwords each element [0-7], 0-15 bits,
 *	as specified by an immediate value.
 *	The shift amount is a const unsigned int in the range 0-15.
 *	A shift count of 0 returns the original value of vra.
 *	Shift counts greater then 15 bits return zero.
 *
 *	@param vra a 128-bit vector treated as a vector unsigned short.
 *	@param shb Shift amount in the range 0-15.
 *	@return 128-bit vector unsigned short, shifted left shb bits.
 */
static inline vui16_t
vec_slhi (vui16_t vra, const unsigned int shb)
{
  vui16_t lshift;
  vui16_t result;

  if (shb < 16)
    {
      /* Load the shift const in a vector.  The element shifts require
         a shift amount for each element. For the immediate form the
         shift constant is splatted to all elements of the
         shift control.  */
      if (__builtin_constant_p(shb))
	lshift = (vui16_t) vec_splat_s16(shb);
      else
	lshift = vec_splats ((unsigned short) shb);

      /* Vector Shift right bytes based on the lower 4-bits of
         corresponding element of lshift.  */
      result = vec_vslh (vra, lshift);
    }
  else
    { /* shifts greater then 15 bits return zeros.  */
      result = vec_xor ((vui16_t) vra, (vui16_t) vra);
    }

  return (vui16_t) result;
}

/** \brief Vector Shift Right Halfword Immediate.
 *
 *	Vector Shift right Halfwords each element [0-7], 0-15 bits,
 *	as specified by an immediate value.
 *	The shift amount is a const unsigned int in the range 0-15.
 *	A shift count of 0 returns the original value of vra.
 *	Shift counts greater then 15 bits return zero.
 *
 *	@param vra a 128-bit vector treated as a vector unsigned short.
 *	@param shb Shift amount in the range 0-15.
 *	@return 128-bit vector unsigned short, shifted right shb bits.
 */
static inline vui16_t
vec_srhi (vui16_t vra, const unsigned int shb)
{
  vui16_t lshift;
  vui16_t result;

  if (shb < 16)
    {
      /* Load the shift const in a vector.  The element shifts require
         a shift amount for each element. For the immediate form the
         shift constant is splatted to all elements of the
         shift control.  */
      if (__builtin_constant_p(shb))
	lshift = (vui16_t) vec_splat_s16(shb);
      else
	lshift = vec_splats ((unsigned short) shb);

      /* Vector Shift right bytes based on the lower 4-bits of
         corresponding element of lshift.  */
      result = vec_vsrh (vra, lshift);
    }
  else
    { /* shifts greater then 15 bits return zeros.  */
      result = vec_xor ((vui16_t) vra, (vui16_t) vra);
    }
  return (vui16_t) result;
}

/** \brief Vector Shift Right Algebraic Halfword Immediate.
 *
 *  Vector Shift Right Algebraic Halfwords each element [0-7],
 *  0-15 bits, as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-15.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 7 bits return the sign bit
 *  propagated to each bit of each element.
 *
 *  @param vra a 128-bit vector treated as a vector signed char.
 *  @param shb Shift amount in the range 0-7.
 *  @return 128-bit vector signed short, shifted right shb bits.
 */
static inline vi16_t
vec_srahi (vi16_t vra, const unsigned int shb)
{
  vui16_t lshift;
  vi16_t result;

  if (shb < 16)
    {
      /* Load the shift const in a vector.  The element shifts require
         a shift amount for each element. For the immediate form the
         shift constant is splatted to all elements of the
         shift control.  */
      if (__builtin_constant_p(shb))
	lshift = (vui16_t) vec_splat_s16(shb);
      else
	lshift = vec_splats ((unsigned short) shb);

      /* Vector Shift Right Algebraic Halfwords based on the lower 4-bits
         of corresponding element of lshift.  */
      result = vec_vsrah (vra, lshift);
    }
  else
    { /* shifts greater then 15 bits returns the sign bit propagated to
         all bits.   This is equivalent to shift Right Algebraic of
         15 bits.  */
      lshift = (vui16_t) vec_splat_s16(15);
      result = vec_vsrah (vra, lshift);
    }

  return (vi16_t) result;
}


#endif /* VEC_INT16_PPC_H_ */
