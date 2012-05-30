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
 


/* sputst   version 4.0             # page 1   29 Apr 2005 11 22 */


/* Copyright (c) 2005, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                   */

/* This is unpublished proprietary source code of GD, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not */
/* indicate an intention of publishing this code.             */

#include  <ctype.h>
#include  <stdio.h>
#include  <string.h>
#include  "geo_lab.h"

int                       sputst(
/*_____________________________*/
    char                 *o_str,
    int                   lab_rgn,
    struct st_str        *stn,
    char                 *rgx,
    int                   wdt
)

{

#include                 "conv_rgn.h"

  short                  *typ = &stn->typ;
  short                  *rgn = &stn->rgn;
  short                  *hnr = &stn->hnr;
  short                  *snr = &stn->snr;
  int                    *lbn = &stn->lbn;
  short                  *slt = &stn->slt;
  short                  *suf = &stn->suf;
  char                   *txt = (char *) &stn->hnr;
  char                    sa[64], *ps = sa;
  char                    na[64], *pn = na;
  char                    xa[64], *px = xa;
  char                    ra[64], *pr = ra;
  int                     d1, d2, d3;
  int                     i, in_EE = 0;
  char                    c, c2, prf_chars = 4;

  /* region number, prefix and name */
  char                    rgn_pref[8], rgn_name[24];

  static  char            *niv_tab[] = {
    NIVTAB
  };


/* sputst   version 4.0             # page 2   29 Apr 2005 11 22 */


  /* find number of \n put rest in rgs */
  *pn = *pr = '\0';
  if (0 <= *typ && *typ <= 8) {
    (void) strcpy(px, rgx);
    while (*px != '\0') {
      if (*px  == '\n')
        *(pn++) = '\n';
      else
        *(pr++) = *px;
      px++;
    }
  }

  *pn = '\0';
  pn  = na;
  pr  = ra;
  px  = xa;

  switch (*pr) {
  case '\0':
    *px       = '\0';
    prf_chars = 0;
    break;
  case '-':
    (void) sprintf(px, "%4s", " ");
    break;
  case '+':
    px = xa;
    *px = '\0';
    if (conv_rgn(*rgn, rgn_pref, rgn_name) > 0) {
      if (*rgn == lab_rgn)
        (void) sprintf(px, "%4s", " ");
      else {
        (void) sprintf(px, "%-2s%2s", rgn_pref, " ");
       }
       in_EE    = !strncmp(rgn_pref, "EE", 2);
     }
     else
    (void) sprintf(px, "%4s", " ");
    break;
  default:
    if (conv_rgn(-1, ra, rgn_name) > 0)
      (void) sprintf(px, "%-2s%2s", ra, " ");
    break;
  }

  (void) strcat(pn, px);
  *px = '\0';



/* sputst   version 4.0             # page 3   29 Apr 2005 11 22 */


  switch (*typ) {
  case LDNR:        /* landsnummer */
    if (*hnr) {
      if (in_EE)
        (void) sprintf(ps, "%3d-%03d-%05d", *hnr, *snr, *lbn);
      else
        (void) sprintf(ps, "%3d-%02d-%05d", *hnr, *snr, *lbn);
    }
    else
      (void) sprintf(ps, " K -%02d-%05d", *snr, *lbn);
    (void) strcat(px, ps);
    if (*suf) {
      (void) sprintf(ps, ".%d", *suf);
      (void) strcat(px, ps);
    }
    break;

  case STNR:        /* stationsnummer */
  case TERM:        /* terminator */
    d1 = *lbn;
    if (d1 < 0)
      d1 = -d1;
    d3 = d1 % 1000;
    d2 = (d1 / 1000) % 1000;
    d1 = d1 / 1000000;
    c  = (char) *slt;
    if (d1) {
      if (*lbn < 0)
        d1 = -d1;
      (void) sprintf(ps, "%3d %03d %03d", d1, d2, d3);
    }
    else if (d2) {
      if (*lbn < 0)
        d2 = -d2;
      (void) sprintf(ps, "%3d %03d", d2, d3);
    }
    else {
      if (*lbn < 0)
        d3 = -d3;
      (void) sprintf(ps, "    %3d", d3);
    }
    (void) strcat(px, ps);


/* sputst   version 4.0             # page 4   29 Apr 2005 11 22 */


    if (c == '.') {
      (void) sprintf(ps, ".%d", *suf);
      (void) strcat(px, ps);
    }
    else if (C_UPPER(c)) {
      (void) sprintf(ps, " %c", c);
      (void) strcat(px, ps);
      if (*suf) {
        (void) sprintf(ps, " %d", *suf);
        (void) strcat(px, ps);
      }
    }
    break;

  case DOSF:        /* Spec. Niv. nummer */
    if (*hnr)
      (void) sprintf(ps, "%3d", *hnr);
    else
      (void) sprintf(ps, " K ");
    (void) strcat(px, ps);
    c = (char) *slt;
    (void) sprintf(ps, "-%02d- %c", *snr, c);
    (void) strcat(px, ps);
    if (*suf) {
      (void) sprintf(ps, ".%d", *suf);
      (void) strcat(px, ps);
    }
    break;

  case NVNR:        /* NIV- and Gravity nmb */
    d1 = *snr;
    d2 = *lbn;
    (void) sprintf(ps, "%s%d", *(niv_tab + *hnr - 1), d1);
    (void) strcat(px, ps);
    if (d2) {
      (void) sprintf(ps, "/%d", d2);
      (void) strcat(px, ps);
    }
    if (*suf) {
      (void) sprintf(ps, ".%d", *suf);
      (void) strcat(px, ps);
    }
    break;

  case PHNR:        /* Fotogramm. nr */
    if (*hnr == 'F') { /* F photonumber */
      (void) sprintf(ps, "F%d", *lbn);
      (void) strcat(px, ps);
    } else
    if (*hnr == 'Q') { /* Q photonumber */
      (void) sprintf(ps, "Q%04d%04d%04d", *snr, *lbn, *slt);
      (void) strcat(px, ps);


/* sputst   version 4.0             # page 5   29 Apr 2005 11 22 */

    } else { /* photonumber */
      /* i vild uorden */
      i = *hnr;
      (void) sprintf(ps, "%c%04d", i, *snr);
      (void) strcat(px, ps);
      i = *lbn;
      if (i) {
        (void) sprintf(ps, "%c%04d", i, *slt);
        (void) strcat(px, ps);
      }
    }
    if (*suf) {
      (void) sprintf(ps, ".%d", *suf);
      (void) strcat(px, ps);
    }
    break;

  case LBNR:
    (void) sprintf(ps, "-%05d", *lbn);
    (void) strcat(px, ps);
    break;

  case LINR:        /* Lsp number as prefix for stationsnummer */
    (void) sprintf(ps, "%4d/", *hnr);
    (void) strcat(px, ps);
    d1 = *lbn;
    if (d1 < 0)
      d1 = -d1;
    d3 = d1 % 1000;
    d2 = (d1 / 1000) % 1000;
    d1 = d1 / 1000000;
    c = (char) *slt;
    if (d1) {
      if (*lbn < 0)
        d1 = -d1;
      (void) sprintf(ps, "%d %03d %03d", d1, d2, d3);
      /* (void) sprintf(ps, "%3d %03d %03d", d1, d2, d3); */
    }
    else if (d2) {
      if (*lbn < 0)
        d2 = -d2;
      (void) sprintf(ps, "%d %03d", d2, d3);
      /* (void) sprintf(ps, "%3d %03d", d2, d3); */
    }
    else {
      if (*lbn < 0)
        d3 = -d3;
      (void) sprintf(ps, "%d", d3);
      /* (void) sprintf(ps, "    %3d", d3); */
    }
    (void) strcat(px, ps);


/* sputst   version 4.0             # page 5   29 Apr 2005 11 22 */

    if (c == '.' || c == '-') {
      (void) sprintf(ps, "%c%04d", c, *suf);
      (void) strcat(px, ps);
    }
    else if (C_UPPER(c)) {
      (void) sprintf(ps, " %c", c);
      (void) strcat(px, ps);
      if (*suf) {
        (void) sprintf(ps, " %d", *suf);
        (void) strcat(px, ps);
      }
    }
    break;

  case GPSR:          /* GPS CHARCODE : CCCCDDDDDCDDD (and more) */
    for (i = 4; i; i --) *(ps ++) = *(txt ++); /* safe at big *lbn */
    *ps = '\0';
    ps  = sa;
    if (*lbn) {
      (void) strcat(px, ps);
      c  = (char) (*lbn % 1000);
      d1 =         *lbn / 1000;
      c2 = (char) *suf;
      if (!*suf) (void) sprintf(ps, " %05d%c%03d", d1, c, *slt);
      else       (void) sprintf(ps, " %05d%c%03d%c", d1, c, *slt, c2);
    }
    (void) strcat(px, ps);
    break;

  default:
    (void) sprintf(ps, "%s", txt);
    (void) strcat(px, ps);
    break;
  }
  (void) sprintf(ps, "%*s", wdt, px);
  (void) sprintf(o_str, "%s%s", pn, ps);

  return ((int) strlen(ps) + prf_chars);

}

