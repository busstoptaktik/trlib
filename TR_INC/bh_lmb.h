

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

