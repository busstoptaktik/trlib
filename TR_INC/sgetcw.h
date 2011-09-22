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
 


/* sgetcw.h version 1.0               # page 1   14 Jun 1994 13 18 */


/* Copyright (c) 1994, Kort-og Matrikelstyrelsen, GD, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of GD, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not */
/* indicate an intention of publishing this code.             */

extern double           sgetcw(
/*___________________________*/
char                   *out,
int                    *used,
int                    *term_c
);

/* function sgetcw : reading of comment word, G.I. date_time
and MD. long word.

sgetcw       Date_time stripped for special characters
             and MD do

out          Pointer to char array holding the cw

used         Number of characters read from out

term_c       Last char read, terminating char

The function reads date, time in the following format
ISO (our)   1992 04 17,  9.45  or
old (our)     17 04 92,  9.45  or
simpel            1992         or
(very old)          do             (not used anymore)

MD cw        1 00045 85 02 3 023

In case of 'do' the function returns -1, the user of this function
must take action in this case. (try prev. cw)

Upon error the function returns -2.

Due to the use of 'sscanf', and the way the cw is read, the
char array must not hold 'spaces'.

*/

