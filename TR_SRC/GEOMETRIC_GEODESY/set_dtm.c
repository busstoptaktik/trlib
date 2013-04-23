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
 
/* Prog: KP OCT 1990                                           */
/* Modified, KP MAY 1993                                       */
/* Modified, KE JUL 2007                                       */
/* Modified, KE JAN 2010                                       */
/* Does not use def_lab anymore - instead use preparsed data stored in extern def_data *DEF_DATA */
/* simlk, june 2012 */

#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <sys/types.h>
#include    <ctype.h>
#include    <fcntl.h>
#include    <math.h>
#include    "KmsFncs.h"
#include    "trthread.h"

int               set_dtm(
/*_____________________*/
short               d_no,
char               *d_nm,
short              *p_no,
char               *p_nm,
char               *e_nm,
struct dsh_str     *trp
)

{
char    rgn_nm[MLBLNG];
short   mask;

return(
  set_dtm_1(d_no, d_nm, p_no, p_nm, e_nm, rgn_nm, &mask, trp));

}

