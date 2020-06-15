/*
 Copyright (c) [2017-2018] IBM Corporation.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 vec_f128_ppc.h

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Apr 11, 2016
 */

/*!
 * \file  vec_f128_ppc.h
 * \brief Header package containing a collection of 128-bit SIMD
 * operations over Quad-Precision floating point elements.
 *
 * PowerISA 3.0 added Quad-Precision floating point type and
 * operations to the Vector-Scalar Extension (VSX) facility.
 * The first hardware implementation is available in POWER9.
 *
 * While all Quad-Precision operations are on 128-bit vector registers,
 * they are defined as scalars in the PowerISA. The OpenPOWER ABI also
 * treats the __float128 type as scalar that just happens to use vector
 * registers for parameter passing and operations.
 * As such no operations using __float128 (_Float128, or __ieee128) as
 * parameter or return value are defined as vector built-ins in the ABI
 * or <altivec.h>.
 *
 * \note GCC 8.2 does document some built-ins, using the <I>scalar</I>
 * prefix (scalar_extract_exp, scalar_extract_sig,
 * scalar_test_data_class), that do accept the __ieee128 type.
 * This work seems to be incomplete as scalar_exp_cmp_* for the
 * __ieee128 type are not present.
 * GCC 7.3 defines vector and scalar forms of the extract/insert_exp
 * for float and double but not for __ieee128.
 * These built-ins are not defined in GCC 6.4. See
 * <a href="https://gcc.gnu.org/onlinedocs/">compiler documentation</a>.
 * These are useful operations and can be implement in a few
 * vector logical instruction for earlier machines. So it seems
 * reasonable to add these to pveclib for both vector and scalar forms.
 *
 * Quad-Precision is not supported in hardware until POWER9. However
 * the compiler and runtime supports the __float128 type and arithmetic
 * operations via soft-float emulation for earlier processors.
 * The soft-float implementation follows the ABI and passes __float128
 * parameters and return values in vector registers.
 *
 * So it is not unreasonable for this header to provide vector forms
 * of the __float128 classification functions
 * (isnormal/subnormal/finite/inf/nan/zero, copysign, and abs).
 * These functions can be implemented directly using (one or more) POWER9
 * instructions, or a few vector logical and integer compare
 * instructions for POWER7/8. Each is comfortably small enough to be
 * in-lined and inherently faster than the equivalent POSIX or compiler
 * built-in runtime functions. Performing these operations in-line and
 * directly in vector registers (VRs) avoids call/return and VR <-> GPR
 * transfer overhead.
 *
 * \note The compiler disables associated <altivec.h> built-ins if the
 * <B>mcpu</B> target does not enable the specific instruction.
 * For example if you compile with <B>-mcpu=power8</B>, Quad-Precision
 * floating-point operations useful
 * for floating point classification are not defined.
 * This header provides the appropriate substitutions,
 * will generate the minimum code, appropriate for the target,
 * and produce correct results.
 *
 * \note Most ppc64le compilers will default to <B>-mcpu=</B><I>power8</I>
 * if  <B>-mcpu</B> is not specified.
 *
 * This header covers operations that are any of the following:
 *
 * - Implemented in hardware instructions in newer processors,
 * but useful to programmers on slightly older processors
 * (even if the equivalent function requires more instructions).
 * - Defined in the OpenPOWER ABI but <I>not</I> yet defined in
 * <altivec.h> provided by available compilers in common use.
 * Examples include scalar_test_neg, scalar_test_data_class, etc.
 * - Providing special vector float tests for special conditions
 * without generating extraneous floating-point exceptions.
 * This is important for implementing __float128 forms of ISO C99 Math
 * functions. Examples include vector isnan, isinf, etc.
 * - Commonly used operations, not covered by the ABI or
 * <altivec.h>, and require multiple instructions or
 * are not obvious.
 *
 * \section f128_examples_0_0 Examples
 * For example: using the the classification functions for implementing
 * the math library function sine and cosine.
 * The Posix specification requires that special input values are
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
 * sinf128():
 * \code
__binary128
test_sinf128 (__binary128 value)
{
  __binary128 result;

  if (vec_all_isnormalf128 (value))
    {
      // body of taylor series.
      ...
    }
  else
    {
      if (vec_all_isinff128 (value))
	result = vec_const_nanf128 ();
      else
	result = value;
    }
  return result;
}
 * \endcode
 *
 * For another example the cos() function.
 * - If the input <I>value</I> is NaN then return a NaN.
 * - If the input <I>value</I> is +-0.0 then return <I>1.0</I>.
 * - If the input <I>value</I> is +-Inf then return a NaN.
 * - Otherwise compute and return cos(value).
 *
 * The following code example uses functions from this header to
 * address the Posix requirements for special values input to
 * cosf128():
 * \code
__binary128
test_cosf128 (__binary128 value)
{
  __binary128 result;

  if (vec_all_isfinitef128 (value))
    {
      if (vec_all_iszerof128 (value))
	result = 1.0Q;
      else
	{
	  // body of taylor series ...
	}
    }
  else
    {
      if (vec_all_isinff128 (value))
	result = vec_const_nanf128 ();
      else
	result = value;
    }
  return result;
}
 * \endcode
 *
 * Neither example raises floating point exceptions or sets
 * <B>errno</B>, as appropriate for a vector math library.
 *
 * \section f128_perf_0_0 Performance data
 * High level performance estimates are provided as an aid to function
 * selection when evaluating algorithms. For background on how
 * <I>Latency</I> and <I>Throughput</I> are derived see:
 * \ref perf_data
 */

