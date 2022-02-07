/*
 Copyright (c) [2017] IBM Corporation.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 arith128_print.h

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: May 10, 2015
 */

#ifndef TESTSUITE_ARITH128_PRINT_H_
#define TESTSUITE_ARITH128_PRINT_H_

#include "arith128.h"
#include <pveclib/vec_f128_ppc.h>
#include <pveclib/vec_int512_ppc.h>

static inline
double
TimeDeltaSec (uint64_t tb_delta)
{
	double temp,  result;

	temp = tb_delta;
	result = temp / 512.e+06;

	return (result);
}

extern long tcount;

#ifndef PVECLIB_DISABLE_DFP
extern void
print_dfp128 (char *prefix, _Decimal128 val128);

extern void
print_dfp128e (char *prefix, _Decimal128 val128);

extern int
check_dfp128 (char *prefix, _Decimal128 val128, _Decimal128 shouldbe);

extern void
print_dfp64 (char *prefix, _Decimal64 val64);

extern int
check_dfp64 (char *prefix, _Decimal64 val64, _Decimal64 shouldbe);

extern void
print_dfp32 (char *prefix, _Decimal32 val32);

extern int
check_dfp32 (char *prefix, _Decimal32 val32, _Decimal32 shouldbe);
#endif

extern void
print_ibm128 (char *prefix, long double ldbl);

extern void
print_ibm128x (char *prefix, long double ldbl);

extern void
print_uint128 (char *prefix, unsigned __int128 val128);

extern int
check_sdiv128_64 (__int128 numerator, int64_t divisor, int64_t exp_q,
                  int64_t exp_r);

extern int
check_sdiv128_64x (__int128 numerator, int64_t divisor, int64_t exp_q,
                   int64_t exp_r);

extern int
check_udiv128_64 (unsigned __int128 numerator, uint64_t divisor, uint64_t exp_q,
                  uint64_t exp_r);

extern void
print_int128 (char *prefix, __int128 val128);

extern void
print_int128x (char *prefix, unsigned __int128 val128);

extern void
print_int64 (char *prefix, long val64);

extern void
print_uint64 (char *prefix, unsigned long val64);

extern void
print_int64x (char *prefix, unsigned long val64);

extern void
print_v2int64 (char *prefix, vui64_t val128);

extern void
print_v2xint64 (char *prefix, vui64_t val128);

extern void
print_v4f32 (char *prefix, vf32_t val);

extern void
print_v4f32x (char *prefix, vf32_t val);

extern void
print_v4b32c (char *prefix, vb32_t val);

extern void
print_v4b32x (char *prefix, vb32_t val);

extern void
print_v2f64 (char *prefix, vf64_t val);

extern void
print_v2f64x (char *prefix, vf64_t val);

extern void
print_v2b64c (char *prefix, vb64_t val);

extern void
print_v2b64x (char *prefix, vb64_t val);

extern void
print_vb128c (char *prefix, vb128_t boolval);

extern void
print_vb128x (char *prefix, vb128_t boolval);

extern int
check_udiv128_64x (unsigned __int128 numerator, uint64_t divisor,
                   uint64_t exp_q, uint64_t exp_r);

extern void
print_int128_sum (char *prefix, unsigned __int128 r, unsigned __int128 a,
                  unsigned __int128 b);

extern void
print_int128_carry (char *prefix, unsigned __int128 r, unsigned __int128 a,
                    unsigned __int128 b, unsigned __int128 c);

extern void
print_vint128 (char *prefix, vui128_t val);

extern void
print_vint128s (char *prefix, vi128_t val128);

extern void
print_vint128x (char *prefix, vui128_t val);

extern void
print_vfloat128x (char *prefix, __binary128 val128);

extern void
print_vint256 (char *prefix, vui128_t val0, vui128_t val1);

extern void
print_vint384 (char *prefix, vui128_t val0, vui128_t val1, vui128_t val2);

extern void
print_vint128_sum (char *prefix, vui32_t r, vui32_t a, vui32_t b);

