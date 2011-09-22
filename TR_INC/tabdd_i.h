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
 


/* tabdd_i  ver 2006           # page 1   24 Jul 2006 11 37 */


/* Copyright (c) 2006 Kort & Matrikelstyrelsen, Denmark    */
/* and Danish National Space Center                        */
/* All rights reserved.                                    */

/* This is unpublished proprietary source code of Kort &  */
/* Matrikelstyrelsen, Denmark, and Danish National Space  */
/* Center.  This copyright claim does not indicate an     */
/* intention of publishing this code.                     */

#ifndef H_TABDD_I
#define H_TABDD_I


extern int               tabdd_i(
/*____________________________*/
char                    *tabdd_n,
int                      tabdd_tp,
struct mtabdd_str       *tab_table,
char                    *err_str
);

/* tabdd_i must be called for double kinds of 2d tables        */
/* before any call of tabdd_val                                */


/* see allso tabdd_d.h                                         */

/* tabdd_i prepares the table given by the tabdd_n name for    */
/* further calls of tabdd_val                                  */
/* the stream to the table is opened and initialization of the */
/* table labels are performed automatically.                   */
/* Closing of the stream is done by tabdd_c.                   */

/* Hint to the user ::                                         */
/* TDD_LAB (tabdd tables) will open the tabdd                  */
/* When more kinds of tables are used simultaineously it is    */
/* recommended to used seperate mtabdd_structures for each     */

/* tabdd_i return value :                                      */
/*   > 0          :: number of tables initialized              */
/*  == TRF_G_NON_ :: number of tables initialized              */
/*  == TRF_G_MAX_ :: too many tables to initialize             */

/* tabdd_i: Initializes a structure containing one tabdd       */
/* administration and array locations for pages of tables,     */
/* and variables needed for computation of table values.       */


/* tabdd_i  ver 2001           # page 2   24 Jul 2006 11 37 */


/* for tabdds optionally ::                                    */
/* tabdd_n:  Pointer to the full path and name of a file con-  */
/*           taining a table.                                  */
/* If tabdd_n is empty or is "STD" the standard is used.       */    
/* for non_tabdds tables mandatory ::                          */
/* tabdd_n:  Pointer to the full path and name of a file con-  */
/*           taining a table.                                  */

/* tabdd_tp: must be the type (se geo_lab.h):: TDD_LAB         */

/* tab_table: Pointer to a structure (defined in geo_lab.h)    */
/*           containing:                                       */
/*     TABDDPAGES pages holding parts of the tables            */
/*           after a fifo-strategy. A page has room for        */
/*     TABDDPAGESIZE doubles.                                   */
/*     tab_max tabdd_labels. (MAX_GEOIDS)                      */
/*           containing (1) the name and path of the file with */
/*           the table, (2) a table-label, (3) a FILE variable */
/*           for the stream of the file (the file is open by   */
/*           the call), (4) a counter for number of calls for  */
/*           data, and (5) a boolean for the existence         */
/*           Note, that the streams should be closed after the */
/*           use by means of the function tabdd_c              */

/* err_str:  will hold the error explanation when errors occur */


/* FILE USED IN THE MANAGING OF TABLES ::                      */

/* manager.tab is a file placed in the same catalog as the     */
/*           grids. I must contain the official information    */
/*           on grid names and search sequences                */

/* example of manager.tab file ::                              */
/*   #tabdd   manager   977         :: info for identification */
/*   REC_TABLE                      :: start of tabdd_names    */
/*   aitken.04  2  0  4  3  geo_euref89  "DK98/1    1.5x3.0 mn" */
/*   stop                           :: last of tabdd_names     */
/*   :: each line contains name  cmplt  global                 */
/*   ::     (length of float)  decimals  projection   text     */
/*                                                             */
/*   STD  DK  FE                    :: list of preceedencies   */
/*                                                             */
/*   STD                            :: start of standard       */
/*   aitken.04                                                 */
/*   stop                           :: last of standard        */
/* example of manager.tab file (continued on next page) ::     */


/* tabdd_i  ver 2001           # page 3   24 Jul 2006 11 37 */


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
/* The private manager.tab is used to name a new tabdd ::      */
/* example of private manager.tab file ::                      */
/*   #tabdd   manager   977         :: info for identification */
/*   REC_TABLE                      :: start of tabdd_names    */
/*   aitken.04  2  0  4  3  geo_euref89  "DK98/1    1.5x3.0 mn" */
/*   stop                           :: last of tabdd_names     */
/*                                                             */
/*   STD                            :: list of preceedencies   */
/*                                                             */
/*   STD                            :: start of standard       */
/*   stop                           :: last of standard        */
/*                                                             */



/* tabdd_i  ver 2001           # page 4   24 Jul 2006 11 37 */


/*                                                             */
/*           The last part of mgde_str contains variables used */
/*           for i/o, numbering etc. See geo_lab.h for the     */
/*           valid definitions.                                */
/*                                                             */
/*  int                  hit;           tabdd-height in page   */
/*  int                  agn;           tabdd-height to page   */
/*  int                  xmt;           hole in tabdd-area     */
/*  long                 pge[GEOIDPAGES]; act. stream for page */
/*  int                  sgd[GEOIDPAGES]; act. nmb. of tabdd   */
/*  int                  victim;          fifo choice of page  */
/*  int                  tab_max;         number of tabdds     */

/*  Params set by the progr. or funct. calling the trnsf. func.*/
/*  int                  init;            0 => initialize      */
/*  int                  tab_nr;           actual tabdd        */


/* Prog: Karsten Engsager, JUL 2006.                           */

#endif

