#include    <stdio.h>
#include    <string.h>
#include    <ctype.h>
#include    <sys/stat.h>
#include    <stdarg.h>
#include    <math.h>

#include    "KmsFncs.h"
#include	"lord.h"

static int srch_def(int mode, char *p_sys, char *w_sys, struct lab_def_str *p_lb);

int conv_w_crd(char *mlb, struct coord_lab *c_lab){
	int lab_lng,res=-1,h_type=-1;
	short lab_rgn;
	struct lab_def_str         lb_ln_ex, *p_lb = &lb_ln_ex;
	char                      *p_dtm = p_lb->mlb2;
	char                      *p_sys = p_lb->name;
	char                      *h_mlb, *h_mlb2;
	char w_sys[MLBLNG];
	lab_lng = get_mlb(mlb,&lab_rgn, p_sys, &p_lb->sepch, p_dtm, &h_mlb);
	res = srch_def(CRD_LAB, p_sys, w_sys, p_lb);
	if (res==0){
		res=conv_crd("",p_lb,c_lab,mlb+lab_lng);
		if (res==CRD_LAB){
			 if (*(c_lab->mlb + c_lab->sepix) == 'H' && h_mlb != NULL){
				      struct hgt_lab  hh_lab;
				      int res_h=-1;
				      /* p_lb has been saved to c_lab */
				      p_lb->lab_rgn = 0; 
				      lab_lng       = get_mlb(h_mlb, &lab_rgn, p_sys,
						      &p_lb->sepch, p_dtm, &h_mlb2);

				      /* search the def_lab_file for p_sys (or w_sys) */ 
				      res_h = srch_def(HGT_LAB, p_sys, w_sys, p_lb);
				      if (res_h < 0){
					      c_lab->lab_type = ILL_LAB;
					      return(ILL_LAB);
				      }
				      if (p_lb->lab_type == HGT_LAB)
					  h_type = (short) conv_hgt("", p_lb, &hh_lab, h_mlb);
				      if (h_type == HGT_LAB) {
					p_lb->lab_type = CRD_LAB;  // RESET
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
		
		
	}
	 return res;
}

static int srch_def(
  int                 srch_type,
  char               *p_sys,
  char               *w_sys,
  struct lab_def_str *p_lb){



  char                       p_name[MLBLNG];
  int                        qr, res = -1, cha_str;
  int                        n_prj=0; 
  int                        l_cstm, l_mode, l_type;
  union rgn_un               rgn_pref, dum_rgn;
  extern def_data *DEF_DATA;
  def_projection *prj;
  
  
  if (DEF_DATA==NULL)
	return -2;
 
  (void) strcpy(dum_rgn.prfx, "ZZ");
  
   /* Replace digits by ?'s in w_sys (i.e. UTM32 -> UTM?? etc.), for easier search */
   for (qr = 0; *(p_sys+qr); qr++)
	*(w_sys+qr) = (isdigit(*(p_sys+qr))) ? '?' : *(p_sys+qr);
   *(w_sys+qr) = '\0';
   

  do {
	prj=DEF_DATA->projections+(n_prj++);
	strcpy(p_name,prj->mlb);
	cha_str=prj->cha_str;
	l_type=prj->type;
	l_cstm=prj->cstm;
	l_mode=prj->mode;
	qr = (!strcmp((cha_str) ? w_sys : p_sys, p_name));
        
	if (qr && l_type == srch_type) {
		(void) strcpy(p_lb->name, p_sys);
		p_lb->lab_type = (short) l_type;
		p_lb->d_kind   = (short) l_cstm;
		p_lb->d_type   = (short) l_mode;
		p_lb->cha_str  = (short) cha_str;
		p_lb->mask     = (short) prj->mask;
		strcpy(rgn_pref.prfx,prj->seq);
		if (dum_rgn.r_nr[0] != rgn_pref.r_nr[0])
			(void) strcpy(p_lb->a_seq, rgn_pref.prfx);
		else *p_lb->a_seq = '\0';
		strcpy(rgn_pref.prfx,prj->rgn);
		p_lb->lab_rgn = (dum_rgn.r_nr[0] != rgn_pref.r_nr[0])
			      ? rgn_pref.r_nr[0] : 0;

		strcpy(p_name,prj->p_datum);
		
		if (*p_name != '\"') {
			(void) strcpy(p_lb->pr_dtm, p_name);
			if (p_lb->sepch == '_') p_lb->sepch = 0;
		}
		else *p_lb->pr_dtm = '\0';
		
		/* datum has been read to p_lb->mlb2 (hopefully) */
		
		p_lb->q_par = (char) prj->q_par;

		if (prj->param_tokens[0]!='\"') {
			(void) strcpy(p_lb->add_p, prj->param_tokens);
		}
		else *p_lb->add_p = '\0';
		
		strcpy(p_name,prj->native_proj);
		if (p_lb->q_par) (void) strcpy(p_lb->par_str,prj->param_text);

		strcpy(p_lb->text,prj->descr); 
		res =0;
	   }
    
  } while (res<0  && n_prj<DEF_DATA->n_prj);


  return(res);
}

	     
