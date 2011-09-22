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
 


/* c_tabdir_file  ver 2010.01          # page 1    15 Feb 2010 15 35 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

/* c_tabdir_file(mode, f_in) closes the file pointed by f_in   */
/*      some GLOBAL PARAMS are initialized to zero             */
/*                                                             */
/* sevice routine for label and table system                   */

/*    result ==  0 : ok                                        */

/*    mode         : kind of file to be opened                 */
/*    mode   ==  0 : one short call for mode = 1, 2, 3         */
/*    mode   ==  1 : manager.tab            GLOBAL PAR:        */
/*                 :  *manager_file = NULL, init_man_pos = 0   */
/*    mode   ==  2 : manager.gps            GLOBAL PAR:        */
/*                 :  *man_gps_file = NULL, init_gps_pos = 0   */
/*    mode   ==  3 : def_lab.txt            GLOBAL PAR:        */
/*                 :  *def_lab_file = NULL, init_lab_pos = 0   */
/*                 :  init_prj_pos = 0, init_rgn_pos = 0       */
/*                 :  init_dtm_pos = 0, init_grs_pos = 0       */
/*    mode   ==  4 : binary file                               */
/*    mode   ==  5 : txt file                                  */
/*    mode   ==  6 : txt file                                  */

/*   *f_in         : NOT USED  in mode 0 | 1 | 2 | 3           */
/*                 : else the file pointer to be closed        */

#include <stdio.h>
#include "trthread.h"

extern THREAD_SAFE  FILE               *def_lab_file;
extern  THREAD_SAFE  FILE               *man_tab_file;
extern THREAD_SAFE FILE               *man_gps_file;
extern THREAD_SAFE size_t              init_lab_pos;
extern THREAD_SAFE size_t              init_prj_pos, init_rgn_pos;
extern THREAD_SAFE  size_t              init_dtm_pos, init_grs_pos;
extern THREAD_SAFE size_t              init_tab_pos, init_gps_pos;

void              c_tabdir_file(
/*____________________________*/
int               mode,
FILE             *f_in
)
{


/* c_tabdir_file  ver 2010.01            # page 2     5 feb 2010 15 35 */


  if (mode == 0 || mode == 1) { // manager.tab
    if (man_tab_file != NULL) {
      (void) fclose(man_tab_file);
      man_tab_file = NULL;
    }
    init_tab_pos = 0;
  }
  if (mode == 0 || mode == 2) { // manager.gps
    if (man_gps_file != NULL) {
      (void) fclose(man_gps_file);
      man_gps_file = NULL;
    }
    init_gps_pos = 0;
  }
  if (mode == 0 || mode == 3) { // def_lab.txt
    if (def_lab_file != NULL) {
      (void) fclose(def_lab_file);
      def_lab_file = NULL;
    }
    init_lab_pos = 0;
    init_prj_pos = 0;
    init_rgn_pos = 0;
    init_dtm_pos = 0;
    init_grs_pos = 0;
  } 
  if (mode > 2) {
    if (f_in != NULL) {
      (void) fclose(f_in);
      f_in = NULL;
    }
  } 

  return;
}
