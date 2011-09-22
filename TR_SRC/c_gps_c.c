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
 


/* c_gps_c    ver 2003.01               # page 1   12 Jan 2003 12 06 */


/* Copyright (c) 2003 GEK, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of LG, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include          <stdio.h>
#include          <math.h>
#include          "geo_lab.h"


int              c_gps_c(
/*______________________*/
struct gps_c_str     *shp,
double                Xi,
double                Yi,
double                Zi,
double                *Xo,
double                *Yo,
double                *Zo
)
{

  /* Molodensky transformation SMALL DATUM SHIFTS only */

#define  MAS         (180.0*3600.0*1000.0/M_PI)

  double             X, Y, Z;


/* c_gps_c    ver 2003.01               # page 2   12 Jan 2003 12 06 */


/*
(void) printf("\nc_gps_c: INPUT   %+12.3f   %+12.3f   %+12.3f",
Xi, Yi, Zi);

(void) fprintf(stdout, "\n*tx, sc : %-8.6f  %-8.6f   %-8.6f  %-8.6f;",
shp->tx, shp->ty, shp->tz, shp->sc*1.0e9);
(void) fprintf(stdout,
"\n*rx    : %-8.6f  %-8.6f   %-8.6f;",
shp->rx * MAS, shp->ry * MAS, shp->rz * MAS);
*/

      /* Molodensky transformation SMALL SHIFTS only */
      X = Xi * shp->sc + shp->tx - Yi * shp->rz + Zi * shp->ry;
      Y = Yi * shp->sc + shp->ty + Xi * shp->rz - Zi * shp->rx;
      Z = Zi * shp->sc + shp->tz - Xi * shp->ry + Yi * shp->rx;
/*
(void) fprintf(stdout, "\n*trf : %-8.6f  %-8.6f   %-8.6f;", X, Y, Z);
*/

      *Xo = Xi + X;
      *Yo = Yi + Y;
      *Zo = Zi + Z;
/*
(void) fprintf(stdout, "\n*res : %-8.6f  %-8.6f   %-8.6f;",
*Xo, *Yo, *Zo);
*/

  return(0);
#undef     MAS
}


