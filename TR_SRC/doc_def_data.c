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
#include <ctype.h>
#include <string.h>
#include "set_grs.h"
#include "trlib_api.h"
#include "parse_def_file.h"
#include "geo_lab.h"
#include "doc_def_data.h"

/* Purpose: move all documentation of labels - also stuff implemented in conv_lab - here for a cleaner 'aspect' oriented structure */
/* Mainly thought of as usable for external apps, which operate on minilabels/strings *not* via the internal system numbers */
/* Doc fuunctions can be called with "" or NULL in which case the 'next' item is requested (via an internal static). */
/* This mode is *NOT* thread safe - otherwise the functions are thread safe. Could be made thread safe by adding THREAD_SAFE macro, This would be a bit wastefull... */

/* simlk, june 2012 */




extern def_data *DEF_DATA;  /* due to design, mimicking 'old style', this is global rather than a parameter to functions. */

/* will simply output  "rgn" or "rgn country" depending on detail level requested */
/* use conv_rgn for other types of call */
int doc_rgn(char *rgn_name, char *descr, int detail){
	int i;
	def_rgn *rgn=NULL;
	int mode=0;
	static int n_rgn=0;
	if (DEF_DATA==NULL)
		return TR_LABEL_ERROR;
	*descr='\0';
	if (rgn_name==NULL || strlen(rgn_name)==0){
		mode=0;
		if (n_rgn==DEF_DATA->n_rgn){
			n_rgn=0;
			return TR_LABEL_ERROR;
		}
		rgn=DEF_DATA->regions+(n_rgn++);
	}
	else{
		mode=1;
		for(i=0; i<DEF_DATA->n_rgn && rgn==NULL; i++){
			if (!strcmp(rgn_name,DEF_DATA->regions[i].rgn_new))
				rgn=DEF_DATA->regions+i;
		}
	}
	if (rgn==NULL)
		return TR_LABEL_ERROR;
	/*decide what to print to descr*/
	if (detail>0 || mode==0)
		descr+=sprintf(descr,"%s ",rgn->rgn_new);
	if (detail>0 || mode==1)
		sprintf(descr,"%s",rgn->country);
	
	return TR_OK;
}

/* search for prj_name. If prj_name is "" or NULL, 'next' prj is requested */
/* type can be used to separate proj labels from other types of labels */
/* impl_datum is useful when describing prj labels like e.g. dktm1 or webmrc where the datum is implicit */

int doc_prj(char *prj_name, char *descr, char *impl_datum, int *type, int detail){
	def_projection *prj=NULL;
	int i,mode=0;
	static int n_prj=0;
	char s_name[MLBLNG],replaced[MLBLNG];
	replaced[0]='\0';
	if (DEF_DATA==NULL)
		return TR_LABEL_ERROR;
	*descr='\0';
	if (prj_name==NULL || strlen(prj_name)==0){
		mode=0;
		if (n_prj==DEF_DATA->n_prj){
			n_prj=0;
			return TR_LABEL_ERROR;
		}
		prj=DEF_DATA->projections+(n_prj++);
		
	}
	/* else look for prj_name */
	else{
		int have_looked=0, n_digits=0;
		mode=1;
		/*replace digits by '?' in second search to be able to find utm projs */
		while (prj==NULL && have_looked<2){
			for (i = 0; *(prj_name+i) && i<MLBLNG; i++){
				if (isdigit(*(prj_name+i)) && have_looked>0){
					*(replaced+n_digits++)=*(prj_name+i);
					*(s_name+i)='?';
					
				}
				else
					*(s_name+i)=*(prj_name+i); /* copy char */
			}
			*(s_name+i) = '\0'; 
			*(replaced+n_digits)='\0';
			for(i=0; i<DEF_DATA->n_prj && prj==NULL; i++)
				if (!strcmp(DEF_DATA->projections[i].mlb,s_name))
					prj=DEF_DATA->projections+i;
			have_looked++;
		}
	}
	/*printf("Look for: %s n_prj is %d, found? %d\n", prj_name,n_prj,(prj==NULL));*/
	if (prj==NULL){
		
		return TR_LABEL_ERROR;
	}
	
	if (impl_datum!=NULL){
		if (prj->p_datum[0]!='\"')
			strncpy(impl_datum,prj->p_datum,MLBLNG);
		else
			*impl_datum='\0';
	}
	*type=prj->type;
	/*decide what to print to descr*/
	if (detail>1 || mode==0) /*iteration mode */
		descr+=sprintf(descr,"%s ",prj->mlb);
	if (detail>1)
	/*todo: add whatever relevant here */
		descr+=sprintf(descr,"mode: %d, cstm: %d ",prj->mode,prj->cstm);
	if (detail>0 || mode==1){
		char *loop_over_descr=prj->descr, *loop_over_replaced=replaced;
		/*replace ?? by the stored digits*/
		while (*loop_over_descr){
			if (*(loop_over_descr)=='?' && *loop_over_replaced){
				*(descr++)=*(loop_over_replaced++);
			}
			else
				*(descr++)=*(loop_over_descr);
			loop_over_descr++;
		}
		*descr='\0';
	}
	return TR_OK;
}

