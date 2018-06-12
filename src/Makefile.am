# bin_PROGRAMS=pveclib_test

lib_LTLIBRARIES = libpvec.la

libpvec_la_SOURCES = tipowof10.c decpowof2.c

EXTRA_DIST =

# This rule is necessary because pvec_test needs to statically link
# .libs/libpvec.a and if make is executed in parallel (-jN) the 
# pveclib_test rule may be run before .libs/libpvec.a is built.
.libs/libpvec.a: libpvec.la

# pveclib definitions
pveclibincludedir = $(includedir)/pveclib

# pveclib_test definitions
pveclib_testincludedir = $(includedir)/testsuite

# The headers in 'plibvec_HEADERS' are meant to be the ones installed
# on 'make install'
pveclibinclude_HEADERS = \
	vec_common_ppc.h \
	vec_int128_ppc.h \
	vec_int64_ppc.h \
	vec_int32_ppc.h \
	vec_int16_ppc.h \
	vec_char_ppc.h \
	vec_bcd_ppc.h

pveclib_la_INCLUDES = \
	$(pveclibinclude_HEADERS)

pveclib_test_la_INCLUDES = \
       $(pveclibinclude_HEADERS)

EXTRA_DIST += $(pveclib_la_INCLUDES)

# Test programs invoked with 'make check'
TESTS =

TESTS += pveclib_test
pveclib_test_SOURCES = \
	testsuite/pveclib_test.c \
	testsuite/arith128_print.c \
	testsuite/arith128_test_i128.c \
	testsuite/arith128_test_i64.c \
	testsuite/arith128_test_i32.c \
	testsuite/arith128_test_i16.c \
	testsuite/arith128_test_char.c \
	testsuite/arith128_test_bcd.c
	
pveclib_test_LDADD = .libs/libpvec.a

check_PROGRAMS = $(TESTS)
