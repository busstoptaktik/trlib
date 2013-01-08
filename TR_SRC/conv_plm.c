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
 


/* conv_plm    ver 2010.01          # page 1     7 jan 2010 10 19 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>
#include "geo_lab.h"
#include "trthread.h"
#include "lord.h"

int                      conv_plm(
/*______________________________*/
char                    *lab_info,
struct lab_def_str      *p_lb,
struct plm_lab          *p_lab,
...
)
{
/* conv_plm(name, p_lb, lab, ...) Service function for conv_lab */
/* result == PLM_LAB : no_net_rotation PLate Motion lab         */
/* ERROR :                                                      */
/* result == ITRF_NON_ : NO entries found.                      */
/* result == ITRF_MAX_ : too many entries found. (> MAX_ENT_PLM)*/
/* result == ITRF_NAM_ : manager.gps table_name (gps_tab) not found.*/
/* result == ITRF_SYS_ : manager.gps file not found.            */

/* name == "input" | "" | "output" | "OUTPUT", "SURVEY"         */
/* will load the plate velocities for the model specified i p_lb*/
/* name == "output" | "OUTPUT" | "SURVEY"                       */
/* will write the label on the o_file: ...                      */
/*      (with info: CAPITAL) (survey: SURVEY)                   */

/* The struct lab_def_str is defined in geo_lab.h in order to   */
/* make it available as a parameter for conv_crd, conv_hgt, etc.*/

/* p_lb->name == "nnr_"                                         */
/* p_lb->mlb2 == <model>                                        */
/* the plate motions model nnr_<model> is searched and loaded   */
/* the name: nnr_std gives the newest (first found) model       */

/* conv_plm initializes the plm_lab : p_lab with the            */
/* rotation rates for the  p_lab->plm_max plates p_lab->plm__tr.*/
/* (MAX_ENT_PLM) plates                                         */

/* special names :                                             */
/* nnr_plates    : general for plm label giving no pl model    */
/* nnr_itrfxx    : searches the appropriate model for itrfxx   */
/* nnr_igsxx     : searches the appropriate model for igsxx    */
/* example       :                                             */
/* nnr_itrf94    : gives nnr_nuvel1a                           */

/* conv_plm    ver 2010.01          # page 2     7 jan 2010 10 19 */


#include     "fgetln_kms.h"
#include     "fgetlhtx.h"
#include     "gettabdir.h"
#include     "i_tabdir_file.h"
#include     "labchsum.h"
#include     "sgetg.h"
#include     "t_status.h"
extern THREAD_SAFE FILE               *man_gps_file;
extern THREAD_SAFE size_t              init_gps_pos;

#define YEAR(yy) ((yy) +(((yy)<88) ? 2000 : (((yy)<100) ? 1900 : 0)))

  FILE                      *o_file = NULL;
  va_list                    o_p;

  char                       pth_mlb[1024], *p_tp, *p_tp2;
  char                       info[128];
  long                       pos;
  size_t                     size = 0;
  int                        r, qr, used, conv_mode;
  int                        lab_tp, stype = STP_LAB;
  int                        ant = 0, run_ch, run_ii, nnr_std = 0;
  /* local pointers to plm_entry */
  struct plm_c_str          *p_plm = &(p_lab->plm__tr[0]);
  struct typ_dec             tpd;

  /* save lab_info */
  (void) strcpy(info, lab_info);

  conv_mode = (!strcmp(info,  "input")) ? 1 :
              (!strcmp(info, "in_u_#")) ? 2 :
              (!strcmp(info, "output")) ? 3 :
              (!strcmp(info, "OUTPUT")) ? 4 :
              (!strcmp(info, "SURVEY")) ? 5 : 0;

  switch (conv_mode) {
  case 0: /* char input */
  case 1: /* file input */
  case 2: /* char input */
    /* The plm label is created from the plm entries in man_gps_file.gps */

    /* clear the plm_lab structure */
    p_tp = (char *) p_lab;
    for (r = sizeof (*p_lab) - 1; r >= 0; r--) *(p_tp + r) = 0;
    p_lab->lab_type = ILL_LAB;


/* conv_plm    ver 2010.01          # page 3     7 jan 2010 10 19 */


    (void) sprintf(p_lab->mlb, "%s_%s", p_lb->name, p_lb->mlb2);
    if (!strcmp(p_lb->mlb2, "std")) {
      nnr_std = 1;
      run_ch  = sprintf(info, "nnr_  manager  874");
      p_tp2   = info + 4;
      run_ch -=        4;
    } else {
      p_tp2  = info;
      run_ch = sprintf(info, "%s  manager  874", p_lab->mlb);
    }

    /* name must be found in          man_gps_file.gps */
    /* or                 in dir_grid/man_gps_file.gps */

    lab_tp          = PLM_LAB;
    p_lab->version  = LAB_VERSION;
    p_lab->sepix    = (short) strlen(p_lab->mlb);
    p_lab->local    = 1;

    if (strcmp(p_lab->mlb, "nnr_plates")) {

      /* search and load the requested nnr_ model      */
      /* e.g. the Boucher-Altamini model NNR-NUVEL-1A  */
      /* to plm_lab->plm__tr                           */
      if (man_gps_file == NULL || init_gps_pos == 0) {
        (void) i_tabdir_file(2, "", &r, pth_mlb);
        if (r) {
          r = (r == -1) ? ITRF_SYS_ : ITRF_NAM_;
          return (r);
        }
      } else r = 0;
      (void) fseek(man_gps_file, (long) init_gps_pos, SEEK_SET);


/* conv_plm    ver 2010.01          # page 4     7 jan 2010 10 19 */


      do {
        /* Test for identification of a man_gps_file.gps file */
        stype = 0;
        qr    = fgetlhtx(man_gps_file, pth_mlb);
        pos   = ftell(man_gps_file) - (long) strlen(pth_mlb);
        (void) fseek(man_gps_file, pos, SEEK_SET);
        if (qr != EOF) qr = fgetln_kms(pth_mlb, &used, man_gps_file);
        if (qr != EOF) {
          ant = 0;
          for (lab_tp = 874; lab_tp >= 873 && ant == 0; lab_tp --) {
            run_ii = 0;
            switch (lab_tp) {
            case 874:
              /* fgetln_kms gives items with 2<sp> between them */
              if (nnr_std && !strncmp(pth_mlb, info, 4))
                   p_tp = strchr(pth_mlb, ' ');
              else p_tp = pth_mlb;
              if (!strncmp(p_tp, p_tp2, run_ch)) {
                if (nnr_std) {
                  qr = (int) ((ptrdiff_t)strchr(pth_mlb, ' ')
                     - (ptrdiff_t)pth_mlb);
                  (void) strncpy(p_lab->mlb, pth_mlb, qr);
                }
                stype       = lab_tp;
                run_ii      = 1;
                ant         = 0;
                p_lab->f777 = 874;
              }
              break;
            case 873:
              if (!strncmp(pth_mlb, "stop  manager", 13))
                  stype = STP_LAB;
              break;
            default:
              ant     = 0;
            }


/* conv_plm    ver 2010.01          # page 5     7 jan 2010 10 19 */


            if (run_ii) {
              /* look for information in REC_TABLE */
              do {
                qr   = fgetln_kms(pth_mlb, &used, man_gps_file);
                size = (size_t) !strncmp(pth_mlb, "REC_TABLE", 9);
              } while (qr > 0 && (size == 0));

              if (size != 0) {
                /* take the entries in REC_TABLE */
                do {
                  used  = fgetln_kms(pth_mlb, &qr, man_gps_file);
                  if (ant < MAX_ENT_PLM) {
                    p_tp = pth_mlb;
                    size = (size_t) strncmp(pth_mlb, "stop", 4);
                    if (size) {
                      *(p_tp + strlen(p_tp) -1) = '\0';
                      (void) strcpy(p_plm->datum, p_tp);
                      (void) fgetln_kms(pth_mlb, &used, man_gps_file);
                      /* skip from and to dates * /
                      p_tp = strchr(pth_mlb, ',');
                      if (p_tp != NULL) {
                        p_plm->fr_dt =
                               sgetdt(p_tp+1, &date, &used, &qr);
                        p_tp         += used;
                        p_plm->to_dt =
                               sgetdt(p_tp, &date, &used, &qr);
                      }
                      / * skip from and to dates */
                      /* skip for 1 textline */
                      (void) fgetln_kms(pth_mlb, &qr, man_gps_file);

                      p_tp  = pth_mlb;
                      (void) fgetln_kms(pth_mlb, &qr, man_gps_file);
                      p_plm->drx = sgetg(p_tp, &tpd, &used, "prad");
                      p_tp       += used;
                      p_plm->dry = sgetg(p_tp, &tpd, &used, "prad");
                      p_tp       += used;
                      p_plm->drz = sgetg(p_tp, &tpd, &used, "prad");
                      ++ p_plm;
                      ++ ant;
                    } /* size */
                  } /* ant test */
                } while (strncmp(pth_mlb, "stop", 4));
                p_lab->plm_max = (short) ant;
                if (ant > MAX_ENT_PLM) {
                  (void) t_status(stdout,
                                  "", "conv_plm", ITRF_MAX_);
                  return(-1);
                } else
                if (run_ii && (ant == 0 || size == 1)) {
                  (void) t_status(stdout,
                                  "", "conv_plm", ITRF_NON_);
                  return(-1);
                }
              } /* man_gps_file.plm file ok */


/* conv_plm    ver 2010.01          # page 6     7 jan 2010 10 19 */


            } /* man_gps_file main label ok */
          } /* lab_tp loop */
        } /* qr != EOF */
      } while (stype != STP_LAB && ant == 0);

      if (p_lab->plm_max > 0) {
        p_lab->lab_type  = PLM_LAB;
        p_lab->cmplt     = 2;
        p_lab->cstm      = 1; /* crt */
        p_lab->ellipsoid = 1; /* grs80 */
        p_lab->datum     = 6; /* itrf */
      }
    }
    else p_lab->lab_type = PLM_LAB;

    /* set checksum */
    p_lab->ch_sum = labchsum((union geo_lab *) p_lab, &(p_lab->ch_sum));

    break; /* end of case 0 and 1: create a plm_lab */


/* conv_plm    ver 2010.01          # page 7     7 jan 2010 10 19 */


  case 3:  /* output of minilabel, simple */
  case 4:  /* output of minilabel, expand */
  case 5:  /* DOCUMENTATION OF THE LABEL  */

    va_start(o_p, p_lab);
    o_file = va_arg(o_p, FILE *);
    va_end(o_p);

    /* check the plm_lab */
    switch (p_lab->lab_type) {
    case PLM_LAB:
      if (p_lab->ch_sum != labchsum(
                         (union geo_lab *) p_lab, &p_lab->ch_sum)) {
        lord_error(0,LORD("file %s of type %d has sum fault"),
                       p_lab->mlb, p_lab->lab_type);
		return (ILL_LAB);
      }
      break;

	default: 
      lord_error(0,LORD("file %s of type %d is not a plm"),
                     p_lab->mlb, p_lab->lab_type);
    }

    (void) fprintf(o_file, "\n %s\n ", p_lab->mlb);

    if (conv_mode >= 4 /* full output */ ) {
      if (p_lab->cmplt > 0 /* propper label */ ) {
       if (p_lab->local) (void) fprintf(o_file,
                         "\n * NAME: %s\n ", p_lab->mlb);
       else {
         p_tp = gettabdir();
#if defined(sparc) || defined(__sparc)
         if (*p_tp == '\0') p_tp = settabdir(NULL);
#endif
         (void) fprintf(o_file,
                         "\n * NAME: %s%s\n ", p_tp, p_lab->mlb);
       }
       (void) fprintf(o_file, "\nlng       = %15ld", (long) sizeof (*p_lab));
       (void) fprintf(o_file, "\nlab_type  = %15d", p_lab->lab_type);
       (void) fprintf(o_file,
                      "         version   = %15d", p_lab->version);
       (void) fprintf(o_file, "\nmlb       = %15s", p_lab->mlb);
       (void) fprintf(o_file, 
                      "         compl     = %15d", p_lab->cmplt);
       (void) fprintf(o_file, "\ndatum     = %15d", p_lab->datum);
       (void) fprintf(o_file, "%9s%s", " ", "itrf");
       (void) fprintf(o_file, "\nellipsoid = %15d", p_lab->ellipsoid);
       (void) fprintf(o_file, "%9s%s", " ", "grs80");
       (void) fprintf(o_file, "\nplates    = %15d", p_lab->plm_max);
      } else
       (void) fprintf(o_file, "\n* Common platemotion label ;");

    } /* 4 & 5 */

    break; /* end case 3, 4 & 5, plm info */

  } /* conv_mode */

  return(p_lab->lab_type);

#undef    YEAR

}

