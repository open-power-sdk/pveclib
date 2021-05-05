/*
 Copyright (c) [2016, 2018] IBM Corporation.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 arith128_test_f128.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Apr 18, 2016
 */

#define __STDC_WANT_DEC_FP__    1

#include <stdint.h>
#include <stdio.h>
#include <float.h>

#include "arith128.h"
#include <testsuite/arith128_print.h>
#include <pveclib/vec_int128_ppc.h>
#include <pveclib/vec_f128_ppc.h>
#include <testsuite/arith128_test_f128.h>

#undef __DEBUG_PRINT__
//#define __DEBUG_PRINT__

#ifdef __DEBUG_PRINT__

static vb128_t
db_vec_isfinitef128 (__binary128 f128)
{
	const vui32_t expmask  = CONST_VINT128_W(0x7fff0000, 0, 0, 0);
	vui32_t tmp, t128;
	vb128_t tmp2, tmp3;
	vb128_t result = (vb128_t)CONST_VINT128_W(0, 0, 0, 0);

	t128 = vec_xfer_bin128_2_vui32t (f128);
	tmp = vec_and (t128, expmask);
	tmp2 = vec_cmpeq(tmp, expmask);
	tmp3 = vec_splat (tmp2,VEC_W_H);
	result = vec_nor (tmp3, tmp3); // vec_not

	print_vfloat128x ("db_vec_isfinitef128", f128);
	print_vint128x   ("               t128:", (vui128_t)t128);
	print_vint128x   ("                tmp:", (vui128_t)tmp);
	print_vint128x   ("               tmp2:", (vui128_t)tmp2);
	print_vint128x   ("               tmp3:", (vui128_t)tmp3);
	print_vint128x   ("             result:", (vui128_t)result);

	return (result);
}
#endif

const vui32_t signmask32  = {0x80000000, 0, 0, 0};
const vui64_t signmask64  = {0x8000000000000000, 0};

const vui64_t vf128_zero  = CONST_VINT64_DW(0x0000000000000000, 0);
const vui64_t vf128_nzero = CONST_VINT64_DW(0x8000000000000000, 0);

const vui64_t vf128_one   = CONST_VINT64_DW(0x3fff000000000000, 0);
const vui64_t vf128_none  = CONST_VINT64_DW(0xbfff000000000000, 0);

const vui64_t vf128_max   = CONST_VINT64_DW(0x7ffeffffffffffff, 0xffffffffffffffff);
const vui64_t vf128_nmax  = CONST_VINT64_DW(0xfffeffffffffffff, 0xffffffffffffffff);

const vui64_t vf128_min   = CONST_VINT64_DW(0x0001000000000000, 0x0000000000000000);
const vui64_t vf128_nmin  = CONST_VINT64_DW(0x8001000000000000, 0x0000000000000000);

const vui64_t vf128_sub   = CONST_VINT64_DW(0x0000ffffffffffff, 0xffffffffffffffff);
const vui64_t vf128_nsub  = CONST_VINT64_DW(0x8000ffffffffffff, 0xffffffffffffffff);

const vui64_t vf128_inf   = CONST_VINT64_DW(0x7fff000000000000, 0);
const vui64_t vf128_ninf  = CONST_VINT64_DW(0xffff000000000000, 0);

const vui64_t vf128_nan   = CONST_VINT64_DW(0x7fff800000000000, 0);
const vui64_t vf128_nnan  = CONST_VINT64_DW(0xffff800000000000, 0);

const vui64_t vf128_snan  = CONST_VINT64_DW(0x7fff400000000000, 0);
const vui64_t vf128_nsnan = CONST_VINT64_DW(0xffff000080000000, 0);

const vui32_t vf128_true = CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
const vui32_t vf128_false = CONST_VINT32_W(0, 0, 0, 0);

int
test_isinf_signf128 (void)
{
  const __binary128 f128_zero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000000000000000, 0));
  const __binary128 f128_nzero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000000000000000, 0));

  const __binary128 f128_one = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x3fff000000000000, 0));
  const __binary128 f128_none = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xbfff000000000000, 0));

  const __binary128 f128_max = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7ffeffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nmax = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xfffeffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_min = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0001000000000000, 0x0000000000000000));
  const __binary128 f128_nmin = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8001000000000000, 0x0000000000000000));

  const __binary128 f128_sub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000ffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nsub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000ffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_inf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff000000000000, 0));
  const __binary128 f128_ninf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000000000000, 0));

  const __binary128 f128_nan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff800000000000, 0));
  const __binary128 f128_nnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff800000000000, 0));

  const __binary128 f128_snan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff400000000000, 0));
  const __binary128 f128_nsnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000080000000, 0));

  __binary128 x;
  long tests_count = 0;
  int test, expt;
  int rc = 0;

  printf ("\ntest_isinf_signf128 f128 -> int , ...\n");

#if 1
  tests_count++;
  x = (__binary128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = -1;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
  /* accumulate the number of values tested, in case we are doing
   * detail timing and want to compute function averages.  */
  tcount += tests_count;
  printf ("\ntest_isinf_signf128, tests=%ld fails=%d\n", tests_count, rc);

  return (rc);
}

int
test_setb_qp (void)
{
  const __binary128 f128_zero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000000000000000, 0));
  const __binary128 f128_nzero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000000000000000, 0));

  const __binary128 f128_one = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x3fff000000000000, 0));
  const __binary128 f128_none = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xbfff000000000000, 0));

  const __binary128 f128_max = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7ffeffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nmax = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xfffeffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_min = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0001000000000000, 0x0000000000000000));
  const __binary128 f128_nmin = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8001000000000000, 0x0000000000000000));

  const __binary128 f128_sub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000ffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nsub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000ffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_inf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff000000000000, 0));
  const __binary128 f128_ninf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000000000000, 0));

  const __binary128 f128_nan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff800000000000, 0));
  const __binary128 f128_nnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff800000000000, 0));

  const __binary128 f128_snan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff400000000000, 0));
  const __binary128 f128_nsnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000080000000, 0));

  __binary128 x;
  vb128_t test, expt;
  long tests_count = 0;
  int rc = 0;

  printf ("\ntest_setb_qp f128 -> vector bool , ...\n");

#if 1
  tests_count++;
  x = (__binary128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif

  /* accumulate the number of values tested, in case we are doing
   * detail timing and want to compute function averages.  */
  tcount += tests_count;
  printf ("\ntest_setb_qp, tests=%ld fails=%d\n", tests_count, rc);

  return (rc);
}

int
test_signbitf128 (void)
{
  const __binary128 f128_zero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000000000000000, 0));
  const __binary128 f128_nzero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000000000000000, 0));

  const __binary128 f128_one = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x3fff000000000000, 0));
  const __binary128 f128_none = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xbfff000000000000, 0));

  const __binary128 f128_max = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7ffeffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nmax = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xfffeffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_min = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0001000000000000, 0x0000000000000000));
  const __binary128 f128_nmin = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8001000000000000, 0x0000000000000000));

  const __binary128 f128_sub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000ffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nsub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000ffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_inf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff000000000000, 0));
  const __binary128 f128_ninf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000000000000, 0));

  const __binary128 f128_nan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff800000000000, 0));
  const __binary128 f128_nnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff800000000000, 0));

  const __binary128 f128_snan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff400000000000, 0));
  const __binary128 f128_nsnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000080000000, 0));

  __binary128 x;
  long tests_count = 0;
  int test, expt;
  int rc = 0;

