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

 vec_char_ppc.h

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Jul 2, 2015
 */

#ifndef VEC_CHAR_PPC_H_
#define VEC_CHAR_PPC_H_

#include <vec_common_ppc.h>

/** \brief Shift left double quadword by octet.
 * Return a vector unsigned char that is the left most 16 chars after
 * shifting left 0-15 octets (chars) of the 32 char double vector
 * (vrw||vrx).  The octet shift amount is from bits 121:124 of vrb.
 *
 * This sequence can be used to align a unaligned 16 char substring
 * based on the result of a vector count leading zero of of the compare
 * boolean.
 *
 *	@param vrw upper 16-bytes of the 32-byte double vector.
 *	@param vrx lower 16-bytes of the 32-byte double vector.
 *	@param vrb Shift amount in bits 121:124.
 *	@return upper 16-bytes of left shifted double vector.
 */
static inline vui8_t
vec_shift_leftdo (vui8_t vrw, vui8_t vrx, vui8_t vrb)
{
	vui8_t result, vt1, vt2, vt3;
	const vui8_t vzero = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	vt1 = vec_slo (vrw, vrb);
	vt3 = vec_sub (vzero, vrb);
	vt2 = vec_sro (vrx, vt3);
	result = vec_or (vt1, vt2);

	return (result);
}

/** \brief Vector isalnum.
 *
 * Return a vector boolean char with a true indicator for any character
 * that is either Lower Case Alpha ASCII, Upper Case ASCII, or numeric ASCII.
 * False otherwise.
 *
 *	@param vec_str vector of 16 ASCII characters
 *	@return vector bool char of the isalnum operation applied to each
 *	character of vec_str. For each byte 0xff indicates true (isalpha),
 *	0x00 indicates false.
 */
static inline vui8_t
vec_isalpha (vui8_t vec_str)
{
	vui8_t result;
	const vui8_t UC_FIRST = {0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40};
	const vui8_t UC_LAST  = {0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a};
	const vui8_t LC_FIRST = {0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60};
	const vui8_t LC_LAST  = {0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a};

	vui8_t cmp1, cmp2, cmp3, cmp4, cmask1, cmask2;

	cmp1 = (vui8_t)vec_cmpgt (vec_str, LC_FIRST);
	cmp2 = (vui8_t)vec_cmpgt (vec_str, LC_LAST);

	cmp3 = (vui8_t)vec_cmpgt (vec_str, UC_FIRST);
	cmp4 = (vui8_t)vec_cmpgt (vec_str, UC_LAST);

	cmask1 = vec_andc (cmp1, cmp2);
	cmask2 = vec_andc (cmp3, cmp4);

	result = vec_or (cmask1, cmask2);

	return (result);
}

/** \brief Vector isalpha.
 *
 * Return a vector boolean char with a true indicator for any character
 * that is either Lower Case Alpha ASCII or Upper Case ASCII.
 * False otherwise.
 *
 *	@param vec_str vector of 16 ASCII characters
 *	@return vector bool char of the isalpha operation applied to each
 *	character of vec_str. For each byte 0xff indicates true (isalpha),
 *	0x00 indicates false.
 */
static inline vui8_t
vec_isalnum (vui8_t vec_str)
{
	vui8_t result;
	const vui8_t UC_FIRST = {0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40};
	const vui8_t UC_LAST  = {0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a};
	const vui8_t LC_FIRST = {0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60};
	const vui8_t LC_LAST  = {0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a};
	const vui8_t DG_FIRST = {0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f};
	const vui8_t DG_LAST  = {0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x5a, 0x39, 0x39, 0x39, 0x39};

	vui8_t cmp1, cmp2, cmp3, cmp4, cmp5, cmp6, cmask1, cmask2, cmask3;

	cmp1 = (vui8_t)vec_cmpgt (vec_str, LC_FIRST);
	cmp2 = (vui8_t)vec_cmpgt (vec_str, LC_LAST);

	cmp3 = (vui8_t)vec_cmpgt (vec_str, UC_FIRST);
	cmp4 = (vui8_t)vec_cmpgt (vec_str, UC_LAST);

	cmp5 = (vui8_t)vec_cmpgt (vec_str, DG_FIRST);
	cmp6 = (vui8_t)vec_cmpgt (vec_str, DG_LAST);

	cmask1 = vec_andc (cmp1, cmp2);
	cmask2 = vec_andc (cmp3, cmp4);
	cmask3 = vec_andc (cmp5, cmp6);

	result = vec_or (vec_or (cmask1, cmask2), cmask3);

	return (result);
}

