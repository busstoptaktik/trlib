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
 


/* conv_tab  ver 2010.1            # page 1   12 jan 2010 15 35 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

/* conv_lab(name, lab) converts the name of a lab to the lab   */
/* see. this                                                   */
/* an unrecognized name is considered a filename containing a  */
/* genuine label                                               */


#ifndef  _CONV_TAB_FNC_
#define  _CONV_TAB_FNC_

#include "geo_lab.h"

extern int            conv_tab(
/*___________________________*/
char                 *lab_info,
struct lab_def_str   *p_lb,
struct gde_lab       *t_lab,
...
);

/*  SEE conv_lab.h for details

service function called from conv_lab : TABLE labels

Prog: K. Engsager, DEC 2001
Prog: K. Engsager, DEC 2010

*/

#endif

