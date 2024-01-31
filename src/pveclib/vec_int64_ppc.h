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

 vec_int64_ppc.h

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Mar 29, 2018
 */

#ifndef VEC_INT64_PPC_H_
#define VEC_INT64_PPC_H_

#include <pveclib/vec_int32_ppc.h>

/*!
 * \file vec_int64_ppc.h
 * \brief Header package containing a collection of 128-bit SIMD
 * operations over 64-bit integer elements.
 *
 * Most of these operations are implemented in a single instruction
 * on newer (POWER8/POWER9) processors.
 * This header serves to fill in functional gaps for older
 * (POWER7, POWER8) processors and provides a in-line assembler
 * implementation for older compilers that do not
 * provide the built-ins.
 *
 * The original VMX (AKA Altivec) did not define any doubleword element
 * (long long integer or double float) operations.
 * The VSX facility (introduced with POWER7) added vector double float
 * but did not add any integer doubleword (64-bit) operations.  However
 * it did add a useful doubleword permute immediate and word wise;
 * merge, shift, and splat immediate operations.
 * Otherwise vector long int (64-bit elements) operations
 * have to be implemented using VMX word and halfword element integer
 * operations for POWER7.
 *
 * POWER8 (PowerISA 2.07B) adds important doubleword integer (add,
 * subtract, compare, shift, rotate, ...) VMX operations. POWER8 also
 * added multiply word operations that produce the full doubleword
 * product and full quadword add / subtract (with carry extend).
 *
 * POWER9 (PowerISA 3.0B) adds the <B>Vector Multiply-Sum Unsigned
 * Doubleword Modulo</B> instruction. This is not the expected
 * multiply even/odd/modulo doubleword nor a full multiply modulo
 * quadword. But with a few extra (permutes and splat zero)
 * instructions you can get equivalent function.
 *
 * \note The doubleword integer multiply implementations are included
 * in vec_int128_ppc.h.  This resolves a circular dependency as 64-bit
 * by 64-bit integer multiplies require 128-bit integer addition
 * (vec_adduqm()) to produce the full product.
 * \sa vec_msumudm, vec_muleud, vec_mulhud, vec_muloud,
 * vec_muludm, vec_vmuleud, and vec_vmuloud
 *
 * Most of these intrinsic (compiler built-in) operations are defined
 * in <altivec.h> and described in the compiler documentation.
 * However it took several compiler releases for all the new POWER8
 * 64-bit integer vector intrinsics to be added to <B>altivec.h</B>.
 * This support started with the GCC 4.9 but was not complete across
 * function/type and bug free until GCC 6.0.
 *
 * \note The compiler disables associated <altivec.h> built-ins if the
 * <B>mcpu</B> target does not enable the specific instruction.
 * For example, if you compile with <B>-mcpu=power7</B>, vec_vclz and
 * vec_vclzd will not be defined.  But vec_clzd is always defined in
 * this header, will generate the minimum code, appropriate for the
 * target, and produce correct results.
 *
 * 64-bit integer operations are commonly used in the implementation of
 * optimized double float math library functions and this applies to the
 * vector equivalents of math functions. So missing, incomplete or
 * buggy support for vector long integer intrinsics can be a impediment
 * to the implementation of optimized and portable vector double math
 * libraries. This header is a prerequisite for vec_f64_ppc.h which
 * together are intended to support the implementation of vector math
 * libraries.
 *
 * Most of these
 * operations are implemented in a single instruction on newer
 * (POWER8/POWER9) processors. So this header serves to fill in
 * functional gaps for older (POWER7, POWER8) processors and provides
 * a in-line assembler implementation for older compilers that do not
 * provide the built-ins.
 *
 * This header covers operations that are any of the following:
 *
 * - Implemented in hardware instructions for later
 * processors and useful to programmers, on slightly older processors,
 * even if the equivalent function requires more instructions.
 * Examples include the doubleword operations:
 * Add, Compare, Maximum, Minimum and Subtract.
 * - Defined in the OpenPOWER ABI but <I>not</I> yet defined in
 * <altivec.n> provided by available compilers in common use.
 * Examples include doubleword forms of:
 * Multiply Even/Odd/Modulo, Count Leading Zeros, Population Count,
 * and Byte Reverse  operations.
 * - Commonly used operations, not covered by the ABI or
 * <altivec.h>, and require multiple instructions or
 * are not obvious.  Examples include doubleword forms of:
 * Merge Algebraic High/Low, Paste,
 * and Rotate/Shift Immediate operations.
 * - Commonly used operations that are useful for doubleword,
 * but are missing from the PowerISA and OpenPOWER ABI.
 * Examples include: Absolute Difference Doubleword
 * and Multiply-Sum Unsigned Word Modulo.
 *
 * \section i64_missing_ops_0_0 Some missing doubleword operations
 *
 * The original VMX instruction set extension was limited to byte,
 * halfword, and word size element operations. This limited vector
 * arithmetic operations to char, short, int and float elements.
 * This limitation persisted until PowerISA 2.06 (POWER7) added the
 * Vector Scalar Extensions (VSX) facility. VSX combined/extended
 * the FPRs and VRs into 64 by 128-bit Vector/Scalar Registers (VSRs).
 *
 * VSX added a large number of scalar double-precision and vector
 * single / double-precision floating-point operations. The
 * double-precision scalar (<B>xs</B> prefix) instructions where
 * largely duplicates of the existing Floating-Point Facility
 * operations, extended to access the whole (64) VSX register set.
 * Similarly the VSX vector single precision floating-point
 * (<B>xv</B> prefix, <B>sp</B> suffix) instructions were added to
 * give vectorized float code access to 64 VSX registers.
 *
 * The addition of VSX vector double-precision (<B>xv</B> prefix)
 * instructions was the most significant addition.  This added
 * vector doubleword floating-point operations and provided
 * access to all 64 VSX registers.
 * Alas, there are no doubleword (64-bit long) integer operations in
 * the initial VSX.  A few logical and permute class (<B>xx</B> prefix)
 * operations on word/doubleword elements where tacked on.
 * These apply equally to float and integer elements.
 * But nothing for 64-bit integer arithmetic.
 *
 * \note The full title in PowerISA 2.06 is <B>Vector-Scalar
 * Floating-Point Operations [Category: VSX]</B>.
 *
 * \subsection i64_missing_ops_0_0_PWR8 POWER8
 * PowerISA 2.07 did add a significant number of doubleword
 * (64-bit) integer operations. Including;
 * - Add and subtract modulo
 * - Signed and unsigned compare, maximum, minimum,
 * - Shift and rotate
 * - Count leading zeros and population count
 *
 * Also a number of new word (32-bit) integer operations;
 * - Multiply even/odd/modulo.
 * - Pack signed/unsigned/saturate and Unpack signed.
 * - Merge even/odd words
 *
 * And some new quadword (128-bit) integer operations;
 * - Add and Subtract modulo/extend/write-carry
 * - Decimal Add and Subtract modulo
 *
 * And some specialized operations;
 * - Crypto, Raid, Polynomial multiply-sum
 *
 * \note The operations above are all Vector Category and can only
 * access the 32 original vector registers (VSRs 32-63).
 *
 * The new VSX operations (with access to all 64 VSRs) were not
 * directly applicable to 64-bit integer arithmetic:
 * - Scalar single precision floating-point
 * - Direct move between GPRs and VSRs
 * - Logical operations; equivalence, not and, or compliment
 *
 * \subsection i64_missing_ops_0_0_PWR9 POWER9
 * PowerISA 3.0 adds a few more doubleword
 * (64-bit) integer operations. Including;
 * - Compare not equal
 * - Count trailing zeros and parity
 * - Extract and Insert
 * - Multiply-sum modulo
 * - Negate
 * - Rotate Left under mask
 *
 * Also a number of new word (32-bit) integer operations;
 * - Absolute Difference word
 * - Extend Sign word to doubleword
 *
 * And some new quadword (128-bit) integer operations;
 * - Multiply-by-10 extend/write-carry
 * - Decimal convert from/to signed (binary) quadword
 * - Decimal convert from/to zoned (ASCII char)
 * - Decimal shift/round/truncate
 *
 * The new VSX operations (with access to all 64 VSRs) were not
 * directly applicable to 64-bit integer arithmetic:
 * - Scalar quad-precision floating-point
 * - Scalar and Vector convert with rounding
 * - Scalar and Vector extract/insert exponent/significand
 * - Scalar and Vector test data class
 * - Permute and Permute right index
 *
 * \subsection i64_missing_ops_0_0_PWR10 POWER10
 * PowerISA 3.1 adds a few more doubleword
 * (64-bit) integer operations. Including:
 * - Vector Multiply/Divide/Modulo instructions.
 *   - Signed/Unsigned
 *   - Multiply Even/Odd
 *   - Multiply High/Low
 *   - Divide Extended
 * - Load/Store Rightmost Doubleword Indexed.
 * - Extend Sign Doubleword to Quadword.
 * - Generate Permute Control Vector
 *
 * Also added Halfword/Word/Quadword operations to compliment
 * some of the new doubleWord operations.
 * There are additional Quad-Precision instructions in support of
 * IEEE128 floating point.
 *
 * \subsection i64_missing_ops_0_1 Challenges and opportunities
 *
 * An impressive list of operations that can be used for;
 * - Vectorizing long integer loops
 * - Implementing useful quadword integer operations which do not have
 * corresponding PowerISA instructions (before POWER9/POWER10).
 * - implementing extended precision multiply, multiplicative
 * inverse, divide and modulo operations
 *
 * The challenge is that useful operations available for POWER10 will
 * need equivalent implementations for POWER9, POWER8 and POWER7.
 * Similarly for operations introduced for POWER9/POWER8.
 * Also there are some obvious missing operations;
 * - Absolute Difference Doubleword (we have byte, halfword, and word)
 * - Average Doubleword (we have byte, halfword, and word)
 * - Extend Sign Doubleword to quadword (we have byte, halfword,
 *   and word).
 * - Multiply-sum Word (we have byte, halfword, and doubleword)
 * - Multiply Even/Odd Doublewords (we have byte, halfword,
 *   and word).
 * - Multiply High/low Doublewords.
 * - Divide/Modulo Double Doubleword (dividing 128-bit by 64-bit).
 *
 * \note Some of these are now instructions on POWER10.
 *
 * The stated goals for pveclib are:
 *  - Provide equivalent functions across versions of the compiler.
 *  - Provide equivalent functions across versions of the PowerISA.
 *  - Provide complete arithmetic operations across supported C types.
 *
 * So the first step is to provide implementations for the
 * key POWER8 doubleword integer operations for older compilers.
 * For example, some of the generic doubleword integer operations
 * were not defined until GCC 6.0.
 * Here we define the specific Compare Equal Unsigned
 * Doubleword implementation:
 * \code
static inline
vb64_t
vec_cmpequd (vui64_t a, vui64_t b)
{
  vb64_t result;
#ifdef _ARCH_PWR8
#if __GNUC__ >= 6
  result = vec_cmpeq(a, b);
#else
  __asm__(
      "vcmpequd %0,%1,%2;\n"
      : "=v" (result)
      : "v" (a),
      "v" (b)
      : );
#endif
#else
  // _ARCH_PWR7 implementation ...
#endif
  return (result);
}
 * \endcode
 * The implementation checks if the compile target is POWER8 then
 * checks of the compiler is new enough to use the generic vector
 * compare built-in. If the generic built-in is not defined in
 * <altivec.h> then we provide the equivalent inline assembler.
 *
 * For POWER7 targets we don't have any vector compare doubleword
 * operations and we need to define the equivalent operation using
 * PowerISA 2.06B (and earlier) instructions.
 * For example:
 * \code
#else
  // _ARCH_PWR7 implementation ...
  vui8_t permute =
    { 0x04,0x05,0x06,0x07, 0x00,0x01,0x02,0x03,
      0x0C,0x0D,0x0E,0x0F, 0x08,0x09,0x0A,0x0B};
  vui32_t r, rr;
  r = (vui32_t) vec_cmpeq ((vui32_t) a, (vui32_t) b);
  if (vec_any_ne ((vui32_t) a, (vui32_t) b))
    {
       rr = vec_perm (r, r, permute);
       r= vec_and (r, rr);
    }
  result = (vb64_t)r;
#endif
 * \endcode
 * Here we use Compare Equal Unsigned Word. If all words are equal,
 * use the result as is.  Otherwise, if any word elements are not
 * equal, we do some extra work. For each doubleword, rotate the word
 * compare result by 32-bits (here we use permute as we don't
 * have rotate doubleword either). Then logical and the original word
 * compare and rotated results to get the final doubleword compare
 * results.
 *
 * Similarly for all the doubleword compare variants. Similarly for
 * doubleword; add, subtract, maximum, minimum, shift, rotate,
 * count leading zeros, population count, and Byte reverse.
 *
 * \subsection i64_missing_ops_0_2 More Challenges
 * Now we can look at the case where vector doubleword operations
 * of interest don't have an equivalent instruction.
 * Here interesting operations include those that are supported
 * for other element sizes and types.
 *
 * The simplest example is absolute difference which was introduced in
 * PowerISA 3.0 for byte, halfword and word elements.
 * From the implementation of vec_absduw() we see how to implement the
 * operation for POWER8 using subtract, maximum, and minimum.
 * For example:
 * \code
static inline vui64_t
vec_absdud (vui64_t vra, vui64_t vrb)
{
  return vec_subudm (vec_maxud (vra, vrb), vec_minud (vra, vrb));
}
 * \endcode
 * This works because pveclib provides implementations for min, max,
 * and sub operations that work across GCC versions and provide
 * processor specific implementations for POWER8/9 and POWER7.
 *
 * \subsubsection i64_missing_ops_0_2_1 Doubleword integer multiplies
 *
 * Now we need to look at the multiply doubleword situation. We need
 * implementations for vec_msumudm(), vec_muleud(), vec_mulhud(),
 * vec_muloud(), and vec_muludm(). We saw in the implementations of
 * vec_int32_ppc.h that multiply high and low/modulo can implemented
 * using multiply and merge even/odd of that element size.
 * Multiply low can also be implemented using the multiply sum and
 * multiply odd of the next smaller element size. Also multiply-sum
 * can be implemented using multiply even/odd and a couple of adds.
 * And multiply even/odd can be implemented using multiply sum by
 * supplying zeros to appropriate inputs/elements.
 *
 * The above discussion has many circular dependencies. Eventually we
 * need to get down to an implementation on each processor using actual
 * hardware instructions. So what multiply doubleword operations
 * does the PowerISA actually have from the list above:
 * - POWER9 added multiply-sum unsigned doubleword modulo but no multiply
 * doubleword even/odd/modulo instructions.
 * - POWER8 added multiply even/odd/modulo word but no multiply-sum word
 * instructions
 * - POWER7 and earlier we have the original VMX multiply even/odd
 * halfword, and multiply-sum unsigned halfword modulo, but no multiply
 * modulo halfword.
 *
 * It seems the best implementation strategy uses;
 * - Multiply-sum doubleword for POWER9
 * - Multiply even/odd word for POWER8
 * - Multiply even/odd halfword for POWER7
 *
 * We really care about performance and latency for POWER9/8. We need
 * POWER7 to work correctly so we can test on and support <I>legacy</I>
 * hardware. The rest is grade school math.
 *
 * First we need to make sure we have implementations across the
 * GCC versions 6, 7, and 8 for the instructions we need. For example:
 *
 * \code
static inline vui128_t
vec_msumudm (vui64_t a, vui64_t b, vui128_t c)
{
  vui128_t res;
#if defined (_ARCH_PWR9) && ((__GNUC__ >= 6) || (__clang_major__ >= 11))
#if (__GNUC__ >= 12)
  res = vec_msum (a, b, c);
#else
  __asm__(
      "vmsumudm %0,%1,%2,%3;\n"
      : "=v" (res)
      : "v" (a), "v" (b), "v" (c)
      : );
#endif
#else
  vui128_t p_even, p_odd, p_sum;

  p_even = vec_muleud (a, b);
  p_odd  = vec_muloud (a, b);
  p_sum  = vec_adduqm (p_even, p_odd);
  res    = vec_adduqm (p_sum, c);
#endif
  return (res);
}
 * \endcode
 * \note The _ARCH_PWR8 implementation above depends on vec_muleud()
 * and vec_muloud() for which there are no hardware instructions.
 * Hold that thought.
 *
 * While we are it we can implement multiply-sum unsigned word modulo.
 * \code
static inline vui64_t
vec_vmsumuwm (vui32_t vra, vui32_t vrb, vui64_t vrc)
{
  vui64_t peven, podd, psum;

  peven = vec_muleuw (vra, vrb);
  podd  = vec_mulouw (vra, vrb);
  psum  = vec_addudm (peven, podd);

  return vec_addudm (psum, vrc);
}
 * \endcode
 * We will need this later.
 *
 * Now we need to provide implementations of vec_muleud()
 * and vec_muloud(). For example:
 * \code
static inline vui128_t
vec_muleud (vui64_t a, vui64_t b)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return vec_vmuloud (a, b);
#else
  return vec_vmuleud (a, b);
#endif
}
 * \endcode
 * The implementation above is just handling the pesky little endian
 * transforms. The real implementations are in vec_vmuleud() and
 * vec_vmuloud() which implement the operation as if the PowerISA
 * included such an instruction.
 * These implementation is NOT endian sensitive and the function is
 * stable across BE/LE implementations.
 * For example:
 * \code
static inline vui128_t
vec_vmuleud (vui64_t a, vui64_t b)
{
  vui64_t res;
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
#if (__GNUC__ >= 12)
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  res = (vui64_t) vec_mulo (a, b);
#else
  res = (vui64_t) vec_mule (a, b);
#endif
#else
  __asm__(
      "vmuleud %0,%1,%2;\n"
      : "=v" (res)
      : "v" (a), "v" (b)
      : );
#endif
#elif defined (_ARCH_PWR9) && ((__GNUC__ >= 6) || (__clang_major__ >= 11))
  const vui64_t zero = { 0, 0 };
  vui64_t b_eud = vec_mrgahd ((vui128_t) b, (vui128_t) zero);
#if (__GNUC__ >= 12)
  res = vec_msum (a, b_eud, (vui128_t) zero)
#else
  __asm__(
      "vmsumudm %0,%1,%2,%3;\n"
      : "=v" (res)
      : "v" (a), "v" (b_eud), "v" (zero)
      : );
#endif
#else
#ifdef _ARCH_PWR8
  const vui64_t zero = { 0, 0 };
  vui64_t p0, p1, pp10, pp01;
  vui32_t m0, m1;

  // Need the endian invariant merge word high here
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  // Nullify the little endian transform
  m0 = vec_mergel ((vui32_t) b, (vui32_t) b);
#else
  m0 = vec_mergeh ((vui32_t) b, (vui32_t) b);
#endif
  m1 = (vui32_t) vec_xxspltd ((vui64_t) a, 0);

  // Need the endian invariant multiply even/odd word here
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  // Nullify the little endian transform
  p1 = vec_muleuw (m1, m0);
  p0 = vec_mulouw (m1, m0);
#else
  p1 = vec_mulouw (m1, m0);
  p0 = vec_muleuw (m1, m0);
#endif
  // res[1] = p1[1];  res[0] = p0[0];
  res = vec_pasted (p0, p1);

  // pp10[1] = p1[0]; pp10[0] = 0;
  // pp01[1] = p0[1]; pp01[0] = 0;
  // Need the endian invariant merge algebraic high/low here
  pp10 = (vui64_t) vec_mrgahd ((vui128_t) zero, (vui128_t) p1);
  pp01 = (vui64_t) vec_mrgald ((vui128_t) zero, (vui128_t) p0);
  // pp01 = pp01 + pp10.
  pp01 = (vui64_t) vec_adduqm ((vui128_t) pp01, (vui128_t) pp10);

  // res = res + (pp01 << 32)
  pp01 = (vui64_t) vec_sld ((vi32_t) pp01, (vi32_t) pp01, 4);
  res = (vui64_t) vec_adduqm ((vui128_t) pp01, (vui128_t) res);
#else
  // _ARCH_PWR7 implementation ...
#endif
#endif
  return ((vui128_t) res);
}
 * \endcode
 * The _ARCH_PWR10 implementation used the Multiply Even Unsigned
 * Doublewords instruction directly.
 * The _ARCH_PWR9 implementation uses the multiply-sum doubleword
 * operation but implements the multiply even behavior by forcing the
 * contents of doubleword element 1 of [VRB] and the contents of
 * [VRC] to 0. Similarly for vec_vmuloud().
 *
 * The _ARCH_PWR8 implementation looks ugly but it works. It starts
 * with some merges and splats to get inputs columns lined up for the
 * multiply.  Then we use (POWER8 instructions) Multiply Even/Odd
 * Unsigned Word to generate doubleword partial products.  Then more
 * merges and a rotate to line up the partial products for summation
 * as the final quadword product.
 *
 * Individually vec_vmuleud() and vec_vmuloud() execute with a latency
 * of 21-23 cycles on POWER8. Normally these operations are used and
 * scheduled together as in the POWER8 implementation of vec_msumudm()
 * or vec_mulhud(). Good scheduling by the compiler and pipelining
 * keeps the POWER8 latency in the 28-32 cycle range.
 * For example, the vec_mulhud() implementation:
 * \code
static inline vui64_t
vec_mulhud (vui64_t vra, vui64_t vrb)
{
  return vec_mrgahd (vec_vmuleud (vra, vrb), vec_vmuloud (vra, vrb));
}
 * \endcode
 * Generates the following code for POWER8:
 * \code
	vspltisw v0,0
	xxmrghw vs33,vs35,vs35
	xxspltd vs45,vs34,0
	xxmrglw vs35,vs35,vs35
	vmulouw v11,v13,v1
	xxspltd vs34,vs34,1
	xxmrghd vs41,vs32,vs43
	vmulouw v12,v2,v3
	vmuleuw v13,v13,v1
	vmuleuw v2,v2,v3
	xxmrghd vs42,vs32,vs44
	xxmrgld vs33,vs32,vs45
	xxmrgld vs32,vs32,vs34
	xxpermdi vs44,vs34,vs44,1
	vadduqm v1,v1,v9
	xxpermdi vs45,vs45,vs43,1
	vadduqm v0,v0,v10
	vsldoi  v1,v1,v1,4
	vsldoi  v0,v0,v0,4
	vadduqm v2,v1,v13
	vadduqm v0,v0,v12
	xxmrghd vs34,vs34,vs32
 * \endcode
 *
 * The POWER9 latencies for this operation range from 5-7 (for
 * vmsumudm itself) to 11-16 (for vec_mulhud()).
 * The additional latency reflects zero constant vector generation and
 * merges required to condition the inputs and output. For these
 * operations the vec_msumudm(), <I>vrc</I> operand is always zero.
 * Selecting the even/odd doubleword for input requires a merge
 * low/high. And selecting the high doubleword for multiply high
 * require a final merge high.
 *
 * vec_mulhud() generates the following code for POWER9:
 * \code
 	xxspltib vs32,0
	xxmrghd vs33,vs35,vs32
	xxmrgld vs35,vs32,vs35
	vmsumudm v1,v2,v1,v0
	vmsumudm v2,v2,v3,v0
	xxmrghd vs34,vs33,vs34
 * \endcode
 *
 * Wrapping up the doubleword multiplies we should look at the multiply
 * low (AKA Multiply Unsigned Doubleword Modulo).
 * The POWER9 implementation is similar to vec_mulhud () and the
 * generated code is similar to the example above.
 *
 * Multiply low doubleword is a special case, as we are discarding the
 * highest partial doubleword product.
 * For POWER8 we can optimize for that case using multiply odd and
 * multiply-sum word operations.
 * Also as we are only generating doubleword partial products we only
 * need add doubleword modulo operations to sum the results.
 * This avoids the more expensive add quadword operation required for
 * the general case.
 * The fact that vec_vmsumuwm() is only a software construct is not
 * an issue.
 * It expands into hardware multiple even/odd word and add doubleword
 * instructions that the compiler can schedule and optimize.
 *
 * Here vec_mulouw() generates low order partial product.
 * Then vec_vrld () and vec_vmsumuwm() generate doubleword sums of the
 * two middle order partial products.
 * Then vec_vsld() shifts the middle order partial sum left
 * 32-bits (discarding the unneeded high order 32-bits).
 * Finally sum the low and middle order partial doubleword products
 * to produce the multiply-low doubleword result.
 * For example, this implementation:
 * \code
static inline vui64_t
vec_muludm (vui64_t vra, vui64_t vrb)
{
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  vui64_t res;
#if (__GNUC__ >= 13)
  res = vec_mul (vra, vrb);
#else
  __asm__(
      "vmulld %0,%1,%2;\n"
      : "=v" (res)
      : "v" (vra), "v" (vrb)
      : );
#endif
  return res;
#elif defined (_ARCH_PWR9) // Effectively use vmsumudm for P9
  return vec_mrgald (vec_vmuleud (vra, vrb), vec_vmuloud (vra, vrb));
#elif defined (_ARCH_PWR8)
  vui64_t s32 = { 32, 32 }; // shift / rotate amount.
  vui64_t z = { 0, 0 };
  vui64_t t2, t3, t4;
  vui32_t t1;

  t1 = (vui32_t) vec_vrld (vrb, s32);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  // Nullify the little endian transform, really want mulouw here.
  t2 = vec_muleuw ((vui32_t) vra, (vui32_t) vrb);
#else
  t2 = vec_mulouw ((vui32_t) vra, (vui32_t) vrb);
#endif
  t3 = vec_vmsumuwm ((vui32_t) vra, t1, z);
  t4 = vec_vsld (t3, s32);
  return (vui64_t) vec_vaddudm (t4, t2);
#else
  return vec_mrgald (vec_vmuleud (vra, vrb), vec_vmuloud (vra, vrb));
#endif
}
 * \endcode
 * Which generates the following for POWER10:
 * \code
     vmulld v2,v2,v3
 * \endcode
 * And generates the following for POWER9:
 * \code
     xxspltib v0,0
     xxmrghd v1,v3,v0
     xxmrgld v3,v0,v3
     vmsumudm v1,v2,v1,v0
     vmsumudm v2,v2,v3,v0
     xxmrgld v2,v1,v2
 * \endcode
 * And generates the following for POWER8:
 * \code
	addis   r9,r2,.rodata.cst16+0x60@ha
	addi    r9,r9,.rodata.cst16+0x60@l
	lxv     vs33,0,r9
	vmulouw v13,v2,v3
	vrld    v0,v3,v1
	vmulouw v3,v2,v0
	vmuleuw v2,v2,v0
	vaddudm v2,v3,v2
	vsld    v2,v2,v1
	vaddudm v2,v13,v2
 * \endcode
 * We can assume that the constant load of <I>{ 32, 32 }</I>
 * will be common-ed with other operations or hoisted out of
 * loops. So the shift constant can be loaded early and vrld is not
 * delayed.
 * This keeps the POWER8 latency in the 19-28 cycle range.
 *
 * \subsubsection i64_missing_ops_0_2_2 Doubleword Integer Divide/Modulo
 *
 * Vector Divide for integer elements is a recent addition to the
 * PowerISA. The original Altivec<SUP>TM</SUP> did not provide any
 * divide operations, even for float elements. Instead it provided a
 * <B>Vector Reciprocal Estimate Floating-Point</B> instruction.
 * This required the <I>Newton-Raphson method</I> to complete the
 * reciprocal to full precision, then a multiply to complete a
 * division.
 *
 * It was not until PowerISA 2.06 (POWER7/VSX) that vector divide
 * was provided for float and double precision. Still no vector divide
 * operations for integer elements. This was the status quo before
 * PowerISA 3.1 (POWER10).
 *
 * POWER10 added vector Divide/Divide-Extend/Modulo (signed/unsigned)
 * over Word/Doubleword/Quadword integer elements.
 * This is now within PVECLIBs mission to provide functionally
 * equivalent vector operations for previous PowerISA (VSX POWER7/8/9)
 * processors.
 * For completeness implement the integer operations across the element
 * sizes (including Halfword and Byte).
 *
 * \note Divide Extended Quadword will be extremely useful in the
 * soft-float implementation of
 * <B>VSX Scalar Divide Quad-Precision [using round to Odd]</B>
 * for POWER7/8.
 *
 * \paragraph i64_missing_ops_0_2_2_0 Vectorizable Divide implementations
 *
 * The trick is to use vector registers and existing instructions to
 * implement division without native vector divide instructions.
 * There are a few ways this can be done:
 * - Vectorize the shift-and-subtract algorithm
 * - Transfer the elements to GPU registers and use scalar divide
 *   instructions (for each element).
 * - Use long division based on a narrower (smaller word) divide
 *   operations.
 * - Or some clever combinations of the above.
 *
 * The selection for best implementation (smallest average cycle time)
 * will depend on a number of factors:
 * - The number of element bits (a quadword shift-subtract algorithm
 *   requires up to 128 iterations).
 * - The cost of transferring vector elements to/from GPRs
 *   (POWER8 and later include Move From/To VSR instructions.
 *   POWER7 does not).
 * - The cycle latency and throughput (IPC) of the scalar divide,
 *   for the platform.
 *
 * \paragraph i64_missing_ops_0_2_2_1 Vectorized Shift-Subtract Divide
 *
 * Consider the algorithm from Hacker's Delight (2nd Edition) Figure 9-2.
 * This is an example of bit-by-bit long division which only requires
 * shift, add/subtract, and compare.
 * It is simple to vectorize by converting the if/then logic into vector
 * compares returning vector bool and vector select.
 * This algorithm requires a double-wide (x || y) dividend/shifter,
 * plus 1-bit (or bool variable) (t) for the carry-out.
 * So for 64-bit doublewords this is logically 129-bits (t || x || y).
 * The result is division of a 2x-bit dividend by x-bit divisor.
 * returning the quotient (in y) and the remainder (in x).
 * \see "Hacker's Delight, 2nd Edition,"
 * Henry S. Warren, Jr, Addison Wesley, 2013.
 * Chapter 9, Integer Divide.
 *
 * For example:
 * \code
vui64_t test_vec_divdud_V0 (vui64_t x, vui64_t y, vui64_t z)
{
  int i;
  vb64_t ge;
  vui64_t t, c, xt;

  for (i = 1; i <= 64; i++)
    {
      // Left shift (x || y) requires 129-bits, -> (t || x || y)
      // capture high bits of x and y into t and c.
      c = vec_srdi (y, 63);
      // capture high bit of x as bool t
      t = (vui64_t) vec_sradi ((vi64_t)x, 63);
      y = vec_addudm (y, y); // Shift left 1, x and y
      x = vec_addudm (x, x);
      // Propagate carry from y to x
      x = vec_addudm (x, c);

      // deconstruct ((t || x) >= z) to (t || (x >= z))
      ge = vec_cmpgeud (x, z);
      // Combine t with (x >= z) for 65-bit compare
      ge = (vb64_t) vec_or ((vui32_t)ge, (vui32_t)t);
      // Convert bool to carry-bit for conditional y+1
      t  = vec_srdi ((vui64_t)ge, 63);

      // if (x >= z) x = x - z ; y++
      xt = vec_subudm (x, z);
      // if ((t || x) >= z) {x = xt; y++}
      y = (vui64_t) vec_or ((vui32_t)y, (vui32_t)t);
      x = vec_selud (x, xt, ge);
    }
  return y;
}
 * \endcode
 * The code above leaves some opportunities for additional optimizations.
 * - The 63-bit shifts require a vector load of the vector constant
 *   {63, 63} under the covers.
 *   Constants like {0, 0} and {1, 1} simpler and faster to generate.
 * - The vec_srdi (y, 63) reduces the sign-bit to simple 1b/0b values as carry-bit c.
 * It is simpler to use rotate-left of 1 and use vec_selud with mask of constant {1, 1}
 * to replace vec_addudm (x, c).
 * to merge the y carry into the left-shifted x value
 * The constant {1, 1} can be shared across the vec_vrld() and the vec_selud().
 * - The vec_sradi(x, 63) creates vector 64-bit bool from the sign bit.
 * This bool is used to generate the bool (t || (x >= z)) -> ge.
 * Then vector t needs to be converted (again) to simple 1b/0b values
 * for the left shifted quotient (vec_or (y, t) -> y).
 * It is simpler to use a signed compare less than 0 to
 * generate the vector bool t.
 * then use vec_sel with mask of constant {1, 1} to replace vec_or(y, t).
 * - Using the bool generating compares allows the compiler to optimize
 *   the vector bool value ge.
 *   It optimizes too vec_orc (vec_cmpgtsd (zero, x), vec_cmpgtud (z, x)).
 *
 * For example:
 * \code
vui64_t test_vec_divdud_V1 (vui64_t x, vui64_t y, vui64_t z)
{
  int i;
  vb64_t ge;
  vui64_t t, c, xt;
  const vui64_t ones = vec_splat_u64(1);
  const vui64_t zeros = vec_splat_u64(0);

  for (i = 1; i <= 64; i++)
    {
      // Left shift (x || y) requires 129-bits, -> (t || x || y)
      // capture high bits of x and y into t and c.
#if defined (_ARCH_PWR8)
      c = vec_vrld (y, ones);
#else
      c = vec_rldi (y, 1);
#endif
      // capture high bit of x as bool t
#if defined (_ARCH_PWR8)
      t = (vui64_t) vec_cmpltsd ((vi64_t) x, (vi64_t) zeros);
#else
      // P7 and earlier did not support DW int.
      // Simpler to convert the sign-bit into a bool
      t = (vui64_t) vec_setb_sd ((vi64_t) x);
#endif
      // Shift left 1, x and y
      y = vec_addudm (y, y);
      x = vec_addudm (x, x);
      // Propagate carry from y to x
      x = vec_selud (x, c, (vb64_t) ones);

      // deconstruct ((t || x) >= z) to (t || (x >= z))
      ge = vec_cmpgeud (x, z);
      // Combine t with (x >= z) for 65-bit compare
      ge = (vb64_t) vec_or ((vui32_t)ge, (vui32_t)t);

      // if (x >= z) x = x - z ; y++
      xt = vec_subudm (x, z);
      // if ((t || x) >= z) {x = xt; y++}
      // Instead of add, OR the boolean ge into bit_0 of y
      y = vec_selud (y, (vui64_t) ge, (vb64_t) ones);
      // Select next x value
      x = vec_selud (x, xt, ge);
    }
  return y;
}
 * \endcode
 * This algorithm is simple to adjust for:
 * - Simple (single wide) divide (initial x = 0),
 * - Extended divide ( (x || <SUP>64</SUP>0b), initial y = 0),
 * - Simple (single wide) modulo (initial x = 0, return x, not y).
 *
 * The single wide divide/modulo can be further simplified because
 * shifting <I>x</I> left can not generate a carry.
 * Thus we can eliminate the special carry <I>t</I> and simplify
 * the compare to just (x >= z).
 * The extended divide can also be simplified because
 * shifting <I>y</I> left can not generate a carry.
 *
 * The same algorithm also works for other element sizes by
 * appropriate type and operation suffix changes.
 *
 * \note Word and Quadword variants can also be optimized to take
 * advantage of instructions that generate the carry directly
 * (vec_addc()). \see \ref int128_Divide_0_1_1_1
 *
 * Another optimization is to leverage the 128-bit vector and use
 * the next larger element size to concatenate the dividend (x_y) into a
 * single integer element. For doubleword divide use a quadword
 * dividend (<I>x_y</I>) while the divisor (<I>z</I>) is the single
 * high doubleword. The result combines the remainder and quotient as
 * doublewords in a single vector.
 * For example;
 * \code
vui64_t test_vec_divqud_V0 (vui128_t x_y, vui64_t z)
{
  int i;
  vui64_t ge;
  //vui128_t cc, c;
  vui64_t t, xt, mone;
  const vui64_t zeros = vec_splat_u64(0);
  mone = (vui64_t) CONST_VINT128_DW (-1, -1);
  // Here only using the high DW of z, generate z as {z, -1}
  z = vec_pasted (z, mone);

  for (i = 1; i <= 64; i++)
    {
      // Left shift (x || y) requires 129-bits, is (t || x || y)
      // capture high bit of x_y as bool t
#if defined (_ARCH_PWR8)
      t = (vui64_t) vec_cmpltsd ((vi64_t) x_y, (vi64_t) zeros);
#else
      { // P7 and earlier did not support DW int compare.
	// But only need to convert the sign-bit into a bool
	vui32_t lts;
	lts = (vui32_t) vec_cmplt ((vi32_t) x_y, (vi32_t) zeros);
	t = (vui64_t) vec_splat (lts, VEC_W_H);
      }
#endif
      // Then shift left Quadword x_y by 1 bit;
      x_y = vec_slqi (x_y, 1);
      // We only need the high DW of t and ge
      // deconstruct ((t || x) >= z) to (t || (x >= z))
#if defined (_ARCH_PWR8)
      // vec_cmpge (x_y,z) is NOT vec_cmpgt (z, x_y)
      ge = (vui64_t) vec_cmpgtud (z, (vui64_t)x_y);
      // Combine t with (x >= z) for 129-bit compare
      ge = (vui64_t) vec_orc ((vui32_t)t, (vui32_t)ge);
#else // P7 and earlier did not support OR Complement
      ge = (vui64_t) vec_cmpgeud ((vui64_t)x_y, z);
      // Combine t with (x >= z) for 129-bit compare
      ge = (vui64_t) vec_or ((vui32_t)t, (vui32_t)ge);
#endif
      // Splat the high ge DW to both DWs for select
      ge = vec_splatd (ge, VEC_DW_H);

      // xt <- {(x - z), (y - ( -1)}
      xt = vec_subudm ((vui64_t)x_y, z);
      x_y = (vui128_t)vec_selud ((vui64_t)x_y, xt, (vb64_t)ge);
    }
  return (vui64_t)x_y;
}
 * \endcode
 * This implementation uses vectors to produce a single 128-bit by
 * 64-bit divide that returns 64-bit remainders and quotients.
 * Here we can use a quadword shift left (vec_slqi()) and avoid
 * the intermediate y to x carry.
 * Also initialize the low doubleword of <I>z</I> as -1. This allows
 * a single vec_subudm() to generate the provisional vector <I>xt</I>
 * as {(x - z), (y - ( -1)} which is effectively {(x - z), (y + 1)}.
 * The final select (vec_selud(x_y, xt, ge)) either; updates the
 * shifted <I>x_y</I> (if bool <I>ge</I> is true),
 * or leaves it unchanged.
 *
 * \note This function form is useful for implementing double element
 * division from long division, for example quadword division.
 * \see "Hacker's Delight, 2nd Edition,"
 * Henry S. Warren, Jr, Addison Wesley, 2013.
 * Chapter 9, Section 9-5 Doubleword Division from Long Division.
 *
 * This reduces the number of instruction within the loop to 8
 * (vs 10-11 for divdud()) instructions.
 * The complex compare (t || (x >= z))
 * generates a serial dependent sequence of three instructions
 * (6 cycles on POWER8).
 * Finally this requires 64 iterations of the inner loop to
 * complete a single divide. The expectations for this sequence
 * is 10-14 cycles per iteration (order of 640-896 cycles total).
 *
 * \note This timing is clearly not acceptable for a single doubleword
 * (or a vector doubleword) result. The scalar fixed-point doubleword
 * divide is 12-23 (14-41 for extended) cycles with a throughput of 2.
 * However when we implement the halfword and byte vector divides the
 * numbers look better for this implementation.
 * Basically the number of iterations is reduced while the number of
 * results per vector increases. For example a vector char divide
 * requires only 8 iterations (~80-112 cycles) while delivering
 * 16 x 8-bit divide results. The scalar (fixed-point) divide
 * (<B>divwu</B>) still runs 12-15 cycles with a throughput of 2.
 * This gives a best case of ((16 / 2) * latency) for rough estimate
 * of 96-120 cycles.
 * This does not include the cost of byte level move from/to VSRs.
 * This can add 5-7 cycles per-byte each way.
 *
 * \paragraph i64_missing_ops_0_2_2_2 Transfer Vector elements for scalar divide
 * The bit-wise long division (above) is disappointing from a
 * performance perspective.
 * One alternative is to use the scalar (fixed-point Unit (FXU)) instructions.
 * These instructions are fast (for a integer divide).
 * For example Power8:
 *  |instruction |Latency|Throughput|
 *  |-----------:|:-----:|:---------|
 *  |divd/divdu  | 12-23 | 2/12-23 cycles |
 *  |divde/divdeu| 14-41 | 2/14-41 cycles |
 * But we need to transfer doublewords from VRs to GPRs to use the
 * scalar divide then transfer the results back to VRs. For example:
 * \code
vui64_t test_vec_divud (vui64_t y, vui64_t z)
{
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  vui64_t res;
#if (__GNUC__ >= 12)
  res = vec_div (y, z);
#else
  __asm__(
      "vdivud %0,%1,%2;\n"
      : "=v" (res)
      : "v" (y), "v" (z)
      : );
#endif
  return res;
#elif defined (_ARCH_PWR7)
  // POWER8/9 Do not have vector integer divide, but do have
  // Move To/From Vector-Scalar Register Instructions
  // So we can use the scalar hardware divide instructions
  __VEC_U_128 qu, yu, zu;
#if (__GNUC__ <= 10)
  // For older GCC force use of mfvsrd
  yu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) y, 1);
  yu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) y, 0);
  zu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) z, 1);
  zu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) z, 0);
#else
  // Use __VEC_U_128 union for transfer
  yu.vx2 = y;
  zu.vx2 = z;
#endif

  qu.ulong.lower = yu.ulong.lower / zu.ulong.lower;
  qu.ulong.upper = yu.ulong.upper / zu.ulong.upper;

  return qu.vx2;
#else
// Default implementation for POWER7 and earlier.
#endif
}
 * \endcode
 * Which should generate:
 * \code
 <test_vec_divud>:
     xxspltd vs11,vs34,1
     xxspltd vs12,vs35,1
     mfvsrd  r9,vs11
     mfvsrd  r8,vs12
     divdu   r10,r9,r8
     mfvsrd  r9,vs34
     mfvsrd  r8,vs35
     divdu   r11,r9,r8
     mtvsrd  vs1,r10
     mtvsrd  vs0,r11
     xxmrghd vs34,vs0,vs1
     blr
 * \endcode
 * The Move From/To VSR Doubleword instruction only transfers the high
 * doubleword. So the sequence requires xxpermdi (xxspltd/xxmrghd)
 * instructions to access the low doubleword.
 *  |instruction |Latency|Throughput|
 *  |-----------:|:-----:|:---------|
 *  |mfvsrd      |  4-5  | 1/cycle  |
 *  |mtvsrd      | 5     | 1/cycle  |
 *  |xxpermdi    | 3     | 2/cycle  |
 * The good news is the transfers are ~8 cycles and the divides are
 * super-scalar dual-issue (i.e. two divides can overlap in the
 * pipeline). So ball-park 8+23+8 = 39 cycles.
 *
 * \note This timing assumes the processor is running in SMT modes 1-2.
 * Higher (4-8) SMT threading levels split pipelines across thread
 * groups and reduce the instruction dispatch/completion group size
 * (per thread). The programmers mileage will vary.
 *
 * This is close enough to the POWER10 vdivud instruction nominal
 * 28-43 cycles to be acceptable.
 *
 * Power8 does not have FXU modulo instructions but this can be
 * implemented with additional multiply and subtract per doubleword.
 *  |instruction |Latency|Throughput|
 *  |-----------:|:-----:|:---------|
 *  |mulld       |  4-5  | 2/cycle  |
 *  |subf        |  1-2  | 6/cycle  |
 * The compiler will generate this for us.
 * For example:
 * \code
vui64_t test_vec_modud (vui64_t y, vui64_t z)
{
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  vui64_t res;
#if (__GNUC__ >= 12)
  res = vec_mod (y, z);
#else
  __asm__(
      "vmodud %0,%1,%2;\n"
      : "=v" (res)
      : "v" (y), "v" (z)
      : );
#endif
  return res;
#elif defined (_ARCH_PWR7)
  // POWER8/9 Do not have vector integer divide, but do have
  // Move To/From Vector-Scalar Register Instructions
  // So we can use the scalar hardware divide instructions
  __VEC_U_128 qu, yu, zu;
#if (__GNUC__ <= 10)
  yu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) y, 1);
  yu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) y, 0);
  zu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) z, 1);
  zu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) z, 0);
#else
  yu.vx2 = y;
  zu.vx2 = z;
#endif

  qu.ulong.lower = yu.ulong.lower % zu.ulong.lower;
  qu.ulong.upper = yu.ulong.upper % zu.ulong.upper;

  return qu.vx2;
#else
// Default implementation for POWER7 and earlier.
#endif
}
 * \endcode
 * Which should generate:
 * \code
 <test_vec_modud>:
     xxspltd vs0,vs34,1
     xxspltd vs1,vs35,1
     mfvsrd  r4,vs0
     mfvsrd  r3,vs1
     divdu   r5,r4,r3
     mfvsrd  r9,vs34
     mfvsrd  r10,vs35
     divdu   r8,r9,r10
     mulld   r5,r5,r3
     subf    r6,r5,r4
     mulld   r10,r8,r10
     subf    r7,r10,r9
     mtvsrd  vs1,r6
     mtvsrd  vs0,r7
     xxmrghd vs34,vs0,vs1
     blr
 * \endcode
 * POWER9 adds the <I>Modulo Unsigned Doubleword</I> and
 * <I>Move To VSR Double Doubleword</I> instructions and the
 * compiler should generate:
 * \code
 <test_vec_modud_PWR9>:
     xxspltd vs11,vs34,1
     xxspltd vs12,vs35,1
     mfvsrd  r9,vs11
     mfvsrd  r8,vs12
     moddu   r10,r9,r8
     mfvsrd  r9,vs34
     mfvsrd  r8,vs35
     moddu   r11,r9,r8
     mtvsrdd vs34,r11,r10
     blr
 * \endcode
 *
 * The C language does not provide an operator for divide extended.
 * So we need to use the GCC built-in <B>__builtin_divdeu()</B> to generate
 * the instruction. For example:
 * \code
  qu.ulong.lower = __builtin_divdeu (yu.ulong.lower, zu.ulong.lower);
  qu.ulong.upper = __builtin_divdeu (yu.ulong.upper, zu.ulong.upper);
 * \endcode
 *
 * The examples above provide functionally equivalent POWER8/9
 * implementations for divide/modulo instructions introduced in
 * POWER10. Now we should look at better implementations of the double
 * doubleword operations <I>test_vec_divdud_V0</I> and
 * <I>test_vec_divqud_V0</I> described in \ref i64_missing_ops_0_2_2_1.
 * While these operations are not defined in the PowerISA they can be
 * useful in the implementation of long division of quadword and
 * multiple quadword divisors.
 *
 * This requires dividing 128-bit (quadwords) dividends by 64-bit
 * divisors. The PowerISA provides divide extended instructions and a
 * Programming Note as an example of how it used.
 * (following the description of Divide Word Extended).
 * This describes the
 * algorithm and example instruction sequence for an implementation.
 * This algorithm applies directly to a scalar based implementation of
 * vec_divqud(). For example:
 * \code
vui64_t test_vec_divqud_V2 (vui128_t x_y, vui64_t z)
{
#if defined (_ARCH_PWR8)
  // POWER8/9 Do not have vector integer divide, but do have
  // Move To/From Vector-Scalar Register Instructions
  // So we can use the scalar hardware divide/divide extended
  __VEC_U_128 qu, xy, zu;
#if (__GNUC__ <= 10)
  xy.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) x_y, 1);
  xy.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) x_y, 0);
  zu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) z, 1);
  zu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) z, 0);
#else
  // Looks like AT16 handles this but what about 15/14 ...
  // AT10 does not.
  xy.vx1 = x_y;
  zu.vx2 = z;
#endif
  // Transfer to GPUs and use scalar divide/divide extended
  // Based on the PowerISA, Programming Note for
  // Divide Word Extended [Unsigned]
  unsigned long long Dh = xy.ulong.upper;
  unsigned long long Dl = xy.ulong.lower;
  unsigned long long Dv = zu.ulong.upper;
  unsigned long long q1, q2, Q;
  unsigned long long r1, r2, R;

  q1 = __builtin_divdeu (Dh, Dv);
  // r1 = -(q1 * Dv);
  r1 = (q1 * Dv);
  q2 = Dl / Dv;
  r2 = Dl - (q2 * Dv);
  Q = q1 + q2;
  // R = r1 + r2;
  R = r2 - r1;
  if ((R < r2) | (R >= Dv))
    {
      Q++;
      R = R - Dv;
    }
  // Transfer R|Q back to VRs and return
  qu.ulong.upper = R;
  qu.ulong.lower = Q;
  return qu.vx2;
}
 * \endcode
 *
 * For the vec_divdud() implementation we need to vectorize the
 * algorithm above. For example:
 * \code
vui64_t test_vec_divdud_V2 (vui64_t x, vui64_t y, vui64_t z)
{
#if defined (_ARCH_PWR8)
  vui64_t Q, R, Qt, Rt;
  vui64_t r1, r2, q1, q2;
  vb64_t CC, c1, c2;
  const vui64_t ones = vec_splat_u64(1);

  // Based on the PowerISA, Programming Note for
  // Divide Word Extended [Unsigned] but vectorized
  // for vector long long int
  q1 = vec_vdiveud_inline (x, z);
  q2 = vec_vdivud_inline  (y, z);
  r1 = vec_muludm (q1, z);

  r2 = vec_muludm (q2, z);
  r2 = vec_subudm (y, r2);
  Q  = vec_addudm (q1, q2);
  R  = vec_subudm (r2, r1);

  c1 = vec_cmpltud (R, r2);
  c2 = vec_cmpgeud (R, z);
  CC = vec_or (c1, c2);
#if 1 // Corrected Quotient returned for divdud.
  Qt = vec_addudm (Q, ones);
  Q = vec_selud (Q, Qt, CC);
  return Q;
#else // Corrected Remainder returned for moddud.
  Rt = vec_subudm (R, z);
  R = vec_selud (R, Rt, CC);
  return R;
#endif
#else
  // P7 Missing some vector DW operations, so use divqud to avoid them.
  vui128_t xy_h, xy_l;
  vui64_t QQ, RQ_h, RQ_l, z_l;

  // merge high x, y into quadword xy_h
  xy_h = (vui128_t) vec_mrghd (x, y);
  // merge low x, y into quadword xy_l
  xy_l = (vui128_t) vec_mrgld (x, y);
  z_l  = vec_swapd (z);
  RQ_h = vec_divqud_inline (xy_h, z);
  RQ_l = vec_divqud_inline (xy_l, z_l);
  // Merge DW Quotients into vector DW int.
  QQ   = vec_mrgld (RQ_h, RQ_l);
  return QQ;
#endif
}
 * \endcode
 * As we are using the PVECLiB doubleword operations from
 * vec_int64_ppc.h this implementation will generate new vector divide
 * instructions for POWER10 and functionally equivalent scalar/vector
 * based code for POWER9/8.
 *
 * \paragraph i64_missing_ops_0_2_2_3 Special consideration for POWER7 and earlier
 *
 * The questions remains which implementation should we use for POWER7
 * and earlier:
 * - \ref i64_missing_ops_0_2_2_1 or
 * - \ref i64_missing_ops_0_2_2_2
 *
 * This early PowerISA implementation does not have the
 * vector doubleword integer instructions that arrived with POWER8.
 * Especially the Move From/To VSR Doubleword instructions and so the
 * generated code has to use store/load sequences to transfer the
 * doublewords.
 * For POWER7 this usually implies Load-Hit-Store reject and pipeline
 * stalls until the store completes into the L2 Cache.
 * This can add many 10s of cycles to the execution for these operations.
 *
 * On the other hand POWER7 includes all the Fixed-point Unit
 * scalar operations and dual-issue capabilities.
 * This is an attractive alternative given the lack of vector
 * doubleword integer instructions for add/subtract/shift/compare.
 *
 * PVECLIB does provide the equivalent vector doubleword
 * operations for POWER7.
 * This is best-effort using vector word add/subtract
 * (with carry) and compare word operations.
 * These expand into significant (2-3X) code
 * for the loop body of \ref i64_missing_ops_0_2_2_1.
 * Given that the \ref i64_missing_ops_0_2_2_1 implementation
 * is already an order of magnitude slower then
 * \ref i64_missing_ops_0_2_2_2, even an additional 100 cycles
 * for load-hit-store is a net win for doubleword divide on POWER7.
 *
 * \note The relative merits for implementation of the other
 * (quadword, word, halfword, byte) element divides, will different,
 * and should be evaluated on a case-by-case basis.
 *
 * \subsection i64_missing_ops_0_2_0 Loading small Doubleword constants
 *
 * Programming with vector doubleword integers will need doubleword
 * constants for masking and arithmetic operations.
 * Doubleword <I>splat</I> constants are common in vectorized long
 * integer code for arithmetic, comparison, and mask operations.
 * For example:
 * \code
  vui64_t
  __test_incud_V0 (vui64_t vra)
  {
    // increament unsigned doubleword elements
    return vra + 1;
  }
 * \endcode
 * The endian sensitive macros from vec_common_ppc.h can be used
 * to construct doubleword integer constants.
 * For example:
 * \code
   const vui64_t dw_one = CONST_VINT64_DW(1, 1);
   const vui64_t dw_ten = CONST_VINT64_DW(10, 10);
   const vui64_t dw_sign_mask = (vui64_t) CONST_VINT128_W(0x80000000, 0x0,
                                                          0x80000000, 0x0);
 * \endcode
 *
 * In most cases this compiler will allocate these constant values to
 * the read-only data (.rodata) section. When these constants are
 * referenced in programming operations the compiler generates the
 * appropriate vector loads.
 * For example the GCC V11 generates the following for the
 * <B>-mcpu=power8</B> target:
 * \code
     addis   r9,r2,.rodata.cst16+0x30@toc@ha
     addi    r9,r9,.rodata.cst16+0x30@toc@l
     lvx     v0,0,r9	# Load { 1, 1 }
     vaddudm v2,v2,v0	# vra + 1
 * \endcode
 * The addis/addi/lvx pattern is common to loading most vector constants
 * for POWER8 and earlier.
 *
 * For some odd reason the compiler might generate the sequence:
 * \code
     addis   r9,r2,.rodata.cst16+0x30@toc@ha
     addi    r9,r9,.rodata.cst16+0x30@toc@l
     rldicr  r9,r9,0,59
     lxvd2x  vs0,0,r9
     xxswapd vs0,vs0
 * \endcode
 * for <B>-mcpu=power8</B> ppc64le targets.
 *
 * The <I>Load VSX Vector Dword*2 Indexed (<B>lxvd2x</B>)</I> would be
 * required if the compiler could not know that the data was quadword
 * aligned. The <B>lxvd2x</B> instruction handles unaligned access but
 * requires the <I>little endian adjustment</I> (xxswapd).
 * However the compiler controls the allocation and alignment of vector
 * constants in .rodata and already insures quadword alignment.
 *
 * \note This is has the look of a compiler phase error bug where
 * important information is lost between compiler phases.
 *
 * For the <B>-mcpu=power9</B> (and later) target
 * GCC uses the <I>Load VXS Vector (<B>lxv</B>)</I> instruction:
 * \code
     addis   r9,r2,.rodata.cst16+0x30@toc@ha
     addi    r9,r9,.rodata.cst16+0x30@toc@l
     lxv     v2,0(r9)
 * \endcode
 * The first sequence is expected for POWER8 as PowerISA 2.07B does not
 * have any displacement form (D-Form) vector (VSX) load/store
 * instructions. The compiler allocates constants to the .rodata
 * sections and the linker collects .rodata from object files into a
 * combined executable .rodata section. This section is placed near the
 * <I>Table of Contents (<B>TOC</B>)</I> section.
 * The ABI dedicates R2 as the base address <B>.TOC.</B> for the TOC
 * and adjacent sections.
 *
 * The <I>Add Immediate Shifted (addis)</I> <I>Add Immediate (addi)</I>
 * sequence above computes a signed 32-bit <B>.TOC.</B>
 * relative offset to a specific .rodata quadword. Two instructions are
 * required as; <I>addis</I> provides the
 * <I>high adjusted (<B>\@ha</B>)</I> 16-bits
 * shifted left 16-bits, while <I>addi</I> provides the
 * <I>low (<B>\@l</B>)</I> 16-bits.
 * The sum of R2 and these immediate values is the 64-bit effective
 * address of a .rodata constant value.
 * A signed 32-bit offset is large enough to support most
 * (<I>-mcmodel=medium</I>) program and library executables.
 *
 * The load itself has a 5-cycle latency assuming a L1 cache hit.
 * The three instruction sequence is sequentially dependent
 * and requires 9-cycles latency (minimum) to execute.
 * A L1 cache miss will increase the latency by 7-28 cycles,
 * assuming the data resides in the L2/L3 caches.
 *
 * \subsubsection int64_const_0_2_0 Optimizing loads from .rodata
 *
 * However the compiler is not following the recommendations of
 * <a href="https://ibm.ent.box.com/s/jd5w15gz301s5b5dt375mshpq9c3lh4u">
 * PowerISA 2.07B</a>, <I>Book II,
 * Chapter 2.1 Performance-Optimized Instruction Sequences</I>.
 * This chapter recommends a specific pattern for the addi/lvx sequence.
 * For example:
 * \code
     addis   rA,r2,.rodata.cst16+0x30@toc@ha
     addi    rx,0,.rodata.cst16+0x30@toc@l
     lvx     v2,rA,rx
 * \endcode
 * In this case rx can be any GPR (including r0) while RA must be a
 * valid base (r1 <-> r31) register.
 *
 * The POWER8 implementation allows for <I>Instruction Funsion</I> combining
 * information from two <I>adjacent</I>t instructions into one (internal)
 * instruction so that it executes faster than the non-fused case.
 * Effectively the addi/lvx combination above becomes a D-Form
 * load vector instruction.
 *
 * There are additional restrictions on the definition of
 * <I>adjacent</I>:
 * - The instruction must be in the same dispatch group.
 *   - In single-threaded mode, up to six non-branch and up to two
 *     branch instructions (6/2 groups).
 *   - In multi-threaded mode, up to three non-branch and up to one
 *     branch instructions (3/1 groups).
 * - Without any intervening branch instructions.
 * - Instructions may span an I-cache line,
 *   with both lines fetched and residing in the i-buffer.
 *
 * This can reduce the latency from 9 to 7-cycles. This would be true
 * even without <I>Instruction Funsion</I> as the addis/addi
 * instructions are now independent and can execute in parallel.
 *
 * The sequence generated for POWER9 is even more disappointing.
 * The lxv is a D-Form (DQ) instruction and the displacement operand
 * could be used to replace the addi instruction.
 * For example:
 * <B>-mcpu=power9</B> target:
 * \code
     addis   r9,r2,.rodata.cst16+0x30@toc@ha
     lxv     v2,.rodata.cst16+0x30@toc@l(r9)
 * \endcode
 * This provides the equivalent 32-bit TOC relative displacement with
 * one less instructions and reduced latency of 7-cycles.
 *
 * \subsubsection int64_const_0_2_1 Alternatives to loading from .rodata
 * This is all a little cumbersome and it seems like there should be
 * a better/faster way. Any instruction sequence that loads quadword
 * integer constants in:
 * - three instruction or less,
 * - latency of 6 cycles or less,
 * - and avoids cache misses
 *
 * is a good deal.
 *
 * The base (Altivec) vector ISA included
 * Vector Splat Immediate Signed Byte/Halfword/Word instructions.
 * These are fast (2-cycle latency) and convenient for small integer
 * constants in the range -16 to 15.
 * So far the ISA has not added doubleword or quadword forms
 * of <i>splat immediate</I>.
 *
 * POWER9 added a VSX Vector Splat Immediate Byte (xxspltib) instruction.
 * This expands the immediate range to -128 to 127 but does not include
 * larger element sizes. POWER9 does provide
 * Vector Extend Sign Byte To Word/Doubleword (vextsb2w/vextsb2d)
 * instructions. For example the two instruction sequence:
 * \code
     xxspltib vs34,127
     vextsb2d v2,v2
 * \endcode
 * can generate a doubleword splat immediate for integers in the
 * range -128 to 127 with a cycle latency of 5-cycles.
 *
 * \note POWER10 does add the interesting
 * <I>VSX Vector Splat Immediate Double-Precision</I> instruction.
 * This is a 64-bit instruction with a 32-bit single precision
 * immediate operand. Interesting but not helpful for doubleword
 * integer.
 *
 * \subsubsection int64_const_0_2_2 Some special quadword constants
 * The GCC compiler does recognize some vector constants as special case.
 * For example:
 * \code
vi128_t
__test_splatisq_n1_V0 (void)
{
  const vui32_t q_ones = {-1, -1, -1, -1};
  return (vi128_t) q_ones;
}

vi128_t
__test_splatisq_0_V0 (void)
{
  const vui32_t q_zero = {0, 0, 0, 0};
  return (vi128_t) q_zero;
}
 * \endcode
 * will generate:
 * \code
0000000000000080 <__test_splatisq_n1_V0>:
     vspltisw v2,-1
     blr
00000000000000a0 <__test_splatisq_0_V0>:
     vspltisw v2,0
     blr
 * \endcode
 * As we will see
 * the all zero/ones constants are common building blocks.
 * So the compiler should treat these as common sub expressions
 * across all operations using those constants.
 *
 * \subsubsection int64_const_0_2_3 Defining our own vec_splat_s64
 * So the compiler can do clever things with vector constants.
 * But so far these are the only examples I have found.
 * Other cases that you might expect to be a special case are not.
 * For example:
 * \code
  vui64_t
  __test_splatudi_15_V1 (void)
  {
    return vec_splats ((unsigned long long) 12);
  }

  vui64_t
  __test_splatudi_15_V0 (void)
  {
    const vui64_t dw_15 = CONST_VINT64_DW(15, 15);
    return dw_15;
  }
 * \endcode
 * both generate the 3 instruction (9-cycle) load from .rodata sequence.
 * Also constants using the vector long long or __int128 types may
 * fail to compile on older versions of the compiler.
 *
 * We can generate small constants in the range -16 <-> 15 with using the
 * following pattern:
 * \code
vi64_t
__test_splatsdi_15_V1 (void)
{
  vi32_t vwi = vec_splat_s32 (15);
  return vec_unpackl (vwi);
}
 * \endcode
 * Which should generate:
 * \code
0000000000000040 <__test_splatisd_15_v2>:
      vspltisw v2,15
      vupklsw v2,v2
      blr
 * \endcode
 * Here we use the vec_splat_s32(15) intrinsic to generate
 * <I>Vector Splat Immediate Signed Word (<B>vspltisw</B>)</I>
 * to splat the value 15 across word elements of  <I>vwi</I>.
 * Then vec_unpackl (vwi) to generate
 * <I>Vector Unpack Low Signed Word <B>vupklsw</B></I> which
 * sign extends the 2 low words of <I>vwi</I> to signed doubleword
 * elements.
 * This sequence is only 2 instructions and
 * will execute with 4-cycle latency.
 *
 * \note unfortunately GCC compilers after GCC-8 will recognize this
 * sequence and convert it back to the three instruction .rodata load
 * sequence.
 * See:
 * <a href="https://gcc.gnu.org/bugzilla/show_bug.cgi?id=104124">
 * GCC PR 104124</a>
 * Until PR 104124 is fixed the following work-around is used
 * for the PVECLIB implementation.
 *
 * Putting this all together we can create a static inline function
 * to generate small doubleword constants (in the range -16 to 15).
 * For example:
 * \code
static inline vi64_t
vec_splat_s64_PWR8 (const int sim)
{
  vi64_t result;
  if (__builtin_constant_p (sim) && ((sim >= -16) && (sim < 16)))
    {
      vi32_t vwi = vec_splat_s32 (sim);

      if (__builtin_constant_p (sim) && ((sim == 0) || (sim == -1)))
	{
	  // Special case for -1 and 0. Skip vec_unpackl().
	  result = (vi64_t) vwi;
	} else {
	  // For P8 can use either vupklsh or vupklsw but P7 only has
	  // vupklsh. Given the reduced range, Either works here.
	  // Unpack signed HW works here because immediate value fits
	  // into the low HW and sign extends to high HW of each word.
	  // Unpack will expand the low HW to low word and high HW
	  // (sign extend) into the high word of each DW.
	  // Unpack low/high (or endian) will not change the result.
#if defined (__GNUC__) && (__GNUC__ == 8)
	  // GCC 8 (AT12) handle this correctly.
	  result = (vi64_t) vec_vupklsh ((vi16_t) vwi);
#else
	  // But GCC 9+ optimized the above to be load from .rodata.
	  // With a little register pressure it adds some gratuitous store/reloads.
	  // So the following work-around is required.
	  __asm__(
	      "vupklsh %0,%1;"
	      : "=v" (result)
	      : "v" (vwi)
	      : );
#endif
	}
    }
  else
    result = vec_splats ((signed long long) sim);

  return (result);
}
 * \endcode
 * This version uses only <altivec.h> intrinsics supported by POWER8
 * and earlier.
 * For constants in the range (-16 to 15) the range is divided into
 * two groups:
 * - Special values -1 and 0 that can be generated in a single instruction.
 * - Values -16 to 15 that require the vwi constant to sign extend.
 *
 * Values outside this range use the vec_splats() intrinsic which will
 * generate the appropriate quadword constant in .rodata and the load
 * sequence to retrieve that value.
 *
 * For POWER9 and later we can use the vec_splats() intrinsic
 * which (so far) generates the xxspltib/vextsb2d sequence for the
 * constant range -128 to 127.
 *
 * \code
static inline vi64_t
vec_splat_s64_PWR9 (const int sim)
{
  return vec_splats ((signed long long) sim);
}
 * \endcode
 *
 * \section i64_endian_issues_0_0 Endian problems with doubleword operations
 *
 * From the examples above we see that the construction of higher
 * precision multiplies requires significant massaging of input and
 * output elements.
 * Here merge even/odd, merge high/low, swap, and splat doubleword
 * element operations are commonly used.
 *
 * PowerISA 2.06 VSX (POWER7) added the general purpose
 * Vector Permute Doubleword Immediate (xxpermdi).
 * The compiler generates some form of xxpermdi for the doubleword
 * (double float, long int, bool long) merge/splat/swap operations.
 * As xxpermdi's element selection is an immediate field, most
 * operations require only a single instruction.
 * All the merge/splat/swap doubleword variant are just a specific
 * select mask value and the inputs to xxpermdi.
 *
 * Which is very useful indeed for assembling, disassembling, merging,
 * splatting, swapping, and pasting doubleword elements.
 *
 * Of course it took several compiler releases to implement all the
 * generic merge/splat/swap operations for the supported types.
 * GCC 4.8 as the first to support vec_xxpermdi as a built-in.
 * GCC 4.8 also supported the generic built-ins vec_mergeh, vec_mergel,
 * and vec_splat for the vector signed/unsigned/bool long type.
 * But endian sensitive vec_mergeh, vec_mergel, and vec_splat were not
 * supported until GCC 7.
 * And the generic vec_mergee, vec_mergeo built-ins where not supported
 * until GCC 8.
 *
 * But as we have explained in \ref mainpage_endian_issues_1_1 and
 * \ref i32_endian_issues_0_0 the little endian transforms applied by
 * the compiler can cause problems for developers of multi-precision
 * libraries. The doubleword forms of the generic merge/splat
 * operations etc. are no exception. This is especially annoying when
 * the endian sensitive transforms are applied between releases of the
 * compiler.
 *
 * So we need a strategy to provide endian invariant merge/splat/swap
 * operations to be used in multi-precision arithmetic.
 * And another set of endian sensitive operations that are mandated by
 * the OpenPOWER ABI.
 *
 * First we need a safely endian invariant version of xxpermdi to use
 * in building other variants:
 * - vec_permdi() provides the basic xxpermdi operation but nullifies
 * the little endian transforms.
 *
 * Then build the core set of endian invariant permute doubleword
 * operations using vec_permdi():
 * - Merge algebraic high/low doubleword operations vec_mrgahd() and
 * vec_mrgald().
 * - Merge the left and right most doublewords from a double quadword
 * operation vec_pasted().
 * - Splat from the high/even or low/odd doubleword operation
 * vec_xxspltd().
 * - Swap high and low doublewords operation vec_swapd().
 *
 * We use the merge algebraic high/low doubleword operations in the
 * implementation of vec_mulhud(), vec_mulhud(), vec_vmuleud(),
 * and vec_vmuloud().
 * We use the vec_xxspltd operation in the
 * implementation of vec_vrld(), vec_vmuleud(), and vec_vmuloud().
 * We use the paste doubleword (vec_pasted()) operation in the
 * implementation of vec_vsrad(), vec_vmuleud(), and vec_vmuloud().
 * We use the swap doubleword operation in the implementation of
 * vec_cmpequq(), vec_cmpneuq(), vec_muludq(), and vec_mulluq().
 *
 * Then use the compilers <B>__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__</B>
 * conditional to invert the vec_permdi() select control for
 * endian sensitive merge/splat doubleword operations:
 * - Merge even/odd doubleword operations vec_mrged() and vec_mrgod().
 * - Merge high/low doubleword operations vec_mrghd() and vec_mrgld().
 * - Splat even/odd doubleword operation vec_splatd().
 *
 * \section int64_examples_0_1 Vector Doubleword Examples
 * Suppose we have a requirement to convert an array of 64-bit
 * time-interval values that need to convert to timespec format.
 * For simplicity we will also assume that the array is nicely
 * (Quadword) aligned and an integer multiple of 2 doublewords
 * or 4 words.
 *
 * The PowerISA provides a 64-bit TimeBase register that clocks at a
 * constant 512MHz. The TimeBase can be read directly as either the
 * full 64-bit value or as 32-bit upper and lower halves.  For this
 * example we assume are dealing with longer intervals (greater than
 * ~8.38 seconds) so the full 64-bit TimeBase is required.
 * TimeBase values of adjacent events are subtracted to generate the
 * intervals stored in the array.
 *
 * The timespec format is a struct of unsigned int fields for seconds
 * and nanoseconds.  So the task is to convert the 512MHz 64-bit
 * TimeBase intervals to seconds and remaining clock ticks.
 * Then convert the remaining (subsecond) clock ticks from 512MHz to
 * nanoseconds. The separate seconds and nanoseconds are combined
 * in the timespec structure.
 *
 * First we need to separate the raw TimeBase into the integer
 * seconds and (subsecond) clock-ticks. Normally scalar codes would
 * use integer divide/modulo by 512000000. Did I mention that the
 * PowerISA vector unit does not have a integer divide operation?
 *
 * Instead we can use the multiplicative inverse which is a
 * scaled fixed point fraction calculated from the original divisor.
 * This works nicely if the fixed radix point is just before
 * the 64-bit fraction and we have a multiply high (vec_mulhud())
 * operation. Multiplying a 64-bit unsigned integer by a 64-bit
 * unsigned fraction generates a 128-bit product with 64-bits above
 * (integer) and below (fraction) the radix point.
 * The high 64-bits of the product is the integer quotient.
 *
 * It turns out that generating the multiplicative inverse can be
 * tricky.  To produce correct results over the full range requires,
 * possible pre-scaling and post-shifting, and sometimes a corrective
 * addition is necessary. Fortunately the mathematics are well
 * understood and are commonly used in optimizing compilers.
 * Even better, Henry Warren's book has a whole chapter on this topic.
 * \see "Hacker's Delight, 2nd Edition,"
 * Henry S. Warren, Jr, Addison Wesley, 2013.
 * Chapter 10, Integer Division by Constants.
 *
 * In the chapter above;
 * <BLOCKQUOTE>Figure 10-2 Computing the magic number for unsigned division.</BLOCKQUOTE>
 * provides a sample C function for generating the magic number
 * (actually a struct containing; the magic multiplicative inverse,
 * "add" indicator, and the shift amount.).
 *
 * For the divisor 512000000 this is { 4835703278458516699, 0 , 27 }:
 * - the multiplier is 4835703278458516699.
 * - no corrective add of the dividend is required.
 * - the final shift is 27-bits right.
 *
 * \code
  // Magic numbers for multiplicative inverse to divide by 512,000,000
  // are 4835703278458516699 and shift right 27 bits.
  const vui64_t mul_invs_clock =
    { 4835703278458516699UL, 4835703278458516699UL };
  const int shift_clock = 27;
  // Need const for TB clocks/second to extract remainder.
  const vui64_t tb_clock_sec =
    { 512000000, 512000000};
  vui64_t tb_v, tmp, tb_clocks, seconds, nseconds;
  vui64_t timespec1, timespec2;

  // extract integer seconds from timebase vector.
  tmp = vec_mulhud (tb_v, mul_invs_clock);
  seconds = vec_srdi (tmp, shift_clock);
  // Extract the remainder in tb clock ticks.
  tmp = vec_muludm (seconds, tb_clock_sec);
  tb_clocks = vec_sub (tb_v, tmp);

 * \endcode
 *
 * Next we need to convert the subseconds from TimeBase clock-ticks to
 * nanoseconds. The subsecond remainder is now small enough (compared
 * to a doubleword) that we can perform the conversion <I>in place</I>.
 * The nanosecond conversion is ((tb_clocks * 1000000000) / 512000000).
 * And we can reduce this to ((tb_clocks * 1000) / 512).
 * We still have a small multiply but the divide can be converted to
 * shift right of 9-bits.
 * \code
  const int shift_512 = 9;
  const vui64_t nano_512 =
    { 1000, 1000};

  // Convert 512MHz timebase to nanoseconds.
  // nseconds = tb_clocks * 1000000000 / 512000000
  // reduces to (tb_clocks * 1000) >> 9
  tmp = vec_muludm (tb_clocks, nano_512);
  nseconds = vec_srdi (tmp, shift_512);
 * \endcode
 *
 * Finally we need to merge the vectors of seconds and nanoseconds into
 * vectors of timespec. So far we have been working with 64-bit
 * integers but the timespec is a struct of 32-bit (word) integers.
 * Here 32-bit seconds and nanosecond provided sufficient range and
 * precision. So the final step <I>packs</I> a pair of 64-bit timespec
 * values into a vector of two 32-bit timespec values, each containing
 * 2 32-bit (second, nanosecond) values.
 * \code
  timespec1 = vec_mergeh (seconds, nseconds);
  timespec2 = vec_mergel (seconds, nseconds);
  // seconds and nanoseconds fit int 32-bits after conversion.
  // So pack the results and store the timespec.
  *timespec++ = vec_vpkudum (timespec1, timespec2);
 * \endcode
 * \note vec_sub(), vec_mergeh(), and vec_mergel() are existing
 * altivec.h generic built-ins.
 * \note vec_vpkudum() is an existing altivec.h built-in that is only
 * defined for <B>_ARCH_PWR8</B> and later. This header insures that
 * vec_vpkudum is defined for older compilers and provides an
 * functional equivalent implementation for POWER7.
 *
 * \subsection i64_example_0_1_0 Vectorized 64-bit TimeBase conversion example
 * Here is the complete vectorized 64-bit TimeBase to timespec
 * conversion example:
 * \code
void
example_dw_convert_timebase (vui64_t *tb, vui32_t *timespec, int n)
{
  // Magic numbers for multiplicative inverse to divide by 512,000,000
  // are 4835703278458516699 and shift right 27 bits.
  const vui64_t mul_invs_clock =
    { 4835703278458516699UL, 4835703278458516699UL };
  const int shift_clock = 27;
  // Need const for TB clocks/second to extract remainder.
  const vui64_t tb_clock_sec =
    { 512000000, 512000000};
  const int shift_512 = 9;
  const vui64_t nano_512 =
    { 1000, 1000};
  vui64_t tb_v, tmp, tb_clocks, seconds, nseconds;
  vui64_t timespec1, timespec2;
  int i;

  for (i = 0; i < n; i++)
    {
      tb_v = *tb++;
      // extract integer seconds from timebase vector.
      tmp = vec_mulhud (tb_v, mul_invs_clock);
      seconds = vec_srdi (tmp, shift_clock);
      // Extract remainder in tb clock ticks.
      tmp = vec_muludm (seconds, tb_clock_sec);
      tb_clocks = vec_sub (tb_v, tmp);
      // Convert 512MHz timebase to nanoseconds.
      // nseconds = tb_clocks * 1000000000 / 512000000
      // reduces to (tb_clocks * 1000) >> 9
      tmp = vec_muludm (tb_clocks, nano_512);
      nseconds = vec_srdi (tmp, shift_512);
      // Use merge high/low to interleave seconds and nseconds
      // into timespec.
      timespec1 = vec_mergeh (seconds, nseconds);
      timespec2 = vec_mergel (seconds, nseconds);
      // seconds and nanoseconds fit int 32-bits after conversion.
      // So pack the results and store the timespec.
      *timespec++ = vec_vpkudum (timespec1, timespec2);
    }
}
 * \endcode
 *
 * \section int64_perf_0_0 Performance data.
 * High level performance estimates are provided as an aid to function
 * selection when evaluating algorithms. For background on how
 * <I>Latency</I> and <I>Throughput</I> are derived see:
 * \ref perf_data
 */

