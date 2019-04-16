/*
 Copyright (c) [2018] IBM Corporation.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 vec_perf_i128.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Jun 21, 2018
 */


#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <fenv.h>
#include <float.h>
#include <math.h>

//#define __DEBUG_PRINT__
#include <vec_int128_ppc.h>

//#include "arith128.h"
#include <testsuite/arith128_print.h>
#include <testsuite/vec_perf_i128.h>

extern const vui128_t vtipowof10 [];
static const vui32_t c_zero = CONST_VINT32_W(0, 0, 0, 0);
static const vui32_t c_one = CONST_VINT32_W(0, 0, 0, 1);
static const vui32_t c_two = CONST_VINT32_W(0, 0, 0, 2);
static const vui32_t c_nine = CONST_VINT32_W(0, 0, 0, 9);
static const vui32_t c_ten = CONST_VINT32_W(0, 0, 0, 10);

/* 10^64th as a binary const requiring 256-bits.  */
static const vui32_t ten_64h =
    CONST_VINT32_W(0x00000000, 0x00184f03, 0xe93ff9f4, 0xdaa797ed);
static const vui32_t ten_64l =
    CONST_VINT32_W(0x6e38ed64, 0xbf6a1f01, 0x00000000, 0x00000000);


int timed_mul10uq (void)
{
  vui128_t i, k;
  int ii;
  int rc = 0;

#ifdef __DEBUG_PRINT__
  print_vint128 ("init i=1", (vui128_t)i);
#endif

  i = (vui128_t)c_one;
  for (ii = 1; ii < 39; ii++)
    {
      k = vec_mul10uq ((vui128_t) i);

#ifdef __DEBUG_PRINT__
      rc += check_vuint128 ("vec_mul10uq:", k, vtipowof10[ii]);
      print_vint128 ("x * 10 ", k);
#endif
      i = k;
    }
  rc += check_vuint128 ("vec_mul10uq:", k, vtipowof10[38]);

  return rc;
}

int timed_mul10uq2x (void)
{
  vui128_t i, k;
  vui128_t i2, k2, ks;
  int ii;
  int rc = 0;

#ifdef __DEBUG_PRINT__
  print_vint128 ("init i=1", (vui128_t)i);
#endif

  i = (vui128_t)c_one;
  i2 = (vui128_t)c_two;
  for (ii = 1; ii < 39; ii++)
    {
      k = vec_mul10uq ((vui128_t) i);
      k2 = vec_mul10uq ((vui128_t) i2);

#ifdef __DEBUG_PRINT__
      rc += check_vuint128 ("vec_mul10uq:", k, vtipowof10[ii]);
      print_vint128 ("x * 10 ", k);
#endif
      i = k;
      i2 = k2;
    }
  rc += check_vuint128 ("vec_mul10uq:", k, vtipowof10[38]);
  ks = vec_adduqm (k,k);
  rc += check_vuint128 ("vec_mul10uq2x:", k2, ks);

  return rc;
}

//#define __DEBUG_PRINT__
int timed_cmul10ecuq (void)
{
  vui128_t i, k, j;
  vui128_t i2, k2, ks;
  int ii;
  int rc = 0;

  i = (vui128_t)c_zero;
  i2 = (vui128_t)c_zero;
  for (ii = 0; ii < 64; ii++)
    {
      k = vec_cmul10ecuq (&j, i, (vui128_t)c_nine);
      k2 = vec_mul10euq ((vui128_t) i2, j);
      i = k;
      i2 = k2;
    }
  /* k2|k should be 10^64-1.  */
  ks = vec_adduqm (k, (vui128_t)c_one);
  rc += check_vuint128x ("vec_cmul10ecuq:", ks, (vui128_t) ten_64l);
  rc += check_vuint128x ("vec_mul10euq:", k2, (vui128_t) ten_64h);

  return rc;
}
//#undef __DEBUG_PRINT__
//#define __DEBUG_PRINT__
int timed_mulluq (void)
{
  vui128_t i, k;
  int ii;
  int rc = 0;

#ifdef __DEBUG_PRINT__
  print_vint128 ("init i=1", (vui128_t)i);
#endif

  i = (vui128_t)c_one;
  for (ii = 1; ii < 39; ii++)
    {
      k = vec_mulluq ((vui128_t) i, (vui128_t) c_ten);

#ifdef __DEBUG_PRINT__
      rc += check_vuint128 ("vec_mulluq:", k, vtipowof10[ii]);
      print_vint128 ("x * 10 ", k);
#endif
      i = k;
    }
  rc += check_vuint128 ("vec_mulluq:", k, vtipowof10[38]);

  return rc;
}

