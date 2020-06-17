/*
 * test_ppc_int128.c
 *
 *  Created on: Jun 15, 2020
 *      Author: sjmunroe
 */

#define GNUC_VERSION (__GNUC__*1000 + __GNUC_MAJOR__*10)
#if (GNUC_VERSION >= 4060) || defined(__clang__)
# pragma GCC diagnostic ignored "-Wdeprecated"
#endif

#include <altivec.h>
int main(int argc, char* argv[])
{
#if defined(__VSX__) && defined(__ALTIVEC__)
	{
	    __vector unsigned __int128 r = {1};
	    __vector signed __int128 s = {-2};
	    return 0;
	}
#else
    int x[-1]; // fail compile
#endif
    return -1;
}

