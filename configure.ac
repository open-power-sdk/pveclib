## dnl Process this file with autoconf to produce a configure script.


AC_PREREQ(2.63)
AC_INIT([pveclib], [1.0.4], munroesj52@gmail.com pc@us.ibm.com, , https://github.com/open-power-sdk/pveclib)

AC_ENABLE_SHARED
LT_INIT([disable-shared])

AC_CANONICAL_TARGET
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/pveclib/vec_common_ppc.h])
AM_INIT_AUTOMAKE(subdir-objects)
AM_MAINTAINER_MODE([disable])
AX_AM_MACROS

# Update this value for every release: (A:B:C will map to foo.so.(A-C).C.B)
# http://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
PVECLIB_SO_VERSION=1:4:0
AC_SUBST(PVECLIB_SO_VERSION)

AC_PROG_CC
AC_PROG_LIBTOOL

# This directive is to avoid buggy libtool that doesn't add the '-Wl,--no-as-needed'
# directive in the correct position of LDFLAGS
LDFLAGS="$LDFLAGS -Wl,--no-as-needed -lc"

# Checks for header files.
AC_CHECK_HEADERS([altivec.h stddef.h stdint.h unistd.h stdio.h fenv.h float.h math.h])

#############################################################################

CNAME=`basename "$CC"`
AM_CPPFLAGS="-DNDEBUG"
AM_CFLAGS="-m64 -O3"

## PowerPC-64 Specific options
## Save CFLAGS
SAVED_CFLAGS="$CFLAGS"

ALTIVEC_CFLAGS="-mcpu=970 -maltivec"
POWER_F128_CFLAGS="-mfloat128"
POWER6_CFLAGS="-mcpu=power6 -maltivec"
POWER7_CFLAGS="-mcpu=power7"
POWER8_CFLAGS="-mcpu=power8"
POWER9_CFLAGS="-mcpu=power9 -mfloat128"

##### Compiler default includes ALTIVEC #####
AC_SUBST([PVECLIB_DEFAULT_CFLAG], [""])

CFLAGS=$AM_CFLAGS
AC_MSG_CHECKING([if $CNAME default supports ALTIVEC])
AC_LINK_IFELSE(
	[AC_LANG_SOURCE([`cat testprograms/test_ppc_VMX.c`])],
	[AC_MSG_RESULT([yes])]
		AC_SUBST([PVECLIB_DEFAULT_CFLAG], [""]),
	[AC_MSG_RESULT([no])]
		AC_DEFINE([PVECLIB_DISABLE_ALTIVEC], [1], [Disable ALTIVEC])
)

if test "$retval" != "0"; then
	AC_SUBST([PVECLIB_DEFAULT_CFLAG],[POWER8_CFLAGS])
fi

##### Compiler default includes DFP #####

CFLAGS=$AM_CFLAGS
AC_MSG_CHECKING([if $CNAME default supports DFP])
AC_LINK_IFELSE(
	[AC_LANG_SOURCE([`cat testprograms/test_ppc_DFP.c`])],
	[AC_MSG_RESULT([yes])]
		AC_SUBST([PVECLIB_DEFAULT_CFLAGS], [""]),
	[AC_MSG_RESULT([no])]
		AC_DEFINE([PVECLIB_DISABLE_DFP], [1], [Disable DFP])
)

if test "$retval" != "0"; then
	AC_SUBST([PVECLIB_DEFAULT_CFLAGS],[POWER8_CFLAGS])
fi

##### Compiler default includes VSX #####
## Use this test to set the minimum flags required to compile PVECLIB.
## This should cover BE compilers where the compiler default -mcpu is
## not at least power7. Otherwise the compiler will disable VSX
## features (like vector double) and associated built-ins.
## Base VSX by the PowerISA architecture compliance rules should
## include VMX, DFP, and vector double.

CFLAGS=$AM_CFLAGS
AC_MSG_CHECKING([if $CNAME default supports VSX])
AC_LINK_IFELSE(
    [AC_LANG_SOURCE([`cat testprograms/test_ppc_VSX.c`])],
    [AC_MSG_RESULT([yes])]
	AC_SUBST([PVECLIB_DEFAULT_CFLAGS], [""]),
    [
	AC_MSG_RESULT([no])
	CFLAGS="$AM_CFLAGS $POWER7_CFLAGS"
        AC_MSG_CHECKING([if $CNAME requires -mcpu=power7 for VSX])
        AC_LINK_IFELSE(
            [AC_LANG_SOURCE([`cat testprograms/test_ppc_VSX.c`])],
            [AC_MSG_RESULT([yes])]
                AC_SUBST([PVECLIB_DEFAULT_CFLAGS], [$POWER7_CFLAGS]),
            [
		AC_MSG_RESULT([no])
                CFLAGS="$AM_CFLAGS $POWER8_CFLAGS"
                AC_MSG_CHECKING([if $CNAME requires -mcpu=power8 for VSX])
                AC_LINK_IFELSE(
                    [AC_LANG_SOURCE([`cat testprograms/test_ppc_VSX.c`])],
                    [AC_MSG_RESULT([yes])]
                        AC_SUBST([PVECLIB_DEFAULT_CFLAGS], [$POWER8_CFLAGS]),
				    [AC_MSG_RESULT([no])]
						AC_DEFINE([PVECLIB_DISABLE_VSX], [1], [Disable VSX])
                )
            ]
        )
    ]
)

