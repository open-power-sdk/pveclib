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

 vec_f32_ppc.h

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Apr 13, 2016
 */

#ifndef VEC_F32_PPC_H_
#define VEC_F32_PPC_H_

/*!
 * \file  vec_f32_ppc.h
 * \brief Header package containing a collection of 128-bit SIMD
 * operations over 4x32-bit floating point elements.
 *
 * Most vector float (32-bit float) operations are implemented
 * with PowerISA VMX instructions either defined by the original VMX
 * (a.k.a. Altivec) or added to later versions of the PowerISA.
 * POWER8 added the Vector Scalar Extended (VSX) with access to
 * additional vector registers (64 total) and operations.
 * Most of these operations (compiler built-ins, or  intrinsics) are
 * defined in <altivec.h> and described in the
 * <a href="https://gcc.gnu.org/onlinedocs/">compiler documentation</a>.
 *
 * \note The compiler disables associated <altivec.h> built-ins if the
 * <B>mcpu</B> target does not enable the specific instruction.
 * For example if you compile with <B>-mcpu=power7</B>, some of the
 * wordwise pack, unpack and merge operations useful for conversions
 * are not defined and the equivalent vec_perm and permute control
 * must be used instead. This header will provide the appropriate
 * substitutions, will generate the minimum code, appropriate for the
 * target, and produce correct results.
 *
 * \note Most ppc64le compilers will default to -mcpu=power8 if not
 * specified.
 *
 * Most of these operations are implemented in a single instruction
 * on newer (POWER8/POWER9) processors.
 * This header serves to fill in functional gaps for older
 * (POWER7, POWER8) processors and provides an inline assembler
 * implementation for older compilers that do not
 * provide the built-ins.
 *
 * POWER9 adds useful vector float operations, including:
 * test data class, extract exponent, extract significand,
 * and insert exponent. These operations are common in math library
 * implementations.
 *
 * \note GCC 7.3 defines vector forms of the test data class,
 * extract significand, and extract/insert_exp
 * for float and double.
 * These built-ins are not defined in GCC 6.4. See
 * <a href="https://gcc.gnu.org/onlinedocs/">compiler documentation</a>.
 * These are useful operations and can be implement in a few
 * vector logical instruction for earlier machines.
 *
 * So it is reasonable for this header to provide vector forms
 * of the floating point classification functions
 * (isnormal/subnormal/finite/inf/nan/zero, etc.).
 * These functions can be implemented directly using (one or more)
 * POWER9 instructions, or a few vector logical and integer compare
 * instructions for POWER7/8. Each is comfortably small enough to be
 * in-lined and inherently faster than the equivalent POSIX or compiler
 * built-in runtime scalar functions.
 *
 * This header covers operations that are any of the following:
 *
 * - Implemented in hardware instructions in newer processors,
 * but useful to programmers on slightly older processors
 * (even if the equivalent function requires more instructions).
 * Examples include the floating point test data class,
 * extract exponent, extract significand, and insert exponent
 * operations.
 * - Defined in the OpenPOWER ABI but <I>not</I> yet defined in
 * <altivec.h> provided by available compilers in common use.
 * Examples include vector float even/odd.
 * - Providing special vector float tests for special conditions
 * without generating extraneous floating-point exceptions.
 * This is important for implementing vectorized forms of ISO C99 Math
 * functions.
 * - Commonly used operations, not covered by the ABI or
 * <altivec.h>, and require multiple instructions or
 * are not obvious.
 *
 * \section f32_examples_0_0 Examples
 * For example: using the the classification functions for implementing
 * the math library function sine and cosine.
 * The POSIX specification requires that special input values are
 * processed without raising extraneous floating point exceptions and
 * return specific floating point values in response.
 * For example the sin() function.
 * - If the input <I>value</I> is NaN then return a NaN.
 * - If the input <I>value</I> is +-0.0 then return <I>value</I>.
 * - If the input <I>value</I> is subnormal then return <I>value</I>.
 * - If the input <I>value</I> is +-Inf then return a NaN.
 * - Otherwise compute and return sin(value).
 *
 * The following code example uses functions from this header to
 * address the POSIX requirements for special values input to
 * for a vectorized sinf():
 * \code
vf32_t
test_vec_sinf32 (vf32_t value)
{
  const vf32_t vec_f0 = { 0.0, 0.0, 0.0, 0.0 };
  const vui32_t vec_f32_qnan =
    { 0x7f800001, 0x7fc00000, 0x7fc00000, 0x7fc00000 };
  vf32_t result;
  vb32_t normmask, infmask;

  normmask = vec_isnormalf32 (value);
  if (vec_any_isnormalf32 (value))
    {
      // replace non-normal input values with safe values.
      vf32_t safeval = vec_sel (vec_f0, value, normmask);
      // body of vec_sin(safeval) computation elided for this example.
    }
  else
    result = value;

  // merge non-normal input values back into result
  result = vec_sel (value, result, normmask);
  // Inf input value elements return quiet-nan
  infmask = vec_isinff32 (value);
  result = vec_sel (result, (vf32_t) vec_f32_qnan, infmask);

  return result;
}
 * \endcode
 * The code generated for this fragment runs between 24 (-mcpu=power9)
 * and 40 (-mcpu=power8) instructions. The normal execution path is
 * 14 to 25 instructions respectively.
 *
 * Another example the cos() function.
 * - If the input <I>value</I> is NaN then return a NaN.
 * - If the input <I>value</I> is +-0.0 then return <I>1.0</I>.
 * - If the input <I>value</I> is +-Inf then return a NaN.
 * - Otherwise compute and return cos(value).
 *
 * The following code example uses functions from this header to
 * address the POSIX requirements for special values input to
 * vectorized cosf():
 * \code
vf32_t
test_vec_cosf32 (vf32_t value)
{
  vf32_t result;
  const vf32_t vec_f0 = { 0.0, 0.0, 0.0, 0.0 };
  const vf32_t vec_f1 = { 1.0, 1.0, 1.0, 1.0 };
  const vui32_t vec_f32_qnan =
    { 0x7f800001, 0x7fc00000, 0x7fc00000, 0x7fc00000 };
  vb32_t finitemask, infmask, zeromask;

  finitemask = vec_isfinitef32 (value);
  if (vec_any_isfinitef32 (value))
    {
      // replace non-finite input values with safe values
      vf32_t safeval = vec_sel (vec_f0, value, finitemask);
      // body of vec_sin(safeval) computation elided for this example
    }
  else
    result = value;

  // merge non-finite input values back into result
  result = vec_sel (value, result, finitemask);
  // Set +-0.0 input elements to exactly 1.0 in result
  zeromask = vec_iszerof32 (value);
  result = vec_sel (result, vec_f1, zeromask);
  // Set Inf input elements to quiet-nan in result
  infmask = vec_isinff32 (value);
  result = vec_sel (result, (vf32_t) vec_f32_qnan, infmask);

  return result;
}
 * \endcode
 *
 * Neither example raises floating point exceptions or sets
 * <B>errno</B>, as appropriate for a vector math library.
 *
 * \section f32_perf_0_0 Performance data.
 * High level performance estimates are provided as an aid to function
 * selection when evaluating algorithms. For background on how
 * <I>Latency</I> and <I>Throughput</I> are derived see:
 * \ref perf_data
 */

#include <pveclib/vec_common_ppc.h>
#include <pveclib/vec_int128_ppc.h>

///@cond INTERNAL
static inline vf64_t
vec_vglfsso (float *array, const long long offset0,
	     const long long offset1);
static inline vf64_t
vec_vlxsspx (const signed long long ra, const float *rb);
static inline void
vec_vsstfsso (vf64_t xs, float *array,
	      const long long offset0, const long long offset1);
static inline void
vec_vstxsspx (vf64_t xs, const signed long long ra, float *rb);

static inline vui32_t
vec_mask32_f32sign (void)
{
#if defined (_ARCH_PWR8)
  const vui32_t vones = vec_splat_u32(-1);
  return vec_sl ( vones, vones);
#else
  // masks = {signmask, sigmask, expmask, hidden};
  const vui32_t masks = {0x80000000, 0x007fffff, 0x7f800000, 0x00800000};
  return vec_splat (masks, 0);
#endif
}

static inline vui32_t
vec_mask32_f32mag (void)
{
#if defined (_ARCH_PWR8)
  vui32_t signmsk = vec_mask32_f32sign();
  return vec_nor (signmsk, signmsk);
#else
  // masks = {signmask, sigmask, expmask, hidden};
  const vui32_t masks = {0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff};
  return masks;
#endif
}

static inline vui32_t
vec_mask32_f32sig (void)
{
#if defined (_ARCH_PWR8)
  const vui32_t vones = vec_splat_u32(-1);
  return vec_srwi (vones, 9);
#else
  // masks = {signmask, sigmask, expmask, hidden};
  const vui32_t masks = {0x80000000, 0x007fffff, 0x7f800000, 0x00800000};
  return vec_splat (masks, 1);
#endif
}

