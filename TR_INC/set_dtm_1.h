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
 


/* set_dtm     ver 2010.1          # page 1   12 jan 2010 10 19 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

#ifndef       _SET_DTM_FNC_
#define       _SET_DTM_FNC_

#include    "geo_lab.h"

extern int      set_dtm_1(
/*____________________*/
short            d_no,
char            *d_nm,
short           *p_no,
char            *p_nm,
char            *e_nm,
char            *rgn_nm,
short           *mask,
struct dsh_str  *trp
);

/*
Function and Parameters:

set_dtm produces the constants and datum information needed for
a datum shift by means of a linear transformation with 3-7 para-
meters.

set_dtm            (call and return)          int
The internal datum number is returned as the result. The result is
> 0 in case of succes and -1 in case of failure. conv_lab accepts
an ellipsoid name as a datum name, but the datum number returned
in the label is then always -2. The handling of ellipsoid names as
datum names is not done by set_dtm

d_no               (call)                     short
The internal datum number or -1. If called with an internal (and
existing) number, the datum name is returned in d_nm, but no other
parameters are returned. The call value -1 is used as a signal for
calling by means of d_nm, the datum name.

*d_nm             (return or call)            char
The datum name returned after a call with an internal datum number,
or used for calling with d_no = -1.

*p_no             (return)                    short
The internal datum number of the parent datum. The parent datum is
the datum, which can be used for datum shifts to/from the actual
datum.

*p_nm             (return)                    char
The name of the parent datum.

*e_nm             (return)                    char
The name of the ellipsoid of the datum.

*rgn_nm           (return)                    char
The name of the region of the datum.

*mask             (return)                    short
The IMIT mask to special hgt transformations.

*/


/* set_dtm     ver 2010.01          # page 2   12 jan 2010 10 19 */

/*

*trp              (return)                   struct dsh_con
The structure containing the datum shift parameters.
See geo_lab.h.

Prog: Knud Poder 1990, 1991, 1992, 1994
Modified, KE JUL 2007, KE JAN 2010, simlk june 2012.
*/

#endif

