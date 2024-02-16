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
#ifndef PVECLIB_DISABLE_F128MATH
/* Disable for __clang__ because of bug involving <floatn.h>
   incombination with -mcpu=power9 -mfloat128 */
#include <stdlib.h>
#endif

//#define __DEBUG_PRINT__
#include <pveclib/vec_int128_ppc.h>

#include <testsuite/arith128_print.h>
#include <testsuite/arith128_test_i128.h>
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

// 10**152-1
static const vui128_t c152[] = {
    CONST_VUINT128_QxW (0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff),
    CONST_VUINT128_QxW (0x3e21f795, 0x4fe4a741, 0xd3ad0eeb, 0xa0ffffff),
    CONST_VUINT128_QxW (0xd2d8af57, 0xd5d929cb, 0x5f1e32bf, 0xfbdc5d1c),
    CONST_VUINT128_QxW (0x01e8ca31, 0x85deb719, 0xa2fd64b0, 0xccbf84ba)};

#if 0
extern vui128_t test_vec_moduq (vui128_t y, vui128_t z);
extern vui128_t test_vec_modduq (vui128_t x, vui128_t y, vui128_t z);
extern __VEC_U_128P test_vec_moddivduq (vui128_t x, vui128_t y, vui128_t z);
#define test_divdqu	test_vec_moddivduq
#define test_moduq	test_vec_moduq
#define test_modduq	test_vec_modduq
#else
extern vui128_t test_moduq (vui128_t y, vui128_t z);
extern vui128_t test_modduq (vui128_t x, vui128_t y, vui128_t z);
extern __VEC_U_128P test_divdqu (vui128_t x, vui128_t y, vui128_t z);
#endif
// Reduce a 150+ digit (4xQW) value into 5x 30-digit values that can be printed
int
timed_vec_divdqu ()
{
__VEC_U_128P mq;
vui128_t ix[4], qx[4];
#ifdef __DEBUG_PRINT__
vui128_t rx[5];
#endif
const vui128_t *dx = c152;
// 10**30
vui128_t c30 = (vui128_t) CONST_VINT128_W (0x0000000c, 0x9f2c9cd0, 0x4674edea, 0x40000000);

vui128_t zero = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
vui128_t er, eq;
int rc = 0;

ix[0] = zero;
ix[1] = dx[3];
ix[2] = c30;

mq = test_divdqu (ix[0], ix[1], ix[2]);

// record 1st (QW) quotient digit
qx[3] = mq.vx0;
// Remainder and next (QW) digit
ix[0] = mq.vx1;
ix[1] = dx[2];

mq = test_divdqu (ix[0], ix[1], ix[2]);
// record 2st (QW) quotient digit
qx[2] = mq.vx0;
// Remainder and next (QW) digit
ix[0] = mq.vx1;
ix[1] = dx[1];

mq = test_divdqu (ix[0], ix[1], ix[2]);
// record 3rd (QW) quotient digit
qx[1] = mq.vx0;
// Remainder and next (QW) digit
ix[0] = mq.vx1;
ix[1] = dx[0];

mq = test_divdqu (ix[0], ix[1], ix[2]);

// record 4th (QW) quotient digit
qx[0] = mq.vx0;
#ifdef __DEBUG_PRINT__
// record 1th (QW) remainder digit
rx[0] = mq.vx1;
#endif
// Remainder and next (QW) digit
ix[0] = qx[3];
ix[1] = qx[2];

mq = test_divdqu (ix[0], ix[1], ix[2]);

// record 1st (QW) quotient digit
qx[3] =(vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
qx[2] = mq.vx0;
// Remainder and next (QW) digit
ix[0] = mq.vx1;
ix[1] = qx[1];

mq = test_divdqu (ix[0], ix[1], ix[2]);

qx[1] = mq.vx0;
// Remainder and next (QW) digit
ix[0] = mq.vx1;
ix[1] = qx[0];

mq = test_divdqu (ix[0], ix[1], ix[2]);

// record 4th (QW) quotient digit
qx[0] = mq.vx0;
#ifdef __DEBUG_PRINT__
// record 2th (QW) remainder digit
rx[1] = mq.vx1;
#endif
// Remainder and next (QW) digit
ix[0] = qx[2];
ix[1] = qx[1];

mq = test_divdqu (ix[0], ix[1], ix[2]);

// record 2st (QW) quotient digit
qx[2] =(vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
qx[1] = mq.vx0;
// Remainder and next (QW) digit
ix[0] = mq.vx1;
ix[1] = qx[0];

mq = test_divdqu (ix[0], ix[1], ix[2]);

// record 4th (QW) quotient digit
qx[0] = mq.vx0;

#ifdef __DEBUG_PRINT__
// record 3th (QW) remainder digit
rx[2] = mq.vx1;
#endif
// Remainder and next (QW) digit
ix[0] = qx[1];
ix[1] = qx[0];

mq = test_divdqu (ix[0], ix[1], ix[2]);

// record 3rd (QW) quotient digit
qx[1] =(vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000000);
// record 4th (QW) quotient digit
qx[0] = mq.vx0;
#ifdef __DEBUG_PRINT__
// record 4th (QW) remainder digit
rx[3] = mq.vx1;
#endif

// Remainder and next (QW) digit
ix[0] = qx[1];
ix[1] = qx[0];

mq = test_divdqu (ix[0], ix[1], ix[2]);

// record 5th (QW) quotient digit
qx[0] = mq.vx0;
#ifdef __DEBUG_PRINT__
// record 5th (QW) remainder digit
rx[4] = mq.vx1;
#endif
// 10**30-1
er = (vui128_t) CONST_VINT128_W (0x0000000c, 0x9f2c9cd0, 0x4674edea, 0x3fffffff);
eq = (vui128_t) CONST_VINT128_W (0x00000000, 0x00000000, 0x00000000, 0x00000063);

rc += check_vuint128x ("moddiv k er:", (vui128_t)mq.vx1, (vui128_t) er);
rc += check_vuint128x ("moddiv k eq:", (vui128_t)mq.vx0, (vui128_t) eq);

#ifdef __DEBUG_PRINT__
print_vint128x ("      Q ", (vui128_t) qx[3]);
print_vint128x ("        ", (vui128_t) qx[2]);
print_vint128x ("        ", (vui128_t) qx[1]);
print_vint128x ("        ", (vui128_t) qx[0]);
print_vint128x ("      R ", (vui128_t) rx[4]);
print_vint128x ("        ", (vui128_t) rx[3]);
print_vint128x ("        ", (vui128_t) rx[2]);
print_vint128x ("        ", (vui128_t) rx[1]);
print_vint128x ("        ", (vui128_t) rx[0]);
#endif

return (rc);
}


#if 0
// extern vui128_t test_vec_divduq (vui128_t x, vui128_t y, vui128_t z);
// #define test_divduq	test_vec_divduq
extern vui128_t test_vec_divuqe (vui128_t x, vui128_t z);
#define test_divuqe	test_vec_divuqe
extern vui128_t test_vec_divuq (vui128_t y, vui128_t z);
#define test_divuq	test_vec_divuq
#else
// extern vui128_t test_divduq (vui128_t x, vui128_t y, vui128_t z);
extern vui128_t test_divuqe (vui128_t x, vui128_t z);
extern vui128_t test_divuq (vui128_t y, vui128_t z);
#endif
int
timed_vec_divuqe (void)
{

  vui128_t ix[4];
  vui128_t rnd = (vui128_t)CONST_VINT128_DW128(0UL, 0x80UL);
  vui128_t es;
  vui128_t rq;
  int rc = 0;

  //Taylor series to compute e as a scaled fix-point value
  // This use case requires a 256-bit / 128-bit divide.
  ix[0] = (vui128_t)CONST_VINT128_DW128(0x0001000000000000UL, 0UL);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0x0100000000000000UL, 0UL);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0x0100000000000000UL, 0UL);
  ix[3] = (vui128_t)CONST_VINT128_DW128(0x0100000000000000UL, 0x0UL);

  rq = test_divuqe (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  // round and normalize
  rq    = vec_adduqm (rq, rnd);
  ix[0] = vec_srqi (rq, 8);
  ix[2] = vec_adduqm (ix[2], ix[1]);

  rq = test_divuqe (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  // round and normalize
  rq    = vec_adduqm (rq, rnd);
  ix[0] = vec_srqi (rq, 8);
  ix[2] = vec_adduqm (ix[2], ix[1]);

  rq = test_divuqe (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  // round and normalize
  //Taylor series to compute e
  rq    = vec_adduqm (rq, rnd);
  ix[0] = vec_srqi (rq, 8);
  ix[2] = vec_adduqm (ix[2], ix[1]);

  rq = test_divuqe (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  // round and normalize
  rq    = vec_adduqm (rq, rnd);
  ix[0] = vec_srqi (rq, 8);
  ix[2] = vec_adduqm (ix[2], ix[1]);

  rq = test_divuqe (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  // round and normalize
  rq    = vec_adduqm (rq, rnd);
  ix[0] = vec_srqi (rq, 8);
  ix[2] = vec_adduqm (ix[2], ix[1]);

  rq = test_divuqe (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  // round and normalize
  rq    = vec_adduqm (rq, rnd);
  ix[0] = vec_srqi (rq, 8);
  ix[2] = vec_adduqm (ix[2], ix[1]);

  rq = test_divuqe (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  // round and normalize
  rq    = vec_adduqm (rq, rnd);
  ix[0] = vec_srqi (rq, 8);
  ix[2] = vec_adduqm (ix[2], ix[1]);

  rq = test_divuqe (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  // round and normalize
  rq    = vec_adduqm (rq, rnd);
  ix[0] = vec_srqi (rq, 8);
  ix[2] = vec_adduqm (ix[2], ix[1]);

  rq = test_divuqe (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  // round and normalize
  rq    = vec_adduqm (rq, rnd);
  ix[0] = vec_srqi (rq, 8);
  ix[2] = vec_adduqm (ix[2], ix[1]);

  rq = test_divuqe (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[3], rq); // last term
  es = (vui128_t)CONST_VINT128_DW128(0x02b7e150ed3c569dUL, 0x975c4df261fe4d9aUL);
  rc += check_vuint128x ("divuqe-sum :", (vui128_t)ix[3], (vui128_t) es);

  return (rc);
}

int
timed_vec_divuq (void)
{
  //Taylor series to compute e
  // This use case requires a 128-bit / 64-bit divide

  vui128_t ix[4];
  vui128_t es;
  vui128_t rq;
  int rc = 0;

  ix[0] = (vui128_t)CONST_VINT128_DW128(0x0100000000000000UL, 0UL);
  ix[1] = (vui128_t)CONST_VINT128_DW128(0UL, 1UL);
  ix[2] = (vui128_t)CONST_VINT128_DW128(0UL, 1UL);
  ix[3] = (vui128_t)CONST_VINT128_DW128(0x0100000000000000UL, 0x0UL);

  rq = test_divuq (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  ix[0] = rq;
  ix[2] = vec_adduqm (ix[2], ix[1]);

  rq = test_divuq (ix[0], ix[2]);
  ix[3] = vec_adduqm (ix[3], rq); // next term
  ix[0] = rq;
  ix[2] = vec_adduqm (ix[2], ix[1]);

  rq = test_divuq (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  ix[0] = rq;
  ix[2] = vec_adduqm (ix[2], ix[1]);

  rq = test_divuq (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  ix[0] = rq;
  ix[2] = vec_adduqm (ix[2], ix[1]);

  rq = test_divuq (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  ix[0] = rq;
  ix[2] = vec_adduqm (ix[2], ix[1]);

  rq = test_divuq (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  ix[0] = rq;
  ix[2] = vec_adduqm (ix[2], ix[1]);

  rq = test_divuq (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  ix[0] = rq;
  ix[2] = vec_adduqm (ix[2], ix[1]);

  rq = test_divuq (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  ix[0] = rq;
  ix[2] = vec_adduqm (ix[2], ix[1]);

  rq = test_divuq (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[3], rq); // next term
  ix[0] = rq;
  ix[2] = vec_adduqm (ix[2], ix[1]);

  rq = test_divuq (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[3], rq); // last term
  es = (vui128_t)CONST_VINT128_DW128(0x02b7e150ed3c569dUL, 0x975c4df261fe4d64UL);
  rc += check_vuint128x ("divuq-sum :", (vui128_t)ix[3], (vui128_t) es);

#ifdef __DEBUG_PRINT__
  print_vint128x ("   'sum ", (vui128_t) ix[3]);
#endif

  return (rc);
}

int
timed_vec_divuq2 (void)
{
  // Newton's method sqr
  // This use case requires a full 128-bit / 128-bit divide.
  vui128_t ix[4];
  vui128_t es;
  vui128_t rq;
  int rc = 0;

  ix[0] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  ix[1] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);

  // initial estimate of g0
  ix[2] = vec_srqi (ix[1], 56);
  // a / g0
  rq = test_divuq (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[2], rq);
  ix[2] = vec_srqi (ix[3], 1);

  // a / gn
  rq = test_divuq (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[2], rq);
  ix[2] = vec_srqi (ix[3], 1);

  // a / gn
  rq = test_divuq (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[2], rq);
  ix[2] = vec_srqi (ix[3], 1);

  // a / gn
  rq = test_divuq (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[2], rq);
  ix[2] = vec_srqi (ix[3], 1);

  // a / gn
  rq = test_divuq (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[2], rq);
  ix[2] = vec_srqi (ix[3], 1);

  // a / gn
  rq = test_divuq (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[2], rq);
  ix[2] = vec_srqi (ix[3], 1);

  // a / gn
  rq = test_divuq (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[2], rq);
  ix[2] = vec_srqi (ix[3], 1);

  // a / gn
  rq = test_divuq (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[2], rq);
  ix[2] = vec_srqi (ix[3], 1);

  // a / gn
  rq = test_divuq (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[2], rq);
  ix[2] = vec_srqi (ix[3], 1);

  // a / gn
  rq = test_divuq (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[2], rq);
  ix[2] = vec_srqi (ix[3], 1);

  // a / gn
  rq = test_divuq (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[2], rq);
  ix[2] = vec_srqi (ix[3], 1);

  // a / gn
  rq = test_divuq (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[2], rq);
  ix[2] = vec_srqi (ix[3], 1);

  // a / gn
  rq = test_divuq (ix[0], ix[2]);

  ix[3] = vec_adduqm (ix[2], rq);
  ix[2] = vec_srqi (ix[3], 1);

  es = (vui128_t)CONST_VINT128_DW128(1UL, 0UL);
  rc += check_vuint128x ("divuq-sum :", (vui128_t)ix[2], (vui128_t) es);

  return (rc);
}

#define CONST_UINT128_QxD(__q0, __q1) ( \
    (((unsigned __int128) __q0) << 64) \
    + ((unsigned __int128) __q1))

unsigned __int128 dummy_i2;
unsigned __int128 dummy_i3;
unsigned __int128 divuq_parm0 = CONST_UINT128_QxD(0x0100000000000000UL, 0UL);
unsigned __int128 divuq_parm1 = CONST_UINT128_QxD(0UL, 1UL);
unsigned __int128 divuq2_parm = CONST_UINT128_QxD(__UINT64_MAX__, __UINT64_MAX__);

extern unsigned __int128 __udivti3 (unsigned __int128, unsigned __int128);

int
timed_gcc_divuq (void)
{
  //Taylor series to compute e**x where x = 1.0
  // This use case requires a 128-bit / 64-bit divide

  unsigned __int128 ix[4];
  unsigned __int128 es;
  unsigned __int128 rq;
  int rc = 0;

  // Making sure this code actually calls __udivti3 !
  ix[0] = divuq_parm0;
  ix[1] = divuq_parm1;
  ix[2] = CONST_UINT128_QxD(0UL, 1UL);
  // Scaled Fixed-point 128.120 == 1.0
  ix[3] = CONST_UINT128_QxD(0x0100000000000000UL, 0x0UL);

  rq = (ix[0] / ix[2]);
  ix[3] = (ix[3] + rq); // next term
  ix[0] = rq;
  ix[2] = (ix[2] + ix[1]);

  rq = (ix[0] / ix[2]);
  ix[3] = (ix[3] + rq); // next term
  ix[0] = rq;
  ix[2] = (ix[2] + ix[1]);

  rq = (ix[0] / ix[2]);
  ix[3] = (ix[3] + rq); // next term
  ix[0] = rq;
  ix[2] = (ix[2] + ix[1]);

  rq = (ix[0] / ix[2]);
  ix[3] = (ix[3] + rq); // next term
  ix[0] = rq;
  ix[2] = (ix[2] + ix[1]);

  rq = (ix[0] / ix[2]);
  ix[3] = (ix[3] + rq); // next term
  ix[0] = rq;
  ix[2] = (ix[2] + ix[1]);

  rq = (ix[0] / ix[2]);
  ix[3] = (ix[3] + rq); // next term
  ix[0] = rq;
  ix[2] = (ix[2] + ix[1]);

  rq = (ix[0] / ix[2]);
  ix[3] = (ix[3] + rq); // next term
  ix[0] = rq;
  ix[2] = (ix[2] + ix[1]);

  rq = (ix[0] / ix[2]);
  ix[3] = (ix[3] + rq); // next term
  ix[0] = rq;
  ix[2] = (ix[2] + ix[1]);

  rq = (ix[0] / ix[2]);
  ix[3] = (ix[3] + rq); // next term
  ix[0] = rq;
  ix[2] = (ix[2] + ix[1]);

  rq = (ix[0] / ix[2]);

  ix[3] = (ix[3] + rq); // last term

  dummy_i3 = ix[3];
  es = CONST_UINT128_QxD(0x02b7e150ed3c569dUL, 0x975c4df261fe4d64UL);
  rc += check_int128 ("div_int128-sum :", ix[3], es);

  return (rc);
}

int
timed_gcc_divuq2 (void)
{
  // Newton's method square root
  // This use case requires a full 128-bit / 128-bit divide.
  unsigned __int128 ix[4];
  unsigned __int128  es;
  unsigned __int128  rq;
  int rc = 0;

  // Making sure this code actually calls __udivti3 !
  ix[0] = divuq2_parm; // parm a from extern
  ix[1] = divuq2_parm;

  // improve initial estimate of g0
  ix[2] = (ix[1] >> 56);

  // a / g0
  rq = (ix[0] / ix[2]);
  ix[3] = (ix[2] + rq);
  ix[2] = (ix[3] >> 1);

  // a / gn
  rq = (ix[0] / ix[2]);
  ix[3] = (ix[2] + rq);
  ix[2] = (ix[3] >> 1);

  // a / gn
  rq = (ix[0] / ix[2]);
  ix[3] = (ix[2] + rq);
  ix[2] = (ix[3] >> 1);

  // a / gn
  rq = (ix[0] / ix[2]);
  ix[3] = (ix[2] + rq);
  ix[2] = (ix[3] >> 1);

  // a / gn
  rq = (ix[0] / ix[2]);
  ix[3] = (ix[2] + rq);
  ix[2] = (ix[3] >> 1);

  // a / gn
  rq = (ix[0] / ix[2]);
  ix[3] = (ix[2] + rq);
  ix[2] = (ix[3] >> 1);

  // a / gn
  rq = (ix[0] / ix[2]);
  ix[3] = (ix[2] + rq);
  ix[2] = (ix[3] >> 1);

  // a / gn
  rq = (ix[0] / ix[2]);
  ix[3] = (ix[2] + rq);
  ix[2] = (ix[3] >> 1);

  // a / gn
  rq = (ix[0] / ix[2]);
  ix[3] = (ix[2] + rq);
  ix[2] = (ix[3] >> 1);

  // a / gn
  rq = (ix[0] / ix[2]);
  ix[3] = (ix[2] + rq);
  ix[2] = (ix[3] >> 1);

  // a / gn
  rq = (ix[0] / ix[2]);
  ix[3] = (ix[2] + rq);
  ix[2] = (ix[3] >> 1);

  // a / gn
  rq = (ix[0] / ix[2]);
  ix[3] = (ix[2] + rq);
  ix[2] = (ix[3] >> 1);

  // a / gn
  rq = (ix[0] / ix[2]);
  ix[3] = (ix[2] + rq);
  ix[2] = (ix[3] >> 1);

  dummy_i2 = ix[2];

  es = CONST_UINT128_QxD(1UL, 0UL);
  rc += check_int128 ("div2_int128-sum :", ix[2], es);

  return (rc);
}

#if 1
// use implementations from vec_int64_dummy compile tests.
extern vui64_t test_divmodud (vui64_t *r, vui64_t y, vui64_t z);
extern __VEC_U_128P test_divdqu (vui128_t x, vui128_t y, vui128_t z);
//#define test_moddivduq	test_divdqu
#endif
#if 1
// Use static lib implementations.
extern vui64_t __VEC_PWR_IMP(vec_divdud) (vui64_t x, vui64_t y, vui64_t z);
#define test_divdud __VEC_PWR_IMP(vec_divdud)
extern vui64_t __VEC_PWR_IMP(vec_divud) (vui64_t y, vui64_t z);
#define test_divud __VEC_PWR_IMP(vec_divud)
extern vui64_t __VEC_PWR_IMP(vec_diveud) (vui64_t x, vui64_t z);
#define test_divude __VEC_PWR_IMP(vec_diveud)
extern vui64_t __VEC_PWR_IMP(vec_modud) (vui64_t y, vui64_t z);
#define test_modud __VEC_PWR_IMP(vec_modud)
extern vui64_t __VEC_PWR_IMP(vec_moddud) (vui64_t x, vui64_t y, vui64_t z);
#define test_moddud __VEC_PWR_IMP(vec_moddud)
#else
// use implementations from vec_int64_dummy compile tests.
extern vui64_t test_divdud (vui64_t x, vui64_t y, vui64_t z);
extern vui64_t test_divud (vui64_t y, vui64_t z);
extern vui64_t test_divude (vui64_t x, vui64_t z);
extern vui64_t test_modud (vui64_t y, vui64_t z);
extern vui64_t test_moddud (vui64_t x, vui64_t y, vui64_t z);
#endif

vui64_t dummy_x2;

int timed_divmodud (void)
{
  int ii;
  int rc = 0;

  vui64_t ix[4], lx, dx, dy, dz, dq, dr;
  vui64_t rq, qq, rr;

  const vui64_t c32_9s =(vui64_t)CONST_VINT128_DW(0x000004ee2d6d415bUL,
						  0x85acef80ffffffffUL);
  const vui64_t ten16th = (vui64_t)CONST_VINT128_DW(10000000000000000UL,
						    0UL);
  const vui64_t ten2 = (vui64_t)CONST_VINT128_DW(100UL,
						 0UL);
  const vui64_t zero = vec_splat_u64(0);

  dx = vec_mrgahd ((vui128_t) c32_9s, (vui128_t) c32_9s);
  dy = vec_mrgald ((vui128_t) c32_9s, (vui128_t) c32_9s);
  dz = vec_mrgahd ((vui128_t) ten16th, (vui128_t) ten16th);
#if 0
  dq = test_divdud (dx, dy, dz);
  dr = test_moddud (dx, dy, dz);
#else
  dq = vec_divdud_inline (dx, dy, dz);
  dr = vec_moddud_inline (dx, dy, dz);
#endif
  lx = vec_mrgahd ((vui128_t) dq, (vui128_t) dr);

  ix[2] = zero;

  // Convert low 16-digit binary values to 100s digits
  for (ii = 1; ii < 9; ii++)
    {
#if 1
#if 1
      qq = test_divmodud (&rr, lx, ten2);
#else
      qq = test_divud (lx, ten2);
      rr = test_modud (lx, ten2);
#endif
#else
      qq = vec_vdivud_inline (lx, ten2);
      rr = vec_vmodud_inline (lx, ten2);
#endif
      lx = vec_mrgald ((vui128_t) zero, (vui128_t) rq); // move Q to dividend
      // Collect digits from remainder, rotate bytes into ix[2]
      ix[2] = (vui64_t) vec_sld ((vui8_t) ix[2], (vui8_t) zero, 1);
      ix[2] = (vui64_t) vec_or ((vui8_t) ix[2], (vui8_t) rr);
      lx = qq;
    }
  dummy_x2 = ix[2];
  return rc;
}

int timed_lib_divmodud (void)
{
  int ii;
  int rc = 0;

  vui64_t ix[4], lx, dx, dy, dz, dq, dr;
  vui64_t rq, qq, rr;

  const vui64_t c32_9s =(vui64_t)CONST_VINT128_DW(0x000004ee2d6d415bUL,
						  0x85acef80ffffffffUL);
  const vui64_t ten16th = (vui64_t)CONST_VINT128_DW(10000000000000000UL,
						    0UL);
  const vui64_t ten2 = (vui64_t)CONST_VINT128_DW(100UL,
						 0UL);
  const vui64_t zero = vec_splat_u64(0);

  dx = vec_mrgahd ((vui128_t) c32_9s, (vui128_t) c32_9s);
  dy = vec_mrgald ((vui128_t) c32_9s, (vui128_t) c32_9s);
  dz = vec_mrgahd ((vui128_t) ten16th, (vui128_t) ten16th);

  dq = test_divdud (dx, dy, dz);
  dr = test_moddud (dx, dy, dz);
  lx = vec_mrgahd ((vui128_t) dq, (vui128_t) dr);

  ix[2] = zero;

  // Convert low 16-digit binary values to 100s digits
  for (ii = 1; ii < 9; ii++)
    {
      qq = test_divud (lx, ten2);
      rr = test_modud (lx, ten2);
      lx = vec_mrgald ((vui128_t) zero, (vui128_t) rq); // move Q to dividend
      // Collect digits from remainder, rotate bytes into ix[2]
      ix[2] = (vui64_t) vec_sld ((vui8_t) ix[2], (vui8_t) zero, 1);
      ix[2] = (vui64_t) vec_or ((vui8_t) ix[2], (vui8_t) rr);
      lx = qq;
    }
  dummy_x2 = ix[2];
  return rc;
}

#if 1
// Use static lib implementations.
extern vui64_t __VEC_PWR_IMP(vec_divqud) (vui128_t x_y, vui64_t z);
#define test_divqud __VEC_PWR_IMP(vec_divqud)
#else
// use implementations from vec_int64_dummy compile tests.
extern vui64_t test_divqud (vui128_t x_y, vui64_t z);
#endif
int timed_divqud (void)
{
  int ii;
  int rc = 0;

  vui64_t ix[4], hx, lx;
  vui64_t rq;

  const vui64_t c32_9s =(vui64_t)CONST_VINT128_DW(0x000004ee2d6d415bUL,
						  0x85acef80ffffffffUL);
  const vui64_t ten16th = (vui64_t)CONST_VINT128_DW(10000000000000000UL,
						    0UL);
  const vui64_t ten2 = (vui64_t)CONST_VINT128_DW(100UL,
						 0UL);
  const vui64_t zero = vec_splat_u64(0);

  // Split QW into high/low 16 digits
  rq = test_divqud ((vui128_t) c32_9s, ten16th);
  // Extend Q as high digits
  hx = vec_mrgald ((vui128_t) zero, (vui128_t) rq);
  // Extend R as low digits
  lx = vec_mrgahd ((vui128_t) zero, (vui128_t) rq);

  ix[2] = zero;

  // Convert high 16-digit binary values to 100s digits
  for (ii = 1; ii < 9; ii++)
    {
      rq = test_divqud ((vui128_t) hx, ten2);
      hx = vec_mrgald ((vui128_t) zero, (vui128_t) rq); // move Q to dividend
      // Collect digits from remainder, rotate bytes into ix[2]
      ix[3] = (vui64_t) vec_sld ((vui8_t) rq, (vui8_t) rq, 7);
      ix[2] = (vui64_t) vec_sld ((vui8_t) ix[2], (vui8_t) ix[3], 1);
    }

  // Convert low 16-digit binary values to 100s digits
  for (ii = 1; ii < 9; ii++)
    {
      rq = test_divqud ((vui128_t) lx, ten2);
      lx = vec_mrgald ((vui128_t) zero, (vui128_t) rq); // move Q to dividend
      // Collect digits from remainder, rotate bytes into ix[2]
      ix[3] = (vui64_t) vec_sld ((vui8_t) rq, (vui8_t) rq, 7);
      ix[2] = (vui64_t) vec_sld ((vui8_t) ix[2], (vui8_t) ix[3], 1);
    }
  dummy_x2 = ix[2];
  return rc;
}

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
#define test_example_longdiv_10e32(_i, _j, _k)	example_longdiv_10e32(_i, _j, _k)

int
timed_longdiv_e32 (void)
{
  vui128_t ix[4], qx[4], qy[4], qz[4], qw[4];
  vui128_t r[5];
  vui128_t el, er;
  int rc = 0;

  ix[0] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  ix[1] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  ix[2] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);
  ix[3] = (vui128_t)CONST_VINT128_DW128(__UINT64_MAX__, __UINT64_MAX__);

  r[4] = test_example_longdiv_10e32 (qx, ix, 4UL);
  r[3] = test_example_longdiv_10e32 (qy, qx, 4UL);
  r[2] = test_example_longdiv_10e32 (qz, qy, 4UL);
  r[1] = test_example_longdiv_10e32 (qw, qz, 4UL);
  r[0] = qw[3];

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

#ifndef PVECLIB_DISABLE_DFP
//#define __DEBUG_PRINT__ 1
#define test_example_longbcdcf_10e32(_i, _j, _k)	example_longbcdcf_10e32(_i, _j, _k)

int
timed_longbcdcf_10e32 (void)
{
  vui128_t ix[8], q1[8], q2[8], q3[8], q4[8], q5[8], q6[8], q7[8], q8[8], q9[8];
  vBCD_t bcd[10], chk[10];
  int rc = 0;

  // Fixed-point equivalent of __DBL_MAX__
  // 1.797693134862315e+308Le+308L
  ix[0] = (vui128_t)CONST_VINT128_DW128(0xFFFFFFFFFFFFF800UL,
					0x0000000000000000UL);
  ix[1] = (vui128_t) { (__int128) 0UL };
  ix[2] = (vui128_t) { (__int128) 0UL };
  ix[3] = (vui128_t) { (__int128) 0UL };
  ix[4] = (vui128_t) { (__int128) 0UL };
  ix[5] = (vui128_t) { (__int128) 0UL };
  ix[6] = (vui128_t) { (__int128) 0UL };
  ix[7] = (vui128_t) { (__int128) 0UL };

  chk[0] = (vBCD_t)CONST_VINT128_DW128(0x0000000000017976UL,
				       0x9313486231570814UL);
  chk[1] = (vBCD_t)CONST_VINT128_DW128(0x5274237317043567UL,
				       0x9807056752584499UL);
  chk[2] = (vBCD_t)CONST_VINT128_DW128(0x6598917476803157UL,
				       0x2607800285387605UL);
  chk[3] = (vBCD_t)CONST_VINT128_DW128(0x8955863276687817UL,
				       0x1540458953514382UL);
  chk[4] = (vBCD_t)CONST_VINT128_DW128(0x4642343213268894UL,
				       0x6418276846754670UL);
  chk[5] = (vBCD_t)CONST_VINT128_DW128(0x3537516986049910UL,
				       0x5765512820762454UL);
  chk[6] = (vBCD_t)CONST_VINT128_DW128(0x9009038932894407UL,
				       0x5868508455133942UL);
  chk[7] = (vBCD_t)CONST_VINT128_DW128(0x3045832369032229UL,
				       0x4816580855933212UL);
  chk[8] = (vBCD_t)CONST_VINT128_DW128(0x3348274797826204UL,
				       0x1447231687381771UL);
  chk[9] = (vBCD_t)CONST_VINT128_DW128(0x8091929988125040UL,
				       0x4026184124858368UL);

  bcd[9] = test_example_longbcdcf_10e32 (q1, ix, 8UL);
  rc += check_vuint128x ("modq_2e1024r9:", (vui128_t)bcd[9], (vui128_t) chk[9]);

  bcd[8] = test_example_longbcdcf_10e32 (q2, q1, 8UL);
  rc += check_vuint128x ("modq_2e1024r8:", (vui128_t)bcd[8], (vui128_t) chk[8]);

  bcd[7] = test_example_longbcdcf_10e32 (q3, q2, 8UL);
  rc += check_vuint128x ("modq_2e1024r7:", (vui128_t)bcd[7], (vui128_t) chk[7]);

  bcd[6] = test_example_longbcdcf_10e32 (q4, q3, 8UL);
  rc += check_vuint128x ("modq_2e1024r6:", (vui128_t)bcd[6], (vui128_t) chk[6]);

  bcd[5] = test_example_longbcdcf_10e32 (q5, q4, 8UL);
  rc += check_vuint128x ("modq_2e1024r5:", (vui128_t)bcd[5], (vui128_t) chk[5]);

  bcd[4] = test_example_longbcdcf_10e32 (q6, q5, 8UL);
  rc += check_vuint128x ("modq_2e1024r4:", (vui128_t)bcd[4], (vui128_t) chk[4]);

  bcd[3] = test_example_longbcdcf_10e32 (q7, q6, 8UL);
  rc += check_vuint128x ("modq_2e1024r3:", (vui128_t)bcd[3], (vui128_t) chk[3]);

  bcd[2] = test_example_longbcdcf_10e32 (q8, q7, 8UL);
  rc += check_vuint128x ("modq_2e1024r2:", (vui128_t)bcd[2], (vui128_t) chk[2]);

  bcd[1] = test_example_longbcdcf_10e32 (q9, q8, 8UL);
  bcd[0] = vec_bcdcfuq (q9[7]);

  rc += check_vuint128x ("modq_2e1024r1:", (vui128_t)bcd[1], (vui128_t) chk[1]);

  rc += check_vuint128x ("   q_2e1024r0:", (vui128_t)bcd[0], (vui128_t) chk[0]);

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

//#define __DEBUG_PRINT__ 1
#define test_example_longbcdct_10e32(_i, _j, _k, _l)	example_longbcdct_10e32(_i, _j, _k, _l)


int
timed_longbcdct_10e32 (void)
{
#ifdef __DEBUG_PRINT__
  /* ten32  = +100000000000000000000000000000000UQ  */
  const vui128_t ten32 = (vui128_t)
	  { (__int128) 10000000000000000UL * (__int128) 10000000000000000UL };
#endif
  vui128_t ix[8], jx[8];
  vBCD_t bcd[10];
  long int cnt;
  int rc = 0;

  // Reverse conversion from test_longbcdcf_10e32
  // Feed BCD from high to low order.
#ifdef __DEBUG_PRINT__
  printf ("\ntimed Vector long multiply bcdct Quadword x 8\n");
  print_vint128x (" 10**32 ", ten32);
#endif
  // Fixed-point equivalent of __DBL_MAX__
  // 1.797693134862315e+308Le+308L
  ix[0] = (vui128_t)CONST_VINT128_DW128(0xFFFFFFFFFFFFF800UL,
					0x0000000000000000UL);
  ix[1] = (vui128_t) { (__int128) 0UL };
  ix[2] = (vui128_t) { (__int128) 0UL };
  ix[3] = (vui128_t) { (__int128) 0UL };
  ix[4] = (vui128_t) { (__int128) 0UL };
  ix[5] = (vui128_t) { (__int128) 0UL };
  ix[6] = (vui128_t) { (__int128) 0UL };
  ix[7] = (vui128_t) { (__int128) 0UL };

  // BCD equivalent of Fixed-point __DBL_MAX__ in 309 digits
  bcd[0] = (vBCD_t)CONST_VINT128_DW128(0x0000000000017976UL,
				       0x9313486231570814UL);
  bcd[1] = (vBCD_t)CONST_VINT128_DW128(0x5274237317043567UL,
				       0x9807056752584499UL);
  bcd[2] = (vBCD_t)CONST_VINT128_DW128(0x6598917476803157UL,
				       0x2607800285387605UL);
  bcd[3] = (vBCD_t)CONST_VINT128_DW128(0x8955863276687817UL,
				       0x1540458953514382UL);
  bcd[4] = (vBCD_t)CONST_VINT128_DW128(0x4642343213268894UL,
				       0x6418276846754670UL);
  bcd[5] = (vBCD_t)CONST_VINT128_DW128(0x3537516986049910UL,
				       0x5765512820762454UL);
  bcd[6] = (vBCD_t)CONST_VINT128_DW128(0x9009038932894407UL,
				       0x5868508455133942UL);
  bcd[7] = (vBCD_t)CONST_VINT128_DW128(0x3045832369032229UL,
				       0x4816580855933212UL);
  bcd[8] = (vBCD_t)CONST_VINT128_DW128(0x3348274797826204UL,
				       0x1447231687381771UL);
  bcd[9] = (vBCD_t)CONST_VINT128_DW128(0x8091929988125040UL,
				       0x4026184124858368UL);

  cnt = 0;
  cnt = test_example_longbcdct_10e32 (jx, bcd[0], cnt, 8UL);
  cnt = test_example_longbcdct_10e32 (jx, bcd[1], cnt, 8UL);
  cnt = test_example_longbcdct_10e32 (jx, bcd[2], cnt, 8UL);
  cnt = test_example_longbcdct_10e32 (jx, bcd[3], cnt, 8UL);
  cnt = test_example_longbcdct_10e32 (jx, bcd[4], cnt, 8UL);
  cnt = test_example_longbcdct_10e32 (jx, bcd[5], cnt, 8UL);
  cnt = test_example_longbcdct_10e32 (jx, bcd[6], cnt, 8UL);
  cnt = test_example_longbcdct_10e32 (jx, bcd[7], cnt, 8UL);
  cnt = test_example_longbcdct_10e32 (jx, bcd[8], cnt, 8UL);
  cnt = test_example_longbcdct_10e32 (jx, bcd[9], cnt, 8UL);

#ifdef __DEBUG_PRINT__
  print_vint128x (" longbcdct ", (vui128_t) jx[0]);
  print_vint128x ("           ", (vui128_t) jx[1]);
  print_vint128x ("           ", (vui128_t) jx[2]);
  print_vint128x ("           ", (vui128_t) jx[3]);
  print_vint128x ("           ", (vui128_t) jx[4]);
  print_vint128x ("           ", (vui128_t) jx[5]);
  print_vint128x ("           ", (vui128_t) jx[6]);
  print_vint128x ("           ", (vui128_t) jx[7]);
#endif

  rc += check_vuint128x ("bcdct_2e1024:", (vui128_t)jx[0], (vui128_t) ix[0]);
  rc += check_vuint128x ("bcdct_2e1024:", (vui128_t)jx[1], (vui128_t) ix[1]);
  rc += check_vuint128x ("bcdct_2e1024:", (vui128_t)jx[2], (vui128_t) ix[2]);
  rc += check_vuint128x ("bcdct_2e1024:", (vui128_t)jx[3], (vui128_t) ix[3]);
  rc += check_vuint128x ("bcdct_2e1024:", (vui128_t)jx[4], (vui128_t) ix[4]);
  rc += check_vuint128x ("bcdct_2e1024:", (vui128_t)jx[5], (vui128_t) ix[5]);
  rc += check_vuint128x ("bcdct_2e1024:", (vui128_t)jx[6], (vui128_t) ix[6]);
  rc += check_vuint128x ("bcdct_2e1024:", (vui128_t)jx[7], (vui128_t) ix[7]);

  return (rc);
}
#endif

// Time spriontf for comparison to timed_longbcdcf_10e32
int
timed_cfmaxdouble_10e32 (void)
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

// Time strtod for comparison to timed_longbcdct_10e32
int
timed_ctmaxdouble_10e32 (void)
{
  int rc = 0;
#ifndef PVECLIB_DISABLE_F128MATH
  // Compiler __DBL_MAX__ "1.79769313486231570814527423731704e+308"
  // 53 digit precision equivalent of __DBL_MAX__
  char buf[64] = "1.7976931348623157081452742373170435679807056752584499e+308";
  char *res;
  double d = 0;

  d = strtod (buf, &res);

  if (d != __DBL_MAX__)
    {
      printf ("strtod fail is % .13a should be % .13a\n", d, __DBL_MAX__);
      rc++;
    }
#endif
  return rc;
}
