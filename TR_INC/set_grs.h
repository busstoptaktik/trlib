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
 


/* set_grs.h version 2009.1         # page 1   39 sep 2009 18 25 */


/* Copyright (c) 2009, Danish National Space Center, DTU       */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of DNSC, DTU    */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */

#ifndef      _SET_GRS_H
#define      _SET_GRS_H
#include     "metadata.h"


int          set_grs(
/*________________________*/
int          ell_nmb,
char         *ellname,
double       *e,
def_data   *data
);

/* 
Function:

    set_grs initializes the parameters of the reference ellipsoid
as a function of the string ellname. The names and KMS-numbers
of the ellipsoids are shown in the macro definitions above.

    The original definitions are used for each ellipsoid, but for
the older ellipsoids (e.g. Hayford) the equator gravity value and
the standard rotation velocity is used to enable a full definition. 

Call and parameters:

set_grs   (integer, return value).
The actual number of the initialized ellipsoid. The numbers can 
be found by running grs_doc. NBNBNB any call will initialize the 
ellipsoid, even the ?, #, and ! without name and number, which 
gives the number -1 and puts e[0] = 0.0. This is also the result 
from an unsuccesfull call, reported as "mode 0 is illegal" on stderr.

ellname  (string, call value). 
The name of the ellipsoid.  The number of the ellipsoid in
character representation can also be used.

e        (double e[9],  (call and) return value).
e[0] = a;    semi-major axis of the reference ellipsoid
e[1] = f;    flattening (a-b)/a, a/(a-b) can be used in a contingent
             call, but (a-b)/a is returned also in this case.
e[2] = J2;   dynamical form factor
e[3] = GM;   gravitational constant * mass
e[4] = omg;  angular velocity [radians/sec]
e[5] = U0;   normal potential at the ellipsoid
e[6] = geq;  normal gravity at equator
e[7] = ksf;  k from Somigliana's formula
e[8] = Q;    The meridian quadrant.

Prog: Maja Boserup, Knud Poder May-Oct 1990
Updated: Knud Poder NOV 1992
Updated: Knud Poder Sep 2009

*/


/* set_grs.h version 2009.1         # page 2   39 sep 2009 18 25 */


/* 
User-defined ellipsoid:

    The user may define an ellipsoid during the execution of a pro-
gramme. This definition is valid until the termination of the pro-
gramme, or until the same programme produces a new definition.

The definition is carried out by calling set_grs with the ellip-
soid name string

                   #<mode><NAME> 

where mode and the necessary parameters in the call are:
     mode    e[0]   e[1]   e[2]   e[3]   e[4]   e[5]   e[6]
      1      a             J2     GM    omega     
      2      a      f                   omega          geq
      3      a      f             GM    omega     
      4    mean dg  f             GM    omeg

The defining constants according to the wanted mode are assigned
to the elements of e according to the table. For GM, geq and 
omega any negative number will signal that the standard values are
used, viz. GM from GRS80 or geq from Hayford 1924 as required for
the mode, and the standard value of omega for the earth, unless
the user assigns a new value. All values of e are available after 
the call, except for the special info-calls, where e[0] = 0.0 and 
set_grs returns -1. <NAME> may be empty, resulting in the ellipsoid 
name  "Special".

Documentation may be obtained by calls with ellname

     !<NAME or NUMBER>   =>  Ellipsoid name, and KMS-number   
     ?<NAME or NUMBER>   =>  Ell. name, nmb, and constants
     *<NAME or NUMBER>   =>  Ell. name, nmb, and const. for mainten.

Calling with ?, #, or ! only produces information for all 
ellipsoids. The programme grs_doc may be used for online docu-
mentation. Use <CR> after prog prompt "grs_doc" for instruction.

*/

#endif

