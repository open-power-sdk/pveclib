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
 * PowerISA 3.0B added Quad-Precision floating point type and
 * operations to the Vector-Scalar Extension (VSX) facility.
 * The first hardware implementation is available in POWER9.
 *
 * PowerISA 3.1 added new min/max/compare Quad-Precision operations.
 * Also added new quadword (128-bit) integer operations including
 * converts between quadword integer and Quad-Precision floating point.
 * The first hardware implementation is available in POWER10.
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
 * It also seems reasonable to provide Quad-Precision extract/insert
 * exponent/significand and compare exponent operations for POWER7/8.
 * And with the PowerISA 3.1 release providing POWER9/8 implementations
 * of min/max/convert/compare.
 *
 * These PVECLIB operations should be useful for applications using
 * Quad-Precision while needing to still support POWER8. They should
 * also be useful and improve performance for soft-float
 * implementations of math library functions.
 *
 * \note The compiler disables associated <altivec.h> built-ins if the
 * <B>mcpu</B> target does not enable the specific instruction.
 * For example if you compile with <B>-mcpu=power8</B>, Quad-Precision
 * floating-point built-ins operations useful
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
 * \section f128_softfloat_0_0 Vector implementation of Quad-Precision Soft-float
 *
 * The discussion above raises a interesting question. If we can
 * provide useful implementations of Quad-Precision; classification,
 * extract/insert, and compare exponent operations, why not continue
 * with Quad-Precision compare, convert to/from integer, and
 * arithmetic operations?
 *
 * This raises the stakes in complexity and size of implementation.
 * Providing a vector soft-float implementation equivalent to the
 * GCC run-time libgcc <B>__addkf3/__divkf3/__mulkf3/__subkf3</B>
 * would be a substantial effort. The IEEE standard is exacting
 * about rounding and exception handling. Comparisons require special
 * handling of; signed zero, infinities, and NaNs.
 * Even float/integer conversions require correct rounding and return
 * special values for overflow.
 * Also it is not clear how such an effort would be accepted.
 *
 * The good news is PVECLIB already provides a strong quadword integer
 * operations set. Integer Add, subtract, and multiply are covered with
 * the usual compare/shift/rotate operations (See vec_int128_ppc.h).
 * The weak spot is general quadword integer divide. Until recently,
 * integer divide has not been part of the vector ISA.
 * But the introduction of Vector Divide Signed/Unsigned Quadword in
 * POWER10 raises the priority of vector integer divide for PVECLIB.
 *
 * For now we propose a phased approach, starting with enablers and
 * infrastructure, building up layers, starting simple and adding
 * complexity.
 *
 * - Basic enablers; classification, extract/insert exponent,
 * compare exponent.
 * - Quad-Precision comparison operators.
 *   - Initially ignore special cases and exceptions
 *   - Add Signed Zero, Infinity. and NaN special cases
 *   - Exceptions when someone asks
 * - Quad-Precision from/to integer word/doubleword/quadword.
 *   - Cases that don't require rounding (i.e truncate and DW to QP).
 *   - Cases that require rounding
 *     - Round to odd. See
 * <a href="https://www.exploringbinary.com/gcc-avoids-double-rounding-errors-with-round-to-odd/">
 * GCC Avoids Double Rounding Errors With Round-To-Odd</a>
 *     - Round to Nearest/Even
 *     - Others if asked
 * - Quad-Precision arithmetic
 *   - Add/Sub/Mul
 *     - Round-to-Odd first
 *     - Initially ignore special cases and exceptions
 *   - Fused Multiply-Add
 *     - Round-to-Odd first
 *   - Divide
 *     - Round-to-Odd first
 *   - Add Signed Zero, Infinity. and NaN special cases
 *   - Other rounding modes
 *   - Exceptions when someone asks
 *
 * The intent is that such PVECLIB operations can be mixed in with or
 * substituted for C Language _FLoat128 expressions or functions.
 * The in-lined operations should have performance advantages over
 * equivalent library functions on both POWER8/9.
 *
 * This is a big list. It is TBD how far I will get given  my current
 * limited resources.
 *
 * \note We are focusing on POWER8 here because the implementation gets
 * a lot harder for POWER7 and earlier. POWER7 is missing: Quadword
 * integer add/sub with carry extend. Wide (word) integer multiply.
 * Direct transfer between VRs and GPRs. Doubleword integer arithmetic,
 * compares. and count-leading zeros.
 *
 * \subsection f128_softfloat_0_0_0 Quad-Precision data class and exponent access for POWER8
 *
 * Most math library functions need to test the data class (normal,
 * infinity, NaN, etc) and or range of input values. This usually involves
 * separating the sign, exponent, and significand out from
 * __float128 values, and comparing one or more of these parts,
 * to special integer values.
 *
 * PowerISA 3.0B (POWER9) provides instructions for these in addition
 * to a comprehensive set of arithmetic and compare instructions.
 * These operations are also useful for the soft-float implementation
 * of __float128 for POWER8 and earlier. The OpenPOWER ABI specifies
 * __float128 parameters are in VRs and are immediately accessible to
 * VMX/VSR instructions. This is important as the cost of transferring
 * values between VRs and GPRs is quite high on POWER8 and even higher
 * for POWER7 and earlier (which requires store to temporaries and
 * reload).
 *
 * Fortunately these operations only require logical (and/or/xor),
 * shift and integer compare operations to implement. These are
 * available as vector intrinsics or provides by PVECLIB
 * (see vec_int128_ppc.h).
 *
 * The operations in this group include:
 * - Altivec like predicates;
 * vec_all_isfinitef128(),
 * vec_all_isinff128(),
 * vec_all_isnanf128(),
 * vec_all_isnormalf128(),
 * vec_all_issubnormalf128(),
 * vec_all_iszerof128(),
 * vec_signbitf128().
 * - Vector boolean predicates;
 * vec_isfinitef128(),
 * vec_isinff128(),
 * vec_isnanf128(),
 * vec_isnormalf128(),
 * vec_issubnormalf128(),
 * vec_iszerof128(),
 * vec_setb_qp().
 * - Data manipulation;
 * vec_copysignf128(),
 * vec_xsiexpqp(),
 * vec_xsxexpqp(),
 * vec_xsxsigqp().
 * - Exponent Compare;
 * TBD: The compare exponent quad-precision operation defined for P9.
 *
 * For example the data class test isnan:
 * \code
static inline vb128_t
vec_isnanf128 (__binary128 f128)
{
#if defined (_ARCH_PWR9) && defined (scalar_test_data_class) && \
  defined (__FLOAT128__) && (__GNUC__ > 7)
  vui32_t result = CONST_VINT128_W(0, 0, 0, 0);

  if (scalar_test_data_class (f128, 0x40))
    result = CONST_VINT128_W(-1, -1, -1, -1);

  return (vb128_t)result;
#else
  vui32_t tmp;
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  const vui32_t expmask = CONST_VINT128_W(0x7fff0000, 0, 0, 0);

  tmp = vec_andc_bin128_2_vui32t (f128, signmask);
  return vec_cmpgtuq ((vui128_t)tmp , (vui128_t)expmask);
#endif
}
 * \endcode
 * Which has implementations for POWER9 (and later) and POWER8
 * (and earlier).
 *
 * For POWER9 it generates:
 * \code
     xststdcqp cr0,v2,64
     bne     .+12
     xxspltib vs34,255
     b       .+8
     xxspltib vs34,0
 * \endcode
 * Which uses the intrinsic scalar_test_data_class() to generate the
 * VSX Scalar Test Data Class Quad-Precision instruction
 * with "data class mask" of <I>class.NaN</I> to set the condition code.
 * If the condition is <I>match</I>, load the 128-bit bool value of all
 * 1's (true). Otherwise load all 0's (false).
 *
 *
 * For POWER8 it generates
 * \code
     addis   r9,r2,@ha.rodata.cst16+0x30
     vspltisw v1,-1
     vspltisw v12,0
     addi    r9,r9,@l.rodata.cst16+0x30
     vslw    v1,v1,v1
     lvx     v0,0,r9
     vsldoi  v1,v1,v12,12

     xxlandc vs33,vs34,vs33
     vsubcuq v0,v0,v1
     xxspltw vs32,vs32,3
     vcmpequw v2,v0,v12
 * \endcode
 * The first 7 instructions above, load the constant vectors needed
 * by the logic. These constants only need to be generated once per
 * function and can be shared across operations.
 *
 * In the C code we use a special transfer function combined with
 * logical AND complement (vec_andc_bin128_2_vui32t()).
 * This is required because while __float128 values are held in VRs,
 * the compiler considers them to be scalars and will not
 * allow simple casts to (any) vector type. So the PVECLIB
 * implementation provides <I>xfer</I> function using a unions to
 * transfer the __float128 value to a vector type.
 * In most case this logical transfer simply serves to make the
 * compiler happy and does not need to generate any code.
 * In this case the
 * <I>xfer</I> function combines the transfer with a vector and
 * complement to mask off the sign bit.
 *
 * Then compare the masked result as a 128-bit integer value greater
 * than infinity (expmask). Here we use the vec_cmpgtuq() operation
 * from vec_int128_ppc.h. For POWER8, vec_cmpgtuq() generates the
 * Vector Subtract and Write Carry Unsigned Quadword instruction for
 * 128-bit unsigned compares. A '0' carry indicates greater than.
 * The next two instructions (from vec_setb_ncq())
 * convert the carry bit to the required
 * 128-bit bool value.
 *
 * While the POWER8 sequence requires more instructions (including the
 * const vector set up) than POWER9, it is not significantly larger.
 * And as mentioned above, the set-up code can be optimized across
 * operations sharing the same constants. The code
 * (less the setup) is only 10 cycles for POWER8 vs 6 for POWER9.
 * Also the code is not any larger than the function call overhead for
 * the libgcc runtime equivalent <B>__unordkf2</B>. And is much faster
 * then the generic soft-float implementation.
 *
 * Another example, Scalar Extract Exponent Quad-Precision:
 *
 * \code
static inline vui64_t
vec_xsxexpqp (__binary128 f128)
{
  vui64_t result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  __asm__(
      "xsxexpqp %0,%1"
      : "=v" (result)
      : "v" (f128)
      : );

#else
  vui32_t tmp;
  const vui32_t expmask = CONST_VINT128_W(0x7fff0000, 0, 0, 0)

  tmp = vec_and_bin128_2_vui32t (f128, expmask);
  result = (vui64_t) vec_sld (tmp, tmp, 10);
#endif
  return result;
}
 * \endcode
 * Which has implementations for POWER9 (and later) and POWER8
 * (and earlier).
 *
 * For POWER9 it generates the
 * VSX Scalar Extract Exponent Quad-Precision instruction.
 * \code
      xsxexpqp v2,v2
 * \endcode
 *
 * \note Should use the intrinsic scalar_extract_exp() here but this is
 * not available until GCC 11. So use in-line assembler until the
 * internsic is available and verified.
 *
 * For POWER8 we generate
 * \code
     addis   r9,r2,.rodata.cst16+0xc0@ha
     addi    r9,r9,.rodata.cst16+0xc0@l
     lvx     v13,0,r9

     xxland  vs34,vs34,vs45
     vsldoi  v2,v2,v2,10
 * \endcode
 * The first 3 instructions above load the constant vector needed
 * by the logic. This constant only needs to be generated once per
 * function and can be shared across operations.
 *
 * Again we use a special transfer function combined with
 * logical AND (vec_and_bin128_2_vui32t()) to transfer the__float128
 * to a vector type and mask off all bits except for the 15-bit
 * exponent. Then we rotate the exponent logically right 48-bit to
 * right justify the exponent in vector doubleword 0.
 * This matches the results of the xsxexpqp instruction.
 *
 * \subsection f128_softfloat_0_0_1 Quad-Precision compares for POWER8
 *
 * IEEE floating-point compare is a bit more complicated than binary
 * integer compare operations. The two main complications are;
 * Not-a-Number (NaN) which IEEE insists are <I>unordered</I>,
 * and signed 0.0 where IEEE insists that -0.0 is equal to +0.0.
 * If you ignore the NaN and signed 0.0 cases you can treat
 * floating-point as signed magnitude binary integers, and
 * use integer compares and boolean logic.
 * Which looks like this:
 *
 * - a \=<sup>f</sup> b \=\= (a \=<sup>s</sup> b)
 * - a \<<sup>f</sup> b \=\=  (a \>\=<sup>s</sup> 0 \& a \<<sup>s</sup> b)
 * \| (a \<<sup>s</sup> 0 \& a \><sup>u</sup> b)
 * - a \<\=<sup>f</sup> b \=\=  (a \>\=<sup>s</sup> 0  \& a \<\=<sup>s</sup> b)
 * \| (a \<<sup>s</sup> 0 \& a \>\=<sup>u</sup> b)
 *
 * Where;
 * \=<sup>f</sup>, \<<sup>f</sup>, and \<\=<sup>f</sup> are the desired
 * floating-point compares,
 * \=<sup>s</sup>, \<<sup>s</sup>, \<\=<sup>s</sup>, \><sup>s</sup>
 * and \>\=<sup>s</sup>, are signed integer compares,
 * and \=<sup>s</sup>, \><sup>u</sup>, and \>\=<sup>u</sup> are unsigned
 * integer compares.
 *
 * \see "Hacker's Delight, 2nd Edition,"
 * Henry S. Warren, Jr, Addison Wesley, 2013.
 * Chapter 17, Floating-point,
 * Section 17-3 Comparing Floating-Point Numbers Using Integer
 * Operations.
 *
 * One key implication of this is that we will need signed and unsigned
 * 128-bit compare operations. Instructions for 128-bit integer
 * compares was added for PowerISA 3.1 (POWER10) but we also need to
 * support POWER8/9. The good news is that PowerISA 2.07B (POWER8)
 * includes Vector Add/Subtract Modulo/Carry/Extend Quadword
 * instructions.
 * Specifically Vector Subtract & write Carry Unsigned Quadword
 * can implement all the unsigned ordered (<. <=, >, >=) compares by
 * manipulating the comparand order and evaluating the carry for
 * 0 or 1.
 *
 * POWER8 also includes vector doubleword integer compare instructions.
 * And the Vector Compare Equal To Unsigned Doubleword instruction can
 * be used with a few additional logical operations to implement 128-bit
 * equal and not equal operations.
 * These operations are already provided by vec_int128_ppc.h.
 *
 * Some additional observations:
 * - The unsigned compare equal can be used for unsigned or signed integers.
 * - (a \>\=<sup>s</sup> 0) \=\= ~(a \<<sup>s</sup> 0).
 *   - So we only need one compare boolean and the binary NOT.
 *   - ((a \>\=<sup>s</sup> 0) \& a \<<sup>s</sup> b)
 *   \| (~(a \>\=<sup>s</sup> 0) \& a \><sup>u</sup> b).
 *   - Now this starts to look like a vector select operation.
 *   - (src1 \& ~mask) \| (src2 \& mask)
 * - (a \>\=<sup>s</sup> 0) is a special case that only depends on
 * the sign-bit.
 *   - A unsigned compare can be used with a slight change,
 *   - Propagating the sign-bit across the (quad)word generates the
 *   same boolean. This is the vec_setb_sq() operation. The __float128
 *   variant is vec_setb_qp()
 *
 * \note The examples that follow, use vector __int128 parameters instead
 * of __binary128 to avoid the hassles of cast incompatibility between
 * scalar __binary128's and vector types. The actual implementations use
 * the xfer functions.
 *
 * \code
vb128_t
test_cmpltf128_v1c (vi128_t vfa128, vi128_t vfb128)
{
  vb128_t altb, agtb;
  vb128_t signbool;
  vb128_t result;

  // Replace (vfa >= 0) with (vfa < 0) == vec_setb_qp (vfa)
  const vui8_t shift = vec_splat_u8 (7);
  vui8_t splat = vec_splat ((vui8_t) vfa128, VEC_BYTE_H);
  signbool = (vb128_t) vec_sra (splat, shift);

  altb = vec_cmpltsq (vfa128, vfb128);
  agtb = vec_cmpgtuq ((vui128_t) vfa128, (vui128_t) vfb128);
  result = (vb128_t) vec_sel ((vui32_t)altb, (vui32_t)agtb, (vui32_t)signbool);
  return result;
}
 * \endcode
 *
 * Now we can tackle the pesky signed 0.0 case. The simplest method is to
 * add another term that test for either a or b is -0.0. This simplifies
 * to just logical a OR b and unsigned compare to -0.0.
 * Which looks like this:
 *
 * - a \=<sup>f</sup> b \=\= (a \=<sup>s</sup> b)
 * \| ((a \| b) \=\= 0x80000000...0)
 * - a \<<sup>f</sup> b \=\=  (a \>\=<sup>s</sup> 0 \& a \<<sup>s</sup> b)
 * \| ((a \<<sup>s</sup> 0 \& a \><sup>u</sup> b)
 * \& ((a \| b) \!\= 0x80000000...0))
 * - a \<\=<sup>f</sup> b \=\=  (a \>\=<sup>s</sup> 0  \& a \<\=<sup>s</sup> b)
 * \| ((a \<<sup>s</sup> 0 \& a \>\=<sup>u</sup> b)
 * \| ((a \| b) \=\= 0x80000000...0))
 *
 * Again we can replace signed compares (a >= 0) and (a < 0) with a
 * single vec_setb_qp() and simplify the boolean logic by using
 * vec_sel().
 * For the  ((a \| b) \!\= 0x80000000...0) term we can save an
 * instruction by replacing vec_cmpneuq() with vec_cmpequq() and
 * replacing the AND operation with AND compliment.
 * \code
