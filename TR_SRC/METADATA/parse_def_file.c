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
#include    "KmsFncs.h"
#include    "parse_xml.h"

/*Values  below copy-pasted from def_lab.txt */
#define KM_HERE       (3986005.e8)
#define OMEGA_HERE (7292115.0e-11)
#define GEQ_HERE     (978049.e-5)
#define GEQ84_HERE  (979764.5e-5)
#define KMW84_HERE (3986004.418e8)
#define KMMOON       KM_HERE*0.0122298 /* Moon parameter relative to Earth - fix of '*' char in def_lab which was interpreted as a */
#define SKIP_SPACE(pos) while(isspace(*pos)) pos++

static int set_ellipsoid(struct tag *grs_tag,  def_grs *ellip);
static int int_converter(char *in, int *out, size_t buf_size, void *extra);
static int double_converter(char *in, double *out, size_t buf_size, void *extra);
static int set_projection(struct tag *prj_tag,  def_projection *proj);
static int set_datum(struct tag *dtm_tag, def_datum *dtm);
static int set_hth(struct tag *hth_tag, def_hth_tr *hth_tr);
static int set_rgn(struct tag *rgn_tag, def_rgn *rgn);


/* 'single' int converter. A 'sep-char' could be input to extra, if more values were to be extracted from a string...*/
static int int_converter(char *in, int *out, size_t buf_size, void *extra){
	char *end;
	SKIP_SPACE(in);
	*out=(int) strtol(in,&end,0);
	return (end>in)? 1:0;
}

static int double_converter(char *in, double *out, size_t buf_size, void *extra){
	char *end;
	struct typ_dec type_in;
	int used;
	SKIP_SPACE(in);
	if (extra==NULL){
		*out=strtod(in, &end);
		return (end>in)?1:0;
	}
	*out=sgetg(in,&type_in,&used,(char*) extra);
	return (type_in.gf>0)? 1: 0;
}



static int set_ellipsoid(struct tag *grs_tag,  def_grs *ellip){
	char buf[128];
	if (!get_value_as_string(get_named_child(grs_tag,"mlb"),ellip->mlb,MLBLNG))
		return 0;
	if (1!=get_value(get_named_child(grs_tag,"id"),&ellip->no,1,int_converter,NULL))
		return 0;
	if (1!=get_value(get_named_child(grs_tag,"mode"),&ellip->mode,1,int_converter,NULL))
		return 0;
	if (1!=get_value(get_named_child(grs_tag,"semi_major_axis"),&ellip->axis,1,double_converter,NULL))
		return 0;
	if (1!=get_value(get_named_child(grs_tag,"flattening"),&ellip->flattening,1,double_converter,NULL))
		return 0;
	if (!get_value_as_string(get_named_child(grs_tag,"gravity"),buf,128))
		return 0;
	if (!isalpha(buf[0])){
		if (1!=double_converter(buf,&ellip->km,1,NULL))
			return 0;
	}
	else if (!strcmp(buf,"KM"))
		ellip->km=KM_HERE;
	else if (!strcmp(buf,"KMMOON"))
		ellip->km=KMMOON;
	else if (!strcmp(buf,"GEQ"))
		ellip->km=GEQ_HERE;
	else if (!strcmp(buf,"KMW84"))
		ellip->km=KMW84_HERE;
	else if (!strcmp(buf,"GEQ84"))
		ellip->km=GEQ84_HERE;
	else
		return 0;
	if (!get_value_as_string(get_named_child(grs_tag,"rotation"),buf,128))
		return 0;
	if (!isalpha(buf[0])){
		if (1!=double_converter(buf,&ellip->omega,1,NULL))
			return 0;
	}
	else if (!strcmp(buf,"OMEGA"))
		ellip->omega=OMEGA_HERE;
	else
		return 0;
	return 1;
}



static int set_projection(struct tag *prj_tag,  def_projection *proj){
	if (!get_value_as_string(get_named_child(prj_tag,"description"),proj->descr,MAX_DSCR_LEN))
		*(proj->descr)='\0';
	if (!get_value_as_string(get_named_child(prj_tag,"mlb"),proj->mlb,MLBLNG))
		return 0;
	if (1!=get_value(get_named_child(prj_tag,"cha_str"),&proj->cha_str,1,int_converter,NULL))
		return 0;
	if (1!=get_value(get_named_child(prj_tag,"type"),&proj->type,1,int_converter,NULL))
		return 0;
	if (1!=get_value(get_named_child(prj_tag,"cstm"),&proj->cstm,1,int_converter,NULL))
		return 0;
	if (1!=get_value(get_named_child(prj_tag,"mode"),&proj->mode,1,int_converter,NULL))
		return 0;
	if (1!=get_value(get_named_child(prj_tag,"mask"),&proj->mask,1,int_converter,NULL))
		return 0;
	if (!get_value_as_string(get_named_child(prj_tag,"kms_seq"),proj->seq,4))
		strcpy(proj->seq,"ZZ");
	if (!get_value_as_string(get_named_child(prj_tag,"native"),proj->native_proj,MLBLNG))
		strcpy(proj->native_proj,proj->mlb);
	if (!get_value_as_string(get_named_child(prj_tag,"region"),proj->rgn,3))
		strcpy(proj->rgn,"ZZ");
	if (!get_value_as_string(get_named_child(prj_tag,"parameters"),proj->param_text,128))
		*proj->param_text='\0';
	if (!get_value_as_string(get_named_child(prj_tag,"parent_datum"),proj->p_datum,16))
		strcpy(proj->p_datum,"\"");
	if (1!=get_value(get_named_child(prj_tag,"n_parameters"),&proj->q_par,1,int_converter,NULL))
		return 0;
	strcpy(proj->param_tokens,"\"");
	if ((proj->q_par)>0 && strlen(proj->param_text)==0){
		if (!get_value_as_string(get_named_child(prj_tag,"parameter_tokens"),proj->param_tokens,32))
			return 0;
	}
	return 1;
}
	
