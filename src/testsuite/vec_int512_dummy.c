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

 vec_int512_dummy.c

 Contributors:
      Steven Munroe
      Created on: Aug 20, 2019
 */

#include <stdint.h>
#include <stdio.h>
#include <pveclib/vec_int512_ppc.h>

__VEC_U_640
__VEC_PWR_IMP (test_add512cu) (__VEC_U_512 a, __VEC_U_512 b)
{
  return vec_add512cu (a, b);
}

__VEC_U_640
__VEC_PWR_IMP (test_add512ecu) (__VEC_U_512 a, __VEC_U_512 b, vui128_t c)
{
  return vec_add512ecu (a, b, c);
}

__VEC_U_512
__VEC_PWR_IMP (test_add512eum) (__VEC_U_512 a, __VEC_U_512 b, vui128_t c)
{
  return vec_add512eum (a, b, c);
}

__VEC_U_512
__VEC_PWR_IMP (test_add512ze) (__VEC_U_512 a, vui128_t c)
{
  return vec_add512ze (a, c);
}

__VEC_U_512
__VEC_PWR_IMP (test_add512ze2) (__VEC_U_512 a, vui128_t c1, vui128_t c2)
{
  return vec_add512ze2 (a, c1, c2);
}

__VEC_U_256 /* __attribute__((__target__("cpu=power8"))) */
__VEC_PWR_IMP (test_mul128x128) (vui128_t m1l, vui128_t m2l)
{
  return vec_mul128x128_inline (m1l, m2l);
}

__VEC_U_512
__VEC_PWR_IMP (test_mul256x256) (__VEC_U_256 m1, __VEC_U_256 m2)
{
  return vec_mul256x256_inline (m1, m2);
}

__VEC_U_640
__VEC_PWR_IMP (test_mul512x128) (__VEC_U_512 m1, vui128_t m2)
{
  return vec_mul512x128_inline (m1, m2);
}

__VEC_U_512
__VEC_PWR_IMP (test_add512um) (__VEC_U_512 a, __VEC_U_512 b)
{
  return vec_add512um (a, b);
}

__VEC_U_640
__VEC_PWR_IMP (test_mulu512x128) (__VEC_U_512 m1, vui128_t m2)
{
  __VEC_U_640 result;
  vui128_t mq3, mq2, mq1, mq0, mq, mc;
  vui128_t mpx0, mpx1, mpx2, mpx3;

  mpx0 = vec_muludq (&mq0, m1.vx0, m2);
  COMPILE_FENCE;
  mpx1 = vec_muludq (&mq1, m1.vx1, m2);
  mpx1 = vec_addcq (&mc, mpx1, mq0);
  COMPILE_FENCE;
  mpx2 = vec_muludq (&mq2, m1.vx2, m2);
  mpx2 = vec_addeq (&mq, mpx2, mq1, mc);
  COMPILE_FENCE;
  mpx3 = vec_muludq (&mq3, m1.vx3, m2);
  mpx3 = vec_addeq (&mc, mpx3, mq2, mq);
  mq3 = vec_adduqm (mc, mq3);
  COMPILE_FENCE;

  result.vx0 = mpx0;
  result.vx1 = mpx1;
  result.vx2 = mpx2;
  result.vx3 = mpx3;
  result.vx4 = mq3;
  return result;
}

__VEC_U_1152
__VEC_PWR_IMP (test_mulu1024x128) (__VEC_U_1024 m1, vui128_t m2)
{
  __VEC_U_1152 result;
  vui128_t mq7, mq6, mq5, mq4, mq3, mq2, mq1, mq0;
  vui128_t mq, mc;
  vui128_t mpx0, mpx1, mpx2, mpx3;
  vui128_t mpx4, mpx5, mpx6, mpx7;
  mpx0 = vec_muludq (&mq0, m1.vx0, m2);
  COMPILE_FENCE;
  mpx1 = vec_muludq (&mq1, m1.vx1, m2);
  mpx1 = vec_addcq (&mc, mpx1, mq0);
  COMPILE_FENCE;
  mpx2 = vec_muludq (&mq2, m1.vx2, m2);
  mpx2 = vec_addeq (&mq, mpx2, mq1, mc);
  COMPILE_FENCE;
  mpx3 = vec_muludq (&mq3, m1.vx3, m2);
  mpx3 = vec_addeq (&mc, mpx3, mq2, mq);
  COMPILE_FENCE;

  mpx4 = vec_muludq (&mq4, m1.vx4, m2);
  mpx4 = vec_addeq (&mq, mpx4, mq3, mc);
  COMPILE_FENCE;
  mpx5 = vec_muludq (&mq5, m1.vx5, m2);
  mpx5 = vec_addeq (&mc, mpx5, mq4, mq);
  COMPILE_FENCE;
  mpx6 = vec_muludq (&mq6, m1.vx6, m2);
  mpx6 = vec_addeq (&mq, mpx6, mq5, mc);
  COMPILE_FENCE;
  mpx7 = vec_muludq (&mq7, m1.vx7, m2);
  mpx7 = vec_addeq (&mc, mpx7, mq6, mq);
  mq7 = vec_adduqm (mc, mq7);
  COMPILE_FENCE;

  result.vx0 = mpx0;
  result.vx1 = mpx1;
  result.vx2 = mpx2;
  result.vx3 = mpx3;
  result.vx4 = mpx4;
  result.vx5 = mpx5;
  result.vx6 = mpx6;
  result.vx7 = mpx7;
  result.vx8 = mq7;
  return result;
}

