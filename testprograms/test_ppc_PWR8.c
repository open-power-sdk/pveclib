#define GNUC_VERSION (__GNUC__*1000 + __GNUC_MAJOR__*10)
#if (GNUC_VERSION >= 4060) || defined(__clang__)
# pragma GCC diagnostic ignored "-Wdeprecated"
#endif

#include <altivec.h>
int main(int argc, char* argv[])
{
#if defined(_ARCH_PWR8) 
#ifdef __BUILTIN_CPU_SUPPORTS__
    if (__builtin_cpu_is ("power8") || __builtin_cpu_supports ("arch_2_07"))
	{
	    // PWR8
	    __vector unsigned long long r = {1, 2};
	    __vector unsigned int s = vec_xl(0, (unsigned int*)argv[0]);  // Power7
	    __vector unsigned long long w = (__vector unsigned long long)r;
	    __vector unsigned long long x = (__vector unsigned long long)s;
	    __vector unsigned long long y = vec_xor(w, x);
	    __vector unsigned long long z = vec_add(y, vec_add(w, x));
	    __vector unsigned __int128 x2 = { 1000UL };
	    __vector unsigned __int128 y2 = { 1000000UL };
	    __vector unsigned __int128 z2 = vec_vadduqm (x2, y2);
            return 0;
	}
#else
	{
	    // PWR8
	    __vector unsigned long long r = {1, 2};
	    __vector unsigned int s = vec_xl(0, (unsigned int*)argv[0]);  // Power7
	    __vector unsigned long long w = (__vector unsigned long long)r;
	    __vector unsigned long long x = (__vector unsigned long long)s;
	    __vector unsigned long long y = vec_xor(w, x);
	    __vector unsigned long long z = vec_add(y, vec_add(w, x));
	    __vector unsigned __int128 x2 = { 1000UL };
	    __vector unsigned __int128 y2 = { 1000000UL };
	    __vector unsigned __int128 z2 = vec_vadduqm (x2, y2);
            return 0;
	}
#endif
#else
    int x[-1]; // fail compile
#endif 
    return -1;
}
