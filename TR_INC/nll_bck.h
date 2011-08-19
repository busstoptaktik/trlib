/* nll_bck.c               # page  1 Thu Feb 25 16:04:13 1993 */

/* Copyright (c) 1993, GD, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/D, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

extern int         nll_bck(
/*-----------------------*/
struct nl_ca_str  *CAT,
struct nl_ca_str  *CAT_rhs,
double            *N,
double            *RHS,
int                f_col,
int                l_col,
int                hlm_col,
int                n);

/* nll_back solves normal equations stored columnwize according */
/* to CAT in an upper triangular matrix : double array N.       */
/* N has a total of n columns excl. the rhs                     */
/* (i.e col.no. n is rhs.                                       */
/* Prog. KP 19 NOV 1990.                                         */
/* Prog. KE 25 FEB 1993.                                         */

