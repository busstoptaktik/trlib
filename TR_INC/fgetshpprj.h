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
 
#if !defined(H_FGETSHPPRJ)
#define      H_FGETSHPPRJ


/* fgetshpprj  ver 2008.01          # page 1     6 Jan 2009 11 57 */


#include <stdio.h>
#include <string.h>
#include "geo_lab.h"
#include "geoid_d.h"

#include              "conv_lab.h"


extern int       fgetshpprj(
/*________________________*/
FILE            *fh_in,
struct coord_lab   *g_lab);

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

#endif

