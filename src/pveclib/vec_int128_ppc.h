/*
 Copyright (c) [2017, 2018] IBM Corporation.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 vec_int128_ppc.h

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: May 10, 2015
      Steven Munroe, additional contributions for POWER9.
 */

#ifndef VEC_INT128_PPC_H_
#define VEC_INT128_PPC_H_

#include <pveclib/vec_common_ppc.h>
#include <pveclib/vec_int64_ppc.h>

/*!
 * \file  vec_int128_ppc.h
 * \brief Header package containing a collection of 128-bit computation
 * functions implemented with PowerISA VMX and VSX instructions.
 *
 * Some of these operations are implemented in a single instruction
 * on newer (POWER8/POWER9) processors.
 * This header serves to fill in functional gaps for older
 * (POWER7, POWER8) processors and provides a in-line assembler
 * implementation for older compilers that do not
 * provide the build-ins.
 * Other operations do not exist as instructions on any current
 * processor but are useful and should be provided.
 * This header serves to provide these operations as
 * inline functions using existing vector built-ins or other
 * pveclib operations.
 *
 * The original VMX (AKA Altivec) only defined a few instructions that
 * operated on the 128-bit vector as a whole.
 * This included the vector shift left/right (bit), vector shift
 * left/right by octet (byte), vector shift left double  by octet
 * (select a contiguous 16-bytes from 2 concatenated vectors)
 * 256-bit), and generalized vector permute (select any 16-bytes from
 * 2 concatenated vectors).
 * Use of these instructions can be complicated when;
 * - the shift amount is more than 8 bits,
 * - the shift amount is not a multiple of 8-bits (octet),
 * - the shift amount is a constant and needs to be generated/loaded
 * before use.
 *
 * These instructions can used in combination to provide generalized
 * vector __int128 shift/rotate operations.
 * Pveclib uses these operations to provide vector __int128 shift /
 * rotate left, shift right and shift algebraic right operations.
 * These operations require pre-conditions to avoid multiple
 * instructions or require a combination of (bit and octet shift)
 * instructions to get the quadword result.  The compiler <altivec.h>
 * built-ins only supports individual instructions. So using these
 * operations quickly inspires a need for a header (like this) to
 * contain implementations of the common operations.
 *
 * The VSX facility (introduced with POWER7) did not add any integer
 * doubleword (64-bit) or quadword (128-bit) operations.  However
 * it did add a useful doubleword permute immediate and word wise;
 * merge, shift, and splat immediate operations.
 * Otherwise vector __int128 (128-bit elements) operations
 * have to be implemented using VMX word and halfword element integer
 * operations for POWER7.
 *
 * POWER8
 * added multiply word operations that produce the full doubleword
 * product and full quadword add / subtract (with carry extend).
 * The add quadword is useful to sum the partial products for a full
 * 128 x 128-bit multiply.
 * The add quadword write carry and extend forms, simplify extending
 * arithmetic to 256-bits and beyond.
 *
 * While POWER8 provided quadword integer add and subtract operations,
 * it did not provide quadword Signed/Unsigned integer compare
 * operations. It is possible to implement quadword compare operations
 * using existing word / doubleword compares and the the new quadword
 * subtract write-carry operation. The trick it so convert the carry
 * into a vector bool __int128 via the vec_setb_ncq () operation.
 * This header provides easy to use quadword compare operations.
 *
 * POWER9 (PowerISA 3.0B) adds the <B>Vector Multiply-Sum unsigned
 * Doubleword Modulo</B> instruction. Aspects of this instruction mean
 * it needs to be used carefully as part of larger quadword multiply.
 * It performs only two of the four required doubleword multiplies.
 * The final quadword modulo sum will discard any overflow/carry from
 * the potential 130-bit result. With careful pre-conditioning of
 * doubleword inputs the results are can not overflow from 128-bits.
 * Then separate add quadword add/write carry operations can be used to
 * complete the sum of partial products.
 * These techniques are used in the POWER9 specific implementations of
 * vec_muleud, vec_muloud, vec_mulluq, and vec_muludq.
 *
 * PowerISA 3.0B also defined additional:
 * Binary Coded Decimal (BCD) and Zoned character format conversions.
 * String processing operations.
 * Vector Parity operations.
 * Integer Extend Sign Operations.
 * Integer Absolute Difference Operations.
 * All of these seem to useful additions to pveclib for older
 * (POWER7/8) processors and across element sizes (including
 * quadword elements).
 *
 * Most of these intrinsic (compiler built-in) operations are defined
 * in <altivec.h> and described in the compiler documentation.
 * However it took several compiler releases for all the new POWER8
 * 64-bit and 128-bit integer vector intrinsics to be added to
 * <B>altivec.h</B>. This support started with the GCC 4.9 but was not
 * complete across function/type and bug free until GCC 6.0.
 *
 * \note The compiler disables associated <altivec.h> built-ins if the
 * <B>mcpu</B> target does not enable the specific instruction.
 * For example, if you compile with <B>-mcpu=power7</B>, vec_vadduqm and
 * vec_vsubudm will not be defined.  But vec_adduqm() and vec_subudm()
 * and always be defined in this header, will generate the minimum code,
 * appropriate for the target, and produce correct results.
 *
 * Most of these
 * operations are implemented in a single instruction on newer
 * (POWER8/POWER9) processors. So this header serves to fill in
 * functional gaps for older (POWER7, POWER8) processors and provides
 * a in-line assembler implementation for older compilers that do not
 * provide the build-ins.
 *
 * This header covers operations that are either:
 *
 * - Operations implemented in hardware instructions for later
 * processors and useful to programmers, on slightly older processors,
 * even if the equivalent function requires more instructions.
 * Examples include quadword byte reverse, add and subtract.
 * - Defined in the OpenPOWER ABI but <I>not</I> yet defined in
 * <altivec.n> provided by available compilers in common use.
 * Examples include quadword byte reverse, add and subtract.
 * - Are commonly used operations, not covered by the ABI or
 * <altivec.h>, and require multiple instructions or
 * are not obvious.  Examples include quadword; Signed and Unsigned
 * compare, shift immediate, multiply, multiply by 10 immediate,
 * count leading zeros and population count.
 *
 * \note The Multiply sum/even/odd doubleword operations are
 * currently implemented here (in <vec_int128_ppc.h>) which resolves a
 * dependency on Add Quadword. These functions (vec_msumudm,
 * vec_muleud, vec_muloud) all produce a quadword results and may use
 * the vec_adduqm implementation to sum partial products.
 *
 * See \ref mainpage_sub_1_3 for more background on extended quadword
 * computation.
 *
 * \section i128_endian_issues_0_0 Endian problems with quadword implementations
 *
 * Technically operations on quadword elements should not require any
 * endian specific transformation. There is only one element so there
 * can be no confusion about element numbering or order. However
 * some of the more complex quadword operations are constructed from
 * operations on smaller elements. And those operations as provided by
 * <altivec.h> are required by the OpenPOWER ABI to be endian sensitive.
 * See \ref i64_endian_issues_0_0 for a more detailed discussion.
 *
 * In any case the arithmetic (high to low) order of bits in a quadword
 * are defined in the PowerISA (See vec_adduqm() and vec_subuqm()).
 * So pveclib implementations will need to either:
 * - Nullify little endian transforms of <altivec.h> operations.
 * The <altivec.h> built-ins vec_muleuw(), vec_mulouw(), vec_mergel(),
 * and vec_mergeh() are endian sensitive and often require
 * nullification that restores the original operation.
 * - Use new operations that are specifically defined to be stable
 * across BE/LE implementations. The pveclib operations; vec_vmuleud()
 * vec_vmuloud(), vec_mrgahd(), vec_mrgald(). and vec_permdi() are
 * defined to be endian stable.
 *
 * \section int128_examples_0_1 Vector Quadword Examples
 *
 * The PowerISA Vector facilities provide logical and integer
 * arithmetic quadword (128-bit) operations.  Some operations as
 * direct PowerISA instructions and other operations composed of
 * short instruction sequences. The Power Vector Library provides
 * a higher level and comprehensive API of quadword integer integer
 * arithmetic and support for extended arithmetic to multiple
 * quadwords.
 *
 * \subsection int128_examples_0_1_1 Printing Vector __int128 values
 *
 * The GCC compiler supports the (vector) __int128 type but the
 * runtime does not support <B>printf()</B> formating for __int128
 * types.  However if we can use divide/modulo operations to split
 * vector __int128 values into modulo 10^16 long int (doubleword)
 * chunks, we can use printf() to convert and concatenate the decimal
 * values into a complete number.
 *
 * For example, from the __int128 value (39 decimal digits):
 * - Detect the sign and set a char to "+' or '-'
 * - Then from the absolute value, divide/modulo by 10000000000000000. Producing:
 *   - The highest 7 digits (t_high)
 *   - The middle 16 digits (t_mid)
 *   - The lowest 16 digits (t_low)
 *
 * We can use signed compare to detect the sign and set a char value to print a ' ' or '+' prefix.
 * If the value is negative we want the absolute value before we do the divide/modulo steps.
 * For example:
 * \code
 *
  if (vec_cmpsq_all_ge (value, zero128))
    {
      sign = ' ';
      val128 = (vui128_t) value;
    }
  else
    {
      sign = '-';
      val128 = vec_subuqm ((vui128_t) zero128, (vui128_t) value);
    }
 * \endcode
 * Here we use the <B>pveclib</B> operation vec_cmpsq_all_ge() because
 * the ABI and compilers do not define compare built-ins operations for
 * the vector __int128 type.
 * For the negative case we use the <B>pveclib</B> operation
 * vec_subuqm() instead of vec_abs. Again the ABI and compilers do not
 * define vec_abs built-ins for the vector __int128 type.
 * Using <B>pveclib</B> operations have the additional benefit of
 * supporting older compilers and platform specific implementations
 * for POWER7 and POWER8.
 *
 * Now we have the absolute value in val128 we can factor it into (3)
 * chunks of 16 digits each.  Normally scalar codes would use
 * integer divide/modulo by 10000000000000000.  And we are reminded
 * that the PowerISA vector unit does not support integer divide
 * operations and definitely not for quadword integers.
 *
 * Instead we can use the multiplicative inverse which is a
 * scaled fixed point fraction calculated from the original divisor.
 * This works nicely if the fixed radix point is just before
 * the 128-bit fraction and we have a multiply high (vec_mulhuq())
 * operation. Multiplying a 128-bit unsigned integer by a 128-bit
 * unsigned fraction generates a 256-bit product with 128-bits above
 * (integer) and below (fraction) the radix point.
 * The high 128-bits of the product is the integer quotient and we can
 * discard the low order 128-bits.
 *
 * It turns out that generating the multiplicative inverse can be
 * tricky.  To produce correct results over the full range requires,
 * possible pre-scaling and post-shifting, and sometimes a corrective
 * addition is necessary. Fortunately the mathematics are well
 * understood and are commonly used in optimizing compilers.
 * Even better, Henry Warren's book has a whole chapter on this topic.
 * \see "Hacker's Delight, 2nd Edition,"
 * Henry S. Warren, Jr, Addison Wesley, 2013.
 * Chapter 10, Integer Division by Constants.
 *
 * In the chapter above;
 * <BLOCKQUOTE>Figure 10-2 Computing the magic number for unsigned division.</BLOCKQUOTE>
 * provides a sample C function for generating the magic number
 * (actually a struct containing; the magic multiplicative inverse,
 * "add" indicator, and the shift amount.).
 * For quadword and the divisor 10000000000000000,this is
 * { 76624777043294442917917351357515459181, 0 , 51 }:
 * - the multiplier is 76624777043294442917917351357515459181.
 * - no corrective add is required.
 * - the final shift is 51-bits right.
 *
 * \code
  const vui128_t mul_ten16 = (vui128_t) CONST_VINT128_DW(
      0UL, 10000000000000000UL);
  // Magic numbers for multiplicative inverse to divide by 10**16
  // are 76624777043294442917917351357515459181, no corrective add,
  // and shift right 51 bits.
  const vui128_t mul_invs_ten16 = (vui128_t) CONST_VINT128_DW(
      0x39a5652fb1137856UL, 0xd30baf9a1e626a6dUL);
  const int shift_ten16 = 51;
  ...

  // first divide/modulo the 39 digits __int128 by 10**16.
  // This separates the high/middle 23 digits (tmpq) and low 16 digits.
  tmpq = vec_mulhuq (val128, mul_invs_ten16);
  tmpq = vec_srqi (tmpq, shift_ten16);
  // Compute remainder of val128 / 10**16
  // t_low = val128 - (tmpq * 10**16)
  // Here we know tmpq and mul_ten16 are less then 64-bits
  // so can use vec_vmuloud instead of vec_mulluq
  tmp = vec_vmuloud ((vui64_t) tmpq, (vui64_t) mul_ten16);
  t_low = (vui64_t) vec_subuqm (val128, tmp);

  // Next divide/modulo the high/middle digits by 10**16.
  // This separates the high 7 and middle 16 digits.
  val128 = tmpq;
  tmpq = vec_mulhuq (tmpq, mul_invs_ten16);
  t_high = (vui64_t) vec_srqi (tmpq, shift_ten16);
  tmp = vec_vmuloud (t_high, (vui64_t) mul_ten16);
  t_mid = (vui64_t) vec_subuqm (val128, tmp);
 * \endcode
 * All the operations used above are defined and implemented by
 * <B>pveclib</B>.  Most of these operations is not defined as
 * single instructions in the PowerISA or as built-ins the ABI or
 * require alternative implementations for older processors.
 *
 * Now we have three vector unsigned __int128 values (t_low, t_mid,
 * t_high) in the range 0-9999999999999999. Fixed point values in that
 * range fit into the low order doubleword of each quadword.
 * We can access these doublewords with array notation ([VEC_DW_L])
 * and the compiler will transfer them to fixed point (long int) GPRs.
 * Then use normal char and long int printf() formating. For example:
 * \code
   printf ("%c%07lld%016lld%016lld", sign,
           t_high[VEC_DW_L], t_mid[VEC_DW_L], t_low[VEC_DW_L]);
 * \endcode
 *
 * Here is the complete vector __int128 printf example:
 * \code
 void
example_print_vint128 (vi128_t value)
{
  const vi128_t max_neg = (vi128_t) CONST_VINT128_DW(
      0x8000000000000000L, 0UL);
  const vi128_t zero128 = (vi128_t) CONST_VINT128_DW(
      0x0L, 0UL);
  const vui128_t mul_ten16 = (vui128_t) CONST_VINT128_DW(
      0UL, 10000000000000000UL);
  // Magic numbers for multiplicative inverse to divide by 10**16
  // are 76624777043294442917917351357515459181, no corrective add,
  // and shift right 51 bits.
  const vui128_t mul_invs_ten16 = (vui128_t) CONST_VINT128_DW(
      0x39a5652fb1137856UL, 0xd30baf9a1e626a6dUL);
  const int shift_ten16 = 51;

  vui128_t tmpq, tmp;
  vui64_t t_low, t_mid, t_high;
  vui128_t val128;
  char sign;

  if (vec_cmpsq_all_ge (value, zero128))
    {
      sign = ' ';
      val128 = (vui128_t) value;
    }
  else
    {
      sign = '-';
      val128 = vec_subuqm ((vui128_t) zero128, (vui128_t) value);
    }
  // Convert the absolute (unsigned) value to Decimal and
  // prefix the sign.

  // first divide/modulo the 39 digits __int128 by 10**16.
  // This separates the high/middle 23 digits (tmpq) and low 16 digits.
  tmpq = vec_mulhuq (val128, mul_invs_ten16);
  tmpq = vec_srqi (tmpq, shift_ten16);
  // Compute remainder of val128 / 10**16
  // t_low = val128 - (tmpq * 10**16)
  // Here we know tmpq and mul_ten16 are less then 64-bits
  // so can use vec_vmuloud instead of vec_mulluq
  tmp = vec_vmuloud ((vui64_t) tmpq, (vui64_t) mul_ten16);
  t_low = (vui64_t) vec_subuqm (val128, tmp);

  // Next divide/modulo the high/middle digits by 10**16.
  // This separates the high 7 and middle 16 digits.
  val128 = tmpq;
  tmpq = vec_mulhuq (tmpq, mul_invs_ten16);
  t_high = (vui64_t) vec_srqi (tmpq, shift_ten16);
  tmp = vec_vmuloud (t_high, (vui64_t) mul_ten16);
  t_mid = (vui64_t) vec_subuqm (val128, tmp);

  printf ("%c%07lld%016lld%016lld", sign, t_high[VEC_DW_L],
	  t_mid[VEC_DW_L], t_low[VEC_DW_L]);
}
 * \endcode
 *
 * \subsection int128_examples_0_1_2 Converting Vector __int128 values to BCD
 *
 * POWER8 and POWER9 added a number of Binary Code Decimal (BCD)
 * and Zoned Decimal operations that should be helpful for radix
 * conversion and even faster large integer formatting for print.
 * \sa vec_bcd_ppc.h
 *
 * The issue remains that __int128 values can represent up to 39
 * decimal digits while Signed BCD supports only 31 digits. POWER9
 * provides a <B>Decimal Convert From Signed Quadword</B> instruction
 * with the following restriction:
 *
 * \note If the signed value of vrb is less then -(10**31-1)
 * or greater than 10**31-1 the result is too large for the BCD format
 * and the result is undefined.
 *
 * It would be useful to check for this and if required, factor the
 * __int128 value into to the high order 8 digits and the low order 31
 * digits. This allows for the safe and correct use of the
 * vec_bcdcfsq() and with some decimal shifts/truncates vec_bcdctz().
 * This also enables conversion to multiple precision Vector BCD to
 * represent 39 digits and more for radix conversions.
 *
 * We first address the factoring by providing
 * <B>Vector Divide by const 10e31 Unsigned Quadword</B> and
 * <B>Vector Modulo by const 10e31 Unsigned Quadword</B> operation.
 * This requires the multiplicative inverse using the
 * vec_mulhuq() operation.
 *
 * \code
static inline vui128_t
vec_divuq_10e31 (vui128_t vra)
  // ten32  = +100000000000000000000000000000000UQ
  const vui128_t ten31 = (vui128_t)
	  { (__int128) 1000000000000000UL * (__int128) 10000000000000000UL };
  // Magic numbers for multiplicative inverse to divide by 10**31
  // are 4804950418589725908363185682083061167, corrective add,
  // and shift right 107 bits.
  const vui128_t mul_invs_ten31 = (vui128_t) CONST_VINT128_DW(
      0x039d66589687f9e9UL, 0x01d59f290ee19dafUL);
  const int shift_ten31 = 103;
  vui128_t result, t, q;

  if (vec_cmpuq_all_ge (vra, ten31))
    {
      q = vec_mulhuq (vra, mul_invs_ten31);
      // Need corrective add but want to avoid carry & double quad shift
      // The following avoids the carry and less instructions
      t = vec_subuqm (vra, q);
      t = vec_srqi (t, 1);
      t = vec_adduqm (t, q);
      result = vec_srqi (t, (shift_ten31 - 1));
    }
  else
    result = (vui128_t) { (__int128) 0 };

  return result;
}
 * \endcode
 * As the vec_mulhuq() operation is relatively expensive and we expect
 * most __int128 values to 31-digits or less, using a compare to bypass
 * the multiplication and return the 0 quotient, seems a prudent
 * optimization.
 *
 * So far we only have the quotient (the high order 8 digits) and still
 * need to extract the remainder (the low order 31 digits). This is
 * simply the quotient from above multiplied by 10e31 and subtracted
 * from the original input. To avoid the multiple return value issue
 * we define a modulo operation to take the original value and the
 * quotient from vec_divuq_10e31().
 *
 * \code
static inline vui128_t
vec_moduq_10e31 (vui128_t vra, vui128_t q)
{
  // ten32  = +100000000000000000000000000000000UQ
  const vui128_t ten31 = (vui128_t)
	  { (__int128) 1000000000000000UL * (__int128) 10000000000000000UL };
  vui128_t result, t;

  if (vec_cmpuq_all_ge (vra, ten31))
    {
      t = vec_mulluq (q, ten31);
      result = vec_subuqm (vra, t);
    }
  else
    result = vra;

  return result;
}
 * \endcode
 * Again as the vec_mulluq() operation is relatively expensive and we expect
 * most __int128 values to 31-digits or less, using a compare to bypass
 * the multiplication and return the input value as the remainder,
 * seems a prudent optimization.
 *
 * We expect these operations to be used together as in this example.
 * \code
  q = vec_divuq_10e31 (a);
  r = vec_moduq_10e31 (a, q);
 * \endcode
 * We also expect the compiler to common the various constant loads across
 * the two operations as the code is in-lined.
 * This header also provides variants for factoring by 10e32
 * (to use with the Zone conversion) and
 * signed variants of the 10e31 operation for direct conversion to
 * extend precision signed BCD.
 * \sa vec_divuq_10e32(), vec_moduq_10e32(), vec_divsq_10e31, vec_modsq_10e31.
 *
 * \subsection int128_examples_0_1_3 Extending integer operations beyond Quadword
 *
 * Some algorithms require even high integer precision than __int128 provides.
 * this includes:
 * - POSIX compliant conversion between __float128 and _Decimal128 types
 * - POSIX compliant conversion from double and __float128 to decimal for print.
 * - Cryptographic operations for Public-key cryptography and Elliptic Curves
 *
 * The POWER8 provides instructions for extending add and subtract to
 * 128-bit integer and beyond with carry/extend operations
 * (see vec_addcuq(), vec_addecuq(), vec_addeuqm(), vec_adduqm(),
 * (see vec_subcuq(), vec_subecuq(), vec_subeuqm(), vec_subuqm()).
 * POWER9 adds instructions to improve decimal / binary conversion
 * to/from 128-bit integer and beyond with carry/extend operations.
 * And while the PowerISA does not yet provide full 128 x 128 bit
 * integer multiply instructions, it has provided wider integer
 * multiply instructions, beginning in POWER8
 * (see vec_mulesw(), vec_mulosw(), vec_muleuw(), vec_mulouw())
 * and again in POWER9 (see vec_msumudm()).
 *
 * This all allows the <B>pveclib</B> to improve (reduce the latency of)
 * the implementation of multiply quadword operations.
 * This includes operations that generate the full 256-bit multiply
 * product (see vec_muludq(), vec_mulhuq(). vec_mulluq()).
 * And this in combination with add/subtract with carry extend quadword
 * allows the coding of even wider (multiple quadword) multiply
 * operations.
 *
 * \subsubsection int128_examples_0_1_3_0 Extended Quadword multiply
 *
 * The following example performs a 256x256 bit unsigned integer
 * multiply generating a 512-bit product:
 * \code
void
test_mul4uq (vui128_t *__restrict__ mulu, vui128_t m1h, vui128_t m1l,
	     vui128_t m2h, vui128_t m2l)
{
  vui128_t mc, mp, mq;
  vui128_t mphh, mphl, mplh, mpll;
  mpll = vec_muludq (&mplh, m1l, m2l);
  mp = vec_muludq (&mphl, m1h, m2l);
  mplh = vec_addcq (&mc, mplh, mp);
  mphl = vec_addcuq (mphl, mc);
  mp = vec_muludq (&mc, m2h, m1l);
  mplh = vec_addcq (&mq, mplh, mp);
  mphl = vec_addcq (&mc, mphl, mq);
  mp = vec_muludq (&mphh, m2h, m1h);
  mplh = vec_addcq (&mc, mplh, mp);
  mphl = vec_addcuq (mphh, mc);

  mulu[0] = mpll;
  mulu[1] = mplh;
  mulu[2] = mphl;
  mulu[3] = mphh;
}
 * \endcode
 * This example generates some additional questions:
 * - Why use vec_muludq() instead of pairing vec_mulhuq() and
 * vec_mulluq()?
 * - Why use vec_addcq() instead of pairing vec_addcuq() and
 * vec_adduqm()?
 * - Why return the 512-bit product via a pointer instead of returning
 * a struct or array of 4 x vui128_t (<I>homogeneous aggregates</I>)?
 *
 * The detailed rationale for this is documented in section
 * \ref mainpage_sub_1_3
 * In this specific case (quadword integer operations that generate
 * two vector values) <B>pveclib</B> provides both alternatives:
 * - separate operations each returning a single (high or low order)
 * vector.
 * - combined operations providing:
 *   - the lower order vector as the function return value.
 *   - the high order (carry or high product) vector via a pointer
 *   reference parameter.
 *
 * Either method should provide the same results. For example:
 * \code
  mplh = vec_addcq (&mc, mplh, mp);
 * \endcode
 * is equivalent to
 * \code
  mc   = vec_addcuq (mplh, mp);
  mplh = vec_adduqm (mplh, mp);
 * \endcode
 * and
 * \code
  mpll = vec_muludq (&mplh, m1l, m2l);
 * \endcode
 * is equivalent to
 * \code
  mpll = vec_mulluq (m1l, m2l);
  mplh = vec_mulhud (m1l, m2l);
 * \endcode
 * So is there any advantage to separate versus combined operations?
 *
 * Functionally it is useful to have separate operations for the cases
 * where only one quadword part is needed.
 * For example if you know that a add/subtract operation can not
 * overflow, why generate the carry?
 * Alternatively the quadword greater/less-than compares are based
 * solely on the carry from the subtract quadword,
 * why generate lower 128-bit (modulo) difference?
 * For multiplication the modulo (multiply low) operation is the
 * expected semantic or is known to be sufficient.
 * Alternatively the multiplicative inverse only uses the high order
 * (multiply high) quadword of the product.
 *
 * From the performance (instruction latency and throughput)
 * perspective,
 * if the algorithm requires the extended result or full product,
 * the combined operation is usually the better choice.
 * Otherwise use the specific single return operation needed.
 * At best, the separate operations may generate the same instruction
 * sequence as the combined operation, But this depends on the target
 * platform and specific optimizations implemented by the compiler.
 *
 * \note For inlined operations the pointer reference in the combined
 * form, is usually optimized to a simple register assignment,
 * by the compiler.
 * \note For platform targets where the separate operations each
 * generate a single instruction, we expect the compiler to generate
 * the same instructions as the combined operation.
 * But this is only likely for add/sub quadword on the POWER8
 * and multiply by 10 quadword on POWER9.
 *
 * \subsubsection int128_examples_0_1_3_1 Quadword Long Division
 *
 * In the section \ref int128_examples_0_1_2 above we used
 * multiplicative inverse to factor a binary quadword value in two
 * (high quotient and low remainder) parts. Here we divide by a large
 * power of 10 (10<SUP>31</SUP> or 10<SUP>32</SUP>) of a size where
 * the quotient and remainder allow direct conversion to BCD
 * (see vec_bcdcfsq(), vec_bcdcfuq()). After conversion, the BCD parts
 * can be concatenated to form the larger (39 digit) decimal radix
 * value equivalent of the 128-bit binary value.
 *
 * We can extend this technique to larger (multiple quadword) binary
 * values but this requires long division. This is the version of the
 * long division you learned in grade school, where a multi-digit value
 * is divided in stages by a single digit. But the digits we are using
 * are really big (10<SUP>31</SUP>-1 or 10<SUP>32</SUP>-1).
 *
 * The first step is relatively easy. Start by dividing the left-most
 * <I>digit</I> of the dividend by the divisor, generating the integer
 * quotient and remainder. We already have operations to implement that.
 * \code
  // initial step for the top digits
  dn = d[0];
  qh = vec_divuq_10e31 (dn);
  rh = vec_moduq_10e31 (dn, qh);
  q[0] = qh;
 * \endcode
 * The array <I>d</I> contains the quadwords of the extended precision
 * integer dividend. The array <I>q</I> will contain the quadwords of
 * the extended precision integer quotient. Here we have generated the
 * first <I>quadword q[0]</I> digit of the quotient. The remainder
 * <I>rh</I> will be used in the next step of the long division.
 *
 * The process repeats except after the first step we have an
 * intermediate dividend formed from:
 * - The remainder from the previous step
 * - Concatenated with the next <I>digit</I> of the extended precision
 * quadword dividend.
 *
 * So for each additional step we need to divide two quadwords
 * (256-bits) by the quadword divisor. Actually this dividend should
 * be less than a full 256-bits because we know the remainder is less
 * than the divisor. So the intermediate dividend is less than
 * ((divisor - 1) * 2<SUP>128</SUP>). So we know the quotient can not
 * exceed (2<SUP>128</SUP>-1) or one quadword.
 *
 * Now we need an operation that will divide this double quadword
 * value and provide quotient and remainder that are correct
 * (or close enough).
 * Remember your grade school long division where you would:
 * - estimate the quotient
 * - multiply the quotient by the divisor
 * - subtract this product from the current 2 digit dividend
 * - check that the remainder is less than the divisor.
 *   - if the remainder is greater than the divisor; the estimated quotient is too small
 *   - if the remainder is negative (the product was greater than the dividend); the estimated quotient is too large.
 * - correct the quotient and remainder if needed before doing the next step.
 *
 * So we don't need to be perfect, but close enough.
 * As long as we can detect any problems and (if needed) correct the
 * results, we can implement long division to any size.
 *
 * We already have an operation for dividing a quadword by 10<SUP>31</SUP>
 * using the magic numbers for multiplicative inverse.
 * This can easily be extended to multiply double quadword high.
 * For example:
 * \code
      // Multiply high [vra||vrb] * mul_invs_ten31
      q = vec_mulhuq (vrb, mul_invs_ten31);
      q1 = vec_muludq (&t, vra, mul_invs_ten31);
      c = vec_addcuq (q1, q);
      q = vec_adduqm (q1, q);
      q1 = vec_adduqm (t, c);
      // corrective add [q2||q1||q] = [q1||q] + [vra||vrb]
      c = vec_addcuq (vrb, q);
      q = vec_adduqm (vrb, q);
      // q2 is the carry-out from the corrective add
      q2 = vec_addecuq (q1, vra, c);
      q1 = vec_addeuqm (q1, vra, c);
      // shift 384-bits (including the carry) right 107 bits
      // Using shift left double quadword shift by (128-107)-bits
      r2 = vec_sldqi (q2, q1, (128 - shift_ten31));
      result = vec_sldqi (q1, q, (128 - shift_ten31));
 * \endcode
 * Here we generate a 256-bit multiply high using the vec_mulhuq()
 * for the low dividend (vrb) and vec_muludq() for high dividend (vra).
 * Then sum the partial products ([t||q1] + [0||q]) to get initial 256-bit product [q1||q].
 * Then apply the corrective add ([q1||q] + [vra||vrb]).
 * This may generate a carry which needs to be included in the final shift.
 *
 * Technically we only expect a 128-bit quotient after the shift,
 * but we have 3 quadwords (2 quadwords and a carry)
 * going into the shift right. Also our (estimated) quotient may be
 * <I>off by 1</I> and generate a 129-bit result.
 * This is due to using a the magic numbers for 128-bit multiplicative
 * inverse and not regenerating magic numbers for 256-bits.
 * We can't do anything about that now
 * and so return a 256-bit double quadword quotient.
 *
 * \note This is where only needing to be "close enough", works in our favor.
 * We will check and correct the quotient in the modulo operation.
 *
 * The 256-bits we want are spanning multiple quadwords so we replace
 * a simple quadword shift right with two <B>Shift Left Double Quadword
 * Immediate</B> operations and complement the shift count
 * (128 - shift_ten31). This gives a 256-bit quotient which we expect
 * to have zero in the high quadword.
 *
 * As this operation will be used in a loop for long division
 * operations and the extended multiplies are fairly expensive,
 * we should check for an short-circuit special conditions.
 * The most important special condition is when the dividend is less
 * that the divisor and the quotient is zero.
 * This also helps when the long division dividend may have
 * leading quadword zeros that need to be skipped over.
 * For the full implementation looks like:
 * \code
static inline vui128_t
vec_divudq_10e31 (vui128_t *qh, vui128_t vra, vui128_t vrb)
{
  const vui128_t ten31 = (vui128_t)
	  { (__int128) 1000000000000000UL * (__int128) 10000000000000000UL };
  const vui128_t zero = (vui128_t) { (__int128) 0UL };
  // Magic numbers for multiplicative inverse to divide by 10**31
  // are 4804950418589725908363185682083061167, corrective add,
  // and shift right 103 bits.
  const vui128_t mul_invs_ten31 = (vui128_t) CONST_VINT128_DW(
      0x039d66589687f9e9UL, 0x01d59f290ee19dafUL);
  const int shift_ten31 = 103;
  vui128_t result, r2, t, q, q1, q2, c;

  if (vec_cmpuq_all_ne (vra, zero) || vec_cmpuq_all_ge (vrb, ten31))
    {
      // Multiply high [vra||vrb] * mul_invs_ten31
      q = vec_mulhuq (vrb, mul_invs_ten31);
      q1 = vec_muludq (&t, vra, mul_invs_ten31);
      c = vec_addcuq (q1, q);
      q = vec_adduqm (q1, q);
      q1 = vec_adduqm (t, c);
      // corrective add [q2||q1||q] = [q1||q] + [vra||vrb]
      c = vec_addcuq (vrb, q);
      q = vec_adduqm (vrb, q);
      // q2 is the carry-out from the corrective add
      q2 = vec_addecuq (q1, vra, c);
      q1 = vec_addeuqm (q1, vra, c);
      // shift 384-bits (including the carry) right 103 bits
      // Using shift left double quadword shift by (128-103)-bits
      r2 = vec_sldqi (q2, q1, (128 - shift_ten31));
      result = vec_sldqi (q1, q, (128 - shift_ten31));
    }
  else
    {
      // Dividend is less than divisor then return zero quotient
      r2 = zero;
      result = zero;
    }

  // return 256-bit quotient
  *qh = r2;
  return result;
}
 * \endcode
 *
 * To complete the long division operation we need to perform double
 * quadword modulo operations.
 * Here the dividend is two quadwords and the low quadword
 * of the quotient from the divide double quadword operation above.
 * We use multiply double quadword to compute the remainder
 * ([vra||vrb] - (q * 10<SUP>31</SUP>).
 * Generating the 256-bit product and difference ensure we can detect
 * the case where the quotient is off-by-1 on the high side.
 *
 * \code
      t = vec_muludq (&th, *ql, ten31);
      c = vec_subcuq (vrb, t);
      t = vec_subuqm (vrb, t);
      th = vec_subeuqm (vra, th, c);
      // The remainder should be less than the divisor
      if (vec_cmpuq_all_ne (th, zero) && vec_cmpuq_all_ge (t, ten31))
	{
	  // Otherwise the estimated quotient is off by 1
	  *ql = vec_adduqm (*ql, minus_one);
	  // And the remainder is negative, so add the divisor
	  t = vec_adduqm (t, ten31);
	}
      result = t;
 * \endcode
 * In this case we need to correct both remainder and the (estimated) quotient.
 * This is a bit tricky as the quotient is normally passed by value,
 * but for this operation we need to pass by reference,
 * which allows the corrected quotient to be passed on to the next step.
 *
 * Again as this operation will be used in a loop for long division
 * operations and the extended multiplies are fairly expensive,
 * we should check for and short-circuit special conditions.
 * The most important special condition is when the dividend is less
 * that the divisor and the remainder is simply the dividend.
 *
 * \code
static inline vui128_t
vec_modudq_10e31 (vui128_t vra, vui128_t vrb, vui128_t *ql)
{
  // ten31  = +100000000000000000000000000000000UQ
  const vui128_t ten31 = (vui128_t)
	  { (__int128) 1000000000000000UL * (__int128) 10000000000000000UL };
  const vui128_t zero = (vui128_t) { (__int128) 0UL };
  const vui128_t minus_one = (vui128_t) { (__int128) -1L };
  vui128_t result, t, th, c;

  if (vec_cmpuq_all_ne (vra, zero) || vec_cmpuq_all_ge (vrb, ten31))
    {
      t = vec_muludq (&th, *ql, ten31);
      c = vec_subcuq (vrb, t);
      t = vec_subuqm (vrb, t);
      th = vec_subeuqm (vra, th, c);
      // The remainder should be less than the divisor
      if (vec_cmpuq_all_ne (th, zero) && vec_cmpuq_all_ge (t, ten31))
	{
	  // If not the estimated quotient is off by 1
	  *ql = vec_adduqm (*ql, minus_one);
	  // And the remainder is negative, so add the divisor
	  t = vec_adduqm (t, ten31);
	}
      result = t;
    }
  else
    result = vrb;

  return result;
}
 * \endcode
 *
 * Now we have all the operations needed to complete the implementation
 * of long division by the decimal constant (10<SUP>31</SUP>).
 *
 * \code
vui128_t
example_longdiv_10e31 (vui128_t *q, vui128_t *d, long int _N)
{
  vui128_t dn, qh, ql, rh;
  long int i;

  // initial step for the top digits
  dn = d[0];
  qh = vec_divuq_10e31 (dn);
  rh = vec_moduq_10e31 (dn, qh);
  q[0] = qh;

  // now we know the remainder is less than the divisor.
  for (i=1; i<_N; i++)
    {
      dn = d[i];
      ql = vec_divudq_10e31 (&qh, rh, dn);
      rh = vec_modudq_10e31 (rh, dn, &ql);
      q[i] = ql;
    }
  // return the final remainder
  return rh;
}
 * \endcode
 * The result of each call to example_longdiv_10e31() is the output
 * array <I>q</I> of quadwords containing the extended quotient,
 * and the remainder as the return value.
 * The input array <I>d</I> and output array <I>q</I> should not
 * overlap in storage.
 * The remainder is in the range 0-9999999999999999999999999999999
 * and is suitable for conversion to BCD or decimal characters.
 * (see vec_bcdcfsq()).
 * Repeated calls passing the quotient from the previous call as the
 * dividend, reduces the quotient by 31 digits and returns another 31
 * digits in the remainder for conversion.
 * This continues until the quotient is less than 10<SUP>31</SUP>
 * which provides the highest order digits of the decimal result.
 *
 * \note Similarly for long division in support of unsigned 32-digit
 * BCD conversion using operations; vec_divuq_10e32(),
 * vec_moduq_10e32(), vec_divudq_10e32(), and vec_modudq_10e32().
 * Long division for other constant divisors or multiple quadword
 * divisors is an exercise for the student.
 *
 * \todo
 * The implementation above gives correct results for all the cases
 * tested for divide by constants 10<SUP>31</SUP> and 10<SUP>32</SUP>).
 * This is not a mathematical proof of correctness, just an observation.
 * Anyone who finds a counter example or offers a mathematical proof
 * should submit a bug report.
 *
 * \section int128_perf_0_0 Performance data.
 * High level performance estimates are provided as an aid to function
 * selection when evaluating algorithms. For background on how
 * <I>Latency</I> and <I>Throughput</I> are derived see:
 * \ref perf_data
 */
