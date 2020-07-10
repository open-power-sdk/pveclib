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

 vec_int512_runtime.c

 Contributors:
      Steven Munroe
      Created on: Aug 20, 2019
 */

#include <pveclib/vec_int512_ppc.h>

#ifdef __VEC_EXPLICITE_FENCE_NOPS__
#undef COMPILE_FENCE
// Generate NOPS inline to make compiler fences visible in obj code.
#define COMPILE_FENCE __asm ("nop":::)
#define COMPILE_FENCE1 __asm ("ori 1,1,0":::)
#define COMPILE_FENCE2 __asm ("ori 2,2,0":::)
#define COMPILE_FENCE3 __asm ("ori 3,3,0":::)
#define COMPILE_FENCE10 __asm ("ori 10,10,0":::)
#define COMPILE_FENCE11 __asm ("ori 11,11,0":::)
#define COMPILE_FENCE12 __asm ("ori 12,12,0":::)
#define COMPILE_FENCE13 __asm ("ori 13,13,0":::)
#define COMPILE_FENCE14 __asm ("ori 14,14,0":::)
#define COMPILE_FENCE15 __asm ("ori 15,15,0":::)
#define COMPILE_FENCE16 __asm ("ori 16,16,0":::)
#define COMPILE_FENCE17 __asm ("ori 17,17,0":::)
#define COMPILE_FENCE20 __asm ("ori 20,20,0":::)
#define COMPILE_FENCE21 __asm ("ori 21,21,0":::)
#define COMPILE_FENCE22 __asm ("ori 22,22,0":::)
#define COMPILE_FENCE23 __asm ("ori 23,23,0":::)
#define COMPILE_FENCE24 __asm ("ori 24,24,0":::)
#define COMPILE_FENCE25 __asm ("ori 25,25,0":::)
#define COMPILE_FENCE26 __asm ("ori 26,26,0":::)
#define COMPILE_FENCE27 __asm ("ori 27,27,0":::)
#define COMPILE_FENCE28 __asm ("ori 28,28,0":::)
#define COMPILE_FENCE29 __asm ("ori 29,29,0":::)
#else
#define COMPILE_FENCE1 COMPILE_FENCE
#define COMPILE_FENCE2 COMPILE_FENCE
#define COMPILE_FENCE3 COMPILE_FENCE
#define COMPILE_FENCE10 COMPILE_FENCE
#define COMPILE_FENCE11 COMPILE_FENCE
#define COMPILE_FENCE12 COMPILE_FENCE
#define COMPILE_FENCE13 COMPILE_FENCE
#define COMPILE_FENCE14 COMPILE_FENCE
#define COMPILE_FENCE15 COMPILE_FENCE
#define COMPILE_FENCE16 COMPILE_FENCE
#define COMPILE_FENCE17 COMPILE_FENCE
#define COMPILE_FENCE20 COMPILE_FENCE
#define COMPILE_FENCE21 COMPILE_FENCE
#define COMPILE_FENCE22 COMPILE_FENCE
#define COMPILE_FENCE23 COMPILE_FENCE
#define COMPILE_FENCE24 COMPILE_FENCE
#define COMPILE_FENCE25 COMPILE_FENCE
#define COMPILE_FENCE26 COMPILE_FENCE
#define COMPILE_FENCE27 COMPILE_FENCE
#define COMPILE_FENCE28 COMPILE_FENCE
#define COMPILE_FENCE29 COMPILE_FENCE
#endif

__VEC_U_256
__VEC_PWR_IMP (vec_mul128x128) (vui128_t m1l, vui128_t m2l)
{
  return vec_mul128x128_inline (m1l, m2l);
}

__VEC_U_512
__VEC_PWR_IMP (vec_mul256x256) (__VEC_U_256 m1, __VEC_U_256 m2)
{
#ifdef _ARCH_PWR9
  __VEC_U_512 result;
  vui128_t mc, mp, mq, mqhl;
  vui128_t mphh, mphl, mplh, mpll;
  mpll = vec_muludq (&mplh, m1.vx0, m2.vx0);

  mp = vec_muludq (&mphl, m1.vx1, m2.vx0);
  mplh = vec_addcq (&mc, mplh, mp);
  mphl = vec_adduqm (mphl, mc);

  mp = vec_muludq (&mqhl, m1.vx0, m2.vx1);
  mplh = vec_addcq (&mq, mplh, mp);
  mphl = vec_addeq (&mc, mphl, mqhl, mq);

  mp = vec_muludq (&mphh, m1.vx1, m2.vx1);
  mphl = vec_addcq (&mq, mphl, mp);
  mphh = vec_addeuqm (mphh, mq, mc);

  result.vx0 = mpll;
  result.vx1 = mplh;
  result.vx2 = mphl;
  result.vx3 = mphh;
  return result;
#else
#ifdef _ARCH_PWR8
  __VEC_U_512 result;
  vui128_t mp, mq;
  vui128_t mphh, mphl, mplh, mpll;
  mpll = vec_muludq (&mplh, m1.vx0, m2.vx0);

  mp = vec_madduq (&mphl, m1.vx1, m2.vx0, mplh);
  mplh = mp;
  COMPILE_FENCE1;
  mp = vec_madduq (&mq, m1.vx0, m2.vx1, mplh);
  mplh = mp;

  mp = vec_madd2uq (&mphh, m1.vx1, m2.vx1, mphl, mq);
  mphl = mp;

  result.vx0 = mpll;
  result.vx1 = mplh;
  result.vx2 = mphl;
  result.vx3 = mphh;
  return result;
#else
  return vec_mul256x256_inline (m1, m2);
#endif
#endif
}

