

/* conv_obs  ver 2010.01            # page 1    7 jan 2010 15 35 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

/* conv_obs(name, p_lb, lab) Service function for conv_lab     */
/* NB: should be called only from conv_lab                     */
/*    result ==  0 : no lab                                    */
/*    result ==  9 : obs lab               in lab              */
/*    UNDEFSYS     : unrecognized name                         */
/* an unrecognized name is considered a filename containing a  */
/* genuine label                                               */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#include "geo_lab.h"

int                      conv_obs(
/*___________________________*/
char                    *lab_info,
struct lab_def_str      *p_lb,
struct obs_lab          *o_lab,
...
)

{

#include              "conv_lab.h"
#include              "conv_rgn.h"
#include              "f_nxt.h"
#include              "fputg.h"
#include              "labchsum.h"
#include              "set_dtm_1.h"
#include              "sgetg.h"
#include              "sputg.h"

  FILE                      *iofile = (FILE *) NULL;
  char                      *io_str = (char *) NULL;
  va_list                    o_p;

  char                       t_lab_a[2*MLBLNG], *t_lab = t_lab_a;
  char                      *p_sys, sepch, *p_tp, *p_dtm, in_str[256];
  double                     w;
  short                      p_no, mask;
  int                        conv_mode, i, used = 0;
  char                       rgn_name[24];
  union rgn_un               rgn_pref;
  struct dsh_str             d_sh;


/* conv_obs  ver 2010.01            # page 2    7 jan 2010 15 35 */


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

  static struct otype_str  gtype[] = {
    { 1, 1, "dir", "Directions",
      "1.5", "sx", "2.0", "cm", "", "", "", "" },

    { 2, 1, "lap", "Laplace az",
      "1.5", "sx", "2.0", "cm", "", "", "", "" },

    { 3, 2, "dst", "Distances",
      "3.0", "ppm", "2.0", "cm", "0.0", "ppm", "", "" },

    { 4, 2, "bmd", "Tape distances",
      "10.0", "ppm", "2.0", "cm", "0.0", "ppm", "", "" },

    {10, 3, "mix",  "Directions and distances",
      "1.5", "sx", "2.0", "cm", "0.0", "ppm", "3.0", "2.0" },

    {11, 1, "vt2",  "Vectors, 2 dim",
      "5.0", "ppm", "2.0", "cm", "", "", "", "" },

    {12, 1, "vt3",  "Vectors, 3 dim",
      "1.0", "ppm", "1.0", "cm", "", "", "", "" },

    { 0, 0, "", "", "", "", "", "", "", "", "", "" }
  };


/* conv_obs  ver 2010.01            # page 3    7 jan 2010 15 35 */


  static struct otype_str  ntype[] = {
    { 5, 1, "prs", "Precise levelling",
      "0.6", "ne", "0.01", "mm", "", "", "", "" },

    { 6, 1, "niv", "levelling",
      "1.0", "ne", "0.01", "mm", "", "", "", "" },

    { 7, 1, "zds", "Zenithdistancer",
      "3.0", "sx", "2.0", "cm", "", "", "", "" },

    { 8, 1, "mtz", "MTL levelling",
      "7.3", "ppm", "0.1", "cm", "", "", "", "" },

    { 9, 1, "pot", "Precise levelling in potential",
      "0.6", "ne", "0.01", "mm", "", "", "", "" },

    {10, 1, "msl", "Precise levelling in metric to MSL station",
      "0.6", "ne", "0.01", "mm", "", "", "", "" },

    {11, 1, "pmsl", "Precise levelling in potential to MSL station",
      "0.6", "ne", "0.01", "mm", "", "", "", "" },

    { 0, 0, "", "", "", "", "", "", "", "", "", "" }
  };

  static struct otype_str  ptype[] = {
    { 1, 0, "cl",  "Coord-block clockwise",
      "", "", "2.0", "cm", "", "", "", "" },

    { 2, 0, "acl",  "Coord-block anti-clockwise",
      "", "", "2.0", "cm", "", "", "", "" },

    { 3, 0, "w",    "Coord-block positive West",
      "", "", "2.0", "cm", "", "", "", "" },

    { 4, 0, "hn",   "Coord-block Normal heights",
      "", "", "50.0", "cm", "", "", "", "" },

    { 5, 0, "he",   "Coord-block Ellipsoid heights",
      "", "", "50.0", "cm", "", "", "", "" },

    { 6, 0, "hs",   "Coord-block Same heights",
      "", "", "10.0", "cm", "", "", "", "" },

    { 7, 0, "cs",   "Coord-block Same coordinates",
      "", "", "10.0", "cm", "", "", "", "" },

    { 9, 0, "",     "Coord-block GPS in <dtm>",
      "2.0", "cm", "", "", "", "", "", "" },

    { 0, 0, "", "", "", "", "", "", "", "", "", "" }
  };


/* conv_obs  ver 2010.01            # page 4    7 jan 2010 15 35 */


  static struct otype_str  mtype[] = {
    { 1, 0, "cl",  "Monocomparator clockwise",
      "", "", "0.020", "mm", "", "", "", "" },

    { 2, 0, "acl", "Monocomparator anti-clockwise",
      "", "", "0.020", "mm", "", "", "", "" },

    /* is changed to acl */
    { 3, 0, "rhs", "Monocomparator OLD gi format",
      "", "", "0.020", "mm", "", "", "", "" },

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

  if (p_lb->lab_type != OBS_LAB) {
    (void) fprintf(stdout,
         "\nconv_obs called with wrong lab_type %d", p_lb->lab_type);
    o_lab->lab_type = ILL_LAB;
    return(ILL_LAB);
  }

  switch(conv_mode) {
  case 0: /* char input */
  case 1: /* file input */

    p_sys = (p_lb->name);  /* name is mlb1 */
    p_dtm = (p_lb->mlb2);

    p_tp = (char *) o_lab; /* clean lab */
    for (i = sizeof (*o_lab) - 1; i >= 0; i--) *(p_tp + i) = 0;

    i               = 0;
    sepch           = (char) p_lb->sepch;
    o_lab->lab_type = p_lb->lab_type;
    o_lab->version  = LAB_VERSION;
    o_lab->obs_kind = p_lb->d_kind;
    o_lab->obs_type = p_lb->d_type;
    o_lab->region   = p_lb->region;
    o_lab->cmplt    = (short) (sepch == 'D');
    o_lab->m2c      = 0.0;
    o_lab->m2d      = 0.0;
    o_lab->spp      = 1.0;
    o_lab->m2cx     = 0.0;
    o_lab->m2dx     = 0.0;


/* conv_obs  ver 2010.01            # page 5    7 jan 2010 15 35 */


    /* PRODUCE THE MINI-LAB  from : mlb1, sepch, dtm */
    (void) sprintf(o_lab->mlb, "%s%c%s", p_sys, sepch, p_dtm);
    o_lab->sepix = (short) strlen(p_sys);

    if (!o_lab->cmplt && conv_mode == 0 && io_str == (char *)0) {
      o_lab->lab_type = ILL_LAB;
      return(ILL_LAB);
    }

    /* set params of the observation skind */
    switch(o_lab->obs_kind) {
    case 1: /* geom. geodesy */
    case 2: /* red. geom. g. */
      p_otype = gtype;
      break;

    case 3: /* levelling */
      p_otype = ntype;
      break;

    case 4: /* photo + coord-block */
      p_otype = ptype;
      break;

    case  5: /* monocomparator */
    case 55: /* mean monocomparator */
      p_otype = mtype;

      {
        union geo_lab   lab1;
        p_tp = (char *) &(o_lab->m2dx);
        if (sscanf(f_nxt(in_str, &i, iofile, used),
            "%s%n", t_lab, &used) == 1) {
          if (strcmp(t_lab, "xxx")) {
            if (conv_lab(t_lab, &lab1, "") != CRD_LAB) {
              (void) fprintf(stdout,
                  "\n***conv_obs: MONO Unknown coordsys %s", t_lab);
              return(STP_LAB);
            }
          }
          if (strlen(t_lab) > 23) {
            (void) fprintf(stdout,
                "\n***conv_obs: MONO too many chars in coordsys %s",
                t_lab);
              return(STP_LAB);
          }
          (void) strcpy(p_tp, t_lab);
        } else (void) strcpy(p_tp, "xxx");
      }

      break;


/* conv_obs  ver 2010.01            # page 6    7 jan 2010 15 35 */


    default: /* unknown kind */
      (void) fprintf(stdout,
          "\n***conv_obs : %s : Unknown observation kind %d",
          o_lab->mlb, o_lab->obs_kind);
      return(ILL_LAB);
    } /* end switch(o_lab->obs_kind) */

    for ( ; p_otype->obs_type; p_otype++) {
      if (!strcmp(p_dtm, p_otype->name)) {
        o_lab->obs_type = p_otype->obs_type;
        break;
      }
    }

    if (o_lab->obs_kind == 2 && /* vt2 and vt3 is gg type !! */
       (o_lab->obs_type == 11 || o_lab->obs_type == 12)) {
      o_lab->obs_kind  = 1;
      *o_lab->mlb      = 'g'; /* "gg" */
    }

    /*            cb_<dtm> ?  test dtm         */
    if (o_lab->obs_kind == 4 && !o_lab->obs_type) {
      o_lab->datum = (short) set_dtm_1(-1, p_dtm, &p_no, in_str,
                                 t_lab, rgn_pref.prfx, &mask, &d_sh);
      if (o_lab->datum > 0) {
        p_otype         = ptype;
        o_lab->obs_type = 9;
        for ( ; p_otype->obs_type; p_otype++) {
          if (o_lab->obs_type == p_otype->obs_type) break;
        }
      }
    }

    if (!o_lab->obs_type) {
      (void) fprintf(stdout,
          "\n***conv_obs unknown observation type %d",
          o_lab->obs_type);
      o_lab->lab_type = ILL_LAB;
      return(ILL_LAB);
    }


/* conv_obs  ver 2010.01            # page 7    7 jan 2010 15 35 */


    if (o_lab->cmplt) {
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


/* conv_obs  ver 2010.01            # page 8    7 jan 2010 15 35 */


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


/* conv_obs  ver 2010.01            # page 9    7 jan 2010 15 35 */


  case 3:  /* output of minilabel, simple */
  case 4:  /* output of minilabel, expand */
  case 5:  /* DOCUMENTATION OF THE LABEL  */

    if (conv_mode == 5)
       (void) fprintf(iofile, "\n\f     OBS-LABEL:\n # ");
    switch(o_lab->obs_kind) {
    case 1: /* gg */
    case 2: /* rg */
      p_otype = gtype;
      break;  /* end case 3, gg + rg*/

    case 3: /* levelling */
      p_otype = ntype;
      break;  /* end case 3, levelling */

    case 4: /* photo + coord-block */
      p_otype = ptype;
      break; /* photo + coord-block */

    case  5: /* monocomparator */
    case 55: /* monocomparator */
      p_otype = mtype;
      break; /* monocomparator */

    default: /* unknown kind */
      (void) fprintf(stdout,
          "\n***conv_obs : %s : Unknown observation kind %d",
          o_lab->mlb, o_lab->obs_kind);
      return(ILL_LAB);
    } /* end switch(o_lab->obs_kind) */

    if (conv_rgn(o_lab->region, rgn_pref.prfx, rgn_name) > 0)
      (void) fprintf(iofile, "%s_", rgn_pref.prfx);
    (void) fprintf(iofile, "%s", o_lab->mlb);

    if (o_lab->obs_kind == 5 /* monocomparator */ ||
        o_lab->obs_kind == 55) /* mean monocomparator */ {
      p_tp = (char *) &(o_lab->m2dx);
      (void) fprintf(iofile, "  %s", p_tp);
    }

    for ( ; p_otype->obs_type; p_otype++) {
      if (o_lab->obs_type == p_otype->obs_type) break;
    }


/* conv_obs  ver 2010.01            # page 10   7 jan 2010 15 35 */


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
    if (o_lab->obs_kind == 4 && o_lab->obs_type == 9) {
      p_dtm = (o_lab->mlb + o_lab->sepix);
      if (*p_dtm != '\0') ++ p_dtm;
      (void) fprintf(iofile, "         * crt_%s ;", p_dtm);
    } /* cb_<dtm> */
    (void) fprintf(iofile, "\n");

    if (conv_mode == 4) {

      if (!o_lab->obs_type) {
        (void) fprintf(stdout,
            "\n***conv_obs %s : unknown observation type %d",
            p_lb->text, o_lab->obs_type);
        return(ILL_LAB);
      }

      /* collect text of the coord kind */
      (void) fprintf(iofile,
                "\n\n*Observations        %s\n                     %s",
                p_lb->text, p_otype->text);

      /* Region info */
      if (o_lab->region != 0 &&
          (conv_rgn(o_lab->region, rgn_pref.prfx, rgn_name) > 0)) {
        (void) fprintf(iofile, "\nRegion:%-6s   %6s -> %-15s",
                       " ", rgn_pref.prfx, rgn_name);
      }


/* conv_obs  ver 2010.01            # page 11   7 jan 2010 15 35 */

      if (*(o_lab->mlb + o_lab->sepix) == 'D') {
        (void) fprintf(iofile, "\nDefault MFC :");
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
          (void) sputg(t_lab, o_lab->spp - 1.0, &o_lab->spp_tpd, " u");
          (void) fprintf(iofile, "  %s", t_lab);
        }
      }

      if (o_lab->obs_kind == 4) { /* coordinate block */
        if (o_lab->obs_type <= 3) { /* coordinates */
          (void) fprintf(iofile,
               "\n\nFirst block (before -1b): is fixing block");
          (void) fprintf(iofile,
               "\nLast block  (after -1b) : is free block");
          (void) fprintf(iofile,
               "\nFirst present : Helmert trans determ on first block");
          (void) fprintf(iofile,
               "\nFirst NOT present : Coordinates are kept fixed");
        }
      }
      (void) fprintf(iofile, "\n;\n");
    } /* end conv_mode == 4 */


/* conv_obs  ver 2010.01            # page 12   7 jan 2010 15 35 */


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
      if (o_lab->obs_kind != 5) {
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
      } else {
        p_tp = (char *) &(o_lab->m2dx);
        (void) fprintf(iofile, "\ncoordlab  = %s", p_tp);
      }
      (void) fprintf(iofile, "\n\n%s slut\n", o_lab->mlb);
    } /* end conv_mode == 5 */
    break /* end case 3 & 4 & 5, output label */;
  } /* end switch(conv_mode) */
  return(o_lab->lab_type);
}
