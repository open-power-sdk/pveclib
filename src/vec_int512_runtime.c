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

__VEC_U_256
__VEC_PWR_IMP (vec_mul128x128) (vui128_t m1l, vui128_t m2l)
{
  return vec_mul128x128_inline (m1l, m2l);
}

__VEC_U_512
__VEC_PWR_IMP (vec_mul256x256) (__VEC_U_256 m1, __VEC_U_256 m2)
{
  return vec_mul256x256_inline (m1, m2);
}

__VEC_U_640
__VEC_PWR_IMP (vec_mul512x128) (__VEC_U_512 m1, vui128_t m2)
{
  return vec_mul512x128_inline (m1, m2);
}

__VEC_U_1024 __attribute__((flatten))
__VEC_PWR_IMP (vec_mul512x512) (__VEC_U_512 m1, __VEC_U_512 m2)
{
  return vec_mul512x512_inline (m1, m2);
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
#if 0
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
  __VEC_U_512 *p2048, *m1, *m2;
  __VEC_U_1024x512 subp0, subp1, subp2, subp3;
  __VEC_U_512x1 sum0, sum1, sum2, sum3, sumx;
  __VEC_U_512 temp[3];

  p2048 = (__VEC_U_512 *) r2048;
  m1 = (__VEC_U_512 *) m1_1024;
  m2 = (__VEC_U_512 *) m2_1024;

  subp0.x1024 = vec_mul512x512_inline (m1[__NDX2(0)], m2[__NDX2(0)]);
  p2048[__NDX4(0)] = subp0.x2.v0x512;

  subp1.x1024 = vec_mul512x512_inline (m1[__NDX2(1)], m2[__NDX2(0)]);
  sum1.x640 = vec_add512cu (subp1.x2.v0x512, subp0.x2.v1x512);

  temp[0] = sum1.x2.v0x512;
  temp[1] = vec_add512ze (subp1.x2.v1x512, sum1.x2.v1x128);
  COMPILE_FENCE;

  subp2.x1024 = vec_mul512x512_inline (m1[__NDX2(0)], m2[__NDX2(1)]);
  sum2.x640 = vec_add512cu (temp[0], subp2.x2.v0x512);
  temp[2] = sum2.x2.v0x512;
  p2048[__NDX4(1)] = temp[2];
  sumx.x640 = vec_add512ecu (temp[1], subp2.x2.v1x512, sum2.x2.v1x128);
  temp[1] = sumx.x2.v0x512;
  COMPILE_FENCE;

  subp3.x1024 = vec_mul512x512_inline (m1[__NDX2(1)], m2[__NDX2(1)]);
  sum3.x640 = vec_add512cu (sumx.x2.v0x512, subp3.x2.v0x512);
  p2048[__NDX4(2)] = sum3.x2.v0x512;
  p2048[__NDX4(3)] = vec_add512ze2 (subp3.x2.v1x512,
				    sumx.x2.v1x128, sum3.x2.v1x128);
}

