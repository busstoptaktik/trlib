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
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <ctype.h>
#include    "geo_lab.h"
#include    "sgetg.h"
#include    "lord.h"
#include    "parse_xml.h"
#include    "metadata.h"


/*Values  below copy-pasted from def_lab.txt */
#define KM_HERE       (3986005.e8)
#define OMEGA_HERE (7292115.0e-11)
#define GEQ_HERE     (978049.e-5)
#define GEQ84_HERE  (979764.5e-5)
#define KMW84_HERE (3986004.418e8)
#define KMMOON       KM_HERE*0.0122298 /* Moon parameter relative to Earth - fix of '*' char in def_lab which was interpreted as a */
#define SKIP_SPACE(pos) while(isspace(*pos)) pos++

static int set_ellips(struct tag *grs_tag,  def_grs *ellip);
static int int_converter(char *in, void *out, size_t buf_size, void *extra);
static int double_converter(char *in, void *out, size_t buf_size, void *extra);
static int set_projection(struct tag *prj_tag,  def_projection *proj);
static int set_datum(struct tag *dtm_tag, def_datum *dtm);
static int set_dtm_shift(struct tag *dtm_shift_tag, def_dtm_shift *dtm_shift, char *dir_name);
static int set_rgn(struct tag *rgn_tag, def_rgn *rgn);
static int set_alias(struct tag *alias_tag, def_alias *alias);
//static char BUF[256];

/* 'single' int converter. A 'sep-char' could be input to extra, if more values were to be extracted from a string...*/
static int int_converter(char *in, void *out, size_t buf_size, void *extra){
	char *end;
	int *pos= (int*) out;
	SKIP_SPACE(in);
	*pos=(int) strtol(in,&end,0);
	return (end>in)? 1:0;
}

static int double_converter(char *in, void *out, size_t buf_size, void *extra){
	char *end;
	double *pos= (double*) out;
	struct typ_dec type_in;
	int used;
	SKIP_SPACE(in);
	if (extra==NULL){
		*pos=strtod(in, &end);
		return (end>in)?1:0;
	}
	*pos=sgetg(in,&type_in,&used,(char*) extra);
	return (type_in.gf>0)? 1: 0;
}



static int set_ellips(struct tag *grs_tag,  def_grs *ellip){
	struct tag t;
	static char BUF[256]; /*crashes in optimized mode for MSVC whenever we have local vars defined on the stack!!!*/
	t=get_named_child(grs_tag,"mlb");
	if (!get_value_as_string(&t,ellip->mlb,MLBLNG))
		return 0;
	t=get_named_child(grs_tag,"id");
	if (1!=get_value(&t,&ellip->no,1,int_converter,NULL))
		return 0;
	t=get_named_child(grs_tag,"mode");
	if (1!=get_value(&t,&ellip->mode,1,int_converter,NULL))
		return 0;
	t=get_named_child(grs_tag,"semi_major_axis");
	if (1!=get_value(&t,&ellip->axis,1,double_converter,NULL))
		return 0;
	t=get_named_child(grs_tag,"flattening");
	if (1!=get_value(&t,&ellip->flattening,1,double_converter,NULL))
		return 0;
	
	t=get_named_child(grs_tag,"gravity");
	if (!get_value_as_string(&t,BUF,128))
		return 0;
	if (!isalpha(BUF[0])){
		if (1!=double_converter(BUF,&ellip->km,1,NULL))
			return 0;
	}
	else if (!strcmp(BUF,"KM"))
		ellip->km=KM_HERE;
	else if (!strcmp(BUF,"KMMOON"))
		ellip->km=KMMOON;
	else if (!strcmp(BUF,"GEQ"))
		ellip->km=GEQ_HERE;
	else if (!strcmp(BUF,"KMW84"))
		ellip->km=KMW84_HERE;
	else if (!strcmp(BUF,"GEQ84"))
		ellip->km=GEQ84_HERE;
	else
		return 0;
	t=get_named_child(grs_tag,"rotation");
	if (!get_value_as_string(&t,BUF,128))
		return 0;
	if (!isalpha(BUF[0])){
		if (1!=double_converter(BUF,&ellip->omega,1,NULL))
			return 0;
	}
	else if (!strcmp(BUF,"OMEGA"))
		ellip->omega=OMEGA_HERE;
	else
		return 0;
	return 1;
}



