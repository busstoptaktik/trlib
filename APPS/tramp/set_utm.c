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
 


/* set_utm                        # page 1   22 Oct 2002 10 45 */


/* Copyright (c) 1992 GD, U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include    <stdio.h>
#include    <math.h>
#include    "proj_c.h"

#ifndef    M_PI
#include   "kms_math.h"
#endif

void                     set_utm(
/*____________________________*/
struct proj_const        *c_lab
)

{

#include            "clenshaw.h"
#include            "gatg.h"

  double                 n, np;
  double                *gb = c_lab->tcgg, *bg = gb + 5;
  double                *utg = c_lab->utg, *gtu = c_lab->gtu;
  double                 dCn, dCe, Z;

  /* test flattening */
  if (c_lab->f >= 1.0)  c_lab->f = 1.0/c_lab->f;

  /* third flattening */
  np = n = c_lab->f/(2.0 - c_lab->f);

  /* transformation tolerances */
  /* 0.000 001 sx = 30 my on the ellipsoid */
  c_lab->tol = 1.0e-6 * (c_lab->a) * M_PI/180./60./60.;



/* set_utm                        # page 2   22 Oct 2002 10 45 */


  /* COEF. OF TRIG SERIES GEO <-> GAUSS */
  /* gb := Gaussian -> Geodetic, KW p190 - 191 (61) - (62) */
  /* bg := Geodetic -> Gaussian, KW p186 - 187 (51) - (52) */
  /* 5 degree : Engsager and Poder: ICC2007 */
  bg  = gb + 5;
  *gb = n*( 2.0 + n*(-2.0/3.0  + n*(-2.0      + n*(116.0/45.0 + n*(26.0/45.0)))));
  *bg = n*(-2.0 + n*( 2.0/3.0  + n*( 4.0/3.0  + n*(-82.0/45.0 + n*(32.0/45.0)))));
  np     *= n;
  *(gb+1) = np*(7.0/3.0     + n*( -8.0/5.0  + n*(-227.0/45.0 + n*(2704.0/315.0))));
  *(bg+1) = np*(5.0/3.0     + n*(-16.0/15.0 + n*( -13.0/ 9.0 + n*( 904.0/315.0))));
  np     *= n;
  *(gb+2) = np*( 56.0/15.0  + n*(-136.0/35.0 + n*(1262.0/105.0)));
  *(bg+2) = np*(-26.0/15.0  + n*(  34.0/21.0 + n*(   8.0/  5.0)));
  np     *= n;
  *(gb+3) = np*(4279.0/630.0 + n*(-322.0/35.0));
  *(bg+3) = np*(1237.0/630.0 + n*( -12.0/ 5.0));
  np     *= n;
  *(gb+4) = np*(4174.0/315.0);
  *(bg+4) = np*(-734.0/315.0);

  /* Constants of the projections */
  /* Transverse Mercator (UTM, ITM, etc) */
  np = n*n;
  /* Norm. mer. quad, K&W p.50 (96), p.19 (38b), p.5 (2) */
  c_lab->Qn = c_lab->a*c_lab->scale/(1 + n)
            * (1. + np*(1./4.0 + np*(1./64.0 + np/256.0)));
  /* coef of trig series */
  /* utg := ell. N, E -> sph. N, E,  KW p194 (65) */
  /* gtu := sph. N, E -> ell. N, E,  KW p196 (69) */
  *utg = n*(-0.5  + n*( 2.0/3.0 + n*(-37.0/96.0 + n*( 1.0/360.0 + n*(  81.0/512.0)))));
  *gtu = n*( 0.5  + n*(-2.0/3.0 + n*(  5.0/16.0 + n*(41.0/180.0 + n*(-127.0/288.0)))));
  *(utg+1) = np*(-1.0/48.0 + n*(-1.0/15.0 + n*(437.0/1440.0 + n*(-46.0/105.0))));
  *(gtu+1) = np*(13.0/48.0 + n*(-3.0/5.0  + n*(557.0/1440.0 + n*(281.0/630.0))));
  np      *= n;
  *(utg+2) = np*(-17.0/480.0 + n*(  37.0/840.0 + n*(  209.0/ 4480.0)));
  *(gtu+2) = np*( 61.0/240.0 + n*(-103.0/140.0 + n*(15061.0/26880.0)));
  np      *= n;
  *(utg+3) = np*(-4397.0/161280.0 + n*(  11.0/504.0));
  *(gtu+3) = np*(49561.0/161280.0 + n*(-179.0/168.0));
  np     *= n;
  *(utg+4) = np*(-4583.0/161280.0);
  *(gtu+4) = np*(34729.0/ 80640.0);
  /* Gaussian latitude value of the origin latitude */
  Z        = gatg(c_lab->tcgg, -1, c_lab->B0);
  /* Origin northing minus true northing at the origin latitude */
  /* i.e. true northing = N - c_lab->Zb                         */
  c_lab->Zb = c_lab->N0 - c_lab->Qn*(Z
            + clenshaw('s', gtu, 5, 2.0*Z, 0.0, &dCn, &dCe));

  return;
}


