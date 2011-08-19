

/*  fputg   version 1.0           # page 1   22 Feb 1993 12 54 */


/* Copyright (c) 1989  LG, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of LG, Kort-og  */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include <stdio.h>
#include "geo_lab.h"

int                      fputg(
/*___________________________*/
FILE                    *f,
double                  val,
struct typ_dec          *typ,
char                    *layout
)

{
#include          "sputg.h"

  char            outstr[80];
  int             i;

  i = sputg(outstr, val, typ, layout);
  if (fputs(outstr, f) == EOF) i = -1;
  return(i);
}


