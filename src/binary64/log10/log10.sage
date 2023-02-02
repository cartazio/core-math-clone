# for 2^e <= x < 2^(e+1) (-1074 <= e <= -1023)
def doit_subnormal(e):
   n = 1075 + e
   assert 1 <= n <= 52, "1 <= n <= 52"
   t0 = 2^(n-1)
   t1 = 2^n
   ein = e+1 # 1/2 <= t/2^n < 1
   if n <= 23:
      print ("./doit0_subnormal.sh " + str(t0) + " " + str(t1) + " " + str(ein) + " " + str(n))
   elif n <= 36:
      print ("./doit1_subnormal.sh " + str(t0) + " " + str(t1) + " " + str(ein) + " " + str(n))
   else:
      print ("./doit2_subnormal.sh " + str(t0) + " " + str(t1) + " " + str(ein) + " " + str(n))

def doit_subnormal_all():
   for e in [-1074..-1023]:
      doit_subnormal(e)

# for 2^(e-1) <= x < 2^e (-1021 <= e <= 1024)
def doit(e):
   t0 = 2^52
   t1 = 2^53
   ein = e
   n = 53
   print ("./doit2_subnormal.sh " + str(t0) + " " + str(t1) + " " + str(ein) + " " + str(n))

def doit_all(emin,emax):
   for e in range(emin,emax):
      doit(e)

from functools import cmp_to_key

# entries are (t0,t1,e)
def cmp(x,y):
   xmin = x[0]*2^x[2]
   xmax = x[1]*2^x[2]
   ymin = y[0]*2^y[2]
   ymax = y[1]*2^y[2]
   if xmax <= ymin:
      return int(-1)
   if ymax <= xmin:
      return int(1)
   if (xmin <= ymin and xmax < ymax):
      return int(-1)
   if (xmin < ymin and xmax <= ymax):
      return int(-1)
   if (ymin <= xmin and ymax < xmax):
      return int(1)
   if (ymin < xmin and ymax <= xmax):
      return int(1)
   return int(0)

# l = statall("/tmp/log")
# [((1, -1074), (9007199254740992, -53)),
# ((4503599627370497, -52), (9007199254740992, 971))]
def statall(f):
   f = open(f,"r")
   l = []
   while true:
      s = f.readline()
      if s=='':
         break
      s = s.split(" ")
      assert len(s) == 5
      t0 = ZZ(s[0])
      t1 = ZZ(s[1])
      e = ZZ(s[2])
      n = ZZ(s[3])
      nn = ZZ(s[4])
      assert nn == 53, "nn == 53"
      assert t0.nbits() == n, "t0.nbits() == n"
      assert (t1-1).nbits() == n, "(t1-1).nbits() == n"
      l.append((t0,t1,e-n))
   f.close()
   l.sort(key=cmp_to_key(cmp))
   l2 = []
   for t0,t1,e in l:
      if l2==[]:
         l2 = [((t0,e),(t1,e))]
      else:
         t1old,e1old = l2[-1][1]
         if t1old*2^e1old > t0*2^e:
            print ((t1old,e1old), (t0, e))
         assert t1old*2^e1old <= t0*2^e, "t1old*2^e1old <= t0*2^e"
         if t1old*2^e1old == t0*2^e:
            l2[-1] = (l2[-1][0],(t1,e))
         else:
            l2.append(((t0,e),(t1,e)))
   l = l2
   return l

