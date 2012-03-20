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
 


/* fgetshpprj  ver 2009.01          # page 1     6 Jan 2009 11 57 */


/* Copyright (c) 2009, National Space Institute, DTU, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of DTU, Denmark */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "geo_lab.h"
#include "geoid_d.h"
#include "typ_dec.h"


int              fgetshpprj(
/*______________________*/
FILE            *fh_in,
union geo_lab   *g_lab)
{

/* fgetshpprj returns::
   0    prj line ok. label_set ok.
   -1   syntax error in prj line.
   -2   def_shp.txt  def_shp.txt file not found in geoid directory
   -3   def_shp.txt  is NOT a def_shp file
   +1   label NOT set from prj. projection NOT found.
   +2   label NOT set from prj. datum NOT found.
   +3   label NOT set from prj. params error
   +4   label NOT set from prj.

   coordinates unit is:deafault: "m",
                   for:geo: "dg" or "rad"
   dependening geo_lab->u_c_lab.g_tpd
      (.gf ==2 && .tf ==3) ? "rad" : "dg"

*/

#include   "c_tabdir_file.h"
#include   "conv_lab.h"
#include   "fgetlhtx.h"
#include   "fgetln_kms.h"
#include   "i_tabdir_file.h"
#include   "set_tpd.h"

  char   prj_txt[1024], temp[128], param[128], std_lat[16];
  char  *p_tmp, c, sep = '_';
  char  *p_txt = prj_txt, *p_txt1 = prj_txt, *p_txt2 = prj_txt, *p_txt3 = prj_txt, *p_txt4 = prj_txt;
  char   entry[128], lab_str[MLBLNG], mlb2[MLBLNG], dtm[MLBLNG];
  int    used, i, j, k;
  int    go_on = 1, params = 0, r_params = 0, completed = 0;
  int    state = 0, lab_state = 0, chrs_in = 0, itm = 0, zone, N = 1;
  double val;
  FILE  *fp;

  g_lab->u_c_lab.g_tpd = *set_tpd("dg", 2, 7);
  k = 0;
  do {
    c = (char) fgetc(fh_in);
    if (c != '\n' && c != '_' && c != ' ') {
      if (c == '(') c = '[';
      else
      if (c == ')') c = ']';
      if (c == '"') k = (k == 0) ? 1 : 0;
      if (c != ',' || k == 0) {
        *(prj_txt + chrs_in) = c;
        ++ chrs_in;
        if (c == '[') {
          ++ state;
          ++ lab_state;
        } else
        if (c == ']') -- state;
      } /* else inside text skip: , */
    }
  } while ((state > 0 || lab_state == 0) &&
            chrs_in < 1023 && c != '\0');
  *(prj_txt + chrs_in) = '\0';

