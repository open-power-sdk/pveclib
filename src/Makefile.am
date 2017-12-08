# bin_PROGRAMS=pveclib_test

noinst_LTLIBRARIES = libvecdummy.la
lib_LTLIBRARIES = libpvec.la

libpvec_la_SOURCES= tipowof10.c decpowof2.c


EXTRA_DIST      =

# This rule is necessary because pvec_test needs to statically link
# .libs/libpvec.a and if make is executed in parallel (-jN) the 
# pveclib_test rule may be run before .libs/libpvec.a is built.
.libs/libpvec.a: libpvec.la

# pveclib definitions
pveclibincludedir = $(includedir)/pveclib

# libvecdummy definitions
pveclib_testincludedir = $(includedir)/testsuite

# The headers in 'plibvec_HEADERS' are meant to be the ones installed
# on 'make install'
pveclibinclude_HEADERS = \
	vec_common_ppc.h \
	vec_int128_ppc.h
	
pveclib_test_la_INCLUDES = \
	$(pveclibinclude_HEADERS) \
	arith128_print.h  \
	arith128_test_i128.h  \
	arith128.h 

EXTRA_DIST          += $(pveclib_test_la_INCLUDES)

libvecdummy_la_SOURCES = testsuite/vec_int128_dummy.c

# Test programs invoked with 'make check'
TESTS = 
TESTS            += pveclib_test
pveclib_test_SOURCES= testsuite/pveclib_test.c testsuite/arith128_print.c testsuite/arith128_test_i128.c
pveclib_test_LDADD = .libs/libpvec.a libvecdummy.la

TESTS            += vec_dummy
vec_dummy_SOURCES = testsuite/vec_dummy_main.c
vec_dummy_LDADD   = .libs/libpvec.a libvecdummy.la


check_PROGRAMS = $(TESTS)

