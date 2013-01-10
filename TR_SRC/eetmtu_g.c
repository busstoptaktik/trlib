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
 


/* eetmtu_g   ver 1999.01         # page 1   12 Jan 1999 12 58 */


/* Copyright (c) 1999, LG,  Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of LG, Kort-og  */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include    <stdio.h>
#include    <math.h>
#include    "KmsFncs.h"

int                      eetmtu_g(
/*______________________________*/
int                      direct,
double                   N_in,
double                   E_in,
double                  *N_out,
double                  *E_out,
char                    *usertxt,
FILE                    *tr_error
)

{
/* Definition of the permissible area for the transformation */
#define COORLIM     (2.0e5)        /* +/- 200 km */
#define MAP_LIM     (3.0e5)        /* +/- 300 km */

  double           N, E, n, e, u, v, tol = 0.001;
  double          *cb = NULL, *cw = NULL;
  double          *tcy, *tcx;
  int              res = 0, i, g, r, c, sz;



/* eetmtu_g   ver 1999.01         # page 2   12 Jan 1999 16 30 */


  /*
  eetm27 -> utm35_euref89
  grad:  5 pass: 11
  my =  5.26 cm    my_loss =    +3  y_enp =  +9.2
  mx =  4.76 cm    mx_loss =    +2  x_enp =  +9.5
  red = 5    m_lim_gen:  0.158
  OBS =    169
  */

  static double TM27_U35[] = {
    /*deg*/  5.0,
    /*Poly NORTH :: e-degree =  0  : n-degree =  5 */
    /*  0*/   6.5081549576e+06,   9.9959783377e-01,   2.7399605038e-11,
    /*  3*/  -2.5711147046e-16,  -3.6237714206e-21,   8.9049608556e-27,
    /*Poly NORTH :: e-degree =  1  : n-degree =  4 */
    /*  6*/   2.7603019230e-06,   6.9355309224e-11,   8.2182054156e-16,
    /*  9*/  -5.8388648005e-21,  -4.2781064069e-26,
    /*Poly NORTH :: e-degree =  2  : n-degree =  3 */
    /* 11*/  -2.7704325746e-11,  -2.7059173312e-16,   8.9961029622e-22,
    /* 14*/  -3.8564364490e-27,
    /*Poly NORTH :: e-degree =  3  : n-degree =  2 */
    /* 15*/  -9.7735626432e-18,  -1.6541387725e-21,  -3.0925871356e-26,
    /*Poly NORTH :: e-degree =  4  : n-degree =  1 */
    /* 18*/   1.6559937429e-21,   4.0374117852e-27,
    /*Poly NORTH :: e-degree =  5  : n-degree =  0 */
    /* 20*/   5.2790084701e-27,
    /*tcy*/    6510883.289185,

    /*Poly EAST  :: n-degree =  0  : e-degree =  5 */
    /*  0*/   4.3472327044e+05,   9.9959646956e-01,   2.5648007750e-11,
    /*  3*/  -1.8014902381e-16,  -1.2209891290e-21,  -2.2124418491e-27,
    /*Poly EAST  :: n-degree =  1  : e-degree =  4 */
    /*  6*/  -2.3250196446e-06,   3.4024314085e-11,   3.3484910857e-16,
    /*  9*/  -1.5362319997e-21,  -1.5864376993e-26,
    /*Poly EAST  :: n-degree =  2  : e-degree =  3 */
    /* 11*/  -4.3744416175e-11,  -2.9746331949e-16,  -9.3401479839e-23,
    /* 14*/   1.5671151241e-26,
    /*Poly EAST  :: n-degree =  3  : e-degree =  2 */
    /* 15*/  -5.6419584365e-16,  -3.2584903362e-21,   5.7326769995e-27,
    /*Poly EAST  :: n-degree =  4  : e-degree =  1 */
    /* 18*/   2.4441781312e-21,   2.8306694732e-26,
    /*Poly EAST  :: n-degree =  5  : e-degree =  0 */
    /* 20*/   4.2022824105e-26,
    /*tcx*/    5434819.352771
  };


/* eetmtu_g   ver 1999.01         # page 3   12 Jan 1999 16 30 */


  /*
  utm35_euref89 -> eetm27
  grad:  5 pass: 11
  my =  5.26 cm    my_loss =    +3  y_enp =  +9.2
  mx =  4.76 cm    mx_loss =    +2  x_enp =  +9.6
  red = 5    m_lim_gen:  0.158
  OBS =    169
  */

  static double U35_TM27[] = {
    /*deg*/  5.0,
    /*Poly NORTH :: e-degree =  0  : n-degree =  5 */
    /*  0*/   6.5108832527e+06,   1.0004023280e+00,  -2.7432817529e-11,
    /*  3*/   2.5752540412e-16,   3.6310717560e-21,  -8.9268606363e-27,
    /*Poly NORTH :: e-degree =  1  : n-degree =  4 */
    /*  6*/  -2.7625127728e-06,  -6.9439074095e-11,  -8.2315878787e-16,
    /*  9*/   5.8506121914e-21,   4.2885200806e-26,
    /*Poly NORTH :: e-degree =  2  : n-degree =  3 */
    /* 11*/   2.7738556980e-11,   2.7104088749e-16,  -9.0148716185e-22,
    /* 14*/   3.8654737393e-27,
    /*Poly NORTH :: e-degree =  3  : n-degree =  2 */
    /* 15*/   9.7863608458e-18,   1.6574737960e-21,   3.1001801812e-26,
    /*Poly NORTH :: e-degree =  4  : n-degree =  1 */
    /* 18*/  -1.6593880664e-21,  -4.0477253174e-27,
    /*Poly NORTH :: e-degree =  5  : n-degree =  0 */
    /* 20*/  -5.2919221081e-27,
    /*tcy*/    6508154.921128,

    /*Poly EAST  :: n-degree =  0  : e-degree =  5 */
    /*  0*/   5.4348193778e+06,   1.0004036933e+00,  -2.5679088530e-11,
    /*  3*/   1.8044218371e-16,   1.2234476929e-21,   2.2177462671e-27,
    /*Poly EAST  :: n-degree =  1  : e-degree =  4 */
    /*  6*/   2.3269000980e-06,  -3.4066727376e-11,  -3.3539446326e-16,
    /*  9*/   1.5394140327e-21,   1.5903503382e-26,
    /*Poly EAST  :: n-degree =  2  : e-degree =  3 */
    /* 11*/   4.3797137816e-11,   2.9793672726e-16,   9.3630057034e-23,
    /* 14*/  -1.5709048111e-26,
    /*Poly EAST  :: n-degree =  3  : e-degree =  2 */
    /* 15*/   5.6511125151e-16,   3.2651636816e-21,  -5.7468077381e-27,
    /*Poly EAST  :: n-degree =  4  : e-degree =  1 */
    /* 18*/  -2.4490949200e-21,  -2.8375107641e-26,
    /*Poly EAST  :: n-degree =  5  : e-degree =  0 */
    /* 20*/  -4.2125049312e-26,
    /*tcx*/     434723.295482
  };


/* eetmtu_g   ver 1999.01         # page 4   12 Jan 1999 16 30 */


  /*
  eelmne -> utm35_euref89
  grad:  3 pass: 2
  my = 11.30 cm    my_loss =    +1  y_enp = +10.1
  mx = 11.87 cm    mx_loss =    +1  x_enp = +10.1
  red = 0    m_lim_gen:  0.356
  OBS =     21
  */

  static double LMNE_U35[] = {
    /*deg*/  3.0,
    /*Poly NORTH :: e-degree =  0  : n-degree =  3 */
    /*  0*/   6.5149374332e+06,   9.9930675247e-01,   4.0618148602e-10,
    /*  3*/  -4.7754332797e-15,
    /*Poly NORTH :: e-degree =  1  : n-degree =  2 */
    /*  4*/  -2.9840520250e-02,  -2.6724808560e-09,  -4.9045952065e-17,
    /*Poly NORTH :: e-degree =  2  : n-degree =  1 */
    /*  7*/  -4.2289289463e-10,   1.2141360216e-14,
    /*Poly NORTH :: e-degree =  3  : n-degree =  0 */
    /*  9*/  -1.3533250197e-16,
    /*tcy*/     -37829.516610,
    /*tcy       162170.483390, */

    /*Poly EAST  :: n-degree =  0  : e-degree =  3 */
    /*  0*/   3.9023210155e+05,   9.9929654375e-01,  -1.3492114666e-09,
    /*  3*/   4.3761022443e-15,
    /*Poly EAST  :: n-degree =  1  : e-degree =  2 */
    /*  4*/   2.9842016858e-02,   8.3108549196e-10,  -3.3611252384e-16,
    /*Poly EAST  :: n-degree =  2  : e-degree =  1 */
    /*  7*/   1.2877410302e-09,  -1.1781277735e-14,
    /*Poly EAST  :: n-degree =  3  : e-degree =  0 */
    /*  9*/   7.3519445904e-17,
    /*tcx*/       5948.442690
    /*tcx       205948.442690 */
  };


/* eetmtu_g   ver 1999.01         # page 5   12 Jan 1999 16 30 */


  /*
  utm35_euref89 -> eelmne
  grad:  3 pass: 2
  my = 11.33 cm    my_loss =    +1  y_enp = +10.2
  mx = 11.70 cm    mx_loss =    +1  x_enp = +10.1
  red = 0    m_lim_gen:  0.351
  OBS =     21
  */

  static double U35_LMNE[] = {
    /*deg*/  3.0,
    /*Poly NORTH :: e-degree =  0  : n-degree =  3 */
    /*  0     1.6216392043e+05, */
    /*  0*/  -3.7836079572e+04,   9.9980215553e-01,  -5.2350312599e-10,
    /*  3*/   4.7430743596e-15,
    /*Poly NORTH :: e-degree =  1  : n-degree =  2 */
    /*  4*/   2.9855581886e-02,   2.5891838708e-09,   1.5508683113e-15,
    /*Poly NORTH :: e-degree =  2  : n-degree =  1 */
    /*  7*/   5.4092966386e-10,  -1.2056876184e-14,
    /*Poly NORTH :: e-degree =  3  : n-degree =  0 */
    /*  9*/  -3.5288946248e-16,
    /*tcy*/    6514931.098962,

    /*Poly EAST  :: n-degree =  0  : e-degree =  3 */
    /*  0     3.9023210155e+05, */
    /*  0*/   5.9407799458e+03,   9.9981231371e-01,   1.3075419492e-09,
    /*  3*/  -4.3365248911e-15,
    /*Poly EAST  :: n-degree =  1  : e-degree =  2 */
    /*  4*/  -2.9857021039e-02,  -1.0651610105e-09,   1.7737634870e-15,
    /*Poly EAST  :: n-degree =  2  : e-degree =  1 */
    /*  7*/  -1.2464407783e-09,   1.1668349082e-14,
    /*Poly EAST  :: n-degree =  3  : e-degree =  0 */
    /*  9*/  -5.7177739731e-16,
    /*tcx*/     390224.248657
  };


/* eetmtu_g   ver 1999.01         # page 6   12 Jan 1999 16 30 */


  /*
  tm27_eeold42 -> eetm27
  grad:  4 pass: 2
  my =  4.01 cm    my_loss =    +3  y_enp =  +9.9
  mx =  8.14 cm    mx_loss =    +3  x_enp = +10.3
  red = 0    m_lim_gen:  0.326
  OBS =     91
  */

  static double OLD_TM27[] = {
    /*deg*/  4.0,
    /*Poly NORTH :: e-degree =  0  : n-degree =  4 */
    /*  0*/   6.5600346778e+06,   9.9999735500e-01,   1.4991190712e-11,
    /*  3*/   7.1224994506e-16,   2.9253078568e-21,
    /*Poly NORTH :: e-degree =  1  : n-degree =  3 */
    /*  5*/  -2.5915993039e-06,  -3.3179319142e-11,   2.2494187864e-16,
    /*  8*/   2.5511455264e-21,
    /*Poly NORTH :: e-degree =  2  : n-degree =  2 */
    /*  9*/   4.9577042704e-12,   2.1704800704e-16,   4.2820693727e-21,
    /*Poly NORTH :: e-degree =  3  : n-degree =  1 */
    /* 12*/   1.3088860081e-16,   2.5366923456e-21,
    /*Poly NORTH :: e-degree =  4  : n-degree =  0 */
    /* 14*/  -1.1936989473e-21,
    /*tcy*/    6560033.991631,

    /*Poly EAST  :: n-degree =  0  : e-degree =  4 */
    /*  0*/   5.4290211794e+06,   1.0000028339e+00,   2.2822179847e-11,
    /*  3*/  -2.7658782678e-17,  -2.0347483696e-21,
    /*Poly EAST  :: n-degree =  1  : e-degree =  3 */
    /*  5*/   1.9119124282e-06,   2.2936868885e-11,  -5.1826929397e-16,
    /*  8*/  -8.4714035064e-22,
    /*Poly EAST  :: n-degree =  2  : e-degree =  2 */
    /*  9*/   1.8830295510e-11,   7.3457796287e-16,   6.6172887814e-22,
    /*Poly EAST  :: n-degree =  3  : e-degree =  1 */
    /* 12*/  -1.2141724613e-16,   7.8881808810e-21,
    /*Poly EAST  :: n-degree =  4  : e-degree =  0 */
    /* 14*/   3.1747835468e-22,
    /*tcx*/     429022.848847
  };


/* eetmtu_g   ver 1999.01         # page 7   12 Jan 1999 16 30 */


  /*
  eetm27 -> tm27_eeold42
  grad:  4 pass: 2
  my =  4.01 cm    my_loss =    +3  y_enp =  +9.9
  mx =  8.14 cm    mx_loss =    +3  x_enp = +10.2
  red = 0    m_lim_gen:  0.326
  OBS =     91
  */

  static double TM27_OLD[] = {
    /*deg*/  4.0,
    /*Poly NORTH :: e-degree =  0  : n-degree =  4 */
    /*  0*/   6.5600339911e+06,   1.0000026450e+00,  -1.4991185097e-11,
    /*  3*/  -7.1225174272e-16,  -2.9253180019e-21,
    /*Poly NORTH :: e-degree =  1  : n-degree =  3 */
    /*  5*/   2.5915996931e-06,   3.3179080334e-11,  -2.2494943000e-16,
    /*  8*/  -2.5511764785e-21,
    /*Poly NORTH :: e-degree =  2  : n-degree =  2 */
    /*  9*/  -4.9577004664e-12,  -2.1704735901e-16,  -4.2820381261e-21,
    /*Poly NORTH :: e-degree =  3  : n-degree =  1 */
    /* 12*/  -1.3088795283e-16,  -2.5366788856e-21,
    /*Poly NORTH :: e-degree =  4  : n-degree =  0 */
    /* 14*/   1.1936868430e-21,
    /*tcy*/    6560034.677271,

    /*Poly EAST  :: n-degree =  0  : e-degree =  4 */
    /*  0*/   4.2902288579e+05,   9.9999716608e-01,  -2.2822008425e-11,
    /*  3*/   2.7661432558e-17,   2.0347288127e-21,
    /*Poly EAST  :: n-degree =  1  : e-degree =  3 */
    /*  5*/  -1.9118860782e-06,  -2.2936054876e-11,   5.1825686846e-16,
    /*  8*/   8.4709292689e-22,
    /*Poly EAST  :: n-degree =  2  : e-degree =  2 */
    /*  9*/  -1.8829718960e-11,  -7.3457712551e-16,  -6.6174284029e-22,
    /*Poly EAST  :: n-degree =  3  : e-degree =  1 */
    /* 12*/   1.2140995883e-16,  -7.8881505696e-21,
    /*Poly EAST  :: n-degree =  4  : e-degree =  0 */
    /* 14*/  -3.1753244878e-22,
    /*tcx*/    5429021.216300
  };


/* eetmtu_g   ver 1999.01         # page 8   12 Jan 1999 16 30 */


  /* Collect start values */
  N = N_in;
  E = E_in;

  switch (direct) {
  case  1 : /* Forward == TM27 -> U35 */
    cb  = TM27_U35;
    cw  = U35_TM27;
    if (N < 6196000 || 6796000 < N ||
        E < 5097000 || 5697000 < E) res = TRF_AREA_;
    else
    if (N < 6296000 || 6696000 < N ||
        E < 5197000 || 5597000 < E) res = TRF_INACC_;
    break;
  case -1 : /* Reverse == U35 -> TM27 */
    cb  = U35_TM27;
    cw  = TM27_U35;
    if (N < 6194000 || 6794000 < N ||
        E <   97000 ||  697000 < E) res = TRF_AREA_;
    else
    if (N < 6294000 || 6694000 < N ||
        E <  197000 ||  597000 < E) res = TRF_INACC_;
    break;
  case  2 : /* Forward == OLD -> TM27 */
    cb  = OLD_TM27;
    cw  = TM27_OLD;
    if (N < 6196000 || 6796000 < N ||
        E <   97000 ||  697000 < E) res = TRF_AREA_;
    else
    if (N < 6296000 || 6696000 < N ||
        E <  197000 ||  597000 < E) res = TRF_INACC_;
    break;
  case -2 : /* Reverse == TM27 -> OLD */
    cb  = TM27_OLD;
    cw  = OLD_TM27;
    if (N < 6196000 || 6796000 < N ||
        E < 5097000 || 5697000 < E) res = TRF_AREA_;
    else
    if (N < 6296000 || 6696000 < N ||
        E < 5197000 || 5597000 < E) res = TRF_INACC_;
    break;


/* eetmtu_g   ver 1999.01         # page 9   12 Jan 1999 16 30 */


  case  3 : /* Forward == LMNE -> U35 */
    cb  = LMNE_U35;
    cw  = U35_LMNE;
    tol = 0.04;
    if (N < -156000 ||  444000 < N ||
        E <  -88000 ||  512000 < E) res = TRF_AREA_;
    else
    if (N <  -56000 ||  344000 < N ||
        E <   12000 ||  412000 < E) res = TRF_INACC_;
    break;
  case -3 : /* Reverse == U35 -> LMNE */
    cb  = U35_LMNE;
    cw  = LMNE_U35;
    tol = 0.04;
    if (N < 6194000 || 6794000 < N ||
        E <   97000 ||  697000 < E) res = TRF_AREA_;
    else
    if (N < 6294000 || 6694000 < N ||
        E <  197000 ||  597000 < E) res = TRF_INACC_;
    break;
  default :
    direct = 0;
  }

  if (direct) {

    /* Transformation loop */
    for (i = -1; i <= 1 && (res >= TRF_TOLLE_); ++i)
      if (i) {

        /* Prepare for double Horner */
        g   = (int) *cb;
        sz  = (g + 1)*(g + 2)/2 + 1;
        tcy = cb + sz;


        tcx = tcy  + sz;
        /* Double Horner's scheme */
        /* N = n*Cy*e -> yout, E = e*Cx*n -> xout */

        n   = N - *tcy;
        e   = E - *tcx;

        for (  r = g, N = *--tcy, E = *--tcx; r >  0; r--) {
          for (c = g, u = *--tcy, v = *--tcx; c >= r; c--) {
            u = n*u + *--tcy;
            v = e*v + *--tcx;
          }
          N = e*N + u;
          E = n*E + v;
        }
      }
      else { /* collect output coord,switch to reverse checking */
        *N_out = N;
        *E_out = E;
        cb    = cw;
      }


/* eetmtu_g   ver 1999.01         # page 10  12 Jan 1999 16 30 */


    if (res == 0) {
      /* tol-check of results*/
      if (fabs(N - N_in) < tol && fabs(E - E_in) < tol) return (0);
      else res = TRF_TOLLE_; /* TOLERANCE EXCEEDED */
    }
  }
  else res = TRF_PROGR_;  /* UNDEFINED DIRECTION */
  return((tr_error==NULL || res==0) ? res :
          t_status(tr_error, usertxt, "eetmtu_g", res,
          "m ", "", N, E, N - N_in, E - E_in));

#undef   COORLIM
#undef   MAP_LIM

}

