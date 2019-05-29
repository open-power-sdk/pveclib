/*
 * vec_bcd_dummy.c
 *
 *  Created on: Mar 1, 2018
 *      Author: sjmunroe
 */

#include <pveclib/vec_common_ppc.h>
#include <pveclib/vec_bcd_ppc.h>

vf64_t
test_pack_Decimal128 (_Decimal128 lval)
{
  return vec_pack_Decimal128 (lval);
}

vBCD_t
test_pack_Decimal128_cast (_Decimal128 lval)
{
  return (vBCD_t) vec_pack_Decimal128 (lval);
}

_Decimal128
test_unpack_Decimal128 (vf64_t lval)
{
  return vec_unpack_Decimal128 (lval);
}

_Decimal128
test_unpack_Decimal128_cast (vBCD_t lval)
{
  return vec_unpack_Decimal128 ((vf64_t) lval);
}

vui64_t
test_vec_BCD2BIN (vBCD_t val)
{
  return vec_BCD2BIN (val);
}

_Decimal128
test_vec_BCD2DFP (vBCD_t val)
{
  return vec_BCD2DFP (val);
}

vBCD_t
test_vec_DFP2BCD (_Decimal128 val)
{
  return vec_DFP2BCD (val);
}

vui8_t
test_vec_bcdctb100s (vui8_t a)
{
  return (vec_rdxct100b (a));
}

vBCD_t
test_vec_bcdadd (vBCD_t a, vBCD_t b)
{
  return vec_bcdadd (a, b);
}

vBCD_t
test_vec_bcdaddcsq (vBCD_t a, vBCD_t b)
{
  return vec_bcdaddcsq (a, b);
}

vBCD_t
test_vec_cbcdaddcsq (vBCD_t *c, vBCD_t a, vBCD_t b)
{
  return vec_cbcdaddcsq (c, a, b);
}

vBCD_t
test_vec_cbcdaddcsq_2 (vBCD_t *c, vBCD_t a, vBCD_t b)
{
  *c = vec_bcdaddcsq (a, b);
  return vec_bcdadd (a, b);
}

vBCD_t
test_vec_bcdaddesqm (vBCD_t a, vBCD_t b, vBCD_t c)
{
  return vec_bcdaddesqm (a, b, c);
}

vBCD_t
test_vec_bcdaddecsq (vBCD_t a, vBCD_t b, vBCD_t c)
{
  return vec_bcdaddecsq (a, b, c);
}

vBCD_t
test_vec_cbcdaddecsq (vBCD_t *co, vBCD_t a, vBCD_t b, vBCD_t c)
{
  return vec_cbcdaddecsq (co, a, b, c);
}

vBCD_t
test_vec_cbcdaddecsq_2 (vBCD_t *co, vBCD_t a, vBCD_t b, vBCD_t c)
{
  *co = vec_bcdaddecsq (a, b, c);
  return vec_bcdaddesqm (a, b, c);
}

vBCD_t
test_vec_bcdsub (vBCD_t a, vBCD_t b)
{
  return vec_bcdsub (a, b);
}

vBCD_t
test_vec_bcdsubcsq (vBCD_t a, vBCD_t b)
{
  return vec_bcdaddcsq (a, b);
}

vBCD_t
test_vec_bcdsubesqm (vBCD_t a, vBCD_t b, vBCD_t c)
{
  return vec_bcdsubesqm (a, b, c);
}

vBCD_t
test_vec_bcdsubecsq (vBCD_t a, vBCD_t b, vBCD_t c)
{
  return vec_bcdsubecsq (a, b, c);
}

vBCD_t
test_vec_cbcdsubecsq (vBCD_t *co, vBCD_t a, vBCD_t b, vBCD_t c)
{
  *co = vec_bcdsubecsq (a, b, c);
  return vec_bcdsubesqm (a, b, c);
}

vBCD_t
test_vec_bcdmulh (vBCD_t a, vBCD_t b)
{
  return vec_bcdmulh (a, b);
}

