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

#include <pveclib/vec_int16_ppc.h>

/*!
 * \file  vec_int32_ppc.h
 * \brief Header package containing a collection of 128-bit SIMD
 * operations over 32-bit integer elements.
 *
 * Most of these operations are implemented in a single instruction
 * on newer (POWER8/POWER9) processors.
 * This header serves to fill in functional gaps for older
 * (POWER7, POWER8) processors and provides a in-line assembler
 * implementation for older compilers that do not
 * provide the build-ins.
 *
 * Most vector int (32-bit integer word) operations are implemented
 * with PowerISA VMX instructions either defined by the original VMX
 * (AKA Altivec) or added to later versions of the PowerISA.
 * Vector word-wise merge, shift, and splat operations
 * were added with VSX in PowerISA 2.06B (POWER7).
 * PowerISA 2.07B (POWER8) added several useful word wise operations
 * (multiply, merge even/odd, count leading zeros, population count)
 * not included in the original VMX.
 * PowerISA 3.0B (POWER9) adds several more (compare not equal,
 * count trailing zeros, extend sign, extract/insert, and parity).
 * Most of these intrinsic (compiler built-ins) operations are defined
 * in <altivec.h> and described in the compiler documentation.
 *
 * \note The compiler disables associated <altivec.h> built-ins if the
 * <B>mcpu</B> target does not enable the specific instruction.
 * For example if you compile with <B>-mcpu=power7</B>, vec_vclz and
 * vec_vclzw will not be defined.
 * Another example if you compile with <B>-mcpu=power8</B>, vec_revb
 * will not be defined.
 * This header provides the appropriate substitutions,
 * will generate the minimum code, appropriate for the target,
 * and produce correct results.
 *
 * \note Most ppc64le compilers will default to -mcpu=power8 if not
 * specified.
 *
 * The newly introduced vector operations imply some useful composite
 * operations. For example, we can make the vector multiply
 * even/odd/modulo word operations available for older compilers.
 * And provide implementations for older (POWER7 and earlier)
 * processors using the original VMX operations.
 *
 * This header covers operations that are either:
 *
 * - Implemented in hardware instructions for later
 * processors and useful to programmers, on slightly older processors,
 * even if the equivalent function requires more instructions.
 * Examples include the multiply even/odd/modulo word operations.
 * - Defined in the OpenPOWER ABI but <I>not</I> yet defined in
 * <altivec.h> provided by available compilers in common use.
 * Examples include Count Leading Zeros, Population Count and Byte
 * Reverse.
 * - Commonly used operations, not covered by the ABI or
 * <altivec.h>, and require multiple instructions or
 * are not obvious.
 * Examples include the shift immediate, merge algebraic high/low,
 * and multiply high operations.
 *
 * \section i32_recent_additions Recent Additions
 *
 * Added vec_vmaddeuw(), vec_vmaddouw(), vec_vmadd2euw(), and
 * vec_vmadd2ouw() as an optimization for
 * the vector multiply quadword implementations on POWER8.
 *
 * \section i32_endian_issues_0_0 Endian problems with word operations
 *
 * It would be useful to provide a vector multiply high word
 * (return the high order 32-bits of the 64-bit product) operation.
 * This can be used for multiplicative inverse (effectively integer
 * divide) operations.  Neither integer multiply high nor divide
 * are available as vector instructions.  However the multiply high word
 * operation can be composed from the existing multiply even/odd word
 * operations followed by the vector merge even word instruction.
 *
 * As a prerequisite we need to provide the merge even/odd word
 * operations for older compilers and an implementation for older
 * (POWER7) processors.  Fortunately vector merge operations are
 * just a special case of vector permute.  So the POWER7 (and earlier)
 * implementation can use vec_perm and appropriate selection vectors
 * to provide these merge operations.
 *
 * But this is complicated by <I>little-endian</I> (LE) support as
 * specified in the OpenPOWER ABI and as implemented in the compilers.
 * Little-endian changes the effective vector element numbering and
 * the location of even and odd elements.  This means that the vector
 * built-ins provided by altivec.h may not generate the instructions
 * you would expect.
 * \sa \ref mainpage_endian_issues_1_1
 *
 * The OpenPOWER ABI provides a helpful table of
 * <a href="http://openpowerfoundation.org/wp-content/uploads/resources/leabi/content/dbdoclet.50655244_90667.html"
 * >Endian Sensitive Operations</a>.
 * For vec_mergee (vmrgew) it specifies:
 * <BLOCKQUOTE>Swap inputs and use vmrgow, for LE.</BLOCKQUOTE>
 * Also for vec_mule (vmuleuw, vmulesw):
 * <BLOCKQUOTE>Replace with vmulouw and so on, for LE.</BLOCKQUOTE>
 * Also for vec_perm (vperm) it specifies:
 * <BLOCKQUOTE>For LE, Swap input arguments and complement the selection vector.</BLOCKQUOTE>
 * The above is just a sampling of a larger list of Endian Sensitive Operations.
 *
 * So the obvious coding for Vector Multiply High Word:
 * \code
vui32_t
test_mulhw (vui32_t vra, vui32_t vrb)
{
  return vec_mergee ((vui32_t)vec_mule (vra, vrb),
		     (vui32_t)vec_mulo (vra, vrb));
}
 * \endcode
 * Would produce the expected code and correct results when compiled
 * for BE:
 * \code
<test_mulhw>:
 	vmuleuw v0,v2,v3
 	vmuluuw v2,v2,v3
 	vmrgew  v2,v0,v2
 	blr
 * \endcode
 * But the following and wrong code for LE:
 * \code
<test_mulhw>:
 	vmulouw v0,v2,v3
 	vmuleuw v2,v2,v3
 	vmrgow  v2,v2,v0
 	blr
 * \endcode
 * The compiler swapped the multiplies even for odd and odd of even.
 * That is somewhat mitigated by swapping the input arguments in
 * the merge.  But changing the merge from even to odd actually
 * returns the low order 32-bits of the product. This is not the
 * correct result for multiply high.
 *
 * This header provides implementations of vector merge even/odd word
 * (vec_mrgew() and vec_mrgow()) that support older compilers and
 * older (POWER7) processor.
 * Similarly for the multiply Even/odd unsigned/signed word
 * instructions (vec_mulesw(), vec_mulosw(), vec_muleuw() and
 * vec_mulouw()). These implementations include the mandated LE
 * transforms.
 *
 * \subsection i32_example_0_0_0 Vector Merge Algebraic High Word example
 * This header also provides the higher level operations Vector Merge
 * Algebraic High/low Word (vec_mrgahw() and vec_mrgalw()).
 * These implementations generate the correct merge even/odd word
 * instruction for the operation independent of endian.
 * \note The parameters are vector unsigned long (vui64_t) to match
 * results from vec_muleuw() and vec_mulouw().
 *
 * \code
static inline vui32_t
vec_mrgahw (vui64_t vra, vui64_t vrb)
{
  vui32_t res;
#ifdef _ARCH_PWR8
#ifdef vec_vmrgew // Use altivec.h builtins
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  // really want vmrgew here! So do the opposite.
  res = vec_vmrgow ((vui32_t)vrb, (vui32_t)vra);
#else
  res = vec_vmrgew ((vui32_t)vra, (vui32_t)vrb);
#endif
#else // Generate vmrgew directly in assembler
  __asm__(
      "vmrgew %0,%1,%2;\n"
      : "=v" (res)
      : "v" (vra),
      "v" (vrb)
      : );
#endif
#else // POWER7 and earlier, Assume BE only
  const vui32_t vconstp =
      CONST_VINT32_W(0x00010203,  0x10111213, 0x08090a0b,  0x18191a1b);
  res = (vui32_t) vec_perm ((vui8_t) vra, (vui8_t) vrb, (vui8_t) vconstp);
#endif
  return (res);
}
 * \endcode
 * The implementation is a bit complicated so that is can nullify the
 * unwanted LE transformation of vec_vmrgew(), in addition to handling
 * older and compilers and processors.
 *
 * \subsection i32_example_0_0_1 Vector Multiply High Unsigned Word example
 * Now we can implement Vector Multiply High Unsigned Word (vec_mulhuw()):
 * \code
static inline vui32_t
vec_mulhuw (vui32_t vra, vui32_t vrb)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return vec_mrgahw (vec_mulouw (vra, vrb), vec_muleuw (vra, vrb));
#else
  return vec_mrgahw (vec_muleuw (vra, vrb), vec_mulouw (vra, vrb));
#endif
}
 * \endcode
 * Again the implementation is more complicated than expected as we
 * still have to nullify the LE transformation associated with
 * multiply even/odd.
 *
 * The good news is all this complexity is contained within pveclib
 * and the generated code is still just 3 instructions.
 * \code
 	vmulouw v0,v2,v3
 	vmuleuw v2,v2,v3
 	vmrgew  v2,v2,v0
 * \endcode
 *
 * \section int32_examples_0_1 Vector Word Examples
 * Suppose we have a requirement to convert an array of 32-bit
 * time-interval values that need to convert to timespec format.
 * For simplicity we will also assume that the array is nicely
 * (Quadword) aligned and an integer multiple of 4 words.
 *
 * The PowerISA provides a 64-bit TimeBase register that clocks at a
 * constant 512MHz. The TimeBase can be read directly as either the
 * full 64-bit value or as 32-bit upper and lower halves.
 * For this example we assume that the lower 32-bits of the TimeBase
 * is sufficient to compute intervals (~8.38 seconds).
 * TimeBase values of adjacent events are subtracted to generate the
 * intervals stored in the array.
 *
 * The timespec format it a struct of unsigned int fields for seconds
 * and microseconds.  So the task is to convert the 512MHz TimeBase
 * intervals to microseconds and then split the integer seconds and
 * microseconds for the timespec.
 *
 * First the TimeBase to microseconds conversion is simply
 * (1000000 / 512000000) which reduces to (1 / 512) or divide by 512.
 * The vector unit does not provide integer divide but luckily, 512 is
 * a power of 2 and we can shift right.
 * If we don't care for the niceties of rounding we can simply shift
 * right 9 bits:
 * \code
      tb_usec = vec_srwi (*tb++, 9);
 * \endcode
 * But if we decide that rounding is important we can leverage the
 * Vector Average Unsigned Word (vavguw) instruction.
 * Here we need to add 256 (512 / 2 = 256) to the timeBase interval
 * before we shift right.
 *
 * But we need to reverse engineer the vavguw operation to get the
 * results we want.  For each word, vavguw computes the sum of A and B
 * plus 1, then shifts the 33-bit sum right 1 bit.
 * We can effectively round by passing the rounding factor as the B
 * operand to the vec_avg() built-in.
 * But we get a +1 and 1 bit right shift for free. So in this case the
 * rounding constant is 256-1 = 255. And we only need to shift an
 * additional 8 bits to complete the conversion:
 * \code
  const vui32_t rnd_512 =
    { (256-1), (256-1), (256-1), (256-1) };
  // Convert 512MHz timebase to microseconds with rounding.
  tmp = vec_avg (*tb++, rnd_512);
  tb_usec = vec_srwi (tmp, 8);
 * \endcode
 * \note vec_avg() is an existing altivec.h generic built-in.
 *
 * Next we need to separate TimeBase microseconds into the integer
 * seconds and microseconds. Normally scalar codes would use integer
 * divide/modulo by 1000000. Did I mention that the PowerISA vector
 * unit does not have a integer divide operation?
 *
 * Instead we can use the multiplicative inverse which is a
 * scaled fixed point fraction calculated from the original divisor.
 * This works nicely if the fixed radix point is just before
 * the 32-bit fraction and we have a multiply high (vec_mulhuw())
 * operation. Multiplying a 32-bit unsigned integer by a 32-bit
 * unsigned fraction generates a 64-bit product with 32-bits above
 * (integer) and below (fraction) the radix point.
 * The high 32-bits of the product is the integer quotient.
 *
 * It turns out that generating the multiplicative inverse can be
 * tricky.  To produce correct results over the full analysis,
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
 * For the divisor 1000000 this is { 1125899907, 0 , 18 }:
 * - the multiplier is 1125899907.
 * - no corrective add of the dividend is required.
 * - the final shift is 18-bits right.
 *
 * \code
  const vui32_t mul_invs_1m =
    { 1125899907, 1125899907, 1125899907, 1125899907 };
  const int shift_1m = 18;

  tmp = vec_mulhuw (tb_usec, mul_invs_1m);
  seconds = vec_srwi (tmp, shift_1m);
 * \endcode
 * Now we need to compute the remainder to get microseconds.
 * \code
  const vui32_t usec_sec =
    { 1000000, 1000000, 1000000, 1000000 };

  tmp = vec_muluwm (seconds, usec_sec);
  useconds = vec_sub (tb_usec, tmp);
 * \endcode
 *
 * Finally we need to merge the vectors of seconds and useconds into
 * vectors of timespec.
 * \code
  timespec1 = vec_mergeh (seconds, useconds);
  timespec2 = vec_mergel (seconds, useconds);
 * \endcode
 * \note vec_sub(), vec_mergeh(), and vec_mergel() are an existing
 * altivec.h generic built-ins.
 *
 * \subsection i32_example_0_1_0 Vectorized TimeBase conversion example
 * Here is the complete vectorized TimeBase to timespec conversion example:
 * \code
void
example_convert_timebase (vui32_t *tb, vui32_t *timespec, int n)
{
  const vui32_t rnd_512 =
    { (256-1), (256-1), (256-1), (256-1) };
  // Magic numbers for multiplicative inverse to divide by 1,000,000
  // are 1125899907 and shift right 18 bits.
  const vui32_t mul_invs_1m =
    { 1125899907, 1125899907, 1125899907, 1125899907 };
  const int shift_1m = 18;
  // Need const for microseconds/second to extract remainder.
  const vui32_t usec_sec =
    { 1000000, 1000000, 1000000, 1000000 };
  vui32_t tmp, tb_usec, seconds, useconds;
  vui32_t timespec1, timespec2;
  int i;

  for (i = 0; i < n; i++)
    {
      // Convert 512MHz timebase to microseconds with rounding.
      tmp = vec_avg (*tb++, rnd_512);
      tb_usec = vec_srwi (tmp, 8);
      // extract integer seconds from tb_usec.
      tmp = vec_mulhuw (tb_usec, mul_invs_1m);
      seconds = vec_srwi (tmp, shift_1m);
      // Extract remainder microseconds.
      tmp = vec_muluwm (seconds, usec_sec);
      useconds = vec_sub (tb_usec, tmp);
      // Use merge high/low to interleave seconds and useconds in timespec.
      timespec1 = vec_mergeh (seconds, useconds);
      timespec2 = vec_mergel (seconds, useconds);
      // Store timespec.
      *timespec++ = timespec1;
      *timespec++ = timespec2;
    }
}
 * \endcode
 *
 * \section int32_perf_0_0 Performance data.
 * High level performance estimates are provided as an aid to function
 * selection when evaluating algorithms. For background on how
 * <I>Latency</I> and <I>Throughput</I> are derived see:
 * \ref perf_data
 */