#ifdef __DEBUG_PRINT__
  x = (__binary128)f128_zero;
  print_vfloat128x(" x=  ", x);
  x = (__binary128)f128_nzero;
  print_vfloat128x(" x=  ", x);
  x = (__binary128)f128_one;
  print_vfloat128x(" x=  ", x);
  x = (__binary128)f128_none;
  print_vfloat128x(" x=  ", x);
  x = (__binary128)f128_max;
  print_vfloat128x(" x=  ", x);
  x = (__binary128)f128_nmax;
  print_vfloat128x(" x=  ", x);
  x = (__binary128)f128_min;
  print_vfloat128x(" x=  ", x);
  x = (__binary128)f128_nmin;
  print_vfloat128x(" x=  ", x);
  x = (__binary128)f128_sub;
  print_vfloat128x(" x=  ", x);
  x = (__binary128)f128_nsub;
  print_vfloat128x(" x=  ", x);
  x = (__binary128)f128_inf;
  print_vfloat128x(" x=  ", x);
  x = (__binary128)f128_ninf;
  print_vfloat128x(" x=  ", x);
  x = (__binary128)f128_nan;
  print_vfloat128x(" x=  ", x);
  x = (__binary128)f128_nnan;
  print_vfloat128x(" x=  ", x);
  x = (__binary128)f128_snan;
  print_vfloat128x(" x=  ", x);
  x = (__binary128)f128_nsnan;
  print_vfloat128x(" x=  ", x);
#endif

  printf ("\ntest_signbitf128 f128 -> int bool , ...\n");

#if 1
  tests_count++;
  x = (__binary128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
  /* accumulate the number of values tested, in case we are doing
   * detail timing and want to compute function averages.  */
  tcount += tests_count;
  printf ("\ntest_signbitf128, tests=%ld fails=%d\n", tests_count, rc);
  return (rc);
}

int
test_isinff128 (void)
{
  const __binary128 f128_zero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000000000000000, 0));
  const __binary128 f128_nzero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000000000000000, 0));

  const __binary128 f128_one = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x3fff000000000000, 0));
  const __binary128 f128_none = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xbfff000000000000, 0));

  const __binary128 f128_max = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7ffeffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nmax = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xfffeffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_min = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0001000000000000, 0x0000000000000000));
  const __binary128 f128_nmin = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8001000000000000, 0x0000000000000000));

  const __binary128 f128_sub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000ffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nsub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000ffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_inf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff000000000000, 0));
  const __binary128 f128_ninf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000000000000, 0));

  const __binary128 f128_nan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff800000000000, 0));
  const __binary128 f128_nnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff800000000000, 0));

  const __binary128 f128_snan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff400000000000, 0));
  const __binary128 f128_nsnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000080000000, 0));

  __binary128 x;
  vb128_t test, expt;
  long tests_count = 0;
  int rc = 0;

  printf ("\ntest_isinff128 f128 -> vector bool ...\n");

#if 1
  tests_count++;
  x = (__binary128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif

  /* accumulate the number of values tested, in case we are doing
   * detail timing and want to compute function averages.  */
  tcount += tests_count;
  printf ("\ntest_isinff128, tests=%ld fails=%d\n", tests_count, rc);
  return (rc);
}

int
test_isnanf128 (void)
{
  const __binary128 f128_zero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000000000000000, 0));
  const __binary128 f128_nzero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000000000000000, 0));

  const __binary128 f128_one = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x3fff000000000000, 0));
  const __binary128 f128_none = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xbfff000000000000, 0));

  const __binary128 f128_max = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7ffeffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nmax = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xfffeffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_min = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0001000000000000, 0x0000000000000000));
  const __binary128 f128_nmin = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8001000000000000, 0x0000000000000000));

  const __binary128 f128_sub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000ffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nsub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000ffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_inf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff000000000000, 0));
  const __binary128 f128_ninf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000000000000, 0));

  const __binary128 f128_nan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff800000000000, 0));
  const __binary128 f128_nnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff800000000000, 0));

  const __binary128 f128_snan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff400000000000, 0));
  const __binary128 f128_nsnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000080000000, 0));

  __binary128 x;
  vb128_t test, expt;
  long tests_count = 0;
  int rc = 0;

  printf ("\ntest_isnanf128 f128 -> vector bool ...\n");

#if 1
  tests_count++;
  x = (__binary128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif

  /* accumulate the number of values tested, in case we are doing
   * detail timing and want to compute function averages.  */
  tcount += tests_count;
  printf ("\ntest_isnanf128, tests=%ld fails=%d\n", tests_count, rc);
  return (rc);
}

int
test_isfinitef128 (void)
{
  const __binary128 f128_zero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000000000000000, 0));
  const __binary128 f128_nzero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000000000000000, 0));

  const __binary128 f128_one = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x3fff000000000000, 0));
  const __binary128 f128_none = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xbfff000000000000, 0));

  const __binary128 f128_max = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7ffeffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nmax = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xfffeffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_min = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0001000000000000, 0x0000000000000000));
  const __binary128 f128_nmin = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8001000000000000, 0x0000000000000000));

  const __binary128 f128_sub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000ffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nsub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000ffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_inf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff000000000000, 0));
  const __binary128 f128_ninf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000000000000, 0));

  const __binary128 f128_nan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff800000000000, 0));
  const __binary128 f128_nnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff800000000000, 0));

  const __binary128 f128_snan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff400000000000, 0));
  const __binary128 f128_nsnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000080000000, 0));

  __binary128 x;
  vb128_t test, expt;
  long tests_count = 0;
  int rc = 0;

  printf ("\ntest_isfinitef128 f128 -> vector bool ...\n");

#if 1
  tests_count++;
  x = (__binary128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif

  /* accumulate the number of values tested, in case we are doing
   * detail timing and want to compute function averages.  */
  tcount += tests_count;
  printf ("\ntest_isfinitef128, tests=%ld fails=%d\n", tests_count, rc);
  return (rc);
}

int
test_isnormalf128 (void)
{
  const __binary128 f128_zero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000000000000000, 0));
  const __binary128 f128_nzero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000000000000000, 0));

  const __binary128 f128_one = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x3fff000000000000, 0));
  const __binary128 f128_none = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xbfff000000000000, 0));

  const __binary128 f128_max = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7ffeffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nmax = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xfffeffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_min = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0001000000000000, 0x0000000000000000));
  const __binary128 f128_nmin = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8001000000000000, 0x0000000000000000));

  const __binary128 f128_sub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000ffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nsub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000ffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_inf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff000000000000, 0));
  const __binary128 f128_ninf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000000000000, 0));

  const __binary128 f128_nan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff800000000000, 0));
  const __binary128 f128_nnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff800000000000, 0));

  const __binary128 f128_snan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff400000000000, 0));
  const __binary128 f128_nsnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000080000000, 0));

  __binary128 x;
  vb128_t test, expt;
  long tests_count = 0;
  int rc = 0;

  printf ("\ntest_isnormalf128 f128 -> vector bool ...\n");

