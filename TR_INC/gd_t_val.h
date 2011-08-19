

/* gd_t_val  ver 2001.01          # page 1   21 Dec 2001 11 57 */


/* Copyright (c) 2001 GD,   Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include "geo_lab.h"
#include "geoid_d.h"

extern int                gd_t_val(
/*________________________________*/
union geo_lab            *i_lab,
double                    N_in,
double                    E_in,
double                   *t_val,
struct mgde_str          *grid_tab,
char                     *eer_txt
);

/* i_lab      crd_sys of input N_in, E_in */
/* t_val      return value of table in (N_in, E_in) */
/* grid_tab   label and buffer initiated to table */
/* err_txt    return error report */