///@cond INTERNAL
static inline vb64_t vec_cmpgtsd (vi64_t a, vi64_t b);
static inline vb64_t vec_cmpequd (vui64_t a, vui64_t b);
static inline vb64_t vec_cmpgeud (vui64_t a, vui64_t b);
static inline vb64_t vec_cmpgtud (vui64_t a, vui64_t b);
static inline vb64_t vec_cmpneud (vui64_t a, vui64_t b);
static inline vui64_t vec_divqud_inline (vui128_t x_y, vui64_t z);
static inline vui64_t vec_maxud (vui64_t vra, vui64_t vrb);
static inline vui64_t vec_minud (vui64_t vra, vui64_t vrb);
static inline vui64_t vec_muludm (vui64_t vra, vui64_t vrb);
static inline vui64_t vec_pasted (vui64_t __VH, vui64_t __VL);
static inline vui64_t vec_permdi (vui64_t vra, vui64_t vrb, const int ctl);
static inline vui64_t vec_mrghd (vui64_t __VA, vui64_t __VB);
static inline vui64_t vec_mrgld (vui64_t __VA, vui64_t __VB);
static inline vui64_t vec_swapd (vui64_t vra);
#ifndef vec_popcntd
static inline vui64_t vec_popcntd (vui64_t vra);
#else
/* Work around for GCC PR85830.  */
#undef vec_popcntd
#define vec_popcntd __builtin_vec_vpopcntd
#endif
static inline vui64_t vec_rldi (vui64_t vra, const unsigned int shb);
static inline vui64_t vec_selud (vui64_t vra, vui64_t vrb, vb64_t vrc);
static inline vb64_t vec_setb_sd (vi64_t vra);
static inline vui64_t vec_sldi (vui64_t vra, const unsigned int shb);
static inline vi64_t vec_splat_s64 (const int sim);
static inline vui64_t vec_splat_u64 (const int sim);
static inline vui64_t vec_splatd (vui64_t vra, const int ctl);
static inline vui64_t vec_subudm (vui64_t a, vui64_t b);
static inline vui64_t vec_vdiveud_inline (vui64_t x, vui64_t z);
static inline vui64_t vec_vdivud_inline (vui64_t y, vui64_t z);
static inline vui64_t
vec_vlsidx (const signed long long a, const unsigned long long *b);
static inline void
vec_vstsidx (vui64_t xs, const signed long long ra, unsigned long long *rb);
static inline vui64_t vec_xxspltd (vui64_t vra, const int ctl);
///@endcond

