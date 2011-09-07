

/* conv_cnd  ver 2010.01            # page 1    7 jan 2010 15 35 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

/* conv_cnd(name, p_lb, lab) Service function for conv_lab     */
/* NB: should be called only from conv_lab                     */
/*    result ==  0 : no lab                                    */
/*    result ==  9 : obs lab               in lab              */
/*    result == 16 : cond lab              in lab              */
/*    UNDEFSYS     : unrecognized name                         */
/* an unrecognized name is considered a filename containing a  */
/* genuine label                                               */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include "geo_lab.h"
#include "trthread.h"

int                      conv_cnd(
/*___________________________*/
char                    *lab_info,
struct lab_def_str      *p_lb,
struct obs_lab          *o_lab,
...
)

{

#include              "conv_rgn.h"
#include              "f_nxt.h"
#include              "fputg.h"
#include              "labchsum.h"
#include              "sgetg.h"
#include              "sputg.h"

  FILE                      *iofile = (FILE *) NULL;
  char                      *io_str = (char *) NULL;
  va_list                    o_p;

  char                       t_lab_a[2*MLBLNG], *t_lab = t_lab_a;
  char                      *p_sys, sepch, *p_tp, *p_dtm, in_str[256];
  double                     w;
  int                        conv_mode, i, used = 0;
  char                       rgn_name[24];
  union rgn_un               rgn_pref;


/* conv_cnd  ver 2010.01            # page 2    7 jan 2010 15 35 */


  struct otype_str {
    short      obs_type;
    short      mc_type;
    char       *name;
    char       *text;
    char       *md_tx;
    char       *md_udt;
    char       *mc_tx;
    char       *mc_udt;
    char       *spp_tx;
    char       *spp_udt;
    char       *mdx_tx;
    char       *mcx_tx;
  };

  struct otype_str   *p_otype;

  static THREAD_SAFE  struct otype_str  ctype[] = {
    { 1, 0, "ngrp",  "Nivellementsgruppe",
         "", "", "", "", "", "", "", "" },
    { 0, 0, "", "", "", "", "", "", "", "", "", "" }
  };

  /* save lab_info */
  (void) strcpy(t_lab, lab_info);

  conv_mode = (!strcmp(t_lab,  "input")) ? 1 :
              (!strcmp(t_lab, "output")) ? 3 :
              (!strcmp(t_lab, "OUTPUT")) ? 4 :
              (!strcmp(t_lab, "SURVEY")) ? 5 : 0;
  va_start(o_p, o_lab);
  if (conv_mode) iofile = va_arg(o_p, FILE *);
  else           io_str = va_arg(o_p, char *);
  va_end(o_p);

  if (p_lb->lab_type != CND_LAB) {
    (void) fprintf(stdout,
         "\nconv_cnd called with wrong lab_type %d", p_lb->lab_type);
    o_lab->lab_type = ILL_LAB;
    return(ILL_LAB);
  }

  switch(conv_mode) {
  case 0: /* char input */
  case 1: /* file input */

    p_sys    = (p_lb->name);  /* name is mlb1 */
    p_dtm    = (p_lb->mlb2);

    p_tp = (char *) o_lab; /* clean lab */
    for (i = sizeof (*o_lab) - 1; i >= 0; i--) *(p_tp + i) = 0;


/* conv_cnd  ver 2010.01            # page 3    7 jan 2010 15 35 */

    i                = 0;
    o_lab->lab_type  = p_lb->lab_type;
    o_lab->version   = LAB_VERSION;
    o_lab->obs_kind  = p_lb->d_kind;
    o_lab->obs_type  = p_lb->d_type;
    o_lab->region    = p_lb->region;
    o_lab->cmplt     = 1;
    o_lab->m2c       = 0.0;
    o_lab->m2d       = 0.0;
    o_lab->spp       = 1.0;
    o_lab->m2cx      = 0.0;
    o_lab->m2dx      = 0.0;
    o_lab->mc_tpd.gf = (unsigned char) 0;
    o_lab->mc_tpd.tf = (unsigned char) 0;
    o_lab->mc_tpd.ff = (unsigned char) 0;
    o_lab->mc_tpd.df = (unsigned char) 0;
    o_lab->md_tpd    = o_lab->mc_tpd;

    /* PRODUCE THE MINI-LAB  from : mlb1, sepch, dtm */
    (void) sprintf(o_lab->mlb, "%sD%s", p_sys, p_dtm);
    o_lab->sepix = (short) strlen(p_sys); /* D is default */

    if (!o_lab->cmplt && conv_mode == 0 && io_str == (char *)0) {
      o_lab->lab_type = ILL_LAB;
      return(ILL_LAB);
    }

    /* set params of the observation skind */
    switch(o_lab->obs_kind) {
    case  1: /* condition */
      p_otype          = ctype;
      break;

    default: /* unknown kind */
      (void) fprintf(stdout,
          "\n***conv_cnd : %s : Unknown observation kind %d",
          o_lab->mlb, o_lab->obs_kind);
      return(ILL_LAB);
    } /* end switch(o_lab->obs_kind) */

    for ( ; p_otype->obs_type; p_otype++) {
      if (!strcmp(p_dtm, p_otype->name)) {
        o_lab->obs_type = p_otype->obs_type;
        break;
      }
    }

    if (!o_lab->obs_type) {
      (void) fprintf(stdout,
          "\n***conv_cnd unknown observation type %d",
          o_lab->obs_type);
      o_lab->lab_type = ILL_LAB;
      return(ILL_LAB);
    }


/* conv_cnd  ver 2010.01            # page 4    7 jan 2010 15 35 */


    if (o_lab->cmplt) {
      if (o_lab->obs_kind != 1) {
        if (p_otype->mc_type) {
          w          = sgetg(p_otype->md_tx, &o_lab->md_tpd, &used,
                             p_otype->md_udt);
          o_lab->m2d = w*w;
        }
        w          = sgetg(p_otype->mc_tx, &o_lab->mc_tpd, &used,
                           p_otype->mc_udt);
        o_lab->m2c = w*w;
        if (p_otype->mc_type == 3) {
          w           = sgetg(p_otype->mdx_tx, &o_lab->mdx_tpd, &used,
                              p_otype->spp_udt);
          o_lab->m2dx = w*w;
          w           = sgetg(p_otype->mcx_tx, &o_lab->mcx_tpd, &used,
                              p_otype->mc_udt);
          o_lab->m2cx = w*w;
        }
        if (p_otype->mc_type >= 2) {
          w          = sgetg(p_otype->spp_tx, &o_lab->spp_tpd, &used,
                             p_otype->spp_udt);
          o_lab->spp = 1.0 + w;
        }
      }
    } else
    if (conv_mode) {
      if (p_otype->mc_type) {
        w          = sgetg(f_nxt(in_str, &i, iofile, used),
                         &(o_lab->md_tpd), &used, p_otype->md_udt);
        o_lab->m2d = w*w;
      }
      w          = sgetg(f_nxt(in_str, &i, iofile, used),
                       &(o_lab->mc_tpd), &used, p_otype->mc_udt);
      o_lab->m2c = w*w;
      if (p_otype->mc_type == 3) {
        w           = sgetg(f_nxt(in_str, &i, iofile, used),
                          &(o_lab->mdx_tpd), &used, p_otype->spp_udt);
        o_lab->m2dx = w*w;
        w           = sgetg(f_nxt(in_str, &i, iofile, used),
                          &(o_lab->mcx_tpd), &used, p_otype->mc_udt);
        o_lab->m2cx = w*w;
      }
      /*   dst and spp */
      if (p_otype->mc_type >= 2 && i) {
        w          = sgetg(f_nxt(in_str, &i, iofile, used),
                          &(o_lab->spp_tpd), &used, p_otype->spp_udt);
        if (o_lab->spp_tpd.gf == 12 && o_lab->spp_tpd.tf == 1)
            o_lab->spp = 1.0 + w;
      }
    }


/* conv_cnd  ver 2010.01            # page 5    7 jan 2010 15 35 */


    else {
      if (p_otype->mc_type) {
        w          = sgetg(io_str, &(o_lab->md_tpd), &used,
                           p_otype->md_udt);
        o_lab->m2d = w*w;
      }
      io_str    += used;
      w          = sgetg(io_str, &(o_lab->mc_tpd), &used,
                         p_otype->mc_udt);
      o_lab->m2c = w*w;
      if (p_otype->mc_type == 3) {
        io_str     += used;
        w           = sgetg(io_str, &(o_lab->mdx_tpd), &used,
                            p_otype->spp_udt);
        o_lab->m2dx = w*w;
        io_str     += used;
        w           = sgetg(io_str, &(o_lab->mcx_tpd), &used,
                           p_otype->mc_udt);
        o_lab->m2cx = w*w;
      }
      /*   dst and spp */
      if (p_otype->mc_type >= 2 && strlen(io_str) >= 4) {
        w          = sgetg(io_str, &(o_lab->spp_tpd), &used,
                          p_otype->spp_udt);
        if (o_lab->spp_tpd.gf == 12 && o_lab->spp_tpd.tf == 1)
            o_lab->spp = 1.0 + w;
      }
    }

    o_lab->ch_sum   = labchsum((union geo_lab *) o_lab, &o_lab->ch_sum);

    break; /* end conv_mode 0 & 1 */


/* conv_cnd  ver 2010.01            # page 6    7 jan 2010 15 35 */


  case 3:  /* output of minilabel, simple */
  case 4:  /* output of minilabel, expand */
  case 5:  /* DOCUMENTATION OF THE LABEL  */

    if (conv_mode == 5)
       (void) fprintf(iofile, "\n\f      COND-LABEL:\n # ");

    switch(o_lab->obs_kind) {
    case  1: /* condition */
      p_otype = ctype;
      break;

    default: /* unknown kind */
      (void) fprintf(stdout,
          "\n***conv_cnd : %s : Unknown observation kind %d",
          o_lab->mlb, o_lab->obs_kind);
      return(ILL_LAB);
    } /* end switch(o_lab->obs_kind) */

    if (conv_rgn(o_lab->region, rgn_pref.prfx, rgn_name) > 0)
      (void) fprintf(iofile, "%s_", rgn_pref.prfx);
    (void) fprintf(iofile, "%s", o_lab->mlb);

    for ( ; p_otype->obs_type; p_otype++) {
      if (o_lab->obs_type == p_otype->obs_type) break;
    }

    if (*(o_lab->mlb + o_lab->sepix) == '_') { /* print mean errors */
      if (p_otype->mc_type) {
        (void) fprintf(iofile, "  ");
        (void) fputg(iofile, sqrt(o_lab->m2d), &o_lab->md_tpd, " u");
      }
      (void) fprintf(iofile, "  ");
      (void) fputg(iofile, sqrt(o_lab->m2c), &o_lab->mc_tpd, " u");
      if (p_otype->mc_type == 3) {
        (void) fprintf(iofile, "  ");
        (void) fputg(iofile,
            sqrt(o_lab->m2dx), &o_lab->mdx_tpd, " u");
        (void) fprintf(iofile, "  ");
        (void) fputg(iofile,
            sqrt(o_lab->m2cx), &o_lab->mcx_tpd, " u");
      }
      if (p_otype->mc_type >= 2 &&
          o_lab->spp != 1.0 && o_lab->spp_tpd.gf) {
        (void) fprintf(iofile, "  ");
        (void) fputg(iofile, o_lab->spp -1.0, &o_lab->spp_tpd, " u");
      }

    } /* end sepch = '_' */
    (void) fprintf(iofile, "\n");


/* conv_cnd  ver 2010.01            # page 7    7 jan 2010 15 35 */


    if (conv_mode == 4) {

      if (!o_lab->obs_type) {
        (void) fprintf(stdout,
            "\n***conv_cnd %s : unknown observation type %d",
            p_lb->text, o_lab->obs_type);
        return(ILL_LAB);
      }

      /* collect text of the obs kind */
      (void) fprintf(iofile,
                "\n\n*Conditions          %s\n                     %s",
                p_lb->text, p_otype->text);

      /* Region info */
      if (o_lab->region != 0 &&
          (conv_rgn(o_lab->region, rgn_pref.prfx, rgn_name) > 0)) {
        (void) fprintf(iofile, "\nRegion:%-6s   %6s -> %-15s",
                       " ", rgn_pref.prfx, rgn_name);
      }

      if (*(o_lab->mlb + o_lab->sepix) == 'D') {
        (void) fprintf(iofile, "\nDefault MFC : all ZERO");
        if (*(o_lab->mlb + o_lab->sepix) != 'D') {
          if (p_otype->mc_type) {
            (void) sputg(t_lab, sqrt(o_lab->m2d), &o_lab->md_tpd, " u");
            (void) fprintf(iofile, "  %s", t_lab);
          }
          (void) sputg(t_lab, sqrt(o_lab->m2c), &o_lab->mc_tpd, " u");
          (void) fprintf(iofile, "  %s", t_lab);
          if (p_otype->mc_type == 3) {
            (void) sputg(t_lab, sqrt(o_lab->m2dx),
                         &o_lab->mdx_tpd, " u");
            (void) fprintf(iofile, "  %s", t_lab);
            (void) sputg(t_lab, sqrt(o_lab->m2cx),
                         &o_lab->mcx_tpd, " u");
            (void) fprintf(iofile, "  %s", t_lab);
          }
          if (p_otype->mc_type >= 2 &&
              o_lab->spp != 1.0 && o_lab->spp_tpd.gf) {
            (void) sputg(t_lab,
                         o_lab->spp - 1.0, &o_lab->spp_tpd, " u");
            (void) fprintf(iofile, "  %s", t_lab);
          }
        }
      }

      (void) fprintf(iofile, "\n;\n");
    } /* end conv_mode == 4 */


/* conv_cnd  ver 2010.01            # page 8    7 jan 2010 15 35 */


    if (conv_mode == 5) { /* SURVEY : DOCUMENTATION OF THE LABEL */
      sepch = *(o_lab->mlb + o_lab->sepix);
      if (sepch == '\0') sepch = ' ';
      /* General info */
      (void) fprintf(iofile, "\nlng       = %15ld", sizeof (*o_lab));
      (void) fprintf(iofile, "\nlab_type  = %15d", o_lab->lab_type);
      (void) fprintf(iofile,
                     "         version   = %15d", o_lab->version);
      (void) fprintf(iofile, "\nmlb       = %15s", o_lab->mlb);
      (void) fprintf(iofile, "         sepch     = %15c", sepch);
      (void) fprintf(iofile, "\nkind      = %15d", o_lab->obs_kind);
      (void) fprintf(iofile, "         compl     = %15d", o_lab->cmplt);
      (void) fprintf(iofile, "\ntype      = %15d", o_lab->obs_type);
      (void) fprintf(iofile, "         datum     = %15d", o_lab->datum);
      /* Region info */
      (void) fprintf(iofile, "\nregion    = %15d", o_lab->region);
      if (o_lab->region != 0) {
        if (conv_rgn(o_lab->region, rgn_pref.prfx, rgn_name) > 0)
           (void) fprintf(iofile, "%9s%-6s    =     %-15s",
                          " ", rgn_pref.prfx, rgn_name);
      }
      if (o_lab->md_tpd.gf != 0) 
        (void) sputg(t_lab, sqrt(o_lab->m2d), &o_lab->md_tpd, " u");
      else *t_lab = '\0';
      (void) fprintf(iofile, "\nm2d, md   = %15e : %15s",
                     o_lab->m2d, t_lab);
      if (o_lab->mc_tpd.gf != 0) 
        (void) sputg(t_lab, sqrt(o_lab->m2c), &o_lab->mc_tpd, " u");
      else *t_lab = '\0';
      (void) fprintf(iofile, "\nm2c, mc   = %15e : %15s",
                     o_lab->m2c, t_lab);
      if (o_lab->spp_tpd.gf != 0 && o_lab->spp != 1.0) 
        (void) sputg(t_lab, o_lab->spp - 1.0, &o_lab->spp_tpd, " u");
      else *t_lab = '\0';
      (void) fprintf(iofile, "\nspp2, spp = %15e : %15s",
                     o_lab->spp, t_lab);
      if (o_lab->mdx_tpd.gf != 0) 
        (void) sputg(t_lab, sqrt(o_lab->m2dx), &o_lab->mdx_tpd, " u");
      else *t_lab = '\0';
      (void) fprintf(iofile, "\nm2dx, mdx = %15e : %15s",
                     o_lab->m2dx, t_lab);
      if (o_lab->mcx_tpd.gf != 0) 
        (void) sputg(t_lab, sqrt(o_lab->m2cx), &o_lab->mcx_tpd, " u");
      else *t_lab = '\0';
      (void) fprintf(iofile, "\nm2cx, mcx = %15e : %15s",
                     o_lab->m2cx, t_lab);
      (void) fprintf(iofile, "\n\n%s slut\n", o_lab->mlb);
    } /* end conv_mode == 5 */
    break /* end case 3 & 4 & 5, output label */;
  } /* end switch(conv_mode) */
  return(o_lab->lab_type);
}
