

/* nll_s.c                            # page 1   22 Feb 1993 12 54 */


/* Copyright (c) 1990 U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */


extern double   *nll_s(
double          *N,
unsigned int     n,
unsigned char    l_lim,
unsigned char   *m_loss,
unsigned char   *loss,
double          *enp
);

/* nll_s solves symmetric normal equations stored columnwize.
   only the upper triangular part is used i N.
N        pointer to Normal equations upper part including 
         right hand side (rhs) of N. N must have (n+1)*n/2 elements.
n        the number of columns including rhs.
l_lim    limit for loss of binary digits (lbd) of the diagonal element.
         when lbd is bigger than l_lim the actual column and row and
         associated unknown is considered to be singular, so
         that the result for the singular unknown is set to zero.
         the solution of the normal equations is else not affected by
         the singular columns.
m_loss   pointer to storage for max. lbd (char).
loss     pointer to storage of lbd for each column (char).
enp      pointer to storage for numerical exponent of precission.

Prog. KP 19 NOV 1990
*/

