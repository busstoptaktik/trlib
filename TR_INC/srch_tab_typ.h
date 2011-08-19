
/* srch_tab_typ     ver 2010.1        # page 1   18 Feb 2010 10 19 */


/* Copyright (c) 2009, National Space Institute, DTU, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of DTU, Denmark */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */

/* srch_tab_typ(mode, f777, *tab_typ_txt) searches the entry   */
/* to a table type                                             */
/*  result  >  0  : tab_typ f777                               */
/*  result == -2  : STOP or EOF                                */
/*                                                             */
/* *lab_typ       : Kind of table (or STP_LAB) See geo_lab.h   */
/*                                                             */
/* *tab_typ_txt may be :: (acording to *lab_typ)               */
/*        "geoid", "dtmtab", "dhtab", "upltab", "demtab",      */
/*        "tabtab", "bagtab", "t3dtab", "t2dtab", "tddtab"     */
/* *pos           : after "REC_TABLE" for next recursive call  */


extern int               srch_tab_typ(
/*__________________________________*/
int                     *lab_typ,
char                    *tab_typ_txt,
long                    *pos
);
