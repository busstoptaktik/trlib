

/* fputst   version 1.0             # page 1    1 Mar 1993 11:18 */


/* Copyright (c) 1991 LG, Kort-og Matrikelstyrelsen, Denmark   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of LG, Kort-og  */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include  <stdio.h>
#include  "geo_lab.h"

int                     fputst(
/*___________________________*/
FILE                *o_str,
int                  lab_rgn,
struct st_str       *stn,
char                *rgx,
int                 wdt
)

{

#include          "sputst.h"

  char          str[200], *p = str;

  (void) sputst(p, lab_rgn, stn, rgx, wdt);

  return(fprintf(o_str,"%s", p));

}

