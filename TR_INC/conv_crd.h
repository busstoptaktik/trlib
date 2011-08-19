

/* conv_crd    ver 2010.01          # page 1   12 jan 2010 10 19 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

/* conv_crd(name, p_lb, lab) service function for conv_lab     */
/*    result ==  0 : no lab                                    */
/*    result ==  1 : coord lab             in lab              */
/* an unrecognized name is considered a filename containing a  */
/* genuine label                                               */

#ifndef       _CONV_CRD_FNC_
#define       _CONV_CRD_FNC_

#include "geo_lab.h"

extern int            conv_crd(
/*___________________________*/
char                 *lab_name,
struct lab_def_str   *p_lb,
struct coord_lab     *c_lab,
...
);

/*  SEE also conv_lab.c and conv_lab.h for details                 */

/* LABEL TABLE */

/* The struct label_table is defined in geo_lab.h in order to make */
/* it available as a parameter for conv_crd, conv_hgt, etc.        */


/*
service function for conversion of COORD labels

Prog: Knud Poder, FEB 1995
      K. Engsager, updated JAN 1999
      K. Engsager, updated JAN 2010

*/

#endif