///@cond INTERNAL
static inline vui128_t vec_addecuq (vui128_t a, vui128_t b, vui128_t ci);
static inline vui128_t vec_addeuqm (vui128_t a, vui128_t b, vui128_t ci);
static inline vb128_t vec_cmpequq (vui128_t vra, vui128_t vrb);
static inline vb128_t vec_cmpgeuq (vui128_t vra, vui128_t vrb);
static inline vb128_t vec_cmpgtuq (vui128_t vra, vui128_t vrb);
static inline vb128_t vec_cmpleuq (vui128_t vra, vui128_t vrb);
static inline vb128_t vec_cmpltuq (vui128_t vra, vui128_t vrb);
static inline vb128_t vec_cmpneuq (vui128_t vra, vui128_t vrb);
static inline vui128_t vec_divuq_10e31 (vui128_t vra);
static inline vui128_t vec_divuq_10e32 (vui128_t vra);
static inline vui128_t vec_maxuq (vui128_t a, vui128_t b);
static inline vui128_t vec_minuq (vui128_t a, vui128_t b);
static inline vui128_t vec_moduq_10e31 (vui128_t vra, vui128_t q);
static inline vui128_t vec_moduq_10e32 (vui128_t vra, vui128_t q);
static inline vui128_t vec_muleud (vui64_t a, vui64_t b);
static inline vui128_t vec_mulhuq (vui128_t a, vui128_t b);
static inline vui128_t vec_mulluq (vui128_t a, vui128_t b);
static inline vui128_t vec_muloud (vui64_t a, vui64_t b);
static inline vui128_t vec_muludq (vui128_t *mulu, vui128_t a, vui128_t b);
static inline vb128_t vec_setb_cyq (vui128_t vcy);
static inline vb128_t vec_setb_ncq (vui128_t vcy);
static inline vb128_t vec_setb_sq (vi128_t vra);
static inline vui128_t vec_sldq (vui128_t vrw, vui128_t vrx,
				 vui128_t vrb);
static inline vui128_t vec_sldqi (vui128_t vrw, vui128_t vrx,
				  const unsigned int shb);
static inline vui128_t vec_srqi (vui128_t vra, const unsigned int shb);
static inline vui128_t vec_subcuq (vui128_t vra, vui128_t vrb);
static inline vui128_t vec_subeuqm (vui128_t vra, vui128_t vrb, vui128_t vrc);
static inline vui128_t vec_subuqm (vui128_t vra, vui128_t vrb);
static inline vui128_t vec_vmuleud (vui64_t a, vui64_t b);
static inline vui128_t vec_vmuloud (vui64_t a, vui64_t b);
///@endcond

/** \brief Vector Absolute Difference Unsigned Quadword.
 *
 *  Compute the absolute difference of the quadwords.
 *  For each unsigned quadword, subtract VRB from VRA and
 *  return the absolute value of the difference.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  14   | 1/cycle  |
 *  |power9   |  11   | 1/cycle  |
 *
 *  @param vra vector of unsigned __int128
 *  @param vrb vector of unsigned __int128
 *  @return vector of the absolute difference.
 */
static inline vui128_t
vec_absduq (vui128_t vra, vui128_t vrb)
{
#ifdef _ARCH_PWR8
  vui128_t tmp1, tmp2;
  vb128_t cmpbool;
  cmpbool = vec_cmpgtuq ( vra, vrb );
  tmp1 = vec_subuqm ( vra, vrb );
  tmp2 = vec_subuqm ( vrb, vra );
  return (vui128_t) vec_sel ((vui32_t) tmp2, (vui32_t) tmp1, (vui32_t) cmpbool);
#else
  return vec_subuqm (vec_maxuq (vra, vrb), vec_minuq (vra, vrb));
#endif
}

/** \brief Vector Average Unsigned Quadword.
 *
 *  Compute the average of two unsigned quadwords as
 *  (VRA + VRB + 1) / 2.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  14   | 1/cycle  |
 *  |power9   |  11   | 1/cycle  |
 *
 *  @param vra vector unsigned quadwords
 *  @param vrb vector unsigned quadwords
 *  @return vector of the absolute differences.
 */
static inline vui128_t
vec_avguq (vui128_t vra, vui128_t vrb)
{
  vui128_t result, tmp1, tmp2;
  const vui128_t qu1 = (vui128_t) CONST_VINT128_W(0, 0, 0, 1);
  // Compute (vra + vrb + 1) with carry
  tmp1 = vec_addeuqm (vra, vrb, qu1);
  tmp2 = vec_addecuq (vra, vrb, qu1);
  // shift sum with carry, right 1 bit
  result = vec_sldqi (tmp2, tmp1, 127);

  return result;
}

/** \brief Vector Add & write Carry Unsigned Quadword.
 *
 *  Add two vector __int128 values and return the carry out.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     |2/2 cycles|
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated a __int128.
 *  @param b 128-bit vector treated a __int128.
 *  @return __int128 carry of the sum of a and b.
 */
static inline vui128_t
vec_addcuq (vui128_t a, vui128_t b)
{
  vui32_t co;
#ifdef _ARCH_PWR8
#ifndef vec_vaddcuq
  __asm__(
      "vaddcuq %0,%1,%2;"
      : "=v" (co)
      : "v" (a),
      "v" (b)
      : );
#else
  co = (vui32_t) vec_vaddcuq (a, b);
#endif
#else
  vui32_t c, c2, t;
  vui32_t z= { 0,0,0,0};

  co = vec_vaddcuw ((vui32_t)a, (vui32_t)b);
  t = vec_vadduwm ((vui32_t)a, (vui32_t)b);
  c = vec_sld (co, z, 4);
  c2 = vec_vaddcuw (t, c);
  t = vec_vadduwm (t, c);
  co = vec_vor (co, c2);
  c = vec_sld (c2, z, 4);
  c2 = vec_vaddcuw (t, c);
  t = vec_vadduwm (t, c);
  co = vec_vor (co, c2);
  c = vec_sld (c2, z, 4);
  c2 = vec_vaddcuw (t, c);
  co = vec_vor (co, c2);
  co = vec_sld (z, co, 4);
#endif
  return ((vui128_t) co);
}

 /** \brief Vector Add Extended & write Carry Unsigned Quadword.
  *
  *  Add two vector __int128 values plus a carry-in (0|1) and return
  *  the carry out bit.
  *
  *  |processor|Latency|Throughput|
  *  |--------:|:-----:|:---------|
  *  |power8   | 4     |2/2 cycles|
  *  |power9   | 3     | 2/cycle  |
  *
  *  @param a 128-bit vector treated a __int128.
  *  @param b 128-bit vector treated a __int128.
  *  @param ci Carry-in from vector bit[127].
  *  @return carry-out in bit[127] of the sum of a + b + c.
  */
 static inline vui128_t
 vec_addecuq (vui128_t a, vui128_t b, vui128_t ci)
 {
   vui32_t co;
 #ifdef _ARCH_PWR8
 #ifndef vec_vaddecuq
   __asm__(
       "vaddecuq %0,%1,%2,%3;"
       : "=v" (co)
       : "v" (a),
       "v" (b),
       "v" (ci)
       : );
 #else
   co = (vui32_t) vec_vaddecuq (a, b, ci);
 #endif
 #else
   vui32_t c, c2, t;
   vui32_t z = { 0, 0, 0, 0 };
   co = (vui32_t){ 1, 1, 1, 1 };

   c2 = vec_and ((vui32_t) ci, co);
   c2 = vec_sld ((vui32_t) c2, z, 12);
   co = vec_vaddcuw ((vui32_t) a, (vui32_t) b);
   t = vec_vadduwm ((vui32_t) a, (vui32_t) b);
   c = vec_sld (co, c2, 4);
   c2 = vec_vaddcuw (t, c);
   t = vec_vadduwm (t, c);
   co = vec_vor (co, c2);
   c = vec_sld (c2, z, 4);
   c2 = vec_vaddcuw (t, c);
   t = vec_vadduwm (t, c);
   co = vec_vor (co, c2);
   c = vec_sld (c2, z, 4);
   c2 = vec_vaddcuw (t, c);
   t = vec_vadduwm (t, c);
   co = vec_vor (co, c2);
   c = vec_sld (c2, z, 4);
   c2 = vec_vaddcuw (t, c);
   co = vec_vor (co, c2);
   co = vec_sld (z, co, 4);
 #endif
   return ((vui128_t) co);
 }

/** \brief Vector Add Extended Unsigned Quadword Modulo.
 *
 *  Add two vector __int128 values plus a carry (0|1) and return
 *  the modulo 128-bit result.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     |2/2 cycles|
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated a __int128.
 *  @param b 128-bit vector treated a __int128.
 *  @param ci Carry-in from vector bit[127].
 *  @return __int128 sum of a + b + c, modulo 128-bits.
 */
static inline vui128_t
vec_addeuqm (vui128_t a, vui128_t b, vui128_t ci)
{
  vui32_t t;
#ifdef _ARCH_PWR8
#ifndef vec_vaddeuqm
  __asm__(
      "vaddeuqm %0,%1,%2,%3;"
      : "=v" (t)
      : "v" (a),
      "v" (b),
      "v" (ci)
      : );
#else
  t = (vui32_t) vec_vaddeuqm (a, b, ci);
#endif
#else
  vui32_t c2, c;
  vui32_t z  = { 0,0,0,0};
  vui32_t co = { 1,1,1,1};

  c2 = vec_and ((vui32_t)ci, co);
  c2 = vec_sld ((vui32_t)ci, z, 12);
  co = vec_vaddcuw ((vui32_t)a, (vui32_t)b);
  t = vec_vadduwm ((vui32_t)a, (vui32_t)b);
  c = vec_sld (co, c2, 4);
  c2 = vec_vaddcuw (t, c);
  t = vec_vadduwm (t, c);
  c = vec_sld (c2, z, 4);
  c2 = vec_vaddcuw (t, c);
  t = vec_vadduwm (t, c);
  c = vec_sld (c2, z, 4);
  c2 = vec_vaddcuw (t, c);
  t = vec_vadduwm (t, c);
  c = vec_sld (c2, z, 4);
  t = vec_vadduwm (t, c);
#endif
  return ((vui128_t) t);
}

