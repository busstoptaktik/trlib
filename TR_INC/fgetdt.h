

/* fgetdt version 2010.1            # page 1   12 jan 2010 09 39 */


/* Copyright (c) 2010, Danish National Space Center, DTU       */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of DNSC, DTU    */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */


extern double       fgetdt(
/*_______________________*/
double             *JD2000,
int                *used,
int                *term_c,
FILE               *fp
);

/* function sgetdt : reading of comment word, G.I. date_time
and MD. long word.

fgetdt       (return and return)        double
_            Date stripped for special characters

fp           (call)                     FILE *
_            Pointer  to a file opened and ready for reading

JD2000       (return)                  *double
_            Julian Date from JD 2000.0       

used         (return)                  *int
_            Number of characters read from the file

term_c       (return)                  *int
_            Last char read, terminating char

The function reads date, time in the following format
ISO (our)   1992 04 17,  9.45  or
ISO         1992 04 17

Upon error the function returns -2.0.

Prog. Karsten Engsager 1999.
Upddated: KE 2010

*/

