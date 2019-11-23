/*
 Copyright (c) [2019] Steven Munroe

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
      Steven Munroe
      Created on: Aug 24, 2019
      Steven Munroe, additional contributions for POWER9.
 */

#ifndef SRC_PVECLIB_VEC_INT512_PPC_H_
#define SRC_PVECLIB_VEC_INT512_PPC_H_

#include <pveclib/vec_int128_ppc.h>

/*!
 * \file  vec_int512_ppc.h
 * \brief Header package containing a collection of multiple precision
 * quadword integer computation functions implemented with 128-bit
 * PowerISA VMX and VSX instructions.
 *
 * PVECLIB vec_int128_ppc.h provides the 128x128-bit multiply and
 * 128-bit add with carry/extend operations. This is most of what we
 * need to implement multiple precision integer computation.
 * This header builds on those operations to build 256x256, 512x128,
 * 512x512, 1024x1024 and 2048x2048 multiplies. We also provide 512-bit
 * add with carry/extend operations as a general aid to construct
 * multiple quadword precision arithmetic.
 *
 * We provide static inline implementations for up to 512x512
 * multiplies and 512x512 add with carry/extend.
 * These in-line operations are provided as building blocks for
 * coding implementations of larger multiply and sum operations.
 * Otherwise the in-line code expansion is getting too large for
 * normal coding. So we also provide callable (static and dynamic)
 * library implementations as well
 * (\ref i512_libary_issues_0_0).
 *
 * \section i512_security_issues_0_0 Security related implications
 *
 * The challenge is delivering a 2048x2048 bit multiply, producing a
 * 4096-bit product, while minimizing cache and timing side-channel
 * exploits. The goal is to minimize the memory visibility of
 * intermediate products and sums and internal conditional logic
 * (like early exit optimizations). The working theory is to use
 * vector registers and operations and avoid storing intermediate
 * results. This implies:
 * - While the final 4096-bit product is so large (32 quadwords),
 *   it requires a memory buffer for the result, we should not use
 *   any part of this buffer to hold intermediate partial sums.
 * - The 2048-bit multiplicands are also large (2 x 16 quadwords) and
 *   will be passed in memory buffers that are effectively constant.
 * - All intermediate partial products and sums should be held
 *   in vector registers (VSRs) until quadwords of the final product
 *   are computed and ready to store into the result buffer.
 * - Avoid conditional logic that effects function timing based on
 *   values of the inputs or results.
 * - Internally the code can be organized as straight line code or
 *   loops, in-line functions or calls to carefully crafted leaf
 *   functions, as long as the above goals are met.
 *
 * Achieving these goals requires some knowledge of the Application
 * Binary Interface (ABI) and foibles of the Instruction Set
 * Architecture (PowerISA) and how they impact what the compiler can
 * generate. The compiler itself has internal strategies (and foibles)
 * that need to be managed as well.
 *
 * \subsection i512_security_issues_0_0_0 Implications of the ABI
 *
 * The computation requires a number of internal temporary vectors
 * in addition to the inputs and outputs.
 * The Power Architecture, 64-Bit ELF V2 ABI Specification
 * (AKA the ABI) places some generous but important restrictions on
 * how the compiler generates code (and how compliant assembler code
 * is written).
 * - Up to 20 volatile vector registers v0-v19 (VSRs vs32-vs51) of which
 * 12 can be used for function arguments/return values.
 *  - Up to 12 vector arguments are passed in vector registers v2-v13
 * (VSRs vs34-vs45).
 *  - Longer vector argument lists are forced into the callers
 *  parameter save area (Stack pointer +32).
 *  - Functions can return a 128-bit vector value or a homogeneous
 *  aggregate of up to 8 vector values in vector registers v2-v9
 *  (VSRs 34-41).
 *  - Wider (8 x vectors) function return values are returned in
 *  memory via a reference pointer passed as a hidden parameter in GPR 3.
 * - Up to 12 additional non-volatile vector registers v20-v31 (vs51-vs63).
 * Any non-volatile registers must be saved before use and restored
 * before function return.
 * - The lower half for the VSRs (vs0-vs31) are prioritized for scalar
 * floating-point operations. If a function is using vectors and but
 * not scalar floating-point then the lower VSRs are available for
 * vector logical and integer operations and temporary spill from
 * vector registers.
 *  - Up to 14 volatile float double (f0-f13) or vector registers (vs0-vs13).
 *  - Up to 18 non-volatile float double (f14-f31) or vector registers (vs14-vs31).
 * - All volatile registers are a considered “clobbered” after a function call.
 *  - So the calling function must hold any local vector variables in
 *  memory or non-volatile registers if the live range extends across
 *  the function call.
 *  - In-lining the called function allows the compiler to manage
 *  register allocation across the  whole sequence. This can reduce
 *  register pressure when the called function does not actually
 *  use/modify all the volatile registers.
 *
 * \subsubsection i512_security_issues_0_0_0_0 Implications for parameter passing and Product size
 *
 * Care is required in selecting the width (256, 512-bit etc) of
 * parameter and return values. Parameters totaling more then 12 vector
 * quadwords or return values totaling more then 8 vector quadwords
 * will be spilled to the callers parameter save area. This may expose
 * intermediate partial products to cache side-channel attacks.
 * A 512x128-bit multiply returning a 640-bit product and a 512x512-bit
 * multiply returning a 1024-bit product meets this criteria (both the
 * parameters and return values fit within the ABI limits). But a
 * 1024x128-bit multiply returning 1152-bits is not OK because the
 * 1152-bit return value requires 9 vector registers, which will be
 * returned in memory.
 *
 * Also if any of these sub-functions are used without in-lining, the
 * generated code must be inspected to insure it is not spilling any
 * local variables. In my experiments with GCC 8.1 the 128x128,
 * 256x256, and 512x128 multiplies all avoid spilling. However the
 * stand-alone 512x512 implementation does require saving 3
 * non-volatile registers. This can be eliminated by in-lining the
 * 512x512 multiply into the 2048x2048 multiply function.
 *
 * \note GCC compilers before version 8 have an incomplete design for
 * homogeneous aggregates of vectors and may generate sub-optimal code
 * for these parameters.
 *
 * \subsection i512_security_issues_0_0_1 Implications of the PowerISA
 *
 * The Power Instruction Set Architecture (PowerISA) also imposes some
 * restriction on the registers vector instructions can access.
 * - The original VMX (AKA Altivec) facility has 32 vector registers
 * and instruction encoding to access those 32 registers.
 *  - This original instruction set was incorporated unchanged into
 *  the later versions of the PowerISA.
 *  - When Vector Scalar Extended facility was added, the original
 *  VMX instructions where restricted to the upper 32 VSRs
 *  (original vector registers).
 * - VSX was originally focused on vector and scalar floating-point
 * operations. With a handful of vector logical/permute/splat
 * operations added for completeness. These instructions where
 * encoded to access all 64 VSRs.
 *  - All vector integer arithmetic operations remained restricted to
 *  the upper 32 VSRs (the original VRs).
 *  - Later versions of the PowerISA (POWER8/9) added new vector
 *  integer arithmetic operations. This includes word/doubleword
 *  multiply and doubleword/quadword add/subtract. But these are also
 *  encoded to access only 32 vector registers.
 *  - The lower VSRs can still be used hold temporaries and local
 *  variables for vector integer operations.
 *
 * \subsection i512_security_issues_0_0_2 Implications for the compiler
 *
 * The compiler has to find a path though the ABI and ISA restriction
 * above while it performs:
 * - function in-lining
 * - instruction selection
 * - instruction scheduling
 * - register allocation
 *
 * For operations defined in PVECLIB, most operations are defined in
 * terms of AltiVec/VSX Built-in Functions.
 * So the compiler does not get much choice for instruction selection.
 * The PVECLIB coding style does leverage C language vector extensions
 * to load constants and manage temporary variables.
 * Using compiler Altivec/VSX built-ins and vector extensions allows
 * the compiler visibility to and control of these optimizations.
 *
 * Internal function calls effectively <I>clobber</I>
 * all (34 VSRs) volatile registers.
 * As the compiler marshals parameters into ABI prescribed VRs
 * it needs to preserve previous live content for later computation.
 * Similarly for volatile registers not used for parameter passing
 * as they are assumed to be clobbered by the called function.
 * The compiler preserves local live variables before the call by copying
 * their contents to non-volatile registers or spilling to memory.
 * This may put more <I>register pressure</I> on the available
 * non-volatile registers.
 * Small to medium sized functions often require only a fraction of
 * the available volatile registers.
 * In this case, in-lining the function avoids the disruptive volatile
 * register clobber and allows better overall register allocation.
 * So there is a strong incentive to in-line local/static functions.
 *
 * These compiler optimizations are not independent processes.
 * For example specific VSX instruction can access all 64 VSRs,
 * others are restricted to the 32 VRs
 * (like vector integer instructions).
 * So the compiler prioritizes VRs (the higher 32 VSRs)
 * for allocation to vector integer computation.
 * While the lower 32 VSRs can be used for logical/permute operations
 * and as a <I>level 1</I> spill area for VRs.
 * These restrictions combined with code size/complexity can increase
 * <I>register pressure</I> to the point the compiler is forced to
 * spill active (or live) vector registers to secondary storage.
 * This secondary storage can be:
 * - other architected registers that are available for direct
 * transfer but not usable in the computation.
 * - Local variables allocated on the stack
 * - Compiler temporaries allocated on the stack.
 *
 * Instruction scheduling can increase register pressure by moving
 * (reordering) instructions.
 * This is more prevalent when there are large differences in
 * instruction latency in the code stream.
 * For example moving independent / long latency instructions earlier
 * and dependent / short latency instructions later.
 * This tends to increase the distance between the instruction that
 * sets a register result and the next instruction the uses that
 * result in its computation.
 * The distance between a registers set and use is called the
 * <I>live range</I>.
 * This also tends to increase the number of concurrently active
 * and overlapping live ranges.
 *
 * For this specific (multi-precision integer multiply) example,
 * integer multiple and add/carry/extend instructions predominate.
 * For POWER9, vector integer multiply instructions run 7 cycles,
 * while integer add/carry/extend quadword instruction run 3 cycles.
 * The compiler will want to move the independent multiply instructions
 * earlier while the dependent add/carry instructions are moved later
 * until the latency of the (multiply) instruction
 * (on which it depends) is satisfied.
 * Moving dependent instructions apart and moving independent
 * instructions into the scheduling gap increases register pressure.
 *
 * In extreme cases, this can get out of hand.
 * At high optimization levels, the compiler can push instruction
 * scheduling to the point that it runs out of registers.
 * This forces the compiler to spill live register values,
 * splitting the live range into two smaller live ranges. Any spilled
 * values have to be reloaded later so they can used in computation.
 * This causes the compiler to generate more instructions
 * that need additional register allocation and scheduling.
 *
 * \note A 2048x2048-bit multiply is definitely an extreme case.
 * The implementation requires 256 128x128-bit multiplies,
 * where each 128x128-bit multiply requires 18-30 instructions.
 * The POWER9 implementation requires 1024 vector doublewword multiplies
 * plus 2400+ vector add/carry/extend quadword instructions.
 * When implemented as straight line code and expanded in-line
 * (<I>attribute (flatten)</I>) the total runs over 6000 instructions.
 *
 * Compiler spill code usually needs registers in addition
 * (perhaps of a different class) to the registers being spilled.
 * This can be as simple as moving to a register of the same size but
 * different class.
 * For example, register moves to/from VRs and the lower 32 VSRs.
 * But it gets more complex when spilling vector registers to memory.
 * For example, vector register spill code needs GPRs to compute stack
 * addresses for vector load/store instructions.
 * Normally this OK, unless the the spill code consumes so many GPRs
 * that it needs to spill GPRs.
 * In that case we can see serious performance bottlenecks.
 *
 * But remember that a primary goal (\ref i512_security_issues_0_0) was
 * to avoid spilling intermediate results to memory.
 * Spilling between high and low VSRs is acceptable
 * (no cache side-channel),
 * but spilling to memory must be avoided.
 * The compiler should have heuristics to back off in-lining and
 * scheduling-driven code motions just enough
 * to avoid negative performance impacts.
 * But this is difficult to model and may not handle all cases with
 * equal grace.
 * Also this may not prevent spilling VRs to memory if the compiler
 * scheduler's cost computation indicates that is an acceptable
 * trade-off.
 *
 * So we will have to directly override compiler settings and
 * heuristics to guarantee the result we want/need.
 * The PVECLIB implementation already marks most operations as
 * <B>static inline</B>.
 * But as we use these inline operations as building blocks to implement
 * larger operations we can push the resulting code size over the
 * compiler's default inline limits (<B>-finline-limit</B>).
 * Then compiler will stop in-lining for the
 * duration of compiling the current function.
 *
 * This may require stronger options/attributes to the compiler like
 * (<I>attribute (always_inline)</I>), (<I>attribute (gnu_inline)</I>),
 * or (<I>attribute (flatten)</I>).
 * The first two are not any help unless you are compiling at lower
 * optimization level (<B>-O0</B> or <B>-O1</B>). <B>-O2</B> defaults
 * to <B>-finline-small-functions</B> and <B>-O3</B> defaults to the
 * stronger <B>-finline-functions</B>.
 * However <I>attribute (flatten)</I> seems do exactly what we want.
 * Every call inside this function is in-lined unless explicitly told
 * not to (<I>attribute (noinline)</I>).
 * It seems that <I>attribute (flatten)</I> ignores the
 * <B>-finline-limit</B>.
 *
 * \note You should be compiling PVECLIB applications at <B>-O3</B> anyway.
 *
 * Now we have a large block of code for the compiler's instruction
 * scheduler to work on.
 * In this case the code is very repetitive
 * (multiply, add the column, generate carries, repeat).
 * The instruction will have lots of opportunity for scheduling long vs
 * short latency instructions and create new and longer live ranges.
 *
 * /note In fact after applying <I>attribute (flatten)</I> to
 * vec_mul2048x2048_PWR9 we see a lot of spill code. This expands
 * the code to over 9300 instructions with ~3300 instructions
 * associated with spill code.
 *
 * We need a mechanism to limit (set boundaries) on code motion
 * while preserving optimization over smaller blocks of code.
 * This normally called a <I>compiler fence</I> but there are multiple
 * definitions so we need to be careful what we use.
 *
 * We want something that will prevent the compiler from moving
 * instructions (in either direction) across specified
 * <I>lines in the code</I>.
 *
 * We don't need an atomic memory fence
 * (like <B> __atomic_thread_fence</B> or <B>__sync_synchronize</B>)
 * that forces the processor to order loads and stores relative to a
 * specific synchronization point.
 *
 * We don't need a compiler memory fence
 * (like <B>asm ("" ::: "memory")</B>).
 * The "memory" clobber forces GCC to assume that any memory may be
 * arbitrarily read or written by the asm block.
 * So any registers holding live local variables will be forced to
 * memory before and need to be reloaded after.
 * This prevents the compiler from reordering loads, stores, and
 * arithmetic operations across it, but does not prevent the processor
 * from reordering them.
 *
 * \note POWER process have an aggressively <I>Speculative Superscalar</I>
 * design with out-of-order issue and execution.
 *
 * Neither of the above are what we want for this case.
 * We specifically want to avoid
 * memory side effects in this computation.
 * We only need the minimal compiler fence
 * (like <B>asm (";" :::)</B>)
 * that prevents the compiler from reordering any code across it
 * but does not prevent the processor from reordering them.
 *
 * By placing this compiler fence between multiply/sum stages of
 * vec_mul512x128_inline(), vec_mul512x512_inline() and
 * vec_mul2048x2048() we limit instruction scheduling and code motion
 * to smaller code blocks. This in turn reduces register pressure to
 * the point where all 64 VSRs are in use, but no spilling to stack
 * memory is required.
 *
 * \todo The above above is true for vec_mul2048x2048_PWR9
 * but vec_mul2048x2048_PWR8 does kick out a few vector spills.
 * Need to look into this and resolve.
 *
 * \todo Look into adding multiply-sum forms of 512x128 and 512x512
 * defined to add to the low order product and propagate the carries.
 * The msum512x128s128 and msum512x512s512 operations will not generate
 * a carry out of 640-bit or 1024-bit product-sum and still fit within
 * the homogeneous aggregate size limits for return values. This will
 * reduce the live range for column sums and should reduce register
 * pressure.
 *
 *
 * \subsection i512_security_issues_0_0_3 So what does this all mean?
 *
 * The 2048x2048 multiplicands and the resulting product are so large
 * (8192-bits, 64 quadwords total) that at the outer most function the
 * inputs and the result must be in memory and passed by reference.
 * The implementation of a 2048x2048-bit multiply requires 256
 * 128x128-bit multiplies.
 * Otherwise the code can be organized into sub-functions generating
 * intermediate partial products and sums.
 *
 * Coding 256 128x128 products and generating column sums would be
 * tedious. One approach builds up products into larger and larger
 * blocks in stages. For example code a vec_mul512x128_inline()
 * operation then use that in the implementation of
 * vec_mul512x512_inline(). We also provide 512-bit add/carry/extend
 * operations to simplify generating sums of 512-bit partial products.
 * Then load blocks of 512-bits (4 quadwords,
 * 64-bytes) using vec_mul512x512_inline() to produce a 1024-bit
 * partial product (\ref i512_security_issues_0_0_0_0).
 *
 * Then multiply the 512-bit blocks across one 2048-bit (4 x 512-bit)
 * multiplicand. The completion of a 2048x512-bit partial product
 * (of 2560-bits) includes the low order 512-bits ready to store to
 * the output operand. Repeat for each 512-bit block of the other
 * 2048-bit multiplicand summing across the 512-bit columns.
 * The final sum, after the final 2048x512 partial
 * product, produces the high order 2048-bits of the 2048x2048 product
 * ready to store to the output operand.
 *
 * \note Security aware implementations could use masking
 * countermeasures associated with these load/store operations.
 * The base PVECLIB implementation does not do this.
 * The source is available in ./src/vec_int512_runtime.c.
 *
 * It is best if the
 * sub-functions code can be fully in-lined into the 2048x2048-bit
 * multiply or the sub-functions are carefully written. In this case
 * these sub-functions should be leaf-functions (does not call other
 * functions) and can execute without spilling register state or
 * requiring stored (by reference) parameters.
 *
 * All levels of implementation should
 * avoid conditional logic based on values of inputs or partial
 * products (For example early exits for leading or trailing zero
 * quadwords).
 * Doing so may expose the multiply function to timing side-channel
 * attacks. So the best case would be one large function implemented
 * as straight-line code.
 *
 * We will need all 64 VSX registers for operations and local
 * variables. So the outer function will need to allocate a
 * stack-frame and save all of the non-volatile floating point
 * registers (allowing the use of  vs14-vs31 for local vector
 * variables) and vector registers (v20-v31 AKA vs51-vs63) on
 * entry. This frees up (18+12=) 30 additional quadword registers
 * for local vector variables within the outer multiply function.
 *
 * These saved registers reflect the state of the calling (or higher)
 * function and may not have any crypto sensitive content.
 * These register save areas will not be updated with internal state
 * from the 2048x2048-bit multiply operation itself.
 *
 * The 128x128-bit vector multiply is implemented with Vector
 * Multiply-Sum Unsigned Doubleword Modulo for Power9 and Vector
 * Multiply Even/Odd Unsigned Word for Power8. The timing for vector
 * integer multiply operations are fixed at 7 cycles latency for
 * Power8/9. The sums of partial products are implemented with Vector
 * Add Unsigned Quadword Modulo/write-Carry/Extended. The timing of
 * integer add quadword operations are fixed at 4 cycles for Power8
 * and 3 cycles for Power9. The rest of the 128x128-bit multiply
 * operation is a combination of Vector Doubleword Permute Immediate,
 * Vector Shift Left Double by Octet Immediate, Vector Splats, and
 * Vector Logical Or (used as a vector register move spanning the 64
 * VSRs). All of these have fixed timings of 2 or 3 cycles.
 *
 * So the overall timing of the 2048x2048-bit multiply should be
 * consistent independent of input values. The only measurable
 * variations would be as the processor changes Simultaneous
 * Multithreading (SMT) modes (controlled by the virtual machine and
 * kernel). The SMT mode (1,2,4,8) controls each hardware thread's
 * priority to issue instructions to the core and if the instruction
 * stream is dual or single issue (from that thread's perspective).
 *
 * But the better news is that with some extra function attributes
 * (always_inline and flatten) the entire 2048x2048 multiply function
 * can be flattened into a single function of straight line code
 * (no internal function calls or conditional branches) running ~6.3K
 * instructions. And no spill code was generated for local variables
 * (no register spill within the function body).
 *
 * \section i512_Endian_issues_0_0 Endian for Multi-quadword precision operations
 *
 * \todo Describe the background and rationale
 *
 * As described in \ref mainpage_endian_issues_1_1
 * and \ref i128_endian_issues_0_0
 * supporting both big and little endian in a single implementation
 * has its challenges. But I think we can leave the details of
 * quadword operations to the vec_int128_ppc.h implementation.
 * The decision needed for these implementations is how the quadwords
 * of a multi-quadword integer are ordered in storage.
 * For example given an array or structure of 16 quadwords
 * representing a single 2048-bit binary number which quadword
 * contains the low order bits and which the high order bits.
 *
 * This is largely arbitrary and independent from the system endian.
 * But we should be consistent within the API defined by this header
 * and PVECLIB as a whole.
 * Placing the low order bits in the first (lowest address in memory)
 * quadword and the high order bits in last (highest address in
 * memory) quadword would be consistent with little endian.
 * While placing the high order bits in the first (lowest address in
 * memory) quadword and the low order bits in last (highest address in
 * memory) quadfword would be consistent with big endian.
 * Either is valid internal to the implementation where the key issue
 * is accessing the quadwords of the multiplicands is a convenient
 * order to generate the partial products in an order that support
 * efficient generation of column sums and carries.
 *
 * It is best for the API if the order of quadwords in multi-quadword
 * integers match the endian of the platform. This should be
 * helpful where we want the use the PVEVLIB implementations
 * under existing APIs using arrays of smaller integer types.
 *
 * So on powerpc64le systems the low order quadword is the first
 * quadword. While on older powwerpc64 systems the high order quadword
 * is the first quadword. For example we can represent a 512-bit
 * integer with the following structure.
 * \code
 typedef struct
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  vui128_t vx0;
  vui128_t vx1;
  vui128_t vx2;
  vui128_t vx3;
#else
  vui128_t vx3;
  vui128_t vx2;
  vui128_t vx1;
  vui128_t vx0;
#endif
} __VEC_U_512;
 * \endcode
 * In this example the field <I>vx0</I> is always the low order quadword
 * and <I>vx3</I> is always the high order quadword, independent of endian.
 * We repeat this pattern for the range of multi-quadword integer sizes
 * (from __VEC_U_256 to __VEC_U_4096) supported by this header.
 * In each case the field name <I>vx0</I> is consistently the low order
 * quadword. The field name suffix numbering continues from low to high
 * with the highest numbered field name being the high order quadword.
 *
 * \subsection i512_Endian_issues_0_0_1 Multi-quadword Integer Constants
 *
 * As we have seen, initializing larger multiple precision constants
 * an be challenging  (\ref int128_const_0_0_1).
 * The good news we can continue to to use aggregate initializers
 * for structures and arrays of vector quadwords. For example:
 *
 * \code
const __VEC_U_512 vec512_one =
    {
      (vui128_t) ((unsigned __int128) 0x00000000),
      (vui128_t) ((unsigned __int128) 0x00000000),
      (vui128_t) ((unsigned __int128) 0x00000000),
      (vui128_t) ((unsigned __int128) 0x00000001)
    };
 * \endcode
 * This example is in the expected high to low order for the 512-bit
 * constant 1. Unfortunately endian raises it ugly head again and this
 * would a different value on little endian platform.
 *
 * So PVECLIB provides another helper macro (CONST_VINT512_Q()) to
 * provide a consistent numbericial order for multiple quadword
 * constants. For example:
 * \code
const __VEC_U_512 vec512_one = CONST_VINT512_Q
    (
      (vui128_t) ((unsigned __int128) 0x00000000),
      (vui128_t) ((unsigned __int128) 0x00000000),
      (vui128_t) ((unsigned __int128) 0x00000000),
      (vui128_t) ((unsigned __int128) 0x00000001)
    );
 * \endcode
 * and
 * \code
// const for 10**128
const __VEC_U_512  vec512_ten128th = CONST_VINT512_Q
    (
      CONST_VUINT128_QxW (0x00000000, 0x00000000, 0x0000024e, 0xe91f2603),
      CONST_VUINT128_QxW (0xa6337f19, 0xbccdb0da, 0xc404dc08, 0xd3cff5ec),
      CONST_VUINT128_QxW (0x2374e42f, 0x0f1538fd, 0x03df9909, 0x2e953e01),
      CONST_VUINT128_QxW (0x00000000, 0x00000000, 0x00000000, 0x00000000)
    );
 * \endcode
 * Unfortunately the compiler can not help with multi-quadword decimal
 * constants. So we must resort to external tools like <B>bc</B> to
 * compute large constant values and convert them to hexadecimal which
 * are easier to break into words and doubleword. These can then be
 * used a constants in program source to represent arbitrarily large
 * binary values.
 *
 * \section i512_libary_issues_0_0 Putting the Library into PVECLIB
 *
 *
 * \todo General discussion of static and dynamic libraries should be
 * moved (eventually) to pveclibmaindox.h. While specifics of
 * multiple quadword precision integer multiply should remain here.
 *
 * Until recently (as of v1.0.3) PVECLIB operations were
 * <B>static inline</B> only. This was reasonable as most operations
 * were small (one to a few vector instructions).
 * This offered the compiler opportunity for:
 * - Better register allocation.
 * - Identifying common subexpressions and factoring them across
 * operation instances.
 * - Better instruction scheduling across operations.
 *
 * Even then, a few operations
 * (quadword multiply, BCD multiply, BCD <-> binary conversions,
 * and some POWER8/7 implementations of POWER9 instructions)
 * were getting uncomfortably large (10s of instructions).
 * But it was the multiple quadword precision operations that
 * forced the issue as they can run to 100s and sometimes 1000s of
 * instructions.
 *
 * \subsection i512_libary_issues_0_0_0 Building Multi-platform Libraries
 *
 * Building libraries of compiled binaries is not that difficult.
 * The challenge is effectively supporting multiple processor
 * (POWER7/8/9) platforms, as many PVECLIB operations have different
 * implementations for each platform.
 * This is especially evident on the multiply integer word, doubleword,
 * and quadword operations (see; vec_muludq(), vec_mulhuq(), vec_mulluq(),
 * vec_vmuleud(), vec_vmuloud(), vec_msumudm(), vec_muleuw(),
 * vec_mulouw()).
 *
 * This is dictated by both changes in the PowerISA and in
 * the micro-architecture as it evolved across processor generations.
 * So an implementation to run on a POWER7 is necessarily restricted to
 * the instructions of PowerISA 2.06.
 * But if we are running on a POWER9, leveraging new instructions from
 * PowerISA 3.0 can yield better performance than the POWER7
 * compatible implementation. When we are dealing with larger operations
 * (10s and 100s of instructions) the compiler can schedule instruction
 * sequences based on the platform (-mtune=) for better performance.
 *
 * So we need to deliver multiple implementations for some operations
 * and we need to provide mechanisms to select a specific platform
 * implementation statically at compile/build or dynamically at
 * runtime. First we need to compile multiple version of these
 * operations, as unique functions, each with a different effective
 * target platform (-mcpu=).
 *
 * Obviously creating multiple source files implementing the same large
 * operation, each supporting a specific target platform,
 * is a possibility.
 * However this could cause maintenance problems where changes to a
 * operation must be coordinated across multiple source files.
 * This is also inconsistent with the current PVECLIB coding style
 * where a file contains an operations complete implementation,
 * including documentation and platform implementation variants.
 *
 * The current PVECLIB implementation makes extensive use of CPP
 * conditions. These include testing for compiler version,
 * target Endian, and current target platform then selects the
 * appropriate source code snippet (\ref mainpage_sub_1_2).
 * This was intended to simplify the application/library developers
 * life were they could use the PVECLIB API and not worry about
 * these details.
 *
 * So far this works as intended (single vector source for multiple
 * PowerISA VMX/VSX targets) when the entire application is compiled
 * for a single target.
 * However this dependence on CPP conditionals is mixed blessing then
 * the application needs to support multiple platforms in a single package.
 *
 * \subsubsection i512_libary_issues_0_0_0_0 The mechanisms available
 * The compiler and ABI offer options that at first glance
 * seem to allow multiple target specific binaries from a single source.
 * Besides the compilers command level target options a number of
 * source level mechanisms to change the target.
 * These include:
 * - __ attribute __ (target ("cpu=power8"))
 * - __ attribute __ (target_clones ("cpu=power9,default"))
 * - \#pragma GCC target ("cpu=power8")
 *
 * The target and target_clones attributes are function attributes
 * (apply to single function).
 * The target attribute overrides the command line -mcpu= option.
 * However it is not clear which version of GCC added explicit support for (target ("cpu=").
 * This was not explicitly documented until GCC 5.
 * The target_clones attribute will cause GCC will create two function clones,
 * one compiled with the specified -mcpu= target and another with the default options.
 * It also creates a resolver function that dynamically selects a clone implementation
 * suitable for current platform architecture.
 * This PowerPC specific variant was not explicitly documented until GCC 8.
 *
 * There are a few issues with function attributes:
 * - The Doxygen preprocessor can not parse function attributes.
 * - The availability of these attributes seems to be limited to the latest compilers.
 *
 * But these is deeper problem related to the usage of CPP conditionals.
 * Many PVECLIB operation implementations depend on GCC/compiler predefined macros including:
 * - __ GNUC __
 * - __ GNUC_MINOR __
 * - __ BYTE_ORDER __
 * - __ ORDER_LITTLE_ENDIAN __
 * - __ ORDER_BIG_ENDIAN __
 *
 * PVECLIB also depends on many system-specific predefined macros including:
 * - __ ALTIVEC __
 * - __ VSX __
 * - __ FLOAT128 __
 * - _ARCH_PWR9
 * - _ARCH_PWR8
 * - _ARCH_PWR7
 *
 * PVECLIB also depends on the <altivec.h> include file which provides
 * the mapping between the ABI defined intrinsics and compiler defined
 * built-ins. In some places PVECLIB conditionally tests if specific
 * built-in is defined and substitutes an in-line assembler
 * implementation if not.
 * Altivec.h also depends on system-specific predefined macros to
 * enable/disable blocks of intrinsic built-ins based on PowerISA level.
 *
 * \subsubsection i512_libary_issues_0_0_0_1 Some things just do not work
 * The issue is that the compiler (GCC at least) only expands the
 * compiler and system-specific predefined macros once per source file.
 * They will not change due to embedded function attributes that change the target.
 * So the following do not work as expected.
 *
 * \code
#include <altivec.h>
#include <pveclib/vec_int128_ppc.h>
#include <pveclib/vec_int512_ppc.h>

// Defined in vec_int512_ppc.h but included here for clarity.
static inline __VEC_U_256
vec_mul128x128_inline (vui128_t a, vui128_t b)
{
  __VEC_U_256 result;
  // vec_muludq is defined in vec_int128_ppc.h
  result.vx0 = vec_muludq (&result.vx1, a, b);
  return result;
}

__VEC_U_256 __attribute__(target ("cpu=power7"))
vec_mul128x128_PWR7 (vui128_t m1l, vui128_t m2l)
{
  return vec_mul128x128_inline (m1l, m2l);
}

__VEC_U_256 __attribute__(target ("cpu=power8"))
vec_mul128x128_PWR8 (vui128_t m1l, vui128_t m2l)
{
  return vec_mul128x128_inline (m1l, m2l);
}

__VEC_U_256 __attribute__(target ("cpu=power9"))
vec_mul128x128_PWR9 (vui128_t m1l, vui128_t m2l)
{
  return vec_mul128x128_inline (m1l, m2l);
}
 * \endcode
 * For example if we assume that the compiler default is (or the
 * command line specifies) -mcpu=power8 the compiler will use this to
 * generate the system-specific predefined macros.
 * This is done before the first include file is processed.
 * In this case <altivec.h>, vec_int128_ppc.h, and vec_int512_ppc.h
 * source will be expanded for power8 (PowerISA-2.07).
 * So the vec_muludq and vec_muludq inline source implementations will
 * be the power8 version.
 *
 * This will all be established before the compiler starts to parse
 * and generate code for vec_mul128x128_PWR7.
 * This is likely to fail because the we are
 * trying to compile code containing power8 instructions with a
 * -mcpu=power7 target.
 *
 * The compilation of vec_mul128x128_PWR8 should work as we are
 * compiling power8 code with a -mcpu=power8 target.
 * The compilation of vec_mul128x128_PWR9 will compile without error
 * but will generate essentially the same code as vec_mul128x128_PWR8.
 * The -mcpu=power9 target allows that compiler to use power9
 * instructions but the expanded source coded from  vec_muludq and
 * vec_mul128x128_inline will not contain any power9 intrinsic
 * built-ins.
 *
 * Pragma GCC target has a similar issue if you try to change the
 * target multiple times within the source file.
 *
 * \code
#include <altivec.h>
#include <pveclib/vec_int128_ppc.h>
#include <pveclib/vec_int512_ppc.h>

// Defined in vec_int512_ppc.h but included here for clarity.
static inline __VEC_U_256
vec_mul128x128_inline (vui128_t a, vui128_t b)
{
  __VEC_U_256 result;
  // vec_muludq is defined in vec_int128_ppc.h
  result.vx0 = vec_muludq (&result.vx1, a, b);
  return result;
}

#pragma GCC push_options
#pragma GCC target ("cpu=power7")

__VEC_U_256
vec_mul128x128_PWR7 (vui128_t m1l, vui128_t m2l)
{
  return vec_mul128x128_inline (m1l, m2l);
}

#pragma GCC pop_options
#pragma GCC push_options
#pragma GCC target ("cpu=power8")

__VEC_U_256
vec_mul128x128_PWR8 (vui128_t m1l, vui128_t m2l)
{
  return vec_mul128x128_inline (m1l, m2l);
}

#pragma GCC pop_options
#pragma GCC push_options
#pragma GCC target ("cpu=power9")

__VEC_U_256
vec_mul128x128_PWR9 (vui128_t m1l, vui128_t m2l)
{
  return vec_mul128x128_inline (m1l, m2l);
}
 * \endcode
 * This has the same issues at the target attribute example above.
 * However you can use pragma GCC target if it proceeds the first
 * \#include in the source file.
 *
 * \code
#pragma GCC target ("cpu=power9")
#include <altivec.h>
#include <pveclib/vec_int128_ppc.h>
#include <pveclib/vec_int512_ppc.h>

// vec_mul128x128_inline is defined in vec_int512_ppc.h

__VEC_U_256
vec_mul128x128_PWR9 (vui128_t m1l, vui128_t m2l)
{
  return vec_mul128x128_inline (m1l, m2l);
}
 * \endcode
 * In this case the cpu=power9 option is applied before the compiler
 * reads the first include file and initializes the system-specific
 * predefined macros. So the CPP source expansion reflects the power9
 * target.
 *
 * \subsubsection i512_libary_issues_0_0_0_2 Some things that do work
 * This will require a unique implementation for each platform target.
 * We still prefer a single file implementation for each function
 * to improve maintenance. So we need to separate setting the platform
 * target from the implementation source. Also we need to provide a
 * unique extern symbol for each platform implementation.
 *
 * This can be handled with a simple macro to append a suffix based on
 * system-specific predefined macro settings.
 *
 * \code
#ifdef _ARCH_PWR9
#define __VEC_PWR_IMP(FNAME) FNAME ## _PWR9
#else
#ifdef _ARCH_PWR8
#define __VEC_PWR_IMP(FNAME) FNAME ## _PWR8
#else
#define __VEC_PWR_IMP(FNAME) FNAME ## _PWR7
#endif
#endif
 * \endcode
 * Then use __VEC_PWR_IMP() as function name wrapper in the implementation source file.
 *
 *
 * \code
 //
 //  \file  vec_int512_runtime.c
 //

#include <altivec.h>
#include <pveclib/vec_int128_ppc.h>
#include <pveclib/vec_int512_ppc.h>

// vec_mul128x128_inline is defined in vec_int512_ppc.h
__VEC_U_256
__VEC_PWR_IMP (vec_mul128x128) (vui128_t m1l, vui128_t m2l)
{
  return vec_mul128x128_inline (m1l, m2l);
}
 * \endcode
 * This simple strategy allows the collection of the larger function
 * implementations into a single source file and build object files
 * for multiple platform targets. For example collect all the multiple
 * precision quadword implementations into a source file named
 * <B>vec_int512_runtime.c</B>.
 *
 * This source file can be compiled multiple times for different
 * platform targets. The resulting object files have unique function
 * symbols due to the platform specific suffix provided by the
 * __VEC_PWR_IMP() macro.
 * There are a number of build strategies for this.
 *
 * For example, create a small source file named <B>vec_runtime_PWR8.c</B>that starts with the
 * target pragma and includes the multi-platform source file.
 * \code
 //
 //  \file  vec_runtime_PWR8.c
 //

#pragma GCC target ("cpu=power8")

#include "vec_int512_runtime.c"
 * \endcode
 * Similarly for <B>vec_runtime_PWR7.c</B>, <B>vec_runtime_PWR9.c</B>.
 *
 * \note Additional runtime source files can be included as needed.
 * Other multiple precision functions supporting BCD and BCD <-> binary
 * conversions are likely candidates.
 *
 * Then include these files in the Makefile.am list of sources to build.
 * \code
libpvec_la_SOURCES = vec_runtime_PWR9.c \
	vec_runtime_PWR8.c \
	vec_runtime_PWR7.c
 * \endcode
 *
 * Alternatively use Makefile.am rules to specify different -mcpu= compile options.
 * This simplifies the platform source files too:
 * \code
 //
 //  \file  vec_runtime_PWR8.c
 //

#include "vec_int512_runtime.c"
 * \endcode
 * Then add the -mcpu compile option to CFLAGS
 * \code
CFLAGS-vec_runtime_PWR7.c += -mcpu=power7
CFLAGS-vec_runtime_PWR8.c += -mcpu=power8
CFLAGS-vec_runtime_PWR9.c += -mcpu=power9
 * \endcode
 *
 * \todo Is there a way for automake to compile vec_int512_runtime.c
 * with -mcpu=power7 with -o vec_runtime_PWR7.o.
 * And similarly for PWR8/PWR9.
 *
 *
 * \subsection i512_libary_issues_0_0_1 Calling Multi-platform functions
 *
 * The next step is to provide mechanisms for applications to call
 * these functions via static or dynamic linkage.
 * For static linkage the application needs to reference a specific
 * platform variant of the functions name.
 * For dynamic linkage we will use <B>STT_GNU_IFUNC</B> symbol resolution
 * (a symbol type extension to the ELF standard).
 *
 * \subsubsection i512_libary_issues_0_0_1_1 Static linkage to platform specific functions
 * For static linkage the application is compiled for a specific
 * platform target (via -mcpu=). So function calls should be bound to
 * the matching platform specific implementations. The application
 * may select the platform specific function directly by defining
 * a <I>extern</I> and invoking the platform qualified function.
 *
 * Or simply use the __VEC_PWR_IMP() macro as wrapper for the function
 * name in the application.
 * This selects the appropriate platform specific implementation based
 * on the -mcpu= specified for the application compile.
 * For example.
 * \code
  k = __VEC_PWR_IMP (vec_mul128x128)(i, j);
 * \endcode
 *
 * This header provides the default platform qualified <I>extern</I>
 * declarations for provided functions based on the -mcpu=
 * specified for the compile of application including this header.
 * For example.
 * \code
extern __VEC_U_256
__VEC_PWR_IMP (vec_mul128x128) (vui128_t, vui128_t);
 * \endcode
 *
 * \subsubsection i512_libary_issues_0_0_1_2 Dynamic linkage to platform specific functions
 * Applications using dynamic linkage will call the unqualified
 * function symbol.
 * For example:
 * \code
extern __VEC_U_256
vec_mul128x128 (vui128_t, vui128_t);
 * \endcode
 *
 * This symbol's implementation has a special <B>STT_GNU_IFUNC</B>
 * attribute recognized by the dynamic linker which associates this
 * symbol with the corresponding runtime resolver function.
 * So in addition to any platform specific implementations we need to
 * provide the resolver function referenced by the <I>IFUNC</I> symbol.
 * For example:
 * \code
 //
 //  \file  vec_runtime_DYN.c
 //
extern __VEC_U_256
vec_mul128x128_PWR7 (vui128_t, vui128_t);

extern __VEC_U_256
vec_mul128x128_PWR8 (vui128_t, vui128_t);

extern __VEC_U_256
vec_mul128x128_PWR9 (vui128_t, vui128_t);

static
__VEC_U_256
(*resolve_vec_mul128x128 (void))(vui128_t, vui128_t)
{
#ifdef  __BUILTIN_CPU_SUPPORTS__
  if (__builtin_cpu_is ("power9"))
    return vec_mul128x128_PWR9;
  else
    {
      if (__builtin_cpu_is ("power8"))
        return vec_mul128x128_PWR8;
      else
        return vec_mul128x128_PWR7;
    }
#else // ! __BUILTIN_CPU_SUPPORTS__
    return vec_mul128x128_PWR7;
#endif
}

__VEC_U_256
vec_mul128x128 (vui128_t, vui128_t)
__attribute__ ((ifunc ("resolve_vec_mul128x128")));
 * \endcode
 *
 * On the program's first call to a <I>IFUNC</I> symbol, the dynamic
 * linker calls the resolver function associated with that symbol.
 * The resolver function performs a runtime check to determine the platform,
 * selects the (closest) matching platform specific function,
 * then returns that functions address to the dynamic linker.
 *
 * The dynamic linker stores this function address in the callers
 * Procedure Linkage Tables (PLT) before forwarding the call to the
 * resolved implementation.  Any subsequent calls to this
 * function symbol branch (via the PLT) directly to appropriate
 * platform specific implementation.
 *
 * \note The platform specific implementations we use here may be the
 * same platform specific object code we created for static linkage.
 *
 *
 * \subsection i512_libary_issues_0_0_2 Building Multi-platform library binaries
 * To complete the packaging of a multiple platform implementation we
 * need create static archive libraries and dynamically linked
 * <I>Shared Object</I> libraries.
 *
 * \note Here, platform means a specific hardware generation
 * (power7, power8, power9) within a hardware family. For IBM systems
 * this identifies a specific combination of Instruction Set
 * Architecture and processor design (micro-architecture).
 *
 *
 * \subsubsection i512_libary_issues_0_0_2_0 Building Static Archives
 * We use the <B>ar</B> command to create static archive libraries.
 * This utility collects any number of (relocatable) object files in
 * to a single archive file.
 * This utility also builds and includes an index of the symbols defined
 * in included object files.
 * As long as the function symbols are unique there is no problem
 * including multiple platform implementation in a single archive.
 *
 * When we use the <B>ld</B> (linker) command to link a program, application
 * provided object files are copied and relocated into to the program image.
 * Symbol references are resolved (bound) to symbols defined
 * within the collection of relocated objects.
 * For any remaining unresolved symbols, the linker will search the
 * symbol index of any provided archives. Matching objects are copied
 * from the archive into the program image, and relocated.
 * The linker then attempts to resolve any remaining unresolved symbol
 * references. This process continues until all the symbols are
 * resolved or all the archives are searched.
 *
 * If the application is statically linking to the PVECLIB archives
 * and using the __VEC_PWR_IMP() macro to qualify application references,
 * The linker matches and extracts only the appropriate platform specific
 * objects for inclusion in the application.
 *
 * \note Once an application is compiled and statically linked with
 * this method, it is bound to that specific platform implementation.
 * Upgrading to an improved implementation required relinking the
 * application. To change the platform target implementation,
 * the application must be recompiled (-mcpu=) and relinked
 * to PVECLIB.
 *
 *
 * \subsubsection i512_libary_issues_0_0_2_1 Building Dynamic Shared Objects
 * Linking to Shared Objects (AKA Dynamically linked libraries) is
 * more flexible than static linking.
 * Shared Objects are separate executable files where search and
 * selection of the specific shared object files to use
 * are delayed until the application is loaded for execution.
 * Similarly, binding of dynamic symbol references to actual data or
 * function addressed is delayed until application execution.
 * This allows programs and libraries to be developed and upgraded
 * independently.
 *
 * This separation of binaries and late binding enables Multi-platform
 * support within a single library package or even a single library.
 * Using the <B>STT_GNU_IFUNC</B> mechanism we can create a single
 * shared object library that contains multiple platform specific
 * implementations and custom <B>IFUNC</B> resolvers for each API
 * function (\ref i512_libary_issues_0_0_1_2).
 *
 * When linking the <B>libpvec</B> shared object file we need to
 * include:
 * - The object files containing platform specific implementations for
 * all supported platforms.
 * - The object files containing the <I>resolver function</I>
 * implementations and <I>indirect function</I> symbols
 * (__attribute__ ((ifunc ("resolver-function"))).
 * - Any additional object files containing platform independent
 * functions and data constants.
 *
 * Applications compiled to use these functions and linked to this
 * shared object will execute on any of the supported POWER platforms.
 * As the application starts up, the dynamic linker will bind these
 * function calls (with help from the resolver functions) to the
 * appropriate platform specific implementation.
 *
 */

