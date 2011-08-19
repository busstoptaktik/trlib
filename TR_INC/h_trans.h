

/* h_trans  ver 2004.01          # page 1    24 Jan 2004 11 57 */


/* Copyright (c) 2003,   Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                     */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */


extern int               h_trans(
/*_____________________________*/
union geo_lab           *i_lab,
short                    oh_dtm,
double                   B_in,
double                   L_in,
double                  *dh,
char                    *tr_str
);


/* h_trans  gives the increment dh for transformations of heights
p.t. only : from h_dvr90 to h_evrf00 ::

             h_evrf00 = h_dvr90 + dh

i_lab must be a label describing the coordinate and height system
      The coordinate must be the requested system for the dh calc.
      the height system must be the input height datuma and 
      correspond to one of the requested height systems.

B_in  the northing coordinate
L_in  the easting coordinate

dh    the pointer for the result

tr_error a File pointer to error text.

SYSTEMS :
h_dvr90 -> h_evrf00: geo_euref89

Programmer: Karsten Engsager, KMS, 2004.

*/

