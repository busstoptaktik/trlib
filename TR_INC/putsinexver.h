

/* putsinexver  ver 2008.01        # page 1    1 Dec 2008 15 35 */


/* Copyright (c) 2008 Danish National Spacecenter.             */
/* All rights reserved.  */

/* This is unpublished proprietary source code of Danish       */
/* National Spacecenter.  This copyright claim does not        */
/* indicate an intention of publishing this code.              */


extern void        putsinexver(
/*___________________________*/
char              *ftx,
int               *state,
int                version);

/* putsinexver sets the version number to the text ftx          */

/* *ftx         :: 1 :: SINEX HEADER                            */
/* *state       :: 1 :: at input                                */
/* *state       :: -9999 error line type                        */
/* version      :: number ver*100+serial to be written on ftx   */


