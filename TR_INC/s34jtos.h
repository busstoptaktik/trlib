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
 


/* s34jtos.h  version 1.0  #          # page 1   22 Feb 1993 12 54 */


/* Copyright (c) 1989 GD,   Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#if !defined     (H_S34JTOS)
#define           H_S34JTOS

extern int                  s34jtos(
/*________________________________*/
double                      y,
double                      x,
double                      *yout,
double                      *xout,
int                         direct,
char                        *usertxt,
FILE                        *tr_error
);


  /*
WARNING: The function is based upon far too few coordinate pairs,
but after the direct demand of the SDJ-division of KMS no improvements
will be introduced.

THE GD OF KMS WILL STRONGLY WARN AGAINST ANY PROFESSIONAL USE OF THE
FUNCTION - EXCEPT FOR FINDING PRELIMINARY COORDINATES FOR AN ADJUSTMENT,
WHICH THUS IS THE ONLY REASON FOR NOT DISCARDING IT.

The x and y notation for the coordinates follow the Prussian cadastral
principles, so that the x-coordinate is north-going. This is the basis
for the unnecessary clumsy swopping of axes in the code, but it seems
to be waste of time to rewrite the code of a too poor function, which is
only kept for finding preliminary coordinates.

s34jtos: int function giving the transformations

          s34 Jylland <-> Ostenfeldts system

The transformation may have errors up to 0.5 m, but the local
consistency is generally much better. The computing precision is
better than 0.5 mm.
s34jtos returns the result of the transformation as follows

      0     =>   all ok
      1<<5  =>   error (tolerance exceed, or parameter error)

y, x input coordinates (s34j or os), double
in units of metres. y and x are called by value.

yout, xout output coordinates in (os or s34j), double
in units of metres. yout and xout are called by reference.

direct, call by value indicating

          direct > 0  => transformation s34j --> os

          direct < 0  => transformation os --> s34j

          direct = 0  => no transformation, exit with error.

tr_cntr      control of alarm actions
      tr_cntr == 1  => activation of an alarm function
      tr_cntr != 1  => no alarm is given, but 1<<5 returned.

example:
if (s34jtos(y, x, &Y, &X, 1, 1))
fprintf(stderr, "\nerror in s34jtos");
produces os coordinates Y,X from the s34j coordinates y,x.
Note the operator & on Y and X.
*/

#endif
