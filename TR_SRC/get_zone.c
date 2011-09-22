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
 


/* get_zone    ver 2000.01          # page 1   19 jan 1999 10 19 */


/* Copyright (c) 1999  LG,  Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of LG, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

/* get_zone(name, lab) service function for conv_lab           */
/* NB should be called only from conv_lab                      */
/*    result ==  0 : no lab                                    */
/*    result ==  1 : coord lab             in lab              */
/*    UNDEFSYS     : unrecognized name                         */
/* an unrecognized name is considered a filename containing a  */
/* genuine label                                               */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "geo_lab.h"

#ifndef    M_PI
#include   "kms_math.h"
#endif

int                     get_zone(
/*_____________________________*/
union geo_lab          *u_lab
)
{
#include "conv_rgn.h"

  struct coord_lab          *c_lab = &(u_lab->u_c_lab);
  int                        zone = -1, i;
  char                      *p_sys = c_lab->mlb, rgn_name[16];

  if (c_lab->lab_type != CRD_LAB) return(-4);


/* get_zone    ver 2000.01          # page 2   19 jan 1999 10 19 */


  /* get zone of the coord systems */
  /*_______________________________*/
  switch(c_lab->cstm) {

  case  3: /* utm, itm, dks, asb, sb */
    switch (c_lab->mode) {
    case  0: /* utm**  */
    case  1: /* utm**n */
    case -1: /* utm**s */
    case 13: /* ETRS-TM, ETRS89 transverse mercator zone ** */
      i = (c_lab->mode == 13) ? 7 : 3;
      if (C_DIGIT(*(p_sys+i))) {
        if ((sscanf(p_sys+i, "%d", &zone) == 1)
            && (1 <= zone && zone <= 60)) {
          if (c_lab->L0 != (zone - 30.5)/30.0*M_PI) zone = -2;
        } else zone = -3;
      } else zone = -3;
      break;

    case 5: /* eetm**   Estonia */
    case 6: /* eetm*    Estonia */
      if (C_DIGIT(*(p_sys+4))) {
        if ((sscanf(p_sys+4, "%d", &zone) == 1)
            && (zone == 27 || zone == 24 || zone == 21
            || (0 <= zone && zone <= 2))) {
          if (zone > 10) {
            if (c_lab->E0 != ((zone == 24) ?  500000.0 :
                             ((zone == 27) ? 5500000.0 : 4500000.0)))
                zone = -2;
          }
          else {
            if (c_lab->E0 != zone * 1000000.0 + 250000.0) zone = -2;
          }
        } else zone = -3;
      } else zone = -3;
      break;

    case 8: /* tm**  */
      if (C_DIGIT(*(p_sys+2))) {
        if ((sscanf(p_sys+2, "%d", &zone) == 1)
            && (zone == 27 || zone == 24 || zone == 21)) {
          if (c_lab->L0 != zone*M_PI/180.0) zone = -2;
        } else zone = -3;
      } else zone = -3;
      break;

    }
    break; /* end case 3 : utm, itm, dks, asb, sb */


/* get_zone  ver 2000.01          # page 10   19 jan 1999 10 19 */


  case  5: /* lmb */
    switch(c_lab->mode) {
    case 1: /* lmb simple */
      if (c_lab->region == conv_rgn(-1, "GL", rgn_name)) {
         /* GI conf. con. W-Grønland or E-Grønland */
        if (C_DIGIT(*(p_sys + 2))
            && (sscanf(p_sys + 2, "%1d", &zone) == 1)
            && (1 <= zone && zone <= 8)) {
          if (c_lab->B0 != (84.0 - 3.0*zone)/180.0*M_PI) zone = -2;
        } else zone = -3;
      }
      break;
    }
    break; /* end case 5 : lmb */
  }

  return(zone);
}

