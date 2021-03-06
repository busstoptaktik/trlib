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
 


/*  ctc.h version 1.0               # page 1   25 Aug 1994 17 13 */


/* Copyright (c) 1990 GD, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of GD, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not */
/* indicate an intention of publishing this code.             */

#if !defined     (H_CTC_FNC_)
#define           H_CTC_FNC_
#include    "metadata.h"

 int            ctc(
/*______________________*/
struct coord_lab         *i_lab_u,
struct coord_lab         *o_lab_u,
double                Xi,
double                Yi,
double                Zi,
double                *Xo,
double                *Yo,
double                *Zo,
tab_dir                 *tdir
);

/* ctc:      Cartesic_Transformation_to_Cartesic            */
/* i_lab_u:  from label                                     */
/* o_lab_u:  to   label                                     */
/* Xi,Yi,Zi: from coordinates                               */
/* Xo,Yo,Zo: to   coordinates                               */
/* tr_cnt:   == 0 causes break on error, othervice continue */
/* usertxt:  to be printed on error                         */
/* tr_error: file for usertxt on error                      */

#endif