///@cond INTERNAL
#ifdef _ARCH_PWR8
/*
 * Vector Shift Left Doubleword was added to PowerISA 2.07 (PWR8).
 * Operations vec_vsld and vec_sldi are define in vec_int64_ppc.h
 * but using those here would create a circular dependency.
 * So need the equivalent of the altivec.h specific vec_vsld.
 * Currently GCC defines both vec_vsld and vec_sl for type long long.
 * But older GCC versions may not and are more likely to support only
 * vec_vsld. Clang seems to only support the generic vec_sl for the
 * long long type and does not define the macro vec_vsld.
 *
 * The following allows vec_int32_ppc.h to use __pvec_vsld as a work
 * around for clang and possible future versions of GCC that drop
 * support for altivec specific built-ins.
 */
#ifdef vec_vsld
#define __pvec_vsld vec_vsld
#else
#define __pvec_vsld vec_sl
#endif
#endif

static inline vui64_t vec_muleuw (vui32_t a, vui32_t b);
static inline vui64_t vec_mulouw (vui32_t a, vui32_t b);
#ifndef vec_popcntw
static inline vui32_t vec_popcntw (vui32_t vra);
#else
/* Work around for GCC PR85830.  */
#undef vec_popcntw
#define vec_popcntw __builtin_vec_vpopcntw
#endif
static inline vi32_t vec_srawi (vi32_t vra, const unsigned int shb);
static inline vui64_t
vec_vlxsiwzx (const signed long long ra, const unsigned int *rb);
static inline vi64_t
vec_vlxsiwax (const signed long long ra, const signed int *rb);
static inline vui64_t vec_vmuleuw (vui32_t a, vui32_t b);
static inline vui64_t vec_vmulouw (vui32_t a, vui32_t b);
static inline void
vec_vsstwso (vui64_t xs, unsigned int *array,
	      const long long offset0, const long long offset1);
static inline void
vec_vstxsiwx (vui32_t xs, const signed long long ra, unsigned int *rb);
///@endcond

/** \brief Vector Absolute Difference Unsigned Word.
 *
 *  Compute the absolute difference for each word.
 *  For each unsigned word, subtract VRB[i] from VRA[i] and return
 *  the absolute value of the difference.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   4   | 1/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param vra vector of 4 x unsigned words
 *  @param vrb vector of 4 x unsigned words
 *  @return vector of the absolute differences.
 */
static inline vui32_t
vec_absduw (vui32_t vra, vui32_t vrb)
{
   vui32_t result;
#ifdef _ARCH_PWR9
#ifdef vec_absdw
  result = vec_absdw (vra, vrb);
#else
  __asm__(
      "vabsduw %0,%1,%2;"
      : "=v" (result)
      : "v" (vra), "v" (vrb)
      : );
#endif
#else
  vui32_t vmin, vmax;

  vmin = vec_min (vra, vrb);
  vmax = vec_max (vra, vrb);
  result = vec_sub (vmax, vmin);
#endif
  return (result);
}

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
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
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
#if defined (vec_vclzw)
  r = vec_vclzw (vra);
#elif defined (__clang__)
  r = vec_cntlz (vra);
#else
  __asm__(
      "vclzw %0,%1;"
      : "=v" (r)
      : "v" (vra)
      : );
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

/** \brief Vector Count Trailing Zeros word.
 *
 *  Count the number of trailing '0' bits (0-32) within each word
 *  element of a 128-bit vector.
 *
 *  For POWER9 (PowerISA 3.0B) or later use the Vector Count Trailing
 *  Zeros Word instruction <B>vctzw</B>. Otherwise use a sequence of
 *  pre ISA 3.0 VMX instructions.
 *  SIMDized count Trailing zeros inspired by:
 *
 *  Warren, Henry S. Jr and <I>Hacker's Delight</I>, 2nd Edition,
 *  Addison Wesley, 2013. Chapter 5 Counting Bits, Section 5-4.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  6-8  | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as 4 x 32-bit integer (words)
 *  elements.
 *  @return 128-bit vector with the Trailng Zeros count for each word
 *  element.
 */
static inline vui32_t
vec_ctzw (vui32_t vra)
{
  vui32_t r;
#ifdef _ARCH_PWR9
#if defined (vec_cnttz) || defined (__clang__)
  r = vec_cnttz (vra);
#else
  __asm__(
      "vctzw %0,%1;"
      : "=v" (r)
      : "v" (vra)
      : );
#endif
#else
// For _ARCH_PWR8 and earlier. Generate 1's for the trailing zeros
// and 0's otherwise. Then count (popcnt) the 1's. _ARCH_PWR8 uses
// the hardware vpopcntw instruction. _ARCH_PWR7 and earlier use the
// PVECLIB vec_popcntw implementation which runs ~20-28 instructions.
  const vui32_t ones = { 1, 1, 1, 1 };
  vui32_t tzmask;
  // tzmask = (!vra & (vra - 1))
  tzmask = vec_andc (vec_sub (vra, ones), vra);
  // return = vec_popcnt (!vra & (vra - 1))
  r = vec_popcntw (tzmask);
#endif
  return ((vui32_t) r);
}

/** \brief Vector Merge Algebraic High Words.
 *
 * Merge only the high words from 4 x Algebraic doublewords
 * across vectors vra and vrb. This effectively the Vector Merge
 * Even Word operation that is not modified for endian.
 *
 * For example merge the high 32-bits from 4 x 64-bit products
 * as generated by vec_muleuw/vec_mulouw. This result is effectively
 * a vector multiply high unsigned word.
 *
 * \note This implementation is NOT endian sensitive and the function
 * is stable across BE/LE implementations.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   2   | 2/cycle  |
 *
 * @param vra 128-bit vector unsigned long.
 * @param vrb 128-bit vector unsigned long.
 * @return A vector merge from only the high words of the 4 x
 * Algebraic doublewords across vra and vrb.
 */
static inline vui32_t
vec_mrgahw (vui64_t vra, vui64_t vrb)
{
  vui32_t res;
#ifdef _ARCH_PWR8
#ifdef vec_vmrgew
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  res = vec_vmrgow ((vui32_t)vrb, (vui32_t)vra);
#else
  res = vec_vmrgew ((vui32_t)vra, (vui32_t)vrb);
#endif
#else
  __asm__(
      "vmrgew %0,%1,%2;\n"
      : "=v" (res)
      : "v" (vra),
      "v" (vrb)
      : );
#endif
#else
  const vui32_t vconstp =
      CONST_VINT32_W(0x00010203,  0x10111213, 0x08090a0b,  0x18191a1b);
  res = (vui32_t) vec_perm ((vui8_t) vra, (vui8_t) vrb, (vui8_t) vconstp);
#endif
  return (res);
}

/** \brief Vector merge Algebraic low words.
 *
 * Merge the arithmetic low words 4 x Algebraic doublewords
 * across vectors vra and vrb. This is effectively the Vector Merge
 * Odd Word operation that is not modified for endian.
 *
 * For example merge the low 32-bits from 4 x 64-bit products
 * as generated by vec_muleuw/vec_mulouw. This result is effectively a
 * vector multiply low unsigned word (multiply unsigned word modulo).
 *
 * \note This implementation is NOT endian sensitive and the function
 * is stable across BE/LE implementations.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |   2   | 2/cycle  |
 * |power9   |   2   | 2/cycle  |
 *
 * @param vra 128-bit vector unsigned long.
 * @param vrb 128-bit vector unsigned long.
 * @return A vector merge from only the low words of the 4 x
 * Algebraic doublewords across vra and vrb.
 */