static inline vui32_t
vec_mask32_f32exp (void)
{
#if defined (_ARCH_PWR8)
  const vui32_t vones = vec_splat_u32(-1);
  const vui32_t v24 = vec_splat_u32(24-32);
  const vui32_t v1 = vec_splat_u32(1);
  vui32_t vFF = vec_sl (vones, v24);
  return vec_sr (vFF, v1);
#else
  // masks = {signmask, sigmask, expmask, hidden};
  const vui32_t masks = {0x80000000, 0x007fffff, 0x7f800000, 0x00800000};
  return vec_splat (masks, 2);
#endif
}

static inline vui32_t
vec_mask32_f32hidden (void)
{
#if defined (_ARCH_PWR8)
  const vui32_t signmsk = vec_mask32_f32sign();
  // rotate right 8 bits for hidden
  return vec_sld (signmsk, signmsk, 3);
#else
  // masks = {signmask, sigmask, expmask, hidden};
  const vui32_t masks = {0x80000000, 0x007fffff, 0x7f800000, 0x00800000};
  return vec_splat (masks, 3);
#endif
}
 ///@endcond

/*! \brief typedef __vbinary32 to vector of 4 xfloat elements. */
typedef vf32_t __vbinary32;

/** \brief Vector float absolute value.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-7   | 2/cycle  |
 *  |power9   | 2     | 2/cycle  |
 *  |power10  | 1 - 3 | 4/cycle  |
 *
 *  @param vf32x vector float values containing the magnitudes.
 *  @return vector absolute values of 4x float elements of vf32x.
 */
static inline vf32_t
vec_absf32 (vf32_t vf32x)
{
#if _ARCH_PWR7
  /* Requires VSX but eliminates a const load. */
  return vec_abs (vf32x);
#else
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000,
      0x80000000, 0x80000000);
  return (vf32_t)vec_andc ((vui32_t)vf32x, signmask);
#endif
}

/** \brief Return true if all 4x32-bit vector float values are Finite
 *  (Not NaN nor Inf).
 *
 *  A IEEE Binary32 finite value has an exponent between 0x000 and
 *  0x7f0 (a 0x7f8 indicates NaN or Inf).  The significand can be
 *  any value.
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal float compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-20  | 2/cycle  |
 *  |power9   |   6   | 1/cycle  |
 *  |power10  | 6 - 8 | 4/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return an int containing 0 or 1.
 */
static inline int
vec_all_isfinitef32 (vf32_t vf32)
{
  vui32_t tmp;
#if _ARCH_PWR9
  const vui32_t vec_zero = CONST_VINT128_W(0, 0, 0, 0);
#ifdef vec_test_data_class
  tmp = (vui32_t)vec_test_data_class (vf32, 0x70);
#else
  __asm__(
      "xvtstdcsp %x0,%x1,0x70;\n"
      : "=wa" (tmp)
      : "wa" (vf32)
      :);
#endif
  return vec_all_eq(tmp, vec_zero);
#else
  const vui32_t expmask = vec_mask32_f32exp ();

  tmp = vec_and ((vui32_t)vf32, expmask);
  return !vec_any_eq(tmp, expmask);
#endif
}

/** \brief Return true if all 4x32-bit vector float values
 *  are infinity.
 *
 *  A IEEE Binary32 infinity has a exponent of 0x7f8 and significand
 *  of all zeros.
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal float compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-20  | 2/cycle  |
 *  |power9   |   6   | 1/cycle  |
 *  |power10  | 6 - 8 | 4/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return boolean int, true if all 4 float values are infinity
 */
static inline int
vec_all_isinff32 (vf32_t vf32)
{
  vui32_t tmp;

#if _ARCH_PWR9 && !(defined(__clang__) && __clang_major__ < 9)
  const vui32_t vec_ones = CONST_VINT128_W(-1, -1, -1, -1);
#ifdef vec_test_data_class
  tmp = (vui32_t)vec_test_data_class (vf32, 0x30);
#else
  __asm__(
      "xvtstdcsp %x0,%x1,0x30;\n"
      : "=wa" (tmp)
      : "wa" (vf32)
      :);
#endif
  return vec_all_eq(tmp, vec_ones);
#else
  const vui32_t expmask = vec_mask32_f32exp ();
  const vui32_t signmask = vec_mask32_f32sign ();

  tmp = vec_andc ((vui32_t)vf32, signmask);
  return vec_all_eq(tmp, expmask);
#endif
}

/** \brief Return true if all of 4x32-bit vector float
 *  values are NaN.
 *
 *  A IEEE Binary32 NaN value has an exponent between 0x7f8 and
 *  the significand is nonzero.
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal float compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-20  | 2/cycle  |
 *  |power9   |   6   | 1/cycle  |
 *  |power10  | 6 - 8 | 4/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a boolean int, true if all of 4 vector float values are
 *  NaN.
 */
static inline int
vec_all_isnanf32 (vf32_t vf32)
{
  vui32_t tmp;

#if _ARCH_PWR9 && !(defined(__clang__) && __clang_major__ < 9)
  const vui32_t vec_ones = CONST_VINT128_W(-1, -1, -1, -1);
#ifdef vec_test_data_class
  tmp = (vui32_t)vec_test_data_class (vf32, 0x40);
#else
  __asm__(
      "xvtstdcsp %x0,%x1,0x40;\n"
      : "=wa" (tmp)
      : "wa" (vf32)
      :);
#endif
  return vec_all_eq(tmp, vec_ones);
#else
  const vui32_t expmask = vec_mask32_f32exp ();
  const vui32_t signmask = vec_mask32_f32sign ();

  tmp = vec_andc ((vui32_t)vf32, signmask);
  return vec_all_gt(tmp, expmask);
#endif
}

/** \brief Return true if all of 4x32-bit vector float
 *  values are normal (Not NaN, Inf, denormal, or zero).
 *
 *  A IEEE Binary32 normal value has an exponent between 0x008 and
 *  0x7f (a 0x7f8 indicates NaN or Inf).  The significand can be
 *  any value (expect 0 if the exponent is zero).
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal float compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-20  | 1/cycle  |
 *  |power9   |   6   | 1/cycle  |
 *  |power10  | 6 - 8 | 4/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a boolean int, true if all of 4 vector float values are
 *  normal.
 */
static inline int
vec_all_isnormalf32 (vf32_t vf32)
{
  vui32_t tmp;
  const vui32_t vec_zero = CONST_VINT128_W(0, 0, 0, 0);
#if _ARCH_PWR9
#ifdef vec_test_data_class
  tmp = (vui32_t)vec_test_data_class (vf32, 0x7f);
#else
  __asm__(
      "xvtstdcsp %x0,%x1,0x7f;\n"
      : "=wa" (tmp)
      : "wa" (vf32)
      :);
#endif
  return vec_all_eq(tmp, vec_zero);
#else
  const vui32_t expmask = vec_mask32_f32exp ();

  tmp = vec_and ((vui32_t) vf32, expmask);
  return !(vec_any_eq (tmp, expmask) || vec_any_eq(tmp, vec_zero));
#endif
}

/** \brief Return true if all of 4x32-bit vector float
 *  values is subnormal (denormal).
 *
 *  A IEEE Binary32 subnormal has an exponent of 0x000 and a
 *  nonzero significand.
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal float compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-30 | 1/cycle  |
 *  |power9   |   6   | 1/cycle  |
 *  |power10  | 6 - 8 | 4/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a boolean int, true if all of 4 vector float values are
 *  subnormal.
 */
static inline int
vec_all_issubnormalf32 (vf32_t vf32)
{
  vui32_t tmp;

#if _ARCH_PWR9
  const vui32_t vec_ones = CONST_VINT128_W(-1, -1, -1, -1);
#ifdef vec_test_data_class
  tmp = (vui32_t)vec_test_data_class (vf32, 0x03);
#else
  __asm__(
      "xvtstdcsp %x0,%x1,0x03;\n"
      : "=wa" (tmp)
      : "wa" (vf32)
      :);
#endif
  return vec_all_eq(tmp, vec_ones);
#else
  const vui32_t signmask = vec_mask32_f32sign ();
  // Min normal exp same as hidden bit.
  // rotate signmask right 8 bits for hidden
  const vui32_t explow = vec_sld (signmask, signmask, 3);
  const vui32_t vec_zero = vec_splat_u32(0);

  tmp = vec_andc ((vui32_t)vf32, signmask);
  return vec_all_lt (tmp, explow) && vec_all_ne (tmp, vec_zero);
#endif
}

