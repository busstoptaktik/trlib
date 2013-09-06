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

/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    "KmsFncs.h"
#include    "trthread.h"

/*             Actions                     */
#define PRE    0   /* crt_???? -> geoE???? */
#define NTP    1   /* Non_reg  -> Proj     */
#define PTG    2   /* Proj     -> Geo      */
#define GTC    3   /* Geo      -> Cart     */
#define CTC    4   /* Cart     -> Cart     */
#define CTG    5   /* Cart     -> Geo      */
#define GTP    6   /* Geo      -> Proj     */
#define PTN    7   /* Proj     -> Non_reg  */
#define ANT    8   /* geoE???? -> crt_???? */
#define IDT    9   /* Ident                */

#define ILL   -1   /* Illegal           */

#define REG_REG   0 /* Regular->Regular */
#define NON_REG   1 /* Non-reg->Regular */
#define REG_NON   2 /* Regular->Non-reg */
#define NON_NON   3 /* Non-reg->Non-reg */

/* TEST
#define  DEBUGGDTRANS
TEST */

int                        gd_trans(
/*________________________________*/
struct coord_lab            *i_lab,
struct coord_lab            *o_lab,
double                    N_in,
double                    E_in,
double                    H_in,
double                   *X,
double                   *Y,
double                   *Z,
double                   *ogh,
char                      req_gh,
struct mgde_str          *grid_tab,
char                     *usertxt,
FILE                     *tr_error
)
{

  static THREAD_SAFE  int      (*dfb_trf)(
    struct coord_lab      *in_lab,
    struct coord_lab      *outlab,
    double              N,    double   E,    double   H,
    double             *Nout, double  *Eout, double  *Hout,
    char               *usertxt,
    FILE               *tr_error
  );

  char                        err_txt[1024];
                                       /* grid systems geoid/table */
  static THREAD_SAFE  struct coord_lab    H0_lab, H1_lab;
  static THREAD_SAFE  struct coord_lab    H2_lab, H3_lab; /* grid cstm dh*/
  static THREAD_SAFE  struct coord_lab   *i_Rlab;  /* beg REG of ETPL  */
  static THREAD_SAFE  struct coord_lab   *G_Rlab;  /* end REG of E geoid */
  static THREAD_SAFE  struct coord_lab   *T_Rlab;  /* end REG of ETPL dh */
  static THREAD_SAFE  struct coord_lab   *O_Rlab;  /* end REG of ETPL tr */
  static THREAD_SAFE  struct coord_lab   *i_Nlab;  /* beg NON of ETPL  */
  static THREAD_SAFE  struct coord_lab   *G_Nlab;  /* end NON of E geoid */
  static THREAD_SAFE  struct coord_lab   *T_Nlab;  /* end NON of ETPL dh */
  static THREAD_SAFE  struct coord_lab   *O_Nlab;  /* end NON of ETPL tr */
  static THREAD_SAFE  struct coord_lab    t_lab;   /* non-reg gateway */
  static THREAD_SAFE  struct coord_lab    g_lab;   /* geo_* PRE/ANT   */

  static THREAD_SAFE  struct mgde_str *s_grid_tab = NULL;
  static THREAD_SAFE  struct mgde_str  h_grid_tab;
  static THREAD_SAFE  struct htr_c_str htr_const;

  static THREAD_SAFE  int              i_chsum = 0;
  static THREAD_SAFE  int              o_chsum = 0, b_lev, s_lev;
  static THREAD_SAFE  short            init = 0, iEh_req =0, oEh_req =0;
  static THREAD_SAFE  short            s_req_gh = 0, ghr = 0;
  static THREAD_SAFE  short            s_req_dh = 0, s_req_tv = 0;
  static THREAD_SAFE  short            sta[4], stp[4], ptp[4];
  static THREAD_SAFE  char             i_sep, nonp_i[4];
  static THREAD_SAFE  char             o_sep, nonp_o[4];

  static THREAD_SAFE  struct coord_lab    *i_clb, *o_clb;
  struct coord_lab           *H_clb  = &(H0_lab);
  struct coord_lab           *H1_clb = &(H1_lab);
  struct coord_lab           *t_clb  = &(t_lab);

  struct coord_lab              *i_wlab = NULL;  /* begin REG of ETPL  */
  struct coord_lab              *o_wlab = NULL;  /* end   REG of ETPL  */
  struct coord_lab             **o_nlab = NULL;  /* end   REG of ETPL  */
  struct coord_lab             **o_rlab = NULL;  /* end   REG of ETPL  */
  struct coord_lab               w_lab;   /* work label         */
  struct coord_lab           *w_oclb;
  union rgn_un                rgn_DE, rgn_EE, rgn_GR;

  short                       i_rgn, o_rgn;
  int                         action, lev, R_N, rs = 0, RES, RGH = 0;
  int                         ies = 0, res, dsh = 0, iEhr, oEhr, req_th = 0;
  char                        dstr[128];
  double                      N, E, H, gh, igh, dh = 0.0, Nh = 0.0;
  double                      NN, EE, HH, iEh, oEh = 0.0, th = 0.0;

  /* ++++++++++++++        WARNING          ++++++++++++++ */
  /* call with non std geoids may give erroneous Heights   */
  /* this is not an error but intended by the users call   */
  /* ++++++++++++++        WARNING          ++++++++++++++ */

  /* Nonregular Geogr is handled as Nonregular Proj */

  /*            ENGSAGER'S TRANSFORMATION PRODUCTION LINE (ETPL)   */
  /*             <-------UP-HILL------> <------DOWN-HILL------>    */
  /* cstm: crt_  NON   PRJ   GEO   CRT   CRT   GEO   PRJ   NON   crt_ */
  /* cstm:  0    8-15  3-7    2     1     1     2    3-7   8-15    0  */
  /* io_nr: 4     1     2     3     4     4     3     2     1      4  */
  /* action:  PRE   NTP   PTG   GTC   CTC   CTG   GTP   PTN    ANT    */
  /* action:   0     1     2     3     4     5     6     7      8     */
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
    /* 0 -  7, regular transf. */ -1, 4, 3, 2,  2, 2, 2, 2,
    /* 8 - 15, non-reg transf. */  1, 1, 1, 1,  1, 1, 1, 1
  };

  /* The STD geoid in DK gives the Normal Height DVR90. The     */
  /* Height is converted to DNN iff requested                   */

  /* sequence of transformations : (part of ETPL)               */
  /* lev == 0 : get Geoid Height : gh                           */
  /*     or   : get Table Value  : tv                           */
  /* lev == 1 : do. : swop to other coordsys (EE_geo_../geo_eur)*/
  /* lev == 2 : get Datum Shift of Height dtms : dh             */
  /* lev == 3 : transform coordinates                           */

  /* Geoid Height gh is not transformed at Datum Shifts of Height*/

  /*                     Geoid requested ::  (allso -g param)   */
  /* crt_          :: is treated as sepch == E                  */
  /* H             :: is treated as sepch == N                  */
  /* in_sepch_out dsh  ghr H_in  igh  gh  H_in   Nh    Hout     */
  /*  _        _   .       =0.0  0.0                   =0.0     */
  /*  _        N   .       =0.0  0.0                   =0.0     */
  /*  _        E   .    +  =0.0        +  +igh                  */

  /*  N        _   .             0.0                   =0.0     */
  /*  N        N   .             0.0                   =H_in    */
  /*  N        E   .    +              +  +igh         =H_tr    */

  /*  E        _   .                                   =0.0     */
  /*  E        N   .    +        0.0   +        Htr-gh =Nh      */
  /*  E        E   .             0.0                   =H_tr    */

  /*  gh : +       :: calculate gh in system t_lab              */
  /*  igh          :: calculate gh in system i_lab              */

  /* Asume  H_in is Ellipsoidal height !!!!!               */
  /* then   H_in = h = H + N = Nh + igh                    */
  /* The transformed to t_lab is also an Ellisoidal height */
  /*        H_tr = h = H + N = Nh + ogh                    */
  /* Therefor Nh = H_in -igh = H_tr - ogh                  */
  /* which gives         igh = H_in - H_tr +ogh            */
  /* and      Nh = H_in -igh                               */

  /* Asume  H_in is Normal height                          */
  /* treat  H_in as a Ellipsoidal heigh as above           */
  /* but Nh should be replaced by Nh_false                 */
  /* and      Nh = H_in                                    */

  /* req_gh  > 0    :: calculate gh in o_lab               */
  /* req_gh == 0    :: calculate gh in o_lab if needed     */
  /* req_gh  < 0    :: DO NOT get gh                       */
  /* H_in/(H_in+igh) transformed to o_lab                  */
  /*                     ogh = H_tr - Nh                   */

  /* In prg is iEh replacing H_in (CRT causes troubles)    */

  if (i_lab == NULL) {
    (void) ng_trans(NULL, NULL, 0.0, 0.0, 0.0, &N, &E, &H, "", NULL);
    i_clb           = NULL;
    if (h_grid_tab.init != 0) {
      h_grid_tab.init = 0;
      (void) fclose(h_grid_tab.table_u[0].fd);
    }
    if (grid_tab != NULL)
        if (grid_tab->init != 0) (void) geoid_c(grid_tab, 0, NULL);
    return(0);
  }

  *X = *Y = *Z = 0.0;

  /* Init of tables */
  /*________________*/
  if (i_clb      != &(i_lab) ||
      i_chsum    !=  (i_lab).ch_tsum ||
      s_grid_tab != grid_tab ||
      ((s_req_tv) ? (req_gh == 0 ||
                     strcmp(grid_tab->table_u[0].clb, H_clb->mlb))
                  : (req_gh  != s_req_gh ||
                     o_clb   != &(o_lab) ||
                     o_chsum != (o_lab).ch_tsum ||
                     ((req_gh > 0) &&
                      grid_tab->table_u[0].lab_type != GDE_LAB)))) {

    if (grid_tab->init != 0 && grid_tab->init != 1)
        grid_tab->init = 0;
    i_chsum    = o_chsum = 0;
    i_clb      = &(i_lab);
    s_grid_tab = grid_tab;
    s_req_gh   = req_gh;
    o_clb      = &(o_lab);
    s_req_tv   = (grid_tab->init == 1) &&
                  grid_tab->table_u[0].lab_type != (short) GDE_LAB;
    s_lev      = (s_req_tv) ? 0 : 3;
    /* initial setting of ghr for setup of sequences */
    ghr        = (! (int) s_req_tv) || !grid_tab->init;
    if (s_req_tv) s_req_tv = grid_tab->table_u[0].lab_type;
    i_rgn      = i_clb->p_rgn;
    i_sep      = (i_clb->cstm != 1)
               ? *(i_clb->mlb+i_clb->sepix) : (char) 'E';
    o_rgn      = o_clb->p_rgn;
    o_sep      = (o_clb->cstm != 1)
               ? *(o_clb->mlb+o_clb->sepix) : (char) 'E';
    if (i_sep == '\0' || i_sep == 'L') i_sep = '_';
    if (o_sep == '\0' || o_sep == 'L') o_sep = '_';

    /* Test for leagal datum and systems */
    if (i_clb->cstm < 1 || 16 < i_clb->cstm ||
        (6 <= i_clb->datum && i_clb->datum <= 8))
      return((tr_error==NULL) ? TRF_ILLEG_ :
              t_status(tr_error, "",
              "gd_trans(unintelligible input label)", TRF_ILLEG_));
    if ((!s_req_tv) && (o_clb->cstm < 1 || 16 < o_clb->cstm ||
                        (6 <= o_clb->datum && o_clb->datum <= 8)))
      return((tr_error==NULL) ? TRF_ILLEG_ :
              t_status(tr_error, "",
              "gd_trans(unintelligible output label)", TRF_ILLEG_));

    /* Test for regular systems */
    if (i_clb->imit == 0 && 7 < i_clb->cstm)
      return((tr_error==NULL) ? TRF_ILLEG_ :
              t_status(tr_error, "",
              "gd_trans(ill. regular system)", TRF_ILLEG_));
    if ((!s_req_tv) && o_clb->imit == 0 && 7 < o_clb->cstm)
      return((tr_error==NULL) ? TRF_ILLEG_ :
              t_status(tr_error, "",
              "gd_trans(ill. regular system)", TRF_ILLEG_));

    (void) strcpy(rgn_DE.prfx, "DE");
    (void) strcpy(rgn_EE.prfx, "EE");
    (void) strcpy(rgn_GR.prfx, "GR");
    ies        = (i_rgn == rgn_EE.r_nr[0]) || (o_rgn == rgn_EE.r_nr[0]);
    if (!ies) {
      if (i_rgn == 0 && o_rgn != 0) i_rgn = o_rgn;
      else
      if (o_rgn == 0 && i_rgn != 0) o_rgn = i_rgn;
    }

    /* Loop for init of geoid/table and transformation */
    for (b_lev = (req_gh == -1) ? 2 : 0; b_lev <= s_lev; b_lev ++) {
      /* b_lev == 0 : init call_table or special_geoid */
      /*            : b_lev increased last in loop */
      /* b_lev == 1 : skipped : for swop of systems to EE */
      /* b_lev == 2 : init table or special geoid */
      /* b_lev == 3 : transformation */
      sta[b_lev] = stp[b_lev] = 0;
      switch (b_lev) {
      case 0:
        if (s_req_tv) {
          (void) conv_w_crd(grid_tab->table_u[0].clb, &H0_lab);
          if (128 <= H_clb->datum && H_clb->datum <= 129) {
            H1_lab = H0_lab;
            (void) strcpy(H1_clb->mlb, "s34");
            G_Nlab = &H1_lab;
          }
          else G_Nlab = &H0_lab;
        } else {
          if (i_clb->imit != FHMASK && o_clb->imit != FHMASK &&
            i_rgn != rgn_DE.r_nr[0] && o_rgn != rgn_DE.r_nr[0]) {
            if (grid_tab->init &&
                (!strcmp(grid_tab->table_u[0].mlb, "fehmarngeoid10.bin")
              || !strcmp(grid_tab->table_u[0].mlb, "fbeltgeoid.bin")))
               (void) geoid_c(grid_tab, 0, NULL);
            res    = i_clb->datum == 51 || o_clb->datum == 51 || ies;
            i_wlab = (res) ? &H1_lab : &H0_lab; /* swop sys for EE_ */
            o_wlab = (res) ? &H0_lab : &H1_lab;
            if (i_rgn == rgn_GR.r_nr[0] || o_rgn == rgn_GR.r_nr[0])
              (void) strcpy(dstr, "geoEgr96");
            else
            if (!ies && (i_rgn != 0)) {
              char     rgn_p[8], rgn_name[24];
              if (conv_rgn(i_rgn, rgn_p, rgn_name) > 0)
                 (void) sprintf(dstr, "%-2s_%s",
                                rgn_p, "geoEeuref89");
            } else
              (void) strcpy(dstr, "geoEeuref89");
            (void) conv_w_crd(dstr, i_wlab);
            (void) conv_w_crd("EE_geoEeuref89", o_wlab);
          } else {
            if (i_clb->imit == FHMASK || o_clb->imit == FHMASK) {
              /* USE: fehmarngeoid10.bin */
              /* because: dvr90g.01 is out by 1cm */
              if (grid_tab->init &&
                  strcmp(grid_tab->table_u[0].mlb, "fehmarngeoid10.bin"))
                  (void) geoid_c(grid_tab, 0, NULL);
              res = (grid_tab->init) ? 0
                  : geoid_i("fehmarngeoid10.bin",
                            GDE_LAB, grid_tab, err_txt);
            } else { /* region DE */
              if (grid_tab->init &&
                  strcmp(grid_tab->table_u[0].mlb, "fbeltgeoid.bin"))
                  (void) geoid_c(grid_tab, 0, NULL);
              res = (grid_tab->init) ? 0
                  : geoid_i("fbeltgeoid.bin",
                            GDE_LAB, grid_tab, err_txt);
            }
            if (res < 0) {
              i_chsum = -i_clb->ch_tsum;
              return((tr_error==NULL) ? TRF_GEOID_ :
              t_status(tr_error, "",
                       "gd_trans(unintelligible labels)", TRF_GEOID_));
            }
            (void) conv_w_crd(grid_tab->table_u[0].clb, &H0_lab);
          }
          G_Nlab = &H0_lab;
        }
        G_Rlab = &H0_lab;
        o_nlab = &G_Nlab;
        o_rlab = &G_Rlab;
        break;

	  case 2:
        htr_const.inv = 0;
        if (i_clb->h_dtm == o_clb->h_dtm) s_req_dh = 0;
        else
        if (o_clb->h_dtm == 0) s_req_dh = 0;
        else
        if (i_clb->h_dtm == 0) s_req_dh = -7;
        else {
          s_req_dh = htr_init(i_clb, o_clb, &H2_lab,
                              &htr_const, H3_lab.mlb, dstr);
          if (s_req_dh < 0) { /* ILLEGAL */
            return((tr_error==NULL) ? TRF_ILLEG_ :
                    t_status(tr_error, "",
                    "gd_trans(ill. kote TRF)", HTRF_ILLEG_));
          }
        }
        switch(s_req_dh) {
        case 1: // DH_TAB height transformation
          if (h_grid_tab.init == 1 &&
             strcmp(H3_lab.mlb, h_grid_tab.table_u[0].mlb) != 0)
            (void) geoid_c(&h_grid_tab, 0, NULL);

          if (h_grid_tab.init == 0) {
            res = geoid_i(H3_lab.mlb,
                          DHH_LAB, &h_grid_tab, usertxt);
            if (res < 0) return(res);
          }
           
          if (128 <= H2_lab.datum
                  && H2_lab.datum <= 129) {
            H3_lab = H2_lab;
            (void) strcpy(H3_lab.mlb, "s34");
            T_Nlab = &H3_lab;
          }
          else T_Nlab = &H2_lab;
          break;
        case 2: // CONSTANT height transformation
          H2_lab = *i_lab; // ident trf
          break;
        case 3: // LINEAR height transformation
          T_Nlab = &H2_lab;
          break;
        default: // IDT
          sta[b_lev] = 1;
          stp[b_lev] = 0;
        }
        T_Rlab = &H2_lab;
        o_nlab = &T_Nlab;
        o_rlab = &T_Rlab;
        break;

      case 3:
        O_Nlab = o_lab;
        O_Rlab = o_lab;
        o_nlab = &O_Nlab;
        o_rlab = &O_Rlab;
        break;
      }
      w_oclb  = &((*o_rlab));

      if (sta[b_lev] != stp[b_lev]) continue;

      /* regular/non-regular decisions */
      /*  in       regular    non-reg   */
      /*  out */
      /* regular     0           1      */
      /* non-reg     2           3      */
      R_N           = ((w_oclb->imit == 0) ? 0 : 2)
                    + (( i_clb->imit == 0) ? 0 : 1);
      /* FE uses utm29 => nonp_i/o must be zero */
      /* when     geo  => nonp_i/o must be one  */
      nonp_o[b_lev] = (char) (w_oclb->imit == EDMASK
                           || w_oclb->imit == DKMASK
                           || w_oclb->imit == NGMASK
                           || w_oclb->imit == FHMASK);
      nonp_i[b_lev] = (char) ( i_clb->imit == EDMASK
                           ||  i_clb->imit == DKMASK
                           ||  i_clb->imit == NGMASK
                           ||  i_clb->imit == FHMASK);
      switch (R_N) {
      case REG_REG: /* regular -> regular */
        rs = 0;
        break;
      case NON_REG: /* non-regular -> regular */
        if (i_clb->datum != w_oclb->datum) rs = i_clb->imit;
        else { // same datum
          rs            = 0;
          R_N           = 0;
          nonp_i[b_lev] = nonp_o[b_lev] = 0;
        }
        break;
      case REG_NON: /* regular -> non-regular */
        if (i_clb->datum != w_oclb->datum) rs = w_oclb->imit;
        else { // same datum
          rs            = 0;
          R_N           = 0;
          nonp_i[b_lev] = nonp_o[b_lev] = 0;
        }
        break;

      case NON_NON: /* non-regular -> non-regular */
        if ((i_clb->imit == NGMASK && w_oclb->imit == NGMASK)) {
          rs            = 0;
          R_N           = 0;
          nonp_i[b_lev] = nonp_o[b_lev] = 0;
        } else
        if (i_clb->imit == EDMASK  ||
            i_clb->imit == w_oclb->imit) rs = w_oclb->imit;
        else
        if (w_oclb->imit == EDMASK) rs = i_clb->imit;
        else {
          (void) sprintf(dstr, "gd_trans %s -> %s illegal",
                 i_clb->mlb, w_oclb->mlb);
          return((tr_error==NULL) ? TRF_ILLEG_ :
                  t_status(tr_error, usertxt, dstr, TRF_ILLEG_));
        }
        break;
      } /* end switch(R_N) */

#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("\n*gd_trans (lev:%d) inlab = %s  outlab = (%s) %s, R_N = %d;"), b_lev, i_clb->mlb, w_oclb->mlb, o_clb->mlb, R_N);
#endif


#ifdef BULK_OF_TREASURE_HERE
      /* how to insert a non-regular group */
      /* --MASK - names should be set in geo_lab.h    */
      /* The gateway-system should be a rectangular   */
      /* system and need only be set here. The trans- */
      /* fornation function name is set here, too.    */
      /* Set the geo label ?_lab in the init block. */
    case ??MASK: /* ??_trans */
      t_lab   = &?_lab;
      dfb_trf = ??_trans;
      break;
#endif

      /* regular/non-regular selections */
      /*________________________________*/
      switch (rs) {

      case DKMASK: /* dk_trans */
      case EDMASK: /* dk_trans */
        init = (short) ((conv_w_crd("DK_tcgeo_ed50", &t_lab)==CRD_LAB)
                     && (conv_w_crd("DK_geo_ed50",  &g_lab)==CRD_LAB));
        dfb_trf = dk_trans;
        break;

      case FEMASK: /* fe_trans */
        init    = (short) conv_w_crd("FO_utm29_etrs89", &t_lab)==CRD_LAB;
        /* utm29_etrs89 => nonp_i/o must be zero */
        if ((R_N == REG_NON || R_N == NON_NON) && w_oclb->cstm == 1) {
          (void) sprintf(dstr, "geoE%s", w_oclb->mlb+4);
          init &= (short) conv_w_crd(dstr, &g_lab)==CRD_LAB;
          if (i_clb->cstm == 1) {
            /* WARN: ONLY POSSIBLE when no gh */
            (void) sprintf(dstr, "geoE%s", i_clb->mlb+4);
            init &= (short) conv_w_crd(dstr, &t_lab)==CRD_LAB;
            if (req_gh) return((tr_error==NULL) ? TRF_ILLEG_ :
              t_status(tr_error, usertxt,
                       "geoid NOT posible", TRF_ILLEG_));
          }
        } else
        if ((R_N == NON_REG || R_N == NON_NON) && i_clb->cstm == 1) {
          (void) sprintf(dstr, "geoE%s", i_clb->mlb+4);
          init &= (short) conv_w_crd(dstr, &g_lab)==CRD_LAB;
        }
        else g_lab = t_lab;
        dfb_trf = fe_trans;
        break;

      case NGMASK: /* ng_trans */
        init    = (short) conv_w_crd("GR_geo_gr96", &t_lab) == CRD_LAB;
        if ((R_N == NON_REG && i_clb->datum != 62) ||
            (R_N == REG_NON && w_oclb->datum != 62)) g_lab = t_lab;
        else
        init   &= (short) conv_w_crd("GR_geo_nad83g", &g_lab)==CRD_LAB;
        dfb_trf = ng_trans;
        break;

      case EEMASK: /* ee_trans */
        init = (short) conv_w_crd("EE_utm35_euref89", &t_lab)==CRD_LAB;
        dfb_trf = ee_trans;
        break;

      case FHMASK: /* fh_trans */
        init = (short) conv_w_crd("DK_geo_feh10", &t_lab) == CRD_LAB;
        /* Here o_clb must be used: to get the correct limits! */
        i_clb->date  = (i_clb->datum == 33 || o_clb->datum == 33)
                     ? 0.0 : 200.0;
        w_oclb->date = i_clb->date;
        t_clb->date  = i_clb->date;
        g_lab        = t_lab;
        dfb_trf      = fh_trans;
        break;

      case 0: /* all regular */
        init =  1;
        break;

      default: /* error report */
        return(TRF_PROGR_);
      } /* end rs switch */

#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("rs = %d, nonp = %d, %d, t,g=%s,%s;"), rs, nonp_i[b_lev], nonp_o[b_lev], t_clb->mlb, (g_lab).mlb);
#endif
      switch(R_N) {
      case REG_REG : /* reg->reg */
        i_Rlab     = i_lab;
        dsh        = (i_clb->datum != w_oclb->datum);
        sta[b_lev] = *(io_nr_tab + i_clb->cstm);
        stp[b_lev] = *(io_nr_tab + w_oclb->cstm) - (short) 1;
        if (dsh) {
          ptp[b_lev] = 0;
          stp[b_lev] = -stp[b_lev] + (short) PTN;
        } else {
          ptp[b_lev] = (short) ((2<i_clb->cstm) && (2<w_oclb->cstm));
          if (ptp[b_lev]) stp[b_lev] = -stp[b_lev] + (short) PTN;
          else {
            if (sta[b_lev] > stp[b_lev] + 1) {
              /* e.g.: (CRT=3)->(GEO=2)->(PRJ=1): */
              /*       (GTC, PTG) changed to (CTG, GTP)   */
              sta[b_lev] = -sta[b_lev] + (short) IDT;
              stp[b_lev] = -stp[b_lev] + (short) PTN;
            }
          }
        }
        if (b_lev == 0 && ghr) {
          sta[1] = sta[0];
          ptp[1] = ptp[0];
          stp[1] = (short) ((i_clb->datum!=H1_clb->datum) ? CTG : PTG);
          if (sta[1] > stp[1] + 1) {
            sta[1] = -sta[1] + (short) IDT;
            stp[1] = -stp[1] + (short) PTN;
          }
        }
        break;

      case NON_REG : /* non-reg input */
        /* o_nlab[lev]: 0 : G_Nlab, 2 : T_Nlab, 3 : O_Nlab(o_lab) */
        *o_nlab    = &t_lab;
        i_Nlab     = i_lab;
        i_Rlab     = &t_lab;
        dsh        = (t_clb->datum != w_oclb->datum);
        sta[b_lev] =  NTP;
        stp[b_lev] = *(io_nr_tab + w_oclb->cstm) - (short) 1;
        if ((i_clb->cstm == 1) &&
            ((i_clb->imit == FEMASK) || nonp_i[b_lev])) {
          /* crt_ed50, crt_nad83g, crt_feh10 */
          i_Nlab     = &g_lab;
          sta[b_lev] = PRE;
        }

        if (dsh) {
          ptp[b_lev] = 0;
          stp[b_lev] = -stp[b_lev] + (short) PTN;
        } else {
          ptp[b_lev] = (short) ((2 < t_clb->cstm) && (2 < w_oclb->cstm)
                           &&   (t_clb->ch_sum !=     w_oclb->ch_sum));
          if (ptp[b_lev]) stp[b_lev] = -stp[b_lev] + (short) PTN;
          if (nonp_i[b_lev]) {
            if (w_oclb->cstm != 1) O_Nlab = *o_rlab;
          }
        }
        if (b_lev == 0 && ghr) {
          sta[1] = sta[0];
          ptp[1] = ptp[0];
          stp[1] = CTG;
          if (sta[1] > stp[1] + 1) {
            sta[1] = -sta[1] + (short) IDT;
            stp[1] = -stp[1] + (short) PTN;
          }
        }
        break;

      case REG_NON : /* non-reg output */
        /* o_rlab[lev]: 0 : G_Nlab, 2 : T_Nlab, 3 : O_Nlab(o_lab) */
        i_Rlab     = i_lab;               /* regular sys is input */
        *o_rlab    = &t_lab;              /* reg_s feeds gate-sys */
        i_Nlab     = &t_lab;
        dsh        = (i_clb->datum != t_clb->datum);
        sta[b_lev] = *(io_nr_tab + i_clb->cstm);
        stp[b_lev] = PTN;
        if (w_oclb->cstm == 1 && b_lev == 3 &&
            (w_oclb->imit == FEMASK || nonp_o[b_lev])) {
          /* o_nlab[lev]: 0 : G_Nlab, 2 : T_Nlab, 3 : O_Nlab */
          *o_nlab    = &g_lab;
          stp[b_lev] = ANT;
        }
        if (dsh) ptp[b_lev] = 0;
        else {
          ptp[b_lev] = (short) ((2 < i_clb->cstm) && (2 < t_clb->cstm)
                           &&   (i_clb->ch_sum !=     t_clb->ch_sum));
          if (!ptp[b_lev]) {
            sta[b_lev] = -sta[b_lev] + (short) IDT;
            if (nonp_o[b_lev]) {
              if (i_clb->cstm != 1) i_Nlab = i_lab;
            }
/* else */
/* +++ HER kunne ch_sum i_clb imod t_lab => stop ved NTP */
          }
          nonp_i[b_lev] = nonp_o[b_lev] = 0;
        }
        if (b_lev == 0 && ghr) {
          sta[1] = sta[0];
          ptp[1] = ptp[0];
          stp[1] = (short) ((i_clb->datum!=H1_clb->datum) ? CTG : PTG);
          if (sta[1] > stp[1] + 1) {
            sta[1] = -sta[1] + (short) PTN;
            stp[1] = -stp[1] + (short) PTN;
          }
        }
        break;

      case NON_NON : /* non-reg input/output */
        /* t_lab: 0 : G_Nlab, 2 : T_Nlab, 3 : O_Nlab(o_lab) */
        i_Nlab     = i_lab;
        dsh        = 0;
        sta[b_lev] = NTP;
        stp[b_lev] = NTP;

        if ((w_oclb->cstm == 1 /* crt_* */) &&
            ((w_oclb->imit == FEMASK) || nonp_o[b_lev])) {
          /* o_nlab[lev]: 0 : G_Nlab, 2 : T_Nlab, 3 : O_Nlab */
          *o_nlab    = &g_lab;
          sta[b_lev] = PTN;
          stp[b_lev] = ANT;
          if (i_clb->cstm == 1 && i_clb->datum != w_oclb->datum) {
            *o_rlab       = &t_lab;     /* reg_s feeds gate-sys */
            i_Nlab        = &t_lab;
            nonp_o[b_lev] = 1;
            sta[b_lev]    = CTG;
          }
        } else
        if ((i_clb->cstm == 1) &&
            ((i_clb->imit == FEMASK) || nonp_i[b_lev])) {
          /* crt_ed50, crt_fodtm, crt_nad83g, crt_feh10 */
          /* NB.:: crt skulle includeres i dk_trans, ng_trans */
          i_Nlab     = &g_lab;
          sta[b_lev] = PRE;
        }
        if (b_lev == 0 && ghr) {
          sta[1] = sta[0];
          ptp[1] = ptp[0];
          stp[1] = CTG;
          if (sta[1] > stp[1] + 1) {
            sta[1] = -sta[1] + (short) IDT;
            stp[1] = -stp[1] + (short) PTN;
          }
        }
        break;
      } /* end switch R_N */

      /* jumper might be set for webmrc when to/from wgs84/etrs89: */
      /* jumper might be set from PTG/GTC to GTP   (ptp[lev])      */
      /* or set startpoint   from GTC to GTP       (sta[lev])      */

      /* test for identical systems */
      if (b_lev == 0) {
        if (i_clb->ch_sum == H_clb->ch_sum ||
               (ghr == 0 && s_req_tv == 0)) {
          sta[b_lev] = IDT;
          stp[b_lev] = IDT;
        }
        if (ghr) {
          if (i_clb->ch_sum == H1_clb->ch_sum) {
            sta[1] = IDT;
            stp[1] = IDT;
          }
        }
        ++ b_lev;
      } else {
        if (i_clb->ch_sum == w_oclb->ch_sum) {
          sta[b_lev] = IDT;
          stp[b_lev] = IDT;
        } else
        if (i_clb->ch_sum != w_oclb->ch_sum &&
            i_clb->region != w_oclb->region) {
          if (i_clb->region != 0) {
            R_N           = i_clb->region;
            res           = i_clb->ch_hsum;
            ies           = i_clb->ch_tsum;
            i_clb->region = 0;
            rs            = labchsum((union geo_lab*) i_lab, &i_clb->ch_sum);
            if (rs == w_oclb->ch_sum) {
              sta[b_lev] = IDT;
              stp[b_lev] = IDT;
            }
            i_clb->region  = R_N;
            i_clb->ch_hsum = res;
            i_clb->ch_tsum = ies;
          } else
          if (w_oclb->region != 0) {
            R_N            = w_oclb->region;
            ies            = w_oclb->ch_sum;
            res            = w_oclb->ch_hsum;
            ies            = w_oclb->ch_tsum;
            w_oclb->region = 0;
            rs             = labchsum((union geo_lab*) w_oclb,
                                      &w_oclb->ch_sum);
            if (i_clb->ch_sum == rs) {
              sta[b_lev] = IDT;
              stp[b_lev] = IDT;
            }
            w_oclb->region  = R_N;
            w_oclb->ch_hsum = res;
            w_oclb->ch_tsum = ies;
          }
        }
      }
    } /* end of b_lev loop */

#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("sta,sto,ptp(0) = %d, %d, %d;"), sta[0], stp[0], ptp[0]);
	(void) lord_debug(0, LORD("sta,sto,ptp(1) = %d, %d, %d;"), sta[1], stp[1], ptp[1]);
	(void) lord_debug(0, LORD("sta,sto,ptp(2) = %d, %d, %d;"), sta[2], stp[2], ptp[2]);
	(void) lord_debug(0, LORD("sta,sto,ptp(3) = %d, %d, %d;"), sta[3], stp[3], ptp[3]);
#endif
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("ghr :: i/o_sep = %c %d   %c %d;"), i_sep, i_sep, o_sep, o_sep);
#endif
    /* final setting of ghr for transformation */
    /* dsh is showing iff datum_shift[level=3] */
    switch (i_sep) {
    case '_':
    case 'N':
    case 'H':
      ghr = (short) ((dsh || o_sep == 'E') ? 1 : 0);
      break;
    case 'E':
      ghr = (short) ((dsh || o_sep == 'H' || o_sep == 'N') ? 1 : 0);
    } /* switch i_sep */
    if (req_gh > 0) ghr = 1;
    else
    if (req_gh < 0) ghr = 0;
    if ( s_req_tv ) ghr = 0;

    iEh_req = ghr && (i_clb->cstm == (short) 1 /* CRT */);
    oEh_req = (short) (((stp[3] == (short) CTC ||
                         stp[3] == (short) GTC) && ghr)
            ? 2 : ((ghr) ? 1 : 0));

    /* reset region */
    if (i_rgn != i_clb->p_rgn) i_rgn = i_clb->p_rgn;
    if (o_rgn != o_clb->p_rgn) o_rgn = o_clb->p_rgn;

    /* save checksums */
    i_chsum = i_clb->ch_tsum;
    o_chsum = o_clb->ch_tsum;

    if (ghr && !grid_tab->init) {
      res = geoid_i("STD", GDE_LAB, grid_tab, err_txt);
      if (res < 0) {
        i_chsum = -i_clb->ch_tsum;
        init    = (short) res;
        return((tr_error==NULL) ? TRF_GEOID_ :
                t_status(tr_error, "",
                "gd_trans(unintelligible labels)", TRF_GEOID_));
      }
    }

    if (!init) {
      i_chsum = -i_clb->ch_tsum;
      return((tr_error==NULL) ? TRF_PROGR_ :
              t_status(tr_error, "",
              "gd_trans(unintelligible labels)", TRF_PROGR_));
    }

  } /* end of init of tables */

  if (i_sep == '_') H_in = 0.0;
  igh    = gh = *ogh = 0.0;
  iEhr   = iEh_req;
  oEhr   = 0;
  N      = N_in;    E    = E_in;    H = iEh = H_in;
  RES    = 0;
  b_lev  = (ghr || s_req_tv) ? 0 : ((s_req_dh) ? 2 : 3);
  for (lev = b_lev; lev <= s_lev && RES >= TRF_TOLLE_; lev++) {

    switch (lev) {
    case 0:
      i_wlab = G_Nlab;
      o_wlab = G_Rlab;
      break;
    case 1: /* swop geo_system (DK <-> EE) for grid value */
      w_lab   = H0_lab;
      H0_lab  = H1_lab;
      H1_lab  = w_lab;
      ies     = sta[0];
      sta[0]  = sta[1];
      sta[1]  = (short) ies;
      ies     = stp[0];
      stp[0]  = stp[1];
      stp[1]  = (short) ies;
      o_wlab  = &H0_lab;
      lev     = 0;
	break;
    case 2:
      i_wlab = T_Nlab;
      o_wlab = T_Rlab;
      break;
    case 3:
      oEhr   = oEh_req;
      i_wlab = O_Nlab;
      o_wlab = O_Rlab;
      break;
    }
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("lev %d, nonp %d, %d, Ehr %d %d;"), lev, nonp_i[lev], nonp_o[lev], iEhr, oEhr);
#endif

    for (action = sta[lev], res = 0;
         action <= stp[lev] && res >= TRF_TOLLE_; action++) {

      switch(action) {
   
      case  PRE: /* crt_* -> geoE* */
        /*_____________________*/
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("case 0: CRT -> GEO"));
	(void) lord_debug(0, LORD("   %s;"), (g_lab).mlb);
	(void) lord_debug(0, LORD(" %7.2f  %7.2f   %7.2f;"), N, E, H);
#endif
        ies = gtc(&g_lab, -1, N, E, H, &N, &E, &H, usertxt, tr_error);
        if (iEhr) {
          iEh  = H;
          iEhr = 0;
        }
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("-> %7.6f  %7.6f   %7.2f;"), N/M_PI*180.0, E/M_PI*180.0, H);
	(void) lord_debug(0, LORD("-> %7.2f  %7.2f   %7.2f, iEh=%f;"), N, E, H, iEh);
