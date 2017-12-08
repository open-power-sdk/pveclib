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

 vec_f64_ppc.h

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Apr 12, 2016
 */

#ifndef VEC_F64_PPC_H_
#define VEC_F64_PPC_H_

#include <vec_common_ppc.h>

typedef __vector double __vf64;
typedef __vector double __vbinary64;

typedef __vector __bool long __f64_bool;

/** \brief Copy the pair of doubles from a IBM long double to a vector
 * double.
 *
 *	@param lval IBM long double as FPR pair.
 *	@return vector double values containing the IBM long double.
 */
static inline __vf64
vec_unpack_longdouble ( long double lval)
{
#ifdef _ARCH_PWR7
	__vf64     t;
        __asm__(
        	"xxmrghd %x0,%1,%L1;\n"
            : "=wa" (t)
            : "f" (lval)
            : );
        return (t);
#else
        U_128   t;
        t.ldbl128 = lval;
        return (t.vf2);
#endif
}

/** \brief Copy the pair of doubles from a vector to IBM long double.
 *
 *	@param lval vector double values containing the IBM long double.
 *	@return IBM long double as FPR pair.
 */
static inline long double
vec_pack_longdouble ( __vf64 lval)
{
#ifdef _ARCH_PWR7
	long double     t;
        __asm__(
        	"xxlor %0,%x1,%x1;\n"
        	"\txxswapd %L0,%x1;\n"
            : "=f" (t)
            : "wa" (lval)
            : );
        return (t);
#else
        U_128   t;
        t.vf2 = lval;
        return (t.ldbl128);
#endif
}

/** \brief Copy the sign bit from vf64y merged with magnitude from
 * vf64x and return the resulting vector double values.
 *
 *	@param vf64x vector double values containing the magnitudes.
 *	@param vf64y vector double values containing the sign bits.
 *	@return vector double values with magnitude from vf64x and the
 *	sign of vf64y.
 */
static inline __vf64
vec_copysignf64 (__vf64 vf64x , __vf64 vf64y)
{
#if _ARCH_PWR7
	return (vec_cpsgn (vf64x, vf64y));
#else
	const vui32_t signmask  = CONST_VINT128_W(0x80000000, 0, 0x80000000, 0);
	__vf64 result;

	result  = (__vf64)vec_sel ((vui32_t)vf64x, (vui32_t)vf64y, signmask);
	return (result);
#endif
}

/** \brief Return 2x64-bit vector boolean true values for each double,
 *  if infinity.
 *
 *  A IEEE Binary64 infinity has a exponent of 0x7ff and significand
 *  of all zeros.
 *
 *	@param vf64 a vector of __binary64 values.
 *	@return a vector boolean long, each containing all 0s or 1s.
 */
static inline __f64_bool
vec_isinff64 (__vf64 vf64)
{
	vui64_t tmp;
	const vui64_t expmask  = CONST_VINT128_DW(0x7ff0000000000000, 0x7ff0000000000000);
	__f64_bool result;

#if _ARCH_PWR7
	/* Eliminate const load. */
	tmp = (vui64_t)vec_abs (vf64);
#else
	const vui64_t signmask = CONST_VINT128_DW(0x8000000000000000, 0x8000000000000000);
	tmp = vec_andc ((vui64_t)vf64, signmask);
#endif
	result = (__f64_bool)vec_cmpeq (tmp, expmask);

	return (result);
}

/** \brief Return 2x64-bit vector boolean true values, for each double
 *  value, if normal (Not NaN, Inf, denormal, or zero).
 *
 *  A IEEE Binary64 normal value has an exponent between 0x001 and
 *  0x7ffe (a 0x7ff indicates NaN or Inf).  The significand can be
 *  any value (expect 0 if the exponent is zero).
 *  The sign bit is ignored.
 *
 *	@param vf64 a vector of __binary64 values.
 *	@return a vector boolean long, each containing all 0s or 1s.
 */
static inline __f64_bool
vec_isnormalf64 (__vf64 vf64)
{
	vui64_t tmp, tmp2;
	const vui64_t expmask  = CONST_VINT128_DW(0x7ff0000000000000, 0x7ff0000000000000);
	const vui64_t vec_zero = CONST_VINT128_DW(0, 0);
	__f64_bool result;

#if _ARCH_PWR7
	/* Eliminate const load. */
	tmp2 = (vui64_t)vec_abs (vf64);
#else
	const vui64_t signmask = CONST_VINT128_DW(0x8000000000000000, 0x8000000000000000);
	tmp2 = vec_andc ((vui64_t)vf64, signmask);
#endif
	tmp = vec_and ((vui64_t)vf64, expmask);
	tmp2 = (vui64_t)vec_cmpeq(tmp2, vec_zero);
	tmp = (vui64_t)vec_cmpeq(tmp, expmask);
	result = (__f64_bool)vec_nor (tmp, tmp2);

	return (result);
}

#endif /* VEC_F64_PPC_H_ */
