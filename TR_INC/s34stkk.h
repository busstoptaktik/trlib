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
 


/*  s34stkk.h version 1.0             # page 1   22 Feb 1993 12 54 */


/* Copyright (c) 1989 GD,   Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

extern int                 s34stkk(
/*_______________________________*/
double                     y,
double                     x,
double                     *yout,
double                     *xout,
int                        direct,
char                       *usertxt,
FILE                       *tr_error
);

/*
s34stkk: int function giving the transformations

          s34 Sjlland <-> Kbh. kommunes system

The transformation may have errors up to 0.5 m, but the local
consistency is generally much better. The computing precision is
better than 0.5 mm.
s34stkk returns

       0 => ok
       != 0 in case of error.

y, x input coordinates (s34j or kk), double,
in units of metres. y and x are called by value.

yout, xout output coordinates in (kk or s34j), double
in units of metres. yout and xout are called by reference.

direct, call by value indicating

          direct > 0  => transformation s34j --> kk

          direct < 0  => transformation kk --> s34j

          direct = 0  => no transformation,
          i. e. yout = y and xout = x (no alarm occurs)

tr_cntr      (call)        int
Control of error actions:
      tr_cntr == 1   => alarm output to tr_error
      tr_cntr == 0   => alarm output and return of -1.

usertxt      (call)        *char
A text string made by the user and copied into the alaem output.

tr_error     (call)        *FILE
A stream opened by the calling programme. The error text from t_report
is copied intp the stream.

example:
(void) s34stkk(y, x, &Y, &X, 1, 1, usertxt, tr_error);
printf("\n Y = %14.6f X = %14.6f", Y, X);
produces KK coordinates Y,X from the s34j coordinates y,x.
Note the operator & on Y and X.

  prog: Olaf Andersson jan 1979
  modification (s34s->kk): WLW, Januar 1989
  C-version  KP 1989

*/
