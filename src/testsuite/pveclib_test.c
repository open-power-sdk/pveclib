/*
 Copyright (c) [2017] IBM Corporation.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 pveclib_test.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Oct 10, 2017
 */

#define __STDC_WANT_DEC_FP__    1

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <fenv.h>
#include <float.h>
#include <math.h>

//#define __DEBUG_PRINT__
#include <vec_int128_ppc.h>
#include <testsuite/arith128_print.h>
#include <testsuite/arith128_test_i128.h>
#include <testsuite/arith128_test_i64.h>
#include <testsuite/arith128_test_i32.h>
#include <testsuite/arith128_test_i16.h>
#include <testsuite/arith128_test_char.h>
#include <testsuite/arith128_test_bcd.h>
#include <testsuite/arith128_test_f32.h>
#include <testsuite/arith128_test_f64.h>
#include <testsuite/arith128_test_f128.h>

int
main (void)
{
  int rc = EXIT_SUCCESS;

  puts ("Power Vector Library testsuite");

#if 1
  rc += test_vec_char ();
  rc += test_vec_bcd ();
  rc += test_vec_i16 ();
#endif
#if 1
  rc += test_vec_i32 ();
#endif
#if 1
  rc += test_vec_i64 ();
#endif
#if 1
  rc += test_vec_i128 ();
#endif
#if 1
  rc += test_vec_f32 ();
#endif
#if 1
  rc += test_vec_f64 ();
#endif
#if 1
  rc += test_vec_f128 ();
#endif

  if (rc > 0)
    printf ("%d failures reported\n", rc);
  return (rc);
}
