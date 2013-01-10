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
 


/* sgetdt   ver 2003.01           # page 1   19 Dec 2002 13 18 */


/* Copyright (c) 2002, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                   */

/* This is unpublished proprietary source code of  Kort-og    */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not */
/* indicate an intention of publishing this code.             */

#include    <math.h>
#include    <stdio.h>
#include    <string.h>
#include    "KmsFncs.h"

double                sgetdt(
/*_________________________*/
char                 *in,
double               *JD2000,
int                  *used,
int                  *term_c
)

/* function sgetdt : reading of comment word, G.I. date_time
and MD. long word.

sgetdt       (return and return)        double
_            Date stripped for special characters

in           (call)                     char *
_            Pointer to char array holding the cw

JD2000       (return)                  *double
_            Julian Date from JD 2000.0       

used         (return)                  *int
_            Number of characters read from in

term_c       (return)                  *int
_            Last char read, terminating char

The function reads date, time in the following format
ISO (our)   1992 04 17,  9.45  or
ISO         1992 04 17

Upon error the function returns -2.0.

Due to the use of 'sscanf', and the way the cw is read, the
char array must not hold 'spaces'. Use fgetln_kms to this end.

*/

{
  char            *ps, c;
  int             ac, ch, ye, mo, da, ho = 0, mi = 0;
  long            w1;
  double          cw, GMT;


/* sgetdt   ver 2003.01           # page 2   19 Dec 2002 13 18 */


  /* first try readning with format as double, if this         */
  /* is an succes and no comma is met, then return date.       */

  /* if comma is met then assume GI. cw, read hours and mi-    */
  /* nutes, and convert all to double.                         */

  ps      = in;
  ac      = sscanf(ps, "%lf%c%n", &cw, &c, &ch);
  *used   = ch;
  *term_c = c;
  if (ac == EOF) return(-2.0);

  if (ac != 0) {

    if (c == ',') {
      /* comma found in input, assume G.I. date_time */
      ps += ch;

      /* read hours and next char */
      ac  = sscanf(ps, "%d%c%n", &ho, &c, &ch);

      if (ac == 2 && c == '.') {  /* read minutes */
        *used += ch;
        ps    += ch;
        (void) sscanf(ps, "%d%c%n", &mi, &c, &ch);
        *used += ch;
      }
    }

    /* check for new/old date format */
    w1 = (long) floor(cw);
    cw = (double) w1;
    ye = (int)  (w1 / 10000);
    mo = (int) ((w1 /   100) %  100);
    da = (int) ( w1 %   100);

    /* check for proper values */
    if ( 1700 < ye && ye < 2101
        &&  0 < mo && mo <   13
        &&  0 < da && da <   32
        && -1 < ho && ho <   25
        && -1 < mi && mi <   61 ) {
      cw = (100.0 * cw + ho) * 100.0 + mi;
      (void) cwtJD(cw, JD2000, &GMT, 0, 0);
    } else
      return(-2.0);

    *term_c = c;
    return (cw);
  }
  return (-2.0);
}


