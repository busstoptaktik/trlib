
/* tab_adm_f                       # page 1   17 Dec 2001 22 00 */


/* Copyright (c) 2001, GD, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */


/* tab_adm_f is a maintenance tool for tables and geoids      */
/*                                                           */
/* the label must be found in manager.tab                    */
/*           holding the limits and steps                    */
/*           which are checked against the file              */
/*                                                           */
/* CALL  ::                                                  */
/* (int) tab_adm_f(                                          */
/*   char *tab_name,  name of table                          */
/*   char  mode,      mode = 'b' / 't' / 'd'                 */
/*   int   header,    = 0/1  recommended == 1                */
/*   int   hd_limits, = 0/1  recommended == 1                */
/*   char *global_dir, directory for tables                  */
/*   FILE *in_file,   table is read from in_file (or stdin)       */
/*   FILE *outfile,   control output is written on outfile (or stdout) */
/*   char *errtx)     string for error                       */
/*                                                           */
/* mode == 'b'                                               */
/* produces a binary table in the file tab_name              */
/*   from the file fi, controloutput to fo                   */
/* format of input file : usual as given by Rene Forsberg    */
/*                                                           */
/* mode == 'z'                                               */
/* as -b but all values are set NILL (== 9999.99)            */
/*       the table in fi is not read (only the limits e.t.c.)*/
/*                                                           */
/* mode == 't'                                               */
/* produces a text table in the file out                     */
/*   from the binary file tab_name, controloutput to fo      */
/* format of output file : with header  (== 1)               */
/*                                                           */
/* mode == 'd'                                               */
/* produces a documentation of the table                     */
/*   from the binary file tab_name, controloutput to fo      */

extern int           tab_adm_f(
/*___________________________*/
  char *tab_name,    /*  name of table                                    */
  char  mode,        /* mode = 'b' / 't' / 'd' / 'z'                      */
  int   header,      /* header (in i/o) = 0/1  recommended == 1           */
  int   hd_limits,   /* hd_limits in (i/file) = 0/1  always == 1          */
  char *global_dir,  /* directory for tables                              */
  FILE *in_file,     /* table is read from in_file (or stdin)        */
  FILE *outfile,     /* control output is written on outfile (or stdout)  */
  char *errtx        /* hard errors                                       */
);

/* progr. K.Engsager, Dec, 2001, */