/** \brief Generate a 512-bit vector unsigned integer constant from
 *  4 x quadword constants.
 *
 *  Combine 4 x quadwords constants into a 512-bit __VEC_U_512 constant.
 *  The 4 parameters are quadword integer constant values in high to
 *  low order. For example:
 *
 * \code
// 512-bit integer constant for 10**128
const __VEC_U_512  vec512_ten128th = CONST_VINT512_Q
    (
      CONST_VUINT128_QxW (0x00000000, 0x00000000, 0x0000024e, 0xe91f2603),
      CONST_VUINT128_QxW (0xa6337f19, 0xbccdb0da, 0xc404dc08, 0xd3cff5ec),
      CONST_VUINT128_QxW (0x2374e42f, 0x0f1538fd, 0x03df9909, 0x2e953e01),
      CONST_VUINT128_QxW (0x00000000, 0x00000000, 0x00000000, 0x00000000)
    );
 * \endcode
 *
 */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define CONST_VINT512_Q(__q0, __q1, __q2, __q3) {__q3, __q2, __q1, __q0}
#else
#define CONST_VINT512_Q(__q0, __q1, __q2, __q3) {__q0, __q1, __q2, __q3}
#endif


/*! \brief A vector representation of a 256-bit unsigned integer.
 *
 *  A homogeneous aggregate of 2 x 128-bit unsigned integer fields.
 *  The low order field is named vx0, progressing to the high order
 *  field vx1.
 */
