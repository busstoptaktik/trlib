

/* conv_hgt    ver 2010.1          # page 1   12 jan 2010 10 19 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

/* conv_hgt(name, p_lb_ex, lab) fills data in the label        */
/*    see conv_lab.h                                           */
/* an unrecognized name is considered a filename containing a  */
/* genuine label                                               */

#ifndef  _CONV_HGT_FNC_
#define  _CONV_HGT_FNC_

#include "geo_lab.h"

extern int               conv_hgt(
/*______________________________*/
char                    *lab_info,
struct lab_def_str      *p_lb,
struct hgt_lab          *h_lab,
...
);

/* service function called from conv_lab

Prog: Knud Poder, FEB 1995
      K. Engsager, updated JAN 1999
      K. Engsager, updated JAN 2010
*/

#endif

