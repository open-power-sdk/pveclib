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

 vec_int16_ppc.h

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Apr 06, 2018
 */

#ifndef VEC_INT16_PPC_H_
#define VEC_INT16_PPC_H_

#include <pveclib/vec_char_ppc.h>

/*!
 * \file  vec_int16_ppc.h
 * \brief Header package containing a collection of 128-bit SIMD
 * operations over 16-bit integer elements.
 *
 * Most of these operations are implemented in a single instruction
 * on newer (POWER6/POWER7POWER8/POWER9) processors.
 * This header serves to fill in functional gaps for older
 * (POWER7, POWER8) processors and provides a in-line assembler
 * implementation for older compilers that do not
 * provide the build-ins.
 *
 * Most vector short (16-bit integer halfword) operations are
 * implemented with PowerISA VMX instructions either defined by the
 * original VMX (AKA Altivec) or added to later versions of the
 * PowerISA.
 * PowerISA 2.07B (POWER8) added several useful halfword operations
 * (count leading zeros, population count) not included in the
 * original VMX.
 * PowerISA 3.0B (POWER9) adds several more (absolute difference,
 * compare not equal, count trailing zeros, extend sign,
 * extract/insert, and reverse bytes).
 * Most of these intrinsic (compiler built-ins) operations are defined
 * in <altivec.h> and described in the compiler documentation.
 *
 * \note The compiler disables associated <altivec.h> built-ins if the
 * <B>mcpu</B> target does not enable the specific instruction.
 * For example if you compile with <B>-mcpu=power7</B>, vec_vclz and
 * vec_vclzh will not be defined.
 * Another example if you compile with <B>-mcpu=power8</B>, vec_revb
 * will not be defined. But vec_vclzh and vec_revbh is always defined
 * in this header.
 * This header provides the appropriate substitutions,
 * will generate the minimum code, appropriate for the target,
 * and produce correct results.
 *
 * \note Most ppc64le compilers will default to -mcpu=power8 if not
 * specified.
 *
 *
 * This header covers operations that are either:
 *
 * - Implemented in hardware instructions for later
 * processors and useful to programmers, on slightly older processors,
 * even if the equivalent function requires more instructions.
 * Examples include Count Leading Zeros, Population Count and Byte
 * Reverse.
 * - Defined in the OpenPOWER ABI but <I>not</I> yet defined in
 * <altivec.h> provided by available compilers in common use.
 * Examples include Count Leading Zeros, Population Count and Byte
 * Reverse.
 * - Commonly used operations, not covered by the ABI or
 * <altivec.h>, and require multiple instructions or
 * are not obvious.
 * Examples include the multiply-add, multiply-high and shift immediate
 * operations.
 *
 * \section i16_recent_additions Recent Additions
 *
 * Added vec_vmaddeuh() and vec_vmaddouh() as an optimization for
 * the vector multiply quadword implementations on POWER7.
 *
 * \section i16_endian_issues_0_0 Endian problems with halfword operations
 *
 * It would be useful to provide a vector multiply high halfword
 * (return the high order 16-bits of the 32-bit product) operation.
 * This can be used for multiplicative inverse (effectively integer
 * divide) operations.  Neither integer multiply high nor divide
 * are available as vector instructions.  However the multiply high
 * halfword operation can be composed from the existing multiply
 * even/odd halfword operations followed by the vector merge even
 * halfword operation.
 * Similarly a multiply low (modulo) halfword operation can be
 * composed from the existing multiply even/odd halfword
 * operations followed by the vector merge odd halfword operation.
 *
 * As a prerequisite we need to provide the merge even/odd halfword
 * operations.
 * While PowerISA has added these operations for word and doubleword,
 * instructions are nor defined for byte and halfword.
 * Fortunately vector merge operations are
 * just a special case of vector permute.  So the vec_vmrgoh() and
 * vec_vmrgeh() implementation can use vec_perm and appropriate
 * selection vectors to provide these merge operations.
 *
 * But this is complicated by <I>little-endian</I> (LE) support as
 * specified in the OpenPOWER ABI and as implemented in the compilers.
 * Little-endian changes the effective vector element numbering and
 * the location of even and odd elements.  This means that the vector
 * built-ins provided by altivec.h may not generate the instructions
 * you would expect.
 * \sa \ref mainpage_endian_issues_1_1
 * \sa \ref i32_endian_issues_0_0
 *
 * The OpenPOWER ABI provides a helpful table of
 * <a href="http://openpowerfoundation.org/wp-content/uploads/resources/leabi/content/dbdoclet.50655244_90667.html"
 * >Endian Sensitive Operations</a>.
 * For for vec_mule (vmuleuh, vmulesh):
 * <BLOCKQUOTE>Replace with vmulouh and so on, for LE.</BLOCKQUOTE>
 * For for vec_mulo (vmulouh, vmulosh):
 * <BLOCKQUOTE>Replace with vmuleuh and so on, for LE.</BLOCKQUOTE>
 * Also for vec_perm (vperm) it specifies:
 * <BLOCKQUOTE>For LE, Swap input arguments and complement the selection vector.</BLOCKQUOTE>
 * The above is just a sampling of a larger list of Endian Sensitive Operations.
 *
 * The obvious coding for Vector Multiply High Halfword would be:
 * \code
vui16_t
test_mulhw (vui16_t vra, vui16_t vrb)
{
  return vec_mergee ((vui16_t)vec_mule (vra, vrb),
		     (vui16_t)vec_mulo (vra, vrb));
}
 * \endcode
 * A couple problems with this:
 * - vec_mergee is only defined for vector int/long and float/double
 * (word/doubleword) types.
 * - vec_mergee is endian sensitive and would produce the wrong
 * results in LE mode.
 * - vec_mule/vec_mulo are endian sensitive and produce the wrong
 * results in LE mode.
 *
 * The first step is to implement Vector Merge Even Halfword operation:
 * \code
static inline vui16_t
vec_vmrgeh (vui16_t vra, vui16_t vrb)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  vui16_t permute =
      { 0x0302,0x1312, 0x0706,0x1716, 0x0B0A,0x1B1A, 0x0F0E,0x1F1E };

  return vec_perm (vrb, vra, (vui8_t)permute);
#else
  vui16_t permute =
      { 0x0001,0x1011, 0x0405,0x1415, 0x0809,0x1819, 0x0C0D,0x1C1D};

  return vec_perm (vra, vrb, (vui8_t)permute);
#endif
}
 * \endcode
 * For big-endian we have a straight forward vec_perm with a permute
 * select vector interleaving even halfwords from vectors vra and vrb.
 *
 * For little-endian we need to nullify the LE transform applied by
 * the compiler.  So the select vector looks like it interleaves odd
 * halfwords from vectors vrb and vra.  It also reverses byte numbering
 * within halfwords.  The compiler transforms this back into the
 * operation we wanted in the first place. The result is <I>not</I>
 * endian sensitive and is stable across BE/LE implementations.
 * Similarly for the Vector Merge Odd Halfword operation.
 *
 * As good OpenPOWER ABI citizens we should also provide endian
 * sensitive operations vec_mrgeh() vec_mrgoh().
 * For example:
 * \code
static inline vui16_t
vec_mrgeh  (vui16_t vra, vui16_t vrb)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return vec_vmrgoh ((vui16_t) vrb, (vui16_t) vra);
#else
  return vec_vmrgeh ((vui16_t) vra, (vui16_t) vrb);
#endif
}
 * \endcode
 * \note This is essentially what the compiler would do for vec_mergee.
 *
 * Also to follow that pattern established for vec_int32_ppc.h we
 * should provide implementations for Vector Merge Algebraic High/Low
 * Halfword. For example:
 * \code
static inline vui16_t
vec_mrgahh  (vui32_t vra, vui32_t vrb)
{
  return vec_vmrgeh ((vui16_t) vra, (vui16_t) vrb);
}
 * \endcode
 * This is simpler as we can use the endian invariant vec_vmrgeh() operation.
 * Similarly for Vector Merge Algebraic Low Halfword using vec_vmrgoh().
 * \note The inputs are defined as 32-bit to match the results from
 * multiply even/odd halfword.
 *
 * Now we have all the parts we need to implement multiply high/low halfword.
 * For example Multiply High Unsigned Halfword:
 * \code
static inline vui16_t
vec_mulhuh (vui16_t vra, vui16_t vrb)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return vec_mrgahh (vec_mulo (vra, vrb), vec_mule (vra, vrb));
#else
  return vec_mrgahh (vec_mule (vra, vrb), vec_mulo (vra, vrb));
#endif
}
 * \endcode
 *
 * Similarly for Multiply High Signed Halfword.
 * \note For LE we need to nullify the compiler transform by reversing
 * of the order of vec_mulo/vec_mule.  This is required to get the
 * algebraically correct (multiply high) result.
 *
 * Finally we can implement the Multiply Low Halfword which by PowerISA
 * conventions is called Multiply Unsigned Halfword Modulo:
 * \code
static inline vui16_t
vec_muluhm (vui16_t vra, vui16_t vrb)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return vec_mrgalh (vec_mulo (vra, vrb), vec_mule (vra, vrb));
#else
  return vec_mrgalh (vec_mule (vra, vrb), vec_mulo (vra, vrb));
#endif
}
 * \endcode
 * \note We use the endian stable vec_mrgalh() for multiply low.
 * Again for LE we have to nullify the compiler transform by reversing
 * of the order of vec_mulo/vec_mule.  This is required to get the
 * algebraically correct (multiply high) result.
 * \note vec_muluhm() works for signed and unsigned multiply low (modulo).
 *
 * \subsection i16_endian_issues_0_1 Multiply High Unsigned Halfword Example
 * So what does the compiler generate after unwinding three levels of inline functions.
 * For this test case:
 * \code
vui16_t
__test_mulhuh (vui16_t a, vui16_t b)
{
  return vec_mulhuh (a, b);
}
 * \endcode
 * The GCC 8 compiler targeting powerpc64le and -mcpu=power8 generates:
 * \code
  	addis   r9,r2,.rodata.cst16@ha
  	vmulouh v1,v2,v3
  	vmuleuh v2,v2,v3
  	addi    r9,r9,.rodata.cst16@l
  	lvx     v0,0,r9
  	xxlnor  vs32,vs32,vs32
  	vperm   v2,v2,v1,v0
 * \endcode
 * The addis, addi, lvx instruction sequence loads the permute
 * selection constant vector.  The xxlnor instruction complements the
 * selection vector for LE.  These instructions are only needed once
 * per function and can be hoisted out of loops and shared across
 * instances of vec_mulhuh(). Which might look like this:
 * \code
  	addis   r9,r2,.rodata.cst16@ha
  	addi    r9,r9,.rodata.cst16@l
  	lvx     v0,0,r9
  	xxlnor  vs32,vs32,vs32
  	...
  Loop:
  	vmulouh v1,v2,v3
  	vmuleuh v2,v2,v3
  	vperm   v2,v2,v1,v0
  	...
 * \endcode
 * The vmulouh, vmuleuh, vperm instruction sequence is the core of the
 * function.  They multiply the elements and selects/merges the high
 * order 16-bits of each product into the result vector.
 *
 * \section int16_examples_0_1 Examples, Divide by integer constant
 * Suppose we have a requirement to convert an array of 16-bit
 * unsigned short values to decimal. The classic <I>itoa</I>
 * implementation performs a sequence of divide / modulo by 10
 * operations that produce one (decimal) value per iteration,
 * until the divide returns 0.
 *
 * For this example we want to vectorize the operation and the
 * PowerISA (and most other platforms) does not provide a
 * vector integer divide instruction. But we do have vector integer
 * multiply.  As we will see the multiply high defined above is very
 * handy for applying the multiplicative inverse.
 * Also, the conversion divide is a constant value applied across
 * the vector which simplifies the coding.
 *
 * Here we can use the multiplicative inverse which is a
 * scaled fixed point fraction calculated from the original divisor.
 * This works nicely if the fixed radix point is just before
 * the 16-bit fraction and we have a multiply high (vec_mulhuh())
 * operation. Multiplying a 16-bit unsigned integer by a 16-bit
 * unsigned fraction generates a 32-bit product with 16-bits above
 * (integer) and below (fraction) the radix point. The high 16-bits
 * of the product is a good approximation of the integer quotient.
 *
 * It turns out that generating the multiplicative inverse can be
 * tricky.  To produce correct results over the full range, requires
 * possible pre-scaling and post-shifting, and sometimes a corrective
 * addition. Fortunately, the mathematics are well
 * understood and are commonly used in optimizing compilers.
 * Even better, Henry Warren's book has a whole chapter on this topic.
 * \see "Hacker's Delight, 2nd Edition,"
 * Henry S. Warren, Jr, Addison Wesley, 2013.
 * Chapter 10, Integer Division by Constants.
 *
 * \subsection int16_examples_0_1_1 Divide by constant 10 examples
 * In the chapter above;
 * <BLOCKQUOTE>Figure 10-2 Computing the magic number for unsigned division.</BLOCKQUOTE>
 * provides a sample C function for generating the magic number
 * (actually a struct containing; the magic multiplicative inverse,
 * "add" indicator, and the shift amount).
 * For the 16-bit unsigned divisor 10, this is {  52429, 0, 3 }:
 * - the multiplier is 52429.
 * - no corrective add of the dividend is required.
 * - the final shift is 3-bits right.
 *
 * Which could look like this:
 * \code
 vui16_t
__test_div10 (vui16_t n)
{
  vui16_t q;
  // M= 52429, a=0, s=3
  vui16_t magic = vec_splats ((unsigned short) 52429);
  const int s = 3;

  q = vec_mulhuh (magic, n);
  return vec_srhi (q, s);
}
 * \endcode
 *
 * But we also need the modulo to extract each digit. The simplest and oldest technique is to
 * multiply the quotient by the divisor (constant 10) and subtract that from the original dividend.
 * Here we can use the vec_muluhm() operation we defined above.
 * Which could look like this:
 * \code
 vui16_t
__test_mod10 (vui16_t n)
{
  vui16_t q;
  // M= 52429, a=0, s=3
  vui16_t magic = vec_splats ((unsigned short) 52429);
  vui16_t c_10 = vec_splats ((unsigned short) 10);
  const int s = 3;
  vui16_t tmp, rem, q_10;

  q = vec_mulhuh (magic, n);
  q_10 = vec_srhi (q, s);
  tmp = vec_muluhm (q_10, c_10);
  rem = vec_sub (n, tmp);
  return rem;
}
 * \endcode
 *
 * \note vec_sub() and vec_splats() are an existing
 * altivec.h generic built-ins.
 *
 * \subsection int16_examples_0_1_2 Divide by constant 10000 example
 * As we mentioned above, some divisors require an add before the shift
 * as a correction.
 * For the 16-bit unsigned divisor 10000 this is { 41839, 1, 14 }:
 * - the multiplier is 41839.
 * - corrective add of the dividend is required.
 * - the final shift is 14-bits right.
 *
 * In this case the perfect multiplier is too large (>= 2**16).
 * So the magic multiplier is reduced by 2**16
 * and to correct for this we need to add the dividend to the product.
 * This add may generate a carry that must be included in the shift.
 * Here vec_avg handles the 17-bit sum internally before shifting right 1.
 * But vec_avg adds an extra +1 (for rounding) that we don't want.
 * So we use (n-1) for the product correction
 * then complete the operation with shift right (s-1).
 * Which could look like this:
 * \code
vui16_t
__test_div10000 (vui16_t n)
{
  vui16_t result, q;
  // M= 41839, a=1, s=14
  vui16_t magic = vec_splats ((unsigned short) 41839);
  const int s = 14;
  vui16_t tmp, rem;

  q = vec_mulhuh (magic, n);
    {
      const vui16_t vec_ones = vec_splat_u16 ( 1 );
      vui16_t n_1 = vec_sub (n, vec_ones);
      // avg = (q + (n-1) + 1) >> 1
      q = vec_avg (q, n_1);
      result = vec_srhi (q, (s - 1));
    }
  return result;
}
 * \endcode
 *
 * \note vec_avg(), vec_sub(), vec_splats() and vec_splat_u16() are
 * existing altivec.h generic built-ins.
 *
 * The modulo computation remains the same as \ref int16_examples_0_1_1.
 *
 * \section int16_perf_0_0 Performance data.
 *
 * We can use the example above (see \ref i16_endian_issues_0_1) to
 * illustrate the performance metrics pveclib provides.
 * For vec_mulhuh() the core operation is the sequence
 * vmulouh/vmuleuh/vperm. This represents the best case latency,
 * when it is used multiple times in a single larger function.
 *
 * The compiler notes that vmulouh/vmuleuh are independent
 * instructions that can execute concurrently
 * (in separate vector pipelines).
 * The compiler schedules them to issue in same cycle.
 * The latency for vmulouh/vmuleuh is listed as 7 cycle and the
 * throughput of 2 per cycle (there are 2 vector pipes for multiply).
 * As we assume this function will use both vector pipelines, the
 * throughput for this function is reduced to 1 per cycle.
 *
 * We still need to select/merge the results. The vperm instruction is
 * dependent on the execution of both vmulouh/vmuleuh and load of the
 * select vector complete.  For this case we assume that the load of
 * the permute select vector has already executed.  The processor can
 * not issue the vperm until both vmulouh/vmuleuh instructions execute.
 * The latency for vperm is 2 cycles (3 on POWER9).  So the best case
 * latency for this operation is is (7 + 2 = 9) cycles (10 on POWER9).
 *
 * Looking at the first or only execution of vec_mulhuh() in a function
 * defines the worse case latency.
 * Here we have to include the permute select vector load and (for LE)
 * the select vector complement.
 * However this case provides additional multiple pipe parallelism that
 * needs to be accounted for in the latencies.
 *
 * The compiler notes that addis/vmulouh/vmuleuh are independent
 * instructions that can execute concurrently in separate pipelines.
 * So the compiler schedules them to issue in same cycle.
 * The latency for vmulouh/vmuleuh is 7 cycles while the addis latency
 * is only 2 cycles.  The dependent addi instruction can issue in
 * the 3rd cycle, while vmulouh/vmuleuh are still executing.
 * The addi also has a 2 cycle latency, so the dependent lvx can issue
 * in the 5th cycle, while vmulouh/vmuleuh are still executing.
 * The lvx has a latency of 5 cycles and will not complete execution
 * until 2 cycles after vmulouh/vmuleuh.
 * The dependent xxlnor is waiting of the load (lvx)
 * and has a latency of 2 cycles.
 *
 * So there are two independent instruction sequences;
 * vmulouh/vmuleuh and addis/addi/lvx/xxlnor.
 * Both must complete
 * execution before the vperm can issue and complete the operation.
 * The later sequence has the longer (2+2+5+2=11) latency and
 * dominates the timing. So the worst latency for the full sequence
 * is (2+2+5+2+2 = 13) cycles (14 on POWER9).
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   | 9-13  | 1/cycle  |
 * |power9   | 10-14 | 1/cycle  |
 *
 * \subsection int16_perf_0_1 More information.
 * High level performance estimates are provided as an aid to function
 * selection when evaluating algorithms. For background on how
 * <I>Latency</I> and <I>Throughput</I> are derived see:
 * \ref perf_data
 */