vb128_t
test_cmpltf128_v2c (vi128_t vfa128, vi128_t vfb128)
{
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vb128_t altb, agtb, nesm;
  vui32_t or_ab;
  vb128_t signbool;
  vb128_t result;

  // Replace (vfa >= 0) with (vfa < 0) == vec_setb_qp (vfa)
  const vui8_t shift = vec_splat_u8 (7);
  vui8_t splat = vec_splat ((vui8_t) vfa128, VEC_BYTE_H);
  signbool = (vb128_t) vec_sra (splat, shift);

  altb = vec_cmpltsq (vfa128, vfb128);
  agtb = vec_cmpgtuq ((vui128_t) vfa128, (vui128_t) vfb128);

  or_ab = vec_or ((vui32_t) vfa128, (vui32_t) vfb128 );
  // For ne compare eq and and compliment
  nesm = vec_cmpequq ((vui128_t) or_ab, (vui128_t) signmask);
  agtb = (vb128_t) vec_andc ((vui32_t) agtb, (vui32_t) nesm);
  // select altb for 0's and agtb for 1's
  return (vb128_t) vec_sel ((vui32_t)altb, (vui32_t)agtb, (vui32_t)signbool);
}
 * \endcode
 * This sequence runs \~27 instructions when you include the constant loads.
 *
 * An alternative compare method converts both floating-point values
 * in a way that a single (unsigned) integer compare can be used.
 *
 * \code
 // for each comparand
  if (n >= 0)
    n = n + 0x80000000;
  else
    n = -n;
 // Use unsigned integer comparison
 * \endcode
 * An interesting feature of this method is that +0.0 becomes
 * (0x00000000 + 0x80000000 = 0x80000000) and -0.0 becomes
 * (0x80000000 - 0x80000000 = 0x80000000) which effectively
 * converts any -0.0 into +0.0 for comparison. Signed 0.0 solved.
 *
 * Another optimization converts (n \= n + 0x80000000) to
 * (n \= n XOR 0x80000000). Gives the same result and for POWER8
 * a vec_xor() is 2 cycles latency vs 4 cycles for _vec_adduqm().
 *
 * \code
vb128_t
test_cmpltf128_v3d (vui128_t vfa128, vui128_t vfb128)
{
  const vui32_t zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  const vui8_t shift = vec_splat_u8 (7);

  vb128_t result;
  vb128_t age0, bge0;
  vui128_t vra, vrap, vran;
  vui128_t vrb, vrbp, vrbn;
  vui8_t splta, spltb;

  // signbool = vec_setb_qp;
  splta = vec_splat ((vui8_t) vfa128, VEC_BYTE_H);
  age0 = (vb128_t) vec_sra (splta, shift);

  vrap = (vui128_t) vec_xor ((vui32_t) vfa128, signmask);
  vran = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vfa128);
  vra  = (vui128_t) vec_sel ((vui32_t)vrap, (vui32_t)vran, (vui32_t)age0);

  spltb = vec_splat ((vui8_t) vfb128, VEC_BYTE_H);
  bge0 = (vb128_t) vec_sra (spltb, shift);

  vrbp = (vui128_t) vec_xor ((vui32_t) vfb128, signmask);
  vrbn = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vfb128);
  vrb  = (vui128_t) vec_sel ((vui32_t)vrbp, (vui32_t)vrbn, (vui32_t)bge0);

  return vec_cmpltuq (vra, vrb);
}
 * \endcode
 * This sequence runs (approximately) 20 instructions when you include
 * loading the required constants.
 * It also manages to use only splat-immediate forms to load
 * constants and so does not need to establish the TOC pointer nor
 * any address calculations to access constants via load instructions.
 *
 * The next IEEE issue is detecting NaNs and returning <I>unordered</I>
 * status. Adding the following code to a compare operation insures
 * that if either comparand is NaN; false (unordered) is returned for
 * compares (eq, lt, gt).
 *
 * \code
  if (vec_all_isnanf128 (vfa) || vec_all_isnanf128 (vfb))
    return (vb128_t) vec_splat_u32 (0);
 * \endcode
 * The pair of vec_all_isnanf128() operations add significant overhead
 * both in code size (39 instructions) and cycles. This form should
 * only be used if is required for correct results and
 * has not been tested by prior logic in this code path.
 *
 * \note At this point we are not trying to comply with PowerISA by
 * setting any FPSCR bits associated with Quad-Precision compare.
 * If such is required, VXSNAN and/or VXVC can be set
 * using the Move To FPSCR Bit 1 (mtfsb1) instruction.
 *
 * \subsection f128_softfloat_0_0_2 Quad-Precision converts for POWER8
 *
 * IEEE floating-point conversions are also a bit complicated.
 * Dealing with Not-a-Number (NaN), Infinities and subnormal is
 * part of it.
 * But the conversion may also require normalization and rounding
 * depending on element size and types involved.
 * Some examples:
 * - Double precision floats and long long ints can be represented
 * exactly in Quad precision float. But:
 * - Down conversions (to doubleword) from Quad-Precision may require
 *  rounding/truncation.
 *  - Conversions to integer that overflow are given special values.
 * - Conversions between QP and quadword integer may also require
 *  rounding/truncation.
 *   - 128-bit integer values may not fit into the QPs 113-bit
 *   significand.
 *
 * For PowerISA 3.0 (POWER9) includes full hardware instruction
 * support for Quad-Precision, Including:
 * - Conversions between Quad-Precison and Double floating-point
 * (xscvdpqp, xscvqpdp[o]).
 * - Conversions between Quad-Precison and doubleword and word integer
 * (xscvqpsdz, xscvqpswz, xscvqpudz, xscvqpuwz, xscvsdqp, xscvudqp).
 *
 * PowerISA 3.1 (POWER10) includes:
 * - Conversions between Quad-Precison and quadword integer
 * (xscvqpsqz, xscvqpuqz, xscvsqqp, xscvuqqp).
 *
 * For POWER8 (and earlier) we need to do a little more work
 * The general plan for conversion starts by disassembling the input
 * value into its parts and analyze.
 * For signed integer values disassemble usually means sign and
 * unsigned magnitude.
 * Analysis might be a range check or counting leading zeros.
 * For floating point values this is usually sign, exponent, and
 * significand.
 * Analysis usually means determining the data class (NaN, infinity,
 * normal, subnormal, zero) as each requires special handling in the
 * conversion.
 *
 * Conversion involves adjusting the <I>parts</I> as needed the match
 * the type of the result. This is normally only adds and shifts.
 * Finally we need to reassemble the parts based on the result type.
 * For integers this normally just converting the unsigned magnitude
 * to a signed '2's complement value based on the sign of the input.
 * For floating-point this requires merging the sign bit with the
 * (adjusted) significand and merging that with the (adjusted)
 * exponent.
 *
 * The good news is that all of the require operations are already
 * available in <B>altivec.h</B> or PVECLIB.
 *
 * \subsubsection f128_softfloat_0_0_2_0 Convert Double-Precision to Quad-Precision
 *
 * This is one of the simpler conversions as the conversion is always
 * exact (no rounding/truncation is required, and no overflow is
 * possible). The process starts with disassembling the
 * double-precision value.
 * \code
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);

  f64[VEC_DW_L] = 0.0; // clear the right most element to zero.
  // Extract the exponent, significand, and sign bit.
  d_exp = vec_xvxexpdp (f64);
  d_sig = vec_xvxsigdp (f64);
  q_sign = vec_and ((vui32_t) f64, signmask);
 * \endcode
 * We insure that the low-order doubleword of the vector f64 is zeroed.
 * This is necessary for then we normalize the 128-bit significand for
 * the quad-precision result.
 * The operations vec_xvxexpdp() and vec_xvxsigdp() are provided by
 * vec_f64_ppc.h supporting bot the POWER9 instruction and equivalent
 * implementation for POWER8.
 * And finally we extract the sign-bit. We can't use the copysign()
 * here due to the difference in type.
 *
 * Now we analyze the data class of the double-precision input.
 * \code
  if (vec_all_isfinitef64 (f64))
    { // Not NaN or Inf
      if (vec_all_isnormalf64 (vec_splat (f64, VEC_DW_H)))
	{
	// ... adjust exponent and expand significand
	}
      else
	{ // Must be zero or subnormal
	  if (vec_all_iszerof64 (f64))
	    {
	      // ... copy zero exponent and significand
	    }
	  else
	    { // Must be subnormal
	      // ... normalize signifcand for QP and adjust exponent
	    }
	}
    }
  else
    { // isinf or isnan.
      // ... set exponent to QP max and expand significand
    }
 * \endcode
 * This code is arranged with an eye to the most common cases and
 * specifics of the conversion required by each data class.
 * The operations vec_all_isfinitef64(), vec_all_isnormalf64() and
 * vec_all_iszerof64() are provided by
 * vec_f64_ppc.h supporting both the POWER9 instruction and equivalent
 * implementation for POWER8.
 *
 * The normal case requires shifting the significand and adjusting the exponent.
 * \code
  const vui64_t exp_delta = (vui64_t) CONST_VINT64_DW( (0x3fff - 0x3ff), 0 );
          ...
	  q_sig = vec_srqi ((vui128_t) d_sig, 4);
	  q_exp = vec_addudm (d_exp, exp_delta);
 * \endcode
 * The double significand has the fraction bits starting a bit-12 and
 * the implied '1' in bit-11. For quad-precision we need to shift this
 * right 4-bits to align the fraction to start in bit-16.
 * We need a quadword shift as the significand will now extend into
 * the high order bits of the second (low order) doubleword.
 * To adjust the exponent we need to convert the double biased exponent
 * (1 to 2046) into unbiased (-1022 to +1023) by subtracting the
 * exponent bias (+1023 or 0x3ff) value. Then we can add the
 * quad-precision exponent bias (+16383 or 0x3fff) to compute the
 * final exponent.
 * We can combine the bias difference into a single constant
 * (0x3fff - 0x3ff) and only need a single add at runtime.
 *
 * The operations vec_srqi() is provided by vec_int128_ppc.h and
 * vec_addudm() is provided by vec_int64_ppc.h.
 * We use PVECLIB operations here to insure that this code is safe to
 * use with older compilers and pre-POWER8 processors.
 *
 * The zero case requires setting the quad-precision significand and
 * exponent to zero.
 * \code
	      q_sig = (vui128_t) d_sig;
	      q_exp = (vui64_t) d_exp;
 * \endcode
 * We know that the double significand and exponent are zero, so just
 * assign them to the quad-precision parts. The sign bit will applied
 * later with the common insert exponent code.
 *
 * The subnormal case is a bit more complicated.
 * The tricky part is while the double-precision value is subnormal
 * the equivalent quad-precision value is not.
 * So we need to normalize the significand and compute a new exponent.
 * \code
    // Need to adjust the quad exponent by the f64 denormal exponent
    // (-1023) knowing that the f64 sig will have at least 12 leading '0's
    vui64_t q_denorm = (vui64_t) CONST_VINT64_DW( (16383 - (1023 -12)), 0 );
    vui64_t f64_clz;
    f64_clz = vec_clzd (d_sig);
    d_sig = vec_vsld (d_sig, f64_clz);
    q_exp = vec_subudm (q_denorm, f64_clz);
    q_sig = vec_srqi ((vui128_t) d_sig, 15);
 * \endcode
 * We use a doubleword count leading zeros (ctz) and shift left to
 * normalize the significand so that the first '1'-bit moves to bit-0.
 * Then we compute the quad-precision exponent by subtracting the ctz
 * value from a constant (16383 - (1023 -12)).
 * This represents the quad-precision exponent bias, minus the
 * double-precsion exponent bias, minus the minimum leading zero count
 * for the double-precision subnormal significand.
 *
 * The NaN/Infinity case requires shifting the significand and setting
 * the exponent to quad-precision max.
 * \code
      q_sig = vec_srqi ((vui128_t) d_sig, 4);
      q_exp = (vui64_t) CONST_VINT64_DW(0x7fff, 0);
 * \endcode
 * We need this shift as NaN has a non-zero significand and it might
 * be nonzero in one of the low order bits.
 * Separating out the infinity case (where the significand is zero)
 * is not worth the extra (isnan) test to avoid the shift.
 *
 * Now all the parts are converted to quad-precision, we are ready to
 * reassemble the QP result.
 * \code
  // Copy Sign-bit to QP significand before insert.
  q_sig = (vui128_t) vec_or ((vui32_t) q_sig, q_sign);
  // Insert exponent into significand to complete conversion to QP
  result = vec_xsiexpqp (q_sig, q_exp);
 * \endcode
 *
 * Putting this all together we get something like:
 * \code
  vui64_t d_exp, d_sig, q_exp;
  vui128_t q_sig;
  vui32_t q_sign;
  const vui64_t exp_delta = (vui64_t) CONST_VINT64_DW( (0x3fff - 0x3ff), 0 );
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);

  f64[VEC_DW_L] = 0.0; // clear the right most element to zero.
  // Extract the exponent, significand, and sign bit.
  d_exp = vec_xvxexpdp (f64);
  d_sig = vec_xvxsigdp (f64);
  q_sign = vec_and ((vui32_t) f64, signmask);
  if (vec_all_isfinitef64 (f64))
    { // Not NaN or Inf
      if (vec_all_isnormalf64 (vec_splat (f64, VEC_DW_H)))
	{
	  q_sig = vec_srqi ((vui128_t) d_sig, 4);
	  q_exp = vec_addudm (d_exp, exp_delta);
	}
      else
	{ // zero or subnormal
	  if (vec_all_iszerof64 (f64))
	    {
	      q_sig = (vui128_t) d_sig;
	      q_exp = (vui64_t) d_exp;
	    }
	  else
	    { // Must be subnormal
	      vui64_t q_denorm = (vui64_t) CONST_VINT64_DW( (0x3fff - 1023 -12), 0 );
	      vui64_t f64_clz;
	      f64_clz = vec_clzd (d_sig);
	      d_sig = vec_vsld (d_sig, f64_clz);
	      q_exp = vec_subudm (q_denorm, f64_clz);
	      q_sig = vec_srqi ((vui128_t) d_sig, 15);
	    }
	}
    }
  else
    { // isinf or isnan.
      q_sig = vec_srqi ((vui128_t) d_sig, 4);
      q_exp = (vui64_t) CONST_VINT64_DW(0x7fff, 0);
    }
  // Copy Sign-bit to QP significand before insert.
  q_sig = (vui128_t) vec_or ((vui32_t) q_sig, q_sign);
  // Insert exponent into significand to complete conversion to QP
  result = vec_xsiexpqp (q_sig, q_exp);
 * \endcode
 *
 * At this stage we have a functionally correct implementation and now
 * we can look for opportunities for optimization.
 * One issue is the generated code is fairly large
 * (~436 bytes and ~100 instructions).
 * For POWER8 the data class predicates (vec_all_isfinitef64, etc)
 * each require one or more vector constant loads and bit mask
 * operations before the associated vector compares.
 * Also the extract significand operation requires the equivalent of
 * isnormal (with two vector compares) as preparation for conditionally
 * restoring the implied (hidden) bit.
 *
 * By testing the extracted parts (exponent and significand) directly
 * we can avoid simplify the data class compare logic and elliminate
 * some (redundant) vector constant loads. For example:
 * \code
  const vui64_t d_naninf = (vui64_t) CONST_VINT64_DW( 0x7ff, 0 );
  const vui64_t d_denorm = (vui64_t) CONST_VINT64_DW( 0, 0 );
  // ...
  // The extract sig operation has already tested for finite/subnormal.
  // So avoid testing isfinite/issubnormal again by simply testing
  // the extracted exponent.
  if (__builtin_expect (!vec_cmpud_all_eq (d_exp, d_naninf), 1))
    { // Not Nan or Inf
      if (__builtin_expect (!vec_cmpud_all_eq (d_exp, d_denorm), 1))
	{
	// ... adjust exponent and expand significand
	}
      else
	{ // Must be zero or subnormal
	  if (vec_cmpud_all_eq (d_sig, d_denorm))
	    {
	      // ... copy zero exponent and significand
	    }
	  else
	    { // Must be subnormal
	      // ... normalize signifcand for QP and adjust exponent
	    }
	}
    }
  else
    { // isinf or isnan.
      // ... set exponent to QP max and expand significand
    }
 * \endcode
 * The implementation based on this logic is smaller
 * (~300 bytes and ~75 instructions).
 * Performance results TBD.
 *
 * \subsubsection f128_softfloat_0_0_2_1 Convert Doubleword integer to Quad-Precision
 * Converting binary integers to floating point is simpler as there
 * are fewer data classes to deal with. Basically zero and non-zero
 * numbers (no signed 0s, infinities or NaNs).
 * Also the conversion from 64-bit integers to 128-bit floating-point
 * is always exact (there is no rounding).
 *
 * Unsigned doubleword is the simplest case. We only need to test for
 * binary zero. If zero just return a QP +0.0 constant. Otherwise
 * we can treat the binary magnitude as a denormalized number and
 * normalize it. The binary zero test and processing looks like this:
 * \code
  int64[VEC_DW_L] = 0UL; // clear the right most element to zero.
  // Quick test for 0UL as this case requires a special exponent.
  d_sig = int64;
  if (vec_cmpud_all_eq (int64, d_zero))
    { // Zero sign, exponent and significand.
      result = vec_xfer_vui64t_2_bin128 (d_zero);
    }
  else ...
 * \endcode
 * For the non-zero case we assume the binary point follows the unit
 * bit (bit-63) of the 64-bit magnitude.
 * Then we use count leading zeros to find the first significant bit.
 * This count is used to normalize/shift (left justify) the
 * magnitude and adjust the QP exponent to reflect the binary
 * point following the unit (original doubleword bit 63) bit.
 * So far we are using only doubleword data and instructions.
 * \code
    { // We need to produce a normalized QP, so we treat the integer
      // like a denormal, then normalize it.
      // Start with the quad exponent bias + 63 then subtract the count of
      // leading '0's. The 64-bit sig can have 0-63 leading '0's.
      vui64_t q_expm = (vui64_t) CONST_VINT64_DW((0x3fff + 63), 0 );
      vui64_t i64_clz = vec_clzd (int64);
      d_sig = vec_vsld (int64, i64_clz);
      q_exp = vec_subudm (q_expm, i64_clz);
      q_sig = vec_srqi ((vui128_t) d_sig, 15);
      result = vec_xsiexpqp (q_sig, q_exp);
    }
 * \endcode
 * The high order bit (after normalization) will become the
 * <I>implicit</I> (hidden) bit in QP format. So we shift the quadword
 * right 15-bits to become the QP significand.
 * This shift includes the low order 64-bits we zeroed out early on
 * and zeros out the sign-bit as a bonus.
 * Finally we use vec_xsiexpqp() to merge the adjusted exponent and
 * significand.
 *
 * The signed doubleword conversion is bit more complicated.
 * We deal with zero case in the same way. Otherwise
 * we need split the signed doubleword into a sign-bit and unsigned
 * 63-bit magnitude. Which looks something like this:
 * \code
  const vui64_t d_zero = (vui64_t) CONST_VINT64_DW( 0, 0 );
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  ...
      // Convert 2s complement to signed magnitude form.
      q_sign = vec_and ((vui32_t) int64, signmask);
      d_neg  = vec_subudm (d_zero, (vui64_t)int64);
      d_sign = (vui64_t) vec_cmpequd ((vui64_t) q_sign, (vui64_t) signmask);
      // Select the original int64 if positive otherwise the negated value.
      d_sig = (vui64_t) vec_sel ((vui32_t) int64, (vui32_t) d_neg, (vui32_t) d_sign);
 * \endcode
 * The normalization process is basically the same as unsigned but we
 * merge the sign-bit into the significant before inserting the
 * exponent.
 * \code
      // Count leading zeros and normalize.
      i64_clz = vec_clzd (d_sig);
      d_sig = vec_vsld (d_sig, i64_clz);
      q_exp = vec_subudm (q_expm, i64_clz);
      q_sig = vec_srqi ((vui128_t) d_sig, 15);
      // Copy Sign-bit to QP significand before insert.
      q_sig = (vui128_t) vec_or ((vui32_t) q_sig, q_sign);
      // Insert exponent into significand to complete conversion to QP
      result = vec_xsiexpqp (q_sig, q_exp);
 * \endcode
 *
 *
 * \subsubsection f128_softfloat_0_0_2_2 Convert Quadword integer to Quad-Precision
 * TBD
 *
 * \note See
 * <a href="https://www.exploringbinary.com/gcc-avoids-double-rounding-errors-with-round-to-odd/">
