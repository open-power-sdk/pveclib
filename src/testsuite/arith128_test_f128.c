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
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgtuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgtuqp 3c", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgtuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgtuzqp 3c", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgttoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgttoqp 3c", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_none );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgtuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgtuqp 3d", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgtuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgtuzqp 3d", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgttoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgttoqp 3d", (vui128_t) exp, (vui128_t) expt);
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

  return (rc);
}

int
test_cmplt_f128 ()
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
  exp = vec_cmpltuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpltuqp 0", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpltuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpltuzqp 0", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmplttoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmplttoqp 0", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpltuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpltuqp 1", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpltuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpltuzqp 1", (vui128_t) exp, (vui128_t) expt);

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  exp = vec_cmplttoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmplttoqp 1", (vui128_t) exp, (vui128_t) expt);
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpltuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpltuqp 1b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpltuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpltuzqp 1b", (vui128_t) exp, (vui128_t) expt);

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  exp = vec_cmplttoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmplttoqp 1b", (vui128_t) exp, (vui128_t) expt);
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpltuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpltuqp 2", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpltuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpltuzqp 2", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmplttoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmplttoqp 2", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nsub );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpltuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpltuqp 2b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpltuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpltuzqp 2b", (vui128_t) exp, (vui128_t) expt);

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  exp = vec_cmplttoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmplttoqp 2b", (vui128_t) exp, (vui128_t) expt);
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_sub );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpltuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpltuqp 3", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpltuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpltuzqp 3", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmplttoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmplttoqp 3", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nsub );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpltuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpltuqp 3b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpltuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpltuzqp 3b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmplttoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmplttoqp 3b", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpltuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpltuqp 3c", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpltuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpltuzqp 3c", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmplttoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmplttoqp 3c", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_none );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpltuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpltuqp 3d", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpltuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpltuzqp 3d", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmplttoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmplttoqp 3d", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpltuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpltuqp 4", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpltuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpltuzqp 4", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmplttoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmplttoqp 4", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
  y = vec_xfer_vui64t_2_bin128 ( vf128_none );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpltuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpltuqp 4b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpltuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpltuzqp 4b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmplttoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmplttoqp 4b", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_max );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpltuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpltuqp 5", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpltuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpltuzqp 5", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmplttoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmplttoqp 5", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpltuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpltuqp 5b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpltuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpltuzqp 5b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmplttoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmplttoqp 5b", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpltuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpltuqp 6", (vui128_t) exp, (vui128_t) expt);

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  exp = vec_cmpltuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpltuzqp 6", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmplttoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmplttoqp 6", (vui128_t) exp, (vui128_t) expt);
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nnan );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpltuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpltuqp 6b", (vui128_t) exp, (vui128_t) expt);

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  exp = vec_cmpltuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpltuzqp 6b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmplttoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmplttoqp 6b", (vui128_t) exp, (vui128_t) expt);
#endif
#endif

  return (rc);
}

