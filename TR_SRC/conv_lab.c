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
 


/* conv_lab  ver 2010.01            # page 1     5 Jan 2010 15 35 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

/* conv_lab(name, lab, ...) converts the name of a lab to the lab   */
/*    result ==  0 : no lab                                    */
/*    result == CRD_LAB : coord lab                    in lab  */
/*    result == HGT_LAB : height lab                   in lab  */
/*    result == GDE_LAB : geoid                        in lab  */
/*    result == WOR_LAB : wor lab                      in lab  */
/*    result == POC_LAB : Photo OC lab                 in lab  */
/*    result == DTM_LAB : Digital Terrain ModeL lab    in lab  */
/*    result == DHH_LAB : Diff Height Model lab        in lab  */
/*    result == UPL_LAB : landUPLift model lab         in lab  */
/*    result == OBS_LAB : obs lab                      in lab  */
/*    result == DEM_LAB : Digital Elevation ModeL lab  in lab  */
/*    result == TAB_LAB : unspecified Table lab        in lab  */
/*    result == PLM_LAB : plate motion lab             in lab  */
/*    result == BAG_LAB : Bouguer anomaly gravi table  in lab  */
/*    result == T3D_LAB : 3 dim table                  in lab  */
/*    result == T2D_LAB : 2 dim table                  in lab  */
/*    result == TDD_LAB : 2 dim table doubles          in lab  */
/*    result == CND_LAB : conditions to adj sys        in lab  */
/*    result == IDT_LAB : idt lab                      in lab  */
/*    result == JDF_LAB : jdf lab                      in lab  */
/*    UNDEFSYS     : unrecognized name                         */
/*    result == STP_LAB : stop lab                             */
/* an unrecognized name is considered a filename containing a  */
/* genuine label                                               */
/* ... must be either: ""  or *file                            */
/* when converting string or input from file (NULL is leagal)  */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <math.h>

#include "geo_lab.h"
#include "fgetlhtx.h"
#include "trthread.h"

int               conv_lab(
/*________________________*/
char             *lab_name,
union geo_lab    *u_lab,
...
)

