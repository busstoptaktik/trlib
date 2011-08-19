

/* bh_lmb.c                           # page 1   22 Feb 1993 12 54 */


/* Copyright (c) 1991 GS, U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include <math.h>

double           bh_lmb(
/*____________________*/
double     K1,
double     f,
double     bm,
double     s1,
double     s)


{
#include         "v_red.h"
  double            h, sr;

  s1 = v_red(2.0*s1 + s);
  sr = v_red(s);
  h  = (1.0/(1.0 - f/2.0) - K1*(1.0 + K1/2.0)/2.0)*s
      - K1*sin(sr)*(cos(s1)
      - K1*cos(v_red(2.0*s1))*cos(sr)/2.0)/2.0;

  return(f*(2.0 - f)*cos(bm)*h/2.0);
}


