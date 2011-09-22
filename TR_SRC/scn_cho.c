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
 


/* scn_cho    ver 2003.01         # page 1    4 Jul 2003 16 33 */


/* Copyright (c) 2003 GEK, Kort-og Matrikelstyrelsen, Denmark   */
/* All rights reserved.                                         */

/* This is unpublished proprietary source code of GD, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include   <stdio.h>
#include   <string.h>
#include   "geo_lab.h"
#include   "trthread.h"

int                      scn_cho(
/*_____________________________*/
char                    *p_in,
union geo_lab           *u_lab,
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

  struct coord_lab                 *c_lab = &(u_lab->u_c_lab);

  char                             *p_stop;
  struct typ_dec                   Nt, Et;
  struct st_str                    stn;
  int                              t_char;

  int                              type, r_val = 1;
  int                              dec = 3, used;

  static THREAD_SAFE  short                     iN, iE, q_c, grp1, grp2, grp3;
  static THREAD_SAFE  int                       c_check;
  static THREAD_SAFE  struct typ_dec            id_tpd;
  static THREAD_SAFE  char                      *id_ch, *id_hh;


  p_stop = p_in + (int) strlen(p_in) - 2;
  if (p_stop < p_in) return(r_val);



/* scn_cho    ver 2003.01         # page 2    4 Jul 2003 16 33 */

  switch(c_lab->lab_type) {

  case CRD_LAB:
  case HGT_LAB:
    if (c_check != c_lab->ch_tsum) {
      c_check = c_lab->ch_tsum;
      if (c_lab->cstm != 2) { /*rect. coord. */
        id_ch  = "m";
        id_tpd = *set_tpd(id_ch, 0, dec);
        grp1   = grp2 = grp3 = id_tpd.gf;
        if (c_lab->cstm == 36) { /* hpot */
          id_hh  = "gpu";
          id_tpd = *set_tpd(id_hh, 0, dec);
          grp1   = grp2 = grp3 = id_tpd.gf;
        }
      }
      else { /* geog. coord. */
        id_tpd = *set_tpd("m", 0, dec);
        grp3   = id_tpd.gf;
        id_tpd = *set_tpd("cc", 0, dec + 2);
        grp2   = id_tpd.gf;
        id_ch  = "sx";
        id_tpd = *set_tpd(id_ch, 0, dec + 2);
        grp1   = id_tpd.gf;
      }
      iN  = c_lab->p_seq;
      iE  = (short) (1 - iN);
      q_c = c_lab->ncoord;
      if (q_c == 3) {
        id_hh = (c_lab->hstm == 36) ? "gpu" : "m";
        id_tpd = *set_tpd(id_hh, 0, dec + 2);
        grp3   = id_tpd.gf;
      }
    }

    /* input of coord records */
    /* _______________________*/

    switch(q_c) {

    case 1 : /* Height */
      *crd  = sgetg(p_in, &Nt, &used, id_hh);
      if (Nt.gf != grp1) {
        type = sgetst(p_in, c_lab->region, &stn, &t_char, &used);
        if (type == TERM && t_char >= 'a') r_val = t_char;
        return(r_val);
      }
      break;

    case 2 : /* N, E (or y,x) */
      *(crd +iN) = sgetg(p_in, &Nt, &used, id_ch);
      if (Nt.gf != grp1 && Nt.gf != grp2) {
        type = sgetst(p_in, c_lab->region, &stn, &t_char, &used);
        if (type == TERM && t_char >= 'a') r_val = t_char;
        return(r_val);
      }
      p_in      += used;
      if (p_stop < p_in) return(r_val);
      *(crd +iE) = sgetg(p_in, &Et, &used, id_ch);
      if (   (Nt.gf != grp1 && Nt.gf != grp2)
          || (Et.gf != grp1 && Et.gf != grp2)) return(r_val);
      if (Nt.gf == Et.gf && Nt.tf == Et.tf) {
        if (Et.ff > Nt.ff) Nt.ff = Et.ff;
        if (Et.df > Nt.df) Nt.df = Et.df;
      }
      break;



/* scn_cho    ver 2003.01         # page 3    4 Jul 2003 16 33 */


    case 3 : /* N, E, H  (or XYZ) */
      *(crd +iN) = sgetg(p_in, &Nt, &used, id_ch);
      if (Nt.gf != grp1 && Nt.gf != grp2) {
        type = sgetst(p_in, c_lab->region, &stn, &t_char, &used);
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
      *(crd +2)  = sgetg(p_in, tpd+1, &used, id_hh);
      if ((tpd+1)->gf != grp3) return(r_val);
      break;

    default: /* FEJL */
      return(r_val);
    }
    *tpd = Nt;

    p_in += used;
    if (p_in < p_stop) {
      *cmt  = sgetcw(p_in, &used, &t_char);
/*
printf("\ncmt input = %+15.1f", *cmt);
*/
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
    break; /* case CRD_LAB */ /* case HGT_LAB */

  default:
    /* ALARM ACTION */
    break;
  }
  return(0);

}