#if 1
  tests_count++;
  x = (__binary128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
  /* accumulate the number of values tested, in case we are doing
   * detail timing and want to compute function averages.  */
  tcount += tests_count;
  printf ("\ntest_isnormalf128, tests=%ld fails=%d\n", tests_count, rc);
  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_issubnormalf128 (void)
{
  const __binary128 f128_zero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000000000000000, 0));
  const __binary128 f128_nzero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000000000000000, 0));

  const __binary128 f128_one = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x3fff000000000000, 0));
  const __binary128 f128_none = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xbfff000000000000, 0));

  const __binary128 f128_max = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7ffeffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nmax = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xfffeffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_min = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0001000000000000, 0x0000000000000000));
  const __binary128 f128_nmin = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8001000000000000, 0x0000000000000000));

  const __binary128 f128_sub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000ffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nsub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000ffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_sub2 = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000000000000000, 0x0000000000000001));
  const __binary128 f128_nsub2 = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000000000000000, 0x0000000000000001));

  const __binary128 f128_inf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff000000000000, 0));
  const __binary128 f128_ninf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000000000000, 0));

  const __binary128 f128_nan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff800000000000, 0));
  const __binary128 f128_nnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff800000000000, 0));

  const __binary128 f128_snan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff400000000000, 0));
  const __binary128 f128_nsnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000080000000, 0));

  __binary128 x;
  vb128_t test, expt;
  long tests_count = 0;
  int rc = 0;

  printf ("\ntest_issubnormalf128 f128 -> vector bool ...\n");

#if 1
  tests_count++;
  x = (__binary128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_sub2;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsub2;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
  /* accumulate the number of values tested, in case we are doing
   * detail timing and want to compute function averages.  */
  tcount += tests_count;
  printf ("\ntest_issubnormalf128, tests=%ld fails=%d\n", tests_count, rc);
  return (rc);
}
#undef __DEBUG_PRINT__

//#define __DEBUG_PRINT__ 1
int
test_iszerof128 (void)
{
  const __binary128 f128_zero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000000000000000, 0));
  const __binary128 f128_nzero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000000000000000, 0));

  const __binary128 f128_one = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x3fff000000000000, 0));
  const __binary128 f128_none = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xbfff000000000000, 0));

  const __binary128 f128_max = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7ffeffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nmax = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xfffeffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_min = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0001000000000000, 0x0000000000000000));
  const __binary128 f128_nmin = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8001000000000000, 0x0000000000000000));

  const __binary128 f128_sub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000ffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nsub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000ffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_sub2 = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000000000000000, 0x0000000000000001));
  const __binary128 f128_nsub2 = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000000000000000, 0x0000000000000001));

  const __binary128 f128_inf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff000000000000, 0));
  const __binary128 f128_ninf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000000000000, 0));

  const __binary128 f128_nan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff800000000000, 0));
  const __binary128 f128_nnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff800000000000, 0));

  const __binary128 f128_snan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff400000000000, 0));
  const __binary128 f128_nsnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000080000000, 0));

  __binary128 x;
  vb128_t test, expt;
  long tests_count = 0;
  int rc = 0;

  printf ("\ntest_iszerof128 f128 -> vector bool ...\n");

#if 1
  tests_count++;
  x = (__binary128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_sub2;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsub2;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
  /* accumulate the number of values tested, in case we are doing
   * detail timing and want to compute function averages.  */
  tcount += tests_count;
  printf ("\ntest_iszerof128, tests=%ld fails=%d\n", tests_count, rc);
  return (rc);
}
#undef __DEBUG_PRINT__

//#define __DEBUG_PRINT__ 1
int
test_absf128 (void)
{
  const __binary128 f128_zero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000000000000000, 0));
  const __binary128 f128_nzero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000000000000000, 0));

  const __binary128 f128_one = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x3fff000000000000, 0));
  const __binary128 f128_none = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xbfff000000000000, 0));

  const __binary128 f128_max = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7ffeffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nmax = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xfffeffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_min = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0001000000000000, 0x0000000000000000));
  const __binary128 f128_nmin = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8001000000000000, 0x0000000000000000));

  const __binary128 f128_sub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000ffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nsub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000ffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_inf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff000000000000, 0));
  const __binary128 f128_ninf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000000000000, 0));

  const __binary128 f128_nan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff800000000000, 0));
  const __binary128 f128_nnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff800000000000, 0));

  const __binary128 f128_snan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff400000000000, 0));
  const __binary128 f128_nsnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff400000000000, 0));


  __binary128 x;
  __binary128 t, e;
  long tests_count = 0;
  int rc = 0;

  printf ("\ntest_absf128 f128 -> f128 ...\n");

#if 1
  tests_count++;
  x = (__binary128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = (__binary128) f128_zero;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = (__binary128) f128_zero;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = f128_one;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = f128_one;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = f128_max;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = f128_max;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = f128_min;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = f128_min;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = f128_sub;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = f128_sub;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = f128_inf;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = f128_inf;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = f128_nan;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = f128_nan;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = f128_snan;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = f128_snan;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
  /* accumulate the number of values tested, in case we are doing
   * detail timing and want to compute function averages.  */
  tcount += tests_count;
  printf ("\ntest_absf128, tests=%ld fails=%d\n", tests_count, rc);
  return (rc);
}
#undef __DEBUG_PRINT__

//#define __DEBUG_PRINT__ 1
int
test_copysignf128 (void)
{
  const __binary128 f128_zero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000000000000000, 0));
  const __binary128 f128_nzero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000000000000000, 0));

  const __binary128 f128_one = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x3fff000000000000, 0));
  const __binary128 f128_none = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xbfff000000000000, 0));

  const __binary128 f128_max = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7ffeffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nmax = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xfffeffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_min = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0001000000000000, 0x0000000000000000));
  const __binary128 f128_nmin = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8001000000000000, 0x0000000000000000));

  const __binary128 f128_sub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000ffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nsub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000ffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_inf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff000000000000, 0));
  const __binary128 f128_ninf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000000000000, 0));

  const __binary128 f128_nan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff800000000000, 0));
  const __binary128 f128_nnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff800000000000, 0));

  const __binary128 f128_snan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff400000000000, 0));

  __binary128 x, y, t, e;
  long tests_count = 0;
  int rc = 0;

  printf ("\ntest_copysignf128 f128 -> f128 ...\n");

