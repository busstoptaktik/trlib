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
 


/* bshlm2g.c                        # page 1   18 Feb 1994 13 55 */


/* Copyright (c) 1993, GD, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#if !defined     (H_BSHLM2G)
#define           H_BSHLM2G

extern int     bshlm2g(
/*___________________*/
double         a,
double         f,
double         B1,
double         B2,
double         L1,
double         L2,
double        *A1,
double        *A2,
double        *S
);

/*The function bshlm2g solves the reverse (or second) geodetic
main problem using the GAUSS mid-latitude formulae for distances
less than appr. 65 km and at latitudes between 85 N and 85 S, 
and the BESSEL-HELMERT formulae otherwise.

Reference: JORDAN-EGGERT-KNEISSL: Handbuch der Vermessungs-
kunde Vol. IV, MCMLIX, p. 968-969. Eq. (xx), (xx), and (xx).

bshlm2g  The result of the call:
         0 => ok
         1 => distance too short, but a result anyway
         2 => no convergence, disaster, probably antipode problem

a        The semi major axis of the ellipsoid in units of metres.

f        The flattening of the ellipsoid. The flattening or its 
         inverse may be used freely.

B1       latitude of station 1 in radians.

B2       latitude of station 2 in radians.

L1       longitude of station 1 in radians.

L2       longitude of station 2 in radians.

A1       the azimuth at station 1 towards station 2 in radians.

A2       the azimuth at station 2 towards station 1 in radians.

S        the length of the geodesic between the stations in units 
         of metres.

Prog: Knud Poder APR 1993.

*/

#endif
