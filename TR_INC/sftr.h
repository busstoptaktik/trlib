

/* sftr.h                        # page 1   22 Feb 1993 12 54 */


/* Copyright (c) 1989 U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */


extern double            sftr(
/*__________________________*/
double            C,
double            a,
double            b,
double            *A,
double            *B
);

/* Function:
Determination of a side and two adjacent angles in a spherical
triangle, where an angle and two adjacent sides are known. All
angles and sides are in radians.

sftr            call and return            double
The side c opposing the known angle C.

C               call                       double
The known angle.
a, b            call                       double
The two known sides, adjacent to C.

*A, *B          return                     double
The angles A (opposing a) and B (opposing b)

      ............. c .............
        .A                    B .
          .                   .
            .               .
              b           a
                .       .
                  . C .
                    .

PROG: KP 1989

*/

