

/* bh_lat.h                           # page 1   22 Feb 1993 12 54 */


/* Copyright (c) 1991 GS, U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */


extern double           bh_lat(
/*___________________________*/
double                  b,
double                  az,
double                  em2,
double                  *K1,
double                  *s1
);

/*

bh_lat determines the max positive reduced latitude of a geo-
       desic starting from reduced latitude red_lat in azimuth az.

b      The reduced latitude of the geodesic where the azimuth is az,
       in units of radians.

az     The azimuth of the geodesic in reduced latitude red_lat. The
       azimuth is counted from north, but the orientation of the geo-
       desic is immaterial. The unit is radians.

em2    The second eccentricity of the ellipsoid = f*(2-f)/(1-f)/(1-f).

K1     The generating parameter for the coefficients of the series of
       the longitude difference and the lengths of the geodesic.

s1     The length of the great circle on the auxiliary sphere from
       the starting point to the to the max reduced latitude point,
       counted in units of radians.

Prog.: Knud Poder SEP 1991.

*/


