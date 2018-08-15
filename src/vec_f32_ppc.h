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
 * This header covers operations that are any of the following:
 *
 * - Implemented in hardware instructions in newer processors,
 * but useful to programmers on slightly older processors
 * (even if the equivalent function requires more instructions).
 * Examples include the multiply even/odd/modulo word operations.
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
 * \section f32_perf_0_0 Performance data.
 * High level performance estimates are provided as an aid to function
 * selection when evaluating algorithms. For background on how
 * <I>Latency</I> and <I>Throughput</I> are derived see:
 * \ref perf_data
 */

#include <vec_common_ppc.h>

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

/** \brief Return true if all 4x32-bit vector float values
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
 *  |power9   | 5-14  | 2/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return boolean int, true if all 4 float values are infinity
 */
static inline int
vec_all_isinff32 (vf32_t vf32)
{
  vui32_t tmp;
  const vui32_t expmask = CONST_VINT128_W(0x7f800000, 0x7f800000, 0x7f800000,
					  0x7f800000);
  int result;

#if _ARCH_PWR9
  /* P9 has a 2 cycle xvabssp and eliminates a const load. */
  tmp = (vui32_t) vec_abs (vf32);
#else
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000,
					   0x80000000);
  tmp = vec_andc ((vui32_t)vf32, signmask);
#endif
  result = vec_all_eq(tmp, expmask);

  return (result);
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
 *  |power9   | 5-14  | 2/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a boolean int, true if all of 4 vector float values are
 *  NaN.
 */
static inline int
vec_all_isnanf32 (vf32_t vf32)
{
  vui32_t tmp2;
  const vui32_t expmask = CONST_VINT128_W(0x7f800000, 0x7f800000, 0x7f800000,
					  0x7f800000);
  int result;

#if _ARCH_PWR9
  /* P9 has a 2 cycle xvabssp and eliminates a const load. */
  tmp2 = (vui32_t) vec_abs (vf32);
#else
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000,
					   0x80000000);
  tmp2 = vec_andc ((vui32_t)vf32, signmask);
#endif
  result = vec_all_gt(tmp2, expmask);

  return (result);
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
 *  |power8   | 10-28 | 1/cycle  |
 *  |power9   | 8-16  | 1/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a boolean int, true if all of 4 vector float values are
 *  normal.
 */
static inline int
vec_all_isnormalf32 (vf32_t vf32)
{
  vui32_t tmp, tmp2;
  const vui32_t expmask = CONST_VINT128_W(0x7f800000, 0x7f800000, 0x7f800000,
					  0x7f800000);
  const vui32_t minnorm = CONST_VINT128_W(0x00800000, 0x00800000, 0x00800000,
					  0x00800000);
//  const vui32_t vec_zero = CONST_VINT128_W(0, 0, 0, 0);
  int result;

#if _ARCH_PWR9
  /* P9 has a 2 cycle xvabssp and eliminates a const load. */
  tmp2 = (vui32_t) vec_abs (vf32);
#else
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000, 0x80000000);
  tmp2 = vec_andc ((vui32_t)vf32, signmask);
#endif
  tmp = vec_and ((vui32_t) vf32, expmask);
  result = vec_all_ge (tmp2, minnorm) && vec_all_ne (tmp, expmask);

  return (result);
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
 *  |power9   | 10-19 | 1/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a boolean int, true if all of 4 vector float values are
 *  subnormal.
 */
static inline int
vec_all_issubnormalf32 (vf32_t vf32)
{
  vui32_t tmp2;
  const vui32_t explow = CONST_VINT128_W(0x00800000, 0x00800000, 0x00800000,
					 0x00800000);
  const vui32_t vec_zero = CONST_VINT128_W(0, 0, 0, 0);
  int result;

#if _ARCH_PWR9
  /* P9 has a 2 cycle xvabssp and eliminates a const load. */
  tmp2 = (vui32_t) vec_abs (vf32);
#else
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000,
					   0x80000000);
  tmp2 = vec_andc ((vui32_t)vf32, signmask);
#endif
  result = vec_all_lt (tmp2, explow) && vec_all_ne (tmp2, vec_zero);

  return (result);
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
 *  |power9   | 5     | 2/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a boolean int, true if all of 4 vector float values are
 *  +/- zero.
 */
static inline int
vec_all_iszerof32 (vf32_t vf32)
{
  vui32_t tmp2;
  const vui32_t vec_zero = CONST_VINT128_W(0, 0, 0, 0);
  int result;

#if _ARCH_PWR9
  /* P9 has a 2 cycle xvabssp and eliminates a const load. */
  tmp2 = (vui32_t) vec_abs (vf32);
#else
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000,
					   0x80000000);
  tmp2 = vec_andc ((vui32_t)vf32, signmask);
