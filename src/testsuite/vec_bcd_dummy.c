/*
 * vec_bcd_dummy.c
 *
 *  Created on: Mar 1, 2018
 *      Author: sjmunroe
 */

#include <vec_common_ppc.h>
#include <vec_bcd_ppc.h>

vui8_t
test_vec_bcdctb100s (vui8_t a)
{
	return (vec_bcdctb100s(a));
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
