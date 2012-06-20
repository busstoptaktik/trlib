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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "geo_lab.h"
#include "fgetln_kms.h"
#include "sgetg.h"
#include "parse_def_file.h"
#include "trlib_api.h"
#define MODE_PRJ "def_prj"
#define MODE_GRS "def_grs"
#define MODE_RGN "def_rgn"
#define MODE_DTM "def_dtm"
#define MODE_HTH "def_hth"
#define MODE_DEF_LAB "def_lab"
#define MODE_STOP "stop"
#define N_TOKENS_PRJ 12
#define N_TOKENS_DTM 14
#define N_TOKENS_RGN 3
#define N_TOKENS_GRS 7
#define N_TOKENS_HTH 6
#define MAX_LINE_DEF 4
#define RGN_STOP "STOP"
/*Values  below pasted from def_lab.txt */
#define KM_HERE       (3986005.e8)
#define OMEGA_HERE (7292115.0e-11)
#define GEQ_HERE     (978049.e-5)
#define GEQ84_HERE  (979764.5e-5)
#define KMW84_HERE (3986004.418e8)

int set_ellipsoid(char **items,  def_grs *ellip, int n_items){
	strncpy(ellip->mlb,items[0],MLBLNG);
	ellip->no=atoi(items[1]);
	ellip->mode=atoi(items[2]);
	ellip->axis=atof(items[3]);
	ellip->flattening=atof(items[4]);
	if (!isalpha(items[5][0]))
		ellip->km=atof(items[5]);
	else if (!strcmp(items[5],"KM"))
		ellip->km=KM_HERE;
	else if (!strcmp(items[5],"GEQ"))
		ellip->km=GEQ_HERE;
	else if (!strcmp(items[5],"KMW84"))
		ellip->km=KMW84_HERE;
	else if (!strcmp(items[5],"GEQ84"))
		ellip->km=GEQ84_HERE;
	else
		return 1;
	if (!isalpha(items[6][0]))
		ellip->omega=get_number(items[6]);
	else if (!strcmp(items[6],"OMEGA"))
		ellip->omega=OMEGA_HERE;
	else
		return 1;
	return 0;
}
	
int set_projection(char **items,  def_projection *proj, int n_items){
	int i,n_param;
	strncpy(proj->mlb,items[0],MLBLNG);
	proj->cha_str=atoi(items[1]);
	proj->type=atoi(items[2]);
	proj->cstm=atoi(items[3]);
	proj->mode=atoi(items[4]);
	proj->mask=atoi(items[5]);
	strncpy(proj->seq,items[6],4);
	strncpy(proj->rgn,items[7],3);
	strncpy(proj->p_datum,items[8],16);
	proj->q_par= atoi(items[9]);
	proj->param_tokens[0]='\0';
	if (items[10][0]!='"')
		n_param=proj->q_par;
	else
		n_param=1;
	/*test for proper number of items */
	if (n_param==1 && n_items!=N_TOKENS_PRJ && n_items!=N_TOKENS_PRJ+proj->q_par){
		
		return 1;
	}
	
	if (n_param>1 && (n_items!=N_TOKENS_PRJ+n_param-1)){
		
		return 1;
	}
	
	for (i=0; i<n_param && i<n_items-10; i++){
		strcat(proj->param_tokens,items[i+10]);
		if (i<n_param-1)
			strcat(proj->param_tokens," ");
	}
	
	if (n_items>10+n_param)
		strncpy(proj->native_proj,items[10+n_param],MLBLNG);
	else
		return 1;
	proj->param_text[0]='\0';
	for(i=0; i<n_items-11-n_param; i++){
		proj->native_params[i]=get_number(items[i+11+n_param]);
		strcat(proj->param_text,items[i+11+n_param]);
		if (i<n_items-11-n_param-1)
			strcat(proj->param_text," ");
	}
	return 0;
	
}
	
