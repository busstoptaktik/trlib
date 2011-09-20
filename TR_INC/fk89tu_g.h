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
 


/* fk89tu_g.h                       # page 1   04 Apr 1997 20 17 */


/* Copyright (c) 1997, GD,  Kort & Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort &   */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */


extern int               fk89tu_g(
/*______________________________*/
int                      direct,
double                   N_in,
double                   E_in,
double                  *N_out,
double                  *E_out,
char                    *usertxt,
FILE                    *tr_error
);

/*  fk89tu_g:   Autochecking dual prediction function for pre- */
/*  diction  between Lambert coordinates  with one paralel at  */
/*  latitude 62 dg and central longitude 27 degrees on ED50    */
/*  datum (redefined fk54a)                                    */
/*  and UTM zone 29 coordinates on EUREF89.                    */
/*                Result: 0 => ok                              */
/*                       -1 => error                           */

/*           (    > 0   => fk89 -> UTM                         */
/*  direct   {   == 0   => No trans, alarm                     */
/*           (    < 0   => UTM -> fk89                         */

/*  N_in,  E_in : Input  coordinates in metres                 */
/*  N_out, E_out: Output coordinates in metres                 */

/*  usertxt: User-def. additional alarm text (e.g. a st_nmb)   */
/*  tr_error: FILE for alarm texts. Opened by the user         */

/*  Prog: Karsten Engsager  4 APR 1997                               */

