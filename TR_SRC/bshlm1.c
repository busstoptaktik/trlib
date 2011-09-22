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
 


/* bshlm1.c                           # page 1    1 Mar 1993 11:18 */


/* Copyright (c) 1991 U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include  <math.h>
#include  "geo_lab.h"

#ifndef    M_PI
#include   "kms_math.h"
#endif

int                      bshlm1(
/*____________________________*/
double                   a,
double                   f,
double                   B1,
double                  *B2,
double                   L1,
double                  *L2,
double                   A1,
double                  *A2,
double                   S
)

{

#include             "bh_lat.h"
#include             "bh_lmb.h"
#include             "clenshaw.h"
#include             "gmb1.h"
#include             "sftr.h"
#include             "v_red.h"

  double     em2, b0, PF;
  double     S1, a2, R, I;
  double     b1, b2, dl, d_lmb, b_max, s, s1, K1, K1sq;
  double     ets[4], ste[4];
  int        res;

  if (S < 0.008*a && fabs(B1) < 86.0/180.0*M_PI) {
    res = gmb1(a, f, B1, &b2, L1, &dl , A1, &a2, S);
    if (!res) {
      *B2 = b2;
      *L2 = dl;
      *A2 = a2;
      return(res);
    }
  }

  if (f > 1.0) f = 1.0/f;
  b0       = a*(1.0 - f);
  em2      = f*(2.0 - f)/(1.0 - f)/(1.0 - f);

  b1       = RD_LAT(f, B1);  /* reducet latitude */
  b_max    = bh_lat(b1, A1, em2, &K1, &s1);

  K1sq     = K1*K1;
  PF       = b0*(1.0 + K1sq/4.0)/(1.0 - K1);

  *(ste+0) =  (1.0 - 3.0*K1sq/8.0)/2.0;
  *(ste+1) = -K1/16.0;
  *(ste+2) =  K1sq/48.0;
  *(ets+0) = -(1.0 - 9.0*K1sq/16.0)/2.0;
  *(ets+1) =  K1*5.0/16.0;
  *(ets+2) = -K1sq*29.0/96.0;

  S1       = s1 + K1*clenshaw('s', ste, 3, 2*s1, 0.0, &R, &I);
  S       /= PF;
  s        = S + K1*(clenshaw('s', ets, 3, 2*(S1+S), 0.0, &R, &I)
           - clenshaw('s', ets, 3, 2*S1, 0.0, &R, &I));
  b2       = M_PI_2 - sftr(A1, M_PI_2 - b1, s, &a2, &dl);
  *B2      = GD_LAT(f, b2);
  d_lmb    = bh_lmb(K1, f, b_max, s1, s);
  *L2      = L1 + dl - d_lmb;
  *A2      = v_red(-a2);

  return(S*PF > a*1e-6 ? 0 : 1);
 }

