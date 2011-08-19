

/* ng_trans  ver 2006          # page 1    3 Oct 2006 13 55 */


/* Copyright (c) 2006 Kort-og Matrikelstyrelsen, Denmark   */
/* and Danish National Space Center: All rights reserved.  */

/* This is unpublished proprietary source code of Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does  */
/* not indicate an intention of publishing this code.      */

/*
#define  DEBUGFETRANS
*/

extern int           ng_trans(
/*___________________________*/
union geo_lab       *in_lab_u,
union geo_lab       *outlab_u,
double               N,
double               E,
double               H,
double              *Nout,
double              *Eout,
double              *Hout,
char                *usertxt,
FILE                *tr_error
);

/* 3D transformation function for NAD83G to/from GR96 */
/* NAD83G is GREENLAND and is  NOT  equal to NAD83 */

/*
*in_lab             :  from label
*out_lab            :    to label
N, E, H             :  from coordinates
*Nout, *Eout, *Hout :    to coordinates
*usertxt            :  to be written in case of errors
*tr_error           :  file for error reports
                    :  tr_error == NULL: NO REPORT IS PRINTED
  
Progr.: K.Engsager, Danish National Space Center, 2006
*/

