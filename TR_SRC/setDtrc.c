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
 


/* setDtrc                          # page 1    1 Mar 1993 11:18 */


/* Copyright (c) 1994 GD, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                      */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include    <stdio.h>
#include    "geo_lab.h"
#include    "trlib_api.h"
#include    "lord.h"

int                 setDtrc(
/*________________________*/
struct coord_lab   *c_lab,
double             *Dgtu)
{

  double                 n, np;

/*
KW == R. Koenig / K.H. Weise: Mathematische
Grundlagen der hoehren Geodaesie und Kartographie, Erster Band, 
Das Erdsphaeriod und seine konformen Abbildungen.
Springer-Verlag, Berlin/Goettingen/Heidelberg 1951.
*/

  if (c_lab->lab_type == 1 ) {
    /* Initialize the Derivatives transformation constants */


/* setDtrc                          # page 2    1 Mar 1993 11:18 */


    /* third flattening */
    np = n = c_lab->f/(2.0 - c_lab->f);
    /* coef of trig serie geo -> norm. Meridian length */
    /* KW p50 - 51 (97) - (97a) */

    /* Constants of the projections */
    switch(c_lab->cstm) {
    case 3: /* Transverse Mercator (UTM, ITM, etc) */
      /* coef of trig series */
      /* Dgtu := m, c,  KW p238 (22a) */
      *Dgtu     = n *(1.0 + n*(-4.0/3 + n*(  1.0/3  + n*  62.0/45)));
      np       *= n;
      *(Dgtu+1) = np*(          5.0/6 + n*(-26.0/15 + n*( 34.0/9)));
      np       *= n;
      *(Dgtu+2) = np*(                      16.0/15 + n*(-178.0/63));
      *(Dgtu+3) = np*                                 n*8159.0/1260;
      break;

    case 4: /* Mercator */
    case 5: /* Lambert */
    case 9: /* GS conf. con. */
    case 6: /* Stereographic coordinates */
    case 7: /* pseudo-equivalent projections */
      break;
    default: /* Programme error */
	    lord_error(TR_LABEL_ERROR,"setDtrc: Should only be used on TM-projection input.\n");
/*#ifndef _WIN32
      (void) fprintf(stderr, 
          "\n***setDtrc: Program- eller maskinfejl  - ak ak");
#endif */
      c_lab->lab_type = 0;
      break;
    } /* switch(c_lab->cstm) */
    return(c_lab->lab_type);

  } /* end of init transf-const.) */

  return(0);
}


