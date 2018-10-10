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
      Commited on: Oct 10, 2018
 */


#define __STDC_WANT_DEC_FP__    1

#include <stdint.h>
#include <stdio.h>
#include <float.h>

#include "arith128.h"
#include <testsuite/arith128_print.h>
#include "vec_int128_ppc.h"
#include "vec_f128_ppc.h"
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
#else
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

  __float128 x;
  long tests_count = 0;
  int test, expt;
  int rc = 0;

  printf ("\ntest_isinf_signf128 f128 -> int , ...\n");

#if 1
  tests_count++;
  x = (__float128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = -1;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinf_signf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_isinf_signf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsnan;
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

  __float128 x;
  vb128_t test, expt;
  long tests_count = 0;
  int rc = 0;

  printf ("\ntest_setb_qp f128 -> vector bool , ...\n");

#if 1
  tests_count++;
  x = (__float128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_setb_qp (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_setb_qp", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsnan;
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

  __float128 x;
  long tests_count = 0;
  int test, expt;
  int rc = 0;

#ifdef __DEBUG_PRINT__
  x = (__float128)f128_zero;
  print_vfloat128x(" x=  ", x);
  x = (__float128)f128_nzero;
  print_vfloat128x(" x=  ", x);
  x = (__float128)f128_one;
  print_vfloat128x(" x=  ", x);
  x = (__float128)f128_none;
  print_vfloat128x(" x=  ", x);
  x = (__float128)f128_max;
  print_vfloat128x(" x=  ", x);
  x = (__float128)f128_nmax;
  print_vfloat128x(" x=  ", x);
  x = (__float128)f128_min;
  print_vfloat128x(" x=  ", x);
  x = (__float128)f128_nmin;
  print_vfloat128x(" x=  ", x);
  x = (__float128)f128_sub;
  print_vfloat128x(" x=  ", x);
  x = (__float128)f128_nsub;
  print_vfloat128x(" x=  ", x);
  x = (__float128)f128_inf;
  print_vfloat128x(" x=  ", x);
  x = (__float128)f128_ninf;
  print_vfloat128x(" x=  ", x);
  x = (__float128)f128_nan;
  print_vfloat128x(" x=  ", x);
  x = (__float128)f128_nnan;
  print_vfloat128x(" x=  ", x);
  x = (__float128)f128_snan;
  print_vfloat128x(" x=  ", x);
  x = (__float128)f128_nsnan;
  print_vfloat128x(" x=  ", x);
#endif

  printf ("\ntest_signbitf128 f128 -> int bool , ...\n");

#if 1
  tests_count++;
  x = (__float128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_signbitf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_signbitf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsnan;
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

  __float128 x;
  vb128_t test, expt;
  long tests_count = 0;
  int rc = 0;

  printf ("\ntest_isinff128 f128 -> vector bool ...\n");
  /* this version (with #undef __DEBUG_PRINT__) is silent if there
   * are no errors.  Better for profiling.  */

#if 1
  tests_count++;
  x = (__float128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isinff128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsnan;
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

  __float128 x;
  vb128_t test, expt;
  long tests_count = 0;
  int rc = 0;

  printf ("\ntest_isnanf128 f128 -> vector bool ...\n");
  /* this version (with #undef __DEBUG_PRINT__) is silent if there
   * are no errors.  Better for profiling.  */

#if 1
  tests_count++;
  x = (__float128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnanf128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsnan;
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

  __float128 x;
  vb128_t test, expt;
  long tests_count = 0;
  int rc = 0;

  printf ("\ntest_isfinitef128 f128 -> vector bool ...\n");
  /* this version (with #undef __DEBUG_PRINT__) is silent if there
   * are no errors.  Better for profiling.  */

#if 1
  tests_count++;
  x = (__float128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isfinitef128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isfinitef128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsnan;
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

  __float128 x;
  vb128_t test, expt;
  long tests_count = 0;
  int rc = 0;

  printf ("\ntest_isnormalf128 f128 -> vector bool ...\n");
  /* this version (with #undef __DEBUG_PRINT__) is silent if there
   * are no errors.  Better for profiling.  */

#if 1
  tests_count++;
  x = (__float128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_isnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsnan;
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

  __float128 x;
  vb128_t test, expt;
  long tests_count = 0;
  int rc = 0;

  printf ("\ntest_issubnormalf128 f128 -> vector bool ...\n");
  /* this version (with #undef __DEBUG_PRINT__) is silent if there
   * are no errors.  Better for profiling.  */

#if 1
  tests_count++;
  x = (__float128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_sub2;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsub2;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_issubnormalf128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsnan;
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

  __float128 x;
  vb128_t test, expt;
  long tests_count = 0;
  int rc = 0;

  printf ("\ntest_iszerof128 f128 -> vector bool ...\n");
  /* this version (with #undef __DEBUG_PRINT__) is silent if there
   * are no errors.  Better for profiling.  */

#if 1
  tests_count++;
  x = (__float128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_true;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_sub2;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsub2;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_iszerof128 (x);
  expt = (vb128_t) vf128_false;
  rc += check_f128bool ("check vec_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsnan;
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


  __float128 x;
  __float128 t, e;
  long tests_count = 0;
  int rc = 0;

  printf ("\ntest_absf128 f128 -> f128 ...\n");

#if 1
  tests_count++;
  x = (__float128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = (__float128) f128_zero;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = (__float128) f128_zero;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = f128_one;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = f128_one;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = f128_max;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = f128_max;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = f128_min;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = f128_min;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = f128_sub;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = f128_sub;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = f128_inf;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = f128_inf;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = f128_nan;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = f128_nan;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_absf128 (x);
  e = f128_snan;
  rc += check_f128 ("check vec_absf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsnan;
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

  __float128 x, y, t, e;
  long tests_count = 0;
  int rc = 0;

  printf ("\ntest_copysignf128 f128 -> f128 ...\n");

#if 1
  tests_count++;
  x = (__float128) f128_zero;
  y = (__float128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = vec_copysignf128 (x, y);
  e = (__float128) f128_zero;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = (__float128) f128_zero;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_one;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_one;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_max;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_max;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_min;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_min;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_sub;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_sub;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_inf;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_inf;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_nan;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_nan;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_snan;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif

#if 1
  tests_count++;
  x = (__float128) f128_zero;
  y = (__float128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
  print_vfloat128x(" y=  ", y);
#endif
  t = vec_copysignf128 (x, y);
  e = (__float128) f128_nzero;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = (__float128) f128_nzero;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_none;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_none;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_nmax;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_nmax;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_nmin;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_nmin;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_nsub;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_nsub;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_ninf;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_ninf;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  t = vec_copysignf128 (x, y);
  e = f128_nnan;
  rc += check_f128 ("check vec_copysignf128", x, t, e);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nnan;
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

  __float128 x, t, e;
  long tests_count = 0;
  int rc = 0;
  /* this version (with #undef __DEBUG_PRINT__) is silent if there
   * are no errors.  Better for profiling.  */
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

  __float128 x;
  long tests_count = 0;
  int test, expt;
  int rc = 0;

  printf ("\ntest_all_isfinitef128 f128 -> bool int , ...\n");
  /* this version (with #undef __DEBUG_PRINT__) is silent if there
   * are no errors.  Better for profiling.  */
#if 1
  tests_count++;
  x = (__float128)f128_zero;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 1;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__float128)f128_nzero;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 1;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__float128)f128_one;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 1;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__float128)f128_none;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 1;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__float128)f128_max;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 1;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__float128)f128_nmax;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 1;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__float128)f128_min;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 1;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__float128)f128_nmin;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 1;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__float128)f128_sub;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 1;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__float128)f128_nsub;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 1;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__float128)f128_inf;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 0;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__float128)f128_ninf;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 0;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__float128)f128_nan;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 0;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__float128)f128_nnan;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 0;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__float128)f128_snan;
#ifdef __DEBUG_PRINT__
	print_vfloat128x(" x=  ", x);
#endif
	test = vec_all_isfinitef128 (x);
	expt = 0;
	rc += check_isf128 ("check vec_all_isfinitef128", x, test, expt) ;
#endif
#if 1
  tests_count++;
  x = (__float128)f128_nsnan;
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

  __float128 x;
  long tests_count = 0;
  int test, expt;
  int rc = 0;

  printf ("\ntest_all_isnanf128 f128 -> bool int , ...\n");

#if 1
  tests_count++;
  x = (__float128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnanf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_isnanf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsnan;
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

  __float128 x;
  long tests_count = 0;
  int test, expt;
  int rc = 0;

  printf ("\ntest_all_isinff128 f128 -> bool int , ...\n");

#if 1
  tests_count++;
  x = (__float128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isinff128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isinff128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsnan;
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

  __float128 x;
  long tests_count = 0;
  int test, expt;
  int rc = 0;

  printf ("\ntest_all_isnormalf128 f128 -> bool int , ...\n");

#if 1
  tests_count++;
  x = (__float128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_isnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_isnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsnan;
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

  __float128 x;
  long tests_count = 0;
  int test, expt;
  int rc = 0;
  tcount = 0;

  printf ("\ntest_all_issubnormalf128 f128 -> bool int , ...\n");

#if 1
  tests_count++;
  x = (__float128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_submin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsubmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_issubnormalf128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_issubnormalf128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsnan;
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

  __float128 x;
  long tests_count = 0;
  int test, expt;
  int rc = 0;

  printf ("\ntest_all_iszerof128 f128 -> bool int , ...\n");

#if 1
  tests_count++;
  x = (__float128) f128_zero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nzero;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 1;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_one;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_none;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_max;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmax;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_min;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_sub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsub;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_submin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsubmin;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_inf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_ninf;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nnan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_snan;
#ifdef __DEBUG_PRINT__
  print_vfloat128x(" x=  ", x);
#endif
  test = vec_all_iszerof128 (x);
  expt = 0;
  rc += check_isf128 ("check vec_all_iszerof128", x, test, expt);
#endif
#if 1
  tests_count++;
  x = (__float128) f128_nsnan;
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
test_vec_f128 (void)
{
  int rc = 0;

  printf ("\n%s\n", __FUNCTION__);

  rc += test_const_f128 ();
  rc += test_all_is_f128 ();
  rc += test_vec_bool_f128 ();
  rc += test_vec_f128_f128 ();

//  rc += test_time_f128 ();

  return (rc);
}