static inline vui32_t
vec_mrgalw (vui64_t vra, vui64_t vrb)
{
  vui32_t res;
#ifdef _ARCH_PWR8
#ifdef vec_vmrgow
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  res = vec_vmrgew ((vui32_t)vrb, (vui32_t)vra);
#else
  res = vec_vmrgow ((vui32_t)vra, (vui32_t)vrb);
#endif
#else
  __asm__(
      "vmrgow %0,%1,%2;\n"
      : "=v" (res)
      : "v" (vra),
      "v" (vrb)
      : );
#endif
#else
  const vui32_t vconstp =
      CONST_VINT32_W(0x04050607,  0x14151617, 0x0c0d0e0f,  0x1c1d1e1f);
  res = (vui32_t) vec_perm ((vui8_t) vra, (vui8_t) vrb, (vui8_t) vconstp);
#endif
  return (res);
}

/** \brief Vector Merge Even Words.
 *
 * Merge the even word elements from the concatenation of 2 x vectors
 * (vra and vrb).
 * - res[0] = vra[0];
 * - res[1] = vrb[0];
 * - res[2] = vra[2];
 * - res[3] = vrb[2];
 *
 * The element numbering changes between big and little-endian
 * environements.
 * So the compiler and this implementation adjusts the generated code
 * to reflect this.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   2   | 2/cycle  |
 *
 * @param vra 128-bit vector unsigned int.
 * @param vrb 128-bit vector unsigned int.
 * @return A vector merge from only the even words of vra and vrb.
 */
static inline vui32_t
vec_mrgew (vui32_t vra, vui32_t vrb)
{
  vui32_t res;
#ifdef _ARCH_PWR8
#ifdef vec_vmrgew
  res = vec_vmrgew (vra, vrb);
#else
  __asm__(
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
      "vmrgow %0,%2,%1;\n"
#else
      "vmrgew %0,%1,%2;\n"
#endif
      : "=v" (res)
      : "v" (vra),
      "v" (vrb)
      : );
#endif
#else
  const vui32_t vconstp =
      CONST_VINT32_W(0x00010203, 0x10111213, 0x08090a0b, 0x18191a1b);
  res = (vui32_t) vec_perm ((vui8_t) vra, (vui8_t) vrb, (vui8_t) vconstp);
#endif
  return (res);
}

/** \brief Vector Merge Odd Words.
 *
 * Merge the odd word elements from the concatenation of 2 x vectors
 * (vra and vrb).
 * - res[0] = vra[1];
 * - res[1] = vrb[1];
 * - res[2] = vra[3];
 * - res[3] = vrb[3];
 *
 * The element numbering changes between big and little-endian
 * environements.
 * So the compiler and this implementation adjusts the generated code
 * to reflect this.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   2   | 2/cycle  |
 *
 * @param vra 128-bit vector unsigned int.
 * @param vrb 128-bit vector unsigned int.
 * @return A vector merge from only the even words of vra and vrb.
 */
static inline vui32_t
vec_mrgow (vui32_t vra, vui32_t vrb)
{
  vui32_t res;
#ifdef _ARCH_PWR8
#ifdef vec_vmrgew
  res = vec_vmrgow (vra, vrb);
#else
  __asm__(
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
      "vmrgew %0,%2,%1;\n"
#else
      "vmrgow %0,%1,%2;\n"
#endif
      : "=v" (res)
      : "v" (vra),
      "v" (vrb)
      : );
#endif
#else
  const vui32_t vconstp =
      CONST_VINT32_W(0x04050607, 0x14151617, 0x0c0d0e0f, 0x1c1d1e1f);
  res = (vui32_t) vec_perm ((vui8_t) vra, (vui8_t) vrb, (vui8_t) vconstp);
#endif
  return (res);
}

/** \brief Vector multiply even signed words.
 *
 * Multiple the even words of two vector signed int values and return
 * the signed long product of the even words.
 *
 * For POWER8 and later we can use the vmulesw instruction.
 * But for POWER7 and earlier we have to construct word multiplies
 * from halfword multiplies. See vec_muleuw().
 *
 * Here we start with a unsigned vec_muleuw product, then correct the
 * high 32-bits of the product to signed. Based on:
 * Warren, Henry S. Jr and <I>Hacker's Delight</I>, 2nd Edition,
 * Addison Wesley, 2013. Chapter 8 Multiplication, Section 8-3
 * High-Order Product Signed from/to Unsigned.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |   7   | 2/cycle  |
 * |power9   |   7   | 2/cycle  |
 *
 * @param a 128-bit vector signed int.
 * @param b 128-bit vector signed int.
 * @return vector signed long product of the even words of a and b.
 */
static inline vi64_t
vec_mulesw (vi32_t a, vi32_t b)
{
  vi64_t res;
#ifdef _ARCH_PWR8
  // The vector vmulosw/vmulesw instructions introduced in PRW8
#if defined __GNUC__ && (__GNUC__ > 7)
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
#else
  // must be PWR7 or older
  vui32_t uia, uib;
  vui32_t amask, bmask, t1, t2, r;
  vui64_t ui_prod;
  const vui32_t zero= { 0,0,0,0};

  uia = (vui32_t) a;
  uib = (vui32_t) b;
  // Generate 32-bit masks from the sign of each input word.
  amask = (vui32_t) vec_srawi (a, 31);
  bmask = (vui32_t) vec_srawi (b, 31);
  // Extend the even masks to the right with zeros to form two 64-bit
  // masks. We need the trailing zeros as the low 32-bits of the
  // product are correct as-is and should not change.
  amask = vec_mrgew (amask, zero);
  bmask = vec_mrgew (bmask, zero);
  // Compute the doubleword even unsigned word product
  ui_prod = vec_muleuw (uia, uib);

  // Generate t1 = amask & b and t2 = bmask & a
  t1 = vec_and (amask, uib);
  t2 = vec_and (bmask, uia);
  // Apply the correction res = ui_prod - t1 - t2
  r = vec_sub ((vui32_t) ui_prod, t1);
  res = (vi64_t) vec_sub (r, t2);
#endif
  return (res);
}

/** \brief Vector multiply odd signed words.
 *
 * Multiple the odd words of two vector signed int values and return
 * the signed long product of the odd words.
 *
 * For POWER8 and later we can use the vmulosw instruction.
 * But for POWER7 and earlier we have to construct word multiplies
 * from halfword multiplies. See vec_mulouw().
 *
 * Here we start with a unsigned vec_mulouw product, then correct the
 * high-order 32-bits of the product to signed. Based on:
 * Warren, Henry S. Jr and <I>Hacker's Delight</I>, 2nd Edition,
 * Addison Wesley, 2013. Chapter 8 Multiplication, Section 8-3
 * High-Order Product Signed from/to Unsigned.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |   7   | 2/cycle  |
 * |power9   |   7   | 2/cycle  |
 *
 * @param a 128-bit vector signed int.
 * @param b 128-bit vector signed int.
 * @return vector signed long product of the odd words of a and b.
 */
static inline vi64_t
vec_mulosw (vi32_t a, vi32_t b)
{
  vi64_t res;
#ifdef _ARCH_PWR8
  // The vector vmulosw/vmulesw instructions introduced in PRW8
#if defined __GNUC__ && (__GNUC__ > 7) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
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
#else
  // must be PWR7 or older
  vui32_t uia, uib;
  vui32_t amask, bmask, t1, t2, r;
  vui64_t ui_prod;
  const vui32_t zero= { 0,0,0,0};

  // duplicate odd words to even
  uia = (vui32_t) a;
  uib = (vui32_t) b;
  uia = vec_mrgow (uia, uia);
  uib = vec_mrgow (uib, uib);
  // Generate 32-bit masks from the sign of each input word.
  amask = (vui32_t) vec_srawi ((vi32_t) uia, 31);
  bmask = (vui32_t) vec_srawi ((vi32_t) uib, 31);
  // Shift the odd masks to the left 32 and extend to the right with
  // zeros to form two 64-bit masks. We need the trailing zeros as the
  // low 32-bits of the product are correct as-is.
  amask = vec_mrgow (amask, zero);
  bmask = vec_mrgow (bmask, zero);
  // Compute the doubleword odd unsigned word product
  ui_prod = vec_mulouw (uia, uib);

  // Generate t1 = amask & b and t2 = bmask & a
  t1 = vec_and (amask, uib);
  t2 = vec_and (bmask, uia);
  // Apply the correction res = ui_prod - t1 - t2
  r = vec_sub ((vui32_t) ui_prod, t1);
  res = (vi64_t) vec_sub (r, t2);
#endif
  return (res);
}

/** \brief Vector multiply even unsigned words.
 *
 * Multiple the even words of two vector unsigned int values and return
 * the unsigned long product of the even words.
 *
 * For POWER8 and later we can use the vmuleuw instruction.
 * But for POWER7 and earlier we have to construct word multiplies
 * from two halfword multiplies (vmuleuh and vmulouh). Then sum the
 * partial products for the final doubleword results. This is
 * complicated by the fact that vector add doubleword is not available
 * for POWER7. So we need to construct the doubleword add from
 * Vector Add Unsigned Word Modulo (vadduwm) and
 * Vector Add and Write Carry-Out Unsigned Word (vaddcuw) with
 * shift double quadword to reposition the low word carry and
 * a final vadduwm to complete the carry propagation for the
 * doubleword add.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |   7   | 2/cycle  |
 * |power9   |   7   | 2/cycle  |
 *
 * @param a 128-bit vector unsigned int.
 * @param b 128-bit vector unsigned int.
 * @return vector unsigned long product of the even words of a and b.
 */
static inline vui64_t
vec_muleuw (vui32_t a, vui32_t b)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return vec_vmulouw (a, b);
#else
  return vec_vmuleuw (a, b);
#endif
}

/** \brief Vector multiply odd unsigned words.
 *
 * Multiple the odd words of two vector unsigned int values and return
 * the unsigned long product of the odd words.
 *
 * For POWER8 and later we can use the vmulouw instruction.
 * But for POWER7 and earlier we have to construct word multiplies
 * from two halfword multiplies (vmuleuh and vmulouh). Then sum the
 * partial products for the final doubleword results. This is
 * complicated by the fact that vector add doubleword is not available
 * for POWER7. So we need to construct the doubleword add from
 * Vector Add Unsigned Word Modulo (vadduwm) and
 * Vector Add and Write Carry-Out Unsigned Word (vaddcuw) with
 * shift double quadword to reposition the low word carry and
 * a final vadduwm to complete the carry propagation for the doubleword
 * add.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |   7   | 2/cycle  |
 * |power9   |   7   | 2/cycle  |
 *
 * @param a 128-bit vector unsigned int.
 * @param b 128-bit vector unsigned int.
 * @return vector unsigned long product of the odd words of a and b.
 */
static inline vui64_t
vec_mulouw (vui32_t a, vui32_t b)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return vec_vmuleuw (a, b);
#else
  return vec_vmulouw (a, b);
#endif
}

/** \brief Vector Multiply High Signed Word.
 *
 *  Multiple the corresponding word elements of two vector signed int
 *  values and return the high order 32-bits, for each
 *  64-bit product element.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   9   | 1/cycle  |
 *  |power9   |   9   | 1/cycle  |
 *
 *  @param vra 128-bit vector signed int.
 *  @param vrb 128-bit vector signed int.
 *  @return vector of the high order 32-bits of the product of the
 *  word elements from vra and vrb.
 */
static inline vi32_t
vec_mulhsw (vi32_t vra, vi32_t vrb)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return (vi32_t) vec_mrgahw ((vui64_t) vec_mulosw (vra, vrb),
			      (vui64_t) vec_mulesw (vra, vrb));
#else
  return (vi32_t) vec_mrgahw ((vui64_t) vec_mulesw (vra, vrb),
			      (vui64_t) vec_mulosw (vra, vrb));
#endif
}

