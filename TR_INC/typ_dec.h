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
 

/* typ_dec.h  version 1.1           # page 1   22 Feb 1994 13 13 */


/* Copyright (c) 1992 GS, U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */



#if !defined(H_TYP_DEC)

/*   Defined in geo_lab.h
struct typ_dec {
  unsigned char   gf;  // group field
  unsigned char   tf;  // type field
  unsigned char   ff;  // minimal chars before point
  unsigned char   df;  // decimals after point
};
// SPECIAL:: df == 255 will shift to highest possible unit having no decimals
// for 1) sx, nt, dg, 2) cc, gr,  3) mm, cm, dm, m, km
// zero decimal will be written in any case!
// e.g. 9 00 sx will be written: 9nt and 9 00 00 sx be written 9dg
// e.g. 9 00.1 sx will be written: 9nt and 9 00.5 sx be written 9 01sx
 
*/

/*
typ_dec.h contains the source code of declarations used by 
the functions set_tpd and get_tpd.

The information is a structure array containing

1.  the name of the unit
2.  the group of the unit
3.  the type of the unit
4.  the conversion factor of the unit

The name is a letter string of 1-4 characters identifying
the unit.

The groups are:

1. sexagesimal angular values
type 1: degrees, minutes and (arc)seconds               (sx)
type 2: hours, minutes and (time)seconds                (tm)
type 3-6: as type 1, but N,S,E,W are available      (N) (S)
          for convenience, S and W means negative   (E) (W)
          values.
type 7: not used at present
type 8: degrees and minutes                             (nt)
type 9: mili(degrees, minutes and (arc)seconds)         (mas)

2. decimal angular values 
type 1: nonagesimal angles (old-degrees with decimals)  (dg)
type 2: arcseconds                                      (asx)
type 3: radians                                         (rad)
type 4: microradians  (1.0e-6 rad)                      (yrad)
type 5: centesimal degrees, minutes and seconds         (cc)
type 6: gon                                             (gr)
type 7: gon                                             (gon)
type 8: mgon                                            (mgon)
type 9: ts (="thousands") 4000 ts is 2 pi               (ts)
type 10: picoradians  (1.0e-9 rad)                      (prad)

3. not used
*/


/* typ_dec.h  version 1.1           # page 2   22 Feb 1994 13 13 */


/*

4. lengths units
type 1: metres                                       (m)
type 2: microns                                      (my)
type 3: millimetres                                  (mm)
type 4: centimetres                                  (cm)
type 5: decimetres                                   (dm)
type 6: kilometres                                   (km)
type 7: nautical miles 1 nm = 1852.0 m               (nm)

5. astronomical units
type 1: astronomical unit in meters                  (aum)
              149 597 870 691 m
type 2: astronomical unit in seconds                 (aus)
                          499.004 783 806 1 sec 
type 3: lysår                                        (aly)
        9 460 730 472 580 800 m

6. - 7. not used

8. potential units
type 1: gpu, geopotential units (appr. 0.98 m)       (gpu)
type 2: milli-gpu                                    (mgpu)
type 3: micro-gpu                                    (ygpu)

9. not used

10. gravity units
type 1: gal    (cm/sec2)                             (gal)
type 2: milligal                                     (mgal)
type 3: microgal                                     (ygal)
type 4: kilogal                                      (kgal)

11. not used

12. mean error units
type 1: ppm   (parts pr million)                     (ppm)
type 2: ppb   (parts pr billion)                     (ppb)
type 3: ne    (mm pr sqrt(km))                       (ne)

13. - 255. not used

Group no. 0 is used as a signal for undefined.

The conversion factor is used to give a unique 
internal representation:

group 1 and 2: radians
group 4:       metres
group 8:       geopotential units
group 10:      gal
group 12:      units of 1.0

The conversion factor is a multiplicator when the
conversion is to the unique interenal representation,
and a divisor when internal units are converted to
the external representation.

It is thus quite feasible to take input in one unit
and give output in any other sensible unit.

*/

#endif


/* typ_dec.h  version 1.1           # page 3   22 Feb 1994 13 13 */


#if !defined(H_GR_TP_FC)
#define      H_GR_TP_FC

#include    <math.h>

#ifndef    M_PI
#include   "kms_math.h"
#endif

struct na_gr_tp_fc {
  char        na[5];
  unsigned char     gr; /* group */
  unsigned char     tp; /* type  */
  short             dd; /* default digits bef. dec.-point */
  short             md; /* max no. digits bef. dec.-point */
  double            fc;  /* conv.  factor from int to ext */
};


#endif