*  GCC Avoids Double Rounding Errors With Round-To-Odd</a>
 *
 * \subsubsection f128_softfloat_0_0_2_x Convert Quad-Precision to Double-Precision
 * TBD
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

///@cond INTERNAL
static inline __binary128 vec_xfer_vui32t_2_bin128 (vui32_t f128);
static inline int vec_all_isnanf128 (__binary128 f128);
static inline vb128_t vec_isnanf128 (__binary128 f128);
static inline vb128_t vec_isunorderedf128 (__binary128 vfa, __binary128 vfb);
static inline vb128_t vec_setb_qp (__binary128 f128);
static inline __binary128 vec_xsiexpqp (vui128_t sig, vui64_t exp);
///@endcond

 /** \brief Select and Transfer from one of two __binary128 scalars
  * under a 128-bit mask. The result is a __binary128 of the selected
  * value.
 *
 *  The compiler does not allow direct transfer (assignment or type
 *  cast) between __binary128 (__float128) scalars and vector types.
 *  This despite the fact the the ABI and ISA require __binary128 in
 *  vector registers (VRs).
 *
 *  \note this function uses a union to effect the (logical) transfer.
 *  The compiler should not generate any code for this.
 *
 *  @param vfa a __binary128 floating point scalar value.
 *  @param vfb a __binary128 floating point scalar value.
 *  @param mask a vector bool __int128
 *  @return The bit of vfa or vfb depending on the mask.
 */
 static inline __binary128
 vec_sel_bin128_2_bin128 (__binary128 vfa, __binary128 vfb, vb128_t mask)
 {
 #if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) && (__GNUC__ > 7) \
      && !defined (_ARCH_PWR9)
   // Work around for GCC PR 100085
   __binary128 result;
 #ifdef __VSX__
   __asm__(
       "xxsel %x0,%x1,%x2,%x3"
       : "=wa" (result)
       : "wa" (vfa), "wa" (vfb), "wa" (mask)
       : );
 #else
   __asm__(
       "vsel %0,%1,%2,%3"
       : "=v" (result)
       : "v" (vfa), "v" (vfb), "v" (mask)
       : );
 #endif
   return result;
 #else
   __VF_128 ua, ub;
   vui32_t result;

   ua.vf1 = vfa;
   ub.vf1 = vfb;

   result = vec_sel (ua.vx4, ub.vx4, (vb32_t) mask);
   return vec_xfer_vui32t_2_bin128 (result);
 #endif
 }

 /** \brief Transfer a quadword from a __binary128 scalar to a vector int
  * and logical AND with a mask.
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
 *  @param mask a vector unsigned int
 *  @return The original value ANDed with mask as a 128-bit vector int.
 */
 static inline vui32_t
 vec_and_bin128_2_vui32t (__binary128 f128, vui32_t mask)
 {
   vui32_t result;
 #if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) && (__GNUC__ > 7) \
    && !defined (_ARCH_PWR9)
   // Work around for GCC PR 100085
 #ifdef __VSX__
   __asm__(
       "xxland %x0,%x1,%x2"
       : "=wa" (result)
       : "wa" (f128), "wa" (mask)
       : );
 #else
   __asm__(
       "vand %0,%1,%2"
       : "=v" (result)
       : "v" (f128), "v" (mask)
       : );
 #endif
 #else
   __VF_128 vunion;

   vunion.vf1 = f128;

   result = (vec_and (vunion.vx4, mask));
 #endif
   return result;
 }

 /** \brief Transfer a quadword from a __binary128 scalar to a vector int
  * and logical AND Compliment with mask.
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
 *  @param mask a vector unsigned int
 *  @return The original value ANDed with mask as a 128-bit vector int.
 */
 static inline vui32_t
 vec_andc_bin128_2_vui32t (__binary128 f128, vui32_t mask)
 {
   vui32_t result;
 #if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) && (__GNUC__ > 7) \
    && !defined (_ARCH_PWR9)
   // Work around for GCC PR 100085
 #ifdef __VSX__
   __asm__(
       "xxlandc %x0,%x1,%x2"
       : "=wa" (result)
       : "wa" (f128), "wa" (mask)
       : );
 #else
   __asm__(
       "vandc %0,%1,%2"
       : "=v" (result)
       : "v" (f128), "v" (mask)
       : );
 #endif
 #else
   __VF_128 vunion;

   vunion.vf1 = f128;

   result = (vec_andc (vunion.vx4, mask));
 #endif
   return result;
 }

 /** \brief Transfer a quadword from a __binary128 scalar to a vector __int128
  * and logical AND Compliment with mask.
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
 *  @param mask a vector unsigned __int128
 *  @return The original value ANDed with mask as a 128-bit vector int.
 */
 static inline vui128_t
 vec_andc_bin128_2_vui128t (__binary128 f128, vui128_t mask)
 {
   vui128_t result;
 #if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) && (__GNUC__ > 7) \
    && !defined (_ARCH_PWR9)
   // Work around for GCC PR 100085
 #ifdef __VSX__
   __asm__(
       "xxlandc %x0,%x1,%x2"
       : "=wa" (result)
       : "wa" (f128), "wa" (mask)
       : );
 #else
   __asm__(
       "vandc %0,%1,%2"
       : "=v" (result)
       : "v" (f128), "v" (mask)
       : );
 #endif
 #else
   __VF_128 vunion;

   vunion.vf1 = f128;
   // vec_andc does not accept vector __int128 type
   result = (vui128_t) vec_andc (vunion.vx4, (vui32_t) mask);
 #endif
   return result;
 }

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
  vui8_t result;
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) && (__GNUC__ > 7) \
    && !defined (_ARCH_PWR9)
  // Work around for GCC PR 100085
#ifdef __VSX__
  __asm__(
      "xxlor %x0,%x1,%x1"
      : "=wa" (result)
      : "wa" (f128)
      : );
#else
  __asm__(
      "vor %0,%1,%1"
      : "=v" (result)
      : "v" (f128)
      : );
#endif
#else
  __VF_128 vunion;

  vunion.vf1 = f128;

  result = (vunion.vx16);
#endif
  return result;
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
  vui32_t result;
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) && (__GNUC__ > 7) \
    && !defined (_ARCH_PWR9)
  // Work around for GCC PR 100085
#ifdef __VSX__
  __asm__(
      "xxlor %x0,%x1,%x1"
      : "=wa" (result)
      : "wa" (f128)
      : );
#else
  __asm__(
      "vor %0,%1,%1"
      : "=v" (result)
      : "v" (f128)
      : );
#endif
#else
  __VF_128 vunion;

  vunion.vf1 = f128;

  result = (vunion.vx4);
#endif
  return result;
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
  vui64_t result;
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) && (__GNUC__ > 7) \
    && !defined (_ARCH_PWR9)
  // Work around for GCC PR 100085
#ifdef __VSX__
  __asm__(
      "xxlor %x0,%x1,%x1"
      : "=wa" (result)
      : "wa" (f128)
      : );
#else
  __asm__(
      "vor %0,%1,%1"
      : "=v" (result)
      : "v" (f128)
      : );
#endif
#else
  __VF_128 vunion;

  vunion.vf1 = f128;

  result = (vunion.vx2);
#endif
  return result;
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
  vui128_t result;
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) && (__GNUC__ > 7) \
    && !defined (_ARCH_PWR9)
  // Work around for GCC PR 100085
#ifdef __VSX__
  __asm__(
      "xxlor %x0,%x1,%x1"
      : "=wa" (result)
      : "wa" (f128)
      : );
#else
  __asm__(
      "vor %0,%1,%1"
      : "=v" (result)
      : "v" (f128)
      : );
#endif
#else
  __VF_128 vunion;

  vunion.vf1 = f128;

  result = (vunion.vx1);
#endif
  return result;
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

  tmp = vec_andc_bin128_2_vui32t (f128, signmask);
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
  vui32_t tmp;
  const vui32_t expmask = CONST_VINT128_W(0x7fff0000, 0, 0, 0);

  tmp = vec_and_bin128_2_vui32t (f128, expmask);
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

  tmp = vec_andc_bin128_2_vui32t (f128, signmask);
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
 *  |power8   |  6-29 | 1/cycle  |
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
#elif defined (_ARCH_PWR8)
  vui32_t tmp;
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  const vui32_t expmask = CONST_VINT128_W(0x7fff0000, 0, 0, 0);

  tmp  = vec_andc_bin128_2_vui32t (f128, signmask);
  return vec_cmpuq_all_gt ((vui128_t) tmp, (vui128_t) expmask);
#else
  vui32_t tmp, tmp2;
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  const vui32_t expmask = CONST_VINT128_W(0x7fff0000, 0, 0, 0);

  tmp  = vec_andc_bin128_2_vui32t (f128, signmask);
  tmp2 = vec_and_bin128_2_vui32t (f128, expmask);
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
  vui32_t tmp;
  const vui32_t expmask = CONST_VINT128_W(0x7fff0000, 0, 0, 0);
  const vui32_t vec_zero = CONST_VINT128_W(0, 0, 0, 0);

  tmp = vec_and_bin128_2_vui32t (f128, expmask);
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
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vui128_t tmp1;

  // Equivalent to vec_absf128 (f128)
  tmp1 = (vui128_t) vec_andc_bin128_2_vui32t (f128, signmask);

  return vec_cmpuq_all_gt ((vui128_t) minnorm, tmp1)
      && !vec_cmpuq_all_eq (tmp1, (vui128_t) vec_zero);
#endif
}

/** \brief Return true if either __float128 value (vra, vrb) is NaN.
 *
 *  A IEEE Binary128 NaN has a exponent of 0x7fff and nonzero
 *  significand. The sign bit is ignored.
 *  For POWER9 and later we use scalar_test_data_class().
 *  Otherwise mask off the sign bit and compare greater than unsigned
 *  quadword to the integer equivalent of Quad-Precision infinity.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal __float128 compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 16-25 | 1/cycle  |
 *  |power9   |   6   | 1/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return a boolean int, true if either __float128 value
 *  (vra, vrb) is NaN.
 */
static inline int
vec_all_isunorderedf128 (__binary128 vfa, __binary128 vfb)
{
  return (vec_all_isnanf128 (vfa) || vec_all_isnanf128 (vfb));
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
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);

  // Equivalent to vec_absf128 (f128)
  tmp2 = (vui64_t) vec_andc_bin128_2_vui32t (f128, signmask);
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

/** \brief Vector Compare Equal (Total-order) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return all '1's,
 *  if vfa == vfb, otherwise all '0's.
 *  Zeros, Infinities and NaN of the same sign compare equal.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision or a VSX Scalar Compare Equal
 *  Quad-Precision instruction.
 *  Otherwise use vector __int128 arithmetic and logical operations
 *  to implement the equivalent Quad-precision floating-point
 *  operation. This leverages operations from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the IEEE standard
 *  relative to signed zero, or NaN comparison.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6     | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return 128-bit vector boolean reflecting __binary128
 *  compare equal.
 */
static inline vb128_t
vec_cmpeqtoqp (__binary128 vfa, __binary128 vfb)
{
  vb128_t result;
#if defined (_ARCH_PWR10) && defined (__FLOAT128__)  && (__GNUC__ >= 10)
  __asm__(
      "xscmpeqqp %0,%1,%2;\n"
      : "=v" (result)
      : "v" (vfa), "v" (vfb)
      : );
#elif defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result= (vb128_t) vec_splat_u32 (0);
  if (vfa == vfb)
    result= (vb128_t) vec_splat_s32 (-1);
#else // defined( _ARCH_PWR8 )
  vui128_t vra, vrb;
  vra = vec_xfer_bin128_2_vui128t (vfa);
  vrb = vec_xfer_bin128_2_vui128t (vfb);
  result = vec_cmpequq ( vra,  vrb );
#endif
  return result;
}

/** \brief Vector Compare Equal (Zero-unordered) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return all '1's,
 *  if vfa == vfb, otherwise all '0's.
 *  Zeros of either sign compare equal.
 *  Infinities and NaNs of the same sign compare equal.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision or a VSX Scalar Compare Equal
 *  Quad-Precision instruction.
 *  Otherwise use vector __int128 arithmetic and logical operations
 *  to implement the equivalent Quad-precision floating-point
 *  operation. This leverages operations from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the IEEE standard
 *  relative NaN comparison.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10    | 1/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return 128-bit vector boolean reflecting __binary128
 *  compare equal.
 */
static inline vb128_t
vec_cmpequzqp (__binary128 vfa, __binary128 vfb)
{
  vb128_t result;
#if defined (_ARCH_PWR10) && defined (__FLOAT128__)  && (__GNUC__ >= 10)
  __asm__(
      "xscmpeqqp %0,%1,%2;\n"
      : "=v" (result)
      : "v" (vfa), "v" (vfb)
      : );
#elif defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result = (vb128_t) vec_splat_u32 (0);
  if (vfa == vfb)
    result = (vb128_t) vec_splat_s32 (-1);
#else // defined( _ARCH_PWR8 )
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vb128_t cmps, or_ab, eq_s;
  vui64_t vra, vrb;

  vra = vec_xfer_bin128_2_vui64t (vfa);
  vrb = vec_xfer_bin128_2_vui64t (vfb);

  or_ab = (vb128_t) vec_or ( vra, vrb );
  eq_s = vec_cmpequq ((vui128_t) or_ab, (vui128_t) signmask);
  cmps = vec_cmpequq ((vui128_t) vra, (vui128_t)vrb);
  result = (vb128_t) vec_or ((vui32_t) cmps, (vui32_t) eq_s);
#endif
  return result;
}

