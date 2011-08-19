

/* set_trc.c                        # page 1   10 Aug 1994 11 40 */


/* Copyright (c) 1992 GD, U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

extern int              set_trc(
/*____________________________*/
struct coord_lab         *c_lab
);

/* Function:
set_trc             (call and return)            int
Produces the constants needed for a transformation,
where the coordinate system is described by a coord_lab. 
The return value is the number of the coordinate system.
An erroneous c_lab gives the return value -1.

Parameter:
*c_lab             (call and return)           coord_lab
The constants needed for the transformation are inserted
in c_lab. A flattening given as inverse flattening is
converted to the flattening. E.g. 297 is converted to
the value of 1/297. The constants for transformation
geographical coordinates to/from Gaussian coordinates
are always inserted - even if not needed by the actual
coordinate system.

Prog: Knud Poder  1992.

*/