vBCD_t
test_vec_bcdmul (vBCD_t a, vBCD_t b)
{
  return vec_bcdmul (a, b);
}

vBCD_t
test_vec_cbcdmul (vBCD_t *p, vBCD_t a, vBCD_t b)
{
  return vec_cbcdmul (p, a, b);
}

vBCD_t
test_vec_cbcdmul_2 (vBCD_t *p, vBCD_t a, vBCD_t b)
{
  *p = vec_bcdmulh (a, b);
  return vec_bcdmul (a, b);
}

vBCD_t
test_vec_bcddiv (vBCD_t a, vBCD_t b)
{
  return vec_bcddiv (a, b);
}

vi128_t
test_vec_bcdctsq (vBCD_t a)
{
  return (vec_bcdctsq (a));
}

vui128_t
test_vec_bcdctuq (vBCD_t a)
{
  return (vec_bcdctuq (a));
}

vui64_t
test_vec_bcdctud (vBCD_t a)
{
  return (vec_bcdctud (a));
}

vui32_t
test_vec_bcdctuw (vBCD_t a)
{
  return (vec_bcdctuw (a));
}

vui16_t
test_vec_bcdctuh (vBCD_t a)
{
  return (vec_bcdctuh (a));
}

vui8_t
test_vec_bcdctub (vBCD_t a)
{
  return (vec_bcdctub (a));
}

vBCD_t
test_vec_bcdcpsgn (vBCD_t a, vBCD_t b)
{
  return vec_bcdcpsgn (a, b);
}

vBCD_t
test_vec_bcdsetsgn (vBCD_t a)
{
  return vec_bcdsetsgn (a);
}

vBCD_t
test_vec_bcdcfz (vui8_t vrb)
{
  return vec_bcdcfz (vrb);
}

vui8_t
test_vec_bcdctz (vBCD_t vrb)
{
  return vec_bcdctz (vrb);
}

vBCD_t
test_vec_bcds (vBCD_t vra, vi8_t vrb)
{
  return vec_bcds (vra, vrb);
}

vBCD_t
test_vec_bcdsr (vBCD_t vra, vi8_t vrb)
{
  return vec_bcdsr (vra, vrb);
}

vBCD_t
test_vec_bcdsrrqi (vBCD_t vra)
{
  return vec_bcdsrrqi (vra, 15);
}

vBCD_t
test_vec_bcdtrunc (vBCD_t vra, vui16_t vrb)
{
  return vec_bcdtrunc (vra, vrb);
}

vBCD_t
test_vec_bcdtruncqi (vBCD_t vra)
{
  return vec_bcdtruncqi (vra, 15);
}

vBCD_t
test_vec_bcdutrunc (vBCD_t vra, vui16_t vrb)
{
  return vec_bcdutrunc (vra, vrb);
}

vBCD_t
test_vec_bcdutruncqi (vBCD_t vra)
{
  return vec_bcdutruncqi (vra, 15);
}

vBCD_t
test_vec_bcdus (vBCD_t vra, vi8_t vrb)
{
  return vec_bcdus (vra, vrb);
}

vBCD_t
test_vec_bcdslqi_1 (vBCD_t vra, const int _N)
{
  return vec_bcdslqi (vra, 1);
}

vBCD_t
test_vec_bcdslqi_2 (vBCD_t vra, const int _N)
{
  return vec_bcdslqi (vra, 2);
}

vBCD_t
test_vec_bcdslqi_3 (vBCD_t vra, const int _N)
{
  return vec_bcdslqi (vra, 3);
}

vBCD_t
test_vec_bcdslqi_15 (vBCD_t vra, const int _N)
{
  return vec_bcdslqi (vra, 15);
}

vBCD_t
test_vec_bcdsrqi_1 (vBCD_t vra, const int _N)
{
  return vec_bcdsrqi (vra, 1);
}

vBCD_t
test_vec_bcdsrqi_2 (vBCD_t vra, const int _N)
{
  return vec_bcdsrqi (vra, 2);
}

