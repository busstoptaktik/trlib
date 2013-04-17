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

/* Copyright (c) 1989 U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include    <math.h>
#include    "KmsFncs.h"

double                   sftr(
/*__________________________*/
double            C,
double            a,
double            b,
double            *A,
double            *B
)
{

  double  svscs, svdss, cvscs, cvdss;
  double  cosC, sinC, sum, dif, c;
  double  n, t;

  a  /= 2.0;
  b  /= 2.0;
  C  /= 2.0;

  sum = a + b;
  dif = a - b;

  cosC = cos(C);
  sinC = sin(C);

  svscs = cosC*cos(dif);
  svdss = cosC*sin(dif);
  cvscs = sinC*cos(sum);
  cvdss = sinC*sin(sum);

  sum = atan2(svscs, cvscs);
  dif = atan2(svdss, cvdss);

  *A = v_red(sum + dif);
  *B = v_red(sum - dif);

#ifdef  WIN32
  t = _hypot(svdss, cvdss);
  n = _hypot(svscs, cvscs);
#else
  t = hypot(svdss, cvdss);
  n = hypot(svscs, cvscs);
#endif
  c = v_red(2.0*atan2(t, n));

  return(c);

}
