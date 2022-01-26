# The CORE-MATH project

CORE-MATH Mission: provide on-the-shelf open-source mathematical
functions with correct rounding that can be integrated into current
mathematical libraries (GNU libc, Intel Math Library, AMD Libm,
Newlib, OpenLibm, Musl, Apple Libm, llvm-libc, CUDA libm, ROCm).

Homepage: https://core-math.gitlabpages.inria.fr/


## Quick guide

To run an exhaustive check of a single-precision single-argument
function, run:

    ./check.sh --exhaustive $FUN [rounding_mode]

where:
- `$FUN` can be `acosf`, `asinf`, etc.
- `[rounding_mode]` can be `-rndn`, `-rndz`, `-rndu`, `-rndd`

This command is sensitive to the following environment variables:
- `CC`
- `CFLAGS`


## Layout

Each function `$NAME` has a dedicated directory
`src/$TYPE/$SHORT_NAME`, where `$TYPE` can be `binary{32,64,80,128}`
and `$SHORT_NAME` is the function name without its type suffix
(`acos`, `asin`, etc.). This directory contains the following files:
- `$NAME.c`: a standalone implementation of function `cr_$NAME`
- other support files
