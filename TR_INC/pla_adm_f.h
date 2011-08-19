

/* pla_adm_f    ver 2003.01       # page 1   18 Feb 2003 14 13 */


/* Copyright (c) 2003, Kort-og Matrikelstyrelsen, Denmark      */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GEK, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */


/* pla_adm_f is a maintenance tool for ITRF plates boundaries */
/* the plate boundaries are used to decide which plate is     */
/* appropriate for a point in question                        */
/*                                                            */
/* CALL  ::                                                   */
/* (int) pla_adm_f(                                           */
/*   char  mode,      mode = 'b' / 't' / 'a' / 'l'            */
/*   FILE *i_file     input file_pointer                      */
/*   FILE *d_file     input file_pointer (loop directives)    */
/*                    only used in mode == 'l'                */
/*   FILE *o_file     output file_pointer                     */
/*   char *errtx)     string for error                        */
/*                                                            */
/* mode == 'b'                                                */
/* produces a binary file o_name from the file i_name         */
/*   from the file i_name                                     */
/*                                                            */
/* mode == 't'                                                */
/* produces a text in the file o_name                         */
/*   from the binary file i_name                              */
/*                                                            */
/* mode == 'a'                                                */
/* converts an arc (txt) file i_name                          */
/*   to poly lines Geodisp format (txt) in o_name             */
/*                                                            */
/* mode == 'l'                                                */
/* from the txt file d_name is read the loops directives      */
/* for lines to be selected from the (txt) file i_name        */
/*   to poly loops (txt) in o_name                            */
/*                                                            */

extern int           pla_adm_f(
/*___________________________*/
char                 mode,
FILE                *i_fd,
FILE                *d_fd,
FILE                *o_fd,
char                *err_txt
);

/* progr.:  Karsten Engsager, feb. 2003                       */