##### Compiler default includes Float128 or needs added CFLAGS #####

CFLAGS=$AM_CFLAGS
AC_MSG_CHECKING([if $CNAME default supports Float128])
AC_LINK_IFELSE(
    [AC_LANG_SOURCE([`cat testprograms/test_ppc_F128.c`])],
    [AC_MSG_RESULT([yes])]
        AC_SUBST([PVECLIB_FLOAT128_CFLAGS], [""]),
    [
	AC_MSG_RESULT([no])
        CFLAGS+=" $POWER_F128_CFLAGS"
        AC_MSG_CHECKING([if $CNAME requires -mfloat128 for Float128])
        AC_LINK_IFELSE(
            [AC_LANG_SOURCE([`cat testprograms/test_ppc_F128.c`])],
            [AC_MSG_RESULT([yes])]
                AC_SUBST([PVECLIB_FLOAT128_CFLAGS], ["$POWER_F128_CFLAGS"]),
            [
		AC_MSG_RESULT([no])
                CFLAGS="$AM_CFLAGS $POWER9_CFLAGS"
                AC_MSG_CHECKING([if $CNAME requires $POWER9_CFLAGS for Float128])
                AC_LINK_IFELSE(
                    [AC_LANG_SOURCE([`cat testprograms/test_ppc_F128.c`])],
                    [AC_MSG_RESULT([yes])]
                        AC_SUBST([PVECLIB_FLOAT128PWR9_CFLAGS], ["POWER9_CFLAGS"]),
                    [AC_MSG_RESULT([no])]
                        AC_DEFINE([PVECLIB_DISABLE_F128], [1], [Disable Float128])
                )
            ]
        )
    ]
)

##### Concatinate FLOAT128 CFLAGS to DEFAULT #####

PVECLIB_DEFAULT_CFLAGS+=" $PVECLIB_FLOAT128_CFLAGS"

##### Provide a nice message is the compiler default is OK #####
if test "$PVECLIB_DEFAULT_CFLAGS" != " "; then
	PVECLIB_DEFAULT_TEXT=$PVECLIB_DEFAULT_CFLAGS
    AC_MSG_RESULT(["PVECLIB_DEFAULT base target set to '$PVECLIB_DEFAULT_CFLAGS'"])
else
	PVECLIB_DEFAULT_TEXT="default"
fi

##### Compiler default|CFLAGS includes Float128 Arithmetic #####

CFLAGS="$AM_CFLAGS $PVECLIB_DEFAULT_CFLAGS"
AC_MSG_CHECKING([if $CNAME $PVECLIB_DEFAULT_TEXT supports Float128 arithmetic])
AC_LINK_IFELSE(
	[AC_LANG_SOURCE([`cat testprograms/test_ppc_F128arith.c`])],
	[AC_MSG_RESULT([yes])]
		AC_SUBST([PVECLIB_FLOAT128ARITH_CFLAGS], ["$PVECLIB_DEFAULT_CFLAGS"]),
	[AC_MSG_RESULT([no])]
		AC_DEFINE([PVECLIB_DISABLE_F128ARITH], [1], [Disable Float128 Arithmetic])
)

##### Compiler default|CFLAGS includes Float128 math.h #####

CFLAGS="$AM_CFLAGS $PVECLIB_DEFAULT_CFLAGS"
AC_MSG_CHECKING([if $CNAME $PVECLIB_DEFAULT_TEXT supports Float128 math.h])
AC_LINK_IFELSE(
	[AC_LANG_SOURCE([`cat testprograms/test_ppc_F128math.c`])],
	[AC_MSG_RESULT([yes])]
		AC_SUBST([PVECLIB_FLOAT128MATH_CFLAGS], ["$PVECLIB_DEFAULT_CFLAGS"]),
	[AC_MSG_RESULT([no])]
		AC_DEFINE([PVECLIB_DISABLE_F128MATH], [1], [Disable Float128 math.h])
)

##### Compiler default|CFLAGS includes vector __int128 #####

