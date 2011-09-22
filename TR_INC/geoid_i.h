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
 


/* geoid_i  ver 2001           # page 1   31 Oct 2001 11 37 */


/* Copyright (c) 1997 GD, Kort & Matrikelstyrelsen, Denmark    */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort &  */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#ifndef H_GRID_I
#define H_GRID_I


extern int               geoid_i(
/*____________________________*/
char                    *geoid_n,
int                      geoid_tp,
struct mgde_str         *tab_table,
char                    *err_str
);

/* geoid_i must be called for all kinds of tables except geoids   */
/* before any call of grid_val                                    */
/* goids are initialized automatically                            */
/* grid_val will call geoid_i("std", GDE_LAB, tab_table, err_str) */
/* when tab_table->init == 0  !!!                                 */


/* see allso geoid_d.h                         (for dir_geoid) */

/* geoid_i prepares the table given by the geoid_n name for    */
/* further calls of grid_val                                   */
/* the stream to the table is opened and initialization of the */
/* table labels are performed automatically.                   */
/* Closing of the stream is done by geoid_c.                   */

/* Hint to the user ::                                         */
/* GDE_LAB (geoid tables) will open tab_max geoids             */
/* DTM_LAB (Digital Terrain Model tables) will open one table  */
/* DEM_LAB (Digital Ellevation Model tables) will open one tab */
/* DHH_LAB (Diff Height model tables) will open one table ONLY */
/* UPL_LAB (UPLift model tables) will open one table ONLY      */
/* TAB_LAB (unspecified TABles) will open one table ONLY       */
/* When more kinds of tables are used simultaineously it is    */
/* recommended to used seperate mgde_structures for each kind  */

/* geoid_i return value :                                      */
/*   > 0          :: number of tables initialized              */
/*  == TRF_G_NON_ :: number of tables initialized              */
/*  == TRF_G_MAX_ :: too many tables to initialize             */

/* geoid_i: Initializes a structure containing up to MAXGEOIDS */
/* (pt. 9, see geoid_d.h) table labels, administration and     */
/* array locations for pages of tables, and variables needed   */
/* for computation of table values.                            */


/* geoid_i  ver 2001           # page 2   31 Oct 2001 11 37 */


/* for geoids optionally ::                                    */
/* geoid_n:  Pointer to the full path and name of a file con-  */
/*           taining a table. (outside the definition area     */
/*           the standard geoids are used).                    */
/* If geoid_n is empty or is "STD" the standard is used.       */    
/* for non_geoids tables mandatory ::                          */
/* geoid_n:  Pointer to the full path and name of a file con-  */
/*           taining a table.                                  */

/* geoid_tp:  must be one of the types (se geo_lab.h)::        */
/*                GDE_LAB, HML_LAB, DHM_LAB, UPL_LAB, TAB_LAB  */

/* tab_table: Pointer to a structure (defined in geo_lab.h)    */
/*           containing:                                       */
/*     GEOIDPAGES pages holding parts of the tables            */
/*           after a fifo-strategy. A page has room for        */
/*     GEOIDPAGESIZE floats.                                   */
/*     tab_max geoid_labels. (MAX_GEOIDS)                      */
/*           containing (1) the name and path of the file with */
/*           the table, (2) a table-label, (3) a FILE variable */
/*           for the stream of the file (the file is open by   */
/*           the call), (4) a counter for number of calls for  */
/*           data, and (5) a boolean for the existence         */
/*           Note, that the streams should be closed after the */
/*           use by means of the function geoid_c              */

/* err_str:  will hold the error explanation when errors occur */


/* FILE USED IN THE MANAGING OF TABLES ::                      */

/* manager.tab is a file placed in the same catalog as the     */
/*           grids. I must contain the official information    */
/*           on grid names and search sequences                */

