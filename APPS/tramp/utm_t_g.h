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
 


/* utm_t_g.h      version 1.0           # page 1   16 Aug 1994 16 51 */


/* Copyright (c) 1989, Kort-og Matrikelstyrelsen, GD, Denmark */
/* Copyright (c) 1991, Kort-og Matrikelstyrelsen, GD, Denmark */
/* Copyright (c) 2008, Kort-og Matrikelstyrelsen, GD, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of GD, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not */
/* indicate an intention of publishing this code.             */

extern int             utm_t_g(
/*_______________________*/
struct proj_const      *TC,
int                    direct,
double                 N,
double                 E,
double                *Nout,
double                *Eout
);

/*
Function 

utm_t_g                                                call   
Dual autochecking transformation function for transformation
between geographical coordinates and the regular coordinate
systems
:
    Projection name           fnc (no). = cstm no.  
    Gauss-Krueger (UTM) (ITM)    3

An alarm is produced when the check by the (internal) back-
transformation exceeds the tolerance of 0.000 001 arcsec (or
the equivalent linear amount 30 my on the earth). The function
returns
            utm_t_g =  0              => ok
            utm_t_g = -1              => tolerance exceeded
            utm_t_g = -2              => 1.e6*tolerance exceeded
            utm_t_g = -3              => OUT of definition AREA
            utm_t_g = -5              => direct == 0

N, E              double                           call
The projection- or geographical coordinates input for trans-
formation in units of metres (projection) or radians (geogr.).

Nout, Eout        double                           return
The transformed projection- or geographical coordinates output
in units of metres (projection) or radians (geogr.).
*/


/* utm_t_g.h      version 1.0           # page 2   16 Aug 1994 16 51 */


/*
TC                struct proj_const *TC             call
The structure of transformation constants defined in the
include-file "proj_c.h", which also describes the elements
in detail, indicating the parts to be set by the user and the
parts initialized by set_utm before the first call. If more than one
single system transformation is called in a programme (or func-
tion), each must have its own TC, declared by

             struct proj_const  TC;

as the constants are retained. TC must point to a declared 
structure holding the elements, eg. by TC = &DTC.

direct               int                             call
direct =  1 => transform projection -> geogr.
direct = -1 => transformation geogr. -> projection
direct =  0 => no transformation, input is passed as output, the

External functions:
math library:   
atan2, cos, sin, sinh, log, fabs
kms functions:  
clencshaw, gatg, v_red

The formulae are taken from R. Koenig / K.H. Weise: 
Mathematische
Grundlagen der hoehren Geodaesie und Kartographie, Erster Band, 
Das Erdsphaeriod und seine konformen Abbildungen.
Springer-Verlag, Berlin/Goettingen/Heidelberg 1951.

Prog: 
Knud Poder 4 NOV 1977 (algol version), OCT 1989 (C-version);
AUG 1990, generalizing  from utm to 4 regular projections.
May 2008, series to 5th degree, K. Engsager.
*/