/** \brief Vector Add Unsigned Quadword Modulo.
 *
 *  Add two vector __int128 values and return result modulo 128-bits.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     |2/2 cycles|
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param a 128-bit vector treated as a __int128.
 *  @param b 128-bit vector treated as a __int128.
 *  @return __int128 sum of a and b.
 */
static inline vui128_t
vec_adduqm (vui128_t a, vui128_t b)
{
  vui32_t t;
#ifdef _ARCH_PWR8
#ifndef vec_vadduqm
  __asm__(
      "vadduqm %0,%1,%2;"
      : "=v" (t)
      : "v" (a),
      "v" (b)
      : );
#else
  t = (vui32_t) vec_vadduqm (a, b);
#endif
#else
  vui32_t c, c2;
  vui32_t z= { 0,0,0,0};

  c = vec_vaddcuw ((vui32_t)a, (vui32_t)b);
  t = vec_vadduwm ((vui32_t)a, (vui32_t)b);
  c = vec_sld (c, z, 4);
  c2 = vec_vaddcuw (t, c);
  t = vec_vadduwm (t, c);
  c = vec_sld (c2, z, 4);
  c2 = vec_vaddcuw (t, c);
  t = vec_vadduwm (t, c);
  c = vec_sld (c2, z, 4);
  t = vec_vadduwm (t, c);
#endif
  return ((vui128_t) t);
}

/** \brief Vector Add with carry Unsigned Quadword.
 *
 *  Add two vector __int128 values and return sum and the carry out.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8     |1/2 cycles|
 *  |power9   | 6     | 2/cycle  |
 *
 *  @param *cout carry out from the sum of a and b.
 *  @param a 128-bit vector treated a __int128.
 *  @param b 128-bit vector treated a __int128.
 *  @return __int128 (lower 128-bits) sum of a and b.
 */
static inline vui128_t
vec_addcq (vui128_t *cout, vui128_t a, vui128_t b)
{
  vui32_t t, co;
#ifdef _ARCH_PWR8
#ifndef vec_vadduqm
  __asm__(
      "vadduqm %0,%2,%3;\n"
      "\tvaddcuq %1,%2,%3;"
      : "=v" (t),
      "=v" (co)
      : "v" (a),
      "v" (b)
      : );
#else
  t = (vui32_t) vec_vadduqm (a, b);
  co = (vui32_t) vec_vaddcuq (a, b);
#endif
#else
  vui32_t c, c2;
  vui32_t z= { 0,0,0,0};

  co = vec_vaddcuw ((vui32_t)a, (vui32_t)b);
  t = vec_vadduwm ((vui32_t)a, (vui32_t)b);
  c = vec_sld (co, z, 4);
  c2 = vec_vaddcuw (t, c);
  t = vec_vadduwm (t, c);
  co = vec_vor (co, c2);
  c = vec_sld (c2, z, 4);
  c2 = vec_vaddcuw (t, c);
  t = vec_vadduwm (t, c);
  co = vec_vor (co, c2);
  c = vec_sld (c2, z, 4);
  c2 = vec_vaddcuw (t, c);
  t = vec_vadduwm (t, c);
  co = vec_vor (co, c2);
  co = vec_sld (z, co, 4);
#endif
  *cout = (vui128_t) co;
  return ((vui128_t) t);
}

/** \brief Vector Add Extend with carry Unsigned Quadword.
 *
 *  Add two vector __int128 values plus a carry-in (0|1)
 *  and return sum and the carry out.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8     |1/2 cycles|
 *  |power9   | 6     | 2/cycle  |
 *
 *  @param *cout carry out from the sum of a and b.
 *  @param a 128-bit vector treated a __int128.
 *  @param b 128-bit vector treated a __int128.
 *  @param ci Carry-in from vector bit[127].
 *  @return __int128 (lower 128-bits) sum of a + b + c.
 */
static inline vui128_t
vec_addeq (vui128_t *cout, vui128_t a, vui128_t b, vui128_t ci)
{
  vui32_t t, co;
#ifdef _ARCH_PWR8
#ifndef vec_vaddeuqm
  __asm__(
      "vaddeuqm %0,%2,%3,%4;\n"
      "\tvaddecuq %1,%2,%3,%4;"
      : "=v" (t),
      "=v" (co)
      : "v" (a),
      "v" (b),
      "v" (c)
      : );
#else
  t = (vui32_t) vec_vaddeuqm (a, b, ci);
  co = (vui32_t) vec_vaddecuq (a, b, ci);
#endif
#else
  vui32_t c, c2;
  vui32_t z= { 0,0,0,0};
  co = (vui32_t){ 1,1,1,1};

  c2 = vec_and ((vui32_t)ci, co);
  c2 = vec_sld ((vui32_t)c2, z, 12);
  co = vec_vaddcuw ((vui32_t)a, (vui32_t)b);
  t = vec_vadduwm ((vui32_t)a, (vui32_t)b);
  c = vec_sld (co, c2, 4);
  c2 = vec_vaddcuw (t, c);
  t = vec_vadduwm (t, c);
  co = vec_vor (co, c2);
  c = vec_sld (c2, z, 4);
  c2 = vec_vaddcuw (t, c);
  t = vec_vadduwm (t, c);
  co = vec_vor (co, c2);
  c = vec_sld (c2, z, 4);
  c2 = vec_vaddcuw (t, c);
  t = vec_vadduwm (t, c);
  co = vec_vor (co, c2);
  c = vec_sld (c2, z, 4);
  c2 = vec_vaddcuw (t, c);
  t = vec_vadduwm (t, c);
  co = vec_vor (co, c2);
  co = vec_sld (z, co, 4);
#endif
  *cout = (vui128_t) co;
  return ((vui128_t) t);
}

/** \brief Vector Count Leading Zeros Quadword.
 *
 *  Count leading zeros for a vector __int128 and return the count in a
 *  vector suitable for use with vector shift (left|right) and vector
 *  shift (left|right) by octet instructions.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 19-28 | 1/cycle  |
 *  |power9   | 25-36 | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated a __int128.
 *  @return a 128-bit vector with bits 121:127 containing the count of
 *  leading zeros.
 */
static inline vui128_t
vec_clzq (vui128_t vra)
{
  vui64_t result;

#ifdef _ARCH_PWR8
  /*
   * Use the Vector Count Leading Zeros Double Word instruction to get
   * the count for the left and right vector halves.  If the left vector
   * doubleword of the input is nonzero then only the left count is
   * included and we need to mask off the right count.
   * Otherwise the left count is 64 and we need to add 64 to the right
   * count.
   * After masking we sum across the left and right counts to
   * get the final 128-bit vector count (0-128).
   */
  vui64_t vt1, vt2, vt3;
  vui64_t vzero = { 0, 0 };
  vui64_t v64 = { 64, 64 };

  vt1 = vec_vclz ((vui64_t) vra);
  vt2 = (vui64_t) vec_cmplt(vt1, v64);
  vt3 = (vui64_t) vec_sld ((vui8_t) vzero, (vui8_t) vt2, 8);
  result = vec_andc (vt1, vt3);
  result = (vui64_t) vec_sums ((vi32_t) result, (vi32_t) vzero);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  result = (vui64_t) vec_sld ((vui8_t) result, (vui8_t) result, 4);
#endif
#else
  /* vector clz instructions were introduced in power8. For power7 and
   * earlier, use the pveclib vec_clzw implementation.  For a quadword
   * clz, this requires pre-conditioning the input before computing the
   * the word clz and sum across.   */
  vui32_t c0, clz;
  vui32_t r32, gt32, gt32sr32, gt64sr64;

  c0 = vec_splat_u32 (0);
  gt32 = (vui32_t) vec_cmpgt ((vui32_t) vra, c0);
  gt32sr32 = vec_sld (c0, gt32, 12);
  gt64sr64 = vec_sld (c0, gt32, 8);
  gt32 = vec_sld (c0, gt32, 4);

  gt32sr32 = vec_or (gt32sr32, gt32);
  gt64sr64 = vec_or (gt64sr64, (vui32_t) vra);
  r32 = vec_or (gt32sr32, gt64sr64);

  clz = vec_clzw (r32);
  result = (vui64_t) vec_sums ((vi32_t) clz, (vi32_t) c0);
#endif

  return ((vui128_t) result);
}

/** \brief Vector Compare Equal Signed Quadword.
 *
 *  Compare signed __int128 (128-bit) integers and return all '1's,
 *  if vra == vrb, otherwise all '0's.  We use
 *  vec_cmpequq as it works for both signed and unsigned compares.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6     | 2/cycle  |
 *  |power9   | 7     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an signed __int128.
 *  @param vrb 128-bit vector treated as an signed __int128.
 *  @return 128-bit vector boolean reflecting vector signed __int128
 *  compare equal.
 */
static inline vb128_t
vec_cmpeqsq (vi128_t vra, vi128_t vrb)
{
  /* vec_cmpequq works for both signed and unsigned compares.  */
  return vec_cmpequq ((vui128_t) vra, (vui128_t) vrb);
}

/** \brief Vector Compare Equal Unsigned Quadword.
 *
 *  Compare unsigned __int128 (128-bit) integers and return all '1's,
 *  if vra == vrb, otherwise all '0's.
 *
 *  For POWER8 (PowerISA 2.07B) or later, use the Vector Compare
 *  Equal Unsigned DoubleWord (<B>vcmpequd</B>) instruction.
 *  To get the correct quadword result, the doubleword element equal
 *  truth values are swapped, then <I>anded</I> with the
 *  original compare results.
 *  Otherwise use vector word compare and additional boolean logic to
 *  insure all word elements are equal.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6     | 2/cycle  |
 *  |power9   | 7     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an unsigned __int128s.
 *  @param vrb 128-bit vector treated as an unsigned __int128.
 *  @return 128-bit vector boolean reflecting vector unsigned __int128
 *  compare equal.
 */
static inline vb128_t
vec_cmpequq (vui128_t vra, vui128_t vrb)
{
#ifdef _ARCH_PWR8
  vui64_t equd, swapd;

  equd = (vui64_t) vec_cmpequd ((vui64_t) vra, (vui64_t) vrb);
  swapd = vec_swapd (equd);
  return (vb128_t) vec_and (equd, swapd);
#else
  vui32_t equw, equ1, equ2, equ3;

  equw = (vui32_t) vec_cmpeq ((vui32_t) vra,
      (vui32_t) vrb);
  equ1 = vec_sld (equw, equw, 4);
  equ2 = vec_sld (equw, equw, 8);
  equ3 = vec_sld (equw, equw, 12);
  equw = vec_and (equw, equ1);
  equ2 = vec_and (equ2, equ3);
  return (vui128_t) vec_and (equw, equ2);
#endif
}

/** \brief Vector Compare Greater Than or Equal Signed Quadword.
 *
 *  Compare signed __int128 (128-bit) integers and return all '1's,
 *  if vra >= vrb, otherwise all '0's.
 *
 *  Flip the operand sign bits and use vec_cmpgeuq for signed compare.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-16 |1/ 2cycles|
 *  |power9   | 8-14  | 1/cycle  |
 *
 *  @param vra 128-bit vector treated as an signed __int128.
 *  @param vrb 128-bit vector treated as an signed __int128.
 *  @return 128-bit vector boolean reflecting vector signed __int128
 *  compare greater than.
 */
static inline vb128_t
vec_cmpgesq (vi128_t vra, vi128_t vrb)
{
  const vui32_t signbit = CONST_VINT128_W (0x80000000, 0, 0, 0);
  vui32_t _a, _b;

  _a = vec_xor ((vui32_t) vra, signbit);
  _b = vec_xor ((vui32_t) vrb, signbit);
  return vec_cmpgeuq ((vui128_t) _a, (vui128_t) _b);
}

/** \brief Vector Compare Greater Than or Equal Unsigned Quadword.
 *
 *  Compare unsigned __int128 (128-bit) integers and return all '1's,
 *  if vra >= vrb, otherwise all '0's.
 *
 *  For POWER8 (PowerISA 2.07B) or later, use the Vector Subtract &
 *  write Carry QuadWord (<B>vsubcuq</B>) instruction.
 *  This generates a carry for greater than or equal
 *  and NOT carry for less than.
 *  Then use vec_setb_cyq ro convert the carry into a vector bool.
 *  Here we use the pveclib implementations (vec_subcuq() and
 *  vec_setb_cyq()), instead of <altivec.h> intrinsics,
 *  to address older compilers and POWER7.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8     |2/ 2cycles|
 *  |power9   | 6     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an unsigned __int128.
 *  @param vrb 128-bit vector treated as an unsigned __int128.
 *  @return 128-bit vector boolean reflecting vector unsigned __int128
 *  compare greater than.
 */
static inline vb128_t
vec_cmpgeuq (vui128_t vra, vui128_t vrb)
{
  vui128_t a_b;

  a_b = vec_subcuq (vra, vrb);
  return vec_setb_cyq (a_b);
}

/** \brief Vector Compare Greater Than Signed Quadword.
 *
 *  Compare signed __int128 (128-bit) integers and return all '1's,
 *  if vra > vrb, otherwise all '0's.
 *
 *  Flip the operand sign bits and use vec_cmpgtuq for signed compare.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-16 |1/ 2cycles|
 *  |power9   | 8-14  | 1/cycle  |
 *
 *  @param vra 128-bit vector treated as an signed __int128.
 *  @param vrb 128-bit vector treated as an signed __int128.
 *  @return 128-bit vector boolean reflecting vector signed __int128
 *  compare greater than.
 */
static inline vb128_t
vec_cmpgtsq (vi128_t vra, vi128_t vrb)
{
  const vui32_t signbit = CONST_VINT128_W (0x80000000, 0, 0, 0);
  vui32_t _a, _b;

  _a = vec_xor ((vui32_t) vra, signbit);
  _b = vec_xor ((vui32_t) vrb, signbit);
  return vec_cmpgtuq ((vui128_t) _a, (vui128_t) _b);
}

/** \brief Vector Compare Greater Than Unsigned Quadword.
 *
 *  Compare unsigned __int128 (128-bit) integers and return all '1's,
 *  if vra > vrb, otherwise all '0's.
 *
 *  For POWER8 (PowerISA 2.07B) or later, use the Vector Subtract &
 *  write Carry QuadWord (<B>vsubcuq</B>) instruction with the
 *  parameters reversed.  This generates a carry for less than or equal
 *  and NOT carry for greater than.
 *  Then use vec_setb_ncq ro convert the carry into a vector bool.
 *  Here we use the pveclib implementations (vec_subcuq() and
 *  vec_setb_ncq()), instead of <altivec.h> intrinsics,
 *  to address older compilers and POWER7.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8     |2/ 2cycles|
 *  |power9   | 6     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an unsigned __int128.
 *  @param vrb 128-bit vector treated as an unsigned __int128.
 *  @return 128-bit vector boolean reflecting vector unsigned __int128
 *  compare greater than.
 */
static inline vb128_t
vec_cmpgtuq (vui128_t vra, vui128_t vrb)
{
  vui128_t b_a;

  b_a = vec_subcuq (vrb, vra);
  return vec_setb_ncq (b_a);
}

/** \brief Vector Compare Less Than or Equal Signed Quadword.
 *
 *  Compare signed __int128 (128-bit) integers and return all '1's,
 *  if vra <= vrb, otherwise all '0's.
 *
 *  Flip the operand sign bits and use vec_cmpleuq for signed compare.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-16 |1/ 2cycles|
 *  |power9   | 8-14  | 1/cycle  |
 *
 *  @param vra 128-bit vector treated as an signed __int128.
 *  @param vrb 128-bit vector treated as an signed __int128.
 *  @return 128-bit vector boolean reflecting vector signed __int128
 *  compare less than or equal.
 */
static inline vb128_t
vec_cmplesq (vi128_t vra, vi128_t vrb)
{
  const vui32_t signbit = CONST_VINT128_W (0x80000000, 0, 0, 0);
  vui32_t _a, _b;

  _a = vec_xor ((vui32_t) vra, signbit);
  _b = vec_xor ((vui32_t) vrb, signbit);
  return vec_cmpleuq ((vui128_t) _a, (vui128_t) _b);
}

/** \brief Vector Compare Less Than or Equal Unsigned Quadword.
 *
 *  Compare unsigned __int128 (128-bit) integers and return all '1's,
 *  if vra <= vrb, otherwise all '0's.
 *
 *  For POWER8 (PowerISA 2.07B) or later, use the Vector Subtract &
 *  write Carry QuadWord (<B>vsubcuq</B>) instruction.
 *  This generates a carry for greater than or equal
 *  and NOT carry for less than.
 *  Then use vec_setb_ncq ro convert the carry into a vector bool.
 *  Here we use the pveclib implementations (vec_subcuq() and
 *  vec_setb_cyq()), instead of <altivec.h> intrinsics,
 *  to address older compilers and POWER7.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8     |2/ 2cycles|
 *  |power9   | 6     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an unsigned __int128.
 *  @param vrb 128-bit vector treated as an unsigned __int128.
 *  @return 128-bit vector boolean reflecting vector unsigned __int128
 *  compare less than or equal.
 */
static inline vb128_t
vec_cmpleuq (vui128_t vra, vui128_t vrb)
{
  vui128_t b_a;

  b_a = vec_subcuq (vrb, vra);
  return vec_setb_cyq (b_a);
}


/** \brief Vector Compare Less Than Signed Quadword.
 *
 *  Compare signed __int128 (128-bit) integers and return all '1's,
 *  if vra < vrb, otherwise all '0's.
 *
 *  Flip the operand sign bits and use vec_cmpltuq for signed compare.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-16 |1/ 2cycles|
 *  |power9   | 8-14  | 1/cycle  |
 *
 *  @param vra 128-bit vector treated as an signed __int128.
 *  @param vrb 128-bit vector treated as an signed __int128.
 *  @return 128-bit vector boolean reflecting vector unsigned __int128
 *  compare less than.
 */
static inline vb128_t
vec_cmpltsq (vi128_t vra, vi128_t vrb)
{
  const vui32_t signbit = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vui32_t _a, _b;

  _a = vec_xor ((vui32_t) vra, signbit);
  _b = vec_xor ((vui32_t) vrb, signbit);
  return vec_cmpltuq ((vui128_t) _a, (vui128_t) _b);
}

/** \brief Vector Compare Less Than Unsigned Quadword.
 *
 *  Compare unsigned __int128 (128-bit) integers and return all '1's,
 *  if vra < vrb, otherwise all '0's.
 *
 *  For POWER8 (PowerISA 2.07B) or later, use the Vector Subtract &
 *  write Carry QuadWord (<B>vsubcuq</B>) instruction.
 *  This generates a carry for greater than or equal
 *  and NOT carry for less than.
 *  Then use vec_setb_ncq ro convert the carry into a vector bool.
 *  Here we use the pveclib implementations (vec_subcuq() and
 *  vec_setb_ncq()), instead of <altivec.h> intrinsics,
 *  to address older compilers and POWER7.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8     |2/ 2cycles|
 *  |power9   | 6     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an unsigned __int128.
 *  @param vrb 128-bit vector treated as an unsigned __int128.
 *  @return 128-bit vector boolean reflecting vector unsigned __int128
 *  compare less than.
 */
static inline vb128_t
vec_cmpltuq (vui128_t vra, vui128_t vrb)
{
  vui128_t  a_b;

  a_b = vec_subcuq (vra, vrb);
  return vec_setb_ncq (a_b);
}

/** \brief Vector Compare Equal Signed Quadword.
 *
 *  Compare signed __int128 (128-bit) integers and return all '1's,
 *  if vra != vrb, otherwise all '0's.  We use
 *  vec_cmpequq as it works for both signed and unsigned compares.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6     | 2/cycle  |
 *  |power9   | 7     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an signed __int128.
 *  @param vrb 128-bit vector treated as an signed __int128.
 *  @return 128-bit vector boolean reflecting vector signed __int128
 *  compare not equal.
 */
static inline vb128_t
vec_cmpnesq (vi128_t vra, vi128_t vrb)
{
  /* vec_cmpneuq works for both signed and unsigned compares.  */
  return vec_cmpneuq ((vui128_t) vra, (vui128_t) vrb);
}

/** \brief Vector Compare Not Equal Unsigned Quadword.
 *
 *  Compare unsigned __int128 (128-bit) integers and return all '1's,
 *  if vra != vrb, otherwise all '0's.
 *
 *  For POWER8 (PowerISA 2.07B) or later, use the Vector Compare
 *  Equal Unsigned DoubleWord (<B>vcmpequd</B>) instruction.
 *  To get the correct quadword result, the doubleword element equal
 *  truth values are swapped, then <I>not anded</I> with the
 *  original compare results.
 *  Otherwise use vector word compare and additional boolean logic to
 *  insure all word elements are equal.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6     | 2/cycle  |
 *  |power9   | 7     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an unsigned __int128.
 *  @param vrb 128-bit vector treated as an unsigned __int128.
 *  @return 128-bit vector boolean reflecting vector unsigned __int128
 *  compare equal.
 */
static inline vb128_t
vec_cmpneuq (vui128_t vra, vui128_t vrb)
{
#ifdef _ARCH_PWR8
  __vector unsigned long equd, swapd;

  equd = (vui64_t) vec_cmpequd ((vui64_t) vra,
      (vui64_t) vrb);
  swapd = vec_swapd (equd);
  return (vb128_t) vec_nand (equd, swapd);
#else
  vui32_t equw, equ1, equ2, equ3;

  equw = (vui32_t) vec_cmpeq ((vui32_t) vra, (vui32_t) vrb);
  equ1 = vec_sld (equw, equw, 4);
  equ2 = vec_sld (equw, equw, 8);
  equ3 = vec_sld (equw, equw, 12);
  equw = vec_and (equw, equ1);
  equ2 = vec_and (equ2, equ3);
  /* POWER7 does not have vnand nor xxlnand, so requires an extra vnor
     after the final vand.  */
  equw = vec_and (equw, equ2);
  return (vb128_t) vec_nor (equw, equw);
#endif
}

/** \brief Vector Compare all Equal Signed Quadword.
 *
 *  Compare vector signed __int128 values and return true if
 *  vra and vrb are equal.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector signed
 *  __int128 (qword) element.
 *  @param vrb 128-bit vector treated as an vector signed
 *  __int128 (qword) element.
 *  @return boolean int for all 128-bits, true if equal,
 *  false otherwise.
 */
static inline
int
vec_cmpsq_all_eq (vi128_t vra, vi128_t vrb)
{
  int result;
#if defined (_ARCH_PWR8) && (__GNUC__ >= 6)
  result = vec_all_eq((vui64_t)vra, (vui64_t)vrb);
#else
  result = vec_all_eq((vui32_t)vra, (vui32_t)vrb);
#endif
  return (result);
}

/** \brief Vector Compare any Greater Than or Equal Signed Quadword.
 *
 *  Compare vector unsigned __int128 values and return true if
 *  vra >= vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-15 |1/ 2cycles|
 *  |power9   | 8     | 1/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector signed
 *  __int128 (qword) element.
 *  @param vrb 128-bit vector treated as an vector signed
 *  __int128 (qword) element.
 *  @return boolean int for all 128-bits, true if Greater Than or Equal,
 *  false otherwise.
 */
static inline int
vec_cmpsq_all_ge (vi128_t vra, vi128_t vrb)
{
  const vui32_t carry128 = CONST_VINT128_W (0, 0, 0, 1);
  const vui32_t signbit = CONST_VINT128_W (0x80000000, 0, 0, 0);
  vui128_t a_b, _a, _b;

  _a = (vui128_t) vec_xor ((vui32_t) vra, signbit);
  _b = (vui128_t) vec_xor ((vui32_t) vrb, signbit);

  a_b = vec_subcuq (_a, _b);
  return vec_all_eq((vui32_t)a_b, carry128);
}

/** \brief Vector Compare any Greater Than Signed Quadword.
 *
 *  Compare vector signed __int128 values and return true if
 *  vra > vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-15 |1/ 2cycles|
 *  |power9   | 8     | 1/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector signed
 *  __int128 (qword) element.
 *  @param vrb 128-bit vector treated as an vector signed
 *  __int128 (qword) element.
 *  @return boolean int for all 128-bits, true if Greater Than,
 *  false otherwise.
 */
static inline int
vec_cmpsq_all_gt (vi128_t vra, vi128_t vrb)
{
  const vui32_t ncarry128 = CONST_VINT128_W (0, 0, 0, 0);
  const vui32_t signbit = CONST_VINT128_W (0x80000000, 0, 0, 0);
  vui128_t b_a, _a, _b;

  _a = (vui128_t) vec_xor ((vui32_t) vra, signbit);
  _b = (vui128_t) vec_xor ((vui32_t) vrb, signbit);

  b_a = vec_subcuq (_b, _a);
  return vec_all_eq((vui32_t)b_a, ncarry128);
}

