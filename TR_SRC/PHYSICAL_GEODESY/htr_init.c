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
#include    <string.h>
#include    <math.h>
#include    "KmsFncs.h"

int find_geoid_seq(int i_hdtm, int o_hdtm, int MAX, GRIM *seq, int *n_steps, int *inv){
    do {
      
      dtm_shift=DEF_DATA->dtm_shifts+(n_dtm_shifts++);

      if ((i_hdtm==dtm_shift->ih_dtm) && (o_hdtm==dtm_shift->oh_dtm)){
         tr_type = 1;
         l_inv   = 0;
      } else
      if ((i_hdtm==dtm_shift->oh_dtm) && (o_hdtm==dtm_shift->ih_dtm)){
         tr_type = 1;
         l_inv   = 1;
      }
      
      else if ((i_hdtm == 202 && o_hdtm==200) || (o_hdtm == 202 && i_hdtm==200)){
      /*todo: def_data shall have a geoid seq standard, which is the same as something that is already standard */
	      
	      } /* msl */

      
      
      
      if (tr_type> -1) { /* FOUND */
        htr_const->inv[n_steps_dh]  = l_inv;
        htr_const->dh[n_steps_dh]= dtm_shift->g;
	if(htr_const->n_steps_dh==0)
		res =  conv_w_crd(grim_crs(dtm_shift->g, "proj_mlb"), htr_lab, DEF_DATA);

	htr_const->n_steps_dh++;
        break;
	}
      
    } while (n_dtm_shifts<DEF_DATA->n_dtm_shifts); /*Prepared for more steps, only one now (20131107)*/
  }
  
  
  
int                      htr_init(
/*______________________________*/
struct coord_lab        *i_clb,
struct coord_lab        *o_clb,
struct coord_lab        *htr_lab,
struct htr_route        *htr_const,
tab_dir                 *tdir
)  
{
 
  def_data             *DEF_DATA;
  char                 *p_tp;
  int                   tr_type = HTRF_ILLEG_, l_inv;
  short                 p_no, i_hdtm, o_hdtm, rgn;
  char                  i_nm[MLBLNG], p_nm[MLBLNG], e_nm[MLBLNG];
  char                  o_nm[MLBLNG];
  struct dsh_str        trp;
  union rgn_un          DK_rgn, FO_rgn, GR_rgn, DE_rgn;
  char dummy_rgn[3];
  short imit;
  def_dtm_shift *dtm_shift;
  int n_dtm_shifts=0;
  htr_const->n_steps_in=0;
  htr_const->n_steps_dh=0;
  htr_const->n_steps_out=0;

  if (i_clb->h_dtm < 200 || 299 < i_clb->h_dtm ||
      o_clb->h_dtm < 200 || 299 < o_clb->h_dtm) return(HTRF_ILLEG_);

  (void) strcpy(DK_rgn.prfx, "DK");
  (void) strcpy(FO_rgn.prfx, "FO");
  (void) strcpy(GR_rgn.prfx, "GR");
  (void) strcpy(DE_rgn.prfx, "DE");
 


  if (!tdir || !(tdir->def_lab))
	return -1;
  DEF_DATA=tdir->def_lab;
 
  //sh_dtm = -1;
  /* find datum name from datum number */
  i_hdtm = i_clb->h_dtm;
  o_hdtm = o_clb->h_dtm;
  /* Ellipsoidal h or CRT search parent */
  rgn = i_clb->p_rgn;
  if (rgn == 0) {
    rgn = o_clb->p_rgn;
    if (rgn == 0) {
      rgn = i_clb->region;
      if (rgn == 0) rgn = o_clb->region;
    }
  }
  if (i_hdtm == 200 && o_hdtm != 200) {
    if (i_clb->imit == FHMASK || o_clb->imit == FHMASK)
        i_hdtm = 213 /*fcsvr10*/;
    else
    if (rgn == DK_rgn.r_nr[0]) i_hdtm = 208 /*dvr90*/;
    else
    if (rgn == DE_rgn.r_nr[0]) i_hdtm = 215 /*dhhn92*/;
    else  /* FO_MSL before 2009 is named:: foldmsl (== 216) */
    if (rgn == FO_rgn.r_nr[0]) i_hdtm = 211; /*fvr09*/
    else
    if (rgn == GR_rgn.r_nr[0]) i_hdtm = 202; /* msl */
    else i_hdtm = 202; /* msl */;
  }
  if (i_hdtm == 202) { /* msl */ 
    if (o_hdtm == 208 || rgn == DK_rgn.r_nr[0]) i_hdtm = 208;
    else
    if (o_hdtm == 211 || rgn == FO_rgn.r_nr[0]) i_hdtm = 211;
  }
  rgn = o_clb->p_rgn;
  if (rgn == 0) {
    rgn = i_clb->p_rgn;
    if (rgn == 0) {
      rgn = o_clb->region;
      if (rgn == 0) rgn = i_clb->region;
    }
  }
  if (o_hdtm == 200 && i_hdtm != 200) {
    if (i_clb->imit == FHMASK || o_clb->imit == FHMASK)
        o_hdtm = 213 /*fcsvr10*/;
    else
    if (rgn == DK_rgn.r_nr[0]) o_hdtm = 208 /*dvr90*/;
    else
    if (rgn == DE_rgn.r_nr[0]) o_hdtm = 215 /*dhhn92*/;
    else
    if (rgn == FO_rgn.r_nr[0]) o_hdtm = 211; /*fvr09*/
    else
    if (rgn == GR_rgn.r_nr[0]) o_hdtm = 202; /* msl */
    else o_hdtm = 202; /* msl */;
  }
  if (o_hdtm == 202) { /* msl */
    if (i_hdtm == 208 || rgn == DK_rgn.r_nr[0]) o_hdtm = 208;
    else
    if (i_hdtm == 211 || rgn == FO_rgn.r_nr[0]) o_hdtm = 211;
  }

  if (i_hdtm == o_hdtm) tr_type = 0;
  else

  tr_type=find_geoid_seq(i_hdtm, o_hdtm, htr_const->geoid_in, 1, &htr_const->n_steps_in, htr_const->inv);
  
  if (tr_type < 0) {
    htr_lab->lab_type = ILL_LAB;
  }

  find_geoid_seq(i_hdtm, 200, htr_const->geoid_in, 10, &htr_const->n_steps_in, &htr_const->inv_in);
  find_geoid_seq(200, o_hdtm, htr_const->geoid_out, 10, &htr_const->n_steps_out, &htr_const->inv_out);
  
  
  return tr_type;
}


