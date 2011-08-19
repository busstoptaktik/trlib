

/* geoid_c                         # page 1   30 Oct 2001 17 25 */


/* Copyright (c) 2001 LG, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                      */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include        <stdio.h>
#include        <string.h>
#include        "geo_lab.h"
#include        "geoid_d.h"


int                       geoid_c(
/*______________________________*/
struct mgde_str          *tab_table,
int                       info,
FILE                     *out
)
{
#include        "gettabdir.h"
#include        "c_tabdir_file.h"

int                         g, res = 0;
  struct gde_lab             *t_lab;

  if (tab_table == NULL) return(0);

  if (tab_table->init) {

    tab_table->init = 0;

    for (g = 0, t_lab = &(tab_table->table_u[0]);
         g < tab_table->tab_max;  g++, t_lab++) {
      if (t_lab->lab_type == GDE_LAB ||
          t_lab->lab_type == DTM_LAB ||
          t_lab->lab_type == DHH_LAB ||
          t_lab->lab_type == UPL_LAB ||
          t_lab->lab_type == DEM_LAB ||
          t_lab->lab_type == TAB_LAB) (void) fclose(t_lab->fd);
    }

    if (info && out != NULL) {
      char     pth_tab[1024], *global_dir;
      global_dir = gettabdir();
#if defined(sparc) || defined(__sparc)
      if (*global_dir == '\0') global_dir = settabdir(NULL);
#endif
      (void) fprintf(out, "\n\n      TABLE INFO");
      (void) fprintf(out,
             "\n\n        table                          used\n");

      for (g = 0, t_lab = &(tab_table->table_u[0]);
           g < tab_table->tab_max;  g++, t_lab++) {
        if (t_lab->used > 0) {
          if (t_lab->local) *pth_tab = '\0';
          else (void) strcpy(pth_tab, global_dir);
          (void) strcat(pth_tab, t_lab->mlb);
          (void) fprintf(out, "\n   %-30s   %7d",
                         pth_tab, t_lab->used);
          t_lab->used = 0L;
        }
      }

      if (info > 1) {
        (void) fprintf(out, "\n\n   Page Acces Info\n");
        (void) fprintf(out,
               "\n   Pages            %7d",  GEOIDPAGES);
        (void) fprintf(out,
               "\n   In pages         %7d", tab_table->hit);
        (void) fprintf(out,
               "\n   Loaded           %7d", tab_table->agn);
        (void) fprintf(out,
               "\n   Holes            %7d", tab_table->xmt);
      } /* statistics */
    } /* info */
  }
  (void) c_tabdir_file(1, NULL);
  return(res);
}