#pragma GCC optimize ("no-unroll-loops")
//#define __DEBUG_PRINT__
int
timed_muludq (void)
{
  vui128_t i, j, k;
  int ii, iii;
  int rc = 0;

  i = (vui128_t) c_ten;
  iii = 2;
#ifdef __DEBUG_PRINT__
  print_vint128 ("init i=10", (vui128_t) i);
#endif

  for (ii = 1; ii < 7; ii++)
    {
      k = vec_muludq (&j, (vui128_t) i, (vui128_t) i);

#ifdef __DEBUG_PRINT__
      if (ii < 6)
	{
	  printf ("iii=%d ", iii);
	  print_vint128 ("10Eiii=", k);
	  rc += check_vuint128 ("vec_muludq:", k, vtipowof10[iii]);
	}
#endif
      i = k;
      iii = iii + iii;
    }
  rc += check_vuint128x ("vec_muludqx k:", k, (vui128_t) ten_64l);
  rc += check_vuint128x ("vec_muludqx j:", j, (vui128_t) ten_64h);

  return rc;
}

//#define __DEBUG_PRINT__
/* Older (GCC6) compilers ignore the "no-unroll-loops" pragma and make
   it hard to isolate each iteration of vec_muludq.  This version
   calls __test_muludq from vec_int128_dummy.c as a work around.  */
extern vui128_t
__test_muludq (vui128_t *mh, vui128_t a, vui128_t b);

int
timed_muludqx (void)
{
  vui128_t i, j, k;
  int ii, iii;
  int rc = 0;

  i = (vui128_t) c_ten;
  iii = 2;
#ifdef __DEBUG_PRINT__
  print_vint128 ("init i=10", (vui128_t) i);
#endif

  for (ii = 1; ii < 7; ii++)
    {
      k = __test_muludq (&j, (vui128_t) i, (vui128_t) i);

#ifdef __DEBUG_PRINT__
      if (ii < 6)
	{
	  printf ("iii=%d ", iii);
	  print_vint128 ("10Eiii=", k);
	  rc += check_vuint128 ("vec_muludq:", k, vtipowof10[iii]);
	}
#endif
      i = k;
      iii = iii + iii;
    }
  rc += check_vuint128x ("vec_muludqx k:", k, (vui128_t) ten_64l);
  rc += check_vuint128x ("vec_muludqx j:", j, (vui128_t) ten_64h);

  return rc;
}

//#define __DEBUG_PRINT__ 1
extern vui128_t
example_longdiv_10e32 (vui128_t *q, vui128_t *d, long int _N);
#ifdef __DEBUG_PRINT__
#define test_example_longdiv_10e32(_i, _j, _k)	example_longdiv_10e32(_i, _j, _k)
#else
#define test_example_longdiv_10e32(_i, _j, _k)	example_longdiv_10e32(_i, _j, _k)
#endif

int
timed_longdiv_e32 (void)
{
#ifdef __DEBUG_PRINT__
  /* ten32  = +100000000000000000000000000000000UQ  */
  const vui128_t ten32 = (vui128_t)
	  { (__int128) 10000000000000000UL * (__int128) 10000000000000000UL };
#endif
  vui128_t ix[4], qx[4], qy[4], qz[4], qw[4];
  vui128_t r[5];
  vui128_t el, er;
  vui128_t rm;
  int rc = 0;

#ifdef __DEBUG_PRINT__
  printf ("\ntimed Vector long divide Unsigned Quadword\n");
  print_vint128x (" 10**32 ", ten32);
  printf ("\ntest Vector long divide Unsigned Quadword x 4\n");
#endif


  ix[0] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  ix[1] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  ix[2] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  ix[3] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);

  rm = test_example_longdiv_10e32 (qx, ix, 4UL);
  r[4] = rm;

#ifdef __DEBUG_PRINT__
  print_vint128x (" longdiv32 ", (vui128_t) qx[0]);
  print_vint128x ("           ", (vui128_t) qx[1]);
  print_vint128x ("           ", (vui128_t) qx[2]);
  print_vint128x ("           ", (vui128_t) qx[3]);
  print_vint128x ("       rm= ", (vui128_t) rm);
  print_vint128  ("       rm=", (vui128_t) rm);
