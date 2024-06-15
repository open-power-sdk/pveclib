/*
   vec_char_dummy.c
  
    Created on: Jul 31, 2017
        Author: sjmunroe
  */

#include <stdint.h>

//#define __DEBUG_PRINT__

#include <pveclib/vec_char_ppc.h>

int
test_vec_cntlz_lsbb_bi (vui8_t vra)
{
  return vec_cntlz_lsbb_bi (vra);
}

int
test_vec_cnttz_lsbb_bi (vui8_t vra)
{
  return vec_cnttz_lsbb_bi (vra);
}

int
test_cntlz_lsbb_bi (vui8_t vra)
{
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  return vec_vctzlsbb (vra);
#else
  return vec_vclzlsbb (vra);
#endif
}

int
test_cnttz_lsbb_bi (vui8_t vra)
{
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  return vec_vclzlsbb (vra);
#else
  return vec_vctzlsbb (vra);
#endif
}

int
test_vec_vclzlsbb (vui8_t vra)
{
  return vec_vclzlsbb (vra);
}
int
test_vec_vctzlsbb (vui8_t vra)
{
  return vec_vctzlsbb (vra);
}

int
test_vclzlsbb (vui8_t vra)
{
  int result;
#ifdef _ARCH_PWR9
#ifdef vec_cntlz_lsbb
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) && ( __GNUC__ >= 12)
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
  const vui8_t zeros = vec_splat_u8(0);
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
  vui8_t gbb;
  vui16_t lsbb, clzls;
  long long int dwres;
  // PowerISA 2.07 (P8) has:
  // Vector Gather Bits by Bytes and Count Leading Zeros
  // So use vec_gb to collect the Least-Significant Bits by Byte
  // Vec_gb is Gather Bits by Bytes by Doubleword
  // so the lsbb's are in bytes 7 and 15.
  // Use vec_perm to collect all 16 lsbb's into a single Halfword.
  // Then vec_cntlz to count the leading zeros within lsbb's
  gbb = vec_gb (vra);
  lsbb = (vui16_t) vec_perm (gbb, zeros, pgbb);
  clzls = vec_cntlz (lsbb);
  dwres = ((vui64_t) clzls) [VEC_DW_H];
  result = (unsigned short) dwres;
#else
  const vui8_t zeros = vec_splat_u8(0);
  const vui8_t LSBBmask = vec_splat_u8(1);
  const vui8_t LSBBshl = CONST_VINT128_B (3, 2, 1, 0, 3, 2, 1, 0,
					  3, 2, 1, 0, 3, 2, 1, 0);
  const vui32_t LSBWshl = CONST_VINT128_W (12, 8, 4, 0);
  vui8_t gbb, gbbsb;
  vui32_t gbbsw;
  long long int dwres;

  gbb = vec_and (vra, LSBBmask);
  // merge lsbb into nibbles by word
  gbbsb = vec_sl (gbb, LSBBshl);
  gbbsw = vec_sum4s (gbbsb, (vui32_t) zeros);
  // merge lsbw into halfword by word
  gbbsw = vec_sl (gbbsw, LSBWshl);
  gbbsw = (vui32_t) vec_sums ((vi32_t) gbbsw, (vi32_t) zeros);
  // transfer from vector to GPR
  dwres = ((vui64_t) gbbsw) [VEC_DW_L];
  // Use GCC Builtin to get final leading zero count
  // with fake unsigned short clz
  result = __builtin_clz ((unsigned int) (dwres)) - 16;

#endif
  return result;
}

int
test_vctzlsbb (vui8_t vra)
{
  int result;
#ifdef _ARCH_PWR9
#ifdef vec_cnttz_lsbb
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) && ( __GNUC__ >= 12)
  result = vec_cntlz_lsbb (vra);
#else
  result = vec_cnttz_lsbb (vra);
