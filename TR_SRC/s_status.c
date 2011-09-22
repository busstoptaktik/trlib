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
 


/* s_status    ver 2009.1        # page 1    7 jan 2009 14 05 */


/* Copyright (c) 2009, National Space Institute, DTU, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of DTU, Denmark */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */


#include    <stdio.h>
#include    <stdarg.h>
#include    <string.h>
#include    "geo_lab.h"

int                      s_status(
/*______________________________*/
char                    *err_str,
char                    *fnc_name,
int                      err_type,
...
)

/* ADDITIONAL PARAMETERS 
char                    *c_unit;
char                    *h_unit;
double                    N,  E,  H; 
double                   dN, dE, dH;

SPECIAL::  c_unit == "non"  ==>>  SKIPS add params writing 
*/

{

#include             "set_tpd.h"
#include             "sputg.h"

  va_list              cp;

  char               *c_unit, *h_unit;
  double              N, E, H, dN, dE, dH;
  struct typ_dec      c_tpd;
  char                tpt[64], *p_er;
  int                 c_err, t_err, i_err;
  int                 crd_out = 0, dif_out = 0;

  va_start(cp, err_type);

  if (err_str != NULL) {
    p_er     = err_str;
    *err_str = '\0';


/* s_status    ver 2009.1        # page 2    7 jan 2009 14 05 */


    c_err = err_type % 100;
    switch(c_err) {
    case TRF_INACC_: /* Outside area */
      (void) strcpy(tpt, "BOUNDARY EXCEEDED (inaccurate)");
      crd_out = 1;
      break;
    case TRF_TOLLE_: /* Tol exceeded */
      (void) strcpy(tpt, "TOLERANCE EXCEEDED");
      crd_out = 1;
      dif_out = 1;
      break;
    case TRF_AREA_: /* Outside area */
      (void) strcpy(tpt, "BOUNDARY EXCEEDED (serious)");
      crd_out = 1;
      break;
    case TRF_GEOID_: /* Illegal transformation */
      (void) strcpy(tpt, "NO GEOID ILLEGAL TRANSFORMATION");
      break;
    case TRF_ILLEG_: /* Illegal transformation */
      (void) strcpy(tpt, "ILLEGAL TRANSFORMATION");
      break;
    case HTRF_ILLEG_: /* Illegal height transformation */
      (void) strcpy(tpt, "ILLEGAL HEIGHT TRANSFORMATION");
      break;
    case TRF_PROGR_: /* Programme/system fault */
      (void) strcpy(tpt, "SYSTEM/PROG FAULT");
      break;
    };
    if (c_err <= TRF_INACC_) {
      p_er += sprintf(p_er, "\n\nERROR % 2d%4s%-20s%4s",
                      c_err, " ", tpt, " ");
    }

    t_err = (err_type / 100) % 100;
    switch(t_err * 100) {
    case TAB_C_ARE_: /* TABLE area out */
      (void) strcpy(tpt, "TABLE AREA OUT");
      crd_out = 1;
      break;
    case TAB_C_PRJ_: /* TABLE called with wrong label */
      (void) strcpy(tpt, "TABLE WRONG LABEL");
      break;
    case TAB_C_NON_: /* TABLE not found */
      (void) strcpy(tpt, "TABLE NONE FOUND");
      break;
    case TAB_G_MAX_: /* Geoid too many */
      (void) strcpy(tpt, "GEOID TOO MANY");
      break;
    case TAB_N_NAM_: /* Table_name not found in Manager.tab */
      (void) strcpy(tpt, "table_name not found in manager.tab");
      break;
    case TAB_R_MAN_: /* Manager.tab REGION not found */
      (void) strcpy(tpt, "manager.tab region not found");
      break;
    case TAB_N_MAN_: /* Manager.tab not found */
      (void) strcpy(tpt, "manager.tab not found");
      break;
    case TAB_N_SYS_: /* Manager.tab not found */
      (void) strcpy(tpt, "manager.tab not found");
      break;
    };


/* s_status    ver 2009.1        # page 3    7 jan 2009 14 05 */


    if (t_err < 0) {
      if (!c_err) p_er += sprintf(p_er, "\n");
      p_er += sprintf(p_er, "\nERROR % 2d%4s%-20s%4s",
                      t_err, " ", tpt, " ");
    }

    i_err = (err_type / 10000) % 100;
    switch(i_err * 10000) {
    case PLATE_NON_: /* PLATE not found */
      (void) strcpy(tpt, "itrf_plat.bin NOT FOUND");
      break;
    case PLATE_OUT_: /* out of PLATE */
      (void) strcpy(tpt, "station out of PLATES");
      break;
    case ITRF_NON_: /* ITRF not found */
      (void) strcpy(tpt, "ITRFyy NONE FOUND");
      break;
    case ITRF_SEQ_: /* ITRF not in sequence */
      (void) strcpy(tpt, "ITRF NOT IN SEQUENCE in manager");
      break;
    case ITRF_DTM_: /* DTM not in {ITRF, IGS, ETRF, EUREF89} */
      (void) strcpy(tpt, "DATUM NOT FOUND");
      break;
    case ITRF_MAX_: /* ITRF too many */
      (void) strcpy(tpt, "ITRF TOO MANY");
      break;
    case ITRF_NAM_: /* ITRF not found */
      (void) strcpy(tpt, "manger.gps NOT FOUND");
      break;
    case ITRF_SYS_: /* ITRF system fault */
      (void) strcpy(tpt, "ITRF SYSTEM FAULT");
      break;
    case PLATE_NO_VEL_: /* PLATE has no velocity */
      (void) strcpy(tpt, "Plate has no velocity");
      break;
    }

    if (i_err < 0) {
      if ((!c_err) && (!t_err)) p_er += sprintf(p_er, "\n");
      p_er += sprintf(p_er, "\nERROR % 2d%4s%-20s%4s",
                      t_err, " ", tpt, " ");
    }

    if (err_type) {
      p_er += sprintf(p_er, "fnc: %-20s", fnc_name);

      if (crd_out) {
        c_unit = va_arg(cp, char *);
        if (strcmp(c_unit, "non") == 0) crd_out = 0;
      }
      if (crd_out) {
        h_unit = va_arg(cp, char *);

        if (strcmp(c_unit, "sx"))
             c_tpd = *set_tpd("m ", 9, 3);
        else c_tpd = *set_tpd("sx", 8, 4);


        N = va_arg(cp, double);
        E = va_arg(cp, double);
        p_er += sprintf(p_er, "\n        ");
        p_er += sputg(p_er, N, &c_tpd, " +u");
        p_er += sputg(p_er, E, &c_tpd, " +u");

        if (err_type == 1 || *h_unit != '\0')
            c_tpd = *set_tpd("m ", 9, 3);

        if (*h_unit != '\0') {
          H = va_arg(cp, double);
          p_er += sputg(p_er, H, &c_tpd, " +u");
        }

        if (dif_out) {
          dN = va_arg(cp, double);
          dE = va_arg(cp, double);

          p_er += sprintf(p_er, "\n%-20s", "     diff");
          p_er += sputg(p_er, dN, &c_tpd, " +u");
          p_er += sputg(p_er, dE, &c_tpd, " +u");

          if (*h_unit != '\0') {
            dH = va_arg(cp, double);
            p_er += sputg(p_er, dH, &c_tpd, " +u");
          }
        } /* end dif out */
      } /* end coord_out */
    } /* end errortxt_out */

  }

  va_end(cp);
  return(err_type);
}
