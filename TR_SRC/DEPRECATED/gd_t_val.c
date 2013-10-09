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

int                       gd_t_val(
/*_______________________________*/
struct coord_lab         *i_lab,
double                    N_in,
double                    E_in,
double                   *t_val,
struct mgde_str       *grid_tab,
char                      *err_txt
)
{

/* i_lab      crd_sys of input N_in, E_in */
/* t_val      return value of table in (N_in, E_in) */
/* grid_tab   label and buffer initiated to table */
/* err_txt    return error report */

  int                   res = 0;
  double             N_out, E_out, H_out;


  if (grid_tab->init) {
    res = gd_trans(i_lab, i_lab, N_in, E_in, 0.0,
                   &N_out, &E_out, &H_out, t_val, 1, grid_tab, "", NULL);

    if (res < 0) (void) s_status(err_txt,
                        "gd_t_val", res, "m", "m", N_in, E_in, 0.0);

  }
  else res = s_status(err_txt, "gd_t_val", TAB_C_NON_);

  return(res);

}
