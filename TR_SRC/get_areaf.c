

/* get_areaf                      # page 1    2 Feb 2004 14 02 */


/* Copyright (c) 2004, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                    */

/* This is unpublished proprietary source code of Kort- og     */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include    <stdio.h>
#include    <stdlib.h>
#include    <math.h>
#include    <string.h>

#include   "geo_lab.h"
#include   "geoid_d.h"

#ifndef    M_PI
#include   "kms_math.h"
#endif

int              get_areaf(
/*_______________________*/
union geo_lab   *ilb_p,
double          *B,
double          *L,
int              max,
double          *tot_sum,
int             *used_max,
char            *wrk_path,
char            *usertxt,
FILE            *tr_error
)

{

#include            "conv_lab.h"
#include            "ptg.h"
#include            "set_dtm_1.h"
#include            "sftr.h"
#include            "gd_trans.h"
#include            "get_ha_s.h"
#include            "v_red.h"

  struct  ne          {
    double              n;
    double              e;
  }                   NE, NE0, *Z = &NE;

  union geo_lab       glb_a, *glb_p = &glb_a;
  union geo_lab       sflb_a, *sflb = &sflb_a;
  struct coord_lab   *icl = &(ilb_p->u_c_lab);
  struct coord_lab   *clb = &(glb_a.u_c_lab);
  struct coord_lab   *csb = &(sflb_a.u_c_lab);
  struct dsh_str      trc;
  struct mgde_str     geoid_table;
  FILE               *fw, *fw1, *fw2;


/* get_areaf                      # page 2    2 Feb 2004 14 02 */


  double              Bm = 0.0, Bmin = +4.0, Bmax = -4.0;
  double              Lm = 0.0, Lmin = +4.0, Lmax = -4.0;
  double              A0, A1, H, gh, n1, e1, n2, e2;
  double              Pm, P1, P2, s1, s2, s3, az, az1, az2;
  double              a, f, Smax, sum, g_sum = 0.0;
  int                 i, j, pkt, ctrf = 0, res = 0, trr = 0;
  short               si, polar = 0, mask;
  char                g_mini[MLBLNG*4], *d_name, *p_name, *e_name;
  char                sep;
  union rgn_un        rgn_pref;

  geoid_table.init = 0;
  d_name           = g_mini + MLBLNG;
  p_name           = d_name + MLBLNG;
  e_name           = p_name + MLBLNG;

  if (CRD_LAB != icl->lab_type) {
    (void) fprintf(tr_error,
        "\n** get_areaf: unintelligible coord label");
    res = 1;
    goto RES;
  }
  (void) fprintf(tr_error, "\nInput-system :   %s\n", icl->mlb);
  (void) fprintf(tr_error, "\nNo.  vertices:   %d\n", max);
  if (max <= 0) {
    (void) fprintf(tr_error, "\n** get_areaf: TOO FEW VERTICES");
    goto RES;
  }


#ifdef  WIN32
  sep = '\\';
#else
  sep = '/';
#endif
  i = (int) strlen(wrk_path) -1;
  if (*(wrk_path + i) == sep) *(wrk_path + i) = '\0';
  (void) sprintf(g_mini, "%s%cworkarea.geo", wrk_path, sep); 
  if ((fw = fopen(g_mini, "w+b")) ==  NULL) {
    (void) fprintf(tr_error, "\nwrkfile: %s ej ok", g_mini);
    res = 3;
    goto RES;
  }
  (void) sprintf(g_mini, "%s%cworkarea1.geo", wrk_path, sep); 
  if ((fw1 = fopen(g_mini, "w+b")) ==  NULL) {
    (void) fprintf(tr_error, "\nwrkfile: %s ej ok", g_mini);
    res = 3;
    goto RES;
  }

  /* collect input datum name */
  if (icl->datum !=
      set_dtm_1(icl->datum, d_name, &si,
                p_name, e_name, rgn_pref.prfx, &mask, &trc)) {
    (void) fprintf(tr_error,
                   "\n*** get_areaf: ukendt datum %s", d_name);
    res = 1;
    goto RES;
  }

  if (icl->cstm != 2
      || icl->imit == DKMASK || icl->imit == FEMASK) {

    /* collect parent datum name for non-regular cstm */
    if ((icl->cstm != 2)
        && (icl->imit == DKMASK || icl->imit == FEMASK)) {
      (void) set_dtm_1(-1, d_name, &si, p_name, e_name, rgn_pref.prfx, &mask, &trc);
      (void) strcpy(d_name, p_name);
    }


/* get_areaf                      # page 3    2 Feb 2004 14 02 */


    /* set geo_lab on the input datum */
    (void) strcpy(g_mini, "geo_");
    (void) strcat(g_mini, d_name);
    if (conv_lab(g_mini, glb_p) != CRD_LAB) {
      (void) fprintf(tr_error,
          "\n**unintelligible coord_lab %s", g_mini);
      res = 2;
      goto RES;
    }
    if (icl->cstm != 2) ctrf = 1;
  }
  else *clb = *icl;

  /* set crd_lab for lambert polar equivalent */
  (void) strcpy(g_mini, "auth_");
  (void) strcat(g_mini, d_name);
  if (conv_lab(g_mini, sflb) != CRD_LAB) {
    (void) fprintf(tr_error,
        "\n***GET_AREA: label %s ikke initialiseret\n", g_mini);
    goto RES;
  }

  a        = clb->a;
  f        = clb->f;
  NE0.n    = 0.0;
  NE0.e    = 0.0;
  sum      = A0 = A1 = 0.0;
  *tot_sum = M_PI * 0.60; /* 108 dg */

  for (i = 0; i < max; i ++) {
    if (ctrf)
      trr = gd_trans(ilb_p, &glb_a, *(B+i), *(L+i), 0.0, &Z->n, &Z->e,
                     &H, &gh, -1, &geoid_table, usertxt, tr_error);
    else {
      Z->n = *(B+i);
      Z->e = *(L+i);
    }
    if (fwrite((char *) Z, sizeof(NE), 1, fw1) - 1) {
      (void) fprintf(tr_error, "\n***problems at workarea1.geo;");
      res = 4;
      goto RES;
    }
    if (i > 0) sum += v_red(Z->e - NE0.e);
    else  NE0 = *Z;
    if (sum < A0) A0 = sum;
    if (sum > A1) A1 = sum;
    if (fabs(sum) > *tot_sum) polar = 1;
    if (Bmin > Z->n) Bmin = Z->n;
    else
    if (Bmax < Z->n) Bmax = Z->n;
    if (Lmin > Z->e) Lmin = Z->e;
    else
    if (Lmax < Z->e) Lmax = Z->e;
  }


/* get_areaf                      # page 4    2 Feb 2004 14 02 */


  if (trr) {
    res = 5;
    goto RES;
  }

  if (NE0.n == NE.n && NE0.e == NE.e) -- max;
  else {
    if (fwrite((char *) (&NE0), sizeof(NE0), 1, fw1) - 1) {
      (void) fprintf(tr_error, "\n***problems at workarea1.geo;");
      res = 4;
      goto RES;
    }
  }

  Bm = (Bmax + Bmin) * 0.5;
  Lm = (Lmax + Lmin) * 0.5;
  if (polar) {
    Bmax = M_PI - Bmax;
    Bm   = (Bmax + Bmin) * 0.5;
  }

  if (fseek(fw1, 0L, SEEK_SET)) {
    (void) fprintf(tr_error, "\n***problems at workarea1.geo;");
    res = 4;
    goto RES;
  }
  for (i= 0; i <= max; i++) {
    if (fread((char *) Z, sizeof(NE), 1, fw1) - 1) {
      (void) fprintf(tr_error, "\n***problems at workarea1.geo;");
      res = 4;
      goto RES;
    }
    Z->e -= Lm;
    if (fabs(Z->n) > M_PI_2 - 1.0e-9) Z->e = 0.0;
    if (fwrite((char *) Z, sizeof(NE), 1, fw) - 1) {
      (void) fprintf(tr_error, "\n***problems at workarea.geo;");
      res = 4;
      goto RES;
    }
  }

  /* ********************** */
  /* lambert polar authalic */
  /* ********************** */
  (void) fprintf(tr_error, "\n\nlambert polar authalic : %s", csb->mlb);
  (void) fprintf(tr_error, "\nusing geographical triangels\n");

  trr = ptg(sflb, -1, Bm, 0.0, &n1, &e1, "", tr_error);
  Pm  = M_PI_2 - n1;
  if (trr) {
    res = 5;
    goto RES;
  }

  pkt       =  max;
  Smax      =  0.0;
  g_sum     = -1.0e9;


/* get_areaf                      # page 5    2 Feb 2004 14 02 */


  do {
    if (Smax * csb->Zb < 11000) {
      if (fseek(fw, 0L, SEEK_SET) ||
          fread((char *) Z, sizeof(NE), 1, fw) - 1) {
        (void) fprintf(tr_error, "\n***problems at workarea.geo;");
        res = 4;
        goto RES;
      }
      trr  |= ptg(sflb, -1, Z->n, Z->e, &n1, &e1, "", tr_error);
      P1    = M_PI_2 - n1;
      s1    = sftr(e1, Pm, P1, &az, &az1);
      Smax  = *tot_sum = 0.0;
      for (i = 0; i < pkt; i++) {
        if (fread((char *) Z, sizeof(NE), 1, fw) - 1) {
          (void) fprintf(tr_error, "\n***problems at workarea.geo;");
          res = 4;
          goto RES;
        }
        trr     |= ptg(sflb, -1, Z->n, Z->e, &n2, &e2, "", tr_error);
        P2       = M_PI_2 - n2;
        s3       = sftr(e2-e1, P1, P2, &az, &az);
        s2       = sftr(e2, Pm, P2, &az, &az2);
        if (Smax < s3) Smax = s3;
        /* area of spherical triangle */
        sum      = asin(sin(0.5*s1) * sin(0.5*s2) * sin(az2-az1)
                 / cos(0.5*s3));
        *tot_sum += sum;
    
        n1  = n2;
        e1  = e2;
        P1  = P2;
        s1  = s2;
        az1 = az2;
      }
      j         = (fabs(*tot_sum - g_sum) > 2.0e-9*fabs(*tot_sum))
                && Smax * csb->Zb > 16.0;
      *used_max = pkt;
      g_sum     = *tot_sum;
    }
    else Smax *= 0.5; 

    if (j) {
      fw2 = fw1;
      fw1 = fw;
      fw  = fw2;
      (void) get_ha_s(fw1, fw, a, f, pkt, &pkt, 0.49*Smax*csb->Zb);
    } else
    if (Smax * csb->Zb < 16.0)
       (void) fprintf(tr_error, "\nBREAK Smax < 16.0 m");
    if (trr) {
      res = 5;
      j   = 0;
    }
  } while (j);
  *tot_sum *= 2.0e-6 * csb->Zb * csb->Zb;
RES:
  (void) fclose(fw);
  (void) fclose(fw1);
  (void) remove("workarea.geo");
  (void) remove("workare1.geo");
  return(res);
}

