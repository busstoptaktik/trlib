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
 


/*  s34tkk   ver 1999.01          # page 1   12 Jan 1999 12 54 */


/* Copyright (c) 1999 LG, Kort-og Matrikelstyrelsen, Denmark   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of LG, Kort-og  */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include    <stdio.h>
#include    <math.h>
#include    "KmsFncs.h"

int                        s34stkk(
/*_______________________________*/
double                     y,
double                     x,
double                     *yout,
double                     *xout,
int                        direct,
char                       *usertxt,
FILE                       *tr_error
)

/*
s34stkk: int function giving the transformations

          s34 Sjælland <-> Kbh. kommunes system

The transformation may have errors up to 0.5 m, but the local
consistency is generally much better. The computing precision is
better than 0.5 mm.
s34stkk returns 0 for ok and a negative for not ok.

y, x input coordinates (s34j or kk), double,
in units of metres. y and x are called by value.

yout, xout output coordinates in (kk or s34j), double
in units of metres. yout and xout are called by reference.

direct, call by value indicating

          direct > 0  => transformation s34j --> kk
          direct < 0  => transformation kk --> s34j
          direct = 0  => no transformation, alarm

  prog: Olaf Andersson jan 1979
  modification (s34s->kk): WLW, Januar 1989
  modification           : KE,  Januar 1999
*/


/*  s34tkk   ver 1999.01          # page 2   12 Jan 1999 12 54 */


{

double            N, E, n, e, w, tol = 0.005;
double           *c, *cb = NULL, *cw = NULL;
int               i, d, t;
int               res = 0;

  static  double    C34kk[] =
  {
    /*lng*/   -14.0,                /*tol*/  0.005,
    /*N,E*/    140000.000000,      72000.000000,
    /* 0 */    7.693586910e+04,   -1.379401748e+05,
    /* 1 */    9.999741493e-01,   -3.210304882e-05,
    /* 2 */   -6.390451712e-10,   -1.133610356e-09,
    /* 3 */    4.949851810e-15,    2.548578925e-14,
    /* 4 */    9.257340940e-18,   -1.247871289e-18
  };

  static  double    Ckk34[] =
  {
    /*lng*/   -14.0,                /*tol*/  0.005,
    /*N,E*/     77000.000000,     -137000.000000,
    /* 0 */    1.400641017e+05,    7.294020022e+04,
    /* 1 */    1.000023833e+00,    3.354660075e-05,
    /* 2 */    7.578997888e-10,    1.101628590e-09,
    /* 3 */   -1.200882360e-14,   -5.998822689e-14,
    /* 4 */   -9.258611225e-18,    1.246673174e-18
  };

  d = ((direct > 0) ? (1) : (direct < 0 ? (-1) : 0));

  if (d > 0) { /* s34j -> kk */
    cb = C34kk;
    cw = Ckk34;
  }
  else if (d < 0) { /* kk -> s3j */
    cb = Ckk34;
    cw = C34kk;
  }
  else res = TRF_ILLEG_;

  N = y;
  E = x;
  for (i = -1; i <= 1 && res == 0; ++i)
    if (i) { /* transformation, i == -1 => f, i == 1 => b */
      t   = (int) fabs(*cb);
      c   = cb + t;
      cb++;
      tol = *(cb++);
      n   = N - *(cb++);
      e   = E - *(cb++);
      if (fabs(n) < 100.0e3 && fabs(e) < 100.0e3) {
        E = *--c;
        N = *--c;
        while (c > cb) {
          w = n*E + e*N + *--c;
          N = n*N - e*E + *--c;
          E = w;
        }
      }
      else res = TRF_AREA_;
    }


/*  s34tkk   ver 1999.01          # page 3   12 Jan 1999 12 54 */


    else { /* collect output coord */
      *yout = N;
      *xout = E;
      cb    = cw;
    }

  /* tol-check of results*/
  if (res == 0) {
    if (fabs(N - y) < tol  && fabs (E - x) < tol) return (0);
    else res = TRF_TOLLE_;
  }
  return((tr_error==NULL || res==0) ? res :
          t_status(tr_error, usertxt, "s34stkk", res,
          "m ", "", N, E, N - y, E - x));

}

