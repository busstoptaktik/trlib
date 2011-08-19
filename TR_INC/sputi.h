

/* sputi.h version 1.0              # page 1   19 Jan 1996 12 39 */


/* Copyright (c) 1993, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                    */

/* This is unpublished proprietary source code of GD, Kort-og  */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

extern int      sputi(
char           *txt_desc,
int            *bit_desc
);

/*

function sputi : convert bitpattern descriptors into strings.

sputi            (call and return)           int
                 Number of descriptors converted

txt_desc         (return)                    *char
                 descriptors i alphabetic order

bit_desc         (call)                      int 
                 Bit pattern holding the descriptors

See stdescr.h for further details on descriptors etc.

*/