int
test_cmpge_f128 ()
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
  exp = vec_cmpgeuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgeuqp 0", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgeuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgeuzqp 0", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgetoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgetoqp 0", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgeuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgeuqp 1", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgeuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgeuzqp 1", (vui128_t) exp, (vui128_t) expt);

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  exp = vec_cmpgetoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgetoqp 1", (vui128_t) exp, (vui128_t) expt);
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgeuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgeuqp 1b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgeuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgeuzqp 1b", (vui128_t) exp, (vui128_t) expt);

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  exp = vec_cmpgetoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgetoqp 1b", (vui128_t) exp, (vui128_t) expt);
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgeuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgeuqp 2", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgeuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgeuzqp 2", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgetoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgetoqp 2", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nsub );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgeuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgeuqp 2b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgeuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgeuzqp 2b", (vui128_t) exp, (vui128_t) expt);

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  exp = vec_cmpgetoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgetoqp 2b", (vui128_t) exp, (vui128_t) expt);
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nsub );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nsub );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgeuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgeuqp 2c", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgeuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgeuzqp 2c", (vui128_t) exp, (vui128_t) expt);

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  exp = vec_cmpgetoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgetoqp 2c", (vui128_t) exp, (vui128_t) expt);
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_sub );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgeuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgeuqp 3", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgeuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgeuzqp 3", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgetoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgetoqp 3", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nsub );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgeuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgeuqp 3b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgeuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgeuzqp 3b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgetoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgetoqp 3b", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgeuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgeuqp 3c", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgeuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgeuzqp 3c", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgetoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgetoqp 3c", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_none );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgeuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgeuqp 3d", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgeuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgeuzqp 3d", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgetoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgetoqp 3d", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgeuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgeuqp 3e", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgeuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgeuzqp 3e", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgetoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgetoqp 3e", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgeuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgeuqp 4", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgeuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgeuzqp 4", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgetoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgetoqp 4", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
  y = vec_xfer_vui64t_2_bin128 ( vf128_none );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgeuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgeuqp 4b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgeuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgeuzqp 4b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgetoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgetoqp 4b", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_max );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgeuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgeuqp 5", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgeuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgeuzqp 5", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgetoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpgetoqp 5", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgeuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgeuqp 5b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgeuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgeuzqp 5b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgetoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgetoqp 5b", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgeuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgeuqp 6", (vui128_t) exp, (vui128_t) expt);

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  exp = vec_cmpgeuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgeuzqp 6", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgetoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgetoqp 6", (vui128_t) exp, (vui128_t) expt);
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nnan );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpgeuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgeuqp 6b", (vui128_t) exp, (vui128_t) expt);

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  exp = vec_cmpgeuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgeuzqp 6b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpgetoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpgetoqp 6b", (vui128_t) exp, (vui128_t) expt);
#endif
#endif

  return (rc);
}
int
test_cmple_f128 ()
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
  exp = vec_cmpleuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpleuqp 0", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpleuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpleuzqp 0", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpletoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpletoqp 0", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpleuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpleuqp 1", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpleuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpleuzqp 1", (vui128_t) exp, (vui128_t) expt);

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  exp = vec_cmpletoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpletoqp 1", (vui128_t) exp, (vui128_t) expt);
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpleuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpleuqp 1b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpleuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpleuzqp 1b", (vui128_t) exp, (vui128_t) expt);

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  exp = vec_cmpletoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpletoqp 1b", (vui128_t) exp, (vui128_t) expt);
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpleuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpleuqp 2", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpleuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpleuzqp 2", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpletoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpletoqp 2", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nsub );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpleuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpleuqp 2b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpleuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpleuzqp 2b", (vui128_t) exp, (vui128_t) expt);

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  exp = vec_cmpletoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpletoqp 2b", (vui128_t) exp, (vui128_t) expt);
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nsub );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nsub );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpleuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpleuqp 2c", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpleuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpleuzqp 2c", (vui128_t) exp, (vui128_t) expt);

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  exp = vec_cmpletoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpletoqp 2c", (vui128_t) exp, (vui128_t) expt);
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_sub );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpleuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpleuqp 3", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpleuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpleuzqp 3", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpletoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpletoqp 3", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nsub );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpleuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpleuqp 3b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpleuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpleuzqp 3b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpletoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpletoqp 3b", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpleuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpleuqp 3c", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpleuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpleuzqp 3c", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpletoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpletoqp 3c", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_none );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpleuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpleuqp 3d", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpleuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpleuzqp 3d", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpletoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpletoqp 3d", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpleuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpleuqp 3e", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpleuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpleuzqp 3e", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpletoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpletoqp 3e", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpleuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpleuqp 4", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpleuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpleuzqp 4", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpletoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpletoqp 4", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
  y = vec_xfer_vui64t_2_bin128 ( vf128_none );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpleuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpleuqp 4b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpleuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpleuzqp 4b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpletoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpletoqp 4b", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_max );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpleuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpleuqp 5", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpleuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpleuzqp 5", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpletoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpletoqp 5", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpleuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpleuqp 5b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpleuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpleuzqp 5b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpletoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpletoqp 5b", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpleuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpleuqp 6", (vui128_t) exp, (vui128_t) expt);

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  exp = vec_cmpleuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpleuzqp 6", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpletoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpletoqp 6", (vui128_t) exp, (vui128_t) expt);
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nnan );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpleuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpleuqp 6b", (vui128_t) exp, (vui128_t) expt);

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  exp = vec_cmpleuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpleuzqp 6b", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpletoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpletoqp 6b", (vui128_t) exp, (vui128_t) expt);
#endif
#endif

  return (rc);
}

