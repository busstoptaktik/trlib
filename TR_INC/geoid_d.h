
/* geoid_d  ver 2004.01           # page 1   25 Jun 2004 11 37 */


/* Copyright (c) 2004 LG, Kort & Matrikelstyrelsen, Denmark    */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of LG, Kort &   */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include  "geo_lab.h"

#if !defined(GEOID_D_H)
#define      GEOID_D_H

/* In this library should allso be found the file :           */
/*       manager.tab                                          */
/* which gives the names of tables and their important adm.   */
/* data and gives the strategy for electing the proper table  */
/* Private tables may be put in the current library           */
/* where a private copy of the manager.tab allso may be found */

#define  GEOIDPGSIZE      16   /* 16 float or 64 bytes        */
#define  TAB3DPGSIZE      48   /* (3d) 16 float               */
#define  TABDDPGSIZE      32   /* (2d) 16 double              */
#define  GEOIDTABHEAD     64   /* Size of head defining table */
#define  GEOIDHEADER       1   /* 1=>compatible with fortran  */
#define  GEOIDPAGES        8   /* 2 is feasible               */
#define  MAXGEOIDS        10   /* Geoids no 0 -(MAXGEOIDS-1)  */
#define  STANDARD_GEOID   "nkg96g.01"
#define  BACKUP_GEOID     "dma84g.b1"

union  tab_pages {
  float            fghtab[GEOIDPGSIZE];     /* g-height as fl. */
/*  unsigned char    bghtab[4*GEOIDPGSIZE]; */  /* g-height + 128  */
};

union  t3d_pages {
  float            fghtab[TAB3DPGSIZE];     /* 3d_val as fl. */
/*  unsigned char    bghtab[4*TAB3DPGSIZE]; */  /*               */
};

union  tdd_pages {
  double           fghtab[TABDDPGSIZE];     /* 2d_val as double */
/*  unsigned char    bghtab[4*TABDDPGSIZE]; */  /*               */
};


/* geoid_d  ver 2004.01           # page 2   25 Jun 2004 11 37 */


/*
geoid_c is IMPORTANT to call before releasing the struct mgde_str
        to close the file pointer used.
+++ IMPORTANT to release file pointers in system +++
gd_trans(NULL, .... will close file pointers to special tables used
+++ IMPORTANT to release file pointers in system +++
*/
struct mgde_str {
  union  tab_pages     g_pg[GEOIDPAGES];  /* pages loaded in RAM  */
  struct gde_lab       table_u[MAXGEOIDS];/* geoid_units no. 0-7  */

  int                  hit;               /* grid_val in page     */
  int                  agn;               /* grid_val to page     */
  int                  xmt;               /* hole in grid-area    */
  long                 pge[GEOIDPAGES];   /* act. stream for page */
  int                  sgd[GEOIDPAGES];   /* act. nmb. of grid    */
  int                  victim;            /* fifo choice of page  */
  int                  tab_max;           /* number of tables     */

  /* Params set by the progr. or funct. calling the trnsf. func.  */
  int                  init;              /* 0 => initialize      */
  int                  tab_nr;            /* actual table         */
};

struct mtab3d_str {
  union  t3d_pages     g_pg[GEOIDPAGES];  /* pages loaded in RAM  */
  struct gde_lab       table_u;           /* geoid_units no. 0-7  */

  int                  hit;               /* grid_val in page     */
  int                  agn;               /* grid_val to page     */
  int                  xmt;               /* hole in grid-area    */
  long                 pge[GEOIDPAGES];   /* act. stream for page */
  int                  sgd[GEOIDPAGES];   /* act. nmb. of grid    */
  int                  victim;            /* fifo choice of page  */
  int                  tab_max;           /* number of tables     */

  /* Params set by the progr. or funct. calling the trnsf. func.  */
  int                  init;              /* 0 => initialize      */
  int                  tab_nr;            /* actual table         */
};

struct mtabdd_str {
  union  tdd_pages     g_pg[GEOIDPAGES];  /* pages loaded in RAM  */
  struct gde_lab       table_u;           /* geoid_units no. 0-7  */

  int                  hit;               /* grid_val in page     */
  int                  agn;               /* grid_val to page     */
  int                  xmt;               /* hole in grid-area    */
  long                 pge[GEOIDPAGES];   /* act. stream for page */
  int                  sgd[GEOIDPAGES];   /* act. nmb. of grid    */
  int                  victim;            /* fifo choice of page  */
  int                  tab_max;           /* number of tables     */

  /* Params set by the progr. or funct. calling the trnsf. func.  */
  int                  init;              /* 0 => initialize      */
  int                  tab_nr;            /* actual table         */
};


/* geoid_d  ver 2004.01           # page 2   25 Jun 2004 11 37 */


