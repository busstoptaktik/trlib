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
#include    <stdlib.h>
#include    <string.h>
#include    "KmsFncs.h"
#include    "trthread.h"

/* TEST
#define  DEBUGGDTRANS
TEST */

int                        gd_trans2(
/*_________________________________*/
union geo_lab            *i_lab,
union geo_lab            *o_lab,
double                    N_in,
double                    E_in,
double                    H_in,
double                   *X,
double                   *Y,
double                   *Z,
double                   *ogh,
char                      req_gh,
struct mgde_str          *grid_tab,
char                     *usertxt,
FILE                     *tr_error
)
{

  /* GD_STATE::
  static THREAD_SAFE  int      (*dfb_trf)(
    union geo_lab      *in_lab,
    union geo_lab      *outlab,
    double              N,    double   E,    double   H,
    double             *Nout, double  *Eout, double  *Hout,
    char               *usertxt,
    FILE               *tr_error
  );

  char                        err_txt[1024];
                                       /* grid systems geoid/table */
  static THREAD_SAFE  union geo_lab    H0_lab, H1_lab;
  static THREAD_SAFE  union geo_lab    H2_lab, H3_lab; /* grid cstm dh*/
  static THREAD_SAFE  union geo_lab   *i_Rlab;  /* beg REG of ETPL  */
  static THREAD_SAFE  union geo_lab   *G_Rlab;  /* end REG of E geoid */
  static THREAD_SAFE  union geo_lab   *T_Rlab;  /* end REG of ETPL dh */
  static THREAD_SAFE  union geo_lab   *O_Rlab;  /* end REG of ETPL tr */
  static THREAD_SAFE  union geo_lab   *i_Nlab;  /* beg NON of ETPL  */
  static THREAD_SAFE  union geo_lab   *G_Nlab;  /* end NON of E geoid */
  static THREAD_SAFE  union geo_lab   *T_Nlab;  /* end NON of ETPL dh */
  static THREAD_SAFE  union geo_lab   *O_Nlab;  /* end NON of ETPL tr */
  static THREAD_SAFE  union geo_lab    t_lab;   /* non-reg gateway */
  static THREAD_SAFE  union geo_lab    g_lab;   /* geo_* PRE/ANT   */

  static THREAD_SAFE  struct mgde_str *s_grid_tab = NULL;
  static THREAD_SAFE  struct mgde_str  h_grid_tab;
  static THREAD_SAFE  struct htr_c_str htr_const;

  static THREAD_SAFE  int              i_chsum = 0;
  static THREAD_SAFE  int              o_chsum = 0, b_lev, s_lev;
  static THREAD_SAFE  short            init = 0, iEh_req =0, oEh_req =0;
  static THREAD_SAFE  short            s_req_gh = 0, ghr = 0;
  static THREAD_SAFE  short            s_req_dh = 0, s_req_tv = 0;
  static THREAD_SAFE  short            sta[4], stp[4], ptp[4];
  static THREAD_SAFE  char             i_sep, nonp_i[4];
  static THREAD_SAFE  char             o_sep, nonp_o[4];

  static THREAD_SAFE  struct coord_lab    *i_clb, *o_clb;
  GD_STATE */

  GD_STATE                   *gd_state;

  struct coord_lab           *H_clb  = &(H0_lab.u_c_lab);
  struct coord_lab           *H1_clb = &(H1_lab.u_c_lab);
  struct coord_lab           *t_clb  = &(t_lab.u_c_lab);

  union geo_lab              *i_wlab = NULL;  /* begin REG of ETPL  */
  union geo_lab              *o_wlab = NULL;  /* end   REG of ETPL  */
  union geo_lab             **o_nlab = NULL;  /* end   REG of ETPL  */
  union geo_lab             **o_rlab = NULL;  /* end   REG of ETPL  */
  union geo_lab               w_lab;   /* work label         */
  struct coord_lab           *w_oclb;
  union rgn_un                rgn_DE, rgn_EE, rgn_GR;

  short                       i_rgn, o_rgn;
  int                         action, lev, R_N, rs = 0, RES, RGH = 0;
  int                         ies = 0, res, dsh = 0, iEhr, oEhr, req_th = 0;
  char                        dstr[128];
  double                      N, E, H, gh, igh, dh = 0.0, Nh = 0.0;
  double                      NN, EE, HH, iEh, oEh = 0.0, th = 0.0;
  
  enum action{PRE        /* crt_???? -> geoE???? */,
              NTP        /* Non_reg  -> Proj     */,
              PTG        /* Proj     -> Geo      */,
              GTC        /* Geo      -> Cart     */,
              CTC        /* Cart     -> Cart     */,
              CTG        /* Cart     -> Geo      */,
              GTP        /* Geo      -> Proj     */,
              PTN        /* Proj     -> Non_reg  */,
              ANT        /* geoE???? -> crt_???? */,
              IDT        /* Ident                */,
              ILL = -1   /* Illegal           */}


  /* ++++++++++++++        WARNING          ++++++++++++++ */
  /* call with non std geoids may give erroneous Heights   */
  /* this is not an error but intended by the users call   */
  /* ++++++++++++++        WARNING          ++++++++++++++ */

  /* Nonregular Geogr is handled as Nonregular Proj */

  /*            ENGSAGER'S TRANSFORMATION PRODUCTION LINE (ETPL)   */
  /*             <-------UP-HILL------> <------DOWN-HILL------>    */
  /* cstm: crt_  NON   PRJ   GEO   CRT   CRT   GEO   PRJ   NON   crt_ */
  /* cstm:  0    8-15  3-7    2     1     1     2    3-7   8-15    0  */
  /* io_nr: 4     1     2     3     4     4     3     2     1      4  */
  /* action:  PRE   NTP   PTG   GTC   CTC   CTG   GTP   PTN    ANT    */
  /* action:   0     1     2     3     4     5     6     7      8     */
  /* nmb_sequence at datum shift = both up-hill and down-hill */
  /* strt_nr =     *io_nr_tab[i_cstm]                         */
  /* stop_no = 8 - *io_nr_tab[o_cstm]                         */

  /* nmb_seq, no dshift, increasing io_nmb = only up-hill     */
  /* strt_nr =     *io_nr_tab[i_cstm]                         */
  /* stop_no =     *io_nr_tab[o_cstm] - 1                     */

  /* nmb_seq, no dshift, decreasing io_nmb = only down-hill   */
  /* strt_nr = 9 - *io_nr_tab[i_cstm]                         */
  /* stop_no = 8 - *io_nr_tab[o_cstm]                         */

  short                 io_nr_tab [] = {
    /* 0 -  7, regular transf. */ -1, 4, 3, 2,  2, 2, 2, 2,
    /* 8 - 15, non-reg transf. */  1, 1, 1, 1,  1, 1, 1, 1
  };

  /* The STD geoid in DK gives the Normal Height DVR90. The     */
  /* Height is converted to DNN iff requested                   */

  /* sequence of transformations : (part of ETPL)               */
  /* lev == 0 : get Geoid Height : gh                           */
  /*     or   : get Table Value  : tv                           */
  /* lev == 1 : do. : swop to other coordsys (EE_geo_../geo_eur)*/
  /* lev == 2 : get Datum Shift of Height dtms : dh             */
  /* lev == 3 : transform coordinates                           */

  /* Geoid Height gh is not transformed at Datum Shifts of Height*/

  /*                     Geoid requested ::  (allso -g param)   */
  /* crt_          :: is treated as sepch == E                  */
  /* H             :: is treated as sepch == N                  */
  /* in_sepch_out dsh  ghr H_in  igh  gh  H_in   Nh    Hout     */
  /*  _        _   .       =0.0  0.0                   =0.0     */
  /*  _        N   .       =0.0  0.0                   =0.0     */
  /*  _        E   .    +  =0.0        +  +igh                  */

  /*  N        _   .             0.0                   =0.0     */
  /*  N        N   .             0.0                   =H_in    */
  /*  N        E   .    +              +  +igh         =H_tr    */

  /*  E        _   .                                   =0.0     */
  /*  E        N   .    +        0.0   +        Htr-gh =Nh      */
  /*  E        E   .             0.0                   =H_tr    */

  /*  gh : +       :: calculate gh in system t_lab              */
  /*  igh          :: calculate gh in system i_lab              */

  /* Asume  H_in is Ellipsoidal height !!!!!               */
  /* then   H_in = h = H + N = Nh + igh                    */
  /* The transformed to t_lab is also an Ellisoidal height */
  /*        H_tr = h = H + N = Nh + ogh                    */
  /* Therefor Nh = H_in -igh = H_tr - ogh                  */
  /* which gives         igh = H_in - H_tr +ogh            */
  /* and      Nh = H_in -igh                               */

  /* Asume  H_in is Normal height                          */
  /* treat  H_in as a Ellipsoidal heigh as above           */
  /* but Nh should be replaced by Nh_false                 */
  /* and      Nh = H_in                                    */

  /* req_gh  > 0    :: calculate gh in o_lab               */
  /* req_gh == 0    :: calculate gh in o_lab if needed     */
  /* req_gh  < 0    :: DO NOT get gh                       */
  /* H_in/(H_in+igh) transformed to o_lab                  */
  /*                     ogh = H_tr - Nh                   */

  /* In prg is iEh replacing H_in (CRT causes troubles)    */

  if (i_lab == NULL) {
    (void) ng_trans(NULL, NULL, 0.0, 0.0, 0.0, &N, &E, &H, "", NULL);
    i_clb           = NULL;
    if (h_grid_tab.init != 0) {
      h_grid_tab.init = 0;
      (void) fclose(h_grid_tab.table_u[0].fd);
    }
    if (grid_tab != NULL)
        if (grid_tab->init != 0) (void) geoid_c(grid_tab, 0, NULL);
    return(0);
  }

  *X = *Y = *Z = 0.0;

  if (i_sep == '_') H_in = 0.0;
  igh    = gh = *ogh = 0.0;
  iEhr   = iEh_req;
  oEhr   = 0;
  N      = N_in;    E    = E_in;    H = iEh = H_in;
  RES    = 0;
  b_lev  = (ghr || s_req_tv) ? 0 : ((s_req_dh) ? 2 : 3);
  for (lev = b_lev; lev <= s_lev && RES >= TRF_TOLLE_; lev++) {

    switch (lev) {
    case 0:
      i_wlab = G_Nlab;
      o_wlab = G_Rlab;
      break;
    case 1: /* swop geo_system (DK <-> EE) for grid value */
      w_lab   = H0_lab;
      H0_lab  = H1_lab;
      H1_lab  = w_lab;
      ies     = sta[0];
      sta[0]  = sta[1];
      sta[1]  = (short) ies;
      ies     = stp[0];
      stp[0]  = stp[1];
      stp[1]  = (short) ies;
      o_wlab  = &H0_lab;
      lev     = 0;
	break;
    case 2:
      i_wlab = T_Nlab;
      o_wlab = T_Rlab;
      break;
    case 3:
      oEhr   = oEh_req;
      i_wlab = O_Nlab;
      o_wlab = O_Rlab;
      break;
    }
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("lev %d, nonp %d, %d, Ehr %d %d;"), lev, nonp_i[lev], nonp_o[lev], iEhr, oEhr);
#endif

    for (action = sta[lev], res = 0;
         action <= stp[lev] && res >= TRF_TOLLE_; action++) {

      switch(action) {
   
      case  PRE: /* crt_* -> geoE* */
        /*_____________________*/
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("case 0: CRT -> GEO"));
	(void) lord_debug(0, LORD("   %s;"), (g_lab.u_c_lab).mlb);
	(void) lord_debug(0, LORD(" %7.2f  %7.2f   %7.2f;"), N, E, H);
#endif
        ies = gtc(&g_lab, -1, N, E, H, &N, &E, &H, usertxt, tr_error);
        if (iEhr) {
          iEh  = H;
          iEhr = 0;
        }
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("-> %7.6f  %7.6f   %7.2f;"), N/M_PI*180.0, E/M_PI*180.0, H);
	(void) lord_debug(0, LORD("-> %7.2f  %7.2f   %7.2f, iEh=%f;"), N, E, H, iEh);
#endif
        break;

      case  NTP: /* NON -> PRJ (or GEO) */
        /*______________________________*/
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("case 1: NON -> PRJ"));
	(void) lord_debug(0, LORD("   %s -> %s;"), (i_Nlab->u_c_lab).mlb, (i_wlab->u_c_lab).mlb);
	(void) lord_debug(0, LORD(" %7.2f  %7.2f   %7.2f;"), N, E, H);
