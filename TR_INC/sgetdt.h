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
 


/* sgetdt                         # page 1    4 Jul 2003 13 18 */


/* Copyright (c) 2002, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                   */

/* This is unpublished proprietary source code of  Kort-og    */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not */
/* indicate an intention of publishing this code.             */


extern double         sgetdt(
/*_________________________*/
char                 *in,
double               *JD2000,
int                  *used,
int                  *term_c
);

/* function sgetdt : reading of comment word, G.I. date_time
and MD. long word.

sgetdt       (return and return)        double
_            Date stripped for special characters

in           (call)                     char *
_            Pointer to char array holding the cw

JD2000       (return)                  *double
_            Julian Date from JD 2000.0       

used         (return)                  *int
_            Number of characters read from in

term_c       (return)                  *int
_            Last char read, terminating char

The function reads date, time in the following format
ISO (our)   1992 04 17,  9.45  or
ISO         1992 04 17

Upon error the function returns -2.0.

Due to the use of 'sscanf', and the way the cw is read, the
char array must not hold 'spaces'. Use fgetln_kms to this end.

*/

