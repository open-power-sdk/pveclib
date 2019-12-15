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

 arith128_test_i512.h

 Contributors:
      Steven Munroe
      Created on: Sep 9, 2019
 */

#ifndef SRC_TESTSUITE_ARITH128_TEST_I512_H_
#define SRC_TESTSUITE_ARITH128_TEST_I512_H_
#include <pveclib/vec_int512_ppc.h>

extern const __VEC_U_512 vec512_zeros;
extern const __VEC_U_512 vec512_one;
extern const __VEC_U_512 vec512_foxes;
extern const __VEC_U_512 vec512_foxeasy;

extern const __VEC_U_512 vec512_ten128th;

extern const __VEC_U_512 vec512_ten256_h;
extern const __VEC_U_512 vec512_ten256_l;

extern const __VEC_U_512 vec512_ten384_h;
extern const __VEC_U_512 vec512_ten384_m;
extern const __VEC_U_512 vec512_ten384_l;

extern const __VEC_U_512 vec512_ten512_3;
extern const __VEC_U_512 vec512_ten512_2;
extern const __VEC_U_512 vec512_ten512_1;
extern const __VEC_U_512 vec512_ten512_0;

extern const __VEC_U_512 vec512_ten1024_6;
extern const __VEC_U_512 vec512_ten1024_5;
extern const __VEC_U_512 vec512_ten1024_4;
extern const __VEC_U_512 vec512_ten1024_3;
extern const __VEC_U_512 vec512_ten1024_2;
extern const __VEC_U_512 vec512_ten1024_1;
extern const __VEC_U_512 vec512_ten1024_0;

extern int test_mul128x128 (void);
extern int test_mul256x256 (void);
extern int test_mul512x128 (void);
extern int test_mul512x512 (void);
extern int test_mul2048x2048 (void);

extern int test_vec_i512 (void);


#endif /* SRC_TESTSUITE_ARITH128_TEST_I512_H_ */
