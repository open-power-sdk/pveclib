#define GNUC_VERSION (__GNUC__*1000 + __GNUC_MAJOR__*10)
#if (GNUC_VERSION >= 4060) || defined(__clang__)
# pragma GCC diagnostic ignored "-Wdeprecated"
#endif

/* Verify that the __float128 type supports the basic math.h 
   operations.  Currently this is supported for gcc >= 7 and will
   gernerate code inline.
   Clang currently fails whenever it includes math.h if -mfloat128
   is enabled.  */

#include <math.h>

int
test_gcc_f128_signbit (__float128 value)
  {
    return (signbit(value));
  }

int
test_gcc_f128_isfinite (__float128 value)
  {
    return (isfinite(value));
  }

int
test_gcc_f128_isnormal (__float128 value)
  {
    return (isnormal(value));
  }

int
test_gcc_f128_isinf (__float128 value)
  {
    return (isinf(value));
  }

int
test_gcc_float128_isnan (__float128 value)
  {
    return (isnan(value));
  }

__float128
test_gcc_f128_copysign (__float128 valx, __float128 valy)
  {
    return (__builtin_copysignf128(valx, valy));
  }

typedef union
     {
       __vector __int128  vx1;
#if defined(__FLOAT128__)
       __float128 vf1;
#endif
     } __VF_128;

int main(int argc, char* argv[])
{
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) && defined(__FLOAT128__)
	{
	    __VF_128 xxx;
	    xxx.vx1 = (__vector __int128)(__int128)0LL;
	    __float128 fff = xxx.vf1;
	    __float128 ggg = -1.0Q;
	    __float128 hhh = 100000000.0Q;
	    __float128 iii = (fff + ggg) * hhh;
	    __float128 jjj = test_gcc_f128_copysign (iii, ggg);
	    return 0;
	}
#else
    int x[-1]; // fail compile
#endif 
    return -1;
}