/** \brief Vector Compare Equal (Unordered) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return all '1's,
 *  if vfa == vfb, otherwise all '0's.
 *  Zeros of either sign compare equal.
 *  Infinities of the same sign compare equal.
 *  A NaN in either or both operands compare unequal.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision or (POWER10) VSX Scalar Compare Equal
 *  Quad-Precision instruction.
 *  Otherwise use vector __int128 arithmetic and logical operations
 *  to implement the equivalent Quad-precision floating-point
 *  operation. This leverages operations from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the PowerISA
 *  relative to NaN comparison and setting the FPSCR.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 18-30 | 1/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return 128-bit vector boolean reflecting __binary128
 *  compare equal.
 */
static inline vb128_t
vec_cmpequqp (__binary128 vfa, __binary128 vfb)
{
  vb128_t result;
#if defined (_ARCH_PWR10) && defined (__FLOAT128__)  && (__GNUC__ >= 10)
  __asm__(
      "xscmpeqqp %0,%1,%2;\n"
      : "=v" (result)
      : "v" (vfa), "v" (vfb)
      : );
#elif defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result = (vb128_t) vec_splat_u32 (0);
  if (vfa == vfb)
    result = (vb128_t) vec_splat_s32 (-1);
#else // defined( _ARCH_PWR8 )
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vb128_t cmps, or_ab, eq_s;
  vui64_t vra, vrb;
  vb128_t unordered;

  unordered = vec_isunorderedf128 (vfa, vfb);
  vra = vec_xfer_bin128_2_vui64t (vfa);
  vrb = vec_xfer_bin128_2_vui64t (vfb);

  or_ab = (vb128_t) vec_or ( vra, vrb );
  eq_s = vec_cmpequq ((vui128_t) or_ab, (vui128_t) signmask);
  cmps = vec_cmpequq ((vui128_t) vra, (vui128_t) vrb);
  result = (vb128_t) vec_or ((vui32_t) cmps, (vui32_t) eq_s);
  result = (vb128_t) vec_andc ((vui32_t) result, (vui32_t) unordered);
#endif
  return result;
}

/** \brief Vector Compare Greater Than or Equal (Total-order) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return all '1's,
 *  if vfa >= vfb, otherwise all '0's.
 *  Zeros, Infinities and NaNs are compared as signed values.
 *  Infinities and NaNs have the highest/lowest magnitudes.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision or (POWER10) VSX Scalar Compare Greater
 *  Than Quad-Precision instruction.
 *  Otherwise comparands are converted to unsigned integer magnitudes
 *  before using vector __int128 comparison to implement the equivalent
 *  Quad-precision floating-point operation. This leverages operations
 *  from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the IEEE standard
 *  relative to signed zero, or NaN comparison.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 26-35 | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return 128-bit vector boolean reflecting __binary128
 *  compare greater than or equal.
 */
static inline vb128_t
vec_cmpgetoqp (__binary128 vfa, __binary128 vfb)
{
  vb128_t result;
#if defined (_ARCH_PWR10) && defined (__FLOAT128__)  && (__GNUC__ >= 10)
  __asm__(
      "xscmpgeqp %0,%1,%2;\n"
      : "=v" (result)
      : "v" (vfa), "v" (vfb)
      : );
#elif defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result= (vb128_t) vec_splat_u32 (0);
  if (vfa >= vfb)
    result= (vb128_t) vec_splat_s32 (-1);
#else // defined( _ARCH_PWR8 )
  vui128_t vfa128, vfb128;
  vb128_t altb, agtb;
  vb128_t signbool;
  const vui8_t shift = vec_splat_u8 (7);
  vui8_t splatvfa;

  vfa128 = vec_xfer_bin128_2_vui128t (vfa);
  vfb128 = vec_xfer_bin128_2_vui128t (vfb);

  // Replace (vfa >= 0) with (vfa < 0) == vec_setb_qp (vfa)
  splatvfa = vec_splat ((vui8_t) vfa128, VEC_BYTE_H);
  signbool = (vb128_t) vec_sra (splatvfa, shift);

  agtb = vec_cmpgesq ((vi128_t) vfa128, (vi128_t) vfb128);
  altb = vec_cmpleuq ((vui128_t) vfa128, (vui128_t) vfb128);
  result = (vb128_t) vec_sel ((vui32_t)agtb, (vui32_t)altb, (vui32_t)signbool);
#endif
  return result;
}

/** \brief Vector Compare Greater Than Or Equal (Zero-unordered) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return all '1's,
 *  if vfa >= vfb, otherwise all '0's.
 *  Zeros of either sign are converted to +0.
 *  Infinities and NaNs are compared as signed values.
 *  Infinities and NaNs have the highest/lowest magnitudes.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision or (POWER10) VSX Scalar Compare Greater
 *  Than Quad-Precision instruction.
 *  Otherwise comparands are converted to unsigned integer magnitudes
 *  before using vector __int128 comparison to implement the equivalent
 *  Quad-precision floating-point operation. This leverages operations
 *  from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the IEEE standard
 *  relative to NaN comparison.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 28-37 | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return 128-bit vector boolean reflecting __binary128
 *  compare greater than or equal.
 */
static inline vb128_t
vec_cmpgeuzqp (__binary128 vfa, __binary128 vfb)
{
  vb128_t result;
#if defined (_ARCH_PWR10) && defined (__FLOAT128__)  && (__GNUC__ >= 10)
  __asm__(
      "xscmpgeqp %0,%1,%2;\n"
      : "=v" (result)
      : "v" (vfa), "v" (vfb)
      : );
#elif defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result = (vb128_t) vec_splat_u32 (0);
  if (vfa >= vfb)
    result = (vb128_t) vec_splat_s32 (-1);
#else // defined( _ARCH_PWR8 )
  const vui32_t zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vui128_t vra, vrb;
  vb128_t age0, bge0;
  vui128_t vrap, vran;
  vui128_t vrbp, vrbn;

  vra = vec_xfer_bin128_2_vui128t (vfa);
  vrb = vec_xfer_bin128_2_vui128t (vfb);

  age0 = vec_setb_qp (vfa);
  vrap = (vui128_t) vec_xor ((vui32_t) vra, signmask);
  vran = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vra);
  vra  = (vui128_t) vec_sel ((vui32_t)vrap, (vui32_t)vran, (vui32_t)age0);

  bge0 = vec_setb_qp (vfb);
  vrbp = (vui128_t) vec_xor ((vui32_t) vrb, signmask);
  vrbn = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vrb);
  vrb  = (vui128_t) vec_sel ((vui32_t)vrbp, (vui32_t)vrbn, (vui32_t)bge0);

  result = vec_cmpgeuq ((vui128_t) vra, (vui128_t) vrb);
#endif
  return result;
}

/** \brief Vector Compare Greater Than or Equal (Unordered) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return all '1's,
 *  if vfa >= vfb, otherwise all '0's.
 *  Zeros of either sign are converted to +0.
 *  Infinities of different signs compare ordered.
 *  A NaN in either or both operands compare unordered.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision or (POWER10) VSX Scalar Compare Greater Than
 *  Quad-Precision instruction.
 *  Otherwise comparands are converted to unsigned integer magnitudes
 *  before using vector __int128 comparison to implement the equivalent
 *  Quad-precision floating-point operation. This leverages operations
 *  from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the PowerISA
 *  relative to NaN comparison and setting the FPSCR.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 28-37 | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return 128-bit vector boolean reflecting __binary128
 *  compare greater than or equal.
 */
static inline vb128_t
vec_cmpgeuqp (__binary128 vfa, __binary128 vfb)
{
  vb128_t result;
#if defined (_ARCH_PWR10) && defined (__FLOAT128__)  && (__GNUC__ >= 10)
  __asm__(
      "xscmpgeqp %0,%1,%2;\n"
      : "=v" (result)
      : "v" (vfa), "v" (vfb)
      : );
#elif defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result= (vb128_t) vec_splat_u32 (0);
  if (vfa >= vfb)
    result= (vb128_t) vec_splat_s32 (-1);
#else // defined( _ARCH_PWR8 )
  const vui32_t zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vui128_t vra, vrb;
  vb128_t age0, bge0;
  vui128_t vrap, vran;
  vui128_t vrbp, vrbn;
  vb128_t unordered;

  unordered = vec_isunorderedf128 (vfa, vfb);

  vra = vec_xfer_bin128_2_vui128t (vfa);
  vrb = vec_xfer_bin128_2_vui128t (vfb);

  age0 = vec_setb_qp (vfa);
  vrap = (vui128_t) vec_xor ((vui32_t) vra, signmask);
  vran = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vra);
  vra  = (vui128_t) vec_sel ((vui32_t)vrap, (vui32_t)vran, (vui32_t)age0);

  bge0 = vec_setb_qp (vfb);
  vrbp = (vui128_t) vec_xor ((vui32_t) vrb, signmask);
  vrbn = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vrb);
  vrb  = (vui128_t) vec_sel ((vui32_t)vrbp, (vui32_t)vrbn, (vui32_t)bge0);

  result = vec_cmpgeuq ((vui128_t) vra, (vui128_t) vrb);
  result = (vb128_t) vec_andc ((vui32_t) result, (vui32_t) unordered);
#endif
  return result;
}

/** \brief Vector Compare Greater Than (Total-order) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return all '1's,
 *  if vfa > vfb, otherwise all '0's.
 *  Zeros, Infinities and NaNs are compared as signed values.
 *  Infinities and NaNs have the highest/lowest magnitudes.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision or (POWER10) VSX Scalar Compare Greater
 *  Than Quad-Precision instruction.
 *  Otherwise comparands are converted to unsigned integer magnitudes
 *  before using vector __int128 comparison to implement the equivalent
 *  Quad-precision floating-point operation. This leverages operations
 *  from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the IEEE standard
 *  relative to signed zero, or NaN comparison.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 26-35 | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return 128-bit vector boolean reflecting __binary128
 *  compare greater than.
 */
static inline vb128_t
vec_cmpgttoqp (__binary128 vfa, __binary128 vfb)
{
  vb128_t result;
#if defined (_ARCH_PWR10) && defined (__FLOAT128__)  && (__GNUC__ >= 10)
  __asm__(
      "xscmpgtqp %0,%1,%2;\n"
      : "=v" (result)
      : "v" (vfa), "v" (vfb)
      : );
#elif defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result= (vb128_t) vec_splat_u32 (0);
  if (vfa > vfb)
    result= (vb128_t) vec_splat_s32 (-1);
#else // defined( _ARCH_PWR8 )
  vui128_t vfa128, vfb128;
  vb128_t altb, agtb;
  vb128_t signbool;
  const vui8_t shift = vec_splat_u8 (7);
  vui8_t splatvfa;

  vfa128 = vec_xfer_bin128_2_vui128t (vfa);
  vfb128 = vec_xfer_bin128_2_vui128t (vfb);

  // Replace (vfa >= 0) with (vfa < 0) == vec_setb_qp (vfa)
  splatvfa = vec_splat ((vui8_t) vfa128, VEC_BYTE_H);
  signbool = (vb128_t) vec_sra (splatvfa, shift);

  agtb = vec_cmpgtsq ((vi128_t) vfa128, (vi128_t) vfb128);
  altb = vec_cmpltuq ((vui128_t) vfa128, (vui128_t) vfb128);
  result = (vb128_t) vec_sel ((vui32_t)agtb, (vui32_t)altb, (vui32_t)signbool);
#endif
  return result;
}

/** \brief Vector Compare Greater Than (Zero-unordered) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return all '1's,
 *  if vfa > vfb, otherwise all '0's.
 *  Zeros of either sign are converted to +0.
 *  Infinities and NaNs are compared as signed values.
 *  Infinities and NaNs have the highest/lowest magnitudes.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision or (POWER10) VSX Scalar Compare Greater
 *  Than Quad-Precision instruction.
 *  Otherwise comparands are converted to unsigned integer magnitudes
 *  before using vector __int128 comparison to implement the equivalent
 *  Quad-precision floating-point operation. This leverages operations
 *  from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the IEEE standard
 *  relative to NaN comparison.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 28-37 | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return 128-bit vector boolean reflecting __binary128
 *  compare greater than.
 */
static inline vb128_t
vec_cmpgtuzqp (__binary128 vfa, __binary128 vfb)
{
  vb128_t result;
#if defined (_ARCH_PWR10) && defined (__FLOAT128__)  && (__GNUC__ >= 10)
  __asm__(
      "xscmpgtqp %0,%1,%2;\n"
      : "=v" (result)
      : "v" (vfa), "v" (vfb)
      : );
#elif defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result = (vb128_t) vec_splat_u32 (0);
  if (vfa > vfb)
    result = (vb128_t) vec_splat_s32 (-1);
#else // defined( _ARCH_PWR8 )
  const vui32_t zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vui128_t vra, vrb;
  vb128_t age0, bge0;
  vui128_t vrap, vran;
  vui128_t vrbp, vrbn;

  vra = vec_xfer_bin128_2_vui128t (vfa);
  vrb = vec_xfer_bin128_2_vui128t (vfb);

  age0 = vec_setb_qp (vfa);
  vrap = (vui128_t) vec_xor ((vui32_t) vra, signmask);
  vran = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vra);
  vra  = (vui128_t) vec_sel ((vui32_t)vrap, (vui32_t)vran, (vui32_t)age0);

  bge0 = vec_setb_qp (vfb);
  vrbp = (vui128_t) vec_xor ((vui32_t) vrb, signmask);
  vrbn = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vrb);
  vrb  = (vui128_t) vec_sel ((vui32_t)vrbp, (vui32_t)vrbn, (vui32_t)bge0);

  result = vec_cmpgtuq ((vui128_t) vra, (vui128_t) vrb);
#endif
  return result;
}

/** \brief Vector Compare Greater Than (Unordered) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return all '1's,
 *  if vfa > vfb, otherwise all '0's.
 *  Zeros of either sign are converted to +0.
 *  Infinities of different signs compare ordered.
 *  A NaN in either or both operands compare unordered.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision or (POWER10) VSX Scalar Compare Greater Than
 *  Quad-Precision instruction.
 *  Otherwise comparands are converted to unsigned integer magnitudes
 *  before using vector __int128 comparison to implement the equivalent
 *  Quad-precision floating-point operation. This leverages operations
 *  from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the PowerISA
 *  relative to NaN comparison and setting the FPSCR.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 28-37 | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return 128-bit vector boolean reflecting __binary128
 *  compare greater than.
 */
static inline vb128_t
vec_cmpgtuqp (__binary128 vfa, __binary128 vfb)
{
  vb128_t result;
#if defined (_ARCH_PWR10) && defined (__FLOAT128__)  && (__GNUC__ >= 10)
  __asm__(
      "xscmpgtqp %0,%1,%2;\n"
      : "=v" (result)
      : "v" (vfa), "v" (vfb)
      : );
#elif defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result= (vb128_t) vec_splat_u32 (0);
  if (vfa > vfb)
    result= (vb128_t) vec_splat_s32 (-1);
#else // defined( _ARCH_PWR8 )
  const vui32_t zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vui128_t vra, vrb;
  vb128_t age0, bge0;
  vui128_t vrap, vran;
  vui128_t vrbp, vrbn;
  vb128_t unordered;

  unordered = vec_isunorderedf128 (vfa, vfb);

  vra = vec_xfer_bin128_2_vui128t (vfa);
  vrb = vec_xfer_bin128_2_vui128t (vfb);

  age0 = vec_setb_qp (vfa);
  vrap = (vui128_t) vec_xor ((vui32_t) vra, signmask);
  vran = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vra);
  vra  = (vui128_t) vec_sel ((vui32_t)vrap, (vui32_t)vran, (vui32_t)age0);

  bge0 = vec_setb_qp (vfb);
  vrbp = (vui128_t) vec_xor ((vui32_t) vrb, signmask);
  vrbn = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vrb);
  vrb  = (vui128_t) vec_sel ((vui32_t)vrbp, (vui32_t)vrbn, (vui32_t)bge0);

  result = vec_cmpgtuq ((vui128_t) vra, (vui128_t) vrb);
  result = (vb128_t) vec_andc ((vui32_t) result, (vui32_t) unordered);
#endif
  return result;
}

/** \brief Vector Compare Less Than or Equal (Total-order) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return all '1's,
 *  if vfa <= vfb, otherwise all '0's.
 *  Zeros, Infinities and NaNs are compared as signed values.
 *  Infinities and NaNs have the highest/lowest magnitudes.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision or (POWER10) VSX Scalar Compare Greater
 *  Than Quad-Precision instruction.
 *  Otherwise comparands are converted to unsigned integer magnitudes
 *  before using vector __int128 comparison to implement the equivalent
 *  Quad-precision floating-point operation. This leverages operations
 *  from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the IEEE standard
 *  relative to signed zero, or NaN comparison.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 26-35 | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return 128-bit vector boolean reflecting __binary128
 *  compare less than or equal.
 */
