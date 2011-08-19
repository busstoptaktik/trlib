

/* ch_tabdir_par  ver 2010.01          # page 1    15 Feb 2010 15 35 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

/* ch_tabdir_par(mode, **s_f, *s_p, *n_f, n_p) saves some      */
/*      PARAMS to (**s_f, *s_p)                                */
/*      and asigns new values from (*n_f, n_p)                 */
/*                                                             */
/* sevice routine for label and table system                   */

/*    result ==  0 : ok                                        */

/*    mode         : kind of file to be opened                 */
/*    mode   ==  1 : *manager_file, init_man_pos               */
/*    mode   ==  2 : *man_gps_file, init_gps_pos               */
/*    mode   ==  3 : *def_lab_file, init_lab_pos               */
/*                 :  init_prj_pos = n_p, init_rgn_pos = 0     */
/*                 :  init_dtm_pos = 0, init_grs_pos = 0       */

#include <stdio.h>

extern void       ch_tabdir_par(
/*____________________________*/
int               mode,
FILE            **s_f,
size_t           *s_p,
FILE             *n_f,
size_t            n_p
);
