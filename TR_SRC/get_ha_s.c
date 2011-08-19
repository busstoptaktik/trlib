

/* get_ha_s.c                        # page 1   28 Apr 1993 11:00 */


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

double      get_ha_s(
FILE                *fw,
FILE                *fw1,
double               a,
double               f,
int                  j,
int                 *i,
double               s_max)
{
#include           "bshlm1.h"
#include           "gmb1.h"
#include           "bshlm2g.h"
#include           "v_red.h"
  double            A0, A1, S, a0m, circum = 0.0;
  int               res, k;
  struct ne        {
    double            n;
    double            e;
  }                 NE[3], *Z = NE, *Zm = NE+1, *Z1 = NE+2;
 
  if (fseek(fw, 0L, SEEK_SET) ||
     (fread((char *) Z,  sizeof(*Z), 1, fw) - 1)) {
    (void) fprintf(stdout, "\n***problemer med work(fw) read pos0;");
    return(-1.0);
  }
  if (fseek(fw1, 0L, SEEK_SET) ||
     (fwrite((char *) Z,  sizeof(*Z), 1, fw1) - 1)) {
    (void) fprintf(stdout, "\n***problemer med work(fw1) write pos0;");
    return(-1.0);
  }

  for (k = 1; k <= j; k++) {
    if (fread((char *) Z1, sizeof(*Z), 1, fw) - 1) {
      (void) fprintf(stdout,
             "\n***problemer med work(fw) read pos %d;", k);
      return(-1.0);
    }
    res = bshlm2g(a, f, Z->n, Z1->n, Z->e, Z1->e, &A0, &A1, &S);
    A1  = v_red(A1 + M_PI);

    if (S > s_max) {

      if (   fabs(Z->n)  < 84.5/180.0*M_PI 
          && fabs(Z1->n) < 84.5/180.0*M_PI
          && S < 0.011*a) {
        res |= gmb1(a, f, Z->n, &Zm->n, Z->e, &Zm->e, A0, &a0m, S*0.5);
      } else {
        res |=
             bshlm1(a, f, Z->n, &Zm->n, Z->e, &Zm->e, A0, &a0m, S*0.5);
      }
      if (fwrite((char *) Zm, sizeof(*Z), 2, fw1) - 2) {
        (void) fprintf(stdout,
               "\n***problemer med work(fw1) write;");
        return(-1.0);
      }
      ++ *i;
    } else {
      if (fwrite((char *) Z1, sizeof(*Z), 1, fw1) - 1) {
        (void) fprintf(stdout,
               "\n***problemer med work(fw1) write;");
        return(-1.0);
      }
    }
    circum += S;
    *Z      = *Z1;
  }
  return(circum);
}