{
#include              "conv_cnd.h"
#include              "conv_crd.h"
#include              "conv_hgt.h"
#include              "conv_obs.h"
#include              "conv_plm.h"
#include              "conv_tab.h"
#include              "conv_spc.h"
#include              "get_mlb.h"
#include              "labchsum.h"


/* conv_lab  ver 2010.01            # page 2     5 Jan 2010 15 35 */


  int srch_def(int mode, char *p_sys, char *w_sys, int s_type,
               int s_cstm, int s_mode, struct lab_def_str *p_lb);

  FILE                      *fc;
  FILE                      *iofile = (FILE *) NULL;
  char                      *io_str = (char *) NULL;
  char                      *descript;
  va_list                    o_p;
  struct coord_lab          *c_lab = &(u_lab->u_c_lab);
  struct hgt_lab            *h_lab = &(u_lab->u_h_lab);
  struct gde_lab            *t_lab = &(u_lab->u_g_lab);
  struct plm_lab            *p_lab = &(u_lab->u_m_lab);
  struct obs_lab            *o_lab = &(u_lab->u_o_lab);
  struct idt_lab            *i_lab = &(u_lab->u_i_lab);

  static THREAD_SAFE int     c_quest = 0, s_quest = 0, recurs = 0;
  int                        conv_mode;
  char                      *OTX;

  int                        i, res = 0;
  int                        lab_lng;
  short                      h_type = ILL_LAB;
  char                       in_str[256];

  char                       t_info_a[8*MLBLNG], *t_info = t_info_a;
  char                       w_sys_a[MLBLNG], *w_sys = w_sys_a;
  
  struct lab_def_str         lb_ln_ex, *p_lb = &lb_ln_ex;
  char                      *p_dtm = p_lb->mlb2;
  char                      *p_sys = p_lb->name;
  char                      *h_mlb, *h_mlb2, *p_tp;
  union rgn_un               DK_rgn;

  /* region number, prefix and name */
  short                      lab_rgn = 0;

  struct sep_str {
    char       sepch;
    short      lab_type;
    char       *text;
  };

  static struct sep_str   sep[] = {
    {'_',   CRD_LAB,   "NO Heights"},
    {'E',   CRD_LAB,   "Ellipsoidal heights"},
    {'N',   CRD_LAB,   "--> H _h_msl"},
    {'H',   CRD_LAB,   "Height System included"},
    {'L',   CRD_LAB,   "Location coord"},
    {'_',   OBS_LAB,   "Input of obs_mean_error"},
    {'D',   OBS_LAB,   "Std obs_mean_error"},
    {'\0',  0,   ""}
  };
  struct sep_str           *sp;

  static THREAD_SAFE struct sep_str   *spr;
  int                       sep_ok = 0;


/* conv_lab  ver 2010.01            # page 3     5 Jan 2010 15 35 */


  (void) strcpy(DK_rgn.prfx, "DK");
  p_lb->d_kind   = 0;
  p_lb->d_type   = 0;
  p_lb->lab_type = ILL_LAB;
  for (i = 8*MLBLNG; i > 0; ) t_info[-- i] = '\0';
  /* save lab_name */
  (void) strcpy(t_info, lab_name);
  lab_lng = (int) strlen(t_info);

  conv_mode = (!strcmp(t_info,  "input"))       ? 1
            : (!strcmp(t_info, "in_u_#"))       ? 2
            : (!strcmp(t_info, "output"))       ? 3
            : (!strcmp(t_info, "OUTPUT"))       ? 4
            : (!strcmp(t_info, "SURVEY"))       ? 5
            : (*t_info == '?' /* ordinary info */
            || *t_info == '*' /* detailed info */
            || *t_info == '&' /* sep */)        ? 6 : 0;

  va_start(o_p, u_lab);
  if (conv_mode >= 1 && conv_mode <= 5) iofile = va_arg(o_p, FILE *);
  else
  if (conv_mode == 0) {
    io_str = va_arg(o_p, char *);
    if((char *) io_str == (char *) lab_name) io_str = NULL;
  }
  va_end(o_p);

  if (conv_mode == 1 || conv_mode == 2) {
    /* Collect the minilab from iofile */
    if (conv_mode == 1) {
      res     = fgetlhtx(iofile, in_str);
      lab_lng = (int) strlen(in_str);
    } else
      res = (fscanf(iofile, "%s%n", in_str, &lab_lng) == 0);
    if (res == 0) (void) sscanf(in_str, "%s%n", t_info, &lab_lng);
    else {
      /* clean lab (i_lab is p.t. shortest) */
      p_tp = (char *) i_lab;
      for (i = sizeof(*i_lab) -1; i >= 0; i--) *(p_tp+i) = 0;
      if (res != EOF) {
        (void) fprintf(stdout, "\n***conv_lab: error in label input");
        i_lab->lab_type = ILL_LAB;
        return(ILL_LAB);
      }
    }
  } /* end conv_mode = 1 || 2 */

  switch(conv_mode) {
  case 0: /* ordinary conv_lab       */
  case 1: /* input from file starting with    : #  */
  case 2: /* input from file starting without : #  */

    /* clean lab (i_lab is p.t. shortest) */
    p_tp = (char *) i_lab;
    for (i = sizeof(*i_lab) -1; i >= 0; i--) *(p_tp+i) = 0;

    if (res != EOF) {
      /* test for empty lab_name */
      if (*(t_info) == '\0') {
        i_lab->lab_type = STP_LAB;
        return(STP_LAB);
      }


/* conv_lab  ver 2010.01            # page 11     5 Jan 2010 15 35 */


      if (strlen(t_info) >= 2) {
        p_lb->lab_rgn = 0; 
        lab_lng = get_mlb(t_info,
                          &lab_rgn, p_sys, &p_lb->sepch, p_dtm, &h_mlb);
        p_lb->region = lab_rgn; 
        /* in mode 0 may extra params be put in io_str */
        /*                            or in t_info after mlb */
        if (conv_mode == 0) {
          if (lab_lng < ((int) strlen(t_info))-1)
            (void) strcpy(in_str, t_info + lab_lng);
          else *in_str = '\0';
          if (io_str != NULL) (void) strcat(in_str, io_str);
        }
/*
(void) fprintf(stdout, "\n%s\n%s %d %s %d %s",
"\nconv_lab: t_info, lab_rgn, p_sys, sepch, p_dtm, h_mlb:",
t_info, lab_rgn, p_sys, p_lb->sepch, p_dtm);
if (h_mlb != NULL) (void) fprintf(stdout, ",  %s", h_mlb);
*/

        /* search the def_lab_file for p_sys (or w_sys) */ 
        i = srch_def(0, p_sys, w_sys, 0, 0, 0, p_lb);
        if (i < 0) return(i);

        if (p_lb->region == 0) {
          p_lb->region = p_lb->lab_rgn; 
        }

        /* SEPARATOR TEST */
        if (p_lb->sepch != '\0' &&
            (p_lb->lab_type == CRD_LAB || p_lb->lab_type == OBS_LAB)) {
          for (sep_ok = 0, sp = sep; sp->sepch; sp++) {
            if (sp->lab_type == p_lb->lab_type && sp->sepch == p_lb->sepch) {
              sep_ok = 1;
              break;
            }
          }
          if (!sep_ok) {
            (void) fprintf(stdout,
                   "\nconv_lab: illegal separator %c", p_lb->sepch);
            return (ILL_LAB);
          }
        }
        
        if (p_lb->lab_type == ILL_LAB) {
          /* unkown lab_type : try GDE_LAB first - then file */
          p_lb->lab_type = GDE_LAB;
        }
      } else {
          /* too short lab_type : try file */
          p_lb->lab_type   = ILL_LAB;
      }


/* conv_lab  ver 2010.01            # page 12     5 Jan 2010 15 35 */


      switch (p_lb->lab_type) {
      case CRD_LAB: /* coordinates */
        p_lb->lab_type = (conv_mode == 0)
                       ? (short) conv_crd("", p_lb, c_lab, in_str)
                       : (short) conv_crd("input", p_lb, c_lab, iofile);
        if (p_lb->sepch == 'N') {
          *(c_lab->mlb + c_lab->sepix) = 'H';
          h_mlb = t_info + lab_lng +1;
          (void) strcpy(h_mlb,
                 (DK_rgn.r_nr[0] == c_lab->p_rgn ||
                  DK_rgn.r_nr[0] == lab_rgn) ? "h_dvr90" : "h_msl");
        }
        if (p_lb->lab_type == CRD_LAB) {
          if (c_lab->cstm != 1) {
            if (*(c_lab->mlb + c_lab->sepix) == 'H' && h_mlb != NULL) {
              union geo_lab    uh_lab;
              struct hgt_lab  *hh_lab = &(uh_lab.u_h_lab);
              /* p_lb has been saved to c_lab */
              p_lb->lab_rgn = 0; 
              lab_lng       = get_mlb(h_mlb, &lab_rgn, p_sys,
                              &p_lb->sepch, p_dtm, &h_mlb2);

              /* search the def_lab_file for p_sys (or w_sys) */ 
              i = srch_def(0, p_sys, w_sys, 0, 0, 0, p_lb);
              if (i < 0) return(i);

              if (p_lb->lab_type == HGT_LAB)
                  h_type = (short) conv_hgt("", p_lb, hh_lab, h_mlb);
              if (h_type == HGT_LAB) {
                p_lb->lab_type = CRD_LAB;  // RESET
                if (c_lab->imit == 0) c_lab->imit = hh_lab->imit;
                if (c_lab->p_rgn == 0) c_lab->p_rgn = hh_lab->p_rgn;
                if (c_lab->region == 0) c_lab->region = c_lab->p_rgn;
                c_lab->hstm    = hh_lab->cstm;
                c_lab->hmode   = hh_lab->mode;
                c_lab->h_dtm   = hh_lab->datum;
                c_lab->h_ix    = (short) strlen(c_lab->mlb) +1;
                (void) strcat(c_lab->mlb, "_");
                (void) strcat(c_lab->mlb, hh_lab->mlb);
                c_lab->ch_sum = labchsum(u_lab, &c_lab->ch_sum);
              } else {
                c_lab->lab_type = ILL_LAB;
                p_lb->lab_type  = ILL_LAB;
              }
            } else
            if (*(c_lab->mlb + c_lab->sepix) == 'E') {
              c_lab->hstm    = 0;
              c_lab->hmode   = 0;
              c_lab->h_dtm   = 200;
            }
          } else
          if (c_lab->cstm == 1 && *(c_lab->mlb + c_lab->sepix) == '_') {
            c_lab->hstm    = 0;
            c_lab->hmode   = 0;
            c_lab->h_dtm   = 200;
          } else
          if ((c_lab->cstm == 1 || h_mlb == NULL)
              && *(c_lab->mlb + c_lab->sepix) == 'H') {
            c_lab->lab_type = ILL_LAB;
            p_lb->lab_type  = ILL_LAB;
          }
        }
        break; /* end case 1, coordinates */

      case HGT_LAB: /* heights */
        p_lb->lab_type = (conv_mode == 0)
                       ?  (short) conv_hgt("", p_lb, h_lab, in_str)
                       :  (short) conv_hgt("input", p_lb, h_lab, iofile);
        break; /* end case HGT_LAB, heights */

      case OBS_LAB: /* observations */
        p_lb->lab_type = (conv_mode == 0)
                       ?  (short) conv_obs("", p_lb, o_lab, in_str)
                       :  (short) conv_obs("input", p_lb, o_lab, iofile);
        break; /* case 2, observations */

      case TAB_LAB: /* Tables */
      case GDE_LAB: /* Geoids */
        p_lb->lab_type = (conv_mode == 0)
                       ? (short) conv_tab("", p_lb, t_lab, in_str)
                       : (short) conv_tab("input", p_lb, t_lab, iofile);
        break; /* case 3, Geoids */

      case PLM_LAB: /* PLate_Motion-label */
        p_lb->lab_type = (conv_mode == 0) 
                       ? (short) conv_plm("", p_lb, p_lab, in_str)
                       : (short) conv_plm("input", p_lb, p_lab, iofile);
        break; /* case 3, Geoids */

      case CND_LAB: /* Condition observations */
        p_lb->lab_type = (conv_mode == 0)
                       ? (short) conv_cnd("", p_lb, o_lab, in_str)
                       : (short) conv_cnd("input", p_lb, o_lab, iofile);
        break;

      case WOR_LAB: /* Write_Out_Result-label */
      case POC_LAB: /* Photo_orientation_constant-label */
        p_lb->lab_type = (conv_mode == 0)
                       ? (short) conv_spc("", p_lb, u_lab, in_str)
                       : (short) conv_spc("input", p_lb, u_lab, iofile);
        break;

      case IDT_LAB: /* Ident-label */
      case JDF_LAB: /* Jobdef-label */

        /* i_lab has already been cleared */
        (void) strcpy(i_lab->mlb, p_sys);
        i_lab->lab_type = p_lb->lab_type;
        i_lab->version  = LAB_VERSION;
        i_lab->sepix    = (short) strlen(t_info);
        i_lab->idt_1    = 0;
        i_lab->idt_2    = p_lb->d_kind;
        i_lab->idt_3    = 0;
        i_lab->region   = p_lb->region;

        if ((!p_lb->region) && p_lb->lab_type == IDT_LAB) {
          (void) fprintf(stdout,
                 "\n***Illegal IDT-label - missing region");
          p_lb->lab_type = ILL_LAB;
        }
        break;


/* conv_lab  ver 2010.01            # page 13     5 Jan 2010 15 35 */


      case STP_LAB: /* stop-label */
        /* PT bruges i_lab for stop-label    */
        i_lab->lab_type = STP_LAB;
        (void) strcpy(i_lab->mlb, "stop");
        break;

      default: /* unintelligible label */
        i_lab->lab_type = ILL_LAB;
        p_lb->lab_type  = ILL_LAB;
        break;
      } /* end switch(lab_type) */

      /* Input from file when compare fails */
      /*____________________________________*/
      if (-2 <= p_lb->lab_type && p_lb->lab_type <= ILL_LAB) {
      /*  NOT geoid        NOT FOUND          */
        if (!recurs) {
          recurs = 1;
          if ((fc = fopen(t_info, "r")) == NULL) {
            (void) fprintf(stdout,
               "\n*** conv_lab: ILLEGAL or FILE %s not found;", t_info);
            recurs = 0;
            return(UNDEFSYS);
          }

          p_lb->lab_type = (short) conv_lab("input", u_lab, fc);
          (void) fclose(fc);
          recurs = 0;
          if (p_lb->lab_type > ILL_LAB) return(p_lb->lab_type);
          else {
            (void) fprintf(stdout,
                   "\n*** conv_lab: %s not a label;", p_sys);
            return(UNDEFSYS);
          }
        } /* ! recurs */
        else {
          recurs = 0;
          return(UNDEFSYS);
        }
      } /* ILL_LAB */

    }
    else { /* EOF */
      p_lb->lab_type  = STP_LAB;
      i_lab->lab_type = STP_LAB;
    }

    break;


/* conv_lab  ver 2010.01            # page 14     5 Jan 2010 15 35 */


  case 3: /* ordinary output */
  case 4: /* special  output */
  case 5: /* label    output */

    OTX = (conv_mode == 3) ? "output"
        :((conv_mode == 4) ? "OUTPUT" : "SURVEY");

    /* get the label info text from def_prj lb_tb_ex */
    /* search the def_lab_file for type cstm and mode */ 
    i = srch_def(1, p_sys, w_sys, i_lab->lab_type, i_lab->idt_2, 
                 (c_lab->lab_type != 9) ? i_lab->idt_3 : 0, p_lb);
    if (i < 0) return(i);

    switch(c_lab->lab_type) {
    case CRD_LAB:
      p_lb->lab_type = (short) conv_crd(OTX, p_lb, c_lab, iofile);
      break;

    case HGT_LAB:
      p_lb->lab_type = (short) conv_hgt(OTX, p_lb, h_lab, iofile);
      break;

    case GDE_LAB:
    case DTM_LAB:
    case DHH_LAB:
    case UPL_LAB:
    case DEM_LAB:
    case TAB_LAB:
    case BAG_LAB:
    case T3D_LAB:
    case T2D_LAB:
    case TDD_LAB:
      p_lb->lab_type = (short) conv_tab(OTX, p_lb, t_lab, iofile);
      break;

    case PLM_LAB:
      p_lb->lab_type = (short) conv_plm(OTX, p_lb, p_lab, iofile);
      break;

    case CND_LAB: /* Condition observations */
      p_lb->lab_type = (short) conv_cnd(OTX, p_lb, o_lab, iofile);
      break;

    case WOR_LAB:
    case IDT_LAB:
    case JDF_LAB:
    case POC_LAB:
      p_lb->lab_type = (short) conv_spc(OTX, p_lb, u_lab, iofile);
      break;

    case OBS_LAB: /* observations */
      p_lb->lab_type = (short) conv_obs(OTX, p_lb, o_lab, iofile);
      break;

    case STP_LAB: /* observations */
      p_lb->lab_type = STP_LAB;
      (void) fprintf(iofile, "stop");
      break;

    default:
      (void) fprintf(stdout,
             "\nconv_lab : ILLEGAL not programmed labtype = %d    %d",
             p_lb->lab_type, t_lab->lab_type);
      p_lb->lab_type = ILL_LAB;
      break;
    }
    break;


/* conv_lab  ver 2010.01            # page 15     5 Jan 2010 15 35 */


  case 6: /* List of cstms and separators */
    va_start(o_p, u_lab);
    descript = va_arg(o_p, char *);
    va_end(o_p);
    (void) strcpy(p_sys, t_info +1);

    if (*t_info == '?' || *t_info == '*') {
      /* Coord, height, geoid, and obs systems info */
      if (strlen(p_sys) == 0) {
        /*all systems*/
        /* search the def_lab_file for next or first */ 
        if (c_quest == 63) return(-1);
        i = srch_def((c_quest) ? 3 : 2, 
                     p_sys, w_sys, 0, 0, 0, p_lb);
        if (i < 0) return(i);
        c_quest = p_lb->lab_type;
        if (*t_info == '?')
          (void) sprintf(descript, "%-16s%-20s=>  %s",
                 p_lb->name, p_lb->add_p, p_lb->text);
        else {
          if (p_lb->d_type)
            (void) sprintf(descript,
                "%3d%+4d  %-16s%-20s=> %s",
                p_lb->d_kind, p_lb->d_type, p_lb->name,
                p_lb->add_p, p_lb->text);
          else
            (void) sprintf(descript,
                "%3d%4d  %-16s%-20s=> %s",
                p_lb->d_kind, p_lb->d_type, p_lb->name,
                p_lb->add_p, p_lb->text);
        }

        return(c_quest);
        /* end all systems */
      }


/* conv_lab  ver 2010.01            # page 16     5 Jan 2010 15 35 */


      else {
        /* selected system */

        /* search the def_lab_file for p_sys (or w_sys) */ 
        i = srch_def(0, p_sys, w_sys, 0, 0, 0, p_lb);
        if (i < 0) return(i);

        if (p_lb->d_type)
           (void) sprintf(descript,
                  "%3d%+4d  %-16s%-20s=> %s",
                  p_lb->d_kind, p_lb->d_type, p_lb->name,
                  p_lb->add_p, p_lb->text);
        else
           (void) sprintf(descript,
                  "%3d%4d  %-16s%-20s=> %s",
                  p_lb->d_kind, p_lb->d_type, p_lb->name,
                  p_lb->add_p, p_lb->text);
        return(p_lb->lab_type);
      } /* info labels */
    } /* special */

    else if (*t_info == '&') {
      /* Separator info */
      if (strlen(p_sys) == 0) {
        /* all separators */
        if (s_quest) ++ spr;
        else            spr = sep;
        s_quest = spr->lab_type;
        (void) sprintf(descript,"%-10c=>  %-40s",
                       spr->sepch, spr->text);
        return(s_quest);
      } /* all */
      else {
        /* selected separator */
        /* NB:  "_"  for input of mean error cannot be found */
        ++ p_sys;
        for (spr = sep; spr->sepch; spr++)
          if (spr->sepch == *p_sys) {
            (void) sprintf(descript, "%s", spr->text);
            return(spr->lab_type);
          }
        return(-1);
      } /* special */
    } /* info separator */
  } /* end switch(convmode) */
  return(p_lb->lab_type);
}