/** \brief Vector isdigit.
 *
 * Return a vector boolean char with a true indicator for any character
 * that is ASCII decimal digit.
 * False otherwise.
 *
 *	@param vec_str vector of 16 ASCII characters
 *	@return vector bool char of the isdigit operation applied to each
 *	character of vec_str. For each byte 0xff indicates true (isdigit),
 *	0x00 indicates false.
 */
static inline vui8_t
vec_isdigit (vui8_t vec_str)
{
	vui8_t result;
	const vui8_t DG_FIRST = {0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f};
	const vui8_t DG_LAST  = {0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x5a, 0x39, 0x39, 0x39, 0x39};

	vui8_t cmp1, cmp2;

	cmp1 = (vui8_t)vec_cmpgt (vec_str, DG_FIRST);
	cmp2 = (vui8_t)vec_cmpgt (vec_str, DG_LAST);

	result = vec_andc (cmp1, cmp2);

	return (result);
}

/** \brief Vector toupper.
 *
 * Convert any Lower Case Alpha ASCII characters within a vector
 * unsigned char into the equivalent Upper Case character.
 * Return the result as a vector unsigned char.
 *
 *	@param vec_str vector of 16 ASCII characters
 *	@return vector char converted to upper case.
 */
static inline vui8_t
vec_toupper (vui8_t vec_str)
{
	vui8_t result;
	const vui8_t UC_MASK  = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20};
	const vui8_t LC_FIRST = {0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60};
	const vui8_t LC_LAST  = {0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a};

	vui8_t cmp1, cmp2, cmask;

	cmp1 = (vui8_t)vec_cmpgt (vec_str, LC_FIRST);
	cmp2 = (vui8_t)vec_cmpgt (vec_str, LC_LAST);

	cmask = vec_andc (cmp1, cmp2);
	cmask = vec_and (cmask, UC_MASK);

	result = vec_andc (vec_str, cmask);

	return (result);
}

/** \brief Vector tolower.
 *
 * Convert any Upper Case Alpha ASCII characters within a vector
 * unsigned char into the equivalent Lower Case character.
 * Return the result as a vector unsigned char.
 *
 *	@param vec_str vector of 16 ASCII characters
 *	@return vector char converted to lower case.
 */
static inline vui8_t
vec_tolower (vui8_t vec_str)
{
	vui8_t result;
	const vui8_t UC_MASK  = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20};
	const vui8_t UC_FIRST = {0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40};
	const vui8_t UC_LAST  = {0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a};

	vui8_t cmp1, cmp2, cmask;

	cmp1 = (vui8_t)vec_cmpgt (vec_str, UC_FIRST);
	cmp2 = (vui8_t)vec_cmpgt (vec_str, UC_LAST);

         	cmask = vec_andc (cmp1, cmp2);
	cmask = vec_and (cmask, UC_MASK);

	result = vec_or (vec_str, cmask);

	return (result);
}

/** \brief Vector Absolute Difference Unsigned byte.
 *
 * Compute the absolute difference for each byte.
 * For each unsigned byte, subtract B[i] from A[i] and return the
 * absolute value of the difference.
 *
 * @param __A vector of 16 unsigned bytes
 * @param __B vector of 16 unsigned bytes
 * @return vector of the absolute difference.
 */
static inline vui8_t
vec_absdub (vui8_t __A, vui8_t __B)
{
  vui8_t result;
#ifdef _ARCH_PWR9
        __asm__(
            "vabsdub %0,%1,%2;"
            : "=v" (result)
            : "v" (__A), "v" (__B)
            : );
#else
  vui8_t a, b;
  vui8_t vmin, vmax;

  a = (vui8_t) __A;
  b = (vui8_t) __B;
  vmin = vec_min (a, b);
  vmax = vec_max (a, b);
  result = vec_sub (vmax, vmin);
#endif
  return (result);
}

#endif /* VEC_CHAR_PPC_H_ */
