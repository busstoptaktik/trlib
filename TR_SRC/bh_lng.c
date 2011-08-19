

/* bh_lng.c                           # page 1   22 Feb 1993 12 54 */


/* Copyright (c) 1991 GS, U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include <math.h>

#ifndef    M_PI
#include   "kms_math.h"
#endif

double                     bh_lng(
/*______________________________*/
double                     b,
double                     l,
double                     az
)

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


/* bh_lng.c                           # page 2   22 Feb 1993 12 54 */


{

#include        "v_red.h"

double l_top;

if (fabs(v_red(az)) > M_PI_2)
az = (v_red(az + M_PI));

l_top = atan2(cos(az), sin(b)*sin(az));

 return(v_red(l + l_top));


}


                







