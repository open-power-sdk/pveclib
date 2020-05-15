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
 * PowerISA 2.07 (POWER8) did add a significant number of doubleword
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
 * PowerISA 3.0 (POWER9) adds a few more doubleword
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
 * An impressive list of operations that can be used for;
 * - Vectorizing long integer loops
 * - Implementing useful quadword integer operations which do not have
 * corresponding PowerISA instructions
 * - implementing extended precision multiply and multiplicative
 * inverse operations
 *
 * The challenge is that useful operations available for POWER9 will
 * need equivalent implementations for POWER8 and POWER7.  Similarly
 * for operations introduced for POWER8 will need POWER7
 * implementations.  Also there are some obvious missing operations;
 * - Absolute Difference Doubleword (we have byte, halfword, and word)
 * - Average Doubleword (we have byte, halfword, and word)
 * - Extend Sign Doubleword to quadword (we have byte, halfword, and word)
 * - Multiply-sum Word (we have byte, halfword, and doubleword)
 * - Multiply Even/Odd Doublewords (we have byte, halfword, and word)
 *
 * \subsection i64_missing_ops_0_1 Challenges and opportunities
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
#ifdef _ARCH_PWR9
  __asm__(
      "vmsumudm %0,%1,%2,%3;\n"
      : "=v" (res)
      : "v" (a), "v" (b), "v" (c)
      : );
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
#ifdef _ARCH_PWR9
  const vui64_t zero = { 0, 0 };
  vui64_t b_eud = vec_mrgahd ((vui128_t) b, (vui128_t) zero);
  __asm__(
      "vmsumudm %0,%1,%2,%3;\n"
      : "=v" (res)
      : "v" (a), "v" (b_eud), "v" (zero)
      : );
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
 * The _ARCH_PWR9 implementation uses the multiply-sum doubleword
 * operation but implements the multiply even behavior by forcing the
 * contents of doubleword element 1 of [VRB] and the contents of
 * [VRC] to 0.
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
 * For example, this POWER8 only implementation:
 * \code
static inline vui64_t
vec_muludm (vui64_t vra, vui64_t vrb)
{
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
}
 * \endcode
 * Which generates the following for POWER8:
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
 * \note The addition of <I>zeros</I> to the final sum of
 * vec_vmsumuwm() (<I>vec_addudm (psum, vrc)</I>)has been optimized
 * away by the compiler.
 * This eliminates the xxspltib and one vaddudm instruction from the
 * final code sequence.
 *
 * And we can assume that the constant load of <I>{ 32, 32 }</I>
 * will be common-ed with other operations or hoisted out of
 * loops. So the shift constant can be loaded early and vrld is not
 * delayed.
 * This keeps the POWER8 latency in the 19-28 cycle range.
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
static inline vui64_t vec_maxud (vui64_t vra, vui64_t vrb);
static inline vui64_t vec_minud (vui64_t vra, vui64_t vrb);
static inline vui64_t vec_permdi (vui64_t vra, vui64_t vrb, const int ctl);
static inline vui64_t vec_subudm (vui64_t a, vui64_t b);
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

/** \brief Count leading zeros for a vector unsigned long int.
 *
 *  Count leading zeros for a vector __int128 and return the count in a
 *  vector suitable for use with vector shift (left|right) and vector
 *  shift (left|right) by octet instructions.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   2   | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated a __int128.
 *  @return a 128-bit vector with bits 121:127 containing the count of
 *  leading zeros.
 */
