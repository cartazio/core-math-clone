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

// Multiply a double with a double double : a * (bh + bl)
static inline void s_mul (double *hi, double *lo, double a, double bh,
                          double bl) {
  a_mul (hi, lo, a, bh); /* exact */
  *lo = __builtin_fma (a, bl, *lo);
}

// Add a + b, assuming |a| >= |b|
static inline void
fast_two_sum(double *hi, double *lo, double a, double b)
{
  double e;

  *hi = a + b;
  e = *hi - a; /* exact */
  *lo = b - e; /* exact */
}

/****************** end of code copied from pow.[ch] *************************/

typedef union {double f; uint64_t u;} b64u64_u;

/* The following is a degree-12 polynomial generated by Sollya
   (file exp2m1_fast.sollya),
   which approximates exp2m1(x) with relative error bounded by 2^-68.559
   for |x| <= 0.125. */
static const double P[] = {
  0x1.62e42fefa39efp-1, 0x1.abd1697afcaf8p-56,  // degree 1, P[0], P[1]
  0x1.ebfbdff82c58fp-3, -0x1.5e5a1d09e1599p-57, // degree 2, P[2], P[3]
  0x1.c6b08d704a0bfp-5,  // degree 3, P[4]
  0x1.3b2ab6fba4e78p-7,  // degree 4, P[5]
  0x1.5d87fe78a84e6p-10, // degree 5, P[6]
  0x1.430912f86a48p-13,  // degree 6, P[7]
  0x1.ffcbfbc1f2b36p-17, // degree 7, P[8]
  0x1.62c0226c7f6d1p-20, // degree 8, P[9]
  0x1.b539529819e63p-24, // degree 9, P[10]
  0x1.e4d552bed5b9cp-28, // degree 10, P[11]
};

/* |x| <= 0.125, put in h + l a double-double approximation of exp2m1(x),
   and return the maximal corresponding absolute error.
   We also have |x| > 0x1.0527dbd87e24dp-51.
   With xmin=RR("0x1.0527dbd87e24dp-51",16), the routine
   exp2m1_fast_tiny_all(xmin,0.125,2^-65.63) in exp2m1.sage returns
   1.74952121608842e-20 < 2^-65.63, and
   exp2m1_fast_tiny_all(-0.125,-xmin,2^-65.54) returns
   1.86402194391062e-20 < 2^-65.54, which proves the relative
   error is bounded by 2^-65.54. */
static double
exp2m1_fast_tiny (double *h, double *l, double x)
{
  /* The maximal value of |P[4]*x^4/exp2m1(x)| over [-0.125,0.125]
     is less than 2^-15.109, thus we can compute the coefficients of degree
     4 or higher using double precision only. */
  double x2 = x * x, x4 = x2 * x2;
  double c8 = __builtin_fma (P[10], x, P[9]); // degree 8
  double c6 = __builtin_fma (P[8], x, P[7]);  // degree 6
  double c4 = __builtin_fma (P[6], x, P[5]);  // degree 4
  c8 = __builtin_fma (P[11], x2, c8);         // degree 8
  c4 = __builtin_fma (c6, x2, c4);            // degree 4
  c4 = __builtin_fma (c8, x4, c4);            // degree 4
  double t;
  // multiply c4 by x and add P[4]
  a_mul (h, l, c4, x);
  fast_two_sum (h, &t, P[4], *h);
  *l += t;
  // multiply (h,l) by x and add P[2]+P[3]
  s_mul (h, l, x, *h, *l);
  fast_two_sum (h, &t, P[2], *h);
  *l += t + P[3];
  // multiply (h,l) by x and add P[0]+P[1]
  s_mul (h, l, x, *h, *l);
  fast_two_sum (h, &t, P[0], *h);
  *l += t + P[1];
  // multiply (h,l) by x
  s_mul (h, l, x, *h, *l);
  return 0x1.61p-66 * *h; // 2^-65.54 < 0x1.61p-66
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

  *h = -2;
  *l = 0;
  return 0;
}

/* The following is a degree-15 polynomial generated by Sollya
   (file exp2m1_accurate.sollya),
   which approximates exp2m1(x) with relative error bounded by 2^-107.666
   for |x| <= 0.125. */