#if 1
  tests_count++;
  x = (__binary128) f128_zero;
  y = (__binary128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = vec_copysignf128 (x, y);
  e = (__binary128) f128_zero;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = (__binary128) f128_zero;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_one;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_one;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_max;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_max;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_min;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_min;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_sub;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_sub;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_inf;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_inf;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_nan;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_nan;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_snan;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif

#if 1
  tests_count++;
  x = (__binary128) f128_zero;
  y = (__binary128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = vec_copysignf128 (x, y);
  e = (__binary128) f128_nzero;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = (__binary128) f128_nzero;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_none;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_none;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_nmax;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_nmax;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_nmin;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_nmin;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_nsub;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_nsub;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_ninf;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_ninf;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_nnan;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_nnan;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
  /* accumulate the number of values tested, in case we are doing
   * detail timing and want to compute function averages.  */
  tcount += tests_count;
  printf ("\ntest_copysignf128, tests=%ld fails=%d\n", tests_count, rc);
  return (rc);
}
#undef __DEBUG_PRINT__

//#define __DEBUG_PRINT__ 1
int
test_const_f128 (void)
{
  const __binary128 f128_inf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff000000000000, 0));

  const __binary128 f128_nan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff800000000000, 0));

  const __binary128 f128_snan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff400000000000, 0));

  __binary128 x, t, e;
  long tests_count = 0;
  int rc = 0;
#if 1
  tests_count++;
  x = vec_const_huge_valf128 ();
#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
#endif
  t = x;
  e = f128_inf;
  rc += check_f128 ("check __huge_valf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = vec_const_inff128 ();
#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
#endif
  t = x;
  e = f128_inf;
  rc += check_f128 ("check inff128", x, t, e);
#endif
#if 1
  tests_count++;
  x = vec_const_nanf128 ();
#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
#endif
  t = x;
  e = f128_nan;
  rc += check_f128 ("check inff128", x, t, e);
#endif
#if 1
  tests_count++;
  x = vec_const_nansf128 ();
#ifdef __DEBUG_PRINT__
  print_vfloat128x (" x=  ", x);
#endif
  t = x;
  e = f128_snan;
  rc += check_f128 ("check inff128", x, t, e);
#endif

  /* accumulate the number of values tested, in case we are doing
   * detail timing and want to compute function averages.  */
  tcount += tests_count;
  return (rc);
}
#undef __DEBUG_PRINT__

int
test_all_isfinitef128 (void)
{
  const __binary128 f128_zero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000000000000000, 0));
  const __binary128 f128_nzero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000000000000000, 0));

  const __binary128 f128_one = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x3fff000000000000, 0));
  const __binary128 f128_none = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xbfff000000000000, 0));

  const __binary128 f128_max = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7ffeffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nmax = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xfffeffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_min = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0001000000000000, 0x0000000000000000));
  const __binary128 f128_nmin = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8001000000000000, 0x0000000000000000));

  const __binary128 f128_sub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000ffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nsub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000ffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_inf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff000000000000, 0));
  const __binary128 f128_ninf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000000000000, 0));

  const __binary128 f128_nan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff800000000000, 0));
  const __binary128 f128_nnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff800000000000, 0));

  const __binary128 f128_snan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff400000000000, 0));
  const __binary128 f128_nsnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000080000000, 0));

  __binary128 x;
  long tests_count = 0;
  int test, expt;
  int rc = 0;

  printf ("\ntest_all_isfinitef128 f128 -> bool int , ...\n");

#if 1
  tests_count++;
  x = (__binary128)f128_zero;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 1;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__binary128)f128_nzero;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 1;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__binary128)f128_one;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 1;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__binary128)f128_none;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 1;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__binary128)f128_max;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 1;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__binary128)f128_nmax;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 1;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__binary128)f128_min;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 1;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__binary128)f128_nmin;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 1;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__binary128)f128_sub;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 1;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__binary128)f128_nsub;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 1;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__binary128)f128_inf;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 0;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__binary128)f128_ninf;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 0;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__binary128)f128_nan;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 0;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__binary128)f128_nnan;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 0;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__binary128)f128_snan;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 0;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__binary128)f128_nsnan;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 0;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif

  /* accumulate the number of values tested, in case we are doing
   * detail timing and want to compute function averages.  */
  tcount += tests_count;
  printf ("\ntest_all_isfinitef128, tests=%ld fails=%d\n", tests_count, rc);

  return (rc);
}

int
test_all_isnanf128 (void)
{
  const __binary128 f128_zero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000000000000000, 0));
  const __binary128 f128_nzero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000000000000000, 0));

  const __binary128 f128_one = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x3fff000000000000, 0));
  const __binary128 f128_none = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xbfff000000000000, 0));

  const __binary128 f128_max = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7ffeffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nmax = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xfffeffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_min = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0001000000000000, 0x0000000000000000));
  const __binary128 f128_nmin = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8001000000000000, 0x0000000000000000));

  const __binary128 f128_sub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000ffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nsub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000ffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_inf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff000000000000, 0));
  const __binary128 f128_ninf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000000000000, 0));

  const __binary128 f128_nan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff800000000000, 0));
  const __binary128 f128_nnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff800000000000, 0));

  const __binary128 f128_snan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff400000000000, 0));
  const __binary128 f128_nsnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000080000000, 0));

  __binary128 x;
  long tests_count = 0;
  int test, expt;
  int rc = 0;

  printf ("\ntest_all_isnanf128 f128 -> bool int , ...\n");

#if 1
  tests_count++;
  x = (__binary128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = vec_const_nanf128 ();
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = vec_const_nansf128 ();
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif

  /* accumulate the number of values tested, in case we are doing
   * detail timing and want to compute function averages.  */
  tcount += tests_count;
  printf ("\ntest_all_isnanf128, tests=%ld fails=%d\n", tests_count, rc);

  return (rc);
}

int
test_all_isinff128 (void)
{
  const __binary128 f128_zero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000000000000000, 0));
  const __binary128 f128_nzero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000000000000000, 0));

  const __binary128 f128_one = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x3fff000000000000, 0));
  const __binary128 f128_none = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xbfff000000000000, 0));

  const __binary128 f128_max = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7ffeffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nmax = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xfffeffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_min = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0001000000000000, 0x0000000000000000));
  const __binary128 f128_nmin = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8001000000000000, 0x0000000000000000));

  const __binary128 f128_sub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000ffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nsub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000ffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_inf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff000000000000, 0));
  const __binary128 f128_ninf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000000000000, 0));

  const __binary128 f128_nan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff800000000000, 0));
  const __binary128 f128_nnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff800000000000, 0));

  const __binary128 f128_snan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff400000000000, 0));
  const __binary128 f128_nsnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000080000000, 0));

  __binary128 x;
  long tests_count = 0;
  int test, expt;
  int rc = 0;

  printf ("\ntest_all_isinff128 f128 -> bool int , ...\n");

#if 1
  tests_count++;
  x = (__binary128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif

  /* accumulate the number of values tested, in case we are doing
   * detail timing and want to compute function averages.  */
  tcount += tests_count;
  printf ("\ntest_all_isinff128, tests=%ld fails=%d\n", tests_count, rc);

  return (rc);
}

