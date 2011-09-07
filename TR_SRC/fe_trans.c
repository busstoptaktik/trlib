

/* fe_trans  ver 2010.1        # page 1    12 Jan 2010 13 55 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

/*
#define  DEBUGFETRANS
*/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include "geo_lab.h"
#include "trthread.h"

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


/* fe_trans  ver 2010.1        # page 2    12 Jan 2010 13 55 */


#include        "conv_lab.h"
#include        "ptg.h"
#include        "fu50tu_g.h"
#include        "fk54tu_g.h"
#include        "fk89tu_g.h"
#include        "t_status.h"

  static THREAD_SAFE  int               in_chsum = 0L;
  static THREAD_SAFE  int               outchsum = 0L;
  static THREAD_SAFE  int               init = 0;

  char                     in_cs[32], outcs[32];
  char                     p_dtm[32], *pl;

  int                      in_gr, outgr, outnr = 0, fe_z;
  int                      nst, gst, act;
  int                      res = 0, ies = 0;

  struct coord_lab        *in_lab = &(in_lab_u->u_c_lab);
  struct coord_lab        *outlab = &(outlab_u->u_c_lab);

  static THREAD_SAFE  union geo_lab     TC_u29;
  static THREAD_SAFE  union geo_lab     TC_fke;
  static THREAD_SAFE  union geo_lab     TC_u50;
  static THREAD_SAFE  union geo_lab     TC_f54;

  /* minilabels */

  struct nr_mlb {
    short        trgr;
    short        trnr;
    char         *s_lab;
  } *pml;

  static THREAD_SAFE  struct nr_mlb    mlab[] = {
    /*  0 */ { 0,  0,   "utm29"},   /* Datum checked later  */
    /*  1 */ { 0,  1,   "geo"},     /* Datum checked later  */
    /*  2 */ { 1,  2,   "fke"},     /* Region checked later */
    /*  3 */ { 1,  3,   "fu50"},    /* Region checked later */
    /*  4 */ { 1,  4,   "fg50"},    /* Region checked later */
    /*  5 */ { 1,  5,   "fk54"},    /* Region checked later */
    /*  6 */ { 1,  6,   "fg54"},    /* Region checked later */
    /*  7 */ { 1,  7,   "fk89"},    /* Region checked later */
    /*stop*/ {-1, -1,   ""}
  };


/* fe_trans  ver 2010.1        # page 3    12 Jan 2010 13 55 */


  struct act_nst {
    short     action;
    short     nstate;
  };

  /* Start values: *ptab->row, in_nr->col */
  static THREAD_SAFE  struct act_nst     *ptab;
  static THREAD_SAFE  int                 in_nr;
  static THREAD_SAFE  int                 fe_w;

  /* Action/state table */
  static THREAD_SAFE  struct act_nst fetab[] = {

    /* u29 = utm29_euref89: 0 */
    /* input ::                                                   */
    /* u29     geo     fke     u50     g50     k54     g54     k89*/
    /* state : */
    /*   0       1       2       3       4       5       6       7*/
    {IDT,0},{GTU,0},{FTU,0},{U0U,0},{GU0,3},{KTU,0},{G4K,5},{K9U,0},

    /* geo = geo_euref89: 1 */
    /* input ::                                                   */
    /* u29     geo     fke     u50     g50     k54     g54     k89*/
    /*   0       1       2       3       4       5       6       7*/
    {UTG,1},{IDT,1},{FTU,0},{U0U,0},{GU0,3},{KTU,0},{G4K,5},{K9U,0},

    /* fke(_euref89): 2 */
    /* input ::                                                   */
    /* u29     geo     fke     u50     g50     k54     g54     k89*/
    /*   0       1       2       3       4       5       6       7*/
    {UTF,2},{GTU,0},{IDT,2},{U0U,0},{GU0,3},{KTU,0},{G4K,5},{K9U,0},

    /* u50 = utm29_ed50: 3 */
    /* input ::                                                   */
    /* u29     geo     fke     u50     g50     k54     g54     k89*/
    /*   0       1       2       3       4       5       6       7*/
    {UU0,3},{GTU,0},{FTU,0},{IDT,3},{GU0,3},{KTU,0},{G4K,5},{K9U,0},

    /* g50 = geo_ed50:  4 */
    /* input ::                                                   */
    /* u29     geo     fke     u50     g50     k54     g54     k89*/
    /*   0       1       2       3       4       5       6       7*/
    {UU0,3},{GTU,0},{FTU,0},{U0G,4},{IDT,4},{KTU,0},{G4K,5},{K9U,0},

    /* k54 = fk54(_fd54): 5 */
    /* input ::                                                   */
    /* u29     geo     fke     u50     g50     k54     g54     k89*/
    /*   0       1       2       3       4       5       6       7*/
    {UTK,5},{GTU,0},{FTU,0},{U0U,0},{GU0,3},{IDT,5},{G4K,5},{K9U,0},

    /* g54 = geo_fd54: 6 */
    /* input ::                                                   */
    /* u29     geo     fke     u50     g50     k54     g54     k89*/
    /*   0       1       2       3       4       5       6       7*/
    {UTK,5},{GTU,0},{FTU,0},{U0U,0},{GU0,3},{KG4,6},{IDT,6},{K9U,0},

    /* k89 = fk89(_fd54a): 7 */
    /* input ::                                                   */
    /* u29     geo     fke     u50     g50     k54     g54     k89*/
    /*   0       1       2       3       4       5       6       7*/
    {U9K,7},{GTU,0},{FTU,0},{U0U,0},{GU0,3},{KTU,0},{G4K,5},{IDT,7}
  };


