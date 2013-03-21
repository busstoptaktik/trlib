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
 
#include   <math.h>

double           cwfJD(
/*___________________*/
char             mode,
double           JD,
int              zone,
int              region
)

/*

cwfJD        Julian Date (n) converted to local time in cw
             in timezone +1 is corrected for danish summertime
             G.I. date_time
             199204170945.0  == 1992 Apr 17,  9.45

mode == 'n': n = JD counted from year 2000.0 (=JD= 2451545.0)
mode == 'J': JD

JD           JD according to mode true/(from 2000.0)

zone         timezone  :
_               0      : UK
_              +1      : DK

region       :  0      : no correction for summertime
_            : else    : include correction for summertime
_                        Programmed for :
_            :  DK     : Denmark.

REF.: Fliegel, H.F. and van Flandern, T.C. (1968).
      Communications of the ACM, Vol. 11, No. 10 (October, 1968).

Progr.: K.Engsager, Feb. 2004.
*/

{

#define  YE_BASE      20

#define SUM_BASE    1980
  static double  summer[] = {
  /* 1980 04 06 - 09 28 : 03 */  -7171.375, -7034.375,
  /* 1981 03 29 - 09 27 : 03 */  -6852.375, -6670.375,
  /* 1982 03 28 - 09 26 : 03 */  -6488.375, -6306.375,
  /* 1983 03 27 - 09 25 : 03 */  -6124.375, -5942.375,
  /* 1984 03 25 - 09 30 : 03 */  -5760.375, -5571.375,
  /* 1985 03 31 - 09 29 : 03 */  -5389.375, -5207.375,
  /* 1986 03 30 - 09 28 : 03 */  -5025.375, -4843.375,
  /* 1987 03 29 - 09 27 : 03 */  -4661.375, -4479.375,
  /* 1988 03 27 - 09 25 : 03 */  -4297.375, -4115.375,
  /* 1989 03 26 - 09 24 : 03 */  -3933.375, -3751.375,
  /* 1990 03 25 - 09 30 : 03 */  -3569.375, -3380.375,
  /* 1991 03 31 - 09 29 : 03 */  -3198.375, -3016.375,
  /* 1992 03 29 - 09 27 : 03 */  -2834.375, -2652.375,
  /* 1993 03 28 - 09 26 : 03 */  -2470.375, -2288.375,
  /* 1994 03 27 - 09 25 : 03 */  -2106.375, -1924.375,
  /* 1995 03 26 - 09 24 : 03 */  -1742.375, -1560.375,
  /* 1996 03 31 - 10 27 : 03 */  -1371.375, -1161.375,
             0.0, -1.0};
#define SUM_LAST    (1996 - SUM_BASE)*2

  int            days, ye4, ye, mo, da, ho, mi;
  double         loc_t;

  /* NO check for proper values */
  {
    if (mode == 'J') JD -= 2451545.0;

    if (region == 17483) {
      for (mo = 0, region = 0; mo <= SUM_LAST; mo += 2)
      if (summer[mo] < JD && JD < summer[mo+1]) region = 1;
    } else region = 0;

    days  = (int) floor(JD + 0.5);
    loc_t = JD +0.5 - (double) days;

    /* timezone  shift */
    loc_t+= ((double) zone) / 24.0;

    if (region) {
      loc_t += 1.0 / 24.0;
      JD    += 1.0 / 24.0;
    }
    days  = (int) (JD + 0.5 - loc_t);

    mi    = (int) floor(loc_t * 1440.0 + 0.5);
    ho    = mi / 60;
    mi   %=      60;
    if (ho >= 24) {
      ++ days;
      ho -= 24;
    }

    days += 2451545;               /* n from 2000.0 to JD */

    days += 68569;                   /* change base */

    ye4   = (4 * days) / 146097;     /* entire   400 years */
    days -= (146097 * ye4 + 3) / 4;  /* days mod 400 years */

    ye    = (4000 * (days+1)) / 1461001; /* entire   years */
    days -= (1461 * ye) / 4 - 31;    /* days mod     year  */

    mo    = (80 * days) / 2447;      /* month mod    year  */
    da    = days - (2447 * mo) / 80; /* days  mod    month */

    days  = mo / 11;                 /* add for jan, feb */
    mo   += 2 - 12 * days;           /* true month       */

    ye    = (ye4 - 49) * 100 + ye + days;

    loc_t = 100000000.0 * ye + 1000000.0 * mo + 10000.0 * da
          +       100.0 * ho +             mi;

  }

  return(loc_t);

#undef  YE_BASE
#undef SUM_BASE
#undef SUM_LAST
}