vBCD_t
test_vec_bcdsrqi_3 (vBCD_t vra, const int _N)
{
  return vec_bcdsrqi (vra, 3);
}

vBCD_t
test_vec_bcdsrqi_15 (vBCD_t vra, const int _N)
{
  return vec_bcdsrqi (vra, 15);
}

vBCD_t
test_vec_bcdsruqi_1 (vBCD_t vra, const int _N)
{
  return vec_bcdsrqi (vra, 1);
}

vBCD_t
test_vec_bcdsruqi_2 (vBCD_t vra, const int _N)
{
  return vec_bcdsruqi (vra, 2);
}

vBCD_t
test_vec_bcdsruqi_3 (vBCD_t vra, const int _N)
{
  return vec_bcdsruqi (vra, 3);
}

vBCD_t
test_vec_bcdsruqi_15 (vBCD_t vra, const int _N)
{
  return vec_bcdsruqi (vra, 15);
}

int
test_vec_signgbit_bcdsq (vBCD_t a)
{
  return vec_signbit_bcdsq (a);
}

vb128_t
test_vec_setbool_bcdsq (vBCD_t a)
{
  return vec_setbool_bcdsq (a);
}

vb128_t
test_vec_setbool_bcdinv (vBCD_t a)
{
  return vec_setbool_bcdinv (a);
}

vbBCD_t
test_vec_bcdcmp_eqsq (vBCD_t vra, vBCD_t vrb)
{
  return vec_bcdcmp_eqsq (vra, vrb);
}

vbBCD_t
test_vec_bcdcmp_nesq (vBCD_t vra, vBCD_t vrb)
{
  return vec_bcdcmp_nesq (vra, vrb);
}

vbBCD_t
test_vec_bcdcmp_gtsq (vBCD_t vra, vBCD_t vrb)
{
  return vec_bcdcmp_gtsq (vra, vrb);
}

vbBCD_t
test_vec_bcdcmp_gesq (vBCD_t vra, vBCD_t vrb)
{
  return vec_bcdcmp_gesq (vra, vrb);
}

vbBCD_t
test_vec_bcdcmp_ltsq (vBCD_t vra, vBCD_t vrb)
{
  return vec_bcdcmp_ltsq (vra, vrb);
}

vbBCD_t
test_vec_bcdcmp_lesq (vBCD_t vra, vBCD_t vrb)
{
  return vec_bcdcmp_lesq (vra, vrb);
}

int
test_vec_bcdcmpeq (vBCD_t vra, vBCD_t vrb)
{
  return vec_bcdcmpeq (vra, vrb);
}

int
test_vec_bcdcmpne (vBCD_t vra, vBCD_t vrb)
{
  return vec_bcdcmpne (vra, vrb);
}

int
test_vec_bcdcmpgt (vBCD_t vra, vBCD_t vrb)
{
  return vec_bcdcmpgt (vra, vrb);
}

int
test_vec_bcdcmpge (vBCD_t vra, vBCD_t vrb)
{
  return vec_bcdcmpge (vra, vrb);
}

int
test_vec_bcdcmplt (vBCD_t vra, vBCD_t vrb)
{
  return vec_bcdcmplt (vra, vrb);
}

int
test_vec_bcdcmple (vBCD_t vra, vBCD_t vrb)
{
  return vec_bcdcmple (vra, vrb);
}

vui128_t
example_vec_bcdctuq (vui8_t vra)
{
  vui8_t d100;
  vui16_t d10k;
  vui32_t d100m;
  vui64_t d10e;
  vui128_t result;

  d100 = vec_rdxct100b ((vui8_t) vra);
  d10k = vec_rdxct10kh (d100);
  d100m = vec_rdxct100mw (d10k);
  d10e = vec_rdxct10E16d (d100m);
  result = vec_rdxct10e32q (d10e);

  return result;
}

