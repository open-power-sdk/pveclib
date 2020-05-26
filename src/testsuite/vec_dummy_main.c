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

 vec_dummy_main.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Nov 15, 2017
 */

#ifndef __clang__
/* Disable for __clang__ because of bug involving <floatn.h>
   incombination with -mcpu=power9 -mfloat128 */
#include <stdlib.h>
#else
#define EXIT_SUCCESS 0
#endif
#include <stdint.h>
#include <stdio.h>

int
main (void)
{
  int rc = EXIT_SUCCESS;

  puts ("Dummy for reviewing generated assembly code\n");

  return (rc);
}