///@cond INTERNAL
static inline vui16_t vec_vmrgeh (vui16_t vra, vui16_t vrb);
static inline vui16_t vec_vmrgoh (vui16_t vra, vui16_t vrb);
///@endcond

/** \brief Vector Absolute Difference Unsigned halfword.
 *
 *  Compute the absolute difference for each halfword.
 *  For each unsigned halfword, subtract VRB[i] from VRA[i] and return
 *  the absolute value of the difference.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   4   | 1/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param vra vector of 8 x unsigned halfword
 *  @param vrb vector of 8 x unsigned halfword
 *  @return vector of the absolute differences.
 */
static inline vui16_t
vec_absduh (vui16_t vra, vui16_t vrb)
{
  vui16_t result;
#ifdef _ARCH_PWR9
#ifdef vec_absdh
  result = vec_absdh (vra, vrb);
#else
  __asm__(
      "vabsduh %0,%1,%2;"
      : "=v" (result)
      : "v" (vra), "v" (vrb)
      : );
#endif
#else
  vui16_t vmin, vmax;

  vmin = vec_min (vra, vrb);
  vmax = vec_max (vra, vrb);
  result = vec_sub (vmax, vmin);
#endif
  return (result);
}

/** \brief Count Leading Zeros for a vector unsigned short (halfword)
 *  elements.
 *
 *  Count the number of leading '0' bits (0-16) within each halfword
 *  element of a 128-bit vector.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the Vector Count Leading
 *  Zeros Halfword instruction <B>vclzh</B>. Otherwise use sequence of pre
 *  2.07 VMX instructions.
 *
 *  \note SIMDized count leading zeros inspired by:
 *  Warren, Henry S. Jr and <I>Hacker's Delight</I>, 2nd Edition,
 *  Addison Wesley, 2013. Chapter 5 Counting Bits, Figure 5-12.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
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

/** \brief Vector Merge Algebraic High Halfword operation.
 *
 * Merge only the high halfwords from 8 x Algebraic words
 * across vectors vra and vrb. This is effectively the Vector Merge
 * Even Halfword operation that is not modified for endian.
 *
 * For example merge the high 16-bits from each of 8 x 32-bit products
 * as generated by vec_muleuh/vec_mulouh. This result is effectively
 * a vector multiply high unsigned halfword.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-13  | 2/cycle  |
 *  |power9   | 3-14  | 2/cycle  |
 *
 * @param vra 128-bit vector unsigned int.
 * @param vrb 128-bit vector unsigned int.
 * @return A vector merge from only the high halfwords of the 8 x
 * Algebraic words across vra and vrb.
 */