/** \brief Return true if all of 4x32-bit vector float
 *  values are +-0.0.
 *
 *  A IEEE Binary32 zero has an exponent of 0x000 and a
 *  zero significand.
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal float compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-20  | 2/cycle  |
 *  |power9   |   6   | 1/cycle  |
 *  |power10  | 6 - 8 | 4/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a boolean int, true if all of 4 vector float values are
 *  +/- zero.
 */
static inline int
vec_all_iszerof32 (vf32_t vf32)
{
  vui32_t tmp;

#if _ARCH_PWR9
  const vui32_t vec_ones = CONST_VINT128_W(-1, -1, -1, -1);
#ifdef vec_test_data_class
  tmp = (vui32_t)vec_test_data_class (vf32, 0x0c);
#else
  __asm__(
      "xvtstdcsp %x0,%x1,0x0c;\n"
      : "=wa" (tmp)
      : "wa" (vf32)
      :);
#endif
  return vec_all_eq(tmp, vec_ones);
#else
  const vui32_t vec_zero = vec_splat_u32(0);
  const vui32_t signmask = vec_mask32_f32sign ();

  tmp = vec_andc ((vui32_t)vf32, signmask);
  return vec_all_eq(tmp, vec_zero);
#endif
}

/** \brief Return true if any 4x32-bit vector float values are Finite
 *  (Not NaN nor Inf).
 *
 *  A IEEE Binary32 finite value has an exponent between 0x000 and
 *  0x7f0 (a 0x7f8 indicates NaN or Inf).  The significand can be
 *  any value.
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal float compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-20  | 2/cycle  |
 *  |power9   |   6   | 1/cycle  |
 *  |power10  | 6 - 8 | 4/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return an int containing 0 or 1.
 */
static inline int
vec_any_isfinitef32 (vf32_t vf32)
{
  vui32_t tmp;
#if _ARCH_PWR9
  const vui32_t vec_zero = CONST_VINT128_W(0, 0, 0, 0);
#ifdef vec_test_data_class
  tmp = (vui32_t)vec_test_data_class (vf32, 0x70);
#else
  __asm__(
      "xvtstdcsp %x0,%x1,0x70;\n"
      : "=wa" (tmp)
      : "wa" (vf32)
      :);
#endif
  return vec_any_eq(tmp, vec_zero);
#else
  const vui32_t expmask = vec_mask32_f32exp ();

  tmp = vec_and ((vui32_t)vf32, expmask);
  return !vec_all_eq(tmp, expmask);
#endif
}

/** \brief Return true if any 4x32-bit vector float values
 *  are infinity.
 *
 *  A IEEE Binary32 infinity has a exponent of 0x7f8 and significand
 *  of all zeros.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal float compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-20  | 2/cycle  |
 *  |power9   |   6   | 2/cycle  |
 *  |power10  | 6 - 8 | 4/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return boolean int, true if any of 4 float values are infinity
 */
static inline int
vec_any_isinff32 (vf32_t vf32)
{
  vui32_t tmp;

#if _ARCH_PWR9 && !(defined(__clang__) && __clang_major__ < 9)
  const vui32_t vec_ones = CONST_VINT128_W(-1, -1, -1, -1);
#ifdef vec_test_data_class
  tmp = (vui32_t)vec_test_data_class (vf32, 0x30);
#else
  __asm__(
      "xvtstdcsp %x0,%x1,0x30;\n"
      : "=wa" (tmp)
      : "wa" (vf32)
      :);
#endif
  return vec_any_eq(tmp, vec_ones);
#else
  const vui32_t expmask = vec_mask32_f32exp ();
  const vui32_t signmask = vec_mask32_f32sign ();

  tmp = vec_andc ((vui32_t)vf32, signmask);
  return vec_any_eq(tmp, expmask);
#endif
}

/** \brief Return true if any of 4x32-bit vector float
 *  values are NaN.
 *
 *  A IEEE Binary32 NaN value has an exponent between 0x7f8 and
 *  the significand is nonzero.
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal float compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-20  | 2/cycle  |
 *  |power9   |   6   | 2/cycle  |
 *  |power10  | 6 - 8 | 4/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a boolean int, true if any of 4 vector float values are
 *  NaN.
 */
static inline int
vec_any_isnanf32 (vf32_t vf32)
{
  vui32_t tmp;

#if _ARCH_PWR9 && !(defined(__clang__) && __clang_major__ < 9)
  const vui32_t vec_ones = CONST_VINT128_W(-1, -1, -1, -1);
#ifdef vec_test_data_class
  tmp = (vui32_t)vec_test_data_class (vf32, 0x40);
#else
  __asm__(
      "xvtstdcsp %x0,%x1,0x40;\n"
      : "=wa" (tmp)
      : "wa" (vf32)
      :);
#endif
  return vec_any_eq(tmp, vec_ones);
#else
  const vui32_t expmask = vec_mask32_f32exp ();
  const vui32_t signmask = vec_mask32_f32sign ();

  tmp = vec_andc ((vui32_t)vf32, signmask);
  return vec_any_gt(tmp, expmask);
#endif
}

/** \brief Return true if any of 4x32-bit vector float
 *  values are normal (Not NaN, Inf, denormal, or zero).
 *
 *  A IEEE Binary32 normal value has an exponent between 0x008 and
 *  0x7f (a 0x7f8 indicates NaN or Inf).  The significand can be
 *  any value (expect 0 if the exponent is zero).
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal float compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-24 | 1/cycle  |
 *  |power9   |   6   | 1/cycle  |
 *  |power10  | 6 - 8 | 4/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a boolean int, true if any of 4 vector float values are
 *  normal.
 */
static inline int
vec_any_isnormalf32 (vf32_t vf32)
{
  vui32_t tmp;
  const vui32_t vec_zero = CONST_VINT128_W(0, 0, 0, 0);
#if _ARCH_PWR9
#ifdef vec_test_data_class
  tmp = (vui32_t)vec_test_data_class (vf32, 0x7f);
#else
  __asm__(
      "xvtstdcsp %x0,%x1,0x7f;\n"
      : "=wa" (tmp)
      : "wa" (vf32)
      :);
#endif
  return vec_any_eq(tmp, vec_zero);
#else
  const vui32_t expmask = vec_mask32_f32exp ();
  vui32_t res;

  tmp = vec_and ((vui32_t) vf32, expmask);
  res = (vui32_t) vec_nor (vec_cmpeq (tmp, expmask), vec_cmpeq (tmp, vec_zero));

  return vec_any_gt(res, vec_zero);
#endif
}

/** \brief Return true if any of 4x32-bit vector float
 *  values is subnormal (denormal).
 *
 *  A IEEE Binary32 subnormal has an exponent of 0x000 and a
 *  nonzero significand.
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal float compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-18 | 1/cycle  |
 *  |power9   |   6   | 1/cycle  |
 *  |power10  | 6 - 8 | 4/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return if any of 4 vector float values are subnormal.
 */
static inline int
vec_any_issubnormalf32 (vf32_t vf32)
{
  vui32_t tmp;

#if _ARCH_PWR9
  const vui32_t vec_ones = CONST_VINT128_W(-1, -1, -1, -1);
#ifdef vec_test_data_class
  tmp = (vui32_t)vec_test_data_class (vf32, 0x03);
#else
  __asm__(
      "xvtstdcsp %x0,%x1,0x03;\n"
      : "=wa" (tmp)
      : "wa" (vf32)
      :);
#endif
  return vec_any_eq(tmp, vec_ones);
#else
  const vui32_t signmask = vec_mask32_f32sign ();
  // Min normal exp same as hidden bit.
  // rotate signmask right 8 bits for hidden
  const vui32_t explow = vec_sld (signmask, signmask, 3);
  const vui32_t vec_zero = vec_splat_u32(0);
  vui32_t tmpz, tmp2;
  vb32_t vsubnorm;

  tmp2 = vec_andc ((vui32_t)vf32, signmask);
  tmp = (vui32_t) vec_cmplt(tmp2, explow);
  tmpz = (vui32_t) vec_cmpeq (tmp2, vec_zero);
  vsubnorm = (vb32_t ) vec_andc (tmp, tmpz);
  return vec_any_ne(vsubnorm, vec_zero);
#endif
}

/** \brief Return true if any of 4x32-bit vector float
 *  values are +-0.0.
 *
 *  A IEEE Binary32 zero has an exponent of 0x000 and a
 *  zero significand.
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal float compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-20  | 2/cycle  |
 *  |power9   |   6   | 1/cycle  |
 *  |power10  | 6 - 8 | 4/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a boolean int, true if any of 4 vector float values are
 *  +/- zero.
 */
