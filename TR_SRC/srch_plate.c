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
 


/* srch_plate  ver 2008.02          # page 1   16 May 2008 11 57 */


/* Copyright (c) 2008, DTU, National Space Research Institute    */
/* All rights reserved.                                          */

/* This is unpublished proprietary source code of DTU,           */
/* National Space Research Institute, Denmark. This copyright    */
/* claim does not indicate an intention of publishing this code. */

#include    <stdio.h>
#include    <string.h>
#include    "KmsFncs.h"
#include    "trthread.h"

int                       srch_plate(
/*_________________________________*/
struct plate_info        *pl_inf,
double                    B,
double                    L,
struct plm_lab           *plm_tab,
struct gps_c_str         *c_gps,
char                     *err_txt
)
{

/* search plate containing coordinates  B, L, */
/* return values ::                           */
/*       >=  0   :: plate number              */
/*          -1   :: NO plate found            */
/*          -2   :: i/o error on binary model */

/* progr. K. Engsager. Feb. 2003, 2008 */

  /* search plate containing coordinates */

  static THREAD_SAFE  char                 s_plm_mlb[MLBLNG];
  static THREAD_SAFE  double               poly[4500];
  static THREAD_SAFE  int                  s_plate = -1;
  struct plate_info          *p_pl_inf = NULL;
  struct plm_c_str           *p_entry = &(plm_tab->plm__tr[0]);
  double                      lon;
  int                         inf_nr;
  int                         new_srch = 1, new_plate = 0;
  FILE                       *f_poly = plm_tab->f_poly;

  /* basic transformation formula (Molodensky) ::                   */
  /* (XS) = (X) + (T1) + ( D  -R3  R2) (X)                          */
  /* (YS) = (Y) + (T2) + ( R3  D  -R1) (Y)                          */
  /* (ZS) = (Z) + (T3) + (-R2  R1  D ) (Z)                          */
  /* T == 0.0,  D = 0.0, R = plm_R * (o_date - i_date)              */


/* srch_plate  ver 2008.02          # page 2   16 May 2008 11 57 */


  /* the search is continued until new_srch == false */
  if (s_plate == -1) *s_plm_mlb = '\0';
  /* QUICK : test last plate */
  if (s_plate >= 0) {
    lon      = L;
    p_pl_inf = pl_inf + s_plate;
    /* 1. inside square test */
    switch (p_pl_inf->poly_tp % 10) {
    case 1: /* NAM */
      new_srch =  B < p_pl_inf->BS /* N-pole */
              && (B < p_pl_inf->B1 || B > p_pl_inf->B2
               || L < p_pl_inf->L1 || L > p_pl_inf->L2);
      break;
    case 2: /* no quick */
      break;
    case 3: /* ANT */
      new_srch =  B > p_pl_inf->BS /* S-pole */;
      break;
    case 4: /* square */
      new_srch = B < p_pl_inf->B1 || B > p_pl_inf->B2
              || L < p_pl_inf->L1 || L > p_pl_inf->L2;
      break;
    case 5: /* DATE - LINE included (problem at +-180dgE/W) */
      new_srch = B < p_pl_inf->B1 || B > p_pl_inf->B2
              || L < p_pl_inf->L1 || L > p_pl_inf->L2;
      lon      = v_red(L + M_PI);
      break;
    }
    if (new_srch) {
      /* test poly_a : 2. simple inner polygon test */
      if (fseek(f_poly, (long) p_pl_inf->poly_a, SEEK_SET) ||
         ((fread((void *) poly, sizeof(double),
                 p_pl_inf->a_ant, f_poly)) - p_pl_inf->a_ant)) {
        (void) sprintf(err_txt,
            "\nsrch_plate: ERROR at read of poly_a");
        (void) sprintf(err_txt,
            "\n poly_a_adr %8ld plate  = %4d size = %4ld ;",
            ftell(f_poly), p_pl_inf->plate_nr, (long) sizeof(double));
        return(-2);
      }
      if (inpoly(poly, &B, &lon) < 0) {
        /* test poly_b : 3. fine polygon test */
        if (fseek(f_poly, (long) p_pl_inf->poly_b, SEEK_SET) ||
           ((fread((void *) poly, sizeof(double),
                   p_pl_inf->b_ant, f_poly)) - p_pl_inf->b_ant)) {
          (void) sprintf(err_txt,
              "\nsrch_plate: ERROR at read of poly_b");
          (void) sprintf(err_txt,
              "\n poly_b_adr %8ld plate  = %4d size = %4ld ;",
              ftell(f_poly), p_pl_inf->plate_nr, (long) sizeof(double));
          return(-2);
        }
        new_srch = inpoly(poly, &B, &lon) < 0;
      }
      else new_srch = 0;
    }
  }


/* srch_plate  ver 2008.02          # page 3   16 May 2008 11 57 */


  /* step through all plates */
  if (new_srch) {
    new_plate = 1;
    inf_nr    = -1;
    p_pl_inf  = pl_inf;
    /* test quick */
    do {
      ++ inf_nr;
      /* 1. inside square test */
      switch (p_pl_inf->poly_tp % 10) {
      case 1: /* NAM */
        new_srch =  B < p_pl_inf->BS /* N-pole */
                && (B < p_pl_inf->B1 || B > p_pl_inf->B2
                 || L < p_pl_inf->L1 || L > p_pl_inf->L2 /* GR */);
        break;
      case 2: /* no quick */
        break;
      case 3: /* ANT */
        new_srch =  B > p_pl_inf->BS /* S-pole */;
        break;
      case 4: /* square */
        new_srch = B < p_pl_inf->B1 || B > p_pl_inf->B2
                || L < p_pl_inf->L1 || L > p_pl_inf->L2;
        break;
      case 5: /* DATE - LINE included (problem at +-180dgE/W) */
        new_srch = B < p_pl_inf->B1 || B > p_pl_inf->B2
                || L < p_pl_inf->L1 || L > p_pl_inf->L2;
        break;
      }
      if (new_srch) ++ p_pl_inf;
    } while (p_pl_inf->plate_nr && new_srch);

    if (new_srch) {
      /* test poly_a : 2. simple inner polygon test */
      inf_nr   = -1;
      p_pl_inf = pl_inf;
      if (fseek(f_poly, (long) p_pl_inf->poly_a, SEEK_SET)) {
        (void) sprintf(err_txt,
            "\nsrch_plate: ERROR at read of poly_a");
        (void) sprintf(err_txt,
            "\n poly_a_adr %8ld plate  = %4d size = %4ld ;",
            ftell(f_poly), p_pl_inf->plate_nr, (long) sizeof(double));
        return(-2);
      }
      do {
        ++ inf_nr;
        lon = (p_pl_inf->poly_tp != 5) ? v_red(L) : v_red(L + M_PI);
        if (fread((void *) poly, sizeof(double),
                  p_pl_inf->a_ant, f_poly) - p_pl_inf->a_ant) {
          (void) sprintf(err_txt,
              "\nsrch_plate: ERROR at read of poly_a");
          (void) sprintf(err_txt,
              "\n poly_a_adr %8ld plate  = %4d size = %4ld ;",
              ftell(f_poly), p_pl_inf->plate_nr, (long) sizeof(double));
          return(-2);
        }
        new_srch = inpoly(poly, &B, &lon) < 0;
        if (new_srch) ++ p_pl_inf;
      } while (p_pl_inf->plate_nr && new_srch);


/* srch_plate  ver 2008.02          # page 4   16 May 2008 11 57 */


      if (new_srch) {
        /* test poly_b : 3. fine polygon test */
        inf_nr   = -1;
        p_pl_inf = pl_inf;
        if (fseek(f_poly, (long) p_pl_inf->poly_b, SEEK_SET)) {
          (void) sprintf(err_txt,
              "\nsrch_plate: ERROR at read of poly_b");
          (void) sprintf(err_txt,
              "\n poly_b_adr %8ld plate  = %4d size = %4ld ;",
              ftell(f_poly), p_pl_inf->plate_nr, (long) sizeof(double));
          return(-2);
        }
        do {
          ++ inf_nr;
          lon = (p_pl_inf->poly_tp != 5) ? v_red(L) : v_red(L + M_PI);
          if ((fread((void *) poly, sizeof(double),
                     p_pl_inf->b_ant, f_poly)) - p_pl_inf->b_ant) {
            (void) sprintf(err_txt,
                "\nsrch_plate: ERROR at read of poly_b");
            (void) sprintf(err_txt,
                "\n poly_b_adr %8ld plate  = %4d size = %4ld ;",
                ftell(f_poly), p_pl_inf->plate_nr, (long) sizeof(double));
            return(-2);
          }
          new_srch = inpoly(poly, &B, &lon) < 0;
          if (new_srch) ++ p_pl_inf;
        } while (p_pl_inf->plate_nr && new_srch);
      }
    }
    if (p_pl_inf->plate_nr == 0) {
      (void) sprintf(err_txt,
                "\nsrch_plate: NO PLATE FOUND : %8.4fdg, %8.4fdg\n",
                B * DOUT, L * DOUT);
      return(-1);
    }
    s_plate  = inf_nr;
    p_pl_inf = pl_inf + inf_nr;
  }

  if (new_plate || strcmp(s_plm_mlb, plm_tab->mlb)) {
    (void) strcpy(s_plm_mlb, plm_tab->mlb);
    p_entry   += p_pl_inf->plate_nr -1;
    /* R = plm_R * (o_date - i_date)              */
    (void) strcpy(c_gps->datum, p_entry->datum);
    c_gps->rx  = p_entry->drx;
    c_gps->ry  = p_entry->dry;
    c_gps->rz  = p_entry->drz;
  }

  return(p_pl_inf->plate_nr -1);

}