static inline vui16_t
vec_mrgahh  (vui32_t vra, vui32_t vrb)
{
  return vec_vmrgeh ((vui16_t) vra, (vui16_t) vrb);
}

/** \brief Vector Merge Algebraic Low Halfword operation.
 *
 * Merge only the low halfwords from 8 x Algebraic words
 * across vectors vra and vrb. This is effectively the Vector Merge
 * Odd Halfword operation that is not modified for endian.
 *
 * For example merge the low 16-bits from each of 8 x 32-bit products
 * as generated by vec_muleuh/vec_mulouh. This result is effectively
 * a vector multiply low unsigned halfword.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-13  | 2/cycle  |
 *  |power9   | 3-14  | 2/cycle  |
 *
 * @param vra 128-bit vector unsigned int.
 * @param vrb 128-bit vector unsigned int.
 * @return A vector merge from only the high halfwords of the 8 x
 * Algebraic words across vra and vrb.
 */
static inline vui16_t
vec_mrgalh  (vui32_t vra, vui32_t vrb)
{
  return vec_vmrgoh ((vui16_t) vra, (vui16_t) vrb);
}

/** \brief Vector Merge Even Halfwords operation.
 *
 * Merge the even halfword elements from the concatenation of 2 x
 * vectors (vra and vrb).
 *
 * \note The element numbering changes between big and little-endian.
 * So the compiler and this implementation adjusts the generated code
 * to reflect this.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-13  | 2/cycle  |
 *  |power9   | 3-14  | 2/cycle  |
 *
 * @param vra 128-bit vector unsigned short.
 * @param vrb 128-bit vector unsigned short.
 * @return A vector merge from only the even halfwords of vra and vrb.
 */
