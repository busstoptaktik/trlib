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
 

/* tab3d_i   ver 2004.1           # page 1    24 Jun 2004 17 23 */


/* Copyright (c) 2004 GEK, Kort & Matrikelstyrelsen, Denmark    */
/* All rights reserved.                                         */

/* This is unpublished proprietary source code of Kort &       */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include        <stdio.h>
#include        <string.h>
#include        <ctype.h>
#include        <fcntl.h>
#include        <sys/types.h>
#include        <sys/stat.h>

#include        "geo_lab.h"
#include        "geoid_d.h"

int                     tab3d_i(
/*____________________________*/
char                    *tab3d_n,
int                      tab3d_tp,
struct mtab3d_str       *tab3d_table,
char                    *err_str
)
{

#include     "conv_tab.h"
#include     "tab3d_c.h"

  int                        qr;

  /* Tab3d pages */
  int                        *ppsg   = &(tab3d_table->sgd[0]);
  long                       *ppge   = &(tab3d_table->pge[0]);

  /* local pointers to tab3d_unit and tab3d_lab */
  union  geo_lab              t_lab_u;
  struct gde_lab             *t_lab = &(t_lab_u.u_g_lab);
  struct gde_lab             *t_lab_p;
  struct lab_def_str          lab_table, *p_lb = &lab_table;


/* tab3d_i   ver 2004.1           # page 2    24 Jun 2004 17 23 */


  /* initialize */
  /*____________*/
  if (tab3d_table->init != 0) (void) tab3d_c(tab3d_table);

  tab3d_table->init    = 0;
  tab3d_table->tab_max = 0;
  /* file acces monitoring */
  tab3d_table->hit    = tab3d_table->agn = tab3d_table->xmt = 0L;
  tab3d_table->victim = -1;

  /* clear page tables for the gridtables */
  for (qr = 0; qr < GEOIDPAGES; qr ++) {
    *(ppsg+qr) = -1;
    *(ppge+qr) = -1l;
  }

/*
(void) fprintf(stdout,
"\n*tab3d_i : after init: tab3d_n = %s;", tab3d_n);
*/
  /* initialize the tab3d_n */
  /*________________________*/
  if (strlen(tab3d_n) > 3 && islower(*tab3d_n)) {
    t_lab_p = &tab3d_table->table_u;
    (void) strcpy(p_lb->name, tab3d_n);
    qr = conv_tab(tab3d_n, p_lb, t_lab, "");
    if (qr == tab3d_tp) {
      *t_lab_p = *t_lab;
      ++ (tab3d_table->tab_max);
    } else        return(TAB_N_NAM_);
  } else        return(TAB_N_NAM_);

  if (tab3d_table->tab_max) tab3d_table->init = 1;
  return(tab3d_table->tab_max);
}

