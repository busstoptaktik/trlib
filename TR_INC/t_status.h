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
 


/* t_status.h                       # page 1   02 Oct 1995 13 26 */


/* Copyright (c) 1995, GD,  Kort & Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort &   */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

extern int               t_status(
/*______________________________*/
FILE                    *tr_error,
char                    *usertxt,
char                    *fnc_name,
int                      err_type,
...
);

/* ADDITIONAL PARAMETERS 
char                    *c_unit;
char                    *h_unit;
double                    N,  E,  H; 
double                   dN, dE, dH;
*/

/*  t_status: alarm function for transformations and predict- */
/*  dicates a programme error.                                */

/*  tr_error: FILE for alarm texts. Opened by the user, std-  */
/*  out or stderr may be used.                                */

/*  usertxt: User-def. additional alarm text (e.g. a st_nmb)  */

/*  fnc_name: The name of the transformation function         */

/* err_type: the type of error                                */
/* 1 =>  Tolerance exceeded                                   */
/* 2 =>  Boundary exceeded (serious)   (result is undefined)  */
/* 3 =>  System/Prog fault                                    */
/* 4 =>  Illegal transformation                               */
/* 5 =>  Boundary exceeded (inacurate result)  (for mapping)  */
/* Any other type number gives -1 by default                  */

/* ... : parameters required as concluded from label and type */

/*  c_unit: "sx" or "m ". Used for units of N and E           */
/*  h_unit: "m " or "". "" means no heights                   */

/* Types 1,2 :   N,  E, (and  H if appropriate)               */
/* Type  1:     dN, dE, (and dH if appropriate) in units of   */
/* metres, from the reverse check transformation              */

/* Example 1: t_status(stdout, st_text, "ptg", 1,             */
/*                "sx", "", N, E, dN, dE);                           */

/*  Prog: Knud Poder  30 SEP 1995                             */

 
