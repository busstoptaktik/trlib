

/* fgetshpprj  ver 2008.01          # page 1     6 Jan 2009 11 57 */


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


extern int       fgetshpprj(
/*________________________*/
FILE            *fh_in,
union geo_lab   *g_lab);

/* fgetshpprj reads the .prj file to a shape file
   and sets the geo_lab accordingly.

   The reference names for label recognition is found
   in the file def_sjp.txt in the geoid catalouge.

   fgetshpprj returns::
   0    prj line ok. label_set ok.
   -1   syntax error in prj line.
   -2   def_shp.txt  help file not found in geoid directory
   -3   def_shp.txt  is NOT a def_shp file
   +1   label NOT set from prj. projection NOT found.
   +2   label NOT set from prj. datum NOT found.
   +3   label NOT set from prj. params error
   +4   label NOT set from prj.

   coordinates unit is:deafault: "m",
                   for:geo: "degree" or "radian"
   sets geo_lab->u_c_lab.g_tpd
      "radian" ? (.gf ==2 && .tf ==3) : (.gf ==2 && .tf ==1)

*/
