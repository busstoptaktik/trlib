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
 

/* set_itrf_c  ver 2009.02      # page 1    12 aug 2009 17 23 */


/* Copyright (c) 2009, National Space Institute, DTU, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of DTU, Denmark */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */

#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <ctype.h>
#include        <fcntl.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include        "geo_lab.h"
#include        "geoid_d.h"
#include        "fgetln_kms.h"
#include        "sgetdt.h"
#include        "s_status.h"
#include        "trthread.h"
extern THREAD_SAFE FILE               *man_gps_file;
extern THREAD_SAFE size_t              init_gps_pos;

int                      set_itrf_c(
/*________________________________*/
struct coord_lab        *from_lb,
struct coord_lab        *to_lb,
double                  *i_JD,
int                      stn_vel,
int                     *o_stn_vel,
struct gps_str          *gps_table,  /* crt_trf parameters */
char                    *tab_t,      /* nnr_plate_vel name */
struct plm_lab          *plm_lb,
struct plate_info       *pl_inf,     /* for plate boundary adm */
char                    *tab_i,      /* intra_plate_vel name */
struct mtab3d_str       *ipl_table,
union geo_lab           *Hipl_lab,
char                    *s_used_plm_nam,
char                    *s_used_ipl_nam,
char                    *err_str
) {
  /* set_itrf_c initializes the transformations constants to i_JD.
  returns: max_seq: number of trans sequences in gps_table         
  *from_lb is the from_coord_mini_label and maybe i_JD
  *to_lb   is the   to_coord_mini_label and o_JD

  Return : set_itrf_c >= 1         : ok.
  ERROR :
  set_itrf_c == TRF_ILLEG_ : illegal National ETRF89 and EUREF89 combi
  set_itrf_c == PLATE_NON_ : itrf_plat.bin file not found.
  set_itrf_c == PLATE_OUT_ : NO entries found.
  set_itrf_c == ITRF_NON_  : NO entries found.
  set_itrf_c == ITRF_SEQ_  : entries are NOT consequtive.
  set_itrf_c == ITRF_DTM_  : Illegal datum in from_lb/to_lb.
  set_itrf_c == ITRF_NAM_  : man_gps_file.gps table_name (gpstab)
                           : not found.
  set_itrf_c == ITRF_SYS_  : man_gps_file.gps file not found.
  */

/* set_itrf_c  ver 2007.02      # page 2    12 aug 2009 17 23 */


  /*
  *i_JD      input coord epoch (may be set)
  stn_vel    stn_velocities given after coordinates

  gps_table->
          seq[0:3]: trf_nr[i] :
  idt_tr  :0:  identical
  itrf_tr :1:  itrfyy -> itrfxx  ( + maybe itrfgate->igs_gate)
  igs__tr :2:  igs_yy -> igs_xx  ( + maybe igs_gate->itrfgate)
  igatetr :3:  itrfyy -> etrfyy or itrfgate -> nkg_euref89
  ogatetr :4:  itrfyy -> etrfyy or itrfgate -> nkg_euref89
  i_ipl_dt  :  ref_JD for intra plate velocity
  o_ipl_dt  :  ref_JD for intra plate velocity

  *tab_t    :: nnr_plate_vel name
  *plm_lb   :: label for tab_t
  *tab_i    :: intra_plate_vel name
  *ipl_table:: table and label for tab_i
  req_plm_tr :: 0: none, +1: requested
  req_ipl_tr :: 0: none, +1: igate, +2: ogate, +3: i & ogate, 4: ipl_req
  plm_yy   :: transformation period
  ipl_yy   :: transformation period
  *s_used_plm_nam :: plm name saved
  *s_used_plm_nam :: ipl name saved
  *err_str  :: error info is placed in this string

  region: (from_lb/to_lb)->rgn  and ETRF89 ::
        = {DK/NO/FI/SE/GR} : National ETRF89
        = EU : the EUREF recommandations are used
       else ITRF PLATE and INTRA PLATE MODELS are used

  Transformation sequence
  I_GATE_TR  ITRF_TR   IGS_TR   ITRF_TR  O_GATE_TR
          0        1        2         3          4

  I_GATE_TR may be ::
            gr96 gate  to ITRF_gate                     i_zz > -1
      ngk_etrf89 gate  to ITRF_gate                  inkg_ii  1
          etrfyy gate  to ITRFyy or ITRF00           ieur_ii  1
           wgs84 gate  to ITRF_gate                     i_zz > -1

  O_GATE_TR may be ::
            gr96 gate  to ITRF_gate                     o_zz > -1
      ngk_etrf89 gate  to ITRF_gate                  onkg_ii  1
          etrfyy gate  to ITRFyy or ITRF00           oeur_ii  1
           wgs84 gate  to ITRF_gate                     o_zz > -1

                        yy <= xx
  ITRF_TR ::
  itrfyy  itrfxx  : add itrf entries from yy to xx  : itrf_ii 1

  IGS_TR ::
  igsyy    igsxx  : add igs  entries from yy to xx  : igs_ii  1

  itrfgate igsgate :                                :   g_zz > -1
  and reverse  . . .

  igsyy/itrfyy -> wgs84 is done to TRANSIT DOPPLER definition
  gr96/etrf89  -> wgs84 is done to itrfmax (latest realisation)
  and reverse  . . .
  */
  

/* set_itrf_c  ver 2007.02      # page 3    12 aug 2009 17 23 */


#define YEAR(yy) ((yy) +(((yy)<88) ? 2000 : (((yy)<100) ? 1900 : 0)))
#include     "c_tabdir_file.h"
#include     "conv_lab.h"
#include     "i_tabdir_file.h"
#include     "tab3d_i.h"
#include     "tab3d_c.h"

long get_tab_part(int type, char *pth_mlb);

int get_tab_item(int req_D, char *pth_mlb, struct gps_c_str *p_gps,
    double i_JD, char *from_str, int fr_ch, char *to___str,
    struct gps_c_str *D_gps, char *err_str);

int get_yy_item(int req_D, char *pth_mlb, struct gps_c_str *p_gps,
     struct gps_c_str *D_gps,
     char *from_str, int fr_ch, int to_ch,
     double *d1, double *d2);

  union geo_lab           *geo_lb;
  struct gde_lab          *ipl_lb = &ipl_table->table_u;
  union rgn_un             i_rgn, o_rgn, rgn_EU;
  char                     from_str[32], to___str[32], plm_nam[MLBLNG];
  char                     ipl_nam[MLBLNG];
  char                    *i_dtm, *o_dtm, pth_mlb[1024], *p_tp;
  long                     pos;
  int                      inkg_ii =0, onkg_ii =0, ipl_std = 0;
  int                      ieur_ii =0, oeur_ii =0, eur_yy =0;
  int                      igs_yy  =0, igs_xx  =0, igs_ii =0;
  int                      itrf_yy =0, itrf_xx =0, itrf_ii=0;
  int                      qr, i_seq = 0;
  int                      i_sys, o_sys, w_is, w_os;
  int                      yy, xx, i, g_zz = -100000; 
  int                      i_zz = -100000, o_zz = -100000;
  double                   dd, ff, run_JD, gate_1d, gate_2d;
  double                   i_plm_dt, o_plm_dt;
  struct gps_c_str        *p_gps, *D_gps = NULL, gate_igs, D_gate_igs;
  struct plate_info       *p_pl_inf;
  int                      dtm_lng[] = {0, 4, 3, 4, 5, 2, 3};
  size_t                   size;

  /* initialize */
  /*____________*/
  (void) strcpy(rgn_EU.prfx, "EU");
  gps_table->seq[0]   = gps_table->seq[1] = 0;
  gps_table->seq[2]   = gps_table->seq[3] = 0;
  gps_table->i_ipl_dt = -10000000.0;
  gps_table->o_ipl_dt = -10000000.0;
  gps_table->plm_dt   = -10000000.0;
  p_gps               = &gps_table->itrf_tr;
  p_gps->tx           = p_gps->ty  = p_gps->tz  = 0.0;
  p_gps->rx           = p_gps->ry  = p_gps->rz  = 0.0;
  p_gps->sc           = 0.0;
  gps_table->igs__tr  = gps_table->itrf_tr;
  gps_table->igat_tr  = gps_table->itrf_tr;
  gps_table->ogat_tr  = gps_table->itrf_tr;
  gps_table->itrf_dt  = gps_table->itrf_tr;
  gps_table->igs__dt  = gps_table->itrf_tr;
  gate_igs            = gps_table->itrf_tr; /* local var */
  D_gate_igs          = gps_table->itrf_tr; /* local var */
  i_rgn.r_nr[0]       = o_rgn.r_nr[0] = 0;
  i_rgn.r_nr[1]       = o_rgn.r_nr[1] = 0;
  i_plm_dt            = *i_JD;
  o_plm_dt            = to_lb->JD;
  for (qr = 16, p_tp = p_gps->datum; qr; qr--) *(p_tp++) = '\0';
  if (stn_vel) stn_vel = 1;


/* set_itrf_c  ver 2007.02      # page 4    12 aug 2009 17 23 */


  i_dtm = from_lb->mlb + from_lb->sepix +1;
  o_dtm =   to_lb->mlb +   to_lb->sepix +1;
  i_sys =  (!strncmp(i_dtm, "itrf", 4))    ? 1
        : ((!strncmp(i_dtm, "igs", 3))     ? 2
        : ((!strncmp(i_dtm, "etrf", 4))    ? 3
        : ((!strncmp(i_dtm, "etrs", 4))    ? 3
        // 3 is national std or ITRFplate/StnVel:: To be changed
        : ((!strncmp(i_dtm, "euref89", 7)) ? 4  // 4 is euref std
        : ((!strncmp(i_dtm, "gr96", 4))    ? 5
        : ((!strncmp(i_dtm, "wgs84", 5))   ? 6 : 9))))));

  o_sys =  (!strncmp(o_dtm, "itrf", 4))    ? 1
        : ((!strncmp(o_dtm, "igs", 3))     ? 2
        : ((!strncmp(o_dtm, "etrf", 4))    ? 3
        : ((!strncmp(i_dtm, "etrs", 4))    ? 3
        : ((!strncmp(o_dtm, "euref89", 7)) ? 4
        : ((!strncmp(o_dtm, "gr96", 4))    ? 5
        : ((!strncmp(o_dtm, "wgs84", 5))   ? 6 : 9))))));
  if (i_sys == 9 || o_sys == 9) return(
                    s_status(err_str, "set_itrf_c", ITRF_DTM_));

  p_tp  = i_dtm + dtm_lng[i_sys];
  (void) sscanf(p_tp, "%d", &yy);
  p_tp  = o_dtm + dtm_lng[o_sys];
  (void) sscanf(p_tp, "%d", &xx);
  if (i_sys == 4) i_sys = 3; // 3 is national std or ITRFplate/StnVel
  if (o_sys == 4) o_sys = 3; // 4 is euref std
  *o_stn_vel = (stn_vel && o_sys <= 2) ? 1 : 0;
  if ((i_sys == 5 && o_sys == 3) || (o_sys == 5 && i_sys == 3))
      return(s_status(err_str, "set_itrf_c", TRF_ILLEG_));


  (void) strcpy(plm_nam, tab_t);
  (void) strcpy(ipl_nam, tab_i);
  i_rgn.r_nr[0] = from_lb->region;
  if (i_sys == 3) {
    if (rgn_EU.r_nr[0] == i_rgn.r_nr[0]) {
      ieur_ii = (int) i_rgn.r_nr[0];
      i_sys   = 4;
      (void) strcpy(plm_nam, "");
      (void) strcpy(ipl_nam, "");
    } else inkg_ii = (int) i_rgn.r_nr[0];
  }
  o_rgn.r_nr[0] = to_lb->region;
  if (o_sys == 3) {
    if (rgn_EU.r_nr[0] == o_rgn.r_nr[0]) {
      oeur_ii = (int) o_rgn.r_nr[0];
      o_sys   = 4;
      (void) strcpy(plm_nam, "");
      (void) strcpy(ipl_nam, "");
    } else onkg_ii = (int) o_rgn.r_nr[0];
  }

  if ((i_sys == 4 && o_sys == 3 && onkg_ii) ||
      (o_sys == 4 && i_sys == 3 && inkg_ii) ||
      (onkg_ii && inkg_ii))
      return(s_status(err_str, "set_itrf_c", TRF_ILLEG_));

  if (inkg_ii || onkg_ii) {
    if (*plm_nam == '\0' || (!strcmp(plm_nam, "std"))
      || (!strcmp(plm_nam, "nnr_std")))
                 (void) strcpy(plm_nam, "nnr_itrf00");
    if (*ipl_nam == '\0'  || (!strcmp(ipl_nam, "std")))
         (void) strcpy(ipl_nam, "nkgrf03vel.01");
  } else
  if (i_sys==5 || o_sys==5) {
    if (*plm_nam == '\0'|| (!strcmp(plm_nam, "nnr_std")))
         (void) strcpy(plm_nam, "nnr_std");
    if (!strcmp(ipl_nam, "std") || *ipl_nam != '\0') {
      (void) strcpy(ipl_nam, "");
      (void) strcpy(s_used_ipl_nam, "out of IPL");
    }
  } else {
    if (!strcmp(plm_nam, "std")) (void) strcpy(plm_nam, "nnr_std");
    if (!strcmp(ipl_nam, "std"))
               (void) strcpy(ipl_nam, "nkgrf03vel.01");
  }
  

/* set_itrf_c  ver 2007.02      # page 5    12 aug 2009 17 23 */


  if (man_gps_file == NULL || init_gps_pos == 0) {
    (void) i_tabdir_file(2, "", &i, pth_mlb);
    if (i) {
      i = (i == -1) ? ITRF_SYS_ : ITRF_NAM_;
      return (i);
    }
  }
  (void) fseek(man_gps_file, (long) init_gps_pos, SEEK_SET);

  /* get GATES */
  pos = get_tab_part(878, pth_mlb);

  if (pos != 0) {
    p_gps = &(gps_table->igat_tr);
    if (inkg_ii) {
      (void) sprintf(from_str, "%2s_etrf89", i_rgn.prfx);
      (void) strcpy(to___str, "itrf");
      i_zz    = get_yy_item(0, pth_mlb, p_gps, D_gps,
                            from_str, 9, 4,
                            &gps_table->i_ipl_dt, &gps_table->plm_dt);
      ipl_std = 1;
/*
(void) fprintf(stdout, "\n*%s_%s: %5.3f years;", from_str, to___str,
gps_table->plm_dt - gps_table->i_ipl_dt);
*/
    } else
    if (i_sys == 5) {
      (void) strcpy(from_str, "GR_gr96");
      (void) strcpy(to___str, "itrf");
      i_zz = get_yy_item(0, pth_mlb, p_gps, D_gps,
                         from_str, 7, 4,
                         &gps_table->i_ipl_dt, &gps_table->plm_dt);
/*
(void) fprintf(stdout, "\n*%s_%s: %5.3f years;", from_str, to___str,
gps_table->plm_dt - gps_table->i_ipl_dt);
*/
    } else
    if (i_sys == 6) {
      (void) strcpy(from_str, "wgs84");
      (void) strcpy(to___str, "itrf");
      i_zz = get_yy_item(0, pth_mlb, p_gps, D_gps,
                         from_str, 5, 4,
                         &gps_table->i_ipl_dt, &gps_table->plm_dt);
/*
(void) fprintf(stdout, "\n*%s_%s: %5.3f years;", from_str, to___str,
gps_table->plm_dt - gps_table->i_ipl_dt);
*/
    }


/* set_itrf_c  ver 2007.02      # page 6    12 aug 2009 17 23 */


    if (i_zz >= 0) {
      gps_table->seq[i_seq ++] = 3;  /* IGAT */
      yy        = i_zz;
      if (i_sys != 6) i_plm_dt = gps_table->plm_dt;
      p_gps->tx = -p_gps->tx;
      p_gps->ty = -p_gps->ty;
      p_gps->tz = -p_gps->tz;
      p_gps->sc = -p_gps->sc;
      p_gps->rx = -p_gps->rx;
      p_gps->ry = -p_gps->ry;
      p_gps->rz = -p_gps->rz;
    }

    (void) fseek(man_gps_file, pos, SEEK_SET);
    if ((i_sys == 2 && o_sys != 2) || (i_sys != 2 && o_sys == 2)) {
      (void) strcpy(from_str, "igs");
      (void) strcpy(to___str, "itrf");
      /* ..item(2, :: Time_diff not set yet:: stack gate and D_gate */
      g_zz = get_yy_item(2, pth_mlb, &(gate_igs), &D_gate_igs,
             from_str, 3, 4, &gate_1d, &gate_2d);
      (void) fseek(man_gps_file, pos, SEEK_SET);
    }

    if (onkg_ii) {
      (void) sprintf(from_str, "%2s_etrf89", o_rgn.prfx);
      (void) strcpy(to___str, "itrf");
      o_zz     = get_yy_item(0, pth_mlb, &(gps_table->ogat_tr),
                             D_gps, from_str, 9, 4,
                             &gps_table->o_ipl_dt, &gps_table->plm_dt);
      ipl_std += 2;
/*
(void) fprintf(stdout, "\n*%s_%s: %5.3f years;", from_str, to___str,
gps_table->plm_dt - gps_table->o_ipl_dt);
*/
    } else
    if (o_sys == 5) {
      (void) strcpy(from_str, "GR_gr96");
      (void) strcpy(to___str, "itrf");
      o_zz = get_yy_item(0, pth_mlb, &(gps_table->ogat_tr), D_gps,
                         from_str, 7, 4,
                         &gps_table->o_ipl_dt, &gps_table->plm_dt);
/*
(void) fprintf(stdout, "\n*%s_%s: %5.3f years;", from_str, to___str,
gps_table->plm_dt - gps_table->o_ipl_dt);
*/
    } else
    if (o_sys == 6) {
      (void) strcpy(from_str, "wgs84");
      (void) strcpy(to___str, "itrf");
      o_zz = get_yy_item(0, pth_mlb, &(gps_table->ogat_tr), D_gps,
                         from_str, 5, 4,
                         &gps_table->o_ipl_dt, &gps_table->plm_dt);
/*
(void) fprintf(stdout, "\n*%s_%s: %5.3f years;", from_str, to___str,
gps_table->plm_dt - gps_table->o_ipl_dt);
*/
    }
    if (o_zz >= 0) {
      xx = o_zz;
      if (o_sys != 6) o_plm_dt = gps_table->plm_dt;
    }
    if (i_zz < -100000 || o_zz < -100000 || g_zz < -100000)
       return(s_status(err_str, "set_itrf_c", ITRF_NON_));
  }
  else return(s_status(err_str, "set_itrf_c", ITRF_NAM_));


/* set_itrf_c  ver 2007.02      # page 7    12 aug 2009 17 23 */


  w_is = (i_zz >= 0) ? 1 : i_sys; // GATE same as ITRF
  w_os = (o_zz >= 0) ? 1 : o_sys; // GATE same as ITRF

  switch (w_is) {
  case 1:    /* itrf and i_gate_itrf */
  case 3:    /* etrf NOT recommended: TEST other models */
    itrf_yy = yy;
    if (w_os == 4) { /* etrf89 EUREF recommandation */
      itrf_xx = yy;
      eur_yy  = yy;
      if (YEAR(yy) > 2000) { /* Special for itrf2005 */
        itrf_xx                  = 0;
        eur_yy                   = 0;
        itrf_ii                  = 1;
        gps_table->seq[i_seq ++] = 1;  /* ITRF */
      }
      gps_table->seq[i_seq ++] = 4;  /* OGAT */
    } else {
      itrf_xx = ((w_os) != 2) ? xx : g_zz;
      if (itrf_yy != itrf_xx) {
        itrf_ii                  = 1;
        gps_table->seq[i_seq ++] = 1;  /* ITRF */
      }
      if (w_os == 2) { /* igs */
        igs_yy = g_zz;
        igs_xx = xx;
        if (igs_yy != igs_xx || g_zz >= 0) {
          igs_ii                   = 1;
          gps_table->seq[i_seq ++] = 2;  /* IGST */
        }
        g_zz = -g_zz;
      }
    }
    break;

  case 2:    /* igs */
    igs_yy = yy;
    igs_xx = ((w_os) == 2) ? xx : g_zz;
    if (igs_yy != igs_xx || g_zz >= 0) {
      igs_ii                   = +1;
      gps_table->seq[i_seq ++] = 2;  /* IGST */
    }
    if (w_os != 2) { /* to itrf */
      itrf_yy = g_zz;
      if (w_os == 4) { /* etrf89 EUREF recommandation */
        itrf_xx = g_zz;
        eur_yy  = g_zz;
        if (YEAR(g_zz) > 2000) {
          itrf_xx                  = 0;
          eur_yy                   = 0;
          itrf_ii                  = 1;
          gps_table->seq[i_seq ++] = 1;  /* ITRF */
        }
        gps_table->seq[i_seq ++] = 4;  /* OGAT */
      } else { 
        itrf_xx = xx;
        if (itrf_yy !=  itrf_xx) {
          itrf_ii                  = +1;
          gps_table->seq[i_seq ++] = 1;  /* ITRF */
        }
      }
    }
    break;


/* set_itrf_c  ver 2007.02      # page 8    12 aug 2009 17 23 */


  case 4:   /* etrf89 EUREF recommandation */
    if (w_os <= 2) { /* itrf */
      eur_yy  = xx;
      itrf_yy = xx;
      itrf_xx = ((w_os) == 1) ? xx : g_zz;
      gps_table->seq[i_seq ++] = 3;  /* IGAT */
      if (YEAR(itrf_xx) > 2000) {
        itrf_yy                  = 0;
        eur_yy                   = 0;
        itrf_ii                  = +1;
        gps_table->seq[i_seq ++] = 1;  /* ITRF */
      }
      if (w_os == 2) { /* igs */
        igs_yy = g_zz;
        igs_xx = xx;
        if (igs_yy != igs_xx || g_zz >= 0) {
          igs_ii                   = +1;
          gps_table->seq[i_seq ++] = 2;  /* IGST */
        }
        g_zz = -g_zz;
      }
    } else return(1);
    break;
  }
 
  if (igs_ii) {
    if (YEAR(igs_xx) < YEAR(igs_yy)) {
      xx      = igs_xx;
      igs_xx  = igs_yy;
      igs_yy  = xx;
      igs_ii  = -1;
      run_JD  = o_plm_dt;
    } 
    else run_JD = i_plm_dt;

    /* get IGS */
    (void) sprintf(from_str, "igs%02d", igs_yy);
    (void) sprintf(to___str, "igs%02d", igs_xx);
    p_gps = &(gps_table->igs__tr);
    D_gps = &(gps_table->igs__dt);
    if (igs_yy != igs_xx) {
      pos  = get_tab_part(877, pth_mlb);
      if (pos == 0) return(s_status(err_str, "set_itrf_c", ITRF_NAM_));
      qr   = get_tab_item(1, pth_mlb, p_gps, run_JD,
                          from_str, 5, to___str, D_gps, err_str);
      if (qr < 0) return(ITRF_DTM_);
/*
(void) fprintf(stdout,
"\n*%s_%s: %5.3f year - <ref_year>;", from_str, to___str,
run_JD / 365.25);
*/
      if (igs_ii < 0) {
        p_gps->tx = -p_gps->tx;
        p_gps->ty = -p_gps->ty;
        p_gps->tz = -p_gps->tz;
        p_gps->sc = -p_gps->sc;
        p_gps->rx = -p_gps->rx;
        p_gps->ry = -p_gps->ry;
        p_gps->rz = -p_gps->rz;
        D_gps->tx = -D_gps->tx;
        D_gps->ty = -D_gps->ty;
        D_gps->tz = -D_gps->tz;
        D_gps->sc = -D_gps->sc;
        D_gps->rx = -D_gps->rx;
        D_gps->ry = -D_gps->ry;
        D_gps->rz = -D_gps->rz;
      }
    }
    if (g_zz > -10000) { /* IGS += IGS_gate_to_ITRF */
      /* 365.25 ONLY VALID for tr_dd < 100 years */
      run_JD     = (g_zz >= 0) ? i_plm_dt : o_plm_dt;
      ff         = (g_zz >= 0) ? 1.0 : -1.0;
      dd         = (run_JD -gate_1d) / 365.25;
      p_gps->tx += (gate_igs.tx + D_gate_igs.tx * dd) * ff;
      p_gps->ty += (gate_igs.ty + D_gate_igs.ty * dd) * ff;
      p_gps->tz += (gate_igs.tz + D_gate_igs.tz * dd) * ff;
      p_gps->sc += (gate_igs.sc + D_gate_igs.sc * dd) * ff;
      p_gps->rx += (gate_igs.rx + D_gate_igs.rx * dd) * ff;
      p_gps->ry += (gate_igs.ry + D_gate_igs.ry * dd) * ff;
      p_gps->rz += (gate_igs.rz + D_gate_igs.rz * dd) * ff;
      D_gps->tx += D_gate_igs.tx * ff;
      D_gps->ty += D_gate_igs.ty * ff;
      D_gps->tz += D_gate_igs.tz * ff;
      D_gps->sc += D_gate_igs.sc * ff;
      D_gps->rx += D_gate_igs.rx * ff;
      D_gps->ry += D_gate_igs.ry * ff;
      D_gps->rz += D_gate_igs.rz * ff;
/*
(void) fprintf(stdout, "\n*igs_itrf: %5.3f years;", from_str, to___str,
dd);
*/
    }
  }


/* set_itrf_c  ver 2007.02      # page 9    12 aug 2009 17 23 */


  if (itrf_ii) {
    if (YEAR(itrf_xx) < YEAR(itrf_yy)) {
      xx      = itrf_xx;
      itrf_xx = itrf_yy;
      itrf_yy = xx;
      itrf_ii = -1;
      run_JD  = (oeur_ii) ? i_plm_dt : o_plm_dt;
    }
    else run_JD = (ieur_ii) ? o_plm_dt : i_plm_dt;
    /* get ITRF */
    (void) sprintf(from_str, "itrf%02d", itrf_yy);
    (void) sprintf(to___str, "itrf%02d", itrf_xx);
    p_gps = &gps_table->itrf_tr;
    pos   = get_tab_part(876, pth_mlb);
    if (pos == 0) return(s_status(err_str, "set_itrf_c", ITRF_NAM_));
    qr    = get_tab_item(1, pth_mlb, p_gps, run_JD,
    /* dummy D_gate.. */ from_str, 6, to___str, &D_gate_igs, err_str);
    if (qr < 0) return(ITRF_DTM_);
/*
(void) fprintf(stdout, "\n*%s_%s: %5.3f year - <ref_year>;",
from_str, to___str, run_JD / 365.25);
*/
    if (itrf_ii < 0) {
      p_gps->tx = -p_gps->tx;
      p_gps->ty = -p_gps->ty;
      p_gps->tz = -p_gps->tz;
      p_gps->sc = -p_gps->sc;
      p_gps->rx = -p_gps->rx;
      p_gps->ry = -p_gps->ry;
      p_gps->rz = -p_gps->rz;
    }
  }

  if (ieur_ii || oeur_ii) {
    run_JD = (ieur_ii) ? o_plm_dt : i_plm_dt;
    (void) sprintf(from_str, "itrf%02d", eur_yy);
    (void) sprintf(to___str, "etrf89");
    p_gps = (ieur_ii) ? &gps_table->igat_tr
          : &gps_table->ogat_tr;
    pos   = get_tab_part(875, pth_mlb);
    if (pos == 0) return(s_status(err_str, "set_itrf_c", ITRF_NAM_));
    qr    = get_tab_item(1-stn_vel, pth_mlb, p_gps, run_JD,
    /* dummy D_gate.. */ from_str, 6, to___str, &D_gate_igs, err_str);
    if (qr < 0) return(ITRF_DTM_);
/*
(void) fprintf(stdout, "\n*%s_%s: %5.3f year - <ref_year>;",
from_str, to___str, run_JD / 365.25);
*/
    if (ieur_ii) {
      p_gps->tx = -p_gps->tx;
      p_gps->ty = -p_gps->ty;
      p_gps->tz = -p_gps->tz;
      p_gps->sc = -p_gps->sc;
      p_gps->rx = -p_gps->rx;
      p_gps->ry = -p_gps->ry;
      p_gps->rz = -p_gps->rz;
    }
  }
  else if (o_zz >= 0) gps_table->seq[i_seq ++] = 4;  /* OGAT */

  gps_table->seq[i_seq] = 0;
  (void) c_tabdir_file(1, man_gps_file);


/* set_itrf_c  ver 2007.02      # page 10   12 aug 2009 17 23 */


  if (stn_vel) {
    gps_table->ipl_yy     = 0.0;
    gps_table->plm_yy     = o_plm_dt - i_plm_dt;
    gps_table->req_ipl_tr = 0;
    gps_table->req_plm_tr = 0;
  } else
  if (ipl_std & 1) {
    gps_table->ipl_yy     = ((ipl_std & 2) ? gps_table->o_ipl_dt
                          : o_plm_dt) - gps_table->i_ipl_dt;
    gps_table->plm_yy     = (ipl_std & 2) ? 0.0
                          : (o_plm_dt - gps_table->plm_dt);
    gps_table->req_ipl_tr = ipl_std;
    gps_table->req_plm_tr = (gps_table->plm_yy != 0.0);
  } else
  if (ipl_std & 2) {
    gps_table->ipl_yy     = gps_table->o_ipl_dt - i_plm_dt;
    gps_table->plm_yy     = gps_table->plm_dt - i_plm_dt;
    gps_table->req_ipl_tr = ipl_std;
    gps_table->req_plm_tr = (*plm_nam != '\0')
                          && (gps_table->plm_yy != 0.0);
  } else {
    gps_table->ipl_yy     = o_plm_dt - i_plm_dt;
    gps_table->plm_yy     = gps_table->ipl_yy;
    gps_table->req_ipl_tr = ((*ipl_nam != '\0')
                          && (gps_table->ipl_yy != 0.0)) ? 4 : 0;
    gps_table->req_plm_tr = (*plm_nam != '\0')
                          && (gps_table->plm_yy != 0.0);
  }
  /* 365.25 ONLY VALID for tr_yy < 100 years */
  gps_table->ipl_yy /= 365.25;
  gps_table->plm_yy /= 365.25;

  if (gps_table->req_plm_tr && strcmp(plm_nam, s_used_plm_nam)) {
    if (*plm_nam != '\0') {
      (void) strcpy(from_str, plm_nam);
      geo_lb = (union geo_lab *) plm_lb;
      i      = conv_lab(from_str, geo_lb, "");
      if (i == PLM_LAB) {
        (void) strcpy(plm_nam, plm_lb->mlb);
        if (plm_lb->f_poly == NULL) {
          plm_lb->f_poly =
                  i_tabdir_file(4, "itrf_plat.bin", &i, pth_mlb);
          if (i) {
            return(s_status(err_str,
                   "set_itrf_c f_poly", PLATE_NON_));
          }

          *err_str = '\0';
          p_pl_inf = pl_inf;
          size     = sizeof(struct plate_info);
          do {
            /* read plates info */
            if ((i = (int) fread((void *) p_pl_inf,
                                  size, 1, plm_lb->f_poly))-1) {
              (void) sprintf(err_str,
                "\n*** pla_adm_f: ERROR at f_poly read of info");
              (void) sprintf(err_str,
                "\n plate_info_adr %8ld nmb = %4d size = %4d i = %d;",
                ftell(plm_lb->f_poly), p_pl_inf - pl_inf, size, i);
              return(s_status(err_str,
                              "set_itrf_c f_poly", PLATE_OUT_));
            }
            ++ p_pl_inf;
          } while ((p_pl_inf -1)->plate_nr > 0);
        }


/* set_itrf_c  ver 2007.02      # page 11   12 aug 2009 17 23 */

      } else {
        *s_used_plm_nam = '\0';
        (void) sprintf(err_str, "\nunknown plate model: %s", plm_nam);
        return(-1);
      }
    }
  }

  if (gps_table->req_ipl_tr && strcmp(ipl_nam, s_used_ipl_nam)) {
    if (ipl_table->init == 1) (void) tab3d_c(ipl_table);

    i = tab3d_i(ipl_nam, T3D_LAB, ipl_table, err_str);
    if (i > 0) (void) strcpy(s_used_ipl_nam, ipl_nam);
    else
    if (i < 0) {
      (void) sprintf(err_str, "\nunknown intra plate: %s", ipl_nam);
      return(i);
    }
    (void) conv_lab(ipl_lb->clb, Hipl_lab, "  20000101");
  }
  return(i_seq);
#undef    YEAR
}


