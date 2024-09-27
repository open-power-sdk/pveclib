/*
   vec_char_dummy.c
  
    Created on: Jul 31, 2017
        Author: sjmunroe
  */

#include <stdint.h>

//#define __DEBUG_PRINT__

#include <pveclib/vec_char_ppc.h>

vui8_t
test_vec_popcntb (vui8_t vra)
{
  return vec_popcntb (vra);
}

vui8_t
test_vec_popcntb_PWR7 (vui8_t vra)
{
  return vec_popcntb_PWR7 (vra);
}

vui8_t
test_popcntb_PWR7 (vui8_t vra)
{
  const vui8_t nibmask = vec_splat_u8 (15);
  const vui8_t nibshft = vec_splat_u8 (4);
  const vui8_t popc_perm = { 0, 1, 1, 2, 1, 2, 2, 3,
                             1, 2, 2, 3, 2, 3, 3, 4};
  vui8_t nib_h, nib_l;
  vui8_t result, rl, rh;

  nib_l = vec_and (vra, nibmask);
  nib_h = vec_sr  (vra, nibshft);
  rl = vec_perm (popc_perm, popc_perm, nib_l);
  rh = vec_perm (popc_perm, popc_perm, nib_h);
  result = vec_add (rh, rl);

  return result;
}

vui8_t
test_vec_vstribr (vui8_t vra)
{
  return vec_vstribr (vra);
}

int
test_vec_vstribr_p (vui8_t vra)
{
  return vec_vstribr_p (vra);
}

int
test_vstribr_p (vui8_t vra)
{
#if defined(_ARCH_PWR10)
#if ((__GNUC__ > 10) || (defined(__clang__) && (__clang_major__ > 12)))
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  return vec_stril_p (vra);
#else
  return vec_strir_p (vra);
#endif
#else
  vui8_t tmp;
  int result;
  __asm__(
      "vstribr. %1,%2;"
      "setbc %0,26;"
      : "=r" (result), "=v" (tmp)
      : "v" (vra)
      : "cr6");
  return result;
#endif
#else
  const vui8_t zeros = vec_splat_u8(0);
  return vec_any_eq (vra, zeros);
#endif
}

vui8_t
test_vstribr (vui8_t vra)
{
#if defined(_ARCH_PWR10)
#if ((__GNUC__ > 10) || (defined(__clang__) && (__clang_major__ > 12)))
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  return vec_stril (vra);
#else
  return vec_strir (vra);
#endif
#else
  vui8_t result;
  __asm__(
      "vstribr %0,%1;"
      : "=v" (result)
      : "v" (vra)
      : );
  return result;
#endif
#else
  const vui8_t zeros = vec_splat_u8(0);
  const vui8_t ones = (vui8_t) vec_splat_s8(-1);
  vui8_t nulchr, clrmask, clrcnt, result;

  result = vra;
  if (vec_any_eq(vra, zeros))
    {
      nulchr = (vui8_t) vec_cmpeq (vra, zeros);
      // clrmask == ones unless nullchr == zeros
      clrmask = ones;
#if 1
      // No Quadword Vector Count Trailing Zeros yet. So
      // for POWER7/8/9 we can use the PVECLIB vec_ctzq_PWR9/8/7
      // Quadword operations from vec_common_ppc.h.
      clrcnt = (vui8_t) vec_ctzq_PWR9 ((vui128_t) nulchr);
#else
	{ // quadword ctz of nulchar
#ifdef _ARCH_PWR8
	// For POWER8/9 we can use the Vector Count Leading Zeros
	// DoubleWord instruction then sum across to get the
	// quadword clz.
	  clrcnt = (vui8_t) vec_ctzq_PWR8 ((vui128_t) nulchr);
#else
	  /* vector clz instructions were introduced in power8. For power7 and
	   * earlier, use the pveclib vec_clzq_PWR7 implementation. */
	  clrcnt = (vui8_t) vec_ctzq_PWR7 ((vui128_t) nulchr);
#endif
	}
#endif
      // Shift clrmask left by quadword ctz of nulchar
      // leaving 0x00 bytes from the first null char to byte 15 in vra
      clrmask = vec_slo (clrmask, clrcnt);
      // And compliment to clear trailing bytes before first nulchr
      result = vec_andc (vra, clrmask);
    }
  return result;
#endif
}