static inline int
vec_any_iszerof32 (vf32_t vf32)
{
  vui32_t tmp;

#if _ARCH_PWR9
  const vui32_t vec_ones = CONST_VINT128_W(-1, -1, -1, -1);
#ifdef vec_test_data_class
  tmp = (vui32_t)vec_test_data_class (vf32, 0x0c);
#else
  __asm__(
      "xvtstdcsp %x0,%x1,0x0c;\n"
      : "=wa" (tmp)
      : "wa" (vf32)
      :);
#endif
  return vec_any_eq(tmp, vec_ones);
#else
  const vui32_t signmask = vec_mask32_f32sign ();
  const vui32_t vec_zero = vec_splat_u32(0);

  tmp = vec_andc ((vui32_t)vf32, signmask);
  return vec_any_eq(tmp, vec_zero);
#endif
}

/** \brief Copy the sign bit from vf32x merged with magnitude from
 *  vf32y and return the resulting vector float values.
 *
 *  \note This operation was patterned after the intrinsic vec_cpsgn
 *  (altivec.h) introduced for POWER7 and VSX. It turns out the
 *  original (GCC 4.9) compiler implementation reversed the operands
 *  and does not match the PowerISA or the Vector Intrinsic Programming
 *  Reference manuals. Subsequent compilers and PVECLIB
 *  implementations replicated this (operand order) error.
 *  This has now been reported as bug against the compilers, which are
 *  in the process of applying fixes and distributing updates.
 *  This version of PVECLIB is updated to match the Vector Intrinsic
 *  Programming Reference. This implementation is independent of the
 *  compilers update status.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-7   | 2/cycle  |
 *  |power9   | 2     | 2/cycle  |
 *  |power10  | 1 - 3 | 4/cycle  |
 *
 *  @param vf32x vector float values containing the sign bits.
 *  @param vf32y vector float values containing the magnitudes.
 *  @return vector float values with magnitude from vf32y and the
 *  sign of vf32x.
 */
static inline vf32_t
vec_copysignf32 (vf32_t vf32x, vf32_t vf32y)
{
#if _ARCH_PWR7
#ifdef PVECLIB_CPSGN_FIXED
  return (vec_cpsgn (vf32x, vf32y));
#else
  vf32_t result;
  __asm__(
      "xvcpsgnsp %x0,%x1,%x2;\n"
      : "=wa" (result)
      : "wa" (vf32x), "wa" (vf32y)
      :);
  return (result);
#endif
#else
  const vui32_t signmask = vec_mask32_f32sign ();
  vf32_t result;

  result = (vf32_t)vec_sel ((vui32_t)vf32y, (vui32_t)vf32x, signmask);
  return (result);
#endif
}

/** \brief Return 4x32-bit vector boolean true values for each float
 *  element that is Finite (Not NaN nor Inf).
 *
 *  A IEEE Binary32 finite value has an exponent between 0x000 and
 *  0x7f0 (a 0x7f8 indicates NaN or Inf).  The significand can be
 *  any value.
 *  Using the vec_cmpeq conditional to generate the predicate mask for
 *  NaN / Inf and then invert this for the finite condition.
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal float compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-15  | 2/cycle  |
 *  |power9   |   5   | 2/cycle  |
 *  |power10  | 4 - 7 | 4/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a vector boolean int, each containing all 0s(false)
 *  or 1s(true).
 */
static inline vb32_t
vec_isfinitef32 (vf32_t vf32)
{
  vb32_t tmp2;
#if defined (_ARCH_PWR9)
#ifdef vec_test_data_class
  tmp2 = vec_test_data_class (vf32, 0x70);
#else
  __asm__(
      "xvtstdcsp %x0,%x1,0x70;\n"
      : "=wa" (tmp2)
      : "wa" (vf32)
      :);
#endif
  return vec_nor (tmp2, tmp2); // vec_not
#else
  const vui32_t expmask = vec_mask32_f32exp ();
  vui32_t tmp;

  tmp = vec_and ((vui32_t)vf32, expmask);
  tmp2 = vec_cmpeq (tmp, expmask);
  return vec_nor (tmp2, tmp2); // vec_not
#endif
}

/** \brief Return 4x32-bit vector boolean true values for each float,
 *  if infinity.
 *
 *  A IEEE Binary32 infinity has a exponent of 0x7f8 and significand
 *  of all zeros.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal float compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-13  | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *  |power10  | 3 - 4 | 4/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a vector boolean int, each containing all 0s(false)
 *  or 1s(true).
 */
static inline vb32_t
vec_isinff32 (vf32_t vf32)
{
  vb32_t result;
#if _ARCH_PWR9  && !(defined(__clang__) && __clang_major__ < 9)
#ifdef vec_test_data_class
  result = vec_test_data_class (vf32, 0x30);
#else
  __asm__(
      "xvtstdcsp %x0,%x1,0x30;\n"
      : "=wa" (result)
      : "wa" (vf32)
      :);
#endif
#else
  const vui32_t expmask = vec_mask32_f32exp ();
  const vui32_t signmask = vec_mask32_f32sign ();
  vui32_t tmp;

  tmp = vec_andc ((vui32_t)vf32, signmask);
  result = vec_cmpeq (tmp, expmask);
#endif
  return (result);
}

/** \brief Return 4x32-bit vector boolean true values, for each float
 *  NaN value.
 *
 *  A IEEE Binary32 NaN value has an exponent between 0x7f8 and
 *  the significand is nonzero.
 *  The sign bit is ignored.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-13  | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *  |power10  | 3 - 4 | 4/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a vector boolean int, each containing all 0s(false)
 *  or 1s(true).
 */
static inline vb32_t
vec_isnanf32 (vf32_t vf32)
{
  vb32_t result;
#if _ARCH_PWR9 && !(defined(__clang__) && __clang_major__ < 9)
#ifdef vec_test_data_class
  result = vec_test_data_class (vf32, 0x40);
#else
  __asm__(
      "xvtstdcsp %x0,%x1,0x40;\n"
      : "=wa" (result)
      : "wa" (vf32)
      :);
#endif
#else
  const vui32_t expmask = vec_mask32_f32exp ();
  const vui32_t signmask = vec_mask32_f32sign ();
  vui32_t tmp2;

  tmp2 = vec_andc ((vui32_t)vf32, signmask);
  result = vec_cmpgt (tmp2, expmask);
#endif
  return (result);
}

/** \brief Return 4x32-bit vector boolean true values, for each float
 *  value, if normal (Not NaN, Inf, denormal, or zero).
 *
 *  A IEEE Binary32 normal value has an exponent between 0x008 and
 *  0x7f (a 0x7f8 indicates NaN or Inf).  The significand can be
 *  any value (expect 0 if the exponent is zero).
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal float compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-15  | 1/cycle  |
 *  |power9   |   5   | 1/cycle  |
 *  |power10  | 3 - 4 | 4/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a vector boolean int, each containing all 0s(false)
 *  or 1s(true).
 */
static inline vb32_t
vec_isnormalf32 (vf32_t vf32)
{
#if _ARCH_PWR9
  vb32_t tmp2;
#ifdef vec_test_data_class
  tmp2 = vec_test_data_class (vf32, 0x7f);
#else
  __asm__(
      "xvtstdcsp %x0,%x1,0x7f;\n"
      : "=wa" (tmp2)
      : "wa" (vf32)
      :);
#endif
  return vec_nor (tmp2, tmp2); // vec_not
#else
  const vui32_t expmask = vec_mask32_f32exp ();
  const vui32_t vec_zero = vec_splat_u32(0);
  vui32_t tmp;

  tmp = vec_and ((vui32_t) vf32, expmask);
  return vec_nor (vec_cmpeq (tmp, expmask), vec_cmpeq (tmp, vec_zero));
#endif
}

/** \brief Return 4x32-bit vector boolean true values, for each float
 *  value that is subnormal (denormal).
 *
 *  A IEEE Binary32 subnormal has an exponent of 0x000 and a
 *  nonzero significand.
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal float compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-16  | 1/cycle  |
 *  |power9   |   3   | 1/cycle  |
 *  |power10  | 3 - 4 | 4/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a vector boolean int, each containing all 0s(false)
 *  or 1s(true).
 */
static inline vb32_t
vec_issubnormalf32 (vf32_t vf32)
{
  vb32_t result;

#if _ARCH_PWR9
#ifdef vec_test_data_class
  result = vec_test_data_class (vf32, 0x03);
#else
  __asm__(
      "xvtstdcsp %x0,%x1,0x03;\n"
      : "=wa" (result)
      : "wa" (vf32)
      :);
#endif
#else
  const vui32_t signmask = vec_mask32_f32sign ();
  // Min normal exp same as hidden bit.
  // rotate signmask right 8 bits for hidden
  const vui32_t explow = vec_sld (signmask, signmask, 3);
  const vui32_t vec_zero = vec_splat_u32(0);
  vui32_t tmp, tmpz, tmp2;

  tmp2 = vec_andc ((vui32_t)vf32, signmask);
  tmp = (vui32_t) vec_cmplt(tmp2, explow);
  tmpz = (vui32_t) vec_cmpeq (tmp2, vec_zero);
  result = (vb32_t) vec_andc (tmp, tmpz);
#endif
  return (result);
}