_INVERSE = [ "0x1.698p+0", "0x1.688p+0", "0x1.678p+0", "0x1.668p+0", "0x1.658p+0", "0x1.648p+0", "0x1.638p+0", "0x1.63p+0", "0x1.62p+0", "0x1.61p+0", "0x1.6p+0", "0x1.5fp+0", "0x1.5ep+0", "0x1.5dp+0", "0x1.5cp+0", "0x1.5bp+0", "0x1.5a8p+0", "0x1.598p+0", "0x1.588p+0", "0x1.578p+0", "0x1.568p+0", "0x1.56p+0", "0x1.55p+0", "0x1.54p+0", "0x1.53p+0", "0x1.52p+0", "0x1.518p+0", "0x1.508p+0", "0x1.4f8p+0", "0x1.4fp+0", "0x1.4ep+0", "0x1.4dp+0", "0x1.4cp+0", "0x1.4b8p+0", "0x1.4a8p+0", "0x1.4ap+0", "0x1.49p+0", "0x1.48p+0", "0x1.478p+0", "0x1.468p+0", "0x1.458p+0", "0x1.45p+0", "0x1.44p+0", "0x1.43p+0", "0x1.428p+0", "0x1.418p+0", "0x1.41p+0", "0x1.4p+0", "0x1.3f8p+0", "0x1.3e8p+0", "0x1.3ep+0", "0x1.3dp+0", "0x1.3cp+0", "0x1.3b8p+0", "0x1.3a8p+0", "0x1.3ap+0", "0x1.39p+0", "0x1.388p+0", "0x1.378p+0", "0x1.37p+0", "0x1.36p+0", "0x1.358p+0", "0x1.35p+0", "0x1.34p+0", "0x1.338p+0", "0x1.328p+0", "0x1.32p+0", "0x1.31p+0", "0x1.308p+0", "0x1.3p+0", "0x1.2fp+0", "0x1.2e8p+0", "0x1.2d8p+0", "0x1.2dp+0", "0x1.2c8p+0", "0x1.2b8p+0", "0x1.2bp+0", "0x1.2ap+0", "0x1.298p+0", "0x1.29p+0", "0x1.28p+0", "0x1.278p+0", "0x1.27p+0", "0x1.26p+0", "0x1.258p+0", "0x1.25p+0", "0x1.24p+0", "0x1.238p+0", "0x1.23p+0", "0x1.228p+0", "0x1.218p+0", "0x1.21p+0", "0x1.208p+0", "0x1.2p+0", "0x1.1fp+0", "0x1.1e8p+0", "0x1.1ep+0", "0x1.1dp+0", "0x1.1c8p+0", "0x1.1cp+0", "0x1.1b8p+0", "0x1.1bp+0", "0x1.1ap+0", "0x1.198p+0", "0x1.19p+0", "0x1.188p+0", "0x1.18p+0", "0x1.17p+0", "0x1.168p+0", "0x1.16p+0", "0x1.158p+0", "0x1.15p+0", "0x1.14p+0", "0x1.138p+0", "0x1.13p+0", "0x1.128p+0", "0x1.12p+0", "0x1.118p+0", "0x1.11p+0", "0x1.1p+0", "0x1.0f8p+0", "0x1.0fp+0", "0x1.0e8p+0", "0x1.0ep+0", "0x1.0d8p+0", "0x1.0dp+0", "0x1.0c8p+0", "0x1.0cp+0", "0x1.0bp+0", "0x1.0a8p+0", "0x1.0ap+0", "0x1.098p+0", "0x1.09p+0", "0x1.088p+0", "0x1.08p+0", "0x1.078p+0", "0x1.07p+0", "0x1.068p+0", "0x1.06p+0", "0x1.058p+0", "0x1.05p+0", "0x1.048p+0", "0x1.04p+0", "0x1.038p+0", "0x1.03p+0", "0x1.028p+0", "0x1.02p+0", "0x1.018p+0", "0x1.01p+0", "0x1.008p+0", "0x1.ff8p-1", "0x1.fe8p-1", "0x1.fd8p-1", "0x1.fc8p-1", "0x1.fb8p-1", "0x1.fa8p-1", "0x1.f98p-1", "0x1.f88p-1", "0x1.f78p-1", "0x1.f68p-1", "0x1.f58p-1", "0x1.f5p-1", "0x1.f4p-1", "0x1.f3p-1", "0x1.f2p-1", "0x1.f1p-1", "0x1.fp-1", "0x1.efp-1", "0x1.eep-1", "0x1.edp-1", "0x1.ec8p-1", "0x1.eb8p-1", "0x1.ea8p-1", "0x1.e98p-1", "0x1.e88p-1", "0x1.e78p-1", "0x1.e7p-1", "0x1.e6p-1", "0x1.e5p-1", "0x1.e4p-1", "0x1.e3p-1", "0x1.e28p-1", "0x1.e18p-1", "0x1.e08p-1", "0x1.df8p-1", "0x1.dfp-1", "0x1.dep-1", "0x1.ddp-1", "0x1.dcp-1", "0x1.db8p-1", "0x1.da8p-1", "0x1.d98p-1", "0x1.d9p-1", "0x1.d8p-1", "0x1.d7p-1", "0x1.d6p-1", "0x1.d58p-1", "0x1.d48p-1", "0x1.d38p-1", "0x1.d3p-1", "0x1.d2p-1", "0x1.d1p-1", "0x1.d08p-1", "0x1.cf8p-1", "0x1.ce8p-1", "0x1.cep-1", "0x1.cdp-1", "0x1.cc8p-1", "0x1.cb8p-1", "0x1.ca8p-1", "0x1.cap-1", "0x1.c9p-1", "0x1.c88p-1", "0x1.c78p-1", "0x1.c68p-1", "0x1.c6p-1", "0x1.c5p-1", "0x1.c48p-1", "0x1.c38p-1", "0x1.c3p-1", "0x1.c2p-1", "0x1.c18p-1", "0x1.c08p-1", "0x1.bf8p-1", "0x1.bfp-1", "0x1.bep-1", "0x1.bd8p-1", "0x1.bc8p-1", "0x1.bcp-1", "0x1.bbp-1", "0x1.ba8p-1", "0x1.b98p-1", "0x1.b9p-1", "0x1.b8p-1", "0x1.b78p-1", "0x1.b68p-1", "0x1.b6p-1", "0x1.b58p-1", "0x1.b48p-1", "0x1.b4p-1", "0x1.b3p-1", "0x1.b28p-1", "0x1.b18p-1", "0x1.b1p-1", "0x1.bp-1", "0x1.af8p-1", "0x1.afp-1", "0x1.aep-1", "0x1.ad8p-1", "0x1.ac8p-1", "0x1.acp-1", "0x1.ab8p-1", "0x1.aa8p-1", "0x1.aap-1", "0x1.a9p-1", "0x1.a88p-1", "0x1.a8p-1", "0x1.a7p-1", "0x1.a68p-1", "0x1.a6p-1", "0x1.a5p-1", "0x1.a48p-1", "0x1.a4p-1", "0x1.a3p-1", "0x1.a28p-1", "0x1.a2p-1", "0x1.a1p-1", "0x1.a08p-1", "0x1.ap-1", "0x1.9fp-1", "0x1.9e8p-1", "0x1.9ep-1", "0x1.9dp-1", "0x1.9c8p-1", "0x1.9cp-1", "0x1.9bp-1", "0x1.9a8p-1", "0x1.9ap-1", "0x1.998p-1", "0x1.988p-1", "0x1.98p-1", "0x1.978p-1", "0x1.968p-1", "0x1.96p-1", "0x1.958p-1", "0x1.95p-1", "0x1.94p-1", "0x1.938p-1", "0x1.93p-1", "0x1.928p-1", "0x1.92p-1", "0x1.91p-1", "0x1.908p-1", "0x1.9p-1", "0x1.8f8p-1", "0x1.8e8p-1", "0x1.8ep-1", "0x1.8d8p-1", "0x1.8dp-1", "0x1.8c8p-1", "0x1.8b8p-1", "0x1.8bp-1", "0x1.8a8p-1", "0x1.8ap-1", "0x1.898p-1", "0x1.888p-1", "0x1.88p-1", "0x1.878p-1", "0x1.87p-1", "0x1.868p-1", "0x1.86p-1", "0x1.85p-1", "0x1.848p-1", "0x1.84p-1", "0x1.838p-1", "0x1.83p-1", "0x1.828p-1", "0x1.82p-1", "0x1.81p-1", "0x1.808p-1", "0x1.8p-1", "0x1.7f8p-1", "0x1.7fp-1", "0x1.7e8p-1", "0x1.7ep-1", "0x1.7d8p-1", "0x1.7c8p-1", "0x1.7cp-1", "0x1.7b8p-1", "0x1.7bp-1", "0x1.7a8p-1", "0x1.7ap-1", "0x1.798p-1", "0x1.79p-1", "0x1.788p-1", "0x1.78p-1", "0x1.778p-1", "0x1.77p-1", "0x1.76p-1", "0x1.758p-1", "0x1.75p-1", "0x1.748p-1", "0x1.74p-1", "0x1.738p-1", "0x1.73p-1", "0x1.728p-1", "0x1.72p-1", "0x1.718p-1", "0x1.71p-1", "0x1.708p-1", "0x1.7p-1", "0x1.6f8p-1", "0x1.6fp-1", "0x1.6e8p-1", "0x1.6ep-1", "0x1.6d8p-1", "0x1.6dp-1", "0x1.6c8p-1", "0x1.6cp-1", "0x1.6b8p-1", "0x1.6bp-1", "0x1.6a8p-1", "0x1.6ap-1"]

# L=[RR(x,16) for x in _INVERSE]  
# output_log10_inv(L)
def output_log10_inv(L):
   print ("static const double _LOG10_INV[" + str(len(L)) + "][2] = {")
   OFFSET = 362
   maxerr = 0
   for i in range(len(L)):
      h, l = get_hl42_log10 (L[i])
      r = L[i].exact_rational()
      err = n(h.exact_rational()+l.exact_rational()+log10(r),200)
      maxerr = max(maxerr,abs(err))
      s = "    {" + get_hex(h) + ", " + get_hex(l) + "}, /* i=" + str(OFFSET+i) + "*/"
      print (s)
   print ("};")
   print ("maxerr=", maxerr)

log10 = lambda x: log(x)/log(10)

# ensure h is an integer multiple of 2^-42
def get_hl42_log10(ri):
   if ri.parent() != QQ:
      ri = ri.exact_rational()
   h = n(-log10(ri)*2^42,200)
   H = ZZ(round(h))/2^42
   h = RR(H)
   l = RR(n(-log10(ri)-H,200))
   return h, l


