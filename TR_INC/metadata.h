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

struct def_dtm_shift{
	int dno1;
	int dno2;
	char *mlb1; 
	char *mlb2;
	double len; /*a weight*/
	int n_tabs;
	GRIM *g;  /*a sequence of methods - possibly spanning different areas...*/
};




typedef struct def_dtm_shift def_dtm_shift;
typedef struct def_grs def_grs;
typedef struct def_datum def_datum;
typedef struct def_projection def_projection;
typedef struct def_rgn def_rgn;
typedef struct def_alias def_alias;

typedef struct {
	int n_shifts;
	def_dtm_shift *shifts[4];
	int direction[4]; /*1 or -1*/
	double len;
	struct coord_lab *clab;
} htr_route;


struct def_data{
	def_projection *projections;
	def_datum      *datums;
	def_grs    *ellipsoids;
	def_dtm_shift *dtm_shifts;
	def_rgn     *regions;
	def_alias *alias_table;
	int n_prj;
	int n_dtm;
	int n_ellip;
	int n_rgn;
	int n_dtm_shifts;
	int n_alias;
};

typedef struct def_data def_data;

def_data *open_def_data(struct tag *root, char *dir, int *n_err);
void close_def_data(def_data *data); 

typedef struct{
	/*GRIM  geoid_seq_std[10];*/
	GRIM *additional_tables;
	int n_additional;
	def_data   *def_lab;
	char dir[256];
} tab_dir;

tab_dir *tab_dir_open(char *path);
void tab_dir_close(tab_dir *self);




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
	struct coord_lab *lab_in, *lab_out;
	struct coord_lab *tcgeo_in, *tcgeo_out; /*should be freed*/
	int start,stop;
	int dtm_in,dtm_out,hdtm_in,hdtm_out;
	htr_route *geoid_route;
	htr_route *dh_route;
	TRANSF_FCT trf_in, trf_out;
	tab_dir *tdir;
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
int conv_w_crd(char *mlb, struct coord_lab *c_lab, def_data *data);
int srch_def(int srch_type, char *p_sys, struct lab_def_str *p_lb, def_data *data);
int find_htr_route(int dtm1,int dtm2,htr_route *route,def_dtm_shift *shifts, int n_shifts);
char *attach_pom_extension(char *dir, char *in);
int get_mlb(char *mlb_str, char *mlb1, short *sepch, char *mlb2, char **h_mlb);
int conv_crd2(struct lab_def_str *p_lb, struct coord_lab *c_lab, def_data *DEF_DATA, char *p_io);
int conv_hgt2(struct lab_def_str *p_lb_ex,struct hgt_lab *h_lab,def_data  *DEF_DATA);




#endif
