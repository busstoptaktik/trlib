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
 

/* srch_tab_typ     ver 2010.1        # page 1   18 Feb 2010 10 19 */


/* Copyright (c) 2009, National Space Institute, DTU, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of DTU, Denmark */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */

/* srch_tab_typ(mode, f777, *tab_typ_txt) searches the entry   */
/* to a table type                                             */
/*  result  >  0  : tab_typ f777                               */
/*  result == -2  : STOP or EOF                                */
/*                                                             */
/* *lab_typ       : Kind of table (or STP_LAB) See geo_lab.h   */
/*                                                             */
/* *tab_typ_txt may be :: (acording to *lab_typ)               */
/*        "geoid", "dtmtab", "dhtab", "upltab", "demtab",      */
/*        "tabtab", "bagtab", "t3dtab", "t2dtab", "tddtab"     */
/* *pos           : after "REC_TABLE" for next recursive call  */

#if !defined     (H_SRCH_TAB_TYP)
#define           H_SRCH_TAB_TYP

extern int               srch_tab_typ(
/*__________________________________*/
int                     *lab_typ,
char                    *tab_typ_txt,
long                    *pos
);

#endif