__VEC_U_640
__VEC_PWR_IMP (vec_mul512x128) (__VEC_U_512 m1, vui128_t m2)
{
#ifdef _ARCH_PWR9
  __VEC_U_640 result;
  vui128_t mq3, mq2, mq1, mq0, mq, mc;
  vui128_t mpx0, mpx1, mpx2, mpx3;
  mpx0 = vec_muludq (&mq0, m1.vx0, m2);

  mpx1 = vec_muludq (&mq1, m1.vx1, m2);
  mpx1 = vec_addcq (&mc, mpx1, mq0);

  mpx2 = vec_muludq (&mq2, m1.vx2, m2);
  mpx2 = vec_addeq (&mq, mpx2, mq1, mc);

  mpx3 = vec_muludq (&mq3, m1.vx3, m2);
  mpx3 = vec_addeq (&mc, mpx3, mq2, mq);
  mq3 = vec_adduqm (mc, mq3);

  result.vx0 = mpx0;
  result.vx1 = mpx1;
  result.vx2 = mpx2;
  result.vx3 = mpx3;
  result.vx4 = mq3;
  return result;
#else
#ifdef _ARCH_PWR8
  __VEC_U_640 result;
  vui128_t mq3, mq2, mq1, mq0, mq, mc;
  vui128_t mpx0, mpx1, mpx2, mpx3;
  mpx0 = vec_muludq (&mq0, m1.vx0, m2);
  mpx1 = vec_madduq (&mq1, m1.vx1, m2, mq0);
  COMPILE_FENCE1;
  mpx2 = vec_madduq (&mq2, m1.vx2, m2, mq1);
  mpx3 = vec_madduq (&mq3, m1.vx3, m2, mq2);

  result.vx0 = mpx0;
  result.vx1 = mpx1;
  result.vx2 = mpx2;
  result.vx3 = mpx3;
  result.vx4 = mq3;
  return result;
#else
  return vec_mul512x128_inline (m1, m2);
#endif
#endif
}

__VEC_U_640
__VEC_PWR_IMP (vec_madd512x128a128) (__VEC_U_512 m1, vui128_t m2,
                                     vui128_t a1)
{
  return vec_madd512x128a128_inline (m1, m2, a1);
}

__VEC_U_640
__VEC_PWR_IMP (vec_madd512x128a512) (__VEC_U_512 m1, vui128_t m2,
                                     __VEC_U_512 a2)
{
#ifdef _ARCH_PWR9
  __VEC_U_640 result;
  vui128_t mq3, mq2, mq1, mq0;
  vui128_t mpx0, mpx1, mpx2, mpx3;
  mpx0 = vec_madduq (&mq0, m1.vx0, m2, a2.vx0);

  mpx1 = vec_madd2uq (&mq1, m1.vx1, m2, mq0, a2.vx1);
  COMPILE_FENCE1;
  mpx2 = vec_madd2uq (&mq2, m1.vx2, m2, mq1, a2.vx2);

  mpx3 = vec_madd2uq (&mq3, m1.vx3, m2, mq2, a2.vx3);
  COMPILE_FENCE3;

  result.vx0 = mpx0;
  result.vx1 = mpx1;
  result.vx2 = mpx2;
  result.vx3 = mpx3;
  result.vx4 = mq3;
  return result;
#else
#ifdef _ARCH_PWR8
  __VEC_U_640 result;
  vui128_t mq3, mq2, mq1, mq0;
  vui128_t mpx0, mpx1, mpx2, mpx3;
  mpx0 = vec_madduq (&mq0, m1.vx0, m2, a2.vx0);

  mpx1 = vec_madd2uq (&mq1, m1.vx1, m2, mq0, a2.vx1);
  COMPILE_FENCE1;
  mpx2 = vec_madd2uq (&mq2, m1.vx2, m2, mq1, a2.vx2);

  mpx3 = vec_madd2uq (&mq3, m1.vx3, m2, mq2, a2.vx3);

  result.vx0 = mpx0;
  result.vx1 = mpx1;
  result.vx2 = mpx2;
  result.vx3 = mpx3;
  result.vx4 = mq3;
  return result;
#else
  return vec_madd512x128a512_inline (m1, m2, a2);
#endif
#endif
}

__VEC_U_640
__VEC_PWR_IMP (vec_madd512x128a128a512) (__VEC_U_512 m1, vui128_t m2,
                                         vui128_t a1, __VEC_U_512 a2)
{
  return vec_madd512x128a128a512_inline (m1, m2, a1, a2);
}

