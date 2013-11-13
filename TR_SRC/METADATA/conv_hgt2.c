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

/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

/* conv_hgt(name, p_lb_ex, lab) fills data in the label        */
/* initiated by conv_lab.                                      */
/*    result ==  0 : no lab                                    */
/*    result ==  2 : height lab            in lab              */
/*    UNDEFSYS     : unrecognized name                         */
/* an unrecognized name is considered a filename containing a  */
/* genuine label                                               */

#include    <stdio.h>
#include    <string.h>


#include    "KmsFncs.h"

int                      conv_hgt2(
struct lab_def_str      *p_lb_ex,
struct hgt_lab          *h_lab,
def_data                  *DEF_DATA
)
{

  

  int                          i;
  short                      par_dtm, mask;
  double                     ell_p[10];
  char                       e_name[24], p_name[24];
  char                       t_lab_a[2*MLBLNG], *t_lab = t_lab_a;
  char                        rgn_name[4];
  char                      *p_sys, *p_dtm, sepch, *p_tp;

  struct typ_dec             m_tpd;
  double                     f;
  struct dsh_str             d_sh;


  if (p_lb_ex->lab_type != HGT_LAB) {
    lord_error(0,LORD("conv_hgt called with wrong lab_type %d"), p_lb_ex->lab_type);
    h_lab->lab_type = ILL_LAB;
    return(ILL_LAB);
  }


  p_dtm  = (p_lb_ex->mlb2);  /* datum */

  

    p_sys = (p_lb_ex->name);  /* name is mlb1 */
    p_dtm = (p_lb_ex->mlb2);  /* datum */

	/* clean lab */
    p_tp = (char *) h_lab;
    for (i = sizeof (*h_lab) - 1; i >= 0; i--) *(p_tp + i) = 0;

    /* label_def_str contain sepch (not sepix) */
    sepch           = (char) p_lb_ex->sepch;
    h_lab->lab_type = p_lb_ex->lab_type;
    h_lab->version  = LAB_VERSION;
    h_lab->cmplt    = (short) ((p_lb_ex->q_par) ? 0 : 1);
    h_lab->cstm     = p_lb_ex->d_kind;
    h_lab->mode     = p_lb_ex->d_type;
    h_lab->region   = 0 ;/* p_lb_ex->region;*/
    h_lab->p_rgn    =0; /* p_lb_ex->lab_rgn;*/
    h_lab->ncoord   = 1;
    h_lab->scale    = 1.0;  /* default */
    h_lab->imit     = 0;    /* default */

    if (sepch != '\0') {
      if (sepch == 'L') {
        (void) strcpy(p_sys, "h");
        (void) strcpy(p_lb_ex->mlb2, "msl");
      } else
      if (sepch != '_') {
        lord_error(0,LORD("illegal separator %c"), sepch);
        h_lab->lab_type = ILL_LAB;
        return(ILL_LAB);
      }
    }

    /* set params of the height systems */
    /*_________________________________*/

    /* PRODUCE THE MINI-LAB  from : mlb1, sepch, dtm */

    /* The minilabel of regular systems with imittance mask of  */
    /* zero contains a concatenation of the coord_system, the   */
    /* separator and the datum. A non-zero imittance mask means */
    /* that only the coord_system is used. A separator != '_'   */
    /* is added to the minilabel, but no datum is given         */

    h_lab->sepix = (short) strlen(p_sys);
    if (*p_lb_ex->pr_dtm == '\0') {
      (void) sprintf(h_lab->mlb, "%s_%s", p_sys, p_dtm);
    }
    else {
      (void) strcpy(h_lab->mlb, p_sys);
      p_dtm = p_lb_ex->pr_dtm;
    }

    /* INPUT OF ADDITIONAL PARAMETERS */
    /*             NONE               */

    /* Set datum */
    h_lab->datum = (short) set_dtm_1(-1, p_dtm, &par_dtm, p_name,
                   e_name, rgn_name, &mask, &(h_lab->dsh_con),DEF_DATA);
    if (h_lab->datum > 0)
      h_lab->ellipsoid = (short) set_grs(-1, e_name, ell_p, DEF_DATA);
    else {
      h_lab->ellipsoid = (short) set_grs(-1, t_lab, ell_p, DEF_DATA);
      h_lab->datum     = -2;
    }

    if (h_lab->ellipsoid >= 0) {
      /* ellipsoid params */
      h_lab->a      = *(ell_p+0);
      h_lab->f      = *(ell_p+1);
      /* datum shift params (from set_dtm) */
      h_lab->p_dtm  = (short) par_dtm;
      h_lab->p_rgn  = 0;
      h_lab->imit   = mask;
      if (h_lab->region == 0) h_lab->region = h_lab->p_rgn;
      /* set trf. constants for completed systems */
      /* if (h_lab->cmplt == 1)                   */
      /*    (void) set_trc((struct coord_lab *) h_lab); */
    }
    else h_lab->lab_type = ILL_LAB; /* unintelligible ellipsoid */

    h_lab->ch_sum = labchsum((union geo_lab *) h_lab, &h_lab->ch_sum);
   



  return(h_lab->lab_type);
}
