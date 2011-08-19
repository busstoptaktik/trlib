
/* gen_pol.h                        # page 1   06 Mar 1995 12 13 */


/* Copyright (c) 1995, GD, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#ifndef     _GEN_POL_
#define     _GEN_POL_

extern int            gen_pol(
/*__________________________*/
double               *C_f,
double               *C_r,
double                N_in,
double                E_in,
double               *Nout,
double               *Eout
);

/* gen_pol:  2 general polynimiae for transformation */
/* C_f:      the polynomiae for forward  transform.  */
/* C_r:      the polynomiae for reverse check transf.*/
/* N, E:     input/output coordinates                */
/* tr_contr: 1 => report, 0=> exit                   */
/* usertxt:  text inserted by the user's programme   */
/* tr_error: file opened and read by the user's prog.*/

/* NB: The internal independant variables used in the*/
/* evaluation is relative to "center values". Values */
/* here exceeding 500 km give a termination.         */

/* PROG: Knud Poder, MAR 1995                        */

#endif


