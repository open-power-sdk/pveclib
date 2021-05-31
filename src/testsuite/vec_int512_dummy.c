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


/*
 * The following const are used in both unit tests
 * and performance kernels for performance tests.
 */
const __VEC_U_512 vec512_zeros =
    {
	(vui128_t) ((unsigned __int128) 0),
	(vui128_t) ((unsigned __int128) 0),
	(vui128_t) ((unsigned __int128) 0),
	(vui128_t) ((unsigned __int128) 0)
    };

const __VEC_U_512 vec512_one = CONST_VINT512_Q
    (
      (vui128_t) ((unsigned __int128) 0x00000000),
      (vui128_t) ((unsigned __int128) 0x00000000),
      (vui128_t) ((unsigned __int128) 0x00000000),
      (vui128_t) ((unsigned __int128) 0x00000001)
    );

const __VEC_U_512 vec512_foxes = CONST_VINT512_Q
    (
      // FFFF...FFFF
      (vui128_t) ((__int128) -1),
      (vui128_t) ((__int128) -1),
      (vui128_t) ((__int128) -1),
      (vui128_t) ((__int128) -1)
    );

const __VEC_U_512 vec512_foxeasy = CONST_VINT512_Q
    (
	// FFFF...FFFE
	(vui128_t) ((__int128) -1),
	(vui128_t) ((__int128) -1),
	(vui128_t) ((__int128) -1),
	(vui128_t) ((__int128) -2)
    );

const __VEC_U_512  vec512_ten128th = CONST_VINT512_Q
    (
      CONST_VUINT128_QxW (0x00000000, 0x00000000, 0x0000024e, 0xe91f2603),
      CONST_VUINT128_QxW (0xa6337f19, 0xbccdb0da, 0xc404dc08, 0xd3cff5ec),
      CONST_VUINT128_QxW (0x2374e42f, 0x0f1538fd, 0x03df9909, 0x2e953e01),
      CONST_VUINT128_QxW (0x00000000, 0x00000000, 0x00000000, 0x00000000)
    );

const __VEC_U_512 vec512_ten256_h = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0x00000000, 0x00000000, 0x00000000, 0x00000000),
	CONST_VUINT128_QxW (0x00000000, 0x000553f7, 0x5fdcefce, 0xf46eeddc),
	CONST_VUINT128_QxW (0x80dcc7f7, 0x55bc28f2, 0x65f9ef17, 0xcc5573c0),
	CONST_VUINT128_QxW (0x63ff540e, 0x3c42d35a, 0x1d153624, 0xadc666b0)
    );

const __VEC_U_512 vec512_ten256_l = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0x26b2716e, 0xd595d80f, 0xcf4a6e70, 0x6bde50c6),
	CONST_VUINT128_QxW (0x12152f87, 0xd8d99f72, 0xbed3875b, 0x982e7c01),
	CONST_VUINT128_QxW (0x00000000, 0x00000000, 0x00000000, 0x00000000),
	CONST_VUINT128_QxW (0x00000000, 0x00000000, 0x00000000, 0x00000000)
    );

const __VEC_U_512 vec512_ten384_h = CONST_VINT512_Q
    (
        CONST_VUINT128_QxW (0x00000000, 0x00000000, 0x00000000, 0x00000000),
	CONST_VUINT128_QxW (0x00000000, 0x00000000, 0x00000000, 0x00000000),
	CONST_VUINT128_QxW (0x0c4c5e31, 0x0aef8aa1, 0x71027fff, 0x56784f44),
	CONST_VUINT128_QxW (0x4e117bef, 0xa6fab7d1, 0x99c34cce, 0x922010ba)
    );

const __VEC_U_512 vec512_ten384_m = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0xcba53571, 0xac20aee4, 0xe9cd9bd7, 0xf1b92377),
	CONST_VUINT128_QxW( 0xfc363385, 0x867b782d, 0x6e921a29, 0x949e1b66),
	CONST_VUINT128_QxW( 0x23303863, 0xb92d6f55, 0x34a8e6d6, 0x997d980d),
	CONST_VUINT128_QxW( 0x61c2dbb1, 0x09169857, 0x4cff3c6c, 0x0892aa8f)
    );

