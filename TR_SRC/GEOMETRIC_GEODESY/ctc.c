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
 
#include    <math.h>
#include    <stdio.h>
#include    "KmsFncs.h"

#define   ID   0
#define   EW   1
#define   NW   2
#define   IW   3

int                   ctc(
/*______________________*/
struct coord_lab        *i_lab,
struct coord_lab        *o_lab,
double                Xi,
double                Yi,
double                Zi,
double                *Xo,
double                *Yo,
double                *Zo,
char                  *usertxt,
FILE                  *tr_error
)
{


  double                   X, Y, Z;
  int                      act, nst, ost, result = 0, res;
  short                    p_no;
  char                     pn[24], *p_nm = pn;
  char                     en[24], *e_nm = en;
  int                      id, od, ci, co;
  static  int               init = 0;

  struct act_nst {
    short     action;
    short     nstate;
  }    *ptab;


  static struct act_nst dtmtab[] = {
    /* in            WGS84    ED50     NWL9D   EUREF89   GR96 */
    /* state           0        1        2        3        4  */
    /* out                                                    */
    /* WGS84   */   { ID,0},  {EW,0},  {NW,0},  {ID,0},  {ID,0},
    /* ED50    */   {-EW,1},  {ID,1},  {NW,0},  {ID,0},  {ID,0},
    /* NWL9D   */   {-NW,2},  {EW,0},  {ID,2},  {ID,0},  {ID,0},
    /* EUREF89 */   {-ID,3},  {EW,0},  {NW,0},  {ID,3},  {ID,0},
    /* GR96    */   {-ID,4},  {EW,0},  {NW,0},  {ID,0},  {ID,4}
    /* 4 places EUREF89 <- / -> WGS84 chance to IW when NOT IDENT */
    /* 4 places GR96    <- / -> WGS84 chance to GW when NOT IDENT */
  };
  /* WHEN CHANGE OF SIZE OF TABLE:: remenber width on page 3 */

  static struct dsh_str   ew_a, *ew = &ew_a; /* PERHAPS THESE NEED TO BE THREAD LOCAL? */
  static struct dsh_str   nw_a, *nw = &nw_a;
  static struct dsh_str   iw_a, *iw = &iw_a;
  struct dsh_str   *fc = NULL, *tc = NULL, *wc = NULL;
  char rgn[3];
  short mask;
  
  if (i_lab->lab_type != CRD_LAB) {
          return((tr_error==NULL) ? TRF_ILLEG_ :
                  t_status(tr_error, usertxt,
                  "ctc(input label fault)", TRF_ILLEG_));
  }

  if (o_lab->lab_type != CRD_LAB) {
    return((tr_error==NULL) ? TRF_ILLEG_ :
            t_status(tr_error, usertxt,
            "ctc(output label fault)", TRF_ILLEG_));
  }

  if (!init) {
    (void) set_dtm_1(-1, "ed50",   &p_no, p_nm, e_nm, rgn,&mask,ew);
    (void) set_dtm_1(-1, "nwl9d",   &p_no, p_nm, e_nm, rgn,&mask,nw);
    (void) set_dtm_1(-1, "etrs89", &p_no, p_nm, e_nm, rgn,&mask,iw);
    init = 1;
  }

  ci = id = i_lab->datum;
  fc = &(i_lab->dsh_con);
  if (id > 5) ci = i_lab->p_dtm;

  co = od = o_lab->datum;
  tc = &(o_lab->dsh_con);
  if (od > 5) co = o_lab->p_dtm;


  /* Start of transformations */
  if (ci > 0 && co > 0) {

    X = Xi;
    Y = Yi;
    Z = Zi;

    /* transformation to central system */
    if ((ci - id) && fc->tp != 0)
      result = cic(fc, +1, X, Y, Z, &X, &Y, &Z, usertxt, tr_error);

    ptab = dtmtab + 5*(co - 1); /* TABLE WIDTH */
    nst  = ci - 1;

    do  {

      ost = nst;
      act = (ptab+nst)->action;
      nst = (ptab+nst)->nstate;

      /*
      (void) printf("\n CTC inside central: act = %+2d nst = %2d",
      act, nst);
      */

      /* transformation inside central system */
      switch(act) {
      case  NW: /* NWL9D->WGS84 */
      case -NW: /* WGS84->NWL9D */
        wc = nw;
        break;

      case  EW: /* ED50->WGS84 */
      case -EW: /* WGS84->ED50 */
        wc = ew;
        break;

      case  IW: /* ETRS89->WGS84 */
      case -IW: /* WGS84->ETRS89 */
        wc = iw;
        break;

      default:
        break;
      }
      if (act) {
        res = (wc->tp == 0) ? 0
            : cic(wc, act, X, Y, Z, &X, &Y, &Z, usertxt, tr_error);
        if (res < result) result = res;
      }

    } while (ost != nst  &&  result >= TRF_TOLLE_);

    /* transformation from central system */
    if ((co - od) && tc->tp != 0) {
      res = cic(tc, -1, X, Y, Z, &X, &Y, &Z, usertxt, tr_error);
      if (res < result) result = res;
    }

  }
  else {
      return((tr_error==NULL) ? TRF_ILLEG_ :
      	      t_status(tr_error, usertxt,
              "ctc(unknown datum shift)", TRF_ILLEG_));
  }

  *Xo = X;
  *Yo = Y;
  *Zo = Z;
  return(result);

#undef   ID
#undef   EW
#undef   NW
#undef   IW

}
