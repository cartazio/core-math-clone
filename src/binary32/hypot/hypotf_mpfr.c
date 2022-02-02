/* Correctly-rounded Euclidean distance function (hypot) for binary32 values.

Copyright (c) 2022 Stéphane Glondu, Inria.

This file is part of the CORE-MATH project
(https://core-math.gitlabpages.inria.fr/).

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */

#include <mpfr.h>
#include "fenv_mpfr.h"

/* reference code using MPFR */
float
ref_hypot (float x, float y)
{
  mpfr_t xm, ym, zm;
  mpfr_init2 (xm, 24);
  mpfr_init2 (ym, 24);
  mpfr_init2 (zm, 24);
  mpfr_set_flt (xm, x, MPFR_RNDN);
  mpfr_set_flt (ym, y, MPFR_RNDN);
  int inex = mpfr_hypot (zm, xm, ym, rnd2[rnd]);
  mpfr_subnormalize (zm, inex, rnd2[rnd]);
  float ret = mpfr_get_flt (zm, MPFR_RNDN);
  mpfr_clear (xm);
  mpfr_clear (ym);
  mpfr_clear (zm);
  return ret;
}
