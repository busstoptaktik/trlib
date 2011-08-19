

/* get_tpd.h                     # page 1   12 Jan 2010 13 14 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

#include "geo_lab.h"

extern int             get_tpd(
/*___________________________*/
struct typ_dec        *gtfd,
int                   *tp,
char                  *name,
int                   *cfp,
int                   *dec,
double                *cf
);

/* See typ_dec.h for parameter description

get_tpd returns the group number (gr) or zero
of the typ_dec *gtfd.

*tp is the type
*name is the name string
*cfp is the number of characters before the point
*dec is the number of digits after the point
*fc is the conversion factor.

PROG:  KP 1989

*/
