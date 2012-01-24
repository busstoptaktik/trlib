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
 


/* geoid_i   ver 2003.1           # page 1    4 Jul 2003 17 23 */


/* Copyright (c) 2003 GEK, Kort & Matrikelstyrelsen, Denmark    */
/* All rights reserved.                                         */

/* This is unpublished proprietary source code of GD, Kort &   */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include        <stddef.h>
#include        <stdio.h>
#include        <string.h>
#include        <ctype.h>
#include        <fcntl.h>
#include        <sys/types.h>
#include        <sys/stat.h>

#include        "geo_lab.h"
#include        "geoid_d.h"
#include        "trthread.h"

int                     geoid_i(
/*____________________________*/
char                    *geoid_n,
int                      geoid_tp,
struct mgde_str         *geoid_table,
char                    *err_str
)
{

#include     "conv_tab.h"
#include     "fgetln_kms.h"
#include     "geoid_c.h"
#include     "i_tabdir_file.h"
#include     "s_status.h"

extern THREAD_SAFE  FILE               *man_tab_file;
extern THREAD_SAFE size_t              init_tab_pos;

  char                       ftx[128], wrk[16], wr1[16], *p_tp;
  size_t                     size;
  int                        qr, used;

  /* Geoid pages */
  int                        *ppsg   = &(geoid_table->sgd[0]);
  long                       *ppge   = &(geoid_table->pge[0]);

  /* local pointers to geoid_unit and gde_lab */
  union  geo_lab              t_lab_u;
  struct gde_lab             *t_lab = &(t_lab_u.u_g_lab);
  struct gde_lab             *t_lab_p;
  struct lab_def_str          lab_table, *p_lb = &lab_table;



/* geoid_i   ver 2003.1           # page 2    4 Jul 2003 17 23 */


  /* initialize */
  /*____________*/
  if (geoid_table->init != 0) (void) geoid_c(geoid_table, 0, NULL);

  geoid_table->init    = 0;
  geoid_table->tab_max = 0;
  /* file acces monitoring */
  geoid_table->hit    = geoid_table->agn = geoid_table->xmt = 0L;
  geoid_table->victim = -1;

  /* clear page tables for the gridtables */
  for (qr = 0; qr < GEOIDPAGES; qr ++) {
    *(ppsg+qr) = -1;
    *(ppge+qr) = -1l;
  }

/*
(void) fprintf(stdout,
"\n*geoid_i : after init: geoid_n = %s;", geoid_n);
*/
  /* initialize the geoid_n */
  /*________________________*/
  if (strlen(geoid_n) > 3 && islower(*geoid_n)) {
    (void) strcpy(p_lb->name, geoid_n); // only param used
    t_lab_p = &(geoid_table->table_u[geoid_table->tab_max]);
    qr = conv_tab("", p_lb, t_lab, "");
    if (qr == geoid_tp) {
      *t_lab_p = *t_lab;
      ++ (geoid_table->tab_max);
    } else        return(TAB_N_NAM_);
    (void) strcpy(wrk, "STD");
  } 
  else (void) strcpy(wrk, geoid_n);

  if (geoid_tp == GDE_LAB) {

    if (man_tab_file == NULL || init_tab_pos == 0) {
      (void) i_tabdir_file(1, "", &qr, ftx);
      if (qr) {
        return(s_status(err_str, "geoid_i", TAB_N_MAN_));
      }
    }
    (void) fseek(man_tab_file, (long) init_tab_pos, SEEK_SET);

    /* initialize the grid's */
    /*________________________*/
    /* Test for identification of a man_tab_file.tab file */
    qr    = fgetln_kms(ftx, &used, man_tab_file);
    size  = !strncmp(ftx, "#geoid  manager  777", 19);


/* geoid_i   ver 2003.1           # page 3    4 Jul 2003 17 23 */


    p_tp = ftx;
    if (size != 0L) {
      /* look for information in manager : STD */
      do {
        qr   = fgetln_kms(ftx, &used, man_tab_file);
        size = (size_t) !strncmp(ftx, "STD", 3);
      } while (qr > 0 && (size == 0));
    }

    if (size != 0L) {
      /* if wrk != "STD" search for REGION  (e.g. GR) */
      if (strncmp(wrk, "STD", 3)) {
        p_tp += 3;
        do {
          (void) sscanf(p_tp, "%s%n", wr1, &used); 
          size  = (size_t) !strcmp(wr1, wrk);
        } while((int) ((ptrdiff_t)p_tp - (ptrdiff_t)ftx) > qr
                && size == 0);
        if (size == 0L) {
          return(s_status(err_str, "geoid_i", TAB_R_MAN_));
        }
      }
    }

    if (size != 0L) {
      /* look for information in manager : wrk */
      do {
        qr   = fgetln_kms(ftx, &used, man_tab_file);
        size = (size_t) !strncmp(ftx, wrk, 2);
      } while (qr > 0 && (size == 0));


/* geoid_i   ver 2003.1           # page 4    4 Jul 2003 17 23 */


      /* get geoid_names from the list : wrk */
      do {
        used = fgetln_kms(ftx, &qr, man_tab_file);
        (void) sscanf(ftx, "%s%n", p_lb->name, &used); 
        qr   = (used > 0) ? strncmp(p_lb->name, "stop", 4) : 1;
        if (qr && geoid_table->tab_max < MAXGEOIDS) {
          t_lab_p = &(geoid_table->table_u[geoid_table->tab_max]);
          (void) strcpy(t_lab_p->mlb, p_lb->name);
          ++(geoid_table->tab_max);
        } else
        if (qr) {
          return(s_status(err_str, "geoid_i", TAB_G_MAX_));
        }
      } while (qr);

      /* initialize geoid_names */
      used                 = geoid_table->tab_max;
      geoid_table->tab_max = 0;
      for (qr = 0; qr < used; qr ++) {
        t_lab_p = &(geoid_table->table_u[qr]);
        (void) strcpy(p_lb->name, t_lab_p->mlb);
        if (conv_tab("", p_lb, t_lab, "") == GDE_LAB) {
          *t_lab_p = *t_lab;
          ++(geoid_table->tab_max);
        } else {
          (void) sprintf(ftx, "geoid_i: %s", p_lb->name);
          return(s_status(err_str, ftx, TAB_N_MAN_));
        }
      }
    } /* size != 0L */
  } /* geoid_tp == GDE_LAB */

  if (geoid_table->tab_max) geoid_table->init = 1;
  else return(s_status(err_str, "geoid_i", TAB_N_NAM_));
  return(geoid_table->tab_max);
} /* end init */
