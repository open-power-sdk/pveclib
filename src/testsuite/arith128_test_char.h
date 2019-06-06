/*
   arith128_test_char.h
  
    Created on: Oct 25, 2017
        Author: sjmunroe
  */

#ifndef TESTSUITE_ARITH128_TEST_CHAR_H_
#define TESTSUITE_ARITH128_TEST_CHAR_H_

#include <pveclib/vec_char_ppc.h>

#ifdef __DEBUG_PRINT__
extern vui8_t
db_vec_tolower (vui8_t vec_str);

extern vui8_t
db_vec_toupper (vui8_t vec_str);

extern vui8_t
db_vec_isalpha (vui8_t vec_str);
#endif

extern int test_vec_ischar (void);

extern int test_48 (void);

extern int test_vec_char (void);

#endif /* TESTSUITE_ARITH128_TEST_CHAR_H_ */
