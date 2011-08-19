

/* fgetlhtx.h                       # page 1   09 Feb 1995 14 49 */


/* Copyright (c) 1995  GD,  Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */


#ifndef _FGETLHTX_
#define _FGETLHTX_

extern int         fgetlhtx(
/*________________________*/
FILE             *iofile,
char             *in_str
);

/*
fgetlhtx: input of the forrunner (#) of a labeltext and the labeltext
iofile  : the input file with the label-text
in_str  : the output string with the labeltext (the # is skipped)
return values:

      0  => ok
     -1  => old forerunner (<) input
     EOF => end of file

PROG: Knud Poder, FEB 1995

*/

#endif