__VEC_U_2176
__VEC_PWR_IMP (test_mulu2048x128) (__VEC_U_2048 m1, vui128_t m2)
{
  __VEC_U_2176 result;
  vui128_t mq7, mq6, mq5, mq4, mq3, mq2, mq1, mq0;
  vui128_t mq15, mq14, mq13, mq12, mq11, mq10, mq9, mq8;
  vui128_t mq, mc;
  vui128_t mpx0, mpx1, mpx2, mpx3;
  vui128_t mpx4, mpx5, mpx6, mpx7;
  vui128_t mpx8, mpx9, mpx10, mpx11;
  vui128_t mpx12, mpx13, mpx14, mpx15;
  mpx0 = vec_muludq (&mq0, m1.vx0, m2);
  COMPILE_FENCE;
  mpx1 = vec_muludq (&mq1, m1.vx1, m2);
  mpx1 = vec_addcq (&mc, mpx1, mq0);
  COMPILE_FENCE;
  mpx2 = vec_muludq (&mq2, m1.vx2, m2);
  mpx2 = vec_addeq (&mq, mpx2, mq1, mc);
  COMPILE_FENCE;
  mpx3 = vec_muludq (&mq3, m1.vx3, m2);
  mpx3 = vec_addeq (&mc, mpx3, mq2, mq);
  COMPILE_FENCE;

  mpx4 = vec_muludq (&mq4, m1.vx4, m2);
  mpx4 = vec_addeq (&mq, mpx4, mq3, mc);
  COMPILE_FENCE;
  mpx5 = vec_muludq (&mq5, m1.vx5, m2);
  mpx5 = vec_addeq (&mc, mpx5, mq4, mq);
  COMPILE_FENCE;
  mpx6 = vec_muludq (&mq6, m1.vx6, m2);
  mpx6 = vec_addeq (&mq, mpx6, mq5, mc);
  COMPILE_FENCE;
  mpx7 = vec_muludq (&mq7, m1.vx7, m2);
  mpx7 = vec_addeq (&mc, mpx7, mq6, mq);
  COMPILE_FENCE;

  mpx8 = vec_muludq (&mq8, m1.vx8, m2);
  mpx8 = vec_addeq (&mq, mpx8, mq7, mc);
  COMPILE_FENCE;
  mpx9 = vec_muludq (&mq9, m1.vx9, m2);
  mpx9 = vec_addeq (&mc, mpx9, mq8, mq);
  COMPILE_FENCE;
  mpx10 = vec_muludq (&mq10, m1.vx10, m2);
  mpx10 = vec_addeq (&mq, mpx10, mq9, mc);
  COMPILE_FENCE;
  mpx11 = vec_muludq (&mq11, m1.vx11, m2);
  mpx11 = vec_addeq (&mc, mpx11, mq10, mq);
  COMPILE_FENCE;

  mpx12 = vec_muludq (&mq12, m1.vx12, m2);
  mpx12 = vec_addeq (&mq, mpx12, mq11, mc);
  COMPILE_FENCE;
  mpx13 = vec_muludq (&mq13, m1.vx13, m2);
  mpx13 = vec_addeq (&mc, mpx13, mq12, mq);
  COMPILE_FENCE;
  mpx14 = vec_muludq (&mq14, m1.vx14, m2);
  mpx14 = vec_addeq (&mq, mpx14, mq13, mc);
  COMPILE_FENCE;
  mpx15 = vec_muludq (&mq15, m1.vx15, m2);
  mpx15 = vec_addeq (&mc, mpx15, mq14, mq);
  mq15 = vec_adduqm (mc, mq15);
  COMPILE_FENCE;

  result.vx0 = mpx0;
  result.vx1 = mpx1;
  result.vx2 = mpx2;
  result.vx3 = mpx3;
  result.vx4 = mpx4;
  result.vx5 = mpx5;
  result.vx6 = mpx6;
  result.vx7 = mpx7;
  result.vx8 = mpx8;
  result.vx9 = mpx9;
  result.vx10 = mpx10;
  result.vx11 = mpx11;
  result.vx12 = mpx12;
  result.vx13 = mpx13;
  result.vx14 = mpx14;
  result.vx15 = mpx15;
  result.vx16 = mq15;
  return result;
}

