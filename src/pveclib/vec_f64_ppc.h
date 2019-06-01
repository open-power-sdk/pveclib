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

 vec_f64_ppc.h

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Apr 12, 2016
 */

#ifndef VEC_F64_PPC_H_
#define VEC_F64_PPC_H_

#include <pveclib/vec_common_ppc.h>
#include <pveclib/vec_int64_ppc.h>
/*!
 * \file  vec_f64_ppc.h
 * \brief Header package containing a collection of 128-bit SIMD
 * operations over 64-bit double-precision floating point elements.
 *
 * Many vector double-precision (64-bit float) operations are
 * implemented with PowerISA-2.06 Vector Scalar Extended (VSX)
 * (POWER7 and later) instructions. Most VSX instructions provide
 * access to 64 combined scalar/vector registers.
 * PowerISA-3.0 (POWER9) provides additional vector double operations:
 * convert with round, convert to/from integer, insert/extract exponent
 * and significand, and test data class.
 * Most of these operations (compiler built-ins, or intrinsics) are
 * defined in <altivec.h> and described in the
 * <a href="https://gcc.gnu.org/onlinedocs/">compiler documentation</a>.
 *
 * \note The compiler disables associated <altivec.h> built-ins if the
 * <B>mcpu</B> target does not enable the specific instruction.
 * For example if you compile with <B>-mcpu=power8</B>, the
 * double-precision vector converts, insert/extract and test data class
 * built-ins are are not defined.
 * This header provides the appropriate substitutions,
 * will generate the minimum code, appropriate for the target,
 * and produce correct results.
 *
 * \note Most ppc64le compilers will default to -mcpu=power8 if not
 * specified.
 *
 * \note GCC 7.3 defines vector forms of the test data class,
 * extract significand, and extract/insert_exp
 * for float and double.
 * These built-ins are not defined in GCC 6.4. See
 * <a href="https://gcc.gnu.org/onlinedocs/">compiler documentation</a>.
 * These are useful operations and can be implemented in a few
 * vector logical instructions for earlier machines.
 *
 * So it is reasonable for this header to provide vector forms
 * of the double-precision floating point classification functions
 * (isnormal/subnormal/finite/inf/nan/zero, etc.).
 * These functions can be implemented directly using (one or more)
 * POWER9 instructions, or a few vector logical and integer compare
 * instructions for POWER7/8. Each is comfortably small enough to be
 * in-lined and inherently faster than the equivalent POSIX or compiler
 * built-in runtime scalar functions.
 *
 * Most of these operations are implemented in a few instructions
 * on newer (POWER7/POWER8/POWER9) processors.
 * This header serves to fill in functional gaps for older
 * (POWER7, POWER8) processors and provides an inline assembler
 * implementation for older compilers that do not
 * provide the built-ins.
 *
 * This header covers operations that are any of the following:
 *
 * - Implemented in hardware instructions in newer processors,
 * but useful to programmers on slightly older processors
 * (even if the equivalent function requires more instructions).
 * - Defined in the OpenPOWER ABI but <I>not</I> yet defined in
 * <altivec.h> provided by available compilers in common use.
 * Examples include vector double even/odd conversions.
 * - Providing special vector double tests for special conditions
 * without generating extraneous floating-point exceptions.
 * This is important for implementing vectorized forms of ISO C99 Math
 * functions. Examples include vector double isnan, isinf, etc.
 * - Commonly used operations, not covered by the ABI or <altivec.h>,
 * and require multiple instructions or are not obvious.
 * For example, converts that change element size and imply
 * converting two vectors into one vector of smaller elements, or
 * one vector into two vectors of larger elements.
 * Another example is the special case of packing/unpacking an IBM long
 * double between a pair of floating-point registers (FPRs) and a
 * single vector register (VR).
 *
 * \section f64_examples_0_0 Examples
 * For example: using the the classification functions for implementing
 * the math library function sine and cosine.
 * The POSIX specification requires that special input values are
 * processed without raising extraneous floating point exceptions and
 * return specific floating point values in response.
 * For example, the sin() function.
 * - If the input <I>value</I> is NaN then return a NaN.
 * - If the input <I>value</I> is +-0.0 then return <I>value</I>.
 * - If the input <I>value</I> is subnormal then return <I>value</I>.
 * - If the input <I>value</I> is +-Inf then return a quiet-NaN.
 * - Otherwise compute and return sin(value).
 *
 * The following code example uses functions from this header to
 * address the POSIX requirements for special values input to
 * for a vectorized sinf():
 * \code
vf64_t
test_vec_sinf64 (vf64_t value)
{
  const vf64_t vec_f0 = { 0.0, 0.0 };
  const vui64_t vec_f64_qnan =
    { 0x7ff8000000000000, 0x7ff8000000000000 };
  vf64_t result;
  vb64_t normmask, infmask;

  normmask = vec_isnormalf64 (value);
  if (vec_any_isnormalf64 (value))
    {
      // replace non-normal input values with safe values.
      vf64_t safeval = vec_sel (vec_f0, value, normmask);
      // body of vec_sin(safeval) computation elided for this example.
    }
  else
    result = value;

  // merge non-normal input values back into result
  result = vec_sel (value, result, normmask);
  // Inf input value elements return quiet-nan.
  infmask = vec_isinff64 (value);
  result = vec_sel (result, (vf64_t) vec_f64_qnan, infmask);

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
 * - If the input <I>value</I> is +-Inf then return a quiet-NaN.
 * - Otherwise compute and return cos(value).
 *
 * The following code example uses functions from this header to
 * address the POSIX requirements for special values input to
 * vectorized cosf():
 * \code
vf64_t
test_vec_cosf64 (vf64_t value)
{
  vf64_t result;
  const vf64_t vec_f0 = { 0.0, 0.0 };
  const vf64_t vec_f1 = { 1.0, 1.0 };
  const vui64_t vec_f64_qnan =
    { 0x7ff8000000000000, 0x7ff8000000000000 };
  vb64_t finitemask, infmask, zeromask;

  finitemask = vec_isfinitef64 (value);
  if (vec_any_isfinitef64 (value))
    {
      // replace non-finite input values with safe values.
      vf64_t safeval = vec_sel (vec_f0, value, finitemask);
      // body of vec_sin(safeval) computation elided for this example.
    }
  else
    result = value;

  // merge non-finite input values back into result
  result = vec_sel (value, result, finitemask);
  // Set +-0.0 input elements to exactly 1.0 in result.
  zeromask = vec_iszerof64 (value);
  result = vec_sel (result, vec_f1, zeromask);
  // Set Inf input elements to quiet-nan in result.
  infmask = vec_isinff64 (value);
  result = vec_sel (result, (vf64_t) vec_f64_qnan, infmask);

  return result;
}
 * \endcode
 *
 * Neither example raises floating point exceptions or sets
 * <B>errno</B>, as appropriate for a vector math library.
 *
 * \section f64_perf_0_0 Performance data.
 * High level performance estimates are provided as an aid to function
 * selection when evaluating algorithms. For background on how
 * <I>Latency</I> and <I>Throughput</I> are derived see:
 * \ref perf_data
 */