typedef struct
{
  ///@cond INTERNAL
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  vui128_t vx0;
  vui128_t vx1;
#else
  vui128_t vx1;
  vui128_t vx0;
#endif
  ///@endcond
} __VEC_U_256;

/*! \brief A vector representation of a 512-bit unsigned integer.
 *
 *  A homogeneous aggregate of 4 x 128-bit unsigned integer fields.
 *  The low order field is named vx0, progressing to the high order
 *  field vx3.
 */
typedef struct
{
  ///@cond INTERNAL
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  vui128_t vx0;
  vui128_t vx1;
  vui128_t vx2;
  vui128_t vx3;
#else
  vui128_t vx3;
  vui128_t vx2;
  vui128_t vx1;
  vui128_t vx0;
#endif
  ///@endcond
} __VEC_U_512;

/*! \brief A vector representation of a 640-bit unsigned integer.
 *
 *  A homogeneous aggregate of 5 x 128-bit unsigned integer fields.
 *  The low order field is named vx0, progressing to the high order
 *  field vx4.
 *
 *  \note Useful for returning the result of a 512x128-bit multiply.
 */
typedef struct
{
  ///@cond INTERNAL
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  vui128_t vx0;
  vui128_t vx1;
  vui128_t vx2;
  vui128_t vx3;
  vui128_t vx4;
#else
  vui128_t vx4;
  vui128_t vx3;
  vui128_t vx2;
  vui128_t vx1;
  vui128_t vx0;
#endif
  ///@endcond
} __VEC_U_640;