/** \brief Return 4x32-bit vector boolean true values, for each float
 *  value that is +-0.0.
 *
 *  A IEEE Binary32 zero has an exponent of 0x000 and a
 *  zero significand.
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal float compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-13  | 2/cycle  |
 *  |power9   | 5     | 2/cycle  |
 *  |power10  | 3 - 4 | 4/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a vector boolean int, each containing all 0s(false)
 *  or 1s(true).
 */
static inline vb32_t
vec_iszerof32 (vf32_t vf32)
{
  vb32_t result;
#if _ARCH_PWR9
#ifdef vec_test_data_class
  result = vec_test_data_class (vf32, 0x0c);
#else
  __asm__(
      "xvtstdcsp %x0,%x1,0x0c;\n"
      : "=wa" (result)
      : "wa" (vf32)
      :);
#endif
#else
  const vui32_t signmask = vec_mask32_f32sign ();
  const vui32_t vec_zero = vec_splat_u32(0);
  vui32_t tmp2;

  tmp2 = vec_andc ((vui32_t)vf32, signmask);
  result = vec_cmpeq (tmp2, vec_zero);
#endif
  return (result);
}

/*! \brief Vector Set Bool from Sign, Single Precision.
 *
 *  For each float, propagate the sign bit to all 32-bits of that
 *  word. The result is vector bool int reflecting the sign
 *  bit of each 32-bit float.
 *
 *  The resulting mask can be used in masking and select operations.
 *
 *  \note This operation will set the sign mask regardless of data
 *  class, while the Vector Test Data Class will not distinguish
 *  between +/- NaN.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-9   | 2/cycle  |
 *  |power9   | 2-8   | 2/cycle  |
 *
 *  @param vra Vector float.
 *  @return vector bool int reflecting the sign bits of each
 *  float value.
 */

static inline vb32_t
vec_setb_sp (vf32_t vra)
{
  return vec_setb_sw ((vi32_t) vra);
}

/** \brief Vector Gather-Load 4 Words from scalar Offsets.
 *
 *  For each scalar offset[0,1,2,3], load the word
 *  from the effective address formed by
 *  *(char*)array+offset[0-3]. Merge resulting float single word
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
static inline vf32_t
vec_vgl4fsso (float *array, const long long offset0,
	     const long long offset1, const long long offset2,
	     const long long offset3)
{
  vf32_t result;

#ifdef _ARCH_PWR8
  vui64_t re0, re1, re2, re3;
  re0 = vec_vlxsiwzx (offset0, (unsigned int *) array);
  re1 = vec_vlxsiwzx (offset1, (unsigned int *) array);
  re2 = vec_vlxsiwzx (offset2, (unsigned int *) array);
  re3 = vec_vlxsiwzx (offset3, (unsigned int *) array);
  /* Need to handle endian as the vec_vlxsiwzx result is always left
   * justified in VR, while element [0] may be left or right. */
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  /* Can't use vec_mergeo here as GCC 7 (AT11) and earlier don't
   * support doubleword vec_merge. */
  re0 = vec_xxpermdi (re0, re2, 3);
  re1 = vec_xxpermdi (re1, re3, 3);
  result = (vf32_t) vec_mergee ((vui32_t) re0, (vui32_t) re1);
#else
  re0 = vec_xxpermdi (re0, re2, 0);
  re1 = vec_xxpermdi (re1, re3, 0);
  result = (vf32_t) vec_mergeo ((vui32_t) re0, (vui32_t) re1);
#endif
#else //  _ARCH_PWR7
  vf32_t xte0, xte1, xte2, xte3;
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
  return (vf32_t)result;
}

/** \brief Vector Gather-Load 4 Words from Vector Word Offsets.
 *
 *  For each signed word element [i] of vra, load the float single word
 *  element at *(char*)array+vra[i]. Merge those word elements [0-3]
 *  and return the resulting vector.
 *
 *  \note Signed word offsets are expanded (unpacked) to doublewords
 *  before transfer to GRPs for effective address calculation.
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
vf32_t
vec_vgl4fswo (float *array, vi32_t vra)
{
  vf32_t r;

#ifdef _ARCH_PWR8
  vi64_t off01, off23;

  off01 = vec_vupkhsw (vra);
  off23 = vec_vupklsw (vra);

  r = vec_vgl4fsso (array, off01[0], off01[1], off23[0], off23[1]);
#else
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);
  signed int off0, off1, off2, off3;

  off0 = scalar_extract_uint64_from_high_uint128(gprp) >> 32;
  off1 = (int) scalar_extract_uint64_from_high_uint128(gprp);
  off2 = scalar_extract_uint64_from_low_uint128(gprp) >> 32;
  off3 = (int) scalar_extract_uint64_from_low_uint128(gprp);

  r = vec_vgl4fsso (array, off0, off1, off2, off3);
#endif
  return  r;
}

/** \brief Vector Gather-Load 4 Words from Vector Word Scaled Indexes.
 *
 *  For each signed word element [i] of vra, load the float single word
 *  element at array[vra[i] << scale]. Merge those word elements [0-3]
 *  and return the resulting vector.
 *
 *  \note Signed word indexes are expanded (unpacked) to doublewords
 *  before shifting left (2+scale) bits before transfer to GRPs for
 *  effective address calculation. This converts each index to an
 *  64-bit offset.
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
vf32_t
vec_vgl4fswsx (float *array, vi32_t vra,
		     const unsigned char scale)
{
  vf32_t r;

#ifdef _ARCH_PWR8
  vi64_t off01, off23;
  vi64_t lshift = vec_splats ((long long) (2+ scale));

  off01 = vec_vupkhsw (vra);
  off23 = vec_vupklsw (vra);

  off01 = (vi64_t) __pvec_vsld (off01, (vui64_t) lshift);
  off23 = (vi64_t) __pvec_vsld (off23, (vui64_t) lshift);

  r = vec_vgl4fsso (array, off01[0], off01[1], off23[0], off23[1]);
#else
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);
  signed long long off0, off1, off2, off3;

  off0 = (scalar_extract_uint64_from_high_uint128(gprp) >> 32) << (2+ scale);
  off1 = ((int) scalar_extract_uint64_from_high_uint128(gprp)) << (2+ scale);
  off2 = (scalar_extract_uint64_from_low_uint128(gprp) >> 32) << (2+ scale);
  off3 = ((int) scalar_extract_uint64_from_low_uint128(gprp)) << (2+ scale);

  r = vec_vgl4fsso (array, off0, off1, off2, off3);
#endif
  return  r;
}

/** \brief Vector Gather-Load 4 Words from Vector Word Indexes.
 *
 *  For word element [i] of vra, load the float single word
 *  element at array[vra[i]]. Merge those word elements [0-3]
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
vf32_t
vec_vgl4fswx (float *array, vi32_t vra)
{
  vf32_t r;

#ifdef _ARCH_PWR8
  vi64_t off01, off23;
  vi64_t lshift = vec_splats ((long long) (2));

  off01 = vec_vupkhsw (vra);
  off23 = vec_vupklsw (vra);

  off01 = (vi64_t) __pvec_vsld (off01, (vui64_t) lshift);
  off23 = (vi64_t) __pvec_vsld (off23, (vui64_t) lshift);

  r = vec_vgl4fsso (array, off01[0], off01[1], off23[0], off23[1]);
#else
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);
  signed long long off0, off1, off2, off3;

  off0 = (scalar_extract_uint64_from_high_uint128(gprp) >> 32) << 2;
  off1 = ((int) scalar_extract_uint64_from_high_uint128(gprp)) << 2;
  off2 = (scalar_extract_uint64_from_low_uint128(gprp) >> 32) << 2;
  off3 = ((int) scalar_extract_uint64_from_low_uint128(gprp)) << 2;

  r = vec_vgl4fsso (array, off0, off1, off2, off3);
#endif
  return  r;
}

/** \brief Vector Gather-Load Single Floats from Vector Doubleword Offsets.
 *
 *  For each doubleword element [0-1] of vra, load the float single word
 *  element at *(char*)array+vra[i] expanding them to float double
 *  format. Merge doubleword elements [0,1]
 *  and return the resulting vector.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   12  | 1/cycle  |
 *  |power9   |   11  | 1/cycle  |
 *
 *  @param array Pointer to array of float singles.
 *  @param vra Vector of doubleword (64-bit) byte offsets from &array.
 *  @return vector doubleword elements [0,1] loaded from expanded
 *  float single words at *(char*)array+vra[i].
 */
static inline
vf64_t
vec_vglfsdo (float *array, vi64_t vra)
{
  vf64_t result;

#ifdef _ARCH_PWR8
  result = vec_vglfsso (array, vra[0], vra[1]);
#else
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);

  result = vec_vglfsso (array, scalar_extract_uint64_from_high_uint128(gprp),
		   scalar_extract_uint64_from_low_uint128(gprp));