static const double Q[] = {
  0x1.62e42fefa39efp-1, 0x1.abc9e3b39804p-56,    // degree 1: Q[0], Q[1]
  0x1.ebfbdff82c58fp-3, -0x1.5e43a53e44dcfp-57,  // degree 2: Q[2], Q[3]
  0x1.c6b08d704a0cp-5, -0x1.d331627517168p-59,   // degree 3: Q[4], Q[5]
  0x1.3b2ab6fba4e77p-7, 0x1.4e65df0779f8cp-62,   // degree 4: Q[6], Q[7]
  0x1.5d87fe78a6731p-10, 0x1.0717fbf4bd05p-66,   // degree 5: Q[8], Q[9]
  0x1.430912f86c787p-13, 0x1.bd2bdec9bcd42p-67,  // degree 6: Q[10], Q[11]
  0x1.ffcbfc588b0c7p-17, -0x1.e60aa6d5e4aa9p-71, // degree 7: Q[12], Q[13]
  0x1.62c0223a5c824p-20,                         // degree 8: Q[14]
  0x1.b5253d395e7d4p-24,                         // degree 9: Q[15]
  0x1.e4cf5158b916p-28,                          // degree 10: Q[16]
  0x1.e8cac734c6058p-32,                         // degree 11: Q[17]
  0x1.c3bd64f17199dp-36,                         // degree 12: Q[18]
  0x1.8161a17e05651p-40,                         // degree 13: Q[19]
  0x1.3150b3d792231p-44,                         // degree 14: Q[20]
  0x1.c184260bfad7ep-49,                         // degree 15: Q[21]
};

/* Accurate path for 0x1.0527dbd87e24dp-51 < |x| <= 0.125. */
static double
exp2m1_accurate_tiny (double x)
{
#define EXCEPTIONS 28
  static const double exceptions[EXCEPTIONS][3] = {
    {-0x1.3b6203ff2cbe6p-29, -0x1.b536a7dace8cap-30, 0x1.07bb47158694fp-137},
    {-0x1.66ff2474821a7p-44, -0x1.f1accede78f51p-45, -0x1.fffffffffffffp-99},
    {-0x1.0f1c08e43f217p-20, -0x1.77d66368613b4p-21, 0x1.0d3de71c269fcp-130},
    {-0x1.ba24ff5dea796p-34, -0x1.3278a26ed0162p-34, 0x1.f837135341102p-142},
    {-0x1.41e4bec9bc547p-19, -0x1.be3d238468f35p-20, -0x1.fffffffffffffp-74},
    {-0x1.bfb3efcdf2bc4p-10, -0x1.3623eff91de91p-10, 0x1.cd506bd8d0439p-117},
    {-0x1.9b28778a9a8a3p-4, -0x1.134dec3a6324bp-4, -0x1.1acaaac7e1ff7p-110},
    {-0x1.922076a30742p-24, -0x1.16bba98a001c1p-24, 0x1.d90eddeada3e4p-133},
    {-0x1.e07a2fbc7fd84p-30, -0x1.4d0a9e634dc35p-30, -0x1.fffffffffffffp-84},
    {-0x1.8288f6bb77d79p-23, -0x1.0becf6ad924bfp-23, -0x1.fffffffffffffp-77},
    {-0x1.d091d774b141ep-35, -0x1.4203e2685b069p-35, -0x1.a166cdf05ff79p-143},
    {-0x1.f6ec73d3948c3p-4, -0x1.4e2d8b0cead45p-4, -0x1.40f3d5244acffp-109},
    {-0x1.6f94484e5e1fdp-5, -0x1.f5baee010ccc6p-6, -0x1.3298bcde4f9a8p-115},
    {-0x1.3918e8608bd5bp-8, -0x1.b153bf52832f9p-9, -0x1.fffffffffffffp-63},
    {-0x1.a6d6a49f2187fp-50, -0x1.2516dafdf17adp-50, -0x1.85a87dc0b88p-157},
    {-0x1.daaead688f65fp-45, -0x1.4906541fb8d5cp-45, -0x1.31de54e6dc4ep-151},
    {-0x1.bfc1e9b0f73aep-35, -0x1.365ca0d933491p-35, -0x1.fffffffffffffp-89},
    {-0x1.98aae9950914ep-19, -0x1.1b443a4805f8fp-19, 0x1.fffffffffffffp-73},
    {-0x1.0d296993d1368p-20, -0x1.752326c780f68p-21, 0x1.1cade8dbbd5d8p-130},
    {-0x1.7b388eb924102p-18, -0x1.06dafba21afffp-18, 0x1.fffffffffffffp-72},
    {-0x1.3b6786a5a9a69p-17, -0x1.b53dee1a96ca4p-18, -0x1.282e0781f97f5p-124},
    {-0x1.8474969f5eb14p-9, -0x1.0cfafc07a957bp-9, -0x1.fffffffffffffp-63},
    {-0x1.a1f242e670d73p-42, -0x1.21b2c54479c4dp-42, 0x1.48a5d173822e8p-150},
    {-0x1.3c7971b0ee205p-14, -0x1.b6b716c4bb87cp-15, 0x1.d7150e84d973dp-121},
    {-0x1.0867d8153350dp-9, -0x1.6e49b44e387f5p-10, -0x1.fffffffffffffp-64},
    {-0x1.0c3ebbd1a501fp-11, -0x1.73ccf8ee62819p-12, 0x1.3a3965a926c6dp-120},
    {-0x1.0ee8225c19555p-23, -0x1.778e77e8b1e13p-24, -0x1.3da14df43e675p-129},
    {-0x1.3dbf41403c0b2p-15, -0x1.b87c37192e4f7p-16, 0x1.098861b427b13p-123},
  };
  for (int i = 0; i < EXCEPTIONS; i++)
    if (x == exceptions[i][0])
      return exceptions[i][1] + exceptions[i][2];
#undef EXCEPTIONS

  double h, l, t;
  double x2 = x * x, x4 = x2 * x2;
  double c13 = __builtin_fma (Q[20], x, Q[19]); // degree 13
  double c11 = __builtin_fma (Q[18], x, Q[17]); // degree 11
  c13 = __builtin_fma (Q[21], x2, c13);         // degree 13
  // add Q[16]*x+c11*x2+c13*x4 to Q[15] (degree 9)
  fast_two_sum (&h, &l, Q[15], Q[16] * x + c11 * x2 + c13 * x4);
  // multiply h+l by x and add Q[14] (degree 8)
  s_mul (&h, &l, x, h, l);
  fast_two_sum (&h, &t, Q[14], h);
  l += t;
  // multiply h+l by x and add Q[12]+Q[13] (degree 7)
  s_mul (&h, &l, x, h, l);
  fast_two_sum (&h, &t, Q[12], h);
  l += t + Q[13];
  // multiply h+l by x and add Q[10]+Q[11] (degree 6)
  s_mul (&h, &l, x, h, l);
  fast_two_sum (&h, &t, Q[10], h);
  l += t + Q[11];
  // multiply h+l by x and add Q[8]+Q[9] (degree 5)
  s_mul (&h, &l, x, h, l);
  fast_two_sum (&h, &t, Q[8], h);
  l += t + Q[9];
  // multiply h+l by x and add Q[6]+Q[7] (degree 4)
  s_mul (&h, &l, x, h, l);
  fast_two_sum (&h, &t, Q[6], h);
  l += t + Q[7];
  // multiply h+l by x and add Q[4]+Q[5] (degree 3)
  s_mul (&h, &l, x, h, l);
  fast_two_sum (&h, &t, Q[4], h);
  l += t + Q[5];
  // multiply h+l by x and add Q[2]+Q[3] (degree 2)
  s_mul (&h, &l, x, h, l);
  fast_two_sum (&h, &t, Q[2], h);
  l += t + Q[3];
  // multiply h+l by x and add Q[0]+Q[1] (degree 2)
  s_mul (&h, &l, x, h, l);
  fast_two_sum (&h, &t, Q[0], h);
  l += t + Q[1];
  // multiply h+l by x
  s_mul (&h, &l, x, h, l);
  return h + l;
}

