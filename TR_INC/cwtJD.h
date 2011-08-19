

/* cw to JD, n and GMT              # page 1    1 Mar 1993 11:18 */


/* Copyright (c) 1994, Kort-og Matrikelstyrelsen, GD, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of GD, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not */
/* indicate an intention of publishing this code.             */


extern double    cwtJD(
/*___________________*/
double           cw,
double          *n,
double          *GMT,
int              zone,
int              region
);

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