extern void
print_vint128x_sum (char *prefix, vui32_t r, vui32_t a, vui32_t b);

extern void
print_vint128_carry (char *prefix,
    vui32_t r, vui32_t a, vui32_t b, vui32_t c);

extern void
print_vint128x_carry (char *prefix,
    vui32_t r, vui32_t a, vui32_t b, vui32_t c);

extern void
print_vint128_prod (char *prefix,
    vui32_t r, vui32_t a, vui32_t b, vui32_t c);

extern void
print_vint512x (char *prefix, __VEC_U_512 r);

extern void
print_vint640x (char *prefix, __VEC_U_640 r);

extern void
print_vint512x_prod (char *prefix, __VEC_U_512 r, __VEC_U_256 a, __VEC_U_256 b);

extern void
print_vint256x_prod (char *prefix, __VEC_U_256 r, vui128_t a, vui128_t b);

extern void
print_vint256_prod (char *prefix, __VEC_U_256 r, vui128_t a, vui128_t b);

extern void
print_vint128_extend (char *prefix,
    vui32_t r, vui32_t co, vui32_t a, vui32_t b, vui32_t c);

extern void
print_vint128x_extend (char *prefix,
    vui32_t r, vui32_t co, vui32_t a, vui32_t b, vui32_t c);

extern void
print_vint32x (char *prefix, vui32_t val);

extern void
print_vint32d (char *prefix, vui32_t val);

extern void
print_vint16x (char *prefix, vui16_t val);

extern void
print_vint16d (char *prefix, vui16_t val);

extern void
print_vint8 (char *prefix, vui8_t val);

extern void
print_vint8c (char *prefix, vui8_t val);

extern void
print_vint8x (char *prefix, vui8_t val);

extern void
print_vint8d (char *prefix, vui8_t val);

extern void
print_vchar (char *prefix, vui8_t val);

extern void
print_vbool8 (char *prefix, vui8_t val);

extern int
check_isf128_priv (char *prefix, __binary128 val128, int val, int shouldbe);

static inline int
check_isf128 (char *prefix, __binary128 val128, int val, int shouldbe)
{
  int rc = 0;
  if (__builtin_expect ((val != shouldbe), 0))
    {
      rc = check_isf128_priv (prefix, val128, val, shouldbe);
    }

  return (rc);
}

extern int
check_f128bool_priv (char *prefix, __binary128 val128, vb128_t boolis,
                     vb128_t shouldbe);

static inline int
check_f128bool (char *prefix, __binary128 val128, vb128_t boolis,
                vb128_t shouldbe)
{
  int rc = 0;
  if (!vec_all_eq((vui32_t)boolis, (vui32_t)shouldbe))
    {
      rc = check_f128bool_priv (prefix, val128, boolis, shouldbe);
    }

  return (rc);
}

static inline int
check_f128 (char *prefix, __binary128 val128, __binary128 f128is,
            __binary128 shouldbe)
{
  __VF_128 xfer;
  vb128_t boolis, boolshould;
  int rc = 0;

  xfer.vf1 = f128is;
  boolis = xfer.vbool1;
  xfer.vf1 = shouldbe;
  boolshould = xfer.vbool1;

  if (!vec_all_eq((vui32_t)boolis, (vui32_t)boolshould))
    {
      rc = check_f128bool_priv (prefix, val128, boolis, boolshould);
    }

  return (rc);
}

extern int
check_uint128_priv (char *prefix, unsigned __int128 val128,
                    unsigned __int128 shouldbe);

extern int
check_int128_priv (char *prefix, __int128 val128, __int128 shouldbe);

extern int
check_vui8_priv (char *prefix, vui8_t val128, vui8_t shouldbe);

extern int
check_v4b32c_priv (char *prefix, vb32_t val128, vb32_t shouldbe);

extern int
check_v4b32x_priv (char *prefix, vb32_t val128, vb32_t shouldbe);

extern int
check_v4f32_priv (char *prefix, vf32_t val128, vf32_t shouldbe);

extern int
check_v4f32x_priv (char *prefix, vf32_t val128, vf32_t shouldbe);