static inline vb128_t
vec_cmpletoqp (__binary128 vfa, __binary128 vfb)
{
  vb128_t result;
#if defined (_ARCH_PWR10) && defined (__FLOAT128__)  && (__GNUC__ >= 10)
  __asm__(
      "xscmpgeqp %0,%2,%1;\n"
      : "=v" (result)
      : "v" (vfa), "v" (vfb)
      : );
#elif defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result= (vb128_t) vec_splat_u32 (0);
  if (vfa <= vfb)
    result= (vb128_t) vec_splat_s32 (-1);
#else // defined( _ARCH_PWR8 )
  vui128_t vfa128, vfb128;
  vb128_t altb, agtb;
  vb128_t signbool;
  const vui8_t shift = vec_splat_u8 (7);
  vui8_t splatvfa;

  vfa128 = vec_xfer_bin128_2_vui128t (vfa);
  vfb128 = vec_xfer_bin128_2_vui128t (vfb);

  // Replace (vfa >= 0) with (vfa < 0) == vec_setb_qp (vfa)
  splatvfa = vec_splat ((vui8_t) vfa128, VEC_BYTE_H);
  signbool = (vb128_t) vec_sra (splatvfa, shift);

  altb = vec_cmplesq ((vi128_t) vfa128, (vi128_t) vfb128);
  agtb = vec_cmpgeuq ((vui128_t) vfa128, (vui128_t) vfb128);
  result = (vb128_t) vec_sel ((vui32_t)altb, (vui32_t)agtb, (vui32_t)signbool);
#endif
  return result;
}

/** \brief Vector Compare Less Than or Equal (Zero-unordered) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return all '1's,
 *  if vfa <= vfb, otherwise all '0's.
 *  Zeros of either sign are converted to +0.
 *  Infinities and NaNs are compared as signed values.
 *  Infinities and NaNs have the highest/lowest magnitudes.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision or (POWER10) VSX Scalar Compare Greater
 *  Than Quad-Precision instruction.
 *  Otherwise comparands are converted to unsigned integer magnitudes
 *  before using vector __int128 comparison to implement the equivalent
 *  Quad-precision floating-point operation. This leverages operations
 *  from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the IEEE standard
 *  relative to NaN comparison.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 28-37 | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return 128-bit vector boolean reflecting __binary128
 *  compare less than or equal.
 */
static inline vb128_t
vec_cmpleuzqp (__binary128 vfa, __binary128 vfb)
{
  vb128_t result;
#if defined (_ARCH_PWR10) && defined (__FLOAT128__)  && (__GNUC__ >= 10)
  __asm__(
      "xscmpgeqp %0,%2,%1;\n"
      : "=v" (result)
      : "v" (vfa), "v" (vfb)
      : );
#elif defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result = (vb128_t) vec_splat_u32 (0);
  if (vfa <= vfb)
    result = (vb128_t) vec_splat_s32 (-1);
#else // defined( _ARCH_PWR8 )
  const vui32_t zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vui128_t vra, vrb;
  vb128_t age0, bge0;
  vui128_t vrap, vran;
  vui128_t vrbp, vrbn;

  vra = vec_xfer_bin128_2_vui128t (vfa);
  vrb = vec_xfer_bin128_2_vui128t (vfb);

  age0 = vec_setb_qp (vfa);
  vrap = (vui128_t) vec_xor ((vui32_t) vra, signmask);
  vran = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vra);
  vra  = (vui128_t) vec_sel ((vui32_t)vrap, (vui32_t)vran, (vui32_t)age0);

  bge0 = vec_setb_qp (vfb);
  vrbp = (vui128_t) vec_xor ((vui32_t) vrb, signmask);
  vrbn = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vrb);
  vrb  = (vui128_t) vec_sel ((vui32_t)vrbp, (vui32_t)vrbn, (vui32_t)bge0);

  result = vec_cmpleuq ((vui128_t) vra, (vui128_t) vrb);
#endif
  return result;
}

/** \brief Vector Compare Less Than or Equal (Unordered) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return all '1's,
 *  if vfa <= vfb, otherwise all '0's.
 *  Zeros of either sign are converted to +0.
 *  Infinities of different signs compare ordered.
 *  A NaN in either or both operands compare unordered.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision or (POWER10) VSX Scalar Compare Greater Than
 *  Quad-Precision instruction.
 *  Otherwise comparands are converted to unsigned integer magnitudes
 *  before using vector __int128 comparison to implement the equivalent
 *  Quad-precision floating-point operation. This leverages operations
 *  from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the PowerISA
 *  relative to NaN comparison and setting the FPSCR.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 28-37 | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return 128-bit vector boolean reflecting __binary128
 *  compare less than or equal.
 */
static inline vb128_t
vec_cmpleuqp (__binary128 vfa, __binary128 vfb)
{
  vb128_t result;
#if defined (_ARCH_PWR10) && defined (__FLOAT128__)  && (__GNUC__ >= 10)
  __asm__(
      "xscmpgeqp %0,%2,%1;\n"
      : "=v" (result)
      : "v" (vfa), "v" (vfb)
      : );
#elif defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result= (vb128_t) vec_splat_u32 (0);
  if (vfa <= vfb)
    result= (vb128_t) vec_splat_s32 (-1);
#else // defined( _ARCH_PWR8 )
  const vui32_t zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vui128_t vra, vrb;
  vb128_t age0, bge0;
  vui128_t vrap, vran;
  vui128_t vrbp, vrbn;
  vb128_t unordered;

  unordered = vec_isunorderedf128 (vfa, vfb);

  vra = vec_xfer_bin128_2_vui128t (vfa);
  vrb = vec_xfer_bin128_2_vui128t (vfb);

  age0 = vec_setb_qp (vfa);
  vrap = (vui128_t) vec_xor ((vui32_t) vra, signmask);
  vran = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vra);
  vra  = (vui128_t) vec_sel ((vui32_t)vrap, (vui32_t)vran, (vui32_t)age0);

  bge0 = vec_setb_qp (vfb);
  vrbp = (vui128_t) vec_xor ((vui32_t) vrb, signmask);
  vrbn = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vrb);
  vrb  = (vui128_t) vec_sel ((vui32_t)vrbp, (vui32_t)vrbn, (vui32_t)bge0);

  result = vec_cmpleuq ((vui128_t) vra, (vui128_t) vrb);
  result = (vb128_t) vec_andc ((vui32_t) result, (vui32_t) unordered);
#endif
  return result;
}

/** \brief Vector Compare Less Than (Total-order) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return all '1's,
 *  if vfa < vfb, otherwise all '0's.
 *  Zeros, Infinities and NaNs are compared as signed values.
 *  Infinities and NaNs have the highest/lowest magnitudes.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision or (POWER10) VSX Scalar Compare Greater
 *  Than Quad-Precision instruction.
 *  Otherwise comparands are converted to unsigned integer magnitudes
 *  before using vector __int128 comparison to implement the equivalent
 *  Quad-precision floating-point operation. This leverages operations
 *  from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the IEEE standard
 *  relative to signed zero, or NaN comparison.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 26-35 | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return 128-bit vector boolean reflecting __binary128
 *  compare less than.
 */
static inline vb128_t
vec_cmplttoqp (__binary128 vfa, __binary128 vfb)
{
  vb128_t result;
#if defined (_ARCH_PWR10) && defined (__FLOAT128__)  && (__GNUC__ >= 10)
  __asm__(
      "xscmpgtqp %0,%2,%1;\n"
      : "=v" (result)
      : "v" (vfa), "v" (vfb)
      : );
#elif defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result= (vb128_t) vec_splat_u32 (0);
  if (vfa < vfb)
    result= (vb128_t) vec_splat_s32 (-1);
#else // defined( _ARCH_PWR8 )
  vui128_t vfa128, vfb128;
  vb128_t altb, agtb;
  vb128_t signbool;
  const vui8_t shift = vec_splat_u8 (7);
  vui8_t splatvfa;

  vfa128 = vec_xfer_bin128_2_vui128t (vfa);
  vfb128 = vec_xfer_bin128_2_vui128t (vfb);

  // Replace (vfa >= 0) with (vfa < 0) == vec_setb_qp (vfa)
  splatvfa = vec_splat ((vui8_t) vfa128, VEC_BYTE_H);
  signbool = (vb128_t) vec_sra (splatvfa, shift);

  altb = vec_cmpltsq ((vi128_t) vfa128, (vi128_t) vfb128);
  agtb = vec_cmpgtuq ((vui128_t) vfa128, (vui128_t) vfb128);
  result = (vb128_t) vec_sel ((vui32_t)altb, (vui32_t)agtb, (vui32_t)signbool);
#endif
  return result;
}

/** \brief Vector Compare Less Than (Zero-unordered) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return all '1's,
 *  if vfa < vfb, otherwise all '0's.
 *  Zeros of either sign are converted to +0.
 *  Infinities and NaNs are compared as signed values.
 *  Infinities and NaNs have the highest/lowest magnitudes.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision or (POWER10) VSX Scalar Compare Greater
 *  Than Quad-Precision instruction.
 *  Otherwise comparands are converted to unsigned integer magnitudes
 *  before using vector __int128 comparison to implement the equivalent
 *  Quad-precision floating-point operation. This leverages operations
 *  from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the IEEE standard
 *  relative to NaN comparison.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 28-37 | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return 128-bit vector boolean reflecting __binary128
 *  compare less than.
 */
static inline vb128_t
vec_cmpltuzqp (__binary128 vfa, __binary128 vfb)
{
  vb128_t result;
#if defined (_ARCH_PWR10) && defined (__FLOAT128__)  && (__GNUC__ >= 10)
  __asm__(
      "xscmpgtqp %0,%2,%1;\n"
      : "=v" (result)
      : "v" (vfa), "v" (vfb)
      : );
#elif defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result = (vb128_t) vec_splat_u32 (0);
  if (vfa < vfb)
    result = (vb128_t) vec_splat_s32 (-1);
#else // defined( _ARCH_PWR8 )
  const vui32_t zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vui128_t vra, vrb;
  vb128_t age0, bge0;
  vui128_t vrap, vran;
  vui128_t vrbp, vrbn;

  vra = vec_xfer_bin128_2_vui128t (vfa);
  vrb = vec_xfer_bin128_2_vui128t (vfb);

  age0 = vec_setb_qp (vfa);
  vrap = (vui128_t) vec_xor ((vui32_t) vra, signmask);
  vran = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vra);
  vra  = (vui128_t) vec_sel ((vui32_t)vrap, (vui32_t)vran, (vui32_t)age0);

  bge0 = vec_setb_qp (vfb);
  vrbp = (vui128_t) vec_xor ((vui32_t) vrb, signmask);
  vrbn = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vrb);
  vrb  = (vui128_t) vec_sel ((vui32_t)vrbp, (vui32_t)vrbn, (vui32_t)bge0);

  result = vec_cmpltuq ((vui128_t) vra, (vui128_t) vrb);
#endif
  return result;
}

/** \brief Vector Compare Less Than (Unordered) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return all '1's,
 *  if vfa < vfb, otherwise all '0's.
 *  Zeros of either sign are converted to +0.
 *  Infinities of different signs compare ordered.
 *  A NaN in either or both operands compare unordered.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision or (POWER10) VSX Scalar Compare Greater Than
 *  Quad-Precision instruction.
 *  Otherwise comparands are converted to unsigned integer magnitudes
 *  before using vector __int128 comparison to implement the equivalent
 *  Quad-precision floating-point operation. This leverages operations
 *  from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the PowerISA
 *  relative to NaN comparison and setting the FPSCR.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 28-37 | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return 128-bit vector boolean reflecting __binary128
 *  compare less than.
 */
static inline vb128_t
vec_cmpltuqp (__binary128 vfa, __binary128 vfb)
{
  vb128_t result;
#if defined (_ARCH_PWR10) && defined (__FLOAT128__)  && (__GNUC__ >= 10)
  __asm__(
      "xscmpgtqp %0,%2,%1;\n"
      : "=v" (result)
      : "v" (vfa), "v" (vfb)
      : );
#elif defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result= (vb128_t) vec_splat_u32 (0);
  if (vfa < vfb)
    result= (vb128_t) vec_splat_s32 (-1);
#else // defined( _ARCH_PWR8 )
  const vui32_t zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vui128_t vra, vrb;
  vb128_t age0, bge0;
  vui128_t vrap, vran;
  vui128_t vrbp, vrbn;
  vb128_t unordered;

  unordered = vec_isunorderedf128 (vfa, vfb);

  vra = vec_xfer_bin128_2_vui128t (vfa);
  vrb = vec_xfer_bin128_2_vui128t (vfb);

  age0 = vec_setb_qp (vfa);
  vrap = (vui128_t) vec_xor ((vui32_t) vra, signmask);
  vran = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vra);
  vra  = (vui128_t) vec_sel ((vui32_t)vrap, (vui32_t)vran, (vui32_t)age0);

  bge0 = vec_setb_qp (vfb);
  vrbp = (vui128_t) vec_xor ((vui32_t) vrb, signmask);
  vrbn = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vrb);
  vrb  = (vui128_t) vec_sel ((vui32_t)vrbp, (vui32_t)vrbn, (vui32_t)bge0);

  result = vec_cmpltuq ((vui128_t) vra, (vui128_t) vrb);
  result = (vb128_t) vec_andc ((vui32_t) result, (vui32_t) unordered);
#endif
  return result;
}

/** \brief Vector Compare Not Equal (Total-order) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return all '1's,
 *  if vfa != vfb, otherwise all '0's.
 *  Zeros, Infinities and NaN of the same sign compare equal.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision or a VSX Scalar Compare Equal
 *  Quad-Precision instruction.
 *  Otherwise use vector __int128 arithmetic and logical operations
 *  to implement the equivalent Quad-precision floating-point
 *  operation. This leverages operations from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the IEEE standard
 *  relative to signed zero, or NaN comparison.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6     | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return 128-bit vector boolean reflecting __binary128
 *  compare not equal.
 */
static inline vb128_t
vec_cmpnetoqp (__binary128 vfa, __binary128 vfb)
{
  vb128_t result;
#if defined (_ARCH_PWR10) && defined (__FLOAT128__)  && (__GNUC__ >= 10)
  __asm__(
      "xscmpeqqp %0,%1,%2;\n"
      : "=v" (result)
      : "v" (vfa), "v" (vfb)
      : );
  result = (vb128_t) vec_nor ((vui32_t) result, (vui32_t) result);
#elif defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result= (vb128_t) vec_splat_u32 (0);
  if (vfa != vfb)
    result= (vb128_t) vec_splat_s32 (-1);
#else // defined( _ARCH_PWR8 )
  vui128_t vra, vrb;
  vra = vec_xfer_bin128_2_vui128t (vfa);
  vrb = vec_xfer_bin128_2_vui128t (vfb);
  result = vec_cmpneuq ( vra,  vrb );
#endif
  return result;
}

/** \brief Vector Compare Not Equal (Zero-unordered) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return all '1's,
 *  if vfa != vfb, otherwise all '0's.
 *  Zeros of either sign compare equal.
 *  Infinities and NaNs of the same sign compare equal.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision or a VSX Scalar Compare Equal
 *  Quad-Precision instruction.
 *  Otherwise use vector __int128 arithmetic and logical operations
 *  to implement the equivalent Quad-precision floating-point
 *  operation. This leverages operations from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the IEEE standard
 *  relative to NaN comparison.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10    | 1/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return 128-bit vector boolean reflecting __binary128
 *  compare not equal.
 */
static inline vb128_t
vec_cmpneuzqp (__binary128 vfa, __binary128 vfb)
{
  vb128_t result;
#if defined (_ARCH_PWR10) && defined (__FLOAT128__)  && (__GNUC__ >= 10)
  __asm__(
      "xscmpeqqp %0,%1,%2;\n"
      : "=v" (result)
      : "v" (vfa), "v" (vfb)
      : );
  result = (vb128_t) vec_nor ((vui32_t) result, (vui32_t) result);
#elif defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result = (vb128_t) vec_splat_u32 (0);
  if (vfa != vfb)
    result = (vb128_t) vec_splat_s32 (-1);
#else // defined( _ARCH_PWR8 )
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vb128_t cmps, or_ab, eq_s;
  vui64_t vra, vrb;

  vra = vec_xfer_bin128_2_vui64t (vfa);
  vrb = vec_xfer_bin128_2_vui64t (vfb);

  or_ab = (vb128_t) vec_or ( vra, vrb );
  eq_s = vec_cmpequq ((vui128_t) or_ab, (vui128_t) signmask);
  cmps = vec_cmpequq ((vui128_t) vra, (vui128_t)vrb);
  result = (vb128_t) vec_nor ((vui32_t) cmps, (vui32_t) eq_s);
#endif
  return result;
}

/** \brief Vector Compare Equal (Unordered) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return all '1's,
 *  if vfa == vfb, otherwise all '0's.
 *  Zeros of either sign compare equal.
 *  Infinities of the same sign compare equal.
 *  A NaN in either or both operands compare unequal.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision or (POWER10) VSX Scalar Compare Equal
 *  Quad-Precision instruction.
 *  Otherwise use vector __int128 arithmetic and logical operations
 *  to implement the equivalent Quad-precision floating-point
 *  operation. This leverages operations from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the PowerISA
 *  relative to NaN comparison and setting the FPSCR.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 18-30 | 1/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return 128-bit vector boolean reflecting __binary128
 *  compare equal.
 */
