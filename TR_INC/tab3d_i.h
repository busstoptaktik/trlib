/*
 * Copyright (c) 2011, National Survey and Cadastre, Denmark
 * (Kort- og Matrikelstyrelsen), kms@kms.dk
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 */
 


/* tab3d_i  ver 2001           # page 1   24 Jun 2004 11 37 */


/* Copyright (c) 1997 GD, Kort & Matrikelstyrelsen, Denmark    */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort &  */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#ifndef H_TAB3D_I
#define H_TAB3D_I


extern int               tab3d_i(
/*____________________________*/
char                    *tab3d_n,
int                      tab3d_tp,
struct mtab3d_str       *tab_table,
char                    *err_str
);

/* tab3d_i must be called for all kinds of 2d or 3d tables        */
/* before any call of tab3d_val                                    */


/* see allso tab3d_d.h                                         */

/* tab3d_i prepares the table given by the tab3d_n name for    */
/* further calls of tab3d_val                                  */
/* the stream to the table is opened and initialization of the */
/* table labels are performed automatically.                   */
/* Closing of the stream is done by tab3d_c.                   */

/* Hint to the user ::                                         */
/* T3D_LAB (tab3d tables) will open the tab3d                  */
/* T2D_LAB (tab2d tables) will open the tab2d                  */
/* When more kinds of tables are used simultaineously it is    */
/* recommended to used seperate mtab3d_structures for each     */

/* tab3d_i return value :                                      */
/*   > 0          :: number of tables initialized              */
/*  == TRF_G_NON_ :: number of tables initialized              */
/*  == TRF_G_MAX_ :: too many tables to initialize             */

/* tab3d_i: Initializes a structure containing one tab3d       */
/* administration and array locations for pages of tables,     */
/* and variables needed for computation of table values.       */


/* tab3d_i  ver 2001           # page 2   24 Jun 2004 11 37 */


/* for tab3ds optionally ::                                    */
/* tab3d_n:  Pointer to the full path and name of a file con-  */
/*           taining a table.                                  */
/* If tab3d_n is empty or is "STD" the standard is used.       */    
/* for non_tab3ds tables mandatory ::                          */
/* tab3d_n:  Pointer to the full path and name of a file con-  */
/*           taining a table.                                  */

/* tab3d_tp:  must be one of the types (se geo_lab.h)::        */
/*                T3d_LAB, T2d_LAB                             */

/* tab_table: Pointer to a structure (defined in geo_lab.h)    */
/*           containing:                                       */
/*     TAB3DPAGES pages holding parts of the tables            */
/*           after a fifo-strategy. A page has room for        */
/*     TAB3DPAGESIZE floats.                                   */
/*     tab_max tab3d_labels. (MAX_GEOIDS)                      */
/*           containing (1) the name and path of the file with */
/*           the table, (2) a table-label, (3) a FILE variable */
/*           for the stream of the file (the file is open by   */
/*           the call), (4) a counter for number of calls for  */
/*           data, and (5) a boolean for the existence         */
/*           Note, that the streams should be closed after the */
/*           use by means of the function tab3d_c              */

/* err_str:  will hold the error explanation when errors occur */


/* FILE USED IN THE MANAGING OF TABLES ::                      */

/* manager.tab is a file placed in the same catalog as the     */
/*           grids. I must contain the official information    */
/*           on grid names and search sequences                */

/* example of manager.tab file ::                              */
/*   #tab3d   manager   977         :: info for identification */
/*   REC_TABLE                      :: start of tab3d_names    */
/*   aitken.04  2  0  4  3  geo_euref89  "DK98/1    1.5x3.0 mn" */
/*   stop                           :: last of tab3d_names     */
/*   :: each line contains name  cmplt  global                 */
/*   ::     (length of float)  decimals  projection   text     */
/*                                                             */
/*   STD  DK  FE                    :: list of preceedencies   */
/*                                                             */
/*   STD                            :: start of standard       */
/*   aitken.04                                                 */
/*   stop                           :: last of standard        */
/* example of manager.tab file (continued on next page) ::     */


/* tab3d_i  ver 2001           # page 3   24 Jun 2004 11 37 */


/* example of manager.tab file (continued) ::                  */

/*                                                             */
/*   DK                             :: start of Danmark        */
/*   aitken.04                                                 */
/*   stop                           :: last of Danmark         */
/*                                                             */
/*   FE                             :: start of Faroes         */
/*   stop                           :: last of Faroes          */
/*                                                             */
/*   #stop    manager                                          */

/* example of manager.tab file :: FINISHED                     */

/* A private manager.tab may allso exist.                      */
/* and the official one is not cunsulted                       */
/* The private manager.tab is used to name a new tab3d ::      */
/* example of private manager.tab file ::                      */
/*   #tab3d   manager   977         :: info for identification */
/*   REC_TABLE                      :: start of tab3d_names    */
/*   aitken.04  2  0  4  3  geo_euref89  "DK98/1    1.5x3.0 mn" */
/*   stop                           :: last of tab3d_names     */
/*                                                             */
/*   STD                            :: list of preceedencies   */
/*                                                             */
/*   STD                            :: start of standard       */
/*   stop                           :: last of standard        */
/*                                                             */



/* tab3d_i  ver 2001           # page 4   24 Jun 2004 11 37 */


/*                                                             */
/*           The last part of mgde_str contains variables used */
/*           for i/o, numbering etc. See geo_lab.h for the     */
/*           valid definitions.                                */
/*                                                             */
/*  int                  hit;           tab3d-height in page   */
/*  int                  agn;           tab3d-height to page   */
/*  int                  xmt;           hole in tab3d-area     */
/*  long                 pge[GEOIDPAGES]; act. stream for page */
/*  int                  sgd[GEOIDPAGES]; act. nmb. of tab3d   */
/*  int                  victim;          fifo choice of page  */
/*  int                  tab_max;         number of tab3ds     */

/*  Params set by the progr. or funct. calling the trnsf. func.*/
/*  int                  init;            0 => initialize      */
/*  int                  tab_nr;           actual tab3d        */


/* Prog: Knud Poder, JULY 1997, updated OCT 1997, upd JUN 1998 */
/* Prog: Karsten Engsager, OCT 2001.                           */

#endif

