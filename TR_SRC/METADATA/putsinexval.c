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

/* putsinexval sets the val to the text ftx                     */

/* *ftx         :: 1 :: SINEX HEADER                            */
/* val          :: Value to be writtena FORTRAN 21.15E          */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void               putsinexval(
/*___________________________*/
char              *ftx,
double             val)
{
  int    i, j = 0;
  char   buf[39], *cp;

  cp   = ftx + 47;
  if (val != 0.0) {
    sprintf(buf, (val > 0.0) ? "%.16E" : "%-.15E", val);
    if (*buf == '-') ++ j;
    *(buf +j +1) = *(buf +j);
    *(buf +j   ) = '.';
    for (i = 0; i < 19; i ++) *(cp +i) = *(buf +i);
    (void) sscanf(buf +19, "%d", &i);
    (void) sprintf(buf, "%+d", i +1);
    *(cp +19) = *buf;
    *(cp +20) = *(buf +1);
  } else {
    for (i = 0; i < 21; i ++) *(cp +i) = '0';
    *cp       = '.';
    *(cp +18) = 'E';
    *(cp +19) = '+';
  }
  return;
}
