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
 


/* erad.c                             # page 1   29 sep 2009 13 05 */


/* Copyright (c) 2009, Danish National Space Center, DTU       */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of DNSC, DTU    */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */

#include <math.h>
#include <stdio.h>
#include "trthread.h"


/* erad.c                             # page 2   29 sep 2009 13 05 */

double                  erad(
/*_________________________*/
double               a,
double               f,
double               lat,
int                  ft
)

/* erad computes, using the semi-major axis and the flattening
(or automatically the inverse flattening):

        (a) 4 radii of curvatures of the ellipsoid,
            as a function of the latitude,
           (a1) M: the radius of crv. in the meridian
           (a2) N: the radius of crv. in the prime vertical   
           (a3) R: the mean radius of crv.
           (a4) r: the radius of a latitude parallel

        (b) G: The Gaussian curvature as a function of 
               the latitude

        (c) K: Half the Gaussian curv. as a function of
               the northing

        (d) Q: The length of the meridian arc from eq. to
               the latitude

        (e) L: The latitude corresponding to the length 
               of the meridian arc. 

a            call               double
The semi-major axis of the ellipsoid in units of metres.
NB: use a*(central scale) for curvatures related to pro-
jections.

f            call               double
The flattening of the ellipsoid (the inverse flattening
may be used instead with equal effect).

lat          call               double
The latitude in units of radians or the meridian arc in units
of metres according to the desired conversion.

ft           call               int 
The boolean controlling the computation:                 

'M' => the meridian curvature radius
'N' => the prime vertical curvature radius
'R' => the mean curvature radius = sqrt(M*N)
'r' => the radius of a latitude parallel = N cos(lat)
'G' => the Gaussian curvature as a fncti. of the latitude
'K' => half the Gauss. curv. as a fnct. of the Northing 
'Q' => the meridian arc              
'L' => the latitude                 

Any other value of ft gives an alarm exit.

Prog. KP OCT 1989/JAN 1992.

*/


/* erad.c                             # page 3   29 sep 2009 13 05 */


{
  double       n, n2, h = 0.0, h1, h2 = 0.0, *p;
  double       e2p, cos_b, cos2_b, V2, c;

  double       gtm[5], nQ = 0.0, FQ = 0.0;
  double       mtg[5], nL = 0.0, FL = 0.0;

  if (f >= 1.0) f = 1.0/f;
  n  = f/(2.0 - f);

  switch (ft) {

  case 'Q': /* find meridian arc */
    if (n != nQ) {
      nQ       = n;
      n2       = n*n;
      *(gtm+0) = n*      (-3.0/2   + n2*(9.0/16 + n2*(-3.0/32)));
      *(gtm+1) = n2*     (15.0/16  + n2*(-15.0/32));
      *(gtm+2) = n*n2*   (-35.0/48 + n2*(105.0/256));
      *(gtm+3) = n2*n2*  (315.0/512);
      *(gtm+4) = n*n2*n2*(-693.0/1280);
      FQ       = (1. +n2*(1./4 +n2*(1./64 +n2/256)))/(1.0 +n);
    }

    cos2_b = 2.*cos(2.0*lat);
    for (p = gtm + 5, h1 = *--p; p - gtm; h2 = h1, h1 = h)
      h = -h2 + cos2_b*h1 + *--p;
    lat += h*sin(2.0*lat);
    lat *= a*FQ;
    return(lat); /*i.e. the meridian arc */

  case 'L': /* find latitude */
  case 'K': /* find lat. for half Gauss. curv.*/

    if (n != nL) {
      nL       = n;
      n2       = n*n;
      *(mtg+0) = n*      ( 3.0/2   + n2*(-27.0/32 + n2*( 269.0/512)));
      *(mtg+1) = n2*     ( 21.0/16 + n2*(-55.0/32));
      *(mtg+2) = n*n2*   (151.0/96 + n2*(-417.0/128));
      *(mtg+3) = n2*n2*  (1097.0/512);
      *(mtg+4) = n*n2*n2*(8011.0/2560);
      FL       = (1. +n) / (1. +n2*(1./4 +n2*(1./64 +n2/256)));
    }

    lat    = lat/a*FL;
    cos2_b = 2.*cos(2.0*lat);
    for (p = mtg + 5, h1 = *--p; p - mtg; h2 = h1, h1 = h)
      h = -h2 + cos2_b*h1 + *--p;
    lat += h*sin(2.0*lat);

    if (ft == 'L') return(lat);
    break;

  default:
    break;
  }

  cos_b  = cos(lat);
  cos2_b = cos_b*cos_b;
  e2p    = f*(2. - f)/(1. - f)/(1. - f);
  V2     = 1. + e2p*cos2_b;
  c      = a/(1. - f);

  switch (ft)
  {
  case 'M': /* meridian c. rad. */
    c = c/V2/sqrt(V2);
    break;

  case 'R': /* mean c. rad */
    c = c/V2;
    break;

  case 'N': /* prime v. c. radius */
    c = c/sqrt(V2);
    break;

  case 'r': /* radius of lat. parallel */
    c = c*cos_b/sqrt(V2);
    break;

  case 'G': /* Gaussian curvature  */
  case 'K': /* Half Gaussian curv. */
    c = pow(c/V2, -2.0);
    if (ft == 'K')  c *= 0.5;
    break;

  default:
    (void) fprintf(stderr,
        "\n*** undef. call of erad with type %c", ft);
    return(0.0);
  }
  return (c);
}


