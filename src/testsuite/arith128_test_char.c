/*
   arith128_test_char.c
  
    Created on: Oct 25, 2017
        Author: sjmunroe
  */


#define __STDC_WANT_DEC_FP__    1

#include <stdint.h>
#include <stdio.h>
#if 0
#include <dfp/fenv.h>
#include <dfp/float.h>
#include <dfp/math.h>
#else
#include <fenv.h>
#include <float.h>
#include <math.h>
#endif

//#define __DEBUG_PRINT__

#include "arith128.h"
#include <testsuite/arith128_print.h>
#include <vec_char_ppc.h>

#include <testsuite/arith128_test_char.h>

/*
 * Return a vector boolean char with a true indicator for any character
 * that is either Lower Case Alpha ASCII or Upper Case ASCII.
 * False otherwise.
 */
vui8_t
db_vec_isalpha (vui8_t vec_str)
{
	vui8_t result;
	const vui8_t UC_FIRST = {0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40};
	const vui8_t UC_LAST  = {0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a};
	const vui8_t LC_FIRST = {0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60};
	const vui8_t LC_LAST  = {0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a};

	vui8_t cmp1, cmp2, cmp3, cmp4, cmask1, cmask2;

    printf ("db_vec_isalpha\n");
	print_vint8 ("vec_str = ", vec_str);

	cmp1 = (vui8_t)vec_cmpgt (vec_str, LC_FIRST);
	cmp2 = (vui8_t)vec_cmpgt (vec_str, LC_LAST);

	print_vint8 ("cmp1    = ", cmp1);
	print_vint8 ("cmp2    = ", cmp2);

	cmp3 = (vui8_t)vec_cmpgt (vec_str, UC_FIRST);
	cmp4 = (vui8_t)vec_cmpgt (vec_str, UC_LAST);

	print_vint8 ("cmp3    = ", cmp3);
	print_vint8 ("cmp4    = ", cmp4);

	cmask1 = vec_andc (cmp1, cmp2);
	cmask2 = vec_andc (cmp3, cmp4);

	print_vint8 ("lcmask1 = ", cmask1);
	print_vint8 ("ucmask2 = ", cmask2);

	result = vec_or (cmask1, cmask2);

	print_vint8 ("result  = ", result);

	return (result);
}
/*
 * Convert any Lower Case Alpha ASCII characters within a vector
 * unsigned char into the equivalent Upper Case character.
 * Return the result as a vector unsigned char.
 */
vui8_t
db_vec_toupper (vui8_t vec_str)
{
	vui8_t result;
	const vui8_t UC_MASK  = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20};
	const vui8_t LC_FIRST = {0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60};
	const vui8_t LC_LAST  = {0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a};

	vui8_t cmp1, cmp2, cmask;

    printf ("db_vec_tolower\n");
	print_vint8 ("vec_str = ", vec_str);

	cmp1 = (vui8_t)vec_cmpgt (vec_str, LC_FIRST);
	cmp2 = (vui8_t)vec_cmpgt (vec_str, LC_LAST);

	print_vint8 ("cmp1    = ", cmp1);
	print_vint8 ("cmp2    = ", cmp2);

	cmask = vec_andc (cmp1, cmp2);
	cmask = vec_and (cmask, UC_MASK);

	print_vint8 ("cmask   = ", cmask);

	result = vec_andc (vec_str, cmask);

	print_vint8 ("result  = ", result);

	return (result);
}
/*
 * Convert any Upper Case Alpha ASCII characters within a vector
 * unsigned char into the equivalent Lower Case character.
 * Return the result as a vector unsigned char.
 */
vui8_t
db_vec_tolower (vui8_t vec_str)
{
	vui8_t result;
	const vui8_t UC_MASK  = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20};
	const vui8_t UC_FIRST = {0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40};
	const vui8_t UC_LAST  = {0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a};

	vui8_t cmp1, cmp2, cmask;

    printf ("db_vec_tolower\n");
	print_vint8 ("vec_str = ", vec_str);

	cmp1 = (vui8_t)vec_cmpgt (vec_str, UC_FIRST);
	cmp2 = (vui8_t)vec_cmpgt (vec_str, UC_LAST);

	print_vint8 ("cmp1    = ", cmp1);
	print_vint8 ("cmp2    = ", cmp2);

	cmask = vec_andc (cmp1, cmp2);
	cmask = vec_and (cmask, UC_MASK);

	print_vint8 ("cmask   = ", cmask);

	result = vec_or (vec_str, cmask);

	print_vint8 ("result  = ", result);

	return (result);
}

#ifdef __DEBUG_PRINT__
#define test_vec_tolower(_l)	db_vec_tolower(_l)
#define test_vec_toupper(_l)	db_vec_toupper(_l)
#define test_vec_isalpha(_l)	db_vec_isalpha(_l)
#define test_vec_isalnum(_l)	db_vec_isalnum(_l)
#define test_vec_isdigit(_l)	db_vec_isdigit(_l)
#else
#define test_vec_tolower(_l)	vec_tolower(_l)
#define test_vec_toupper(_l)	vec_toupper(_l)
#define test_vec_isalpha(_l)	vec_isalpha(_l)
#define test_vec_isalnum(_l)	vec_isalnum(_l)
#define test_vec_isdigit(_l)	vec_isdigit(_l)
#endif

