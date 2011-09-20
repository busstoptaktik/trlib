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
 


/* bh_lat.c                           # page 1   22 Feb 1993 12 54 */
  

/* Copyright (c) 1991 GS, U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include <math.h>


double                  bh_lat(
/*___________________________*/
double                  b,
double                  az,
double                  em2,
double                  *K1,
double                  *s1
)

/*
bh_lat determines the max positive reduced latitude of a geo-
       desic starting from reduced latitude red_lat in azimuth az.

b      The reduced latitude of the geodesic where the azimuth is az,
       in units of radians.

az     The azimuth of the geodesic in reduced latitude red_lat. The
       azimuth is counted from north, but the orientation of the geo-
       desic is immaterial. The unit is radians.

em2    The second eccentricity of the ellipsoid = f*(2-f)/(1-f)/(1-f).

K1     The generating parameter for the coefficients of the series of
       the longitude difference and the lengths of the geodesic.

s1     The length of the great circle on the auxiliary sphere from
       the starting point to the to the max reduced latitude point,
       counted in units of radians.

Prog.: Knud Poder SEP 1991.

*/


/* bh_lat.c                           # page 2   22 Feb 1993 12 54 */


{
  double             bm, k;
  double             cos_b, sin_b, cb_sa, cb_ca;
  double             t, rs;

  cos_b = cos(b);
  sin_b = sin(b);
  cb_sa = cos_b*sin(az);
  cb_ca = cos_b*cos(az);
#ifdef  WIN32
  bm    = atan2(_hypot(cb_ca, sin_b), cb_sa);
#else
  bm    = atan2(hypot(cb_ca, sin_b), cb_sa);
#endif
  /* k = em2*sin(bm)*sin(bm)/(1+em2*sin(bm)*sin(bm)) */
  k = sin(bm);
  k = em2*k*k;
  k = k/(1.0 + k);
  /* K1 = 1 - sqrt(1-k))/(1+sqrt(1-k))i, JEK p. 989 (6a) */
  /* series for sqrt(1-k)-1, up to 5th order             */
  for (t = 6.0, rs = 0.0; t > 0.5; t -= 1.0)
  rs = k*(t - 1.5)/t*(1.0 + rs);

  *K1 = -rs/(2.0 + rs);
  *s1 = -atan2(cb_ca, sin_b);

   return(bm);
}

