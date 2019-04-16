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

 vec_perf_i128.h

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Jun 21, 2018
 */

#ifndef TESTSUITE_VEC_PERF_I128_H_
#define TESTSUITE_VEC_PERF_I128_H_

extern int timed_mul10uq (void);
extern int timed_mul10uq2x (void);
extern int timed_cmul10ecuq (void);
extern int timed_mulluq (void);
extern int timed_muludq (void);
extern int timed_muludqx (void);
extern int timed_longdiv_e32 (void);
extern int timed_longbcdcf_10e32 (void);
extern int timed_maxdouble_10e32 (void);

#endif /* TESTSUITE_VEC_PERF_I128_H_ */
