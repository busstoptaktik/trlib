

/*  ctc.h version 1.0               # page 1   25 Aug 1994 17 13 */


/* Copyright (c) 1990 GD, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of GD, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not */
/* indicate an intention of publishing this code.             */

extern int            ctc(
/*______________________*/
union geo_lab         *i_lab_u,
union geo_lab         *o_lab_u,
double                Xi,
double                Yi,
double                Zi,
double                *Xo,
double                *Yo,
double                *Zo,
char                  *usertxt,
FILE                  *tr_error
);

/* ctc:      Cartesic_Transformation_to_Cartesic            */
/* i_lab_u:  from label                                     */
/* o_lab_u:  to   label                                     */
/* Xi,Yi,Zi: from coordinates                               */
/* Xo,Yo,Zo: to   coordinates                               */
/* tr_cnt:   == 0 causes break on error, othervice continue */
/* usertxt:  to be printed on error                         */
/* tr_error: file for usertxt on error                      */

