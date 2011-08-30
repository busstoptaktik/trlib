

/* fputjnsd.c version 1.0             # page 1   21 Sep 1994 12 14 */


/* Copyright (c) 1989 U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include <stdio.h>

int                 fputjnsd(
/*_________________________*/
FILE               *f_o,
double              jnsd
)
{

  char           *p, *p1, c, outstr[24], str[24];
  int             res;
  unsigned int    mask = 1;

  /* first print jnsd_number on string */
  (void) sprintf(str, "%15.0f", jnsd);

  /* set space mask in output */
  mask = ((mask << 1) + (mask << 4) + (mask << 7)
      + (mask << 10) + (mask << 13));

  /* Insert spaces in output */
  for (p = str, p1 = outstr; *p; ) {
    c = *(p++);
    if (c != ' ') {
      *(p1++) = c;
      if (mask & 1) *(p1++) = ' ';
    }
    mask >>= 1;
  }
  *p1 = '\0';

  res =   fprintf(f_o, "%20s ", outstr);

  /*
  return((int) strlen(outstr));
  */
  return(res);
}


