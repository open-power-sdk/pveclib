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

 vec_f128_ppc.h

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Apr 11, 2016
 */

#ifndef VEC_F128_PPC_H_
#define VEC_F128_PPC_H_

#include <vec_common_ppc.h>

#ifdef __FLOAT128__
typedef __float128 __Float128;
typedef __float128 __binary128;
typedef __ibm128 __IBM128;
#else
/* __float128 was added in GCC 6.0.  */
typedef  __vector unsigned __int128 vfloat128_t;
typedef vfloat128_t __Float128;
typedef vfloat128_t __binary128;
typedef vfloat128_t __float128;
typedef long double __IBM128;
#endif
typedef __vector __bool int __f128_bool;
typedef __vector __bool __int128 __i128_bool;

#include "vec_int128_ppc.h"

typedef union
     {
       vui8_t    vx16;
       vui16_t   vx8;
       vui32_t   vx4;
       vui64_t   vx2;
       vui128_t  vx1;
       __f128_bool vbool1;
       __i128_bool vbool2;
       __binary128 vf1;
     } __VF_128;

static inline vui16_t
vec_xfer_bin128_2_vui16t (__binary128 f128)
{
	__VF_128 vunion;

	vunion.vf1 = f128;

	return (vunion.vx8);
}

static inline vui32_t
vec_xfer_bin128_2_vui32t (__binary128 f128)
{
	__VF_128 vunion;

	vunion.vf1 = f128;

	return (vunion.vx4);
}

static inline vui64_t
vec_xfer_bin128_2_vui64t (__binary128 f128)
{
	__VF_128 vunion;

	vunion.vf1 = f128;

	return (vunion.vx2);
}

static inline __binary128
vec_xfer_vui16t_2_bin128 (vui16_t f128)
{
	__VF_128 vunion;

	vunion.vx8 = f128;

	return (vunion.vf1);
}

static inline __binary128
vec_xfer_vui32t_2_bin128 (vui32_t f128)
{
	__VF_128 vunion;

	vunion.vx4 = f128;

	return (vunion.vf1);
}

static inline __binary128
vec_xfer_vui64t_2_bin128 (vui64_t f128)
{
	__VF_128 vunion;

	vunion.vx2 = f128;

	return (vunion.vf1);
}
#if 0
/** \brief Extract the sign bit from a __float128 value.
 *
 * Extract the sign bit (bit 0) the vector containing a __float128.
 *
 *	@param f128 a __float128 value in vector.
 *	@return an int containing 0 or 1.
 */
static inline int
__signbitf128 (__binary128 f128)
{
	vui64_t tmp;
	int result;

	tmp = vec_xfer_bin128_2_vui64t (f128);
#if _ARCH_PWR8
	/* where we have direct move copy immediately to a GPR.  */
	unsigned long tmp2;
	tmp2 = tmp[VEC_DW_H];

	/* then shift to extract the sign bit.  */
	result = tmp2 >> 63;
#else
	/* Otherwise perform a masking and compare in vector regs.  */
	const vui64_t signmask  = CONST_VINT128_DW(0x8000000000000000, 0);

	tmp = vec_and (tmp, signmask);

	result = vec_all_eq((vui32_t)tmp, (vui32_t)signmask);
#endif
	return (result);
}
#endif
/** \brief return a positive infinity.
 *
 *	@return a __float128 positive infinity.
 */
static inline __binary128
__huge_valf128 ()
{
	const vui32_t posinf  = CONST_VINT128_W(0x7fff0000, 0, 0, 0);
	__binary128 result;

	result = vec_xfer_vui32t_2_bin128 (posinf);

	return (result);
}

/** \brief return a quiet NaN.
 *
 *	@return a __float128 quiet NaN.
 */
static inline __binary128
__nanf128 ()
{
	const vui32_t posinf  = CONST_VINT128_W(0x7fffffff, 0, 0, 0);
	__binary128 result;

	result = vec_xfer_vui32t_2_bin128 (posinf);

	return (result);
}

