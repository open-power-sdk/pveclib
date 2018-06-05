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
      Created on: Mar 29, 2018
 */

#ifndef VEC_INT32_PPC_H_
#define VEC_INT32_PPC_H_

#include <vec_int16_ppc.h>

/*!
 * \file  vec_int32_ppc.h
 * \brief Header package containing a collection of 128-bit SIMD
 * operations over 32-bit integer elements.
 *
 * Most vector int (32-bit integer word) operations are implemented
 * with PowerISA VMX instructions either defined by the original VMX
 * (AKA Altivec) or added to later versions of the PowerISA.
 * POWER8 added several multiply word operations not included in the
 * original VMX.
 *
 * Also some useful word wise merge, shift, and splat operations were
 * added with VSX in PowerISA 2.06B.
 * Most of these intrinsic (compiler built-ins) operations are defined
 * in <altivec.h> and described in the compiler documentation.
 *
 * \note The compiler disables associated <altivec.h> built-ins if the
 * <B>mcpu</B> target does not enable the specific instruction.
 * For example if you compile with <B>-mcpu=power7</B>, vec_vclz and
 * vec_vclzw will not be defined.  But vec_clzw is always defined in
 * this header, will generate the minimum code, appropriate for the
 * target, and produce correct results.
 *
 * Most of these operations are implemented in a single instruction
 * on newer (POWER8/POWER9) processors.
 * This header serves to fill in functional gaps for older
 * (POWER7, POWER8) processors and provides a in-line assembler
 * implementation for older compilers that do not
 * provide the build-ins.
 *
 * This header covers operations that are either:
 *
 * - Implemented in hardware instructions for later
 * processors and useful to programmers, on slightly older processors,
 * even if the equivalent function requires more instructions.
 * Examples include the multiply even/odd/modulo word operations.
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

/** \brief Vector Count Leading Zeros word.
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
 *  Addison Wesley, 2013. Chapter 5 Counting Bits, Figure 5-12.
 *
 *  @param vra 128-bit vector treated as 4 x 32-bit integer (words)
 *  elements.
 *  @return 128-bit vector with the Leading Zeros count for each word
 *  element.
 */