static inline vui16_t
vec_mrgeh  (vui16_t vra, vui16_t vrb)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return vec_vmrgoh ( vrb, vra );
#else
  return vec_vmrgeh ( vra, vrb );
#endif
}

/** \brief Vector Merge Odd Halfwords operation.
 *
 * Merge the odd halfword elements from the concatenation of 2 x
 * vectors (vra and vrb).
 *
 * \note The element numbering changes between big and little-endian.
 * So the compiler and this implementation adjusts the generated code
 * to reflect this.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-13  | 2/cycle  |
 *  |power9   | 3-14  | 2/cycle  |
 *
 * @param vra 128-bit vector unsigned short.
 * @param vrb 128-bit vector unsigned short.
 * @return A vector merge from only the odd halfwords of vra and vrb.
 */
static inline vui16_t
vec_mrgoh  (vui16_t vra, vui16_t vrb)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return vec_vmrgeh ( vrb, vra );
#else
  return vec_vmrgoh ( vra, vrb );
#endif
}

/** \brief Vector Multiply High Signed halfword.
 *
 *  Multiple the corresponding halfword elements of two vector signed
 *  short values and return the high order 16-bits, for each
 *  32-bit product element.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 9-13  | 1/cycle  |
 *  |power9   | 10-14 | 1/cycle  |
 *
 *  @param vra 128-bit vector signed short.
 *  @param vrb 128-bit vector signed short.
 *  @return vector of the high order 16-bits of the product of the
 *  halfword elements from vra and vrb.
 */