int srch_def(
  int                 mode,
  char               *p_sys,
  char               *w_sys,
  int                 s_type,
  int                 s_cstm,
  int                 s_mode,
  struct lab_def_str *p_lb)
{
#include      "i_tabdir_file.h"

  static THREAD_SAFE size_t  pos = 0;
  char                       pth_mlb[512];
  char                       p_name[MLBLNG], *p_tp;
  int                        qr, res = 0, used, cha_str;
  int                        l_cstm, l_mode, l_type;
  union rgn_un               rgn_pref, dum_rgn;

extern THREAD_SAFE FILE     *def_lab_file;
extern THREAD_SAFE size_t    init_prj_pos;
  (void) strcpy(dum_rgn.prfx, "ZZ");

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

  if (mode != 3 || pos == 0) pos = init_prj_pos;
  (void) fseek(def_lab_file, (long) pos, SEEK_SET);
  /* prj definition file detected */

  // mode = 0 seach p_lb->s_sys or w_sys
  // mode = 1 seach s_type, s_cstm, s_mode
  // mode = 2 seach first
  // mode = 3 seach next

  if (mode == 0) { /* Replace digits by *'s in w_sys */
    for (qr = 0; *(p_sys+qr); qr++)
        *(w_sys+qr) = (C_DIGIT(*(p_sys+qr))) ? '?' : *(p_sys+qr);
    *(w_sys+qr) = '\0';
  }