/** \brief return a signaling NaN.
 *
 *	@return a __float128 signaling NaN.
 */
static inline __binary128
__nansf128 ()
{
	const vui32_t posinf  = CONST_VINT128_W(0x7fff7fff, 0, 0, 0);
	__binary128 result;

	result = vec_xfer_vui32t_2_bin128 (posinf);

	return (result);
}

/** \brief return a positive infinity.
 *
 *	@return a __float128 positive infinity.
 */
static inline __binary128
__inff128 ()
{
	const vui32_t posinf  = CONST_VINT128_W(0x7fff0000, 0, 0, 0);
	__binary128 result;

	result = vec_xfer_vui32t_2_bin128 (posinf);

	return (result);
}

/** \brief Clear the sign bit of __float128 input
 * and return the resulting positive __float128 value.
 *
 *	@param f128 a __float128 value containing a signed value.
 *	@return a __float128 value with magnitude from f128 and a positive
 *	sign of f128.
 */
static inline __binary128
__fabsf128 (__binary128 f128)
{
	vui32_t tmp;
	const vui32_t signmask  = CONST_VINT128_W(0x80000000, 0, 0, 0);
	__binary128 result;

#if _ARCH_PWR9
    __asm__(
        "xsabsqp %0,%1;\n"
        : "=v" (result)
        : "v" (f128)
        :);
#else
	tmp = vec_xfer_bin128_2_vui32t (f128);
	tmp = vec_andc (tmp, signmask);
	result = vec_xfer_vui32t_2_bin128 (tmp);
#endif
	return (result);
}

/** \brief Copy the sign bit from f128y merge with magnitude from f128x
 * and return the resulting __float128 value.
 *
 *	@param f128x a __float128 value containing the magnitude.
 *	@param f128y a __float128 value containing the sign bit.
 *	@return a __float128 value with magnitude from f128x and the
 *	sign of f128y.
 */
static inline __binary128
__copysignf128 (__binary128 f128x , __binary128 f128y)
{
	const vui32_t signmask  = CONST_VINT128_W(0x80000000, 0, 0, 0);
	__binary128 result;
	vui32_t tmpx, tmpy, tmp;

#if _ARCH_PWR9
    __asm__(
        "xscpsgnqp %0,%2,%1;\n"
        : "=v" (result)
        : "v" (f128x), "v" (f128y)
        :);
#else
	tmpx = vec_xfer_bin128_2_vui32t (f128x);
	tmpy = vec_xfer_bin128_2_vui32t (f128y);

	tmp = vec_sel (tmpx, tmpy, signmask);
	result = vec_xfer_vui32t_2_bin128 (tmp);
#endif
	return (result);
}
#if 0
/** \brief Return true if the __float128 value is infinity.
 *
 *  A IEEE Binary128 infinity has a exponent of 0x7fff and significand
 *  of all zeros.  Using the vec_all_eq compare conditional verifies
 *  both conditions and avoids a vector -> GPR transfer for platforms
 *  before PowerISA-2.07.
 *
 *	@param f128 a __float128 value in vector.
 *	@return an int containing 0 or 1.
 */
static inline int
__isinff128 (__binary128 f128)
{
	vui32_t tmp;
	const vui32_t signmask  = CONST_VINT128_W(0x80000000, 0, 0, 0);
	const vui32_t expmask  = CONST_VINT128_W(0x7fff0000, 0, 0, 0);
	int result = 0;

	tmp = vec_xfer_bin128_2_vui32t (f128);
	tmp = vec_andc (tmp, signmask);
	result = vec_all_eq (tmp, expmask);

	return (result);
}
#endif
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
 *	@param f128 a __float128 value in vector.
 *	@return an int containing 0 if not inifinity and +1/-1 otherwise.
 */