/** \brief Vector Multiply High Unsigned Word.
 *
 *  Multiple the corresponding word elements of two vector unsigned int
 *  values and return the high order 32-bits, from each
 *  64-bit product.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   9   | 1/cycle  |
 *  |power9   |   9   | 1/cycle  |
 *
 *  \note This operation can be used to effectively perform a divide
 *  by multiplying by the scaled multiplicative inverse (reciprocal).
 *
 *  Warren, Henry S. Jr and <I>Hacker's Delight</I>, 2nd Edition,
 *  Addison Wesley, 2013. Chapter 10, Integer Division by Constants.
 *
 *  @param vra 128-bit vector unsigned int.
 *  @param vrb 128-bit vector unsigned int.
 *  @return vector of the high order 32-bits of the signed product
 *  of the word elements from vra and vrb.
 */
static inline vui32_t
vec_mulhuw (vui32_t vra, vui32_t vrb)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return vec_mrgahw (vec_mulouw (vra, vrb), vec_muleuw (vra, vrb));
#else
  return vec_mrgahw (vec_muleuw (vra, vrb), vec_mulouw (vra, vrb));
#endif
}

/** \brief Vector Multiply Unsigned Word Modulo.
 *
 *  Multiple the corresponding word elements of two vector unsigned int
 *  values and return the low order 32-bits of the 64-bit product for
 *  each element.
 *
 *  \note vec_muluwm can be used for unsigned or signed integers.
 *  It is the vector equivalent of Multiply Low Word.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   7   | 2/cycle  |
 *  |power9   |   7   | 2/cycle  |
 *
 *  @param a 128-bit vector signed int.
 *  @param b 128-bit vector signed int.
 *  @return vector of the low order 32-bits of the unsigned product
 *  of the word elements from vra and vrb.
 */
static inline vui32_t
vec_muluwm (vui32_t a, vui32_t b)
{
#if defined __GNUC__ && (__GNUC__ > 7)
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
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
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
#if defined (vec_vpopcntw)
  r = vec_vpopcntw (vra);
#elif defined (__clang__)
  r = vec_popcnt (vra);
#else
  __asm__(
      "vpopcntw %0,%1;"
      : "=v" (r)
      : "v" (vra)
      : );
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
 *  For each word of the input vector, reverse the order of
 *  bytes / octets within the word.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-11  | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param vra a 128-bit vector unsigned int.
 *  @return a 128-bit vector with the bytes of each word
 *  reversed.
 */
static inline vui32_t
vec_revbw (vui32_t vra)
{
  vui32_t result;

#ifdef _ARCH_PWR9
#if defined (vec_revb) || defined (__clang__)
  result = vec_revb (vra);
#else
  __asm__(
      "xxbrw %x0,%x1;"
      : "=wa" (result)
      : "wa" (vra)
      : );
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

/*! \brief Vector Set Bool from Signed Word.
 *
 *  For each word, propagate the sign bit to all 32-bits of that
 *  word. The result is vector bool int reflecting the sign
 *  bit of each 32-bit word.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-4   | 2/cycle  |
 *  |power9   | 2-5   | 2/cycle  |
 *
 *  @param vra Vector signed int.
 *  @return vector bool int reflecting the sign bits of each
 *  word.
 */

static inline vb32_t
vec_setb_sw (vi32_t vra)
{
  vb32_t result;

#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  __asm__(
      "vexpandwm %0,%1"
      : "=v" (result)
      : "v" (vra)
      : );
#else
  // Compare signed word less than zero
  const vi32_t zero = {0, 0, 0, 0};
  result = vec_cmplt (vra, zero);
#endif
  return result;
}

/** \brief Vector Shift left Word Immediate.
 *
 *  Shift left each word element [0-3], 0-31 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-31.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 31 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-11  | 2/cycle  |
 *  |power9   | 5-11  | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned int.
 *  @param shb shift amount in the range 0-31.
 *  @return 128-bit vector unsigned int, shifted left shb bits.
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
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-11  | 2/cycle  |
 *  |power9   | 5-11  | 2/cycle  |
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
 *  Shift right each word element [0-3], 0-31 bits,
 *  as specified by an immediate value.
 *  The shift amount is a const unsigned int in the range 0-31.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 31 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-11  | 2/cycle  |
 *  |power9   | 5-11  | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as a vector unsigned char.
 *  @param shb shift amount in the range 0-31.
 *  @return 128-bit vector unsigned int, shifted right shb bits.
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

/** \brief Vector Gather-Load 4 Words from scalar Offsets.
 *
 *  For each scalar offset[0,1,2,3], load the word
 *  from the effective address formed by
 *  *(char*)array+offset[0-3]. Merge resulting word
 *  elements [0,1,2,3] and return the resulting vector.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  10   | 1/cycle  |
 *  |power9   |  11   | 1/cycle  |
 *
 *  @param array Pointer to array of integer words.
 *  @param offset0 Scalar (64-bit) byte offset from &array.
 *  @param offset1 Scalar (64-bit) byte offset from &array.
 *  @param offset2 Scalar (64-bit) byte offset from &array.
 *  @param offset3 Scalar (64-bit) byte offset from &array.
 *  @return vector word containing word elements [0-3] loaded from
 *  *(char*)array+offset[0-3].
 */
static inline vui32_t
vec_vgl4wso (unsigned int *array, const long long offset0,
	     const long long offset1, const long long offset2,
	     const long long offset3)
{
  vui32_t result;

#ifdef _ARCH_PWR8
  vui64_t re0, re1, re2, re3;
  re0 = vec_vlxsiwzx (offset0, array);
  re1 = vec_vlxsiwzx (offset1, array);
  re2 = vec_vlxsiwzx (offset2, array);
  re3 = vec_vlxsiwzx (offset3, array);
  /* Need to handle endian as the vec_vlxsiwzx result is always left
   * justified in VR, while element [0] may be left or right. */
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  /* Can't use vec_mergeo here as GCC 7 (AT11) and earlier don't
   * support doubleword vec_merge. */
  re0 = vec_xxpermdi (re0, re2, 3);
  re1 = vec_xxpermdi (re1, re3, 3);
  result = vec_mergee ((vui32_t) re0, (vui32_t) re1);
#else
  re0 = vec_xxpermdi (re0, re2, 0);
  re1 = vec_xxpermdi (re1, re3, 0);
  result = vec_mergeo ((vui32_t) re0, (vui32_t) re1);
#endif
#else //  _ARCH_PWR7
  vui32_t xte0, xte1, xte2, xte3;
  vui8_t perm0, perm1, perm2, perm3;

  perm0 = vec_lvsl (offset0, array);
  xte0 = vec_lde (offset0, array);
  xte0 = vec_perm (xte0, xte0, perm0);

  perm1 = vec_lvsl (offset1, array);
  xte1 = vec_lde (offset1, array);
  xte1 = vec_perm (xte1, xte1, perm1);

  perm2 = vec_lvsl (offset2, array);
  xte2 = vec_lde (offset2, array);
  xte2 = vec_perm (xte2, xte2, perm2);

  perm3 = vec_lvsl (offset3, array);
  xte3 = vec_lde (offset3, array);
  xte3 = vec_perm (xte3, xte3, perm3);

  xte0 = vec_mergeh (xte0, xte2);
  xte1 = vec_mergeh (xte1, xte3);
  result = vec_mergeh (xte0, xte1);
#endif
  return result;
}

/** \brief Vector Gather-Load 4 Words from Vector Word Offsets.
 *
 *  For each signed word element [i] of vra, load the word
 *  element at *(char*)array+vra[i]. Merge those word elements
 *  and return the resulting vector.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  14   | 1/cycle  |
 *  |power9   |  15   | 1/cycle  |
 *
 *  @param array Pointer to array of integer words.
 *  @param vra Vector of signed word (32-bit) byte offsets from &array.
 *  @return vector word containing word elements [0-3], each loaded
 *  from *(char*)array+vra[0-3].
 */
static inline
vui32_t
vec_vgl4wwo (unsigned int *array, vi32_t vra)
{
  vui32_t r;

#ifdef _ARCH_PWR8
#if 1
  vi64_t off01, off23;

  off01 = vec_vupkhsw (vra);
  off23 = vec_vupklsw (vra);

  r = vec_vgl4wso (array, off01[0], off01[1], off23[0], off23[1]);
#else
  r = vec_vgl4wso (array, vra[0], vra[1], vra[2], vra[3]);
#endif
#else
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);
  signed int off0, off1, off2, off3;

  off0 = scalar_extract_uint64_from_high_uint128(gprp) >> 32;
  off1 = (int) scalar_extract_uint64_from_high_uint128(gprp);
  off2 = scalar_extract_uint64_from_low_uint128(gprp) >> 32;
  off3 = (int) scalar_extract_uint64_from_low_uint128(gprp);

  r = vec_vgl4wso (array, off0, off1, off2, off3);
#endif
  return  r;
}

/** \brief Vector Gather-Load 4 Words from Vector Word Scaled Indexes.
 *
 *  For each signed word element [i] of vra, load the word
 *  element at array[vra[i] << scale]. Merge those word elements
 *  and return the resulting vector.
 *
 *  \note Signed word indexes are expanded (unpacked) to doublewords
 *  before shifting left 2+scale bits. This converts each index to an
 *  64-bit offset for effective address calculation.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 16-25 | 1/cycle  |
 *  |power9   | 18-27 | 1/cycle  |
 *
 *  @param array Pointer to array of integer words.
 *  @param vra Vector of signed word (32-bit) indexes.
 *  @param scale 8-bit integer. Indexes are multiplying by
 *  2<sup>scale</sup>.
 *  @return vector word containing word elements [0-3] each loaded from
 *  array[vra[0-3] << scale].
 */
static inline
vui32_t
vec_vgl4wwsx (unsigned int *array, vi32_t vra,
		     const unsigned char scale)
{
  vui32_t r;

#ifdef _ARCH_PWR8
  vi64_t off01, off23;
  vi64_t lshift = vec_splats ((long long) (2+ scale));

  off01 = vec_vupkhsw (vra);
  off23 = vec_vupklsw (vra);

  off01 = (vi64_t) __pvec_vsld (off01, (vui64_t) lshift);
  off23 = (vi64_t) __pvec_vsld (off23, (vui64_t) lshift);

  r = vec_vgl4wso (array, off01[0], off01[1], off23[0], off23[1]);
#else
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);
  signed long long off0, off1, off2, off3;

  off0 = (scalar_extract_uint64_from_high_uint128(gprp) >> 32) << (2+ scale);
  off1 = ((int) scalar_extract_uint64_from_high_uint128(gprp)) << (2+ scale);
  off2 = (scalar_extract_uint64_from_low_uint128(gprp) >> 32) << (2+ scale);
  off3 = ((int) scalar_extract_uint64_from_low_uint128(gprp)) << (2+ scale);

  r = vec_vgl4wso (array, off0, off1, off2, off3);
#endif
  return  r;
}

/** \brief Vector Gather-Load 4 Words from Vector Word Indexes.
 *
 *  For word element [i] of vra, load the word
 *  element at array[vra[i]]. Merge those word elements
 *  and return the resulting vector.
 *
 *  \note Signed word indexes are expanded (unpacked) to doublewords
 *  before shifting left 2 bits. This converts each index to an 64-bit
 *  offset for effective address calculation.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 16-25 | 1/cycle  |
 *  |power9   | 18-27 | 1/cycle  |
 *
 *  @param array Pointer to array of integer words.
 *  @param vra Vector of signed word (32-bit) indexes.
 *  @return vector word containing word elements [0-3], each loaded from
 *  array[vra[0-3]].
 */