#endif
  return  result;
}

/** \brief Vector Gather-Load Single Floats from Vector Doubleword Scaled Indexes.
 *
 *  For each doubleword element [0-1] of vra, load the float single word
 *  element at array[vra[i] << scale)]. Merge doubleword elements [0,1]
 *  and return the resulting vector.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 14-23 | 1/cycle  |
 *  |power9   | 13-22 | 1/cycle  |
 *
 *  @param array Pointer to array of float.
 *  @param vra Vector of doubleword indexes from &array.
 *  @param scale 8-bit integer. Indexes are multiplying by
 *  2<sup>scale</sup>.
 *  @return vector doubleword elements [0,1] loaded from the float single
 *  words at array[vra[0,1]<<scale].
 */
static inline
vf64_t
vec_vglfsdsx (float *array, vi64_t vra,
		     const unsigned char scale)
{
  vf64_t r;

#ifdef _ARCH_PWR8
  vi64_t lshift = vec_splats ((long long) (2 + scale));
  vi64_t offset;

  offset = (vi64_t) __pvec_vsld (vra, (vui64_t) lshift);
  r = vec_vglfsso (array, offset[0], offset[1]);
#else
  long long offset0, offset1;
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);
  offset0 = scalar_extract_uint64_from_high_uint128(gprp) << (2 + scale);
  offset1 = scalar_extract_uint64_from_low_uint128(gprp) << (2 + scale);

  r = vec_vglfsso (array, offset0, offset1);
#endif
  return  r;
}

/** \brief Vector Gather-Load Single Floats from Vector Doubleword Indexes.
 *
 *  For each doubleword element [0-1] of vra, load the float single word
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
 *  @param array Pointer to array of float.
 *  @param vra Vector of doubleword indexes from &array.
 *  @return vector doubleword elements [0,1] loaded from float single
 *  words at array[vra[0,1]].
 */
static inline
vf64_t
vec_vglfsdx (float *array, vi64_t vra)
{
  vf64_t r;

#ifdef _ARCH_PWR8
  vi64_t lshift = vec_splats ((long long) 2);
  vi64_t offset;

  offset = (vi64_t) __pvec_vsld (vra, (vui64_t) lshift);
  r = vec_vglfsso (array, offset[0], offset[1]);
#else
  long long offset0, offset1;
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);
  offset0 = scalar_extract_uint64_from_high_uint128(gprp) << 2;
  offset1 = scalar_extract_uint64_from_low_uint128(gprp) << 2;

  r = vec_vglfsso (array, offset0, offset1);
#endif
  return  r;
}

/** \brief Vector Gather-Load Float Single from scalar Offsets.
 *
 *  For each scalar offset[0|1], load the float single element at
 *  *(char*)array+offset[0|1] expanding them to float double format.
 *  Merge doubleword elements [0,1] and return the resulting vector.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   7   | 2/cycle  |
 *  |power9   |  11   | 2/cycle  |
 *
 *  @param array Pointer to array of floats.
 *  @param offset0 Scalar (64-bit) byte offsets from &array.
 *  @param offset1 Scalar (64-bit) byte offsets from &array.
 *  @return vector double containing elements loaded from
 *  *(char*)array+offset0 and *(char*)array+offset1.
 */
static inline vf64_t
vec_vglfsso (float *array, const long long offset0,
	     const long long offset1)
{
  vf64_t re0, re1, result;

  re0 = vec_vlxsspx (offset0, array);
  re1 = vec_vlxsspx (offset1, array);
  /* Need to handle endian as the vec_vlxsspx result is always left
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

/** \brief Vector Load Scalar Single Float Indexed.
 *
 *  Load doubleword[0] of vector <B>xt</B> as a scalar
 *  (double float formatted) single float word
 *  from the effective address formed by <B>rb+ra</B>.
 *  The operand <B>rb</B> is a pointer to an array of float words.
 *  The operand <B>ra</B> is a doubleword integer byte offset
 *  from <B>rb</B>. The result <B>xt</B> is returned as a vf64_t
 *  vector. For best performance <B>rb</B> and <B>ra</B>
 *  should be word aligned (integer multiple of 4).
 *
 *  \note The Left most doubleword is the single float value,
 *  expanded and formated as a double float.
 *  The right most doubleword of vector <B>xt</B> is left
 *  <I>undefined</I> by this operation.
 *
 *  This operation is an alternate form of Vector Load Element
 *  (vec_lde), with the added simplification that data is always left
 *  justified in the vector. Another advantage for Power8 and later,
 *  the lxsspx instruction can load directly into any of the 64 VSRs,
 *  while expanding the single float word value into float double
 *  format, in a single operation.
 *  Both simplify merging elements for gather operations.
 *
 *  \note The lxsspx instruction was introduced in PowerISA 2.07
 *  (POWER8). Power7 and earlier will use lfs[x] and xxpermdi
 *  to move the result from VSR/FPR range to VSR/VR range if needed.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   5   | 2/cycle  |
 *  |power9   |   8   | 2/cycle  |
 *
 *  @param ra const doubleword index (offset/displacement).
 *  @param rb const pointer to an array of floats.
 *  @return The word stored at (ra + rb) is expanded from single to
 *  double float format and loaded into vector doubleword element 0.
 *  Element 1 is undefined.
 */
static inline vf64_t
vec_vlxsspx (const signed long long ra, const float *rb)
{
  vf64_t xt;

#if (defined(__clang__) && __clang_major__ < 8)
  __VEC_U_128 t;

  float *p = (float *)((char *)rb + ra);
  // Splat the load, otherwise some compilers will treat this as dead code.
  t.vf2[0] = t.vf2[1] = *p;
  xt = t.vf2;
#elif _ARCH_PWR8
  if (__builtin_constant_p (ra) && (ra < 32760) && (ra >= -32768)
      && ((ra & 3) == 0))
    {
  #if defined (_ARCH_PWR9)
        __asm__(
  	  "lxssp%X1 %0,%1;"
  	  : "=v" (xt)
  	  : "m" (*(float*)((char *)rb + ra))
  	  : );
  #else
      if (ra == 0)
	{
	  __asm__(
	      "lxsspx %x0,%y1;"
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
	      "lxsspx %x0,%y1;"
	      : "=wa" (xt)
	      : "Z" (*(float *)((char *)rb+rt))
	      : );
	}
  #endif
    } else {
      __asm__(
	  "lxsspx %x0,%y1;"
	  : "=wa" (xt)
	  : "Z" (*(float *)((char *)rb+ra))
	  : );
    }
#else // _ARCH_PWR7
  __VEC_U_128 t;

  float *p = (float *)((char *)rb + ra);
  // Let the compiler generate a Load Float Single Indexed
  t.vf2[0] = t.vf2[1] = *p;
  xt = t.vf2;
#endif
  return xt;
}

/** \brief Vector Scatter-Store 4 Float Singles to Scalar Offsets.
 *
 *  For each float word element [0-3] of xs, store the float
 *  element xs[i] at *(char*)array+offset[i].
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   6   | 1/cycle  |
 *  |power9   |   4   | 2/cycle  |
 *
 *  @param xs Vector float elements to scatter store.
 *  @param array Pointer to array of float words.
 *  @param offset0 Scalar (64-bit) byte offset from &array.
 *  @param offset1 Scalar (64-bit) byte offset from &array.
 *  @param offset2 Scalar (64-bit) byte offset from &array.
 *  @param offset3 Scalar (64-bit) byte offset from &array.
 */
static inline void
vec_vsst4fsso (vf32_t xs, float *array,
	      const long long offset0, const long long offset1,
	      const long long offset2, const long long offset3)
{
  vf32_t xs0, xs1, xs2, xs3;

  xs0 = vec_splat (xs, 0);
  xs1 = vec_splat (xs, 1);
  xs2 = vec_splat (xs, 2);
  xs3 = vec_splat (xs, 3);
  vec_ste (xs0, offset0, array);
  vec_ste (xs1, offset1, array);
  vec_ste (xs2, offset2, array);
  vec_ste (xs3, offset3, array);
}

/** \brief Vector Scatter-Store 4 Float Singles to Vector Word Offsets.
 *
 *  For each float word element [0-3] of xs, store the float
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
 *  @param xs Vector float elements to scatter store.
 *  @param array Pointer to array of float words.
 *  @param vra Vector of signed word (32-bit) byte offsets from &array.
 */
static inline void
vec_vsst4fswo (vf32_t xs, float *array,
	      vi32_t vra)
{
#ifdef _ARCH_PWR8
  vi64_t off01, off23;

  off01 = vec_vupkhsw (vra);
  off23 = vec_vupklsw (vra);

  vec_vsst4fsso (xs, array, off01[0], off01[1], off23[0], off23[1]);
#else
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);
  signed int off0, off1, off2, off3;

  off0 = scalar_extract_uint64_from_high_uint128(gprp) >> 32;
  off1 = (int) scalar_extract_uint64_from_high_uint128(gprp);
  off2 = scalar_extract_uint64_from_low_uint128(gprp) >> 32;
  off3 = (int) scalar_extract_uint64_from_low_uint128(gprp);

  vec_vsst4fsso (xs, array, off0, off1, off2, off3);
#endif
}