#endif
#else
  __asm__(
      "vctzlsbb %0,%1;"
      : "=r" (result)
      : "v" (vra)
      : );
#endif
#elif _ARCH_PWR8
  const vui16_t zeros = vec_splat_u16 (0);
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
  vui8_t gbb;
  vui16_t lsbb, ctzls, tzmask;
  long long int dwres;
  // PowerISA 2.07 (P8) has:
  // Vector Gather Bits by Bytes but no Count Trailing Zeros.
  // It does have Vector Population Count.
  // So after collecting the lsbb's into a Halfword we can use the
  // formula !(lsbb | -lsbb) to Generate 1's for the trailing
  // zeros and 0's otherwise. Then count the 1's (via vec_popcnt())
  // to generate the count of trailing zeros
  gbb = vec_gb (vra);
  lsbb = (vui16_t) vec_perm (gbb, (vui8_t) zeros, pgbb);
  // tzmask = !(lsbb | -lsbb)
  // tzmask = !(lsbb | (0-lsbb))
  tzmask = vec_nor (lsbb, vec_sub (zeros, lsbb));
  // return = vec_popcnt (!lsbb & (lsbb - 1))
  ctzls = vec_popcnt (tzmask);
  dwres = ((vui64_t) ctzls) [VEC_DW_H];
  result = (unsigned short) dwres;
#else
  const vui8_t zeros = vec_splat_u8(0);
  const vui8_t LSBBmask = vec_splat_u8(1);
  const vui8_t LSBBshl = CONST_VINT128_B (3, 2, 1, 0, 3, 2, 1, 0,
					  3, 2, 1, 0, 3, 2, 1, 0);
  const vui32_t LSBWshl = CONST_VINT128_W (12, 8, 4, 0);
  vui8_t gbb, gbbsb;
  vui32_t gbbsw;
  long long int dwres;

  // Mask the least significant bit of each byte
  gbb = vec_and (vra, LSBBmask);
  // merge lsbb into nibbles by word
  gbbsb = vec_sl (gbb, LSBBshl);
  gbbsw = vec_sum4s (gbbsb, (vui32_t) zeros);
  // merge lsbw into halfword by word
  gbbsw = vec_sl (gbbsw, LSBWshl);
  gbbsw = (vui32_t) vec_sums ((vi32_t) gbbsw, (vi32_t) zeros);
  // transfer from vector to GPR
  dwres = ((vui64_t) gbbsw) [VEC_DW_L];
  // Use GCC Builtin to get final trailing zero count
  // with fake unsigned short ctz
  result = __builtin_ctz ((unsigned int) (dwres+0x10000));
#endif

  return result;
}

long long int
test_vctzlsbb_V0 (vui8_t vra)
{
  long long int result;
#ifdef _ARCH_PWR9
#ifdef vec_cnttz_lsbb
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) && ( __GNUC__ >= 12)
  result = vec_cntlz_lsbb (vra);
#else
  result = vec_cnttz_lsbb (vra);
#endif
#else
  __asm__(
      "vctzlsbb %0,%1;"
      : "=r" (result)
      : "v" (vra)
      : );
#endif
#elif _ARCH_PWR8
  vui8_t gbb;
  vui16_t lsbb, ctzls;
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
  // PowerISA 2.07 (P8) has:
  // Vector Gather Bits by Bytes but no Count Trailing Zeros.
  // It does have Vector Population Count.
  // So after collecting the lsbb's into a Halfword we can use the
  // formula (!lsbb & (lsbb - 1)) to Generate 1's for the trailing
  // zeros and 0's otherwise. Then count the 1's (via vec_popcnt())
  // to generate the count of trailing zeros
  gbb = vec_gb (vra);
#if 0
  lsbb = (vui16_t) vec_perm (gbb, pgbb, pgbb);
#else
  lsbb = (vui16_t) vec_perm (gbb, VEOS, pgbb);
