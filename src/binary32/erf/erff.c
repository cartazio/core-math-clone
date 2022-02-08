/* Correctly-rounded error function of binary32 value.

Copyright (c) 2022 Paul Zimmermann, INRIA.

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

#define SAMPLE 16

/* For 0 <= i < 63, P[i] is a degree-8 polynomial approximating erf(x)
   on [i/SAMPLE,(i+1)/SAMPLE], generated by gen_P(8,0.0625) from erf.sage.
   The numbers in comment at the end of each line are the index i and
   the relative accuracy of the minimax polynomial generated by Sollya
   (in terms of bits). */
static const double P[63][9] = {
  { 0x0p+0, 0x1.20dd750429b6dp+0, 0x1.1a1fa038de73fp-44, -0x1.812746b09d739p-2, 0x1.a74e2007e4ad5p-29, 0x1.ce2eec8e04e99p-4, 0x1.cb02ed79ae069p-18, -0x1.ba50a1c8a8c66p-6, 0x1.53ba5079f10cap-10 }, /* 0:54.681 */
  { 0x1.17522e7161c8ep-39, 0x1.20dd750349b0cp+0, 0x1.3d1d29c38f2d2p-27, -0x1.812756f01cc35p-2, 0x1.102eabdcd0f6p-18, 0x1.cdfff30e339d3p-4, 0x1.5aec79e9f632dp-12, -0x1.d1ae1fa130cadp-6, 0x1.177cf09d8294bp-8 }, /* 1:57.409 */
  { 0x1.d770ff9e6ab3cp-33, 0x1.20dd74cdaa079p+0, 0x1.65d62b05993fbp-22, -0x1.81289ce96a985p-2, 0x1.a487bc9d58f4cp-15, 0x1.ccd6871a0c1f9p-4, 0x1.79400df4d7666p-10, -0x1.fae4e5e8a169cp-6, 0x1.be094c6813327p-8 }, /* 2:58.389 */
  { 0x1.05c54d2412c58p-28, 0x1.20dd7254519fap+0, 0x1.9201984735bcfp-19, -0x1.812fdb389a1e4p-2, 0x1.e3990dd283729p-13, 0x1.c9b9f408d312fp-4, 0x1.c483a994a4e0cp-9, -0x1.169ccca042847p-5, 0x1.22b9dd3d479dp-7 }, /* 3:59.254 */
  { 0x1.cf14f0538be5ap-26, 0x1.20dd66214dda2p+0, 0x1.b51ed49aa9444p-17, -0x1.8144abda53798p-2, 0x1.47ac16515889cp-11, 0x1.c491e843dbb9ap-4, 0x1.878b45cf90cap-8, -0x1.2e5d85168ffp-5, 0x1.52b7603448aaap-7 }, /* 4:60.259 */
  { 0x1.959dc6ccdab3ap-24, 0x1.20dd493858436p+0, 0x1.0f2dd5711a1dbp-15, -0x1.816522f7ae1f4p-2, 0x1.26621c73eb1e3p-10, 0x1.bf4e80a4d6c3ap-4, 0x1.07dac904839c8p-7, -0x1.3e21e8aee84bfp-5, 0x1.6c579ac750449p-7 }, /* 5:61.935 */
  { 0x1.903809630b906p-24, 0x1.20dd485bfcdf8p+0, 0x1.194cb8c05465dp-15, -0x1.8167d7702b40bp-2, 0x1.32ce7ad45fb3ap-10, 0x1.bec739b84c959p-4, 0x1.0eb8def355296p-7, -0x1.3fad8c5accf43p-5, 0x1.6eb95e6d8419fp-7 }, /* 6:62.775 */
  { -0x1.c1ffc82e40859p-21, 0x1.20de6067f974ep+0, -0x1.98dc27d77cb35p-14, -0x1.80cda511544d5p-2, -0x1.f962e319d4541p-12, 0x1.caf597336866fp-4, 0x1.41f5ab9a230e1p-8, -0x1.2e0091e26acf1p-5, 0x1.5ac59a499e173p-7 }, /* 7:61.22 */
  { -0x1.8b3ce2c76cce8p-18, 0x1.20e39cf73f331p+0, -0x1.554c4f8722b69p-11, -0x1.7e8ec3bc20921p-2, -0x1.83d3c8036c60ep-8, 0x1.ee509f0d3be05p-4, -0x1.e00e8820090dfp-9, -0x1.06151e980f24ep-5, 0x1.3308588f3288bp-7 }, /* 8:60.699 */
  { -0x1.8be17e78be477p-16, 0x1.20f3fd5ab91c9p+0, -0x1.1fb47d5c751dep-9, -0x1.78f87c79acdaep-2, -0x1.268a27bde7daep-6, 0x1.1a1f7270a43ecp-3, -0x1.33b7233f74a14p-6, -0x1.8eb708060887ep-6, 0x1.f6ca53c17216fp-8 }, /* 9:60.474 */
  { -0x1.2f0e94706329cp-14, 0x1.211c94787f9a8p+0, -0x1.720a3d20ebf54p-8, -0x1.6db5a5cc1ceb9p-2, -0x1.46cbce0b73649p-5, 0x1.5366cf5f9b77ap-3, -0x1.50bafa938e8e2p-5, -0x1.cf93569295709p-7, 0x1.71600644e4177p-8 }, /* 10:60.4 */
  { -0x1.8311b38892cbap-13, 0x1.2171e18e65146p+0, -0x1.91614b39f785cp-7, -0x1.5a1979c1c5884p-2, -0x1.31aaa4f9dff37p-4, 0x1.a604bc52f3ba3p-3, -0x1.205f828f040afp-4, -0x1.039e905eb2fbdp-9, 0x1.c0e05909eb39dp-9 }, /* 11:60.434 */
  { -0x1.adadf51bd571fp-12, 0x1.220ed735780e5p+0, -0x1.7f58b040ce227p-6, -0x1.3bb83f30bcde5p-2, -0x1.fbdfb30c59252p-4, 0x1.08dead7d56356p-2, -0x1.afe7afa265fbfp-4, 0x1.746ffd5a33e5ep-7, 0x1.3a794ed0fa395p-10 }, /* 12:60.558 */
  { -0x1.a810ec5537f2dp-11, 0x1.230fcea9e25e3p+0, -0x1.49cd82d65b393p-5, -0x1.114639510b7b9p-2, -0x1.806afe9d81b91p-3, 0x1.4912b7e5a470fp-2, -0x1.26f654d211058p-3, 0x1.988ca4370a30bp-6, -0x1.d258177b8251cp-11 }, /* 13:60.769 */
#if 0
  { -0x1.78d89e48a53f6p-10, 0x1.24880485b11dp+0, -0x1.02dc27aee0746p-4, -0x1.b73ce6352161fp-3, -0x1.0cd5e3e317d16p-2, 0x1.8f224434b875ap-2, -0x1.770fce058233dp-3, 0x1.34f847ce948f6p-5, -0x1.6424e95f4ceffp-9 }, /* 14:61.079 */
#else
  /* coefficient of degree 1 modified to avoid an exceptional case for
     x=-0x1.cb2452p-1 and rndd */
  { -0x1.78d89e48a53f6p-10, 0x1.24880485b11ep+0, -0x1.02dc27aee0746p-4, -0x1.b73ce6352161fp-3, -0x1.0cd5e3e317d16p-2, 0x1.8f224434b875ap-2, -0x1.770fce058233dp-3, 0x1.34f847ce948f6p-5, -0x1.6424e95f4ceffp-9 }, /* 14:61.079 */
#endif
  { -0x1.2f7854ff8ef7bp-9, 0x1.26728ecb8239bp+0, -0x1.754708b273ed9p-4, -0x1.3d3353afd28b9p-3, -0x1.5e3685a7ea032p-2, 0x1.d49cffc1f0a58p-2, -0x1.c13c3b9a3255cp-3, 0x1.8f7e777860d88p-5, -0x1.12c534d70236fp-8 }, /* 15:61.437 */
  { -0x1.b944b9611e298p-9, 0x1.2899ce7774285p+0, -0x1.ede8316bea9ecp-4, -0x1.88fd90bbfec3dp-4, -0x1.a9b894cb0b0d5p-2, 0x1.088b749b8fa83p-1, -0x1.fdcc1b631d593p-3, 0x1.d4d2f931a20e8p-5, -0x1.583c366b34ce1p-8 }, /* 16:61.052 */
  { -0x1.1d940c81edefdp-8, 0x1.2a83a2610a58cp+0, -0x1.2979882c6bbcfp-3, -0x1.94c3014e4d937p-5, -0x1.e1eab53d43df3p-2, 0x1.1dc197df00709p-1, -0x1.12eb4eb016b6ep-2, 0x1.0003483799f31p-4, -0x1.81078223d980fp-8 }, /* 17:63.231 */
  { -0x1.3dbb2492c1851p-8, 0x1.2b6a29990969fp+0, -0x1.401344f618329p-3, -0x1.e55a1341cc883p-6, -0x1.f89fb52db226p-2, 0x1.25e6a1c4e3a09p-1, -0x1.1a393d5339647p-2, 0x1.07802069f0f8p-4, -0x1.8e769784541edp-8 }, /* 18:68.227 */
  { -0x1.0d22acb495d76p-8, 0x1.2a25dc5205448p+0, -0x1.227bbb1d3b7b3p-3, -0x1.b8326c71d3216p-5, -0x1.dedef1e5247aep-2, 0x1.1d4de78710b86p-1, -0x1.130c7d11ccb41p-2, 0x1.00a7f96540989p-4, -0x1.83093e4d12b84p-8 }, /* 19:63.545 */
  { -0x1.25392d712ac6cp-10, 0x1.2544e485d02acp+0, -0x1.6b1ebf9c55d94p-4, -0x1.1bc70fc4dbab2p-3, -0x1.88443a50d4486p-2, 0x1.01ab5d47085d4p-1, -0x1.f9fff40a39f3ap-3, 0x1.d918785f330d6p-5, -0x1.62f0361180069p-8 }, /* 20:62.714 */
  { 0x1.604801d07d378p-8, 0x1.1b28516412dcep+0, 0x1.0dd8cac4977e3p-6, -0x1.319f4d9dda20ep-2, -0x1.d92dd63478e36p-3, 0x1.a497fe058e897p-2, -0x1.b1e95f53528f2p-3, 0x1.9a66ae1af0e2p-5, -0x1.3338cb3b2000ep-8 }, /* 21:62.336 */
  { 0x1.127604309e4f3p-6, 0x1.0a3cc74c3631p+0, 0x1.79d5685eb39e7p-3, -0x1.15caed0fd5ae9p-1, -0x1.32bc48b25d63p-7, 0x1.20a314bbc5fa1p-2, -0x1.52040e741925ep-3, 0x1.4abeafeba6963p-5, -0x1.f2a2c0412e2dap-9 }, /* 22:62.165 */
  { 0x1.18f58f056ca05p-5, 0x1.e2887dd4a0572p-1, 0x1.afe4dcce947cp-2, -0x1.beb0de5c874c1p-1, 0x1.1bf685ea40949p-2, 0x1.f56afb052e2e2p-4, -0x1.c0f175b3a0245p-4, 0x1.e0fe61914535p-6, -0x1.7516e7f765269p-9 }, /* 23:62.124 */
  { 0x1.e2b8cf6db29e5p-5, 0x1.9f536f1a0d7a8p-1, 0x1.74ac354e64f0dp-1, -0x1.47c7d93b9a5cbp+0, 0x1.3bfac77271869p-1, -0x1.e19af438b4764p-5, -0x1.9321056d55063p-5, 0x1.24839594cc40ep-6, -0x1.eed7a5a2fa6f5p-10 }, /* 24:62.181 */
  { 0x1.742c2a597591fp-4, 0x1.4ba48d9547627p-1, 0x1.1807ec11e2effp+0, -0x1.bfaf045a8cd2fp+0, 0x1.fbcdab38cbbedp-1, -0x1.00a0bfdc66fa3p-2, 0x1.8f1d1945e843p-7, 0x1.b23763624c96p-8, -0x1.034c346689e51p-10 }, /* 25:62.331 */
  { 0x1.08daffa9dbc52p-3, 0x1.d56df8be243b1p-2, 0x1.8067bc15af0f6p+0, -0x1.2012947b8f1cbp1, 0x1.60ba9081bc44ep+0, -0x1.c3480c1aabbadp-2, 0x1.2189437f4cd3p-4, -0x1.e0756f5e5ab66p-9, -0x1.9d5de0401b444p-13 }, /* 26:62.566 */
  { 0x1.60d097fec2c15p-3, 0x1.04ec70ea7307cp-2, 0x1.ec88d3f6abf7fp+0, -0x1.6029fed83319ep1, 0x1.bfb5bc8d87f59p+0, -0x1.3bbc071fb9da9p-1, 0x1.f7335e4600014p-4, -0x1.99b26d40298d2p-7, 0x1.e035ff62737c7p-12 }, /* 27:62.898 */
  { 0x1.baf818ac5bp-3, 0x1.b65438ce6abd8p-5, 0x1.29d2385e0df4dp1, -0x1.9b1c9782e5814p1, 0x1.09fc12cb7b2f3p1, -0x1.88d299591af68p-1, 0x1.53c458548c6bdp-3, -0x1.4019d5cbaf38dp-6, 0x1.f7c82276d1e6p-11 }, /* 28:63.351 */
  { 0x1.06cd8cfe691d1p-2, -0x1.fed15fcba1234p-4, 0x1.55e7cf8f797d3p1, -0x1.cbcc2c24558bdp1, 0x1.2b9796fc79581p1, -0x1.c43780d5bb181p-1, 0x1.956058fe96509p-3, -0x1.92efa1d518c4ep-6, 0x1.5768a29a448abp-10 }, /* 29:63.987 */
  { 0x1.266823e84aa8cp-2, -0x1.06bac5b504c6bp-2, 0x1.7574c932a907dp1, -0x1.ed80804657738p1, 0x1.4218d5a40e406p1, -0x1.eab02c3f125d6p-1, 0x1.be7b9c15d334cp-3, -0x1.c522eb6b596b2p-6, 0x1.8d0e2ba549087p-10 }, /* 30:64.982 */
  { 0x1.c0770fb34ebcap-3, 0x1.5915e4d02ed3p-6, 0x1.3679fa7788efdp1, -0x1.adbb1e1b1f0cbp1, 0x1.19beaa1cdee44p1, -0x1.a954d14e9ab0ap-1, 0x1.7c551a00defc6p-3, -0x1.78a117adda86cp-6, 0x1.3fa478721967bp-10 }, /* 31:55.323 */
  { 0x1.817abe0b84f38p-3, 0x1.0cbdd5ffea26p-3, 0x1.20ad71ba5e35ap1, -0x1.9ace2e9fda9fp1, 0x1.0fb5e98c30a39p1, -0x1.9c253e6222776p-1, 0x1.72039e8916748p-3, -0x1.701d04db3e0ffp-6, 0x1.3a67265195282p-10 }, /* 32:55.323 */
#if 0
  { 0x1.09d0dc6a0b9a1p-2, -0x1.37094c280152fp-3, 0x1.5f8d21e63afeep1, -0x1.d9212fe1155b6p1, 0x1.3650612a085cp1, -0x1.d957f79ff0573p-1, 0x1.aea32667c224dp-3, -0x1.b4bb5ee828b5ep-6, 0x1.7e596b01c7e21p-10 }, /* 33:64.983 */
#else
  /* Coefficients of degree 4 and 6 modified to avoid exceptional cases for
     |x|=0x1.083cd4p+1 and rndd, and for |x|=0x1.092c5ap+1 and rndu. */
  { 0x1.09d0dc6a0b9a1p-2, -0x1.37094c280152fp-3, 0x1.5f8d21e63afeep1, -0x1.d9212fe1155b6p1, 0x1.3650612a085c1p1, -0x1.d957f79ff0573p-1, 0x1.aea32667c224cp-3, -0x1.b4bb5ee828b5ep-6, 0x1.7e596b01c7e21p-10 }, /* 33:64.983 */
#endif
  { 0x1.88d92b7d0b2ep-3, 0x1.a5c83f62eceb9p-4, 0x1.29e089190895bp1, -0x1.a6a90677d25fcp1, 0x1.18a6ff4d452efp1, -0x1.acb6ab6f94121p-1, 0x1.84aa309ba04aap-3, -0x1.879dfacb93f6fp-6, 0x1.53ea80133aff3p-10 }, /* 34:64.593 */
  { 0x1.6f0a11e4354aep-4, 0x1.e7f7fd1852db3p-2, 0x1.bad610cc308ebp+0, -0x1.60caf27ec807dp1, 0x1.e17eb7dc6e68dp+0, -0x1.725d1c12d6a29p-1, 0x1.4f5625641d43ep-3, -0x1.4fe996514b35dp-6, 0x1.20ffea645fe77p-10 }, /* 35:64.429 */
  { -0x1.7dcc8b6e22c0fp-5, 0x1.ebdafcd5f7061p-1, 0x1.f43bbdde0e578p-1, -0x1.0b2a782719164p1, 0x1.8260d4635d6bp+0, -0x1.2ebd239883d03p-1, 0x1.133c0ae13b006p-3, -0x1.12dccc17e6dcbp-6, 0x1.d5789a1d2736dp-11 }, /* 36:64.4 */
  { -0x1.b566cc7daf43p-3, 0x1.89c36239296d8p+0, 0x1.a688d3d83bf53p-4, -0x1.54e1bda84472ep+0, 0x1.19d2027e403e5p+0, -0x1.cccc62cf54ee5p-2, 0x1.a9571db5dcfd9p-4, -0x1.aa09e80732cfp-7, 0x1.6a7ceb5764d6p-11 }, /* 37:64.467 */
  { -0x1.9f06394976829p-2, 0x1.17890ca36ab46p1, -0x1.b2604bf70ca6ep-1, -0x1.0f8049f65b7b7p-1, 0x1.5bb4ad0d199fep-1, -0x1.3b508f5cae4b2p-2, 0x1.2ecfe5452163fp-4, -0x1.3418a4a9b0cb5p-7, 0x1.0725646d5c132p-11 }, /* 38:64.612 */
  { -0x1.3a0e5feb35b76p-1, 0x1.6ef4ed4e24979p1, -0x1.d441ec8ecae5fp+0, 0x1.1919a57def1fep-2, 0x1.10bc397515907p-2, -0x1.6120fbe73a9eep-3, 0x1.79e10abf10c54p-5, -0x1.92921b939e0ffp-8, 0x1.5ef4f24c0ea4ep-12 }, /* 39:64.827 */
  { -0x1.a7c4bfb70723ap-1, 0x1.c6bc8f25b318fp1, -0x1.650a1f6442f19p1, 0x1.0af898f450113p+0, -0x1.e3010d57d27b9p-4, -0x1.94e820592993p-5, 0x1.609589fc261fcp-6, -0x1.b463e9387cb9ep-9, 0x1.96cad0696f6efp-13 }, /* 40:65.113 */
  { -0x1.09775e1426c89p+0, 0x1.0d328cfbaf13p2, -0x1.d75583bca27dbp1, 0x1.bd6f62ef1454fp+0, -0x1.d51753c0aec97p-2, 0x1.d16c34b8363ap-5, 0x1.99a4ffbe2e69ap-11, -0x1.0a570169a02b9p-10, 0x1.540770ec67003p-14 }, /* 41:65.469 */
  { -0x1.3acb5a629c744p+0, 0x1.32cb49b679ad8p2, -0x1.1dd0c89cdc5e8p2, 0x1.2b2ad52d01bf3p1, -0x1.7c3b8301b653dp-1, 0x1.260b85d3ad94bp-3, -0x1.021c650a6df5fp-6, 0x1.9b74c8de5fdbfp-11, -0x1.3dd78fec64623p-18 }, /* 42:65.533 */
  { -0x1.657c70e381c59p+0, 0x1.529497a4125edp2, -0x1.473bd78185c8ep2, 0x1.68d91a418d387p1, -0x1.ef0e89846209cp-1, 0x1.aeda928112a92p-3, -0x1.cddf14331e86p-6, 0x1.14492e1034ea1p-9, -0x1.162cd990ef4f4p-14 }, /* 43:66.449 */
  { -0x1.86fa9099ed877p+0, 0x1.6af47a13091f9p2, -0x1.6647155543e99p2, 0x1.96093bed02355p1, -0x1.20a3dace5a1a2p+0, 0x1.074def92f886p-2, -0x1.2ca25bf0f9802p-5, 0x1.883fe407e5663p-9, -0x1.bf00bd223955fp-14 }, /* 44:67.15 */
  { -0x1.9dabceaa53ef2p+0, 0x1.7b1bbbdd16e75p2, -0x1.7a66a15bda47bp2, 0x1.b2afe002ce322p1, -0x1.3a22dcd3f6f65p+0, 0x1.2458ce85f7ad8p-2, -0x1.55fcf722dadf7p-5, 0x1.cb8c557818833p-9, -0x1.0f6b0ed5612f3p-13 }, /* 45:68.151 */
  { -0x1.a8896e4a6d2aap+0, 0x1.82aeda87720fdp2, -0x1.83a45683821edp2, 0x1.bf9269c9d744dp1, -0x1.455d70a548e03p+0, 0x1.30df6f11bbb1p-2, -0x1.67746121beaap-5, 0x1.e76252c6857bbp-9, -0x1.22d3ac72f9019p-13 }, /* 46:70.206 */
  { -0x1.a70baf34bc6dcp+0, 0x1.81af02a272b92p2, -0x1.82785730280bfp2, 0x1.be007a741e79dp1, -0x1.440cf2e535b97p+0, 0x1.2f76f09f2faa6p-2, -0x1.6591bcdaa90bap-5, 0x1.e480062060f24p-9, -0x1.20e5b36588ae5p-13 }, /* 47:66.967 */
  { -0x1.9a0e93ac10922p+0, 0x1.790a1dddaca16p2, -0x1.78670b1575d81p2, 0x1.b099dde3f1a0ap1, -0x1.38e6d862ca653p+0, 0x1.239788249a257p-2, -0x1.55c3b43d826a6p-5, 0x1.cc7462bbb5709p-9, -0x1.10e452d561655p-13 }, /* 48:66.967 */
#if 0
  { -0x1.8252aaa5fdbcap+0, 0x1.698cf66a164d8p2, -0x1.66b67fa2d929ap2, 0x1.9982dda5f52fep1, -0x1.261088a67efd4p+0, 0x1.0febb2ecaa837p-2, -0x1.3c16038736604p-5, 0x1.a6250d1a023acp-9, -0x1.efc59eaa8ea2ap-14 }, /* 49:68.353 */
#else
  /* coefficient of degree 3 modified to avoid an exceptional case for
     x=0x1.8ef87cp+1 and rndd */
  { -0x1.8252aaa5fdbcap+0, 0x1.698cf66a164d8p2, -0x1.66b67fa2d929ap2, 0x1.9982dda5f5300p1, -0x1.261088a67efd4p+0, 0x1.0febb2ecaa837p-2, -0x1.3c16038736604p-5, 0x1.a6250d1a023acp-9, -0x1.efc59eaa8ea2ap-14 }, /* 49:68.353 */
#endif
  { -0x1.6119a9f57324bp+0, 0x1.544ba93908e0ep2, -0x1.4eea346825e61p2, 0x1.7b0f08786bebbp1, -0x1.0db58acb4855dp+0, 0x1.edf9385ce2e65p-3, -0x1.1c2d6a0e4f02cp-5, 0x1.7779cd1152312p-9, -0x1.b40b998327067p-14 }, /* 50:68.212 */
  { -0x1.38538135c2d6bp+0, 0x1.3ab778681b0d9p2, -0x1.32d53653c3e48p2, 0x1.57d2cbc3c3721p1, -0x1.e42746b7c5034p-1, 0x1.b67f26c55b0bp-3, -0x1.f2bdb1b6647e8p-6, 0x1.458eef7440dcep-9, -0x1.7567817ced13cp-14 }, /* 51:68.163 */
  { -0x1.09fcc03a0e6e6p+0, 0x1.1e33f04319b2ap2, -0x1.1420a766801fbp2, 0x1.32089b4c7724ap1, -0x1.aa0410b391639p-1, 0x1.7d40b38d645edp-3, -0x1.ac48bc9826c5bp-6, 0x1.1400c90749fdfp-9, -0x1.386862e1912a1p-14 }, /* 52:68.283 */
  { -0x1.b0327507012bbp-1, 0x1.0014f28088c34p2, -0x1.e89a3c9984ba5p1, 0x1.0b99bf7416867p1, -0x1.70001bdff73a3p-1, 0x1.4533fc46665c7p-3, -0x1.6898290ab1bb4p-6, 0x1.ca941b22269c4p-10, -0x1.fffadf5ad2eecp-15 }, /* 53:68.38 */
  { -0x1.494f0e7edd913p-1, 0x1.c330b22f4fe98p1, -0x1.a95ddae62b176p1, 0x1.cc3f8f2099078p+0, -0x1.38793a98e6c79p-1, 0x1.108c5991a7873p-3, -0x1.2a2dc3b02c1dfp-6, 0x1.76057aeb92203p-10, -0x1.9bbe5bc349673p-15 }, /* 54:68.62 */
  { -0x1.c5635bc5b5c63p-2, 0x1.877b4bcd87ab6p1, -0x1.6c90c441642cep1, 0x1.857cfd1efa39dp+0, -0x1.0500e818aefcbp-1, 0x1.c140582d5e3a6p-4, -0x1.e4ce5440b1272p-7, 0x1.2bd48314faba5p-10, -0x1.4563f82bd37a1p-15 }, /* 55:68.859 */
  { -0x1.fd7d7c6c12e98p-3, 0x1.4eb818bc8b8c9p1, -0x1.33cb5f699ca6p1, 0x1.44988778fec7dp+0, -0x1.ad491ee6f6a1bp-2, 0x1.6c758b5aabbddp-4, -0x1.83e0abfc84418p-7, 0x1.d906f62b5ade4p-11, -0x1.fa04569466a81p-16 }, /* 56:69.132 */
  { -0x1.0d89a80f6436ap-4, 0x1.1a1e3e67ac84dp1, -0x1.001b580223e05p1, 0x1.0a8c7d02d3fp+0, -0x1.5bcc3dbdafdaep-2, 0x1.233ec12b50445p-4, -0x1.31a6894809be1p-7, 0x1.6f7b827d77b0ap-11, -0x1.83795fce4295fp-16 }, /* 57:69.436 */
  { 0x1.a4f4ffaa5a394p-4, 0x1.d4fa3e193e3bap+0, -0x1.a438655dc7b25p+0, 0x1.af9075ee3e37ep-1, -0x1.15c1e0b4ce3d9p-2, 0x1.cacc93e7bcfbep-5, -0x1.dac64d7bcde0cp-8, 0x1.195ebce9492f6p-11, -0x1.246c0fe0d5bc6p-16 }, /* 58:69.77 */
  { 0x1.04ebe38ea8eb2p-2, 0x1.808583c3b6737p+0, -0x1.540a3637cd391p+0, 0x1.58911cc2053abp-1, -0x1.b5850ef1de405p-3, 0x1.645fa17d64385p-5, -0x1.6ba25bf3d63abp-8, 0x1.a8e9a244e5f39p-12, -0x1.b348b07f975dbp-17 }, /* 59:70.129 */
  { 0x1.8e90243f12365p-2, 0x1.3718628fa6c3p+0, -0x1.0f7dbabfb2b6fp+0, 0x1.0f6d959a579eap-1, -0x1.53f9058a7081bp-3, 0x1.111b802581a18p-5, -0x1.12c99f48497bdp-8, 0x1.3c91033540aep-12, -0x1.3fac0d25e886cp-17 }, /* 60:70.513 */
  { 0x1.0300d47401e23p-1, 0x1.f0d7305e1ebf5p-1, -0x1.abe00f68d579bp-1, 0x1.a60d7ad53ba65p-2, -0x1.04bb8fbc7bb59p-3, 0x1.9d26cd3cc8b41p-6, -0x1.99eb5268623b1p-9, 0x1.d19d846b79653p-13, -0x1.cf853c59e8c7dp-18 }, /* 61:70.917 */
  { 0x1.35ea1608c9887p-1, 0x1.87b45f800192p-1, -0x1.4ce2bf5dd9a7dp-1, 0x1.43f7e85413fcfp-2, -0x1.8adcffe1e4b4cp-4, 0x1.349106455fc8ap-6, -0x1.2debcc727980ep-9, 0x1.52289f1b6f48dp-13, -0x1.4be655cf56d8p-18 }, /* 62:71.311 */
};

float
cr_erff (float x)
{
  /* Deal with NaN here */

  double xx = (x >= 0) ? x : -x, y;

  if (xx > 0x1.f5a888p1) /* "overflow" to 1 or -1 */
    return (x > 0) ? 1.0f - 0x1p-25f : -1.0f + 0x1p-25f;

  /* now xx <= 0x1.f5a888p1 ~ 3.91921 */
  int i = SAMPLE * xx; /* i < 64 */
  const double *p;
  p = P[i];
  y = p[8];
  y = p[7] + y * xx;
  y = p[6] + y * xx;
  y = p[5] + y * xx;
  y = p[4] + y * xx;
  y = p[3] + y * xx;
  y = p[2] + y * xx;
  y = p[1] + y * xx;
  y = p[0] + y * xx;

  return (x > 0) ? y : (-y);
}
