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

#include <vec_common_ppc.h>
#include <vec_int64_ppc.h>
/*!
 * \file  vec_f64_ppc.h
 * \brief Header package containing a collection of 128-bit SIMD
 * operations over 64-bit double floating point elements.
 *
 * Most vector double (64-bit float) operations are implemented
 * with PowerISA-2.06 (POWER7 and later) VSX instructions.
 * Most of these operations (compiler built-ins, or intrinsics) are
 * defined in <altivec.h> and described in the
 * <a href="https://gcc.gnu.org/onlinedocs/">compiler documentation</a>.
 *
 * \note The compiler disables associated <altivec.h> built-ins if the
 * <B>mcpu</B> target does not enable the specific instruction.
 * For example if you compile with <B>-mcpu=power7</B>, most of the
 * doubleword integer add, subtract, and compare operations useful
 * for floating point classification are not defined.
 * This header provides the appropriate substitutions,
 * will generate the minimum code, appropriate for the target,
 * and produce correct results.
 *
 * \note Most ppc64le compilers will default to -mcpu=power8 if not
 * specified.
 *
 * Most of these operations are implemented in a few instructions
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
 * - Defined in the OpenPOWER ABI but <I>not</I> yet defined in
 * <altivec.h> provided by available compilers in common use.
 * Examples include vector double even/odd.
 * - Providing special vector float tests for special conditions
 * without generating extraneous floating-point exceptions.
 * This is important for implementing vectorized forms of ISO C99 Math
 * functions. Examples include vector isnan, isinf, etc.
 * - Commonly used operations, not covered by the ABI or
 * <altivec.h>, and require multiple instructions or
 * are not obvious.
 * See example ISO C99 functions above.
 *
 * \section f64_perf_0_0 Performance data.
 * High level performance estimates are provided as an aid to function
 * selection when evaluating algorithms. For background on how
 * <I>Latency</I> and <I>Throughput</I> are derived see:
 * \ref perf_data
 */

/*! \brief vector of 64-bit binary64 elements.
 *  Same as vector double for PPC.  */
typedef __vector double __vbinary64;

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
  const vui32_t signmask = CONST_VINT128_DW(0x8000000000000000UL,
                                            0x8000000000000000UL);
  return (vf64_t)vec_andc ((vui32_t)vf64x, signmask);
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
 *  |power9   | 5-14  | 2/cycle  |
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return boolean int, true if all 2 double values are infinity
 */
