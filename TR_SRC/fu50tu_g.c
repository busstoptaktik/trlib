

/* fu50tu_g   ver 1999.01         # page 1   12 Jan 1999 12 58 */


/* Copyright (c) 1999, LG,  Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of LG, Kort-og  */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include <stdio.h>
#include <math.h>
#include "geo_lab.h"


int                      fu50tu_g(
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
  utm29_ed50 <-> utm29_euref89
  grad:  5 pass: 5
  my =   2.85 cm    my_loss =  2    y_enp =  +8.3
  mx =   2.43 cm    mx_loss =  3    x_enp =  +9.0
  OBS = 475   red = 5    m_lim_gen:  0.143
  */


/* fu50tu_g   ver 1999.01         # page 2   12 Jan 1999 12 58 */

  static double FU50_U29[] = {
    /*deg*/  5.0,
    /*  0*/  +6.8793664259e+06,  +9.9996438118e-01,  +3.0792201207e-11,
    /*  3*/  +1.1643892478e-15,  -3.7603597193e-20,  -3.7362108052e-25,
    /*  6*/  +8.3304016047e-06,  -1.4244403747e-10,  +6.4248979220e-15,
    /*  9*/  +3.9997013407e-20,  -2.2073242784e-24,
    /* 11*/  -1.5330980886e-10,  +4.8562490501e-15,  +1.4804251026e-19,
    /* 14*/  -8.8959192706e-26,
    /* 15*/  +2.5674165506e-15,  -2.2386596609e-20,  -1.3728202987e-23,
    /* 18*/  +9.5680779291e-20,  -3.6144015532e-24,
    /* 20*/  +4.2965770033e-25,
    /*tcy*/     +6879602.8851,

    /*  0*/  +6.1225198856e+05,  +9.9996610999e-01,  -3.8321303586e-10,
    /*  3*/  +8.1215468279e-15,  +5.9245171671e-19,  +7.3022054095e-24,
    /*  6*/  -1.0507605306e-05,  +8.9298280108e-11,  +2.4820724456e-14,
    /*  9*/  -1.3875913971e-18,  -3.9151916221e-23,
    /* 11*/  -1.4804168040e-11,  -2.3016503296e-15,  -1.5181721924e-19,
    /* 14*/  +4.1863317067e-23,
    /* 15*/  +1.0540168924e-15,  -4.6258832994e-20,  -5.2430007120e-24,
    /* 18*/  +3.1701892885e-20,  +1.1169477664e-24,
    /* 20*/  +3.8441984386e-25,
    /*tcx*/      +612360.5191
  };


  static double U29_FU50[] = {
    /*deg*/  5.0,
    /*  0*/  +6.8796028403e+06,  +1.0000356200e+00,  -3.0794863355e-11,
    /*  3*/  -1.1646270044e-15,  +3.7610363645e-20,  +3.7373337444e-25,
    /*  6*/  -8.3310129478e-06,  +1.4246146121e-10,  -6.4255385065e-15,
    /*  9*/  -4.0010853591e-20,  +2.2075853355e-24,
    /* 11*/  +1.5331742748e-10,  -4.8564751883e-15,  -1.4807359056e-19,
    /* 14*/  +8.9419647748e-26,
    /* 15*/  -2.5676669527e-15,  +2.2368828957e-20,  +1.3731744745e-23,
    /* 18*/  -9.5694563858e-20,  +3.6147179369e-24,
    /* 20*/  -4.2970911688e-25,
    /*tcy*/     +6879366.3811,

    /*  0*/  +6.1236049720e+05,  +1.0000338910e+00,  +3.8323997498e-10,
    /*  3*/  -8.1218803153e-15,  -5.9251766062e-19,  -7.3033590912e-24,
    /*  6*/  +1.0508277324e-05,  -8.9288692392e-11,  -2.4823143113e-14,
    /*  9*/  +1.3876965595e-18,  +3.9157367857e-23,
    /* 11*/  +1.4805852693e-11,  +2.3011122143e-15,  +1.5187832486e-19,
    /* 14*/  -4.1867084137e-23,
    /* 15*/  -1.0541043850e-15,  +4.6267362204e-20,  +5.2423565321e-24,
    /* 18*/  -3.1707239880e-20,  -1.1169565837e-24,
    /* 20*/  -3.8451479159e-25,
    /*tcx*/      +612251.9667
  };


/* fu50tu_g   ver 1999.01         # page 4   12 Jan 1999 12 58 */


  /* Collect start values */
  N = N_in;
  E = E_in;
  if (direct > 0) { /* Forward == FU50 -> U29 */
    cb = FU50_U29;
    cw = U29_FU50;
  }
  else if (direct < 0) { /* Reverse == U29 -> FU50 */
    cb = U29_FU50;
    cw = FU50_U29;
  }

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
      if (fabs(N - N_in) < tol && fabs(E - E_in) < tol) return (0);
      else
      res = (fabs(N - N_in) < 10.0 && fabs(E - E_in) < 10.0)
          ? TRF_INACC_ : TRF_TOLLE_;
    }
  }
  else res = TRF_PROGR_;  /* UNDEFINED DIRECTION */

  return(t_status(tr_error, usertxt, "fu50tu_g", res, 
         "m ", "", N, E, N - N_in, E - E_in));

#undef   COORDLIM
}