/*! \brief A vector representation of a 512-bit unsigned integer
 * and a 128-bit carry-out.
 *
 *  A union of:
 *  - homogeneous aggregate of 640-bit unsigned integer.
 *  - struct of:
 *    - homogeneous aggregate of 512-bit unsigned integer.
 *    - vector representation of the carry out of 512-bit add.
 *  - The Carry out vector overlays the high-order 128-bits of the
 *  640-bit vector.
 *
 *  \note Useful for passing the carry-out of a 512-bit add into the
 *  carry-in of an extended add.
 *
 */
typedef union
{
  ///@cond INTERNAL
  __VEC_U_640 x640;
  struct
  {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    __VEC_U_512 v0x512;
    vui128_t v1x128;
#else
    vui128_t v1x128;
    __VEC_U_512 v0x512;
#endif
  } x2;
  ///@endcond
} __VEC_U_512x1;

/*! \brief A vector representation of a 1024-bit unsigned integer.
 *
 *  A homogeneous aggregate of 8 x 128-bit unsigned integer fields.
 *  The low order field is named vx0, progressing to the high order
 *  field vx7.
 */
typedef struct
{
  ///@cond INTERNAL
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  vui128_t vx0;
  vui128_t vx1;
  vui128_t vx2;
  vui128_t vx3;
  vui128_t vx4;
  vui128_t vx5;
  vui128_t vx6;
  vui128_t vx7;
#else
  vui128_t vx7;
  vui128_t vx6;
  vui128_t vx5;
  vui128_t vx4;
  vui128_t vx3;
  vui128_t vx2;
  vui128_t vx1;
  vui128_t vx0;
#endif
  ///@endcond
} __VEC_U_1024;

