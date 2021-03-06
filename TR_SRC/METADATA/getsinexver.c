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

/* getsinexver sets the version number to the text ftx          */

/* *ftx         :: 1 :: SINEX HEADER                            */
/* *state       :: 1 :: at input                                */
/* version      :: number ver*100+serial to be written on ftx   */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void               getsinexver(
/*___________________________*/
char              *ftx,
int               *state,
int               *version) 
{
  double ver;

  if (*state == 1) {
    sscanf(ftx+6, "%lf", &ver);
    *version = (int) ver * 100;     // SINEX HEADER LINE
    if (*version != 210 && *version > 202) *state = -9999;
    // versions up 2.02 (== 2.10) is accepted
  }
  else *state = -9999;
  return;
}