#endif
        ies = (*dfb_trf)(i_Nlab, i_wlab,
                         N, E, H, &N, &E, &HH, usertxt, tr_error);
        if (lev == 3 && ((*dfb_trf) == ng_trans)) {
          req_th = -1;
          th     = HH - H;
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("case 1: NON -> PRJ: th = %f"), th);
#endif
        }
        if (nonp_i[b_lev]) action ++;
        break;

      case  PTG: /* PRJ (or GEO) -> GEO */
        /*______________________________*/
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("\n*case 2: PRJ -> GEO"));
	(void) lord_debug(0, LORD("   %s;"), (i_Rlab->u_c_lab).mlb);
#endif
        ies = ptg(i_Rlab, +1, N, E, &N, &E, usertxt, tr_error);
        if (ptp[lev]) action += 3;
        break;

   
      case  GTC: /* GEO -> CRT */
        /*_____________________*/
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("\n*case 3: GEO -> CRT"));
	(void) lord_debug(0, LORD("   %s;"), (i_Rlab->u_c_lab).mlb);
	(void) lord_debug(0, LORD("\n*  %10.5f  %10.5f    %8.5f  %8.5f;"), N, E, N*180.0/M_PI, E*180.0/M_PI);
#endif
        if (req_th < 0 && stp[3] == GTC) { /* dh from 3D table_trf */
          H      += th;
          req_th  = 0;
        }
        ies = gtc(i_Rlab, +1, N, E, H, &N, &E, &H, usertxt, tr_error);
        break;

      case  CTC: /* CRT -> CRT */
        /*______________________*/
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("case 4: CRT -> CRT"));
	(void) lord_debug(0, LORD("   %s -> %s;"), (i_Rlab->u_c_lab).mlb, (o_wlab->u_c_lab).mlb);