#endif
  result = vec_all_eq(tmp2, vec_zero);

  return (result);
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
 *  |power9   | 5-14  | 2/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return boolean int, true if any of 4 float values are infinity
 */

static inline int
vec_any_isinff32 (vf32_t vf32)
{
  vui32_t tmp;
  const vui32_t expmask = CONST_VINT128_W(0x7f800000, 0x7f800000, 0x7f800000,
					  0x7f800000);
  int result;

#if _ARCH_PWR9
  /* P9 has a 2 cycle xvabssp and eliminates a const load. */
  tmp = (vui32_t) vec_abs (vf32);
#else
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000,
					   0x80000000);
  tmp = vec_andc ((vui32_t)vf32, signmask);
#endif
  result = vec_any_eq(tmp, expmask);

  return (result);
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
 *  |power9   | 5-14  | 2/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a boolean int, true if any of 4 vector float values are
 *  NaN.
 */
static inline int
vec_any_isnanf32 (vf32_t vf32)
{
  vui32_t tmp2;
  const vui32_t expmask = CONST_VINT128_W(0x7f800000, 0x7f800000, 0x7f800000,
					  0x7f800000);
  int result;

#if _ARCH_PWR9
  /* P9 has a 2 cycle xvabssp and eliminates a const load. */
  tmp2 = (vui32_t) vec_abs (vf32);
#else
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000,
					   0x80000000);
  tmp2 = vec_andc ((vui32_t)vf32, signmask);
#endif
  result = vec_any_gt(tmp2, expmask);

  return (result);
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
 *  |power8   | 10-25 | 1/cycle  |
 *  |power9   | 10-19 | 1/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a boolean int, true if any of 4 vector float values are
 *  normal.
 */
static inline int
vec_any_isnormalf32 (vf32_t vf32)
{
  vui32_t tmp, tmp2;
  const vui32_t expmask = CONST_VINT128_W(0x7f800000, 0x7f800000, 0x7f800000,
					  0x7f800000);
  const vui32_t minnorm = CONST_VINT128_W(0x00800000, 0x00800000, 0x00800000,
					  0x00800000);
  const vui32_t vec_zero = CONST_VINT128_W(0, 0, 0, 0);
  vb32_t vnorm;
  int result;

#if _ARCH_PWR9
  /* P9 has a 2 cycle xvabssp and eliminates a const load. */
  tmp2 = (vui32_t) vec_abs (vf32);
#else
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000, 0x80000000);
  tmp2 = vec_andc ((vui32_t)vf32, signmask);
#endif
  tmp = vec_and ((vui32_t) vf32, expmask);
  tmp2 = (vui32_t) vec_cmplt(tmp2, minnorm);
  tmp = (vui32_t) vec_cmpeq (tmp, expmask);
  vnorm = (vb32_t ) vec_nor (tmp, tmp2);

  result = vec_any_gt(vnorm, vec_zero);

  return (result);
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
 *  |power9   | 14    | 1/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return if any of 4 vector float values are subnormal.
 */
static inline int
vec_any_issubnormalf32 (vf32_t vf32)
{
  vui32_t tmp, tmpz, tmp2;
  const vui32_t explow = CONST_VINT128_W(0x00800000, 0x00800000, 0x00800000,
					 0x00800000);
  const vui32_t vec_zero = CONST_VINT128_W(0, 0, 0, 0);
  vb32_t vsubnorm;
  int result;

#if _ARCH_PWR9
  /* P9 has a 2 cycle xvabssp and eliminates a const load. */
  tmp2 = (vui32_t) vec_abs (vf32);
#else
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000,
					   0x80000000);
  tmp2 = vec_andc ((vui32_t)vf32, signmask);
#endif
  tmp = (vui32_t) vec_cmplt(tmp2, explow);
  tmpz = (vui32_t) vec_cmpeq (tmp2, vec_zero);
  vsubnorm = (vb32_t ) vec_andc (tmp, tmpz);
  result = vec_any_ne(vsubnorm, vec_zero);

  return (result);
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
 *  |power9   | 5     | 2/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a boolean int, true if any of 4 vector float values are
 *  +/- zero.
 */
static inline int
vec_any_iszerof32 (vf32_t vf32)
{
  vui32_t tmp2;
  const vui32_t vec_zero = CONST_VINT128_W(0, 0, 0, 0);
  int result;

#if _ARCH_PWR9
  /* P9 has a 2 cycle xvabssp and eliminates a const load. */
  tmp2 = (vui32_t) vec_abs (vf32);
#else
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000,
					   0x80000000);
  tmp2 = vec_andc ((vui32_t)vf32, signmask);
