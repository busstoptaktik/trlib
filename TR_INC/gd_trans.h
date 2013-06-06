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
 


/* gd_trans  ver 2010.1          # page 1   12 jan 2010 11 57 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

#if !defined     (H_GD_TRANS)
#define           H_GD_TRANS

#include <stdio.h>
#include "geo_lab.h"
#include "geoid_d.h"

extern int                 gd_trans(
/*________________________________*/
union geo_lab            *i_lab,
union geo_lab            *o_lab,
double                    N_in,
double                    E_in,
double                    H_in,
double                   *X,
double                   *Y,
double                   *Z,
double                   *gh,
char                      req_gh,
struct mgde_str          *grid_tab,
char                     *usertxt,
FILE                     *tr_error
);


/* Function:

gd_trans              (call & return)              int

Transforms and/or predicts between (allmost) all systems described by
csysdoc or the instructions following an unsuccesfull call of kmstr.
Senseless transformations as e.g. a local Danish system to/from a 
Greenland conical system are not tested for, but will be trapped by
the checks in the activated transformation functions.

The return values are:

                 0 => ok
or ER_GP1 + ER_GP2 < 0 => ERROR

ER_GP1 (-1 to -99):
TRF_INACC_  => boundary exceeded (inaccurate)
TRF_TOLLE_  => transformation tollerance exceeded
TRF_AREA_   => boundary exceeded (serious)
TRF_GEOID_  => no geoid cause illegal transformation
TRF_ILLEG_  => illegal transformation
TRF_ILLEGH_ => illegal height transformation
TRF_PROGR_  => programe/system fault

ER_GP2 (-100 to -9900):
TAB_C_ARE_  => table area out (boundary exceeded)
TAB_C_PRJ_  => table called with wrong label
TAB_C_NON_  => table none found
TAB_G_MAX_  => too many tables in list
TAB_N_NAM_  => table name not found in manager
TAB_R_MAN_  => manager region not found
TAB_N_MAN_  => manager not found
TAB_N_SYS_  => manager system fault

ER_GP3 (-10000 to -990000):
PLATE_NON_  => PLATE not found
PLATE_OUT_  => PLATE station out of area (boundary exceeded)
ITRF_NON_   => ITRFyy not found
ITRF_SEQ_   => ITRF not in sequence in manager
ITRF_DTM_   => DTM not in {ITRF, IGS, ETRF, EUREF89}
ITRF_MAX_   => ITRF too many
ITRF_NAM_   => ITRF manager.gps not found
ITRF_SYS_   => ITRF system fault

NB: the values TRF_*_ and TAB_*_ are defined in geo_lab.h in 
decreasing order. 
For mapping purposes it may then be admissible to accept 
the result BIGGER OR EQUAL to TRF_INACC_ (or even TRF_TOLLE_).

NB: Sofisticated used when errors in geoid height is not important:
accept results as above or when result < TRF_PROGR_
(or use req_gh = -1; see below)

The function is not dual, but as each of the individual transformation
functions activated by gd_trans are dual auto checking functions, every
transformation in the chain is checked.

Parameters:

*i_lab_u, *o_lab_u         (call & return)          union geo_lab
Geo_labels describing the input system and the output system generated
by conv_lab.
*/


/* gd_trans  ver 2010.1          # page 2   12 jan 2010 11 57 */

/*

N, E, H                    (call)                   double
The input coordinates:

       1. Cartesian        X, Y, Z   or
       2. Geographical     latitude, longitude, height   or
       3. Rectangular      y, x, height.

The height is treated :
   ellipsoidal height : used in transformation
   height             : geoid height is added and used in trf.
   no height          : geoid height is used in trf (when req_gh >= 0).
Special danish systems: transformation of heights are performed::
   from/to old_danish sytems (h_gm91, h_gi50, h_msl (small islands))
   to/from h_dvr90 


In some cases the geoid height is not calculated.

The coordinates are in units of metres, except for the latitude and
longitude, which are in radians.

*X, *Y, *Z           (return)               double
The output coordinates:

       1. Cartesian        X, Y, Z   or
       2. Geographical     latitude, longitude, height   or
       3. Rectangular      y, x, height.

See N, E, H for details of special precautions and units.

*gh                  (return)               double
The geoid heigh or table value in the output coordinate system
IS calculated when req_gh != 0 ONLY

req_gh                (call)                char
_      == 0 NO output BUT *gh is calculated when needed
_       > 0    output *gh is calculated ALLWAYS
_       < 0 NO        grid table  is connected !!

*grid_tab             (call and return)    mtab struct
holds tables and names and statistics of use.
the entry : tab_nr gives the index of the gde_lab
enabling a report of the name of the grid actually used.

*usertxt               (call)                   char
A textstring supplied by the user (may be empty) to be inserted in the
alarm output. E.g. a station number may be useful for identifying the
reported error.

tr_error               (call and return)        FILE
A stream created, opened and manipulated by the calling programme.
The errors are reported here in text form. The usertxt is inserted
in the stream, so that the usertxt should be created before the call
of unitrans. The error output may be collected after each error or
after all calls of unitrans. The strategy for reading, contingent
reset and closing of the stream is left to the calling programme.

Prog: Karsten Engsager 1999.
Former versions:   Knud Poder & Karsten Engsager.

*/

#endif
