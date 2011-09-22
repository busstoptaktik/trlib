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
 


/* fh_trans  ver 2010.1        # page 1    12 Jan 2010 13 55 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */


#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include "geo_lab.h"
#include "trthread.h"

/* ACTION NUMBERS */

#define  IDT    0
#define  PTG    1
#define  GTP    2
#define  CTG    3
#define  GTC    4
#define  TSC    5
#define  TSP    6

int                    fh_trans(
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

  /* Fehmarn Bridge Projection test of AREA */
  /* and transf to/from geo                 */

#include        "gtc.h"
#include        "ptg.h"
#include        "sgetg.h"
#include        "typ_dec.h"
#include        "t_status.h"


/* fh_trans  ver 2010.1        # page 2    12 Jan 2010 13 55 */


  static THREAD_SAFE  int               in_chsum = 0L;
  static THREAD_SAFE  int               outchsum = 0L;
  static THREAD_SAFE  int               init = 0;
  static THREAD_SAFE  double            lim[4], date = -10.0;

  int                      outnr = 0, fh_z, RES = 0, IES = 0;
  int                      act, nst, res = 0, ies = 0, rrs = 0;
  struct typ_dec           tpd;
  struct coord_lab        *in_lab = &(in_lab_u->u_c_lab);
  struct coord_lab        *outlab = &(outlab_u->u_c_lab);

  struct act_nst {
    short     action;
    short     nstate;
  };

  /* Start values: *ptab->row, in_nr->col */
  static THREAD_SAFE  struct act_nst     *ptab;
  static THREAD_SAFE  int                 in_nr;
  static THREAD_SAFE  int                 fh_w;

  /* Action/state table */
  static THREAD_SAFE  struct act_nst fhtab[] = {
    /*         input ::   crt     geo     prj*/
    /*         state ::    0       1       2 */
    /* OUTPUT::     */
    /* crt_feh10: 0 */ {TSC,0},{GTC,0},{GTP,1},
    /* geo_feh10: 1 */ {CTG,0},{IDT,0},{PTG,0},
    /* prj_feh10: 2 */ {CTG,1},{GTP,0},{TSP,0}
  };

  /* Test io-labels */
  if (in_lab->lab_type == CRD_LAB && outlab->lab_type == CRD_LAB) {

    if (init == 0) {
      /* State/action table size and width */
      fh_z = sizeof(fhtab)/sizeof(struct act_nst);
      fh_w = (int) sqrt(1.0000001*fh_z);
      init = fh_z == fh_w*fh_w;
      if (!init)
        return(t_status(
               tr_error, "", "fh_trans(internal labels)", TRF_ILLEG_));
    }


/* fh_trans  ver 2010.1        # page 2    12 Jan 2010 13 55 */


    /* Check i/o labels, init of actual transf. systems */
    if (in_chsum != in_lab->ch_sum || date != in_lab->date ||
        outchsum != outlab->ch_sum) {

      /* Out-system */
      /*____________*/
      outnr = outlab->cstm -1;
      if (outnr > 2) outnr = 2;

      if (outlab->datum != 32)
        return(t_status(
               tr_error, "", "fh_trans(outsys)", TRF_ILLEG_));

      /* In-system */
      /*___________*/
      in_nr = in_lab->cstm -1;
      if (in_nr > 2) in_nr = 2;
#ifdef DEBUGFETRANS
(void) printf("\n*OUT  nr = %4d   navn = %s;", outnr, outlab->mlb);
(void) printf("\n*IN   nr = %4d   navn = %s;", in_nr, in_lab->mlb);
#endif

      if (in_lab->datum != 32)
        return(t_status(
               tr_error, "", "fh_trans(in_sys)", TRF_ILLEG_));

      if (in_lab->date < 50.0) {
        lim[0] = sgetg("5422nt", &tpd, &nst, "nt");
        lim[1] = sgetg("5433nt", &tpd, &nst, "nt");
        lim[2] = sgetg("1055nt", &tpd, &nst, "nt");
        lim[3] = sgetg("1125nt", &tpd, &nst, "nt");
      } else {
        lim[0] = sgetg("5420nt", &tpd, &nst, "nt");
        lim[1] = sgetg("5450nt", &tpd, &nst, "nt");
        lim[2] = sgetg("1040nt", &tpd, &nst, "nt");
        lim[3] = sgetg("1200nt", &tpd, &nst, "nt");
      }

      /* Save check-sums */
      in_chsum = in_lab->ch_sum;
      outchsum = outlab->ch_sum;
      date     = in_lab->date;

      ptab = fhtab + fh_w*outnr;  /* output row */
    } /* End of init actions */


/* fh_trans  ver 2010.1        # page 3    12 Jan 2010 13 55 */


    /* transformation module */
    *Hout = H;
    nst   = in_nr;
    do {
      act = (ptab + nst)->action;
      nst = (ptab + nst)->nstate;
      switch(act) {
      case GTP: /* geo -> prj */
        if (N < lim[0] || lim[1] < N || E < lim[2] || lim[3] < E)
           ies = t_status(tr_error, usertxt, "fh_trans", TRF_AREA_, "sx", "", N, E); 
        res = ptg(outlab_u, -1, N, E, &N, &E, usertxt, tr_error);
        *Nout = N;
        *Eout = E;
        break;
      case PTG: /* prj -> geo */
        res = ptg(in_lab_u, +1, N, E, &N, &E,  usertxt, tr_error);
        if (N < lim[0] || lim[1] < N || E < lim[2] || lim[3] < E)
           ies = t_status(tr_error, usertxt, "fh_trans", TRF_AREA_, "sx", "", N, E); 
        *Nout = N;
        *Eout = E;
        break;
      case GTC: /* geo -> crt */
        if (N < lim[0] || lim[1] < N || E < lim[2] || lim[3] < E)
           ies = t_status(tr_error, usertxt, "fh_trans", TRF_AREA_, "sx", "", N, E); 
        res   = gtc(in_lab_u, +1, N, E, H, &N, &E, &H, usertxt, tr_error);
        *Nout = N;
        *Eout = E;
        *Hout = H;
        break;
      case CTG: /* crt -> geo */
        res = gtc(outlab_u, -1, N, E, H, &N, &E, &H, usertxt, tr_error);
        if (N < lim[0] || lim[1] < N || E < lim[2] || lim[3] < E)
            ies = t_status(tr_error, usertxt, "fh_trans", TRF_AREA_, "sx", "", N, E); 
        *Nout = N;
        *Eout = E;
        *Hout = H;
        break;
      case TSP: /* prj -> prj */
        *Nout = N;
        *Eout = E;
        res = ptg(in_lab_u, +1, N, E, &N, &E,  usertxt, tr_error);
        if (N < lim[0] || lim[1] < N || E < lim[2] || lim[3] < E)
           ies = t_status(tr_error, usertxt, "fh_trans", TRF_AREA_, "sx", "", N, E); 
        if (in_lab_u->u_c_lab.cstm != outlab_u->u_c_lab.cstm) {
          rrs   = ptg(outlab_u, -1, N, E, &N, &E,  usertxt, tr_error); 
          if (rrs < res) res = rrs;
          *Nout = N;
          *Eout = E;
        }
        break;
      case IDT: /* ident, geo -> geo */
        if (N < lim[0] || lim[1] < N || E < lim[2] || lim[3] < E)
            ies = t_status(tr_error, usertxt, "fh_trans", TRF_AREA_, "sx", "", N, E); 
        *Nout = N;
        *Eout = E;
        break;
      default: /* programme error */
        return(t_status(
               tr_error, "", "fh_trans(prog error)", TRF_ILLEG_));
      } /* end switch(action) */

      if (res < RES) RES = res;
      if (ies < IES) IES = ies;
    } while (nst && RES >= TRF_TOLLE_);
  } else
    return(t_status(
           tr_error, "", "fh_trans(i/o labels)", TRF_ILLEG_));
  /* Return coord and result */
  if (IES < RES) RES = IES;
  return (RES);

/* UNDEFINE ACTION NUMBERS */
#undef  IDT
#undef  PTG
#undef  GTP
#undef  TST
}
