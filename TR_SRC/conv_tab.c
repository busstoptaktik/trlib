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
 


/* conv_tab    ver 2010.1          # page 1   12 jan 2010 10 19 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

/* conv_tab(name, p_lb, lab) converts the name of a lab to the lab*/
/*    result == GDE_LAB : geoid lab                            */
/*    result == DTM_LAB : Digital Terrain Model lab            */
/*    result == DHH_LAB : Diff Height Model lab                */
/*    result == UPL_LAB : land UPLift Model lab                */
/*    result == DEM_LAB : Digital Terrain Model lab            */
/*    result == BAG_LAB : Bouguer Anomaly Model lab            */
/*    result == TAB_LAB : UNDEFINED TABLE                      */
/*    result == T3D_LAB : 3d table                             */
/*    result == T2D_LAB : 2d table                             */
/*    result == TDD_LAB : 2d table doubles                     */
/*    result == -1      : file is not specified type or not found*/
/*    result == -2      : file is too short                    */
/*    result == -3      : file table inconsistent              */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <math.h>
#include "geo_lab.h"
#include "geoid_d.h"

#ifndef    M_PI
#include   "kms_math.h"
#endif

int                      conv_tab(
/*______________________________*/
char                    *lab_info,
struct lab_def_str      *p_lb,
struct gde_lab          *t_lab,
...
)

