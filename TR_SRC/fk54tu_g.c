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
 

/* fk54tu_g   ver 1999.01         # page 1   12 Jan 1999 12 58 */


/* Copyright (c) 1999, LG,  Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of LG, Kort-og  */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include <stdio.h>
#include <math.h>
#include "geo_lab.h"


int                      fk54tu_g(
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



/*
Y&X-generelt pol. resultater
fk54 <-> utm29_euref89
grad:  5 pass: 5
my =   3.12 cm    my_loss =  2    y_enp =  +8.2
mx =   3.44 cm    mx_loss =  3    x_enp =  +8.8
OBS = 474  red = 4    m_lim_gen:  0.172
*/


/* fk54tu_g   ver 1999.01         # page 2   12 Jan 1999 12 58 */


static   double FK54_U29[] = {
/*deg*/  5.0,
/*  0*/  +6.8793036608e+06,  +9.9971502685e-01,  -1.4674714832e-11,
/*  3*/  -1.5233541045e-15,  -2.7206193428e-20,  -4.9344353006e-25,
/*  6*/  -1.9938622869e-05,  -2.5991967212e-09,  -8.5874345533e-15,
/*  9*/  -2.9850318777e-20,  +3.5303629937e-24,
/* 11*/  -2.5647253644e-10,  +2.3627228049e-14,  +2.4004548968e-19,
/* 14*/  -4.4028584773e-24,
/* 15*/  -1.3290519185e-15,  +4.8817625195e-20,  +1.3245816860e-23,
/* 18*/  +2.3928472507e-19,  -5.8253528462e-24,
/* 20*/  -3.2386924768e-24,
/*tcy*/      +505045.2368,

/*  0*/  +6.1227852329e+05,  -9.9971815540e-01,  +1.1426839498e-09,
/*  3*/  -1.2755621557e-14,  +1.1581863386e-19,  +5.8181833917e-25,
/*  6*/  -2.2272827599e-05,  +1.5458470767e-10,  +2.7482921046e-14,
/*  9*/  +8.4913625229e-19,  -1.9995693258e-23,
/* 11*/  -1.3558340423e-09,  +1.5554409638e-14,  -3.6819499541e-19,
/* 14*/  -3.4991993903e-23,
/* 15*/  +4.4436423311e-16,  -9.4084877752e-20,  -1.3590633715e-23,
/* 18*/  +5.5317310306e-21,  -3.1448342226e-24,
/* 20*/  +2.7344195472e-25,
/*tcx*/      +387382.1738
};


static   double U29_FK54[] = {
/*deg*/  5.0,
/*  0*/  +5.0504516875e+05,  +1.0002850563e+00,  +1.4601105157e-11,
/*  3*/  +1.5221882264e-15,  +2.7242713605e-20,  +4.9396615045e-25,
/*  6*/  -1.9950370308e-05,  -2.6013952610e-09,  -8.5958823357e-15,
/*  9*/  -2.9995693195e-20,  +3.5348746084e-24,
/* 11*/  +2.5677824475e-10,  -2.3642412267e-14,  -2.4039627678e-19,
/* 14*/  +4.4097258770e-24,
/* 15*/  -1.3291994907e-15,  +4.9095544419e-20,  +1.3265482381e-23,
/* 18*/  -2.3963174053e-19,  +5.8325302546e-24,
/* 20*/  -3.2453310983e-24,
/*tcy*/     +6879303.5928,

/*  0*/  +3.8738315582e+05,  -1.0002819253e+00,  +1.1436368668e-09,
/*  3*/  +1.2764186506e-14,  +1.1588820443e-19,  -5.8333437954e-25,
/*  6*/  -2.2285525979e-05,  -1.5458093838e-10,  +2.7519183608e-14,
/*  9*/  -8.5010842588e-19,  -2.0029750099e-23,
/* 11*/  -1.3569656605e-09,  -1.5564634534e-14,  -3.6848361280e-19,
/* 14*/  +3.5052051414e-23,
/* 15*/  +4.4512366181e-16,  +9.4274158187e-20,  -1.3622345585e-23,
/* 18*/  +5.5058162299e-21,  +3.1518974279e-24,
/* 20*/  +2.7355532504e-25,
/*tcx*/      +612277.5415
};


/* fk54tu_g   ver 1999.01         # page 3   12 Jan 1999 12 58 */


  /* Collect start values */
  N = N_in;
  E = E_in;

  if (direct > 0) { /* Forward == FK54 -> U29 */
    cb = FK54_U29;
    cw = U29_FK54;
  }
  else if (direct < 0) { /* Reverse == U29 -> FK54 */
    cb = U29_FK54;
    cw = FK54_U29;
  }

  if (direct) {

    /* Transformation loop */
    for (i = -1; i <= 1 && res == 0; ++i)
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
        cb     = cw;
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

  return(t_status(tr_error, usertxt, "fk54tu_g", res,
         "m ", "", N, E, N - N_in, E - E_in));

#undef   COORDLIM
}