int
test_all_isnormalf128 (void)
{
  const __binary128 f128_zero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000000000000000, 0));
  const __binary128 f128_nzero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000000000000000, 0));

  const __binary128 f128_one = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x3fff000000000000, 0));
  const __binary128 f128_none = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xbfff000000000000, 0));

  const __binary128 f128_max = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7ffeffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nmax = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xfffeffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_min = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0001000000000000, 0x0000000000000000));
  const __binary128 f128_nmin = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8001000000000000, 0x0000000000000000));

  const __binary128 f128_sub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000ffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nsub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000ffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_inf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff000000000000, 0));
  const __binary128 f128_ninf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000000000000, 0));

  const __binary128 f128_nan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff800000000000, 0));
  const __binary128 f128_nnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff800000000000, 0));

  const __binary128 f128_snan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff400000000000, 0));
  const __binary128 f128_nsnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000080000000, 0));

  __binary128 x;
  long tests_count = 0;
  int test, expt;
  int rc = 0;

  printf ("\ntest_all_isnormalf128 f128 -> bool int , ...\n");

#if 1
  tests_count++;
  x = (__binary128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif

  /* accumulate the number of values tested, in case we are doing
   * detail timing and want to compute function averages.  */
  tcount += tests_count;
  printf ("\ntest_all_isnormalf128, tests=%ld fails=%d\n", tests_count, rc);

  return (rc);
}

int
test_all_issubnormalf128 (void)
{
  const __binary128 f128_zero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000000000000000, 0));
  const __binary128 f128_nzero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000000000000000, 0));

  const __binary128 f128_one = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x3fff000000000000, 0));
  const __binary128 f128_none = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xbfff000000000000, 0));

  const __binary128 f128_max = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7ffeffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nmax = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xfffeffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_min = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0001000000000000, 0x0000000000000000));
  const __binary128 f128_nmin = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8001000000000000, 0x0000000000000000));

  const __binary128 f128_sub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000ffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nsub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000ffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_submin = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000000000000000, 0x0000000000000001));
  const __binary128 f128_nsubmin = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000000000000000, 0x0000000000000001));

  const __binary128 f128_inf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff000000000000, 0));
  const __binary128 f128_ninf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000000000000, 0));

  const __binary128 f128_nan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff800000000000, 0));
  const __binary128 f128_nnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff800000000000, 0));

  const __binary128 f128_snan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff400000000000, 0));
  const __binary128 f128_nsnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000080000000, 0));

  __binary128 x;
  long tests_count = 0;
  int test, expt;
  int rc = 0;
  tcount = 0;

  printf ("\ntest_all_issubnormalf128 f128 -> bool int , ...\n");

#if 1
  tests_count++;
  x = (__binary128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_submin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsubmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif

  /* accumulate the number of values tested, in case we are doing
   * detail timing and want to compute function averages.  */
  tcount += tests_count;
  printf ("\ntest_all_issubnormalf128, tests=%ld fails=%d\n", tests_count, rc);

  return (rc);
}

int
test_all_iszerof128 (void)
{
  const __binary128 f128_zero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000000000000000, 0));
  const __binary128 f128_nzero = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000000000000000, 0));

  const __binary128 f128_one = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x3fff000000000000, 0));
  const __binary128 f128_none = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xbfff000000000000, 0));

  const __binary128 f128_max = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7ffeffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nmax = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xfffeffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_min = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0001000000000000, 0x0000000000000000));
  const __binary128 f128_nmin = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8001000000000000, 0x0000000000000000));

  const __binary128 f128_sub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000ffffffffffff, 0xffffffffffffffff));
  const __binary128 f128_nsub = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000ffffffffffff, 0xffffffffffffffff));

  const __binary128 f128_submin = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x0000000000000000, 0x0000000000000001));
  const __binary128 f128_nsubmin = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x8000000000000000, 0x0000000000000001));

  const __binary128 f128_inf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff000000000000, 0));
  const __binary128 f128_ninf = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000000000000, 0));

  const __binary128 f128_nan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff800000000000, 0));
  const __binary128 f128_nnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff800000000000, 0));

  const __binary128 f128_snan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0x7fff400000000000, 0));
  const __binary128 f128_nsnan = vec_xfer_vui64t_2_bin128 (
      CONST_VINT128_DW(0xffff000080000000, 0));

  __binary128 x;
  long tests_count = 0;
  int test, expt;
  int rc = 0;

  printf ("\ntest_all_iszerof128 f128 -> bool int , ...\n");

#if 1
  tests_count++;
  x = (__binary128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_submin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsubmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__binary128) f128_nsnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif

  /* accumulate the number of values tested, in case we are doing
   * detail timing and want to compute function averages.  */
  tcount += tests_count;
  printf ("\ntest_all_iszerof128, tests=%ld fails=%d\n", tests_count, rc);

  return (rc);
}

int
test_all_is_f128 (void)
{
    int rc = 0;
    tcount = 0;

    printf ("\ntest_all_is_f128, ...\n");

    rc += test_all_isfinitef128 ();
    rc += test_all_isinff128 ();
    rc += test_all_isnanf128 ();
    rc += test_all_isnormalf128 ();
    rc += test_all_issubnormalf128 ();
    rc += test_all_iszerof128 ();
    rc += test_isinf_signf128 ();
    rc += test_signbitf128 ();

    printf ("\ntest_all_is f128, tests=%ld fails=%d\n", tcount, rc);

    return (rc);
}

int
test_vec_bool_f128 (void)
{
    int rc = 0;
    tcount = 0;

    printf ("\ntest_vec_bool_f128, ...\n");

    rc += test_setb_qp ();
    rc += test_isinff128 ();
    rc += test_isnanf128 ();
    rc += test_isfinitef128 ();
    rc += test_isnormalf128 ();
    rc += test_issubnormalf128 ();
    rc += test_iszerof128 ();

    printf ("\ntest_vec_bool f128, tests=%ld fails=%d\n", tcount, rc);

    return (rc);
}

int
test_vec_f128_f128 (void)
{
    int rc = 0;
    tcount = 0;

    printf ("\ntest_vec_f128 -> f128, ...\n");

    rc += test_absf128 ();
    rc += test_copysignf128 ();

    printf ("\ntest_vec_f128 f128, tests=%ld fails=%d\n", tcount, rc);

    return (rc);
}

#ifndef PVECLIB_DISABLE_F128MATH
#ifdef __FLOAT128__
extern __float128
test_scalar_add128 (__float128 vra, __float128 vrb);

extern __float128
test_scalar_div128 (__float128 vra, __float128 vrb);

extern __float128
test_scalar_mul128 (__float128 vra, __float128 vrb);

extern __float128
test_scalar_sub128 (__float128 vra, __float128 vrb);

extern __float128
test_scalarCC_expxsuba_128 (__float128 x, __float128 a, __float128 expa);

const __float128 f128_one = 1.0Q;
const __float128 f128_e = 2.71828182845904523536028747135266249775724709369996Q;
const __float128 inv_fact2 = (1.0Q / 2.0Q);
const __float128 inv_fact3 = (1.0Q / 6.0Q);
const __float128 inv_fact4 = (1.0Q / 24.0Q);
const __float128 inv_fact5 = (1.0Q / 120.0Q);
const __float128 inv_fact6 = (1.0Q / 720.0Q);
const __float128 inv_fact7 = (1.0Q / 5040.0Q);
const __float128 inv_fact8 = (1.0Q / 40320.0Q);