#endif
        break;

      case  NTP: /* NON -> PRJ (or GEO) */
        /*______________________________*/
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("case 1: NON -> PRJ"));
	(void) lord_debug(0, LORD("   %s -> %s;"), (i_Nlab).mlb, (i_wlab).mlb);
	(void) lord_debug(0, LORD(" %7.2f  %7.2f   %7.2f;"), N, E, H);
#endif
        ies = (*dfb_trf)(i_Nlab, i_wlab,
                         N, E, H, &N, &E, &HH, usertxt, tr_error);
        if (lev == 3 && ((*dfb_trf) == ng_trans)) {
          req_th = -1;
          th     = HH - H;
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("case 1: NON -> PRJ: th = %f"), th);
#endif
        }
        if (nonp_i[b_lev]) action ++;
        break;

      case  PTG: /* PRJ (or GEO) -> GEO */
        /*______________________________*/
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("\n*case 2: PRJ -> GEO"));
	(void) lord_debug(0, LORD("   %s;"), (i_Rlab).mlb);
#endif
        ies = ptg(i_Rlab, +1, N, E, &N, &E, usertxt, tr_error);
        if (ptp[lev]) action += 3;
        break;

   
      case  GTC: /* GEO -> CRT */
        /*_____________________*/
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("\n*case 3: GEO -> CRT"));
	(void) lord_debug(0, LORD("   %s;"), (i_Rlab).mlb);
	(void) lord_debug(0, LORD("\n*  %10.5f  %10.5f    %8.5f  %8.5f;"), N, E, N*180.0/M_PI, E*180.0/M_PI);
