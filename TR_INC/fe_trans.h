


/* fe_trans.h version 2010.1            # page 1   12 jan 2010 13 02 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

#include "geo_lab.h"

extern int          fe_trans(
/*___________________________*/
union geo_lab      *in_lab,
union geo_lab      *outlab,
double              N,
double              E,
double              H,
double             *Nout,
double             *Eout,
double             *Hout,
char               *usertxt,
FILE               *tr_error
);

/* 2. dim. transformation function for area FE */

/*
*in_lab      :  from label
*out_lab     :    to label
N, E, H      :  from coordinates
*Nout, *Eout :    to coordinates
*Hout        :    to height == H
*usertxt     :  to be written in case of errors
*tr_error    :  file for error reports
             :  tr_error == NULL: NO REPORT IS PRINTED

*/