/** \brief Vector Compare any Less Than or Equal Signed Quadword.
 *
 *  Compare vector signed __int128 values and return true if
 *  vra <= vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-15 |1/ 2cycles|
 *  |power9   | 8     | 1/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector signed
 *  __int128 (qword) element.
 *  @param vrb 128-bit vector treated as an vector signed
 *  __int128 (qword) element.
 *  @return boolean int for all 128-bits, true if Less Than or Equal,
 *  false otherwise.
 */
static inline int
vec_cmpsq_all_le (vi128_t vra, vi128_t vrb)
{
  const vui32_t carry128 = CONST_VINT128_W (0, 0, 0, 1);
  const vui32_t signbit = CONST_VINT128_W (0x80000000, 0, 0, 0);
  vui128_t b_a, _a, _b;

  _a = (vui128_t) vec_xor ((vui32_t) vra, signbit);
  _b = (vui128_t) vec_xor ((vui32_t) vrb, signbit);

  b_a = vec_subcuq (_b, _a);
  return vec_all_eq((vui32_t)b_a, carry128);
}

/** \brief Vector Compare any Less Than Signed Quadword.
 *
 *  Compare vector signed __int128 values and return true if
 *  vra < vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-15 |1/ 2cycles|
 *  |power9   | 8     | 1/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector signed
 *  __int128 (qword) element.
 *  @param vrb 128-bit vector treated as an vector signed
 *  __int128 (qword) element.
 *  @return boolean int for all 128-bits, true if Less Than,
 *  false otherwise.
 */
static inline int
vec_cmpsq_all_lt (vi128_t vra, vi128_t vrb)
{
  const vui32_t ncarry128 = CONST_VINT128_W (0, 0, 0, 0);
  const vui32_t signbit = CONST_VINT128_W (0x80000000, 0, 0, 0);
  vui128_t a_b, _a, _b;

  _a = (vui128_t) vec_xor ((vui32_t) vra, signbit);
  _b = (vui128_t) vec_xor ((vui32_t) vrb, signbit);

  a_b = vec_subcuq (_a, _b);
  return vec_all_eq((vui32_t)a_b, ncarry128);
}

/** \brief Vector Compare all Not Equal Signed Quadword.
 *
 *  Compare vector signed __int128 values and return true if
 *  vra and vrb are not equal.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector signed
 *  __int128 (qword) element.
 *  @param vrb 128-bit vector treated as an vector signed
 *  __int128 (qword) element.
 *  @return boolean __int128 for all 128-bits, true if equal,
 *  false otherwise.
 */
static inline
int
vec_cmpsq_all_ne (vi128_t vra, vi128_t vrb)
{
  int result;
#if defined (_ARCH_PWR8) && (__GNUC__ >= 6)
  result = !vec_all_eq ((vui64_t) vra, (vui64_t) vrb);
#else
  result = !vec_all_eq ((vui32_t) vra, (vui32_t) vrb);
#endif
  return (result);
}

/** \brief Vector Compare all Equal Unsigned Quadword.
 *
 *  Compare vector unsigned __int128 values and return true if
 *  vra and vrb are equal.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector unsigned
 *  __int128 (qword) element.
 *  @param vrb 128-bit vector treated as an vector unsigned
 *  __int128 (qword) element.
 *  @return boolean int for all 128-bits, true if equal,
 *  false otherwise.
 */
static inline
int
vec_cmpuq_all_eq (vui128_t vra, vui128_t vrb)
{
  int result;
#if defined (_ARCH_PWR8) && (__GNUC__ >= 6)
  result = vec_all_eq ((vui64_t) vra, (vui64_t) vrb);
#else
  result = vec_all_eq ((vui32_t) vra, (vui32_t) vrb);
#endif
  return (result);
}

/** \brief Vector Compare any Greater Than or Equal Unsigned Quadword.
 *
 *  Compare vector unsigned __int128 values and return true if
 *  vra >= vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8-13  |2/ 2cycles|
 *  |power9   | 6     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector unsigned
 *  __int128 (qword) element.
 *  @param vrb 128-bit vector treated as an vector unsigned
 *  __int128 (qword) element.
 *  @return boolean int for all 128-bits, true if Greater Than or Equal,
 *  false otherwise.
 */
static inline int
vec_cmpuq_all_ge (vui128_t vra, vui128_t vrb)
{
  const vui32_t carry128 = CONST_VINT128_W (0, 0, 0, 1);
  vui128_t a_b;

  a_b = vec_subcuq (vra, vrb);
  return vec_all_eq ((vui32_t) a_b, carry128);
}

/** \brief Vector Compare any Greater Than Unsigned Quadword.
 *
 *  Compare vector unsigned __int128 values and return true if
 *  vra > vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8-13  |2/ 2cycles|
 *  |power9   | 6     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector unsigned
 *  __int128 (qword) element.
 *  @param vrb 128-bit vector treated as an vector unsigned
 *  __int128 (qword) element.
 *  @return boolean int for all 128-bits, true if Greater Than,
 *  false otherwise.
 */
static inline int
vec_cmpuq_all_gt (vui128_t vra, vui128_t vrb)
{
  const vui32_t ncarry128 = CONST_VINT128_W (0, 0, 0, 0);
  vui128_t b_a;

  b_a = vec_subcuq (vrb, vra);
  return vec_all_eq ((vui32_t) b_a, ncarry128);
}

/** \brief Vector Compare any Less Than or Equal Unsigned Quadword.
 *
 *  Compare vector unsigned __int128 values and return true if
 *  vra <= vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8-13  |2/ 2cycles|
 *  |power9   | 6     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector unsigned
 *  __int128 (qword) element.
 *  @param vrb 128-bit vector treated as an vector unsigned
 *  __int128 (qword) element.
 *  @return boolean int for all 128-bits, true if Less Than or Equal,
 *  false otherwise.
 */
static inline int
vec_cmpuq_all_le (vui128_t vra, vui128_t vrb)
{
  const vui32_t carry128 = CONST_VINT128_W (0, 0, 0, 1);
  vui128_t b_a;

  b_a = vec_subcuq (vrb, vra);
  return vec_all_eq ((vui32_t) b_a, carry128);
}

/** \brief Vector Compare any Less Than Unsigned Quadword.
 *
 *  Compare vector unsigned __int128 values and return true if
 *  vra < vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 8-13  |2/ 2cycles|
 *  |power9   | 6     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector unsigned
 *  __int128 (qword) element.
 *  @param vrb 128-bit vector treated as an vector unsigned
 *  __int128 (qword) element.
 *  @return boolean int for all 128-bits, true if Less Than,
 *  false otherwise.
 */
static inline int
vec_cmpuq_all_lt (vui128_t vra, vui128_t vrb)
{
  const vui32_t ncarry128 = CONST_VINT128_W (0, 0, 0, 0);
  vui128_t  a_b;

  a_b = vec_subcuq (vra, vrb);
  return vec_all_eq ((vui32_t) a_b, ncarry128);
}

/** \brief Vector Compare all Not Equal Unsigned Quadword.
 *
 *  Compare vector unsigned __int128 values and return true if
 *  vra and vrb are not equal.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-9   | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as an vector unsigned
 *  __int128 (qword) element.
 *  @param vrb 128-bit vector treated as an vector unsigned
 *  __int128 (qword) element.
 *  @return boolean __int128 for all 128-bits, true if equal,
 *  false otherwise.
 */
static inline
int
vec_cmpuq_all_ne (vui128_t vra, vui128_t vrb)
{
  int result;
#if defined (_ARCH_PWR8) && (__GNUC__ >= 6)
  result = !vec_all_eq ((vui64_t) vra, (vui64_t) vrb);
#else
  result = !vec_all_eq ((vui32_t) vra, (vui32_t) vrb);
#endif
  return (result);
}

/** \brief Vector combined Multiply by 10 Extended & write Carry Unsigned Quadword.
 *
 *  Compute the product of a 128 bit value a * 10 + digit(cin).
 *  Only the low order 128 bits of the extended product are returned.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 13-15 | 1/cycle  |
 *  |power9   | 3     |1/ 2cycles|
 *
 *  @param *cout pointer to upper 128-bits of the product.
 *  @param a 128-bit vector treated as a unsigned __int128.
 *  @param cin values 0-9 in bits 124:127 of a vector.
 *  @return vector __int128 (upper 128-bits of the 256-bit product) a * 10.
 */
static inline vui128_t
vec_cmul10ecuq (vui128_t *cout, vui128_t a, vui128_t cin)
{
  vui32_t t;
  vui32_t t_carry;
#ifdef _ARCH_PWR9
  __asm__(
      "vmul10ecuq %0,%2,%3;\n"
      "vmul10euq %1,%2,%3;\n"
      : "=&v" (t_carry),
      "=v" (t)
      : "v" (a),
      "v" (cin)
      : );
#else
  vui16_t ts = (vui16_t) a;
  vui32_t tc;
  vui16_t t10;
  vui32_t t_odd, t_even, t_high;
  vui32_t z = { 0, 0, 0, 0 };
  t10 = vec_splat_u16(10);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = vec_vmulouh (ts, t10);
  t_odd = vec_vmuleuh (ts, t10);
#else
  t_even = vec_vmuleuh (ts, t10);
  t_odd = vec_vmulouh (ts, t10);
#endif
  /* Shift t_even left 16-bits (right 112-bits) for the partial carry.  */
  t_high = vec_sld (z, t_even, 2);
  /* Shift cin left 112 bits.  */
  tc = vec_sld ((vui32_t) cin, z, 14);
  /* Shift t_even left 16 bits, merging the carry into the low bits.  */
  t_even = vec_sld (t_even, tc, 2);
  /* then add the even/odd sub-products to generate the final product */
#ifdef _ARCH_PWR8
  /* Any compiler that supports ARCH_PWR8 should support these builtins.  */
  t_carry = t_high; /* there is not carry into high */
  t = (vui32_t) vec_vadduqm ((vui128_t) t_even, (vui128_t) t_odd);
#else
  t_carry = t_high; /* there is no carry into high */
  /* Use pveclib adduqm implementation for pre _ARCH_PWR8.  */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);
#endif
#endif
  *cout = (vui128_t) t_carry;
  return ((vui128_t) t);
}

/** \brief Vector combined Multiply by 10 & write Carry Unsigned Quadword.
 *
 *  compute the product of a 128 bit values a * 10.
 *  Only the low order 128 bits of the product are returned.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 13-15 | 1/cycle  |
 *  |power9   | 3     |1/ 2cycles|
 *
 *  @param *cout pointer to upper 128-bits of the product.
 *  @param a 128-bit vector treated as a unsigned __int128.
 *  @return vector __int128 (lower 128-bits of the 256-bit product) a * 10.
 */
static inline vui128_t
vec_cmul10cuq (vui128_t *cout, vui128_t a)
{
  vui32_t t;
  vui32_t t_carry;
#ifdef _ARCH_PWR9
  __asm__(
      "vmul10cuq %0,%2;\n"
      "vmul10uq %1,%2;\n"
      : "=&v" (t_carry),
      "=v" (t)
      : "v" (a)
      : );
#else
  vui16_t ts = (vui16_t) a;
  vui16_t t10;
  vui32_t t_odd, t_even, t_high;
  vui32_t z = { 0, 0, 0, 0 };
  t10 = vec_splat_u16(10);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = vec_vmulouh (ts, t10);
  t_odd = vec_vmuleuh (ts, t10);
#else
  t_even = vec_vmuleuh(ts, t10);
  t_odd = vec_vmulouh(ts, t10);
#endif
  /* Shift t_even left 16-bits (right 112-bits) for the partial carry.  */
  t_high = vec_sld (z, t_even, 2);
  /* Shift t_even left 16 bits to align for lower 128-bits. */
  t_even = vec_sld (t_even, z, 2);
  /* then add the even/odd sub-products to generate the final product */
#ifdef _ARCH_PWR8
  /* Any compiler that supports ARCH_PWR8 should support these builtins.  */
  t_carry = t_high; /* there is no carry into high */
  t = (vui32_t) vec_vadduqm ((vui128_t) t_even, (vui128_t) t_odd);
#else
  t_carry = t_high; /* there is no carry into high */
  /* Use pveclib adduqm implementation for pre _ARCH_PWR8.  */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);
#endif
#endif
  *cout = (vui128_t) t_carry;
  return ((vui128_t) t);
}

/** \brief Vector Divide by const 10e31 Signed Quadword.
 *
 *  Compute the quotient of a 128 bit values vra / 10e31.
 *
 *  \note vec_divsq_10e31() and vec_modsq_10e31() can be used to
 *  prepare for <B>Decimal Convert From Signed Quadword</B>
 *  (See vec_bcdcfsq()), This guarantees that the conversion to
 *  Vector BCD does not overflow and the 39-digit extended result is
 *  obtained.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 18-60 | 1/cycle  |
 *  |power9   | 20-45 | 1/cycle  |
 *
 *  @param vra the dividend as a vector treated as a unsigned __int128.
 *  @return the quotient as vector unsigned __int128.
 */
static inline vi128_t
vec_divsq_10e31 (vi128_t vra)
{
  const vui128_t zero = (vui128_t) { (__int128) 0 };
  /* ten32  = +100000000000000000000000000000000UQ  */
  const vui128_t ten31 = (vui128_t)
	  { (__int128) 1000000000000000UL * (__int128) 10000000000000000UL };
  /* Magic numbers for multiplicative inverse to divide by 10**31
   are 4804950418589725908363185682083061167, corrective add,
   and shift right 107 bits.  */
  const vui128_t mul_invs_ten31 = (vui128_t) CONST_VINT128_DW(
      0x039d66589687f9e9UL, 0x01d59f290ee19dafUL);
  const int shift_ten31 = 103;
  vui128_t result, t, q, uvra;
  vb128_t negbool;

  negbool = vec_setb_sq (vra);
  uvra = (vui128_t) vec_sel ((vui32_t) vra,
  			    (vui32_t) vec_subuqm (zero, (vui128_t) vra),
  			    (vb32_t) negbool);

  if (vec_cmpuq_all_ge (uvra, ten31))
    {
      q = vec_mulhuq (uvra, mul_invs_ten31);
      // Need corrective add but want to avoid carry & double quad shift
      // The following avoids the carry and less instructions
      t = vec_subuqm (uvra, q);
      t = vec_srqi (t, 1);
      t = vec_adduqm (t, q);
      result = vec_srqi (t, shift_ten31-1);
      result = (vui128_t) vec_sel ((vui32_t) result,
      			    (vui32_t) vec_subuqm (zero, (vui128_t) result),
      			    (vb32_t) negbool);
    }
  else
    result = zero;

  return (vi128_t) result;
}

/** \brief Vector Divide Unsigned Double Quadword by const 10e31.
 *
 *  Compute the quotient of 256 bit value vra||vrb / 10e31.
 *
 *  \note vec_divudq_10e31() and vec_modudq_10e31() can be used
 *  to perform long division of a multi-quaqword binary value by
 *  the constant 10e31. The final remainder can be passed to
 *  <B>Decimal Convert From Signed Quadword</B> (See vec_bcdcfsq()).
 *  Long division is repeated on the resulting multi-quadword quotient
 *  to extract 31-digits for each step. This continues until the
 *  multi-quadword quotient is less than 10e31 which
 *  provides the highest order 31-digits of the of the multiple
 *  precision binary to BCD conversion.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 12-192| 1/cycle  |
 *  |power9   | 9-127 | 1/cycle  |
 *
 *  @param *qh the high quotient as a vector unsigned __int128.
 *  @param vra the high dividend as a vector unsigned __int128.
 *  @param vrb the low dividend as a vector unsigned __int128.
 *  @return the low quotient as vector unsigned __int128.
 */
static inline vui128_t
vec_divudq_10e31 (vui128_t *qh, vui128_t vra, vui128_t vrb)
{
  const vui128_t ten31 = (vui128_t)
	  { (__int128) 1000000000000000UL * (__int128) 10000000000000000UL };
  const vui128_t zero = (vui128_t) { (__int128) 0UL };
  /* Magic numbers for multiplicative inverse to divide by 10**31
   are 4804950418589725908363185682083061167, corrective add,
   and shift right 103 bits.  */
  const vui128_t mul_invs_ten31 = (vui128_t) CONST_VINT128_DW(
      0x039d66589687f9e9UL, 0x01d59f290ee19dafUL);
  const int shift_ten31 = 103;
  vui128_t result, r2, t, q, q1, q2, c;

  if (vec_cmpuq_all_ne (vra, zero) || vec_cmpuq_all_ge (vrb, ten31))
    {
      // Multiply high [vra||vrb] * mul_invs_ten31
      q = vec_mulhuq (vrb, mul_invs_ten31);
      q1 = vec_muludq (&t, vra, mul_invs_ten31);
      c = vec_addcuq (q1, q);
      q = vec_adduqm (q1, q);
      q1 = vec_adduqm (t, c);
      // corrective add [q2||q1||q] = [q1||q] + [vra||vrb]
      c = vec_addcuq (vrb, q);
      q = vec_adduqm (vrb, q);
      // q2 is the carry-out from the corrective add
      q2 = vec_addecuq (q1, vra, c);
      q1 = vec_addeuqm (q1, vra, c);
      // shift 384-bits (including the carry) right 107 bits
      // Using shift left double quadword shift by (128-107)-bits
      r2 = vec_sldqi (q2, q1, (128 - shift_ten31));
      result = vec_sldqi (q1, q, (128 - shift_ten31));
    }
  else
    {
      // Dividend less than divisor then return zero quotient
      r2 = zero;
      result = zero;
    }

  // return 256-bit quotient
  *qh = r2;
  return result;
}


/** \brief Vector Divide Unsigned Double Quadword by const 10e32.
 *
 *  Compute the quotient of 256 bit value vra||vrb / 10e32.
 *
 *  \note vec_divudq_10e32() and vec_modudq_10e32() can be used
 *  to perform long division of a multi-quaqword binary value by
 *  the constant 10e32. The final remainder can be passed to
 *  <B>Decimal Convert From Unsigned Quadword</B> (See vec_bcdcfuq()).
 *  Long division it repeated on the resulting multi-quadword quotient
 *  to extract 32-digits for each step. This continues until the
 *  multi-quadword quotient result is less than 10e32 which
 *  provides the highest order 32-digits of the of the multiple
 *  precision binary to BCD conversion.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 12-192| 1/cycle  |
 *  |power9   | 9-127 | 1/cycle  |
 *
 *  @param *qh the high quotient as a vector unsigned __int128.
 *  @param vra the high dividend as a vector unsigned __int128.
 *  @param vrb the low dividend as a vector unsigned __int128.
 *  @return the low quotient as vector unsigned __int128.
 */
static inline vui128_t
vec_divudq_10e32 (vui128_t *qh, vui128_t vra, vui128_t vrb)
{
  /* ten32  = +100000000000000000000000000000000UQ  */
  const vui128_t ten32 = (vui128_t)
	  { (__int128) 10000000000000000UL * (__int128) 10000000000000000UL };
  const vui128_t zero = (vui128_t) { (__int128) 0UL };
  /* Magic numbers for multiplicative inverse to divide by 10**32
   are 211857340822306639531405861550393824741, corrective add,
   and shift right 107 bits.  */
  const vui128_t mul_invs_ten32 = (vui128_t) CONST_VINT128_DW(
      0x9f623d5a8a732974UL, 0xcfbc31db4b0295e5UL);
  const int shift_ten32 = 107;
  vui128_t result, r2, t, q, q1, q2, c;

  if (vec_cmpuq_all_ne (vra, zero) || vec_cmpuq_all_ge (vrb, ten32))
    {
      // Multiply high [vra||vrb] * mul_invs_ten31
      q = vec_mulhuq (vrb, mul_invs_ten32);
      q1 = vec_muludq (&t, vra, mul_invs_ten32);
      c = vec_addcuq (q1, q);
      q = vec_adduqm (q1, q);
      q1 = vec_adduqm (t, c);
      // corrective add [q2||q1||q] = [q1||q] + [vra||vrb]
      c = vec_addcuq (vrb, q);
      q = vec_adduqm (vrb, q);
      // q2 is the carry-out from the corrective add
      q2 = vec_addecuq (q1, vra, c);
      q1 = vec_addeuqm (q1, vra, c);
      // shift 384-bits (including the carry) right 107 bits
      // Using shift left double quadword shift by (128-107)-bits
      r2 = vec_sldqi (q2, q1, (128 - shift_ten32));
      result = vec_sldqi (q1, q, (128 - shift_ten32));
    }
  else
    {
      // Dividend less than divisor then return zero quotient
      r2 = zero;
      result = zero;
    }

  // return 256-bit quotient
  *qh = r2;
  return result;
}

/** \brief Vector Divide by const 10e31 Unsigned Quadword.
 *
 *  Compute the quotient of a 128 bit values vra / 10e31.
 *
 *  \note vec_divuq_10e31() and vec_moduq_10e31() can be used to
 *  prepare for <B>Decimal Convert From Signed Quadword</B>
 *  (See vec_bcdcfsq()), This guarantees that the conversion to
 *  Vector BCD does not overflow and the 39-digit extended result is
 *  obtained.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  8-48 | 1/cycle  |
 *  |power9   |  9-31 | 1/cycle  |
 *
 *  @param vra the dividend as a vector treated as a unsigned __int128.
 *  @return the quotient as vector unsigned __int128.
 */
static inline vui128_t
vec_divuq_10e31 (vui128_t vra)
{
  /* ten32  = +100000000000000000000000000000000UQ  */
  const vui128_t ten31 = (vui128_t)
	  { (__int128) 1000000000000000UL * (__int128) 10000000000000000UL };
  /* Magic numbers for multiplicative inverse to divide by 10**31
   are 4804950418589725908363185682083061167, corrective add,
   and shift right 103 bits.  */
  const vui128_t mul_invs_ten31 = (vui128_t) CONST_VINT128_DW(
      0x039d66589687f9e9UL, 0x01d59f290ee19dafUL);
  const int shift_ten31 = 103;
  vui128_t result, t, q;

  if (vec_cmpuq_all_ge (vra, ten31))
    {
      q = vec_mulhuq (vra, mul_invs_ten31);
      // Need corrective add but want to avoid carry & double quad shift
      // The following avoids the carry and less instructions
      t = vec_subuqm (vra, q);
      t = vec_srqi (t, 1);
      t = vec_adduqm (t, q);
      result = vec_srqi (t, shift_ten31-1);
    }
  else
    result = (vui128_t) { (__int128) 0 };

  return result;
}

/** \brief Vector Divide by const 10e32 Unsigned Quadword.
 *
 *  Compute the quotient of a 128 bit values vra / 10e32.
 *
 *  \note vec_divuq_10e32() and vec_moduq_10e32() can be used to
 *  prepare for <B>Decimal Convert From Unsigned Quadword</B>
 *  (See vec_bcdcfuq()), This guarantees that the conversion to
 *  Vector BCD does not overflow and the 39-digit extended result is
 *  obtained.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  8-48 | 1/cycle  |
 *  |power9   |  9-31 | 1/cycle  |
 *
 *  @param vra the dividend as a vector treated as a unsigned __int128.
 *  @return the quotient as vector unsigned __int128.
 */
static inline vui128_t
vec_divuq_10e32 (vui128_t vra)
{
  /* ten32  = +100000000000000000000000000000000UQ  */
  const vui128_t ten32 = (vui128_t)
	  { (__int128) 10000000000000000UL * (__int128) 10000000000000000UL };
  /* Magic numbers for multiplicative inverse to divide by 10**32
   are 211857340822306639531405861550393824741, corrective add,
   and shift right 107 bits.  */
  const vui128_t mul_invs_ten32 = (vui128_t) CONST_VINT128_DW(
      0x9f623d5a8a732974UL, 0xcfbc31db4b0295e5UL);
  const int shift_ten32 = 107;
  vui128_t result, t, q;

  if (vec_cmpuq_all_ge (vra, ten32))
    {
      q = vec_mulhuq (vra, mul_invs_ten32);
      // Need corrective add but want to avoid carry & double quad shift
      // The following avoids the carry and less instructions
      t = vec_subuqm (vra, q);
      t = vec_srqi (t, 1);
      t = vec_adduqm (t, q);
      result = vec_srqi (t, shift_ten32-1);
    }
  else
    result = (vui128_t) { (__int128) 0 };

  return result;
}