#ifndef VEC_F128_PPC_H_
#define VEC_F128_PPC_H_

#include <pveclib/vec_common_ppc.h>
#include <pveclib/vec_int128_ppc.h>
#include <pveclib/vec_f64_ppc.h>


/* __float128 was added in GCC 6.0.  But only with -mfloat128.
   Later compilers typedef __float128 to __ieee128 and
   long double to __ibm128. The intent was to allow the switch of
   long double from __ibm128 to __ieee128 (someday).

   Clang does not define __FLOAT128__ or __float128 without both
   -mcu=power9 and -mfloat128.
   So far clang does not support/define the __ibm128 type. */
#ifdef __FLOAT128__
typedef __float128 __Float128;
typedef __float128 __binary128;
typedef __float128 __ieee128;
#ifndef __clang__
typedef __ibm128 __IBM128;
#else
/* Clang does not define __ibm128 over IBM long double.
   So defined it here. */
typedef long double __IBM128;
#endif
#else
/* Before GCC 6.0 (or without -mfloat128) we need to fake it.  */
/*! \brief vector of 128-bit binary128 element.
 *  Same as __float128 for PPC.  */
typedef vui128_t vf128_t;
/*! \brief Define __Float128 if not defined by the compiler.
 *  Same as __float128 for PPC.  */
typedef vf128_t __Float128;
/*! \brief Define __binary128 if not defined by the compiler.
 *  Same as __float128 for PPC.  */
typedef vf128_t __binary128;
#ifndef __clang__
// Clang will not allow redefining __float128 even is it not enabled
/*! \brief Define __float128 if not defined by the compiler.
 *  Same as __float128 for PPC.  */
typedef vf128_t __float128;
#endif
/*! \brief Define __IBM128 if not defined by the compiler.
 *  Same as old long double for PPC.  */
typedef long double __IBM128;
#endif

/*! \brief Union used to transfer 128-bit data between vector and
 * __float128 types. */
typedef union
     {
       /*! \brief union field of vector unsigned char elements. */
       vui8_t    vx16;
       /*! \brief union field of vector unsigned short elements. */
       vui16_t   vx8;
       /*! \brief union field of vector unsigned int elements. */
       vui32_t   vx4;
       /*! \brief union field of vector unsigned long long elements. */
       vui64_t   vx2;
       /*! \brief union field of vector unsigned __int128 elements. */
       vui128_t  vx1;
       /*! \brief union field of vector __bool __int128 elements. */
       vb128_t vbool1;
       /*! \brief union field of __float128 elements. */
       __binary128 vf1;
     } __VF_128;

/** \brief Transfer function from a __binary128 scalar to a vector char.
*
*  The compiler does not allow direct transfer (assignment or type
*  cast) between __binary128 (__float128) scalars and vector types.
*  This despite the fact the the ABI and ISA require __binary128 in
*  vector registers (VRs).
*
*  \note this function uses a union to effect the (logical) transfer.
*  The compiler should not generate any code for this.
*
*  @param f128 a __binary128 floating point scalar value.
*  @return The original value as a 128-bit vector char.
*/
static inline vui8_t
vec_xfer_bin128_2_vui8t (__binary128 f128)
{
  __VF_128 vunion;

  vunion.vf1 = f128;

  return (vunion.vx16);
}

/** \brief Transfer function from a __binary128 scalar to a vector short int.
*
*  The compiler does not allow direct transfer (assignment or type
*  cast) between __binary128 (__float128) scalars and vector types.
*  This despite the fact the the ABI and ISA require __binary128 in
*  vector registers (VRs).
*
*  \note this function uses a union to effect the (logical) transfer.
*  The compiler should not generate any code for this.
*
*  @param f128 a __binary128 floating point scalar value.
*  @return The original value as a 128-bit vector short int.
*/
static inline vui16_t
vec_xfer_bin128_2_vui16t (__binary128 f128)
{
  __VF_128 vunion;

  vunion.vf1 = f128;

  return (vunion.vx8);
}

