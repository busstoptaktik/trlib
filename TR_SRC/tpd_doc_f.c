

/* set_tpd  version 2010.1           # page 1   12 Jan 2010 13 14 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

#include <string.h>
#include <math.h>
#include "geo_lab.h"
#include "typ_dec.h"
#include "trthread.h" /*is this really needed here??? */

extern struct na_gr_tp_fc    ngtf[];

int                          tpd_doc_f(
/*___________________________________*/
char                        *head_txt,
char                        *tpd_txt
)
{
  static THREAD_SAFE  struct na_gr_tp_fc  *p_t = NULL;
  static THREAD_SAFE  unsigned char        s_gr = 0;
  char                       *p_c, *p_f;   

  if (p_t == NULL) p_t = ngtf; else ++ p_t;
  if (p_t->gr) {
    if (s_gr != p_t->gr) {
      switch(p_t->gr) {
      case  1: p_c = "Sexagesimal";  p_f = "rad"; break;
      case  2: p_c = "Centesimal";   p_f = "rad"; break;
      case  4: p_c = "Metric";       p_f = "m"; break;
      case  5: p_c = "Astronomic";   p_f = "m"; break;
      case  8: p_c = "Geopotential"; p_f = "gpu"; break;
      case 10: p_c = "Gravity";      p_f = "gal"; break;
      case 12: p_c = "Special";      p_f = "special"; break;
      default: p_c = "Undefined";    p_f = "Undefined"; break;
      }
      (void) sprintf(head_txt, "\n\n        %s types\n\n    %s%s->out",
                p_c, "grp   typ    unit  defdg maxdg  convfct ", p_f);
    }
    (void) sprintf(tpd_txt,
              "\n   %3d   %3d     %-5s %3d   %3d   %12.11e",
              p_t->gr, p_t->tp, p_t->na, p_t->dd, p_t->md, p_t->fc);
    s_gr = p_t->gr;
    return((int) s_gr);
  } else {
    *head_txt = '\0';
    *tpd_txt  = '\0';
    s_gr      = 0;
    p_t       = NULL;
    return(0);
  }
}
