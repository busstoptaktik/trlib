

/* scn_crd.h                        # page 1   4 feb 2009 16 33 */


/* Copyright (c) 2009, National Space Institute, DTU, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of DTU, Denmark */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */

#include   "geo_lab.h"

extern int               scn_crd(
/*_____________________________*/
char                    *p_in,
struct coord_lab        *d_lab,
double                  *crd,
struct typ_dec          *tpd,
double                  *cmt,
double                  *old_cmt
);

  /* Prog. KE, DEC 1993                        */
  /* scn_crd returns 0 for succes and 1 for    */
  /* failure                                   */

  /* p_in:          input string (from fgetln) */
  /* d_lab:         the actual input label     */
  /* crd:           pointer to coordinates[3]  */
  /* tpd:           pointer to typedecimals[2] */
  /* cmt, old_cmt:  pointer to commentwords[2] */

