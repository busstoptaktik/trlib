

/* sputi.c version 1.0                # page 1   02 Sep 1993 10 27 */


/* Copyright (c) 1997, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                   */

/* This is unpublished proprietary source code of ITG, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

/* Prog.: F. Steffensen AUG. 1997 (MI/ITG) */

#include        "stdescr.h"


int       sputi(
char           *txt_desc,
int            *bit_desc
)

{
  struct t_descrip    *p_descrip;
  int                 c = 0;
  unsigned short      i;

  *txt_desc = '\0';
  for (p_descrip = coo_dsc, i = 0;
      *(p_descrip->desc); p_descrip++, i++)
    if ((*bit_desc >> (p_descrip->b_no)) & ~(~0 << 1)) {
      c++;
      *txt_desc++ = *(p_descrip->desc);
    }

  *txt_desc = '\0';

  return(c);
}


