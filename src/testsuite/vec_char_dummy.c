/*
   vec_char_dummy.c
  
    Created on: Jul 31, 2017
        Author: sjmunroe
  */

#include <stdint.h>

//#define __DEBUG_PRINT__

#include <pveclib/vec_char_ppc.h>

vb8_t
test_setb_sb (vi8_t vra)
{
  return vec_setb_sb (vra);
}

vui8_t
test_ctzb_v1 (vui8_t vra)
{
  const vui8_t ones = vec_splat_u8 (1);
  const vui8_t c8s = vec_splat_u8 (8);
  vui8_t term;
  // term = (!vra & (vra - 1))
  term = vec_andc (vec_sub (vra, ones), vra);
  // return = 8 - vec_clz (!vra & (vra - 1))
  return vec_sub (c8s, vec_clzb (term));
}

vui8_t
test_ctzb_v2 (vui8_t vra)
{
  const vui8_t ones = vec_splat_u8 (1);
  vui8_t term;
  // term = (!vra & (vra - 1))
  term = vec_andc (vec_sub (vra, ones), vra);
  // return = vec_popcnt (!vra & (vra - 1))
  return vec_popcntb (term);
}

vui8_t
test_ctzb_v3 (vui8_t vra)
{
  const vui8_t zeros = vec_splat_u8 (0);
  const vui8_t c8s = vec_splat_u8 (8);
  vui8_t term;
  // term = (vra | -vra))
  term = vec_or (vra, vec_sub (zeros, vra));
  // return = 8 - vec_poptcnt (vra & -vra)
  return vec_sub (c8s, vec_popcntb (term));
}

vui8_t
test_ctzb (vui8_t vra)
{
  return vec_ctzb (vra);
}

vui8_t
test_clzb (vui8_t vra)
{
  return vec_clzb (vra);
}

vui8_t
test_popcntb (vui8_t vra)
{
  return vec_popcntb (vra);
}

#if __GNUC__ >= 7
/* Generic vec_mul not supported for vector char until GCC 7.  */
vui8_t
__test_mulubm_gcc (vui8_t __A, vui8_t __B)
{
  return vec_mul (__A, __B);
}
#endif

vui8_t
__test_mulubm (vui8_t __A, vui8_t __B)
{
  return vec_mulubm (__A, __B);
}

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

vui8_t
test_vec_mrgalb (vui16_t vra, vui16_t vrb)
{
  return vec_mrgalb (vra, vrb);
}

vui8_t
test_vec_vmrgob (vui8_t vra, vui8_t vrb)
{
  return vec_vmrgob (vra, vrb);
}

vui8_t
test_vec_packub  (vui8_t a, vui8_t b)
{
  return vec_pack ((vui16_t) a, (vui16_t) b);
}