vui8_t
test_vec_vstribl (vui8_t vra)
{
  return vec_vstribl (vra);
}

int
test_vec_vstribl_p (vui8_t vra)
{
  return vec_vstribl_p (vra);
}

int
test_vstribl_p (vui8_t vra)
{
#if defined(_ARCH_PWR10)
#if ((__GNUC__ > 10) || (defined(__clang__) && (__clang_major__ > 12)))
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  return vec_strir_p (vra);
#else
  return vec_stril_p (vra);
#endif
#else
  vui8_t tmp;
  int result;
  __asm__(
      "vstribl. %1,%2;"
      "setbc %0,26;"
      : "=r" (result), "=v" (tmp)
      : "v" (vra)
      : "cr6");
  return result;
#endif
#else
  const vui8_t zeros = vec_splat_u8(0);
  return vec_any_eq (vra, zeros);
#endif
}

vui8_t
test_vstribl (vui8_t vra)
{
#if defined(_ARCH_PWR10)
#if ((__GNUC__ > 10) || (defined(__clang__) && (__clang_major__ > 12)))
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  return vec_strir (vra);
#else
  return vec_stril (vra);
#endif
#else
  vui8_t result;
  __asm__(
      "vstribl %0,%1;"
      : "=v" (result)
      : "v" (vra)
      : );
  return result;
#endif
#else
  const vui8_t zeros = vec_splat_u8(0);
  const vui8_t ones = (vui8_t) vec_splat_s8(-1);
  vui8_t nulchr, clrmask, clrcnt, result;

  result = vra;
  if (vec_any_eq(vra, zeros))
    {
      nulchr = (vui8_t) vec_cmpeq (vra, zeros);
      // clrmask == ones unless nullchr == zeros
      clrmask = ones;
#if 1
      // No Quadword Vector Count Leading Zeros yet. So
      // for POWER7/8/9 we can use the PVECLIB vec_clzq_PWR8/7
      // Quadword operations from vec_common_ppc.h.
      clrcnt = (vui8_t) vec_clzq_PWR8 ((vui128_t) nulchr);
#else
	{ // quadword clz of nulchar
#ifdef _ARCH_PWR8
	// For POWER8/9 we can use the Vector Count Leading Zeros
	// DoubleWord instruction then sum across to get the
	// quadword clz.
	  clrcnt = (vui8_t) vec_clzq_PWR8 ((vui128_t) nulchr);
#else
	  /* vector clz instructions were introduced in power8. For power7 and
	   * earlier, use the pveclib vec_clzq_PWR7 implementation. */
	  clrcnt = (vui8_t) vec_clzq_PWR7 ((vui128_t) nulchr);
#endif
	}
#endif
      // Shift clrmask right by quadword clz of nulchar
      // leaving 0x00 bytes from byte 0 to first null char in vra
      clrmask = vec_sro (clrmask, clrcnt);
      // And compliment to clear trailing bytes after first nulchr
      result = vec_andc (vra, clrmask);
    }
  return result;
#endif
}

vui8_t
test_vec_vclrlb (vui8_t vra, unsigned int rb)
{
  return vec_vclrlb (vra, rb);
}

vui8_t
test_vec_vclrrb (vui8_t vra, unsigned int rb)
{
  return vec_vclrrb (vra, rb);
}

// Verify CSE of mask generation
vui8_t
test_vec_vclrxb_rb79 (vui8_t vral, vui8_t vrar)
{
  vui8_t left, right;
  right = vec_vclrrb (vrar, 9);
  left  = vec_vclrlb (vral, 7);
  return vec_or (left, right);
}

