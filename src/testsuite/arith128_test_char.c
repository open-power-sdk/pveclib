/*
   arith128_test_char.c
  
    Created on: Oct 25, 2017
        Author: sjmunroe
  */


#define __STDC_WANT_DEC_FP__    1

#include <stdint.h>
#include <stdio.h>

//#define __DEBUG_PRINT__

#include "arith128.h"
#include <testsuite/arith128_print.h>
#include <pveclib/vec_char_ppc.h>

#include <testsuite/arith128_test_char.h>

/*
 * Return a vector boolean char with a true indicator for any character
 * that is either Lower Case Alpha ASCII or Upper Case Alpha ASCII.
 * False otherwise.
 */
vui8_t
db_vec_isalpha (vui8_t vec_str)
{
  vui8_t result;
  const vui8_t UC_FIRST =
    { 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x40 };
  const vui8_t UC_LAST =
    { 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a,
	0x5a, 0x5a, 0x5a, 0x5a };
  const vui8_t LC_FIRST =
    { 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60,
	0x60, 0x60, 0x60, 0x60 };
  const vui8_t LC_LAST =
    { 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a,
	0x7a, 0x7a, 0x7a, 0x7a };

  vui8_t cmp1, cmp2, cmp3, cmp4, cmask1, cmask2;

  printf ("db_vec_isalpha\n");
  print_vint8 ("vec_str = ", vec_str);

  cmp1 = (vui8_t) vec_cmpgt (vec_str, LC_FIRST);
  cmp2 = (vui8_t) vec_cmpgt (vec_str, LC_LAST);

  print_vint8 ("cmp1    = ", cmp1);
  print_vint8 ("cmp2    = ", cmp2);

  cmp3 = (vui8_t) vec_cmpgt (vec_str, UC_FIRST);
  cmp4 = (vui8_t) vec_cmpgt (vec_str, UC_LAST);

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
  const vui8_t UC_MASK =
    { 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20 };
  const vui8_t LC_FIRST =
    { 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60,
	0x60, 0x60, 0x60, 0x60 };
  const vui8_t LC_LAST =
    { 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a, 0x7a,
	0x7a, 0x7a, 0x7a, 0x7a };

  vui8_t cmp1, cmp2, cmask;

  printf ("db_vec_tolower\n");
  print_vint8 ("vec_str = ", vec_str);

  cmp1 = (vui8_t) vec_cmpgt (vec_str, LC_FIRST);
  cmp2 = (vui8_t) vec_cmpgt (vec_str, LC_LAST);

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
  const vui8_t UC_MASK =
    { 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20 };
  const vui8_t UC_FIRST =
    { 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x40 };
  const vui8_t UC_LAST =
    { 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a,
	0x5a, 0x5a, 0x5a, 0x5a };

  vui8_t cmp1, cmp2, cmask;

  printf ("db_vec_tolower\n");
  print_vint8 ("vec_str = ", vec_str);

  cmp1 = (vui8_t) vec_cmpgt (vec_str, UC_FIRST);
  cmp2 = (vui8_t) vec_cmpgt (vec_str, UC_LAST);

  print_vint8 ("cmp1    = ", cmp1);
  print_vint8 ("cmp2    = ", cmp2);

  cmask = vec_andc (cmp1, cmp2);
  cmask = vec_and (cmask, UC_MASK);

  print_vint8 ("cmask   = ", cmask);

  result = vec_or (vec_str, cmask);

  print_vint8 ("result  = ", result);

  return (result);
}

