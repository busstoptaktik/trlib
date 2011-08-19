

/* get_areaf                      # page 1    2 Feb 2004 14 02 */


/* Copyright (c) 2004, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                    */

/* This is unpublished proprietary source code of Kort- og     */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */


extern int       get_areacf(
/*________________________*/
union geo_lab   *ilb_p,
double          *B,
double          *L,
int              max,
double          *area,
double          *circum,
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
*area    : the calculated area.
*circum  : The circumference of the area.
*used_max: the number of sections points for the calculation

*wrk_path: The PATH to a work directory for two temporary workfiles
         : e.g.  "."  (NB: "" is illegal)

*usertxt : text to be written at transformation error.
*tr_error: filepointer for error report.

progr.: Karsten engsager, 2. Feb 2004

*/