static inline int
__isinf_signf128 (__binary128 f128)
{
	vui32_t tmp, t128;
	const vui32_t signmask  = CONST_VINT128_W(0x80000000, 0, 0, 0);
	const vui32_t expmask  = CONST_VINT128_W(0x7fff0000, 0, 0, 0);
	int result = 0;

	t128 = vec_xfer_bin128_2_vui32t (f128);
	tmp  = vec_andc (t128, signmask);

	if (vec_all_eq(tmp, expmask))
	{
		if (vec_any_gt(t128, expmask))
			result = -1;
		else
			result = 1;
	}
	return (result);
}
#if 0
/** \brief Return true if the __float128 value is Not a Number (NaN).
 *
 *  A IEEE Binary128 NaN has a exponent of 0x7fff and nonzero
 *  significand.  Using the combined vec_all_eq / vec_any_gt compare
 *  conditional verify both conditions and avoids a vector -> GPR
 *  transfer for platforms before PowerISA-2.07.
 *  The sign bit is ignored.
 *
 *	@param f128 a __float128 value in vector.
 *	@return an int containing 0 or 1.
 */
static inline int
__isnanf128 (__binary128 f128)
{
	vui32_t tmp, tmp2, t128;
	const vui32_t signmask  = CONST_VINT128_W(0x80000000, 0, 0, 0);
	const vui32_t expmask  = CONST_VINT128_W(0x7fff0000, 0, 0, 0);
	int result;

#if _ARCH_PWR9
    __asm__(
        "xscmpuqp cr7,%1,%1;\n"
        "\tmfocrf  %0,1;\n"
        "\tclrlwi  %0,%0,31;\n"
        : "=r" (result)
        : "v" (f128)
        : "cr7");
#else
	t128 = vec_xfer_bin128_2_vui32t (f128);
	tmp  = vec_andc (t128, signmask);
	tmp2 = vec_and  (t128, expmask);
	result = (vec_all_eq (tmp2, expmask) && vec_any_gt (tmp, expmask));
#endif
	return (result);
}
#endif
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
 *	@param f128 a __float128 value in vector.
 *	@return an int containing 0 or 1.
 */