int set_datum(char **tokens,  def_datum *dtm, int n_items){
	int i;
	strncpy(dtm->mlb,tokens[0],MLBLNG);
	dtm->no= atoi(tokens[1]);
	dtm->p_no=-1;
	strncpy(dtm->p_datum,tokens[2],MLBLNG);
	strncpy(dtm->ellipsoid,tokens[3],MLBLNG);
	dtm->imit= atoi(tokens[4]);
	strncpy(dtm->rgn,tokens[5],2);
	dtm->type=atoi(tokens[6]);
	dtm->scale=get_number(tokens[10]);
	for (i=0;i<3;i++){
		dtm->translation[i]=get_number(tokens[7+i]);
		dtm->rotation[i]=get_number(tokens[11+i]);
	}
	return 0;
}

int set_hth(char **tokens,  def_hth_tr *entry, int n_items){
	int i;
	double val;
	strncpy(entry->from_mlb,tokens[0],MLBLNG);
	strncpy(entry->to_dtm,tokens[1],MLBLNG);
	entry->type=atoi(tokens[2]);
	entry->B0=get_number(tokens[3]);
	entry->L0=get_number(tokens[4]);
	switch (entry->type){
		case 1:
			strncpy(entry->table,tokens[5],MAX_TABLE_LEN);
			break;
		case 2:
			entry->constants[0]=get_number(tokens[5]);
			break;
		case 3:
			if (n_items<N_TOKENS_HTH+4)
				return 1;
			for(i=0;i<5 && i<n_items-5; i++){
				val=get_number(tokens[5+i]);
				(entry->constants)[i]=val;
				/*printf("hth: %s, item: %s, val: %e\n",tokens[0],tokens[5+i],val);*/
			}
			break;
		default:
			return 1;
	}
	return 0;
}

/* gets a number - if it has a unit sgetg is used */
double get_number(char *item){
	int n_chars=strlen(item);
	int used;
	struct typ_dec type;
	char term[2];
	if (isalpha(item[n_chars-1]))
		return sgetg(item,&type,&used,term);
	return atof(item);
}	

/*append items from list2 to list1 */
char **append_items(char **items1, char **items2, int n_items1, int n_items2){
	int i;
	char **new_items= (char **) realloc(items1,sizeof(char*)*(n_items1+n_items2));
	if (new_items==NULL)
		return NULL;
	
	for (i=0;i<n_items2;i++)
		new_items[n_items1+i]=items2[i];
	return new_items;
}


/* splits string into items */
char **get_items(char *line, char *delimiter, int *item_count){
	char *item;
	int n_items=0,buf_length=40,buf_size=40;
	char **items=malloc(sizeof(char*)*buf_length);
	if (items==NULL)
		return NULL;
	item=strtok(line,delimiter);
	if (item==NULL){
		free(items);
		return NULL;
	}
	while (item && items){
		*(items+(n_items++))=item;
	
		if (n_items>buf_size)
			{
			buf_size+=buf_length;
			items=realloc(items,(buf_size)*sizeof(char*));
			}
		item=strtok(NULL,delimiter);
	}
	items=realloc(items,n_items*sizeof(char*));
	*item_count=n_items;
	return items;
}


