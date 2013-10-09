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


void gd_close(gd_state *self){
	if (!self)
		return;
	if (&self->h_grid_tab)
		geoid_c(&self->h_grid_tab,0,NULL);
	if (!self->grid_tab){
		free(self);
		return;
	}
	if (self->grid_tab==gd_global_fehmarngeoid(1) || self->grid_tab==gd_global_fbeltgeoid(1) || self->grid_tab==gd_global_stdgeoids(1)){
		free(self);
		return;
	}
	geoid_c(self->grid_tab,0,NULL);
	free(self->grid_tab);
	free(self);
	return;
}

struct mgde_str *gd_global_stdgeoids(int open_g){
	static struct mgde_str *self=NULL;
	if(open_g){
		if (!self){
			self=malloc(sizeof(struct mgde_str));
			if (!self) {
				lord_error(TR_ALLOCATION_ERROR,LORD("Geoids: Failed to allocate space."));
				return NULL;
			}
			self->init = 0;
			if (geoid_i("STD", GDE_LAB, self, NULL) > 0) 
				return self;
			geoid_c(self, 0, NULL);
			free(self);
			lord_error(TR_ALLOCATION_ERROR,LORD("Can not open geoids."));
			return NULL;
		}
		return self;
	}
	geoid_c(self, 0, NULL);
	free(self);
	return NULL;
}

struct mgde_str *gd_global_fehmarngeoid(int open_g){
	static struct mgde_str *self=NULL;
	if(open_g){
		if (!self){
			self=malloc(sizeof(struct mgde_str));
			if (!self) {
				lord_error(TR_ALLOCATION_ERROR,LORD("Geoids: Failed to allocate space."));
				return NULL;
			}
			self->init = 0;
			if (geoid_i("fehmarngeoid10.bin", GDE_LAB, self, NULL) > 0) 
				return self;
				
			geoid_c(self, 0, NULL);
			free(self);
			lord_error(TR_ALLOCATION_ERROR,LORD("Can not open fehmarngeoid."));
			return NULL;
		}
		return self;
	}
	geoid_c(self, 0, NULL);
	free(self);
	return NULL;
}

struct mgde_str *gd_global_fbeltgeoid(int open_g){
	static struct mgde_str *self=NULL;
	if(open_g){
		if (!self){
			self=malloc(sizeof(struct mgde_str));
			if (!self) {
				lord_error(TR_ALLOCATION_ERROR,LORD("Geoids: Failed to allocate space."));
				return NULL;
			}
			self->init = 0;
			if (geoid_i("fbeltgeoid.bin", GDE_LAB, self, NULL) > 0) 
				return self;
			geoid_c(self, 0, NULL);
			free(self);
			lord_error(TR_ALLOCATION_ERROR,LORD("Can not open fbeltgeoid."));
			return NULL;
		}
		return self;
	}
	geoid_c(self, 0, NULL);
	free(self);
	return NULL;
}

struct mgde_str *gd_special_table(char *geoid_name){
	struct mgde_str *self;
	self=malloc(sizeof(struct mgde_str));
	if (!self) {
		lord_error(TR_ALLOCATION_ERROR,LORD("SpecialGeoidTable: Failed to allocate space."));
		return NULL;
	}
	self->init = 0;
	if (geoid_i(geoid_name, GDE_LAB, self, NULL) > 0) return self;
	
