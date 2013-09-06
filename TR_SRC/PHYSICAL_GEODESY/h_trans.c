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
#include    <string.h>
#include    <math.h>
#include    "KmsFncs.h"
#include    "trthread.h"

int                      h_trans(
/*_____________________________*/
struct coord_lab      *ic_lab,
short                    oh_dtm,
double                   B_in,
double                   L_in,
double                  *dh,
char                    *tr_str
)
{

 

  struct h_trc_str {
    struct coord_lab ref_lab;
    short oh_dtm, tr_type;
    double LAT0, LON0, M0, N0;
    double a1, a2, a3;};

  struct coord_lab           *rc_lab = &(h_trc.ref_lab);
  char                        pth_mlb[512], c_mlb[MLBLNG], *p_tp;
  int                         res = 1, qr, used;
  long                        pos = 0;
  short                       p_no;
  char                        i_nm[MLBLNG], p_nm[MLBLNG], e_nm[MLBLNG];
  char                        o_nm[MLBLNG];
  struct typ_dec              g_tpd;
  struct dsh_str              trp;
  extern def_data *DEF_DATA;
  def_hth_tr   *hth_tr;


  /* basic transformation formula UELN                              */
  /* h_EVRFxx = h_DVRyy + dh                                        */
  /* dLAT     = B_in - LAT0                                         */
  /* dLON     = L_in - LON0                                         */
  /* dh = a1 - a2* M0 *(dLAT) - a3* N0 * (dLON) cos(LAT)            */

  /* LAT0, LON0, M0, N0, coord and Curvatures of ref stn            */

  if (ic_lab->lab_type != CRD_LAB)
    return(s_status(tr_str, "h_trans(i labels error)", TRF_ILLEG_));

  if (DEF_DATA==NULL)
	return -2;
  hth_tr=def_data->hth_entries;   

 /* FInd de rigtige konstanter---- og retning*/
  /*
  *dh = h_trc.a1 + h_trc.a2 * h_trc.M0 * (B_in - h_trc.LAT0)
      + h_trc.a3 * h_trc.N0 * (L_in - h_trc.LON0) * cos(B_in);
  /* dh = a1 - a2* M0 *(dLAT) - a3* N0 * (dLON) cos(LAT)            */
  *dh=0;
  if (inv) *dh = -*dh;
  return(0);

}
