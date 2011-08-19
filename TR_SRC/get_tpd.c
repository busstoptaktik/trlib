

/* get_tpd    ver 2010.1          # page 1   12 Jan 2010 13 14 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

#include <string.h>
#include <math.h>
#include "geo_lab.h"
#include "typ_dec.h"


#if !defined(H_NG_TF)
#define      H_NG_TF
struct na_gr_tp_fc  ngtf[] = {
   /*unit  grp typ defdg maxdg convfct internal->out*/
   {"sx",    1,  1, 7, 10, 180.0*60.0*60.0/M_PI},
   {"tm",    1,  2, 6,  9,  12.0*60.0*60.0/M_PI},
   {"N" ,    1,  3, 7, 10, 180.0*60.0*60.0/M_PI},
   {"S" ,    1,  4, 7, 10, 180.0*60.0*60.0/M_PI},
   {"E" ,    1,  5, 7, 10, 180.0*60.0*60.0/M_PI},
   {"W" ,    1,  6, 7, 10, 180.0*60.0*60.0/M_PI},
   {"nt",    1,  8, 5,  8, 180.0*60.0     /M_PI},
   {"mas",   1,  9, 2, 10, 180.0*60.0*60.0*1000.0/M_PI},

   {"dg",    2,  1, 3,  6, 180.0          /M_PI},
   {"asx",   2,  2, 2,  5, 180.0*60.0*60.0/M_PI},
   {"rad",   2,  3, 2,  5, 1.},
   {"yrad",  2,  4, 6,  9, 1.0e+6},
   {"cc",    2,  5, 7, 10, 2.0e+6/M_PI},
   {"gr",    2,  6, 3,  6, 2.0e+2/M_PI},
   {"gon",   2,  7, 3,  6, 2.0e+2/M_PI},
   {"mgon",  2,  8, 3,  6, 2.0e+5/M_PI},
   {"ts",    2,  9, 4,  7, 3200.0/M_PI},
   {"prad",  2, 10, 6,  9, 1.0e+9},

   {"m ",    4,  1, 7, 15, 1.},
   {"m",     4,  1, 7, 15, 1.},
   {"my",    4,  2, 3, 15, 1.0e+6},
   {"mm",    4,  3, 3, 15, 1.0e+3},
   {"cm",    4,  4, 3, 15, 1.0e+2},
   {"dm",    4,  5, 3, 15, 1.0e+1},
   {"km",    4,  6, 4, 15, 1.0e-3},
   {"nm",    4,  7, 4, 15, 1.0/1852.0},

   {"aum",   5,  1,14, 14, 149597870691.0},
   {"aus",   5,  1,14, 14, 499.0047838061},
   {"aly",   5,  1,14, 14, 9460730472580800.0},

   {"gpu",   8,  1, 4, 15, 1.},
   {"mpu",   8,  2, 4, 15, 1.0e+3},
   {"ypu",   8,  3, 4, 15, 1.0e+6},

   {"gal",  10,  1, 4, 15, 1.},
   {"mgal", 10,  2, 4, 15, 1.0e+3},
   {"ygal", 10,  3, 4, 15, 1.0e+6},
   {"kgal", 10,  4, 4, 15, 1.0e-3},

   {"ppm",  12,  1, 4, 15, 1.0e+6},
   {"ppb",  12,  2, 4, 15, 1.0e+9},
   {"ne",   12,  3, 4, 15, 31622.77660168379331995},
    /*                     1000 * sqrt(1000)   */

   {"",      0,  0, 0,  0, 0.0}  /* stop record */
};
#endif


int                    get_tpd(
/*___________________________*/
struct typ_dec        *gtfd,
int                   *tp,
char                  *name,
int                   *cfp,
int                   *dec,
double                *cf
)

{

  struct na_gr_tp_fc         *p;

  for (p = ngtf; p->gr; p++)
    if ((gtfd->gf == p->gr) && (gtfd->tf == p->tp)) {
      *tp = gtfd->tf;
      (void) strcpy(name, p->na);
      *cfp = gtfd->ff;
      if (*cfp == 0) *cfp = p->dd;
      *dec = gtfd->df;
      *cf  = p->fc;
      return(p->gr);
    }
  return(0);
}