  do {
    qr = fgetlhtx(def_lab_file, p_name);
    if (qr != EOF) {
      (void) fgets(pth_mlb, 510, def_lab_file);
      (void) sscanf(pth_mlb, "%d%n", &cha_str, &used);
      p_tp  = pth_mlb + used;
      (void) sscanf(p_tp, "%d%n", &l_type, &used);
      p_tp += used;
      (void) sscanf(p_tp, "%d%n", &l_cstm, &used);
      p_tp += used;
      (void) sscanf(p_tp, "%d%n", &l_mode, &used);

      qr = (mode == 0) ? (!strcmp((cha_str) ? w_sys : p_sys, p_name))
         : (mode == 1) ? 
              (s_type == l_type && s_cstm == l_cstm && s_mode == l_mode)
         : (mode >= 2);
      if (qr) {
        (void) strcpy(p_lb->name, (mode < 2) ? p_sys : p_name);
        p_lb->lab_type = (short) l_type;
        p_lb->d_kind   = (short) l_cstm;
        p_lb->d_type   = (short) l_mode;
        p_lb->cha_str  = (short) cha_str;

        p_tp += used;
        (void) sscanf(p_tp, "%hd%n", &p_lb->mask, &used);
        p_tp += used;

        (void) sscanf(p_tp, "%s%n", rgn_pref.prfx, &used);
        p_tp  += used;
        if (dum_rgn.r_nr[0] != rgn_pref.r_nr[0])
            (void) strcpy(p_lb->a_seq, rgn_pref.prfx);
        else *p_lb->a_seq = '\0';

        (void) sscanf(p_tp, "%s%n", rgn_pref.prfx, &used);
        p_tp  += used;
        p_lb->lab_rgn = (dum_rgn.r_nr[0] != rgn_pref.r_nr[0])
                      ? rgn_pref.r_nr[0] : 0;

        (void) sscanf(p_tp, "%s%n", p_name, &used);
        p_tp  += used;
        if (*p_name != '\"') {
          (void) strcpy(p_lb->pr_dtm, p_name);
          if (p_lb->sepch == '_') p_lb->sepch = 0;
        }
        else *p_lb->pr_dtm = '\0';
        /* datum has been read to p_lb->mlb2 (hopefully) */

        (void) sscanf(p_tp, "%d%n", &qr, &used);
        p_tp += used +2;
        p_lb->q_par = (char) qr;

        if (*p_tp != '\"') {
          used = (int) (strlen(p_tp) -1);
          if (*(p_tp + used) == '\n') *(p_tp + used) = '\0';
          (void) strcpy(p_lb->add_p, p_tp);
        }
        else *p_lb->add_p = '\0';

        (void) fgets(pth_mlb, 510, def_lab_file);
        (void) sscanf(pth_mlb, "%s%n", p_name, &used);
        if (p_lb->q_par) (void) strcpy(p_lb->par_str, pth_mlb +used +2);

        (void) fgets(p_lb->text, 512, def_lab_file);
        res = p_lb->lab_type;
      }
    } else {
      pos = init_prj_pos;
      return(-1);
    }
  } while (res <= 0 && strcmp(p_name, "stop"));
  pos = ftell(def_lab_file);

  return(res);
}