/** \brief Vector Absolute Difference Unsigned Doubleword.
 *
 *  Compute the absolute difference for each doubleword.
 *  For each unsigned doubleword, subtract VRB[i] from VRA[i] and
 *  return the absolute value of the difference.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   4   | 1/cycle  |
 *  |power9   |   5   | 1/cycle  |
 *
 *  @param vra vector of 2 x unsigned doublewords
 *  @param vrb vector of 2 x unsigned doublewords
 *  @return vector of the absolute differences.
 */
static inline vui64_t
vec_absdud (vui64_t vra, vui64_t vrb)
{
  return vec_subudm (vec_maxud (vra, vrb), vec_minud (vra, vrb));
}

/** \brief Vector Add Unsigned Doubleword Modulo.
 *
 *  Add two vector long int values and return modulo 64-bits result.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2     | 2/cycle  |
 *  |power9   | 2     | 2/cycle  |
 *
 *  @param a 128-bit vector long int.
 *  @param b 128-bit vector long int.
 *  @return vector long int sums of a and b.
 */
static inline
vui64_t
vec_addudm(vui64_t a, vui64_t b)
{
  vui32_t r;

#ifdef _ARCH_PWR8
#if defined (vec_vaddudm)
  r = (vui32_t) vec_vaddudm (a, b);
#elif defined (__clang__)
  r = (vui32_t) vec_add (a, b);
#else
  __asm__(
      "vaddudm %0,%1,%2;"
      : "=v" (r)
      : "v" (a),
      "v" (b)
      : );
#endif
#else
  vui32_t c;
  vui32_t z= { 0,0,0,0};
  vui32_t cm= { 0,1,0,1};

  c = vec_vaddcuw ((vui32_t)a, (vui32_t)b);
  r = vec_vadduwm ((vui32_t)a, (vui32_t)b);
  c = vec_and (c, cm);
  c = vec_sld (c, z, 4);
  r = vec_vadduwm (r, c);
#endif
  return ((vui64_t) r);
}

