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

/* conv_spc(name, p_lb, lab) service function for conv_lab     */
/* NB should be called only by conv_lab                        */
/*    result ==  0 : no lab                                    */
/*    result ==  LAB_TYPE :                                    */
/*    UNDEFSYS     : unrecognized name                         */
/* an unrecognized name is considered a filename containing a  */
/* genuine label                                               */

#include    <stdio.h>
#include    <string.h>
#include    <stdarg.h>
#include    <sys/stat.h>

#include    "KmsFncs.h"

int                      conv_spc(
/*______________________________*/
char                    *lab_info,
struct lab_def_str      *p_lb,
union  geo_lab          *u_lab,
...
)
{

  FILE                      *iofile = (FILE *) NULL;
  va_list                    o_p;
  struct idt_lab            *i_lab = &(u_lab->u_i_lab);
  struct poc_lab            *p_lab = &(u_lab->u_p_lab);
  struct wor_lab            *w_lab = &(u_lab->u_w_lab);
  struct wrh_lab            *h_lab = &(w_lab->wh_lab);
  struct coord_lab          *c_lab = &(w_lab->wc_lab);
  union  geo_lab            wk1_lab, *wrk_lab = &wk1_lab;
  int                        conv_mode, i, used;
  int                        crd_type, t_char, iq;
  double                     r_wrk;
  char                       t_lab[2*MLBLNG], in_str[256], w_str[64];
  char                       sepch;
  char                       p_dtm_a[MLBLNG], *p_dtm = p_dtm_a;
  char                       p_sys_a[MLBLNG], *p_sys = p_sys_a;
  char                      *p_tp;

  struct typ_dec             m_tpd;

  /* region prefix and name */
  char                      rgn_pref[8], rgn_name[24];

  /* save lab_info */
  (void) strcpy(t_lab, lab_info);

  conv_mode = (!strcmp(t_lab,  "input")) ? 1 :
              (!strcmp(t_lab, "output")) ? 3 :
              (!strcmp(t_lab, "OUTPUT")) ? 4 :
              (!strcmp(t_lab, "SURVEY")) ? 5 : 0;

  va_start(o_p, u_lab);
  if (conv_mode) iofile = va_arg(o_p, FILE *);
  va_end(o_p);

  switch (conv_mode) {
  case 0: /* char input */
    lord_error(0,LORD("conv_spc may take input from files only"));
    h_lab->lab_type = ILL_LAB;
    return(ILL_LAB);

  case 1: /* file input */

    if (p_lb->lab_type != WOR_LAB && p_lb->lab_type != POC_LAB) {
      lord_error(0,LORD("conv_spc called with wrong lab_type %d"),
             p_lb->lab_type);
      h_lab->lab_type = ILL_LAB;
      return(ILL_LAB);
    }

    sepch   = (char) p_lb->sepch;
    (void) strcpy(p_sys, p_lb->name);  /* name is mlb1 */
    if (*p_lb->pr_dtm == '\0') (void) strcpy(p_dtm, p_lb->mlb2);
    else *p_dtm = '\0';

    /* clean lab */
    p_tp = (char *) w_lab;
    for (i = sizeof (*w_lab) - 1; i >= 0; i--) *(p_tp + i) = 0;

    h_lab->lab_type = p_lb->lab_type;
    h_lab->version  = LAB_VERSION;
    h_lab->region   = p_lb->region;

    /* get coord_sys */
    switch(h_lab->lab_type) {
    case WOR_LAB:
      crd_type = conv_lab("in_u_#", wrk_lab, iofile);

      if ((crd_type == CRD_LAB || crd_type == HGT_LAB)
          && ((wrk_lab->u_c_lab).cmplt == 1)) {
        if (!(wrk_lab->u_c_lab).region)
           (wrk_lab->u_c_lab).region = h_lab->region;
        else           h_lab->region = (wrk_lab->u_c_lab).region;
        (wrk_lab->u_c_lab).ch_sum =
                  labchsum(wrk_lab, &((wrk_lab->u_c_lab).ch_sum));

        /* set params of the wor result */
        /*______________________________*/
        *c_lab = wrk_lab->u_c_lab;
        (void) strcpy(h_lab->c_mlb, c_lab->mlb);

        /* you can't use f_nxt with sgetcw FS AUG 20105      */
        /* so work with fgetln_kms until adj_date has been read */
        do {
          p_tp = in_str;
          iq   = fgetln_kms(in_str, &i, iofile);
        } while (iq < (/*int) (p_tp -in_str*/ +2));

        (void) sscanf(p_tp, "%s%n", w_str, &used);

        if (!strcmp(w_str, "beregningsdato")) {
          p_tp           += used;
          h_lab->adj_date = sgetcw(p_tp, &used, &t_char);
        }

        /* now continue with f_nxt, init for f_nxt */
        used = i = 0; 
        (void) sscanf(f_nxt(in_str, &i, iofile, used),
                      "%s%n", w_str, &used);

        if (!strcmp(w_str, "prodnr")) 
           r_wrk = sgetg(f_nxt(in_str, &i, iofile, used),
                         &m_tpd, &used, "m");
        else
           r_wrk = sgetg(in_str, &m_tpd, &used, "m");
        h_lab->prodnr = (int) r_wrk;
        (void) sscanf(f_nxt(in_str, &i, iofile, used),
                      "%s%n", w_str, &used);
        if (!strcmp(w_str, "bnorm")) 
           r_wrk = sgetg(f_nxt(in_str, &i, iofile, used),
                         &m_tpd, &used, "m");
        h_lab->bnorm = (short) r_wrk;
        h_lab->cmplt = 2;
      } else {
        lord_error(0,LORD("conv_spc: %s Unknown coordsys/kotesys %s"),
               h_lab->mlb, c_lab->mlb);
        return(ILL_LAB);
      }

      break; /* end WOR_LAB */

    case POC_LAB:
      c_lab    = &(wrk_lab->u_c_lab);
      crd_type = conv_lab("in_u_#", wrk_lab, iofile);
      if (crd_type == CRD_LAB && c_lab->cmplt == 1) {
        (void) strcpy(p_lab->c_mlb, c_lab->mlb);
        p_lab->cmplt = 2;
      } else {
        lord_error(0,LORD("conv_spc: %s Unknown coordsys %s"),
               p_lab->mlb, c_lab->mlb);
      }
      break; /* end POC_LAB */
    }

    /* PRODUCE THE MINI-LAB  from : mlb1, sepch, dtm */
    (void) strcpy(h_lab->mlb, p_sys);
    if (*p_lb->pr_dtm != '\0' && sepch == '_') sepch = '\0';
    else {
      *(t_lab + 0) = sepch;
      *(t_lab + 1) = '\0';
      (void) strcat(h_lab->mlb, t_lab);
    }
    if (!*p_lb->pr_dtm == '\0') (void) strcat(h_lab->mlb, p_dtm);
    h_lab->sepix = (short) strlen(p_sys);
    if (h_lab->lab_type == WOR_LAB)
       h_lab->ch_sum = labchsum(u_lab, &h_lab->ch_sum);
    break; /* end conv_mode == 1 */

  case 3:  /* output of minilabel, simple */
  case 4:  /* output of minilabel, expand */

    if (conv_rgn(h_lab->region, rgn_pref, rgn_name) > 0)
      (void) fprintf(iofile, "%s_", rgn_pref);
    (void) fprintf(iofile, "%s", h_lab->mlb);

    switch (h_lab->lab_type) {
    case WOR_LAB :
      if (h_lab->cmplt == 2) {
        (void) fprintf(iofile, "   %s\n", h_lab->c_mlb);
        (void) fprintf(iofile, "beregningsdato  ");
        (void) fputcw(iofile, h_lab->adj_date, 0);
        (void) fprintf(iofile, "\nprodnr    %4d", h_lab->prodnr);
        (void) fprintf(iofile, "\nbnorm     %4d\n", h_lab->bnorm);
      }  /* complete lab */
      else {
        (void) fprintf(iofile, "   UNCOMPLETED +++\n");
        lord_error(0,LORD("conv_spc : %s UNCOMPLETED"), h_lab->mlb);
      }
      break; /* WOR_LAB */

    case POC_LAB :
      if (p_lab->cmplt == 2) {
        (void) fprintf(iofile, "   %s", h_lab->c_mlb);
      }  /* complete lab */
      else {
        (void) fprintf(iofile, "   UNCOMPLETED +++\n");
        lord_error(0,LORD("conv_spc : %s UNCOMPLETED"), h_lab->mlb);
      }
      break; /* WOR_LAB */

    case IDT_LAB :
    case JDF_LAB :
      break; /* IDT_LAB */

    default : /* unintelligible label */
      lord_error(0,LORD("conv_spc : %s not programmed"), h_lab->mlb);
    } /* end switch(lab_type) */

    break; /* end case 3 & 4 */

  case 5:   /* DOCUMENTATION OF THE LABEL */
    sepch = *(p_lab->mlb + p_lab->sepix);
    if (sepch == '\0') sepch = ' ';

    switch (h_lab->lab_type) {
    case WOR_LAB :
      (void) fprintf(iofile, "\n\f     WOR-LABEL:\n # ");
      if (conv_rgn(h_lab->region, rgn_pref, rgn_name) > 0)
        (void) fprintf(iofile, "%s_", rgn_pref);
      (void) fprintf(iofile, "%s", h_lab->mlb);
      if (h_lab->cmplt == 2) {
        (void) fprintf(iofile, "   %s\n", h_lab->c_mlb);
        (void) fprintf(iofile, "beregningsdato  ");
        (void) fputcw(iofile, h_lab->adj_date, 0);
        (void) fprintf(iofile, "\nprodnr    %4d", h_lab->prodnr);
        (void) fprintf(iofile, "\nbnorm     %4d\n", h_lab->bnorm);
      }  /* complete lab */
      else {
        (void) fprintf(iofile, "   UNCOMPLETED +++\n");
        lord_error(0,LORD("conv_spc : %s UNCOMPLETED"), h_lab->mlb);
      }

      /* General info */
      (void) fprintf(iofile, "\nlng       = %15ld", (long) sizeof (*w_lab));
      (void) fprintf(iofile, "\nlab_type  = %15d", h_lab->lab_type);
      (void) fprintf(iofile,
                     "         version   = %15d", h_lab->version);
      (void) fprintf(iofile, "\nmlb       = %15s", h_lab->mlb);
      (void) fprintf(iofile, "         sepch     = %15c", sepch);
      (void) fprintf(iofile, "\ncompl     = %15d", h_lab->cmplt);
      (void) fprintf(iofile, "         bnorm     = %15d", h_lab->bnorm);
      /* Region info */
      (void) fprintf(iofile, "\nregion    = %15d", h_lab->region);
      if (h_lab->region != 0) {
        if (conv_rgn(h_lab->region, rgn_pref, rgn_name) > 0)
           (void) fprintf(iofile, "%9s%-6s    =     %-15s",
                          " ", rgn_pref, rgn_name);
      }
      (void) fprintf(iofile, "\nc_mlb     = %15s", h_lab->c_mlb );
      (void) fprintf(iofile, "\n\nslut    %15s\n", h_lab->mlb );
      break; /* WOR_LAB */

    case POC_LAB :
      (void) fprintf(iofile, "\n\f     PHOTO_OC-LABEL:\n # ");
      if (conv_rgn(p_lab->region, rgn_pref, rgn_name) > 0)
        (void) fprintf(iofile, "%s_", rgn_pref);
      (void) fprintf(iofile, "%s", p_lab->mlb);
      if (p_lab->cmplt == 2)
        (void) fprintf(iofile, "   %s\n", p_lab->c_mlb);
      else {
        (void) fprintf(iofile, "  UNCOMPLETED +++\n");
        lord_error(0,LORD("conv_spc : %s UNCOMPLETED"), h_lab->mlb);
      }

      /* General info */
      (void) fprintf(iofile, "\nlng       = %15ld", (long) sizeof (*p_lab));
      (void) fprintf(iofile, "\nlab_type  = %15d", p_lab->lab_type);
      (void) fprintf(iofile,
                     "         version   = %15d", p_lab->version);
      (void) fprintf(iofile, "\nmlb       = %15s", p_lab->mlb);
      (void) fprintf(iofile, "         sepch     = %15c", sepch);
      (void) fprintf(iofile, "\ncompl     = %15d", p_lab->cmplt);
      /* Region info */
      (void) fprintf(iofile, "\nregion    = %15d", p_lab->region);
      if (p_lab->region != 0 &&
          conv_rgn(p_lab->region, rgn_pref, rgn_name) > 0)
            (void) fprintf(iofile, "%9s%-6s    =     %-15s",
                           " ", rgn_pref, rgn_name);
      (void) fprintf(iofile, "\n\nslut    %15s\n", p_lab->mlb );
      break; /* POC_LAB */

    case IDT_LAB :
    case JDF_LAB :
      if (i_lab->lab_type == IDT_LAB)
           (void) fprintf(iofile, "\n\f     IDENT-LABEL:\n # ");
      else (void) fprintf(iofile, "\n\f     JOBDEF-LABEL:\n # ");
      if (conv_rgn(i_lab->region, rgn_pref, rgn_name) > 0)
        (void) fprintf(iofile, "%s_", rgn_pref);
      (void) fprintf(iofile, "%s", i_lab->mlb);

      /* General info */
      (void) fprintf(iofile, "\nlng       = %15ld", (long) sizeof (*i_lab));
      (void) fprintf(iofile, "\nlab_type  = %15d", i_lab->lab_type);
      (void) fprintf(iofile,
                     "         version   = %15d", i_lab->version);
      /* Region info */
      (void) fprintf(iofile, "\nregion    = %15d", i_lab->region);
      if (i_lab->region != 0 &&
          conv_rgn(i_lab->region, rgn_pref, rgn_name) > 0)
             (void) fprintf(iofile, "%9s%-6s    =     %-15s",
                            " ", rgn_pref, rgn_name);
      (void) fprintf(iofile, "\n\nslut    %15s\n", i_lab->mlb );
      break; /* IDT_LAB */

    default : /* unintelligible label */
      lord_error(0,LORD("conv_spc : %s not programmed"), h_lab->mlb);
    } /* end switch(lab_type) */

    break; /* end case 5 */
  } /* end switch(conv_mode) */
  return(h_lab->lab_type);
}
