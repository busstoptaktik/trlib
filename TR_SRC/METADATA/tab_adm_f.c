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
 
#include    <stdio.h>
#include    <stdlib.h>
#include    <limits.h>
#include    <string.h>
#include    <fcntl.h>
#include    <math.h>
#include    <sys/types.h>
#include    <sys/stat.h>
#include    "KmsFncs.h"
#include    "trthread.h"

/* tab_adm_f is a maintenance tool for tables and geoids      */
/*                                                           */
/* the label must be found in manager.tab                    */
/*           holding the limits and steps                    */
/*           which are checked against the file              */
/*                                                           */
/* CALL  ::                                                  */
/* (int) tab_adm_f(                                          */
/*   char *tab_name,  name of table (or empty at mode 'b')   */
/*   char  mode,      mode = 'b' / 't' / 'l' / 'z'           */
/*   int   header,    = 0/1    recommended == 1              */
/*   int   hd_limits, = 1/2/3  recommended == 1/2/3          */
/*           NOT LEAGAL    0   no header in file             */
/*                    = 1      header in file                */
/*                    = 2      h + NO ROUNDING of limits     */
/*   char *global_dir, directory for tables                  */
/*   FILE *fi,        table is read from fi                  */
/*   FILE *fo,        control output is written on fo        */
/*   char *errtx)     string for error                       */
/*                                                           */
/* mode == 'b'                                               */
/* produces a binary table in the file tab_name              */
/*   from the file fi, controloutput to fo                   */
/* when tab_name == "", is tabname taken from label in fi    */
/* format of input file : usual as given by Rene Forsberg    */
/*                                                           */
/* mode == 'z'                                               */
/* ass mode == 'b' : except the table is set NILL (== 9999.99) */
/*   in the file fi the table is omitted                     */
/*                                                           */
/* mode == 't'                                               */
/* produces a text table in the file out                     */
/*   from the binary file tab_name, controloutput to fo      */
/* format of output file : with header  (== 1)               */
/*                                                           */
/* mode == 'l'                                               */
/* produces a list of the table in given coord_sys           */
/* nr    crd1    crd2    val    val                          */
/*   from the binary file tab_name, controloutput to fo      */

static void          reset_dir();
static char          sav_dir[256], in[256];