/** \brief Transfer function from a __binary128 scalar to a vector int.
*
*  The compiler does not allow direct transfer (assignment or type
*  cast) between __binary128 (__float128) scalars and vector types.
*  This despite the fact the the ABI and ISA require __binary128 in
*  vector registers (VRs).
*
*  \note this function uses a union to effect the (logical) transfer.
*  The compiler should not generate any code for this.
*
*  @param f128 a __binary128 floating point scalar value.
*  @return The original value as a 128-bit vector int.
*/
static inline vui32_t
vec_xfer_bin128_2_vui32t (__binary128 f128)
{
  __VF_128 vunion;

  vunion.vf1 = f128;

  return (vunion.vx4);
}

/** \brief Transfer function from a __binary128 scalar to a vector long long int.
*
*  The compiler does not allow direct transfer (assignment or type
*  cast) between __binary128 (__float128) scalars and vector types.
*  This despite the fact the the ABI and ISA require __binary128 in
*  vector registers (VRs).
*
*  \note this function uses a union to effect the (logical) transfer.
*  The compiler should not generate any code for this.
*
*  @param f128 a __binary128 floating point scalar value.
*  @return The original value as a 128-bit vector long long int.
*/
static inline vui64_t
vec_xfer_bin128_2_vui64t (__binary128 f128)
{
  __VF_128 vunion;

  vunion.vf1 = f128;

  return (vunion.vx2);
}

/** \brief Transfer function from a __binary128 scalar to a vector __int128.
*
*  The compiler does not allow direct transfer (assignment or type
*  cast) between __binary128 (__float128) scalars and vector types.
*  This despite the fact the the ABI and ISA require __binary128 in
*  vector registers (VRs).
*
*  \note this function uses a union to effect the (logical) transfer.
*  The compiler should not generate any code for this.
*
*  @param f128 a __binary128 floating point scalar value.
*  @return The original value as a 128-bit vector __int128.
*/
static inline vui128_t
vec_xfer_bin128_2_vui128t (__binary128 f128)
{
  __VF_128 vunion;

  vunion.vf1 = f128;

  return (vunion.vx1);
}

/** \brief Transfer a vector unsigned char to __binary128 scalar.
 *
 *  The compiler does not allow direct transfer (assignment or type
 *  cast) between __binary128 (__float128) scalars and vector types.
 *  This despite the fact the the ABI and ISA require __binary128 in
 *  vector registers (VRs).
 *
 *  \note this function uses a union to effect the (logical) transfer.
 *  The compiler should not generate any code for this.
 *
 *  @param f128 a vector unsigned char value.
 *  @return The original value returned as a __binary128 scalar.
 */
static inline __binary128
vec_xfer_vui8t_2_bin128 (vui8_t f128)
{
  __VF_128 vunion;

  vunion.vx16 = f128;

  return (vunion.vf1);
}

/** \brief Transfer a vector unsigned short to __binary128 scalar.
 *
 *  The compiler does not allow direct transfer (assignment or type
 *  cast) between __binary128 (__float128) scalars and vector types.
 *  This despite the fact the the ABI and ISA require __binary128 in
 *  vector registers (VRs).
 *
 *  \note this function uses a union to effect the (logical) transfer.
 *  The compiler should not generate any code for this.
 *
 *  @param f128 a vector unsigned short value.
 *  @return The original value returned as a __binary128 scalar.
 */
static inline __binary128
vec_xfer_vui16t_2_bin128 (vui16_t f128)
{
  __VF_128 vunion;

  vunion.vx8 = f128;

  return (vunion.vf1);
}

/** \brief Transfer a vector unsigned int to __binary128 scalar.
 *
 *  The compiler does not allow direct transfer (assignment or type
 *  cast) between __binary128 (__float128) scalars and vector types.
 *  This despite the fact the the ABI and ISA require __binary128 in
 *  vector registers (VRs).
 *
 *  \note this function uses a union to effect the (logical) transfer.
 *  The compiler should not generate any code for this.
 *
 *  @param f128 a vector unsigned int value.
 *  @return The original value returned as a __binary128 scalar.
 */
static inline __binary128
vec_xfer_vui32t_2_bin128 (vui32_t f128)
{
  __VF_128 vunion;

  vunion.vx4 = f128;

  return (vunion.vf1);
}

/** \brief Transfer a vector unsigned long long  to __binary128 scalar.
 *
 *  The compiler does not allow direct transfer (assignment or type
 *  cast) between __binary128 (__float128) scalars and vector types.
 *  This despite the fact the the ABI and ISA require __binary128 in
 *  vector registers (VRs).
 *
 *  \note this function uses a union to effect the (logical) transfer.
 *  The compiler should not generate any code for this.
 *
 *  @param f128 a vector unsigned long long value.
 *  @return The original value returned as a __binary128 scalar.
 */