/** \brief Vector double absolute value.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-7   | 2/cycle  |
 *  |power9   | 2     | 2/cycle  |
 *
 *  @param vf64x vector double values containing the magnitudes.
 *  @return vector double absolute values of vf64x.
 */
static inline vf64_t
vec_absf64 (vf64_t vf64x)
{
#if _ARCH_PWR7
  /* Requires VSX but eliminates a const load. */
  return vec_abs (vf64x);
#else
  const vui32_t signmask = CONST_VINT128_DW (0x8000000000000000UL,
                                             0x8000000000000000UL);
  return (vf64_t)vec_andc ((vui32_t)vf64x, signmask);
#endif
}

/** \brief Return true if all 2x64-bit vector double values are Finite
 *  (Not NaN nor Inf).
 *
 *  A IEEE Binary64 finite value has an exponent between 0x000 and
 *  0x7fe (a 0x7ff indicates NaN or Inf).  The significand can be
 *  any value.
 *  The sign bit is ignored.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-20  | 2/cycle  |
 *  |power9   |   6   | 1/cycle  |
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal __binary64 compare can.
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return an int containing 0 or 1.
 */
static inline int
vec_all_isfinitef64 (vf64_t vf64)
{
  vui64_t tmp;
#if _ARCH_PWR9
  const vui64_t vec_zero = CONST_VINT128_DW (0, 0);
#ifdef vec_test_data_class
  tmp = (vui64_t)vec_test_data_class (vf64, 0x70);
#else
  __asm__(
      "xvtstdcdp %x0,%x1,0x70;\n"
      : "=wa" (tmp)
      : "wf" (vf64)
      :);
#endif
  return vec_all_eq(tmp, vec_zero);
#else
  const vui64_t expmask = CONST_VINT128_DW (0x7ff0000000000000UL,
					    0x7ff0000000000000UL);
  tmp = vec_and ((vui64_t)vf64, expmask);
  return !vec_cmpud_any_eq(tmp, expmask);
#endif
}

