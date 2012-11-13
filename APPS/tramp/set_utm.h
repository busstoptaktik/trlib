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
 


/* set_utm.c                        # page 1     30 May 2008 11 40 */


/* Copyright (c) 1992 GD, U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

extern void             set_utm(
/*____________________________*/
struct proj_const      *c_lab
);

/* Function:
set_utm             (call and return)            int
Produces the constants needed for a transformation,
where the coordinate system is described by parameters
in proj_const::
  double    a;            Semi major axis of the ellipsoid
  double    f;            Flattening of the ellipsoid
  double    B0;           Origin latitude
  double    N0;           Origin northing
  double    L0;           Origin longitude
  double    E0;           Origin easting 
 Prepared for lambert and stereographic projections::
  double    B1;           Contact/int.sect lat (lmb), 90-B0 (stg) 
  double    B2;           Latitude of intersection for lmb
  double    scale;        Central scale of the projection 


Parameter:
*c_lab             (call and return)   struct proj_const
The constants needed for the transformation are inserted
in c_lab. A flattening given as inverse flattening is
converted to the flattening. E.g. 297 is converted to
the value of 1/297. The constants for transformation
geographical coordinates to/from Gaussian coordinates
are always inserted - even if not needed by the actual
coordinate system.

Prog: Knud Poder   1992.
Prog: K. Engsdager 2008.

*/


