

/* v_std.h                        # page 1   22 Feb 1993 12 54 */


/* Copyright (c) 1989 U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

extern double      v_std(
/*_____________________*/
double             v
);

/* Function:
v_std modifies the angle to be zero or positive less than 2pi.

v_std  an angle in the interval 0.0 to 2pi in radians.

v      The angle in radians to be reduced.

Prog KP, 1989 

*/

