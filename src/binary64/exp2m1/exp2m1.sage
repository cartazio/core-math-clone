def doit_bacsel_subnormal():
   for e in [-1073..-1022]:
      nn = 1074 + e # number of bits of output
      # deal with |exp2m1(x)| in [2^(e-1),2^e)
      # first deal with positive values
      a = RR(2^-1074)
      b = RR(1)
      while a.nextabove()!=b:
         c = (a+b)/2
         if n(c*log(2)) < 2^(e-1):
            a = c
         else:
            b = c
      x0 = b
      a = b
      b = RR(1)
      while a.nextabove()!=b:
         c = (a+b)/2
         if n(c*log(2)) < 2^e:
            a = c
         else:
            b = c
      x1 = b
      t0, e0, n0 = t_exp (x0)
      t1, e1, n1 = t_exp (x1)
      print_bacsel_pos (t0, e0, n0, t1, e1, n1, nn)

def t_exp(x):
   if abs(x)<2^-1022:
      m = ceil(x.exact_rational()*2^1074)
      n = m.nbits()
      assert n<=52, "n<=52"
      return m, -1074+n, n
   else:
      s,m,e = x.sign_mantissa_exponent()
      assert m.nbits()==53, "m.nbits()==53"
      return s*m, e+53, 53

def print_bacsel_pos (t0, e0, n0, t1, e1, n1, nn):
   assert n0<=n1, "n0<=n1"
   if n0<n1:
      print_bacsel_pos (t0, e0, n0, 2^n0, e0, n0, nn)
      print_bacsel_pos (2^n0, e0+1, n0+1, t1, e1, n1, nn)
   else:
      assert e0<=e1, "e0<=e1"
      if e0<e1:
         print_bacsel_pos (t0, e0, n0, 2^n0, e0, n0, nn)
         print_bacsel_pos (2^(n0-1), e0+1, n0, t1, e1, n1, nn)
      else:
         print ("./doit1.sh " + str(t0) + " " + str(t1) + " " + str(n0) + " " + str(e0) + " 64 10 " + str(nn) + " >> out")

# return the 'ulp' of the interval x, i.e., max(ulp(t)) for t in x
# this internal routine is used below
def RIFulp(x):
   return max(x.lower().ulp(),x.upper().ulp())

def exp2m1_fast_tiny(xmin=-0.125,xmax=0.125,verbose=false,rel=false):
   P = ["0x1.62e42fefa39efp-1","0x1.abd1697afcaf8p-56","0x1.ebfbdff82c58fp-3","-0x1.5e5a1d09e1599p-57","0x1.c6b08d704a0bfp-5","0x1.3b2ab6fba4e78p-7","0x1.5d87fe78a84e6p-10","0x1.430912f86a48p-13","0x1.ffcbfbc1f2b36p-17","0x1.62c0226c7f6d1p-20","0x1.b539529819e63p-24","0x1.e4d552bed5b9cp-28"]
   P = [RR(x,16) for x in P]
   x = RIF(xmin,xmax)
   err0 = 2^-68.559*max(abs(expm1(xmin)),abs(expm1(xmax)))
   if verbose:
      print ("err0=", log(err0)/log(2.))
   # x2 = x * x
   x2 = x * x
   err_x2 = RIFulp(x2)
   # x4 = x2 * x2
   x4 = x2 * x2
   err_x4 = RIFulp(x4) + 2*err_x2*x2.abs().upper()
   # c8 = __builtin_fma (P[10], x, P[9])
   c8 = P[10]*x+P[9]
   err2 = RIFulp(c8)*x.abs().upper()^8
   if verbose:
      print ("err2=", log(err2)/log(2.))
   # c6 = __builtin_fma (P[8], x, P[7])
   c6 = P[8]*x+P[7]
   err3 = RIFulp(c6)*x.abs().upper()^6
   if verbose:
      print ("err3=", log(err3)/log(2.))
   # c4 = __builtin_fma (P[6], x, P[5])
   c4 = P[6]*x+P[5]
   err4 = RIFulp(c4)*x.abs().upper()^4
   if verbose:
      print ("err4=", log(err4)/log(2.))
   # c8 = __builtin_fma (P[11], x2, c8)
   c8 = P[11]*x2+c8
   err5 = (RIFulp(c8)+P[11]*err_x2)*x.abs().upper()^8
   if verbose:
      print ("err5=", log(err5)/log(2.))
   # c4 = __builtin_fma (c6, x2, c4)
   c4 = c6*x2+c4
   err6 = (RIFulp(c4)+c6.abs().upper()*err_x2)*x.abs().upper()^4
   if verbose:
      print ("err6=", log(err6)/log(2.))
   # c4 = __builtin_fma (c8, x4, c4)
   c4 = c8*x4+c4
   err7 = (RIFulp(c4)+c8.abs().upper()*err_x4)*x.abs().upper()^4
   if verbose:
      print ("err7=", log(err7)/log(2.))
   # a_mul (h, l, c4, x)
   h = c4*x
   u = RIFulp(h)
   l = RIF(-u,u)
   # fast_two_sum (h, &t, P[4], *h)
   h = P[4]+h
   u = RIFulp(h)
   t = RIF(-u,u)
   err8 = h.abs().upper()*2^-105*x.abs().upper()^3
   if verbose:
      print ("err8=", log(err8)/log(2.))
   # t += *l
   t += l
   err9 = RIFulp(t)*x.abs().upper()^3
   if verbose:
      print ("err9=", log(err9)/log(2.))
   # a_mul (h, l, *h, x)
   h = h*x
   u = RIFulp(h)
   l = RIF(-u,u)
   # *l = __builtin_fma (t, x, *l)
   l = t*x+l
   err10 = (RIFulp(t*x)+RIFulp(l))*x.abs().upper()^2
   if verbose:
      print ("err10=", log(err10)/log(2.))
   # fast_two_sum (h, &t, P[2], *h)
   h = P[2]+h
   u = RIFulp(h)
   t = RIF(-u,u)
   err11 = h.abs().upper()*2^-105*x.abs().upper()^2
   if verbose:
      print ("err11=", log(err11)/log(2.))
   # t += *l
   t += l
   err12 = RIFulp(t)*x.abs().upper()^2
   if verbose:
      print ("err12=", log(err12)/log(2.))
   # a_mul (h, l, *h, x)
   h = h*x
   u = RIFulp(h)
   l = RIF(-u,u)
   # *l = __builtin_fma (t, x, *l)
   l = t*x+l
   err13 = (RIFulp(t*x)+RIFulp(l))*x.abs().upper()
   if verbose:
      print ("err13=", log(err13)/log(2.))
   # fast_two_sum (h, &t, P[1], *h)
   h = P[1]+h
   u = RIFulp(h)
   t = RIF(-u,u)
   err14 = h.abs().upper()*2^-105*x.abs().upper()
   if verbose:
      print ("err14=", log(err14)/log(2.))
   # t += *l
   t += l
   err15 = RIFulp(t)*x.abs().upper()
   if verbose:
      print ("err15=", log(err15)/log(2.))
   # a_mul (h, l, *h, x)
   h = h*x
   u = RIFulp(h)
   l = RIF(-u,u)
   # *l = __builtin_fma (t, x, *l)
   l = t*x+l
   err16 = RIFulp(t*x)+RIFulp(l)
   if verbose:
      print ("err16=", log(err16)/log(2.))
   err = err0+err2+err3+err4+err5+err6+err7+err8+err9+err10+err11+err12+err13+err14+err15+err16
   if rel:
      err = err/(h+l).abs().lower()
   return err
