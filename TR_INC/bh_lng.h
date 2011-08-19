

/* bh_lng.h                           # page 1   22 Feb 1993 12 54 */


/* Copyright (c) 1991 GS, U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */


extern double              bh_lng(
/*______________________________*/
double                     b,
double                     l,
double                     az
);

/*

bh_lng determines the longitude of the point of max latitude of
       the geodesic starting from reduced latitude b in azimuth az.

b      The reduced latitude of the geodesic where the azimuth is az,
       in units of radians.

l      The longitude of the point on the geodesic with azimuth az,
       in units of radians.

az     The azimuth of the geodesic in reduced latitude red_lat. The
       azimuth is counted from north, but the orientation of the geo-
       desic is immaterial. The unit is radians.

Prog.: Knud Poder SEP 1991.
*/


