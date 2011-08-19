/* nll_bck.c               # page  1 Thu Feb 25 16:04:13 1993 */

/* Copyright (c) 1993, GD, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/D, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include <math.h>
#include <stdio.h>
#include "nll.h"

int                nll_bck(
/*________________________*/
struct nl_ca_str  *CAT,
struct nl_ca_str  *CAT_rhs,
double            *N,
double            *RHS,
int                f_col,
int                l_col,
int                hlm_col,
int                n)

/* nll_back solves normal equations stored columnwize according */
/* to CAT in an upper triangular matrix : double array N.       */
/* N has a total of n columns excl. the rhs                     */
/* (i.e col.no. n is rhs.                                       */
/* Prog. KP 19 NOV 1990.                                         */
/* Prog. KE 25 FEB 1993.                                         */



/* nll_back                # page 2   Feb 25 16:04:13 1993 */


{
  struct nl_ca_str  *p_cat;
  double        *pN_u, *p_rhs, *pr;
  int           sz_u, sz_r;
  int           r, u, lr;

  p_cat = CAT_rhs + n;
  sz_r  = p_cat->sz_f;

  if (sz_r > l_col) return(0);

  p_rhs = RHS + (CAT_rhs+n)->ix_0 + l_col;
  p_cat = CAT + l_col;
  if (sz_r > f_col) f_col = sz_r;

  /* loop over cols of N */
  for (u = l_col; u >= f_col; u--, p_cat--, p_rhs--) {
    lr   = (u<hlm_col) ? u : hlm_col;
    pN_u = N + p_cat->ix_0 + lr;
    sz_u =     p_cat->sz_f;
    if (sz_r > sz_u) sz_u = sz_r;

/*
if (u<0) (void) printf(
"lr=%3d, p_rhs=%5d, pN_u=%5d, ", lr, p_rhs-RHS, pN_u-N);
*/
    if (u < hlm_col) *p_rhs *= *pN_u;
/*
if (u<0) (void) printf("*prhs=%lf\n", *p_rhs);
*/

    /* loop over rows of col */

    for (r = lr-1, pr=p_rhs-1; r >= sz_u; r--, pr--) {
      pN_u --;
/*
if (u<0) (void) printf("r=%3d, pr,=%5d pN_u=%5d, ", r, pr-N, pN_u-N);
*/
      *pr -= (*p_rhs * *pN_u);
/*
if (u<0) (void) printf("*pr=%lf\n", *pr);
*/
      }

  } /* u - loop */

return(0);

}
