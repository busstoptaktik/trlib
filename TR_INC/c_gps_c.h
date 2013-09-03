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
 


/* c_gps_c    ver 2003.01             # page 1    4 Jul 2003 12 06 */


/* Copyright (c) 1999 LG, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of LG, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#if !defined     (H_C_GPS_C)
#define           H_C_GPS_C

extern int       c_gps_c(
/*_____________________*/
struct gps_c_str    *shp,
double                Xi,
double                Yi,
double                Zi,
double                *Xo,
double                *Yo,
double                *Zo
);

/* transforms cartesian coordinates to cartesian coordinates */

/* Molodensky transformation SMALL DATUM SHIFTS only */

/* (*Xo) = (Xi) + (T1) + ( D  -R3  R2) (Xi)             */
/* (*Yo) = (Yi) + (T2) + ( R3  D  -R1) (Yi)             */
/* (*Zo) = (Zi) + (T3) + (-R2  R1  D ) (Zi)             */

#endif
