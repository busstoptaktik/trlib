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
 


/* utmtmgr.h                        # page 1   22 Aug 1994 11 08 */


/* Copyright (c) 1994, GD, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#if !defined     (H_UTMTMGR)
#define           H_UTMTMGR

extern int          utmtmgr(
/*________________________*/
union geo_lab       *c_lab_u,
int                 direct,
double              *N,
double              *E,
double              *B,
double              *L,
char                *mgref,
int                 cif
);

/* Function:
Transformation to/from MilGridRef (MGR) and UTM. The UTM 
coordinates refer to the zone described in the label, but 
may be outside the zone limits. The MGR-strings produced 
are however placed inside the proper UTM zone.

utmtmgr          (call and return)        int
The value of the function is for UTM -> MGR the zone number
difference and 0 for MGR -> UTM in case of succes and -100
in case of inconsistency of the MGR-string.

Parameters:
*c_lab           (call)                   struct coord_lab
The label describing the UTM zone of the input coordinates.
In case of transformation MGR -> UTM any UTM coordinate label
of proper datum is accepted.

direct           (call)                   int
Control of the direction of the transformation:

      direct == +1   => UTM -> MGR
      direct == -1   => MGR -> UTM

any other value causes an error exit.

*N, *E          (call and return)       double
The UTM coordinates. The input values should be in the UTM
zone described by the label. The return values are in the
proper UTM zone.

*B, *L          (return)                double
The geographical coordinates on the datum described in the
UTM coordinate label.
(continued)

*/


/* utmtmgr.h                        # page 2   22 Aug 1994 11 08 */


/*
*mgref         (return or call)         char
The MGR string, returned in case of UTM -> MGR and input in 
case of MGR -> UTM.

cif            (call)                   int
The number of digits in the E and N values of the MGR-string.
The call value is not used in case of transformation MGR->UTM.

PROG:  Knud Poder, AUG 1994

*/

#endif
