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
 


/* srch_plate  ver 2008.02          # page 1   16 May 2008 11 57 */


/* Copyright (c) 2008, DTU, National Space Research Institute    */
/* All rights reserved.                                          */

/* This is unpublished proprietary source code of DTU,           */
/* National Space Research Institute, Denmark. This copyright    */
/* claim does not indicate an intention of publishing this code. */

#if !defined     (H_SRCH_PLATE)
#define           H_SRCH_PLATE

#include <stdio.h>
#include <string.h>
#include "geo_lab.h"

extern int                srch_plate(
/*_________________________________*/
struct plate_info        *pl_inf,
double                    B,
double                    L,
struct plm_lab           *plm_tab,
struct gps_c_str         *c_gps,
char                     *err_txt
);


/* search plate containing coordinates  B, L, */
/* return values ::                           */
/*       >=  0   :: plate number              */
/*          -1   :: NO plate found            */
/*          -2   :: i/o error on binary model */

/* the platenumber is used to find the plate_information in the */
/* struct plate_info pl_inf                                     */

/* progr. K. Engsager. Feb. 2003, 2008 */

#endif
