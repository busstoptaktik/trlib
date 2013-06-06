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
 


/* sputg.h version 1.0                # page 1   22 Feb 1993 12 54 */


/* Copyright (c) 1989 U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#if !defined     (H_SPUTG_)
#define           H_SPUTG_

extern int                 sputg(
/*_____________________________*/
char                       *outstr,         
double                     val,
struct typ_dec             *typ,
char                       *layout
);

/* converts val acording to *typ and writes on out_str

   layout may be given :
   "\0"   : default
   " "    : include blanks in output
   "0"    : start with zero
   "+"    : start with + (before possible zero)
   in any combination

*/

#endif
