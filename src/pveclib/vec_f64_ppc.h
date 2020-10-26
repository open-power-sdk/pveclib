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
#include <pveclib/vec_int128_ppc.h>
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

///@cond INTERNAL
static inline vf64_t
vec_vlsfdux (const signed long long ra, const double *rb);
static inline void
vec_vstsfdux (vf64_t xs, const signed long long ra, double *rb);
///@endcond

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
#if _ARCH_PWR9 && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  const vui64_t vec_zero = CONST_VINT128_DW (0, 0);
#ifdef vec_test_data_class
  tmp = (vui64_t)vec_test_data_class (vf64, 0x70);
#else
  __asm__(
      "xvtstdcdp %x0,%x1,0x70;\n"
      : "=wa" (tmp)
      : "wa" (vf64)
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

#if _ARCH_PWR9 && !(defined(__clang__) && __clang_major__ < 9)
  const vui64_t vec_ones = CONST_VINT128_DW (-1, -1);
#ifdef vec_test_data_class
  tmp = (vui64_t)vec_test_data_class (vf64, 0x30);
#else
  __asm__(
      "xvtstdcdp %x0,%x1,0x30;\n"
      : "=wa" (tmp)
      : "wa" (vf64)
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

#if _ARCH_PWR9  && !(defined(__clang__) && __clang_major__ < 9)
  const vui64_t vec_ones = CONST_VINT128_DW (-1, -1);
#ifdef vec_test_data_class
  tmp = (vui64_t)vec_test_data_class (vf64, 0x40);
#else
  __asm__(
      "xvtstdcdp %x0,%x1,0x40;\n"
      : "=wa" (tmp)
      : "wa" (vf64)
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
#if _ARCH_PWR9 && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#ifdef vec_test_data_class
  tmp = (vui64_t)vec_test_data_class (vf64, 0x7f);
#else
  __asm__(
      "xvtstdcdp %x0,%x1,0x7f;\n"
      : "=wa" (tmp)
      : "wa" (vf64)
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

#if _ARCH_PWR9 && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  const vui64_t vec_ones = CONST_VINT128_DW (-1, -1);
#ifdef vec_test_data_class
  tmp = (vui64_t)vec_test_data_class (vf64, 0x03);
#else
  __asm__(
      "xvtstdcdp %x0,%x1,0x03;\n"
      : "=wa" (tmp)
      : "wa" (vf64)
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

#if _ARCH_PWR9 && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  const vui64_t vec_ones = CONST_VINT128_DW (-1, -1);
#ifdef vec_test_data_class
  tmp = (vui64_t)vec_test_data_class (vf64, 0x0c);
#else
  __asm__(
      "xvtstdcdp %x0,%x1,0x0c;\n"
      : "=wa" (tmp)
      : "wa" (vf64)
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
      : "wa" (vf64)
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

#if _ARCH_PWR9  && !(defined(__clang__) && __clang_major__ < 9)
  const vui64_t vec_ones = CONST_VINT128_DW (-1, -1);
#ifdef vec_test_data_class
  tmp = (vui64_t)vec_test_data_class (vf64, 0x30);
#else
  __asm__(
      "xvtstdcdp %x0,%x1,0x30;\n"
      : "=wa" (tmp)
      : "wa" (vf64)
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

#if _ARCH_PWR9 && !(defined(__clang__) && __clang_major__ < 9)
  const vui64_t vec_ones = CONST_VINT128_DW (-1, -1);
#ifdef vec_test_data_class
  tmp = (vui64_t)vec_test_data_class (vf64, 0x40);
#else
  __asm__(
      "xvtstdcdp %x0,%x1,0x40;\n"
      : "=wa" (tmp)
      : "wa" (vf64)
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
      : "wa" (vf64)
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
      : "wa" (vf64)
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
      : "wa" (vf64)
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
      : "wa" (vf64)
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

#if _ARCH_PWR9 && !(defined(__clang__) && __clang_major__ < 9)
#ifdef vec_test_data_class
  result = vec_test_data_class (vf64, 0x30);
#else
  __asm__(
      "xvtstdcdp %x0,%x1,0x30;\n"
      : "=wa" (result)
      : "wa" (vf64)
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

#if _ARCH_PWR9 && !(defined(__clang__) && __clang_major__ < 9)
#ifdef vec_test_data_class
  result = vec_test_data_class (vf64, 0x40);
#else
  __asm__(
      "xvtstdcdp %x0,%x1,0x40;\n"
      : "=wa" (result)
      : "wa" (vf64)
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
      : "wa" (vf64)
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
      : "wa" (vf64)
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
      : "wa" (vf64)
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
#ifdef __clang__
  __VEC_U_128 t;
  t.vf2 = lval;
  return (t.ldbl128);
#else
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
  __VEC_U_128 t;
  t.vf2 = lval;
  return (t.ldbl128);
#endif
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
#ifdef __clang__
  __VEC_U_128 t;
  t.ldbl128 = lval;
  return (t.vf2);
#else
#ifdef _ARCH_PWR7
  vf64_t t;
  __asm__(
      "xxmrghd %x0,%1,%L1;\n"
      : "=wa" (t)
      : "f" (lval)
      : );
  return (t);
#else
  __VEC_U_128 t;
  t.ldbl128 = lval;
  return (t.vf2);
#endif
#endif
}

/** \brief Vector Gather-Load Float Double from scalar Offsets.
 *
 *  For each scalar offset[0|1], load the float double element at
 *  *(char*)array+offset[0|1]. Merge those float double elements
 *  and return the resulting vector.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   12  | 1/cycle  |
 *  |power9   |   11  | 1/cycle  |
 *
 *  @param array Pointer to array of doubles.
 *  @param offset0 Scalar (64-bit) byte offsets from &array.
 *  @param offset1 Scalar (64-bit) byte offsets from &array.
 *  @return vector double containing elements loaded from
 *  *(char*)array+offset0 and *(char*)array+offset1.
 */
static inline vf64_t
vec_vglfdso (double *array, const long long offset0,
	     const long long offset1)
{
  vf64_t re0, re1, result;

  re0 = vec_vlsfdux (offset0, array);
  re1 = vec_vlsfdux (offset1, array);
  /* Need to handle endian as the vec_vlsfdux result is always left
   * justified in VR, while element [0] may be left ot right. */
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  result = (vf64_t) vec_permdi ((vui64_t) re1, (vui64_t) re0, 0);
#else
  result = (vf64_t) vec_permdi ((vui64_t) re0, (vui64_t) re1, 0);
#endif
  return result;
}

/** \brief Vector Gather-Load Float Double from Doubleword Offsets.
 *
 *  For each doubleword element [i] of vra, load the float double
 *  element at *(char*)array+vra[i]. Merge those float double elements
 *  and return the resulting vector.
 *
 *  \note As effective address calculation is modulo 64-bits, signed or
 *  unsigned doubleword offsets are equivalent.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   12  | 1/cycle  |
 *  |power9   |   11  | 1/cycle  |
 *
 *  @param array Pointer to array of doubles.
 *  @param vra Vector of doubleword (64-bit) byte offsets from &array.
 *  @return vector double containing elements loaded from
 *  *(char*)array+vra[0] and *(char*)array+vra[1].
 */
static inline vf64_t
vec_vglfddo (double *array, vi64_t vra)
{
  vf64_t rese0, rese1;

#ifdef _ARCH_PWR8
  rese0 = vec_vlsfdux (vra[VEC_DW_H], array);
  rese1 = vec_vlsfdux (vra[VEC_DW_L], array);
#else
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);

  rese0 = vec_vlsfdux (scalar_extract_uint64_from_high_uint128(gprp), array);
  rese1 = vec_vlsfdux (scalar_extract_uint64_from_low_uint128(gprp), array);
#endif
  return (vf64_t) vec_permdi ((vui64_t) rese0, (vui64_t) rese1, 0);
}

/** \brief Vector Gather-Load Float Double from Doubleword Scaled Indexes.
 *
 *  For each doubleword element [i] of vra, load the float double
 *  element *array[vra[i] * (1 << scale)]. Merge those float double
 *  elements and return the resulting vector. Indexes are converted to
 *  offsets from *array by shifting each doubleword left (3+scale) bits.
 *
 *  \note As effective address calculation is modulo 64-bits, signed or
 *  unsigned doubleword indexes are equivalent.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 14-23 | 1/cycle  |
 *  |power9   | 13-22 | 1/cycle  |
 *
 *  @param array Pointer to array of doubles.
 *  @param vra Vector of doubleword indexes.
 *  @param scale 8-bit integer. Indexes are multiplied by
 *  2<sup>scale</sup>.
 *  @return Vector double containing array[vra[0]*(1<<scale)]
 *  and array[vra[1]*(1<<scale)].
 */
static inline vf64_t
vec_vglfddsx (double *array, vi64_t vra,
	     const unsigned char scale)
{
  vi64_t offset;

  offset = (vi64_t) vec_sldi ((vui64_t) vra, (3 + scale));
  return vec_vglfddo (array, offset);
}

/** \brief Vector Gather-Load Float Double from Doubleword indexes.
 *
 *  For each doubleword element [i] of vra, load the double
 *  element array[vra[i]]. Merge those float double elements and
 *  return the resulting vector. The indexes are converted to offsets
 *  from *array by shifting each doubleword index left 3-bits (*8).
 *
 *  \note As effective address calculation is modulo 64-bits, signed or
 *  unsigned doubleword indexes are equivalent.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 14-23 | 1/cycle  |
 *  |power9   | 13-22 | 1/cycle  |
 *
 *  @param array Pointer to array of doubles.
 *  @param vra Vector of doubleword indexes.
 *  @return vector double containing {array[vra[0]], array[vra[1]]}.
 */
static inline vf64_t
vec_vglfddx (double *array, vi64_t vra)
{
  vi64_t offset;

  offset = (vi64_t) vec_sldi ((vui64_t) vra, 3);
  return vec_vglfddo (array, offset);
}

/** \brief Vector Scatter-Store Float Double to Scalar Offsets.
 *
 *  For each doubleword element [i] of vra, Store the double
 *  element xs[i] at *(char*)array+offset[0|1].
 *
 *  \note As effective address calculation is modulo 64-bits, signed or
 *  unsigned doubleword offsets are equivalent.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   12  | 1/cycle  |
 *  |power9   |   8   | 1/cycle  |
 *
 *  @param xs Vector double elements to scatter store.
 *  @param array Pointer to array of doubles.
 *  @param offset0 Scalar (64-bit) byte offset from &array.
 *  @param offset1 Scalar (64-bit) byte offset from &array.
 */
static inline void
vec_vsstfdso (vf64_t xs, double *array,
	      const long long offset0, const long long offset1)
{
  vf64_t xs1;

  xs1 = (vf64_t) vec_xxspltd ((vui64_t) xs, 1);
  /* Need to handle endian as vec_vstsfdux always left side of
   * the VR, while the element [0] may in the left or right. */
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  vec_vstsfdux (xs, offset1, array);
  vec_vstsfdux (xs1, offset0, array);
#else
  vec_vstsfdux (xs, offset0, array);
  vec_vstsfdux (xs1, offset1, array);
#endif
}

/** \brief Vector Scatter-Store Float Double to Doubleword Offsets.
 *
 *  For each doubleword element [i] of vra, Store the double
 *  element xs[i] at *(char*)array+vra[i].
 *
 *  \note As effective address calculation is modulo 64-bits, signed or
 *  unsigned doubleword offsets are equivalent.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   12  | 1/cycle  |
 *  |power9   |   8   | 1/cycle  |
 *
 *  @param xs Vector double elements to scatter store.
 *  @param array Pointer to array of doubles.
 *  @param vra Vector of doubleword (64-bit) byte offsets from &array.
 */
static inline void
vec_vsstfddo (vf64_t xs, double *array,
	    vi64_t vra)
{
  vf64_t xs1 = (vf64_t) vec_xxspltd ((vui64_t) xs, 1);
#ifdef _ARCH_PWR8
  vec_vstsfdux (xs, vra[VEC_DW_H], array);
  vec_vstsfdux (xs1, vra[VEC_DW_L], array);
#else
  // Need to explicitly manage the VR/GPR xfer for PWR7
  unsigned __int128 gprp = vec_transfer_vui128t_to_uint128 ((vui128_t) vra);
  vec_vstsfdux (xs, scalar_extract_uint64_from_high_uint128(gprp), array);
  vec_vstsfdux (xs1, scalar_extract_uint64_from_low_uint128(gprp), array);
#endif
}

/** \brief Vector Scatter-Store Float Double to Doubleword Scaled Index.
 *
 *  For each doubleword element [i] of vra, store the double
 *  element xs[i] at array[vra[i] * (1 << scale)]. Indexes are
 *  converted to offsets from *array by shifting each doubleword of vra
 *  left (3+scale) bits.
 *
 *  \note As effective address calculation is modulo 64-bits, signed or
 *  unsigned doubleword indexes are equivalent.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 14-23 | 1/cycle  |
 *  |power9   | 10-19 | 1/cycle  |
 *
 *  @param xs Vector double elements to store.
 *  @param array Pointer to array of doubles.
 *  @param vra Vector of doubleword indexes.
 *  @param scale Factor effectually multiplying the indexes by
 *  2<sup>scale</sup>.
 */
static inline void
vec_vsstfddsx (vf64_t xs, double *array,
	    vi64_t vra, const unsigned char scale)
{
  vi64_t offset;

  offset = (vi64_t) vec_sldi ((vui64_t) vra, (3 + scale));
  vec_vsstfddo (xs, array, offset);
}

/** \brief Vector Scatter-Store Float Double to Doubleword Indexes.
 *
 *  For each doubleword element [i] of vra, store the double
 *  element xs[i] at array[vra[i]]. Indexes are converted to offsets
 *  from *array by shifting each doubleword of vra
 *  left 3 bits.
 *
 *  \note As effective address calculation is modulo 64-bits, signed or
 *  unsigned doubleword indexes are equivalent.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 14-23 | 1/cycle  |
 *  |power9   | 10-19 | 1/cycle  |
 *
 *  @param xs Vector double elements to store.
 *  @param array Pointer to array of doubles.
 *  @param vra Vector of doubleword indexes.
 */
static inline void
vec_vsstfddx (vf64_t xs, double *array, vi64_t vra)
{
  vi64_t offset;

  offset = (vi64_t) vec_sldi ((vui64_t) vra, 3);
  vec_vsstfddo (xs, array, offset);
}

/** \brief Vector Scalar Load Float Double Signed Doubleword Indexed.
 *
 *  Load the left most doubleword of vector <B>xt</B> as a scalar
 *  double from the effective address formed by <B>rb+ra</B>. The
 *  operand <B>rb</B> is a pointer to an array of doubles.
 *  The operand <B>ra</B> is a doubleword integer byte offset
 *  from <B>rb</B>. The result <B>xt</B> is returned as a vf64_t
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
 *  \note This is instruction was introduced in PowerISA 2.06 (POWER7).
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
 *  @param ra const doubleword index (offset/displacement).
 *  @param rb const doubleword pointer to an array of doubles.
 *  @return The data stored at (ra + rb) is loaded into vector
 *  doubleword element 0. Element 1 is undefined.
 */
static inline vf64_t
vec_vlsfdux (const signed long long ra, const double *rb)
{
  vf64_t xt;

#if defined (__clang__)
  __VEC_U_128 t;
  unsigned long long *p = (unsigned long long *)((char *)rb + ra);
  t.ulong.upper = *p;
  xt = t.vx1;
#else
  if (__builtin_constant_p (ra) && (ra < 32760) && (ra >= -32768)
      && ((ra & 3) == 0))
    {
#if defined (_ARCH_PWR9)
      __asm__(
	  "lxsd%X1 %0,%1;"
	  : "=v" (xt)
	  : "m" (*(double*)((char *)rb + ra))
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
	      : "I" (ra), "b" (rb), "m" (*(double*)((char *)rb+rt))
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
	      : "Z" (*(double*)((char *)rb+rt))
	      : );
#endif
	}
#endif
    } else {
      __asm__(
	  "lxsdx %x0,%y1;"
	  : "=wa" (xt)
	  : "Z" (*(double*)((char *)rb+ra))
	  : );
    }
#endif
  return xt;
}

/** \brief Vector Store Scalar Float Double Signed Doubleword Indexed.
 *
 *  Stores the left most doubleword of vector <B>xs</B> as a scalar
 *  double float at the effective address formed by <B>rb+ra</B>. The
 *  operand <B>rb</B> is a pointer to an array of doubles.
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
 *  @param ra const doubleword index (offset/displacement).
 *  @param rb const doubleword pointer to an array of doubles.
 */
static inline void
vec_vstsfdux (vf64_t xs, const signed long long ra, double *rb)
{
#if defined (__clang__)
  __VEC_U_128 t;
  unsigned long long *p = (unsigned long long *)((char *)rb + ra);
  t.vx1 = xs;
  *p = t.ulong.upper;
#else
  if (__builtin_constant_p (ra) && (ra <= 32760) && (ra >= -32768)
      && ((ra & 3) == 0))
    {
#if defined (_ARCH_PWR9)
      __asm__(
	  "stxsd%X0 %1,%0;"
	  : "=m" (*(double*)((char *)rb + ra))
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
	      : "=Z" (*(double*)((char *)rb+rt))
	      : "wa" (xs)
	      : );
	}
#endif
    } else {
      __asm__(
	  "stxsdx %x1,%y0;"
	  : "=Z" (*(double*)((char *)rb+ra))
	  : "wa" (xs)
	  : );
    }
#endif
}

#endif /* VEC_F64_PPC_H_ */
