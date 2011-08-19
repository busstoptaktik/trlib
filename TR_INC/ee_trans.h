

/* ee_trans.h version 2.0            # page 1   25 Aug 1995 14 03 */


/* Copyright (c) 1995 GD, Kort-og Matrikelstyrelsen, Denmark   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */


extern int         ee_trans(
/*________________________*/
union geo_lab      *in_lab_a,
union geo_lab      *outlab_a,
double              N_in,
double              E_in,
double              H_in,
double             *Nout,
double             *Eout,
double             *Hout,
char               *usertxt,
FILE               *tr_error
);

/* ee_trans   : prediction function for Estonian coord-systems*/
/* ee_trans   : returns 0 for succes, and -1 for error        */
/* in_lab_a   : label describing the actual input system      */
/* outlab_a   : label descrining the actual output system     */
/* N_in, E_in, H_in : input coordinates in units of metres    */
/* Nout, Eout, Hout : output coordinates in units of metres   */
/*                  : Hout == H_in                            */
/* usertxt    : info text from the user, output->tr_error     */
/* tr_error   : txtfile openend, read and closer by the user  */

/* PROG: Knud Poder, SEP 1995                                 */

