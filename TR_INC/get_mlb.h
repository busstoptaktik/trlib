
/* get_mlb.h     ver 2003.01          # page   4 Jul 2003 14.06 */

/* Copyright (c) 1998 GD, Kort & Matrikelstyrelsen, Denmark    */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort &   */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */



extern int           get_mlb(
/*_________________________*/
char                *mlb_str,
short               *region,
char                *mlb1,
short               *sepch,
char                *mlb2,
char               **h_mlb
);

/* get_mlb :: returns the number af characters in the label.

The items of the label *mlb_str are put into the parameters:

*region    :: the twoo region characters.
*mlb1      :: the kind/projection part is copied to *mlb1.
*sepch     :: the seperatior char is copied to *sepch.
*mlb2      :: the type/datum part is copied to *mlb2.
**h_mlb    :: the pointer to the HGT_mlb is put in **h_mlb.
           :: NULL when sep_ch != H or no h_lab

*/