__float128
test_scalarLib_expxsuba_128 (__float128 x, __float128 a, __float128 expa)
{
  __float128 term, xma, xma2, xmaf2;
  __float128 xma3, xmaf3, xma4, xmaf4, xma5, xmaf5;
  __float128 xma6, xmaf6, xma7, xmaf7, xma8, xmaf8;

  // 1st 8 terms of e**x = e**a * taylor( x-a )
  xma = test_scalar_sub128 (x, a);
  term = test_scalar_add128 (f128_one, xma);
  xma2 = test_scalar_mul128 (xma, xma);
  xmaf2 = test_scalar_mul128 (xma2, inv_fact2);
  term = test_scalar_add128 (term, xmaf2);
  xma3 = test_scalar_mul128 (xma2, xma);
  xmaf3 = test_scalar_mul128 (xma3, inv_fact3);
  term = test_scalar_add128 (term, xmaf3);
  xma4 = test_scalar_mul128 (xma3, xma);
  xmaf4 = test_scalar_mul128 (xma4, inv_fact4);
  term = test_scalar_add128 (term, xmaf4);
  xma5 = test_scalar_mul128 (xma4, xma);
  xmaf5 = test_scalar_mul128 (xma5, inv_fact5);
  term = test_scalar_add128 (term, xmaf5);
  xma6 = test_scalar_mul128 (xma5, xma);
  xmaf6 = test_scalar_mul128 (xma6, inv_fact6);
  term = test_scalar_add128 (term, xmaf6);
  xma7 = test_scalar_mul128 (xma6, xma);
  xmaf7 = test_scalar_mul128 (xma7, inv_fact7);
  term = test_scalar_add128 (term, xmaf7);
  xma8 = test_scalar_mul128 (xma7, xma);
  xmaf8 = test_scalar_mul128 (xma8, inv_fact8);
  term = test_scalar_add128 (term, xmaf8);
  return test_scalar_mul128(expa, term);;
}

#define N 10
#define TIMING_ITERATIONS 10

extern __binary128
test_vec_max8_f128 (__binary128 vf1, __binary128 vf2,
		    __binary128 vf3, __binary128 vf4,
		    __binary128 vf5, __binary128 vf6,
		    __binary128 vf7, __binary128 vf8);

extern __binary128
test_vec_max8_f128uz (__binary128 vf1, __binary128 vf2,
		    __binary128 vf3, __binary128 vf4,
		    __binary128 vf5, __binary128 vf6,
		    __binary128 vf7, __binary128 vf8);

extern __binary128
test_gcc_max8_f128 (__binary128 vf1, __binary128 vf2,
		    __binary128 vf3, __binary128 vf4,
		    __binary128 vf5, __binary128 vf6,
		    __binary128 vf7, __binary128 vf8);

int timed_gcc_max8_f128 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  __float128 accum = 0.0Q;
  int i;

  for (i=0; i<N; i++)
    {
      accum += test_gcc_max8_f128 (f128_e, inv_fact2,
				   inv_fact3, inv_fact4,
				   inv_fact5, inv_fact6,
				   inv_fact7, inv_fact8);
    }
#endif
   return 0;
}

int timed_vec_max8_f128 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  __float128 accum = 0.0Q;
  int i;

  for (i=0; i<N; i++)
    {
      accum += test_vec_max8_f128 (f128_e, inv_fact2,
				   inv_fact3, inv_fact4,
				   inv_fact5, inv_fact6,
				   inv_fact7, inv_fact8);
    }
#endif
   return 0;
}

int timed_vec_max8_f128uz (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  __float128 accum = 0.0Q;
  int i;

  for (i=0; i<N; i++)
    {
      accum += test_vec_max8_f128uz (f128_e, inv_fact2,
				   inv_fact3, inv_fact4,
				   inv_fact5, inv_fact6,
				   inv_fact7, inv_fact8);
    }
#endif
   return 0;
}

int timed_expxsuba_v1_f128 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  __float128 accum = 0.0Q;
  int i;

  for (i=0; i<N; i++)
    {
      accum += test_scalarCC_expxsuba_128 (f128_one, f128_one, f128_e);
    }
#endif
   return 0;
}

int timed_expxsuba_v2_f128 (void)
{
#ifndef PVECLIB_DISABLE_F128MATH
  __float128 accum = 0.0Q;
  int i;

  for (i=0; i<N; i++)
    {
      accum += test_scalarLib_expxsuba_128 (f128_one, f128_one, f128_e);
    }
#endif
   return 0;
}

int
test_time_f128 (void)
{
  long i;
  uint64_t t_start, t_end, t_delta;
  double delta_sec;
  int rc = 0;

  printf ("\n%s f128 CC start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_expxsuba_v1_f128 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s f128 CC end", __FUNCTION__);
  printf ("\n%s f128 CC  tb delta = %lu, sec = %10.6g\n", __FUNCTION__, t_delta,
	  delta_sec);

  printf ("\n%s f128_LIB start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_expxsuba_v2_f128 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s f128_LIB end", __FUNCTION__);
  printf ("\n%s f128_LIB  tb delta = %lu, sec = %10.6g\n", __FUNCTION__,
	  t_delta, delta_sec);

  printf ("\n%s cmpgtuqp_gcc start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_gcc_max8_f128 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s cmpgtuqp_gcc end", __FUNCTION__);
  printf ("\n%s cmpgtuqp_gcc  tb delta = %lu, sec = %10.6g\n", __FUNCTION__,
	  t_delta, delta_sec);

  printf ("\n%s cmpgtuqp_vec start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_vec_max8_f128 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s cmpgtuqp_vec end", __FUNCTION__);
  printf ("\n%s cmpgtuqp_vec  tb delta = %lu, sec = %10.6g\n", __FUNCTION__,
	  t_delta, delta_sec);

  printf ("\n%s cmpgtuzqp_vec start, ...\n", __FUNCTION__);
  t_start = __builtin_ppc_get_timebase ();
  for (i = 0; i < TIMING_ITERATIONS; i++)
    {
      rc += timed_vec_max8_f128 ();
    }
  t_end = __builtin_ppc_get_timebase ();
  t_delta = t_end - t_start;
  delta_sec = TimeDeltaSec (t_delta);

  printf ("\n%s cmpgtuzqp_vec end", __FUNCTION__);
  printf ("\n%s cmpgtuzqp_vec  tb delta = %lu, sec = %10.6g\n", __FUNCTION__,
	  t_delta, delta_sec);

  return (rc);
}
#endif
#endif

