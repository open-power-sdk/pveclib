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

/** \mainpage POWER Vector Library
* \brief library of useful vector functions for POWER. This library
* fills in the gap between the PowerPC vector intrinsic functions
* (as implemented by altivec.h) and major libraries like ESSL and MASSV.
*
*  \authors Steven Munroe
*
*  \section Rationale
*
*  Higher level vector intrinsic operations are needed.
*  For example the PowerISA provides population count and count
*  leading zero operations on vectors of doubleword but not on the
*  whole vector as a __int128 value.
*  Another example the ISA 2.07 provides vector multiply on even/odd
*  unsigned int values but does not directly provide a full __int128
*  by __int128 multiply producing a 256-bit product.
*  Also some useful operations can be constructed from existing PowerISA
*  instructions and GCC Altivec Intrinsic built-ins but the
*  implementation may not be obvious. And finally the optimum sequence will
*  vary across the PowerISA levels as new instructions are added.
*
*  So the goal of this project to provide well crafted
*  implementations of useful vector and large number operations.
*
*  - Provide equivalent functions across versions of the PowerISA.
*  For example the Vector Multiply-by-10 Unsigned Quadword operations
*  introduced in PowerISA 3.0 (POWER9) can be implement in a few vector
*  instructions on earlier PowerISA versions.
*  - Provide equivalent functions across versions of the compiler.
*  For example builtins provided in later versions of the compiler
*  can be implemented as inline functions with inline asm in earlier
*  compiler versions.
*  - Provide higher order functions not provided directly by the PowerISA.
*  For example vector SIMI implementation for ASCII __isalpha, etc.
*  Another example full __int128 implementations of Count Leading Zeros,
*  Population Count, and Multiply.
*
*  \subsection POWER Vector Library Intrinsic headerss
*  - vec_common_ppc.h Typedefs and helper macros
*  - vec_int128_ppc.h Operations on vector __int128 values
*
**/

#endif /* __PVECLIB_MAIN_DOX_H  */