/** \brief Vector Maximum Signed Quadword.
 *
 *  Compare Quadwords vra and vrb as
 *  signed integers and return the larger value in the result.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 12-18 | 2/cycle  |
 *  |power9   | 10-18 | 2/cycle  |
 *
 *  @param vra 128-bit vector __int128.
 *  @param vrb 128-bit vector __int128.
 *  @return vector __int128 maximum of a and b.
 */
static inline
vi128_t
vec_maxsq(vi128_t vra, vi128_t vrb)
{
  vb32_t maxmask;

  maxmask = (vb32_t) vec_cmpgtsq ( vra, vrb );
  return (vi128_t) vec_sel ((vui32_t) vrb, (vui32_t) vra, maxmask);
}

/** \brief Vector Maximum Unsigned Quadword.
 *
 *  Compare Quadwords vra and vrb as
 *  unsigned integers and return the larger value in the result.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  10   | 2/cycle  |
 *  |power9   |   8   | 2/cycle  |
 *
 *  @param vra 128-bit vector unsigned __int128.
 *  @param vrb 128-bit vector unsigned __int128.
 *  @return vector unsigned __int128 maximum of a and b.
 */
static inline
vui128_t
vec_maxuq(vui128_t vra, vui128_t vrb)
{
  vb32_t maxmask;

  maxmask = (vb32_t) vec_cmpgtuq ( vra, vrb );
  return (vui128_t) vec_sel ((vui32_t) vrb, (vui32_t) vra, maxmask);
}

/** \brief Vector Minimum Signed Quadword.
 *
 *  Compare Quadwords vra and vrb as
 *  signed integers and return the smaller value in the result.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 12-18 | 2/cycle  |
 *  |power9   | 10-18 | 2/cycle  |
 *
 *  @param vra 128-bit vector __int128.
 *  @param vrb 128-bit vector __int128.
 *  @return vector __int128 minimum of a and b.
 */
static inline
vi128_t
vec_minsq(vi128_t vra, vi128_t vrb)
{
  vb32_t minmask;

  minmask = (vb32_t) vec_cmpgtsq ( vrb, vra );
  return (vi128_t) vec_sel ((vui32_t) vrb, (vui32_t) vra, minmask);
}

/** \brief Vector Minimum Unsigned Quadword.
 *
 *  Compare Quadwords vra and vrb as
 *  unsigned integers and return the smaller value in the result.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  10   | 2/cycle  |
 *  |power9   |   8   | 2/cycle  |
 *
 *  @param vra 128-bit vector unsigned __int128 int.
 *  @param vrb 128-bit vector unsigned __int128 int.
 *  @return vector unsigned __int128 minimum of a and b.
 */
static inline
vui128_t
vec_minuq(vui128_t vra, vui128_t vrb)
{
  vb32_t minmask;

  minmask = (vb32_t) vec_cmpgtuq ( vrb, vra );
  return (vui128_t) vec_sel ((vui32_t) vrb, (vui32_t) vra, minmask);
}

/** \brief Vector Modulo by const 10e31 Signed Quadword.
 *
 *  Compute the remainder of a 128 bit values vra % 10e31.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  8-52 | 1/cycle  |
 *  |power9   |  9-23 | 2/cycle  |
 *
 *  @param vra the dividend as a vector treated as a signed __int128.
 *  @param q 128-bit signed __int128 containing the quotient from vec_divuq_10e31().
 *  @return the remainder as vector signed __int128.
 */
static inline vi128_t
vec_modsq_10e31 (vi128_t vra, vi128_t q)
{
  const vui128_t zero = (vui128_t) { (__int128) 0 };
  /* ten32  = +100000000000000000000000000000000UQ  */
  const vui128_t ten31 = (vui128_t)
	  { (__int128) 1000000000000000UL * (__int128) 10000000000000000UL };
  vui128_t result, t;

  // multiply low and subtract modulo are the same for signed/unsigned
  // But now easier to compare q for zero than signed compare to vra
  if (vec_cmpuq_all_ne ((vui128_t) vra, zero))
    {
      t = vec_mulluq ((vui128_t) q, ten31);
      result = vec_subuqm ((vui128_t) vra, (vui128_t) t);
    }
  else
    result = (vui128_t) vra;

  return (vi128_t) result;
}

/** \brief Vector Modulo Unsigned Double Quadword by const 10e31.
 *
 *  Compute the remainder (vra||vrb) - (ql * 10e31).
 *
 *  \note As we are using 128-bit multiplicative inverse for 128-bit
 *  integer in a 256-bit divide, so the quotient may not be exact
 *  (one bit off). So we check here if the remainder is too high
 *  (greater than 10e31) and correct both the remainder and quotient
 *  if needed.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 12-124| 1/cycle  |
 *  |power9   | 12-75 | 1/cycle  |
 *
 *  @param vra the high dividend as a vector unsigned __int128.
 *  @param vrb the low dividend as a vector unsigned __int128.
 *  @param *ql 128-bit unsigned __int128 containing the quotient from vec_divudq_10e31().
 *  @return the remainder as vector unsigned __int128.
 */
static inline vui128_t
vec_modudq_10e31 (vui128_t vra, vui128_t vrb, vui128_t *ql)
{
  /* ten31  = +100000000000000000000000000000000UQ  */
  const vui128_t ten31 = (vui128_t)
	  { (__int128) 1000000000000000UL * (__int128) 10000000000000000UL };
  const vui128_t zero = (vui128_t) { (__int128) 0UL };
  const vui128_t minus_one = (vui128_t) { (__int128) -1L };
  vui128_t result, t, th, c;

  if (vec_cmpuq_all_ne (vra, zero) || vec_cmpuq_all_ge (vrb, ten31))
    {
      t = vec_muludq (&th, *ql, ten31);
      c = vec_subcuq (vrb, t);
      t = vec_subuqm (vrb, t);
      th = vec_subeuqm (vra, th, c);
      // The remainder should be less than the divisor
      if (vec_cmpuq_all_ne (th, zero) && vec_cmpuq_all_ge (t, ten31))
	{
	  // If not the estimated quotient is off by 1
	  *ql = vec_adduqm (*ql, minus_one);
	  // And the remainder is negative, so add the divisor
	  t = vec_adduqm (t, ten31);
	}
      result = t;
    }
  else
    result = vrb;

  return result;
}


/** \brief Vector Modulo Unsigned Double Quadword by const 10e32.
 *
 *  Compute the remainder (vra||vrb) - (ql * 10e32).
 *
 *  \note As we are using 128-bit multiplicative inverse for 128-bit
 *  integer in a 256-bit divide, so the quotient may not be exact
 *  (one bit off). So we check here if the remainder is too high
 *  (greater than 10e32) and correct both the remainder and quotient
 *  if needed.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 12-124| 1/cycle  |
 *  |power9   | 12-75 | 1/cycle  |
 *
 *  @param vra the high dividend as a vector unsigned __int128.
 *  @param vrb the low dividend as a vector unsigned __int128.
 *  @param *ql 128-bit unsigned __int128 containing the quotient from vec_divudq_10e31().
 *  @return the remainder as vector unsigned __int128.
 */
static inline vui128_t
vec_modudq_10e32 (vui128_t vra, vui128_t vrb, vui128_t *ql)
{
  /* ten32  = +100000000000000000000000000000000UQ  */
  const vui128_t ten32 = (vui128_t)
	  { (__int128) 10000000000000000UL * (__int128) 10000000000000000UL };
  const vui128_t zero = (vui128_t) { (__int128) 0UL };
  const vui128_t minus_one = (vui128_t) { (__int128) -1L };
  vui128_t result, t, th, c;

  if (vec_cmpuq_all_ne (vra, zero) || vec_cmpuq_all_ge (vrb, ten32))
    {
      t = vec_muludq (&th, *ql, ten32);
      c = vec_subcuq (vrb, t);
      t = vec_subuqm (vrb, t);
      th = vec_subeuqm (vra, th, c);
      // The remainder should be less than the divisor
      if (vec_cmpuq_all_ne (th, zero) && vec_cmpuq_all_ge (t, ten32))
	{
	  // If not the estimated quotient is off by 1
	  *ql = vec_adduqm (*ql, minus_one);
	  // And the remainder is negative, so add the divisor
	  t = vec_adduqm (t, ten32);
	}
      result = t;
    }
  else
    result = vrb;

  return result;
}

/** \brief Vector Modulo by const 10e31 Unsigned Quadword.
 *
 *  Compute the remainder of a 128 bit values vra % 10e31.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  8-52 | 1/cycle  |
 *  |power9   |  9-23 | 2/cycle  |
 *
 *  @param vra the dividend as a vector treated as a unsigned __int128.
 *  @param q 128-bit unsigned __int128 containing the quotient from vec_divuq_10e31().
 *  @return the remainder as vector unsigned __int128.
 */
static inline vui128_t
vec_moduq_10e31 (vui128_t vra, vui128_t q)
{
  /* ten31  = +100000000000000000000000000000000UQ  */
  const vui128_t ten31 = (vui128_t)
	  { (__int128) 1000000000000000UL
         * (__int128) 10000000000000000UL };
  vui128_t result, t;

  if (vec_cmpuq_all_ge (vra, ten31))
    {
      t = vec_mulluq (q, ten31);
      result = vec_subuqm (vra, t);
    }
  else
    result = vra;

  return result;
}

/** \brief Vector Modulo by const 10e32 Unsigned Quadword.
 *
 *  Compute the remainder of a 128 bit values vra % 10e32.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  8-52 | 1/cycle  |
 *  |power9   |  9-23 | 2/cycle  |
 *
 *  @param vra the dividend as a vector treated as a unsigned __int128.
 *  @param q 128-bit unsigned __int128 containing the quotient from vec_divuq_10e32().
 *  @return the remainder as vector unsigned __int128.
 */
static inline vui128_t
vec_moduq_10e32 (vui128_t vra, vui128_t q)
{
  /* ten32  = +100000000000000000000000000000000UQ  */
  const vui128_t ten32 = (vui128_t)
	  { (__int128) 10000000000000000UL * (__int128) 10000000000000000UL };
  vui128_t result, t;

  if (vec_cmpuq_all_ge (vra, ten32))
    {
      t = vec_mulluq (q, ten32);
      result = vec_subuqm (vra, t);
    }
  else
    result = vra;

  return result;
}

/** \brief Vector Multiply by 10 & write Carry Unsigned Quadword.
 *
 *  compute the product of a 128 bit value a * 10.
 *  Only the high order 128 bits of the product are returned.
 *  This will be binary coded decimal value 0-9 in bits 124-127,
 *  Bits 0-123 will be '0'.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 13-15 | 1/cycle  |
 *  |power9   | 3     | 1/cycle  |
 *
 *  @param a 128-bit vector treated as a unsigned __int128.
 *  @return __int128 (upper 128-bits of the 256-bit product) a * 10 >> 128.
 */
static inline vui128_t
vec_mul10cuq (vui128_t a)
{
  vui32_t t_carry;
#ifdef _ARCH_PWR9
  __asm__(
      "vmul10cuq %0,%1;\n"
      : "=v" (t_carry)
      : "v" (a)
      : );
#else
  vui16_t ts = (vui16_t) a;
  vui16_t t10;
  vui32_t t_even, t_odd, t_high;
  vui32_t z = { 0, 0, 0, 0 };
  t10 = vec_splat_u16(10);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = vec_vmulouh (ts, t10);
  t_odd = vec_vmuleuh (ts, t10);
#else
  t_even = vec_vmuleuh(ts, t10);
  t_odd = vec_vmulouh(ts, t10);
#endif
  /* Shift t_even left 16-bits (right 112-bits) for the partial carry.  */
  t_high = vec_sld (z, t_even, 2);
  /* Shift t_even left 16 bits to align for lower 128-bits. */
  t_even = vec_sld (t_even, z, 2);
  /* then add the even/odd sub-products to generate the final product */
#ifdef _ARCH_PWR8
  /* Any compiler that supports ARCH_PWR8 should support these builtins.  */
  t_carry = (vui32_t) vec_vaddcuq ((vui128_t) t_even, (vui128_t) t_odd);
  t_carry = (vui32_t) vec_vadduqm ((vui128_t) t_carry, (vui128_t) t_high);
#else
  /* Use pveclib addcuq implementation for pre _ARCH_PWR8.  */
  t_carry = (vui32_t) vec_addcuq ((vui128_t) t_even, (vui128_t) t_odd);
  /* The final carry is small (0-9) so use word add, ignore carries.  */
  t_carry = vec_vadduwm (t_carry, t_high);
#endif
#endif
  return ((vui128_t) t_carry);
}

/** \brief Vector Multiply by 10 Extended & write Carry Unsigned Quadword.
 *
 *  Compute the product of a 128 bit value a * 10 + digit(cin).
 *  Only the low order 128 bits of the extended product are returned.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 15-17 | 1/cycle  |
 *  |power9   | 3     | 1/cycle  |
 *
 *  @param a 128-bit vector treated as unsigned __int128.
 *  @param cin values 0-9 in bits 124:127 of a vector.
 *  @return __int128 (upper 128-bits of the 256-bit product) a * 10 >> 128.
 */
static inline vui128_t
vec_mul10ecuq (vui128_t a, vui128_t cin)
{
//        vui32_t  t;
  vui32_t t_carry;
#ifdef _ARCH_PWR9
  __asm__(
      "vmul10ecuq %0,%1,%2;\n"
      : "=&v" (t_carry)
      : "v" (a),
      "v" (cin)
      : );
#else
  vui16_t ts = (vui16_t) a;
  vui32_t tc;
  vui16_t t10;
  vui32_t t_odd;
  vui32_t t_even, t_high;
  vui32_t z = { 0, 0, 0, 0 };
  t10 = vec_splat_u16(10);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = vec_vmulouh (ts, t10);
  t_odd = vec_vmuleuh (ts, t10);
#else
  t_even = vec_vmuleuh(ts, t10);
  t_odd = vec_vmulouh(ts, t10);
#endif
  /* Shift t_even left 16-bits (right 112-bits) for the partial carry.  */
  t_high = vec_sld (z, t_even, 2);
  /* Shift cin left 112 bits.  */
  tc = vec_sld ((vui32_t) cin, z, 14);
  /* Shift t_even left 16 bits, merging the carry into the low bits.  */
  t_even = vec_sld (t_even, tc, 2);
  /* then add the even/odd sub-products to generate the final product */
#ifdef _ARCH_PWR8
  /* Any compiler that supports ARCH_PWR8 should support these builtins.  */
  t_carry = (vui32_t) vec_vaddcuq ((vui128_t) t_even, (vui128_t) t_odd);
  t_carry = (vui32_t) vec_vadduqm ((vui128_t) t_carry, (vui128_t) t_high);
#else
  /* Use pveclib addcuq implementation for pre _ARCH_PWR8.  */
  t_carry = (vui32_t) vec_addcuq ((vui128_t) t_even, (vui128_t) t_odd);
  /* The final carry is small (0-9) so use word add, ignore carries.  */
  t_carry = vec_vadduwm (t_carry, t_high);
#endif
#endif
  return ((vui128_t) t_carry);
}

/** \brief Vector Multiply by 10 Extended Unsigned Quadword.
 *
 *  compute the product of a 128 bit value a * 10 + digit(cin).
 *  Only the low order 128 bits of the extended product are returned.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 13-15 | 1/cycle  |
 *  |power9   | 3     | 1/cycle  |
 *
 *  @param a 128-bit vector treated as unsigned __int128.
 *  @param cin values 0-9 in bits 124:127 of a vector.
 *  @return __int128 (lower 128-bits) a * 10.
 */
static inline vui128_t
vec_mul10euq (vui128_t a, vui128_t cin)
{
  vui32_t t;
#ifdef _ARCH_PWR9
  __asm__(
      "vmul10euq %0,%1,%2;\n"
      : "=v" (t)
      : "v" (a),
      "v" (cin)
      : );
#else
  vui16_t ts = (vui16_t) a;
  vui32_t tc;
  vui16_t t10;
  vui32_t t_odd, t_even;
  vui32_t z = { 0, 0, 0, 0 };
  t10 = vec_splat_u16(10);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = vec_vmulouh (ts, t10);
  t_odd = vec_vmuleuh (ts, t10);
#else
  t_even = vec_vmuleuh(ts, t10);
  t_odd = vec_vmulouh(ts, t10);
#endif
  /* Shift cin left 112 bits.  */
  tc = vec_sld ((vui32_t) cin, z, 14);
  /* Shift t_even left 16 bits, merging the carry into the low bits.  */
  t_even = vec_sld (t_even, tc, 2);
  /* then add the even/odd sub-products to generate the final product.  */
#ifdef _ARCH_PWR8
  t = (vui32_t) vec_vadduqm ((vui128_t) t_even, (vui128_t) t_odd);
#else
  /* Use pveclib addcuq implementation for pre _ARCH_PWR8.  */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);
#endif
#endif
  return ((vui128_t) t);
}

/** \brief Vector Multiply by 10 Unsigned Quadword.
 *
 *  compute the product of a 128 bit value a * 10.
 *  Only the low order 128 bits of the product are returned.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 13-15 | 1/cycle  |
 *  |power9   | 3     | 1/cycle  |
 *
 *  @param a 128-bit vector treated as unsigned __int128.
 *  @return __int128 (lower 128-bits) a * 10.
 */
static inline vui128_t
vec_mul10uq (vui128_t a)
{
  vui32_t t;
#ifdef _ARCH_PWR9
  __asm__(
      "vmul10uq %0,%1;\n"
      : "=v" (t)
      : "v" (a)
      : );
#else
  vui16_t ts = (vui16_t) a;
  vui16_t t10;
  vui32_t t_odd, t_even;
  vui32_t z = { 0, 0, 0, 0 };
  t10 = vec_splat_u16(10);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = vec_vmulouh (ts, t10);
  t_odd = vec_vmuleuh (ts, t10);
#else
  t_even = vec_vmuleuh(ts, t10);
  t_odd = vec_vmulouh(ts, t10);
#endif
  /* Shift t_even left 16 bits */
  t_even = vec_sld (t_even, z, 2);
  /* then add the even/odd sub-products to generate the final product */
#ifdef _ARCH_PWR8
  t = (vui32_t) vec_vadduqm ((vui128_t) t_even, (vui128_t) t_odd);
#else
  /* Use pveclib addcuq implementation for pre _ARCH_PWR8.  */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);
#endif
#endif
  return ((vui128_t) t);
}

/** \brief Vector combined Multiply by 100 & write Carry Unsigned Quadword.
 *
 *  compute the product of a 128 bit values a * 100.
 *  Only the low order 128 bits of the product are returned.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 13-15 | 1/cycle  |
 *  |power9   | 6     | 1/cycle  |
 *
 *  @param *cout pointer to upper 128-bits of the product.
 *  @param a 128-bit vector treated as unsigned __int128.
 *  @return vector __int128 (lower 128-bits of the 256-bit product) a * 100.
 */
static inline vui128_t
vec_cmul100cuq (vui128_t *cout, vui128_t a)
{
  vui32_t t;
  vui32_t t_carry;
#ifdef _ARCH_PWR9
  vui128_t t0, t1, tc0, tc1;
  /* Times 10 with 1st carry.  */
  tc0 = vec_mul10cuq (a);
  t0  = vec_mul10uq (a);
  /* Times 10 again with 2nd carry.  */
  tc1 = vec_mul10cuq (t0);
  t1  = vec_mul10uq (t0);
  /* 1st carry times 10 plus 2nd carry.  */
  t_carry  = (vui32_t) vec_mul10euq (tc0, tc1);
  t = (vui32_t)t1;
#else
  vui16_t ts = (vui16_t) a;
  vui16_t t100 = (vui16_t ) { 100, 100, 100, 100, 100, 100, 100, 100 };
  vui32_t t_odd, t_even, t_high;
  vui32_t z = { 0, 0, 0, 0 };
  //t100 = vec_splat_u16 (100);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = vec_vmulouh (ts, t100);
  t_odd = vec_vmuleuh (ts, t100);
#else
  t_even = vec_vmuleuh(ts, t100);
  t_odd = vec_vmulouh(ts, t100);
#endif
  /* Shift t_even left 16-bits (right 112-bits) for the partial carry.  */
  t_high = vec_sld (z, t_even, 2);
  /* Shift t_even left 16 bits to align for lower 128-bits. */
  t_even = vec_sld (t_even, z, 2);
  /* then add the even/odd sub-products to generate the final product */
#ifdef _ARCH_PWR8
  /* Any compiler that supports ARCH_PWR8 should support these builtins.  */
  t_carry = t_high; /* there is no carry into high */
  t = (vui32_t) vec_vadduqm ((vui128_t) t_even, (vui128_t) t_odd);
#else
  t_carry = t_high; /* there is no carry into high */
  /* Use pveclib adduqm implementation for pre _ARCH_PWR8.  */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);
#endif
#endif
  *cout = (vui128_t) t_carry;
  return ((vui128_t) t);
}

/** \brief Vector combined Multiply by 100 Extended & write Carry Unsigned Quadword.
 *
 *  Compute the product of a 128 bit value a * 100 + digit(cin).
 *  The function return its low order 128 bits of the extended product.
 *  The first parameter (*cout) it the address of the vector to receive
 *  the generated carry out in the range 0-99.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 15-17 | 1/cycle  |
 *  |power9   | 9     | 1/cycle  |
 *
 *  @param *cout pointer to upper 128-bits of the product.
 *  @param a 128-bit vector treated as unsigned __int128.
 *  @param cin values 0-99 in bits 120:127 of a vector.
 *  @return vector __int128 (lower 128-bits of the 256-bit product) a * 100.
 */
static inline vui128_t
vec_cmul100ecuq (vui128_t *cout, vui128_t a, vui128_t cin)
{
  vui32_t t;
  vui32_t t_carry;
#ifdef _ARCH_PWR9
  vui128_t t0, t1, tc0, tc1;
  /* Times 10 with 1st carry.  */
  tc0 = vec_mul10cuq (a);
  t0  = vec_mul10uq (a);
  /* Times 10 again with 2nd carry.  No carry in yet.  */
  tc1 = vec_mul10cuq (t0);
  t1  = vec_mul10uq (t0);
  /* 1st carry times 10 plus 2nd carry.  */
  t_carry  = (vui32_t) vec_mul10euq (tc0, tc1);
  /* Add cin to the low bits of a * 100.  If cin is in valid range
   * (0-99) then can not generate carry out of low 128-bits.  */
  t = (vui32_t) vec_vadduqm ((vui128_t) t1, cin);
#else
  vui16_t ts = (vui16_t) a;
  vui32_t tc;
  vui16_t t100 = (vui16_t ) { 100, 100, 100, 100, 100, 100, 100, 100 };
  vui32_t t_odd, t_even, t_high;
  vui32_t z = { 0, 0, 0, 0 };
  //t100 = vec_splat_u16 (100);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = vec_vmulouh (ts, t100);
  t_odd = vec_vmuleuh (ts, t100);
#else
  t_even = vec_vmuleuh (ts, t100);
  t_odd = vec_vmulouh (ts, t100);
#endif
  /* Shift t_even left 16-bits (right 112-bits) for the partial carry.  */
  t_high = vec_sld (z, t_even, 2);
  /* Shift cin left 112 bits.  */
  tc = vec_sld ((vui32_t) cin, z, 14);
  /* Shift t_even left 16 bits, merging the carry into the low bits.  */
  t_even = vec_sld (t_even, tc, 2);
  /* then add the even/odd sub-products to generate the final product */
#ifdef _ARCH_PWR8
  /* Any compiler that supports ARCH_PWR8 should support these builtins.  */
  t_carry = t_high; /* there is no carry into high */
  t = (vui32_t) vec_vadduqm ((vui128_t) t_even, (vui128_t) t_odd);
#else
  t_carry = t_high; /* there is no carry into high */
  /* Use pveclib adduqm implementation for pre _ARCH_PWR8.  */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);
#endif
#endif
  *cout = (vui128_t) t_carry;
  return ((vui128_t) t);
}

/** \brief Vector Multiply-Sum Unsigned Doubleword Modulo.
 *
 *  compute the even and odd produ256 bit product of two 128 bit values a, b.
 *  Only the low order 128 bits of the product are returned.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 30-32 | 1/cycle  |
 *  |power9   | 5-7   | 2/cycle  |
 *
 *  @param a 128-bit vector treated as __vector unsigned long int.
 *  @param b 128-bit vector treated as __vector unsigned long int.
 *  @param c 128-bit vector treated as __vector unsigned __int128.
 *  @return __vector unsigned Modulo Sum of the 128-bit even / odd
 *  products of operands a and b plus the unsigned __int128
 *  operand c.
 */