/*! \brief A vector representation of a 1152-bit unsigned integer.
 *
 *  A homogeneous aggregate of 9 x 128-bit unsigned integer fields.
 *  The low order field is named vx0, progressing to the high order
 *  field vx8.
 *
 *  \note Useful for returning the result of a 1024x128-bit multiply.
 *
 *  \note This structure does not qualify for parameter passing in
 *  registers (more than 8 registers are required) and will be passed
 *  in memory.
 */
typedef struct
{
  ///@cond INTERNAL
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  vui128_t vx0;
  vui128_t vx1;
  vui128_t vx2;
  vui128_t vx3;
  vui128_t vx4;
  vui128_t vx5;
  vui128_t vx6;
  vui128_t vx7;
  vui128_t vx8;
#else
  vui128_t vx8;
  vui128_t vx7;
  vui128_t vx6;
  vui128_t vx5;
  vui128_t vx4;
  vui128_t vx3;
  vui128_t vx2;
  vui128_t vx1;
  vui128_t vx0;
#endif
  ///@endcond
} __VEC_U_1152;

/*! \brief A vector representation of a 2048-bit unsigned integer.
 *
 *  A homogeneous aggregate of 16 x 128-bit unsigned integer fields.
 *  The low order field is named vx0, progressing to the high order
 *  field vx15.
 *
 *  \note This structure does not qualify for parameter passing in
 *  registers (more than 8 registers are required) and will be passed
 *  in memory.
 */
