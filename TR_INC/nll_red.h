
/* nll_red.c  version 1.0                # page 1   12 Feb 1993 16:18 */


/* Copyright (c) 1993 GD, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */


extern int        nll_red(
/*----------------------*/
struct nl_ca_str *CAT, 
double           *Nr,
double           *Nu,
int               f_col_r,
int               l_col_r,
int               f_col_u,
int               l_col_u,
int               hlm_col,
int               exp_lim);

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

