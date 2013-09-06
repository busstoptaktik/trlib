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
 


/*  gtc.h                        # page 1   22 Feb 1993 12 54 */


/* Copyright (c) 1990 GD,   Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#if !defined     (H_GTC_FNC_)
#define           H_GTC_FNC_

extern int                gtc(
/*__________________________*/
struct coord_lab             *g_lab,
int                       direct,
double                    B,
double                    L,
double                    H,
double                    *X,
double                    *Y,
double                    *Z,
char                      *usertxt,
FILE                      *tr_error
);

/* Function:
Dual autochecking transformation function for geographic
coordinates to cartesian coordinates or reverse. The cartesian
system is righthanded with z-axis towards the mean pole and
x-axis intersecting the Greenwich meridian. The geographic
coordinates are latitude (B), longitude (L) and ellipsoidal
height (H).

ctg       (return)         int, transf. fnc no. = 2.
Return value
        0    => ok
        1<<2 => tolerance exceeded.
        -1   => severe error, gtc cannot work.

g_lab       (call)           struct coord_lab
The geo_lab union must contain a a coordinate label. Only the
type ( == 1), the init (== 1), the semi major-axis and the
flattening are used, but the imit-mask may be necessary.

direct      (call)           int
The direction of the transformation:

    direct > 0  => geo->crt
    direct < 0  => crt->geo
    direct == 0 => alarm: return(UNDEFSYS)

(continued)
*/

/*   # page     */

/*

B, L, H,     (call)           double
Input lat, lng, hgt, or X, Y, Z. Lat and lng in radians,
all others in metres.

*X, *Y, *Z   (return)         double
Output X, Y, Z or lat, lng, hgt. Lat and lng in radians,
all others in metres.

tr_cnt       (call)           int
Control of error actions for tolerance exceeded. The tolerance
is 0.5 micro arcsec, i.e. 15 microns on the earth.
     tr_cnt = 0  => alarm: return(UNDEFSYS)
     tr_cnt = 1  => error mess on stderr, return of 1<<2.
     tr_cnt > 1  => no    mess          , return of 1<<2;

Algol version (ctg) by RF, august 1981.
C-version KP JAN 1991.
*/

#endif
