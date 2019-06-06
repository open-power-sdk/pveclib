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

/*! \brief typedef __vbinary32 to vector of 4 xfloat elements. */
typedef vf32_t __vbinary32;

/** \brief Vector float absolute value.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-7   | 2/cycle  |
 *  |power9   | 2     | 2/cycle  |
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
      : "wf" (vf32)
      :);
#endif
  return vec_all_eq(tmp, vec_zero);
#else
  const vui32_t expmask = CONST_VINT128_W(0x7f800000, 0x7f800000, 0x7f800000,
					  0x7f800000);
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
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return boolean int, true if all 4 float values are infinity
 */
static inline int
vec_all_isinff32 (vf32_t vf32)
{
  vui32_t tmp;

#if _ARCH_PWR9
  const vui32_t vec_ones = CONST_VINT128_W(-1, -1, -1, -1);
#ifdef vec_test_data_class
  tmp = (vui32_t)vec_test_data_class (vf32, 0x30);
#else
  __asm__(
      "xvtstdcsp %x0,%x1,0x30;\n"
      : "=wa" (tmp)
      : "wf" (vf32)
      :);
#endif
  return vec_all_eq(tmp, vec_ones);
#else
  const vui32_t expmask = CONST_VINT128_W(0x7f800000, 0x7f800000, 0x7f800000,
					  0x7f800000);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000,
					   0x80000000);
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
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a boolean int, true if all of 4 vector float values are
 *  NaN.
 */
static inline int
vec_all_isnanf32 (vf32_t vf32)
{
  vui32_t tmp;

#if _ARCH_PWR9
  const vui32_t vec_ones = CONST_VINT128_W(-1, -1, -1, -1);
#ifdef vec_test_data_class
  tmp = (vui32_t)vec_test_data_class (vf32, 0x40);
#else
  __asm__(
      "xvtstdcsp %x0,%x1,0x40;\n"
      : "=wa" (tmp)
      : "wf" (vf32)
      :);
#endif
  return vec_all_eq(tmp, vec_ones);
#else
  const vui32_t expmask = CONST_VINT128_W(0x7f800000, 0x7f800000, 0x7f800000,
					  0x7f800000);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000,
					   0x80000000);
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
      : "wf" (vf32)
      :);
#endif
  return vec_all_eq(tmp, vec_zero);
#else
  const vui32_t expmask = CONST_VINT128_W(0x7f800000, 0x7f800000, 0x7f800000,
					  0x7f800000);
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
      : "wf" (vf32)
      :);
#endif
  return vec_all_eq(tmp, vec_ones);
#else
  const vui32_t explow = CONST_VINT128_W(0x00800000, 0x00800000, 0x00800000,
					 0x00800000);
  const vui32_t vec_zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000,
					   0x80000000);
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
      : "wf" (vf32)
      :);
#endif
  return vec_all_eq(tmp, vec_ones);
#else
  const vui32_t vec_zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000,
					   0x80000000);
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
      : "wf" (vf32)
      :);
#endif
  return vec_any_eq(tmp, vec_zero);
#else
  const vui32_t expmask = CONST_VINT128_W(0x7f800000, 0x7f800000, 0x7f800000,
					  0x7f800000);
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
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return boolean int, true if any of 4 float values are infinity
 */
static inline int
vec_any_isinff32 (vf32_t vf32)
{
  vui32_t tmp;

#if _ARCH_PWR9
  const vui32_t vec_ones = CONST_VINT128_W(-1, -1, -1, -1);
#ifdef vec_test_data_class
  tmp = (vui32_t)vec_test_data_class (vf32, 0x30);
#else
  __asm__(
      "xvtstdcsp %x0,%x1,0x30;\n"
      : "=wa" (tmp)
      : "wf" (vf32)
      :);
#endif
  return vec_any_eq(tmp, vec_ones);
#else
  const vui32_t expmask = CONST_VINT128_W(0x7f800000, 0x7f800000, 0x7f800000,
					  0x7f800000);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000,
					   0x80000000);
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
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a boolean int, true if any of 4 vector float values are
 *  NaN.
 */
static inline int
vec_any_isnanf32 (vf32_t vf32)
{
  vui32_t tmp;

#if _ARCH_PWR9
  const vui32_t vec_ones = CONST_VINT128_W(-1, -1, -1, -1);
#ifdef vec_test_data_class
  tmp = (vui32_t)vec_test_data_class (vf32, 0x40);
#else
  __asm__(
      "xvtstdcsp %x0,%x1,0x40;\n"
      : "=wa" (tmp)
      : "wf" (vf32)
      :);
#endif
  return vec_any_eq(tmp, vec_ones);
#else
  const vui32_t expmask = CONST_VINT128_W(0x7f800000, 0x7f800000, 0x7f800000,
					  0x7f800000);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000,
					   0x80000000);
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
      : "wf" (vf32)
      :);
