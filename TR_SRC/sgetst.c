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
 


/* sgetst  version 5.0              # page 1   30 sep 2009 10 19 */


/* Copyright (c) 2009, Danish National Space Center, DTU       */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of DNSC, DTU    */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */

#include  <stdio.h>
#include  <stddef.h>
#include  <string.h>
#include  <ctype.h>
#include  "geo_lab.h"
#include  "trthread.h"

#define    ERR    (17)

#define    DD     (54)
#define    DC     (63)
#define    DS     (72)
#define    DP     (81)
#define    CD     (90)
#define    CC     (99)
#define    CS    (108)
#define    CP    (117)
#define    SD    (126)

#define    DDD   (135)
#define    DDC   (144)
#define    DDS   (153)
#define    DDP   (162)
#define    DCD   (171)
#define    DSD   (180)
#define    DPD   (189)
#define    CDD   (198)
#define    CDC   (207)
#define    CCD   (216)
#define    CCC   (225)
#define    CSD   (234)
#define    CPC   (243)
#define    SDD   (252)

#define    DDDD  (261)
#define    CCCC  (270)



/* sgetst  version 4.0              # page 2   30 sep 2009 10 19 */


int                sgetst(
/*______________________*/
char               *in,
int                 lab_rgn,
struct st_str      *stn,
int                *tchar,
int                *used
)