static inline __binary128
vec_xfer_vui64t_2_bin128 (vui64_t f128)
{
  __VF_128 vunion;

  vunion.vx2 = f128;

  return (vunion.vf1);
}

/** \brief Transfer a vector unsigned __int128 to __binary128 scalar.
 *
 *  The compiler does not allow direct transfer (assignment or type
 *  cast) between __binary128 (__float128) scalars and vector types.
 *  This despite the fact the the ABI and ISA require __binary128 in
 *  vector registers (VRs).
 *
 *  \note this function uses a union to effect the (logical) transfer.
 *  The compiler should not generate any code for this.
 *
 *  @param f128 a vector unsigned __int128 value.
 *  @return The original value returned as a __binary128 scalar.
 */
static inline __binary128
vec_xfer_vui128t_2_bin128 (vui128_t f128)
{
  __VF_128 vunion;

  vunion.vx1 = f128;

  return (vunion.vf1);
}

/** \brief Clear the sign bit of __float128 input
 *  and return the resulting positive __float128 value.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-11  | 2/cycle  |
 *  |power9   | 2     | 4/cycle  |
 *
 *  @param f128 a __float128 value containing a signed value.
 *  @return a __float128 value with magnitude from f128 and a positive
 *  sign of f128.
 */
static inline __binary128
vec_absf128 (__binary128 f128)
{
  __binary128 result;
#if _ARCH_PWR9
  __asm__(
      "xsabsqp %0,%1;\n"
      : "=v" (result)
      : "v" (f128)
      :);
#else
  vui32_t tmp;
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  tmp = vec_xfer_bin128_2_vui32t (f128);
  tmp = vec_andc (tmp, signmask);
  result = vec_xfer_vui32t_2_bin128 (tmp);
#endif
  return (result);
}

/** \brief Return true if the __float128 value is Finite
 *  (Not NaN nor Inf).
 *
 *  A IEEE Binary128 finite value has an exponent between 0x0000 and
 *  0x7ffe (a 0x7fff indicates NaN or Inf).  The significand can be
 *  any value.
 *  Using the !vec_all_eq compare conditional verify this condition and
 *  avoids a vector -> GPR transfer for platforms before PowerISA-2.07.
 *  The sign bit is ignored.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-20  | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal __float128 compare can.
 *
 *  @param f128 a __float128 value in vector.
 *  @return an int containing 0 or 1.
 */
static inline int
vec_all_isfinitef128 (__binary128 f128)
{
#if defined (_ARCH_PWR9) && defined (scalar_test_data_class) && defined (__FLOAT128__) && (__GNUC__ > 7)
  return !scalar_test_data_class (f128, 0x70);
#else
  vui32_t tmp, t128;
  const vui32_t expmask = CONST_VINT128_W(0x7fff0000, 0, 0, 0);

  t128 = vec_xfer_bin128_2_vui32t (f128);
  tmp = vec_and (t128, expmask);
  return !vec_all_eq(tmp, expmask);
#endif
}

/** \brief Return true if the __float128 value is infinity.
 *
 *  A IEEE Binary128 infinity has a exponent of 0x7fff and significand
 *  of all zeros.  Using the vec_all_eq compare conditional verifies
 *  both conditions and avoids a vector -> GPR transfer for platforms
 *  before PowerISA-2.07.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-20  | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal __float128 compare can.
 *
 *  @param f128 a __float128 value in vector.
 *  @return an int containing 0 or 1.
 */
static inline int
vec_all_isinff128 (__binary128 f128)
{
#if defined (_ARCH_PWR9) && defined (scalar_test_data_class) && defined (__FLOAT128__) && (__GNUC__ > 7)
  return scalar_test_data_class (f128, 0x30);
#else
  vui32_t tmp;
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  const vui32_t expmask = CONST_VINT128_W(0x7fff0000, 0, 0, 0);

  tmp = vec_xfer_bin128_2_vui32t (f128);
  tmp = vec_andc (tmp, signmask);
  return vec_all_eq(tmp, expmask);
#endif
}

/** \brief Return true if the __float128 value is Not a Number (NaN).
 *
 *  A IEEE Binary128 NaN has a exponent of 0x7fff and nonzero
 *  significand.  Using the combined vec_all_eq / vec_any_gt compare
 *  conditional verify both conditions and avoids a vector -> GPR
 *  transfer for platforms before PowerISA-2.07.
 *  The sign bit is ignored.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-29  | 1/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal __float128 compare can.
 *
 *  @param f128 a __float128 value in vector.
 *  @return an int containing 0 or 1.
 */
