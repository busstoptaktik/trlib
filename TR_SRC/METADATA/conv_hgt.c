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

/* conv_hgt(name, p_lb_ex, lab) fills data in the label        */
/* initiated by conv_lab.                                      */
/*    result ==  0 : no lab                                    */
/*    result ==  2 : height lab            in lab              */
/*    UNDEFSYS     : unrecognized name                         */
/* an unrecognized name is considered a filename containing a  */
/* genuine label                                               */

#include    <stdio.h>
#include    <string.h>
#include    <stdarg.h>

#include    "KmsFncs.h"

int                      conv_hgt(
/*______________________________*/
char                    *lab_info,
struct lab_def_str      *p_lb_ex,
struct hgt_lab          *h_lab,
def_data                  *DEF_DATA,
...
)
{

  FILE                      *iofile = (FILE *) NULL;
  va_list                    o_p;

  int                        conv_mode, i;
  short                      par_dtm, mask;
  double                     ell_p[10];
  char                       e_name[24], p_name[24];
  char                       t_lab_a[2*MLBLNG], *t_lab = t_lab_a;
  char                      *p_sys, *p_dtm, sepch, *p_tp;

  struct typ_dec             m_tpd;
  double                     f;
  struct dsh_str             d_sh;

  /* region number, prefix and name */
  char                       rgn_name[24];
  union rgn_un               rgn_pref, dum_pref;

  /* save lab_info */
  (void) strcpy(t_lab, lab_info);

  conv_mode = (!strcmp(t_lab,  "input")) ? 1 :
              (!strcmp(t_lab, "output")) ? 3 :
              (!strcmp(t_lab, "OUTPUT")) ? 4 :
              (!strcmp(t_lab, "SURVEY")) ? 5 : 0;

  va_start(o_p, h_lab);
  if (conv_mode) iofile = va_arg(o_p, FILE *);
  // NO additional params
  //else           io_str = va_arg(o_p, char *);
  va_end(o_p);

  if (p_lb_ex->lab_type != HGT_LAB) {
    lord_error(0,LORD("conv_hgt called with wrong lab_type %d"), p_lb_ex->lab_type);
    h_lab->lab_type = ILL_LAB;
    return(ILL_LAB);
  }

  (void) strcpy(dum_pref.prfx, "ZZ");
  p_dtm  = (p_lb_ex->mlb2);  /* datum */

  switch(conv_mode) {
  case 0: /* char input */
  case 1: /* file input */

    p_sys = (p_lb_ex->name);  /* name is mlb1 */
    p_dtm = (p_lb_ex->mlb2);  /* datum */

	/* clean lab */
    p_tp = (char *) h_lab;
    for (i = sizeof (*h_lab) - 1; i >= 0; i--) *(p_tp + i) = 0;

    /* label_def_str contain sepch (not sepix) */
    sepch           = (char) p_lb_ex->sepch;
    h_lab->lab_type = p_lb_ex->lab_type;
    h_lab->version  = LAB_VERSION;
    h_lab->cmplt    = (short) ((p_lb_ex->q_par) ? 0 : 1);
    h_lab->cstm     = p_lb_ex->d_kind;
    h_lab->mode     = p_lb_ex->d_type;
    h_lab->region   = p_lb_ex->region;
    h_lab->p_rgn    = p_lb_ex->lab_rgn;
    h_lab->ncoord   = 1;
    h_lab->scale    = 1.0;  /* default */
    h_lab->imit     = 0;    /* default */

    if (sepch != '\0') {
      if (sepch == 'L') {
        (void) strcpy(p_sys, "h");
        (void) strcpy(p_lb_ex->mlb2, "msl");
      } else
      if (sepch != '_') {
        lord_error(0,LORD("illegal separator %c"), sepch);
        h_lab->lab_type = ILL_LAB;
        return(ILL_LAB);
      }
    }

    /* set params of the height systems */
    /*_________________________________*/

    /* PRODUCE THE MINI-LAB  from : mlb1, sepch, dtm */

    /* The minilabel of regular systems with imittance mask of  */
    /* zero contains a concatenation of the coord_system, the   */
    /* separator and the datum. A non-zero imittance mask means */
    /* that only the coord_system is used. A separator != '_'   */
    /* is added to the minilabel, but no datum is given         */

    h_lab->sepix = (short) strlen(p_sys);
    if (*p_lb_ex->pr_dtm == '\0') {
      (void) sprintf(h_lab->mlb, "%s_%s", p_sys, p_dtm);
    }
    else {
      (void) strcpy(h_lab->mlb, p_sys);
      p_dtm = p_lb_ex->pr_dtm;
    }

    /* INPUT OF ADDITIONAL PARAMETERS */
    /*             NONE               */

    /* Set datum */
    h_lab->datum = (short) set_dtm_1(-1, p_dtm, &par_dtm, p_name,
                   e_name, rgn_pref.prfx, &mask, &(h_lab->dsh_con),DEF_DATA);
    if (h_lab->datum > 0)
      h_lab->ellipsoid = (short) set_grs(-1, e_name, ell_p);
    else {
      h_lab->ellipsoid = (short) set_grs(-1, t_lab, ell_p);
      h_lab->datum     = -2;
    }

    if (h_lab->ellipsoid >= 0) {
      /* ellipsoid params */
      h_lab->a      = *(ell_p+0);
      h_lab->f      = *(ell_p+1);
      /* datum shift params (from set_dtm) */
      h_lab->p_dtm  = (short) par_dtm;
      h_lab->p_rgn  = (dum_pref.r_nr[0] != rgn_pref.r_nr[0])
                    ?  rgn_pref.r_nr[0] : 0;
      h_lab->imit   = mask;
      if (h_lab->region == 0) h_lab->region = h_lab->p_rgn;
      /* set trf. constants for completed systems */
      /* if (h_lab->cmplt == 1)                   */
      /*    (void) set_trc((struct coord_lab *) h_lab); */
    }
    else h_lab->lab_type = ILL_LAB; /* unintelligible ellipsoid */

    h_lab->ch_sum = labchsum((union geo_lab *) h_lab, &h_lab->ch_sum);
    break;  /* conv_mode 0 & 1 */

  case 3:  /* output of minilabel, simple */
  case 4:  /* output of minilabel, expand */

    if (conv_rgn(h_lab->region, rgn_pref.prfx, rgn_name) > 0)
      (void) fprintf(iofile, "%s_", rgn_pref.prfx);
    (void) fprintf(iofile, "%s", h_lab->mlb);

    if (conv_mode == 4) {
      if (h_lab->cstm == 33 && h_lab->mode == 9)
         (void) fprintf(iofile, "    * time series in Data Base ;");
      (void) fprintf(iofile, "\n*\n");

      /* collect text of the height kind */
      if (32 < h_lab->cstm && h_lab->cstm < 39)
          (void) fprintf(iofile,
          "\nHeights:            %s", p_lb_ex->text);
      if (h_lab->datum == 201)
          (void) fprintf(iofile, "\nLocal Reference Level");
      else
      if (h_lab->datum == 207)
          (void) fprintf(iofile,
          "\nMIDLERTIDIGT REGNENIVEAU : BENYT DVR90");

      /* Region info */
      if (h_lab->region != 0 &&
          (conv_rgn(h_lab->region, rgn_pref.prfx, rgn_name) > 0)) {
        (void) fprintf(iofile, "\nRegion:%-6s   %6s -> %-15s",
                       " ", rgn_pref.prfx, rgn_name);
      }
      /* Ellipsoid, datum, and parent datum */
      if (set_grs(h_lab->ellipsoid, e_name, ell_p) > 0) {
        (void) fprintf(iofile, "\nEllipsoid: %9s%s", " ", e_name);
        (void) fprintf(iofile,
                    "\n  a       =        %14.4f m", h_lab->a);
        f = h_lab->f;
        if (0.0 < f && f < 1.0)
          (void) fprintf(iofile, "\n  1/f     =       %12.5f", 1 / f);
        else
          (void) fprintf(iofile, "\n  f       =       %12.5f", f);
      }

      if (h_lab->datum > 0) {
        if (set_dtm_1(h_lab->datum, p_dtm, &par_dtm, p_name,
          e_name, rgn_pref.prfx, &mask, &d_sh,DEF_DATA) > 0) {
          (void) fprintf(iofile, "\nDatum:     %9s%s", " ", p_dtm);
        }
      }
      if (h_lab->p_rgn != 0 &&
          (conv_rgn(h_lab->p_rgn, rgn_pref.prfx, rgn_name) > 0)) {
        (void) fprintf(iofile, "\nProj.Rgn:%-6s   %6s -> %-15s",
                       " ", rgn_pref.prfx, rgn_name);
      }

      (void) fprintf(iofile, "\n;\n");

    } /* end conv_mode == 4 */

    break; /* end case 3 & 4, output label */

  case 5:   /* DOCUMENTATION OF THE LABEL */

    m_tpd = *set_tpd("m ", 10, 6);

    (void) fprintf(iofile, "\n\f     HEIGHT-LABEL:\n # ");
    if (conv_rgn(h_lab->region, rgn_pref.prfx, rgn_name) > 0
        && h_lab->imit == 0L)
       (void) fprintf(iofile, "%s_", rgn_pref.prfx);

    (void) fprintf(iofile, "%s", h_lab->mlb);
    if (h_lab->cstm == 33 && h_lab->mode == 9)
       (void) fprintf(iofile, "    * time series in Data Base ;");
    if (h_lab->mode == 8)
       (void) fprintf(iofile, "    * with conditions in Group ;");
    if (h_lab->datum == 201)
        (void) fprintf(iofile, "\nLocal Reference Level");
    else
    if (h_lab->datum == 207)
        (void) fprintf(iofile,
        "\nMIDLERTIDIGT REGNENIVEAU : BENYT DVR90");

    sepch = *(h_lab->mlb + h_lab->sepix);
    if (sepch == '\0') sepch = ' ';
    /* General info */
    (void) fprintf(iofile, "\nlng       = %15ld", (long) sizeof (*h_lab));
    (void) fprintf(iofile, "\nlab_type  = %15d", h_lab->lab_type);
    (void) fprintf(iofile,
                   "         version   = %15d", h_lab->version);
    (void) fprintf(iofile, "\nmlb       = %15s", h_lab->mlb);
    (void) fprintf(iofile, "         sepch     = %15c", sepch);
    (void) fprintf(iofile, "\ninit      = %15d", h_lab->init);
    (void) fprintf(iofile, "         compl     = %15d", h_lab->cmplt);
    (void) fprintf(iofile, "\nncoord    = %15d", h_lab->ncoord);
    (void) fprintf(iofile, "         p_seq     = %15d", h_lab->p_seq);
    /* Region info */
    (void) fprintf(iofile, "\nregion    = %15d", h_lab->region);
    if (h_lab->region != 0) {
      if (conv_rgn(h_lab->region, rgn_pref.prfx, rgn_name) > 0)
         (void) fprintf(iofile, "%9s%-6s    =     %-15s",
                        " ", rgn_pref.prfx, rgn_name);
    }
    /* Ellipsoid, datum, and parent datum */
    (void) fprintf(iofile, "\nellipsoid = %15d", h_lab->ellipsoid);
    if (set_grs(h_lab->ellipsoid, e_name, ell_p) > 0)
        (void) fprintf(iofile, "%9s%s", " ", e_name);
    (void) fprintf(iofile, "\ndatum     = %15d", h_lab->datum);
    /* get the datum name */
    if (h_lab->datum > 0) {
      if (set_dtm_1(h_lab->datum, p_dtm, &par_dtm,
          p_name, e_name, rgn_pref.prfx, &mask, &d_sh,DEF_DATA) > 0) {
        (void) fprintf(iofile, "%9s%s", " ", p_dtm);
        /* get the parent datum name */
        if (set_dtm_1(h_lab->p_dtm, p_dtm, &par_dtm,
            p_name, e_name, rgn_pref.prfx, &mask, &d_sh,DEF_DATA) > 0) {
          (void) fprintf(iofile, "\npar. dtm  = %15d", h_lab->p_dtm);
          (void) fprintf(iofile, "%9s%s", " ", p_dtm);
        }
      }
    }
    if (h_lab->p_rgn != 0 &&
        (conv_rgn(h_lab->p_rgn, rgn_pref.prfx, rgn_name) > 0)) {
      (void) fprintf(iofile, "\nProj.Rgn  = %6s -> %-15s",
                     rgn_pref.prfx, rgn_name);
    }

    /* Enumeration of coord system */
    (void) fprintf(iofile, "\ncstm      = %15d", h_lab->cstm);
    (void) fprintf(iofile, "         mode      = %15d", h_lab->mode);
    /* NOT USED
    (void) fprintf(iofile, "\nS_crd     = %15d", c_lab->S_crd);
    (void) fprintf(iofile, "         W_crd     = %15d", c_lab->W_crd);
    NOT USED */

    /* Ellipsoidal parameters */
    for (i = sizeof (t_lab_a) - 1; i >= 0; i--) *(t_lab_a + i) = '\0';
    (void) sputg(t_lab_a, h_lab->a, &m_tpd, "g ");
    (void) fprintf(iofile, "\na         = %s", t_lab_a);
    f = h_lab->f;
    if (0.0 < f && f < 1.0)
      (void) fprintf(iofile, "\n1/f       = %21.14f", 1 / f);
    else
      (void) fprintf(iofile, "\nf         = %21.14f", f);
    (void) fprintf(iofile, "    (n       = %15.12e)", f / (2.0 - f));
    (void) fprintf(iofile, "\n (e2      = %21.12e)", f * (2.0 - f));
    (void) fprintf(iofile, "   (eprim2  = %15.12e)",
                   f * (2.0 - f) / (1.0 - f) / (1.0 - f));

    /* Origo and scale  */
    /* NOT USED
    (void) sputg(t_lab, h_lab->B0, &g_tpd, "g ");
    (void) fprintf(iofile, "\nB0        = %s", t_lab);
    (void) sputg(t_lab, h_lab->N0, &m_tpd, "g ");
    (void) fprintf(iofile, "\nN0        = %s", t_lab);
    (void) sputg(t_lab, h_lab->L0, &g_tpd, "g ");
    (void) fprintf(iofile, "\nL0        = %s", t_lab);
    (void) sputg(t_lab, h_lab->E0, &m_tpd, "g ");
    (void) fprintf(iofile, "\nE0        = %s", t_lab);
    (void) fprintf(iofile, "\nscale     = %21.12f", h_lab->scale);
    NOT USED */

    /* Additional parameters, tolerance, and spec. region */
    /* NOT USED
    (void) sputg(t_lab, h_lab->B1, &g_tpd, "g ");
    (void) fprintf(iofile, "\nB1        = %s", t_lab);
    (void) sputg(t_lab, h_lab->B2, &g_tpd, "g ");
    (void) fprintf(iofile, "\nB2        = %s", t_lab);
    (void) fprintf(iofile, "\ntol       =      %15.0f my",
                   h_lab->tol * 1.0e6);
    (void) fprintf(iofile, "\nimit      =  %15d", h_lab->imit);
    NOT USED */

    /* Meridian arc, polar radii, cone const. etc. */
    /* NOT USED
    (void) sputg(t_lab, h_lab->Qn, &m_tpd, "g ");
    (void) fprintf(iofile, "\nQn        = %15s", t_lab);
    (void) sputg(t_lab, h_lab->Zb, &m_tpd, "g ");
    (void) fprintf(iofile, "\nZb        = %s", w_sys);
    NOT USED */

    /* Gaussian latitude coeff. */
    /* NOT USED
    p = h_lab->tcgg;
    (void) fprintf(iofile, "\n\n        Gauss -> Geogr.");
    (void) fprintf(iofile, "                  Geogr. -> Gauss");
    for (i = 0; i < 4; i++) {
      (void) fprintf(iofile, "\n  %3d   %+22.12e", i, *(p + i));
      (void) fprintf(iofile, "      %3d   %+22.12e",
                     i + 4, *(p + i + 4));
    }
    NOT USED */

    /* UTM coefficients */
    /* NOT USED
    p = h_lab->utg;
    (void) fprintf(iofile, "\n\n        UTM -> Geogr.");
    (void) fprintf(iofile, "                    Geogr. -> UTM");
    for (i = 0; i < 4; i++) {
      (void) fprintf(iofile, "\n  %3d   %+22.12e", i, *(p + i));
      (void) fprintf(iofile, "      %3d   %+22.12e", i, *(p + i + 4));
    }
    NOT USED */

    /* Datum shift params */
    /* NOT USED
    d_s = &(h_lab->dsh_con);
    (void) fprintf(iofile, "\n\n   Datum shift parameters");
    (void) fprintf(iofile, "\n    type   %4d", d_s->tp);
    (void) fprintf(iofile, "\n    tx     %+8.3f m", d_s->tx);
    (void) fprintf(iofile, "\n    ty     %+8.3f m", d_s->ty);
    (void) fprintf(iofile, "\n    tz     %+8.3f m", d_s->tz);
    (void) fprintf(iofile, "\n    dm     %+7.6e", d_s->scale - 1.0);
    (void) fprintf(iofile, "\n  ROTATION-MATRIX - DIAGONAL UNIT M.");
    (void) fprintf(iofile, "\n    %+9.6e  %+9.6e  %+9.6e",
                   d_s->r11 - 1.0, d_s->r12, d_s->r13);
    (void) fprintf(iofile, "\n    %+9.6e  %+9.6e  %+9.6e",
                   d_s->r21, d_s->r22 - 1.0, d_s->r23);
    (void) fprintf(iofile, "\n    %+9.6e  %+9.6e  %+9.6e",
                   d_s->r31, d_s->r32, d_s->r33 - 1.0);
    NOT USED */
    (void) fprintf(iofile, "\n\n%s slut\n", h_lab->mlb);

    break;   /* end case 5, documentation */
  } /* end switch (conv_mode) */

  return(h_lab->lab_type);
}
