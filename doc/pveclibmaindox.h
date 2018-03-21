/*
 Copyright (c) 2017 IBM Corporation.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 Contributors:
      IBM Corporation, Steven Munroe
 */

#ifndef __PVECLIB_MAIN_DOX_H
#define __PVECLIB_MAIN_DOX_H

/** \mainpage POWER Vector Library (pveclib)
* \brief library of useful vector functions for POWER. This library
* fills in the gap between the instructions defined in the POWER
* Instruction Set Architecture (<B>PowerISA</B>) and major application
* libraries like ESSL and MASSV.
* The C/C++ language compilers (that support PowerISA) may implement
* vector intrinsic functions (compiler built-ins as embodied by
* altivec.h). These vector intrinsics offer an alternative to
* assembler programming, but do not offer higher function then that
* already provided by the PowerISA.
*
*  \authors Steven Munroe
*
*  \section Rationale
*
*  Higher level vector intrinsic operations are needed. One key reason
*  is to smooth over the high complexity of the evolving PowerISA and
*  compiler support.
*
*  For example: the PowerISA 2.07 (Power8) provides population count
*  and count leading zero operations on vectors of byte, halfword,
*  word, and doubleword elements but not on the whole vector as a
*  __int128 value. Before PowerISA 2.07, neither operation was
*  supported, for any element size.
*
*  Another example: The original <B>Altivec</B>
*  (AKA Vector Multimedia Extension (<B>VMX</B>))
*  provided Vector Multiply Odd / Even operations for signed / unsigned
*  byte and halfword elements.  The PowerISA 2.07 added Vector Multiply
*  Even/Odd operations for signed / unsigned word elements.  This
*  release also added a Vector Multiply Unsigned Word Modulo operation.
*  This was important to allow auto vectorization of C loops using
*  32-bit (int) multiply.
*
*  But PowerISA 2.07 did not add support for doubleword or quadword
*  (__int128) multiply directly.  Nor did it fill in the missing
*  multiply modulo operations for byte and halfword.  However it
*  did add support for doubleword and quadword add / subtract modulo,
*  This can be helpful, if you are willing to apply grade school
*  arithmetic (add, carry the 1) to vector elements.
*
*  PowerISA 3.0 (Power9) did add a Vector Multiply-Sum Unsigned
*  Doubleword Modulo operation.  With this instruction (and a generated
*  vector of zeros as input) you can effectively implement the simple
*  doubleword integer multiply modulo operation in a few instructions.
*  Similarly for Vector Multiply-Sum Unsigned Halfword Modulo.
*  But this may not be obvious.
*
*  This history embodies a set of trade-offs negotiated between the
*  Software and Processor design architects at specific points in time.
*  But most programmers would prefer to use a set of operators applied
*  across the supported element types and sizes.
*
*  \subsection mainpage_sub0 POWER Vector Library Goals
*
*  Obviously many useful operations can be constructed from existing
*  PowerISA operations and GCC <altivec.h> built-ins but the
*  implementation may not be obvious. The optimum sequence will
*  vary across the PowerISA levels as new instructions are added.
*  And finally the compiler's built-in support for new PowerISA
*  instructions evolves with the compiler's release cycle.
*
*  So the goal of this project is to provide well crafted
*  implementations of useful vector and large number operations.
*
*  - Provide equivalent functions across versions of the PowerISA.
*  This includes some of the most useful vector instructions added to
*  Power9 (PowerISA 3.0B).
*  Many of these operations can be implemented as in-line function in
*  a few vector instructions on earlier PowerISA versions.
*  - Provide equivalent functions across versions of the compiler.
*  For example built-ins provided in later versions of the compiler
*  can be implemented as in-line functions with in-line asm in earlier
*  compiler versions.
*  - Provide complete arithmetic operations across supported C types.
*  For example multiply modulo and even/odd for int, long, and __int128.
*  - Provide complete extended arithmetic (carry / extend /
*  multiple high) operations across supported C types.
*  For example add / subtract with carry and extent for int, long,
*  and __int128.
*  - Provide higher order functions not provided directly by the PowerISA.
*  For example vector SIMD implementation for ASCII __isalpha, etc.
*  Another example full __int128 implementations of Count Leading Zeros,
*  Population Count, Shift left/right immediate, and integer divide.
*  - Such implementations should be small enough to in-line and allow
*  the compiler opportunity to apply common optimization techniques.
*
*  \subsubsection mainpage_sub0_1 POWER Vector Library Intrinsic headers
*
*  The POWER Vector Library will be primarily delivered as C language
*  in-line functions in headers files.
*  - vec_common_ppc.h Typedefs and helper macros
*  - vec_int128_ppc.h Operations on vector __int128 values
*  - vec_int64_ppc.h Operations on vector long int (64-bit) values
*  - vec_int32_ppc.h Operations on vector int (32-bit) values
*  - vec_char_ppc.h Operations on vector char (values) values
*  - vec_bcd_ppc.h Operations on vectors of Binary Code Decimal
*  and Zoned Decimal values
*  - vec_f128_ppc.h Operations on vector _Float128 values
*  - vec_f64_ppc.h Operations on vector double values
*  - vec_f32_ppc.h Operations on vector float values
*
*  \note The list above more of an aspiration at this time.
*  You will not find all of these headers or complete operation set
*  and platform coverage in the current public github.
*  But many of these headers do exist in private trees as we work
*  on completing function and testing across compilers and PowerISA
*  versions.
*
*  The goal is to provide high quality implementations that adapt to
*  the specifics of the compile target (-mcpu=) and compiler
*  (<altive.h>) version you are using. Initially pveclib will focus on
*  the GCC compiler and -mcpu=[power7|power8|power9] for Linux.
*  Testing will focus on Little Endian (<B>powerpc64le</B> for power8
*  and power9 targets.  Any testing for Big Endian (<B>powerpc64</B>
*  will be initially restricted to power7 and power8 targets.
*
*  Expanding pveclib support beyond this list to include:
*  - additional compilers (ie Clang)
*  - additional PPC platforms (970, power6, ...)
*  - Larger functions that just happen to use vector registers
*  (Checksum, Crypto, compress/decompress, lower precision neural networks, ...)
*  .
*  will largely depend on additional skilled practitioners joining this
*  project and contributing (code and platform testing)
*  on a sustained basis.
*
*  \subsection mainpage_sub1 How pveclib is different from compiler vector built-ins
*
*  The PowerPC vector build-ins evolved from the original
*  <a href="https://www.nxp.com/docs/en/reference-manual/ALTIVECPIM.pdf">
*  AltiVec (TM) Technology Programming Interface Manual</a> (PIM).
*  The PIM defined the minimal extensions to the application binary
*  interface (ABI) required to support the Vector Facility.
*  This included new keywords (vector, pixel, bool) for defining
*  new vector types, and new operators (build-in functions) required
*  for any supporting and compliant C language compiler.
*
*  The vector build-in function support included:
*  - generic AltiVec operations, like vec_add()
*  - specific AltiVec operations (instructions, like vec_vaddubm())
*  - predicates computed from a AltiVec operations, like vec_all_eq()
*  which are also generic
*
*  See \ref mainpage_sub_1_3 for more details.
*
*  There are clear advantages with the compiler implementing the
*  vector operations as built-ins:
*  - The compiler can access the C language type information and
*  vector extensions to implement the function overloading
*  required to process generic operations.
*  - Built-ins can be generated in-line, which eliminates function
*  call overhead and allows more compact code generation.
*  - The compiler can then apply higher order optimization across
*  built-ins including:
*  Local and global register allocation.
*  Global common subexpression elimination.
*  Loop-invariant code motion.
*  - The compiler can automatically select the best instructions
*  for the <I>target</I> processor ISA level
*  (from the -mcpu compiler option).
*
*  While this is an improvement over writing assembler code,  it does
*  not provide much function beyond the specific operations specified in
*  the PowerISA.
*
*  Another issue is that generic operations where not
*  uniformly applicable across vector types.
*  For example:
*  - vec_add / vec_sub applied to float, int, short and char.
*  - Later compilers added support for double
*  (with POWER7 and the Vector Scalar Extensions (VSX) facility)
*  - Integer long (64-bit) and __int128 support for POWER8
*  (PowerISA 2.07B).
*
*  But vec_mul vec_div did not:
*  - vec_mul applied to float (and later double, with POWER7 VSX).
*  - vec_mule / vec_mulo (Multiply even / odd elements)
*  applied to [signed | unsigned] integer short and char.
*  Later compilers added support for vector int after
*  Power8 added vector multiply word instructions.
*  - vec_div was not included in the original PIM as
*  Altivec (VMX) only included vector reciprocal estimate for float
*  and no vector integer divide for any size.
*  Later compilers added support for vec_div float / double after
*  POWER7 (VSX) added vector divide single/double-precision instructions.
*
*  \note While the processor you (plan to) use,  may support the
*  specific instructions you want to exploit,  the compiler you are
*  using may not support,  the generic or specific vector operations,
*  for the element size/types, you want to use.
*  This is common for GCC versions installed by "Enterprise Linux"
*  distributions. They tend to freeze the GCC version early and
*  maintain that GCC version for long term stability.
*  One solution is to use the
*  <a href="https://developer.ibm.com/linuxonpower/advance-toolchain/">
*  IBM Advance toolchain for Linux on Power</a> (AT).
*  AT is free for download and new AT versions are released yearly
*  (usually in August) with the latest stable GCC from that spring.
*
*  This all can be very frustrating, at minimum,  or even a show
*  stopper, if you are on a tight schedule for you project.
*  Especially if you are not familiar with the evolving history of the
*  PowerISA and supporting compilers.
*
*  \subsubsection mainpage_sub_1_1 What can we do about this?
*
*  First the Binutils assembler is usually updated within weeks of the
*  public release of the PowerISA document. So while your compiler
*  may not support the latest vector operations as built-in operations,
*  an older compiler with an updated assembler,
*  may support the instructions as in-line assembler.
*
*  Sequences of in-line assembler instructions can be wrapped within
*  C language static inline functions and placed in a header files
*  for shared use. If you are careful with the input / output register
*  <I>constraints</I> the GCC compiler can provide local register
*  allocation and minimize parameter marshaling overhead. This is very
*  close (in function) to a specific Altivec (built-in) operation.
*
*  \note Using GCC's in-line assembler can be challenging even for the
*  experienced programmer. The register constraints have grown in
*  complexity as new facilities and categories where added.
*  The fact that some (VMX) instructions are restricted to the original
*  32 Vector Registers (<B>VRs</B>) (the high half of the Vector-Scalar
*  Registers <B>VSRs</B>), while others (Binary and Decimal
*  Floating-Point) are restricted to the original 32 Floating-Point
*  Registers (<B>FPRs</B> (overlapping the low half of the VSRs), and
*  the new VSX instructions can access all 64 VSRs, is just one source
*  of complexity.
*  So it is very important to get your input/output constraints correct
*  if you want in-line assembler code to work correctly.
*
*  In-line assembler should be
*  reserved for the first implementation using the latest PowerISA.
*  Where possible you should use existing vector built-ins to implement
*  specific operations for wider element types, support older hardware,
*  or higher order operations.
*  Again wrapping these implementations in static inline functions for
*  collection in header files for reuse and distribution is recommended.
*
*  The PowerISA vector facility has all the instructions you need to
*  implement extended precision operations for add, subtract,
*  and multiply. Add / subtract with carry-out and permute or
*  double vector shift and grade-school arithmetic is all you need.
*
*  For example the Vector Add Unsigned Quadword Modulo introduced in
*  POWER8 (PowerISA 2.07B) can be implemented for POWER7 and earlier
*  machines in 10-11 instructions. This uses a combination of
*  Vector Add Unsigned Word Modulo (vadduwm), Vector Add and Write
*  Carry-Out Unsigned Word (vaddcuw), and Vector Shift Left Double by
*  Octet Immediate (sldoi), to propagate the word carries through the
*  quadword.
*
*  For Power8 and later, C vector integer (modulo) multiply can be
*  implemented in a single Vector Unsigned Word Modulo (<B>vmuluwm</B>)
*  instruction. This was added explicitly to address vectorizing loops
*  using int multiply in C language code.  And some newer compilers do
*  support generic vec_mul() for vector int. But this is not
*  documented. Similarly for char (byte) and short (halfword) elements.
*
*  Power8 also introduced Vector Multiply Even Signed Word
*  (<B>vmulesw</B>) and Vector Multiply Odd Signed Word
*  (<B>vmulosw</B>) instructions.  So you would expect the generic
*  vec_mule and vec_mulo operations to be extended to support
*  <I>vector int</I>,  as these operations have long been supported for
*  char and short.  Sadly this is not supported as of GCC 7.3.
*  We hope to see this implemented for GCC 8.
*
*  So what will the compiler do for vector multiply int (modulo, even,
*  or odd) for targeting power7?  Older compilers will reject this as a
*  <I>invalid parameter combination ...</I>.  A newer compiler may
*  implement the equivalent function in a short sequence of VMX
*  instructions from PowerISA 2.06 or earlier.
*  And GCC 7.3 does support vec_mul for element types char,
*  short, and int. These sequences are in the 2-7 instruction range
*  depending on the operation and element type. This includes some
*  constant loads and permute control vectors that can be factored
*  and reused across operations.
*
*  Once the pattern is understood it is not hard to write equivalent
*  sequences using operations from the original <altivec.h>.  With a
*  little care these sequences will be compatible with older compilers
*  and older PowerISA versions.
*  These concepts can be extended to operations that PowerISA and the
*  compiler does not support yet.  For example; a processor that may
*  not have multiply even/odd/modulo of the required width (word,
*  doubleword, or quadword). This it might take 10-12 instructions to
*  implement the next element size bigger then the current processor.
*  A full 128-bit by 128-bit multiply with 256-bit result only
*  requires 32 instructions on a Power8 (using multiple word even/odd).
*
*  Also many of the operations missing from the vector facility,
*  exist in the Fixed-point, Floating-point,
*  or Decimal Floating-point scalar facilities.
*  There will some loss of efficiency in the data transfer but
*  compared to a complex operation like divide or decimal conversions,
*  this can be a workable solution.
*  On older POWER processors (before power7/8) transfers between
*  register banks (GPR, FPR, VR) had to go through memory.
*  But with the VSX facility (Power7) FPRs and VRs overlap with the
*  lower and upper halves of the 64 VSR registers.
*  So FPR <-> VSR transfer are 0-2 cycles latency.
*  And with power8 we GPR <-> FPR | VR | VSR direct transfer
*  instructions in the 4-5 cycle latency range.
*
*  For example Power8 added Binary Coded Decimal (<B>BCD</B>)
*  add/subtract for signed 31 digit vector values. The vector unit
*  does not support BCD multiply / divide.
*  But the Decimal Floating-Point (<B>DFP</B>) facility (introduced
*  with PowerISA 2.05 and Power6) supports up to 34-digit
*  (__Decimal128) precision and all the expected
*  (add/subtract/multiply/divide/...) arithmetic operations. DFP also
*  supports conversion to/from 31-digit BCD and __Decimal128 precision.
*  This is all supported with a hardware Decimal Floating-Point Unit
*  (<B>DFU</B>).
*
*  So bcd_add / bcd_sub can be generated as a single instruction on
*  Power8 and later, and 10-11 instructions for Power6/7.
*  This count include the VSR <-> FPRp transfers,
*  BCD <-> DFP conversions, and DFP add/sub.
*  Similarly bcd_mul / bcd_div are implemented in 11 instructions using
*  register transfer and the DFU operations for Power6/7/8.
*
*  \note So why does anybody care about BCD and DFP? Sometimes you get
*  large numbers in decimal that you need converted to binary for
*  extended computation. Sometimes you need to display the results of
*  your extended binary computation in decimal. The multiply by 10 and
*  BCD vector operations help simplify and speed-up these conversions.
*
*  And finally: Henry S. Warren's wonderful book Hacker's Delight
*  provides inspiration for SIMD versions of; count leading zeros,
*  population count, parity, etc.
*
*  \subsubsection  mainpage_sub_1_2 So what can the Power Vector Library project do?
*
*  Clearly the PowerISA provides multiple, extensive, and powerful
*  computational facilities that continue to evolve and grow.
*  But the best instruction sequence for a specific computation depends
*  on which POWER processor(s) you have or plan to support.
*  It can also depend on the specific compiler version you use, unless
*  you are willing to write some of your application code in assembler.
*  Even then you need to be aware of The PowerISA versions and when
*  specific instructions where introduced.  This can be frustrating if
*  you just want to port you application to POWER for a quick
*  evaluation.
*
*  So you would like to start evaluating how to leverage this power
*  for key algorithms at the heart of your application.
*  - But you are working with older POWER processor
*  (until the latest POWER box is delivered).
*  - Or the latest POWER machine just arrived at your site (or cloud)
*  but you are stuck using an older/stable Linux distro version
*  (with an older distro compiler).
*  - Or you need extended precision multiply for your crypto code
*  but you are not really an assembler level programmer
*  (or don't want to be).
*  - Or you would like to program with higher level operations to
*  improve your own productivity.
*
*  Someone with the right background (knowledge of the; PowerISA,
*  assembler level programming, compilers and the vector built-ins,
*  ...) can solve any of the issues described above. But you don't
*  have time for this.
*
*  There should be an easier way to exploit the POWER vector hardware
*  without getting lost in the details. And this extends beyond
*  classical vector (Single Instruction Multiple Data (SIMD))
*  programming to exploiting; larger data width (128-bit and beyond),
*  and larger register space (64 x 128 Vector Scalar Registers)
*
*  Here is an example of what can be done: \code
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
*  \endcode
*
*  The <B>_ARCH_PWR8</B> macro is defined by the compiler when it targets
*  POWER8 (PowerISA 2.07) or later. This is the first processor and
*  PowerISA level to support vector quadword add/subtract. Otherwise we
*  need to use the vector word add modulo and vector word add and
*  write carry-out word, to add 32-bit chunks and propagate the
*  carries through the quadword.
*
*  One little detail remains. Support for vec_vadduqm was added to GCC
*  in March of 2014, after GCC 4.8 was released and GCC 4.9's feature
*  freeze.  So the only guarantee is that this feature is in GCC
*  5.0 and later.  At some point this change was backported to GCC 4.8
*  and 4.9 as it is included in the current GCC 4.8/4.9 documentation.
*  When or if these backports where propagated to a specific Linux
*  Distro version or update is difficult to determine.
*  So support for this vector built-in dependes on the specific
*  version of the GCC compiler, or if specific Distro update includes
*  these specific backports for the GCC 4.8/4.9 compiler they support.
*  The: \code
*  #ifndef vec_vadduqm
*  \endcode
*  C preprocessor conditional checks if the <B>vec_vadduqm</B>
*  is defined in <altivec.h>. If defined we can assume that the
*  compiler implements <B>__builtin_vec_vadduqm</B> and that
*  <altivec.h> includes the macro definition: \code
#define vec_vadduqm __builtin_vec_vadduqm
*  \endcode
*  For <B>_ARCH_PWR7</B> and earlier we need a little grade school
*  arithmetic using Vector Add Unsigned Word Modulo (<B>vadduwm</B>)
*  and Vector Add and Write Carry-Out Unsigned Word (<B>vaddcuw</B>).
*  This treats the vector __int128 as 4 32-bit binary digits.
*  The first instruction sums each (32-bit digit) column and the second
*  records the carry out of the high order bit.  This leaves the carry
*  bit in the original (word) column, so use a shift left to line up
*  the carries with the next higher word.
*
*  To propagate any carries across all 4 (word) digits,  repeat this
*  (add / carry / shift) sequence three times.
*  Then a final add modulo word to complete the 128-bit add.
*  This sequence requires 10-11 instructions. The 11th instruction is
*  a vector splat word 0 immediate, which in needed in the shift left
*  (vsldoi) instructions. This common in vector codes and the compile
*  can usually reuse this register across several blocks of code and
*  in-line functions.
*
*  For Power7/8 these instructions are all 2 cycle latency and 2 per
*  cycle throughput.  The vadduwm / vaddcuw instruction pairs should
*  issue in the same cycle and execute in parallel.
*  So the expected a latency for this sequence is 14 cycles.
*  For Power8 the vadduqm instruction has a 4 cycle latency.
*
*  Similarly for the carry / extend forms which can be combined to
*  support wider (256, 512, 1024, ...) extended arithmetic.
*  \sa vec_addcuq, vec_addeuqm, and vec_addecuq
*
*  Another example <B>Vector Multiply-by-10 Unsigned Quadword</B>:
*  \code
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
  t_even = vec_sld (t_even, z, 2);
#ifdef _ARCH_PWR8
  t = (vui32_t) vec_vadduqm ((vui128_t) t_even, (vui128_t) t_odd);
#else
  t = (vui32_t) vec_adduqm ((vui128_t) t_even, (vui128_t) t_odd);
#endif
#endif
  return ((vui128_t) t);
}
*  \endcode
*
*  PowerISA 3.0 added this instruction and it's extend / carry forms
*  to speed up decimal to binary conversion for large numbers.
*
*  Notice that under the <B>_ARCH_PWR9</B> conditional, there is no
*  check for the specific <B>vec_vmul10uq</B> built-in.  As of this
*  writing <B>vec_vmul10uq</B> is not included in the
*  <I>OpenPOWER ELF2 ABI</I> documentation nor in the latest GCC trunk
*  source code.
*
*  \note The <I>OpenPOWER ELF2 ABI</I> does define <B>bcd_mul10</B>
*  which (from the description) will actually generate Decimal Shift
*  (<B>bcds</B>). This instruction shifts 4-bit nibbles (BCD digits)
*  left or right while preserving the BCD sign nibble in bits 124-127,
*  While this is a handy instruction to have, it is not the same
*  operation as <B>vec_vmul10uq</B>, which is a true 128-bit binary
*  multiply by 10.  As of this writing <B>bcd_mul10</B> support is not
*  included in the latest GCC trunk source code.
*
*  For <B>_ARCH_PWR8</B> and earlier we need a little grade school
*  arithmetic using <B>Vector Multiply Even/Odd Unsigned Halfword</B>.
*  This treats the vector __int128 as 8 16-bit binary digits.  We
*  multiply each of these 16-bit digits by 10, which is done in two
*  (even and odd) parts. The result is 4 32-bit (2 16-bit digits)
*  partial products for the even digits and 4 32-bit products for the
*  odd digits. The vector register the even product elements are the
*  higher order elements and odd product element are lower order.
*
*  The even digit partial products are offset right by 16-bits in the
*  register.  If we shift the even products left 1 (16-bit) digit,
*  the even digits are lined up in columns with the odd digits.  Now
*  we can sum across partial products to get the final 128 bit product.
*
*  Notice also the conditional code for endian around the
*  <B>vec_vmulouh</B> and <B>vec_vmuleuh</B> built-ins: \code
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
*  \endcode
*  This is due to the explicit decision to support little endian
*  (<B>LE</B>) element ordering for ELF V2 LE targets. This also
*  changes the meaning of even / odd element numbering and the
*  relationship to high and low (digit ordering).
*  Basically in <B>LE</B> it is the opposite of what the corresponding
*  instructions actually do.  So EVF V2 compilers will swap even and
*  odd during code generation.  This can be helpful for porting Intel
*  vector intrinsic codes to PowerISA.
*
*  But this is the wrong thing to do for this computation.  For LE
*  targets the compiler will generate code that swaps the high and low
*  order partial products. This miss-aligns the digit columns and
*  produces incorrect results.  So the pveclib implementation needs to
*  be endian sensitive and pre-swaps the partial product multiplies
*  for LE, to get the correct results.
*
*  Now we are ready to sum the partial product <I>digits</I> while
*  propagating the digit carries across the 128-bit product.
*  For <B>_ARCH_PWR8</B> we can use <B>Vector Add Unsigned Quadword
*  Modulo</B> which handles all the internal carries in hardware.
*  Before <B>_ARCH_PWR8</B> we only have <B>Vector Add Unsigned Word
*  Modulo</B> and <B>Vector Add and Write Carry-Out Unsigned Word</B>.
*
*  We see these instructions used in the <B>else</B> leg of the
*  pveclib <B>vec_adduqm</B> implementation above. We can assume that
*  this implementation is correct and tested for supported platforms.
*  So here we use another pveclib function to complete the
*  implementation of <B>Vector Multiply-by-10 Unsigned Quadword</B>
*
*  Again similarly for the carry / extend forms which can be combined
*  to support wider (256, 512,  1024, ...) extended decimal to binary
*  conversions.
*  \sa vec_mul10cuq, vec_mul10euq, and vec_mul10ecuq
*
*  And similarly for full 128-bit x 128-bit multiply which combined
*  with the add quadword carry / extended forms above can be used to
*  implement wider (256, 512, 1024, ...) multiply operations.
*  \sa vec_mulluq and vec_muludq
*
*  \subsubsection mainpage_sub_1_3 Background on the evolution  of <altivec.h>
*
*  The original
*  <a href="https://www.nxp.com/docs/en/reference-manual/ALTIVECPIM.pdf">
*  AltiVec (TM) Technology Programming Interface Manual</a>
*  defined the minimal vector extensions to the application binary
*  interface (ABI), new keywords (vector, pixel, bool) for defining
*  new vector types, and new operators (build-in functions).
*
*  - generic AltiVec operations, like vec_add()
*  - specific AltiVec operations (instructions, like vec_addubm())
*  - predicates computed from a AltiVec operation like vec_all_eq()
*
*  A generic operation generates specific instructions based on
*  the types of the actual parameters.
*  So a generic vec_add operation, with vector char parameters,
*  will generate the (specific) vector add unsigned byte modulo
*  (vaddubm) instruction.
*  Predicates are used within if statement conditional clauses
*  to access the condition code from vector operations that set
*  Condition Register 6 (vector SIMD compares and Decimal Integer
*  arithmetic and format conversions).
*
*  The PIM defined a set of compiler built-ins for vector instructions
*  (see section "4.4 Generic and Specific AltiVec Operations")
*  that compilers should support.
*  The document suggests that any requires typedefs and supporting
*  macros definitions be collected into an include file named <altivec.h>.
*
*  The built-ins defined by the PIM closely match the vector
*  instructions of the underlying PowerISA.
*  For example: vec_mul, vec_mule / vec_mulo, and vec_muleub / vec_muloub.
*  - vec_mul is defined for float and double and will (usually)
*  generate a single instruction for the type. This is a simpler case
*  as floating point operations usually stay in their lanes
*  (result elements are the same size as the input operand elements).
*  - vec_mule / vec_mulo (multiply even / odd) are defined for
*  integer multiply as integer products require twice as many bits
*  as the inputs (the results don't stay in their lane).
*  \par
*  The RISC philosophy resists and POWER Architecture avoids
*  instructions that write to more then one register.
*  So the hardware and PowerISA vector integer multiply generate
*  even and odd product results (from even and odd input elements)
*  from two instructions executing separately.
*  The PIM defines and compiler supports
*  these operation as overloaded built-ins
*  and selects the specific instructions based on the operand
*  (char or short) type.
*
*  This is complicated as the PowerISA evolves.
*  The the original Altivec (VMX) provided vector multiply (even / odd)
*  operations for byte (char) and halfword (short) integers.
*  Multiple even / odd word (int) instruction where not introduced
*  until PowerISA V2.07 (POWER8). PowerISA 2.07 also introduced vector
*  multiply word modulo which is included under the generic vec_mul.
*
*  As the PowerISA evolved adding; new vector (VMX) instructions,
*  new facilities (Vector Scalar Extended (VSX)),
*  and specialized vector categories (little endian, AES, SHA2, RAID),
*  these new operators where added to <altivec.h>.
*  This included new specific and generic operations and
*  additional vector element types (long (64-bit) int, __int128,
*  double and quad precision (__Float128) float).
*
*  However the PIM documents where primarily focused on embedded
*  processors and where not updated to include the vector extensions
*  implemented by the server processors.
*  So any documentation for new vector operations where relegated to
*  the various compilers.  This was a haphazard process and some
*  divergence in operation naming did occur between compilers.
*
*  In the run up to the POWER8 launch and the OpenPOWER initiative it
*  was recognized that switching to Little Endian would require and
*  new and well documented Application Binary Interface (<B>ABI</B>).
*  It was also recognized that new <altivec.h> extensions needed to be
*  documented in a common place so the various compilers could
*  implement a common vector build-in API. So ...
*
*  The
*  <a href="https://openpowerfoundation.org/?resource_lib=64-bit-elf-v2-abi-specification-power-architecture">
*  OpenPOWER ELF V2 application binary interface (ABI)</a>:
*  Chapter 6. Vector Programming Interfaces and Appendix A.
*  Predefined Functions for Vector Programming document the
*  current and proposed vector built-ins we expect all C/C++
*  compilers to implement for the PowerISA.
*
*  The ABI also defines many overloaded built-in functions as
*  generic operations. Here the compiler selects a specific PowerISA
*  implementation based on the operand (vector element) types.
*  The ABI also defines the (big/little) endian behavior and the
*  compiler may select different instructions based on the endian
*  of the target.
*
*  Also note that the vector element numbering
*  changes between big and little endian, and so does the meaning of
*  even and odd. Both effect what the compiler supports and the
*  instruction sequence generated.
*  - <B>vec_muleub</B> and <B>vec_muloub</B> (multiply even / odd
*  unsigned byte) are examples of non-overloaded built-ins provided by
*  the GCC compiler but not defined in the ABI.
*  One would assume these built-ins will generate the matching
*  instruction, however the GCC compiler will adjust the generated
*  instruction based on the target endian
*  (even / odd is reversed for little endian).
*
*  The ABI also defines vec_mul as an overloaded operation on integer
*  types, where only the low order half (modulo element size)
*  of the product is returned.
*  The PowerISA does not provide a direct multiply modulo instructions
*  for all the integer sizes / types. So this requires a multiple
*  instruction sequence to implement.
*  Also integer vec_mul is defined in the ABI as "phased in" and is
*  only implemented in the latest GCC versions.
*
*  This is a small sample of the complexity we encounter programming
*  at this low level (vector intrinsic) API. Partially this due to
*  RISC design philosophy where there is a trade-off software
*  complexity for simpler (hopefully faster) hardware design.
*
*  \subsection mainpage_sub2 pveclib is not a vector math library
*
*  The pveclib does not implement general purpose vector math operations.
*  These should continue to be developed and improved within existing
*  projects (ie LAPACK, OpenBLAS, ATLAS and libmvec).
*
*  We believe that pveclib will be helpful to implementors of vector
*  math libraries by providing a higher level, more portable,
*  and more consistent vector interface for the PowerISA.
*  Similarly for implementors of extended arithmetic, cryptographic,
*  compression/decompression, and pattern matching / search libraries.
*
**/

#endif /* __PVECLIB_MAIN_DOX_H  */
