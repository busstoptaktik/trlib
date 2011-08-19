

/* fgetg.h                            # page 1   22 Feb 1993 12 54 */


/* Copyright (c) 1991 U/32, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */


extern double               fgetg(
/*______________________________*/
FILE                        *str,
struct typ_dec              *tpd,
int                         *used,
char                        *udt
);

/*
fgetg returns a geodetic value read from the FILE *str.
*tpd contains the type and decimal of the read value
*used is the number of characters read
*udt is a terminating type string, which is used when none is read

reading is stopped after two 'sp' or a terminating string

the values are :
for metric values     in units of metres
for angular values    in units of radians
for potential values  in units of gpu
for gravity values    in units of gal
for mean error values in units of 1

*/

