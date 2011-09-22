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
 


/* ng_trans  ver 2006          # page 1    3 Oct 2006 13 55 */


/* Copyright (c) 2006 Kort-og Matrikelstyrelsen, Denmark   */
/* and Danish National Space Center: All rights reserved.  */

/* This is unpublished proprietary source code of Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does  */
/* not indicate an intention of publishing this code.      */

/* 
#define  DEBUGNGTRANS
*/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include "geo_lab.h"
#include "geoid_d.h"
#include "trthread.h"

/* ACTION NUMBERS */

#define  IDT    0
#define  NTG    1
#define  GTN    2
#define  PTG    3
#define  GTP    4

int                    ng_trans(
/*___________________________*/
union geo_lab       *in_lab_u,
union geo_lab       *outlab_u,
double               N_in,
double               E_in,
double               H_in,
double              *Nout,
double              *Eout,
double              *Hout,
char                *usertxt,
FILE                *tr_error
)

{

#include        "conv_lab.h"
#include        "ptg.h"
#include        "tab3d_i.h"
#include        "tab3d_val.h"
#include        "t_status.h"

  static THREAD_SAFE  int                 in_chsum = 0L;
  static THREAD_SAFE  int                 outchsum = 0L;
  static THREAD_SAFE  int                 init = 0;
  static THREAD_SAFE  struct mtab3d_str   nadg_gr96_tab;
  static THREAD_SAFE  union  geo_lab      w_lab;


/* ng_trans  ver 2006          # page 2    3 Oct 2006 13 55 */


  char                     in_cs[MLBLNG], outcs[MLBLNG];
  char                    *p_str, err_str[512];

  int                      in_gr, outgr, outnr = 0, ng_z;
  int                      nst, gst, act;
  int                      res = 0, ies = 0;
  double                   N, E, H, N_g, E_g, val[3];

  struct coord_lab        *in_lab = &(in_lab_u->u_c_lab);
  struct coord_lab        *outlab = &(outlab_u->u_c_lab);

  struct nr_mlb {
    short        trgr;
    short        trnr;
    short        chrs;
    char        *s_lab;
  } *pml;

  static THREAD_SAFE  struct nr_mlb    mlab[] = {
    /*  0 */ { 0,  0,  10,   "geo_nad83g"},
    /*  1 */ { 0,  1,   8,   "geo_gr96"},
    /*  2 */ { 1,  2,   6,   "*nad83g"},
    /*  3 */ { 1,  3,   4,   "*gr96"},
    /*stop*/ {-1, -1,   0,   ""}
  };

  struct act_nst {
    short     action;
    short     nstate;
  };

  /* Start values: *ptab->row, in_nr->col */
  static THREAD_SAFE  struct act_nst     *ptab;
  static THREAD_SAFE  int                 in_nr;
  static THREAD_SAFE  int                 ng_w;

  /* Action/state table */
  static THREAD_SAFE  struct act_nst ngtab[] = {
    /* nadg  = geo_nad83g: 0   */
    /* input   nadg    ggr96    *nadg    *gr96 */
    /* state :    0        1        2        3 */
    /*  0 */ {IDT,0}, {GTN,0}, {PTG,0}, {PTG,1},
    /* gr96  = geo_gr96:   1   */
    /* input   nadg    ggr96    *nadg    *gr96 */
    /* state :    0        1        2        3 */
    /*  1 */ {NTG,1}, {IDT,1}, {PTG,0}, {PTG,1},
    /* *nadg = *nad83g:    2   */
    /* input   nadg    ggr96    *nadg    *gr96 */
    /* state :    0        1        2        3 */
    /*  2 */ {GTP,2}, {GTN,0}, {IDT,2}, {PTG,1},
    /* *gr96 = *gr96:     3   */
    /* input   nadg    ggr96    *nadg    *gr96 */
    /* state :    0        1        2        3 */
    /*  3 */ {NTG,1}, {GTP,3}, {PTG,0}, {IDT,3},
  };


/* ng_trans  ver 2006          # page 3    3 Oct 2006 13 55 */


  if (in_lab == NULL) {
    if (init) (void) fclose(nadg_gr96_tab.table_u.fd);
    init               = 0;
    nadg_gr96_tab.init = 0;
    return(0);
  }
  /* Test io-labels */
  if (in_lab->lab_type == CRD_LAB && outlab->lab_type == CRD_LAB) {

    if (init == 0) {
      char   err_str[128];
      res  = tab3d_i("nadggr96.06", T3D_LAB, &nadg_gr96_tab, err_str);
      /* State/action table size and width */
      ng_z = sizeof(ngtab)/sizeof(struct act_nst);
      ng_w = (int) sqrt(1.0000001*ng_z);
      init = (res > 0) && (ng_z == ng_w*ng_w);
      (void) conv_lab("geo_nad83g", &w_lab, "");
      if (!init)
        return(t_status(
               tr_error, "", "ng_trans(table)", res));
    }

    /* Check i/o labels, init of actual transf. systems */
    if (in_chsum != in_lab->ch_sum || outchsum != outlab->ch_sum) {

      /* Out-system */
      /*____________*/
      /* Datum actions */
      (void) strcpy(outcs, outlab->mlb);
      *(outcs + outlab->sepix) = '_';

      /* Group and number */
      for (pml = mlab, outgr = -1; pml->trgr < 1; pml++)
        if (!strncmp(pml->s_lab, outcs, pml->chrs)) {
          outgr = pml->trgr;
          outnr = pml->trnr;
#ifdef DEBUGNGTRANS
(void) printf("\n*OUT  grp = %4d    nr = %4d   navn = %s;",
pml->trgr, pml->trnr, pml->s_lab);
#endif
          break;
        }
      /* Group and number DTM only */
      if (outgr == -1) {
        p_str = outcs + outlab->sepix +1;
        for ( ; pml->trgr != -1; pml++)
          if (!strncmp(pml->s_lab +1, p_str, pml->chrs)) {
            outgr = pml->trgr;
            outnr = pml->trnr;
#ifdef DEBUGNGTRANS
(void) printf("\n*OUT  grp = %4d    nr = %4d   navn = %s;",
pml->trgr, pml->trnr, pml->s_lab);
#endif
            break;
          }
      }


/* ng_trans  ver 2006          # page 4    3 Oct 2006 13 55 */


      /* In-system */
      /*___________*/

      /* Datum actions */
      (void) strcpy(in_cs, in_lab->mlb);
      *(in_cs + in_lab->sepix) = '_';

      /* Group and number */
      for (pml = mlab, in_gr = -1; pml->trgr < 1; pml++)
        if (!strncmp(pml->s_lab, in_cs, pml->chrs)) {
          in_gr = pml->trgr;
          in_nr = pml->trnr;
#ifdef DEBUGNGTRANS
(void) printf("\n*IN   grp = %4d    nr = %4d   navn = %s;",
pml->trgr, pml->trnr, pml->s_lab);
#endif
          break;
        }
      /* Group and number DTM only */
      if (in_gr == -1) {
        p_str = in_cs + in_lab->sepix +1;
        for ( ; pml->trgr != -1; pml++)
          if (!strncmp(pml->s_lab +1, p_str, pml->chrs)) {
            in_gr = pml->trgr;
            in_nr = pml->trnr;
#ifdef DEBUGNGTRANS
(void) printf("\n*IN   grp = %4d    nr = %4d   navn = %s;",
pml->trgr, pml->trnr, pml->s_lab);
#endif
            break;
          }
      }

      /* Save check-sums */
      in_chsum = in_lab->ch_sum;
      outchsum = outlab->ch_sum;

      /* Test identical labels */
      if (in_chsum == outchsum) in_nr = outnr = 0;

#ifdef  DEBUGNGTRANS
(void) fprintf(stdout, "\n* in = %s, out = %s;\n", in_cs, outcs);
(void) fprintf(stdout, "\n*ng_trans in_nr     %d  outnr      %d;",
in_nr, outnr);
#endif

      if (outnr == -1 || in_nr == -1) {
        return(t_status(
               tr_error, "", "ng_trans(table)", TRF_ILLEG_));
      }

      ptab = ngtab + ng_w*outnr;  /* output row */
    } /* End of init actions */


/* ng_trans  ver 2006          # page 5    3 Oct 2006 13 55 */


    /* transformation module */

    nst = in_nr;               /* input col  */
    N_g = N_in;
    E_g = E_in;
    N   = N_in;
    E   = E_in;
    H   = H_in;

    do {
      gst = nst;
      act = (ptab+nst)->action;
      nst = (ptab+nst)->nstate;

#ifdef  DEBUGNGTRANS
(void) fprintf(stdout, "\n*NG_TRANS ACTION  %d:   %d -> %d;",
act, gst, nst);
#endif

      switch(act) {

      case NTG: /* geo_nad83g -> geo_gr96 */
        ies = tab3d_val(&w_lab, &nadg_gr96_tab,
                        N_g, E_g, val, err_str);
#ifdef  DEBUGNGTRANS
(void) fprintf(stdout, "\n*NG_TRANS ACTION  NTG:   %f, %f, res=%d;",
N_g*180.0 / M_PI, E_g*180.0 / M_PI, ies);
#endif
        if (ies >= 0) {
          N  = N_g + *(val   ) * 0.0000001;
          E  = E_g + *(val +1) * 0.0000001;
          H +=       *(val +2);
        } else
        if (ies == TRF_AREA_) {
          N   = N_g + 0.0000001424;
          E   = E_g + 0.0000001111;
          ies = 0;
        } else {
          if (ies < res) res = ies;
          if (tr_error != NULL)
             (void) t_status(tr_error, "", "\nng_trans", ies,
                             "sx", "", N_g, E_g, 0.0, 0.0);
        }
#ifdef  DEBUGNGTRANS
(void) fprintf(stdout, "\n*NG_TRANS ACTION  NTG:   %f, %f, res=%d;",
N*180.0 / M_PI, E*180.0 / M_PI, ies);
#endif
        break;

      case GTN: /* geo_gr96 -> geo_nad83g */
        ies = tab3d_val(&w_lab, &nadg_gr96_tab,
                        N_g, E_g, val, err_str);
#ifdef  DEBUGNGTRANS
(void) fprintf(stdout, "\n*NG_TRANS ACTION  GTN1:   %f, %f, res=%d;",
*val, *(val+1), ies);
#endif
        if (ies >= 0) {
          N = N_g - *(val   ) * 0.0000001;
          E = E_g - *(val +1) * 0.0000001;
          ies = tab3d_val(&w_lab, &nadg_gr96_tab, N, E, val, err_str);
#ifdef  DEBUGNGTRANS
(void) fprintf(stdout, "\n*NG_TRANS ACTION  GTN2:   %f, %f, res=%d;",
*val, *(val+1), ies);
#endif
          if (ies >= 0) {
            N = N_g - *(val   ) * 0.0000001;
            E = E_g - *(val +1) * 0.0000001;
            ies = tab3d_val(&w_lab, &nadg_gr96_tab, N, E, val, err_str);
#ifdef  DEBUGNGTRANS
(void) fprintf(stdout, "\n*NG_TRANS ACTION  GTN2:   %f, %f, res=%d;",
*val, *(val+1), ies);
#endif
            if (ies >= 0) {
              N  = N_g - *(val   ) * 0.0000001;
              E  = E_g - *(val +1) * 0.0000001;
              H -=       *(val +2);
            } else
            if (ies == TRF_AREA_) {
              N   = N_g - 0.0000001424;
              E   = E_g - 0.0000001111;
              ies = 0;
            } else {
              if (ies < res) res = ies;
              res = ies;
              if (tr_error != NULL)
                 (void) t_status(tr_error, "", "\nng_trans", ies,
                                 "sx", "", N_g, E_g, 0.0, 0.0);
            }
          } else
          if (ies == TRF_AREA_) {
            N   = N_g - 0.0000001424;
            E   = E_g - 0.0000001111;
            ies = 0;
          } else {
            if (ies < res) res = ies;
            res = ies;
            if (tr_error != NULL)
               (void) t_status(tr_error, "", "\nng_trans", ies,
                               "sx", "", N_g, E_g, 0.0, 0.0);
          }
        } else
        if (ies == TRF_AREA_) {
          N   = N_g - 0.0000001424;
          E   = E_g - 0.0000001111;
          ies = 0;
        } else {
          if (ies < res) res = ies;
          res = ies;
          if (tr_error != NULL)
             (void) t_status(tr_error, "", "\nng_trans", ies,
                             "sx", "", N_g, E_g, 0.0, 0.0);
        }
        break;


/* ng_trans  ver 2006          # page 6    3 Oct 2006 13 55 */


      case  PTG: /* PRJ -> GEO */
        /*______________________________*/
        ies = ptg(in_lab_u, +1, N, E, &N, &E,
                  "\nng_trans error:", tr_error);
        N_g = N;
        E_g = E;
#ifdef  DEBUGNGTRANS
(void) fprintf(stdout, "\n*NG_TRANS ACTION  PTG:  %s, %f, %f, res=%d;",
in_lab->mlb, N*180.0 / M_PI, E*180.0 / M_PI, ies);
#endif
        if (ies < res) res = ies;
        break;

      case  GTP: /* GEO -> PRJ */
        /*______________________________*/
        ies = ptg(outlab_u, -1, N, E, &N, &E,
                  "\nng_trans error:", tr_error);
#ifdef  DEBUGNGTRANS
(void) fprintf(stdout, "\n*NG_TRANS ACTION  GTP:   %s, res=%d;",
outlab->mlb, ies);
#endif
        if (ies < res) res = ies;
        break;

      case IDT: /* ident, no action */
        break;

      default: /* programme error */
        return(t_status(
               tr_error, "", "ng_trans(prog error)", TRF_ILLEG_));
      } /* end switch(action) */
      if (ies < res) res = ies;

    } while (nst != gst && res >= TRF_TOLLE_);
  }
  else {
    return(t_status(
           tr_error, "", "ng_trans(i/o labels)", TRF_ILLEG_));
  }

  /* Return coord and result */
  *Nout = N;
  *Eout = E;
  *Hout = H;
  return (res);

  /* UNDEFINE ACTION NUMBERS */
#undef  IDT
#undef  NTG
#undef  GTN
#undef  PTG
#undef  GTP

}


