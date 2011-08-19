

/* sgetcw.c version 1.0               # page 1   14 Jun 1994 13 18 */


/* Copyright (c) 1994, Kort-og Matrikelstyrelsen, GD, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of GD, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not */
/* indicate an intention of publishing this code.             */

#include   <stdio.h>
#include   <string.h>

double                sgetcw(
/*_________________________*/
char                 *in,
int                  *used,
int                  *term_c
)

/* function sgetcw : reading of comment word, G.I. date_time
and MD. long word.

sgetcw       (return and return)        double
_            Date_time stripped for special characters
             and MD do

in           (call)                     char *
_            Pointer to char array holding the cw

used         (return)                  *int
_            Number of characters read from in

term_c       (return)                  *int
_            Last char read, terminating char

The function reads date, time in the following format
ISO (our)   1992 04 17,  9.45  or
old (our)     17 04 92,  9.45  or
simpel            1992         or
(very old)          do             (not used anymore)

MD cw        1 00045 85 02 3 023   max val. 900 00000 00 00 0 000

In case of 'do' the function returns -1.0. The user of this function
must take action in this case. (try prev. cw)

Upon error the function returns -2.0.

Due to the use of 'sscanf', and the way the cw is read, the
char array must not hold 'spaces'. Use fgetln to this end.

*/


/* sgetcw.c version 1.0               # page 2   14 Jun 1994 13 18 */


{
  char            *ps, c, char_do[3];
  int             ac, ch, ye, mo, ho, mi;
  int             w1, da;
  double          cw;

  /* first try readning with format as double, if this         */
  /* is an succes and no comma is met, then return MD cw.      */

  /* if comma is met then assume GI. cw, read hours and mi-    */
  /* nutes, and convert all to double.                         */

  /* else check for do, and return -1                          */

  ps      = in;
  ac      = sscanf(ps, "%lf%c%n", &cw, &c, &ch);
  *used   = ch;
  *term_c = c;
  if (ac == EOF) return(-2.0);

  if (ac != 0) {

    if (c != ',') {
      /* more than 12 digits found, and no ',' */
      /* assume MD. comment word or jnsd      */

      return (cw);
    }
    else {

      (void) sscanf(ps, "%d%c%n", &w1, &c, &ch);

      /* comma found in input, assume G.I. date_time */
      ps += ch;

      /* read hours and next char */
      ac  = sscanf(ps, "%d%c%n", &ho, &c, &ch);

      if (ac == 2 && c == '.') {  /* read minutes */
        *used += ch;
        ps    += ch;
        (void) sscanf(ps, "%d%c%n", &mi, &c, &ch);
        *used += ch;
      }
      else {  /* set to 0 */
        ho   = 0;
        mi   = 0;
      }



/* sgetcw.c version 1.0               # page 3   14 Jun 1994 13 18 */


      /* check for new/old date format */
      if (w1 > 1000000L) { /* new */
        ye =  w1 / 10000;
        mo = (w1 /   100) %  100;
        da =  w1 %   100;
      } else { /* old */
        ye =  w1 %   100  + 1900;
        mo = (w1 /   100) %  100;
        da =  w1 / 10000;
      }


/*
      if (w1 < 1000000) {
        ye =  w1 %   100  + 1900;
        mo = (w1 /   100) %  100;
        da =  w1 / 10000;
      } else {
        ye =  w1 / 10000;
        mo = (w1 /   100) %  100;
        da =  w1 %   100;
      }
*/
      /* check for proper values */
/*
      if (ye < 1701 || 2100 < ye ||
          mo <    0 ||   12 < mo ||
          da <    0 ||   31 < da ||
          ho <    0 ||   24 < ho ||
          mi <    0 ||   60 < mi) return (-2);

      cw = (double) ye * 100000000 +
          mo           *   1000000 +
          da           *     10000 +
          ho           *       100 + mi;

*/
      if ( 1700 < ye && ye < 2101
          &&  0 < mo && mo <   13
          &&  0 < da && da <   32
          && -1 < ho && ho <   25
          && -1 < mi && mi <   61 )
         cw = mi + 1e2*(ho + 1e2*(da + 1e2*(mo + 1e2*ye)));
      else
      return(-2.0);

      *term_c = c;
      return (cw);
    }
  }

  else {
    /* no comma found in input, test for do */

    (void) sscanf(in, " %2s%n", char_do, &ch);
    if  (!strncmp(char_do, "do", 2)) {
      *used   = ch;
      *term_c = c;
      return (-1.0);
    } else return(-2.0);
  }

}


