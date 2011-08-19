

/*  fputg.h version 1.0               # page 1   22 Feb 1993 12 54 */


/* Copyright (c) 1989 U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

extern int               fputg(
/*___________________________*/
FILE                    *f,
double                  val,
struct typ_dec          *typ,
char                    *layout
);


/* converts val acording to *typ and writes on FILE *f

   layout may be given :
   "\0"   : default
   " "    : include blanks in output
   "0"    : start with zero
   "+"    : start with + (before possible zero)
   in any combination

*/

