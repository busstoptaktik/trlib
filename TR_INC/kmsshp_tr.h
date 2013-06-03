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
 


/* kmsshp_tr  ver 2009.01          # page 1     7 Jan 2009 11 57 */


/* Copyright (c) 2009, National Space Institute, DTU, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of DTU, Denmark */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */

#if !defined     (H_KMSSHP_TR)
#define           H_KMSSHP_TR

#include "geo_lab.h"

extern int       kmsshp_tr(
/*_______________________*/
char            *shpin_txt,
char            *shpout_txt,
int              SOE_no_h,
union geo_lab   *g_lab_in,
union geo_lab   *g_lab_out,
int             *qTrn,
char            *err_txt);

/* 
kmsshp_tr transforms the shape file <shpin_txt> from the coord sys 
   g_lab_in (may be set by fgetshpprj) to the coord sys g_lab_out
   if (SOE_no_h !=0) ==>> NO Z-values are written
   err_txt may hold any error messages.

   the .shp file is transformed from <shpin_txt> to <shpout_txt>.
   the .shx and the .dbf files are copied to <shpout_txt> where the
       extension has been changed to .shx and .dbf accordingly
   a .prj file is written when the esri code is known
*/

/* kmsshp_tr returns::
   >0        Number of trf OK (== *qTrn).
   0         No transformations no errors.
   _TRF_**** error numbers se geo_lab.h
   -95       .prj file not set: Unknown esri code
   -96       Uknown SHP Type
   -97       Output file not writeable
   -98       Input file not found
   -99       Output file not found

   coordinates unit is (_in/_out):deafault: "m",
                              for:geo: "dg" or "rad"
   dependening geo_lab->u_c_lab.g_tpd
      (.gf ==2 && .tf ==3) ? "rad" : "dg"

   *qTrn     Number of trf OK
*/

#endif
