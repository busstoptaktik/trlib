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
 


/* utm_t_g     ver 2008.03           # page 1   22 May 2008 10 41 */


/* Copyright (c) 1999 LG,   Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of LG, Kort-og  */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "proj_c.h"

#ifndef    M_PI
#include   "kms_math.h"
#endif

int                    utm_t_g(
/*_______________________*/
struct proj_const     *TC,
int                    direct,
double                 N,
double                 E,
double                *Nout,
double                *Eout
)

{
#include          "clenshaw.h"
#include          "gatg.h"
#include          "v_red.h"

  int                res = 0, i, s, h;
  double             Cn, Ce, dCn = 0.0, dCe = 0.0;
  double             sin_Cn, cos_Cn = 0.0, sin_Ce, cos_Ce;
  double            *GP, *utg, *gtu;

  GP    = TC->tcgg;
  utg   = TC->utg;
  gtu   = TC->gtu;


/* utm_t_g     ver 2008.03           # page 2   22 May 2008 10 41 */


  /* transformation sequence */
  if (direct) {
    i       = (direct > 0 ? 1 : -1);
    s       = -i;
    h       = -i -i;
    if (direct < 0) E = v_red(E);
  
    Cn = N;
    Ce = E;

    /* Transverse mercator */
    for ( ; (h - i); i += s) {
      switch(i) {
        case 1: /* UTM -> GEO */
          /* normalize N, E */
          Cn = (Cn - TC->Zb)/(TC->Qn);
          Ce = (Ce - TC->E0)/(TC->Qn);
          if (fabs(Ce) <= 2.623395162778) {
            if (fabs(Ce) > 1.14691472225) 
               res = (fabs(Ce) > 2.2938245694606)
                   ? TRF_AREA_ : TRF_INACC_;
            /* norm. N, E -> compl. sph. LAT, LNG */
            Cn += clenshaw('S', utg, 5, 2.*Cn, 2.*Ce, &dCn, &dCe);
            Ce += dCe;
            Ce = 2.0*(atan(exp(Ce)) - M_PI_4);
            /* compl. sph. LAT -> Gaussian LAT, LNG */
            cos_Cn = cos(Cn);
            sin_Ce = sin(Ce);
            cos_Ce = cos(Ce);
            Ce     = atan2(sin_Ce, cos_Ce*cos_Cn);
#ifdef  WIN32
            Cn     = atan2(sin(Cn)*cos_Ce,
                     _hypot(sin_Ce, cos_Ce*cos_Cn));
#else
            Cn     = atan2(sin(Cn)*cos_Ce,
                     hypot(sin_Ce, cos_Ce*cos_Cn));
#endif
            /* Gaussian LAT, LNG -> ell. LAT, LNG */
            Cn = gatg(GP,  1, Cn);
            Ce = v_red(Ce + TC->L0);
          }
          else res = TRF_AREA_;
          break;
        case -1: /* GEO -> UTM */
          /* ell. LAT, LNG -> Gaussian LAT, LNG */
          Cn  = gatg(GP, -1, Cn);
          Ce -= TC->L0;
          /* Gaussian LAT, LNG -> compl. sph. LAT */
          sin_Cn = sin(Cn);
          cos_Cn = cos(Cn);
          cos_Ce = cos(Ce);
          Cn     = atan2(sin_Cn, cos_Ce*cos_Cn);
#ifdef  WIN32
          Ce     = atan2(sin(Ce)*cos_Cn,
                   _hypot(sin_Cn, cos_Cn*cos_Ce));
#else
          Ce     = atan2(sin(Ce)*cos_Cn,
                   hypot(sin_Cn, cos_Cn*cos_Ce));
#endif
          /* compl. sph. N, E -> ell. norm. N, E */
          Ce  = log(tan(M_PI_4 + Ce*0.5));
          Cn += clenshaw('S', gtu, 5, 2.*Cn, 2.*Ce, &dCn, &dCe);
          Ce += dCe;
          if (fabs(Ce) <= 2.623395162778) {
            if (fabs(Ce) > 1.14691472225)
               res = (fabs(Ce) > 2.2938245694606)
                   ? TRF_AREA_ : TRF_INACC_;
            /* ell. norm. N, E -> metric N, E */
            Cn  = (TC->Qn) * Cn + TC->Zb;  /* Northing */
            Ce  = (TC->Qn) * Ce + TC->E0;  /* Easting  */
          }
          else res = TRF_AREA_;
          break;


/* utm_t_g     ver 2008.03           # page 4   22 May 2008 10 41 */


        case 0: /* transfer results */
          *Nout = Cn;
          *Eout = Ce;
          if (res <= TRF_AREA_) i += s; /* stop */
          break;
        }
      }
  }
  else return(TRF_ILLEG_);


/* utm_t_g     ver 2008.03           # page 12   22 May 2008 10 41 */


  /* tolerance test */
  if (res >= TRF_INACC_) {
    dCn = Cn - N;
    dCe = Ce - E;
    if (direct < 0) {
      if   (dCe >  M_PI) dCe -= (2.0*M_PI);
      else
      if (dCe < -M_PI) dCe += (2.0*M_PI);
      dCn *=  TC->Qn;
      dCe *= (TC->Qn * cos_Cn);
    }
    if ( (fabs(dCn) < TC->tol)
      && (fabs(dCe) < TC->tol)) return (0);
    else
    if ((fabs(dCn) < 1.0e6 * TC->tol) &&  /* 30 m */
        (fabs(dCe) < 1.0e6 * TC->tol)) res = TRF_INACC_;
    else res = TRF_TOLLE_;
  }
  if (res <= TRF_AREA_) {
    *Nout = 0.0;
    *Eout = 0.0;
  }
 

/* utm_t_g     ver 2008.03           # page 13   22 May 2008 10 41 */


  return(res);
}


