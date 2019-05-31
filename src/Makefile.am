# bin_PROGRAMS=pveclib_test

#Test only codes mostly for eyeballing the generated code
noinst_LTLIBRARIES = libvecdummy.la
#Any runtime and const tables needed by pveclib functions 
lib_LTLIBRARIES = libpvec.la

libpvec_la_SOURCES = tipowof10.c decpowof2.c
libvecdummy_la_SOURCES = testsuite/vec_int128_dummy.c \
	testsuite/vec_int64_dummy.c \
	testsuite/vec_int32_dummy.c \
	testsuite/vec_int16_dummy.c \
	testsuite/vec_f128_dummy.c \
	testsuite/vec_f64_dummy.c \
	testsuite/vec_f32_dummy.c \
	testsuite/vec_pwr9_dummy.c \
	testsuite/vec_bcd_dummy.c \
	testsuite/vec_char_dummy.c

EXTRA_DIST =

# This rule is necessary because pvec_test needs to statically link
# .libs/libpvec.a and if make is executed in parallel (-jN) the 
# pveclib_test rule may be run before .libs/libpvec.a is built.
.libs/libpvec.a: libpvec.la
.libs/libpvecdummy.a: libpvecdummy.la

# pveclib definitions
pveclibincludedir = $(includedir)/pveclib

# pveclib_test definitions
pveclib_testincludedir = $(includedir)/testsuite

# The headers in 'plibvec_HEADERS' are meant to be the ones installed
# on 'make install'
pveclibinclude_HEADERS = \
	vec_common_ppc.h \
	vec_f128_ppc.h \
	vec_f64_ppc.h \
	vec_f32_ppc.h \
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
	testsuite/vec_perf_f64.c \
	testsuite/vec_perf_f32.c \
	testsuite/vec_perf_i128.c \
	testsuite/arith128_test_f128.c \
	testsuite/arith128_test_f64.c \
	testsuite/arith128_test_f32.c \
	testsuite/arith128_test_i128.c \
	testsuite/arith128_test_i64.c \
	testsuite/arith128_test_i32.c \
	testsuite/arith128_test_i16.c \
	testsuite/arith128_test_char.c \
	testsuite/arith128_test_bcd.c \
	testsuite/arith128.h \
	testsuite/arith128_print.h \
	testsuite/arith128_test_bcd.h \
	testsuite/arith128_test_char.h \
	testsuite/arith128_test_f128.h \
	testsuite/arith128_test_f32.h \
	testsuite/arith128_test_f64.h \
	testsuite/arith128_test_i128.h \
	testsuite/arith128_test_i16.h \
	testsuite/arith128_test_i32.h \
	testsuite/arith128_test_i64.h \
	testsuite/vec_perf_f32.h \
	testsuite/vec_perf_f64.h \
	testsuite/vec_perf_i128.h
	
pveclib_test_LDADD = .libs/libpvec.a .libs/libvecdummy.a
	
TESTS += vec_dummy

#Dummy main to force generation of vec_dummy_* codes
vec_dummy_SOURCES = testsuite/vec_dummy_main.c 

vec_dummy_LDADD   = .libs/libpvec.a libvecdummy.la

check_PROGRAMS = $(TESTS)