static inline
vui32_t
vec_vgl4wwx (unsigned int *array, vi32_t vra)
{
  vui32_t r;

#ifdef _ARCH_PWR8
  vi64_t off01, off23;
  vi64_t lshift = vec_splats ((long long) (2));

  off01 = vec_vupkhsw (vra);
  off23 = vec_vupklsw (vra);

  off01 = (vi64_t) __pvec_vsld (off01, (vui64_t) lshift);
  off23 = (vi64_t) __pvec_vsld (off23, (vui64_t) lshift);

  r = vec_vgl4wso (array, off01[0], off01[1], off23[0], off23[1]);
#else
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);
  signed long long off0, off1, off2, off3;

  off0 = (scalar_extract_uint64_from_high_uint128(gprp) >> 32) << 2;
  off1 = ((int) scalar_extract_uint64_from_high_uint128(gprp)) << 2;
  off2 = (scalar_extract_uint64_from_low_uint128(gprp) >> 32) << 2;
  off3 = ((int) scalar_extract_uint64_from_low_uint128(gprp)) << 2;

  r = vec_vgl4wso (array, off0, off1, off2, off3);
#endif
  return  r;
}

/** \brief Vector Gather-Load Signed Word from Scalar Offsets.
 *
 *  For each scalar offset[0|1], load the signed word
 *  (sign extended) from the effective address formed by
 *  *(char*)array+offset[0|1]. Merge resulting doubleword
 *  elements and return the resulting vector.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   7   | 1/cycle  |
 *  |power9   |   8   | 1/cycle  |
 *
 *  @param array Pointer to array of words.
 *  @param offset0 Scalar (64-bit) byte offsets from &array.
 *  @param offset1 Scalar (64-bit) byte offsets from &array.
 *  @return vector doubleword elements [0,1] loaded from
 *  sign extend words at *(char*)array+offset[0,1].
 */
static inline vi64_t
vec_vglswso (signed int *array, const long long offset0,
	     const long long offset1)
{
  vi64_t re0, re1, result;

  re0 = vec_vlxsiwax (offset0, array);
  re1 = vec_vlxsiwax (offset1, array);
  /* Need to handle endian as the vec_vlxsiwzx result is always left
   * justified in VR, while element [0] may be left or right. */
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  /* Can't use vec_mergeo here as GCC 7 (AT11) and earlier don't
   * support doubleword vec_merge. */
  result = vec_xxpermdi (re0, re1, 3);
#else
#ifdef _ARCH_PWR7
  result = vec_xxpermdi (re0, re1, 0);
#else
  re0 = (vi64_t) vec_sld (re0, re0, 8);
  result = (vi64_t) vec_sld (re0, re1, 8);
#endif
#endif
  return result;
}

/** \brief Vector Gather-Load Signed Words from Vector Doubleword Offsets.
 *
 *  For each doubleword element [i] of vra, load the sign extended word
 *  element at *(char*)array+vra[i]. Merge doubleword elements [0,1]
 *  and return the resulting vector.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   12  | 1/cycle  |
 *  |power9   |   11  | 1/cycle  |
 *
 *  @param array Pointer to array of signed words.
 *  @param vra Vector of doubleword (64-bit) byte offsets from &array.
 *  @return vector doubleword elements [0,1] loaded from sign extended
 *  words at *(char*)array+vra[0,1].
 */
static inline
vi64_t
vec_vglswdo (signed int *array, vi64_t vra)
{
  vi64_t r;

#ifdef _ARCH_PWR8
  r = vec_vglswso (array, vra[0], vra[1]);
#else
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);

  r = vec_vglswso (array, scalar_extract_uint64_from_high_uint128(gprp),
		   scalar_extract_uint64_from_low_uint128(gprp));
#endif
  return  r;
}

/** \brief Vector Gather-Load Signed Words from Vector Doubleword Scaled Indexes.
 *
 *  For each doubleword element [i] of vra, load the sign extended word
 *  element at array[vra[i] << scale)]. Merge doubleword elements [0,1]
 *  and return the resulting vector.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 14-23 | 1/cycle  |
 *  |power9   | 13-22 | 1/cycle  |
 *
 *  @param array Pointer to array of signed words.
 *  @param vra Vector of doubleword indexes from &array.
 *  @param scale 8-bit integer. Indexes are multiplying by
 *  2<sup>scale</sup>.
 *  @return vector doubleword elements [0,1] loaded from the sign
 *  extended words at array[vra[0,1]<<scale].
 */
static inline
vi64_t
vec_vglswdsx (signed int *array, vi64_t vra,
		     const unsigned char scale)
{
  vi64_t r;

#ifdef _ARCH_PWR8
  vi64_t lshift = vec_splats ((long long) (2 + scale));
  vi64_t offset;

  offset = (vi64_t) __pvec_vsld (vra, (vui64_t) lshift);
  r = vec_vglswso (array, offset[0], offset[1]);
#else
  long long offset0, offset1;
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);
  offset0 = scalar_extract_uint64_from_high_uint128(gprp) << (2 + scale);
  offset1 = scalar_extract_uint64_from_low_uint128(gprp) << (2 + scale);

  r = vec_vglswso (array, offset0, offset1);
#endif
  return  r;
}

/** \brief Vector Gather-Load Signed Words from Vector Doubleword Indexes.
 *
 *  For each doubleword element [i] of vra, load the sign extended word
 *  element at array[vra[i]]. Merge doubleword elements [0,1]
 *  and return the resulting vector.
 *
 *  \note As effective address calculation is modulo 64-bits, signed or
 *  unsigned doubleword offsets are equivalent.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 14-23 | 1/cycle  |
 *  |power9   | 13-22 | 1/cycle  |
 *
 *  @param array Pointer to array of signed words.
 *  @param vra Vector of doubleword indexes from &array.
 *  @return vector doubleword elements [0,1] loaded from sign extended
 *  words at array[vra[0,1]].
 */
static inline
vi64_t
vec_vglswdx (signed int *array, vi64_t vra)
{
  vi64_t r;

#ifdef _ARCH_PWR8
  vi64_t lshift = vec_splats ((long long) 2);
  vi64_t offset;

  offset = (vi64_t) __pvec_vsld (vra, (vui64_t) lshift);
  r = vec_vglswso (array, offset[0], offset[1]);
#else
  long long offset0, offset1;
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);
  offset0 = scalar_extract_uint64_from_high_uint128(gprp) << 2;
  offset1 = scalar_extract_uint64_from_low_uint128(gprp) << 2;

  r = vec_vglswso (array, offset0, offset1);
#endif
  return  r;
}

/** \brief Vector Gather-Load Unsigned Word from Scalar Offsets.
 *
 *  For each scalar offset[0,1], load the unsigned word
 *  (zero extended) from the effective address formed by
 *  *(char*)array+offset[0,1] Merge resulting doubleword [0,1]
 *  elements and return the resulting vector.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   7   | 1/cycle  |
 *  |power9   |   8   | 1/cycle  |
 *
 *  @param array Pointer to array of words.
 *  @param offset0 Scalar (64-bit) byte offsets from &array.
 *  @param offset1 Scalar (64-bit) byte offsets from &array.
 *  @return vector doubleword elements [0,1] loaded from zero extened
 *  words at *(char*)array+offset[0,1].
 */
static inline vui64_t
vec_vgluwso (unsigned int *array, const long long offset0,
	     const long long offset1)
{
  vui64_t re0, re1, result;

  re0 = vec_vlxsiwzx (offset0, array);
  re1 = vec_vlxsiwzx (offset1, array);
  /* Need to handle endian as the vec_vlxsiwzx result is always left
   * justified in VR, while element [0] may be left or right. */
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  /* Can't use vec_mergeo here as GCC 7 (AT11) and earlier don't
   * support doubleword vec_merge. */
  result = vec_xxpermdi (re0, re1, 3);
#else
#ifdef _ARCH_PWR7
  result = vec_xxpermdi (re0, re1, 0);
#else
  re0 = (vui64_t) vec_sld (re0, re0, 8);
  result = (vui64_t) vec_sld (re0, re1, 8);
#endif
#endif
  return result;
}

/** \brief Vector Gather-Load Unsigned Words from Vector Doubleword Offsets.
 *
 *  For each doubleword element [0,1] of vra, load the zero extended word
 *  element at *(char*)array+vra[0,1]. Merge those doubleword elements [0,1]
 *  and return the resulting vector.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   12  | 1/cycle  |
 *  |power9   |   11  | 1/cycle  |
 *
 *  @param array Pointer to array of unsigned words.
 *  @param vra Vector of doubleword (64-bit) byte offsets from &array.
 *  @return vector doubleword elements [0,1] loaded from zero extended words
 *  at *(char*)array+vra[0,1].
 */
static inline
vui64_t
vec_vgluwdo (unsigned int *array, vi64_t vra)
{
  vui64_t r;

#ifdef _ARCH_PWR8
  r = vec_vgluwso (array, vra[0], vra[1]);
#else
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);

  r = vec_vgluwso (array, scalar_extract_uint64_from_high_uint128(gprp),
		   scalar_extract_uint64_from_low_uint128(gprp));
#endif
  return  r;
}

/** \brief Vector Gather-Load Unsigned Words from Vector Doubleword Scaled Indexes.
 *
 *  For each doubleword element [0,1] of vra, load the zero extended word
 *  element at array[vra[0,1] << scale)]. Merge doubleword elements [0,1]
 *  and return the resulting vector.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 14-23 | 1/cycle  |
 *  |power9   | 13-22 | 1/cycle  |
 *
 *  @param array Pointer to array of unsigned words.
 *  @param vra Vector of doubleword indexes from &array.
 *  @param scale 8-bit integer. Indexes are multiplying by
 *  2<sup>scale</sup>.
 *  @return vector doubleword elements [0,1] loaded from zero extended
 *  words at array[vra[0,1]<<scale].
 */
static inline
vui64_t
vec_vgluwdsx (unsigned int *array, vi64_t vra,
		     const unsigned char scale)
{
  vui64_t r;

#ifdef _ARCH_PWR8
  vui64_t lshift = vec_splats ((unsigned long long) (2 + scale));
  vui64_t offset;

  offset = (vui64_t) __pvec_vsld (vra, (vui64_t) lshift);
  r = vec_vgluwso (array, offset[0], offset[1]);
#else
  long long offset0, offset1;
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);
  offset0 = scalar_extract_uint64_from_high_uint128(gprp) << (2 + scale);
  offset1 = scalar_extract_uint64_from_low_uint128(gprp) << (2 + scale);

  r = vec_vgluwso (array, offset0, offset1);
#endif
  return  r;
}

/** \brief Vector Gather-Load Unsigned Words from Vector Doubleword Indexes.
 *
 *  For each doubleword element [0,1] of vra, load the zero extended word
 *  element at array[vra[0,1]]. Merge those doubleword elements [0,1]
 *  and return the resulting vector.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 14-23 | 1/cycle  |
 *  |power9   | 13-22 | 1/cycle  |
 *
 *  @param array Pointer to array of unsigned words.
 *  @param vra Vector of doubleword indexes from &array.
 *  @return Vector doubleword [0,1] loaded from zero extended words at
 *  array[vra[0,1]].
 */