static inline int
vec_all_isnanf128 (__binary128 f128)
{
#if defined (_ARCH_PWR9) && defined (scalar_test_data_class) && defined (__FLOAT128__) && (__GNUC__ > 7)
  return scalar_test_data_class (f128, 0x40);
#else
  vui32_t tmp, tmp2, t128;
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  const vui32_t expmask = CONST_VINT128_W(0x7fff0000, 0, 0, 0);

  t128 = vec_xfer_bin128_2_vui32t (f128);
  tmp = vec_andc (t128, signmask);
  tmp2 = vec_and (t128, expmask);
  return (vec_all_eq (tmp2, expmask) && vec_any_gt(tmp, expmask));
#endif
}

/** \brief Return true if the __float128 value is normal
 *  (Not NaN, Inf, denormal, or zero).
 *
 *  A IEEE Binary128 normal value has an exponent between 0x0001 and
 *  0x7ffe (a 0x7fff indicates NaN or Inf).  The significand can be
 *  any value (expect 0 if the exponent is zero).
 *  Using the combined vec_all_ne compares conditional verify both
 *  conditions and avoids a vector -> GPR transfer for platforms
 *  before PowerISA-2.07.  The sign bit is ignored.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-29  | 1/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal __float128 compare can.
 *
 *  @param f128 a __float128 value in vector.
 *  @return an int containing 0 or 1.
 */
static inline int
vec_all_isnormalf128 (__binary128 f128)
{
#if defined (_ARCH_PWR9) && defined (scalar_test_data_class) && defined (__FLOAT128__) && (__GNUC__ > 7)
  return !scalar_test_data_class (f128, 0x7f);
#else
  vui32_t tmp, t128;
  const vui32_t expmask = CONST_VINT128_W(0x7fff0000, 0, 0, 0);
  const vui32_t vec_zero = CONST_VINT128_W(0, 0, 0, 0);

  t128 = vec_xfer_bin128_2_vui32t (f128);
  tmp = vec_and (t128, expmask);

  return !(vec_all_eq (tmp, expmask) || vec_all_eq(tmp, vec_zero));
#endif
}

/** \brief Return true if the __float128
 *  value is subnormal (denormal).
 *
 *  A IEEE Binary128 subnormal has an exponent of 0x0000 and a
 *  nonzero significand.
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal __float128 compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8-29  | 1/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param f128 a vector of __binary128 values.
 *  @return a boolean int, true if the __float128 value is
 *  subnormal.
 */
static inline int
vec_all_issubnormalf128 (__binary128 f128)
{
#if defined (_ARCH_PWR9) && defined (scalar_test_data_class) && defined (__FLOAT128__) && (__GNUC__ > 7)
  return scalar_test_data_class (f128, 0x03);
#else
  const vui64_t minnorm = CONST_VINT128_DW(0x0001000000000000UL, 0UL);
  const vui64_t vec_zero = CONST_VINT128_DW(0, 0);
  vui128_t tmp1;

  tmp1 = vec_xfer_bin128_2_vui128t (vec_absf128 (f128));
  return vec_cmpuq_all_gt ((vui128_t) minnorm, tmp1)
      && !vec_cmpuq_all_eq (tmp1, (vui128_t) vec_zero);
#endif
}

/** \brief Return true if the __float128
 *  value is +-0.0.
 *
 *  A IEEE Binary128 zero has an exponent of 0x0000 and a
 *  zero significand.
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal __float128 compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-20  | 1/cycle  |
 *  |power9   |  3    | 2/cycle  |
 *
 *  @param f128 a vector of __binary64 values.
 *  @return a boolean int, true if the __float128 value is
 *  +/- zero.
 */
static inline int
vec_all_iszerof128 (__binary128 f128)
{
#if defined (_ARCH_PWR9) && defined (scalar_test_data_class) && defined (__FLOAT128__) && (__GNUC__ > 7)
  return scalar_test_data_class (f128, 0x0c);
#else
  vui64_t tmp2;
  const vui64_t vec_zero = CONST_VINT128_DW(0, 0);

  tmp2 = vec_xfer_bin128_2_vui64t (vec_absf128 (f128));
#if _ARCH_PWR8
  return vec_all_eq(tmp2, vec_zero);
#else
  return vec_all_eq((vui32_t)tmp2, (vui32_t)vec_zero);
#endif
#endif
}

/** \brief Copy the sign bit from f128y and merge with the magnitude
 *  from f128x. The merged result is returned as a __float128 value.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-11  | 2/cycle  |
 *  |power9   | 2     | 4/cycle  |
 *
 *  @param f128x a __float128 value containing the magnitude.
 *  @param f128y a __float128 value containing the sign bit.
 *  @return a __float128 value with magnitude from f128x and the
 *  sign of f128y.
 */