/** \brief Return true if all 2x64-bit vector double values
 *  are infinity.
 *
 *  A IEEE Binary64 infinity has a exponent of 0x7ff and significand
 *  of all zeros.
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal double compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-20  | 2/cycle  |
 *  |power9   |   6   | 1/cycle  |
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return boolean int, true if all 2 double values are infinity
 */
static inline int
vec_all_isinff64 (vf64_t vf64)
{
  vui64_t tmp;

#if _ARCH_PWR9
  const vui64_t vec_ones = CONST_VINT128_DW (-1, -1);
#ifdef vec_test_data_class
  tmp = (vui64_t)vec_test_data_class (vf64, 0x30);
#else
  __asm__(
      "xvtstdcdp %x0,%x1,0x30;\n"
      : "=wa" (tmp)
      : "wf" (vf64)
      :);
#endif
  return vec_all_eq(tmp, vec_ones);
#else
  const vui64_t signmask = CONST_VINT128_DW (0x8000000000000000UL,
					     0x8000000000000000UL);
  const vui64_t expmask = CONST_VINT128_DW (0x7ff0000000000000UL,
					    0x7ff0000000000000UL);
  tmp = vec_andc ((vui64_t)vf64, signmask);
  return vec_cmpud_all_eq(tmp, expmask);
#endif
}

/** \brief Return true if all 2x64-bit vector double
 *  values are NaN.
 *
 *  A IEEE Binary64 NaN value has an exponent between 0x7ff and
 *  the significand is nonzero.
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal double compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-20  | 2/cycle  |
 *  |power9   |   6   | 1/cycle  |
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return a boolean int, true if all 2 vector double values are
 *  NaN.
 */
static inline int
vec_all_isnanf64 (vf64_t vf64)
{
  vui64_t tmp;

#if _ARCH_PWR9
  const vui64_t vec_ones = CONST_VINT128_DW (-1, -1);
#ifdef vec_test_data_class
  tmp = (vui64_t)vec_test_data_class (vf64, 0x40);
#else
  __asm__(
      "xvtstdcdp %x0,%x1,0x40;\n"
      : "=wa" (tmp)
      : "wf" (vf64)
      :);
#endif
  return vec_all_eq(tmp, vec_ones);
#else
  const vui64_t signmask = CONST_VINT128_DW (0x8000000000000000UL,
					     0x8000000000000000UL);
  const vui64_t expmask = CONST_VINT128_DW (0x7ff0000000000000UL,
					    0x7ff0000000000000UL);
  tmp = vec_andc ((vui64_t)vf64, signmask);
  return vec_cmpud_all_gt(tmp, expmask);
#endif
}

/** \brief Return true if all 2x64-bit vector double
 *  values are normal (Not NaN, Inf, denormal, or zero).
 *
 *  A IEEE Binary64 normal value has an exponent between 0x001 and
 *  0x7fe (a 0x7ff indicates NaN or Inf).  The significand can be
 *  any value (expect 0 if the exponent is zero).
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal double compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-28 | 1/cycle  |
 *  |power9   |   6   | 1/cycle  |
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return a boolean int, true if all 2 vector double values are
 *  normal.
 */
static inline int
vec_all_isnormalf64 (vf64_t vf64)
{
  vui64_t tmp;
  const vui64_t vec_zero = CONST_VINT128_DW (0, 0);
#if _ARCH_PWR9
#ifdef vec_test_data_class
  tmp = (vui64_t)vec_test_data_class (vf64, 0x7f);
#else
  __asm__(
      "xvtstdcdp %x0,%x1,0x7f;\n"
      : "=wa" (tmp)
      : "wf" (vf64)
      :);
#endif
  return vec_all_eq(tmp, vec_zero);
#else
  const vui64_t expmask = CONST_VINT128_DW (0x7ff0000000000000UL,
					    0x7ff0000000000000UL);
  tmp = vec_and ((vui64_t) vf64, expmask);
  return !(vec_cmpud_any_eq (tmp, expmask)
        || vec_cmpud_any_eq (tmp, vec_zero));
#endif
}

