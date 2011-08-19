

/* bshlm1.h                           # page 1    1 Mar 1993 11:18 */


/* Copyright (c) 1991 U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */


extern int               bshlm1(
/*____________________________*/
double                   a,
double                   f,
double                   B1,
double                   *B2,
double                   L1,
double                   *L2,
double                   A1,
double                   *A2,
double                   S
);



/*
The function bshlm1 solves the direct (or first) geodetic
main problem using the Besssel-Helmert formulae.
Reference: JORDAN-EGGERT-KNEISSL: Handbuch der Vermessungs-
kunde Vol. IV, MCMLIX, p. 968-969. Eq. (xx), (xx), and (xx).

bshlm1         (call & return)     int
The result of the call: 0 => ok, 1 => ok, but the line is
shorter than a/1000000.

a            (call)              double
The semi major axis of the ellipsoid in units of metres.

f            (call)              double
The flattening of the ellipsoid. The flattening or its inverse
may be used freely.

B1           (call)              double
latitude of station 1 in radians.

*B2          (return)            double
latitude of station 2 in radians.

L1           (call)              double
longitude of station 1 in radians.

*L2          (return)            double
longitude of station 2 in radians.

A1          (call)              double
the azimuth at station 1 towards station 2 in radians.

*A2         (return)            double
the azimuth at station 2 towards station 1 in radians.

S            (call)              double
the length of the geodesic between the stations in units of
metres.

Prog: Knud Poder NOV 1990

*/

