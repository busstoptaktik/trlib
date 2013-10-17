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
 

#if !defined     (H_FPUTSHPPRJ)
#define           H_FPUTSHPPRJ

/* fputshpprj  now based on sputshpprj, which is based on prev. verison of fputshpprj, simlk, oct 2011 */

#include <stdio.h>
#include "geo_lab.h"
#include "metadata.h"


extern int       fputshpprj(
/*________________________*/
FILE            *fh_out,
struct coord_lab   *g_lab, def_data *data);

/* fputshpprj writes the .prj file to a shape file
   from the geo_lab.

   The reference names for label recognition is found
   in the file def_shp.txt in the geoid catalouge.

   fputshpprj returns::
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

#endif
