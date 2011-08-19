

/* getsinexver  ver 2008.01        # page 1    1 Dec 2008 15 35 */


/* Copyright (c) 2008 Danish National Spacecenter.             */
/* All rights reserved.  */

/* This is unpublished proprietary source code of Danish       */
/* National Spacecenter.  This copyright claim does not        */
/* indicate an intention of publishing this code.              */

/* getsinexver sets the version number to the text ftx          */

/* *ftx         :: 1 :: SINEX HEADER                            */
/* *state       :: 1 :: at input                                */
/* version      :: number ver*100+serial to be written on ftx   */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* getsinexver  ver 2008.01        # page 1    1 Dec 2008 15 35 */


void               getsinexver(
/*___________________________*/
char              *ftx,
int               *state,
int               *version) 
{
  double ver;

  if (*state == 1) {
    sscanf(ftx+6, "%lf", &ver);
    *version = (int) ver * 100;     // SINEX HEADER LINE
    if (*version != 210 && *version > 202) *state = -9999;
    // versions up 2.02 (== 2.10) is accepted
  }
  else *state = -9999;
  return;
}