static int set_projection(struct tag *prj_tag,  def_projection *proj){
	struct tag t;
	t=get_named_child(prj_tag,"description");
	if (!get_value_as_string(&t,proj->descr,MAX_DSCR_LEN))
		*(proj->descr)='\0';
	t=get_named_child(prj_tag,"mlb");
	if (!get_value_as_string(&t,proj->mlb,MLBLNG))
		return 0;
	t=get_named_child(prj_tag,"cha_str");
	if (1!=get_value(&t,&proj->cha_str,1,int_converter,NULL))
		return 0;
	t=get_named_child(prj_tag,"type");
	if (1!=get_value(&t,&proj->type,1,int_converter,NULL))
		return 0;
	t=get_named_child(prj_tag,"cstm");
	if (1!=get_value(&t,&proj->cstm,1,int_converter,NULL))
		return 0;
	t=get_named_child(prj_tag,"mode");
	if (1!=get_value(&t,&proj->mode,1,int_converter,NULL))
		return 0;
	t=get_named_child(prj_tag,"mask");
	if (1!=get_value(&t,&proj->mask,1,int_converter,NULL))
		return 0;
	t=get_named_child(prj_tag,"kms_seq");
	if (!get_value_as_string(&t,proj->seq,4))
		strcpy(proj->seq,"ZZ");
	t=get_named_child(prj_tag,"native");
	if (!get_value_as_string(&t,proj->native_proj,MLBLNG))
		strcpy(proj->native_proj,proj->mlb);
	t=get_named_child(prj_tag,"region");
	if (!get_value_as_string(&t,proj->rgn,3))
		strcpy(proj->rgn,"ZZ");
	t=get_named_child(prj_tag,"n_parameters");
	if (1!=get_value(&t,&proj->q_par,1,int_converter,NULL))
		return 0;
	t=get_named_child(prj_tag,"parameters");
	if (!get_value_as_string(&t,proj->param_text,128))
		*proj->param_text='\0';
	else{ /*parse the parameters*/
		int i,used;
		char *pos=proj->param_text;
		struct typ_dec type;
		for(i=0; i<proj->q_par; i++){
			proj->native_params[i]=sgetg(pos,&type,&used,"m"); /*TODO: fix this hack (reading scale as m)*/
			if (type.gf==0){
				lord_error(TR_LABEL_ERROR,LORD("Failed to parse parameter."));
				return 0;
			}
			pos+=used;
		}
	}
	t=get_named_child(prj_tag,"parent_datum");
	if (!get_value_as_string(&t,proj->p_datum,16))
		strcpy(proj->p_datum,"\"");
	
	strcpy(proj->param_tokens,"\"");
	if ((proj->q_par)>0 && strlen(proj->param_text)==0){
		t=get_named_child(prj_tag,"parameter_tokens");
		if (!get_value_as_string(&t,proj->param_tokens,32))
			return 0;
	}
	return 1;
}
	
static int set_datum(struct tag *dtm_tag, def_datum *dtm){
	struct tag t;
	t=get_named_child(dtm_tag,"mlb");
	if (!get_value_as_string(&t,dtm->mlb,MLBLNG))
		return 0;
	t=get_named_child(dtm_tag,"description");
	if (!get_value_as_string(&t,dtm->descr,MAX_DSCR_LEN))
		*dtm->descr='\0';
	t=get_named_child(dtm_tag,"id");
	if (1!=get_value(&t,&dtm->no,1,int_converter,NULL))
		return 0;
	dtm->p_no=-1;
	t=get_named_child(dtm_tag,"parent");
	if (!get_value_as_string(&t,dtm->p_datum,MLBLNG))
		return 0;
	t=get_named_child(dtm_tag,"ellipsoid");
	if (!get_value_as_string(&t,dtm->ellipsoid,MLBLNG))
		return 0;
	t=get_named_child(dtm_tag,"imit");
	if (1!=get_value(&t,&dtm->imit,1,int_converter,NULL))
		return 0;
	t=get_named_child(dtm_tag,"region");
	if (!get_value_as_string(&t,dtm->rgn,3))
		strcpy(dtm->rgn,"ZZ");
	t=get_named_child(dtm_tag,"type");
	if (1!=get_value(&t,&dtm->type,1,int_converter,NULL))
		return 0;
	if (dtm->type!=0 && dtm->type!=1){
		t=get_named_child(dtm_tag,"t_x");
		if (1!=get_value(&t,dtm->translation,1,double_converter,NULL))
			return 0;
		t=get_named_child(dtm_tag,"t_y");
		if (1!=get_value(&t,dtm->translation+1,1,double_converter,NULL))
			return 0;
		t=get_named_child(dtm_tag,"t_z");
		if (1!=get_value(&t,dtm->translation+2,1,double_converter,NULL))
			return 0;
		if (dtm->type!=3){
			t=get_named_child(dtm_tag,"rot_x");
			if (1!=get_value(&t,dtm->rotation,1,double_converter,"sx"))
				return 0;
			t=get_named_child(dtm_tag,"rot_y");
			if (1!=get_value(&t,dtm->rotation+1,1,double_converter,"sx"))
				return 0;
			t=get_named_child(dtm_tag,"rot_z");
			if (1!=get_value(&t,dtm->rotation+2,1,double_converter,"sx"))
				return 0;
			t=get_named_child(dtm_tag,"scale");
			if (1!=get_value(&t,&dtm->scale,1,double_converter,"ppm"))
				return 0;
		} /*end should have 7 parameters */
	} /*end should have datum shift parameters */
	return 1;
}