static inline vui64_t
vec_clzd (vui64_t vra)
{
  vui64_t r;
#ifdef _ARCH_PWR8
#if defined (vec_vclzd)
  r = vec_vclzd (vra);
#elif defined (__clang__)
  r = (vui32_t) vec_cntlz (vra);
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
  r = (vui64_t) vec_sum2s ((vi32_t) n, (vi32_t) z);
#endif
  return (r);
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
#elif defined (__clang__)
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
#elif defined (__clang__)
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
#elif defined (__clang__)
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
#elif defined (__clang__)
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

/** \brief \copybrief vec_int128_ppc.h::vec_msumudm()
 *
 * \note this implementation exists in
 * \ref vec_int128_ppc.h::vec_msumudm()
 * as it requires vec_adduqm().
 */
static inline vui128_t
vec_msumudm (vui64_t a, vui64_t b, vui128_t c);

/** \brief \copybrief vec_int128_ppc.h::vec_muleud()
 *
 * \note this implementation exists in
 * \ref vec_int128_ppc.h::vec_muleud()
 * as it requires vec_vmuleud and vec_adduqm().
 */
static inline vui128_t
vec_muleud (vui64_t a, vui64_t b);

/** \brief \copybrief vec_int128_ppc.h::vec_mulhud()
 *
 * \note this implementation exists in
 * \ref vec_int128_ppc.h::vec_mulhud()
 * as it requires vec_vmuleud() and vec_vmuloud().
 */
static inline vui64_t
vec_mulhud (vui64_t vra, vui64_t vrb);

/** \brief \copybrief vec_int128_ppc.h::vec_muloud()
 *
 * \note this implementation exists in
 * \ref vec_int128_ppc.h::vec_muloud()
 * as it requires vec_vmuloud() and vec_adduqm().
 */
static inline vui128_t
vec_muloud (vui64_t a, vui64_t b);

/** \brief \copybrief vec_int128_ppc.h::vec_muludm()
 *
 * \note this implementation exists in
 * \ref vec_int128_ppc.h::vec_muludm()
 * as it requires vec_vmuleud() and vec_vmuloud().
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
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
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
  r = (vui64_t) vec_sum2s ((vi32_t) x, (vi32_t) z);
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
  vui64_t lshift;
  vui64_t result;

  if (shb < 64)
    {
      /* Load the shift const in a vector.  The element shifts require
         a shift amount for each element. For the immediate form the
         shift constant is splatted to all elements of the
         shift control.  */
      if (__builtin_constant_p (shb) && (shb < 16))
	lshift = (vui64_t) vec_splat_s32(shb);
      else
	lshift = vec_splats ((unsigned long long) shb);

      /* Vector Shift right bytes based on the lower 6-bits of
         corresponding element of lshift.  */
      result = vec_vsld (vra, lshift);
    }
  else
    { /* shifts greater then 31 bits return zeros.  */
      result = vec_xor ((vui64_t) vra, (vui64_t) vra);
    }

  return (vui64_t) result;
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
  vui64_t rshift;
  vui64_t result;

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
      /* Vector Shift right bytes based on the lower 6-bits of
         corresponding element of rshift.  */
      result = vec_vsrd (vra, rshift);
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

/** \brief \copybrief vec_int128_ppc.h::vec_vmadd2eud()
 *
 * \note this implementation exists in
 * \ref vec_int128_ppc.h::vec_vmadd2eud()
 * as it requires vec_msumudm() and vec_adduqm().
 */
static inline vui128_t
vec_vmadd2eud (vui64_t a, vui64_t b, vui64_t c, vui64_t d);

/** \brief \copybrief vec_int128_ppc.h::vec_vmaddeud()
 *
 * \note this implementation exists in
 * \ref vec_int128_ppc.h::vec_vmaddeud()
 * as it requires vec_msumudm() and vec_adduqm().
 */
static inline vui128_t
vec_vmaddeud (vui64_t a, vui64_t b, vui64_t c);

/** \brief \copybrief vec_int128_ppc.h::vec_vmadd2oud()
 *
 * \note this implementation exists in
 * \ref vec_int128_ppc.h::vec_vmadd2oud()
 * as it requires vec_msumudm() and vec_adduqm().
 */
static inline vui128_t
vec_vmadd2oud (vui64_t a, vui64_t b, vui64_t c, vui64_t d);

/** \brief \copybrief vec_int128_ppc.h::vec_vmaddoud()
 *
 * \note this implementation exists in
 * \ref vec_int128_ppc.h::vec_vmaddoud()
 * as it requires vec_msumudm() and vec_adduqm().
 */
static inline vui128_t
vec_vmaddoud (vui64_t a, vui64_t b, vui64_t c);

/** \brief \copybrief vec_int128_ppc.h::vec_vmuleud()
 *
 * \note this implementation exists in
 * \ref vec_int128_ppc.h::vec_vmuleud()
 * as it requires vec_msumudm() and vec_adduqm().
 */
static inline vui128_t
vec_vmuleud (vui64_t a, vui64_t b);

/** \brief \copybrief vec_int128_ppc.h::vec_vmuloud()
 *
 * \note this implementation exists in
 * \ref vec_int128_ppc.h::vec_vmuloud()
 * as it requires vec_msumudm() and vec_adduqm().
 */
static inline vui128_t
vec_vmuloud (vui64_t a, vui64_t b);

/** \brief \copybrief vec_int128_ppc.h::vec_vmsumeud()
 *
 * \note this implementation exists in
 * \ref vec_int128_ppc.h::vec_vmsumeud()
 * as it requires vec_msumudm() and vec_adduqm().
 */
static inline vui128_t
vec_vmsumeud (vui64_t a, vui64_t b, vui128_t c);

/** \brief \copybrief vec_int128_ppc.h::vec_vmsumoud()
 *
 * \note this implementation exists in
 * \ref vec_int128_ppc.h::vec_vmsumoud()
 * as it requires vec_msumudm() and vec_adduqm().
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
#ifndef __clang__
#ifndef vec_vpkudum
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
