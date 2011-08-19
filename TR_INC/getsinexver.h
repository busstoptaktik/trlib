

/* getsinexver  ver 2008.01        # page 1    1 Dec 2008 15 35 */


/* Copyright (c) 2008 Danish National Spacecenter.             */
/* All rights reserved.  */

/* This is unpublished proprietary source code of Danish       */
/* National Spacecenter.  This copyright claim does not        */
/* indicate an intention of publishing this code.              */


extern void        getsinexver(
/*___________________________*/
char              *ftx,
int               *state,
int               *version); 

/* getsinexver sets the version number to the text ftx          */

/* *ftx         :: 1 :: SINEX HEADER                            */
/* *state       :: 1 :: at input                                */
/* version      :: number ver*100+serial to be written on ftx   */

