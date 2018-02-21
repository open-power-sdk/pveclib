/*
 Copyright [2017] IBM Corporation.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 vec_int128_dummy.c

 Contributors:
      IBM Corporation, Steven Munroe
      Created on: May 9, 2017
 */

#include <vec_int128_ppc.h>

vui128_t
test_vec_slqi_0 (vui128_t __A)
{
  return vec_slqi (__A, 0);
}

vui128_t
test_vec_slqi_4 (vui128_t __A)
{
  return vec_slqi (__A, 4);
}

vui128_t
test_vec_slqi_7 (vui128_t __A)
{
  return vec_slqi (__A, 7);
}

vui128_t
test_vec_slqi_8 (vui128_t __A)
{
  return vec_slqi (__A, 8);
}

vui128_t
test_vec_slqi_14 (vui128_t __A)
{
  return vec_slqi (__A, 14);
}

vui128_t
test_vec_slqi_16 (vui128_t __A)
{
  return vec_slqi (__A, 16);
}
#if 1

vui128_t
test_vec_slqi_17 (vui128_t __A)
{
  return vec_slqi (__A, 17);
}
vui128_t
test_vec_slqi_31 (vui128_t __A)
{
  return vec_slqi (__A, 31);
}

vui128_t
test_vec_slqi_48 (vui128_t __A)
{
  return vec_slqi (__A, 48);
}

vui128_t
test_vec_slqi_120 (vui128_t __A)
{
  return vec_slqi (__A, 120);
}

vui128_t
test_vec_slqi_128 (vui128_t __A)
{
  return vec_slqi (__A, 128);
}

vui128_t
test_vec_slqi_129 (vui128_t __A)
{
  return vec_slqi (__A, 129);
}
#endif

vui128_t
test_vec_srqi_0 (vui128_t __A)
{
  return vec_srqi (__A, 0);
}

vui128_t
test_vec_srqi_4 (vui128_t __A)
{
  return vec_srqi (__A, 4);
}

vui128_t
test_vec_srqi_7 (vui128_t __A)
{
  return vec_srqi (__A, 7);
}

vui128_t
test_vec_srqi_8 (vui128_t __A)
{
  return vec_srqi (__A, 8);
}

vui128_t
test_vec_srqi_14 (vui128_t __A)
{
  return vec_srqi (__A, 14);
}

vui128_t
test_vec_srqi_16 (vui128_t __A)
{
  return vec_srqi (__A, 16);
}
#if 1
vui128_t
test_vec_srqi_31 (vui128_t __A)
{
  return vec_srqi (__A, 31);
}

vui128_t
test_vec_srqi_48 (vui128_t __A)
{
  return vec_srqi (__A, 48);
}

vui128_t
test_vec_srqi_120 (vui128_t __A)
{
  return vec_srqi (__A, 120);
}

vui128_t
test_vec_srqi_128 (vui128_t __A)
{
  return vec_srqi (__A, 128);
}

vui128_t
test_vec_srqi_129 (vui128_t __A)
{
  return vec_srqi (__A, 129);
}
#endif

vui128_t
test_vsl4 (vui128_t a)
{
	return (vec_slq4(a));
}

vui128_t
test_vsr4 (vui128_t a)
{
	return (vec_srq4(a));
}

vui128_t
test_vec_sldq (vui128_t a, vui128_t b, vui128_t sh)
{
  return (vec_sldq (a, b, sh));
}

vui128_t
test_vec_srq  (vui128_t a, vui128_t sh)
{
  return (vec_srq (a, sh));
}

vui128_t
test_vec_slq  (vui128_t a, vui128_t sh)
{
  return (vec_slq (a, sh));
}

vui128_t
test_vec_adduqm (vui128_t a, vui128_t b)
{
  return (vec_adduqm (a, b));
}

vui128_t
test_vec_addcuq (vui128_t a, vui128_t b)
{
  return (vec_addcuq (a, b));
}

vui128_t
test_vec_addcq (vui128_t *cout, vui128_t a, vui128_t b)
{
  return (vec_addcq (cout, a, b));
}

vui128_t
test_vec_addeuqm (vui128_t a, vui128_t b, vui128_t c)
{
  return (vec_addeuqm (a, b, c));
}

vui128_t
test_vec_addecuq (vui128_t a, vui128_t b, vui128_t c)
{
  return (vec_addecuq (a, b, c));
}

vui128_t
test_vec_addeq (vui128_t *cout, vui128_t a, vui128_t b, vui128_t c)
{
  return (vec_addeq (cout, a, b, c));
}

vui128_t
test_muloud (vui64_t a, vui64_t b)
{
  return (vec_muloud (a, b));
}

vui128_t
test_muleud (vui64_t a, vui64_t b)
{
  return (vec_muleud (a, b));
}

vui128_t
test_muludq (vui128_t *mulu, vui128_t a, vui128_t b)
{
  return (vec_muludq (mulu, a, b));
}

void
test_mul4uq (vui128_t *__restrict__ mulu, vui128_t m1h, vui128_t m1l, vui128_t m2h, vui128_t m2l)
{
  vui128_t mc, mp, mq;
  vui128_t mphh, mphl, mplh, mpll;
  mpll = vec_muludq (&mplh, m1l, m2l);
  mp = vec_muludq (&mphl, m1h, m2l);
  mplh = vec_addcq (&mc, mplh, mp);
  mphl = vec_addcuq (mphl, mc);
  mp = vec_muludq (&mc, m2h, m1l);
  mplh = vec_addcq (&mq, mplh, mp);
  mphl = vec_addcq (&mc, mphl, mq);
  mp = vec_muludq (&mphh, m2h, m1h);
  mplh = vec_addcq (&mc, mplh, mp);
  mphl = vec_addcuq (mphh, mc);

  mulu[0] = mpll;
  mulu[1] = mplh;
  mulu[2] = mphl;
  mulu[3] = mphh;
}