/*
grid_val  is a function giving the grid value in the defined system

The structure "mgde_str" serves as the work space for
grid_val. The user should declare a structure as e.g.

     static struct mgde_str        tab_table;

The statement of initializing once before first call:

     tab_table.init = 0;

DEFAULT : grid_val will initialize the geoids
without any other programming action needed.

The call of grid_val, now using and giving only euref89 values
is described in the prototype file grid_val.h

extern int                grid_val(
__________________________________
union  geo_lab          *c_lab_u,
struct mgde_str         *tab_table,
double                   B_in,
double                   L_in,
double                  *tab_val,
char                    *usertxt
);

The return value is
zero     tab_val found.
> 0      recall the function with other euref89 datum (e.g. EE_)
< 0      error


The grid index in the array of structure mgde_str *tab_table
is found in the mtab structure : entry tab_nr,
enabling a report of the name of the actually used grid.

grid_val opens the streams to the tables, and the function geoid_c
closes again and writes information about the processing.

extern int               geoid_c(
_________________________________
struct mgde_str         *tab_table,
int                      info,
FILE                    *out
);

geoid_c is IMPORTANT to call before releasing the struct mgde_str
        to close the file pointer used.
+++ IMPORTANT to release file pointers in system +++

The value of info determines the information given about
the use of the tables, so that
info == 0 suppresses the information and
info != 0 produces it.

The return value is 0 for ok and -1 for failure.
*/


/* geoid_d  ver 2004.01           # page 3   25 Jun 2004 11 37 */


/*
The number of GEOIDPAGES - here actually defined to 8 -
gives a hit-rate of mostly over 90 %. The size of the pages,
GEOIDPGSIZE, is mandatory.

The user may define a selection of a gridtable different from
the series of standard tables automatically initialized by geoid_h.

extern int               geoid_i(
_________________________________
char                    *geoid_n,
int                      geoid_tp,
struct mgde_str         *tab_table,
char                    *err_str
);

geoid_tp gives the type of table (p.t.):
    GDE_LAB, DTM_LAB, DHH_LAB, UPL_LAB, DEM_LAB, TAB_LAB,
    BAG_LAB, T3D_LAB, T2D_LAB, TDD_LAB

FOR GEOIDS ONLY:
The name of the geoidfile may be empty or "STD" or 
any other region defined (ex "DK") else 
The name of the geoidfile is set in the parameter geoid_n. Only one
geoid is permitted. This geoid must be found in the stdin library.
If the position is outside the definition area of geoid_n, then
the standard ("STD")geoids are searched.

FOR OTHER TABLES:
The name of the tablefile may be empty or "STD" or 
any other region defined (ex "DK") else 
The name of the tablefile is set in the parameter geoid_n. Only one
grid is permitted. This grid must be found in the stdin library.
If the position is outside the definition area of geoid_n, then
NO OTHER tables are searched.

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

tab3d_val  is a function giving the grid value in the defined system

The structure "mtab3d_str" serves as the work space for
tab3d_val. The user should declare a structure as e.g.

     static struct mtab3d_str        tab_table;

The statement of initializing once before first call:

     tab_table.init = 0;

DEFAULT : tab3d_val will initialize the tab3ds
without any other programming action needed.

The call of tab3d_val, now using and giving only euref89 values
is described in the prototype file tab3d_val.h
*/


/* geoid_d  ver 2004.01           # page 4   25 Jun 2004 11 37 */


/*
extern int               tab3d_val(
__________________________________
union  geo_lab          *c_lab_u,
struct mtab3d_str       *tab_table,
double                   B_in,
double                   L_in,
double                  *tab_val,
char                    *usertxt,
FILE                    *tr_error
);

The return value is
zero     tab_val found.
> 0      recall the function with other euref89 datum (e.g. EE_)
< 0      error

*tab_val return values: ix: 0 = N_val, 1 = E_val, 2 = H_val

The grid index in the array of structure mtab3d_str *tab_table
is found in the mtab structure : entry tab_nr,
enabling a report of the name of the actually used grid.

tab3d_val opens the streams to the tables, and the function tab3d_c
closes again and writes information about the processing.

extern int               tab3d_c(
_________________________________
struct mtab3d_str       *tab_table,
int                      info,
FILE                    *out
);

tab3d_c may be neglected, because the file is closed when
the programme terminates.
The value of info determines the information given about
the use of the tables, so that
info == 0 suppresses the information and
info != 0 produces it.

The return value is 0 for ok and -1 for failure.
*/


/* geoid_d  ver 2004.01           # page 5   25 Jun 2004 11 37 */


/*
The number of GEOIDPAGES - here actually defined to 8 -
gives a hit-rate of mostly over 90 %. The size of the pages,
TAB3DPGSIZE, is mandatory.


extern int               tab3d_i(
_________________________________
char                    *tab3d_n,
struct mtab3d_str       *tab_table,
FILE                    *tr_error
);

FOR TAB3D TABLES:
The name of the tablefile may be empty or "STD" or 
any other region defined (ex "DK") else 
The name of the tablefile is set in the parameter tab3d_n. Only one
grid is permitted. This grid must be found in the stdin library.
If the position is outside the definition area of tab3d_n, then
NO OTHER tables are searched.


PROG: Knud Poder,  JAN 1995, APR 1995
PROG: K. Engsager, JAN 1999, JUN 2004
*/

#endif

