#include    <string.h>
#include    <ctype.h>
#include    "KmsFncs.h"
#include	"lord.h"



int conv_w_crd(char *mlb, struct coord_lab *c_lab, def_data *DEF_DATA){
	int lab_lng,res=-1,h_type=-1;
	short lab_rgn;
	struct lab_def_str         lb_ln_ex, *p_lb = &lb_ln_ex;
	char                      *p_dtm = p_lb->mlb2;
	char                      *p_sys = p_lb->name;
	char                      *h_mlb, *h_mlb2;
	lord_debug(0,LORD("mlb in con_w_crd: %s"),mlb);
	lab_lng = get_mlb(mlb,&lab_rgn, p_sys, &p_lb->sepch, p_dtm, &h_mlb);
	lord_debug(0,LORD("p_sys is now: %s, p_dtm"),p_sys, p_dtm);
	res = srch_def(CRD_LAB, p_sys, p_lb,DEF_DATA);
	if (res!=0){
		lord_debug(0,LORD("mlb: %s, not found!"),mlb);
		return res;
	}
	lord_debug(0,LORD("So far so good! mlb: %s, p_sys: %s"),mlb,p_sys);
	res=conv_crd("",p_lb,c_lab,DEF_DATA,mlb+lab_lng);
	lord_debug(0,LORD("So far so good! - res is %d, CRD_LAB: %d"),res,CRD_LAB);
	if (res==CRD_LAB){
		 if (*(c_lab->mlb + c_lab->sepix) == 'H' && h_mlb != NULL){
			      struct hgt_lab  hh_lab;
			      int res_h=-1;
			      /* p_lb has been saved to c_lab */
			      p_lb->lab_rgn = 0; 
			      lab_lng       = get_mlb(h_mlb, &lab_rgn, p_sys,
					      &p_lb->sepch, p_dtm, &h_mlb2);

			      /* search the def_lab_file for p_sys (or w_sys) */ 
			      res_h = srch_def(HGT_LAB, p_sys, p_lb,DEF_DATA);
			      if (res_h < 0){
				      c_lab->lab_type = ILL_LAB;
				      return(ILL_LAB);
			      }
			      if (p_lb->lab_type == HGT_LAB)
				  h_type = (short) conv_hgt("", p_lb, &hh_lab,DEF_DATA, h_mlb); /*hmm - check call*/
			          lord_debug(0,LORD("So far so good! - htyp is %d"),h_type);
			      if (h_type == HGT_LAB) {
				p_lb->lab_type = CRD_LAB;  /* RESET*/
				if (c_lab->imit == 0) c_lab->imit = hh_lab.imit;
				if (c_lab->p_rgn == 0) c_lab->p_rgn = hh_lab.p_rgn;
				if (c_lab->region == 0) c_lab->region = c_lab->p_rgn;
				c_lab->hstm    = hh_lab.cstm;
				c_lab->hmode   = hh_lab.mode;
				c_lab->h_dtm   = hh_lab.datum;
				c_lab->h_ix    = (short) strlen(c_lab->mlb) +1;
				(void) strcat(c_lab->mlb, "_");
				(void) strcat(c_lab->mlb, hh_lab.mlb);
				c_lab->ch_sum = labchsum((union geo_lab*) c_lab, &c_lab->ch_sum);
			      } else {
				c_lab->lab_type = ILL_LAB;
				return(ILL_LAB);
			      }
			 
		 }
	 }
		
		
	
	 return res;
}



	     
