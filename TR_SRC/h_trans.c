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
 


/* h_trans  ver 2004.01          # page 1    24 Jan 2004 11 57 */


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


int                      h_trans(
/*_____________________________*/
union geo_lab           *i_lab,
short                    oh_dtm,
double                   B_in,
double                   L_in,
double                  *dh,
char                    *tr_str
)
{

/*
#include              "erad.h"
*/
#include              "conv_lab.h"
#include              "fgetlhtx.h"
#include              "fgetln_kms.h"
#include              "i_tabdir_file.h"
#include              "s_status.h"
#include              "set_dtm.h"
#include              "sgetg.h"

  extern THREAD_SAFE FILE               *def_lab_file;
  extern THREAD_SAFE size_t              init_prj_pos, init_hth_pos;


/* h_trans  ver 2004.01          # page 2    24 Jan 2004 11 57 */


  struct h_trc_str {
    union geo_lab ref_lab;
    short oh_dtm, tr_type;
    double LAT0, LON0, M0, N0;
    double a1, a2, a3;};

  static THREAD_SAFE  struct h_trc_str     h_trc;
  static THREAD_SAFE  short                sh_dtm = 0, inv = 0;
  struct coord_lab           *ic_lab = &i_lab->u_c_lab;
  struct coord_lab           *rc_lab = &(h_trc.ref_lab.u_c_lab);
  char                        pth_mlb[512], c_mlb[MLBLNG], *p_tp;
  int                         res = 1, qr, used;
  long                        pos = 0;
  short                       p_no;
  char                        i_nm[MLBLNG], p_nm[MLBLNG], e_nm[MLBLNG];
  char                        o_nm[MLBLNG];
  struct typ_dec              g_tpd;
  struct dsh_str              trp;


  /* basic transformation formula UELN                              */
  /* h_EVRFxx = h_DVRyy + dh                                        */
  /* dLAT     = B_in - LAT0                                         */
  /* dLON     = L_in - LON0                                         */
  /* dh = a1 - a2* M0 *(dLAT) - a3* N0 * (dLON) cos(LAT)            */

  /* LAT0, LON0, M0, N0, coord and Curvatures of ref stn            */



/* h_trans  ver 2004.01          # page 3    24 Jan 2004 11 57 */


  if (ic_lab->lab_type != CRD_LAB)
    return(s_status(tr_str, "h_trans(i labels error)", TRF_ILLEG_));

  if (sh_dtm == 0) {
    if (def_lab_file == NULL || init_prj_pos == 0) {
      (void) i_tabdir_file(3, "", &res, pth_mlb);
      if (res) {
        (void) fprintf(stdout, "\n*** def_lab.txt: %s %s;\n", pth_mlb,
          (res == -1) ? "NOT FOUND" :
          (res == -2) ? "NOT lab definition file" :
                        "Content not accepted");
        return (-2);
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

    rc_lab->h_dtm = 0;
  }
  else (void) fseek(def_lab_file, (long) init_hth_pos, SEEK_SET);

  if (((inv) ? (h_trc.oh_dtm != ic_lab->h_dtm || rc_lab->h_dtm != oh_dtm)
             : (h_trc.oh_dtm != oh_dtm || rc_lab->h_dtm != ic_lab->h_dtm)) ||
      rc_lab->p_rgn != ic_lab->p_rgn ||
      rc_lab->cstm != ic_lab->cstm || rc_lab->datum != ic_lab->datum) {
    sh_dtm = -1;
    /* find datum name from datum number */
    pos = ftell(def_lab_file);
    qr  = set_dtm(ic_lab->h_dtm, i_nm, &p_no, p_nm, e_nm, &trp);
    qr  = set_dtm(oh_dtm, o_nm, &p_no, p_nm, e_nm, &trp);
    (void) fseek(def_lab_file, (long) pos, SEEK_SET);

    do {
      pos = ftell(def_lab_file);
      qr  = conv_lab("input", &h_trc.ref_lab, def_lab_file, "");
      if (qr == 1) {
        (void) fseek(def_lab_file, (long) pos, SEEK_SET);
        (void) fgetln_kms(pth_mlb, &used, def_lab_file); //now:skip lab line

        inv = rc_lab->h_dtm == oh_dtm;
        if (rc_lab->p_rgn == ic_lab->p_rgn &&
            rc_lab->cstm  == ic_lab->cstm &&
            rc_lab->datum == ic_lab->datum &&
           (rc_lab->h_dtm == ic_lab->h_dtm || inv)) {
          qr = fgetln_kms(pth_mlb, &used, def_lab_file);
          if (qr != EOF) {
            (void) sscanf(pth_mlb, "%s%n", c_mlb, &used); // to_dtm
            if (!strcmp((inv) ? i_nm : o_nm, c_mlb)) { // FOUND
              sh_dtm = oh_dtm;
              p_tp   = pth_mlb + used;
              (void) sscanf(p_tp, "%hd%n", &h_trc.tr_type, &used);
              p_tp       += used;
              h_trc.LAT0  = sgetg(p_tp, &g_tpd, &used, "nt");
              p_tp       += used;
              h_trc.LON0  = sgetg(p_tp, &g_tpd, &used, "nt");
              qr          = fgetln_kms(pth_mlb, &used, def_lab_file); // params
              if (qr != EOF) {
                h_trc.M0     = sgetg(pth_mlb, &g_tpd, &used, "m");
                p_tp         = pth_mlb + used;
                h_trc.N0     = sgetg(p_tp, &g_tpd, &used, "m");
                p_tp        += used;
                h_trc.a1     = sgetg(p_tp, &g_tpd, &used, "m");
                p_tp        += used;
                h_trc.a2     = sgetg(p_tp, &g_tpd, &used, "sx");
                p_tp        += used;
                h_trc.a3     = sgetg(p_tp, &g_tpd, &used, "sx");
                h_trc.oh_dtm = (inv) ? ic_lab->h_dtm : oh_dtm;
              }
            } else {
              used = fgetln_kms(pth_mlb, &used, def_lab_file); // params
            }
            used = fgetln_kms(pth_mlb, &used, def_lab_file); // reference
          }
        }
      }
    } while (qr != STP_LAB && sh_dtm != oh_dtm);
  }

  /* H_datum check */
  if (sh_dtm != oh_dtm)
      return(s_status(tr_str, "h_trans(i labels error)", TRF_ILLEG_));

  *dh = h_trc.a1 + h_trc.a2 * h_trc.M0 * (B_in - h_trc.LAT0)
      + h_trc.a3 * h_trc.N0 * (L_in - h_trc.LON0) * cos(B_in);
  /* dh = a1 - a2* M0 *(dLAT) - a3* N0 * (dLON) cos(LAT)            */

  if (inv) *dh = -*dh;
  return(0);

}

