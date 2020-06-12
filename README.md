# pveclib

## Power Vector Library

Header files that contain useful functions leveraging the PowerISA
Vector Facilities: Vector Multimedia Extension (VMX AKA Altivec) and
Vector Scalar Extension (VSX). Larger functions like quadword multiply
and multiple quadword multiply and madd are large enough to justify
CPU specific and tuned run-time libraries. The user can choose to bind
to platform specific static archives or dynamic shared object libraries
which automatically (dynamic linking with IFUNC resolves) select the
correct implementation for the CPU it is running on.

The goal of this project to provide well crafted implementations
of useful vector and large number operations:

- Provide equivalent functions across versions of the PowerISA.
  For example the Vector Multiply-by-10 Unsigned Quadword
  operations introduced in PowerISA 3.0 (POWER9) can be implement in a
  few vector instructions on earlier PowerISA versions.
- Provide equivalent functions across versions of the compiler.
  For example builtins provided in later versions of the compiler
  can be implemented as inline functions with inline asm in earlier
  compiler versions.
- Provide higher order functions not provided directly by the PowerISA.
  For example vector SIMD implementation for ASCII `__isalpha`, etc.
  Another example full `__int128` implementations of Count Leading Zeros,
  Population Count, and Multiply.
- Provide optimized run-time libraries for quadword integer multiply
  and multi-quadword integer multiply and add.

## Build

PVECLIB now supports CPU tuned run-time libraries, both static archives
and dynamic (IFUNC selected) shared objects. This complicates the build
process as it now has to build the same source code, multiple times,
with different compile targets (-mcpu=). Another complication comes
from compiling for big endian systems where the compiler default target
may not include the vector facilities (VMX and VSX).

## Configure and option flags

The project can use configure test to define options like AM_CPPFLAGS
and AM_CFLAGS but the user command line options (CPPFLAGS and CFLAGS)
are always applied last and take precedent.
See: Automake "Flag Variables Ordering" 
https://www.gnu.org/software/automake/manual/html_node/Flag-Variables-Ordering.html

So a configure flag like CFLAGS='-O3 -mcpu=power7' would be OK for
functional verification tests of the POWER7 specific implementations
of PVECLIB operations. But this would interfere with building the POWER8
and POWER9 specific objects for the production version of libpvec.so.
So builds for production level PVECLIB should never specify -mcpu= in
CFLAGS.

On the other hand if the user does not specify any CFLAGS, autoconf will
fill in a default value of '-O2 -g'. This is bad! PVECLIB needs the
global common subexpression, loop, and vector cost model optimizations
enabled by '-O3'. Also '-g' will generate huge debug tables for the vector
int512 run-time and slow down the build. If you need to profile or
debug with basic back-trace information, use '-g1'.

So unless you are involved in the functional testing of new PVECLIB
operations, the safe options are:

CFLAGS='-m64 -g1 -O3'

The PVECLIB Makefile.am files include special macros for CPU specific
run-time compiles. These macros exclude the user CFLAGS from those
compile commands.

On the other hand, if the compiler default target does not support
PowerISA vector facilities and an appropriate '-mcpu=' option is not
supplied, the compile will fail. So the PVECLIB configure.ac includes a
number of configure tests that detect this and provide appropriate
compile targets.

The current PVECLIB implementation assumes the target supports both VMX
(Altivec) and VSX facilities. So the minimum targets are set internally
(PVECLIB_DEFAULT_CFLAG) to '-mcpu=power7' for BE and '-mcpu=power8' for LE.

The  PVECLIB configure.ac also includes configure tests for related
PowerISA facilities that can be leveraged for PVECLIB operations but
are not core functions. This includes decimal floating-point and IEEE
128-bit binary floating-point. These are both target and compiler
support checks. The compiler checks are especially important for the
Clang compiler as it is currently missing Decimalxx and Float128
support. Some PVECLIB operations will be disabled in this case.

The default compiler is 'gcc'. The project can be configured to use
the Clang / LLVM compiler using the CC=clang flag.

Run './configure', to verify the build tools and environment.

    $ ./configure CFLAGS='-O3 -g1'

On a big endian / biarch systems it is wise to explicitely specify 64-bit.

    $ ./configure CFLAGS='-m64 -O3 -g1' LDFLAGS='-m64'

To use the Advance Toolchain.

    $ ./configure  CC=/opt/at13.0/bin/powerpc64le-linux-gnu-gcc \
	AR=/opt/at13.0/bin/powerpc64le-linux-gnu-ar \
	RANLIB=/opt/at13.0/bin/powerpc64le-linux-gnu-ranlib \
	CFLAGS='-m64 -O3 -g1' LDFLAGS='-m64'

Then run 'make' to perform the basic compile tests and build the
run-time libraries:

    $ make

and, optionally run the functional verication tests:

    $ make check
    
and, install the headers and librarys so your programs can use them:

    $ make install

If the included autotools dont match the version installed on your
system, perform these step:

    $ aclocal
    $ autoconf
    $ automake

## Usage

Once pveclib is installed on the POWER or OpenPOWER system
simply include the appropriate header. For example:

    #include <pveclib/vec_int128_ppc.h>

The headers are organized by element type:

    vec_common_ppc.h; Typedefs and helper macros
    vec_f128_ppc.h; Operations on vector _Float128 values
    vec_f64_ppc.h; Operations on vector double values
    vec_f32_ppc.h; Operations on vector float values
    vec_int512_ppc.h; Operations on Multi-quadword integer values
    vec_int128_ppc.h; Operations on vector __int128 values
    vec_int64_ppc.h; Operations on vector long int (64-bit) values
    vec_int32_ppc.h; Operations on vector int (32-bit) values
    vec_int16_ppc.h; Operations on vector short int (16-bit) values
    vec_char_ppc.h; Operations on vector char (8-bit) values
    vec_bcd_ppc.h; Operations on vectors of Binary Code Decimal and Zoned Decimal values


Full documentation is linked off of:

    https://github.com/open-power-sdk/pveclib/wiki