static int set_dtm_shift(struct tag *dtm_shift_tag, def_dtm_shift *dtm_shift, char *dir_name){
	struct tag t;
	int i,n_ok;
	static char file_name[512];
	static char mlb[MLBLNG];
	static int degree;
	static	double coefficients[4];
	static	double lon_0=0.0,lat_0=0.0;
	GRIM g;
	
	t=get_named_child(dtm_shift_tag,"from");
	if (1!=get_value(&t,&dtm_shift->dno1,1,int_converter,NULL))
		return 0;
	
	t=get_named_child(dtm_shift_tag,"to");
	if (!get_value(&t,&dtm_shift->dno2,1,int_converter,NULL))
		return 0;
	
	
	t=get_named_child(dtm_shift_tag,"file");
	if (t.valid){
		if (!get_value_as_string(&t,file_name,128))
			return 0;
		g=grim_open(attach_pom_extension(dir_name,file_name));
		if (g==NULL)
			return 0; /*TODO: check for POM proj_mlb field name is set*/
	}
	else{
		
		
		mlb[0]='\0';
		
		t=get_named_child(dtm_shift_tag,"mlb");
		if (!get_value_as_string(&t,mlb,MLBLNG))
			return 0;

		t=get_named_child(dtm_shift_tag,"htr_constant");
		if (t.valid) {
			if (1!=get_value(&t,coefficients,1,double_converter,NULL))
				return 0;
			coefficients[1]=0;
			coefficients[2]=0;
			coefficients[3]=0;
			degree = 0;
		}
		else {
			struct tag s;
			t=get_named_child(dtm_shift_tag,"htr_constants");
			if (t.valid) {
				s=get_named_child(&t,"lat_0");
				if (1!=get_value(&s,&lat_0,1,double_converter,"nt"))
					return 0;
				s=get_named_child(&t,"lon_0");
				if (1!=get_value(&s,&lon_0,1,double_converter,"nt"))
					return 0;
				s=get_named_child(&t,"k0");
				if (1!=get_value(&s,coefficients,1,double_converter,NULL))
					return 0;
				s=get_named_child(&t,"M0");
				if (1!=get_value(&s,coefficients+1,1,double_converter,NULL))
					return 0;
				s=get_named_child(&t,"kN");
				if (1!=get_value(&s,coefficients+2,1,double_converter,"sx"))
					return 0;
				coefficients[1]*=coefficients[2];
				s=get_named_child(&t,"N0");
				if (1!=get_value(&s,coefficients+2,1,double_converter,NULL))
					return 0;
				s=get_named_child(&t,"kE");
				if (1!=get_value(&s,coefficients+3,1,double_converter,"sx"))
					return 0;
				coefficients[2]*=coefficients[3];
				coefficients[3]=0;
				degree=1;
			}
			else
			return 0;
		}
		g=grim_polynomial(lat_0,lon_0,coefficients, 3, 1, degree, mlb);
	}
	if (g==NULL)
		return 0;
	dtm_shift->g=calloc(sizeof(GRIM),1);
	dtm_shift->n_tabs=1;
	dtm_shift->g[0]=g;
	dtm_shift->len=1; /*TODO*/
	lord_debug(0,LORD("DATUM SHIFT from: %d, to: %d, mlb: %s, adress: %p"),dtm_shift->dno1,dtm_shift->dno2,grim_crs(dtm_shift->g[0]),dtm_shift->g[0]);
	lord_debug(0,LORD("rows: %d, columns: %d"),grim_rows(dtm_shift->g[0]),grim_columns(dtm_shift->g[0]));
	return 1;
}

