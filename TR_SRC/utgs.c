

/*  utgs   ver 1999.01            # page 1   12 Jan 1999 12 54 */


/* Copyright (c) 1999 LG,   Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of LG, Kort-og  */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include <stdio.h>
#include <math.h>
#include "geo_lab.h"

#define  GSTOLMS   0.010

int                       utgs(
/*___________________________*/
double                    Nin,
double                    Ein,
double                    *yout,
double                    *xout,
int                       landsdel,
int                       direct,
char                      *usertxt,
FILE                      *tr_error
)

{
  /* utgs: int function giving the transformations

          utm zone 32 <-> gs system (Jylland and Sjlland)
                         and
          utm zone 33 <-> gsb system (Bornholm).

The transformation may have errors up to 1 m, but the local
consistency and computing precision is of the order of 1 cm.
utgs returns the same value as returned in yout.

Nin, Ein input coordinates (utm or gs(b)), double,
in units of metres. Nin and Ein are called by value.

yout, xout output coordinates in (gs(b) or utm), double = lon in
units of metres. yout and xout are called by reference.

landsdel, call by value indicating

          1 (or 2) => gs and utm zone 32 i.e. Jylland + Sjlland,
          's', 'j', or 'd' gives the same effect.

          3 (or 'b')  => gsb and utm zone 33 i.e. Bornholm.

direct, call by value indicating
          direct > 0  => transformation utm --> gs
          direct < 0  => transformation gs --> utm
direct = 0  => no transformation, and alarm

produces UTM 32 Northing and Easting of the gs coordinates y,x
in Sealand or Jutland. Note the & operator on N1, E1.
*/


/*  utgs   ver 1999.01            # page 2   12 Jan 1999 12 54 */


#include       "t_status.h"

  double        N, E, n, e, w, tol = GSTOLMS;
  double        N_max, N_min, E_max, E_min;
  double       *c, *cb = NULL, *cw = NULL, *cbu, *cbg;
  int           i, d, t;
  int           res = 0;

  static double    Cu32gs[] = {
    /*lng*/   -16.0,              /*tol*/  GSTOLMS,
    /*N,E*/    6206216.610000,     500000.000000,
    /* 0 */     112123.507889,      85938.100252,
    /* 1 */    1.000186984e+00,   -1.994422057e-02,
    /* 2 */   -6.567625485e-13,   -1.298008380e-13,
    /* 3 */    4.091732043e-15,   -8.187299687e-17,
    /* 4 */    2.351689064e-22,   -4.394110355e-24,
    /* 5 */    5.021865838e-29,   -1.008031811e-30
  };
  static double    Cu33gsb[] = {
    /*lng*/   -14.0,              /*tol*/  GSTOLMS,
    /*N,E*/    6107000.000000,      493000.000000,
    /* 0 */     17606.901573,      21150.097571,
    /* 1 */    1.000528635e+00,    9.507307614e-04,
    /* 2 */   -1.828526053e-09,    3.828207311e-11,
    /* 3 */    2.481016058e-15,    1.683466885e-14,
    /* 4 */    1.795149378e-19,   -9.161067528e-19
  };
  static double    Cgsu32[] = {
    /*lng*/   -14.0,                /*tol*/  GSTOLMS,
    /*N,E*/     112177.020000,       85948.680000,
    /* 0 */    6206269.880000,      499988.360000,
    /* 1 */    9.994156600e-01,   -1.992884000e-02,
    /* 2 */    0.000000000e+00,    0.000000000e+00,
    /* 3 */   -4.079000000e-15,    2.440000000e-16,
    /* 4 */   -2.340000000e-22,    1.900000000e-23
  };
  static double    Cgsbu33[] = {
    /*lng*/   -14.0,              /*tol*/  GSTOLMS,
    /*N,E*/       18000.000000,      21000.000000,
    /* 0 */     6107392.748060,     493150.391692,
    /* 1 */    9.994721570e-01,    9.503132652e-04,
    /* 2 */    1.815515582e-09,    6.128234254e-11,
    /* 3 */   -2.261322580e-15,    1.524694510e-14,
    /* 4 */   -1.745394113e-19,   -9.146885316e-19
  };

  N = Nin;
  E = Ein;


/*  utgs   ver 1999.01            # page 3   12 Jan 1999 17 59 */


  switch (landsdel)
  {
  case 1:  /* jylland */
  case 2:  /* sjlland */
  case 'j':
  case 's':
  case 'd':
    cbu   = Cu32gs;
    cbg   = Cgsu32;
    if (direct > 0) {
      N_max = 6420000.0;
      N_min = 6036000.0;
      E_max =  763000.0;
      E_min =  425000.0;
    } else {
      /* Changed by frs jun. 2010, MIT
      *  N_max and N_min
      */
      N_max =  326000.0;
      N_min =  -68000.0;
      E_max =  160000.0;
      E_min = -176000.0;
    };
    break;
  case 3:  /*bornholm */
  case 'b':
  cbu = Cu33gsb;
    cbg   = Cgsbu33;
    if (direct > 0) {
      /* Changed by frs jun. 2010, MIT
      *  N_min, E_max and E_min
      */
      N_max = 6141000.0;
      N_min = 6084000.0;
      E_max =  524000.0;
      E_min =  420000.0;
    } else {
      /* Changed by frs jun. 2010, MIT
      *  N_max, E_max and E_min
      */
      N_max =   62000.0;
      N_min =   -1500.0;
      E_max =   52000.0;
      E_min =  -10000.0;
    };
    break;
  default:
    return(t_status(
           tr_error, usertxt, "utgs(landsdel?)", TRF_AREA_));
  } /* end landsdel cases */

  d = ((direct > 0) ? (1) : (direct < 0 ? (-1) : 0));
  if (d > 0) {  /* utm -> gs */
    cb = cbu;
    cw = cbg;
  }
  else if (d < 0) {  /* gs -> utm */
    cb = cbg;
    cw = cbu;
  }
  else res = TRF_ILLEG_;


/*  utgs   ver 1999.01            # page 4   12 Jan 1999 17 59 */


  if (N < N_min || N_max < N || E < E_min || E_max < E) res = TRF_AREA_;

  for (i = -1; i <= 1 && res == 0; ++i)
    if (i) { /* transformation, i == -1 => f, i == 1 => b */
      t   = (int) fabs(*cb);
      c   = cb + t;
      cb++;
      tol = *cb++;
      n   = N - *cb++;
      e   = *cb++ - E;
      E = *--c;
      N = *--c;
      while (c > cb) {
        w = n*E + e*N + *--c;
        N = n*N - e*E + *--c;
        E = w;
      }
    }
    else { /* collect output coord */
      *yout = N;
      *xout = E;
      cb    = cw;
    }

  /* tol-check of results*/
  if (fabs(N - Nin) < tol && fabs (E - Ein) < tol
      && res == 0)   return (0);
  else if (res >= TRF_INACC_) res = TRF_TOLLE_;

  return(t_status(tr_error, usertxt, "utgs", res,
         "m ", "", N, E, N - Nin, E - Ein));

#undef  GSTOLMS
}