__VEC_U_1024 __attribute__((flatten))
__VEC_PWR_IMP (vec_mul512x512) (__VEC_U_512 m1, __VEC_U_512 m2)
{
#ifdef _ARCH_PWR8
  __VEC_U_1024 result;
#ifdef _ARCH_PWR9
  vui128_t mp, mq;
  __VEC_U_640 mp3, mp2, mp1, mp0;

  mp0 = __VEC_PWR_IMP (vec_mul512x128) (m1, m2.vx0);
  result.vx0 = mp0.vx0;
  COMPILE_FENCE10;

  mp1 = __VEC_PWR_IMP (vec_mul512x128) (m1, m2.vx1);
  result.vx1 = vec_addcq (&mq, mp1.vx0, mp0.vx1);
  result.vx2 = vec_addeq (&mp, mp1.vx1, mp0.vx2, mq);
  result.vx3 = vec_addeq (&mq, mp1.vx2, mp0.vx3, mp);
  result.vx4 = vec_addeq (&mp, mp1.vx3, mp0.vx4, mq);
  result.vx5 = vec_addcq (&result.vx6, mp1.vx4, mp);
  COMPILE_FENCE11;


  mp2 = __VEC_PWR_IMP (vec_mul512x128) (m1, m2.vx2);
  result.vx2 = vec_addcq (&mq, mp2.vx0, result.vx2);
  result.vx3 = vec_addeq (&mp, mp2.vx1, result.vx3, mq);
  result.vx4 = vec_addeq (&mq, mp2.vx2, result.vx4, mp);
  result.vx5 = vec_addeq (&mp, mp2.vx3, result.vx5, mq);
  result.vx6 = vec_addeq (&result.vx7, mp2.vx4, result.vx6, mp);
  COMPILE_FENCE12;

  mp3 = __VEC_PWR_IMP (vec_mul512x128) (m1, m2.vx3);
  result.vx3 = vec_addcq (&mq, mp3.vx0, result.vx3);
  result.vx4 = vec_addeq (&mp, mp3.vx1, result.vx4, mq);
  result.vx5 = vec_addeq (&mq, mp3.vx2, result.vx5, mp);
  result.vx6 = vec_addeq (&mp, mp3.vx3, result.vx6, mq);
  result.vx7 = vec_addeuqm (result.vx7, mp3.vx4, mp);
#else
  __VEC_U_512x1 mp3, mp2, mp1, mp0;

  mp0.x640 = __VEC_PWR_IMP(vec_mul512x128) (m1, m2.vx0);
  result.vx0 = mp0.x3.v1x128;
  COMPILE_FENCE10;
  mp1.x640 = __VEC_PWR_IMP(vec_madd512x128a512) (m1, m2.vx1, mp0.x3.v0x512);
  result.vx1 = mp1.x3.v1x128;
  COMPILE_FENCE11;
  mp2.x640 = __VEC_PWR_IMP(vec_madd512x128a512) (m1, m2.vx2, mp1.x3.v0x512);
  result.vx2 = mp2.x3.v1x128;
  COMPILE_FENCE12;
  mp3.x640 = __VEC_PWR_IMP(vec_madd512x128a512) (m1, m2.vx3, mp2.x3.v0x512);
  result.vx3 = mp3.x3.v1x128;
  result.vx4 = mp3.x3.v0x512.vx0;
  result.vx5 = mp3.x3.v0x512.vx1;
  result.vx6 = mp3.x3.v0x512.vx2;
  result.vx7 = mp3.x3.v0x512.vx3;
#endif
  return result;
#else
  return vec_mul512x512_inline (m1, m2);
#endif
}

__VEC_U_1024 __attribute__((flatten))
__VEC_PWR_IMP (vec_madd512x512a512) (__VEC_U_512 m1, __VEC_U_512 m2,
                                     __VEC_U_512 a1)
{
#ifdef _ARCH_PWR8
  __VEC_U_1024 result;
  __VEC_U_512x1 mp3, mp2, mp1, mp0;

  mp0.x640 = __VEC_PWR_IMP(vec_madd512x128a512) (m1, m2.vx0, a1);
  result.vx0 = mp0.x3.v1x128;
  COMPILE_FENCE14;
  mp1.x640 = __VEC_PWR_IMP(vec_madd512x128a512) (m1, m2.vx1, mp0.x3.v0x512);
  result.vx1 = mp1.x3.v1x128;
  COMPILE_FENCE15;
  mp2.x640 = __VEC_PWR_IMP(vec_madd512x128a512) (m1, m2.vx2, mp1.x3.v0x512);
  result.vx2 = mp2.x3.v1x128;
  COMPILE_FENCE16;
  mp3.x640 = __VEC_PWR_IMP(vec_madd512x128a512) (m1, m2.vx3, mp2.x3.v0x512);
  result.vx3 = mp3.x3.v1x128;
  result.vx4 = mp3.x3.v0x512.vx0;
  result.vx5 = mp3.x3.v0x512.vx1;
  result.vx6 = mp3.x3.v0x512.vx2;
  result.vx7 = mp3.x3.v0x512.vx3;
  return result;
#else
  return vec_madd512x512a512_inline (m1, m2, a1);
#endif
}

/** \brief Macros that invert the indexes so that quadword array endian
 * follows quadword endian.
 *
 * The low order quadword index is always 0.
 * The high order quadword index is 1, 3, 7.
 *
 */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define __NDX2(__index) (__index)
