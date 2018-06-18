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
* \brief A library of useful vector functions for POWER. This library
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
*  For example: the PowerISA 2.07 (POWER8) provides population count
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
*  PowerISA 3.0 (POWER9) did add a Vector Multiply-Sum Unsigned
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
*  POWER9 (PowerISA 3.0B).
*  Many of these operations can be implemented as inline function in
*  a few vector instructions on earlier PowerISA versions.
*  - Provide equivalent functions across versions of the compiler.
*  For example built-ins provided in later versions of the compiler
*  can be implemented as inline functions with inline asm in earlier
*  compiler versions.
*  - Provide complete arithmetic operations across supported C types.
*  For example multiply modulo and even/odd for int, long, and __int128.
*  - Provide complete extended arithmetic (carry / extend /
*  multiple high) operations across supported C types.
*  For example add / subtract with carry and extend for int, long,
*  and __int128.
*  - Provide higher order functions not provided directly by the PowerISA.
*  For example vector SIMD implementation for ASCII __isalpha, etc.
*  As another example full __int128 implementations of Count Leading Zeros,
*  Population Count, Shift left/right immediate, and integer divide.
*  - Such implementations should be small enough to inline and allow
*  the compiler opportunity to apply common optimization techniques.
*
*  \subsubsection mainpage_sub0_1 POWER Vector Library Intrinsic headers
*
*  The POWER Vector Library will be primarily delivered as C language
*  inline functions in headers files.
*  - vec_common_ppc.h Typedefs and helper macros
*  - vec_int128_ppc.h Operations on vector __int128 values
*  - vec_int64_ppc.h Operations on vector long int (64-bit) values
*  - vec_int32_ppc.h Operations on vector int (32-bit) values
*  - vec_int16_ppc.h Operations on vector short int (16-bit) values
*  - vec_char_ppc.h Operations on vector char (values) values
*  - vec_bcd_ppc.h Operations on vectors of Binary Code Decimal
*  and Zoned Decimal values
*  - vec_f128_ppc.h Operations on vector _Float128 values
*  - vec_f64_ppc.h Operations on vector double values
*  - vec_f32_ppc.h Operations on vector float values
*
*  \note The list above is more of an aspiration at this time.
*  You will not find all of these headers or complete operation set
*  and platform coverage in the current public github.
*  But many of these headers do exist in private trees as we work
*  on completing function and testing across compilers and PowerISA
*  versions.
*
*  The goal is to provide high quality implementations that adapt to
*  the specifics of the compile target (-mcpu=) and compiler
*  (<altivec.h>) version you are using. Initially pveclib will focus on
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
*  The PowerPC vector built-ins evolved from the original
*  <a href="https://www.nxp.com/docs/en/reference-manual/ALTIVECPIM.pdf">
*  AltiVec (TM) Technology Programming Interface Manual</a> (PIM).
*  The PIM defined the minimal extensions to the application binary
*  interface (ABI) required to support the Vector Facility.
*  This included new keywords (vector, pixel, bool) for defining
*  new vector types, and new operators (built-in functions) required
*  for any supporting and compliant C language compiler.
*
*  The vector built-in function support included:
*  - generic AltiVec operations, like vec_add()
*  - specific AltiVec operations (instructions, like vec_vaddubm())
*  - predicates computed from AltiVec operations, like vec_all_eq()
*  which are also generic
*
*  See \ref mainpage_sub2 for more details.
*
*  There are clear advantages with the compiler implementing the
*  vector operations as built-ins:
*  - The compiler can access the C language type information and
*  vector extensions to implement the function overloading
*  required to process generic operations.
*  - Built-ins can be generated inline, which eliminates function
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
*  Another issue is that generic operations were not
*  uniformly applicable across vector types.
*  For example:
*  - vec_add / vec_sub applied to float, int, short and char.
*  - Later compilers added support for double
*  (with POWER7 and the Vector Scalar Extensions (VSX) facility)
*  - Integer long (64-bit) and __int128 support for POWER8
*  (PowerISA 2.07B).
*
*  But vec_mul / vec_div did not:
*  - vec_mul applied to float (and later double, with POWER7 VSX).
*  - vec_mule / vec_mulo (Multiply even / odd elements)
*  applied to [signed | unsigned] integer short and char.
*  Later compilers added support for vector int after
*  POWER8 added vector multiply word instructions.
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
*  stopper, if you are on a tight schedule for your project.
*  Especially if you are not familiar with the evolving history of the
*  PowerISA and supporting compilers.
*
*  \subsubsection mainpage_sub_1_1 What can we do about this?
*
*  First the Binutils assembler is usually updated within weeks of the
*  public release of the PowerISA document. So while your compiler
*  may not support the latest vector operations as built-in operations,
*  an older compiler with an updated assembler,
*  may support the instructions as inline assembler.
*
*  Sequences of inline assembler instructions can be wrapped within
*  C language static inline functions and placed in a header files
*  for shared use. If you are careful with the input / output register
*  <I>constraints</I> the GCC compiler can provide local register
*  allocation and minimize parameter marshaling overhead. This is very
*  close (in function) to a specific Altivec (built-in) operation.
*
*  \note Using GCC's inline assembler can be challenging even for the
*  experienced programmer. The register constraints have grown in
*  complexity as new facilities and categories were added.
*  The fact that some (VMX) instructions are restricted to the original
*  32 Vector Registers (<B>VRs</B>) (the high half of the Vector-Scalar
*  Registers <B>VSRs</B>), while others (Binary and Decimal
*  Floating-Point) are restricted to the original 32 Floating-Point
*  Registers (<B>FPRs</B> (overlapping the low half of the VSRs), and
*  the new VSX instructions can access all 64 VSRs, is just one source
*  of complexity.
*  So it is very important to get your input/output constraints correct
*  if you want inline assembler code to work correctly.
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
*  Octet Immediate (vsldoi), to propagate the word carries through the
*  quadword.
*
*  For POWER8 and later, C vector integer (modulo) multiply can be
*  implemented in a single Vector Unsigned Word Modulo (<B>vmuluwm</B>)
*  instruction. This was added explicitly to address vectorizing loops
*  using int multiply in C language code.  And some newer compilers do
*  support generic vec_mul() for vector int. But this is not
*  documented. Similarly for char (byte) and short (halfword) elements.
*
*  POWER8 also introduced Vector Multiply Even Signed Word
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
*  doubleword, or quadword). This might take 10-12 instructions to
*  implement the next element size bigger then the current processor.
*  A full 128-bit by 128-bit multiply with 256-bit result only
*  requires 32 instructions on a POWER8 (using multiple word even/odd).
*
*  Also many of the operations missing from the vector facility,
*  exist in the Fixed-point, Floating-point,
*  or Decimal Floating-point scalar facilities.
*  There will be some loss of efficiency in the data transfer but
*  compared to a complex operation like divide or decimal conversions,
*  this can be a workable solution.
*  On older POWER processors (before power7/8) transfers between
*  register banks (GPR, FPR, VR) had to go through memory.
*  But with the VSX facility (POWER7) FPRs and VRs overlap with the
*  lower and upper halves of the 64 VSR registers.
*  So FPR <-> VSR transfer are 0-2 cycles latency.
*  And with power8 we have direct transfer (GPR <-> FPR | VR | VSR)
*  instructions in the 4-5 cycle latency range.
*
*  For example POWER8 added Binary Coded Decimal (<B>BCD</B>)
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
*  POWER8 and later, and 10-11 instructions for Power6/7.
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
*  Even then you need to be aware of the PowerISA versions and when
*  specific instructions where introduced.  This can be frustrating if
*  you just want to port your application to POWER for a quick
*  evaluation.
*
*  So you would like to start evaluating how to leverage this power
*  for key algorithms at the heart of your application.
*  - But you are working with an older POWER processor
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
*  Someone with the right background (knowledge of the PowerISA,
*  assembler level programming, compilers and the vector built-ins,
*  ...) can solve any of the issues described above. But you don't
*  have time for this.
*
*  There should be an easier way to exploit the POWER vector hardware
*  without getting lost in the details. And this extends beyond
*  classical vector (Single Instruction Multiple Data (SIMD))
*  programming to exploiting larger data width (128-bit and beyond),
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
*  (vsldoi) instructions. This is common in vector codes and the
*  compiler can usually reuse this register across several blocks of
*  code and inline functions.
*
*  For POWER7/8 these instructions are all 2 cycle latency and 2 per
*  cycle throughput.  The vadduwm / vaddcuw instruction pairs should
*  issue in the same cycle and execute in parallel.
*  So the expected latency for this sequence is 14 cycles.
*  For POWER8 the vadduqm instruction has a 4 cycle latency.
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
*  odd digits. The vector register (independent of endian);
*  the even product elements are higher order and odd product elements
*  are lower order.
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
*  order partial products. This misaligns the digit columns and
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
*  implementation of <B>Vector Multiply-by-10 Unsigned Quadword</B>.
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
*  \subsubsection  mainpage_sub_1_3 Returning extended quadword results.
*
*  Extended quadword add, subtract and multiply results can exceed the
*  width of a single 128-bit vector. A 128-bit add can produce 129-bit
*  results. A unsigned 128-bit by 128-bit multiply result can produce
*  256-bit results. This is simplified for the <I>modulo</I> case where
*  any result bits above the low order 128 can be discarded.
*  But extended arithmetic requires returning the full precision
*  result.  Returning double wide quadword results are a complication
*  for both RISC processor and C language library design.
*
*  \paragraph mainpage_sub_1_3_1 PowerISA and Implementation.
*
*  For a RISC processor, encoding multiple return registers forces hard
*  trade-offs in a fixed sized instruction format.  Also building a
*  vector register file that can support at least one (or more) double
*  wide register writes per cycle is challenging.  For a super-scalar
*  machine with multiple vector execution pipelines, the processor can
*  issue and complete multiple instructions per cycle. As most
*  operations return single vector results, this is a higher
*  priority than optimizing for double wide results.
*
*  The PowerISA addresses this by splitting these operations into two
*  instructions that execute independently. Here independent means that
*  given the same inputs, one instruction does not depend on the result
*  of the other. Independent instructions can execute out-of-order,
*  or if the processor has multiple vector execution pipelines,
*  can execute (issue and complete) concurrently.
*
*  The original VMX implementation had Vector Add/Subtract Unsigned
*  Word Modulo (<B>vadduwm</B> / <B>vsubuwm</B>), paired with
*  Vector Add/Subtract and Write Carry-out Unsigned Word
*  (<B>vaddcuw</B> / <B>vsubcuw</B>).
*  Most usage ignores the carry-out and only uses the add/sub modulo
*  instructions.  Applications requiring extended precision,
*  pair the add/sub modulo with add/sub write carry-out, to capture
*  the carry and propagate it to higher order bits.
*
*  The (four word) carries are generated into the same <I>word lane</I>
*  as the source addends and modulo result.
*  Propagating the carries require a separate shift (to align the
*  carry-out with the low order (carry-in) bit of the next higher word)
*  and another add word modulo.
*
*  POWER8 (PowerISA 2.07B) added full Vector Add/Subtract Unsigned
*  Quadword Modulo (<B>vadduqm</B> / <B>vsubuqm</B>) instructions,
*  paired with corresponding Write Carry-out instructions.
*  (<B>vaddcuq</B> / <B>vsubcuq</B>).
*  A further improvement over the word instructions was the addition
*  of three operand <I>Extend</I> forms which combine add/subtract
*  with carry-in (<B>vaddeuqm</B>, <B>vsubeuqm</B>,
*  <B>vaddecuq</B> and <B>vsubecuq</B>).
*  This simplifies propagating the carry-out into higher quadword
*  operations.
*  \sa vec_adduqm, vec_addcuq, vec_addeuqm, vec_addecuq
*
*  POWER9 (PowerISA 3.0B) added Vector Multiply-by-10 Unsigned
*  Quadword (Modulo is implied), paired with Vector Multiply-by-10 and
*  Write Carry-out Unsigned Quadword
*  (<B>vmul10uq</B> / <B>vmul10cuq</B>).
*  And the <I>Extend</I> forms (<B>vmul10euq</B> / <B>vmul10ecuq</B>)
*  simplifies the digit (0-9) carry-in for extended precision decimal
*  to binary conversions.
*  \sa vec_mul10uq, vec_mul10cuq, vec_mul10euq, vec_mul10ecuq
*
*  The VMX integer multiply operations are split into multiply even/odd
*  instructions by element size.
*  The product requires the next larger element size
*  (twice as many bits).
*  So a vector multiply byte would generate 16 halfword products
*  (256-bits in total).
*  Requiring separate even and odd multiply instructions cuts the
*  total generated product bits (per instruction) in half.
*  It also simplifies the hardware design by keeping the generated
*  product in adjacent element lanes.
*  So each vector multiply even or odd byte operation generates 8
*  halfword products (128-bits) per instruction.
*
*  This multiply even/odd technique applies to most element sizes from
*  byte up to doubleword.  The original VMX supports multiply even/odd
*  byte and halfword operations.  In the original VMX, arithmetic
*  operations where restricted to byte, halfword, and word elements.
*  Multiply halfword products fit within the integer word element.
*  No multiply byte/halfword modulo instructions were provided, but
*  could be implemented via a vmule, vmulo, vperm sequence.
*
*  POWER8 (PowerISA 2.07B) added multiply even/odd word and multiply
*  modulo word instructions.
*  \sa vec_muleuw, vec_mulouw, vec_muluwm
*
*  The latest PowerISA (3.0B for POWER9) does add a doubleword integer
*  multiply via
*  <B>Vector Multiply-Sum unsigned Doubleword Modulo</B>.
*  This is a departure from the Multiply even/odd byte/halfword/word
*  instructions available in earlier Power processors.
*  But careful conditioning of the inputs can generate the equivalent
*  of multiply even/odd unsigned doubleword.
*  \sa vec_msumudm, vec_muleud, vec_muloud
*
*  This (multiply even/odd) technique breaks down when the input
*  element size is quadword or larger.
*  A quadword integer multiply forces a different split.
*  The easiest next step would be a high/low split (like the
*  Fixed-point integer multiply).
*  A multiply low (modulo) quadword would be a useful function.
*  Paired with multiply high quadword provides the double quadword
*  product. This would provide the basis for higher (multi-quadword)
*  precision multiplies.
*  \sa vec_mulluq, vec_muludq
*
*  \paragraph mainpage_sub_1_3_2 C Language restrictions.
*
*  The Power Vector Library is implemented using C language (inline)
*  functions and this imposes its own restrictions.
*  Standard C language allows an
*  arbitrary number of formal parameters and one return value per
*  function.  Parameters and return values with simple C types are
*  normally transfered (passed / returned) efficiently in local
*  (high performance) hardware registers.
*  Aggregate types (struct, union, and arrays of arbitrary size)
*  are normally handled by pointer indirection.
*  The details are defined in the appropriate Application Binary
*  Interface (ABI) documentation.
*
*  The POWER processor provides lots (96) of registers so we want to
*  use registers wherever possible.  Especially when our application is
*  composed of collections of small functions.  And more especially
*  when these functions are small enough to inline and we want the
*  compiler to perform local register allocation and common
*  subexpression elimination optimizations across these functions.
*  The PowerISA defines 3 kinds of registers;
*  - General Purpose Registers (GPRs),
*  - Floating-point Registers (FPRs),
*  - Vector registers (VRs),
*
*  with 32 of each kind.
*  We will ignore the various special registers for now.
*
*  The PowerPC64 64-bit ELF (and OpenPOWER ELF V2) ABIs normally
*  pass simple arguments and return values in a single register
*  (of the appropriate kind) per value.
*  Arguments of aggregate types are passed as storage pointers in
*  General Purpose Registers (GPRs).
*
*  The language specification, the language implementation, and the
*  ABI provide some exceptions.
*  The C99 language adds _Complex floating types which are composed of
*  real and imaginary parts.  GCC adds _Complex integer types.
*  For PowerPC ABIs complex values are held in a pair of registers of
*  the appropriate kind.
*  C99 also adds double word integers as the <I>long long int</I> type.
*  This only matters for PowerPC 32-bit ABIs.
*  For PowerPC64 ABIs <I>long long</I> and <I>long</I> are both 64-bit
*  integers and are held in 64-bit GPRs.
*
*  GCC also adds the __int128 type for some targets including the
*  PowerPC64 ABIs. Values of __int128 type are held (for operations,
*  parameter passing and function return) in 64-bit GPR pairs.
*  GCC  adds __ibm128 and _Decimal128 floating point types which are
*  held in Floating-point Registers pairs.
*  GCC recently added the __float128 floating point type which are held
*  in single vector register.  Similarly for vector __int128.
*
*  GCC defines Generic Vector Extensions that allow typedefs for
*  vectors of various element sizes/types and generic SIMD
*  (arithmetic, logical, and element indexing) operations.
*  For PowerPC64 ABIs this is currently restricted to 16-byte vectors
*  as defined in <altivec.h>.
*  For currently available compilers attempts to define vector types
*  with larger (32 or 64 byte) <I>vector_size</I> values are treated
*  as arrays of scalar elements.  Only vector_size(16) variables are
*  passed and returned in vector registers.
*
*  The OpenPOWER 64-Bit ELF V2 ABI Specification makes specific
*  provisions for passing/returning <I>homogeneous aggregates</I>
*  of multiple like data types.  Such aggregates can be passed
*  as up to eight floating-point or  vector registers.
*  This is defined for the Little Endian ELF V2 ABI and is
*  not applicable to Big Endian ELF V1 targets.
*  Also current GCC versions, in common use,
*  do not fully implement this ABI feature.
*
*  So we have shown that there are mechanisms for functions to return
*  multiple vector register values.  But none are really practical at
*  this time as they not yet available (function or optimal code
*  generation) in current GCC compilers, that are in common use.
*
*  Returning pairs of vector __int128 values as _Complex
*  __float128 would be awkward at best. And it is not clear when or if
*  _Complex vector __int128 will be supported.  GCC's Generic Vector
*  Extensions are only implemented for vector_size(16).
*  And current GCC compilers can generate some sub-optimal code for
*  passing/returning <I>homogeneous aggregates</I> as suggested in the
*  OpenPOWER ABI.
*
*  \paragraph mainpage_sub_1_3_3 Subsetting the problem.
*
*  We can simplify this problem by remembering that:
*  - Only a subset of the pveclib functions need to return more than
*  one 128-bit vector.
*  - The PowerISA normally splits these cases into multiple
*  instructions anyway.
*  - So far these functions are small and fully inlined.
*
*  So we have two (or three) options given the current state of GCC
*  compilers in common use:
*  - Mimic the PowerISA and split the operation into two functions,
*  where each function only returns (up to) 128-bits of the result.
*  - Use pointer parameters to return a second vector value in
*  addition to the function return.
*  - Support both and let the user decide which works best.
*
*  The add/subtract quadword operations provide good examples.
*  For exmaple adding two 256-bit unsigned integer values and returning
*  the 257-bit (the high / low sum and the carry)result looks like this: \code
s1 = vec_vadduqm (a1, b1); // sum low 128-bits a1+b1
c1 = vec_vaddcuq (a1, b1); // write-carry from low a1+b1
s0 = vec_vaddeuqm (a0, b0, c1); // Add-extend high 128-bits a0+b0+c1
c0 = vec_vaddecuq (a0, b0, c1); // write-carry from high a0+b0+c1
*  \endcode
*  This sequence uses the built-ins from <altivec.h> and generates
*  instructions that will execute on POWER8 and POWER9.
*  The compiler must target POWER8 (-mcpu=power8) or higher.
*  In fact the compile will fail if the target is POWER7.
*
*  Now let's look at the pveclib version of these operations from
*  <vec_int128_ppc.h>: \code
s1 = vec_adduqm (a1, b1); // sum low 128-bits a1+b1
c1 = vec_addcuq (a1, b1); // write-carry from low a1+b1
s0 = vec_addeuqm (a0, b0, c1); // Add-extend high 128-bits a0+b0+c1
c0 = vec_addecuq (a0, b0, c1); // write-carry from high a0+b0+c1
*  \endcode
*  Looks almost the same but the operations do not use the 'v' prefix
*  on the operation name.
*  This sequence generates the same instructions for (-mcpu=power8)
*  as the <altivec.h> version above.
*  It will also generate a different (slightly longer) instruction
*  sequence for (-mcpu=power7) which is functionally equivalent.
*
*  The pveclib <vec_int128_ppc.h> header also provides a coding style
*  alternative: \code
s1 = vec_addcq (&c1, a1, b1);
s0 = vec_addeq (&c0, a0, b0, c1);
*  \endcode
*  Here vec_addcq combines the adduqm/addcuq operations into a
*  <I>add and carry quadword</I> operation.
*  The first parameter is a pointer to vector to receive the carry-out
*  while the 128-bit modulo sum is the function return value.
*  Similarly vec_addeq combines the addeuqm/addecuq operations
*  into a <I>add with extend and carry quadword</I> operation.
*
*  As these functions are inlined by the compiler the implied
*  store / reload of the carry can be converted into a simple
*  register assignment.
*  For (-mcpu=power8) the compiler should generate the same
*  instruction sequence as the two previous examples.
*
*  For (-mcpu=power7) these functions will expand into a different
*  (slightly longer) instruction sequence which is functionally
*  equivalent to the instruction sequence generated for (-mcpu=power8).
*
*  For older processors (power7 and earlier) and under some
*  circumstances instructions generated for this "combined form"
*  may perform better than the "split form" equivalent from the
*  second example.
*  Here the compiler may not recognize all the common subexpressions,
*  as the "split forms" are expanded before optimization.
*
*  \subsection mainpage_sub2 Background on the evolution  of <altivec.h>
*
*  The original
*  <a href="https://www.nxp.com/docs/en/reference-manual/ALTIVECPIM.pdf">
*  AltiVec (TM) Technology Programming Interface Manual</a>
*  defined the minimal vector extensions to the application binary
*  interface (ABI), new keywords (vector, pixel, bool) for defining
*  new vector types, and new operators (built-in functions).
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
*  The document suggests that any required typedefs and supporting
*  macro definitions be collected into an include file named
*  <altivec.h>.
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
*  instructions that write to more than one register.
*  So the hardware and PowerISA vector integer multiply generate
*  even and odd product results (from even and odd input elements)
*  from two instructions executing separately.
*  The PIM defines and compiler supports
*  these operations as overloaded built-ins
*  and selects the specific instructions based on the operand
*  (char or short) type.
*
*  This is complicated as the PowerISA evolves.
*  The original Altivec (VMX) provided vector multiply (even / odd)
*  operations for byte (char) and halfword (short) integers.
*  Multiple even / odd word (int) instructions were not introduced
*  until PowerISA V2.07 (POWER8). PowerISA 2.07 also introduced vector
*  multiply word modulo which is included under the generic vec_mul.
*
*  As the PowerISA evolved adding new vector (VMX) instructions,
*  new facilities (Vector Scalar Extended (VSX)),
*  and specialized vector categories (little endian, AES, SHA2, RAID),
*  these new operators were added to <altivec.h>.
*  This included new specific and generic operations and
*  additional vector element types (long (64-bit) int, __int128,
*  double and quad precision (__Float128) float).
*
*  However the PIM documents were primarily focused on embedded
*  processors and were not updated to include the vector extensions
*  implemented by the server processors.
*  So any documentation for new vector operations were relegated to
*  the various compilers.  This was a haphazard process and some
*  divergence in operation naming did occur between compilers.
*
*  In the run up to the POWER8 launch and the OpenPOWER initiative it
*  was recognized that switching to Little Endian would require and
*  new and well documented Application Binary Interface (<B>ABI</B>).
*  It was also recognized that new <altivec.h> extensions needed to be
*  documented in a common place so the various compilers could
*  implement a common vector built-in API. So ...
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
*  compiler may select different instructions based on the endianness
*  of the target.
*
*  Also note that the vector element numbering
*  changes between big and little endian, and so does the meaning of
*  even and odd. Both affect what the compiler supports and the
*  instruction sequence generated.
*  - <B>vec_muleub</B> and <B>vec_muloub</B> (multiply even / odd
*  unsigned byte) are examples of non-overloaded built-ins provided by
*  the GCC compiler but not defined in the ABI.
*  One would assume these built-ins will generate the matching
*  instruction, however the GCC compiler will adjust the generated
*  instruction based on the target endianness
*  (even / odd is reversed for little endian).
*
*  The ABI also defines vec_mul as an overloaded operation on integer
*  types, where only the low order half (modulo element size)
*  of the product is returned.
*  The PowerISA does not provide direct multiply modulo instructions
*  for all the integer sizes / types. So this requires a
*  multiple-instruction sequence to implement.
*  Also integer vec_mul is defined in the ABI as "phased in" and is
*  only implemented in the latest GCC versions.
*
*  This is a small sample of the complexity we encounter programming
*  at this low level (vector intrinsic) API. Partially this is due to
*  RISC design philosophy where there is a trade-off of software
*  complexity for simpler (hopefully faster) hardware design.
*
*
*  \subsection mainpage_sub3 pveclib is not a vector math library
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
