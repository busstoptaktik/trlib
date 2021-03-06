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
 


/* utsb.h                         # page 1   22 Feb 1993 12 54 */


/* Copyright (c) 1992, GD,  Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */


#if !defined     (H_UTSB)
#define           H_UTSB

extern int               utsb(
/*__________________________*/
double                   N_in,
double                   E_in,
double                   *N_out,
double                   *E_out,
int                      direct,
char                     *usertxt,
FILE                     *tr_error
);

/*
  function and parameters

  utsb               (return)                   int
  Dual autochecking transformation procedure for transformation
  between UTM Zone 32 and system sb. An alarm is produced, when
  the check by the inverse transformation exceeds the tolerance
  of 0.5 mm.

  The values of the function are:

          0       => ok
          1<<11   => check tolerance of 0.0005 m exceeded
          -1      => outside the sb area

  The permissible sb area is in sb-coord:

        6 125 000 m  <= Northing <=  6 142 000 m
                485 000 m  <= Easting  <=    518 000 m

  N_in, E_in             (call)                   double
  The s34- or sb-coordinates input in units of metres.

  N_out, E_out             (return)                 double
  The sb- or UTM32-coordinates output from the procedure
  as transformed and checked coordinates in units of metres.

  direct                 (call)                   int
  direct >= +1  => transformation utm32 -> sb
  direct <= -1  => transformation sb -> utm32
  direct ==  0  => no transformation (output == input)


        Prog: Knud Poder  DEC 1992
  */

#endif
