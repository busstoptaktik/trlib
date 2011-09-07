

/* itrf_trans  ver 2007.02          # page 1    10 Oct 2008 11 57 */


/* Copyright (c) 2007, National Space Institute, DTU, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of DTU, Denmark */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */

#include <stdio.h>
#include <string.h>
#include "geo_lab.h"
#include "geoid_d.h"

#include "conv_lab.h"
#include "trthread.h"


int                      itrf_trans(
/*________________________________*/
union geo_lab            *i_lab,
union geo_lab            *o_lab,
int                       stn_vel,      /* velocity for stn in i_vel */
char                     *tab_t,        /* name of plate model requested */
char                     *tab_i,        /* name of intra plate model requested */
double                   *i_crd,        /* input coords:: 0:2 */
double                   *i_vel,        /* velocity to coords:: 0:2 */
double                    i_JD,         /* Julian Day from 2000.0 */
double                   *o_crd,        /* output coords:: 0:2 */
double                   *o_vel,        /* NOT calculated:: 0:2 (==0.0) */
int                      *plm_trf,      /* plates model used :      0/1 */
int                      *ipl_trf,      /* intra plate model used : 0/1 */
double                   *plm_JD,       /* Plate model gate date */
double                   *ipl_iJD,      /* input INTRA plate gate date */
double                   *ipl_oJD,      /*       INTRA plate gate date output */
char                     *used_plm_nam, /* name of plate used */
char                     *used_plt_nam, /* name of plate used */
char                     *used_ipt_nam, /* name of intra plate used */
char                     *usertxt,
char                     *err_str
)
{

#include              "gtc.h"
#include              "ptg.h"
#include              "set_itrf_c.h"
#include              "srch_plate.h"
#include              "s_status.h"
#include              "tab3d_c.h"

int ipl_move(union geo_lab *Hipl_lab, struct mtab3d_str *tab3d_table,
             double tr_yy, double B, double L,
             double *dX, double *dY, double *dZ, char *used_ipt_nam,
             char *err_str);


/* itrf_trans  ver 2007.02          # page 2    10 Oct 2008 11 57 */


  /* itrf_trans initialize the tables needed or closes the tables */
  /* o_lab = NULL or i_lab = NULL closes the tables            */

  /* itrf_trans transforms i_crd[3] coordinates from:          */
  /*     i_lab   i_JD  to  o_lab   o_lab->u_c_lab.JD (= o_JD)  */
  /* using the official convertions between the ITRF's and IGS */
  /*     and ETRF89 and nationsl realisations DK_ETRF89,       */
  /*     NO_ETRF89, SE_ETRF89, SF_ETRF89 and gr96              */
  /* and  1) the STD plate velocities                          */
  /* or   2) the named plate velocities                        */
  /* or   3) no velocities             : NON                   */
  /* and  1) the STD intra plate velocities                    */
  /* or   2) no intra plate velocities : NON                   */
  /* National ETRF89 is governed by the i_lab and/or o_lab     */
  /*      region: lab->u_c_lab.rgn = {DK/NO/FI/SE/GR}          */
  /*      else the EUREF recommandations are used              */

  /* usertxt: at ERROR: put first in err_str                   */ 

  /* itrf_trans may return the values:                                  */
  /*       0               : OK                                         */
  /*      -1 TRF_INACC_    : inaccurate result                          */
  /*      -5 TRF_ILLEG_    : illegal label combination                  */
  /*      -6 TRF_PROGR_    : programming error in function              */
  /*    -100 TRF_C_ARE_    : out of table area                          */
  /*  -20000 PLATE_OUT_    : No plate for position found  (TRF_PROGR_)  */
  /*  -30000 ITRF_NON_     : NO entries found             (TRF_PROGR_)  */
  /*  -40000 ITRF_SEQ_     : entries are NOT consequtive  (TRF_PROGR_)  */
  /*  -50000 ITRF_DTM_     : Illegal datum in from_lb/to_lb             */
  /*  -70000 ITRF_NAM_     : manager.gps table_name (gpstab) not found  */
  /*  -80000 ITRF_SYS_     : manager.gps file not found                 */
  /*  -90000 PLATE_NO_VEL_ : Plate has no velocity                      */

  /* tab_name strategy ::                                             */
  /* stn_vel == 1  :  pl_tr i_JD to    o_JD i_vel                     */
  /* stn_vel == 0  ::                                                 */
  /*     tab_t == nnr_std : pl_tr i_JD to o_JD STD:newest nnr_itrfYY  */
  /*     tab_t == std :  pl_tr    i_JD to o_JD STD:newest nnr_itrfYY  */
  /*     tab_t != '\0':  pl_tr    i_JD to o_JD table tab_t vel        */
  /*     tab_t == '\0':  NO pl_tr                                     */
  /*     tab_i == std :  ipl_tr   i_JD to o_JD SDT:ipl model for area */
  /*     tab_i != '\0':  ipl_tr   i_JD to o_JD table tab_t vel        */
  /*     tab_i == '\0':  NO ipl_tr                                    */

  /* Exceptions:  region: (i_lab/o_lab)->u_c_lab.rgn                  */
  /* 1) NATIONAL ETRF89 : DK, NO, SV, SF::                            */
  /*     tab_t == {"" or "std"} used_plm_nam == nnr_itrf00            */
  /*              to:  i_JD to nkg_dt;  fr: nkg_dt to o_JD            */
  /*           else the given name is used as NON STANDARD            */
  /*     tab_i == {"" or "std"} used_ipl_nam == nkgrf03vel.01         */
  /*              to:  i_JD to ipl_dt;  fr: ipl_dt to o_JD            */
  /*           else the given name is used as NON STANDARD            */
  /* 2) EUREF recommandation : EU                                     */
  /*     tab_t == {"" or "STD"} used_plm_nam == recommended           */
  /*     tab_i == {"" or "STD"} used_ipl_nam == ""                    */
  /*           else the given name is used as NON STANDARD            */



/* itrf_trans  ver 2007.02          # page 2    10 Oct 2008 11 57 */


  /* GR96 epoch       :: 1996 08 15 :: 1996.620 :: ITRF94      */
  /* ETRF89 NKG area DK, NO, SE, FI :: ipl_dt ::   GATE        */
  /* ETRF89 NKG epoch :: 2003 10 01 :: 2003.750 :: ITRF2000    */
  /* DK_ETRF89  epoch :: 1994 09 15 :: 1994.707 :: ITRF93      */
  /* NO_ETRF89  epoch :: 1994 08 31 :: 1994.665 :: ITRF93      */
  /* SV_ETRF89  epoch :: 1999 07 02 :: 1999.500 :: ITRF97      */
  /* SF_ETRF89  epoch :: 1997 01 01 :: 1997.000 :: ITRF96      */

  /* RETURN PARAMETERS:                                        */
  /* o_crd[3] : Transformed i_crd[3]                           */
  /* o_vel[3] : Transformed i_vel[3]: NOT ASSIGNED: 0.0        */
  /* plm_trf = 1/0: nnr plate model used/(not used)            */
  /* ipl_trf : {1,2,3} : std gates using:                      */
  /*                    {ipl_iJD, ipl_oJD, ipl_iJD & ipl_oJD}  */ 
  /*         : {4, 0} : intra plate model {used, not used}     */
  /*                    (ipl_iJD & ipl_oJD = -10000000.0)      */
  /* plm_JD   : gate date for Plate Model                      */
  /* ipl_iJD  : ipl_dt of input National  (INTRA Plate)        */
  /* ipl_oJD  : ipl_dt of output National (INTRA Plate)        */
  /* plm_nam is the name of the actual nnr plate model         */
  /* plt_nam is the name of the actual plate                   */
  /* ipl_nam is tha name of the actual intra plate model       */
  /* err_str: at ERROR: user_txt cat ERROR description         */

  /*         TRANSFORMATION PRODUCTION LINE   */
  /*         <--UP-HILL--> <--DOWN-HILL-->    */
  /* cstm:     PRJ   CRT     CRT    PRJ       */
  /* action:     GDTR    CTR    GDTR          */
  /* action:       0      1       2           */

  /* basic transformation formula (Molodensky) ::                   */
  /* (XS) = (X) + (T1) + ( D  -R3  R2) (X)                          */
  /* (YS) = (Y) + (T2) + ( R3  D  -R1) (Y)                          */
  /* (ZS) = (Z) + (T3) + (-R2  R1  D ) (Z)                          */


  /* req_ipl_tr (gps_table)is true when region is from {DK, NO, SE, FI}*/
  /* req_ipl_tr   :i: ipl_tr i_JD to   ipl_dt STD:nkgrf03vel           */
  /* req_ipl_tr   :o: ipl_tr ipl_dt to o_JD STD:nkgrf03vel             */

#define     IDTT  (0)
#define     ITRF  (1)
#define     IGST  (2)
#define     IGAT  (3)
#define     OGAT  (4)
#define     ILLE  (9)

  static THREAD_SAFE  struct coord_lab    *i_clb = NULL, *o_clb = NULL;
  static THREAD_SAFE  int                  i_chsum = 0, o_chsum = 0, b_lev=-99;
  static THREAD_SAFE  int                  o_stn_vel = 0, s_lev, seq_max = -1;
  static THREAD_SAFE  int                  s_stn_vel = -1;
  static THREAD_SAFE  double               sate_in = 0.0, sate_out = 0.0;
  static THREAD_SAFE  struct gps_str       gps_table;
  static THREAD_SAFE  struct gps_c_str     plm_tr;
  static THREAD_SAFE  struct plate_info    pl_inf[MAX_ENT_PLM];
  static THREAD_SAFE  char                 s_tab_t[MLBLNG], s_tab_i[MLBLNG];
  static THREAD_SAFE  char                 s_used_plm_nam[MLBLNG];
  static THREAD_SAFE  char                 s_used_ipl_nam[MLBLNG];


/* itrf_trans  ver 2007.02          # page 3    10 Oct 2008 11 57 */


  static THREAD_SAFE  struct mtab3d_str    ipl_table;
  static THREAD_SAFE  union geo_lab        plm_lab_a;
  static THREAD_SAFE  union geo_lab        Hipl_lab;      /* prj_datum    */

  struct plm_lab             *plm_lab = &plm_lab_a.u_m_lab;
  struct gps_c_str           *shp = NULL, *Dshp = NULL;

  int                         act, lev, rs, ipl_tr;
  int                         res = 0, ipl_res = 0;
  double                      N, E, H, h, B=0.0, L=0.0, X, Y, Z;
  double                      VX=0.0, VY=0.0, VZ=0.0, WX, WY, WZ;

  /* TEST
  char                       *ACT_STR[] = {
    "IDTT", "ITRF", "IGST", "IGT", "OGT", "ILLE",
     "ILLE", "ILLE","ILLE", "ILLE"
  };
  TEST */
  int                         TU = 0;

  *(o_crd+0) = 0.0;
  *(o_crd+1) = 0.0;
  *(o_crd+2) = 0.0;
  *(o_vel+0) = 0.0;
  *(o_vel+1) = 0.0;
  *(o_vel+2) = 0.0;

  if (i_lab == NULL || o_lab == NULL) {
    if (i_clb != NULL && o_clb != NULL) {
      /* clean up and close */
      if (ipl_table.init) (void) tab3d_c(&ipl_table);
      if (plm_lab->f_poly != NULL) (void) fclose(plm_lab->f_poly);
      ipl_table.init       = 0;
      i_clb                = NULL;
      o_clb                = NULL;
      plm_lab->f_poly      = NULL;
      b_lev                = -49;
      gps_table.req_plm_tr = -2;
      gps_table.req_ipl_tr = 0;
      *s_used_plm_nam      = *s_used_ipl_nam = '\0';
      return(0);
    } else
    return(-999);
  } else
  if (b_lev == -99) {
    b_lev            = -49;
    ipl_table.init   = 0;
    *s_used_plm_nam  = *s_used_ipl_nam = '\0';
  }

/* itrf_trans  ver 2007.02          # page 4    10 Oct 2008 11 57 */


  /* Init of tables */
  /*________________*/
  if (i_clb     != &(i_lab->u_c_lab) ||
      i_chsum   != (i_lab->u_c_lab).ch_sum ||
      o_clb     != &(o_lab->u_c_lab) ||
      o_chsum   != (o_lab->u_c_lab).ch_sum ||
      s_stn_vel != stn_vel ||
      sate_in   != i_JD || sate_out != o_lab->u_c_lab.JD ||
      strcmp(tab_t, s_tab_t) || strcmp(tab_i, s_tab_i)) {  
/*
(void) fprintf(stdout, "\n\nitrf_trans:: %s, %s, %s, %s\n",
i_lab->u_c_lab.mlb, o_lab->u_c_lab.mlb, tab_t, tab_i);
*/
    /* Init of IGS and ITRF-tables */
    i_clb                = &(i_lab->u_c_lab);
    o_clb                = &(o_lab->u_c_lab);
    i_chsum              = o_chsum = 0;
    gps_table.req_plm_tr = -2;
    gps_table.req_ipl_tr = 0;
    /* Coord labels */
    if (i_clb->lab_type != CRD_LAB || o_clb->lab_type != CRD_LAB) {
      return(s_status(err_str,
             "itrf_trans(i/o labels error)", TRF_PROGR_));
    }
    if ((i_clb->cstm != 1 && *(i_clb->mlb + i_clb->sepix) != 'E') ||
        (o_clb->cstm != 1 && *(o_clb->mlb + o_clb->sepix) != 'E')) {
      return(s_status(err_str,
             "itrf_trans(i/o CRT or Ellipsoid error)", TRF_PROGR_));
    }
/*
(void) fprintf(stdout, "\n*itrf_trans : i_cstm, dtm = %d, %d;",
i_clb->cstm, i_clb->datum);
(void) fprintf(stdout, "\n*itrf_trans : o_cstm, dtm = %d, %d;",
o_clb->cstm, o_clb->datum);
*/

    (void) strcpy(used_plm_nam, "");
    (void) strcpy(used_plt_nam, "non");
    (void) strcpy(used_ipt_nam, "non");

    seq_max = set_itrf_c(i_clb, o_clb, &i_JD, stn_vel, &o_stn_vel,
                         &gps_table, tab_t, plm_lab, pl_inf,
                         tab_i, &ipl_table, &Hipl_lab,
                         s_used_plm_nam, s_used_ipl_nam,
                         err_str);
if (TU)
(void) fprintf(stdout, "\n*itrf_trans : seq_max = %d;", seq_max);

    if (seq_max < 0) return(seq_max);
    /* start and stop level: get a geo_itrf */
    b_lev = 0;
    s_lev = (o_clb->cstm != 1) ? 2 : 1;

if (TU)
(void) fprintf(stdout,
"\n*itrf_trans : req_plm = %d;", gps_table.req_plm_tr);


/* itrf_trans  ver 2007.02          # page 5    10 Oct 2008 11 57 */


    /* test for identical systems */
    if (i_clb->ch_sum == o_clb->ch_sum 
        && !stn_vel && !gps_table.req_plm_tr && !gps_table.req_ipl_tr) {
      b_lev = 5;
      s_lev = 4;
    }

    act = 0;

    /* save checksums */
    i_chsum   = i_clb->ch_sum;
    o_chsum   = o_clb->ch_sum;
    s_stn_vel = stn_vel;
    sate_in   = i_JD;
    sate_out  = o_clb->JD;  
    (void) strcpy(s_tab_t, tab_t);
    (void) strcpy(s_tab_i, tab_i);

  } /* init of tables */

  *plm_trf = 0;
  *ipl_trf = 0;
  *ipl_iJD = -10000000.0;
  *ipl_oJD = -10000000.0;

/*
(void) fprintf(stdout, "\n*itrf_trans : b_lev, s_lev = %d, %d;",
b_lev, s_lev);
(void) fprintf(stdout, "\n*itrf_trans : i_dat,o_dat = %f, %f;",
i_JD, o_clb->JD);
*/
  N = *(i_crd +0);
  E = *(i_crd +1);
  H = *(i_crd +2);
  if (stn_vel) {
    VX = *(i_vel +0);
    VY = *(i_vel +1);
    VZ = *(i_vel +2);
  }

  for (lev = b_lev; lev <= s_lev && res >= TRF_TOLLE_; lev++) {
if (TU)
(void) fprintf(stdout, "\n\n*itrf_trans  lev %d;", lev);

    switch(lev) {
    case 0: /* XXX_dtm -> geo_itrf && PRJ_dtm -> CRT_dtm */
      if (i_clb->cstm == 1 /* crt */) {
        res = gtc(i_lab, -1, N, E, H, &B, &L, &h, "", NULL);
      } else {
        if (i_clb->cstm == 2 /* geo */) {
          B   = *(i_crd +0);
          L   = *(i_crd +1);
        } else res = ptg(i_lab, +1, N, E, &B, &L, "", NULL);
        res = gtc(i_lab, +1, B, L, *(i_crd+2), &N, &E, &H, "", NULL);
      }
      if (res < TRF_TOLLE_) return(res);
      break;


/* itrf_trans  ver 2007.02          # page 6    10 Oct 2008 11 57 */


    case 1: /* central: CRT shifts to CRT */
      ipl_tr = (!gps_table.req_ipl_tr) ? -1
             : gps_table.seq[(gps_table.req_ipl_tr & 2)
             ? (seq_max -1) : 0];
      if (gps_table.req_ipl_tr & 1) *ipl_iJD = gps_table.i_ipl_dt;
      if (gps_table.req_ipl_tr & 2) *ipl_oJD = gps_table.o_ipl_dt;
      if (seq_max == 0 && gps_table.req_ipl_tr) {
        ipl_res  = ipl_move(&Hipl_lab, &ipl_table, gps_table.ipl_yy,
                            B, L, &X, &Y, &Z, used_ipt_nam,
                            err_str);
        N       += X;
        E       += Y;
        H       += Z;
        ipl_tr   = -1;
        *ipl_trf = (gps_table.req_ipl_tr) ? gps_table.req_ipl_tr : 4;
      }
      if (ipl_tr == OGAT) ipl_tr += 10;

      for (rs = 0; rs < seq_max; rs ++) {
        act = gps_table.seq[rs];
/*
(void) fprintf(stdout, "\n*ITRF_TRANS ACTION  %s;", ACT_STR[act]);
*/
        switch(act) {
        case IGAT: /* IGATE <-> ITRFyy */
          shp  = &gps_table.igat_tr;
          Dshp = NULL;
          break;
        case OGAT: /* OGATE <-> ITRFyy */
          shp  = &gps_table.ogat_tr;
          Dshp = NULL;
          break;
        case ITRF: /* ITRFyy  <-> ITRFxx */
          shp  = &gps_table.itrf_tr;
          Dshp = &gps_table.itrf_dt;
          break;
        case IGST: /* IGSyy   <-> IGSxx  */
          shp  = &gps_table.igs__tr;
          Dshp = &gps_table.igs__dt;
          break;
        case IDTT: /* IDENT  */
          shp = NULL;
          break;
        default: /* ERROR  */
          shp = NULL;
          return(s_status(err_str, "itrf_trans", TRF_ILLEG_));
        }


/* itrf_trans  ver 2007.02          # page 7    10 Oct 2008 11 57 */


        if (act +10 == ipl_tr) {
/*
(void) fprintf(stdout, "\n*trf : %-8.6f  %-8.6f   %-8.6f;", N, E, H);
*/
          ipl_res  = ipl_move(&Hipl_lab, &ipl_table, gps_table.ipl_yy,
                              B, L, &X, &Y, &Z, used_ipt_nam,
                              err_str);
          N       += X;
          E       += Y;
          H       += Z;
          ipl_tr   = -1;
          *ipl_trf = (gps_table.req_ipl_tr) ? gps_table.req_ipl_tr : 4;

/*
(void) fprintf(stdout, "\n*ipl : %-8.6f  %-8.6f   %-8.6f;", X, Y, Z);
*/
        }

        if (shp != NULL) {
          if (stn_vel && Dshp != NULL) {
            /* Molodensky transformation SMALL SHIFTS only */
            /* SECOND ORDER REMOVED */            /* I_VEL Trf + DT */
            WX  = /*VX*shp->sc*/ +Dshp->tx - VY*shp->rz + VZ * shp->ry;
            WY  = /*VY*shp->sc*/ +Dshp->ty + VX*shp->rz - VZ * shp->rx;
            WZ  = /*VZ*shp->sc*/ +Dshp->tz - VX*shp->ry + VY * shp->rx;
                                                  /* crd D_ROT */
            WX += /*(shp->sc+1.0)* */(- E * Dshp->rz + H * Dshp->ry);
            WY += /*(shp->sc+1.0)* */(+ N * Dshp->rz - H * Dshp->rx);
            WZ += /*(shp->sc+1.0)* */(- N * Dshp->ry + E * Dshp->rx);
                                                  /* crd D_scale */
            WX += Dshp->sc * (N /*- E*shp->rz + H*shp->ry*/);
            WY += Dshp->sc * (E /*+ N*shp->rz - H*shp->rx*/);
            WZ += Dshp->sc * (H /*- N*shp->ry + E*shp->rx*/);
            VX += WX;
            VY += WY;
            VZ += WZ;
          }

/*
(void) fprintf(stdout, "\n*trf : %-8.6f  %-8.6f   %-8.6f;", N, E, H);
(void) fprintf(stdout, "\n*tras: %-8.6f  %-8.6f   %-8.6f   %-8.6f;",
shp->tx, shp->ty, shp->tz, shp->sc);
(void) fprintf(stdout, "\n*trot: %-8.6e  %-8.6e   %-8.6e;",
shp->rx, shp->ry, shp->rz);
*/
          /* Molodensky transformation SMALL SHIFTS only */
          X = N * shp->sc + shp->tx - E * shp->rz + H * shp->ry;
          Y = E * shp->sc + shp->ty + N * shp->rz - H * shp->rx;
          Z = H * shp->sc + shp->tz - N * shp->ry + E * shp->rx;
/*
(void) fprintf(stdout, "\n*TRF : %-8.6f  %-8.6f   %-8.6f;", X, Y, Z);
(void) fprintf(stdout, "\n*res : %-8.6f  %-8.6f   %-8.6f;", N, E, H);
*/
          N += X;
          E += Y;
          H += Z;
        }
        if (act == ipl_tr) {
/*
(void) fprintf(stdout, "\n*trf : %-8.6f  %-8.6f   %-8.6f;", N, E, H);
(void) fprintf(stdout, "\n*trf :ipl:B,L: %-8.6f  %-8.6f;",
B*180/M_PI, L*180/M_PI);
*/
          ipl_res  = ipl_move(&Hipl_lab, &ipl_table, gps_table.ipl_yy,
                              B, L, &X, &Y, &Z, used_ipt_nam,
                              err_str);
          N       += X;
          E       += Y;
          H       += Z;
          ipl_tr   = -1;
          *ipl_trf = (gps_table.req_ipl_tr) ? gps_table.req_ipl_tr : 4;
/*
(void) fprintf(stdout, "\n*ipl : %-8.6f  %-8.6f   %-8.6f;", X, Y, Z);
*/
        }
      } /* loop CRT */


/* itrf_trans  ver 2007.02          # page 8    10 Oct 2008 11 57 */


      *plm_JD = -10000000.0;
      if (gps_table.req_plm_tr) {
/*
(void) fprintf(stdout, "\n*trf : %-8.6f  %-8.6f   %-8.6f;", N, E, H);
*/
        /* select plate no *plate_nr */
        if ((res = srch_plate(pl_inf, B, L,
                   plm_lab, &plm_tr, err_str)) >= 0) {
          (void) strcpy(used_plm_nam, plm_lab->mlb);
          if (pl_inf->poly_tp < 60) {
            (void) strcpy(used_plm_nam, plm_lab->mlb);
            (void) strcpy(used_plt_nam, plm_tr.datum);
            *plm_trf = 1;
            *plm_JD  = gps_table.plm_dt;
/*
(void) fprintf(stdout, "\n*plm :rot: %f, %f, %f, yy=%f;",
plm_tr.rx*180*60*60/M_PI, plm_tr.ry*180*60*60/M_PI,
plm_tr.rz*180*60*60/M_PI, gps_table.plm_yy);
*/
            /* Molodensky transformation SMALL SHIFTS only */
            X      = (-E * plm_tr.rz + H * plm_tr.ry) *gps_table.plm_yy;
            Y      = ( N * plm_tr.rz - H * plm_tr.rx) *gps_table.plm_yy;
            Z      = (-N * plm_tr.ry + E * plm_tr.rx) *gps_table.plm_yy;
/*
(void) fprintf(stdout, "*plm_yy %-5.3f;", gps_table.plm_yy);
(void) fprintf(stdout, "\n*plm %-8.6f  %-8.6f   %-8.6f;", X, Y, Z);
*/
            N     += X;
            E     += Y;
            H     += Z;
          }
          else res = PLATE_NO_VEL_;
        } else {
          (void) strcpy(used_plt_nam, "PLATE_OUT");
          plm_tr.rx = 0.0;
          plm_tr.ry = 0.0;
          plm_tr.rz = 0.0;
          res       = PLATE_OUT_;
          (void) s_status(err_str, usertxt, res);
        }
      }
      else
      if (stn_vel) {
        X      = *(i_vel +0) * gps_table.plm_yy;
        Y      = *(i_vel +1) * gps_table.plm_yy;
        Z      = *(i_vel +2) * gps_table.plm_yy;
        N     += X;
        E     += Y;
        H     += Z;
      }

      /* central: CTR shifts to CRT */
      break;

    case 2: /* CRT_dtm -> PRJ_dtm */
      res = gtc(i_lab, -1, N, E, H, &N, &E, &H, "", NULL);
      if (o_clb->cstm != 2 /* geo */)
         res = ptg(i_lab, -1, N, E, &N, &E, "", NULL);
      if (res < TRF_TOLLE_) return(res);
      break;
    default:
      return(s_status(err_str, usertxt, TRF_ILLEG_));
    }
  } /* lev LOOP */


/* itrf_trans  ver 2007.02          # page 9    10 Oct 2008 11 57 */


  /* return of transformation */
  *(o_crd+0) = N;
  *(o_crd+1) = E;
  *(o_crd+2) = H;
  if (o_stn_vel) {
    *(o_vel +0) = VX;
    *(o_vel +1) = VY;
    *(o_vel +2) = VZ;
  }

  if (res > ipl_res) res = ipl_res;
  return(res);

#undef     IDTT
#undef     ITRF
#undef     IGST
#undef     IGAT
#undef     OGAT
#undef     ILLE

}

