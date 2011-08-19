

/* get_cent.c                        # page 1   28 Apr 1993 11:00 */


/* Copyright (c) 1993, GD, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include   <stdio.h>
#include   <math.h>

#ifndef    M_PI
#include   "kms_math.h"
#endif

int         get_cent(
FILE                 *fw,
double                a,
double                f,
double                B0,
double                L0,
double                B1,
double                L1,
double                Smax,
int                  *i)
{
#include           "bshlm1.h"
#include           "gmb1.h"
#include           "bshlm2g.h"
  double            A0, A1, S;
  double            Bm, Lm, a0m;
  int               res;
  struct ne        {
    double            n;
    double            e;
  }                 NE, *Z = &NE;

  if (fabs(B0) > M_PI_2 - 1.0e-8) L0 = L1;
  else
  if (fabs(B1) > M_PI_2 - 1.0e-8) L1 = L0;
  res = bshlm2g(a, f, B0, B1, L0, L1, &A0, &A1, &S);

  if (S > Smax) {
    if (   fabs(B0) < 84.5/180.0*M_PI 
        && fabs(B1) < 84.5/180.0*M_PI
        && S < 0.0155*a) {
      res |= gmb1(  a, f, B0, &Bm, L0, &Lm, A0, &a0m, S/2.0);
    }
    else {
      res |= bshlm1(a, f, B0, &Bm, L0, &Lm, A0, &a0m, S/2.0);
    }
    res |= get_cent(fw, a, f, B0, L0, Bm, Lm, Smax, i);
    res |= get_cent(fw, a, f, Bm, Lm, B1, L1, Smax, i);
  }
  else {
    (*i)++;
    Z->n = B1;
    Z->e = L1;
    if (fwrite((char *) Z, sizeof(NE), 1, fw) - 1) {
      (void) fprintf(stdout, "\n***problemer med workarea;");
      return(1);
    }
  }
  return(res);
}

