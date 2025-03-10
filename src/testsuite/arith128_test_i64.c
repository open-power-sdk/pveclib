/*
 Copyright (c) [2017, 2018] IBM Corporation.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 arith128_test_i64.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: Apr 5, 2018
 */

#include <stdint.h>
#include <stdio.h>

//#define __DEBUG_PRINT__
#include <testsuite/arith128_print.h>
#include <pveclib/vec_common_ppc.h>
#include <pveclib/vec_int32_ppc.h>
#include <pveclib/vec_int64_ppc.h>

#include <testsuite/arith128_test_i64.h>

//#define __DEBUG_PRINT__ 1

#ifdef __DEBUG_PRINT__
vui64_t
db_vec_vsld (vui64_t vra, vui64_t vrb)
{
  vui64_t result;

#ifdef _ARCH_PWR8
  __asm__(
      "vsld %0,%1,%2;"
      : "=v" (result)
      : "v" (vra),
      "v" (vrb)
      : );
#else
  vui8_t  vsh_h, vsh_l;
  vui8_t  vr_h, vr_l;
  vui64_t sel_mask = CONST_VINT128_DW (0, -1LL);
  vui64_t shft_mask = CONST_VINT128_DW (63, 63);
  print_v2xint64 ("db_vec_vsld (", vra);
  print_v2xint64 ("             ", vrb);

  /* constrain the dword shift amounts to 0-63.  */
  vsh_l = vec_and ((vui8_t)vrb, (vui8_t)shft_mask);
  /* Isolate the low dword so that bits from the high dword,
   * do not contaminate the result.  */
  vr_h = vec_andc ((vui8_t)vra, (vui8_t)sel_mask);
  vr_l  = vec_and ((vui8_t)vra, (vui8_t)sel_mask);
  /* The vsr instruction only works correctly if the bit shift
   * value is splatted to each byte of the vector.  */
  vsh_h = vec_splat (vsh_l, VEC_BYTE_L_DWH);
  vsh_l = vec_splat (vsh_l, VEC_BYTE_L_DWL);
  print_v2xint64 ("      vsh_h (", (vui64_t)vsh_h);
  print_v2xint64 ("      vsh_l (", (vui64_t)vsh_l);
//  vsht_splat = vec_splat ((vui8_t) vrb, VEC_BYTE_L);
  /* Shift the high dword by vsh_h.  */
  vr_h = vec_vslo (vr_h,  vsh_h);
  vr_h = vec_vsl  (vr_h, vsh_h);
  print_v2xint64 ("       vr_h (", (vui64_t)vr_h);
  /* Shift the low dword by vsh_l.  */
  vr_l = vec_vslo (vr_l,  vsh_l);
  vr_l = vec_vsl  (vr_l, vsh_l);
  print_v2xint64 ("       vr_l (", (vui64_t)vr_l);
  /* Merge the dwords after shift.  */
  result = (vui64_t)vec_sel (vr_h, vr_l, (vui8_t)sel_mask);
#endif
  return ((vui64_t) result);
}

vui64_t
db_vec_vsrd (vui64_t vra, vui64_t vrb)
{
  vui64_t result;

#ifdef _ARCH_PWR8
  __asm__(
      "vsrd %0,%1,%2;"
      : "=v" (result)
      : "v" (vra),
      "v" (vrb)
      : );
#else
  vui8_t  vsh_h, vsh_l;
  vui8_t  vr_h, vr_l;
  vui64_t sel_mask = CONST_VINT128_DW (0, -1LL);
  vui64_t shft_mask = CONST_VINT128_DW (63, 63);
  print_v2xint64 ("db_vec_vsrd (", vra);
  print_v2xint64 ("             ", vrb);

  /* constrain the dword shift amounts to 0-63.  */
  vsh_l = vec_and ((vui8_t)vrb, (vui8_t)shft_mask);
  /* Isolate the low dword so that bits from the high dword,
   * do not contaminate the result.  */
  vr_l  = vec_and ((vui8_t)vra, (vui8_t)sel_mask);
  /* The vsr instruction only works correctly if the bit shift
   * value is splatted to each byte of the vector.  */
  vsh_h = vec_splat (vsh_l, VEC_BYTE_L_DWH);
  vsh_l = vec_splat (vsh_l, VEC_BYTE_L_DWL);
  print_v2xint64 ("      vsh_h (", (vui64_t)vsh_h);
  print_v2xint64 ("      vsh_l (", (vui64_t)vsh_l);
//  vsht_splat = vec_splat ((vui8_t) vrb, VEC_BYTE_L);
  /* Shift the high dword by vsh_h.  */
  vr_h = vec_vsro ((vui8_t)vra,  vsh_h);
  vr_h = vec_vsr  (vr_h, vsh_h);
  print_v2xint64 ("       vr_h (", (vui64_t)vr_h);
  /* Shift the low dword by vsh_l.  */
  vr_l = vec_vsro (vr_l,  vsh_l);
  vr_l = vec_vsr  (vr_l, vsh_l);
  print_v2xint64 ("       vr_l (", (vui64_t)vr_l);
  /* Merge the dwords after shift.  */
  result = (vui64_t)vec_sel (vr_h, vr_l, (vui8_t)sel_mask);
#endif
  return ((vui64_t) result);
}

vi64_t
db_vec_vsrad (vi64_t vra, vui64_t vrb)
{
  vi64_t result;

#ifdef _ARCH_PWR8
  __asm__(
      "vsrad %0,%1,%2;"
      : "=v" (result)
      : "v" (vra),
      "v" (brb)
      : );
#else
  vui8_t  vsh_h, vsh_l;
  vui8_t  vr_h, vr_l;
  vi32_t exsa;
  vui32_t shw31 = CONST_VINT128_W (31, 31, 31, 31);
  vui64_t exsah, exsal;
//  vui64_t sel_mask = CONST_VINT128_DW (0, -1LL);
  vui64_t shft_mask = CONST_VINT128_DW (63, 63);
  print_v2xint64 ("db_vec_vsrad (", (vui64_t)vra);
  print_v2xint64 ("              ", vrb);

  /* Need to extend each signed long int to __int128. So the unsigned
   * (128-bit) shift right behave as a arithmetic (64-bit) shift.  */
  exsa = vec_vsraw ((vi32_t)vra, shw31);
  exsah = (vui64_t)vec_vmrghw (exsa, exsa);
  exsal = (vui64_t)vec_vmrglw (exsa, exsa);
  print_v2xint64 ("      exsah (", (vui64_t)exsah);
  print_v2xint64 ("      exsal (", (vui64_t)exsal);
  /* constrain the dword shift amounts to 0-63.  */
  vsh_l = vec_and ((vui8_t)vrb, (vui8_t)shft_mask);
  /* Isolate the low dword so that bits from the high dword,
   * do not contaminate the result.  */
//  vr_l  = vec_and ((vui8_t)vra, (vui8_t)sel_mask);
  /* The vsr instruction only works correctly if the bit shift
   * value is splatted to each byte of the vector.  */
  vsh_h = vec_splat (vsh_l, VEC_BYTE_L_DWH);
  vsh_l = vec_splat (vsh_l, VEC_BYTE_L_DWL);
  print_v2xint64 ("      vsh_h (", (vui64_t)vsh_h);
  print_v2xint64 ("      vsh_l (", (vui64_t)vsh_l);
//  vsht_splat = vec_splat ((vui8_t) vrb, VEC_BYTE_L);
  /* Merge the extended sign with high dword.  */
  exsah = vec_mrghd (exsah, (vui64_t)vra);
  print_v2xint64 ("exsd-vra[0] (", (vui64_t)exsah);
  /* Shift the high dword by vsh_h.  */
  vr_h = vec_vsro ((vui8_t)exsah,  vsh_h);
  vr_h = vec_vsr  (vr_h, vsh_h);
  print_v2xint64 ("       vr_h (", (vui64_t)vr_h);
  /* Merge the extended sign with high dword.  */
  exsal = vec_pasted (exsal, (vui64_t)vra);
  print_v2xint64 ("exsd-vra[1] (", (vui64_t)exsal);
  /* Shift the low dword by vsh_l.  */
  vr_l = vec_vsro ((vui8_t)exsal,  vsh_l);
  vr_l = vec_vsr  (vr_l, vsh_l);
  print_v2xint64 ("       vr_l (", (vui64_t)vr_l);
  /* Merge the dwords after shift.  */
//  result = (vi64_t)vec_sel (vr_h, vr_l, (vui8_t)sel_mask);
  result = (vi64_t)vec_mrgld ((vui64_t)vr_h, (vui64_t)vr_l);
#endif
  return ((vi64_t) result);
}

vui64_t db_vec_modud (vui64_t y, vui64_t z)
{
#if defined (_ARCH_PWR10)  && (__GNUC__ >= 10)
  vui64_t res;
#if (__GNUC__ >= 13)
  res = vec_mod (y, z);
#else
  __asm__(
      "vmodud %0,%1,%2;\n"
      : "=v" (res)
      : "v" (y), "v" (z)
      : );
#endif
  return res;
#elif defined (_ARCH_PWR9)
  __VEC_U_128 qu, yu, zu;
#if (__GNUC__ <= 10)
  yu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) y, 1);
  yu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) y, 0);
  zu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) z, 1);
  zu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) z, 0);
#else
  yu.vx2 = y;
  zu.vx2 = z;
#endif

  qu.ulong.lower = yu.ulong.lower % zu.ulong.lower;
  qu.ulong.upper = yu.ulong.upper % zu.ulong.upper;

  return qu.vx2;
#else
  int i;
  vb64_t ge;
  vui64_t c, xt;
  const vui64_t ones = vec_splat_u64(1);
  vui64_t x = vec_splat_u64(0);

  print_v2xint64 ("db_vec_modud (", y);
  print_v2xint64 ("              ", z);

  for (i = 1; i <= 64; i++)
    {
      /* Left shift (x || y) requires 129-bits, -> (t || x || y) */
      /* capture high bits of x and y into t and c. */
#if 1
#if 1
      c = vec_vrld (y, ones);
#else
      c = vec_rldi (y, 1);
#endif
#else
      c = vec_srdi (y, 63);
#endif
#if 0
      /* capture high bit of x as bool */
      t = (vui64_t) vec_sradi ((vi64_t)x, 63);
#endif
      y = vec_addudm (y, y); /* Shift left 1, x and y */
      x = vec_addudm (x, x);
      /* Propagate carry from y to x */
#if 1
      x = vec_selud (x, c, (vb64_t) ones);
#else
      x = vec_addudm (x, c);
#endif

      // deconstruct ((t || x) >= z)
      ge = vec_cmpgeud (x, z);
#if 0
      // Combine t with (x >= z) for 65-bit compare
      ge = (vb64_t) vec_or ((vui32_t)ge, (vui32_t)t);
      // Convert bool to carry-bit for conditional y+1
#if 0
      t  = vec_srdi ((vui64_t)ge, 63);
#endif
#endif

      /* if (x >= z) x = x - z ; y++ */
      xt = vec_subudm (x, z);
      /* if ((t || x) >= z) {x = xt; y++} */
      print_v2xint64 ("           ge ", (vui64_t) ge);
      print_v2xint64 ("           xt ", xt);
#if 1
      /* Instead of add, OR the boolean ge into bit_0 of y */
      y = vec_selud (y, (vui64_t) ge, (vb64_t) ones);
#else
      y = (vui64_t) vec_or ((vui32_t)y, (vui32_t)t);
#endif
      x = vec_selud (x, xt, ge);

      print_v2xint64 ("            x ", x);
      print_v2xint64 ("            y ", y);
    }
  return x;
#endif
}
#endif

vui64_t db_vec_divqud (vui128_t x_y, vui64_t z)
{
#if defined (_ARCH_PWR9)
  // POWER8/9 Do not have vector integer divide, but do have
  // Move To/From Vector-Scalar Register Instructions
  // So we can use the scalar hardware divide instructions
  __VEC_U_128 qu, xy, zu;
#if (__GNUC__ <= 10)
  xy.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) x_y, 1);
  xy.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) x_y, 0);
  zu.ulong.lower = __builtin_unpack_vector_int128 ((vi128_t) z, 1);
  zu.ulong.upper = __builtin_unpack_vector_int128 ((vi128_t) z, 0);
#else
  // Looks like AT16 handles this but what about 15/14 ...
  // AT10 does not.
  xy.vx1 = x_y;
  zu.vx2 = z;
#endif
  unsigned long long Dh = xy.ulong.upper;
  unsigned long long Dl = xy.ulong.lower;
  unsigned long long Dv = zu.ulong.upper;
  unsigned long long q1, q2, Q;
  unsigned long long r1, r2, R;

  // Transfer to GPUs and use scalar divide/divide extended
  // Based on the PowerISA, Programming Note for
  // Divide Word Extended [Unsigned]
  q1 = __builtin_divdeu (Dh, Dv);
  //r1 = -(q1 * Dv);
  r1 = (q1 * Dv);
  q2 = Dl / Dv;
  r2 = Dl - (q2 * Dv);
  Q = q1 + q2;
  //R = r1 + r2;
  R = r2 - r1;
  if ((R < r2) | (R >= Dv))
    {
      Q++;
      R = R - Dv;
    }

  // Transfer R|Q back to VRs and return
  qu.ulong.upper = R;
  qu.ulong.lower = Q;
  return qu.vx2;
#else
  /* Based on Hacker's Delight (2nd Edition) Figure 9-2.
   * "Divide long unsigned shift-and-subtract algorithm."
   * Converted to use vector unsigned __int128 and PVEClIB
   * operations.
   * As cmpgeuq is based on detecting the carry-out of (x -z) and
   * setting the bool via setb_cyq, we can use this carry (variable t)
   * to generate quotient bits.
   * Multi-precision shift-left is simpler then general addition,
   * so we can simplify carry generation. This allows delaying the
   * the y left-shift / quotient accumulation to a later.
   * */
  int i;
  vui64_t ge;
  //vui128_t cc, c;
  vui64_t t, xt, mone;
  const vui64_t zeros = vec_splat_u64(0);
  // t = (vui64_t) CONST_VINT128_DW (0, 0);
  mone = (vui64_t) CONST_VINT128_DW (-1, -1);
  /* Here only using the high DW of z, generated z as {z'', -1} */
  z = vec_pasted (z, mone);

  print_v2xint64 ("db_vec_divqud (", (vui64_t) x_y);
  print_v2xint64 ("           , z)", z);

  for (i = 1; i <= 64; i++)
    {
      // Left shift (x || y) requires 129-bits, is (t || x || y)
      /* capture high bit of x_y as bool t */
#if defined (_ARCH_PWR8)
      t = (vui64_t) vec_cmpltsd ((vi64_t) x_y, (vi64_t) zeros);
#else
      { // P7 and earlier did not support DW int.
	// But only need to convert the sign-bit into a bool
	vui32_t lts;
	lts = (vui32_t) vec_cmplt ((vi32_t) x_y, (vi32_t) zeros);
	t = (vui64_t) vec_splat (lts, VEC_W_H);
      }
#endif
      // Then shift left Quadword x_y by 1 bit;
      x_y = vec_slqi (x_y, 1);
#if (__DEBUG_PRINT__ > 1)
      print_v2xint64 ("             t ", t);
      print_v2xint64 ("      slqi x_y ", (vui64_t) x_y);
#endif
      /* We only need the high DW of t and ge */
      /* deconstruct ((t || x) >= z) to (t || (x >= z)) */
#if defined (_ARCH_PWR8)
      // vec_cmpge (x_y,z) is NOT vec_cmpgt (z, x_y)
      ge = (vui64_t) vec_cmpgtud (z, (vui64_t)x_y);
      /* Combine t with (x >= z) for 129-bit compare */
      ge = (vui64_t) vec_orc ((vui32_t)t, (vui32_t)ge);
#else
      ge = (vui64_t) vec_cmpgeud ((vui64_t)x_y, z);
      /* Combine t with (x >= z) for 129-bit compare */
      ge = (vui64_t) vec_or ((vui32_t)t, (vui32_t)ge);
#endif

#if (__DEBUG_PRINT__ > 1)
      print_v2xint64 ("            ge ", ge);
#endif
      /* Splat the high ge DW to both DWs for select */
      ge = vec_splatd (ge, VEC_DW_H);
#if (__DEBUG_PRINT__ > 1)
      print_v2xint64 ("           'ge ", ge);
#endif

      /* xt <- {(x - z), (y - ( -1)} */
      xt = vec_subudm ((vui64_t)x_y, z);

#if (__DEBUG_PRINT__ > 1)
      print_v2xint64 ("            xt ", xt);
#endif
      x_y = (vui128_t)vec_selud ((vui64_t)x_y, xt, (vb64_t)ge);
    }

  print_v2xint64 ("   return  x_y ", (vui64_t) x_y);
  return (vui64_t)x_y;
#endif
}