/* example of manager.tab file ::                              */
/*   #geoid   manager   777         :: info for identification */
/*   REC_TABLE                      :: start of geoid_names    */
/*   dk98g.01   2  0  4  3  geo_euref89  "DK98/1    1.5x3.0 mn" */
/*   fe95g.01   2  0  4  3  geo_euref89  "Faroes    1.5x3.0 mn" */
/*   nkg96g.01  2  0  4  3  geo_euref89  "NKG96/1   1.5x3.0 mn" */
/*   egm96g.01  2  1  4  1  geo_wgs84    "EGM96  0.5x0.5dg  1 m" */
/*   stop                           :: last of geoid_names     */
/*   :: each line contains name  cmplt  global                 */
/*   ::     (length of float)  decimals  projection   text     */
/*                                                             */
/*   STD  DK  FE                    :: list of preceedencies   */
/*                                                             */
/*   STD                            :: start of standard       */
/*   dk98g.01                                                  */
/*   fe95g.01                                                  */
/*   nkg96g.01                                                 */
/*   egm96g.01                                                 */
/*   stop                           :: last of standard        */
/* example of manager.tab file (continued on next page) ::     */


/* geoid_i  ver 2001           # page 3   31 Oct 2001 11 37 */


/* example of manager.tab file (continued) ::                  */

/*                                                             */
/*   DK                             :: start of Danmark        */
/*   dk98g.01                                                  */
/*   nkg96g.01                                                 */
/*   egm96g.01                                                 */
/*   stop                           :: last of Danmark         */
/*                                                             */
/*   FE                             :: start of Faroes         */
/*   fe95g.01                                                  */
/*   egm96g.01                                                 */
/*   stop                           :: last of Faroes          */
/*                                                             */
/*                                                             */
/*   #dhtab   manager   775                                    */
/*   dhdnn50.01    2   0   4   3   s34j  "GM91 NY-GL 200x200m" */
/*   stop                                                      */
/*                                                             */
/*   *                                                         */
/*   name        COM GLO size dec datum    "descr"             */
/*                PL BAL                                       */
/*   ;                                                         */
/*                                                             */
/*   * list of tables of precedenses (STD is mandatory);       */
/*                                                             */
/*   STD                                                       */
/*                                                             */
/*   * STD is mandatory precedense of grids                    */
/*                                                             */
/*   STD                                                       */
/*   dhdnn50.01                                                */
/*   stop                                                      */
/*                                                             */
/*   #stop    manager                                          */

/* example of manager.tab file :: FINISHED                     */

/* A private manager.tab may allso exist.                      */
/* and the official one is not cunsulted                       */
/* The private manager.tab is used to name a new geoid ::      */
/* example of private manager.tab file ::                      */
/*   #geoid   manager   777         :: info for identification */
/*   REC_TABLE                      :: start of geoid_names    */
/*   dk20g.01   2  0  4  3   euref89  "DK2000    1.5x3.0 mn"   */
/*   stop                           :: last of geoid_names     */
/*                                                             */
/*   STD                            :: list of preceedencies   */
/*                                                             */
/*   STD                            :: start of standard       */
/*   stop                           :: last of standard        */
/*                                                             */



/* geoid_i  ver 2001           # page 4   31 Oct 2001 11 37 */


/*                                                             */
/*           The last part of mgde_str contains variables used */
/*           for i/o, numbering etc. See geo_lab.h for the     */
/*           valid definitions.                                */
/*                                                             */
/*  int                  hit;           geoid-height in page   */
/*  int                  agn;           geoid-height to page   */
/*  int                  xmt;           hole in geoid-area     */
/*  long                 pge[GEOIDPAGES]; act. stream for page */
/*  int                  sgd[GEOIDPAGES]; act. nmb. of geoid   */
/*  int                  victim;          fifo choice of page  */
/*  int                  tab_max;         number of geoids     */

/*  Params set by the progr. or funct. calling the trnsf. func.*/
/*  int                  init;            0 => initialize      */
/*  int                  tab_nr;           actual geoid        */


/* Prog: Knud Poder, JULY 1997, updated OCT 1997, upd JUN 1998 */
/* Prog: Karsten Engsager, OCT 2001.                           */

#endif

