

/* labchsum   ver 2003.01         # page 1    4 Jul 2003 10 03 */


/* Copyright (c) 2003 GEK, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include     <stddef.h>
#include     "geo_lab.h"

int                   labchsum(
/*___________________________*/
union geo_lab        *u_lab,
int                  *long_sum
)

{
  struct coord_lab      *c_lab = &(u_lab->u_c_lab);
  int                    i, sum = 0, sum1 = 0, dim3 = 0;
  char                  *ch_strt, *sepch, *hlab;

  if (c_lab->lab_type != CRD_LAB) {
    for (i = 1000L, ch_strt = (char *) u_lab;
         ch_strt < (char *) long_sum; ch_strt++)
       sum += *ch_strt * (i++);
  } else 
  if (*(c_lab->mlb + c_lab->sepix) != 'H') {
    if (c_lab->ncoord == 3) {  /* set sum for 2 dim only */
      dim3 = 1;
      -- c_lab->ncoord;
    }
    for (i = 1000L, ch_strt = (char *) u_lab;
         ch_strt < (char *) long_sum; ch_strt++)
       sum += *ch_strt * (i++);
    c_lab->ch_tsum = sum;
    sepch          = c_lab->mlb + c_lab->sepix;
    i              = (int) ((ptrdiff_t)sepch
                   - (ptrdiff_t) ((char *) c_lab));
    c_lab->ch_hsum = (1000 + i) * *sepch;
    sum           -= c_lab->ch_hsum; /* take sepch out of sum */
    if (dim3) ++ c_lab->ncoord; /* reset 3 dim */
  } else {   /* sepch == 'H' */
    -- c_lab->ncoord; /* set sum for 2 dim only */
    hlab = c_lab->mlb + c_lab->h_ix -1; /* mlb_lab uden h_lab part */
    for (i = 1000L, ch_strt = (char *) u_lab; ch_strt < hlab; ch_strt++)
       sum += *ch_strt * (i++);
    for ( ; ch_strt <= c_lab->mlb + MLBLNG; ch_strt++) (i++);
    for ( ; ch_strt < (char *) long_sum; ch_strt++)
       sum += *ch_strt * (i++);
    for (i = 1000, ch_strt = (char *) &(c_lab->hstm);
         ch_strt < (char *) &(c_lab->ch_hsum); ch_strt++)
       sum1 += *ch_strt * (i++); /* h_lab (uden h_ix) */
    c_lab->ch_tsum  = sum + sum1;
    sepch           = c_lab->mlb + c_lab->sepix; /* sepch out of sum */
    i               = (int) ((ptrdiff_t)sepch
                    - (ptrdiff_t) ((char *) c_lab));
    sum            -= (1000 + i) * *sepch;
    c_lab->ch_hsum  = sum1;
    ++ c_lab->ncoord; /* reset 3 dim */
  }

  return(sum);
}