static inline vui32_t
vec_clzw (vui32_t vra)
{
  vui32_t r;
#ifdef _ARCH_PWR8
#ifndef vec_vclzw
  __asm__(
      "vclzw %0,%1;"
      : "=v" (r)
      : "v" (vra)
      : );
#else
  r = vec_vclzw (vra);
#endif
#else
//#warning Implememention pre POWER8
  vui32_t n, nt, y, x, s, m;
  vui32_t z= {0,0,0,0};
  vui32_t one = {1,1,1,1};

  /* n = 32 s = 16 */
  s = vec_splat_u32(8);
  s = vec_add (s, s);
  n = vec_add (s, s);

  x = vra;
  /* y=x>>16 if (y!=0) (n=n-16 x=y)  */
  y = vec_sr(x, s);
  nt = vec_sub(n,s);
  m = (vui32_t)vec_cmpgt(y, z);
  s = vec_sr(s,one);
  x = vec_sel (x, y, m);
  n = vec_sel (n, nt, m);

  /* y=x>>8 if (y!=0) (n=n-8 x=y)  */
  y = vec_sr(x, s);
  nt = vec_sub(n,s);
  m = (vui32_t)vec_cmpgt(y, z);
  s = vec_sr(s,one);
  x = vec_sel (x, y, m);
  n = vec_sel (n, nt, m);

  /* y=x>>4 if (y!=0) (n=n-4 x=y)  */
  y = vec_sr(x, s);
  nt = vec_sub(n,s);
  m = (vui32_t)vec_cmpgt(y, z);
  s = vec_sr(s,one);
  x = vec_sel (x, y, m);
  n = vec_sel (n, nt, m);

  /* y=x>>2 if (y!=0) (n=n-2 x=y)  */
  y = vec_sr(x, s);
  nt = vec_sub(n,s);
  m = (vui32_t)vec_cmpgt(y, z);
  s = vec_sr(s,one);
  x = vec_sel (x, y, m);
  n = vec_sel (n, nt, m);

  /* y=x>>1 if (y!=0) return (n=n-2)   */
  y = vec_sr(x, s);
  nt = vec_sub(n,s);
  nt = vec_sub(nt,s);
  m = (vui32_t)vec_cmpgt(y, z);
  n = vec_sel (n, nt, m);

  /* else return (x-n)  */
  nt = vec_sub (n, x);
  n = vec_sel (nt, n, m);
  r = n;
#endif
  return ((vui32_t) r);
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

/** \brief Vector multiply odd signed words.
 *
 * Multiple the odd words of two vector signed int values and return
 * the signed long product of the odd words.
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
#ifdef _ARCH_PWR8
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
#else
  const vui32_t zero = {0,0,0,0};
  const vui32_t ones = {-1,-1,-1,-1};
  vui32_t wmask01;
  vui32_t p0, p1, pp10, pp01, resw;
  vui16_t m0, m1, mt, mth, mtl;

  /* generate {0,-1,0,-1}  mask.  */
  wmask01 = vec_vmrghw (zero, ones);

  mt = (vui16_t)b;
  mtl = vec_mergeh (mt, mt);
  mth = vec_mergel (mt, mt);

#ifdef _ARCH_PWR7
  m0 = (vui16_t)vec_xxpermdi ((vui64_t)mtl, (vui64_t)mth, 0);
#else
  {
    vui32_t temp;
    temp = vec_sld ((vui32_t) mtl, (vui32_t) mth, 8);
    m0 = (vui16_t) vec_sld (temp, (vui32_t) mth, 8);
  }
#endif

  resw = vec_sld (a, a, 12);
  resw = vec_sel (a, resw, wmask01);
  m1 = (vui16_t)resw;

  p0 = vec_vmuleuh (m1, m0);
  p1 = vec_vmulouh (m1, m0);
  resw = vec_sel (p0, p1, wmask01);
  res = (vui64_t)resw;

  pp10 = vec_sld (p1, p1, 12);
  pp01 = p0;
  /* pp01 = vec_addudm (pp01, pp10).  */
  {
    vui32_t c;
    vui32_t xmask;
    xmask = vec_sld (wmask01, wmask01, 2);
    c    = vec_vaddcuw (pp01, pp10);
    pp01 = vec_vadduwm (pp01, pp10);
    c    = vec_sld (c, c, 6);
    pp01 = vec_sld (pp01, pp01, 2);
    pp01 = vec_sel (c, pp01, xmask);
  }
  /* res = vec_addudm (pp01, res).  */
  {
    vui32_t c, r;
    c = vec_vaddcuw (pp01, (vui32_t)res);
    r = vec_vadduwm (pp01, (vui32_t)res);
    c = vec_sld (c, zero, 4);
    res = (vui64_t)vec_vadduwm (r, c);
  }
#endif
  return (res);
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
#ifdef _ARCH_PWR8
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
#else
  const vui32_t zero = {0,0,0,0};
  const vui32_t ones = {-1,-1,-1,-1};
  vui32_t wmask01;
  vui32_t p0, p1, pp10, pp01, resw;
  vui16_t m0, m1, mt, mth, mtl;
  /* generate {0,-1,0,-1}  mask.  */
  wmask01 = vec_vmrghw (zero, ones);

  mt = (vui16_t)b;
  mtl = vec_mergel (mt, mt);
  mth = vec_mergeh (mt, mt);
#ifdef _ARCH_PWR7
  m0 = (vui16_t)vec_xxpermdi ((vui64_t)mth, (vui64_t)mtl, 3);
#else
  {
    vui32_t temp;
    temp = vec_sld ((vui32_t) mtl, (vui32_t) mtl, 8);
    result = (vui64_t) vec_sld ((vui32_t) mth, temp, 8);
  }
#endif

  resw = vec_sld (a, a, 4);
  m1 = (vui16_t)vec_sel (resw, a, wmask01);

  p0 = vec_vmuleuh (m1, m0);
  p1 = vec_vmulouh (m1, m0);

  resw = vec_sel (p0, p1, wmask01);
  res = (vui64_t)resw;

  pp10 = vec_sld (p1, p1, 12);
  pp01 = p0;
  /* pp01 = vec_addudm (pp01, pp10).  */
  {
    vui32_t c;
    vui32_t xmask;
    xmask = vec_sld (wmask01, wmask01, 2);
    c    = vec_vaddcuw (pp01, pp10);
    pp01 = vec_vadduwm (pp01, pp10);
    c    = vec_sld (c, c, 6);
    pp01 = vec_sld (pp01, pp01, 2);
    pp01 = vec_sel (c, pp01, xmask);
  }
  /* res = vec_addudm (pp01, res).  */
  {
    vui32_t c, r;
    c = vec_vaddcuw (pp01, (vui32_t)res);
    r = vec_vadduwm (pp01, (vui32_t)res);
    c = vec_sld (c, zero, 4);
    res = (vui64_t)vec_vadduwm (r, c);
  }
#endif
  return (res);
}