/** \brief Vector Count Leading Zeros Doubleword for unsigned
 *  long long elements.
 *
 *  Count the number of leading '0' bits (0-64) within each doubleword
 *  element of a 128-bit vector.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the Vector Count Leading
 *  Zeros Doubleword instruction <B>vclzd</B>. Otherwise use sequence of
 *  pre 2.07 VMX instructions.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   2   | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as 2 x 64-bit unsigned
 *  long long (doubleword) elements.
 *  @return 128-bit vector with the leading zeros count for each
 *  doubleword element.
 */
static inline vui64_t
vec_clzd (vui64_t vra)
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
  vui32_t n, nt, y, x, m;
  vui32_t z = { 0, 0, 0, 0 };
  vui32_t dlwm = { 0, -1, 0, -1 };

  x = (vui32_t) vra;

  m = (vui32_t) vec_cmpgt (x, z);
  n = vec_sld (z, m, 12);
  y = vec_and (n, dlwm);
  nt = vec_or (x, y);

  n = vec_clzw (nt);
  r = (vui64_t) vec_vsum2sw ((vi32_t) n, (vi32_t) z);
#endif
  return (r);
}

/** \brief Vector Count Trailing Zeros Doubleword for unsigned
 *  long long elements.
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
vec_ctzd (vui64_t vra)
{
  vui64_t r;
#ifdef _ARCH_PWR9
#if defined (vec_cnttz) || defined (__clang__)
  r = vec_cnttz (vra);
#else
  __asm__(
      "vctzd %0,%1;"
      : "=v" (r)
      : "v" (vra)
      : );
#endif
#else
// For _ARCH_PWR8 and earlier. Generate 1's for the trailing zeros
// and 0's otherwise. Then count (popcnt) the 1's. _ARCH_PWR8 uses
// the hardware vpopcntd instruction. _ARCH_PWR7 and earlier use the
// PVECLIB vec_popcntd implementation which runs ~24-33 instructions.
  const vui64_t ones = { -1, -1 };
  vui64_t tzmask;
  // tzmask = (!vra & (vra - 1))
  tzmask = vec_andc (vec_addudm (vra, ones), vra);
  // return = vec_popcnt (!vra & (vra - 1))
  r = vec_popcntd (tzmask);
#endif
  return ((vui64_t) r);
}

/** \brief Vector Compare Equal Signed Doubleword.
 *
 *  Compare each signed long (64-bit) integer and return all '1's,
 *  if a[i] == b[i], otherwise all '0's.
 *
 *  For POWER8 (PowerISA 2.07B) or later, use the Vector Compare
 *  Equal Unsigned DoubleWord (<B>vcmpequd</B>) instruction. Otherwise
 *  use boolean logic using word compares.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2     | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @return 128-bit vector with each dword boolean reflecting compare
 *  equal result for each element.
 */
static inline
vb64_t
vec_cmpeqsd (vi64_t a, vi64_t b)
{
  /* vcmpequd works for both signed and unsigned compares.  */
  return vec_cmpequd ((vui64_t) a, (vui64_t) b);
}

/** \brief Vector Compare Equal Unsigned Doubleword.
 *
 *  Compare each unsigned long (64-bit) integer and return all '1's,
 *  if a[i] == b[i], otherwise all '0's.
 *
 *  For POWER8 (PowerISA 2.07B) or later, use the Vector Compare
 *  Equal Unsigned DoubleWord (<B>vcmpequd</B>) instruction. Otherwise
 *  use boolean logic using word compares.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2     | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @return 128-bit vector with each dword boolean reflecting compare
 *  equal result for each element.
 */
static inline
vb64_t
vec_cmpequd (vui64_t a, vui64_t b)
{
  vb64_t result;
#ifdef _ARCH_PWR8
#if __GNUC__ >= 6
  result = vec_cmpeq(a, b);
#else
  __asm__(
      "vcmpequd %0,%1,%2;\n"
      : "=v" (result)
      : "v" (a),
      "v" (b)
      : );
#endif
#else
  /*
   * Don't have vector compare equal unsigned doubleword until power8.
   * So we have to compare word and unless all_eq we need to do some
   * extra work, ie the words may have different truth values.  So we
   * rotate each doubleword by 32-bits (here we use permute as we don't
   * have rotate doubleword either). Then vand the original word
   * compare and rotated value to get the final value.
   */
  vui8_t permute =
    { 0x04,0x05,0x6,0x7, 0x00,0x01,0x2,0x03, 0x0C,0x0D,0x0E,0x0F, 0x08,0x09,0x0A,0x0B};
  vui32_t r, rr;
  r = (vui32_t) vec_cmpeq ((vui32_t) a, (vui32_t) b);
  if (vec_any_ne ((vui32_t) a, (vui32_t) b))
    {
       rr = vec_perm (r, r, permute);
       r= vec_and (r, rr);
    }
  result = (vb64_t)r;
#endif
  return (result);
}

/** \brief Vector Compare Greater Than or Equal Signed Doubleword.
*
*  Compare each signed long (64-bit) integer and return all '1's,
*  if a[i] >= b[i], otherwise all '0's.
*  Use vec_cmpgtsd with parameters reversed to implement vec_cmpltud,
*  then return the logical inverse.
*
*  |processor|Latency|Throughput|
*  |--------:|:-----:|:---------|
*  |power8   | 4     | 2/cycle  |
*  |power9   | 5     | 2/cycle  |
*
*  @param a 128-bit vector treated as 2 x 64-bit signed long
*  integer (dword) elements.
*  @param b 128-bit vector treated as 2 x 64-bit signed long
*  integer (dword) elements.
*  @return 128-bit vector with each dword boolean reflecting compare
*  greater then or equal result for each element.
*/
static inline
vb64_t
vec_cmpgesd (vi64_t a, vi64_t b)
{
  vb64_t r;
  /* vec_cmpge is implemented as the not of vec_cmplt. And vec_cmplt
     is implemented as vec_cmpgt with parms reversed.  */
  r = vec_cmpgtsd (b, a);
  return vec_nor (r, r);
}

/** \brief Vector Compare Greater Than or Equal Unsigned Doubleword.
 *
 *  Compare each unsigned long (64-bit) integer and return all '1's,
 *  if a[i] >= b[i], otherwise all '0's.
 *  Use vec_cmpgtud with parameters reversed to implement vec_cmpltud,
 *  then return the logical inverse.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     | 2/cycle  |
 *  |power9   | 5     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @return 128-bit vector with each dword boolean reflecting compare
 *  greater then or equal result for each element.
 */
static inline
vb64_t
vec_cmpgeud (vui64_t a, vui64_t b)
{
  vb64_t r;
  /* vec_cmpge is implemented as the not of vec_cmplt. And vec_cmplt
     is implemented as vec_cmpgt with parms reversed.  */
  r = vec_cmpgtud (b, a);
  return vec_nor (r, r);
}

/** \brief Vector Compare Greater Than Signed Doubleword.
 *
 *  Compare each signed long (64-bit) integer and return all '1's,
 *  if a[i] > b[i], otherwise all '0's.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the Vector Compare Greater
 *  Than Unsigned DoubleWord (<B>vcmpgtsd</B>) instruction. Otherwise
 *  use boolean logic using word compares.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2     | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @return 128-bit vector with each dword boolean reflecting compare
 *  greater result for each element.
 */
static inline
vb64_t
vec_cmpgtsd (vi64_t a, vi64_t b)
{
  vb64_t result;
#ifdef _ARCH_PWR8
#if __GNUC__ >= 6
  result = vec_cmpgt(a, b);
#else
  __asm__(
      "vcmpgtsd %0,%1,%2;\n"
      : "=v" (result)
      : "v" (a),
      "v" (b)
      : );
#endif
#else
  vui64_t _A, _B;
  const vui64_t signmask = CONST_VINT128_DW(0x8000000000000000UL,
					    0x8000000000000000UL);
  /* For a signed compare we can flip the sign bits, which give
     unsigned magnitudes, that retain the correct relative different.
   */
  _A = vec_xor ((vui64_t)a, signmask);
  _B = vec_xor ((vui64_t)b, signmask);
  result = vec_cmpgtud (_A, _B);
#endif
  return (result);
}

/** \brief Vector Compare Greater Than Unsigned Doubleword.
 *
 *  Compare each unsigned long (64-bit) integer and return all '1's,
 *  if a[i] > b[i], otherwise all '0's.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the Vector Compare Greater
 *  Than Unsigned DoubleWord (<B>vcmpgtud</B>) instruction. Otherwise
 *  use boolean logic using word compares.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2     | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @return 128-bit vector with each dword boolean reflecting compare
 *  greater result for each element.
 */
static inline
vb64_t
vec_cmpgtud (vui64_t a, vui64_t b)
{
  vb64_t result;
#ifdef _ARCH_PWR8
#if __GNUC__ >= 6
  result = vec_cmpgt(a, b);
#else
  __asm__(
      "vcmpgtud %0,%1,%2;\n"
      : "=v" (result)
      : "v" (a),
      "v" (b)
      : );
#endif
#else
  /*
   * Don't have vector compare greater than unsigned doubleword until
   * power8.  So we have to use compare word and logic to compute the
   * doubleword truth values.
   */
  __vector unsigned int r, x, y;
  __vector unsigned int c0, c1, c01;
  __vector unsigned int eq, gt, a32, b32;

  /* c10 = {0, -1, 0, -1}  */
  c0 = vec_splat_u32 (0);
  c1 = vec_splat_u32 (-1);
  c01 = vec_mergeh (c0, c1);

  a32 = (__vector unsigned int)a;
  b32 = (__vector unsigned int)b;

  gt = (__vector unsigned int)vec_cmpgt (a32, b32);
  eq = (__vector unsigned int)vec_cmpeq (a32, b32);
  /* GTxw = GThw | (EQhw & GTlw)  */
  x = vec_sld (gt, c0, 4);
  y = vec_and (eq, x);
  x = vec_or  (gt, y);
  /* Duplicate result word to dword width.  */
  y = vec_sld (c0, x, 12);
  r = vec_sel (x, y, c01);
  result = (vb64_t)r;
#endif
  return (result);
}

/** \brief Vector Compare Less Than Equal Signed Doubleword.
 *
 *  Compare each signed long (64-bit) integer and return all '1's,
 *  if a[i] > b[i], otherwise all '0's.
 *  Use vec_cmpgtsd with parameters reversed to implement vec_cmpltsd
 *  then return the logical inverse.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     | 2/cycle  |
 *  |power9   | 5     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @return 128-bit vector with each dword boolean reflecting compare
 *  greater result for each element.
 */
static inline
vb64_t
vec_cmplesd (vi64_t a, vi64_t b)
{
  vb64_t result;
  /* vec_cmple is implemented as the not of vec_cmpgt.   */
  result = vec_cmpgtsd (a, b);
  return vec_nor (result, result);
}

/** \brief Vector Compare Less Than Equal Unsigned Doubleword.
 *
 *  Compare each unsigned long (64-bit) integer and return all '1's,
 *  if a[i] > b[i], otherwise all '0's.
 *  Use vec_cmpgtud with parameters reversed to implement vec_cmpltud.
 *  Use vec_cmpgtud then return the logical inverse.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     | 2/cycle  |
 *  |power9   | 5     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @return 128-bit vector with each dword boolean reflecting compare
 *  greater result for each element.
 */
static inline
vb64_t
vec_cmpleud (vui64_t a, vui64_t b)
{
  vb64_t result;
  /* vec_cmple is implemented as the not of vec_cmpgt.   */
  result = vec_cmpgtud (a, b);
  return vec_nor (result, result);
}

/** \brief Vector Compare less Than Signed Doubleword.
 *
 *  Compare each signed long (64-bit) integer and return all '1's,
 *  if a[i] < b[i], otherwise all '0's.
 *  Use vec_cmpgtsd with parameters reversed to implement vec_cmpltsd.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2     | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @return 128-bit vector with each dword boolean reflecting compare
 *  less result for each element.
 */
static inline
vb64_t
vec_cmpltsd (vi64_t a, vi64_t b)
{
  return vec_cmpgtsd (b, a);
}

/** \brief Vector Compare less Than Unsigned Doubleword.
 *
 *  Compare each unsigned long (64-bit) integer and return all '1's,
 *  if a[i] < b[i], otherwise all '0's.
 *  Use vec_cmpgtud with parameters reversed to implement vec_cmpltud.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2     | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @return 128-bit vector with each dword boolean reflecting compare
 *  less result for each element.
 */
static inline
vb64_t
vec_cmpltud (vui64_t a, vui64_t b)
{
  return vec_cmpgtud (b, a);
}

/** \brief Vector Compare Not Equal Signed Doubleword.
 *
 *  Compare each signed long (64-bit) integer and return all '1's,
 *  if a[i] != b[i], otherwise all '0's.
 *  Use vec_cmpequd then return the logical inverse.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     | 2/cycle  |
 *  |power9   | 5     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @return 128-bit vector with each dword boolean reflecting compare
 *  not equal result for each element.
 */
static inline
vb64_t
vec_cmpnesd (vi64_t a, vi64_t b)
{
  return vec_cmpneud ((vui64_t) a, (vui64_t) b);
}

/** \brief Vector Compare Not Equal Unsigned Doubleword.
 *
 *  Compare each unsigned long (64-bit) integer and return all '1's,
 *  if a[i] != b[i], otherwise all '0's.
 *  Use vec_cmpequd then return the logical inverse.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     | 2/cycle  |
 *  |power9   | 5     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @return 128-bit vector with each dword boolean reflecting compare
 *  not equal result for each element.
 */
static inline
vb64_t
vec_cmpneud (vui64_t a, vui64_t b)
{
  vb64_t r;
  /* vec_cmpne is implemented as the not of vec_cmpeq.  */
  r = vec_cmpequd (a, b);
  return vec_nor (r, r);
}

/** \brief Vector Compare all Equal Signed Doubleword.
 *
 *  Compare each signed long (64-bit) integer and return true if all
 *  elements of a and b are equal.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @return boolean int for all 128-bits, true if equal, false otherwise.
 */
static inline
int
vec_cmpsd_all_eq (vi64_t a, vi64_t b)
{
  int result;
#if defined (_ARCH_PWR8) && (__GNUC__ >= 6) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  result = vec_all_eq(a, b);
#else
  result = vec_all_eq((vui32_t)a, (vui32_t)b);
#endif
  return (result);
}

/** \brief Vector Compare all Greater Than or Equal Signed Doubleword.
 *
 *  Compare each signed long (64-bit) integer and return true if all
 *  elements of a >= b.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @return boolean int for all 128-bits, true if all Greater Than,
 *  false otherwise.
 */
static inline
int
vec_cmpsd_all_ge (vi64_t a, vi64_t b)
{
  int result;
#if defined (_ARCH_PWR8) && (__GNUC__ >= 6) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  result = vec_all_ge(a, b);
#else
  vui32_t wt = { -1, -1, -1, -1};
  vb64_t gt_bool = vec_cmpgesd (a, b);
  result = vec_all_eq((vui32_t)gt_bool, wt);
#endif
  return (result);
}

/** \brief Vector Compare all Greater Than Signed Doubleword.
 *
 *  Compare each signed long (64-bit) integer and return true if all
 *  elements of a > b.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @return boolean int for all 128-bits, true if all Greater Than,
 *  false otherwise.
 */
static inline
int
vec_cmpsd_all_gt (vi64_t a, vi64_t b)
{
  int result;
#if defined (_ARCH_PWR8) && (__GNUC__ >= 6) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  result = vec_all_gt(a, b);
#else
  vui32_t wt = { -1, -1, -1, -1};
  vb64_t gt_bool = vec_cmpgtsd (a, b);
  result = vec_all_eq((vui32_t)gt_bool, wt);
#endif
  return (result);
}

/** \brief Vector Compare all Less than equal Signed Doubleword.
 *
 *  Compare each signed long (64-bit) integer and return true if all
 *  elements of a <= b.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @return boolean int for all 128-bits, true if all Greater Than,
 *  false otherwise.
 */
static inline
int
vec_cmpsd_all_le (vi64_t a, vi64_t b)
{
  return vec_cmpsd_all_ge (b, a);
}

/** \brief Vector Compare all Less than Signed Doubleword.
 *
 *  Compare each signed long (64-bit) integer and return true if all
 *  elements of a < b.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @return boolean int for all 128-bits, true if all Greater Than,
 *  false otherwise.
 */
static inline
int
vec_cmpsd_all_lt (vi64_t a, vi64_t b)
{
  return vec_cmpsd_all_gt (b, a);
}

/** \brief Vector Compare all Not Equal Signed Doubleword.
 *
 *  Compare each signed long (64-bit) integer and return true if all
 *  elements of a and b are not equal.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @return boolean int for all 128-bits, true if equal, false otherwise.
 */
static inline
int
vec_cmpsd_all_ne (vi64_t a, vi64_t b)
{
  int result;
#if defined (_ARCH_PWR8) && (__GNUC__ >= 6) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  result = vec_all_ne(a, b);
#else
  vui32_t wt = { -1, -1, -1, -1};
  vb64_t gt_bool = vec_cmpneud ((vui64_t)a, (vui64_t)b);
  result = vec_all_eq((vui32_t)gt_bool, wt);
#endif
  return (result);
}

/** \brief Vector Compare any Equal Signed Doubleword.
 *
 *  Compare each signed long (64-bit) integer and return true if any
 *  elements of a and b are equal.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @return boolean int for all 128-bits, true if equal, false otherwise.
 */
static inline
int
vec_cmpsd_any_eq (vi64_t a, vi64_t b)
{
  int result;
#if defined (_ARCH_PWR8) && (__GNUC__ >= 6) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  result = vec_any_eq(a, b);
#else
  vui32_t wt = { -1, -1, -1, -1};
  vb64_t gt_bool = vec_cmpequd ((vui64_t)a, (vui64_t)b);
  result = vec_any_eq((vui32_t)gt_bool, wt);
#endif
  return (result);
}

