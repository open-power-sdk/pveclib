#define GNUC_VERSION (__GNUC__*1000 + __GNUC_MAJOR__*10)
#if (GNUC_VERSION >= 14010) || defined(__clang__)
# pragma GCC diagnostic ignored "-Wdeprecated"
#endif

#include <altivec.h>
int main(int argc, char* argv[])
{
#if defined(_ARCH_PWR10) 
#ifdef __BUILTIN_CPU_SUPPORTS__
    if (__builtin_cpu_is ("power10") || __builtin_cpu_supports ("arch_3_1"))
	{
	    __vector unsigned long long vra = { 123, 456 };
	    __vector unsigned long long vrb = { 789, 1023 };
	    __vector unsigned long long vrt, vrt_e,  vrt_o;

		__asm__(
		"vmuleud %0,%1,%2;\n"
		: "=v" (vrt_e)
		: "v" (vra), "v" (vrb)
		: );

		__asm__(
		"vmuloud %0,%1,%2;\n"
		: "=v" (vrt_o)
		: "v" (vra), "v" (vrb)
		: );

		__asm__(
		"vadduqm %0,%1,%2;\n"
		: "=v" (vrt)
		: "v" (vrt_e), "v" (vrt_o)
		: );
            return 0;
	}
#else
	{
	    __vector unsigned long long vra = { 123, 456 };
	    __vector unsigned long long vrb = { 789, 1023 };
	    __vector unsigned long long vrt, vrt_e,  vrt_o;

		__asm__(
		"vmuleud %0,%1,%2;\n"
		: "=v" (vrt_e)
		: "v" (vra), "v" (vrb)
		: );

		__asm__(
		"vmuloud %0,%1,%2;\n"
		: "=v" (vrt_o)
		: "v" (vra), "v" (vrb)
		: );

		__asm__(
		"vadduqm %0,%1,%2;\n"
		: "=v" (vrt)
		: "v" (vrt_e), "v" (vrt_o)
		: );
            return 0;
	}
#endif
#else
    int x[-1]; // fail compile
#endif 
  return -1;
}
