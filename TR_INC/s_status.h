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
 


/* s_status.h                       # page 1   7 jan 2009 13 26 */


/* Copyright (c) 2009, National Space Institute, DTU, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of DTU, Denmark */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */

#if !defined     (H_S_STATUS)
#define           H_S_STATUS

extern int               s_status(
/*______________________________*/
char                    *err_str,
char                    *fnc_name,
int                      err_type,
...
);

/* ADDITIONAL PARAMETERS 
char                    *c_unit;
char                    *h_unit;
double                    N,  E,  H; 
double                   dN, dE, dH;

SPECIAL::  c_unit == "non"  ==>>  SKIPS add params writing 
*/

/*  s_status: alarm function for transformations and predict- */
/*  dicates a programme error.                                */

/*  tr_error: FILE for alarm texts. Opened by the user, std-  */
/*  out or stderr may be used.                                */

/*  usertxt: User-def. additional alarm text (e.g. a st_nmb)  */

/*  fnc_name: The name of the transformation function         */

/* err_type: the type of error (1.group) + (2.group) + (3.group) */
/* 1.group   ::  TRANSFORMATION of CRD                        */
/* TRF_INACC_ =>  Boundary exceeded (inacurate result) 5 add params */
/*               (may still be usefull for mapping)           */
/* TRF_TOLLE_ =>  Tolerance exceeded                   8 add params */
/* TRF_AREA_  => Boundary exceeded (serious)           5 add params */
/*                (result is undefined)                       */
/* TRF_GEOID_ => NO GEOID ILLEGAL TRANSFORMATION              */
/* TRF_ILLEG_ =>  Illegal transformation                      */
/* TRF_PROGR_ =>  System/Prog fault                           */
/* 2.group   ::  GET GRID_VALUE                               */
/* TAB_C_ARE_ => Table Boundary exceeded (serious)            */
/* TAB_C_PRJ_ => TABLE called with wrong coord label          */
/* TAB_C_NON_ => manager.tab table_type not found             */
/* TAB_G_MAX_ => Table too many tables to be initilized       */
/* TAB_N_NAM_ => manager.tab table_name not found             */
/* TAB_R_MAN_ => manager.tab region not found                 */
/* TAB_N_MAN_ => manager.tab not found                        */
/* TAB_N_SYS_ => manager.tab not found                        */
/* 3.group   ::  GNNS transformation errors                   */
/* PLATE_NON_ => itrf_plat.bin NOT FOUND                      */
/* PLATE_OUT_ => station out of PLATES                        */
/* ITRF_NON_  => ITRFyy NONE FOUND                            */
/* ITRF_SEQ_  => ITRF NOT IN SEQUENCE in manager              */
/* ITRF_DTM_  => DATUM NOT FOUND                              */
/* ITRF_MAX_  => ITRF TOO MANY                                */
/* ITRF_NAM_  => manger.gps NOT FOUND                         */
/* ITRF_SYS_  => ITRF SYSTEM FAULT                            */
/* PLATE_NO_VEL_ => PLATE has no velocity (warning)           */

/* Any other type number gives -1 by default                  */

/* ... : parameters required as concluded from label and type */

/*  c_unit: "sx" or "m ". Used for units of N and E           */
/*          "non" skips the parameter wtiting                 */
/*  h_unit: "m " or "". "" means no heights                   */

/* Types 1,2 :   N,  E, (and  H if appropriate)               */
/* Type  1:     dN, dE, (and dH if appropriate) in units of   */
/* metres, from the reverse check transformation              */

/* Example 1: s_status(err_str, "ptg", 1, "sx", "", N, E, dN, dE);*/

/*  Prog: Knud Poder  30 SEP 1995                             */
/*  Updated: Karsten Engsager jan 2009                        */

#endif