/** \brief Vector Scatter-Store 4 Float Singles to Vector Word Indexes.
 *
 *  For each float word element [0-4] of xs, store the float
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
 *  @param xs Vector float elements to scatter store.
 *  @param array Pointer to array of float words.
 *  @param vra Vector of signed word (32-bit) indexes from array.
 *  @param scale 8-bit integer. Indexes are multiplying by
 *  2<sup>scale</sup>.
 */
static inline void
vec_vsst4fswsx (vf32_t xs, float *array,
	      vi32_t vra, const unsigned char scale)
{
#ifdef _ARCH_PWR8
  vi64_t off01, off23;
  vui64_t lshift = vec_splats ((unsigned long long) (2 + scale));

  off01 = vec_vupkhsw (vra);
  off23 = vec_vupklsw (vra);

  off01 = (vi64_t) __pvec_vsld (off01, (vui64_t) lshift);
  off23 = (vi64_t) __pvec_vsld (off23, (vui64_t) lshift);

  vec_vsst4fsso (xs, array, off01[0], off01[1], off23[0], off23[1]);
#else
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);
  signed int off0, off1, off2, off3;

  off0 = (scalar_extract_uint64_from_high_uint128(gprp) >> 32) << (2 + scale);
  off1 = ((int) scalar_extract_uint64_from_high_uint128(gprp)) << (2 + scale);
  off2 = (scalar_extract_uint64_from_low_uint128(gprp) >> 32) << (2 + scale);
  off3 = ((int) scalar_extract_uint64_from_low_uint128(gprp)) << (2 + scale);

  vec_vsst4fsso (xs, array, off0, off1, off2, off3);
#endif
}

/** \brief Vector Scatter-Store 4 Float Singles to Vector Word Indexes.
 *
 *  For each float word element [0-3] of xs, store the float
 *  element xs[i] at *(char*)array[vra[i]].
 *
 *  \note Signed word indexes are expanded (unpacked) to doublewords
 *  before shifting left 2 bits before transfer to GRPs for
 *  effective address calculation. This converts each index to an
 *  64-bit offset.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 12-21 | 1/cycle  |
 *  |power9   | 15-24 | 2/cycle  |
 *
 *  @param xs Vector float elements to scatter store.
 *  @param array Pointer to array of float words.
 *  @param vra Vector of signed word (32-bit) indexes from array.
 */
static inline void
vec_vsst4fswx (vf32_t xs, float *array,
	      vi32_t vra)
{
#ifdef _ARCH_PWR8
  vi64_t off01, off23;
  vui64_t lshift = vec_splats ((unsigned long long) 2);

  off01 = vec_vupkhsw (vra);
  off23 = vec_vupklsw (vra);

  off01 = (vi64_t) __pvec_vsld (off01, (vui64_t) lshift);
  off23 = (vi64_t) __pvec_vsld (off23, (vui64_t) lshift);

  vec_vsst4fsso (xs, array, off01[0], off01[1], off23[0], off23[1]);
#else
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);
  signed int off0, off1, off2, off3;

  off0 = (scalar_extract_uint64_from_high_uint128(gprp) >> 32) << 2;
  off1 = ((int) scalar_extract_uint64_from_high_uint128(gprp)) << 2;
  off2 = (scalar_extract_uint64_from_low_uint128(gprp) >> 32) << 2;
  off3 = ((int) scalar_extract_uint64_from_low_uint128(gprp)) << 2;

  vec_vsst4fsso (xs, array, off0, off1, off2, off3);
#endif
}

/** \brief Vector Scatter-Store Floats Singles to Vector Doubleword Offsets
 *
 *  For each doubleword element [0-1] of vra, store the doubleword float
 *  element xs[i], converted to float single word format,
 *  at *(char*)array+vra[i].
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   8   | 1/cycle  |
 *  |power9   |   9   | 2/cycle  |
 *
 *  @param xs Vector doubleword elements to scatter store
 *  as float single words.
 *  @param array Pointer to array of float words.
 *  @param vra Vector of doubleword (64-bit) byte offsets from &array.
 */
static inline void
vec_vsstfsdo (vf64_t xs, float *array, vi64_t vra)
{
#ifdef _ARCH_PWR8
  vec_vsstfsso (xs, array, vra[0], vra[1]);
#else
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);

  vec_vsstfsso (xs, array,
		scalar_extract_uint64_from_high_uint128(gprp),
		scalar_extract_uint64_from_low_uint128(gprp));
#endif
}

/** \brief Vector Scatter-Store Words to Vector Doubleword Scaled Indexes.
 *
 *  For each doubleword element [0-1] of vra, store the doubleword float
 *  element xs[i], converted to float single word format,
 *  at array[vra[i]<<scale].
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-19 | 1/cycle  |
 *  |power9   | 10-19 | 1/cycle  |
 *
 *  @param xs Vector doubleword elements to scatter store
 *  as float single words.
 *  @param array Pointer to array of float words.
 *  @param vra Vector of doubleword (64-bit) indexes from &array.
 *  @param scale 8-bit integer. Indexes are multiplying by
 *  2<sup>scale</sup>.
 */
static inline void
vec_vsstfsdsx (vf64_t xs, float *array, vi64_t vra,
	      const unsigned char scale)
{
#ifdef _ARCH_PWR8
  vui64_t lshift = vec_splats ((unsigned long long) (2 + scale));
  vui64_t offset;

  offset = (vui64_t) __pvec_vsld (vra, (vui64_t) lshift);
  vec_vsstfsso (xs, array, offset[0], offset[1]);
#else
  long long offset0, offset1;
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);
  offset0 = scalar_extract_uint64_from_high_uint128(gprp) << (2 + scale);
  offset1 = scalar_extract_uint64_from_low_uint128(gprp) << (2 + scale);

  vec_vsstfsso (xs, array, offset0, offset1);
#endif
}

/** \brief Vector Scatter-Store Words to Vector Doubleword Indexes.
 *
 *  For each doubleword element [0-1] of vra, store the doubleword float
 *  element xs[i], converted to float single word format,
 *  at array[vra[i]].
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-19 | 1/cycle  |
 *  |power9   | 10-19 | 1/cycle  |
 *
 *  @param xs Vector doubleword elements to scatter store
 *  as float single words.
 *  @param array Pointer to array of float words.
 *  @param vra Vector of doubleword (64-bit) indexes from &array.
 */
static inline void
vec_vsstfsdx (vf64_t xs, float *array, vi64_t vra)
{
#ifdef _ARCH_PWR8
  vui64_t lshift = vec_splats ((unsigned long long) 2);
  vui64_t offset;

  offset = (vui64_t) __pvec_vsld (vra, (vui64_t) lshift);
  vec_vsstfsso (xs, array, offset[0], offset[1]);
#else
  long long offset0, offset1;
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);
  offset0 = scalar_extract_uint64_from_high_uint128(gprp) << 2;
  offset1 = scalar_extract_uint64_from_low_uint128(gprp) << 2;

  vec_vsstfsso (xs, array, offset0, offset1);
#endif
}

/** \brief Vector Scatter-Store Float Singles to Scalar Offsets.
 *
 *  For each scalar offset[0-1], Store the doubleword element xs[i],
 *  converted to float single word format, at *(char*)array+offset[0|1].
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   3   | 1/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param xs Vector doubleword elements to scatter store
 *  as float single words.
 *  @param array Pointer to array of float words.
 *  @param offset0 Scalar (64-bit) byte offset from &array.
 *  @param offset1 Scalar (64-bit) byte offset from &array.
 */
static inline void
vec_vsstfsso (vf64_t xs, float *array,
	      const long long offset0, const long long offset1)
{
  vf64_t xs0, xs1;

  xs0 = xs;
  // xs1 = vec_xxswapd (xs);
#ifdef _ARCH_PWR7
  xs1 = vec_xxpermdi (xs, xs, 2);
#else
  xs1 = vec_sld (xs0, xs0, 8);
#endif
  /* Need to handle endian as vec_vstxsspx always stores the
   * left doubleword of the VSR, while doubleword element [0] may in
   * the left or right doubleword. */
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  vec_vstxsspx (xs0, offset1, array);
  vec_vstxsspx (xs1, offset0, array);
#else
  vec_vstxsspx (xs0, offset0, array);
  vec_vstxsspx (xs1, offset1, array);
#endif
}

