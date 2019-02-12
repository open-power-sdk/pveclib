/*
 * vec_bcd_dummy.c
 *
 *  Created on: Mar 1, 2018
 *      Author: sjmunroe
 */

#include <vec_common_ppc.h>
#include <vec_bcd_ppc.h>

vf64_t
test_pack_Decimal128 (_Decimal128 lval)
{
  return vec_pack_Decimal128 (lval);
}

vui32_t
test_pack_Decimal128_cast (_Decimal128 lval)
{
  return (vui32_t) vec_pack_Decimal128 (lval);
}

_Decimal128
test_unpack_Decimal128 (vf64_t lval)
{
  return vec_unpack_Decimal128 (lval);
}

_Decimal128
test_unpack_Decimal128_cast (vui32_t lval)
{
  return vec_unpack_Decimal128 ((vf64_t) lval);
}

_Decimal128
test_vec_BCD2DFP (vui32_t val)
{
  return vec_BCD2DFP (val);
}

vui32_t
test_vec_DFP2BCD (_Decimal128 val)
{
  return vec_DFP2BCD (val);
}

vui8_t
test_vec_bcdctb100s (vui8_t a)
{
  return (vec_rdxct100b (a));
}

vui32_t
test_vec_bcdadd (vui32_t a, vui32_t b)
{
  return vec_bcdadd (a, b);
}

vui32_t
test_vec_bcdsub (vui32_t a, vui32_t b)
{
  return vec_bcdsub (a, b);
}

vui32_t
test_vec_bcdmul (vui32_t a, vui32_t b)
{
  return vec_bcdmul (a, b);
}

vui32_t
test_vec_bcddiv (vui32_t a, vui32_t b)
{
  return vec_bcddiv (a, b);
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
example_vec_bcdctuq_loop (vui128_t *out, vui8_t* in, int cnt)
{
  vui8_t d100;
  vui16_t d10k;
  vui32_t d100m;
  vui64_t d10e;
  vui128_t result;
  int i;

  for (i = 0; i < cnt; i++)
    {
      d100 = vec_rdxct100b (*in++);
      d10k = vec_rdxct10kh (d100);
      d100m = vec_rdxct100mw (d10k);
      d10e = vec_rdxct10E16d (d100m);
      *out++ = vec_rdxct10e32q (d10e);
    }
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

vi128_t
test__builtin_bcdmax (vi128_t vra, vi128_t vrb)
{
  vi128_t result = vra;
  if (__builtin_bcdsub_lt (vra, vrb, 0))
    result = vrb;

  return result;
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
#endif
#endif
