

/* grid_val                      # page 1   26 Oct 2001 11 37 */


/* Copyright (c) 2001 LG, Kort-og Matrikelstyrelsen, Denmark   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of LG, Kort-og  */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#ifndef H_GRID_VAL
#define H_GRID_VAL

#include     "geoid_d.h"

extern int                grid_val(
/*_______________________________*/
union  geo_lab           *c_lab_u,
struct mgde_str          *tab_table,
double                    N,
double                    E,
double                   *g_v,
char                     *err_str
);

/* see allso grid_d.h  and  grid_i.h                          */

/* grid_val produces the table value from a table of gridded data.

The label be initialized by call of           grid_i.

Only one table may be open at a time.

g_v      is the interpolated table value to the point N, E

The coordinates system c_lab_u must be the same as the table.
The system may be checked by the call ::
    table_adm -d -#<table_name>

The coordinates are checked to be inside the table limits.

Closing of the streams is done by grid_c. 

PROG: Karsten Engsager, OCT 2001.

*/

#endif


