#define GNUC_VERSION (__GNUC__*1000 + __GNUC_MAJOR__*10)
#if (GNUC_VERSION >= 4060) || defined(__clang__)
# pragma GCC diagnostic ignored "-Wdeprecated"
#endif

/* Verify that the __float128 type works for unions and assignment.    
   This allows for transfers between __float128 ands vector types as
   casts between __float128 scalars and vectors of any type are not
   allowed.
   This normally requires -mfloat128.  Later versions of clang may
   also require -mcpu=power9.  */

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
	    return 0;
	}
#else
    int x[-1]; // fail compile
#endif 
    return -1;
}