  lab_state = 0;
  if (state == 0 && chrs_in > 0) {
    fp = i_tabdir_file(5, "def_shp.txt", &i, temp);
    if (i) return ((i == -1) ? -2 : -3);

    *param = '\0';
    (void) strcpy(mlb2, "_h_msl");
    p_txt1 = strchr(p_txt, '[');
    k      = (int)((ptrdiff_t)p_txt1 - (ptrdiff_t)p_txt);
    (void) fgetlhtx(fp, entry);
    for (j = 0; go_on && state == 0; j++) {
      (void) fgetln_kms(entry, &i, fp);
      if (strncmp(p_txt, entry, k) == 0) {
        switch (j) {
        case 0:  // COMPDCS
          ++ go_on;
          sep   = 'H';
          p_txt = strchr(p_txt, ',') +1;
          break;
        case 1:  // GEOGCS
        case 2:  // GEOCCS
          -- go_on;
          lab_state = 2;
          (void) sprintf(lab_str, "geo%c", sep);
          p_txt1 = strchr(p_txt, '"') +1;
          p_txt2 = strchr(p_txt1, '"');
          p_txt3 = strstr(p_txt1, "UNIT");
          if (p_txt3 != NULL) {
            p_txt4 = strchr(p_txt3+2, '"');
            if (strncmp(p_txt3, "radian", p_txt4-p_txt3-2)==0)
                g_lab->u_c_lab.g_tpd = *set_tpd("rad", 2, 7);
          }
          break;
        case 3:  // PROJCS
          -- go_on;
          lab_state = 1;
          p_txt1 = strchr(p_txt, '"') +1;
          p_txt2 = strchr(p_txt1, '"');
          (void) fgetlhtx(fp, entry);
          do {
            (void) fgetln_kms(entry, &i, fp);
            (void) sscanf(entry, "%s%n", temp, &used);
            j      = used;
            p_txt3 = strstr(p_txt1, temp);
            if (p_txt3 != 0 && p_txt3 < p_txt2) {
            p_tmp = entry + used; // get params:
            (void) sscanf(p_tmp, "%d%n", &params, &used);
            p_tmp += used; // get completed:
            (void) sscanf(p_tmp, "%d%n", &completed, &used);
            p_tmp += used; // proj_name
            (void) sscanf(p_tmp, "%s%n", lab_str, &used);
            if (params == 0) {
              // known projection (no params)
              lab_state       = 3;
              j               = (int) strlen(lab_str);
              *(lab_str +j)   = sep;
              *(lab_str +j+1) = '\0';
            } else
              if (params == -1) { // utm zone (no params)
                lab_state = 2;
                r_params  = params;
                p_txt4    = p_txt3 + j; // read zone
                (void) sscanf(p_txt4, "%d%n", &k, &used);
                if (*(p_txt4+used) == 'S') N = 0;
                (void) sprintf(lab_str,
                        (N == 1) ? "utm%02d%c" : "utms%02d%c", k, sep);
              } else {
                // known projection with <params> params
                itm    = (strcmp(lab_str, "itm") == 0) ? 1 : 0;
                zone   = (itm == 1) ? 0 : -1;
                p_tmp  = strstr(p_txt3, "PROJECTION");
                p_txt3 = strstr(p_tmp , "LatitudeOfOrigin");
                if (p_txt3 != NULL) {
                  p_txt4 = strchr(p_txt3, ',') +1;
                  p_txt3 = strchr(p_txt4, ']');
                  (void) strncpy(std_lat, p_txt4, p_txt3 - p_txt4);
                  (void) strncpy(param,   p_txt4, p_txt3 - p_txt4);
                  (void) strcat(param, "dg  ");
                  ++ r_params;
                  if (itm == 1) {
                    (void) sscanf(p_txt4, "%lf", &val);
                    if (val != 0.0) itm = 0;
                  }
                }
                else (void) strcpy(param, "0dg  ");
                p_txt3 = strstr(p_tmp, "FalseNorthing");
                if (p_txt3 != NULL) {
                  p_txt4 = strchr(p_txt3, ',') +1;
                  p_txt3 = strchr(p_txt4, ']');
                  (void) strncat(param, p_txt4, p_txt3 - p_txt4);
                  (void) strcat(param, "m  ");
                  ++ r_params;
                  if (itm == 1) {
                    (void) sscanf(p_txt4, "%lf", &val);
                    if (val != 0.0) itm = 0;
                  }
                }
                else (void) strcat(param, "0km  ");
                p_txt3 = strstr(p_tmp, "CentralMeridian");
                if (p_txt3 != NULL) {
                  p_txt4 = strchr(p_txt3, ',') +1;
                  p_txt3 = strchr(p_txt4, ']');
                  (void) strncat(param, p_txt4, p_txt3 - p_txt4);
                  (void) strcat(param, "dg  ");
                  ++ r_params;
                  if (itm == 1) {
                    (void) sscanf(p_txt4, "%lf", &val);
                    zone = (int) ((val + 183.0) / 6);
                    if (zone > 60) zone -= 60;
                  }
                } else {
                  (void) strcat(param, "0dg  ");
                  itm = 0;
                }
                p_txt3 = strstr(p_tmp, "FalseEasting");
                if (p_txt3 != NULL) {
                  p_txt4 = strchr(p_txt3, ',') +1;
                  p_txt3 = strchr(p_txt4, ']');
                  (void) strncat(param, p_txt4, p_txt3 - p_txt4);
                  (void) strcat(param, "m  ");
                  ++ r_params;
                  if (itm == 1) {
                    (void) sscanf(p_txt4, "%lf", &val);
                    if (val != 500000.0) itm = 0;
                  }
                }
                else (void) strcat(param, "0km  ");
                p_txt3 = strstr(p_tmp, "ScaleFactor");
                if (p_txt3 != NULL) {
                  p_txt4 = strchr(p_txt3, ',') +1;
                  p_txt3 = strchr(p_txt4, ']');
                  (void) strncat(param, p_txt4, p_txt3 - p_txt4);
                  (void) strcat(param, "  ");
                  ++ r_params;
                  if (itm == 1) {
                    (void) sscanf(p_txt4, "%lf", &val);
                    if (val != 0.9996) itm = 0;
                  }
                } else
                if (params == 5) {
                  (void) strcat(param, "1.0  ");
                  itm = 0;
                }
                if (params == 6) {
                  p_txt3 = strstr(p_tmp, "standardparallel1");
                  if (p_txt3 != NULL) {
                    p_txt4 = strchr(p_txt3, ',') +1;
                    p_txt3 = strchr(p_txt4, ']');
                    (void) strncat(param, p_txt4, p_txt3 - p_txt4);
                    ++ r_params;
                  }
                  else (void) strcat(param, std_lat);
                  (void) strcat(param, "dg  ");
                  p_txt3 = strstr(p_tmp, "standardparallel2");
                  if (p_txt3 != NULL) {
                    p_txt4 = strchr(p_txt3, ',') +1;
                    p_txt3 = strchr(p_txt4, ']');
                    (void) strncat(param, p_txt4, p_txt3 - p_txt4);
                    ++ r_params;
                  }
                  else (void) strcat(param, std_lat);
                  (void) strcat(param, "dg  ");
                }
                lab_state = 2;
                if (itm == 1 && 0 <= zone && zone <= 60) {
                  (void) sprintf(lab_str, "utm%02dH", zone);
                  *param = '\0';
                }
              }
            }
          } while (lab_state == 1 && strcmp(temp, "stop"));
          break;
        case 4:  // VERTCS
          -- go_on;
          lab_state = 3;
          p_txt3 = strstr(p_txt2, "dvr90");
          if (p_txt3 != NULL) (void) strcpy(mlb2, "_h_dvr90");
          break;
        case 5:  // STOP
          state = 1;
          break;
        }
      }
    }

    if (state == 0) {
      if (!completed) { // read datum
        (void) fgetlhtx(fp, entry);
        do {
          (void) fgetln_kms(entry, &i, fp);
          (void) sscanf(entry, "%s%n", temp, &used);
          // search in PROJCS or GEOCS
          p_txt3 = strstr(p_txt1, temp);
          if (p_txt3 != 0 && p_txt3 < p_txt2) {
            if (*temp != '<') {
              lab_state = 3;
              p_tmp = entry + used; // datum_name
              (void) sscanf(p_tmp, "%s%n", dtm, &used);
              (void) strcat(lab_str, dtm);
            } 
          }
        } while (lab_state == 2 && strcmp(temp, "stop"));
        if (lab_state == 2) return(+2);
      }
      if (sep == 'H') (void) strcat(lab_str, mlb2);

      i = conv_lab(lab_str, g_lab, param);
      i = (i == CRD_LAB) ? ((params == r_params) ? 0 : +3): +4;
    } else i = 1;
    (void) c_tabdir_file(5, fp);
    return(i);
  } else return(-1);

  return(0);
}