static double exp2m1_accurate (double x)
{
  b64u64_u t = {.f = x};
  uint64_t ux = t.u, ax = ux & 0x7ffffffffffffffflu;

  if (ax <= 0x3fc0000000000000lu) // |x| <= 0.125
    return exp2m1_accurate_tiny (x);

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
#define EXCEPTIONS 6
      static const double exceptions[EXCEPTIONS][3] = {
        {-0x1.d8bedc057858cp-65, -0x1.47aea7608c02bp-65, -0x1.ef8a5d5p-172},
        {-0x1.ec44ae4bc644p-74, -0x1.5536e12eb7335p-74, -0x1.fb8acp-181},
        {-0x1.f1bc3ef3e6f36p-65, -0x1.5900fbf46981dp-65, -0x1.fffffffffffffp-119},
        {-0x1.a16826a8e825dp-56, -0x1.21530a306cc85p-56, -0x1.38ac4a67cep-161},
        {-0x1.8c525b64ed08ep-59, -0x1.12b592f889516p-59, -0x1.7b71d1eep-169},
        {-0x1.bacdbd3005cd7p-60, -0x1.32ed98e196cf5p-60, -0x1.9b32a24b8p-166},
      };
      for (int i = 0; i < EXCEPTIONS; i++)
        if (x == exceptions[i][0])
          return exceptions[i][1] + exceptions[i][2];
#undef EXCEPTIONS
      /* the 2nd term of the Taylor expansion of 2^x-1 at x=0 is
         log(2)^2/2*x^2 */
      static const double C2 = 0x1.ebfbdff82c58fp-3; // log(2)^2/2
      a_mul (&h, &l, LN2H, x);
      l = __builtin_fma (LN2L, x, l);
      /* we add C2*x^2 last, so that in case there is a cancellation in
         LN2L*x+l, it will contribute more bits */
      l += C2 * x * x;
      if (x == -0x1.f1bc3ef3e6f36p-65) printf ("h=%la l=%la\n", h, l);
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

  return exp2m1_accurate (x);
}

// fake function as long as GNU libc does not provide it
double exp2m1 (double x)
{
  return exp (x) - 1.0;
}
