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

#include              <math.h>

#include              "geo_lab.h"

int                   gen_pol(
/*__________________________*/
double               *C_f,
double               *C_r,
double                N_in,
double                E_in,
double               *Nout,
double               *Eout
)

{

#define            FLOATLIMIT      5.0e5

  double             N, E, n, e;
  double            *Cp, *tcy, *tcx, u, v;
  double             tol = 1.0e-4;
  int                i;
  int                g;
  int                sz;
  int                r, c;
  int                res = 0;

  /* Preserve input for reverse check */
  N  = N_in;
  E  = E_in;
  Cp = C_f;


/* gen_pol                        # page 2   10 Sep 1998 12 02 */


  /* Transformation loop */
  for (i = -1; i <= 1 && res == 0; ++i)
    if (i) {

      /* Prepare for double Horner */
      g   = (int) *Cp;
      sz  = (g + 1)*(g + 2)/2 + 1;
      tcy = Cp + sz;
      tcx = tcy  + sz;
      /* Double Horner's scheme */
      /* N = n*Cy*e -> yout, E = e*Cx*n -> xout */
      n   = N - *tcy;
      e   = E - *tcx;
      if ((fabs(n) < FLOATLIMIT) && (fabs(e) < FLOATLIMIT)) {

        for (  r = g, N = *--tcy, E = *--tcx; r >  0; r--) {
          for (c = g, u = *--tcy, v = *--tcx; c >= r; c--) {
            u = n*u + *--tcy;
            v = e*v + *--tcx;
          }
          N = e*N + u;
          E = n*E + v;
        }
      } else res = TRF_AREA_;
    }
    else { /* collect output coord,switch to reverse checking */
      *Nout = N;
      *Eout = E;
      Cp    = C_r;
    }

  /* tol-check of results*/
  if (res == 0 && (fabs(N - N_in) < tol && fabs(E - E_in) < tol))
      return (0);
  else if (res == 0) res = TRF_AREA_;
  return(res);

#undef   FLOATLIMIT

}


