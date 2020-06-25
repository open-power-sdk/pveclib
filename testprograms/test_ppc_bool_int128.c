/*
 * test_ppc_bool_int128.c
 *
 *  Created on: Jun 15, 2020
 *      Author: sjmunroe
 */

#define GNUC_VERSION (__GNUC__*1000 + __GNUC_MAJOR__*10)
#if (GNUC_VERSION >= 4060) || defined(__clang__)
# pragma GCC diagnostic ignored "-Wdeprecated"
#endif

#include <altivec.h>
int
main (int argc, char* argv[])
{
#if defined(__ALTIVEC__) && defined(__VSX__)
  __vector __bool __int128 r =  { 0 };
#else
  int x[-1]; // fail compile
#endif
  return 0;
}
