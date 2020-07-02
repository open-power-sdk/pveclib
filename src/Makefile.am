# bin_PROGRAMS=pveclib_test

AM_CPPFLAGS= -DNDEBUG
AM_CFLAGS= -m64 -O3

#Test only codes mostly for eyeballing the generated code
noinst_LTLIBRARIES = libvecdummy.la libvecdummyPWR9.la
#Any runtime and const tables needed by pveclib functions 
lib_LTLIBRARIES = libpvec.la libpvecstatic.la

libpvec_la_SOURCES = vec_runtime_DYN.c

libpvecstatic_la_SOURCES = tipowof10.c decpowof2.c

libvecdummyPWR9_la_SOURCES = testsuite/vec_pwr9_dummy.c

libvecdummy_la_SOURCES = testsuite/vec_int128_dummy.c \
	testsuite/vec_int512_dummy.c \
	testsuite/vec_int64_dummy.c \
	testsuite/vec_int32_dummy.c \
	testsuite/vec_int16_dummy.c \
	testsuite/vec_f128_dummy.c \
	testsuite/vec_f64_dummy.c \
	testsuite/vec_f32_dummy.c \
	testsuite/vec_bcd_dummy.c \
	testsuite/vec_char_dummy.c

libvecdummy_la_CFLAGS = $(AM_CPPFLAGS) $(PVECLIB_DEFAULT_CFLAGS) $(AM_CFLAGS)

libvecdummyPWR9_la_CFLAGS = $(AM_CPPFLAGS) $(PVECLIB_POWER9_CFLAGS) $(AM_CFLAGS)

libpvecstatic_la_CFLAGS = $(AM_CPPFLAGS) $(PVECLIB_DEFAULT_CFLAGS) $(AM_CFLAGS)

libpvec_la_CFLAGS = $(AM_CPPFLAGS) -fpic $(PVECLIB_DEFAULT_CFLAGS) $(AM_CFLAGS)

# C source files that correspond to .o's required for IFUNC targets
# for supported all machines.

# Alternative to LTCOMPILE which avoids user CFLAGS which interfere
# with the IFUNC library build
PVECCOMPILE = $(LIBTOOL) $(AM_V_lt) --tag=CC $(AM_LIBTOOLFLAGS) \
	$(LIBTOOLFLAGS) --mode=compile $(CC) $(DEFS) \
	$(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) \
	$(AM_CFLAGS)

vec_dynrt_PWR9.lo: vec_runtime_PWR9.c $(pveclibinclude_HEADERS)
if am__fastdepCC
	$(PVECCOMPILE) -fpic $(PVECLIB_POWER9_CFLAGS) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $(srcdir)/vec_runtime_PWR9.c
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Plo
else
if AMDEP
	source='vec_runtime_PWR9.c' object='$@' libtool=yes @AMDEPBACKSLASH@
	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) @AMDEPBACKSLASH@
endif
	$(PVECCOMPILE) -fpic $(PVECLIB_POWER9_CFLAGS) -c -o $@ $(srcdir)/vec_runtime_PWR9.c
endif

vec_staticrt_PWR9.lo: vec_runtime_PWR9.c $(pveclibinclude_HEADERS)
if am__fastdepCC
	$(PVECCOMPILE) $(PVECLIB_POWER9_CFLAGS) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $(srcdir)/vec_runtime_PWR9.c
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Plo
else
if AMDEP
	source='vec_runtime_PWR9.c' object='$@' libtool=yes @AMDEPBACKSLASH@
	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) @AMDEPBACKSLASH@
endif
	$(PVECCOMPILE) $(PVECLIB_POWER9_CFLAGS) -c -o $@ $(srcdir)/vec_runtime_PWR9.c
endif

vec_dynrt_PWR8.lo: vec_runtime_PWR8.c $(pveclibinclude_HEADERS)
if am__fastdepCC
	$(PVECCOMPILE) -fpic $(PVECLIB_POWER8_CFLAGS) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $(srcdir)/vec_runtime_PWR8.c
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Plo
else
if AMDEP
	source='vec_runtime_PWR8.c' object='$@' libtool=yes @AMDEPBACKSLASH@
	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) @AMDEPBACKSLASH@
endif
	$(PVECCOMPILE) -fpic $(PVECLIB_POWER8_CFLAGS) -c -o $@ $(srcdir)/vec_runtime_PWR8.c
endif

vec_staticrt_PWR8.lo: vec_runtime_PWR8.c $(pveclibinclude_HEADERS)
if am__fastdepCC
	$(PVECCOMPILE) $(PVECLIB_POWER8_CFLAGS) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $(srcdir)/vec_runtime_PWR8.c
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Plo
else
if AMDEP
	source='vec_runtime_PWR8.c' object='$@' libtool=yes @AMDEPBACKSLASH@
	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) @AMDEPBACKSLASH@
endif
	$(PVECCOMPILE) $(PVECLIB_POWER8_CFLAGS) -c -o $@ $(srcdir)/vec_runtime_PWR8.c
endif

vec_dynrt_PWR7.lo: vec_runtime_PWR7.c $(pveclibinclude_HEADERS)
if am__fastdepCC
	$(PVECCOMPILE) -fpic $(PVECLIB_POWER7_CFLAGS) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $(srcdir)/vec_runtime_PWR7.c
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Plo
else
if AMDEP
	source='vec_runtime_PWR7.c' object='$@' libtool=yes @AMDEPBACKSLASH@
	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) @AMDEPBACKSLASH@
