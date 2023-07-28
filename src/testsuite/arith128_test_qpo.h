/*
 Copyright (c) [2023] IBM Corporation.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 arith128_test_qpo.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Jul 26, 2023
 */

  extern int test_add_qpo ();
  extern int test_add_qpo_xtra ();

  extern int test_mul_qpo ();
  extern int test_mul_qpo_xtra ();

  extern int test_sub_qpo ();
  extern int test_sub_qpo_xtra ();

  extern int test_madd_qpo ();
  extern int test_madd_qpo_zero_c ();
  extern int test_madd_qpo_xtra ();

  extern int test_msub_qpo ();
  extern int test_msub_qpo_zero_c ();
  extern int test_msub_qpo_xtra ();
