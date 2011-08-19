

/* hlm_tr.h                         # page 1   03 Mar 1995 13 14 */


/* Copyright (c) 1995, GD, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#ifndef   _HLM_TR_
#define   _HLM_TR_

/*
Transformation items
____________________
N0_in =  f_t[0]
E0_in =  f_t[1]
N0out =  f_t[2]
E0out =  f_t[3]
a     =  f_t[4]
b     =  f_t[5]
D     =  a*a + b*b
A     =  a/D
B     = -b/D

forward:
Nout  =  N0out + a*(N_in - N0_in) - b*(E_in - E0_in)
Eout  =  E0out + b*(N_in - N0_in) + a*(E_in - E0_in)

backward:
Nout  =  N0_in + A*(N_in - N0out) - B*(E_in - E0out)
Eout  =  E0_in + B*(N_in - N0out) - A*(E_in - E0out)

forward check:
N  =  N0_in + A*(Nout - N0out) - B*(Eout - E0out)
E  =  E0_in + B*(Nout - N0out) - A*(Eout - E0out)

backward check:
N  =  N0out + a*(Nout - N0_in) - b*(Eout - E0_in)
E  =  E0out + b*(Nout - N0_in) + a*(Eout - E0_in)
*/



extern int        hlm_tr(
/*_____________________*/
double         *f_t,
int             direct,
double          N_in,
double          E_in,
double         *Nout,
double         *Eout,
char           *usertxt,
FILE           *tr_error
);


/* hlm_tr.c                         # page 2   03 Mar 1995 13 14 */

/* hlmtr       simple Helmert transformation in 2 dim. */
/* Dual autochecking transformation, tol 30 microns.   */

/* direct > 0  => direct transformation                */
/* direct < 0  => inverse transformation               */
/* direct == 0  => identical transformation            */

/* tr_contr == 1  => alarm output to tr_error          */
/* tr_contr == 0  => exit                              */

/* Prog: Knud Poder, MAR 1995                          */

#endif