#endif
        if (iEhr) {
          (void) gtc(i_lab, -1, N, E, H, &NN, &EE, &HH, "", NULL);
          iEh  = HH;
          iEhr = 0;
        }
        ies = ctc(i_Rlab, o_wlab, N, E, H, &N, &E, &H,
                  usertxt, tr_error);
        break;
   
      case  CTG: /* CRT -> GEO */
        /*_____________________*/
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("case 5: CRT -> GEO"));
	(void) lord_debug(0, LORD("   %s;"), (o_wlab->u_c_lab).mlb);
#endif
        ies = gtc(o_wlab, -1, N, E, H, &N, &E, &H, usertxt, tr_error);
        break;

      case  GTP: /* GEO -> PRJ (or GEO) */
        /*______________________________*/
        if (!nonp_o[lev]) {
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("case 6: GEO -> PRJ"));
	(void) lord_debug(0, LORD("   %s;"), (o_wlab->u_c_lab).mlb);
#endif
          ies = ptg(o_wlab, -1, N, E, &N, &E, usertxt, tr_error);
        }
        break;

      case  PTN: /* PRJ (or GEO) -> NON */
        /*______________________________*/
#ifdef DEBUGGDTRANS
  (void) fprintf(stdout, "case 7: PRJ -> NON");
  (void) fprintf(stdout, "   %s -> %s;",
         (i_Nlab->u_c_lab).mlb, (i_wlab->u_c_lab).mlb);
  (void) fprintf(stdout, "  %10.5f  %10.5f    %8.5f  %8.5f  %6.2f;",
  N, E, N*180.0/M_PI, E*180.0/M_PI, H);
