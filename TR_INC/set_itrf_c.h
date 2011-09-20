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

#include        "geo_lab.h"
#include        "geoid_d.h"

extern int            set_itrf_c(
/*_____________________________*/
struct coord_lab        *from_lb,
struct coord_lab        *to_lb,
double                  *i_JD,
int                      stn_vel,
int                     *z_o_vel,
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
);

  /* set_itrf_c initializes the transformations constants to i_date.
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
  set_itrf_c == ITRF_NAM_  : manager.gps table_name (gpstab) not found.
  set_itrf_c == ITRF_SYS_  : manager.gps file not found.
  */


/* set_itrf_c  ver 2007.02      # page 2    12 aug 2009 17 23 */


  /*
  i_JD       input coord epoch
  stn_vel    stn_velocities given after coordinates
  *z_o_vel   shows that o_vel should be set zero (*z_o_vel > 0)

  gps_table->
          seq[0:3]: trf_nr[i] :
  idt_tr  :0:  identical
  itrf_tr :1:  itrfyy -> itrfxx  ( + maybe itrfgate->igs_gate)
  igs__tr :2:  igs_yy -> igs_xx  ( + maybe igs_gate->itrfgate)
  igatetr :3:  itrfyy -> etrfyy or itrfgate -> nkg_euref89
  ogatetr :4:  itrfyy -> etrfyy or itrfgate -> nkg_euref89
  i_ipl_dt  :  ref_date for intra plate velocity
  o_ipl_dt  :  ref_date for intra plate velocity

  *tab_t    :: nnr_plate_vel name
  *plm_lb   :: label for tab_t
  *tab_i    :: intra_plate_vel name
  *ipl_table:: table and label for tab_i
  *req_plm_tr :: 0: none, +1: requested
  *req_ipl_tr :: 0: none, >0: requested in seq[i]==*req_ipl_tr
  *plm_dd   :: transformation period
  *ipl_dd   :: transformation period
  *s_used_plm_nam :: plm name saved
  *s_used_plm_nam :: ipl name saved
  *err_str  :: error info is placed in this string

  region: (from_lb/to_lb)->rgn  and ETRF89 ::
        = {DK/NO/SF/SV/GR} : National ETRF89
        = EU : the EUREF recommandations are used
       else ITRF PLATE and INTRA PLATE MODELS are used

  Transformation sequence
  I_GATE_TR  ITRF_TR   IGS_TR   ITRF_TR  O_GATE_TR
          0        1        2         3          4

  I_GATE_TR may be ::
            gr96 gate  to ITRF_gate                     i_zz > -1
      ngk_etrf89 gate  to ITRF_gate                  inkg_ii  1
          etrfyy gate  to ITRFyy                     ieur_ii  1
           wgs84 gate  to ITRF_gate                     i_zz > -1

  O_GATE_TR may be ::
            gr96 gate  to ITRF_gate                     o_zz > -1
      ngk_etrf89 gate  to ITRF_gate                  onkg_ii  1
          etrfyy gate  to ITRFyy                     oeur_ii  1
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
  
