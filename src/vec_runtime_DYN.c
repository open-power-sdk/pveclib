 /*
 Copyright (c) [2019] Steven Munroe.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 vec_runtime_DYN.c

 Contributors:
      Steven Munroe
      Created on: Sep 17, 2019
 */

/*!
 * \file  vec_runtime_DYN.c
 * \brief Source file for generating the dynamic IFUNC resolves for
 * Power Vector Library.
 *
 * The working assumption is that the base and highest supported
 * platform (-mcpu=) for Big/Little Endian can and should be different.
 *
 * The base platform for Big Endian is POWER7 as the first
 * implementation of Vector Scalar Extended (VXS). The highest
 * supported platform is POWER8.
 * Distros supporting Big Endian are older and usually restricted to
 * POWER8 Compatibility mode when running on POWER9 hardware.
 *
 * The base platform for Little Endian is POWER8 as the first
 * officially supported LE platform for POWER. The highest supported
 * platform is is currently POWER10.  All the latest Distros are
 * Little Endian.
 *
 * We depend on the compiler and runtime (GLIBC 2.23) to support
 * __builtin_cpu_is() in the resolver function.
 * GCC support for __builtin_cpu_is() on PPC started
 * with version 6. You need both (GCC 6 or later and GLIBC 2.23 or
 * later).  We check that the compiler defines the macro
 * __BUILTIN_CPU_SUPPORTS__ to verify support is available.
 *
 * If __BUILTIN_CPU_SUPPORTS__ is not defined we default to the
 * appropriate (for the platform endian) base platform.
 */

#include <pveclib/vec_int512_ppc.h>
#include <pveclib/vec_f128_ppc.h>
#if 1
/*! \brief Macro to expand the parameterize resolver.
 * \sa \ref main_libary_issues_0_0_2
 * \sa \ref main_libary_issues_0_0 */
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#ifdef __BUILTIN_CPU_SUPPORTS__
#ifdef PVECLIB_DISABLE_POWER9
#define VEC_DYN_RESOLVER(FNAME) \
  if (__builtin_cpu_is ("power8")) \
    return FNAME ## _PWR8; \
  else \
    return FNAME ## _PWR7;
#else
#define VEC_DYN_RESOLVER(FNAME) \
  if (__builtin_cpu_is ("power9")) \
    return FNAME ## _PWR9; \
  else if (__builtin_cpu_is ("power8")) \
    return FNAME ## _PWR8; \
  else \
    return FNAME ## _PWR7;
#endif
#else
#define VEC_DYN_RESOLVER(__FNAME) \
   return __FNAME ## _PWR7;
#endif
#else // __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#ifdef  __BUILTIN_CPU_SUPPORTS__
#ifdef PVECLIB_DISABLE_POWER10
#define VEC_DYN_RESOLVER(FNAME) \
  if (__builtin_cpu_is ("power9")) \
    return FNAME ## _PWR9; \
  else \
    return FNAME ## _PWR8;
#else
#define VEC_DYN_RESOLVER(FNAME) \
  if (__builtin_cpu_is ("power10")) \
    return FNAME ## _PWR10; \
  else if (__builtin_cpu_is ("power9")) \
    return FNAME ## _PWR9; \
  else \
    return FNAME ## _PWR8;
#endif
#else
#define VEC_DYN_RESOLVER(FNAME) \
   return FNAME ## _PWR8;
#endif
#endif
#endif


/*! \brief Macros paste resolver_ prefix on function names. */
#define RESPASTE(_XF) resolve_ ## _XF

/*! \brief Macros to generate the static resolver function and
 * ifunc alias. These have to be defined in matched pairs linked by a
 * common <B>resolver name</B>.
 * We use multiple macros to address the varying number of function
 * parameters.
 * \sa \ref main_libary_issues_0_0_2
 * \sa \ref main_libary_issues_0_0 */
