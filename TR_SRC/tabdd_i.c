

/* tabdd_i   ver 2006.1           # page 1    24 Jul 2006 17 23 */


/* Copyright (c) 2004 Kort & Matrikelstyrelsen, Denmark   */
/* and Danish National Space Center                       */
/* All rights reserved.                                   */

/* This is unpublished proprietary source code of Kort &         */
/* Matrikelstyrelsen, Denmark, and Danish National Space Center. */
/* This copyright claim does not indicate an intention of        */
/* publishing this code.                                         */

#include        <stdio.h>
#include        <string.h>
#include        <ctype.h>
#include        <fcntl.h>
#include        <sys/types.h>
#include        <sys/stat.h>

#include        "geo_lab.h"
#include        "geoid_d.h"

int                     tabdd_i(
/*____________________________*/
char                    *tabdd_n,
int                      tabdd_tp,
struct mtabdd_str       *tabdd_table,
char                    *err_str
)
{

#include     "conv_tab.h"
#include     "fgetln.h"
#include     "tabdd_c.h"
#include     "s_status.h"

  int                        qr;

  /* tabdd pages */
  int                        *ppsg   = &(tabdd_table->sgd[0]);
  long                       *ppge   = &(tabdd_table->pge[0]);

  /* local pointers to tabdd_unit and tabdd_lab */
  union  geo_lab              t_lab_u;
  struct gde_lab             *t_lab = &(t_lab_u.u_g_lab);
  struct gde_lab             *t_lab_p;
  struct lab_def_str          lab_table, *p_lb = &lab_table;


/* tabdd_i   ver 2006.1           # page 2    24 Jul 2006 17 23 */


#ifndef   _WIN32
  int                         r;
#endif

  /* initialize */
  /*____________*/
  if (tabdd_table->init != 0) (void) tabdd_c(tabdd_table);

  tabdd_table->init    = 0;
  tabdd_table->tab_max = 0;
  /* file acces monitoring */
  tabdd_table->hit    = tabdd_table->agn = tabdd_table->xmt = 0L;
  tabdd_table->victim = -1;

  /* clear page tables for the gridtables */
  for (qr = 0; qr < GEOIDPAGES; qr ++) {
    *(ppsg+qr) = -1;
    *(ppge+qr) = -1l;
  }

/*
(void) fprintf(stdout,
"\n*tabdd_i : after init: tabdd_n = %s;", tabdd_n);
*/
  /* initialize the tabdd_n */
  /*________________________*/
  if (strlen(tabdd_n) > 3 && islower(*tabdd_n)) {
    t_lab_p = &tabdd_table->table_u;
    (void) strcpy(p_lb->name, tabdd_n); // only param used
    qr = conv_tab(tabdd_n, p_lb, t_lab, "");
    if (qr == tabdd_tp) {
      *t_lab_p = *t_lab;
      ++ (tabdd_table->tab_max);
    } else        return(TAB_N_NAM_);
  } else        return(TAB_N_NAM_);

  if (tabdd_table->tab_max) tabdd_table->init = 1;
  return(tabdd_table->tab_max);
} /* end init */