static inline vui128_t
vec_msumudm (vui64_t a, vui64_t b, vui128_t c)
{
  vui128_t res;
#ifdef _ARCH_PWR9
  __asm__(
      "vmsumudm %0,%1,%2,%3;\n"
      : "=v" (res)
      : "v" (a), "v" (b), "v" (c)
      : );
#else
  vui128_t p_even, p_odd, p_sum;

  p_even = vec_muleud (a, b);
  p_odd  = vec_muloud (a, b);
  p_sum  = vec_adduqm (p_even, p_odd);
  res    = vec_adduqm (p_sum, c);
#endif

  return (res);
}

/** \brief Vector Multiply Even Unsigned Doublewords.
 *
 *  Multiple the even 64-bit doublewords of two vector unsigned long
 *  values and return the unsigned __int128 product of the even
 *  doublewords.
 *
 *  \note The element numbering changes between big and little-endian.
 *  So the compiler and this implementation adjusts the generated code
 *  to reflect this.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 21-23 | 1/cycle  |
 *  |power9   | 8-13  | 2/cycle  |
 *
 *  @param a 128-bit vector unsigned long int.
 *  @param b 128-bit vector unsigned long int.
 *  @return vector unsigned __int128 product of the even double words
 *  of a and b.
 */
static inline vui128_t
vec_muleud (vui64_t a, vui64_t b)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return vec_vmuloud (a, b);
#else
  return vec_vmuleud (a, b);
#endif
}

/** \brief Vector Multiply High Unsigned Doubleword.
 *
 *  Multiple the corresponding doubleword elements of two vector
 *  unsigned long values and return the high order 64-bits, from each
 *  128-bit product.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 28-32 | 1/cycle  |
 *  |power9   | 11-16 | 1/cycle  |
 *
 *  \note This operation can be used to effectively perform a divide
 *  by multiplying by the scaled multiplicative inverse (reciprocal).
 *
 *  Warren, Henry S. Jr and <I>Hacker's Delight</I>, 2nd Edition,
 *  Addison Wesley, 2013. Chapter 10, Integer Division by Constants.
 *
 *  @param vra 128-bit vector unsigned long int.
 *  @param vrb 128-bit vector unsigned long int.
 *  @return vector unsigned long int of the high order 64-bits of the
 *  unsigned 128-bit product of the doubleword elements from vra
 *  and vrb.
 */
static inline vui64_t
vec_mulhud (vui64_t vra, vui64_t vrb)
{
  return vec_mrgahd (vec_vmuleud (vra, vrb), vec_vmuloud (vra, vrb));
}

/** \brief Vector Multiply Odd Unsigned Doublewords.
 *
 *  Multiple the odd 64-bit doublewords of two vector unsigned
 *  long values and return the unsigned __int128 product of the odd
 *  doublewords.
 *
 *  \note The element numbering changes between big and little-endian.
 *  So the compiler and this implementation adjusts the generated code
 *  to reflect this.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 21-23 | 1/cycle  |
 *  |power9   | 8-13  | 2/cycle  |
 *
 *  @param a 128-bit vector unsigned long int.
 *  @param b 128-bit vector unsigned long int.
 *  @return vector unsigned __int128 product of the odd double words
 *  of a and b.
 */
static inline vui128_t
vec_muloud (vui64_t a, vui64_t b)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return vec_vmuleud (a, b);
#else
  return vec_vmuloud (a, b);
#endif
}

/** \brief Vector Multiply Unsigned Doubleword Modulo.
 *
 *  Multiple the corresponding doubleword elements of two vector
 *  unsigned long values and return the low order 64-bits of the
 *  128-bit product for each element.
 *
 *  \note vec_muludm can be used for unsigned or signed integers.
 *  It is the vector equivalent of Multiply Low Doubleword.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 19-28 | 1/cycle  |
 *  |power9   | 11-16 | 1/cycle  |
 *
 *
 *  @param vra 128-bit vector unsigned long int.
 *  @param vrb 128-bit vector unsigned long int.
 *  @return vector unsigned long int of the low order 64-bits of the
 *  unsigned 128-bit product of the doubleword elements from vra
 *  and vrb.
 */
static inline vui64_t
vec_muludm (vui64_t vra, vui64_t vrb)
{
#ifdef _ARCH_PWR9
  return vec_mrgald (vec_vmuleud (vra, vrb), vec_vmuloud (vra, vrb));
#else
#ifdef _ARCH_PWR8
  vui64_t s32 = { 32, 32 }; // shift / rotate amount.
  vui64_t z = { 0, 0 };
  vui64_t t2, t3, t4;
  vui32_t t1;

  t1 = (vui32_t) vec_vrld (vrb, s32);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  // Nullify the little endian transform
  t2 = vec_muleuw ((vui32_t)vra, (vui32_t)vrb);
#else
  t2 = vec_mulouw ((vui32_t)vra, (vui32_t)vrb);
#endif
  t3 = vec_vmsumuwm ((vui32_t)vra, t1, z);
  t4 = vec_vsld (t3, s32);
  return (vui64_t) vec_vaddudm (t4, t2);
#else
  return vec_mrgald (vec_vmuleud (vra, vrb), vec_vmuloud (vra, vrb));
#endif
#endif
}

/** \brief Vector Multiply High Unsigned Quadword.
 *
 *  compute the 256 bit product of two 128 bit values a, b.
 *  The high order 128 bits of the product are returned.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 56-64 | 1/cycle  |
 *  |power9   | 33-39 | 1/cycle  |
 *
 *  @param a 128-bit vector treated as unsigned __int128.
 *  @param b 128-bit vector treated as unsigned __int128.
 *  @return vector unsigned __int128 (upper 128-bits) of a * b.
 */
static inline vui128_t
vec_mulhuq (vui128_t a, vui128_t b)
{
  vui32_t t, tmq;
  /* compute the 256 bit product of two 128 bit values a, b.
   * The high 128 bits are accumulated in t and the low 128-bits
   * in tmq. The high 128-bits are the return value.
   */
#ifdef _ARCH_PWR9
  const vui64_t zero = { 0, 0 };
  vui64_t b_swap = vec_swapd ((vui64_t) b);
  vui128_t tmh, tab, tba, tb0, tc1, tc2;
  /* multiply the low 64-bits of a and b.  For PWR9 this is just
   * vmsumudm with conditioned inputs.  */
  tmq = (vui32_t) vec_vmuloud ((vui64_t) a, (vui64_t) b);
  /* compute the 2 middle partial projects.  Can't directly use
   * vmsumudm here because the sum of partial products can overflow.  */
  tab = vec_vmuloud ((vui64_t) a, b_swap);
  tba = vec_vmuleud ((vui64_t) a, b_swap);
  t   = (vui32_t) vec_adduqm (tab, tba);
  tc1 = vec_addcuq (tab, tba);
  tmh = (vui128_t) vec_mrgahd ((vui128_t) zero, (vui128_t) tmq);
  t   = (vui32_t ) vec_adduqm ((vui128_t) t, tmh);
  tc2 = vec_addcuq ((vui128_t) t, tmh);
  tc1 = (vui128_t) vec_vadduwm ((vui32_t) tc1, (vui32_t) tc2);
  /* result = t[l] || tmq[l].  */
  tmq = (vui32_t) vec_mrgald ((vui128_t) t, (vui128_t) tmq);
  /* we can use multiply sum here because the high product plus the
   * high sum of middle partial products can't overflow.  */
  t   = (vui32_t) vec_permdi ((vui64_t) tc1, (vui64_t) t, 2);
  tb0 = (vui128_t) vec_mrgahd ((vui128_t) b, (vui128_t) zero);
  /* sum = (a[h] * b[h]) + (a[l] * 0) + (tc1[l] || t[h]).  */
  t   = (vui32_t) vec_msumudm ((vui64_t) a, (vui64_t) tb0, (vui128_t) t);
#else
#ifdef _ARCH_PWR8
  vui32_t tsw;
  vui32_t tc;
  vui32_t t_odd, t_even;
  vui32_t z = { 0, 0, 0, 0 };
  /* We use Vector Multiply Even/Odd Unsigned Word to compute
   * the 128 x 32 partial (160-bit) product of vector a with a
   * word element of b. The (for each word of vector b) 4 X 160-bit
   * partial products are  summed to produce the full 256-bit product.
   * See the comment in vec_muludq for details.
   */

  tsw = vec_splat ((vui32_t) b, VEC_WE_3);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = (vui32_t) vec_mulouw ((vui32_t) a, tsw);
  t_odd = (vui32_t) vec_muleuw ((vui32_t) a, tsw);
#else
  t_even = (vui32_t) vec_muleuw ((vui32_t) a, tsw);
  t_odd = (vui32_t) vec_mulouw ((vui32_t) a, tsw);
#endif
  /* Rotate the low 32-bits (right) into tmq. This is actually
   * implemented as 96-bit (12-byte) shift left. */
  tmq = vec_sld (t_odd, z, 12);
  /* shift the low 128 bits of partial product right 32-bits */
  t_odd = vec_sld (z, t_odd, 12);
  /* add the high 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui32_t) b, VEC_WE_2);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = (vui32_t) vec_mulouw ((vui32_t) a, tsw);
  t_odd = (vui32_t) vec_muleuw ((vui32_t) a, tsw);
#else
  t_even = (vui32_t) vec_muleuw ((vui32_t) a, tsw);
  t_odd = (vui32_t) vec_mulouw ((vui32_t) a, tsw);
#endif
  /* Sum the low 128 bits of odd previous partial products */
  tc = (vui32_t) vec_addcuq ((vui128_t) t_odd, (vui128_t) t);
  t_odd = (vui32_t) vec_adduqm ((vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 32-bits into tmq */
  tmq = vec_sld (t_odd, tmq, 12);
  /* shift the low 128 bits (with carry) of partial product right
   * 32-bits */
  t_odd = vec_sld (tc, t_odd, 12);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui32_t) b, VEC_WE_1);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = (vui32_t) vec_mulouw ((vui32_t) a, tsw);
  t_odd = (vui32_t) vec_muleuw ((vui32_t) a, tsw);
#else
  t_even = (vui32_t) vec_muleuw ((vui32_t) a, tsw);
  t_odd = (vui32_t) vec_mulouw ((vui32_t) a, tsw);
#endif
  /* Sum the low 128 bits of odd previous partial products */
  tc = (vui32_t) vec_addcuq ((vui128_t) t_odd, (vui128_t) t);
  t_odd = (vui32_t) vec_adduqm ((vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 32-bits into tmq */
  tmq = vec_sld (t_odd, tmq, 12);
  /* shift the low 129 bits (with carry) of partial product right
   * 32-bits */
  t_odd = vec_sld (tc, t_odd, 12);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui32_t) b, VEC_WE_0);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = (vui32_t) vec_mulouw ((vui32_t) a, tsw);
  t_odd = (vui32_t) vec_muleuw ((vui32_t) a, tsw);
#else
  t_even = (vui32_t) vec_muleuw ((vui32_t) a, tsw);
  t_odd = (vui32_t) vec_mulouw ((vui32_t) a, tsw);
#endif
  /* Sum the low 128 bits of odd previous partial products,
   * generate the carry.  */
  tc = (vui32_t) vec_addcuq ((vui128_t) t_odd, (vui128_t) t);
  t_odd = (vui32_t) vec_adduqm ((vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 32-bits into tmq */
  tmq = vec_sld (t_odd, tmq, 12);
  /* shift the low 129 bits (with carry) of partial product right
   * 32-bits */
  t_odd = vec_sld (tc, t_odd, 12);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);
#else // _ARCH_PWR7 or earlier and Big Endian only.  */

  /* We use Vector Multiply Even/Odd Unsigned Halfword to compute
   * the 128 x 16 partial (144-bit) product of vector a with a
   * halfword element of b. The (for each halfword of vector b)
   * 8 X 144-bit partial products are  summed to produce the full
   * 256-bit product. */
  vui16_t tsw;
  vui16_t tc;
  vui16_t t_odd, t_even;
  vui16_t z = { 0, 0, 0, 0, 0, 0, 0, 0 };

  tsw = vec_splat ((vui16_t) b, 7);
  t_even = (vui16_t) vec_vmuleuh ((vui16_t) a, tsw);
  t_odd = (vui16_t) vec_vmulouh ((vui16_t) a, tsw);

  /* Rotate the low 16-bits (right) into tmq. This is actually
   * implemented as 112-bit (14-byte) shift left. */
  tmq = (vui32_t) vec_sld (t_odd, z, 14);
  /* shift the low 128 bits of partial product right 16-bits */
  t_odd = vec_sld (z, t_odd, 14);
  /* add the high 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui16_t) b, 6);

  t_even = (vui16_t) vec_vmuleuh ((vui16_t) a, tsw);
  t_odd = (vui16_t) vec_vmulouh ((vui16_t) a, tsw);

  /* Sum the low 128 bits of odd previous partial products,
   * generate the carry.  */
  t_odd = (vui16_t) vec_addcq ((vui128_t*) &tc, (vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 16-bits into tmq */
  tmq = (vui32_t) vec_sld (t_odd, (vui16_t) tmq, 14);
  /* shift the low 128 bits (with carry) of partial product right
   * 16-bits */
  t_odd = vec_sld (tc, t_odd, 14);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui16_t) b, 5);

  t_even = (vui16_t) vec_vmuleuh ((vui16_t) a, tsw);
  t_odd = (vui16_t) vec_vmulouh ((vui16_t) a, tsw);

  /* Sum the low 128 bits of odd previous partial products,
   * generate the carry.  */
  t_odd = (vui16_t) vec_addcq ((vui128_t*) &tc, (vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 16-bits into tmq */
  tmq = (vui32_t) vec_sld (t_odd, (vui16_t) tmq, 14);
  /* shift the low 128 bits (with carry) of partial product right
   * 16-bits */
  t_odd = vec_sld (tc, t_odd, 14);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui16_t) b, 4);

  t_even = (vui16_t) vec_vmuleuh ((vui16_t) a, tsw);
  t_odd = (vui16_t) vec_vmulouh ((vui16_t) a, tsw);

  /* Sum the low 128 bits of odd previous partial products,
   * generate the carry.  */
  t_odd = (vui16_t) vec_addcq ((vui128_t*) &tc, (vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 16-bits into tmq */
  tmq = (vui32_t) vec_sld (t_odd, (vui16_t) tmq, 14);
  /* shift the low 128 bits (with carry) of partial product right
   * 16-bits */
  t_odd = vec_sld (tc, t_odd, 14);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui16_t) b, 3);

  t_even = (vui16_t) vec_vmuleuh ((vui16_t) a, tsw);
  t_odd = (vui16_t) vec_vmulouh ((vui16_t) a, tsw);

  /* Sum the low 128 bits of odd previous partial products,
   * generate the carry.  */
  t_odd = (vui16_t) vec_addcq ((vui128_t*) &tc, (vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 16-bits into tmq */
  tmq = (vui32_t) vec_sld (t_odd, (vui16_t) tmq, 14);
  /* shift the low 128 bits (with carry) of partial product right
   * 16-bits */
  t_odd = vec_sld (tc, t_odd, 14);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui16_t) b, 2);

  t_even = (vui16_t) vec_vmuleuh ((vui16_t) a, tsw);
  t_odd = (vui16_t) vec_vmulouh ((vui16_t) a, tsw);

  /* Sum the low 128 bits of odd previous partial products,
   * generate the carry.  */
  t_odd = (vui16_t) vec_addcq ((vui128_t*) &tc, (vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 16-bits into tmq */
  tmq = (vui32_t) vec_sld (t_odd, (vui16_t) tmq, 14);
  /* shift the low 128 bits (with carry) of partial product right
   * 16-bits */
  t_odd = vec_sld (tc, t_odd, 14);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui16_t) b, 1);

  t_even = (vui16_t) vec_vmuleuh ((vui16_t) a, tsw);
  t_odd = (vui16_t) vec_vmulouh ((vui16_t) a, tsw);

  /* Sum the low 128 bits of odd previous partial products,
   * generate the carry.  */
  t_odd = (vui16_t) vec_addcq ((vui128_t*) &tc, (vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 16-bits into tmq */
  tmq = (vui32_t) vec_sld (t_odd, (vui16_t) tmq, 14);
  /* shift the low 128 bits (with carry) of partial product right
   * 16-bits */
  t_odd = vec_sld (tc, t_odd, 14);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui16_t) b, 0);

  t_even = (vui16_t) vec_vmuleuh ((vui16_t) a, tsw);
  t_odd = (vui16_t) vec_vmulouh ((vui16_t) a, tsw);

  /* Sum the low 128 bits of odd previous partial products,
   * generate the carry.  */
  t_odd = (vui16_t) vec_addcq ((vui128_t*) &tc, (vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 16-bits into tmq */
  tmq = (vui32_t) vec_sld (t_odd, (vui16_t) tmq, 14);
  /* shift the low 128 bits (with carry) of partial product right
   * 16-bits */
  t_odd = vec_sld (tc, t_odd, 14);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);
#endif
#endif
  return ((vui128_t) t);
}

/** \brief Vector Multiply Low Unsigned Quadword.
 *
 *  compute the 256 bit product of two 128 bit values a, b.
 *  Only the low order 128 bits of the product are returned.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 42-48 | 1/cycle  |
 *  |power9   | 16-20 | 2/cycle  |
 *
 *  @param a 128-bit vector treated as unsigned __int128.
 *  @param b 128-bit vector treated as unsigned __int128.
 *  @return vector unsigned __int128 (lower 128-bits) a * b.
 */
static inline vui128_t
vec_mulluq (vui128_t a, vui128_t b)
{
  vui32_t t, tmq;
  /* compute the 256 bit product of two 128 bit values a, b.
   * The high 128 bits are accumulated in t and the low 128-bits
   * in tmq.  Only the low order 128 bits of the product are
   * returned.
   */
#ifdef _ARCH_PWR9
  const vui64_t zero = { 0, 0 };
  vui64_t b_swap = vec_swapd ((vui64_t) b);
  /* multiply the low 64-bits of a and b.  For PWR9 this is just
   * vmsumudm with conditioned inputs.  */
  tmq = (vui32_t) vec_vmuloud ((vui64_t) a, (vui64_t) b);
  /* we can use multiply sum here because we only need the low 64-bits
   * and don't care if we lose the carry / overflow.  */
  t   = (vui32_t) vec_mrgahd ((vui128_t) zero, (vui128_t) tmq);
  /* sum = (a[h] * b[l]) + (a[l] * b[h]) + (zero || tmq[h]).  */
  t   = (vui32_t) vec_msumudm ((vui64_t) a, b_swap, (vui128_t) t);
  /* result = t[l] || tmq[l].  */
  tmq = (vui32_t) vec_mrgald ((vui128_t) t, (vui128_t) tmq);
#else
#ifdef _ARCH_PWR8
  /* We use Vector Multiply Even/Odd Unsigned Word to compute
   * the 128 x 32 partial (160-bit) product of vector a with a
   * word element of b. The (for each word of vector b) 4 X 160-bit
   * partial products are  summed to produce the full 256-bit product.
   * See the comment in vec_muludq for details.
   */
  vui32_t tsw;
  vui32_t t_odd, t_even;
  vui32_t z = { 0, 0, 0, 0 };

  tsw = vec_splat ((vui32_t) b, VEC_WE_3);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = (vui32_t) vec_mulouw ((vui32_t) a, tsw);
  t_odd = (vui32_t) vec_muleuw ((vui32_t) a, tsw);
#else
  t_even = (vui32_t) vec_muleuw ((vui32_t) a, tsw);
  t_odd = (vui32_t) vec_mulouw ((vui32_t) a, tsw);
#endif
  /* Rotate the low 32-bits (right) into tmq. This is actually
   * implemented as 96-bit (12-byte) shift left. */
  tmq = vec_sld (t_odd, z, 12);
  /* shift the low 128 bits of partial product right 32-bits */
  t_odd = vec_sld (z, t_odd, 12);
  /* add the high 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui32_t) b, VEC_WE_2);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = (vui32_t) vec_mulouw ((vui32_t) a, tsw);
  t_odd = (vui32_t) vec_muleuw ((vui32_t) a, tsw);
#else
  t_even = (vui32_t) vec_muleuw ((vui32_t) a, tsw);
  t_odd = (vui32_t) vec_mulouw ((vui32_t) a, tsw);
#endif
  /* Sum the low 128 bits of odd previous partial products */
  t_odd = (vui32_t) vec_adduqm ((vui128_t) t_odd, (vui128_t) t);
  /* rotate right the low 32-bits into tmq */
  tmq = vec_sld (t_odd, tmq, 12);
  /* shift the low 128 bits of partial product right 32-bits */
  t_odd = vec_sld (z, t_odd, 12);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui32_t) b, VEC_WE_1);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = (vui32_t) vec_mulouw ((vui32_t) a, tsw);
  t_odd = (vui32_t) vec_muleuw ((vui32_t) a, tsw);
#else
  t_even = (vui32_t) vec_muleuw ((vui32_t) a, tsw);
  t_odd = (vui32_t) vec_mulouw ((vui32_t) a, tsw);
#endif
  /* add the low 128 bits of odd / previous partial products */
  t_odd = (vui32_t) vec_adduqm ((vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 32-bits into tmq */
  tmq = vec_sld (t_odd, tmq, 12);
  /* shift the low 128 bits of partial product right 32-bits */
  t_odd = vec_sld (z, t_odd, 12);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui32_t) b, VEC_WE_0);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = (vui32_t) vec_mulouw ((vui32_t) a, tsw);
  t_odd = (vui32_t) vec_muleuw ((vui32_t) a, tsw);
#else
  t_even = (vui32_t) vec_muleuw ((vui32_t) a, tsw);
  t_odd = (vui32_t) vec_mulouw ((vui32_t) a, tsw);
#endif
  /* add the low 128 bits of odd / previous partial products */
  t_odd = (vui32_t) vec_adduqm ((vui128_t) t_odd, (vui128_t) t);
  /* rotate right the low 32-bits into tmq */
  tmq = vec_sld (t_odd, tmq, 12);
  /* shift the low 128 bits of partial product right 32-bits */
  t_odd = vec_sld (z, t_odd, 12);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);
#else
//#warning implementation pre power8

  tmq = (vui32_t) vec_muludq ((vui128_t*) &t, a, b);
#endif
#endif
  return ((vui128_t) tmq);
}

/** \brief Vector Multiply Unsigned Double Quadword.
 *
 *  compute the 256 bit product of two 128 bit values a, b.
 *  The low order 128 bits of the product are returned, while
 *  the high order 128-bits are "stored" via the mulu pointer.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 56-64 | 1/cycle  |
 *  |power9   | 33-39 | 1/cycle  |
 *
 *  @param *mulu pointer to vector unsigned __int128 to receive the
 *  upper 128-bits of the product.
 *  @param a 128-bit vector treated as unsigned __int128.
 *  @param b 128-bit vector treated as unsigned __int128.
 *  @return vector unsigned __int128 (lower 128-bits) of a * b.
 */
