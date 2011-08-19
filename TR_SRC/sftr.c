

/* sftr.c                        # page 1   22 Feb 1993 12 54 */


/* Copyright (c) 1989 U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include <math.h>

double                   sftr(
/*__________________________*/
double            C,
double            a,
double            b,
double            *A,
double            *B
)

{
#include          "v_red.h"

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


