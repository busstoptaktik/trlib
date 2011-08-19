
/* f_nxt.c                                  * page   */

/* Copyright (c) 1995, GD, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include      <stdio.h>

char                *f_nxt(
/*_______________________*/
char                *str,
int                 *items,
FILE                *infile,
int                 used
)

{
#include         "fgetln.h"

  static char      *cp;

  if (*items == 0) {
    (void) fgetln(str, items, infile);
    cp = str;
  }
  else {
    cp += used;
  }

  --(*items);

  return(cp);
}


