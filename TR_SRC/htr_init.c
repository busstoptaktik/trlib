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
 


/* htr_init  ver 2010            # page 1    24 Jul 2010 11 57 */


/* Copyright (c) 2003,   Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                     */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "geo_lab.h"
#include "kms_math.h"
#include "trthread.h"


int                      htr_init(
/*______________________________*/
struct coord_lab        *i_clb,
struct coord_lab        *o_clb,
union  geo_lab          *htr_lab,
struct htr_c_str        *htr_const,
char                    *dh_table_name,
char                    *dh_tr_info
)
{

/*
#include              "erad.h"
*/
#include              "conv_lab.h"
#include              "fgetlhtx.h"
#include              "fgetln.h"
#include              "i_tabdir_file.h"
#include              "set_dtm.h"
#include              "sgetg.h"

  extern THREAD_SAFE FILE               *def_lab_file;
  extern THREAD_SAFE size_t              init_prj_pos, init_hth_pos;


/* htr_init  ver 2004.01          # page 2    24 Jan 2004 11 57 */


  static THREAD_SAFE  short          sh_dtm = 0;
  union  geo_lab        test_lab;
  char                  pth_mlb[512], c_mlb[MLBLNG], *p_tp;
  int                   tr_type = HTRF_ILLEG_, res = 1, l_inv, qr, used;
  long                  pos = 0;
  short                 p_no, i_hdtm, o_hdtm, rgn;
  char                  i_nm[MLBLNG], p_nm[MLBLNG], e_nm[MLBLNG];
  char                  o_nm[MLBLNG];
  struct typ_dec        g_tpd;
  struct dsh_str        trp;
  union rgn_un          DK_rgn, FO_rgn, GR_rgn, DE_rgn;

  if (i_clb->h_dtm < 200 || 299 < i_clb->h_dtm ||
      o_clb->h_dtm < 200 || 299 < o_clb->h_dtm) return(HTRF_ILLEG_);

  (void) strcpy(DK_rgn.prfx, "DK");
  (void) strcpy(FO_rgn.prfx, "FO");
  (void) strcpy(GR_rgn.prfx, "GR");
  (void) strcpy(DE_rgn.prfx, "DE");


/* htr_init  ver 2004.01          # page 3    24 Jan 2004 11 57 */


  if (sh_dtm == 0) {
    if (def_lab_file == NULL || init_prj_pos == 0) {
      (void) i_tabdir_file(3, "", &res, pth_mlb);
      if (res) {
        (void) fprintf(stdout, "\n*** def_lab.txt: %s %s;\n", pth_mlb,
          (res == -1) ? "NOT FOUND" :
          (res == -2) ? "NOT lab definition file" :
                        "Content not accepted");
        return (-1);
      }
    } else res = 0;

    if (init_hth_pos) {
      (void) fseek(def_lab_file, (long) init_hth_pos, SEEK_SET);
    } else { // search "def_hth" (fr
      qr = 1;
      do {
        res = fgetlhtx(def_lab_file, c_mlb);
        if (res == 0) {
          if (!strcmp("def_hth", c_mlb)) qr = 0;
        }
      } while (qr && res != EOF);
      if (qr) {
        (void) fprintf(stdout, 
           "\n*** set_dtm: lab_def.txt H_to_H not found ;\n");
        return (-2);
      }
      init_hth_pos = ftell(def_lab_file);
    }

    htr_lab->u_c_lab.h_dtm    = 0;
    htr_lab->u_c_lab.lab_type = ILL_LAB;
  }
  else (void) fseek(def_lab_file, (long) init_hth_pos, SEEK_SET);

  sh_dtm = -1;
  /* find datum name from datum number */
  i_hdtm = i_clb->h_dtm;
  o_hdtm = o_clb->h_dtm;
  /* Ellipsoidal h or CRT search parent */
  if (i_hdtm == 200 && o_hdtm != 200) {
    rgn = i_clb->p_rgn;
    if (rgn == 0) rgn = o_clb->p_rgn;
    if (i_clb->imit == FHMASK || o_clb->imit == FHMASK) i_hdtm = 213 /*fcsvr10*/;
    else
    if (rgn == DK_rgn.r_nr[0]) i_hdtm = 208/*dvr90*/;
    else
    if (rgn == DE_rgn.r_nr[0]) i_hdtm = 215/*dhhn92*/;
    else  /* FO_MSL before 2009 is named:: foldmsl (== 216) */
    if (rgn == FO_rgn.r_nr[0]) i_hdtm = 211; /*fvr09*/
    else
    if (rgn == GR_rgn.r_nr[0]) i_hdtm = 202; /* msl */
    else i_hdtm = 202; /* msl */;
  }
  if (i_hdtm == 202) { /* msl */ 
    if (o_hdtm == 208 || rgn == DK_rgn.r_nr[0]) i_hdtm = 208;
    else
    if (o_hdtm == 211 || rgn == FO_rgn.r_nr[0]) i_hdtm = 211;
  }
  if (o_hdtm == 200 && i_hdtm != 200) {
    rgn = o_clb->p_rgn;
    if (rgn == 0) rgn = i_clb->p_rgn;
    if (i_clb->imit == FHMASK || o_clb->imit == FHMASK) o_hdtm = 213 /*fcsvr10*/;
    else
    if (rgn == DK_rgn.r_nr[0]) o_hdtm = 208/*dvr90*/;
    else
    if (rgn == DE_rgn.r_nr[0]) o_hdtm = 215/*dhhn92*/;
    else
    if (rgn == FO_rgn.r_nr[0]) o_hdtm = 211; /*fvr09*/
    else
    if (rgn == GR_rgn.r_nr[0]) o_hdtm = 202; /* msl */
    else o_hdtm = 202; /* msl */;
  }
  if (o_hdtm == 202) { /* msl */
    if (i_hdtm == 208 || rgn == DK_rgn.r_nr[0]) o_hdtm = 208;
    else
    if (i_hdtm == 211 || rgn == FO_rgn.r_nr[0]) o_hdtm = 211;
  }

  if (i_hdtm == o_hdtm) tr_type = 0;
  else {
    pos = ftell(def_lab_file);
    (void) set_dtm(i_hdtm, i_nm, &p_no, p_nm, e_nm, &trp);
    (void) set_dtm(o_hdtm, o_nm, &p_no, p_nm, e_nm, &trp);
    (void) fseek(def_lab_file, (long) pos, SEEK_SET);

    do {
      pos = ftell(def_lab_file);
      (void) conv_lab("input", &test_lab, def_lab_file, "");
      if (test_lab.u_c_lab.lab_type == 1) {
        (void) fseek(def_lab_file, (long) pos, SEEK_SET);
        (void) fgetln(pth_mlb, &used, def_lab_file); //now:skip lab line
        l_inv = test_lab.u_c_lab.h_dtm == o_hdtm;
        if (test_lab.u_c_lab.h_dtm == i_hdtm || l_inv) {
          qr = fgetln(pth_mlb, &used, def_lab_file);
          if (qr != EOF) {
            (void) sscanf(pth_mlb, "%s%n", c_mlb, &used); // to_dtm
            if (!strcmp((l_inv) ? i_nm : o_nm, c_mlb)) { // FOUND
              // The grid_val accepts NO height label part::
              p_tp = test_lab.u_c_lab.mlb + test_lab.u_c_lab.sepix;
              if (test_lab.u_c_lab.sepix + 2 == test_lab.u_c_lab.h_ix)
                *(test_lab.u_c_lab.mlb + test_lab.u_c_lab.sepix)   = '\0';
              else {
                *(test_lab.u_c_lab.mlb + test_lab.u_c_lab.sepix)   = '_';
                *(test_lab.u_c_lab.mlb + test_lab.u_c_lab.h_ix -1) = '\0';
              }
              htr_const->inv   = l_inv;
              *htr_lab         = test_lab;
              sh_dtm           = o_hdtm;
              p_tp             = pth_mlb + used;
              (void) sscanf(p_tp, "%d%n", &tr_type, &used);
              p_tp            += used;
              htr_const->LAT0  = sgetg(p_tp, &g_tpd, &used, "nt");
              p_tp            += used;
              htr_const->LON0  = sgetg(p_tp, &g_tpd, &used, "nt");
              qr               = fgetln(pth_mlb, &used, def_lab_file); // params
              if (qr != EOF) {
                switch (tr_type) {
                case 1: // dh_table
                  (void) sscanf(pth_mlb, "%s%n", dh_table_name, &used);
                break;
                case 2: // Constant
                  htr_const->a1 = sgetg(pth_mlb, &g_tpd, &used, "m");
                break;
                case 3: // Linear
                  htr_const->M0  = sgetg(pth_mlb, &g_tpd, &used, "m");
                  p_tp           = pth_mlb + used;
                  htr_const->N0  = sgetg(p_tp, &g_tpd, &used, "m");
                  p_tp          += used;
                  htr_const->a1  = sgetg(p_tp, &g_tpd, &used, "m");
                  p_tp          += used;
                  htr_const->a2  = sgetg(p_tp, &g_tpd, &used, "sx");
                  p_tp          += used;
                  htr_const->a3  = sgetg(p_tp, &g_tpd, &used, "sx");
                  o_hdtm         = (htr_const->inv) ? i_hdtm : o_hdtm;
                }
              }
            } else {
              used = fgetln(pth_mlb, &used, def_lab_file); // params
            }
            if (tr_type > 0) // reference
             (void) fgets(dh_tr_info, 127, def_lab_file);
            else
             (void) fgets(pth_mlb, 127, def_lab_file);
          }
        } else {
          qr = fgetln(pth_mlb, &used, def_lab_file);
          used = fgetln(pth_mlb, &used, def_lab_file); // params
          (void) fgets(dh_tr_info, 127, def_lab_file); // info
        }
      } else test_lab.u_c_lab.lab_type = STP_LAB;
    } while (test_lab.u_c_lab.lab_type != STP_LAB && tr_type <= 0);
  }

  if (tr_type < 0) {
    htr_lab->u_c_lab.h_dtm    = 0;
    htr_lab->u_c_lab.lab_type = ILL_LAB;
  }

  return(tr_type);
}