__VEC_U_1024
__VEC_PWR_IMP (test_mulu512x512) (__VEC_U_512 m1, __VEC_U_512 m2)
{
#if 1
  return vec_mul512x512_inline (m1, m2);
#else
  __VEC_U_1024 result;
  vui128_t mc, mp, mq;
  __VEC_U_640 mp3, mp2, mp1, mp0;
#if 1

  mp0 = test_mul512x128_inline (m1, m2.vx0);
  result.vx0 = mp0.vx0;
  result.vx1 = mp0.vx1;
  result.vx2 = mp0.vx2;
  result.vx3 = mp0.vx3;
  result.vx4 = mp0.vx4;
  COMPILE_FENCE;
  mp1 = test_mul512x128_inline (m1, m2.vx1);
  result.vx1 = vec_addcq (&mq, mp1.vx0, result.vx1);
  result.vx2 = vec_addeq (&mp, mp1.vx1, result.vx2, mq);
  result.vx3 = vec_addeq (&mq, mp1.vx2, result.vx3, mp);
  result.vx4 = vec_addeq (&mp, mp1.vx3, result.vx4, mq);
  result.vx5 = vec_addcq (&result.vx6, mp1.vx4, mp);
  COMPILE_FENCE;
  mp2 = test_mul512x128_inline (m1, m2.vx2);
  result.vx2 = vec_addcq (&mq, mp2.vx0, result.vx2);
  result.vx3 = vec_addeq (&mp, mp2.vx1, result.vx3, mq);
  result.vx4 = vec_addeq (&mq, mp2.vx2, result.vx4, mp);
  result.vx5 = vec_addeq (&mp, mp2.vx3, result.vx5, mq);
  result.vx6 = vec_addeq (&result.vx7, mp2.vx4, result.vx6, mp);
  COMPILE_FENCE;
  mp3 = test_mul512x128_inline (m1, m2.vx3);
  result.vx3 = vec_addcq (&mq, mp3.vx0, result.vx3);
  result.vx4 = vec_addeq (&mp, mp3.vx1, result.vx4, mq);
  result.vx5 = vec_addeq (&mq, mp3.vx2, result.vx5, mp);
  result.vx6 = vec_addeq (&mp, mp3.vx3, result.vx6, mq);
  result.vx7 = vec_addeuqm (result.vx7, mp3.vx4, mp);
#else
  mp0 = test_muluq512x128_inline_PWR9 (m1, m2.vx0);
  result.vx0 = mp0.vx0;
  result.vx1 = mp0.vx1;
  result.vx2 = mp0.vx2;
  result.vx3 = mp0.vx3;
  result.vx4 = mp0.vx4;
  mp1 = test_muluq512x128_inline_PWR9 (m1, m2.vx1);
  result.vx1 = vec_addcq (&mq, mp1.vx0, result.vx1);
  result.vx2 = vec_addeq (&mp, mp1.vx1, result.vx2, mq);
  result.vx3 = vec_addeq (&mq, mp1.vx2, result.vx3, mp);
  result.vx4 = vec_addeq (&mp, mp1.vx3, result.vx4, mq);
  result.vx5 = vec_addcq (&result.vx6, mp1.vx4, mp);
  mp2 = test_muluq512x128_inline_PWR9 (m1, m2.vx2);
  result.vx2 = vec_addcq (&mq, mp2.vx0, result.vx2);
  result.vx3 = vec_addeq (&mp, mp2.vx1, result.vx3, mq);
  result.vx4 = vec_addeq (&mq, mp2.vx2, result.vx4, mp);
  result.vx5 = vec_addeq (&mp, mp2.vx3, result.vx5, mq);
  result.vx6 = vec_addeq (&result.vx7, mp2.vx4, result.vx6, mp);
  mp3 = test_muluq512x128_inline_PWR9 (m1, m2.vx3);
  result.vx3 = vec_addcq (&mq, mp3.vx0, result.vx3);
  result.vx4 = vec_addeq (&mp, mp3.vx1, result.vx4, mq);
  result.vx5 = vec_addeq (&mq, mp3.vx2, result.vx5, mp);
  result.vx6 = vec_addeq (&mp, mp3.vx3, result.vx6, mq);
  result.vx7 = vec_addeuqm (mp3.vx4, result.vx7, mp);
#endif
  return result;
#endif
}