static inline vi16_t
vec_mulhsh (vi16_t vra, vi16_t vrb)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return (vi16_t) vec_mrgahh ((vui32_t)vec_mulo (vra, vrb),
			      (vui32_t)vec_mule (vra, vrb));
#else
  return (vi16_t) vec_mrgahh ((vui32_t)vec_mule (vra, vrb),
			      (vui32_t)vec_mulo (vra, vrb));
#endif
}

/** \brief Vector Multiply High Unsigned halfword.
 *
 *  Multiply the corresponding halfword elements of two vector unsigned
 *  short values and return the high order 16-bits, for each
 *  32-bit product element.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 9-13  | 1/cycle  |
 *  |power9   | 10-14 | 1/cycle  |
 *
 *  @param vra 128-bit vector unsigned short.
 *  @param vrb 128-bit vector unsigned short.
 *  @return vector of the high order 16-bits of the product of the
 *  halfword elements from vra and vrb.
 */
static inline vui16_t
vec_mulhuh (vui16_t vra, vui16_t vrb)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return vec_mrgahh (vec_mulo (vra, vrb), vec_mule (vra, vrb));
#else
  return vec_mrgahh (vec_mule (vra, vrb), vec_mulo (vra, vrb));
#endif
}

/** \brief Vector Multiply Unsigned halfword Modulo.
 *
 *  Multiply the corresponding halfword elements of two vector unsigned
 *  short values and return the low order 16-bits of the 32-bit product
 *  for each element.
 *
 *  \note vec_muluhm can be used for unsigned or signed short integers.
 *  It is the vector equivalent of Multiply Low Halfword.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 9-13  | 1/cycle  |
 *  |power9   | 10-14 | 1/cycle  |
 *
 *  @param vra 128-bit vector unsigned short.
 *  @param vrb 128-bit vector unsigned short.
 *  @return vector of the low order 16-bits of the unsigned product
 *  of the halfword elements from vra and vrb.
 */
