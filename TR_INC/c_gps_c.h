

/* c_gps_c    ver 2003.01             # page 1    4 Jul 2003 12 06 */


/* Copyright (c) 1999 LG, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of LG, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */


extern int       c_gps_c(
/*_____________________*/
struct gps_c_str    *shp,
double                Xi,
double                Yi,
double                Zi,
double                *Xo,
double                *Yo,
double                *Zo
);

/* transforms cartesian coordinates to cartesian coordinates */

/* Molodensky transformation SMALL DATUM SHIFTS only */

/* (*Xo) = (Xi) + (T1) + ( D  -R3  R2) (Xi)             */
/* (*Yo) = (Yi) + (T2) + ( R3  D  -R1) (Yi)             */
/* (*Zo) = (Zi) + (T3) + (-R2  R1  D ) (Zi)             */


