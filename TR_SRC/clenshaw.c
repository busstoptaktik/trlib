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
 

#include    <math.h>
#include    <stdio.h>

double            clenshaw(
/*_______________________*/
int               M,
double           *a,
int               g,
double            arg_r,
double            arg_i,
double           *R,
double           *I)

/*
Ext. proc used
sin cos sinh cosh

Prog.: K. Engsager, MAY 1994 
*/

{
  double      *p, r, i, hr, hr1, hr2, hi, hi1, hi2;
  double      sin_arg_r, cos_arg_r, sinh_arg_i, cosh_arg_i;

  /* arguments */
  p = a + g;
  switch (M) {
  case 'C':
  case 'S':
    sin_arg_r  = sin(arg_r);
    cos_arg_r  = cos(arg_r);
    sinh_arg_i = sinh(arg_i);
    cosh_arg_i = cosh(arg_i);
    r          =  2.0*cos_arg_r*cosh_arg_i;
    i          = -2.0*sin_arg_r*sinh_arg_i;
    /* summation loop */
    for (hi1 = hr1 = hi = 0.0, hr = *--p; a - p;) {
      hr2 = hr1;
      hi2 = hi1;
      hr1 = hr;
      hi1 = hi;
      hr  = -hr2 + r*hr1 - i*hi1 + *--p;
      hi  = -hi2 + i*hr1 + r*hi1;
    }
    /* sum of the series */
    if (M == 'C') {
      r  *= 0.5;
      i  *= 0.5;
      *R  = -hr1 + r*hr - i*hi;
      *I  = -hi1 + r*hi + i*hr;
    } else {
      r   = sin_arg_r*cosh_arg_i;
      i   = cos_arg_r*sinh_arg_i;
      *R  = r*hr - i*hi;
      *I  = r*hi + i*hr;
    }
  break; /* C, S */

  case 'G':
    r          =  2.0*arg_r;
    i          = -2.0*arg_i;
    p ++;
    /* summation loop */
    for (hi1 = hr1 = hi = 0.0, hr = *--p; a - p;) {
      hr2 = hr1;
      hi2 = hi1;
      hr1 = hr;
      hi1 = hi;
      hr  = -hr2 + r*hr1 - i*hi1 + *--p;
      hi  = -hi2 + i*hr1 + r*hi1;
    }
    /* sum of the series */
    *R = -arg_r*hr1 + arg_i*hi1 + hr;
    *I = -arg_r*hi1 - arg_i*hr1 + hi;
    break; /* G */

  case 'c':
  case 's':
    cos_arg_r  = cos(arg_r);
    r          =  2.0*cos_arg_r;
    /* summation loop */
    for (hr1 = 0.0, hr = *--p; a - p;) {
      hr2 = hr1;
      hr1 = hr;
      hr  = -hr2 + r*hr1 + *--p;
    }
    /* sum of the series */
    if (M == 'c')
    {
      *R = -hr1 + cos_arg_r*hr;
    }
    else
      *R = sin(arg_r)*hr;
    break; /* c, s */

  case 'g':
    r          =  2.0*arg_r;
    p ++;
    /* summation loop */
    for (hr1 = 0.0, hr = *--p; a - p;) {
      hr2 = hr1;
      hr1 = hr;
      hr  = -hr2 + r*hr1 + *--p;
    }
    /* sum of the series */
    *R = -arg_r*hr1 + hr;
    break; /* g */

  default :
  *R = *I = 0.0;
  }

  return(*R);

}
