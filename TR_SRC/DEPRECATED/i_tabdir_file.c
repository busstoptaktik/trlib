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

#include    <stdio.h>
#include    <string.h>
#include    <ctype.h>
#include    <sys/stat.h>
#include    "KmsFncs.h"
#include    "trthread.h"

THREAD_SAFE    FILE               *def_lab_file = (FILE *) NULL;
THREAD_SAFE    FILE               *man_tab_file = (FILE *) NULL;
THREAD_SAFE    FILE               *man_gps_file = (FILE *) NULL;
THREAD_SAFE    size_t             init_lab_pos = 0;
THREAD_SAFE    size_t             init_prj_pos = 0, init_rgn_pos = 0;
THREAD_SAFE    size_t             init_dtm_pos = 0, init_grs_pos = 0;
THREAD_SAFE    size_t             init_hth_pos = 0;
THREAD_SAFE    size_t             init_tab_pos = 0, init_gps_pos = 0;


FILE             *i_tabdir_file(
/*____________________________*/
int               mode,
char             *name,
int              *result,
char             *dir_name
)
{

  char               *global_dir = NULL;
  char                pth_mlb[512], *p_c;
  char                p_name[32], t_name[32], r_mode[4];
  int                 test = 1;
  size_t             *init_pos = (size_t *) NULL, init_pos_a = 0, size;
  struct stat         buf;
  FILE              **f_in = (FILE **) NULL, *f_in_a = NULL;

  (void) strcpy(r_mode, "rb"); //Binary mode used to support uniform line endings \n across platforms!
  (void) strcpy(p_name, name);

  switch (mode) {
  case 1:  // manager.tab
    f_in     = &man_tab_file;
    init_pos = &init_tab_pos;
    (void) strcpy(p_name, "manager.tab");
    (void) strcpy(t_name, "managertab");
    break;
  case 2:  // manager.gps
    f_in     = &man_gps_file;
    init_pos = &init_gps_pos;
    (void) strcpy(p_name, "manager.gps");
    (void) strcpy(t_name, "managergps");
    break;
  case 3:  // def_lab.txt
    f_in     = &def_lab_file;
    init_pos = &init_lab_pos;
    (void) strcpy(p_name, "def_lab.txt");
    (void) strcpy(t_name, "def_lab");
    break;
  case 4:  // binary no test
    f_in     = &f_in_a;
    init_pos = &init_pos_a;
    test     = 0;
    (void) strcpy(r_mode, "rb");
    break;
  case 5:  // a txt file test <name>
    f_in     = &f_in_a;
    init_pos = &init_pos_a;
    (void) strcpy(t_name, p_name);
    p_c  = strchr(t_name, '.');
    if (p_c != NULL) *p_c = '\0';
    break;
  case 6:  // a txt file no test
    f_in     = &f_in_a;
    init_pos = &init_pos_a;
    test     = 0;
    break;
  }

  if (*f_in == NULL) {
    /* Look for p_name in user's directory */
    /* or         in <global_dir>/<p_name> */
    size  = stat(p_name, &buf) ? 0L : buf.st_size;
    (void) strcpy(pth_mlb, p_name);

    *dir_name = '\0'; /* directory: local */
    if (size == 0L) {
      /* Look for grid in public grid directory */
      global_dir = gettabdir();
#if defined(sparc) || defined(__sparc)
      if (*global_dir == '\0') global_dir = settabdir(NULL);
#endif
      (void) strcpy(pth_mlb, global_dir);
      (void) strcat(pth_mlb, p_name);
      size = stat(pth_mlb, &buf) ? 0 : buf.st_size;
      if (size == 0L) {
        *init_pos = 0;
        *result   = -1;
        return (NULL);
      }
      (void) strcpy(dir_name, global_dir);
    } /* directory: dir_grid */

    /* open the file */
#ifndef _WIN32
    if ((*f_in = fopen(pth_mlb, r_mode)) == NULL) {
      *init_pos = 0; *result = -2; return (NULL); }

#else
    if ((*f_in = fopen(pth_mlb, r_mode)) == NULL) {
      *init_pos = 0; *result = -2; return (NULL); }
#endif
  }
  else (void) fseek(*f_in, (long) *init_pos, SEEK_SET);
  
  if (test && *init_pos == 0) {
    *result = fgetlhtx(*f_in, pth_mlb);
    if (mode == 3) {  // def_lab.txt
      init_prj_pos = 0;
      init_rgn_pos = 0;
      init_dtm_pos = 0;
      init_grs_pos = 0;
      init_hth_pos = 0;
    }
    if (*result == 0) {
      if (!strcmp(pth_mlb, t_name)) {
        *init_pos = ftell(*f_in);
        if (mode == 3) {  // def_lab.txt
          *result = fgetlhtx(*f_in, pth_mlb);
          if (!strcmp(pth_mlb, "def_prj"))
              init_prj_pos = ftell(*f_in);
          else {
            *result = -3;
            return(NULL);
          }
        }
        return (*f_in);
      } else {
        *init_pos = 0;
        *result   = -3;
        (void) fclose(*f_in);
        *f_in     = NULL;
        return (NULL);
      }
    }
  }
  else *result = 0;
  return(*f_in);
}
