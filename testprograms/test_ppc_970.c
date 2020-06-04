#define GNUC_VERSION (__GNUC__*1000 + __GNUC_MAJOR__*10)
#if (GNUC_VERSION >= 4060) || defined(__clang__)
# pragma GCC diagnostic ignored "-Wdeprecated"
#endif

#include <altivec.h>
int main(int argc, char* argv[])
{
#if defined(_ARCH_PWR4) && defined(__ALTIVEC__) && defined(__BIG_ENDIAN__)
#ifdef __BUILTIN_CPU_SUPPORTS__
    if (__builtin_cpu_is ("ppc970") || __builtin_cpu_supports ("altivec"))
	{
	    __vector unsigned int r = {1, 2, 2, 4};
	    __vector unsigned int s = vec_ld (0, (vector unsigned int*)argv[0]);
	    __vector unsigned int w = vec_add (r, s);
	    __vector float x = {1.0, 0.0, 1.0, 0.0};
	    __vector float y = {0.0, 1.0, 0.0, 1.0};
	    __vector float z = vec_sub (y, x);
	    return 0;
	}
#else
	{
	    __vector unsigned int r = {1, 2, 2, 4};
	    __vector unsigned int s = vec_ld (0, (vector unsigned int*)argv[0]);
	    __vector unsigned int w = vec_add (r, s);
	    __vector float x = {1.0, 0.0, 1.0, 0.0};
	    __vector float y = {0.0, 1.0, 0.0, 1.0};
	    __vector float z = vec_sub (y, x);
	    return 0;
	}
#endif
#else
    int x[-1]; // fail compile
#endif 
    return -1;
}