long get_tab_part(int type, char *pth_mlb)
{
#include        "fgetlhtx.h"
  int     stype = STP_LAB, qr, used;
  long    pos;
  size_t  size;

  do {
    qr    = fgetlhtx(man_gps_file, pth_mlb);
    pos   = ftell(man_gps_file) - (long) strlen(pth_mlb);
    (void) fseek(man_gps_file, pos, SEEK_SET);
    /* fgetln_kms gives items with 2<sp> between them */
    if (qr != EOF) qr = fgetln_kms(pth_mlb, &used, man_gps_file);
    if (qr != EOF) {
      switch (type) {
      case 878: /* LINKTAB */
        qr = !strncmp(pth_mlb, "linktab  manager  878", 21); break;
      case 877: /* IGSTAB */
        qr = !strncmp(pth_mlb, "igstab  manager  877", 20); break;
      case 876: /* IGSTAB */
        qr = !strncmp(pth_mlb, "itrftab  manager  876", 21); break;
      case 875: /* IGSTAB */
        qr = !strncmp(pth_mlb, "etrftab  manager  875", 21); break;
      }
      if (qr) {
        do {
          qr   = fgetln_kms(pth_mlb, &used, man_gps_file);
          size = (size_t) !strncmp(pth_mlb, "REC_TABLE", 9);
        } while (qr > 0 && (size == 0));
        pos   = (size) ? ftell(man_gps_file) : 0;
        stype = 0;
      } else {
        if (!strncmp(pth_mlb, "stop  manager  871", 18)) {
          pos   = 0;
          stype = 0;
        }
      }
    }
  } while (stype);
  return(pos);
}


