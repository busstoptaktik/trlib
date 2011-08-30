

/* grid_val                 # page 1   30 Oct 2001 11 37 */


/* Copyright (c) 1992 GD, U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>

#include        "geo_lab.h"
#include        "geoid_d.h"

int                       grid_val(
/*_______________________________*/
union  geo_lab           *c_lab_u,
struct mgde_str          *grid_table,
double                    N,
double                    E,
double                   *g_v,
char                     *err_str
)
{
#include        "geoid_i.h"
#include        "v_std.h"
#include        "s_status.h"

  struct gde_lab             *t_lab;
  union  tab_pages           *g_pg = &(grid_table->g_pg[0]), *g_pg_i;
  float                      *pfgh = (float *) NULL;
  int                        *pigh = (int *) NULL;
  double                     *pdgh = (double *) NULL;

  /* Geoid pages */
  int                        *ppsg  = &(grid_table->sgd[0]);
  long                       *ppge  = &(grid_table->pge[0]);

  /* Pageing info */
  int                        *xmt     = &(grid_table->xmt);
  int                        *victim  = &(grid_table->victim);
  int                        *tab_max = &(grid_table->tab_max); /* 1 */
  int                        *tab_nr  = &(grid_table->tab_nr);  /* 1 */


/* grid_val                 # page 2   30 Oct 2001 11 37 */


  /* Labels and descriptors */
  struct coord_lab           *crd_lab = &(c_lab_u->u_c_lab);

  int                         val_mode, nw_idx, idx = 0;
  long                        nw_pos, pos = 0;
  int                         n_i, e_i, geo;
  double                      N_frac, E_frac, N_i, E_i;
  double                      L_p, scale;

  /* enumeration of vertices : nw = 0, ne = 1, sw = 2, se = 3 */

  /* table value at vertices */
  double                      gh[4];
  int                         s, i, t, g, res = 0;
  int                         miss, qr, qr1;

  *g_v = 9999.9;

  /* initialize */
  if (grid_table->init == 0) 
     res = geoid_i("STD", GDE_LAB, grid_table, err_str);
  if (res < 0) return(res);

  /* select the table */
  t_lab = &(grid_table->table_u[0]);
  for (g = 0, miss = 1; g < *tab_max && miss; t_lab++, g++) {
/*
(void) fprintf(stdout, "\n*grid_val, t_lab[%d:%d] = %s;",
g, *tab_max, t_lab->mlb);
*/

    geo = (crd_lab->cstm == 2) ? 1 : 0;
    if (!geo && t_lab->cstm == 0) continue;
    if ( geo && t_lab->cstm != 0) continue;


/* grid_val                 # page 3   30 Oct 2001 11 37 */


    L_p = E - t_lab->L_min;
    if (geo) L_p = v_std(L_p);
    if (t_lab->B_min <= N && N <= t_lab->B_max 
                        && L_p <= t_lab->deltaL) {
      /* TEST datum */               /* !s34j / !s34s */
      if (  t_lab->datum < 128 || 129 <   t_lab->datum || 
          crd_lab->datum < 128 || 129 < crd_lab->datum) {
        if (t_lab->lab_type == GDE_LAB) {
          if (t_lab->datum == crd_lab->datum) i = 0;
          else
          /* NEED Backloop as EE WGS84 != euref89 || WGS84 != GR96  */
          /*              WGS84           EUREF89              GR96 */
          i = ((  t_lab->datum == 1 ||   t_lab->datum == 4 ||   t_lab->datum == 5)
            && (crd_lab->datum == 1 || crd_lab->datum == 4 || crd_lab->datum == 5))
            ? 0 : TAB_C_PRJ_;
        } else
        i = (strcmp(t_lab->clb, crd_lab->mlb)) ? TAB_C_PRJ_ : 0;
        if (i) return((t_lab->datum != 51 && crd_lab->datum != 51)
                  ? s_status(err_str, "grid_val", TAB_C_PRJ_)
                  : t_lab->datum);
      }

      val_mode = (abs(t_lab->rec_size) == 8) ? 3 :
                 (t_lab->rec_size > 0) ? 1 : 2;
      scale    = (t_lab->rec_size > 0) ? 0.0 : 0.000001;

      miss     = 0;
      *tab_nr  = g;

      /* Table argument coordinates */
      N_i      = (t_lab->B_max - N) / t_lab->dB;
      E_i      = (L_p             ) / t_lab->dL;

      /* Int and fractional parts of arguments */
      n_i      = (int) floor(N_i);
      if (n_i == t_lab->n_max - 1) --n_i;
      e_i      = (int) floor(E_i);
      if (e_i == t_lab->e_max - 1) --e_i;
      N_frac   = N_i - n_i;
      E_frac   = E_i - e_i;

      /* Coordinates of nw vertex
      N_b      = t_lab->B_max - n_i * t_lab->dB;
      E_b      = t_lab->L_min + e_i * t_lab->dL; */

      /* addressing of nw-corner block */
      qr       = n_i * ((int) t_lab->row_size) + t_lab->nbase;
      qr1      = ((e_i)/t_lab->rec_p_bl)* ((int) t_lab->blk_size);
      nw_pos   = (long) qr + (long) qr1;
      nw_idx   = e_i % t_lab->rec_p_bl;


/* grid_val                 # page 5   30 Oct 2001 11 37 */


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
            if (g == *(ppsg+i) && pos == *(ppge+i)) {
              s      = i;
              g_pg_i = g_pg + i;
              pfgh   = &(g_pg_i->fghtab[0]);
              pigh   = (int *) pfgh;
              ++ (grid_table->hit);
              break;
            }
          }


/* grid_val                 # page 6   30 Oct 2001 11 37 */


          /* not found */
          if (s == -1) {
            i         = *victim = (++(*victim)) % GEOIDPAGES;
            *(ppsg+i) = g;
            *(ppge+i) = pos;
            g_pg_i    = g_pg + i;
            pfgh      = &(g_pg_i->fghtab[0]);
            pigh      = (int *) pfgh;

            if (fseek(t_lab->fd, pos, SEEK_SET) ||
                fread((char *) pfgh, t_lab->blk_size, 1, t_lab->fd)- 1) 
                return(TAB_N_SYS_);
            ++ (grid_table->agn);
          } /* end not found actions */

          /* Table of floats */
          gh[t] = (val_mode == 1) ? (double) *(pfgh + idx)
                : (val_mode == 2) ? ((double) *(pigh + idx)) * scale
                : *(pdgh + idx);
          if (gh[t] > 9998.0) {
            t    = 4;
            miss = 1;
            ++ (*xmt);
          }
        } /* end t-loop */

        if (miss == 0) {
          /* gh[0:3] :: NW, NE, Sw, SE */
          *g_v =  gh[0] + N_frac * (1.0 - E_frac) * (gh[2] - gh[0])
               + E_frac * (gh[1] - gh[0] + N_frac * (gh[3] - gh[1]));
          /* == gh[0] *(1.0 - N_frac) * (1.0 - E_frac)
              + gh[1] *(1.0 - N_frac) *        E_frac
              + gh[2] *       N_frac  * (1.0 - E_frac)
              + gh[3] *       N_frac  *        E_frac; */
          (t_lab->used)++;
        }
        return(0);

      } /* end inside test */

      else {
        return(s_status(err_str, "grid_val",
               TAB_C_ARE_, (geo) ? "sx" : "m", "", N, E));
      } /* end outside error */
    } /* end inside selected table */
    else
    if (!strcmp(t_lab->clb, "geo_feh10")) {
      return(s_status(err_str, "grid_val",
             TAB_C_ARE_, (geo) ? "sx" : "m", "", N, E));
    }

  }
  {
    char name[3 * MLBLNG];
    t_lab = &(grid_table->table_u[0]);
    if (*tab_max == 1) {
      (void) sprintf(name, "grid_val(%s)", t_lab->mlb);
      return(s_status(err_str, name, TAB_C_ARE_,
                    (geo) ? "sx" : "m", "", N, E));
    } else {
      (void) sprintf(name, "grid_val(%s)",
             (t_lab->lab_type == GDE_LAB) ? "geoid" :
             (t_lab->lab_type == DTM_LAB) ? "DTMmodel" :
             (t_lab->lab_type == DHH_LAB) ? "DHHmodel" :
             (t_lab->lab_type == UPL_LAB) ? "UPLmodel" :
             (t_lab->lab_type == DEM_LAB) ? "DEMmodel" : "tabel");
      return(s_status(err_str, name, TAB_C_NON_));
    }
  }
}