static inline
vui64_t
vec_vgluwdx (unsigned int *array, vi64_t vra)
{
  vui64_t r;

#ifdef _ARCH_PWR8
  vui64_t lshift = vec_splats ((unsigned long long) 2);
  vui64_t offset;

  offset = (vui64_t) __pvec_vsld (vra, (vui64_t) lshift);
  r = vec_vgluwso (array, offset[0], offset[1]);
#else
  long long offset0, offset1;
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);
  offset0 = scalar_extract_uint64_from_high_uint128(gprp) << 2;
  offset1 = scalar_extract_uint64_from_low_uint128(gprp) << 2;

  r = vec_vgluwso (array, offset0, offset1);
#endif
  return  r;
}

/** \brief Vector Load Scalar Integer Word Algebraic Indexed.
 *
 *  Load the left most doubleword of vector <B>xt</B> as a scalar
 *  sign extended word from the effective address formed
 *  by <B>rb+ra</B>.
 *  The operand <B>rb</B> is a pointer to an array of words.
 *  The operand <B>ra</B> is a doubleword integer byte offset
 *  from <B>rb</B>. The result <B>xt</B> is returned as a vi64_t
 *  vector. For best performance <B>rb</B> and <B>ra</B>
 *  should be word aligned (integer multiple of 4).
 *
 *  \note The right most doubleword of vector <B>xt</B> is left
 *  <I>undefined</I> by this operation.
 *
 *  This operation is an alternate form of Vector Load Element
 *  (vec_lde), with the added simplification that data is always left
 *  justified in the vector. Another advantage for Power8 and later,
 *  the lxsiwax instruction combines load with sign extend word and
 *  can load directly into any of the 64 VSRs.
 *  Both simplify merging elements for gather operations.
 *
 *  \note The lxsiwax instruction was introduced in PowerISA 2.07
 *  (POWER8). Power7 and earlier will use lvewx.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   5   | 2/cycle  |
 *  |power9   |   5   | 2/cycle  |
 *
 *  @param ra const doubleword index (offset/displacement).
 *  @param rb const word pointer to an array of integers.
 *  @return The word stored at (ra + rb) is sign extended and loaded
 *  into vector doubleword element 0. Element 1 is undefined.
 */
static inline vi64_t
vec_vlxsiwax (const signed long long ra, const signed int *rb)
{
  vi64_t xt;

#if (defined(__clang__) && __clang_major__ < 8)
  __VEC_U_128 t;

  signed int *p = (signed int *)((char *)rb + ra);
  t.ulong.upper = *p;
  xt = t.vx2;
#elif _ARCH_PWR8
  if (__builtin_constant_p (ra) && (ra <= 32760) && (ra >= -32768))
    {
      if (ra == 0)
	{
	  __asm__(
	      "lxsiwax %x0,%y1;"
	      : "=wa" (xt)
	      : "Z" (*rb)
	      : );
	} else {
	  unsigned long long rt;
	  __asm__(
	      "li %0,%1;"
	      : "=r" (rt)
	      : "I" (ra)
	      : );
	  __asm__(
	      "lxsiwax %x0,%y1;"
	      : "=wa" (xt)
	      : "Z" (*(signed int *)((char *)rb+rt))
	      : );
	}
    } else {
      __asm__(
	  "lxsiwax %x0,%y1;"
	  : "=wa" (xt)
	  : "Z" (*(signed int *)((char *)rb+ra))
	  : );
    }
#else // _ARCH_PWR7
  vui32_t const shb = { 31, 0, 0 ,0 };
  vi32_t xte;
  vui8_t perm;

  perm = vec_lvsl (ra, rb);
  xte = vec_lde (ra, rb);
  perm = (vui8_t) vec_mergeh ((vui32_t) perm, (vui32_t) perm);
  xte = vec_perm (xte, xte, perm);
  xt = (vi64_t) vec_sra (xte, shb);
#endif
  return xt;
}

/** \brief Vector Load Scalar Integer Word and Zero Indexed.
 *
 *  Load the left most doubleword of vector <B>xt</B> as a scalar
 *  unsigned word (zero extended to doubleword) from the effective
 *  address formed by <B>rb+ra</B>.
 *  The operand <B>rb</B> is a pointer to an array of words.
 *  The operand <B>ra</B> is a doubleword integer byte offset
 *  from <B>rb</B>. The result <B>xt</B> is returned as a vui64_t
 *  vector. For best performance <B>rb</B> and <B>ra</B>
 *  should be word aligned (integer multiple of 4).
 *
 *  \note the right most doubleword of vector <B>xt</B> is left
 *  <I>undefined</I> by this operation.
 *
 *  This operation is an alternate form of Vector Load Element
 *  (vec_lde), with the added simplification that data is always left
 *  justified in the vector. Another advantage for Power8 and later,
 *  the lxsiwzx instruction combines load with zero extend word and
 *  can load directly into any of the 64 VSRs.
 *  Both simplify merging elements for gather operations.
 *
 *  \note
 *
 *  \note The lxsiwzx instruction was introduced in PowerISA 2.07
 *  (POWER8). Power7 and earlier will use lvewx.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   5   | 2/cycle  |
 *  |power9   |   5   | 2/cycle  |
 *
 *  @param ra const doubleword index (offset/displacement).
 *  @param rb const word pointer to an array of integers.
 *  @return The word stored at (ra + rb) is zero extended and loaded
 *  into vector doubleword element 0. Element 1 is undefined.
 */
static inline vui64_t
vec_vlxsiwzx (const signed long long ra, const unsigned int *rb)
{
  vui64_t xt;

#if (defined(__clang__) && __clang_major__ < 8)
  __VEC_U_128 t;

  unsigned int *p = (unsigned int *)((char *)rb + ra);
  t.ulong.upper = *p;
  xt = t.vx2;
#elif _ARCH_PWR8
  if (__builtin_constant_p (ra) && (ra <= 32760) && (ra >= -32768))
    {
      if (ra == 0)
	{
	  __asm__(
	      "lxsiwzx %x0,%y1;"
	      : "=wa" (xt)
	      : "Z" (*rb)
	      : );
	} else {
	  unsigned long long rt;
	  __asm__(
	      "li %0,%1;"
	      : "=r" (rt)
	      : "I" (ra)
	      : );
	  __asm__(
	      "lxsiwzx %x0,%y1;"
	      : "=wa" (xt)
	      : "Z" (*(signed int *)((char *)rb+rt))
	      : );
	}
    } else {
      __asm__(
	  "lxsiwzx %x0,%y1;"
	  : "=wa" (xt)
	  : "Z" (*(signed int *)((char *)rb+ra))
	  : );
    }
#else // _ARCH_PWR7
  const vui32_t zero = {0,0,0,0};
  vui32_t xte;
  vui8_t perm;

  perm = vec_lvsl (ra, rb);
  xte = vec_lde (ra, rb);
  xte = vec_perm (xte, xte, perm);
  xt = (vui64_t) vec_sld (zero, xte, 12);
#endif
  return xt;
}

/** \brief \copybrief vec_int64_ppc.h::vec_vmadd2euw()
 *
 * \note this implementation exists in
 * \ref vec_int64_ppc.h::vec_vmadd2euw()
 * as it requires vec_addudm().
 */
static inline vui64_t
vec_vmadd2euw (vui32_t a, vui32_t b, vui32_t c, vui32_t d);

/** \brief \copybrief vec_int64_ppc.h::vec_vmadd2ouw()
 *
 * \note this implementation exists in
 * \ref vec_int64_ppc.h::vec_vmadd2ouw()
 * as it requires vec_addudm().
 */
static inline vui64_t
vec_vmadd2ouw (vui32_t a, vui32_t b, vui32_t c, vui32_t d);

/** \brief \copybrief vec_int64_ppc.h::vec_vmaddeuw()
 *
 * \note this implementation exists in
 * \ref vec_int64_ppc.h::vec_vmaddeuw()
 * as it requires vec_addudm().
 */
static inline vui64_t
vec_vmaddeuw (vui32_t a, vui32_t b, vui32_t c);

/** \brief \copybrief vec_int64_ppc.h::vec_vmaddouw()
 *
 * \note this implementation exists in
 * \ref vec_int64_ppc.h::vec_vmaddouw()
 * as it requires vec_addudm().
 */
static inline vui64_t
vec_vmaddouw (vui32_t a, vui32_t b, vui32_t c);

/** \brief \copybrief vec_int64_ppc.h::vec_vmsumuwm()
 *
 * \note this implementation exists in
 * \ref vec_int64_ppc.h::vec_vmsumuwm()
 * as it requires vec_addudm().
 */
static inline vui64_t
vec_vmsumuwm (vui32_t a, vui32_t b, vui64_t c);

/** \brief Vector Multiply Even Unsigned words.
 *
 * Multiply the even words of two vector unsigned int values and return
 * the unsigned long product of the even words.
 *
 * For POWER8 and later we can use the vmuleuw instruction.
 * But for POWER7 and earlier we have to construct word multiplies
 * from two halfword multiplies (vmuleuh and vmulouh). Then sum the
 * partial products for the final doubleword results. This is
 * complicated by the fact that vector add doubleword is not available
 * for POWER7. So we need to construct the doubleword add from
 * Vector Add Unsigned Word Modulo (vadduwm) and
 * Vector Add and Write Carry-Out Unsigned Word (vaddcuw) with
 * shift double quadword to reposition the low word carry and
 * a final vadduwm to complete the carry propagation for the
 * doubleword add.
 *
 * \note This implementation is NOT endian sensitive and the function
 * is stable across BE/LE implementations.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |   7   | 2/cycle  |
 * |power9   |   7   | 2/cycle  |
 *
 * @param vra 128-bit vector unsigned int.
 * @param vrb 128-bit vector unsigned int.
 * @return vector unsigned long product of the even words of a and b.
 */
