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
 


/* ee_trans   version 3.0           # page 1   10 Sep 1998 11 36 */


/* Copyright (c) 1998 GD, Kort-og Matrikelstyrelsen, Denmark   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

/*
#define  DEBUGEETRANS
*/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include "geo_lab.h"
#include "trthread.h"

/* ACTION NUMBERS */
/* termination action */
#define  IDT    0

/* regular estonian mappings */
#define  ETG    1
#define  GTE    2
#define  OTG    3
#define  GTO    4
#define  VTG    5
#define  GTV    6
#define  TTG    7
#define  GTT    8
/* predictions euref <-> estonian */
#define  UPE    9
#define  PEU   10
#define  OPE   11
#define  PEO   12
#define  UPV   13
#define  PVU   14

#ifdef   DEBUGEETRANS

static char          *ACTION[] = {
  "idt", "etg", "gte", "otg", "gto", "vtg", "gtv",
  "ttg", "gtt", "upe", "peu", "ope", "peo",
  "upv", "pvu", "ctg", "gtc"
};

#endif

int                    ee_trans(
/*___________________________*/
union geo_lab       *in_lab_u,
union geo_lab       *outlab_u,
double               N,
double               E,
double               H,
double              *Nout,
double              *Eout,
double              *Hout,
char                *usertxt,
FILE                *tr_error
)