#endif


  rm = test_example_longdiv_10e32 (qy, qx, 4UL);
  r[3] = rm;

#ifdef __DEBUG_PRINT__
  print_vint128x (" longdiv32 ", (vui128_t) qy[0]);
  print_vint128x ("           ", (vui128_t) qy[1]);
  print_vint128x ("           ", (vui128_t) qy[2]);
  print_vint128x ("           ", (vui128_t) qy[3]);
  print_vint128x ("       rm= ", (vui128_t) rm);
  print_vint128  ("       rm=", (vui128_t) rm);
#endif

  rm = test_example_longdiv_10e32 (qz, qy, 4UL);
  r[2] = rm;

#ifdef __DEBUG_PRINT__
  print_vint128x (" longdiv32 ", (vui128_t) qz[0]);
  print_vint128x ("           ", (vui128_t) qz[1]);
  print_vint128x ("           ", (vui128_t) qz[2]);
  print_vint128x ("           ", (vui128_t) qz[3]);
  print_vint128x ("       rm= ", (vui128_t) rm);
  print_vint128  ("       rm=", (vui128_t) rm);
#endif

  rm = test_example_longdiv_10e32 (qw, qz, 4UL);
  r[1] = rm;
  r[0] = qw[3];

#ifdef __DEBUG_PRINT__
  print_vint128x (" longdiv32 ", (vui128_t) qw[0]);
  print_vint128x ("           ", (vui128_t) qw[1]);
  print_vint128x ("           ", (vui128_t) qw[2]);
  print_vint128x ("           ", (vui128_t) qw[3]);
  print_vint128x ("       rm= ", (vui128_t) rm);
  print_vint128  ("       rm=", (vui128_t) rm);
#endif
  er = (vui128_t)CONST_VINT128_DW128(0x000002A818C1645CUL,
				     0xF38EEB06FFFFFFFFUL);
  rc += check_vuint128x ("modq_2e512r:", (vui128_t)r[4], (vui128_t) er);

  er = (vui128_t)CONST_VINT128_DW128(0x00000406CEE1CCBEUL,
				     0x6C331B4910F46179UL);
  rc += check_vuint128x ("modq_2e512r:", (vui128_t)r[3], (vui128_t) er);

  er = (vui128_t)CONST_VINT128_DW128(0x000002C4A4402B14UL,
				     0x8ADF2CB703147025UL);
  rc += check_vuint128x ("modq_2e512r:", (vui128_t)r[2], (vui128_t) er);

  er = (vui128_t)CONST_VINT128_DW128(0x000004D78833F4BDUL,
				     0x83B9B4EFF89D57BBUL);
  el = (vui128_t)CONST_VINT128_DW128(0x00000000006EE823UL,
				     0x3E325E7250563E59UL);
  rc += check_vuint128x ("modq_2e512r:", (vui128_t)r[1], (vui128_t) er);
  rc += check_vuint128x ("   q_2e512r:", (vui128_t)r[0], (vui128_t) el);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
