

/* sidefeed                        # page 1   20 Sep 1994 14 11 */


/* Copyright (c) 1994, GD, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include <stdio.h>
#include "geo_lab.h"


int               sidefeed(
/*________________________*/
struct o_fp_str  *out_data
)

/* function : sidefeed 

sidefeed     produces a form feed and a page head.
             returns no of printed chars 

out-data contains :
*o_fp         file receiving the print

o_lc          the actual line no.

o_pg          the actual page no.
              Increased by one before the printout

out_time      giso_dt time to be written.
         == 0 give a call of giso_dt, prn_dt is updated.
*o_hd         array or pointer holding the headline text

*/

{
#include       "giso_dt.h"
#include       "sputcw.h"
  FILE           *out /* = out_data->o_fp */;
  static int      First_time = 0;
  int             ch;
  char            str_d[24];

  out = out_data->o_fp;
  if (!First_time) {
    First_time     =  1;
    out_data->o_lc =  0;
    if (out_data->out_time == 0.0) out_data->out_time = giso_dt();
  } /* if (!First_time) */

  (void) sputcw(str_d, out_data->out_time, 1);
  ch  = fprintf(out, "\n\f\n\n* %-33s ", out_data->o_hd);
  ch += fprintf(out, "%s ", str_d);
  ch += fprintf(out, "  side %-4d ;\n\n", ++(out_data->o_pg));

  return(ch);
}

