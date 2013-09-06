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

#include    <stdio.h>
#include    <math.h>
#include    <string.h>
#include    <ctype.h>
#include    "KmsFncs.h"
#include    "trthread.h"

/* ACTION NUMBERS */
#define  IDNT    0
#define  T32J    1
#define  JT32    2
#define  T32S    3
#define  ST32    4
#define  T32B    5
#define  BT32    6
#define  U32G    7
#define  GU32    8
#define  P_TG    9
#define  G_TP   10
#define  ZTZU   11
#define  ZTZD   12
#define  U_GS   13
#define  GS_U   14
#define  UGSB   15
#define  GSBU   16
#define  J_OS   17
#define  OS_J   18
#define  S_KK   19
#define  KK_S   20
#define  U_SB   21
#define  SB_U   22
#define  PTGS   23
#define  GSTP   24
#define  PTBS   25
#define  BSTP   26
#define  SDKS   27
#define  DKSS   28
#define  T32U   29
#define  U32T   30
#define  XIDT   31
#define  ILLG   32
#define  TGTP   33
#define  GTTP   34
#define  P_TP   35
#define  T32L   36
#define  LT32   37

/* #define  DEBUGDKTRANS */

int                   dk_trans(
/*___________________________*/
struct coord_lab        *in_lab,
struct coord_lab        *outlab,
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

  static THREAD_SAFE  int               in_chsum = 0;
  static THREAD_SAFE  int               outchsum = 0, proj_proj = 0;
  static int               TC_init = 0;

  char                    *pl;

  int                      in_gr, outgr, outnr = 0, ed_z;
  int                      nst, act, level;
  int                      result = 0, res = 0, ijs = 0;
  double  Njs[] = { 186000.0, 113000.0,  89000.0,  56000.0,      0.0};
  double  Ejs[] = { 154000.0, 180000.0, 175000.0, 170000.0, 177000.0};
  double  N32[] = {6219861.0,6146376.0,6122487.0,6109593.0,6033467.0};
  double  E32[] = { 641594.0, 617062.0, 622538.0, 627794.0, 622297.0};

 

  static THREAD_SAFE char  outcs[32], in_cs[32];
  static struct coord_lab      TC_u32, TC_u33, TC_tr2, TC__gs, TC_gsb;

#ifdef   DEBUGDKTRANS
  static char          *ACTION[] = {
    "idnt", "t32j", "j32t", "t32s", "st32", "t32b", "bt32",
    "u32g", "gu32", "p_tg", "g_tp", "ztzu", "ztzd", "u_gs", "gs_u",
    "ugsb", "gsbu", "j_os", "os_j", "s_kk", "kk_s", "u_sb", "sb_u",
    "ptgs", "gstp", "ptbs", "bstp", "sdks", "dkss", "t32u", "u32t",
    "xidt", "illg", "tgtp", "gttp", "p_tp", "t32l", "lt32"
  };
#endif

  struct nr_mlb {
    short        trgr;
    short        trnr;
    char         *s_lab;
  } *pml;


  static struct nr_mlb    mlab[] = {
    /* GENERAL GLOBAL */
    /*  0 */ { 0,  0,   "tc32_ed50"},  /* tr system to/from EUREF */
    /*  1 */ { 0,  1,   "geo_ed50"},
    /*  2 */ { 0,  2,   "utm32_ed50"},
    /*  3a*/ { 0,  3,   "sb"},
    /*  3b*/ { 0,  3,   "sbf"},
    /*  4 */ { 0,  4,   "dm"},
    /*  5 */ { 0,  5,   "tcgeo_ed50"},  /* tr_sys to/from EUREF */
    /*  6 */ { 0,  6,   "crt_ed50"},  /* illegal */
    /*  7      0,  7,   "*_ed50",    general projection in ed50 */

    /* Jylland + Sjælland */
    /*  0      1,  0,   "tc32_ed50",   */
    /*  1 */ { 1,  1,   "s34j"},
    /*  2 */ { 1,  2,   "s34s"},
    /*  3 */ { 1,  3,   "os"},
    /*  4 */ { 1,  4,   "kk"},
    /* Øresund */
    /*  5a*/ { 1,  5,   "dks"},
    /*  5b*/ { 1,  5,   "rt90v"},
    /*  5c*/ { 1,  5,   "rt38v"},
    /*  6 */ { 1,  6,   "s34"},

    /* Bornholm */
    /*  0      2,  0,   "tc32_ed50",    */
    /*  1 */ { 2,  1,   "s45b"},
    /*  2      2,  2,   "utm32_ed50",   */

    /* General Staben */
    /*  0      3,  0,   "tc32_ed50",    */
    /*  1 */ { 3,  1,   "gs"},
    /*  2      3,  2,   "utm32_ed50",   */
    /*  3a*/ { 3,  3,   "geo_gs"},
    /*  3b*/ { 3,  3,   "gsgeo"},

    /* General Stabens Bornholm */
    /*  0      4,  0,   "tc32_ed50",    */
    /*  1 */ { 4,  1,   "gsb"},
    /*  2      4,  2,   "utm32_ed50",   */
    /*  3a*/ { 4,  3,   "geo_gsb"},
    /*  3b*/ { 4,  3,   "gsbgeo"},
    /*  4 */ { 4,  4,   "utm33_ed50"},

    /*stop*/ {-1, -1,   ""}
  };

  struct act_nst {
    short     action;
    short     nstate;
  };

  /* Start values: *ptab->row, in_nr->col */
  static THREAD_SAFE struct act_nst *ptab, *pt[2];
  static THREAD_SAFE int             in_nr, stlev, levst, in[2];
  static int                         ed_w, js_w, bo_w, gs_w, bs_w;

  static struct act_nst edtab[] = {
    /* i/o_sys :: TC 32: 0 */
    /* input    tc32     geo      u32      sb      proj     tcgeo */
    /* state no. 0        1        2        3        4        5   */
    /* tc32*/ {IDNT,0},{GU32,2},{U32T,0},{SB_U,2},{P_TG,1},{TGTP,0},
    /* GEO:   1 */
    /* geo */ {T32U,2},{IDNT,1},{U32G,1},{SB_U,2},{P_TG,1},{TGTP,0},
    /* UTM32: 2 */
    /*utm32*/ {T32U,2},{GU32,2},{IDNT,2},{SB_U,2},{P_TG,1},{TGTP,0},
    /* SB:    3 */
    /* sb  */ {T32U,2},{GU32,2},{U_SB,3},{IDNT,3},{P_TG,1},{TGTP,0},
    /* PROJ:  4 */
    /* proj*/ {T32U,2},{G_TP,4},{U32G,1},{SB_U,2},{P_TP,4},{TGTP,0},
    /* TCGE:  5 */
    /* tcge*/ {U32G,5},{GTTP,2},{U32T,0},{SB_U,2},{P_TG,1},{IDNT,5}
  };

  static struct act_nst jstab[] = {
    /* i/o_sys :: TC 32: 0 */
    /* input     : */
    /* tc32    s34j     s34s      os       kk       dks      s34  */
    /* state no. : */
    /*   0        1        2        3        4        5        6   */
    /* tc32 : 0 */
    {IDNT,0},{JT32,0},{ST32,0},{OS_J,1},{KK_S,2},{DKSS,2},{LT32,0},
    /* S34 J: 1 */
    {T32J,1},{IDNT,1},{ST32,0},{OS_J,1},{ILLG,0},{ILLG,0},{IDNT,6},
    /* S34 S: 2 */
    {T32S,2},{JT32,0},{IDNT,2},{ILLG,0},{KK_S,2},{DKSS,2},{IDNT,6},
    /* OS:    3 */
    {T32J,1},{J_OS,3},{ILLG,0},{IDNT,3},{ILLG,0},{ILLG,0},{ILLG,0},
    /* KK:    4 */
    {T32S,2},{ILLG,0},{S_KK,4},{ILLG,0},{IDNT,4},{DKSS,2},{ILLG,0},
    /* DKS:   5 */
    {T32S,2},{ILLG,0},{SDKS,5},{ILLG,0},{KK_S,2},{XIDT,5},{ILLG,0},
    /* s34:   6 */
    {T32L,6},{IDNT,6},{IDNT,6},{ILLG,0},{ILLG,0},{ILLG,0},{IDNT,6}
  };

  static struct act_nst botab[] = {
    /* i/o_sys :: TC 32: 0 */
    /* input    tc32    s45b      u32  */
    /* state no. 0        1        2   */
    /* tc32*/ {IDNT,0},{BT32,0},{U32T,0},
    /* S45B:  1 */
    /* s45b*/ {T32B,1},{IDNT,1},{U32T,0},
    /* UTM32: 2 */
    /* u32 */ {T32U,2},{BT32,0},{IDNT,2}
  };

  static struct act_nst gstab[] = {
    /* i/o_sys :: TC32: 0 */
    /* input   tc32       gs     u32      gsgeo  */
    /* state no. 0        1        2        3    */
    /* tc32*/ {IDNT,0},{GS_U,2},{U32T,0},{GSTP,1},
    /* GS:    1 */
    /* gs  */ {T32U,2},{IDNT,1},{U_GS,1},{GSTP,1},
    /* u32:   2 */
    /* u32 */ {T32U,2},{GS_U,2},{IDNT,2},{GSTP,1},
    /* GSGEO: 3 */
    /*gsgeo*/ {T32U,2},{PTGS,3},{U_GS,1},{IDNT,3}
  };

  static struct act_nst bstab[] = {
    /* i/o_sys :: TC32: 0 */
    /* input   tc32      gsb      u32    gsbgeo    u33   */
    /* state no. 0        1        2        3        4   */
    /* tc32*/ {IDNT,0},{GSBU,4},{U32T,0},{BSTP,1},{ZTZD,2},
    /* GS:    1 */
    /* gs  */ {T32U,2},{IDNT,1},{ZTZU,4},{BSTP,1},{UGSB,1},
    /* u32:   2 */
    /* u32 */ {T32U,2},{GSBU,4},{IDNT,2},{BSTP,1},{ZTZD,2},
    /* GSBGEO: 3 */
    /*gsbgeo*/{T32U,2},{PTBS,3},{ZTZU,4},{IDNT,3},{UGSB,1},
    /* U33:   4 */
    /* u33 */ {T32U,2},{GSBU,4},{ZTZU,4},{BSTP,1},{IDNT,4}
  };

  if (!TC_init) {
    act = conv_w_crd("DK_tc32_ed50",  &TC_tr2) == 1
        &&    conv_w_crd("DK_utm32_ed50", &TC_u32) == 1
        &&    conv_w_crd("DK_utm33_ed50", &TC_u33) == 1
        &&    conv_w_crd("DK_gs",         &TC__gs) == 1
        &&    conv_w_crd("DK_gsb",        &TC_gsb) == 1;

    ed_z    = sizeof(edtab)/sizeof(struct act_nst);
    ed_w    = (int) sqrt(1.0000001*ed_z);
    act = act && ed_z == ed_w*ed_w;
    ed_z    = sizeof(jstab)/sizeof(struct act_nst);
    js_w    = (int) sqrt(1.0000001*ed_z);
    act = act && ed_z == js_w*js_w;
    ed_z    = sizeof(botab)/sizeof(struct act_nst);
    bo_w    = (int) sqrt(1.0000001*ed_z);
    act = act && ed_z == bo_w*bo_w;
    ed_z    = sizeof(gstab)/sizeof(struct act_nst);
    gs_w    = (int) sqrt(1.0000001*ed_z);
    act = act && ed_z == gs_w*gs_w;
    ed_z    = sizeof(bstab)/sizeof(struct act_nst);
    bs_w    = (int) sqrt(1.0000001*ed_z);
    TC_init = act && ed_z == bs_w*bs_w;

    if (!TC_init)
      return((tr_error==NULL) ? TRF_PROGR_ :
              t_status(tr_error, usertxt,
              "dk_trans(init labels)", TRF_PROGR_));
    }

  /* Check i/o labels, init of actual transf. systems */
  if (in_chsum != in_lab->ch_sum || outchsum != outlab->ch_sum) {

    /* Coord labels */
    if   ( in_lab->lab_type != CRD_LAB
        || outlab->lab_type != CRD_LAB) {
      return((tr_error==NULL) ? TRF_PROGR_ :
              t_status(tr_error, usertxt,
              "dk_trans(i/o labels error)", TRF_PROGR_));
    }

    /* Out-system */
    /*____________*/

    (void) strcpy(outcs, outlab->mlb);
    pl = outcs + outlab->sepix;
    if (outlab->h_ix) *(outcs + outlab->h_ix -1) = (char) '\0';
    if (isupper(*pl)) *pl = (char) ((*(pl+1) != (char) '\0') ? '_' : '\0');		// C_UPPER udskiftet med isupper 20120529 stl
    /* Group and number */
    for (pml = mlab, outgr = -1; pml->trgr != -1; pml++)
      if (!strcmp(pml->s_lab, outcs)) {
        outgr = pml->trgr;
        outnr = pml->trnr;
#ifdef DEBUGDKTRANS
		lord_debug(0,LORD("OUT  grp = %4d    nr = %4d   navn = %s, call = %s"),
			pml->trgr, pml->trnr, pml->s_lab, outcs);
#endif
        break;
      }

    if (outgr == 0 && outnr == 6) {
      return((tr_error==NULL) ? TRF_PROGR_ :
              t_status(tr_error, usertxt,
              "dk_trans(illegal crt_ed50)", TRF_PROGR_));
    }

    /* Datum actions */
    if (outgr == -1) {
      if (outlab->datum == TC_u32.datum) {
        outgr = 0;  // ed50
        outnr = 4;
#ifdef DEBUGDKTRANS
pml = mlab +8;
lord_debug(0,LORD("OUT  grp = %4d    nr = %4d   navn = %s, call = %s"),
outgr, outnr, pml->s_lab, outcs);
#endif
      } else
        return((tr_error==NULL) ? TRF_ILLEG_ :
                t_status(tr_error, usertxt,
                "dk_trans(unknown o-label)", TRF_ILLEG_));
      }

    /* In-system */
    /*____________*/

    (void) strcpy(in_cs, in_lab->mlb);
    pl = in_cs + in_lab->sepix;
    if (in_lab->h_ix) *(in_cs + in_lab->h_ix -1) = '\0';
    if (isupper(*pl)) *pl = (char) ((*(pl+1) != (char) '\0') ? '_' : '\0');		// C_UPPER udskiftet med isupper 20120529 stl
    /* Group and number */
    for (pml = mlab, in_gr = -1; pml->trgr != -1; pml++)
      if (!strcmp(pml->s_lab, in_cs)) {
        in_gr = pml->trgr;
        in_nr = pml->trnr;
#ifdef DEBUGDKTRANS
		lord_debug(0,LORD("IN   grp = %4d    nr = %4d   navn = %s, call = %s"),
pml->trgr, pml->trnr, pml->s_lab, in_cs);
#endif
        break;
      }

    if (in_gr == 0 && in_nr == 6) {
      return((tr_error==NULL) ? TRF_ILLEG_ :
              t_status(tr_error, usertxt,
              "dk_trans(illegal crt_ed50)", TRF_ILLEG_));
    }

    /* Datum actions */
    if (in_gr == -1) {
      if (in_lab->datum == TC_u32.datum) {
        in_gr = 0;  // ed50
        in_nr = 4;
#ifdef DEBUGDKTRANS
pml = mlab +8;
lord_debug(0,LORD("IN   grp = %4d    nr = %4d   navn = %s, call = %s"),
outgr, outnr, pml->s_lab, in_cs);
#endif
      } else
      return((tr_error!=NULL) ? TRF_ILLEG_ :
              t_status(tr_error, usertxt,
              "dk_trans(unknown i-label)", TRF_ILLEG_));
    }

    /* Save check-sums */
    in_chsum = in_lab->ch_sum;
    outchsum = outlab->ch_sum;

    /* Test identical labels */
    if (in_chsum == outchsum) {
      in_gr = outgr = 0;
      in_nr = outnr = 0;
    } else
    if (outgr >= 2 && in_gr == 0 && in_nr == 2) in_gr = outgr;
    else
    if (in_gr >= 2 && outgr == 0 && outnr == 2) outgr = in_gr;
    else
    if (outgr == 0 && in_gr == 4 && in_nr == 4) in_gr = outgr;
    else
    if (in_gr == 0 && outgr == 4 && outnr == 4) outgr = in_gr;
    proj_proj = in_gr == 0 && outgr == 0
             && in_nr == 4 && outnr == 4;
#ifdef  DEBUGDKTRANS
lord_debug(0,LORD(" in = %s, in_gr,nr %d, %d,"),
in_cs, in_gr, in_nr);
lord_debug(0,LORD(" out = %s, outgr,nr %d, %d, proj_proj = %d"),
outcs, outgr, outnr, proj_proj);
#endif

    /* set levels and tables */
    levst = 1;
    if (in_gr == outgr) {
      stlev = 1;
      in[1] = in_nr;
    } else {
      if (in_gr == 0 && (in_nr == 0)) {
        stlev = 1;
        in[1] = 0;
      } else {
        stlev = 0;
        in[0] = in_nr;
        in[1] = 0;
        switch(in_gr) {
        case 0: pt[0] = edtab; break;
        case 1: pt[0] = jstab; break;
        case 2: pt[0] = botab; break;
        case 3: pt[0] = gstab; break;
        case 4: pt[0] = bstab; break;
        }
        if (outgr == 0 && outnr == 0) levst = 0;
      }
    }
    switch(outgr) {
    case 0: pt[1] = edtab + ed_w * outnr; break;
    case 1: pt[1] = jstab + js_w * outnr; break;
    case 2: pt[1] = botab + bo_w * outnr; break;
    case 3: pt[1] = gstab + gs_w * outnr; break;
    case 4: pt[1] = bstab + bs_w * outnr; break;
    }

  } /* end of init after input def */

  /* transformation module */
  *Hout = H;
  for (level = stlev; level <= levst; level ++) {
    nst  = in[level];               /* input col   */
    ptab = pt[level];               /* input table */
    do {
      act = (ptab+nst)->action;
      nst = (ptab+nst)->nstate;

#ifdef  DEBUGDKTRANS
lord_debug(0,LORD("DK_TRANS ACTION  %-6s: STATE -> %2d, LEVEL = %d"),
ACTION[act], nst, level);
#endif

      switch(act) {

      /* REGULAR TRANSFORMATIONS */
      case U32G: /* utm32 -> geo */
        res = ptg(&TC_u32, +1, N, E, &N, &E, usertxt, tr_error);
        break;
      case GU32: /* geo -> utm32 */
        res = ptg(&TC_u32, -1, N, E, &N, &E, usertxt, tr_error);
        break;
      case ZTZD: /* utm33 -> utm32 */
        res = ptg(&TC_u33, +2, N, E, &N, &E, usertxt, tr_error);
        if (res < result) result = res;
        res = ptg(&TC_u32, -2, N, E, &N, &E, usertxt, tr_error);
        break;
      case ZTZU: /* utm32 -> utm33 */
        res = ptg(&TC_u32, +2, N, E, &N, &E, usertxt, tr_error);
        if (res < result) result = res;
        res = ptg(&TC_u33, -2, N, E, &N, &E, usertxt, tr_error);
        break;
      case P_TG: /* proj -> geo */
        res = ptg(in_lab, +1, N, E, &N, &E, usertxt, tr_error);
        break;
      case G_TP: /* geo -> proj */
        res = ptg(outlab, -1, N, E, &N, &E, usertxt, tr_error);
        break;
      case P_TP: /* proj -> proj (or IDNT) */
        if (proj_proj) {
          res = ptg(in_lab, +1, N, E, &N, &E, usertxt, tr_error);
          if (res < result) result = res;
          res = ptg(outlab, -1, N, E, &N, &E, usertxt, tr_error);
        }
        act   = IDNT; /* stop */
        *Nout = N;
        *Eout = E;
        break;
      case TGTP: /* tcgeo_ed50 -> tc32_ed50 */
      case GTTP: /* geo_ed50 -> utm32_ed50 */
        if (fabs(M_PI*0.06667 - E) < M_PI*0.03333)
        res = ptg(&TC_u32, -1, N, E, &N, &E, usertxt, tr_error);
        else nst = (act == TGTP) ? 1 : 5;
        /* when totally outside DK, where utm32_ed50 == tc32_ed50 */
        break;

      case PTGS: /* gs -> geo */
        res = ptg(&TC__gs, +1, N, E, &N, &E, usertxt, tr_error);
        break;
      case GSTP: /* geo -> gs */
        res = ptg(&TC__gs, -1, N, E, &N, &E, usertxt, tr_error);
        break;
      case PTBS: /* gsb -> geo */
        res = ptg(&TC_gsb, +1, N, E, &N, &E, usertxt, tr_error);
        break;
      case BSTP: /* geo -> gsb */
        res = ptg(&TC_gsb, -1, N, E, &N, &E, usertxt, tr_error);
        break;

        /* UTM ED50 */
      case T32U: /* tc32 -> utm32 */
        res = tcts_u(N, E, &N, &E, 'u',  1, usertxt, tr_error);
        break;
      case U32T: /* utm32 -> tc32 */
        res = tcts_u(N, E, &N, &E, 'u', -1, usertxt, tr_error);
        break;

        /* DK */
      case T32J: /* tc32 -> s34j */
        res = tcts_u(N, E, &N, &E, 'j',  1, usertxt, tr_error);
        break;
      case JT32: /* s34j -> tc32 */
        res = tcts_u(N, E, &N, &E, 'j', -1, usertxt, tr_error);
        break;
      case T32S: /* tc32 -> s34s */
        res = tcts_u(N, E, &N, &E, 's',  1, usertxt, tr_error);
        break;
      case ST32: /* s34s -> tc32 */
        res = tcts_u(N, E, &N, &E, 's', -1, usertxt, tr_error);
        break;
      case T32L: /* tc32 -> s34  */
        if (N > 6033467.0) {
          if (E < +775000.0) { /* Jylland + Sjælland */
            while (N < *(N32 + ijs)) ijs ++;
            res = (E < *(E32 + ijs))
                ? tcts_u(N, E, &N, &E, 'j',  1, usertxt, tr_error)
                : tcts_u(N, E, &N, &E, 's',  1, usertxt, tr_error);
          } else /* BORNHOLM */
            res = tcts_u(N, E, &N, &E, 'b',  1, usertxt, tr_error);
        } else
        if (tr_error!=NULL)
           (void) t_status(tr_error, usertxt, "dk_trans", TRF_AREA_,
                           "m ", "", N, E, 0.0, 0.0);
        break;
      case LT32: /* s34  -> tc32 */
        if (N > 0.0) {
          if (N > 75000.0 || E > +65000.0) { /* Jylland + Sjælland */
            while (N < *(Njs + ijs)) ijs ++;
            res = (E < *(Ejs + ijs))
                ? tcts_u(N, E, &N, &E, 's', -1, usertxt, tr_error)
                : tcts_u(N, E, &N, &E, 'j', -1, usertxt, tr_error);
          } else /* BORNHOLM */
            res = tcts_u(N, E, &N, &E, 'b', -1, usertxt, tr_error);
        } else
        if (tr_error!=NULL)
           (void) t_status(tr_error, usertxt, "dk_trans", TRF_AREA_,
                        "m ", "", N, E, 0.0, 0.0);
        break;

      case U_GS: /* utm32 -> gs  */
        res = utgs(N, E, &N, &E, 'd', +1, usertxt, tr_error);
        break;
      case GS_U: /* gs -> utm32  */
        res = utgs(N, E, &N, &E, 'd', -1, usertxt, tr_error);
        break;

      /* KK and OS - sic transit gloria mundi */
      case J_OS: /* s34j -> os */
        res = s34jtos(N, E, &N, &E, +1, usertxt, tr_error);
        break;
      case OS_J: /* os -> s34j */
        res = s34jtos(N, E, &N, &E, -1, usertxt, tr_error);
        break;
      case S_KK: /* s34s -> kk */
        res = s34stkk(N, E, &N, &E, +1, usertxt, tr_error);
        break;
      case KK_S: /* kk -> s34s */
        res = s34stkk(N, E, &N, &E, -1, usertxt, tr_error);
        break;

      /* STOREBÆLT */
      case U_SB: /* u32 -> sb */
        res = utsb(N, E, &N, &E, +1, usertxt, tr_error);
        break;
      case SB_U: /* sb -> u32 */
        res = utsb(N, E, &N, &E, -1, usertxt, tr_error);
        break;

      /* DKS */
      case SDKS: /* s34s -> dks */
        res = dks_tr("s34s", outcs, N, E, Nout, Eout,
            usertxt, tr_error);
        act   = 0;
        break;
      case DKSS: /* dks -> s34s */
        res = dks_tr(in_cs, "s34s", N, E, &N, &E,
            usertxt, tr_error);
        break;

      /* BORNHOLM */
      case T32B: /* tc32 -> s45b */
        res = tcts_u(N, E, &N, &E, 'b',  1, usertxt, tr_error);
        break;
      case BT32: /* s45b -> tc32 */
        res = tcts_u(N, E, &N, &E, 'b', -1, usertxt, tr_error);
        break;
      case UGSB: /* utm33 -> gsb */
        res = utgs(N, E, &N, &E, 'b',  1, usertxt, tr_error);
        break;
      case GSBU: /* gsb -> utm33 */
        res = utgs(N, E, &N, &E, 'b', -1, usertxt, tr_error);
        break;

      case IDNT: /* ident and terminating transf */
        *Nout = N;
        *Eout = E;
        break;

      case XIDT: /* ident and terminating transf for dks */
        res = dks_tr(in_cs, outcs, N, E, &N, &E,
            usertxt, tr_error);
        *Nout = N;
        *Eout = E;
        act   = 0;
        break;

      case ILLG: /* illegal transformation */
        return((tr_error==NULL) ? TRF_ILLEG_ :
                t_status(tr_error, usertxt,
                "dk_trans(illegal)", TRF_ILLEG_));
      default: /* programme error */
        return((tr_error==NULL) ? TRF_PROGR_ :
                t_status(tr_error, usertxt,
                "dk_trans(prog error)", TRF_PROGR_));
      }
#ifdef  DEBUGDKTRANS
	  lord_debug(0,LORD(", res = %d"), res);
#endif

      if (res < result) result = res;
#ifdef  DEBUGDKTRANS
	  lord_debug(0,LORD(", act = %d;"), act);
#endif
    }    while (act && result >= TRF_TOLLE_);
  }
  return (result);

  /* UNDEFINE ACTION NUMBERS */
#undef  IDNT
#undef  T32J
#undef  JT32
#undef  T32S
#undef  ST32
#undef  T32B
#undef  BT32
#undef  U32G
#undef  GU32
#undef  P_TG
#undef  G_TP
#undef  ZTZU
#undef  ZTZD
#undef  U_GS
#undef  GS_U
#undef  UGSB
#undef  GSBU
#undef  J_OS
#undef  OS_J
#undef  S_KK
#undef  KK_S
#undef  U_SB
#undef  SB_U
#undef  PTGS
#undef  GSTP
#undef  PTBS
#undef  BSTP
#undef  SDKS
#undef  DKSS
#undef  T32U
#undef  U32T
#undef  XIDT
#undef  ILLG
#undef  TGTP
#undef  GTTP
#undef  T32L
#undef  LT32

}
