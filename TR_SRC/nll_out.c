
/* nll_out.c                      # page 1   12 Feb 1993 16:18 */


/* Copyright (c) 1993 GD, Kort-og Matrikelstyrelsen, Denmark   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include <stdio.h>
#include <math.h> 
#include "nll.h" 

int                nll_out(
/*_______________________*/
FILE              *out,
struct nl_ca_str  *CAT,
double            *N,
int                f_col,
int                l_col,
int                ts)

/* 

CAT          (call)        *struct nl_ca_str
The catalogue giving the packing of N.

N            (call)         real *double
A  block of reduced elements of normal equations.

ts           (call)         int 
  1:         ureduceret NORMALLIGNING
  2:          reduceret NORMALLIGNING
  3:            backsol NORMALLIGNING
  4:            inverse NORMALLIGNING

Prog: Karsten Engsager MAR 1993, C version.
*/

{
  struct nl_ca_str     *cat;
  double               *pN;
  int                  sz;
  int                  col, row, t;

  switch (ts) {
  case 1: (void) fprintf(out, "\n\nureduceret NORMALLIGNING\n");
    break;
  case 2: (void) fprintf(out, "\n\n reduceret NORMALLIGNING\n");
    break;
  case 3: (void) fprintf(out, "\n\n   backsol NORMALLIGNING\n");
    break;
  case 4: (void) fprintf(out, "\n\n   inverse NORMALLIGNING\n");
    break;
  default : break;
  }

    cat  = CAT + f_col;
    for (col = f_col; col <= l_col; col++, cat++) {
      sz =     cat->sz_f;
      pN = N + cat->ix_0 + sz;
      (void) fprintf(out, "\n\ncol = %3d, pn0 = %5d, sz = %3d",
                     col, cat->ix_0, sz);

      for (row = sz, t = 1; row <= col; row++) {
        if ((t++) % 4 == 1) (void) fprintf(out, "\nrow = %3d  ", row);
        (void) fprintf(out, "  %10.3e", *(pN++));
      }  /* row - loop */
      (void) fprintf(out, "  status %d", cat->st_f);
    }  /* col - loop */

  return(0);
}

