

/* gatg.c      version 1.0          # page 1   10 Aug 1994 17 25 */


/* Copyright (c) 1992 GD, U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include  <math.h>
#include  <stdio.h>

double        gatg(
/*_______________*/
double               *TC,
int                 direct,
double               B
)

{
  double            *gb, *bg;
  double            *p, *p1;
  double            h, h1, h2, cos_2B;

  gb = TC;
  bg = gb + 5;

  /* direction of the transformation */
  if (direct) {
    /* Clenshaw sine summation */
    p1     = (direct > 0) ? gb : bg;
    cos_2B = 2.*cos(2.0*B);
    h      = h2     = 0.0;
    for (p = p1 + 5, h1 = *--p; p - p1; h2 = h1, h1 = h)
      h = -h2 + cos_2B*h1 + *--p;
    B = B + h*sin(2.0*B);
  }
  else {
    (void) fprintf(stderr,
        "\nUndefined direction of Gauss <-> geo transformation");
    B = 1.0e+30;
  }
  return(B);
}