#endif
        if (req_th < 0 && stp[3] == GTC) { /* dh from 3D table_trf */
          H      += th;
          req_th  = 0;
        }
        ies = gtc(i_Rlab, +1, N, E, H, &N, &E, &H, usertxt, tr_error);
        break;

      case  CTC: /* CRT -> CRT */
        /*______________________*/
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("case 4: CRT -> CRT"));
	(void) lord_debug(0, LORD("   %s -> %s;"), (i_Rlab).mlb, (o_wlab).mlb);
#endif
        if (iEhr) {
          (void) gtc(i_lab, -1, N, E, H, &NN, &EE, &HH, "", NULL);
          iEh  = HH;
          iEhr = 0;
        }
        ies = ctc(i_Rlab, o_wlab, N, E, H, &N, &E, &H,
                  usertxt, tr_error);
        break;
   
      case  CTG: /* CRT -> GEO */
        /*_____________________*/
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("case 5: CRT -> GEO"));
	(void) lord_debug(0, LORD("   %s;"), (o_wlab).mlb);
#endif
        ies = gtc(o_wlab, -1, N, E, H, &N, &E, &H, usertxt, tr_error);
        break;

      case  GTP: /* GEO -> PRJ (or GEO) */
        /*______________________________*/
        if (!nonp_o[lev]) {
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("case 6: GEO -> PRJ"));
	(void) lord_debug(0, LORD("   %s;"), (o_wlab).mlb);
