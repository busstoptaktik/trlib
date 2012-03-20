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
 

/* tab_doc_f     ver 2009.01          # page 1   26 Mar 2009 10 19 */


/* Copyright (c) 2009, National Space Institute, DTU, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of DTU, Denmark */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */

/* tab_doc_f(name, fh_out) find the name of a table            */
/*                        and writes some parameters on fh_out */
/*    result  >  0      : grid lab ok (==lab_type, geo_lab.h)  */
/*    result ==  0      : STOP recursive call                  */
/*    result == -1      : name is not found                    */
/*    result == -2      : manager.tab NOT FOUND                */
/*    result == -3      : unknown first char in name           */
/*                                                             */
/*   ?<NAME>   =>  Table name, short description               */
/*   *<NAME>   =>  Table name, detail description              */
/*                                                             */
/*   ?<TYPE>   =>  Table name, short description  (recursive)  */
/*   *<TYPE>   =>  Table name, detail description (recursive)  */
/*                                                             */
/*   ?         =>  Table name, short description  (recursive)  */
/*   *         =>  Table name, detail description (recursive)  */
/*                                                             */
/*   !         =>  works as ?                                  */
/*                                                             */
/* Recursive calls produces information for one table in each  */
/* call until result == 1. Continuing after this will restart  */
/* the loop.                                                   */
/*                                                             */
/* <TYPE> may be ::                                            */
/*        "geoid", "dtmtab", "dhtab", "upltab", "demtab",      */
/*        "tabtab", "bagtab", "t3dtab", "t2dtab", "tddtab"     */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "geo_lab.h"
#include "geoid_d.h"
#include "trthread.h"

