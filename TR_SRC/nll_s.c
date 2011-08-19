

/* nll_s.c                            # page 1   22 Feb 1993 12 54 */


/* Copyright (c) 1990 U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include <math.h>
#include <stdio.h>

double   *nll_s(
double         *N,
unsigned int    n,
unsigned char   l_lim,
unsigned char  *m_loss,
unsigned char  *loss,
double         *enp)

/* nll_s solves symmetric normal equations stored columnwize.
only the upper triangular part is used i N.
N        pointer to Normal equations upper part including
         right hand side (rhs) of N. N must have (n+1)*n/2 elements.
n        the number of columns including rhs.
l_lim    limit for loss of binary digits (lbd) of the diagonal element.
         when lbd is bigger than l_lim the actual column and
         associated unknown is considered to be singular, so
         that the result for the singular unknown is set to zero.
         the solution of the normal equations is else not affected by
         the singular columns.
m_loss   pointer to storage for max. lbd (char).
loss     pointer to storage of lbd for each column (char).
enp      pointer to storage for numerical exponent of precission.

Prog. KP 19 NOV 1990
*/


/* nll_s.c                            # page 2   22 Feb 1993 12 54 */


{
  double      *N_r, *N_u, *N_t, *N_c, *N_k;
  double      N_o, sum;

  /* init */
  N_u  = N;
  N_k  = N + n*(n + 1)/2 - 1;

  /* forward reduction */
  for (N_c = N; N_u <= N_k; N_c = N_u) {
    for (N_r = N; N_r <= N_c; N_u++) {
      for (N_t = N_c, sum = 0.0; N_t < N_u;  
          sum += *(N_r++)*(*(N_t++)));
      if (N_r != N_u) *N_u = (*N_u - sum)*(*N_r++);
      else {
        N_o   = *N_u;
        *N_u -= sum;
        if (N_u != N_k) { /* diagonal elem. excl. rhs */
          if (*N_u > 0.0 && N_o > 0.0) {
            *loss = (unsigned char) (log10(N_o/(*N_u)) +0.5);
            if (*loss > *m_loss) *m_loss = *loss;
            if (*loss < l_lim)
              *N_u = 1.0/sqrt(*N_r);
            else *N_u = 0.0;
          }
          else {
            *loss = *m_loss = 20;
            *N_u  = 0.0;
          }
          loss++;
          N_r++;
        }
        else /* rhs diag. */
          *enp = (*N_u > 0.0 && sum > 0.0) ?
              0.5*log10(*N_u/sum) : -50.0;
      }
    }
  }

  /* back solution */
  for (N_t = N_r = N_u-- -n; N_u > N_t; ) {
    *(--N_u) *= *(--N_r);
    for (N_c = N_u; N_c > N_t;  
        *--N_c -= ((*N_u)*(*--N_r)));
  }
  return(N_t);
}