void
example_vec_bcdctuq_loop (vui128_t *out, vui8_t* in, long cnt)
{
  vui8_t d100;
  vui16_t d10k;
  vui32_t d100m;
  vui64_t d10e;
//  vui128_t result;
  long i;

  for (i = 0; i < cnt; i++)
    {
      d100 = vec_rdxct100b (*in++);
      d10k = vec_rdxct10kh (d100);
      d100m = vec_rdxct100mw (d10k);
      d10e = vec_rdxct10E16d (d100m);
      *out++ = vec_rdxct10e32q (d10e);
    }
}

void
example_vec_cbcdecsq_loop (vBCD_t *cout, vBCD_t* out, vBCD_t* a, vBCD_t* b, long cnt)
{
  vBCD_t c, cn;
  long i;

  out[cnt-1] = vec_cbcdaddcsq (&c, a[cnt-1], b[cnt-1]);

  cn = _BCD_CONST_ZERO;
  for (i = (cnt-2); i >= 0; i--)
    {
      out[i] = vec_cbcdaddecsq (&cn, a[i], b[i], c);
      c = cn;
    }
  *cout = cn;
}


void
example_bcdmul_2x2 (vBCD_t *__restrict__ mulu, vBCD_t m1h, vBCD_t m1l,
		    vBCD_t m2h, vBCD_t m2l)
{
  vBCD_t mc, mp, mq;
  vBCD_t mphh, mphl, mplh, mpll;
  mpll = vec_cbcdmul (&mplh, m1l, m2l);
  mp = vec_cbcdmul (&mphl, m1h, m2l);
  mplh = vec_bcdadd (mplh, mp);
  mc   = vec_bcdaddcsq (mplh, mp);
  mphl   = vec_bcdadd (mphl, mp);
  mp = vec_cbcdmul (&mc, m2h, m1l);
  mplh = vec_bcdadd (mplh, mp);
  mq   = vec_bcdaddcsq (mplh, mp);
  mphl = vec_bcdadd (mphl, mq);
  mc   = vec_bcdaddcsq (mplh, mq);
  mp = vec_cbcdmul (&mphh, m2h, m1h);
  mphl = vec_bcdadd (mphl, mp);
  mp   = vec_bcdaddcsq (mplh, mp);
  mphh = vec_bcdadd (mphh, mp);
  mphh = vec_bcdadd (mphh, mc);

  mulu[0] = mpll;
  mulu[1] = mplh;
  mulu[2] = mphl;
  mulu[3] = mphh;
}

// Convert extended quadword binary to BCD 32-digits at a time.
vBCD_t
example_longbcdcf_10e32 (vui128_t *q, vui128_t *d, long int _N)
{
  vui128_t dn, qh, ql, rh;
  long int i;

  // init step for the top digits
  dn = d[0];
  qh = vec_divuq_10e32 (dn);
  rh = vec_moduq_10e32 (dn, qh);
  q[0] = qh;

  // now we know the remainder is less than the divisor.
  for (i=1; i<_N; i++)
    {
      dn = d[i];
      ql = vec_divudq_10e32 (&qh, rh, dn);
      rh = vec_modudq_10e32 (rh, dn, &ql);
      q[i] = ql;
    }
  // convert to BCD and return the remainder for this step
  return vec_bcdcfuq (rh);
}

long int
example_longbcdct_10e32 (vui128_t *d, vBCD_t decimal, long int _C , long int _N)
{
  /* ten32  = +100000000000000000000000000000000UQ  */
  const vui128_t ten32 = (vui128_t)
	  { (__int128) 10000000000000000UL * (__int128) 10000000000000000UL };
  const vui128_t zero = (vui128_t) { (__int128) 0UL };
  vui128_t dn, ph, pl, cn, c;
  long int i, cnt;

  cnt = _C;

  dn = zero;
  cn  = zero;
  if ( cnt == 0 )
    {
      if (vec_cmpuq_all_ne ((vui128_t) decimal, zero))
	{
	  cnt++;
	  dn = vec_bcdctuq (decimal);
	}

      for ( i = 0; i < (_N - 1); i++ )
	{
	  d[i] = zero;
	}
      d[_N - cnt] = dn;
    }
  else
    {
      if (vec_cmpuq_all_ne ((vui128_t) decimal, zero))
	{
	  dn = vec_bcdctuq (decimal);
	}
      for ( i = (_N - 1); i >= (_N - cnt); i--)
	{
	  pl = vec_muludq (&ph, d[i], ten32);

	  c = vec_addecuq (pl, dn, cn);
	  d[i] = vec_addeuqm (pl, dn, cn);
	  cn = c;
	  dn = ph;
	}
      if (vec_cmpuq_all_ne (dn, zero) || vec_cmpuq_all_ne (cn, zero))
	{
	  cnt++;
	  dn = vec_adduqm (dn, cn);
	  d[_N - cnt] = dn;
	}
    }

  return cnt;
}

