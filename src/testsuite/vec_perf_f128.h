/*
 Copyright (c) [2021] Steven Munroe

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 vec_perf_f128.h

 Contributors: Steven Munroe
 Created on: Apr 2, 2021
*/

#ifndef SRC_TESTSUITE_VEC_PERF_F128_H_
#define SRC_TESTSUITE_VEC_PERF_F128_H_

extern int timed_expxsuba_v1_f128 (void);
extern int timed_expxsuba_v2_f128 (void);

extern int timed_gcc_max8_f128 (void);
extern int timed_lib_max8_f128 (void);
extern int timed_vec_max8_f128 (void);
extern int timed_vec_max8_f128uz (void);

extern int timed_gcc_dpqp_f128 (void);
extern int timed_lib_dpqp_f128 (void);
extern int timed_vec_dpqp_f128 (void);

extern int timed_gcc_uqqp_f128 (void);
extern int timed_lib_uqqp_f128 (void);
extern int timed_vec_uqqp_f128 (void);

extern int timed_gcc_qpuq_f128 (void);
extern int timed_lib_qpuq_f128 (void);
extern int timed_vec_qpuq_f128 (void);

extern int timed_gcc_qpdpo_f128 (void);
extern int timed_lib_qpdpo_f128 (void);
extern int timed_vec_qpdpo_f128 (void);

#endif /* SRC_TESTSUITE_VEC_PERF_F128_H_ */
