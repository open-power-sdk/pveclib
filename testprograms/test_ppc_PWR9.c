#define GNUC_VERSION (__GNUC__*1000 + __GNUC_MAJOR__*10)
#if (GNUC_VERSION >= 4060) || defined(__clang__)
# pragma GCC diagnostic ignored "-Wdeprecated"
#endif

#include <altivec.h>
int main(int argc, char* argv[])
{
#if defined(_ARCH_PWR9) 
#ifdef __BUILTIN_CPU_SUPPORTS__
    if (__builtin_cpu_is ("power9") || __builtin_cpu_supports ("arch_3_00"))
	{
	    // PWR9
	    __vector unsigned long long r = {1, 2};
	    __vector unsigned __int128 x = { 1000UL };
#ifdef __clang__
	    // early clang did not support ctz/prty but did rev
	    __vector unsigned long long w = vec_revb (r);
	    __vector unsigned __int128 z = vec_revb (x);
#else
	    __vector unsigned long long w = vec_vctz(r);
	    __vector unsigned __int128 z = vec_vprtyb (x);
#endif
            return 0;
	}
#else
	{
	    // PWR9
	    __vector unsigned long long r = {1, 2};
	    __vector unsigned __int128 x = { 1000UL };
#ifdef __clang__
	    // early clang did not support ctz/prty but did rev
	    __vector unsigned long long w = vec_revb (r);
	    __vector unsigned __int128 z = vec_revb (x);
#else
	    __vector unsigned long long w = vec_vctz(r);
	    __vector unsigned __int128 z = vec_vprtyb (x);
#endif
            return 0;
	}
#endif
#else
    int x[-1]; // fail compile
#endif 
  return -1;
}
