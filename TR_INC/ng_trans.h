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
 


/* ng_trans  ver 2006          # page 1    3 Oct 2006 13 55 */


/* Copyright (c) 2006 Kort-og Matrikelstyrelsen, Denmark   */
/* and Danish National Space Center: All rights reserved.  */

/* This is unpublished proprietary source code of Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does  */
/* not indicate an intention of publishing this code.      */

#if !defined     (H_NG_TRANS)
#define           H_NG_TRANS

/*
#define  DEBUGFETRANS
*/

extern int           ng_trans(
/*___________________________*/
struct coord_lab       *in_lab_u,
struct coord_lab       *outlab_u,
double               N,
double               E,
double               H,
double              *Nout,
double              *Eout,
double              *Hout,
char                *usertxt,
FILE                *tr_error
);

/* 3D transformation function for NAD83G to/from GR96 */
/* NAD83G is GREENLAND and is  NOT  equal to NAD83 */

/*
*in_lab             :  from label
*out_lab            :    to label
N, E, H             :  from coordinates
*Nout, *Eout, *Hout :    to coordinates
*usertxt            :  to be written in case of errors
*tr_error           :  file for error reports
                    :  tr_error == NULL: NO REPORT IS PRINTED
  
Progr.: K.Engsager, Danish National Space Center, 2006
*/

#endif
