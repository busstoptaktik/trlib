

/* fputst.h                          # page 1    1 Mar 1993 11:18 */


/* Copyright (c) 1990 U/32, Kort-og Matrikelstyrelsen, Denmark */
/* Copyright (c) 1991 U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */


extern int              fputst(
/*___________________________*/
FILE                *o_str,
int                  lab_rgn,
struct st_str       *stn,
char                *rgx,
int                 wdt
);


/*
the stationnumber in *stn is written on file *o_str
if (stn->rgn != lab_rgn) a region prefix is written
the width gives a standard width of output
and *rgx gives a number of newlines and/or 
:  '\0'  : no precedence
:  '-'   : 4 'sp' precedence
:  '+'   : 4 'sp'  or region  precedence
:  others: preceedenchars
*/


