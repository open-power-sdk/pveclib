#define GNUC_VERSION (__GNUC__*1000 + __GNUC_MAJOR__*10)
#if (GNUC_VERSION >= 4060) || defined(__clang__)
# pragma GCC diagnostic ignored "-Wdeprecated"
#endif

/* Verify that the __float128 type supports the basic arithmetic
   operations.  This allows for inline generation of Quad
   Floating-Point add/subtract/muliply/divide instruction on POWER9.
   Currently this is supported for gcc with -mcpu=power9 -mfloat128.
   Clang seems to only generate calls to the Soft-FP runtime.  */

typedef union
     {
       __vector __int128  vx1;
#if defined(__FLOAT128__)
       __float128 vf1;
#endif
     } __VF_128;

int main(int argc, char* argv[])
{
#if defined(__FLOAT128__)
	{
	    __VF_128 xxx;
	    xxx.vx1 = (__vector __int128)(__int128)0LL;
	    __float128 fff = xxx.vf1;
	    __float128 ggg = 1.0Q;
	    __float128 hhh = 100000000.0Q;
	    __float128 iii = (fff + ggg) * hhh;
	    return 0;
	}
#else
    int x[-1]; // fail compile
#endif 
    return -1;
}