int
test_cmpne_f128 ()
{
  __binary128 x, y;
  vb128_t exp, expt;
  int rc = 0;

  printf ("\ntest_cmpne_f128 ...\n");
#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpneuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpneuqp 1", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpneuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpneuzqp 1", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpnetoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpnetoqp 1", (vui128_t) exp, (vui128_t) expt);
#endif
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpneuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpneuqp 2", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpneuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpneuzqp 2", (vui128_t) exp, (vui128_t) expt);

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  exp = vec_cmpnetoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpnetoqp 2", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpneuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpneuqp 3", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpneuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpneuzqp 3", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpnetoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpnetoqp 3", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_max );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpneuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpneuqp 4", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpneuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpneuzqp 4", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpnetoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpnetoqp 4", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpneuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpneuqp 5", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpneuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpneuzqp 5", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpnetoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpnetoqp 5", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpneuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpneuqp 6", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpneuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpneuzqp 6", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpnetoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpnetoqp 6", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpneuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpneuqp 7", (vui128_t) exp, (vui128_t) expt);

  // P8 should detect these because absolute magnitudes are different
  exp = vec_cmpneuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpneuzqp 7", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpnetoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpnetoqp 7", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpneuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpneuqp 8", (vui128_t) exp, (vui128_t) expt);

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  exp = vec_cmpneuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpneuzqp 8", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpnetoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpnetoqp 8", (vui128_t) exp, (vui128_t) expt);
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpneuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpneuqp 9", (vui128_t) exp, (vui128_t) expt);

  // P8 should detect these because absolute magnitudes are different
  exp = vec_cmpneuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpneuzqp 9", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpnetoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpnetoqp 9", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpneuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpneuqp 10", (vui128_t) exp, (vui128_t) expt);

  // P8 should detect these because absolute magnitudes are different
  exp = vec_cmpneuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpneuzqp 10", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpnetoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpnetoqp 10", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpneuqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpneuqp 11", (vui128_t) exp, (vui128_t) expt);

  // P8 should detect these because absolute magnitudes are different
  exp = vec_cmpneuzqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpneuzqp 11", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpnetoqp (x, y);
  expt = (vb128_t) vf128_false;
  rc += check_vuint128x ("check vec_cmpnetoqp 11", (vui128_t) exp, (vui128_t) expt);
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  exp = vec_cmpneuqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpneuqp 12", (vui128_t) exp, (vui128_t) expt);

  // P8 should detect these because absolute magnitudes are different
  exp = vec_cmpneuzqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpneuzqp 12", (vui128_t) exp, (vui128_t) expt);

  exp = vec_cmpnetoqp (x, y);
  expt = (vb128_t) vf128_true;
  rc += check_vuint128x ("check vec_cmpnetoqp 12", (vui128_t) exp, (vui128_t) expt);
#endif
  return (rc);
}

