

/* fputshpprj  ver 2008.01          # page 1     6 Jan 2009 11 57 */


/* Copyright (c) 2009, National Space Institute, DTU, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of DTU, Denmark */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */

#include <stdio.h>
#include <string.h>
#include "geo_lab.h"
#include "geoid_d.h"

#include              "conv_lab.h"


extern int       fputshpprj(
/*________________________*/
FILE            *fh_out,
union geo_lab   *g_lab);

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