static inline int
__isfinitef128 (__binary128 f128)
{
	vui32_t tmp, t128;
	const vui32_t expmask  = CONST_VINT128_W(0x7fff0000, 0, 0, 0);
	int result = 0;

	t128 = vec_xfer_bin128_2_vui32t (f128);
	tmp = vec_and (t128, expmask);
	result = !vec_all_eq (tmp, expmask);
	//result = !(result);

	return (result);
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
 *	@param f128 a __float128 value in vector.
 *	@return an int containing 0 or 1.
 */
static inline int
__isnormalf128 (__binary128 f128)
{
	vui32_t tmp, t128;
	const vui32_t expmask  = CONST_VINT128_W(0x7fff0000, 0, 0, 0);
	const vui32_t vec_zero = CONST_VINT128_W(0, 0, 0, 0);
	int result = 0;

	t128 = vec_xfer_bin128_2_vui32t (f128);
	tmp = vec_and (t128, expmask);
	result = !(vec_all_eq (tmp, expmask) || vec_all_eq (tmp, vec_zero));

	return (result);
}

/** \brief Return128-bit vector boolean  true if the __float128 value
 *  is negative (sign bit is '1').
 *
 *  Anding with a signmask and then a vector compare with that mask
 *  generates the vector boolean of the sign bit.
 *
 *	@param f128 a __float128 value in vector.
 *	@return a vector boolean containing all 0s or 1s..
 */
static inline __f128_bool
vec_signbitf128 (__binary128 f128)
{
	vui32_t tmp, tmp2, t128;
	const vui32_t signmask  = CONST_VINT128_W(0x80000000, 0, 0, 0);
	__f128_bool result;

	t128 = vec_xfer_bin128_2_vui32t (f128);
	tmp = vec_and (t128, signmask);
	tmp2 = (vui32_t)vec_cmpeq (tmp, signmask);
	result = (__f128_bool)vec_splat(tmp2, VEC_W_H);

	return (result);
}

/** \brief Return128-bit vector boolean  true if the __float128 value
 *  is infinity.
 *
 *  A IEEE Binary128 infinity has a exponent of 0x7fff and significand
 *  of all zeros.
 *
 *	@param f128 a __float128 value in vector.
 *	@return a vector boolean containing all 0s or 1s..
 */
static inline __f128_bool
vec_isinff128 (__binary128 f128)
{
	vui32_t tmp, /*tmp2,*/ t128;
	const vui32_t signmask  = CONST_VINT128_W(0x80000000, 0, 0, 0);
	const vui32_t expmask   = CONST_VINT128_W(0x7fff0000, 0, 0, 0);
	__f128_bool result = (__f128_bool)CONST_VINT128_W(0, 0, 0, 0);

	t128 = vec_xfer_bin128_2_vui32t (f128);
	tmp = vec_andc (t128, signmask);
#if 0
	tmp2 = (vui32_t)vec_cmpeq (tmp, expmask);
	result = (__f128_bool)vec_splat(tmp2, VEC_W_H);
#else
	if (vec_all_eq(tmp, expmask))
	{
		result = vec_nor (result, result);
	}
#endif
	return (result);
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
 *	@param f128 a __float128 value in vector.
 *	@return a vector boolean containing all 0s or 1s.
 */
static inline __f128_bool
vec_isnanf128 (__binary128 f128)
{
	vui32_t tmp, tmp2, t128;
	const vui32_t signmask  = CONST_VINT128_W(0x80000000, 0, 0, 0);
	const vui32_t expmask  = CONST_VINT128_W(0x7fff0000, 0, 0, 0);
	__f128_bool result = (__f128_bool)CONST_VINT128_W(0, 0, 0, 0);

	t128 = vec_xfer_bin128_2_vui32t (f128);
	tmp  = vec_andc (t128, signmask);
	tmp2 = vec_and (t128, expmask);

	if (vec_all_eq(tmp2, expmask) && vec_any_gt(tmp, expmask))
	{
		result = vec_nor (result, result);
	}
	return (result);
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
 *	@param f128 a __float128 value in vector.
 *	@return a vector boolean containing all 0s or 1s.
 */
static inline __f128_bool
vec_isfinitef128 (__binary128 f128)
{
	const vui32_t expmask  = CONST_VINT128_W(0x7fff0000, 0, 0, 0);
	vui32_t tmp, t128;
	__f128_bool tmp2, tmp3;
	__f128_bool result;

	t128 = vec_xfer_bin128_2_vui32t (f128);
	tmp = vec_and (t128, expmask);
	tmp2 = vec_cmpeq(tmp, expmask);
	tmp3 = vec_splat(tmp2, VEC_W_H);
	result = (__f128_bool)vec_nor (tmp3, tmp3); // vec_not

	return (result);
}

/** \brief Return 128-bit vector boolean true if the __float128 value
 *  is normal (Not NaN, Inf, denormal, or zero).
 *
 *  A IEEE Binary128 normal value has an exponent between 0x0001 and
 *  0x7ffe (a 0x7fff indicates NaN or Inf).  The significand can be
 *  any value (expect 0 if the exponent is zero).
 *  The sign bit is ignored.
 *
 *	@param f128 a __float128 value in vector.
 *	@return a vector boolean containing all 0s or 1s.
 */
static inline __f128_bool
vec_isnormalf128 (__binary128 f128)
{
	vui32_t tmp, t128;
	const vui32_t expmask  = CONST_VINT128_W(0x7fff0000, 0, 0, 0);
	const vui32_t vec_zero = CONST_VINT128_W(0, 0, 0, 0);
	__f128_bool result;

	t128 = vec_xfer_bin128_2_vui32t (f128);
	tmp = vec_and (t128, expmask);
	result = vec_nor (vec_cmpeq(tmp, expmask), vec_cmpeq(tmp, vec_zero));
	result = (__f128_bool)vec_splat((vui32_t)result, VEC_W_H);

	return (result);
}

#endif /* VEC_F128_PPC_H_ */
