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
#include    "KmsFncs.h"

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
          i         = (++(*victim)) % GEOIDPAGES;
          *victim   = i;
          *(ppsg+i) = g;
          *(ppge+i) = pos;
          g_pg_i    = g_pg + i;
          pfgh      = &(g_pg_i->fghtab[0]);

          if ((s=fseek(t_lab->fd, pos, SEEK_SET))) {
            (void) lord_error(0, LORD("ERROR fseek tab3de %s blokaddr %8ld ;"), t_lab->mlb, pos);        
		   return(TAB_N_SYS_);
          }

          /* tab3d_val of floats */
          if ((qr = (int) fread((char *) pfgh, t_lab->blk_size,
               1, t_lab->fd))- 1) {
            (void) lord_error(0, LORD("ERROR input tab3de %s blokaddr %8ld number = %4d size = %4ld ;"), t_lab->mlb, pos, qr, (long) t_lab->blk_size);        
		    return(TAB_N_SYS_);
          }
          ++ (tab3d_table->agn);
        } /* end not found actions */

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

