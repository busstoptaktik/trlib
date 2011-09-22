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
 


/* gatg.h      version 1.0          # page 1   10 Aug 1994 17 23 */


/* Copyright (c) 1992 GD, U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

extern double        gatg(
/*_______________*/
double               *TC,
int                 direct,
double               B
);
/*    Function and Parameters

gatg               double function           call and return
Dual transformation function from Gaussian latitude to   
geodetic latitude and the reverse transformation. The 
returned value is the transformed latitude. Note that
gatg has no internal reverse check computation, leaving 
this to the user's programme. The gatg uses Clenshaw sum- 
mation for a series expressing the difference of the two
kinds of latitude by diff = sum(a[j-1]*sin(2*j*B)), with 
j = 4,3,2,1, a[] as a function of n (3. flattn.), and  
B is the relevant latitude. The coefficients for both
directions are storeed in TC, which is initialized by 
conv_lab with the parameters of the actual ellipsoid,
even in the cases where gatg is not needed. The computation 
precision is better than 0.000 000 1 arcseconds. 
The output is in units of radians.

TC                  double array              call and return
The constants needed for the transformation, size at  
least given by TC[8], but mostly a pointer to a general
array may be preferable. The content is as follows:

        TC[0:3] = the coefficients for the transformation
                Gaussian to geodetic.
        TC[4:7] = the coefficients for the transformation
                geodetic to Gaussian.

direct               int                         call
Determines the direction of the transformation:
        direct > 0  => Gaussian to geogr.
        direct < 0  => geogr. to Gaussian
        direct = 0  => Return of a latitude of 1.0e+30

B                   double                       call
The latitude to be transformed, given in radians.

*/


/* gatg.h      version 1.0          # page 2   10 Aug 1994 17 23 */


/*

The formulae are taken from KW = R.Koenig and K.H. Weise,
Mathematische Grundlagen der hoeheren Geodaesie und
Kartographie, Springer-Verlag, Berlin/Goettingen/
Heidelberg, 1951. 

gauss->geo: p. 190-91, eq. (61) & (62).
Last B in (61 0) should be b.
geo->gauss: p. 186-87, eq. (51) & (52).

prog: Knud Poder 10 APR 1990;
modf: Knud Poder  1 JUN 1992, automatic init now by conv_lab;:
modf: Knud Poder  3 MAR 1994, protyping.
*/