/** \brief Return true if all 2x64-bit vector double
 *  values are subnormal (denormal).
 *
 *  A IEEE Binary64 subnormal has an exponent of 0x000 and a
 *  nonzero significand.
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal double compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-30 | 1/cycle  |
 *  |power9   |   6   | 1/cycle  |
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return a boolean int, true if all of 2 vector double values are
 *  subnormal.
 */
static inline int
vec_all_issubnormalf64 (vf64_t vf64)
{
  vui64_t tmp;

#if _ARCH_PWR9
  const vui64_t vec_ones = CONST_VINT128_DW (-1, -1);
#ifdef vec_test_data_class
  tmp = (vui64_t)vec_test_data_class (vf64, 0x03);
#else
  __asm__(
      "xvtstdcdp %x0,%x1,0x03;\n"
      : "=wa" (tmp)
      : "wf" (vf64)
      :);
#endif
  return vec_all_eq(tmp, vec_ones);
#else
  const vui64_t explow = CONST_VINT128_DW (0x0010000000000000,
					   0x0010000000000000);
  const vui64_t signmask = CONST_VINT128_DW (0x8000000000000000UL,
					     0x8000000000000000UL);
  const vui64_t vec_zero = CONST_VINT128_DW (0, 0);

  tmp = vec_andc ((vui64_t)vf64, signmask);
  return vec_cmpud_all_lt (tmp, explow) && vec_cmpud_all_ne (tmp, vec_zero);
#endif
}

/** \brief Return true if all 2x64-bit vector double
 *  values are +-0.0.
 *
 *  A IEEE Binary64 zero has an exponent of 0x000 and a
 *  zero significand.
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal double compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-20  | 2/cycle  |
 *  |power9   |   6   | 1/cycle  |
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return a boolean int, true if all 2 vector double values are
 *  +/- zero.
 */
static inline int
vec_all_iszerof64 (vf64_t vf64)
{
  vui64_t tmp;

#if _ARCH_PWR9
  const vui64_t vec_ones = CONST_VINT128_DW (-1, -1);
#ifdef vec_test_data_class
  tmp = (vui64_t)vec_test_data_class (vf64, 0x0c);
#else
  __asm__(
      "xvtstdcdp %x0,%x1,0x0c;\n"
      : "=wa" (tmp)
      : "wf" (vf64)
      :);
#endif
  return vec_all_eq(tmp, vec_ones);
#else
  const vui64_t signmask = CONST_VINT128_DW (0x8000000000000000UL,
					     0x8000000000000000UL);
  const vui64_t vec_zero = CONST_VINT128_DW (0, 0);

  tmp = vec_andc ((vui64_t)vf64, signmask);
  return vec_all_eq((vui32_t)tmp, (vui32_t)vec_zero);
#endif
}

/** \brief Return true if any of 2x64-bit vector double values are
 *  Finite (Not NaN nor Inf).
 *
 *  A IEEE Binary64 finite value has an exponent between 0x000 and
 *  0x7fe (a 0x7ff indicates NaN or Inf).  The significand can be
 *  any value.
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal double compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-20  | 2/cycle  |
 *  |power9   |   6   | 1/cycle  |
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return an int containing 0 or 1.
 */
static inline int
vec_any_isfinitef64 (vf64_t vf64)
{
  vui64_t tmp;
#if _ARCH_PWR9
  const vui64_t vec_zero = CONST_VINT128_DW (0, 0);
#ifdef vec_test_data_class
  tmp = (vui64_t)vec_test_data_class (vf64, 0x70);
#else
  __asm__(
      "xvtstdcdp %x0,%x1,0x70;\n"
      : "=wa" (tmp)
      : "wf" (vf64)
      :);
#endif
  return vec_any_eq(tmp, vec_zero);
#else
  const vui64_t expmask = CONST_VINT128_DW (0x7ff0000000000000UL,
					    0x7ff0000000000000UL);
  tmp = vec_and ((vui64_t)vf64, expmask);
  return !vec_cmpud_all_eq(tmp, expmask);
#endif
}