/** \brief Vector Compare any Greater Than or Equal Signed Doubleword.
*
*  Compare each signed long (64-bit) integer and return true if any
*  elements of a >= b.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
*
*  @param a 128-bit vector treated as 2 x 64-bit signed long
*  integer (dword) elements.
*  @param b 128-bit vector treated as 2 x 64-bit signed long
*  integer (dword) elements.
*  @return boolean int for all 128-bits, true if all Greater Than,
*  false otherwise.
*/
static inline
int
vec_cmpsd_any_ge (vi64_t a, vi64_t b)
{
  int result;
#if defined (_ARCH_PWR8) && (__GNUC__ >= 6) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  result = vec_any_ge(a, b);
#else
  vui32_t wt = { -1, -1, -1, -1};
  vb64_t gt_bool = vec_cmpgesd (a, b);
  result = vec_any_eq((vui32_t)gt_bool, wt);
#endif
  return (result);
}

/** \brief Vector Compare any Greater Than Signed Doubleword.
 *
 *  Compare each signed long (64-bit) integer and return true if all
 *  elements of a > b.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @return boolean int for all 128-bits, true if all Greater Than,
 *  false otherwise.
 */
static inline
int
vec_cmpsd_any_gt (vi64_t a, vi64_t b)
{
  int result;
#if defined (_ARCH_PWR8) && (__GNUC__ >= 6) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  result = vec_any_gt(a, b);
#else
  vui32_t wt = { -1, -1, -1, -1};
  vb64_t gt_bool = vec_cmpgtsd (a, b);
  result = vec_any_eq((vui32_t)gt_bool, wt);
#endif
  return (result);
}

/** \brief Vector Compare any Less than equal Signed Doubleword.
 *
 *  Compare each signed long (64-bit) integer and return true if any
 *  elements of a <= b.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @return boolean int for any 128-bits, true if any Greater Than,
 *  false otherwise.
 */
static inline
int
vec_cmpsd_any_le (vi64_t a, vi64_t b)
{
  return vec_cmpsd_any_ge (b, a);
}

/** \brief Vector Compare any Less than Signed Doubleword.
 *
 *  Compare each signed long (64-bit) integer and return true if any
 *  elements of a < b.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @return boolean int for any 128-bits, true if any Greater Than,
 *  false otherwise.
 */
static inline
int
vec_cmpsd_any_lt (vi64_t a, vi64_t b)
{
  return vec_cmpsd_any_gt (b, a);
}

/** \brief Vector Compare any Not Equal Signed Doubleword.
 *
 *  Compare each signed long (64-bit) integer and return true if any
 *  elements of a and b are not equal.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit signed long
 *  integer (dword) elements.
 *  @return boolean int for any 128-bits, true if equal, false otherwise.
 */
static inline
int
vec_cmpsd_any_ne (vi64_t a, vi64_t b)
{
  int result;
#if defined (_ARCH_PWR8) && (__GNUC__ >= 6) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  result = vec_any_ne(a, b);
#else
  vui32_t wt = { -1, -1, -1, -1};
  vb64_t gt_bool = vec_cmpneud ((vui64_t)a, (vui64_t)b);
  result = vec_any_eq((vui32_t)gt_bool, wt);
#endif
  return (result);
}

/** \brief Vector Compare all Equal Unsigned Doubleword.
 *
 *  Compare each unsigned long (64-bit) integer and return true if all
 *  elements of a and b are equal.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @return boolean int for all 128-bits, true if equal, false otherwise.
 */
static inline
int
vec_cmpud_all_eq (vui64_t a, vui64_t b)
{
  int result;
#if defined (_ARCH_PWR8) && (__GNUC__ >= 6) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  result = vec_all_eq(a, b);
#else
  result = vec_all_eq((vui32_t)a, (vui32_t)b);
#endif
  return (result);
}

/** \brief Vector Compare all Greater Than or Equal Unsigned Doubleword.
 *
 *  Compare each unsigned long (64-bit) integer and return true if all
 *  elements of a >= b.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @return boolean int for all 128-bits, true if all Greater Than,
 *  false otherwise.
 */
static inline
int
vec_cmpud_all_ge (vui64_t a, vui64_t b)
{
  int result;
#if defined (_ARCH_PWR8) && (__GNUC__ >= 6) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  result = vec_all_ge(a, b);
#else
  vui32_t wt = { -1, -1, -1, -1};
  vb64_t gt_bool = vec_cmpgeud (a, b);
  result = vec_all_eq((vui32_t)gt_bool, wt);
#endif
  return (result);
}

/** \brief Vector Compare all Greater Than Unsigned Doubleword.
 *
 *  Compare each unsigned long (64-bit) integer and return true if all
 *  elements of a > b.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @return boolean int for all 128-bits, true if all Greater Than,
 *  false otherwise.
 */
static inline
int
vec_cmpud_all_gt (vui64_t a, vui64_t b)
{
  int result;
#if defined (_ARCH_PWR8) && (__GNUC__ >= 6) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  result = vec_all_gt(a, b);
#else
  vui32_t wt = { -1, -1, -1, -1};
  vb64_t gt_bool = vec_cmpgtud (a, b);
  result = vec_all_eq((vui32_t)gt_bool, wt);
#endif
  return (result);
}

/** \brief Vector Compare all Less than equal Unsigned Doubleword.
 *
 *  Compare each unsigned long (64-bit) integer and return true if all
 *  elements of a <= b.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @return boolean int for all 128-bits, true if all Greater Than,
 *  false otherwise.
 */
static inline
int
vec_cmpud_all_le (vui64_t a, vui64_t b)
{
  return vec_cmpud_all_ge (b, a);
}

/** \brief Vector Compare all Less than Unsigned Doubleword.
 *
 *  Compare each unsigned long (64-bit) integer and return true if all
 *  elements of a < b.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @return boolean int for all 128-bits, true if all Greater Than,
 *  false otherwise.
 */
static inline
int
vec_cmpud_all_lt (vui64_t a, vui64_t b)
{
  return vec_cmpud_all_gt (b, a);
}

/** \brief Vector Compare all Not Equal Unsigned Doubleword.
 *
 *  Compare each unsigned long (64-bit) integer and return true if all
 *  elements of a and b are not equal.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @return boolean int for all 128-bits, true if equal, false otherwise.
 */
static inline
int
vec_cmpud_all_ne (vui64_t a, vui64_t b)
{
  int result;
#if defined (_ARCH_PWR8) && (__GNUC__ >= 6) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  result = vec_all_ne(a, b);
#else
  vui32_t wt = { -1, -1, -1, -1};
  vb64_t gt_bool = vec_cmpneud (a, b);
  result = vec_all_eq((vui32_t)gt_bool, wt);
#endif
  return (result);
}

/** \brief Vector Compare any Equal Unsigned Doubleword.
 *
 *  Compare each unsigned long (64-bit) integer and return true if any
 *  elements of a and b are equal.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @return boolean int for all 128-bits, true if equal, false otherwise.
 */
static inline
int
vec_cmpud_any_eq (vui64_t a, vui64_t b)
{
  int result;
#if defined (_ARCH_PWR8) && (__GNUC__ >= 6) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  result = vec_any_eq(a, b);
#else
  vui32_t wt = { -1, -1, -1, -1};
  vb64_t gt_bool = vec_cmpequd (a, b);
  result = vec_any_eq((vui32_t)gt_bool, wt);
#endif
  return (result);
}

/** \brief Vector Compare any Greater Than or Equal Unsigned Doubleword.
*
*  Compare each unsigned long (64-bit) integer and return true if any
*  elements of a >= b.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
*
*  @param a 128-bit vector treated as 2 x 64-bit unsigned long
*  integer (dword) elements.
*  @param b 128-bit vector treated as 2 x 64-bit unsigned long
*  integer (dword) elements.
*  @return boolean int for all 128-bits, true if all Greater Than,
*  false otherwise.
*/
static inline
int
vec_cmpud_any_ge (vui64_t a, vui64_t b)
{
  int result;
#if defined (_ARCH_PWR8) && (__GNUC__ >= 6) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  result = vec_any_ge(a, b);
#else
  vui32_t wt = { -1, -1, -1, -1};
  vb64_t gt_bool = vec_cmpgeud (a, b);
  result = vec_any_eq((vui32_t)gt_bool, wt);
#endif
  return (result);
}

/** \brief Vector Compare any Greater Than Unsigned Doubleword.
 *
 *  Compare each unsigned long (64-bit) integer and return true if all
 *  elements of a > b.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @return boolean int for all 128-bits, true if all Greater Than,
 *  false otherwise.
 */
static inline
int
vec_cmpud_any_gt (vui64_t a, vui64_t b)
{
  int result;
#if defined (_ARCH_PWR8) && (__GNUC__ >= 6) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  result = vec_any_gt(a, b);
#else
  vui32_t wt = { -1, -1, -1, -1};
  vb64_t gt_bool = vec_cmpgtud (a, b);
  result = vec_any_eq((vui32_t)gt_bool, wt);
#endif
  return (result);
}

/** \brief Vector Compare any Less than equal Unsigned Doubleword.
 *
 *  Compare each unsigned long (64-bit) integer and return true if any
 *  elements of a <= b.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @return boolean int for any 128-bits, true if any Greater Than,
 *  false otherwise.
 */
static inline
int
vec_cmpud_any_le (vui64_t a, vui64_t b)
{
  return vec_cmpud_any_ge (b, a);
}

/** \brief Vector Compare any Less than Unsigned Doubleword.
 *
 *  Compare each unsigned long (64-bit) integer and return true if any
 *  elements of a < b.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @return boolean int for any 128-bits, true if any Greater Than,
 *  false otherwise.
 */
static inline
int
vec_cmpud_any_lt (vui64_t a, vui64_t b)
{
  return vec_cmpud_any_gt (b, a);
}

/** \brief Vector Compare any Not Equal Unsigned Doubleword.
 *
 *  Compare each unsigned long (64-bit) integer and return true if any
 *  elements of a and b are not equal.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @param b 128-bit vector treated as 2 x 64-bit unsigned long
 *  integer (dword) elements.
 *  @return boolean int for any 128-bits, true if equal, false otherwise.
 */
static inline
int
vec_cmpud_any_ne (vui64_t a, vui64_t b)
{
  int result;
#if defined (_ARCH_PWR8) && (__GNUC__ >= 6) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  result = vec_any_ne(a, b);
#else
  vui32_t wt = { -1, -1, -1, -1};
  vb64_t gt_bool = vec_cmpneud (a, b);
  result = vec_any_eq((vui32_t)gt_bool, wt);
#endif
  return (result);
}

/** \brief Vector Divide Double Unsigned Doubleword.
 *
 *  A vectorized 128-bit by 64=bit divide returning two 64-bit
 *  Unsigned Doubleword quotients.
 *  The corresponding Doubleword elements of vectors x and y are
 *  concatenated to from the 128-bit dividends.
 *  For integer value i from 0 to 1 over doubleword elements:
 *  quotient[i] = (x[i] || y[i]) / z[i].
 *  The quotients of are returned as a
 *  vector unsigned long long int.
 *
 *  \note The quotient element results may be undefined if;
 *  the quotient cannot be represented in 64-bits,
 *  or the corresponding divisor element is 0.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | ~78   |1/40 cycle|
 *  |power9   | 67-72 |1/9 cycle |
 *  |power10  | 38-88 |2/22 cycle|
 *
 *  @param x 128-bit vector of the high 64-bit elements of the 128-bit dividends.
 *  @param y 128-bit vector of the low 64-bit elements of the 128-bit dividends.
 *  @param z 128-bit vector of 64-bit elements for the divisor.
 *  @return The quotients in a vector unsigned long long int.
 */
static inline vui64_t
vec_divdud_inline (vui64_t x, vui64_t y, vui64_t z)
{
#if defined (_ARCH_PWR8)
  vui64_t Q, R;
  vui64_t r1, r2, q1, q2;
  vb64_t CC, c1, c2;
  const vui64_t ones = vec_splat_u64(1);

  // Based on the PowerISA, Programming Note for
  // Divide Word Extended [Unsigned] but vectorized
  // for vector long long int
  q1 = vec_vdiveud_inline (x, z);
  q2 = vec_vdivud_inline  (y, z);
  r1 = vec_muludm (q1, z);

  r2 = vec_muludm (q2, z);
  r2 = vec_subudm (y, r2);
  Q  = vec_addudm (q1, q2);
  R  = vec_subudm (r2, r1);

  c1 = vec_cmpltud (R, r2);
  c2 = vec_cmpgeud (R, z);
  CC = vec_or (c1, c2);
#if 1
  vui64_t Qt;
  Qt = vec_addudm (Q, ones);
  Q = vec_selud (Q, Qt, CC);
  return Q;
#else // Corrected Remainder not returned for divdud.
  vui64_t Rt;
  Rt = vec_subudm (R, z);
  R = vec_selud (R, Rt, CC);
  return R;
#endif
#else // defined (_ARCH_PWR7)
  // P7 Missing some DW operations, so use divqud to avoid them.
  vui128_t xy_h, xy_l;
  vui64_t QQ, RQ_h, RQ_l, z_l;

  xy_h = (vui128_t) vec_mrghd (x, y);
  xy_l = (vui128_t) vec_mrgld (x, y);
  z_l  = vec_swapd (z);
  RQ_h = vec_divqud_inline (xy_h, z);
  RQ_l = vec_divqud_inline (xy_l, z_l);
  QQ   = vec_mrgld (RQ_h, RQ_l);
  return QQ;
#endif
}

/** \brief Vector Divide Quadword Unsigned by Doubleword.
 *
 *  A vector implementation of a 128-bit by 64-bit divide returning
 *  64-bit remainder and quotient.
 *  The quadword element x_y is the 128-bit dividend.
 *  The high-order doubleword element of z is the divisor
 *  (the low-order element of z is not used).
 *  The 64-bit remainder/quotient are returned as the
 *  high/low order elements of a vector unsigned long long int.
 *
 *  \note The quotient element results may be undefined if;
 *  the quotient cannot be represented in 64-bits,
 *  or the corresponding divisor element is 0.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 30-41 |1/23 cycle|
 *  |power9   | 35-63 |1/9 cycle |
 *  |power10  | 36-64 |1/11 cycle|
 *
 *  @param x_y 128-bit vector 128-bit dividend.
 *  @param z 128-bit vector of 64-bit elements. The high doubleword is the divisor.
 *  @return The remainder/quotient in a vector unsigned long long int.
 */
static inline vui64_t
vec_divqud_inline (vui128_t x_y, vui64_t z)
{
#if defined (_ARCH_PWR7)
  // POWER8/9 Do not have vector integer divide, but do have
  // Move To/From Vector-Scalar Register Instructions
  // So we can use the scalar hardware divide/divide extended
  __VEC_U_128 qu, xy, zu;
#if (__GNUC__ <= 10) &&  defined (_ARCH_PWR8)
  xy.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) x_y, 1);
  xy.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) x_y, 0);
  zu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) z, 1);
  zu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) z, 0);
#else
  // Looks like AT15/16 handles this but what about AT14 ...
  // AT10 does not.
  xy.vx1 = x_y;
  zu.vx2 = z;
#endif
  unsigned long long Dh = xy.ulong.upper;
  unsigned long long Dl = xy.ulong.lower;
  unsigned long long Dv = zu.ulong.upper;
  unsigned long long q1, q2, Q;
  unsigned long long r1, r2, R;

  // Transfer to GPUs and use scalar divide/divide extended
  // Based on the PowerISA, Programming Note for
  // Divide Word Extended [Unsigned]
  q1 = __builtin_divdeu (Dh, Dv);
  //r1 = -(q1 * Dv);
  r1 = (q1 * Dv);
#if  defined (_ARCH_PWR9)
  // Prevent the compiler from generating modud
  __asm__(
      "divdu %0,%1,%2;\n"
      : "=r" (q2)
      : "r" (Dl), "r" (Dv)
      : );
#else
  q2 = Dl / Dv;
#endif
  r2 = Dl - (q2 * Dv);
  Q = q1 + q2;
  //R = r1 + r2;
  R = r2 - r1;
  if ((R < r2) | (R >= Dv))
    {
      Q++;
      R = R - Dv;
    }

  // Transfer R|Q back to VRs and return
  qu.ulong.upper = R;
  qu.ulong.lower = Q;
  return qu.vx2;
#else
  /* Based on Hacker's Delight (2nd Edition) Figure 9-2.
   * "Divide long unsigned shift-and-subtract algorithm."
   * Converted to use vector unsigned __int128 and PVEClIB
   * operations.
   * As cmpgeuq is based on detecting the carry-out of (x -z) and
   * setting the bool via setb_cyq, we can use this carry (variable t)
   * to generate quotient bits.
   * Multi-precision shift-left is simpler then general addition,
   * so we can simplify carry generation. This allows delaying the
   * the y left-shift / quotient accumulation to a later.
   * */
  int i;
  vui64_t ge;
  //vui128_t cc, c;
  vui64_t t, xt, mone;
  const vui64_t zeros = vec_splat_u64(0);
  const vui8_t ones8 = vec_splat_u8(1);
  // t = (vui64_t) CONST_VINT128_DW (0, 0);
  mone = (vui64_t) CONST_VINT128_DW (-1, -1);
  /* Here only using the high DW of z, generated z as {z'', -1} */
  z = vec_pasted (z, mone);

  for (i = 1; i <= 64; i++)
    {
      // Left shift (x || y) requires 129-bits, is (t || x || y)
      /* capture high bit of x_y as bool t */
#if defined (_ARCH_PWR8)
      t = (vui64_t) vec_cmpltsd ((vi64_t) x_y, (vi64_t) zeros);
#else
      { // P7 and earlier did not support DW int.
	// But only need to convert the sign-bit into a bool
	vui32_t lts;
	lts = (vui32_t) vec_cmplt ((vi32_t) x_y, (vi32_t) zeros);
	t = (vui64_t) vec_splat (lts, VEC_W_H);
      }
#endif
      // Then shift left Quadword x_y by 1 bit;
      //x_y = vec_slqi (x_y, 1);
      // Can't user slqi here, forward dependency to vec_int128_ppc.h
      x_y = (vui128_t) vec_sll ((vui32_t) x_y, (vui32_t)ones8);
      /* We only need the high DW of t and ge */
      /* deconstruct ((t || x) >= z) to (t || (x >= z)) */
#if defined (_ARCH_PWR8)
      // vec_cmpge (x_y,z) is NOT vec_cmpgt (z, x_y)
      ge = (vui64_t) vec_cmpgtud (z, (vui64_t)x_y);
      /* Combine t with (x >= z) for 129-bit compare */
      ge = (vui64_t) vec_orc ((vui32_t)t, (vui32_t)ge);
#else // P7 and earlier did not support OR Complement
      ge = (vui64_t) vec_cmpgeud ((vui64_t)x_y, z);
      /* Combine t with (x >= z) for 129-bit compare */
      ge = (vui64_t) vec_or ((vui32_t)t, (vui32_t)ge);
#endif
      /* Splat the high ge DW to both DWs for select */
      ge = vec_splatd (ge, VEC_DW_H);

      /* xt <- {(x - z), (y - ( -1)} */
      xt = vec_subudm ((vui64_t)x_y, z);
      x_y = (vui128_t)vec_selud ((vui64_t)x_y, xt, (vb64_t)ge);
    }
  return (vui64_t)x_y;
#endif
}

/** \brief Vector Maximum Signed Doubleword.
 *
 *  For each doubleword element [0|1] of vra and vrb compare as
 *  signed integers and return the larger value in the result.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2     | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vra 128-bit vector long int.
 *  @param vrb 128-bit vector long int.
 *  @return vector long maximum of a and b.
 */
static inline
vi64_t
vec_maxsd (vi64_t vra, vi64_t vrb)
{
  vi64_t r;

#ifdef _ARCH_PWR8
#if defined (vec_vmaxsd)
  r = vec_vmaxsd (vra, vrb);
#elif defined (__clang__) || (__GNUC__ >= 10)
  r = vec_max (vra, vrb);
#else
  __asm__(
      "vmaxsd %0,%1,%2;"
      : "=v" (r)
      : "v" (vra),
      "v" (vrb)
      : );
#endif
#else
  vb64_t maxmask;

  maxmask = vec_cmpgtsd ( vra, vrb );
  r = vec_sel (vrb, vra, maxmask);
#endif
  return r;
}

/** \brief Vector Maximum Unsigned Doubleword.
 *
 *  For each doubleword element [0|1] of vra and vrb compare as
 *  unsigned integers and return the larger value in the result.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2     | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vra 128-bit vector long int.
 *  @param vrb 128-bit vector long int.
 *  @return vector unsigned long maximum of a and b.
 */
static inline
vui64_t
vec_maxud (vui64_t vra, vui64_t vrb)
{
  vui64_t r;

#ifdef _ARCH_PWR8
#if defined (vec_vmaxud)
  r = vec_vmaxud (vra, vrb);
#elif defined (__clang__) || (__GNUC__ >= 10)
  r = vec_max (vra, vrb);
#else
  __asm__(
      "vmaxud %0,%1,%2;"
      : "=v" (r)
      : "v" (vra),
      "v" (vrb)
      : );
#endif
#else
  vb64_t maxmask;

  maxmask = vec_cmpgtud ( vra, vrb );
  r = vec_sel (vrb, vra, maxmask);
#endif
  return r;
}

/** \brief Vector Minimum Signed Doubleword.
 *
 *  For each doubleword element [0|1] of vra and vrb compare as
 *  signed integers and return the smaller value in the result.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2     | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vra 128-bit vector long int.
 *  @param vrb 128-bit vector long int.
 *  @return vector long minimum of a and b.
 */
static inline
vi64_t
vec_minsd (vi64_t vra, vi64_t vrb)
{
  vi64_t r;

#ifdef _ARCH_PWR8
#if defined (vec_vminsd)
  r = vec_vminsd (vra, vrb);
#elif defined (__clang__) || (__GNUC__ >= 10)
  r = vec_min (vra, vrb);
#else
  __asm__(
      "vminsd %0,%1,%2;"
      : "=v" (r)
      : "v" (vra),
      "v" (vrb)
      : );
#endif
#else
  vb64_t minmask;

  minmask = vec_cmpgtsd ( vrb, vra );
  r = vec_sel (vrb, vra, minmask);
#endif
  return r;
}

/** \brief Vector Minimum Unsigned Doubleword.
 *
 *  For each doubleword element [0|1] of vra and vrb compare as
 *  unsigned integers and return the smaller value in the result.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2     | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vra 128-bit vector unsigned long int.
 *  @param vrb 128-bit vector unsignedlong int.
 *  @return vector unsigned long minimum of a and b.
 */
static inline
vui64_t
vec_minud (vui64_t vra, vui64_t vrb)
{
  vui64_t r;

#ifdef _ARCH_PWR8
#if defined (vec_vminud)
  r = vec_vminud (vra, vrb);
#elif defined (__clang__) || (__GNUC__ >= 10)
  r = vec_min (vra, vrb);
#else
  __asm__(
      "vminud %0,%1,%2;"
      : "=v" (r)
      : "v" (vra),
      "v" (vrb)
      : );
#endif
#else
  vb64_t minmask;

  minmask = vec_cmpgtud ( vrb, vra );
  r = vec_sel (vrb, vra, minmask);
#endif
  return r;
}

/** \brief Vector Modulo Double Unsigned Doubleword.
 *
 *  A vectorized 128-bit by 64=bit modulo divide returning
 *  two 64-bit Unsigned Doubleword remainders.
 *  The corresponding Doubleword elements of vectors x and y are
 *  concatenated to from the 128-bit dividends.
 *  For integer value i from 0 to 1 over doubleword elements:
 *  remainder[i] = (x[i] || y[i]) % z[i].
 *  The remainders of are returned as a
 *  vector unsigned long long int.
 *
 *  \note The remainder element results may be undefined if;
 *  the quotient cannot be represented in 64-bits,
 *  or the corresponding divisor element is 0.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  ~38  |1/40 cycle|
 *  |power9   | 67-72 |1/9 cycle |
 *  |power10  | 38-88 |2/22 cycle|
 *
 *  @param x 128-bit vector of the high 64-bit elements of the 128-bit dividends.
 *  @param y 128-bit vector of the low 64-bit elements of the 128-bit dividends.
 *  @param z 128-bit vector of 64-bit elements for the divisor.
 *  @return The remainders in a vector unsigned long long int.
 */
static inline vui64_t
vec_moddud_inline (vui64_t x, vui64_t y, vui64_t z)
{
#if defined (_ARCH_PWR8)
  vui64_t R;
  vui64_t r1, r2, q1, q2;
  vb64_t CC, c1, c2;

  // Based on the PowerISA, Programming Note for
  // Divide Word Extended [Unsigned] but vectorized
  // for vector long long int
  q1 = vec_vdiveud_inline (x, z);
  q2 = vec_vdivud_inline  (y, z);
  r1 = vec_muludm (q1, z);

  r2 = vec_muludm (q2, z);
  r2 = vec_subudm (y, r2);
  R  = vec_subudm (r2, r1);

  c1 = vec_cmpltud (R, r2);
  c2 = vec_cmpgeud (R, z);
  CC = vec_or (c1, c2);
#if 0
  vui64_t Q, Qt;
  const vui64_t ones = vec_splat_u64(1);
  Q  = vec_addudm (q1, q2);
  Qt = vec_addudm (Q, ones);
  Q = vec_selud (Q, Qt, CC);
  return Q;
#else // Corrected Remainder not returned for divdud.
  vui64_t Rt;
  Rt = vec_subudm (R, z);
  R = vec_selud (R, Rt, CC);
  return R;
#endif
#else // defined (_ARCH_PWR7)
  // P7 Missing some DW operations, so use divqud to avoid them.
  vui128_t xy_h, xy_l;
  vui64_t RR, RQ_h, RQ_l, z_l;

  xy_h = (vui128_t) vec_mrghd (x, y);
  xy_l = (vui128_t) vec_mrgld (x, y);
  z_l  = vec_swapd (z);
  RQ_h = vec_divqud_inline (xy_h, z);
  RQ_l = vec_divqud_inline (xy_l, z_l);
  RR   = vec_mrghd (RQ_h, RQ_l);
  return RR;
#endif
}

