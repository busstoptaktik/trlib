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

/* Copyright (c) 2009, National Space Institute, DTU, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of DTU, Denmark */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */

/* srch_tab_typ(mode, f777, *tab_typ_txt) searches the entry   */
/* to a table type                                             */
/*  result  >  0  : tab_typ f777                               */
/*  result == -2  : STOP or EOF                                */
/*                                                             */
/* *lab_typ       : Kind of table (or STP_LAB) See geo_lab.h   */
/*                                                             */
/* *tab_typ_txt may be :: (acording to *lab_typ)               */
/*        "geoid", "dtmtab", "dhtab", "upltab", "demtab",      */
/*        "tabtab", "bagtab", "t3dtab", "t2dtab", "tddtab"     */
/* *pos           : after "REC_TABLE" for next recursive call  */

#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <fcntl.h>
#include    <sys/types.h>
#include    "KmsFncs.h"
#include    "trthread.h"

int                      srch_tab_typ(
/*__________________________________*/
int                     *lab_typ,
char                    *tab_typ_txt,
long                    *pos
)
{

extern THREAD_SAFE FILE               *man_tab_file;
extern THREAD_SAFE size_t              init_tab_pos;

#define  LAST_TP       (767)
// To be edited when new table types are added

  int                 stype, tab_typ = -2;
  char                pth_mlb[512];
  int                 f777, qr, used;

  stype    = STP_LAB;
  qr       = fgetlhtx(man_tab_file, pth_mlb);
  // to enshure 2 sp between entities
  *pos  = ftell(man_tab_file) - (long) strlen(pth_mlb);
  (void) fseek(man_tab_file, *pos, SEEK_SET);
  if (qr != EOF) qr = fgetln_kms(pth_mlb, &used, man_tab_file);

  if (qr != EOF) {
    for (f777 = 777 ; f777 >= LAST_TP && stype == STP_LAB; f777 --) {
      switch (f777) {
      case 777:
        /* fgetln_kms gives items with 2<sp> between them */
        if (!strncmp(pth_mlb, "geoid  manager  777", 19)) {
          (void) strcpy(tab_typ_txt, "geoid");
          stype    = GDE_LAB;
          tab_typ = f777;
        }
        break;
      case 776:
        if (!strncmp(pth_mlb, "dtmtab  manager  776", 20)) {
          (void) strcpy(tab_typ_txt, "dtmtab");
          stype    = DTM_LAB;
          tab_typ = f777;
        }
        break;
      case 775:
        if (!strncmp(pth_mlb, "dhtab  manager  775", 19)) {
          (void) strcpy(tab_typ_txt, "dhtab");
          stype    = DHH_LAB;
          tab_typ = f777;
        }
        break;
      case 774:
        if (!strncmp(pth_mlb, "upltab  manager  774", 20)) {
          (void) strcpy(tab_typ_txt, "upltab");
          stype    = UPL_LAB;
          tab_typ = f777;
        }
        break;
      case 773:
        if (!strncmp(pth_mlb, "demtab  manager  773", 20)) {
          (void) strcpy(tab_typ_txt, "demtab");
          stype    = DEM_LAB;
          tab_typ = f777;
        }
        break;
      case 772:
        if (!strncmp(pth_mlb, "tabtab  manager  772", 20)) {
          (void) strcpy(tab_typ_txt, "tabtab");
          stype    = TAB_LAB;
          tab_typ = f777;
        }
        break;
      case 771:
        if (!strncmp(pth_mlb, "bagtab  manager  771", 20)) {
          (void) strcpy(tab_typ_txt, "bagtab");
          stype    = BAG_LAB;
          tab_typ = f777;
        }
        break;

      case 770:
        if (!strncmp(pth_mlb, "t3dtab  manager  770", 20)) {
          (void) strcpy(tab_typ_txt, "t3dtab");
          stype    = T3D_LAB;
          tab_typ = f777;
        }
        break;
      case 769:
        if (!strncmp(pth_mlb, "t2dtab  manager  769", 20)) {
          (void) strcpy(tab_typ_txt, "t2dtab");
          stype    = T2D_LAB;
          tab_typ = f777;
        }
        break;
      case 768:
        if (!strncmp(pth_mlb, "tddtab  manager  768", 20)) {
          (void) strcpy(tab_typ_txt, "tddtab");
          stype    = TDD_LAB;
          tab_typ = f777;
        }
        break;
      case 767:
        /* (void) !strncmp(pth_mlb, "stop  manager", 13); */
        stype    = STP_LAB;
        tab_typ = f777;
      }
    } /* f777 loop */
  } else {
    tab_typ = LAST_TP;
  }

  if (stype == STP_LAB) *pos = (long) init_tab_pos;
  else {  /* pos to REC_TABLE */
    do {
      qr   = fgetln_kms(pth_mlb, &used, man_tab_file);
      used = !strncmp(pth_mlb, "REC_TABLE", 9);
    } while (qr > 0 && (used == 0));
  }

  *lab_typ = stype;
  return(tab_typ);
}