/** \brief Return true if any of 2x64-bit vector double values
 *  are infinity.
 *
 *  A IEEE Binary64 infinity has a exponent of 0x7ff and significand
 *  of all zeros.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal double compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-20  | 2/cycle  |
 *  |power9   |   6   | 1/cycle  |
 *
 *  @param vf64 a vector of __binary32 values.
 *  @return boolean int, true if any of 2 double values are infinity
 */
static inline int
vec_any_isinff64 (vf64_t vf64)
{
  vui64_t tmp;

#if _ARCH_PWR9
  const vui64_t vec_ones = CONST_VINT128_DW (-1, -1);
#ifdef vec_test_data_class
  tmp = (vui64_t)vec_test_data_class (vf64, 0x30);
#else
  __asm__(
      "xvtstdcdp %x0,%x1,0x30;\n"
      : "=wa" (tmp)
      : "wf" (vf64)
      :);
#endif
  return vec_any_eq(tmp, vec_ones);
#else
  const vui64_t expmask = CONST_VINT128_DW (0x7ff0000000000000UL,
					    0x7ff0000000000000UL);
  const vui64_t signmask = CONST_VINT128_DW (0x8000000000000000UL,
					     0x8000000000000000UL);
  tmp = vec_andc ((vui64_t)vf64, signmask);
  return vec_cmpud_any_eq(tmp, expmask);
#endif
}

/** \brief Return true if any of 2x64-bit vector double
 *  values are NaN.
 *
 *  A IEEE Binary64 NaN value has an exponent between 0x7ff and
 *  the significand is nonzero.
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal double compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-20  | 2/cycle  |
 *  |power9   |   6   | 1/cycle  |
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return a boolean int, true if any of 2 vector double values are
 *  NaN.
 */
static inline int
vec_any_isnanf64 (vf64_t vf64)
{
  vui64_t tmp;

#if _ARCH_PWR9
  const vui64_t vec_ones = CONST_VINT128_DW (-1, -1);
#ifdef vec_test_data_class
  tmp = (vui64_t)vec_test_data_class (vf64, 0x40);
#else
  __asm__(
      "xvtstdcdp %x0,%x1,0x40;\n"
      : "=wa" (tmp)
      : "wf" (vf64)
      :);
#endif
  return vec_any_eq(tmp, vec_ones);
#else
  const vui64_t signmask = CONST_VINT128_DW (0x8000000000000000UL,
					     0x8000000000000000UL);
  const vui64_t expmask = CONST_VINT128_DW (0x7ff0000000000000UL,
					    0x7ff0000000000000UL);
  tmp = vec_andc ((vui64_t)vf64, signmask);
  return vec_cmpud_any_gt(tmp, expmask);
#endif
}

/** \brief Return true if any of 2x64-bit vector double
 *  values are normal (Not NaN, Inf, denormal, or zero).
 *
 *  A IEEE Binary64 normal value has an exponent between 0x001 and
 *  0x7fe (a 0x7ff indicates NaN or Inf).  The significand can be
 *  any value (expect 0 if the exponent is zero).
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal double compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-20  | 1/cycle  |
 *  |power9   |   6   | 1/cycle  |
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return a boolean int, true if any of 2 vector double values are
 *  normal.
 */
static inline int
vec_any_isnormalf64 (vf64_t vf64)
{
  const vui64_t vec_zero = CONST_VINT128_DW (0, 0);
  vui64_t tmp;
#if _ARCH_PWR9
#ifdef vec_test_data_class
  tmp = (vui64_t)vec_test_data_class (vf64, 0x7f);
#else
  __asm__(
      "xvtstdcdp %x0,%x1,0x7f;\n"
      : "=wa" (tmp)
      : "wf" (vf64)
      :);
#endif
  return vec_any_eq(tmp, vec_zero);
#else
  vui64_t res;
  const vui64_t expmask = CONST_VINT128_DW (0x7ff0000000000000UL,
					    0x7ff0000000000000UL);

  tmp = vec_and ((vui64_t) vf64, expmask);
  res = (vui64_t) vec_nor (vec_cmpequd (tmp, expmask),
			   vec_cmpequd (tmp, vec_zero));
  return vec_cmpud_any_gt (res, vec_zero);
#endif
}