CFLAGS="$AM_CFLAGS $PVECLIB_DEFAULT_CFLAGS"
AC_MSG_CHECKING([if $CNAME $PVECLIB_DEFAULT_TEXT supports vector __int128])
AC_LINK_IFELSE(
	[AC_LANG_SOURCE([`cat testprograms/test_ppc_int128.c`])],
	[AC_MSG_RESULT([yes])]
		AC_SUBST([PVECLIB_VINT128_CFLAGS], ["$PVECLIB_DEFAULT_CFLAGS"]),
	[AC_MSG_RESULT([no])]
		AC_DEFINE([PVECLIB_DISABLE_INT128], [1], [Disable vector __int128])
)

##### Compiler default|CFLAGS includes vector __bool __int128 #####

CFLAGS="$AM_CFLAGS $PVECLIB_DEFAULT_CFLAGS"
AC_MSG_CHECKING([if $CNAME $PVECLIB_DEFAULT_TEXT supports vector __bool __int128])
AC_LINK_IFELSE(
	[AC_LANG_SOURCE([`cat testprograms/test_ppc_bool_int128.c`])],
	[AC_MSG_RESULT([yes])]
		AC_SUBST([PVECLIB_VBOOL128_CFLAGS], ["$PVECLIB_DEFAULT_CFLAGS"]),
	[AC_MSG_RESULT([no])]
		AC_DEFINE([PVECLIB_DISABLE_BOOLINT128], [1], [Disable vector __bool __int128])
)

##### Compiler default|CFLAGS includes vector __int128 comstants #####

CFLAGS="$AM_CFLAGS $PVECLIB_DEFAULT_CFLAGS"
AC_MSG_CHECKING([if $CNAME $PVECLIB_DEFAULT_TEXT supports vector __int128 comstants])
AC_LINK_IFELSE(
	[AC_LANG_SOURCE([`cat testprograms/test_ppc_const_int128.c`])],
	[AC_MSG_RESULT([yes])]
		AC_SUBST([PVECLIB_VCONST128_CFLAGS], ["$PVECLIB_DEFAULT_CFLAGS"]),
	[AC_MSG_RESULT([no])]
		AC_DEFINE([PVECLIB_DISABLE_CONSTINT128], [1], [Disable vector __int128 constants])
)

##### Power7 #####

CFLAGS="$POWER7_CFLAGS"
AC_MSG_CHECKING([if $CNAME supports $CFLAGS (Power7)])
AC_LINK_IFELSE(
	[AC_LANG_SOURCE([`cat testprograms/test_ppc_PWR7.c`])],
	[AC_MSG_RESULT([yes])]
		AC_SUBST([PVECLIB_POWER7_CFLAGS], [$POWER7_CFLAGS]),
	[AC_MSG_RESULT([no])]
		AC_DEFINE([PVECLIB_DISABLE_POWER7], [1], [Disable POWER7])
)

if test "$retval" != "0"; then
	AC_SUBST([POWER7_CFLAGS],[""])
fi

##### Power8 #####

CFLAGS="$POWER8_CFLAGS"
AC_MSG_CHECKING([if $CNAME supports $CFLAGS (Power8)])
AC_LINK_IFELSE(
	[AC_LANG_SOURCE([`cat testprograms/test_ppc_PWR8.c`])],
	[AC_MSG_RESULT([yes])]
		AC_SUBST([PVECLIB_POWER8_CFLAGS], [$POWER8_CFLAGS]),
	[AC_MSG_RESULT([no])]
		AC_DEFINE([PVECLIB_DISABLE_POWER8], [1], [Disable POWER9])
)

if test "$retval" != "0"; then
	AC_SUBST([POWER8_CFLAGS],[""])
fi

##### Power9 #####

CFLAGS="$POWER9_CFLAGS"
AC_MSG_CHECKING([if $CNAME supports $CFLAGS (Power9)])
AC_LINK_IFELSE(
	[AC_LANG_SOURCE([`cat testprograms/test_ppc_PWR9.c`])],
	[AC_MSG_RESULT([yes])]
		AC_SUBST([PVECLIB_POWER9_CFLAGS], [$POWER9_CFLAGS]),
	[AC_MSG_RESULT([no])]
		AC_DEFINE([PVECLIB_DISABLE_POWER9], [1], [Disable POWER9])
)

if test "$retval" != "0"; then
	AC_SUBST([POWER9_CFLAGS],[""])
fi


CFLAGS="$SAVED_CFLAGS"

#############################################################################

# Doxygen support
DX_HTML_FEATURE(ON)
DX_MAN_FEATURE(OFF)
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN($PACKAGE_NAME, [ doc/pveclib-doxygen-pveclib.doxy ])

AC_CONFIG_FILES(Makefile src/Makefile)
AC_OUTPUT
