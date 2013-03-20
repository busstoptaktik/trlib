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
 
#include   <stdio.h>
#include   <math.h>

#include    "KmsFncs.h"

double      get_ha_s(
FILE                *fw,
FILE                *fw1,
double               a,
double               f,
int                  j,
int                 *i,
double               s_max)
{

  double            A0, A1, S, a0m, circum = 0.0;
  int               res, k;
  struct ne        {
    double            n;
    double            e;
  }                 NE[3], *Z = NE, *Zm = NE+1, *Z1 = NE+2;
 
  if (fseek(fw, 0L, SEEK_SET) ||
     (fread((char *) Z,  sizeof(*Z), 1, fw) - 1)) {
	    (void) lord_warning(0, LORD("problemer med work(fw) read pos0;"));
    return(-1.0);
  }
  if (fseek(fw1, 0L, SEEK_SET) ||
     (fwrite((char *) Z,  sizeof(*Z), 1, fw1) - 1)) {
	    (void) lord_warning(0, LORD("problemer med work(fw1) write pos0;"));
    return(-1.0);
  }

  for (k = 1; k <= j; k++) {
    if (fread((char *) Z1, sizeof(*Z), 1, fw) - 1) {
	    (void) lord_warning(0, LORD("problemer med work(fw) read pos %d;"), k);
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
        (void) lord_warning(0, LORD("problemer med work(fw1) write;"));
        return(-1.0);
      }
      ++ *i;
    } else {
      if (fwrite((char *) Z1, sizeof(*Z), 1, fw1) - 1) {
            (void) lord_warning(0, LORD("problemer med work(fw1) write;"));
        return(-1.0);
      }
    }
    circum += S;
    *Z      = *Z1;
  }
  return(circum);
}