#endif
          ies = ptg(o_wlab, -1, N, E, &N, &E, usertxt, tr_error);
        }
        break;

      case  PTN: /* PRJ (or GEO) -> NON */
        /*______________________________*/
#ifdef DEBUGGDTRANS
  (void) fprintf(stdout, "case 7: PRJ -> NON");
  (void) fprintf(stdout, "   %s -> %s;",
         (i_Nlab).mlb, (i_wlab).mlb);
  (void) fprintf(stdout, "  %10.5f  %10.5f    %8.5f  %8.5f  %6.2f;",
  N, E, N*180.0/M_PI, E*180.0/M_PI, H);
#endif
        ies = (*dfb_trf)(i_Nlab, i_wlab,
                         N, E, H, &N, &E, &HH, usertxt, tr_error);
        if (lev == 3 && ((*dfb_trf) == ng_trans)) {
          req_th = -1;
          th     = HH - H;
#ifdef DEBUGGDTRANS
(void) fprintf(stdout, "case 7: PRJ -> NON: th = %f", th);
#endif
        }
        break;
   
      case  ANT: /* geoE* -> crt_* */
        /*_____________________*/
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("case 8: GEO -> CRT"));
	(void) lord_debug(0, LORD("   %s;"), (g_lab).mlb);
	(void) lord_debug(0, LORD(" %10.5f  %10.5f    %8.5f  %8.5f  %6.2f;"), N, E, N*180.0/M_PI, E*180.0/M_PI, H);
