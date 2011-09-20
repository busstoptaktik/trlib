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
 


/* bshlm2g                        # page 1   18 Feb 1994 13 55 */


/* Copyright (c) 1993, GD, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include  <math.h>
#include  <stdio.h>
#include  "geo_lab.h"

#ifndef    M_PI
#include   "kms_math.h"
#endif

int          bshlm2g(
/*_________________*/
double       a,
double       f,
double       B1,
double       B2,
double       L1,
double       L2,
double      *A1,
double      *A2,
double      *S
)

/*The function bshlm2g solves the reverse (or second) geodetic
main problem using the GAUSS mid-latitude formulae for distances
less than appr. 65 km and at latitudes between 85 N and 85 S, 
and the BESSEL-HELMERT formulae otherwise.

Reference: JORDAN-EGGERT-KNEISSL: Handbuch der Vermessungs-
kunde Vol. IV, MCMLIX, p. 968-969. Eq. (xx), (xx), and (xx).

bshlm2g  The result of the call:
         0 => ok
         1 => distance too short, but a result anyway
         2 => no convergence, disaster, probably antipode problem

a        The semi major axis of the ellipsoid in units of metres.

f        The flattening of the ellipsoid. The flattening or its 
         inverse may be used freely.

B1       latitude of station 1 in radians.

B2       latitude of station 2 in radians.

L1       longitude of station 1 in radians.

L2       longitude of station 2 in radians.

A1       the azimuth at station 1 towards station 2 in radians.

A2       the azimuth at station 2 towards station 1 in radians.

S        the length of the geodesic between the stations in units 
         of metres.

Prog: Knud Poder APR 1993.

*/


/* bshlm2g                        # page 2   18 Feb 1994 13 55 */


{
#include                 "bh_lat.h"
#include                 "bh_lmb.h"
#include                 "clenshaw.h"
#include                 "gmb2.h"
#include                 "sftr.h"
#include                 "v_red.h"

  double     em2, V1, V2, b0, PF;
  double     dL, d_lmb, d_lmb1;
  double     b1, b2, dl, a1, a2, b_max, s, s1;
  double     K1, K1sq, R, I;
  double     ste[4];
  int        i = 20;

  double     max_lat = 86.0/180.0*M_PI; /* latitude > 85   */
  double     max_dif =  0.6/180.0*M_PI; /* 0.6 deg = 65 km */
  int        res;

  /* Using gmb2 */
  if (   fabs(B1)                     < max_lat 
      && fabs(B2)                     < max_lat
      && fabs(B2-B1)                  < max_dif

      && fabs(L2-L1)*cos((B1+B2)/2.0) < max_dif) {
/*
(void) fprintf(stderr, "\ngmb2 start");
*/
    res = gmb2(a, f, B1, B2, L1, L2, A1, A2, S);
/*
(void) fprintf(stderr, "\ngmb2 stop res = %4d", res);
*/
    if (!res) return(res);

  }



/* bshlm2g                        # page 3   18 Feb 1994 13 55 */

/*
(void) fprintf(stderr, "\nbshlm2 start");
*/
  if (f > 1.0) f = 1.0/f;
  b0    = a*(1.0 - f);
  em2   = f*(2.0 - f)/(1.0 - f)/(1.0 - f);

  b1    = RD_LAT(f, B1);
  b2    = RD_LAT(f, B2);
  dL    = (L2 - L1);
  V1    = cos(B1);
  V1    = sqrt(1.0 + em2*V1*V1);
  V2    = cos(B2);
  V2    = sqrt(1.0 + em2*V2*V2);
  dl    = dL/(V1 + V2)*2.0;
  d_lmb = 0.0;

  do {
    d_lmb1 = d_lmb;
    s      = sftr(dl, M_PI_2-b2, M_PI_2-b1, &a1, &a2);
    b_max  = bh_lat(b1, a1, em2, &K1, &s1);
    d_lmb  = bh_lmb(K1, f, b_max, s1, s);
    dl     = dL + d_lmb;
  } while ((i--)
      && fabs(d_lmb - d_lmb1) >= fabs(d_lmb)*1.0e-10);

  

/* bshlm2g                        # page 4   18 Feb 1994 13 55 */


  if (i) {
    K1sq     = K1*K1;
    PF       = b0*(1.0 + K1sq/4.0)/(1.0 - K1);
    *(ste+0) =  (1.0 - 3.0*K1sq/8.0)/2.0;
    *(ste+1) = -K1/16.0;
    *(ste+2) =  K1sq/48.0;
    *S       = PF*(s + K1*(clenshaw('s', ste, 3, 2*(s1+s), 0.0, &R, &I)
             - clenshaw('s', ste, 3, 2*s1, 0.0, &R, &I)));
    *A1      = a1;
    *A2      = v_red(-a2);

#ifdef  BHTEST
    { 
      double oms = 180.0/M_PI;
      (void) fprintf(stdout, "\nA1 = %-8.3f", *A1*oms);
      (void) fprintf(stdout, "\ns1 = %-8.3f", s1*oms);
      (void) fprintf(stdout, "\nb_max = %-8.3f", b_max*oms);
      (void) fprintf(stdout, "\nd_lmb = %-8.3f", d_lmb*oms);
      (void) fprintf(stdout, "\nA2 = %-8.3f", *A2*oms);
    }
#endif

  }
  else {
    (void) fprintf(stderr,"\n***BESSEL-HELMERT no convergency");
    *A1 = *A2 = *S = a1 = 0.0;
    return(2);
  }
  res = (*S > a*1e-6) ? 0 : 1;
/*
(void) fprintf(stderr, "\nbshlm2 stop res = %4d", res);
*/
  return(res);

}