static inline vui16_t
vec_muluhm (vui16_t vra, vui16_t vrb)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return vec_mrgalh (vec_mulo (vra, vrb), vec_mule (vra, vrb));
#else
  return vec_mrgalh (vec_mule (vra, vrb), vec_mulo (vra, vrb));
#endif
}

/** \brief Vector Population Count halfword.
 *
 *  Count the number of '1' bits (0-16) within each byte element of
 *  a 128-bit vector.
 *
 *  For POWER8 (PowerISA 2.07B) or later use the Vector Population
 *  Count Halfword instruction. Otherwise use simple Vector (VMX)
 *  instructions to count bits in bytes in parallel.
 *
 *  \note SIMDized population count inspired by:
 *  Warren, Henry S. Jr and <I>Hacker's Delight</I>, 2nd Edition,
 *  Addison Wesley, 2013. Chapter 5 Counting Bits, Figure 5-2.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
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
 *  For each halfword of the input vector, reverse the order of
 *  bytes / octets within the halfword.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-11  | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param vra a 128-bit vector unsigned short.
 *  @return a 128-bit vector with the bytes of each halfword
 *  reversed.
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
 *  Shift left each halfword element [0-7], 0-15 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-15.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 15 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-11  | 2/cycle  |
 *  |power9   | 5-11  | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned short.
 *  @param shb Shift amount in the range 0-15.
 *  @return 128-bit vector unsigned short, shifted left shb bits.
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
 *  Shift right each halfword element [0-7], 0-15 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-15.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 15 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-11  | 2/cycle  |
 *  |power9   | 5-11  | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned short.
 *  @param shb Shift amount in the range 0-15.
 *  @return 128-bit vector unsigned short, shifted right shb bits.
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

      /* Vector Shift right halfword based on the lower 4-bits of
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
 *  Shift right algebraic each halfword element [0-7],
 *  0-15 bits, as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-15.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 7 bits return the sign bit
 *  propagated to each bit of each element.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-11  | 2/cycle  |
 *  |power9   | 5-11  | 2/cycle  |
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
         all bits.  This is equivalent to shift Right Algebraic of
         15 bits.  */
      lshift = (vui16_t) vec_splat_s16(15);
      result = vec_vsrah (vra, lshift);
    }

  return (vi16_t) result;
}

