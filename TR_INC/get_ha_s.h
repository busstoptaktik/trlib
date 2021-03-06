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
 


/* get_half.h                        # page 1   28 Apr 1993 11:00 */


/* Copyright (c) 1993, GD, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#if !defined     (H_GET_HA_S)
#define           H_GET_HA_S

#include   <stdio.h>
#include   <math.h>

extern double      get_ha_s(
FILE              *fw,
FILE              *fw1,
double             a,
double             f,
int                j,
int               *i,
double             s_max
);

/* service routine for get_areaf and get_areacf                 */
/* get_ha_s   returns the circumference of the points in fw1    */

/* get_ha_s   copies the j geo_crd in fw to fw1                 */
/* when dist(P(i), P(i-1)) > s_max a new point is inserted      */
/* midway between the two points                                */

/* a      is the major axis of the ellipsoid                    */
/* f      is the flattening of the ellipsoid                    */
/* j      is the number of points in fw                         */
/* *i     is the number of points in fw1                        */
/* s_max  id the max distance for insertion of new points       */

#endif