long int
example_longbcdct_10e31 (vui128_t *d, vBCD_t decimal, long int _C,
			      long int _N)
{
  /* ten32  = +100000000000000000000000000000000UQ  */
  const vui128_t ten32 = (vui128_t)
	  { (__int128) 10000000000000000UL * (__int128) 10000000000000000UL };
  const vui128_t zero = (vui128_t) { (__int128) 0UL };
  vui128_t dn, ph, pl, cn, c;
  long int i, cnt;

  cnt = _C;

  dn = zero;
  cn = zero;
  if (cnt == 0)
    {
      if (vec_cmpuq_all_ne ((vui128_t) decimal, zero))
	{
	  cnt++;
	  dn = vec_bcdctuq (decimal);
	}

      for (i = 0; i < (_N - 1); i++)
	{
	  d[i] = zero;
	}
      d[_N - cnt] = dn;
    }
  else
    {
      if (vec_cmpuq_all_ne ((vui128_t) decimal, zero))
	{
	  dn = vec_bcdctuq (decimal);
	}
      for ( i = (_N - 1); i >= (_N - cnt); i--)
	{
	  pl = vec_muludq (&ph, d[i], ten32);

	  c = vec_addecuq (pl, dn, cn);
	  d[i] = vec_addeuqm (pl, dn, cn);
	  cn = c;
	  dn = ph;
	}
      if (vec_cmpuq_all_ne (dn, zero) || vec_cmpuq_all_ne (cn, zero))
	{
	  cnt++;
	  dn = vec_adduqm (dn, cn);
	  d[_N - cnt] = dn;
	}
    }

  return cnt;
}

vui8_t
test_vec_rdxcf100b (vui8_t vra)
{
  return vec_rdxcf100b (vra);
}

vui8_t
test_vec_rdxcf10kh (vui16_t vra)
{
  return vec_rdxcf10kh (vra);
}

vui16_t
test_vec_rdxcf100mw (vui32_t vra)
{
  return vec_rdxcf100mw (vra);
}

vui32_t
test_vec_rdxcf10E16d (vui64_t vra)
{
  return vec_rdxcf10E16d (vra);
}

vui64_t
test_vec_rdxcf10e32q (vui128_t vra)
{
  return vec_rdxcf10e32q (vra);
}

vui8_t
test_vec_rdxct100b_0 (vui8_t vra)
{
  vui8_t x10, c10, high_digit, low_digit;
  // Isolate the low_digit
  low_digit = vec_slbi (vra, 4);
  low_digit = vec_srbi (low_digit, 4);
  // Shift the high digit into the units position
  high_digit = vec_srbi (vra, 4);
  // multiply the high digit by 10
  c10 = vec_splats ((unsigned char) 10);
#if (__GNUC__ > 7)
  x10 = vec_mul (high_digit, c10);
#else
  x10 = vec_mulubm (high_digit, c10);
#endif
  // add the low_digit to high_digit * 10.
  return vec_add (x10, low_digit);
}