#define __NDX4(__index) (__index)
#define __NDX8(__index) (__index)
#else
#define __NDX2(__index) (1 - __index)
#define __NDX4(__index) (3 - __index)
#define __NDX8(__index) (7 - __index)
#endif

void __attribute__((flatten ))
#ifdef __VEC_USE_RESTRICT__
/* The Compiler team strongly suggested using  restrict here
 * but in practice (at least for GCC 8/9) we are better off without.
 * Waiting for a bug fix.
 */
__VEC_PWR_IMP (vec_mul1024x1024) (__VEC_U_2048* restrict r2048,
				  __VEC_U_1024* restrict m1_1024,
				  __VEC_U_1024* restrict m2_1024)
#else
__VEC_PWR_IMP (vec_mul1024x1024) (__VEC_U_2048* r2048,
				  __VEC_U_1024* m1_1024,
				  __VEC_U_1024* m2_1024)
#endif
{
#ifdef _ARCH_PWR8
  __VEC_U_512 *p2048, *m1, *m2;
  __VEC_U_1024x512 subp0, subp1, subp2, subp3;
  __VEC_U_512x1 sum0, sum1, sum2, sum3;
  __VEC_U_512x1 sumx;
  __VEC_U_512 temp[3];

  p2048 = (__VEC_U_512 *) r2048;
  m1 = (__VEC_U_512 *) m1_1024;
  m2 = (__VEC_U_512 *) m2_1024;

  subp0.x1024 = __VEC_PWR_IMP(vec_mul512x512) (m1[__NDX2(0)], m2[__NDX2(0)]);
  p2048[__NDX4(0)] = subp0.x2.v0x512;

  COMPILE_FENCE20;
  subp1.x1024 = __VEC_PWR_IMP(vec_mul512x512) (m1[__NDX2(1)], m2[__NDX2(0)]);
  sum1.x640 = vec_add512cu (subp1.x2.v0x512, subp0.x2.v1x512);
  temp[0] = sum1.x2.v0x512;
  temp[1] = vec_add512ze (subp1.x2.v1x512, sum1.x2.v1x128);

  COMPILE_FENCE21;
  subp2.x1024 = __VEC_PWR_IMP(vec_mul512x512) (m1[__NDX2(0)], m2[__NDX2(1)]);
  sum2.x640 = vec_add512cu (temp[0], subp2.x2.v0x512);
  temp[2] = sum2.x2.v0x512;
  p2048[__NDX4(1)] = temp[2];
  sumx.x640 = vec_add512ecu (temp[1], subp2.x2.v1x512, sum2.x2.v1x128);
  temp[1] = sumx.x2.v0x512;

  COMPILE_FENCE22;
  subp3.x1024 = __VEC_PWR_IMP(vec_mul512x512) (m1[__NDX2(1)], m2[__NDX2(1)]);
  sum3.x640 = vec_add512cu (sumx.x2.v0x512, subp3.x2.v0x512);
  p2048[__NDX4(2)] = sum3.x2.v0x512;
  p2048[__NDX4(3)] = vec_add512ze2 (subp3.x2.v1x512,
				    sumx.x2.v1x128, sum3.x2.v1x128);
#else
  __VEC_U_512 *p2048, *m1, *m2;
  __VEC_U_1024x512 subp0, subp1, subp2, subp3;
  __VEC_U_512x1 sumx;

  p2048 = (__VEC_U_512 *) r2048;
  m1 = (__VEC_U_512 *) m1_1024;
  m2 = (__VEC_U_512 *) m2_1024;

  subp0.x1024 = __VEC_PWR_IMP(vec_mul512x512) (m1[__NDX2(0)], m2[__NDX2(0)]);
  p2048[__NDX4(0)] = subp0.x2.v0x512;

  COMPILE_FENCE20;
  subp1.x1024 = __VEC_PWR_IMP(vec_madd512x512a512) (m1[__NDX2(1)], m2[__NDX2(0)],
					    subp0.x2.v1x512);

  COMPILE_FENCE21;
  subp2.x1024 = __VEC_PWR_IMP(vec_madd512x512a512) (m1[__NDX2(0)], m2[__NDX2(1)],
					    subp1.x2.v0x512);
  p2048[__NDX4(1)] = subp2.x2.v0x512;

  sumx.x640 = vec_add512cu (subp1.x2.v1x512, subp2.x2.v1x512);

  COMPILE_FENCE22;
  subp3.x1024 = __VEC_PWR_IMP(vec_madd512x512a512) (m1[__NDX2(1)], m2[__NDX2(1)],
					    sumx.x2.v0x512);
  p2048[__NDX4(2)] = subp3.x2.v0x512;
  p2048[__NDX4(3)] = vec_add512ze (subp3.x2.v1x512, sumx.x2.v1x128);
#endif
}

void __attribute__((flatten ))
#ifdef __VEC_USE_RESTRICT__
/* The Compiler team strongly suggested using  restrict here
 * but in practice (at least for GCC 8/9) we are better off without.
 * Waiting for a bug fix.
 */
__VEC_PWR_IMP (vec_mul2048x2048) (__VEC_U_4096* restrict r4096,
				  __VEC_U_2048* restrict m1_2048,
				  __VEC_U_2048* restrict m2_2048)
