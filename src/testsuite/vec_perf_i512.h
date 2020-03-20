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

 vec_perf_i512.h

 Contributors:
      Steven Munroe
      Created on: Sep 9, 2019
 */

#ifndef SRC_TESTSUITE_VEC_PERF_I512_H_
#define SRC_TESTSUITE_VEC_PERF_I512_H_

extern int timed_mul128x128 (void);
extern int timed_mul256x256 (void);
extern int timed_mul512x512 (void);
extern int timed_mul512x512by8 (void);
extern int timed_mul1024x1024 (void);
extern int timed_mul1024x1024by8 (void);
extern int timed_mul2048x2048 (void);
extern int timed_mul2048x2048by8 (void);
extern int timed_mul2048x2048_MN (void);
extern int timed_mul4096x4096_MN (void);

#endif /* SRC_TESTSUITE_VEC_PERF_I512_H_ */
