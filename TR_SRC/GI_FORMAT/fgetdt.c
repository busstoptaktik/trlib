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

/* Copyright (c) 2010, Danish National Space Center, DTU       */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of DNSC, DTU    */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */


#include    <stdio.h>
#include    <stddef.h>
#include    "KmsFncs.h"

double              fgetdt(
/*_______________________*/
double             *JD2000,
int                *used,
int                *term_c,
FILE               *fp
)
{

  double          cw;
  int             c, e, act, klasse, res;
  int             nst = 0, item = 0, n_item = 0, mode_ap = 0;
  char            outstr[72], *p_outstr = outstr;

  struct act_nst {
    short     action;
    short     nstate;
  };

  static struct act_nst tab[] = {
    /*       STATE  init  more  letter 2+sp  tab  init* more* more" */
    /* input          0     1     2     3     4     5     6     7   */
    /* CLASS     */
    /* 0 sp      */ {0,0},{0,1},{0,3},{2,1},{0,1},{0,5},{0,6},{1,7},
    /* 1 letter  */ {1,2},{1,2},{1,2},{1,2},{1,2},{0,5},{0,6},{1,7},
    /* 2 tab     */ {0,4},{2,4},{2,4},{2,4},{0,4},{0,5},{0,6},{2,7},
    /* 3 cr      */ {0,0},{7,9},{6,9},{6,9},{6,9},{0,5},{0,6},{7,9},
    /* 4 *       */ {0,5},{0,6},{2,6},{2,6},{0,6},{0,5},{0,6},{0,6},
    /* 5 ;       */ {0,0},{1,4},{1,4},{1,4},{1,4},{0,0},{0,1},{0,7},
    /* 6 "       */ {1,7},{1,7},{1,7},{1,7},{1,7},{0,5},{0,6},{1,7},
    /* 7 EOF     */ {8,9},{8,9},{8,9},{8,9},{8,9},{8,9},{8,9},{8,9},
    /* nil       */ {0,0}
  }, 
  *ptab;

  /* action = 0  no action          */
  /*        = 1  include letter     */
  /*        = 2  set double sp      */
  /*        = 6  terminate string   */
  /*        = 7  set \n and termin. */
  /*        = 8  set EOF            */

  /* state  = 0  start              */
  /*        = 1  more               */
  /*        = 2  after letter       */
  /*        = 3  after letter + sp  */
  /*        = 4  after tab          */
  /*        = 5  after init *       */
  /*        = 6  after more *       */
  /*        = 7  after more "       */
  /*        = 9  return  dummy      */

  *p_outstr = '\0';
  res       = 0;
  n_item    = 0;

  do {
    c = getc(fp);

    switch(c) {

    case '/' :
      klasse = 1;
      e = getc(fp);
      if ( e == '*') {
        c     = e;
        klasse = 4;
        break;
      }
      else
        (void) ungetc(e, fp);
      break;

    case  ' ':
      klasse = 0;
      break;

    case '\t':
      klasse = 2;
      break;

    case '\n':
    case '\r':
    case '\f':
      klasse = 3;
      break;

    case '*' :
      klasse = 4;
      c = getc(fp);
      if (c == '/') {
        klasse = 5;
        break;
      }
      else
        c = ungetc(c, fp);
      break;

    case ';' :
      klasse = 5;
      break;

    case '"' :
      klasse   = 6;
      mode_ap = 1 -mode_ap;
      break;

    case  EOF:
    case  0:
      klasse = (res > 0) ? 3 : 7;
      break;

    default:
      klasse = c > 32 ? 1 : 0;
      break;
    }

    if (klasse == 5 && mode_ap)
      nst      = 7;
    ptab       = tab + 8*klasse + nst;
    act        = ptab->action;
    nst        = ptab->nstate;

    switch(act) {
    case  0: /* no action */
      break;

    case 1: /* include acc char */
      *(p_outstr++) = (char) c;
      res++;
      item = 1;
      break;

    case 2: /* set double sp */
      *(p_outstr++) = ' ';
      *(p_outstr++) = ' ';
      if (item) {
        n_item ++;
        item = 0;
        *(p_outstr++) = '\0';
      }
      res        += 2;
      break;

    case 6: /* terminate string */
      *(p_outstr++) = (char) c;
      *(p_outstr++) = '\0';
      if (item) {
        n_item ++;
        item = 0;
      }
      res++;
      break;

    case 7: /* set \n and terminate */
      *(p_outstr++) = '\n';
      *(p_outstr++) = '\0';
      if (item) {
        n_item ++;
        item = 0;
      }
      res++;
      break;

    case 8: /* set EOF */
      *(p_outstr++) = EOF;
      *(p_outstr++) = '\0';
      if (item) {
        n_item ++;
        item = 0;
      }
      (void) ungetc(c, fp);
      res = EOF;
      break;

    default:
	  (void) lord_error(0,LORD("\n*** fgetdt programfejl"));
      return(-1.0);
    }
  } while (nst < 9 && n_item == 0);
  cw    = sgetdt(outstr, JD2000, used, term_c);
  *used = (int) ((ptrdiff_t) p_outstr - (ptrdiff_t) outstr);
  /*
  *used = (int) (p_outstr - outstr);
  */
  return(cw);
}