static int set_datum(struct tag *dtm_tag, def_datum *dtm){
	if (!get_value_as_string(get_named_child(dtm_tag,"mlb"),dtm->mlb,MLBLNG))
		return 0;
	if (!get_value_as_string(get_named_child(dtm_tag,"description"),dtm->descr,MAX_DSCR_LEN))
		*dtm->descr='\0';
	if (1!=get_value(get_named_child(dtm_tag,"id"),&dtm->no,1,int_converter,NULL))
		return 0;
	dtm->p_no=-1;
	if (!get_value_as_string(get_named_child(dtm_tag,"parent"),dtm->p_datum,MLBLNG))
		return 0;
	if (!get_value_as_string(get_named_child(dtm_tag,"ellipsoid"),dtm->ellipsoid,MLBLNG))
		return 0;
	if (1!=get_value(get_named_child(dtm_tag,"imit"),&dtm->imit,1,int_converter,NULL))
		return 0;
	if (!get_value_as_string(get_named_child(dtm_tag,"region"),dtm->rgn,3))
		strcpy(dtm->rgn,"ZZ");
	if (1!=get_value(get_named_child(dtm_tag,"type"),&dtm->type,1,int_converter,NULL))
		return 0;
	if (dtm->type!=0 && dtm->type!=1){
		if (1!=get_value(get_named_child(dtm_tag,"t_x"),dtm->translation,1,double_converter,NULL))
			return 0;
		if (1!=get_value(get_named_child(dtm_tag,"t_y"),dtm->translation+1,1,double_converter,NULL))
			return 0;
		if (1!=get_value(get_named_child(dtm_tag,"t_z"),dtm->translation+2,1,double_converter,NULL))
			return 0;
		if (dtm->type!=3){
			if (1!=get_value(get_named_child(dtm_tag,"rot_x"),dtm->rotation,1,double_converter,"sx"))
				return 0;
			if (1!=get_value(get_named_child(dtm_tag,"rot_y"),dtm->rotation+1,1,double_converter,"sx"))
				return 0;
			if (1!=get_value(get_named_child(dtm_tag,"rot_z"),dtm->rotation+2,1,double_converter,"sx"))
				return 0;
			if (1!=get_value(get_named_child(dtm_tag,"scale"),&dtm->scale,1,double_converter,"ppm"))
				return 0;
		} /*end should have 7 parameters */
	} /*end should have datum shift parameters */
	return 1;
}

static int set_hth(struct tag *hth_tag, def_hth_tr *hth_tr){
	if (!get_value_as_string(get_named_child(hth_tag,"from"),hth_tr->from_mlb,MLBLNG))
		return 0;
	if (!get_value_as_string(get_named_child(hth_tag,"description"),hth_tr->descr,MAX_DSCR_LEN))
		*(hth_tr->descr)='\0';
	if (!get_value_as_string(get_named_child(hth_tag,"to"),hth_tr->to_dtm,MLBLNG))
		return 0;
	if (1!=get_value(get_named_child(hth_tag,"type"),&hth_tr->type,1,int_converter,NULL))
		return 0;
	if (1!=get_value(get_named_child(hth_tag,"lat_0"),&hth_tr->B0,1,double_converter,"nt"))
		return 0;
	if (1!=get_value(get_named_child(hth_tag,"lon_0"),&hth_tr->L0,1,double_converter,"nt"))
		return 0;
	switch (hth_tr->type){
		case 1:
			if (!get_value_as_string(get_named_child(hth_tag,"table"),hth_tr->table,MAX_TABLE_LEN))
				return 0;
			break;
		case 2:
			if (1!=get_value(get_named_child(hth_tag,"k0"),hth_tr->constants,1,double_converter,NULL))
				return 0;
			break;
		case 3:
			if (1!=get_value(get_named_child(hth_tag,"M0"),hth_tr->constants,1,double_converter,NULL))
				return 0;
			if (1!=get_value(get_named_child(hth_tag,"N0"),hth_tr->constants+1,1,double_converter,NULL))
				return 0;
			if (1!=get_value(get_named_child(hth_tag,"k0"),hth_tr->constants+2,1,double_converter,NULL))
				return 0;
			if (1!=get_value(get_named_child(hth_tag,"kN"),hth_tr->constants+3,1,double_converter,"sx"))
				return 0;
			if (1!=get_value(get_named_child(hth_tag,"kE"),hth_tr->constants+4,1,double_converter,"sx"))
				return 0;
			break;
		default:
			return 0;
	}
	return 1;
}