/* fe_trans  ver 2010.1        # page 4    12 Jan 2010 13 55 */


  /* Test io-labels */
  if (in_lab->lab_type == CRD_LAB && outlab->lab_type == CRD_LAB) {

    if (init == 0) {
      /* Internal wrk-labels */
      init = (conv_lab("utm29_euref89", &TC_u29, "") == CRD_LAB
          &&  conv_lab("fke",           &TC_fke, "") == CRD_LAB
          &&  conv_lab("fu50",          &TC_u50, "") == CRD_LAB
          &&  conv_lab("fk54",          &TC_f54, "") == CRD_LAB);
      /* State/action table size and width */
      fe_z    = sizeof(fetab)/sizeof(struct act_nst);
      fe_w    = (int) sqrt(1.0000001*fe_z);
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
pml->trgr, pml->trnr, pml->s_lab);
#endif
          break;
        }


/* fe_trans  ver 2010.1        # page 5    12 Jan 2010 13 55 */


      /* Datum and region check */
      switch (outgr) {
      case 0: /* Datum check */
        if (strcmp(p_dtm, "euref89")) res = TRF_ILLEG_;
        break;

      case 1: /* Region check */
        if (outlab->p_rgn != TC_fke.u_c_lab.p_rgn)
           res = TRF_ILLEG_;
        break;

      default: /* Unknown label */
        res = TRF_ILLEG_;
        break;
      } /* end out-label check */

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
pml->trgr, pml->trnr, pml->s_lab);
#endif
          break;
        }

      /* Datum and region check */
      switch (in_gr) {
      case 0: /* Datum check */
        if (strcmp(p_dtm, "euref89")) res = TRF_ILLEG_;
        break;

      case 1: /* Region check */
        if (in_lab->p_rgn != TC_fke.u_c_lab.p_rgn)
            res = TRF_ILLEG_;
        break;

      default: /* Unknown label */
        res = TRF_ILLEG_;
        break;
      } /* end in_-label check */


/* fe_trans  ver 2010.1        # page 6    12 Jan 2010 13 55 */


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

      case GTU: /* geo_euref -> utm29_euref */
        ies = ptg(&TC_u29, -1, N, E, &N, &E, usertxt, tr_error);
        break;
      case UTG: /* utm29_euref89 -> geo_euref89 */
        ies = ptg(&TC_u29, +1, N, E, &N, &E,  usertxt, tr_error);
        break;


/* fe_trans  ver 2010.1        # page 7    12 Jan 2010 13 55 */


      case FTU: /* FO national system -> utm29_euref89 */
        ies = ptg(&TC_fke, +2, N, E, &N, &E, usertxt, tr_error);
        if (ies < res) res = ies;
        ies = ptg(&TC_u29, -2, N, E, &N, &E, usertxt, tr_error);
        break;

      case UTF: /* utm29_euref89 -> FO national system */
        ies = ptg(&TC_u29, +2, N, E, &N, &E, usertxt, tr_error);
        if (ies < res) res = ies;
        ies = ptg(&TC_fke, -2, N, E, &N, &E, usertxt, tr_error);
        break;

      case UU0: /* utm29_euref89 -> utm29_ed50 */
        ies = fu50tu_g(-1, N, E, &N, &E, usertxt, tr_error);
        break;

      case U0U: /* utm29_ed50 -> utm29_euref89 */
        ies = fu50tu_g(+1, N, E, &N, &E, usertxt, tr_error);
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

      case UTK: /* utm29_euref89 -> fk54 */
        ies = fk54tu_g(-1, N, E, &N, &E, usertxt, tr_error);
        break;

      case KTU: /* fk54 -> utm29_euref89 */
        ies = fk54tu_g(+1, N, E, &N, &E, usertxt, tr_error);
        break;

      case U9K: /* utm29_euref89 -> fk89 */
        ies = fk89tu_g(-1, N, E, &N, &E, usertxt, tr_error);
        break;

      case K9U: /* fk89 -> utm29_euref89 */
        ies = fk89tu_g(+1, N, E, &N, &E, usertxt, tr_error);
        break;

      case IDT: /* ident, no action */
        break;



/* fe_trans  ver 2010.1        # page 8    12 Jan 2010 13 55 */

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


