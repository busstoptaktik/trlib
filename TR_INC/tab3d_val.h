

/* tab3d_val  ver 2004.01           # page 1   25 Jun 2004 11 37 */


/* Copyright (c) 2004, Kort-og Matrikelstyrelsen, Denmark   */
/* All rights reserved.                                     */

/* This is unpublished proprietary source code of Kort-og      */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#ifndef H_TAB3D_VAL
#define H_TAB3D_VAL

extern int                tab3d_val(
/*______________________________*/
union  geo_lab           *c_lab_u,
struct mtab3d_str        *tab3d_table,
double                    B_in,
double                    L_in,
double                   *val,
char                     *usertxt
);

/* see allso tab3d_d.h                         (for GEOIDPATH) */

/* tab3d_val produces the 3d_val from the table covering part
of the ellipsoid.

val      is the returned 3d_valus: (N,E,H) or (X,Y,Z)

The stream to the table must be opened by tab3d_i.
Closing of the streams is done by tab3d_c. 

PROG: K. Engsager, JUN 2004.

*/

#endif


