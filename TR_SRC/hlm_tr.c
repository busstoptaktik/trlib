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
 


/* hlm_tr                         # page 1   03 Mar 1995 13 29 */


/* Copyright (c) 1995, GD, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include     <stdio.h>
#include     <math.h>
#include     "geo_lab.h"

/*
Transformation items
____________________
N0_in =  f_t[0]
E0_in =  f_t[1]
N0out =  f_t[2]
E0out =  f_t[3]
a     =  f_t[4]
b     =  f_t[5]
D     =  a*a + b*b
A     =  a/D
B     = -b/D

forward:
Nout  =  N0out + a*(N_in - N0_in) - b*(E_in - E0_in)
Eout  =  E0out + b*(N_in - N0_in) + a*(E_in - E0_in)

backward:
Nout  =  N0_in + A*(N_in - N0out) - B*(E_in - E0out)
Eout  =  E0_in + B*(N_in - N0out) - A*(E_in - E0out)

forward check:
N  =  N0_in + A*(Nout - N0out) - B*(Eout - E0out)
E  =  E0_in + B*(Nout - N0out) - A*(Eout - E0out)

backward check:
N  =  N0out + a*(Nout - N0_in) - b*(Eout - E0_in)
E  =  E0out + b*(Nout - N0_in) + a*(Eout - E0_in)
*/

int               hlm_tr(
/*_____________________*/
double         *f_t,
int             direct,
double          N_in,
double          E_in,
double         *Nout,
double         *Eout,
char           *usertxt,
FILE           *tr_error
)
{


/* hlm_tr                         # page 2   03 Mar 1995 13 29 */


#include         "t_status.h"

  double              N, E, n, e;
  double              a, b, A, B, D;
  double              tol = 3.0e-5;
  int                 i, s, h, d, res;

  /* check values */
  N =  N_in;
  E =  E_in;

  /* common params */
  a =  *(f_t + 4);
  b =  *(f_t + 5);
  D =  a*a + b*b;
  A =  a/D;
  B = -b/D;

  d = ((direct > 0) ? (1) : (direct < 0 ? (-1) : 0));
  if (d) {
    s = d;
    h = s + d;

    for (i = -d; i != h; i +=s)
      switch(i) {
      case -1: /*forward*/
        n = N - *(f_t+0);
        e = E - *(f_t+1);
        N = *(f_t+2) + n*a - e*b;
        E = *(f_t+3) + n*b + e*a;
        break;

      case 0:
        *Nout = N;
        *Eout = E;
        break;

      case +1: /*backward*/
        n = N - *(f_t+2);
        e = E - *(f_t+3);
        N = *(f_t+0) + n*A - e*B;
        E = *(f_t+1) + n*B + e*A;
        break;
      }

    /* tol-check of results*/

    if (fabs(N - N_in) < tol && fabs (E - E_in) < tol) return (0);
    else res = TRF_TOLLE_;
  }
  else res = TRF_PROGR_;

  return(t_status(tr_error, usertxt, "hlm_tr", res,
                  "m ", "", N, E, N - N_in, E - E_in));

}

