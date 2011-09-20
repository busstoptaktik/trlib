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
 


/*  gmb1.h                            # page 1    1 Mar 1993 11:18 */


/* Copyright (c) 1991 U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */


extern int                 gmb1(
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
);

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