/** \brief Vector Multiply Unsigned Word Modulo.
 *
 *  Multiple the corresponding word elements of two vector unsigned int
 *  values and return the low order 32-bits of the 64-bit product for
 *  each element.
 *
 *  @param a 128-bit vector signed int.
 *  @param b 128-bit vector signed int.
 *  @return vector low order 32-bit of the product of the word elements
 *  of a and b.
 */
static inline vui32_t
vec_muluwm (vui32_t a, vui32_t b)
{
#if __GNUC__ >= 7
  return vec_mul (a, b);
#else
  vui32_t r;
#ifdef _ARCH_PWR8
  __asm__(
      "vmuluwm %0,%1,%2;\n"
      : "=v" (r)
      : "v" (a),
      "v" (b)
      : );
#else
  vui32_t s16 = (vui32_t)vec_vspltisw (-16);
  vui32_t z = (vui32_t)vec_vspltisw (0);
  vui32_t t4;
  vui32_t t2, t3;
  vui16_t t1;

  t1 = (vui16_t)vec_vrlw (b, s16);
  t2 = vec_vmulouh ((vui16_t)a, (vui16_t)b);
  t3 = vec_vmsumuhm ((vui16_t)a, t1, z);
  t4 = vec_vslw (t3, s16);
  r = (vui32_t)vec_vadduwm (t4, t2);
#endif
  return (r);
#endif
}

/** \brief Vector Population Count word.
 *
 *  Count the number of '1' bits (0-32) within each word element of
 *  a 128-bit vector.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the Vector Population
 *  Count Word instruction. Otherwise use the pveclib vec_popcntb
 *  to count each byte then sum across with Vector Sum across Quarter
 *  Unsigned Byte Saturate.
 *
 *  @param vra 128-bit vector treated as 4 x 32-bit integer (words)
 *  elements.
 *  @return 128-bit vector with the population count for each word
 *  element.
 */
#ifndef vec_popcntw
static inline vui32_t
vec_popcntw (vui32_t vra)
{
  vui32_t r;
#ifdef _ARCH_PWR8
#ifndef vec_vpopcntw
  __asm__(
      "vpopcntw %0,%1;"
      : "=v" (r)
      : "v" (vra)
      : );
#else
  r = vec_vpopcntw (vra);
#endif
#else
//#warning Implememention pre power8
  vui32_t z= { 0,0,0,0};
  vui8_t x;
  x = vec_popcntb ((vui8_t)vra);
  r = vec_vsum4ubs (x, z);
#endif
  return (r);
}
#else
/* Work around for GCC PR85830.  */
#undef vec_popcntw
#define vec_popcntw __builtin_vec_vpopcntw
#endif

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
vec_revbw (vui32_t vra)
{
  vui32_t result;

#ifdef _ARCH_PWR9
#ifndef vec_revb
  __asm__(
      "xxbrw %x0,%x1;"
      : "=wa" (result)
      : "wa" (vra)
      : );
#else
  result = vec_revb (vra);
#endif
#else
  const vui64_t vconstp =
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
      CONST_VINT64_DW(0x0302010007060504UL, 0x0B0A09080F0E0D0CUL);
#else
      CONST_VINT64_DW(0x0C0D0E0F08090A0BUL, 0x0405060700010203UL);
#endif
  result = (vui32_t) vec_perm ((vui8_t) vra, (vui8_t) vra, (vui8_t) vconstp);
#endif

  return (result);
}

