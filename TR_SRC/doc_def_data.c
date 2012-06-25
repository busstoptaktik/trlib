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
/* Mainly thought of as usable for external apps, which operate on minilabels/strings or via consequtive array numbers *not* via the internal system numbers */

/* simlk, june 2012 */




extern def_data *DEF_DATA;  /* due to design, mimicking 'old style', this is global rather than a parameter to functions. */

/* will simply output  "rgn country" */
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

int doc_prj(char *prj_name, char *descr, char *impl_datum, int *type, int detail){
	def_projection *prj=NULL;
	int i,mode=0;
	static int n_prj=0;
	char s_name[MLBLNG];
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
		mode=1;
		strncpy(s_name,prj_name,MLBLNG);
		/*replace digits by '?' - to be able to find utm projs */
		for (i = 0; *(s_name+i); i++)  *(s_name+i) = (isdigit(*(s_name+i))) ? '?' : *(s_name+i);
		*(s_name+i) = '\0';
		for(i=0; i<DEF_DATA->n_prj && prj==NULL; i++)
			if (!strcmp(DEF_DATA->projections[i].mlb,s_name))
				prj=DEF_DATA->projections+i;
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
	if (detail>1 || mode==0)
		descr+=sprintf(descr,"%s ",prj->mlb);
	if (detail>1)
	/*todo: add whatever relevant here */
		descr+=sprintf(descr,"mode: %d, cstm: %d ",prj->mode,prj->cstm);
	if (detail>0 || mode==1)
		sprintf(descr,"%s",prj->descr);
	return TR_OK;
}

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
	if (mode==0 || detail>1)
		descr+=sprintf(descr,"%s ",dtm->mlb);
	if (detail>1)
		/*todo: put more stuff here */
		descr+=sprintf(descr,"%s %d %s ",dtm->ellipsoid,dtm->type,dtm->p_datum);
	if (detail>0 || mode==1)
		sprintf(descr,"%s",dtm->descr);
	return TR_OK;
}


			
			
			
	
	
	
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

int get_ellipsoid_data(union geo_lab *plab_in, double *data){
	return 0;
}