static inline vb128_t
vec_cmpneuqp (__binary128 vfa, __binary128 vfb)
{
  vb128_t result;
#if defined (_ARCH_PWR10) && defined (__FLOAT128__)  && (__GNUC__ >= 10)
  __asm__(
      "xscmpeqqp %0,%1,%2;\n"
      : "=v" (result)
      : "v" (vfa), "v" (vfb)
      : );
  result = (vb128_t) vec_nor ((vui32_t) result, (vui32_t) result);
#elif defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result = (vb128_t) vec_splat_u32 (0);
  if (vfa != vfb)
    result = (vb128_t) vec_splat_s32 (-1);
#else // defined( _ARCH_PWR8 )
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vb128_t cmps, or_ab, eq_s;
  vui64_t vra, vrb;
  vb128_t unordered;

  unordered = vec_isunorderedf128 (vfa, vfb);

  vra = vec_xfer_bin128_2_vui64t (vfa);
  vrb = vec_xfer_bin128_2_vui64t (vfb);

  or_ab = (vb128_t) vec_or ( vra, vrb );
  eq_s = vec_cmpequq ((vui128_t) or_ab, (vui128_t) signmask);
  cmps = vec_cmpequq ((vui128_t) vra, (vui128_t) vrb);
  result = (vb128_t) vec_nor ((vui32_t) cmps, (vui32_t) eq_s);
  result = (vb128_t) vec_or ((vui32_t) result, (vui32_t) unordered);
#endif
  return result;
}

/** \brief Vector Compare all Equal (Total-order) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return 1,
 *  if vfa == vfb, otherwise 0.
 *  Zeros, Infinities and NaN of the same sign compare equal.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision.
 *  Otherwise use vector __int128 arithmetic and logical operations
 *  to implement the equivalent Quad-precision floating-point
 *  operation. This leverages operations from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the IEEE standard
 *  relative to signed zero, or NaN comparison.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6     | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return int boolean reflecting __binary128 compare equal.
 */
static inline int
vec_cmpqp_all_toeq (__binary128 vfa, __binary128 vfb)
{
  int result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result= (vfa == vfb);
#else // defined( _ARCH_PWR8 )
  vui128_t vra, vrb;
  vra = vec_xfer_bin128_2_vui128t (vfa);
  vrb = vec_xfer_bin128_2_vui128t (vfb);
  result = vec_cmpuq_all_eq ( vra,  vrb );
#endif
  return result;
}

/** \brief Vector Compare all Equal (Zero-unordered) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return 1,
 *  if vfa == vfb, otherwise 0.
 *  Zeros of either sign compare equal.
 *  Infinities and NaNs of the same sign compare equal.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision.
 *  Otherwise use vector __int128 arithmetic and logical operations
 *  to implement the equivalent Quad-precision floating-point
 *  operation. This leverages operations from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the IEEE standard
 *  relative NaN comparison.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10    | 1/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return int boolean reflecting __binary128 compare equal.
 */
static inline int
vec_cmpqp_all_uzeq (__binary128 vfa, __binary128 vfb)
{
  int result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result = (vfa == vfb);
#else // defined( _ARCH_PWR8 )
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vb128_t or_ab;
  vui64_t vra, vrb;

  vra = vec_xfer_bin128_2_vui64t (vfa);
  vrb = vec_xfer_bin128_2_vui64t (vfb);

  or_ab = (vb128_t) vec_or ( vra, vrb );
  result = vec_cmpuq_all_eq ((vui128_t) or_ab, (vui128_t) signmask)
        || vec_cmpuq_all_eq ((vui128_t) vra, (vui128_t)vrb);
#endif
  return result;
}

/** \brief Vector Compare all Equal (Unordered) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return 1,
 *  if vfa == vfb, otherwise 0.
 *  Zeros of either sign compare equal.
 *  Infinities of the same sign compare equal.
 *  A NaN in either or both operands compare unequal.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision.
 *  Otherwise use vector __int128 arithmetic and logical operations
 *  to implement the equivalent Quad-precision floating-point
 *  operation. This leverages operations from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the PowerISA
 *  relative to NaN comparison and setting the FPSCR.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 18-30 | 1/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return int boolean reflecting __binary128 compare equal.
 */
static inline int
vec_cmpqp_all_eq (__binary128 vfa, __binary128 vfb)
{
  int result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result = (vfa == vfb);
#else // defined( _ARCH_PWR8 )
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vb128_t or_ab;
  vui64_t vra, vrb;

  vra = vec_xfer_bin128_2_vui64t (vfa);
  vrb = vec_xfer_bin128_2_vui64t (vfb);

  or_ab = (vb128_t) vec_or ( vra, vrb );
  result = (vec_cmpuq_all_eq ((vui128_t) or_ab, (vui128_t) signmask)
            || vec_cmpuq_all_eq ((vui128_t) vra, (vui128_t)vrb))
         && !vec_all_isunorderedf128 (vfa, vfb);
#endif
  return result;
}

/** \brief Vector Compare all Greater Than Or Equal (Total-order) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return 1 (true),
 *  if vfa >= vfb, otherwise 0 (false).
 *  Zeros, Infinities and NaNs are compared as signed values.
 *  Infinities and NaNs have the highest/lowest magnitudes.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision.
 *  Otherwise comparands are converted to unsigned integer magnitudes
 *  before using vector __int128 comparison to implement the equivalent
 *  Quad-precision floating-point operation. This leverages operations
 *  from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the IEEE standard
 *  relative to signed zero, or NaN comparison.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 26-35 | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return int boolean reflecting __binary128 greater than or equal.
 */
static inline int
vec_cmpqp_all_toge (__binary128 vfa, __binary128 vfb)
{
  int result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result = (vfa >= vfb);
#else // defined( _ARCH_PWR8 )
  vui128_t vfa128, vfb128;
  vb128_t altb, agtb;
  vb128_t signbool;
  const vui8_t shift = vec_splat_u8 (7);
  vui8_t splatvfa;
  vui32_t togt;
  const vui32_t zeros = (vui32_t) vec_splat_u32 (0);

  vfa128 = vec_xfer_bin128_2_vui128t (vfa);
  vfb128 = vec_xfer_bin128_2_vui128t (vfb);

  // Replace (vfa >= 0) with (vfa < 0) == vec_setb_qp (vfa)
  splatvfa = vec_splat ((vui8_t) vfa128, VEC_BYTE_H);
  signbool = (vb128_t) vec_sra (splatvfa, shift);

  agtb = vec_cmpgesq ((vi128_t) vfa128, (vi128_t) vfb128);
  altb = vec_cmpleuq ((vui128_t) vfa128, (vui128_t) vfb128);
  togt = vec_sel ((vui32_t)agtb, (vui32_t)altb, (vui32_t)signbool);
  result = vec_all_ne (togt, zeros);
#endif
  return result;
}

/** \brief Vector Compare all Greater Than Or Equal (Zero-unordered) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return 1 (true),
 *  if vfa >= vfb, otherwise 0 (false).
 *  Zeros of either sign are converted to +0.
 *  Infinities and NaNs are compared as signed values.
 *  Infinities and NaNs have the highest/lowest magnitudes.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision.
 *  Otherwise comparands are converted to unsigned integer magnitudes
 *  before using vector __int128 comparison to implement the equivalent
 *  Quad-precision floating-point operation. This leverages operations
 *  from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the IEEE standard
 *  relative to NaN comparison.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 28-37 | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return int boolean reflecting __binary128 greater than or equal.
 */
static inline int
vec_cmpqp_all_uzge (__binary128 vfa, __binary128 vfb)
{
  int result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result = (vfa >= vfb);
#else // defined( _ARCH_PWR8 )
  const vui32_t zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vui128_t vra, vrb;
  vb128_t age0, bge0;
  vui128_t vrap, vran;
  vui128_t vrbp, vrbn;

  vra = vec_xfer_bin128_2_vui128t (vfa);
  vrb = vec_xfer_bin128_2_vui128t (vfb);

  age0 = vec_setb_qp (vfa);
  vrap = (vui128_t) vec_xor ((vui32_t) vra, signmask);
  vran = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vra);
  vra  = (vui128_t) vec_sel ((vui32_t)vrap, (vui32_t)vran, (vui32_t)age0);

  bge0 = vec_setb_qp (vfb);
  vrbp = (vui128_t) vec_xor ((vui32_t) vrb, signmask);
  vrbn = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vrb);
  vrb  = (vui128_t) vec_sel ((vui32_t)vrbp, (vui32_t)vrbn, (vui32_t)bge0);

  result = vec_cmpuq_all_ge ((vui128_t) vra, (vui128_t) vrb);
#endif
  return result;
}

/** \brief Vector Compare all  Greater Than Or Equal (Unordered) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return 1 (true),
 *  if vfa >= vfb, otherwise 0 (false).
 *  Zeros of either sign are converted to +0.
 *  Infinities of different signs compare ordered.
 *  A NaN in either or both operands compare unordered.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision.
 *  Otherwise comparands are converted to unsigned integer magnitudes
 *  before using vector __int128 comparison to implement the equivalent
 *  Quad-precision floating-point operation. This leverages operations
 *  from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the PowerISA
 *  relative to NaN comparison and setting the FPSCR.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 28-37 | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return int boolean reflecting __binary128 greater than.
 */
static inline int
vec_cmpqp_all_ge (__binary128 vfa, __binary128 vfb)
{
  int result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result = (vfa >= vfb);
#else // defined( _ARCH_PWR8 )
  const vui32_t zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vui128_t vra, vrb;
  vb128_t age0, bge0;
  vui128_t vrap, vran;
  vui128_t vrbp, vrbn;

  vra = vec_xfer_bin128_2_vui128t (vfa);
  vrb = vec_xfer_bin128_2_vui128t (vfb);

  age0 = vec_setb_qp (vfa);
  vrap = (vui128_t) vec_xor ((vui32_t) vra, signmask);
  vran = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vra);
  vra  = (vui128_t) vec_sel ((vui32_t)vrap, (vui32_t)vran, (vui32_t)age0);

  bge0 = vec_setb_qp (vfb);
  vrbp = (vui128_t) vec_xor ((vui32_t) vrb, signmask);
  vrbn = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vrb);
  vrb  = (vui128_t) vec_sel ((vui32_t)vrbp, (vui32_t)vrbn, (vui32_t)bge0);

  result = vec_cmpuq_all_ge ((vui128_t) vra, (vui128_t) vrb)
         && !vec_all_isunorderedf128 (vfa, vfb);
#endif
  return result;
}

/** \brief Vector Compare all Greater Than (Total-order) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return 1 (true),
 *  if vfa > vfb, otherwise 0 (false).
 *  Zeros, Infinities and NaNs are compared as signed values.
 *  Infinities and NaNs have the highest/lowest magnitudes.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision.
 *  Otherwise comparands are converted to unsigned integer magnitudes
 *  before using vector __int128 comparison to implement the equivalent
 *  Quad-precision floating-point operation. This leverages operations
 *  from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the IEEE standard
 *  relative to signed zero, or NaN comparison.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 26-35 | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return int boolean reflecting __binary128 greater than.
 */
static inline int
vec_cmpqp_all_togt (__binary128 vfa, __binary128 vfb)
{
  int result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result = (vfa > vfb);
#else // defined( _ARCH_PWR8 )
  vui128_t vfa128, vfb128;
  vb128_t altb, agtb;
  vb128_t signbool;
  const vui8_t shift = vec_splat_u8 (7);
  vui8_t splatvfa;
  vui32_t togt;
  const vui32_t zeros = (vui32_t) vec_splat_u32 (0);

  vfa128 = vec_xfer_bin128_2_vui128t (vfa);
  vfb128 = vec_xfer_bin128_2_vui128t (vfb);

  // Replace (vfa >= 0) with (vfa < 0) == vec_setb_qp (vfa)
  splatvfa = vec_splat ((vui8_t) vfa128, VEC_BYTE_H);
  signbool = (vb128_t) vec_sra (splatvfa, shift);

  agtb = vec_cmpgtsq ((vi128_t) vfa128, (vi128_t) vfb128);
  altb = vec_cmpltuq ((vui128_t) vfa128, (vui128_t) vfb128);
  togt = vec_sel ((vui32_t)agtb, (vui32_t)altb, (vui32_t)signbool);
  result = vec_all_ne (togt, zeros);
#endif
  return result;
}

/** \brief Vector Compare all Greater Than (Zero-unordered) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return 1 (true),
 *  if vfa > vfb, otherwise 0 (false).
 *  Zeros of either sign are converted to +0.
 *  Infinities and NaNs are compared as signed values.
 *  Infinities and NaNs have the highest/lowest magnitudes.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision.
 *  Otherwise comparands are converted to unsigned integer magnitudes
 *  before using vector __int128 comparison to implement the equivalent
 *  Quad-precision floating-point operation. This leverages operations
 *  from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the IEEE standard
 *  relative to NaN comparison.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 28-37 | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return int boolean reflecting __binary128 greater than.
 */
static inline int
vec_cmpqp_all_uzgt (__binary128 vfa, __binary128 vfb)
{
  int result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result = (vfa > vfb);
#else // defined( _ARCH_PWR8 )
  const vui32_t zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vui128_t vra, vrb;
  vb128_t age0, bge0;
  vui128_t vrap, vran;
  vui128_t vrbp, vrbn;

  vra = vec_xfer_bin128_2_vui128t (vfa);
  vrb = vec_xfer_bin128_2_vui128t (vfb);

  age0 = vec_setb_qp (vfa);
  vrap = (vui128_t) vec_xor ((vui32_t) vra, signmask);
  vran = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vra);
  vra  = (vui128_t) vec_sel ((vui32_t)vrap, (vui32_t)vran, (vui32_t)age0);

  bge0 = vec_setb_qp (vfb);
  vrbp = (vui128_t) vec_xor ((vui32_t) vrb, signmask);
  vrbn = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vrb);
  vrb  = (vui128_t) vec_sel ((vui32_t)vrbp, (vui32_t)vrbn, (vui32_t)bge0);

  result = vec_cmpuq_all_gt ((vui128_t) vra, (vui128_t) vrb);
#endif
  return result;
}

/** \brief Vector Compare all Greater Than (Unordered) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return 1 (true),
 *  if vfa > vfb, otherwise 0 (false).
 *  Zeros of either sign are converted to +0.
 *  Infinities of different signs compare ordered.
 *  A NaN in either or both operands compare unordered.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision.
 *  Otherwise comparands are converted to unsigned integer magnitudes
 *  before using vector __int128 comparison to implement the equivalent
 *  Quad-precision floating-point operation. This leverages operations
 *  from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the PowerISA
 *  relative to NaN comparison and setting the FPSCR.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 28-37 | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return int boolean reflecting __binary128 greater than.
 */
static inline int
vec_cmpqp_all_gt (__binary128 vfa, __binary128 vfb)
{
  int result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result = (vfa > vfb);
#else // defined( _ARCH_PWR8 )
  const vui32_t zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vui128_t vra, vrb;
  vb128_t age0, bge0;
  vui128_t vrap, vran;
  vui128_t vrbp, vrbn;

  vra = vec_xfer_bin128_2_vui128t (vfa);
  vrb = vec_xfer_bin128_2_vui128t (vfb);

  age0 = vec_setb_qp (vfa);
  vrap = (vui128_t) vec_xor ((vui32_t) vra, signmask);
  vran = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vra);
  vra  = (vui128_t) vec_sel ((vui32_t)vrap, (vui32_t)vran, (vui32_t)age0);

  bge0 = vec_setb_qp (vfb);
  vrbp = (vui128_t) vec_xor ((vui32_t) vrb, signmask);
  vrbn = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vrb);
  vrb  = (vui128_t) vec_sel ((vui32_t)vrbp, (vui32_t)vrbn, (vui32_t)bge0);

  result = vec_cmpuq_all_gt ((vui128_t) vra, (vui128_t) vrb)
         && !vec_all_isunorderedf128 (vfa, vfb);
#endif
  return result;
}
/** \brief Vector Compare All Less Than Or Equal (Total-order) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return 1 (true),
 *  if vfa <= vfb, otherwise 0 (false).
 *  Zeros, Infinities and NaNs are compared as signed values.
 *  Infinities and NaNs have the highest/lowest magnitudes.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision.
 *  Otherwise comparands are converted to unsigned integer magnitudes
 *  before using vector __int128 comparison to implement the equivalent
 *  Quad-precision floating-point operation. This leverages operations
 *  from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the IEEE standard
 *  relative to signed zero, or NaN comparison.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 26-35 | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return int boolean reflecting __binary128 compare less than or equal.
 */
static inline int
vec_cmpqp_all_tole (__binary128 vfa, __binary128 vfb)
{
  int result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result = (vfa <= vfb);
#else // defined( _ARCH_PWR8 )
  vui128_t vfa128, vfb128;
  vb128_t altb, agtb;
  vb128_t signbool;
  const vui8_t shift = vec_splat_u8 (7);
  vui8_t splatvfa;
  vui32_t tolt;
  const vui32_t zeros = (vui32_t) vec_splat_u32 (0);

  vfa128 = vec_xfer_bin128_2_vui128t (vfa);
  vfb128 = vec_xfer_bin128_2_vui128t (vfb);

  // Replace (vfa >= 0) with (vfa < 0) == vec_setb_qp (vfa)
  splatvfa = vec_splat ((vui8_t) vfa128, VEC_BYTE_H);
  signbool = (vb128_t) vec_sra (splatvfa, shift);

  altb = vec_cmplesq ((vi128_t) vfa128, (vi128_t) vfb128);
  agtb = vec_cmpgeuq ((vui128_t) vfa128, (vui128_t) vfb128);
  tolt = vec_sel ((vui32_t)altb, (vui32_t)agtb, (vui32_t)signbool);
  result = vec_all_ne (tolt, zeros);
#endif
  return result;
}

