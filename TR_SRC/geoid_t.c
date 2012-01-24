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
 


/* geoid_t     ver 2004.01          # page 1   28 Jun 2004 10 19 */


/* Copyright (c) 2004 GEK,  Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GEK, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

/* geoid_t(lab, name, h_dtm, descr) find the name of a table   */
/*                           and reads some parameters         */
/*  result ==  0        : grid lab ok                          */
/*  result ==  0        : Digital Terrain Model lab ok         */
/*  result ==  0        : Digital Ellevation Model lab ok      */
/*  result ==  0        : DHeight ModeL lab ok                 */
/*  result ==  0        : UPLift modeL lab ok                  */
/*  result == -1        : name is not found                    */
/*  result == TAB_N_SYS : manager.tab not found                */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "geo_lab.h"
#include "geoid_d.h"
#include "trthread.h"

int                      geoid_t(
/*____________________________*/
struct gde_lab          *t_lab,
char                    *datum,
char                    *h_dtm,
char                    *descr
)

{

#include              "fgetln_kms.h"
#include              "i_tabdir_file.h"
#include              "s_status.h"
#include              "srch_tab_typ.h"

extern THREAD_SAFE FILE               *man_tab_file;
extern THREAD_SAFE size_t              init_tab_pos;

  long                       pos;
  size_t                     b_t;
  char                       pth_mlb[1024], hlp_mlb[MLBLNG], *p_tp;
  int                        r, qr, used;
  int                        f777 = 0, stype = 0;

  (void) strcpy(hlp_mlb, t_lab->mlb);
  if (strncmp(hlp_mlb, "grsoek", 6) == 0)
     (void) strcpy(hlp_mlb, "grsoek.06");


/* geoid_t     ver 2004.01          # page 2   28 Jun 2004 10 19 */


  if (man_tab_file == NULL || init_tab_pos == 0) {
    (void) i_tabdir_file(1, "", &r, pth_mlb);
    if (r) {
      return(s_status(descr, "geoid_t", TAB_N_SYS_));
    }
  }
  (void) fseek(man_tab_file, (long) init_tab_pos, SEEK_SET);

  r = 1;
  do {
    f777 = srch_tab_typ(&stype, pth_mlb, &pos);
    if (stype != STP_LAB) {
      p_tp = pth_mlb;
      if (stype != STP_LAB) {
        b_t = strlen(hlp_mlb);
        /* look for grid name in REC_TABLE */
        do {
          used = fgetln_kms(pth_mlb, &qr, man_tab_file);
          if (!strncmp(pth_mlb, hlp_mlb, b_t) && qr >= 8) {
            t_lab->lab_type = (short) stype;
            t_lab->f777     = (short) f777;
            p_tp            = pth_mlb + b_t;
            (void) sscanf(p_tp, "%d%n", &qr, &used);
            t_lab->cmplt    = (short) qr;
            p_tp           += used;
            (void) sscanf(p_tp, "%d%n", &qr, &used);
            t_lab->global   = (short) qr;
            p_tp           += used;
            (void) sscanf(p_tp, "%d%n", &qr, &used);
            t_lab->rec_size = qr;
            p_tp           += used;
            (void) sscanf(p_tp, "%d%n", &(t_lab->gd_dec), &used);
            p_tp           += used;
            (void) sscanf(p_tp, "%s%n", h_dtm, &used);
            p_tp           += used;
            (void) sscanf(p_tp, "%s%n", datum, &used);
            p_tp            = strchr(p_tp, '\"') +1;
            b_t             = (ptrdiff_t)strchr(p_tp, '\"')
                            - (ptrdiff_t)p_tp;
            (void) strncpy(descr, p_tp, b_t);
            *(descr + b_t)  = '\0';
            r               = 0;
          }
        } while (strncmp(pth_mlb, "stop", 4) && r && used > 0);
      } /* manager main label ok */
    } /* qr != EOF */
  } while (stype != STP_LAB && r);

  if (stype == STP_LAB || r) {
    t_lab->lab_type = ILL_LAB;
    return(-1);
  }
  return(0);
}
