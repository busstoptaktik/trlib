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

int                      htr_init(
/*______________________________*/
struct coord_lab        *i_clb,
struct coord_lab        *o_clb,
union  geo_lab          *htr_lab,
struct htr_c_str        *htr_const,
char                    *dh_table_name,
char                    *dh_tr_info
)
{
 
  //static THREAD_SAFE  short          sh_dtm = 0;
  union  geo_lab        test_lab;
  char                *p_tp;
  int                   tr_type = HTRF_ILLEG_, l_inv;
  short                 p_no, i_hdtm, o_hdtm, rgn;
  char                  i_nm[MLBLNG], p_nm[MLBLNG], e_nm[MLBLNG];
  char                  o_nm[MLBLNG];
  struct dsh_str        trp;
  union rgn_un          DK_rgn, FO_rgn, GR_rgn, DE_rgn;
  char dummy_rgn[3];
  short imit;
  extern def_data *DEF_DATA;
  def_hth_tr *htr_def;
  int n_hth=0;

  if (i_clb->h_dtm < 200 || 299 < i_clb->h_dtm ||
      o_clb->h_dtm < 200 || 299 < o_clb->h_dtm) return(HTRF_ILLEG_);

  (void) strcpy(DK_rgn.prfx, "DK");
  (void) strcpy(FO_rgn.prfx, "FO");
  (void) strcpy(GR_rgn.prfx, "GR");
  (void) strcpy(DE_rgn.prfx, "DE");



  if (DEF_DATA==NULL)
	return -1;

 
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
  /* idt for DK h_dtms */
  if (i_hdtm == 203 || i_hdtm == 204 || i_hdtm == 206) i_hdtm = 209;
  if (o_hdtm == 203 || o_hdtm == 204 || o_hdtm == 206) o_hdtm = 209;

  if (i_hdtm == o_hdtm) tr_type = 0;
  else {
    
    (void) set_dtm_1(i_hdtm, i_nm, &p_no, p_nm, e_nm, dummy_rgn,&imit,&trp);
    (void) set_dtm_1(o_hdtm, o_nm, &p_no, p_nm, e_nm, dummy_rgn,&imit, &trp);
  

    do {
      
      htr_def=DEF_DATA->hth_entries+(n_hth++);
	
      (void) conv_lab(htr_def->from_mlb, &test_lab, "");
      if (test_lab.u_c_lab.lab_type == 1) {
      
        l_inv = test_lab.u_c_lab.h_dtm == o_hdtm;
        if (test_lab.u_c_lab.h_dtm == i_hdtm || l_inv) {
        
            if (!strcmp((l_inv) ? i_nm : o_nm, htr_def->to_dtm)) { // FOUND
              // The grid_val accepts NO height label part::
              p_tp = test_lab.u_c_lab.mlb+test_lab.u_c_lab.sepix;
              if (test_lab.u_c_lab.sepix+2 == test_lab.u_c_lab.h_ix)
                *(test_lab.u_c_lab.mlb+test_lab.u_c_lab.sepix)   = '\0';
              else {
                *(test_lab.u_c_lab.mlb+test_lab.u_c_lab.sepix)   = '_';
                *(test_lab.u_c_lab.mlb+test_lab.u_c_lab.h_ix -1) = '\0';
              }
              htr_const->inv   = l_inv;
              *htr_lab         = test_lab;
              //sh_dtm           = o_hdtm;
            
	      tr_type=htr_def->type;
            
              htr_const->LAT0  = htr_def->B0; 
            
              htr_const->LON0  = htr_def->L0; 
              // params ::
           
              
                switch (tr_type) {
                case 1: // dh_table
                  strncpy(dh_table_name,htr_def->table,MAX_TABLE_LEN); 
                break;
                case 2: // Constant
                  htr_const->a1 =htr_def->constants[0]; 
                break;
                case 3: // Linear
                  htr_const->M0  = htr_def->constants[0];
                
                  htr_const->N0  = htr_def->constants[1]; 
                 
                  htr_const->a1  = htr_def->constants[2]; 
                  
                  htr_const->a2  = htr_def->constants[3]; 
               
                  htr_const->a3  = htr_def->constants[4]; 
                  o_hdtm         = (htr_const->inv) ? i_hdtm : o_hdtm;
                }
              
            } 
            
            if (tr_type > 0) // reference
		    strncpy(dh_tr_info, "Not available.",127); /*TODO: preparse and add this */
         
          
        } else {
         
           strncpy(dh_tr_info, "Not available.",127); 
        }
      } else test_lab.u_c_lab.lab_type = STP_LAB;
    } while (test_lab.u_c_lab.lab_type != STP_LAB && tr_type <= 0 && n_hth<DEF_DATA->n_hth);
  }

  if (tr_type < 0) {
    htr_lab->u_c_lab.h_dtm    = 0;
    htr_lab->u_c_lab.lab_type = ILL_LAB;
  }

  return(tr_type);
}