extern int
check_v2b64c_priv (char *prefix, vb64_t val128, vb64_t shouldbe);

extern int
check_v2b64x_priv (char *prefix, vb64_t val128, vb64_t shouldbe);

extern int
check_v2ui64x_priv (char *prefix, vui64_t val128, vui64_t shouldbe);

extern int
check_v2f64_priv (char *prefix, vf64_t val128, vf64_t shouldbe);

extern int
check_v2f64x_priv (char *prefix, vf64_t val128, vf64_t shouldbe);

extern int
check_vb128c_priv (char *prefix, vb128_t val128, vb128_t shouldbe);

extern int
check_vb128x_priv (char *prefix, vb128_t val128, vb128_t shouldbe);

extern int
check_vuint128_priv (char *prefix, vui128_t val128, vui128_t shouldbe);

extern int
check_vuint128x_priv (char *prefix, vui128_t val128, vui128_t shouldbe);

extern int
check_vuint128c_priv (char *prefix, vui128_t val128, vui128_t carry128,
                      vui128_t shouldbe, vui128_t shouldcarry);

extern int
check_vint256_priv (char *prefix, vui128_t val0_128, vui128_t val1_128,
                    vui128_t sb0_128, vui128_t sb1_128);

extern int
check_vint384_priv (char *prefix, vui128_t val0_128, vui128_t val1_128,
                    vui128_t val2_128, vui128_t sb0_128, vui128_t sb1_128,
                    vui128_t sb2_128);

extern int
check_vint512_priv (char *prefix, __VEC_U_512 val_is, __VEC_U_512 val_sb);

static inline int
check_vint384 (char *prefix, vui128_t val0_128, vui128_t val1_128,
               vui128_t val2_128, vui128_t sb0_128, vui128_t sb1_128,
               vui128_t sb2_128)
{
  int rc = 0;
  if (vec_any_ne ((vui32_t)val0_128, (vui32_t)sb0_128)
  ||  vec_any_ne ((vui32_t)val1_128, (vui32_t)sb1_128)
  ||  vec_any_ne ((vui32_t)val2_128, (vui32_t)sb2_128))
    {
      rc = check_vint384_priv (prefix, val0_128, val1_128, val2_128, sb0_128,
                               sb1_128, sb2_128);
    }

  return (rc);
}

static inline int
check_vint256 (char *prefix, vui128_t val0_128, vui128_t val1_128,
               vui128_t sb0_128, vui128_t sb1_128)
{
  int rc = 0;
  if (vec_any_ne((vui32_t)val0_128, (vui32_t)sb0_128)
  ||  vec_any_ne ((vui32_t)val1_128, (vui32_t)sb1_128))
    {
      rc = check_vint256_priv (prefix, val0_128, val1_128, sb0_128, sb1_128);
    }

  return (rc);
}

static inline int
check_vint512 (char *prefix, __VEC_U_512 val_is, __VEC_U_512 val_sb)
{
  int rc = 0;
  if (vec_any_ne ((vui32_t) val_is.vx0, (vui32_t) val_sb.vx0)
   || vec_any_ne ((vui32_t) val_is.vx1, (vui32_t) val_sb.vx1)
   || vec_any_ne ((vui32_t) val_is.vx2, (vui32_t) val_sb.vx2)
   || vec_any_ne ((vui32_t) val_is.vx3, (vui32_t) val_sb.vx3))
    {
      rc = check_vint512_priv (prefix, val_is, val_sb);
    }

  return (rc);
}

static inline int
check_vuint128c (char *prefix, vui128_t val128, vui128_t carry128,
                 vui128_t shouldbe, vui128_t shouldcarry)
{
  int rc = 0;
  if (vec_any_ne((vui32_t )val128, (vui32_t )shouldbe)
   || vec_any_ne ((vui32_t)carry128, (vui32_t)shouldcarry))
    {
      rc = check_vuint128c_priv (prefix, val128, carry128, shouldbe, shouldcarry);
    }

  return (rc);
}

