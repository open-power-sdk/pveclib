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

#include <vec_common_ppc.h>
/*!
 * \file  vec_f32_ppc.h
 * \brief Header package containing a collection of 128-bit SIMD
 * operations over 32-bit floating point elements.
 *
 * Most vector float (32-bit float) operations are implemented
 * with PowerISA VMX instructions either defined by the original VMX
 * (AKA Altivec) or added to later versions of the PowerISA.
 * POWER8 added the Vector Scalar Extended (VSX) with access to
 * additional vector (64) registers and operations.
 * Most of these intrinsic (compiler built-ins) operations are defined
 * in <altivec.h> and described in the compiler documentation.
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
 * Most of these operations are implemented in a single instruction
 * on newer (POWER8/POWER9) processors.
 * This header serves to fill in functional gaps for older
 * (POWER7, POWER8) processors and provides a in-line assembler
 * implementation for older compilers that do not
 * provide the build-ins.
 *
 * This header covers operations that are either:
 *
 * - Implemented in hardware instructions for later
 * processors and useful to programmers, on slightly older processors,
 * even if the equivalent function requires more instructions.
 * Examples include the multiply even/odd/modulo word operations.
 * - Defined in the OpenPOWER ABI but <I>not</I> yet defined in
 * <altivec.n> provided by available compilers in common use.
 * Examples include vector float even/odd.
 * - Provide special vector float tests for special conditions
 * without generating extraneous floating-point exceptions.
 * This is important for implementing vectorized forms of ISO C99 Math
 * functions.
 * - Commonly used operations, not covered by the ABI or
 * <altivec.h>, and require multiple instructions or
 * are not obvious.
 * See example above.
 *
 * \section f32_sub_0_0_1 Performance data.
 * It is useful to provide basic performance data for each pveclib
 * function.  This is challenging as these functions are small and
 * intended to be in-lined within larger functions (algorithms).
 * As such they are subject to both the compilers instruction
 * scheduling and common subexpression optimizations plus the
 * processors super-scalar and out-of-order execution design features.
 *
 * As pveclib functions are normally only a few
 * instructions, the actual timing will depend on the context it
 * is in (the instructions that is depends on for data and instructions
 * that proceed them in the pipelines).
 *
 * The simplest approach is to use the same performance metrics as the
 * Power Processor Users Manuals Performance Profile.
 * This is normally per instruction latency in cycles and
 * throughput in instructions issued per cycle. There may also be
 * additional information for special conditions that may apply.
 *
 * For example the vector float absolute value function.
 * For recent PowerISA implementations this a single
 * (VSX <B>xvabssp</B>) instruction which we can look up in the
 * POWER8 / POWER9 Processor User's Manuals (<B>UM</B>).
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-7   | 2/cycle  |
 *  |power9   | 2     | 2/cycle  |
 *
 * The POWER8 UM specifies a latency of
 * <I>"6 cycles to FPU (+1 cycle to other VSU ops"</I>
 * for this class of VSX single precision FPU instructions.
 * So the minimum latency is 6 cycles if the register result is input
 * to another VSX single precision FPU instruction.
 * Otherwise if the result is input to a VSU logical or integer
 * instruction then the latency is 7 cycles.
 * The POWER9 UM shows the pipeline improvement of 2 cycles latency
 * for simple FPU instructions like this.
 * Both processors support dual pipelines for a 2/cycle throughput
 * capability.
 *
 * A more complicated example: \code
static inline vb32_t
vec_isnanf32 (vf32_t vf32)
{
  vui32_t tmp2;
  const vui32_t expmask = CONST_VINT128_W(0x7f800000, 0x7f800000, 0x7f800000,
					  0x7f800000);
#if _ARCH_PWR9
  // P9 has a 2 cycle xvabssp and eliminates a const load.
  tmp2 = (vui32_t) vec_abs (vf32);
#else
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000,
					   0x80000000);
  tmp2 = vec_andc ((vui32_t)vf32, signmask);
#endif
  return vec_cmpgt (tmp2, expmask);
}
 * \endcode
 * Here we want to test for <I>Not A Number</I> without triggering any
 * of the associate floating-point exceptions (VXSNAN or VXVC).
 * For this test the sign bit does not effect the result so we need to
 * zero the sign bit before the actual test. The vector abs would work
 * for this, but we know from the example above that this instruction
 * has a high latency as we are definitely passing the result to a
 * non-FPU instruction (vector compare greater than unsigned word).
 *
 * So the code needs to load two constant vectors masks, then vector
 * and-compliment to clear the sign-bit, before comparing each word
 * for greater then infinity. The generated code should look
 * something like this: \code
 	addis   r9,r2,.rodata.cst16+0x10@ha
 	addis   r10,r2,.rodata.cst16+0x20@ha
 	addi    r9,r9,.rodata.cst16+0x10@l
 	addi    r10,r10,.rodata.cst16+0x20@l
 	lvx     v0,0,r10	# load vector const signmask
 	lvx     v12,0,r9	# load vector const expmask
 	xxlandc vs34,vs34,vs32
 	vcmpgtuw v2,v2,v12
 * \endcode
 * So six instructions to load the const masks and two instructions
 * for the actual vec_isnanf32 function. The first six instructions
 * are only needed once for each containing function, can be hoisted
 * out of loops and into the function prologue, can be <I>commoned</I>
 * with the same constant for other pveclib functions, or executed
 * out-of-order and early by the processor.
 *
 * Most of the time, constant setup does not contribute measurably to
 * the over all performance of vec_isnanf32. When it does it is
 * limited by the longest (in cycles latency) of the various
 * independent paths that load constants.  In this case the const load
 * sequence is composed of three pairs of instructions that can issue
 * and execute in parallel. The addis/addi FXU instructions supports
 * throughput of 6/cycle and the lvx load supports 2/cycle.
 * So the two vector constant load sequences can execute
 * in parallel and the latency is same as a single const load.
 *
 * For POWER8 it appears to be (2+2+5=) 9 cycles latency for the const
 * load. While the core vec_isnanf32 function (xxlandc/vcmpgtuw) is a
 * dependent sequence and runs (2+2) 4 cycles latency.
 * Similar analysis for POWER9 where the addis/addi/lvx sequence is
 * still listed as (2+2+5) 9 cycles latency. While the xxlandc/vcmpgtuw
 * sequence increases to (2+3) 5 cycles.
 *
 * The next interesting question is what can we say about throughput
 * (if anything) for this example.  The thought experiment is "what
 * would happen if?";
 * - two or more instances of vec_isnanf32 are used within a single
 * function,
 * - in close proximity in the code,
 * - with independent data as input,
 *
 * could the generated instructions execute in parallel and to what
 * extent. This illustrated by the following (contrived) example:
 * \code
int
test512_all_f32_nan (vf32_t val0, vf32_t val1, vf32_t val2, vf32_t val3)
{
  const vb32_t alltrue = { -1, -1, -1, -1 };
  vb32_t nan0, nan1, nan2, nan3;

  nan0 = vec_isnanf32 (val0);
  nan1 = vec_isnanf32 (val1);
  nan2 = vec_isnanf32 (val2);
  nan3 = vec_isnanf32 (val3);

  nan0 = vec_and (nan0, nan1);
  nan2 = vec_and (nan2, nan3);
  nan0 = vec_and (nan2, nan0);

  return vec_all_eq(nan0, alltrue);
}
 * \endcode
 * which tests 4 X vector float (16 X float) values and returns true
 * if all 16 floats are NaN. Recent compilers will generates something
 * like the following PowerISA code:
 * \code
     addis   r9,r2,-2
     addis   r10,r2,-2
     vspltisw v13,-1	# load vector const alltrue
     addi    r9,r9,21184
     addi    r10,r10,-13760
     lvx     v0,0,r9	# load vector const signmask
     lvx     v1,0,r10	# load vector const expmask
     xxlandc vs35,vs35,vs32
     xxlandc vs34,vs34,vs32
     xxlandc vs37,vs37,vs32
     xxlandc vs36,vs36,vs32
     vcmpgtuw v3,v3,v1	# nan1 = vec_isnanf32 (val1);
     vcmpgtuw v2,v2,v1	# nan0 = vec_isnanf32 (val0);
     vcmpgtuw v5,v5,v1	# nan3 = vec_isnanf32 (val3);
     vcmpgtuw v4,v4,v1	# nan2 = vec_isnanf32 (val2);
     xxland  vs35,vs35,vs34	# nan0 = vec_and (nan0, nan1);
     xxland  vs36,vs37,vs36	# nan2 = vec_and (nan2, nan3);
     xxland  vs36,vs35,vs36	# nan0 = vec_and (nan2, nan0);
     vcmpequw. v4,v4,v13	# vec_all_eq(nan0, alltrue);
...
 * \endcode
 * first the generated code loading the vector constants for signmask,
 * expmask, and alltrue. We see that the code is generated only once
 * for each constant. Then the compiler generate the core vec_isnanf32
 * function four times and interleaves the instructions. This enables
 * parallel pipeline execution where conditions allow. Finally the 16X
 * isnan results are reduced to 8X, then 4X, then to a single
 * condition code.
 *
 * For this exercise we will ignore the constant load as in any
 * realistic usage it will be <I>commoned</I> across several pveclib
 * functions and hoisted out of any loops. The reduction code is not
 * part of the vec_isnanf32 implementation and also ignored.
 * The sequence of 4X xxlandc and 4X vcmpgtuw in the middle it the
 * interesting part.
 *
 * For POWER8 both xxlandc and vcmpgtuw are listed as 2 cycles
 * latency and throughput of 2 per cycle. So we can assume that (only)
 * the first two xxlandc will issue in the same cycle (assuming the
 * input vectors are ready).  The issue of the next two xxlandc
 * instructions will be delay by 1 cycle. The following vcmpgtuw
 * instruction are dependent on the xxlandc results and will not
 * execute until their input vectors are ready. The first two vcmpgtuw
 * instruction will execute 2 cycles (latency) after the first two
 * xxlandc instructions execute. Execution of the second two vcmpgtuw
 * instructions will be delayed 1 cycle due to the issue delay in the
 * second pair of xxlandc instructions.
 *
 * So at least for this example and this set of simplifying assumptions
 * we suggest that the throughput metric for vec_isnanf32 is 2/cycle.
 * For latency metric we offer the range with the latency for the core
 * function (without and constant load overhead) first. Followed by the
 * total latency (the sum of the constant load and core function
 * latency). For the vec_isnanf32 example the metrics are:
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-13  | 2/cycle  |
 *  |power9   | 5-14  | 2/cycle  |
 *
 */

