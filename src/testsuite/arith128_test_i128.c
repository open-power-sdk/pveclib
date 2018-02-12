/*
 Copyright [2017] IBM Corporation.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 arith128_test_i128.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Oct 25, 2017
 */

#define __STDC_WANT_DEC_FP__    1

#include <stdint.h>
#include <stdio.h>
#if 0
#include <dfp/fenv.h>
#include <dfp/float.h>
#include <dfp/math.h>
#else
#include <fenv.h>
#include <float.h>
#include <math.h>
#endif

//#define __DEBUG_PRINT__
#include <vec_common_ppc.h>

#include "arith128.h"
#include "arith128_print.h"

#include "arith128_test_i128.h"

extern const vui128_t vtipowof10 [];

int
test_1 (void)
{
  unsigned __int128
  a, b, c;
  unsigned __int128
  i, j, k, l;
  int rc = 0;

  printf ("\ntest_1 __int128\n");
  a = 1UL;
  b = 1UL;
  c = a + b;

  print_int128_sum ("1 + 1", c, a, b);

  i = 1UL;
  j = 1UL;
  k = adduqm (i, j);

  print_int128_sum ("1 + 1", k, i, j);

  printf ("\n");
  a = 1UL;
  b = __UINTMAX_MAX__;
  c = a + b;

  print_int128_sum ("1 + 2E64-1", c, a, b);

  i = 1UL;
  j = __UINTMAX_MAX__;
  k = adduqm (i, j);

  print_int128_sum ("1 + 2E64-1", k, i, j);

  printf ("\n");
  a = 1UL;
  b = __UINT32_MAX__;
  b = b << 64;
  b = b + __UINTMAX_MAX__;
  c = a + b;

  print_int128_sum ("1 + 2E96-1", c, a, b);

  i = 1UL;
  j = __UINT32_MAX__;
  j = j << 64;
  j = j + __UINTMAX_MAX__;
  k = adduqm (i, j);

  print_int128_sum ("1 + 2E96-1", k, i, j);

  printf ("\n");
  a = 1UL;
  b = __UINTMAX_MAX__;
  b = b << 64;
  b = b + __UINTMAX_MAX__;
  c = a + b;

  print_int128_sum ("1 + 2E128-1", c, a, b);

  i = 1UL;
  j = __UINTMAX_MAX__;
  j = j << 64;
  j = j + __UINTMAX_MAX__;
  k = adduqm (i, j);

  print_int128_sum ("1 + 2E128-1", k, i, j);

  printf ("\n");
  l = 1UL;
  i = 1UL;
  j = 1UL;
  k = addeuqm (i, j, l);

  print_int128_carry ("1 + 1 + c=1", k, i, j, l);

  l = 1UL;
  i = 0UL;
  j = __UINTMAX_MAX__;
  k = addeuqm (i, j, l);

  print_int128_carry ("0 + 2E64-1 + c=1", k, i, j, l);

  l = 1UL;
  i = 0UL;
  j = __UINTMAX_MAX__;
  j = j << 64;
  j = j + __UINTMAX_MAX__;
  k = addeuqm (i, j, l);

  print_int128_carry ("0 + 2E128-1 + c=1", k, i, j, l);

  l = 1UL;
  i = 1UL;
  j = __UINTMAX_MAX__;
  j = j << 64;
  j = j + __UINTMAX_MAX__;
  k = addeuqm (i, j, l);

  print_int128_carry ("1 + 2E128-1 + c=1", k, i, j, l);

  return rc;
}

int
test_2 (void)
{
#ifdef __DEBUG_PRINT__
  unsigned __int128 a, b, c;
#endif
  vui32_t i, j, k, l, m;
  vui32_t e, ec;
  int rc = 0;

  printf ("\ntest_2 Vector add __int128\n");

#ifdef __DEBUG_PRINT__
  a = 1UL;
  b = __UINT32_MAX__;
  c = a + b;

  print_int128_sum ("1 + 2E32-1", c, a, b);
#endif
  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  k = (vui32_t) vec_adduqm ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E32-1", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000001, 0x00000000);
  rc += check_vuint128x ("vec_adduqm:", (vui128_t) k, (vui128_t) e);

#ifdef __DEBUG_PRINT__
  a = __UINT32_MAX__;
  b = __UINT32_MAX__;
  c = a + b;

  print_int128_sum ("2E32-1 + 2E32-1", c, a, b);
#endif
  i = (vui32_t )CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  k = (vui32_t) vec_adduqm ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2E32-1 + 2E32-1", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000001, 0xfffffffe);
  rc += check_vuint128x ("vec_adduqm:", (vui128_t) k, (vui128_t) e);

#ifdef __DEBUG_PRINT__
  a = 1UL;
  b = __UINTMAX_MAX__;
  c = a + b;

  print_int128_sum ("1 + 2E64-1", c, a, b);
#endif
  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t )CONST_VINT32_W(0, 0, __UINT32_MAX__, __UINT32_MAX__);
  k = (vui32_t) vec_adduqm ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E64-1", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000001, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_adduqm:", (vui128_t) k, (vui128_t) e);

#ifdef __DEBUG_PRINT__
  a = 1UL;
  b = __UINTMAX_MAX__;
  b = b << 32;
  b = b + __UINT32_MAX__;
  c = a + b;

  print_int128_sum ("1 + 2E96-1", c, a, b);
#endif
  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t )
          CONST_VINT32_W(0, __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__);
  k = (vui32_t) vec_adduqm ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E96-1", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000001, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_adduqm:", (vui128_t) k, (vui128_t) e);

#ifdef __DEBUG_PRINT__
  a = 1UL;
  b = __UINTMAX_MAX__;
  b = b << 64;
  b = b + __UINTMAX_MAX__;
  c = a + b;

  print_int128_sum ("1 + 2E128-1", c, a, b);
