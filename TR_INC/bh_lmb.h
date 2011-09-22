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
 


/* bh_lmb.h                           # page 1   22 Feb 1993 12 54 */


/* Copyright (c) 1991 GS, U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */


extern double                bh_lmb(
/*________________________________*/
double           K1,
double           f,
double           bm,
double           s1,
double           s
);


/* 
bh_lmb determines the correction to the spherical longitude difference
       to get the geodetic longtude difference.

K1     The generating parameter for the coefficients of the series of
       the longitude difference and the lengths of the geodesic.

f      flattening of the ellipsoid.


bm     the max positive reduced latitude of a geodesic 
       starting from reduced latitude red_lat in azimuth az.

s1     The length of the great circle on the auxiliary sphere from
       the starting point to the to the max reduced latitude point,
       counted in units of radians.

s      the length of the great circle on the auxiliary sphere from
       the starting point to the object, counted in units of radians.

*/

