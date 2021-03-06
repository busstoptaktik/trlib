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
 


/* set_trc.c                        # page 1   10 Aug 1994 11 40 */


/* Copyright (c) 1992 GD, U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#if !defined     (H_SET_TRC)
#define           H_SET_TRC

extern int              set_trc(
/*____________________________*/
struct coord_lab         *c_lab
);

/* Function:
set_trc             (call and return)            int
Produces the constants needed for a transformation,
where the coordinate system is described by a coord_lab. 
The return value is the number of the coordinate system.
An erroneous c_lab gives the return value -1.

Parameter:
*c_lab             (call and return)           coord_lab
The constants needed for the transformation are inserted
in c_lab. A flattening given as inverse flattening is
converted to the flattening. E.g. 297 is converted to
the value of 1/297. The constants for transformation
geographical coordinates to/from Gaussian coordinates
are always inserted - even if not needed by the actual
coordinate system.

Prog: Knud Poder  1992.

*/

  #endif
