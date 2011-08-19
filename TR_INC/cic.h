

/* cic.h                            # page 1   24 Aug 1994 17 49 */


/* Copyright (c) 1994, GD, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

extern int            cic(
/*______________________*/
struct dsh_str        *shp,  
int                   direct,
double                Xi,
double                Yi,
double                Zi,
double                *Xo,
double                *Yo,
double                *Zo,
char                  *usertxt,
FILE                  *tr_error
);

/* cic:      Cartesic_Transformation_to_Cartesic            */
/* *shp:     datum shift parameters                         */
/* direct>0: the direct formula is used                     */
/* direct<0: the inverse formula is used                    */
/* Xi,Yi,Zi: from coordinates                               */
/* Xo,Yo,Zo: to   coordinates                               */
/* usertxt:  to be printed on error                         */
/* tr_error: file for usertxt on error                      */