/** \brief Vector Merge Algebraic High Doublewords.
 *
 * Merge only the high doublewords from 2 x Algebraic quadwords
 * across vectors vra and vrb. This is effectively the Vector Merge
 * Even Doubleword operation that is not modified for endian.
 *
 * For example, merge the high 64-bits from 2 x 128-bit products
 * as generated by vec_muleud/vec_muloud. This result is effectively
 * a vector multiply high unsigned doubleword.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   2   | 2/cycle  |
 *
 * @param vra 128-bit vector unsigned __int128.
 * @param vrb 128-bit vector unsigned __int128.
 * @return A vector merge from only the high doublewords of the 2 x
 * algebraic quadwords across vra and vrb.
 */
static inline vui64_t
vec_mrgahd (vui128_t vra, vui128_t vrb)
{
  return vec_permdi ((vui64_t) vra, (vui64_t) vrb, 0);
}

/** \brief Vector Merge Algebraic Low Doublewords.
 *
 * Merge only the low doublewords from 2 x Algebraic quadwords
 * across vectors vra and vrb. This effectively the Vector Merge
 * Odd doubleword operation that is not modified for endian.
 *
 * For example, merge the low 64-bits from 2 x 128-bit products
 * as generated by vec_muleud/vec_muloud. This result is effectively
 * a vector multiply low unsigned doubleword.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   2   | 2/cycle  |
 *
 * @param vra 128-bit vector unsigned __int128.
 * @param vrb 128-bit vector unsigned __int128.
 * @return A vector merge from only the low doublewords of the 2 x
 * algebraic quadwords across vra and vrb.
 */
static inline vui64_t
vec_mrgald (vui128_t vra, vui128_t vrb)
{
  return vec_permdi ((vui64_t) vra, (vui64_t) vrb, 3);
}

/** \brief Vector Merge Even Doubleword.
 *  Merge the even doubleword elements from two vectors into the high
 *  and low doubleword elements of the result. This is effectively the
 *  VSX Permute Doubleword Immediate operation modified for endian.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param __VA a 128-bit vector as the source of the
 *  results even doubleword.
 *  @param __VB a 128-bit vector as the source of the
 *  results odd doubleword.
 *  @return A vector merge from only the even doublewords of the 2 x
 *  quadwords across __VA and __VB.
 */
static inline vui64_t
vec_mrged (vui64_t __VA, vui64_t __VB)
{
  vui64_t result;
  /*
   result[0] = __VA[0];
   result[1] = __VB[0];
   */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  result = vec_permdi (__VB, __VA, 3);
#else
  result = vec_permdi (__VA, __VB, 0);
#endif
  return (result);
}

/** \brief Vector Merge High Doubleword.
 *  Merge the high doubleword elements from two vectors into the high
 *  and low doubleword elements of the result. This is effectively the
 *  VSX Permute Doubleword Immediate operation modified for endian.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param __VA a 128-bit vector as the source of the
 *  results even doubleword.
 *  @param __VB a 128-bit vector as the source of the
 *  results odd doubleword.
 *  @return A vector merge from only the high doublewords of the 2 x
 *  quadwords across __VA and __VB.
 */
static inline vui64_t
vec_mrghd (vui64_t __VA, vui64_t __VB)
{
  vui64_t result;
  /*
   result[0] = __VA[0];
   result[1] = __VB[0];
   */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  result = vec_permdi (__VB, __VA, 3);
#else
  result = vec_permdi (__VA, __VB, 0);
#endif
  return (result);
}

/** \brief Vector Merge Low Doubleword.
 *  Merge the low doubleword elements from two vectors into the high
 *  and low doubleword elements of the result. This is effectively the
 *  VSX Permute Doubleword Immediate operation modified for endian.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param __VA a 128-bit vector as the source of the
 *  results even doubleword.
 *  @param __VB a 128-bit vector as the source of the
 *  results odd doubleword.
 *  @return A vector merge from only the low doublewords of the 2 x
 *  quadwords across __VA and __VB.
 */
static inline vui64_t
vec_mrgld (vui64_t __VA, vui64_t __VB)
{
  vui64_t result;
  /*
   result[0] = __VA[1];
   result[1] = __VB[1];
   */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  result = vec_permdi (__VB, __VA, 0);
#else
  result = vec_permdi (__VA, __VB, 3);
#endif

  return (result);
}

/** \brief Vector Merge Odd Doubleword.
 *  Merge the odd doubleword elements from two vectors into the high
 *  and low doubleword elements of the result. This is effectively the
 *  VSX Permute Doubleword Immediate operation modified for endian.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param __VA a 128-bit vector as the source of the
 *  results even doubleword.
 *  @param __VB a 128-bit vector as the source of the
 *  results odd doubleword.
 *  @return A vector merge from only the odd doublewords of the 2 x
 *  quadwords across __VA and __VB.
 */
static inline vui64_t
vec_mrgod (vui64_t __VA, vui64_t __VB)
{
  vui64_t result;
  /*
   result[0] = __VA[1];
   result[1] = __VB[1];
   */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  result = vec_permdi (__VB, __VA, 0);
#else
  result = vec_permdi (__VA, __VB, 3);
#endif

  return (result);
}

/** \brief \copybrief vec_msumudm()
 *
 * \note this implementation exists in
 * vec_int128_ppc.h as it requires vec_adduqm().
 * See \ref vec_msumudm() for full description.
 */
static inline vui128_t
vec_msumudm (vui64_t a, vui64_t b, vui128_t c);

/** \brief \copybrief vec_muleud()
 *
 * \note this implementation exists in
 * vec_int128_ppc.h as it requires vec_vmuleud and vec_adduqm().
 * See \ref vec_muleud() for full description.
 */
static inline vui128_t
vec_muleud (vui64_t a, vui64_t b);

/** \brief \copybrief vec_mulhud()
 *
 * \note this implementation exists in
 * vec_int128_ppc.h as it requires vec_vmuleud() and vec_vmuloud().
 * See \ref vec_mulhud() for full description.
 */
static inline vui64_t
vec_mulhud (vui64_t vra, vui64_t vrb);

/** \brief \copybrief vec_muloud()
 *
 * \note this implementation exists in
 * vec_int128_ppc.h as it requires vec_vmuloud() and vec_adduqm().
 * See \ref vec_muloud() for full description.
 */
static inline vui128_t
vec_muloud (vui64_t a, vui64_t b);

/** \brief \copybrief vec_muludm()
 *
 * \note this implementation exists in
 * vec_int128_ppc.h as it requires vec_vmuleud() and vec_vmuloud().
 * See \ref vec_muludm() for full description.
 */
static inline vui64_t
vec_muludm (vui64_t vra, vui64_t vrb);

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
vec_pasted (vui64_t __VH, vui64_t __VL)
{
  vui64_t result;
  /*
   result[1] = __VH[1];
   result[0] = __VL[0];
   */
  result = vec_permdi (__VH, __VL, 1);

  return (result);
}

/** \brief Vector Permute Doubleword Immediate.
 *  Combine a doubleword selected from the 1st (vra) vector with
 *  a doubleword selected from the 2nd (vrb) vector.
 *
 *  \note This function implements the operation of a VSX Permute
 *  Doubleword Immediate instruction.
 *  This implementation is NOT Endian sensitive and the function is
 *  stable across BE/LE implementations.
 *
 *  The 2-bit control operand (ctl) selects which doubleword from the
 *  1st and 2nd vector operands are transfered to the result vector.
 *  Control table:
 *  ctl |  vrt[0:63]  | vrt[64:127]
 *  :-: | :---------: | :----------:
 *   0  |  vra[0:63]  | vrb[0:63]
 *   1  |  vra[0:63]  | vrb[64:127]
 *   2  | vra[64:127] | vrb[0:63]
 *   3  | vra[64:127] | vrb[64:127]
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param vra a 128-bit vector as the source of the
 *  high order doubleword of the result.
 *  @param vrb a 128-bit vector as the source of the
 *  low order doubleword of the result.
 *  @param ctl const integer where the low order 2 bits control the
 *  selection of doublewords from input vector vra and vrb.
 *  @return The combined 128-bit vector composed of the high order
 *  doubleword of vra and the low order doubleword of vrb.
 */
static inline vui64_t
vec_permdi (vui64_t vra, vui64_t vrb, const int ctl)
{
  vui64_t result;
#ifdef _ARCH_PWR7
  switch (ctl & 3)
    {
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) || (defined (__clang__) && (__clang_major__ < 7))
    case 0:
      result = vec_xxpermdi (vra, vrb, 0);
      break;
    case 1:
      result = vec_xxpermdi (vra, vrb, 1);
      break;
    case 2:
      result = vec_xxpermdi (vra, vrb, 2);
      break;
    case 3:
      result = vec_xxpermdi (vra, vrb, 3);
      break;
#else
    case 0:
      result = vec_xxpermdi (vrb, vra, 3);
      break;
    case 1:
      result = vec_xxpermdi (vrb, vra, 1);
      break;
    case 2:
      result = vec_xxpermdi (vrb, vra, 2);
      break;
    case 3:
      result = vec_xxpermdi (vrb, vra, 0);
      break;
#endif
    default:
      result = (vui64_t){ 0, 0 };
    }
#else
  /* Current compilers don't accept vector unsigned long int as vector
   * parms to vec_sld, so use vector unsigned int.  The vsldoi
   * instruction does not care).  */
  vui32_t temp;
  switch (ctl & 3)
    {
      case 0:
      temp = vec_sld ((vui32_t) vra, (vui32_t) vra, 8);
      result = (vui64_t) vec_sld (temp, (vui32_t) vrb, 8);
      break;
      case 1:
      temp = vec_sld ((vui32_t) vrb, (vui32_t) vra, 8);
      result = (vui64_t) vec_sld (temp, temp, 8);
      break;
      case 2:
      result = (vui64_t) vec_sld ((vui32_t) vra, (vui32_t) vrb, 8);
      break;
      case 3:
      temp = vec_sld ((vui32_t) vrb, (vui32_t) vrb, 8);
      result = (vui64_t) vec_sld ((vui32_t) vra, temp, 8);
      break;
    }
#endif
  return (result);
}

/** \brief Vector Population Count doubleword.
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
 *  pveclib vec_popcntw to count each word then sum across with Vector
 *  Sum across Half Signed Word Saturate (<B>vsum2sws</B>).
 *
 *  @param vra 128-bit vector treated as 2 x 64-bit integer (dwords)
 *  elements.
 *  @return 128-bit vector with the population count for each dword
 *  element.
 */
#ifndef vec_popcntd
static inline vui64_t
vec_popcntd (vui64_t vra)
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
  vui32_t z= { 0,0,0,0};
  vui32_t x;
  x = vec_popcntw ((vui32_t) vra);
  r = (vui64_t) vec_vsum2sw ((vi32_t) x, (vi32_t) z);
#endif
  return (r);
}
#else
/* Work around for GCC PR85830.  */
#undef vec_popcntd
#define vec_popcntd __builtin_vec_vpopcntd
#endif

/*! \brief byte reverse each doubleword for a vector unsigned long int.
 *
 *  For each doubleword of the input vector, reverse the order of
 *  bytes / octets within the doubleword.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  2-11 | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param vra a 128-bit vector unsigned long int.
 *  @return a 128-bit vector with the bytes of each doubleword
 *  reversed.
 */
static inline vui64_t
vec_revbd (vui64_t vra)
{
  vui64_t result;

#ifdef _ARCH_PWR9
#if defined (vec_revb) || defined (__clang__)
  result = vec_revb (vra);
#else
  __asm__(
      "xxbrd %x0,%x1;"
      : "=wa" (result)
      : "wa" (vra)
      : );
#endif
#else
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  const vui64_t vconstp =
	CONST_VINT64_DW(0x0706050403020100UL, 0x0F0E0D0C0B0A0908UL);
#else
  const vui64_t vconstp =
      CONST_VINT64_DW(0x08090A0B0C0D0E0FUL, 0x0001020304050607UL);
#endif
  result = (vui64_t) vec_perm ((vui8_t) vra, (vui8_t) vra, (vui8_t) vconstp);
#endif

  return (result);
}

#ifndef vec_vsld
static inline vui64_t vec_vrld (vui64_t vra, vui64_t vrb);
static inline vui64_t vec_vsld (vui64_t vra, vui64_t vrb);
static inline vui64_t vec_vsrd (vui64_t vra, vui64_t vrb);
static inline vi64_t vec_vsrad (vi64_t vra, vui64_t vrb);
#endif

/*! \brief Vector Set Bool from Signed Doubleword.
 *
 *  For each doubleword, propagate the sign bit to all 64-bits of that
 *  doubleword. The result is vector bool long long reflecting the sign
 *  bit of each 64-bit doubleword.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-4   | 2/cycle  |
 *  |power9   | 2-5   | 2/cycle  |
 *
 *  @param vra Vector signed long long.
 *  @return vector bool long long reflecting the sign bits of each
 *  doubleword.
 */

static inline vb64_t
vec_setb_sd (vi64_t vra)
{
  vb64_t result;

#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
#if (__GNUC__ >= 12)
      result = (vb64_t) vec_expandm ((vui64_t) vra);
#else
  __asm__(
      "vexpanddm %0,%1;\n"
      : "=v" (result)
      : "v" (vra)
      : );
#endif
#elif defined (_ARCH_PWR8)
  // Compare signed doubleword less than zero
  const vi64_t zero = {0, 0};
  result = vec_cmpltsd (vra, zero);
#else // ARCH_PWR7 or older, without compare signed doubleword
  const vui8_t rshift =  vec_splat_u8( 7 );
  const vui8_t sperm = { 0,0,0,0, 0,0,0,0, 8,8,8,8, 8,8,8,8 };
  // Splat the high byte of each doubleword across.
  vui8_t splat = vec_perm ((vui8_t) vra, (vui8_t) vra, sperm);
  // Vector Shift Right Algebraic Bytes 7-bits.
  result = (vb64_t) vec_sra (splat, rshift);
#endif
  return result;
}

/** \brief Vector Rotate left Doubleword Immediate.
 *
 *  Rotate left each doubleword element [0-1], 0-63 bits,
 *  as specified by an immediate value.
 *  The rotate amount is a const unsigned int in the range 0-63.
 *  A rotate count of 0 returns the original value of vra.
 *  Shift counts greater then 63 bits handled modulo 64.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-4   | 2/cycle  |
 *  |power9   | 2-5   | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned long int.
 *  @param shb rotate amount in the range 0-63.
 *  @return 128-bit vector unsigned long int, shifted left shb bits.
 */
static inline vui64_t
vec_rldi (vui64_t vra, const unsigned int shb)
{
  vui64_t lshift;
  vui64_t result;

  if ((shb%64) != 0)
    {
      /* Load the rotate const in a vector.  The element rotates require
         a rotate amount for each element. For the immediate form the
         rotate constant is splatted to all elements of the
         rotate control.  */
      if (__builtin_constant_p (shb) && (shb < 16))
	lshift = (vui64_t) vec_splat_s32(shb);
      else
	lshift = vec_splats ((unsigned long long) shb);

      /* Vector Shift right bytes based on the lower 6-bits of
         corresponding element of lshift.  */
      result = vec_vrld (vra, lshift);
    }
  else
    { /* Rotation of 0 bits returns vra unchanged.  */
      result = vra;
    }

  return (vui64_t) result;
}

/** \brief Vector Shift left Doubleword Immediate.
 *
 *  Shift left each doubleword element [0-1], 0-63 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned long int in the range 0-63.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 63 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-4   | 2/cycle  |
 *  |power9   | 2-5   | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned long int.
 *  @param shb shift amount in the range 0-63.
 *  @return 128-bit vector unsigned long int, shifted left shb bits.
 */
static inline vui64_t
vec_sldi (vui64_t vra, const unsigned int shb)
{
  vui64_t result;

  if (shb < 64)
    {
      /* Load the shift const in a vector.  The element shifts require
         a shift amount for each element. For the immediate form the
         shift constant is splatted to all elements of the
         shift control.  */
#ifdef _ARCH_PWR8
      vui64_t lshift;

      if (__builtin_constant_p (shb) && (shb < 16))
	lshift = (vui64_t) vec_splat_s32(shb);
      else
	lshift = vec_splats ((unsigned long long) shb);
      /* Vector Shift left doubleword from the lower 6-bits of
         corresponding element of lshift.  */
      result = vec_vsld (vra, lshift);
#else
      /*
       * POWER7 and earlier do not have vsld. So use the vector shift
       * left bit/octet instructions. But these may shift bits from
       * element 1 in the low bits of element 0. So generate a mask of
       * '1's, shifted left by the same shb and rotated into the
       * element 0 position.
       */
      vui8_t lshift;

      if (__builtin_constant_p (shb) && (shb < 16))
	lshift = vec_splat_u8(shb);
      else
	lshift = vec_splats ((unsigned char) shb);

      {
	vui8_t sl_a;
	vui8_t sl_m = (vui8_t) vec_splat_s8(-1);

	sl_a = ((vui8_t) vra);
	if (shb > 7)
	  {
	    /* Vector Shift Left By Octet by bits 121-124 of lshift.  */
	    sl_m = vec_slo (sl_m, lshift);
	    sl_a = vec_slo ((vui8_t) vra, lshift);
	  }
	if ((shb & 7) != 0)
	  {
	    /* Vector Shift Left by bits 125-127 of lshift.  */
	    sl_m = vec_sll (sl_m, lshift);
	    sl_a = vec_sll (sl_a, lshift);
	  }
	/* Rotate mask and clear low order bits of Element 0. */
	sl_m = vec_sld (sl_m, sl_m, 8);
	result = (vui64_t) vec_and (sl_a, sl_m);
      }
#endif
    }
  else
    { /* shifts greater then 63 bits return zeros.  */
      result = vec_xor ((vui64_t) vra, (vui64_t) vra);
    }

  return (vui64_t) result;
}

/** \brief Vector Select Signed Doubleword.
 *
 *  Return the value, (vra & ~vrc) | (vrb & vrc).
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  2    | 2/cycle  |
 *  |power9   |  3    | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector signed long long int.
 *  @param vrb a 128-bit vector treated as a vector signed long long int.
 *  @param vrc a 128-bit vector treated as vector bool long long int.
 *  @return The selected bits from vra and vrb
 */
static inline vi64_t
vec_selsd (vi64_t vra, vi64_t vrb, vb64_t vrc)
{
  return (vi64_t) vec_sel ((vui32_t) vra, (vui32_t)vrb, (vui32_t)vrc);
}

/** \brief Vector Select Unsigned Doubleword.
 *
 *  Return the value, (vra & ~vrc) | (vrb & vrc).
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  2    | 2/cycle  |
 *  |power9   |  3    | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned long long int.
 *  @param vrb a 128-bit vector treated as a vector unsigned long long int.
 *  @param vrc a 128-bit vector treated as vector bool long long int.
 *  @return The selected bits from vra and vrb
 */
static inline vui64_t
vec_selud (vui64_t vra, vui64_t vrb, vb64_t vrc)
{
  return (vui64_t) vec_sel ((vui32_t) vra, (vui32_t)vrb, (vui32_t)vrc);
}

/** \brief Vector splat doubleword.
 *  Duplicate the selected doubleword element across the doubleword
 *  elements of the result. This is effectively the
 *  VSX Merge doubleword operation modified for endian.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  The 1-bit control operand (ctl) selects which (0:1) doubleword
 *  element, from the vector operand, is replicated to both doublewords
 *  of the result vector.  Control table:
 *  ctl |  vrt[0]  | vrt[1]
 *  :-: | :------: | :----------:
 *   0  |  vra[0]  | vra[0]
 *   1  |  vra[1]  | vra[1]
 *
 *  @param vra a 128-bit vector.
 *  @param ctl a const integer encoding the source doubleword.
 *  @return The original vector with the doubleword elements swapped.
 */
static inline vui64_t
vec_splatd (vui64_t vra, const int ctl)
{
  vui64_t result;
  switch (ctl & 1)
    {
    case 0:
      /*
       result[1] = vra[0];
       result[0] = vra[0];
       */
      result = vec_mrged (vra, vra);
      break;
    case 1:
      /*
       result[1] = vra[1];
       result[0] = vra[1];
       */
      result = vec_mrgod (vra, vra);
      break;
    }

  return (result);
}

/** \brief Vector Splat Immediate Signed Doubleword.
 *  Duplicate the signed integer constant across doubleword
 *  elements of the result. This is the doubleword equivalent
 *  Vector Splat Immediate Signed (Byte | Halfword |Word).
 *
 *  \note POWER9/10 will generate the 2 instruction sequence
 *  xxspltib/vextsb2d for values -128 to 128. Larger values
 *  will be loaded as a quadword constant from the read-only
 *  data (.rodata) section.
 *  POWER8 (and earlier) does not have vextsb2d instructions.
 *  For a smaller range (-16 -> 15) POWER8 can use the sequence
 *  vec_splat_s32/vec_unpackl but the latest compilers are too clever
 *  for this and generate a load from .rodata anyway.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4 - 9 | 2/cycle  |
 *  |power9   |   5   | 2/cycle  |
 *
 *  @param sim a small signed integer const.
 *  @return Vector with sim value splatted to doublewords.
 */
static inline vi64_t
vec_splat_s64 (const int sim)
{
  vi64_t result;
#ifdef _ARCH_PWR9
  result = vec_splats ((signed long long) sim);
#else
  if (__builtin_constant_p (sim) && ((sim >= -16) && (sim < 16)))
    {
      vi32_t vwi = vec_splat_s32 (sim);

      if (__builtin_constant_p (sim) && ((sim == 0) || (sim == -1)))
	{
	  // Special case for -1 and 0. Skip vec_unpackl().
	  result = (vi64_t) vwi;
	} else {
	  // For P8 can use either vupklsh or vupklsw but P7 only has
	  // vupklsh. Given the reduced range, Either works here.
	  // Unpack signed HW works here because immediate value fits
	  // into the low HW and sign extends to high HW of each word.
	  // Unpack will expand the low HW to low word and high HW
	  // (sign extend) into the high word of each DW.
	  // Unpack low/high (or endian) will not change the result.
#if defined (__GNUC__) && (__GNUC__ == 8)
	  // GCC 8 (AT12) handle this correctly.
	  result = (vi64_t) vec_vupklsh ((vi16_t) vwi);
#else
	  // But GCC 9+ optimized the above to be load from .rodata.
	  // With a little register pressure it adds some gratuitous store/reloads.
	  // So the following work-around is required.
	  __asm__(
	      "vupklsh %0,%1;"
	      : "=v" (result)
	      : "v" (vwi)
	      : );
#endif
	}
    }
  else
    result = vec_splats ((signed long long) sim);
#endif
  return (result);
}

/** \brief Vector Splat Immediate Unsigned Doubleword.
 *  Duplicate the unsigned integer constant across doubleword
 *  elements of the result. This is the doubleword equivalent
 *  Vector Splat Immediate Unsigned (Byte | Halfword |Word).
 *
 *  \note POWER9/10 will generate the 2 instruction sequence
 *  xxspltib/vextsb2d for values -128 to 128. Larger values
 *  will be loaded as a quadword constant from the read-only
 *  data (.rodata) section.
 *  POWER8 (and earlier) does not have vextsb2d instructions.
 *  For a smaller range (-16 -> 15) POWER8 can use the sequence
 *  vec_splat_s32/vec_unpackl but the latest compilers are too clever
 *  for this and generate a load from .rodata anyway.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4 - 9 | 2/cycle  |
 *  |power9   |   5   | 2/cycle  |
 *
 *  @param sim a small signed integer const.
 *  @return Vector with sim value splatted to doublewords.
 */
