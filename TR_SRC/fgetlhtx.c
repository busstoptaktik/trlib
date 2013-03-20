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
 
#include <stdio.h>
#include <stdlib.h>

#include "lord.h"

int                fgetlhtx(
/*________________________*/
FILE              *iofile,
char              *in_str
)

{

  int                        i, c;

    /* collect label forerunner ( = #) */
    i = 1;
    do {
      c = getc(iofile);
      switch(i) {
      case 1:
        switch(c) {
        case '#':
          i = 0;
          break;
        case '*':
          i = 2;
          break;
        case '/':
          i = 3;
          break;
        default :
          break;
        }
        break;



/* fgetlhtx                      # page 2   09 Feb 1995 14 49 */


      case 2: /* comment * ... ; */
        if (c == ';') i = 1;
        break;
      case 3: /* efter / søges * */
        if (c == '*') i++;
        else i = 1;
        break;
      case 4: /* søg stop *      */
        if (c == '*') i++;
        break;
      case 5: /* efter * søges / */
        i = (c == '/') ? 1 : 4;
        break;
      default:
        break;
      }
    }  while (i != 0 && c != EOF);

    if (c == EOF) {
      (void) ungetc(c, iofile);
      (void) lord_info(0,LORD("\n\nEND OF FILE\n"));
      return(EOF);
    }

    (void) fscanf(iofile, "%s", in_str);

  return(0);

}


