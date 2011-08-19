

/* tabdd_val  ver 2006.01           # page 1   25 Jun 2006 11 37 */


/* Copyright (c) 2004, Kort-og Matrikelstyrelsen, Denmark   */
/* All rights reserved.                                     */

/* This is unpublished proprietary source code of Kort-og      */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#ifndef H_TABDD_VAL
#define H_TABDD_VAL

extern int                tabdd_val(
/*______________________________*/
union  geo_lab           *c_lab_u,
struct mtabdd_str        *tabdd_table,
double                    B_in,
double                    L_in,
double                   *val,
char                     *usertxt
);

/* see allso geoid_d.h                         (for GEOIDPATH) */

/* tabdd_val produces the 2d_val (double) from the table covering part
of the ellipsoid.

val      is the returned 2d_valus: (N,E) or (X,Y)

The stream to the table must be opened by tabdd_i.
Closing of the streams is done by tabdd_c. 

PROG: K. Engsager, JUN 2006.

*/

#endif


