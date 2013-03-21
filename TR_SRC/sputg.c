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

/* Copyright (c) 1989 LG, Kort-og Matrikelstyrelsen, Denmark   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of LG, Kort-og  */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include    <stdio.h>
#include    <math.h>
#include    <string.h>
#include    "KmsFncs.h"

int                      sputg(
/*___________________________*/
char                    *outstr,
double                   val,
struct typ_dec          *typ,
char                    *layout
)
{

  int             gr, tp, scf, cf, dc, fw;
  int             unit = 0, plus = 0, blnk = 0, zero = 0;
  double          factor = 1.0, g, m, s = 0.0, rd, w_val;
  char            *p, *p1, *np, *cstr, name[5], h[80];
  int             c, sgch, sgpr;
  unsigned int    mask = 1, upgrade = 0;
  struct typ_dec  typ1;

  /* decode typedecimal */
  if ((gr = get_tpd(typ, &tp, name, &cf, &dc, &factor)) > 0) {

    fw = cf + 1 + (dc ? dc + 1 : 0);
    if (dc == 255) {
      dc      = 0;
      fw      = 1;
      upgrade = 1;
    }

    /* decode layout */
    if (*layout != '\0')
      do {
        switch(*layout) {
        case ' ':
          blnk = 1;
          break;
        case '0':
          zero = 1;
          break;
        case '+':
          plus = 1;
          break;
        default:
          unit = 1;
          break;
        }
      } while (*(++layout));
    /* select layout of zero and plus */
    if (zero && (plus || blnk)) cstr = "%+0*.*f %s";
    else if (plus || blnk)      cstr = "%+*.*f %s";
    else if (zero)              cstr = "% 0*.*f %s";
    else cstr = "% *.*f %s";

    do {
      if (upgrade > 1) upgrade = 1;
      /* select output of units */
      np    = (unit ? name : "");
      w_val = val;

      /* select blanks in output */
      if (blnk) {
        switch(gr) {
        case 1: /*group 1*/
          switch(tp) {
          case 1: /*sx*/
          case 3: /*N*/
          case 4: /*S*/
          case 5: /*E*/
          case 6: /*W*/
            mask = ((mask << 2) + (mask << 4));
            scf  = 7;
            break;
          case 2: /*tm*/
            mask = ((mask << 1) + (mask << 3));
            scf  = 6;
            break;
          case 8: /*nt*/
            mask = ((mask << 2));
            scf  = 5;
            break;
          default:
            mask = 0L;
            scf  = 0;
            break;
          }; /*end of group 1*/
          break;
        case 2: /*group 2*/
          switch(tp) {
          case 5: /*cc*/
            mask = ((mask << 2) + (mask << 4));
            scf  = 7;
            break;
          case 6: /*gr*/
          case 7: /*gon*/
            mask = 0L;
            scf  = 3;
            break;
          default:
            mask = 0L;
            scf  = 3;
            break;
          }; /*end of group 2*/
          break;

        default: /*group 4-12*/
          mask = ((mask << 2) + (mask << 5) + (mask << 8)
               + (mask << 11) + (mask << 14));
          scf  = 18;
          break;
        } /*end of all groups*/
        if (scf > cf) mask = mask >> (scf - cf);
        else
        if (cf > scf) mask = mask << (cf - scf);
      } /* end mask actions */

      /* adjust angles to  (-2pi, +2pi) */
      if (gr < 3) {  /* angles */
        if ((gr == 1) && (tp == 4 /*S*/ || tp == 6 /*W*/))
             w_val = -w_val;
        while (w_val >   2.0*M_PI) w_val -= (2*M_PI);
        while (w_val <= -2.0*M_PI) w_val += (2*M_PI);
      };

      /* conv to wanted units */
      w_val *= factor;

      /* sexagesimal angles - excl. nonagesimal */
      if (gr == 1) {
        if (fabs(w_val) > 59.0) {
          /* round-off */
          rd = (w_val >= 0) ? 0.4999 : -0.4999;
          for (c = 0; c < dc; c++) rd *= 0.1;
          w_val += rd;

          if (tp < 8) /* seconds incl */ {
            s     = fmod(w_val, 60.0);
            w_val = (w_val - s)/60.0;
            if (upgrade && fabs(s - rd) < 0.01) {
              ++ upgrade;
              (void) strcpy(name, "nt");
            }
            m = fmod(w_val, 60.0);
            g = (w_val - m)/60.0;
            if (g > 359.5) g = 0.0;
          } else {
            m = fmod(w_val, 60.0);
            g = (w_val - m)/60.0;
            if (g > 359.5) g = 0.0;
            if (upgrade && fabs(m - rd) < 0.01) {
              ++ upgrade;
              (void) strcpy(name, "dg");
            }
          }
         
          w_val = (tp < 8)
              ? (1.0e4*g + 1.0e2*m + s - rd)
              : (1.0e2*g + m - rd);
        } else
        if (upgrade && fabs(w_val) < 0.5) {
          ++ upgrade;
          (void) strcpy(name, "dg");
        }
      } else
      if (upgrade) {
        if (gr == 2 && tp == 5) {
          s = fmod(w_val, 100.0);
          if (fabs(s) < 0.5) {
            ++ upgrade;
            (void) strcpy(name, "cc");
          }
        } else
        if (gr == 4 && (tp != 2 && tp <6)) {
          s = fmod(w_val, (tp == 1) ? 1000.0 : 10.0);
          if (fabs(s) < 0.5) {
            ++ upgrade;
            (void) strcpy(name, (tp == 1) ? "km" : (tp == 3) ? "cm" :
                         (tp == 4) ? "dm" : "m");
          }
        }
      }
      if (upgrade == 2) {
        typ1 = *set_tpd(name, 1, 0);
        gr   = get_tpd(&typ1, &tp, name, &cf, &dc, &factor);
        dc   = 0;
      }

    } while (upgrade > 1);

    if (upgrade) {
      cstr = (w_val < 0.0) ? " %-*.*f%s" : " %*.*f%s";
      if (*(np +1) == ' ') *(np +1) = '\0';
    }
    p = blnk ? h : outstr;

	(void) sprintf(p, cstr, fw, dc, w_val, np);

    /* Insert spaces in output */
    if (blnk) {
      sgch = (w_val >= 0 ? '+' : '-');
      sgpr = plus ? sgch : (w_val >= 0 ? ' ' : '-');
      for (p = h, p1 = outstr; *p; ) {
        c = *(p++);
        if (c != sgch) {
          *(p1++) = (char) c;
          if (mask & 1) *(p1++) = ' ';
          mask >>= 1;
        }
        else *(p1++) = (char) sgpr;
      };
      *p1 = '\0';
    } /* end of output with blanks */
  }
  return((int) strlen(outstr));
}
