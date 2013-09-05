#include    <stdio.h>
#include    <string.h>
#include    "KmsFncs.h"
#include	"lord.h"

int conv_w_tab(char *mlb, struct gde_lab *t_lab){
	struct lab_def_str         lb_ln_ex, *p_lb = &lb_ln_ex;
	strncpy(p_lb->name,mlb,MLBLNG);
	
	return /*of the jedi*/ conv_tab("",p_lb,t_lab,"");
};