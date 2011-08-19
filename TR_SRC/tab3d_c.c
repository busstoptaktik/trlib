

/* tab3d_c                         # page 1   25 Jun 2004 17 25 */


/* Copyright (c) 2001 LG, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                      */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include        <stdio.h>
#include        <string.h>
#include        "geo_lab.h"
#include        "geoid_d.h"


int                       tab3d_c(
/*______________________________*/
struct mtab3d_str        *tab_table
)
{

  int                         res = 0;
  struct gde_lab             *t_lab;

  if (tab_table == NULL) return(0);

  if (tab_table->init) {
    t_lab = &(tab_table->table_u);
    if (t_lab->lab_type == T3D_LAB) {
      (void) fclose(t_lab->fd);
      tab_table->init = 0;
    }
    else res = 1;
  }

  return(res);
}

