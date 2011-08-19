

/* fputshpprj  ver 2009.01          # page 1     6 Jan 2009 11 57 */


/* Copyright (c) 2009, National Space Institute, DTU, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of DTU, Denmark */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */

#include <stdio.h>
#include <string.h>
#include "geo_lab.h"
#include "geoid_d.h"


int              fputshpprj(
/*______________________*/
FILE            *fh_out,
union geo_lab   *g_lab)
{

/* fputshpprj writes the .prj file to a shape file
   from the geo_lab.

   The reference names for label recognition is found
   in the file def_sjp.txt in the geoid catalouge.

   fgetshpprj returns::
   0    prj line written ok.
   -2   def_shp.txt  def file not found in geoid directory
   -3   def_shp.txt  is NOT a def_shp file
   -4   geo_lab unknown shape.prj format.
   -5   illegal geo_label.
   -6   program error.

   coordinates unit is:deafault: "m",
                   for:geo: "dg" or "rad"
   dependening geo_lab->u_c_lab.g_tpd
      (.gf ==2 && .tf ==3) ? "rad" : "dg"

*/

#include   "c_tabdir_file.h"
#include   "fgetlhtx.h"
#include   "fgetln.h"
#include   "get_mlb.h"
#include   "i_tabdir_file.h"
#include   "set_dtm_1.h"
#include   "set_grs.h"

  int get_val_str(char* val_str, double val);

  char              prj_txt1[MLBLNG], prj_txt2[2*MLBLNG];
  char              dtm_txt1[MLBLNG], dtm_txt2[2*MLBLNG];
  char             *h_mlb, *p_txt = NULL, *p_txt1;
  char              hdtm_txt1[MLBLNG], hdtm_txt2[MLBLNG];
  char              ell_txt[MLBLNG], ell[MLBLNG], h_sys[MLBLNG];
  char              prj[MLBLNG], dtm[MLBLNG], h_dtm[MLBLNG];
  char              entry[128], val_str[24];
  short             region, sepch, p_no, mask;
  int               mode, used, i, k, params = 0;
  int               str_state = 0;
  double            e[9];
  struct coord_lab *c_lab = &(g_lab->u_c_lab);
  struct dsh_str    trp;
  union rgn_un      rgn_pref;
  FILE             *fp;

  fp = i_tabdir_file(5, "def_shp.txt", &i, prj_txt2);
  if (i) {
    (void) fprintf(fh_out, 
              "\n*** def_shp.txt: NOT FOUND;\n", g_lab->u_c_lab.mlb);
    return ((i == -1) ? -2 : -3);
  }

  if (c_lab->lab_type == CRD_LAB) {
    mode = (c_lab->h_ix > 0) ? 3 : 1;
    // get proj and datum
    (void) get_mlb(c_lab->mlb, &region, prj, &sepch, dtm, &h_mlb);
    do {
      (void) fgetlhtx(fp, entry);
      i = strcmp(entry, "kms_prj");
    } while (i != 0 && strcmp(entry, "stop"));
    // search proj
    do {
      (void) fgetln(entry, &i, fp);
      (void) sscanf(entry, "%s%n", prj_txt1, &used);
      p_txt = entry + used;
      (void) sscanf(p_txt, "%d%n", &k, &used);
      i = strncmp(prj_txt1, prj, k);
      if (i == 0) {
        p_txt += used;
        (void) sscanf(p_txt, "%s%n", prj_txt1, &used);
        p_txt += used;
        (void) sscanf(p_txt, "%s%n", prj_txt2, &used);
        p_txt += used;
        (void) sscanf(p_txt, "%d%n", &params, &used);
        if (params == -1) { // utm zone (no params)
          params = 5;
          p_txt1 = prj + k; // read zone
          (void) strncat(prj_txt1, p_txt1, 2);
          (void) strcat(prj_txt1, (c_lab->S_crd) ? "S" : "N");
        }
        // get datum and ellipsoide
        (void) set_dtm_1(c_lab->datum, dtm, &p_no, h_dtm,
                         ell, rgn_pref.prfx, &mask, &trp);
        (void) set_grs(c_lab->ellipsoid, ell, e);
      }
    } while (i != 0 && strcmp(prj_txt1, "stop"));
    if (i != 0) return(-4);
    // search datum
    (void) fgetlhtx(fp, entry);
    // i = strcmp(entry, "kms_dtm");
    do {
      (void) fgetln(entry, &i, fp);
      (void) sscanf(entry, "%s%n", dtm_txt1, &used);
      i = strcmp(dtm_txt1, dtm);
      if (i == 0) {
        p_txt = entry + used;
        (void) sscanf(p_txt, "%s%n", dtm_txt1, &used);
        p_txt += used;
        (void) sscanf(p_txt, "%s%n", dtm_txt2, &used);
        (void) strcpy(hdtm_txt1, prj_txt1);
        (void) sprintf(prj_txt1, "%s_%s", dtm_txt1, hdtm_txt1);
      }
    } while (i != 0 && strcmp(dtm_txt1, "stop"));
    if (i != 0) return(-4);
    // search ellipsoid
    (void) fgetlhtx(fp, entry);
    // i = strcmp(entry, "kms_ell");
    do {
      (void) fgetln(entry, &i, fp);
      (void) sscanf(entry, "%s%n", ell_txt, &used);
      i = strcmp(ell_txt, ell);
      if (i == 0) {
        p_txt = entry + used;
        (void) sscanf(p_txt, "%s%n", ell_txt, &used);
        p_txt += used;
        (void) sscanf(p_txt, "%s%n", ell_txt, &used);
      }
    } while (i != 0 && strcmp(ell_txt, "stop"));
    if (i != 0) return(-4);
  } else {
    h_mlb = c_lab->mlb;
    mode  = 2;
  }
  if (c_lab->lab_type == HGT_LAB || mode == 3) {
    (void) get_mlb(h_mlb, &region, h_sys, &sepch, h_dtm, &p_txt);      
    // search proj
    if (strcmp(h_sys, "h")) return(-4);
    if (strcmp(h_dtm, "msl") == 0) mode -= 2;
    else {
      // search datum
      (void) fgetlhtx(fp, entry);
      // i = strcmp(entry, "kms_hdtm");
      do {
        (void) fgetln(entry, &i, fp);
        (void) sscanf(entry, "%s%n", hdtm_txt1, &used);
        i = strcmp(hdtm_txt1, h_dtm);
        if (i == 0) {
          p_txt = entry + used;
          (void) sscanf(p_txt, "%s%n", hdtm_txt1, &used);
          p_txt += used;
          (void) sscanf(p_txt, "%s%n", hdtm_txt2, &used);
        }
      } while (i != 0 && strcmp(hdtm_txt1, "stop"));
      if (i != 0) return(-4);
    }
  }
  (void) c_tabdir_file(5, fp);

