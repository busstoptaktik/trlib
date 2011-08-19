

/* conv_rgn                       # page 1   21 dec 2010 10 36 */


/* Copyright (c) 2009 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

/* Prog: KP OCT 1990                                           */
/* Modified, KE DEC 2009                                       */

#include    <string.h>
#include    <stdio.h>
#include    <sys/stat.h>
#include    <stdarg.h>
#include    "geo_lab.h"

short            conv_rgn(
/*______________________*/
short            rgn_nr,
char            *rgn_prfx,
char            *rgn_name
)
{

/* conv_rgn : info of region-nr, - prefix, and the country og region */
/* rgn_nr   : the internally used number = the num. rep. the prefix  */
/* rgn_prfx : region prefix (2-letter abrev. country name)           */
/* rgn_name : Country                                                */

/* NB: Some old rgn_prfx has been used. Old_prfx is converted to new */

/* RETURN::                                                          */
/* conv_rgn >  0: == rgn_nr                                          */
/* conv_rgn = -1: rgn_nr or rgn_prfx NOT FOUND                       */
/* conv_rgn = -2: def_lab_file (def_lab.txt and "def_rgn") NOT FOUND */

/* CALL ::                                                           */
/* rgn_nr = -1: rgn_prfx is searched to return: conv_rgn, rgn_name   */
/* rgn_nr > 0 : rgn_nr is searched to return: rgn_prfx, rgn_name     */


/* special call: (rgn_nr = -1)                                       */
/*             : and (first char in rgn_prfx is '?' | '!' | '*')     */
/* RETURN::                                                          */
/* conv_rgn = old_rgn_nr                                             */
/* rgn_prfx = NEW_rgn_prfx                                           */
/* CALL ::                                                           */
/* rgn_prfx = "?<region>": rgn_prfx is updated                       */
/* rgn_prfx = "?" | "!" | "*"  : loops over all defined Names        */
/*            when called reiterately                                */
/*            until conv_rgn == -1 signals the end.                  */
/*   ONLY by "*" calls the old_prfx are included                     */


/* NB:: illegal call: conv_rgn(-1, "?", name);                       */

/* NB::   legal call:                                                */
/*   union rgn_un   rgn_new, rgn_old;                                */
/*                                                                   */
/*   (void) strcpy(rgn_new.prfx, "?");                               */
/*   rgn_old.r_nr[1] = 0;                                            */
/*   rgn_old.r_nr[0] = conv_rgn(-1, rgn_new.prfx, name);             */

#include              "fgetln.h"
#include              "fgetlhtx.h"
#include              "i_tabdir_file.h"

extern FILE               *def_lab_file;
extern size_t              init_prj_pos, init_rgn_pos;


  static size_t    pos = 0;
  static int       r_quest = 0;

  char             pth_mlb[512], *p_tp;
  char             w_name[24], *w = w_name;
  int              empty = 1, doc = 0, used;
  int              qr = 1, res = -1;
  union rgn_un     f_rgn, r_rgn;


/* conv_rgn                       # page 2   21 dec 2010 10 36 */


  (void) strcpy(rgn_name, "Undefined");
  if (def_lab_file == NULL || init_prj_pos == 0) {
    (void) i_tabdir_file(3, "", &res, pth_mlb);
    if (res) {
      (void) fprintf(stdout, 
             "\n*** set_rgn: def_lab.txt not found ;\n");
      if (rgn_nr > 0) *rgn_prfx = '\0';
      return (-2);
    }
  } else res = 0;

  (void) fseek(def_lab_file, (long) init_rgn_pos, SEEK_SET);
  if (!init_rgn_pos) { // search "def_rgn"
    qr = 1;
    do {
      res = fgetlhtx(def_lab_file, w_name);
      if (res == 0) {
        if (!strcmp("def_rgn", w_name)) qr = 0;
      }
    } while (qr && res != EOF);
    if (qr) {
      (void) fprintf(stdout, 
         "\n*** set_rgn: def_lab.txt REGION not found ;\n");
      if (rgn_nr > 0) *rgn_prfx = '\0';
      return (-2);
    }
    init_rgn_pos = ftell(def_lab_file);
  }
  if (r_quest <= 0) pos = init_rgn_pos;
  /* region definition file detected */

  if (rgn_nr == -1) {
    (void) strcpy(w_name, rgn_prfx);
    switch(*w_name) {
    case '!': /* list of National prefixes */
    case '?': /* documentation     */
    case '*': /* documentation     */
      doc = 1;
      if (!strlen(++w)) {
        if (r_quest > 0)
	    (void) fseek(def_lab_file, (long) pos, SEEK_SET);
        else {
          pos     = init_rgn_pos;
          r_quest = -1;
        }
        doc   =  2;
        empty = -1;
      } else {
        r_quest = 0;
      }

      break;
    default: /* terminate scan */
      break;
    }

    /* search for region prefix */
    do {
      if (qr != EOF) qr = fgetln(pth_mlb, &used, def_lab_file);
      if (qr != EOF) {
        (void) sscanf(pth_mlb, "%s%n", r_rgn.prfx, &used);
        if (!strcmp(rgn_prfx, r_rgn.prfx) || doc == 2) {
          empty = 0;
          p_tp  = pth_mlb + used;
          (void) sscanf(p_tp, "%s%n", f_rgn.prfx, &used);
          p_tp += used;
          (void) sscanf(p_tp, "%s%n", rgn_name, &used);
        }
      }
      if (doc) {
        if (qr == EOF || strcmp(r_rgn.prfx, "ZZ") == 0) {
          pos     = init_rgn_pos;
          r_quest = 0;
          return(-1);
        } else
        if (*w_name!= '*' && r_rgn.r_nr[0]!= f_rgn.r_nr[0] && doc==2) {
          empty = 1; // skip old prefixes
        } else {
          r_quest = 1;
          pos     = ftell(def_lab_file);
          (void) strcpy(rgn_prfx, f_rgn.prfx);
          return(r_rgn.r_nr[0]);
        }
      }
    } while (empty && strcmp(r_rgn.prfx, "ZZ"));
    if (empty == 0) return(f_rgn.r_nr[0]);
  } else
  if (rgn_nr > 0) { 
    /* search for region number */
    r_rgn.r_nr[0] = 0;
    do {
      if (qr != EOF) qr = fgetln(pth_mlb, &used, def_lab_file);
      if (qr != EOF) {
        (void) sscanf(pth_mlb, "%s%n", r_rgn.prfx, &used);
        if (rgn_nr == r_rgn.r_nr[0]) {
          empty  = 0;
          p_tp = pth_mlb + used;
          (void) sscanf(p_tp, "%s%n", f_rgn.prfx, &used);
          (void) strcpy(rgn_prfx, f_rgn.prfx);
          p_tp  += used;
          (void) sscanf(p_tp, "%s%n", rgn_name, &used);
          qr = (int) (strlen(rgn_name) -1);
          if (*(rgn_name +qr) == '\n') *(rgn_name +qr) = '\0';
          return(f_rgn.r_nr[0]);
        }
      }
    } while (empty && strcmp(r_rgn.prfx, "ZZ"));
  }

  (void) strcpy(rgn_name, "undef National prefix");
  return(-1);
}