#endif
  //ctzls = vec_cntlz (lsbb);
  { // No Count Trailing Zeros in PowerISA 2.07 or earlier. so ...
    // For _ARCH_PWR8 and earlier. Generate 1's for the trailing zeros
    // and 0's otherwise. Then count (popcnt) the 1's. _ARCH_PWR8 uses
    // the hardware vpopcnth instruction.
      vui16_t tzmask;
#if 0
      const vui16_t ones = vec_splat_u16 (1);
      // tzmask = (!lsbb & (lsbb - 1))
      tzmask = vec_andc (vec_sub (lsbb, ones), lsbb);
#else
#if 0
      // tzmask = !(lsbb | -lsbb)
      tzmask = vec_nor ((vui16_t)vec_neg ((vi16_t)lsbb), lsbb);
#else
      const vui16_t zeros = vec_splat_u16 (0);
      // tzmask = !(lsbb | (0-lsbb))
      tzmask = vec_nor (lsbb, vec_sub (zeros, lsbb));
#endif
#endif
      // return = vec_popcnt (!lsbb & (lsbb - 1))
      ctzls = vec_popcnt (tzmask);
  }
  result = ((vui64_t) ctzls) [VEC_DW_H];
  result = (unsigned short) result;
#else
  const vui8_t zeros = vec_splat_u8(0);
  const vui8_t LSBBmask = vec_splat_u8(1);
  const vui8_t LSBBshl = CONST_VINT128_B (3, 2, 1, 0, 3, 2, 1, 0,
					  3, 2, 1, 0, 3, 2, 1, 0);
  const vui32_t LSBWshl = CONST_VINT128_W (12, 8, 4, 0);
  vui8_t gbb, gbbsb;
  vui32_t gbbsw;

  // Mask the least significant bit of each byte
  gbb = vec_and (vra, LSBBmask);
  // merge lsbb into nibbles by word
  gbbsb = vec_sl (gbb, LSBBshl);
  gbbsw = vec_sum4s (gbbsb, (vui32_t) zeros);
  // merge lsbw into halfword by word
  gbbsw = vec_sl (gbbsw, LSBWshl);
  gbbsw = (vui32_t) vec_sums ((vi32_t) gbbsw, (vi32_t) zeros);
  // transfer from vector to GPR
  result = ((vui64_t) gbbsw) [VEC_DW_L];
  // Use GCC Builtin to get final trailing zero count
  // with fake unsigned short ctz
  result = __builtin_ctz ((unsigned int) (result+0x10000));
#endif

  return result;
}

vb8_t
test_vec_vcmpnezb (vui8_t vra, vui8_t vrb)
{ // from vec_char_ppc.h
  return vec_vcmpnezb (vra, vrb);
}

vb8_t
test_vcmpnezb_v0 (vui8_t vra, vui8_t vrb)
{
  vb8_t result;
#ifdef _ARCH_PWR9
#ifdef vec_cmpnez
  result = vec_cmpnez (vra, vrb);
#else
  __asm__(
      "vcmpnezb %0,%1,%2;"
      : "=v" (result)
      : "v" (vra), "v" (vrb)
      : );
#endif
#else
  const vui8_t VEOS = vec_splat_u8(0);
  vb8_t eosa, eosb, eosc, abne;

  abne = vec_cmpne (vra, vrb);
  eosa = vec_cmpeq (vra, VEOS);
  eosb = vec_cmpeq (vrb, VEOS);
  eosc = vec_or (eosa, eosb);
  result = vec_or (abne, eosc);
#endif
  return result;
}

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
test_vctzb (vui8_t vra)
{ // from vec_char_ppc.h
  return vec_ctzb (vra);
}

vui8_t
test_vclzb (vui8_t vra)
{ // from vec_char_ppc.h
  return vec_clzb (vra);
}

vui8_t
test_vpopcntb (vui8_t vra)
{ // from vec_char_ppc.h
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
