

/* setDtrc.c                        # page 1   10 Aug 1994 11 40 */


/* Copyright (c) 1992 GD, U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

extern int              setDtrc(
/*____________________________*/
struct coord_lab         *c_lab,
double                   *DD
);

/* Function:
setDtrc             (call and return)            int
Produces the constants needed for calculation of scale and
meridan convergence (m, c) in the UTM projection, where the
coordinate system is described by a coord_lab. 
An erroneous c_lab gives the return value -1.

Parameter:
*c_lab             (return)                 coord_lab
giving the ellipsoid parameters.

*DD                 (return)                    double
constants for calculation of m and c.

Prog: K. Engsager  1994.

*/


