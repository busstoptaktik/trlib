

/* nll_i.h                                       # page  */


/* Copyright (c) 1995 GD, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                      */

/* This is unpublished proprietary source code of GD, Kort-  */
/* og Matrikelstyrelsen, Denmark.  This copyright claim does */
/* not indicate an intention of publishing this code.        */


extern void           nll_i(
/*________________________*/
double               *N,
unsigned int          n
);

/* nll_i: Produces the inverse matrix of a Cholesky-reduced matrix N */
/*        The inverse is returned in N. The rhs is lost              */

/* N:     Cholesky reduced normal equations, dim 0:n-1               */
/*        rhs in column n is used as work space.                     */

/* n:     Number of columns in N, incl. the rhs.                     */

/* Prog: Knud Poder, JULY 1995                                       */
