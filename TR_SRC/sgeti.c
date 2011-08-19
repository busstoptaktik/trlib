

/* sgeti.c version 1.0              # page 1   19 Jan 1996 12 13 */


/* Copyright (c) 1993 U/32, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include       <stdio.h>
#include       <string.h>
#include       <ctype.h>
#include       "stdescr.h"

int       sgeti(
char            *in,
int             *bit_desc
)

{
  struct t_descrip    *p_descrip;

  int                 c, alng = 0;

  char                *p = in;

  while ((islower(*p) && isalpha(*p))
      || (*p =='æ' || *p =='ø' || *p =='å')) {
    p++;
    alng++;
  }
  p         = in;
  c         = alng;
  *bit_desc = 0;

  while (c) {
    for (p_descrip = coo_dsc; *(p_descrip->desc); p_descrip++)
      if (!strncmp(p_descrip->desc, p, 1)) {
        *bit_desc = (*bit_desc | (1 << p_descrip->b_no));
        break;
      }
    p++;
    c--;
  }

  return(alng);
}


