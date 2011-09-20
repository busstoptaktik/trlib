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
 


/* inpoly      version 1.1        # page 1   10 Sep 1998 08 57 */


/* Copyright (c) 1996, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                   */

/* This is unpublished proprietary source code of GD, Kort-og  */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include  <math.h>

int          inpoly(
double      *pol_base,
double      *Nc,
double      *Ec
)

/* C function : inpoly

 inpoly        (call & return)         int 
               inpoly > 0   (Nc, Ec inside polygon )
               inpoly = 0   (Nc, Ec on polygon line)
               inpoly < 0   (Nc, Ec outside polygon)

 *pol_base     (call)                  * double
               pol_base[0] = number of corners in polygon
               pol_base[1] = 1. crd to 1. point          
               pol_base[2] = 2. crd to 1. point          
               pol_base[3] = 1. crd to 2. point          
               ................................          
               (Northing Easting osv.)

 *Nc           (call)                  * double
               1. coordinat for testing point in polygon

 *Ec           (call)                  * double
               2. coordinat for testing point in polygon


 test is made on cut between line(ph, pj) and line(N=*Nc, E<=*Ec)
  
 p_j =  p_h + 1
 *Nc  <*p_h  ==*p_h  >*p_h,   *Nc  <*p_j  ==*p_j  >*p_j 
 ca  =  0       3      6  ,   cb  =  0       1      2

 ca+cb == 0  : line over           point (*Nc)
 ca+cb == 1  : line over to        point (*Nc)
 ca+cb == 2  : line over to under  point (*Nc)
 ca+cb == 3  : line over from      point (*Nc)
 ca+cb == 4  : line through        point (*Nc)
 ca+cb == 5  : line under from     point (*Nc)
 ca+cb == 6  : line under to over  point (*Nc)
 ca+cb == 7  : line under to       point (*Nc)
 ca+cb == 8  : line under          point (*Nc)


 F. Steffensen FEB 1994
 K. Engsager   SEP 1998
*/


/* inpoly        version 1.         # page 2   10 Sep 1998 08 57 */


{
  int              cn, ca, cb, dia, noc, ocab = -1, scab = -1;
  double           s1, s2, ay, ax, hx, hy;
  double           *p_low, *p_high, *p_j, *p_h;

  dia    =   0;
  cn     =   0;
  noc    =   (int) *pol_base;
  noc   *=   2;
  p_low  =   pol_base +1;
  p_high =   p_low    +noc -2;

  if ((*p_low == *p_high) && *(p_low +1) == *(p_high +1)) {
    noc    -= 2;
    p_high  = p_low + noc -2;
  }
  ca       = (*p_low > *Nc) ? 0 : (*p_low == *Nc) ? 3 : 6;

  for (p_h = p_low   ; p_h <= p_high   ; p_h += 2) {
    p_j    = p_low +((p_h +2 -p_low) % noc) ;
    cb     = (*p_j > *Nc) ? 0 : (*p_j == *Nc) ? 1 : 2;

    switch (ca +cb) {
    case 0: /* line over  *Nc : no test */
    case 8: /* line under *Nc : no test */
      break;

    case 1: /* online test : j */
    case 7:
      if (*(p_j +1) == *Ec) {
        dia  = 1;
        p_h  = p_high;
      } else  /* special case peak at start/end point */
      if (*(p_h +1) < *Ec && p_j == p_low &&
          ((scab == 3 && ca + cb == 1) ||
           (scab == 5 && ca + cb == 7))) cn++;
      ocab = ca + cb;
      break;

    case 2: /* line over to under *NC : cut test */
    case 6: /* line under to over *NC : cut test */
      if ((*(p_h +1) < *Ec) ? (*Ec <= *(p_j +1)) : (*(p_j +1) <= *Ec)) {
        ax =  *(p_j +1) - *(p_h +1);
        hx =  *Ec       - *(p_h +1);
        ay = fabs(*p_j - *p_h);
        hy = fabs(*Nc  - *p_h);
        s1 = ax * hy;
        s2 = ay * hx; /* on line :: hx = ax * (hy/ay) :: s2 = s1 */

        if (fabs(s1 - s2) < ldexp((s1 + s2), -34)) {
          dia = 1;
          p_h = p_high;
        }
        else /* cut line :: hx > ax * (hy/ay) :: s2 > s1 */
          if (s1 < s2) cn++;
      }
      else
        if (*(p_h +1) < *Ec) cn++;
      ocab = 2;
      break;


/* inpoly        version 1.         # page 3   10 Sep 1998 08 57 */


    case 3: /* line over  from *Nc : online, cut- test : h */
    case 5: /* line under from *Nc : online, cut- test : h */
      if (*(p_h +1) == *Ec) {
        dia = 1;
        p_h = p_high;
      }
      else
      if (*(p_h +1) < *Ec) {
        /* if not peak increase */
        if (ocab != 1 && ca+cb == 3) cn++;
        else
        if (ocab != 7 && ca+cb == 5) cn++;
      }
      if (ocab == -1) scab = ca + cb;
      ocab = 3;
      break;

    case 4: /* line through *Nc : online test : h + j */
      if ((*(p_h +1) < *Ec) ? (*Ec <= *(p_j +1)) : (*(p_j +1) <= *Ec)) {
        dia = 1;
        p_h = p_high;
      }
      break;

    } /* switch (ca +cb) */
    ca   = cb *3;

  } /* for (h = .... */

  return ((dia == 1) ? 0 : ((cn % 2) == 1) ? 1 : -1);
}