static int set_rgn(struct tag *rgn_tag, def_rgn *rgn){
	if (!get_value_as_string(get_named_child(rgn_tag,"new"),rgn->rgn_new,3))
		return 0;
	if (!get_value_as_string(get_named_child(rgn_tag,"old"),rgn->rgn_old,3))
		return 0;
	if (!get_value_as_string(get_named_child(rgn_tag,"country"),rgn->country,64))
		return 0;
	return 1;
}

/* 
Function that parses def_lab file. All is well as long as we can determine via a min_tokens attribute if 'entries' spanning more than one line
 are completed after reading at least two lines! And 'entries' SHOULD span at least two lines! 
If formatting in the file is changed, this logic should be reconsidered :-)
Mode def_rgn is special since new 'entries' are not prefixed by '#'. Thus regions could be considered as one 'entry' spanning an unpredictable number of lines. 
*/

 def_data *open_def_data(struct tag *root, int *n_err){
	/*stuff that match the formatting and 'modes' of the def_lab file */
	struct tag def_tag,item_tag;
	def_data *data=NULL;
	int n_set[5]={0,0,0,0,0},mode,i;
	enum modes {mode_prj=0,mode_grs=1,mode_rgn=2,mode_dtm=3,mode_hth=4};
	void *entries[5]={NULL,NULL,NULL,NULL,NULL};
	char *mode_names[5]={"def_prj","def_grs","def_rgn","def_dtm","def_hth"};
	char *item_names[5]={"prj","grs","rgn","dtm","hth"};
	/* specifications for how much to preallocate */
	int n_prealloc[5]={256,128,128,128,64};
	int n_alloc[5]={256,128,128,128,64};
	int mode_sizes[5]={sizeof( def_projection),sizeof( def_grs),sizeof(def_rgn),sizeof( def_datum), sizeof( def_hth_tr)};
	
	*n_err=0; /* set no errors - yet! */
	/* allocate memory for objects */
	for(mode=0;mode<5;mode++){
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
	for(mode=0; mode<5; mode++){
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
				int ok;
				void *entry=entries[mode];
				switch(mode){
					case 0:
						ok=set_projection(&item_tag,(def_projection*) entry+n_set[mode]);
						break;
					case 1:
						ok=set_ellipsoid(&item_tag,(def_grs*) entry+n_set[mode]);
						break;
					case 2:
						ok=set_rgn(&item_tag, (def_rgn*) entry+n_set[mode]);
						break;
					case 3:
						ok=set_datum(&item_tag, (def_datum*) entry+n_set[mode]);
						break;
					case 4:
						ok=set_hth(&item_tag, (def_hth_tr*) entry+n_set[mode]);
						break;
					default:
						lord_error(TR_LABEL_ERROR,LORD("Invalid mode."));
						goto error;
					
				} /*end switch */
				if (!ok){
					lord_error(TR_LABEL_ERROR,LORD("Invalid xml format of entry in mode %d"),mode);
					goto error;
				}
			}
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
			item_tag=get_next_child(&def_tag,&item_tag);
		} /* end iteration over items */ 
	}/*end mode iteration*/
	/*reallocate sizes of objects */
	for(i=0;i<5;i++)
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
		if (dtm1->p_no<=0)
			lord_debug(0,"parse_def: Could not find p_datum!\n");
	}
	}
	
	/*transfer stuff to data object */
	data->n_prj=n_set[mode_prj];
	data->n_rgn=n_set[mode_rgn];
	data->n_ellip=n_set[mode_grs];
	data->n_dtm=n_set[mode_dtm];
	data->n_hth=n_set[mode_hth];
	data->projections=entries[mode_prj];
	data->datums=entries[mode_dtm];
	data->ellipsoids=entries[mode_grs];
	data->regions=entries[mode_rgn];
	data->hth_entries=entries[mode_hth];
	/* perhaps sort entries after number?? */
	return data;
	error:
	    (*n_err)++;
	    for(i=0;i<5;i++)
	        free(entries[i]);
	    free(data);
	    return NULL;
}

void close_def_data( def_data *data){
	if (!data)
		return;
	free(data->projections);
	free(data->datums);
	free(data->ellipsoids);
	free(data->regions);
	free(data->hth_entries);
	free(data);
	return;
}