#endif
  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  k = (vui32_t) vec_adduqm ((vui128_t) i, (vui128_t) j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("1 + 2E128-1", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_adduqm:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addeuqm ((vui128_t) i, (vui128_t) j, (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_carry ("1 + 2E32-1 + c=1", k, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000001, 0x00000001);
  rc += check_vuint128x ("vec_addeuqm:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addeuqm ((vui128_t) i, (vui128_t) j, (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_carry ("2E32-1 + 2E32-1 + c=1", k, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000001, 0xffffffff);
  rc += check_vuint128x ("vec_addeuqm:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addecuq ((vui128_t) i, (vui128_t) j, (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_carry ("carry 2E32-1 + 2E32-1 + c=1", k, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_addecuq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addeq ((vui128_t*) &m, (vui128_t) i, (vui128_t) j,
                           (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_extend ("extent 2E32-1 + 2E32-1 + c=1", k, m, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000001, 0xffffffff);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint256 ("vec_addeq:", (vui128_t) m, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  k = (vui32_t) vec_addeuqm ((vui128_t) i, (vui128_t) j, (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_carry ("1 + 2E128-1 + c=0", k, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_addeuqm:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  k = (vui32_t) vec_addecuq ((vui128_t) i, (vui128_t) j, (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_carry ("carry 1 + 2E128-1 + c=0", k, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_addecuq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  k = (vui32_t) vec_addeq ((vui128_t*) &m, (vui128_t) i, (vui128_t) j,
                           (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_extend ("extend 1 + 2E128-1 + c=0", k, m, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vint256 ("vec_addeq:", (vui128_t) m, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  j = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addeuqm ((vui128_t) i, (vui128_t) j, (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_carry ("0 + 2E128-1 + c=1", k, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vuint128x ("vec_addeuqm:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  j = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addecuq ((vui128_t) i, (vui128_t) j, (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_carry ("carry 0 + 2E128-1 + c=1", k, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_addecuq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  j = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addeq ((vui128_t*) &m, (vui128_t) i, (vui128_t) j,
                           (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_extend ("extend 0 + 2E128-1 + c=1", k, m, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vint256 ("vec_addeq:", (vui128_t) m, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addeuqm ((vui128_t) i, (vui128_t) j, (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_carry ("1 + 2E128-1 + c=1", k, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_addeuqm:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 2);
  j = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addecuq ((vui128_t) i, (vui128_t) j, (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_carry ("carry 2 + 2E128-1 + c=1", k, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_addecuq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 2);
  j = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  l = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) vec_addeq ((vui128_t*) &m, (vui128_t) i, (vui128_t) j,
                           (vui128_t) l);

#ifdef __DEBUG_PRINT__
  print_vint128x_extend ("extend 2 + 2E128-1 + c=1", k, m, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000002);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vint256 ("vec_addeq:", (vui128_t) m, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  return (rc);
}

int
test_3 (void)
{
  vui32_t i, j, k /*, l, m*/;
  vui32_t e;
  int rc = 0;

  printf ("\ntest_3 Vector BCD +-*/\n");

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x0000001c);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x0000001c);
  k = vec_bcdadd (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (1+1)", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x2c);
  rc += check_vuint128x ("vec_bcdadd:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x9999999c);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x0000001c);
  k = vec_bcdadd (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (9999999+1)", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0, 0, 0x1, 0x0000000c);
  rc += check_vuint128x ("vec_bcdadd:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x0000001c);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x0000001c);
  k = vec_bcdsub (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (1-1)", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x0000000c);
  rc += check_vuint128x ("vec_bcdsub:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x9999999c);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x0000001c);
  k = vec_bcdsub (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (9999999-1)", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x9999998c);
  rc += check_vuint128x ("vec_bcdsub:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x0000001c);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x9999999c);
  k = vec_bcdsub (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (1-9999999)", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x9999998d);
  rc += check_vuint128x ("vec_bcdsub:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x0000001c);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x9999999c);
  k = vec_bcdmul (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (1*9999999)", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x9999999c);
  rc += check_vuint128x ("vec_bcdmul:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x9999999c);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x9999999c);
  k = vec_bcdmul (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (9999999*9999999)", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0, 0, 0x9999998, 0x0000001c);
  rc += check_vuint128x ("vec_bcdmul:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0x99999999, 0x9999999c);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0x99999999, 0x9999999c);
  k = vec_bcdmul (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (999999999999999*999999999999999)", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0x09999999, 0x99999998, 0x00000000, 0x0000001c);
  rc += check_vuint128x ("vec_bcdmul:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0x09999999, 0x99999998, 0x00000000, 0x0000001c);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0x99999999, 0x9999999c);
  k = vec_bcddiv (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (999999999999998000000000000001/999999999999999)", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x99999999, 0x9999999c);
  rc += check_vuint128x ("vec_bcddiv:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0x99999999, 0x9999999c);
  j = (vui32_t )CONST_VINT32_W(0, 0x99999999, 0x99999999, 0x9999999c);
  k = vec_bcdmul (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (999999999999999*99999999999999999999999)", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x0000000c);
  rc += check_vuint128x ("vec_bcdmul:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0x00000001, 0x0000000c);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x3c);
  k = vec_bcddiv (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("bcd (10000000/3)", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x3333333c);
  rc += check_vuint128x ("vec_bcddiv:", (vui128_t) k, (vui128_t) e);

  return rc;
}

int
test_4 (void)
{
  vui32_t i, e /*j, l, m*/;
  vui128_t k;
  int ii;
  int rc = 0;

  printf ("\ntest_4 Vector Multiply by 10\n");

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 10);
  k = vec_mul10uq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("1 * 10 ", k);
  print_vint128x("1 * 10 ", k);
#endif
  rc += check_vuint128 ("vec_mul10uq:", k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 65535);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 655350);
  k = vec_mul10uq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("65535 * 10 ", k);
  print_vint128x("65535 * 10 ", (vui128_t)e);
#endif
  rc += check_vuint128 ("vec_mul10uq:", k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 65536);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 0xa0000U);
  k = vec_mul10uq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("65536 * 10 ", k);
  print_vint128x("65536 * 10 ", (vui128_t)e);
#endif
  rc += check_vuint128 ("vec_mul10uq:", k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 65537);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 0xa000aU);
  k = vec_mul10uq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("65537 * 10 ", k);
  print_vint128x("65537 * 10 ", (vui128_t)e);
#endif
  rc += check_vuint128 ("vec_mul10uq:", k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, __UINT32_MAX__);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0x00000009U, 0xfffffff6U);
  k = vec_mul10uq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("2E32-1 * 10 ", k);
  print_vint128x("2E32-1 * 10 ", (vui128_t)e);
#endif
  rc += check_vuint128 ("vec_mul10uq:", k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 65535, __UINT32_MAX__);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0x9ffffU, 0xfffffff6U);
  k = vec_mul10uq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("2E48-1 * 10 ", k);
  print_vint128x("2E48-1 * 10 ", (vui128_t)e);
#endif
  rc += check_vuint128 ("vec_mul10uq:", k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, __UINT32_MAX__, __UINT32_MAX__);
  e = (vui32_t )CONST_VINT32_W(0, 0x00000009U, __UINT32_MAX__, 0xfffffff6U);
  k = vec_mul10uq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("2E64-1 * 10 ", k);
  print_vint128x("2E64-1 * 10 ", (vui128_t)e);
#endif
  rc += check_vuint128 ("vec_mul10uq:", k, (vui128_t) e);

  i = (vui32_t )
          CONST_VINT32_W(0, __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__);
  e = (vui32_t )
          CONST_VINT32_W(0x00000009U, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffff6U);
  k = vec_mul10uq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("2E96-1 * 10 ", k);
  print_vint128x("2E96-1 * 10 ", (vui128_t)e);
#endif
  rc += check_vuint128 ("vec_mul10uq:", k, (vui128_t) e);

  i = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  e = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffff6U);
  k = vec_mul10uq ((vui128_t) i);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1 * 10 ", k);
  print_vint128x("2E128-1 * 10 ", (vui128_t)e);
#endif
  rc += check_vuint128 ("vec_mul10uq:", k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);

#ifdef __DEBUG_PRINT__
  print_vint128 ("init i=1", (vui128_t)i);
#endif
  for (ii = 1; ii < 39; ii++)
    {
      k = vec_mul10uq ((vui128_t) i);

      rc += check_vuint128 ("vec_mul10uq:", k, vtipowof10[ii]);
#ifdef __DEBUG_PRINT__
      print_vint128 ("x * 10 ", k);
#endif
      i = (vui32_t) k;
    }

  if (rc)
    printf ("\ntest_4 Vector Multiply by 10 %d errors\n", rc);

  return (rc);
}

int
test_4b (void)
{
  vui32_t i, e, em, ec;
  vui128_t j, k, l, m, n;
  int ii;
  int rc = 0;

  printf ("\ntest_4b Vector Multiply by 10 carry/extend\n");
  i = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  e = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffff6);
  ec = (vui32_t )CONST_VINT32_W(0, 0, 0, 9);

  k = vec_mul10uq ((vui128_t) i);
#if 1
  j = vec_mul10cuq ((vui128_t) i);
#else
  j = db_vec_mul10cuq ((vui128_t)i);
#endif

#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1 * 10 ", k);
  print_vint128 ("2E128-1 *10c ", j);
  print_vint128x("2E128-1 * 10 ", k);
  print_vint128x("2E128-1 *10c ", j);
#endif
  rc += check_vint256 ("vec_mul10cuq:", j, k, (vui128_t) ec, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0x19999999, 0x99999999, 0x99999999, 0x99999999);
  e = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffffa);
  ec = (vui32_t )CONST_VINT32_W(0, 0, 0, 0);

  k = vec_mul10uq ((vui128_t) i);
  j = vec_mul10cuq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1 * 10 ", k);
  print_vint128 ("2E128-1 *10c ", j);
  print_vint128x("2E128-1 * 10 ", k);
  print_vint128x("2E128-1 *10c ", j);
#endif
  rc += check_vint256 ("vec_mul10cuq:", j, k, (vui128_t) ec, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0x19999999, 0x99999999, 0x99999999, 0x9999999a);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 4);
  ec = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);

  k = vec_mul10uq ((vui128_t) i);
  j = vec_mul10cuq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1 * 10 ", k);
  print_vint128 ("2E128-1 *10c ", j);
  print_vint128x("2E128-1 * 10 ", k);
  print_vint128x("2E128-1 *10c ", j);
#endif
  rc += check_vint256 ("vec_mul10cuq:", j, k, (vui128_t) ec, (vui128_t) e);

  i = (vui32_t )
          CONST_VINT32_W(0x7fffffff, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  e = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffff6);
  ec = (vui32_t )CONST_VINT32_W(0, 0, 0, 4);

  k = vec_mul10uq ((vui128_t) i);
  j = vec_mul10cuq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1 * 10 ", k);
  print_vint128 ("2E128-1 *10c ", j);
  print_vint128x("2E128-1 * 10 ", k);
  print_vint128x("2E128-1 *10c ", j);
#endif
  rc += check_vint256 ("vec_mul10cuq:", j, k, (vui128_t) ec, (vui128_t) e);

  i = (vui32_t )
          CONST_VINT32_W(0x7fffffff, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  j = (vui128_t) (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  e = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffff6);
  k = vec_mul10euq ((vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
#endif
  rc += check_vuint128 ("vec_mul10euq:", k, (vui128_t) e);

  i = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  j = (vui128_t) (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  e = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffff6);
  k = vec_mul10euq ((vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
#endif
  rc += check_vuint128 ("vec_mul10euq:", k, (vui128_t) e);
  i = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  j = (vui128_t) (vui32_t )CONST_VINT32_W(0, 0, 0, 4);
  e = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffffa);
  k = vec_mul10euq ((vui128_t) i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
#endif
  rc += check_vuint128 ("vec_mul10euq:", k, (vui128_t) e);

  i = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  j = (vui128_t) (vui32_t )CONST_VINT32_W(0, 0, 0, 9);
  e = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  k = vec_mul10euq ((vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
#endif
  rc += check_vuint128 ("vec_mul10euq:", k, (vui128_t) e);

  i = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  j = (vui128_t) (vui32_t ) { 0, 0, 0, 0 };
  e = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffff6);
  ec = (vui32_t )CONST_VINT32_W(0, 0, 0, 9);
  k = vec_mul10euq ((vui128_t) i, j);
  l = vec_mul10ecuq ((vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
  print_vint128 ("2E128-1 *10c ", l);
  print_vint128x("2E128-1 *10c ", l);
#endif
  rc += check_vint256 ("vec_mul10ecuq:", l, k, (vui128_t) ec, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0x19999999, 0x99999999, 0x99999999, 0x99999999);
  j = (vui128_t) (vui32_t ) { 0, 0, 0, 0 };
  e = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffffa);
  ec = (vui32_t )CONST_VINT32_W(0, 0, 0, 0);

  k = vec_mul10euq ((vui128_t) i, j);
  l = vec_mul10ecuq ((vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
  print_vint128 ("2E128-1 *10c ", l);
  print_vint128x("2E128-1 *10c ", l);
#endif
  rc += check_vint256 ("vec_mul10ecuq:", l, k, (vui128_t) ec, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0x19999999, 0x99999999, 0x99999999, 0x99999999);
  j = (vui128_t) ((vui32_t )CONST_VINT32_W(0, 0, 0, 1));
  e = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffffb);
  ec = (vui32_t )CONST_VINT32_W(0, 0, 0, 0);

  k = vec_mul10euq ((vui128_t) i, j);
  l = vec_mul10ecuq ((vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
  print_vint128 ("2E128-1 *10c ", l);
  print_vint128x("2E128-1 *10c ", l);
#endif
  rc += check_vint256 ("vec_mul10ecuq:", l, k, (vui128_t) ec, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0x19999999, 0x99999999, 0x99999999, 0x99999999);
  j = (vui128_t) ((vui32_t )CONST_VINT32_W(0, 0, 0, 6));
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  ec = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);

  k = vec_mul10euq ((vui128_t) i, j);
  l = vec_mul10ecuq ((vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
  print_vint128 ("2E128-1 *10c ", l);
  print_vint128x("2E128-1 *10c ", l);
#endif
  rc += check_vint256 ("vec_mul10ecuq:", l, k, (vui128_t) ec, (vui128_t) e);

  return (rc);
}


/* Needed to split these tests into a separate function to avoid a ICE
   in GCC 6.3 / AT10.  */
int
test_4b1 (void)
{
  vui32_t i, e, em, ec;
  vui128_t j, k, l, m, n;
  int ii;
  int rc = 0;

  i = (vui32_t )CONST_VINT32_W(0x33333333, 0x33333333, 0x33333333, 0x33333333);
  j = (vui128_t) ((vui32_t )CONST_VINT32_W(0, 0, 0, 0));
  e = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffffe);
  ec = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);

  k = vec_mul10euq ((vui128_t) i, j);
  l = vec_mul10ecuq ((vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
  print_vint128 ("2E128-1 *10c ", l);
  print_vint128x("2E128-1 *10c ", l);
#endif
  rc += check_vint256 ("vec_mul10ecuq:", l, k, (vui128_t) ec, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0x33333333, 0x33333333, 0x33333333, 0x33333333);
  j = (vui128_t) ((vui32_t )CONST_VINT32_W(0, 0, 0, 1));
  e = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xffffffff);
  ec = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);

  k = vec_mul10euq ((vui128_t) i, j);
  l = vec_mul10ecuq ((vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
  print_vint128 ("2E128-1 *10c ", l);
  print_vint128x("2E128-1 *10c ", l);
#endif
  rc += check_vint256 ("vec_mul10ecuq:", l, k, (vui128_t) ec, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0x33333333, 0x33333333, 0x33333333, 0x33333333);
  j = (vui128_t) ((vui32_t )CONST_VINT32_W(0, 0, 0, 2));
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  ec = (vui32_t )CONST_VINT32_W(0, 0, 0, 2);

  k = vec_mul10euq ((vui128_t) i, j);
  l = vec_mul10ecuq ((vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
  print_vint128 ("2E128-1 *10c ", l);
  print_vint128x("2E128-1 *10c ", l);
#endif
  rc += check_vint256 ("vec_mul10ecuq:", l, k, (vui128_t) ec, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0x33333333, 0x33333333, 0x33333333, 0x33333333);
  j = (vui128_t) ((vui32_t )CONST_VINT32_W(0, 0, 0, 9));
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 7);
  ec = (vui32_t )CONST_VINT32_W(0, 0, 0, 2);

  k = vec_mul10euq ((vui128_t) i, j);
  l = vec_mul10ecuq ((vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
  print_vint128 ("2E128-1 *10c ", l);
  print_vint128x("2E128-1 *10c ", l);
#endif
  rc += check_vint256 ("vec_mul10ecuq:", l, k, (vui128_t) ec, (vui128_t) e);

  i = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  j = (vui128_t) (vui32_t )CONST_VINT32_W(0, 0, 0, 4);
  e = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         0xfffffffa);
  ec = (vui32_t )CONST_VINT32_W(0, 0, 0, 9);
  k = vec_mul10euq ((vui128_t) i, j);
  l = vec_mul10ecuq ((vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
  print_vint128 ("2E128-1 *10c ", l);
  print_vint128x("2E128-1 *10c ", l);
#endif
  rc += check_vint256 ("vec_mul10ecuq:", l, k, (vui128_t) ec, (vui128_t) e);

  i = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  j = (vui128_t) (vui32_t )CONST_VINT32_W(0, 0, 0, 9);
  e = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  ec = (vui32_t )CONST_VINT32_W(0, 0, 0, 9);
  k = vec_mul10euq ((vui128_t) i, j);
  l = vec_mul10ecuq ((vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*10+e ", k);
  print_vint128x("2E128-1*10+e ", k);
  print_vint128 ("2E128-1 *10c ", l);
  print_vint128x("2E128-1 *10c ", l);
#endif
  rc += check_vint256 ("vec_mul10ecuq:", l, k, (vui128_t) ec, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 10);
  m = (vui128_t) (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  e = (vui32_t )CONST_VINT32_W(0x7775a5f1, 0x71951000, 0x00000000, 0x00000000);
  ec = (vui32_t )CONST_VINT32_W(0x161bcca7, 0x119915b5, 0x0764b4ab, 0xe8652979);
  for (ii = 0; ii < 75; ii++)
    {

      j = vec_mul10cuq ((vui128_t) i);
      k = vec_mul10uq ((vui128_t) i);
      m = vec_mul10euq (m, j);

#ifdef __DEBUG_PRINT__
      print_vint256 ("x *=10 ", m, k);
#endif
      i = (vui32_t) k;
    }
  rc += check_vint256 ("vec_mul10euq:", m, k, (vui128_t) ec, (vui128_t) e);

  n = (vui128_t) (vui32_t ) { 0, 0, 0, 0 };
  e = (vui32_t )CONST_VINT32_W(0xae8a0000, 0x00000000, 0x00000000 , 0x00000000);
  em = (vui32_t )CONST_VINT32_W(0x1c91d1ea, 0xc1fe9754, 0xbd25d537, 0x4e6376ef);
  ec = (vui32_t )CONST_VINT32_W(0x00a65399, 0x30bf6bff, 0x4584db83, 0x46b78615);
  for (ii = 75; ii < 112; ii++)
    {
      j = vec_mul10cuq ((vui128_t) i);
      k = vec_mul10uq ((vui128_t) i);
      l = vec_mul10ecuq (m, j);
      m = vec_mul10euq (m, j);
      n = vec_mul10euq (n, l);

#ifdef __DEBUG_PRINT__
      print_vint384 ("x *=10 ", n, m, k);
#endif

      i = (vui32_t) k;
    }
  rc += check_vint384 ("vec_mul10ecuq:", n, m, k, (vui128_t) ec, (vui128_t) em,
                       (vui128_t) e);

  return (rc);
}

int
test_4c (void)
{
  vui32_t i;
  vui32_t e, em, ec;
  vui128_t j, k, l, m, n;
  int ii;
  int rc = 0;

  printf ("\ntest_4c Vector Multiply by 100\n");
  i = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  k = vec_cmul100cuq (&j, (vui128_t) i);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1 * 100 ", k);
  print_vint128 ("2E128-1 *100c ", j);
#endif
  e = (vui32_t )CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffff9c);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000063);
  rc += check_vint256 ("vec_cmul100cuq:", (vui128_t) j, (vui128_t) k,
                       (vui128_t) ec, (vui128_t) e);

  i = (vui32_t )
          CONST_VINT32_W(__UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
                         __UINT32_MAX__);
  j = (vui128_t) (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  k = vec_cmul100ecuq (&l, (vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*100+e ", k);
  print_vint128 ("2E128-1 *100c ", l);
#endif
  e = (vui32_t )CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffff9c);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000063);
  rc += check_vint256 ("vec_cmul100ecuq:", (vui128_t) l, (vui128_t) k,
                       (vui128_t) ec, (vui128_t) e);

  i = (vui32_t ) { __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
          __UINT32_MAX__ };
  j = (vui128_t) (vui32_t )CONST_VINT32_W(0, 0, 0, 4);
  k = vec_cmul100ecuq (&l, (vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*100+e ", k);
  print_vint128 ("2E128-1 *100c ", l);
#endif
  e = (vui32_t )CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffa0);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000063);
  rc += check_vint256 ("vec_cmul100ecuq:", (vui128_t) l, (vui128_t) k,
                       (vui128_t) ec, (vui128_t) e);

  i = (vui32_t ) { __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
          __UINT32_MAX__ };
  j = (vui128_t) (vui32_t )CONST_VINT32_W(0, 0, 0, 9);
  k = vec_cmul100ecuq (&l, (vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*100+e ", k);
  print_vint128 ("2E128-1 *100c ", l);
#endif
  e = (vui32_t )CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffa5);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000063);
  rc += check_vint256 ("vec_cmul100ecuq:", (vui128_t) l, (vui128_t) k,
                       (vui128_t) ec, (vui128_t) e);

  i = (vui32_t ) { __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
          __UINT32_MAX__ };
  j = (vui128_t) (vui32_t )CONST_VINT32_W(0, 0, 0, 10);
  k = vec_cmul100ecuq (&l, (vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*100+e ", k);
  print_vint128 ("2E128-1 *100c ", l);
#endif
  e = (vui32_t )CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffa6);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000063);
  rc += check_vint256 ("vec_cmul100ecuq:", (vui128_t) l, (vui128_t) k,
                       (vui128_t) ec, (vui128_t) e);

  i = (vui32_t ) { __UINT32_MAX__, __UINT32_MAX__, __UINT32_MAX__,
          __UINT32_MAX__ };
  j = (vui128_t) (vui32_t )CONST_VINT32_W(0, 0, 0, 99);
  k = vec_cmul100ecuq (&l, (vui128_t) i, j);
#ifdef __DEBUG_PRINT__
  print_vint128 ("2E128-1*100+e ", k);
  print_vint128 ("2E128-1 *100c ", l);
#endif
  e = (vui32_t )CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000063);
  rc += check_vint256 ("vec_cmul100ecuq:", (vui128_t) l, (vui128_t) k,
                       (vui128_t) ec, (vui128_t) e);

#if 1
  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 100);
  m = (vui128_t) (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  e = (vui32_t )CONST_VINT32_W(0x7775a5f1, 0x71951000, 0x00000000, 0x00000000);
  ec = (vui32_t )CONST_VINT32_W(0x161bcca7, 0x119915b5, 0x0764b4ab, 0xe8652979);

  for (ii = 0; ii < 37; ii++)
    {
      k = vec_cmul100cuq (&j, (vui128_t) i);
      m = vec_cmul100ecuq (&l, m, j);
#ifdef __DEBUG_PRINT__
      print_vint256 ("x *=100 ", m, k);
#endif
      i = (vui32_t) k;
    }
  rc += check_vint256 ("vec_mul10euq:", m, k, (vui128_t) ec, (vui128_t) e);
#endif

#if 1
  n = (vui128_t) (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  e = (vui32_t )CONST_VINT32_W (0xd1640000, 0000000000, 0x00000000, 0x00000000);
  em = (vui32_t )CONST_VINT32_W(0x1db2332b, 0x93f1e94f, 0x637a5429, 0x0fe2a55c);
  ec = (vui32_t )CONST_VINT32_W(0x067f43fb, 0xe77a37f8, 0xb7309320, 0xc32b3cd3);

  for (ii = 37; ii < 56; ii++)
    {
      vui128_t x;
      k = vec_cmul100cuq (&j, (vui128_t) i);
      m = vec_cmul100ecuq (&l, m, j);
      n = vec_cmul100ecuq (&x, n, l);
#ifdef __DEBUG_PRINT__
      print_vint384 ("x *=10 ", n, m, k);
#endif
      i = (vui32_t) k;
    }
  rc += check_vint384 ("vec_mul10ecuq:", n, m, k, (vui128_t) ec, (vui128_t) em,
                       (vui128_t) e);
#endif
  return (rc);
}

#ifdef __DEBUG_PRINT__
#define test_vec_mulluq(_i, _j)	db_vec_mulluq(_i, _j)
#else
#define test_vec_mulluq(_i, _j)	vec_mulluq(_i, _j)
#endif

int
test_5 (void)
{
  vui32_t i, j, k /*, l, m*/;
  vui32_t e;
  int rc = 0;

  printf ("\ntest_5 vector multiply low __int128\n");

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  k = (vui32_t) test_vec_mulluq((vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128_sum ("1 * 1 ", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 1);
  rc += check_vuint128 ("vec_mulluq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 100);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 100);
  k = (vui32_t) test_vec_mulluq((vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128_sum ("100 * 100 ", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 10000);
  rc += check_vuint128 ("vec_mulluq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x10000);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 0x10000);
  k = (vui32_t) test_vec_mulluq((vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("65536 * 65536 ", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0, 0, 0x00000001, 0x00000000);
  rc += check_vuint128x ("vec_mulluq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0xffffffff);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 0xffffffff);
  k = (vui32_t) test_vec_mulluq((vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2**32-1 * 2**32-1 ", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0, 0, 0xfffffffe, 0x00000001);
  rc += check_vuint128x ("vec_mulluq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0xffff, 0xffffffff);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0xffff, 0xffffffff);
  k = (vui32_t) test_vec_mulluq((vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2**48-1 * 2**48-1 ", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0, 0xffffffff, 0xfffe0000, 0x00000001);
  rc += check_vuint128x ("vec_mulluq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0x1, 0x2);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0x3, 0x5);
  k = (vui32_t) test_vec_mulluq((vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("0X100000002 * 0X300000005 ", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0, 0x00000003, 0x0000000b, 0x0000000a);
  rc += check_vuint128x ("vec_mulluq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0xffffffff, 0xffffffff);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0xffffffff, 0xffffffff);
  k = (vui32_t) test_vec_mulluq((vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2**64-1 * 2**64-1 ", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0xffffffff, 0xfffffffe, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_mulluq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 0xffffffff);
  k = (vui32_t) test_vec_mulluq((vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2**96-1 * 2**32-1 ", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0xfffffffe, 0xffffffff, 0xffffffff, 0x00000001);
  rc += check_vuint128x ("vec_mulluq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0xffffffff);
  j = (vui32_t )CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  k = (vui32_t) test_vec_mulluq((vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2**32-1 * 2**96-1 ", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0xfffffffe, 0xffffffff, 0xffffffff, 0x00000001);
  rc += check_vuint128x ("vec_mulluq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  j = (vui32_t )CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  k = (vui32_t) test_vec_mulluq((vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128x_sum ("2**96-1 * 2**96-1 ", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0xfffffffe, 0x00000000, 0x00000000, 0x00000001);
  rc += check_vuint128x ("vec_mulluq:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 100000000);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 100000000);
  k = (vui32_t) test_vec_mulluq((vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128_sum ("10**8 * 10**8 ", k, i, j);
  print_vint128x_sum ("10**8 * 10**8 ", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0, 0, 0x002386f2, 0x6fc10000);
  rc += check_vuint128x ("vec_mulluq:", (vui128_t) k, (vui128_t) e);

  i = k;
  j = k;
  k = (vui32_t) test_vec_mulluq((vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128_sum ("10**16 * 10**16 ", k, i, j);
  print_vint128x_sum ("10**16 * 10**16 ", k, i, j);
#endif
  e = (vui32_t )CONST_VINT32_W(0x000004ee, 0x2d6d415b, 0x85acef81, 0x00000000);
  rc += check_vuint128x ("vec_mulluq:", (vui128_t) k, (vui128_t) e);

  return (rc);
}

#ifdef __DEBUG_PRINT__
#define test_vec_muludq(_l, _i, _j)	db_vec_muluq(_l, _i, _j)
#else
#define test_vec_muludq(_l, _i, _j)	vec_muludq(_l, _i, _j)
#endif

int
test_6 (void)
{
  vui32_t i, j, k, l /*, m*/;
  vui32_t e, ec;
  int rc = 0;

  printf ("\ntest_6 vector multiply __int128, 256-bit product\n");

  i = (vui32_t )CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  j = (vui32_t )CONST_VINT32_W(0, 0xffffffff, 0xffffffff, 0xffffffff);
  k = (vui32_t) test_vec_muludq((vui128_t* )&l, (vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128_prod ("2**96-1 * 2**96-1 ", k, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0xfffffffe, 0x00000000, 0x00000000, 0x00000001);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0xffffffff, 0xffffffff);
  rc += check_vint256 ("vec_muludq:", (vui128_t) l, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  j = (vui32_t )CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  k = (vui32_t) test_vec_muludq((vui128_t* )&l, (vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128_prod ("2**128-1 * 2**128-1 ", k, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000001);
  ec = (vui32_t )CONST_VINT32_W(0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe);
  rc += check_vint256 ("vec_muludq:", (vui128_t) l, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 100000000);
  j = (vui32_t )CONST_VINT32_W(0, 0, 0, 100000000);
  k = (vui32_t) test_vec_muludq((vui128_t* )&l, (vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128_prod ("10**8 * 10**8 ", k, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W (0x00000000, 0x00000000, 0x002386f2, 0x6fc10000);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint256 ("vec_muludq:", (vui128_t) l, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = k;
  j = k;
  k = (vui32_t) test_vec_muludq((vui128_t* )&l, (vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128_prod ("10**16 * 10**16 ", k, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W (0x000004ee, 0x2d6d415b, 0x85acef81, 0x00000000);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000000, 0x00000000, 0x00000000);
  rc += check_vint256 ("vec_muludq:", (vui128_t) l, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  i = k;
  j = k;
  k = (vui32_t) test_vec_muludq((vui128_t* )&l, (vui128_t )i, (vui128_t )j);

#ifdef __DEBUG_PRINT__
  print_vint128_prod ("10**32 * 10**32 ", k, i, j, l);
#endif
  e = (vui32_t )CONST_VINT32_W (0x6e38ed64, 0xbf6a1f01, 0x00000000, 0x00000000);
  ec = (vui32_t )CONST_VINT32_W(0x00000000, 0x00184f03, 0xe93ff9f4, 0xdaa797ed);
  rc += check_vint256 ("vec_muludq:", (vui128_t) l, (vui128_t) k, (vui128_t) ec,
                       (vui128_t) e);

  return (rc);
}
#ifdef __DEBUG_PRINT__
#define test_vec_clzq(_l)	db_vec_clzq(_l)
#else
#define test_vec_clzq(_l)	vec_clzq(_l)
#endif

int
test_8 (void)
{
  vui32_t i, e /*,  k, l , m*/;
  vui128_t j;
  int rc = 0;

  printf ("\ntest_8 Vector Count Leading Zeros\n");

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 128);
  j = test_vec_clzq((vui128_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0) ", j);
#endif
  rc += check_vuint128x ("vec_revq:", j, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0xffffffff, 0, 0, 0);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 0);
  j = test_vec_clzq((vui128_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0xffffffff,0,0,0) ", j);
#endif
  rc += check_vuint128x ("vec_revq:", j, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0xffffffff, 0, 0);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 32);
  j = test_vec_clzq((vui128_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0xffffffff,0,0) ", j);
#endif
  rc += check_vuint128x ("vec_revq:", j, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0, 0xffffffff);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 96);
  j = test_vec_clzq((vui128_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0,0,0xffffffff) ", j);
#endif
  rc += check_vuint128x ("vec_revq:", j, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0xffffffff, 0);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 64);
  j = test_vec_clzq((vui128_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0,0xffffffff,0) ", j);
#endif
  rc += check_vuint128x ("vec_revq:", j, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0, 0xffff, 0);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 80);
  j = test_vec_clzq((vui128_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0,0xffff,0) ", j);
#endif
  rc += check_vuint128x ("vec_revq:", j, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0xffffffff, 0, 0xffffffff);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 32);
  j = test_vec_clzq((vui128_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0xffffffff,0,0xffffffff) ", j);
#endif
  rc += check_vuint128x ("vec_revq:", j, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0xffff, 0, 0xffffffff);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 48);
  j = test_vec_clzq((vui128_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0xffff,0,0xffffffff) ", j);
#endif
  rc += check_vuint128x ("vec_revq:", j, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 0xf, 0, 0xffffffff);
  e = (vui32_t )CONST_VINT32_W(0, 0, 0, 60);
  j = test_vec_clzq((vui128_t )i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0,0xf,0,0xffffffff) ", j);
#endif
  rc += check_vuint128x ("vec_revq:", j, (vui128_t) e);

  return (rc);
}

int
test_43 (void)
{
  vui32_t i, e /*j, l, m*/;
  vui32_t *ip;
  vui128_t k;
//    vui128_t *kp;
  unsigned char mem[16] __attribute__ ((aligned (16))) = { 0xf0, 0xf1, 0xf2,
      0xf3, 0xe0, 0xe1, 0xe2, 0xe3, 0xd0, 0xd1, 0xd2, 0xd3, 0xc0, 0xc1, 0xc2,
      0xc3 };
  int rc = 0;

  printf ("\ntest_43 vec_revq\n");

  i = (vui32_t )CONST_VINT32_W(0, 1, 2, 3);
  e = (vui32_t )CONST_VINT32_W(0x03000000, 0x02000000, 0x01000000, 0x00000000);
  k = vec_revq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_revq i=", (vui128_t)i);
  print_vint128x ("         k=", k);
#endif
  rc += check_vuint128 ("vec_revq:", k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0x01020304, 0x11121314, 0x21222324, 0x31323334);
  e = (vui32_t )CONST_VINT32_W(0x34333231, 0x24232221, 0x14131211, 0x04030201);
  k = vec_revq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_revq i=", (vui128_t)i);
  print_vint128x ("         k=", k);
#endif
  rc += check_vuint128 ("vec_revq:", k, (vui128_t) e);

  ip = (vui32_t*) mem;
  i = *ip;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  e = (vui32_t )CONST_VINT32_W(0xc3c2c1c0, 0xd3d2d1d0, 0xe3e2e1e0, 0xf3f2f1f0);
#else
  e = (vui32_t)CONST_VINT32_W(0xf0f1f2f3, 0xe0e1e2e3, 0xd0d1d2d3, 0xc0c1c2c3);
#endif
  k = vec_revq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_revq i=", (vui128_t)i);
  print_vint128x ("         k=", k);
#endif
  rc += check_vuint128 ("vec_revq:", k, (vui128_t) e);

  i = (vui32_t ) { 0, 1, 2, 3 };
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  e = (vui32_t )CONST_VINT32_W(0x01000000, 0x00000000, 0x03000000, 0x02000000);
#else
  e = (vui32_t)CONST_VINT32_W(0x02000000, 0x03000000, 0x00000000, 0x01000000);
#endif
  k = (vui128_t) vec_revd ((vui64_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_revd i=", (vui128_t)i);
  print_vint128x ("         k=", k);
#endif
  rc += check_vuint128 ("vec_revd:", k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0x01020304, 0x11121314, 0x21222324, 0x31323334);
  e = (vui32_t )CONST_VINT32_W(0x14131211, 0x04030201, 0x34333231, 0x24232221);
  k = (vui128_t) vec_revd ((vui64_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_revd i=", (vui128_t)i);
  print_vint128x ("         k=", k);
#endif
  rc += check_vuint128 ("vec_revd:", k, (vui128_t) e);

  ip = (vui32_t*) mem;
  i = *ip;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  e = (vui32_t )CONST_VINT32_W(0xe3e2e1e0, 0xf3f2f1f0, 0xc3c2c1c0, 0xd3d2d1d0);
#else
  e = (vui32_t)CONST_VINT32_W(0xd0d1d2d3, 0xc0c1c2c3, 0xf0f1f2f3, 0xe0e1e2e3);
#endif
  k = (vui128_t) vec_revd ((vui64_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_revd i=", (vui128_t)i);
  print_vint128x ("         k=", k);
#endif
  rc += check_vuint128 ("vec_revd:", k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0, 1, 2, 3);
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x01000000, 0x02000000, 0x03000000);
  k = (vui128_t) vec_revw ((vui32_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_revw i=", (vui128_t)i);
  print_vint128x ("         k=", k);
#endif
  rc += check_vuint128 ("vec_revw:", k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0x01020304, 0x11121314, 0x21222324, 0x31323334);
  e = (vui32_t )CONST_VINT32_W(0x04030201, 0x14131211, 0x24232221, 0x34333231);
  k = (vui128_t) vec_revw ((vui32_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_revw i=", (vui128_t)i);
  print_vint128x ("         k=", k);
#endif
  rc += check_vuint128 ("vec_revw:", k, (vui128_t) e);

  ip = (vui32_t*) mem;
  i = *ip;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  e = (vui32_t )CONST_VINT32_W(0xf3f2f1f0, 0xe3e2e1e0, 0xd3d2d1d0, 0xc3c2c1c0);
#else
  e = (vui32_t)CONST_VINT32_W(0xc0c1c2c3, 0xd0d1d2d3, 0xe0e1e2e3, 0xf0f1f2f3);
#endif
  k = (vui128_t) vec_revw ((vui32_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_revw i=", (vui128_t)i);
  print_vint128x ("         k=", k);
#endif
  rc += check_vuint128 ("vec_revw:", k, (vui128_t) e);

  i = (vui32_t ) { 0, 1, 2, 3 };
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  e = (vui32_t )CONST_VINT32_W(0x00000000, 0x00000100, 0x00000200, 0x00000300);
#else
  e = (vui32_t)CONST_VINT32_W(0x00000300, 0x00000200, 0x00000100, 0x00000000);
#endif
  k = (vui128_t) vec_revh ((vui16_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_revh i=", (vui128_t)i);
  print_vint128x ("         k=", k);
#endif
  rc += check_vuint128 ("vec_revh:", k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0x01020304, 0x11121314, 0x21222324, 0x31323334);
  e = (vui32_t )CONST_VINT32_W(0x02010403, 0x12111413, 0x22212423, 0x32313433);
  k = (vui128_t) vec_revh ((vui16_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_revh i=", (vui128_t)i);
  print_vint128x ("         k=", k);
#endif
  rc += check_vuint128 ("vec_revh:", k, (vui128_t) e);

  ip = (vui32_t*) mem;
  i = *ip;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  e = (vui32_t )CONST_VINT32_W(0xf1f0f3f2, 0xe1e0e3e2, 0xd1d0d3d2, 0xc1c0c3c2);
#else
  e = (vui32_t)CONST_VINT32_W(0xc2c3c0c1, 0xd2d3d0d1, 0xe2e3e0e1, 0xf2f3f0f1);
#endif
  k = (vui128_t) vec_revh ((vui16_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_revh i=", (vui128_t)i);
  print_vint128x ("         k=", k);
#endif
  rc += check_vuint128 ("vec_revh:", k, (vui128_t) e);

  return (rc);
}
