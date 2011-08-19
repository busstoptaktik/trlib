

/* sputcw.h version 1.0              # page 1    1 Mar 1993 11:18 */


/* Copyright (c) 1994, Kort-og Matrikelstyrelsen, GD, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of GD, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not */
/* indicate an intention of publishing this code.             */

extern int         sputcw(
/*______________________*/
char              *str,
double             cw,
int                mode
);

/*

sputcw       Number of characters written

str          pointer to a char array

cw           G.I. date_time, MD comment word or jnsd

mode         ==1 writes month in text (day month year, clock)
             ==3 writes month in text    and no clock at ISO
             ==0 writes month in number
             ==2 writes month in number  and no clock at ISO

ISO (our)   1992 04 17,  9.45
simple            1992
MD            1 00045 89 02 3 012   max < 900 00000 00 00 0 000

*/