void __attribute__((flatten))
__VEC_PWR_IMP (test_mulu2048x512) (__VEC_U_512 p2560[5], __VEC_U_512 m1[4], __VEC_U_512 m2)
{
  __VEC_U_1024x512 subp0, subp1, subp2, subp3;
  __VEC_U_512x1 sum0, sum1, sum2, sum3, sumx;
  __VEC_U_512 temp[4];

  subp0.x1024 = vec_mul512x512_inline (m1[0], m2);
  p2560[0] = subp0.x2.v0x512;

  subp1.x1024 = vec_mul512x512_inline (m1[1], m2);
  sum0.x640 = vec_add512cu (subp1.x2.v0x512, subp0.x2.v1x512);

  p2560[1] = sum0.x2.v0x512;
  COMPILE_FENCE;

  subp2.x1024 = vec_mul512x512_inline (m1[2], m2);
  sum1.x640 = vec_add512ecu (subp2.x2.v0x512, subp1.x2.v1x512, sum0.x2.v1x128);

  p2560[2] = sum1.x2.v0x512;
  COMPILE_FENCE;

  subp3.x1024 = vec_mul512x512_inline (m1[3], m2);
  sum2.x640 = vec_add512ecu (subp3.x2.v0x512, subp2.x2.v1x512, sum1.x2.v1x128);

  p2560[3] = sum2.x2.v0x512;
  p2560[4] = vec_add512ze  (subp3.x2.v1x512, sum2.x2.v1x128);
  COMPILE_FENCE;

}

void  __attribute__((flatten ))
__VEC_PWR_IMP (test_mul1024x1024) (__VEC_U_2048 r2048, __VEC_U_1024 m1_1024, __VEC_U_1024 m2_1024)
{
  __VEC_U_2048x512 *p2048;
  __VEC_U_1024x512 *m1, *m2;
  __VEC_U_1024x512 subp0, subp1, subp2, subp3;
  __VEC_U_512x1 sum0, sum1, sum2, sum3, sumx;
  __VEC_U_512 temp[3];

  p2048 = (__VEC_U_2048x512 *) &r2048;
  m1 = (__VEC_U_1024x512 *) &m1_1024;
  m2 = (__VEC_U_1024x512 *) &m2_1024;

  subp0.x1024 = vec_mul512x512_inline (m1->x2.v0x512, m2->x2.v0x512);
  p2048->x4.v0x512 = subp0.x2.v0x512;

  subp1.x1024 = vec_mul512x512_inline (m1->x2.v1x512, m2->x2.v0x512);
  sum1.x640 = vec_add512cu (subp1.x2.v0x512, subp0.x2.v1x512);

  temp[0] = sum1.x2.v0x512;
  temp[1] = vec_add512ze  (subp1.x2.v1x512, sum1.x2.v1x128);
  COMPILE_FENCE;

  subp2.x1024 = vec_mul512x512_inline (m1->x2.v0x512, m2->x2.v1x512);
  sum2.x640 = vec_add512cu (temp[0], subp2.x2.v0x512);
  temp[2] = sum2.x2.v0x512;
  p2048->x4.v1x512 = temp[2];
  sumx.x640 = vec_add512ecu (temp[1], subp2.x2.v1x512, sum2.x2.v1x128);
  temp[1] = sumx.x2.v0x512;
  COMPILE_FENCE;

  subp3.x1024 = vec_mul512x512_inline (m1->x2.v1x512, m2->x2.v1x512);
  sum3.x640 = vec_add512cu (sumx.x2.v0x512, subp3.x2.v0x512);
  p2048->x4.v2x512 = sum3.x2.v0x512;
  p2048->x4.v3x512 = vec_add512ze2 (subp3.x2.v1x512, sumx.x2.v1x128, sum3.x2.v1x128);

}

void  __attribute__((flatten ))
__VEC_PWR_IMP (test_mul1024x1024x) (__VEC_U_2048 *r2048, __VEC_U_1024 *m1_1024, __VEC_U_1024 *m2_1024)
{
  __VEC_U_512 *p2048, *m1, *m2;
  __VEC_U_1024x512 subp0, subp1, subp2, subp3;
  __VEC_U_512x1 sum0, sum1, sum2, sum3, sumx;
  __VEC_U_512 temp[3];

  p2048 = (__VEC_U_512 *)r2048;
  m1 = (__VEC_U_512 *) m1_1024;
  m2 = (__VEC_U_512 *) m2_1024;

  subp0.x1024 = vec_mul512x512_inline (m1[0], m2[0]);
  p2048[0] = subp0.x2.v0x512;

  subp1.x1024 = vec_mul512x512_inline (m1[1], m2[0]);
  sum1.x640 = vec_add512cu (subp1.x2.v0x512, subp0.x2.v1x512);

  temp[0] = sum1.x2.v0x512;
  temp[1] = vec_add512ze  (subp1.x2.v1x512, sum1.x2.v1x128);
  COMPILE_FENCE;

  subp2.x1024 = vec_mul512x512_inline (m1[0], m2[1]);
  sum2.x640 = vec_add512cu (temp[0], subp2.x2.v0x512);
  temp[2] = sum2.x2.v0x512;
  p2048[1] = temp[2];
  sumx.x640 = vec_add512ecu (temp[1], subp2.x2.v1x512, sum2.x2.v1x128);
  temp[1] = sumx.x2.v0x512;
  COMPILE_FENCE;

  subp3.x1024 = vec_mul512x512_inline (m1[1], m2[1]);
  sum3.x640 = vec_add512cu (sumx.x2.v0x512, subp3.x2.v0x512);
  p2048[2] = sum3.x2.v0x512;
  p2048[3] = vec_add512ze2 (subp3.x2.v1x512, sumx.x2.v1x128, sum3.x2.v1x128);

}

