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
#include "grim.h"
#include "parse_xml.h"

#define MAX_TABLE_LEN (32)
#define MAX_DSCR_LEN (128)

struct def_alias {
	char key[MLBLNG];
	char value[MLBLNG];
};






struct def_grs {
	char mlb[MLBLNG];
	int no;
	int mode;
	double axis;
	double flattening;
	double km;
	double omega;
};

struct def_rgn {
	char rgn_new[3];
	char rgn_old[3];
	char country[64];
};

struct def_datum {
	char mlb[MLBLNG];
	int no;
	char p_datum[MLBLNG];
	char ellipsoid[MLBLNG]; /*really a reference to a def_grs struct */
	int imit;
	int type;
	int p_no;
	char rgn[3];
	double translation[3];
	double rotation[3];
	double scale;
	char descr[MAX_DSCR_LEN];
	
};

struct def_projection{
	char mlb[MLBLNG];
	int cha_str, type, cstm, mode, mask;
	char seq[4];
	char rgn[3];
	char p_datum[16];
	int q_par;
	char param_tokens[32];
	char param_text[128];
	char native_proj[MLBLNG];
	double native_params[8];
	char descr[MAX_DSCR_LEN];
	
};

struct  def_hth_tr{
	char from[MLBLNG];
	char to[MLBLNG];
	int ih_dtm;
	int oh_dtm;
	GRIM g;
};

typedef struct def_hth_tr def_hth_tr;
typedef struct def_grs def_grs;
typedef struct def_datum def_datum;
typedef struct def_projection def_projection;
typedef struct def_rgn def_rgn;
typedef struct def_alias def_alias;

struct def_data{
	def_projection *projections;
	def_datum      *datums;
	def_grs    *ellipsoids;
	def_hth_tr *hth_entries;
	def_rgn     *regions;
	def_alias *alias_table;
	int n_prj;
	int n_dtm;
	int n_ellip;
	int n_rgn;
	int n_hth;
	int n_alias;
};

typedef struct def_data def_data;

def_data *open_def_data(struct tag *root, int *n_err);
void close_def_data(def_data *data); 

typedef struct{
	GRIM geoids[128];
	GRIM dhtabs[30];
	GRIM t3dtabs[30];
	GRIM  geoid_seq_std[10];
	int n_geoids,n_dhtabs,n_3dtabs,n_geoid_seq_std;
	def_data   *def_lab;
	char dir[128];
} tab_dir;

tab_dir *tab_dir_open(char *path);
void tab_dir_close(tab_dir *self);


typedef struct {
 GRIM *table_sequence;
 int n_tables;
 int cur_table;
 int free_sequence;
 int type;
}
table_adm_str;

typedef int( *TRANSF_FCT)(
struct coord_lab      *in_lab,
struct coord_lab      *outlab,
double              N,
double              E,
double              H,
double             *Nout,
double             *Eout,
double             *Hout,
tab_dir              *tdir
); 


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
        char   geoid_name[MLBLNG]; /* should  be used to something */
	tab_dir            *TAB_DIR;
        table_adm_str   *grid_tab;
	table_adm_str   *h_grid_tab;
	struct htr_c_str    htr_const;
	int                 b_lev, s_lev;
	short               iEh_req, oEh_req;
	short               s_req_dh;
  /*                  START, STOP, ProjToProj*/
	short               sta[3], stp[3], ptp[3];
	char                i_sep, nonp_i[3];
	char                o_sep, nonp_o[3];
	/*handle to non-standard transformations*/
	TRANSF_FCT dfb_trf;
		
  };

typedef struct gd_state gd_state;

gd_state                      *gd_open(
/*___________________________*/
struct coord_lab            *i_lab,
struct coord_lab            *o_lab,
char                            *special_table,
tab_dir                         *tdir
);

void gd_close(gd_state *self);
  
table_adm_str *table_adm_open(char *name, tab_dir *tdir);
GRIM *get_specific_table(char *name, int type, tab_dir *tdir);
int grid_val2(struct coord_lab*,table_adm_str*,double,double,double*);
  
/*
table_adm_str *gd_global_stdgeoids(int open_g);
table_adm_str *gd_global_fehmarngeoid(int open_g);
table_adm_str *gd_global_fbeltgeoid(int open_g);*/

int conv_w_crd(char *mlb, struct coord_lab *c_lab, def_data *data);
/*int conv_w_tab(char *mlb, struct gde_lab *t_lab);
int conv_w_plm(char *mlb, struct plm_lab *p_lab);*/
int srch_def(int srch_type, char *p_sys, struct lab_def_str *p_lb, def_data *data);




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
