# ChangeLog

## v1.0.2 - 2019-06-24

 - Rename local aminclude.am to doxygen.am

 - Mark pveclib as version 1.0.2 and regen configure and Makefiles

## v1.0.1 - 2019-06-17

 - Add script to generate ChangeLog.md file from git change log.

 - Mark pveclib as version 1.0.1 for release. Revert attempt to use .@INC_AMINCLUDE@

 - Mark pveclib as version 1.0.1 for release. Update and regenerate autoconf and automake files.

 - Test falures while testing Fedora 30 and GCC 9 compiler. Traced back to bad code generation for if (__builtin_bcdsub_gt) tests in vec_bcdaddcsq, vec_bcdsubcsq. Found a worrk around by replacing the if test (which is setting the carry sign code) with vec_bcdcpsgn() Also found check_vint384 was not printing 128-bit values in the correct high to low order. Corrected this.

 - Fix vec_f128_ppc.h and dummmy for older compilers & without -mfloat128: Float128 support is work in progress and Distro toolchains do not include all Float128 backports provided by the Advance Toolchain. So PVECLIB f128 operations that compile with AT may not compile with the distro toolchain of the same GCC version. This would be dispointing for distros that claim power9 support. This patch set insures that the Ubuntu 18.04 LTS toolchain can compile and execute all pveclib vec_f128_ppc.h operations for -mcpu=power8/9.

 - Add pveclib subdir prefix to include directives in installed headers

 - Fix make dist check