void  __attribute__((flatten ))
__VEC_PWR_IMP (test_mul1024x1024y) (__VEC_U_2048 *r2048, __VEC_U_1024 *m1_1024, __VEC_U_1024 *m2_1024)
{
  __VEC_U_2048x512 *p2048;
  __VEC_U_1024x512 *m1, *m2;
  __VEC_U_1024x512 subp0, subp1, subp2, subp3;
  __VEC_U_512x1 sum0, sum1, sum2, sum3, sumx;
  __VEC_U_512 temp[3];

  p2048 = (__VEC_U_2048x512 *)r2048;
  m1 = (__VEC_U_1024x512 *) m1_1024;
  m2 = (__VEC_U_1024x512 *) m2_1024;

  subp0.x1024 = vec_mul512x512_inline (m1->x2.v0x512, m2->x2.v0x512);
  p2048->x4.v0x512 = subp0.x2.v0x512;

  subp1.x1024 = vec_mul512x512_inline (m1->x2.v1x512, m2->x2.v0x512);
  sum1.x640 = vec_add512cu (subp1.x2.v0x512, subp0.x2.v1x512);

  temp[0] = sum1.x2.v0x512;
  temp[1] = vec_add512ze  (subp1.x2.v1x512, sum1.x2.v1x128);
  COMPILE_FENCE;

  subp2.x1024 = vec_mul512x512_inline (m1->x2.v0x512, m2->x2.v1x512);
  sum2.x640 = vec_add512cu (temp[0], subp2.x2.v0x512);
  temp[2] = sum2.x2.v0x512;
  p2048->x4.v1x512 = temp[2];
  sumx.x640 = vec_add512ecu (temp[1], subp2.x2.v1x512, sum2.x2.v1x128);
  temp[1] = sumx.x2.v0x512;
  COMPILE_FENCE;

  subp3.x1024 = vec_mul512x512_inline (m1->x2.v1x512, m2->x2.v1x512);
  sum3.x640 = vec_add512cu (sumx.x2.v0x512, subp3.x2.v0x512);
  p2048->x4.v2x512 = sum3.x2.v0x512;
  p2048->x4.v3x512 = vec_add512ze2 (subp3.x2.v1x512, sumx.x2.v1x128, sum3.x2.v1x128);

}

void  __attribute__((flatten ))
__VEC_PWR_IMP (test_mul1024x1024z) (__VEC_U_2048x512 p2048, __VEC_U_1024x512 m1, __VEC_U_1024x512 m2)
{
  //__VEC_U_2048x512 *p2048;
  //__VEC_U_1024x512 *m1, *m2;
  __VEC_U_1024x512 subp0, subp1, subp2, subp3;
  __VEC_U_512x1 sum0, sum1, sum2, sum3, sumx;
  __VEC_U_512 temp[3];

  //p2048 = (__VEC_U_2048x512 *)r2048;
  //m1 = (__VEC_U_1024x512 *) m1_1024;
  //m2 = (__VEC_U_1024x512 *) m2_1024;

  subp0.x1024 = vec_mul512x512_inline (m1.x2.v0x512, m2.x2.v0x512);
  p2048.x4.v0x512 = subp0.x2.v0x512;

  subp1.x1024 = vec_mul512x512_inline (m1.x2.v1x512, m2.x2.v0x512);
  sum1.x640 = vec_add512cu (subp1.x2.v0x512, subp0.x2.v1x512);

  temp[0] = sum1.x2.v0x512;
  temp[1] = vec_add512ze  (subp1.x2.v1x512, sum1.x2.v1x128);
  COMPILE_FENCE;

  subp2.x1024 = vec_mul512x512_inline (m1.x2.v0x512, m2.x2.v1x512);
  sum2.x640 = vec_add512cu (temp[0], subp2.x2.v0x512);
  temp[2] = sum2.x2.v0x512;
  p2048.x4.v1x512 = temp[2];
  sumx.x640 = vec_add512ecu (temp[1], subp2.x2.v1x512, sum2.x2.v1x128);
  temp[1] = sumx.x2.v0x512;
  COMPILE_FENCE;

  subp3.x1024 = vec_mul512x512_inline (m1.x2.v1x512, m2.x2.v1x512);
  sum3.x640 = vec_add512cu (sumx.x2.v0x512, subp3.x2.v0x512);
  p2048.x4.v2x512 = sum3.x2.v0x512;
  p2048.x4.v3x512 = vec_add512ze2 (subp3.x2.v1x512, sumx.x2.v1x128, sum3.x2.v1x128);

}

