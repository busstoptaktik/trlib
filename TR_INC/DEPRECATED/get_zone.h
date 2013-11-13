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
 


/* get_zone    ver 2000.01          # page 1   19 jan 1999 10 19 */


/* Copyright (c) 2000  LG,  Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of LG, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

/* get_zone(lab) : Service function for finding the zone       */
/*    result  >  0 : the zone of the projection (e.g. utm)     */
/*    result == -1 : the coord lab has no zone                 */
/*    result == -2 : the zone and the parameters go NOT agree  */
/*    result == -3 : the zone was not found                    */
/*    result == -4 : the label is not a coord label            */

#ifndef       _GET_ZONE_FNC_
#define       _GET_ZONE_FNC_

extern int            get_zone(
/*___________________________*/
struct coord_lab       *c_lab
);

/*  SEE also conv_crd.c and conv_crd.h for details                 */


/*
service function for finding zone of a projection
    and testing of appropriate parameter of the COORD labels

Prog: K. Engsager, SEP 2000

*/

#endif