#define VEC_RESOLVER_2(_VT,_FUNC,_VA,_VB) \
static _VT \
(* RESPASTE(_FUNC) (void))(_VA, _VB) \
{ \
  VEC_DYN_RESOLVER(_FUNC); \
} \
_VT _FUNC (_VA, _VB) \
__attribute__ ((ifunc ("resolve_" #_FUNC )));

#define VEC_RESOLVER_3(_VT,_FUNC,_VA,_VB,_VC) \
static _VT \
(* RESPASTE(_FUNC) (void))(_VA, _VB, _VC) \
{ \
  VEC_DYN_RESOLVER(_FUNC); \
} \
_VT _FUNC (_VA, _VB, _VC) \
__attribute__ ((ifunc ("resolve_" #_FUNC )));

/*! \brief Macros listing externs for CPU specific runtime library
 * functions.
 * These will be expanded multiple time, once for each supported
 * -mcpu target.*/
#define VEC_F128_LIB_LIST(_TARGET) \
extern __binary128 vec_xsaddqpo ## _TARGET (__binary128, __binary128); \
extern __binary128 vec_xssubqpo ## _TARGET (__binary128, __binary128); \
extern __binary128 vec_xsmulqpo ## _TARGET (__binary128, __binary128); \
extern __binary128 vec_xsmaddqpo ## _TARGET (__binary128, __binary128, __binary128); \
extern __binary128 vec_xsmsubqpo ## _TARGET (__binary128, __binary128, __binary128);

#define VEC_INT512_LIB_LIST(_TARGET) \
extern __VEC_U_256 vec_mul128x128 ## _TARGET (vui128_t, vui128_t); \
extern __VEC_U_512 vec_mul256x256 ## _TARGET (__VEC_U_256, __VEC_U_256); \
extern __VEC_U_640 vec_mul512x128 ## _TARGET (__VEC_U_512, vui128_t); \
extern __VEC_U_640 vec_madd512x128a512 ## _TARGET (__VEC_U_512, vui128_t, __VEC_U_512); \
extern __VEC_U_1024 vec_mul512x512 ## _TARGET (__VEC_U_512, __VEC_U_512); \
extern void vec_mul1024x1024 ## _TARGET (__VEC_U_2048 *, __VEC_U_1024 *, __VEC_U_1024 *); \
extern void vec_mul2048x2048 ## _TARGET (__VEC_U_4096 *, __VEC_U_2048 *, __VEC_U_2048 *); \
extern void vec_mul128_byMN ## _TARGET (vui128_t *p, \
		  vui128_t *m1, vui128_t *m2, \
		  unsigned long M, unsigned long N); \
extern void vec_mul512_byMN ## _TARGET (__VEC_U_512 *p, \
                  __VEC_U_512 *m1, __VEC_U_512 *m2, \
		  unsigned long M, unsigned long N);

#ifndef PVECLIB_DISABLE_POWER7
// POWER7 supports only BIG Endian. So declare PWR7 externs only for BE.
VEC_INT512_LIB_LIST (_PWR7)

VEC_F128_LIB_LIST (_PWR7)
#endif

// POWER8 supports both Endians. So declare PWR8 externs unconditionally.
VEC_INT512_LIB_LIST (_PWR8)

VEC_F128_LIB_LIST (_PWR8)

#ifndef PVECLIB_DISABLE_POWER9
/* Older distros running Big Endian are unlikely to support PWR9.
 * So declare PWR9 externs only for LE.  */

VEC_INT512_LIB_LIST (_PWR9)

VEC_F128_LIB_LIST (_PWR9)
#endif

#ifndef PVECLIB_DISABLE_POWER10
/* Older distros running Big Endian are unlikely to support PWR10.
 * So declare PWR9 externs only for LE.  */

VEC_INT512_LIB_LIST (_PWR10)

VEC_F128_LIB_LIST (_PWR10)
#endif

/* Declare the required static resolvers and ifunc aliases.
 * For example:
 *
 * \code
static
__VEC_U_256
(*resolve_vec_mul128x128 (void))(vui128_t, vui128_t)
{
  VEC_DYN_RESOLVER(vec_mul128x128);
}

__VEC_U_256
vec_mul128x128 (vui128_t, vui128_t)
__attribute__ ((ifunc ("resolve_vec_mul128x128")));
 * \endcode
 *
 * Here we can use the macros VEC_RESOLVER_2() and VEC_RESOLVER_3()
 * */

/* Declare the required static resolvers and ifunc aliases for dynamic
 * selection of CPU specific implementations supporting
 * vec_int512_ppc.h
 * */
VEC_RESOLVER_2 (__VEC_U_256, vec_mul128x128, vui128_t, vui128_t);

VEC_RESOLVER_2 (__VEC_U_512, vec_mul256x256, __VEC_U_256, __VEC_U_256);

VEC_RESOLVER_2 (__VEC_U_640, vec_mul512x128, __VEC_U_512, vui128_t);

VEC_RESOLVER_3 (__VEC_U_640, vec_madd512x128a512, __VEC_U_512, vui128_t, __VEC_U_512);

VEC_RESOLVER_2 (__VEC_U_1024, vec_mul512x512, __VEC_U_512, __VEC_U_512);

VEC_RESOLVER_3 (void, vec_mul1024x1024, __VEC_U_2048*, __VEC_U_1024*, __VEC_U_1024*);

VEC_RESOLVER_3 (void, vec_mul2048x2048, __VEC_U_4096*, __VEC_U_2048*, __VEC_U_2048*);

static
void
(*resolve_vec_mul128_byMN (void))
(vui128_t *p, vui128_t *m1, vui128_t *m2,
	  unsigned long M, unsigned long N)
{
  VEC_DYN_RESOLVER(vec_mul128_byMN);
}

void
vec_mul128_byMN (vui128_t *p, vui128_t *m1, vui128_t *m2,
		  unsigned long M, unsigned long N)
__attribute__ ((ifunc ("resolve_vec_mul128_byMN")));

static
void
(*resolve_vec_mul512_byMN (void))
(__VEC_U_512 *p, __VEC_U_512 *m1, __VEC_U_512 *m2,
	  unsigned long M, unsigned long N)
{
  VEC_DYN_RESOLVER(vec_mul512_byMN);
}

void
vec_mul512_byMN (__VEC_U_512 *p, __VEC_U_512 *m1, __VEC_U_512 *m2,
		  unsigned long M, unsigned long N)
__attribute__ ((ifunc ("resolve_vec_mul512_byMN")));

/* Declare the required static resolvers and ifunc aliases for dynamic
 * selection of CPU specific implementations supporting
 * vec_f128_ppc.h
 * */
VEC_RESOLVER_2 (__binary128, vec_xsaddqpo, __binary128, __binary128);
VEC_RESOLVER_2 (__binary128, vec_xsmulqpo, __binary128, __binary128);
VEC_RESOLVER_2 (__binary128, vec_xssubqpo, __binary128, __binary128);
VEC_RESOLVER_3 (__binary128, vec_xsmaddqpo, __binary128, __binary128, __binary128);
VEC_RESOLVER_3 (__binary128, vec_xsmsubqpo, __binary128, __binary128, __binary128);
