

/* conv_plm    ver 2010.1          # page 1   12 jan 2010 10 19 */


/* Copyright (c) 1995  GD,  Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

/* conv_plm(name, p_lb, lab, ...) Service function for conv_lab */
/* result == PLM_LAB : no_net_rotation PLate Motion lab         */
/* ERROR :                                                      */
/* result == ITRF_NON_ : NO entries found.                      */
/* result == ITRF_MAX_ : too many entries found. (> MAX_ENT_PLM)*/
/* result == ITRF_NAM_ : manager.gps table_name (gps_tab) not found.*/
/* result == ITRF_SYS_ : manager.gps file not found.            */

/* name == "input" | "" | "output" | "OUTPUT", "SURVEY"         */
/* will load the plate velocities for the model specified i p_lb*/
/* name == "output" | "OUTPUT" | "SURVEY"                       */
/* will write the label on the o_file: ...                      */
/*      (with info: CAPITAL) (survey: SURVEY)                   */

/* The struct lab_def_str is defined in geo_lab.h in order to   */
/* make it available as a parameter for conv_crd, conv_hgt, etc.*/

/* p_lb->name == "nnnr_"                                        */
/* p_lb->mlb2 == <model>                                        */
/* the plate motions model nnr_<model> is searched and loaded   */
/* the name: nnr_std gives the newest (first found) model       */

/* conv_plm initializes the plm_lab : p_lab with the            */
/* rotation rates for the  p_lab->plm_max plates p_lab->plm__tr.*/
/* (MAX_ENT_PLM) plates                                         */

#ifndef       _CONV_PLM_FNC_
#define       _CONV_PLM_FNC_

#include "geo_lab.h"

extern int               conv_plm(
/*______________________________*/
char                    *lab_info,
struct lab_def_str      *p_lb,
struct plm_lab          *p_lab,
...
);

/*  SEE also conv_lab.c and conv_lab.h for details                 */

#endif