vui8_t
test_vec_rdxct100b_1 (vui8_t vra)
{
  vui8_t x6, c6, high_digit;
  /* Compute the high digit correction factor. For BCD to binary 100s
   * this is the isolated high digit multiplied by the radix difference
   * in binary.  For this stage we use 0x10 - 10 = 6.  */
  high_digit = vec_srbi (vra, 4);
  c6 = vec_splats ((unsigned char) (16-10));
#if (__GNUC__ > 7)
  x6 = vec_mul (high_digit, c6);
#else
  x6 = vec_mulubm (high_digit, c6);
#endif
  /* Subtract the high digit correction bytes from the original
   * BCD bytes in binary.  This reduces byte range to 0-99. */
  return vec_sub (vra, x6);
}

vui8_t
test_vec_rdxct100b (vui8_t vra)
{
  return vec_rdxct100b (vra);
}

vui16_t
test_vec_rdxct10kh (vui8_t vra)
{
  return vec_rdxct10kh (vra);
}

vui64_t
test_vec_rdxct10E16d (vui32_t vra)
{
  return vec_rdxct10E16d (vra);
}

vui128_t
test_vec_rdxct10e32q (vui64_t vra)
{
  return vec_rdxct10e32q (vra);
}

vBCD_t
__test_vec_bcdcfsq (vi128_t vra)
{
  return vec_bcdcfsq (vra);
}

vBCD_t
__test_vec_bcdcfud (vui64_t vra)
{
  return vec_bcdcfud (vra);
}

vBCD_t
__test_vec_bcdcfuq (vui128_t vra)
{
  return vec_bcdcfuq (vra);
}

#if (__GNUC__ > 4)
_Decimal128
test__builtin_ddedpdq ( _Decimal128 value)
{
  return __builtin_ddedpdq (2, value);
}

_Decimal128
test__builtin_denbcdq ( _Decimal128 value)
{
  return __builtin_denbcdq (1, value);
}

#ifdef _ARCH_PWR8
vi128_t
test__builtin_bcdadd (vi128_t vra, vi128_t vrb)
{
  return __builtin_bcdadd (vra, vrb, 0);
}

int
test__builtin_bcdadd_eq (vi128_t vra, vi128_t vrb)
{
  return __builtin_bcdadd_eq (vra, vrb, 0);
}

vi128_t
test__builtin_bcdsub (vi128_t vra, vi128_t vrb)
{
  return __builtin_bcdsub (vra, vrb, 0);
}

int
test__builtin_bcdsub_eq (vi128_t vra, vi128_t vrb)
{
  return __builtin_bcdsub_eq (vra, vrb, 0);
}

int
test__builtin_bcdsub_lt (vi128_t vra, vi128_t vrb)
{
  return __builtin_bcdsub_lt (vra, vrb, 0);
}

vi128_t
test__builtin_bcdmax (vi128_t vra, vi128_t vrb)
{
  vi128_t result = vra;
  if (__builtin_bcdsub_lt (vra, vrb, 0))
    result = vrb;

  return result;
}

vi128_t
test__vec_bcdaddcsq (vi128_t a, vi128_t b)
{
  vi128_t t, c;
  c = (vi128_t) _BCD_CONST_ZERO;
  t = __builtin_bcdadd (a, b, 0);
  if (__builtin_expect (__builtin_bcdadd_ov ( a, b, 0), 0))
    {
      c = (vi128_t) vec_bcdcpsgn (_BCD_CONST_PLUS_ONE, (vBCD_t) t);
    }
  return (c);
}

vi128_t
test__builtin_bcdabsd (vi128_t vra, vi128_t vrb)
{
  vi128_t result;
  if (__builtin_bcdsub_gt (vra, vrb, 0))
    result = __builtin_bcdsub (vra, vrb, 0);
  else
    result = __builtin_bcdsub (vrb, vra, 0);

  return result;
}

vi128_t
test__builtin_bcdabs (vi128_t vra)
{
  vi128_t result = vra;

  if (__builtin_bcdadd_lt (vra, (vi128_t) _BCD_CONST_ZERO, 0))
    result = __builtin_bcdsub ((vi128_t) _BCD_CONST_ZERO, vra, 0);

  return result;
}
#endif
#endif
