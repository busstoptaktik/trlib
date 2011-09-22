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
 


/* sputcw.c version 1.0             # page 1   31 May 1996 08 06 */


/* Copyright (c) 1994, Kort-og Matrikelstyrelsen, GD, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of GD, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not */
/* indicate an intention of publishing this code.             */

#include <stdio.h>

int                sputcw(
/*______________________*/
char              *str,
double             cw,
int                mode
)

/*

sputcw       Number of characters written

str          pointer to a char array

cw           G.I. date_time, MD comment word or jnsd

mode         ==1 writes month in text (day month year)
             ==3 writes month in text    and no clock at ISO
             ==0 writes month in number
             ==2 writes month in number  and no clock at ISO

ISO (our)   1992 04 17,  9.45
simple            1992
MD            1 00045 89 02 3 012   max < 900 00000 00 00 0 000

*/


/* sputcw.c version 1.0             # page 2   31 May 1996 08 06 */


{
  int     ch, ek, ye, mo, da, ho, mi;
  static char  *mo_s[] = {
    "NIL ", "Jan.", "Feb.", "Mar.",
    "Apr.", "Maj ", "Juni", "Juli", "Aug.", "Sep.",
    "Okt.", "Nov.", "Dec."    };

  /* Return for an empty cmt */
  if (cw <= 0.0) {
    ch = sprintf(str, "%22s", " ");
    return(ch);
  }

  if (170003010000.0 < cw  && cw < 999912312359.0) {
    /* date time                   */
    /* layout : xxxx xx xx, xx.xx  */
    ye  = (int) (cw / 1e8);
    cw -= ye * 1e8;
    mo  = (int) (cw / 1e6);
    cw -= mo * 1e6;
    da  = (int) (cw / 1e4);
    cw -= da * 1e4;
    ho  = (int) (cw / 1e2);
    cw -= ho * 1e2;
    mi  = (int) cw;

    if (mode == 0)
       ch = sprintf(str, "%9d %02d %02d, %02d.%02d",
                    ye, mo, da, ho, mi);
    else if (mode == 2)
       ch = sprintf(str, "%6d %02d %02d", ye, mo, da);
    else {
      if (mo < 0 || 12 < mo) mo = 0;
      if (mo)
        if (mode == 1)
           ch = sprintf(str, "%4d. %4s %4d, %02d.%02d",
                        da, mo_s[mo], ye, ho, mi);
        else ch = sprintf(str, "%4d. %4s %4d", da, mo_s[mo], ye);
      else
        ch = sprintf(str, "%22d", ye);
    }
  }
  else
    if (cw <= 100001010001.0) {
      if (1700.0 < cw  && cw < 9999.0) {
        /* jnsd or year                */
        /* layout :              xxxx  */
        ch = sprintf(str, "%22d", (int) cw);
      }
      else
        if (10000.0 < cw  && cw < 99999.0) {
          /* jnsd or short MD            */
          /* layout :          xx xxx x  */
          da  = (int) (cw / 1e4);
          cw -= da * 1e4;
          ho  = (int) (cw / 1e3);
          cw -= ho * 1e3;
          mi  = (int) cw;

          ch    = sprintf(str, "%16d %d %03d", da, ho, mi);
        }


/* sputcw.c version 1.0             # page 3   31 May 1996 08 06 */


        else {
          /* number in any kind          */
          /* layoyt :   xx xxx xx xxx x  */
          ye    = (int) (cw / 1e9);
          cw   -= ye * 1e9;
          mo    = (int) (cw / 1e6);
          cw   -= mo * 1e6;
          da    = (int) (cw / 1e4);
          cw   -= da * 1e4;
          ho    = (int) (cw / 1e1);
          cw   -= ho * 1e1;
          mi    = (int) cw;

          if (ye) ch = sprintf(str, "%9d %03d %02d %03d %d",
              ye, mo, da, ho, mi);
          else
            if (mo) ch = sprintf(str, "%13d %02d %03d %d",
                mo, da, ho, mi);
            else
              if (da) ch = sprintf(str, "%16d %03d %d", da, ho,
                  mi);
              else
                ch = sprintf(str, "%20d %d", ho, mi);
        }
    }
    else
      if (cw < 9e15) {
        /* MD                              */
        /* layout : ??x xxxxx xx xx x xxx  */
        ek  = (int) (cw / 1e13);
        cw -= ek * 1e13;
        ye  = (int) (cw / 1e8);
        cw -= ye * 1e8;
        mo  = (int) (cw / 1e6);
        cw -= mo * 1e6;
        da  = (int) (cw / 1e4);
        cw -= da * 1e4;
        ho  = (int) (cw / 1e3);
        cw -= ho * 1e3;
        mi  = (int) cw;

        ch    = sprintf(str, "%4d %05d %02d %02d %d %03d",
            ek, ye, mo, da, ho, mi);
      }
      else ch = sprintf(str, "%22s", "   ??? cmt");

  return(ch);
}


