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
 


/* prn_cho.h                        # page 1   14 Sep 1994 16 33 */


/* Copyright (c) 1993 GD, Kort-og Matrikelstyrelsen, Denmark   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#ifndef     _PRN_CHO_
#define     _PRN_CHO_


extern short             prn_cho(
/*_____________________________*/
FILE                    *out,
union geo_lab           *d_lab,
double                  *crd,
struct typ_dec          *tpd,
char                    *o_str,
double                  *cmt,
double                  *old_cmt
);

/*
prn_cho writes a coordinate set acording to the label *d_lab
on the file *out.

*crd holds the coordinates (1 to 3)
*tpd holds the type decimal controling the output format
*cmt holds the commentwords
*old_cmt holds the last used comment_words.
*old_cmt equals *cmt at return.

*o_str == "\0"  : no 'sp' and no unit in output (f.ex.: sx)
*o_str == " \0" :             no unit in output
*o_str == "c\0" : no 'sp'
*o_str == " c"  : both 'sp' and unit in output


*/
#endif


