

/* eetmtu_g.h                       # page 1   29 Sep 1995 16 30 */


/* Copyright (c) 1992, GD,  Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */


/* Definition of the permissible area for the transformation */

extern int               eetmtu_g(
/*______________________________*/
int                      direct,
double                   N_in,
double                   E_in,
double                  *N_out,
double                  *E_out,
char                    *usertxt,
FILE                    *tr_error
);

/*  eetmtu_g:   Autochecking dual prediction function for pre- */
/*  diction  between Gauss-Krueger coordinates  with central   */
/*  longitude 27 degrees on Pulkovo 1942 datum  and UTM zone   */
/*  32 coordinates on EUREF89.                                 */
/*          Result: 0 => ok                                    */
/*                < 0 => error                                 */

/*           (    > 0   => eetm27 -> UTM                       */
/*  direct   {   == 0   => No trans, alarm                     */
/*           (    < 0   => UTM -> eetm27                       */

/*  N_in,  E_in : Input  coordinates in metres                 */
/*  N_out, E_out: Output coordinates in metres                 */

/*  usertxt: User-def. additional alarm text (e.g. a st_nmb)   */
/*  tr_error: FILE for alarm texts. Opened by the user         */

/*  Prog: Knud Poder  5 OCT 1995                               */


