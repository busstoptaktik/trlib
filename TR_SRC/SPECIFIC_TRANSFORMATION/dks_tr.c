/*
 * Copyright (c) 2011, National Survey and Cadastre, Denmark
 * (Kort- og Matrikelstyrelsen), kms@kms.dk
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 */
#include     <stdio.h>
#include     <math.h>
#include     <string.h>
#include    "KmsFncs.h"
#include    "trthread.h"

int              dks_tr(
/*____________________*/
char           *in_lab,
char           *outlab,
double         N_in,
double         E_in,
double         *N_out,
double         *E_out,
char           *usertxt,
FILE           *tr_error
)

{

#define  TOL     0.002   /* 2 mm */

  /* DKS, Absolute limits for transformation */
#define  NMIN    249998.0
#define  NMAX    290002.0
#define  EMIN     99998.0
#define  EMAX    140002.0

  /* ACTION NAMES */
#define  IDNT     0
#define  DK_U    +1
#define  U_DK    -1
#define  DK_S    +2
#define  S_DK    -2
#define  DK_R    +3
#define  R_DK    -3
#define  R9R3    +4
#define  R3R9    -4
#define  ILLG    99


  double                      N, E;

  int                  in_nr, outnr;
  int                  as_w, as_z;
  int                  result = 0, res = 0, nst, act;

  /* minilabels */
  struct nr_mlb {
    short        trnr;
    char         *s_lab;
  } *pml;

  static struct nr_mlb    mlab[] = {
    /*  0 */ {  0,   "u33"},
    /*  1 */ {  1,   "dks"},
    /*  2 */ {  2,   "s34s"},
    /*  3 */ {  3,   "rt90v"},
    /*  4 */ {  4,   "rt38v"},
    /*stop*/ { -1,   ""}
  };

  static double Cdks_s34s[] = {

    /*deg*/  5.0,

    /*  0*/   1.4289260828e+05,   9.9939330016e-01,   1.1255920917e-10,
    /*  3*/  -2.6375546482e-15,  -9.5149396290e-20,   1.7024223972e-24,

    /*  6*/   3.5038969565e-02,   1.4367473901e-09,   2.5967234457e-15,
    /*  9*/  -4.7318054910e-20,   1.0265769345e-25,

    /* 11*/   1.1268175971e-10,   1.8503698060e-16,  -6.8019174157e-20,
    /* 14*/   9.8847577733e-25,

    /* 15*/   2.6480969928e-15,  -3.8268659605e-22,  -6.6031274142e-25,

    /* 18*/  -2.5246448408e-20,  -1.5598835890e-26,

    /* 20*/  -4.9904833585e-25,

    /* Y0*/     280128.241398,


    /*  0*/   7.2992315151e+04,  -9.9939054228e-01,  -6.1602287732e-10,
    /*  3*/  -1.5060657493e-16,  -3.1481665924e-20,   2.3660238855e-25,

    /*  6*/   3.5040338291e-02,   1.3005431284e-10,   2.2046418037e-15,
    /*  9*/  -3.0675854396e-20,  -3.0843518371e-25,

    /* 11*/   9.2088671194e-10,  -4.9324983810e-15,  -1.1615028246e-19,
    /* 14*/   2.2834235884e-24,

    /* 15*/  -1.7516677174e-15,   5.6527449255e-21,   2.0709971943e-26,

    /* 18*/  -5.3155865196e-20,   1.2063364265e-24,

    /* 20*/   5.7367077414e-25,

    /* X0*/     103637.116675
  };

  static double Cs34s_dks[] = {

    /*deg*/  5.0,

    /*  0*/   2.8012829414e+05,   9.9937856026e-01,  -1.9513137375e-10,
    /*  3*/   2.6059058957e-15,   9.8448661849e-20,  -1.7223526525e-24,

    /*  6*/   3.5038549546e-02,   1.4331049899e-09,   2.7227293861e-15,
    /*  9*/  -3.8294077311e-20,  -9.0065980349e-26,

    /* 11*/  -4.0647703863e-11,  -3.5589933425e-16,   6.7644775026e-20,
    /* 14*/  -9.1814477971e-25,

    /* 15*/   2.6300007633e-15,  -2.6376701341e-22,  -6.8625063399e-25,

    /* 18*/   2.6302016560e-20,   6.7653023582e-26,

    /* 20*/  -4.8874165220e-25,

    /* Y0*/     142892.658020,


    /*  0*/   1.0363703201e+05,  -9.9938131800e-01,  -6.2049147588e-10,
    /*  3*/   3.2453856216e-16,  -2.9559528487e-20,  -2.6636721706e-25,

    /*  6*/   3.5039918195e-02,   2.8136082701e-11,   2.5067644278e-15,
    /*  9*/   2.6769767533e-20,  -4.2152089805e-25,

    /* 11*/   9.1726367185e-10,   4.6753315432e-15,  -1.1741186042e-19,
    /* 14*/  -2.2514440015e-24,

    /* 15*/  -1.8281255006e-15,  -6.1723492089e-21,   6.3467999989e-26,

    /* 18*/  -4.9554614249e-20,  -1.1100655258e-24,

    /* 20*/   5.5422468524e-25,

    /* X0*/      72992.401611
  };

  static double Cdks_rt90[] = {

    /*deg*/  5.0,

    /*  0*/   6.1747948567e+06,   9.9998783779e-01,   1.5940396333e-11,
    /*  3*/   1.0337399545e-15,  -3.7284520909e-20,   3.2628163288e-25,

    /*  6*/  -1.0903628014e-02,  -1.2548572515e-09,   2.8768409041e-15,
    /*  9*/  -1.6904543175e-20,  -1.5899486081e-25,

    /* 11*/   3.8016550550e-11,   1.6683326548e-16,   5.7311606657e-20,
    /* 14*/  -3.6452206429e-25,

    /* 15*/   1.6835914026e-15,   4.5431539298e-20,  -2.6062884625e-24,

    /* 18*/   4.1532363135e-21,   1.7339935314e-25,

    /* 20*/  -5.6876400192e-25,

    /* Y0*/     280194.110447,


    /*  0*/  -6.2969003311e+04,   9.9998753582e-01,  -6.4777388625e-10,
    /*  3*/   1.1319584530e-15,   3.4141233289e-20,  -7.3270746904e-25,

    /*  6*/   1.0903948348e-02,  -8.0911669247e-11,  -1.5328047431e-15,
    /*  9*/   3.1715288158e-21,   1.1071955896e-24,

    /* 11*/   5.4163431301e-10,   2.1819402457e-15,   4.5248719604e-20,
    /* 14*/  -1.8015985560e-24,

    /* 15*/  -2.7581065101e-16,  -1.3077384273e-20,   3.1952814758e-26,

    /* 18*/  -2.5451298641e-21,  -1.5733211883e-25,

    /* 20*/   1.3905974518e-25,

    /* X0*/     103611.774935
  };

  static double Crt90_dks[] = {

    /*deg*/  5.0,

    /*  0*/   2.8019408707e+05,   9.9989327957e-01,  -3.5528878828e-11,
    /*  3*/  -9.9938280560e-16,   3.7098159102e-20,  -3.2927059336e-25,

    /*  6*/   1.0902600278e-02,   1.2553866970e-09,  -2.9297346890e-15,
    /*  9*/   1.9893255282e-20,   1.3582225800e-25,

    /* 11*/  -1.7263397716e-11,  -1.5601935222e-16,  -5.5683655252e-20,
    /* 14*/   2.8446664438e-25,

    /* 15*/  -1.6974520412e-15,  -4.6481229699e-20,   2.6432668206e-24,

    /* 18*/  -5.0209661448e-21,  -1.5939706255e-25,

    /* 20*/   5.7471147879e-25,

    /* Y0*/    6174794.832972,


    /*  0*/   1.0361180354e+05,   9.9989358153e-01,   6.4864879579e-10,
    /*  3*/  -1.0960199003e-15,  -3.4110894914e-20,   7.1422153877e-25,

    /*  6*/  -1.0902920538e-02,   4.1265461581e-11,   1.5234643126e-15,
    /*  9*/  -2.1544385880e-21,  -1.1051479016e-24,

    /* 11*/  -5.4194406360e-10,  -2.1756541186e-15,  -4.4097149605e-20,
    /* 14*/   1.8188860874e-24,

    /* 15*/   3.1061085385e-16,   1.3948499597e-20,  -8.7958282061e-26,

    /* 18*/   1.9942067486e-21,   1.4942610070e-25,

    /* 20*/  -1.3715927206e-25,

    /* X0*/     -62968.974963
  };

  static double Cdks_u33[] = {

    /*deg*/  5.0,

    /*  0*/   6.1753449582e+06,   9.9938439361e-01,  -1.1367078869e-10,
    /*  3*/  -2.5403175887e-18,  -8.8557210585e-23,   2.2277681714e-27,

    /*  6*/  -3.1717191777e-02,  -3.3839917838e-09,   3.8851265591e-16,
    /*  9*/   3.3223268716e-23,  -8.4662796894e-29,

    /* 11*/   1.1370608227e-10,  -1.9568055751e-17,   6.4217089450e-23,
    /* 14*/   6.0931366111e-27,

    /* 15*/  -1.2644730528e-16,  -3.7824395200e-22,   2.7144566784e-27,

    /* 18*/   1.5782322963e-22,   7.6506299524e-27,

    /* 20*/  -7.0430430171e-27,

    /* Y0*/     280128.241398,


    /*  0*/   3.4637726267e+05,   9.9938439595e-01,  -1.6914145600e-09,
    /*  3*/  -1.1676997710e-17,  -8.6343407334e-22,   1.4412664249e-26,

    /*  6*/   3.1717193570e-02,  -2.2787480084e-10,   3.8876392896e-16,
    /*  9*/   4.1378571514e-22,  -3.2155379374e-27,

    /* 11*/   1.6918049543e-09,   1.5399751014e-17,   1.5736069348e-22,
    /* 14*/   2.8049985250e-27,

    /* 15*/  -1.2446684650e-16,   2.8608732735e-22,  -9.6647316257e-27,

    /* 18*/   1.1920556850e-22,  -5.7354022787e-27,

    /* 20*/  -2.5467960337e-27,

    /* X0*/     103637.116675
  };

  static double Cu33_dks[] ={

    /*deg*/  5.0,

    /*  0*/   2.8012815233e+05,   9.9960915963e-01,  -4.7652483036e-11,
    /*  3*/   1.3055384772e-17,   8.6961602906e-23,  -2.1559084190e-27,

    /*  6*/   3.1724324725e-02,   3.3914456063e-09,  -3.8749167545e-16,
    /*  9*/  -2.5599569341e-23,   2.8681204538e-28,

    /* 11*/   4.7604868483e-11,  -1.2206612925e-17,  -1.1435964544e-22,
    /* 14*/  -5.5085499756e-27,

    /* 15*/   1.2659688241e-16,   3.7511098464e-22,  -2.3110835061e-27,

    /* 18*/  -1.1765329266e-22,  -8.7424822723e-27,

    /* 20*/   6.3318728414e-27,

    /* Y0*/    6175344.865414,


    /*  0*/   1.0363723638e+05,   9.9960915729e-01,   1.6951499880e-09,
    /*  3*/   1.0720101020e-18,   8.5293649463e-22,  -1.4494347944e-26,

    /*  6*/  -3.1724326518e-02,  -9.4735021807e-11,  -3.8896493839e-16,
    /*  9*/  -5.4865717937e-22,   5.6091436062e-27,

    /* 11*/  -1.6955392782e-09,   1.6191004376e-17,  -1.3592013191e-22,
    /* 14*/  -2.3148081289e-27,

    /* 15*/   1.2403877826e-16,  -2.8628678133e-22,   1.0813749749e-26,

    /* 18*/  -1.1295254048e-22,   5.4803768447e-27,

    /* 20*/   2.4390174432e-27,

    /* X0*/     346377.379477
  };

  /* Constants given by LMV, Geodetiska Utvecklingsenheten */
  /* Transformation RT38 5 GON V 0.0 -> RT90 5 GON V 0.0   */
  /* Fax of 7 MAR 1995. Max error 0.360 m at stn 712861.0  */
  double        Crt38_rt90[] = {
    /* N38_0, E38_0 */           0.0000,         0.0000,
    /* N90_0, E90_0 */         -21.7489,      +154.4398,
    /* a,     b     */   +1.00000318451, -0.00002456195
  };

  struct act_nst {
    short     action;
    short     nstate;
  };
  static THREAD_SAFE struct act_nst     *ptab;

  static struct act_nst astab[] = {

    /* U33: 0 */
    /* input    u33       dks      s34s      rt90v     rt38v   */
    /* state no. 0         1         2         3         4     */
    /* u33 */ {IDNT,0}, {DK_U,0}, {S_DK,1}, {R_DK,1}, {R3R9,3},

    /* DKS: 1 */
    /* input    u33       dks      s34s      rt90v     rt38v   */
    /* state no. 0         1         2         3         4     */
    /* dks */ {U_DK,1}, {IDNT,1}, {S_DK,1}, {R_DK,1}, {R3R9,3},

    /* S34: 2 */
    /* input    u33       dks      s34s      rt90v     rt38v   */
    /* state no. 0         1         2         3         4     */
    /* u33 */ {U_DK,1}, {DK_S,2}, {IDNT,2}, {R_DK,1}, {R3R9,3},

    /* RT90: 3 */
    /* input    u33       dks      s34s      rt90v     rt38v   */
    /* state no. 0         1         2         3         4     */
    /* rt90*/ {U_DK,1}, {DK_R,3}, {S_DK,1}, {IDNT,3}, {R3R9,3},

    /* RT38: 4 */
    /* input    u33       dks      s34s      rt90v     rt38v   */
    /* state no. 0         1         2         3         4     */
    /* rt38*/ {U_DK,1}, {DK_R,3}, {S_DK,1}, {R9R3,4}, {IDNT,4}
  };

  static struct act_nst      iltab[] = {
    {ILLG, 0}
  };


/* dks_tr   ver 1999.01           # page 10   12 Jan 1999 13 41 */


  

    as_z    = sizeof(astab)/sizeof(struct act_nst);
    as_w    = (int) sqrt(1.0*as_z);
     ;

    if (as_z != as_w*as_w) {
      return((tr_error==NULL) ? TRF_PROGR_ :
              t_status(tr_error, usertxt,
              "dks_tr(state/action tabeller ej ok)", TRF_PROGR_));
    }
  

  /* lookup outlab */
  /*_______________*/
  
    /* insert out-system and clear in-system */
    for (pml = mlab, outnr = -1; pml->trnr != -1; pml++) {
      if (!strcmp(pml->s_lab, outlab)) {
	outnr = pml->trnr;
        break;
      }
    }
  

  /* lookup in_lab */
  /*_______________*/
  
    /* insert in-system */
    for (pml = mlab, in_nr = -1; pml->trnr != -1; pml++) {
      if (!strcmp(pml->s_lab, in_lab)) {
        in_nr = pml->trnr;
        break;
      }
    }
  


  /* Preserve input for reverse check */
  N = N_in;
  E = E_in;

  ptab = astab + as_w*outnr;

  if (outnr < 0 || in_nr < 0) {
    ptab  = iltab;
    in_nr = 0;
  }

  /* transformation module */
  *N_out = *E_out = 0.0;
  nst    = in_nr;
  do {
    act = (ptab+nst)->action;
    nst = (ptab+nst)->nstate;

    switch (act) {

    case DK_U: /* dks->u33 */
      if (NMIN < N && N < NMAX && EMIN < E && E < EMAX)
        res = gen_pol(Cdks_u33, Cu33_dks, N, E, &N, &E);
      else res = TRF_AREA_;
      break;

    case U_DK: /* u33->dks */
      res = gen_pol(Cu33_dks, Cdks_u33, N, E, &N, &E);
      if (N < NMIN || NMAX < N || E < EMIN || EMAX < E)
          res = TRF_AREA_;
      break;

    case DK_S: /* dks->s34s */
      if (NMIN < N && N < NMAX && EMIN < E && E < EMAX)
        res = gen_pol(Cdks_s34s, Cs34s_dks, N, E, &N, &E);
      else res = TRF_AREA_;
      break;

    case S_DK: /* s34s->dks */
      res = gen_pol(Cs34s_dks, Cdks_s34s, N, E, &N, &E);
      if (N < NMIN || NMAX < N || E < EMIN || EMAX < E)
          res = TRF_AREA_;
      break;

    case DK_R: /* dks->rt90 */
      if (NMIN < N && N < NMAX && EMIN < E && E < EMAX)
        res = gen_pol(Cdks_rt90, Crt90_dks, N, E, &N, &E);
      else res = TRF_AREA_;
      break;

    case R_DK: /* rt90->dks */
      res = gen_pol(Crt90_dks, Cdks_rt90, N, E, &N, &E);
      if (N < NMIN || NMAX < N || E < EMIN || EMAX < E)
          res = TRF_AREA_;
      break;

    case R9R3: /* rt90->rt38 */
      res = hlm_tr(Crt38_rt90, -1, N, E, &N, &E, usertxt, tr_error);
      break;

    case R3R9: /* rt38->rt90 */
      res = hlm_tr(Crt38_rt90, +1, N, E, &N, &E, usertxt, tr_error);
      break;

    case ILLG:
      res = TRF_ILLEG_;
      act = 0;
      break;

    case IDNT: /* return output */
      *N_out = N;
      *E_out = E;
      if (res == 0) return(0);
      break;
    } /* end switch(act) */

    if (res < result) result = res;

  } while (act && result >= TRF_TOLLE_);

  return((tr_error==NULL) ? result :
          t_status(tr_error, usertxt, "dks_tr", result,
          "m ", "", N_in, E_in));

#undef   TOL

#undef   NMIN
#undef   NMAX
#undef   EMIN
#undef   EMAX

#undef  IDNT
#undef  DK_U
#undef  U_DK
#undef  DK_S
#undef  S_DK
#undef  DK_R
#undef  R_DK
#undef  R9R3
#undef  R3R9
#undef  ILLG

}
