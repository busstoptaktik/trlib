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
 


/* ptg_d      version 2010.1           # page 1   12 Jan 2010 16 51 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

#if !defined     (H_PTG_D_FUNC_)
#define           H_PTG_D_FUNC_

extern int             ptg_d(
/*_________________________*/
union geo_lab         *TC,
int                    direct,
double                 N,
double                 E,
double                *Nout,
double                *Eout,
char                  *usertxt,
FILE                  *tr_error,
double                *D,
double                *Dgtu
);

/*
Function 

ptg_d            int function, fnc. no = 3-6       call   
Dual autochecking transformation function for transformation
between geographical coordinates and the regular coordinate
systems
:
    Projection name           fnc (no). = cstm no.  
    Gauss-Krueger (UTM)          3
    Mercator                     4
    Lambert                      5
    Stereographic                6

An alarm is produced when the check by the (internal) back-
transformation exceeds the tolerance of 0.000 001 arcsec (or
the equivalent linear amount 30 my on the earth). The function
returns
            ptg_d =  0              => ok
            ptg_d =  1 << (fnc)     => tolerance exceeded
            ptg_d =  UNDEFSYS (=-1) => non-applicable coord sys
            ptg_d =  ARAEOUT  (=-2) => outside acceptable area

N, E              double                           call
The projection- or geographical coordinates input for trans-
formation in units of metres (projection) or radians (geogr.).

Nout, Eout        double                           return
The transformed projection- or geographical coordinates output
in units of metres (projection) or radians (geogr.).
*/


/* ptg_d      version 1.0           # page 2   12 Jan 2010 16 51 */


/*
TC                struct coord_lab TC              call
The structure of transformation constants defined in the
include-file "geo_lab.h", which also describes the elements
in detail, indicating the parts to be set by the user and the
parts initialized by ptg_d at the first call. If more than one
single system transformation is called in a programme (or func-
tion), each must have its own TC.

direct               int                             call
direct =  1 => transform projection -> geogr.
direct >  1 => transformation projection -> Gaussian Lat + Lng
direct = -1 => transformation geogr. -> projection
direct < -1 => transformation Gaussian Lat + LNG -> projection
direct =  0 => no transformation, input is passed as output, the
ptg_d gives the result -1, i.e. alarm as undefined system. 

The alarm is printed on tr_error.

D                 double                           return
The values m and c for transformation from regular coordinate
system to geographical.

Dgtu              double                           call
The constants for the trig series to calc 1/m and -c. (setDtrc.c)

External functions:
math library:   
atan2, cos, sin, sinh, log, fabs
kms functions:  
clenshaw, gatg, t_status

The formulae are taken from R. Koenig / K.H. Weise: 
Mathematische Grundlagen der hoehren Geodaesie und
Kartographie, Erster Band. 
Das Erdsphaeriod und seine konformen Abbildungen.
Springer-Verlag, Berlin/Goettingen/Heidelberg 1951.

Prog: 
Knud Poder 4 NOV 1977 (algol version), OCT 1989 (C-version);
AUG 1990, generalizing  from utm to 4 regular projections.
AUG 1994, calculating m and c in utm projection. KE.
Karsten Engsager  12 Jan 2010. updated
*/

#endif
