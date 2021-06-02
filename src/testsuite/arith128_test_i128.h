/*
 Copyright (c) [2017] IBM Corporation.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 arith128_test_i128.h

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: May 10, 2015
 */

#ifndef TEST_ARITH128_TEST_I128_H_
#define TEST_ARITH128_TEST_I128_H_

#ifdef __DEBUG_PRINT__

extern vui128_t
db_shift_rightq (vui128_t vra, vui128_t vrb);

extern vui128_t
db_shift_leftq (vui128_t vra, vui128_t vrb);

extern vui128_t
db_vec_clzq (vui128_t vra);

extern vui32_t
db_vec_addeuqm (vui32_t a, vui32_t b, vui32_t c);

extern vui32_t
db_vec_addeq (vui32_t *cout, vui32_t a, vui32_t b, vui32_t c);

extern vui32_t
db_vec_mulluq (vui32_t a, vui32_t b);

extern vui32_t
db_vec_muludq (vui32_t *mulu, vui32_t a, vui32_t b);
#endif

extern vui128_t
db_vec_mul10uq (vui128_t a);

extern vui128_t
db_vec_mul10cuq (vui128_t a);

extern int
test_addq (void);

extern int
test_mul10uq (void);

extern int
test_mul10ecuq (void);

extern int
test_cmul100 (void);

extern int
test_mulluq (void);

extern int
test_muludq(void);

extern int
test_msumudm (void);

extern vui128_t
__test_muludq (vui128_t *mh, vui128_t a, vui128_t b);

extern vui128_t
example_longdiv_10e32 (vui128_t *q, vui128_t *d, long int _N);

#ifndef PVECLIB_DISABLE_DFP
extern vBCD_t
example_longbcdcf_10e32 (vui128_t *q, vui128_t *d, long int _N);

extern long int
example_longbcdct_10e32 (vui128_t *d, vBCD_t decimal,
			 long int _C , long int _N);
#endif

extern int
test_vec_i128 (void);

#endif /* TEST_ARITH128_TEST_I128_H_ */