static inline vui128_t
vec_muludq (vui128_t *mulu, vui128_t a, vui128_t b)
{
  vui32_t t, tmq;
  /* compute the 256 bit product of two 128 bit values a, b.
   * The high 128 bits are accumulated in t and the low 128-bits
   * in tmq. The high 128-bits of the product are returned to the
   * address of the 1st parm. The low 128-bits are the return
   * value.
   */
#ifdef _ARCH_PWR9
  const vui64_t zero = { 0, 0 };
  vui64_t b_swap = vec_swapd ((vui64_t) b);
  vui128_t tmh, tab, tba, tb0, tc1, tc2;
  /* multiply the low 64-bits of a and b.  For PWR9 this is just
   * vmsumudm with conditioned inputs.  */
  tmq = (vui32_t) vec_vmuloud ((vui64_t)a, (vui64_t)b);
  /* compute the 2 middle partial projects.  Can't directly use
   * vmsumudm here because the sum of partial products can overflow.  */
  tab = vec_vmuloud ((vui64_t) a, b_swap);
  tba = vec_vmuleud ((vui64_t) a, b_swap);
  t   = (vui32_t) vec_adduqm (tab, tba);
  tc1 = vec_addcuq (tab, tba);
  tmh = (vui128_t) vec_mrgahd ((vui128_t) zero, (vui128_t) tmq);
  t   = (vui32_t) vec_adduqm ((vui128_t) t, tmh);
  tc2 = vec_addcuq ((vui128_t) t, tmh);
  tc1 = (vui128_t) vec_vadduwm ((vui32_t) tc1, (vui32_t) tc2);
  /* result = t[l] || tmq[l].  */
  tmq = (vui32_t) vec_mrgald ((vui128_t) t, (vui128_t) tmq);
  /* we can use multiply sum here because the high product plus the
   * high sum of middle partial products can't overflow.  */
  t   = (vui32_t) vec_permdi ((vui64_t) tc1, (vui64_t) t, 2);
  tb0 = (vui128_t) vec_mrgahd ((vui128_t) b, (vui128_t) zero);
  /* sum = (a[h] * b[h]) + (a[l] * 0) + (tc1[l] || t[h]).  */
  t   = (vui32_t) vec_msumudm ((vui64_t) a, (vui64_t) tb0, (vui128_t) t);
#else
#ifdef _ARCH_PWR8
  vui32_t tsw;
  vui32_t tc;
  vui32_t t_odd, t_even;
  vui32_t z = { 0, 0, 0, 0 };
  /* We use the Vector Multiple Even/Odd Unsigned Word to compute
   * the 128 x 32 partial (160-bit) product of value a with the
   * word splat of b. This produces four 64-bit (32 x 32)
   * partial products in two vector registers. These results
   * are not aligned for summation as is. So the odd result is
   * shifted right 32-bits before it is summed (via Vector Add
   * Unsigned Quadword Modulo) with the the even result.
   * The low order 32-bits, of the 160-bit product
   * is shifted (right) in to a separate vector (tmq).
   *
   * This is repeated for each (low to high order) words of b.
   * After the first (160-bit) partial product, the high 128-bits
   * (t) of the previous partial product is summed with the current
   * odd multiply result, before this sum (including any carry out)
   * is shifted right 32-bits.  Bits shifted out of the of this sum
   * are shifted (32-bits at a time) into the low order 128-bits
   * of the product (tmq). The shifted odd sum is then added to the
   * current even product, After the 4th step this sum is the
   * final high order 128-bits of the quadword product. */
  tsw = vec_splat ((vui32_t) b, VEC_WE_3);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = (vui32_t) vec_mulouw ((vui32_t) a, tsw);
  t_odd = (vui32_t) vec_muleuw ((vui32_t) a, tsw);
#else
  t_even = (vui32_t)vec_muleuw((vui32_t)a, tsw);
  t_odd = (vui32_t)vec_mulouw((vui32_t)a, tsw);
#endif
  /* Rotate the low 32-bits (right) into tmq. This is actually
   * implemented as 96-bit (12-byte) shift left. */
  tmq = vec_sld (t_odd, z, 12);
  /* shift the low 128 bits of partial product right 32-bits */
  t_odd = vec_sld (z, t_odd, 12);
  /* add the high 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui32_t) b, VEC_WE_2);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = (vui32_t) vec_mulouw ((vui32_t) a, tsw);
  t_odd = (vui32_t) vec_muleuw ((vui32_t) a, tsw);
#else
  t_even = (vui32_t)vec_muleuw((vui32_t)a, tsw);
  t_odd = (vui32_t)vec_mulouw((vui32_t)a, tsw);
#endif
  /* Sum the low 128 bits of odd previous partial products */
  tc = (vui32_t) vec_addcuq ((vui128_t) t_odd, (vui128_t) t);
  t_odd = (vui32_t) vec_adduqm ((vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 32-bits into tmq */
  tmq = vec_sld (t_odd, tmq, 12);
  /* shift the low 128 bits (with carry) of partial product right
   * 32-bits */
  t_odd = vec_sld (tc, t_odd, 12);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui32_t) b, VEC_WE_1);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = (vui32_t) vec_mulouw ((vui32_t) a, tsw);
  t_odd = (vui32_t) vec_muleuw ((vui32_t) a, tsw);
#else
  t_even = (vui32_t)vec_muleuw((vui32_t)a, tsw);
  t_odd = (vui32_t)vec_mulouw((vui32_t)a, tsw);
#endif
  /* Sum the low 128 bits of odd previous partial products */
  tc = (vui32_t) vec_addcuq ((vui128_t) t_odd, (vui128_t) t);
  t_odd = (vui32_t) vec_adduqm ((vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 32-bits into tmq */
  tmq = vec_sld (t_odd, tmq, 12);
  /* shift the low 129 bits (with carry) of partial product right
   * 32-bits */
  t_odd = vec_sld (tc, t_odd, 12);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui32_t) b, VEC_WE_0);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  t_even = (vui32_t) vec_mulouw ((vui32_t) a, tsw);
  t_odd = (vui32_t) vec_muleuw ((vui32_t) a, tsw);
#else
  t_even = (vui32_t)vec_muleuw((vui32_t)a, tsw);
  t_odd = (vui32_t)vec_mulouw((vui32_t)a, tsw);
#endif
  /* Sum the low 128 bits of odd previous partial products,
   * generate the carry.  */
  tc = (vui32_t) vec_addcuq ((vui128_t) t_odd, (vui128_t) t);
  t_odd = (vui32_t) vec_adduqm ((vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 32-bits into tmq */
  tmq = vec_sld (t_odd, tmq, 12);
  /* shift the low 129 bits (with carry) of partial product right
   * 32-bits */
  t_odd = vec_sld (tc, t_odd, 12);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);
#else // _ARCH_PWR7 or earlier and Big Endian only.  */

  /* We use Vector Multiply Even/Odd Unsigned Halfword to compute
   * the 128 x 16 partial (144-bit) product of vector a with a
   * halfword element of b. The (for each halfword of vector b)
   * 8 X 144-bit partial products are  summed to produce the full
   * 256-bit product. */
  vui16_t tsw;
  vui16_t tc;
  vui16_t t_odd, t_even;
  vui16_t z = { 0, 0, 0, 0, 0, 0, 0, 0 };

  tsw = vec_splat ((vui16_t) b, 7);
  t_even = (vui16_t)vec_vmuleuh((vui16_t)a, tsw);
  t_odd = (vui16_t)vec_vmulouh((vui16_t)a, tsw);

  /* Rotate the low 16-bits (right) into tmq. This is actually
   * implemented as 112-bit (14-byte) shift left. */
  tmq = (vui32_t)vec_sld (t_odd, z, 14);
  /* shift the low 128 bits of partial product right 16-bits */
  t_odd = vec_sld (z, t_odd, 14);
  /* add the high 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui16_t) b, 6);

  t_even = (vui16_t)vec_vmuleuh((vui16_t)a, tsw);
  t_odd = (vui16_t)vec_vmulouh((vui16_t)a, tsw);

  /* Sum the low 128 bits of odd previous partial products,
   * generate the carry.  */
  t_odd = (vui16_t) vec_addcq ((vui128_t*) &tc, (vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 16-bits into tmq */
  tmq = (vui32_t)vec_sld (t_odd, (vui16_t)tmq, 14);
  /* shift the low 128 bits (with carry) of partial product right
   * 16-bits */
  t_odd = vec_sld (tc, t_odd, 14);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui16_t) b, 5);

  t_even = (vui16_t)vec_vmuleuh((vui16_t)a, tsw);
  t_odd = (vui16_t)vec_vmulouh((vui16_t)a, tsw);

  /* Sum the low 128 bits of odd previous partial products,
   * generate the carry.  */
  t_odd = (vui16_t) vec_addcq ((vui128_t*) &tc, (vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 16-bits into tmq */
  tmq = (vui32_t)vec_sld (t_odd, (vui16_t)tmq, 14);
  /* shift the low 128 bits (with carry) of partial product right
   * 16-bits */
  t_odd = vec_sld (tc, t_odd, 14);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui16_t) b, 4);

  t_even = (vui16_t)vec_vmuleuh((vui16_t)a, tsw);
  t_odd = (vui16_t)vec_vmulouh((vui16_t)a, tsw);

  /* Sum the low 128 bits of odd previous partial products,
   * generate the carry.  */
  t_odd = (vui16_t) vec_addcq ((vui128_t*) &tc, (vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 16-bits into tmq */
  tmq = (vui32_t)vec_sld (t_odd, (vui16_t)tmq, 14);
  /* shift the low 128 bits (with carry) of partial product right
   * 16-bits */
  t_odd = vec_sld (tc, t_odd, 14);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui16_t) b, 3);

  t_even = (vui16_t)vec_vmuleuh((vui16_t)a, tsw);
  t_odd = (vui16_t)vec_vmulouh((vui16_t)a, tsw);

  /* Sum the low 128 bits of odd previous partial products,
   * generate the carry.  */
  t_odd = (vui16_t) vec_addcq ((vui128_t*) &tc, (vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 16-bits into tmq */
  tmq = (vui32_t)vec_sld (t_odd, (vui16_t)tmq, 14);
  /* shift the low 128 bits (with carry) of partial product right
   * 16-bits */
  t_odd = vec_sld (tc, t_odd, 14);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui16_t) b, 2);

  t_even = (vui16_t)vec_vmuleuh((vui16_t)a, tsw);
  t_odd = (vui16_t)vec_vmulouh((vui16_t)a, tsw);

  /* Sum the low 128 bits of odd previous partial products,
   * generate the carry.  */
  t_odd = (vui16_t) vec_addcq ((vui128_t*) &tc, (vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 16-bits into tmq */
  tmq = (vui32_t)vec_sld (t_odd, (vui16_t)tmq, 14);
  /* shift the low 128 bits (with carry) of partial product right
   * 16-bits */
  t_odd = vec_sld (tc, t_odd, 14);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui16_t) b, 1);

  t_even = (vui16_t)vec_vmuleuh((vui16_t)a, tsw);
  t_odd = (vui16_t)vec_vmulouh((vui16_t)a, tsw);

  /* Sum the low 128 bits of odd previous partial products,
   * generate the carry.  */
  t_odd = (vui16_t) vec_addcq ((vui128_t*) &tc, (vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 16-bits into tmq */
  tmq = (vui32_t)vec_sld (t_odd, (vui16_t)tmq, 14);
  /* shift the low 128 bits (with carry) of partial product right
   * 16-bits */
  t_odd = vec_sld (tc, t_odd, 14);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);

  tsw = vec_splat ((vui16_t) b, 0);

  t_even = (vui16_t)vec_vmuleuh((vui16_t)a, tsw);
  t_odd = (vui16_t)vec_vmulouh((vui16_t)a, tsw);

  /* Sum the low 128 bits of odd previous partial products,
   * generate the carry.  */
  t_odd = (vui16_t) vec_addcq ((vui128_t*) &tc, (vui128_t) t_odd, (vui128_t) t);

  /* rotate right the low 16-bits into tmq */
  tmq = (vui32_t)vec_sld (t_odd, (vui16_t)tmq, 14);
  /* shift the low 128 bits (with carry) of partial product right
   * 16-bits */
  t_odd = vec_sld (tc, t_odd, 14);
  /* add the top 128 bits of even / odd partial products */
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);
#endif
#endif
  *mulu = (vui128_t) t;
  return ((vui128_t) tmq);
}

/** \brief Vector Population Count Quadword.
 *
 *  Count the number of '1' bits within a vector __int128 and return
 *  the count (0-128) in a vector __int128.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 15    |2/2 cycles|
 *  |power9   | 16    | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @return a 128-bit vector with bits 121:127 containing the
 *  population count.
 */
static inline vui128_t
vec_popcntq (vui128_t vra)
{
  vui64_t result;

#ifdef _ARCH_PWR8
  /*
   * Use the Vector Population Count Doubleword instruction to get
   * the count for the left and right vector halves.  Then sum across
   * the left and right counts to get the final 128-bit vector count
   * (0-128).
   */
  vui64_t vt1;
  const vui64_t vzero = { 0, 0 };

  vt1 = vec_vpopcnt ((__vector unsigned long long) vra);
  result = (vui64_t) vec_sums ((__vector int) vt1,
                               (__vector int) vzero);
#else
  //#warning Implememention pre power8
  vui32_t z= { 0,0,0,0};
  vui32_t x;
  x = vec_popcntw ((vui32_t)vra);
  result = (vui64_t) vec_sums ((vi32_t) x, (vi32_t) z);
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  result = (vui64_t) vec_sld (
      (vui8_t) result, (vui8_t) result, 4);
#endif
  return ((vui128_t) result);
}

/*! \brief Vector Byte Reverse Quadword.
 *
 *  Return the bytes / octets of a 128-bit vector in reverse order.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-13  | 2 cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned  __int128.
 *  @return a 128-bit vector with the bytes in reserve order.
 */
static inline vui128_t
vec_revbq (vui128_t vra)
{
  vui128_t result;

#ifdef _ARCH_PWR9
#ifndef vec_revb
  __asm__(
      "xxbrq %x0,%x1;"
      : "=wa" (result)
      : "wa" (vra)
      : );
#else
  result = vec_revb (vra);
#endif
#else
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  const vui64_t vconstp = CONST_VINT64_DW(0x0F0E0D0C0B0A0908UL, 0x0706050403020100UL);
#else
  const vui64_t vconstp =
      CONST_VINT64_DW(0x0001020304050607UL, 0x08090A0B0C0D0E0FUL);
#endif
  result = (vui128_t) vec_perm ((vui8_t) vra, (vui8_t) vra, (vui8_t) vconstp);
#endif

  return (result);
}

/** \brief Vector Rotate Left Quadword.
 *
 *  Vector Rotate Left Quadword 0-127 bits.
 *  The shift amount is from bits 121-127 of vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10    | 1 cycle  |
 *  |power9   | 14    | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @param vrb Shift amount in bits 121:127.
 *  @return Left shifted vector.
 */
static inline vui128_t
vec_rlq (vui128_t vra, vui128_t vrb)
{
  vui128_t result;

  result = vec_sldq (vra, vra, vrb);

  return ((vui128_t) result);
}

/** \brief Vector Rotate Left Quadword Immediate.
 *
 *  Vector Rotate Left Quadword 0-127 bits.
 *  The shift amount is from bits 121-127 of vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10    | 1 cycle  |
 *  |power9   | 14    | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @param shb Shift amount in the range 0-127.
 *  @return Left shifted vector.
 */
static inline vui128_t
vec_rlqi (vui128_t vra, const unsigned int shb)
{
  vui8_t result;

  if (__builtin_constant_p (shb) && ((shb % 8) == 0))
    {
      /* When shifting an multiple of 8 bits (octet), use Vector
       Shift Left Double By Octet Immediate.  This eliminates
       loading the shift const into a VR.  */
      if (shb > 0)
	result = vec_sld ((vui8_t) vra, (vui8_t) vra, (shb / 8));
      else
	result = (vui8_t) vra;
    }
  else
    {
      result = (vui8_t) vec_sldqi (vra, vra, shb);
    }
  return ((vui128_t) result);
}

/*! \brief Vector Set Bool from Quadword Carry.
 *
 *  If the vector quadword carry bit (vcy.bit[127]) is '1'
 *  then return a vector bool __int128 that is all '1's.
 *  Otherwise return all '0's.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4 - 6 |2/2 cycles|
 *  |power9   | 3 - 5 | 2/cycle  |
 *
 *  Vector quadword carries are normally the result of a
 *  <I>write-Carry</I> operation. For example; vec_addcuq(),
 *  vec_addecuq(), vec_subcuq(), vec_subecuq(), vec_addcq(),
 *  vec_addeq().
 *
 *  @param vcy a 128-bit vector generated from a <I>write-Carry</I>
 *  operation.
 *  @return a 128-bit vector bool of all '1's if the carry bit is '1'.
 *  Otherwise all '0's.
 */
static inline vb128_t
vec_setb_cyq (vui128_t vcy)
{
#ifdef _ARCH_PWR8
  const vui128_t zero = (vui128_t) vec_splat_u32(0);

  return (vb128_t) vec_vsubuqm (zero, vcy);
#else
  const vui32_t ones =  vec_splat_u32(1);
  vui32_t rcy;

  rcy = vec_and ((vui32_t)vcy, ones);
  rcy = (vui32_t) vec_cmpeq (rcy, ones);
  rcy = vec_splat (rcy, VEC_W_L);

  return (vb128_t) rcy;
#endif
}

/*! \brief Vector Set Bool from Quadword not Carry.
 *
 *  If the vector quadword carry bit (vcy.bit[127]) is '1'
 *  then return a vector bool __int128 that is all '0's.
 *  Otherwise return all '1's.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4 - 6 |2/2 cycles|
 *  |power9   | 3 - 5 | 2/cycle  |
 *
 *  Vector quadword carries are normally the result of a
 *  <I>write-Carry</I> operation. For example; vec_addcuq(),
 *  vec_addecuq(), vec_subcuq(), vec_subecuq(), vec_addcq(),
 *  vec_addeq().
 *
 *  @param vcy a 128-bit vector generated from a <I>write-Carry</I>
 *  operation.
 *  @return a 128-bit vector bool of all '1's if the carry bit is '0'.
 *  Otherwise all '0's.
 */
static inline vb128_t
vec_setb_ncq (vui128_t vcy)
{
#ifdef _ARCH_PWR8
  const vui128_t zero = (vui128_t) vec_splat_u32(0);

  return (vb128_t) vec_vsubeuqm (zero, zero, vcy);
#else
  const vui32_t ones =  vec_splat_u32(1);
  vui32_t rcy;

  rcy = vec_and ((vui32_t)vcy, ones);
  rcy = (vui32_t) vec_cmplt (rcy, ones);
  rcy = vec_splat (rcy, VEC_W_L);

  return (vb128_t) rcy;
#endif
}

/*! \brief Vector Set Bool from Signed Quadword.
 *
 *  If the quadword's sign bit is '1' then return a vector bool
 *  __int128 that is all '1's. Otherwise return all '0's.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4 - 6 | 2/cycle  |
 *  |power9   | 5 - 8 | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as signed __int128.
 *  @return a 128-bit vector bool of all '1's if the sign bit is '1'.
 *  Otherwise all '0's.
 */
static inline vb128_t
vec_setb_sq (vi128_t vra)
{
  const vui8_t shift = vec_splat_u8 (7);
  vui8_t splat = vec_splat ((vui8_t) vra, VEC_BYTE_H);

  return (vb128_t) vec_sra (splat, shift);
}

/** \brief Vector Shift Left Double Quadword.
 *
 *  Vector Shift Left double Quadword 0-127 bits.
 *  Return a vector __int128 that is the left most 128-bits after
 *  shifting left 0-127-bits of the 256-bit double vector
 *  (vrw||vrx).  The shift amount is from bits 121:127 of vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10    | 1 cycle  |
 *  |power9   | 14    | 1/cycle  |
 *
 *  @param vrw upper 128-bits of the 256-bit double vector.
 *  @param vrx lower 128-bits of the 256-bit double vector.
 *  @param vrb Shift amount in bits 121:127.
 *  @return high 128-bits of left shifted double vector.
 */
static inline vui128_t
vec_sldq (vui128_t vrw, vui128_t vrx, vui128_t vrb)
{
  vui8_t result, vt1, vt2, vt3, vbs;
  const vui8_t vzero = vec_splat_u8 (0);

  vt1 = vec_slo ((vui8_t) vrw, (vui8_t) vrb);
  /* The vsl/vsr instruction only works correctly if the bit shift
     value is splatted to each byte of the vector.  */
  vbs = vec_splat ((vui8_t) vrb, VEC_BYTE_L);
  vt1 = vec_sll (vt1, vbs);
  vt3 = vec_sub (vzero, vbs);
  vt2 = vec_sro ((vui8_t) vrx, vt3);
  vt2 = vec_srl (vt2, vt3);
  result = vec_or (vt1, vt2);

  return ((vui128_t) result);
}

/** \brief Vector Shift Left Double Quadword Immediate.
 *
 *  Vector Shift Left double Quadword 0-127 bits.
 *  Return a vector __int128 that is the left most 128-bits after
 *  shifting left 0-127-bits of the 256-bit double vector
 *  (vrw||vrx).  The shift amount is from bits 121:127 of vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10    | 1 cycle  |
 *  |power9   | 14    | 1/cycle  |
 *
 *  @param vrw upper 128-bits of the 256-bit double vector.
 *  @param vrx lower 128-bits of the 256-bit double vector.
 *  @param shb Shift amount in the range 0-127.
 *  @return high 128-bits of left shifted double vector.
 */
static inline vui128_t
vec_sldqi (vui128_t vrw, vui128_t vrx, const unsigned int shb)
{
  vui8_t result;

  if (__builtin_constant_p (shb) && ((shb % 8) == 0))
    {
      /* When shifting an multiple of 8 bits (octet), use Vector
       Shift Left Double By Octet Immediate.  This eliminates
       loading the shift const into a VR.  */
      if (shb > 0)
	result = vec_sld ((vui8_t) vrw, (vui8_t) vrx, (shb / 8));
      else
	result = (vui8_t) vrw;
    }
  else
    {
      const vui8_t vrb = vec_splats ((unsigned char) shb);
      result = (vui8_t) vec_sldq (vrw, vrx, (vui128_t) vrb);
    }

  return ((vui128_t) result);
}

/** \brief Vector Shift Left Quadword.
 *
 *  Vector Shift Left Quadword 0-127 bits.
 *  The shift amount is from bits 121-127 of vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     | 1/cycle  |
 *  |power9   | 6     | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @param vrb Shift amount in bits 121:127.
 *  @return Left shifted vector.
 */
static inline vui128_t
vec_slq (vui128_t vra, vui128_t vrb)
{
  vui8_t result, vshift_splat;

  /* For some reason, the vsl instruction only works
   * correctly if the bit shift value is splatted to each byte
   * of the vector.  */
  vshift_splat = vec_splat ((vui8_t) vrb, VEC_BYTE_L);
  result = vec_slo ((vui8_t) vra, (vui8_t) vrb);
  result = vec_sll (result, vshift_splat);

  return ((vui128_t) result);
}

/** \brief Vector Shift Left Quadword Immediate.
 *
 *  Shift left Quadword 0-127 bits.
 *  The shift amount is a const unsigned int in the range 0-127.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 127 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-13  | 2 cycle  |
 *  |power9   | 3-15  | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @param shb Shift amount in the range 0-127.
 *  @return 128-bit vector shifted left shb bits.
 */
static inline vui128_t
vec_slqi (vui128_t vra, const unsigned int shb)
{
  vui8_t lshift;
  vui8_t result;

  if (shb < 128)
    {
      if (__builtin_constant_p (shb) && ((shb % 8) == 0))
	{
	  /* When shifting an multiple of 8 bits (octet), use Vector
	   Shift Left Double By Octet Immediate.  This eliminates
	   loading the shift const into a VR, but requires an
	   explicit vector of zeros.  */
	  vui8_t zero =
	    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	  if (shb > 0)
	    result = vec_sld ((vui8_t) vra, zero, (shb / 8));
	  else
	    result = (vui8_t) vra;
	}
      else
	{
	  /* Load the shift const in a vector.  The bit level shifts
	   require the shift amount is splatted to all 16-bytes of
	   the shift control.  */
	  if (__builtin_constant_p (shb) && (shb < 16))
	    lshift = (vui8_t) vec_splat_s8(shb);
	  else
	    lshift = vec_splats ((unsigned char) shb);

	  if (shb > 7)
	    /* Vector Shift Left By Octet by bits 121-124 of lshift.  */
	    result = vec_slo ((vui8_t) vra, lshift);
	  else
	    result = ((vui8_t) vra);

	  /* Vector Shift Left by bits 125-127 of lshift.  */
	  result = vec_sll (result, lshift);
	}
    }
  else
    { /* shifts greater then 127 bits return zeros.  */
      result = vec_xor ((vui8_t) vra, (vui8_t) vra);
    }
  return (vui128_t) result;
}

/** \brief Vector Shift Right Algebraic Quadword.
 *
 *  Vector Shift Right Algebraic Quadword 0-127 bits.
 *  The shift amount is from bits 121-127 of vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10    | 1 cycle  |
 *  |power9   | 14    | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as signed __int128.
 *  @param vrb Shift amount in bits 121:127.
 *  @return Right algebraic shifted vector.
 */
static inline vi128_t
vec_sraq (vi128_t vra, vui128_t vrb)
{
  vui8_t result, vsht;
  vui128_t vsgn;

  const vui8_t zero = vec_splat_u8 (0);

  /* For some reason the vsr instruction only works
   * correctly if the bit shift value is splatted to each byte
   * of the vector.  */
  vsgn = vec_setb_sq (vra);
  vsht = vec_sub (zero, (vui8_t) vrb);
  result = (vui8_t) vec_sldq (vsgn, (vui128_t) vra, (vui128_t) vsht);

  return ((vi128_t) result);
}

/** \brief Vector Shift Right Algebraic Quadword Immediate.
 *
 *  Vector Shift Right Algebraic Quadword 0-127 bits.
 *  The shift amount is from bits 121-127 of vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6-15  | 1 cycle  |
 *  |power9   | 9-18  | 1/cycle  |
 *
 *  \note vec_sraqi optimizes for some special cases.
 *  For shift by octet (multiple of 8 bits) use vec_setb_sq ()
 *  to extend sign then vector shift left double by octet immediate
 *  by (16 - (shb / 8)) to effect the right octet shift.
 *  For _ARCH_PWR8 and shifts less than 64 bits, use both vec_srqi ()
 *  and vector shift right algebraic doubleword. Then use vec_pasted ()
 *  to combine the high 64-bits from vec_sradi () and the low 64-bits
 *  from vec_srqi ().
 *
 *  @param vra a 128-bit vector treated as signed __int128.
 *  @param shb Shift amount in the range 0-127.
 *  @return Right algebraic shifted vector.
 */
static inline vi128_t
vec_sraqi (vi128_t vra, const unsigned int shb)
{
  vui8_t result, lshift;
  vui128_t vsgn;

  if (shb < 127)
    {
      if (__builtin_constant_p (shb) && ((shb % 8) == 0))
	{
	  if (shb > 0)
	    {
	      vsgn = vec_setb_sq (vra);
	      result = vec_sld ((vui8_t) vsgn, (vui8_t) vra, 16 - (shb / 8));
	    }
	  else
	    result = (vui8_t) vra;
	}
      else
	{
#ifdef _ARCH_PWR8
	  if (shb < 64)
	    {
	      vui128_t vrshq;
	      vi64_t vrshd;
	      vrshq = vec_srqi ((vui128_t) vra, shb);
	      vrshd = vec_sradi ((vi64_t) vra, shb);
	      result = (vui8_t) vec_pasted ((vui64_t) vrshd, (vui64_t) vrshq);
	    }
	  else
	    {
#endif
	      const unsigned int lshb = 128 - shb;
	      if (__builtin_constant_p (shb) && (lshb < 16))
		lshift = (vui8_t) vec_splat_s8(shb);
	      else
		lshift = vec_splats ((unsigned char) lshb);

	      vsgn = vec_setb_sq (vra);
	      result = (vui8_t) vec_sldq (vsgn, (vui128_t) vra,
					  (vui128_t) lshift);
#ifdef _ARCH_PWR8
	    }
#endif
	}
    }
  else
    { /* shifts greater then 126 bits returns the sign bit.  */
      result = (vui8_t) vec_setb_sq (vra);
    }

  return ((vi128_t) result);
}

/** \brief Vector Shift Right Quadword.
 *
 *  Vector Shift Right Quadword 0-127 bits.
 *  The shift amount is from bits 121-127 of vrb.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     | 1/cycle  |
 *  |power9   | 6     | 1/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @param vrb Shift amount in bits 121:127.
 *  @return Right shifted vector.
 */
static inline vui128_t
vec_srq (vui128_t vra, vui128_t vrb)
{
  vui8_t result, vsht_splat;

  /* For some reason the vsr instruction only works
   * correctly if the bit shift value is splatted to each byte
   * of the vector.  */
  vsht_splat = vec_splat ((vui8_t) vrb, VEC_BYTE_L);
  result = vec_sro ((vui8_t) vra, (vui8_t) vrb);
  result = vec_srl (result, vsht_splat);

  return ((vui128_t) result);
}

/** \brief Vector Shift Right Quadword Immediate.
 *
 *  Shift right Quadword 0-127 bits.
 *  The shift amount is a const unsigned int in the range 0-127.
 *  A shift count of 0 returns the original value of vra.
 *  Shift counts greater then 127 bits return zero.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2-13  | 2 cycle  |
 *  |power9   | 3-15  | 2/cycle  |
 *
 *  @param vra a 128-bit vector treated as unsigned __int128.
 *  @param shb Shift amount in the range 0-127.
 *  @return 128-bit vector shifted right shb bits.
 */
static inline vui128_t
vec_srqi (vui128_t vra, const unsigned int shb)
{
  vui8_t lshift;
  vui8_t result;

  if (shb < 128)
    {
      if (__builtin_constant_p (shb) && ((shb % 8)) == 0)
	{
	  /* When shifting an multiple of 8 bits (octet), use Vector
	   Shift Left Double By Octet Immediate.  This eliminates
	   loading the shift const into a VR, but requires an
	   explicit vector of zeros.  */
	  vui8_t zero =
	    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	  /* The compiler needs to know at compile time that
	     0 < shb < 128 is true to insure the constraint (4 bit
	     immediate field) of vsldoi is meet.  So the following if
	     is required but should not generate any branch code.  */
	  if (shb > 0)
	    result = vec_sld (zero, (vui8_t) vra, (16 - (shb / 8)));
	  else
	    result = (vui8_t) vra;
	}
      else
	{
	  /* Load the shift const in a vector.  The bit level shifts
	   require the shift amount is splatted to all 16-bytes of
	   the shift control.  */
	  if ((__builtin_constant_p (shb) && (shb < 16)))
	    lshift = (vui8_t) vec_splat_s8(shb);
	  else
	    lshift = vec_splats ((unsigned char) shb);

	  if (shb > 7)
	    /* Vector Shift right By Octet based on the bits 121-124 of
	     lshift.  */
	    result = vec_sro ((vui8_t) vra, lshift);
	  else
	    result = ((vui8_t) vra);

	  /* Vector Shift right based on the lower 3-bits of lshift.  */
	  result = vec_srl (result, lshift);
	}
    }
  else
    { /* shifts greater then 127 bits return zeros.  */
      result = vec_xor ((vui8_t) vra, (vui8_t) vra);
    }
  return (vui128_t) result;
}

/** \deprecated Vector Shift Left 4-bits Quadword.
 * Replaced by vec_slqi with shb param = 4.
 *
 * Vector Shift Left Quadword 0-127 bits.
 * The shift amount is from bits 121-127 of vrb.
 *
 * @param vra a 128-bit vector treated a __int128.
 * @return Left shifted vector.
 */
static inline vui128_t
vec_slq4 (vui128_t vra)
{
  __vector unsigned char result, vsht_splat;

  /* The vsl instruction only works correctly if the bit shift value
   * is splatted to each byte of the vector.  */
  vsht_splat = vec_splat_u8(4);
  result = vec_sll ((__vector unsigned char) vra, vsht_splat);

  return ((vui128_t) result);
}

/** \deprecated Vector Shift Left 5-bits Quadword.
 * Replaced by vec_slqi with shb param = 5.
 *
 * Vector Shift Left Quadword 0-127 bits.
 * The shift amount is from bits 121-127 of vrb.
 *
 *	@param vra a 128-bit vector treated a __int128.
 *	@return Left shifted vector.
 */
static inline vui128_t
vec_slq5 (vui128_t vra)
{
  __vector unsigned char result, vsht_splat;

  /* The vsl instruction only works correctly if the bit shift value
   * is splatted to each byte of the vector.  */
  vsht_splat = vec_splat_u8(5);
  result = vec_sll ((__vector unsigned char) vra, vsht_splat);

  return ((vui128_t) result);
}

/** \deprecated Vector Shift right 4-bits Quadword.
 * Replaced by vec_srqi with shb param = 4.
 *
 * Vector Shift Right Quadword 0-127 bits.
 * The shift amount is from bits 121-127 of vrb.
 *
 * @param vra a 128-bit vector treated as a __int128.
 * @return Right shifted vector.
 */
static inline vui128_t
vec_srq4 (vui128_t vra)
{
  __vector unsigned char result, vsht_splat;

  /* The vsr instruction only works correctly if the bit shift value
   * is splatted to each byte of the vector.  */
  vsht_splat = vec_splat_u8(4);
  result = vec_srl ((__vector unsigned char) vra, vsht_splat);

  return ((vui128_t) result);
}

/** \deprecated Vector Shift right 5-bits Quadword.
 * Replaced by vec_srqi with shb param = 5.
 *
 * Vector Shift Right Quadword 0-127 bits.
 * The shift amount is from bits 121-127 of vrb.
 *
 * @param vra a 128-bit vector treated a __int128.
 * @return Right shifted vector.
 */
static inline vui128_t
vec_srq5 (vui128_t vra)
{
  __vector unsigned char result, vsht_splat;

  /* The vsr instruction only works correctly if the bit shift value
   * is splatted to each byte of the vector.  */
  vsht_splat = vec_splat_u8(5);
  result = vec_srl ((__vector unsigned char) vra, vsht_splat);

  return ((vui128_t) result);
}

/** \brief Vector Subtract and Write Carry Unsigned Quadword.
 *
 *  Generate the carry-out of the sum (vra + NOT(vrb) + 1).
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     |2/2 cycles|
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as unsigned __int128.
 *  @param vrb 128-bit vector treated as unsigned __int128.
 *  @return __int128 carry from the unsigned difference vra - vrb.
 */
static inline vui128_t
vec_subcuq (vui128_t vra, vui128_t vrb)
{
  vui32_t t;
#ifdef _ARCH_PWR8
#ifndef vec_vsubcuq
  __asm__(
      "vsubcuq %0,%1,%2;"
      : "=v" (t)
      : "v" (vra),
      "v" (vrb)
      : );
#else
  t = (vui32_t) vec_vsubcuq (vra, vrb);
#endif
#else
  /* vsubcuq is defined as (vra + NOT(vrb) + 1) >> 128.  */
  vui32_t _b = vec_nor ((vui32_t) vrb, (vui32_t) vrb);
  const vui32_t ci= { 0,0,0,1 };

  t = (vui32_t) vec_addecuq (vra, (vui128_t) _b, (vui128_t) ci);
#endif
  return ((vui128_t) t);
}

/** \brief Vector Subtract Extended and Write Carry Unsigned Quadword.
 *
 *  Generate the carry-out of the sum (vra + NOT(vrb) + vrc.bit[127]).
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     |2/2 cycles|
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as unsigned __int128.
 *  @param vrb 128-bit vector treated as unsigned __int128.
 *  @param vrc 128-bit vector carry-in from bit 127.
 *  @return __int128 carry from the extended __int128 difference.
 */
static inline vui128_t
vec_subecuq (vui128_t vra, vui128_t vrb, vui128_t vrc)
{
  vui32_t t;
#ifdef _ARCH_PWR8
#ifndef vec_vsubcuq
  __asm__(
      "vsubecuq %0,%1,%2,%3;"
      : "=v" (t)
      : "v" (vra),
	"v" (vrb),
        "v" (vrc)
      : );
#else
  t = (vui32_t) vec_vsubecuq (vra, vrb, vrc);
#endif
#else
  /* vsubcuq is defined as (vra + NOT(vrb) + vrc.bit[127]) >> 128.  */
  vui32_t _b = vec_nor ((vui32_t) vrb, (vui32_t) vrb);

  t = (vui32_t) vec_addecuq (vra, (vui128_t) _b, vrc);
#endif
  return ((vui128_t) t);
}

/** \brief Vector Subtract Extended Unsigned Quadword Modulo.
 *
 *  Subtract two vector __int128 values and return result modulo 128-bits.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     |2/2 cycles|
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as unsigned __int128.
 *  @param vrb 128-bit vector treated as unsigned __int128.
 *  @param vrc 128-bit vector carry-in from bit 127.
 *  @return __int128 unsigned difference of vra minus vrb.
 */
static inline vui128_t
vec_subeuqm (vui128_t vra, vui128_t vrb, vui128_t vrc)
{
  vui32_t t;
#ifdef _ARCH_PWR8
#ifndef vec_vsubuqm
  __asm__(
      "vsubeuqm %0,%1,%2,%3;"
      : "=v" (t)
      : "v" (vra),
	"v" (vrb),
        "v" (vrc)
      : );
#else
  t = (vui32_t) vec_vsubeuqm (vra, vrb, vrc);
#endif
#else
  /* vsubeuqm is defined as vra + NOT(vrb) + vrc.bit[127].  */
  vui32_t _b = vec_nor ((vui32_t) vrb, (vui32_t) vrb);

  t = (vui32_t) vec_addeuqm (vra, (vui128_t) _b, vrc);
#endif
  return ((vui128_t) t);
}

/** \brief Vector Subtract Unsigned Quadword Modulo.
 *
 *  Subtract two vector __int128 values and return result modulo 128-bits.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4     |2/2 cycles|
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vra 128-bit vector treated as unsigned __int128.
 *  @param vrb 128-bit vector treated as unsigned __int128.
 *  @return __int128 unsigned difference of vra minus vrb.
 */
static inline vui128_t
vec_subuqm (vui128_t vra, vui128_t vrb)
{
  vui32_t t;
#ifdef _ARCH_PWR8
#ifndef vec_vsubuqm
  __asm__(
      "vsubuqm %0,%1,%2;"
      : "=v" (t)
      : "v" (vra),
      "v" (vrb)
      : );
#else
  t = (vui32_t) vec_vsubuqm (vra, vrb);
#endif
#else
  /* vsubuqm is defined as vra + NOT(vrb) + 1.  */
  vui32_t _b = vec_nor ((vui32_t) vrb, (vui32_t) vrb);
  const vui32_t ci= { 0,0,0,1 };

  t = (vui32_t) vec_addeuqm (vra, (vui128_t) _b, (vui128_t) ci);
#endif
  return ((vui128_t) t);
}


/** \brief Vector Multiply Even Unsigned Doublewords.
 *
 *  Multiply the even 64-bit doublewords of two vector unsigned long
 *  values and return the unsigned __int128 product of the even
 *  doublewords.
 *
 *  \note This function implements the operation of a Vector Multiply
 *  Even Doubleword instruction, as if the PowerISA included such an
 *  instruction.
 *  This implementation is NOT endian sensitive and the function is
 *  stable across BE/LE implementations.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 21-23 | 1/cycle  |
 *  |power9   | 8-13  | 2/cycle  |
 *
 *  @param a 128-bit vector unsigned long int.
 *  @param b 128-bit vector unsigned long int.
 *  @return vector unsigned __int128 product of the even double words of a and b.
 */
static inline vui128_t
vec_vmuleud (vui64_t a, vui64_t b)
{
  vui64_t res;

#ifdef _ARCH_PWR9
  const vui64_t zero = { 0, 0 };
  vui64_t b_eud = vec_mrgahd ((vui128_t) b, (vui128_t) zero);
  __asm__(
      "vmsumudm %0,%1,%2,%3;\n"
      : "=v" (res)
      : "v" (a), "v" (b_eud), "v" (zero)
      : );
#else
#ifdef _ARCH_PWR8
  const vui64_t zero = { 0, 0 };
  vui64_t p0, p1, pp10, pp01;
  vui32_t m0, m1;

// Need the endian invariant merge word high here
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
// Nullify the little endian transform
  m0 = vec_mergel ((vui32_t) b, (vui32_t) b);
#else
  m0 = vec_mergeh ((vui32_t) b, (vui32_t) b);
#endif
  m1 = (vui32_t) vec_xxspltd ((vui64_t) a, 0);

  // Need the endian invariant multiply even/odd word here
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  // Nullify the little endian transform
  p1 = vec_muleuw (m1, m0);
  p0 = vec_mulouw (m1, m0);
#else
  p1 = vec_mulouw (m1, m0);
  p0 = vec_muleuw (m1, m0);
#endif
  /* res[1] = p1[1];  res[0] = p0[0];  */
  res = vec_pasted (p0, p1);
  /*
   pp10[1] = p1[0]; pp10[0] = 0;
   pp01[1] = p0[1]; pp01[0] = 0;
   */
  // Need the endian invariant merge algebraic high/low here
  pp10 = (vui64_t) vec_mrgahd ((vui128_t) zero, (vui128_t) p1);
  pp01 = (vui64_t) vec_mrgald ((vui128_t) zero, (vui128_t) p0);
  /* pp01 = pp01 + pp10.  */
  pp01 = (vui64_t) vec_adduqm ((vui128_t) pp01, (vui128_t) pp10);

  /* res = res + (pp01 << 32)  */
  pp01 = (vui64_t) vec_sld ((vi32_t) pp01, (vi32_t) pp01, 4);
  res = (vui64_t) vec_adduqm ((vui128_t) pp01, (vui128_t) res);
#else
  const vui32_t zero = {0,0,0,0};
  vui32_t p0, p1;
  vui32_t resw;
  vui16_t m0, m1, mm;

  m0 = (vui16_t) vec_mergeh (a, (vui64_t) zero);
  mm = (vui16_t) vec_mergeh (b, (vui64_t) zero);

  m1 = vec_splat (mm, 3);

  p0 = vec_vmuleuh (m0, m1);
  p1 = vec_vmulouh (m0, m1);

  resw = vec_sld (zero, p1, 14);
  {
    vui32_t c;
    c    = vec_vaddcuw (resw, p0);
    resw = vec_vadduwm (resw, p0);
    c    = vec_sld (c, c, 4);
    resw = vec_vadduwm (resw, c);
  }

  m1 = vec_splat (mm, 2);
  p0 = vec_vmuleuh (m0, m1);
  p1 = vec_vmulouh (m0, m1);

  {
    vui32_t c;
    c    = vec_vaddcuw (resw, p1);
    resw = vec_vadduwm (resw, p1);
    c    = vec_sld (c, c, 4);
    resw = vec_vadduwm (resw, c);
    resw = vec_sld (c, resw, 14);
  }

  {
    vui32_t c;
    c    = vec_vaddcuw (resw, p0);
    resw = vec_vadduwm (resw, p0);
    c    = vec_sld (c, c, 4);
    resw = vec_vadduwm (resw, c);
  }

  m1 = vec_splat (mm, 1);
  p0 = vec_vmuleuh (m0, m1);
  p1 = vec_vmulouh (m0, m1);

  {
    vui32_t c;
    c    = vec_vaddcuw (resw, p1);
    resw = vec_vadduwm (resw, p1);
    c    = vec_sld (c, c, 4);
    resw = vec_vadduwm (resw, c);
    resw = vec_sld (c, resw, 14);
  }

  {
    vui32_t c;
    c    = vec_vaddcuw (resw, p0);
    resw = vec_vadduwm (resw, p0);
    c    = vec_sld (c, c, 4);
    resw = vec_vadduwm (resw, c);
  }

  m1 = vec_splat (mm, 0);
  p0 = vec_vmuleuh (m0, m1);
  p1 = vec_vmulouh (m0, m1);

  {
    vui32_t c;
    c    = vec_vaddcuw (resw, p1);
    resw = vec_vadduwm (resw, p1);
    c    = vec_sld (c, c, 4);
    resw = vec_vadduwm (resw, c);
    resw = vec_sld (c, resw, 14);
  }

  {
    vui32_t c;
    c    = vec_vaddcuw (resw, p0);
    resw = vec_vadduwm (resw, p0);
    c    = vec_sld (c, c, 4);
    resw = vec_vadduwm (resw, c);
  }

  res = (vui64_t)resw;
#endif
#endif
  return ((vui128_t) res);
}

/** \brief Vector Multiply Odd Unsigned Doublewords.
 *
 *  Multiply the odd 64-bit doublewords of two vector unsigned long
 *  values and return the unsigned __int128 product of the odd
 *  doublewords.
 *
 *  \note This function implements the operation of a Vector Multiply
 *  Odd Doubleword instruction, as if the PowerISA included such an
 *  instruction.
 *  This implementation is NOT endian sensitive and the function is
 *  stable across BE/LE implementations.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 21-23 | 1/cycle  |
 *  |power9   | 8-13  | 2/cycle  |
 *
 *  @param a 128-bit vector unsigned long int.
 *  @param b 128-bit vector unsigned long int.
 *  @return vector unsigned __int128 product of the odd double words
 *  of a and b.
 */
static inline vui128_t
vec_vmuloud (vui64_t a, vui64_t b)
{
  vui64_t res;

#ifdef _ARCH_PWR9
  const vui64_t zero = { 0, 0 };
  vui64_t b_oud = vec_mrgald ((vui128_t) zero, (vui128_t)b);
  __asm__(
      "vmsumudm %0,%1,%2,%3;\n"
      : "=v" (res)
      : "v" (a), "v" (b_oud), "v" (zero)
      : );
#else
#ifdef _ARCH_PWR8
  const vui64_t zero = { 0, 0 };
  vui64_t p0, p1, pp10, pp01;
  vui32_t m0, m1;

  // Need the endian invariant merge word low here
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  // Nullify the little endian transform
  m0 = vec_mergeh ((vui32_t) b, (vui32_t) b);
#else
  m0 = vec_mergel ((vui32_t) b, (vui32_t) b);
#endif
  m1 = (vui32_t) vec_xxspltd ((vui64_t) a, 1);

  // Need the endian invariant multiply even/odd word here
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  // Nullify the little endian transform
  p1 = vec_muleuw (m1, m0);
  p0 = vec_mulouw (m1, m0);
#else
  p0 = vec_muleuw (m1, m0);
  p1 = vec_mulouw (m1, m0);
#endif

  /* res[1] = p1[1];  res[0] = p0[0];  */
  res = vec_pasted (p0, p1);
  /*
   pp10[0] = p1[0]; pp10[1] = 0;
   pp01[0] = p0[1]; pp01[1] = 0;
   */
  // Need the endian invariant merge algebraic high/low here
  pp10 = (vui64_t) vec_mrgahd ((vui128_t) zero, (vui128_t) p1);
  pp01 = (vui64_t) vec_mrgald ((vui128_t) zero, (vui128_t) p0);

  pp01 = (vui64_t) vec_adduqm ((vui128_t) pp01, (vui128_t) pp10);

  pp01 = (vui64_t) vec_sld ((vi32_t) pp01, (vi32_t) pp01, 4);

  res = (vui64_t) vec_adduqm ((vui128_t) pp01, (vui128_t) res);
#else
// POWER7 and earlier are big Endian only
  const vui32_t zero = {0,0,0,0};
  vui32_t p0, p1;
  vui32_t resw;
  vui16_t m0, m1, mm;

  m0 = (vui16_t) vec_mergel (a, (vui64_t) zero);
  mm = (vui16_t) vec_mergel (b, (vui64_t) zero);

  m1 = vec_splat (mm, 3);

  p0 = vec_vmuleuh (m0, m1);
  p1 = vec_vmulouh (m0, m1);

  resw = vec_sld (zero, p1, 14);

  {
    vui32_t c;
    c    = vec_vaddcuw (resw, p0);
    resw = vec_vadduwm (resw, p0);
    c    = vec_sld (c, c, 4);
    resw = vec_vadduwm (resw, c);
  }

  m1 = vec_splat (mm, 2);

  p0 = vec_vmuleuh (m0, m1);
  p1 = vec_vmulouh (m0, m1);
  {
    vui32_t c;
    c    = vec_vaddcuw (resw, p1);
    resw = vec_vadduwm (resw, p1);

    c    = vec_sld (c, c, 4);
    resw = vec_vadduwm (resw, c);
    resw = vec_sld (c, resw, 14);
  }

  {
    vui32_t c;
    c    = vec_vaddcuw (resw, p0);
    resw = vec_vadduwm (resw, p0);
    c    = vec_sld (c, c, 4);
    resw = vec_vadduwm (resw, c);
  }

  m1 = vec_splat (mm, 1);

  p0 = vec_vmuleuh (m0, m1);
  p1 = vec_vmulouh (m0, m1);

  {
    vui32_t c;
    c    = vec_vaddcuw (resw, p1);
    resw = vec_vadduwm (resw, p1);

    c    = vec_sld (c, c, 4);
    resw = vec_vadduwm (resw, c);
    resw = vec_sld (c, resw, 14);
  }

  {
    vui32_t c;
    c    = vec_vaddcuw (resw, p0);
    resw = vec_vadduwm (resw, p0);
    c    = vec_sld (c, c, 4);
    resw = vec_vadduwm (resw, c);
  }

  m1 = vec_splat (mm, 0);

  p0 = vec_vmuleuh (m0, m1);
  p1 = vec_vmulouh (m0, m1);

  {
    vui32_t c;
    c    = vec_vaddcuw (resw, p1);
    resw = vec_vadduwm (resw, p1);

    c    = vec_sld (c, c, 4);
    resw = vec_vadduwm (resw, c);
    resw = vec_sld (c, resw, 14);
  }

  {
    vui32_t c;
    c    = vec_vaddcuw (resw, p0);
    resw = vec_vadduwm (resw, p0);
    c    = vec_sld (c, c, 4);
    resw = vec_vadduwm (resw, c);
  }

  res = (vui64_t)resw;
#endif
#endif
  return ((vui128_t) res);
}
#endif /* VEC_INT128_PPC_H_ */