//#define __DEBUG_PRINT__
int
test_cmpeq_all_f128 ()
{
  __binary128 x, y;
  int rc = 0;

  printf ("\ntest_cmpeq_all_f128 ...\n");
#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (!vec_cmpqp_all_eq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_eq 1 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzeq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzeq 1 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }

  if (!vec_cmpqp_all_toeq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_toeq 1 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }
#endif
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (!vec_cmpqp_all_eq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_eq 2 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzeq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzeq 2 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  if (!vec_cmpqp_all_toeq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_toeq 2 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (!vec_cmpqp_all_eq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_eq 3 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzeq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzeq 3 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }

  if (!vec_cmpqp_all_toeq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_toeq 3 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_max );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (!vec_cmpqp_all_eq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_eq 4 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzeq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzeq 4 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }

  if (!vec_cmpqp_all_toeq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_toeq 4 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_eq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_eq 5 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }

  if (vec_cmpqp_all_uzeq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzeq 5 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }

  if (vec_cmpqp_all_toeq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_toeq 5 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (!vec_cmpqp_all_eq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_eq 6 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzeq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzeq 6 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }

  if (!vec_cmpqp_all_toeq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_toeq 6 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_eq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_eq 7 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }

  // P8 should detect these because absolute magnitudes are different
  if (vec_cmpqp_all_uzeq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzeq 7 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }

  if (vec_cmpqp_all_toeq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_toeq 7 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_eq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_eq 8 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  if (vec_cmpqp_all_uzeq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzeq 8 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }

  if (vec_cmpqp_all_toeq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_toeq 8 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_eq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_eq 9 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }

  // P8 should detect these because absolute magnitudes are different
  if (vec_cmpqp_all_uzeq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzeq 9 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }

  if (vec_cmpqp_all_toeq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_toeq 9 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_eq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_eq 10 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }

  // P8 should detect these because absolute magnitudes are different
  if (vec_cmpqp_all_uzeq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzeq 10 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }

  if (vec_cmpqp_all_toeq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_toeq 10 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (!vec_cmpqp_all_eq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_eq 11 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }

  // P8 should detect these because absolute magnitudes are different
  if (!vec_cmpqp_all_uzeq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzeq 11 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }

  if (!vec_cmpqp_all_toeq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_toeq 11 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_eq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_eq 12 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }

  // P8 should detect these because absolute magnitudes are different
  if (vec_cmpqp_all_uzeq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzeq 12 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }

  if (vec_cmpqp_all_toeq (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_toeq 12 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_eq( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_eq (x, y));
#endif
  }
#endif
  return (rc);
}

int
test_cmpgt_all_f128 ()
{
  __binary128 x, y;
  int rc = 0;

  printf ("\n%s\n", __FUNCTION__);
#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_gt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_gt 0 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

  if (vec_cmpqp_all_uzgt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzgt 0 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

  if (vec_cmpqp_all_togt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_togt 0 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif

  if (vec_cmpqp_all_gt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_gt 1 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

  if (vec_cmpqp_all_uzgt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzgt 1 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  if (vec_cmpqp_all_togt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_togt 1 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif

  if (vec_cmpqp_all_gt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_gt 1b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

  if (vec_cmpqp_all_uzgt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzgt 1b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  if (vec_cmpqp_all_togt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_togt 1b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (!vec_cmpqp_all_gt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_gt 2 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzgt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzgt 2 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

  if (!vec_cmpqp_all_togt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_togt 2 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nsub );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_gt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_gt 2b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

  if (vec_cmpqp_all_uzgt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzgt 2b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  if (vec_cmpqp_all_togt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_togt 2b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_sub );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (!vec_cmpqp_all_gt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_gt 3 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzgt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzgt 3 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

  if (!vec_cmpqp_all_togt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_togt 3 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nsub );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif

  if (vec_cmpqp_all_gt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_gt 3b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

  if (vec_cmpqp_all_uzgt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzgt 3b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

  if (vec_cmpqp_all_togt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_togt 3b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif

  if (vec_cmpqp_all_gt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_gt 3c fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

  if (vec_cmpqp_all_uzgt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzgt 3c fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

  if (vec_cmpqp_all_togt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_togt 3c fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_none );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (!vec_cmpqp_all_gt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_gt 3d fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzgt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzgt 3d fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

  if (!vec_cmpqp_all_togt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_togt 3d fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (!vec_cmpqp_all_gt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_gt 4 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzgt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzgt 4 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

  if (!vec_cmpqp_all_togt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_togt 4 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
  y = vec_xfer_vui64t_2_bin128 ( vf128_none );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif

  if (vec_cmpqp_all_gt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_gt 4b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

  if (vec_cmpqp_all_uzgt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzgt 4b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

  if (vec_cmpqp_all_togt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_togt 4b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_max );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (!vec_cmpqp_all_gt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_gt 5 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzgt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzgt 5 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

  if (!vec_cmpqp_all_togt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_togt 5 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif

  if (vec_cmpqp_all_gt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_gt 5b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

  if (vec_cmpqp_all_uzgt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzgt 5b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

  if (vec_cmpqp_all_togt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_togt 5b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_gt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_gt 6 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  if (vec_cmpqp_all_uzgt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzgt 6 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

  if (vec_cmpqp_all_togt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_togt 6 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nnan );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_gt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_gt 6b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  if (vec_cmpqp_all_uzgt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzgt 6b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }

  if (vec_cmpqp_all_togt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_togt 6b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_gt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_gt (x, y));
#endif
  }
#endif
#endif

  return (rc);
}

int
test_cmplt_all_f128 ()
{
  __binary128 x, y;
  int rc = 0;

  printf ("\n%s\n", __FUNCTION__);
#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_lt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_lt 0 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

  if (vec_cmpqp_all_uzlt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzlt 0 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

  if (vec_cmpqp_all_tolt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tolt 0 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif

  if (vec_cmpqp_all_lt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_lt 1 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

  if (vec_cmpqp_all_uzlt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzlt 1 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  if (vec_cmpqp_all_tolt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tolt 1 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif

  if (vec_cmpqp_all_lt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_lt 1b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

  if (vec_cmpqp_all_uzlt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzlt 1b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  if (vec_cmpqp_all_tolt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tolt 1b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_lt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_lt 2 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

  if (vec_cmpqp_all_uzlt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzlt 2 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

  if (vec_cmpqp_all_tolt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tolt 2 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nsub );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (!vec_cmpqp_all_lt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_lt 2b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzlt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzlt 2b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  if (!vec_cmpqp_all_tolt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tolt 2b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_sub );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_lt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_lt 3 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

  if (vec_cmpqp_all_uzlt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzlt 3 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

  if (vec_cmpqp_all_tolt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tolt 3 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nsub );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif

  if (!vec_cmpqp_all_lt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_lt 3b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzlt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzlt 3b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

  if (!vec_cmpqp_all_tolt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tolt 3b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif

  if (!vec_cmpqp_all_lt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_lt 3c fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzlt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzlt 3c fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

  if (!vec_cmpqp_all_tolt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tolt 3c fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_none );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_lt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_lt 3d fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

  if (vec_cmpqp_all_uzlt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzlt 3d fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

  if (vec_cmpqp_all_tolt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tolt 3d fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_lt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_lt 4 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

  if (vec_cmpqp_all_uzlt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzlt 4 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

  if (vec_cmpqp_all_tolt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tolt 4 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
  y = vec_xfer_vui64t_2_bin128 ( vf128_none );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif

  if (!vec_cmpqp_all_lt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_lt 4b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzlt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzlt 4b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

  if (!vec_cmpqp_all_tolt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tolt 4b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_max );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_lt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_lt 5 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

  if (vec_cmpqp_all_uzlt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzlt 5 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

  if (vec_cmpqp_all_tolt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tolt 5 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif

  if (!vec_cmpqp_all_lt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_lt 5b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzlt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzlt 5b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

  if (!vec_cmpqp_all_tolt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tolt 5b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_lt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_lt 6 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  if (vec_cmpqp_all_uzlt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzlt 6 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

  if (vec_cmpqp_all_tolt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tolt 6 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nnan );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_lt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_lt 6b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  if (vec_cmpqp_all_uzlt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzlt 6b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }

  if (vec_cmpqp_all_tolt (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tolt 6b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_lt( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_lt (x, y));
#endif
  }
#endif
#endif

  return (rc);
}

int
test_cmpge_all_f128 ()
{
  __binary128 x, y;
  int rc = 0;

  printf ("\n%s\n", __FUNCTION__);
#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (!vec_cmpqp_all_ge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_ge 0 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzge 0 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

  if (!vec_cmpqp_all_toge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_toge 0 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif

  if (!vec_cmpqp_all_ge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_ge 1 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzge 1 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  if (!vec_cmpqp_all_toge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_toge 1 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif

  if (!vec_cmpqp_all_ge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_ge 1b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzge 1b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  if (!vec_cmpqp_all_toge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_toge 1b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (!vec_cmpqp_all_ge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_ge 2 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzge 2 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

  if (!vec_cmpqp_all_toge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_toge 2 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nsub );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_ge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_ge 2b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

  if (vec_cmpqp_all_uzge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzge 2b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  if (vec_cmpqp_all_toge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_toge 2b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_sub );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (!vec_cmpqp_all_ge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_ge 3 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzge 3 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

  if (!vec_cmpqp_all_toge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_toge 3 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nsub );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif

  if (vec_cmpqp_all_ge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_ge 3b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

  if (vec_cmpqp_all_uzge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzge 3b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

  if (vec_cmpqp_all_toge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_toge 3b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif

  if (vec_cmpqp_all_ge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_ge 3c fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

  if (vec_cmpqp_all_uzge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzge 3c fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

  if (vec_cmpqp_all_toge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_toge 3c fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_none );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (!vec_cmpqp_all_ge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_ge 3d fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzge 3d fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

  if (!vec_cmpqp_all_toge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_toge 3d fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (!vec_cmpqp_all_ge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_ge 4 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzge 4 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

  if (!vec_cmpqp_all_toge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_toge 4 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
  y = vec_xfer_vui64t_2_bin128 ( vf128_none );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif

  if (vec_cmpqp_all_ge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_ge 4b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

  if (vec_cmpqp_all_uzge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzge 4b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

  if (vec_cmpqp_all_toge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_toge 4b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_max );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (!vec_cmpqp_all_ge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_ge 5 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzge 5 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

  if (!vec_cmpqp_all_toge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_toge 5 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif

  if (vec_cmpqp_all_ge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_ge 5b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

  if (vec_cmpqp_all_uzge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzge 5b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

  if (vec_cmpqp_all_toge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_toge 5b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_ge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_ge 6 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  if (vec_cmpqp_all_uzge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzge 6 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

  if (vec_cmpqp_all_toge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_toge 6 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nnan );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_ge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_ge 6b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  if (vec_cmpqp_all_uzge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzge 6b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }

  if (vec_cmpqp_all_toge (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_toge 6b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ge( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ge (x, y));
#endif
  }
#endif
#endif

  return (rc);
}

int
test_cmple_all_f128 ()
{
  __binary128 x, y;
  int rc = 0;

  printf ("\n%s\n", __FUNCTION__);
#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (!vec_cmpqp_all_le (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_le 0 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzle (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzle 0 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

  if (!vec_cmpqp_all_tole (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tole 0 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif

  if (!vec_cmpqp_all_le (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_le 1 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzle (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzle 1 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  if (!vec_cmpqp_all_tole (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tole 1 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif

  if (!vec_cmpqp_all_le (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_le 1b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzle (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzle 1b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  if (!vec_cmpqp_all_tole (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tole 1b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_sub );
  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_le (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_le 2 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

  if (vec_cmpqp_all_uzle (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzle 2 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

  if (vec_cmpqp_all_tole (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tole 2 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nsub );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (!vec_cmpqp_all_le (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_le 2b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzle (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzle 2b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  if (!vec_cmpqp_all_tole (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tole 2b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_sub );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_le (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_le 3 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

  if (vec_cmpqp_all_uzle (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzle 3 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

  if (vec_cmpqp_all_tole (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tole 3 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nsub );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif

  if (!vec_cmpqp_all_le (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_le 3b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzle (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzle 3b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

  if (!vec_cmpqp_all_tole (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tole 3b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_none );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif

  if (!vec_cmpqp_all_le (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_le 3c fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzle (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzle 3c fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

  if (!vec_cmpqp_all_tole (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tole 3c fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_one );
  y = vec_xfer_vui64t_2_bin128 ( vf128_none );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_le (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_le 3d fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

  if (vec_cmpqp_all_uzle (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzle 3d fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

  if (vec_cmpqp_all_tole (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tole 3d fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_one );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_le (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_le 4 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

  if (vec_cmpqp_all_uzle (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzle 4 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

  if (vec_cmpqp_all_tole (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tole 4 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
  y = vec_xfer_vui64t_2_bin128 ( vf128_none );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif

  if (!vec_cmpqp_all_le (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_le 4b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzle (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzle 4b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

  if (!vec_cmpqp_all_tole (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tole 4b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_max );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_le (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_le 5 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

  if (vec_cmpqp_all_uzle (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzle 5 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

  if (vec_cmpqp_all_tole (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tole 5 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif

  if (!vec_cmpqp_all_le (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_le 5b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzle (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzle 5b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

  if (!vec_cmpqp_all_tole (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tole 5b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_le (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_le 6 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  if (vec_cmpqp_all_uzle (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzle 6 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

  if (vec_cmpqp_all_tole (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tole 6 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nnan );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_le (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_le 6b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  if (vec_cmpqp_all_uzle (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzle 6b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }

  if (vec_cmpqp_all_tole (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tole 6b fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_le( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_le (x, y));
#endif
  }
#endif
#endif

  return (rc);
}

//#define __DEBUG_PRINT__
int
test_cmpne_all_f128 ()
{
  __binary128 x, y;
  int rc = 0;

  printf ("\ntest_cmpne_all_f128 ...\n");
#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_zero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_ne (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_ne 1 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }

  if (vec_cmpqp_all_uzne (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzne 1 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }

  if (vec_cmpqp_all_tone (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tone 1 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }
#endif
  x = vec_xfer_vui64t_2_bin128 ( vf128_zero );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_ne (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_ne 2 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }

  if (vec_cmpqp_all_uzne (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzne 2 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  if (vec_cmpqp_all_tone (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tone 2 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nzero );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_ne (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_ne 3 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }

  if (vec_cmpqp_all_uzne (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzne 3 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }

  if (vec_cmpqp_all_tone (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tone 3 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_max );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_ne (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_ne 4 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }

  if (vec_cmpqp_all_uzne (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzne 4 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }

  if (vec_cmpqp_all_tone (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tone 4 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (!vec_cmpqp_all_ne (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_ne 5 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }

  if (!vec_cmpqp_all_uzne (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzne 5 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }

  if (!vec_cmpqp_all_tone (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tone 5 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_ne (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_ne 6 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }

  if (vec_cmpqp_all_uzne (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzne 6 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }

  if (vec_cmpqp_all_tone (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tone 6 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nmax );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_ne (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_ne 7 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // P8 should detect these because absolute magnitudes are different
  if (vec_cmpqp_all_uzne (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzne 7 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }

  if (vec_cmpqp_all_tone (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tone 7 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_nan );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_ne (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_ne 8 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // Don't expect to detect this case for P8 but should on P9
  if (vec_cmpqp_all_uzne (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzne 8 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }

  if (vec_cmpqp_all_tone (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tone 8 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_nan );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_ne (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_ne 9 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }

#if defined (_ARCH_PWR9) && defined (__FLOAT128__) && (__GNUC__ > 7)
  // P8 should detect these because absolute magnitudes are different
  if (vec_cmpqp_all_uzne (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzne 9 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }

  if (vec_cmpqp_all_tone (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tone 9 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }
#endif
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_max );
  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (!vec_cmpqp_all_ne (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_ne 10 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }

  // P8 should detect these because absolute magnitudes are different
  if (!vec_cmpqp_all_uzne (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzne 10 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }

  if (!vec_cmpqp_all_tone (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tone 10 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_inf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (vec_cmpqp_all_ne (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_ne 11 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }

  // P8 should detect these because absolute magnitudes are different
  if (vec_cmpqp_all_uzne (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzne 11 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }

  if (vec_cmpqp_all_tone (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tone 11 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }
#endif

#if 1
  x = vec_xfer_vui64t_2_bin128 ( vf128_ninf );
  y = vec_xfer_vui64t_2_bin128 ( vf128_inf );
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  if (!vec_cmpqp_all_ne (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_ne 12 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }

  // P8 should detect these because absolute magnitudes are different
  if (!vec_cmpqp_all_uzne (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_uzne 12 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }

  if (!vec_cmpqp_all_tone (x, y))
  {
      rc += 1;
      printf ("vec_cmpqp_all_tone 12 fail\n");
#ifdef __DEBUG_PRINT__
      print_vfloat128x ("vec_cmpqp_all_ne( ", x);
      print_vfloat128x ("                 ,", y);
      printf           ("                )= %d",
			vec_cmpqp_all_ne (x, y));
#endif
  }
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
  rc += test_cmplt_f128 ();
  rc += test_cmpge_f128 ();
  rc += test_cmple_f128 ();
  rc += test_cmpne_f128 ();

  rc += test_cmpeq_all_f128 ();
  rc += test_cmpgt_all_f128 ();
  rc += test_cmplt_all_f128 ();
  rc += test_cmpge_all_f128 ();
  rc += test_cmple_all_f128 ();
  rc += test_cmpne_all_f128 ();
  return (rc);
}
