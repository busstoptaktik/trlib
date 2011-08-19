

/* cw from JD or n                 # page 1    1 Feb 2004 11:18 */


/* Copyright (c) 1994, Kort-og Matrikelstyrelsen, GD, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of GD, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not */
/* indicate an intention of publishing this code.             */


extern double    cwfJD(
/*___________________*/
char             mode,
double           JD,
int              zone,
int              region
);

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