{
#include            "conv_rgn.h"

  short             *typ = &stn->typ;
  short             *rgn = &stn->rgn;
  short             *hnr = &stn->hnr;
  short             *snr = &stn->snr;
  int               *lbn = &stn->lbn;
  short             *slt = &stn->slt;
  short             *suf = &stn->suf;
  char              *txt = (char *) &stn->hnr;

  static THREAD_SAFE  char       *niv_tab[] = {
    NIVTAB 
  };
  static THREAD_SAFE  int         nz = sizeof(niv_tab) / sizeof(*niv_tab);

  /* region prefix and name */
  union rgn_un       rgn_FI;
  char               rgn_name[24];
  int                clss, state = 18;
  int                i, c, u;
  char               ic, help[100], *p_h = help;
  char              *p = in, *ps, *st_ad = in;
  int                d;


  static THREAD_SAFE  unsigned short  first_5[288] = {
    /*             0     1     2     3     4     5     6     7     8 */
    /* INPUT class D     C     S     P    sp     c    CR     /  other*/
    /*            dig   CAP   SIG   POI         low   EOF   sla      */
    /* state    */

    /* the two first rows are filled with 0 (zero), this is done     */
    /* to prevent interfering with enumeration of stn_number_type,   */
    /* in the interval 0 to 17. See geo_lab.h for further details.   */
    /* 0. char  start   0 + 0 */
    /*       0*/    0,    0,    0,    0,    0,    0,    0,    0,    0,
    /*       9*/    0,    0,    0,    0,    0,    0,    0,    0,    0,


/* sgetst  version 4.0              # page 3   30 sep 2009 10 19 */


    /*             0     1     2     3     4     5     6     7     8 */
    /* INPUT class D     C     S     P    sp     c    CR     /  other*/

    /* 1. char  start  18 + 9 */
    /* init 18*/   27,   36,   45,  ERR,   18,  ERR,  ERR,  ERR,  ERR,

    /*             0     1     2     3     4     5     6     7     8 */
    /* INPUT class D     C     S     P    sp     c    CR     /  other*/
    /* 2. char >  dig   CAP   SIG   POI         low   EOF   sla      */
    /* D    27*/   DD,   DC,   DS,   DP, STNR, STNR, STNR,  ERR, STNR,
    /* C    36*/   CD,   CC,   CS,   CP, STXT, STXT, STXT,  ERR,  ERR,
    /* S    45*/   SD,  ERR,  ERR,  ERR,  ERR,  ERR,  ERR,  ERR,  ERR,

    /*             0     1     2     3     4     5     6     7     8 */
    /* INPUT class D     C     S     P    sp     c    CR     /  other*/
    /* 3. char >  dig   CAP   SIG   POI         low   EOF   sla      */
    /* DD   54*/  DDD,  DDC,  DDS,  DDP, STNR, STNR, STNR,  ERR, STNR,
    /* DC   63*/  DCD,  ERR,  ERR,  ERR, STNR, STNR, STNR,  ERR,  ERR,
    /* DS   72*/  DSD,  ERR,  ERR,  ERR,  ERR,  ERR,  ERR,  ERR,  ERR,
    /* DP   81*/  DPD,  ERR,  ERR,  ERR,  ERR,  ERR,  ERR,  ERR,  ERR,
    /* CD   90*/  CDD,  CDC,  ERR, PHNR, PHNR, PHNR, PHNR,  ERR,  ERR,
    /* CC   99*/  CCD,  CCC,  ERR,  ERR, STXT, STXT, STXT,  ERR, STXT,
    /* CS  108*/  CSD,  ERR,  ERR,  ERR,  ERR,  ERR,  ERR,  ERR,  ERR,
    /* CP  117*/  ERR,  CPC,  ERR,  ERR,  ERR,  ERR,  ERR,  ERR,  ERR,
    /* SD  126*/  SDD,  ERR,  ERR,  ERR, LBNR, LBNR, LBNR,  ERR, STNR,

    /*             0     1     2     3     4     5     6     7     8 */
    /* INPUT class D     C     S     P    sp     c    CR     /  other*/
    /* 4. char >  dig   CAP   SIG   POI         low   EOF   sla      */
    /* DDD 135*/ DDDD, STNR, LDNR, STNR, STNR, STNR, STNR, LINR, STNR,
    /* DDC 144*/ STNR,  ERR,  ERR,  ERR, STNR, STNR, STNR,  ERR,  ERR,
    /* DDS 153*/ LDNR,  ERR,  ERR,  ERR,  ERR,  ERR,  ERR,  ERR,  ERR,
    /* DDP 162*/ STNR,  ERR,  ERR,  ERR,  ERR,  ERR,  ERR,  ERR,  ERR,
    /* DCD 171*/ STNR,  ERR,  ERR,  ERR, STNR, STNR, STNR,  ERR,  ERR,
    /* DSD 180*/ LDNR,  ERR, LDNR,  ERR,  ERR,  ERR,  ERR,  ERR,  ERR,
    /* DPD 189*/ STNR,  ERR,  ERR,  ERR, STNR, STNR, STNR,  ERR,  ERR,
    /* CDD 198*/ PHNR, PHNR,  ERR, PHNR, PHNR, PHNR, PHNR,  ERR, STXT,
    /* CDC 207*/ PHNR, CCCC,  ERR,  ERR,  ERR,  ERR, STXT,  ERR,  ERR,
    /* CCD 216*/ STXT, STXT,  ERR,  ERR,  ERR,  ERR, STXT,  ERR,  ERR,
    /* CCC 225*/ CCCC, CCCC,  ERR, NVNR, STXT, STXT, STXT,  ERR,  ERR,
    /* CSD 234*/ LDNR,  ERR, LDNR,  ERR,  ERR,  ERR,  ERR,  ERR,  ERR,
    /* CPC 243*/  ERR,  ERR,  ERR, NVNR,  ERR,  ERR,  ERR,  ERR,  ERR,
    /* SDD 252*/ LBNR,  ERR,  ERR,  ERR, LBNR, LBNR, LBNR,  ERR,  ERR,

    /*             0     1     2     3     4     5     6     7     8 */
    /* INPUT class D     C     S     P    sp     c    CR     /  other*/
    /* 5. char >  dig   CAP   SIG   POI         low   EOF   sla      */
    /* DDDD261*/ STNR, STNR, LDNR, STNR, STNR, STNR, STNR, LINR, STNR,
    /* CCCC270*/ GPSR, STXT, STXT, STXT, GPSR, GPSR, GPSR, STXT, STXT,
    /* nil 279*/    0,    0,    0,    0,    0,    0,    0,    0,    0
  };


/* sgetst  version 4.0              # page 4   30 sep 2009 10 19 */


  *typ = *rgn = *hnr = *snr = 0;
  *slt = *suf = 0;
  *lbn = 0;

  /*
  pointers to in
  ps    = original, in case of STXT this one is used in the end.
  p     = wrk pointer
  st_ad = start address for in
  */

  while (state >= 18) {
    c = *(p++);
    clss
         = (C_DIGIT(c)) ? 0
         : (C_UPPER(c)) ? 1
         : (c == '-')   ? 2
         : (c == '.')   ? 3
         : (c == ' ')   ? 4
         : (C_LOWER(c)) ? 5
         : (c == '\n' ||
            c == '\0' || c == '\r' || c == '\f' || c == EOF) ? 6
         : (c == '/')   ? 7 
         : 8;
/*
(void) printf("\nstate = %4d class = %4d %4d  %c u=%d, l=%d",
state, clss,c,c, isupper(c), islower(c));
(void) printf("\nstate = %4d class = %4d ", state, clss);
*/
    state = first_5[state+clss];
/*
(void) printf(" -> ny state %4d", state);
*/
    if (state == 18) in += 1;
  }
  ps = in;
  p  = in;


/* sgetst  version 4.0              # page 5   30 sep 2009 10 19 */


  switch (state) {
  case STXT:
/* Special test for stations only holding text and starting with
   2 letters forming a valid national prefix. If the syntax rules
   are violated the national prefix becomes part of the text. 
*/
    while (((c = *p) != '\0') && (C_UPPER(c))) *(p_h++) = *(p++);
    *p_h = '\0';
    u    = (int) strlen(help);
    c    = (int) ((ptrdiff_t) p - (ptrdiff_t) st_ad);
    p    = in;

    if (u == 2) {
      if ((i = conv_rgn(-1, help, rgn_name)) > 0) {
        /*   *rgn = i; */
        p    += 2;
        *typ  = (short) sgetst(p, i, stn, tchar, &u);
        *used = c + u;
        if (0 <= *typ && *typ <= 7) *rgn  = (short) i;
        if (*rgn == 0)              *rgn  = (short) lab_rgn;
        return(*typ);
      } else {
        (void) strcpy(rgn_FI.prfx, "FI");
        if (lab_rgn == rgn_FI.r_nr[0]) {
          *typ = STXT;
          *rgn = (short) lab_rgn;
        } else return(-1);
      }
    }
    break;

  case LDNR:
    *typ = LDNR;
    if (!strncmp(p, "K-", 2)) {
      *hnr = 0;
      p    = p + 2;
    } else {
      (void) sscanf(p, "%d%n", &d, &u);
      if (0 <= d && d <= 32767)
        *hnr = (short) d;
      else
        *typ = STXT;
      p  += u + 1;
    }
    break;

  case STNR:
    *typ = STNR;
    break;

  case NVNR:
    *typ = STXT;
    for (i = 0; i < nz; i++)
      if (!strncmp(p, *(niv_tab + i), 4)) {
        *hnr = (short) (i + 1);
        p   += 4;
        *typ = NVNR;
      }
    break;


/* sgetst  version 4.0              # page 6   30 sep 2009 10 19 */


  case PHNR:
    *typ = PHNR;
    *hnr = *(p++);
    break;

  case DOSF:
    *typ = DOSF;
    break;

  case LBNR:
    *typ = LBNR;
    break;

  case LINR:
    *typ = LINR;
    break;

  case GPSR:
    *typ = GPSR;
    break;

  case ERR:
    /* (void) printf("\n**number ? %s", ps); */
    *typ = STXT;
    /* return(TERM); */

  default:
    *typ = STXT;
    break;
  }

  if (*rgn == 0)
    *rgn = (short) lab_rgn;

  /* establishing the stn record */
  switch (*typ) {

  case LDNR:
    (void) sscanf(p, "%d%n", &d, &u);
    if (0 <= d && d <= 32767)
      *snr = (short) d;
    else
      *typ = STXT;
    p += u;

    if (*p == '-') {
      p++;
      if (C_DIGIT(*p)) {
        (void) sscanf(p, "%d%n", lbn, &u);
        p += u;
      } 


/* sgetst  version 4.0              # page 7   30 sep 2009 10 19 */


      else if (C_UPPER(*p)) {
        *typ = DOSF;
        *slt = *(p++);
      } else {
        *typ = STXT;
        p += u;
        break;
      }
      if (*p == '.') {
        p++;
        (void) sscanf(p, "%d%n", &d, &u);
        if (0 <= d && d <= 32767)
          *suf = (short) d;
        else
          *typ = STXT;
        p += u;
      }
    } else
      *typ = STXT;
    break;

  case STNR: /* stationsnummer */
    (void) sscanf(p, "%d%n", lbn, &u);
    p += u;
    if (*p == '.') {
      *slt = *p;
      p++;
      (void) sscanf(p, "%d%n", &d, &u);
      if (0 <= d && d <= 32767)
        *suf = (short) d;
      else
        *typ = STXT;
      p   += u;
    } else if (C_UPPER(*p)) {
      *slt = *p;
      p++;
      if (C_DIGIT(*p)) {
        (void) sscanf(p, "%d%n", &d, &u);
        if (0 <= d && d <= 32767)
          *suf = (short) d;
        else
          *typ = STXT;
        p   += u;
      }
    }
    if (*lbn == -1) {
      *typ   = TERM;
      *tchar = *p;
    }
    break;


/* sgetst  version 4.0              # page 8   30 sep 2009 10 19 */


  case NVNR:
    (void) sscanf(p, "%d%n", &d, &u);
    if (0 <= d && d <= 32767)
      *snr = (short) d;
    else {
      *typ = STXT;
      break;
    }
    p    += u;
    if (*p == '/') {
      if (*hnr == 7 /* G.G. group nr */) {
        *typ = STXT;
        break;
      }
      p++;
      (void) sscanf(p, "%d%n", &d, &u);
      if (0 <= d && d <= 32767)
        *lbn = d;
      else {
        *typ = STXT;
        break;
      }
      p   += u;
    }
    if (*p == '.') {
      *slt = *p;
      p++;
      (void) sscanf(p, "%d%n", &d, &u);
      if (0 <= d && d <= 32767)
        *suf = (short) d;
      else {
        *typ = STXT;
        break;
      }
      p   += u;
    }
    break;

  case PHNR:
    (void) sscanf(p, "%d%n", &d, &u);
    if (*hnr == 'F') { /* F photo-numbers */
      *lbn  = d;
      p    += u;
      if (C_UPPER(*p)) *typ = STXT;
    } else
    if (*hnr == 'Q') { /* Q photo-numbers */
      (void) strcpy(help, p);
      *(help +4) = '\0';
      (void) sscanf(help, "%d%n", &d, &u);
      *snr  = (short) d;
      p    += u;
      if (!C_DIGIT(*p) || u < 4) *typ = STXT;
      (void) strcpy(help, p);
      *(help +4) = '\0';
      (void) sscanf(help, "%d%n", &d, &u);
      *lbn  = d;
      p    += u;


/* sgetst  version 4.0              # page 9   30 sep 2009 10 19 */


      if (!C_DIGIT(*p) || u < 4) *typ = STXT;
      (void) sscanf(p, "%d%n", &d, &u);
      if (0 <= d && d <= 32767)
        *slt = (short) d;
      else {
        *typ = STXT;
        break;
      }
      p   += u;
    } else { /* photo-numbers */
      if (0 <= d && d <= 32767)
        *snr = (short) d;
      else {
        *typ = STXT;
        break;
      }
      p    += u;
      if ((C_DIGIT(*(p+1)) || *(p+1) == '.') && C_ALPHA(*p)) {
        (void) sscanf(p, "%c%d%n", &ic, &d, &u);
        if (islower(ic)) ic = (char) toupper(ic);
        else
        if (ic == 'æ') ic = 'Æ';
        else
        if (ic == 'ø') ic = 'Ø';
        else
        if (ic == 'å') ic = 'Å';
        if (0 <= d && d <= 32767) {
          *lbn = ic;
          *slt = (short) d;
        } else {
          *typ = STXT;
          break;
        }
        p   += u;
      }
    }
    if (*p == '.') {
      p++;
      (void) sscanf(p, "%d%n", &d, &u);
      if (0 <= d && d <= 32767)
        *suf = (short) d;
      else {
        *typ = STXT;
        break;
      }
      p   += u;
    }
    break;


/* sgetst  version 4.0              # page 10   30 sep 2009 10 19 */


  case LBNR:
    (void) sscanf(p, "%d%n", lbn, &u);
    p += u;
    if (*lbn == -1) {
      *typ    = TERM;
      *tchar  = *p;
    } else
      *lbn *= -1;
    break;

  case LINR: /* landinspektørnummer / stationsnummer */
    (void) sscanf(p, "%d%n", &d, &u);
    if (0 <= d && d <= 9999)
      *hnr = (short) d;
    else
      *typ = STXT;
    p   += u;
    /* next char is '/' , so skip */
    if (*p != '/') {
      *typ = STXT;
      break;
    }
    p++;
    (void) sscanf(p, "%d%n", lbn, &u);
    p += u;

    if (*p == '.' || *p == '-') {
      *slt = *p;
      p++;
      (void) sscanf(p, "%d%n", &d, &u);
      if (0 <= d && d <= 32767)
        *suf = (short) d;
      else
        *typ = STXT;
      p   += u;
    } else if (C_UPPER(*p)) {
      *slt = *p;
      p++;
      if (C_DIGIT(*p)) {
        (void) sscanf(p, "%d%n", &d, &u);
        if (0 <= d && d <= 32767)
          *suf = (short) d;
        else
          *typ = STXT;
        p   += u;
      }
    }
    if (*lbn == -1) {
      *typ   = TERM;
      *tchar = *p;
    }
    break;


/* sgetst  version 4.0              # page 11   30 sep 2009 10 19 */


  case GPSR:
    for (i = 4; i; i --) *(txt ++) = *(p ++);
    /* test DDDDDCDDD */
    if (C_DIGIT(*p    ) && C_DIGIT(*(p+1)) && C_DIGIT(*(p+2)) &&
        C_DIGIT(*(p+3)) && C_DIGIT(*(p+4)) && C_UPPER(*(p+5)) &&
        C_DIGIT(*(p+6)) && C_DIGIT(*(p+7)) && C_DIGIT(*(p+8)) &&
       (C_LOWER(*(p+9)) || C_LOWER(*(p+9)) || C_SPACE(*(p+9))
                                             || *(p+9) == EOF)) {
      (void) sscanf(p, "%d%n", &d, &u);
      p    += u;
      *lbn  = d * 1000 + *(p ++);
      (void) sscanf(p, "%hd%n", slt, &u);
      p    += u;
      if (C_UPPER(*p)) *suf  = *(p ++);
    } else
    if (!(C_LOWER(*p) || C_SPACE(*p) || *p == '\0' || *p == EOF))  {
      *typ = STXT;
      txt  = (char *) &stn->hnr;
    }

    *used  = (*p == 'a') ? 
             (int) ((ptrdiff_t) (p +1) -(ptrdiff_t) st_ad) :
             (int) ((ptrdiff_t)  p     -(ptrdiff_t) st_ad);
    *tchar = *p;

    break;

  default:
    *typ = STXT;
    break;
  }

  /* type STXT can hold up to 11 uppercase characters and one \0 */
  if (*typ == STXT) {
    u = 11;
    while (((c = *(ps++)) != '\0') &&
           (u-- && (!C_SPACE(c) && c != EOF)))
      *(txt ++) = (char) c;
    *txt = '\0';
    ps--;

    /* read to first terminating character */
    while (((c = *ps) != '\0') && (!C_SPACE(c) && c != EOF)) ++ ps;
    *used  = (int) ((ptrdiff_t) ps - (ptrdiff_t) st_ad);
    if (*(ps -1) == 'a') {
      -- ps;
      if (*(txt -1) == 'a') *(txt -1) = '\0';
    }
    *tchar = *ps;
  } else {
    /* read to first terminating character */
    while (((c = *p) != '\0') && c != EOF
           && (!(C_LOWER(c) || C_SPACE(c) || C_PUNCT(c)))) ++ p;
    *used  = (int) ((ptrdiff_t) p - (ptrdiff_t) st_ad);
    if (c == 'a' || c == 'x' || c == 'z') ++ *used;
    *tchar = *p;
  }


/* sgetst  version 4.0              # page 11   30 sep 2009 10 19 */


  /* all zero as terminator */
  if (*lbn == 0 && *typ == 2)
    *typ = TERM;

  return (*typ);

}

#undef    ERR 

#undef    DD 
#undef    DC 
#undef    DS 
#undef    DP 
#undef    CD 
#undef    CC 
#undef    CS 
#undef    CP 
#undef    SD 

#undef    DDD 
#undef    DDC 
#undef    DDS 
#undef    DDP 
#undef    DCD 
#undef    DSD 
#undef    DPD 
#undef    CDD 
#undef    CDC 
#undef    CCD 
#undef    CCC 
#undef    CSD 
#undef    CPC 
#undef    SDD

#undef    DDDD