static inline vui64_t
vec_splat_u64 (const int sim)
{
  vui64_t result;
#ifdef _ARCH_PWR9
  result = vec_splats ((unsigned long long) sim);
#else
  if (__builtin_constant_p (sim) && ((sim >= 0) && (sim < 16)))
    {
      vui32_t vwi = vec_splat_u32 (sim);

      if (__builtin_constant_p (sim) && (sim == 0))
	{
	  // Special case for -1 and 0. Skip vec_unpackl().
	  result = (vui64_t) vwi;
	} else {
	  // For P8 can use either vupklsh or vupklsw but P7 only has
	  // vupklsh. Given the reduced range, Either works here.
	  // Unpack unsigned HW works here because immediate value fits
	  // into the low HW and zero extends to high HW of each word.
	  // Unpack will expand the low HW to low word and high HW
	  // (zero extended) into the high word of each DW.
	  // Unpack low/high (or endian) will not change the result.
#if defined (__GNUC__) && (__GNUC__ == 8)
	  // GCC 8 (AT12) handle this correctly.
	  result = (vui64_t) vec_vupklsh ((vi16_t) vwi);
#else
	  // But GCC 9+ optimized the above to be load from .rodata.
	  // With a little register pressure it adds some gratuitous store/reloads.
	  // So the following work-around is required.
	  __asm__(
	      "vupklsh %0,%1;"
	      : "=v" (result)
	      : "v" (vwi)
	      : );
#endif
	}
    }
  else
    result = vec_splats ((unsigned long long) sim);
#endif
  return (result);
}

/** \deprecated Vector splat doubleword.
 *  Duplicate the selected doubleword element across the doubleword
 *  elements of the result.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  The 1-bit control operand (ctl) selects which (0:1) doubleword
 *  element, from the vector operand, is replicated to both doublewords
 *  of the result vector.  Control table:
 *  ctl |  vrt[0:63]  | vrt[64:127]
 *  :-: | :---------: | :----------:
 *   0  |  vra[0:63]  | vra[0:63]
 *   1  | vra[64:127] | vra[64:127]
 *
 *  @param vra a 128-bit vector.
 *  @param ctl a const integer encoding the source doubleword.
 *  @return The original vector with the doubleword elements swapped.
 */
static inline vui64_t
vec_spltd (vui64_t vra, const int ctl)
{
  vui64_t result;
  /* Don't need to reverse the cases for LE because vec_permdi handles
     that.  */
  switch (ctl & 1)
    {
    case 0:
      /*
       result[1] = vra[0];
       result[0] = vra[0];
       */
      result = vec_permdi (vra, vra, 0);
      break;
    case 1:
      /*
       result[1] = vra[1];
       result[0] = vra[1];
       */
      result = vec_permdi (vra, vra, 3);
      break;
    }

  return (result);
}

/** \brief Vector Shift Right Doubleword Immediate.
 *
 *  Shift Right each doubleword element [0-1], 0-63 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-63.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 63 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-4   | 2/cycle  |
 *  |power9   | 2-5   | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned long int.
 *  @param shb shift amount in the range 0-63.
 *  @return 128-bit vector unsigned long int, shifted right shb bits.
 */
static inline vui64_t
vec_srdi (vui64_t vra, const unsigned int shb)
{
  vui64_t result;

  if (shb < 64)
    {
      /* Load the shift const in a vector.  The element shifts require
         a shift amount for each element. For the immediate form the
         shift constant is splatted to all elements of the
         shift control.  */
#ifdef _ARCH_PWR8
      vui64_t rshift;

#if defined (__GNUC__) && (__GNUC__ < 8)
      if (__builtin_constant_p (shb) && (shb < 16))
	rshift = (vui64_t) vec_splat_s32(shb);
      else
	rshift = vec_splats ((unsigned long long) shb);
#else
      rshift = CONST_VINT128_DW (shb, shb);
#endif
      /* Vector Shift right bytes based on the lower 6-bits of
         corresponding element of rshift.  */
      result = vec_vsrd (vra, rshift);
#else
      /*
       * POWER7 and earlier do not have vsrd. So use the vector shift
       * right bit/octet instructions. But these may shift bits from
       * element 0 in the high bits of element 1. So generate a mask of
       * '1's, shifted right by the same shb and rotated into the
       * element 1 position.
       */
      vui8_t rshift;

      if (__builtin_constant_p (shb) && (shb < 16))
	rshift = vec_splat_u8(shb);
      else
	rshift = vec_splats ((unsigned char) shb);

      {
	vui8_t sr_a;
	vui8_t sr_m = (vui8_t) vec_splat_s8(-1);

	sr_a = ((vui8_t) vra);
	if (shb > 7)
	  {
	    /* Vector Shift Right By Octet by bits 121-124 of rshift.  */
	    sr_m = vec_sro (sr_m, rshift);
	    sr_a = vec_sro ((vui8_t) vra, rshift);
	  }
	if ((shb & 7) != 0)
	  {
	    /* Vector Shift Right by bits 125-127 of rshift.  */
	    sr_m = vec_srl (sr_m, rshift);
	    sr_a = vec_srl (sr_a, rshift);
	  }
	/* Rotate mask and clear high order bits of Element 1. */
	sr_m = vec_sld (sr_m, sr_m, 8);
	result = (vui64_t) vec_and (sr_a, sr_m);
      }
#endif
    }
  else
    { /* shifts greater then 63 bits return zeros.  */
      result = vec_xor ((vui64_t) vra, (vui64_t) vra);
    }
  return (vui64_t) result;
}

/** \brief Vector Shift Right Algebraic Doubleword Immediate.
 *
 *  Shift Right Algebraic each doubleword element [0-1],
 *  0-63 bits, as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-63.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 63 bits return the sign bit
 *  propagated to each bit of each element.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-4   | 2/cycle  |
 *  |power9   | 2-5   | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector signed long int.
 *  @param shb shift amount in the range 0-63.
 *  @return 128-bit vector signed long int, shifted right shb bits.
 */
static inline vi64_t
vec_sradi (vi64_t vra, const unsigned int shb)
{
  vui64_t rshift;
  vi64_t result;

  if (shb < 64)
    {
      /* Load the shift const in a vector.  The element shifts require
         a shift amount for each element. For the immediate form the
         shift constant is splatted to all elements of the
         shift control.  */
#if defined (__GNUC__) && (__GNUC__ < 8)
      if (__builtin_constant_p (shb) && (shb < 16))
	rshift = (vui64_t) vec_splat_s32(shb);
      else
	rshift = vec_splats ((unsigned long long) shb);
#else
      rshift = CONST_VINT128_DW (shb, shb);
#endif
      /* Vector Shift Right Algebraic Doublewords based on the lower 6-bits
         of corresponding element of rshift.  */
      result = vec_vsrad (vra, rshift);
    }
  else
    { /* shifts greater then 63 bits returns the sign bit propagated to
         all bits.   This is equivalent to shift Right Algebraic of
         63 bits.  */
      rshift = (vui64_t) vec_splats(63);
      result = vec_vsrad (vra, rshift);
    }

  return (vi64_t) result;
}

/** \brief Vector Subtract Unsigned Doubleword Modulo.
 *
 *  For each unsigned long (64-bit) integer element c[i] = a[i] +
 *  NOT(b[i]) + 1.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2     | 2/cycle  |
 *  |power9   | 2     | 2/cycle  |
 *
 *  For POWER8 (PowerISA 2.07B) or later use the Vector Subtract
 *  Unsigned Doubleword Modulo (<B>vsubudm</B>) instruction. Otherwise
 *  use vector add word modulo forms and propagate the carry bits.
 *
 *  @param a 128-bit vector treated as 2 X unsigned long int.
 *  @param b 128-bit vector treated as 2 X unsigned long int.
 *  @return  vector unsigned long int sum of a[0] + NOT(b[0]) + 1
 *  and a[1] + NOT(b[1]) + 1.
 */
static inline vui64_t
vec_subudm(vui64_t a, vui64_t b)
{
  vui32_t r;

#ifdef _ARCH_PWR8
#if defined (vec_vsubudm)
  r = (vui32_t) vec_vsubudm (a, b);
#elif defined (__clang__)
  r = (vui32_t) vec_sub (a, b);
#else
  __asm__(
      "vsubudm %0,%1,%2;"
      : "=v" (r)
      : "v" (a),
      "v" (b)
      : );
#endif
#else
  vui32_t c;
  vui32_t z= { 0,0,0,0};
  vui32_t cm= { 0,1,0,1};

  c = vec_vsubcuw ((vui32_t)a, (vui32_t)b);
  r = vec_vsubuwm ((vui32_t)a, (vui32_t)b);
  c = vec_andc (cm, c);
  c = vec_sld (c, z, 4);
  r = vec_vsubuwm (r, c);
#endif
  return ((vui64_t) r);
}

/** \brief Vector doubleword swap.
 *  Exchange the high and low doubleword elements of a vector.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param vra a 128-bit vector.
 *  @return The original vector with the doubleword elements swapped.
 */
static inline vui64_t
vec_swapd (vui64_t vra)
{
  vui64_t result;
  /*
   result[1] = vra[0];
   result[0] = vra[1];
   */
  result = vec_permdi (vra, vra, 2);

  return (result);
}

/** \brief Vector Divide Extended Unsigned Doubleword.
 *
 *  Divide the [zero] extended doubleword elements x by the
 *  corresponding doubleword elements of z. The extended dividend is
 *  the 64-bit element from x extended to the right with 64-bits of 0b.
 *  This is effectively a vectorized 128x64 bit unsigned divide
 *  returning 64-bit quotients.
 *  The quotients of the extended divide is returned as a vector
 *  unsigned long long int.
 *
 *  \note The element results may be undefined if;
 *  the quotient cannot be represented in 64-bits,
 *  or the divisor is 0.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   54  |1/40 cycle|
 *  |power9   |   48  |1/9 cycle |
 *  |power10  | 26-75 |2/22 cycle|
 *
 *
 *  @param x 128-bit vector unsigned long long.
 *  @param z 128-bit vector unsigned long long.
 *  @return The quotients in a vector unsigned long long int.
 */
static inline vui64_t
vec_vdiveud_inline (vui64_t x, vui64_t z)
{
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  vui64_t res;
#if (__GNUC__ >= 12)
  res = vec_dive (x, z);
#else
  __asm__(
      "vdiveud %0,%1,%2;\n"
      : "=v" (res)
      : "v" (x), "v" (z)
      : );
#endif
  return res;
#else // defined (_ARCH_PWR7)
  // POWER7/8/9 Do not have vector integer divide, but does have
  // Fixed-point (FXU) Divide and Divide-Extended Doubleword.
  // POWER8/9 also have
  // Move To/From Vector-Scalar Register Instructions
  // So we can use the scalar hardware divide instructions
  __VEC_U_128 qu, yu, zu;
#if (__GNUC__ <= 10) &&  defined (_ARCH_PWR8)
  yu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) x, 1);
  yu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) x, 0);
  zu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) z, 1);
  zu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) z, 0);
#else
  // Looks like AT16 handles this but what about 15/14 ...
  // AT10 does not.
  yu.vx2 = x;
  zu.vx2 = z;
#endif

  qu.ulong.lower = __builtin_divdeu (yu.ulong.lower, zu.ulong.lower);
  qu.ulong.upper = __builtin_divdeu (yu.ulong.upper, zu.ulong.upper);

  return qu.vx2;
#endif
}

/** \brief Vector Divide Unsigned Doubleword.
 *
 *  Divide the doubleword elements y by the
 *  corresponding doubleword elements of z.
 *  This is effectively a vectorized 64x64 bit unsigned divide
 *  returning 64-bit quotients.
 *  The quotients of the divide is returned as a vector
 *  unsigned long long int.
 *
 *  \note The element results will be undefined if
 *  the divisor is 0.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   54  |1/40 cycle|
 *  |power9   |   48  |1/9 cycle |
 *  |power10  | 26-43 |2/22 cycle|
 *
 *  @param y 128-bit vector unsigned long long.
 *  @param z 128-bit vector unsigned long long.
 *  @return The quotients in a vector unsigned long long int.
 */
static inline vui64_t
vec_vdivud_inline (vui64_t y, vui64_t z)
{
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  vui64_t res;
#if (__GNUC__ >= 12)
  res = vec_div (y, z);
#else
  __asm__(
      "vdivud %0,%1,%2;\n"
      : "=v" (res)
      : "v" (y), "v" (z)
      : );
#endif
  return res;
#else // defined (_ARCH_PWR7)
  // POWER7/8/9 Do not have vector integer divide, but does have
  // Fixed-point (FXU) Divide Doubleword.
  // POWER8/9 also have
  // Move To/From Vector-Scalar Register Instructions
  // So we can use the scalar hardware divide instructions
  __VEC_U_128 qu, yu, zu;
#if (__GNUC__ <= 10) &&  defined (_ARCH_PWR8)
  yu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) y, 1);
  yu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) y, 0);
  zu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) z, 1);
  zu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) z, 0);
#else
  yu.vx2 = y;
  zu.vx2 = z;
#endif

  qu.ulong.lower = yu.ulong.lower / zu.ulong.lower;
  qu.ulong.upper = yu.ulong.upper / zu.ulong.upper;

  return qu.vx2;
#endif
}

/** \brief Vector Gather-Load Integer Doublewords from Vector Doubleword Offsets.
 *
 *  For each doubleword element [i] of vra, load the doubleword
 *  element at *(char*)array+vra[i]. Merge those doubleword elements
 *  and return the resulting vector. For best performance <B>&array</B>
 *  and doubleword offsets <B>vra</B> should be doubleword aligned
 *  (integer multiple of 8).
 *
 *  \note As effective address calculation is modulo 64-bits, signed or
 *  unsigned doubleword offsets are equivalent.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   12  | 1/cycle  |
 *  |power9   |   11  | 1/cycle  |
 *
 *  @param array Pointer to array of integer doublewords.
 *  @param vra Vector of doubleword (64-bit) byte offsets from &array.
 *  @return vector doubleword containing elements loaded from
 *  *(char*)array+vra[0] and *(char*)array+vra[1].
 */
static inline
vui64_t
vec_vgluddo (unsigned long long *array, vi64_t vra)
{
  vui64_t rese0, rese1;

#ifdef _ARCH_PWR8
  rese0 = vec_vlsidx (vra[VEC_DW_H], array);
  rese1 = vec_vlsidx (vra[VEC_DW_L], array);
#else
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);

  rese0 = vec_vlsidx (scalar_extract_uint64_from_high_uint128(gprp), array);
  rese1 = vec_vlsidx (scalar_extract_uint64_from_low_uint128(gprp), array);
#endif
  return  vec_permdi (rese0, rese1, 0);
}

/** \brief Vector Gather-Load Integer Doublewords from Vector Doubleword Scaled Indexes.
 *
 *  For each doubleword element [i] of vra, load the doubleword
 *  element array[vra[i] * (1 << scale)]. Merge those doubleword
 *  elements and return the resulting vector. Array element indices are
 *  converted to byte offsets from (array) by multiplying each index by
 *  (sizeof (array element) * scale), which is effected by shifting
 *  left (3+scale) bits.
 *
 *
 *  \note As effective address calculation is modulo 64-bits, signed or
 *  unsigned doubleword indexes are equivalent.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 14-23 | 1/cycle  |
 *  |power9   | 13-22 | 1/cycle  |
 *
 *  @param array Pointer to array of integer doublewords.
 *  @param vra Vector of signed doubleword indexes.
 *  @param scale 8-bit integer. Indexes are multiplying by
 *  2<sup>scale</sup>.
 *  @return vector containing doublewords from array[(vra[0,1]<<scale)].
 */
static inline vui64_t
vec_vgluddsx (unsigned long long *array, vi64_t vra,
	     const unsigned char scale)
{
  vi64_t offset;

  offset = (vi64_t) vec_sldi ((vui64_t) vra, (3 + scale));
  return vec_vgluddo (array, offset);
}

/** \brief Vector Gather-Load Integer Doublewords from Vector Doubleword Indexes.
 *
 *  For each doubleword element [i] of vra, load the doubleword
 *  element from array[vra[i]]. Merge those doubleword elements and
 *  return the resulting vector. Array element indices are
 *  converted to byte offsets from (array) by multiplying each index by
 *  (sizeof (array element) * scale), which is effected by shifting
 *  left 3 bits.
 *
 *  \note As effective address calculation is modulo 64-bits, signed or
 *  unsigned doubleword indexes are equivalent.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 14-23 | 1/cycle  |
 *  |power9   | 13-22 | 1/cycle  |
 *
 *  @param array Pointer to array of integer doublewords.
 *  @param vra Vector of signed doubleword indexes.
 *  @return vector containing doublewords array[vra[0,1]].
 */
static inline
vui64_t
vec_vgluddx (unsigned long long *array, vi64_t vra)
{
  vi64_t offset;

  offset = (vi64_t) vec_sldi ((vui64_t) vra, 3);
  return vec_vgluddo (array, offset);
}

/** \brief Vector Gather-Load Integer Doublewords from Scalar Offsets.
 *
 *  For each scalar offset[0|1], load the doubleword element at
 *  *(char*)array+offset[0|1]. Merge those doubleword elements
 *  and return the resulting vector. For best performance <B>&array</B>
 *  and doubleword offsets should be doubleword aligned
 *  (integer multiple of 8).
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   7   | 1/cycle  |
 *  |power9   |   8   | 1/cycle  |
 *
 *  @param array Pointer to array of integer doublewords.
 *  @param offset0 Scalar (64-bit) byte offsets from &array.
 *  @param offset1 Scalar (64-bit) byte offsets from &array.
 *  @return vector doubleword containing elements loaded from
 *  *(char*)array+offset0 and *(char*)array+offset1.
 */
static inline vui64_t
vec_vgludso (unsigned long long *array, const long long offset0,
	     const long long offset1)
{
  vui64_t re0, re1, result;

  re0 = vec_vlsidx (offset0, array);
  re1 = vec_vlsidx (offset1, array);
  /* Need to handle endian as the vec_vlsidx result is always left
   * justified in VSR, while element [0] may be left or right. */
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  result = vec_permdi (re1, re0, 0);
#else
  result = vec_permdi (re0, re1, 0);
#endif
  return result;
}

/** \brief Vector Load Scalar Integer Doubleword Indexed.
 *
 *  Load the left most doubleword of vector <B>xt</B> as a scalar
 *  doubleword from the effective address formed by <B>rb+ra</B>. The
 *  operand <B>rb</B> is a pointer to an array of doublewords.
 *  The operand <B>ra</B> is a doubleword integer byte offset
 *  from <B>rb</B>. The result <B>xt</B> is returned as a vui64_t
 *  vector. For best performance <B>rb</B> and <B>ra</B>
 *  should be doubleword aligned (integer multiple of 8).
 *
 *  \note the right most doubleword of vector <B>xt</B> is left
 *  <I>undefined</I> by this operation.
 *
 *  This operation is an alternate form of Vector Load Element
 *  (vec_lde), with the added simplification that data is always left
 *  justified in the vector. This simplifies merging elements for
 *  gather operations.
 *
 *  \note This instruction was introduced in PowerISA 2.06 (POWER7).
 *  For POWER8/9 there are additional optimizations by effectively
 *  converting small constant index values into displacements. For
 *  POWER8 a specific pattern of addi/lsxdx instruction is <I>fused</I>
 *  into a single load displacement internal operation. For POWER9 we can
 *  use the lxsd (DS-form) instruction directly.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   5   | 2/cycle  |
 *  |power9   |   5   | 2/cycle  |
 *
 *  @param ra const signed doubleword index (offset/displacement).
 *  @param rb const doubleword pointer to an array of doubles.
 *  @return The data stored at (ra + rb) is loaded into vector
 *  doubleword element 0. Element 1 is undefined.
 */
static inline vui64_t
vec_vlsidx (const signed long long ra, const unsigned long long *rb)
{
  vui64_t xt;

#if (defined(__clang__) && __clang_major__ < 8)
  __VEC_U_128 t;
  unsigned long long *p = (unsigned long long *)((char *)rb + ra);
  t.ulong.upper = *p;
  xt = t.vx1;
#else
  if (__builtin_constant_p (ra) && (ra <= 32760) && (ra >= -32768)
      && ((ra & 3) == 0))
    {
#if defined (_ARCH_PWR9)
      __asm__(
	  "lxsd%X1 %0,%1;"
	  : "=v" (xt)
	  : "m" (*(unsigned long long *)((char *)rb + ra))
	  : );
#else
      if (ra == 0)
	{
	  __asm__(
	      "lxsdx %x0,%y1;"
	      : "=wa" (xt)
	      : "Z" (*rb)
	      : );
	} else {
	  unsigned long long rt;
#if defined (_ARCH_PWR8)
	  // For P8 better if li and lxsdx shared a single asm block
	  // (enforcing consecutive instructions).
	  // This enables instruction fusion for P8.
	  __asm__(
	      "li %0,%2;"
	      "lxsdx %x1,%3,%0;"
	      : "=&r" (rt), "=wa" (xt)
	      : "I" (ra), "b" (rb), "Z" (*(unsigned long long *)((char *)rb+ra))
	      : );
#else // _ARCH_PWR7
	  // This generates operationally the same code, but the
	  // compiler may rearrange/schedule the code.
	  __asm__(
	      "li %0,%1;"
	      : "=r" (rt)
	      : "I" (ra)
	      : );
	  __asm__(
	      "lxsdx %x0,%y1;"
	      : "=wa" (xt)
	      : "Z" (*(unsigned long long *)((char *)rb+rt))
	      : );
#endif
	}
#endif
    } else {
      __asm__(
	  "lxsdx %x0,%y1;"
	  : "=wa" (xt)
	  : "Z" (*(unsigned long long *)((char *)rb+ra))
	  : );
    }
#endif
  return xt;
}

/** \brief \copybrief vec_vmadd2eud()
 *
 * \note this implementation exists in
 * vec_int128_ppc.h as it requires vec_msumudm() and vec_adduqm().
 * See \ref vec_vmadd2eud() for full description.
 */
static inline vui128_t
vec_vmadd2eud (vui64_t a, vui64_t b, vui64_t c, vui64_t d);

/** \brief \copybrief vec_vmaddeud()
 *
 * \note this implementation exists in
 * vec_int128_ppc.h as it requires vec_msumudm() and vec_adduqm().
 * See \ref vec_vmaddeud() for full description.
 */
static inline vui128_t
vec_vmaddeud (vui64_t a, vui64_t b, vui64_t c);

/** \brief \copybrief vec_vmadd2oud()
 *
 * \note this implementation exists in
 * vec_int128_ppc.h as it requires vec_msumudm() and vec_adduqm().
 * See \ref vec_vmadd2oud() for full description.
 */
static inline vui128_t
vec_vmadd2oud (vui64_t a, vui64_t b, vui64_t c, vui64_t d);

/** \brief \copybrief vec_vmaddoud()
 *
 * \note this implementation exists in
 * vec_int128_ppc.h as it requires vec_msumudm() and vec_adduqm().
 * See \ref vec_vmaddoud() for full description.
 */
static inline vui128_t
vec_vmaddoud (vui64_t a, vui64_t b, vui64_t c);

/** \brief Vector Modulo Unsigned Doubleword.
 *
 *  Divide the doubleword elements y by the
 *  corresponding doubleword elements of z
 *  and return the remainder.
 *  This is effectively a vectorized 64x64 bit unsigned modulo
 *  returning 64-bit remainders.
 *  The remainders of the divide is returned as a vector
 *  unsigned long long int.
 *
 *  \note The element results will be undefined if
 *  the divisor is 0.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   62  |1/40 cycle|
 *  |power9   |   48  |1/9 cycle |
 *  |power10  | 26-47 |2/22 cycle|
 *
 *  @param y 128-bit vector unsigned long long.
 *  @param z 128-bit vector unsigned long long.
 *  @return The remainders in a vector unsigned long long int.
 */
static inline vui64_t
vec_vmodud_inline (vui64_t y, vui64_t z)
{
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  vui64_t res;
#if (__GNUC__ >= 12)
  res = vec_mod (y, z);
#else
  __asm__(
      "vmodud %0,%1,%2;\n"
      : "=v" (res)
      : "v" (y), "v" (z)
      : );
#endif
  return res;
#else // defined (_ARCH_PWR7, _ARCH_PWR8, _ARCH_PWR9)
  // POWER7/8/9 Do not have vector integer modulo, but does have
  // Fixed-point (FXU) Divide/Muliply Doubleword and
  // POWER9 also has FXU Modulo Doubleword.
  // POWER8/9 also have
  // Move To/From Vector-Scalar Register Instructions
  // So we can use the scalar hardware divide instructions
  __VEC_U_128 qu, yu, zu;
#if (__GNUC__ <= 10) &&  defined (_ARCH_PWR8)
  yu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) y, 1);
  yu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) y, 0);
  zu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) z, 1);
  zu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) z, 0);
#else
  yu.vx2 = y;
  zu.vx2 = z;
#endif

  qu.ulong.lower = yu.ulong.lower % zu.ulong.lower;
  qu.ulong.upper = yu.ulong.upper % zu.ulong.upper;

  return qu.vx2;
#endif
}

/** \brief \copybrief vec_vmuleud()
 *
 * \note this implementation exists in
 * \ref vec_vmuleud()
 * in vec_int128_ppc.h as it requires vec_msumudm() and vec_adduqm().
 */
static inline vui128_t
vec_vmuleud (vui64_t a, vui64_t b);

/** \brief \copybrief vec_vmuloud()
 *
 * \note this implementation exists in
 * \ref vec_vmuloud()
 * in vec_int128_ppc.h as it requires vec_msumudm() and vec_adduqm().
 */
static inline vui128_t
vec_vmuloud (vui64_t a, vui64_t b);