/** \brief Return true if any of 2x64-bit vector double
 *  values is subnormal (denormal).
 *
 *  A IEEE Binary64 subnormal has an exponent of 0x000 and a
 *  nonzero significand.
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal double compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-18 | 1/cycle  |
 *  |power9   |   6   | 1/cycle  |
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return true if any of 2 vector double values are subnormal.
 */
static inline int
vec_any_issubnormalf64 (vf64_t vf64)
{
  vui64_t tmp;

#if _ARCH_PWR9
  const vui64_t vec_ones = CONST_VINT128_DW (-1, -1);
#ifdef vec_test_data_class
  tmp = (vui64_t)vec_test_data_class (vf64, 0x03);
#else
  __asm__(
      "xvtstdcdp %x0,%x1,0x03;\n"
      : "=wa" (tmp)
      : "wf" (vf64)
      :);
#endif
  return vec_any_eq(tmp, vec_ones);
#else
  const vui64_t signmask = CONST_VINT128_DW (0x8000000000000000UL,
					     0x8000000000000000UL);
  const vui64_t minnorm = CONST_VINT128_DW (0x0010000000000000UL,
					    0x0010000000000000UL);
  const vui64_t vec_zero = CONST_VINT128_DW (0, 0);
  vui64_t tmpz, tmp2, vsubnorm;

  tmp2 = vec_andc ((vui64_t)vf64, signmask);
  tmp = (vui64_t) vec_cmpltud(tmp2, minnorm);
  tmpz = (vui64_t) vec_cmpequd (tmp2, vec_zero);
  vsubnorm = vec_andc (tmp, tmpz);
  return vec_cmpud_any_ne(vsubnorm, vec_zero);
#endif
}

/** \brief Return true if any of 2x64-bit vector double
 *  values are +-0.0.
 *
 *  A IEEE Binary64 zero has an exponent of 0x000 and a
 *  zero significand.
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal double compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-20  | 2/cycle  |
 *  |power9   |   6   | 1/cycle  |
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return a boolean int, true if any of 2 vector double values are
 *  +/- zero.
 */
static inline int
vec_any_iszerof64 (vf64_t vf64)
{
  vui64_t tmp;

#if _ARCH_PWR9
  const vui64_t vec_ones = CONST_VINT128_DW (-1, -1);
#ifdef vec_test_data_class
  tmp = (vui64_t)vec_test_data_class (vf64, 0x0c);
#else
  __asm__(
      "xvtstdcdp %x0,%x1,0x0c;\n"
      : "=wa" (tmp)
      : "wf" (vf64)
      :);
#endif
  return vec_any_eq(tmp, vec_ones);
#else
  const vui64_t signmask = CONST_VINT128_DW (0x8000000000000000UL,
					     0x8000000000000000UL);
  const vui64_t vec_zero = CONST_VINT128_DW (0, 0);
  tmp = vec_andc ((vui64_t)vf64, signmask);
  return vec_cmpud_any_eq(tmp, vec_zero);
#endif
}

/** \brief Copy the sign bit from vf64y merged with magnitude from
 *  vf64x and return the resulting vector double values.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-7   | 2/cycle  |
 *  |power9   | 2     | 2/cycle  |
 *
 *  @param vf64x vector double values containing the magnitudes.
 *  @param vf64y vector double values containing the sign bits.
 *  @return vector double values with magnitude from vf64x and the
 *  sign of vf64y.
 */
static inline vf64_t
vec_copysignf64 (vf64_t vf64x , vf64_t vf64y)
{
#if _ARCH_PWR7
  /* P9 has a 2 cycle xvcpsgndp and eliminates a const load. */
	return (vec_cpsgn (vf64x, vf64y));
#else
	const vui32_t signmask  = CONST_VINT128_W(0x80000000, 0, 0x80000000, 0);
	vf64_t result;

	result  = (vf64_t)vec_sel ((vui32_t)vf64x, (vui32_t)vf64y, signmask);
	return (result);
#endif
}

/** \brief Return 2x64-bit vector boolean true values for each double
 *  element that is Finite (Not NaN nor Inf).
 *
 *  A IEEE Binary64 finite value has an exponent between 0x000 and
 *  0x7fe (a 0x7ff indicates NaN or Inf).  The significand can be
 *  any value.
 *
 *  Using the vec_cmpeq conditional to generate the predicate mask for
 *  NaN / Inf and then invert this for the finite condition.
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal double compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-15  | 2/cycle  |
 *  |power9   |   5   | 2/cycle  |
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return a vector boolean long, each containing all 0s(false)
 *  or 1s(true).
 */