static inline int
check_vui8 (char *prefix, vui8_t val128, vui8_t shouldbe)
{
  int rc = 0;
  if (vec_any_ne((vui8_t )val128, (vui8_t )shouldbe))
    {
      rc = check_vui8_priv (prefix, val128, shouldbe);
    }

  return (rc);
}

static inline int
check_v4b32c (char *prefix, vb32_t val128, vb32_t shouldbe)
{
  int rc = 0;
  if (vec_any_ne((vui32_t )val128, (vui32_t )shouldbe))
    {
      rc = check_v4b32c_priv (prefix, val128, shouldbe);
    }

  return (rc);

}

static inline int
check_v4b32x (char *prefix, vb32_t val128, vb32_t shouldbe)
{
  int rc = 0;
  if (vec_any_ne((vui32_t )val128, (vui32_t )shouldbe))
    {
      rc = check_v4b32x_priv (prefix, val128, shouldbe);
    }

  return (rc);

}

static inline int
check_v4f32 (char *prefix, vf32_t val128, vf32_t shouldbe)
{
  int rc = 0;
  if (vec_any_ne((vui32_t )val128, (vui32_t )shouldbe))
    {
      rc = check_v4f32_priv (prefix, val128, shouldbe);
    }

  return (rc);

}

static inline int
check_v4f32x (char *prefix, vf32_t val128, vf32_t shouldbe)
{
  int rc = 0;
  if (vec_any_ne((vui32_t )val128, (vui32_t )shouldbe))
    {
      rc = check_v4f32x_priv (prefix, val128, shouldbe);
    }

  return (rc);

}

static inline int
check_v2b64c (char *prefix, vb64_t val128, vb64_t shouldbe)
{
  int rc = 0;
  if (vec_any_ne((vui32_t)val128, (vui32_t)shouldbe))
    {
      rc = check_v2b64c_priv (prefix, val128, shouldbe);
    }

  return (rc);

}

static inline int
check_v2b64x (char *prefix, vb64_t val128, vb64_t shouldbe)
{
  int rc = 0;
  if (vec_cmpud_any_ne((vui64_t )val128, (vui64_t )shouldbe))
    {
      rc = check_v2b64x_priv (prefix, val128, shouldbe);
    }

  return (rc);

}

static inline int
check_v2ui64x (char *prefix, vui64_t val128, vui64_t shouldbe)
{
  int rc = 0;
  if (vec_cmpud_any_ne((vui64_t )val128, (vui64_t )shouldbe))
    {
      rc = check_v2ui64x_priv (prefix, val128, shouldbe);
    }

  return (rc);

}

static inline int
check_v2f64 (char *prefix, vf64_t val128, vf64_t shouldbe)
{
  int rc = 0;
  if (vec_any_ne((vui32_t)val128, (vui32_t)shouldbe))
    {
      rc = check_v2f64_priv (prefix, val128, shouldbe);
    }

  return (rc);

}

static inline int
check_v2f64x (char *prefix, vf64_t val128, vf64_t shouldbe)
{
  int rc = 0;
  if (vec_any_ne((vui32_t)val128, (vui32_t)shouldbe))
    {
      rc = check_v2f64x_priv (prefix, val128, shouldbe);
    }

  return (rc);

}

static inline int
check_vb128c (char *prefix, vb128_t val128, vb128_t shouldbe)
{
  int rc = 0;
  if (vec_any_ne((vui32_t)val128, (vui32_t)shouldbe))
    {
      rc = check_vb128c_priv (prefix, val128, shouldbe);
    }

  return (rc);

}

static inline int
check_vb128x (char *prefix, vb128_t val128, vb128_t shouldbe)
{
  int rc = 0;
  if (vec_any_ne((vui32_t )val128, (vui32_t )shouldbe))
    {
      rc = check_vb128x_priv (prefix, val128, shouldbe);
    }

  return (rc);

}