static inline int
vec_all_isinff64 (vf64_t vf64)
{
  vui64_t tmp;
  const vui64_t expmask = CONST_VINT128_DW(0x7ff0000000000000UL,
					   0x7ff0000000000000UL);
  int result;

#if _ARCH_PWR9
  /* P9 has a 2 cycle xvabsdp and eliminates a const load. */
  tmp = (vui64_t) vec_abs (vf64);
#else
  const vui64_t signmask = CONST_VINT128_DW(0x8000000000000000UL,
					    0x8000000000000000UL);
  tmp = vec_andc ((vui64_t)vf64, signmask);
#endif
#if _ARCH_PWR8
  result = vec_all_eq(tmp, expmask);
#else
  result = vec_cmpud_all_eq(tmp, expmask);
#endif
  return (result);
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
 *  |power9   | 5-14  | 2/cycle  |
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return a boolean int, true if all 2 vector double values are
 *  NaN.
 */
static inline int
vec_all_isnanf64 (vf64_t vf64)
{
  vui64_t tmp2;
  const vui64_t expmask = CONST_VINT128_DW(0x7ff0000000000000UL,
					   0x7ff0000000000000UL);
  int result;

#if _ARCH_PWR9
  /* P9 has a 2 cycle xvabsdp and eliminates a const load. */
  tmp2 = (vui64_t) vec_abs (vf64);
#else
  const vui64_t signmask = CONST_VINT128_DW(0x8000000000000000UL,
					    0x8000000000000000UL);
  tmp2 = vec_andc ((vui64_t)vf64, signmask);
#endif
#if _ARCH_PWR8
  result = vec_all_gt(tmp2, expmask);
#else
  result = vec_cmpud_all_gt(tmp2, expmask);
#endif
  return (result);
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
 *  |power9   | 8-16  | 1/cycle  |
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return a boolean int, true if all 2 vector double values are
 *  normal.
 */
static inline int
vec_all_isnormalf64 (vf64_t vf64)
{
  vui64_t tmp;
  const vui64_t expmask = CONST_VINT128_DW(0x7ff0000000000000UL,
					   0x7ff0000000000000UL);
  const vui64_t veczero = CONST_VINT128_DW(0, 0);

  tmp = vec_and ((vui64_t) vf64, expmask);
#ifdef _ARCH_PWR8
  return !(vec_any_eq (tmp, expmask) || vec_any_eq(tmp, veczero));
#else
  return !(vec_cmpud_any_eq (tmp, expmask)
        || vec_cmpud_any_eq (tmp, veczero));
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
 *  |power9   | 10-19 | 1/cycle  |
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return a boolean int, true if all of 2 vector double values are
 *  subnormal.
 */
static inline int
vec_all_issubnormalf64 (vf64_t vf64)
{
  vui64_t tmp2;
  const vui64_t minnorm = CONST_VINT128_DW(0x0010000000000000UL,
					   0x0010000000000000UL);
  const vui64_t vec_zero = CONST_VINT128_DW(0, 0);
  int result;

#if _ARCH_PWR9
  /* P9 has a 2 cycle xvabsdp and eliminates a const load. */
  tmp2 = (vui64_t) vec_abs (vf64);
#else
  const vui64_t signmask = CONST_VINT128_DW(0x8000000000000000UL,
					    0x8000000000000000UL);
  tmp2 = vec_andc ((vui64_t)vf64, signmask);
#endif
#if _ARCH_PWR8
  result = vec_all_lt (tmp2, minnorm) && !vec_all_eq (tmp2, vec_zero);
#else
  result = vec_cmpud_all_gt (minnorm, tmp2) && !vec_cmpud_all_eq (tmp2, vec_zero);
#endif
  return (result);
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
 *  |power9   | 5     | 2/cycle  |
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return a boolean int, true if all 2 vector double values are
 *  +/- zero.
 */
static inline int
vec_all_iszerof64 (vf64_t vf64)
{
  vui64_t tmp2;
  const vui64_t vec_zero = CONST_VINT128_DW(0, 0);
  int result;

#if _ARCH_PWR9
  /* P9 has a 2 cycle xvabsdp and eliminates a const load. */
  tmp2 = (vui64_t) vec_abs (vf64);
#else
  const vui64_t signmask = CONST_VINT128_DW(0x8000000000000000UL,
					    0x8000000000000000UL);
  tmp2 = vec_andc ((vui64_t)vf64, signmask);
#endif
#if _ARCH_PWR8
  result = vec_all_eq(tmp2, vec_zero);
#else
  result = vec_all_eq((vui32_t)tmp2, (vui32_t)vec_zero);
#endif
  return (result);
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
 *  |power9   | 5-14  | 2/cycle  |
 *
 *  @param vf64 a vector of __binary32 values.
 *  @return boolean int, true if any of 2 double values are infinity
 */
static inline int
vec_any_isinff64 (vf64_t vf64)
{
  vui64_t tmp;
  const vui64_t expmask = CONST_VINT128_DW(0x7ff0000000000000UL,
					   0x7ff0000000000000UL);
  int result;

#if _ARCH_PWR9
  /* P9 has a 2 cycle xvabsdp and eliminates a const load. */
  tmp = (vui64_t) vec_abs (vf64);
#else
  const vui64_t signmask = CONST_VINT128_DW(0x8000000000000000UL,
					    0x8000000000000000UL);
  tmp = vec_andc ((vui64_t)vf64, signmask);
#endif
#if _ARCH_PWR8
  result = vec_any_eq(tmp, expmask);
#else
  result = vec_cmpud_any_eq(tmp, expmask);
#endif
  return (result);
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
 *  |power9   | 5-14  | 2/cycle  |
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return a boolean int, true if any of 2 vector double values are
 *  NaN.
 */
static inline int
vec_any_isnanf64 (vf64_t vf64)
{
  vui64_t tmp2;
  const vui64_t expmask = CONST_VINT128_DW(0x7ff0000000000000UL,
					   0x7ff0000000000000UL);
  int result;

#if _ARCH_PWR9
  /* P9 has a 2 cycle xvabsdp and eliminates a const load. */
  tmp2 = (vui64_t) vec_abs (vf64);
#else
  const vui64_t signmask = CONST_VINT128_DW(0x8000000000000000UL,
					    0x8000000000000000UL);
  tmp2 = vec_andc ((vui64_t)vf64, signmask);
#endif
#if _ARCH_PWR8
  result = vec_any_gt(tmp2, expmask);
#else
  result = vec_cmpud_any_gt(tmp2, expmask);
#endif
  return (result);
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
 *  |power9   | 5-14  | 1/cycle  |
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return a boolean int, true if any of 2 vector double values are
 *  normal.
 */
static inline int
vec_any_isnormalf64 (vf64_t vf64)
{
  vui64_t tmp, res;
  const vui64_t expmask = CONST_VINT128_DW(0x7ff0000000000000UL,
					   0x7ff0000000000000UL);
  const vui64_t veczero = CONST_VINT128_DW(0, 0);

  tmp = vec_and ((vui64_t) vf64, expmask);
#ifdef _ARCH_PWR8
  res = (vui64_t) vec_nor (vec_cmpeq (tmp, expmask), vec_cmpeq (tmp, veczero));
  return vec_any_gt(res, veczero);
#else
  res = (vui64_t) vec_nor (vec_cmpequd (tmp, expmask),
			   vec_cmpequd (tmp, veczero));
  return vec_cmpud_any_gt (res, veczero);
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
 *  |power9   | 14    | 1/cycle  |
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return true if any of 2 vector double values are subnormal.
 */
static inline int
vec_any_issubnormalf64 (vf64_t vf64)
{
  vui64_t tmp, tmpz, tmp2;
  const vui64_t minnorm = CONST_VINT128_DW(0x0010000000000000UL,
					   0x0010000000000000UL);
  const vui64_t vec_zero = CONST_VINT128_DW(0, 0);
  vb64_t vsubnorm;
  int result;

#if _ARCH_PWR9
  /* P9 has a 2 cycle xvabsdp and eliminates a const load. */
  tmp2 = (vui64_t) vec_abs (vf64);
#else
  const vui64_t signmask = CONST_VINT128_DW(0x8000000000000000UL,
					    0x8000000000000000UL);
  tmp2 = vec_andc ((vui64_t)vf64, signmask);
#endif
#ifdef _ARCH_PWR8
  tmp = (vui64_t) vec_cmplt(tmp2, minnorm);
  tmpz = (vui64_t) vec_cmpeq (tmp2, vec_zero);
  vsubnorm = (vb64_t) vec_andc (tmp, tmpz);
  result = vec_any_ne(vsubnorm, vec_zero);
#else
  tmp = (vui64_t) vec_cmpltud(tmp2, minnorm);
  tmpz = (vui64_t) vec_cmpequd (tmp2, vec_zero);
  vsubnorm = (vb64_t) vec_andc (tmp, tmpz);
  result = vec_cmpud_any_ne((vui64_t)vsubnorm, vec_zero);
#endif
  return (result);
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
 *  |power9   | 5     | 2/cycle  |
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return a boolean int, true if any of 2 vector double values are
 *  +/- zero.
 */
static inline int
vec_any_iszerof64 (vf64_t vf64)
{
  vui64_t tmp2;
  const vui64_t vec_zero = CONST_VINT128_DW(0, 0);
  int result;

#if _ARCH_PWR9
  /* P9 has a 2 cycle xvabsdp and eliminates a const load. */
  tmp2 = (vui64_t) vec_abs (vf64);
#else
  const vui64_t signmask = CONST_VINT128_DW(0x8000000000000000UL,
					    0x8000000000000000UL);
  tmp2 = vec_andc ((vui64_t)vf64, signmask);
#endif
  result = vec_any_eq(tmp2, vec_zero);
#if _ARCH_PWR8
  /* P8 has Vector Compare Equal To Unsigned Doubleword. */
  result = vec_any_eq(tmp2, vec_zero);
#else
  result = vec_cmpud_any_eq(tmp2, vec_zero);
#endif
  return (result);
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
 *  |power9   | 5-14  | 2/cycle  |
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return a vector boolean long long, each containing all 0s(false)
 *  or 1s(true).
 */
static inline vb64_t
vec_isinff64 (vf64_t vf64)
{
  vui64_t tmp;
  const vui64_t expmask = CONST_VINT128_DW(0x7ff0000000000000UL,
					   0x7ff0000000000000UL);
  vb64_t result;

#if _ARCH_PWR9
  /* P9 has a 2 cycle xvabsdp and eliminates a const load. */
  tmp = (vui64_t)vec_abs (vf64);
#else
  const vui64_t signmask = CONST_VINT128_DW(0x8000000000000000UL,
					    0x8000000000000000UL);
  tmp = vec_andc ((vui64_t) vf64, signmask);
#endif
#if _ARCH_PWR8
  /* P8 has Vector Compare Equal To Unsigned Doubleword. */
  result = vec_cmpeq (tmp, expmask);
#else
  /* P7 and earlier only have word compares. */
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
 *  |power9   | 5-14  | 2/cycle  |
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return a vector boolean long long, each containing all 0s(false)
 *  or 1s(true).
 */
static inline vb64_t
vec_isnanf64 (vf64_t vf64)
{
  vui64_t tmp2;
  const vui64_t expmask = CONST_VINT128_DW(0x7ff0000000000000UL,
					   0x7ff0000000000000UL);
  vb64_t result;

#if _ARCH_PWR9
  /* P9 has a 2 cycle xvabsdp and eliminates a const load. */
  tmp2 = (vui64_t) vec_abs (vf64);
#else
  const vui64_t signmask = CONST_VINT128_DW(0x8000000000000000UL,
					    0x8000000000000000UL);
  tmp2 = vec_andc ((vui64_t)vf64, signmask);
#endif
#if _ARCH_PWR8
  /* P8 has Vector Compare Equal To Unsigned Doubleword. */
  result = vec_cmpgt (tmp2, expmask);
#else
  /* P7 and earlier only have word compares. */
  result = (vb64_t)vec_cmpgtud (tmp2, expmask);
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
 *  |power9   | 7-16  | 1/cycle  |
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return a vector boolean long long, each containing all 0s(false)
 *  or 1s(true).
 */
static inline vb64_t
vec_isnormalf64 (vf64_t vf64)
{
  const vui64_t expmask = CONST_VINT128_DW(0x7ff0000000000000UL,
					   0x7ff0000000000000UL);
  const vui64_t veczero = CONST_VINT128_DW(0UL, 0UL);
  vui64_t tmp;
  vb64_t result;

  tmp = vec_and ((vui64_t) vf64, expmask);
#ifdef _ARCH_PWR8
  result = (vb64_t) vec_nor (vec_cmpeq (tmp, expmask),
			     vec_cmpeq (tmp, veczero));
#else
  result = (vb64_t) vec_nor (vec_cmpequd (tmp, expmask),
			     vec_cmpequd (tmp, veczero));
#endif
  return (result);
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
 *  |power9   | 7-16  | 1/cycle  |
 *
 *  @param vf64 a vector of __binary64 values.
 *  @return a vector boolean long long, each containing all 0s(false)
 *  or 1s(true).
 */
static inline vb64_t
vec_issubnormalf64 (vf64_t vf64)
{
  vui64_t tmp, tmpz, tmp2;
  const vui64_t minnorm = CONST_VINT128_DW(0x0010000000000000UL,
					   0x0010000000000000UL);
  const vui64_t vec_zero = CONST_VINT128_DW(0, 0);
  vb64_t result;

#if _ARCH_PWR9
  /* P9 has a 2 cycle xvabsdp and eliminates a const load. */
  tmp2 = (vui64_t) vec_abs (vf64);
#else
  const vui64_t signmask = CONST_VINT128_DW(0x8000000000000000UL,
					    0x8000000000000000UL);
  tmp2 = vec_andc ((vui64_t) vf64, signmask);
#endif
#ifdef _ARCH_PWR8
  tmp = (vui64_t) vec_cmplt(tmp2, minnorm);
  tmpz = (vui64_t) vec_cmpeq (tmp2, vec_zero);
  result = (vb64_t) vec_andc (tmp, tmpz);
#else
  tmp = (vui64_t) vec_cmpltud(tmp2, minnorm);
  tmpz = (vui64_t) vec_cmpequd (tmp2, vec_zero);
  result = (vb64_t ) vec_andc (tmp, tmpz);
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
 *  |power9   | 5     | 2/cycle  |
 *
 *  @param vf64 a vector of __binary32 values.
 *  @return a vector boolean int, each containing all 0s(false)
 *  or 1s(true).
 */
static inline vb64_t
vec_iszerof64 (vf64_t vf64)
{
  vui64_t tmp2;
  const vui64_t vec_zero = CONST_VINT128_DW(0, 0);
  vb64_t result;

#if _ARCH_PWR9
  /* P9 has a 2 cycle xvabsdp and eliminates a const load. */
  tmp2 = (vui64_t) vec_abs (vf64);
#else
  const vui64_t signmask = CONST_VINT128_DW(0x8000000000000000UL,
					    0x8000000000000000UL);
  tmp2 = vec_andc ((vui64_t)vf64, signmask);
#endif
#if _ARCH_PWR8
  /* P8 has Vector Compare Equal To Unsigned Doubleword. */
  result = vec_cmpeq (tmp2, vec_zero);
#else
  /* P7 and earlier only have word compares has. */
  result = (vb64_t)vec_cmpequd (tmp2, vec_zero);
#endif
  return (result);
}

#endif /* VEC_F64_PPC_H_ */