typedef struct
{
  ///@cond INTERNAL
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  vui128_t vx0;
  vui128_t vx1;
  vui128_t vx2;
  vui128_t vx3;
  vui128_t vx4;
  vui128_t vx5;
  vui128_t vx6;
  vui128_t vx7;
  vui128_t vx8;
  vui128_t vx9;
  vui128_t vx10;
  vui128_t vx11;
  vui128_t vx12;
  vui128_t vx13;
  vui128_t vx14;
  vui128_t vx15;
#else
  vui128_t vx15;
  vui128_t vx14;
  vui128_t vx13;
  vui128_t vx12;
  vui128_t vx11;
  vui128_t vx10;
  vui128_t vx9;
  vui128_t vx8;
  vui128_t vx7;
  vui128_t vx6;
  vui128_t vx5;
  vui128_t vx4;
  vui128_t vx3;
  vui128_t vx2;
  vui128_t vx1;
  vui128_t vx0;
#endif
  ///@endcond
} __VEC_U_2048;

/*! \brief A vector representation of a 1024-bit unsigned integer
 * as two 512-bit fields.
 *
 *  A union of:
 *  - homogeneous aggregate of 1024-bit unsigned integer.
 *  - struct of:
 *    - two x homogeneous aggregate of 512-bit unsigned integers.
 *
 *  \note Useful for summing partial products based on a
 *  512x512-bit multiply.
 *
 */
typedef union
{
  ///@cond INTERNAL
  __VEC_U_1024 x1024;
  struct
  {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    __VEC_U_512 v0x512;
    __VEC_U_512 v1x512;
#else
    __VEC_U_512 v1x512;
    __VEC_U_512 v0x512;
#endif
  } x2;
  ///@endcond
} __VEC_U_1024x512;

/*! \brief A vector representation of a 2048-bit unsigned integer
 * as 4 x 512-bit integer fields.
 *
 *  A union of:
 *  - homogeneous aggregate of 2048-bit unsigned integer.
 *  - struct of:
 *    - 4 x homogeneous aggregate of 512-bit unsigned integers.
 *
 *  \note Useful to access 512-bit blocks to pass to a 512x512-bit
 *  multiplies. These can be used as partial products in a larger
 *  2048x2048-bit multiply.
 *
 *  \note This structure does not qualify for parameter passing in
 *  registers (more than 8 registers are required) and will be passed
 *  in memory.
 *
 */
typedef union
{
  ///@cond INTERNAL
  __VEC_U_2048 x2048;
  struct
  {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    __VEC_U_512 v0x512;
    __VEC_U_512 v1x512;
    __VEC_U_512 v2x512;
    __VEC_U_512 v3x512;
#else
    __VEC_U_512 v3x512;
    __VEC_U_512 v2x512;
    __VEC_U_512 v1x512;
    __VEC_U_512 v0x512;
#endif
  } x4;
  ///@endcond
} __VEC_U_2048x512;

/*! \brief A vector representation of a 2176-bit unsigned integer.
 *
 *  A homogeneous aggregate of 17 x 128-bit unsigned integer fields.
 *  The low order field is named vx0, progressing to the high order
 *  field vx16.
 *
 *  \note Useful for returning the result of a 2048x128-bit multiply.
 *
 *  \note This structure does not qualify for parameter passing in
 *  registers (more than 8 registers are required) and will be passed
 *  in memory.
 */
typedef struct
{
  ///@cond INTERNAL
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  vui128_t vx0;
  vui128_t vx1;
  vui128_t vx2;
  vui128_t vx3;
  vui128_t vx4;
  vui128_t vx5;
  vui128_t vx6;
  vui128_t vx7;
  vui128_t vx8;
  vui128_t vx9;
  vui128_t vx10;
  vui128_t vx11;
  vui128_t vx12;
  vui128_t vx13;
  vui128_t vx14;
  vui128_t vx15;
  vui128_t vx16;
#else
  vui128_t vx16;
  vui128_t vx15;
  vui128_t vx14;
  vui128_t vx13;
  vui128_t vx12;
  vui128_t vx11;
  vui128_t vx10;
  vui128_t vx9;
  vui128_t vx8;
  vui128_t vx7;
  vui128_t vx6;
  vui128_t vx5;
  vui128_t vx4;
  vui128_t vx3;
  vui128_t vx2;
  vui128_t vx1;
  vui128_t vx0;
#endif
  ///@endcond
} __VEC_U_2176;

/*! \brief A vector representation of a 4096-bit unsigned integer.
 *
 *  A homogeneous aggregate of 32 x 128-bit unsigned integer fields.
 *  The low order field is named vx0, progressing to the high order
 *  field vx31.
 *
 *  \note This structure does not qualify for parameter passing in
 *  registers (more than 8 registers are required) and will be passed
 *  in memory.
 */
typedef struct
{
  ///@cond INTERNAL
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  vui128_t vx0;
  vui128_t vx1;
  vui128_t vx2;
  vui128_t vx3;
  vui128_t vx4;
  vui128_t vx5;
  vui128_t vx6;
  vui128_t vx7;
  vui128_t vx8;
  vui128_t vx9;
  vui128_t vx10;
  vui128_t vx11;
  vui128_t vx12;
  vui128_t vx13;
  vui128_t vx14;
  vui128_t vx15;
  vui128_t vx16;
  vui128_t vx17;
  vui128_t vx18;
  vui128_t vx19;
  vui128_t vx20;
  vui128_t vx21;
  vui128_t vx22;
  vui128_t vx23;
  vui128_t vx24;
  vui128_t vx25;
  vui128_t vx26;
  vui128_t vx27;
  vui128_t vx28;
  vui128_t vx29;
  vui128_t vx30;
  vui128_t vx31;
#else
  vui128_t vx31;
  vui128_t vx30;
  vui128_t vx29;
  vui128_t vx28;
  vui128_t vx27;
  vui128_t vx26;
  vui128_t vx25;
  vui128_t vx24;
  vui128_t vx23;
  vui128_t vx22;
  vui128_t vx21;
  vui128_t vx20;
  vui128_t vx19;
  vui128_t vx18;
  vui128_t vx17;
  vui128_t vx16;
  vui128_t vx15;
  vui128_t vx14;
  vui128_t vx13;
  vui128_t vx12;
  vui128_t vx11;
  vui128_t vx10;
  vui128_t vx9;
  vui128_t vx8;
  vui128_t vx7;
  vui128_t vx6;
  vui128_t vx5;
  vui128_t vx4;
  vui128_t vx3;
  vui128_t vx2;
  vui128_t vx1;
  vui128_t vx0;
#endif
  ///@endcond
} __VEC_U_4096;

/*! \brief A vector representation of a 4096-bit unsigned integer
 * as 8 x 512-bit integer fields.
 *
 *  A union of:
 *  - homogeneous aggregate of 4096-bit unsigned integer.
 *  - struct of:
 *    - 8 x homogeneous aggregate of 512-bit unsigned integers.
 *
 *  \note Useful to access 512-bit blocks to pass to a 512x512-bit
 *  multiplies. These can be used as partial products in a larger
 *  2048x2048-bit multiply.
 *
 *  \note This structure does not qualify for parameter passing in
 *  registers (more than 8 registers are required) and will be passed
 *  in memory.
 *
 */
typedef union
{
  ///@cond INTERNAL
  __VEC_U_4096 x4096;
  struct
  {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    __VEC_U_512 v0x512;
    __VEC_U_512 v1x512;
    __VEC_U_512 v2x512;
    __VEC_U_512 v3x512;
    __VEC_U_512 v4x512;
    __VEC_U_512 v5x512;
    __VEC_U_512 v6x512;
    __VEC_U_512 v7x512;
#else
    __VEC_U_512 v7x512;
    __VEC_U_512 v6x512;
    __VEC_U_512 v5x512;
    __VEC_U_512 v4x512;
    __VEC_U_512 v3x512;
    __VEC_U_512 v2x512;
    __VEC_U_512 v1x512;
    __VEC_U_512 v0x512;
#endif
  } x8;
  ///@endcond
} __VEC_U_4096x512;

/*! \brief A compiler fence to prevent excessive code motion.
 *
 *  We use the COMPILER_FENCE to limit instruction scheduling
 *  and code motion to smaller code blocks. This in turn reduces
 *  register pressure and avoids generating spill code.
 */
#if 0
#define COMPILE_FENCE __asm (";":::)
#else
#define COMPILE_FENCE __asm ("nop":::)
#endif

/*! \brief Macro to add platform suffix for static calls. */
#ifdef _ARCH_PWR9
#define __VEC_PWR_IMP(FNAME) FNAME ## _PWR9
#else
#ifdef _ARCH_PWR8
#define __VEC_PWR_IMP(FNAME) FNAME ## _PWR8
#else
#define __VEC_PWR_IMP(FNAME) FNAME ## _PWR7
#endif
#endif

/** \brief Vector Add 512-bit Unsigned Integer & Write Carry.
 *
 *  Compute the 512 bit sum of two 512 bit values a, b and
 *  produce the carry.
 *  The sum (with-carry) is returned as single 640-bit integer in a
 *  homogeneous aggregate structure.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  16   | 1/cycle  |
 *  |power9   |  12   | 1/cycle  |
 *
 *  @param a vector representation of a unsigned 512-bit integer.
 *  @param b vector representation of a unsigned 512-bit integer.
 *  @return homogeneous aggregate representation of the unsigned
 *  640-bit sum of a + b.
 */
static inline __VEC_U_640
vec_add512cu (__VEC_U_512 a, __VEC_U_512 b)
{
  __VEC_U_640 result;
  vui128_t mc, mp, mq;

  result.vx0 = vec_addcq (&mc, a.vx0, b.vx0);
  result.vx1 = vec_addeq (&mp, a.vx1, b.vx1, mc);
  result.vx2 = vec_addeq (&mc, a.vx2, b.vx2, mp);
  result.vx3 = vec_addeq (&result.vx4, a.vx3, b.vx3, mc);
  return result;
}