#endif
  return vec_any_eq(tmp, vec_zero);
#else
  vui32_t res;
  const vui32_t expmask = CONST_VINT128_W(0x7f800000, 0x7f800000, 0x7f800000,
					  0x7f800000);
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
      : "wf" (vf32)
      :);
#endif
  return vec_any_eq(tmp, vec_ones);
#else
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000,
					   0x80000000);
  const vui32_t explow = CONST_VINT128_W(0x00800000, 0x00800000, 0x00800000,
					 0x00800000);
  const vui32_t vec_zero = CONST_VINT128_W(0, 0, 0, 0);
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
      : "wf" (vf32)
      :);
#endif
  return vec_any_eq(tmp, vec_ones);
#else
  const vui32_t vec_zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000,
					   0x80000000);
  tmp = vec_andc ((vui32_t)vf32, signmask);
  return vec_any_eq(tmp, vec_zero);
#endif
}

/** \brief Copy the sign bit from vf32y merged with magnitude from
 *  vf32x and return the resulting vector float values.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-7   | 2/cycle  |
 *  |power9   | 2     | 2/cycle  |
 *
 *  @param vf32x vector float values containing the magnitudes.
 *  @param vf32y vector float values containing the sign bits.
 *  @return vector float values with magnitude from vf32x and the
 *  sign of vf32y.
 */
static inline vf32_t
vec_copysignf32 (vf32_t vf32x, vf32_t vf32y)
{
#if _ARCH_PWR7
  /* P9 has a 2 cycle xvcpsgnsp and eliminates a const load. */
  return (vec_cpsgn (vf32x, vf32y));
#else
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000,
      0x80000000, 0x80000000);
  vf32_t result;

  result = (vf32_t)vec_sel ((vui32_t)vf32x, (vui32_t)vf32y, signmask);
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
      : "wf" (vf32)
      :);
#endif
  return vec_nor (tmp2, tmp2); // vec_not
#else
  const vui32_t expmask = CONST_VINT128_W(0x7f800000, 0x7f800000, 0x7f800000,
					  0x7f800000);
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
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a vector boolean int, each containing all 0s(false)
 *  or 1s(true).
 */
static inline vb32_t
vec_isinff32 (vf32_t vf32)
{
  vb32_t result;
#if _ARCH_PWR9
#ifdef vec_test_data_class
  result = vec_test_data_class (vf32, 0x30);
#else
  __asm__(
      "xvtstdcsp %x0,%x1,0x30;\n"
      : "=wa" (result)
      : "wf" (vf32)
      :);
#endif
#else
  vui32_t tmp;
  const vui32_t expmask = CONST_VINT128_W(0x7f800000, 0x7f800000, 0x7f800000,
					  0x7f800000);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000,
					   0x80000000);
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
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a vector boolean int, each containing all 0s(false)
 *  or 1s(true).
 */
static inline vb32_t
vec_isnanf32 (vf32_t vf32)
{
  vb32_t result;
#if _ARCH_PWR9
#ifdef vec_test_data_class
  result = vec_test_data_class (vf32, 0x40);
#else
  __asm__(
      "xvtstdcsp %x0,%x1,0x40;\n"
      : "=wa" (result)
      : "wf" (vf32)
      :);
#endif
#else
  vui32_t tmp2;
  const vui32_t expmask = CONST_VINT128_W(0x7f800000, 0x7f800000, 0x7f800000,
					  0x7f800000);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000,
					   0x80000000);
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
      : "wf" (vf32)
      :);
#endif
  return vec_nor (tmp2, tmp2); // vec_not
#else
  const vui32_t expmask = CONST_VINT128_W(0x7f800000, 0x7f800000, 0x7f800000,
					  0x7f800000);
  const vui32_t veczero = CONST_VINT128_W(0, 0, 0, 0);
  vui32_t tmp;

  tmp = vec_and ((vui32_t) vf32, expmask);
  return vec_nor (vec_cmpeq (tmp, expmask), vec_cmpeq (tmp, veczero));
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
      : "wf" (vf32)
      :);
#endif
#else
  vui32_t tmp, tmpz, tmp2;
  const vui32_t explow = CONST_VINT128_W(0x00800000, 0x00800000, 0x00800000,
					 0x00800000);
  const vui32_t vec_zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000,
					   0x80000000);
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
      : "wf" (vf32)
      :);
#endif
#else
  vui32_t tmp2;
  const vui32_t vec_zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000,
					   0x80000000);
  tmp2 = vec_andc ((vui32_t)vf32, signmask);
  result = vec_cmpeq (tmp2, vec_zero);
#endif
  return (result);
}

#endif /* VEC_F32_PPC_H_ */
