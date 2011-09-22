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
 


/* gd_t_val  ver 2001.01          # page 1   21 Dec 2001 11 57 */


/* Copyright (c) 2001 GD,   Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include <stdio.h>
#include <string.h>
#include "geo_lab.h"
#include "geoid_d.h"

int                       gd_t_val(
/*_______________________________*/
union geo_lab            *i_lab,
double                    N_in,
double                    E_in,
double                   *t_val,
struct mgde_str          *grid_tab,
char                     *err_txt
)
{

/* i_lab      crd_sys of input N_in, E_in */
/* t_val      return value of table in (N_in, E_in) */
/* grid_tab   label and buffer initiated to table */
/* err_txt    return error report */



/* gd_t_val  ver 2001.01          # page 2   21 Dec 2001 11 57 */


#include              "gd_trans.h"
#include              "s_status.h"

  int                   res = 0;
  double                N_out, E_out, H_out;


  if (grid_tab->init) {
    res = gd_trans(i_lab, i_lab, N_in, E_in, 0.0,
                   &N_out, &E_out, &H_out, t_val, 1, grid_tab, "", NULL);

    if (res < 0) (void) s_status(err_txt,
                        "gd_t_val", res, "m", "m", N_in, E_in, 0.0);

  }
  else res = s_status(err_txt, "gd_t_val", TAB_C_NON_);

  return(res);

}