static inline __binary128
vec_copysignf128 (__binary128 f128x, __binary128 f128y)
{
  __binary128 result;
#if _ARCH_PWR9
  __asm__(
      "xscpsgnqp %0,%2,%1;\n"
      : "=v" (result)
      : "v" (f128x), "v" (f128y)
      :);
#else
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vui32_t tmpx, tmpy, tmp;
  tmpx = vec_xfer_bin128_2_vui32t (f128x);
  tmpy = vec_xfer_bin128_2_vui32t (f128y);

  tmp = vec_sel (tmpx, tmpy, signmask);
  result = vec_xfer_vui32t_2_bin128 (tmp);
#endif
  return (result);
}

/** \brief return a positive infinity.
 *
 *  @return const __float128 positive infinity.
 */
static inline __binary128
vec_const_huge_valf128 ()
{
  const vui32_t posinf = CONST_VINT128_W(0x7fff0000, 0, 0, 0);

  return vec_xfer_vui32t_2_bin128 (posinf);
}

/** \brief return a positive infinity.
 *
 *  @return a const __float128 positive infinity.
 */
static inline __binary128
vec_const_inff128 ()
{
  const vui32_t posinf = CONST_VINT128_W(0x7fff0000, 0, 0, 0);

  return vec_xfer_vui32t_2_bin128 (posinf);
}

/** \brief return a quiet NaN.
 *
 *  @return a const __float128 quiet NaN.
 */
static inline __binary128
vec_const_nanf128 ()
{
  const vui32_t posnan = CONST_VINT128_W(0x7fff8000, 0, 0, 0);

  return vec_xfer_vui32t_2_bin128 (posnan);
}

/** \brief return a signaling NaN.
 *
 *  @return a const __float128 signaling NaN.
 */
static inline __binary128
vec_const_nansf128 ()
{
  const vui32_t signan = CONST_VINT128_W(0x7fff4000, 0, 0, 0);

  return vec_xfer_vui32t_2_bin128 (signan);
}

/** \brief Return 128-bit vector boolean true if the __float128 value
 *  is Finite (Not NaN nor Inf).
 *
 *  A IEEE Binary128 finite value has an exponent between 0x0000 and
 *  0x7ffe (a 0x7fff indicates NaN or Inf).  The significand can be
 *  any value.
 *  Using the vec_cmpeq conditional to generate the predicate mask for
 *  NaN / Inf and then invert this for the finite condition.
 *  The sign bit is ignored.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8-17  | 2/cycle  |
 *  |power9   |   6   | 2/cycle  |
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal __float128 compare can.
 *
 *  @param f128 a __float128 value in vector.
 *  @return a vector boolean containing all 0s or 1s.
 */
static inline vb128_t
vec_isfinitef128 (__binary128 f128)
{
#if defined (_ARCH_PWR9) && defined (scalar_test_data_class) && defined (__FLOAT128__) && (__GNUC__ > 7)
  vui32_t result = CONST_VINT128_W(-1, -1, -1, -1);

  if (scalar_test_data_class (f128, 0x70))
    result = CONST_VINT128_W(0, 0, 0, 0);

  return (vb128_t)result;
#else
  const vui32_t expmask = CONST_VINT128_W(0x7fff0000, 0, 0, 0);
  vui32_t tmp, t128;
  vb128_t tmp2, tmp3;

  t128 = vec_xfer_bin128_2_vui32t (f128);
  tmp = vec_and (t128, expmask);
  tmp2 = (vb128_t) vec_cmpeq (tmp, expmask);
  tmp3 = (vb128_t) vec_splat ((vui32_t) tmp2, VEC_W_H);
  return (vb128_t) vec_nor ((vui32_t) tmp3, (vui32_t) tmp3); // vec_not
#endif
}

/** \brief Return true (nonzero) value if the __float128 value is
 * infinity. For infinity indicate the sign as +1 for positive infinity
 * and -1 for negative infinity.
 *
 *  A IEEE Binary128 infinity has a exponent of 0x7fff and significand
 *  of all zeros.  Using the vec_all_eq compare conditional verifies
 *  both conditions. A subsequent vec_any_gt checks the sign bit and
 *  set the result appropriately. The sign bit is ignored.
 *
 *  This sequence avoids a vector -> GPR transfer for platforms
 *  before PowerISA-2.07.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 12-32 | 1/cycle  |
 *  |power9   | 3-12  | 2/cycle  |
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal __float128 compare can.
 *
 *  @param f128 a __float128 value in vector.
 *  @return an int containing 0 if not infinity and +1/-1 otherwise.
 */
static inline int
vec_isinf_signf128 (__binary128 f128)
{
  int result;
#if defined (_ARCH_PWR9) && defined (scalar_test_data_class) && defined (__FLOAT128__) && (__GNUC__ > 7)
  if (scalar_test_data_class (f128, 0x20))
    result = 1;
  else if (scalar_test_data_class (f128, 0x10))
    result = -1;
  else
    result = 0;
#else
  vui32_t tmp, t128;
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  const vui32_t expmask = CONST_VINT128_W(0x7fff0000, 0, 0, 0);

  t128 = vec_xfer_bin128_2_vui32t (f128);
  tmp = vec_andc (t128, signmask);

  if (vec_all_eq(tmp, expmask))
    {
      if (vec_any_gt(t128, expmask))
	result = -1;
      else
	result = 1;
    }
  else
    result = 0;
#endif
  return (result);
}

