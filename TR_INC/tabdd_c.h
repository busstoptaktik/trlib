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
 


/* tabdd_c                         # page 1   25 Jul 2006 17 25 */


/* Copyright (c) 2004 Kort & Matrikelstyrelsen, Denmark   */
/* and Danish National Space Center                       */
/* All rights reserved.                                   */

/* This is unpublished proprietary source code of Kort &         */
/* Matrikelstyrelsen, Denmark, and Danish National Space Center. */
/* This copyright claim does not indicate an intention of        */
/* publishing this code.                                         */

#if !defined     (H_TABDD_C)
#define           H_TABDD_C

extern int               tabdd_c(
/*____________________________*/
struct mtabdd_str       *tab_table
);

/* see allso geoid_d.h  and tabdd_i.h                          */

/* closes the streams to the FILE used for access to the table */

/* tab_table: the structure describing the use of the table    */ 

/* PROG: Karsten Engsager, JUL 2006 */

#endif