#else
__VEC_PWR_IMP (vec_mul2048x2048) (__VEC_U_4096* r4096,
				__VEC_U_2048* m1_2048,
				__VEC_U_2048* m2_2048)
#endif
  {
#ifdef _ARCH_PWR9
    __VEC_U_512 *p4096, *m1, *m2;
    __VEC_U_1024x512 subp0, subp1, subp2, subp3;
    __VEC_U_512x1 sum0, sum1, sum2, sum3, sumx;
    __VEC_U_512 temp[4];

    p4096 = (__VEC_U_512 *)r4096;
    m1 = (__VEC_U_512 *) m1_2048;
    m2 = (__VEC_U_512 *) m2_2048;

    subp0.x1024 = __VEC_PWR_IMP(vec_mul512x512) (m1[__NDX4(0)], m2[__NDX4(0)]);
    p4096[__NDX8(0)] = subp0.x2.v0x512;

    COMPILE_FENCE20;
    subp1.x1024 = __VEC_PWR_IMP(vec_mul512x512) (m1[__NDX4(1)], m2[__NDX4(0)]);
    sum1.x640 = vec_add512cu (subp1.x2.v0x512, subp0.x2.v1x512);
    temp[0] = sum1.x2.v0x512;

    COMPILE_FENCE20;
    subp2.x1024 = __VEC_PWR_IMP(vec_mul512x512) (m1[__NDX4(2)], m2[__NDX4(0)]);
    sum2.x640 = vec_add512ecu (subp2.x2.v0x512, subp1.x2.v1x512, sum1.x2.v1x128);
    temp[1] = sum2.x2.v0x512;

    COMPILE_FENCE21;
    subp3.x1024 = __VEC_PWR_IMP(vec_mul512x512) (m1[__NDX4(3)], m2[__NDX4(0)]);
    sum3.x640 = vec_add512ecu (subp3.x2.v0x512, subp2.x2.v1x512, sum2.x2.v1x128);

    temp[2] = sum3.x2.v0x512;
    temp[3] = vec_add512ze (subp3.x2.v1x512, sum3.x2.v1x128);

    COMPILE_FENCE22;
    subp0.x1024 = __VEC_PWR_IMP(vec_mul512x512) (m1[__NDX4(0)], m2[__NDX4(1)]);
    sum0.x640 = vec_add512cu (subp0.x2.v0x512, temp[0]);
    p4096[__NDX8(1)] = sum0.x2.v0x512;

    COMPILE_FENCE23;
    subp1.x1024 = __VEC_PWR_IMP(vec_mul512x512) (m1[__NDX4(1)], m2[__NDX4(1)]);
    sum1.x640 = vec_add512ecu (subp1.x2.v0x512, subp0.x2.v1x512, sum0.x2.v1x128);
    sumx.x640 = vec_add512cu (sum1.x2.v0x512, temp[1]);
    temp[0] = sumx.x2.v0x512;

    COMPILE_FENCE24;
    subp2.x1024 = __VEC_PWR_IMP(vec_mul512x512) (m1[__NDX4(2)], m2[__NDX4(1)]);
    sum2.x640 = vec_add512ecu (subp2.x2.v0x512, subp1.x2.v1x512, sum1.x2.v1x128);
    sumx.x640 = vec_add512ecu (sum2.x2.v0x512, temp[2], sumx.x2.v1x128);
    temp[1] = sumx.x2.v0x512;

    COMPILE_FENCE25;
    subp3.x1024 = __VEC_PWR_IMP(vec_mul512x512) (m1[__NDX4(3)], m2[__NDX4(1)]);
    sum3.x640 = vec_add512ecu (subp3.x2.v0x512, subp2.x2.v1x512, sum2.x2.v1x128);
    subp3.x2.v1x512 = vec_add512ze (subp3.x2.v1x512, sum3.x2.v1x128);
    sumx.x640 = vec_add512ecu (sum3.x2.v0x512, temp[3], sumx.x2.v1x128);
    temp[2] = sumx.x2.v0x512;

    temp[3] = vec_add512ze (subp3.x2.v1x512, sumx.x2.v1x128);

    COMPILE_FENCE26;
    subp0.x1024 = __VEC_PWR_IMP(vec_mul512x512) (m1[__NDX4(0)], m2[__NDX4(2)]);
    sum0.x640 = vec_add512cu (subp0.x2.v0x512, temp[0]);
    p4096[__NDX8(2)] = sum0.x2.v0x512;

    COMPILE_FENCE27;
    subp1.x1024 = __VEC_PWR_IMP(vec_mul512x512) (m1[__NDX4(1)], m2[__NDX4(2)]);
    sum1.x640 = vec_add512ecu (subp1.x2.v0x512, subp0.x2.v1x512, sum0.x2.v1x128);
    sumx.x640 = vec_add512cu (sum1.x2.v0x512, temp[1]);
    temp[0] = sumx.x2.v0x512;

    COMPILE_FENCE28;
    subp2.x1024 = __VEC_PWR_IMP(vec_mul512x512) (m1[__NDX4(2)], m2[__NDX4(2)]);
    sum2.x640 = vec_add512ecu (subp2.x2.v0x512, subp1.x2.v1x512, sum1.x2.v1x128);
    sumx.x640 = vec_add512ecu (sum2.x2.v0x512, temp[2], sumx.x2.v1x128);
    temp[1] = sumx.x2.v0x512;

    COMPILE_FENCE20;
    subp3.x1024 = __VEC_PWR_IMP(vec_mul512x512) (m1[__NDX4(3)], m2[__NDX4(2)]);
    sum3.x640 = vec_add512ecu (subp3.x2.v0x512, subp2.x2.v1x512, sum2.x2.v1x128);
    subp3.x2.v1x512 = vec_add512ze (subp3.x2.v1x512, sum3.x2.v1x128);
    sumx.x640 = vec_add512ecu (sum3.x2.v0x512, temp[3], sumx.x2.v1x128);
    temp[2] = sumx.x2.v0x512;
    temp[3] = vec_add512ze (subp3.x2.v1x512, sumx.x2.v1x128);

    COMPILE_FENCE21;
    subp0.x1024 = __VEC_PWR_IMP(vec_mul512x512) (m1[__NDX4(0)], m2[__NDX4(3)]);
    sum0.x640 = vec_add512cu (subp0.x2.v0x512, temp[0]);
    p4096[__NDX8(3)] = sum0.x2.v0x512;

    COMPILE_FENCE22;
    subp1.x1024 = __VEC_PWR_IMP(vec_mul512x512) (m1[__NDX4(1)], m2[__NDX4(3)]);
    sum1.x640 = vec_add512ecu (subp1.x2.v0x512, subp0.x2.v1x512, sum0.x2.v1x128);
    sumx.x640 = vec_add512cu (sum1.x2.v0x512, temp[1]);
    p4096[__NDX8(4)] = sumx.x2.v0x512;

    COMPILE_FENCE23;
    subp2.x1024 = __VEC_PWR_IMP(vec_mul512x512) (m1[__NDX4(2)], m2[__NDX4(3)]);
    sum2.x640 = vec_add512ecu (subp2.x2.v0x512, subp1.x2.v1x512, sum1.x2.v1x128);
    sumx.x640 = vec_add512ecu (sum2.x2.v0x512, temp[2], sumx.x2.v1x128);
    p4096[__NDX8(5)] = sumx.x2.v0x512;

    COMPILE_FENCE24;
    subp3.x1024 = __VEC_PWR_IMP(vec_mul512x512) (m1[__NDX4(3)], m2[__NDX4(3)]);
    sum3.x640 = vec_add512ecu (subp3.x2.v0x512, subp2.x2.v1x512, sum2.x2.v1x128);
    subp3.x2.v1x512 = vec_add512ze (subp3.x2.v1x512, sum3.x2.v1x128);
    sumx.x640 = vec_add512ecu (sum3.x2.v0x512, temp[3], sumx.x2.v1x128);
    p4096[__NDX8(6)] = sumx.x2.v0x512;
    COMPILE_FENCE25;

    p4096[__NDX8(7)] = vec_add512ze (subp3.x2.v1x512, sumx.x2.v1x128);
#else
    __VEC_U_512 *p4096, *m1, *m2;
    __VEC_U_1024x512 subp0, subp1, subp2, subp3;
    __VEC_U_512x1 sumx;
    __VEC_U_512 temp[4];

    p4096 = (__VEC_U_512 *)r4096;
    m1 = (__VEC_U_512 *) m1_2048;
    m2 = (__VEC_U_512 *) m2_2048;

    subp0.x1024 = __VEC_PWR_IMP(vec_mul512x512) (m1[__NDX4(0)], m2[__NDX4(0)]);
    p4096[__NDX8(0)] = subp0.x2.v0x512;

    COMPILE_FENCE20;
    subp1.x1024 = __VEC_PWR_IMP(vec_madd512x512a512) (m1[__NDX4(1)], m2[__NDX4(0)],
					      subp0.x2.v1x512);
    temp[0] = subp1.x2.v0x512;
    COMPILE_FENCE20;

    subp2.x1024 = __VEC_PWR_IMP(vec_madd512x512a512) (m1[__NDX4(2)], m2[__NDX4(0)],
					      subp1.x2.v1x512);
    temp[1] = subp2.x2.v0x512;
    COMPILE_FENCE21;

    subp3.x1024 = __VEC_PWR_IMP(vec_madd512x512a512) (m1[__NDX4(3)], m2[__NDX4(0)],
					      subp2.x2.v1x512);
    temp[2] = subp3.x2.v0x512;
    temp[3] = subp3.x2.v1x512;
    COMPILE_FENCE22;

    subp0.x1024 = __VEC_PWR_IMP(vec_madd512x512a512) (m1[__NDX4(0)], m2[__NDX4(1)],
					      temp[0]);
    p4096[__NDX8(1)] = subp0.x2.v0x512;
    COMPILE_FENCE23;

    subp1.x1024 = __VEC_PWR_IMP(vec_madd512x512a512) (m1[__NDX4(1)], m2[__NDX4(1)],
					      temp[1]);
    sumx.x640 = vec_add512cu (subp1.x2.v0x512, subp0.x2.v1x512);
    temp[0] = sumx.x2.v0x512;
    COMPILE_FENCE24;

    subp2.x1024 = __VEC_PWR_IMP(vec_madd512x512a512) (m1[__NDX4(2)], m2[__NDX4(1)],
					      temp[2]);
    sumx.x640 = vec_add512ecu (subp2.x2.v0x512, subp1.x2.v1x512, sumx.x2.v1x128);
    temp[1] = sumx.x2.v0x512;
    COMPILE_FENCE25;

    subp3.x1024 = __VEC_PWR_IMP(vec_madd512x512a512) (m1[__NDX4(3)], m2[__NDX4(1)],
					      temp[3]);
    sumx.x640 = vec_add512ecu (subp3.x2.v0x512, subp2.x2.v1x512, sumx.x2.v1x128);
    temp[2] = sumx.x2.v0x512;
    temp[3] = vec_add512ze (subp3.x2.v1x512, sumx.x2.v1x128);
    COMPILE_FENCE26;

    subp0.x1024 = __VEC_PWR_IMP(vec_madd512x512a512) (m1[__NDX4(0)], m2[__NDX4(2)],
					      temp[0]);
    p4096[__NDX8(2)] = subp0.x2.v0x512;
    COMPILE_FENCE27;

    subp1.x1024 = __VEC_PWR_IMP(vec_madd512x512a512) (m1[__NDX4(1)], m2[__NDX4(2)],
					      temp[1]);
    sumx.x640 = vec_add512cu (subp1.x2.v0x512, subp0.x2.v1x512);
    temp[0] = sumx.x2.v0x512;
    COMPILE_FENCE20;

    subp2.x1024 = __VEC_PWR_IMP(vec_madd512x512a512) (m1[__NDX4(2)], m2[__NDX4(2)],
					      temp[2]);
    sumx.x640 = vec_add512ecu (subp2.x2.v0x512, subp1.x2.v1x512, sumx.x2.v1x128);
    temp[1] = sumx.x2.v0x512;
    COMPILE_FENCE21;

    subp3.x1024 = __VEC_PWR_IMP(vec_madd512x512a512) (m1[__NDX4(3)], m2[__NDX4(2)],
					      temp[3]);
    sumx.x640 = vec_add512ecu (subp3.x2.v0x512, subp2.x2.v1x512, sumx.x2.v1x128);
    temp[2] = sumx.x2.v0x512;

    temp[3] = vec_add512ze (subp3.x2.v1x512, sumx.x2.v1x128);
    COMPILE_FENCE22;

    subp0.x1024 = __VEC_PWR_IMP(vec_madd512x512a512) (m1[__NDX4(0)], m2[__NDX4(3)],
					      temp[0]);
    p4096[__NDX8(3)] = subp0.x2.v0x512;
    COMPILE_FENCE23;

    subp1.x1024 = __VEC_PWR_IMP(vec_madd512x512a512) (m1[__NDX4(1)], m2[__NDX4(3)],
					      temp[1]);
    sumx.x640 = vec_add512cu (subp1.x2.v0x512, subp0.x2.v1x512);
    p4096[__NDX8(4)] = sumx.x2.v0x512;
    COMPILE_FENCE24;

    subp2.x1024 = __VEC_PWR_IMP(vec_madd512x512a512) (m1[__NDX4(2)], m2[__NDX4(3)],
					      temp[2]);
    sumx.x640 = vec_add512ecu (subp2.x2.v0x512, subp1.x2.v1x512, sumx.x2.v1x128);
    p4096[__NDX8(5)] = sumx.x2.v0x512;
    COMPILE_FENCE25;

    subp3.x1024 = __VEC_PWR_IMP(vec_madd512x512a512) (m1[__NDX4(3)], m2[__NDX4(3)],
					      temp[3]);
    sumx.x640 = vec_add512ecu (subp3.x2.v0x512, subp2.x2.v1x512, sumx.x2.v1x128);
    p4096[__NDX8(6)] = sumx.x2.v0x512;
    COMPILE_FENCE26;

    p4096[__NDX8(7)] = vec_add512ze (subp3.x2.v1x512, sumx.x2.v1x128);
#endif
  }

