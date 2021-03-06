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

/* Copyright (c) 1989 U/32, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "KmsFncs.h"

#ifdef _WIN32
#include <process.h>
#endif

#define DG3   {1,3}
#define DP4   {2,4}
#define NA0   {7,0}
#define NA1   {7,1}
#define NA2   {7,2}
#define NA3   {7,3}
#define NA4   {7,4}
#define RP2   {4,2}
#define RP4   {4,4}
#define RS1   {3,1}
#define EX5   {5,5}
#define AL6   {6,6}
#define MASK   (int) 0377

double                      sgetg(
/*______________________________*/
char                        *str,
struct typ_dec              *tpd,
int                         *used,
char                        *udt
)
{
/* tpd is zero iff no number is read from str */

  int              gr, tp, cf = 0, dc = 0, sc = 0;
  double           factor = 1., val = 0.;
  char             *p, *np, *ps, h[80], h1[80];
  int              i, c, len, f = 0;
  int              nst = 0, act, clss;
  int              g, s;
  struct typ_dec   itd;

  struct act_nst {
    short      action;
    short      nstate;
  };

  static  struct act_nst acns_tab[] = {
    /*state           init   sign    pnt     nmb   nmb+pnt */
    /*                 0      1       2       3       4    */
    /*class*/
    /*0 = digit*/     DG3,   DG3,    DP4,    DG3,    DP4,
    /*1 = sign */     RS1,   AL6,    RP2,    AL6,    AL6,
    /*2 = point*/     RP2,   RP2,    AL6,    RP4,    AL6,
    /*3 = space*/     NA0,   NA1,    NA2,    NA3,    NA4,
    /*4 = dblsp*/     NA0,   NA1,    AL6,    AL6,    AL6,
    /*5 = other*/     NA0,   AL6,    AL6,    EX5,    EX5
  };

  struct act_nst      *p_tab;

  itd.gf = itd.tf = itd.ff = itd.df = 0;

  len = (int) strlen(str);
  ps  = str;
  p   = h;
  np  = h1;
  *np = '\0';

  /* input loop */

  while ((nst < 5) && ((int) ((ptrdiff_t)str -(ptrdiff_t)ps) <= len)) {
    c = *(str++);
    /* class of input char */
    switch(c) {
    case '0': /* digit */
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      clss = 0;
      break;
    case '+': /* sign */
    case '-':
      clss = 1;
      break;
    case '.': /* point */
      clss = 2;
      break;
    case ' ': /* space */
      clss = ((f != ' ' || nst < 2) ? 3 : 4);
      break;
    default: /* all other */
      clss = 5;
    };

    p_tab = acns_tab + 5*clss + nst;
    nst   = (*(p_tab)).nstate;
    act   = (*(p_tab)).action;

    switch(act) {
    case 1: /* DG = include digit */
      *(p++) = (char) c;
      cf++;
      break;
    case 2: /* DP = incl. digit after point */
      *(p++) = (char) c;
      dc++;
      break;
    case 3: /* RS = record sign */
    case 4: /* RP = record point */
      *(p++) = (char) c;
      sc++;
      break;
    case 5: /* EX = ok exit */
    case 6: /* AL = alarm exit */
      *p = '\0';
      while (isalpha(c)) {			// C_ALPHA udskiftet med isalpha
        *(np++) = (char) c;
        c       = *(str++);
      };
      if (np != h1) {
        *np = '\0';
        np  = h1;
      }
      else np = udt;
      break;
    case 7: /* no action */
      break;
    default:
		(void) lord_error(0, LORD("programfejl i in_geo"));
      exit(1);
    };

    f = c;

  }; /* end of input loop */

  if (cf + dc) {
    itd  = *set_tpd(np, cf, dc);
    np   = h1;
    i    = get_tpd(&itd, &tp, np, &cf, &dc, &factor);

    if (i) {
      val = ((cf + dc) ? atof(h) : 0.0);
      gr  = itd.gf;
      tp  = itd.tf;
      /* test for sexages. group */
      if (gr == 1) {
        (void) sscanf(h, "%d", &g);
        if (fabs(val) >= 50.0) {
          s = g%100;
          g = g/100;
          if (tp < 8) /*sx, tm, N, S, E, W*/
            g = 60*(g/100) + g%100;
          g = 60*g +s;

          /* copy until after point */
          if (dc) {
            p = h + cf + sc;
            /* copy decimals as a string */
            (void) sprintf(h1, "%d.%s ", g, p);
            (void) sscanf(h1, "%lf", &val);
          }
          else val = g;
        }
        if (tp == 4 /*S*/ || tp == 6 /*W*/) val = -val;
      } /* end of sexages. test */
    }
  }
  else str = ps +1;

  *tpd  = itd;
  *used = (int) ((ptrdiff_t)str - (ptrdiff_t)ps - 1);

  return((val/factor));

}
