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
 


/* clenshaw.h version 1.0             # page 1   11 Aug 1994 12 54 */


/* Copyright (c) 1992 GD, U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

extern double     clenshaw(
/*_______________________*/
int               M,
double           *a,
int               g,
double            arg_r,
double            arg_i,
double           *R,
double           *I);

/*
a:           coeffs of the series, decl. a[8] at least.
g:           The number of coefficients in the series.
argr, argi:  The real and imaginary part of the argument of cosine. 
R, I:        The real and imaginary part of the sum.

clenshaw:
Computes the sum of a series A[j-1]*F(j*argr + i*j*argi)
(where i is the imaginary unit) by Clenshaw summation
from g down to S. The coefficients are here real and
the argument of F is complex. The real part of the sum
is the value of the function.

The function finds in complex arithmetic an array h
so that h(j) := A(j) + 2*G(argr+i*argi)*h(j+1) - h(j+2)
with h(g+1) = h(g+2) = 0.

Given M the A[j], S and the functions F and G are :
-------------------------------------------------
      C :   a[j], 1         Complex_cos Complex_cos
      S :   a[j], 1         Complex_sin Complex_sin
      G : a[j+1], 0   Complex_chebyshev arg_r+j*argi
      c :   a[j], 1              cos    cos
      s :   a[j], 1              sin    sin
      g : a[j+1], 0           Chebyshev arg_r

Given M the sum of the series is :
--------------------------------
      C :      -h(1)+h(0)*cos(argr+i*argi)
      S :            h(0)*sin(argr+i*argi)
      G :      -h(1)*(argr+i*argi)+h(0)
      c :      -h(1)+h(0)*cos(argr)
      s :            h(0)*sin(argr)
      g :      -h(1)*argr+h(0)

Prog.: K. Engsager, MAY 1994 
*/