/** \brief Vector Add Extended 512-bit Unsigned Integer & Write Carry.
 *
 *  Compute the 512 bit sum of two 512 bit values a, b and
 *  1 bit value carry-in value c.
 *  Produce the carry out of the high order bit of the sum.
 *  The sum (with-carry) is returned as single 640-bit integer in a
 *  homogeneous aggregate structure.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  16   | 1/cycle  |
 *  |power9   |  12   | 1/cycle  |
 *
 *  @param a vector representation of a unsigned 512-bit integer.
 *  @param b vector representation of a unsigned 512-bit integer.
 *  @param c vector representation of a unsigned 1-bit carry.
 *  @return homogeneous aggregate representation of the unsigned
 *  640-bit sum of a + b + c.
 */
static inline __VEC_U_640
vec_add512ecu (__VEC_U_512 a, __VEC_U_512 b, vui128_t c)
{
  __VEC_U_640 result;
  vui128_t mp, mq;

  result.vx0 = vec_addeq (&mq, a.vx0, b.vx0, c);
  result.vx1 = vec_addeq (&mp, a.vx1, b.vx1, mq);
  result.vx2 = vec_addeq (&mq, a.vx2, b.vx2, mp);
  result.vx3 = vec_addeq (&result.vx4, a.vx3, b.vx3, mq);
  return result;
}

/** \brief Vector Add Extended 512-bit Unsigned Integer Modulo.
 *
 *  Compute the 512 bit sum of two 512 bit values a, b and
 *  1 bit value carry-in value c.
 *  The sum is returned as single 512-bit integer in a
 *  homogeneous aggregate structure.
 *  Any carry-out of the high order bit of the sum is lost.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  16   | 1/cycle  |
 *  |power9   |  12   | 1/cycle  |
 *
 *  @param a vector representation of a unsigned 512-bit integer.
 *  @param b vector representation of a unsigned 512-bit integer.
 *  @param c vector representation of a unsigned 1-bit carry.
 *  @return homogeneous aggregate representation of the unsigned
 *  512-bit sum of a + b + c.
 */
static inline __VEC_U_512
vec_add512eum (__VEC_U_512 a, __VEC_U_512 b, vui128_t c)
{
  __VEC_U_512 result;
  vui128_t mp, mq;

  result.vx0 = vec_addeq (&mq, a.vx0, b.vx0, c);
  result.vx1 = vec_addeq (&mp, a.vx1, b.vx1, mq);
  result.vx2 = vec_addeq (&mq, a.vx2, b.vx2, mp);
  result.vx3 = vec_addeuqm (a.vx3, b.vx3, mq);
  return result;
}

/** \brief Vector Add 512-bit Unsigned Integer Modulo.
 *
 *  Compute the 512 bit sum of two 512 bit values a, b.
 *  The sum is returned as single 512-bit integer in a
 *  homogeneous aggregate structure.
 *  Any carry-out of the high order bit of the sum is lost.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  16   | 1/cycle  |
 *  |power9   |  12   | 1/cycle  |
 *
 *  @param a vector representation of a unsigned 512-bit integer.
 *  @param b vector representation of a unsigned 512-bit integer.
 *  @return homogeneous aggregate representation of the unsigned
 *  512-bit sum of a + b.
 */
static inline __VEC_U_512
vec_add512um (__VEC_U_512 a, __VEC_U_512 b)
{
  __VEC_U_512 result;
  vui128_t mc, mp, mq;

  result.vx0 = vec_addcq (&mc, a.vx0, b.vx0);
  result.vx1 = vec_addeq (&mp, a.vx1, b.vx1, mc);
  result.vx2 = vec_addeq (&mc, a.vx2, b.vx2, mp);
  result.vx3 = vec_addeuqm ( a.vx3, b.vx3, mc);
  return result;
}

/** \brief Vector Add 512-bit to Zero Extended Unsigned
 *  Integer Modulo.
 *
 *  The carry-in is zero extended to the left before computing the
 *  512-bit sum a + c.
 *  The sum is returned as single 512-bit integer in a
 *  homogeneous aggregate structure.
 *  Any carry-out of the high order bit of the sum is lost.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  16   | 1/cycle  |
 *  |power9   |  12   | 1/cycle  |
 *
 *  @param a vector representation of a unsigned 512-bit integer.
 *  @param c vector representation of a unsigned 1-bit carry.
 *  @return homogeneous aggregate representation of the unsigned
 *  512-bit sum of a + c.
 */
__VEC_U_512
static inline vec_add512ze (__VEC_U_512 a, vui128_t c)
{
  __VEC_U_512 result;
  vui128_t mp, mq;

  result.vx0 = vec_adduqm (a.vx0, c);
  mq = vec_addcuq (a.vx0, c);
  result.vx1 = vec_adduqm (a.vx1, mq);
  mp = vec_addcuq (a.vx1, mq);
  result.vx2 = vec_adduqm (a.vx2, mp);
  mq = vec_addcuq (a.vx2, mp);
  result.vx3 = vec_adduqm (a.vx3, mq);
  return result;
}

/** \brief Vector Add 512-bit to Zero Extended2 Unsigned
 *  Integer Modulo.
 *
 *  The two carry-ins are zero extended to the left before Computing the
 *  512 bit sum a + c1 + c2.
 *  The sum is returned as single 512-bit integer in a
 *  homogeneous aggregate structure.
 *  Any carry-out of the high order bit of the sum is lost.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |  16   | 1/cycle  |
 *  |power9   |  12   | 1/cycle  |
 *
 *  @param a vector representation of a unsigned 512-bit integer.
 *  @param c1 vector representation of a unsigned 1-bit carry.
 *  @param c2 vector representation of a unsigned 1-bit carry.
 *  @return homogeneous aggregate representation of the unsigned
 *  512-bit sum of a + c1 + c2.
 */
__VEC_U_512
static inline vec_add512ze2 (__VEC_U_512 a, vui128_t c1, vui128_t c2)
{
  __VEC_U_512 result;
  vui128_t mp, mq;

  result.vx0 = vec_addeuqm (a.vx0, c1, c2);
  mq = vec_addecuq (a.vx0, c1, c2);
  result.vx1 = vec_adduqm (a.vx1, mq);
  mp = vec_addcuq (a.vx1, mq);
  result.vx2 = vec_adduqm (a.vx2, mp);
  mq = vec_addcuq (a.vx2, mp);
  result.vx3 = vec_adduqm (a.vx3, mq);
  return result;
}

/** \brief Vector 128x128bit Unsigned Integer Multiply.
 *
 *  Compute the 256 bit product of two 128 bit values a, b.
 *  The product is returned as single 256-bit integer in a
 *  homogeneous aggregate structure.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 56-64 | 1/cycle  |
 *  |power9   | 33-39 | 1/cycle  |
 *
 *  @param a vector representation of a unsigned 128-bit integer.
 *  @param b vector representation of a unsigned 128-bit integer.
 *  @return homogeneous aggregate representation of the unsigned
 *  256-bit product of a * b.
 */
static inline __VEC_U_256
vec_mul128x128_inline (vui128_t a, vui128_t b)
{
  __VEC_U_256 result;
  result.vx0 = vec_muludq (&result.vx1, a, b);
  return result;
}

/** \brief Vector 256x256-bit Unsigned Integer Multiply.
 *
 *  Compute the 512 bit product of two 256 bit values a, b.
 *  The product is returned as single 512-bit integer in a
 *  homogeneous aggregate structure.
 *
 *  \note We use the COMPILER_FENCE to limit instruction scheduling
 *  and code motion to smaller code blocks. This in turn reduces
 *  register pressure and avoids generating spill code.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |224-232| 1/cycle  |
 *  |power9   |132-135| 1/cycle  |
 *
 *  @param m1 vector representation of a unsigned 256-bit integer.
 *  @param m2 vector representation of a unsigned 256-bit integer.
 *  @return homogeneous aggregate representation of the unsigned
 *  512-bit product of m1 * m2.
 */
static inline __VEC_U_512
vec_mul256x256_inline (__VEC_U_256 m1, __VEC_U_256 m2)
{
  __VEC_U_512 result;
  vui128_t mc, mp, mq, mqhl;
  vui128_t mphh, mphl, mplh, mpll;
  mpll = vec_muludq (&mplh, m1.vx0, m2.vx0);
  COMPILE_FENCE;
  mp = vec_muludq (&mphl, m1.vx1, m2.vx0);
  mplh = vec_addcq (&mc, mplh, mp);
  mphl = vec_adduqm (mphl, mc);
  COMPILE_FENCE;
  mp = vec_muludq (&mqhl, m1.vx0, m2.vx1);
  mplh = vec_addcq (&mq, mplh, mp);
  mphl = vec_addeq (&mc, mphl, mqhl, mq);
  COMPILE_FENCE;
  mp = vec_muludq (&mphh, m1.vx1, m2.vx1);
  mphl = vec_addcq (&mq, mphl, mp);
  mphh = vec_addeuqm (mphh, mq, mc);

  result.vx0 = mpll;
  result.vx1 = mplh;
  result.vx2 = mphl;
  result.vx3 = mphh;
  return result;
}

/** \brief Vector 512x128-bit Unsigned Integer Multiply.
 *
 *  Compute the 640 bit product of 512 bit value m1 and
 *  128-bit value m2.
 *  The product is returned as single 640-bit integer in a
 *  homogeneous aggregate structure.
 *
 *  \note We use the COMPILER_FENCE to limit instruction scheduling
 *  and code motion to smaller code blocks. This in turn reduces
 *  register pressure and avoids generating spill code.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |224-232| 1/cycle  |
 *  |power9   |132-135| 1/cycle  |
 *
 *  @param m1 vector representation of a unsigned 512-bit integer.
 *  @param m2 vector representation of a unsigned 128-bit integer.
 *  @return homogeneous aggregate representation of the unsigned
 *  640-bit product of m1 * m2.
 */
static inline __VEC_U_640
vec_mul512x128_inline (__VEC_U_512 m1, vui128_t m2)
{
  __VEC_U_640 result;
  vui128_t mq3, mq2, mq1, mq0, mq, mc;
  vui128_t mpx0, mpx1, mpx2, mpx3;
  mpx0 = vec_muludq (&mq0, m1.vx0, m2);
  COMPILE_FENCE;
  mpx1 = vec_muludq (&mq1, m1.vx1, m2);
  mpx1 = vec_addcq (&mc, mpx1, mq0);
  COMPILE_FENCE;
  mpx2 = vec_muludq (&mq2, m1.vx2, m2);
  mpx2 = vec_addeq (&mq, mpx2, mq1, mc);
  COMPILE_FENCE;
  mpx3 = vec_muludq (&mq3, m1.vx3, m2);
  mpx3 = vec_addeq (&mc, mpx3, mq2, mq);
  mq3 = vec_adduqm (mc, mq3);
  COMPILE_FENCE;

  result.vx0 = mpx0;
  result.vx1 = mpx1;
  result.vx2 = mpx2;
  result.vx3 = mpx3;
  result.vx4 = mq3;
  return result;
}