#endif
  result = vec_any_eq(tmp2, vec_zero);

  return (result);
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
 *  |power9   | 5-14  | 2/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a vector boolean int, each containing all 0s(false)
 *  or 1s(true).
 */
static inline vb32_t
vec_isinff32 (vf32_t vf32)
{
  vui32_t tmp;
  const vui32_t expmask = CONST_VINT128_W(0x7f800000, 0x7f800000, 0x7f800000,
					  0x7f800000);
  vb32_t result;

#if _ARCH_PWR9
  /* P9 has a 2 cycle xvabssp and eliminates a const load. */
  tmp = (vui32_t) vec_abs (vf32);
#else
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000,
					   0x80000000);
  tmp = vec_andc ((vui32_t)vf32, signmask);
#endif
  result = vec_cmpeq (tmp, expmask);

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
 *  |power9   | 5-14  | 2/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a vector boolean int, each containing all 0s(false)
 *  or 1s(true).
 */
static inline vb32_t
vec_isnanf32 (vf32_t vf32)
{
  vui32_t tmp2;
  const vui32_t expmask = CONST_VINT128_W(0x7f800000, 0x7f800000, 0x7f800000,
					  0x7f800000);
  vb32_t result;

#if _ARCH_PWR9
  /* P9 has a 2 cycle xvabssp and eliminates a const load. */
  tmp2 = (vui32_t) vec_abs (vf32);
#else
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000,
					   0x80000000);
  tmp2 = vec_andc ((vui32_t)vf32, signmask);
#endif
  result = vec_cmpgt (tmp2, expmask);

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
 *  |power8   | 6-16  | 1/cycle  |
 *  |power9   | 7-16  | 1/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a vector boolean int, each containing all 0s(false)
 *  or 1s(true).
 */
static inline vb32_t
vec_isnormalf32 (vf32_t vf32)
{
  vui32_t tmp, tmp2;
  const vui32_t expmask = CONST_VINT128_W(0x7f800000, 0x7f800000, 0x7f800000,
					  0x7f800000);
  const vui32_t minnorm = CONST_VINT128_W(0x00800000, 0x00800000, 0x00800000,
					  0x00800000);
  vb32_t result;

#if _ARCH_PWR9
  /* P9 has a 2 cycle xvabssp and eliminates a const load. */
  tmp2 = (vui32_t) vec_abs (vf32);
#else
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000,
					   0x80000000);
  tmp2 = vec_andc ((vui32_t)vf32, signmask);
#endif
  tmp = vec_and ((vui32_t) vf32, expmask);
  tmp2 = (vui32_t) vec_cmplt (tmp2, minnorm);
  tmp = (vui32_t) vec_cmpeq (tmp, expmask);
  result = (vb32_t ) vec_nor (tmp, tmp2);

  return (result);
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
 *  |power9   | 7-16  | 1/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a vector boolean int, each containing all 0s(false)
 *  or 1s(true).
 */
static inline vb32_t
vec_issubnormalf32 (vf32_t vf32)
{
  vui32_t tmp, tmpz, tmp2;
  const vui32_t explow = CONST_VINT128_W(0x00800000, 0x00800000, 0x00800000,
					 0x00800000);
  const vui32_t vec_zero = CONST_VINT128_W(0, 0, 0, 0);
  vb32_t result;

#if _ARCH_PWR9
  /* P9 has a 2 cycle xvabssp and eliminates a const load. */
  tmp2 = (vui32_t) vec_abs (vf32);
#else
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000,
					   0x80000000);
  tmp2 = vec_andc ((vui32_t)vf32, signmask);
#endif
  tmp = (vui32_t) vec_cmplt(tmp2, explow);
  tmpz = (vui32_t) vec_cmpeq (tmp2, vec_zero);
  result = (vb32_t ) vec_andc (tmp, tmpz);

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
  vui32_t tmp2;
  const vui32_t vec_zero = CONST_VINT128_W(0, 0, 0, 0);
  vb32_t result;

#if _ARCH_PWR9
  /* P9 has a 2 cycle xvabssp and eliminates a const load. */
  tmp2 = (vui32_t) vec_abs (vf32);
#else
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000,
					   0x80000000);
  tmp2 = vec_andc ((vui32_t)vf32, signmask);
#endif
  result = vec_cmpeq (tmp2, vec_zero);

  return (result);
}

#endif /* VEC_F32_PPC_H_ */
