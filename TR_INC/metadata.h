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
 


/* conv_lab  ver 2010.01            # page 1    12 jan 2010 17 25 */


/* Copyright (c) 1998  LG,  Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#if !defined     (H_METADATA)
#define           H_METADATA

#include "geo_lab.h"
#include "geoid_d.h"

struct gd_state{
	struct coord_lab    *i_lab, *o_lab; 
	struct coord_lab    H0_lab;
	struct coord_lab    H2_lab, H3_lab; /* grid cstm dh*/
	struct coord_lab   *i_Rlab;  /* beg REG of ETPL  */
	struct coord_lab   *G_Rlab;  /* end REG of E geoid */
	struct coord_lab   *T_Rlab;  /* end REG of ETPL dh */
	struct coord_lab   *O_Rlab;  /* end REG of ETPL tr */
	struct coord_lab   *i_Nlab;  /* beg NON of ETPL  */
	struct coord_lab   *G_Nlab;  /* end NON of E geoid */
	struct coord_lab   *T_Nlab;  /* end NON of ETPL dh */
	struct coord_lab   *O_Nlab;  /* end NON of ETPL tr */
	struct coord_lab    t_lab;   /* non-reg gateway */
	struct coord_lab    g_lab;   /* geo_* PRE/ANT   */
        char   geoid_name[MLBLNG];
        struct mgde_str    *grid_tab;
	/*struct mgde_str  *s_grid_tab = NULL;*/
	struct mgde_str     h_grid_tab;
	struct htr_c_str    htr_const;
	int                 b_lev, s_lev;
	short               iEh_req, oEh_req;
	short               s_req_dh;
  /*                  START, STOP, ProjToProj*/
	short               sta[3], stp[3], ptp[3];
	char                i_sep, nonp_i[3];
	char                o_sep, nonp_o[3];
	/*handle to non-standard transformations*/
	int      (*dfb_trf)(
		struct coord_lab      *i_lab,
		struct coord_lab      *o_lab,
		double              N,    double   E,    double   H,
		double             *Nout, double  *Eout, double  *Hout,
		char               *usertxt,
		FILE               *tr_error
	);
  };

typedef struct gd_state gd_state;

gd_state                      *gd_open(
/*___________________________*/
struct coord_lab            *i_lab,
struct coord_lab            *o_lab,
char                            *special_table
);

void gd_close(gd_state *self);
struct mgde_str *gd_global_stdgeoids(int open_g);
struct mgde_str *gd_global_fehmarngeoid(int open_g);
struct mgde_str *gd_global_fbeltgeoid(int open_g);

int conv_w_crd(char *mlb, struct coord_lab *c_lab);
int conv_w_tab(char *mlb, struct gde_lab *t_lab);
int conv_w_plm(char *mlb, struct plm_lab *p_lab);
int srch_def(int srch_type, char *p_sys, struct lab_def_str *p_lb);


/*    result ==  -2     : def_prj.txt file not correct content */
/*    result ==  -1     : def_prj.txt file not found           */
/*    result ==   0     : no lab                               */
/*    result == CRD_LAB : coord lab                    in lab  */
/*    result == HGT_LAB : height lab                   in lab  */
/*    result == GDE_LAB : geoid                        in lab  */
/*    result == WOR_LAB : wor lab                      in lab  */
/*    result == POC_LAB : Photo OC lab                 in lab  */
/*    result == DTM_LAB : Digital Terrain ModeL lab    in lab  */
/*    result == DHH_LAB : Diff Height Model lab        in lab  */
/*    result == UPL_LAB : landUPLift model lab         in lab  */
/*    result == OBS_LAB : obs lab                      in lab  */
/*    result == DEM_LAB : Digital Elevation ModeL lab  in lab  */
/*    result == TAB_LAB : unspecified TABle lab        in lab  */
/*    result == IDT_LAB : idt lab                      in lab  */
/*    result == JDF_LAB : jdf lab                      in lab  */
/*    UNDEFSYS     : unrecognized name                         */
/*    result == STP_LAB : stop lab                             */
/* an unrecognized name is considered a filename containing a  */
/* genuine label                                               */


/* Parameters

lab_name              call               *char
The mini_label string of the system. See labels below
extra info may be read from 3rd parameter except :
special :
lab_name == "input"  : from file <3rd parameter>
lab_name == "in_u_#" : from file <3rd parameter> (no #)  1)
lab_name == "output" :   to file <3rd parameter>
lab_name == "OUTPUT" :   to file <3rd parameter>
lab_name == "?"      : ordinary info                     2)
lab_name == "*"      : detailed info                     2)
lab_name == "&"      : separated info                    2)
special special :
lab_name may be a filename containing a label.

1) only used internally in recursive mode.
2) for documentary purposes only.

g_lab                 return             *union geo_lab
The full label after a succesfull conversion of the 
lab_name. See geo_lab.h.

Prog: KP, KE, FS.

*/



/* conv_lab  ver 2010.01            # page 2    12 jan 2010 17 25 */


/*   ABOUT  LABELS

LABELS read from a <file> must begin with #


All labels may be written in a file which name is different from
the first caracters of a label name. Special coordinate labels
having extra parametres may then be read from the file.
conv_lab reads from the file <filename> when this is named as a
label. Obscure errors may occure when an erroneous <label name> is
written and this <label name> allso exists as a file name.

              STOP             
             ******
#stop will stop further reading from the file


              COORDINATES / HEIGHTS
             ***********************

Coordinate labels have usually a datum definition associated ::
   _ed50             European Datum 1950
   _euref89          Euref Datum 1989
  (_wgs84            == _euref89)
Special coordinate systems has an implicit datum definition
example : System 1934.  (_ed50)

A coordinate label may include a height by ::
-  substituting _ in the datum definition by : H or E
-  concatenating a special label by          : H or E

   E       means Heights above the ellipsoid
   H       means Heights above the zero reference surface
           the coordinate label is then concatenated by :
           _<height label>
   N       is at present converted to H and concatenation  _h_msl

Labels ::
   s34j                        System 34 Jylland
   s34jH_h_msl                  System 34 Jylland and Normal heights.
   s34jE                        System 34 Jylland and Ellips. heights.
                        
   s34s                        System 34 Sjælland

   s34b                        System 34 Bornholm
   s45b                        System 34 Bornholm

   u32                          UTM zone 32 ED50
   utm33_ED50                   UTM zone 33 ED50
   utm32Eeuref89                UTM 32 euref89 and Ellips. heights.
   utm33Heuref89_h_msl          UTM 33 euref89 and Normal heights.
   .
   .
*/




/*  (LABELS CONTINUED)

   geo_euref89                 Geographic EUREF89
   geoHeuref89_h_dvr90         Geographic EUREF89 and Normal heights.
   geoEeuref89                 Geographic EUREF89 and Ellips. heights.

   geo_ed50                    Geographic ED50

   crt_ed50                    Cartesic ED50
   crt_euref89                 Cartesic EUREF89

   h_dvr90                     Othometric heights DVR 1990
   h_gm91                      Othometric heights G.M. 1891 (Jylland)
   h_gi44                      Othometric heights G.I. 1944 (Fyn/Sjl.)
   DK_h_msl                    Othometric heights in known local system
   DK_h_kn44                   Koebenhavns Heights e.a.
   DK_ellh_euref89             Ellipsoid heights in euref89.

For KMS computers ::
special labels and observation labels may be seen by 
hjaelp labels

*/

#endif
