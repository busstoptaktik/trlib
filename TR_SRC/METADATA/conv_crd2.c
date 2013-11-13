/*
 * Copyright (c) 2012, National Survey and Cadastre, Denmark
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

/* Copyright (c) 2012 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

/* conv_crd(name, p_lb, lab) service function for conv_lab  - well not anymore. Congrats conv_crd :-)   */
/* NB should be called only from conv_lab - nope, thats not what we agreed - don't do it unless you're conv_lib.so*/
/*    result ==  0 : no lab                                    */
/*    result ==  1 : coord lab             in lab              */
/*    UNDEFSYS     : unrecognized name                         */
/* an unrecognized name is considered a filename containing a  */
/* genuine label                                               */

#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <ctype.h>

#include    <math.h>

#include    "KmsFncs.h"


int                      conv_crd2(
/*______________________________*/
struct lab_def_str      *p_lb,
struct coord_lab        *c_lab,
def_data               *DEF_DATA,
char                     *p_io        /*pointer to string containing parameters*/
)
{


  int                         i, used,io_lng;
  int                        zone;
  short                      par_dtm, dtm_req = 1, mask;
  double                     ell_p[10];
  char                       e_name[MLBLNG], p_name[MLBLNG];
  char                        rgn_name[4];
  char                       t_lab_a[2*MLBLNG], *t_lab = t_lab_a;
  char                      *p_dtm, sepch, *p_sys, *p_tp;

  char                      *p_eesti42 = "eesti42";
  char                      *p_gs      = "gs";
  char                      *p_gsb     = "gsb";

  struct typ_dec             g_tpd, m_tpd;

  double                    *p, f, e, pM, sc;
  struct dsh_str            *d_s, d_sh;

  /* region prefix and name */


  static char    *c_types[] = {
    /*  1 */ "3-d Cartesian",
    /*  2 */ "Geographic",
    /*  3 */ "Transversal Mercator",
    /*  4 */ "Mercator",
    /*  5 */ "Lambert c. conical",
    /*  6 */ "Stereographic",
    /*  7 */ "Equal-area",
    /*  8 */ "System 1934/45",
    /*  9 */ "GS conf. conical"
    /* 10 */ "Koebenhavns Kommune",
    /* 11 */ "Cartesic 2D",
    /* 12 */ "RiksTriangukationen SE",
    /* 13 */ "",
    /* 14 */ "WEB-mercator",
    /* 15 */ "",
    /* 16 */ "geoid heights, deflections",
    /* 17 */ "ellipsoid heights",
    /* 18 */ "gravity",
    /* 19 */ "",
  };

 
 

  


  if (p_lb->lab_type != CRD_LAB) {
    lord_error(0,LORD("conv_crd called with wrong lab_type %d"), p_lb->lab_type);
    c_lab->lab_type = ILL_LAB;
    return(ILL_LAB);
  }

  
    p_dtm = p_lb->mlb2;

 

    p_sys = p_lb->name;  /* name is mlb1 */

    /* clean lab */
    p_tp = (char *) c_lab;
    for (i = sizeof (*c_lab) - 1; i >= 0; i--) *(p_tp + i) = 0;

    /* label_def_str contain sepch (not sepix) */
    sepch           = (char) p_lb->sepch;
    c_lab->lab_type = p_lb->lab_type;
    c_lab->version  = LAB_VERSION;
    c_lab->cmplt    = (short) 0;
    c_lab->cstm     = p_lb->d_kind;
    c_lab->mode     = p_lb->d_type;
    c_lab->region   = 0;/*p_lb->region;*/
    c_lab->p_rgn    = 0; /*p_lb->lab_rgn;*/
    c_lab->ncoord   = (short) strlen(p_lb->a_seq);
    c_lab->scale    = 1.0;        /* default */
    c_lab->imit     = p_lb->mask;
    /*lord_debug(0,LORD("System is now: %d, lab_type: %d"),c_lab->cstm,c_lab->lab_type);*/
    if (c_lab->cstm == 1 && sepch != '_') sepch = '_';
    if (sepch == 'N' || sepch == 'E' || sepch == 'H') ++ c_lab->ncoord;

    switch (*p_lb->a_seq) {
    case 'N' :
      if (*(p_lb->a_seq +1) == 'W') c_lab->W_crd = 1;
      else
      if (*(p_lb->a_seq +1) == 'V') c_lab->W_crd = 3;
      break;
    case 'S' :
      c_lab->S_crd = 1;
      if (*(p_lb->a_seq +1) == 'W') c_lab->W_crd = 1;
      else
      if (*(p_lb->a_seq +1) == 'V') c_lab->W_crd = 3;
      break;
    case 'E' :
      c_lab->p_seq = 1;
      if (*(p_lb->a_seq +1) == 'S') c_lab->S_crd = 1;
      break;
    case 'W' :
      c_lab->p_seq = 1;
      c_lab->W_crd = 1;
      if (*(p_lb->a_seq +1) == 'S') c_lab->S_crd = 1;
      break;
    case 'V' :
      c_lab->p_seq = 1;
      c_lab->W_crd = 3;
      if (*(p_lb->a_seq +1) == 'S') c_lab->S_crd = 1;
      break;
    }

    /* set params of the coord systems */
    /*_________________________________*/
    switch(c_lab->cstm) {

    case  1: /* crt, 3-dim */
      if (!strcmp(p_lb->mlb2, p_eesti42)) c_lab->mode = 10;

      if (c_lab->mode == 10) { /* Estonia */
        (void) strcpy(p_sys, "eegeo");
        (void) strcpy(p_lb->pr_dtm, p_eesti42);
        if (sepch == '_') sepch = '\0';
      }
      if (c_lab->mode == 11) c_lab->ncoord = 2;
      break;

    case  2: /* geo */
      /* GS geographical coord */
      if (!strcmp(p_lb->mlb2, p_gs)) c_lab->mode = 2;
      else
      /* GSB geographical coord */
      if (!strcmp(p_lb->mlb2, p_gsb)) c_lab->mode = 3;

      switch (c_lab->mode) {
      case 2:       /* geo_gs -->> gsgeo */
        (void) strcpy(p_sys, "gsgeo");
        (void) strcpy(p_lb->pr_dtm, p_gs);
        if (sepch == '_') sepch = '\0';
        break;
      case 3:       /* geo_gsb -->> gsbgeo */
        (void) strcpy(p_sys, "gsbgeo");
        (void) strcpy(p_lb->pr_dtm, p_gsb);
        if (sepch == '_') sepch = '\0';
        break;

      case 0:
      default:
        break;
      } /* geo */

      break; /* end cs: 2, geo coord */

    case  3: /* utm, itm, dks, asb, sb */
     
      switch (c_lab->mode) {
      case    0: /* utm**   (N, E) */
      case    1: /* utm**n  (N, E) */
      case    9: /* tsu**   Time Series utm zone ** */
      case   13: /* ETRS-TM, ETRS89 transverse mercator zone ** */
        if (p_lb->cha_str > 0) {
          if (isdigit(*(p_sys+p_lb->cha_str))) {   // C_DIGIT udskiftet med isdigit  20120529 stl
            if ((sscanf(p_sys+p_lb->cha_str, "%d", &zone) == 1)
                && (1 <= zone && zone <= 60)) {
              c_lab->B0     = 0.0;
              c_lab->N0     = (!c_lab->S_crd) ? 0.0 : 1.0e+7;
              c_lab->L0     = (zone - 30.5)/30.0*M_PI;
              c_lab->E0     = 500000.0;
              c_lab->scale  = 0.9996;
            }
            else c_lab->lab_type = ILL_LAB;
          }
          else c_lab->lab_type = ILL_LAB;
        }
	
        break;

      case 8: /* tm**  */
        if (p_lb->cha_str > 0) {
          if (isdigit(*(p_sys+p_lb->cha_str))) {	// C_DIGIT udskiftet med isdigit  20120529 stl
            (void) sscanf(p_sys+p_lb->cha_str, "%d", &zone);
            c_lab->N0 = 0.0;
            c_lab->B0 = 0.0;
            c_lab->L0 = zone*M_PI/60.0;
            c_lab->E0 = zone*1000000.0 + 500000.0;
          }
          else c_lab->lab_type = ILL_LAB;
        }
        break;

      case -8: /* eetm**  */
        c_lab->mode = 8;
        break;

      case 132: /* tc32 => TR utm32_ed50 */
        if (sepch == '\0') sepch = '_';
        break;
      }
      break; /* end case 3 : utm, itm, dks, asb, sb */

	case  4: /* mrc */
    case  5: /* lmb */
    case  6: /* stg */
    case  7: /* safle, safl, lmbac, lmbap, lmblap, lmbaps, lmblaps, authalic geog. */
             /* ETRS Lambert Azimuthal Equal Area                 */
    case  9: /* gs and gsb */
    case 12: /* rt90v and rt38v, spec. f. dks */
    case 10: /* kk (mode = 0) and os (mode = 1) */
      break;

    case  8: /* s34 */
      if (c_lab->mode == 3) (void) strcpy(p_sys, "s45b");
      break; /* end case 8 : s34 */

    case 11: /* crt2 */
      dtm_req = 0;
      break;

    case 16: /* geoid heights, deflections of the vertical */
    case 17: /* ellipsoid heights */
      sepch = '_';
      break;

    case 18: /* gravity : gravi, freeair, bouguer, normal */
      sepch = '_';
      if (*p_lb->mlb2 == '\0') (void) strcpy(p_lb->mlb2, "igsn");
      break;

    default: /* unintelligible system */
    
      c_lab->lab_type = ILL_LAB;
      break;
    }

    /* PRODUCE THE MINI-LAB  from : mlb1, sepch, dtm */

    /* The minilabel of regular systems with imittance mask of  */
    /* zero contains a concatenation of the coord_system, the   */
    /* separator and the datum. A non-zero imittance mask means */
    /* that only the coord_system is used. A separator != '_'   */
    /* is added to the minilabel, but no datum is given         */

    c_lab->sepix = (short) strlen(p_sys);
    if (*p_lb->pr_dtm == '\0') {
      (void) sprintf(c_lab->mlb, "%s%c%s", p_sys, sepch, p_dtm);
    } else {
      (void) sprintf(c_lab->mlb, "%s%c", p_sys, sepch);
      p_dtm = p_lb->pr_dtm;
    }
    
    if (dtm_req) {
	
    
      /* datum and datum shift constants */
      if (*p_lb->pr_dtm != '\0') p_dtm = p_lb->pr_dtm;
      (void) strcpy(t_lab, p_dtm);
      c_lab->datum = (short) set_dtm_1(-1, p_dtm, &par_dtm, p_name,
                     e_name,rgn_name, &mask, &(c_lab->dsh_con),DEF_DATA);
     
      if (c_lab->datum > 0) {
       
        if (c_lab->imit == 0) c_lab->imit = mask;
        c_lab->ellipsoid = (short) set_grs(-1, e_name, ell_p, DEF_DATA);
      } else {
        c_lab->ellipsoid = (short) set_grs(-1, t_lab, ell_p, DEF_DATA);
        c_lab->datum     = -2;
      }

      if (c_lab->ellipsoid >= 0) {
        /* ellipsoid params */
        c_lab->a = *(ell_p+0);
        c_lab->f = *(ell_p+1);
        /* datum shift params (from set_dtm) */
        c_lab->p_dtm = (short) par_dtm;
      }
      else
        /* unintelligible ellipsoid */
        if (c_lab->cstm != 11 /* crt2 */) c_lab->lab_type = ILL_LAB;
    } else {
      c_lab->datum     = 0;
      c_lab->ellipsoid = 0;
    }
      
 
    /* INPUT OF ADDITIONAL PARAMETERS */
    if (p_lb->q_par) {
      /* read predifined  ? */
      p_tp   = (*p_lb->add_p == '\0') ? p_lb->par_str : p_io;
      io_lng = strlen(p_tp);
      if (*p_tp != '\0' && io_lng > 3) {
        switch (p_lb->q_par) {
        case 1:
          c_lab->L0  = sgetg(p_tp, &(c_lab->g_tpd), &used, "sx");
          break;
        case 2:
          c_lab->B1 = sgetg(p_tp, &(c_lab->g_tpd), &used, "sx");
          p_tp    += used;
          io_lng  -= used;
          if (io_lng > 2)
             (void) sscanf(p_tp, "%lf%n", &(c_lab->scale), &used);
          else {
            lord_error(0,LORD("conv_crd %s too few par."), c_lab->mlb);
            return(ILL_LAB);
          }
          break;
        case 3:
          f = sgetg(p_tp, &(c_lab->m_tpd), &used, "m");
          if (c_lab->cstm == 6) // upsn / upss
              c_lab->N0 = f;
          else  /* s34 */ c_lab->B0 = f;
          p_tp    += used;
          io_lng  -= used;
          if (io_lng > 2)
             c_lab->E0  = sgetg(p_tp, &m_tpd, &used, "m");
          else {
            lord_error(0,LORD("conv_crd %s too few par."), c_lab->mlb);
            return(ILL_LAB);
          }
          p_tp    += used;
          io_lng  -= used;
          if (io_lng > 2)
             (void) sscanf(p_tp, "%lf%n", &(c_lab->scale), &used);
          else {
            lord_error(0,LORD("conv_crd %s too few par."), c_lab->mlb);
            return(ILL_LAB);
          }
          break;
        case 4:
        case 5:
        case 6:
          c_lab->B0  = sgetg(p_tp, &(c_lab->g_tpd), &used, "sx");
          p_tp    += used;
          io_lng  -= used;
          if (io_lng > 2)
             c_lab->N0  = sgetg(p_tp, &(c_lab->m_tpd), &used, "m");
          else {
            lord_error(0,LORD("conv_crd %s too few par."), c_lab->mlb);
            return(ILL_LAB);
          }
          p_tp   += used;
          io_lng -= used;
          if (io_lng > 2)
             c_lab->L0  = sgetg(p_tp, &g_tpd, &used, "sx");
          else {
            lord_error(0,LORD("conv_crd %s too few par."), c_lab->mlb);
            return(ILL_LAB);
          }
          p_tp    += used;
          io_lng  -= used;
          if (io_lng > 2)
              c_lab->E0  = sgetg(p_tp, &m_tpd, &used, "m");
          else {
            lord_error(0,LORD("conv_crd %s too few par."), c_lab->mlb);
            return(ILL_LAB);
          }
          if (p_lb->q_par >= 5) {
            p_tp   += used;
            io_lng -= used;
            if (io_lng > 2) {
              if (p_lb->q_par == 5 && (c_lab->cstm == 3 || c_lab->cstm == 12))
                 (void) sscanf(p_tp, "%lf%n", &(c_lab->scale), &used);
              else {
                c_lab->B1 = sgetg(p_tp, &g_tpd, &used, "sx");
                if (p_lb->q_par == 6) { // dlmb
                  p_tp    += used;
                  io_lng  -= used;
                  if (io_lng > 2)
                  c_lab->B2  = sgetg(p_tp, &g_tpd, &used, "sx");
                  else {
                    lord_error(0,LORD("conv_crd %s too few par."), c_lab->mlb);
                    return(ILL_LAB);
                  }
                }
              }
            } else {
              lord_error(0,LORD("conv_crd %s too few par."), c_lab->mlb);
              return(ILL_LAB);
            }
          }
          break;
        }
        io_lng       -= used;
        c_lab->cmplt  = (short) ((*p_lb->add_p == '\0') ? 1 : 2);
        p_tp          = p_io;
      } /* not empty string */
      else {
	lord_error(0,LORD("conv_crd %s too few par."), c_lab->mlb);
        c_lab->lab_type = ILL_LAB;
        return (ILL_LAB);
      }

      if (c_lab->cstm == 6 /*ste*/) {
        if (c_lab->mode == 3) { /* np_stg or sp_stg ? */
          if (c_lab->B0 == M_PI_2 || c_lab->B0 == -M_PI_2) {
            c_lab->mode = (c_lab->B0 > 0.0) ? +1 : -1;
          }
        }
        if (c_lab->mode == 4) { /* np_estg or sp_esg ? */
          if (c_lab->B0 == M_PI_2 || c_lab->B0 == -M_PI_2) {
            c_lab->mode  = (c_lab->B0 > 0.0) ? +2 : -2;
            c_lab->N0    = c_lab->E0 = 2000000.0;
            c_lab->scale = 0.994;
          }
        }
      }  /* ste */
    }  /* end input of additional params */
    else p_tp = p_io;

    /* set trf. constants for completed systems */
    (void) set_trc(c_lab);
      /* lord_debug(0,LORD("Mode is now: %d, type is: %d, datum: %d"),c_lab->mode,c_lab->lab_type,c_lab->datum);*/
     /* INPUT OF DATE at datums: ITRF, IGS, ETRF */
    if (6 <= c_lab->datum && c_lab->datum <= 8) {
     
        if (p_tp != (char *) 0 && strlen(p_tp) >= 8) {
          c_lab->date = sgetdt(p_tp, &c_lab->JD, &used, &i);
          if (c_lab->date < -1) {
            c_lab->date = -1.0;
            c_lab->JD   = -36525.0;
          }
        } /* not empty string */
        else {
          c_lab->date = -1.0;
          c_lab->JD   = -36525.0;
        }
      
    }  /* end input of date */

    /* checksum for identification of the label */
    if (*(c_lab->mlb + c_lab->sepix) != 'H')
      c_lab->ch_sum = labchsum((union geo_lab *) c_lab, &c_lab->ch_sum);
   
 



  return(c_lab->lab_type);
}