void  __attribute__((flatten /*, target_clones("cpu=power9,default") */))
__VEC_PWR_IMP (test_mulu2048x2048) (__VEC_U_512 p4096[8], __VEC_U_512 m1[4], __VEC_U_512 m2[4])
{
  __VEC_U_1024x512 subp0, subp1, subp2, subp3;
  __VEC_U_512x1 sum0, sum1, sum2, sum3, sumx;
  __VEC_U_512 temp[4];

  subp0.x1024 = vec_mul512x512_inline (m1[0], m2[0]);
  p4096[0] = subp0.x2.v0x512;

  subp1.x1024 = vec_mul512x512_inline (m1[1], m2[0]);
  sum0.x640 = vec_add512cu (subp1.x2.v0x512, subp0.x2.v1x512);

  temp[0] = sum0.x2.v0x512;
  COMPILE_FENCE;

  subp2.x1024 = vec_mul512x512_inline (m1[2], m2[0]);
  sum1.x640 = vec_add512ecu (subp2.x2.v0x512, subp1.x2.v1x512, sum0.x2.v1x128);

  temp[1] = sum1.x2.v0x512;
  COMPILE_FENCE;

  subp3.x1024 = vec_mul512x512_inline (m1[3], m2[0]);
  sum2.x640 = vec_add512ecu (subp3.x2.v0x512, subp2.x2.v1x512, sum1.x2.v1x128);

  temp[2] = sum2.x2.v0x512;
  temp[3] = vec_add512ze  (subp3.x2.v1x512, sum2.x2.v1x128);
  COMPILE_FENCE;



  subp0.x1024 = vec_mul512x512_inline (m1[0], m2[1]);
  sumx.x640 = vec_add512cu (subp0.x2.v0x512, temp[0]);
  p4096[1] = sumx.x2.v0x512;
  COMPILE_FENCE;

  subp1.x1024 = vec_mul512x512_inline (m1[1], m2[1]);
  sum0.x640 = vec_add512cu (subp1.x2.v0x512, subp0.x2.v1x512);
  sumx.x640 = vec_add512ecu (sum0.x2.v0x512, temp[1], sum0.x2.v1x128);
  sumx.x2.v1x128 = vec_adduqm (sumx.x2.v1x128, sum0.x2.v1x128);

  temp[0] = sumx.x2.v0x512;
  COMPILE_FENCE;

  subp2.x1024 = vec_mul512x512_inline (m1[2], m2[1]);
  sum1.x640 = vec_add512cu (subp2.x2.v0x512, subp1.x2.v1x512);
  sumx.x640 = vec_add512ecu (sum1.x2.v0x512, temp[2], sum1.x2.v1x128);
  sumx.x2.v1x128 = vec_adduqm (sumx.x2.v1x128, sum0.x2.v1x128);

  temp[1] = sumx.x2.v0x512;
  COMPILE_FENCE;

  subp3.x1024 = vec_mul512x512_inline (m1[3], m2[1]);
  sum2.x640 = vec_add512cu (subp3.x2.v0x512, subp2.x2.v1x512);
  sumx.x640 = vec_add512ecu (sum2.x2.v0x512, temp[3], sum1.x2.v1x128);
  sumx.x2.v1x128 = vec_adduqm (sumx.x2.v1x128, sum0.x2.v1x128);

  temp[2] = sumx.x2.v0x512;
  temp[3] = vec_add512ze  (subp3.x2.v1x512, sumx.x2.v1x128);
  COMPILE_FENCE;


  subp0.x1024 = vec_mul512x512_inline (m1[0], m2[2]);
  sumx.x640 = vec_add512cu (subp0.x2.v0x512, temp[0]);
  p4096[2] = sumx.x2.v0x512;
  COMPILE_FENCE;

  subp1.x1024 = vec_mul512x512_inline (m1[1], m2[2]);
  sum0.x640 = vec_add512cu (subp1.x2.v0x512, subp0.x2.v1x512);
  sumx.x640 = vec_add512ecu (sum0.x2.v0x512, temp[1], sum0.x2.v1x128);
  sumx.x2.v1x128 = vec_adduqm (sumx.x2.v1x128, sum0.x2.v1x128);

  temp[0] = sumx.x2.v0x512;
  COMPILE_FENCE;

  subp2.x1024 = vec_mul512x512_inline (m1[2], m2[2]);
  sum1.x640 = vec_add512cu (subp2.x2.v0x512, subp1.x2.v1x512);
  sumx.x640 = vec_add512ecu (sum1.x2.v0x512, temp[2], sum1.x2.v1x128);
  sumx.x2.v1x128 = vec_adduqm (sumx.x2.v1x128, sum0.x2.v1x128);

  temp[1] = sumx.x2.v0x512;
  COMPILE_FENCE;

  subp3.x1024 = vec_mul512x512_inline (m1[3], m2[2]);
  sum2.x640 = vec_add512cu (subp3.x2.v0x512, subp2.x2.v1x512);
  sumx.x640 = vec_add512ecu (sum2.x2.v0x512, temp[3], sum1.x2.v1x128);
  sumx.x2.v1x128 = vec_adduqm (sumx.x2.v1x128, sum0.x2.v1x128);

  temp[2] = sumx.x2.v0x512;
  temp[3] = vec_add512ze  (subp3.x2.v1x512, sumx.x2.v1x128);
  COMPILE_FENCE;



  subp0.x1024 = vec_mul512x512_inline (m1[0], m2[3]);
  sumx.x640 = vec_add512cu (subp0.x2.v0x512, temp[0]);
  p4096[3] = sumx.x2.v0x512;
  COMPILE_FENCE;

  subp1.x1024 = vec_mul512x512_inline (m1[1], m2[3]);
  sum0.x640 = vec_add512cu (subp1.x2.v0x512, subp0.x2.v1x512);
  sumx.x640 = vec_add512ecu (sum0.x2.v0x512, temp[1], sum0.x2.v1x128);
  sumx.x2.v1x128 = vec_adduqm (sumx.x2.v1x128, sum0.x2.v1x128);

  p4096[4] = sumx.x2.v0x512;
  COMPILE_FENCE;

  subp2.x1024 = vec_mul512x512_inline (m1[2], m2[3]);
  sum1.x640 = vec_add512cu (subp2.x2.v0x512, subp1.x2.v1x512);
  sumx.x640 = vec_add512ecu (sum1.x2.v0x512, temp[2], sum1.x2.v1x128);
  sumx.x2.v1x128 = vec_adduqm (sumx.x2.v1x128, sum0.x2.v1x128);

  p4096[5] = sumx.x2.v0x512;
  COMPILE_FENCE;

  subp3.x1024 = vec_mul512x512_inline (m1[3], m2[3]);
  sum2.x640 = vec_add512cu (subp3.x2.v0x512, subp2.x2.v1x512);
  sumx.x640 = vec_add512ecu (sum2.x2.v0x512, temp[3], sum1.x2.v1x128);
  sumx.x2.v1x128 = vec_adduqm (sumx.x2.v1x128, sum0.x2.v1x128);

  p4096[6] = sumx.x2.v0x512;
  p4096[7] = vec_add512ze  (subp3.x2.v1x512, sumx.x2.v1x128);

}

