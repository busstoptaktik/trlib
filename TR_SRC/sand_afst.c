

/* sand_afst version                    # page 1   2 Feb 2009 12 54 */


/* Copyright (c) 2009, National Space Institute, DTU, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of DTU, Denmark */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */

#include <math.h>
#include <string.h>
#include <stdio.h>


double         sand_afst(
/*_____________________*/
char          *datum,
double         afst,
double         E_middel
)

{
char           sys;
double         scale, E0;

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

  E_middel -= E0;
  scale    *= 1.0 + 1.226938801E-14 * E_middel * E_middel;

  return(afst / scale);
}