const __VEC_U_512 vec512_ten384_l = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0x3347f75d, 0xe297de35, 0x60e19896, 0x34cbba01),
	CONST_VUINT128_QxW( 0x00000000, 0x00000000, 0x00000000, 0x00000000),
	CONST_VUINT128_QxW( 0x00000000, 0x00000000, 0x00000000, 0x00000000),
	CONST_VUINT128_QxW( 0x00000000, 0x00000000, 0x00000000, 0x00000000)
    );


const __VEC_U_512 vec512_ten512_3 = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0x00000000, 0x00000000, 0x00000000, 0x00000000),
	CONST_VUINT128_QxW (0x00000000, 0x00000000, 0x00000000, 0x00000000),
	CONST_VUINT128_QxW (0x00000000, 0x00000000, 0x0000001C, 0x633415D4),
	CONST_VUINT128_QxW (0xC1D238D9, 0x8CAB8A97, 0x8A0B1F13, 0x8CB07303)
    );

const __VEC_U_512 vec512_ten512_2 = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0xA2699748, 0x45A71D46, 0xB099BC81, 0x7343AFAC),
	CONST_VUINT128_QxW (0x69BE5B0E, 0x9449775C, 0x1366732A, 0x93ABADE4),
	CONST_VUINT128_QxW (0xB2908EE0, 0xF95F635E, 0x85A91924, 0xC3FC0695),
	CONST_VUINT128_QxW (0xE7FC7153, 0x329C57AE, 0xBFA3EDAC, 0x96E14F5D)
    );

const __VEC_U_512 vec512_ten512_1 = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0xBC51FB2E, 0xB21A2F22, 0x1E25CFEA, 0x703ED321),
	CONST_VUINT128_QxW (0xAA1DA1BF, 0x28F8733B, 0x4475B579, 0xC88976C1),
	CONST_VUINT128_QxW (0x94E65747, 0x46C40513, 0xC31E1AD9, 0xB83A8A97),
	CONST_VUINT128_QxW (0x5D96976F, 0x8F9546DC, 0x77F27267, 0xFC6CF801)
    );

const __VEC_U_512 vec512_ten512_0 = CONST_VINT512_Q
    (
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0)
    );

const __VEC_U_512 vec512_ten1024_6 = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0x00000000, 0x00000000, 0x00000000, 0x00000000),
	CONST_VUINT128_QxW (0x00000000, 0x00000325, 0xD9D61A05, 0xD4305D94),
	CONST_VUINT128_QxW (0x34F4A3C6, 0x2D433949, 0xAE6209D4, 0x926C3F5B),
	CONST_VUINT128_QxW (0xD2DB49EF, 0x47187094, 0xC1A6970C, 0xA7E6BD2A)
    );

const __VEC_U_512 vec512_ten1024_5 = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0x73C55349, 0x36A8DE06, 0x1E8D4649, 0xF4F3235E),
	CONST_VUINT128_QxW (0x005B8041, 0x1640114A, 0x88BC491B, 0x9FC4ED52),
	CONST_VUINT128_QxW (0x0190FBA0, 0x35FAABA6, 0xC356E38A, 0x31B5653F),
	CONST_VUINT128_QxW (0x44597583, 0x6CB0B6C9, 0x75A351A2, 0x8E4262CE)
    );

const __VEC_U_512 vec512_ten1024_4 = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0x3CE3A0B8, 0xDF68368A, 0xE26A7B7E, 0x976A3310),
	CONST_VUINT128_QxW (0xFC8F1F90, 0x31EB0F66, 0x9A202882, 0x80BDA5A5),
	CONST_VUINT128_QxW (0x80D98089, 0xDC1A47FE, 0x6B7595FB, 0x101A3616),
	CONST_VUINT128_QxW (0xB6F4654B, 0x31FB6BFD, 0xF56DEEEC, 0xB1B896BC)
    );

