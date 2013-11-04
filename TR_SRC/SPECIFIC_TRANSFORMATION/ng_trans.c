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
 


/* 
#define  DEBUGNGTRANS
*/

#include    <stdio.h>
#include    <math.h>
#include    <string.h>
#include    <ctype.h>
#include    "KmsFncs.h"
#include    "trthread.h"

/* ACTION NUMBERS */
#define  IDT    0
#define  NTG    1
#define  GTN    2
#define  PTG    3
#define  GTP    4


/*TODO: see if we can get rid of THREAD_SAFE and most init actions...*/

int                    ng_trans(
/*___________________________*/
struct coord_lab     *in_lab,
struct coord_lab     *outlab,
double               N_in,
double               E_in,
double               H_in,
double              *Nout,
double              *Eout,
double              *Hout,
tab_dir               *tdir
)
{

  static  int                 init = 0;
  static GRIM   *nadg_gr96_tab=NULL;
  static struct coord_lab     w_lab;

  char                     in_cs[MLBLNG], outcs[MLBLNG];
  char                    *p_str;

  int                      in_gr, outgr, outnr = 0, ng_z;
  int                      nst, gst, act;
  int                      res = 0, ies = 0;
  double                   N, E, H, N_g, E_g, val[3];



  struct nr_mlb {
    short        trgr;
    short        trnr;
    short        chrs;
    char        *s_lab;
  } *pml;

  static struct nr_mlb    mlab[] = {
    /*  0 */ { 0,  0,  10,   "geo_nad83g"},
    /*  1 */ { 0,  1,   8,   "geo_gr96"},
    /*  2 */ { 1,  2,   6,   "*nad83g"},
    /*  3 */ { 1,  3,   4,   "*gr96"},
    /*stop*/ {-1, -1,   0,   ""}
  };

  struct act_nst {
    short     action;
    short     nstate;
  };

  /* Start values: *ptab->row, in_nr->col */
struct act_nst  *ptab;
  int              in_nr;
  static int                           ng_w;

  /* Action/state table */
  static struct act_nst ngtab[] = {
    /* nadg  = geo_nad83g: 0   */
    /* input   nadg    ggr96    *nadg    *gr96 */
    /* state :    0        1        2        3 */
    /*  0 */ {IDT,0}, {GTN,0}, {PTG,0}, {PTG,1},
    /* gr96  = geo_gr96:   1   */
    /* input   nadg    ggr96    *nadg    *gr96 */
    /* state :    0        1        2        3 */
    /*  1 */ {NTG,1}, {IDT,1}, {PTG,0}, {PTG,1},
    /* *nadg = *nad83g:    2   */
    /* input   nadg    ggr96    *nadg    *gr96 */
    /* state :    0        1        2        3 */
    /*  2 */ {GTP,2}, {GTN,0}, {IDT,2}, {PTG,1},
    /* *gr96 = *gr96:     3   */
    /* input   nadg    ggr96    *nadg    *gr96 */
    /* state :    0        1        2        3 */
    /*  3 */ {NTG,1}, {GTP,3}, {PTG,0}, {IDT,3},
  };

  if (in_lab == NULL) {
    lord_error(TRF_ILLEG_,LORD("Input label is NULL - table close down is handled elsewhere."));
    return TRF_ILLEG_;
  }
  /* Test io-labels */
  if (in_lab->lab_type == CRD_LAB && outlab->lab_type == CRD_LAB) {

    if (nadg_gr96_tab == NULL) {
      nadg_gr96_tab=get_specific_table("nadggr96.06", T3D_LAB, tdir);
     /* res  = tab3d_i("nadggr96.06", T3D_LAB, &nadg_gr96_tab, err_str);*/
      /* State/action table size and width */
      ng_z = sizeof(ngtab)/sizeof(struct act_nst);
      ng_w = (int) sqrt(1.0000001*ng_z);
      (void) conv_w_crd("geo_nad83g", &w_lab, tdir->def_lab);
      init =  (nadg_gr96_tab!=NULL && ng_z == ng_w*ng_w);
      if (!init){
	      lord_error(res,LORD( "ng_trans(table)"));
	      return res;
      }
    }

    /* Check i/o labels, init of actual transf. systems */

      /* Out-system */
      /*____________*/
      /* Datum actions */
      (void) strcpy(outcs, outlab->mlb);
      *(outcs + outlab->sepix) = '_';

      /* Group and number */
      for (pml = mlab, outgr = -1; pml->trgr < 1; pml++)
        if (!strncmp(pml->s_lab, outcs, pml->chrs)) {
          outgr = pml->trgr;
          outnr = pml->trnr;
#ifdef DEBUGNGTRANS
(void) printf("\n*OUT  grp = %4d    nr = %4d   navn = %s;",
pml->trgr, pml->trnr, pml->s_lab);
#endif
          break;
        }
      /* Group and number DTM only */
      if (outgr == -1) {
        p_str = outcs + outlab->sepix +1;
        for ( ; pml->trgr != -1; pml++)
          if (!strncmp(pml->s_lab +1, p_str, pml->chrs)) {
            outgr = pml->trgr;
            outnr = pml->trnr;
#ifdef DEBUGNGTRANS
(void) printf("\n*OUT  grp = %4d    nr = %4d   navn = %s;",
pml->trgr, pml->trnr, pml->s_lab);
#endif
            break;
          }
      }

      /* In-system */
      /*___________*/

      /* Datum actions */
      (void) strcpy(in_cs, in_lab->mlb);
      *(in_cs + in_lab->sepix) = '_';

      /* Group and number */
      for (pml = mlab, in_gr = -1; pml->trgr < 1; pml++)
        if (!strncmp(pml->s_lab, in_cs, pml->chrs)) {
          in_gr = pml->trgr;
          in_nr = pml->trnr;
#ifdef DEBUGNGTRANS
(void) printf("\n*IN   grp = %4d    nr = %4d   navn = %s;",
pml->trgr, pml->trnr, pml->s_lab);
#endif
          break;
        }
      /* Group and number DTM only */
      if (in_gr == -1) {
        p_str = in_cs + in_lab->sepix +1;
        for ( ; pml->trgr != -1; pml++)
          if (!strncmp(pml->s_lab +1, p_str, pml->chrs)) {
            in_gr = pml->trgr;
            in_nr = pml->trnr;
#ifdef DEBUGNGTRANS
(void) printf("\n*IN   grp = %4d    nr = %4d   navn = %s;",
pml->trgr, pml->trnr, pml->s_lab);
#endif
            break;
          }
      }

      /* Test identical labels */
      if (in_lab->ch_sum == outlab->ch_sum) in_nr = outnr = 0;

#ifdef  DEBUGNGTRANS
(void) lord_debug(0, LORD("in = %s, out = %s;\n"), in_cs, outcs);
(void) lord_debug(0, LORD("ng_trans in_nr     %d  outnr      %d;"), in_nr, outnr);
#endif

      if (outnr == -1 || in_nr == -1) {
	        lord_error(TRF_ILLEG_,LORD( "ng_trans(table)"));
	        return TRF_ILLEG_;
      
      }

      ptab = ngtab + ng_w*outnr;  /* output row */
     /* End of init actions */

    /* transformation module */

    nst = in_nr;               /* input col  */
    N_g = N_in;
    E_g = E_in;
    N   = N_in;
    E   = E_in;
    H   = H_in;

    do {
      gst = nst;
      act = (ptab+nst)->action;
      nst = (ptab+nst)->nstate;

#ifdef  DEBUGNGTRANS
(void) lord_debug(0, LORD("NG_TRANS ACTION  %d:   %d -> %d;"), act, gst, nst);
#endif

      switch(act) {

      case NTG: /* geo_nad83g -> geo_gr96 */
        ies = grim_values(*nadg_gr96_tab,N_g, E_g, 0, val);
#ifdef  DEBUGNGTRANS
(void) lord_debug(0, LORD("NG_TRANS ACTION  NTG:   %f, %f, res=%d;"), N_g*180.0 / M_PI, E_g*180.0 / M_PI, ies);
#endif
        if (ies == 0) {
          N  = N_g + *(val   ); 
          E  = E_g + *(val +1);
          H +=       *(val +2);
        } else
        /*if (ies == TRF_AREA_) */{
          N   = N_g + 0.0000001424;
          E   = E_g + 0.0000001111;
          ies = 0;
        } /*else {
          if (ies < res) res = ies;
         
             lord_debug(0,LORD("ng_trans, ies: %d, N: %.6f, E: %.6f"),ies, N_g, E_g);
        }*/
#ifdef  DEBUGNGTRANS
(void) lord_debug(0, LORD("NG_TRANS ACTION  NTG:   %f, %f, res=%d;"), N*180.0 / M_PI, E*180.0 / M_PI, ies);
#endif
        break;

      case GTN: /* geo_gr96 -> geo_nad83g */
        ies = grim_values(*nadg_gr96_tab,N_g, E_g, 0, val);
#ifdef  DEBUGNGTRANS
(void) lord_debug(0, LORD("NG_TRANS ACTION  GTN1:   %f, %f, res=%d;"), *val, *(val+1), ies);
#endif
        if (ies == 0) {
          N = N_g - *(val   );
          E = E_g - *(val +1);
           ies = grim_values(*nadg_gr96_tab,N, E, 0, val);
#ifdef  DEBUGNGTRANS
(void) lord_debug(0, LORD("NG_TRANS ACTION  GTN2:   %f, %f, res=%d;"), *val, *(val+1), ies);
#endif
          if (ies == 0) {
            N = N_g - *(val   );
            E = E_g - *(val +1);
            ies = grim_values(*nadg_gr96_tab,N, E, 0, val);
#ifdef  DEBUGNGTRANS
(void) lord_debug(0, LORD("NG_TRANS ACTION  GTN2:   %f, %f, res=%d;"), *val, *(val+1), ies);
#endif
            if (ies == 0) {
              N  = N_g - *(val   ) * 0.0000001;
              E  = E_g - *(val +1) * 0.0000001;
              H -=       *(val +2);
            } else
            /*if (ies == TRF_AREA_) */{
              N   = N_g - 0.0000001424;
              E   = E_g - 0.0000001111;
              ies = 0;
            }/* else {
              if (ies < res) res = ies;
              res = ies;
             
             lord_debug(0,LORD("ng_trans, ies: %d, N: %.6f, E: %.6f"),ies, N_g, E_g);
            }*/
          } else
          /*if (ies == TRF_AREA_)*/ {
            N   = N_g - 0.0000001424;
            E   = E_g - 0.0000001111;
            ies = 0;
          }/* else {
            if (ies < res) res = ies;
            res = ies;
         
             lord_debug(0,LORD("ng_trans, ies: %d, N: %.6f, E: %.6f"),ies, N_g, E_g);
          }*/
        } else
        /*if (ies == TRF_AREA_)*/ {
          N   = N_g - 0.0000001424;
          E   = E_g - 0.0000001111;
          ies = 0;
        }/* else {
          if (ies < res) res = ies;
          res = ies;
       
             lord_debug(0,LORD("ng_trans, ies: %d, N: %.6f, E: %.6f"),ies, N_g, E_g);
        }*/
        break;

      case  PTG: /* PRJ -> GEO */
        /*______________________________*/
        ies = ptg(in_lab, +1, N, E, &N, &E,
                  "\nng_trans error:", NULL);
        N_g = N;
        E_g = E;
#ifdef  DEBUGNGTRANS
(void) lord_debug(0, LORD("NG_TRANS ACTION  PTG:  %s, %f, %f, res=%d;"), in_lab->mlb, N*180.0 / M_PI, E*180.0 / M_PI, ies);
#endif
        if (ies < res) res = ies;
        break;

      case  GTP: /* GEO -> PRJ */
        /*______________________________*/
        ies = ptg(outlab, -1, N, E, &N, &E,
                  "\nng_trans error:", NULL);
#ifdef  DEBUGNGTRANS
(void) lord_debug(0, LORD("\n*NG_TRANS ACTION  GTP:   %s, res=%d;"), outlab->mlb, ies);
#endif
        if (ies < res) res = ies;
        break;

      case IDT: /* ident, no action */
        break;

      default: /* programme error */
	      lord_error(TRF_ILLEG_,LORD("ng_trans(prog error)"));
	      return TRF_ILLEG_;
      
      } /* end switch(action) */
      if (ies < res) res = ies;

    } while (nst != gst && res >= TRF_TOLLE_);
  }
  else {
	lord_error(TRF_ILLEG_,LORD("ng_trans(i/o labels)"));
	return TRF_ILLEG_;
  
  }

  /* Return coord and result */
  *Nout = N;
  *Eout = E;
  *Hout = H;
  return (res);

  /* UNDEFINE ACTION NUMBERS */
#undef  IDT
#undef  NTG
#undef  GTN
#undef  PTG
#undef  GTP

}