#endif
        ies = (*dfb_trf)(i_Nlab, i_wlab,
                         N, E, H, &N, &E, &HH, usertxt, tr_error);
        if (lev == 3 && ((*dfb_trf) == ng_trans)) {
          req_th = -1;
          th     = HH - H;
#ifdef DEBUGGDTRANS
(void) fprintf(stdout, "case 7: PRJ -> NON: th = %f", th);
#endif
        }
        break;
   
      case  ANT: /* geoE* -> crt_* */
        /*_____________________*/
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("case 8: GEO -> CRT"));
	(void) lord_debug(0, LORD("   %s;"), (g_lab.u_c_lab).mlb);
	(void) lord_debug(0, LORD(" %10.5f  %10.5f    %8.5f  %8.5f  %6.2f;"), N, E, N*180.0/M_PI, E*180.0/M_PI, H);
#endif
        if (oEhr == 1) {
          oEh  = H;
          oEhr = 0;
        }
        if (req_th < 0) { /* dh from 3D table_trf */
          H      += th;
          req_th  = 0;
        }
        ies = gtc(&g_lab, +1, N, E, H, &N, &E, &H, usertxt, tr_error);
        break;

      case  IDT: /* No action, ident, ok */
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("case 9: IDT, oEhr = %d, lev = %d"), oEhr, lev);
#endif
        if (oEhr) {
          oEh  = Nh + igh;
          oEhr = 0;
        }
        ies  = 0;
        break;

      case  ILL: /* Illegal */
        *X  = *Y = *Z = 0.0;
        return((tr_error==NULL) ? TRF_ILLEG_ :
                t_status(tr_error, "",
                "gd_trans(unintelligible labels)", TRF_ILLEG_));

      default:
        *X = *Y = *Z = 0.0;
        return((tr_error==NULL) ? TRF_ILLEG_ :
                t_status(tr_error, "",
                "gd_trans(unintelligible labels)", TRF_ILLEG_));
      } /* end switch(action) */

      if (res > ies) res = ies;
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("*    res = %d;"), res);
#endif

	} /* action LOOP */

    if (RES > res) RES = res;
    switch (lev) {
    case 0:
      if (RES >= TRF_TOLLE_) {
        RGH = grid_val(&H0_lab, grid_tab, N, E, &gh, err_txt);
        if (sta[0] == CTG && iEhr) {
          iEh  = H;
          iEhr = 0;
        }
        if (RGH == 0) {
          if (ghr) {
            ++ lev;
            if (! s_req_dh) ++ lev;
            Nh  = (i_sep != 'E') ? iEh : H - gh;
            igh = iEh - H + gh;
            if (oEhr && stp[3] <= GTC) {
              oEh  = Nh + igh;
              oEhr = 0;
            }
            H   = (i_sep != 'E') ? H_in + igh : H_in;
          }
        } else
        if (RGH < 0) {
          if (RES > RGH) RES = RGH;
          if (tr_error != NULL)
          	(void) lord_error(0, LORD("%s    %s"), err_txt, usertxt);
          gh   = 9999.9;
          *ogh = gh;
        }
        N = N_in;
        E = E_in;
      } else *ogh = gh = 0.0;

      break;

    case 2:
      if (RES >= TRF_TOLLE_) {
        switch (s_req_dh) {
        case 1: /* DVR90 <-> DNN, DVR90 <-> FCSVR10, FVR09 <-> MSL */
          RGH = grid_val(&H2_lab, &h_grid_tab, N, E, &dh, err_txt);
          /* table will give std dh to dvr90 */
          break;
        case 2: /* constant:: DVR90 <-> KN44 */
          RGH = 0;
          dh  = htr_const.a1;
          break;
        case 3: /* linear:: DVR90 <-> EVRF20?? */
          RGH = 0;
          dh  = htr_const.a1 + htr_const.a2 * htr_const.M0 * (N - htr_const.LAT0)
              + htr_const.a3 * htr_const.N0 * (E - htr_const.LON0) * cos(N);
          /* dh = a1 - a2* M0 *(dLAT) - a3* N0 * (dLON) cos(LAT)            */
        }
        if (htr_const.inv > 0) dh = -dh;
        H = (i_sep != 'E') ? H_in + igh : H_in;
        if (RGH == 0) {
          if (dh > 9990.0) dh = 0.0;
          if (htr_const.inv == 0) { /* direct */
            H  += dh;
            Nh += dh;
          }
        } else {
          if (RES > RGH) RES = RGH;
          dh = 0.0;
          if (tr_error != NULL)
			(void) lord_error(0, LORD("%s    %s"), err_txt, usertxt);
        }
      }
      else {
        dh  = 0.0;
        H   = (i_sep != 'E') ? H_in + igh : H_in;
      }
      N   = N_in;
      E   = E_in;
      break;

    case 3:
      if (RES >= TRF_TOLLE_) {
        if (oEhr == 1) {
          oEh  = H;
          oEhr = 0;
        } else
        if (oEhr == 2) {
          (void) gtc(o_wlab, -1, N, E, H, &NN, &EE, &HH, "", NULL);
          oEh  = HH;
          oEhr = 0;
        }
        if (req_gh)    *ogh = oEh - Nh;
        if (o_sep == '_') H = 0.0;
        else
        if (o_sep == 'H' || o_sep == 'N')
            H = (i_sep == 'E') ? Nh
              : ((s_req_dh && htr_const.inv == 0) ? H_in + dh : H_in);
        if (s_req_dh && htr_const.inv) H += dh; /* inverse */
        if (req_th < 0) H += th; /* dh from 3D table_trf */
      }
      break;
    }
#ifdef DEBUGGDTRANS
	(void) lord_debug(0, LORD("H_in = %7.3f, H   = %7.3f, iEh = %7.3f, Nh = %7.3f;"), H_in, H, iEh, Nh);
	(void) lord_debug(0, LORD("gh   = %7.3f, igh = %7.3f, oEh = %7.3f, dh = %7.3f, th = %7.3f;") ,gh, igh, oEh, dh, th);
#endif

  } /* lev LOOP */

  /* return of transformation */
  *X = N;
  *Y = E;
  *Z = H;

  return(RES);

  /* Undef macros */

#undef NTP
#undef PTG
#undef GTC
#undef CTC
#undef CTG
#undef GTP
#undef PTN
#undef IDT
#undef ILL

#undef REG_REG
#undef NON_REG
#undef REG_NON
#undef NON_NON

}