/** \brief Vector Compare all Less Than Or Equal (Zero-unordered) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return 1 (true),
 *  if vfa <= vfb, otherwise 0 (false).
 *  Zeros of either sign are converted to +0.
 *  Infinities and NaNs are compared as signed values.
 *  Infinities and NaNs have the highest/lowest magnitudes.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision.
 *  Otherwise comparands are converted to unsigned integer magnitudes
 *  before using vector __int128 comparison to implement the equivalent
 *  Quad-precision floating-point operation. This leverages operations
 *  from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the IEEE standard
 *  relative to NaN comparison.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 28-37 | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return int boolean reflecting __binary128 less than or equal.
 */
static inline int
vec_cmpqp_all_uzle (__binary128 vfa, __binary128 vfb)
{
  int result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result = (vfa <= vfb);
#else // defined( _ARCH_PWR8 )
  const vui32_t zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vui128_t vra, vrb;
  vb128_t age0, bge0;
  vui128_t vrap, vran;
  vui128_t vrbp, vrbn;

  vra = vec_xfer_bin128_2_vui128t (vfa);
  vrb = vec_xfer_bin128_2_vui128t (vfb);

  age0 = vec_setb_qp (vfa);
  vrap = (vui128_t) vec_xor ((vui32_t) vra, signmask);
  vran = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vra);
  vra  = (vui128_t) vec_sel ((vui32_t)vrap, (vui32_t)vran, (vui32_t)age0);

  bge0 = vec_setb_qp (vfb);
  vrbp = (vui128_t) vec_xor ((vui32_t) vrb, signmask);
  vrbn = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vrb);
  vrb  = (vui128_t) vec_sel ((vui32_t)vrbp, (vui32_t)vrbn, (vui32_t)bge0);

  result = vec_cmpuq_all_le ((vui128_t) vra, (vui128_t) vrb);
#endif
  return result;
}

/** \brief Vector Compare all Less Than Or Equal (Unordered) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return 1 (true),
 *  if vfa <= vfb, otherwise 0 (false).
 *  Zeros of either sign are converted to +0.
 *  Infinities of different signs compare ordered.
 *  A NaN in either or both operands compare unordered.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision.
 *  Otherwise comparands are converted to unsigned integer magnitudes
 *  before using vector __int128 comparison to implement the equivalent
 *  Quad-precision floating-point operation. This leverages operations
 *  from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the PowerISA
 *  relative to NaN comparison and setting the FPSCR.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 28-37 | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return int boolean reflecting __binary128 less than or equal.
 */
static inline int
vec_cmpqp_all_le (__binary128 vfa, __binary128 vfb)
{
  int result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result = (vfa <= vfb);
#else // defined( _ARCH_PWR8 )
  const vui32_t zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vui128_t vra, vrb;
  vb128_t age0, bge0;
  vui128_t vrap, vran;
  vui128_t vrbp, vrbn;

  vra = vec_xfer_bin128_2_vui128t (vfa);
  vrb = vec_xfer_bin128_2_vui128t (vfb);

  age0 = vec_setb_qp (vfa);
  vrap = (vui128_t) vec_xor ((vui32_t) vra, signmask);
  vran = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vra);
  vra  = (vui128_t) vec_sel ((vui32_t)vrap, (vui32_t)vran, (vui32_t)age0);

  bge0 = vec_setb_qp (vfb);
  vrbp = (vui128_t) vec_xor ((vui32_t) vrb, signmask);
  vrbn = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vrb);
  vrb  = (vui128_t) vec_sel ((vui32_t)vrbp, (vui32_t)vrbn, (vui32_t)bge0);

  result = vec_cmpuq_all_le ((vui128_t) vra, (vui128_t) vrb)
         && !vec_all_isunorderedf128 (vfa, vfb);
#endif
  return result;
}

/** \brief Vector Compare All Less Than (Total-order) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return 1 (true),
 *  if vfa < vfb, otherwise 0 (false).
 *  Zeros, Infinities and NaNs are compared as signed values.
 *  Infinities and NaNs have the highest/lowest magnitudes.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision.
 *  Otherwise comparands are converted to unsigned integer magnitudes
 *  before using vector __int128 comparison to implement the equivalent
 *  Quad-precision floating-point operation. This leverages operations
 *  from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the IEEE standard
 *  relative to signed zero, or NaN comparison.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 26-35 | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return int boolean reflecting __binary128 compare less than.
 */
static inline int
vec_cmpqp_all_tolt (__binary128 vfa, __binary128 vfb)
{
  int result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result = (vfa < vfb);
#else // defined( _ARCH_PWR8 )
  vui128_t vfa128, vfb128;
  vb128_t altb, agtb;
  vb128_t signbool;
  const vui8_t shift = vec_splat_u8 (7);
  vui8_t splatvfa;
  vui32_t tolt;
  const vui32_t zeros = (vui32_t) vec_splat_u32 (0);

  vfa128 = vec_xfer_bin128_2_vui128t (vfa);
  vfb128 = vec_xfer_bin128_2_vui128t (vfb);

  // Replace (vfa >= 0) with (vfa < 0) == vec_setb_qp (vfa)
  splatvfa = vec_splat ((vui8_t) vfa128, VEC_BYTE_H);
  signbool = (vb128_t) vec_sra (splatvfa, shift);

  altb = vec_cmpltsq ((vi128_t) vfa128, (vi128_t) vfb128);
  agtb = vec_cmpgtuq ((vui128_t) vfa128, (vui128_t) vfb128);
  tolt = vec_sel ((vui32_t)altb, (vui32_t)agtb, (vui32_t)signbool);
  result = vec_all_ne (tolt, zeros);
#endif
  return result;
}

/** \brief Vector Compare all Less Than (Zero-unordered) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return 1 (true),
 *  if vfa < vfb, otherwise 0 (false).
 *  Zeros of either sign are converted to +0.
 *  Infinities and NaNs are compared as signed values.
 *  Infinities and NaNs have the highest/lowest magnitudes.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision.
 *  Otherwise comparands are converted to unsigned integer magnitudes
 *  before using vector __int128 comparison to implement the equivalent
 *  Quad-precision floating-point operation. This leverages operations
 *  from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the IEEE standard
 *  relative to NaN comparison.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 28-37 | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return int boolean reflecting __binary128 less than.
 */
static inline int
vec_cmpqp_all_uzlt (__binary128 vfa, __binary128 vfb)
{
  int result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result = (vfa < vfb);
#else // defined( _ARCH_PWR8 )
  const vui32_t zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vui128_t vra, vrb;
  vb128_t age0, bge0;
  vui128_t vrap, vran;
  vui128_t vrbp, vrbn;

  vra = vec_xfer_bin128_2_vui128t (vfa);
  vrb = vec_xfer_bin128_2_vui128t (vfb);

  age0 = vec_setb_qp (vfa);
  vrap = (vui128_t) vec_xor ((vui32_t) vra, signmask);
  vran = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vra);
  vra  = (vui128_t) vec_sel ((vui32_t)vrap, (vui32_t)vran, (vui32_t)age0);

  bge0 = vec_setb_qp (vfb);
  vrbp = (vui128_t) vec_xor ((vui32_t) vrb, signmask);
  vrbn = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vrb);
  vrb  = (vui128_t) vec_sel ((vui32_t)vrbp, (vui32_t)vrbn, (vui32_t)bge0);

  result = vec_cmpuq_all_lt ((vui128_t) vra, (vui128_t) vrb);
#endif
  return result;
}

/** \brief Vector Compare all Less Than (Unordered) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return 1 (true),
 *  if vfa < vfb, otherwise 0 (false).
 *  Zeros of either sign are converted to +0.
 *  Infinities of different signs compare ordered.
 *  A NaN in either or both operands compare unordered.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision.
 *  Otherwise comparands are converted to unsigned integer magnitudes
 *  before using vector __int128 comparison to implement the equivalent
 *  Quad-precision floating-point operation. This leverages operations
 *  from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the PowerISA
 *  relative to NaN comparison and setting the FPSCR.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 28-37 | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return int boolean reflecting __binary128 less than.
 */
static inline int
vec_cmpqp_all_lt (__binary128 vfa, __binary128 vfb)
{
  int result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result = (vfa < vfb);
#else // defined( _ARCH_PWR8 )
  const vui32_t zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vui128_t vra, vrb;
  vb128_t age0, bge0;
  vui128_t vrap, vran;
  vui128_t vrbp, vrbn;

  vra = vec_xfer_bin128_2_vui128t (vfa);
  vrb = vec_xfer_bin128_2_vui128t (vfb);

  age0 = vec_setb_qp (vfa);
  vrap = (vui128_t) vec_xor ((vui32_t) vra, signmask);
  vran = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vra);
  vra  = (vui128_t) vec_sel ((vui32_t)vrap, (vui32_t)vran, (vui32_t)age0);

  bge0 = vec_setb_qp (vfb);
  vrbp = (vui128_t) vec_xor ((vui32_t) vrb, signmask);
  vrbn = (vui128_t) vec_subuqm ((vui128_t) zero, (vui128_t) vrb);
  vrb  = (vui128_t) vec_sel ((vui32_t)vrbp, (vui32_t)vrbn, (vui32_t)bge0);

  result = vec_cmpuq_all_lt ((vui128_t) vra, (vui128_t) vrb)
         && !vec_all_isunorderedf128 (vfa, vfb);
#endif
  return result;
}

/** \brief Vector Compare all Not-Equal (Total-order) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return 1,
 *  if vfa != vfb, otherwise 0.
 *  Zeros, Infinities and NaN of the same sign compare equal.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision.
 *  Otherwise use vector __int128 arithmetic and logical operations
 *  to implement the equivalent Quad-precision floating-point
 *  operation. This leverages operations from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the IEEE standard
 *  relative to signed zero, or NaN comparison.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 6     | 2/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return int boolean reflecting __binary128 compare not-equal.
 */
static inline int
vec_cmpqp_all_tone (__binary128 vfa, __binary128 vfb)
{
  int result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result= (vfa != vfb);
#else // defined( _ARCH_PWR8 )
  vui128_t vra, vrb;
  vra = vec_xfer_bin128_2_vui128t (vfa);
  vrb = vec_xfer_bin128_2_vui128t (vfb);
  result = vec_cmpuq_all_ne ( vra,  vrb );
#endif
  return result;
}

/** \brief Vector Compare all Not-Equal (Zero-unordered) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return 1,
 *  if vfa != vfb, otherwise 0.
 *  Zeros of either sign compare equal.
 *  Infinities and NaNs of the same sign compare equal.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision.
 *  Otherwise use vector __int128 arithmetic and logical operations
 *  to implement the equivalent Quad-precision floating-point
 *  operation. This leverages operations from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the IEEE standard
 *  relative NaN comparison.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10    | 1/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return int boolean reflecting __binary128 compare equal.
 */
static inline int
vec_cmpqp_all_uzne (__binary128 vfa, __binary128 vfb)
{
  int result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result = (vfa != vfb);
#else // defined( _ARCH_PWR8 )
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vb128_t or_ab;
  vui64_t vra, vrb;

  vra = vec_xfer_bin128_2_vui64t (vfa);
  vrb = vec_xfer_bin128_2_vui64t (vfb);

  or_ab = (vb128_t) vec_or ( vra, vrb );
  result = vec_cmpuq_all_ne ((vui128_t) or_ab, (vui128_t) signmask)
        && vec_cmpuq_all_ne ((vui128_t) vra, (vui128_t)vrb);
#endif
  return result;
}

/** \brief Vector Compare all Not-Equal (Unordered) Quad-Precision.
 *
 *  Compare Binary-float 128-bit values and return 1,
 *  if vfa == vfb, otherwise 0.
 *  Zeros of either sign compare equal.
 *  Infinities of the same sign compare equal.
 *  A NaN in either or both operands compare unequal.
 *
 *  For POWER9 (PowerISA 3.0B) or later, use a VSX Scalar Compare
 *  Unordered Quad-Precision.
 *  Otherwise use vector __int128 arithmetic and logical operations
 *  to implement the equivalent Quad-precision floating-point
 *  operation. This leverages operations from vec_int128_ppc.h.
 *
 *  \note This operation <I>may not</I> follow the PowerISA
 *  relative to NaN comparison and setting the FPSCR.
 *  However if the hardware target includes an instruction that does
 *  implement the IEEE standard, the implementation may use that.
 *  This relaxed implementation may be useful for implementations on
 *  POWER8 and earlier. Especially for soft-float implementations
 *  where it is known these special cases do not occur.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 18-30 | 1/cycle  |
 *  |power9   | 3     | 2/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return int boolean reflecting __binary128 compare not-equal.
 */
static inline int
vec_cmpqp_all_ne (__binary128 vfa, __binary128 vfb)
{
  int result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  result = (vfa != vfb);
#else // defined( _ARCH_PWR8 )
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  vb128_t or_ab;
  vui64_t vra, vrb;

  vra = vec_xfer_bin128_2_vui64t (vfa);
  vrb = vec_xfer_bin128_2_vui64t (vfb);

  or_ab = (vb128_t) vec_or ( vra, vrb );
  result = (vec_cmpuq_all_ne ((vui128_t) or_ab, (vui128_t) signmask)
            && vec_cmpuq_all_ne ((vui128_t) vra, (vui128_t)vrb))
         || vec_all_isunorderedf128 (vfa, vfb);
#endif
  return result;
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
  vui32_t tmp;
  vb128_t tmp2, tmp3;

  tmp = vec_and_bin128_2_vui32t (f128, expmask);
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
  tmp = vec_andc_bin128_2_vui32t (f128, signmask);

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
  vui32_t tmp;
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  const vui32_t expmask = CONST_VINT128_W(0x7fff0000, 0, 0, 0);

  tmp = vec_andc_bin128_2_vui32t (f128, signmask);
  return vec_cmpequq ((vui128_t)tmp , (vui128_t)expmask);
#endif
}

/** \brief Return 128-bit vector boolean true if the __float128 value
 *  is Not a Number (NaN).
 *
 *  A IEEE Binary128 NaN has a exponent of 0x7fff and nonzero
 *  significand. The sign bit is ignored.
 *  For POWER9 and later we use scalar_test_data_class().
 *  Otherwise mask off the sign bit and compare greater than unsigned
 *  quadword to the integer equivalent of Quad-Precision infinity.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 14-23 | 1/cycle  |
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
  vui32_t tmp;
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  const vui32_t expmask = CONST_VINT128_W(0x7fff0000, 0, 0, 0);

  tmp = vec_andc_bin128_2_vui32t (f128, signmask);
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
  vui32_t tmp;
  const vui32_t expmask = CONST_VINT128_W(0x7fff0000, 0, 0, 0);
  const vui32_t vec_zero = CONST_VINT128_W(0, 0, 0, 0);
  vb128_t result;

  tmp = vec_and_bin128_2_vui32t (f128, expmask);
  result = (vb128_t) vec_nor (vec_cmpeq (tmp, expmask),
			      vec_cmpeq (tmp, vec_zero));
  return (vb128_t) vec_splat ((vui32_t) result, VEC_W_H);
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
  vui32_t tmp, tmpz, tmp2;
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);
  const vui32_t vec_zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t minnorm = CONST_VINT128_W(0x00010000, 0, 0, 0);

  // Equivalent to vec_absf128 (f128)
  tmp = vec_andc_bin128_2_vui32t (f128, signmask);

  tmp2 = (vui32_t) vec_cmpltuq ((vui128_t)tmp, (vui128_t)minnorm);
  tmpz = (vui32_t) vec_cmpequq ((vui128_t)tmp, (vui128_t)vec_zero);
  return (vb128_t) vec_andc (tmp2, tmpz);
#endif
}

/** \brief Return 128-bit vector boolean true value,
 *  if either __float128 value (vra, vrb) is NaN.
 *
 *  A IEEE Binary128 NaN has a exponent of 0x7fff and nonzero
 *  significand. The sign bit is ignored.
 *  For POWER9 and later we use scalar_test_data_class().
 *  Otherwise mask off the sign bit and compare greater than unsigned
 *  quadword to the integer equivalent of Quad-Precision infinity.
 *
 *  \note This function will not raise VXSNAN or VXVC (FE_INVALID)
 *  exceptions. A normal __float128 compare can.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 16-25 | 1/cycle  |
 *  |power9   |   6   | 1/cycle  |
 *
 *  @param vfa 128-bit vector treated as a scalar __binary128.
 *  @param vfb 128-bit vector treated as a scalar __binary128.
 *  @return a vector boolean _int128.
 */
static inline vb128_t
vec_isunorderedf128 (__binary128 vfa, __binary128 vfb)
{
  return (vb128_t) vec_or ((vui32_t) vec_isnanf128 (vfa),
			   (vui32_t) vec_isnanf128 (vfb));
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
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);

  // Equivalent to vec_absf128 (f128)
  t128 = (vui128_t) vec_andc_bin128_2_vui32t (f128, signmask);
  return  (vb128_t)vec_cmpequq (t128, (vui128_t)vec_zero);
#endif
}
/** \brief Select and Transfer from one of two __binary128 scalars
 * under a 128-bit mask. The result is a __binary128 of the selected
 * value.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 2     | 2/cycle  |
 *  |power9   | 2     | 4/cycle  |
*
*  @param vfa a __binary128 floating point scalar value.
*  @param vfb a __binary128 floating point scalar value.
*  @param mask a vector bool __int128
*  @return The bit of vfa or vfb depending on the mask.
*/
static inline __binary128
vec_self128 (__binary128 vfa, __binary128 vfb, vb128_t mask)
{
  return vec_sel_bin128_2_bin128 (vfa, vfb, mask);
}