static inline vb64_t
vec_isfinitef64 (vf64_t vf64)
{
  vb64_t tmp2;
#if defined (_ARCH_PWR9)
#ifdef vec_test_data_class
  tmp2 = vec_test_data_class (vf64, 0x70);
#else
  __asm__(
      "xvtstdcdp %x0,%x1,0x70;\n"
      : "=wa" (tmp2)
      : "wf" (vf64)
      :);
#endif
  return vec_nor (tmp2, tmp2); // vec_not
#else
  const vui64_t expmask = CONST_VINT128_DW (0x7ff0000000000000UL,
					    0x7ff0000000000000UL);
  vui64_t tmp;

  tmp = vec_and ((vui64_t)vf64, expmask);
  tmp2 = vec_cmpequd (tmp, expmask);
  return vec_nor (tmp2, tmp2); // vec_not
#endif
}

/** \brief Return 2x64-bit vector boolean true values for each double,
 *  if infinity.
 *
 *  A IEEE Binary64 infinity has a exponent of 0x7ff and significand
 *  of all zeros.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal double compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-13  | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return a vector boolean long long, each containing all 0s(false)
 *  or 1s(true).
 */
static inline vb64_t
vec_isinff64 (vf64_t vf64)
{
  vb64_t result;

#if _ARCH_PWR9
#ifdef vec_test_data_class
  result = vec_test_data_class (vf64, 0x30);
#else
  __asm__(
      "xvtstdcdp %x0,%x1,0x30;\n"
      : "=wa" (result)
      : "wf" (vf64)
      :);
#endif
#else
  vui64_t tmp;
  const vui64_t expmask = CONST_VINT128_DW (0x7ff0000000000000UL,
					    0x7ff0000000000000UL);
  const vui64_t signmask = CONST_VINT128_DW (0x8000000000000000UL,
					     0x8000000000000000UL);
  tmp = vec_andc ((vui64_t) vf64, signmask);
  result = (vb64_t)vec_cmpequd (tmp, expmask);
#endif
  return (result);
}

/** \brief Return 2x64-bit vector boolean true values, for each double
 *  NaN value.
 *
 *  A IEEE Binary64 NaN value has an exponent between 0x7ff and
 *  the significand is nonzero.
 *  The sign bit is ignored.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-13  | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return a vector boolean long long, each containing all 0s(false)
 *  or 1s(true).
 */
static inline vb64_t
vec_isnanf64 (vf64_t vf64)
{
  vb64_t result;

#if _ARCH_PWR9
#ifdef vec_test_data_class
  result = vec_test_data_class (vf64, 0x40);
#else
  __asm__(
      "xvtstdcdp %x0,%x1,0x40;\n"
      : "=wa" (result)
      : "wf" (vf64)
      :);
#endif
#else
  vui64_t tmp;
  const vui64_t expmask = CONST_VINT128_DW (0x7ff0000000000000UL,
					    0x7ff0000000000000UL);
  const vui64_t signmask = CONST_VINT128_DW (0x8000000000000000UL,
					     0x8000000000000000UL);
  tmp = vec_andc ((vui64_t)vf64, signmask);
  result = (vb64_t)vec_cmpgtud (tmp, expmask);
#endif
  return (result);
}

/** \brief Return 2x64-bit vector boolean true values, for each double
 *  value, if normal (Not NaN, Inf, denormal, or zero).
 *
 *  A IEEE Binary64 normal value has an exponent between 0x001 and
 *  0x7ffe (a 0x7ff indicates NaN or Inf).  The significand can be
 *  any value (expect 0 if the exponent is zero).
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal double compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-15  | 1/cycle  |
 *  |power9   |   5   | 1/cycle  |
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return a vector boolean long long, each containing all 0s(false)
 *  or 1s(true).
 */
