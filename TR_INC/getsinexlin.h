

/* getsinexlin  ver 2008.01        # page 1    1 Dec 2008 15 35 */


/* Copyright (c) 2008 Danish National Spacecenter.             */
/* All rights reserved.  */

/* This is unpublished proprietary source code of Danish       */
/* National Spacecenter.  This copyright claim does not        */
/* indicate an intention of publishing this code.              */


extern int        getsinexlin(
/*___________________________*/
char             *ftx,
int              *state,
FILE             *fh_in);


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
