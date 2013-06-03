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
 



/* dk_trans.h version 1.0            # page 1   08 Mar 1995 13 02 */


/* Copyright (c) 1989 U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */


#if !defined     (H_DK_TRANS)
#define           H_DK_TRANS

extern int          dk_trans(
/*___________________________*/
union geo_lab      *in_lab,
union geo_lab      *outlab,
double              N,
double              E,
double              H,
double             *Nout,
double             *Eout,
double             *Hout,
char               *usertxt,
FILE               *tr_error
);

/* 2. dim. transformation function for area DK */

/*
*in_lab      :  from label
*out_lab     :    to label
N, E, H      :  from coordinates
*Nout, *Eout :    to coordinates
*Hout        :    to height == H
*usertxt     :  to be written in case of errors
*tr_error    :  file for error reports
             :  tr_error == NULL: NO REPORT IS PRINTED

*/

#endif