/* 
Function that parses def_lab file. All is well as long as we can determine via a min_tokens attribute if 'entries' spanning more than one line
 are completed after reading at least two lines! And 'entries' SHOULD span at least two lines! 
If formatting in the file is changed, this logic should be reconsidered :-)
Mode def_rgn is special since new 'entries' are not prefixed by '#'. Thus regions could be considered as one 'entry' spanning an unpredictable number of lines. 
*/

 def_data *open_def_data(FILE *fp, int *n_err){
	/*stuff that match the formatting and 'modes' of the def_lab file */
	void *entries[5]={NULL,NULL,NULL,NULL,NULL};
	def_data *data=NULL;
	enum modes {mode_prj,mode_grs,mode_rgn,mode_dtm,mode_hth,mode_none};
	enum modes mode=mode_none;
	int n_modes=6,new_mode;
	int min_tokens[5]={N_TOKENS_PRJ,N_TOKENS_GRS,N_TOKENS_RGN,N_TOKENS_DTM,N_TOKENS_HTH};
	int n_set[5]={0,0,0,0,0};
	/* specifications for how much to preallocate */
	int n_prealloc[5]={256,128,128,128,64};
	int n_alloc[5]={256,128,128,128,64};
	int mode_sizes[5]={sizeof( def_projection),sizeof( def_grs),sizeof(def_rgn),sizeof( def_datum), sizeof( def_hth_tr)};
	char *mode_names[7]={MODE_PRJ,MODE_GRS,MODE_RGN,MODE_DTM,MODE_HTH,MODE_STOP,NULL},*mode_name;
	/*some other needed stuff for buffering etc */
	char buf[4096],savelines[MAX_LINE_DEF][1024];
	int n_lines=0;
	char **tokens=NULL,**next_tokens=NULL,*sub_str;
	int n_items=0,n_items2,n_items_line;
	int n_chars,i;
	int err;
	int completed=1; /* needed for entries (all) that span more than one line */ 
	*n_err=0; /* set no errors - yet! */
	/* allocate memory for objects */
	for(i=0;i<n_modes-1;i++){
		entries[i]=malloc(n_prealloc[i]*mode_sizes[i]);
		if (entries[i]==NULL)
			goto error;
	}
	data=malloc(sizeof( def_data));
	if (data==NULL){
		goto error;
	}
	/*loop over def-file */
	while ((n_chars=fgetln_kms(buf,&n_items_line,fp))!=EOF){
		if (strlen(buf)==0 || n_items_line==0)
			continue;
		if (buf[0]=='#'){
			/* OK: so we might have found the beginning of the def_data - perhaps use this later */
			if (strstr(buf+1,MODE_DEF_LAB)!=NULL)
				continue;
			
			/*If we got here, we have either found a new item or should start a new mode */
			if (!completed)
				(*n_err)++;
			completed=1;
			
			new_mode=0;
			while((mode_name=mode_names[new_mode])){
				if ((sub_str=strstr(buf+1,mode_name))!=NULL)
					break;
				new_mode++;
			}
			
			/*if mode_rgn - start reading */
			if (new_mode==mode_rgn)
				completed=0;
			/*on new mode - continue */
			if (mode_name!=NULL){
				mode=new_mode;
				continue;
			}
			/*this shouldn't happen! */
			if (mode==mode_none){
				(*n_err)++;
				continue;
			}
			/*else we might have encountered a new item in some mode */
			n_lines=0;
			strncpy(savelines[0],buf+1,1024);
			if (tokens!=NULL)
				free(tokens);
			tokens=get_items(savelines[0]," \n\r", &n_items);
			if (tokens==NULL || n_items==0)
				continue;
			/*set completion flag to allow finishing up the element after reading next lines*/
			completed=0;
			/* check to see if we should reallocate */
			if (n_set[mode]<n_alloc[mode])
				continue;
			/*else re-allocate data*/
			{
				void *more_space;
				more_space=realloc(entries[mode],mode_sizes[mode]*(n_alloc[mode]+n_prealloc[mode]));
				if (more_space!=NULL){
					n_alloc[mode]+=n_prealloc[mode];
					entries[mode]=more_space;
				}
				else{
					(*n_err)++;
					completed=1;
					mode=mode_none;
				}
			continue;
			} /*end re-allocation */

		}/*end found hash */
	if (completed || mode==mode_none) /*keep going if entry is completed */
		continue;
	if (mode==mode_rgn){
		if (tokens!=NULL)
			free(tokens);
		tokens=get_items(buf," \n\r",&n_items);
		if (n_items>=N_TOKENS_RGN){
			if (!strcmp(tokens[2],RGN_STOP)){
				mode=mode_none;
				completed=1;
			}
			else if (n_set[mode_rgn]<n_alloc[mode_rgn]){
				//strncpy(((char*) entries[mode_rgn])+(n_set[mode_rgn]++)*mode_sizes[mode_rgn],tokens[0],mode_sizes[mode_rgn]);
				{
					def_rgn *this_rgn;
					this_rgn=((def_rgn*) entries[mode_rgn])+(n_set[mode_rgn]++);
					strncpy(this_rgn->rgn_new,tokens[0],3);
					strncpy(this_rgn->rgn_old,tokens[1],3);
				}
				}
			}
		free(tokens); tokens=NULL;
		continue;
		}
	/*something wrong?*/
	if (n_lines>=MAX_LINE_DEF){
		n_lines=0;
		(*n_err)++;
		completed=1;
		continue;
	}
	/*append items from  next line */
	strncpy(savelines[++n_lines],buf,1024);
	next_tokens=get_items(savelines[n_lines]," \n\r", &n_items2);
	if (next_tokens!=NULL && n_items2>0){
		tokens=append_items(tokens,next_tokens,n_items,n_items2);
		/*printf("n_items: %d, n_items2: %d first: %s\n",n_items,n_items2,tokens[0]);*/
		n_items+=n_items2;
		free(next_tokens);
	}
	
	if (n_items<min_tokens[mode]) /*important logic here! We must be able to decide after reading some lines if we have the minimal number of items! */
		continue;
	
	if (mode==mode_prj)
		err=set_projection(tokens,((def_projection*) entries[mode])+n_set[mode],n_items);
	
	else if (mode==mode_grs)
		err=set_ellipsoid(tokens,((def_grs*) entries[mode])+n_set[mode],n_items);
	
	else if (mode==mode_dtm)
		err=set_datum(tokens,((def_datum*) entries[mode])+n_set[mode],n_items);
	
	else if (mode==mode_hth)	
		err=set_hth(tokens,((def_hth_tr*) entries[mode])+n_set[mode],n_items);
	
	else{
		fprintf(stdout,"Something wrong!\n"); /*TODO: use GLOBAL error handling system! */
		continue;
	}
	if (!err)
		n_set[mode]++;
	else{
		(*n_err)++;
		/*printf("Err: mode: %s, item: %s n_items: %d\n",mode_names[mode],tokens[0],n_items);*/
		}
	completed=1;
	/*end main loop over lines */
	}
	if (tokens!=NULL)
		free(tokens);
	/*reallocate sizes of objects */
	for(i=0;i<n_modes-1;i++)
		entries[i]=realloc(entries[i],mode_sizes[i]*n_set[i]);
	/*set parent no of datums*/
	
	{
	def_datum *dtm1,*dtm2;
	int j;
	for(i=0;i<n_set[mode_dtm]; i++){
		dtm1=((def_datum*) entries[mode_dtm])+i;
		//dtm1->p_no=-1;
		for (j=0;j<n_set[mode_dtm];j++){
			dtm2=((def_datum*) entries[mode_dtm])+j;
			if (!strcmp(dtm2->mlb,dtm1->p_datum)){
				dtm1->p_no=dtm2->no;
				break;
			}
		}
		if (dtm1->p_no<=0)
			printf("Could not find p_datum!\n");
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
	    for(i=0;i<n_modes-1;i++)
	        free(entries[i]);
	    free(data);
	    return NULL;
}

void present_data(FILE *fp,def_data *data){
	int i,j;
	double R2D=57.295779513082323;
	def_projection *projections=data->projections;
	def_datum *datums=data->datums;
	def_grs *ellipsoids=data->ellipsoids;
	def_rgn *regions=data->regions;
	int n_rgn=data->n_rgn;
	int n_grs=data->n_ellip;
	int n_dtm=data->n_dtm;
	int n_prj=data->n_prj;
	int n_hth=data->n_hth;
	fprintf(fp,"n_prj: %d\n",n_prj);
	fprintf(fp,"n_rgn: %d\n",n_rgn);
	fprintf(fp,"n_dtm: %d\n",n_rgn);
	fprintf(fp,"n_grs: %d\n",n_grs);
	fprintf(fp,"n_hth: %d\n\n",n_hth);
	for (i=0; i<data->n_prj; i++){
		fprintf(fp,"prj: %s\n",projections[i].mlb);
		fprintf(fp,"cha_str: %d type: %d cstm: %d mode: %d mask: %d\n",projections[i].cha_str,projections[i].type,projections[i].cstm,projections[i].mode,projections[i].mask);
		fprintf(fp,"seq: %s rgn: %s p_dtm: %s par_tokens: %s\n",projections[i].seq,projections[i].rgn,projections[i].p_datum,projections[i].param_tokens);
		fprintf(fp,"n_par: %d\n",projections[i].q_par);
		fprintf(fp,"native_proj: %s\n",projections[i].native_proj);
		for(j=0;j<projections[i].q_par;j++)
			fprintf(fp," %f",projections[i].native_params[j]);
		if (projections[i].q_par>0)
			fprintf(fp,"\n");
	fprintf(fp,"\n");
	}
	fprintf(fp,"******************\n\n");
	for(i=0; i<n_grs; i++){
		fprintf(fp,"grs: %s\n",ellipsoids[i].mlb);
		fprintf(fp,"%d %d %f %f %e %e\n",ellipsoids[i].no,ellipsoids[i].mode,ellipsoids[i].axis,ellipsoids[i].flattening,ellipsoids[i].km,ellipsoids[i].omega);
	}
	fprintf(fp,"******************\n\n");
	fprintf(fp,"Regions:\n");
	for(i=0; i<n_rgn; i++)
		fprintf(fp,"%s %s\n",regions[i].rgn_new,regions[i].rgn_old);
	fprintf(fp,"\n******************\n\n");
	for(i=0;i<n_dtm;i++){
		fprintf(fp,"dtm : %s, no: %d\n",datums[i].mlb,datums[i].no);
		fprintf(fp,"parent: %s, ellipsoid: %s\n",datums[i].p_datum,datums[i].ellipsoid);
		fprintf(fp,"imit: %d, type: %d, rgn: %s\n",datums[i].imit,datums[i].type,datums[i].rgn);
		fprintf(fp,"TO-WGS84:\n%f %f %f\n%f ppm %f dg %f dg %f dg\n\n",datums[i].translation[0],datums[i].translation[1],datums[i].translation[2],datums[i].scale*1e6,
		datums[i].rotation[0]*R2D,datums[i].rotation[1]*R2D,datums[i].rotation[2]*R2D);
	}
	fprintf(fp,"*****************\n\n");
		
	for(i=0;i<n_hth;i++){
		fprintf(fp,"hth: %s to %s\n", data->hth_entries[i].from_mlb,data->hth_entries[i].to_dtm);
		fprintf(fp,"Bo: %f, L0: %f\n",data->hth_entries[i].B0,data->hth_entries[i].L0);
		switch( (data->hth_entries[i]).type)
		{
			case 1:
				fprintf(fp,"Table: %s\n",data->hth_entries[i].table);
				break;
			case 2:
				fprintf(fp,"constant: %f\n",data->hth_entries[i].constants[0]);
				break;
			case 3:
				fprintf(fp,"Constants: ");
				for(j=0;j<5;j++)
					fprintf(fp," %e",data->hth_entries[i].constants[j]);
				fprintf(fp,"\n");
				break;
			default:
				fprintf(fp,"Unrecognized type!\n");
		
		}
		fprintf(fp,"\n");
				
	}
	fprintf(fp,"*****************\n\n");
	{
	int n_bytes=0;
	n_bytes=sizeof( def_projection)*n_prj+sizeof( def_datum)*n_dtm+sizeof( def_grs)*n_grs+n_rgn*3+sizeof( def_hth_tr)*n_hth;
	fprintf(fp,"bytes used: %d\n",n_bytes);
	}
	
	return;
}

void close_def_data( def_data *data){
	free(data->projections);
	free(data->datums);
	free(data->ellipsoids);
	free(data->regions);
	free(data->hth_entries);
	free(data);
	return;
}

/*

int main(void){
	int n_err=0;
	FILE *fp=fopen("def_lab.txt","rb");
	 def_data *data=NULL;
	data=open_def_data(fp,&n_err);
	fclose(fp);
	present_data(stdout,data);
	close_def_data(data);
	printf("Errors: %d\n",n_err);
	return 0;}
	
*/

