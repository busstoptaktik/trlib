


/* dk_trans.h version 1.0            # page 1   08 Mar 1995 13 02 */


/* Copyright (c) 1989 U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */




extern int          dk_trans(
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

/* 2. dim. transformation function for area DK */

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
