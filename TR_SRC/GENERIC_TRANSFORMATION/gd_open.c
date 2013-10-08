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

struct mgde_str *gd_special_table(char *geoid_name){
	struct mgde_str *self;
	self=malloc(sizeof(struct mgde_str));
	if (!self) {
		lord_error(TR_ALLOCATION_ERROR,LORD("SpecialGeoidTable: Failed to allocate space."));
		return NULL;
	}
	self->init=0;
	if (geoid_i(geoid_name, GDE_LAB, self, NULL) > 0) return self;
	
	geoid_c(self, 0, NULL);
	free(self);
	lord_error(TR_ALLOCATION_ERROR,LORD("Can not open: %s."), geoid_name);
	return NULL;
}


/* This is unpublished proprietary source code  of Danish       */
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

struct gd_state{
	struct coord_lab    H0_lab, H1_lab;
	struct coord_lab    H2_lab, H3_lab; /* grid cstm dh*/
	struct coord_lab   *i_Rlab;  /* beg REG of ETPL  */
	struct coord_lab   *G_Rlab;  /* end REG of E geoid */
	struct coord_lab   *T_Rlab;  /* end REG of ETPL dh */
	struct coord_lab   *O_Rlab;  /* end REG of ETPL tr */
	struct coord_lab   *i_Nlab;  /* beg NON of ETPL  */
	struct coord_lab   *G_Nlab;  /* end NON of E geoid */
	struct coord_lab   *T_Nlab;  /* end NON of ETPL dh */
	struct coord_lab   *O_Nlab;  /* end NON of ETPL tr */
	struct coord_lab    t_lab;   /* non-reg gateway */
	struct coord_lab    g_lab;   /* geo_* PRE/ANT   */

        struct mgde_str   *grid_tab;
	/*struct mgde_str *s_grid_tab = NULL;*/
	struct mgde_str  h_grid_tab;
	struct htr_c_str htr_const;
	int               b_lev, s_lev;
	short            init, iEh_req, oEh_req;
	short            s_req_gh, ghr;
	short            s_req_dh, s_req_tv;
        /*                     START, STOP, ProjTilProj*/
	short            sta[4], stp[4], ptp[4];
	char             i_sep, nonp_i[4];
	char             o_sep, nonp_o[4];
	/*handle to non-standard transformations*/
	int      (*dfb_trf)(
		struct coord_lab      *i_lab,
		struct coord_lab      *o_lab,
		double              N,    double   E,    double   H,
		double             *Nout, double  *Eout, double  *Hout,
		char               *usertxt,
		FILE               *tr_error
	);

	/*struct coord_lab    *i_lab, *o_lab;*/
}
typedef struct gd_state gd_state;