#endif
        if (oEhr == 1) {
          oEh  = H;
          oEhr = 0;
        }
        if (req_th < 0) { /* dh from 3D table_trf */
          H      += th;
          req_th  = 0;
        }
        ies = gtc(&g_lab, +1, N, E, H, &N, &E, &H, usertxt, tr_error);
        break;

      case  IDT: /* No action, ident, ok */
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("case 9: IDT, oEhr = %d, lev = %d"), oEhr, lev);
#endif
        if (oEhr) {
          oEh  = Nh + igh;
          oEhr = 0;
        }
        ies  = 0;
        break;

      case  ILL: /* Illegal */
        *X  = *Y = *Z = 0.0;
        return((tr_error==NULL) ? TRF_ILLEG_ :
                t_status(tr_error, "",
                "gd_trans(unintelligible labels)", TRF_ILLEG_));

      default:
        *X = *Y = *Z = 0.0;
        return((tr_error==NULL) ? TRF_ILLEG_ :
                t_status(tr_error, "",
                "gd_trans(unintelligible labels)", TRF_ILLEG_));
      } /* end switch(action) */

      if (res > ies) res = ies;
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("*    res = %d;"), res);
#endif

	} /* action LOOP */

    if (RES > res) RES = res;
    switch (lev) {
    case 0:
      if (RES >= TRF_TOLLE_) {
        RGH = grid_val(&H0_lab, grid_tab, N, E, &gh, err_txt);
        if (sta[0] == CTG && iEhr) {
          iEh  = H;
          iEhr = 0;
        }
        if (RGH == 0) {
          if (ghr) {
            ++ lev;
            if (! s_req_dh) ++ lev;
            Nh  = (i_sep != 'E') ? iEh : H - gh;
            igh = iEh - H + gh;
            if (oEhr && stp[3] <= GTC) {
              oEh  = Nh + igh;
              oEhr = 0;
            }
            H   = (i_sep != 'E') ? H_in + igh : H_in;
          }
        } else
        if (RGH < 0) {
          if (RES > RGH) RES = RGH;
          if (tr_error != NULL)
          	(void) lord_error(0, LORD("%s    %s"), err_txt, usertxt);
          gh   = 9999.9;
          *ogh = gh;
        }
        N = N_in;
        E = E_in;
      } else *ogh = gh = 0.0;

      break;

    case 2:
      if (RES >= TRF_TOLLE_) {
        switch (s_req_dh) {
        case 1: /* DVR90 <-> DNN, DVR90 <-> FCSVR10, FVR09 <-> MSL */
          RGH = grid_val(&H2_lab, &h_grid_tab, N, E, &dh, err_txt);
          /* table will give std dh to dvr90 */
          break;
        case 2: /* constant:: DVR90 <-> KN44 */
          RGH = 0;
          dh  = htr_const.a1;
          break;
        case 3: /* linear:: DVR90 <-> EVRF20?? */
          RGH = 0;
          dh  = htr_const.a1 + htr_const.a2 * htr_const.M0 * (N - htr_const.LAT0)
              + htr_const.a3 * htr_const.N0 * (E - htr_const.LON0) * cos(N);
          /* dh = a1 - a2* M0 *(dLAT) - a3* N0 * (dLON) cos(LAT)            */
        }
        if (htr_const.inv > 0) dh = -dh;
        H = (i_sep != 'E') ? H_in + igh : H_in;
        if (RGH == 0) {
          if (dh > 9990.0) dh = 0.0;
          if (htr_const.inv == 0) { /* direct */
            H  += dh;
            Nh += dh;
          }
        } else {
          if (RES > RGH) RES = RGH;
          dh = 0.0;
          if (tr_error != NULL)
			(void) lord_error(0, LORD("%s    %s"), err_txt, usertxt);
        }
      }
      else {
        dh  = 0.0;
        H   = (i_sep != 'E') ? H_in + igh : H_in;
      }
      N   = N_in;
      E   = E_in;
      break;

    case 3:
      if (RES >= TRF_TOLLE_) {
        if (oEhr == 1) {
          oEh  = H;
          oEhr = 0;
        } else
        if (oEhr == 2) {
          (void) gtc(o_wlab, -1, N, E, H, &NN, &EE, &HH, "", NULL);
          oEh  = HH;
          oEhr = 0;
        }
        if (req_gh)    *ogh = oEh - Nh;
        if (o_sep == '_') H = 0.0;
        else
        if (o_sep == 'H' || o_sep == 'N')
            H = (i_sep == 'E') ? Nh
              : ((s_req_dh && htr_const.inv == 0) ? H_in + dh : H_in);
        if (s_req_dh && htr_const.inv) H += dh; /* inverse */
        if (req_th < 0) H += th; /* dh from 3D table_trf */
      }
      break;
    }
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("H_in = %7.3f, H   = %7.3f, iEh = %7.3f, Nh = %7.3f;"), H_in, H, iEh, Nh);
	(void) lord_debug(0, LORD("gh   = %7.3f, igh = %7.3f, oEh = %7.3f, dh = %7.3f, th = %7.3f;") ,gh, igh, oEh, dh, th);
#endif

  } /* lev LOOP */

  /* return of transformation */
  *X = N;
  *Y = E;
  *Z = H;

  return(RES);

  /* Undef macros */

#undef NTP
#undef PTG
#undef GTC
#undef CTC
#undef CTG
#undef GTP
#undef PTN
#undef IDT
#undef ILL

#undef REG_REG
#undef NON_REG
#undef REG_NON
#undef NON_NON

}
