

/* geoid_c                         # page 1   30 Oct 2001 17 25 */


/* Copyright (c) 2001 LG, Kort & Matrikelstyrelsen, Denmark    */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort &   */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */



extern int               geoid_c(
/*____________________________*/
struct mgde_str         *tab_table,
int                      info,
FILE                    *out
);

/* see allso geoid_d.h  and geoid_i.h                            */

/* closes the streams to the FILES used for access to the tables */

/* tab_table: the structure describing the use of the tables     */ 

/* if info != 0 will the names of the tables be listed on the    */
/*      file pointed by *out                                     */
/* if info > 1 will the statistic be listed                      */

/* PROG: Karsten Engsager, OCT 2001 */


