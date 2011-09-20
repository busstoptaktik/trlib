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