int
test_extract_insert_f128 ()
{
  __binary128 x, xp, xpt;
  vui128_t sig, sigt, sigs;
  vui64_t exp, expt;
  int rc = 0;

  printf ("\ntest_extract_insert_f128 ...\n");

  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  exp = vec_xsxexpqp (x);
  expt =  CONST_VINT128_DW(0x0000000000000000, 0);
  rc += check_vuint128x ("check vec_xsxexpqp 1", (vui128_t) exp, (vui128_t) expt);

  sig = vec_xsxsigqp (x);
  sigt =  (vui128_t) CONST_VINT128_DW(0x0000000000000000, 0);
  rc += check_vuint128x ("check vec_xsxsigqp 1", (vui128_t) sig, (vui128_t) sigt);

  xp = vec_xsiexpqp (sig, exp);
  xpt = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  rc += check_f128 ("check vec_xsiexpqp 1", x, xpt, xp);

  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  exp = vec_xsxexpqp (x);
  expt =  CONST_VINT128_DW(0x0000000000000000, 0);
  rc += check_vuint128x ("check vec_xsxexpqp 2", (vui128_t) exp, (vui128_t) expt);

  sig = vec_xsxsigqp (x);
  sigt =  (vui128_t) CONST_VINT128_DW(0x0000000000000000, 0);
  rc += check_vuint128x ("check vec_xsxsigqp 2", (vui128_t) sig, (vui128_t) sigt);

  sigs = (vui128_t) vec_or ((vui64_t) sig, vf128_nzero);
  xp = vec_xsiexpqp (sigs, exp);
  xpt = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  rc += check_f128 ("check vec_xsiexpqp 2", x, xp, xpt);

  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  exp = vec_xsxexpqp (x);
  expt =  CONST_VINT128_DW(0x0000000000003fff, 0);
  rc += check_vuint128x ("check vec_xsxexpqp 3", (vui128_t) exp, (vui128_t) expt);

  sig = vec_xsxsigqp (x);
  sigt =  (vui128_t) CONST_VINT128_DW(0x0001000000000000, 0);
  rc += check_vuint128x ("check vec_xsxsigqp 3", (vui128_t) sig, (vui128_t) sigt);

  xp = vec_xsiexpqp (sig, exp);
  xpt = vec_xfer_vui64t_2_bin128 ( vf128_one );
  rc += check_f128 ("check vec_xsiexpqp 3", x, xpt, xp);

  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  exp = vec_xsxexpqp (x);
  expt =  CONST_VINT128_DW(0x0000000000003fff, 0);
  rc += check_vuint128x ("check vec_xsxexpqp 4", (vui128_t) exp, (vui128_t) expt);

  sig = vec_xsxsigqp (x);
  sigt =  (vui128_t) CONST_VINT128_DW(0x0001000000000000, 0);
  rc += check_vuint128x ("check vec_xsxsigqp 4", (vui128_t) sig, (vui128_t) sigt);

  sigs = (vui128_t) vec_or ((vui64_t) sig, vf128_nzero);
  xp = vec_xsiexpqp (sigs, exp);
  xpt = vec_xfer_vui64t_2_bin128 ( vf128_none );
  rc += check_f128 ("check vec_xsiexpqp 4", x, xpt, xp);

  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  exp = vec_xsxexpqp (x);
  expt =  CONST_VINT128_DW(0x0000000000007ffe, 0);
  rc += check_vuint128x ("check vec_xsxexpqp 5", (vui128_t) exp, (vui128_t) expt);

  sig = vec_xsxsigqp (x);
  sigt =  (vui128_t) CONST_VINT128_DW(0x0001ffffffffffff, 0xffffffffffffffff);
  rc += check_vuint128x ("check vec_xsxsigqp 5", (vui128_t) sig, (vui128_t) sigt);

  xp = vec_xsiexpqp (sig, exp);
  xpt = vec_xfer_vui64t_2_bin128 ( vf128_max );
  rc += check_f128 ("check vec_xsiexpqp 5", x, xpt, xp);

  x = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  exp = vec_xsxexpqp (x);
  expt =  CONST_VINT128_DW(0x0000000000007ffe, 0);
  rc += check_vuint128x ("check vec_xsxexpqp 6", (vui128_t) exp, (vui128_t) expt);

  sig = vec_xsxsigqp (x);
  sigt =  (vui128_t) CONST_VINT128_DW(0x0001ffffffffffff, 0xffffffffffffffff);
  rc += check_vuint128x ("check vec_xsxsigqp 6", (vui128_t) sig, (vui128_t) sigt);

  sigs = (vui128_t) vec_or ((vui64_t) sig, vf128_nzero);
  xp = vec_xsiexpqp (sigs, exp);
  xpt = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
  rc += check_f128 ("check vec_xsiexpqp 6", x, xpt, xp);

  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  exp = vec_xsxexpqp (x);
  expt =  CONST_VINT128_DW(0x0000000000000000, 0);
  rc += check_vuint128x ("check vec_xsxexpqp 7", (vui128_t) exp, (vui128_t) expt);

  sig = vec_xsxsigqp (x);
  sigt =  (vui128_t) CONST_VINT128_DW(0x0000ffffffffffff, 0xffffffffffffffff);
  rc += check_vuint128x ("check vec_xsxsigqp 7", (vui128_t) sig, (vui128_t) sigt);

  xp = vec_xsiexpqp (sig, exp);
  xpt = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  rc += check_f128 ("check vec_xsiexpqp 7", x, xpt, xp);

  x = vec_xfer_vui64t_2_bin128 ( vf128_nsub );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  exp = vec_xsxexpqp (x);
  expt =  CONST_VINT128_DW(0x0000000000000000, 0);
  rc += check_vuint128x ("check vec_xsxexpqp 8", (vui128_t) exp, (vui128_t) expt);

  sig = vec_xsxsigqp (x);
  sigt =  (vui128_t) CONST_VINT128_DW(0x0000ffffffffffff, 0xffffffffffffffff);
  rc += check_vuint128x ("check vec_xsxsigqp 8", (vui128_t) sig, (vui128_t) sigt);

  sigs = (vui128_t) vec_or ((vui64_t) sig, vf128_nzero);
  xp = vec_xsiexpqp (sigs, exp);
  xpt = vec_xfer_vui64t_2_bin128 ( vf128_nsub );
  rc += check_f128 ("check vec_xsiexpqp 8", x, xpt, xp);

  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  exp = vec_xsxexpqp (x);
  expt =  CONST_VINT128_DW(0x0000000000007fff, 0);
  rc += check_vuint128x ("check vec_xsxexpqp 9", (vui128_t) exp, (vui128_t) expt);

  sig = vec_xsxsigqp (x);
  sigt =  (vui128_t) CONST_VINT128_DW(0x0000000000000000, 0);
  rc += check_vuint128x ("check vec_xsxsigqp 9", (vui128_t) sig, (vui128_t) sigt);

  xp = vec_xsiexpqp (sig, exp);
  xpt = vec_xfer_vui64t_2_bin128 ( vf128_inf);
  rc += check_f128 ("check vec_xsiexpqp 9", x, xpt, xp);

  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  exp = vec_xsxexpqp (x);
  expt =  CONST_VINT128_DW(0x0000000000007fff, 0);
  rc += check_vuint128x ("check vec_xsxexpqp 10", (vui128_t) exp, (vui128_t) expt);

  sig = vec_xsxsigqp (x);
  sigt =  (vui128_t) CONST_VINT128_DW(0x0000000000000000, 0);
  rc += check_vuint128x ("check vec_xsxsigqp 10", (vui128_t) sig, (vui128_t) sigt);

  sigs = (vui128_t) vec_or ((vui64_t) sig, vf128_nzero);
  xp = vec_xsiexpqp (sigs, exp);
  xpt = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  rc += check_f128 ("check vec_xsiexpqp 10", x, xp, xpt);

  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  exp = vec_xsxexpqp (x);
  expt =  CONST_VINT128_DW(0x0000000000007fff, 0);
  rc += check_vuint128x ("check vec_xsxexpqp 11", (vui128_t) exp, (vui128_t) expt);

  sig = vec_xsxsigqp (x);
  sigt =  (vui128_t) CONST_VINT128_DW(0x0000800000000000, 0);
  rc += check_vuint128x ("check vec_xsxsigqp 11", (vui128_t) sig, (vui128_t) sigt);

  xp = vec_xsiexpqp (sig, exp);
  xpt = vec_xfer_vui64t_2_bin128 ( vf128_nan);
  rc += check_f128 ("check vec_xsiexpqp 11", x, xpt, xp);

  x = vec_xfer_vui64t_2_bin128 ( vf128_nnan );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  exp = vec_xsxexpqp (x);
  expt =  CONST_VINT128_DW(0x0000000000007fff, 0);
  rc += check_vuint128x ("check vec_xsxexpqp 12", (vui128_t) exp, (vui128_t) expt);

  sig = vec_xsxsigqp (x);
  sigt =  (vui128_t) CONST_VINT128_DW(0x0000800000000000, 0);
  rc += check_vuint128x ("check vec_xsxsigqp 12", (vui128_t) sig, (vui128_t) sigt);

  sigs = (vui128_t) vec_or ((vui64_t) sig, vf128_nzero);
  xp = vec_xsiexpqp (sigs, exp);
  xpt = vec_xfer_vui64t_2_bin128 ( vf128_nnan );
  rc += check_f128 ("check vec_xsiexpqp 12", x, xp, xpt);

  return (rc);
}

