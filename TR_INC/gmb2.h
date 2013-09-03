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
 


/* gmb2.c version 1.0                 # page 1    1 Mar 1993 11:18 */


/* Copyright (c) 1991 U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */


#if !defined     (H_GMB2)
#define           H_GMB2

extern int                 gmb2(
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
);

/*
The function gmb2 solves the inverse (or second) geodetic
main problem using the gaussian mid-latitude formulae.
Reference: JORDAN-EGGERT-KNEISSL: Handbuch der Vermessungs-
kunde Vol. IV, MCMLIX, p. 968-969. Eq. (36), (37), and (38).

gmb2         (call & return)     int
The result of the call:
    0 => ok
    1 => not ok, line > 0.01*a (64 km) or abs(lat) > 86 deg.

a            (call)              double
The semi major axis of the ellipsoid in units of metres.

f            (call)              double
The flattening of the ellipsoid. The flattening or its inverse
may be used freely.

lat1         (call)              double
The latitude of stn1 in radians.

lat2         (call)              double
The latitude of stn2 in radians.

lng1         (call)              double
The longitude of stn1 in radians.

lng2         (call)              double
The longitude of stn2 in radians.

*az1        (return)            double
The azimuth at station 1 towards station 2 in radians in the
interval -pi, +pi.

*az2        (return)            double
The azimuth at station 2 towards station 1 in radians in the
interval -pi, +pi.

*s          (return)            double
The length of the geodesic between the stations in units of
metres.

Prog: Knud Poder NOV 1990 after older algol version by
Frede Madsen 1981 , where 5th order terms were included.
5th order terms removed, as bshlm2 is used for long lines.

*/

#endif
