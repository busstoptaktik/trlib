

/* conv_cnd  ver 2010.01            # page 1   12 jan 2010 15 35 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

/* conv_cnd(name, p_lb, lab) Service function for conv_lab     */
/* NB: should be called only from conv_lab                     */
/*    result ==  0 : no lab                                    */
/*    result ==  9 : obs lab               in lab              */
/*    result == 16 : cond lab              in lab              */
/*    UNDEFSYS     : unrecognized name                         */
/* an unrecognized name is considered a filename containing a  */
/* genuine label                                               */

/* progr. Karsten Engsager, 2005.                              */
/* updt.  Karsten Engsager, 2010.                              */

#include "geo_lab.h"

extern int               conv_cnd(
/*______________________________*/
char                    *lab_info,
struct lab_def_str      *p_lb,
struct obs_lab          *o_lab,
...
);