/** \brief \copybrief vec_vmsumeud()
 *
 * \note this implementation exists in
 * \ref vec_vmsumeud()
 * in vec_int128_ppc.h as it requires vec_msumudm() and vec_adduqm().
 */
static inline vui128_t
vec_vmsumeud (vui64_t a, vui64_t b, vui128_t c);

/** \brief \copybrief vec_vmsumoud()
 *
 * \note this implementation exists in
 * \ref vec_vmsumoud()
 * in vec_int128_ppc.h as it requires vec_msumudm() and vec_adduqm().
 */
static inline vui128_t
vec_vmsumoud (vui64_t a, vui64_t b, vui128_t c);

/** \brief Vector Pack Unsigned Doubleword Unsigned Modulo.
 *
 *  The doubleword source is the concatination of vra and vrb.
 *  For each integer word from 0 to 3, of the result vector, do the
 *  following: place the contents of bits 32:63 of the corresponding
 *  doubleword source element [i] into word element [i] of the result.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   2   | 2/cycle  |
 *
 *  \note Use vec_vpkudum naming but only
 *  if the compiler does not define it in <altivec.h>.
 *
 *  @param vra a 128-bit vector treated as 2 x unsigned long integers.
 *  @param vrb a 128-bit vector treated as 2 x unsigned long integers.
 *  @return 128-bit vector treated as 4 x unsigned integers.
 */
#ifndef vec_vpkudum
// May be defined as inline function for clang
// But only for _ARCH_PWR8 or higher.
#if !defined(__clang__) || !defined(_ARCH_PWR8)
static inline vui32_t
vec_vpkudum (vui64_t vra, vui64_t vrb)
{
  vui32_t r;
#ifdef _ARCH_PWR8
  __asm__(
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
      "vpkudum %0,%2,%1;\n"
#else
      "vpkudum %0,%1,%2;\n"
#endif
      : "=v" (r)
      : "v" (vra),
	"v" (vrb)
      : );
#else
  const vui32_t vconstp =
      CONST_VINT128_W(0x04050607, 0x0c0d0e0f, 0x14151617, 0x1c1d1e1f);

  r = vec_perm ((vui32_t) vra, (vui32_t) vrb, (vui8_t) vconstp);
#endif
  return (r);
}
#endif
#endif

/** \brief Vector Rotate Left Doubleword.
 *
 *  Vector Rotate Left Doubleword 0-63 bits.
 *  The shift amount is from bits 58-63 and 122-127 of vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   2   | 2/cycle  |
 *
 *  \note Use vec_vrld naming but only
 *  if the compiler does not define it in <altivec.h>.
 *
 *  @param vra a 128-bit vector treated as 2 x unsigned long integers.
 *  @param vrb shift amount in bits 58:63 and 122:127.
 *  @return Left shifted vector unsigned long.
 */
#ifndef vec_vrld
static inline vui64_t
vec_vrld (vui64_t vra, vui64_t vrb)
{
  vui64_t r;
#ifdef _ARCH_PWR8
#ifdef __clang__
  r = vec_rl (vra, vrb);
#else
  __asm__(
      "vrld %0,%1,%2;"
      : "=v" (r)
      : "v" (vra),
	"v" (vrb)
      : );
#endif
#else
  vui64_t hd, ld;
  vui32_t t1, t2;
  vui8_t shh, shl;

  shh = vec_splat ((vui8_t) vrb, VEC_BYTE_L_DWH);
  shl = vec_splat ((vui8_t) vrb, VEC_BYTE_L_DWL);
  hd = vec_xxspltd (vra, VEC_DW_H);
  ld = vec_xxspltd (vra, VEC_DW_L);
  t1 = vec_vslo ((vui32_t)hd, shh);
  t2 = vec_vslo ((vui32_t)ld, shl);
  t1 = vec_vsl (t1, shh);
  t2 = vec_vsl (t2, shl);
  r = vec_mrghd ((vui64_t)t1, (vui64_t)t2);
#endif
  return (r);
}
#endif

/** \brief Vector Shift Left Doubleword.
 *
 *  Vector Shift Left Doubleword 0-63 bits.
 *  The shift amount is from bits 58-63 and 122-127 of vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   2   | 2/cycle  |
 *
 *  \note Can not use vec_sld naming here as that would conflict
 *  with the generic Shift Left Double Vector. Use vec_vsld but only
 *  if the compiler does not define it in <altivec.h>.
 *
 *  @param vra a 128-bit vector treated as 2 x unsigned long integers.
 *  @param vrb shift amount in bits 58:63 and 122:127.
 *  @return Left shifted vector unsigned long.
 */
#ifndef vec_vsld
static inline vui64_t
vec_vsld (vui64_t vra, vui64_t vrb)
{
  vui64_t result;

#ifdef _ARCH_PWR8
#ifdef __clang__
  result = vec_sl (vra, vrb);
#else
  __asm__(
      "vsld %0,%1,%2;"
      : "=v" (result)
      : "v" (vra),
      "v" (vrb)
      : );
#endif
#else
  vui8_t  vsh_h, vsh_l;
  vui8_t  vr_h, vr_l;
  vui64_t sel_mask = CONST_VINT128_DW(0, -1LL);
  vui64_t shft_mask = CONST_VINT128_DW(63, 63);

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
#endif
  return ((vui64_t) result);
}
#endif

/** \brief Vector Shift Right Algebraic Doubleword.
 *
 *  Vector Shift Right Algebraic Doubleword 0-63 bits.
 *  The shift amount is from bits 58-63 and 122-127 of vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   2   | 2/cycle  |
 *
 *  \note Use the vec_vsrad for consistency with vec_vsld above.
 *  Define vec_vsrad only if the compiler does not define it in
 *  <altivec.h>.
 *
 *  @param vra a 128-bit vector treated as 2 x unsigned long integers.
 *  @param vrb shift amount in bits 58:63 and 122:127.
 *  @return Right shifted vector unsigned long.
 */
#ifndef vec_vsrad
static inline vi64_t
vec_vsrad (vi64_t vra, vui64_t vrb)
{
  vi64_t result;

#ifdef _ARCH_PWR8
#ifdef __clang__bad
// clang8/9 has code gen bug here, disabled for now
  result = vec_sra (vra, vrb);
#else
  __asm__(
      "vsrad %0,%1,%2;"
      : "=v" (result)
      : "v" (vra),
      "v" (vrb)
      : );
#endif
#else
  vui8_t  vsh_h, vsh_l;
  vui8_t  vr_h, vr_l;
  vi32_t exsa;
  vui32_t shw31 = CONST_VINT128_W (-1, -1, -1, -1);
  vui64_t exsah, exsal;
  vui64_t shft_mask = CONST_VINT128_DW(63, 63);

  /* Need to extend each signed long int to __int128. So the unsigned
   * (128-bit) shift right behaves as a arithmetic (64-bit) shift.  */
  exsa = vec_vsraw ((vi32_t) vra, shw31);
  exsah = (vui64_t) vec_vmrghw (exsa, exsa);
  exsal = (vui64_t) vec_vmrglw (exsa, exsa);
  /* constrain the dword shift amounts to 0-63.  */
  vsh_l = vec_and ((vui8_t) vrb, (vui8_t) shft_mask);
  /* The vsr instruction only works correctly if the bit shift
   * value is splatted to each byte of the vector.  */
  vsh_h = vec_splat (vsh_l, VEC_BYTE_L_DWH);
  vsh_l = vec_splat (vsh_l, VEC_BYTE_L_DWL);
  /* Merge the extended sign with high dword.  */
  exsah = vec_mrghd (exsah, (vui64_t) vra);
  /* Shift the high dword by vsh_h.  */
  vr_h = vec_vsro ((vui8_t) exsah,  vsh_h);
  vr_h = vec_vsr  (vr_h, vsh_h);
  /* Merge the extended sign with high dword.  */
  exsal = vec_pasted (exsal, (vui64_t) vra);
  /* Shift the low dword by vsh_l.  */
  vr_l = vec_vsro ((vui8_t) exsal, vsh_l);
  vr_l = vec_vsr  (vr_l, vsh_l);
  /* Merge the dwords after shift.  */
  result = (vi64_t) vec_mrgld ((vui64_t) vr_h, (vui64_t) vr_l);
#endif
  return ((vi64_t) result);
}
#endif

/** \brief Vector Shift Right Doubleword.
 *
 *  Vector Shift Right Doubleword 0-63 bits.
 *  The shift amount is from bits 58-63 and 122-127 of vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   2   | 2/cycle  |
 *
 *  \note Use the vec_vsrd for consistency with vec_vsld above.
 *  Define vec_vsrd only if the compiler does not define it in
 *  <altivec.h>.
 *
 *  @param vra a 128-bit vector treated as 2 x unsigned long integers.
 *  @param vrb shift amount in bits 58:63 and 122:127.
 *  @return Right shifted vector unsigned long.
 */
#ifndef vec_vsrd
static inline vui64_t
vec_vsrd (vui64_t vra, vui64_t vrb)
{
  vui64_t result;

#ifdef _ARCH_PWR8
#ifdef __clang__
  result = vec_sr (vra, vrb);
#else
  __asm__(
      "vsrd %0,%1,%2;"
      : "=v" (result)
      : "v" (vra),
      "v" (vrb)
      : );
#endif
#else
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
#endif
  return ((vui64_t) result);
}
#endif

/** \brief Vector Scatter-Store Integer Doublewords to Vector Doublewords Offsets.
 *
 *  For each doubleword element [i] of vra, Store the doubleword
 *  element xs[i] at the address *(char*)array+vra[i]
 *  For best performance <B>&array</B> and doubleword offsets <B>vra</B>
 *  should be doubleword aligned (integer multiple of 8).
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   12  | 1/cycle  |
 *  |power9   |   8   | 1/cycle  |
 *
 *  @param xs Vector of integer doubleword elements to scatter store.
 *  @param array Pointer to array of integer doublewords.
 *  @param vra Vector of doubleword (64-bit) byte offsets from &array.
 */
static inline void
vec_vsstuddo (vui64_t xs, unsigned long long *array, vi64_t vra)
{
  vui64_t xs1;

  xs1 = vec_xxspltd (xs, 1);
#ifdef _ARCH_PWR8
  vec_vstsidx (xs, vra[VEC_DW_H], array);
  vec_vstsidx (xs1, vra[VEC_DW_L], array);
#else
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);
  vec_vstsidx (xs, scalar_extract_uint64_from_high_uint128(gprp), array);
  vec_vstsidx (xs1, scalar_extract_uint64_from_low_uint128(gprp), array);
#endif
}

/** \brief Vector Scatter-Store Integer Doublewords to Vector Doubleword Scaled Indexes.
 *
 *  For each doubleword element [i] of vra, store the doubleword
 *  element xs[i] at array[(vra[i] << scale)]. Array element
 *  indices are converted to byte offsets from (array) by multiplying
 *  each index by (sizeof (array element) * scale), which is effected
 *  by shifting left (3+scale) bits.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 14-23 | 1/cycle  |
 *  |power9   | 10-19 | 1/cycle  |
 *
 *  @param xs Vector of integer doubleword elements to scatter store.
 *  @param array Pointer to array of integer doublewords.
 *  @param vra Vector of signed doubleword indexes.
 *  @param scale 8-bit integer. Indexes are multiplying by
 *  2<sup>scale</sup>.
 */
static inline void
vec_vsstuddsx (vui64_t xs, unsigned long long *array,
	    vi64_t vra, const unsigned char scale)
{
  vi64_t offset;

  offset = (vi64_t) vec_sldi ((vui64_t) vra, (3 + scale));
  vec_vsstuddo (xs, array, offset);
}

/** \brief Vector Scatter-Store Integer Doublewords to Vector Doubleword Indexes.
 *
 *  For each doubleword element [i] of vra, store the doubleword
 *  element xs[i] at array[vra[i]]. Indexes are converted to offsets
 *  from *array by shifting each doubleword of vra
 *  left (3+scale) bits.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 14-23 | 1/cycle  |
 *  |power9   | 10-19 | 1/cycle  |
 *
 *  @param xs Vector of integer doubleword elements to scatter store.
 *  @param array Pointer to array of integer doublewords.
 *  @param vra Vector of signed doubleword indexes.
 */
static inline void
vec_vsstuddx (vui64_t xs, unsigned long long *array,
	    vi64_t vra)
{
  vi64_t offset;

  offset = (vi64_t) vec_sldi ((vui64_t) vra, 3);
  vec_vsstuddo (xs, array, offset);
}

/** \brief Vector Scatter-Store Integer Doublewords to Scalar Offsets.
 *
 *  For each doubleword element [i] of vra, Store the doubleword
 *  element xs[i] at *(char*)array+offset[0|1].
 *  For best performance, <B>&array</B> and doubleword offsets
 *  should be doubleword aligned (integer multiple of 8).
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   12  | 1/cycle  |
 *  |power9   |   8   | 1/cycle  |
 *
 *  @param xs Vector of integer doubleword elements to scatter store.
 *  @param array Pointer to array of integer doublewords.
 *  @param offset0 Scalar (64-bit) byte offset from &array.
 *  @param offset1 Scalar (64-bit) byte offset from &array.
 */
static inline void
vec_vsstudso (vui64_t xs, unsigned long long *array,
	      const long long offset0, const long long offset1)
{
  vui64_t xs1;

  xs1 = vec_xxspltd (xs, 1);
  /* Need to handle endian as vec_vstsfdux always left side of
   * the VR, while the element [0] may in the left or right. */
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  vec_vstsidx (xs, offset1, array);
  vec_vstsidx (xs1, offset0, array);
#else
  vec_vstsidx (xs, offset0, array);
  vec_vstsidx (xs1, offset1, array);
#endif
}

/** \brief Vector Store Scalar Integer Doubleword Indexed.
 *
 *  Stores the left most doubleword of vector <B>xs</B> as a scalar
 *  doubleword at the effective address formed by <B>rb+ra</B>. The
 *  operand <B>rb</B> is a pointer to an array of doublewords.
 *  The operand <B>ra</B> is a doubleword integer byte offset
 *  from <B>rb</B>. For best performance <B>rb</B> and <B>ra</B>
 *  should be doubleword aligned (integer multiple of 8).
 *
 *  This operation is an alternate form of vector store element, with
 *  the added simplification that data is always left justified in the
 *  vector. This simplifies scatter operations.
 *
 *  \note This is instruction was introduced in PowerISA 2.06 (POWER7).
 *  For POWER9 there are additional optimizations by effectively
 *  converting small constant index values into displacements. For
 *  POWER9 we can use the stxsd (DS-form) instruction directly.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 0 - 2 | 2/cycle  |
 *  |power9   | 0 - 2 | 4/cycle  |
 *
 *  @param xs vector doubleword element 0 to be stored.
 *  @param ra const signed long long index (offset/displacement).
 *  @param rb const doubleword pointer to an array of doubles.
 */
static inline void
vec_vstsidx (vui64_t xs, const signed long long ra, unsigned long long *rb)
{
#if (defined(__clang__) && __clang_major__ < 8)
  __VEC_U_128 t;
  unsigned long long *p = (unsigned long long *)((char *)rb + ra);
  t.vx1 = xs;
  *p = t.ulong.upper;
#else
  if (__builtin_constant_p (ra) &&  (ra <= 32760) && (ra >= -32768)
      && ((ra & 3) == 0))
    {
#if defined (_ARCH_PWR9)
      __asm__(
	  "stxsd%X0 %1,%0;"
	  : "=m" (*((char *)rb + ra))
	  : "v" (xs)
	  : );
#else
      if (ra == 0)
	{
	  __asm__(
	      "stxsdx %x1,%y0;"
	      : "=Z" (*rb)
	      : "wa" (xs)
	      : );
	} else {
	  unsigned long long rt;
	  __asm__(
	      "li %0,%1;"
	      : "=r" (rt)
	      : "I" (ra)
	      : );
	  __asm__(
	      "stxsdx %x1,%y0;"
	      : "=Z" (*((char *)rb+rt))
	      : "wa" (xs)
	      : );
	}
#endif
    } else {
      __asm__(
	  "stxsdx %x1,%y0;"
	  : "=Z" (*((char *)rb+ra))
	  : "wa" (xs)
	  : );
    }
#endif
}

/** \brief Vector splat doubleword.
 *  Duplicate the selected doubleword element across the doubleword
 *  elements of the result.
 *
 *  \note This function implements the operation of a VSX Splat
 *  Doubleword Immediate instruction.
 *  This implementation is NOT Endian sensitive and the function is
 *  stable across BE/LE implementations.
 *
 *  The 1-bit control operand (ctl) selects which (0:1) doubleword
 *  element, from the vector operand, is replicated to both doublewords
 *  of the result vector.  Control table:
 *  ctl |  vrt[0:63]  | vrt[64:127]
 *  :-: | :---------: | :----------:
 *   0  |  vra[0:63]  | vra[0:63]
 *   1  | vra[64:127] | vra[64:127]
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param vra a 128-bit vector.
 *  @param ctl a const integer encoding the source doubleword.
 *  @return The original vector with the doubleword elements swapped.
 */
static inline vui64_t
vec_xxspltd (vui64_t vra, const int ctl)
{
  vui64_t result;
  /* Don't need to reverse the cases for LE because vec_permdi handles
     that.  */
  switch (ctl & 1)
    {
    case 0:
      result = vec_permdi (vra, vra, 0);
      break;
    case 1:
      result = vec_permdi (vra, vra, 3);
      break;
    }

  return (result);
}

/** \brief Vector Multiply-Add Even Unsigned Words.
 *
 *  Multiply the even 32-bit Words of vector unsigned int
 *  values (a * b) and return sums of the unsigned 64-bit product and
 *  the even 32-bit words of c
 *  (a<SUB>even</SUB> * b<SUB>even</SUB>) + EXTZ(c<SUB>even</SUB>).
 *
 *  \note The advantage of this form (versus Multiply-Sum) is that
 *  the final 64 bit sums can not overflow.
 *  \note This implementation is NOT endian sensitive and the function is
 *  stable across BE/LE implementations.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   9   | 2/cycle  |
 *  |power9   |   9   | 2/cycle  |
 *
 *  @param a 128-bit vector unsigned int.
 *  @param b 128-bit vector unsigned int.
 *  @param c 128-bit vector unsigned int.
 *  @return vector unsigned long int sum (a<SUB>even</SUB> * b<SUB>even</SUB>) + EXTZ(c<SUB>even</SUB>).
 */
static inline vui64_t
vec_vmaddeuw (vui32_t a, vui32_t b, vui32_t c)
{
  const vui32_t zero = { 0, 0, 0, 0 };
  vui64_t res;
  vui32_t c_euw = vec_mrgahw ((vui64_t) zero, (vui64_t) c);
  res = vec_vmuleuw (a, b);
  return vec_addudm (res, (vui64_t) c_euw);
}

/** \brief Vector Multiply-Add2 Even Unsigned Words.
 *
 *  Multiply the even 32-bit Words of vector unsigned int
 *  values (a * b) and return sums of the unsigned 64-bit product and
 *  the even 32-bit words of c and d
 *  (a<SUB>even</SUB> * b<SUB>even</SUB>) +
 *  EXTZ(c<SUB>even</SUB> + EXTZ(d<SUB>even</SUB>).
 *
 *  \note The advantage of this form (versus Multiply-Sum) is that
 *  the final 64 bit sums can not overflow.
 *  \note This implementation is NOT endian sensitive and the function is
 *  stable across BE/LE implementations.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   9   | 1/cycle  |
 *  |power9   |   9   | 1/cycle  |
 *
 *  @param a 128-bit vector unsigned int.
 *  @param b 128-bit vector unsigned int.
 *  @param c 128-bit vector unsigned int.
 *  @param d 128-bit vector unsigned int.
 *  @return vector unsigned long int sum (a<SUB>even</SUB> * b<SUB>even</SUB>) +
 *  EXTZ(c<SUB>even</SUB>) + EXTZ(d<SUB>even</SUB>).
 */
static inline vui64_t
vec_vmadd2euw (vui32_t a, vui32_t b, vui32_t c, vui32_t d)
{
  const vui32_t zero = { 0, 0, 0, 0 };
  vui64_t res, sum;
  vui32_t c_euw = vec_mrgahw ((vui64_t) zero, (vui64_t) c);
  vui32_t d_euw = vec_mrgahw ((vui64_t) zero, (vui64_t) d);
  res = vec_vmuleuw (a, b);
  sum = vec_addudm ( (vui64_t) c_euw, (vui64_t) d_euw);
  return vec_addudm (res, sum);
}

/** \brief Vector Multiply-Add Odd Unsigned Words.
 *
 *  Multiply the odd 32-bit Words of vector unsigned int
 *  values (a * b) and return sums of the unsigned 64-bit product and
 *  the odd 32-bit words of c
 *  (a<SUB>odd</SUB> * b<SUB>odd</SUB>) + EXTZ(c<SUB>odd</SUB>).
 *
 *  \note The advantage of this form (versus Multiply-Sum) is that
 *  the final 64 bit sums can not overflow.
 *  \note This implementation is NOT endian sensitive and the function is
 *  stable across BE/LE implementations.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   9   | 2/cycle  |
 *  |power9   |   9   | 2/cycle  |
 *
 *  @param a 128-bit vector unsigned int.
 *  @param b 128-bit vector unsigned int.
 *  @param c 128-bit vector unsigned int.
 *  @return vector unsigned long int sum (a<SUB>odd</SUB> * b<SUB>odd</SUB>) + EXTZ(c<SUB>odd</SUB>).
 */
static inline vui64_t
vec_vmaddouw (vui32_t a, vui32_t b, vui32_t c)
{
  const vui32_t zero = { 0, 0, 0, 0 };
  vui64_t res;
  vui32_t c_ouw = vec_mrgalw ((vui64_t) zero, (vui64_t) c);
  res = vec_vmulouw (a, b);
  return vec_addudm (res, (vui64_t) c_ouw);
}

/** \brief Vector Multiply-Add2 Odd Unsigned Words.
 *
 *  Multiply the odd 32-bit Words of vector unsigned int
 *  values (a * b) and return sums of the unsigned 64-bit product and
 *  the odd 32-bit words of c and d
 *  (a<SUB>odd</SUB> * b<SUB>odd</SUB>) +
 *  EXTZ(c<SUB>odd</SUB> + EXTZ(d<SUB>odd</SUB>).
 *
 *  \note The advantage of this form (versus Multiply-Sum) is that
 *  the final 64 bit sums can not overflow.
 *  \note This implementation is NOT endian sensitive and the function is
 *  stable across BE/LE implementations.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   9   | 1/cycle  |
 *  |power9   |   9   | 1/cycle  |
 *
 *  @param a 128-bit vector unsigned int.
 *  @param b 128-bit vector unsigned int.
 *  @param c 128-bit vector unsigned int.
 *  @param d 128-bit vector unsigned int.
 *  @return vector unsigned long int sum (a<SUB>odd</SUB> * b<SUB>odd</SUB>) +
 *  EXTZ(c<SUB>odd</SUB> + EXTZ(d<SUB>odd</SUB>).
 */
static inline vui64_t
vec_vmadd2ouw (vui32_t a, vui32_t b, vui32_t c, vui32_t d)
{
  const vui32_t zero = { 0, 0, 0, 0 };
  vui64_t res, sum;
  vui32_t c_ouw = vec_mrgalw ((vui64_t) zero, (vui64_t) c);
  vui32_t d_ouw = vec_mrgalw ((vui64_t) zero, (vui64_t) d);
  res = vec_vmulouw (a, b);
  sum = vec_addudm ((vui64_t) c_ouw, (vui64_t) d_ouw);
  return vec_addudm (res, sum);
}

/** \brief Vector Multiply-Sum Unsigned Word Modulo
 *
 *  Multiply the unsigned word elements of vra and vrb, internally
 *  generating doubleword products. Then generate three-way sum of
 *  adjacent doubleword product pairs, plus the doubleword elements
 *  from vrc. The final summation is modulo 64-bits.
 *
 *  \note This function implements the operation of a Vector
 *  Multiply-Sum Unsigned Word Modulo instruction, if the PowerISA
 *  included such an instruction.
 *  This implementation is NOT endian sensitive and the function is
 *  stable across BE/LE implementations.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  11   | 1/cycle  |
 *  |power9   |  11   | 1/cycle  |
 *
 *  @param vra 128-bit vector unsigned int.
 *  @param vrb 128-bit vector unsigned int.
 *  @param vrc 128-bit vector unsigned long.
 *  @return vector of doubleword elements where each is the sum of
 *  the even and odd adjacent products of the vra and vrb,
 *  plus the corresponding doubleword element of vrc.
 */
static inline vui64_t
vec_vmsumuwm (vui32_t vra, vui32_t vrb, vui64_t vrc)
{
  vui64_t peven, podd, psum;

  peven = vec_muleuw (vra, vrb);
  podd  = vec_mulouw (vra, vrb);
  psum  = vec_addudm (peven, podd);

  return vec_addudm (psum, vrc);
}

#endif /* VEC_INT64_PPC_H_ */
