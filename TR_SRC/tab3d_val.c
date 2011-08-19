

/* tab3d_val  ver 2004.01           # page 1   25 Jun 2004 11 37 */


/* Copyright (c) 2004 Kort & Matrikelstyrelsen, Denmark   */
/* and Danish National Space Center                       */
/* All rights reserved.                                   */

/* This is unpublished proprietary source code of Kort &         */
/* Matrikelstyrelsen, Denmark, and Danish National Space Center. */
/* This copyright claim does not indicate an intention of        */
/* publishing this code.                                         */

#include        <stdio.h>
#include        <string.h>

#include        "geo_lab.h"
#include        "geoid_d.h"

int                       tab3d_val(
/*________________________________*/
union  geo_lab           *c_lab_u,
struct mtab3d_str        *tab3d_table,
double                    B_89,
double                    L_89,
double                   *val,
char                     *err_str
)
{

#include        "s_status.h"
#include        "v_std.h"
/* double N_b, E_b; */

  struct gde_lab             *t_lab;
  union  t3d_pages           *g_pg = &(tab3d_table->g_pg[0]), *g_pg_i;
  float                      *pfgh = (float *) NULL;

  /* Tab3d pages */
  int                        *ppsg  = &(tab3d_table->sgd[0]);
  long                       *ppge  = &(tab3d_table->pge[0]);

  /* Pageing info */
  int                        *xmt    = &(tab3d_table->xmt);
  int                        *victim = &(tab3d_table->victim);

  /* Labels and descriptors */
  struct coord_lab           *crd_lab = &(c_lab_u->u_c_lab);

  int                         nw_idx, idx = 0;
  long                        nw_pos, pos = 0;
  int                         n_i, e_i, geo;
  double                      N_frac, E_frac, NE_frac, N_i, E_i;
  double                      L_p;

  /* enumeration of vertices : nw = 0, ne = 1, sw = 2, se = 3 */

  /* tab3d height at vertices */
  double                      tval[12];
  int                         s, g, i, t, k, kix;
  int                         miss, qr, qr1, tab3d;
  int                         dim3d, dim3d2, dim3d3;


/* tab3d_val  ver 2004.01           # page 2   25 Jun 2004 11 37 */


  *val = *(val +1) = 9998.0;

  /* test initialize */
  if (tab3d_table->init == 0) 
     return(s_status(err_str, "tab3d_val", TRF_PROGR_));

  geo = (crd_lab->cstm == 2) ? 1 : 0;

  /* select the tab3d */
  t_lab    = &(tab3d_table->table_u);
  tab3d    = (t_lab->lab_type == T3D_LAB);
  dim3d    = (t_lab->lab_type == T3D_LAB) ? 3 : 2;
  dim3d2   = dim3d + dim3d;
  dim3d3   = dim3d + dim3d2;
  miss     = 1;
  g        = 0;
  if (tab3d) *(val +2) = 9998.0;

#ifdef    HOPSATEST
(void) printf("\nINPUT CRD  %+10.5f    %+10.5f", B_89*DOUT, L_89*DOUT);
(void) printf("\nBREDDE     %+10.5f    %+10.5f",
        t_lab->B_min*DOUT, t_lab->B_max*DOUT);
(void) printf("\nLÆNGDE     %+10.5f    %+10.5f\n",
        t_lab->L_min*DOUT, (t_lab->L_min + t_lab->deltaL)*DOUT);
#endif

  /* TEST datum */               /* !s34j / !s34s */
  if (  t_lab->datum < 128 || 129 <   t_lab->datum || 
      crd_lab->datum < 128 || 129 < crd_lab->datum) {
    if (strcmp(t_lab->clb, crd_lab->mlb)) {
      return(s_status(err_str, "tab3d_val", TAB_C_PRJ_));
    }
  }

  L_p = L_89 - t_lab->L_min;
  if (geo) L_p = v_std(L_p);
  if (t_lab->B_min <= B_89 && B_89 <= t_lab->B_max 
                           && L_p  <= t_lab->deltaL) {

    miss     = 0;

    /* Table argument coordinates */
    N_i      = (t_lab->B_max - B_89) / t_lab->dB;
    E_i      = (L_p                ) / t_lab->dL;

    /* Int and fractional parts of arguments */
    n_i      = (int) floor(N_i);
    if (n_i == t_lab->n_max - 1) --n_i;
    e_i      = (int) floor(E_i);
    if (e_i == t_lab->e_max - 1) --e_i;
    N_frac   = N_i - n_i;
    E_frac   = E_i - e_i;

    /* Coordinates of nw vertex
    N_b = t_lab->B_max - n_i * t_lab->dB;
    E_b = t_lab->L_min + e_i * t_lab->dL; */

    /* addressing of nw-corner block */
    qr     = (int) (n_i * t_lab->row_size + t_lab->nbase);
    qr1    = (int) (((e_i)/t_lab->rec_p_bl)* t_lab->blk_size);
    nw_pos = (long) qr + (long) qr1;
    nw_idx = (e_i % t_lab->rec_p_bl);



/* tab3d_val  ver 2004.01           # page 3   25 Jun 2004 11 37 */


    /* Interpolation loop */
    if (0 <= n_i && n_i < t_lab->n_max &&
        0 <= e_i && e_i < t_lab->e_max) {

      /* Vertex loop */
      for (t = 0; t < 4; t++) {

        switch(t) {
        case 0: /* NW-vertex */
          pos = nw_pos;
          idx = nw_idx;
          break;

        case 1: /* NE-vertex */
          /* pos = nw_pos; */
          ++ idx;
          if (idx == t_lab->rec_p_bl) {
            pos += (long) t_lab->blk_size;
            idx  = 0;
          }
          break;

        case 2: /* SW-vertex */
          pos = nw_pos + (long) t_lab->row_size;
          idx = nw_idx;
          break;

        case 3: /* SE-vertex */
          /* pos = nw_pos + (long) t_lab->row_size; */
          ++ idx;
          if (idx == t_lab->rec_p_bl) {
            pos += (long) t_lab->blk_size;
            idx  = 0;
          }
          break;
        } /* end t-switch ; */


/* tab3d_valver 2004.01           # page 4   25 Jun 2004 11 37 */


        for (s = -1, i = 0; i < GEOIDPAGES; i++) {
          if (/* g == *(ppsg+i) && */ pos == *(ppge+i)) {
            s      = i;
            g_pg_i = g_pg + i;
            pfgh   = &(g_pg_i->fghtab[0]);
            ++ (tab3d_table->hit);
            break;
          }
        }

        /* not found */
        if (s == -1) {
          i         = *victim = (++(*victim)) % GEOIDPAGES;
          *(ppsg+i) = g;
          *(ppge+i) = pos;
          g_pg_i    = g_pg + i;
          pfgh      = &(g_pg_i->fghtab[0]);

          if ((s=fseek(t_lab->fd, pos, SEEK_SET))) {
            (void) fprintf(stdout,
                "\n*** tab3d_val: ERROR fseek tab3de %s", t_lab->mlb);
            (void) fprintf(stdout,
                " blokaddr %8ld ;", pos);
            return(TAB_N_SYS_);
          }

          /* tab3d_val of floats */
          if ((qr = (int) fread((char *) pfgh, t_lab->blk_size,
               1, t_lab->fd))- 1) {
            (void) fprintf(stdout,
                "\n*** tab3d_val: ERROR input tab3de %s", t_lab->mlb);
            (void) fprintf(stdout,
                "\n blokaddr %8ld number = %4d size = %4ld ;",
                pos, qr, t_lab->blk_size);
            return(TAB_N_SYS_);
          }
          ++ (tab3d_table->agn);
        } /* end not found actions */


/* tab3d_valver 2004.01           # page 5   25 Jun 2004 11 37 */


        /* Table of floats */
        k          = dim3d * t;
        kix        = dim3d * idx;
        tval[k]    = (double) *(pfgh + kix);
        tval[k +1] = (double) *(pfgh + kix +1);
        if (tab3d) {
          tval[k +2] = (double) *(pfgh + kix +2);
          if (tval[k +2] > 9998.0) miss = 1;
        }
        if (tval[k] > 9998.0 || tval[k +1] > 9998.0) miss = 1;

        if (miss > 0) {
          ++(*xmt);
          t = 4;
        }
      } /* end t-loop */

      if (miss == 0) {
        NE_frac   = N_frac * (1.0 - E_frac);
        *val      = tval[0]
                  + NE_frac * (tval[dim3d2] - tval[0])
                  + E_frac  * (tval[dim3d] - tval[0]
                  + N_frac * (tval[dim3d3] - tval[dim3d]));
        /* == tval[0] *(1.0 - N_frac) * (1.0 - E_frac)
            + tval[3] *(1.0 - N_frac) *        E_frac
            + tval[6] *       N_frac  * (1.0 - E_frac)
            + tval[9] *       N_frac  *        E_frac; */
        (t_lab->used)++;
        *(val +1) = tval[1]
                  + NE_frac * (tval[dim3d2+1] - tval[1])
                  + E_frac  * (tval[dim3d+1] - tval[1]
                  + N_frac * (tval[dim3d3+1] - tval[dim3d+1]));
        if (tab3d) {
          *(val +2) = tval[2]
                    + NE_frac * (tval[8] - tval[2])
                    + E_frac  * (tval[5] - tval[2]
                    + N_frac * (tval[11] - tval[5]));
        }

        return(0);
      }

    } /* end inside test */

    else
      return(TAB_C_ARE_);

  } /* end inside selected tab3d */

  return(TAB_C_ARE_);
}

