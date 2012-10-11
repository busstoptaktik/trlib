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


