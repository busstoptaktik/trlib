

/* sidefeed.h                        # page 1   20 Sep 1994 14 11 */


/* Copyright (c) 1994, GD, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

extern int         sidefeed(
/*________________________*/
struct o_fp_str   *ou_st
);

/* function : sidefeed 

sidefeed     produces a form feed and a page head.
             returns no of printed chars 

ou_st contains :

    o_fp          file receiving the print

    o_lc          the actual line no (not used).
    o_pg          the actual page no.
                  Increased by one before the printout

    out_time      giso_dt time to be written.
                  == 0 give a call of giso_dt, prn_dt is updated.
    o_head        array or pointer holding the headline text
    u_lab         union of a label   (not used).

*/


