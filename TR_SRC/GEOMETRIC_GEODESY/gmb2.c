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

int                        gmb2(
/*____________________________*/
double                     a,
double                     f,
double                     lat1,
double                     lat2,
double                     lng1,
double                     lng2,
double                    *az1,
double                    *az2,
double                    *s
)


{


  double     e2, n2, V, V2, cosb, sinb, t2;
  double     lat, dlat, dlng, dB, dL, B2, L2, A, Am, dA;

  if (f > 1.0) f = 1.0/f;
  e2   = f*(2.0 - f)/(1.0 - f)/(1.0 - f);

  lat  = (lat1 + lat2)/2.0;
  dlat = lat2 - lat1;
  dlng = lng2 - lng1;
  sinb = sin(lat);
  cosb = cos(lat);
  t2   = sinb/cosb;
  t2   = t2*t2;
  n2   = e2*cosb*cosb;
  V2   = 1. + n2;
  V    = sqrt(V2);
  dB  = dlat/V2;
  dL  = dlng * cosb;
  dA  = dlng*sinb/2.0;
  B2  = dB*dB/24.0;
  L2  = dL*dL/24.0;

  dL = dL*(1.0         - t2*L2        + (V2 - 9.*n2*t2)*B2);
  dB = dB*(1.0 + (1.-2.*n2)*L2 - 3.*n2*(t2-V2-4.*n2*t2)*B2)
      * cos(dlng/2.0);
  dA = dA*(1.0 + V2*(   2.0*L2            +(3. + 5.*n2)*B2));

  Am = atan2(dL, dB);
#ifdef  WIN32
  *s = _hypot(dL, dB)*a/(1.0 - f)/V;
#else
  *s = hypot(dL, dB)*a/(1.0 - f)/V;
#endif

  A    = Am + dA;
  *az2 = A - (A >= 0 ? M_PI : -M_PI);
  A    = Am - dA;
  if (fabs(A) > M_PI)
    A    = A - (A > 0 ? 2.0*M_PI : -2.0*M_PI);
  *az1 = A;

  return((*s < a*0.01) ? 0 : (*s < 0.03*a) ? 1 : 2);
  }
