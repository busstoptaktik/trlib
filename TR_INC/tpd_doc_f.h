

/* set_tpd  version 2010.1           # page 1   12 Jan 2010 13 14 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

/* tpd_doc_f returns the tpd_group number (> 0)                */
/* tpd_doc_f should be called until tpd_doc_f == 0             */

/* head_txt descibes the tpd type                              */
/*          (to be written when a new group number appear)     */
/* tpd_txt  descibes the tpd                                   */

/* head_txt must be declared at least 128 chars                */
/* tpd_txt  must be declared at least 128 chars                */


extern int                   tpd_doc_f(
/*___________________________________*/
char                        *head_txt,
char                        *tpd_txt
);

/*
Progr. Karsten Engsager jan 2010
*/
