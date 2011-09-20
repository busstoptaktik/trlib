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
 

/* set_dtm     ver 2010.1          # page 1   12 jan 2010 10 19 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

/* Prog: KP OCT 1990                                           */
/* Modified, KP MAY 1993                                       */
/* Modified, KE JUL 2007                                       */
/* Modified, KE JAN 2010                                       */

#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <sys/types.h>
#include    <sys/stat.h>
#include    <ctype.h>
#include    <fcntl.h>
#include    <math.h>
#include    "geo_lab.h"
#include    "trthread.h"

int             set_dtm(
/*____________________*/
short               d_no,
char               *d_nm,
short              *p_no,
char               *p_nm,
char               *e_nm,
struct dsh_str     *trp
)

{
#include              "fgetln.h"
#include              "fgetlhtx.h"
#include              "i_tabdir_file.h"
#include              "sgetg.h"

#define  MAS  (RHO*1000.0)

  /* DATUM SHIFT TYPES, See geo_lab.h */
  /* DATUM NAMES */
  /* central block */
  /*     LOKAL    0     lokalt  */
#define  W84GS    (1)  /* wgs84   */
#define  E50DT    (2)  /* ed50    */
#define  N9WLD    (3)  /* nwl9d   */
#define  E89RF    (4)  /* euref89 = ITRF92 timetrf -> ERTF89 */
#define  GR96     (5)  /* GR96    = ITRF96 */

  /* igs and itrf datums */
#define  IGSyy    (6)  /* IGSyy   for GPS only */
#define  ITRFyy   (7)  /* ITRFyy  NOT for GPS */
#define  ETRFyy   (8)  /* ETRFyy              */


/* set_dtm     ver 2010.1          # page 2   12 jan 2010 10 19 */


  /* adjacent datums */
#define  ETOPEX   (9)  /* TOPEX datum */
#define  E87DT   (10)  /* ed87    */
#define  W72GS   (11)  /* wgs72   */
#define  E79DT   (12)  /* ed79    */
#define  F54DT   (13)  /* fd54    */
#define  QORNK   (14)  /* qornoq  */
#define  N83AD   (15)  /* nad83   */
#define  N27AD   (16)  /* nad27   */
#define  N27AC   (17)  /* nad27c  */
#define  P42UL   (18)  /* pulkovo */
#define  ISLND   (20)  /* island  */
#define  DHDNT   (21)  /* dhdn    */
#define  SCOSD   (22)  /* scosd   */

#define  AMMLK   (23)  /* ammlk   */
#define  RT90G   (24)  /* rt90    */
#define  SRT90G  (25)  /* rt90    */
#define  INdia   (30)  /* india   */
#define  M50SK   (31)  /* mærsk_ed50    */
#define  EEU89   (51)  /* eesti_euref89 */
#define  N83GD   (62)  /* nad83g  */
#define  EXTRA   (63)  /* test    */

  /* implicit datums */
#define  MIN_IMPL_CDTM   (128)
#define  SJ34D  (129)  /* s34j    */
#define  SS34D  (130)  /* s34s    */
#define  SB45D  (131)  /* s45b    */
#define  GSDTM  (132)  /* gs      */
#define  GSBDT  (133)  /* gsb     */
#define  OSDTM  (134)  /* os      */
#define  KKDTM  (135)  /* kk      */
#define  SBDTM  (136)  /* sb      */
#define  RT90V  (137)  /* rt90v   */
#define  RT38V  (138)  /* rt38v   */
#define  RT38G  (139)  /* rt38g   */
#define  D90KS  (140)  /* dks     */
#define  E42ES  (141)  /* eesti42 */
#define  FD54G  (142)  /* fg54    */
#define  FD50G  (143)  /* fg50    */
#define  F54LM  (144)  /* fk54    */
#define  F77UT  (145)  /* fu50    topografisk ed50 forløbig */
#define  F89LM  (146)  /* fk89    */
#define  EOLES  (147)  /* eeold42 */
#define  E37ES  (148)  /* eeepv37 */
#define  A90SB  (149)  /* asb     */
#define  NAH67  (150)  /* Qatar Nahwan 1967 */
#define  QNG24  (151)  /* Qatar National Gridsystem 1924 */
#define  QBC01  (152)  /* Qatar Basic Coordinate System 2001 */
                       /* QBC01 COWI DEF */
#define  QND95  (153)  /* Qatar National Datum 1995 */

#define  MAX_IMPL_CDTM   (153)


/* set_dtm     ver 2010.1          # page 3   12 jan 2010 10 19 */


  /* implicit datums Height */
#define  HLOKD  (201)  /* Local Reference Level */
#define  HMSLD  (202)  /* Mean Sea Level of geoid dtm */
#define  GM91D  (203)  /* G.M. 1891 */
#define  GI44D  (204)  /* G.I. 1944 */
#define  KN44D  (205)  /* Kbh.Nul 1944 */
#define  GI50D  (206)  /* G.I. 1950 */
#define  KMSD   (207)  /* KMS  1990 MIDLERTIDIGT REGNENIVEAU */
#define  DVRD   (208)  /* DVR90  */
#define  DNND   (209)  /* DNN: gm91/gi44/h_msl(dnn50)  */
#define  EVRF00D (210) /* European Vertical Reference Frame 2000 */
  /* EESTONIA datums Height */
#define  EE40D  (291)  /* ESTONIA 1940 */
#define  EE48D  (292)  /* ESTONIA 1948 */
#define  EE77D  (293)  /* ESTONIA 1977 */

  /* implicit datums Gravity */
#define  IGSN   (301)  /* International Gravity System */
#define  GABS   (302)  /* Absolut Gravity System */
#define  GPOT   (303)  /* Potsdam Gravity System */

  /* DATUM SHIFT IS PERFORMED from datum to p_datum : +ST7PA = +7 */
  /* TO = scale * ROTZ * ROTY * ROTX * FROM + TRANSLA */
  /*      ( cz sz 0)       (cy 0 -sy)       (1   0  0) */
  /* ROTZ=(-sz cz 0), ROTY=(0  1   0), ROTX=(0  cx sx) */
  /*      (  0  0 1)       (sy 0  cy)       (0 -sx cx) */
  /* small rotations appr. ==                          */
  /*                      ( 1    rz  -ry)              */
  /* ROTZ * ROTY * ROTX = (-ry   1    rx)              */
  /*                      ( ry  -rx   1 )              */

  /* DATUM SHIFT IS PERFORMED from datum to p_datum : -ST7PA = -7 */
  /* TO = ROTX^T * ROTY^T * ROTZ^T * (FROM - TRANSLA) / scale */

  /* INVERSE DATUM SHIFT IS PERFORMED from p_datum to datum : -type */

  /* datum parameters are read from ~geoid/def_dtm.txt : */
  /* datum                                    */
  /* no        datum     p_datum    ellipsoid */
  /* type        tx         ty         tz     */
  /* dm          rx         ry         rz     */
  /* Datumtext                                */
extern THREAD_SAFE FILE               *def_lab_file;
extern THREAD_SAFE size_t              init_dtm_pos, init_prj_pos;

  static THREAD_SAFE  int                  d_quest = 0;
  static THREAD_SAFE  size_t               pos = 0;
  char                        pth_mlb[512], *p_tp;
  char                        d_name[24], e_name[24], p_name[24];
  short                       d_nmb;  /* datum number    */
  int                         mode, all, qr, res = -1, used;
  char                       *w, w_name[24];
  double                      sin_rx, cos_rx, scale;
  double                      sin_ry, cos_ry;
  double                      sin_rz, cos_rz;


/* set_dtm     ver 2010.1          # page 4    12 jan 2010 10 19 */


  struct typ_dec              g_tpd, *r_tpd;
  struct dsh_par {
    double       tx, ty, tz;
    double       r11, r12, r13;
    double       r21, r22, r23;
    double       r31, r32, r33;
    double       scale;
  };

  if (def_lab_file == NULL || init_prj_pos == 0) {
    (void) i_tabdir_file(3, "", &res, pth_mlb);
    if (res) {
      (void) fprintf(stdout, "\n*** def_lab.txt: %s %s;\n", pth_mlb,
        (res == -1) ? "NOT FOUND" :
        (res == -2) ? "NOT lab definition file" :
                      "Content not accepted");
      return (-2);
    }
  }

  if (init_dtm_pos) {
    (void) fseek(def_lab_file, (long) init_dtm_pos, SEEK_SET);
  } else { // search "def_dtm"
    qr = 1;
    do {
      res = fgetlhtx(def_lab_file, w_name);
      if (res == 0) {
        if (!strcmp("def_dtm", w_name)) qr = 0;
      }
    } while (qr && res != EOF);
    if (qr) {
      (void) fprintf(stdout, 
         "\n*** set_dtm: lab_def.txt DATUM not found ;\n");
      return (-2);
    }
    init_dtm_pos = ftell(def_lab_file);
  }
  if (d_quest <= 0) pos = init_dtm_pos;
  /* datum definition file detected */
  res = -1;


/* set_dtm     ver 2010.1          # page 5    12 jan 2010 10 19 */


  /* find datum name from datum number */
  if (d_no > 0) {
    d_quest = 0;
    do {
      qr = fgetlhtx(def_lab_file, d_name);
      if (qr != EOF) qr = fgetln(pth_mlb, &used, def_lab_file);
      if (qr != EOF) {
        (void) sscanf(pth_mlb, "%hd%n", &d_nmb, &used);
        if (d_nmb == d_no) {
          (void) strcpy(d_nm, d_name);
          return((int) d_no);
        }
      }
    } while (strcmp(d_name, "stop"));
    return(-1);
  } else {
    /* change to lover case letters */
    (void) strcpy(w_name, d_nm);
    w     = w_name;
    while ((*w = (char) tolower(*w)) != '\0') w++;

    if (isalpha(*d_nm) && strlen(d_nm) > 1) {
      /* get datum info from datum name */

      d_quest = 0;
      for (mode = 2; mode; mode --) {
        do {
          qr = fgetlhtx(def_lab_file, d_name);
          if (qr != EOF) {
            if (!strcmp(d_name, w_name)) {
              qr = fgetln(pth_mlb, &used, def_lab_file);
              (void) sscanf(pth_mlb, "%hd%n", &d_nmb, &used);
              p_tp = pth_mlb + used;
              /* collect datum no */
              res = d_nmb;

              /* get names of parent datum and actual ellipsoid */
              (void) sscanf(p_tp, "%s%n", p_name, &used);
              p_tp += used;
              (void) strcpy(p_nm, p_name);
              (void) sscanf(p_tp, "%s%n", e_name, &used);
              p_tp += used;
              (void) strcpy(e_nm, e_name);


/*   set_dtm     ver 2010.1          # page 6    12 jan 2010 10 19 */


              /* collect datum shift params */
              /*____________________________*/
              if (qr != EOF) qr = fgetln(pth_mlb, &used, def_lab_file);

              /* transformation type */
              (void) sscanf(pth_mlb, "%d%n", &(trp->tp), &used);
              p_tp = pth_mlb + used;

              /* translations */
              trp->tx = sgetg(p_tp, &g_tpd, &used, "m");
              p_tp   += used;
              trp->ty = sgetg(p_tp, &g_tpd, &used, "m");
              p_tp   += used;
              trp->tz = sgetg(p_tp, &g_tpd, &used, "m");
              p_tp   += used;

              /* scale and rotations */
              if (qr != EOF) qr = fgetln(pth_mlb, &used, def_lab_file);
              scale   = sgetg(pth_mlb, &g_tpd, &used, "ppm");
              p_tp    = pth_mlb + used;
              cos_rx  = sgetg(p_tp, &g_tpd, &used, "sx");
              p_tp   += used;
              sin_rx  = sin(cos_rx);
              cos_rx  = cos(cos_rx);

              cos_ry  = sgetg(p_tp, &g_tpd, &used, "sx");
              p_tp   += used;
              sin_ry  = sin(cos_ry);
              cos_ry  = cos(cos_ry);

              cos_rz  = sgetg(p_tp, &g_tpd, &used, "sx");
              sin_rz  = sin(cos_rz);
              cos_rz  = cos(cos_rz);

              /* DATUM SHIFT IS PERFORMED  STD : */
              /* TO = scale * ROTZ * ROTY * ROTX * FROM + TRANSLA */
              /*      ( cz sz 0)       (cy 0 -sy)       (1   0  0) */
              /* ROTZ=(-sz cz 0), ROTY=(0  1   0), ROTX=(0  cx sx) */
              /*      (  0  0 1)       (sy 0  cy)       (0 -sx cx) */


/*   set_dtm     ver 2010.1          # page 7    12 jan 2010 10 19 */


              trp->r11  =  cos_ry*cos_rz;
              trp->r12  =  cos_rx*sin_rz + sin_rx*sin_ry*cos_rz;
              trp->r13  =  sin_rx*sin_rz - cos_rx*sin_ry*cos_rz;

              trp->r21  = -cos_ry*sin_rz;
              trp->r22  =  cos_rx*cos_rz - sin_rx*sin_ry*sin_rz;
              trp->r23  =  sin_rx*cos_rz + cos_rx*sin_ry*sin_rz;

              trp->r31  =  sin_ry;
              trp->r32  = -sin_rx*cos_ry;
              trp->r33  =  cos_rx*cos_ry;

              trp->scale = 1.0 + scale;

              (void) fseek(def_lab_file, (long) init_dtm_pos, SEEK_SET);
              if (strcmp(p_name, w_name)) {
                *p_no = -1;
                do {
                  qr = fgetlhtx(def_lab_file, pth_mlb);
                  if (qr != EOF) {
                    if (!strcmp(p_name, pth_mlb)) {
                      qr = fgetln(pth_mlb, &used, def_lab_file);
                      (void) sscanf(pth_mlb, "%hd%n", p_no, &used);
                    }
                  }
                } while (*p_no <= 0 && strcmp(d_name, "stop"));
                if (*p_no == -1) {
                  (void) fprintf(stderr,
                      "\n*** set_dtm PROG FEJL, parent datum mgl for %s",
                      d_name);
                  return(-1);
                }
              } else *p_no = d_nmb;
            }
          }
        } while (res <= 0 && strcmp(d_name, "stop"));
        if (mode == 2) {
          if (res > 0  || strlen(w_name) < 4) -- mode;
          else 
          if (*w_name == 'i' || *w_name == 'e') {
            (void) fseek(def_lab_file, (long) init_dtm_pos, SEEK_SET);
            /* try igsyy, itrfyy or etrfyy */
            w       = w_name + strlen(w_name) - 2; 
            *(w)    = 'y';
            *(w +1) = 'y';
          }
          else -- mode;
        }
      }
      /* return res or 'no datum found' */
      return(res);
    } else


/* set_dtm     ver 2010.1          # page 8    12 jan 2010 10 19 */


    if (*d_nm == '!' || *d_nm == '?' || *d_nm == '*') {
      all = (strlen(d_nm) == 1)   ? 0 : /* all datum texts */
            (isalpha(*(d_nm +1))) ? 2 : 1;
      if (all == 1) (void) sscanf(d_nm +1, "%hd", &d_no);
      (void) fseek(def_lab_file, (long) pos, SEEK_SET);
      for (mode = (all < 2) ? 1 : 2; mode; mode --) {
        do {
          qr = fgetlhtx(def_lab_file, d_name);
          if (qr != EOF) {
            qr = fgetln(pth_mlb, &used, def_lab_file);
            /* collect datum no */
            (void) sscanf(pth_mlb, "%hd%n", &d_nmb, &used);
            if (all == 0 || (all == 1 && d_no == d_nmb) ||
               (all == 2 && !strcmp(w_name +1, d_name))) {
              p_tp = pth_mlb + used;
              /* get names of parent datum and actual ellipsoid */
              (void) sscanf(p_tp, "%s%n", p_name, &used);
              p_tp += used;
              (void) strcpy(p_nm, p_name);
              (void) sscanf(p_tp, "%s%n", e_name, &used);
              p_tp += used;
              (void) strcpy(e_nm, e_name);
              qr = fgetln(pth_mlb, &used, def_lab_file);
              if (*d_nm == '*') {
                /* transformation type */
                (void) sscanf(pth_mlb, "%d%n", &(trp->tp), &used);
                p_tp = pth_mlb + used;
                /* translations */
                r_tpd    = (struct typ_dec *) (&trp->r31);
                trp->tx  = sgetg(p_tp, r_tpd, &used, "m");
                p_tp    += used;
                trp->ty  = sgetg(p_tp, r_tpd, &used, "m");
                p_tp    += used;
                trp->tz  = sgetg(p_tp, r_tpd, &used, "m");
                p_tp    += used;
                /* scale and rotations */
                r_tpd = (struct typ_dec *) (&trp->r32);
                if (qr != EOF) qr = fgetln(pth_mlb, &used, def_lab_file);
                trp->scale = sgetg(pth_mlb, r_tpd, &used, "ppm");
                r_tpd      = (struct typ_dec *) (&trp->r33);
                p_tp       = pth_mlb + used;
                trp->r11   = sgetg(p_tp, r_tpd, &used, "sx");
                p_tp      += used;
                trp->r12   = sgetg(p_tp, r_tpd, &used, "sx");
                p_tp      += used;
                trp->r13   = sgetg(p_tp, r_tpd, &used, "sx");
              }
              else qr = fgetln(pth_mlb, &used, def_lab_file);
              (void) fgets(pth_mlb, 72, def_lab_file);
              qr = (int) strlen(pth_mlb) -1;
              if (*(pth_mlb +qr) == '\n') *(pth_mlb +qr) = '\0';
              (void) sprintf(d_nm, "%-10s   * %-36s;", d_name, pth_mlb);


/*   set_dtm     ver 2010.1          # page 9    12 jan 2010 10 19 */

              if (all == 0 && d_nmb > 0) {
                pos     = ftell(def_lab_file);
                ++ d_quest;
              } else {
                pos     = init_dtm_pos;
                d_quest = 0;
              }
              return((int) d_nmb);
            }
          } else {
            (void) fprintf(stderr,
                   "\n*** set_dtm def_dtm.txt ERROR after count %d",
                   d_quest);
            pos     = init_dtm_pos;
            d_quest = 0;
            return(-1);
          }
        } while (res <= 0 && strcmp(d_name, "stop"));
        if (mode == 2) {
          if (res > 0) -- mode;
          else {
            /* try igsyy, itrfyy or etrfyy */
            w       = w_name + strlen(w_name) - 2; 
            *(w)    = 'y';
            *(w +1) = 'y';
            (void) fseek(def_lab_file, (long) pos, SEEK_SET);
          }
        } else {
          pos = 0;
        }
      }
    }
  }
  return(-1);
}

