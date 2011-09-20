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
 


/* conv_spc    ver 2010.1          # page 1   12 jan 2010 10 19 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

/* conv_spc(name, p_lb, lab) service function for conv_lab     */
/*    result ==  0 : no lab                                    */
/*    result ==  1 : wor lab               in lab              */
/* an unrecognized name is considered a filename containing a  */
/* genuine label                                               */

#ifndef       _CONV_SPC_FNC_
#define       _CONV_SPC_FNC_

#include "geo_lab.h"

extern int               conv_spc(
/*______________________________*/
char                    *lab_info,
struct lab_def_str      *p_lb,
union  geo_lab          *u_lab,
...
);

/*  SEE conv_lab.h for details

service function called from conv_lab : WOR, POC, IDT, JDF labels

Prog: Knud Poder, FEB 1995
      K. Engsager, updated jan 1999
      K. Engsager, updated jan 2010

*/

#endif