static int set_alias(struct tag *alias_tag, def_alias *alias){
	struct tag t;
	t=get_named_child(alias_tag,"key");
	if (!get_value_as_string(&t,alias->key,MLBLNG))
		return 0;
	t=get_named_child(alias_tag,"value");
	if (!get_value_as_string(&t,alias->value,MLBLNG))
		return 0;
	return 1;
}

static int set_rgn(struct tag *rgn_tag, def_rgn *rgn){
	struct tag t;
	t=get_named_child(rgn_tag,"new");
	if (!get_value_as_string(&t,rgn->rgn_new,3))
		return 0;
	t=get_named_child(rgn_tag,"old");
	if (!get_value_as_string(&t,rgn->rgn_old,3))
		return 0;
	t=get_named_child(rgn_tag,"country");
	if (!get_value_as_string(&t,rgn->country,64))
		return 0;
	return 1;
}

/* 
Function that parses def_lab file. Now in XML :-)
*/
#define N_MODES  (6)
 def_data *open_def_data(struct tag *root, char *dir_name, int *n_err){
	/*stuff that match the formatting and 'modes' of the def_lab file */
	struct tag def_tag,item_tag;
	def_data *data=NULL;
	int n_set[N_MODES]={0,0,0,0,0,0},mode,i;
	enum modes {mode_prj=0,mode_rgn=1,mode_dtm=2,mode_alias=3,mode_dtm_shift=4,mode_grs=5};
	void *entries[N_MODES]={NULL,NULL,NULL,NULL,NULL,NULL};
	char *mode_names[N_MODES]={"def_prj","def_rgn","def_dtm","def_alias","def_dtm_shift","def_grs"};
	char *item_names[N_MODES]={"prj","rgn","dtm","alias","dtm_shift","grs"};
	/* specifications for how much to preallocate */
	int n_prealloc[N_MODES]={256,128,128,20,128,64};
	int n_alloc[N_MODES]={256,128,128,20,128,64};
	size_t mode_sizes[N_MODES]={sizeof( def_projection),sizeof(def_rgn),sizeof( def_datum), sizeof(def_alias), sizeof( def_dtm_shift),sizeof( def_grs)};
	/*printf("def_grs: %d\n",sizeof(def_grs));*/
	*n_err=0; /* set no errors - yet! */
	/* allocate memory for objects */
	for(mode=0;mode<N_MODES;mode++){
		entries[mode]=malloc(n_prealloc[mode]*mode_sizes[mode]);
		if (entries[mode]==NULL)
			goto error;
	}
	data=malloc(sizeof( def_data));
	if (data==NULL){
		goto error;
	}
	if (!(root->valid) || !compare_tag(root,"def_lab")){
		lord_error(XML_MALFORMED,LORD("Seems to be an invalid def-file.."));
		goto error;
	}
	/*iterate over modes*/
	for(mode=0; mode<N_MODES; mode++){
		lord_debug(0,LORD("Hello - mode is : %d"),mode);
		def_tag=get_named_child(root,mode_names[mode]);
		if (!def_tag.valid){
			lord_error(XML_ROOT_NOT_FOUND,LORD("Failed to find tag %s."),mode_names[mode]);
			goto error;
		}
		item_tag=get_next_child(&def_tag,NULL);
		while(item_tag.valid){
			if (!compare_tag(&item_tag,item_names[mode])){
				lord_error(XML_MALFORMED,LORD("Unexpected tag (not %s) in mode %s"),item_names[mode],mode_names[mode]);
				*n_err++;
			}
			else{
				int ok=0;
				void *entry=entries[mode];
				/*lord_debug(0,LORD("mode: %d, n_set: %d"),mode,n_set[mode]);*/
				switch(mode){
					case mode_prj:
						ok=set_projection(&item_tag,(def_projection*) entry+n_set[mode]);
						break;
					case mode_rgn:
						ok=set_rgn(&item_tag, (def_rgn*) entry+n_set[mode]);
						break;
					case mode_dtm:
						ok=set_datum(&item_tag, (def_datum*) entry+n_set[mode]);
						break;
					case mode_alias:
						ok=set_alias(&item_tag, (def_alias*) entry+n_set[mode]);
						break;
					case mode_dtm_shift:
						ok=set_dtm_shift(&item_tag, (def_dtm_shift*) entry+n_set[mode], dir_name);
						break;
					case mode_grs:
						ok=set_ellips(&item_tag,(def_grs*) entry+n_set[mode]);
						break;
					default:
						lord_error(TR_LABEL_ERROR,LORD("Invalid mode."));
						goto error;
					
				} /*end switch */
				if (!ok){
					lord_error(TR_LABEL_ERROR,LORD("Invalid xml format of entry in mode %d"),mode);
					goto error;
				}
				else{
					n_set[mode]++;
					/* allocate more space if needed*/
					if (n_set[mode]==n_alloc[mode]){
						void *more_space;
						more_space=realloc(entries[mode],mode_sizes[mode]*(n_alloc[mode]+n_prealloc[mode]));
						if (more_space!=NULL){
							n_alloc[mode]+=n_prealloc[mode];
							entries[mode]=more_space;
						}
						else{
							lord_error(TR_ALLOCATION_ERROR,LORD("Failed to allocate space"));
							goto error;
						}
					}
				}
			
			}
			item_tag=get_next_child(&def_tag,&item_tag);
		} /* end iteration over items */ 
		if (n_set[mode]==0){
			lord_error(TR_LABEL_ERROR,LORD("No items defined in mode %s"),mode_names[mode]);
			goto error;
		}
	}/*end mode iteration*/
	/*reallocate sizes of objects */
	for(i=0;i<N_MODES;i++)
		entries[i]=realloc(entries[i],mode_sizes[i]*n_set[i]);
	/*set parent no of datums*/
	
	{
		def_datum *dtm1,*dtm2;
		int j;
		for(i=0;i<n_set[mode_dtm]; i++){
			dtm1=((def_datum*) entries[mode_dtm])+i;
			/*dtm1->p_no=-1;*/
			for (j=0;j<n_set[mode_dtm];j++){
				dtm2=((def_datum*) entries[mode_dtm])+j;
				if (!strcmp(dtm2->mlb,dtm1->p_datum)){
					dtm1->p_no=dtm2->no;
					break;
				}
			}
			if (dtm1->p_no<=0){
				lord_debug(0,LORD("Could not find p_datum: %s"),dtm1->p_datum);
				*n_err++;
			}
		}
	}
	
	/*transfer stuff to data object */
	data->n_prj=n_set[mode_prj];
	data->n_rgn=n_set[mode_rgn];
	data->n_ellip=n_set[mode_grs];
	data->n_dtm=n_set[mode_dtm];
	data->n_dtm_shifts=n_set[mode_dtm_shift];
	data->n_alias=n_set[mode_alias];
	data->projections=entries[mode_prj];
	data->datums=entries[mode_dtm];
	data->ellipsoids=entries[mode_grs];
	data->regions=entries[mode_rgn];
	data->dtm_shifts=entries[mode_dtm_shift];
	data->alias_table=entries[mode_alias];
	lord_debug(0,LORD("n_prj: %d, n_rgn: %d, n_ellip: %d, n_dtm: %d, n_shifts: %d, n_alias: %d"),data->n_prj,data->n_rgn,data->n_ellip,data->n_dtm,
	data->n_dtm_shifts,data->n_alias);
	/* perhaps sort entries after number?? */
	//present_data(stdout,data);
	return data;
	error:
	    (*n_err)++;
	    for(i=0;i<5;i++)
	        free(entries[i]);
	    free(data);
	    return NULL;
}

void close_def_data( def_data *data){
	int i;
	if (!data)
		return;
	free(data->projections);
	free(data->datums);
	free(data->ellipsoids);
	free(data->regions);
	for (i=0; i<data->n_dtm_shifts;i++){
		grim_close(data->dtm_shifts[i].g[0]); /*TODO: close all*/
		free(data->dtm_shifts[i].g);
	}
	free(data->dtm_shifts);
	free(data->alias_table);
	free(data);
	return;
}