/** \brief Vector Multiply-Add Even Unsigned Halfwords.
 *
 *  Multiply the even 16-bit Words of vector unsigned short
 *  values (a * b) and return sums of the unsigned 32-bit product and
 *  the even 16-bit halfwords of c
 *  (a<SUB>even</SUB> * b<SUB>even</SUB>) + EXTZ(c<SUB>even</SUB>).
 *
 *  \note The advantage of this form (versus Multiply-Sum) is that
 *  the final 32 bit sums can not overflow.
 *  \note This implementation is NOT endian sensitive and the function is
 *  stable across BE/LE implementations.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 9-18  | 2/cycle  |
 *  |power9   | 9-16  | 2/cycle  |
 *
 *  @param a 128-bit vector unsigned short.
 *  @param b 128-bit vector unsigned short.
 *  @param c 128-bit vector unsigned short.
 *  @return vector unsigned int sum (a<SUB>even</SUB> * b<SUB>even</SUB>) + EXTZ(c<SUB>even</SUB>).
 */
static inline vui32_t
vec_vmaddeuh (vui16_t a, vui16_t b, vui16_t c)
{
  const vui16_t zero = { 0, 0, 0, 0,  0, 0, 0, 0 };
  vui32_t res;
  vui16_t c_euh = vec_mrgahh ((vui32_t) zero, (vui32_t) c);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  res = vec_vmulouh (a, b);
#else
  res = vec_vmuleuh (a, b);
#endif
  return vec_vadduwm (res, (vui32_t) c_euh);
}

