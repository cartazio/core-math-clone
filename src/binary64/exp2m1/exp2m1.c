/* Correctly-rounded exp2m1 function for binary64 value.

Copyright (c) 2022-2023 Paul Zimmermann, Tom Hubrecht and Claude-Pierre Jeannerod

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
SOFTWARE.
*/

#include <stdio.h>
#include <stdint.h>
#include <math.h>

/****************** code copied from pow.[ch] ********************************/

// Multiply exactly a and b, such that *hi + *lo = a * b.
static inline void a_mul(double *hi, double *lo, double a, double b) {
  *hi = a * b;
  *lo = __builtin_fma (a, b, -*hi);
}

/****************** end of code copied from pow.[ch] *************************/

typedef union {double f; uint64_t u;} b64u64_u;

/* |x| <= 0.125, put in h + l a double-double approximation of exp2m1(x),
   and return the maximal corresponding absolute error.
   We also have |x| > 0x1.0527dbd87e24dp-51. */
static double
exp2m1_fast_tiny (double *h, double *l, double x)
{
  /* The maximal value of |P_fast_tiny[4]*x^4/expm1(x)| over [-0.125,0.125]
     is less than 2^-13.495, thus we can compute the coefficients of degree
     4 or higher using double precision only. */
  double x2 = x * x, x4 = x2 * x2;
  double c10 = __builtin_fma (P[11], x, P[10]);
  double c8 = __builtin_fma (P[9], x, P[8]);
  double c6 = __builtin_fma (P[7], x, P[6]);
  double c4 = __builtin_fma (P[5], x, P[4]);
  c8 = __builtin_fma (c10, x2, c8);
  c4 = __builtin_fma (c6, x2, c4);
  c4 = __builtin_fma (c8, x4, c4);
  double t;
  // multiply c4 by x and add P[3]
  a_mul (h, l, c4, x);
  fast_two_sum (h, &t, P[3], *h);
  *l += t;
  // multiply (h,l) by x and add P[2]
  s_mul (h, l, x, *h, *l);
  fast_two_sum (h, &t, P[2], *h);
  *l += t;
  // multiply (h,l) by x and add P[1]
  s_mul (h, l, x, *h, *l);
  fast_two_sum (h, &t, P[1], *h);
  *l += t;
  // multiply (h,l) by x
  s_mul (h, l, x, *h, *l);
  return 0x1.d4p-65 * *h; // 2^-64.13 < 0x1.d4p-65
}

/* Given -54 < x < -0x1.0527dbd87e24dp-51 or
   0x1.0527dbd87e24dp-51 < x < 1024, put in h + l a
   double-double approximation of exp2m1(x), and return the maximal
   corresponding absolute error.
   The input tiny is true iff |x| <= 0.125. */
static double
exp2m1_fast (double *h, double *l, double x, int tiny)
{
  if (tiny) // |x| <= 0.125
    return exp2m1_fast_tiny (h, l, x);

  return -2;
}

double
cr_exp2m1 (double x)
{
  b64u64_u t = {.f = x};
  uint64_t ux = t.u, ax = ux & 0x7ffffffffffffffflu;

  if (__builtin_expect (ux >= 0xc04b000000000000, 0))
  {
    // x = -NaN or x <= -54
    if ((ux >> 52) == 0xfff) // -NaN or -Inf
      return (ux > 0xfff0000000000000lu) ? x : -1.0;
    // for x <= -54, exp2m1(x) rounds to -1 to nearest
    return -1.0 + 0x1p-54;
  }
  else if (__builtin_expect (ax >= 0x4090000000000000, 0))
  {
    // x = +NaN or x >= 1024
    if ((ux >> 52) == 0x7ff) // +NaN
      return x;
    // for x >= 1024, exp2m1(x) rounds to +Inf to nearest
    return 0x1.fffffffffffffp+1023 * x;
  }
  else if (ax <= 0x3cc0527dbd87e24d) // |x| <= 0x1.0527dbd87e24dp-51
    /* then the second term of the Taylor expansion of 2^x-1 at x=0 is
       smaller in absolute value than 1/2 ulp(first term):
       log(2)*x + log(2)^2*x^2/2 + ... */
  {
    static const double LN2H = 0x1.62e42fefa39efp-1;
    static const double LN2L = 0x1.abc9e3b39803fp-56;
    double h, l;
    /* we use special code when log(2)*|x| < 2^-968, in which case
       the double-double approximation h+l has its lower part l
       "truncated" */
    if (ax <= 0x3771547652b82fe) // |x| <= 0x1.71547652b82fep-968
    {
      // special case for 0
      if (x == 0)
        return x;
      // scale x by 2^53
      x = x * 0x1p53;
      a_mul (&h, &l, LN2H, x);
      l = __builtin_fma (LN2L, x, l);
      double h2 = h + l; // round to 53-bit precision
      // scale back, hoping to avoid double rounding
      h2 = h2 * 0x1p-53;
      // now subtract back h2 * 2^53 from h to get the correction term
      h = __builtin_fma (-h2, 0x1p53, h);
      // add l
      h += l;
      // add h2 + h * 2^-53
      return __builtin_fma (h, 0x1p-53, h2);
    }
    else // 0x1.71547652b82fep-968 < |x| <= 0x1.0527dbd87e24dp-51
    {
      /* the 2nd term of the Taylor expansion of 2^x-1 at x=0 is
         log(2)^2/2*x^2 */
      static const double C2 = 0x1.ebfbdff82c58fp-3; // log(2)^2/2
      a_mul (&h, &l, LN2H, x);
      l = __builtin_fma (LN2L, x, l);
      /* we add C2*x^2 last, so that in case there is a cancellation in
         LN2L*x+l, it will contribute more bits */
      l += C2 * x * x;
      return h + l;
    }
  }

  /* now -54 < x < -0x1.0527dbd87e24dp-51
     or 0x1.0527dbd87e24dp-51 < x < 1024 */

  double err, h, l;
  err = exp2m1_fast (&h, &l, x, ax <= 0x3fc0000000000000lu);
  double left = h + (l - err);
  double right = h + (l + err);
  if (left == right)
    return left;

  return -2;
}