vui8_t
test_vclrlb (vui8_t vra, unsigned int rb)
{
#if defined(_ARCH_PWR10)
#if ((__GNUC__ > 10) || (defined(__clang__) && (__clang_major__ > 12)))
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  return vec_clrr (vra, rb);
#else
  return vec_clrl (vra, rb);
#endif
#else
  vui8_t result;
  __asm__(
      "vclrlb %0,%1,%2;"
      : "=v" (result)
      : "v" (vra), "r" (rb)
      : );
  return result;
#endif
#else
  const vui8_t zeros = vec_splat_u8(0);
  const vui8_t ones = (vui8_t) vec_splat_s8(-1);
  vui8_t clrmask;
  // In case rb==0 clear all 16 bytes.
  vui8_t result = vec_splat_u8(0);
  unsigned int N;
  // rb == 0 clears 16 bytes
  if (rb != 0)
    {
      // rb >= 16 clears no bytes
      clrmask = ones;
      if (rb < 16)
	{
	  // Clear N bytes in the range 15-1
	  N = 16 - rb;
	  if (__builtin_constant_p(rb))
	    {
	      // If rb/N is const use vsldoi
	      clrmask = vec_sld (zeros, clrmask, rb);
	    }
	  else
	    { // otherwise xfer N*8 to VR and use vslo
#if defined(_ARCH_PWR8)
	      // Take advantage of P8 VSX and direct move.
	      vui64_t shfcnt;
	      shfcnt[VEC_DW_L] = N * 8;
#else
	      vui32_t shfcnt;
	      shfcnt [VEC_W_L] = N * 8;
#endif
	      clrmask = vec_sro (clrmask, (vui8_t) shfcnt);
	    }
	}
      return vec_and (vra, clrmask);
    }
  return result;
#endif
}

vui8_t
test_vclrlb_rb0 (vui8_t vra)
{
  return test_vclrlb (vra, 0);
}

vui8_t
test_vclrlb_rb1 (vui8_t vra)
{
  return test_vclrlb (vra, 1);
}

vui8_t
test_vclrlb_rb15 (vui8_t vra)
{
  return test_vclrlb (vra, 15);
}

vui8_t
test_vclrlb_rb16 (vui8_t vra)
{
  return test_vclrlb (vra, 16);
}

vui8_t
test_vclrrb (vui8_t vra, unsigned int rb)
{
#if defined(_ARCH_PWR10)
#if ((__GNUC__ > 10) || (defined(__clang__) && (__clang_major__ > 12)))
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  return vec_clrl (vra, rb);
#else
  return vec_clrr (vra, rb);
#endif
#else
  vui8_t result;
  __asm__(
      "vclrrb %0,%1,%2;"
      : "=v" (result)
      : "v" (vra), "r" (rb)
      : );
  return result;
#endif
#else
  const vui8_t zeros = vec_splat_u8(0);
  const vui8_t ones = (vui8_t) vec_splat_s8(-1);
  vui8_t clrmask;
  vui8_t result = vec_splat_u8(0);
  unsigned int N;

  if ( rb != 0)
    {
      // rb >= 16 clears no bytes
      clrmask = ones;
      if (rb < 16)
	{
	  // Clear N bytes in the range 15-1
	  N = 16 - rb;
	  if (__builtin_constant_p(rb))
	    {
	      // If rb/N is const use vsldoi
	      clrmask = vec_sld (clrmask, zeros, N);
	    }
	  else
	    { // otherwise xfer N*8 to VR and use vslo
#if defined(_ARCH_PWR8)
	      // Take advantage of P8 VSX and direct move.
	      vui64_t shfcnt;
	      shfcnt[VEC_DW_L] = N * 8;
#else
	      vui32_t shfcnt;
	      shfcnt [VEC_W_L] = N * 8;
#endif
	      clrmask = vec_slo (clrmask, (vui8_t) shfcnt);
	    }
	}
      result = vec_and (vra, clrmask);
    }
  return result;
#endif
}

vui8_t
test_vclrrb_rb0 (vui8_t vra)
{
  return test_vclrrb (vra, 0);
}

vui8_t
test_vclrrb_rb1 (vui8_t vra)
{
  return test_vclrrb (vra, 1);
}

vui8_t
test_vclrrb_rb8 (vui8_t vra)
{
  return test_vclrrb (vra, 8);
}

vui8_t
test_vclrrb_rb15 (vui8_t vra)
{
  return test_vclrrb (vra, 15);
}

vui8_t
test_vclrrb_rb16 (vui8_t vra)
{
  return test_vclrrb (vra, 16);
}

// Verify CSE of mask generation
vui8_t
test_vclrxb_rb79 (vui8_t vral, vui8_t vrar)
{
  vui8_t left, right;
  right = test_vclrrb (vrar, 9);
  left  = test_vclrlb (vral, 7);
  return vec_or (left, right);
}

