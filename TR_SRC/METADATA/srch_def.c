#include    <string.h>
#include    <ctype.h>
#include    "KmsFncs.h"


int srch_def(
  int                 srch_type,
  char               *p_sys,
  struct lab_def_str *p_lb,
   def_data *DEF_DATA
  ){



  char                       p_name[MLBLNG], w_sys[MLBLNG];
  int                        qr, res = -1, cha_str;
  int                        n_prj=0; 
  int                        l_cstm, l_mode, l_type;
  union rgn_un               rgn_pref, dum_rgn; /*hmm - get rid of this hack*/
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