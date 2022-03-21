# The CORE-MATH project

CORE-MATH Mission: provide on-the-shelf open-source mathematical
functions with correct rounding that can be integrated into current
mathematical libraries (GNU libc, Intel Math Library, AMD Libm,
Newlib, OpenLibm, Musl, Apple Libm, llvm-libc, CUDA libm, ROCm).

Homepage: https://core-math.gitlabpages.inria.fr/


## Quick guide

### Exhaustive checks

To run an exhaustive check of a single-precision single-argument
function, run:

    ./check.sh --exhaustive [rounding_modes] $FUN

where:
- `$FUN` can be `acosf`, `asinf`, etc.
- `[rounding_modes]` can be a selection of `--rndn` (round to
  nearest), `--rndz` (toward zero), `--rndu` (upwards), `--rndd`
  (downwards). The default is all four.

This command is sensitive to the following environment variables:
- `CC`
- `CFLAGS`
- OpenMP variables such as `OMP_NUM_THREADS`

Note: on Debian, you need the libomp-dev package to use clang.

### Worst case checks

These checks are available for bivariate single-precision functions,
and double-precision functions.

    ./check.sh --worst [rounding_modes] $FUN

### Special checks

These checks are available for functions where some interesting worst
cases can be automatically (and cheaply) computed (at the time of
writing, only `hypot`).

    ./check.sh --special [rounding_modes] $FUN

### Performance measurements

Performance measurement scripts rely on the Linux perf framework. You
might need to run the command:

    echo -1 > /proc/sys/kernel/perf_event_paranoid

as root before running the following commands.

To evaluate the reciprocal throughput of some function, run:

    ./perf.sh acosf

It outputs two numbers: the performance of core-math, and the one the
libc, in cycles/call. You can also evaluate the performance of some
other libm (given by a static .a archive, typically llvmlibc), by
setting the `LIBM` environment variable to the absolute path of the .a
file. In the case, `./perf.sh` will output three numbers: the
performances of core-math, standard libm, given libm.

To evaluate performance of all supported functions, run:

    ./perf-all.sh

You can also set the `PERF_ARGS` environment variable to `--latency`
to get latency instead of reciprocal throughput.


## Layout

Each function `$NAME` has a dedicated directory
`src/$TYPE/$SHORT_NAME`, where `$TYPE` can be `binary{32,64,80,128}`
and `$SHORT_NAME` is the function name without its type suffix
(`acos`, `asin`, etc.). This directory contains the following files:
- `$NAME.c`: a standalone implementation of function `cr_$NAME`
- other support files


## How to add support for a new function?

You can take inspiration from a similar function of the same bitsize
and/or arity.

For example, suppose you want to add support for a binary64 bivariate
function `foo`. In `src/binary64/foo`, you will need the following
files:
- `foo.c`: defining `cr_foo`
- `foo.wc`: the worst cases to be tested
- `foo_mpfr.c`: defining `ref_foo`, using MPFR
- `Makefile`: defining `FUNCTION_UNDER_TEST`, and including `../support/Makefile.bivariate`
- `function_under_test.h`: defining `{cr,ref}_function_under_test`

In addition, in `src/generic/foo`, you will need the following file:
- `random_under_test.h`: defining a function `random_under_test`,
  which samples suitable inputs for `foo`
