/*
 * Copyright (c) 2011, National Survey and Cadastre, Denmark
 * (Kort- og Matrikelstyrelsen), kms@kms.dk
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 */
 
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

