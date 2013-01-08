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
 


/* bshlm2.c                         # page 1   18 Feb 1994 13 50 */


/* Copyright (c) 1991 U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include  <math.h>
#include  <stdio.h>
#include  "geo_lab.h"
#include "lord.h"

#ifndef    M_PI
#include   "kms_math.h"
#endif

int                     bshlm2(
/*___________________________*/
double                  a,
double                  f,
double                  B1,
double                  B2,
double                  L1,
double                  L2,
double                *A1,
double                *A2,
double                *S,
int                    mode
)

/*The function bshlm2 solves the reverse (or second) geodetic
main problem using the Besssel-Helmert formulae.
Reference: JORDAN-EGGERT-KNEISSL: Handbuch der Vermessungs-
kunde Vol. IV, MCMLIX, p. 968-969. Eq. (xx), (xx), and (xx).

bshlm2  The result of the call:
        0 => ok
        1 => distance too short, but a result anyway
        2 => no convergence, disaster, probably antipode problem

a       The semi major axis of the ellipsoid in units of metres.

f       The flattening of the ellipsoid. The flattening or its
        inverse may be used freely.

B1      latitude of station 1 in radians.

B2      latitude of station 2 in radians.

L1      longitude of station 1 in radians.

L2      longitude of station 2 in radians.

A1      the azimuth at station 1 towards station 2 in radians.

A2      the azimuth at station 2 towards station 1 in radians.

S       the length of the geodesic between the stations in units 
        of metres.

Prog: Knud Poder NOV 1990
*/


/* bshlm2.c   Turbo C version       # page 2   18 Feb 1994 13 50 */


{
#include        "bh_lat.h"
#include        "bh_lmb.h"
#include        "clenshaw.h"
#include        "sftr.h"
#include        "v_red.h"

  double     em2, V1, V2, b0, PF;
  double     dL, d_lmb, d_lmb1;
  double     b1, b2, dl, a1, a2, b_max, s, s1, mp;
  double     K1, K1sq, R, I;
  double     ste[4];
  int        i = 20, md;

  md = (mode + 1)/2;
  mp = 2.0*md*M_PI;

  if (f > 1.0) f = 1.0/f;
  b0  = a*(1.0 - f);
  em2 = f*(2.0 - f)/(1.0 - f)/(1.0 - f);

  b1 = RD_LAT(f, B1);
  b2 = RD_LAT(f, B2);
  dL = (L2 - L1);
  V1 = cos(B1);
  V1 = sqrt(1.0 + em2*V1*V1);
  V2 = cos(B2);
  V2 = sqrt(1.0 + em2*V2*V2);
  dl = dL/(V1 + V2)*2.0;
  d_lmb = 0.0;

  do {
    d_lmb1 = d_lmb;
    s      = sftr(dl, M_PI_2-b2, M_PI_2-b1, &a1, &a2);
    if (mode%2) {
      s  = -s;
      a1 = v_red(a1 + M_PI);
      a2 = v_red(a2 + M_PI);
    }
    s     = s + mp;
    b_max = bh_lat(b1, a1, em2, &K1, &s1);
    d_lmb = bh_lmb(K1, f, b_max, s1, s);
    dl    = dL + d_lmb;
  } while ((i--)
      && fabs(d_lmb - d_lmb1) >= fabs(d_lmb)*1.0e-10);



/* bshlm2.c   Turbo C version       # page 3   18 Feb 1994 13 50 */


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
      lord_debug(0,LORD("\nA1 = %-8.3f"), *A1*oms);
      lord_debug(0,LORD("\ns1 = %-8.3f"), s1*oms);
      lord_debug(0,LORD("\nb_max = %-8.3f"), b_max*oms);
      lord_debug(0,LORD("\nd_lmb = %-8.3f"), d_lmb*oms);
      lord_debug(0,LORD("\nA2 = %-8.3f"), *A2*oms);
    }
#endif

  }
  else {
    lord_error(0,LORD("BESSEL-HELMERT no convergency"));
    *A1 = *A2 = *S = a1 = 0.0;
    return(2);
  }
  return(*S > a*1e-6 ? 0 : 1);
}

