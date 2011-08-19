

/* sgeti.h version 1.0              # page 1   19 Jan 1996 12 09 */


/* Copyright (c) 1993, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                    */

/* This is unpublished proprietary source code of GD, Kort-og  */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#if !defined  (H_SGETI)
#define        H_SGETI

extern int       sgeti(
char                  *in,
int                   *bit_desc
);

/*

function sgeti : read lowercase characters from string and convert
                 them to descriptor bitpattern.
                 Terminate on the first non lowercase character.

sgeti            (call and return)             int
                 Number of lowercase- and alphacharacters read
                 (not the same as legal descriptors)

in               (call)                        *char
                 Char array holding the descriptors

bit_desc         (return)                      int 
                 Bitpattern holding the coordinate descriptors

See stdescr.h for further details on descriptors etc.

*/

#endif