/* search for dtm_name. If dtm_name is "" or NULL, 'next' prj is requested */
/* if detail<0 only the ellipsoid name is copied to descr - usefull when looking only for ellipsoid data */
int doc_dtm( char *dtm_name, char *descr, int detail){
	def_datum *dtm=NULL;
	int i,s_mode,mode;
	static int n_dtm=0;
	char *w,s_name[MLBLNG];
	if (DEF_DATA==NULL)
		return TR_LABEL_ERROR;
	/*if called with dtm_no, look for that - not the internal KMS-number, simply the consequtive array index */
	if (dtm_name==NULL || strlen(dtm_name)==0){
		mode=0;
		if (n_dtm==DEF_DATA->n_dtm){
			n_dtm=0;
			return TR_LABEL_ERROR; /*actually not needed!*/
		}
		dtm=DEF_DATA->datums+(n_dtm++);
	}
	/* else look for dtm_name */
	else {
		mode=1;
		strncpy(s_name,dtm_name,MLBLNG);
		for(s_mode=0; s_mode<2; s_mode++){
			/* do the actual search */
			for(i=0; i<DEF_DATA->n_dtm && dtm==NULL; i++){
				if(!strcmp(DEF_DATA->datums[i].mlb,s_name))
					dtm=DEF_DATA->datums+i;
				}
				/* if not found and w_name looks right have another go with igsyy, itrfyy, etrfyy */
			if (dtm==NULL && (*s_name=='i' || *s_name=='e') && strlen(s_name)>4 && isdigit(s_name[strlen(s_name)-1])){
				w       = s_name + strlen(s_name) - 2; 
				*(w)    = 'y';
				*(w +1) = 'y';
			}
			else
				break;
		}
	}
	if (dtm==NULL)
		return TR_LABEL_ERROR;
	if (detail>-1){
		if (mode==0 || detail>1)
			descr+=sprintf(descr,"%s ",dtm->mlb);
		if (detail>1)
			/*todo: put more stuff here */
			descr+=sprintf(descr,"%s %d %s ",dtm->ellipsoid,dtm->type,dtm->p_datum);
		if (detail>0 || mode==1)
			sprintf(descr,"%s",dtm->descr);
	}
	else
		strcpy(descr,dtm->ellipsoid);
	return TR_OK;
}


			
			
			
/* output number of predefined labels */	
	
	
int get_def_numbers(int *n_prj, int *n_dtm, int *n_grs, int *n_rgn){
	if (DEF_DATA==NULL)
		return TR_ERROR;
	*n_prj=DEF_DATA->n_prj;
	*n_dtm=DEF_DATA->n_dtm;
	*n_rgn=DEF_DATA->n_rgn;
	*n_grs=DEF_DATA->n_ellip;
	return TR_OK;
}


/* Copied from previous set_grs - out could be changed to a string */

