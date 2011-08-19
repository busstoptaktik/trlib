
/* nll_red.c  version 1.0                # page 1   12 Feb 1993 16:18 */


/* Copyright (c) 1993 GD, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include <stdio.h>
#include <math.h> 
#include "nll.h" 

int                nll_red(
/*_______________________*/
struct nl_ca_str  *CAT, 
double            *Nr,
double            *Nu,
int                f_col_r,
int                l_col_r,
int                f_col_u,
int                l_col_u,
int                hlm_col,
int                exp_lim)

/* 
nll        (call and return)    function
Operates  on two datamatic blocks of normal equations after
CHOLESKY. The value of the function is

(1) Nr != Nu  0         : undefined
(2) Nr == Nu, coef part : max binal loss

The right_hand_side must be own-reduced by nll_rhs.
The Helmert block must be own-reduced by nll_hlm.
The backsolution must be done by nll_bck.

The columns are numbered from 0 to n, asssuming n-1 unknowns,
so that a formal declaration of the normal equations in a 
full array would have been N[n][n]. The normals are actually 
stored blockwize in arrays pointed by Nr and Nu. The catalo-
gues pointed by CAT contail the catalogie of cols adresse by the
col_no.

Reciprocal  values  of  the  diagonal  coeff.  elements are
stored with a minus for imaginary values and a zero in case
of singularity.

CAT          (call)        *struct nl_ca_str
The catalogue giving the packing of N.

Nr           (call)         real *double
A  block of reduced elements of normal equations.

Nu       (call and return)  real *double
A block of unreduced or partially reduced normal equations. 

hlm_col gives the first row of the helmert limit.

Prog: Knud Poder MAR 1983, algol version.
      Knud Poder SEP 1991, C version. 0.
      Karsten Engsager MAR 1993, C version.

*/


/* nll2.c  version 1.0                # page 2   12 Feb 1993 16:18 */

{
  struct nl_ca_str     *cat_r, *cat_u;

  double        *pN0_r, *pN0_u, *p_r, *p_u, *pNd_r, *pNa_u;
  int           sz_r, sz_u;
  int           r, u;
  double        sum, NI;
  double       *n_s = &sum; /* scalar product contr.*/
  int           exp_NI;     /* exp of unred. elem.  */
  int           p_rh;       /* rel p. of h-col      */
  int           dia_res;    /* res of dia operation */
  int           t;          /* work var.            */
  int           max_loss;   /* collect max losses   */

    max_loss = 0;

    /* reduction*/
    /* loop over unred cols*/
    cat_u  = CAT + f_col_u;
    for (u = f_col_u; u <= l_col_u ; u++, cat_u++) {
      pN0_u = Nu + cat_u->ix_0;
      sz_u  =      cat_u->sz_f;
/*(void) fprintf(stdout, "u=%3d, pn0=%5d, szu=%3d\n",u,pN0_u-Nu,sz_u);*/

      /* loop over red cols*/
      t      = (f_col_r < sz_u) ? sz_u : f_col_r;
      cat_r  = CAT + t;
      for (r = t; r <= l_col_r && r <= u; r++, cat_r++) {
        pN0_r = Nr + cat_r->ix_0;
        sz_r  =      cat_r->sz_f;
        if (sz_r < sz_u) sz_r = sz_u;
        pNd_r = pN0_r + r; /* divisor */
        pNa_u = pN0_u + r; /* result */
        p_r   = pN0_r + sz_r; /* scalar product*/
        p_u   = pN0_u + sz_r;
        p_rh  = (r < hlm_col) ? 1 : 2;
        switch(p_rh) {
        case 1: /* simple neg accum. */
          n_s = pN0_r + r;
          break;
        case 2: /* neg. accum to hlmlimit */
          n_s = pN0_r + hlm_col;
          break;
        }

        sum = 0.0;
        /* neg accum */
        while (p_r < n_s)
          sum += (*(p_r++)*(*p_u++));


/* nll2.c  version 1.0                # page 3   12 Feb 1993 16:18 */


        /* check and store reduced coef */
        switch(p_rh) {
        case 1: /* store, divide or sqrt */
          if (r != u) /* non_dia-elements */
            *pNa_u = (*pNa_u - sum) * (*pNd_r);
          else { /* dia-elements */
            /* unreduced element */
            NI      = *pNa_u;
            *pNa_u -= sum;
            /* result and loss of binals*/
            (void) frexp(NI,     &exp_NI);
            (void) frexp(*pNa_u, &t);
            exp_NI -= t;
            if (exp_NI > 53) exp_NI = 53;
            if (exp_NI > max_loss) max_loss = exp_NI;
            if (exp_NI < exp_lim && *pNa_u != 0.0)
              dia_res = (*pNa_u > 0.0 ? 1 : 2);
            else dia_res =
              ((*pNa_u == 0.0) && (sum == 0.0)) ? 4 : 3;
            cat_u->st_f = (unsigned char) dia_res;
            cat_u->loss = (unsigned char) exp_NI;
            /* inverse diagonal element */
            switch(dia_res) {
            case 1:
              *pNa_u =  1.0/sqrt(*pNa_u);
              break;
            case 2:
              *pNa_u = -1.0/sqrt(-*pNa_u);
              break;
            default:
              *pNa_u = 0.0;
              break;
            }
          }
          break;
        case 2: /* store, no div or sqrt */
          /* partially reduced elements**/
          *pNa_u -= sum;
          break;
        }  /* p_rh swi  */
      }  /* r - loop */
    }  /* u - loop */

  return(max_loss);

}

