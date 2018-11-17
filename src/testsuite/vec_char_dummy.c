/*
   vec_char_dummy.c
  
    Created on: Jul 31, 2017
        Author: sjmunroe
  */


#include <stdint.h>

//#define __DEBUG_PRINT__

#include "vec_char_ppc.h"

#if __GNUC__ >= 8
/* Generic vec_mul not supported for vector char until GCC 8.  */
vui8_t
__test_mulubm (vui8_t __A, vui8_t __B)
{
  return vec_mul (__A, __B);
}
#endif

vui8_t
__test_absdub (vui8_t __A, vui8_t __B)
{
  return vec_absdub (__A, __B);
}

vui8_t
__test_sldo (vui8_t __A, vui8_t __B, vui8_t __C)
{
  return vec_shift_leftdo (__A, __B, __C);
}


vui8_t
__test_clzb (vui8_t a)
{
  return (vec_clzb (a));
}

vui8_t
test_vec_slbi_4 (vui8_t a)
{
  return vec_slbi (a, 4);
}

vi8_t
test_vec_srabi_4 (vi8_t a)
{
  return vec_srabi (a, 4);
}

vui8_t
test_vec_srbi_4 (vui8_t a)
{
  return vec_srbi (a, 4);
}

vui8_t
test_vec_slbi_8 (vui8_t a)
{
  return vec_slbi (a, 8);
}

vi8_t
test_vec_srabi_8 (vi8_t a)
{
  return vec_srabi (a, 8);
}

vui8_t
test_vec_srbi_8 (vui8_t a)
{
  return vec_srbi (a, 8);
}

vui8_t
test_vec_slbi (vui8_t a, const unsigned int shb)
{
  return vec_slbi (a, shb);
}

vi8_t
test_vec_srabi (vi8_t a, const unsigned int shb)
{
  return vec_srabi (a, shb);
}

vui8_t
test_vec_srbi (vui8_t a, const unsigned int shb)
{
  return vec_srbi (a, shb);
}

vui8_t
test_vec_isalnum (vui8_t a)
{
  return vec_isalnum (a);
}
vui8_t
test_vec_isdigit (vui8_t a)
{
  return vec_isdigit (a);
}

vui8_t
test_vec_isalnum2 (vui8_t a)
{
  return vec_or (vec_isalpha (a), vec_isdigit (a));
}

vui8_t
test_vec_toupper (vui8_t a)
{
  return vec_toupper (a);
}

vui8_t
test_vec_tolower (vui8_t a)
{
  return vec_tolower (a);
}

vui8_t
test_vec_mergel  (vui8_t a, vui8_t b)
{
  return vec_mergel (a, b);
}

vui8_t
test_vec_vmrglb  (vui8_t a, vui8_t b)
{
  return vec_vmrglb (a, b);
}
