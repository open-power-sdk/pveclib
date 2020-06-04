#define GNUC_VERSION (__GNUC__*1000 + __GNUC_MAJOR__*10)
#if (GNUC_VERSION >= 4060) || defined(__clang__)
# pragma GCC diagnostic ignored "-Wdeprecated"
#endif

int main(int argc, char* argv[])
{
#if defined(_ARCH_PWR6) && defined(__DEC128_MAX__)
	{
	    _Decimal128 r = 1.0E+0DL;
	    _Decimal128 s = __DEC128_MAX__;
	    _Decimal128 w = s - r;
	    _Decimal128 v = r + s;
	    return 0;
	}
#else
    int x[-1]; // fail compile
#endif 
    return -1;
}