/** \brief Vector Multiply-Add Odd Unsigned Halfwords.
 *
 *  Multiply the odd 16-bit Halfwords of vector unsigned short
 *  values (a * b) and return sums of the unsigned 32-bit product and
 *  the odd 16-bit halfwords of c
 *  (a<SUB>odd</SUB> * b<SUB>odd</SUB>) + EXTZ(c<SUB>odd</SUB>).
 *
 *  \note The advantage of this form (versus Multiply-Sum) is that
 *  the final 32 bit sums can not overflow.
 *  \note This implementation is NOT endian sensitive and the function is
 *  stable across BE/LE implementations.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 9-18  | 2/cycle  |
 *  |power9   | 9-16  | 2/cycle  |
 *
 *  @param a 128-bit vector unsigned short.
 *  @param b 128-bit vector unsigned short.
 *  @param c 128-bit vector unsigned short.
 *  @return vector unsigned int sum (a<SUB>odd</SUB> * b<SUB>odd</SUB>) + EXTZ(c<SUB>odd</SUB>).
 */
static inline vui32_t
vec_vmaddouh (vui16_t a, vui16_t b, vui16_t c)
{
  const vui16_t zero = { 0, 0, 0, 0,  0, 0, 0, 0 };
  vui32_t res;
  vui16_t c_ouh = vec_mrgalh ((vui32_t) zero, (vui32_t) c);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  res = vec_vmuleuh (a, b);
#else
  res = vec_vmulouh (a, b);
#endif
  return vec_vadduwm (res, (vui32_t) c_ouh);
}

/** \brief Vector Merge Even Halfwords.
 *
 * Merge the even halfword elements from the concatenation of 2 x
 * vectors (vra and vrb).
 *
 * \note This function implements the operation of a Vector Merge Even
 * Halfword instruction, if the PowerISA included such an instruction.
 * This implementation is NOT endian sensitive and the function is
 * stable across BE/LE implementations. Using big-endian element
 * numbering:
 * - res[0] = vra[0];
 * - res[1] = vrb[0];
 * - res[2] = vra[2];
 * - res[3] = vrb[2];
 * - res[4] = vra[4];
 * - res[5] = vrb[4];
 * - res[6] = vra[6];
 * - res[7] = vrb[6];
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-13  | 2/cycle  |
 *  |power9   | 3-14  | 2/cycle  |
 *
 * @param vra 128-bit vector unsigned short.
 * @param vrb 128-bit vector unsigned short.
 * @return A vector merge from only the even halfwords of vra and vrb.
 */
static inline vui16_t
vec_vmrgeh (vui16_t vra, vui16_t vrb)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  const vui16_t permute =
      { 0x0302,0x1312, 0x0706,0x1716, 0x0B0A,0x1B1A, 0x0F0E,0x1F1E };
  return vec_perm (vrb, vra, (vui8_t)permute);
#else
  const vui16_t permute =
      { 0x0001,0x1011, 0x0405,0x1415, 0x0809,0x1819, 0x0C0D,0x1C1D };
  return vec_perm (vra, vrb, (vui8_t)permute);
#endif
}

/** \brief Vector Merge Odd Halfwords.
 *
 * Merge the odd halfword elements from the concatenation of 2 x
 * vectors (vra and vrb).
 *
 * \note This function implements the operation of a Vector Merge Odd
 * Halfword instruction, if the PowerISA included such an instruction.
 * This implementation is NOT endian sensitive and the function is
 * stable across BE/LE implementations. Using big-endian element
 * numbering:
 * - res[0] = vra[1];
 * - res[1] = vrb[1];
 * - res[2] = vra[3];
 * - res[3] = vrb[3];
 * - res[4] = vra[5];
 * - res[5] = vrb[5];
 * - res[6] = vra[7];
 * - res[7] = vrb[7];
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-13  | 2/cycle  |
 *  |power9   | 3-14  | 2/cycle  |
 *
 * @param vra 128-bit vector unsigned short.
 * @param vrb 128-bit vector unsigned short.
 * @return A vector merge from only the odd halfwords of vra and vrb.
 */
static inline vui16_t
vec_vmrgoh (vui16_t vra, vui16_t vrb)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  const vui16_t permute =
      { 0x0100,0x1110, 0x0504,0x1514, 0x0908,0x1918, 0x0D0C,0x1D1C };
  return vec_perm (vrb, vra, (vui8_t)permute);
#else
  const vui16_t permute =
      { 0x0203,0x1213, 0x0607,0x1617, 0x0A0B,0x1A1B, 0x0E0F,0x1E1F };
  return vec_perm (vra, vrb, (vui8_t)permute);
#endif
}

#endif /* VEC_INT16_PPC_H_ */