long long int
db_vec_clzlsbb (vui8_t vra)
{
  long long int result;
#ifdef _ARCH_PWR9
#ifdef vec_cntlz_lsbb
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  result = vec_cnttz_lsbb (vra);
#else
  result = vec_cntlz_lsbb (vra);
#endif
#else
  __asm__(
      "vclzlsbb %0,%1;"
      : "=r" (result)
      : "v" (vra)
      : );
#endif
#elif _ARCH_PWR8
  vui8_t gbb;
  vui16_t lsbb, clzls;
  const vui8_t VEOS = vec_splat_u8(0);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  const vui8_t pgbb = CONST_VINT128_B (0x10, 0x10, 0x10, 0x10,
				       0x10, 0x10, 0x08, 0x00,
				       0x10, 0x10, 0x10, 0x10,
				       0x10, 0x10, 0x10, 0x10);
#else
  const vui8_t pgbb = CONST_VINT128_B (0x10, 0x10, 0x10, 0x10,
				       0x10, 0x10, 0x07, 0x0F,
				       0x10, 0x10, 0x10, 0x10,
				       0x10, 0x10, 0x10, 0x10);
#endif

  printf ("db_vec_vclzlsbb\n");
  print_vint8x ("vra   = ", vra);
  print_vint8x ("pgbb  = ", pgbb);
  lsbb = (vui16_t) vec_perm (vra, VEOS, pgbb);
  print_vint8x ("perm  = ", (vui8_t) lsbb);
  //lsbb = (vui16_t) vec_perm (VEOS, vra, pgbb);
  //print_vint8x ("perm  = ", (vui8_t) lsbb);
  gbb = vec_gb (vra);
  print_vint8x ("gbb   = ", gbb);
  lsbb = (vui16_t) vec_perm (gbb, VEOS, pgbb);
  print_vint8x ("lsbb  = ", (vui8_t) lsbb);
  clzls = vec_cntlz (lsbb);
  print_vint8x ("clzls = ", (vui8_t) clzls);
  result = ((vui64_t) clzls) [VEC_DW_H];
  result = (unsigned short) result;
#else
  const vui8_t zeros = vec_splat_u8(0);
  const vui8_t LSBBmask = vec_splat_u8(1);
  const vui8_t LSBBshl = CONST_VINT128_B (3, 2, 1, 0, 3, 2, 1, 0,
					  3, 2, 1, 0, 3, 2, 1, 0);
  const vui32_t LSBWshl = CONST_VINT128_W (12, 8, 4, 0);
  vui8_t gbb, gbbsb;
  vui32_t gbbsw;

  printf ("db_vec_vclzlsbb\n");
  print_vint8x ("vra     = ", vra);
  print_vint8x ("LSBBshl = ", LSBBshl);

  gbb = vec_and (vra, LSBBmask);
  print_vint8x ("gbb   = ", gbb);
  // merge lsbb into nibbles by word
  gbbsb = vec_sl (gbb, LSBBshl);
  print_vint8x ("gbbsb = ", gbbsb);
  gbbsw = vec_sum4s (gbbsb, (vui32_t) zeros);
  print_vint8x ("gbbsw = ", (vui8_t) gbbsw);
  // merge lsbw into halfword by word
  gbbsw = vec_sl (gbbsw, LSBWshl);
  print_vint8x ("gbbsw = ", (vui8_t) gbbsw);
  gbbsw = (vui32_t) vec_sums ((vi32_t) gbbsw, (vi32_t) zeros);
  print_vint8x ("gbbsw = ", (vui8_t) gbbsw);
  // transfer from vector to GPR
  result = ((vui64_t) gbbsw) [VEC_DW_L];
  // Use GCC Builtin to get final leading zero count
  // with fake unsigned short clz
  result = __builtin_clz ((unsigned int) (result)) - 16;

#endif
  return result;
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
    vui8_t i, j, k, e;
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
test_ctzb (void)
{
  vui32_t i, e;
  vui8_t j;
  int rc = 0;

  printf ("\ntest_ctzb Vector Count Trailing Zeros in bytes\n");

  i = (vui32_t )CONST_VINT32_W(0x00010204, 0x08102040, 0x80882211, 0xf0ffaa55);
  e = (vui32_t )CONST_VINT32_W(0x08000102, 0x03040506, 0x07030100, 0x04000100);
  j = vec_ctzb((vui8_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("ctz(0x00010204, 0x08102040, 0x80882211, 0xf0ffaa55) ", j);
#endif
  rc += check_vuint128x ("vec_ctzb:", (vui128_t)j, (vui128_t) e);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
#if 1
#if 0
// test directly from vec_char_ppc.h
#define test_popcnt_b(_l)	vec_popcntb(_l)
#else
// test from vec_char_ppc.h via vec_char_dummy.c
extern vui8_t test_vec_popcntb (vui8_t);
#define test_popcnt_b(_l)	test_vec_popcntb(_l)
#endif
#else
// test from vec_char_dummy.c
extern vui8_t test_vec_popcntb_PWR7 (vui8_t);
#define test_popcnt_b(_j)	test_vec_popcntb_PWR7(_j)
#endif
int
test_popcntb (void)
{
  vui8_t i, e;
  vui8_t j;
  int rc = 0;

  printf ("\ntest_popcntb Vector Pop Count bytes\n");

  i = (vui8_t){0,1,2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15};
  e = (vui8_t){0,1,1,2, 1,2,2,3, 1,2,2,3, 2,3,3,4};
  j = test_popcnt_b((vui8_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntb({0,1,2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15}) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntb:", (vui128_t)j, (vui128_t) e);

  i = (vui8_t){0,1,2,4, 8,16,32,64, 128,136,34,17, 240,255,170,85};
  e = (vui8_t){0,1,1,1, 1,1,1,1, 1,2,2,2, 4,8,4,4};
  j = test_popcnt_b((vui8_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntb({0,1,2,4, 8,16,32,64, 128,136,34,17, 240,255,170,85}) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntb:", (vui128_t)j, (vui128_t) e);

  i = (vui8_t){7,224,168,133, 15,240,225,170, 31,248,79,229, 63,245,235,190};
  e = (vui8_t){3,3,3,3, 4,4,4,4, 5,5,5,5, 6,6,6,6};
  j = test_popcnt_b((vui8_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntb({7,224,168,133, 15,240,225,170, 31,248,79,229, 63,245,235,190}) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntb:", (vui128_t)j, (vui128_t) e);

  i = (vui8_t){127,251,223,239, 255,0,255,0, 24,130,138,176, 49,165,23,60};
  e = (vui8_t){7,7,7,7, 8,0,8,0, 2,2,3,3, 3,4,4,4};
  j = test_popcnt_b((vui8_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntb({127,251,223,239, 255,0,255,0, 24,130,138,176, 49,165,23,60}) ", (vui128_t)j);
#endif
  rc += check_vuint128x ("vec_popcntb:", (vui128_t)j, (vui128_t) e);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_mrgeob (void)
{
  vui8_t i, j, k, e;
  int rc = 0;

  printf ("\ntest_mrgeob Vector Merge Even/Odd Bytes\n");

  i = (vui8_t) { 10, 1, 20, 2, 30, 3, 40, 4,
		50, 5, 60, 6, 70, 7, 80, 8 };
  j = (vui8_t) { 90, 9, 160, 10, 176, 11, 192, 12,
                208, 13, 224, 14, 240, 15, 255, 0 };
  e = (vui8_t) { 10, 90, 20, 160, 30, 176, 40, 192,
		 50, 208, 60, 224, 70, 240, 80, 255 };
  k = vec_mrgeb (i, j);

#ifdef __DEBUG_PRINT__
  print_vint8d ("mrgeb(\t{", i);
  print_vint8d ("\t\t{", j);
  print_vint8d ("\t\t{", k);
#endif
  rc += check_vuint128x ("vec_mrgeb:", (vui128_t) k, (vui128_t) e);

  e = (vui8_t)
	  { 1, 9, 2, 10, 3, 11, 4, 12, 5, 13, 6, 14, 7, 15, 8, 0 };
  k = vec_mrgob (i, j);

#ifdef __DEBUG_PRINT__
  print_vint8d ("mrgob(\t{", i);
  print_vint8d ("\t\t{", j);
  print_vint8d ("\t\t{", k);
#endif
  rc += check_vuint128x ("vec_mrgob:", (vui128_t) k, (vui128_t) e);

  return (rc);
}

int
test_mrgahlb (void)
{
  vui16_t i, j;
  vui8_t k, e;
  int rc = 0;

  printf ("\ntest_mrgahlb Vector Merge Algebraic High/Low Bytes\n");

  i = (vui16_t)CONST_VINT128_H(0xf101, 0xf202, 0xf303, 0xf404, 0xf505, 0xf606, 0xf707, 0xf808);
  j = (vui16_t)CONST_VINT128_H(0xf909, 0xfa0a, 0xfb0b, 0xfc0c, 0xfd0d, 0xfe0e, 0xff0f, 0xf000);
  e = (vui8_t)(vui16_t)CONST_VINT128_H(0xf1f9, 0xf2fa, 0xf3fb, 0xf4fc,
			      0xf5fd, 0xf6fe, 0xf7ff, 0xf8f0);
  k = vec_mrgahb(i, j);

#ifdef __DEBUG_PRINT__
  print_vint16x ("mrgahb(\t{", i);
  print_vint16x ("\t\t{", j);
  print_vint8x  ("\t\t{", k);
#endif
  rc += check_vuint128x ("vec_mrgahb:", (vui128_t)k, (vui128_t) e);

  e = (vui8_t)(vui16_t)CONST_VINT128_H(0x0109, 0x020a, 0x030b, 0x040c,
			      0x050d, 0x060e, 0x070f, 0x0800);
  k = vec_mrgalb(i, j);

#ifdef __DEBUG_PRINT__
  print_vint16x ("mrgalb(\t{", i);
  print_vint16x ("\t\t{", j);
  print_vint8x  ("\t\t{", k);
#endif
  rc += check_vuint128x ("vec_mrgalb:", (vui128_t)k, (vui128_t) e);

  return (rc);
}

int
test_mulhub (void)
{
  vui8_t i, j;
  vui8_t k, e;
  int rc = 0;

  printf ("\ntest_mulhub Vector Multiply High Unsigned Bytes\n");

  // Same as vec_splats ((unsigned char)255), only faster
  i = (vui8_t) vec_splat_u8 (-1);
  j = (vui8_t)
	  { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
  e = (vui8_t)
	  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
  k = vec_mulhub (i, j);

#ifdef __DEBUG_PRINT__
  print_vint8x ("mulhub(\t{", i);
  print_vint8x ("\t\t{", j);
  print_vint8x ("\t\t{", k);
#endif
  rc += check_vuint128x ("vec_mulhub:", (vui128_t) k, (vui128_t) e);

  i = (vui8_t)
	  { 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
	      0x10, 0x10, 0x10, 0x10, 0x10 };
  j = (vui8_t)
	  { 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xa0, 0xb0,
	      0xc0, 0xd0, 0xe0, 0xf0, 0x00, };
  e = (vui8_t)
	  { 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd,
              0xe, 0xf, 0x0 };
  k = vec_mulhub (i, j);

#ifdef __DEBUG_PRINT__
  print_vint8x ("mulhub(\t{", i);
  print_vint8x ("\t\t{", j);
  print_vint8x ("\t\t{", k);
#endif
  rc += check_vuint128x ("vec_mulhub:", (vui128_t) k, (vui128_t) e);

  i = (vui8_t)
	  { 100, 100, 100, 100, 100, 100, 100, 100,
            255, 255, 255, 255, 255, 255, 255, 255 };
  j = (vui8_t)
	  { 85, 51, 42, 36, 25, 23, 19, 17,
            85, 51, 42, 36, 25, 23, 19, 17 };
  e = (vui8_t)
	  { 33, 19, 16, 14, 9, 8, 7, 6,
	    84, 50, 41, 35, 24, 22, 18, 16 };
  k = vec_mulhub (i, j);

#ifdef __DEBUG_PRINT__
  print_vint8d ("mulhub(\t{", i);
  print_vint8d ("\t\t{", j);
  print_vint8d ("\t\t{", k);
#endif
  rc += check_vuint128x ("vec_mulhub:", (vui128_t) k, (vui128_t) e);

  i = (vui8_t)
	  { 100, 100, 100, 100, 100, 100, 100, 100,
            255, 255, 255, 255, 255, 255, 255, 255 };
  j = (vui8_t)
	  { 255, 128, 127, 64, 63, 32, 31, 16,
	    255, 128, 127, 64, 63, 32, 31, 16 };
  e = (vui8_t)
          { 99, 50, 49, 25, 24, 12, 12, 6,
           254, 127, 126, 63, 62, 31, 30, 15 };
  k = vec_mulhub (i, j);

#ifdef __DEBUG_PRINT__
  print_vint8d ("mulhub(\t{", i);
  print_vint8d ("\t\t{", j);
  print_vint8d ("\t\t{", k);
#endif
  rc += check_vuint128x ("vec_mulhub:", (vui128_t) k, (vui128_t) e);

  return (rc);
}

int
test_mulhsb (void)
{
  vi8_t i, j;
  vi8_t k, e;
  int rc = 0;

  printf ("\ntest_mulhsb Vector Multiply High Signed Bytes\n");

  i = (vi8_t)
	  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
  j = (vi8_t)
	  { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
  e = (vi8_t)
	  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
  k = vec_mulhsb (i, j);

#ifdef __DEBUG_PRINT__
  print_vint8x ("mulhsb(\t{", (vui8_t) i);
  print_vint8x ("\t\t{", (vui8_t) j);
  print_vint8x ("\t\t{", (vui8_t) k);
#endif
  rc += check_vuint128x ("vec_mulhsb:", (vui128_t) k, (vui128_t) e);

  i = (vi8_t)
	  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
  j = (vi8_t)
	  { -1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -11, -12, -13, -14, -15,
	      -16 };
  e = (vi8_t)
	  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  k = vec_mulhsb (i, j);

#ifdef __DEBUG_PRINT__
  print_vint8x ("mulhsb(\t{", (vui8_t) i);
  print_vint8x ("\t\t{", (vui8_t) j);
  print_vint8x ("\t\t{", (vui8_t) k);
#endif
  rc += check_vuint128x ("vec_mulhsb:", (vui128_t) k, (vui128_t) e);

  i = (vi8_t)
	  { 16, 16, 16, 16, 16, 16, 16, 16, 8, 8, 8, 8, 8, 8, 8, 8 };
  j = (vi8_t)
	  { 16, 32, 48, 64, 80, 96, 112, 127, 16, 32, 48, 64, 80, 96, 112,
		  127 };
  e = (vi8_t)
	  { 1, 2, 3, 4, 5, 6, 7, 7, 0, 1, 1, 2, 2, 3, 3, 3 };
  k = vec_mulhsb (i, j);

#ifdef __DEBUG_PRINT__
  print_vint8x ("mulhsb(\t{", (vui8_t) i);
  print_vint8x ("\t\t{", (vui8_t) j);
  print_vint8x ("\t\t{", (vui8_t) k);
#endif
  rc += check_vuint128x ("vec_mulhsb:", (vui128_t) k, (vui128_t) e);

  i = (vi8_t)
	  { -16, -16, -16, -16, -16, -16, -16, -16, -8, -8, -8, -8, -8, -8, -8,
	      -8 };
  j = (vi8_t)
	  { 16, 32, 48, 64, 80, 96, 112, 127, 16, 32, 48, 64, 80, 96, 112,
		  127 };
  e = (vi8_t)
	  { -1, -2, -3, -4, -5, -6, -7, -8, -1, -1, -2, -2, -3, -3, -4, -4 };
  k = vec_mulhsb (i, j);

#ifdef __DEBUG_PRINT__
  print_vint8x ("mulhsb(\t{", (vui8_t) i);
  print_vint8x ("\t\t{", (vui8_t) j);
  print_vint8x ("\t\t{", (vui8_t) k);
#endif
  rc += check_vuint128x ("vec_mulhsb:", (vui128_t) k, (vui128_t) e);

  i = (vi8_t)
	  { -16, -16, -16, -16, -16, -16, -16, -16, -8, -8, -8, -8, -8, -8, -8,
	      -8 };
  j = (vi8_t)
	  { -16, -32, -48, -64, -80, -96, -112, -127, -16, -32, -48, -64, -80,
	      -96, -112, -127 };
  e = (vi8_t)
	  { 1, 2, 3, 4, 5, 6, 7, 7, 0, 1, 1, 2, 2, 3, 3, 3 };
  k = vec_mulhsb (i, j);

#ifdef __DEBUG_PRINT__
  print_vint8x ("mulhsb(\t{", (vui8_t) i);
  print_vint8x ("\t\t{", (vui8_t) j);
  print_vint8x ("\t\t{", (vui8_t) k);
#endif
  rc += check_vuint128x ("vec_mulhsb:", (vui128_t) k, (vui128_t) e);

  return (rc);
}

int
test_mulubm (void)
{
  vui8_t i, j, k, e;
  int rc = 0;

  printf ("\ntest_mulubm Vector Multiply Unsigned Byte Modulo\n");

  i = (vui8_t) { 1, 2, 3, 4, 5, 6, 7, 8,
		 9, 10, 11, 12, 13, 14, 15, 16 };
  j = (vui8_t) { 0, 1, 2, 3, 4, 5, 6, 7,
		 8, 9, 10, 11, 12, 13, 14, 15 };
  e = (vui8_t) { 0, 2, 6, 12, 20, 30, 42, 56,
		 72, 90, 110, 132, 156, 182, 210, 240};
  k = vec_mulubm (i, j);

#ifdef __DEBUG_PRINT__
  print_vint8x ("mulubm(\t{", i);
  print_vint8x ("\t\t{", j);
  print_vint8x ("\t\t{", k);
#endif
  rc += check_vuint128x ("vec_mulubm:", (vui128_t)k, (vui128_t) e);

  i = (vui8_t) { -1, -2, -3, -4, -5, -6, -7, -8,
		 -9, -10, -11, -12, -13, -14, -15, -16 };
  j = (vui8_t) { 0, 1, 2, 3, 4, 5, 6, 7,
		 8, 9, 10, 11, 12, 13, 14, 15 };
  e = (vui8_t) { 0, -2, -6, -12, -20, -30, -42, -56,
		 -72, -90, -110, 0x7c, 0x64, 0x4a, 0x2e, 0x10};
  k = vec_mulubm (i, j);

#ifdef __DEBUG_PRINT__
  print_vint8x ("mulubm(\t{", i);
  print_vint8x ("\t\t{", j);
  print_vint8x ("\t\t{", k);
#endif
  rc += check_vuint128x ("vec_mulubm:", (vui128_t)k, (vui128_t) e);

  i = (vui8_t) { 1, 2, 3, 4, 5, 6, 7, 8,
		 9, 10, 11, 12, 13, 14, 15, 16 };
  j = (vui8_t) { 0, -1, -2, -3, -4, -5, -6, -7,
		 -8, -9, -10, -11, -12, -13, -14, -15 };
  e = (vui8_t) { 0, -2, -6, -12, -20, -30, -42, -56,
		 -72, -90, -110, 0x7c, 0x64, 0x4a, 0x2e, 0x10};
  k = vec_mulubm (i, j);

#ifdef __DEBUG_PRINT__
  print_vint8x ("mulubm(\t{", i);
  print_vint8x ("\t\t{", j);
  print_vint8x ("\t\t{", k);
#endif
  rc += check_vuint128x ("vec_mulubm:", (vui128_t)k, (vui128_t) e);

  i = (vui8_t) { -1, -2, -3, -4, -5, -6, -7, -8,
		 -9, -10, -11, -12, -13, -14, -15, -16 };
  j = (vui8_t) { 0, -1, -2, -3, -4, -5, -6, -7,
		 -8, -9, -10, -11, -12, -13, -14, -15 };
  e = (vui8_t) { 0, 2, 6, 12, 20, 30, 42, 56,
		 72, 90, 110, 132, 156, 182, 210, 240};
  k = vec_mulubm (i, j);

#ifdef __DEBUG_PRINT__
  print_vint8x ("mulubm(\t{", i);
  print_vint8x ("\t\t{", j);
  print_vint8x ("\t\t{", k);
#endif
  rc += check_vuint128x ("vec_mulubm:", (vui128_t)k, (vui128_t) e);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_setbb (void)
{
  vi8_t i;
  vui8_t e, k;
  int rc = 0;

  printf ("\ntest_setbb Vector Set Bool Byte\n");

  i = (vi8_t)CONST_VINT8_B(0xff, 0x7f, 0x3f, 0x1f,
			   0x0f, 0x07, 0x03, 0x01,
			   0x01, 0x02, 0x04, 0x08,
			   0x10, 0x20, 0x40, 0x80);
  k = (vui8_t) vec_setb_sb ( i );

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_setb ", (vui128_t) i);
  print_vint128x ("        =", (vui128_t) k);
#endif
  e = (vui8_t)CONST_VINT8_B( -1, 0, 0, 0, 0, 0, 0, 0,
			      0, 0, 0, 0, 0, 0, 0,-1);
  rc += check_vuint128x ("vec_setb_sh:", (vui128_t) k, (vui128_t) e);

  i = (vi8_t)CONST_VINT8_B(-1, -1, -1, -1, -1, -1, -1, -1,
			   -1, -1, -1, -1, -1, -1, -1, -1);
  k = (vui8_t) vec_setb_sb ( i );

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_setb ", (vui128_t) i);
  print_vint128x ("        =", (vui128_t) k);
#endif
  e = (vui8_t)CONST_VINT8_B(-1, -1, -1, -1, -1, -1, -1, -1,
			    -1, -1, -1, -1, -1, -1, -1, -1);
  rc += check_vuint128x ("vec_setb_sb:", (vui128_t) k, (vui128_t) e);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
#if 1
// test from vec_char_ppc.h via vec_char_dummy.c
extern vb8_t test_vec_vcmpnezb (vui8_t, vui8_t);
#define test_cmpnezb(_l,_k)	test_vec_vcmpnezb(_l,_k)
#else
// test from vec_char_dummy.c
extern vb8_t test_vcmpnezb_v0 (vui8_t, vui8_t);
#define test_cmpnezb(_l,_k)	test_vcmpnezb_v0(_l,_k)
#endif

int
test_vec_cmpnezb (void)
{
    vui8_t i, j, k, e;
    int rc = 0;
    printf ("\n%s\n", __FUNCTION__);

    i = (vui8_t){0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
                 0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d};
    j = (vui8_t){0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
                 0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d};
    e = vec_splat_u8 (0);
    // e = (vui8_t){0x20, 0x40, 0x7a, 0x5b, 0x61, 0x62, 0x63, 0x64,
    //             0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d};
    k = (vui8_t) test_cmpnezb (i,j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("cmpnez  of ", i);
    print_vint8x ("         , ", j);
    print_vint8x ("         = ", k);
#endif
    rc += check_vui8 ("vec_cmpnez", k, e);

    i = (vui8_t){0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
                 0x61, 0x62, 0x63, 0x64, 0x31, 0x38, 0x5c, 0x5d};
    j = (vui8_t){0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
                 0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d};
    e = (vui8_t){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00};
    k = (vui8_t) test_cmpnezb (i,j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("cmpnez  of ", i);
    print_vint8x ("         , ", j);
    print_vint8x ("         = ", k);
#endif
    rc += check_vui8 ("vec_cmpnez", k, e);

    i = (vui8_t){0x20, 0x40, 0x5a, 0x00, 0x41, 0x42, 0x43, 0x44,
                 0x61, 0x62, 0x63, 0x64, 0x31, 0x38, 0x5c, 0x5d};
    j = (vui8_t){0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
                 0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d};
    e = (vui8_t){0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00,
                 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00};
    k = (vui8_t) test_cmpnezb (i,j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("cmpnez  of ", i);
    print_vint8x ("         , ", j);
    print_vint8x ("         = ", k);
#endif
    rc += check_vui8 ("vec_cmpnez", k, e);

    i = (vui8_t){0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
                 0x61, 0x62, 0x63, 0x64, 0x31, 0x38, 0x5c, 0x5d};
    j = (vui8_t){0x20, 0x40, 0x5a, 0x00, 0x41, 0x42, 0x43, 0x44,
                 0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d};
    e = (vui8_t){0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00,
                 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00};
    k = (vui8_t) test_cmpnezb (i,j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("cmpnez  of ", i);
    print_vint8x ("         , ", j);
    print_vint8x ("         = ", k);
#endif
    rc += check_vui8 ("vec_cmpnez", k, e);

    i = (vui8_t){0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x00,
                 0x61, 0x62, 0x63, 0x64, 0x31, 0x38, 0x5c, 0x5d};
    j = (vui8_t){0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x00,
                 0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d};
    e = (vui8_t){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
                 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00};
    k = (vui8_t) test_cmpnezb (i,j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("cmpnez  of ", i);
    print_vint8x ("         , ", j);
    print_vint8x ("         = ", k);
#endif
    rc += check_vui8 ("vec_cmpnez", k, e);

    i = (vui8_t){0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x00,
                 0x61, 0x62, 0x63, 0x64, 0x31, 0x38, 0x5c, 0x5d};
    j = (vui8_t){0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x00,
                 0x61, 0x62, 0x63, 0x64, 0x31, 0x38, 0x5c, 0x5d};
    e = (vui8_t){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    k = (vui8_t) test_cmpnezb (i,j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("cmpnez  of ", i);
    print_vint8x ("         , ", j);
    print_vint8x ("         = ", k);
#endif
    rc += check_vui8 ("vec_cmpnez", k, e);

    return (rc);
  }

//#define __DEBUG_PRINT__ 1
#if 0
#define test_clzlsbb(_l)	db_vec_clzlsbb(_l)
#else
#if 1
// test from vec_char_ppc.h via vec_char_dummy.c
extern int test_vec_vclzlsbb (vui8_t);
#define test_clzlsbb(_l)	test_vec_vclzlsbb(_l)
#else
// test from vec_char_dummy.c
extern int test_vclzlsbb (vui8_t);
#define test_clzlsbb(_l)	test_vclzlsbb(_l)
#endif
#endif

int
test_vec_clzlsbb (void)
{
    vui8_t i;
    int r, er;
    int rc = 0;
    printf ("\n%s\n", __FUNCTION__);

    i = CONST_VINT128_B (0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27);
    er = 1;
    r = test_clzlsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_clzlsbb of ", i);
    print_int64x ("             = ", r);
#endif
    rc += check_uint64 ("vec_clzlsbb", r, er);

    i = vec_splat_u8(0);
    er = 16;
    r = test_clzlsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_clzlsbb of ", i);
    print_int64x ("             = ", r);
#endif
    rc += check_uint64 ("vec_clzlsbb", r, er);

    i = (vui8_t) vec_splat_s8(-1);
    er = 0;
    r = test_clzlsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_clzlsbb of ", i);
    print_int64x ("             = ", r);
#endif
    rc += check_uint64 ("vec_clzlsbb", r, er);

    i = (vui8_t) vec_splat_s8(-2);
    er = 16;
    r = test_clzlsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_clzlsbb of ", i);
    print_int64x ("             = ", r);
#endif
    rc += check_uint64 ("vec_clzlsbb", r, er);

    i = (vui8_t) vec_splat_u16(1);
    er = 1;
    r = test_clzlsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_clzlsbb of ", i);
    print_int64x ("             = ", r);
#endif
    rc += check_uint64 ("vec_clzlsbb", r, er);

    i = (vui8_t) vec_splat_u32(1);
    er = 3;
    r = test_clzlsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_clzlsbb of ", i);
    print_int64x ("             = ", r);
#endif
    rc += check_uint64 ("vec_clzlsbb", r, er);

    i = vec_splat_u8(1);
    er = 0;
    r = test_clzlsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_clzlsbb of ", i);
    print_int64x ("             = ", r);
#endif
    rc += check_uint64 ("vec_clzlsbb", r, er);

    i = CONST_VINT128_B (0x10, 0x10, 0x11, 0x12, 0x14, 0x14, 0x16, 0x17,
                 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27);
    er = 2;
    r = test_clzlsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_clzlsbb of ", i);
    print_int64x ("             = ", r);
#endif
    rc += check_uint64 ("vec_clzlsbb", r, er);

    i = CONST_VINT128_B (0x10, 0x10, 0x12, 0x12, 0x14, 0x14, 0x17, 0x17,
                 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27);
    er = 6;
    r = test_clzlsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_clzlsbb of ", i);
    print_int64x ("             = ", r);
#endif
    rc += check_uint64 ("vec_clzlsbb", r, er);

    i = CONST_VINT128_B (0x10, 0x10, 0x12, 0x12, 0x14, 0x14, 0x16, 0x17,
                 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27);
    er = 7;
    r = test_clzlsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_clzlsbb of ", i);
    print_int64x ("             = ", r);
#endif
    rc += check_uint64 ("vec_clzlsbb", r, er);



    i = CONST_VINT128_B (0x20, 0x20, 0x22, 0x22, 0x24, 0x24, 0x26, 0x26,
			 0x10, 0x10, 0x11, 0x12, 0x14, 0x14, 0x16, 0x17);
    er = 10;
    r = test_clzlsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_clzlsbb of ", i);
    print_int64x ("             = ", r);
#endif
    rc += check_uint64 ("vec_clzlsbb", r, er);

    i = CONST_VINT128_B (0x20, 0x20, 0x22, 0x22, 0x24, 0x24, 0x26, 0x26,
			 0x10, 0x10, 0x12, 0x12, 0x14, 0x14, 0x17, 0x17);
    er = 14;
    r = test_clzlsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_clzlsbb of ", i);
    print_int64x ("             = ", r);
#endif
    rc += check_uint64 ("vec_clzlsbb", r, er);

    i = CONST_VINT128_B (0x20, 0x20, 0x22, 0x22, 0x24, 0x24, 0x26, 0x26,
			 0x10, 0x10, 0x12, 0x12, 0x14, 0x14, 0x16, 0x17);
    er = 15;
    r = test_clzlsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_clzlsbb of ", i);
    print_int64x ("             = ", r);
#endif
    rc += check_uint64 ("vec_clzlsbb", r, er);

    return (rc);
  }

//#define __DEBUG_PRINT__ 1
#if 1
// test from vec_char_ppc.h via vec_char_dummy.c
extern int test_vec_vctzlsbb (vui8_t);
#define test_ctzlsbb(_l)	test_vec_vctzlsbb(_l)
#else
// test from vec_char_dummy.c
extern int test_vctzlsbb (vui8_t);
#define test_ctzlsbb(_l)	test_vctzlsbb(_l)
#endif

int
test_vec_ctzlsbb (void)
{
    vui8_t i;
    int r, er;
    int rc = 0;
    printf ("\n%s\n", __FUNCTION__);

    i = CONST_VINT128_B (0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                         0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27);
    er = 0;
    r = test_ctzlsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_ctzlsbb of ", i);
    print_int64x ("             = ", r);
#endif
    rc += check_uint64 ("vec_ctzlsbb", r, er);

    i = CONST_VINT128_B (0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                         0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x26);
    er = 2;
    r = test_ctzlsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_ctzlsbb of ", i);
    print_int64x ("             = ", r);
#endif
    rc += check_uint64 ("vec_ctzlsbb", r, er);

    i = CONST_VINT128_B (0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                         0x20, 0x21, 0x22, 0x22, 0x24, 0x24, 0x26, 0x26);
    er = 6;
    r = test_ctzlsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_ctzlsbb of ", i);
    print_int64x ("             = ", r);
#endif
    rc += check_uint64 ("vec_ctzlsbb", r, er);

    i = CONST_VINT128_B (0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                         0x20, 0x20, 0x22, 0x22, 0x24, 0x24, 0x26, 0x26);
    er = 8;
    r = test_ctzlsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_ctzlsbb of ", i);
    print_int64x ("             = ", r);
#endif
    rc += check_uint64 ("vec_ctzlsbb", r, er);

    i = CONST_VINT128_B (0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x16,
                         0x20, 0x22, 0x22, 0x22, 0x24, 0x24, 0x26, 0x26);
    er = 10;
    r = test_ctzlsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_ctzlsbb of ", i);
    print_int64x ("             = ", r);
#endif
    rc += check_uint64 ("vec_ctzlsbb", r, er);

    i = CONST_VINT128_B (0x10, 0x11, 0x11, 0x12, 0x14, 0x14, 0x16, 0x16,
                         0x20, 0x22, 0x22, 0x22, 0x24, 0x24, 0x26, 0x26);
    er = 13;
    r = test_ctzlsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_ctzlsbb of ", i);
    print_int64x ("             = ", r);
#endif
    rc += check_uint64 ("vec_ctzlsbb", r, er);

    i = vec_splat_u8(0);
    er = 16;
    r = test_ctzlsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_ctzlsbb of ", i);
    print_int64x ("             = ", r);
#endif
    rc += check_uint64 ("vec_ctzlsbb", r, er);

    i = (vui8_t) vec_splat_s8(-1);
    er = 0;
    r = test_ctzlsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_ctzlsbb of ", i);
    print_int64x ("             = ", r);
#endif
    rc += check_uint64 ("vec_ctzlsbb", r, er);

    i = (vui8_t) vec_splat_s8(-2);
    er = 16;
    r = test_ctzlsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_ctzlsbb of ", i);
    print_int64x ("             = ", r);
#endif
    rc += check_uint64 ("vec_ctzlsbb", r, er);

#if ( __GNUC__ == 12)
#else
    i = (vui8_t) vec_splat_u16(1);
    i = vec_sld (i,i,1);
    er = 1;
    r = test_ctzlsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_ctzlsbb of ", i);
    print_int64x ("             = ", r);
#endif
    rc += check_uint64 ("vec_ctzlsbb", r, er);
#endif

    i = (vui8_t) vec_splat_u32(1);
    i = vec_sld (i,i,2);
    er = 2;
    r = test_ctzlsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_ctzlsbb of ", i);
    print_int64x ("             = ", r);
#endif
    rc += check_uint64 ("vec_ctzlsbb", r, er);

    i = (vui8_t) vec_splat_u32(1);
    i = vec_sld (i,i,3);
    er = 3;
    r = test_ctzlsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_ctzlsbb of ", i);
    print_int64x ("             = ", r);
#endif
    rc += check_uint64 ("vec_ctzlsbb", r, er);

    i = vec_splat_u8(1);
    er = 0;
    r = test_ctzlsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_ctzlsbb of ", i);
    print_int64x ("             = ", r);
#endif
    rc += check_uint64 ("vec_ctzlsbb", r, er);

    return (rc);
  }

#if 1
// test from vec_char_ppc.h via vec_char_dummy.c
extern int test_vec_cntlz_lsbb_bi (vui8_t);
#define test_cntlz_lsbb(_l)	test_vec_cntlz_lsbb_bi(_l)
#else
// test from vec_char_dummy.c
extern int test_cntlz_lsbb_bi (vui8_t);
#define test_cntlz_lsbb(_l)	test_cntlz_lsbb_bi(_l)
#endif

int
test_vec_cntlz_lsbb (void)
{
    vui8_t i;
    int r, er;
    int rc = 0;
    printf ("\n%s\n", __FUNCTION__);

    i = CONST_VINT128_B (0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                         0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27);
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    er = 0;
#else
    er = 1;
#endif
    r = test_cntlz_lsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_cntlz_lsbb of ", i);
    print_int64x ("                = ", r);
#endif
    rc += check_uint64 ("vec_cntlz_lsbb", r, er);

    i = CONST_VINT128_B (0x11, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                         0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x26);
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    er = 2;
#else
    er = 0;
#endif
    r = test_cntlz_lsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_cntlz_lsbb of ", i);
    print_int64x ("                = ", r);
#endif
    rc += check_uint64 ("vec_cntlz_lsbb", r, er);

    i = vec_splat_u8(0);
    er = 16;
    r = test_cntlz_lsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_cntlz_lsbb of ", i);
    print_int64x ("                = ", r);
#endif
    rc += check_uint64 ("vec_cntlz_lsbb", r, er);

    return (rc);
  }

#if 1
// test from vec_char_ppc.h via vec_char_dummy.c
extern int test_vec_cnttz_lsbb_bi (vui8_t);
#define test_cnttz_lsbb(_l)	test_vec_cnttz_lsbb_bi(_l)
#else
// test from vec_char_dummy.c
extern int test_cnttz_lsbb_bi (vui8_t);
#define test_cnttz_lsbb(_l)	test_cnttz_lsbb_bi(_l)
#endif

int
test_vec_cnttz_lsbb (void)
{
    vui8_t i;
    int r, er;
    int rc = 0;
    printf ("\n%s\n", __FUNCTION__);

    i = CONST_VINT128_B (0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                         0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27);
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    er = 1;
#else
    er = 0;
#endif
    r = test_cnttz_lsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_cnttz_lsbb of ", i);
    print_int64x ("                = ", r);
#endif
    rc += check_uint64 ("vec_cnttz_lsbb", r, er);

    i = CONST_VINT128_B (0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                         0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x26);
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    er = 1;
#else
    er = 2;
#endif
    r = test_cnttz_lsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_cnttz_lsbb of ", i);
    print_int64x ("                = ", r);
#endif
    rc += check_uint64 ("vec_cnttz_lsbb", r, er);

    i = vec_splat_u8(0);
    er = 16;
    r = test_cnttz_lsbb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_cnttz_lsbb of ", i);
    print_int64x ("                = ", r);
#endif
    rc += check_uint64 ("vec_cnttz_lsbb", r, er);

    return (rc);
  }

#if 1
// test from vec_char_ppc.h via vec_char_dummy.c
extern int test_vec_first_match_byte_index (vui8_t, vui8_t);
#define test_first_match_index(_l,_k)	\
        test_vec_first_match_byte_index(_l,_k)
#else
// test from vec_char_dummy.c
extern int test_first_match_byte_index (vui8_t, vui8_t);
#define test_first_match_index(_l,_k)	\
        test_first_match_byte_index(_l,_k)
#endif

int
test_first_match (void)
{
    vui8_t i, j;
    int r, er;
    int rc = 0;
    printf ("\n%s\n", __FUNCTION__);

    i = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};
    j = (vui8_t) {0x11, 0x12, 0x13, 0x14, 0x14, 0x35, 0x36, 0x37,
                  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x47, 0x47};

    er = 4;
    r = test_first_match_index (i, j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_first_match of ", i);
    print_int64x ("                 = ", r);
#endif
    rc += check_uint64 ("vec_first_match", r, er);

    i = (vui8_t) vec_splat_u8(1);
    j = (vui8_t) vec_splat_u8(0);

    er = 16;
    r = test_first_match_index (i, j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_first_match of ", i);
    print_int64x ("                 = ", r);
#endif
    rc += check_uint64 ("vec_first_match", r, er);

    i = (vui8_t) vec_splat_u8(1);
    j = (vui8_t) vec_splat_u8(1);

    er = 0;
    r = test_first_match_index (i, j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_first_match of ", i);
    print_int64x ("                 = ", r);
#endif
    rc += check_uint64 ("vec_first_match", r, er);

    i = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};
    j = (vui8_t) {0x11, 0x12, 0x13, 0x14, 0x15, 0x36, 0x37, 0x38,
                  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x27};

    er = 15;
    r = test_first_match_index (i, j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_first_match of ", i);
    print_int64x ("                 = ", r);
#endif
    rc += check_uint64 ("vec_first_match", r, er);

    i = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};
    j = (vui8_t) {0x11, 0x12, 0x13, 0x14, 0x15, 0x36, 0x37, 0x38,
                  0x40, 0x41, 0x42, 0x43, 0x24, 0x45, 0x46, 0x27};

    er = 12;
    r = test_first_match_index (i, j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_first_match of ", i);
    print_int64x ("                 = ", r);
#endif
    rc += check_uint64 ("vec_first_match", r, er);

    return (rc);
  }

#if 1
// test from vec_char_ppc.h via vec_char_dummy.c
extern int test_vec_first_mismatch_byte_index (vui8_t, vui8_t);
#define test_first_mismatch_index(_l,_k)	\
        test_vec_first_mismatch_byte_index(_l,_k)
#else
// test from vec_char_dummy.c
extern int test_first_mismatch_byte_index (vui8_t, vui8_t);
#define test_first_mismatch_index(_l,_k)	\
        test_first_mismatch_byte_index(_l,_k)
#endif

int
test_first_mismatch (void)
{
    vui8_t i, j;
    int r, er;
    int rc = 0;
    printf ("\n%s\n", __FUNCTION__);

    i = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};
    j = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};

    er = 16;
    r = test_first_mismatch_index (i, j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_first_mismatch of ", i);
    print_int64x ("                 = ", r);
#endif
    rc += check_uint64 ("vec_first_mismatch", r, er);

    i = (vui8_t) {0x18, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};
    j = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};

    er = 0;
    r = test_first_mismatch_index (i, j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_first_mismatch of ", i);
    print_int64x ("                 = ", r);
#endif
    rc += check_uint64 ("vec_first_mismatch", r, er);

    i = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};
    j = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x28};

    er = 15;
    r = test_first_mismatch_index (i, j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_first_mismatch of ", i);
    print_int64x ("                 = ", r);
#endif
    rc += check_uint64 ("vec_first_mismatch", r, er);

    i = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};
    j = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x18, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x28};

    er = 4;
    r = test_first_mismatch_index (i, j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_first_mismatch of ", i);
    print_int64x ("                 = ", r);
#endif
    rc += check_uint64 ("vec_first_mismatch", r, er);

    return (rc);
  }

#if 1
// test from vec_char_ppc.h via vec_char_dummy.c
extern int
test_vec_first_match_byte_or_eos_index (vui8_t vra, vui8_t vrb);
#define test_first_match_or_eos_index(_l,_k)	\
        test_vec_first_match_byte_or_eos_index(_l,_k)
#else
// test from vec_char_dummy.c
extern int
test_first_match_byte_or_eos_index (vui8_t, vui8_t);
#define test_first_match_or_eos_index(_l,_k)	\
        test_first_match_byte_or_eos_index(_l,_k)
#endif

int
test_first_match_or_eos (void)
{
    vui8_t i, j;
    int r, er;
    int rc = 0;
    printf ("\n%s\n", __FUNCTION__);

    i = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};
    j = (vui8_t) {0x11, 0x12, 0x13, 0x14, 0x14, 0x35, 0x36, 0x37,
                  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x47, 0x47};

    er = 4;
    r = test_first_match_or_eos_index (i, j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_first_match_or_eos of ", i);
    print_int64x ("                 = ", r);
#endif
    rc += check_uint64 ("vec_first_match_or_eos", r, er);

    i = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x00,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};
    j = (vui8_t) {0x11, 0x12, 0x13, 0x14, 0x14, 0x35, 0x36, 0x37,
                  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x47, 0x47};

    er = 4;
    r = test_first_match_or_eos_index (i, j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_first_match_or_eos of ", i);
    print_int64x ("                 = ", r);
#endif
    rc += check_uint64 ("vec_first_match_or_eos", r, er);

    i = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};
    j = (vui8_t) {0x11, 0x12, 0x13, 0x14, 0x14, 0x00, 0x36, 0x37,
                  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x47, 0x47};

    er = 4;
    r = test_first_match_or_eos_index (i, j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_first_match_or_eos of ", i);
    print_int64x ("                 = ", r);
#endif
    rc += check_uint64 ("vec_first_match_or_eos", r, er);

    i = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};
    j = (vui8_t) {0x11, 0x12, 0x13, 0x00, 0x14, 0x35, 0x36, 0x37,
                  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x47, 0x47};

    er = 3;
    r = test_first_match_or_eos_index (i, j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_first_match_or_eos of ", i);
    print_int64x ("                 = ", r);
#endif
    rc += check_uint64 ("vec_first_match_or_eos", r, er);

    i = (vui8_t) {0x10, 0x11, 0x12, 0x00, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};
    j = (vui8_t) {0x11, 0x12, 0x13, 0x14, 0x14, 0x35, 0x36, 0x37,
                  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x47, 0x47};

    er = 3;
    r = test_first_match_or_eos_index (i, j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_first_match_or_eos of ", i);
    print_int64x ("                 = ", r);
#endif
    rc += check_uint64 ("vec_first_match_or_eos", r, er);

    i = (vui8_t) vec_splat_u8(1);
    j = (vui8_t) vec_splat_u8(0);

    er = 0;
    r = test_first_match_or_eos_index (i, j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_first_match_or_eos of ", i);
    print_int64x ("                 = ", r);
#endif
    rc += check_uint64 ("vec_first_match_or_eos", r, er);

    i = (vui8_t) vec_splat_u8(1);
    j = (vui8_t) vec_splat_u8(2);

    er = 16;
    r = test_first_match_or_eos_index (i, j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_first_match_or_eos of ", i);
    print_int64x ("                 = ", r);
#endif
    rc += check_uint64 ("vec_first_match_or_eos", r, er);

    i = (vui8_t) vec_splat_u8(1);
    j = (vui8_t) vec_splat_u8(1);

    er = 0;
    r = test_first_match_or_eos_index (i, j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_first_match_or_eos of ", i);
    print_int64x ("                 = ", r);
#endif
    rc += check_uint64 ("vec_first_match_or_eos", r, er);

    i = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};
    j = (vui8_t) {0x11, 0x12, 0x13, 0x14, 0x15, 0x36, 0x37, 0x38,
                  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x27};

    er = 15;
    r = test_first_match_or_eos_index (i, j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_first_match_or_eos of ", i);
    print_int64x ("                 = ", r);
#endif
    rc += check_uint64 ("vec_first_match_or_eos", r, er);

    i = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};
    j = (vui8_t) {0x11, 0x12, 0x13, 0x14, 0x15, 0x36, 0x37, 0x38,
                  0x40, 0x41, 0x42, 0x43, 0x24, 0x45, 0x46, 0x27};

    er = 12;
    r = test_first_match_or_eos_index (i, j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_first_match_or_eos of ", i);
    print_int64x ("                 = ", r);
#endif
    rc += check_uint64 ("vec_first_match_or_eos", r, er);

    return (rc);
  }

#if 1
// test from vec_char_ppc.h via vec_char_dummy.c
extern int
test_vec_first_mismatch_byte_or_eos_index (vui8_t vra, vui8_t vrb);
#define test_first_mismatch_or_eos_index(_l,_k)	\
    test_vec_first_mismatch_byte_or_eos_index(_l,_k)
#else
// test from vec_char_dummy.c
extern int test_first_mismatch_byte_or_eos_index (vui8_t, vui8_t);
#define test_first_mismatch_or_eos_index(_l,_k)	\
    test_first_mismatch_byte_or_eos_index(_l,_k)
#endif

int
test_first_mismatch_or_eos (void)
{
    vui8_t i, j;
    int r, er;
    int rc = 0;
    printf ("\n%s\n", __FUNCTION__);

    i = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};
    j = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};

    er = 16;
    r = test_first_mismatch_or_eos_index (i, j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_first_mismatch_or_eos of ", i);
    print_int64x ("                 = ", r);
#endif
    rc += check_uint64 ("vec_first_mismatch_or_eos", r, er);

    i = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x00, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};
    j = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};

    er = 6;
    r = test_first_mismatch_or_eos_index (i, j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_first_mismatch_or_eos of ", i);
    print_int64x ("                 = ", r);
#endif
    rc += check_uint64 ("vec_first_mismatch_or_eos", r, er);

    i = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};
    j = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x00, 0x25, 0x27, 0x27};

    er = 12;
    r = test_first_mismatch_or_eos_index (i, j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_first_mismatch_or_eos of ", i);
    print_int64x ("                 = ", r);
#endif
    rc += check_uint64 ("vec_first_mismatch_or_eos", r, er);

    i = (vui8_t) {0x18, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};
    j = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};

    er = 0;
    r = test_first_mismatch_or_eos_index (i, j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_first_mismatch_or_eos of ", i);
    print_int64x ("                 = ", r);
#endif
    rc += check_uint64 ("vec_first_mismatch_or_eos", r, er);

    i = (vui8_t) {0x18, 0x11, 0x12, 0x13, 0x00, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};
    j = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x00, 0x27};

    er = 0;
    r = test_first_mismatch_or_eos_index (i, j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_first_mismatch_or_eos of ", i);
    print_int64x ("                 = ", r);
#endif
    rc += check_uint64 ("vec_first_mismatch_or_eos", r, er);

    i = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};
    j = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x28};

    er = 15;
    r = test_first_mismatch_or_eos_index (i, j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_first_mismatch_or_eos of ", i);
    print_int64x ("                 = ", r);
#endif
    rc += check_uint64 ("vec_first_mismatch_or_eos", r, er);

    i = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x00, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};
    j = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x00, 0x28};

    er = 4;
    r = test_first_mismatch_or_eos_index (i, j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_first_mismatch_or_eos of ", i);
    print_int64x ("                 = ", r);
#endif
    rc += check_uint64 ("vec_first_mismatch_or_eos", r, er);

    i = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};
    j = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x18, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x28};

    er = 4;
    r = test_first_mismatch_or_eos_index (i, j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_first_mismatch_or_eos of ", i);
    print_int64x ("                 = ", r);
#endif
    rc += check_uint64 ("vec_first_mismatch_or_eos", r, er);

    i = (vui8_t) {0x10, 0x11, 0x12, 0x00, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};
    j = (vui8_t) {0x10, 0x11, 0x12, 0x13, 0x18, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x28};

    er = 3;
    r = test_first_mismatch_or_eos_index (i, j);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_first_mismatch_or_eos of ", i);
    print_int64x ("                 = ", r);
#endif
    rc += check_uint64 ("vec_first_mismatch_or_eos", r, er);

    return (rc);
  }

#if 1
// test from vec_char_ppc.h via vec_char_dummy.c
extern int test_vec_lsbb_all_ones (vui8_t);
#define test_lsbb_all_ones(_l)	test_vec_lsbb_all_ones(_l)

extern int test_vec_lsbb_all_zeros (vui8_t);
#define test_lsbb_all_zeros(_l)	test_vec_lsbb_all_zeros(_l)
#else
// test from vec_char_dummy.c
extern int test_lsbb_all_ones (vui8_t vra);
extern int test_lsbb_all_zeros (vui8_t vra);
#endif

int
test_lsbb (void)
{
    vui8_t i;
    int r, er;
    int rc = 0;
    printf ("\n%s\n", __FUNCTION__);

    i = vec_splat_u8(1);
    er = 1;
    r = test_lsbb_all_ones (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_test_lsbb_all_ones of ", i);
    print_int64x ("                = ", r);
#endif
    rc += check_uint64 ("vec_test_lsbb_all_ones-1", r, er);

    i = vec_splat_u8(-1);
    er = 1;
    r = test_lsbb_all_ones (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_test_lsbb_all_ones of ", i);
    print_int64x ("                = ", r);
#endif
    rc += check_uint64 ("vec_test_lsbb_all_ones-2", r, er);

    i = vec_splat_u8(-2);
    er = 0;
    r = test_lsbb_all_ones (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_test_lsbb_all_ones of ", i);
    print_int64x ("                = ", r);
#endif
    rc += check_uint64 ("vec_test_lsbb_all_ones-3", r, er);

    i = (vui8_t) {0x10, 0x11, 0x12, 0x00, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};
    er = 0;
    r = test_lsbb_all_ones (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_test_lsbb_all_ones of ", i);
    print_int64x ("                = ", r);
#endif
    rc += check_uint64 ("vec_test_lsbb_all_ones-4", r, er);

    i = vec_splat_u8(1);
    er = 0;
    r = test_lsbb_all_zeros (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_test_lsbb_all_zeros of ", i);
    print_int64x ("                = ", r);
#endif
    rc += check_uint64 ("vec_test_lsbb_all_zeros-1", r, er);

    i = vec_splat_u8(-2);
    er = 1;
    r = test_lsbb_all_zeros (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_test_lsbb_all_zeros of ", i);
    print_int64x ("                = ", r);
#endif
    rc += check_uint64 ("vec_test_lsbb_all_zeros-2", r, er);

    i = vec_splat_u8(-1);
    er = 0;
    r = test_lsbb_all_zeros (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_test_lsbb_all_zeros of ", i);
    print_int64x ("                = ", r);
#endif
    rc += check_uint64 ("vec_test_lsbb_all_zeros-3", r, er);

    i = (vui8_t) {0x10, 0x11, 0x12, 0x00, 0x14, 0x15, 0x16, 0x17,
                  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x27, 0x27};
    er = 0;
    r = test_lsbb_all_zeros (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_test_lsbb_all_zeros of ", i);
    print_int64x ("                = ", r);
#endif
    rc += check_uint64 ("vec_test_lsbb_all_zeros=4", r, er);

    return (rc);
  }

//#define __DEBUG_PRINT__ 1
#if 1
#if 1
// test directly from vec_char_ppc.h
#define test_clrlb(_l,_k)	vec_vclrlb(_l,_k)
#else
// test from vec_char_ppc.h via vec_char_dummy.c
extern vui8_t test_vec_vclrlb (vui8_t, unsigned int);
#define test_clrlb(_l,_k)	test_vec_vclrlb(_l,_k)
#endif
#else
// test from vec_char_dummy.c
extern vui8_t test_vclrlb (vui8_t, unsigned int);
#define test_clrlb(_j,_k)	test_vclrlb(_j,_k)
#endif

int
test_clear_left(void)
{
    vui8_t i, j, e;
    unsigned int rb;
    int rc = 0;
    printf ("\n%s\n", __FUNCTION__);

    i = CONST_VINT128_B (0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
                         0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d);
    rb = 8;

    e = CONST_VINT128_B (0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d);
    j = (vui8_t) test_clrlb (i,rb);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_clrlb  of ", i);
    printf       ("            , ", rb);
    print_vint8x ("            = ", k);
#endif
    rc += check_vui8 ("vec_clrlb 8", j, e);

    i = CONST_VINT128_B (0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
                         0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d);
    rb = 7;

    e = CONST_VINT128_B (0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d);
    j = (vui8_t) test_clrlb (i,rb);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_clrlb  of ", i);
    printf       ("            , ", rb);
    print_vint8x ("            = ", k);
#endif
    rc += check_vui8 ("vec_clrlb 7", j, e);

    i = CONST_VINT128_B (0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
                         0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d);
    rb = 9;

    e = CONST_VINT128_B (0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44,
	                 0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d);
    j = (vui8_t) test_clrlb (i,rb);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_clrlb  of ", i);
    printf       ("            , ", rb);
    print_vint8x ("            = ", k);
#endif
    rc += check_vui8 ("vec_clrlb 9", j, e);

    i = CONST_VINT128_B (0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
                         0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d);
    rb = 16;

    e = i;
    j = (vui8_t) test_clrlb (i,rb);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_clrlb  of ", i);
    printf       ("            , %i", rb);
    print_vint8x ("            = ", k);
#endif
    rc += check_vui8 ("vec_clrlb 16", j, e);

    i = CONST_VINT128_B (0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
                         0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d);
    rb = 0;

    e = (vui8_t){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    j = (vui8_t) test_clrlb (i,rb);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_clrlb  of ", i);
    printf       ("            , ", rb);
    print_vint8x ("            = ", k);
#endif
    rc += check_vui8 ("vec_clrlb 0", j, e);

    return (rc);
  }

//#define __DEBUG_PRINT__ 1
#if 1
#if 1
// test directly from vec_char_ppc.h
#define test_clrrb(_l,_k)	vec_vclrrb(_l,_k)
#else
// test from vec_char_ppc.h via vec_char_dummy.c
extern vui8_t test_vec_vclrrb (vui8_t, unsigned int);
#define test_clrrb(_l,_k)	test_vec_vclrrb(_l,_k)
#endif
#else
// test from vec_char_dummy.c
extern vui8_t test_vclrrb (vui8_t, unsigned int);
#define test_clrrb(_j,_k)	test_vclrrb(_j,_k)
#endif

int
test_clear_right(void)
{
    vui8_t i, j, e;
    unsigned int rb;
    int rc = 0;
    printf ("\n%s\n", __FUNCTION__);

    i = CONST_VINT128_B (0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
                         0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d);
    rb = 8;

    e = CONST_VINT128_B (0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    j = (vui8_t) test_clrrb (i,rb);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_clrrb  of ", i);
    printf       ("            , ", rb);
    print_vint8x ("            = ", k);
#endif
    rc += check_vui8 ("vec_clrrb 8", j, e);

    i = CONST_VINT128_B (0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
                         0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d);
    rb = 7;

    e = CONST_VINT128_B (0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    j = (vui8_t) test_clrrb (i,rb);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_clrrb  of ", i);
    printf       ("            , ", rb);
    print_vint8x ("            = ", k);
#endif
    rc += check_vui8 ("vec_clrrb 7", j, e);

    i = CONST_VINT128_B (0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
                         0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d);
    rb = 9;

    e = CONST_VINT128_B (0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
	                 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    j = (vui8_t) test_clrrb (i,rb);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_clrrb  of ", i);
    printf       ("            , ", rb);
    print_vint8x ("            = ", k);
#endif
    rc += check_vui8 ("vec_clrrb 9", j, e);

    i = CONST_VINT128_B (0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
                         0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d);
    rb = 16;

    e = i;
    j = (vui8_t) test_clrrb (i,rb);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_clrrb  of ", i);
    printf       ("            , %i", rb);
    print_vint8x ("            = ", k);
#endif
    rc += check_vui8 ("vec_clrrb 16", j, e);

    i = CONST_VINT128_B (0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
                         0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d);
    rb = 0;

    e = (vui8_t){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    j = (vui8_t) test_clrrb (i,rb);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_clrrb  of ", i);
    printf       ("            , ", rb);
    print_vint8x ("            = ", k);
#endif
    rc += check_vui8 ("vec_clrrb 0", j, e);

    i = CONST_VINT128_B (0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
                         0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d);
    rb = 17;

    e = i;
    j = (vui8_t) test_clrrb (i,rb);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_clrrb  of ", i);
    printf       ("            , %i", rb);
    print_vint8x ("            = ", k);
#endif
    rc += check_vui8 ("vec_clrrb 17", j, e);

    return (rc);
  }

//#define __DEBUG_PRINT__ 1
#if 1
#if 0
// test directly from vec_char_ppc.h
#define test_stribl(_l)	vec_vstribl(_l)
#else
// test from vec_char_ppc.h via vec_char_dummy.c
extern vui8_t test_vec_vstribl (vui8_t);
#define test_stribl(_l)	test_vec_vstribl(_l)
#endif
#else
// test from vec_char_dummy.c
extern vui8_t test_vstribl (vui8_t);
#define test_stribl(_j)	test_vstribl(_j)
#endif

int
test_clear_left_justified(void)
{
    vui8_t i, j, e;
    int rc = 0;
    printf ("\n%s\n", __FUNCTION__);

    i = CONST_VINT128_B (0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
                         0x00, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d);

    e = CONST_VINT128_B (0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    j = (vui8_t) test_stribl (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_vstribl  of ", i);
    print_vint8x ("            = ", j);
#endif
    rc += check_vui8 ("vec_vstribl 1", j, e);

    i = CONST_VINT128_B (0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
			 0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x00);

    e = CONST_VINT128_B (0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
			 0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x00);
    j = (vui8_t) test_stribl (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_vstribl  of ", i);
    print_vint8x ("            = ", j);
#endif
    rc += check_vui8 ("vec_vstribl 2", j, e);

    i = CONST_VINT128_B (0x00, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
                         0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d);

    e = CONST_VINT128_B (0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    j = (vui8_t) test_stribl (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_vstribl  of ", i);
    print_vint8x ("            = ", j);
#endif
    rc += check_vui8 ("vec_vstribl 3", j, e);

    i = CONST_VINT128_B (0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
                         0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d);

    e = CONST_VINT128_B (0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
                         0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d);
    j = (vui8_t) test_stribl (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_vstribl  of ", i);
    print_vint8x ("            = ", j);
#endif
    rc += check_vui8 ("vec_vstribl 4", j, e);

    return (rc);
  }

//#define __DEBUG_PRINT__ 1
#if 1
#if 0
// test directly from vec_char_ppc.h
#define test_stribr(_l)	vec_vstribr(_l)
#else
// test from vec_char_ppc.h via vec_char_dummy.c
extern vui8_t test_vec_vstribr (vui8_t);
#define test_stribr(_l)	test_vec_vstribr(_l)
#endif
#else
// test from vec_char_dummy.c
extern vui8_t test_vstribr (vui8_t);
#define test_stribr(_j)	test_vstribr(_j)
#endif
int
test_clear_right_justified(void)
{
    vui8_t i, j, e;
    int rc = 0;
    printf ("\n%s\n", __FUNCTION__);

    i = CONST_VINT128_B (0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
                         0x00, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d);

    e = CONST_VINT128_B (0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d);
    j = (vui8_t) test_stribr (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_vstribr  of ", i);
    print_vint8x ("            = ", j);
#endif
    rc += check_vui8 ("vec_vstribr 1", j, e);

    i = CONST_VINT128_B (0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
			 0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x00);

    e = CONST_VINT128_B (0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    j = (vui8_t) test_stribr (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_vstribr  of ", i);
    print_vint8x ("            = ", j);
#endif
    rc += check_vui8 ("vec_vstribr 2", j, e);

    i = CONST_VINT128_B (0x00, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
                         0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d);

    e = CONST_VINT128_B (0x00, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
                         0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d);
    j = (vui8_t) test_stribr (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_vstribr  of ", i);
    print_vint8x ("            = ", j);
#endif
    rc += check_vui8 ("vec_vstribr 3", j, e);

    i = CONST_VINT128_B (0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
                         0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d);

    e = CONST_VINT128_B (0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
                         0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d);
    j = (vui8_t) test_stribr (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_vstribr  of ", i);
    print_vint8x ("            = ", j);
#endif
    rc += check_vui8 ("vec_vstribr 4", j, e);

    return (rc);
  }

// #define __DEBUG_PRINT__ 0
#if 1
#if 0
// test directly from vec_char_ppc.h
#define test_stribr_p(_l)	vec_vstribr_p(_l)
#else
// test from vec_char_ppc.h via vec_char_dummy.c
extern int test_vec_vstribr_p (vui8_t);
#define test_stribr_p(_l)	test_vec_vstribr_p(_l)
#endif
#else
// test from vec_char_dummy.c
extern int test_vstribr_p (vui8_t);
#define test_stribr_p(_j)	test_vstribr_p(_j)
#endif
int
test_isolate_right_justified_p(void)
{
    vui8_t i;
    int r, er;
    int rc = 0;
    printf ("\n%s\n", __FUNCTION__);

    i = CONST_VINT128_B (0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
                         0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d);
    er = 0;
    r = test_stribr_p (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_vec_vstrir_p of ", i);
    print_int64x ("                = ", r);
#endif
    rc += check_uint64 ("vec_vec_vstrir_p 1", r, er);


    i = CONST_VINT128_B (0x20, 0x40, 0x5a, 0x5b, 0x41, 0x00, 0x43, 0x44,
                         0x00, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d);
    er = 1;
    r = test_stribr_p (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_vec_vstrir_p of ", i);
    print_int64x ("                = ", r);
#endif
    rc += check_uint64 ("vec_vec_vstrir_p 2", r, er);
    return (rc);
  }

// #define __DEBUG_PRINT__ 0
#if 1
#if 0
// test directly from vec_char_ppc.h
#define test_stribl_p(_l)	vec_vstribl_p(_l)
#else
// test from vec_char_ppc.h via vec_char_dummy.c
extern int test_vec_vstribl_p (vui8_t);
#define test_stribl_p(_l)	test_vec_vstribl_p(_l)
#endif
#else
// test from vec_char_dummy.c
extern int test_vstribl_p (vui8_t);
#define test_stribl_p(_j)	test_vstribl_p(_j)
#endif
int
test_isolate_left_justified_p(void)
{
    vui8_t i;
    int r, er;
    int rc = 0;
    printf ("\n%s\n", __FUNCTION__);

    i = CONST_VINT128_B (0x20, 0x40, 0x5a, 0x5b, 0x41, 0x42, 0x43, 0x44,
                         0x61, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d);
    er = 0;
    r = test_stribl_p (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_vec_vstrir_p of ", i);
    print_int64x ("                = ", r);
#endif
    rc += check_uint64 ("vec_vec_vstrir_p 1", r, er);


    i = CONST_VINT128_B (0x20, 0x40, 0x5a, 0x5b, 0x41, 0x00, 0x43, 0x44,
                         0x00, 0x62, 0x63, 0x64, 0x31, 0x39, 0x5c, 0x5d);
    er = 1;
    r = test_stribl_p (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_vec_vstrir_p of ", i);
    print_int64x ("                = ", r);
#endif
    rc += check_uint64 ("vec_vec_vstrir_p 2", r, er);
    return (rc);
  }

// #define __DEBUG_PRINT__ 1
#if 1
// test directly from vec_char_ppc.h
#define test_splat6(_l)	vec_splat6_u8(_l)
#else
// test from vec_char_ppc.h via vec_char_dummy.c
#endif

int
test_splat6_u8(void)
{
    vui8_t i, j, k, e, m6;
    int rc = 0;
    printf ("\n%s\n", __FUNCTION__);

    m6 = (vui8_t) {63, 63, 63, 63, 63, 63, 63, 63,
                   63, 63, 63, 63, 63, 63, 63, 63};

    i = (vui8_t) {0, 1, 2, 3, 4, 5, 6, 7,
                  8, 9, 10, 11, 12, 13, 14, 15};

    e = vec_splat (i, 1);
    j = test_splat6 (1);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_u8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat6_u8 1", j, e);

    e = vec_splat (i, 15);
    j = test_splat6 (15);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_u8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat6_u8 15", j, e);

    i = (vui8_t) {16, 17, 18, 19, 20, 21, 22, 23,
                  24, 25, 26, 27, 28, 29, 30, 31};

    e = vec_splat (i, (16-16));
    j = test_splat6 (16);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_u8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat6_u8 16", j, e);

    e = vec_splat (i, (17-16));
    j = test_splat6 (17);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_u8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat6_u8 17", j, e);

    e = vec_splat (i, (24-16));
    j = test_splat6 (24);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_u8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat6_u8 24", j, e);

    e = vec_splat (i, (30-16));
    j = test_splat6 (30);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_u8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat6_u8 30", j, e);

    e = vec_splat (i, (31-16));
    j = test_splat6 (31);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_u8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat6_u8 31", j, e);

    i = (vui8_t) {32, 33, 34, 35, 36, 37, 38, 39,
                  40, 41, 42, 43, 44, 45, 46, 47};

    e = vec_splat (i, (32-32));
    k = test_splat6 (32);
    j = vec_and (k, m6);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_u8(%i)", e[0]);
    print_vint8x (" = ", k);
    print_vint8x ("                  = ", j);
#endif
    rc += check_vui8 ("vec_splat6_u8 32", j, e);

    e = vec_splat (i, (33-32));
    k = test_splat6 (33);
    j = vec_and (k, m6);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_u8(%i)", e[0]);
    print_vint8x (" = ", k);
    print_vint8x ("                  = ", j);
#endif
    rc += check_vui8 ("vec_splat6_u8 33", j, e);

    e = vec_splat (i, (46-32));
    k = test_splat6 (46);
    j = vec_and (k, m6);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_u8(%i)", e[0]);
    print_vint8x (" = ", k);
    print_vint8x ("                  = ", j);
#endif
    rc += check_vui8 ("vec_splat6_u8 46", j, e);

    e = vec_splat (i, (47-32));
    k = test_splat6 (47);
    j = vec_and (k, m6);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_u8(%i)", e[0]);
    print_vint8x (" = ", k);
    print_vint8x ("                  = ", j);
#endif
    rc += check_vui8 ("vec_splat6_u8 47", j, e);

    i = (vui8_t) {48, 49, 50, 51, 52, 53, 54, 55,
                  56, 57, 58, 59, 60, 61, 62, 63};

    e = vec_splat (i, (48-48));
    k = test_splat6 (48);
    j = vec_and (k, m6);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_u8(%i)", e[0]);
    print_vint8x (" = ", k);
    print_vint8x ("                  = ", j);
#endif
    rc += check_vui8 ("vec_splat6_u8 48", j, e);

    e = vec_splat (i, (52-48));
    k = test_splat6 (52);
    j = vec_and (k, m6);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_u8(%i)", e[0]);
    print_vint8x (" = ", k);
    print_vint8x ("                  = ", j);
#endif
    rc += check_vui8 ("vec_splat6_u8 52", j, e);

    e = vec_splat (i, (55-48));
    k = test_splat6 (55);
    j = vec_and (k, m6);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_u8(%i)", e[0]);
    print_vint8x (" = ", k);
    print_vint8x ("                  = ", j);
#endif
    rc += check_vui8 ("vec_splat6_u8 55", j, e);

    e = vec_splat (i, (56-48));
    k = test_splat6 (56);
    j = vec_and (k, m6);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_u8(%i)", e[0]);
    print_vint8x (" = ", k);
    print_vint8x ("                  = ", j);
#endif
    rc += check_vui8 ("vec_splat6_u8 56", j, e);

    e = vec_splat (i, (63-48));
    k = test_splat6 (63);
    j = vec_and (k, m6);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_u8(%i)", e[0]);
    print_vint8x (" = ", k);
    print_vint8x ("                  = ", j);
#endif
    rc += check_vui8 ("vec_splat6_u8 53", j, e);

    return (rc);
  }

// #define __DEBUG_PRINT__ 1
#if 1
// test directly from vec_char_ppc.h
#define test_splats6(_l)	vec_splat6_s8(_l)
#else
// test from vec_char_ppc.h via vec_char_dummy.c
#endif

int
test_splat6_s8(void)
{
    vi8_t i, j, e;
    int rc = 0;
    printf ("\n%s\n", __FUNCTION__);

    i = (vi8_t) {0, 1, 2, 3, 4, 5, 6, 7,
                 8, 9, 10, 11, 12, 13, 14, 15};

    e = vec_splat (i, 1);
    j = test_splats6 (1);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_s8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat6_s8 1", (vui8_t) j, (vui8_t) e);

    e = vec_splat (i, 15);
    j = test_splats6 (15);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_s8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat6_s8 15", (vui8_t) j, (vui8_t) e);

    i = (vi8_t) {16, 17, 18, 19, 20, 21, 22, 23,
                  24, 25, 26, 27, 28, 29, 30, 31};

    e = vec_splat (i, (16-16));
    j = test_splats6 (16);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_s8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat6_s8 16", (vui8_t) j, (vui8_t) e);

    e = vec_splat (i, (17-16));
    j = test_splats6 (17);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_s8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat6_s8 17", (vui8_t) j, (vui8_t) e);

    e = vec_splat (i, (24-16));
    j = test_splats6 (24);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_e8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat6_e8 24", (vui8_t) j, (vui8_t) e);

    e = vec_splat (i, (30-16));
    j = test_splats6 (30);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_s8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat6_s8 30",(vui8_t) j, (vui8_t) e);

    e = vec_splat (i, (31-16));
    j = test_splats6 (31);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_s8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat6_s8 31",(vui8_t) j, (vui8_t) e);

    i = (vi8_t) {-32, -31, -30, -29, -28, -27, -26, -25,
                 -24, -23, -22, -21, -20, -19, -18, -17};

    e = vec_splat (i, (32-32));
    j = test_splats6 (-32);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_s8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat6_s8 -32", (vui8_t) j, (vui8_t) e);

    e = vec_splat (i, (32-25));
    j = test_splats6 (-25);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_s8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat6_s8 -25", (vui8_t) j, (vui8_t) e);

    e = vec_splat (i, (32-24));
    j = test_splats6 (-24);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_s8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat6_s8 -24", (vui8_t) j, (vui8_t) e);

    e = vec_splat (i, (32-23));
    j = test_splats6 (-23);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_s8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat6_s8 -23", (vui8_t) j, (vui8_t) e);

    e = vec_splat (i, (32-17));
    j = test_splats6 (-17);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_s8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat6_s8 -17", (vui8_t) j, (vui8_t) e);

    i = (vi8_t) {-16, -15, -14, -13, -12, -11, -10, -9,
                 -8, -7, -6, -5, -4, -3, -2, -1};

    e = vec_splat (i, (16-16));
    j = test_splats6 (-16);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_s8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat6_s8 -16", (vui8_t) j, (vui8_t) e);

    e = vec_splat (i, (16-8));
    j = test_splats6 (-8);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_s8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat6_s8 -8", (vui8_t) j, (vui8_t) e);

    e = vec_splat (i, (16-1));
    j = test_splats6 (-1);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat6_s8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat6_s8 -1", (vui8_t) j, (vui8_t) e);

    return (rc);
  }

//#define __DEBUG_PRINT__ 1
#if 1
// test directly from vec_char_ppc.h
#define test_splat7(_l)	vec_splat7_u8(_l)
#else
// test from vec_char_ppc.h via vec_char_dummy.c
#endif

int
test_splat7_u8(void)
{
    vui8_t i, j, k, e, m7;
    int rc = 0;
    printf ("\n%s\n", __FUNCTION__);

    m7 = (vui8_t) {127, 127, 127, 127, 127, 127, 127, 127,
      127, 127, 127, 127, 127, 127, 127, 127};

    i = (vui8_t) {0, 1, 2, 3, 4, 5, 6, 7,
                  8, 9, 10, 11, 12, 13, 14, 15};
    e = vec_splat (i, 1);
    j = test_splat7 (1);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat7_u8 1", j, e);

    e = vec_splat (i, 15);
    j = test_splat7 (15);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat7_u8 15", j, e);

    i = (vui8_t) {16, 17, 18, 19, 20, 21, 22, 23,
                  24, 25, 26, 27, 28, 29, 30, 31};

    e = vec_splat (i, (16-16));
    j = test_splat7 (16);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat7_u8 16", j, e);

    e = vec_splat (i, (17-16));
    j = test_splat7 (17);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat7_u8 17", j, e);
    e = vec_splat (i, (18-16));
    j = test_splat7 (18);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat7_u8 18", j, e);

    e = vec_splat (i, (24-16));
    j = test_splat7 (24);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat7_u8 24", j, e);
    e = vec_splat (i, (30-16));
    j = test_splat7 (30);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat7_u8 30", j, e);

    e = vec_splat (i, (31-16));
    j = test_splat7 (31);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x ("               = ", j);
#endif
    rc += check_vui8 ("vec_splat7_u8 31", j, e);

    i = (vui8_t) {32, 33, 34, 35, 36, 37, 38, 39,
                  40, 41, 42, 43, 44, 45, 46, 47};

    e = vec_splat (i, (32-32));
    k = test_splat7 (32);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x (" = ", k);
#endif
    rc += check_vui8 ("vec_splat7_u8 32", k, e);

    e = vec_splat (i, (33-32));
    k = test_splat7 (33);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x (" = ", k);
#endif
    rc += check_vui8 ("vec_splat7_u8 33", k, e);

    e = vec_splat (i, (40-32));
    k = test_splat7 (40);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x (" = ", k);
#endif
    rc += check_vui8 ("vec_splat7_u8 40", k, e);

    e = vec_splat (i, (47-32));
    k = test_splat7 (47);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x (" = ", k);
#endif
    rc += check_vui8 ("vec_splat7_u8 47", k, e);

    i = (vui8_t) {48, 49, 50, 51, 52, 53, 54, 55,
                  56, 57, 58, 59, 60, 61, 62, 63};

    e = vec_splat (i, (48-48));
    k = test_splat7 (48);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x (" = ", k);
#endif
    rc += check_vui8 ("vec_splat7_u8 48", k, e);

    e = vec_splat (i, (49-48));
    k = test_splat7 (49);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x (" = ", k);
#endif
    rc += check_vui8 ("vec_splat7_u8 49", k, e);

    e = vec_splat (i, (52-48));
    k = test_splat7 (52);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x (" = ", k);
#endif
    rc += check_vui8 ("vec_splat7_u8 52", k, e);

    e = vec_splat (i, (55-48));
    k = test_splat7 (55);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x (" = ", k);
#endif
    rc += check_vui8 ("vec_splat7_u8 55", k, e);

    e = vec_splat (i, (56-48));
    k = test_splat7 (56);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x (" = ", k);
#endif
    rc += check_vui8 ("vec_splat7_u8 56", k, e);

    e = vec_splat (i, (63-48));
    k = test_splat7 (63);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x (" = ", k);
#endif
    rc += check_vui8 ("vec_splat7_u8 63", k, e);

    i = (vui8_t) {64, 65, 66, 67, 68, 69, 70, 71,
                  72, 73, 74, 75, 76, 77, 78, 79};

    e = vec_splat (i, (64-64));
    k = test_splat7 (64);
    j = vec_and (k, m7);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x (" = ", k);
    print_vint8x ("                  = ", j);
#endif
    rc += check_vui8 ("vec_splat7_u8 64", j, e);

    e = vec_splat (i, (65-64));
    k = test_splat7 (65);
    j = vec_and (k, m7);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x (" = ", k);
    print_vint8x ("                  = ", j);
#endif
    rc += check_vui8 ("vec_splat7_u8 65", j, e);

    e = vec_splat (i, (72-64));
    k = test_splat7 (72);
    j = vec_and (k, m7);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x (" = ", k);
    print_vint8x ("                  = ", j);
#endif
    rc += check_vui8 ("vec_splat7_u8 72", j, e);

    e = vec_splat (i, (79-64));
    k = test_splat7 (79);
    j = vec_and (k, m7);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x (" = ", k);
    print_vint8x ("                  = ", j);
#endif
    rc += check_vui8 ("vec_splat7_u8 79", j, e);

    i = (vui8_t) {80, 81, 82, 83, 84, 85, 86, 87,
                  88, 89, 90, 91, 92, 93, 94, 95};

    e = vec_splat (i, (80-80));
    k = test_splat7 (80);
    j = vec_and (k, m7);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x (" = ", k);
    print_vint8x ("                  = ", j);
#endif
    rc += check_vui8 ("vec_splat7_u8 80", j, e);

    e = vec_splat (i, (81-80));
    k = test_splat7 (81);
    j = vec_and (k, m7);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x (" = ", k);
    print_vint8x ("                  = ", j);
#endif
    rc += check_vui8 ("vec_splat7_u8 81", j, e);

    e = vec_splat (i, (88-80));
    k = test_splat7 (88);
    j = vec_and (k, m7);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x (" = ", k);
    print_vint8x ("                  = ", j);
#endif
    rc += check_vui8 ("vec_splat7_u8 88", j, e);

    e = vec_splat (i, (95-80));
    k = test_splat7 (95);
    j = vec_and (k, m7);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x (" = ", k);
    print_vint8x ("                  = ", j);
#endif
    rc += check_vui8 ("vec_splat7_u8 95", j, e);

    i = (vui8_t) {96, 97, 98, 99, 100, 101, 102, 103,
                  104, 105, 106, 107, 108, 109, 110, 111};

    e = vec_splat (i, (96-96));
    k = test_splat7 (96);
    j = vec_and (k, m7);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x (" = ", k);
    print_vint8x ("                  = ", j);
#endif
    rc += check_vui8 ("vec_splat7_u8 96", j, e);

    e = vec_splat (i, (97-96));
    k = test_splat7 (97);
    j = vec_and (k, m7);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x (" = ", k);
    print_vint8x ("                  = ", j);
#endif
    rc += check_vui8 ("vec_splat7_u8 97", j, e);

    e = vec_splat (i, (98-96));
    k = test_splat7 (98);
    j = vec_and (k, m7);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x (" = ", k);
    print_vint8x ("                  = ", j);
#endif
    rc += check_vui8 ("vec_splat7_u8 98", j, e);

    e = vec_splat (i, (100-96));
    k = test_splat7 (100);
    j = vec_and (k, m7);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x (" = ", k);
    print_vint8x ("                  = ", j);
#endif
    rc += check_vui8 ("vec_splat7_u8 100", j, e);

    e = vec_splat (i, (102-96));
    k = test_splat7 (102);
    j = vec_and (k, m7);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x (" = ", k);
    print_vint8x ("                  = ", j);
#endif
    rc += check_vui8 ("vec_splat7_u8 102", j, e);

    e = vec_splat (i, (104-96));
    k = test_splat7 (104);
    j = vec_and (k, m7);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x (" = ", k);
    print_vint8x ("                  = ", j);
#endif
    rc += check_vui8 ("vec_splat7_u8 104", j, e);

    e = vec_splat (i, (111-96));
    k = test_splat7 (111);
    j = vec_and (k, m7);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x (" = ", k);
    print_vint8x ("                  = ", j);
#endif
    rc += check_vui8 ("vec_splat7_u8 111", j, e);

    i = (vui8_t) {112, 113, 114, 115, 116, 117, 118, 119,
                  120, 121, 122, 123, 124, 125, 126, 127};

    e = vec_splat (i, (112-112));
    k = test_splat7 (112);
    j = vec_and (k, m7);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x (" = ", k);
    print_vint8x ("                   = ", j);
#endif
    rc += check_vui8 ("vec_splat7_u8 112", j, e);

    e = vec_splat (i, (127-112));
    k = test_splat7 (127);
    j = vec_and (k, m7);

#ifdef __DEBUG_PRINT__
    printf       ("vec_splat7_u8(%i)", e[0]);
    print_vint8x (" = ", k);
    print_vint8x ("                   = ", j);
#endif
    rc += check_vui8 ("vec_splat7_u8 127", j, e);

    return (rc);
  }

//#define __DEBUG_PRINT__ 1
#if 0
// test directly from vec_char_ppc.h
#define test_expandm(_l)	vec_expandm_byte(_l)
#else
// test from vec_char_ppc.h via vec_char_dummy.c
extern vui8_t test_vec_expandm_byte (vui8_t);
#define test_expandm(_l)	test_vec_expandm_byte(_l)
#endif

int
test_expandm_byte(void)
{
    vui8_t i, j, e;
    int rc = 0;

    printf ("\n%s\n", __FUNCTION__);

    i = CONST_VINT128_B (0x00, 0x80, 0x01, 0xc0, 0x02, 0xe0, 0x04, 0xf0,
                         0x08, 0x81, 0x10, 0x83, 0x20, 0x87, 0x40, 0x8f);

    e = CONST_VINT128_B (0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff,
			 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff);
    j = (vui8_t) test_expandm (i);

#ifdef __DEBUG_PRINT__
    print_vint8x ("vec_expandm of ", i);
    print_vint8x ("             = ", j);
#endif
    rc += check_vui8 ("vec_expandm", j, e);

    return (rc);
  }

int
test_vec_char (void)
{
  int rc = 0;

  printf ("\n%s\n", __FUNCTION__);
#if 1
  rc += test_clzb ();
  rc += test_ctzb ();
  rc += test_popcntb ();
  rc += test_vec_ischar ();
  rc += test_mrgeob ();
  rc += test_mrgahlb ();
  rc += test_mulhub ();
  rc += test_mulhsb ();
  rc += test_mulubm ();
  rc += test_setbb ();
  rc += test_vec_cmpnezb ();
  rc += test_vec_clzlsbb ();
  rc += test_vec_ctzlsbb ();
  rc += test_vec_cntlz_lsbb ();
  rc += test_vec_cnttz_lsbb ();
  rc += test_first_match ();
  rc += test_first_match_or_eos ();
  rc += test_first_mismatch ();
  rc += test_first_mismatch_or_eos ();
  rc += test_lsbb ();
  rc += test_clear_left ();
  rc += test_clear_right ();
  rc += test_clear_left_justified ();
  rc += test_clear_right_justified ();
  rc += test_isolate_right_justified_p ();
  rc += test_isolate_left_justified_p ();
  rc += test_splat6_u8 ();
  rc += test_splat6_s8 ();
  rc += test_splat7_u8 ();
  rc += test_expandm_byte ();
#endif
  return (rc);
}