/** \brief Return a 128-bit vector boolean true if the __float128 value
 *  is infinity.
 *
 *  A IEEE Binary128 infinity has a exponent of 0x7fff and significand
 *  of all zeros.
 *  The sign bit is ignored.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8-17  | 2/cycle  |
 *  |power9   |   6   | 2/cycle  |
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal __float128 compare can.
 *
 *  @param f128 a __float128 value in vector.
 *  @return a vector boolean containing all 0s or 1s..
 */
static inline vb128_t
vec_isinff128 (__binary128 f128)
{
#if defined (_ARCH_PWR9) && defined (scalar_test_data_class) && defined (__FLOAT128__) && (__GNUC__ > 7)
  vui32_t result = CONST_VINT128_W(0, 0, 0, 0);

  if (scalar_test_data_class (f128, 0x30))
    result = CONST_VINT128_W(-1, -1, -1, -1);

  return (vb128_t)result;
#else
  vui32_t tmp, t128;
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  const vui32_t expmask = CONST_VINT128_W(0x7fff0000, 0, 0, 0);

  t128 = vec_xfer_bin128_2_vui32t (f128);
  tmp = vec_andc (t128, signmask);
  return vec_cmpequq ((vui128_t)tmp , (vui128_t)expmask);
#endif
}

/** \brief Return 128-bit vector boolean true if the __float128 value
 *  is Not a Number (NaN).
 *
 *  A IEEE Binary128 NaN has a exponent of 0x7fff and nonzero
 *  significand.  This requires a combination of verifying the
 *  exponent and that any bit of the significand is nonzero.
 *  Using the combined vec_all_eq / vec_any_gt compare
 *  conditional verify both conditions before negating the result
 *  from zero to all ones..
 *  The sign bit is ignored.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-19 | 1/cycle  |
 *  |power9   |   6   | 2/cycle  |
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal __float128 compare can.
 *
 *  @param f128 a __float128 value in vector.
 *  @return a vector boolean containing all 0s or 1s.
 */
static inline vb128_t
vec_isnanf128 (__binary128 f128)
{
#if defined (_ARCH_PWR9) && defined (scalar_test_data_class) && defined (__FLOAT128__) && (__GNUC__ > 7)
  vui32_t result = CONST_VINT128_W(0, 0, 0, 0);

  if (scalar_test_data_class (f128, 0x40))
    result = CONST_VINT128_W(-1, -1, -1, -1);

  return (vb128_t)result;
#else
  vui32_t tmp, t128;
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  const vui32_t expmask = CONST_VINT128_W(0x7fff0000, 0, 0, 0);

  t128 = vec_xfer_bin128_2_vui32t (f128);
  tmp = vec_andc (t128, signmask);
  return vec_cmpgtuq ((vui128_t)tmp , (vui128_t)expmask);
#endif
}

/** \brief Return 128-bit vector boolean true if the __float128 value
 *  is normal (Not NaN, Inf, denormal, or zero).
 *
 *  A IEEE Binary128 normal value has an exponent between 0x0001 and
 *  0x7ffe (a 0x7fff indicates NaN or Inf).  The significand can be
 *  any value (expect 0 if the exponent is zero).
 *  The sign bit is ignored.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-19 | 2/cycle  |
 *  |power9   |   6   | 2/cycle  |
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal __float128 compare can.
 *
 *  @param f128 a __float128 value in vector.
 *  @return a vector boolean containing all 0s or 1s.
 */
static inline vb128_t
vec_isnormalf128 (__binary128 f128)
{
#if defined (_ARCH_PWR9) && defined (scalar_test_data_class) && defined (__FLOAT128__) && (__GNUC__ > 7)
  vui32_t result = CONST_VINT128_W(-1, -1, -1, -1);

  if (scalar_test_data_class (f128, 0x7f))
    result = CONST_VINT128_W(0, 0, 0, 0);

  return (vb128_t)result;
#else
  vui32_t tmp, t128;
  const vui32_t expmask = CONST_VINT128_W(0x7fff0000, 0, 0, 0);
  const vui32_t vec_zero = CONST_VINT128_W(0, 0, 0, 0);
  vb128_t result;

  t128 = vec_xfer_bin128_2_vui32t (f128);
  tmp = vec_and (t128, expmask);
  result = (vb128_t) vec_nor (vec_cmpeq (tmp, expmask),
			      vec_cmpeq (tmp, vec_zero));
  result = (vb128_t) vec_splat ((vui32_t) result, VEC_W_H);

  return (result);
#endif
}

