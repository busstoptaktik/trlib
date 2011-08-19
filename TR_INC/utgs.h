

/*  utgs.h version 2.0                # page 1   22 Feb 1993 12 54 */


/* Copyright (c) 1989 GD,   Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

extern int               utgs(
/*___________________________*/
double                    Nin,
double                    Ein,
double                    *yout,
double                    *xout,
int                       landsdel,
int                       tr_cntr,
char                      *usertxt,
FILE                      *tr_error
);


/*
utgs: int function giving the transformations

          utm zone 32 <-> gs system (Jylland and Sjlland)
                         and
          utm zone 33 <-> gsb system (Bornholm).

The transformation may have errors up to 1 m, but the local
consistency and computing precision is of the order of 1 cm.
utgs returns the same value as returned in yout.

Nin, Ein input coordinates (utm or gs(b)), double
in units of metres. Nin and Ein are called by value.

yout, xout output coordinates in (gs(b) or utm), double
units of metres. yout and xout are called by reference.

landsdel, call by value indicating

          1 (or 2) => gs and utm zone 32 i.e. Jylland + Sjlland,
          's', 'j', or 'd' gives the same effect.

          3 (or 'b')  => gsb and utm zone 33 i.e. Bornholm.

direct, call by value indicating

          direct > 0  => transformation utm --> gs

          direct < 0  => transformation gs --> utm

          direct = 0  => no transformation,
          i. e. yout = Nin and xout = Ein (no alarm occurs)


*/
