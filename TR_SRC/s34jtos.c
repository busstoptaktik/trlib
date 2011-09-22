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
 


/* s34jtos  ver 1999.01           # page 1   10 Sep 1998 12 54 */


/* Copyright (c) 1999 LG, Kort-og Matrikelstyrelsen, Denmark   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of LG, Kort-og  */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include <stdio.h>
#include <math.h>
#include "geo_lab.h"


int                         s34jtos(
/*________________________________*/
double                      y,
double                      x,
double                      *yout,
double                      *xout,
int                         direct,
char                        *usertxt,
FILE                        *tr_error
)

{
  /*
s34jtos: int function giving the transformations

          s34 Jylland <-> Ostenfeldts system

The transformation may have errors up to 0.5 m, but the local
consistency is generally much better. The computing precision is
better than 0.5 mm.
s34jtos returns the result of the transformation as follows

      0     =>   all ok
      !=0   =>   error (tolerance exceed, or parameter error)
                 is reported on *tr_error (if not *NUL)

y, x input coordinates (s34j or os), double,
in units of metres. y and x are called by value.

yout, xout output coordinates in (os or s34j), double
in units of metres. yout and xout are called by reference.

direct, call by value indicating

          direct > 0  => transformation s34j --> os
          direct < 0  => transformation os --> s34j
          direct = 0  => no transformation, exit with error.

example:
s34jtos(y, x, &Y, &X, 1, 1);
produces os coordinates Y,X from the s34j coordinates y,x.
Note the operator & on Y and X.
*/


/* s34jtos  ver 1999.01           # page 2   10 Sep 1998 12 54 */

#include             "t_status.h"

  double               N = 0.0, E = 0.0, n, e, w, tol = 0.0005;
  double               *c, *cb = NULL, *cw = NULL;
  int                  i, d, t;
  int                  res = 0;

  static   double    C34os[] =
  {
    /*lng*/    18.0,                /*tol*/  0.0005,
    /*N,E*/     73000.000000,       280000.000000,
    /* 0 */    6.853626449e+04,    2.955448253e+03,
    /* 1 */    9.998670630e-01,   -1.655899148e-02,
    /* 2 */    1.602055277e-11,   -2.841831044e-10,
    /* 3 */   -2.578438745e-15,   -2.376733581e-16,
    /* 4 */    6.111814588e-20,   -1.802899003e-20,
    /* 5 */   -1.539564291e-25,    1.417145176e-24,
    /* 6 */    2.410664720e-29,   -7.990298697e-30
  };

  static   double    Cos34[] =
  {
    /*lng*/    18.0,                /*tol*/  0.0005,
    /*N,E*/    68000.000000,        2000.000000,
    /* 0 */    7.247963213e+04,    2.809641934e+05,
    /* 1 */    9.998592871e-01,   -1.655861449e-02,
    /* 2 */   -3.289800932e-11,   -2.745975219e-10,
    /* 3 */    2.751904974e-15,   -6.304891315e-16,
    /* 4 */   -6.969771723e-20,   -1.481705770e-20,
    /* 5 */    4.032281321e-25,    1.268841834e-24,
    /* 6 */   -2.486793478e-29,   -5.147362835e-30
  };

  d = ((direct > 0) ? (1) : (direct < 0 ? (-1) : 0));

  if (d > 0) { /* s34j -> os */
    N = y;
    E = x;
    cb = C34os;
    cw = Cos34;
  }
  else if (d < 0) {/* os -> s34j */
    N = x;
    E = y;
    cb = Cos34;
    cw = C34os;
  }
  else res = TRF_ILLEG_;


/* s34jtos  ver 1999.01           # page 3   10 Sep 1998 12 54 */


  for (i = -1; i <= 1 && res == 0; ++i)
    if (i) { /* transformation, i == -1 => f, i == 1 => b */
      t   = (int) fabs(*cb);
      c   = cb + t;
      cb++;
      tol = *(cb++);
      n   = N - *(cb++);
      e   = *(cb++) - E;
/*    if (fabs(n) < 32000.0 && fabs(e) < 60000.0) org. */
      if (-32000.0 < n && n < 47530.0 && fabs(e) < 60000.0) {
        E = *--c;
        N = *--c;
        while (c > cb) {
          w = n*E + e*N + *--c;
          N = n*N - e*E + *--c;
          E = w;
        }
      } else res = TRF_AREA_;
    } else { /* collect output coord */
      if (d > 0) {
        *yout = E;
        *xout = N;
      } else {
        *yout = N;
        *xout = E;
      }
      cb    = cw;
    }

  /* tol-check of results*/
  if (d < 0) { /*swop N and E */
    w = N;
    N = E;
    E = w;
  }
  if (fabs(N - y) < tol  && fabs (E - x) < tol
      && res == 0) return (0);
  else if (res == 0) res = TRF_TOLLE_;

  return(t_status(tr_error, usertxt, "s34jtos", res,
         "m ", "", N, E, N - y, E - x));

}