int                      tab_doc_f(
/*____________________________*/
char                    *name,
FILE                    *fh_out
)
{


/* tab_doc_f     ver 2009.01          # page 2   26 Mar 2009 10 19 */


#include              "conv_lab.h"
#include              "fgetln_kms.h"
#include              "fgetlhtx.h"
#include              "s_status.h"
#include              "i_tabdir_file.h"
#include              "srch_tab_typ.h"

extern THREAD_SAFE FILE               *man_tab_file;
extern THREAD_SAFE size_t              init_tab_pos;

#define  LAST_TP       (767)
// To be edited when new table types are added

  static THREAD_SAFE  int                 n_quest = 0, type_quest = 0, new_table = 0, stype = 0;
  static THREAD_SAFE  int                 first = 1;
  static THREAD_SAFE  long                init_pos = 0, pos = 0;
  static THREAD_SAFE  char                s_name[24], pth_mlb[512];
  size_t                     b_t;
  char                       datum[MLBLNG], p_dtm[MLBLNG];
  char                       tab_typ_txt[12], *p_nm = s_name +1, *p_tp, *p_te;
  int                        i, r, qr, used, empty = 0;
  int                        f777 = 0;
  union geo_lab              g_lab;
  struct gde_lab            *t_lab = &(g_lab.u_g_lab);

  if (man_tab_file == NULL || init_tab_pos == 0) {
    (void) i_tabdir_file(1, "", &r, pth_mlb);
    if (r) {
      (void) s_status(pth_mlb, "geoid_i", TAB_N_MAN_);
      (void) fprintf(fh_out, "\n%s\n", pth_mlb);
    }
  }
  (void) fseek(man_tab_file, (long) init_tab_pos, SEEK_SET);


/* tab_doc_f     ver 2009.01          # page 2   26 Mar 2009 10 19 */


  p_tp = name +1;
  switch(*name) {
  case '!': /* list of ell-names */
  case '?': /* documentation     */
  case '*': /* full digit docum. */
    b_t = strlen(p_tp);
    if (b_t == 0) {
      if (*name != *s_name || n_quest <= 0) {
        *s_name = *name;
        pos     = (long) init_tab_pos;
        n_quest = -1;
      } 
      else (void) fseek(man_tab_file, (long) pos, SEEK_SET);
      empty      = 1;
      type_quest = 0;
    } else
    if (!type_quest || *name != *s_name ||
        (strcmp(p_tp, p_nm) != 0 && strncmp(p_tp, "grsoek", 6) != 0)) {
      // name search or new table type
      (void) strcpy(s_name, name);
      if (strncmp(p_nm, "grsoek", 6) == 0) {
        (void) strcpy(p_nm, "grsoek.06");
        b_t = 9;
      }
      empty      = 0;
      n_quest    = 0;
      type_quest = 0;
    } else {
      // continue in same table type
      (void) fseek(man_tab_file, (long) pos, SEEK_SET);
    }
    break;
  default:
    pos        = (long) init_tab_pos;
    init_pos   = pos;
    empty      = 0;
    n_quest    = 0;
    type_quest = 0;
    new_table  = 0;
    return(-3);
  }

  if (n_quest <= 0) {
    // find first table type
    f777 = srch_tab_typ(&stype, tab_typ_txt, &init_pos);
    if (stype == STP_LAB) return(-2);

    first = 1;
    if (empty == 0 && 
        strcmp(tab_typ_txt, p_nm) == 0) type_quest = 1;
  }


/* tab_doc_f     ver 2009.01          # page 3   26 Mar 2009 10 19 */


  do {
    if (new_table) {
      f777 = srch_tab_typ(&stype, tab_typ_txt, &pos);
      new_table = 0;
      if (type_quest || f777 == LAST_TP) {
        empty      = 0;
        n_quest    = 0;
        type_quest = 0;
        return((type_quest || n_quest > 0) ? 0 : -1);
      }
      first = 1;
      if (empty == 0 && 
          strcmp(tab_typ_txt, p_nm) == 0) type_quest = 1;
    }
   
    p_tp = pth_mlb;
    if (r != 0) {
      /* look for grid name in REC_TABLE */
      do {
        used = fgetln_kms(pth_mlb, &qr, man_tab_file);
        if (((!strncmp(pth_mlb, p_nm, b_t)) || 
             type_quest) && qr >= 7) {
          // collect name, coord_sys, beskr, possibly params
          (void) sscanf(pth_mlb, "%s%n", p_nm, &used);
          
          if (*s_name == '*') {
            pos = ftell(man_tab_file);
            if (conv_lab(p_nm, &g_lab, "") == stype)
               (void) conv_lab("OUTPUT", &g_lab, fh_out);
            (void) fseek(man_tab_file, (long) pos, SEEK_SET);
          } else {
            if (first) {
              first = 0;
              (void) fprintf(fh_out, "\n\n%s", tab_typ_txt);
            }
            p_tp            = pth_mlb + used;
            (void) sscanf(p_tp, "%d%n", &qr, &used);
            p_tp           += used;
            (void) sscanf(p_tp, "%d%n", &qr, &used);
            p_tp           += used;
            (void) sscanf(p_tp, "%d%n", &qr, &used);
            p_tp           += used;
            (void) sscanf(p_tp, "%d%n", &(t_lab->gd_dec), &used);
            p_tp           += used;
            (void) sscanf(p_tp, "%s%n", p_dtm, &used);
            p_tp           += used;
            (void) sscanf(p_tp, "%s%n", datum, &used);
            p_tp            = strchr(p_tp, '\"') +1;
            p_te            = strchr(p_tp, '\"');
            *p_te           = '\0';
            (void) fprintf(fh_out, "\n%-18s%-16s, %-10s=> %s",
                           p_nm, datum, p_dtm, p_tp);
            r   = 0;
            pos = ftell(man_tab_file);
            if (n_quest > 0) ++ n_quest;
            else
            if (n_quest == -1 || type_quest) n_quest = 1;
          }
        }
      } while (strncmp(pth_mlb, "stop", 4) && r && used > 0);

/* tab_doc_f     ver 2009.01          # page 4   26 Mar 2009 10 19 */


      if (r != 0 && strncmp(pth_mlb, "stop", 4) == 0) {
        new_table = 1;
      }
    
    } else {
      n_quest    = 0;
      type_quest = 0;
      new_table  = 0;
      return(-2);
    }
  } while (stype != STP_LAB && r);

  i = (r == 0) ? stype
    : (type_quest || n_quest > 0) ? 0 : -1;
  // save name to next loop
  if (type_quest) (void) strcpy(s_name, name);
  if (i == 0 && (size_t) init_pos > init_tab_pos) {
    init_pos = (long) init_tab_pos;
    (void) fprintf(fh_out, "\n");
  }
  return(i);
}