static inline vui64_t
vec_vmuleuw (vui32_t vra, vui32_t vrb)
{
  vui64_t res;
#ifdef _ARCH_PWR8
#if defined __GNUC__ && (__GNUC__ > 7)
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  res = vec_mulo (vra, vrb);
#else
  res = vec_mule (vra, vrb);
#endif
#else
  __asm__(
      "vmuleuw %0,%1,%2;\n"
      : "=v" (res)
      : "v" (vra),
      "v" (vrb)
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

  mt = (vui16_t)vrb;
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

  resw = vec_sld (vra, vra, 12);
  resw = vec_sel (vra, resw, wmask01);
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

/** \brief Vector Multiply Odd Unsigned Words.
 *
 * Multiply the odd words of two vector unsigned int values and return
 * the unsigned long product of the odd words.
 *
 * For POWER8 and later we can use the vmulouw instruction.
 * But for POWER7 and earlier we have to construct word multiplies
 * from two halfword multiplies (vmuleuh and vmulouh). Then sum the
 * partial products for the final doubleword results. This is
 * complicated by the fact that vector add doubleword is not available
 * for POWER7. So we need to construct the doubleword add from
 * Vector Add Unsigned Word Modulo (vadduwm) and
 * Vector Add and Write Carry-Out Unsigned Word (vaddcuw) with
 * shift double quadword to reposition the low word carry and
 * a final vadduwm to complete the carry propagation for the doubleword
 * add.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |   7   | 2/cycle  |
 * |power9   |   7   | 2/cycle  |
 *
 * @param vra 128-bit vector unsigned int.
 * @param vrb 128-bit vector unsigned int.
 * @return vector unsigned long product of the odd words of a and b.
 */
static inline vui64_t
vec_vmulouw (vui32_t vra, vui32_t vrb)
{
  vui64_t res;
#ifdef _ARCH_PWR8
#if defined __GNUC__ && (__GNUC__ > 7)
  /* Not supported in GCC yet.  ETA GCC-8.  */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  res = vec_mule (vra, vrb);
#else
  res = vec_mulo (vra, vrb);
#endif
#else
  __asm__(
      "vmulouw %0,%1,%2;\n"
      : "=v" (res)
      : "v" (vra),
      "v" (vrb)
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

  mt = (vui16_t)vrb;
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

  resw = vec_sld (vra, vra, 4);
  m1 = (vui16_t)vec_sel (resw, vra, wmask01);

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

/** \brief Vector Scatter-Store 4 words to Scalar Offsets.
 *
 *  For each word element [i] of xs, store the
 *  element xs[i] at *(char*)array+offset[i].
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   6   | 1/cycle  |
 *  |power9   |   4   | 2/cycle  |
 *
 *  @param xs Vector integer word elements to scatter store.
 *  @param array Pointer to array of integer words.
 *  @param offset0 Scalar (64-bit) byte offset from &array.
 *  @param offset1 Scalar (64-bit) byte offset from &array.
 *  @param offset2 Scalar (64-bit) byte offset from &array.
 *  @param offset3 Scalar (64-bit) byte offset from &array.
 */
static inline void
vec_vsst4wso (vui32_t xs, unsigned int *array,
	      const long long offset0, const long long offset1,
	      const long long offset2, const long long offset3)
{
  vui32_t xs0, xs1, xs2, xs3;

  xs0 = vec_splat (xs, 0);
  xs1 = vec_splat (xs, 1);
  xs2 = vec_splat (xs, 2);
  xs3 = vec_splat (xs, 3);
  vec_ste (xs0, offset0, array);
  vec_ste (xs1, offset1, array);
  vec_ste (xs2, offset2, array);
  vec_ste (xs3, offset3, array);
}

/** \brief Vector Scatter-Store 4 words to Vector Word Offsets.
 *
 *  For each word element [i] of xs, store the
 *  element xs[i] at *(char*)array+vra[i].
 *
 *  \note Signed word offsets are expanded (unpacked) to doublewords
 *  before transfer to GRPs for effective address calculation.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  10   | 1/cycle  |
 *  |power9   |  12   | 2/cycle  |
 *
 *  @param xs Vector integer word elements to scatter store.
 *  @param array Pointer to array of integer words.
 *  @param vra Vector of signed word (32-bit) byte offsets from &array.
 */
static inline void
vec_vsst4wwo (vui32_t xs, unsigned int *array,
	      vi32_t vra)
{
#ifdef _ARCH_PWR8
  vi64_t off01, off23;

  off01 = vec_vupkhsw (vra);
  off23 = vec_vupklsw (vra);

  vec_vsst4wso (xs, array, off01[0], off01[1], off23[0], off23[1]);
#else
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);
  signed int off0, off1, off2, off3;

  off0 = scalar_extract_uint64_from_high_uint128(gprp) >> 32;
  off1 = (int) scalar_extract_uint64_from_high_uint128(gprp);
  off2 = scalar_extract_uint64_from_low_uint128(gprp) >> 32;
  off3 = (int) scalar_extract_uint64_from_low_uint128(gprp);

  vec_vsst4wso (xs, array, off0, off1, off2, off3);
#endif
}

/** \brief Vector Scatter-Store 4 words to Vector Word Indexes.
 *
 *  For each word element [i] of xs, store the
 *  element xs[i] at *(char*)array[vra[i]<<scale].
 *
 *  \note Signed word indexes are expanded (unpacked) to doublewords
 *  before shifting left (2+scale) bits before transfer to GRPs for
 *  effective address calculation. This converts each index to an
 *  64-bit offset.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 12-21 | 1/cycle  |
 *  |power9   | 15-24 | 2/cycle  |
 *
 *  @param xs Vector integer word elements to scatter store.
 *  @param array Pointer to array of integer words.
 *  @param vra Vector of signed word (32-bit) indexes from array.
 *  @param scale 8-bit integer. Indexes are multiplying by
 *  2<sup>scale</sup>.
 */
static inline void
vec_vsst4wwsx (vui32_t xs, unsigned int *array,
	      vi32_t vra, const unsigned char scale)
{
#ifdef _ARCH_PWR8
  vi64_t off01, off23;
  vui64_t lshift = vec_splats ((unsigned long long) (2 + scale));

  off01 = vec_vupkhsw (vra);
  off23 = vec_vupklsw (vra);

  off01 = (vi64_t) __pvec_vsld (off01, (vui64_t) lshift);
  off23 = (vi64_t) __pvec_vsld (off23, (vui64_t) lshift);

  vec_vsst4wso (xs, array, off01[0], off01[1], off23[0], off23[1]);
#else
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);
  signed int off0, off1, off2, off3;

  off0 = (scalar_extract_uint64_from_high_uint128(gprp) >> 32) << (2 + scale);
  off1 = ((int) scalar_extract_uint64_from_high_uint128(gprp)) << (2 + scale);
  off2 = (scalar_extract_uint64_from_low_uint128(gprp) >> 32) << (2 + scale);
  off3 = ((int) scalar_extract_uint64_from_low_uint128(gprp)) << (2 + scale);

  vec_vsst4wso (xs, array, off0, off1, off2, off3);
#endif
}

/** \brief Vector Scatter-Store 4 words to Vector Word Indexes.
 *
 *  For each word element [i] of xs, store the
 *  element xs[i] at *(char*)array[vra[i]].
 *
 *  \note Signed word indexes are expanded (unpacked) to doublewords
 *  before shifting left 2 bits. This converts each index to an
 *  64-bit offset for effective address calculation.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 12-21 | 1/cycle  |
 *  |power9   | 15-24 | 2/cycle  |
 *
 *  @param xs Vector doubleword elements to scatter store.
 *  @param array Pointer to array of integer words.
 *  @param vra Vector of signed word (32-bit) indexes from array.
 */
static inline void
vec_vsst4wwx (vui32_t xs, unsigned int *array,
	      vi32_t vra)
{
#ifdef _ARCH_PWR8
  vi64_t off01, off23;
  vui64_t lshift = vec_splats ((unsigned long long) 2);

  off01 = vec_vupkhsw (vra);
  off23 = vec_vupklsw (vra);

  off01 = (vi64_t) __pvec_vsld (off01, (vui64_t) lshift);
  off23 = (vi64_t) __pvec_vsld (off23, (vui64_t) lshift);

  vec_vsst4wso (xs, array, off01[0], off01[1], off23[0], off23[1]);
#else
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);
  signed int off0, off1, off2, off3;

  off0 = (scalar_extract_uint64_from_high_uint128(gprp) >> 32) << 2;
  off1 = ((int) scalar_extract_uint64_from_high_uint128(gprp)) << 2;
  off2 = (scalar_extract_uint64_from_low_uint128(gprp) >> 32) << 2;
  off3 = ((int) scalar_extract_uint64_from_low_uint128(gprp)) << 2;

  vec_vsst4wso (xs, array, off0, off1, off2, off3);
#endif
}

/** \brief Vector Scatter-Store Words to Vector Doubleword Offsets
 *
 *  For each doubleword element [i] of vra, Store the low order word
 *  element xs[i+1] at *(char*)array+offset[0|1].
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   8   | 1/cycle  |
 *  |power9   |   9   | 2/cycle  |
 *
 *  @param xs Vector doubleword elements to scatter store
 *  low order words of each doubleword.
 *  @param array Pointer to array of integer words.
 *  @param vra Vector of doubleword (64-bit) byte offsets from &array.
 */
static inline void
vec_vsstwdo (vui64_t xs, unsigned int *array, vi64_t vra)
{
#ifdef _ARCH_PWR8
  vec_vsstwso (xs, array, vra[0], vra[1]);
#else
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);

  vec_vsstwso (xs, array,
		scalar_extract_uint64_from_high_uint128(gprp),
		scalar_extract_uint64_from_low_uint128(gprp));
#endif
}

/** \brief Vector Scatter-Store Words to Vector Doubleword Scaled Indexes.
 *
 *  For each doubleword element [i] of vra, Store the low order word
 *  element xs[i+1] at array[vra[i]<<scale].
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-19 | 1/cycle  |
 *  |power9   | 10-19 | 1/cycle  |
 *
 *  @param xs Vector doubleword elements to scatter store
 *  low order words of each doubleword.
 *  @param array Pointer to array of integer words.
 *  @param vra Vector of doubleword (64-bit) indexes from &array.
 *  @param scale 8-bit integer. Indexes are multiplying by
 *  2<sup>scale</sup>.
 */
static inline void
vec_vsstwdsx (vui64_t xs, unsigned int *array, vi64_t vra,
	      const unsigned char scale)
{
#ifdef _ARCH_PWR8
  vui64_t lshift = vec_splats ((unsigned long long) (2 + scale));
  vui64_t offset;

  offset = (vui64_t) __pvec_vsld (vra, (vui64_t) lshift);
  vec_vsstwso (xs, array, offset[0], offset[1]);
#else
  long long offset0, offset1;
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);
  offset0 = scalar_extract_uint64_from_high_uint128(gprp) << (2 + scale);
  offset1 = scalar_extract_uint64_from_low_uint128(gprp) << (2 + scale);

  vec_vsstwso (xs, array, offset0, offset1);
#endif
}

/** \brief Vector Scatter-Store Words to Vector Doubleword Indexes.
 *
 *  For each doubleword element [i] of vra, Store the low order word
 *  element xs[i+1] at array[vra[i]].
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-19 | 1/cycle  |
 *  |power9   | 10-19 | 1/cycle  |
 *
 *  @param xs Vector doubleword elements to scatter store
 *  low order words of each doubleword.
 *  @param array Pointer to array of integer words.
 *  @param vra Vector of doubleword (64-bit) indexes from &array.
 */
static inline void
vec_vsstwdx (vui64_t xs, unsigned int *array, vi64_t vra)
{
#ifdef _ARCH_PWR8
  vui64_t lshift = vec_splats ((unsigned long long) 2);
  vui64_t offset;

  offset = (vui64_t) __pvec_vsld (vra, (vui64_t) lshift);
  vec_vsstwso (xs, array, offset[0], offset[1]);
#else
  long long offset0, offset1;
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);
  offset0 = scalar_extract_uint64_from_high_uint128(gprp) << 2;
  offset1 = scalar_extract_uint64_from_low_uint128(gprp) << 2;

  vec_vsstwso (xs, array, offset0, offset1);
#endif
}

/** \brief Vector Scatter-Store Words to Scalar Offsets.
 *
 *  For each doubleword element [i] of vra, Store the low order word
 *  element xs[i+1] at *(char*)array+offset[0|1].
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   3   | 1/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param xs Vector doubleword elements to scatter store
 *  low order words of each doubleword.
 *  @param array Pointer to array of integer words.
 *  @param offset0 Scalar (64-bit) byte offset from &array.
 *  @param offset1 Scalar (64-bit) byte offset from &array.
 */
static inline void
vec_vsstwso (vui64_t xs, unsigned int *array,
	      const long long offset0, const long long offset1)
{
  vui32_t xs0, xs1;

  xs0 = (vui32_t) xs;
  // xs1 = vec_xxswapd (xs);
#ifdef _ARCH_PWR7
  xs1 = (vui32_t) vec_xxpermdi (xs, xs, 2);
#else
  xs1 = vec_sld (xs0, xs0, 8);
#endif
  /* Need to handle endian as vec_vstsxiwx always stores the right word
   * from the left doubleword of the VSR, while word element [1] may in
   * the left or right doubleword. */
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  vec_vstxsiwx (xs0, offset1, array);
  vec_vstxsiwx (xs1, offset0, array);
#else
  vec_vstxsiwx (xs0, offset0, array);
  vec_vstxsiwx (xs1, offset1, array);
#endif
}

