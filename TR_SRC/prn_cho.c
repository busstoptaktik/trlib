

/* prn_cho    ver 2003.01          # page 1    4 Jul 2003 16 33 */


/* Copyright (c) 2003 GEK, Kort-og Matrikelstyrelsen, Denmark   */
/* All rights reserved.                                         */

/* This is unpublished proprietary source code of GD, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include   <stdio.h>
#include   "geo_lab.h"

short                    prn_cho(
/*_____________________________*/
FILE                    *out,
union geo_lab           *d_lab,
double                  *crd,
struct typ_dec          *tpd,
char                    *o_str,
double                  *cmt,
double                  *old_cmt
)

{

#include                 "fputg.h"
#include                 "fputcw.h"

  struct coord_lab                *c_lab = &(d_lab->u_c_lab);
  static short                     oN, oE, q_c;
  static int                       id_check;


  switch(c_lab->lab_type) {

  case CRD_LAB:
  case HGT_LAB:

    if (id_check != c_lab->ch_tsum) {
      id_check = c_lab->ch_tsum;
      oN       = c_lab->p_seq;
      oE       = (short) (1 - oN);
      q_c      = c_lab->ncoord;
    }

    /* output of coord records */
    /* ________________________*/


/* prn_cho    ver 2003.01         # page 2    4 Jul 2003 16 33 */


    switch(q_c) {

    case 1 : /* Height */
      (void) fputg(out, *(crd), tpd, o_str);
      break;

    case 2 : /* N, E (or y,x) */
      (void) fputg(out, *(crd +oN), tpd, o_str);
      (void) fputg(out, *(crd +oE), tpd, o_str);
      break;

    case 3 : /* N, E, H  (or XYZ) */
      (void) fputg(out, *(crd +oN), tpd, o_str);
      (void) fputg(out, *(crd +oE), tpd, o_str);
      (void) fputg(out, *(crd+2), (tpd+1), o_str);
      break;

    default: /* FEJL */
      return(1);
    }

    if (*cmt != 0.0) {

      if (q_c == 3) (void) fprintf(out, " *\n;%-18s", " ");
      if (*cmt == -1.0) *cmt = *old_cmt;
      if (*cmt == -2.0) return(0);
      (void) fputcw(out, *(cmt+0), 0);
      *old_cmt  = *cmt;

      if (*(cmt+1) != 0.0 && c_lab->lab_type == CRD_LAB) {
        if (*(cmt +1) == -1.0) *(cmt +1) = *(old_cmt +1);
        if (*(cmt +1) == -2.0) return(0);
        (void) fputcw(out, *(cmt+1), 0);
        *(old_cmt +1)  = *(cmt +1);
      }
      else *(cmt +1) = *(old_cmt +1) = 0.0;
    }
    else {
      *(cmt +0) = *(old_cmt +0) = 0.0;
      *(cmt +1) = *(old_cmt +1) = 0.0;
    }
    break; /* end CRD_LAB */ /* end HGT_LAB */

  default:
    /* ALARM ACTION */
    break;

  } /* end cstm-switch */

  return(0);

}


