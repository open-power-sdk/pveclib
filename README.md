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
  For example vector SIMD implementation for ASCII `__isalpha`, etc.
  Another example full `__int128` implementations of Count Leading Zeros,
  Population Count, and Multiply.

## Build

It is likely sufficient to perform:

    $ ./configure  CFLAGS="-O3 -mcpu=power8"
    $ make

and, optionally:

    $ make check

If the included autotools poorly match what is installed on the system,
perform this step first:

    $ autoreconf

## Usage

Once pveclib is installed on the POWER or OpenPOWER system
simply include the appropriate header. For example:

    #include <pveclib/vec_int128_ppc.h>

The headers are organized by element type:

    vec_common_ppc.h; Typedefs and helper macros
    vec_f128_ppc.h; Operations on vector _Float128 values
    vec_f64_ppc.h; Operations on vector double values
    vec_f32_ppc.h; Operations on vector float values
    vec_int128_ppc.h; Operations on vector __int128 values
    vec_int64_ppc.h; Operations on vector long int (64-bit) values
    vec_int32_ppc.h; Operations on vector int (32-bit) values
    vec_int16_ppc.h; Operations on vector short int (16-bit) values
    vec_char_ppc.h; Operations on vector char (8-bit) values
    vec_bcd_ppc.h; Operations on vectors of Binary Code Decimal and Zoned Decimal values


Full documentation is linked off of:

    https://github.com/open-power-sdk/pveclib/wiki