int                       ipl_move(
/*________________________________*/
union geo_lab            *Hipl_lab,
struct mtab3d_str        *tab3d_table,
double                    tr_yy, /* Julian Year shift */
double                    B,
double                    L,
double                   *dX,
double                   *dY,
double                   *dZ,
char                     *used_ipt_nam,
char                     *err_str
)
{

#include              "tab3d_val.h"


/* itrf_trans  ver 2007.02          # page 10   10 Oct 2008 11 57 */


  int                         res = 0;
  double                      dc[3];
  double                      csB, snB, csL, snL;

  /* basic transformation formula (Molodensky) ::             */
  /* (dX) = (T1) + ( D  -R3  R2) (X)                          */
  /* (dY) = (T2) + ( R3  D  -R1) (Y); (X,Y,Z) += (dX,dY,dZ)   */
  /* (dZ) = (T3) + (-R2  R1  D ) (Z)                          */

  /* T == 0.0,  D = 0.0, R = ipl_R * tr_yy * 0.001   */

  /* TEST */
  int                         TU = 0;

  tr_yy *= 0.001; /* vel in mm/year: conv to m/year */

  res    = tab3d_val(Hipl_lab, tab3d_table, B, L, dc, err_str);
  (void) strcpy(used_ipt_nam, tab3d_table->table_u.mlb);
if (TU) (void) fprintf(stdout,
"\n*ipl_move  dc = %f, %f, %f, res = %d;",
dc[0], dc[1],dc[2], res);

  if (res >= TRF_TOLLE_) {
if (TU) (void) fprintf(stdout,
"\n*ipl_move  dc = %f, %f, %f, res = %d;",
dc[0], dc[1],dc[2], res);
    dc[0] *= tr_yy;
    dc[1] *= tr_yy;
    dc[2] *= tr_yy;
if (TU) (void) fprintf(stdout, "\n*ipl_move  dc = %f, %f, %f, dd = %e;",
dc[0], dc[1],dc[2], tr_yy);
    csB    = cos(B);
    snB    = sin(B);
    csL    = cos(L);
    snL    = sin(L);
if (TU) {
(void) fprintf(stdout, "\n*ipl_move  ROT(B)= %f, %f, %f ",
-snB*csL, -snL, csB*csL);
(void) fprintf(stdout, "\n ipl_move  ROT(B)= %f, %f, %f ",
-snB*snL, csL, csB*snL);
(void) fprintf(stdout, "\n ipl_move  ROT(B)= %f, %f, %f;",
csB, 0.0, snB);
}
    *dX    = -snB*csL * dc[0]  - snL * dc[1]  + csB*csL * dc[2];
    *dY    = -snB*snL * dc[0]  + csL * dc[1]  + csB*snL * dc[2];
    *dZ    =  csB     * dc[0] /*+0.0* dc[1]*/ + snB     * dc[2];
if (TU) (void) fprintf(stdout, "\n*ipl_move  dc = %f, %f, %f;",
*dX, *dY,*dZ);
  } else {
if (TU) (void) fprintf(stdout, "\n\n*ipl_move  res = %d;", res);
    if (res != TAB_C_ARE_) res = 0;
    *dX = 0.0;
    *dY = 0.0;
    *dZ = 0.0;
  }

  /* return of transformation */
  return(res);

}