gd_state                      *gd_open(
/*___________________________*/
struct coord_lab            *i_lab,
struct coord_lab            *o_lab,
char                            *special_table
)
{

 
  gd_state         *self;
  short                 io_nr_tab [] = {
    /* 0 -  7, regular transf. */ -1, 4, 3, 2,  2, 2, 2, 2,
    /* 8 - 15, non-reg transf. */  1, 1, 1, 1,  1, 1, 1, 1
  };
  struct coord_lab           *H_clb, *H1_clb, *t_clb ;
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
  
  self=malloc(sizeof(gd_state));
  if (0==self){
	  lord_error(0,TR_ALLOCATION_ERROR,LORD("Failed to allocate memory."));
	  return self;
  }

  self->init = 0;
  self->iEh_req =0;
  self->oEh_req =0;
  self->s_req_gh = 0;
  self->ghr = 0;
  self->s_req_dh = 0;
  self->s_req_tv = 0;
  
  H_clb  = &(self->self->H0_lab);
  H1_clb = &(self->self->H1_lab);
  t_clb  = &(self->t_lab);

 
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


  /* The STD geoid in DK gives the Normal Height DVR90. The     */
  /* Height is converted to DNN iff requested                   */

  /* sequence of transformations : (part of ETPL)               */
  /* lev == 0 : get Geoid Height : gh                           */
  /*     or   : get Table Value  : tv                           */
  /* lev == 1 : do. : swop to other coordsys (EE_geo_../geo_eur)*/
  /* lev == 2 : get Datum Shift of Height dtms : dh             */
  /* lev == 3 : transform coordinates                           */

  /* Geoid Height gh is not transformed at Datum Shifts of Height*/

  /*                     Geoid requested ::  (also -g param)   */
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

  /* Init of tables */
  /*__IS SET ONLY__*/
  if (special_table && *special_table) {
    self->grid_tab = gd_special_table(special_table);
    if (0 == self->grid_tab) {
	    free(self);
	    return NULL;
    }
  }
  
   
    self->b_lev      = (req_gh == -1) ? 2 : 0;
    self->s_req_tv   = (grid_tab->init == 1) &&
                  grid_tab->table_u[0].lab_type != (short) GDE_LAB;
    self->s_lev      = (self->s_req_tv) ? 0 : 3;
    /* initial setting of ghr for setup of sequences */
    self->ghr        = (! (int) self->s_req_tv) || !grid_tab->init;
    if (self->s_req_tv) self->s_req_tv = grid_tab->table_u[0].lab_type;
    i_rgn      = i_lab->p_rgn;
    i_sep      = (i_lab->cstm != 1)
               ? *(i_lab->mlb+i_lab->sepix) : (char) 'E';
    o_rgn      = o_lab->p_rgn;
    o_sep      = (o_lab->cstm != 1)
               ? *(o_lab->mlb+o_lab->sepix) : (char) 'E';
    if (i_sep == '\0' || i_sep == 'L') i_sep = '_';
    if (o_sep == '\0' || o_sep == 'L') o_sep = '_';

    /* Test for leagal datum and systems */
    if (i_lab->cstm < 1 || 16 < i_lab->cstm ||
        (6 <= i_lab->datum && i_lab->datum <= 8)) {
         free(self);
	lord_error( TRF_ILLEG_, "gd_trans(unintelligible input label)");
	return(NULL);
	}
    if ((!s_req_tv) && (o_lab->cstm < 1 || 16 < o_lab->cstm ||
                        (6 <= o_lab->datum && o_lab->datum <= 8))) {
        free(self);
	lord_error( TRF_ILLEG_, "gd_trans(unintelligible output label)");
	return(NULL);
    }

    /* Test for regular systems */
    if (i_lab->imit == 0 && 7 < i_lab->cstm) {
        free(self);
	lord_error( TRF_ILLEG_, "gd_trans(ill. regular system)");
	return(NULL);
    }
    if ((!s_req_tv) && o_lab->imit == 0 && 7 < o_lab->cstm) {
       free(self);
	lord_error( TRF_ILLEG_, "gd_trans(ill. regular system)");
	return(NULL);
    }

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
    for (lev = self->b_lev; lev <= self->s_lev; lev ++) {
      /* lev == 0 : init call_table or special_geoid */
      /*            : lev increased last in loop */
      /* lev == 1 : skipped : for swop of systems to EE */
      /* lev == 2 : init table or special geoid */
      /* lev == 3 : transformation */
      sta[lev] = stp[lev] = 0;
      switch (lev) {
      case 0:
        if (self->s_req_tv) {
          (void) conv_w_crd(grid_tab->table_u[0].clb, &self->H0_lab);
          if (128 <= H_clb->datum && H_clb->datum <= 129) {
            self->H1_lab = self->H0_lab;
            (void) strcpy(H1_clb->mlb, "s34");
            G_Nlab = &self->H1_lab;
          }
          else G_Nlab = &self->H0_lab;
        } else {
          if (i_lab->imit != FHMASK && o_lab->imit != FHMASK &&
            i_rgn != rgn_DE.r_nr[0] && o_rgn != rgn_DE.r_nr[0]) {
            if (grid_tab->init &&
                (!strcmp(grid_tab->table_u[0].mlb, "fehmarngeoid10.bin")
              || !strcmp(grid_tab->table_u[0].mlb, "fbeltgeoid.bin")))
               (void) geoid_c(grid_tab, 0, NULL);
            res    = i_lab->datum == 51 || o_lab->datum == 51 || ies;
            i_wlab = (res) ? &self->H1_lab : &self->H0_lab; /* swop sys for EE_ */
            o_wlab = (res) ? &self->H0_lab : &self->H1_lab;
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
            if (i_lab->imit == FHMASK || o_lab->imit == FHMASK) {
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
              i_chsum = -i_lab->ch_tsum;
              return((tr_error==NULL) ? TRF_GEOID_ :
              t_status(tr_error, "",
                       "gd_trans(unintelligible labels)", TRF_GEOID_));
            }
            (void) conv_w_crd(grid_tab->table_u[0].clb, &self->H0_lab);
          }
          G_Nlab = &self->H0_lab;
        }
        G_Rlab = &self->H0_lab;
        o_nlab = &G_Nlab;
        o_rlab = &G_Rlab;
        break;

	  case 2:
        htr_const.inv = 0;
        if (i_lab->h_dtm == o_lab->h_dtm) s_req_dh = 0;
        else
        if (o_lab->h_dtm == 0) s_req_dh = 0;
        else
        if (i_lab->h_dtm == 0) s_req_dh = -7;
        else {
          s_req_dh = htr_init(i_lab, o_lab, &self->H2_lab,
                              &htr_const, self->H3_lab.mlb, dstr);
          if (s_req_dh < 0) { /* ILLEGAL */
            return((tr_error==NULL) ? TRF_ILLEG_ :
                    t_status(tr_error, "",
                    "gd_trans(ill. kote TRF)", HTRF_ILLEG_));
          }
        }
        switch(s_req_dh) {
        case 1: // DH_TAB height transformation
          if (h_grid_tab.init == 1 &&
             strcmp(self->H3_lab.mlb, h_grid_tab.table_u[0].mlb) != 0)
            (void) geoid_c(&h_grid_tab, 0, NULL);

          if (h_grid_tab.init == 0) {
            res = geoid_i(self->H3_lab.mlb,
                          DHH_LAB, &h_grid_tab, usertxt);
            if (res < 0) return(res);
          }
           
          if (128 <= self->H2_lab.datum
                  && self->H2_lab.datum <= 129) {
            self->H3_lab = self->H2_lab;
            (void) strcpy(self->H3_lab.mlb, "s34");
            T_Nlab = &self->H3_lab;
          }
          else T_Nlab = &self->H2_lab;
          break;
        case 2: // CONSTANT height transformation
          self->H2_lab = *i_lab; // ident trf
          break;
        case 3: // LINEAR height transformation
          T_Nlab = &self->H2_lab;
          break;
        default: // IDT
          sta[lev] = 1;
          stp[lev] = 0;
        }
        T_Rlab = &self->H2_lab;
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
      w_oclb  = *o_rlab;

      if (sta[lev] != stp[lev]) continue;

      /* regular/non-regular decisions */
      /*  in       regular    non-reg   */
      /*  out */
      /* regular     0           1      */
      /* non-reg     2           3      */
      R_N           = ((w_oclb->imit == 0) ? 0 : 2)
                    + (( i_lab->imit == 0) ? 0 : 1);
      /* FE uses utm29 => nonp_i/o must be zero */
      /* when     geo  => nonp_i/o must be one  */
      nonp_o[lev] = (char) (w_oclb->imit == EDMASK
                           || w_oclb->imit == DKMASK
                           || w_oclb->imit == NGMASK
                           || w_oclb->imit == FHMASK);
      nonp_i[lev] = (char) ( i_lab->imit == EDMASK
                           ||  i_lab->imit == DKMASK
                           ||  i_lab->imit == NGMASK
                           ||  i_lab->imit == FHMASK);
      switch (R_N) {
      case REG_REG: /* regular -> regular */
        rs = 0;
        break;
      case NON_REG: /* non-regular -> regular */
        if (i_lab->datum != w_oclb->datum) rs = i_lab->imit;
        else { // same datum
          rs            = 0;
          R_N           = 0;
          nonp_i[lev] = nonp_o[lev] = 0;
        }
        break;
      case REG_NON: /* regular -> non-regular */
        if (i_lab->datum != w_oclb->datum) rs = w_oclb->imit;
        else { // same datum
          rs            = 0;
          R_N           = 0;
          nonp_i[lev] = nonp_o[lev] = 0;
        }
        break;

      case NON_NON: /* non-regular -> non-regular */
        if ((i_lab->imit == NGMASK && w_oclb->imit == NGMASK)) {
          rs            = 0;
          R_N           = 0;
          nonp_i[lev] = nonp_o[lev] = 0;
        } else
        if (i_lab->imit == EDMASK  ||
            i_lab->imit == w_oclb->imit) rs = w_oclb->imit;
        else
        if (w_oclb->imit == EDMASK) rs = i_lab->imit;
        else {
          (void) sprintf(dstr, "gd_trans %s -> %s illegal",
                 i_lab->mlb, w_oclb->mlb);
          return((tr_error==NULL) ? TRF_ILLEG_ :
                  t_status(tr_error, usertxt, dstr, TRF_ILLEG_));
        }
        break;
      } /* end switch(R_N) */

#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("\n*gd_trans (lev:%d) i_lab = %s  o_lab = (%s) %s, R_N = %d;"), lev, i_lab->mlb, w_oclb->mlb, o_lab->mlb, R_N);
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
          if (i_lab->cstm == 1) {
            /* WARN: ONLY POSSIBLE when no gh */
            (void) sprintf(dstr, "geoE%s", i_lab->mlb+4);
            init &= (short) conv_w_crd(dstr, &t_lab)==CRD_LAB;
            if (req_gh) return((tr_error==NULL) ? TRF_ILLEG_ :
              t_status(tr_error, usertxt,
                       "geoid NOT posible", TRF_ILLEG_));
          }
        } else
        if ((R_N == NON_REG || R_N == NON_NON) && i_lab->cstm == 1) {
          (void) sprintf(dstr, "geoE%s", i_lab->mlb+4);
          init &= (short) conv_w_crd(dstr, &g_lab)==CRD_LAB;
        }
        else g_lab = t_lab;
        dfb_trf = fe_trans;
        break;

      case NGMASK: /* ng_trans */
        init    = (short) conv_w_crd("GR_geo_gr96", &t_lab) == CRD_LAB;
        if ((R_N == NON_REG && i_lab->datum != 62) ||
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
        /* Here o_lab must be used: to get the correct limits! */
        i_lab->date  = (i_lab->datum == 33 || o_lab->datum == 33)
                     ? 0.0 : 200.0;
        w_oclb->date = i_lab->date;
        t_clb->date  = i_lab->date;
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
	(void) lord_debug(0, LORD("rs = %d, nonp = %d, %d, t,g=%s,%s;"), rs, nonp_i[lev], nonp_o[lev], t_clb->mlb, (g_lab).mlb);
#endif
      switch(R_N) {
      case REG_REG : /* reg->reg */
        i_Rlab     = i_lab;
        dsh        = (i_lab->datum != w_oclb->datum);
        sta[lev] = *(io_nr_tab + i_lab->cstm);
        stp[lev] = *(io_nr_tab + w_oclb->cstm) - (short) 1;
        if (dsh) {
          ptp[lev] = 0;
          stp[lev] = -stp[lev] + (short) PTN;
        } else {
          ptp[lev] = (short) ((2<i_lab->cstm) && (2<w_oclb->cstm));
          if (ptp[lev]) stp[lev] = -stp[lev] + (short) PTN;
          else {
            if (sta[lev] > stp[lev] + 1) {
              /* e.g.: (CRT=3)->(GEO=2)->(PRJ=1): */
              /*       (GTC, PTG) changed to (CTG, GTP)   */
              sta[lev] = -sta[lev] + (short) IDT;
              stp[lev] = -stp[lev] + (short) PTN;
            }
          }
        }
        if (lev == 0 && ghr) {
          sta[1] = sta[0];
          ptp[1] = ptp[0];
          stp[1] = (short) ((i_lab->datum!=H1_clb->datum) ? CTG : PTG);
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
        sta[lev] =  NTP;
        stp[lev] = *(io_nr_tab + w_oclb->cstm) - (short) 1;
        if ((i_lab->cstm == 1) &&
            ((i_lab->imit == FEMASK) || nonp_i[lev])) {
          /* crt_ed50, crt_nad83g, crt_feh10 */
          i_Nlab     = &g_lab;
          sta[lev] = PRE;
        }

        if (dsh) {
          ptp[lev] = 0;
          stp[lev] = -stp[lev] + (short) PTN;
        } else {
          ptp[lev] = (short) ((2 < t_clb->cstm) && (2 < w_oclb->cstm)
                           &&   (t_clb->ch_sum !=     w_oclb->ch_sum));
          if (ptp[lev]) stp[lev] = -stp[lev] + (short) PTN;
          if (nonp_i[lev]) {
            if (w_oclb->cstm != 1) O_Nlab = *o_rlab;
          }
        }
        if (lev == 0 && ghr) {
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
        dsh        = (i_lab->datum != t_clb->datum);
        sta[lev] = *(io_nr_tab + i_lab->cstm);
        stp[lev] = PTN;
        if (w_oclb->cstm == 1 && lev == 3 &&
            (w_oclb->imit == FEMASK || nonp_o[lev])) {
          /* o_nlab[lev]: 0 : G_Nlab, 2 : T_Nlab, 3 : O_Nlab */
          *o_nlab    = &g_lab;
          stp[lev] = ANT;
        }
        if (dsh) ptp[lev] = 0;
        else {
          ptp[lev] = (short) ((2 < i_lab->cstm) && (2 < t_clb->cstm)
                           &&   (i_lab->ch_sum !=     t_clb->ch_sum));
          if (!ptp[lev]) {
            sta[lev] = -sta[lev] + (short) IDT;
            if (nonp_o[lev]) {
              if (i_lab->cstm != 1) i_Nlab = i_lab;
            }
/* else */
/* +++ HER kunne ch_sum i_lab imod t_lab => stop ved NTP */
          }
          nonp_i[lev] = nonp_o[lev] = 0;
        }
        if (lev == 0 && ghr) {
          sta[1] = sta[0];
          ptp[1] = ptp[0];
          stp[1] = (short) ((i_lab->datum!=H1_clb->datum) ? CTG : PTG);
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
        sta[lev] = NTP;
        stp[lev] = NTP;

        if ((w_oclb->cstm == 1 /* crt_* */) &&
            ((w_oclb->imit == FEMASK) || nonp_o[lev])) {
          /* o_nlab[lev]: 0 : G_Nlab, 2 : T_Nlab, 3 : O_Nlab */
          *o_nlab    = &g_lab;
          sta[lev] = PTN;
          stp[lev] = ANT;
          if (i_lab->cstm == 1 && i_lab->datum != w_oclb->datum) {
            *o_rlab       = &t_lab;     /* reg_s feeds gate-sys */
            i_Nlab        = &t_lab;
            nonp_o[lev] = 1;
            sta[lev]    = CTG;
          }
        } else
        if ((i_lab->cstm == 1) &&
            ((i_lab->imit == FEMASK) || nonp_i[lev])) {
          /* crt_ed50, crt_fodtm, crt_nad83g, crt_feh10 */
          /* NB.:: crt skulle includeres i dk_trans, ng_trans */
          i_Nlab     = &g_lab;
          sta[lev] = PRE;
        }
        if (lev == 0 && ghr) {
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
      if (lev == 0) {
        if (i_lab->ch_sum == H_clb->ch_sum ||
               (ghr == 0 && self->s_req_tv == 0)) {
          sta[lev] = IDT;
          stp[lev] = IDT;
        }
        if (ghr) {
          if (i_lab->ch_sum == H1_clb->ch_sum) {
            sta[1] = IDT;
            stp[1] = IDT;
          }
        }
        ++ lev;
      } else {
        if (i_lab->ch_sum == w_oclb->ch_sum) {
          sta[lev] = IDT;
          stp[lev] = IDT;
        } else
        if (i_lab->ch_sum != w_oclb->ch_sum &&
            i_lab->region != w_oclb->region) {
          if (i_lab->region != 0) {
            R_N           = i_lab->region;
            res           = i_lab->ch_hsum;
            ies           = i_lab->ch_tsum;
            i_lab->region = 0;
            rs            = labchsum((union geo_lab*) i_lab, &i_lab->ch_sum);
            if (rs == w_oclb->ch_sum) {
              sta[lev] = IDT;
              stp[lev] = IDT;
            }
            i_lab->region  = R_N;
            i_lab->ch_hsum = res;
            i_lab->ch_tsum = ies;
          } else
          if (w_oclb->region != 0) {
            R_N            = w_oclb->region;
            ies            = w_oclb->ch_sum;
            res            = w_oclb->ch_hsum;
            ies            = w_oclb->ch_tsum;
            w_oclb->region = 0;
            rs             = labchsum((union geo_lab*) w_oclb,
                                      &w_oclb->ch_sum);
            if (i_lab->ch_sum == rs) {
              sta[lev] = IDT;
              stp[lev] = IDT;
            }
            w_oclb->region  = R_N;
            w_oclb->ch_hsum = res;
            w_oclb->ch_tsum = ies;
          }
        }
      }
    } /* end of lev loop */
    
    self->b_lev  = (ghr || self->s_req_tv) ? 0 : ((self->s_req_dh) ? 2 : 3);
    return /*of the jedi*/ self;
}
