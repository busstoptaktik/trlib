

/* sgetst.h  version 1.0              # page 1    3 May 1993 14:40 */


/* Copyright (c) 2009, Danish National Space Center, DTU       */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of DNSC, DTU    */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */

#include  "geo_lab.h"


extern int                 sgetst(
/*______________________________*/
char              *in,
int                lab_rgn,
struct st_str     *stn,
int               *tchar,
int               *used
);

/* Function:

sgetst             (call and return)            int
Inputs a station number from a text string and stores it in
a station structure. The input starts from the first occurring
digit, capital letter or - (the sign minus). All other leading
characters are skipped. The input stops after double space or
first occurring lower case letter after input has started.
Single spaces are blind. Reading brackets are assumed removed
by the function producing the string (fgetln). Syntax errors
or other input troubles causes the input to be stored as a
string of at most 10 characters.
*/

/* Parameters:

*in                  (call)                     char
The element of a string array, where the input string of the station
number begins.

lab_rgn gives a default region

*stn                 (call and return)          struct st_str
A station structure. See geo_lab.h for the description.

*tchar               (call and return)          int
The terminating character of the station number, mostly a lower
case letter, a double space or TAB. Note that only the first
character of a sequence of descriptors are input.

*used                (call and return)          int
The number of characters input (including the skipped ones, so that
     "in += *used;" will move the input string pointer to the next
element of the string.

*/

/* Prog: Frands Steffensen 1992, 1994, 2009; */
