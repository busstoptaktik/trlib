/*
* Copyright (c) 2012, National Survey and Cadastre, Denmark
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
 
#if !defined(H_PARSE_DEF_FILE)
#define      H_PARSE_DEF_FILE


 #include "geo_lab.h"
 #define MAX_TABLE_LEN (32)
 #define MAX_DSCR_LEN (128)
 
struct def_grs {
	char mlb[MLBLNG];
	short no;
	short mode;
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
	char from_mlb[MLBLNG];
	char to_dtm[MLBLNG];
	int ih_dtm;
	int oh_dtm;
	int type;
	double B0;
	double L0;
	double constants[5];
	char table[MAX_TABLE_LEN];
	char descr[MAX_DSCR_LEN];
};
	

typedef struct def_hth_tr def_hth_tr;
typedef struct def_grs def_grs;
typedef struct def_datum def_datum;
typedef struct def_projection def_projection;
typedef struct def_rgn def_rgn;

struct def_data{
	def_projection *projections;
	def_datum      *datums;
	def_grs    *ellipsoids;
	def_hth_tr *hth_entries;
	def_rgn     *regions;
	int n_prj;
	int n_dtm;
	int n_ellip;
	int n_rgn;
	int n_hth;
};

typedef struct def_data def_data;

double get_number(char *item);
def_data *open_def_data(FILE *fp, int *n_err);
void close_def_data(def_data *data); 

#endif
