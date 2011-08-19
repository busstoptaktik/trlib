

/* htr_init  ver 2010            # page 1    24 Jul 2010 11 57 */


/* Copyright (c) 2003,   Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                     */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include    "geo_lab.h"

extern int               htr_init(
/*                              */
struct  coord_lab       *ih_dtm,
struct  coord_lab       *oh_dtm,
union  geo_lab          *htr_lab,
struct htr_c_str        *htr_const,
char                    *dh_table_name,
char                    *dh_tr_info
);

/*
Initiates the parameters for height transformation between 
Vertical reference frames.

htr_init = 1   : transformation by DHTAB
         = 2   : add a1 (inv : sub a1)
         = 3   : linear transformation
  errors::
            0  : NO transf
           -1  : def_lab.txt NOT FOUND/SYYNTAX ERROR
           -2  : def_lab.txt H_TR definition NOT FOUND
   TRF_ILLEG_  : in_lab error
  HTRF_ILLEG_  : h_transformation illegal

ih_dtm         : from datum
oh_dtm         :   to datum
*htr_lab       : label defining the coordsys for transformation
*inv           : Direct formula or inverse
*htr_const     ::
               inv    : Indirect or direct i.e. subtract/add
               LAT0   : Ref Latitude
               LON0   : Ref Longitude
               M0     : Curvature in Lat
               N0     : Curvature in Lon
               a1     : add konstant
               a2     : factor in Lat
               a3     : factor in Lon
*dh_table_name :  DHTAB name
*dh_tr_info    :  Ref. or description

*/

