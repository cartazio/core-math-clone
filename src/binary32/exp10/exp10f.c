/* Correctly-rounded 10^x function for binary32 value.

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
typedef union {double f; uint64_t u;} b64u64_u;

float cr_exp10f(float x){
  static const double c[] =
    {0x1.62e42fefa398bp-5, 0x1.ebfbdff84555ap-11, 0x1.c6b08d4ad86d3p-17,
     0x1.3b2ad1b1716a2p-23, 0x1.5d7472718ce9dp-30, 0x1.4a1d7f457ac56p-37};
  static const double tb[] =
    {0x1p+0, 0x1.0b5586cf9890fp+0, 0x1.172b83c7d517bp+0, 0x1.2387a6e756238p+0,
     0x1.306fe0a31b715p+0, 0x1.3dea64c123422p+0, 0x1.4bfdad5362a27p+0, 0x1.5ab07dd485429p+0,
     0x1.6a09e667f3bcdp+0, 0x1.7a11473eb0187p+0, 0x1.8ace5422aa0dbp+0, 0x1.9c49182a3f09p+0,
     0x1.ae89f995ad3adp+0, 0x1.c199bdd85529cp+0, 0x1.d5818dcfba487p+0, 0x1.ea4afa2a490dap+0};
  const double iln2h = 0x1.a934f098p+1*16, iln2l = -0x1.9723a81p-33*16;
  b32u32_u t = {.f = x};
  double z = x;
  uint32_t ux = t.u, ex = (ux>>23)&0xff;
  if (__builtin_expect(ex>(127+6), 0)){
    if(ex==0xff) {
      if(ux<<9) return x; // nan
      static const float ir[] = {__builtin_inff(), 0.0f};
      return ir[ux>>31]; // +-inf
    }
    static const float q[][2] = {{0x1.fffffep127f,0x1.fffffep127f}, {0x1.fffffep-126f,0x1.fffffep-126f}};
    return q[ux>>31][0]*q[ux>>31][1];
  }
  if (__builtin_expect(ex<(127-27), 0)){
    return 1.0f + x;
  } else {
    static const struct {union{float arg; uint32_t uarg;}; float rh, rl;} st[] = {
      {{-0x1.4de862p+3}, 0x1.435996p-35f, -0x1.fffffep-60f},
    };
    if(__builtin_expect(ux == st[0].uarg, 0)) return st[0].rh + st[0].rl;
    if(__builtin_expect(!(ux<<12), 0)){
      int k = (ux >> 20) - 1016;
      if(__builtin_expect(k>=0 && k <= 26, 0)){
	if(k== 0) return 10.0f;
	if(k== 8) return 100.0f;
	if(k==12) return 1000.0f;
	if(k==16) return 10000.0f;
	if(k==18) return 100000.0f;
	if(k==20) return 1000000.0f;
	if(k==22) return 10000000.0f;
	if(k==24) return 100000000.0f;
	if(k==25) return 1000000000.0f;
	if(k==26) return 10000000000.0f;
      }
    }
    double a = iln2h*z, ia = __builtin_floor(a), h = (a - ia) + iln2l*z;
    long i = ia, j = i&0xf, e = i - j;
    e >>= 4;
    double s = tb[j];
    b64u64_u su = {.u = (e + 0x3fful)<<52};
    s *= su.f;
    double h2 = h*h;
    double c0 = c[0] + h*c[1];
    double c2 = c[2] + h*c[3];
    double c4 = c[4] + h*c[5];
    c0 += h2*(c2 + h2*c4);
    double w = s*h;
    return s + w*c0;
  }
}