/** \brief Vector Shift left Word Immediate.
 *
 *	Shift left each word element [0-3], 0-31 bits,
 *	as specified by an immediate value.
 *	The shift amount is a const unsigned int in the range 0-31.
 *	A shift count of 0 returns the original value of vra.
 *	Shift counts greater then 31 bits return zero.
 *
 *	@param vra a 128-bit vector treated as a vector unsigned int.
 *	@param shb shift amount in the range 0-31.
 *	@return 128-bit vector unsigned int, shifted left shb bits.
 */
static inline vui32_t
vec_slwi (vui32_t vra, const unsigned int shb)
{
  vui32_t lshift;
  vui32_t result;

  if (shb < 32)
    {
      /* Load the shift const in a vector.  The element shifts require
         a shift amount for each element. For the immediate form the
         shift constant is splatted to all elements of the
         shift control.  */
      if (__builtin_constant_p (shb) && (shb < 16))
	lshift = (vui32_t) vec_splat_s32(shb);
      else
	lshift = vec_splats ((unsigned int) shb);

      /* Vector Shift right bytes based on the lower 5-bits of
         corresponding element of lshift.  */
      result = vec_vslw (vra, lshift);
    }
  else
    { /* shifts greater then 31 bits return zeros.  */
      result = vec_xor ((vui32_t) vra, (vui32_t) vra);
    }

  return (vui32_t) result;
}

/** \brief Vector Shift Right Algebraic Word Immediate.
 *
 *  Shift Right Algebraic each word element [0-3],
 *  0-31 bits, as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-31.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 31 bits return the sign bit
 *  propagated to each bit of each element.
 *
 *  @param vra a 128-bit vector treated as a vector signed int.
 *  @param shb shift amount in the range 0-31.
 *  @return 128-bit vector signed int, shifted right shb bits.
 */
static inline vi32_t
vec_srawi (vi32_t vra, const unsigned int shb)
{
  vui32_t lshift;
  vi32_t result;

  if (shb < 32)
    {
      /* Load the shift const in a vector.  The element shifts require
         a shift amount for each element. For the immediate form the
         shift constant is splatted to all elements of the
         shift control.  */
      if (__builtin_constant_p (shb) && (shb < 16))
	lshift = (vui32_t) vec_splat_s32(shb);
      else
	lshift = vec_splats ((unsigned int) shb);

      /* Vector Shift Right Algebraic Words based on the lower 5-bits
         of corresponding element of lshift.  */
      result = vec_vsraw (vra, lshift);
    }
  else
    { /* shifts greater then 31 bits returns the sign bit propagated to
         all bits.   This is equivalent to shift Right Algebraic of
         31 bits.  */
      lshift = (vui32_t) vec_splats(31);
      result = vec_vsraw (vra, lshift);
    }

  return (vi32_t) result;
}

/** \brief Vector Shift Right Word Immediate.
 *
 *	Shift right each word element [0-3], 0-31 bits,
 *	as specified by an immediate value.
 *	The shift amount is a const unsigned int in the range 0-31.
 *	A shift count of 0 returns the original value of vra.
 *	Shift counts greater then 31 bits return zero.
 *
 *	@param vra a 128-bit vector treated as a vector unsigned char.
 *	@param shb shift amount in the range 0-31.
 *	@return 128-bit vector unsigned int, shifted right shb bits.
 */
static inline vui32_t
vec_srwi (vui32_t vra, const unsigned int shb)
{
  vui32_t lshift;
  vui32_t result;

  if (shb < 32)
    {
      /* Load the shift const in a vector.  The element shifts require
         a shift amount for each element. For the immediate form the
         shift constant is splatted to all elements of the
         shift control.  */
      if (__builtin_constant_p (shb) && (shb < 16))
	lshift = (vui32_t) vec_splat_s32(shb);
      else
	lshift = vec_splats ((unsigned int) shb);

      /* Vector Shift right bytes based on the lower 5-bits of
         corresponding element of lshift.  */
      result = vec_vsrw (vra, lshift);
    }
  else
    { /* shifts greater then 31 bits return zeros.  */
      result = vec_xor ((vui32_t) vra, (vui32_t) vra);
    }
  return (vui32_t) result;
}

#endif /* VEC_INT32_PPC_H_ */