static inline vb64_t
vec_isnormalf64 (vf64_t vf64)
{
#if _ARCH_PWR9
  vb64_t tmp2;
#ifdef vec_test_data_class
  tmp2 = vec_test_data_class (vf64, 0x7f);
#else
  __asm__(
      "xvtstdcdp %x0,%x1,0x7f;\n"
      : "=wa" (tmp2)
      : "wf" (vf64)
      :);
#endif
  return vec_nor (tmp2, tmp2); // vec_not
#else
  const vui64_t expmask = CONST_VINT128_DW (0x7ff0000000000000UL,
					    0x7ff0000000000000UL);
  const vui64_t veczero = CONST_VINT128_DW (0UL, 0UL);
  vui64_t tmp;

  tmp = vec_and ((vui64_t) vf64, expmask);
  return (vb64_t) vec_nor (vec_cmpequd (tmp, expmask),
			   vec_cmpequd (tmp, veczero));
#endif
}

/** \brief Return 2x64-bit vector boolean true values, for each double
 *  value that is subnormal (denormal).
 *
 *  A IEEE Binary64 subnormal has an exponent of 0x000 and a
 *  nonzero significand.
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal double compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-16  | 1/cycle  |
 *  |power9   |   3   | 1/cycle  |
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return a vector boolean long long, each containing all 0s(false)
 *  or 1s(true).
 */
static inline vb64_t
vec_issubnormalf64 (vf64_t vf64)
{
  vb64_t result;

#if _ARCH_PWR9
#ifdef vec_test_data_class
  result = vec_test_data_class (vf64, 0x03);
#else
  __asm__(
      "xvtstdcdp %x0,%x1,0x03;\n"
      : "=wa" (result)
      : "wf" (vf64)
      :);
#endif
#else
  vui64_t tmp;
  const vui64_t minnorm = CONST_VINT128_DW (0x0010000000000000UL,
					    0x0010000000000000UL);
  const vui64_t vec_zero = CONST_VINT128_DW (0, 0);
  const vui64_t signmask = CONST_VINT128_DW (0x8000000000000000UL,
					     0x8000000000000000UL);
  tmp = vec_andc ((vui64_t) vf64, signmask);
  result = vec_andc (vec_cmpltud (tmp, minnorm),
		     vec_cmpequd (tmp, vec_zero));
#endif
  return (result);
}

/** \brief Return 2x64-bit vector boolean true values, for each double
 *  value that is +-0.0.
 *
 *  A IEEE Binary64 zero has an exponent of 0x000 and a
 *  zero significand.
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal double compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-13  | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param vf64 a vector of __binary32 values.
 *  @return a vector boolean int, each containing all 0s(false)
 *  or 1s(true).
 */
static inline vb64_t
vec_iszerof64 (vf64_t vf64)
{
  vb64_t result;

#if _ARCH_PWR9
#ifdef vec_test_data_class
  result = vec_test_data_class (vf64, 0x0c);
#else
  __asm__(
      "xvtstdcdp %x0,%x1,0x0c;\n"
      : "=wa" (result)
      : "wf" (vf64)
      :);
#endif
#else
  vui64_t tmp2;
  const vui64_t vec_zero = CONST_VINT128_DW (0, 0);
  const vui64_t signmask = CONST_VINT128_DW (0x8000000000000000UL,
					     0x8000000000000000UL);
  tmp2 = vec_andc ((vui64_t)vf64, signmask);
  result = (vb64_t)vec_cmpequd (tmp2, vec_zero);
#endif
  return (result);
}

/** \brief Copy the pair of doubles from a vector to IBM long double.
 *
 *  @param lval vector double values containing the IBM long double.
 *  @return IBM long double as FPR pair.
 */
static inline long double
vec_pack_longdouble (vf64_t lval)
{
#ifdef _ARCH_PWR7
  long double t;
  __asm__(
      "xxlor %0,%x1,%x1;\n"
      "\txxswapd %L0,%x1;\n"
      : "=f" (t)
      : "wa" (lval)
      : );
  return (t);
#else
  U_128 t;
  t.vf2 = lval;
  return (t.ldbl128);
#endif
}

/** \brief Copy the pair of doubles from a IBM long double to a vector
 * double.
 *
 *  @param lval IBM long double as FPR pair.
 *  @return vector double values containing the IBM long double.
 */
static inline vf64_t
vec_unpack_longdouble (long double lval)
{
#ifdef _ARCH_PWR7
  vf64_t t;
  __asm__(
      "xxmrghd %x0,%1,%L1;\n"
      : "=wa" (t)
      : "f" (lval)
      : );
  return (t);
#else
  U_128 t;
  t.ldbl128 = lval;
  return (t.vf2);
#endif
}

#endif /* VEC_F64_PPC_H_ */