void  __attribute__((flatten))
__VEC_PWR_IMP (test_mulu2048x2048z) (__VEC_U_4096 *r4096,
              __VEC_U_2048 *m1_2048, __VEC_U_2048 *m2_2048)
{
  __VEC_U_512 *p4096, *m1, *m2;
  __VEC_U_1024x512 subp0, subp1, subp2, subp3;
  __VEC_U_512x1 sum0, sum1, sum2, sum3, sumx;
  __VEC_U_512 temp[4];

  p4096 = (__VEC_U_512 *)r4096;
  m1 = (__VEC_U_512 *) m1_2048;
  m2 = (__VEC_U_512 *) m2_2048;

  subp0.x1024 = vec_mul512x512_inline (m1[0], m2[0]);
  p4096[0] = subp0.x2.v0x512;

  subp1.x1024 = vec_mul512x512_inline (m1[1], m2[0]);
  sum0.x640 = vec_add512cu (subp1.x2.v0x512, subp0.x2.v1x512);

  temp[0] = sum0.x2.v0x512;
  COMPILE_FENCE;

  subp2.x1024 = vec_mul512x512_inline (m1[2], m2[0]);
  sum1.x640 = vec_add512ecu (subp2.x2.v0x512, subp1.x2.v1x512, sum0.x2.v1x128);

  temp[1] = sum1.x2.v0x512;
  COMPILE_FENCE;

  subp3.x1024 = vec_mul512x512_inline (m1[3], m2[0]);
  sum2.x640 = vec_add512ecu (subp3.x2.v0x512, subp2.x2.v1x512, sum1.x2.v1x128);

  temp[2] = sum2.x2.v0x512;
  temp[3] = vec_add512ze  (subp3.x2.v1x512, sum2.x2.v1x128);
  COMPILE_FENCE;



  subp0.x1024 = vec_mul512x512_inline (m1[0], m2[1]);
  sumx.x640 = vec_add512cu (subp0.x2.v0x512, temp[0]);
  p4096[1] = sumx.x2.v0x512;
  COMPILE_FENCE;

  subp1.x1024 = vec_mul512x512_inline (m1[1], m2[1]);
  sum0.x640 = vec_add512cu (subp1.x2.v0x512, subp0.x2.v1x512);
  sumx.x640 = vec_add512ecu (sum0.x2.v0x512, temp[1], sum0.x2.v1x128);
  sumx.x2.v1x128 = vec_adduqm (sumx.x2.v1x128, sum0.x2.v1x128);

  temp[0] = sumx.x2.v0x512;
  COMPILE_FENCE;

  subp2.x1024 = vec_mul512x512_inline (m1[2], m2[1]);
  sum1.x640 = vec_add512cu (subp2.x2.v0x512, subp1.x2.v1x512);
  sumx.x640 = vec_add512ecu (sum1.x2.v0x512, temp[2], sum1.x2.v1x128);
  sumx.x2.v1x128 = vec_adduqm (sumx.x2.v1x128, sum0.x2.v1x128);

  temp[1] = sumx.x2.v0x512;
  COMPILE_FENCE;

  subp3.x1024 = vec_mul512x512_inline (m1[3], m2[1]);
  sum2.x640 = vec_add512cu (subp3.x2.v0x512, subp2.x2.v1x512);
  sumx.x640 = vec_add512ecu (sum2.x2.v0x512, temp[3], sum1.x2.v1x128);
  sumx.x2.v1x128 = vec_adduqm (sumx.x2.v1x128, sum0.x2.v1x128);

  temp[2] = sumx.x2.v0x512;
  temp[3] = vec_add512ze  (subp3.x2.v1x512, sumx.x2.v1x128);
  COMPILE_FENCE;


  subp0.x1024 = vec_mul512x512_inline (m1[0], m2[2]);
  sumx.x640 = vec_add512cu (subp0.x2.v0x512, temp[0]);
  p4096[2] = sumx.x2.v0x512;
  COMPILE_FENCE;

  subp1.x1024 = vec_mul512x512_inline (m1[1], m2[2]);
  sum0.x640 = vec_add512cu (subp1.x2.v0x512, subp0.x2.v1x512);
  sumx.x640 = vec_add512ecu (sum0.x2.v0x512, temp[1], sum0.x2.v1x128);
  sumx.x2.v1x128 = vec_adduqm (sumx.x2.v1x128, sum0.x2.v1x128);

  temp[0] = sumx.x2.v0x512;
  COMPILE_FENCE;

  subp2.x1024 = vec_mul512x512_inline (m1[2], m2[2]);
  sum1.x640 = vec_add512cu (subp2.x2.v0x512, subp1.x2.v1x512);
  sumx.x640 = vec_add512ecu (sum1.x2.v0x512, temp[2], sum1.x2.v1x128);
  sumx.x2.v1x128 = vec_adduqm (sumx.x2.v1x128, sum0.x2.v1x128);

  temp[1] = sumx.x2.v0x512;
  COMPILE_FENCE;

  subp3.x1024 = vec_mul512x512_inline (m1[3], m2[2]);
  sum2.x640 = vec_add512cu (subp3.x2.v0x512, subp2.x2.v1x512);
  sumx.x640 = vec_add512ecu (sum2.x2.v0x512, temp[3], sum1.x2.v1x128);
  sumx.x2.v1x128 = vec_adduqm (sumx.x2.v1x128, sum0.x2.v1x128);

  temp[2] = sumx.x2.v0x512;
  temp[3] = vec_add512ze  (subp3.x2.v1x512, sumx.x2.v1x128);
  COMPILE_FENCE;



  subp0.x1024 = vec_mul512x512_inline (m1[0], m2[3]);
  sumx.x640 = vec_add512cu (subp0.x2.v0x512, temp[0]);
  p4096[3] = sumx.x2.v0x512;
  COMPILE_FENCE;

  subp1.x1024 = vec_mul512x512_inline (m1[1], m2[3]);
  sum0.x640 = vec_add512cu (subp1.x2.v0x512, subp0.x2.v1x512);
  sumx.x640 = vec_add512ecu (sum0.x2.v0x512, temp[1], sum0.x2.v1x128);
  sumx.x2.v1x128 = vec_adduqm (sumx.x2.v1x128, sum0.x2.v1x128);

  p4096[4] = sumx.x2.v0x512;
  COMPILE_FENCE;

  subp2.x1024 = vec_mul512x512_inline (m1[2], m2[3]);
  sum1.x640 = vec_add512cu (subp2.x2.v0x512, subp1.x2.v1x512);
  sumx.x640 = vec_add512ecu (sum1.x2.v0x512, temp[2], sum1.x2.v1x128);
  sumx.x2.v1x128 = vec_adduqm (sumx.x2.v1x128, sum0.x2.v1x128);

  p4096[5] = sumx.x2.v0x512;
  COMPILE_FENCE;

  subp3.x1024 = vec_mul512x512_inline (m1[3], m2[3]);
  sum2.x640 = vec_add512cu (subp3.x2.v0x512, subp2.x2.v1x512);
  sumx.x640 = vec_add512ecu (sum2.x2.v0x512, temp[3], sum1.x2.v1x128);
  sumx.x2.v1x128 = vec_adduqm (sumx.x2.v1x128, sum0.x2.v1x128);

  p4096[6] = sumx.x2.v0x512;
  p4096[7] = vec_add512ze  (subp3.x2.v1x512, sumx.x2.v1x128);

}

