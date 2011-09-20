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
 


/* i_tabdir_file  ver 2010.01          # page 1    15 Feb 2010 15 35 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

/* FILE *i_tabdir_file(mode, *name, *result, *dir_path)        */
/*      opens the file for reading                             */
/*      some files are TESTED                                  */
/*      some GLOBAL PARAMS are initialized                     */
/*  returns the file pointer opened ready for reading          */
/* sevice routine for label and table system                   */

/*    result ==  0 : ok                                        */
/*    result == -1 : name neither found in local nor geoid dir */
/*    result == -2 : name may NOT be opened                    */
/*    result == -3 : FILE TEST FAILED                          */

/*    mode         : kind of file to be opened                 */
/*    mode   ==  1 : manager.tab     TEST,  GLOBAL PAR:        */
/*                 :  *man_tab_file, init_tab_pos              */
/*    mode   ==  2 : manager.gps     TEST,  GLOBAL PAR:        */
/*                 :  *man_gps_file, init_gps_pos              */
/*    mode   ==  3 : def_lab.txt     TEST,  GLOBAL PAR:        */
/*                 :  *def_lab_file, init_lab_pos              */
/*                 :  init_prj_pos, init_rgn_pos               */
/*                 :  init_dtm_pos, init_grs_pos               */
/*    mode   ==  4 : binary file NO TEST                       */
/*    mode   ==  5 : txt file  TEST: <name> (excl. extension)  */
/*    mode   ==  6 : txt file  NO TEST                         */

/*   *name         : NOT USED in mode 1 | 2 |  3               */
/*                 : else the name of the file to be opened    */
/*   *dir_name     : path to the file ( == "" when local)      */

#include <stdio.h>

extern FILE      *i_tabdir_file(
/*____________________________*/
int               mode,
char             *name,
int              *result,
char             *dir_name
);
