

/* nll_out.h                                       # page  */


/* Copyright (c) 1995 GD, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                      */

/* This is unpublished proprietary source code of GD, Kort-  */
/* og Matrikelstyrelsen, Denmark.  This copyright claim does */
/* not indicate an intention of publishing this code.        */


extern void         nll_out(
/*________________________*/
FILE                 *f,
struct nl_ca_str     *CAT,
double               *N,
int                   f_col,
int                   l_col,
int                   ts
);

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
  
*/

