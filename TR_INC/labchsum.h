

/* labchsum.h                       # page 1   30 Aug 1995 10 04 */


/* Copyright (c) 1995, GD, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#ifndef  _LABCHSUM_
#define  _LABCHSUM_

extern int            labchsum(
/*___________________________*/
union geo_lab        *u_lab,
int                  *int_sum
);

/*
labchsum: The (weightd) sum of the eleemnts of a label
u_lab   : The label to be summed            

The sum must be inserted by the programme, e.g.:

      c_lab->ch_sum =  labchsum(u_lab);

Note, that the lab_type must be used in the LV.

PROG: Knud Poder, FEB 1995
*/

#endif


