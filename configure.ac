dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.63)
# AC_INIT(pveclib, 1.0.0)
AC_INIT([pveclib], [1.0.0], munroesj@us.ibm.com raji@linux.vnet.ibm.com tulioqm@br.ibm.com)


AC_CANONICAL_SYSTEM
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/vec_common_ppc.h])
AM_INIT_AUTOMAKE(subdir-objects)

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL

# Checks for header files.
AC_CHECK_HEADERS([altivec.h stddef.h stdint.h unistd.h stdio.h fenv.h float.h math.h])

# Doxygen support
#DX_HTML_FEATURE(ON)
#DX_MAN_FEATURE(OFF)
#DX_PDF_FEATURE(OFF)
#DX_PS_FEATURE(OFF)
#DX_INIT_DOXYGEN($PACKAGE_NAME, [ doc/pveclib-doxygen-pveclib.doxy ])

AC_CONFIG_FILES(Makefile src/Makefile)
AC_OUTPUT

