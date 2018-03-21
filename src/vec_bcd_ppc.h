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

 vec_bcd_ppc.h

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Aug 12, 2015
 */

#ifndef VEC_BCD_PPC_H_
#define VEC_BCD_PPC_H_

#include <vec_common_ppc.h>

/*!
 * \file  vec_bcd_ppc.h
 * \brief Header package containing a collection of Binary Coded
 * Decimal (<B>BCD</B> computation functions implemented
 * with PowerISA VMX, VSX, and DFP instructions.
 */

/** \brief Quantize (truncate) a _Decimal128 value before convert to
 * BCD.
 *
 * The truncate (round toward 0) and justify right the input
 * _Decimal128 value so that the unit digit is in the right most
 * position.  This supports BCD multiply and divide using DFP
 * instructions by truncating fractional digits before conversion
 * back to BCD.
 *
 * @param val a _Decimal128 value.
 * @return The quantized __Decimal128 value in a double float pair.
 */
static inline _Decimal128
bcd_qauntize0 (_Decimal128 val)
{
#ifdef _ARCH_PWR7
        _Decimal128 t;
        __asm__(
            "dquaiq 0,%0,%1,0b01;\n"
            : "=d" (t)
            : "d" (val)
            : );
        return (t);
#else
        return (quantized128(t, 0DL));
#endif
}

/** \brief Convert a Vector Signed BCD value to __Decimal128
 *
 * The BCD vector is permuted into a double float pair before
 * conversion to DPD format via the DFP Encode BCD To DPD Quad.
 *
 * @param val a 128-bit vector treated a signed BCD 31 digit value.
 * @return a __Decimal128 in a double float pair.
 */
static inline _Decimal128
vec_BCD2DFP (vui32_t val)
{
#ifdef _ARCH_PWR7
        _Decimal128 t;
        __asm__(
            "xxpermdi %0,%x1,%x1,0b00;\n"
            "\txxpermdi %L0,%x1,%x1,0b10;\n"
            "\tdenbcdq 1,%0,%0;\n"
            : "=&d" (t)
            : "v" (val)
            : );
        return (t);
#else
        /* \todo needs work for P6 without xxpermdi */
        __VEC_U_128   t, x;
        t.vx4 = val;
        return (t.dpd128);
#endif
}

/** \brief Convert a __Decimal128 value to Vector BCD
 *
 * The _Decimal128 value is converted to a signed BCD 31 digit value
 * via "DFP Decode DPD To BCD Quad".  The conversion result is still
 * in a double float register pair and so is permuted into single
 * vector register for use.
 *
 * @param val a __Decimal128 in a double float pair.
 * @return a 128-bit vector treated a signed BCD 31 digit value.
 */
static inline vui32_t
vec_DFP2BCD (_Decimal128 val)
{
#ifdef _ARCH_PWR7
        vui32_t  t;
        _Decimal128 x;
        __asm__(
            "ddedpdq 2,%1,%2;\n"
            "\txxpermdi %x0,%1,%L1,0b00;\n"
            : "=v" (t),
              "=&d" (x)
            : "d" (val)
            : );
        return (t);
#else
        /* \todo needs work for P6 without xxpermdi */
        __VEC_U_128   t, x;
        t.dpd128 = val;
        return (t.vx4);
#endif
}

/* \todo To be useful for number conversion will need to implement
 * carry and extend forms of bcdadd/sub. This will require conditional
 * logic based on CR6.
 *
 */

/** \brief Add two Vector Signed BCD 31 digit values.
 *
 * Two Signed 31 digit values are added and lower 31 digits of the
 * sum are returned.  Overflow (carry-out) is ignored.
 *
 * @param a a 128-bit vector treated a signed BCD 31 digit value.
 * @param b a 128-bit vector treated a signed BCD 31 digit value.
 * @return a 128-bit vector which is the lower 31 digits of (a + b).
 */
static inline vui32_t
vec_bcdadd (vui32_t a, vui32_t b)
{
        vui32_t  t;
#ifdef _ARCH_PWR8
        __asm__(
            "bcdadd. %0,%1,%2,0;\n"
            : "=v" (t)
            : "v" (a),
              "v" (b)
            : "cr6" );
#else
#if 0
        _Decimal128 d_t, d_a, d_b;
        d_a = vec_BCD2DFP (a);
        d_b = vec_BCD2DFP (b);
        d_t = d_a + d_b;
#else
        _Decimal128 d_t;
        d_t = vec_BCD2DFP (a) + vec_BCD2DFP (b);
#endif
        t = vec_DFP2BCD(d_t);
#endif
        return (t);
}

/** \brief Subtract two Vector Signed BCD 31 digit values.
 *
 * Subtract Signed 31 digit values and return the lower 31 digits of
 * of the result.  Overflow (carry-out/barrow) is ignored.
 *
 * @param a a 128-bit vector treated a signed BCD 31 digit value.
 * @param b a 128-bit vector treated a signed BCD 31 digit value.
 * @return a 128-bit vector which is the lower 31 digits of (a - b).
 */
static inline vui32_t
vec_bcdsub (vui32_t a, vui32_t b)
{
        vui32_t  t;
#ifdef _ARCH_PWR8
        __asm__(
            "bcdsub. %0,%1,%2,0;\n"
            : "=v" (t)
            : "v" (a),
              "v" (b)
            : "cr6" );
#else
        _Decimal128 d_t, d_a, d_b;
        d_a = vec_BCD2DFP (a);
        d_b = vec_BCD2DFP (b);
        d_t = d_a - d_b;
        t = vec_DFP2BCD(d_t);
#endif
        return (t);
}

/** \brief Multiply two Vector Signed BCD 31 digit values.
 *
 * Two Signed 31 digit values are multiplied and lower 31 digits of the
 * product are returned.  Overflow is ignored.
 *
 * \todo This is tricky as the product can be up to 62 digits and
 * _Decimal128 can only hold 34 digits. So products that are more
 * than 34 digits will loose precision on the right during the
 * conversion back to BCD.
 *
 * @param a a 128-bit vector treated a signed BCD 31 digit value.
 * @param b a 128-bit vector treated a signed BCD 31 digit value.
 * @return a 128-bit vector which is the lower 31 digits of (a * b).
 */
static inline vui32_t
vec_bcdmul (vui32_t a, vui32_t b)
{
        vui32_t  t;
        _Decimal128 d_t, d_a, d_b;
        d_a = vec_BCD2DFP (a);
        d_b = vec_BCD2DFP (b);
        d_t = bcd_qauntize0(d_a * d_b);
        t = vec_DFP2BCD(d_t);
        return (t);
}

/** \brief Divide a Vector Signed BCD 31 digit value by another BCD value.
 *
 * One Signed 31 digit value is divided by a second 31 digit value
 * and the quotient is returned.
 *
 * @param a a 128-bit vector treated a signed BCD 31 digit value.
 * @param b a 128-bit vector treated a signed BCD 31 digit value.
 * @return a 128-bit vector which is the lower 31 digits of (a / b).
 */
static inline vui32_t
vec_bcddiv (vui32_t a, vui32_t b)
{
        vui32_t  t;
        _Decimal128 d_t, d_a, d_b;
        d_a = vec_BCD2DFP (a);
        d_b = vec_BCD2DFP (b);
        d_t = bcd_qauntize0(d_a / d_b);
        t = vec_DFP2BCD(d_t);
        return (t);
}

#endif /* VEC_BCD_PPC_H_ */
