
/* f_next.h                                  * page   */

/* Copyright (c) 1995, GD, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#ifndef   _F_NXTH_
#define   _F_NXTH_

extern char         *f_nxt(
/*________________________*/
char                *str,
int                 *items,
FILE                *infile,
int                 used
);

/*
*f_nxt : next item of a line of character strings from fgetln
*str   : the array of characters used by fgetln
*items : the number of items remaining after call by fgetln
*infile: the stream connect to the file
used   : the number of characters used in the previous call

The function utilises the filtering facilities of fgetln.

A typical call for sgetg would be

G = sgetg(f_nxt(in_a, &q, infile, used), &tpd, &used, udt);

Before first call use:  q = 0; (the value of used is then 
not used).

Use the %n facility in sscanf for finding  the value of used.

PROG: Knud Poder, FEB 1995

*/

#endif

