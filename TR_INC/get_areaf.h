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
 


/* get_areaf                      # page 1    2 Feb 2004 14 02 */


/* Copyright (c) 2004, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                    */

/* This is unpublished proprietary source code of Kort- og     */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */


extern int       get_areaf(
/*_______________________*/
union geo_lab   *ilb_p,
double          *B,
double          *L,
int              max,
double          *area,
int             *used_max,
char            *wrk_path,
char            *usertxt,
FILE            *tr_error
);

/* get_areaf  calculates the ellipsoidal area of the coordinates
given by the label ilb_p and coordinates B, L in total max numbers

get_area = 0 : result == ok
         = 1 : illegal input coordinate label (ilb_p)
         = 2 : illegal work label (system error)
         = 3 : work_area.geo not initiated
         = 4 : work_area.geo i/o error
         = 5 : Transformation error (system error)

*ilb_p   : pointer to input label.
*B       : Northing/Latitude coordinates (0:max-1)
*L       : Easting/Longitude coordinates (0:max-1)
           (first and last need not be equal)
max      : number of coordinates.
*area    : the calculated area in km*km.
*used_max: the number of sections points for the calculation

*wrk_path: The PATH to a work directory for two temporary workfiles
         : e.g.  "."  (NB: "" is illegal)

*usertxt : text to be written at transformation error.
*tr_error: filepointer for error report.

progr.: Karsten engsager, 2. Feb 2004

*/


