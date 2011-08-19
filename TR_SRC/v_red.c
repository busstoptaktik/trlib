

/* v_red.c version                    # page 1   22 Feb 1993 12 54 */


/* Copyright (c) 1989 U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include <math.h>

#ifndef    M_PI
#include   "kms_math.h"
#endif

double             v_red(
/*_____________________*/
double             v
)

{

  if      (v > 0.0) {
    while (v >  M_PI)  v -= (2.0*M_PI);
  }
  else if (v < 0.0) {
      while (v < -M_PI)  v += (2.0*M_PI);
  }
  return(v);
}


