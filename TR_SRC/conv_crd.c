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
 


/* conv_crd    ver 2012.01          # page 1    31 May 2012 10 19 */


/* Copyright (c) 2012 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

/* conv_crd(name, p_lb, lab) service function for conv_lab     */
/* NB should be called only from conv_lab                      */
/*    result ==  0 : no lab                                    */
/*    result ==  1 : coord lab             in lab              */
/*    UNDEFSYS     : unrecognized name                         */
/* an unrecognized name is considered a filename containing a  */
/* genuine label                                               */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <math.h>

#include "geo_lab.h"
#include "trthread.h"

int                      conv_crd(
/*______________________________*/
char                    *lab_info,
struct lab_def_str      *p_lb,
struct coord_lab        *c_lab,
...
)
{

#include              "conv_rgn.h"
#include              "set_grs.h"
#include              "set_dtm_1.h"
#include              "set_trc.h"
#include              "fgetln_kms.h"
#include              "sgetdt.h"
#include              "sgetg.h"
#include              "sputg.h"
#include              "labchsum.h"
#include              "fputg.h"
#include              "fputcw.h"
#include              "set_tpd.h"


/* conv_crd    ver 2012.01          # page 2    31 May 2012 10 19 */


  FILE                      *iofile = (FILE *) NULL;
  char                      *io_str = (char *) NULL;
  va_list                    o_p;

  int                        conv_mode, i, used;
  int                        zone;
  short                      par_dtm, dtm_req = 1, mask;
  double                     ell_p[10];
  char                       e_name[MLBLNG], p_name[MLBLNG];

  char                       t_lab_a[2*MLBLNG], *t_lab = t_lab_a;
  char                       iofile_str[256], *p_io;
  char                      *p_dtm, sepch, *p_sys, *p_tp;

  char                      *p_eesti42 = "eesti42";
  char                      *p_gs      = "gs";
  char                      *p_gsb     = "gsb";

  struct typ_dec             g_tpd, m_tpd;

  double                    *p, f, e, pM, sc;
  struct dsh_str            *d_s, d_sh;

  /* region prefix and name */
  char                       rgn_name[24];
  union rgn_un               rgn_EE, rgn_pref, dum_pref;

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


/* conv_crd    ver 2012.01          # page 3    31 May 2012 10 19 */


  (void) strcpy(rgn_EE.prfx, "EE");
  /* save lab_info */
  (void) strcpy(t_lab, lab_info);

  conv_mode = (!strcmp(t_lab,  "input")) ? 1 :
              (!strcmp(t_lab, "output")) ? 3 :
              (!strcmp(t_lab, "OUTPUT")) ? 4 :
              (!strcmp(t_lab, "SURVEY")) ? 5 : 0;

  va_start(o_p, c_lab);
  if (conv_mode) iofile = va_arg(o_p, FILE *);
  else           io_str = va_arg(o_p, char *);
  va_end(o_p);

  if (p_lb->lab_type != CRD_LAB) {
    (void) fprintf(stdout,
           "\nconv_crd called with wrong lab_type %d",
           p_lb->lab_type);
    c_lab->lab_type = ILL_LAB;
    return(ILL_LAB);
  }

  (void) strcpy(dum_pref.prfx, "ZZ");
  p_dtm = p_lb->mlb2;

  switch(conv_mode) {
  case 0: /* char input */
  case 1: /* file input */

    p_sys = p_lb->name;  /* name is mlb1 */

    /* clean lab */
    p_tp = (char *) c_lab;
    for (i = sizeof (*c_lab) - 1; i >= 0; i--) *(p_tp + i) = 0;

    /* label_def_str contain sepch (not sepix) */
    sepch           = (char) p_lb->sepch;
    c_lab->lab_type = p_lb->lab_type;
    c_lab->version  = LAB_VERSION;
    c_lab->cmplt    = (short) ((p_lb->q_par) ? 0 : 1);
    c_lab->cstm     = p_lb->d_kind;
    c_lab->mode     = p_lb->d_type;
    c_lab->region   = p_lb->region;
    c_lab->p_rgn    = p_lb->lab_rgn;
    c_lab->ncoord   = (short) strlen(p_lb->a_seq);
    c_lab->scale    = 1.0;        /* default */
    c_lab->imit     = p_lb->mask;
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


/* conv_crd    ver 2012.01          # page 4    31 May 2012 10 19 */


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


/* conv_crd    ver 2012.01          # page 5    31 May 2012 10 19 */


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


/* conv_crd    ver 2012.01          # page 6    31 May 2012 10 19 */


    case  4: /* mrc */
    case  5: /* lmb */
    case  6: /* stg */
    case  7: /* safle, safl, lmbac, lmbap, lmbaps, authalic geog. */
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


/* conv_crd    ver 2012.01          # page  7    31 May 2012 10 19 */


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
      /* prepare generated c_label */
      /*___________________________*/

      /* SPECIAL : Estonian EUREF89 <-> WGS84 */
      if (c_lab->region == rgn_EE.r_nr[0] ||
          c_lab->p_rgn  == rgn_EE.r_nr[0]) {
        if (!strcmp(p_dtm, "etrf89") ||
            !strcmp(p_dtm, "etrs89") ||
            !strcmp(p_dtm, "euref89")) {
          (void) strcpy(p_dtm, "eeref89");
        }
      }

      /* datum and datum shift constants */
      if (*p_lb->pr_dtm != '\0') p_dtm = p_lb->pr_dtm;
      (void) strcpy(t_lab, p_dtm);
      c_lab->datum = (short) set_dtm_1(-1, p_dtm, &par_dtm, p_name,
                     e_name, rgn_pref.prfx, &mask, &(c_lab->dsh_con));

      if (c_lab->datum > 0) {
        if (c_lab->p_rgn == 0 && dum_pref.r_nr[0] != rgn_pref.r_nr[0])
            c_lab->p_rgn = rgn_pref.r_nr[0];
        if (c_lab->imit == 0) c_lab->imit = mask;
        c_lab->ellipsoid = (short) set_grs(-1, e_name, ell_p);
      } else {
        c_lab->ellipsoid = (short) set_grs(-1, t_lab, ell_p);
        c_lab->datum     = -2;
      }


/* conv_crd    ver 2012.01          # page  8    31 May 2012 10 19 */


      if (c_lab->ellipsoid >= 0) {
        /* ellipsoid params */
        c_lab->a = *(ell_p+0);
        c_lab->f = *(ell_p+1);
        /* datum shift params (from set_dtm) */
        c_lab->p_dtm = (short) par_dtm;

        /* set trf. constants for completed systems */
        (void) set_trc(c_lab);
  
      }
      else
        /* unintelligible ellipsoid */
        if (c_lab->cstm != 11 /* crt2 */) c_lab->lab_type = ILL_LAB;
    } else {
      c_lab->datum     = 0;
      c_lab->ellipsoid = 0;
    }

    if (conv_mode == 1) {  /* iofile */
      if ((p_lb->q_par && *p_lb->add_p == '\0') || 
          (6 <= c_lab->datum && c_lab->datum <= 8)) {
        (void) fgetln_kms(iofile_str, &i, iofile);
      }
      p_io = iofile_str;
    }  /* iofile */
    else p_io = io_str;

    /* INPUT OF ADDITIONAL PARAMETERS */
    if (p_lb->q_par) {
      if (*p_lb->add_p == '\0') { /* read predifined */
        p_tp = p_lb->par_str;
        switch (p_lb->q_par) {
        case 2:
          c_lab->B1 = sgetg(p_tp, &(c_lab->g_tpd), &used, "sx");
          p_tp     += used;
          (void) sscanf(p_tp, "%lf%n", &(c_lab->scale), &used);
          break;
        case 3:
          f = sgetg(p_tp, &(c_lab->m_tpd), &used, "m");
          if (c_lab->cstm == 6) // upsn / upss
              c_lab->N0 = f;
          else  /* s34 */ c_lab->B0 = f;
          p_tp      += used;
          c_lab->E0  = sgetg(p_tp, &m_tpd, &used, "m");
          p_tp      += used;
          (void) sscanf(p_tp, "%lf%n", &(c_lab->scale), &used);
          break;
        case 4:
        case 5:
        case 6:
          c_lab->B0  = sgetg(p_tp, &(c_lab->g_tpd), &used, "sx");
          p_tp      += used;
          c_lab->N0  = sgetg(p_tp, &(c_lab->m_tpd), &used, "m");
          p_tp      += used;
          c_lab->L0  = sgetg(p_tp, &g_tpd, &used, "sx");
          p_tp      += used;
          c_lab->E0  = sgetg(p_tp, &m_tpd, &used, "m");
          p_tp += used;
          if (p_lb->q_par == 5 && (c_lab->cstm == 3 || c_lab->cstm == 12))
             (void) sscanf(p_tp, "%lf%n", &(c_lab->scale), &used);
          else {
            c_lab->B1 = sgetg(p_tp, &g_tpd, &used, "sx");
            if (p_lb->q_par == 6) { // dlmb
              p_tp      += used;
              c_lab->B2  = sgetg(p_tp, &g_tpd, &used, "sx");
            }
          }
          break;
        }
        c_lab->cmplt = 1;
        p_tp = p_io;
      } else {
        p_tp = p_io;
        if (*p_io != '\0' && strlen(p_io) > 4) {
         switch (p_lb->q_par) {
          case 2:
            c_lab->B1  = sgetg(p_tp, &(c_lab->g_tpd), &used, "sx");
            p_tp      += used;
            (void) sscanf(p_tp, "%lf%n", &(c_lab->scale), &used);
            p_tp      += used;
            break;
          case 3:
            f = sgetg(p_tp, &(c_lab->m_tpd), &used, "m");
            if (c_lab->cstm == 6) // upsn / upss
                c_lab->N0 = f;
            else  /* s34 */ c_lab->B0 = f;
            c_lab->E0  = sgetg(p_tp, &m_tpd, &used, "m");
            (void) sscanf(p_tp, "%lf%n", &(c_lab->scale), &used);
            p_tp      += used;
            break;
          case 4:
          case 5:
          case 6:
            c_lab->B0  = sgetg(p_tp, &(c_lab->g_tpd), &used, "sx");
            p_tp      += used;
            c_lab->N0  = sgetg(p_tp, &(c_lab->m_tpd), &used, "m");
            p_tp      += used;
            c_lab->L0  = sgetg(p_tp, &g_tpd, &used, "sx");
            p_tp      += used;
            c_lab->E0  = sgetg(p_tp, &m_tpd, &used, "m");
            p_tp += used;
            if (p_lb->q_par == 5 && c_lab->cstm == 3)
               (void) sscanf(p_tp, "%lf%n", &(c_lab->scale), &used);
            else {
              c_lab->B1 = sgetg(p_tp, &g_tpd, &used, "sx");
              if (p_lb->q_par == 6) { // dlmb
                p_tp      += used;
                c_lab->B2  = sgetg(p_tp, &g_tpd, &used, "sx");
              }
            }
            p_tp += used;
            break;
          }
          c_lab->cmplt = 1;
        } /* not empty string */
        else {
          c_lab->lab_type = ILL_LAB;
          return (ILL_LAB);
        }
        c_lab->cmplt = 2;
      }  /* input from string */

      if (c_lab->cstm == 6 /*ste*/) {
        if (c_lab->mode == 3) {
          if (c_lab->B0 == M_PI_2 || c_lab->B0 == -M_PI_2) {
            c_lab->mode = (c_lab->B0 > 0.0) ? +1 : -1;
          }
        }
        if (c_lab->mode == 4) {
          if (c_lab->B0 == M_PI_2 || c_lab->B0 == -M_PI_2) {
            c_lab->mode  = (c_lab->B0 > 0.0) ? +2 : -2;
            c_lab->N0    = c_lab->E0 = 2000000.0;
            c_lab->scale = 0.994;
          }
        }
      }  /* ste */
    }  /* end input of additional params */


/* conv_crd    ver 2012.01          # page  9    31 May 2012 10 19 */

     /* INPUT OF DATE at datums: ITRF, IGS, ETRF */
    if (6 <= c_lab->datum && c_lab->datum <= 8) {
      if (conv_mode == 0 || conv_mode == 1) {
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
      }
    }  /* end input of date */

    /* checksum for identification of the label */
    if (*(c_lab->mlb + c_lab->sepix) != 'H')
      c_lab->ch_sum = labchsum((union geo_lab *) c_lab, &c_lab->ch_sum);

    break; /* end conv_mode 0 & 1 */


/* conv_crd    ver 2012.01          # page 10    31 May 2012 10 19 */


  case 3:  /* output of minilabel, simple */
  case 4:  /* output of minilabel, expand */

    if (c_lab->lab_type != CRD_LAB) {
      (void) fprintf(stdout,
             "\nconv_crd called with wrong lab_type %d",
             c_lab->lab_type);
      return(ILL_LAB);
    }

    if (conv_rgn(c_lab->region, rgn_pref.prfx, rgn_name) > 0
         && (c_lab->imit == 0L || c_lab->imit == EDMASK))
       (void) fprintf(iofile, "%s_", rgn_pref.prfx);

    if (*(c_lab->mlb + c_lab->sepix) != 'H' ||
          c_lab->hstm != 33 || c_lab->hmode != 0 ||
          c_lab->h_dtm != 202)
       (void) fprintf(iofile, "%s", c_lab->mlb);
    else {  /* <proj>H<dtm>_h_msl -->> <proj>N<dtm> */
      (void) strcpy(t_lab, c_lab->mlb);
      *(t_lab + c_lab->sepix)   = 'N';
      *(t_lab + c_lab->h_ix -1) = '\0';
      (void) fprintf(iofile, "%s", t_lab);
    }

    if (conv_mode == 4 && c_lab->mode == 9) {
      if (c_lab->cstm == 1)
      (void) fprintf(iofile, "    * crt time series in Data Base ;");
      if (c_lab->cstm == 3)
      (void) fprintf(iofile, "    * utm time series in Data Base ;");
    }

    if ((c_lab->cmplt == 2)
        && (3 <= c_lab->cstm) && (c_lab->cstm <= 6)) {
      g_tpd = *set_tpd("sx", 9, 255);
      m_tpd = *set_tpd("m ", 9, 255);
      switch (p_lb->q_par) {
      case 2:
        /* npstg, spstg: scaled */
        (void) fputg(iofile, c_lab->B1, &g_tpd, "u");
        e  = sqrt(c_lab->f*(2.0 - c_lab->f));
        e  = pow((1.0 - e)/(1.0 + e), e/2.0);
        pM = 2.0*c_lab->a*(1.0-c_lab->f)*e / c_lab->Qn;
        sc = c_lab->scale * pM;
        i  = sprintf(rgn_name, "  %12.10f", sc) -1;
        while (*(rgn_name +i) == '0') -- i;
        if (*(rgn_name +i) == '.') {
          *(rgn_name +i) = '\0';
          (void) fprintf(iofile, "%s.0", rgn_name);
        } else {
          *(rgn_name +i +1) = '\0';
          (void) fprintf(iofile, "%s", rgn_name);
        }
        (void) fprintf(iofile, "  %f", c_lab->scale);
        break;
      case 4:
      case 5:
      case 6:
        (void) fprintf(iofile, "  ");
        (void) fputg(iofile, c_lab->B0, &g_tpd, "u");
        (void) fprintf(iofile, "  ");
        (void) fputg(iofile, c_lab->N0, &m_tpd, "u");
        (void) fprintf(iofile, "  ");
        (void) fputg(iofile, c_lab->L0, &g_tpd, "u");
        (void) fprintf(iofile, "  ");
        (void) fputg(iofile, c_lab->E0, &m_tpd, "u");
        if (p_lb->q_par == 5 && c_lab->cstm == 3) {
          i = sprintf(rgn_name, "  %12.10f", c_lab->scale) -1;
          while (*(rgn_name +i) == '0') -- i;
          if (*(rgn_name +i) == '.') {
            *(rgn_name +i) = '\0';
            (void) fprintf(iofile, "%s.0", rgn_name);
          } else {
            *(rgn_name +i +1) = '\0';
            (void) fprintf(iofile, "%s", rgn_name);
          }
        } else {
          (void) fprintf(iofile, "  ");
          (void) fputg(iofile, c_lab->B1, &g_tpd, "u");
          if (p_lb->q_par == 6) { // dlmb
            (void) fprintf(iofile, "  ");
            (void) fputg(iofile, c_lab->B2, &g_tpd, "u");
          }
        }
        break;
      }
    }  /* complete lab */

    /* OUTPUT OF DATE at datums: ITRF, IGS, ETRF */
    if (6 <= c_lab->datum && c_lab->datum <= 8) {
      if (c_lab->date > 0.0) {
        (void) fprintf(iofile, "  ");
        (void) fputcw(iofile, c_lab->date, 2);
      } else (void) fprintf(iofile, "  * date at coords ;");
    }  /* end output of date */

    if (conv_mode == 4) {
      (void) fprintf(iofile, "\n*\n");

      /* collect text of the coord kind */
      if (0 < c_lab->cstm && c_lab->cstm < 10)
          (void) fprintf(iofile,
          "\nCoordinates:        %s", c_types[c_lab->cstm - 1]);

      /* Region info */
      if (c_lab->region != 0 &&
          (conv_rgn(c_lab->region, rgn_pref.prfx, rgn_name) > 0)) {
        (void) fprintf(iofile, "\nRegion:%-6s   %6s -> %-15s",
                       " ", rgn_pref.prfx, rgn_name);
      }


/* conv_crd    ver 2012.01          # page 11    31 May 2012 10 19 */


      /* Datum, ellipsoid, and parent datum */

      if (c_lab->datum > 0
          && set_dtm_1(c_lab->datum, p_dtm, &par_dtm, p_name,
                     e_name, rgn_pref.prfx, &mask, &d_sh) > 0)
         (void) fprintf(iofile, "\nDatum:     %9s%s", " ", p_dtm);
      if (c_lab->p_rgn != 0 &&
          (conv_rgn(c_lab->p_rgn, rgn_pref.prfx, rgn_name) > 0)) {
        (void) fprintf(iofile, "\nProj.Rgn:%-6s   %6s -> %-15s",
                       " ", rgn_pref.prfx, rgn_name);
      }
      if (c_lab->ellipsoid > 0 &&
          set_grs(c_lab->ellipsoid, e_name, ell_p) > 0) {
        (void) fprintf(iofile, "\nEllipsoid: %9s%s", " ", e_name);
        (void) fprintf(iofile,
                    "\n  a       =        %14.4f m", c_lab->a);
        f = c_lab->f;
        if (0.0 < f && f < 1.0)
          (void) fprintf(iofile, "\n  1/f     =       %12.5f", 1 / f);
        else
          (void) fprintf(iofile, "\n  f       =       %12.5f", f);
        if (c_lab->cstm == 4 && c_lab->mode == 3) {
          (void) fprintf(iofile, "\nCalcDatum: %9sgoogrs80", " ");
          (void) fprintf(iofile, "\nCalcEllip: %9sspgrs80", " ");
          (void) fprintf(iofile, "\n  f       =       %12.5f", 0.0);
        }
      }

      if (*(c_lab->mlb + c_lab->sepix) == 'H') {
        if (c_lab->h_dtm > 0
            && set_dtm_1(c_lab->h_dtm, p_dtm, &par_dtm, p_name,
                     e_name, rgn_pref.prfx, &mask, &d_sh) > 0)
           (void) fprintf(iofile, "\nHeight Datum: %9s%s", " ", p_dtm);
      }

      (void) fprintf(iofile, "\n;\n");

    } /* end conv_mode == 4 */

    break; /* end case 3 & 4, output label */


/* conv_crd    ver 2012.01          # page 12    31 May 2012 10 19 */


  case 5:   /* DOCUMENTATION OF THE LABEL */

    if (c_lab->lab_type != CRD_LAB) {
      (void) fprintf(stdout,
             "\nconv_crd called with wrong lab_type %d",
             c_lab->lab_type);
      c_lab->lab_type = ILL_LAB;
      return(ILL_LAB);
    }

    (void) fprintf(iofile, "\n\f     COORD-LABEL:\n # ");
    if (conv_rgn(c_lab->region, rgn_pref.prfx, rgn_name) > 0
        && c_lab->imit == 0L)
       (void) fprintf(iofile, "%s_", rgn_pref.prfx);

    if (*(c_lab->mlb + c_lab->sepix) != 'H' ||
          c_lab->hstm != 33 || c_lab->hmode != 0 ||
          c_lab->h_dtm != 202)
       (void) fprintf(iofile, "%s", c_lab->mlb);
    else {  /* <proj>H<dtm>_h_msl -->> <proj>N<dtm> */
      (void) strcpy(t_lab, c_lab->mlb);
      *(t_lab + c_lab->sepix)   = 'N';
      *(t_lab + c_lab->h_ix -1) = '\0';
      (void) fprintf(iofile, "%s", t_lab);
    }

    if ((c_lab->cmplt == 2)
        && (3 <= c_lab->cstm) && (c_lab->cstm <= 6)) {
      g_tpd = *set_tpd("sx", 9, 255);
      m_tpd = *set_tpd("m ", 9, 255);
      switch (p_lb->q_par) {
      case 2:
        /* npstg, spstg: scaled */
        (void) fprintf(iofile, "  ");
        (void) fputg(iofile, c_lab->B1, &g_tpd, "u");
        e  = sqrt(c_lab->f*(2.0 - c_lab->f));
        e  = pow((1.0 - e)/(1.0 + e), e/2.0);
        pM = 2.0*c_lab->a*(1.0-c_lab->f)*e / c_lab->Qn;
        sc = c_lab->scale * pM;
        i  = sprintf(rgn_name, "  %12.10f", sc) -1;
        while (*(rgn_name +i) == '0') -- i;
        if (*(rgn_name +i) == '.') {
          *(rgn_name +i) = '\0';
          (void) fprintf(iofile, "%s.0", rgn_name);
        } else {
          *(rgn_name +i +1) = '\0';
          (void) fprintf(iofile, "%s", rgn_name);
        }
        (void) fprintf(iofile, "  %f", c_lab->scale);
        break;
      case 4:
      case 5:
      case 6:
        (void) fprintf(iofile, "  ");
        (void) fputg(iofile, c_lab->B0, &g_tpd, "u");
        (void) fprintf(iofile, "  ");
        (void) fputg(iofile, c_lab->N0, &m_tpd, "u");
        (void) fprintf(iofile, "  ");
        (void) fputg(iofile, c_lab->L0, &g_tpd, "u");
        (void) fprintf(iofile, "  ");
        (void) fputg(iofile, c_lab->E0, &m_tpd, "u");
        if (p_lb->q_par == 5 && c_lab->cstm == 3) {
          i = sprintf(rgn_name, "  %12.10f", c_lab->scale) -1;
          while (*(rgn_name +i) == '0') -- i;
          if (*(rgn_name +i) == '.') {
            *(rgn_name +i) = '\0';
            (void) fprintf(iofile, "%s.0", rgn_name);
          } else {
            *(rgn_name +i +1) = '\0';
            (void) fprintf(iofile, "%s", rgn_name);
          }
        } else {
          (void) fprintf(iofile, "  ");
          (void) fputg(iofile, c_lab->B1, &g_tpd, "u");
          if (p_lb->q_par == 6) { // dlmb
            (void) fputg(iofile, c_lab->B2, &g_tpd, "u");
          }
        }
        break;
      }
      (void) fprintf(iofile, "\n");
    }  /* complete lab */

    g_tpd = *set_tpd("sx", 12, 6);
    m_tpd = *set_tpd("m ", 12, 8);

    if (c_lab->cstm == 1 && c_lab->mode == 9)
        (void) fprintf(iofile, "    * crt time series in Data Base ;");

    /* OUTPUT OF DATE at datums: ITRF, IGS, ETRF */
    if (6 <= c_lab->datum && c_lab->datum <= 8) {
      if (c_lab->date > 0.0) {
        (void) fprintf(iofile, "  ");
        (void) fputcw(iofile, c_lab->date, 2);
      } else (void) fprintf(iofile, "  * date at coords ;");
    }  /* end output of date */

    sepch = *(c_lab->mlb + c_lab->sepix);
    if (sepch == '\0') sepch = ' ';
    /* General info */
    (void) fprintf(iofile, "\nlng       = %15ld", (long) sizeof (*c_lab));
    (void) fprintf(iofile, "\nlab_type  = %15d", c_lab->lab_type);
    (void) fprintf(iofile,
                   "         version   = %15d", c_lab->version);
    (void) fprintf(iofile, "\nmlb       = %15s", c_lab->mlb);
    (void) fprintf(iofile, "\nsepch     = %15c", sepch);
    (void) fprintf(iofile, "         compl     = %15d", c_lab->cmplt);
    (void) fprintf(iofile, "\ninit      = %15d", c_lab->init);
    (void) fprintf(iofile, "         p_seq     = %15d", c_lab->p_seq);
    (void) fprintf(iofile, "\nncoord    = %15d", c_lab->ncoord);
    /* Region info */
    (void) fprintf(iofile, "         region    = %15d", c_lab->region);
    if (c_lab->region != 0) {
      if (conv_rgn(c_lab->region, rgn_pref.prfx, rgn_name) > 0)
         (void) fprintf(iofile, "\n%27s%9s%-6s    =     %-15s",
                        " ", " ", rgn_pref.prfx, rgn_name);
    }
    (void) fprintf(iofile, "\ndatum     = %15d", c_lab->datum);
    if (c_lab->datum > 0) {
      i = set_dtm_1(c_lab->datum, p_dtm, &par_dtm,
                    p_name, e_name, rgn_pref.prfx, &mask, &d_sh);
      if (i > 0) (void) fprintf(iofile, "%9s%s", " ", p_dtm);
      if (c_lab->p_rgn != 0 &&
          (conv_rgn(c_lab->p_rgn, rgn_pref.prfx, rgn_name) > 0)) {
        (void) fprintf(iofile, "\nProj.Rgn  = %6s -> %-15s",
                       rgn_pref.prfx, rgn_name);
      }
      /* Ellipsoid, and parent datum */
      (void) fprintf(iofile, "\nellipsoid = %15d", c_lab->ellipsoid);
      if (set_grs(c_lab->ellipsoid, e_name, ell_p) > 0)
          (void) fprintf(iofile, "%9s%s", " ", e_name);
      /* get the datum name */
      if (i > 0) {
        /* get the parent datum name */
        if (set_dtm_1(c_lab->p_dtm, p_dtm, &par_dtm,
                    p_name, e_name, rgn_pref.prfx, &mask, &d_sh) > 0) {
          (void) fprintf(iofile, "\npar. dtm  = %15d", c_lab->p_dtm);
          (void) fprintf(iofile, "%9s%s", " ", p_dtm);
          /* p_name from first call could be used */
          /* but then is the p_dtm number not checked */
        }
      }
      if (c_lab->cstm == 4 && c_lab->mode == 3) {
        (void) fprintf(iofile, "\nCalcDatum: %9sgoogrs80", " ");
        (void) fprintf(iofile, "\nCalcEllip: %9sspgrs80", " ");
        (void) fprintf(iofile, "\n  f       =       %12.5f", 0.0);
      }
    }
    if (sepch == 'H') {
      if (c_lab->h_dtm > 0
          && set_dtm_1(c_lab->h_dtm, p_dtm, &par_dtm, p_name,
                   e_name, rgn_pref.prfx, &mask, &d_sh) > 0) {
        (void) fprintf(iofile, "\nHeight Dtm= %15d", c_lab->h_dtm);
        (void) fprintf(iofile, "%9s%s", " ", p_dtm);
        (void) fprintf(iofile, "\nHeight ix = %15d", c_lab->h_ix);
        (void) fprintf(iofile, "%9s%s", " ", c_lab->mlb+c_lab->h_ix);
      }
    }


/* conv_crd    ver 2012.01          # page 13    31 May 2012 10 19 */


    /* Enumeration of coord system */
    (void) fprintf(iofile, "\ncstm      = %15d", c_lab->cstm);
    (void) fprintf(iofile, "         mode      = %15d", c_lab->mode);
    (void) fprintf(iofile, "\nS_crd     = %15d", c_lab->S_crd);
    (void) fprintf(iofile, "         W_crd     = %15d", c_lab->W_crd);

    /* Ellipsoidal parameters */
    for (i = sizeof (t_lab_a) - 1; i >= 0; i--) *(t_lab_a + i) = '\0';
    (void) sputg(t_lab_a, c_lab->a, &m_tpd, "g ");
    (void) fprintf(iofile, "\na         = %s", t_lab_a);
    f = c_lab->f;
    if (0.0 < f && f < 1.0)
      (void) fprintf(iofile, "\n1/f       = %21.14f", 1 / f);
    else
      (void) fprintf(iofile, "\nf         = %21.14f", f);
    (void) fprintf(iofile, "  (n       = %15.14e)", f / (2.0 - f));
    (void) fprintf(iofile, "\n (e2      = %21.14e)", f * (2.0 - f));
    (void) fprintf(iofile, " (eprim2  = %15.14e)",
                   f * (2.0 - f) / (1.0 - f) / (1.0 - f));

    /* Origo and scale  */
    (void) sputg(t_lab, c_lab->B0, &g_tpd, "g ");
    (void) fprintf(iofile, "\nB0        = %s", t_lab);
    (void) sputg(t_lab, c_lab->N0, &m_tpd, "g ");
    (void) fprintf(iofile, "\nN0        = %s", t_lab);
    (void) sputg(t_lab, c_lab->L0, &g_tpd, "g ");
    (void) fprintf(iofile, "\nL0        = %s", t_lab);
    (void) sputg(t_lab, c_lab->E0, &m_tpd, "g ");
    (void) fprintf(iofile, "\nE0        = %s", t_lab);
    (void) fprintf(iofile, "\nscale     = %21.14f", c_lab->scale);

    /* Additional parameters, tolerance, and spec. region */
    (void) sputg(t_lab, c_lab->B1, &g_tpd, "g ");
    (void) fprintf(iofile, "\nB1        = %s", t_lab);
    (void) sputg(t_lab, c_lab->B2, &g_tpd, "g ");
    (void) fprintf(iofile, "\nB2        = %s", t_lab);
    (void) fprintf(iofile, "\ntol       =      %15.0f my",
                   c_lab->tol * 1.0e6);
    (void) fprintf(iofile, "\nimit      =  %15d", c_lab->imit);

    /* Meridian arc, polar radii, cone const. etc. */
    (void) sputg(t_lab, c_lab->Qn, &m_tpd, "g ");
    (void) fprintf(iofile, "\nQn        = %15s", t_lab);
    (void) sputg(t_lab, c_lab->Zb, &m_tpd, "g ");
    (void) fprintf(iofile, "\nZb        = %s", t_lab);

    if (c_lab->cstm != 8 /* s34 */ )
       (void) fprintf(iofile, "\ncP        = %15.14f", c_lab->cP);
    else
       (void) fprintf(iofile, "\ns34 curv. = %15.14e", c_lab->cP);


/* conv_crd    ver 2012.01          # page 14    31 May 2012 10 19 */


    if (c_lab->cstm != 7) {
      /* Gaussian latitude coeff. */
      p = c_lab->tcgg;
      (void) fprintf(iofile, "\n\n        Gauss -> Geogr.");
      (void) fprintf(iofile, "                  Geogr. -> Gauss");
      for (i = 0; i < 4; i++) {
        (void) fprintf(iofile, "\n  %3d   %+22.14e", i, *(p + i));
        (void) fprintf(iofile, "      %3d   %+22.14e",
                       i + 4, *(p + i + 4));
      }

      /* UTM coefficients */
      p = c_lab->utg;
      (void) fprintf(iofile, "\n\n        UTM -> Geogr.");
      (void) fprintf(iofile, "                    Geogr. -> UTM");
      for (i = 0; i < 4; i++) {
        (void) fprintf(iofile, "\n  %3d   %+22.14e", i, *(p + i));
        (void) fprintf(iofile, "      %3d   %+22.14e", i, *(p + i + 4));
      }
    } else {
      /* Equivalent projections : Authalic latitude coeff. */
      p = c_lab->tcgg;
      (void) fprintf(iofile, "\n\n     Authalic -> Geogr.");
      (void) fprintf(iofile, "                  Geogr. -> Authalic");
      for (i = 0; i < 5; i++) {
        (void) fprintf(iofile, "\n  %3d   %+22.14e", i, *(p + i));
        (void) fprintf(iofile, "      %3d   %+22.14e",
                       i + 5, *(p + i + 5));
      }
    }

    /* Datum shift params */
    d_s = &(c_lab->dsh_con);
    (void) fprintf(iofile, "\n\n   Datum shift parameters");
    (void) fprintf(iofile, "\n    type   %4d", d_s->tp);
    (void) fprintf(iofile, "\n    tx     %+8.3f m", d_s->tx);
    (void) fprintf(iofile, "\n    ty     %+8.3f m", d_s->ty);
    (void) fprintf(iofile, "\n    tz     %+8.3f m", d_s->tz);
    (void) fprintf(iofile, "\n    dm     %+7.6e", d_s->scale - 1.0);
    (void) fprintf(iofile, "\n  ROTATION-MATRIX - (DIAGONAL 1.0).");
    (void) fprintf(iofile, "\n    %+9.6e  %+9.6e  %+9.6e",
                   d_s->r11 - 1.0, d_s->r12, d_s->r13);
    (void) fprintf(iofile, "\n    %+9.6e  %+9.6e  %+9.6e",
                   d_s->r21, d_s->r22 - 1.0, d_s->r23);
    (void) fprintf(iofile, "\n    %+9.6e  %+9.6e  %+9.6e",
                   d_s->r31, d_s->r32, d_s->r33 - 1.0);
    (void) fprintf(iofile, "\n\n%s slut\n", c_lab->mlb);

    break;   /* end case 5, documentation */

  } /* end switch(conv_mode) */

  return(c_lab->lab_type);
}

