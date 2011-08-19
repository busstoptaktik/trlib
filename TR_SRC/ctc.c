

/*  ctc   ver 1999.01            # page 1   12 Jan 1999 15 40 */


/* Copyright (c) 1999, Kort-og Matrikelstyrelsen, Denmark     */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of LG, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not */
/* indicate an intention of publishing this code.             */

#include <math.h>
#include <stdio.h>
#include "geo_lab.h"

#define   ID   0
#define   EW   1
#define   NW   2
#define   IW   3

int                   ctc(
/*______________________*/
union geo_lab         *i_lab_u,
union geo_lab         *o_lab_u,
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

#include          "cic.h"
#include          "set_dtm.h"
#include          "t_status.h"

  struct coord_lab   *i_lab = &(i_lab_u->u_c_lab);
  struct coord_lab   *o_lab = &(o_lab_u->u_c_lab);

  double             X, Y, Z;
  int                act, nst, ost, result = 0, res;
  short              p_no;
  char               pn[24], *p_nm = pn;
  char               en[24], *e_nm = en;
  int                id, od, ci, co;
  static int         init = 0;

  struct act_nst {
    short     action;
    short     nstate;
  }    *ptab;


/*  ctc   ver 1999.01            # page 2   12 Jan 1999 15 40 */


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

  static struct dsh_str   ew_a, *ew = &ew_a;
  static struct dsh_str   nw_a, *nw = &nw_a;
  static struct dsh_str   iw_a, *iw = &iw_a;
  static struct dsh_str   *fc, *tc, *wc;

  if (i_lab->lab_type != CRD_LAB) {
    return(t_status(tr_error, usertxt,
           "ctc(input label fault)", TRF_ILLEG_));
  }

  if (o_lab->lab_type != CRD_LAB) {
    return(t_status(tr_error, usertxt,
           "ctc(output label fault)", TRF_ILLEG_));
  }

  if (!init) {
    init = 1;
    (void) set_dtm(-1, "ed50",   &p_no, p_nm, e_nm, ew);
    (void) set_dtm(-1, "nwl9d",   &p_no, p_nm, e_nm, nw);
    (void) set_dtm(-1, "euref89", &p_no, p_nm, e_nm, iw);
  }

  ci = id = i_lab->datum;
  fc = &(i_lab->dsh_con);
  if (id > 5) ci = i_lab->p_dtm;


/*  ctc   ver 1999.01            # page 3   12 Jan 1999 15 40 */


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


/*  ctc   ver 1999.01            # page 4   12 Jan 1999 15 40 */


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
    return(t_status(tr_error, usertxt,
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


