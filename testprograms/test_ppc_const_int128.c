/*
 * test_ppc_const_int128.c
 *
 *  Created on: Jun 15, 2020
 *      Author: sjmunroe
 */

#define GNUC_VERSION (__GNUC__*1000 + __GNUC_MAJOR__*10)
#if (GNUC_VERSION >= 4060) || defined(__clang__)
# pragma GCC diagnostic ignored "-Wdeprecated"
#endif

/* Current compilers do not support 128-bit integer constants. But most
 * support 64-bit (long long int) constants. If the compiler also
 * supports constant folding on __int128 constants then a little simple
 * arithmetic will generate a 128-bit constant.
 *
 * Apparently __clang__ does not support this.
 */

typedef __vector unsigned int vui32_t;
typedef __vector unsigned long long int vui64_t;
typedef __vector unsigned __int128 vui128_t;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define CONST_VINT128_DW128(__dw0, __dw1) (vui128_t)((vui64_t){__dw1, __dw0})
#else
#define CONST_VINT128_DW128(__dw0, __dw1) (vui128_t)((vui64_t){__dw0, __dw1})
#endif

#define CONST_VUINT128_QxW(__q0, __q1, __q2, __q3) ( (vui128_t) \
      (((unsigned __int128) __q0) << 96) \
    + (((unsigned __int128) __q1) << 64) \
    + (((unsigned __int128) __q2) << 32) \
    +  ((unsigned __int128) __q3) )

/** \brief Generate a vector unsigned __int128 constant from doublewords. */
#define CONST_VUINT128_QxD(__q0, __q1) ( (vui128_t) \
    (((unsigned __int128) __q0) << 64) \
    + ((unsigned __int128) __q1) )

/** \brief Generate a vector unsigned __int128 constant from doublewords. */
#define CONST_VUINT128_Qx19d(__q0, __q1) ( (vui128_t) \
    (((unsigned __int128) __q0) * 10000000000000000000UL) \
    + ((unsigned __int128) __q1) )

/** \brief Generate a vector unsigned __int128 constant from doublewords. */
#define CONST_VUINT128_Qx18d(__q0, __q1) ( (vui128_t) \
    (((unsigned __int128) __q0) * 1000000000000000000UL) \
    + ((unsigned __int128) __q1) )

/** \brief Generate a vector unsigned __int128 constant from doublewords.*/
#define CONST_VUINT128_Qx16d(__q0, __q1) ( (vui128_t) \
    (((unsigned __int128) __q0) * 10000000000000000UL) \
    + ((unsigned __int128) __q1) )


