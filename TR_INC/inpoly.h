

/* inpoly.h      version 1.1        # page 1   10 May 1994 08 57 */


/* Copyright (c) 1997, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                   */

/* This is unpublished proprietary source code of ITG, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

/* Prog.: F. Steffensen MAR. 1997 (MI/ITG) */


# if !defined       (H_INPOLY)
#define              H_INPOLY

extern int             inpoly(
double                       *pol_base,
double                       *Nc,
double                       *Ec
);

/* C function : inpoly

 inpoly        (call & return)         int 
               inpoly > 0   (Nc, Ec inside polygon )
               inpoly = 0   (Nc, Ec on polygon line)
               inpoly < 0   (Nc, Ec outside polygon)

 *pol_base     (call)                  * double
               pol_base[0] = number of corners in polygon
               pol_base[1] = 1. crd to 1. point          
               pol_base[2] = 2. crd to 1. point          
               pol_base[3] = 1. crd to 2. point          
               ................................          
               (Northing Easting osv.)

 *Nc           (call)                  * double
               1. coordinat for testing point in polygon

 *Ec           (call)                  * double
               2. coordinat for testing point in polygon

*/

#endif