// Verify constant propagation for rb
vui8_t
test_vclrxb_rb79x (vui8_t vral, vui8_t vrar)
{
  vui8_t left, right;
  int rb;
  rb = 9;
  right = test_vclrrb (vrar, rb);
  rb = 7;
  left  = test_vclrlb (vral, rb);
  return vec_or (left, right);
}

int
test_vec_first_match_byte_index (vui8_t vra, vui8_t vrb)
{
  return vec_first_match_byte_index (vra, vrb);
}

int
test_vec_first_mismatch_byte_index (vui8_t vra, vui8_t vrb)
{
  return vec_first_mismatch_byte_index (vra, vrb);
}

int
test_vec_first_match_byte_or_eos_index (vui8_t vra, vui8_t vrb)
{
  return vec_first_match_byte_or_eos_index (vra, vrb);
}

int
test_vec_first_mismatch_byte_or_eos_index (vui8_t vra, vui8_t vrb)
{
  return vec_first_mismatch_byte_or_eos_index (vra, vrb);
}

int
test_first_match_byte_index (vui8_t vra, vui8_t vrb)
{
#if defined(_ARCH_PWR9) && (__GNUC__ > 10)
  return vec_first_match_index (vra, vrb);
#else
  vui8_t abeq;
  int result;
  abeq = (vui8_t) vec_cmpeq(vra, vrb);
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  result = vec_vctzlsbb (abeq);
#else
  result = vec_vclzlsbb (abeq);
#endif
  return result;
#endif
}

int
test_first_mismatch_byte_index (vui8_t vra, vui8_t vrb)
{
#if defined(_ARCH_PWR9) && (__GNUC__ > 10)
  return vec_first_mismatch_index (vra, vrb);
#else
  vui8_t abeq;
  int result;
  abeq = (vui8_t) vec_vcmpneb(vra, vrb);
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  result = vec_vctzlsbb (abeq);
#else
  result = vec_vclzlsbb (abeq);
#endif
  return result;
#endif
}

int
test_first_match_byte_or_eos_index (vui8_t vra, vui8_t vrb)
{
#if defined(_ARCH_PWR9) && (__GNUC__ > 10)
  return vec_first_match_or_eos_index (vra, vrb);
#else
  const vui8_t VEOS = vec_splat_u8(0);
  vui8_t abeq;
  vb8_t eosa, eosb, eosc;
  int result;
#if 1
  vb8_t ab_b;
  eosa = vec_cmpeq (vra, VEOS);
  eosb = vec_cmpeq (vrb, VEOS);
  ab_b = vec_cmpeq (vra, vrb);
  eosc = vec_or (eosa, eosb);
  abeq = (vui8_t) vec_or (ab_b, eosc);
#else
  vb8_t abnez, abne;
  eosa =  vec_vcmpneb(vra, VEOS);
  eosb =  vec_vcmpneb(vrb, VEOS);
  abnez = vec_vcmpnezb(vra, vrb);
  eosc = vec_and (eosa, eosb);
#ifdef _ARCH_PWR8
  abeq = (vui8_t) vec_nand (eosc, abnez);
#else
  abeq = (vui8_t) vec_and (eosc, abnez);
  abeq = (vui8_t) vec_nor (abeq, abeq);
#endif
#endif
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  result = vec_vctzlsbb (abeq);
#else
  result = vec_vclzlsbb (abeq);
#endif
  return result;
#endif
}

int
test_first_mismatch_byte_or_eos_index (vui8_t vra, vui8_t vrb)
{
#ifdef _ARCH_PWR9
#if (__GNUC__ > 13)
  return vec_first_mismatch_or_eos_index (vra, vrb);
#else
  vui8_t abnez;
  abnez = (vui8_t) vec_vcmpnezb (vra, vrb);
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  return vec_vctzlsbb (abnez);
#else
  return vec_vclzlsbb (abnez);
#endif

#endif
#else
  const vui8_t VEOS = vec_splat_u8(0);
  vui8_t abnez;
  vb8_t eosa, eosb, eosc;
  int result;
  vb8_t ab_b;
  eosa = vec_cmpeq (vra, VEOS);
  eosb = vec_cmpeq (vrb, VEOS);
  // vcmpneb requires _ARCH_PWR9, so use NOT cmpeq
  ab_b = vec_cmpeq (vra, vrb);
  eosc = vec_or (eosa, eosb);
#ifdef _ARCH_PWR8
  abnez = (vui8_t) vec_orc (eosc, ab_b);
#else // vorc requires _ARCH_PWR8, so use cmpeq, nor and or
  abnez = (vui8_t) vec_nor (ab_b, ab_b);
  abnez = vec_or ((vui8_t) eosc, abnez);
#endif
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  result = vec_vctzlsbb (abnez);
#else
  result = vec_vclzlsbb (abnez);
#endif
  return result;
#endif
}

