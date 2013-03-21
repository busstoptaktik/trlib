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

/* Copyright (c) 1999, LG,  Kort & Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of LG, Kort &   */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include    <stdio.h>
#include    <stdarg.h>
#include    <string.h>
#include    "KmsFncs.h"

int                      t_status(
/*______________________________*/
FILE                    *tr_error,
char                    *usertxt,
char                    *fnc_name,
int                      err_type,
...
)

/* ADDITIONAL PARAMETERS 
char                    *c_unit;
char                    *h_unit;
double                    N,  E,  H; 
double                   dN, dE, dH;
*/

{

  va_list              cp;

  char               *c_unit, *h_unit;
  double              N, E, H = 0.0;
  char                tpt[512];
  int                 c_err, t_err;

  va_start(cp, err_type);
  if (tr_error != NULL) {

    c_err =  err_type        % 100;
    t_err = ((err_type / 100) % 100) * 100;
    if ((TRF_AREA_  <= c_err && c_err < 0) ||
        (TAB_C_ARE_ <= t_err && t_err < 0)) {
      c_unit = va_arg(cp, char *);
      h_unit = va_arg(cp, char *);
      N      = va_arg(cp, double);
      E      = va_arg(cp, double);
      if (*h_unit != '\0') H = va_arg(cp, double);
      (void) s_status(tpt, fnc_name, err_type, c_unit, h_unit, N, E, H);
    }
    else (void) s_status(tpt, fnc_name, err_type);

    if (strlen(usertxt)) (void) fprintf(tr_error, "\n%-20s", usertxt);
    (void) fprintf(tr_error, "%s", tpt);

  }
  va_end(cp);
  return(err_type);
}