int                  tab_adm_f(
/*___________________________*/
  char *tab_name,    /*  name of table                   */
  char  mode,        /* mode = 'b' / 't' / 'd'           */
  int   header,      /* header (in i/o) = 0/1  recommended == 1   */
  int   hd_limits,   /* hd_limits in (i/file) = 0/1  always == 1   */
  char *global_dir,  /* directory for tables             */
  FILE *fi,          /* table is read from fi            */
  FILE *fo,          /* control output is written on fo  */
  char *errtx        /* hard errors                      */
)
{

  extern THREAD_SAFE FILE           *man_tab_file;

  union geo_lab          t_lab_u;
  union geo_lab          x_lab_u;
  struct gde_lab        *t_lab = &(t_lab_u.u_g_lab);
  struct gde_lab        *x_lab = &(x_lab_u.u_g_lab);
  struct coord_lab      *c_lab = &(x_lab_u.u_c_lab);

  double                *table;

  int                    n = 0, e, n1, e1, j, val_mode;
  int                    n_max, e_max, recsz, estop, r_size;
  int                    tbwd;
  size_t                 pos = 0, fill = 0, pgsize, qr_t;
  size_t                 size = 0, sz_u, chsz = 0, load_sz, rest_sz;
  int                    head = GEOIDHEADER*4*GEOIDPGSIZE;
  double                 B, L, B1, L1;
  double                 diff, lim = 1.0;
  int                    r, s, qr, res = 0, lab_ok = 0;
  int                    geo_def = 0, utm_def = 0, prj_def = 0;
  int                    gdd, used, pr_val, dim3 = 1;
  char                  *ptx = in, tp[64], *p_tp = tp;
  char                  *prst, des[64], des1[64], h_dtm[MLBLNG];
  struct stat            buf;
  double                 ff;
  int                    f777;    /* identifier of grid table       */
  double                 BL[6];   /* Bmin, Bmax, Lmin, Lmax, dB, dL */
  int                    ezf[3];  /* ellipsoid, utm zone, fill      */

  FILE                  *fd = (FILE *) NULL, *in_2d = (FILE *) NULL, *in_3d = (FILE *) NULL, *f_act = (FILE *) NULL;

  struct typ_dec         g_tpd, r_tpd;

  if (global_dir == NULL) {
    (void) sprintf(errtx, "\nglobal_dir undefined");
    return(-1);
  }

prst = gettabdir();
#if defined(sparc) || defined(__sparc)
      if (*prst == '\0') global_dir = settabdir(NULL);
#endif
  (void) strcpy(sav_dir, prst);
  if (strcmp(sav_dir, global_dir)) {
    c_tabdir_file(0, NULL); // close saved GLOBALS
    (void) settabdir(global_dir);
    (void) i_tabdir_file(1, "", &e, in); // open new GLOBALS
  }

  switch (mode) {
  case 'b': /* CREATE BINARY TABLE */
  case 'z': /* CREATE BINARY TABLE  zero value */

    if (mode == 'z') mode = '\0';

    if (*tab_name == '\0') {
      if (fgetlhtx(fi, tab_name) == 0) {
        (void) fseek(fi, 0L, SEEK_SET);
      } else {
        (void) sprintf(errtx,
               "\n\ntab_adm_f : txtfile no label found ***\n");
        reset_dir();
        return(-1);
      }
    }

    (void) fprintf(fo, "\nCREATE A BINARY TABLE %s", tab_name);

    set_lord_error_mode(NULL, "all");
    set_lord_warning_mode(NULL, "all");

    qr     = conv_lab(tab_name, &t_lab_u, "");
    lab_ok = (qr == GDE_LAB) || (qr == DTM_LAB) || (qr == DHH_LAB)
          || (qr == UPL_LAB) || (qr == DEM_LAB) || (qr == TAB_LAB)
          || (qr == BAG_LAB) || (qr == T3D_LAB) || (qr == T2D_LAB)
          || (qr == TDD_LAB);

    if (!lab_ok) {
      (void) fprintf(fo,
             "\ntab_adm_f: new file %s not found", tab_name);
      (void) fprintf(fo,
             "\ntab_adm_f: +++table label %s vil be created+++;\n",
             tab_name);
      if (man_tab_file == NULL) (void) i_tabdir_file(1, "", &e, in);
      (void) strcpy(t_lab->mlb, tab_name);
      t_lab->local = *in == '\0';
      t_lab->sepix = (short) strlen(tab_name);
/* GEOID_T skal repareres */
      e = geoid_t(t_lab, des, h_dtm, des1);
      if (e) {
        (void) sprintf(errtx,
               "\ntab_adm_f check: %s not found in manager.tab ***\n",
               t_lab->mlb);
        reset_dir();
        return(-1);
      }

      (void) strcpy(t_lab->clb, des);
      (void) fprintf(fo, "\ncrd label fra manager : %s", des);
      if (conv_lab(t_lab->clb, &x_lab_u, "") != CRD_LAB) {
        (void) sprintf(errtx,
               "\ntab_adm_f check: projection %s not CRD_LAB ***\n",
               t_lab->clb);
        reset_dir();
        return(-1);
      }
      geo_def      = c_lab->cstm == 2;
      utm_def      = c_lab->cstm == 3 &&
                     -1 <= c_lab->mode && c_lab->mode <= 1;
      prj_def      = !(geo_def || utm_def);
      t_lab->datum = c_lab->datum;
/* cstm    -99: skal repareres */
/* NO rounding: skal stå i label info, HVIS nødvendig */
      /*     cstm    -99  is used for geo NO rounding                 */
      t_lab->cstm  = (short) ((geo_def) ? ((hd_limits == 1) ? 0 : -99) :
                     (utm_def) ? get_zone(&x_lab_u) : -c_lab->cstm);
      t_lab->mode  = (short) ((prj_def) ? c_lab->mode : 0);

    } else {
      geo_def      = t_lab->cstm == 0 || t_lab->cstm == -99;
      utm_def      = t_lab->cstm  > 0;
      prj_def      = !(geo_def || utm_def);
      if (geo_def && hd_limits >= 2) t_lab->cstm = -99;
      else if (t_lab->cstm == -99) hd_limits = 2; /* NO rounding */
      g_tpd        = (!geo_def) ? *set_tpd("m", 10, 0)
                   : (hd_limits == 1) ? *set_tpd("sx", 10, 0) : *set_tpd("dg", 10, 6);
      fclose(t_lab->fd);
    }
    dim3      = (t_lab->lab_type == T3D_LAB) ? 3
              : (t_lab->lab_type == T2D_LAB) ? 2
              : (t_lab->lab_type == TDD_LAB) ? 2 : 1;
    r_size    = abs(t_lab->rec_size);
/* val_mode : unødig/skal repareres */
    val_mode  = (r_size == 8) ? 3 : ((t_lab->rec_size > 0) ? 1 : 2);
    /*                     double                        float int */
/* pgsize : unødig/skal repareres */
    pgsize    = (dim3 == 1) ? GEOIDPGSIZE :
                ((dim3 == 2 && val_mode == 3) ? TABDDPGSIZE :
                ((dim3 == 2) ? TAB3DPGSIZE * 2 / 3 : TAB3DPGSIZE));
/* hvis header room : new format/skal repareres */
    *ezf      = t_lab->datum;
    *(ezf +1) = t_lab->cstm;
    *(ezf +2) = t_lab->mode;
    
    if (dim3 > 1) {
      (void) strcpy(des, t_lab->mlb);
      (void) strcat(des, "Ex");
      (void) fprintf(fo, "\n\n+++%dD table: E_file = %s", dim3, des);
      if (dim3 > 2) {
        (void) strcpy(des1, t_lab->mlb);
        (void) strcat(des1, "Hx");
        (void) fprintf(fo, ", H_file = %s\n", des1);
      } else (void) fprintf(fo, "\n");
      in_2d = fopen(des, "r");
      if (in_2d == NULL) {
        (void) sprintf(errtx, "\n\n***DDim file: %s NOT FOUND", des);
        reset_dir();
        return(-1);
      }
      if (dim3 > 2) {
        in_3d = fopen(des1, "r");
        if (in_3d == NULL) {
          (void) sprintf(errtx, "\n\n***3Dim file: %s NOT FOUND", des1);
          reset_dir();
          return(-1);
        }
      }
    }

    (void) fprintf(fo, "\nPath:  %s\n\n", t_lab->mlb);

    p_tp = tp;
    if (fgetlhtx(fi, p_tp) == 0) {
      if (lab_ok) {
        if (strcmp(t_lab->mlb, p_tp)) {
          (void) sprintf(errtx, "\n\n***tab_adm_f: %s",
                 " txtfile not correct table label found\n");
          reset_dir();
          return(-1);
        }
      }
      qr = t_lab->lab_type;
    } else {
      (void) sprintf(errtx,
             "\n\n***tab_adm_f : txtfile no label found\n");
      reset_dir();
      return(-1);
    }
    f777 = t_lab->f777;

    (void) fprintf(fo,
        "\nFILNAVN FOR CREATION %s, c_lab = %s", tab_name, t_lab->clb);

    if ((fd = fopen(t_lab->mlb, "wb")) == NULL) {
      (void) sprintf(errtx,
          "\n***tab_adm_f: table file %s not created ;\n", tab_name);
      reset_dir();
      return(-1);
    }

    (void) fprintf(fo, "\n # %s \n\n", tab_name);
    (void) fprintf(fo, "rec_size = %d", r_size);

    /* head of grid_tabs */
    if (hd_limits) {

      /* must be written with terminator */
      strcpy(des; (geo_def) ? "dg" : "m");
      for (r = 0; r < 6; r ++)
          *(BL+r) = fgetg(fi, &r_tpd, &used, des);
      qr = conv_lab("input", &x_lab_u, fi);
      if (qr != CRD_LAB || strcmp(x_lab->mlb, t_lab->clb)) {
        if (qr == CRD_LAB) {
          (void) sprintf(errtx, "\n%s%s%s%s%s)\n",
             "\n***tab_adm_f: crdlab (", x_lab->mlb,
             ") in in_file is\n            :",
             "NOT the same as in manager(", t_lab->clb);
        } else {
          (void) sprintf(errtx, "\n%s\n            :%s(%s)\n",
             "***tab_adm_f: crdlab in in_file is MISSING",
             " the manager claims ", t_lab->clb);
        }
        reset_dir();
        return(-1);
      }

      if (dim3 > 1) {
        /* position files: NO TEST of HEAD info */
        for (j = 1, f_act = in_2d; j < dim3; j ++, f_act = in_3d) {
          if (fgetlhtx(f_act, p_tp) == 0) {
            if (fgetlhtx(f_act, p_tp) != 0) {
              (void) sprintf(errtx,
                   "\n\ntab_adm_f : txtfile: %s no label found ***\n",
                   (j == 1) ? des : des1);
              reset_dir();
              return(-1);
            }
          } else {
            (void) sprintf(errtx,
                   "\n\ntab_adm_f : txtfile: %s no label found ***\n",
                   (j == 1) ? des : des1);
            reset_dir();
            return(-1);
          }
        }
        (void) fprintf(fo, "\n\nN_file pos = %ld", ftell(fi));
        (void) fprintf(fo, "\nE_file pos = %ld", ftell(in_2d));
        if (dim3 > 2) 
           (void) fprintf(fo, "\n\nH_file pos = %ld", ftell(in_3d));
      }

      if (geo_def) {
        /*     sx  -> radian */
        lim = 0.01 / RHO;
        /* change for both longitudes negative */
        if (*(BL+2) < 0.0 && *(BL+3) < 0.0) {
          *(BL+2) += M_PI;
          *(BL+3) += M_PI;
        }

        /* change for equal longitudes */
        if (fabs(*(BL+2) - *(BL+3)) < lim) *(BL+3) += 2 * M_PI;
      } 

      if (lab_ok) {
        for (i = 0, n = 0; i < 6; i ++) {
          if (fabs(((i == 0) ? t_lab->B_min :
              ((i == 1) ? t_lab->B_max :
              ((i == 2) ? t_lab->L_min :
              ((i == 3) ? t_lab->L_min + t_lab->deltaL :
              ((i == 4) ? t_lab->dB    : /*5*/ t_lab->dL))))) - BL[i]) > lim) ++ n;
        }
        r_tpd = t_lab->g_tpd;
      }

      if (n && lab_ok) {
        (void) fprintf(fo, "\n\nLabel changed +++\n");
        (void) fprintf(fo, "\nBmin :");
        (void) fputg(fo, t_lab->B_min, &r_tpd, " +u");
        (void) fprintf(fo, "  ->  ");
        (void) fputg(fo, BL[0], &g_tpd, " +u");
        (void) fprintf(fo, "\nBmax :");
        (void) fputg(fo, t_lab->B_max, &r_tpd, " +u");
        (void) fprintf(fo, "  ->  ");
        (void) fputg(fo, BL[1], &g_tpd, " +u");
        (void) fprintf(fo, "\nLmin :");
        (void) fputg(fo, t_lab->L_min, &r_tpd, " +u");
        (void) fprintf(fo, "  ->  ");
        (void) fputg(fo, BL[2], &g_tpd, " +u");
        (void) fprintf(fo, "\nLmax :");
        (void) fputg(fo, t_lab->L_min + t_lab->deltaL,
                     &r_tpd, " +u");
        (void) fprintf(fo, "  ->  ");
        (void) fputg(fo, BL[3], &g_tpd, " +u");
        (void) fprintf(fo, "\ndB   :");
        (void) fputg(fo, t_lab->dB, &r_tpd, " +u");
        (void) fprintf(fo, "  ->  ");
        (void) fputg(fo, BL[4], &g_tpd, " +u");
        (void) fprintf(fo, "\ndL   :");
        (void) fputg(fo, t_lab->dL, &r_tpd, " +u");
        (void) fprintf(fo, "  ->  ");
        (void) fputg(fo, BL[5], &g_tpd, " +u");
        t_lab->B_min    = BL[0];
        t_lab->B_max    = BL[1];
        t_lab->L_min    = BL[2];
        t_lab->deltaL   = (BL[3] - BL[2]);
        t_lab->dB       = BL[4];
        t_lab->dL       = BL[5];
        t_lab->g_tpd    = g_tpd;
        t_lab->n_max    = (int) ((t_lab->B_max - t_lab->B_min)
                                               / t_lab->dB + 1.1);
        t_lab->e_max    = (int) ((t_lab->deltaL              )
                                               / t_lab->dL + 1.1);
        t_lab->estop    = ((t_lab->e_max + t_lab->rec_p_bl -1)
                                 / t_lab->rec_p_bl) * t_lab->rec_p_bl;
        t_lab->row_size = (t_lab->estop / t_lab->rec_p_bl)
                                        * t_lab->blk_size;
      }
      else {
        t_lab->B_min    = BL[0];
        t_lab->B_max    = BL[1];
        t_lab->L_min    = BL[2];
        t_lab->deltaL   = (BL[3] - BL[2]);
        t_lab->dB       = BL[4];
        t_lab->dL       = BL[5];
        t_lab->g_tpd    = g_tpd;
        t_lab->n_max    = (int) ((t_lab->B_max - t_lab->B_min)
                                               / t_lab->dB + 1.1);
        t_lab->e_max    = (int) ((t_lab->deltaL              )
                                               / t_lab->dL + 1.1);
        t_lab->nbase    = (GEOIDHEADER) * 4L * (GEOIDPGSIZE);
        t_lab->blk_size = pgsize * r_size;
        t_lab->rec_p_bl = (int) (t_lab->blk_size / r_size / dim3);
        t_lab->estop    = ((t_lab->e_max + t_lab->rec_p_bl -1)
                                 / t_lab->rec_p_bl) * t_lab->rec_p_bl;
        t_lab->row_size = (t_lab->estop / t_lab->rec_p_bl)
                                        * t_lab->blk_size;
        t_lab->f_comp   = 2;
      }
    } else {
      (void) sprintf(errtx, "%s%s",
             "\n\ntab_adm_f missing label or header limits ::", 
             "\n    B_min, B_max, L_min, L_max, dB, dL\n");
      reset_dir();
      return(-1);
    }
    (void) fprintf(fo, "\n\nFrom label\n");
    (void) fprintf(fo, "\nBmax :");
    (void) fputg(fo, t_lab->B_max, &g_tpd, " +u");
    (void) fputg(fo, BL[1], &g_tpd, " +u");
    (void) fprintf(fo, "\nBmin :");
    (void) fputg(fo, t_lab->B_min, &g_tpd, " +u");
    (void) fputg(fo, BL[0], &g_tpd, " +u");
    (void) fprintf(fo, "\nLmax :");
    (void) fputg(fo, t_lab->L_min + t_lab->deltaL, &g_tpd, " +u");
    (void) fputg(fo, BL[3], &g_tpd, " +u");
    (void) fprintf(fo, "\nLmin :");
    (void) fputg(fo, t_lab->L_min, &g_tpd, " +u");
    (void) fputg(fo, BL[2], &g_tpd, " +u");
    (void) fprintf(fo, "\ndB   :");
    (void) fputg(fo, t_lab->dB, &g_tpd, " +u");
    (void) fputg(fo, BL[4], &g_tpd, " +u");
    (void) fprintf(fo, "\ndL   :");
    (void) fputg(fo, t_lab->dL, &g_tpd, " +u");
    (void) fputg(fo, BL[5], &g_tpd, " +u");

    load_sz = t_lab->row_size;
    table   = malloc(load_sz);
    e_max   = t_lab->e_max;
    estop   = t_lab->estop;

/* hvis header room : new format/skal repareres */
    /* write HEAD info */
    if (fwrite((void *) &f777, sizeof(int), 1, fd) - 1) {
      (void) sprintf(errtx,
          "\n*** tab_adm_f: ERROR at output of table-mark");
      reset_dir();
      return(-1);
    }
    pos = ftell(fd);
    (void) fprintf(fo, "\n\nPos after f777  %ld", pos);

    if (fwrite((void *) BL, sizeof(BL), 1, fd) - 1) {
      (void) sprintf(errtx,
          "\n*** tab_adm_f: ERROR at output of lat/long info");
      reset_dir();
      return(-1);
    }
    pos = ftell(fd);
    (void) fprintf(fo, "\nPos after BL   %ld", pos);

    if (fwrite((void *) ezf, sizeof(ezf), 1, fd) - 1) {
      (void) sprintf(errtx,
          "\n*** tab_adm_f: ERROR at output of projection info");
      reset_dir();
      return(-1);
    }
    pos = ftell(fd);
    (void) fprintf(fo, "\nPos after ezf  %ld", pos);

    /* NO TEST OF 2dim table head and 3dim table head */

    /* input of table data */
    pr_val = 0;
    for (n = 0, B = t_lab->B_max; n < t_lab->n_max;
        n ++, B = t_lab->B_max - n*t_lab->dB) {
      for (e = 0, s = 0, L = t_lab->L_min; e < estop;
          e ++, L = t_lab->L_min + e*t_lab->dL) {
        ff = 0.0;
        if ((e%t_lab->rec_p_bl) == 0) {

          if (header && mode) {
            if (dim3 > 1) {
              (void) fgetln_kms(ptx, &qr, in_2d);
              if (dim3 > 2) (void) fgetln_kms(ptx, &qr, in_3d);
            }
            (void) fprintf(fo, "\n");
            ptx  = in;
            used = 0;
            (void) fgetln_kms(ptx, &qr, fi);
            B1   = sgetg(ptx, &g_tpd, &used, "m");
            ptx += used;
            L1 = sgetg(ptx, &g_tpd, &used, "m");
            ptx += used;
            (void) sscanf(ptx, "%d %d", &n1, &e1);
            (void) fputg(fo, B1, &g_tpd, " +u");
            (void) fprintf(fo, "  ");
            (void) fputg(fo, L1, &g_tpd, " +u");
            (void) fprintf(fo, "  ");
            (void) fprintf(fo, "   n = %6d  e = %6d\n", n1, e1);
            pr_val = n<2 || n == t_lab->n_max;
            if (n != n1 || e != e1) {
              (void) fprintf(fo, "\n*** tab_adm_f: Input-error:");
              (void) fprintf(fo, "\nExpected input : ");
              (void) fputg(fo, B, &g_tpd, " +u");
              (void) fputg(fo, L, &g_tpd, " +u");
              (void) fprintf(fo, "   n = %6d  e = %6d ;", n, e);
              (void) fprintf(fo, "\nREAD           : ");
              (void) fputg(fo, B1, &g_tpd, " +u");
              (void) fputg(fo, L1, &g_tpd, " +u");
              (void) fprintf(fo, "   n = %6d  e = %6d ;\n", n1, e1);
              exit(-1);
            }
          }
          else 
          if (mode) {
            (void) fprintf(fo, "\n* Input block :");
            (void) fputg(fo, B, &g_tpd, " +u");
            (void) fputg(fo, L, &g_tpd, " +u");
            (void) fprintf(fo, "   n = %4d  e = %4d", n, e);
            if (pr_val) (void) fprintf(fo, " ;\n");
          }
        }

/* ER DER andet end doubles ? */
        /* grid_tab of double */
        j = e * dim3;
        if (e < e_max && mode) {
          *(table + j) = fgetg(fi, r_tpd, "m");
          ++ chsz;
          if (res == EOF) {
            (void) fprintf(fo, "\n\n*** TABLE OUT n = %d, e= %d ;\n",
                           n, e);
            reset_dir();
            return(-1);
          }
          for (r = 1, f_act = in_2d; r < dim3; f_act = in_3d, r++) {
            *(table +j +r) = fgetg(f_act, r_tpd, "m");
            if (res == EOF) {
              (void) fprintf(fo, "\n\n*** %c-TAB OUT n = %d, e= %d ;\n",
                             (r == 1) ? 'E' : 'H', n, e);
            }
          }
          if (pr_val) {
            (void) fprintf(fo, "\n   val(%d) = ", j);
            for (r = 0; r < dim3; r++)
                (void) fprintf(fo, "%5.*lf(%ld)  ", r_tpd->df, *(table + j + r),
                  (!r)?ftell(fi) : (r==1)?ftell(in_2d) :ftell(in_3d));
            (void) fprintf(fo, " ;\n");
            pr_val = 0;
          }
        }
        else {
          *(table + j) = 99999999.99;
          if (dim3 > 1) {
            *(table + j +1) = 99999999.99;
            if (dim3 > 2) *(table + j +2) = 99999999.99;
          }
          ff = 99999999.99;
          ++ fill;
        }

        if (pr_val) {
          if (e==0) (void) fprintf(fo, "\n");
          (void) fprintf(fo, "\n n:%3d, e: %3d, g(%d):", n, e, j);
          for (r = 0; r < dim3; r++)
            (void) fprintf(fo, "  %11.*lf", r_tpd->df, *(table + j + r));
          (void) fprintf(fo, "   ");
          (void) fputg(fo, B, &g_tpd, " +u");
          (void) fputg(fo, L, &g_tpd, " +u");
          pr_val = 0;
        }

      } /* end e-loop */

      /* grid_tab of double */
      qr_t = fwrite((void *) table, (size_t) load_sz, 1, fd);
      if (qr_t != 1) {
        (void) fprintf(fo,
            "\n*** tab_adm_f: ERROR at output of table value");
        (void) fprintf(fo,
            "\n blockaddr %8ld size = %4ld ;",
            ftell(fd), (long) load_sz);
      }

    } /* end n_loop */
    t_lab->tab_dec = r_tpd->df;
/* SKAL GEMMES I POM (T og PRAGT) */

    if (!stat(tab_name, &buf)) {
      size = buf.st_size;
      (void) fprintf(fo,
          "\n\n\nTABLE\n  %-18s  %8ld bytes", t_lab->mlb, size);
    } else {
      (void) sprintf(errtx, "\nTABLE %s not found\n", t_lab->mlb);
      reset_dir();
      return(-1);
    }

    chsz *= r_size;
    fill *= r_size;
    sz_u  = (chsz + fill) * dim3 + head;
    (void) fprintf(fo, "\n  %-18s  %8d   -  ", "Head", head);
    (void) fprintf(fo, "\n  %-18s  %ld   -  ", "Table data", chsz);
    if (dim3 == 2) (void) fprintf(fo, "  2 dim.");
    else
    if (dim3 == 3) (void) fprintf(fo, "  3 dim.");
    (void) fprintf(fo, "\n  %-18s  %8ld   -  ", "Fill-in", fill);
    if (dim3 == 2) (void) fprintf(fo, "  2 dim.");
    else
    if (dim3 == 3) (void) fprintf(fo, "  3 dim.");
    if (size == sz_u) (void) fprintf(fo, "\nOK");
    else {
      (void) sprintf(errtx, "\n****tab_adm_f: %s input error ;\n",
                     t_lab->mlb);
      reset_dir();
      return(-1);
    }

    break; /* end binary output */

  case 't': /* OUTPUT A TABLE  AS TEXT */

    (void) fprintf(fo, "\nOUTPUT TABLE %s AS TEXT", tab_name);

    qr       = conv_lab(tab_name, &t_lab_u, "");
    f777     = t_lab->f777;
    lab_ok   = 767 < f777 && f777 <= 777;
    dim3      = (t_lab->lab_type == T3D_LAB) ? 3
              : (t_lab->lab_type == T2D_LAB) ? 2
              : (t_lab->lab_type == TDD_LAB) ? 2 : 1;
    r_size   = abs(t_lab->rec_size);
/* val_mode : unødig/skal repareres */
    val_mode = (r_size == 8) ? 3 : ((t_lab->rec_size > 0) ? 1 : 2);
    /*                    double                        float int */
/* pgsize : unødig/skal repareres */
    pgsize    = (dim3 == 1) ? GEOIDPGSIZE :
                ((dim3 == 2 && val_mode == 3) ? TABDDPGSIZE :
                ((dim3 == 2) ? TAB3DPGSIZE * 2 / 3 : TAB3DPGSIZE));
    utm_def  = t_lab->cstm  > 0;
    geo_def  = t_lab->cstm == 0 || t_lab->cstm == -99;
    
    if (dim3 > 1) {
      (void) strcpy(des, t_lab->mlb);
      (void) strcat(des, "Ex");
      (void) fprintf(fo, "\n\n+++%dD table: E_file = %s", dim3, des);
      if (dim3 > 2) {
        (void) strcpy(des1, t_lab->mlb);
        (void) strcat(des1, "Hx");
        (void) fprintf(fo, ", H_file = %s\n", des1);
      } else (void) fprintf(fo, "\n");
      in_2d = fopen(des, "w+");
      if (in_2d == NULL) {
        (void) sprintf(errtx, "\n\n***DDim file: %s NOT FOUND", des);
        reset_dir();
        return(-1);
      }
      if (dim3 > 2) {
        in_3d = fopen(des1, "w+");
        if (in_3d == NULL) {
          (void) sprintf(errtx, "\n\n***3Dim file: %s NOT FOUND", des1);
          reset_dir();
          return(-1);
        }
      }
    }

    if (geo_def) {
      /*     sx  -> radian */
      lim = 0.01 / RHO;
    }
    if (!lab_ok) {
      (void) sprintf(errtx,
                     "\n*** tab_adm_f: label %s not ok;\n", tab_name);
      reset_dir();
      return(-1);
    }
    (void) conv_lab("OUTPUT", &t_lab_u, fo);


/* tab_adm_f                       # page 15   27 Jun 2004 22 00 */


    if (t_lab->local) *in = '\0';
    else (void) strcpy(in, global_dir);
    (void) strcat(in, tab_name);
    if (!stat(in, &buf)) {
      size = buf.st_size;

      chsz = size - head;
      tbwd = chsz/t_lab->n_max/r_size;

      /* consistency check */
      if (tbwd*t_lab->n_max*r_size != chsz) {
        (void) fprintf(fo, "\n*** tab_adm_f: inconsistent file and label");
        (void) fprintf(fo, "\n*     TABLE:  %s\n size = %ld ;\n", t_lab->mlb, size);
        (void) fprintf(fo, "\nfile: %s  size (incl. head): %ld head: %d ",
            t_lab->mlb, size, head);
        (void) fprintf(fo, "\nrows: %d datacols: %d columns %d appar. size: %d\n",
            t_lab->n_max, t_lab->e_max, tbwd, tbwd*t_lab->n_max*r_size + head);

        (void) sprintf(errtx,
            "\n*** tab_adm_f: inconsistent file and label");
        reset_dir();
        return(-1);
      }
    }
    else {
      (void) sprintf(errtx, "\nTABLE %s not found\n", t_lab->mlb);
      reset_dir();
      return(-1);
    }
    g_tpd    = t_lab->g_tpd;
    gdd      = t_lab->gd_dec;

    (void) fprintf(fo, "\nFrom CONV_TAB  %ld bytes expected\n", size);
    (void) fprintf(fo, "\nFull Path for input: %s\n\n", in);

    fd  = t_lab->fd;
    pos = 0;

    for (f_act = fo, r = 0; r < dim3; r ++) {
      (void) fprintf(f_act, "\n # %s \n\n", t_lab->mlb);
      (void) fprintf(f_act, "\n\n");
      (void) fputg(f_act, t_lab->B_min, &g_tpd, " +u");
      (void) fputg(f_act, t_lab->B_max, &g_tpd, " +u");
      (void) fputg(f_act, t_lab->L_min, &g_tpd, " +u");
      if (geo_def && fabs(t_lab->L_min + t_lab->deltaL - 2*M_PI) < lim)
         (void) fprintf(f_act, "    +360.000000");
      else
        (void) fputg(f_act, t_lab->L_min + t_lab->deltaL, &r_tpd, " +u");
      (void) fprintf(f_act, "\n");
      (void) fputg(f_act, t_lab->dB, &g_tpd, " +u");
      (void) fputg(f_act, t_lab->dL, &g_tpd, " +u");
      (void) fprintf(f_act, "\n");
      (void) fprintf(f_act, "#%s\n\n", t_lab->clb);
      f_act = (r == 0) ? in_2d : in_3d;
    }

    if (hd_limits) {
/* hvis header room : new format/skal repareres */
      (void) fseek(fd, pos, SEEK_SET);
      if (fread((void *) &res, sizeof(int), 1, fd) - 1) {
        (void) sprintf(errtx,
            "\n*** tab_adm_f: ERROR at input of table-mark");
        reset_dir();
        return(-1);
      }

      if (f777 != res) {
        (void) sprintf(errtx,
            "\n*** tab_adm_f: %s is not a table of correct type;\n",
            t_lab->mlb);
        reset_dir();
        return(-1);
      }

      if (fread((void *) BL, sizeof(BL), 1, fd) - 1) {
        (void) sprintf(errtx,
            "\n*** tab_adm_f: ERROR at input of table-limits");
        reset_dir();
        return(-1);
      }

      if (fread((void *) ezf, sizeof(ezf), 1, fd) - 1) {
        (void) sprintf(errtx,
            "\n*** tab_adm_f: ERROR at input of projection-info");
        reset_dir();
        return(-1);
      }

      for (e = 0, qr = 0, n = 0; e < 6; e++) {
        diff =  (e == 0) ? t_lab->B_min :
               ((e == 1) ? t_lab->B_max :
               ((e == 2) ? t_lab->L_min :
               ((e == 3) ? t_lab->L_min + t_lab->deltaL :
               ((e == 4) ? t_lab->dB   : /*5*/ t_lab->dL))));
        if (fabs(diff - BL[e]) > lim) {
          if (! geo_def) n ++;
          else {
            if ((fabs(diff - BL[e] - 2*M_PI) > lim)
             && (fabs(diff - BL[e] + 2*M_PI) > lim)) n ++;
          }
        }
        if (n == 1 && qr == 0) (void) fprintf(fo,
             "\n\n*** LABEL og TABEL ikke konsistente ***\n");
        if (n != qr) {
          (void) fprintf(fo, "BL[%d] =", e);
          (void) fputg(fo, *(BL+e), &g_tpd, " +u");
          (void) fprintf(fo, ", t_lab =");
          (void) fputg(fo, ", diff, &g_tpd, " +u");
          (void) fprintf(fo, ", diff =");
          (void) fputg(fo, diff - BL[e], &g_tpd, " +u");
          (void) fprintf(fo, "\n");
          qr = n;
        }
      }
      if (n) {
        (void) sprintf(errtx,
             "\n\n*** LABEL og TABEL ikke konsistente ***\n");
        reset_dir();
        return(-1);
      }
    }

    chsz    = 0;
    load_sz = t_lab->row_size;
    table   = malloc(load_sz);
    e_max   = t_lab->e_max;
    r_tpd   = t_lab->g_tpd;

    gdd = t_lab->tab_dec;
    for (n = 0, B = t_lab->B_max; n < t_lab->n_max; n++) {
      /* grid_tab of double */
      qr_t = fread((void *) table, (size_t) load_sz, 1, fd);
      if (qr_t != 1) {
        (void) sprintf(errtx,
            "\n*** tab_adm_f: FEJL ved input af table values");
        reset_dir();
        return(-1);
      }

      B = t_lab->B_max - n*t_lab->dB;
      for (e = 0; e < e_max; e++) {
        L = t_lab->L_min + e*t_lab->dL;

        if ((e%t_lab->rec_p_bl) == 0) {
          (void) fprintf(fo, "\n\n");
          if (dim3 > 1) (void) fprintf(in_2d, "\n\n");
          if (dim3 > 2) (void) fprintf(in_3d, "\n\n");

          if (header) {
            for (f_act = fo, r = 0; r < dim3; r ++) {
              (void) fputg(f_act, B, &r_tpd, " +u");
              (void) fputg(f_act, L, &r_tpd, " +u");
              (void) fprintf(f_act, "  %4d  %4d", n, e);
              f_act = (r == 0) ? in_2d : in_3d;
            }
          }
        }

        if ((e%8) == 0) {
          (void) fprintf(fo, "\n");
          if (dim3 > 1) (void) fprintf(in_2d, "\n");
          if (dim3 > 2) (void) fprintf(in_3d, "\n");
        }
        ++ chsz;
        j = e * dim3;
        for (f_act = fo, r = 0; r < dim3; r ++) {
          (void) fprintf(f_act, "  %+11.*f", gdd, *(table +j +r));
          f_act = (r == 0) ? in_2d : in_3d;
        }

      } /* end e-loop */
    } /* end n_loop */

    chsz *= r_size;
    fill  = (t_lab->estop - e_max) * t_lab->n_max * r_size;
    sz_u  = (chsz + fill) * dim3 + head;
    for (f_act = fo, r = 0; r < dim3; r ++) {
      (void) fprintf(f_act,
            "\n\n\nTABLE\n  %-18s  %8ld bytes", t_lab->mlb, size);
      (void) fprintf(f_act, "\n  %-18s  %8d   -  ", "Head", head);
      (void) fprintf(f_act, "\n  %-18s  %ld   -  ", "Table data", chsz);
      if (dim3 == 2) (void) fprintf(f_act, "  2 dim.");
      else
      if (dim3 == 3) (void) fprintf(f_act, "  3 dim.");
      (void) fprintf(f_act, "\n  %-18s  %8ld   -  ", "Fill-in", fill);
      if (dim3 == 2) (void) fprintf(f_act, "  2 dim.");
      else
      if (dim3 == 3) (void) fprintf(f_act, "  3 dim.");
      f_act = (r == 0) ? in_2d : in_3d;
    }
    if (size == sz_u) {
      (void) fprintf(fo, "\nOK\n\n");
      if (dim3 > 1) (void) fprintf(in_2d, "\nOK\n\n");
      if (dim3 > 2) (void) fprintf(in_3d, "\nOK\n\n");
    } else {
      (void) sprintf(errtx, "\n****tab_adm_f: %s input error ;\n",
                     t_lab->mlb);
      reset_dir();
      return(-1);
    }

  break; /* Output of table  */

  case 'l': /* OUTPUT A TABLE  AS LIST */

    (void) fprintf(fo, "\nOUTPUT TABLE %s AS LIST", tab_name);

    qr       = conv_lab(tab_name, &t_lab_u, "");
    f777     = t_lab->f777;
    lab_ok   = 767 < f777 && f777 <= 777;
    dim3      = (t_lab->lab_type == T3D_LAB) ? 3
              : (t_lab->lab_type == T2D_LAB) ? 2
              : (t_lab->lab_type == TDD_LAB) ? 2 : 1;
    r_size   = abs(t_lab->rec_size);
/* val_mode : unødig/skal repareres */
    val_mode = (r_size == 8) ? 3 : ((t_lab->rec_size > 0) ? 1 : 2);
    /*                    double                        float int */
/* pgsize : unødig/skal repareres */
    pgsize    = (dim3 == 1) ? GEOIDPGSIZE :
                ((dim3 == 2 && val_mode == 3) ? TABDDPGSIZE :
                ((dim3 == 2) ? TAB3DPGSIZE * 2 / 3 : TAB3DPGSIZE));
    utm_def  = t_lab->cstm  > 0;
    geo_def  = t_lab->cstm == 0 || t_lab->cstm == -99;
    
    if (dim3 > 1) {
      (void) strcpy(des, t_lab->mlb);
      (void) strcat(des, "Ex");
      (void) fprintf(fo, "\n\n+++%dD table: E_file = %s", dim3, des);
      if (dim3 > 2) {
        (void) strcpy(des1, t_lab->mlb);
        (void) strcat(des1, "Hx");
        (void) fprintf(fo, ", H_file = %s\n", des1);
      } else (void) fprintf(fo, "\n");
      in_2d = fopen(des, "w+");
      if (in_2d == NULL) {
        (void) sprintf(errtx, "\n\n***DDim file: %s NOT FOUND", des);
        reset_dir();
        return(-1);
      }
      if (dim3 > 2) {
        in_3d = fopen(des1, "w+");
        if (in_3d == NULL) {
          (void) sprintf(errtx, "\n\n***3Dim file: %s NOT FOUND", des1);
          reset_dir();
          return(-1);
        }
      }
    }
    
    if (geo_def) {
      /*     sx  -> radian */
      lim = 0.01 / RHO;
    }

    if (!lab_ok) {
      (void) sprintf(errtx,
                     "\n*** tab_adm_f: label %s not ok;\n", tab_name);
      reset_dir();
      return(-1);
    }

    (void) conv_lab("OUTPUT", &t_lab_u, fo);
    if (t_lab->local) *in = '\0';
    else (void) strcpy(in, global_dir);
    (void) strcat(in, tab_name);
    if (!stat(in, &buf)) {
      size = buf.st_size;

      chsz = size - head;
      tbwd = chsz/t_lab->n_max/r_size;

      /* consistency check */
      if (tbwd*t_lab->n_max*r_size != chsz) {
        (void) fprintf(fo, "\n*** tab_adm_f: inconsistent file and label");
        (void) fprintf(fo, "\n*     TABLE:  %s\n size = %ld ;\n", t_lab->mlb, size);
        (void) fprintf(fo, "\nfile: %s  size (incl. head): %ld head: %d ",
            t_lab->mlb, size, head);
        (void) fprintf(fo, "\nrows: %d datacols: %d columns %d appar. size: %d\n",
            t_lab->n_max, t_lab->e_max, tbwd, tbwd*t_lab->n_max*r_size + head);

        (void) sprintf(errtx,
            "\n*** tab_adm_f: inconsistent file and label");
        reset_dir();
        return(-1);
      }
    }
    else {
      (void) sprintf(errtx, "\nTABLE %s not found\n", t_lab->mlb);
      reset_dir();
      return(-1);
    }
    g_tpd    = t_lab->g_tpd;
    gdd      = t_lab->gd_dec;

    (void) fprintf(fo, "\nFull Path for input: %s\n\n", in);

    fd  = t_lab->fd;
    pos = 0;

    for (f_act = fo, r = 0; r < dim3; r ++) {
      (void) fprintf(fo, "\n#%s\n", t_lab_u.u_g_lab.clb);
      f_act = (r == 0) ? in_2d : in_3d;
    }

    if (hd_limits) {
/* hvis header room : new format/skal repareres */
      (void) fseek(fd, pos, SEEK_SET);
      if (fread((void *) &res, sizeof(int), 1, fd) - 1) {
        (void) sprintf(errtx,
            "\n*** tab_adm_f: ERROR at input of table-mark");
        reset_dir();
        return(-1);
      }

      if (f777 != res) {
        (void) sprintf(errtx,
            "\n*** tab_adm_f: %s is not a table of correct type;\n",
            t_lab->mlb);
        reset_dir();
        return(-1);
      }

      if ((qr = (int) fread((void *) BL, sizeof(BL), 1, fd)) - 1) {
        (void) sprintf(errtx,
            "\n*** tab_adm_f: ERROR at input of table-limits");
        reset_dir();
        return(-1);
      }

      if (fread((void *) ezf, sizeof(ezf), 1, fd) - 1) {
        (void) sprintf(errtx,
            "\n*** tab_adm_f: ERROR at input of projection-info");
        reset_dir();
        return(-1);
      }

      for (e = 0, qr = 0, n = 0; e < 6; e++) {
        diff =  (e == 0) ? t_lab->B_min :
               ((e == 1) ? t_lab->B_max :
               ((e == 2) ? t_lab->L_min :
               ((e == 3) ? t_lab->L_min + t_lab->deltaL :
               ((e == 4) ? t_lab->dB    : /*5*/ t_lab->dL))));
        if (fabs(diff - BL[e]) > lim) {
          if (! geo_def) n ++;
          else {
            if ((fabs(diff - BL[e] - 2*M_PI > lim)
             && (fabs(diff - BL[e] + 2*M_PI > lim)) n ++;
          }
        }
        if (n == 1 && qr == 0) (void) fprintf(fo,
             "\n\n*** LABEL og TABEL ikke konsistente ***\n");
        if (n != qr) {
          (void) fprintf(fo, "BL[%d] =", e);
          (void) fputg(fo, *(BL+e), &g_tpd, " +u");
          (void) fprintf(fo, ", t_lab =");
          (void) fputg(fo, ", diff, &g_tpd, " +u");
          (void) fprintf(fo, ", diff =");
          (void) fputg(fo, diff - BL[e], &g_tpd, " +u");
          (void) fprintf(fo, "\n");
          qr = n;
        }
      }
      if (n) {
        (void) sprintf(errtx,
             "\n\n*** LABEL og TABEL ikke konsistente ***\n");
        reset_dir();
        return(-1);
      }
    }

    s       = 0;
    chsz    = 0;
    load_sz = t_lab->row_size;
    table   = malloc(load_sz);
    e_max   = t_lab->e_max;
    r_tpd   = t_lab->g_tpd;
    gdd     = t_lab->tab_dec;

    for (n = 0, B = t_lab->B_max; n < t_lab->n_max; n++) {
      /* grid_tab of double */
      qr_t = fread((void *) table, (size_t) load_sz, 1, fd);
      if (qr_t != 1) {
        (void) sprintf(errtx,
            "\n*** tab_adm_f: FEJL ved input af table values");
        reset_dir();
        return(-1);
      }

      B       = t_lab->B_max - n*t_lab->dB;

      for (e = 0; e < e_max; e++) {
        L = t_lab->L_min + e*t_lab->dL;

		j = e * dim3;
        ++ s;
        ++ chsz;
        for (f_act = fo, r = 0; r < dim3; r ++) {
          (void) fprintf(f_act, "\n%5d  ", s);
          (void) fputg(f_act, B, &r_tpd, "+u");
          (void) fprintf(f_act, "  ");
          (void) fputg(f_act, L, &r_tpd, "+u");
          (void) fprintf(f_act, "  %+11.*f", gdd, *(table +j +r));
          f_act = (r == 0) ? in_2d : in_3d;
        }

      } /* end e-loop */
    } /* end n_loop */

    for (f_act = fo, r = 0; r < dim3; r ++) {
      (void) fprintf(f_act, "\n-1z\n");
      f_act = (r == 0) ? in_2d : in_3d;
    }

  break; /* Output of list  */

  case 'd': /* TABLE DOCUMENTATION */

    (void) fprintf(fo, "\nTABLE DOCUMENTATION\n");

    /* check by label */
    qr      = conv_lab(tab_name, &t_lab_u, "");
    f777    = t_lab->f777;
    lab_ok  = 767 < f777 && f777 <= 777;
    dim3      = (t_lab->lab_type == T3D_LAB) ? 3
              : (t_lab->lab_type == T2D_LAB) ? 2
              : (t_lab->lab_type == TDD_LAB) ? 2 : 1;
    r_size   = abs(t_lab->rec_size);
/* val_mode : unødig/skal repareres */
    val_mode = (r_size == 8) ? 3 : ((t_lab->rec_size > 0) ? 1 : 2);
    /*                    double                        float int */
/* pgsize : unødig/skal repareres */
    pgsize    = (dim3 == 1) ? GEOIDPGSIZE :
                ((dim3 == 2 && val_mode == 3) ? TABDDPGSIZE :
                ((dim3 == 2) ? TAB3DPGSIZE * 2 / 3 : TAB3DPGSIZE));
    geo_def = t_lab->cstm == 0 || t_lab->cstm == -99;
    utm_def = t_lab->cstm  < 0;
    if (geo_def) {
      /*     sx  -> radian */
      lim = 0.01 / RHO;
    }
    if (!lab_ok) {
      (void) sprintf(errtx,
             "\n*** tab_adm_f: label %s ej ok;\n", tab_name);
      reset_dir();
      return(-1);
    }
    (void) conv_lab("OUTPUT", &t_lab_u, fo);

    if (t_lab->local) *in = '\0';
    else (void) strcpy(in, global_dir);
    (void) strcat(in, t_lab->mlb);
    (void) fprintf(fo, "\nFull Path for input: %s\n\n", in);

    chsz = size - head;
    tbwd = chsz/t_lab->n_max/r_size;

    /* consistency check */
    if (tbwd*t_lab->n_max*r_size != chsz) {
      (void) fprintf(fo, "\n*** tab_adm_f: inconsistent file and label");
      (void) fprintf(fo, "\n*     TABLE:  %s\n size = %ld ;\n", t_lab->mlb, size);
      (void) fprintf(fo, "\nfile: %s  size (incl. head): %ld head: %d ",
          t_lab->mlb, size, head);
      (void) fprintf(fo, "\nrows: %d datacols: %d columns %d appar. size: %d\n",
          t_lab->n_max, t_lab->e_max, tbwd, tbwd*t_lab->n_max*r_size + head);

      (void) sprintf(errtx,
          "\n*** tab_adm_f: inconsistent file and label");
      reset_dir();
      return(-1);
    }

    fd    = t_lab->fd;
    g_tpd = t_lab->g_tpd;
    gdd   = t_lab->gd_dec;

    /* check by file head */
    if (!stat(in, &buf)) {

      (void) fprintf(fo, "\n\nCheck by the file");
      size = buf.st_size;
      (void) fprintf(fo,
          "\n  table_file: %s  size = %ld", in, size);

      pos = 0;
      (void) fseek(fd, pos, SEEK_SET);
      if (fread((void *) &res, sizeof(int), 1, fd) - 1) {
        (void) sprintf(errtx,
            "\n*** tab_adm_f: ERROR at input of table-mark");
        reset_dir();
        return(-1);
      }

      if (f777 != res) {
        (void) sprintf(errtx,
            "\n*** tab_adm_f: %s is not a table of correct type;\n",
            t_lab->mlb);
        reset_dir();
        return(-1);
      }

      if (fread((void *) BL, sizeof(BL), 1, fd) - 1) {
        (void) sprintf(errtx,
            "\n*** tab_adm_f: ERROR at input of table-limits");
        reset_dir();
        return(-1);
      }

      if (fread((void *) ezf, sizeof(ezf), 1, fd) - 1) {
        (void) sprintf(errtx,
            "\n*** tab_adm_f: ERROR at input of projection-info");
        reset_dir();
        return(-1);
      }

      (void) fprintf(fo, "\n\n");
      for (r = 0; r < 6; r++) {
        (void) fputg(f_act, *(BL+r), &g_tpd, " +u");
        if (r == 3) {
          (void) fprintf(fo, "\n");
        }
      }

      n_max = (int) ((BL[1] - BL[0])/BL[4] + 1.1);
      e_max = (int) ((BL[3] - BL[2])/BL[5] + 1.1);
      recsz = ((int) size - head) / (n_max * e_max);
      estop = ((e_max + t_lab->rec_p_bl -1)
              / t_lab->rec_p_bl) * t_lab->rec_p_bl;
      sz_u  = head + dim3 * n_max * estop * recsz;
      if (sz_u != size) {
        (void) fprintf(fo,
            "\ntab_adm_f check: file %s inconsistent", in);
        (void) fprintf(fo, "\nAssumed head size %d", head);
        (void) fprintf(fo,
            "\n  rows       %5d   columns    %5d   data cols  %5d",
            n_max, estop, e_max);
        if (dim3 == 2) (void) fprintf(fo, "  2 dim.");
        if (dim3 == 3) (void) fprintf(fo, "  3 dim.");
        (void) fprintf(fo, "\n  record_type  %s  size    %d \n",
            (recsz == 4) ? "float" :
            (recsz == 1) ? "unsigned byte" : "undef", recsz);
        (void) sprintf(errtx,
            "\ntab_adm_f check: file %s inconsistent", in);
        reset_dir();
        return(-1);
      }

      for (e = 0, qr = 0, n = 0; e < 6; e++) {
        diff =  (e == 0) ? t_lab->B_min :
               ((e == 1) ? t_lab->B_max :
               ((e == 2) ? t_lab->L_min :
               ((e == 3) ? t_lab->L_min + t_lab->deltaL :
               ((e == 4) ? t_lab->dB   :
               ((e == 4) ? t_lab->dB    : /*5*/ t_lab->dL))));
        if (fabs(diff - BL[e]) > lim) {
          if (! geo_def) n ++;
          else {
            if ((fabs(diff - BL[e] - 2*M_PI > lim)
             && (fabs(diff - BL[e] + 2*M_PI > lim)) n ++;
          }
          if (n == 1 && qr == 0) (void) fprintf(fo,
                     "\n\n*** LABEL og TABEL ikke konsistente ***\n");
          if (n != qr) {
            (void) fprintf(fo, "BL[%d] =", e);
            (void) fputg(fo, *(BL+e), &g_tpd, " +u");
            (void) fprintf(fo, ", t_lab =");
            (void) fputg(fo, ", diff, &g_tpd, " +u");
            (void) fprintf(fo, ", diff =");
            (void) fputg(fo, diff - BL[e], &g_tpd, " +u");
            (void) fprintf(fo, "\n");
            qr = n;
          }
        }
      }

      chsz = n_max *        e_max  * recsz;
      fill = n_max * (estop-e_max) * recsz;
      (void) fprintf(fo, "\n  %-18s  %8ld bytes", "File", size);
      (void) fprintf(fo, "\n  %-18s  %8d   -  ", "Head", head);
      (void) fprintf(fo,
             "\n  %-18s  %ld   -  ", "Table data", chsz);
      if (dim3 == 2) (void) fprintf(fo, "  2 dim.");
      else
      if (dim3 == 3) (void) fprintf(fo, "  3 dim.");
      (void) fprintf(fo, "\n  %-18s  %8ld   -  ", "Fill-in", fill);
      if (dim3 == 2) (void) fprintf(fo, "  2 dim.");
      else
      if (dim3 == 3) (void) fprintf(fo, "  3 dim.");

      sz_u  = (chsz + fill) * dim3 + head;
      if (size == sz_u) (void) fprintf(fo, "\n\n File OK\n");
      else {
        (void) sprintf(errtx, "\n****tab_adm_f: %s input error ;\n",
            t_lab->mlb);
        reset_dir();
        return(-1);
      }

    }
    else {
      (void) sprintf(errtx, "\nTABLE %s not found\n", t_lab->mlb);
      reset_dir();
      return(-1);

    }

    break; /* Documentation of table */

  default:
    (void) sprintf(errtx, "\n*** tab_adm_f: i/o undefined;");
    reset_dir();
    return(-1);
  } /* mode */

  reset_dir();
  return(0);
}

void reset_dir() {
  int    i;
  if (table) free(table);
  if (fd) fclose(fd);
  if (in_2d) fclose(in_2d);
  if (in_3d) fclose(in_3d);
  c_tabdir_file(0, NULL); // close new GLOBALS
  (void) settabdir(sav_dir);
  (void) i_tabdir_file(1, "", &i, in); // open saved GLOBALS
  return;
}

