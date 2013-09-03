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
 


/* fputcw.h version 1.0              # page 1    1 Mar 1993 11:18 */


/* Copyright (c) 1994, Kort-og Matrikelstyrelsen, GD, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of GD, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not */
/* indicate an intention of publishing this code.             */

#if !defined     (H_FPUTCW)
#define           H_FPUTCW

extern int           fputcw(
/*________________________*/
FILE                *fp,
double               cw,
int                  mode
);

/*

fputcw        Number of characters written

fp            File pointer to an open file

cw            G.I. date_time, MD comment word or jnsd or ?

mode          mode == 0: digital date. mode == 1: month names 



ISO (our)   1992 04 17, 09.45   or 17. Apr. 1992, 09.45
simple            1992
MD            1 00045 89 02 3 012   max < 900 00000 00 00 0 000

PROG: Frands Steffensen    1994

*/

#endif