extern vBCD_t
example_longbcdcf_10e32 (vui128_t *q, vui128_t *d, long int _N);
#ifdef __DEBUG_PRINT__
#define test_example_longbcdcf_10e32(_i, _j, _k)	example_longbcdcf_10e32(_i, _j, _k)
#else
#define test_example_longbcdcf_10e32(_i, _j, _k)	example_longbcdcf_10e32(_i, _j, _k)
#endif
int
timed_longbcdcf_10e32 (void)
{
#ifdef __DEBUG_PRINT__
  /* ten32  = +100000000000000000000000000000000UQ  */
  const vui128_t ten32 = (vui128_t)
	  { (__int128) 10000000000000000UL * (__int128) 10000000000000000UL };
#endif
  vui128_t ix[8], q1[8], q2[8], q3[8], q4[8], q5[8], q6[8], q7[8], q8[8], q9[8];
  vBCD_t el, er;
  vBCD_t rm, bcd[10];
  int rc = 0;

#ifdef __DEBUG_PRINT__
  printf ("\ntest Vector long divide bcdcf Quadword x 8\n");
  print_vint128x (" 10**32 ", ten32);
#endif
  // Fixed-point equivalent of __DBL_MAX__
  // 1.797693134862315e+308Le+308L
  ix[0] = (vui128_t)CONST_VINT128_DW128(0xFFFFFFFFFFFFFFFFUL,
					0xFFFFFFFFFFFFF800UL);
  ix[1] = (vui128_t) { (__int128) 0UL };
  ix[2] = (vui128_t) { (__int128) 0UL };
  ix[3] = (vui128_t) { (__int128) 0UL };
  ix[4] = (vui128_t) { (__int128) 0UL };
  ix[5] = (vui128_t) { (__int128) 0UL };
  ix[6] = (vui128_t) { (__int128) 0UL };
  ix[7] = (vui128_t) { (__int128) 0UL };

  rm = test_example_longbcdcf_10e32 (q1, ix, 8UL);
  bcd[9]= rm;

#ifdef __DEBUG_PRINT__
  print_vint128x (" longdiv32 ", (vui128_t) q1[0]);
  print_vint128x ("           ", (vui128_t) q1[1]);
  print_vint128x ("           ", (vui128_t) q1[2]);
  print_vint128x ("           ", (vui128_t) q1[3]);
  print_vint128x ("       rm= ", (vui128_t) rm);
#endif
  er = (vBCD_t)CONST_VINT128_DW128(0x6358901245426034UL,
				     0x2651056803545088UL);
  rc += check_vuint128x ("modq_2e1024r:", (vui128_t)bcd[9], (vui128_t) er);

  er = (vBCD_t)CONST_VINT128_DW128(0x3385468188567129UL,
				   0x3878570989103972UL);

  rm = test_example_longbcdcf_10e32 (q2, q1, 8UL);
  bcd[8]= rm;

#ifdef __DEBUG_PRINT__
  print_vint128x (" longdiv32 ", (vui128_t) q2[1]);
  print_vint128x ("           ", (vui128_t) q2[2]);
  print_vint128x ("           ", (vui128_t) q2[3]);
  print_vint128x ("           ", (vui128_t) q2[4]);
  print_vint128x ("       rm= ", (vui128_t) rm);
#endif

  er = (vBCD_t)CONST_VINT128_DW128(0x3385468188567129UL,
				   0x3878570989103972UL);
  rc += check_vuint128x ("modq_2e1024r:", (vui128_t)bcd[8], (vui128_t) er);

  rm = test_example_longbcdcf_10e32 (q3, q2, 8UL);
  bcd[7]= rm;

#ifdef __DEBUG_PRINT__
  print_vint128x (" longdiv32 ", (vui128_t) q3[2]);
  print_vint128x ("           ", (vui128_t) q3[3]);
  print_vint128x ("           ", (vui128_t) q3[4]);
  print_vint128x ("           ", (vui128_t) q3[5]);
  print_vint128x ("       rm= ", (vui128_t) rm);
#endif

  er = (vBCD_t)CONST_VINT128_DW128(0x2760024810211116UL,
				     0x0488028411452431UL);
  rc += check_vuint128x ("modq_2e1024r:", (vui128_t)bcd[7], (vui128_t) er);

  rm = test_example_longbcdcf_10e32 (q4, q3, 8UL);
  bcd[6]= rm;

#ifdef __DEBUG_PRINT__
  print_vint128x (" longdiv32 ", (vui128_t) q4[3]);
  print_vint128x ("           ", (vui128_t) q4[4]);
  print_vint128x ("           ", (vui128_t) q4[5]);
  print_vint128x ("           ", (vui128_t) q4[6]);
  print_vint128x ("       rm= ", (vui128_t) rm);
#endif
  er = (vBCD_t)CONST_VINT128_DW128(0x9170914584061107UL,
				   0x7154122122593391UL);
  rc += check_vuint128x ("modq_2e1024r:", (vui128_t)bcd[6], (vui128_t) er);

  rm = test_example_longbcdcf_10e32 (q5, q4, 8UL);
  bcd[5]= rm;

#ifdef __DEBUG_PRINT__
  print_vint128x (" longdiv32 ", (vui128_t) q5[4]);
  print_vint128x ("           ", (vui128_t) q5[5]);
  print_vint128x ("           ", (vui128_t) q5[6]);
  print_vint128x ("           ", (vui128_t) q5[7]);
  print_vint128x ("       rm= ", (vui128_t) rm);
#endif
  er = (vBCD_t)CONST_VINT128_DW128(0x8874590026720091UL,
				   0x5633655049871983UL);
  rc += check_vuint128x ("modq_2e1024r:", (vui128_t)bcd[5], (vui128_t) er);

  rm = test_example_longbcdcf_10e32 (q6, q5, 8UL);
  bcd[4]= rm;

#ifdef __DEBUG_PRINT__
  print_vint128x (" longdiv32 ", (vui128_t) q6[4]);
  print_vint128x ("           ", (vui128_t) q6[5]);
  print_vint128x ("           ", (vui128_t) q6[6]);
  print_vint128x ("           ", (vui128_t) q6[7]);
  print_vint128x ("       rm= ", (vui128_t) rm);
#endif
  er = (vBCD_t)CONST_VINT128_DW128(0x1540522497108924UL,
				   0x9581419968648444UL);
  rc += check_vuint128x ("modq_2e1024r:", (vui128_t)bcd[4], (vui128_t) er);

  rm = test_example_longbcdcf_10e32 (q7, q6, 8UL);
  bcd[3]= rm;

#ifdef __DEBUG_PRINT__
  print_vint128x (" longdiv32 ", (vui128_t) q7[4]);
  print_vint128x ("           ", (vui128_t) q7[5]);
  print_vint128x ("           ", (vui128_t) q7[6]);
  print_vint128x ("           ", (vui128_t) q7[7]);
  print_vint128x ("       rm= ", (vui128_t) rm);
#endif
  er = (vBCD_t)CONST_VINT128_DW128(0x3160000434400393UL,
				   0x4491961443698881UL);
  rc += check_vuint128x ("modq_2e1024r:", (vui128_t)bcd[3], (vui128_t) er);


  rm = test_example_longbcdcf_10e32 (q8, q7, 8UL);
  bcd[2]= rm;

#ifdef __DEBUG_PRINT__
  print_vint128x (" longdiv32 ", (vui128_t) q8[4]);
  print_vint128x ("           ", (vui128_t) q8[5]);
  print_vint128x ("           ", (vui128_t) q8[6]);
  print_vint128x ("           ", (vui128_t) q8[7]);
  print_vint128x ("       rm= ", (vui128_t) rm);
#endif
  er = (vBCD_t)CONST_VINT128_DW128(0x2743838201648706UL,
				   0x6656549016394107UL);
  rc += check_vuint128x ("modq_2e1024r:", (vui128_t)bcd[2], (vui128_t) er);

  rm = test_example_longbcdcf_10e32 (q9, q8, 8UL);
  bcd[1]= rm;

#ifdef __DEBUG_PRINT__
  print_vint128x (" longdiv32 ", (vui128_t) q9[4]);
  print_vint128x ("           ", (vui128_t) q9[5]);
  print_vint128x ("           ", (vui128_t) q9[6]);
  print_vint128x ("           ", (vui128_t) q9[7]);
  print_vint128x ("       rm= ", (vui128_t) rm);
#endif
  bcd[0] = vec_bcdcfuq (q9[7]);

  er = (vBCD_t)CONST_VINT128_DW128(0x9305190789024722UL,
				   0x7985049812838823UL);
  rc += check_vuint128x ("modq_2e1024r:", (vui128_t)bcd[1], (vui128_t) er);

  el = (vBCD_t)CONST_VINT128_DW128(0x0000000000017976UL,
				   0x9313486231590772UL);
  rc += check_vuint128x ("   q_2e1024r:", (vui128_t)bcd[0], (vui128_t) el);

#ifdef __DEBUG_PRINT__
  print_vint128x (" bcdcfuqX= ", (vui128_t) bcd[0]);
  print_vint128x ("           ", (vui128_t) bcd[1]);
  print_vint128x ("           ", (vui128_t) bcd[2]);
  print_vint128x ("           ", (vui128_t) bcd[3]);
  print_vint128x ("           ", (vui128_t) bcd[4]);
  print_vint128x ("           ", (vui128_t) bcd[5]);
  print_vint128x ("           ", (vui128_t) bcd[6]);
  print_vint128x ("           ", (vui128_t) bcd[7]);
  print_vint128x ("           ", (vui128_t) bcd[8]);
  print_vint128x ("           ", (vui128_t) bcd[9]);
#endif

  return (rc);
}

int
timed_maxdouble_10e32 (void)
{
  char buf[64];
  char cmp[64] = "1.79769313486231570814527423731704e+308\n";
  double max_double = __DBL_MAX__;
  int rc = 0;

  sprintf (buf, "%36.32e\n", max_double);

  if (strcmp (cmp, buf))
    {
      printf ("sprintf fail=%s\n", buf);
      rc++;
    }

  return rc;
}
