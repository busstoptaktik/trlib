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

/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */


#include    <stdio.h>
#include    <math.h>
#include    <string.h>
#include    <ctype.h>
#include    "KmsFncs.h"
#include    "trthread.h"

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
struct coord_lab       *in_lab,
struct coord_lab       *outlab,
double              N,
double              E,
double              H,
double              *Nout,
double              *Eout,
double              *Hout,
tab_dir               *tdir
)
{

  /* Fehmarn Bridge Projection test of AREA */
  /* and transf to/from geo                 */
  
  static const double c_lim[] = {0.948877337, 0.952077107, 0.190531777, 0.199258423, 0.948295560, 0.957022207, 0.186168454, 0.209439510};
/*
  Germany
  lim[0] = sgetg("5422nt", &tpd, &nst, "nt");
  lim[1] = sgetg("5433nt", &tpd, &nst, "nt");
  lim[2] = sgetg("1055nt", &tpd, &nst, "nt");
  lim[3] = sgetg("1125nt", &tpd, &nst, "nt");
Fehmarn:
          lim[0] = sgetg("5420nt", &tpd, &nst, "nt");
        lim[1] = sgetg("5450nt", &tpd, &nst, "nt");
        lim[2] = sgetg("1040nt", &tpd, &nst, "nt");
        lim[3] = sgetg("1200nt", &tpd, &nst, "nt");
*/ 
  
  int                      outnr = 0, fh_z, RES = 0, IES = 0;
  int                      act, nst, res = 0, ies = 0, rrs = 0;
  int 				init;
  const double 				*lim;

  struct act_nst {
    short     action;
    short     nstate;
  };

  /* Start values: *ptab->row, in_nr->col */
  struct act_nst *ptab;
  int             in_nr;
  int                          fh_w;

  /* Action/state table */
  static struct act_nst fhtab[] = {
    /*         input ::   crt     geo     prj*/
    /*         state ::    0       1       2 */
    /* OUTPUT::     */
    /* crt_feh10: 0 */ {TSC,0},{GTC,0},{GTP,1},
    /* geo_feh10: 1 */ {CTG,0},{IDT,0},{PTG,0},
    /* prj_feh10: 2 */ {CTG,1},{GTP,0},{TSP,0}
  };

  
  /* Test io-labels */
  if (in_lab->lab_type == CRD_LAB && outlab->lab_type == CRD_LAB) {

      /* State/action table size and width */
      fh_z = sizeof(fhtab)/sizeof(struct act_nst);
      fh_w = (int) sqrt(1.0000001*fh_z);
      init = fh_z == fh_w*fh_w;
      
	if (!init){
		lord_error(TRF_PROGR_ ,LORD("Program error,  internal labels"));
		return TRF_PROGR_ ;
	}

      /* Out-system */
      /*____________*/
      outnr = outlab->cstm -1;
      if (outnr > 2) outnr = 2;

      if (outlab->datum != 32){
	lord_error(TRF_ILLEG_,LORD("Illegal outsys"));
	return TRF_ILLEG_;
      }

      /* In-system */
      /*___________*/
      in_nr = in_lab->cstm -1;
      if (in_nr > 2) in_nr = 2;
#ifdef DEBUGFETRANS
(void) printf("\n*OUT  nr = %4d   navn = %s;", outnr, outlab->mlb);
(void) printf("\n*IN   nr = %4d   navn = %s;", in_nr, in_lab->mlb);
#endif

      if (in_lab->datum != 32){
	lord_error(TRF_ILLEG_, LORD("Illegal insys"));
        return TRF_ILLEG_;
      }
      
     /*Dette er smukt*/
      lim = &c_lim[(in_lab->imit == FHMASK || outlab->imit == FHMASK) ? 4:0];

/*     lord_debug(0,LORD("fh_limit init area %9.5lf %9.5lf %9.5lf %9.5lf"),lim[0]*DOUT,lim[1] *DOUT,lim[2]*DOUT,lim[3]*DOUT);
*/
      ptab = fhtab + fh_w*outnr;  /* output row */
    /* End of init actions */

    /* transformation module */
    *Hout = H;
    nst   = in_nr;
    do {
      act = (ptab + nst)->action;
      nst = (ptab + nst)->nstate;
      switch(act) {
      case GTP: /* geo -> prj */
        if (N < lim[0] || lim[1] < N || E < lim[2] || lim[3] < E){
           ies = TRF_AREA_;
           lord_error(TRF_AREA_,LORD("Out of area lat: %9.5lf long: %9.5lf"), N*DOUT, E*DOUT); 
	}
        res = ptg(outlab, -1, N, E, &N, &E, "", NULL);
        *Nout = N;
        *Eout = E;
        break;
      case PTG: /* prj -> geo */
        res = ptg(in_lab, +1, N, E, &N, &E,  "", NULL);
        if (N < lim[0] || lim[1] < N || E < lim[2] || lim[3] < E){
           ies = TRF_AREA_;
           lord_error(TRF_AREA_,LORD("Out of area lat: %9.5lf long: %9.5lf"), N*DOUT, E*DOUT); 
	}
        *Nout = N;
        *Eout = E;
        break;
      case GTC: /* geo -> crt */
        if (N < lim[0] || lim[1] < N || E < lim[2] || lim[3] < E){
           ies = TRF_AREA_;
           lord_error(TRF_AREA_,LORD("Out of area lat: %9.5lf long: %9.5lf"), N*DOUT, E*DOUT); 
	}
        res   = gtc(in_lab, +1, N, E, H, &N, &E, &H, "", NULL);
        *Nout = N;
        *Eout = E;
        *Hout = H;
        break;
      case CTG: /* crt -> geo */
        res = gtc(outlab, -1, N, E, H, &N, &E, &H, "", NULL);
        if (N < lim[0] || lim[1] < N || E < lim[2] || lim[3] < E){
           ies = TRF_AREA_;
           lord_error(TRF_AREA_,LORD("Out of area lat: %9.5lf long: %9.5lf"), N*DOUT, E*DOUT); 
	}
        *Nout = N;
        *Eout = E;
        *Hout = H;
        break;
      case TSP: /* prj -> prj */
        *Nout = N;
        *Eout = E;
        res = ptg(in_lab, +1, N, E, &N, &E,  "", NULL);
        if (N < lim[0] || lim[1] < N || E < lim[2] || lim[3] < E){
           ies = TRF_AREA_;
           lord_error(TRF_AREA_,LORD("Out of area lat: %9.5lf long: %9.5lf"), N*DOUT, E*DOUT); 
	}
        if (in_lab->cstm != outlab->cstm) {
          rrs   = ptg(outlab, -1, N, E, &N, &E,  "", NULL); 
          if (rrs < res) res = rrs;
          *Nout = N;
          *Eout = E;
        }
        break;
      case IDT: /* ident, geo -> geo */
        if (N < lim[0] || lim[1] < N || E < lim[2] || lim[3] < E){
		ies = TRF_AREA_;
		lord_debug(0,LORD("limits er %9.5lf %9.5lf %9.5lf %9.5lf"),lim[0]*DOUT,lim[1] *DOUT,lim[2]*DOUT,lim[3]*DOUT);
		lord_error(TRF_AREA_,LORD("Out of area lat: %9.5lf long: %9.5lf"), N*DOUT, E*DOUT); 
	}
        *Nout = N;
        *Eout = E;
        break;
      default: /* programme error */
	   lord_error(TRF_ILLEG_,LORD("Program error")); 
	   return TRF_ILLEG_;
      } /* end switch(action) */

      if (res < RES) RES = res;
      if (ies < IES) IES = ies;
    } while (nst && RES >= TRF_TOLLE_);
  } else {
	lord_error(TRF_ILLEG_,LORD("fh_trans(i/o labels)"));
	return TRF_ILLEG_;
  }
  /* Return coord and result */
  if (IES < RES) RES = IES;
  return (RES);

/* UNDEFINE ACTION NUMBERS */
#undef  IDT
#undef  PTG
#undef  GTP
#undef  TST
}
