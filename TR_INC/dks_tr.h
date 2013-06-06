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
 


/* dks_tr.h version.1               # page 1   17 Feb 2000 10 41 */


/* Copyright (c) 1992 LG, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                      */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#if !defined     (H_DKS_TR)
#define           H_DKS_TR

extern int              dks_tr(
/*___________________________*/
char           *in_lab,
char           *outlab,
double         Nin,
double         Ein,
double         *yout,
double         *xout,
char           *usertxt,
FILE           *tr_error
);

/*
Function and parameters

dks_tr          int function,   fnc no. 8       call and return
Dual autochecking prediction (non-regular transform.) function for

          DKS (in ED50)     <-> System 1934 Sjælland
          DKS (in ED50)     <-> RT90 5 GON W
          DKS (in ED50)     <-> UTM Zone 33 (ED 50)

The function gives the result of the transformation as

      dks_tr = 0        => ok,
      dks_tr = -1       => inaccurate (may be used in mapping)
      dks_tr = -2       => tolerance exceeded
      dks_tr = -3       => outside the accepted area
      dks_tr = -4       => programming error

The physical precision of the transformation is generally
1-2 cm, but the numerical precision is some few mm,
increasing to some cm far from the central parts.

The accepted area is limited to dks-coordinates satisfying

      250 km <= N <= 290 km
      100 km <= E <= 140 km

in_lab, outlab  char                             call
The minilabels for the systems. The names are:

dks:    The Øresund bridge system
s34s:   System 1934 Sjælland
u33:    UTM zone 33, ED50
rt90v:  The Swedish RT90 5 Gon W.

(continued) */


/* dks_tr.h version.1               # page 2   17 Feb 2000 10 41 */


/*

Nin, Ein        double                           call
UTM or s34 coordinates input for transformation in metres.

yout, xout      double                           return
s34 or UTM coordinates output after transformation in metres.

The tolerance is set by a define statement to 2 mm, and may
be redefined. The chosen value is comparable to the precision
of the forward/backward computation, but the precision of the
polynomial approximations is rather 4-5 cm (mean error appr.
1.5 cm).

Prog: Knud Poder, OCT 1992
Prog: K. Engsager, OCT 1999

*/

#endif