int
test_vec_ischar (void)
{
    vui8_t i, j, k/*, l, m*/;
    vui8_t e;
    int rc = 0;

    printf ("\ntest_vec_ischar Vector tolower, toupper, ...\n");

    i = (vui8_t){0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44, 0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d};
    e = (vui8_t){0x20, 0x40, 0x7a, 0x5b, 0x61, 0x62, 0x63, 0x64, 0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d};
    k = test_vec_tolower (i);

#ifdef __DEBUG_PRINT__
    print_vchar ("tolower of ", i);
    print_vchar ("         = ", k);
#endif
    rc += check_vui8 ("vec_tolower", k, e);

    e = (vui8_t){0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44, 0x41, 0x42, 0x43, 0x44, 0x31, 0x39, 0x5c, 0x5d};
    j = test_vec_toupper (i);

#ifdef __DEBUG_PRINT__
    print_vchar ("toupper of ", i);
    print_vchar ("         = ", j);
#endif
    rc += check_vui8 ("vec_toupper", j, e);

    e = (vui8_t){0x00, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00};
    k = test_vec_isalpha (i);

#ifdef __DEBUG_PRINT__
    print_vchar  ("isalpha of ", i);
    print_vbool8 ("         = ", k);
#endif
    rc += check_vui8 ("vec_isalpha", k, e);

    e = (vui8_t){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00};
    k = test_vec_isdigit (i);

#ifdef __DEBUG_PRINT__
    print_vchar  ("isdigit of ", i);
    print_vbool8 ("         = ", k);
#endif
    rc += check_vui8 ("vec_isdigit", k, e);

    e = (vui8_t){0x00, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00};
    k = test_vec_isalnum (i);

#ifdef __DEBUG_PRINT__
    print_vchar  ("isalnum of ", i);
    print_vbool8 ("         = ", k);
#endif
    rc += check_vui8 ("vec_isalnum", k, e);

    return (rc);
}

int
test_clzb (void)
{
  vui32_t i, e;
  vui8_t j;
  int rc = 0;

  printf ("\ntest_clzb Vector Count Leading Zeros in bytes\n");

  i = (vui32_t )CONST_VINT32_W(0x00010204, 0x08102040, 0x80882211, 0xf0ffaa55);
  e = (vui32_t )CONST_VINT32_W(0x08070605, 0x04030201, 0x00000203, 0x00000001);
  j = vec_clzb((vui8_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0x00010204, 0x08102040, 0x80882211, 0xf0ffaa55) ", j);
#endif
  rc += check_vuint128x ("vec_clzb:", (vui128_t)j, (vui128_t) e);

  return (rc);
}

int
test_popcntb (void)
{
  vui8_t i, e;
  vui8_t j;
  int rc = 0;

  printf ("\ntest_popcntb Vector Pop Count bytes\n");

  i = (vui8_t){0,1,2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15};
  e = (vui8_t){0,1,1,2, 1,2,2,3, 1,2,2,3, 2,3,3,4};
  j = vec_popcntb((vui8_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntb({0,1,2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15}) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntb:", (vui128_t)j, (vui128_t) e);

  i = (vui8_t){0,1,2,4, 8,16,32,64, 128,136,34,17, 240,255,170,85};
  e = (vui8_t){0,1,1,1, 1,1,1,1, 1,2,2,2, 4,8,4,4};
  j = vec_popcntb((vui8_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntb({0,1,2,4, 8,16,32,64, 128,136,34,17, 240,255,170,85}) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntb:", (vui128_t)j, (vui128_t) e);

  i = (vui8_t){7,224,168,133, 15,240,225,170, 31,248,79,229, 63,245,235,190};
  e = (vui8_t){3,3,3,3, 4,4,4,4, 5,5,5,5, 6,6,6,6};
  j = vec_popcntb((vui8_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntb({7,224,168,133, 15,240,225,170, 31,248,79,229, 63,245,235,190}) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntb:", (vui128_t)j, (vui128_t) e);

  i = (vui8_t){127,251,223,239, 255,0,255,0, 24,130,138,176, 49,165,23,60};
  e = (vui8_t){7,7,7,7, 8,0,8,0, 2,2,3,3, 3,4,4,4};
  j = vec_popcntb((vui8_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntb({127,251,223,239, 255,0,255,0, 24,130,138,176, 49,165,23,60}) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntb:", (vui128_t)j, (vui128_t) e);

  return (rc);
}

int
test_vec_char (void)
{
  int rc = 0;

  printf ("\ntest_vec_char/i8\n");
#if 1
  rc += test_clzb ();
  rc += test_popcntb();
  rc += test_vec_ischar();
#endif
  return (rc);
}