/* set_itrf_c  ver 2007.02      # page 12   12 aug 2009 17 23 */


void get_molodensky(int req_D, double dd,
     char *pth_mlb, char *p_tp,
     struct gps_c_str *p_gps, struct gps_c_str *D_gps)
{
#include        "sgetg.h"
  int               qr, used;
  double            Dtx, Dty, Dtz, Dsc, Drx, Dry, Drz;
  struct typ_dec    tpd;

  /* read translations */
  p_gps->tx += sgetg(p_tp, &tpd, &used, "cm");
  p_tp      += used;
  p_gps->ty += sgetg(p_tp, &tpd, &used, "cm");
  p_tp      += used;
  p_gps->tz += sgetg(p_tp, &tpd, &used, "cm");
  p_tp      += used;

  /* read scale */
  p_gps->sc += sgetg(p_tp, &tpd, &used, "ppb");
  p_tp      += used;

  /* read rotations mas = sx/1000 */
  p_gps->rx += sgetg(p_tp, &tpd, &used, "mas");
  p_tp      += used;
  p_gps->ry += sgetg(p_tp, &tpd, &used, "mas");
  p_tp      += used;
  p_gps->rz += sgetg(p_tp, &tpd, &used, "mas");

  /* read Dtranslations */
  p_tp        = pth_mlb;
  (void) fgetln_kms(pth_mlb, &qr, man_gps_file);


/* set_itrf_c  ver 2007.02      # page 13   12 aug 2009 17 23 */


