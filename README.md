# pveclib

## Power Vector Library

Header files that contain useful functions leveraging the PowerISA
Vector Facilities: Vector Multimedia Extension (VMX AKA Altivec) and
Vector Scalar Extention (VSX).

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
  For example vector SIMI implementation for ASCII __isalpha, etc.
  Another example full __int128 implementations of Count Leading Zeros,
  Population Count, and Multiply.

## Build

    $ libtoolize
    $ aclocal
    $ automake --add-missing
    $ autoconf
    $ ./configure
    $ make

and, optionally:

    $ make check

## Usage

Once pveclib is installed on the POWER or OpenPOWER system
simply add -I/<path>/pveclib to the compile command line. Then
include the appropriate header. For example:

#include <vec_int128_ppc.h>

