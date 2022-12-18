/* Correctly-rounded half-revolution arc-tangent of binary32 value.

Copyright (c) 2022 Alexei Sibidanov.

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

#include <stdint.h>

typedef union {float f; uint32_t u;} b32u32_u;

float cr_atanpif(float x){
  b32u32_u t = {.f = x};
  int32_t e = (t.u>>23)&0xff, gt = e>=127;
  if(__builtin_expect(e>127+24, 0)) {
    float f = __builtin_copysignf(0.5f, x);
    if(__builtin_expect(e==0xff, 0)) {
      if(t.u<<9) return x; // nan
      return f; // inf
    }
    return f - 0x1.45f306p-2f/x;
  }
  double z = x;
  if (__builtin_expect(e<127-13, 0)){
    double sx = z*0x1.45f306dc9c883p-2;
    if (__builtin_expect(e<127-25, 0)) return sx;
    return sx - (0x1.5555555555555p-2*sx)*(x*x);
  }
  unsigned ax = t.u&(~0u>>1);
  if(__builtin_expect(ax == 0x3fa267ddu, 0)) return __builtin_copysignf(0x1.267004p-2f,x) - __builtin_copysignf(0x1p-55f,x);
  if(__builtin_expect(ax == 0x3f693531u, 0)) return __builtin_copysignf(0x1.e1a662p-3f,x) + __builtin_copysignf(0x1p-28f,x);
  if(__builtin_expect(ax == 0x3f800000u, 0)) return __builtin_copysignf(0x1p-2f,x);
  if(gt) z = 1/z;
  double z2 = z*z, z4 = z2*z2, z8 = z4*z4;
  static const double cn[] =
    {0x1.45f306dc9c882p-2, 0x1.733b561bc23d5p-1, 0x1.28d9805bdfbf2p-1,
     0x1.8c3ba966ae287p-3, 0x1.94a7f81ee634bp-6, 0x1.a6bbf6127a6dfp-11};
  static const double cd[] =
    {0x1p+0, 0x1.4e3b3ecc2518fp+1, 0x1.3ef4a360ff063p+1, 0x1.0f1dc55bad551p+0,
     0x1.8da0fecc018a4p-3, 0x1.8fa87803776bfp-7, 0x1.dadf2ca0acb43p-14};
  double cn0 = cn[0] + z2*cn[1];
  double cn2 = cn[2] + z2*cn[3];
  double cn4 = cn[4] + z2*cn[5];
  cn0 += z4*cn2;
  cn0 += z8*cn4;
  cn0 *= z;
  double cd0 = cd[0] + z2*cd[1];
  double cd2 = cd[2] + z2*cd[3];
  double cd4 = cd[4] + z2*cd[5];
  double cd6 = cd[6];
  cd0 += z4*cd2;
  cd4 += z4*cd6;
  cd0 += z8*cd4;
  double r = cn0/cd0;
  if (gt) r = __builtin_copysign(0.5, z) - r;
  return r;
}

/* just to compile since glibc does not contain this function*/
float atanpif(float x){
  return cr_atanpif(x);
}