#include <vec_common_ppc.h>

/*! \brief typedef __vbinary32 to vector of float elements. */
typedef vf32_t __vbinary32;

/** \brief Vector float absolute value.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-7   | 2/cycle  |
 *  |power9   | 2     | 2/cycle  |
 *
 *  @param vf32x vector float values containing the magnitudes.
 *  @return vector float absolute values of vf32x.
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
 *  |power8   | 4-15  | 2/cycle  |
 *  |power9   | 6     | 2/cycle  |
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
 *  |power8   | 4-15  | 2/cycle  |
 *  |power9   | 6     | 2/cycle  |
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
 *  |power8   | 11-18 | 1/cycle  |
 *  |power9   | 14    | 1/cycle  |
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

  result = vec_all_gt(vnorm, vec_zero);

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
 *  |power8   | 10-18 | 1/cycle  |
 *  |power9   | 14    | 1/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a boolean int, true if all of 4 vector float values are
 *  subnormal.
 */
static inline int
vec_all_issubnormalf32 (vf32_t vf32)
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
  result = vec_all_ne(vsubnorm, vec_zero);

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
 *  |power8   | 6-12  | 2/cycle  |
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
 *  @param vf32 a vector of __binary32 values.
 *  @return a boolean int, true if aany of 4 vector float values are
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
 *  |power8   | 11-18 | 1/cycle  |
 *  |power9   | 14    | 1/cycle  |
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
 *  |power8   | 6-12  | 2/cycle  |
 *  |power9   | 5     | 2/cycle  |
 *
 *  @param vf32 a vector of __binary32 values.
 *  @return a boolean int, true if aany of 4 vector float values are
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
 *  |power8   | 4-15  | 2/cycle  |
 *  |power9   | 6     | 2/cycle  |
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
 *  |power8   | 9-16  | 1/cycle  |
 *  |power9   | 14    | 1/cycle  |
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
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000, 0x80000000);
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
 *  |power8   | 8-16  | 2/cycle  |
 *  |power9   | 14    | 2/cycle  |
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
 *  |power8   | 6-12  | 2/cycle  |
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