/** \brief Vector Store Scalar Single Float Indexed.
 *
 *  Stores doubleword float element 0 of vector <B>xs</B> as a scalar
 *  float word at the effective address formed by <B>rb+ra</B>. The
 *  operand <B>rb</B> is a pointer to an array of float.
 *  The operand <B>ra</B> is a doubleword integer byte offset
 *  from <B>rb</B>. For best performance <B>rb</B> and <B>ra</B>
 *  should be word aligned (integer multiple of 4).
 *
 *  This operation is an alternate form of vector store element
 *  (vec_ste), with the added simplification that data is always left
 *  justified in the vector. Another advantage for Power8 and later,
 *  the stxsspx instruction can load directly into any of the 64 VSRs.
 *  Both simplify scatter operations.
 *
 *  \note The stxsspx instruction was introduced in PowerISA 2.07
 *  (POWER8). Power7 and earlier will, move the source (xs) from
 *  VSR/VR range to VSR/FPR range if needed, then use stsf[x].
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 0 - 2 | 2/cycle  |
 *  |power9   | 0 - 2 | 4/cycle  |
 *
 *  @param xs vector doubleword element 0 to be stored as single float.
 *  @param ra const doubleword index (offset/displacement).
 *  @param rb const pointer to an array of floats.
 */
static inline void
vec_vstxsspx (vf64_t xs, const signed long long ra, float *rb)
{
#if (defined(__clang__) && __clang_major__ < 8)
  __VEC_U_128 t;
  float *p = (float *)((char *)rb + ra);
  t.vf2 = xs;
  *p = t.vf2[0];
#elif _ARCH_PWR8
  if (__builtin_constant_p (ra) && (ra < 32760) && (ra >= -32768)
      && ((ra & 3) == 0))
    {
#if defined (_ARCH_PWR9)
      __asm__(
	  "stxssp%X0 %1,%0;"
	  : "=m" (*(float*)((char *)rb + ra))
	  : "v" (xs)
	  : );
#else
      if (ra == 0)
	{
	  __asm__(
	      "stxsspx %x1,%y0;"
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
	      "stxsspx %x1,%y0;"
	      : "=Z" (*(float *)((char *)rb+rt))
	      : "wa" (xs)
	      : );
	}
#endif
    } else {
      __asm__(
	  "stxsspx %x1,%y0;"
	  : "=Z" (*(float *)((char *)rb+ra))
	  : "wa" (xs)
	  : );
    }
#else //_ARCH_PWR7
  // Let the compiler generate a Store Float Single Indexed
  __VEC_U_128 t;
  float *p = (float *)((char *)rb + ra);
  t.vf2 = xs;
  *p = t.vf2[0];
#endif
}

/** \brief Vector Insert Exponent Single-Precision
 *
 *  For each word of <B>sig</B> and <B>exp</B>,
 *  merge the sign (bit 0) and significand (bits 9:31) from <B>sig</B>
 *  with the 8-bit exponent from <B>exp</B> (bits 24:31). The exponent
 *  is merged into bits 1:8 of the final result.
 *  The result is returned as a Vector Single-Precision floating point
 *  value.
 *
 *  \note This operation is equivalent to the POWER9 xviexpsp
 *  instruction and the built-in vec_insert_exp. These require a
 *  POWER9-enabled compiler targeting -mcpu=power9 and are not
 *  available for older compilers nor POWER8 and earlier.
 *  This function provides this operation for all VSX-enabled
 *  platforms.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  6-15 | 2/cycle  |
 *  |power9   |   2   | 4/cycle  |
 *  |power10  | 1 - 3 | 4/cycle  |
 *
 *  @param sig Vector unsigned int containing the Sign Bit and 23-bit significand.
 *  @param exp Vector unsigned int containing the 8-bit exponent.
 *  @return a vf32_t value where the exponent bits (1:8) of sig
 *  are replaced from bits 24:31 of exp.
 *
 */
static inline vf32_t
vec_xviexpsp (vui32_t sig, vui32_t exp)
{
  vf32_t result;
#if defined (_ARCH_PWR9) && defined (__VSX__) && (__GNUC__ > 7)
#if defined (vec_insert_exp)
  result = vec_insert_exp (sig, exp);
#else
  __asm__(
      "xviexpsp %x0,%x1,%x2"
      : "=wa" (result)
      : "wa" (sig), "wa" (exp)
      : );
#endif
#else // defined (_ARCH_PWR8)
  const vui32_t expmask = vec_mask32_f32exp ();
  vui32_t tmp;

  tmp = vec_slwi (exp, 23);
  result = (vf32_t) vec_sel ((vui32_t) sig, tmp, expmask);
#endif
  return result;
}

/** \brief Vector Extract Exponent Single-Precision
 *
 *  For each word of <B>vrb</B>,
 *  Extract the single-precision exponent (bits 1:8) and right justify
 *  it to (bits 24:31 of) of the result vector word.
 *  The result is returned as vector unsigned integer value.
 *
 *  \note This operation is equivalent to the POWER9 xvxexpsp
 *  instruction and the built-in vec_extract_exp. These require a
 *  POWER9-enabled compiler targeting -mcpu=power9 and are not
 *  available for older compilers nor POWER8 and earlier.
 *  This function provides this operation for all VSX-enabled
 *  platforms.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  6-15 | 2/cycle  |
 *  |power9   |   2   | 4/cycle  |
 *  |power10  | 1 - 3 | 4/cycle  |
 *
 *  @param vrb vector double value.
 *  @return vector unsigned int containing the 8-bit exponent right justified
 *  in each word
 *
 */
static inline vui32_t
vec_xvxexpsp (vf32_t vrb)
{
  vui32_t result;
#if defined (_ARCH_PWR9) && defined (__VSX__) && (__GNUC__ > 7)
#if defined (vec_extract_exp)
  result = vec_extract_exp (vrb);
#else
  __asm__(
      "xvxexpsp %x0,%x1"
      : "=wa" (result)
      : "wa" (vrb)
      : );
#endif
#else // defined (_ARCH_PWR8)
  const vui32_t signmask = vec_mask32_f32sign ();
  vui32_t tmp;

  tmp = vec_andc ((vui32_t) vrb, signmask);
  result = vec_srwi (tmp, 23);
#endif
  return result;
}

/** \brief Vector Extract Significand Single-Precision
 *
 *  For each word of <B>vrb</B>,
 *  Extract the single-precision significand (bits 0:31) and
 *  restore the implied (hidden) bit (bit 8) if the single-precision
 *  value is normal (not zero, subnormal, Infinity or NaN).
 *  The result is return as vector unsigned int value with
 *  up to 24 bits of significance.
 *
 *  \note This operation is equivalent to the POWER9 xvxsigsp
 *  instruction and the built-in vec_extract_sig. These require a
 *  POWER9-enabled compiler targeting -mcpu=power9 and are not
 *  available for older compilers nor POWER8 and earlier.
 *  This function provides this operation for all VSX-enabled
 *  platforms.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  8-17 | 1/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *  |power10  | 1 - 3 | 4/cycle  |
 *
 *  @param vrb vector double value.
 *  @return vector unsigned int containing the significand.
 *
 */
static inline vui32_t
vec_xvxsigsp (vf32_t vrb)
{
  vui32_t result;
#if defined (_ARCH_PWR9) && defined (__VSX__) && (__GNUC__ > 7)
#if defined (vec_extract_sig)
  result = vec_extract_sig (vrb);
#else
  __asm__(
      "xvxsigsp %x0,%x1"
      : "=wa" (result)
      : "wa" (vrb)
      : );
#endif
#else // defined (_ARCH_PWR8)
  const vui32_t zero = vec_splat_u32(0);
  const vui32_t sigmask = vec_mask32_f32sig ();
#if defined (__VSX__) && (__GNUC__ > 7)
  const vui32_t expmask = vec_mask32_f32exp ();
  const vui32_t hidden = vec_mask32_f32hidden ();
#else
  const vui32_t magmask = vec_mask32_f32mag ();
  const vui32_t expmask = vec_andc (magmask, sigmask);
  const vui32_t hidden = sigmask + 1;
#endif
  vui32_t t128, tmp;
  vui32_t normal;

  // Check if vrb is normal. Normal values need the hidden bit
  // restored to the significand. We use a simpler sequence here as
  // vec_isnormalf32 does more then we need.
  tmp = vec_and ((vui32_t) vrb, expmask);
  normal = vec_nor ((vui32_t) vec_cmpeq (tmp, expmask),
		    (vui32_t) vec_cmpeq (tmp, zero));
  t128 = vec_and ((vui32_t) vrb, sigmask);
  result = (vui32_t) vec_sel (t128, normal, hidden);
#endif
  return result;
}

#endif /* VEC_F32_PPC_H_ */
