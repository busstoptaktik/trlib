#if !defined     (H_TRANSFORMATIONS)
#define           H_TRANSFORMATIONS
#include "metadata.h"


/*special transformations*/
int dk_trans(struct coord_lab*,struct coord_lab*,double,double,double,double*,double*,double*,tab_dir*);       
int fe_trans(struct coord_lab*,struct coord_lab*,double,double,double,double*,double*,double*,tab_dir*);  
int fh_trans(struct coord_lab*,struct coord_lab*,double,double,double,double*,double*,double*,tab_dir*);  
int ng_trans(struct coord_lab*,struct coord_lab*,double,double,double,double*,double*,double*,tab_dir*);  
int ee_trans(struct coord_lab*,struct coord_lab*,double,double,double,double*,double*,double*,tab_dir*);  

/*generic transformations*/


#endif