{


/* conv_tab    ver 2010.1          # page 2   12 jan 2010 10 19 */


#include              "conv_lab.h"
#include              "get_zone.h"
#include              "fputg.h"
#include              "geoid_t.h"
#include              "gettabdir.h"
#include              "i_tabdir_file.h"
#include              "labchsum.h"
#include              "set_dtm_1.h"
#include              "set_grs.h"
#include              "set_tpd.h"
#include              "sgetg.h"
#include              "sputg.h"
#include              "v_std.h"

  FILE                      *iofile = (FILE *) NULL;
  char                      *io_str = (char *) NULL;
  va_list                    o_p;

  double                    *BL = &(t_lab->B_min);

  /* Head of a grid_table in geocol format */
  int                        t_3d = 1;
  int                        lab_tp, f777; /* identifier of table  */
  int                        ezf[3];   /* ellipsoid, utm zone, fill  */
                                       /* or ellipsoid, -cstm, mode  */
  short                      p_no, mask;
  char                       pth_mlb[512], *p_tp, h_dtm[24];
  char                       d_name[24], e_name[24], p_name[24];
  double                     ell_p[10];
  struct dsh_str             trp_a;
  struct typ_dec             g_tpd, w_tpd;
  union rgn_un               rgn_pref;
  long                       pos;
  size_t                     qr;
  int                        r, used, conv_mode, all_out, r_size;
  FILE                      *tab_file;

  char                       t_lab_info[128];

  /* manager information :
    char           *name;       Minilab of the table
    int             cmplt;      0: own,  1: opt,  2: std.
    int             global;     0: regional, 1: global geoid
    int             recsz;      Bytes for a table value
    int             dec;        Decimals of a table value
    char           *datum;      Datum/projection
    char           *text;       External name of the table
  */

  /* save lab_info */
  (void) strcpy(t_lab_info, lab_info);


/* conv_tab    ver 2010.1          # page 3   12 jan 2010 10 19 */


  all_out   =  strcmp(t_lab_info,  "nnput");
  if (!all_out) *t_lab_info = 'i';
  conv_mode = (!strcmp(t_lab_info,  "input")) ? 1 :
              (!strcmp(t_lab_info, "in_u_#")) ? 2 :
              (!strcmp(t_lab_info, "output")) ? 3 :
              (!strcmp(t_lab_info, "OUTPUT")) ? 4 :
              (!strcmp(t_lab_info, "SURVEY")) ? 5 : 0;

  va_start(o_p, t_lab);
  if (conv_mode >= 1 && conv_mode <= 5) iofile = va_arg(o_p, FILE *);
  else
  if (conv_mode == 0) {
    io_str = va_arg(o_p, char *);
    if((char *) io_str == (char *) lab_info) io_str = NULL;
  }
  va_end(o_p);

  switch (conv_mode) {
  case 0: /* char input */
  case 1: /* file input */
  case 2: /* file input */
    /* The grid label is created from the binary grid file head */

    /* free any open file pointer */
    if ((t_lab->fd != NULL)    && (t_lab->lab_type == 777 ||
         t_lab->lab_type == 776 || t_lab->lab_type == 775 ||
         t_lab->lab_type == 774 || t_lab->lab_type == 773 ||
         t_lab->lab_type == 772 || t_lab->lab_type == 771 ||
         t_lab->lab_type == 770 || t_lab->lab_type == 769 ||
         t_lab->lab_type == 768)) {
      fclose(t_lab->fd);
      t_lab->fd = NULL;
    }

    /* clear the gde_lab structure */
    p_tp = (char *) t_lab;
    for (r = sizeof (*t_lab) - 1; r >= 0; r--) *(p_tp + r) = 0;
    t_lab->lab_type = ILL_LAB;

    (void) strcpy(t_lab->mlb, p_lb->name);

    tab_file = i_tabdir_file(4, t_lab->mlb, &r, pth_mlb);
    if (r) {
      if (all_out)
         (void) fprintf(stdout, 
                        "\n*** table: %s NOT FOUND;\n", t_lab->mlb);
      return (-1);
    }

    /* Test for identification of a binary grid file */
    pos = 0;
    if ((qr = fread((void *) &f777, sizeof (int),
                    1, tab_file)) - 1) {
      (void) fprintf(stdout,
                "\n*** conv_tab: FEJL ved input af table-mark");
      (void) fprintf(stdout,
                "\n blokaddr %8ld antal  = %4ld size = %4ld ;",
                pos, qr, sizeof (int));
      fclose(tab_file);
      return (-1);
    }
    if (f777 < 768 || 777 < f777) {
      if (all_out) (void) fprintf(stdout,
         "\n*** conv_tab: %s not a table_file ;\n", t_lab->mlb);
      fclose(tab_file);
      return (-2);
    }
    /* table file detected */


/* conv_tab    ver 2010.1          # page 4   12 jan 2010 10 19 */


    /* Input table area limits */
    pos = pos + (long) qr;
    if ((qr = fread((void *) BL, 6 * sizeof (double),
                    1, tab_file)) - 1) {
      (void) fprintf(stdout,
                "\n*** conv_tab: FEJL ved input af table-limits");
      (void) fprintf(stdout,
                "\n blokaddr %8ld antal  = %4ld size = %4ld ;\n",
                pos, qr, 6 * sizeof (double));
      fclose(tab_file);
      return (-2);
    }

    /* Input additional params (ellipsoid, "zone", fill) */
    pos = pos + (long) qr;
    if ((qr = fread((void *) ezf, sizeof (ezf),
                    1, tab_file)) - 1) {
      (void) fprintf(stdout,
                "\n*** conv_tab: FEJL ved input af table-params");
      (void) fprintf(stdout,
                "\n blokaddr %8ld antal  = %4ld size = %4ld ;\n",
                pos, qr, sizeof (ezf));
      fclose(tab_file);
      return (-2);
    }

    /* ezf[1] == 0  => geographical coord grid */
    if (ezf[1] != 0 && f777 == 777) {
      (void) fprintf(stdout,
                "\n*** conv_tab: %s not geogr. grid ;\n", pth_mlb);
      fclose(tab_file);
      return (ILL_LAB);
    }

    t_lab->global = 0;
    if (ezf[1] == 0) {
      /* round the area limits to nearest integer sx */
      g_tpd = *set_tpd("sx", 10, 0);
      for (r = 0; r < 6; r++) {
        (void) sputg(t_lab_info, *(BL + r) * M_PI / 180.0,
                                 &g_tpd, "+u!");
        *(BL + r) = sgetg(t_lab_info, &w_tpd, &used, "");
      }

      /* area limits */
      *(BL + 2) = v_std(*(BL + 2));
      *(BL + 3) = v_std(*(BL + 3) - *(BL + 2));

      if (*(BL + 3) < 1e-10 ||
          *(BL + 3) > 2.0 * M_PI - 1e-10) {
        t_lab->global = 1;
        *(BL + 2)     = 0.0;
        *(BL + 3)     = 2.0 * M_PI;
      }


/* conv_tab    ver 2010.1          # page 5   12 jan 2010 10 19 */


      /* Test for fortran identification of a binary grid file */
      t_lab->f_comp   = 1;
      t_lab->g_tpd    = *set_tpd("dg", 10, 6);
    }
    else {
      t_lab->f_comp  = 2;
      r              = strncmp("grsoek", t_lab->mlb, 6) ? 0 : 6;
      t_lab->g_tpd   = *set_tpd("m", 10, r);
      *(BL + 3)     -= *(BL + 2);
    }

    /* generate the table_label */

    /* insert items in the table_lab */
    lab_tp          = (f777 == 777) ? GDE_LAB :
                      (f777 == 776) ? DTM_LAB :
                      (f777 == 775) ? DHH_LAB :
                      (f777 == 774) ? UPL_LAB :
                      (f777 == 773) ? DEM_LAB :
                      (f777 == 772) ? TAB_LAB :
                      (f777 == 771) ? BAG_LAB :
                      (f777 == 770) ? T3D_LAB :
                      (f777 == 769) ? T2D_LAB :
                      (f777 == 768) ? TDD_LAB : ILL_LAB;
    t_lab->version  = LAB_VERSION;
    t_lab->cmplt    = 0;
    t_lab->sepix    = (short) strlen(t_lab->mlb);
    /* 0 : geo, > 0 : utm zone, < 0 : -cstm */
    t_lab->cstm     = (short) ezf[1];
    /* 0 : geo or utm,          > 0 : mode */
    t_lab->mode     = (short) ezf[2];

    r = geoid_t(t_lab, d_name, h_dtm, pth_mlb);
    if (r) {
      t_lab->lab_type = ILL_LAB;
      fclose(tab_file);
      return(r);
    } 

    if (t_lab->lab_type != lab_tp || t_lab->f777 != f777) {
      if (all_out) (void) fprintf(stdout,
         "\n*** conv_tab: label inconsistent to table_file %s;\n",
         pth_mlb);
      fclose(tab_file);
      return (-2);
    }


/* conv_tab    ver 2010.1          # page 6   12 jan 2010 10 19 */


    /* datum, ellipsoid, proj */
    {
      union geo_lab   lab1;
      if (conv_lab(d_name, &lab1, "  20000101") == CRD_LAB) {
        if (t_lab->cstm == 0) /* geo */
          r = lab1.u_c_lab.cstm != 2 ||
              t_lab->mode != lab1.u_c_lab.mode;
        else
        if (t_lab->cstm > 0) /* utm */
          r = lab1.u_c_lab.cstm != 3 || t_lab->cstm != get_zone(&lab1);
        else
          r = lab1.u_c_lab.cstm != -t_lab->cstm ||
              t_lab->mode != lab1.u_c_lab.mode;
      }
      else r = 1;
      if (r) {
        (void) fprintf(stdout,
               "\n***conv_tab: Unknown coordsys %s", d_name);
        fclose(tab_file);
        return(ILL_LAB);
      }
      (void) strcpy(t_lab->clb, d_name);
      t_lab->datum     = (lab1.u_c_lab).datum;
      t_lab->ellipsoid = (lab1.u_c_lab).ellipsoid;
      (void) set_dtm_1(-1, h_dtm, &t_lab->p_dtm, p_name, e_name,
                       rgn_pref.prfx, &mask, &trp_a);
    }

    t_lab->fd = tab_file; // NOW SET THE FILE POINTER

    r_size = abs(t_lab->rec_size);
    t_3d   = (t_lab->lab_type == T3D_LAB) ? 3
           : (t_lab->lab_type == T2D_LAB) ? 2
           : (t_lab->lab_type == TDD_LAB) ? 2 : 1;

    /* table indices (NB: when changed remember also in unigeoid.c) */
    t_lab->n_max    = (int) ((*(BL + 1) - *(BL + 0)) / *(BL + 4) + 1.1);
    t_lab->e_max    = (int) ((*(BL + 3)            ) / *(BL + 5) + 1.1);
    t_lab->nbase    = (GEOIDHEADER) * 4L * (GEOIDPGSIZE);
    t_lab->blk_size = ((t_3d == 1) ? GEOIDPGSIZE :
                      ((t_3d == 2 && r_size == 8) ? TABDDPGSIZE :
                      ((t_3d == 2) ? TAB3DPGSIZE * 2 / 3
                                   : TAB3DPGSIZE))) * r_size;
    t_lab->rec_p_bl = ((int) t_lab->blk_size) / r_size / t_3d;

    t_lab->estop    = ((t_lab->e_max + t_lab->rec_p_bl -1)
                              / t_lab->rec_p_bl) * t_lab->rec_p_bl;
    t_lab->row_size = ((size_t) (t_lab->estop / t_lab->rec_p_bl))
                                              * t_lab->blk_size;

    /* set checksum */
    t_lab->ch_sum = labchsum((union geo_lab *) t_lab, &(t_lab->ch_sum));

    /* zero counter of calls */
    t_lab->used = 0;

    break; /* end of case 0 and 1: create a gde_lab */


/* conv_tab    ver 2010.1          # page 7   12 jan 2010 10 19 */


  case 3:  /* output of minilabel, simple */
  case 4:  /* output of minilabel, expand */
  case 5:  /* DOCUMENTATION OF THE LABEL  */

    /* check the gde_lab */
    switch (t_lab->lab_type) {
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
      if (t_lab->ch_sum != labchsum(
                         (union geo_lab *) t_lab, &t_lab->ch_sum)) {
        (void) fprintf(stdout, "\n***conv_tab: file %s of type %d",
                       t_lab->mlb, t_lab->lab_type);
        (void) fprintf(stdout, "\n             has sum fault;");
        return (ILL_LAB);
      }
      break;
    default: 
      (void) fprintf(stdout, "\n***conv_tab: file %s of type %d",
                     t_lab->mlb, t_lab->lab_type);
      (void) fprintf(stdout, "\n             is not a table;");
    }

    (void) fprintf(iofile, "\n %s\n ", t_lab->mlb);

    if (conv_mode >= 4 /* full output */ ) {
      r_size = abs(t_lab->rec_size);
      if (t_lab->local) 
         (void) fprintf(iofile, "\n * FULL NAME: %s\n ", t_lab->mlb);
      else {
        p_tp = gettabdir();
#if defined(sparc) || defined(__sparc)
        if(*p_tp == '\0') p_tp = settabdir(NULL);
#endif
      }
      (void) fprintf(iofile, "\n * FULL NAME: %s%s\n ",
                     p_tp, t_lab->mlb);


/* conv_tab    ver 2010.1          # page 8   12 jan 2010 10 19 */

      (void) fprintf(iofile, "\nlng       = %15ld", sizeof (*t_lab));
      (void) fprintf(iofile, "\nlab_type  = %15d", t_lab->lab_type);
      if (geoid_t(t_lab, d_name, h_dtm, pth_mlb) == 0) {
        (void) fprintf(iofile, "\n DESCRIPTION: %s", pth_mlb);
        (void) fprintf(iofile, "\n\n  %s Table\n",
                (t_lab->cmplt == 2) ? "Standard" :
                (t_lab->cmplt == 1) ? "Optional" : "External");
        (void) fprintf(iofile, "\n  ellipsoid  = %15d",
                       t_lab->ellipsoid);
        if (t_lab->ellipsoid) {
          if (set_grs(t_lab->ellipsoid, e_name, ell_p) > 0)
              (void) fprintf(iofile, "%9s%s", " ", e_name);
        }
        (void) fprintf(iofile, "\n  datum      = %15d", t_lab->datum);
        if (t_lab->ellipsoid) {
          if (set_dtm_1(t_lab->datum, d_name, &p_no, p_name,
                        e_name, rgn_pref.prfx, &mask,  &trp_a) > 0)
            (void) fprintf(iofile, "%9s%s", " ", d_name);
        }
        (void) fprintf(iofile, "\n  H datum    = %15d", t_lab->p_dtm);
        if (t_lab->p_dtm > 200) {
          if (set_dtm_1(t_lab->p_dtm, d_name, &p_no, p_name,
                        e_name, rgn_pref.prfx, &mask, &trp_a) > 0)
            (void) fprintf(iofile, "%9s%s", " ", d_name);
        }

        g_tpd = t_lab->g_tpd;

        (void) fprintf(iofile, "\n  projection =      %2d      %2d",
               t_lab->cstm, t_lab->mode);
        (void) fprintf(iofile, "        %-12s", t_lab->clb);
        (void) fprintf(iofile, "\n  LAT ");
        (void) fputg(iofile, t_lab->B_min, &g_tpd, " +u");
        (void) fputg(iofile, t_lab->B_max, &g_tpd, " +u");
        (void) fprintf(iofile, "\n  LNG ");
        (void) fputg(iofile, t_lab->L_min, &g_tpd, " +u");
        (void) fputg(iofile,
                     t_lab->L_min + t_lab->deltaL, &g_tpd, " +u");
        (void) fprintf(iofile, "\n  STEP");
        (void) fputg(iofile, t_lab->dB, &g_tpd, " +u");
        (void) fputg(iofile, t_lab->dL, &g_tpd, " +u");
        (void) fprintf(iofile, "\n");

        (void) fprintf(iofile, "\n  %-12s %7d", "n_max", t_lab->n_max);
        (void) fprintf(iofile, "     %-12s %7d", "e_max", t_lab->e_max);

        (void) fprintf(iofile, "\n  %-12s %7d", "nbase", t_lab->nbase);
        (void) fprintf(iofile, "     %-12s %7d", "estop", t_lab->estop);

        (void) fprintf(iofile,
                       "\n  %-12s %7ld", "row_size", t_lab->row_size);
        (void) fprintf(iofile,
                       "     %-12s %7ld", "blk_size", t_lab->blk_size);

        (void) fprintf(iofile,
                       "\n  %-12s %7d", "rec_size", t_lab->rec_size);
        (void) fprintf(iofile,
                       "     %-12s %7d", "rec_p_bl", t_lab->rec_p_bl);


/* conv_tab    ver 2010.1          # page  9  12 jan 2010 10 19 */


        (void) fprintf(iofile,
                       "\n  %-12s %7s", "rec_type",
                       (abs(t_lab->rec_size) == 8) ? "double" :
                       ((t_lab->rec_size > 0) ? "float" : "int"));
        (void) fprintf(iofile, "     %-15s %4s", "fortran comptb",
                       (t_lab->f_comp == 1) ? "yes" : "no");

        (void) fprintf(iofile, "\n  %-12s %7d", "file size",
              t_lab->n_max * t_lab->estop * r_size + t_lab->nbase);
        (void) fprintf(iofile,
                       "     %-12s %7d", "head size", t_lab->nbase);

        (void) fprintf(iofile, "\n  %-12s %7d", "table_bytes",
                       t_lab->n_max * t_lab->e_max * r_size);
        (void) fprintf(iofile, "     %-12s %7d", "table_fill",
              t_lab->n_max * (t_lab->estop - t_lab->e_max) * r_size);

        (void) fprintf(iofile,
                       "\n  %-12s %7d", "decimals", t_lab->gd_dec);
        (void) fprintf(iofile, "\n;\n");
      }

    } /* 4 & 5 */

    break; /* end case 3, 4 & 5, table info */

  } /* conv_mode */

  return(t_lab->lab_type);
}
