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
 


/* itrf_trans  ver 2007.02          # page 1    10 Oct 2008 11 57 */


/* Copyright (c) 2007, National Space Institute, DTU, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of DTU, Denmark */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */

#if !defined(H_ITRF_TRANS)
#define      H_ITRF_TRANS

#include              "gd_trans.h"
#include              "trlib_intern.h"

extern int               itrf_trans(
/*________________________________*/
struct coord_lab         *i_lab,
struct coord_lab         *o_lab,
int                       stn_vel,      /* velocity for stn in i_vel */
double                   *i_crd,        /* input coords:: 0:2 */
double                   *i_vel,        /* velocity to coords:: 0:2 */
double                    i_JD,         /* Julian Day from 2000.0 */
double                   *o_crd,        /* output coords:: 0:2 */
double                   *o_vel,        /* NOT calculated:: 0:2 (==0.0) */
struct PLATE             *plate_info,   /* call and return info on plates */
double                   *tr_par,       /* 7-par describing the transformation */
char                     *usertxt,
char                     *err_str
);

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
/* region: (i_lab/o_lab)->u_c_lab.rgn                        */
/*       = {DK/NO/SF/SV/GR} : National ETRF89                */
/*       = EU : the EUREF recommandations are used           */
/*      else ITRF PLATE and INTRA PLATE MODELS are used      */

/* usertxt: at ERROR: put first in err_str                   */


/* itrf_trans  ver 2007.02          # page 2    10 Oct 2008 11 57 */


/* itrf_trans may return the values:                                 */
/*       0               : OK                                        */
/*      -1 TRF_INACC_    : inaccurate result                         */
/*      -5 TRF_ILLEG_    : illegal label combination                 */
/*      -6 TRF_PROGR_    : programming error in function             */
/*    -100 TRF_C_ARE_    : out of table area                         */
/*  -20000 PLATE_OUT_    : No plate for position found  (TRF_PROGR_) */
/*  -30000 ITRF_NON_     : NO entries found             (TRF_PROGR_) */
/*  -40000 ITRF_SEQ_     : entries are NOT consequtive  (TRF_PROGR_) */
/*  -50000 ITRF_DTM_     : Illegal datum in from_lb/to_lb            */
/*  -60000               : tr_par != NULL && stn_vel !=0 : ILLEGAL   */
/*  -70000 ITRF_NAM_     : manager.gps table_name (gpstab) not found */
/*  -90000 ITRF_SYS_     : manager.gps file not found                */
/* -100000 PLATE_NO_VEL_ : Plate has no velocity                     */


/* tab_name strategy ::                                             */
/* tab_t = plate_info->plate_model         to be used               */
/* tab_i = plate_info->intra_plate_model   to be used               */
/* stn_vel == 1  :  pl_tr i_JD to    o_JD i_vel                     */
/* stn_vel == 0  ::                                                 */
/*     tab_t == nnr_std : pl_tr i_JD to o_JD STD:newest nnr_itrfYY  */
/*     tab_t == std :  pl_tr    i_JD to o_JD STD:newest nnr_itrfYY  */
/*     tab_t != '\0':  pl_tr    i_JD to o_JD table tab_t vel        */
/*     tab_t == '\0':  NO pl_tr                                     */
/*     tab_i == std :  ipl_tr   i_JD to o_JD SDT:ipl model for area */
/*     tab_i != '\0':  ipl_tr   i_JD to o_JD table tab_i vel        */
/*     tab_i == '\0':  NO ipl_tr                                    */

/* Exceptions:                                                      */
/* 1) to/from NATIONAL ETRF89 : DK, NO, SF, SV::                    */
/*     tab_t == {"" or "STD"} used_plm_nam == nnr_itrf00            */
/*              to:  i_JD to nkg_dt;  fr: nkg_dt to o_JD            */
/*           else the given name is used as NON STANDARD            */
/*     tab_i == {"" or "std"} used_ipl_nam == nkgrf03vel.01         */
/*              to:  i_JD to ipl_dt;  fr: ipl_dt to o_JD            */
/*           else the given name is used as NON STANDARD            */
/* 2) EUREF recommandation ETRF89 : EU ::                           */
/*     tab_t == {"" or "STD"} used_plm_nam == recommended           */
/*     tab_i == {"" or "STD"} used_ipl_nam == ""                    */
/*           else the given name is used as NON STANDARD            */

/* GR96 epoch       :: 1996 08 15 :: 1996.xxx :: ITRF94      */
/* ETRF89 NKG area DK, NO, SE, SF :: ipl_dt   ::   GATE      */
/* ETRF89 NKG epoch :: 2003 10 01 :: 2003.750 :: ITRF2000    */
/* DK_ETRF89  epoch :: 1994 09 15 :: 1994.707 :: ITRF93      */
/* NO_ETRF89  epoch :: 1994 08 31 :: 1994.665 :: ITRF93      */
/* SE_ETRF89  epoch :: 1999 07 02 :: 1999.500 :: ITRF97      */
/* SF_ETRF89  epoch :: 1997 01 01 :: 1997.000 :: ITRF96      */

/* RETURN PARAMETERS:                                        */
/* o_crd[3] : Transformed i_crd[3]                           */
/* o_vel[3] : Transformed i_vel[3]: NOT ASSIGNED: 0.0        */
/* plate_info-> ::                                           */
/*   plm_trf = 1/0: nnr plate model used/(not used)          */
/*   ipl_trf : {1,2,3} : std gates using:                    */
/*                      {ipl_iJD, ipl_oJD, ipl_iJD & ipl_oJD}*/ 
/*           : {4, 0} : intra plate model {used, not used}   */
/*                      (ipl_iJD & ipl_oJD = -10000000.0)    */
/*   plm_dt   : gate Julian date for Plate Model             */
/*   ipl_idt  : Julian date of input National  (INTRA Plate) */
/*   ipl_odt  : Julian date of output National (INTRA Plate) */
/*   plm_nam is the name of the actual nnr plate model       */
/*   plt_nam is the name of the actual plate                 */
/*   ipl_nam is the name of the actual intra plate model       */

/* err_str: at ERROR: user_txt cat ERROR description         */


/* itrf_trans  ver 2007.02          # page 3    10 Oct 2008 11 57 */


/* tr_par == NULL gives transformations only                 */
/* tr_par != NULL && stn_vel !=0 gives -6000: ILLEGAL        */
/* tr_par != NULL && stn_vel ==0                             */
/*           gives transformations and 7-par values          */
/*           in tr_par: T1, T2, T3, R1, R2, R3, D            */
/*           se formula below                                */

/*         TRANSFORMATION PRODUCTION LINE   */
/*         <--UP-HILL--> <--DOWN-HILL-->    */
/* cstm:     PRJ   CRT     CRT    PRJ       */
/* action:     GDTR    CTR    GDTR          */
/* action:       0      1       2           */

/* basic transformation formula (Molodensky) ::                   */
/* (XS)   (X)   (T1)   ( D  -R3  R2) (X)                          */
/* (YS) = (Y) + (T2) + ( R3  D  -R1)*(Y)                          */
/* (ZS)   (Z)   (T3)   (-R2  R1  D ) (Z)                          */

#endif