{
#include        "conv_lab.h"
#include        "ptg.h"
#include        "eetmtu_g.h"
#include        "t_status.h"


/* ee_trans   version 2.0           # page 2   10 Sep 1998 11 36 */


  static THREAD_SAFE  int  in_chsum = 0;
  static THREAD_SAFE  int  outchsum = 0;
  static  int  init = 0;

  char                     in_cs[32], outcs[32];
  char                     p_dtm[32], *pl;

  int                      in_gr, outgr, outnr = 0, ee_z;
  int                      nst, gst, act, level;
  int                      result = 0, res = 0;

  struct coord_lab        *in_lab = &(in_lab_u->u_c_lab);
  struct coord_lab        *outlab = &(outlab_u->u_c_lab);

  static union geo_lab     TC_u35;
  static union geo_lab     TC_eetm;
  static union geo_lab     TC_eold;
  static union geo_lab     TC_elmn;


  /* minilabels */

  struct nr_mlb {
    short        trgr;
    short        trnr;
    char         *s_lab;
  } *pml;

  static struct nr_mlb    mlab[] = {
    /*  0 */ { 0,  0,   "utm35"},    /* Datum checked later  */

    /*  1 */ { 1,  0,   "eetm27"},   /*                      */
    /*  2 */ { 1,  1,   "eetm24"},   /*                      */
    /*  3 */ { 1,  1,   "eetm21"},   /*                      */
    /*  4 */ { 1,  1,   "eetm0"},    /*                      */
    /*  5 */ { 1,  1,   "eetm1"},    /*                      */
    /*  6 */ { 1,  1,   "eetm2"},    /*                      */
    /*  7 */ { 1,  2,   "eegeo"},    /*                      */
    /*  8 */ { 1,  3,   "eecrt"},    /*                      */

    /*  9 */ { 2,  0,   "tm27"},     /* Datum checked later  */
    /* 10 */ { 2,  1,   "tm24"},     /* Datum checked later  */
    /* 11 */ { 2,  1,   "tm21"},     /* Datum checked later  */
    /* 12 */ { 2,  4,   "geo"},      /* Datum checked later  */
    /* 13 */ { 2,  5,   "crt"},      /* Datum : _eeold42     */

    /* 14 */ { 3,  0,   "eelmne"},   /*                      */
    /* 15 */ { 3,  1,   "eelmse"},   /*                      */
    /* 16 */ { 3,  2,   "geo"},      /* Datum : _eepv37      */
    /* 17 */ { 3,  3,   "crt"},      /* Datum : _eepv37      */

    /*stop*/ {-1, -1,   ""}
  };



/* ee_trans   version 2.0           # page 3   10 Sep 1998 11 36 */


  struct act_nst {
    short     action;
    short     nstate;
  };

  /* Start values: *ptab->row, in_nr->col */
  static THREAD_SAFE  struct act_nst *ptab, *pt[3];
  static THREAD_SAFE  int             in_nr, stlev, levst, in[3];
  static int                          ee_w, ol_w, pv_w, gr_w;

  /* Action/state table :: inter group */
  static struct act_nst gr_tab[] = {
    /* utm35_euref89: 0 */
    /* input    u35      etm      tmo     eelmne  */
    /* state no. 0        1        2        3     */
    /* u35 */ {IDT,0}, {PEU,0}, {OPE,1}, {PVU,0},
    /* eetm27: 1  (pul42 predifined) */
    /* etm */ {UPE,1}, {IDT,1}, {OPE,1}, {PVU,0},
    /* tm27_eeold42: 2  */
    /* tmo */ {UPE,1}, {PEO,2}, {IDT,2}, {PVU,0},
    /* eelmne: 3  (eepv37 predefined) */
    /*elmne*/ {UPV,3}, {PEU,0}, {OPE,1}, {IDT,3}
  };

  /* Action/state table grp 1 : eesti42 */
  static struct act_nst ee_tab[] = {
    /* eetm27: 0 */
    /* input    etm      tmi      geo     */
    /* state no. 0        1        2      */
    /* etm */ {IDT,0}, {TTG,2}, {GTE,0},
    /* eetm*: 1 ; * == 0/1/2/21/24/27  */
    /* tmi */ {ETG,2}, {TTG,2}, {GTT,2},
    /* geo_eesti42: 2 */
    /* geo */ {ETG,2}, {TTG,2}, {IDT,2}
  };

  /* Action/state table grp 2 : eeold42 */
  static struct act_nst ol_tab[] = {
    /* tm27_eeold42: 0 */
    /* input    tmo      tmi      geo     */
    /* state no. 0        1        2      */
    /* tmo */ {IDT,0}, {TTG,2}, {GTO,0},
    /* tm**: 1 ; ** == 21/24/27  */
    /* tmi */ {OTG,2}, {TTG,2}, {GTT,2},
    /* geo_eeold42: 2 */
    /* geo */ {OTG,2}, {TTG,2}, {IDT,2}
  };

  /* Action/state table grp 3 : eepv37 */
  static struct act_nst pv_tab[] = {
    /* eelmne: 0 */
    /* input    lmn      lms      geo     */
    /* state no. 0        1        2      */
    /* lmn */ {IDT,0}, {TTG,2}, {GTV,0},
    /* eetm*: 1 ; * == 0/1/2/21/24/27  */
    /* lms */ {VTG,2}, {IDT,1}, {GTT,1},
    /* geo_eepv37: 2 */
    /* geo */ {VTG,2}, {TTG,2}, {IDT,2}
  };



/* ee_trans   version 2.0           # page 4   10 Sep 1998 11 36 */


  if (init == 0) {
    /* Internal wrk-labels */
    act = (conv_lab("eetm27",        &TC_eetm, "") == CRD_LAB
       &&  conv_lab("tm27_eeold42",  &TC_eold, "") == CRD_LAB
       &&  conv_lab("eelmne",        &TC_elmn, "") == CRD_LAB
       &&  conv_lab("utm35_euref89", &TC_u35,  "") == CRD_LAB);

    /* State/action table size and width */
    ee_z = sizeof(gr_tab)/sizeof(struct act_nst);
    gr_w = (int) sqrt(1.0000001*ee_z);
    act  = act && (ee_z == gr_w*gr_w);
    ee_z = sizeof(ee_tab)/sizeof(struct act_nst);
    ee_w = (int) sqrt(1.0000001*ee_z);
    act  = act && (ee_z == ee_w*ee_w);
    ee_z = sizeof(ol_tab)/sizeof(struct act_nst);
    ol_w = (int) sqrt(1.0000001*ee_z);
    act  = act && (ee_z == ol_w*ol_w);
    ee_z = sizeof(pv_tab)/sizeof(struct act_nst);
    pv_w = (int) sqrt(1.0000001*ee_z);
    init = act && (ee_z == pv_w*pv_w);

    if (!init) return((tr_error==NULL) ? TRF_ILLEG_ :
                       t_status(tr_error, usertxt,
                       "ee_trans(init lab error)", TRF_ILLEG_));
  }

  /* Check i/o labels, init of actual transf. systems */
  if (in_chsum != in_lab->ch_sum || outchsum != outlab->ch_sum) {

    /* Coord labels */
    if   ( in_lab->lab_type != CRD_LAB
        || outlab->lab_type != CRD_LAB) {
      return((tr_error==NULL) ? TRF_ILLEG_ :
              t_status(tr_error, usertxt,
              "ee_trans(i/o labels error)", TRF_ILLEG_));
    }



/* ee_trans   version 2.0           # page 5   10 Sep 1998 11 36 */


    /* Out-system */
    /*____________*/

    /* Datum actions */
    (void) strcpy(outcs, outlab->mlb);
    pl = outcs + outlab->sepix;
    if (*pl != '\0') (void) strcpy(p_dtm, (pl+1));
    else *p_dtm = '\0';
    *pl = '\0';

    /* Group and number */
    for (pml = mlab, outgr = -1; pml->trgr != -1; pml++)
      if (!strcmp(pml->s_lab, outcs)) {
        outgr = pml->trgr;
        outnr = pml->trnr;
#ifdef DEBUGEETRANS
(void) printf("\n*OUT  grp = %4d    nr = %4d   navn = %s, call = %s;",
pml->trgr, pml->trnr, pml->s_lab, outcs);
#endif
        break;
      }

    /* Label and region check */
    switch (outgr) {
    case 0: /* Label */
      if (!strcmp(p_dtm, "eeref89"))
        return((tr_error==NULL) ? TRF_ILLEG_ :
                t_status(tr_error, usertxt,
                "ee_trans(missing : EE_ or ill_lab)", TRF_ILLEG_));
      break;
    case 1:
      break;
    case 2:
    case 3:
      if (outnr >= 4) {
        if (!strcmp(p_dtm, "eeold42")) outnr -= 2;
        else
        if (!strcmp(p_dtm, "eepv37")) {
          outgr  = 3;
          outnr -= 2;
        }
      }
      break;

    default: /* Unknown label */
      return((tr_error==NULL) ? TRF_ILLEG_ :
              t_status(tr_error, usertxt,
              "ee_trans(unknown o-label)", TRF_ILLEG_));
    } /* end out-label check */



/* ee_trans   version 2.0           # page 6   10 Sep 1998 11 36 */


    /* In-system */
    /*___________*/

    /* Datum actions */
    (void) strcpy(in_cs, in_lab->mlb);
    pl = in_cs + in_lab->sepix;
    if (*pl != '\0') (void) strcpy(p_dtm, (pl+1));
    else *p_dtm = '\0';
    *pl = '\0';

    /* Group and number */
    for (pml = mlab, in_gr = -1; pml->trgr != -1; pml++)
      if (!strcmp(pml->s_lab, in_cs)) {
        in_gr = pml->trgr;
        in_nr = pml->trnr;
#ifdef DEBUGEETRANS
(void) printf("\n*IN   grp = %4d    nr = %4d   navn = %s;",
pml->trgr, pml->trnr, pml->s_lab);
#endif
        break;
      }

    /* Label and region check */
    switch (in_gr) {
    case 0: /* Label */
      if (!strcmp(p_dtm, "eeref89"))
        return((tr_error==NULL) ? TRF_ILLEG_ :
                t_status(tr_error, usertxt,
                "ee_trans(missing : EE_ or ill_lab)", TRF_ILLEG_));
      break;
    case 1:
      break;
    case 2:
    case 3:
      if (in_nr >= 4) {
        if (!strcmp(p_dtm, "eeold42")) in_nr -= 2;
        else
        if (!strcmp(p_dtm, "eepv37")) {
          in_gr  = 3;
          in_nr -= 2;
        }
      }
      break;

    default: /* Unknown label */
      return((tr_error==NULL) ? TRF_ILLEG_ :
              t_status(tr_error, usertxt,
              "ee_trans(unknown i-label)", TRF_ILLEG_));

    } /* end in-label check */

    if (in_nr == 3 || outnr == 3)
      return((tr_error==NULL) ? TRF_ILLEG_ :
              t_status(tr_error, usertxt,
              "ee_trans(crt_) ILLEGAL", TRF_ILLEG_));

    /* Save check-sums */
    in_chsum = in_lab->ch_sum;
    outchsum = outlab->ch_sum;

    /* Test identical labels */
    if (in_lab->ch_sum == outlab->ch_sum) {
      in_gr = outgr = 0;
      in_nr = outnr = 0;
    }
#ifdef  DEBUGEETRANS
(void) fprintf(stdout, "\n* in = %s, out = %s", in_cs, outcs);
(void) fprintf(stdout, "\n ee_trans in_nr     %d  outnr      %d;",
in_nr, outnr);
#endif

    /* set levels and tables */
    if (in_gr == outgr) {
      stlev = 2;
      levst = 2;
      in[2] = in_nr;
    } else {
      if (in_nr == 0) stlev = 1;
      else {
        stlev = 0;
        switch(in_gr) {
        case 0: /*euref89*/ stlev = 1;      break;
        case 1: /*eesti42*/ pt[0] = ee_tab; break;
        case 2: /*eeold42*/ pt[0] = ol_tab; break;
        case 3: /*eepv37 */ pt[0] = pv_tab; break;
        }
      }
      in[0] = in_nr;
      pt[1] = gr_tab + gr_w * outgr;
      in[1] = in_gr;
      in[2] = 0;
      levst = (outnr == 0) ?  1 : 2;
    }
    switch(outgr) {
    case 0: /*euref89*/ pt[2] = gr_tab;                break;
    case 1: /*eesti42*/ pt[2] = ee_tab + ee_w * outnr; break;
    case 2: /*eeold42*/ pt[2] = ol_tab + ol_w * outnr; break;
    case 3: /*eepv37 */ pt[2] = pv_tab + pv_w * outnr; break;
    }

  } /* End of init actions */



/* ee_trans   version 2.0           # page 7   10 Sep 1998 11 36 */


  /* transformation module */
  *Hout = H;
  for (level = stlev; level <= levst; level ++) {
    nst  = in[level];               /* input col   */
    ptab = pt[level];               /* input table */

    do {
      gst = nst;
      act = (ptab+nst)->action;
      nst = (ptab+nst)->nstate;

#ifdef  DEBUGEETRANS
(void) fprintf(stdout,
"\n*EE_TRANS ACTION  %-6s: STATE %2d -> %2d, LEVEL = %d",
ACTION[act], gst, nst, level);
#endif



/* ee_trans   version 2.0           # page 8   10 Sep 1998 11 36 */


      switch(act) {

      case ETG: /* eetm27 -> eegeo */
        res = ptg(&TC_eetm, +1, N, E, &N, &E, usertxt, tr_error);
        break;
      case GTE: /* eegeo -> eetm27 */
        res = ptg(&TC_eetm, -1, N, E, &N, &E, usertxt, tr_error);
        break;
      case OTG: /* tm27_eeold42 -> geo_eeold42 */
        res = ptg(&TC_eold, +1, N, E, &N, &E, usertxt, tr_error);
        break;
      case GTO: /* geo_eeold42 -> tm27_eeold42 */
        res = ptg(&TC_eold, -1, N, E, &N, &E, usertxt, tr_error);
        break;
      case VTG: /* eelmne -> geo_eepv37 */
        res = ptg(&TC_elmn, +1, N, E, &N, &E, usertxt, tr_error);
        break;
      case GTV: /* geo_eepv37 -> eelmne */
        res = ptg(&TC_elmn, -1, N, E, &N, &E, usertxt, tr_error);
        break;
      case TTG: /* eetm -> eegeo */
        res = ptg(in_lab_u,  +1, N, E, &N, &E, usertxt, tr_error);
        break;
      case GTT: /* eegeo -> eetm */
        res = ptg(outlab_u,  -1, N, E, &N, &E, usertxt, tr_error);
        break;

      case UPE: /* utm35_euref89 -> eetm27 */
        res = eetmtu_g(-1, N, E, &N, &E, usertxt, tr_error);
        break;
      case PEU: /* eetm27 -> utm35_euref89 */
        res = eetmtu_g(+1, N, E, &N, &E, usertxt, tr_error);
        break;
      case PEO: /* eetm27 -> tm27_eeold42 */
        res = eetmtu_g(-2, N, E, &N, &E, usertxt, tr_error);
        break;
      case OPE: /* tm27_eeold42 -> eetm27 */
        res = eetmtu_g(+2, N, E, &N, &E, usertxt, tr_error);
        break;
      case UPV: /* utm35_euref89 -> eelmne */
        res = eetmtu_g(-3, N, E, &N, &E, usertxt, tr_error);
        break;
      case PVU: /* eelmne -> utm35_euref89 */
        res = eetmtu_g(+3, N, E, &N, &E, usertxt, tr_error);
        break;

      case IDT: /* ident, no action */
        break;


  
/* ee_trans   version 2.0           # page 9   10 Sep 1998 11 36 */


      default: /* programme error */
        return((tr_error==NULL) ? TRF_ILLEG_ :
                t_status(tr_error, usertxt,
                "ee_trans(prog error)", TRF_PROGR_));
      }
      if (res < result) result = res;

    } while (nst != gst);
  } /* level LOOP */

  /* Return coord and result */
  *Nout = N;
  *Eout = E;
  return(result);

  /* UNDEFINE ACTION NUMBERS */

#undef  IDT
#undef  ETG
#undef  GTE
#undef  OTG
#undef  GTO
#undef  VTG
#undef  GTV
#undef  TTG
#undef  GTT
#undef  UPE
#undef  PEU
#undef  OPE
#undef  PEO
#undef  UPV
#undef  PVU

}