int
test_cmpeq_f128 ()
{
  __binary128 x, y;
  vb128_t exp, expt;
  int rc = 0;

  printf ("\ntest_cmpeq_f128 ...\n");
#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpequqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpequqp 1", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpequzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpequzqp 1", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpeqtoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpeqtoqp 1", (vui128_t) exp, (vui128_t) expt);
#endif
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpequqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpequqp 2", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpequzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpequzqp 2", (vui128_t) exp, (vui128_t) expt);

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  exp = vec_cmpeqtoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpeqtoqp 2", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpequqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpequqp 3", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpequzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpequzqp 3", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpeqtoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpeqtoqp 3", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_max );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpequqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpequqp 4", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpequzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpequzqp 4", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpeqtoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpeqtoqp 4", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpequqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpequqp 5", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpequzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpequzqp 5", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpeqtoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpeqtoqp 5", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpequqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpequqp 6", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpequzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpequzqp 6", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpeqtoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpeqtoqp 6", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpequqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpequqp 7", (vui128_t) exp, (vui128_t) expt);

  // P8 should detect these because absolute magnitudes are different
  exp = vec_cmpequzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpequzqp 7", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpeqtoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpeqtoqp 7", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpequqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpequqp 8", (vui128_t) exp, (vui128_t) expt);

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  exp = vec_cmpequzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpequzqp 8", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpeqtoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpeqtoqp 8", (vui128_t) exp, (vui128_t) expt);
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpequqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpequqp 9", (vui128_t) exp, (vui128_t) expt);

  // P8 should detect these because absolute magnitudes are different
  exp = vec_cmpequzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpequzqp 9", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpeqtoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpeqtoqp 9", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpequqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpequqp 10", (vui128_t) exp, (vui128_t) expt);

  // P8 should detect these because absolute magnitudes are different
  exp = vec_cmpequzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpequzqp 10", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpeqtoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpeqtoqp 10", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpequqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpequqp 11", (vui128_t) exp, (vui128_t) expt);

  // P8 should detect these because absolute magnitudes are different
  exp = vec_cmpequzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpequzqp 11", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpeqtoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpeqtoqp 11", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpequqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpequqp 12", (vui128_t) exp, (vui128_t) expt);

  // P8 should detect these because absolute magnitudes are different
  exp = vec_cmpequzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpequzqp 12", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpeqtoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpeqtoqp 12", (vui128_t) exp, (vui128_t) expt);
#endif
  return (rc);
}

int
test_cmpgt_f128 ()
{
  __binary128 x, y;
  vb128_t exp, expt;
  int rc = 0;

  printf ("\n%s\n", __FUNCTION__);
#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgtuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgtuqp 0", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgtuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgtuzqp 0", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgttoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgttoqp 0", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgtuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgtuqp 1", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgtuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgtuzqp 1", (vui128_t) exp, (vui128_t) expt);

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  exp = vec_cmpgttoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgttoqp 1", (vui128_t) exp, (vui128_t) expt);
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgtuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgtuqp 1b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgtuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgtuzqp 1b", (vui128_t) exp, (vui128_t) expt);

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  exp = vec_cmpgttoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgttoqp 1b", (vui128_t) exp, (vui128_t) expt);
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgtuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgtuqp 2", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgtuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgtuzqp 2", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgttoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgttoqp 2", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nsub );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgtuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgtuqp 2b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgtuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgtuzqp 2b", (vui128_t) exp, (vui128_t) expt);

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  exp = vec_cmpgttoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgttoqp 2b", (vui128_t) exp, (vui128_t) expt);
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_sub );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgtuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgtuqp 3", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgtuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgtuzqp 3", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgttoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgttoqp 3", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nsub );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgtuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgtuqp 3b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgtuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgtuzqp 3b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgttoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgttoqp 3b", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgtuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgtuqp 4", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgtuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgtuzqp 4", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgttoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgttoqp 4", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
  y = vec_xfer_vui64t_2_bin128 ( vf128_none );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgtuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgtuqp 4b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgtuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgtuzqp 4b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgttoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgttoqp 4b", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_max );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgtuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgtuqp 5", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgtuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgtuzqp 5", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgttoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgttoqp 5", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgtuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgtuqp 5b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgtuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgtuzqp 5b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgttoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgttoqp 5b", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgtuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgtuqp 6", (vui128_t) exp, (vui128_t) expt);

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  exp = vec_cmpgtuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgtuzqp 6", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgttoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgttoqp 6", (vui128_t) exp, (vui128_t) expt);
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nnan );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgtuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgtuqp 6b", (vui128_t) exp, (vui128_t) expt);

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  exp = vec_cmpgtuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgtuzqp 6b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgttoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgttoqp 6b", (vui128_t) exp, (vui128_t) expt);
#endif
#endif

  // Protect Clang from Q constants
#ifndef PVECLIB_DISABLE_F128MATH
  x = test_vec_max8_f128 (f128_e, inv_fact2, inv_fact3, inv_fact4, inv_fact5,
			  inv_fact6, inv_fact7, inv_fact8);
  exp = vec_cmpequqp (x, inv_fact8);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_max8_f128", x, exp, expt);
#endif

  return (rc);
}

int
test_vec_f128 (void)
{
  int rc = 0;

  printf ("\n%s\n", __FUNCTION__);

  rc += test_const_f128 ();
  rc += test_all_is_f128 ();
  rc += test_vec_bool_f128 ();
  rc += test_vec_f128_f128 ();
  rc += test_extract_insert_f128 ();
  rc += test_cmpeq_f128 ();
  rc += test_cmpgt_f128 ();
#ifndef PVECLIB_DISABLE_F128MATH
#ifdef __FLOAT128__
  rc += test_time_f128 ();
#endif
#endif
  return (rc);
}
