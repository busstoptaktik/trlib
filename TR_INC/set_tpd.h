

/* set_tpd.h                        # page 1   12 Jan 2010 13 14 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */


extern struct typ_dec      *set_tpd(
/*________________________________*/
char                       *name,
int                         fwd,
int                         dec
);

/* See typ_dec,h for paremeter descriptions

set_tpd returns a type decimal as described in the string *name
fwd gives the number of digits before the decimalpoint
(a negative fwd gives a standard output)
and dec gives the number of decimals
(a negative dec gives zero decimals)

PROG: KP 1989
Karsten Engsager: updated 2010.
*/


