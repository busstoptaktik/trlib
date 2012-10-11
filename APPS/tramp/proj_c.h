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
 


/* proj_const   ver 2003.01          # page 1   11 Dec 2002 11 57 */


/* Copyright (c) 2003, Kort-og Matrikelstyrelsen, Denmark   */
/* All rights reserved.                                     */

/* This is unpublished proprietary source code, Kort-og        */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#if !defined(H_proj_const)
#define      H_proj_const

#include    <stdio.h>
#include    <sys/types.h>
#include    <math.h>

#ifndef    M_PI
#include   "kms_math.h"
#endif

#define    PROJ_CONST_VERSION      (1)

#define  TRF_INACC_       (-1)
#define  TRF_TOLLE_       (-2)
#define  TRF_AREA_        (-3)
#define  TRF_ILLEG_       (-5)

struct  proj_const {
  double    a;           /* Semi major axis of the ellipsoid */
  double    f;           /* Flattening of the ellipsoid */
  double    B0;          /* Origin latitude */
  double    N0;          /* Origin northing */
  double    L0;          /* Origin longitude */
  double    E0;          /* Origin easting */
/* Prepared for lambert and stereographic projections::
  double    B1;           Contact/int.sect lat (lmb), 90-B0 (stg) 
  double    B2;           Latitude of intersection for lmb */
  double    scale;       /* Central scale of the projection */
  double    tol;         /* Tolerance of check transformation */
  double    Qn;          /* Merid. quad., scaled to the projection */
  double    Zb;          /* Radius vector in polar coord. systems  */
  double    tcgg[10];    /* Constants for Gauss <-> Geo lat.   */
  double    utg[5];      /* Constants for transv. merc. -> geo */
  double    gtu[5];      /* Constants for geo -> transv. merc. */
};

#endif


