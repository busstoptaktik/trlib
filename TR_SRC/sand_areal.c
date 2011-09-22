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
 


/* sand_areal                      # page 1   2 Feb 2009 12 54 */


/* Copyright (c) 2009, National Space Institute, DTU, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of DTU, Denmark */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */

#include <string.h>
#include <stdio.h>
#include <math.h>


double         sand_areal(
/*______________________*/
char          *datum,
double        *pol_base
)

{
char           sys;
int            noc;
double        *p_low, *p_high, *p_h, *p_j;
double         scale, E0, N1, E1, min, max, elling = 0.0;

  sys = (strncmp("dktm", datum, 4) == 0) ? *(datum+4)
    : (strncmp("kp2000", datum, 6) == 0) ? *(datum+6) : -1;

  switch(sys) {
  case '1':
  case '2':
  case '3':
    scale = 0.999980;
    E0    = 200000.0 * ((int) (sys - '0'));
    break;
  case '4':
    scale = 1.000000;
    E0    = 800000.0;
    break;
  case 'j':
    scale = 0.999950;
    E0    = 200000.0;
    break;
  case 's':
    scale = 0.999950;
    E0    = 500000.0;
    break;
  case 'b':
    scale = 1.000000;
    E0    = 900000.0;
    break;
  default:
    if ((!strcmp("etrf89", datum)) ||
       (!strcmp("etrs89", datum)) || !strcmp("euref89", datum)) {
      scale = 0.999600;
      E0    = 500000.0;
    }
    else return(-1.0);
  }

  noc    = (int) *pol_base; 
  noc   *= 2;
  p_low  = pol_base +1;
  p_high = p_low    +noc -2;
  if ((*p_low == *p_high) && *(p_low +1) == *(p_high +1)) {
    noc    -= 2;
    p_high -= 2;
  }
  N1  = *(p_low   ) * 2.0;
  E1  = *(p_low +1);
  min = E1;
  max = E1;
  for (p_h = p_low; p_h <= p_high; p_h += 2) {
    p_j     = p_low +((p_h +2 -p_low) % noc);
    elling += (*(p_h+1) - *(p_j+1)) * (*p_h + *p_j - N1);
    if (*(p_j+1) < min) min = *(p_j+1);
    else
    if (*(p_j+1) > max) max = *(p_j+1);
  }
  elling = fabs(0.5 * elling);
  

  E1      = (min + max) * 0.5 - E0;
  scale  *= 1.0 + 1.226938801E-14 * E1 * E1;
  /*        1.0 - 2.0 * (scale - 1.0) */
  elling *= 3.0 - 2.0 * scale;

  return(elling);
}


