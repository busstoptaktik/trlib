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
 


/* conv_rgn.h                       # page 1   21 dec 2010 10 46 */


/* Copyright (c) 2009 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

#if !defined     (H_CONV_RGN)
#define           H_CONV_RGN

extern short     conv_rgn(
/*______________________*/
short            rgn_nr,
char            *rgn_prfx,
char            *rgn_name
);

/* conv_rgn : info of region-nr, - prefix, and the country og region */
/* rgn_nr   : the internally used number = the num. rep. the prefix  */
/* rgn_prfx : region prefix (2-letter abrev. country name)           */
/* rgn_name : Country                                                */

/* NB: Some old rgn_prfx has been used. Old_prfx is converted to new */

/* RETURN::                                                          */
/* conv_rgn >  0: == rgn_nr                                          */
/* conv_rgn = -1: rgn_nr or rgn_prfx NOT FOUND                       */
/* conv_rgn = -2: def_file (def_lab.txt and "def_rgn") NOT FOUND     */

/* CALL ::                                                           */
/* rgn_nr = -1: rgn_prfx is searched to return: conv_rgn, rgn_name   */
/* rgn_nr > 0 : rgn_nr is searched to return: rgn_prfx, rgn_name     */


/* special call: (rgn_nr = -1)                                       */
/*             : and (first char in rgn_prfx is '?' | '!' | '*')     */
/* RETURN::                                                          */
/* conv_rgn = old_rgn_nr                                             */
/* rgn_prfx = NEW_rgn_prfx                                           */
/* CALL ::                                                           */
/* rgn_prfx = "?<region>": rgn_prfx is updated                       */
/* rgn_prfx = "?" | "!" | "*"  : loops over all defined Names        */
/*            when called reiterately                                */
/*            until conv_rgn == -1 signals the end.                  */
/*   ONLY by "*" calls the old_prfx are included                     */


/* NB:: illegal call: conv_rgn(-1, "?", name);                       */

/* NB::   legal call:                                                */
/*   union rgn_un   rgn_new, rgn_old;                                */
/*                                                                   */
/*   (void) strcpy(rgn_new.prfx, "?");                               */
/*   rgn_old.r_nr[1] = 0;                                            */
/*   rgn_old.r_nr[0] = conv_rgn(-1, rgn_new.prfx, name);             */

/* Prog. Knud Poder, 16 NOV 1994, after ideas by K.E and F.S.       */
/* Modified, KE SEP 2010                                            */

#endif