	geoid_c(self, 0, NULL);
	free(self);
	lord_error(TR_ALLOCATION_ERROR,LORD("Can not open: %s."), geoid_name);
	return NULL;
}



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
  struct coord_lab           *H_clb, *t_clb ;
  struct coord_lab              *o_wlab = NULL;  /* end   REG of ETPL  */
  struct coord_lab             **o_nlab = NULL;  /* end   REG of ETPL  */
  struct coord_lab             **o_rlab = NULL;  /* end   REG of ETPL  */
  
  struct coord_lab           *w_oclb;
  union rgn_un                rgn_DE, rgn_EE, rgn_GR;

  short                       i_rgn, o_rgn;
  int                         lev, R_N, rs = 0, RGH = 0;
  int                         ies = 0, res, dsh = 0, req_th = 0, use_geoids=0;
  char                        dstr[128];
  
  self=calloc(sizeof(gd_state),1);
  if (0==self){
	  lord_error(TR_ALLOCATION_ERROR,LORD("Failed to allocate memory."));
	  return self;
  }

  
  
  H_clb  = &(self->H0_lab);
  t_clb  = &(self->t_lab);

  (void) strcpy(rgn_DE.prfx, "DE");
  (void) strcpy(rgn_EE.prfx, "EE");
  (void) strcpy(rgn_GR.prfx, "GR");
    
 
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
  /* lev == 1 : get Datum Shift of Height dtms : dh             */
  /* lev == 2 : transform coordinates                           */

  /* Geoid Height gh is not transformed at Datum Shifts of Height*/

  /*                     Geoid requested ::  (also -g param)   */
  /* crt_          :: is treated as sepch == E                  */
  /* H             :: is treated as sepch == N                  */
  /*                          ghr=use_geoids                            */
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

  /*  gh : +       :: calculate gh in system self->t_lab              */
  /*  igh          :: calculate gh in system i_lab              */

  /* Asume  H_in is Ellipsoidal height !!!!!               */
  /* then   H_in = h = H + N = Nh + igh                    */
  /* The transformed to self->t_lab is also an Ellisoidal height */
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
  i_rgn      = i_lab->p_rgn;
  o_rgn      = o_lab->p_rgn;
  ies        = (i_rgn == rgn_EE.r_nr[0]) || (o_rgn == rgn_EE.r_nr[0]);
  self->geoid_name[0]='\0';
  use_geoids=(HAS_HEIGHTS(i_lab) || HAS_HEIGHTS(o_lab)) && (GET_HDTM(i_lab)!=GET_HDTM(o_lab)) 
  && (GET_DTM(i_lab)!=GET_DTM(o_lab));
  if (use_geoids){
        if (special_table && *special_table){
	    self->grid_tab = gd_special_table(special_table);
	    strncpy(self->geoid_name,special_table,MLBLNG);
	   }
	  else if (i_lab->imit == FHMASK || o_lab->imit == FHMASK) {
              /* USE: fehmarngeoid10.bin */
              /* because: dvr90g.01 is out by 1cm */
		  self->grid_tab=gd_global_fehmarngeoid(1);
	  }
	  else  if  (i_rgn == rgn_DE.r_nr[0] || o_rgn == rgn_DE.r_nr[0]){
		  self->grid_tab=gd_global_fbeltgeoid(1);
	  }
	  else
		  self->grid_tab=gd_global_stdgeoids(1);
	   if (0 == self->grid_tab) {
		    free(self);
		    return NULL;
	    }

	    if (i_lab->imit == FHMASK || o_lab->imit == FHMASK)
                     (void) strcpy(dstr, self->grid_tab->table_u[0].clb);
	    else
	    if (i_rgn == rgn_GR.r_nr[0] || o_rgn == rgn_GR.r_nr[0])
                    (void) strcpy(dstr, "geoEgr96");
            else if (ies) 
		    (void) strcpy(dstr, "geoEeetrf89");
	    else if (i_rgn != 0) {
              char     rgn_p[8], rgn_name[24];
              if (conv_rgn(i_rgn, rgn_p, rgn_name) > 0)
                 (void) sprintf(dstr, "%-2s_%s",
                                rgn_p, "geoEetrf89");
            } else
                  (void) strcpy(dstr, "geoEetrf89");
            (void) conv_w_crd(dstr, &self->H0_lab);
  }
  else
	  self->grid_tab=NULL;
  
    self->b_lev      = use_geoids ? 0 : 1;
    self->s_lev      =   2;
    self->i_sep      = (i_lab->cstm != 1)
               ? *(i_lab->mlb+i_lab->sepix) : (char) 'E';
    self->o_sep      = (o_lab->cstm != 1)
               ? *(o_lab->mlb+o_lab->sepix) : (char) 'E';
    if (self->i_sep == '\0' || self->i_sep == 'L') self->i_sep = '_';
    if (self->o_sep == '\0' || self->o_sep == 'L') self->o_sep = '_';

    /* Test for leagal datum and systems */
    if (i_lab->cstm < 1 || 16 < i_lab->cstm ||
        (6 <= i_lab->datum && i_lab->datum <= 8)) {
         gd_close(self);
	 lord_error(TRF_ILLEG_, "gd_trans(unintelligible input label)");
	 return(NULL);
	}
    if  (o_lab->cstm < 1 || 16 < o_lab->cstm ||
                        (6 <= o_lab->datum && o_lab->datum <= 8)) {
        gd_close(self);
	lord_error(TRF_ILLEG_, "gd_trans(unintelligible output label)");
	return(NULL);
    }

    /* Test for regular systems */
    if (i_lab->imit == 0 && 7 < i_lab->cstm) {
        gd_close(self);
	lord_error(TRF_ILLEG_, "gd_trans(ill. regular system)");
	return(NULL);
    }
    
    if (o_lab->imit == 0 && 7 < o_lab->cstm) {
        gd_close(self);
	lord_error(TRF_ILLEG_, "gd_trans(ill. regular system)");
	return(NULL);
    }

    if (!ies) {
      if (i_rgn == 0 && o_rgn != 0) i_rgn = o_rgn;
      else
      if (o_rgn == 0 && i_rgn != 0) o_rgn = i_rgn;
    }

    /* Loop for init of geoid/table and transformation */
    for (lev = self->b_lev; lev <= self->s_lev; lev ++) {
      /* lev == 0 : init call_table or special_geoid */
      /* lev == 1 : init table or special geoid */
      /* lev == 2 : transformation */
      self->sta[lev] = self->stp[lev] = 0;
      switch (lev) {
      case 0:
	self->G_Nlab = &self->H0_lab;
        self->G_Rlab = &self->H0_lab;
        o_nlab = &self->G_Nlab;
        o_rlab = &self->G_Rlab;
        break;

    case 1:
        self->htr_const.inv = 0;
        if (i_lab->h_dtm == o_lab->h_dtm) self->s_req_dh = 0;
        else
        if (o_lab->h_dtm == 0) self->s_req_dh = 0;
        else
        if (i_lab->h_dtm == 0) self->s_req_dh = -7;
        else {
          self->s_req_dh = htr_init(i_lab, o_lab, &self->H2_lab,
                              &self->htr_const, self->H3_lab.mlb, dstr);
          if (self->s_req_dh < 0) { /* ILLEGAL */
            lord_error(HTRF_ILLEG_ , "gd_trans(ill. height TRF)");
	    gd_close(self);
	    return NULL;
          }
        }
        switch(self->s_req_dh) {
        case 1: // DH_TAB height transformation
              res = geoid_i(self->H3_lab.mlb,
                          DHH_LAB, &self->h_grid_tab, NULL);
              if (res < 0){ 
		  lord_error(TR_ALLOCATION_ERROR ,"Failed to open dh-table.");
	          gd_close(self);
	          return NULL;
	      }		      
          
           
          if (128 <= self->H2_lab.datum
                  && self->H2_lab.datum <= 129) {
            self->H3_lab = self->H2_lab;
            (void) strcpy(self->H3_lab.mlb, "s34");
            self->T_Nlab = &self->H3_lab;
          }
          else self->T_Nlab = &self->H2_lab;
          break;
        case 2: // CONSTANT height transformation
          self->H2_lab = *i_lab; // ident trf
          break;
        case 3: // LINEAR height transformation
          self->T_Nlab = &self->H2_lab;
          break;
        default: // IDT
          self->sta[lev] = 1;
          self->stp[lev] = 0;
        }
        self->T_Rlab = &self->H2_lab;
        o_nlab = &self->T_Nlab;
        o_rlab = &self->T_Rlab;
        break;

      case 2:
        self->O_Nlab = o_lab;
        self->O_Rlab = o_lab;
        o_nlab = &self->O_Nlab;
        o_rlab = &self->O_Rlab;
        break;
      }
      w_oclb  = *o_rlab;

      if (self->sta[lev] != self->stp[lev]) continue;

      /* regular/non-regular decisions */
      /*  in       regular    non-reg   */
      /*  out */
      /* regular     0           1      */
      /* non-reg     2           3      */
      R_N           = ((w_oclb->imit == 0) ? 0 : 2)
                    + (( i_lab->imit == 0) ? 0 : 1);
      /* FE uses utm29 => self->nonp_i/o must be zero */
      /* when     geo  => self->nonp_i/o must be one  */
      self->nonp_o[lev] = (char) (w_oclb->imit == EDMASK
                           || w_oclb->imit == DKMASK
                           || w_oclb->imit == NGMASK
                           || w_oclb->imit == FHMASK);
      self->nonp_i[lev] = (char) ( i_lab->imit == EDMASK
                           ||  i_lab->imit == DKMASK
                           ||  i_lab->imit == NGMASK
                           ||  i_lab->imit == FHMASK);
      switch (R_N) {
      case REG_REG: /* regular -> regular */
        rs = 0;
        break;
      case NON_REG: /* non-regular -> regular */
        if (i_lab->datum != w_oclb->datum) rs = i_lab->imit;
        else { /* same datum */
          rs            = 0;
          R_N           = 0;
          self->nonp_i[lev] = self->nonp_o[lev] = 0;
        }
        break;
      case REG_NON: /* regular -> non-regular */
        if (i_lab->datum != w_oclb->datum) rs = w_oclb->imit;
        else { // same datum
          rs            = 0;
          R_N           = 0;
          self->nonp_i[lev] = self->nonp_o[lev] = 0;
        }
        break;

      case NON_NON: /* non-regular -> non-regular */
        if ((i_lab->imit == NGMASK && w_oclb->imit == NGMASK)) {
          rs            = 0;
          R_N           = 0;
          self->nonp_i[lev] = self->nonp_o[lev] = 0;
        } else
        if (i_lab->imit == EDMASK  ||
            i_lab->imit == w_oclb->imit) rs = w_oclb->imit;
        else
        if (w_oclb->imit == EDMASK) rs = i_lab->imit;
        else {
		lord_error(TRF_ILLEG_ ,"gd_trans %s -> %s illegal",i_lab->mlb, w_oclb->mlb);
	        gd_close(self);
		return NULL;
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
      self->t_lab   = &?_lab;
      self->dfb_trf = ??_trans;
      break;
#endif

      /* regular/non-regular selections */
      /*________________________________*/
      switch (rs) {

      case DKMASK: /* dk_trans */
      case EDMASK: /* dk_trans */
         conv_w_crd("DK_tcgeo_ed50", &self->t_lab);
	 conv_w_crd("DK_geo_ed50",  &self->g_lab);
         self->dfb_trf = dk_trans;
        break;

      case FEMASK: /* fe_trans */
          conv_w_crd("FO_utm29_etrf89", &self->t_lab);
        /* utm29_etrs89 => self->nonp_i/o must be zero */
        if ((R_N == REG_NON || R_N == NON_NON) && w_oclb->cstm == 1) {
          (void) sprintf(dstr, "geoE%s", w_oclb->mlb+4);
            conv_w_crd(dstr, &self->g_lab);
          if (i_lab->cstm == 1) {
            /* WARN: ONLY POSSIBLE when no gh */
            (void) sprintf(dstr, "geoE%s", i_lab->mlb+4);
             conv_w_crd(dstr, &self->t_lab);
	    if (use_geoids){
		    lord_error(TRF_ILLEG_,LORD("Impossible to use geoids here! %s->%s"),i_lab->mlb,o_lab->mlb);
		    gd_close(self);
	    }
          }
        } else
        if ((R_N == NON_REG || R_N == NON_NON) && i_lab->cstm == 1) {
          (void) sprintf(dstr, "geoE%s", i_lab->mlb+4);
           conv_w_crd(dstr, &self->g_lab);
        }
        else self->g_lab = self->t_lab;
        self->dfb_trf = fe_trans;
        break;

      case NGMASK: /* ng_trans */
	conv_w_crd("GR_geo_gr96", &self->t_lab);
        if ((R_N == NON_REG && i_lab->datum != 62) ||
            (R_N == REG_NON && w_oclb->datum != 62)) self->g_lab = self->t_lab;
        else
           conv_w_crd("GR_geo_nad83g", &self->g_lab);
        self->dfb_trf = ng_trans;
        break;

      case EEMASK: /* ee_trans */
        conv_w_crd("EE_utm35_eetrf89", &self->t_lab);
        self->dfb_trf = ee_trans;
        break;

      case FHMASK: /* fh_trans */
        conv_w_crd("DK_geo_feh10", &self->t_lab);
        /* Here o_lab must be used: to get the correct limits! */
        i_lab->date  = (i_lab->datum == 33 || o_lab->datum == 33)
                     ? 0.0 : 200.0;
        w_oclb->date = i_lab->date;
        t_clb->date  = i_lab->date;
        self->g_lab        = self->t_lab;
        self->dfb_trf      = fh_trans;
        break;

      case 0: /* all regular */
        break;

      default: /* error report */
	lord_error(TRF_PROGR_,"Program error");
	gd_close(self);
        return NULL;
      } /* end rs switch */

#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("rs = %d, nonp = %d, %d, t,g=%s,%s;"), rs, self->nonp_i[lev], self->nonp_o[lev], t_clb->mlb, (self->g_lab).mlb);
#endif
      switch(R_N) {
      case REG_REG : /* reg->reg */
        self->i_Rlab     = i_lab;
        dsh        = (i_lab->datum != w_oclb->datum);
        self->sta[lev] = *(io_nr_tab + i_lab->cstm);
        self->stp[lev] = *(io_nr_tab + w_oclb->cstm) - (short) 1;
        if (dsh) {
          self->ptp[lev] = 0;
          self->stp[lev] = -self->stp[lev] + (short) PTN;
        } else {
          self->ptp[lev] = (short) ((2<i_lab->cstm) && (2<w_oclb->cstm));
          if (self->ptp[lev]) self->stp[lev] = -self->stp[lev] + (short) PTN;
          else {
            if (self->sta[lev] > self->stp[lev] + 1) {
              /* e.g.: (CRT=3)->(GEO=2)->(PRJ=1): */
              /*       (GTC, PTG) changed to (CTG, GTP)   */
              self->sta[lev] = -self->sta[lev] + (short) IDT;
              self->stp[lev] = -self->stp[lev] + (short) PTN;
            }
          }
        }
      
        break;

      case NON_REG : /* non-reg input */
        /* o_nlab[lev]: 0 : self->G_Nlab, 2 : self->T_Nlab, 3 : self->O_Nlab(o_lab) */
        *o_nlab        = &self->t_lab;
        self->i_Nlab   = i_lab;
        self->i_Rlab   = &self->t_lab;
        dsh            = (t_clb->datum != w_oclb->datum);
        self->sta[lev] =  NTP;
        self->stp[lev] = *(io_nr_tab + w_oclb->cstm) - (short) 1;
        if ((i_lab->cstm == 1) &&
            ((i_lab->imit == FEMASK) || self->nonp_i[lev])) {
          /* crt_ed50, crt_nad83g, crt_feh10 */
          self->i_Nlab     = &self->g_lab;
          self->sta[lev] = PRE;
        }

        if (dsh) {
          self->ptp[lev] = 0;
          self->stp[lev] = -self->stp[lev] + (short) PTN;
        } else {
          self->ptp[lev] = (short) ((2 < t_clb->cstm) && (2 < w_oclb->cstm)
                           &&   (t_clb->ch_sum !=     w_oclb->ch_sum));
          if (self->ptp[lev]) self->stp[lev] = -self->stp[lev] + (short) PTN;
          if (self->nonp_i[lev]) {
            if (w_oclb->cstm != 1) self->O_Nlab = *o_rlab;
          }
        }
      break;

      case REG_NON : /* non-reg output */
        /* o_rlab[lev]: 0 : self->G_Nlab, 1 : self->T_Nlab, 2 : self->O_Nlab(o_lab) */
        self->i_Rlab     = i_lab;               /* regular sys is input */
        *o_rlab    = &self->t_lab;              /* reg_s feeds gate-sys */
        self->i_Nlab     = &self->t_lab;
        dsh        = (i_lab->datum != t_clb->datum);
        self->sta[lev] = *(io_nr_tab + i_lab->cstm);
        self->stp[lev] = PTN;
        if (w_oclb->cstm == 1 && lev == 3 &&
            (w_oclb->imit == FEMASK || self->nonp_o[lev])) {
          /* o_nlab[lev]: 0 : self->G_Nlab, 1 : self->T_Nlab, 2 : self->O_Nlab */
          *o_nlab    = &self->g_lab;
          self->stp[lev] = ANT;
        }
        if (dsh) self->ptp[lev] = 0;
        else {
          self->ptp[lev] = (short) ((2 < i_lab->cstm) && (2 < t_clb->cstm)
                           &&   (i_lab->ch_sum !=     t_clb->ch_sum));
          if (!self->ptp[lev]) {
            self->sta[lev] = -self->sta[lev] + (short) IDT;
            if (self->nonp_o[lev] && i_lab->cstm != 1) 
		    self->i_Nlab = i_lab;
            
/* else */
/* +++ HER kunne ch_sum i_lab imod self->t_lab => stop ved NTP */
          }
          self->nonp_i[lev] = self->nonp_o[lev] = 0;
        }
       break;

      case NON_NON : /* non-reg input/output */
        /* self->t_lab: 0 : self->G_Nlab, 1 : self->T_Nlab, 2 : self->O_Nlab(o_lab) */
        self->i_Nlab     = i_lab;
        dsh        = 0;
        self->sta[lev] = NTP;
        self->stp[lev] = NTP;

        if ((w_oclb->cstm == 1 /* crt_* */) &&
            ((w_oclb->imit == FEMASK) || self->nonp_o[lev])) {
          /* o_nlab[lev]: 0 : self->G_Nlab, 1 : self->T_Nlab, 2 : self->O_Nlab */
          *o_nlab    = &self->g_lab;
          self->sta[lev] = PTN;
          self->stp[lev] = ANT;
          if (i_lab->cstm == 1 && i_lab->datum != w_oclb->datum) {
            *o_rlab       = &self->t_lab;     /* reg_s feeds gate-sys */
            self->i_Nlab        = &self->t_lab;
            self->nonp_o[lev] = 1;
            self->sta[lev]    = CTG;
          }
        } else
        if ((i_lab->cstm == 1) &&
            ((i_lab->imit == FEMASK) || self->nonp_i[lev])) {
          /* crt_ed50, crt_fodtm, crt_nad83g, crt_feh10 */
          /* NB.:: crt skulle includeres i dk_trans, ng_trans */
          self->i_Nlab     = &self->g_lab;
          self->sta[lev] = PRE;
        }
       
        break;
      } /* end switch R_N */

      /* jumper might be set for webmrc when to/from wgs84/etrf89: */
      /* jumper might be set from PTG/GTC to GTP   (self->ptp[lev])      */
      /* or set startpoint   from GTC to GTP       (self->sta[lev])      */

      /* test for identical systems */
      if (lev == 0) {
        if (i_lab->ch_sum == H_clb->ch_sum) {
          self->sta[0] = IDT;
          self->stp[0] = IDT;
        }
       } else {
        if (i_lab->ch_sum == w_oclb->ch_sum) {
          self->sta[lev] = IDT;
          self->stp[lev] = IDT;
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
              self->sta[lev] = IDT;
              self->stp[lev] = IDT;
            }
            i_lab->region  = R_N;
            i_lab->ch_hsum = res;
            i_lab->ch_tsum = ies;
          } else
          if (w_oclb->region != 0) {
            R_N            = w_oclb->region;
            res            = w_oclb->ch_hsum;
            ies            = w_oclb->ch_tsum;
            w_oclb->region = 0;
            rs             = labchsum((union geo_lab*) w_oclb,
                                      &w_oclb->ch_sum);
            if (i_lab->ch_sum == rs) {
              self->sta[lev] = IDT;
              self->stp[lev] = IDT;
            }
            w_oclb->region  = R_N;
            w_oclb->ch_hsum = res;
            w_oclb->ch_tsum = ies;
          }
        }
      }
    } /* end of lev loop */
    self->iEh_req = use_geoids && (i_lab->cstm == (short) 1 /* CRT */);
    self->oEh_req = (short) (((self->stp[2] == (short) CTC || self->stp[2] == (short) GTC) && use_geoids) ? 2 : (use_geoids ? 1 : 0));
    self->b_lev  = (use_geoids) ? 0 : ((self->s_req_dh) ? 1 : 2);
    self->i_lab=i_lab;
    self->o_lab=o_lab;
    return /*of the jedi*/ self;
}
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
