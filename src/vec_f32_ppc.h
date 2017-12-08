/*
 Copyright [2017] IBM Corporation.

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
/*
 * vec_f32_ppc.h
 *
 *  Created on: Apr 13, 2016
 *      Author: sjmunroe
 */

#ifndef VEC_F32_PPC_H_
#define VEC_F32_PPC_H_

/* define vf32_t etc.  */
#include <vec_common_ppc.h>

typedef vf32_t __vbinary32;

typedef __vector __bool int __f32_bool;

/** \brief Copy the sign bit from vf32y merged with magnitude from
 * vf32x and return the resulting vector float values.
 *
 *	@param vf32x vector float values containing the magnitudes.
 *	@param vf32y vector float values containing the sign bits.
 *	@return vector float values with magnitude from vf32x and the
 *	sign of vf32y.
 */
static inline vf32_t
vec_copysignf32 (vf32_t vf32x , vf32_t vf32y)
{
#if _ARCH_PWR7
	return (vec_cpsgn (vf32x, vf32y));
#else
	const vui32_t signmask  = CONST_VINT128_W(0x80000000, 0x80000000,
											0x80000000, 0x80000000);
	vf32_t result;

	result  = (vf32_t)vec_sel ((vui32_t)vf32x, (vui32_t)vf32y, signmask);
	return (result);
#endif
}

/** \brief Return 4x32-bit vector boolean true values for each float,
 *  if infinity.
 *
 *  A IEEE Binary32 infinity has a exponent of 0x7f8 and significand
 *  of all zeros.
 *
 *	@param vf32 a vector of __binary32 values.
 *	@return a vector boolean int, each containing all 0s or 1s.
 */
static inline __f32_bool
vec_isinff32 (vf32_t vf32)
{
	vui32_t tmp;
	const vui32_t expmask  = CONST_VINT128_W(0x7f800000, 0x7f800000, 0x7f800000, 0x7f800000);
	__f32_bool result;

#if _ARCH_PWR7
	/* Eliminate const load. */
	tmp = (vui32_t)vec_abs (vf32);
#else
	const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000, 0x80000000);
	tmp = vec_andc ((vui32_t)vf32, signmask);
#endif
	result = vec_cmpeq (tmp, expmask);

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
 *	@param vf32 a vector of __binary32 values.
 *	@return a vector boolean int, each containing all 0s or 1s..
 */
static inline __f32_bool
vec_isnormalf32 (vf32_t vf32)
{
	vui32_t tmp, tmp2;
	const vui32_t expmask  = CONST_VINT128_W(0x7f800000, 0x7f800000, 0x7f800000, 0x7f800000);
	const vui32_t vec_zero = CONST_VINT128_W(0, 0, 0, 0);
	__f32_bool result;

#if _ARCH_PWR7
	/* Eliminate const load. */
	tmp2 = (vui32_t)vec_abs (vf32);
#else
	const vui32_t signmask = CONST_VINT128_W(0x80000000, 0x80000000, 0x80000000, 0x80000000);
	tmp2 = vec_andc ((vui32_t)vf32, signmask);
#endif
	tmp = vec_and ((vui32_t)vf32, expmask);
	tmp2 = (vui32_t)vec_cmpeq(tmp2, vec_zero);
	tmp = (vui32_t)vec_cmpeq(tmp, expmask);
	result = (__f32_bool)vec_nor (tmp, tmp2);

	return (result);
}

#endif /* VEC_F32_PPC_H_ */