/** \brief Vector 512x512-bit Unsigned Integer Multiply.
 *
 *  Compute the 640 bit product of 512 bit values m1 and m2.
 *  The product is returned as single 1024-bit integer in a
 *  homogeneous aggregate structure.
 *
 *  \note We use the COMPILER_FENCE to limit instruction scheduling
 *  and code motion to smaller code blocks. This in turn reduces
 *  register pressure and avoids generating spill code.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | ?? | 1/cycle  |
 *  |power9   | ?? | 1/cycle  |
 *
 *  @param m1 vector representation of a unsigned 512-bit integer.
 *  @param m2 vector representation of a unsigned 512-bit integer.
 *  @return homogeneous aggregate representation of the unsigned
 *  1028-bit product of a * b.
 */
static inline __VEC_U_1024
vec_mul512x512_inline (__VEC_U_512 m1, __VEC_U_512 m2)
{
  __VEC_U_1024 result;
  vui128_t mc, mp, mq;
  __VEC_U_640 mp3, mp2, mp1, mp0;

  mp0 = vec_mul512x128_inline (m1, m2.vx0);
  result.vx0 = mp0.vx0;
  COMPILE_FENCE;
  mp1 = vec_mul512x128_inline (m1, m2.vx1);
  result.vx1 = vec_addcq (&mq, mp1.vx0, mp0.vx1);
  result.vx2 = vec_addeq (&mp, mp1.vx1, mp0.vx2, mq);
  result.vx3 = vec_addeq (&mq, mp1.vx2, mp0.vx3, mp);
  result.vx4 = vec_addeq (&mp, mp1.vx3, mp0.vx4, mq);
  result.vx5 = vec_addcq (&result.vx6, mp1.vx4, mp);
  COMPILE_FENCE;
  mp2 = vec_mul512x128_inline (m1, m2.vx2);
  result.vx2 = vec_addcq (&mq, mp2.vx0, result.vx2);
  result.vx3 = vec_addeq (&mp, mp2.vx1, result.vx3, mq);
  result.vx4 = vec_addeq (&mq, mp2.vx2, result.vx4, mp);
  result.vx5 = vec_addeq (&mp, mp2.vx3, result.vx5, mq);
  result.vx6 = vec_addeq (&result.vx7, mp2.vx4, result.vx6, mp);
  COMPILE_FENCE;
  mp3 = vec_mul512x128_inline (m1, m2.vx3);
  result.vx3 = vec_addcq (&mq, mp3.vx0, result.vx3);
  result.vx4 = vec_addeq (&mp, mp3.vx1, result.vx4, mq);
  result.vx5 = vec_addeq (&mq, mp3.vx2, result.vx5, mp);
  result.vx6 = vec_addeq (&mp, mp3.vx3, result.vx6, mq);
  result.vx7 = vec_addeuqm (result.vx7, mp3.vx4, mp);
  COMPILE_FENCE;
  return result;
}

/** \brief Vector 128x128bit Unsigned Integer Multiply.
 *
 *  Compute the 256 bit product of two 128 bit values a, b.
 *  The product is returned as single 256-bit integer in a
 *  homogeneous aggregate structure.
 *
 *  \note This is the dynamic call ABI for IFUNC selection.
 *  The static implementations are vec_mul128x128_PWR8 and
 *  vec_mul128x128_PWR9. For static calls the __VEC_PWR_IMP() macro
 *  will add appropriate suffix based on the compile -mcpu= option.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | 56-64 | 1/cycle  |
 *  |power9   | 33-39 | 1/cycle  |
 *
 *  @param m1 vector representation of a unsigned 128-bit integer.
 *  @param m2 vector representation of a unsigned 128-bit integer.
 *  @return homogeneous aggregate representation of the unsigned
 *  256-bit product of a * b.
 */
extern __VEC_U_256
vec_mul128x128 (vui128_t m1, vui128_t m2);

/** \brief Vector 256x256-bit Unsigned Integer Multiply.
 *
 *  Compute the 512 bit product of two 256 bit values a, b.
 *  The product is returned as single 512-bit integer in a
 *  homogeneous aggregate structure.
 *
 *  \note This is the dynamic call ABI for IFUNC selection.
 *  The static implementations are vec_mul256x256_PWR8 and
 *  vec_mul256x256_PWR9. For static calls the __VEC_PWR_IMP() macro
 *  will add appropriate suffix based on the compile -mcpu= option.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |224-232| 1/cycle  |
 *  |power9   |132-135| 1/cycle  |
 *
 *  @param m1 vector representation of a unsigned 256-bit integer.
 *  @param m2 vector representation of a unsigned 256-bit integer.
 *  @return homogeneous aggregate representation of the unsigned
 *  512-bit product of m1 * m2.
 */
extern __VEC_U_512
vec_mul256x256 (__VEC_U_256 m1, __VEC_U_256 m2);

/** \brief Vector 512x128-bit Unsigned Integer Multiply.
 *
 *  Compute the 640 bit product of 512 bit value m1 and
 *  128-bit value m2.
 *  The product is returned as single 640-bit integer in a
 *  homogeneous aggregate structure.
 *
 *  \note This is the dynamic call ABI for IFUNC selection.
 *  The static implementations are vec_mul256x256_PWR8 and
 *  vec_mul256x256_PWR9. For static calls the __VEC_PWR_IMP() macro
 *  will add appropriate suffix based on the compile -mcpu= option.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   |224-232| 1/cycle  |
 *  |power9   |132-135| 1/cycle  |
 *
 *  @param m1 vector representation of a unsigned 512-bit integer.
 *  @param m2 vector representation of a unsigned 128-bit integer.
 *  @return homogeneous aggregate representation of the unsigned
 *  640-bit product of m1 * m2.
 */
extern __VEC_U_640
vec_mul512x128 (__VEC_U_512 m1, vui128_t m2);

/** \brief Vector 512x512-bit Unsigned Integer Multiply.
 *
 *  Compute the 1024 bit product of 512 bit values m1 and m2.
 *  The product is returned as single 1024-bit integer in a
 *  homogeneous aggregate structure.
 *
 *  \note This is the dynamic call ABI for IFUNC selection.
 *  The static implementations are vec_mul512x512_PWR8 and
 *  vec_mul512x512_PWR9. For static calls the __VEC_PWR_IMP() macro
 *  will add appropriate suffix based on the compile -mcpu= option.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | ~600  | 1/cycle  |
 *  |power9   | ~240  | 1/cycle  |
 *
 *  @param m1 vector representation of a unsigned 512-bit integer.
 *  @param m2 vector representation of a unsigned 512-bit integer.
 *  @return homogeneous aggregate representation of the unsigned
 *  1028-bit product of a * b.
 */
extern __VEC_U_1024
vec_mul512x512 (__VEC_U_512 m1, __VEC_U_512 m2);

/** \brief Vector 1024x1024-bit Unsigned Integer Multiply.
 *
 *  Compute the 2048 bit product of 1024 bit values m1 and m2.
 *  The product is returned as single 2048-bit integer in a
 *  homogeneous aggregate structure.
 *
 *  \note This is the dynamic call ABI for IFUNC selection.
 *  The static implementations are vec_mul1024x1024_PWR8 and
 *  vec_mul1024x1024_PWR9. For static calls the __VEC_PWR_IMP() macro
 *  will add appropriate suffix based on the compile -mcpu= option.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | ?? | 1/cycle  |
 *  |power9   | ?? | 1/cycle  |
 *
 *  @param p2048 vector result as a unsigned 2048-bit integer in storage.
 *  @param m1 vector representation of a unsigned 1024-bit integer.
 *  @param m2 vector representation of a unsigned 1024-bit integer.
 */
extern void
vec_mul1024x1024 (__VEC_U_2048 *p2048, __VEC_U_1024 *m1, __VEC_U_1024 *m2);

/** \brief Vector 2048x2048-bit Unsigned Integer Multiply.
 *
 *  Compute the 4096 bit product of 2048 bit values m1 and m2.
 *  The product is returned as single 4096-bit integer in a
 *  homogeneous aggregate structure.
 *
 *  \note This is the dynamic call ABI for IFUNC selection.
 *  The static implementations are vec_mul2048x2048_PWR8 and
 *  vec_mul2048x2048_PWR9. For static calls the __VEC_PWR_IMP() macro
 *  will add appropriate suffix based on the compile -mcpu= option.
 *
 *  |processor|Latency|Throughput|
 *  |--------:|:-----:|:---------|
 *  |power8   | ?? | 1/cycle  |
 *  |power9   | ?? | 1/cycle  |
 *
 *  @param p4096 vector result as a unsigned 4096-bit integer in storage.
 *  @param m1 vector representation of a unsigned 2048-bit integer.
 *  @param m2 vector representation of a unsigned 2048-bit integer.
 */
extern void
vec_mul2048x2048 (__VEC_U_4096 *p4096,
                  __VEC_U_2048 *m1, __VEC_U_2048 *m2);

///@cond INTERNAL
/* Doxygen can not handle macros or attributes */
extern __VEC_U_256
__VEC_PWR_IMP (vec_mul128x128) (vui128_t m1l, vui128_t m2l);

extern __VEC_U_512
__VEC_PWR_IMP (vec_mul256x256) (__VEC_U_256 m1, __VEC_U_256 m2);

extern __VEC_U_640
__VEC_PWR_IMP (vec_mul512x128) (__VEC_U_512 m1, vui128_t m2);

extern __VEC_U_1024
__VEC_PWR_IMP (vec_mul512x512) (__VEC_U_512 m1, __VEC_U_512 m2);

extern void
__VEC_PWR_IMP (vec_mul1024x1024) (__VEC_U_2048 *r2048,
                                  __VEC_U_1024 *m1_1024, __VEC_U_1024 *m2_1024);

extern void
__VEC_PWR_IMP (vec_mul2048x2048) (__VEC_U_4096 *r4096,
                                  __VEC_U_2048 *m1_2048, __VEC_U_2048 *m2_2048);
///@endcond

#endif /* SRC_PVECLIB_VEC_INT512_PPC_H_ */
