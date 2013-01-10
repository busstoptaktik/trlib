/*
 * Copyright (c) 2012, National Survey and Cadastre, Denmark
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
 


/* fe_trans  ver 2012.03        # page 1   18 dec 2012 13 55 */


/* Copyright (c) 2011 SPACE Danish Technical University (DTU)  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of DTU SPACE    */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */

/*
#define  DEBUGFETRANS
*/

#include    <stdio.h>
#include    <math.h>
#include    <string.h>
#include    <ctype.h>
#include    "KmsFncs.h"
#include    "trthread.h"

/* ACTION NUMBERS */

#define  IDT    0
#define  GTU    1
#define  UTG    2
#define  FTU    3
#define  UTF    4
#define  KTU    5
#define  UTK    6
#define  KG4    7
#define  G4K    8
#define  UU0    9
#define  U0U   10
#define  U0G   11
#define  GU0   12
#define  K9U   13
#define  U9K   14

int                    fe_trans(
/*___________________________*/
union geo_lab       *in_lab_u,
union geo_lab       *outlab_u,
double              N,
double              E,
double              H,
double              *Nout,
double              *Eout,
double              *Hout,
char                *usertxt,
FILE                *tr_error
)

{


/* fe_trans  ver 2012.03        # page 2   18 dec 2012 13 55 */


  static THREAD_SAFE int   in_chsum = 0L;
  static THREAD_SAFE int   outchsum = 0L;
  static int   init = 0;

  char                     in_cs[32], outcs[32];
  char                     p_dtm[32], *pl;

  int                      in_gr, outgr, outnr = 0, fe_z;
  int                      nst, gst, act;
  int                      res = 0, ies = 0;

  struct coord_lab        *in_lab = &(in_lab_u->u_c_lab);
  struct coord_lab        *outlab = &(outlab_u->u_c_lab);

  static union geo_lab     TC_u29;
  static union geo_lab     TC_fke;
  static union geo_lab     TC_u50;
  static union geo_lab     TC_f54;

  /* minilabels */

  struct nr_mlb {
    short        trgr;
    short        trnr;
    char         *s_lab;
  } *pml;

  static struct nr_mlb    mlab[] = {
    /*  0 */ { 0,  0,   "utm29"}, /* Datum checked later */
    /*  1 */ { 0,  1,   "geo"},   /* Datum checked later */
    /*  2 */ { 1,  2,   "fke"},   /* Region is FO */
    /*  3 */ { 1,  3,   "fu50"},  /* Region is FO */
    /*  4 */ { 1,  4,   "fg50"},  /* Region is FO */
    /*  5 */ { 1,  5,   "fk54"},  /* Region is FO */
    /*  6 */ { 1,  6,   "fg54"},  /* Region is FO */
    /*  7 */ { 1,  7,   "fk89"},  /* Region is FO */
    /*stop*/ {-1, -1,   ""}
  };


/* fe_trans  ver 2012.03        # page 3   18 dec 2012 13 55 */


  struct act_nst {
    short     action;
    short     nstate;
  };

  /* Start values: *ptab->row, in_nr->col */
  static THREAD_SAFE struct act_nst  *ptab;
  static THREAD_SAFE int              in_nr;
  static int                          fe_w;

  /* Action/state table */
  static struct act_nst fetab[] = {

    /* output: u29 = utm29_etrs89: 0 */
    /* input ::                                                   */
    /* u29     geo     fke     u50     g50     k54     g54     k89*/
    /* state : */
    /*   0       1       2       3       4       5       6       7*/
    {IDT,0},{GTU,0},{FTU,0},{U0U,0},{GU0,3},{KTU,0},{G4K,5},{K9U,0},
    /* read above line::                                          */
    /* input :: 0 :: IDT                                          */
    /* input :: 1 :: GTU -> IDT                                   */
    /* input :: 2 :: FTU -> IDT                                   */
    /* input :: 3 :: U0U -> IDT                                   */
    /* input :: 4 :: GU0 -> U0U -> IDT                            */
    /* input :: 5 :: KTU -> IDT                                   */
    /* input :: 6 :: G4K -> KTU -> IDT                            */
    /* input :: 7 :: K9U -> IDT                                   */

    /* output: geo = geo_etrs89: 1 */
    /* input ::                                                   */
    /* u29     geo     fke     u50     g50     k54     g54     k89*/
    /*   0       1       2       3       4       5       6       7*/
    {UTG,1},{IDT,1},{FTU,0},{U0U,0},{GU0,3},{KTU,0},{G4K,5},{K9U,0},
    /* read above line::                                          */
    /* input :: 0 :: UTG -> IDT                                   */
    /* input :: 1 :: IDT                                          */
    /* input :: 2 :: FTU -> UTG -> IDT                            */
    /* input :: 3 :: U0U -> UTG -> IDT                            */
    /* input :: 4 :: GU0 -> U0U -> UTG -> IDT                     */
    /* input :: 5 :: KTU -> UTG -> IDT                            */
    /* input :: 6 :: G4K -> KTU -> UTG -> IDT                     */
    /* input :: 7 :: K9U -> UTG -> IDT                            */

    /* output: fke(_etrs89): 2 */
    /* input ::                                                   */
    /* u29     geo     fke     u50     g50     k54     g54     k89*/
    /*   0       1       2       3       4       5       6       7*/
    {UTF,2},{GTU,0},{IDT,2},{U0U,0},{GU0,3},{KTU,0},{G4K,5},{K9U,0},
    /* read above line::                                          */
    /* input :: 0 :: UTF -> IDT                                   */
    /* input :: 1 :: GTU -> UTF -> IDT                            */
    /* input :: 2 :: IDT                                          */
    /* input :: 3 :: U0U -> UTF -> IDT                            */
    /* input :: 4 :: GU0 -> U0U -> UTF -> IDT                     */
    /* input :: 5 :: KTU -> UTF -> IDT                            */
    /* input :: 6 :: G4K -> KTU -> UTF -> IDT                     */
    /* input :: 7 :: K9U -> UTF -> IDT                            */

    /* output: u50 = utm29_ed50: 3 */
    /* input ::                                                   */
    /* u29     geo     fke     u50     g50     k54     g54     k89*/
    /*   0       1       2       3       4       5       6       7*/
    {UU0,3},{GTU,0},{FTU,0},{IDT,3},{GU0,3},{KTU,0},{G4K,5},{K9U,0},
    /* read above line::                                          */
    /* input :: 0 :: UU0 -> IDT                                   */
    /* input :: 1 :: GTU -> UU0 -> IDT                            */
    /* input :: 2 :: FTU -> UU0 -> IDT                            */
    /* input :: 3 :: IDT                                          */
    /* input :: 4 :: GU0 -> IDT                                   */
    /* input :: 5 :: KTU -> UU0 -> IDT                            */
    /* input :: 6 :: G4K -> KTU -> UU0 -> IDT                     */
    /* input :: 7 :: K9U -> UU0 -> IDT                            */

    /* output: g50 = geo_ed50:  4 */
    /* input ::                                                   */
    /* u29     geo     fke     u50     g50     k54     g54     k89*/
    /*   0       1       2       3       4       5       6       7*/
    {UU0,3},{GTU,0},{FTU,0},{U0G,4},{IDT,4},{KTU,0},{G4K,5},{K9U,0},
    /* read above line::                                          */
    /* input :: 0 :: UU0 -> U0G -> IDT                            */
    /* input :: 1 :: GTU -> UU0 -> U0G -> IDT                     */
    /* input :: 2 :: FTU -> UU0 -> U0G -> IDT                     */
    /* input :: 3 :: U0G -> IDT                                   */
    /* input :: 4 :: IDT                                          */
    /* input :: 5 :: KTU -> UU0 -> U0G -> IDT                     */
    /* input :: 6 :: G4K -> KTU -> UU0 -> U0G -> IDT              */
    /* input :: 7 :: K9U -> UU0 -> U0G -> IDT                     */

    /* output: k54 = fk54(_fd54): 5 */
    /* input ::                                                   */
    /* u29     geo     fke     u50     g50     k54     g54     k89*/
    /*   0       1       2       3       4       5       6       7*/
    {UTK,5},{GTU,0},{FTU,0},{U0U,0},{GU0,3},{IDT,5},{G4K,5},{K9U,0},
    /* read above line::                                          */
    /* input :: 0 :: UTK -> IDT                                   */
    /* input :: 1 :: GTU -> UTK -> IDT                            */
    /* input :: 2 :: FTU -> UTK -> IDT                            */
    /* input :: 3 :: U0U -> UTK -> IDT                            */
    /* input :: 4 :: GU0 -> UU0 -> UTK -> IDT                     */
    /* input :: 5 :: IDT                                          */
    /* input :: 6 :: G4K -> IDT                                   */
    /* input :: 7 :: K9U -> UTK -> IDT                            */

    /* output: g54 = geo_fd54: 6 */
    /* input ::                                                   */
    /* u29     geo     fke     u50     g50     k54     g54     k89*/
    /*   0       1       2       3       4       5       6       7*/
    {UTK,5},{GTU,0},{FTU,0},{U0U,0},{GU0,3},{KG4,6},{IDT,6},{K9U,0},
    /* read above line::                                          */
    /* input :: 0 :: UTK -> KG4 -> IDT                            */
    /* input :: 1 :: GTU -> UTK -> KG4 -> IDT                     */
    /* input :: 2 :: FTU -> UTK -> KG4 -> IDT                     */
    /* input :: 3 :: U0U -> UTK -> KG4 -> IDT                     */
    /* input :: 4 :: GU0 -> UU0 -> UTK -> IDT                     */
    /* input :: 5 :: G4K -> IDT                                   */
    /* input :: 6 :: IDT                                          */
    /* input :: 7 :: K9U -> UTK -> KG4 -> IDT                     */

    /* output: k89 = fk89(_fd54a): 7 */
    /* input ::                                                   */
    /* u29     geo     fke     u50     g50     k54     g54     k89*/
    /*   0       1       2       3       4       5       6       7*/
    {U9K,7},{GTU,0},{FTU,0},{U0U,0},{GU0,3},{KTU,0},{G4K,5},{IDT,7}
    /* read above line::                                          */
    /* input :: 0 :: U9K -> IDT                                   */
    /* input :: 1 :: GTU -> U9K -> IDT                            */
    /* input :: 2 :: FTU -> U9K -> IDT                            */
    /* input :: 3 :: U0U -> U9K -> IDT                            */
    /* input :: 4 :: GU0 -> UU0 -> U9K -> IDT                     */
    /* input :: 5 :: KTU -> U9K -> IDT                            */
    /* input :: 6 :: G4K -> KTU -> U9K -> IDT                     */
    /* input :: 7 :: IDT                                          */
  };


/* fe_trans  ver 2012.03        # page 4   18 dec 2012 13 55 */


  /* Test io-labels */
  if (in_lab->lab_type == CRD_LAB && outlab->lab_type == CRD_LAB) {

    if (init == 0) {
      /* Internal wrk-labels */
      init = (conv_lab("utm29_etrs89", &TC_u29, "") == CRD_LAB
          &&  conv_lab("fke",          &TC_fke, "") == CRD_LAB
          &&  conv_lab("fu50",         &TC_u50, "") == CRD_LAB
          &&  conv_lab("fk54",         &TC_f54, "") == CRD_LAB);
      /* State/action table size and width */
      fe_z = sizeof(fetab)/sizeof(struct act_nst);
      fe_w = (int) sqrt(1.0000001*fe_z);
      init = init && (fe_z == fe_w*fe_w);
      if (!init)
        return(t_status(
               tr_error, "", "fe_trans(internal labels)", TRF_ILLEG_));
    }

    /* Check i/o labels, init of actual transf. systems */
    if (in_chsum != in_lab->ch_sum || outchsum != outlab->ch_sum) {

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
#ifdef DEBUGFETRANS
(void) printf("\n*OUT  grp = %4d    nr = %4d   navn = %s;",
outgr, outnr, pml->s_lab);
#endif
          break;
        }


/* fe_trans  ver 2012.03        # page 5   18 dec 2012 13 55 */


      /* Datum and region check */
      switch (outgr) {
      case 0: /* Datum check */
        if (outlab->datum != TC_u29.u_c_lab.datum) {
          switch(outlab->datum) {
          case  13: /* fd54 */
          case 142: /* fg54 */
          case 144: /* fk54 */
            outnr = 6;
            break;
          case 143: /* fg50 */
          case 145: /* fu50 */
            outnr = 4;
            break;
          case 146: /* fk89 */
            outnr = 4;
            break;
          default:
            res = TRF_ILLEG_;
            break;
          }
          outgr = 1;
        }
        break;

      case 1: /* Region check */
        /* NOT needed: set in conv_lab 
        if (outlab->p_rgn != TC_fke.p_rgn)
           res = TRF_ILLEG_; */
        break;

      default: /* Unknown label */
        res = TRF_ILLEG_;
        break;
      } /* end out-label check */
#ifdef DEBUGFETRANS
(void) printf("\n*OUT  grp = %4d    nr = %4d;", pml->trgr, pml->trnr);
#endif

      if (res == TRF_ILLEG_)
        return(t_status(
               tr_error, "", "fe_trans(outsys - lab)", TRF_ILLEG_));

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
#ifdef DEBUGFETRANS
(void) printf("\n*IN   grp = %4d    nr = %4d   navn = %s;",
in_gr, in_nr, pml->s_lab);
#endif
          break;
        }

      /* Datum and region check */
      switch (in_gr) {
      case 0: /* Datum check */
        if (in_lab->datum != TC_u29.u_c_lab.datum) {
          switch(in_lab->datum) {
          case  13: /* fd54 */
          case 142: /* fg54 */
          case 144: /* fk54 */
            in_nr = 6;
            break;
          case 143: /* fg50 */
          case 145: /* fu50 */
            in_nr = 4;
            break;
          case 146: /* fk89 */
            in_nr = 4;
            break;
          default:
            res = TRF_ILLEG_;
            break;
          }
          in_gr = 1;
        }
        break;

      case 1: /* Region check */
        /* NOT needed: set in conv_lab 
        if (in_lab->p_rgn != TC_fke.p_rgn)
           res = TRF_ILLEG_; */
        break;

      default: /* Unknown label */
        res = TRF_ILLEG_;
        break;
      } /* end in_-label check */
#ifdef DEBUGFETRANS
(void) printf("\n*IN   grp = %4d    nr = %4d;", in_gr, in_nr);
#endif


/* fe_trans  ver 2012.03        # page 6   18 dec 2012 13 55 */


      if (res == TRF_ILLEG_)
        return(t_status(
               tr_error, "", "fe_trans(in_sys)", TRF_ILLEG_));

      /* Save check-sums */
      in_chsum = in_lab->ch_sum;
      outchsum = outlab->ch_sum;

      /* Test identical labels */
      if (in_chsum == outchsum) in_nr = outnr = 0;

#ifdef  DEBUGFETRANS
(void) fprintf(stdout, "\n* in = %s, out = %s;\n", in_cs, outcs);
(void) fprintf(stdout, "\n*fe_trans in_nr     %d  outnr      %d;",
in_nr, outnr);
#endif

      ptab = fetab + fe_w*outnr;  /* output row */
    } /* End of init actions */

    /* transformation module */
    *Hout = H;
    nst   = in_nr;               /* input col  */
    do {
      gst = nst;
      act = (ptab+nst)->action;
      nst = (ptab+nst)->nstate;

#ifdef  DEBUGFETRANS
(void) fprintf(stdout, "\n*FE_TRANS ACTION  %d:   %d -> %d;",
act, gst, nst);
#endif

      switch(act) {

      case GTU: /* geo_etrs89 -> utm29_etrs89 */
        ies = ptg(&TC_u29, -1, N, E, &N, &E, usertxt, tr_error);
        break;
      case UTG: /* utm29_etrs89 -> geo_etrs89 */
        ies = ptg(&TC_u29, +1, N, E, &N, &E,  usertxt, tr_error);
        break;


/* fe_trans  ver 2012.03        # page 7   18 dec 2012 13 55 */


      case FTU: /* FO national system -> utm29_etrs89 */
        ies = ptg(&TC_fke, +2, N, E, &N, &E, usertxt, tr_error);
        if (ies < res) res = ies;
        ies = ptg(&TC_u29, -2, N, E, &N, &E, usertxt, tr_error);
        break;

      case UTF: /* utm29_etrs89 -> FO national system */
        ies = ptg(&TC_u29, +2, N, E, &N, &E, usertxt, tr_error);
        if (ies < res) res = ies;
        ies = ptg(&TC_fke, -2, N, E, &N, &E, usertxt, tr_error);
        break;

      case UU0: /* utm29_etrs89 -> utm29_ed50 */
        ies = fo_g_tr(3, +1, N, E, &N, &E, usertxt, tr_error);
        break;

      case U0U: /* utm29_ed50 -> utm29_etrs89 */
        ies = fo_g_tr(3, -1, N, E, &N, &E, usertxt, tr_error);
        break;

      case G4K: /* fgeo(_fd54) -> fk(_fd54) */
        ies = ptg(&TC_f54, -1, N, E, &N, &E, usertxt, tr_error);
        break;

      case KG4: /* fk(_fd54) -> fgeo(_fd54) */
        ies = ptg(&TC_f54, +1, N, E, &N, &E, usertxt, tr_error);
        break;

      case U0G: /* utm29_ed50 -> geo_ed50 */
        ies = ptg(&TC_u50, +1, N, E, &N, &E, usertxt, tr_error);
        break;

      case GU0: /* geo_ed50 -> utm29_ed50 */
        ies = ptg(&TC_u50, -1, N, E, &N, &E, usertxt, tr_error);
        break;

      case UTK: /* utm29_etrs89 -> fk54 */
        ies = fo_g_tr(5, +1, N, E, &N, &E, usertxt, tr_error);
        break;

      case KTU: /* fk54 -> utm29_etrs89 */
        ies = fo_g_tr(5, -1, N, E, &N, &E, usertxt, tr_error);
        break;

      case U9K: /* utm29_etrs89 -> fk89 */
        ies = fo_g_tr(7, +1, N, E, &N, &E, usertxt, tr_error);
        break;

      case K9U: /* fk89 -> utm29_etrs89 */
        ies = fo_g_tr(7, -1, N, E, &N, &E, usertxt, tr_error);
        break;

      case IDT: /* ident, no action */
        break;



/* fe_trans  ver 2012.03        # page 8   18 dec 2012 13 55 */

      default: /* programme error */
        return(t_status(
               tr_error, "", "fe_trans(prog error)", TRF_ILLEG_));
      } /* end switch(action) */
      if (ies < res) res = ies;

    } while (nst != gst && res >= TRF_TOLLE_);
  }
  else {

    return(t_status(
           tr_error, "", "fe_trans(i/o labels)", TRF_ILLEG_));
  }

  /* Return coord and result */
  *Nout = N;
  *Eout = E;
  return (res);

  /* UNDEFINE ACTION NUMBERS */
#undef  IDT
#undef  GTU
#undef  UTG
#undef  FTU
#undef  UTF
#undef  KTU
#undef  UTK
#undef  KG4
#undef  G4K
#undef  UU0
#undef  U0U
#undef  U0G
#undef  GU0
#undef  K9U
#undef  U9K

}
