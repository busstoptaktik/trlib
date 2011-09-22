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
 


/* erad.h                           # page 1   29 sep 2009 16 08 */


/* Copyright (c) 2009, Danish National Space Center, DTU       */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of DNSC, DTU    */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */

extern double               erad(
/*_____________________________*/
double               a,
double               f,
double               lat,
int                  ft
);

/* Function
 erad computes, using the semi-major axis and the flattening
(or automatically the inverse flattening):

        (a) 4 radii of curvatures of the ellipsoid,
            as a function of the latitude,
           (a1) M: the radius of crv. in the meridian
           (a2) N: the radius of crv. in the prime vertical   
           (a3) R: the mean radius of crv.
           (a4) r: the radius of a latitude parallel

        (b) G: The Gaussian curvature as a function of 
               the latitude

        (c) K: Half the Gaussian curv. as a function of
               the northing

        (d) Q: The length of the meridian arc from eq. to
               the latitude

        (e) L: The latitude corresponding to the length 
               of the meridian arc. 
(continued)
*/


/* erad.h                           # page 2   29 sep 2009 16 08 */


/* Parameters:  

 a            call               double 
 The semi-major axis of the ellipsoid in units of metres.
 NB: use a*(central scale) for curvatures related to projections.  
 
 f            call               double       
 The flattening of the ellipsoid (the inverse flattening 
 may be used instead with equal effect).

 lat          call               double       
 The latitude in units of radians or the meridian arc in units 
 of metres according to the desired conversion.   

 ft           call               int  
 The flag controlling the computation: 
 
 'M' => the meridian curvature radius
 'N' => the prime vertical curvature radius 
 'R' => the mean curvature radius = sqrt(M*N)
 'r' => the radius of a latitude parallel = N cos(lat)   
 'G' => the Gaussian curvature as a fncti. of the latitude
 'K' => half the Gauss. curv. as a fnct. of the Northing         
 'Q' => the meridian arc                     
 'L' => the latitude                 

 Any other value of ft gives an alarm exit.        

 Prog. KP OCT 1989/JAN 1992.       

 */


