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
 


/* tcts_u.h  version 2.0             # page 1   01 Jul 1994 11 02 */


/* Copyright (c) 1991 U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#if !defined     (H_TCTS_U)
#define           H_TCTS_U

extern int             tcts_u(
/*__________________________*/
double       Nin,
double       Ein,
double       *yout,
double       *xout,
int          landsdel,
int          direct,
char         *usertxt,
FILE         *trferror,
...
);

/* 
Function and parameters

tcts_u          int function,   fnc no. 8        call and return
Dual autochecking transformation (prediction) function for

  u       TR TC zone 32, ED50 <-> UTM zone 32, ED50
  j       TR TC zone 32, ED50 <-> System 1934 Jylland
  s       TR TC zone 32, ED50 <-> System 1934 Sjælland
  b       TR TC zone 33, ED50 <-> System 1934/45 Bornholm

When landsdel is CAPITAL LETTER then the partial derivatives are
included and the results are placed in the last double pointer
position (called *DD):
          *(DD+0) = d(*yout) / d(Nin)
          *(DD+1) = d(*yout) / d(Ein)
          *(DD+2) = d(*xout) / d(Nin)
          *(DD+3) = d(*xout) / d(Ein)
          *(DD+4) = d(Nin) / d(*yout)
          *(DD+5) = d(Nin) / d(*xout)
          *(DD+6) = d(Ein) / d(*yout)
          *(DD+7) = d(Ein) / d(*xout)

The function gives the result of the transformation as

      tcts_u = 0        => ok, 
      tcts_u = -1       => parameter error (landsdel or direct)
      tcts_u = -2       => coordinates too far away.

The physical precision of the transformation is generally
1-2 cm, but the numerical precision is generally some few
mm, increasing to some cm far from the central parts. 

Alarm printing is to an error file. The user may add    
identification of the alarm.

Nin, Ein        double                           call
UTM or s34 coordinates input for transformation in metres.

*yout, *xout    double                           return
s34 or UTM coordinates output after transformation in metres.

(continued) */


/* uts34.h  version 2.0             # page 2   01 Jul 1994 11 02 */


/*
landsdel        int                              call
Control of the used coordinate systems.  
landsdel = 'j' (or 1) => TR TC 32 ED50 <-> S34 Jylland
landsdel = 's' (or 2) => TR TC 32 ED50 <-> S34 Sjælland
landsdel = 'b' (or 3) => TR TC 32 ED50 <-> S34 (= S45) Bornholm
landsdel = 'u' (or 4) => TR TC 32 ED50 <-> UTM 32 ED50
Any other character (or value) creates alarm output on stdout
and return(-1).

direct           int                             call
Control of the direction of transformation.
     direct > 0 =>  UTM -> s34
     direct < 0 =>  s34 -> UTM
     direct = 0 =>  alarm output to the errorfile.        

*usertxt          char                           return
A string pointer to a char array. The string should be written
before the call of the function. The station number is a possi- 
bility. The string is written to the alarm file.

trferror        FILE                            return
A pointer to a stream. The file must be produced, opened and
closed by the user. Alternatively stdout or stderr may be used.

optional when landsdel is a CAPITAL LETTER:
*DD             double                           return
A pointer to the partial derivatives result
          *(DD+0) = d(*yout) / d(Nin)
          *(DD+1) = d(*yout) / d(Ein)
          *(DD+2) = d(*xout) / d(Nin)
          *(DD+3) = d(*xout) / d(Ein)
          *(DD+4) = d(Nin) / d(*yout)
          *(DD+5) = d(Nin) / d(*xout)
          *(DD+6) = d(Ein) / d(*yout)
          *(DD+7) = d(Ein) / d(*xout)

C extern functions:
double fabs()
int    printf()
int    fprintf()

KMS special C extern functions:
none

The tolerance is set by a define statement to 6 cm, and may
be redefined. The chosen value is comparable to the precision
of the polynomial approximation.

Prog: Knud Poder, APR 1991

*/

#endif