int doc_grs(char *ell_name, FILE *out, int detail){
	int rc,i,internal_no,p=0,mode;
	static int n_grs=0;
	def_grs *grs=NULL;
	double e[9],a,f,J2,GM,omg,U0,geq,ksf,Q;
	char dummy_name[2*MLBLNG];
	if (DEF_DATA==NULL)
		return TR_ERROR;
	if (ell_name==NULL || strlen(ell_name)==0){
		mode=0;
		if (n_grs==DEF_DATA->n_ellip){
			n_grs=0;
			return TR_LABEL_ERROR;
		}
		grs=DEF_DATA->ellipsoids+(n_grs++);
	}
	else{
		mode=1;
		for(i=0; i<DEF_DATA->n_ellip && grs==NULL; i++){
			if (!strcmp(ell_name,DEF_DATA->ellipsoids[i].mlb))
				grs=DEF_DATA->ellipsoids+i;
		}
	}
	if (grs==NULL)
		return TR_LABEL_ERROR;
	internal_no=grs->no;
	mode=grs->mode;
	rc=set_grs(internal_no,dummy_name,e);
	if (rc<0)
		return TR_LABEL_ERROR;
	if (out==NULL)
		return TR_OK;
	if (detail>0)
		p=6;
	if (detail>1)
		p=12;
	a=*(e+0);
	f=*(e+1);
	J2=*(e+2);
	GM=*(e+3);
	omg=*(e+4);
	U0=*(e+5);
	geq=*(e+6);
	ksf=*(e+7);
	Q=*(e+8);
	if (n_grs==1){
		fprintf(out, "\n\n   Ellipsoid    KMS-nr");
		fprintf(out, "             a            1/f");
	}
	fprintf(out, ((p) ? "\n\nEllipsoid = %14s\nKMS_nmb: %17d"
		     : "\n   %-12s %4d"), dummy_name, internal_no);
	if (p) { /* output of constants */
		fprintf(out, "\n\na   = %20.*g m       definition.", p+6, a);
		/* def by mean degree - todo: add this*/
		/*if (mode == 4  && p == 12) { 
			(void) fprintf(out, "\nmg  = %20.*g", p+6, amg);
			if (toise_t_m != 1.0)
				(void) fprintf(out, " toises  m/toise = %14.12f", toise_t_m);
			else (void) fprintf(out, " m");
		}*/
		if (f != 0.0) {
			(void) fprintf(out, "\n1/f = %20.*g", p+6, 1.0/f);
			if (mode != 1) (void) fprintf(out, "         definition.");
		}
		if (p == 12 || f == 0.0){
			(void) fprintf(out, "\nf   =   %17.*e", p+4, f);
			if (mode != 1) (void) fprintf(out, "     definition.");
		}
		(void) fprintf(out, "\nJ2  = %20.*e", p, J2);
		if (mode == 1) (void) fprintf(out, "         definition.");
		(void) fprintf(out, "\nGM  = %20.*e m3/s2", p, GM);
		if ((mode == 1) || (mode == 3) || (mode == 4))
			(void) fprintf(out, "   definition.");
		(void) fprintf(out, "\nomg = %20.*e rad/s   definition.", p, omg);
		(void) fprintf(out, "\nU0  = %20.*e m2/s2", p, U0);
		(void) fprintf(out, "\ngeq = %20.*e m/s2", p, geq);
		if (mode == 2) (void) fprintf(out, "    definition.");
		(void) fprintf(out, "\nksf = %20.*e", p, ksf);
		(void) fprintf(out, "\nMkv = %20.*g m", p+6, Q);
		(void) fprintf(out, "\n");
		/* The description of the ellipsoids is not preparsed and thus not available here */
		/* Not considered interesting enough to 'waste' space with that, or? */  
		/*if (e_no) {
		do {
		  qr = fgetc(def_lab_file);
		  if (qr != EOF) qr = '"';
		  else
		  if (qr != '"') (void) fputc((qr != '¨') ? qr : '#', out);
		} while (qr != '"');
	      } else {
		(void) fprintf(out,"\nUser-defined Geodetic Reference System.");
		(void) fprintf(out,"\nDefining parameters are indicated.");
	      } */
		
	} /* end output of constants */
	
	else /* output of a and f */{
		(void) fprintf(out, "%6s%14.4f m", " ", a);
		if (f != 0.0) (void) fprintf(out, "%16.8f", 1.0/f);
		else          (void) fprintf(out, "  Spherical");
	}

	return TR_OK;		
}

