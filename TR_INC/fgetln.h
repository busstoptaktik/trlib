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
 


/* fgetln version 1.0               # page 1   06 Sep 1994 16 42 */


/* Copyright (c) 1989 U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

extern int                  fgetln(
/*_______________________________*/
char                        *outstr,
int                         *n_item,
FILE                        *fp
);

/*

fgetln       (return)          int function
_            Number of characters pointed to by *outstr or EOF

outstr       (return)          char *
_            Char pointer to char array

n_item       (return)          int *
_            Integer pointer holding number of item returned

fp           (call)            FILE *
_            File pointer to open file

The function reads one line from a file pointed to by fp, skips
all single spaces, replace two or more spaces, or tabs with
two spaces.

Comments * ; (old algol) and the ones found in c_prog (can't be
shown here) are skipped and replaced with two spaces, comments
over more than one line, are skipped and returned in one line
with two spaces.

Strings in "xx" will be returned as they are found.

The \n at the end are returned too.

The function itself return numbers of characters including \n,
or EOF.

PROG:  Frands Steffensen
F. Steffensen SEP. 1998 (MI/ITK) return strings unformated

*/


