/*
 Copyright (c) [2016, 2018] IBM Corporation.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 arith128_test_f128.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Apr 18, 2016
 */

/* Collection function for all f128 unit tests.  */

// Already defined in arith128_test_f128.c
extern const vui64_t vf128_zero;
extern const vui64_t vf128_nzero;

extern const vui64_t vf128_one;
extern const vui64_t vf128_none;

extern const vui64_t vf128_two;
extern const vui64_t vf128_ntwo;

extern const vui64_t vf128_max;
extern const vui64_t vf128_nmax;

extern const vui64_t vf128_sub;
extern const vui64_t vf128_nsub;

extern const vui64_t vf128_inf;
extern const vui64_t vf128_ninf;

extern const vui64_t vf128_nan;
extern const vui64_t vf128_snan;

int test_vec_f128 (void);
