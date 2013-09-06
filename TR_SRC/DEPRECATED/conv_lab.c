#include    <stdio.h>
#include    <string.h>
#include    <ctype.h>
#include    <sys/stat.h>
#include    <stdarg.h>
#include    <math.h>

#include    "KmsFncs.h"
#include    "trthread.h"
#include	"lord.h"
int conv_w_tab(char *mlb, struct gde_lab *t_lab);
int conv_w_crd(char *mlb, struct coord_lab *c_lab);


int               conv_lab(
/*________________________*/
char             *lab_name,
union geo_lab    *u_lab,
...
)
{
	int res;
	struct coord_lab *crd_lab=&u_lab->u_c_lab;
	struct gde_lab    *t_lab=&u_lab->u_g_lab;
	res=conv_w_crd(lab_name, crd_lab);
	if (res==CRD_LAB)
		return res;
	res=conv_w_tab(lab_name,t_lab);
	return res;
}