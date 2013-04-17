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

#include  <math.h>
#include  <stdio.h>

#ifndef    M_PI
#include   "kms_math.h"
#endif

int                        gmb1(
/*____________________________*/
double                     a,
double                     f,
double                     lat1,
double                    *lat2,
double                     lng1,
double                    *lng2,
double                     az1,
double                    *az2,
double                     s
)

/*
The function gmb1 solves the direct (or first) geodetic
main problem using the gaussian mid-latitude formulae.
Reference: JORDAN-EGGERT-KNEISSL: Handbuch der Vermessungs-
kunde Vol. IV, MCMLIX, p. 968-969. Eq. (xx), (xx), and (xx).

gmb1   Result of computing:
       gmb1 == 0      => ok
       gmb1 == 1      => line > 0.01*a (64 km) or abs(lat) > 86 dg.

a      The semi major axis of the ellipsoid in units of metres.

f      The flattening of the ellipsoid. The flattening or its inverse
       may be used freely.

lat1   latitude of station 1 in radians.

lat2   latitude of station 2 in radians. (res)

lng1   longitude of station 1 in radians.

lng2   longitude of station 2 in radians. (res)

az1    the azimuth at station 1 towards station 2 in radians.

az2    the azimuth at station 2 towards station 1 in radians. (res)

s      the length of the geodesic between the stations in units 
       of metres.

Prog: Knud Poder NOV 1990 after older algol version by
Frede Madsen 1981 , where 5th order terms were included.
5th order terms removed, as bshlm1 is used for longer distances

*/

{
  double     e2, n2, V, V2, c, s_N, cosb, sinb, t1, t2;
  double     lat, dB, dL, dLh, B2, L2, dA;
  double     dBt, dLt, dAt;

/* check and compute ellipsoidal parameters */
 if (f > 1.0) f = 1.0/f;
  c   = a/(1.0 -f);
  e2  = f*(2.0 - f)/(1.0 - f)/(1.0 - f);

  /* Iteration for the mid-latitude and mid-l functions */
  dB = dL = dA = 0;
  do {
    lat  = lat1 + dB/2.0;
    sinb = sin(lat);
    cosb = cos(lat);
    t1   = sinb/cosb;
    t2   = t1*t1;
    n2   = e2*cosb*cosb;
    V2   = 1. + n2;
    V    = sqrt(V2);
    s_N  = s*V/c;

    dBt = dB;
    dLt = dL;
    dAt = dA;
    dB  = dB/V2;
    B2  = dB*dB/24.0;
    L2  = dL*dL/24.0;

    dL  = s_N*sin(az1 + dA)
        *(1.0         + t2*L2        - (V2 - 9.*n2*t2)*B2);
    dLh = 0.5*dL/cosb;
    dB  = s_N*cos(az1 + dA)
        *(1.0 - (1.-2.*n2)*L2 + 3.*n2*(t2-V2-4.*n2*t2)*B2)
        *V2/cos(dLh);
    dA  = dLh*sinb
        *(1.0    + V2*(2.0*L2            +(3. + 5.*n2)*B2));

  } while (fabs(dB - dBt) > 1.0e-13
      ||   fabs(dL - dLt) > 1.0e-13
      ||   fabs(dA - dAt) > 1.0e-13
      );

/* Reverse azimuth, longitude, and latitude */
  dA    = az1  + 2.0*dA;
  *lng2 = lng1 + 2.0*dLh;
  *az2  = dA - (dA > 0.0 ? M_PI : -M_PI);
  *lat2 = lat1 + dB;

  /* Test for high latitude and line exceeding 64 km */
  return((fabs(lat1) < 86.0/180.0*M_PI && s < 0.01*a) ? 0 : 1);
}


