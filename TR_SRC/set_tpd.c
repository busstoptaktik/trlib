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
 


/* set_tpd  version 2010.1           # page 1   12 Jan 2010 13 14 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

#include <string.h>
#include <math.h>
#include "geo_lab.h"
#include "typ_dec.h"

extern struct na_gr_tp_fc    ngtf[];

struct typ_dec              *set_tpd(
/*_________________________________*/
char                        *name,
int                          fwd,
int                          dec
)

{

  static struct typ_dec      td, *res = &td;
  struct na_gr_tp_fc        *p;

  /* set a NULL-typedecimal */
  res->gf = res->tf = res->ff = res->df = 0;

  /* collect group and type of name, set dec, get convfact. */
  if (*name) {
    for (p = ngtf; p->gr; p++) {
      if (!strcmp(name, p->na)) {
        res->gf = p->gr;
        res->tf = p->tp;
        if (fwd > p->md) fwd = p->md;
        res->ff = (unsigned char) (fwd > 0 ? (char) fwd : p->dd);
        if (255 > dec && dec > 10) dec = 10;
        else if (dec < 0) dec = 0;
        res->df = (char) dec;
        break;
      }
    }
  }
  return (res);
}