## v1.0.0 - 2019-05-22

 - Mark pveclib as version 1.0.0 for release

 - Add extended quadword BCD <> binary conversion to vec_bcd_ppc.h: Doxygen text updates after review.

 - Add long division factoring support to vec_int128_ppv.h: Minor updated after review.

 - Add long division factoring support to vec_int128_ppc.h: Doxygen text changes after review.

 - Add extended quadword multiply support to vec_int128_ppc.h: Enable extended quadword BCD conversion. Additional timed tests.

 - Add extended BCD convert support to vec_int128_ppc.h: Enable extended quadword multiply by constant powers of 10. Additional timed performance tests. The functions timed_ctmaxdouble_10e32 (sprintf) and timed_cfmaxdouble_10e32 (strtod) for the double __DBL_MAX__ value provide a comparison with timed_longbcdcf_10e32 and timed_longbcdct_10e32 using pveclib.

 - Add long division factoring support to vec_int128_ppv.h: Enable extended quadword long division by constant powers of 10. Addition compile tests and examples. Some examples are used as performance tests.

 - Add extended quadword BCD <> binary conversion to vec_bcd_ppc.h: Add unit tests for extended quadword BCD <-> Binary conversion.

 - Add extended quadword BCD <> binary conversion to vec_bcd_ppc.h: Add examples for extended quadword BCD <-> Binary conversion. These are called from unit and timed performance tests as well as used as examples in the overview documentation.

 - Add extended quadword BCD <> binary conversion to vec_bcd_ppc.h: Documnent extened BCD <-> binary conversions with examples. Add a POWER9 specific optimization to vec_bcdctuq().

 - Add long division factoring support to vec_int128_ppv.h: Enable extended quadword long division by constant powers of 10. Additional unit tests.

 - Add long division factoring support to vec_int128_ppv.h: Enable extended quadword long division by constant powers of 10. Additional timed performance tests.

 - Add long division factoring support to vec_int128_ppv.h: Enable extended quadword long division by constant powers of 10. Addition compile tests and examples. Some examples are used as performance tests.

 - Add long division factoring support to vec_int128_ppv.h: Enable extended quadword long division by constant powers of 10. This support conversion of multi-quadword binary integers to Decimal.

 - More vec_bcd_ppc.h add binary to BCD conversion, after review: Add compile tests for new operations. Some clean up.

 - More vec_bcd_ppc.h add binary to BCD conversio, after review: Add unit tests for new operations.

 - More vec_bcd_ppc.h add binary to BCD conversion. After review: Cleaned up doxygent documentation for syntax and clarity. General code cleanup and compile warning removal.

 - More vec_bcd_ppc.h add binary to BCD conversion: Add compile tests for new operations.

 - More vec_bcd_ppc.h add binary to BCD conversion: Add unit tests for new operations. Clean up debug prints from last round.

 - More vec_bcd_ppc.h add binary to BCD conversion: More doxygent documentation. New functions for binary to BCD conversion. Selective optimizations to leverage; POWER9, DFU, and compiler strength redunction.

 - More vec_bcd_ppc.h add factoring support to vec_int128_ppv.h: After review: Update doxygent text. Clarifying code changes. Add new compile tests to vec_int128_dummy.c and vec_pwr9_dummy.c

 - More vec_bcd_ppc.h compile and unit tests: One more spelling error.

 - More vec_bcd_ppc.h compile and unit tests: Updates after review

 - More vec_bcd_ppc.h add factoring support to vec_int128_ppv.h: Added divide/modulo operations to to factor signed/unsigned vector __int128 values to convert to BCD. The __int128 type can represent intergers upto 39 digits while vector BCD can represent 31 (signed) or 32 (unsigned) digits. The operations use the multiplicative inverse to divide by 10**31 or 10*32) for the quotient. Then multiple and subtrace to issolate the remainder.

 - More vec_bcd_ppc.h compile and unit tests: Update unit tests after change to bcdsub and related functions. Added unit tests for new operations. Added compile tests for new operations. Added power9 target tests for new operations.

 - More vec_bcd_ppc.h: Clean up after review. Update Doxygen comments for spelling & syntax. Adjust indent and whitespace in some functions.

 - More vec_bcd_ppc.h: Clean up of Extend/carry support for BCD add/sub. Fix a bug with borrow. Added BCD compare for predicate and bool New functions; vec_bcdcfz vec_bcdctz Add shift and round, shift unsigned, Truncate, and Unsigned truncate. Latentcy estimates for new functions. Add Doxygen exmaple of extented multiply.

 - More vec_bcd_ppc.h with updates after review. Mostly comments and Doxygen. Some minor code formating.

 - More vec_bcd_ppc.h compile and unit tests. Added: Signed BCD to vector __int128 conversion. Extend/carry support for BCD add/sub. BCD Multiply low/High combined (62-digit). BCD Digit shifts.

 - More vec_bcd_ppc.h. Added: Signed BCD to vector __int128 conversion. Extend/carry support for BCD add/sub. BCD Multiply low/High combined (62-digit). BCD Digit shifts. Latentcy estimates for all implemented so far Add Doxygen to explain why extend/carry got so complicated.

 - Minor updates after review.

 - Complete the vec_bcd_ppc.h unsigned BCD to binary operations. Updates after Paul's review.

 - Complete compiler & unit tests for unsigned BCD to binary operations. Add unit tests for radix decimal to binary conversion. Add unit tests for BCD to binary conversion. Add BCD compile tests. Add vector char compile tests. Add POWER9 compile test.

 - Complete the vec_bcd_ppc.h. unsigned BCD to binary operations. Added the widening radix conversion functions that are the basis for the BCD to Binary conversions. Then defined the unsigned BCD to binary functions. Will add the signed BCD convertions later (need the appropriate set sign set bool operations first). Finally updated the Doxygen comments to include the PowerISA history for BCD and DFP.

 - Cleanup arith128_test_bcd.c and vec_bcd_dummy.c. After review cleanup.

 - Cleanup arith128_test_bcd.c and vec_bcd_dummy.c. Use vBCD_t type and fix formatting and white-space. Add compile test for BCD and some DFP builtins.

 - Cleanup vec_bcd_ppc.h. Use __builtin_bcdadd/bcdsub/denbcdq/ddedpdq where posible. Otherwise use inline asm where needed. Turns out there are compiler bugs that prevent using the __builtin functions on older compilers. Also add a forward ref declare for vec_muludq in vec_int128_ppc.h. This fixes a function not found when compiling -mcpu=power7.

 - More Fixes for vec_int128_ppc.h. Changes to address Paul's review comments and a clean doxygen generated text.

 - P3 Fixes for vec_int128_ppc.h. Add compile tests

 - P2 Unit tests for vec_int128_ppc.h. Specifics follow: Added new quadword unit tests; vec_absduq,vec_avguq, vec_maxsq, vec_maxuq, vec_minsq, vec_minuq, vec_mulhuq, vec_rlq, vec_rlqi, vec_sldqi, vec_sraq, and vec_sraqi.

 - More Fixes for vec_int64_ppc.h and functions for vec_int128_ppc.h. Also POWER9 and Endian fixes for vec_muludq, vec_mulluq. Added vec_mulhuq. Specifics follow: 1) Unit tests for vec_sraqi detected a regression in GCC 8 in the code generation for builtins vec_sr, vec_sra, vec_vsrd, vec_vsrad. This also failed unit tests for vec_srdi and vec_sradi. Bill Schmidt is working the problem in Upstream (GCC9) and will need a fix backported to GCC8 and AT12. 2) On testing the quadword multiplies (vec_muludq, vec_mulluq) were not endian stable for POWER9. Corrected that using endian stable merge and multiply doubleword operations. Also added vec_mulhuq to support multiplicative inverse over quadword integers. 3) Added new quadword operations; vec_absduq,vec_avguq, vec_maxsq, vec_maxuq, vec_minsq, vec_minuq, vec_mulhuq, vec_rlq, vec_rlqi, vec_sldqi, vec_sraq, and vec_sraqi.

 - Fixes and test case update for vec_int64_ppc.h A reccent compiler bug exposed a lack of test coverage for the shift/rotate doubleword immediate operations. So added four test variations to arith128_test_i64.c. Also added two compile tests for vec_srdi to vec_int64_dummy.c. This exposed a pasto bug in shift range check for vec_srdi.

 - Updates for white-space, capitalization, etc after review.

 - Update vec_int64_ppc.h and vec_int128_ppc.h after review. Mostly Doxygen text spelling/grammer and white space changes.

 - Text updates after Paul's review

 - Update vec_int64_ppc.h to fix vec_vpkudum for PWR7 case.

 - Update vec_int128_dummy.c fix for AT10/ P7/BE compile. Use vec_subudm for remainder.

 - Update vec_int128_dummy.c, vec_int32_dummy.c, vec_int64_dummy.c vec_pwr9_dummy.c Eyeball code gen for merge/multiply operations and example.

 - Update arith128_test_i64.c with need tests for merge/rotate/splat multiply high/low/even/odd. Endian sensitive. Fix whitespace.

 - Update vec_int64_ppc.h to add permute/merge/splat/swap. Additional examples in the \file doxygen text. Add vec_absdud, vec_maxsd, vec_maxud, vec_minsd, vec_minud, vec_mrgahd, vec_mrgald. Multiply doubleword in vec_int128_ppc.h vec_int128_ppc.h add doubleword multiply high/low/even/odd. Unit test in next update.

 - Update main doc for endian with examples.

 - Update vec_common_ppc.h to add byte const endian macros. Additional update to spelling and grammar based on Pauls review.

 - Update vec_char_ppc.h to add merge even/odd and multiply high Clean up after review.

 - Update vec_int16_ppc.h to add merge even/odd and multiply high Updates after review.

 - Update vec_common_ppc.h to add byte const endian macros. Plus additional examples in the \file doxygen text.

 - Disable debug prints in src/testsuite/arith128_test_char.c

 - Update vec_char_ppc.h to add merge even/odd and multiply high Plus additional examples in the \file doxygen text. Add latency info and clean up doxygent comment alignment. Add more unit tests to cover new functions.

 - Update vec_int16_ppc.h to add merge even/odd and multiply high Plus POWER9 absolute difference, additional examples in the \file doxygen text, and clean up doxygent comment alignment. Add more unit tests to cover new functions.

 - Update vec_int32_ppc.h to add merge even/odd and multiply high. Fixes due to review comments.

 - Update vec_int32_ppc.h to add merge even/odd and multiply high Plus POWER9 absolute difference, additional examples in the \file doxygen text, and clean up doxygen comment alignment. Add more unit tests to cover new functions.

 - Update vec_f64_ppc.h to use POWER9 test data class instructions. Fixes after POWER9 testing in mambo.

 - Update vec_f64_ppc.h to use POWER9 test date class instructions. After review.

 - Update vec_f64_ppc.h to use POWER9 test date class instructions. Simplify POWER8/POWER7 vector long int compares using pveclib functions. Add appropriate unit tests.

 - Update vec_f32_ppc.h to use POWER9 test date class instructions. Clean up after review.

 - Update vec_f32_ppc.h to use POWER9 test date class instructions. Fixes after power9 mambo testing.

 - Update vec_f32_ppc.h to use POWER9 test date class instructions.

 - Introduce vec_f128_ppc.h with initial __float128 support. Part3b minor updates after review.

 - Part3, Introduce vec_f128_ppc.h with initial __float128 support. Includes the Makefile.ac updates to install vec_f128_ppc.h and build/run the f128 specific unit tests. Also updated the \note on the \mainpage for current status.

 - Introduce vec_f128_ppc.h with initial __float128 support. Part2c updates after Paul's comments.

 - Introduce vec_f128_ppc.h with initial __float128 support. Part2b including Unit tests, and dummy compile sources. Oops needed 2 more files.

 - Introduce vec_f128_ppc.h with initial __float128 support. Part2 including Unit tests, and dummy compile sources.

 - Address review comments from Paul.

 - Remove back-level doxygen generated files from project.

 - Per issue #29, change the vec_int128_ppc.h to return vector bool for compares.

 - Per discussion in issue #29, this patch changes vec_int64_ppc.h compares to return the appropriate vector bool.

 - Introducing vec_f128_ppc.h with initial __float128 support. This will be in 3 parts. This is 1st part including vec_f128_ppc.h itself and require changes to doc/pveclib-doxygen-pveclib.doxy and src/vec_common_ppc.h.

 - Addtional small fixes for -mcpu=power9. After int128 commit. Found compile failures under _ARCH_PWR9 conditional due to wrong type. Found compile failures for GCC6 (AT10) as vec_cmpne is not defined. Runtime bad results due to early clobbler and reuse of carry VR in inline __asm.

 - Clean up in responce to Paul's review.

 - Part 3 add unit tests for new functions in vec_i128_ppc.h. Completes this update.

 - Updates following Paul's review.

 - Part 2 of vec_int128_ppc.h update. Includes minor comment/doxygen updates and supporting arith128_print changes. Additional int128 specific performance and trace tests. And new dummy examples for eyeballing the compiler generated code.

 - Simplify isnormal and variants implementation for f32/f64. While implementing vec_f128_ppc.h realized there was a simpler isnormal implmentation that did not require the vec_abs op.

 - Added quadword compares, cleaned up the documentation, for vec_i128_ppc.h. This part 1, unit and performance tests to follow.

 - Final Makefile and test driver changes to activate vec_f64_ppc.h install and make check.

 - Updated per Paul's review comments.

 - Add new unit tests for vec_f64_ppc.h

 - Update vec_f64_ppc.h with new functions and prepare to publish. This commit is just the f64 header and doxy INPUT. Test cases and configure.ac/Makefile.am changes will follow in part2/3.

 - Addtional changes after review.

 - Complete the compare intrinsics for vector long (doubleword integer). These are needed for the vec_f64_ppc.h vec_is* implementations that avoid Floating-point exceptions.

 - Additional clean up after Paul's comments.

 - Addtional changes and documentation for vec_f32_ppc.h

 - Adding the float 32-bit stuff with an attempt on performance metrics.

 - updates after Paul's review

 - Refine the dox introduction and add appropriate copyright and trademark noticies.

 - document updates based on Paul's review.

 - Complete the P9 int128 and multiply implementation.

 - Addressed review comments from Paul Clarke

 - Address review comments from Paul Clarke

 - Update and refactor the "dummy" files. Dummy files use the pveclib headers to generate short sequences of code that can be examined via opbdump. This used both to evaulate correctness and performance.

 - Enable the new test framework to test across the new function added for the new header files.

 - Quick fix, __FUNCTION__ does not work in quoted strings.

 - updates per comments from Paul Clarke (ThinkOpenly)

 - Refactor the unit tests by type and element size. Here integer types. Some of these tests have been where in arith128_test_i128.c and moved to the file with matching element size. Some test functions where given more descriptive as part of this.

 - Temporarily disable db_vec_mul10cuq for __ARCH_PWR9 until the rest of the infrastructure is in place. We want this patch to compile and run on Mambo power9.

 - Part 2 of the power9 update. Add changes to existing headers, specific to power9 and fix latent bugs in the quadword multiply. Add appropriate doxygen comments and reorder functions (where posible) so the function list is more (or less) in alphabetical order.

 - Change vector shift immediate description to be consistent across element sizes

 - More nits picked.

 - More nits fixed. s/Shift/shift/ again.

 - More nits fixed. No change to changelog.

 - Fixup after review. Same change log as before.

 - First chunk of the power9 integer enablement. Starting with new files, dependant changed to vec_common_ppc.h, and more fixes. These will not be activated until parts change Makefile.am.

 - Fix the doxygen docs: Add a complete introduction; laying out the rationale, issues, and plans. Reduce the noise from make doxygen-doc by adding doxygen \brief comment for every function, typedef, and macro. Correct the doxygen function descriptions so text and parms have the same alignment. To make the examples clearer, simplify the code by replacing some long in-line assembler sequences with equivalent built-in functions from altivec.h.

 - Add test cases for shift left/right quadword [immediate].

 - Add call to test_43 to the test driver. This runs specific tests for vector byte reverse.

 - Cleanup white space and some left over debug only conditionals.

 - For GCC 5/6 compiling arith128_test_i128.c (test_4b) with -g causes an ICE.  As this only effects older GCC vertions (GCC 7 and later are OK), and there are simple work arounds, the GCC maintainers do not think it is worth fixing.

 - ../COMMIT_EDITMSG-20180126

 - Delete commit.txt

 - README: minor formatting fixes

 - Update README with standard build procedure

 - Disable Autoconf maintainer mode

 - Add missing files

 - Fix testsuite build

 - Ignore more files

 - Fix whitespace issues

 - some fixes for doxygen support

 - add/update project files

 - Initial commit

 - Initial commit
