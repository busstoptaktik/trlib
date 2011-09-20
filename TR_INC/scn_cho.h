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
 


/* scn_cho.h                        # page 1   14 Sep 1994 16 33 */


/* Copyright (c) 1993 GD, Kort-og Matrikelstyrelsen, Denmark   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#ifndef     _SCN_CHO_
#define     _SCN_CHO_


extern int               scn_cho(
/*_____________________________*/
char                    *p_in,
union geo_lab           *u_lab,
double                  *crd,
struct typ_dec          *tpd,
double                  *cmt,
double                  *old_cmt
);

  /* Prog. KE, DEC 1993                        */
  /* Modified from scn_crd FEB 1995, KP        */

  /* scn_cho returns 0 for succes and 1 for failure */

  /* scn_cho reads (from the string *p_in) a line of */
  /* coordinates of the type determined by the label *u_lab. */

  /* p_in:          input string (from fgetln) */
  /* u_lab:         the actual input label     */
  /* crd:           pointer to coordinates     */
  /* tpd:           pointer to typedecimal     */
  /* cmt, old_cmt:  pointer to commentword     */

#endif


