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
 


/* getsinexlin  ver 2008.01        # page 1    1 Dec 2008 15 35 */


/* Copyright (c) 2008 Danish National Spacecenter.             */
/* All rights reserved.  */

/* This is unpublished proprietary source code of Danish       */
/* National Spacecenter.  This copyright claim does not        */
/* indicate an intention of publishing this code.              */

/* getsinexline reads a line from teh open and ready file fh_in */
/* to the string ftx. The state gives info on which linetype is */
/* input and the return value is the number of chars input.     */

/* getsinexline :: number of chars input to ftx                 */
/* *ftx         :: char array for one input line                */
/* *state       :: input line type                              */
/* *fh_in       :: File pointer to an open and ready file       */

/* *state       :: -9999 error line type                        */
/*              :: 0<*state<200 :: begin block and info lines   */
/*              :: -200<*state<0 :: end block                   */
/*              :: 1000<*state<1200 :: comment line             */
/*       ::   1 :: SINEX HEADER                                 */
/*       :: 101 :: FILE/REFERENCE                               */
/*       :: 102 :: FILE/COMMENT                                 */
/*       :: 103 :: INPUT/HISTORY                                */
/*       :: 104 :: INPUT/FILES                                  */
/*       :: 105 :: INPUT/ACKNOWLEDGMENTS                        */
/*       :: 106 :: NUTATION/DATA                                */
/*       :: 107 :: PRECESSION/DATA                              */
/*       :: 108 :: SOURCE/ID                                    */
/*       :: 109 :: SITE/ID                                      */
/*       :: 110 :: SITE/DATA                                    */
/*       :: 111 :: SITE/RECEIVER                                */
/*       :: 112 :: SITE/ANTENNA                                 */
/*       :: 113 :: SITE/GPS_PHASE_CENTER                        */
/*       :: 114 :: SITE/GAL_PHASE_CENTER                        */
/*       :: 115 :: SITE/ECCENTRICITY                            */
/*       :: 116 :: SATELLITE/ID                                 */
/*       :: 117 :: SATELLITE/PHASE_CENTER                       */
/*       :: 118 :: BIAS/EPOCHS                                  */
/*       :: 119 :: SOLUTION/EPOCHS                              */
/*       :: 120 :: SOLUTION/STATISTICS                          */
/*       :: 121 :: SOLUTION/ESTIMATE                            */
/*       :: 122 :: SOLUTION/APRIORI                             */
/*       :: 123 :: SOLUTION/MATRIX_ESTIMATE                     */
/*       :: 124 :: SOLUTION/MATRIX_APRIORI                      */
/*       :: 125 :: SOLUTION/NORMAL_EQUATION_VECTOR              */
/*       :: 126 :: SOLUTION/NORMAL_EQUATION_MATRIX              */

#include <stdio.h>
#include <string.h>


/* getsinexlin  ver 2008.01        # page 1    1 Dec 2008 15 35 */


int               getsinexlin(
/*___________________________*/
char             *ftx,
int              *state,
FILE             *fh_in) 
{
  char  *ch_ptr = ftx;
  int    c, Stop = 0, res = 1, qChars = 0;

  do {
    c = fgetc(fh_in);
    if (c == EOF) Stop = 1; 
    else 
    if (c != '\0') {
      if (c == '\n') {    // skip nl
        Stop = (qChars >= 1);
      } 
      else {
        *(ch_ptr++) = (char) c;
        ++ qChars;
        if (qChars > 80) {
          res  = 0;
          Stop = 1;
        }
      }
    }
    else  Stop = 1;
  } while (!Stop);
  *ch_ptr = '\0';

  if (res && qChars) {
    switch (*ftx) {
    case '%': 
      if (strncmp(ftx, "%=SNX", 5) == 0) *state = 1;
      else
      if (strncmp(ftx, "%=ENDSNX", 8) == 0) *state = -1;
      else
      *state = -9999;
      break;
    case '+':
    case '-':
      *state = (strncmp(ftx +1, "FILE/REFERENCE", 14) == 0)                  ? 101
             : (strncmp(ftx +1, "FILE/COMMENT", 12) == 0)                    ? 102
             : (strncmp(ftx +1, "INPUT/HISTORY", 13) == 0)                   ? 103
             : (strncmp(ftx +1, "INPUT/FILES", 11) == 0)                     ? 104
             : (strncmp(ftx +1, "INPUT/ACKNOWLEDGMENTS", 21) == 0)           ? 105
             : (strncmp(ftx +1, "NUTATION/DATA", 13) == 0)                   ? 106
             : (strncmp(ftx +1, "PRECESSION/DATA", 15) == 0)                 ? 107
             : (strncmp(ftx +1, "SOURCE/ID", 9) == 0)                        ? 108
             : (strncmp(ftx +1, "SITE/ID", 7) == 0)                          ? 109
             : (strncmp(ftx +1, "SITE/DATA", 9) == 0)                        ? 110
             : (strncmp(ftx +1, "SITE/RECEIVER", 13) == 0)                   ? 111
             : (strncmp(ftx +1, "SITE/ANTENNA", 12) == 0)                    ? 112
             : (strncmp(ftx +1, "SITE/GPS_PHASE_CENTER", 21) == 0)           ? 113
             : (strncmp(ftx +1, "SITE/GAL_PHASE_CENTER", 21) == 0)           ? 114
             : (strncmp(ftx +1, "SITE/ECCENTRICITY", 17) == 0)               ? 115
             : (strncmp(ftx +1, "SATELLITE/ID", 12) == 0)                    ? 116
             : (strncmp(ftx +1, "SATELLITE/PHASE_CENTER", 22) == 0)          ? 117
             : (strncmp(ftx +1, "BIAS/EPOCHS", 11) == 0)                     ? 118
             : (strncmp(ftx +1, "SOLUTION/EPOCHS", 15) == 0)                 ? 119
             : (strncmp(ftx +1, "SOLUTION/STATISTICS", 19) == 0)             ? 120
             : (strncmp(ftx +1, "SOLUTION/ESTIMATE", 17) == 0)               ? 121
             : (strncmp(ftx +1, "SOLUTION/APRIORI", 16) == 0)                ? 122
             : (strncmp(ftx +1, "SOLUTION/MATRIX_ESTIMATE", 24) == 0)        ? 123
             : (strncmp(ftx +1, "SOLUTION/MATRIX_APRIORI", 23) == 0)         ? 124
             : (strncmp(ftx +1, "SOLUTION/NORMAL_EQUATION_VECTOR", 31) == 0) ? 125
             : (strncmp(ftx +1, "SOLUTION/NORMAL_EQUATION_MATRIX", 31) == 0) ? 126
             : -9999;
      if (*ftx == '-') *state = -*state;
      break;
    case ' ':
      if (*state > 1000) *state -= 1000;
      break;
    case '*':
      if (0 < *state && *state < 1000) *state += 1000;
      break;
    default:
      *state = -9999;
    }
  }
  return qChars;
}
