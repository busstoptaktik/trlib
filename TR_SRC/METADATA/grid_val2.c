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
 
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    "KmsFncs.h"
#include    "grim.h"

/* mock up - possible add channel in call */
int                       grid_val2(
/*_______________________________*/
struct coord_lab         *crd_lab,
table_adm_str          *grid_table,
double                    N,
double                    E,
double                   *g_v
)
{
	int i;
	GRIM   table=NULL;
	double row,column;
	lord_debug(0,LORD("Not fully implemented yet..., N: %.2f, E: %.2f, mlb: %s"),N,E,crd_lab->mlb);
	/*loop over tables - we assume for now that input coords are in the right system*/
	for(i=0; i<grid_table->n_tables; i++){
		table=grid_table->table_sequence[i];
		lord_debug(0,LORD("Grid: %s, mlb: %s"),grim_filename(table),grim_proj(table));
		row=grim_row(table,N,E);
		column=grim_column(table,N,E);
		if (0<=row && row<grim_rows(table) && 0<=column && column<grim_columns(table)){
			lord_debug(0,LORD("HIT: %s"),grim_filename(table));
			grid_table->cur_table=i;
			*g_v=grim_value(table,N,E,0,0);
			return 0;
		}
	}
	*g_v=-9999;
	lord_error(TAB_C_ARE_,LORD("Oh no - out of area...."));
	return TAB_C_ARE_;
}