  if (req_D >= 1) { /* get velocities */
    /* transform ITRFzz to run_JD */
    Dtx   = sgetg(p_tp, &tpd, &used, "cm");
    p_tp += used;
    Dty   = sgetg(p_tp, &tpd, &used, "cm");
    p_tp += used;
    Dtz   = sgetg(p_tp, &tpd, &used, "cm");
    p_tp += used;

    /* read Dscale */
    Dsc   = sgetg(p_tp, &tpd, &used, "ppb");
    p_tp += used;

    /* read Drotations mas = sx/1000 */
    Drx   = sgetg(p_tp, &tpd, &used, "mas");
    p_tp += used;
    Dry   = sgetg(p_tp, &tpd, &used, "mas");
    p_tp += used;
    Drz   = sgetg(p_tp, &tpd, &used, "mas");
    /* add D_ITRFzz */
    D_gps->tx += Dtx;
    D_gps->ty += Dty;
    D_gps->tz += Dtz;
    D_gps->sc += Dsc;
    D_gps->rx += Drx;
    D_gps->ry += Dry;
    D_gps->rz += Drz;
    if (req_D == 1) {
      /* transform ITRFzz to run_JD */
      p_gps->tx += Dtx * dd;
      p_gps->ty += Dty * dd;
      p_gps->tz += Dtz * dd;
      p_gps->sc += Dsc * dd;
      p_gps->rx += Drx * dd;
      p_gps->ry += Dry * dd;
      p_gps->rz += Drz * dd;
    } /* req_D = 2 : dd not yet determined */
  } /* get velocities */
  return;
}