/*! \brief Vector Set Bool from Quadword Floating-point.
 *
 *  If the quadword's sign bit is '1' then return a vector bool
 *  __int128 that is all '1's. Otherwise return all '0's.
 *
 *  The resulting mask can be used in vector masking and select
 *  operations.
 *
 *  \note This operation will set the sign mask regardless of data
 *  class. For POWER9 the Scalar Test Data Class instructions copy the
 *  sign bit to CR bit 0 which distinguishes between +/- NaN.
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
  vb128_t result;
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  __asm__(
      "vexpandqm %0,%1"
      : "=v" (result)
      : "v" (f128)
      : );
#elif defined (_ARCH_PWR9) && defined (scalar_test_neg) && (__GNUC__ > 7)
  result = (vb128_t) {(__int128) 0};

  if (scalar_test_neg (f128))
    result = (vb128_t) {(__int128)-1};

  return (vb128_t)result;
#else
  const vui8_t shift = vec_splat_u8 (7);
  vui8_t t128 = vec_xfer_bin128_2_vui8t (f128);
  vui8_t splat = vec_splat (t128, VEC_BYTE_H);

  result = (vb128_t) vec_sra (splat, shift);
#endif
  return result;
}

/** \brief Return int boolean true if the __float128 value
 *  is negative (sign bit is '1').
 *
 *  For POWER9 use scalar_test_neg (a special case of
 *  scalar_test_data_class). For POWER8 and earlier, vec_and
 *  with a signmask and then vec_all_eq compare with that mask
 *  generates the boolean of the sign bit.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 4-10  | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param f128 a __float128 value in vector.
 *  @return a int boolean indicating the sign bit.
 */
static inline int
vec_signbitf128 (__binary128 f128)
{
#if defined (_ARCH_PWR9) && defined (scalar_test_neg) && (__GNUC__ > 7)
  return scalar_test_neg (f128);
#else
  vui32_t tmp;
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);

  tmp = vec_and_bin128_2_vui32t (f128, signmask);
  return vec_all_eq(tmp, signmask);
#endif
}

/** \brief VXS Scalar Convert Double-Precision to Quad-Precision format.
 *
 *  The left most double-precision element of vector f64 is converted
 *  to quad-precision format.
 *
 *  For POWER9 use the xscvdpqp instruction.
 *  For POWER8 and earlier use vector instruction generated by PVECLIB
 *  operations.
 *
 *  \note This operation <I>may not</I> follow the PowerISA
 *  relative to Signaling NaN and setting the FPSCR.
 *  However if the hardware target includes the xscvdpqp instruction,
 *  the implementation may use that.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   ?   | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param f64 a vector double. The left most element is converted.
 *  @return a __binary128 value.
 */
__binary128
static inline vec_xscvdpqp (vf64_t f64)
{
  __binary128 result;
#if defined (_ARCH_PWR9) && (__GNUC__ > 7)
#if defined (__FLOAT128__) && (__GNUC__ > 9)
  // earlier GCC versions generate extra data moves for this.
  result = f64[VEC_DW_H];
#else
  // No extra data moves here.
  __asm__(
      "xscvdpqp %0,%1"
      : "=v" (result)
      : "v" (f64)
      : );
#endif
#elif  defined (_ARCH_PWR8)
  vui64_t d_exp, d_sig, q_exp;
  vui128_t q_sig;
  vui32_t q_sign;
  const vui64_t exp_delta = (vui64_t) CONST_VINT64_DW( (0x3fff - 0x3ff), 0 );
  const vui64_t d_naninf = (vui64_t) CONST_VINT64_DW( 0x7ff, 0 );
  const vui64_t d_denorm = (vui64_t) CONST_VINT64_DW( 0, 0 );
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);


  f64[VEC_DW_L] = 0.0; // clear the right most element to zero.
  // Extract the exponent, significand, and sign bit.
  d_exp = vec_xvxexpdp (f64);
  d_sig = vec_xvxsigdp (f64);
  q_sign = vec_and ((vui32_t) f64, signmask);
  // The extract sig operation has already tested for finite/subnormal.
  // So avoid testing isfinite/issubnormal again by simply testing
  // the extracted exponent.
  if (__builtin_expect (!vec_cmpud_all_eq (d_exp, d_naninf), 1))
    {
      if (__builtin_expect (!vec_cmpud_all_eq (d_exp, d_denorm), 1))
	{
	  q_sig = vec_srqi ((vui128_t) d_sig, 4);
	  q_exp = vec_addudm (d_exp, exp_delta);
	}
      else
	{
	  if (vec_cmpud_all_eq (d_sig, d_denorm))
	    {
	      q_sig = (vui128_t) d_sig;
	      q_exp = (vui64_t) d_exp;
	    }
	  else
	    { // Must be subnormal but we need to produce a normal QP.
	      // So need to adjust the quad exponent by the f64 denormal
	      // exponent (-1023) and any leading '0's in the f64 sig.
	      // There will be at least 12.
	      vui64_t q_denorm = (vui64_t) CONST_VINT64_DW( (0x3fff - (1023 - 12)), 0 );
	      vui64_t f64_clz;
	      f64_clz = vec_clzd (d_sig);
	      d_sig = vec_vsld (d_sig, f64_clz);
	      q_exp = vec_subudm (q_denorm, f64_clz);
	      q_sig = vec_srqi ((vui128_t) d_sig, 15);
	    }
	}
    }
  else
    { // isinf or isnan.
      q_sig = vec_srqi ((vui128_t) d_sig, 4);
      q_exp = (vui64_t) CONST_VINT64_DW(0x7fff, 0);
    }
  // Copy Sign-bit to QP significand before insert.
  q_sig = (vui128_t) vec_or ((vui32_t) q_sig, q_sign);
  // Insert exponent into significand to complete conversion to QP
  result = vec_xsiexpqp (q_sig, q_exp);
#else
  result = f64[VEC_DW_H];
#endif
  return result;
}

/** \brief VXS Scalar Convert Signed-Doubleword to Quad-Precision format.
 *
 *  The left most signed doubleword element of vector int64 is converted
 *  to quad-precision format.
 *
 *  For POWER9 use the xscvsdqp instruction.
 *  For POWER8 and earlier use vector instruction generated by PVECLIB
 *  operations.
 *
 *
 *  \note At this point we are not trying to comply with PowerISA by
 *  setting any FPSCR bits associated with Quad-Precision convert.
 *  If such is required, FR and/or FI can be set
 *  using the Move To FPSCR Bit 0 (mtfsb0) instruction.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   ?   | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param int64 a vector signed long long. The left most element is converted.
 *  @return a __binary128 value.
 */
__binary128
static inline vec_xscvsdqp (vi64_t int64)
{
  __binary128 result;
#if defined (_ARCH_PWR9) && (__GNUC__ > 7)
#if defined (__FLOAT128__) && (__GNUC__ > 9)
  // earlier GCC versions generate extra data moves for this.
  result = int64[VEC_DW_H];
#else
  // No extra data moves here.
  __asm__(
      "xscvsdqp %0,%1"
      : "=v" (result)
      : "v" (int64)
      : );
#endif
#elif  defined (_ARCH_PWR8)
  vui64_t d_sig, q_exp, d_sign, d_neg;
  vui128_t q_sig;
  vui32_t q_sign;
  const vui64_t d_zero = (vui64_t) CONST_VINT64_DW( 0, 0 );
  const vui32_t signmask = CONST_VINT128_W(0x80000000, 0, 0, 0);

  int64[VEC_DW_L] = 0UL; // clear the right most element to zero.

  if (vec_cmpud_all_eq ((vui64_t) int64, d_zero))
    {
      result = vec_xfer_vui64t_2_bin128 (d_zero);
    }
  else
    {
      // We need to produce a normal QP, so we treat the integer like a
      // denormal, then normalize it.
      // Start with the quad exponent bias + 63 then subtract the count
      // leading '0's. The 64-bit magnitude has 1-63 leading '0's
      vui64_t q_expm = (vui64_t) CONST_VINT64_DW((0x3fff + 63), 0 );
      vui64_t i64_clz;
      // Convert 2s complement to signed magnitude form.
      q_sign = vec_and ((vui32_t) int64, signmask);
      d_neg  = vec_subudm (d_zero, (vui64_t)int64);
      d_sign = (vui64_t) vec_cmpequd ((vui64_t) q_sign, (vui64_t) signmask);
      d_sig = (vui64_t) vec_sel ((vui32_t) int64, (vui32_t) d_neg, (vui32_t) d_sign);
      // Count leading zeros and normalize.
      i64_clz = vec_clzd (d_sig);
      d_sig = vec_vsld (d_sig, i64_clz);
      q_exp = vec_subudm (q_expm, i64_clz);
      q_sig = vec_srqi ((vui128_t) d_sig, 15);
      // Copy Sign-bit to QP significand before insert.
      q_sig = (vui128_t) vec_or ((vui32_t) q_sig, q_sign);
      // Insert exponent into significand to complete conversion to QP
      result = vec_xsiexpqp (q_sig, q_exp);
    }
#else
  result = int64[VEC_DW_H];
#endif
  return result;
}

/** \brief VXS Scalar Convert Unsigned-Doubleword to Quad-Precision format.
 *
 *  The left most unsigned doubleword element of vector int64 is converted
 *  to quad-precision format.
 *
 *  For POWER9 use the xscvudqp instruction.
 *  For POWER8 and earlier use vector instruction generated by PVECLIB
 *  operations.
 *
 *  \note At this point we are not trying to comply with PowerISA by
 *  setting any FPSCR bits associated with Quad-Precision convert.
 *  If such is required, FR and/or FI can be set
 *  using the Move To FPSCR Bit 0 (mtfsb0) instruction.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |   ?   | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param int64 a vector unsigned long long. The left most element is converted.
 *  @return a __binary128 value.
 */
__binary128
static inline vec_xscvudqp (vui64_t int64)
{
  __binary128 result;
#if defined (_ARCH_PWR9) && (__GNUC__ > 7)
#if defined (__FLOAT128__) && (__GNUC__ > 9)
  // earlier GCC versions generate extra data moves for this.
  result = int64[VEC_DW_H];
#else
  // No extra data moves here.
  __asm__(
      "xscvudqp %0,%1"
      : "=v" (result)
      : "v" (int64)
      : );
#endif
#elif  defined (_ARCH_PWR8)
  vui64_t d_sig, q_exp;
  vui128_t q_sig;
  const vui64_t d_zero = (vui64_t) CONST_VINT64_DW( 0, 0 );

  int64[VEC_DW_L] = 0UL; // clear the right most element to zero.
  d_sig = int64;
  // Quick test for 0UL as this case requires a special exponent.
  if (vec_cmpud_all_eq (int64, d_zero))
    {
      result = vec_xfer_vui64t_2_bin128 (d_zero);
    }
  else
    { // We need to produce a normal QP, so we treat the integer like a
      // denormal, then normalize it.
      // Start with the quad exponent bias + 63 then subtract the count of
      // leading '0's. The 64-bit sig can have 0-63 leading '0's.
      const vui64_t q_expm = (vui64_t) CONST_VINT64_DW((0x3fff + 63), 0 );
      vui64_t i64_clz = vec_clzd (int64);
      d_sig = vec_vsld (int64, i64_clz);
      q_exp = vec_subudm (q_expm, i64_clz);
      q_sig = vec_srqi ((vui128_t) d_sig, 15);
      // Insert exponent into significand to complete conversion to QP
      result = vec_xsiexpqp (q_sig, q_exp);
    }
#else
  result = f64[VEC_DW_H];
#endif
  return result;
}

/** \brief Scalar Insert Exponent Quad-Precision
 *
 *  Merge the sign (bit 0) and significand (bits 16:127) from sig
 *  with the 15-bit exponent from exp (bits 49:63). The exponent is
 *  moved to bits 1:15 of the final result.
 *  The result is returned as a Quad_precision floating point value.
 *
 *  \note This operation is equivalent to the POWER9 xsiexpqp
 *  instruction, This instruction requires a
 *  POWER9-enabled compiler targeting -mcpu=power9 and is not
 *  available for older compilers nor POWER8 and earlier.
 *  We can't use the built-in scalar_insert_exp because it requires
 *  scalar (GPR) inputs and vec_insert_exp is not defined for
 *  Quad-Precision.
 *  We expect (in context) inputs will be in VRs.
 *  This operation provides implementations for all VSX-enabled
 *  platforms.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  2-11 | 2/cycle  |
 *  |power9   |   2   | 4/cycle  |
 *
 *  @param sig vector __int128 containing the Sign Bit and 112-bit significand.
 *  @param exp vector unsigned long long element 0 containing the 15-bit exponent.
 *  @return a __binary128 value where the exponent bits (1:15) of sig
 *  are replaced from bits 49:63 of exp.
 *
 */
static inline __binary128
vec_xsiexpqp (vui128_t sig, vui64_t exp)
{
  __binary128 result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  __asm__(
      "xsiexpqp %0,%1,%2"
      : "=v" (result)
      : "v" (sig), "v" (exp)
      : );

#else
  vui32_t tmp, t128;
  const vui32_t expmask = CONST_VINT128_W(0x7fff0000, 0, 0, 0);

  tmp = vec_sld ((vui32_t) exp, (vui32_t) exp, 6);
  t128 =  vec_sel ((vui32_t) sig, tmp, expmask);
  result = vec_xfer_vui32t_2_bin128 (t128);
#endif
  return result;
}

/** \brief Scalar Extract Exponent Quad-Precision
 *
 *  Extract the quad-precision exponent (bits 1:15) and right justify
 *  it to (bits 49:63 of) doubleword 0 of the result vector.
 *  The result is returned as vector long long integer value.
 *
 *  \note This operation is equivalent to the POWER9 xsxexpqp
 *  instruction. This instruction requires a
 *  POWER9-enabled compiler targeting -mcpu=power9 and is not
 *  available for older compilers nor POWER8 and earlier.
 *  We can't use the built-in scalar_extract_exp because it returns
 *  scalar (GPR) results and vec_extract_exp is not defined for
 *  Quad-Precision.
 *  We expect (in context) results are needed in VRs.
 *  This operation provides implementations for all VSX-enabled
 *  platforms.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  4-13 | 2/cycle  |
 *  |power9   |   2   | 4/cycle  |
 *
 *  @param f128 __binary128 scalar value in a vector register.
 *  @return vector unsigned long long element 0 containing the 15-bit
 *  exponent
 *
 */
static inline vui64_t
vec_xsxexpqp (__binary128 f128)
{
  vui64_t result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  __asm__(
      "xsxexpqp %0,%1"
      : "=v" (result)
      : "v" (f128)
      : );

#else
  vui32_t tmp;
  const vui32_t expmask = CONST_VINT128_W(0x7fff0000, 0, 0, 0);

  tmp = vec_and_bin128_2_vui32t (f128, expmask);
  result = (vui64_t) vec_sld (tmp, tmp, 10);
#endif
  return result;
}

/** \brief Scalar Extract Significand Quad-Precision
 *
 *  Extract the quad-precision significand (bits 16:127) and
 *  restore the implied (hidden) bit (bit 15) if the quad-precition
 *  value is normal (not zero, subnormal, Infinity or NaN).
 *  The result is returned as vector __int128 integer value with
 *  up to 113 bits of significance.
 *
 *  \note This operation is equivalent to the POWER9 xsxsigqp
 *  instruction. This instruction requires a
 *  POWER9-enabled compiler targeting -mcpu=power9 and is not
 *  available for older compilers nor POWER8 and earlier.
 *  We can't use the built-in scalar_extract_sig because it returns
 *  scalar (GPR) results and vec_extract_sig is not defined for
 *  Quad-Precision.
 *  We expect (in context) results are needed in VRs.
 *  This operation provides implementations for all VSX-enabled
 *  platforms.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 10-19 | 2/cycle  |
 *  |power9   |   3   | 2/cycle  |
 *
 *  @param f128 __binary128 scalar value in a vector register.
 *  @return vector __int128 containing the significand.
 *
 */
static inline vui128_t
vec_xsxsigqp (__binary128 f128)
{
  vui128_t result;
#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  __asm__(
      "xsxsigqp %0,%1"
      : "=v" (result)
      : "v" (f128)
      : );
#else
  vui32_t t128, tmp, normal;
  const vui32_t zero = CONST_VINT128_W(0, 0, 0, 0);
  const vui32_t sigmask = CONST_VINT128_W(0x0000ffff, -1, -1, -1);
  const vui32_t expmask = CONST_VINT128_W(0x7fff0000, 0, 0, 0);
  const vui32_t hidden = CONST_VINT128_W(0x00010000, 0, 0, 0);

  // Check if f128 is normal. Normal values need the hidden bit
  // restored to the significand. We use a simpler sequence here as
  // vec_isnormalf128 does more then we need.
  tmp = vec_and_bin128_2_vui32t (f128, expmask);
  normal = (vui32_t) vec_nor (vec_cmpeq (tmp, expmask),
		              vec_cmpeq (tmp, zero));
  t128 = vec_and_bin128_2_vui32t (f128, sigmask);
  result = (vui128_t) vec_sel (t128, normal, hidden);
#endif
  return result;
}

#endif /* VEC_F128_PPC_H_ */