void __attribute__((flatten ))
#if 0
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
    __VEC_U_512 *p4096, *m1, *m2;
    __VEC_U_1024x512 subp0, subp1, subp2, subp3;
    __VEC_U_512x1 sum0, sum1, sum2, sum3, sumx;
    __VEC_U_512 temp[4];

    p4096 = (__VEC_U_512 *)r4096;
    m1 = (__VEC_U_512 *) m1_2048;
    m2 = (__VEC_U_512 *) m2_2048;

    subp0.x1024 = vec_mul512x512_inline (m1[__NDX4(0)], m2[__NDX4(0)]);
    p4096[__NDX8(0)] = subp0.x2.v0x512;

    subp1.x1024 = vec_mul512x512_inline (m1[__NDX4(1)], m2[__NDX4(0)]);
    sum1.x640 = vec_add512cu (subp1.x2.v0x512, subp0.x2.v1x512);
    temp[0] = sum1.x2.v0x512;
    COMPILE_FENCE;

    subp2.x1024 = vec_mul512x512_inline (m1[__NDX4(2)], m2[__NDX4(0)]);
    sum2.x640 = vec_add512ecu (subp2.x2.v0x512, subp1.x2.v1x512, sum1.x2.v1x128);
    temp[1] = sum2.x2.v0x512;
    COMPILE_FENCE;

    subp3.x1024 = vec_mul512x512_inline (m1[__NDX4(3)], m2[__NDX4(0)]);
    sum3.x640 = vec_add512ecu (subp3.x2.v0x512, subp2.x2.v1x512, sum2.x2.v1x128);

    temp[2] = sum3.x2.v0x512;
    temp[3] = vec_add512ze (subp3.x2.v1x512, sum3.x2.v1x128);
    COMPILE_FENCE;

    subp0.x1024 = vec_mul512x512_inline (m1[__NDX4(0)], m2[__NDX4(1)]);
    sum0.x640 = vec_add512cu (subp0.x2.v0x512, temp[0]);
    p4096[__NDX8(1)] = sum0.x2.v0x512;
    COMPILE_FENCE;

    subp1.x1024 = vec_mul512x512_inline (m1[__NDX4(1)], m2[__NDX4(1)]);
    sum1.x640 = vec_add512ecu (subp1.x2.v0x512, subp0.x2.v1x512, sum0.x2.v1x128);
    sumx.x640 = vec_add512cu (sum1.x2.v0x512, temp[1]);
    temp[0] = sumx.x2.v0x512;
    COMPILE_FENCE;

    subp2.x1024 = vec_mul512x512_inline (m1[__NDX4(2)], m2[__NDX4(1)]);
    sum2.x640 = vec_add512ecu (subp2.x2.v0x512, subp1.x2.v1x512, sum1.x2.v1x128);
    sumx.x640 = vec_add512ecu (sum2.x2.v0x512, temp[2], sumx.x2.v1x128);
    temp[1] = sumx.x2.v0x512;
    COMPILE_FENCE;

    subp3.x1024 = vec_mul512x512_inline (m1[__NDX4(3)], m2[__NDX4(1)]);
    sum3.x640 = vec_add512ecu (subp3.x2.v0x512, subp2.x2.v1x512, sum2.x2.v1x128);
    subp3.x2.v1x512 = vec_add512ze (subp3.x2.v1x512, sum3.x2.v1x128);
    sumx.x640 = vec_add512ecu (sum3.x2.v0x512, temp[3], sumx.x2.v1x128);
    temp[2] = sumx.x2.v0x512;
    COMPILE_FENCE;

    temp[3] = vec_add512ze (subp3.x2.v1x512, sumx.x2.v1x128);
    COMPILE_FENCE;

    subp0.x1024 = vec_mul512x512_inline (m1[__NDX4(0)], m2[__NDX4(2)]);
    sum0.x640 = vec_add512cu (subp0.x2.v0x512, temp[0]);
    p4096[__NDX8(2)] = sum0.x2.v0x512;
    COMPILE_FENCE;

    subp1.x1024 = vec_mul512x512_inline (m1[__NDX4(1)], m2[__NDX4(2)]);
    sum1.x640 = vec_add512ecu (subp1.x2.v0x512, subp0.x2.v1x512, sum0.x2.v1x128);
    sumx.x640 = vec_add512cu (sum1.x2.v0x512, temp[1]);
    temp[0] = sumx.x2.v0x512;
    COMPILE_FENCE;

    subp2.x1024 = vec_mul512x512_inline (m1[__NDX4(2)], m2[__NDX4(2)]);
    sum2.x640 = vec_add512ecu (subp2.x2.v0x512, subp1.x2.v1x512, sum1.x2.v1x128);
    sumx.x640 = vec_add512ecu (sum2.x2.v0x512, temp[2], sumx.x2.v1x128);
    temp[1] = sumx.x2.v0x512;
    COMPILE_FENCE;

    subp3.x1024 = vec_mul512x512_inline (m1[__NDX4(3)], m2[__NDX4(2)]);
    sum3.x640 = vec_add512ecu (subp3.x2.v0x512, subp2.x2.v1x512, sum2.x2.v1x128);
    subp3.x2.v1x512 = vec_add512ze (subp3.x2.v1x512, sum3.x2.v1x128);
    sumx.x640 = vec_add512ecu (sum3.x2.v0x512, temp[3], sumx.x2.v1x128);
    temp[2] = sumx.x2.v0x512;
    COMPILE_FENCE;

    temp[3] = vec_add512ze (subp3.x2.v1x512, sumx.x2.v1x128);
    COMPILE_FENCE;

    subp0.x1024 = vec_mul512x512_inline (m1[__NDX4(0)], m2[__NDX4(3)]);
    sum0.x640 = vec_add512cu (subp0.x2.v0x512, temp[0]);
    p4096[__NDX8(3)] = sum0.x2.v0x512;
    COMPILE_FENCE;

    subp1.x1024 = vec_mul512x512_inline (m1[__NDX4(1)], m2[__NDX4(3)]);
    sum1.x640 = vec_add512ecu (subp1.x2.v0x512, subp0.x2.v1x512, sum0.x2.v1x128);
    sumx.x640 = vec_add512cu (sum1.x2.v0x512, temp[1]);
    p4096[__NDX8(4)] = sumx.x2.v0x512;
    COMPILE_FENCE;

    subp2.x1024 = vec_mul512x512_inline (m1[__NDX4(2)], m2[__NDX4(3)]);
    sum2.x640 = vec_add512ecu (subp2.x2.v0x512, subp1.x2.v1x512, sum1.x2.v1x128);
    sumx.x640 = vec_add512ecu (sum2.x2.v0x512, temp[2], sumx.x2.v1x128);
    p4096[__NDX8(5)] = sumx.x2.v0x512;
    COMPILE_FENCE;

    subp3.x1024 = vec_mul512x512_inline (m1[__NDX4(3)], m2[__NDX4(3)]);
    sum3.x640 = vec_add512ecu (subp3.x2.v0x512, subp2.x2.v1x512, sum2.x2.v1x128);
    subp3.x2.v1x512 = vec_add512ze (subp3.x2.v1x512, sum3.x2.v1x128);
    sumx.x640 = vec_add512ecu (sum3.x2.v0x512, temp[3], sumx.x2.v1x128);
    p4096[__NDX8(6)] = sumx.x2.v0x512;
    COMPILE_FENCE;

    p4096[__NDX8(7)] = vec_add512ze (subp3.x2.v1x512, sumx.x2.v1x128);
  }