const vui128_t vtipowof10[] = { (vui128_t) (__int128 ) 1ll, /* 10**0 */
  (vui128_t) (__int128 ) 10ll, /* 10**1 */
  (vui128_t) (__int128 ) 100ll, /* 10**2 */
  (vui128_t) (__int128 ) 1000ll, /* 10**3 */
  (vui128_t) (__int128 ) 10000ll, /* 10**4 */
  (vui128_t) (__int128 ) 100000ll, /* 10**5 */
  (vui128_t) (__int128 ) 1000000ll, /* 10**6 */
  (vui128_t) (__int128 ) 10000000ll, /* 10**7 */
  (vui128_t) (__int128 ) 100000000ll, /* 10**8 */
  (vui128_t) (__int128 ) 1000000000ll, /* 10**9 */
  CONST_VUINT128_QxW (0, 0, 0x2, 0x540be400),  /* 10**10 */
  CONST_VUINT128_QxW (0, 0, 0x17, 0x4876e800),  /* 10**11 */
  CONST_VUINT128_QxW (0, 0, 0xe8, 0xd4a51000),  /* 10**12 */
  CONST_VUINT128_QxW (0, 0, 0x918, 0x4e72a000),  /* 10**13 */
  CONST_VUINT128_QxW (0, 0, 0x5af3, 0x107a4000),  /* 10**14 */
  CONST_VUINT128_QxW (0, 0, 0x38d7e, 0xa4c68000),  /* 10**15 */
  CONST_VUINT128_QxW (0, 0, 0x2386f2, 0x6fc10000),  /* 10**16 */
  CONST_VUINT128_QxW (0, 0, 0x1634578, 0x5d8a0000),  /* 10**17 */
  CONST_VUINT128_QxW (0, 0, 0xde086b3, 0xa7640000),  /* 10**18 */
  (vui128_t) (__int128 ) 10ll * (__int128 ) 1000000000000000000ll, /* 10**19 */
  (vui128_t) (__int128 ) 100ll * (__int128 ) 1000000000000000000ll, /* 10**20 */
  (vui128_t) (__int128 ) 1000ll * (__int128 ) 1000000000000000000ll, /* 10**21 */
  (vui128_t) (__int128 ) 10000ll * (__int128 ) 1000000000000000000ll, /* 10**22 */
  (vui128_t) (__int128 ) 100000ll * (__int128 ) 1000000000000000000ll, /* 10**23 */
  (vui128_t) (__int128 ) 1000000ll * (__int128 ) 1000000000000000000ll, /* 10**24 */
  (vui128_t) (__int128 ) 10000000ll * (__int128 ) 1000000000000000000ll, /* 10**25 */
  (vui128_t) (__int128 ) 100000000ll * (__int128 ) 1000000000000000000ll, /* 10**26 */
  (vui128_t) (__int128 ) 1000000000ll * (__int128 ) 1000000000000000000ll, /* 10**27 */
  (vui128_t) (__int128 ) 10000000000ll * (__int128 ) 1000000000000000000ll, /* 10**28 */
  (vui128_t) (__int128 ) 100000000000ll * (__int128 ) 1000000000000000000ll, /* 10**29 */
  (vui128_t) (__int128 ) 1000000000000ll * (__int128 ) 1000000000000000000ll, /* 10**30 */
  (vui128_t) (__int128 ) 10000000000000ll * (__int128 ) 1000000000000000000ll, /* 10**31 */
  CONST_VUINT128_Qx16d (10000000000000000UL, 0), /* 10**32 */
  (vui128_t) (__int128 ) 1000000000000000ll * (__int128 ) 1000000000000000000ll, /* 10**33 */
  (vui128_t) (__int128 ) 10000000000000000ll * (__int128 ) 1000000000000000000ll, /* 10**34 */
  (vui128_t) (__int128 ) 100000000000000000ll * (__int128 ) 1000000000000000000ll, /* 10**35 */
  CONST_VUINT128_Qx18d (1000000000000000000UL, 0), /* 10**36 */
  (vui128_t) (__int128 ) 10ll * (__int128 ) 1000000000000000000ll
           * (__int128 ) 1000000000000000000ll, /* 10**37 */
  CONST_VUINT128_Qx19d (10000000000000000000UL, 0), /* 10**38 */
  (vui128_t) (__int128 ) 0L };/* end marker */

#include <altivec.h>
int
main (int argc, char* argv[])
{
#if defined(__ALTIVEC__) && defined(__VSX__)
  __vector unsigned __int128 r = CONST_VUINT128_QxW(1, 1, 1, 1);
  __vector unsigned __int128 s = CONST_VUINT128_QxD(1UL, 1UL);
  __vector unsigned __int128 t = CONST_VUINT128_Qx19d(9999999999999999999UL,
						      9999999999999999999UL);
  __vector unsigned __int128 u = CONST_VUINT128_Qx18d(999999999999999999UL,
						      999999999999999999UL);
  __vector unsigned __int128 v = CONST_VUINT128_Qx16d(9999999999999999UL,
						      9999999999999999UL);

  __vector unsigned __int128 ten18_hex =
      (vui128_t) (__int128) 1000000000000000000ll; /* 10**18 */
  __vector unsigned __int128 ten36_hex = CONST_VINT128_DW128(
      0x00c097ce7bc90715ll, 0xb34b9f1000000000ll); /* 10**36 */
  __vector unsigned __int128 ten37_hex = CONST_VINT128_DW128(
      0x0785ee10d5da46d9ll, 0x00f436a000000000ll); /* 10**37 */
  __vector unsigned __int128 ten38_hex = CONST_VINT128_DW128(
      0x4b3b4ca85a86c47all, 0x098a224000000000ll); /* 10**38 */

  if (vec_all_eq ( (vui32_t ) ten18_hex, (vui32_t) vtipowof10[18])
      & vec_all_eq ((vui32_t) ten36_hex, (vui32_t) vtipowof10[36])
      & vec_all_eq ((vui32_t) ten37_hex, (vui32_t) vtipowof10[37])
      & vec_all_eq ((vui32_t) ten38_hex, (vui32_t) vtipowof10[38]))
    return 0;
  else
    return -1;
#else
  int x[-1]; // fail compile
  return -1;
#endif
}