const __VEC_U_512 vec512_ten1024_3 = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0x8FC51A16, 0xBF3FDEB3, 0xD814B505, 0xBA34C411),
	CONST_VUINT128_QxW (0x8AD822A5, 0x1ABE1DE3, 0x045B7A74, 0x8E1042C4),
	CONST_VUINT128_QxW (0x62BE695A, 0x9F9F2A07, 0xA7E89431, 0x922BBB9F),
	CONST_VUINT128_QxW (0xC9635986, 0x1C5CD134, 0xF451218B, 0x65DC60D7)
    );

const __VEC_U_512 vec512_ten1024_2 = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0x233E55C7, 0x231D2B9C, 0x9FCE837D, 0x1E43F61F),
	CONST_VUINT128_QxW (0x7DE16CFB, 0x896634EE, 0x0ED1440E, 0xCC2CD819),
	CONST_VUINT128_QxW (0x4C7D1E1A, 0x140AC535, 0x15C51A88, 0x991C4E87),
	CONST_VUINT128_QxW (0x1EC29F86, 0x6E7C215B, 0xF55B2B72, 0x2919F001)
    );

const __VEC_U_512 vec512_ten1024_1 = CONST_VINT512_Q
    (
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0)
    );

const __VEC_U_512 vec512_ten1024_0 = CONST_VINT512_Q
    (
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0)
    );

const __VEC_U_512 vec512_ten2048_13 = CONST_VINT512_Q
    (
	(vui128_t) ((unsigned __int128) 0),
	(vui128_t) ((unsigned __int128) 0),
	CONST_VUINT128_QxW (0x00000000, 0x00000000, 0x00000000, 0x0009e8b3),
	CONST_VUINT128_QxW (0xb5dc53d5, 0xde4a74d2, 0x8ce329ac, 0xe526a319)
    );

const __VEC_U_512 vec512_ten2048_12 = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0x7bbebe30, 0x34f77154, 0xce2bcba1, 0x9648b21c),
	CONST_VUINT128_QxW (0x11eb962b, 0x1b61b93c, 0xf2ee5ca6, 0xf7e928e6),
	CONST_VUINT128_QxW (0x1d08e2d6, 0x94222771, 0xe50f3027, 0x8c983623),
	CONST_VUINT128_QxW (0x0af908b4, 0x0a753b7d, 0x77cd8c6b, 0xe7151aab)
    );

const __VEC_U_512 vec512_ten2048_11 = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0x4efac5dc, 0xd83e49d6, 0x907855ee, 0xb028af62),
	CONST_VUINT128_QxW (0x3f6f7024, 0xd2c36fa9, 0xce9d04a4, 0x87fa1fb9),
	CONST_VUINT128_QxW (0x92be221e, 0xf1bd0ad5, 0xf775677c, 0xe0de0840),
	CONST_VUINT128_QxW (0x2ad3fa14, 0x0eac7d56, 0xc7c9dee0, 0xbedd8a6c)
    );

const __VEC_U_512 vec512_ten2048_10 = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0x038f9245, 0xb2e87c34, 0x8ad803ec, 0xca8f0070),
	CONST_VUINT128_QxW (0xf8dbb57a, 0x6a445f27, 0x8b3d5cf4, 0x2915e818),
	CONST_VUINT128_QxW (0x415c7f3e, 0xf82df846, 0x58ccf45c, 0xfad37943),
	CONST_VUINT128_QxW (0x3f3389a4, 0x408f43c5, 0x13ef5a83, 0xfb8886fb)
    );

const __VEC_U_512 vec512_ten2048_9 = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0xf56d9d4b, 0xd5f86079, 0x2e55ecee, 0x70beb181),
	CONST_VUINT128_QxW (0x0d76ce39, 0xde9ec24b, 0xcf99d019, 0x53761abd),
	CONST_VUINT128_QxW (0x9d7389c0, 0xa244de3c, 0x195355d8, 0x4eeebeee),
	CONST_VUINT128_QxW (0x6f46eadb, 0x56c6815b, 0x785ce6b7, 0xb125ac8e)
    );