/* a function which should be usefull for extracting ellipsoid data to be used in bshlm1 */
int get_ellipsoid_data(union geo_lab *plab_in, double *data){
	return 0;
}

/* this function simply dumps all preparsed input - only usefull for debugging purposes */
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
	fprintf(fp,"n_dtm: %d\n",n_dtm);
	fprintf(fp,"n_grs: %d\n",n_grs);
	fprintf(fp,"n_hth: %d\n\n",n_hth);
	for (i=0; i<data->n_prj; i++){
		fprintf(fp,"prj: %s\n",projections[i].mlb);
		fprintf(fp,"cha_str: %d type: %d cstm: %d mode: %d mask: %d\n",projections[i].cha_str,projections[i].type,projections[i].cstm,projections[i].mode,projections[i].mask);
		fprintf(fp,"seq: %s rgn: %s p_dtm: %s par_tokens: %s\n",projections[i].seq,projections[i].rgn,projections[i].p_datum,projections[i].param_tokens);
		fprintf(fp,"n_par: %d\n",projections[i].q_par);
		fprintf(fp,"native_proj: %s\n",projections[i].native_proj);
		if (projections[i].q_par>0 && projections[i].param_tokens[0]=='\"'){
			for(j=0;j<projections[i].q_par;j++) fprintf(fp," %f",projections[i].native_params[j]);
			fprintf(fp,"\n");
		}
		fprintf(fp,"%s\n\n",projections[i].descr);
	
	}
	fprintf(fp,"******************\n\n");
	for(i=0; i<n_grs; i++){
		fprintf(fp,"grs: %s\n",ellipsoids[i].mlb);
		fprintf(fp,"%d %d %f %f %e %e\n\n",ellipsoids[i].no,ellipsoids[i].mode,ellipsoids[i].axis,ellipsoids[i].flattening,ellipsoids[i].km,ellipsoids[i].omega);
	}
	fprintf(fp,"******************\n\n");
	fprintf(fp,"Regions:\n");
	for(i=0; i<n_rgn; i++)
		fprintf(fp,"%s %s %s\n",regions[i].rgn_new,regions[i].rgn_old,regions[i].country);
	fprintf(fp,"\n******************\n\n");
	for(i=0;i<n_dtm;i++){
		fprintf(fp,"dtm : %s, no: %d\n",datums[i].mlb,datums[i].no);
		fprintf(fp,"parent: %s, ellipsoid: %s\n",datums[i].p_datum,datums[i].ellipsoid);
		fprintf(fp,"imit: %d, type: %d, rgn: %s\n",datums[i].imit,datums[i].type,datums[i].rgn);
		fprintf(fp,"TO-WGS84:\n%f %f %f\n%f ppm %f dg %f dg %f dg\n",datums[i].translation[0],datums[i].translation[1],datums[i].translation[2],datums[i].scale*1e6,
		datums[i].rotation[0]*R2D,datums[i].rotation[1]*R2D,datums[i].rotation[2]*R2D);
		fprintf(fp,"%s\n\n",datums[i].descr);
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
		fprintf(fp,"%s\n\n",data->hth_entries[i].descr);
		
	}
	fprintf(fp,"*****************\n\n");
	{
	int n_bytes=0;
	n_bytes=sizeof( def_projection)*n_prj+sizeof( def_datum)*n_dtm+sizeof( def_grs)*n_grs+n_rgn*3+sizeof( def_hth_tr)*n_hth;
	fprintf(fp,"bytes used: %d\n",n_bytes);
	}
	
	return;
}

