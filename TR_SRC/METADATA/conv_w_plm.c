#include <stdio.h>
#include  "KmsFncs.h"
int conv_w_plm(char *mlb, struct plm_lab *p_lab){
	short lab_rgn, lab_lng;
	struct lab_def_str      p_lb;
	char                      *h_mlb;
	int res;
	lab_lng = get_mlb(mlb, &lab_rgn, p_lb.name, &p_lb.sepch, p_lb.mlb2, &h_mlb);
	res = srch_def(PLM_LAB, p_lb.name, &p_lb);
	if (res!=0)
		return res;
	res=conv_plm("", &p_lb, p_lab);
	return res ;
}