const __VEC_U_512 vec512_ten2048_8 = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0xdb0708fd, 0x8f6cae5f, 0x5715f791, 0x5b33eb41),
	CONST_VUINT128_QxW (0x7bf03c19, 0xd7917c7b, 0xa1fc6b96, 0x81428c85),
	CONST_VUINT128_QxW (0x744695f0, 0xe866d7ef, 0xc9ac375d, 0x77c1a42f),
	CONST_VUINT128_QxW (0x40660460, 0x944545ff, 0x87a7dc62, 0xd752f7a6)
    );

const __VEC_U_512 vec512_ten2048_7 = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0x6a57b1ab, 0x730f203c, 0x1aa9f444, 0x84d80e2e),
	CONST_VUINT128_QxW (0x5fc5a047, 0x79c56b8a, 0x9e110c7b, 0xcbea4ca7),
	CONST_VUINT128_QxW (0x982da466, 0x3cfe491d, 0x0dbd21fe, 0xab498697),
	CONST_VUINT128_QxW (0x33554c36, 0x685e5510, 0xc4a65665, 0x4419bd43)
    );

const __VEC_U_512 vec512_ten2048_6 = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0x8e48ff35, 0xd6c7d6ab, 0x91bac974, 0xfb1264b4),
	CONST_VUINT128_QxW (0xf111821f, 0xa2bca416, 0xafe609c3, 0x13b41e44),
	CONST_VUINT128_QxW (0x9952fbed, 0x5a151440, 0x967abbb3, 0xa8281ed6),
	CONST_VUINT128_QxW (0xa8f16f92, 0x10c17f94, 0xe3892ee9, 0x8074ff01)
    );

const __VEC_U_512 vec512_ten2048_5 = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0xe3cb64f3, 0x2dbb6643, 0xa7a8289c, 0x8c6c54de),
	CONST_VUINT128_QxW (0x34c10134, 0x9713b449, 0x38209ce1, 0xf3861ce0),
	CONST_VUINT128_QxW (0xfb7fedcc, 0x235552eb, 0x57a7842d, 0x71c7fd8f),
	CONST_VUINT128_QxW (0x66912e4a, 0xd2f869c2, 0x92794987, 0x19342c12)
    );

const __VEC_U_512 vec512_ten2048_4 = CONST_VINT512_Q
    (
	CONST_VUINT128_QxW (0x866ed6f1, 0xc850dabc, 0x98342c9e, 0x51b78db2),
	CONST_VUINT128_QxW (0xea50d142, 0xfd827773, 0x2ed56d55, 0xa5e5a191),
	CONST_VUINT128_QxW (0x368b8abb, 0xb6067584, 0xee87e354, 0xec2e4721),
	CONST_VUINT128_QxW (0x49e28dcf, 0xb27d4d3f, 0xe3096865, 0x1333e001)
    );

const __VEC_U_512 vec512_ten2048_3 = CONST_VINT512_Q
    (
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0)
    );

const __VEC_U_512 vec512_ten2048_2 = CONST_VINT512_Q
    (
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0)
    );

const __VEC_U_512 vec512_ten2048_1 = CONST_VINT512_Q
    (
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0)
    );

const __VEC_U_512 vec512_ten2048_0 = CONST_VINT512_Q
    (
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0),
      (vui128_t) ((unsigned __int128) 0)
    );

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
  return result;
#endif
}

void __attribute__((flatten))
__VEC_PWR_IMP (test_mulu2048x512) (__VEC_U_512 p2560[5], __VEC_U_512 m1[4], __VEC_U_512 m2)
{
  __VEC_U_1024x512 subp0, subp1, subp2, subp3;
  __VEC_U_512x1 sum0, sum1, sum2;

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
  __VEC_U_512x1 sum1, sum2, sum3, sumx;
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
  __VEC_U_512x1 sum1, sum2, sum3, sumx;
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
  __VEC_U_512x1 sum1, sum2, sum3, sumx;
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
  __VEC_U_1024x512 subp0, subp1, subp2, subp3;
  __VEC_U_512x1 sum1, sum2, sum3, sumx;
  __VEC_U_512 temp[3];

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
  __VEC_U_512x1 sum0, sum1, sum2, sumx;
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
  __VEC_U_512x1 sum0, sum1, sum2, sumx;
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

