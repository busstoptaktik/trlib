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
 


/* fo_g_tr.h                       # page 1   29 Sep 1995 16 30 */


/* Copyright (c) 2001 SPACE Danish Technical University (DTU)  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of DTU SPACE    */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */

#if !defined     (H_FO_G_TR)
#define           H_FO_G_TR

extern int               fo_g_tr(
/*______________________________*/
int                      mode,
int                      direct,
double                   N_in,
double                   E_in,
double                  *N_out,
double                  *E_out,
char                    *usertxt,
FILE                    *tr_error
);

/*  fo_g_tr:   Autochecking dual prediction function for pre- */
/*  diction  from UTM zone 29 ETRS89 to                       */
/*      to_sys = {UTM zone 29 ED 50, FK54, FU50} or reverse   */
/*                                                            */
/*  Result: 0 => ok                                           */
/*         -1 => error                                        */

/*           (    > 0   => utm29_etrs89 to to_sys             */
/*  direct   {   == 0   => No trans, alarm                    */
/*           (    < 0   => to_sys to utm29_etrs89             */

/*  N_in,  E_in : Input  coordinates in metres                */
/*  N_out, E_out: Output coordinates in metres                */

/*  usertxt: User-def. additional alarm text (e.g. a st_nmb)  */
/*  tr_error: FILE for alarm texts. Opened by the user        */

/*  Prog: Knud Poder  5 OCT 1995                              */
/*  Prog: K. Engsager 4 MAY 2011                              */

#endif
