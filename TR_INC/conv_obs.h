

/* conv_obs  ver 2010.1            # page 1   12 jan 2010 15 35 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

/* conv_obs(name, p_lb, lab) Service function for conv_lab     */
/* NB: Should be called d from conv_lab only                   */
/*    result ==  0 : no lab                                    */
/*    result ==  9 : obs lab               in lab              */
/*    UNDEFSYS     : unrecognized name                         */
/* an unrecognized name is considered a filename containing a  */
/* genuine label                                               */

#ifndef  _CONV_OBS_FNC_
#define  _CONV_OBS_FNC_

#include "geo_lab.h"

extern int               conv_obs(
/*______________________________*/
char                    *lab_info,
struct lab_def_str      *p_lb,
struct obs_lab          *o_lab,
...
);

/*  SEE conv_lab.h for details

service function called from conv_lab : OBS labels

Prog: Knud Poder, FEB 1995
      K. Engsager, updated jan 1999
      K. Engsager, updated jan 2010

*/

#endif