// Adjust the order of quadwords in multiple quadword precision value,
// to match the endian order of the bytes in a quadword.
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define __MDX(__index) (__index)
#define __NDX(__index) (__index)
#define __PDX(__index) (__index)
#else
#define __MDX(__index) ((mx - 1) - (__index))
#define __NDX(__index) ((nx - 1) - (__index))
#define __PDX(__index) ((px - 1) - (__index))
#endif

void __attribute__((flatten ))
__VEC_PWR_IMP (vec_mul128_byMN) (vui128_t *p,
		  vui128_t *m1, vui128_t *m2,
		  unsigned long M, unsigned long N)
{
  vui128_t *mp = m1;
  vui128_t *np = m2;
  unsigned long mx = M;
  unsigned long nx = N;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  unsigned long px = M + N;
#endif
  unsigned long i, j;
  vui128_t mpx0, mqx0, mpx1, mqx1;

  /* sizeof(m1) < sizeof(m2) swap the pointers and quadword counts.
   * This allows for early exit when size of either is 1. */
  if (mx < nx)
    {
      vui128_t *xp;
      unsigned long x;

      x = mx;
      xp = mp;
      mx = nx;
      mp = np;
      nx = x;
      np = xp;
    }

  if (nx > 0)
    {
      mpx0 = vec_muludq (&mqx0, mp[__MDX(0)], np[__NDX(0)]);
      p[__PDX(0)] = mpx0;
      for (i = 1; i < mx; i++)
	{
	  mpx1 = vec_madduq (&mqx1, mp[__MDX(i)], np[__NDX(0)], mqx0);
	  p[__PDX(i)] = mpx1;
	  mqx0 = mqx1;
	}
      p[__PDX(mx)] = mqx0;

      for (j = 1; j < nx; j++)
	{
	  mpx0 = vec_madduq (&mqx0, mp[__MDX(0)], np[__NDX(j)], p[__PDX(j)]);
	  p[__PDX(j)] = mpx0;
	  for (i = 1; i < mx; i++)
	    {
	      mpx1 = vec_madd2uq (&mqx1, mp[__MDX(i)], np[__NDX(j)], mqx0,
				  p[__PDX(i + j)]);
	      p[__PDX(i + j)] = mpx1;
	      mqx0 = mqx1;
	    }
	  p[__PDX(mx + j)] = mqx0;
	}
    }
  else
    {
      /* If one of multipliers is sizeof() zero, treat it a multiply
       * by zero, and if the other multiplier is not sizeof() zero, set
       * the product array to 0. */
      const vui128_t c_zero = (vui128_t) ((unsigned __int128) 0);
      if (mx > 0)
	{
	  for (i = 0; i < mx; i++)
	    {
	      p[__PDX(i)] = c_zero;
	    }
	}
    }
}

