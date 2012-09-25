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
 


/* utsb  ver 1999.01              # page 1   12 Jan 1999 12 54 */


/* Copyright (c) 1999, LG,  Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of LG, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include <stdio.h>
#include <math.h>
#include "geo_lab.h"

/* Definition of the permissible area for the transformation */

#define NMIN     6124998.0
#define NMAX     6142002.0
#define EMIN      484998.0
#define EMAX      518002.0

int                      utsb(
/*__________________________*/
double                   N_in,
double                   E_in,
double                   *N_out,
double                   *E_out,
int                      direct,
char                     *usertxt,
FILE                     *tr_error
)

/*
  function and parameters

  utsb               (return)                   int
  Dual autochecking transformation procedure for transformation
  between UTM Zone 32 and system sb. An alarm is produced, when
  the check by the inverse transformation exceeds the tolerance
  of 0.5 mm.

  The values of the function are:

          0       => ok
          -1      => outside the sb area set by the define
                     statements above.

  N_in, E_in             (call)                   double
  The UTM32- or sb-coordinates input in units of metres.

  N_out, E_out             (return)                 double
  The sb- or UTM32-coordinates output from the procedure
  as transformed and checked coordinates in units of metres.

  direct                 (call)                   int
  direct >= +1  => transformation utm32 -> sb
  direct <= -1  => transformation sb -> utm32
  direct ==  0  => no transformation, alarm


  prog: Knud Poder  DEC 1992
  */


/* utsb  ver 1999.01              # page 2   12 Jan 1999 12 54 */


{

#include      "t_status.h"

  double         N, E, n, e, w, tol = 0.005;
  double         *c, *cb = NULL, *cw = NULL;
  int            ok = 0;
  int            i, t;
  int            res = 0;

  static  double    Cu32_sb[] = {
    /* lng*/                   12.0,         /*tol*/ 0.0005,
    /*y0x0*/   6.13258568148837e+06,   6.19480258923588e+05,
    /*  0 */   6.13342118787027e+06,   4.94690181709311e+05,
    /*  1 */   9.99824464710368e-01,   2.82279070814774e-02,
    /*  2 */   7.66123542220864e-11,   1.78425334628927e-09,
    /*  3 */  -1.05584823306400e-14,  -3.32554258683744e-15
  };

  static  double    Csb_u32[] = {
    /* lng*/                   12.0,         /*tol*/ 0.0005,
    /*y0x0*/   6.13342113183056e+06,   4.94690026817276e+05,
    /*  0 */   6.13258562111350e+06,   6.19480105709997e+05,
    /*  1 */   9.99378966275206e-01,  -2.82153291753490e-02,
    /*  2 */  -2.27089979140026e-10,  -1.77019590701470e-09,
    /*  3 */   1.08522286274070e-14,   2.11430298751604e-15,
  };

  if (direct > 0) { /* u32 -> sb */
    cb = Cu32_sb;
    cw = Csb_u32;
  }
  else if (direct < 0) { /* sb -> u32 */
    cb = Csb_u32;
    cw = Cu32_sb;
  }
  else res = TRF_PROGR_;



/* utsb  ver 1999.01              # page 3   12 Jan 1999 12 54 */


  /* Transformation */

  N = N_in;
  E = E_in;

  /* transformation, i == -1 => fwd, i == 1 => bck */
  for (i = -1; i <= 1 && res == 0; ++i)
    if (i) { /* transformation, i == -1 => f, i == 1 => b */
      /* check limit of sb-coord */
      if (direct == i) {
        ok =  (NMIN <= N && N <= NMAX
            && EMIN <= E && E <= EMAX);
      }
      t   = (int) *cb;
      c   = cb + t;
      cb++;
      tol = *cb++;
      n   = N - *cb++;
      e   = E - *cb++;
      if (fabs(n) < 100.0e3 && fabs(e) < 100.0e3) {
        E = *--c;
        N = *--c;
        while (c > cb) {
          w = n*E + e*N + *--c;
          N = n*N - e*E + *--c;
          E = w;
        }
      }
      else {
        res = TRF_AREA_;
        ok  = 0;
      }
    }
    else { /* collect output coord */
      *N_out = N;
      *E_out = E;
      cb     = cw;
    }

  if (ok) {
    /* tolerance check */
    if (res == 0) {
      if ((fabs(N_in - N) < tol) && (fabs(E_in - E) < tol)) return (0);
      else res = TRF_TOLLE_;
    }
  }
  else if (res > TRF_AREA_) res = TRF_AREA_;

  return((tr_error==NULL || res==0) ? res :
          t_status(tr_error, usertxt, "utsb", res,
          "m ", "", N_in, E_in,N_in - N, E_in - E));
}


