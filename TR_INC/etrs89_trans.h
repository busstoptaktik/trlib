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
 

#if !defined(H_TRS89_TRANS)
#define      H_TRS89_TRANS


/* etrs89_trans  ver 2009.03          # page 1   04 Mar 2009 11 36 */


/* Copyright (c) 2009, National Space Institude, Danish Technical */
/* University and Kort-og Matrikelstyrelsen, Denmark              */
/* All rights reserved.                                           */

/* This is unpublished proprietary source code of National Space  */
/* Institude DTU.  This copyright claim does not indicate an      */
/* intention of publishing this code.                             */


extern int                etrs89_trans(
/*___________________________________*/
char                     *i_lab_str,
char                     *o_lab_str,
double                    N_in,
double                    E_in,
double                   *X,
double                   *Y,
FILE                     *tr_error
);

  /* etrs89_trans is a tiny version of gd_trans having        */
  /* transformations between projections in etrs89 Datum ONLY */
  /* crt and ellipsoidal heights are not included             */
  /* leagal i_lab_str and o_lab_str are                       */
  /* (zz is two digits for zone):                             */
  /*   "geo", "utmzz", "dktm1", "dktm2", "dktm3", "dktm4",    */
  /*   "kp2000j", "kp2000s", "kp2000b", "kfmr", "fke",        */
  /*   "etrs-tmzz", "ETRS-TMzz", "etrs-lcc", "ETRS-LCC",      */
  /*   "etrs-laea", "ETRS-LAEA"                               */

  /* The return values are:
     0           => ok
     TRF_INACC_  => boundary exceeded (inaccurate)
     TRF_TOLLE_  => transformation tollerance exceeded
     TRF_AREA_   => boundary exceeded (serious)
     TRF_ILLEG_  => illegal transformation

     in case of error an error text is returned in err_txt,
     which should be declared with a length bigger than 250

     NB: the values TRF_*_ and TAB_*_ are defined in geo_lab.h in 
     decreasing order. 
     For mapping purposes it may then be admissible to accept 
     the result bigger or equal to TRF_INACC_ (or even TRF_TOLLE_).

     Parameters:
     char *i_lab_str, *o_lab_str : string from the list above
           describing the input and output.
     double N, E: The input coordinates:
       Geographical     latitude, longitude (unit: radians).
       Projection       Northing, Easting (unit: meter).
     double *X, *Y: The input coordinates:
       Geographical     latitude, longitude (unit: radians).
       Projection       Northing, Easting (unit: meter).

  Prog: Karsten Engsager 2009.
  */

  #endif

