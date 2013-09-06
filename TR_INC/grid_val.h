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
struct coord_lab         *c_lab_u,
struct mgde_str          *tab_table,
double                    N,
double                    E,
double                   *g_v,
char                     *err_str
);

/* see allso grid_d.h  and  grid_i.h                          */

/* grid_val produces the table value from a table of gridded data.

The label be initialized by call of           grid_i.

Up to MAXGEOIDS geoid tables are initialized automatically.
Only one table may be open at a time.

The grid_val return value is::
    == 0  tab_val found.
     > 0  (no of wanted datum)
          recall the function with other euref89 datum (e.g. EE_)
     < 0  error
g_v       is the interpolated table value to the point N, E

The coordinates system c_lab_u must be the same as the table.
The system may be checked by the call ::
    table_adm -d -#<table_name>

The coordinates are checked to be inside the table limits.

Closing of the streams is done by grid_c. 

PROG: Karsten Engsager, OCT 2001.

*/

#endif


