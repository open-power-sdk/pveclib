#define GNUC_VERSION (__GNUC__*1000 + __GNUC_MAJOR__*10)
#if (GNUC_VERSION >= 4060) || defined(__clang__)
# pragma GCC diagnostic ignored "-Wdeprecated"
#endif

#include <altivec.h>
int main(int argc, char* argv[])
{
#if defined(_ARCH_PWR7) && defined(__BIG_ENDIAN__)
#ifdef __BUILTIN_CPU_SUPPORTS__
    if (__builtin_cpu_is ("power7") || __builtin_cpu_supports ("arch_2_06"))
	{
	    __vector unsigned long long r = {1, 2};
	    __vector unsigned int s = vec_xl(0, (unsigned int*)argv[0]);  // Power7
	    __vector unsigned long long w = (__vector unsigned long long)r;
	    __vector unsigned long long x = (__vector unsigned long long)s;
	    __vector unsigned long long y = vec_xor(w, x);
	    __vector unsigned long long z = vec_xxpermdi(y, x, 0);
	    return 0;
	}
#else
	{
	    __vector unsigned long long r = {1, 2};
	    __vector unsigned int s = vec_xl(0, (unsigned int*)argv[0]);  // Power7
	    __vector unsigned long long w = (__vector unsigned long long)r;
	    __vector unsigned long long x = (__vector unsigned long long)s;
	    __vector unsigned long long y = vec_xor(w, x);
	    __vector unsigned long long z = vec_xxpermdi(y, x, 0);
	    return 0;
	}
#endif
#else
    int x[-1]; // fail compile
#endif 
    return -1;
}