/** \brief Return 128-bit vector boolean true value,
 *  if the __float128 value is subnormal (denormal).
 *
 *  A IEEE Binary128 subnormal has an exponent of 0x0000 and a
 *  nonzero significand.
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal __float128 compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 16-25 | 1/cycle  |
 *  |power9   |   6   | 1/cycle  |
 *
 *  @param f128 a vector of __binary64 values.
 *  @return a vector boolean long long, each containing all 0s(false)
 *  or 1s(true).
 */
static inline vb128_t
vec_issubnormalf128 (__binary128 f128)
{
#if defined (_ARCH_PWR9) && defined (scalar_test_data_class) && defined (__FLOAT128__) && (__GNUC__ > 7)
  vui32_t result = CONST_VINT128_W(0, 0, 0, 0);

  if (scalar_test_data_class (f128, 0x03))
    result = CONST_VINT128_W(-1, -1, -1, -1);

  return (vb128_t)result;
#else
  vui32_t tmp, tmpz, tmp2, t128;
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  const vui32_t vec_zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t minnorm = CONST_VINT128_W(0x00010000, 0, 0, 0);

  t128 = vec_xfer_bin128_2_vui32t (f128);
  tmp = vec_andc (t128, signmask);
  tmp2 = (vui32_t) vec_cmpltuq ((vui128_t)tmp, (vui128_t)minnorm);
  tmpz = (vui32_t) vec_cmpequq ((vui128_t)tmp, (vui128_t)vec_zero);
  return (vb128_t) vec_andc (tmp2, tmpz);
#endif
}

/** \brief Return 128-bit vector boolean true value, if the
 *  value that is +-0.0.
 *
 *  A IEEE Binary64 zero has an exponent of 0x000 and a
 *  zero significand.
 *  The sign bit is ignored.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal __float128 compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8-17  | 2/cycle  |
 *  |power9   |   6   | 2/cycle  |
 *
 *  @param f128 a vector of __binary32 values.
 *  @return a vector boolean int, each containing all 0s(false)
 *  or 1s(true).
 */
static inline vb128_t
vec_iszerof128 (__binary128 f128)
{
#if defined (_ARCH_PWR9) && defined (scalar_test_data_class) && defined (__FLOAT128__) && (__GNUC__ > 7)
  vui32_t result = CONST_VINT128_W(0, 0, 0, 0);

  if (scalar_test_data_class (f128, 0x0c))
    result = CONST_VINT128_W(-1, -1, -1, -1);

  return (vb128_t)result;
#else
  vui128_t t128;
  const vui64_t vec_zero = CONST_VINT128_DW(0, 0);

  t128 = vec_xfer_bin128_2_vui128t (vec_absf128(f128));
  return  (vb128_t)vec_cmpequq (t128, (vui128_t)vec_zero);
#endif
}

/*! \brief Vector Set Bool from Quadword Floating-point.
 *
 *  If the quadword's sign bit is '1' then return a vector bool
 *  __int128 that is all '1's. Otherwise return all '0's.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4 - 6 | 2/cycle  |
 *  |power9   |   6   | 2/cycle  |
 *
 *  @param f128 a 128-bit vector treated a signed __int128.
 *  @return a 128-bit vector bool of all '1's if the sign bit is '1'.
 *  Otherwise all '0's.
 */
static inline vb128_t
vec_setb_qp (__binary128 f128)
{
#if defined (_ARCH_PWR9) && defined (scalar_test_neg) && (__GNUC__ > 7)
  vui32_t result = CONST_VINT128_W(0, 0, 0, 0);

  if (scalar_test_neg (f128))
    result = CONST_VINT128_W(-1, -1, -1, -1);

  return (vb128_t)result;
#else
  const vui8_t shift = vec_splat_u8 (7);
  vui8_t t128 = vec_xfer_bin128_2_vui8t (f128);
  vui8_t splat = vec_splat (t128, VEC_BYTE_H);

  return (vb128_t) vec_sra (splat, shift);
#endif
}

/** \brief Return int boolean true if the __float128 value
 *  is negative (sign bit is '1').
 *
 *  Anding with a signmask and then vec_all_eq compare with that mask
 *  generates the boolean of the sign bit.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-20  | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param f128 a __float128 value in vector.
 *  @return a int boolean indicating the sign bit.
 */
static inline int
vec_signbitf128 (__binary128 f128)
{
#if defined (_ARCH_PWR9) && defined (scalar_test_neg) && (__GNUC > 7)
  return (scalar_test_neg (f128);
#else
  vui32_t tmp, t128;
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);

  t128 = vec_xfer_bin128_2_vui32t (f128);
  tmp = vec_and (t128, signmask);
  return vec_all_eq(tmp, signmask);
#endif
}

#endif /* VEC_F128_PPC_H_ */
