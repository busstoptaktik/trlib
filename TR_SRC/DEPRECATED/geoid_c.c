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

int                       geoid_c(
/*______________________________*/
struct mgde_str          *tab_table,
int                       info,
FILE                     *out
)
{

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