static inline int
check_vuint128 (char *prefix, vui128_t val128, vui128_t shouldbe)
{
  int rc = 0;
  if (vec_any_ne((vui32_t )val128, (vui32_t )shouldbe))
    {
      rc = check_vuint128_priv (prefix, val128, shouldbe);
    }

  return (rc);
}

static inline int
check_vuint128x (char *prefix, vui128_t val128, vui128_t shouldbe)
{
  int rc = 0;
  if (vec_any_ne((vui32_t )val128, (vui32_t )shouldbe))
    {
      rc = check_vuint128x_priv (prefix, val128, shouldbe);
    }

  return (rc);
}

static inline int
check_uint128 (char *prefix, unsigned __int128 val128,
               unsigned __int128 shouldbe)
{
  int rc = 0;
  if (val128 != shouldbe)
    {
      rc = check_uint128_priv (prefix, val128, shouldbe);
    }

  return (rc);
}

static inline int
check_int128 (char *prefix, __int128 val128, __int128 shouldbe)
{
  int rc = 0;
  if (val128 != shouldbe)
    {
      rc = check_int128_priv (prefix, val128, shouldbe);
    }

  return (rc);
}
extern int
check_uint64 (char *prefix, unsigned long val64, unsigned long shouldbe);

extern int
check_int64 (char *prefix, long val64, long shouldbe);

#ifndef PVECLIB_DISABLE_DFP
extern int
check_frexptftd_priv (char *prefix, _Decimal128 val128, long exp,
                      _Decimal128 shouldbe, long shouldexp);

static inline int
check_frexptftd (char *prefix, _Decimal128 val128, long exp,
                 _Decimal128 shouldbe, long shouldexp)
{
  int rc = 0;
  if ((val128 != shouldbe) || (exp != shouldexp))
    {
      rc = check_frexptftd_priv (prefix, val128, exp, shouldbe, shouldexp);
    }

  return (rc);
}

extern int
check_frexptfkf_priv (char *prefix, vui128_t val128, long exp,
                      vui128_t shouldbe, long shouldexp);

static inline int
check_frexptfkf (char *prefix, vui128_t val128, long exp, vui128_t shouldbe,
                 long shouldexp)
{
  int rc = 0;
  if ((!vec_all_eq((vui32_t )val128, (vui32_t )shouldbe)) || (exp != shouldexp))
    {
      rc = check_frexptfkf_priv (prefix, val128, exp, shouldbe, shouldexp);
    }

  return (rc);
}
#endif

extern int
check_kf_priv (char *prefix, vui128_t val128, vui128_t shouldbe);

static inline int
check_kf (char *prefix, vui128_t val128, vui128_t shouldbe)
{
  int rc = 0;
  if (!vec_all_eq((vui32_t )val128, (vui32_t )shouldbe))
    {
      rc = check_kf_priv (prefix, val128, shouldbe);
    }

  return (rc);
}

extern int
check_frexptfvti_priv (char *prefix, vui128_t val128, long exp,
                       vui128_t shouldbe, long shouldexp);

extern int
check_frexptfti_priv (char *prefix, __int128 val128, long exp,
                      __int128 shouldbe, long shouldexp);

static inline int
check_frexptfvti (char *prefix, vui128_t val128, long exp, vui128_t shouldbe,
                  long shouldexp)
{
  int rc = 0;
  if ((!vec_all_eq((vui32_t )val128, (vui32_t )shouldbe)) || (exp != shouldexp))
    {
      rc = check_frexptfvti_priv (prefix, val128, exp, shouldbe, shouldexp);
    }

  return (rc);
}

static inline int
check_frexptfti (char *prefix, __int128 val128, long exp, __int128 shouldbe,
                 long shouldexp)
{
  int rc = 0;
  if ((val128 != shouldbe) || (exp != shouldexp))
    {
      rc = check_frexptfti_priv (prefix, val128, exp, shouldbe, shouldexp);
    }

  return (rc);
}

#ifndef PVECLIB_DISABLE_DFP
extern void
print_dfp128p2 (char *prefix, _Decimal128 val128, long exp);
#endif

#endif /* TESTSUITE_ARITH128_PRINT_H_ */