void __attribute__((flatten ))
__VEC_PWR_IMP (vec_mul512_byMN) (__VEC_U_512 *p,
                  __VEC_U_512 *m1, __VEC_U_512 *m2,
		  unsigned long M, unsigned long N)
{
  __VEC_U_512 *mp = m1;
  __VEC_U_512 *np = m2;
  unsigned long mx = M;
  unsigned long nx = N;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  unsigned long px = M + N;
#endif
  unsigned long i, j;
  __VEC_U_1024x512 mpx0, mpx1;
  __VEC_U_512x1 sum1;
  __VEC_U_512 mqx0;
  vui128_t mcx0;

  /* sizeof(m1) < sizeof(m2) swap the pointers and quadword counts.
   * This allows for early exit when size of either is 1. */
  if (mx < nx)
    {
      __VEC_U_512 *xp;
      unsigned long x;

      x = mx;
      xp = mp;
      mx = nx;
      mp = np;
      nx = x;
      np = xp;
    }

  if (nx > 0)
    {
      mpx0.x1024 = __VEC_PWR_IMP(vec_mul512x512) (mp[__MDX(0)], np[__NDX(0)]);
      p[__PDX(0)] = mpx0.x2.v0x512;
      mqx0 = mpx0.x2.v1x512;
      for (i = 1; i < mx; i++)
	{
	  mpx1.x1024 = __VEC_PWR_IMP(vec_madd512x512a512) (mp[__MDX(i)], np[__NDX(0)], mqx0);
	  p[__PDX(i)] = mpx1.x2.v0x512;
	  mqx0 = mpx1.x2.v1x512;
	}
      p[__PDX(mx)] = mqx0;

      for (j = 1; j < nx; j++)
	{
	  mpx0.x1024 = __VEC_PWR_IMP(vec_madd512x512a512) (mp[__MDX(0)], np[__NDX(j)], p[__PDX(j)]);
	  p[__PDX(j)] = mpx0.x2.v0x512;
	  mqx0 = mpx0.x2.v1x512;
	  mcx0 = (vui128_t) ((unsigned __int128) 0);
	  for (i = 1; i < mx; i++)
	    {
	      mpx1.x1024 = __VEC_PWR_IMP(vec_madd512x512a512) (mp[__MDX(i)], np[__NDX(j)], mqx0);

	      sum1.x640 = vec_add512ecu (mpx1.x2.v0x512, p[__PDX(i + j)], mcx0);
	      p[__PDX(i + j)] = sum1.x2.v0x512;
	      mcx0 = sum1.x2.v1x128;
	      mqx0 = mpx1.x2.v1x512;
	    }
	  p[__PDX(mx + j)] = vec_add512ze (mqx0, mcx0);
	}
    }
  else
    {
      /* If one of multipliers is sizeof() zero, treat it a multiply
       * by zero, and if the other multiplier is not sizeof() zero, set
       * the product array to 0. */
      __VEC_U_512 p_zero;
      p_zero.vx0 = (vui128_t) ((unsigned __int128) 0);
      p_zero.vx1 = (vui128_t) ((unsigned __int128) 0);
      p_zero.vx2 = (vui128_t) ((unsigned __int128) 0);
      p_zero.vx3 = (vui128_t) ((unsigned __int128) 0);
      if (mx > 0)
	{
	  for (i = 0; i < mx; i++)
	    {
	      p[__PDX(i)] = p_zero;
	    }
	}
    }
}

