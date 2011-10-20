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
 


/* cw to JD, n and GMT              # page 1    1 Mar 1993 11:18 */


/* Copyright (c) 1994, Kort-og Matrikelstyrelsen, GD, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of GD, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not */
/* indicate an intention of publishing this code.             */

#include <string.h>
#include "geo_lab.h"


double           cwtJD(
/*___________________*/
double           cw,
double          *n,
double          *GMT,
int              zone,
int              region
)

/*

cwtJD        Julian Date converted from local time in cw
             in timezone +1 is corrected for danish summertime

cw           G.I. date_time
             199204170945.0  == 1992 Apr 17,  9.45

*n           days from JD 2000.0

*GMT         fraction of day from 0 hour UT.

zone         timezone  :
_               0      : UK
_              +1      : DK

region       :  0      : no correction for summertime
_            : else    : include correction for summertime
_                        Programmed for :
_            :  DK     : Denmark.

*/



/* cw to JD, n and GMT              # page 2    1 Mar 1993 11:18 */


{

#define  YE_BASE      20

#define SUM_BASE    1980
  static double  summer[] = {
  /* 1980 04 06 - 09 28 : 03 */  04060300.0, 09280300.0,
  /* 1981 03 29 - 09 27 : 03 */  03290300.0, 09270300.0,
  /* 1982 03 28 - 09 26 : 03 */  03280300.0, 09260300.0,
  /* 1983 03 27 - 09 25 : 03 */  03270300.0, 09250300.0,
  /* 1984 03 25 - 09 30 : 03 */  03250300.0, 09300300.0,
  /* 1985 03 31 - 09 29 : 03 */  03310300.0, 09290300.0,
  /* 1986 03 30 - 09 28 : 03 */  03300300.0, 09280300.0,
  /* 1987 03 29 - 09 27 : 03 */  03290300.0, 09270300.0,
  /* 1988 03 27 - 09 25 : 03 */  03270300.0, 09250300.0,
  /* 1989 03 26 - 09 24 : 03 */  03260300.0, 09240300.0,
  /* 1990 03 25 - 09 30 : 03 */  03250300.0, 09300300.0,
  /* 1991 03 31 - 09 29 : 03 */  03310300.0, 09290300.0,
  /* 1992 03 29 - 09 27 : 03 */  03290300.0, 09270300.0,
  /* 1993 03 28 - 09 26 : 03 */  03280300.0, 09260300.0,
  /* 1994 03 27 - 09 25 : 03 */  03270300.0, 09250300.0,
  /* 1995 03 26 - 09 24 : 03 */  03260300.0, 09240300.0,
  /* 1996 03 31 - 10 27 : 03 */  03310300.0, 10270300.0,
             0.0};
#define   SUM_LAST    (1996 - SUM_BASE)*2

  int            year, ye, mo, da, ho, mi;
  int            days;
  double         JD, loc_t;
  union rgn_un   rgn_DK;

  /* check for proper values */
  if (170003010000.0 < cw && cw < 300000000000.0) {

    year  = (int)      (cw  / 100000000);
    cw   -= ((double) year) * 100000000;

    (void) strcpy(rgn_DK.prfx, "DK");
    if (region == rgn_DK.r_nr[0]) {
      ye     = (year - SUM_BASE) * 2;
      region = 0 <= ye && ye <= SUM_LAST && 
               summer[ye] < cw && cw < summer[ye+1];
    } else region = 0;

    /* spilt cw */
    mo    = (int)      (cw  /   1000000);
    cw   -= ((double)   mo) *   1000000;
    da    = (int)      (cw  /     10000);
    cw   -= ((double)   da) *     10000;
    ho    = (int)      (cw  /       100);
    cw   -= ((double)   ho) *       100;
    mi    = (int)       cw;

    /* reset month to Mar = 0, adjust month and year */
    mo   -= 3;
    if (mo < 0) {
      year--;
      mo += 12;
    }
    days  = (int) ((year % 100) * 365.25); /* days within a century */
    ye    = (int) ( year / 100 - YE_BASE);
    days +=  ye * 36524;           /* entire centuries */
    days += (ye + 4) / 4;    /* addition leap days per 400 years */

    mo    = (int) (mo * 30.61 + 0.5);     /* months -> days */

    /* sum of all days (adjust for jan and febr) */
    days  =  days + mo + 59 + da;
    loc_t =  (60.0 * ho + mi) / 1440.0;
    *n    =  days -1.5 + loc_t;

    if (region) {
      loc_t -= 1.0 / 24.0;
      *n    -= 1.0 / 24.0;
    }

    /* timezone  shift */
    *GMT  = (-1.0 * zone) / 24.0 + loc_t;
    *n    = days -1.5 + *GMT;
    JD    = *n + 2451545.0;

  } else {
    *GMT = -10.0;
    *n   = -1.0e9;
    JD   = -1.0e9;
  }

  return(JD);

#undef  YE_BASE
#undef SUM_BASE
#undef SUM_LAST
}


