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

/* Copyright (c) 2009, National Space Institude, Danish Technical */
/* University and Kort-og Matrikelstyrelsen, Denmark              */
/* All rights reserved.                                           */

/* This is unpublished proprietary source code of National Space  */
/* Institude DTU.  This copyright claim does not indicate an      */
/* intention of publishing this code.                             */

#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    "KmsFncs.h"
#include    "trthread.h"

/*             Actions                     */
#define PTG    2   /* Proj     -> Geo      */
#define GTP    6   /* Geo      -> Proj     */
#define IDT    9   /* Ident                */

#define ILL   -1   /* Illegal           */

/* TEST
#define  DEBUGTRANS 
TEST */

int                       etrs89_trans(
/*___________________________________*/
char                     *i_lab_str,
char                     *o_lab_str,
double                    N_in,
double                    E_in,
double                   *X,
double                   *Y,
FILE                     *tr_error
)
{

  /* etrs89_trans is a tiny version of gd_trans having        */
  /* transformations between projections in etrs89 Datum ONLY */
  /* crt and ellipsoidal heights are not included             */
  /* leagal i_lab_str and o_lab_str are                       */
  /* (zz is two digits for zone):                             */
  /*   "geo", "utmzz", "dktm1", "dktm2", "dktm3", "dktm4",    */
  /*   "kp2000j", "kp2000s", "kp2000b", "kfmr", "fke",        */
  /*   "etrs-tmzz", "ETRS-TMzz", "etrs-lcc", "ETRS-LCC",      */
  /*   "etrs-laea", "ETRS-LAEA"                               */

  static THREAD_SAFE  char                 si_lab_str[MLBLNG];
  static THREAD_SAFE  char                 so_lab_str[MLBLNG];
  static THREAD_SAFE  union geo_lab        i_lab;
  static THREAD_SAFE  union geo_lab        o_lab;

  static THREAD_SAFE  short                init = 0;
  static THREAD_SAFE  short                sta, stp, ptp, ste;

  static THREAD_SAFE  struct coord_lab    *i_clb, *o_clb;

  struct cstm_lng {
    char cstm_str[16];
    int  str_lng;
  };

  struct cstm_lng *p_cstm_lst;
  struct cstm_lng  cstm_lst[] = {{"geo", 3}, {"utm", 3},
    {"dktm1", 5}, {"dktm2", 5}, {"dktm3", 5}, {"dktm4", 5},
    {"kp2000j", 7}, {"kp2000s", 7}, {"kp2000b", 7}, {"kfmr", 4},
    {"etrs-tm", 7}, {"ETRS-TM", 7}, {"fke", 3},
    {"etrs-lcc", 8}, {"ETRS-LCC", 8},
    {"etrs-laea", 9}, {"ETRS-LAEA", 9}, {"", 0}};

  struct coord_lab           *o_wclb;
  char                        mlb_str[MLBLNG];
  int                         action;
  int                         ies = 0, res;
  double                      N, E;

  /*            ENGSAGER'S TRANSFORMATION PRODUCTION LINE (ETPL)   */
  /*             <--UP-HILL--><--DOWN-HILL-->    */
  /* cstm:       PRJ   GEO         GEO   PRJ     */
  /* cstm:       3-7    2           2    3-7     */
  /* io_nr:       2     3           3     2      */
  /* action:        PTG               GTP        */
  /* action:         2                 6         */
  /* nmb_sequence at datum shift = both up-hill and down-hill */
  /* strt_nr =     *io_nr_tab[i_cstm]                         */
  /* stop_no = 8 - *io_nr_tab[o_cstm]                         */
 
  /* nmb_seq, no dshift, increasing io_nmb = only up-hill     */
  /* strt_nr =     *io_nr_tab[i_cstm]                         */
  /* stop_no =     *io_nr_tab[o_cstm] - 1                     */

  /* nmb_seq, no dshift, decreasing io_nmb = only down-hill   */
  /* strt_nr = 9 - *io_nr_tab[i_cstm]                         */
  /* stop_no = 8 - *io_nr_tab[o_cstm]                         */

  short                 io_nr_tab [] = {
    /* 0 -  7, regular transf. */ -1, 4, 3, 2, 2, 2, 2, 2,
  };

  short                 stop_ell [] = {
    /* 0 -  7, regular transf. */  0, 0, 0, 1, 1, 1, 1, 0,
  };

  /* sequence of transformations : (part of ETPL)               */
  /* lev == 3 : transform coordinates                           */


  /* Init of tables */
  /*________________*/
  if (init == 0 ||
      (strcmp(i_lab_str, si_lab_str) != 0) ||
      (strcmp(o_lab_str, so_lab_str) != 0)) {

    i_clb = &(i_lab.u_c_lab);
    o_clb = &(o_lab.u_c_lab);

    /* Test for leagal systems */
    for (p_cstm_lst = cstm_lst, res = 1;
         p_cstm_lst->str_lng && res > 0; ++ p_cstm_lst) {
      if (strncmp(i_lab_str,
          p_cstm_lst->cstm_str, p_cstm_lst->str_lng) == 0) {
        res = 0;
      }
    }
    if (res) return((tr_error==NULL) ? TRF_ILLEG_ :
                     t_status(tr_error, "",
                     "etrs89_trans(unintelligible input_str)", TRF_ILLEG_));

    for (p_cstm_lst = cstm_lst, res = 1;
         p_cstm_lst->str_lng && res > 0; ++ p_cstm_lst) {
      if (strncmp(o_lab_str,
          p_cstm_lst->cstm_str, p_cstm_lst->str_lng) == 0) {
        res = 0;
      }
    }
    if (res) return((tr_error==NULL) ? TRF_ILLEG_ :
                     t_status(tr_error, "",
                     "etrs89_trans(unintelligible output_str)", TRF_ILLEG_));

    (void) sprintf(mlb_str, "%s_etrs89", i_lab_str);
    res = conv_lab(mlb_str, &i_lab, "");
    if (res != CRD_LAB) return((tr_error==NULL) ? TRF_ILLEG_ :
               t_status(tr_error, "",
               "etrs89_trans(unintelligible input_str)", TRF_ILLEG_));
    (void) strcpy(si_lab_str, i_lab_str);

    (void) sprintf(mlb_str, "%s_etrs89", o_lab_str);
    res = conv_lab(mlb_str, &o_lab, "");
    if (res != CRD_LAB) return((tr_error==NULL) ? TRF_ILLEG_ :
               t_status(tr_error, "",
               "etrs89_trans(unintelligible input_str)", TRF_ILLEG_));
    (void) strcpy(so_lab_str, o_lab_str);

    init =  1;

    /* init of transformation  (b_lev == 3) */
    o_wclb  = &(o_lab.u_c_lab);

#ifdef DEBUGTRANS
  (void) lord_debug(0, LORD("\n*etrs89_trans inlab = %s  outlab = (%s) %s;"), i_clb->mlb, o_wclb->mlb, o_clb->mlb);
#endif
    /* reg->reg */
    sta = *(io_nr_tab + i_clb->cstm);
    stp = *(io_nr_tab + o_wclb->cstm) - (short) 1;
    ptp = (short) (((2<i_clb->cstm) && (2<o_wclb->cstm)) ? 1 : 0);
    if (ptp) {
      stp = -stp + 7;
      ste = (*(stop_ell + i_clb->cstm) && *(stop_ell + o_clb->cstm)) ? 1 : 2;
    } else {
      if (sta > stp + 1) {
        /* e.g.: (CRT=3)->(GEO=2)->(PRJ=1): */
        /*       (GTC, PTG) changed to (CTG, GTP)   */
        sta = -sta + (short) IDT;
        stp = -stp + 7;
      }
      ste = 1;
    }

    /* test for identical systems */
    if (i_clb->ch_sum == o_wclb->ch_sum) {
      sta = IDT;
      stp = IDT;
    }

#ifdef DEBUGTRANS
  (void) lord_debug(0, LORD("\n*sta,sto,ptp = %d, %d, %d;"), sta, stp, ptp);
#endif

  } /* end of init of tables */

  N      = N_in;
  E      = E_in;

  for (action = sta, res = 0;
       action <= stp && res >= TRF_TOLLE_; action++) {
/*
(void) fprintf(stdout, "\n*etrs89_trans :  action  %d;", action);
*/
    switch(action) {
    case  PTG: /* PRJ (or GEO) -> GEO */
      /*______________________________*/
#ifdef DEBUGTRANS
  (void) lord_debug(0, LORD("\n*case 2: PRJ -> GEO"));
  (void) lord_debug(0, LORD("   %s;"), (i_lab.u_c_lab).mlb);
#endif
      ies = ptg(&i_lab, (int) ste, N, E, &N, &E, "etrs89_trans", tr_error);
      if (ptp) action += 3;
      break;

    case  GTP: /* GEO -> PRJ (or GEO) */
      /*______________________________*/
#ifdef DEBUGTRANS
  (void) lord_debug(0, LORD("\n*case 6: GEO -> PRJ"));
  (void) lord_debug(0, LORD("   %s;"), (o_lab.u_c_lab).mlb);
#endif
      ies = ptg(&o_lab, (int) (-ste), N, E, &N, &E, "etrs89_trans", tr_error);
      break;

    case  IDT: /* No action, ident, ok */
      ies  = 0;
      break;

    case  ILL: /* Illegal */
    default:
      *X  = *Y = 0.0;
      return((tr_error==NULL) ? TRF_ILLEG_ :
              t_status(tr_error, "",
              "etrs89_trans(unintelligible labels)", TRF_ILLEG_));
      break;
    } /* end switch(action) */

    if (res > ies) res = ies;
#ifdef DEBUGTRANS
  (void) lord_debug(0, LORD("*    res = %d;"), res);
#endif
  } /* action LOOP */

  /* return of transformation */
  *X = N;
  *Y = E;
  return(res);

  /* Undef macros */

#undef PTG
#undef GTP
#undef IDT
#undef ILL

}