int
test_vec_lsbb_all_ones (vui8_t vra)
{
  return vec_testlsbb_all_ones (vra);
}

int
test_vec_lsbb_all_zeros (vui8_t vra)
{
  return vec_testlsbb_all_zeros (vra);
}

int
test_lsbb_all_ones (vui8_t vra)
{
#if defined(_ARCH_PWR10) && (__GNUC__ > 9)
  int r;
  __asm__(
      "xvtlsbb 6,%x1;\n"
      "setbc   %0,24;\n"
      : "=r" (r)
      : "wa" (vra)
      : "cr6"
	);
  return r;
#else
  const vui8_t ones = vec_splat_u8(1);
  vui8_t lsbb;

  lsbb = vec_and (vra, ones);
  return vec_all_eq (lsbb, ones);
#endif
}

int
test_lsbb_all_zeros (vui8_t vra)
{
#if defined(_ARCH_PWR10) && (__GNUC__ > 9)
  int r;
  __asm__(
      "xvtlsbb 6,%x1;\n"
      "setbc   %0,26;\n"
      : "=r" (r)
      : "wa" (vra)
      : "cr6"
	);
  return r;
#else
  const vui8_t ones = vec_splat_u8(1);
  vui8_t lsbb;

  lsbb = vec_and (vra, ones);
#if 1
  return vec_all_ne (lsbb, ones);
#else
  const vui8_t zeros = vec_splat_u8(0);
  return vec_all_eq (lsbb, zeros);
#endif
#endif
}

vb8_t
test_vec_vcmpneb (vui8_t vra, vui8_t vrb)
{
  return vec_vcmpneb (vra, vrb);
}

vb8_t
test_vcmpneb_v0 (vui8_t vra, vui8_t vrb)
{
  vb8_t result;
#ifdef _ARCH_PWR9
#ifdef vec_cmpne
  result = vec_cmpne (vra, vrb);
#else
  __asm__(
      "vcmpneb %0,%1,%2;"
      : "=v" (result)
      : "v" (vra), "v" (vrb)
      : );
#endif
#else
  vb8_t abne;
  abne = vec_cmpeq (vra, vrb);
  result = vec_nor (abne, abne);
#endif
  return result;
}

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
  // Convert Gather Bits by Bytes by Doubleword
  // to Gather Bits by Halfwords by Quadword
  lsbb = (vui16_t) vec_perm (gbb, zeros, pgbb);
  clzls = vec_cntlz (lsbb);
  dwres = ((vui64_t) clzls) [VEC_DW_H];
  result = (unsigned short) dwres;
#else //  _ARCH_PWR7 and earlier
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
  // _ARCH_PWR8 supports vpopcnth
#if defined (vec_vpopcntb)
  ctzls = vec_vpopcnth (tzmask);
#else // clang
  ctzls = vec_popcnt (tzmask);
#endif
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
      // _ARCH_PWR8 supports vpopcnth
#if defined (vec_vpopcntb)
      ctzls = vec_vpopcnth (tzmask);
#else // clang
      ctzls = vec_popcnt (tzmask);
#endif
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
  // vcmpneb requires _ARCH_PWR9, so use cmpeq and orc
  eosa = vec_cmpeq (vra, VEOS);
  eosb = vec_cmpeq (vrb, VEOS);
  abne = vec_cmpeq (vra, vrb);
  eosc = vec_or (eosa, eosb);
#ifdef _ARCH_PWR8
  result = vec_orc (eosc, abne);
#else // vorc requires _ARCH_PWR8, so use cmpeq, nor and or
  abne = vec_nor (abne, abne);
  result = vec_or (eosc, abne);
#endif
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