endif
	$(PVECCOMPILE) -fpic $(PVECLIB_POWER7_CFLAGS) -c -o $@ $(srcdir)/vec_runtime_PWR7.c
endif

vec_staticrt_PWR7.lo: vec_runtime_PWR7.c $(pveclibinclude_HEADERS)
if am__fastdepCC
	$(PVECCOMPILE) $(PVECLIB_POWER7_CFLAGS) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $(srcdir)/vec_runtime_PWR7.c
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Plo
else
if AMDEP
	source='vec_runtime_PWR7.c' object='$@' libtool=yes @AMDEPBACKSLASH@
	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) @AMDEPBACKSLASH@
endif
	$(PVECCOMPILE) $(PVECLIB_POWER7_CFLAGS) -c -o $@ $(srcdir)/vec_runtime_PWR7.c
endif

vec_dynrt_common.lo: vec_runtime_common.c $(pveclibinclude_HEADERS)
if am__fastdepCC
	$(PVECCOMPILE) -fpic $(PVECLIB_DEFAULT_CFLAGS) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $(srcdir)/vec_runtime_common.c
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Plo
else
if AMDEP
	source='vec_runtime_common.c' object='$@' libtool=yes @AMDEPBACKSLASH@
	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) @AMDEPBACKSLASH@
endif
	$(PVECCOMPILE) -fpic $(PVECLIB_DEFAULT_CFLAGS) -c -o $@ $(srcdir)/vec_runtime_common.c
endif

EXTRA_DIST =

# This rule is necessary because pvec_test needs to statically link
# .libs/libpvecstatic.a and if make is executed in parallel (-jN) the 
# pveclib_test rule may be run before .libs/libpvecstatic.a is built.
.libs/libpvecstatic.a: libpvecstatic.la
.libs/libpvecdummy.a: libpvecdummy.la
.libs/libpvecdummyPWR9.a: libpvecdummyPWR9.la

# libpvec definitions.
# libpvec_la already includes vec_runtime_DYN.c compiled compiled -fpic
# for IFUNC resolvers.
# Now adding the -fpic -mcpu= target built runtimes.
libpvec_la_LDFLAGS = -version-info $(PVECLIB_SO_VERSION)
libpvec_la_LIBADD = vec_dynrt_PWR9.lo
libpvec_la_LIBADD += vec_dynrt_PWR8.lo
libpvec_la_LIBADD += vec_dynrt_PWR7.lo
libpvec_la_LIBADD += vec_dynrt_common.lo
libpvec_la_LIBADD += -lc

# libpvecstatic definitions, compiled without -fpic
# libpvecstatic_la already includes tipowof10.c decpowof2.c.
# Now adding the name qualified -mcpu= target built runtimes.
libpvecstatic_la_LIBADD = vec_staticrt_PWR9.lo
libpvecstatic_la_LIBADD += vec_staticrt_PWR8.lo
libpvecstatic_la_LIBADD += vec_staticrt_PWR7.lo

# pveclib definitions
pveclibincludedir = $(includedir)/pveclib

# pveclib_test definitions
pveclib_testincludedir = $(includedir)/testsuite

# The headers in 'plibvec_HEADERS' are meant to be the ones installed
# on 'make install'
pveclibinclude_HEADERS = \
	pveclib/vec_common_ppc.h \
	pveclib/vec_f128_ppc.h \
	pveclib/vec_f64_ppc.h \
	pveclib/vec_f32_ppc.h \
	pveclib/vec_int512_ppc.h \
	pveclib/vec_int128_ppc.h \
	pveclib/vec_int64_ppc.h \
	pveclib/vec_int32_ppc.h \
	pveclib/vec_int16_ppc.h \
	pveclib/vec_char_ppc.h \
	pveclib/vec_bcd_ppc.h

pveclib_la_INCLUDES = $(pveclibinclude_HEADERS)

pveclib_test_la_INCLUDES = $(pveclibinclude_HEADERS)

EXTRA_DIST += $(pveclib_la_INCLUDES)

# Test programs invoked with 'make check'
TESTS =

TESTS += pveclib_test
pveclib_test_SOURCES = \
	testsuite/pveclib_test.c \
	testsuite/arith128_print.c \
	testsuite/vec_perf_f64.c \
	testsuite/vec_perf_f32.c \
	testsuite/vec_perf_i512.c \
	testsuite/vec_perf_i128.c \
	testsuite/arith128_test_f128.c \
	testsuite/arith128_test_f64.c \
	testsuite/arith128_test_f32.c \
	testsuite/arith128_test_i512.c \
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
	testsuite/arith128_test_i512.h \
	testsuite/arith128_test_i16.h \
	testsuite/arith128_test_i32.h \
	testsuite/arith128_test_i64.h \
	testsuite/vec_perf_f32.h \
	testsuite/vec_perf_f64.h \
	testsuite/vec_perf_i512.h \
	testsuite/vec_perf_i128.h

pveclib_test_CFLAGS = $(AM_CPPFLAGS) $(PVECLIB_DEFAULT_CFLAGS) $(AM_CFLAGS)
pveclib_test_LDADD = .libs/libpvecstatic.a .libs/libvecdummy.a
	
TESTS += vec_dummy

#Dummy main to force generation of vec_dummy_* codes
vec_dummy_SOURCES = testsuite/vec_dummy_main.c 

vec_dummy_CFLAGS = $(AM_CPPFLAGS) $(PVECLIB_DEFAULT_CFLAGS) $(AM_CFLAGS)

vec_dummy_LDADD   = libpvecstatic.la libvecdummy.la libvecdummyPWR9.la

check_PROGRAMS = $(TESTS)
