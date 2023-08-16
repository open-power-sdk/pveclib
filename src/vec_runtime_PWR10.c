/*
 Copyright (c) [2020] Steven Munroe.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 vec_runtime_PWR10.c

 Contributors:
      Steven Munroe
      Created on: Nov 24, 2020
 */

#ifndef PVECLIB_DISABLE_POWER10
/* Older Linux distros running Big Endian are unlikely to support
   PWR10.  */

#include "vec_int512_runtime.c"
#include "vec_f128_runtime.c"
#endif


