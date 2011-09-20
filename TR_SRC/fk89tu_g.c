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
 


/* fk89tu_g   ver 1999.01         # page 1   12 Jan 1999 12 58 */


/* Copyright (c) 1999, LG,  Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of LG, Kort-og  */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include <stdio.h>
#include <math.h>
#include "geo_lab.h"


int                      fk89tu_g(
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
#define COORDLIM     (2.0e5)        /* +/- 200 km */

#include         "t_status.h"

  double           N, E, n, e, u, v, tol = 0.001;
  double          *cb = NULL, *cw = NULL;
  double          *tcy, *tcx;
  int              res = 0, i, g, r, c, sz;


/* fk89tu_g   ver 1999.01         # page 2   12 Jan 1999 12 58 */


/*
Y&X-generelt pol. resultater
fk89 <-> utm29_euref89
grad:  4
me  =   1.172 cm    PKT = 620   excl = 72
*/

static   double FK89_U29[] = {
/*deg*/  4.0,

/* Coefficients for y-polynomium, forward transformation: */
/*Poly NORTH :: e-degree =  0  : n-degree = 4 */
/*  0*/  +6.8844171135e+06,  +9.9974817862e-01,  -8.3463313299e-11,
/*  3*/  -3.8795602894e-15,  -2.7886546056e-21,
/*Poly NORTH :: e-degree =  1  : n-degree = 3 */
/*  5*/  -2.5576317233e-05,  -2.8171987234e-09,  -2.8847251372e-16,
/*  8*/  +3.5746999342e-20,
/*Poly NORTH :: e-degree =  2  : n-degree = 2 */
/*  9*/  +7.5396740491e-11,  +1.4485819436e-14,  -9.2864066964e-20,
/*Poly NORTH :: e-degree =  3  : n-degree = 1 */
/* 12*/  +5.5648474476e-16,  -5.4779920577e-20,
/*Poly NORTH :: e-degree =  4  : n-degree = 0 */
/* 14*/  -3.2038608745e-21,
/*tcy*/        710159.946,

/* Coefficients for x-polynomium, forward transformation: */
/*Poly  EAST :: n-degree =  0  : e-degree = 4 */
/*  0*/  +6.1312437114e+05,  -9.9974839005e-01,  +1.3329789310e-09,
/*  3*/  -1.5526731695e-16,  -5.3371661787e-20,
/*Poly  EAST :: n-degree =  1  : e-degree = 3 */
/*  5*/  -2.5617537411e-05,  +1.6757329113e-10,  +3.6719630143e-15,
/*  8*/  -1.3884330999e-19,
/*Poly  EAST :: n-degree =  2  : e-degree = 2 */
/*  9*/  -1.3963961976e-09,  +1.2884555652e-14,  +3.2310086814e-20,
/*Poly  EAST :: n-degree =  3  : e-degree = 1 */
/* 12*/  -5.8210136078e-16,  +1.9479363661e-20,
/*Poly  EAST :: n-degree =  4  : e-degree = 0 */
/* 14*/  -3.7850688548e-21,
/*tcy*/        586535.945
};


/* fk89tu_g   ver 1999.01         # page 3   12 Jan 1999 12 58 */


static   double U29_FK89[] = {
/*deg*/  4.0,

/* Coefficients for y-polynomium, backward transformation: */
/*Poly NORTH :: e-degree =  0  : n-degree = 4 */
/*  0*/  +7.1016003191e+05,  +1.0002518863e+00,  +8.3428188272e-11,
/*  3*/  +3.8795863485e-15,  +2.7863046171e-21,
/*Poly NORTH :: e-degree =  1  : n-degree = 3 */
/*  5*/  -2.5589524058e-05,  -2.8192972514e-09,  -2.9160855917e-16,
/*  8*/  +3.5661732927e-20,
/*Poly NORTH :: e-degree =  2  : n-degree = 2 */
/*  9*/  -7.5347121673e-11,  -1.4489497642e-14,  +9.2997911570e-20,
/*Poly NORTH :: e-degree =  3  : n-degree = 1 */
/* 12*/  +5.5871182262e-16,  -5.4795431835e-20,
/*Poly NORTH :: e-degree =  4  : n-degree = 0 */
/* 14*/  +3.2240313861e-21,
/*tcy*/       6884417.200,

/* Coefficients for x-polynomium, backward transformation: */
/*Poly  EAST :: n-degree =  0  : e-degree = 4 */
/*  0*/  +5.8653673812e+05,  -1.0002516747e+00,  +1.3339842263e-09,
/*  3*/  +1.5103696476e-16,  -5.3479060278e-20,
/*Poly  EAST :: n-degree =  1  : e-degree = 3 */
/*  5*/  -2.5630594864e-05,  -1.6748300401e-10,  +3.6794636952e-15,
/*  8*/  +1.3904344413e-19,
/*Poly  EAST :: n-degree =  2  : e-degree = 2 */
/*  9*/  -1.3974472611e-09,  -1.2886412977e-14,  +3.2482866374e-20,
/*Poly  EAST :: n-degree =  3  : e-degree = 1 */
/* 12*/  -5.8357904809e-16,  -1.9499678514e-20,
/*Poly  EAST :: n-degree =  4  : e-degree = 0 */
/* 14*/  -3.8173622737e-21,
/*tcx*/        613123.578
};

  /* Collect start values */
  N = N_in;
  E = E_in;

  if (direct > 0) { /* Forward == FK89 -> U29 */
    cb = FK89_U29;
    cw = U29_FK89;
  }
  else if (direct < 0) { /* Reverse == U29 -> FK89 */
    cb = U29_FK89;
    cw = FK89_U29;
  }


/* fk89tu_g   ver 1999.01         # page 4   12 Jan 1999 12 58 */


  if (direct) {

    /* Transformation loop */
    for (i = -1; i <= 1 && res >= TRF_TOLLE_; ++i)
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

        if (fabs(n) < COORDLIM && fabs(e) < COORDLIM) {

          for (  r = g, N = *--tcy, E = *--tcx; r >  0; r--) {
            for (c = g, u = *--tcy, v = *--tcx; c >= r; c--) {
              u = n*u + *--tcy;
              v = e*v + *--tcx;
            }
            N = e*N + u;
            E = n*E + v;
          }
        }
        else res = TRF_AREA_;
      }
      else { /* collect output coord,switch to reverse checking */
        *N_out = N;
        *E_out = E;
        cb    = cw;
      }

    if (res >= TRF_TOLLE_) {
      /* tol-check of results*/
      if (fabs(N - N_in) < tol && fabs(E - E_in) < tol) return (0);
      else
      res = (fabs(N - N_in) < 10.0 && fabs(E - E_in) < 10.0)
          ? TRF_INACC_ : TRF_TOLLE_;
    }
  }
  else res = TRF_PROGR_;  /* UNDEFINED DIRECTION */

  return(t_status(tr_error, usertxt, "fk89tu_g", res,
                  "m ", "", N, E, N - N_in, E - E_in));

#undef   COORDLIM
}

