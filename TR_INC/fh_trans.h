

/* fh_trans  ver 2010.1        # page 1    12 Jan 2010 13 55 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */



extern int          fh_trans(
/*___________________________*/
union geo_lab       *in_lab_u,
union geo_lab       *outlab_u,
double              N,
double              E,
double              H,
double              *Nout,
double              *Eout,
double              *Hout,
char                *usertxt,
FILE                *tr_error
);

/* Fehmarn Bridge Projection test of AREA */
/* and transf to/from geo                 */

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