/** \brief Vector Store Scalar Integer Word Indexed.
 *
 *  Stores word element 1 of vector <B>xs</B> as a scalar
 *  word at the effective address formed by <B>rb+ra</B>. The
 *  operand <B>rb</B> is a pointer to an array of integer words.
 *  The operand <B>ra</B> is a doubleword integer byte offset
 *  from <B>rb</B>. For best performance <B>rb</B> and <B>ra</B>
 *  should be word aligned (integer multiple of 4).
 *
 *  This operation is an alternate form of vector store element
 *  (vec_ste), with the added simplification that data is always left
 *  justified in the vector. Another advantage for Power8 and later,
 *  the stxsiwx instruction can load directly into any of the 64 VSRs.
 *  Both simplify scatter operations.
 *
 *  \note The stxsiwx instruction was introduced in PowerISA 2.07
 *  (POWER8). Power7 and earlier will use stvewx.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 0 - 2 | 2/cycle  |
 *  |power9   | 0 - 2 | 4/cycle  |
 *
 *  @param xs vector doubleword element 0 to be stored.
 *  @param ra const doubleword index (offset/displacement).
 *  @param rb const doubleword pointer to an array of doubles.
 */
static inline void
vec_vstxsiwx (vui32_t xs, const signed long long ra, unsigned int *rb)
{
#if (defined(__clang__) && __clang_major__ < 8)
  __VEC_U_128 t;
  unsigned int *p = (unsigned int *)((char *)rb + ra);
  t.vx4 = xs;
  *p = t.ulong.upper;
#elif _ARCH_PWR8
  if (__builtin_constant_p (ra) &&  (ra <= 32760) && (ra >= -32768))
    {
      if (ra == 0)
	{
	  __asm__(
	      "stxsiwx %x1,%y0;"
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
	      "stxsiwx %x1,%y0;"
	      : "=Z" (*(unsigned int *)((char *)rb+rt))
	      : "wa" (xs)
	      : );
	}
    } else {
      __asm__(
	  "stxsiwx %x1,%y0;"
	  : "=Z" (*(unsigned int *)((char *)rb+ra))
	  : "wa" (xs)
	  : );
    }
#else //_ARCH_PWR8
  // Splat word element 1 to all elements
  vui32_t xss = vec_splat (xs, 1);
  // store a word element at the EA (ra+rb)
  vec_ste (xss, ra, rb);
#endif
}

/** \brief Vector Sum-across Half Signed Word Saturate.
 *
 * Sum across adjacent signed words within doublewords from <I>vra</I>
 * and word addends from <I>vrb</I>.
 * This is effectively the vec_sum2s built-in operation (vsum2sws
 * instruction) without the endian sensitive modifications
 * mandated by the ABI.
 *
 * This is useful for computing the final doubleword counts for
 * operations like population count and count leading/trailing zeros.
 * These results are often used as inputs to shift operations that
 * require shift counts in bits 58:63 of the doubleword element
 * (word elements 1 and 3).
 *
 * For vec_sum2s and little endian the ABI mandates that the addend
 * words from vrb be from little endian word elements 1 and 3
 * (vector element 0 and 2) be used for the sum. The ABI also mandates
 * that saturated word sum results are are in little endian
 * elements 1 and 3 (vector element 0 and 2). This requires a 3
 * instruction dependent sequence to precondition vrb and and rotate
 * the vsum2sws result to match little endian element numbering.
 * This adds 4 (6 for POWER9) cycles latency.
 *
 * This also leaves the sums in bits 26:31 of the doubleword element
 * and out of position for doubleword shift/rotate. This in turn
 * requires an additional corrective shift/rotate before using the
 * sums. Or use this operation instead of vec_sum2s.
 *
 * \note This implementation is NOT endian sensitive and the function
 * is stable across BE/LE implementations.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |   7   | 2/cycle  |
 * |power9   |   7   | 2/cycle  |
 *
 * @param vra Vector signed int as adjcent words within doublewords.
 * @param vrb Vector signed int where odd words are summed
 * with adjacent words from vra.
 * @return Vector signed int with even words set to 0
 * and odd words containing the word sums within doublewords.
 */
static inline vi32_t
vec_vsum2sw (vi32_t vra, vi32_t vrb)
{
  vi32_t res;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  res = vec_sum2s (vra, vrb);
#else
  __asm__(
      "vsum2sws %0,%1,%2;\n"
      : "=v" (res)
      : "v" (vra),
      "v" (vrb)
      : );
#endif
  return ((vi32_t) res);
}

/** \brief Vector Sum-across Signed Word Saturate.
 *
 * Sum across the 4 signed words from <I>vra</I>
 * and word element 3 from <I>vrb</I>.
 * This is effectively the vec_sums built-in operation (vsumsws
 * instruction) without the endian sensitive modifications
 * mandated by the ABI.
 *
 * This is useful for computing the final quadword counts for
 * operations like population count and count leading/trailing zeros.
 * These results are often used as inputs to shift operations that
 * require shift counts in bits 121:127 of the quadword
 * (word element 3).
 *
 * For vec_sums and little endian the ABI mandates that the addend
 * word from vrb be from little endian word elements 3
 * (vector element 0) be used for the sum. The ABI also mandates
 * that saturated word sum results are are in little endian
 * elements 3 (vector element 0). This requires a 3
 * instruction dependent sequence to precondition vrb and and rotate
 * the vsumsws result to match little endian element numbering.
 * This adds 4 (6 for POWER9) cycles latency.
 *
 * This also leaves the sums in bits 25:31 of the quadword
 * and out of position for quadword shift/rotate. This in turn
 * requires an additional corrective shift/rotate before using the
 * sums. Or use this operation instead of vec_sums.
 *
 * \note This implementation is NOT endian sensitive and the function
 * is stable across BE/LE implementations.
 *
 * |processor|Latency|Throughput|
 * |--------:|:-----:|:---------|
 * |power8   |   7   | 2/cycle  |
 * |power9   |   7   | 2/cycle  |
 *
 * @param vra Vector signed int as words within quadword.
 * @param vrb Vector signed int where word element 3 is summed
 * with words from vra.
 * @return Vector signed int with words 0-2 set to 0
 * and word element 3 containing the word sums.
 */
static inline vi32_t
vec_vsumsw (vi32_t vra, vi32_t vrb)
{
  vi32_t res;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  res = vec_sums (vra, vrb);
#else
  __asm__(
      "vsumsws %0,%1,%2;\n"
      : "=v" (res)
      : "v" (vra),
      "v" (vrb)
      : );
#endif
  return ((vi32_t) res);
}

/** \brief Vector Unpack High Signed Word.
 *
 *  From the word source in vra.
 *  For each integer word [i] from 0 to 1,
 *  sign extend to 64-bit and place in
 *  doubleword element [i] of the result vector.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   2   | 2/cycle  |
 *
 *  \note This operation is the equivalent of the generic vec_unpackh
 *  for type vector signed int. However vec_unpackh (for this type)
 *  is not available for _ARCH_PWR7 and earlier versions of GCC.
 *  This PVECLIB operation is available to both.
 *  \note Use vec_vupkhsw naming but only
 *  if the compiler does not define it in <altivec.h>.
 *
 *  @param vra a 128-bit vector treated as 4 x signed integers.
 *  @return 128-bit vector treated as 2 x signed long long integers.
 */
#ifndef vec_vupkhsw
// May be defined as inline function for clang
// But only for _ARCH_PWR8 or higher.
#if !defined(__clang__) || !defined(_ARCH_PWR8)
static inline vi64_t
vec_vupkhsw (vi32_t vra)
{
  vi64_t r;
#ifdef _ARCH_PWR8
  __asm__(
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
      "vupklsw %0,%1;\n"
#else
      "vupkhsw %0,%1;\n"
#endif
      : "=v" (r)
      : "v" (vra)
      : );
#else
  vui32_t const shb = { 31, 0, 31 ,0 };
  vi32_t xra;

  xra = vec_mergeh (vra, vra);
  r = (vi64_t) vec_sra (xra, shb);
#endif
  return (r);
}
#endif
#endif

/** \brief Vector Unpack High Unsigned Word.
 *
 *  From the word source in vra.
 *  For each integer word [i] from 0 to 1,
 *  zero extend to 64-bit and place in
 *  doubleword element [i] of the result vector.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  2-4  | 2/cycle  |
 *  |power9   |  2-4  | 2/cycle  |
 *
 *  \note vec_vupkhuw does not exist in <altivec.h> nor as an
 *  instruction is the PowerISA. But it is easy to construct using
 *  vec_mergeh and a zero vector.
 *
 *  @param vra a 128-bit vector treated as 4 x unsigned integers.
 *  @return 128-bit vector treated as 2 x unsigned long long integers.
 */
static inline vui64_t
vec_vupkhuw (vui32_t vra)
{
  vui32_t const zero = { 0, 0, 0 ,0 };
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return (vui64_t) vec_mergeh (vra, zero);
#else
  return (vui64_t) vec_mergeh (zero, vra);
#endif
}

/** \brief Vector Unpack Low Signed Word.
 *
 *  From the word source in vra.
 *  For each integer word [i+2] from 0 to 1 (words 2 and 3),
 *  sign extend to 64-bit and place in
 *  doubleword element [i] of the result vector.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   2   | 2/cycle  |
 *  |power9   |   2   | 2/cycle  |
 *
 *  \note Use vec_vupkhsw naming but only
 *  if the compiler does not define it in <altivec.h>.
 *
 *  @param vra a 128-bit vector treated as 4 x signed integers.
 *  @return 128-bit vector treated as 2 x signed long long integers.
 */
#ifndef vec_vupklsw
// May be defined as inline function for clang
// But only for _ARCH_PWR8 or higher.
#if !defined(__clang__) || !defined(_ARCH_PWR8)
static inline vi64_t
vec_vupklsw (vi32_t vra)
{
  vi64_t r;
#ifdef _ARCH_PWR8
  __asm__(
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
      "vupkhsw %0,%1;\n"
#else
      "vupklsw %0,%1;\n"
#endif
      : "=v" (r)
      : "v" (vra)
      : );
#else
  vui32_t const shb = { 31, 0, 31 ,0 };
  vi32_t xra;

  xra = vec_mergel (vra, vra);
  r = (vi64_t) vec_sra (xra, shb);
#endif
  return (r);
}
#endif
#endif

/** \brief Vector Unpack Low Unsigned Word.
 *
 *  From the word source in vra.
 *  For each integer word [i+2] from 0 to 1 (words 2 and 3),
 *  zero extend to 64-bit and place in
 *  doubleword element [i] of the result vector.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  2-4  | 2/cycle  |
 *  |power9   |  2-4  | 2/cycle  |
 *
 *  \note vec_vupkluw does not exist in <altivec.h> nor as an
 *  instruction is the PowerISA. But it is easy to construct using
 *  vec_mergeh and a zero vector.
 *
 *  @param vra a 128-bit vector treated as 4 x unsigned integers.
 *  @return 128-bit vector treated as 2 x unsigned long long integers.
 */
static inline vui64_t
vec_vupkluw (vui32_t vra)
{
  vui32_t const zero = { 0, 0, 0 ,0 };
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return (vui64_t) vec_mergel (vra, zero);
#else
  return (vui64_t) vec_mergel (zero, vra);
#endif
}

#endif /* VEC_INT32_PPC_H_ */