/* set_itrf_c  ver 2007.02      # page 14   12 aug 2009 17 23 */


int get_tab_item(int req_D, char *pth_mlb, struct gps_c_str *p_gps,
    double i_JD, char *from_str, int fr_ch, char *to___str,
    struct gps_c_str *D_gps, char *err_str)
{
  char   *p_tp, next[10];
  int     qr, ant, used;
  double  dd, ref_epoch;
  size_t  size, nsz;

  nsz  = 1;
  ant  = 0;
  size = 0;
  do { /* look for: from_str */
    qr  = fgetln_kms(pth_mlb, &qr, man_gps_file);
    if (ant || (!strncmp(pth_mlb, from_str, fr_ch))) {
      p_tp  = pth_mlb + fr_ch +1;
      if (ant == 0) {
        size = 1;
        (void) sprintf(p_gps->datum, "%s_%s", from_str, to___str);
      }
      else size = strncmp(pth_mlb, "stop", 4);
      if (size) { /* get until to___str */
        nsz = (size_t) strncmp(p_tp, to___str, fr_ch);
        if (ant) { /* test propper sequence */
          if (strncmp(pth_mlb, next, fr_ch))
             return(s_status(err_str, "set_itrf_c", ITRF_SEQ_));
        }
        (void) strcpy(next, p_tp);
      }

      if (size) {
        (void) fgetln_kms(pth_mlb, &used, man_gps_file);
        /* skip dates and 1 textline */
        (void) fgetln_kms(pth_mlb, &qr, man_gps_file);
        p_tp  = pth_mlb; /* read ref_epoch */
        (void) fgetln_kms(pth_mlb, &qr, man_gps_file);
        (void) sgetdt(p_tp, &ref_epoch, &used, &qr);
        p_tp += used;
        /* 365.25 ONLY VALID for dd < 100 years */
        dd    = (i_JD - ref_epoch) / 365.25;
        get_molodensky(req_D, dd, pth_mlb, p_tp, p_gps, D_gps);
        ++ ant;
        size = nsz; /* mark for last entry */
      } /* size */
    } /* first ITRF test */
    else {
      (void) fgetln_kms(pth_mlb, &used, man_gps_file);
      (void) fgetln_kms(pth_mlb, &used, man_gps_file);
      (void) fgetln_kms(pth_mlb, &used, man_gps_file);
      (void) fgetln_kms(pth_mlb, &used, man_gps_file);
    } /* skip lines of entry */
  } while (nsz && strncmp(pth_mlb, "stop", 4));
  return((ant == 0 || nsz) ? -100 : ant);
}