  // compose the .prj text
  if (mode == 3) {
    (void) fprintf(fh_out, "COMPDCS[\"%s_+_%s\",", prj_txt1, hdtm_txt1);
    ++ str_state;
  }
  if (mode % 2 == 1) {
    if (c_lab->cstm != 2) {
      (void) fprintf(fh_out, "PROJCS[\"%s\",", prj_txt1);
      ++ str_state;
    }
    (void) fprintf(fh_out,
                   "GEOGCS[\"%s\",DATUM[\"%s\",", dtm_txt1, dtm_txt2);
    str_state += 2;
    (void) fprintf(fh_out, "SPHEROID[\"%s\",", ell_txt);
    ++ str_state;
    (void) get_val_str(val_str, c_lab->a);
    (void) fprintf(fh_out, "%s,", val_str);
    (void) get_val_str(val_str, 1/c_lab->f);
    (void) fprintf(fh_out, "%s]]", val_str);
    str_state -= 2;
    (void) fprintf(fh_out, ",PRIMEM[\"Greenwich\",0.0]");
    if (c_lab->cstm == 2)
         (void) fprintf(fh_out,
                        ",AXIS[\"lat\",NORTH],AXIS[\"lon\",EAST]");
    (void) fprintf(fh_out, ",UNIT[\"");
    if (c_lab->cstm == 2 && c_lab->g_tpd.gf==2 && c_lab->g_tpd.tf==3)
         (void) fprintf(fh_out, "Radian\",1.0]]");
    else (void) fprintf(fh_out, "Degree\",0.0174532925199432955]]");
    -- str_state;
    if (c_lab->cstm != 2) {
      (void) fprintf(fh_out, ",PROJECTION[\"%s\"]", prj_txt2);
      (void) get_val_str(val_str, c_lab->L0 * 180 / M_PI);
      (void) fprintf(fh_out,
                     ",PARAMETER[\"central_meridian\",%s]", val_str);
      (void) get_val_str(val_str, c_lab->B0 * 180 / M_PI);
      (void) fprintf(fh_out,
                     ",PARAMETER[\"latitude_of_origin\",%s]", val_str);
      (void) get_val_str(val_str, c_lab->E0);
      (void) fprintf(fh_out,
                     ",PARAMETER[\"false_easting\",%s]", val_str);
      (void) get_val_str(val_str, c_lab->N0);
      (void) fprintf(fh_out,
                     ",PARAMETER[\"false_northing\",%s]", val_str);
      if (params == 5) {
        (void) get_val_str(val_str, c_lab->scale);
        (void) fprintf(fh_out,
                       ",PARAMETER[\"scale_factor\",%s]", val_str);
      } else
      if (params == 6) {
        (void) get_val_str(val_str, c_lab->B1 * 180 / M_PI);
        (void) fprintf(fh_out,
               ",PARAMETER[\"Standard_Parallel_1\",%s]", val_str);
        (void) get_val_str(val_str, c_lab->B2 * 180 / M_PI);
        (void) fprintf(fh_out,
               ",PARAMETER[\"Standard_Parallel_2\",%s]", val_str);
      }
      // (void) fprintf(fh_out, ",AXIS[\"N\",NORTH],AXIS[\"E\",EAST]");
      (void) fprintf(fh_out, ",UNIT[\"m\",1.0]]");
      -- str_state;
    }
  }
  if (mode == 3) (void) fprintf(fh_out, ",");
  if (mode >= 2) {
    (void) fprintf(fh_out, "VERTCS[\"%s\",", hdtm_txt1);
    ++ str_state;
    (void) fprintf(fh_out,
                   ",VERT_DATUM[\"%s\",UNIT[\"m\",1.0]]", hdtm_txt2);
    -- str_state;
  }
  if (mode == 3) {
    (void) fprintf(fh_out, "]");
    -- str_state;
  }

  if (str_state) return(-6);  
  return(0);
}

int get_val_str(char* val_str, double val) {
  double  val1, val2, val3;
  int     dec = 0;

  val1 = val;
  val2 = floor(val1 + 0.5);
  while (dec < 10 && fabs(val2 -val1) > fabs(val1) * 1.0e-12) {
    val1 *= 10.0;
    val2 = floor(val1 + 0.5);
    val3 = fabs(val2 -val1);
    ++ dec;
  }
  if (dec == 0) dec = 1;
  dec = sprintf(val_str, "%.*lf", dec, val);
  return(dec);
}

