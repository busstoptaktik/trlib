

/* scn_crd                        # page 1   04 feb 2009 08 03 */


/* Copyright (c) 2009, National Space Institute, DTU, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of DTU, Denmark */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */

#include   <stdio.h>
#include   <string.h>
#include   <math.h>
#include   "geo_lab.h"

int                      scn_crd(
/*_____________________________*/
char                    *p_in,
struct coord_lab        *d_lab,
double                  *crd,
struct typ_dec          *tpd,
double                  *cmt,
double                  *old_cmt
)

{

#include                 "set_tpd.h"
#include                 "sgetst.h"
#include                 "sgetg.h"
#include                 "sgetcw.h"

  char                            *p_stop, *p_save, c;
  struct hgt_lab                  *h_lab;
  struct typ_dec                   Nt, Et;
  struct st_str                    stn;
  int                              t_char;
  int                              year_n = 0, year_o = 0;

  int                              type, r_val = 1;
  int                              dec = 3, used;

  static short                     iN, iE, grp1, grp2, grp3;
  static int                       q_c;
  static int                       id_check;
  static struct typ_dec            id_tpd;
  static char                     *id_ch;

  p_stop = p_in + (int) strlen(p_in) - 2;
  if (p_stop < p_in)   return(r_val);


/* scn_crd                        # page 2   04 feb 2009 08 03 */


  type  = d_lab->lab_type;
  if (type == 1) type = d_lab->ch_tsum;
  else 
  if (type == 2) {
    h_lab = (struct hgt_lab *) d_lab;
    type  = h_lab->ch_sum;
  } else return(r_val);
    
  if (id_check != type) {
    id_check = d_lab->ch_tsum;
    if (d_lab->cstm == 2) { /* geog. coord. */
      id_tpd = *set_tpd("m", 0, dec);
      grp3   = id_tpd.gf;
      id_tpd = *set_tpd("cc", 0, dec + 2);
      grp2   = id_tpd.gf;
      id_ch  = "sx";
      id_tpd = *set_tpd(id_ch, 0, dec + 2);
      grp1   = id_tpd.gf;
    } else 
    if (d_lab->cstm == 36) { /*gpu  (hpot)  */
      id_ch  = "gpu";
      id_tpd = *set_tpd(id_ch, 0, dec + 2);
      grp1   = grp2 = grp3 = id_tpd.gf;
    } else
    if (d_lab->cstm == 18) { /* gravity */
      id_ch  = "mgal";
      id_tpd = *set_tpd(id_ch, 0, dec);
      grp1   = grp2 = grp3 = id_tpd.gf;
    } else { /*rect. coord. */
      id_ch  = "m";
      id_tpd = *set_tpd(id_ch, 0, 2);
      grp1   = grp2 = grp3 = id_tpd.gf;
    }
    iN  = d_lab->p_seq;
    iE  = (short) (1 - iN);
    q_c = d_lab->ncoord;
  }

  /* input of coord records */
  /* _______________________*/

  switch(q_c) {

  case 1 : /* Height */
    *crd  = sgetg(p_in, &Nt, &used, id_ch);
    if (Nt.gf != grp1) {
      type = sgetst(p_in, d_lab->region, &stn, &t_char, &used);
      if (type == TERM && t_char >= 'a') r_val = t_char;
      return(r_val);
    }
    break;

  case 2 : /* N, E (or y,x) */
    *(crd +iN) = sgetg(p_in, &Nt, &used, id_ch);
    if (Nt.gf != grp1 && Nt.gf != grp2) {
      type = sgetst(p_in, d_lab->region, &stn, &t_char, &used);
      if (type == TERM && t_char >= 'a') r_val = t_char;
      return(r_val);
    }
    p_in      += used;
    if (p_stop < p_in) return(r_val);
    *(crd +iE) = sgetg(p_in, &Et, &used, id_ch);
    if (Et.gf != grp1 && Et.gf != grp2) return(r_val);
    if (Nt.gf == Et.gf && Nt.tf == Et.tf) {
      if (Et.ff > Nt.ff) Nt.ff = Et.ff;
      if (Et.df > Nt.df) Nt.df = Et.df;
    }
    break;

                

/* scn_crd                        # page 3   04 feb 2009 08 03 */


  case 3 : /* N, E, H  (or XYZ) */
    *(crd +iN) = sgetg(p_in, &Nt, &used, id_ch);
    if (Nt.gf != grp1 && Nt.gf != grp2) {
      type = sgetst(p_in, d_lab->region, &stn, &t_char, &used);
      if (type == TERM && t_char >= 'a') r_val = t_char;
      return(r_val);
    }
    p_in      += used;
    if (p_stop < p_in) return(r_val);
    *(crd +iE) = sgetg(p_in, &Et, &used, id_ch);
    p_in      += used;
    if (p_stop < p_in) return(r_val);
    if (   (Nt.gf != grp1 && Nt.gf != grp2)
        || (Et.gf != grp1 && Et.gf != grp2)) return(r_val);
    if (Nt.gf == Et.gf && Nt.tf == Et.tf) {
      if (Et.ff > Nt.ff) Nt.ff = Et.ff;
      if (Et.df > Nt.df) Nt.df = Et.ff;
    }
    *(crd +2)  = sgetg(p_in, tpd+1, &used, "m");
    if ((tpd+1)->gf != grp3) return(r_val);
    break;

  default: /* FEJL */
    return(r_val);
  }
  *tpd = Nt;

  p_in  += used;
  p_save = p_in;
  if (p_in < p_stop) {
    if (q_c == 1 && p_in < p_stop) {
      /* year of measurement, low and high ? */
      *(crd +1) = 0.0;
      *(crd +2) = 0.0;

      (void) sscanf(p_in, "%d%c%n", &year_n, &c, &used);
      /* G.I. comment word or ? */
      if (c != ',' || year_n < 2999) {
        p_in     += used;
        *(crd +1) = (double) year_n;

        if (c == '/' && p_in < p_stop) {
          /* scan for year_h */
          (void) sscanf(p_in, "%d%c%n", &year_o, &c, &used);
          p_in     += used;
          *(crd +2) = (double) year_o;
        }
      }
      if (p_stop < p_in) {
        /* no year of measurement, reset p_in */
        p_in = p_save;
      }
    }

    *cmt  = sgetcw(p_in, &used, &t_char);
    if (*cmt == -1.0) *cmt = *old_cmt;
    if (*cmt == -2.0) return(0);
    *old_cmt  = *cmt;
    p_in     += used;

    if (q_c == 3 && p_in < p_stop) {
      *(cmt +1) = sgetcw(p_in, &used, &t_char);
      if (*(cmt +1) == -1.0) *(cmt +1) = *(old_cmt +1);
      if (*(cmt +1) == -2.0) return(0);
      *(old_cmt +1)  = *(cmt +1);
    }
    else *(cmt +1) = *(old_cmt +1) = 0.0;
  }
  else {
    *(cmt +0) = *(old_cmt +0) = 0.0;
    *(cmt +1) = *(old_cmt +1) = 0.0;
  }
  return(0);

}