/* set_itrf_c  ver 2007.02      # page 15   12 aug 2009 17 23 */


int get_yy_item(int req_D, char *pth_mlb, struct gps_c_str *p_gps,
     struct gps_c_str *D_gps,
     char *from_str, int fr_ch, int to_ch,
     double *d1, double *d2)
{
  char   *p_tp, nsz = 1;
  int     qr, used, yy=-1000000;

  do { /* look for "from_str" */
    qr = fgetln_kms(pth_mlb, &qr, man_gps_file);
    if (!strncmp(pth_mlb, from_str, fr_ch)) {
      nsz   = 0;
      qr    = ((req_D != 2) ? 3 : 5) + fr_ch + to_ch;
      p_tp  = pth_mlb + qr -2;
      (void) strncpy(p_gps->datum, pth_mlb, qr);
      *(p_gps->datum + qr) = '\0';
      /* get yy */
      (void) sscanf(p_tp, "%d", &yy);
      (void) fgetln_kms(pth_mlb, &used, man_gps_file);
      /* get realisation_epoch == ipl_gate_epoch */
      p_tp = strchr(pth_mlb, ','); /* after , */
      if (p_tp != NULL) (void) sgetdt(p_tp+1, d1, &used, &qr);

      /* skip for 1 textline */
      (void) fgetln_kms(pth_mlb, &qr, man_gps_file);

      /* read plm_gate_epoch */
      p_tp  = pth_mlb;
      (void) fgetln_kms(pth_mlb, &qr, man_gps_file);
      (void) sgetdt(p_tp, d2, &used, &qr);
      p_tp += used;

      get_molodensky(req_D, 1.0, pth_mlb, p_tp, p_gps, D_gps);

    } /* first ITRF test */
    else {
      (void) fgetln_kms(pth_mlb, &used, man_gps_file);
      (void) fgetln_kms(pth_mlb, &used, man_gps_file);
      (void) fgetln_kms(pth_mlb, &used, man_gps_file);
      (void) fgetln_kms(pth_mlb, &used, man_gps_file);
    } /* skip lines of entry */

  } while (nsz && strncmp(pth_mlb, "stop", 4));

  return((nsz) ? -1000000 : yy);
}
