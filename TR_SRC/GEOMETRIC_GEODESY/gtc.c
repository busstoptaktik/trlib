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
 
#include    <math.h>
#include    <stdio.h>
#include    "KmsFncs.h"

int                       gtc(
/*__________________________*/
struct coord_lab           *c_lab,
int                       direct,
double                    B,
double                    L,
double                    H,
double                    *X,
double                    *Y,
double                    *Z,
char                      *usertxt,
FILE                      *tr_error
)

/* Dual autochecking transformation function for geographic
coordinates to cartesian coordinates or reverse. The cartesian
system is righthanded with z-axis towards the mean pole and
x-axis intersecting the Greenwich meridian. The geographic
coordinates are latitude (B), longitude (L) and ellipsoidal
height (H).

gtc       (return)         int
Return value
0           => ok
TRF_TOLLE_  => tolerance exceeded.
TRF_PROGR_  => syntax error.
*/

/*
g_lab       (call)           union geo_lab
The geo_lab union must contain a a coordinate label. Only the
type ( == 1), the init (== 1), the semi major-axis and the
flattening are used, but the imit-mask may be necessary.

direct      (call)           int
The direction of the transformation:

direct  > 0  => geo->crt
direct  < 0  => crt->geo
direct == 0  => alarm: return(UNDEFSYS)

B, L, H,     (call)           double
Input lat, lng, hgt, or X, Y, Z. Lat and lng in radians,
all others in metres.

*X, *Y, *Z   (return)         double
Output X, Y, Z or lat, lng, hgt. Lat and lng in radians,
all others in metres.

*usertxt     (call)           char
text to be printed when error occurs (see below)

*tr_error    (call)           FILE
if (*tr_error == *NON) no error report will be printed.
else error report is printed on file tr_error.

Algol version (ctg) by RF, august 1981.
C-version KP JAN 1991.
errors    KE JAN 2001.
*/

{


  double                   a, f, e2;
  int                      i, s, h, res = 0;
  double                   Cx, Cy, Cz, Co;
  double                   dCx = 0.0, dCy = 0.0, dCz = 0.0, tol;
  double                   p, N, Nh, cosB = 0.0, sinB;
  double                   ia, ie2;

  /* alarm for label error */
  if (c_lab->lab_type == CRD_LAB) {

    /* Ellipsoid parameters */
    a = c_lab->a;
    f = c_lab->f;

    e2 =  f*(2.0 - f);
    /* copy input values */

    if (direct > 0) L = v_red(L);

    Cx = B;
    Cy = L;
    Cz = H;

    if (direct) {
      i = (direct > 0 ? 1 : -1);

      for (s = -i, h = 2*s; h - i; i +=s)
        switch(i) {
        case 1: /* geo->cart */
          cosB = cos(Cx);
          sinB = sin(Cx);
          N    = a/sqrt(1.0 - e2*sinB*sinB);
          Nh   = N + Cz;
          p    = Nh*cosB;
          Cx   = p*cos(Cy);
          Cy   = p*sin(Cy);
          Cz   = (Nh - N*e2)*sinB;
          break;
        case 0: /* result */
          *X = Cx;
          *Y = Cy;
          *Z = Cz;
          break;
        case -1: /* cart->geo */
          /* GDA Technical Manual, Australia  */
          p   =    Cy*Cy + Cx*Cx;
          ia  =  a * e2 / sqrt(p + Cz*Cz);
          p   =  sqrt(p);
          ie2 =  sqrt(1 - e2);
          Cy  =  v_red(atan2(Cy, Cx));      /* longitude, final  */
          Cx  =  atan2(Cz * (ie2 + ia), p);
          Co  =  cos(Cx);
          Cx  =  sin(Cx);
          Cx *=  Cx * Cx;
          Co *=  Co * Co;
          ia  =  a * e2;
          Cx  = atan2(Cz + ia / ie2 * Cx, p - ia * Co);
          Co  = sin(Cx);
          Cz  = p * cos(Cx) + Cz * Co - a * sqrt(1 - e2 * Co * Co);
          break;
        } /* end switch(i) */

      if (res == 0) {
        /* tolerance check of results */
        dCx = Cx - B;
        dCy = Cy - L;
        dCz = Cz - H;

        if (direct > 0) {
          dCx *= a;
          dCy  = v_red(dCy)*a*cosB;
        }
        tol = a*0.5e-6/RHO; /* 0.5 micro arcsec */

        if (   fabs(dCx) < tol
            && fabs(dCy) < tol
            && fabs(dCz) < tol) return(0);
      }
      res = TRF_TOLLE_;
    }
    else res = TRF_PROGR_;
  }
  else res = TRF_PROGR_;

  return((tr_error==NULL || res == 0) ? res :
          t_status(tr_error, usertxt, "gtc", res,
          (direct > 0) ? "sx" : "m ", "m ",
          B, L, H, dCx, dCy, dCz));

}
