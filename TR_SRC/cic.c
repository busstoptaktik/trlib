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
 


/* cic    ver 1999.01               # page 1   12 Jan 1999 12 06 */


/* Copyright (c) 1999 LG, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of LG, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include          <stdio.h>
#include          <math.h>
#include          "geo_lab.h"

int                   cic(
/*______________________*/
struct dsh_str        *shp,
int                   direct,
double                Xi,
double                Yi,
double                Zi,
double                *Xo,
double                *Yo,
double                *Zo,
char                  *usertxt,
FILE                  *tr_error
)
{

#include             "t_status.h"

  int                i, b, s, e, d;
  double             X, Y, Z, Xw, Yw, Zw;
  double             tol = 1.0e-6;

  if (shp->tp == 0) {
    *Xo = Xi;
    *Yo = Yi;
    *Zo = Zi;
  } else {
    d = direct*(shp->tp);
    X = Xi;
    Y = Yi;
    Z = Zi;
    if (d > 0) {
      b = 1;
      s = +1;
      e = +5;
    }
    else if (d < 0) {
      b = 3;
      s = -1;
      e = -1;
    }
    else
      return(t_status(tr_error, usertxt,
             "\n*** cic: %s UDEFINERET RETNING;\n", TRF_PROGR_));


/* cic    ver 1999.01               # page 2   12 Jan 1999 12 06 */


/*
(void) printf("\nINPUT   %+12.3f   %+12.3f   %+12.3f", X, Y, Z);
*/
    for (i = b; i != e; i += s) switch(i) {

    case 1: /* forward */
      Xw = X;
      Yw = Y;
      Zw = Z;
      X  = shp->tx
          + shp->scale*(shp->r11*Xw + shp->r12*Yw + shp->r13*Zw);
      Y  = shp->ty
          + shp->scale*(shp->r21*Xw + shp->r22*Yw + shp->r23*Zw);
      Z  = shp->tz
          + shp->scale*(shp->r31*Xw + shp->r32*Yw + shp->r33*Zw);
      break;

    case 2: /* results */
      *Xo = X;
      *Yo = Y;
      *Zo = Z;
/*
(void) printf("\nOUTPUT  %+12.3f   %+12.3f   %+12.3f", X, Y, Z);
*/
      break;

    case 3: /* reverse */
      Xw = X - shp->tx;
      Yw = Y - shp->ty;
      Zw = Z - shp->tz;
      X  = (shp->r11*Xw + shp->r21*Yw + shp->r31*Zw)/shp->scale;
      Y  = (shp->r12*Xw + shp->r22*Yw + shp->r32*Zw)/shp->scale;
      Z  = (shp->r13*Xw + shp->r23*Yw + shp->r33*Zw)/shp->scale;
      break;

    case 0:
    case 4: /* control */
/*
(void) printf("\nCHECK   %+12.3f   %+12.3f   %+12.3f", X, Y, Z);
(void) printf("\nDIFF    %+12.7e   %+12.7e   %+12.7e\n",
X-Xi, Y-Yi, Z-Zi);
*/

      if     (fabs(X - Xi) < tol
          &&  fabs(Y - Yi) < tol
          &&  fabs(Z - Zi) < tol) return(0);
      else return(t_status(tr_error, usertxt, 
                   "cic", TRF_TOLLE_,
                   "m ", "m ", X, Y, Z, X - Xi, Y - Yi, Z - Zi));
    }

  }
  return(0);
}