int
test_vrld (void)
{
  vui64_t i1, i2, e;
  vui64_t j;
  int rc = 0;

  printf ("\ntest_vrld Vector Rotate Left Doubleword\n");

  i1 = (vui64_t) CONST_VINT64_DW (1, 2);
  i2 = (vui64_t) CONST_VINT64_DW (2, 1);
  e = (vui64_t) CONST_VINT64_DW (4, 4);
  j = vec_vrld(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vrld   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vrld:", (vui128_t) j, (vui128_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x0800000000000010UL);
  i2 = (vui64_t) CONST_VINT64_DW (1, 2);
  e = (vui64_t) CONST_VINT64_DW (0x0000000000000003UL, 0x2000000000000040UL);
  j = vec_vrld(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vrld   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vrld:", (vui128_t) j, (vui128_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x0800000000000010UL);
  i2 = (vui64_t) CONST_VINT64_DW (2, 4);
  e = (vui64_t) CONST_VINT64_DW (0x0000000000000006UL, 0x8000000000000100UL);
  j = vec_vrld(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vrld   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vrld:", (vui128_t) j, (vui128_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x0800000000000010UL);
  i2 = (vui64_t) CONST_VINT64_DW (32, 32);
  e = (vui64_t) CONST_VINT64_DW (0x0000000180000000UL, 0x0000001008000000UL);
  j = vec_vrld(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vrld   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vrld:", (vui128_t) j, (vui128_t) e);

  return (rc);
}

#ifdef __DEBUG_PRINT__
#define test_vec_vsld(_i, _j)	db_vec_vsld(_i, _j)
#else
#define test_vec_vsld(_i, _j)	vec_vsld(_i, _j)
#endif
int
test_vsld (void)
{
  vui64_t i1, i2, e;
  vui64_t j;
  int rc = 0;

  printf ("\ntest_vsld Vector Shift Left Doubleword\n");

  i1 = (vui64_t) CONST_VINT64_DW (1, 2);
  i2 = (vui64_t) CONST_VINT64_DW (2, 1);
  e = (vui64_t) CONST_VINT64_DW (4, 4);
  j = test_vec_vsld(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsld   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vsld:", (vui128_t) j, (vui128_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vui64_t) CONST_VINT64_DW (33, 31);
  e = (vui64_t) CONST_VINT64_DW (0xfffffffe00000000, 0xffffffff80000000);
  j = test_vec_vsld(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsld   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vsld:", (vui128_t) j, (vui128_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vui64_t) CONST_VINT64_DW (63, 60);
  e = (vui64_t) CONST_VINT64_DW (0x8000000000000000, 0xf000000000000000);
  j = test_vec_vsld(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsld   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vsld:", (vui128_t) j, (vui128_t) e);

  return (rc);
}

#ifdef __DEBUG_PRINT__
#define test_vec_vsrd(_i, _j)	db_vec_vsrd(_i, _j)
#else
#define test_vec_vsrd(_i, _j)	vec_vsrd(_i, _j)
#endif
int
test_vsrd (void)
{
  vui64_t i1, i2, e;
  vui64_t j;
  int rc = 0;

  printf ("\ntest_vsrd Vector Shift Right Doubleword\n");

  i1 = (vui64_t) CONST_VINT64_DW (4, 2);
  i2 = (vui64_t) CONST_VINT64_DW (2, 1);
  e = (vui64_t) CONST_VINT64_DW (1, 1);
  j = test_vec_vsrd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrd   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vsrd:", (vui128_t) j, (vui128_t) e);
#if 1
  i1 = (vui64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vui64_t) CONST_VINT64_DW (33, 31);
  e = (vui64_t) CONST_VINT64_DW (0x000000007fffffff, 0x00000001ffffffff);
  j = test_vec_vsrd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrd   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vsrd:", (vui128_t) j, (vui128_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vui64_t) CONST_VINT64_DW (63, 60);
  e = (vui64_t) CONST_VINT64_DW (0x1, 0xf);
  j = test_vec_vsrd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrd   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vsrd:", (vui128_t) j, (vui128_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (100000000000, 100000000000);
  i2 = (vui64_t) CONST_VINT64_DW (33, 31);
  e = (vui64_t) CONST_VINT64_DW (11, 46);
  j = test_vec_vsrd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrd   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vsrd:", (vui128_t) j, (vui128_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (100000000000, 100000000000);
  i2 = (vui64_t) CONST_VINT64_DW (48, 40);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = test_vec_vsrd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrd   ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_vsrd:", (vui128_t) j, (vui128_t) e);
#endif
  return (rc);
}

#ifdef __DEBUG_PRINT__
#define test_vec_vsrad(_i, _j)	db_vec_vsrad(_i, _j)
#else
#define test_vec_vsrad(_i, _j)	vec_vsrad(_i, _j)
#endif
int
test_vsrad (void)
{
  vi64_t i1, e;
  vui64_t i2;
  vi64_t j;
  int rc = 0;

  printf ("\ntest_vsrd Vector Shift Right Arithmetic Doubleword\n");

  i1 = (vi64_t) CONST_VINT64_DW (4, 2);
  i2 = (vui64_t) CONST_VINT64_DW (2, 1);
  e = (vi64_t) CONST_VINT64_DW (1, 1);
  j = test_vec_vsrad(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrad  ( ", (vui64_t)i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_vsrad:", (vui128_t) j, (vui128_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-4, -2);
  i2 = (vui64_t) CONST_VINT64_DW (2, 1);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = test_vec_vsrad(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrad  ( ", (vui64_t)i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_vsrad:", (vui128_t) j, (vui128_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000, 0x7fffffffffffffff);
  i2 = (vui64_t) CONST_VINT64_DW (63, 63);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = test_vec_vsrad(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrad  ( ", (vui64_t)i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_vsrad:", (vui128_t) j, (vui128_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0x7fffffffffffffff, 0x8000000000000000);
  i2 = (vui64_t) CONST_VINT64_DW (63, 63);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = test_vec_vsrad(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrad  ( ", (vui64_t)i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_vsrad:", (vui128_t) j, (vui128_t) e);

  return (rc);
}
#undef __DEBUG_PRINT__

//#define __DEBUG_PRINT__ 1
int
test_sradi (void)
{
  vi64_t i, e;
  vi64_t j;
  int rc = 0;

  printf ("\ntest_sradi Vector Shift Right Arithmetic Doubleword Immediate\n");

  i = (vi64_t) CONST_VINT64_DW (0x0123456789abcdef, 0xfedcba9876543210);
  e = (vi64_t) CONST_VINT64_DW (0x0123456789abcdef, 0xfedcba9876543210);
  j = vec_sradi(i, 0);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsradi (  ", (vui64_t)i);
  printf         ("        ,   0");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_sradi:", (vui128_t) j, (vui128_t) e);

  e = (vi64_t) CONST_VINT64_DW (0x00123456789abcde, 0xffedcba987654321);
  j = vec_sradi(i, 4);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsradi (  ", (vui64_t)i);
  printf         ("        ,   4");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_sradi:", (vui128_t) j, (vui128_t) e);

  e = (vi64_t) CONST_VINT64_DW (0x000123456789abcd, 0xfffedcba98765432);
  j = vec_sradi(i, 8);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsradi (  ", (vui64_t)i);
  printf         ("        ,   8");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_sradi:", (vui128_t) j, (vui128_t) e);

  e = (vi64_t) CONST_VINT64_DW (0x000002468acf1357, 0xfffffdb97530eca8);
  j = vec_sradi(i, 15);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsradi (  ", (vui64_t)i);
  printf         ("        ,  15");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_sradi:", (vui128_t) j, (vui128_t) e);

  e = (vi64_t) CONST_VINT64_DW (0x00000123456789ab, 0xfffffedcba987654);
  j = vec_sradi(i, 16);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsradi (  ", (vui64_t)i);
  printf         ("        ,  16");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_sradi:", (vui128_t) j, (vui128_t) e);

  e = (vi64_t) CONST_VINT64_DW (0x0000000002468acf, 0xfffffffffdb97530);
  j = vec_sradi(i, 31);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsradi (  ", (vui64_t)i);
  printf         ("        ,  31");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_sradi:", (vui128_t) j, (vui128_t) e);

  e = (vi64_t) CONST_VINT64_DW (0x0000000001234567, 0xfffffffffedcba98);
  j = vec_sradi(i, 32);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsradi (  ", (vui64_t)i);
  printf         ("        ,  32");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_sradi:", (vui128_t) j, (vui128_t) e);

  e = (vi64_t) CONST_VINT64_DW (0x0000000000000246, 0xfffffffffffffdb9);
  j = vec_sradi(i, 47);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsradi (  ", (vui64_t)i);
  printf         ("        ,  47");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_sradi:", (vui128_t) j, (vui128_t) e);

  e = (vi64_t) CONST_VINT64_DW (0x0000000000000123, 0xfffffffffffffedc);
  j = vec_sradi(i, 48);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsradi (  ", (vui64_t)i);
  printf         ("        ,  48");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_sradi:", (vui128_t) j, (vui128_t) e);

  e = (vi64_t) CONST_VINT64_DW (0x0000000000000000, 0xffffffffffffffff);
  j = vec_sradi(i, 63);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsradi (  ", (vui64_t)i);
  printf         ("        ,  63");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_sradi:", (vui128_t) j, (vui128_t) e);

  e = (vi64_t) CONST_VINT64_DW (0x0000000000000000, 0xffffffffffffffff);
  j = vec_sradi(i, 64);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsradi (  ", (vui64_t)i);
  printf         ("        ,  64");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_sradi:", (vui128_t) j, (vui128_t) e);

  return (rc);
}
#undef __DEBUG_PRINT__

//#define __DEBUG_PRINT__ 1
int
test_srdi (void)
{
  vui64_t i, e;
  vui64_t j;
  int rc = 0;

  printf ("\ntest_srdi Vector Shift Right Doubleword Immediate\n");

  i = (vui64_t) CONST_VINT64_DW (0x0123456789abcdef, 0xfedcba9876543210);
  e = (vui64_t) CONST_VINT64_DW (0x0123456789abcdef, 0xfedcba9876543210);
  j = vec_srdi(i, 0);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrdi (  ", (vui64_t)i);
  printf         ("        ,   0");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_srdi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0x00123456789abcde, 0x0fedcba987654321);
  j = vec_srdi(i, 4);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrdi (  ", (vui64_t)i);
  printf         ("        ,   4");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_srdi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0x000123456789abcd, 0x00fedcba98765432);
  j = vec_srdi(i, 8);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrdi (  ", (vui64_t)i);
  printf         ("        ,   8");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_srdi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0x000002468acf1357, 0x0001fdb97530eca8);
  j = vec_srdi(i, 15);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrdi (  ", (vui64_t)i);
  printf         ("        ,  15");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_srdi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0x00000123456789ab, 0x0000fedcba987654);
  j = vec_srdi(i, 16);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrdi (  ", (vui64_t)i);
  printf         ("        ,  16");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_srdi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0x0000000002468acf, 0x00000001fdb97530);
  j = vec_srdi(i, 31);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrdi (  ", (vui64_t)i);
  printf         ("        ,  31");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_srdi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0x0000000001234567, 0x00000000fedcba98);
  j = vec_srdi(i, 32);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrdi (  ", (vui64_t)i);
  printf         ("        ,  32");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_srdi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0x0000000000000246, 0x000000000001fdb9);
  j = vec_srdi(i, 47);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrdi (  ", (vui64_t)i);
  printf         ("        ,  47");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_srdi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0x0000000000000123, 0x000000000000fedc);
  j = vec_srdi(i, 48);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrdi (  ", (vui64_t)i);
  printf         ("        ,  48");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_srdi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0x0000000000000000, 0x0000000000000001);
  j = vec_srdi(i, 63);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrdi (  ", (vui64_t)i);
  printf         ("        ,  63");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_srdi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0x0000000000000000, 0x0000000000000000);
  j = vec_srdi(i, 64);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsrdi (  ", (vui64_t)i);
  printf         ("        ,  64");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_srdi:", (vui128_t) j, (vui128_t) e);

  return (rc);
}
#undef __DEBUG_PRINT__

//#define __DEBUG_PRINT__ 1
int
test_sldi (void)
{
  vui64_t i, e;
  vui64_t j;
  int rc = 0;

  printf ("\ntest_sldi Vector Shift Left Doubleword Immediate\n");

  i = (vui64_t) CONST_VINT64_DW (0x0123456789abcdef, 0xfedcba9876543210);
  e = (vui64_t) CONST_VINT64_DW (0x0123456789abcdef, 0xfedcba9876543210);
  j = vec_sldi(i, 0);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsldi (  ", (vui64_t)i);
  printf         ("        ,   0");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_sldi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0x123456789abcdef0, 0xedcba98765432100);
  j = vec_sldi(i, 4);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsldi (  ", (vui64_t)i);
  printf         ("        ,   4");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_sldi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0x23456789abcdef00, 0xdcba987654321000);
  j = vec_sldi(i, 8);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsldi (  ", (vui64_t)i);
  printf         ("        ,   8");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_sldi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0xa2b3c4d5e6f78000, 0x5d4c3b2a19080000);
  j = vec_sldi(i, 15);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsldi (  ", (vui64_t)i);
  printf         ("        ,  15");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_sldi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0x456789abcdef0000, 0xba98765432100000);
  j = vec_sldi(i, 16);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsldi (  ", (vui64_t)i);
  printf         ("        ,  16");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_sldi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0xc4d5e6f780000000, 0x3b2a190800000000);
  j = vec_sldi(i, 31);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsldi (  ", (vui64_t)i);
  printf         ("        ,  31");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_sldi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0x89abcdef00000000, 0x7654321000000000);
  j = vec_sldi(i, 32);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsldi (  ", (vui64_t)i);
  printf         ("        ,  32");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_sldi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0xe6f7800000000000, 0x1908000000000000);
  j = vec_sldi(i, 47);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsldi (  ", (vui64_t)i);
  printf         ("        ,  47");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_sldi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0xcdef000000000000, 0x3210000000000000);
  j = vec_sldi(i, 48);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsldi (  ", (vui64_t)i);
  printf         ("        ,  48");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_sldi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0x8000000000000000, 0x0000000000000000);
  j = vec_sldi(i, 63);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsldi (  ", (vui64_t)i);
  printf         ("        ,  63");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_sldi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0x0000000000000000, 0x0000000000000000);
  j = vec_sldi(i, 64);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vsldi (  ", (vui64_t)i);
  printf         ("        ,  64");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_sldi:", (vui128_t) j, (vui128_t) e);

  return (rc);
}
#undef __DEBUG_PRINT__

//#define __DEBUG_PRINT__ 1
int
test_rldi (void)
{
  vui64_t i, e;
  vui64_t j;
  int rc = 0;

  printf ("\ntest_rldi Vector Rotate Left Doubleword Immediate\n");

  i = (vui64_t) CONST_VINT64_DW (0x0123456789abcdef, 0xfedcba9876543210);
  e = (vui64_t) CONST_VINT64_DW (0x0123456789abcdef, 0xfedcba9876543210);
  j = vec_rldi(i, 0);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vrldi (  ", (vui64_t)i);
  printf         ("        ,   0");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_rldi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0x123456789abcdef0, 0xedcba9876543210f);
  j = vec_rldi(i, 4);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vrldi (  ", (vui64_t)i);
  printf         ("        ,   4");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_rldi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0x23456789abcdef01, 0xdcba9876543210fe);
  j = vec_rldi(i, 8);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vrldi (  ", (vui64_t)i);
  printf         ("        ,   8");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_rldi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0xa2b3c4d5e6f78091, 0x5d4c3b2a19087f6e);
  j = vec_rldi(i, 15);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vrldi (  ", (vui64_t)i);
  printf         ("        ,  15");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_rldi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0x456789abcdef0123, 0xba9876543210fedc);
  j = vec_rldi(i, 16);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vrldi (  ", (vui64_t)i);
  printf         ("        ,  16");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_rldi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0xc4d5e6f78091a2b3, 0x3b2a19087f6e5d4c);
  j = vec_rldi(i, 31);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vrldi (  ", (vui64_t)i);
  printf         ("        ,  31");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_rldi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0x89abcdef01234567, 0x76543210fedcba98);
  j = vec_rldi(i, 32);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vrldi (  ", (vui64_t)i);
  printf         ("        ,  32");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_rldi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0xe6f78091a2b3c4d5, 0x19087f6e5d4c3b2a);
  j = vec_rldi(i, 47);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vrldi (  ", (vui64_t)i);
  printf         ("        ,  47");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_rldi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0xcdef0123456789ab, 0x3210fedcba987654);
  j = vec_rldi(i, 48);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vrldi (  ", (vui64_t)i);
  printf         ("        ,  48");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_rldi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0x8091a2b3c4d5e6f7, 0x7f6e5d4c3b2a1908);
  j = vec_rldi(i, 63);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vrldi (  ", (vui64_t)i);
  printf         ("        ,  63");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_rldi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (0x0123456789abcdef, 0xfedcba9876543210);
  j = vec_rldi(i, 64);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vrldi (  ", (vui64_t)i);
  printf         ("        ,  64");
  print_v2xint64 ("       )=", (vui64_t)j);
#endif
  rc += check_vuint128x ("vec_rldi:", (vui128_t) j, (vui128_t) e);

  return (rc);
}
#undef __DEBUG_PRINT__

static unsigned long long test_ui64[] =
    {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
    };

int
test_lvgudx (void)
{
  vi64_t i1, i2;
  vui64_t e;
  vui64_t j, j0, j1;
  int rc = 0;

  printf ("\ntest_Vector Gather-Load Doubleword\n");

  e =  (vui64_t) CONST_VINT64_DW ( 0, -1 );
  j0 = vec_vlsidx (0, test_ui64);
  j = vec_permdi (j0, e, 1);

  rc += check_vuint128x ("vec_vlsidx 1:", (vui128_t) j, (vui128_t) e);

  e =  (vui64_t) CONST_VINT64_DW ( 1, -1 );
  j0 = vec_vlsidx (8, test_ui64);
  j = vec_permdi (j0, e, 1);

  rc += check_vuint128x ("vec_vlsidx 2:", (vui128_t) j, (vui128_t) e);

  e =  (vui64_t) CONST_VINT64_DW ( 15, -1 );
  j0 = vec_vlsidx (120, test_ui64);
  j = vec_permdi (j0, e, 1);

  rc += check_vuint128x ("vec_vlsidx 3:", (vui128_t) j, (vui128_t) e);

  i1 = (vi64_t) { 8, 120 };
  e =  (vui64_t) CONST_VINT64_DW ( 1, -1 );
  j0 = vec_vlsidx (i1[0], test_ui64);
  j = vec_permdi (j0, e, 1);

  rc += check_vuint128x ("vec_vlsidx 4:", (vui128_t) j, (vui128_t) e);

  i1 = (vi64_t) { 8, 120 };
  e =  (vui64_t) CONST_VINT64_DW ( 15, -1 );
  j1 = vec_vlsidx (i1[1], test_ui64);
  j = vec_permdi (j1, e, 1);

  rc += check_vuint128x ("vec_vlsidx 5:", (vui128_t) j, (vui128_t) e);

  // This test depends on the merge of scalars from lsudux 4/5
  e =  (vui64_t) { 1, 15 };
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  j = vec_permdi (j1, j0, 0);
#else
  j = vec_permdi (j0, j1, 0);
#endif

  rc += check_vuint128x ("vec_vlsidx2 :", (vui128_t) j, (vui128_t) e);

  // This test replecates the results of the last 3 tests in single op.
  e =  (vui64_t) { 1, 15 };
  j = vec_vgludso (test_ui64, 8, 120);

  rc += check_vuint128x ("vec_vgludso :", (vui128_t) j, (vui128_t) e);

  // This test replecates the results of the last 3 tests in single op.
  e =  (vui64_t) { 1, 15 };
  j = vec_vgluddo (test_ui64, i1);

  rc += check_vuint128x ("vec_vgluddo :", (vui128_t) j, (vui128_t) e);


  i2 = (vi64_t) { 1, 15 };
  i1 = (vi64_t) vec_sldi ((vui64_t) i2, 3);
  e =  (vui64_t) CONST_VINT64_DW ( 1, -1 );
  j0 = vec_vlsidx (i1[0], test_ui64);
  j = vec_permdi (j0, e, 1);

  rc += check_vuint128x ("vec_vlsidx 6:", (vui128_t) j, (vui128_t) e);

  e =  (vui64_t) CONST_VINT64_DW ( 15, -1 );
  j1 = vec_vlsidx (i1[1], test_ui64);
  j = vec_permdi (j1, e, 1);

  rc += check_vuint128x ("vec_vlsidx 7:", (vui128_t) j, (vui128_t) e);

  e =  (vui64_t) { 1, 15 };
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  j = vec_permdi (j1, j0, 0);
#else
  j = vec_permdi (j0, j1, 0);
#endif

  rc += check_vuint128x ("vec_vlsidx2 :", (vui128_t) j, (vui128_t) e);

  i1 = (vi64_t) { 1, 2 };
  e =  (vui64_t) { 1, 2 };
  j = vec_vgluddx (test_ui64, i1);

  rc += check_vuint128x ("vec_vgluddx :", (vui128_t) j, (vui128_t) e);

  i1 = (vi64_t) { 15, 7 };
  e =  (vui64_t) { 15, 7 };
  j = vec_vgluddx (test_ui64, i1);

  rc += check_vuint128x ("vec_vgluddx :", (vui128_t) j, (vui128_t) e);

  i1 = (vi64_t) { 1, 2 };
  e =  (vui64_t) { 2, 4 };
  j = vec_vgluddsx (test_ui64, i1, 1);

  rc += check_vuint128x ("vec_vgluddsx :", (vui128_t) j, (vui128_t) e);

  i1 = (vi64_t) { 3, 2 };
  e =  (vui64_t) { 12, 8 };
  j = vec_vgluddsx (test_ui64, i1, 2);

  rc += check_vuint128x ("vec_vgluddsx :", (vui128_t) j, (vui128_t) e);

  return (rc);
}

static unsigned long long test_stui64[16];

int
test_stvgudx (void)
{
  vi64_t i2;
  vui64_t e, *mem;
  vui64_t j, j1, j2;
  int rc = 0;
  int i;

  mem = (vui64_t *)& test_stui64;

  for (i=0; i<16; i++)
    test_stui64[i] = test_ui64[i];

  printf ("\ntest_Store Vector with scatter Doubleword\n");

  j1 = (vui64_t) CONST_VINT64_DW ( 16, 1616 );
  j2 = (vui64_t) CONST_VINT64_DW ( 31, 3131 );
  vec_vstsidx (j1, 0, test_stui64);
  vec_vstsidx (j2, 120, test_stui64);

  j = mem [0];
  e = (vui64_t) { 16, 1 };

  rc += check_vuint128x ("vec_stsudux 1:", (vui128_t) j, (vui128_t) e);

  j = mem [7];
  e = (vui64_t) { 14, 31 };

  rc += check_vuint128x ("vec_stsudux 2:", (vui128_t) j, (vui128_t) e);

  j1 = (vui64_t) { 17, 30 };
  i2 = (vi64_t) { 8, 112 };
  vec_vsstuddo (j1, test_stui64, i2);

  j = mem [0];
  e = (vui64_t) { 16, 17 };

  rc += check_vuint128x ("vec_stvsudo 1:", (vui128_t) j, (vui128_t) e);

  j = mem [7];
  e = (vui64_t) { 30, 31 };

  rc += check_vuint128x ("vec_stvsudo 2:", (vui128_t) j, (vui128_t) e);

  j1 = (vui64_t) { 18, 29 };
  i2 = (vi64_t) { 2, 13 };
  vec_vsstuddx (j1, test_stui64, i2);

  j = mem [1];
  e = (vui64_t) { 18, 3 };

  rc += check_vuint128x ("vec_stvsudx 1:", (vui128_t) j, (vui128_t) e);

  j = mem [6];
  e = (vui64_t) { 12, 29 };

  rc += check_vuint128x ("vec_stvsudx 2:", (vui128_t) j, (vui128_t) e);

  j1 = (vui64_t) { 20, 28 };
  i2 = (vi64_t) { 2, 6 };
  vec_vsstuddsx (j1, test_stui64, i2, 1);

  j = mem [2];
  e = (vui64_t) { 20, 5 };

  rc += check_vuint128x ("vec_stvsudsx 1:", (vui128_t) j, (vui128_t) e);

  j = mem [6];
  e = (vui64_t) { 28, 29 };

  rc += check_vuint128x ("vec_stvsudsx 2:", (vui128_t) j, (vui128_t) e);


  return (rc);
}

int
test_mrghld (void)
{
  vui64_t i1, i2, e;
  vui64_t j;
  int rc = 0;

  printf ("\ntest_merge Vector Merge High/Low Doubleword\n");

  i1 = (vui64_t) { 1, 2 };
  i2 = (vui64_t) { 101, 102 };

  e = (vui64_t) { 1, 101 };
  j = vec_mrghd (i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("mrghd  ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_mrghd :", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) { 2, 102 };
  j = vec_mrgld (i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("mrgld  ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_mrgld :", (vui128_t) j, (vui128_t) e);

  return (rc);
}

int
test_mrgeod (void)
{
  vui64_t i1, i2, e;
  vui64_t j;
  int rc = 0;

  printf ("\ntest_merge Vector Merge Even/Odd Doubleword\n");

  i1 = (vui64_t) { 1, 2 };
  i2 = (vui64_t) { 101, 102 };

  e = (vui64_t) { 1, 101 };
  j = vec_mrged (i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("mrged  ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_mrged :", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) { 2, 102 };
  j = vec_mrgod (i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("mrgod  ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_mrgod :", (vui128_t) j, (vui128_t) e);

  return (rc);
}

int
test_mrgahld (void)
{
  vui64_t i1, i2, e;
  vui64_t j;
  int rc = 0;

  printf ("\ntest_mrgahld Vector Merge Algebraic High/Low Doubleword\n");

  i1 = (vui64_t) CONST_VINT64_DW (1, 2);
  i2 = (vui64_t) CONST_VINT64_DW (101, 102);

  e = (vui64_t) CONST_VINT64_DW (1, 101);
  j = vec_mrgahd ((vui128_t)i1, (vui128_t)i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("mrgahd  ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_mrgahd :", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (2, 102);
  j = vec_mrgald ((vui128_t)i1, (vui128_t)i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("mrgald  ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_mrgald :", (vui128_t) j, (vui128_t) e);

  return (rc);
}

int
test_splatd (void)
{
  vui64_t i1, e;
  vui64_t j;
  int rc = 0;

  printf ("\ntest_splatd Vector Splat Doubleword Immediate\n");

  i1 = (vui64_t) { 1, 2 };

  e = (vui64_t) { 1, 1 };
  j = vec_splatd(i1, 0);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("splatd  ( ", i1);
  print_v2xint64 ("     ,0)=", j);
#endif
  rc += check_vuint128x ("vec_splatd (x,0):", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) { 2, 2 };
  j = vec_splatd(i1, 1);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("splatd  ( ", i1);
  print_v2xint64 ("     ,1)=", j);
#endif
  rc += check_vuint128x ("vec_splatd (x,1):", (vui128_t) j, (vui128_t) e);

  return (rc);
}

int
test_xxspltd (void)
{
  vui64_t i1, e;
  vui64_t j;
  int rc = 0;

  printf ("\ntest_xxspltd Vector Splat Doubleword Immediate\n");

  i1 = (vui64_t) CONST_VINT64_DW (1, 2);

  e = (vui64_t) CONST_VINT64_DW (1, 1);
  j = vec_xxspltd(i1, 0);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("spltd  ( ", i1);
  print_v2xint64 ("     ,0)=", j);
#endif
  rc += check_vuint128x ("vec_xxspltd (x,0):", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (2, 2);
  j = vec_xxspltd(i1, 1);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("spltd  ( ", i1);
  print_v2xint64 ("     ,1)=", j);
#endif
  rc += check_vuint128x ("vec_xxspltd (x,1):", (vui128_t) j, (vui128_t) e);

  return (rc);
}

int
test_permdi (void)
{
  vui64_t i1, i2, e;
  vui64_t j;
  int rc = 0;

  printf ("\ntest_permdi Vector Permute Doubleword Immediate\n");

  i1 = (vui64_t) CONST_VINT64_DW (1, 2);
  i2 = (vui64_t) CONST_VINT64_DW (101, 102);
  e = (vui64_t) CONST_VINT64_DW (1, 101);
  j = vec_permdi(i1, i2, 0);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("permdi ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("     ,0)=", j);
#endif
  rc += check_vuint128x ("vec_permdi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (1, 102);
  j = vec_permdi(i1, i2, 1);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("permdi ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("     ,1)=", j);
#endif
  rc += check_vuint128x ("vec_permdi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (2, 101);
  j = vec_permdi(i1, i2, 2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("permdi ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("     ,2)=", j);
#endif
  rc += check_vuint128x ("vec_permdi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (2, 102);
  j = vec_permdi(i1, i2, 3);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("permdi ( ", i1);
  print_v2xint64 ("        ,", i2);
  print_v2xint64 ("     ,3)=", j);
#endif
  rc += check_vuint128x ("vec_permdi:", (vui128_t) j, (vui128_t) e);

  e = (vui64_t) CONST_VINT64_DW (2, 1);
  j = vec_swapd(i1);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("swapd  ( ", i1);
  print_v2xint64 ("       )=", j);
#endif
  rc += check_vuint128x ("vec_swapd (x):", (vui128_t) j, (vui128_t) e);

  return (rc);
}

int
test_revbd (void)
{
  vui32_t i, e, *ip;
  vui128_t k;
  unsigned char mem[16] __attribute__ ((aligned (16))) = { 0xf0, 0xf1, 0xf2,
      0xf3, 0xe0, 0xe1, 0xe2, 0xe3, 0xd0, 0xd1, 0xd2, 0xd3, 0xc0, 0xc1, 0xc2,
      0xc3 };
  int rc = 0;

  printf ("\ntest_revbd Reverse Bytes in Doublewords\n");

  i = (vui32_t ) { 0, 1, 2, 3 };
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  e = (vui32_t )CONST_VINT32_W(0x01000000, 0x00000000, 0x03000000, 0x02000000);
#else
  e = (vui32_t)CONST_VINT32_W(0x02000000, 0x03000000, 0x00000000, 0x01000000);
#endif
  k = (vui128_t) vec_revbd ((vui64_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_revbd i=", (vui128_t)i);
  print_vint128x ("         k=", k);
#endif
  rc += check_vuint128x ("vec_revbd 1:", k, (vui128_t) e);

  i = (vui32_t )CONST_VINT32_W(0x01020304, 0x11121314, 0x21222324, 0x31323334);
  e = (vui32_t )CONST_VINT32_W(0x14131211, 0x04030201, 0x34333231, 0x24232221);
  k = (vui128_t) vec_revbd ((vui64_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_revbd i=", (vui128_t)i);
  print_vint128x ("         k=", k);
#endif
  rc += check_vuint128x ("vec_revbd 2:", k, (vui128_t) e);

  ip = (vui32_t*) mem;
  i = *ip;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  e = (vui32_t )CONST_VINT32_W(0xe3e2e1e0, 0xf3f2f1f0, 0xc3c2c1c0, 0xd3d2d1d0);
#else
  e = (vui32_t)CONST_VINT32_W(0xd0d1d2d3, 0xc0c1c2c3, 0xf0f1f2f3, 0xe0e1e2e3);
#endif
  k = (vui128_t) vec_revbd ((vui64_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_revbd i=", (vui128_t)i);
  print_vint128x ("         k=", k);
#endif
  rc += check_vuint128x ("vec_revbd 3:", k, (vui128_t) e);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
#if 1
#if 0
// test directly from vec_int64_ppc.h
#define test_popcnt_d(_l)	vec_popcntd(_l)
#else
// test from vec_int64_ppc.h via vec_int64_dummy.c
extern vui64_t test_vec_popcntd (vui64_t);
#define test_popcnt_d(_l)	test_vec_popcntd(_l)
#endif
#else
// test from vec_int64_dummy.c
extern vui64_t test_vec_popcntd_PWR7 (vui64_t);
#define test_popcnt_d(_j)	test_vec_popcntd_PWR7(_j)
#endif

int
test_popcntd (void)
{
  vui64_t i, e;
  vui64_t j;
  int rc = 0;

  printf ("\ntest_popcntd Vector Pop Count Doubleword\n");

  i = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = test_popcnt_d(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({0, 0) ", (vui128_t) j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (0, -1);
  e = (vui64_t) CONST_VINT64_DW (0, 64);
  j = test_popcnt_d(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({0, -1) ", (vui128_t) j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (-1, 0);
  e = (vui64_t) CONST_VINT64_DW (64, 0);
  j = test_popcnt_d(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({-1, 0) ", (vui128_t) j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (-1, -1);
  e = (vui64_t) CONST_VINT64_DW (64, 64);
  j = test_popcnt_d(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({-1, -1) ", (vui128_t) j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (1, 8589934596);
  e = (vui64_t) CONST_VINT64_DW (1, 2);
  j = test_popcnt_d(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({1, 8589934596) ", (vui128_t) j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (34359738384, 137438953536);
  e = (vui64_t) CONST_VINT64_DW (2, 2);
  j = test_popcnt_d(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({34359738384, 137438953536) ", (vui128_t) j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (549755814144, 2199023256576);
  e = (vui64_t) CONST_VINT64_DW (2, 2);
  j = test_popcnt_d(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({549755814144, 2199023256576) ", (vui128_t) j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (8796093026304, 35184372105216);
  e = (vui64_t) CONST_VINT64_DW (2, 2);
  j = test_popcnt_d(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({8796093026304, 35184372105216) ", (vui128_t) j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (140737488420864, 562949953683456);
  e = (vui64_t) CONST_VINT64_DW (2, 2);
  j = test_popcnt_d(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({140737488420864, 562949953683456) ", (vui128_t) j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (2251799814733824, 9007199258935296);
  e = (vui64_t) CONST_VINT64_DW (2, 2);
  j = test_popcnt_d(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({2251799814733824, 9007199258935296) ", (vui128_t) j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (36028797035741184, 144115188142964736);
  e = (vui64_t) CONST_VINT64_DW (2, 2);
  j = test_popcnt_d(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({36028797035741184, 144115188142964736) ", (vui128_t) j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (576460752571858944, 2305843010287435776);
  e = (vui64_t) CONST_VINT64_DW (2, 2);
  j = test_popcnt_d(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({576460752571858944, 2305843010287435776) ", (vui128_t) j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (429496725405032703, 429496725405032703);
  e = (vui64_t) CONST_VINT64_DW (38, 38);
  j = test_popcnt_d(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({429496725405032703, 429496725405032703) ", (vui128_t) j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (381774867026695736, 381774867026695736);
  e = (vui64_t) CONST_VINT64_DW (24, 24);
  j = test_popcnt_d(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({381774867026695736, 381774867026695736) ", (vui128_t) j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (1000000000, 1000000000);
  e = (vui64_t) CONST_VINT64_DW (13, 13);
  j = test_popcnt_d(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({1000000000, 1000000000) ", (vui128_t) j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (10000000000, 1000000000);
  e = (vui64_t) CONST_VINT64_DW (11, 13);
  j = test_popcnt_d(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({10000000000, 1000000000) ", (vui128_t) j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (1000000000, 10000000000);
  e = (vui64_t) CONST_VINT64_DW (13, 11);
  j = test_popcnt_d(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({1000000000, 10000000000) ", (vui128_t) j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (1000000000000000000, 0);
  e = (vui64_t) CONST_VINT64_DW (24, 0);
  j = test_popcnt_d(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({1000000000000000000, 0) ", (vui128_t) j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (0, 1000000000000000000);
  e = (vui64_t) CONST_VINT64_DW (0, 24);
  j = test_popcnt_d(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({0, 1000000000000000000) ", (vui128_t) j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (0, 1000000000000000000);
  e = (vui64_t) CONST_VINT64_DW (0, 24);
  j = test_popcnt_d(i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("popcntd({0, 1000000000000000000) ", (vui128_t) j);
#endif
  rc += check_vuint128x ("vec_popcntd:", (vui128_t) j, (vui128_t) e);

  return (rc);
}

//#undef __DEBUG_PRINT__

// #define __DEBUG_PRINT__ 1
#if 1
#if 0
// test directly from vec_int64_ppc.h
#define test_clz_d(_l)	vec_clzd(_l)
#else
// test from vec_int64_ppc.h via vec_i64_dummy.c
extern vui64_t test_vec_clzd (vui64_t);
#define test_clz_d(_l)	test_vec_clzd(_l)
#endif
#else
// test from vec_i64_dummy.c
#if 1
extern vui64_t test_vec_clzd_PWR7 (vui64_t);
#define test_clz_d(_j)	test_vec_clzd_PWR7(_j)
#else
extern vui64_t test_clzd_PWR7 (vui64_t);
#define test_clz_d(_j)	test_clzd_PWR7(_j)
#endif
#endif

int
test_clzd (void)
{
  vui64_t i, e;
  vui64_t j;
  int rc = 0;

  printf ("\ntest_clzd Vector Count Leading Zeros in Doublewords\n");

  i = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (64, 64);
  j = test_clz_d((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("clz(0, 0) ", j);
#endif
  rc += check_vuint128x ("vec_clzd 1:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (-1, -1);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = test_clz_d((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("clz(-1, -1) ", j);
#endif
  rc += check_vuint128x ("vec_clzd 2:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (0, -1);
  e = (vui64_t) CONST_VINT64_DW (64, 0);
  j = test_clz_d((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("clz(0, -1) ", j);
#endif
  rc += check_vuint128x ("vec_clzd 3:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (-1, 0);
  e = (vui64_t) CONST_VINT64_DW (0, 64);
  j = test_clz_d((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("clz(-1, 0) ", j);
#endif
  rc += check_vuint128x ("vec_clzd 4:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (1, 8589934596);
  e = (vui64_t) CONST_VINT64_DW (63, 30);
  j = test_clz_d((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("clz(1, 8589934596) ", j);
#endif
  rc += check_vuint128x ("vec_clzd 5:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (34359738384, 137438953536);
  e = (vui64_t) CONST_VINT64_DW (28, 26);
  j = test_clz_d((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("clz(34359738384, 137438953536) ", j);
#endif
  rc += check_vuint128x ("vec_clzd 6:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (549755814144, 2199023256576);
  e = (vui64_t) CONST_VINT64_DW (24, 22);
  j = test_clz_d((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("clz(549755814144, 2199023256576) ", j);
#endif
  rc += check_vuint128x ("vec_clzd 7:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (8796093026304, 35184372105216);
  e = (vui64_t) CONST_VINT64_DW (20, 18);
  j = test_clz_d((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("clz(8796093026304, 35184372105216) ", j);
#endif
  rc += check_vuint128x ("vec_clzd 8:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (140737488420864, 562949953683456);
  e = (vui64_t) CONST_VINT64_DW (16, 14);
  j = test_clz_d((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("clz(140737488420864, 562949953683456) ", j);
#endif
  rc += check_vuint128x ("vec_clzd 9:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (2251799814733824, 9007199258935296);
  e = (vui64_t) CONST_VINT64_DW (12, 10);
  j = test_clz_d((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("clz(2251799814733824, 9007199258935296) ", j);
#endif
  rc += check_vuint128x ("vec_clzd 10:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (36028797035741184, 144115188142964736);
  e = (vui64_t) CONST_VINT64_DW (8, 6);
  j = test_clz_d((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("clz(36028797035741184, 144115188142964736) ", j);
#endif
  rc += check_vuint128x ("vec_clzd 11:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (576460752571858944, 2305843010287435776);
  e = (vui64_t) CONST_VINT64_DW (4, 2);
  j = test_clz_d((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("clz(576460752571858944, 2305843010287435776) ", j);
#endif
  rc += check_vuint128x ("vec_clzd 12:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (0x0800000000000000ULL, 0x2000000000000000ULL);
  e = (vui64_t) CONST_VINT64_DW (4, 2);
  j = test_clz_d((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("clz(0x0800000000000000ULL, 0x2000000000000000ULL) ", j);
#endif
  rc += check_vuint128x ("vec_clzd 13:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (0x000004ee2d6d415bULL, 0x85acef8100000000ULL);
  e = (vui64_t) CONST_VINT64_DW (21, 0);
  j = test_clz_d((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("clz(0x000004ee2d6d415bULL, 0x85acef8100000000ULL) ", j);
#endif

  rc += check_vuint128x ("vec_clzd 14:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (0x4b3b4ca85a86c47aULL, 0x098a224000000000ULL);
  e = (vui64_t) CONST_VINT64_DW (1, 4);
  j = test_clz_d((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("clz(0x4b3b4ca85a86c47aULL, 0x098a224000000000ULL) ", j);
#endif
  rc += check_vuint128x ("vec_clzd 15:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (0x100000000ULL, 0x1ffffffffULL);
  e = (vui64_t) CONST_VINT64_DW (31, 31);
  j = test_clz_d((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("clz(0x100000000ULL, 0x1ffffffffULL) ", j);
#endif

  rc += check_vuint128x ("vec_clzd 16:", (vui128_t) j, (vui128_t) e);

  return (rc);
}

#undef __DEBUG_PRINT__
int
test_ctzd (void)
{
  vui64_t i, e;
  vui64_t j;
  int rc = 0;

  printf ("\ntest_ctzd Vector Count Trailing Zeros in Doublewords\n");

  i = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (64, 64);
  j = vec_ctzd((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0, 0) ", j);
#endif
  rc += check_vuint128x ("vec_ctzd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (-1, -1);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_ctzd((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(-1, -1) ", j);
#endif
  rc += check_vuint128x ("vec_ctzd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (0, -1);
  e = (vui64_t) CONST_VINT64_DW (64, 0);
  j = vec_ctzd((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(0, -1) ", j);
#endif
  rc += check_vuint128x ("vec_ctzd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (-1, 0);
  e = (vui64_t) CONST_VINT64_DW (0, 64);
  j = vec_ctzd((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(-1, 0) ", j);
#endif
  rc += check_vuint128x ("vec_ctzd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (1, 8589934596);
  e = (vui64_t) CONST_VINT64_DW (0, 2);
  j = vec_ctzd((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(1, 8589934596) ", j);
#endif
  rc += check_vuint128x ("vec_ctzd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (34359738384, 137438953536);
  e = (vui64_t) CONST_VINT64_DW (4, 6);
  j = vec_ctzd((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(34359738384, 137438953536) ", j);
#endif
  rc += check_vuint128x ("vec_ctzd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (549755814144, 2199023256576);
  e = (vui64_t) CONST_VINT64_DW (8, 10);
  j = vec_ctzd((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(549755814144, 2199023256576) ", j);
#endif
  rc += check_vuint128x ("vec_ctzd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (8796093026304, 35184372105216);
  e = (vui64_t) CONST_VINT64_DW (12, 14);
  j = vec_ctzd((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(8796093026304, 35184372105216) ", j);
#endif
  rc += check_vuint128x ("vec_ctzd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (140737488420864, 562949953683456);
  e = (vui64_t) CONST_VINT64_DW (16, 18);
  j = vec_ctzd((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(140737488420864, 562949953683456) ", j);
#endif
  rc += check_vuint128x ("vec_ctzd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (2251799814733824, 9007199258935296);
  e = (vui64_t) CONST_VINT64_DW (20, 22);
  j = vec_ctzd((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(2251799814733824, 9007199258935296) ", j);
#endif
  rc += check_vuint128x ("vec_ctzd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (36028797035741184, 144115188142964736);
  e = (vui64_t) CONST_VINT64_DW (24, 26);
  j = vec_ctzd((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(36028797035741184, 144115188142964736) ", j);
#endif
  rc += check_vuint128x ("vec_ctzd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (576460752571858944, 2305843010287435776);
  e = (vui64_t) CONST_VINT64_DW (28, 30);
  j = vec_ctzd((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(576460752571858944, 2305843010287435776) ", j);
#endif
  rc += check_vuint128x ("vec_ctzd:", (vui128_t) j, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (4611686020574871552UL, 18446744069414584320UL);
  e = (vui64_t) CONST_VINT64_DW (31, 32);
  j = vec_ctzd((vui64_t)i);

#ifdef __DEBUG_PRINT__
  print_vint128 ("clz(4611686020574871552, 18446744069414584320) ", j);
#endif
  rc += check_vuint128x ("vec_ctzd:", (vui128_t) j, (vui128_t) e);

  return (rc);
}

int
test_muleud (void)
{
  vui64_t i, j;
  vui128_t k, e;
  int rc = 0;

  printf ("\ntest_muleud Vector Multiply Even Unsigned Doublewords\n");
  i = (vui64_t){1, 2};
  j = (vui64_t){101, 102};
  e = (vui128_t) CONST_VINT128_DW128(0, 101);
  k = vec_muleud (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({1, 2, 3, 4}, {10, 20, 30, 40}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){1000000000, 1000000000};
  j = (vui64_t){1000000000, 1000000000};
  e = (vui128_t) CONST_VINT128_DW128(0, 1000000000000000000UL);
  k = vec_muleud (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({1000000000, 1000000000}, {1000000000, 1000000000}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){10000000000, 1000000000};
  j = (vui64_t){10000000000, 1000000000};
  e = (vui128_t) CONST_VINT128_DW128(0x5, 0x6bc75e2d63100000UL);
  k = vec_muleud (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({10000000000, 1000000000}, {10000000000, 1000000000}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){1000000000000000000UL, 0};
  j = (vui64_t){1000000000000000000UL, 0};
  e = (vui128_t) CONST_VINT128_DW128(0x00c097ce7bc90715UL, 0xb34b9f1000000000UL);
  k = vec_muleud (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({10**18, 0}, {10**18, 0}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){0, 1000000000000000000UL};
  j = (vui64_t){0, 1000000000000000000UL};
  e = (vui128_t) CONST_VINT128_DW128(0x0UL, 0x0UL);
  k = vec_muleud (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({0, 10**18}, {0, 10**18}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){100000000000UL, 100000000000UL};
  j = (vui64_t){100000000000UL, 100000000000UL};
  e = (vui128_t) CONST_VINT128_DW128(0x021eUL, 0x19e0c9bab2400000UL);
  k = vec_muleud (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({10**11, 10**11}, {10**11, 10**11}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){0x00000000ffffffffUL, 0x00000000ffffffffUL};
  j = (vui64_t){0x00000000ffffffffUL, 0x00000000ffffffffUL};
  e = (vui128_t) CONST_VINT128_DW128(0x0UL, 0xfffffffe00000001UL);
  k = vec_muleud (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({2**32-1, 2**32-1}, {2**32-1, 2**32-1}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){0xffffffff00000000UL, 0xffffffff00000000UL};
  j = (vui64_t){0xffffffff00000000UL, 0xffffffff00000000UL};
  e = (vui128_t) CONST_VINT128_DW128(0xfffffffe00000001UL, 0x0UL);
  k = vec_muleud (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({2**64-2**32 , 2**64-2**32}, {2**64-2**32 , 2**64-2**32}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){0xffffffffffffffffUL, 0x0000000000000000UL};
  j = (vui64_t){0xffffffffffffffffUL, 0x0000000000000000UL};
  e = (vui128_t) CONST_VINT128_DW128(0xfffffffffffffffeUL, 0x0000000000000001UL);
  k = vec_muleud (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({2**64-1 , 0}, {2**64-1 , 0}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){2000000000000000000UL, 1000000000000000000UL};
  j = (vui64_t){1UL, -1UL};
  e = (vui128_t) CONST_VINT128_DW128(0UL, 2000000000000000000UL);
  k = vec_muleud (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muleud({2**64-1 , 0}, {2**64-1 , 0}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  return (rc);
}

int
test_muloud (void)
{
  vui64_t i, j;
  vui128_t k, e;
  int rc = 0;

  printf ("\ntest_muloud Vector Multiply Odd Unsigned Doublewords\n");
  i = (vui64_t){1, 2};
  j = (vui64_t){101, 102};
  e = (vui128_t) CONST_VINT128_DW128(0, 204);
  k = vec_muloud (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({1, 2, 3, 4}, {10, 20, 30, 40}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){1000000000, 1000000000};
  j = (vui64_t){1000000000, 1000000000};
  e = (vui128_t) CONST_VINT128_DW128(0, 1000000000000000000UL);
  k = vec_muloud (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({1000000000, 1000000000}, {1000000000, 1000000000}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){10000000000, 1000000000};
  j = (vui64_t){10000000000, 1000000000};
  e = (vui128_t) CONST_VINT128_DW128(0x0, 0x0de0b6b3a7640000UL);
  k = vec_muloud (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({10000000000, 1000000000}, {10000000000, 1000000000}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){1000000000000000000UL, 0};
  j = (vui64_t){1000000000000000000UL, 0};
  e = (vui128_t) CONST_VINT128_DW128(0, 0);
  k = vec_muloud (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({10**18, 0}, {10**18, 0}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){0, 1000000000000000000UL};
  j = (vui64_t){0, 1000000000000000000UL};
  e = (vui128_t) CONST_VINT128_DW128(0x00c097ce7bc90715UL, 0xb34b9f1000000000UL);
  k = vec_muloud (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({0, 10**18}, {0, 10**18}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){100000000000UL, 100000000000UL};
  j = (vui64_t){100000000000UL, 100000000000UL};
  e = (vui128_t) CONST_VINT128_DW128(0x021eUL, 0x19e0c9bab2400000UL);
  k = vec_muloud (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({10**11, 10**11}, {10**11, 10**11}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){0x00000000ffffffffUL, 0x00000000ffffffffUL};
  j = (vui64_t){0x00000000ffffffffUL, 0x00000000ffffffffUL};
  e = (vui128_t) CONST_VINT128_DW128(0x0UL, 0xfffffffe00000001UL);
  k = vec_muloud (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({2**32-1, 2**32-1}, {2**32-1, 2**32-1}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){0xffffffff00000000UL, 0xffffffff00000000UL};
  j = (vui64_t){0xffffffff00000000UL, 0xffffffff00000000UL};
  e = (vui128_t) CONST_VINT128_DW128(0xfffffffe00000001UL, 0x0UL);
  k = vec_muloud (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({2**64-2**32 , 2**64-2**32}, {2**64-2**32 , 2**64-2**32}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){ 0x0000000000000000UL, 0xffffffffffffffffUL};
  j = (vui64_t){ 0x0000000000000000UL, 0xffffffffffffffffUL};
  e = (vui128_t) CONST_VINT128_DW128(0xfffffffffffffffeUL, 0x0000000000000001UL);
  k = vec_muloud (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({2**64-1 , 0}, {2**64-1 , 0}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){2000000000000000000UL, 1000000000000000000UL};
  j = (vui64_t){1UL, -1L};
  e = (vui128_t) CONST_VINT128_DW128(0x0de0b6b3a763ffffUL, -1000000000000000000L);
  k = vec_muloud (i, j);

#ifdef __DEBUG_PRINT__
  print_vint128 ("muloud({2**64-1 , 0}, {2**64-1 , 0}) ", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  return (rc);
}

int
test_mulhud (void)
{
  vui64_t i, j;
  vui64_t k, e;
  int rc = 0;

  printf ("\ntest_mulhud Vector Multiply High Unsigned Doublewords\n");

  i = (vui64_t) { 1, 2 };
  j = (vui64_t) { 101, 102 };
  e = (vui64_t) { 0, 0 };
  k = vec_mulhud ( i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("mulhud  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulhud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) {-1, -1 };
  j = (vui64_t) { 1, 2 };
  e = (vui64_t) { 0, 1 };
  k = vec_mulhud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("mulhud  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulhud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){ -1, -1 };
  j = (vui64_t){ 3, 4 };
  e = (vui64_t){ 2, 3 };
  k = vec_mulhud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("mulhud  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulhud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){ 0x100000000, 0x100000000 };
  j = (vui64_t){ 0x100000000, 0x200000000 };
  e = (vui64_t){ 0x1, 0x2 };
  k = vec_mulhud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("mulhud  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulhud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){ 0x200000000, 0x200000000 };
  j = (vui64_t){ 0x300000000, 0x400000000 };
  e = (vui64_t){ 0x6, 0x8 };
  k = vec_mulhud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("mulhud  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulhud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){ 1000000000000000000, 1000000000000000000 };
  j = (vui64_t){ 19, 18447 };
  e = (vui64_t){ 1, 1000 };
  k = vec_mulhud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("mulhud  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulhud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){ 1000000000000000000, 1000000000000000000 };
  j = (vui64_t){ 18446745, 18446744074 };
  e = (vui64_t){ 1000000, 1000000000 };
  k = vec_mulhud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("mulhud  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulhud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){ __UINT64_MAX__, __UINT64_MAX__ };
  j = (vui64_t){ 19, 18447 };
  e = (vui64_t){ 18, 18446 };
  k = vec_mulhud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("mulhud  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulhud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){ __UINT64_MAX__, __UINT64_MAX__ };
  j = (vui64_t){ 18446745, 18446744074 };
  e = (vui64_t){ 18446744, 18446744073 };
  k = vec_mulhud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("mulhud  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulhud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){ __UINT64_MAX__, __UINT64_MAX__ };
  j = (vui64_t){ __UINT64_MAX__, __UINT64_MAX__ };
  e = (vui64_t){ __UINT64_MAX__-1, __UINT64_MAX__-1 };
  k = vec_mulhud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("mulhud  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_mulhud:", (vui128_t)k, (vui128_t) e);

  return (rc);
}

int
test_muludm (void)
{
  vui64_t i, j;
  vui64_t k, e;
  int rc = 0;

  printf ("\ntest_muludm Vector Multiply Unsigned Doubleword Modulo\n");
  i = (vui64_t){1, 2};
  j = (vui64_t){101, 102};
  e = (vui64_t){101, 204};
  k = vec_muludm (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("muludm  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muludm:", (vui128_t)k, (vui128_t) e);
  i = (vui64_t){0x8000000000000000, 0x4000000000000000};
  j = (vui64_t){0x2000000000000000, 0x4000000000000000};
  e = (vui64_t){0x0000000000000000, 0x0000000000000000};
  k = vec_muludm (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("muludm  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muludm:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){ __UINT64_MAX__, __INT64_MAX__ };
  j = (vui64_t){ __UINT64_MAX__, __UINT64_MAX__ };
  e = (vui64_t){ 1, -(__INT64_MAX__)};
  k = vec_muludm (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("muludm  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muludm:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){ __UINT64_MAX__, __INT64_MAX__ };
  j = (vui64_t){ __INT64_MAX__, __INT64_MAX__ };
  e = (vui64_t){ -(__INT64_MAX__), 1 };
  k = vec_muludm (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("muludm  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muludm:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t){1, 20};
  j = (vui64_t){4000, 300};
  e = (vui64_t){4000, 6000};
  k = vec_muludm (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("muludm  ( ", i);
  print_v2int64 ("         ,", j);
  print_v2int64 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muludm:", (vui128_t)k, (vui128_t) e);

  return (rc);
}


//#define __DEBUG_PRINT__ 1
int
test_vmaddeud (void)
{
  vui64_t i, j, m, n;
  vui128_t k, e;
  int rc = 0;

  printf ("\ntest_vmaddeud Vector Multiply-Add Even Unsigned Doublewords\n");
  i = (vui64_t) CONST_VINT64_DW (1, 2);
  j = (vui64_t) CONST_VINT64_DW (101, 102);
  m = (vui64_t) CONST_VINT64_DW (10, 20);
  e = (vui128_t) CONST_VINT128_DW128(0, 111);
  k = vec_vmaddeud (i, j, m);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmaddeud ( ", i);
  print_v2int64 ("          ,", j);
  print_v2int64 ("          ,", m);
  print_vint128 ("         )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmaddeud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (1000000000, 1000000000 );
  j = (vui64_t) CONST_VINT64_DW (1000000000, 1000000000 );
  m = (vui64_t) CONST_VINT64_DW (1000000000, 1000000000 );
  e = (vui128_t) CONST_VINT128_DW128 (0, 1000000001000000000UL);
  k = vec_vmaddeud (i, j, m);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmaddeud ( ", i);
  print_v2int64 ("          ,", j);
  print_v2int64 ("          ,", m);
  print_vint128 ("         )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_maddeud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 1000000000000000000UL, 0 );
  j = (vui64_t) CONST_VINT64_DW ( 1000000000000000000UL, 0 );
  m = (vui64_t) CONST_VINT64_DW ( 1000000000000000000UL, 0 );
  e = (vui128_t) CONST_VINT128_DW128 ( 0x00c097ce7bc90715UL, 0xc12c55c3a7640000UL );
  k = vec_vmaddeud (i, j, m);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmaddeud ( ", i);
  print_v2int64 ("          ,", j);
  print_v2int64 ("          ,", m);
  print_vint128 ("         )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmaddeud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 0xffffffffffffffffUL, 0x0000000000000000UL );
  j = (vui64_t) CONST_VINT64_DW ( 0xffffffffffffffffUL, 0x0000000000000000UL );
  m = (vui64_t) CONST_VINT64_DW ( 0xffffffffffffffffUL, 0x0000000000000000UL );
  e = (vui128_t) CONST_VINT128_DW128 (0xffffffffffffffffUL, 0x0000000000000000UL );
  k = vec_vmaddeud (i, j, m);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmaddeud ( ", i);
  print_v2int64 ("          ,", j);
  print_v2int64 ("          ,", m);
  print_vint128 ("         )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmaddeud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (1, 2);
  j = (vui64_t) CONST_VINT64_DW (101, 102);
  m = (vui64_t) CONST_VINT64_DW (10, 20);
  n = (vui64_t) CONST_VINT64_DW (4, 1);
  e = (vui128_t) CONST_VINT128_DW128(0, 115);
  k = vec_vmadd2eud (i, j, m, n);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmadd2eud ( ", i);
  print_v2int64 ("           ,", j);
  print_v2int64 ("           ,", m);
  print_v2int64 ("           ,", n);
  print_vint128 ("          )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmadd2eud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 1000000000000000000UL, 0 );
  j = (vui64_t) CONST_VINT64_DW ( 1000000000000000000UL, 0 );
  m = (vui64_t) CONST_VINT64_DW ( 0, 1000000000000000000UL );
  n = (vui64_t) CONST_VINT64_DW ( 1000000000000000000UL, 0 );
//  e = (vui128_t) CONST_VINT128_DW128 ( 0x00c097ce7bc90715UL, 0xc12c55c3a7640000UL );
  e = CONST_VUINT128_Qx18d (1000000000000000001UL, 000000000000000000UL);
  k = vec_vmadd2eud (i, j, m, n);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmadd2eud ( ", i);
  print_v2int64 ("           ,", j);
  print_v2int64 ("           ,", m);
  print_v2int64 ("           ,", n);
  print_vint128 ("          )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmadd2eud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 1000000000000000000UL, 0 );
  j = (vui64_t) CONST_VINT64_DW ( 1000000000000000000UL, 0 );
  m = (vui64_t) CONST_VINT64_DW ( 1000000000000000000UL, 0 );
  n = (vui64_t) CONST_VINT64_DW ( 1000000000000000000UL, 0 );
  e = CONST_VUINT128_Qx18d (1000000000000000002UL, 000000000000000000UL);
  k = vec_vmadd2eud (i, j, m, n);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmadd2eud ( ", i);
  print_v2int64 ("           ,", j);
  print_v2int64 ("           ,", m);
  print_v2int64 ("           ,", n);
  print_vint128 ("          )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmadd2eud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 0xffffffffffffffffUL, 0x0000000000000000UL );
  j = (vui64_t) CONST_VINT64_DW ( 0xffffffffffffffffUL, 0x0000000000000000UL );
  m = (vui64_t) CONST_VINT64_DW ( 0xffffffffffffffffUL, 0x0000000000000000UL );
  n = (vui64_t) CONST_VINT64_DW ( 0x0000000000000000UL, 0xffffffffffffffffUL );
  e = (vui128_t) CONST_VINT128_DW128 (0xffffffffffffffffUL, 0x0000000000000000UL );
  k = vec_vmadd2eud (i, j, m, n);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmadd2eud ( ", i);
  print_v2int64 ("           ,", j);
  print_v2int64 ("           ,", m);
  print_v2int64 ("           ,", n);
  print_vint128 ("          )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmadd2eud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 0xffffffffffffffffUL, 0x0000000000000000UL );
  j = (vui64_t) CONST_VINT64_DW ( 0xffffffffffffffffUL, 0x0000000000000000UL );
  m = (vui64_t) CONST_VINT64_DW ( 0xffffffffffffffffUL, 0x0000000000000000UL );
  n = (vui64_t) CONST_VINT64_DW ( 0xffffffffffffffffUL, 0x0000000000000000UL );
  e = (vui128_t) CONST_VINT128_DW128 (0xffffffffffffffffUL, 0xffffffffffffffffUL );
  k = vec_vmadd2eud (i, j, m, n);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmadd2eud ( ", i);
  print_v2int64 ("           ,", j);
  print_v2int64 ("           ,", m);
  print_v2int64 ("           ,", n);
  print_vint128 ("          )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmadd2eud:", (vui128_t)k, (vui128_t) e);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_vmaddoud (void)
{
  vui64_t i, j, m, n;
  vui128_t k, e;
  int rc = 0;

  printf ("\ntest_vmaddoud Vector Multiply-Add Odd Unsigned Doublewords\n");
  i = (vui64_t) CONST_VINT64_DW (1, 2);
  j = (vui64_t) CONST_VINT64_DW (101, 102);
  m = (vui64_t) CONST_VINT64_DW (10, 20);
  e = (vui128_t) CONST_VINT128_DW128(0, 224);
  k = vec_vmaddoud (i, j, m);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmaddoud ( ", i);
  print_v2int64 ("          ,", j);
  print_v2int64 ("          ,", m);
  print_vint128 ("         )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmaddoud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (1000000000, 1000000000 );
  j = (vui64_t) CONST_VINT64_DW (1000000000, 1000000000 );
  m = (vui64_t) CONST_VINT64_DW (1000000000, 1000000000 );
  e = (vui128_t) CONST_VINT128_DW128 (0, 1000000001000000000UL);
  k = vec_vmaddoud (i, j, m);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmaddoud ( ", i);
  print_v2int64 ("          ,", j);
  print_v2int64 ("          ,", m);
  print_vint128 ("         )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_maddoud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 0, 1000000000000000000UL );
  j = (vui64_t) CONST_VINT64_DW ( 0, 1000000000000000000UL );
  m = (vui64_t) CONST_VINT64_DW ( 0, 1000000000000000000UL );
//  e = (vui128_t) CONST_VINT128_DW128 ( 0x00c097ce7bc90715UL, 0xc12c55c3a7640000UL );
  e = CONST_VUINT128_Qx18d (1000000000000000001UL, 000000000000000000UL);
  k = vec_vmaddoud (i, j, m);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmaddoud ( ", i);
  print_v2int64 ("          ,", j);
  print_v2int64 ("          ,", m);
  print_vint128 ("         )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmaddoud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 0xffffffffffffffffUL, 0x0000000000000000UL );
  j = (vui64_t) CONST_VINT64_DW ( 0xffffffffffffffffUL, 0x0000000000000000UL );
  m = (vui64_t) CONST_VINT64_DW ( 0xffffffffffffffffUL, 0x0000000000000000UL );
  e = (vui128_t) CONST_VINT128_DW128 (0x0000000000000000UL, 0x0000000000000000UL );
  k = vec_vmaddoud (i, j, m);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmaddoud ( ", i);
  print_v2int64 ("          ,", j);
  print_v2int64 ("          ,", m);
  print_vint128 ("         )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmaddoud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 0, 0xffffffffffffffffUL );
  j = (vui64_t) CONST_VINT64_DW ( 0, 0xffffffffffffffffUL );
  m = (vui64_t) CONST_VINT64_DW ( 0, 0xffffffffffffffffUL );
  e = (vui128_t) CONST_VINT128_DW128 (0xffffffffffffffffUL, 0x0000000000000000UL );
  k = vec_vmaddoud (i, j, m);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmaddoud ( ", i);
  print_v2int64 ("          ,", j);
  print_v2int64 ("          ,", m);
  print_vint128 ("         )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmaddoud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (1, 2);
  j = (vui64_t) CONST_VINT64_DW (101, 102);
  m = (vui64_t) CONST_VINT64_DW (10, 20);
  n = (vui64_t) CONST_VINT64_DW (4, 1);
  e = (vui128_t) CONST_VINT128_DW128(0, 225);
  k = vec_vmadd2oud (i, j, m, n);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmadd2oud ( ", i);
  print_v2int64 ("           ,", j);
  print_v2int64 ("           ,", m);
  print_v2int64 ("           ,", n);
  print_vint128 ("          )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmadd2oud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 0, 1000000000000000000UL );
  j = (vui64_t) CONST_VINT64_DW ( 0, 1000000000000000000UL );
  m = (vui64_t) CONST_VINT64_DW ( 0, 1000000000000000000UL );
  n = (vui64_t) CONST_VINT64_DW ( 1000000000000000000UL, 0 );
//  e = (vui128_t) CONST_VINT128_DW128 ( 0x00c097ce7bc90715UL, 0xc12c55c3a7640000UL );
  e = CONST_VUINT128_Qx18d (1000000000000000001UL, 000000000000000000UL);
  k = vec_vmadd2oud (i, j, m, n);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmadd2oud ( ", i);
  print_v2int64 ("           ,", j);
  print_v2int64 ("           ,", m);
  print_v2int64 ("           ,", n);
  print_vint128 ("          )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmadd2oud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 0, 1000000000000000000UL );
  j = (vui64_t) CONST_VINT64_DW ( 0, 1000000000000000000UL );
  m = (vui64_t) CONST_VINT64_DW ( 0, 1000000000000000000UL );
  n = (vui64_t) CONST_VINT64_DW ( 0, 1000000000000000000UL );
//  e = (vui128_t) CONST_VINT128_DW128 ( 0x00c097ce7bc90715UL, 0xc12c55c3a7640000UL );
  e = CONST_VUINT128_Qx18d (1000000000000000002UL, 000000000000000000UL);
  k = vec_vmadd2oud (i, j, m, n);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmadd2oud ( ", i);
  print_v2int64 ("           ,", j);
  print_v2int64 ("           ,", m);
  print_v2int64 ("           ,", n);
  print_vint128 ("          )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmadd2oud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 0, 0xffffffffffffffffUL );
  j = (vui64_t) CONST_VINT64_DW ( 0, 0xffffffffffffffffUL );
  m = (vui64_t) CONST_VINT64_DW ( 0xffffffffffffffffUL, 0x0000000000000000UL );
  n = (vui64_t) CONST_VINT64_DW ( 0x0000000000000000UL, 0xffffffffffffffffUL );
  e = (vui128_t) CONST_VINT128_DW128 (0xffffffffffffffffUL, 0x0000000000000000UL );
  k = vec_vmadd2oud (i, j, m, n);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmadd2oud ( ", i);
  print_v2int64 ("           ,", j);
  print_v2int64 ("           ,", m);
  print_v2int64 ("           ,", n);
  print_vint128 ("          )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmadd2oud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 0, 0xffffffffffffffffUL );
  j = (vui64_t) CONST_VINT64_DW ( 0, 0xffffffffffffffffUL );
  m = (vui64_t) CONST_VINT64_DW ( 0, 0xffffffffffffffffUL );
  n = (vui64_t) CONST_VINT64_DW ( 0, 0xffffffffffffffffUL );
  e = (vui128_t) CONST_VINT128_DW128 (0xffffffffffffffffUL, 0xffffffffffffffffUL );
  k = vec_vmadd2oud (i, j, m, n);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmadd2oud ( ", i);
  print_v2int64 ("           ,", j);
  print_v2int64 ("           ,", m);
  print_v2int64 ("           ,", n);
  print_vint128 ("          )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmadd2oud:", (vui128_t)k, (vui128_t) e);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_vmuleud (void)
{
  vui64_t i, j;
  vui128_t k, e;
  int rc = 0;

  printf ("\ntest_vmuleud Vector Multiply Even Unsigned Doublewords\n");
  i = (vui64_t) CONST_VINT64_DW (1, 2);
  j = (vui64_t) CONST_VINT64_DW (101, 102);
  e = (vui128_t) CONST_VINT128_DW128(0, 101);
  k = vec_vmuleud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuleud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128  ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (1000000000, 1000000000 );
  j = (vui64_t) CONST_VINT64_DW (1000000000, 1000000000 );
  e = (vui128_t) CONST_VINT128_DW128 (0, 1000000000000000000UL);
  k = vec_vmuleud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuleud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW (10000000000, 1000000000 );
  j = (vui64_t) CONST_VINT64_DW (10000000000, 1000000000 );
  e = (vui128_t) CONST_VINT128_DW128 (0x5, 0x6bc75e2d63100000UL);
  k = vec_vmuleud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuleud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 1000000000000000000UL, 0 );
  j = (vui64_t) CONST_VINT64_DW ( 1000000000000000000UL, 0 );
  e = (vui128_t) CONST_VINT128_DW128 ( 0x00c097ce7bc90715UL, 0xb34b9f1000000000UL );
  k = vec_vmuleud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuleud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 0, 1000000000000000000UL );
  j = (vui64_t) CONST_VINT64_DW ( 0, 1000000000000000000UL );
  e = (vui128_t) CONST_VINT128_DW128 ( 0x0UL, 0x0UL );
  k = vec_vmuleud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuleud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 100000000000UL, 100000000000UL );
  j = (vui64_t) CONST_VINT64_DW ( 100000000000UL, 100000000000UL );
  e = (vui128_t) CONST_VINT128_DW128 ( 0x021eUL, 0x19e0c9bab2400000UL );
  k = vec_vmuleud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuleud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 0x00000000ffffffffUL, 0x00000000ffffffffUL );
  j = (vui64_t) CONST_VINT64_DW ( 0x00000000ffffffffUL, 0x00000000ffffffffUL );
  e = (vui128_t) CONST_VINT128_DW128 ( 0x0UL, 0xfffffffe00000001UL );
  k = vec_muleud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuleud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 0xffffffff00000000UL, 0xffffffff00000000UL );
  j = (vui64_t) CONST_VINT64_DW ( 0xffffffff00000000UL, 0xffffffff00000000UL );
  e = (vui128_t) CONST_VINT128_DW128 ( 0xfffffffe00000001UL, 0x0UL );
  k = vec_vmuleud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuleud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 0xffffffffffffffffUL, 0x0000000000000000UL );
  j = (vui64_t) CONST_VINT64_DW ( 0xffffffffffffffffUL, 0x0000000000000000UL );
  e = (vui128_t) CONST_VINT128_DW128 (0xfffffffffffffffeUL, 0x0000000000000001UL );
  k = vec_vmuleud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuleud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuleud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 2000000000000000000UL, 1000000000000000000UL );
  j = (vui64_t) CONST_VINT64_DW ( 1UL, -1UL );
  e = (vui128_t) CONST_VINT128_DW128 ( 0UL, 2000000000000000000UL );
  k = vec_vmuleud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuleud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuleud:", (vui128_t)k, (vui128_t) e);

  return (rc);
}

int
test_vmuloud (void)
{
  vui64_t i, j;
  vui128_t k, e;
  int rc = 0;

  printf ("\ntest_vmuloud Vector Multiply Odd Unsigned Doublewords\n");
  i = (vui64_t) CONST_VINT64_DW ( 1, 2 );
  j = (vui64_t) CONST_VINT64_DW ( 101, 102 );
  e = (vui128_t) CONST_VINT128_DW128 (0, 204);
  k = vec_vmuloud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuloud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 1000000000, 1000000000 );
  j = (vui64_t) CONST_VINT64_DW ( 1000000000, 1000000000 );
  e = (vui128_t) CONST_VINT128_DW128 ( 0, 1000000000000000000UL );
  k = vec_vmuloud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuloud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 10000000000, 1000000000 );
  j = (vui64_t) CONST_VINT64_DW ( 10000000000, 1000000000 );
  e = (vui128_t) CONST_VINT128_DW128 ( 0x0, 0x0de0b6b3a7640000UL );
  k = vec_vmuloud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuloud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 1000000000000000000UL, 0 );
  j = (vui64_t) CONST_VINT64_DW ( 1000000000000000000UL, 0 );
  e = (vui128_t) CONST_VINT128_DW128 ( 0, 0 );
  k = vec_vmuloud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuloud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 0, 1000000000000000000UL );
  j = (vui64_t) CONST_VINT64_DW ( 0, 1000000000000000000UL );
  e = (vui128_t) CONST_VINT128_DW128 ( 0x00c097ce7bc90715UL, 0xb34b9f1000000000UL );
  k = vec_vmuloud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuloud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 100000000000UL, 100000000000UL );
  j = (vui64_t) CONST_VINT64_DW ( 100000000000UL, 100000000000UL );
  e = (vui128_t) CONST_VINT128_DW128 ( 0x021eUL, 0x19e0c9bab2400000UL );
  k = vec_vmuloud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuloud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 0x00000000ffffffffUL, 0x00000000ffffffffUL );
  j = (vui64_t) CONST_VINT64_DW ( 0x00000000ffffffffUL, 0x00000000ffffffffUL );
  e = (vui128_t) CONST_VINT128_DW128 ( 0x0UL, 0xfffffffe00000001UL );
  k = vec_vmuloud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuloud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 0xffffffff00000000UL, 0xffffffff00000000UL );
  j = (vui64_t) CONST_VINT64_DW ( 0xffffffff00000000UL, 0xffffffff00000000UL );
  e = (vui128_t) CONST_VINT128_DW128 ( 0xfffffffe00000001UL, 0x0UL );
  k = vec_vmuloud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuloud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_muloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 0x0000000000000000UL, 0xffffffffffffffffUL );
  j = (vui64_t) CONST_VINT64_DW ( 0x0000000000000000UL, 0xffffffffffffffffUL );
  e = (vui128_t) CONST_VINT128_DW128 ( 0xfffffffffffffffeUL, 0x0000000000000001UL );
  k = vec_vmuloud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuloud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuloud:", (vui128_t)k, (vui128_t) e);

  i = (vui64_t) CONST_VINT64_DW ( 2000000000000000000UL, 1000000000000000000UL );
  j = (vui64_t) CONST_VINT64_DW ( 1UL, -1L );
  e = (vui128_t) CONST_VINT128_DW128 ( 0x0de0b6b3a763ffffUL, -1000000000000000000L );
  k = vec_vmuloud (i, j);

#ifdef __DEBUG_PRINT__
  print_v2int64 ("vmuloud ( ", i);
  print_v2int64 ("         ,", j);
  print_vint128 ("        )=", (vui128_t)k);
#endif
  rc += check_vuint128x ("vec_vmuloud:", (vui128_t)k, (vui128_t) e);

  return (rc);
}
#undef __DEBUG_PRINT__

//#define __DEBUG_PRINT__ 1
int
test_maxud (void)
{
  vui64_t i1, i2, j, e;
  int rc = 0;

  printf ("\ntest_maxud Vector Maximum Unsigned Doubleword\n");

  i1 = (vui64_t) { 0, 0 };
  i2 = (vui64_t) { 0, 0 };
  e =  (vui64_t) { 0, 0 };
  j = vec_maxud (i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vmaxud( ", i1);
  print_v2xint64 ("       ,", i2);
  print_v2xint64 ("      )=", j);
#endif
  rc += check_vuint128x ("vec_maxud:", (vui128_t) j, (vui128_t) e);

  i1 = (vui64_t) { 1, 2 };
  i2 = (vui64_t) { 0, 1 };
  e =  (vui64_t) { 1, 2 };
  j = vec_maxud (i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vmaxud( ", i1);
  print_v2xint64 ("       ,", i2);
  print_v2xint64 ("      )=", j);
#endif
  rc += check_vuint128x ("vec_maxud:", (vui128_t) j, (vui128_t) e);

  i1 = (vui64_t) { 0, 1 };
  i2 = (vui64_t) { 1, 2 };
  e =  (vui64_t) { 1, 2 };
  j = vec_maxud (i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vmaxud( ", i1);
  print_v2xint64 ("       ,", i2);
  print_v2xint64 ("      )=", j);
#endif
  rc += check_vuint128x ("vec_maxud:", (vui128_t) j, (vui128_t) e);

  i1 = (vui64_t) { __UINT64_MAX__, __INT64_MAX__ };
  i2 = (vui64_t) { __INT64_MAX__, __UINT64_MAX__ };
  e =  (vui64_t) { __UINT64_MAX__, __UINT64_MAX__ };
  j = vec_maxud (i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vmaxud( ", i1);
  print_v2xint64 ("       ,", i2);
  print_v2xint64 ("      )=", j);
#endif
  rc += check_vuint128x ("vec_maxud:", (vui128_t) j, (vui128_t) e);

  return (rc);
}

int
test_maxsd (void)
{
  vi64_t i1, i2, j, e;
  int rc = 0;

  printf ("\ntest_maxsd Vector Maximum Unsigned Doubleword\n");

  i1 = (vi64_t) { 0, 0 };
  i2 = (vi64_t) { 0, 0 };
  e =  (vi64_t) { 0, 0 };
  j = vec_maxsd (i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vmaxsd( ", (vui64_t) i1);
  print_v2xint64 ("       ,", (vui64_t) i2);
  print_v2xint64 ("      )=", (vui64_t) j);
#endif
  rc += check_vuint128x ("vec_maxsd:", (vui128_t) j, (vui128_t) e);

  i1 = (vi64_t) { 1, 2 };
  i2 = (vi64_t) { 0, 1 };
  e =  (vi64_t) { 1, 2 };
  j = vec_maxsd (i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vmaxsd( ", (vui64_t) i1);
  print_v2xint64 ("       ,", (vui64_t) i2);
  print_v2xint64 ("      )=", (vui64_t) j);
#endif
  rc += check_vuint128x ("vec_maxsd:", (vui128_t) j, (vui128_t) e);

  i1 = (vi64_t) { -1, -2 };
  i2 = (vi64_t) { 0, 1 };
  e =  (vi64_t) { 0, 1 };
  j = vec_maxsd (i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vmaxsd( ", (vui64_t) i1);
  print_v2xint64 ("       ,", (vui64_t) i2);
  print_v2xint64 ("      )=", (vui64_t) j);
#endif
  rc += check_vuint128x ("vec_maxsd:", (vui128_t) j, (vui128_t) e);

  i1 = (vi64_t) { 0, 1 };
  i2 = (vi64_t) { 1, 2 };
  e =  (vi64_t) { 1, 2 };
  j = vec_maxsd (i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vmaxsd( ", (vui64_t) i1);
  print_v2xint64 ("       ,", (vui64_t) i2);
  print_v2xint64 ("      )=", (vui64_t) j);
#endif
  rc += check_vuint128x ("vec_maxsd:", (vui128_t) j, (vui128_t) e);

  i1 = (vi64_t) { 0, 1 };
  i2 = (vi64_t) { -1, -2 };
  e =  (vi64_t) { 0, 1 };
  j = vec_maxsd (i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vmaxsd( ", (vui64_t) i1);
  print_v2xint64 ("       ,", (vui64_t) i2);
  print_v2xint64 ("      )=", (vui64_t) j);
#endif
  rc += check_vuint128x ("vec_maxsd:", (vui128_t) j, (vui128_t) e);

  i1 = (vi64_t) { __UINT64_MAX__, __INT64_MAX__ };
  i2 = (vi64_t) { __INT64_MAX__, __UINT64_MAX__ };
  e =  (vi64_t) { __INT64_MAX__, __INT64_MAX__ };
  j = vec_maxsd (i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vmaxsd( ", (vui64_t) i1);
  print_v2xint64 ("       ,", (vui64_t) i2);
  print_v2xint64 ("      )=", (vui64_t) j);
#endif
  rc += check_vuint128x ("vec_maxsd:", (vui128_t) j, (vui128_t) e);

  return (rc);
}

int
test_minud (void)
{
  vui64_t i1, i2, j, e;
  int rc = 0;

  printf ("\ntest_minud Vector Minimum Unsigned Doubleword\n");

  i1 = (vui64_t) { 0, 0 };
  i2 = (vui64_t) { 0, 0 };
  e =  (vui64_t) { 0, 0 };
  j = vec_minud (i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vminud( ", i1);
  print_v2xint64 ("       ,", i2);
  print_v2xint64 ("      )=", j);
#endif
  rc += check_vuint128x ("vec_minud:", (vui128_t) j, (vui128_t) e);

  i1 = (vui64_t) { 1, 2 };
  i2 = (vui64_t) { 0, 1 };
  e =  (vui64_t) { 0, 1 };
  j = vec_minud (i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vminud( ", i1);
  print_v2xint64 ("       ,", i2);
  print_v2xint64 ("      )=", j);
#endif
  rc += check_vuint128x ("vec_minud:", (vui128_t) j, (vui128_t) e);

  i1 = (vui64_t) { 0, 1 };
  i2 = (vui64_t) { 1, 2 };
  e =  (vui64_t) { 0, 1 };
  j = vec_minud (i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vminud( ", i1);
  print_v2xint64 ("       ,", i2);
  print_v2xint64 ("      )=", j);
#endif
  rc += check_vuint128x ("vec_minud:", (vui128_t) j, (vui128_t) e);

  i1 = (vui64_t) { __UINT64_MAX__, __INT64_MAX__ };
  i2 = (vui64_t) { __INT64_MAX__, __UINT64_MAX__ };
  e =  (vui64_t) { __INT64_MAX__, __INT64_MAX__ };
  j = vec_minud (i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vminud( ", i1);
  print_v2xint64 ("       ,", i2);
  print_v2xint64 ("      )=", j);
#endif
  rc += check_vuint128x ("vec_minud:", (vui128_t) j, (vui128_t) e);

  return (rc);
}

int
test_minsd (void)
{
  vi64_t i1, i2, j, e;
  int rc = 0;

  printf ("\ntest_minsd Vector Minimum Unsigned Doubleword\n");

  i1 = (vi64_t) { 0, 0 };
  i2 = (vi64_t) { 0, 0 };
  e =  (vi64_t) { 0, 0 };
  j = vec_minsd (i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vminsd( ", (vui64_t) i1);
  print_v2xint64 ("       ,", (vui64_t) i2);
  print_v2xint64 ("      )=", (vui64_t) j);
#endif
  rc += check_vuint128x ("vec_minsd:", (vui128_t) j, (vui128_t) e);

  i1 = (vi64_t) { 1, 2 };
  i2 = (vi64_t) { 0, 1 };
  e =  (vi64_t) { 0, 1 };
  j = vec_minsd (i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vminsd( ", (vui64_t) i1);
  print_v2xint64 ("       ,", (vui64_t) i2);
  print_v2xint64 ("      )=", (vui64_t) j);
#endif
  rc += check_vuint128x ("vec_minsd:", (vui128_t) j, (vui128_t) e);

  i1 = (vi64_t) { -1, -2 };
  i2 = (vi64_t) { 0, 1 };
  e =  (vi64_t) { -1, -2 };
  j = vec_minsd (i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vminsd( ", (vui64_t) i1);
  print_v2xint64 ("       ,", (vui64_t) i2);
  print_v2xint64 ("      )=", (vui64_t) j);
#endif
  rc += check_vuint128x ("vec_minsd:", (vui128_t) j, (vui128_t) e);

  i1 = (vi64_t) { 0, 1 };
  i2 = (vi64_t) { 1, 2 };
  e =  (vi64_t) { 0, 1 };
  j = vec_minsd (i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vminsd( ", (vui64_t) i1);
  print_v2xint64 ("       ,", (vui64_t) i2);
  print_v2xint64 ("      )=", (vui64_t) j);
#endif
  rc += check_vuint128x ("vec_minsd:", (vui128_t) j, (vui128_t) e);

  i1 = (vi64_t) { 0, 1 };
  i2 = (vi64_t) { -1, -2 };
  e =  (vi64_t) { -1, -2 };
  j = vec_minsd (i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vminsd( ", (vui64_t) i1);
  print_v2xint64 ("       ,", (vui64_t) i2);
  print_v2xint64 ("      )=", (vui64_t) j);
#endif
  rc += check_vuint128x ("vec_minsd:", (vui128_t) j, (vui128_t) e);

  i1 = (vi64_t) { __UINT64_MAX__, __INT64_MAX__ };
  i2 = (vi64_t) { __INT64_MAX__, __UINT64_MAX__ };
  e =  (vi64_t) { __UINT64_MAX__, __UINT64_MAX__ };
  j = vec_minsd (i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vminsd( ", (vui64_t) i1);
  print_v2xint64 ("       ,", (vui64_t) i2);
  print_v2xint64 ("      )=", (vui64_t) j);
#endif
  rc += check_vuint128x ("vec_minsd:", (vui128_t) j, (vui128_t) e);

  return (rc);
}
#undef __DEBUG_PRINT__

//#define __DEBUG_PRINT__ 1
int
test_cmpud (void)
{
  vui64_t i1, i2, e;
  vb64_t j;
  int rc = 0;

  printf ("\ntest_cmpud Vector Compare Unsigned Doubleword\n");

  printf ("test_cmpequd\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpequd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpequd:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x200000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x200000000);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpequd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpequd:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (2, 2);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpequd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpequd:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 1);
  i2 = (vui64_t) CONST_VINT64_DW (2, 2);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpequd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpequd:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (2, 0x200000000);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpequd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpequd:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpequd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpequd:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpequd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpequd:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 0xffffffff00000000);
  i2 = (vui64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpequd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpequd:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpequd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpequd:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0xffffffff00000000, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpequd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpequd:", (vb64_t)j, (vb64_t) e);

  printf ("test_cmpneud\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpneud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpneud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x200000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x200000000);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpneud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpneud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (2, 2);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpneud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpneud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 1);
  i2 = (vui64_t) CONST_VINT64_DW (2, 2);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpneud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpneud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (2, 0x200000000);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpneud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpneud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpneud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpneud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpneud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpneud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 0xffffffff00000000);
  i2 = (vui64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpneud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpneud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpneud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpneud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0xffffffff00000000, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpneud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpneud:", (vb64_t)j, (vb64_t) e);

  printf ("test_cmpgtud\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpgtud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgtud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgtud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgtud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgtud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgtud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpgtud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgtud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x100000000);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpgtud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgtud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 2);
  i2 = (vui64_t) CONST_VINT64_DW (2, 0x100000000);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpgtud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgtud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgtud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgtud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgtud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgtud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 2);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgtud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgtud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpgtud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgtud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (2, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000000);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpgtud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgtud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000001);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpgtud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgtud:", (vb64_t)j, (vb64_t) e);

  printf ("test_cmpgeud\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpgeud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgeud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgeud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgeud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgeud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgeud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgeud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgeud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgeud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpgeud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgeud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgeud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x100000000);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpgeud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgeud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgeud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 2);
  i2 = (vui64_t) CONST_VINT64_DW (2, 0x100000000);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpgeud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgeud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgeud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpgeud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgeud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgeud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpgeud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgeud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgeud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 2);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpgeud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgeud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgeud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgeud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgeud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgeud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (2, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000000);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgeud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgeud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgeud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000001);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpgeud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgeud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpgeud:", (vb64_t)j, (vb64_t) e);

  printf ("test_cmpltud\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpltud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpltud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpltud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpltud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpltud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpltud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpltud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpltud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x100000000);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpltud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpltud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 2);
  i2 = (vui64_t) CONST_VINT64_DW (2, 0x100000000);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpltud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpltud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpltud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpltud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpltud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpltud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 2);
  e = (vui64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpltud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpltud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpltud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpltud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (2, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000000);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpltud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpltud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000001);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpltud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpltud:", (vb64_t)j, (vb64_t) e);

  printf ("test_cmpleud\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpleud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpleud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpleud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpleud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpleud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpleud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpleud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpleud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpleud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpleud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpleud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpleud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x100000000);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpleud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpleud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpleud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 2);
  i2 = (vui64_t) CONST_VINT64_DW (2, 0x100000000);
  e = (vui64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpleud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpleud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpleud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpleud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpleud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpleud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpleud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpleud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpleud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 2);
  e = (vui64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpleud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpleud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpleud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpleud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpleud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpleud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (2, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000000);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpleud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpleud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpleud:", (vb64_t)j, (vb64_t) e);

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x100000000);
  i2 = (vui64_t) CONST_VINT64_DW (0x100000000, 0x100000001);
  e = (vui64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpleud(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpleud( ", i1);
  print_v2xint64 ("         ,", i2);
  print_v2xint64 ("        )=", j);
#endif
  rc += check_v2b64c ("vec_cmpleud:", (vb64_t)j, (vb64_t) e);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_cmpud_all (void)
{
  vui64_t i1, i2;
#ifdef __DEBUG_PRINT__
  vb64_t j;
#endif
  int rc = 0;

  printf ("\ntest_cmpud_all Vector Compare Unsigned Doubleword\n");

  printf ("test_cmp_all_eq\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_all_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_all_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_all_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_all_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpud_all_eq (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpequd(i1, i2);
    print_v2xint64 ("vec_cmpud_all_eq( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpud_all_eq (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpequd(i1, i2);
    print_v2xint64 ("vec_cmpud_all_eq( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_all_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpequd(i1, i2);
    print_v2xint64 ("vec_cmpud_all_eq( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_all_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_all_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_all_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_all_eq (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpequd(i1, i2);
    print_v2xint64 ("vec_cmpud_all_eq( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_all_eq (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpequd(i1, i2);
    print_v2xint64 ("vec_cmpud_all_eq( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_all_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpequd(i1, i2);
    print_v2xint64 ("vec_cmpud_all_eq( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpud_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_all_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpud_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_all_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  printf ("test_cmp_all_ne\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpud_all_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpneud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_ne( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpud_all_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpneud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_ne( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_all_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpneud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_ne( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_all_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpneud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_ne( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_all_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpneud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_ne( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_all_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpneud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_ne( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpud_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpud_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  printf ("test_cmp_all_gt\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpud_all_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_gt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpud_all_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_gt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_all_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_gt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_all_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_gt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_all_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_gt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_all_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_gt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpud_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpud_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_all_gt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_all_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_gt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  printf ("test_cmp_all_ge\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpud_all_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgeud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_ge( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpud_all_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgeud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_ge( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_all_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgeud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_ge( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_all_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgeud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_ge( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_all_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgeud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_ge( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_all_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgeud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_ge( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpud_all_ge (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpud_all_ge (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_all_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_all_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgeud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_ge( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  printf ("test_cmp_all_lt\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_all_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpud_all_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpud_all_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_all_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_all_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_all_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_all_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_all_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpud_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpud_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);

  if (vec_cmpud_all_lt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_all_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  printf ("test_cmp_all_le\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpud_all_le (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpud_all_le (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_all_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_all_le (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_all_le (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_all_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpud_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpud_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_all_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);

  if (vec_cmpud_all_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_all_le (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_all_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_cmpud_any (void)
{
  vui64_t i1, i2;
#ifdef __DEBUG_PRINT__
  vb64_t j;
#endif
  int rc = 0;

  printf ("\ntest_cmpud_any Vector Compare Unsigned Doubleword\n");

  printf ("test_cmp_any_eq\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_any_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_any_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_any_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_any_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpud_any_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpequd(i1, i2);
    print_v2xint64 ("vec_cmpud_any_eq( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpud_any_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpequd(i1, i2);
    print_v2xint64 ("vec_cmpud_any_eq( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_any_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpequd(i1, i2);
    print_v2xint64 ("vec_cmpud_any_eq( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_any_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_any_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_any_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_any_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpequd(i1, i2);
    print_v2xint64 ("vec_cmpud_any_eq( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_any_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpequd(i1, i2);
    print_v2xint64 ("vec_cmpud_any_eq( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_any_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpequd(i1, i2);
    print_v2xint64 ("vec_cmpud_any_eq( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpud_any_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_any_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpud_any_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpequd(i1, i2);
      print_v2xint64 ("vec_cmpud_any_eq( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  printf ("test_cmp_any_ne\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpud_any_ne (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpneud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_ne( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpud_any_ne (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpneud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_ne( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_any_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpneud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_ne( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_any_ne (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpneud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_ne( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_any_ne (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpneud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_ne( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_any_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpneud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_ne( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpud_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpud_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpneud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ne( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  printf ("test_cmp_any_gt\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpud_any_gt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_gt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpud_any_gt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_gt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_any_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_gt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_any_gt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_gt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_any_gt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_gt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_any_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_gt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpud_any_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpud_any_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_any_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_any_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_gt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_any_gt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_gt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  printf ("test_cmp_any_ge\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpud_any_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgeud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_ge( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpud_any_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgeud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_ge( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_any_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgeud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_ge( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_any_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgeud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_ge( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_any_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgeud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_ge( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_any_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgeud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_ge( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpud_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpud_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgeud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_ge( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_any_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgeud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_ge( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  printf ("test_cmp_any_lt\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_any_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpud_any_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpud_any_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_any_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_any_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_any_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_any_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_any_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpud_any_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpud_any_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_lt( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);

  if (vec_cmpud_any_lt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_any_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_lt( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  printf ("test_cmp_any_le\n");

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpud_any_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpud_any_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (1, 1);
  i2 = (vui64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpud_any_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpud_any_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    } else {
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_any_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_any_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_any_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpud_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpud_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpud_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpud_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpleud(i1, i2);
      print_v2xint64 ("vec_cmpud_any_le( ", i1);
      print_v2xint64 ("                 ,", i2);
      print_v2xint64 ("                )=", j);
#endif
    }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);

  if (vec_cmpud_any_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  }

  i1 = (vui64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vui64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpud_any_le (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpud_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpleud(i1, i2);
    print_v2xint64 ("vec_cmpud_any_le( ", i1);
    print_v2xint64 ("                 ,", i2);
    print_v2xint64 ("                )=", j);
#endif
  } else {
  }

  return (rc);
}
//#undef __DEBUG_PRINT__
//#define __DEBUG_PRINT__ 1
int
test_cmpsd (void)
{
  vi64_t i1, i2, e;
  vb64_t j;
  int rc = 0;

  printf ("\ntest_cmpsd Vector Compare Signed Doubleword\n");

  printf ("test_cmpeqsd\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpeqsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpeqsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpeqsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpeqsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpeqsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpeqsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpeqsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpeqsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, -1);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpeqsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpeqsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpeqsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpeqsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1,-1);
  i2 = (vi64_t) CONST_VINT64_DW (0, -1);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpeqsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpeqsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpeqsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpeqsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1,-1);
  i2 = (vi64_t) CONST_VINT64_DW (-1, -1);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpeqsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpeqsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0xffffffff00000000);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpeqsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpeqsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff00000001, 0xffffffff00000000);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpeqsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpeqsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff00000001, 0xffffffffffffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpeqsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpeqsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffffffffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff7fffffff);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpeqsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpeqsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff7fffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffffffffffff);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpeqsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpequd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpeqsd:", (vb64_t)j, (vb64_t) e);

  printf ("test_cmpgtsd\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, -1);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1,-1);
  i2 = (vi64_t) CONST_VINT64_DW (0, -1);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vi64_t) CONST_VINT64_DW (-1, -1);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vi64_t) CONST_VINT64_DW (-1, -2);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vi64_t) CONST_VINT64_DW (-2, -1);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0xffffffff00000000);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff00000001, 0xffffffff00000000);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff00000001, 0xffffffffffffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffffffffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff7fffffff);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff7fffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffffffffffff);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpgtsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgtsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgtsd:", (vb64_t)j, (vb64_t) e);

  printf ("test_cmpgesd\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, -1);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1,-1);
  i2 = (vi64_t) CONST_VINT64_DW (0, -1);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vi64_t) CONST_VINT64_DW (-1, -1);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vi64_t) CONST_VINT64_DW (-1, -2);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vi64_t) CONST_VINT64_DW (-2, -1);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0xffffffff00000000);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff00000001, 0xffffffff00000000);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff00000001, 0xffffffffffffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffffffffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff7fffffff);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff7fffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffffffffffff);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpgesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpgesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpgesd:", (vb64_t)j, (vb64_t) e);

  printf ("test_cmpltsd\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, -1);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1,-1);
  i2 = (vi64_t) CONST_VINT64_DW (0, -1);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vi64_t) CONST_VINT64_DW (-1, -1);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vi64_t) CONST_VINT64_DW (-1, -2);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vi64_t) CONST_VINT64_DW (-2, -1);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0xffffffff00000000);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff00000001, 0xffffffff00000000);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff00000001, 0xffffffffffffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffffffffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff7fffffff);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff7fffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffffffffffff);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpltsd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpltsd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpltsd:", (vb64_t)j, (vb64_t) e);

  printf ("test_cmplesd\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, -1);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1,-1);
  i2 = (vi64_t) CONST_VINT64_DW (0, -1);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vi64_t) CONST_VINT64_DW (-1, -1);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vi64_t) CONST_VINT64_DW (-1, -2);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vi64_t) CONST_VINT64_DW (-2, -1);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0xffffffff00000000);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff00000001, 0xffffffff00000000);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff00000001, 0xffffffffffffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffffffffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff7fffffff);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff7fffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffffffffffff);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmplesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmplesd( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmplesd:", (vb64_t)j, (vb64_t) e);

  printf ("test_cmpnesd\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpnesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpnesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpnesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpnesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpnesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpnesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpnesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpnesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, -1);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpnesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpnesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpnesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpnesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1,-1);
  i2 = (vi64_t) CONST_VINT64_DW (0, -1);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpnesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpnesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpnesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpnesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (-1, -1);
  i2 = (vi64_t) CONST_VINT64_DW (-1, -1);
  e = (vi64_t) CONST_VINT64_DW (0, 0);
  j = vec_cmpnesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpnesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (1, 0xffffffff00000000);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpnesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpnesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff00000001, 0xffffffff00000000);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff00000000, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (-1, 0);
  j = vec_cmpnesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpnesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff00000001, 0xffffffffffffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff00000000);
  e = (vi64_t) CONST_VINT64_DW (-1, -1);
  j = vec_cmpnesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpnesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffffffffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff7fffffff);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpnesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpnesd:", (vb64_t)j, (vb64_t) e);

  i1 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffff7fffffff);
  i2 = (vi64_t) CONST_VINT64_DW (0xffffffff7fffffff, 0xffffffffffffffff);
  e = (vi64_t) CONST_VINT64_DW (0, -1);
  j = vec_cmpnesd(i1, i2);

#ifdef __DEBUG_PRINT__
  print_v2xint64 ("vcmpneud( ", (vui64_t)i1);
  print_v2xint64 ("         ,", (vui64_t)i2);
  print_v2xint64 ("        )=", (vui64_t)j);
#endif
  rc += check_v2b64c ("vec_cmpnesd:", (vb64_t)j, (vb64_t) e);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_cmpsd_all (void)
{
  vi64_t i1, i2;
#ifdef __DEBUG_PRINT__
  vb64_t j;
#endif
  int rc = 0;

  printf ("\ntest_cmpsd_all Vector Compare Unsigned Doubleword\n");

  printf ("test_cmp_all_eq\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpsd_all_eq (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpeqsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpsd_all_eq (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpeqsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_all_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpeqsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_all_eq (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpeqsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_all_eq (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpeqsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_all_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpeqsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpsd_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpsd_all_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  printf ("test_cmp_all_ne\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpsd_all_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpnesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpsd_all_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpnesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_all_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpnesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_all_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpnesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_all_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpnesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_all_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpnesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpsd_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpsd_all_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  printf ("test_cmp_all_gt\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpsd_all_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpsd_all_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_all_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_all_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_all_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_all_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpsd_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpsd_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_all_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_all_gt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_all_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_gt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  printf ("test_cmp_all_ge\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpsd_all_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpsd_all_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_all_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_ge (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_ge (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_ge (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_all_ge (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_all_ge (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_all_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpsd_all_ge (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpsd_all_ge (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_all_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_all_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_all_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_ge( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  printf ("test_cmp_all_lt\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_all_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpsd_all_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpsd_all_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_all_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_all_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_all_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_all_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpsd_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpsd_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_all_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);

  if (vec_cmpsd_all_lt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_all_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  printf ("test_cmp_all_le\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpsd_all_le (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpsd_all_le (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_all_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_all_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_all_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_all_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpsd_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpsd_all_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_all_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);

  if (vec_cmpsd_all_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_all_le (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_all_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_all_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_cmpsd_any (void)
{
  vi64_t i1, i2;
#ifdef __DEBUG_PRINT__
  vb64_t j;
#endif
  int rc = 0;

  printf ("\ntest_cmpsd_any Vector Compare Unsigned Doubleword\n");

  printf ("test_cmp_any_eq\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpsd_any_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpeqsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpsd_any_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpeqsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_any_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpeqsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_eq (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_any_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpeqsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_any_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpeqsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_any_eq (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpeqsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpsd_any_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpsd_any_eq (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_eq fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpeqsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_eq( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  printf ("test_cmp_any_ne\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_ne (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpsd_any_ne (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpnesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpsd_any_ne (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpnesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_any_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpnesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_any_ne (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpnesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_any_ne (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpnesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_any_ne (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpnesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpsd_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpsd_any_ne (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ne fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpnesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ne( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  printf ("test_cmp_any_gt\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpsd_any_gt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpsd_any_gt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_any_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_any_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_any_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_any_gt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpsd_any_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpsd_any_gt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_any_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_any_gt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgtsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_any_gt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_gt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgtsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_gt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  printf ("test_cmp_any_ge\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpsd_any_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpsd_any_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_any_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_ge (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_any_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_any_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_any_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpsd_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpsd_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_any_ge (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpgesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_any_ge (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_ge fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpgesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_ge( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  printf ("test_cmp_any_lt\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_any_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpsd_any_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpsd_any_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_any_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_any_lt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_any_lt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_any_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpsd_any_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpsd_any_lt (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_any_lt (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmpltsd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);

  if (vec_cmpsd_any_lt (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_any_lt (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_any_lt fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmpltsd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_lt( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  printf ("test_cmp_any_le\n");

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 0);

  if (vec_cmpsd_any_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (0, 1);

  if (vec_cmpsd_any_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (1, 1);
  i2 = (vi64_t) CONST_VINT64_DW (1, 1);

  if (vec_cmpsd_any_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0);

  if (vec_cmpsd_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_any_le (i1, i2))
    {
      rc += 1;
      printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    } else {
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_any_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_any_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_any_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);

  if (vec_cmpsd_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);

  if (vec_cmpsd_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0);

  if (vec_cmpsd_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0, 0x8000000000000000UL);

  if (vec_cmpsd_any_le (i1, i2))
    {
    } else {
      rc += 1;
      printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
      j = vec_cmplesd(i1, i2);
      print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
      print_v2xint64 ("                 ,", (vui64_t)i2);
      print_v2xint64 ("                )=", (vui64_t)j);
#endif
    }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);

  if (vec_cmpsd_any_le (i1, i2))
  {
  } else {
    rc += 1;
    printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  }

  i1 = (vi64_t) CONST_VINT64_DW (0x8000000000000001UL, 0x8000000000000001UL);
  i2 = (vi64_t) CONST_VINT64_DW (0x8000000000000000UL, 0x8000000000000000UL);

  if (vec_cmpsd_any_le (i1, i2))
  {
    rc += 1;
    printf ("vec_cmpsd_any_le fail\n");
#ifdef __DEBUG_PRINT__
    j = vec_cmplesd(i1, i2);
    print_v2xint64 ("vec_cmpsd_any_le( ", (vui64_t)i1);
    print_v2xint64 ("                 ,", (vui64_t)i2);
    print_v2xint64 ("                )=", (vui64_t)j);
#endif
  } else {
  }

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_setbd (void)
{
  vui32_t i, k;
  vui32_t e;
  int rc = 0;

  printf ("\ntest_setbd Vector set bool doubleword\n");

  i = (vui32_t)CONST_VINT32_W(0x7fffffff, 0xff, 0xff000000, 0);
  k = (vui32_t) vec_setb_sd ((vi64_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_setb ", (vui128_t) i);
  print_vint128x ("        =", (vui128_t) k);
#endif
  e = (vui32_t)CONST_VINT32_W(0, 0, -1, -1);
  rc += check_vuint128x ("vec_setb_sd:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(0x80000000, 0, 0, 1);
  k = (vui32_t) vec_setb_sd ((vi64_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_setb ", (vui128_t) i);
  print_vint128x ("        =", (vui128_t) k);
#endif
  e = (vui32_t)CONST_VINT32_W(-1, -1, 0, 0);
  rc += check_vuint128x ("vec_setb_sd:", (vui128_t) k, (vui128_t) e);

  i = (vui32_t)CONST_VINT32_W(-1, -1, -1, -1);
  k = (vui32_t) vec_setb_sd ((vi64_t) i);

#ifdef __DEBUG_PRINT__
  print_vint128x ("vec_setb ", (vui128_t) i);
  print_vint128x ("        =", (vui128_t) k);
#endif
  e = (vui32_t)CONST_VINT32_W(-1, -1, -1, -1);
  rc += check_vuint128x ("vec_setb_sd:", (vui128_t) k, (vui128_t) e);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_splatisd (void)
{
  vi64_t k;
  vi64_t e;
  int rc = 0;

  printf ("\ntest_splatisd Vector Splat Immediate Signed Doubleword\n");

  k = (vi64_t) vec_splat_s64 (0);

#ifdef __DEBUG_PRINT__
  printf         ("vec_splat_s64 ( 0 ) \n");
  print_v2xint64 ("        =", (vui64_t) k);
#endif
  e = (vi64_t)CONST_VINT64_DW(0, 0);
  rc += check_v2ui64x ("vec_splat_s64 :", (vui64_t) k, (vui64_t) e);

  k = (vi64_t) vec_splat_s64 (-16);

#ifdef __DEBUG_PRINT__
  printf         ("vec_splat_s64 ( -16 ) \n");
  print_v2xint64 ("        =", (vui64_t) k);
#endif
  e = (vi64_t)CONST_VINT64_DW(-16, -16);
  rc += check_v2ui64x ("vec_splat_s64 :", (vui64_t) k, (vui64_t) e);

  k = (vi64_t) vec_splat_s64 (-32);

#ifdef __DEBUG_PRINT__
  printf         ("vec_splat_s64 ( -32 ) \n");
  print_v2xint64 ("        =", (vui64_t) k);
#endif
  e = (vi64_t)CONST_VINT64_DW(-32, -32);
  rc += check_v2ui64x ("vec_splat_s64 :", (vui64_t) k, (vui64_t) e);

  k = (vi64_t) vec_splat_s64 (-128);

#ifdef __DEBUG_PRINT__
  printf         ("vec_splat_s64 ( -128 ) \n");
  print_v2xint64 ("        =", (vui64_t) k);
#endif
  e = (vi64_t)CONST_VINT64_DW(-128, -128);
  rc += check_v2ui64x ("vec_splat_s64 :", (vui64_t) k, (vui64_t) e);

  k = (vi64_t) vec_splat_s64 (-129);

#ifdef __DEBUG_PRINT__
  printf         ("vec_splat_s64 ( -129 ) \n");
  print_v2xint64 ("        =", (vui64_t) k);
#endif
  e = (vi64_t)CONST_VINT64_DW(-129, -129);
  rc += check_v2ui64x ("vec_splat_s64 :", (vui64_t) k, (vui64_t) e);

  k = (vi64_t) vec_splat_s64 (-256);

#ifdef __DEBUG_PRINT__
  printf         ("vec_splat_s64 ( -256 ) \n");
  print_v2xint64 ("        =", (vui64_t) k);
#endif
  e = (vi64_t)CONST_VINT64_DW(-256, -256);
  rc += check_v2ui64x ("vec_splat_s64 :", (vui64_t) k, (vui64_t) e);

  k = (vi64_t) vec_splat_s64 (-1024);

#ifdef __DEBUG_PRINT__
  printf         ("vec_splat_s64 ( -1024 ) \n");
  print_v2xint64 ("        =", (vui64_t) k);
#endif
  e = (vi64_t)CONST_VINT64_DW(-1024, -1024);
  rc += check_v2ui64x ("vec_splat_s64 :", (vui64_t) k, (vui64_t) e);

  k = (vi64_t) vec_splat_s64 (15);

#ifdef __DEBUG_PRINT__
  printf         ("vec_splat_s64 ( 15 ) \n");
  print_v2xint64 ("        =", (vui64_t) k);
#endif
  e = (vi64_t)CONST_VINT64_DW(15, 15);
  rc += check_v2ui64x ("vec_splat_s64 :", (vui64_t) k, (vui64_t) e);

  k = (vi64_t) vec_splat_s64 (30);

#ifdef __DEBUG_PRINT__
  printf         ("vec_splat_s64 ( 30 ) \n");
  print_v2xint64 ("        =", (vui64_t) k);
#endif
  e = (vi64_t)CONST_VINT64_DW(30, 30);
  rc += check_v2ui64x ("vec_splat_s64 :", (vui64_t) k, (vui64_t) e);

  k = (vi64_t) vec_splat_s64 (127);

#ifdef __DEBUG_PRINT__
  printf         ("vec_splat_s64 ( 127 ) \n");
  print_v2xint64 ("        =", (vui64_t) k);
#endif
  e = (vi64_t)CONST_VINT64_DW(127, 127);
  rc += check_v2ui64x ("vec_splat_s64 :", (vui64_t) k, (vui64_t) e);

  k = (vi64_t) vec_splat_s64 (128);

#ifdef __DEBUG_PRINT__
  printf         ("vec_splat_s64 ( 128 ) \n");
  print_v2xint64 ("        =", (vui64_t) k);
#endif
  e = (vi64_t)CONST_VINT64_DW(128, 128);
  rc += check_v2ui64x ("vec_splat_s64 :", (vui64_t) k, (vui64_t) e);

  k = (vi64_t) vec_splat_s64 (246);

#ifdef __DEBUG_PRINT__
  printf         ("vec_splat_s64 ( 246 ) \n");
  print_v2xint64 ("        =", (vui64_t) k);
#endif
  e = (vi64_t)CONST_VINT64_DW(246, 246);
  rc += check_v2ui64x ("vec_splat_s64 :", (vui64_t) k, (vui64_t) e);

  k = (vi64_t) vec_splat_s64 (255);

#ifdef __DEBUG_PRINT__
  printf         ("vec_splat_s64 ( 255 ) \n");
  print_v2xint64 ("        =", (vui64_t) k);
#endif
  e = (vi64_t)CONST_VINT64_DW(255, 255);
  rc += check_v2ui64x ("vec_splat_s64 :", (vui64_t) k, (vui64_t) e);

  k = (vi64_t) vec_splat_s64 (1024);

#ifdef __DEBUG_PRINT__
  printf         ("vec_splat_s64 ( 1024 ) \n");
  print_v2xint64 ("        =", (vui64_t) k);
#endif
  e = (vi64_t)CONST_VINT64_DW(1024, 1024);
  rc += check_v2ui64x ("vec_splat_s64 :", (vui64_t) k, (vui64_t) e);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
int
test_splatiud (void)
{
  vui64_t k;
  vui64_t e;
  int rc = 0;

  printf ("\ntest_splatiud Vector Splat Immediate Unsigned Doubleword\n");

  k = (vui64_t) vec_splat_s64 (0);

#ifdef __DEBUG_PRINT__
  printf         ("vec_splat_u64 ( 0 ) \n");
  print_v2xint64 ("        =", (vui64_t) k);
#endif
  e = (vui64_t)CONST_VINT64_DW(0, 0);
  rc += check_v2ui64x ("vec_splat_u64 :", (vui64_t) k, (vui64_t) e);

  k = (vui64_t) vec_splat_u64 (15);

#ifdef __DEBUG_PRINT__
  printf         ("vec_splat_u64 ( 15 ) \n");
  print_v2xint64 ("        =", (vui64_t) k);
#endif
  e = (vui64_t)CONST_VINT64_DW(15, 15);
  rc += check_v2ui64x ("vec_splat_u64 :", (vui64_t) k, (vui64_t) e);

  k = (vui64_t) vec_splat_u64 (32);

#ifdef __DEBUG_PRINT__
  printf         ("vec_splat_u64 ( 32 ) \n");
  print_v2xint64 ("        =", (vui64_t) k);
#endif
  e = (vui64_t)CONST_VINT64_DW(32, 32);
  rc += check_v2ui64x ("vec_splat_u64 :", (vui64_t) k, (vui64_t) e);

  k = (vui64_t) vec_splat_u64 (64);

#ifdef __DEBUG_PRINT__
  printf         ("vec_splat_u64 ( 64 ) \n");
  print_v2xint64 ("        =", (vui64_t) k);
#endif
  e = (vui64_t)CONST_VINT64_DW(64, 64);
  rc += check_v2ui64x ("vec_splat_u64 :", (vui64_t) k, (vui64_t) e);

  k = (vui64_t) vec_splat_u64 (127);

#ifdef __DEBUG_PRINT__
  printf         ("vec_splat_u64 ( 127 ) \n");
  print_v2xint64 ("        =", (vui64_t) k);
#endif
  e = (vui64_t)CONST_VINT64_DW(127, 127);
  rc += check_v2ui64x ("vec_splat_u64 :", (vui64_t) k, (vui64_t) e);

  k = (vui64_t) vec_splat_u64 (128);

#ifdef __DEBUG_PRINT__
  printf         ("vec_splat_u64 ( 128 ) \n");
  print_v2xint64 ("        =", (vui64_t) k);
#endif
  e = (vui64_t)CONST_VINT64_DW(128, 128);
  rc += check_v2ui64x ("vec_splat_u64 :", (vui64_t) k, (vui64_t) e);

  k = (vui64_t) vec_splat_u64 (1024);

#ifdef __DEBUG_PRINT__
  printf         ("vec_splat_u64 ( 1024 ) \n");
  print_v2xint64 ("        =", (vui64_t) k);
#endif
  e = (vui64_t)CONST_VINT64_DW(1024, 1024);
  rc += check_v2ui64x ("vec_splat_u64 :", (vui64_t) k, (vui64_t) e);

  k = (vui64_t) vec_splat_u64 (0x7FFFFFFF);

#ifdef __DEBUG_PRINT__
  printf         ("vec_splat_u64 ( 0x7FFFFFFF ) \n");
  print_v2xint64 ("        =", (vui64_t) k);
#endif
  e = (vui64_t)CONST_VINT64_DW(0x7FFFFFFF, 0x7FFFFFFF);
  rc += check_v2ui64x ("vec_splat_u64 :", (vui64_t) k, (vui64_t) e);

  return (rc);
}

#if 0
extern vui64_t test_vec_divud (vui64_t y, vui64_t z);
extern vui64_t test_vec_divude (vui64_t x, vui64_t z);
#define test_divud test_vec_divud
#define test_divude test_vec_divude
#if 0
extern vui64_t test_vec_divdud (vui64_t x, vui64_t y, vui64_t z);
#define test_divdud test_vec_divdud
#else
#if 0
extern vui64_t test_vec_divdud_V0 (vui64_t x, vui64_t y, vui64_t z);
#define test_divdud test_vec_divdud_V0
#else
extern vui64_t test_vec_divdud_V1 (vui64_t x, vui64_t y, vui64_t z);
#define test_divdud test_vec_divdud_V1
#endif
#endif
#else
#if 1
// Use static lib implementations.
extern vui64_t __VEC_PWR_IMP(vec_divdud) (vui64_t x, vui64_t y, vui64_t z);
#define test_divdud __VEC_PWR_IMP(vec_divdud)
extern vui64_t __VEC_PWR_IMP(vec_divud) (vui64_t y, vui64_t z);
#define test_divud __VEC_PWR_IMP(vec_divud)
extern vui64_t __VEC_PWR_IMP(vec_diveud) (vui64_t x, vui64_t z);
#define test_divude __VEC_PWR_IMP(vec_diveud)
#else
// use implementations from vec_int64_dummy compile tests.
extern vui64_t test_divdud (vui64_t x, vui64_t y, vui64_t z);
extern vui64_t test_divud (vui64_t y, vui64_t z);
extern vui64_t test_divude (vui64_t x, vui64_t z);
#endif
#endif

int
test_vec_divide_dw (void)
{

  vui64_t ix[4];
  vui64_t er;
  vui64_t rq;
  int rc = 0;

  printf ("\ntest Vector divide Unsigned Doubleword\n");

  ix[0] = (vui64_t)CONST_VINT128_DW(0, 0);
  ix[1] = (vui64_t)CONST_VINT128_DW(__UINT64_MAX__, __UINT64_MAX__);
  ix[2] = (vui64_t)CONST_VINT128_DW(1, __UINT64_MAX__);
  er = (vui64_t)CONST_VINT128_DW(__UINT64_MAX__, 1);

  rq = test_divdud (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_v2xint64 (" divdud ", ix[0]);
  print_v2xint64 ("        ", ix[1]);
  print_v2xint64 ("        ", ix[2]);
  print_v2xint64 ("     rq=", rq);
#endif

  rc += check_v2ui64x ("divdud:", rq, er);

  ix[0] = (vui64_t)CONST_VINT128_DW(0, 0);
  ix[1] = (vui64_t)CONST_VINT128_DW(__UINT64_MAX__, __UINT64_MAX__);
  ix[2] = (vui64_t)CONST_VINT128_DW(1, __UINT64_MAX__);
  er = (vui64_t)CONST_VINT128_DW(__UINT64_MAX__, 1);

  rq = test_divud (ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_v2xint64 (" divud  ", ix[1]);
  print_v2xint64 ("        ", ix[2]);
  print_v2xint64 ("     rq=", rq);
#endif

  rc += check_v2ui64x ("divud:", rq, er);

  ix[1] = (vui64_t)CONST_VINT128_DW(__UINT64_MAX__, 1000000000000000000UL);
  ix[2] = (vui64_t)CONST_VINT128_DW(1000000000000000000UL,
				    1000000000000000000UL);
  er =    (vui64_t)CONST_VINT128_DW(18UL, 1UL);

  rq = test_divud (ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_v2xint64 (" divud  ", ix[1]);
  print_v2xint64 ("        ", ix[2]);
  print_v2xint64 ("     rq=", rq);
#endif

  rc += check_v2ui64x ("divud:", rq, er);

  ix[1] = (vui64_t)CONST_VINT128_DW(__UINT64_MAX__, 1000000000000000000UL);
  ix[2] = (vui64_t)CONST_VINT128_DW(1000000000UL,
				    1000000000UL);
  er =    (vui64_t)CONST_VINT128_DW(0x44b82fa09UL, 1000000000UL);

  rq = test_divud (ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_v2xint64 (" divud  ", ix[1]);
  print_v2xint64 ("        ", ix[2]);
  print_v2xint64 ("     rq=", rq);
#endif

  rc += check_v2ui64x ("divud:", rq, er);

  ix[0] = (vui64_t)CONST_VINT128_DW((__UINT64_MAX__-1), (__UINT64_MAX__-2));
  ix[1] = (vui64_t)CONST_VINT128_DW(0, 0);
  ix[2] = (vui64_t)CONST_VINT128_DW(__UINT64_MAX__, __UINT64_MAX__);
  er = (vui64_t)CONST_VINT128_DW((__UINT64_MAX__-1), (__UINT64_MAX__-2));

  rq = test_divdud (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_v2xint64 (" divdud ", ix[0]);
  print_v2xint64 ("        ", ix[1]);
  print_v2xint64 ("        ", ix[2]);
  print_v2xint64 ("     rq=", rq);
#endif

  rc += check_v2ui64x ("divdud:", rq, er);

  rq = test_divude (ix[0], ix[2]);

#ifdef __DEBUG_PRINT__
  print_v2xint64 (" divude ", ix[0]);
  print_v2xint64 ("        ", ix[2]);
  print_v2xint64 ("     rq=", rq);
#endif

  rc += check_v2ui64x ("divude:", rq, er);

  ix[0] = (vui64_t)CONST_VINT128_DW((__UINT64_MAX__-1), (__UINT64_MAX__-2));
  ix[1] = (vui64_t)CONST_VINT128_DW(1, 1);
  ix[2] = (vui64_t)CONST_VINT128_DW(__UINT64_MAX__, __UINT64_MAX__);
  er = (vui64_t)CONST_VINT128_DW(__UINT64_MAX__, (__UINT64_MAX__-2));

  rq = test_divdud (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_v2xint64 (" divdud ", ix[0]);
  print_v2xint64 ("        ", ix[1]);
  print_v2xint64 ("        ", ix[2]);
  print_v2xint64 ("     rq=", rq);
#endif

  rc += check_v2ui64x ("divdud:", rq, er);

  ix[0] = (vui64_t)CONST_VINT128_DW(0x00c097ce7bc90715UL,
				    0x00c097ce7bc90715UL ); /* 10**36 */
  ix[1] = (vui64_t)CONST_VINT128_DW(0xb34b9f1000000000UL,
				    0xC12C55C3A763ffffUL); /* +10**18 -1 */
  ix[2] = (vui64_t)CONST_VINT128_DW(1000000000000000000UL,
				    1000000000000000000UL);
  er =    (vui64_t)CONST_VINT128_DW(1000000000000000000UL,
				    1000000000000000000UL);

  rq = test_divdud (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_v2xint64 (" divdud ", ix[0]);
  print_v2xint64 ("        ", ix[1]);
  print_v2xint64 ("        ", ix[2]);
  print_v2xint64 ("     rq=", rq);
#endif

  rc += check_v2ui64x ("divdud:", rq, er);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
#ifdef __DEBUG_PRINT__
#define test_modud db_vec_modud
#else
#if 0
extern vui64_t test_vec_modud (vui64_t y, vui64_t z);
extern vui64_t test_vec_moddud (vui64_t x, vui64_t y, vui64_t z);
#define test_modud test_vec_modud
#define test_moddud test_vec_moddud
#else
#if 1
// Use static lib implementations.
extern vui64_t __VEC_PWR_IMP(vec_modud) (vui64_t y, vui64_t z);
#define test_modud __VEC_PWR_IMP(vec_modud)
extern vui64_t __VEC_PWR_IMP(vec_moddud) (vui64_t x, vui64_t y, vui64_t z);
#define test_moddud __VEC_PWR_IMP(vec_moddud)
#else
// use implementations from vec_int64_dummy compile tests.
extern vui64_t test_modud (vui64_t y, vui64_t z);
extern vui64_t test_moddud (vui64_t x, vui64_t y, vui64_t z);
#endif
#endif
#endif

int
test_vec_modulo_dw (void)
{

  vui64_t ix[4];
  vui64_t er;
  vui64_t rq;
  int rc = 0;

  printf ("\ntest Vector modulo Unsigned Doubleword\n");

  ix[0] = (vui64_t)CONST_VINT128_DW((__UINT64_MAX__-1), (__UINT64_MAX__-2));
  ix[1] = (vui64_t)CONST_VINT128_DW(0, 0);
  ix[2] = (vui64_t)CONST_VINT128_DW(__UINT64_MAX__, __UINT64_MAX__);
  er = (vui64_t)CONST_VINT128_DW((__UINT64_MAX__-1), (__UINT64_MAX__-2));

  rq = test_moddud (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_v2xint64 (" moddud ", ix[0]);
  print_v2xint64 ("        ", ix[1]);
  print_v2xint64 ("        ", ix[2]);
  print_v2xint64 ("     rq=", rq);
#endif

  rc += check_v2ui64x ("moddud:", rq, er);

  ix[0] = (vui64_t)CONST_VINT128_DW((__UINT64_MAX__-1), (__UINT64_MAX__-2));
  ix[1] = (vui64_t)CONST_VINT128_DW(1, 1);
  ix[2] = (vui64_t)CONST_VINT128_DW(__UINT64_MAX__, __UINT64_MAX__);
  er = (vui64_t)CONST_VINT128_DW(0, (__UINT64_MAX__-1));

  rq = test_moddud (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_v2xint64 (" moddud ", ix[0]);
  print_v2xint64 ("        ", ix[1]);
  print_v2xint64 ("        ", ix[2]);
  print_v2xint64 ("     rq=", rq);
#endif

  rc += check_v2ui64x ("moddud:", rq, er);

  ix[0] = (vui64_t)CONST_VINT128_DW(0x00c097ce7bc90715UL,
				    0x00c097ce7bc90715UL ); /* 10**36 */
  ix[1] = (vui64_t)CONST_VINT128_DW(0xb34b9f1000000000UL,
				    0xC12C55C3A763ffffUL); /* +10**18 -1 */
  ix[2] = (vui64_t)CONST_VINT128_DW(1000000000000000000UL,
				    1000000000000000000UL);
  er =    (vui64_t)CONST_VINT128_DW(0UL, 999999999999999999UL);

  rq = test_moddud (ix[0], ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_v2xint64 (" moddud ", ix[0]);
  print_v2xint64 ("        ", ix[1]);
  print_v2xint64 ("        ", ix[2]);
  print_v2xint64 ("     rq=", rq);
#endif

  rc += check_v2ui64x ("moddud:", rq, er);

  ix[0] = (vui64_t)CONST_VINT128_DW(0, 0);
  ix[1] = (vui64_t)CONST_VINT128_DW(__UINT64_MAX__, __UINT64_MAX__);
  ix[2] = (vui64_t)CONST_VINT128_DW(16UL, 100UL);
  er = (vui64_t)CONST_VINT128_DW(0xfUL, 15UL);

  rq = test_modud (ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_v2xint64 (" modud  ", ix[1]);
  print_v2xint64 ("        ", ix[2]);
  print_v2xint64 ("     rq=", rq);
#endif

  rc += check_v2ui64x ("modud:", rq, er);

  ix[1] = (vui64_t)CONST_VINT128_DW(__UINT64_MAX__, 1000000000000000000UL);
  ix[2] = (vui64_t)CONST_VINT128_DW(1000000000000000000UL,
				    1000000000000000000UL);
  er =    (vui64_t)CONST_VINT128_DW(0x633275e3af7ffffUL,0000000000000000UL);

  rq = test_modud (ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_v2xint64 (" modud  ", ix[1]);
  print_v2xint64 ("        ", ix[2]);
  print_v2xint64 ("     rq=", rq);
#endif

  rc += check_v2ui64x ("modud:", rq, er);

  ix[1] = (vui64_t)CONST_VINT128_DW(__UINT64_MAX__, 1000000000000000000UL);
  ix[2] = (vui64_t)CONST_VINT128_DW(1000000000UL,
				    1000000000UL);
  er =    (vui64_t)CONST_VINT128_DW(0x44b82fa09UL, 1000000000UL);

  rq = test_modud (ix[1], ix[2]);

#ifdef __DEBUG_PRINT__
  print_v2xint64 (" modud  ", ix[1]);
  print_v2xint64 ("        ", ix[2]);
  print_v2xint64 ("     rq=", rq);
#endif

  return (rc);
}

//#define __DEBUG_PRINT__ 1
#ifdef __DEBUG_PRINT__
#define test_divqud db_vec_divqud
#else
#if 0
extern vui64_t test_vec_divqud (vui128_t x_y, vui64_t z);
#define test_divqud test_vec_divqud
#else
#if 1
// Use static lib implementations.
extern vui64_t __VEC_PWR_IMP(vec_divqud) (vui128_t x_y, vui64_t z);
#define test_divqud __VEC_PWR_IMP(vec_divqud)
#else
// use implementations from vec_int64_dummy compile tests.
extern vui64_t test_divqud (vui128_t x_y, vui64_t z);
#endif
#endif
#endif

int
test_vec_divide_qud (void)
{
  vui64_t ix[4];
  vui64_t er;
  vui64_t rq;
  int rc = 0;

  printf ("\ntest Vector divide Unsigned quadword by doubleword\n");

  ix[0] = (vui64_t)CONST_VINT128_DW(0, 0);
  ix[1] = (vui64_t)CONST_VINT128_DW(__UINT64_MAX__, 0UL);
  er = (vui64_t)CONST_VINT128_DW(0, 0);

  rq = test_divqud ((vui128_t) ix[0], ix[1]);

#ifdef __DEBUG_PRINT__
  print_v2xint64 (" divqud ", ix[0]);
  print_v2xint64 ("        ", ix[1]);
  print_v2xint64 ("     rq=", rq);
#endif

  rc += check_v2ui64x ("divqud:", rq, er);

  ix[0] = (vui64_t)CONST_VINT128_DW(0, __UINT64_MAX__);
  ix[1] = (vui64_t)CONST_VINT128_DW(__UINT64_MAX__, 0UL);
  er = (vui64_t)CONST_VINT128_DW(0, 1);

  rq = test_divqud ((vui128_t) ix[0], ix[1]);

#ifdef __DEBUG_PRINT__
  print_v2xint64 (" divqud ", ix[0]);
  print_v2xint64 ("        ", ix[1]);
  print_v2xint64 ("     rq=", rq);
#endif

  rc += check_v2ui64x ("divqud:", rq, er);

  ix[0] = (vui64_t)CONST_VINT128_DW((__UINT64_MAX__-1), 1);
  ix[1] = (vui64_t)CONST_VINT128_DW(__UINT64_MAX__, 0UL);
  er = (vui64_t)CONST_VINT128_DW(0, __UINT64_MAX__);

  rq = test_divqud ((vui128_t) ix[0], ix[1]);

#ifdef __DEBUG_PRINT__
  print_v2xint64 (" divqud ", ix[0]);
  print_v2xint64 ("        ", ix[1]);
  print_v2xint64 ("     rq=", rq);
#endif

  rc += check_v2ui64x ("divqud:", rq, er);

  ix[0] = (vui64_t)CONST_VINT128_DW((__UINT64_MAX__-1), 2);
  ix[1] = (vui64_t)CONST_VINT128_DW(__UINT64_MAX__, 0UL);
  er = (vui64_t)CONST_VINT128_DW(1, __UINT64_MAX__);

  rq = test_divqud ((vui128_t) ix[0], ix[1]);

#ifdef __DEBUG_PRINT__
  print_v2xint64 (" divqud ", ix[0]);
  print_v2xint64 ("        ", ix[1]);
  print_v2xint64 ("     rq=", rq);
#endif

  rc += check_v2ui64x ("divqud:", rq, er);

  ix[0] = (vui64_t)CONST_VINT128_DW((__UINT64_MAX__-1), __UINT64_MAX__);
  ix[1] = (vui64_t)CONST_VINT128_DW(__UINT64_MAX__, 0UL);
  er = (vui64_t)CONST_VINT128_DW((__UINT64_MAX__-1), __UINT64_MAX__);

  rq = test_divqud ((vui128_t) ix[0], ix[1]);

#ifdef __DEBUG_PRINT__
  print_v2xint64 (" divqud ", ix[0]);
  print_v2xint64 ("        ", ix[1]);
  print_v2xint64 ("     rq=", rq);
#endif

  rc += check_v2ui64x ("divqud:", rq, er);

  return (rc);
}

//#define __DEBUG_PRINT__ 1
#if 0
// test directly from vec_char_ppc.h
#define test_expandm(_l)	vec_expandm_doubleword(_l)
#else
// test from vec_char_ppc.h via vec_int32_dummy.c
extern vui64_t test_vec_expandm_doubleword (vui64_t);
#define test_expandm(_l)	test_vec_expandm_doubleword(_l)
#endif

int
test_expandm_doubleword(void)
{
  vui64_t i, j, e;
    int rc = 0;

    printf ("\n%s\n", __FUNCTION__);

    i = CONST_VINT128_DW (0x00000000c0000000UL,
			  0x00020000f0000000UL);

    e = CONST_VINT128_DW (0x0000000000000000ULL,
			  0x0000000000000000ULL);
    j = test_expandm (i);

#ifdef __DEBUG_PRINT__
    print_v2xint64 ("vec_expandm of ", i);
    print_v2xint64 ("             = ", j);
#endif
    rc += check_vuint128x ("vec_expandm:", (vui128_t) j, (vui128_t) e);

    i = CONST_VINT128_DW (0x00000000c0000000UL,
			  0x80020000f0000000UL);

    e = CONST_VINT128_DW (0x0000000000000000ULL,
			  0xffffffffffffffffULL);
    j = test_expandm (i);

#ifdef __DEBUG_PRINT__
    print_v2xint64 ("vec_expandm of ", i);
    print_v2xint64 ("             = ", j);
#endif
    rc += check_vuint128x ("vec_expandm:", (vui128_t) j, (vui128_t) e);

    i = CONST_VINT128_DW (0xf0000000c0000000UL,
			  0x80020000f0000000UL);

    e = CONST_VINT128_DW (0xffffffffffffffffULL,
			  0xffffffffffffffffULL);
    j = test_expandm (i);

#ifdef __DEBUG_PRINT__
    print_v2xint64 ("vec_expandm of ", i);
    print_v2xint64 ("             = ", j);
#endif
    rc += check_vuint128x ("vec_expandm:", (vui128_t) j, (vui128_t) e);

    i = CONST_VINT128_DW (0xf0000000c0000000UL,
			  0x70020000f0000000UL);

    e = CONST_VINT128_DW (0xffffffffffffffffULL,
			  0x0000000000000000ULL);
    j = test_expandm (i);

#ifdef __DEBUG_PRINT__
    print_v2xint64 ("vec_expandm of ", i);
    print_v2xint64 ("             = ", j);
#endif
    rc += check_vuint128x ("vec_expandm:", (vui128_t) j, (vui128_t) e);

    return (rc);
  }

//#define __DEBUG_PRINT__ 1
#if 0
// test directly from vec_char_ppc.h
#define test_signextllb(_l)	vec_signextll_byte(_l)
#else
// test from vec_char_ppc.h via vec_char_dummy.c
extern vi64_t test_vec_signextll_byte (vi8_t);
#define test_signextllb(_l)	test_vec_signextll_byte(_l)
#endif

int
test_signextll_b(void)
{
    vi8_t i;
    vi64_t j, e;
    int rc = 0;

    printf ("\n%s\n", __FUNCTION__);

    i = (vi8_t)  {0x00, 0x01, 0x02, 0x03, 0x80, 0xc0, 0xe0, 0xf0,
                  0x08, 0x0c, 0x0e, 0x0f, 0x8f, 0xcf, 0xef, 0xff};
    e = (vi64_t) {0x0000000000000000, 0x0000000000000008};
    j = test_signextllb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x   ("vec_signextll of ", (vui8_t)i);
    print_v2xint64 ("               = ", (vui64_t)j);
#endif
    rc += check_vuint128x ("vec_signextll:", (vui128_t) j, (vui128_t) e);

    i = (vi8_t)  {0x00, 0x01, 0x02, 0x03, 0x08, 0x0c, 0x0e, 0x0f,
                  0x80, 0xc0, 0xe0, 0xf0, 0x8f, 0xcf, 0xef, 0xff};
    e = (vi64_t) { 0x0000000000000000, 0xffffffffffffff80};
    j = test_signextllb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x   ("vec_signextll of ", (vui8_t)i);
    print_v2xint64 ("               = ", (vui64_t)j);
#endif
    rc += check_vuint128x ("vec_signextll:", (vui128_t) j, (vui128_t) e);

    i = (vi8_t)  {0x8f, 0xcf, 0xef, 0xff, 0x80, 0xc0, 0xe0, 0xf0,
                  0x08, 0x0c, 0x0e, 0x0f, 0x00, 0x01, 0x02, 0x03};
    e = (vi64_t) {0xffffffffffffff8f, 0x0000000000000008};
    j = test_signextllb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x   ("vec_signextll of ", (vui8_t)i);
    print_v2xint64 ("               = ", (vui64_t)j);
#endif
    rc += check_vuint128x ("vec_signextll:", (vui128_t) j, (vui128_t) e);

    i = (vi8_t)  {0x8f, 0xcf, 0xef, 0xff, 0x08, 0x0c, 0x0e, 0x0f,
                  0x80, 0xc0, 0xe0, 0xf0, 0x00, 0x01, 0x02, 0x03};
    e = (vi64_t) {0xffffffffffffff8f, 0xffffffffffffff80};
    j = test_signextllb (i);

#ifdef __DEBUG_PRINT__
    print_vint8x   ("vec_signextll of ", (vui8_t)i);
    print_v2xint64 ("               = ", (vui64_t)j);
#endif
    rc += check_vuint128x ("vec_signextll:", (vui128_t) j, (vui128_t) e);

    return (rc);
  }

//#define __DEBUG_PRINT__ 1
#if 0
// test directly from vec_char_ppc.h
#define test_signextllh(_l)	vec_signextll_halfword(_l)
#else
// test from vec_char_ppc.h via vec_char_dummy.c
extern vi64_t test_vec_signextll_halfword (vi16_t);
#define test_signextllh(_l)	test_vec_signextll_halfword(_l)
#endif

int
test_signextll_h(void)
{
    vi16_t i;
    vi64_t j, e;
    int rc = 0;

    printf ("\n%s\n", __FUNCTION__);

    i = (vi16_t) {0x0001, 0x0203, 0x80c0, 0xe0f0,
                  0x080c, 0x0e0f, 0x8fcf, 0xefff};
    e = (vi64_t) {0x0000000000000001, 0x000000000000080c};
    j = test_signextllh (i);

#ifdef __DEBUG_PRINT__
    print_vint16x  ("vec_signextll of ", (vui16_t)i);
    print_v2xint64 ("               = ", (vui64_t)j);
#endif
    rc += check_vuint128x ("vec_signextll:", (vui128_t) j, (vui128_t) e);

    i = (vi16_t) {0x0001, 0x0203, 0x080c, 0x0e0f,
                  0x80c0, 0xe0f0, 0x8fcf, 0xefff};
    e = (vi64_t) {0x0000000000000001, 0xffffffffffff80c0};
    j = test_signextllh (i);

#ifdef __DEBUG_PRINT__
    print_vint16x  ("vec_signextll of ", (vui16_t)i);
    print_v2xint64 ("               = ", (vui64_t)j);
#endif
    rc += check_vuint128x ("vec_signextll:", (vui128_t) j, (vui128_t) e);

    i = (vi16_t) {0x8fcf, 0xefff, 0x80c0, 0xe0f0,
                  0x080c, 0x0e0f, 0x0001, 0x0203};
    e = (vi64_t) {0xffffffffffff8fcf, 0x000000000000080c};
    j = test_signextllh (i);

#ifdef __DEBUG_PRINT__
    print_vint16x  ("vec_signextll of ", (vui16_t)i);
    print_v2xint64 ("               = ", (vui64_t)j);
#endif
    rc += check_vuint128x ("vec_signextll:", (vui128_t) j, (vui128_t) e);

    i = (vi16_t) {0x8fcf, 0xefff, 0x080c, 0x0e0f,
                  0x80c0, 0xe0f0, 0x0001, 0x0203};
    e = (vi64_t) {0xffffffffffff8fcf, 0xffffffffffff80c0};
    j = test_signextllh (i);

#ifdef __DEBUG_PRINT__
    print_vint16x  ("vec_signextll of ", (vui16_t)i);
    print_v2xint64 ("               = ", (vui64_t)j);
#endif
    rc += check_vuint128x ("vec_signextll:", (vui128_t) j, (vui128_t) e);

    return (rc);
  }

//#define __DEBUG_PRINT__ 1
#if 0
// test directly from vec_char_ppc.h
#define test_signextllw(_l)	vec_signextll_word(_l)
#else
// test from vec_char_ppc.h via vec_char_dummy.c
extern vi64_t test_vec_signextll_word (vi32_t);
#define test_signextllw(_l)	test_vec_signextll_word(_l)
#endif

int
test_signextll_w(void)
{
    vi32_t i;
    vi64_t j, e;
    int rc = 0;

    printf ("\n%s\n", __FUNCTION__);

    i = (vi32_t) {0x00010203, 0x80c0e0f0,
                  0x080c0e0f, 0x8fcfefff};
    e = (vi64_t) {0x0000000000010203, 0x00000000080c0e0f};
    j = test_signextllw (i);

#ifdef __DEBUG_PRINT__
    print_vint32x  ("vec_signextll of ", (vui32_t)i);
    print_v2xint64 ("               = ", (vui64_t)j);
#endif
    rc += check_vuint128x ("vec_signextll:", (vui128_t) j, (vui128_t) e);

    i = (vi32_t) {0x00010203, 0x080c0e0f,
                  0x80c0e0f0, 0x8fcfefff};
    e = (vi64_t) {0x0000000000010203, 0xffffffff80c0e0f0};
    j = test_signextllw (i);

#ifdef __DEBUG_PRINT__
    print_vint32x  ("vec_signextll of ", (vui32_t)i);
    print_v2xint64 ("               = ", (vui64_t)j);
#endif
    rc += check_vuint128x ("vec_signextll:", (vui128_t) j, (vui128_t) e);

    i = (vi32_t) {0x8fcfefff, 0x80c0e0f0,
                  0x080c0e0f, 0x00010203};
    e = (vi64_t) {0xffffffff8fcfefff, 0x0000000080c0e0f};
    j = test_signextllw (i);

#ifdef __DEBUG_PRINT__
    print_vint32x  ("vec_signextll of ", (vui32_t)i);
    print_v2xint64 ("               = ", (vui64_t)j);
#endif
    rc += check_vuint128x ("vec_signextll:", (vui128_t) j, (vui128_t) e);

    i = (vi32_t) {0x8fcfefff, 0x080c0e0f,
                  0x80c0e0f0, 0x00010203};
    e = (vi64_t) {0xffffffff8fcfefff, 0xffffffff80c0e0f0};
    j = test_signextllw (i);

#ifdef __DEBUG_PRINT__
    print_vint32x  ("vec_signextll of ", (vui32_t)i);
    print_v2xint64 ("               = ", (vui64_t)j);
#endif
    rc += check_vuint128x ("vec_signextll:", (vui128_t) j, (vui128_t) e);

    return (rc);
  }

int
test_vec_i64 (void)
{
  int rc = 0;

  printf ("\n%s\n", __FUNCTION__);

  rc += test_signextll_b ();
  rc += test_signextll_h ();
  rc += test_signextll_w ();
  rc += test_expandm_doubleword ();
  rc += test_permdi ();
  rc += test_xxspltd ();
  rc += test_mrgahld ();
  rc += test_mrghld ();
  rc += test_mrgeod ();
  rc += test_splatd ();
  rc += test_revbd ();
  rc += test_clzd ();
  rc += test_ctzd ();
  rc += test_popcntd ();
  rc += test_vrld ();
  rc += test_vsld ();
  rc += test_vsrd ();
  rc += test_vsrad ();
  rc += test_muleud ();
  rc += test_muloud ();
  rc += test_mulhud ();
  rc += test_muludm ();
  rc += test_cmpud ();
  rc += test_cmpud_all ();
  rc += test_cmpud_any ();
  rc += test_cmpsd ();
  rc += test_cmpsd_all ();
  rc += test_cmpsd_any ();
  rc += test_maxud ();
  rc += test_maxsd ();
  rc += test_minud ();
  rc += test_minsd ();
  rc += test_vmuleud ();
  rc += test_vmuloud ();
  rc += test_sradi ();
  rc += test_srdi ();
  rc += test_sldi ();
  rc += test_rldi ();
  rc += test_vmaddeud ();
  rc += test_vmaddoud ();
  rc += test_lvgudx ();
  rc += test_stvgudx ();
  rc += test_setbd ();
  rc += test_splatisd ();
  rc += test_splatiud ();
  rc += test_vec_divide_dw ();
  rc += test_vec_modulo_dw ();
  rc += test_vec_divide_qud ();

  return (rc);
}
