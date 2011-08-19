

/* tabdd_val  ver 2006.01           # page 1   25 Jun 2006 11 37 */


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

int                       tabdd_val(
/*______________________________*/
union  geo_lab           *c_lab_u,
struct mtabdd_str        *tabdd_table,
double                    B_89,
double                    L_89,
double                   *val,
char                     *err_str
)
{

#include        "s_status.h"
#include        "v_std.h"

  struct gde_lab             *t_lab;
  union  tdd_pages           *g_pg = &(tabdd_table->g_pg[0]), *g_pg_i;
  double                     *pdgh = (double *) NULL;

  /* tabdd pages */
  int                        *ppsg  = &(tabdd_table->sgd[0]);
  long                       *ppge  = &(tabdd_table->pge[0]);

  /* Pageing info */
  int                        *xmt    = &(tabdd_table->xmt);
  int                        *victim = &(tabdd_table->victim);

  /* Labels and descriptors */
  struct coord_lab           *crd_lab = &(c_lab_u->u_c_lab);

  int                         nw_idx, idx = 0;
  long                        nw_pos, pos = 0;
  int                         n_i, e_i, geo;
  double                      N_frac, E_frac, NE_frac, N_i, E_i;
  double                      L_p;

  /* enumeration of vertices : nw = 0, ne = 1, sw = 2, se = 3 */

  /* tabdd values at vertices */
  double                      tval[8];
  int                         s, g, i, t, k, kix;
  int                         miss, qr, qr1;


/* tabdd_val  ver 2006.01           # page 2   25 Jun 2006 11 37 */


  *val = *(val +1) = 99999999.0;

  /* test initialize */
  if (tabdd_table->init == 0) 
     return(s_status(err_str, "tabdd_val", TRF_PROGR_));

  geo = (crd_lab->cstm == 2) ? 1 : 0;

  /* select the tabdd */
  t_lab    = &(tabdd_table->table_u);
  miss     = 1;
  g        = 0;

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
      return(s_status(err_str, "grid_val", TAB_C_PRJ_));
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
    qr1    = (int)  (((e_i)/t_lab->rec_p_bl)* t_lab->blk_size);
    nw_pos = (long) qr + (long) qr1;
    nw_idx = (e_i % t_lab->rec_p_bl);


/* tabdd_val  ver 2006.01           # page 3   25 Jun 2006 11 37 */


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


/* tabdd_val  ver 2006.01           # page 4   25 Jun 2006 11 37 */


        for (s = -1, i = 0; i < GEOIDPAGES; i++) {
          if (/* g == *(ppsg+i) && */ pos == *(ppge+i)) {
            s      = i;
            g_pg_i = g_pg + i;
            pdgh   = &(g_pg_i->fghtab[0]);
            ++ (tabdd_table->hit);
            break;
          }
        }

        /* not found */
        if (s == -1) {
          i         = *victim = (++(*victim)) % GEOIDPAGES;
          *(ppsg+i) = g;
          *(ppge+i) = pos;
          g_pg_i    = g_pg + i;
          pdgh      = &(g_pg_i->fghtab[0]);

          s = fseek(t_lab->fd, pos, SEEK_SET);
          if (s) {
            (void) fprintf(stdout,
                "\n*** tabdd_val: ERROR fseek tabdd %s", t_lab->mlb);
            (void) fprintf(stdout,
                " blokaddr %8ld ;", pos);
            return(TAB_N_SYS_);
          }

          /* tabdd_valab of doubles */
          qr = (int) (fread((char *) pdgh, t_lab->blk_size, 1, t_lab->fd)- 1);
          if (qr) {
            (void) fprintf(stdout,
                "\n*** tabdd_val: ERROR input tabdd %s", t_lab->mlb);
            (void) fprintf(stdout,
                "\n blokaddr %8ld number = %4d size = %4d ;",
                pos, qr, t_lab->blk_size);
            return(TAB_N_SYS_);
          }
          ++ (tabdd_table->agn);
        } /* end not found actions */


/* tabdd_val  ver 2006.01           # page 5   25 Jun 2006 11 37 */


        /* Table of double */
        k          = 2 * t;
        kix        = 2 * idx;
        tval[k]    = *(pdgh + kix);
        tval[k +1] = *(pdgh + kix +1);
        if (tval[k] > 99999998.0 || tval[k +1] > 99999998.0) miss = 1;

        if (miss > 0) {
          ++(*xmt);
          t = 4;
        }
      } /* end t-loop */

      if (miss == 0) {
        NE_frac   = N_frac * (1.0 - E_frac);
        *val      = tval[0]
                  + NE_frac * (tval[4] - tval[0])
                  + E_frac  * (tval[2] - tval[0]
                  + N_frac  * (tval[6] - tval[2]));
        /* == tval[0] *(1.0 - N_frac) * (1.0 - E_frac)
            + tval[2] *(1.0 - N_frac) *        E_frac
            + tval[4] *       N_frac  * (1.0 - E_frac)
            + tval[6] *       N_frac  *        E_frac; */
        (t_lab->used)++;
        *(val +1) = tval[1]
                  + NE_frac * (tval[5] - tval[1])
                  + E_frac  * (tval[3] - tval[1]
                  + N_frac  * (tval[7] - tval[3]));

        return(0);
      }

    } /* end inside test */

    else
      return(TAB_C_ARE_);

  } /* end inside selected tabdd */

  return(TAB_C_ARE_);
}

