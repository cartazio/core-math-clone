/* Correctly-rounded base-10 exponent function biased by 1 for binary32 value.

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

float cr_exp10m1f(float x){
  static const double c[] =
    {0x1.62e42fefa398bp-5, 0x1.ebfbdff84555ap-11, 0x1.c6b08d4ad86d3p-17,
     0x1.3b2ad1b1716a2p-23, 0x1.5d7472718ce9dp-30, 0x1.4a1d7f457ac56p-37};
  static const double tb[] =
    {0x1p+0, 0x1.0b5586cf9890fp+0, 0x1.172b83c7d517bp+0, 0x1.2387a6e756238p+0,
     0x1.306fe0a31b715p+0, 0x1.3dea64c123422p+0, 0x1.4bfdad5362a27p+0, 0x1.5ab07dd485429p+0,
     0x1.6a09e667f3bcdp+0, 0x1.7a11473eb0187p+0, 0x1.8ace5422aa0dap+0, 0x1.9c49182a3f09p+0,
     0x1.ae89f995ad3adp+0, 0x1.c199bdd85529cp+0, 0x1.d5818dcfba487p+0, 0x1.ea4afa2a490dap+0};
  static const float q[][2] = {{0x1.fffffep127f, 0x1.fffffep127f}, {-1.0f, 0x1p-26f}};
  const double iln10h = 0x1.a934f09p+1*16, iln10l = 0x1.e68dc57f2496p-29*16;
  b32u32_u t = {.f = x};
  double z = x;
  uint32_t ux = t.u, ax = ux&(~0u>>1);
  if(__builtin_expect(ux>0xc0f0d2f1u, 0)){ // x < -7.52575
    if(ax>(0xffu<<23)) return x; // nan
    return q[1][0] + q[1][1];
  } else if(__builtin_expect(ax>0x421a209bu, 0)){  // x > 38.5318
    if(ax>(0xffu<<23)) return x; // nan
    return q[0][0] + q[0][1];
  } else if (__builtin_expect(ax<0x3d89c604u, 0)){ // |x| < 0.1549/log(10)
    double z2 = z*z, r;
    if (__builtin_expect(ax<0x3d1622fbu, 0)){ // |x| < 8.44e-2/log(10)
      if (__builtin_expect(ax<0x3c8b76a3u, 0)){ // |x| < 3.92e-2/log(10)
	if (__builtin_expect(ax<0x3bcced04u, 0)){ // |x| < 1.44e-2/log(10)
	  if (__builtin_expect(ax<0x3acf33ebu, 0)){ // |x| < 3.64e-3/log(10)
	    if (__builtin_expect(ax<0x395a966bu, 0)){ // |x| < 4.8e-4/log(10)
	      if (__builtin_expect(ax<0x36fe4a4bu, 0)){ // |x| < 1.745e-5/log(10)
		if (__builtin_expect(ax<0x32407f39u, 0)){ // |x| < 2.58e-8/log(10)
		  if (__builtin_expect(ax<0x245e5bd9u, 0)){ // |x| < 4.82164e-17
		    r = 0x1.26bb1bbb55516p+1;
		  } else {
		    if (__builtin_expect(ux == 0x2c994b7bu, 0)) return 0x1.60f974p-37f - 0x1p-90f;
		    r = 0x1.26bb1bbb55516p+1 + z * 0x1.53524c73cea69p+1;
		  }
		} else {
		  if (__builtin_expect(ux == 0xb6fa215bu, 0)) return -0x1.1ff87ep-16 + 0x1p-68;
		  r = 0x1.26bb1bbb55516p+1 + z * (0x1.53524c73ea62fp+1 + z * 0x1.0470591de2c75p+1);
		}
	      } else {
		static const double c[] =
		  {0x1.26bb1bbb55515p+1, 0x1.53524c73cea69p+1, 0x1.0470595038cc2p+1, 0x1.2bd7609fe1561p+0};
		r = (c[0] + z * c[1]) + z2 * (c[2] + z * c[3]);
	      }
	    } else {
	      static const double c[] =
		{0x1.26bb1bbb55516p+1, 0x1.53524c73ce6dbp+1, 0x1.0470591de3024p+1, 0x1.2bd76b79060e6p+0,
		 0x1.1429ffd3a963dp-1};
	      r = (c[0] + z * c[1]) + z2 * (c[2] + z * (c[3] + z * c[4]));
	    }
	  } else {
	    static const double c[] =
	      {0x1.26bb1bbb55516p+1, 0x1.53524c73cea67p+1, 0x1.0470591dc2953p+1, 0x1.2bd760a004d64p+0,
	       0x1.142a85da6f072p-1, 0x1.a7ed70725b00ep-3};
	    r = (c[0] + z * c[1]) + z2 * ((c[2] + z * c[3]) + z2 * (c[4] + z * c[5]));
	  }
	} else {
	  static const double c[] =
	    {0x1.26bb1bbb55516p+1, 0x1.53524c73ceadep+1, 0x1.0470591de2bb4p+1, 0x1.2bd76099a9d33p+0,
	     0x1.1429ffd829b0bp-1, 0x1.a7f2a6a0f7dc8p-3, 0x1.16e4dfbce0f56p-4};
	  r = (c[0] + z * c[1]) + z2 * ((c[2] + z * c[3]) + z2 * (c[4] + z * (c[5] + z * c[6])));
	}
      } else {
	static const double c[] =
	  {0x1.26bb1bbb55515p+1, 0x1.53524c73cea6ap+1, 0x1.0470591de476p+1, 0x1.2bd7609fd4ee2p+0,
	   0x1.1429ff70a9b48p-1, 0x1.a7ed71259ba5bp-3, 0x1.16f3004fb3ac1p-4, 0x1.4116b0388aa9fp-6};
	r = ((c[0] + z * c[1]) + z2 * (c[2] + z * c[3])) + (z2*z2) * ((c[4] + z * c[5]) + z2 * (c[6] + z * c[7]));
      }
    } else {
      static const double c[] =
	{0x1.26bb1bbb55515p+1, 0x1.53524c73cea42p+1, 0x1.0470591de2d1dp+1, 0x1.2bd760a010a53p+0,
	 0x1.1429ffd16170cp-1, 0x1.a7ed6b2a0d97fp-3, 0x1.16e4e37fa51e4p-4, 0x1.4147fe4c1676fp-6,
	 0x1.4897c4b3e329ap-8};
      r = ((c[0] + z * c[1]) + z2 * (c[2] + z * c[3])) + (z2*z2) * ((c[4] + z * c[5]) + z2 * (c[6] + z * (c[7] + z *c[8])));
    }
    r *= z;
    return r;
  } else {
    if(__builtin_expect((ux<<11)==0,0)){
      uint32_t k = (ux>>21) - 0x1fc;
      if(k<=0xb){
	if(k== 0) return 10.0f-1.0f;
	if(k== 4) return 100.0f-1.0f;
	if(k== 6) return 1000.0f-1.0f;
	if(k== 8) return 10000.0f-1.0f;
	if(k== 9) return 100000.0f-1.0f;
	if(k==10) return 1000000.0f-1.0f;
	if(k==11) return 10000000.0f-1.0f;
      }
    }
    double a = iln10h*z, ia = __builtin_floor(a), h = (a - ia) + iln10l*z;
    int64_t i = ia, j = i&0xf, e = i - j;
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
    return (s-1.0) + w*c0;
  }
}

/* just to compile since glibc does not contain this function*/
float exp10m1f(float x){
  return cr_exp10m1f(x);
}
