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
#include <math.h>
#include "geo_lab.h"
#include "proj4_to_mlb.h"
#include "strong.h"
#include "trlib_api.h"
#include "lord.h"
#include "parse_def_file.h"
/* A sketchy implementation of proj4 to mlb translation. 
* lots of hardcoded stuff, which should perhaps be made more visible....
*/

/* flag which signals whether or not we allow extended (kms) datums in the proj4 +datum key*/ 
static int ALLOW_KMS_DATUMS=1; 

struct translation_entry{
	char *key;
	char *value;
};

struct towgs84_translation{
	 char *kms_dtm;
	 char *proj4_ellps;
	 int min_params;
	 double towgs84[7];
 };

 /* not used at the moment...
static struct proj4_to_kms_ellps 
ELLIPSOID_TRANSLATIONS[]={
{"intl","Hayford"},
{"bessel","Bessel"},
{NULL,NULL}
};
*/
 
 #define NO_DATA (-9999)
 #define RADIUS_FLAT (6378137) /*used to recognize web Mercator */
/* the datums that we want to guess from proj4-ellipsoids */
static struct translation_entry
ELLIPSOID_TO_DATUM[]={
{"grs80","etrs89"}, /*could be gr96 as well */
{"wgs84","wgs84"},
{NULL,NULL}
};

/*translations of proj4 datums to kms datums
* only datums (usually) build into the library "proj", which have direct kms-translations should be stated here...
* If we allow kms datums in the +datum parameter (see global flag above), only proj4-datums which do not appear under the same name as kms-datum are relevant
*/
static struct translation_entry
PROJ4_TO_KMS_DATUM[]={
{"wgs84","wgs84"},
{"nad83","nad83"},
/*perhaps we should add some more ourselves, even if they are not supported by the library proj, but only by the language proj?? */
{NULL,NULL}
};

/* A few datums that we want to recognize from ellipsoid and common proj4 datum shift parameters */
static struct towgs84_translation
COMMON_PROJ4_DEFINITIONS[]={
{"ed50","intl",3,{-87,-98,-121,0,0,0,0}},
{"etrs89","grs80",3,{0,0,0,0,0,0,0}},   /*and this could be gr96 as well! */
{NULL,NULL,0,{0,0,0,0,0,0,0}}
};

/* Some KMS aliases for etrs89 - possibly others as well*/

static struct translation_entry KMS_DATUM_ALIAS[]={
{"etrf89","etrs89"},
{"euref89","etrs89"},
{NULL,NULL}
};


void normalise_datum(char *datum){
	struct translation_entry *look_up=KMS_DATUM_ALIAS;
	while (look_up->key){
		if (!strcmp(datum,look_up->key)){
			strcpy(datum,look_up->value);
			break;
		}
		look_up++;
	}
}


/*setter function for allowing/disallowing KMS-datum extensions */
/*Not thread safe! */

void set_kms_datums_allowed(int is_allowed){
	(is_allowed) ? (ALLOW_KMS_DATUMS=1) : (ALLOW_KMS_DATUMS=0);
}



/*Function that parses proj4 tokens into a struct*/
 proj4_entry *parse_proj4(char *proj4_text){
	 char **tokens=NULL, *no_spaces;
	 int item_count,i,pair;
	 proj4_entry *proj_entry;
	 no_spaces=malloc(sizeof(char)*strlen(proj4_text));
	 if (no_spaces==NULL){
		 lord_error(TR_ALLOCATION_ERROR,LORD("Unable to allocate memory!"));
		 return NULL;
	 }
	 remove_whitespace(proj4_text,no_spaces);
	 tokens=get_items(no_spaces,"+",&item_count);
	 if (tokens==NULL||item_count<2){
		 lord_error(TR_LABEL_ERROR,LORD("Proj4-string could not be splitted."));
		 free(no_spaces);
		 return NULL;
	 }
	 proj_entry=malloc(sizeof(proj4_entry));
	 /*no memory, we exit*/
	 if (proj_entry==NULL){
		 free(tokens);
		 free(no_spaces);
		 return NULL;
	 }
	 /*set default values*/
	 proj_entry->proj[0]='\0';
	 proj_entry->datum[0]='\0';
	 proj_entry->vdatum[0]='\0';
	 proj_entry->ellps[0]='\0';
	 proj_entry->nadgrids[0]='\0';
	 strcpy(proj_entry->units,"m");
	 proj_entry->x_0=0;
	 proj_entry->y_0=0;
	 proj_entry->k=1.0;
	 proj_entry->n_params=0;
	 proj_entry->lon_0=NO_DATA;
	 proj_entry->lat_0=NO_DATA;
	 proj_entry->lat_ts=NO_DATA;
	 proj_entry->zone=NO_DATA;
	 proj_entry->a=NO_DATA;
	 proj_entry->b=NO_DATA;
	 
	 for(i=0;i<item_count;i++){
		 /*printf("token: %s\n",tokens[i]);*/
		 char **key_val=get_items(tokens[i],"=",&pair);
		
		 
		 if (key_val==NULL || pair!=2){
			 /*printf("proj4 token: %s not valid\n",tokens[i]);*/
			 continue;
		 }
		 /*parse proj key value pairs*/
		 /* convert string to lower case for easier comparison */
		 string_tolower(key_val[0]);
		 string_tolower(key_val[1]);
		 
		 if (!strcmp(key_val[0],"proj"))
			 strncpy(proj_entry->proj,key_val[1],64);
		else if (!strcmp(key_val[0],"datum"))
			strncpy(proj_entry->datum,key_val[1],64);
		else if(!strcmp(key_val[0],"vdatum"))
			strncpy(proj_entry->vdatum,key_val[1],64);
		else if (!strcmp(key_val[0],"ellps"))
			strncpy(proj_entry->ellps,key_val[1],64);
		else if (!strcmp(key_val[0],"units"))
			strncpy(proj_entry->units,key_val[1],16);
		else if (!strcmp(key_val[0],"nadgrids"))
			strncpy(proj_entry->nadgrids,key_val[1],64);
		else if (!strcmp(key_val[0],"zone"))
			proj_entry->zone=atoi(key_val[1]);
		else if (!strcmp(key_val[0],"x_0"))
			proj_entry->x_0=atof(key_val[1]);
		else if (!strcmp(key_val[0],"y_0"))
			proj_entry->y_0=atof(key_val[1]);
		else if (!strcmp(key_val[0],"lat_0"))
			proj_entry->lat_0=atof(key_val[1]);
		else if (!strcmp(key_val[0],"lon_0"))
			proj_entry->lon_0=atof(key_val[1]);
		else if (!strcmp(key_val[0],"lat_ts"))
			proj_entry->lat_ts=atof(key_val[1]);
		else if (!strcmp(key_val[0],"a"))
			proj_entry->a=atof(key_val[1]);
		else if (!strcmp(key_val[0],"b"))
			proj_entry->b=atof(key_val[1]);
		else if (!strcmp(key_val[0],"k_0") || !strcmp(key_val[0],"k"))
			proj_entry->k=atof(key_val[1]);
		else if (!strcmp(key_val[0],"towgs84")){
			char **params;
			int n_params,j;
			params=get_items(key_val[1],",",&n_params);
			for(j=0; j<n_params; j++)
				proj_entry->towgs84[j]=atof(params[j]);
			free(params);
			proj_entry->n_params=n_params;
		}
		free(key_val);
	}
	free(tokens);
	free(no_spaces);
	return proj_entry;
}

/*Guess a datum from proj4 data 
* This is a mess if the +datum token is not give explicitely, since there does not seem to universal agreement on the +towgs84 7-parameter transformation for many datums!
* The proj4 policy seems also to have been not to include the datum-shift in the above case 
* Thus there is no bullet proof method of setting a KMS datum from a proj4 string, unless the datum is explicitly given with the +datum token.
*  E.g. there is no way of distinguishing etrs89 and gr96 in proj4 currently. (towgs both zeroes...)
*  A *few* shaky defaults will be built in here however.... 
*/
static int guess_proj4_datum(proj4_entry *proj_entry, char *datum){
	extern def_data *DEF_DATA; /*pointer to the preparsed def_data structure*/
	/*if no datum - look for towgs84 params or guesssssss*/
	datum[0]='\0';
	/*if proj4 datum token is not set, we will conduct a search for a KMS-match */
	if (proj_entry->datum[0]=='\0'){
		/*if no datum shift parameters are set, guess datum from ellipsoid name from a list of a few defaults - shaky! */
		if (proj_entry->n_params==0){
			lord_warning(1,"Proj4: no proper datum (or datum shift) definition. Guessing datum from ellipsoid.");
			/* if ellipsoid is not set, default to wgs84*/
			if (proj_entry->ellps[0]=='\0'){
				lord_warning(1,LORD("Proj4: proj and ellps keys both undefined, setting datum to WGS84"));
				strcpy(datum,"wgs84");
			}
			else{/*look for a default translation of ellipsoid name to datum */
				struct translation_entry *current_try=ELLIPSOID_TO_DATUM;
				while (current_try->key!=NULL){
					if (!strcmp(current_try->key,proj_entry->ellps)){
						strcpy(datum,current_try->value);
						break;
					}
					current_try++;
				}
			}
			
		}
		/* else shift parameters are set. Then see if datum shift looks like something we know */
		else{ 
			
			struct towgs84_translation *current_try=COMMON_PROJ4_DEFINITIONS;
			int i;
			while (current_try->kms_dtm!=NULL){
				if (!strcmp(proj_entry->ellps,current_try->proj4_ellps) && proj_entry->n_params>=current_try->min_params){
					int nok=0;
					for (i=0; i<proj_entry->n_params && i<7; i++)
						nok+=(fabs(proj_entry->towgs84[i]-current_try->towgs84[i])<1e-7);
					if (nok==proj_entry->n_params){
						strcpy(datum,current_try->kms_dtm);
						break;
					} /*end found datum*/
				} /*end start checking ccommon proj4 def*/
				current_try++;
			} /*end loop over common proj4 definitions */
			/*TODO: perhaps loop through kms-definitions in DEF_DATA to find a match from parameters...*/
		}
			
			
		
	} /*end datum token not given */
	else{ /*datum token is given and we look for a translation */
		/*First check the proj4->kms datum translations*/
		struct translation_entry *current_try=PROJ4_TO_KMS_DATUM;
		while (current_try->key!=NULL){
			if (!strcmp(current_try->key,proj_entry->datum)){
				strcpy(datum,current_try->value);
				break;
			}
			current_try++;
		} /*end loop over translations */
		/*IF allowed, see if the datum given is a valid kms-datum*/
		if (ALLOW_KMS_DATUMS && strlen(datum)==0){
			if (DEF_DATA==NULL)
				lord_warning(1,"Label definitions not initialised - unable to check proj4-datum");
			else{ /*look for a kms-datum*/
				def_datum *current_dtm;
				int i;
				for(i=0;i<DEF_DATA->n_dtm;i++){
					current_dtm=DEF_DATA->datums+i;
					if (!strcmp(proj_entry->datum,current_dtm->mlb)){
						strcpy(datum,current_dtm->mlb);
						break;
					}
				}
			} /*end look for kms-datum*/
		}
		if (strlen(datum)==0){ /*if datum not supported */
			lord_error(TR_LABEL_ERROR,"Proj4-datum: %s, not supported.",proj_entry->datum);
			return TR_LABEL_ERROR;
		}
	}/* end else datum is given */
	/*normalise datum - i.e. if given as an alias name return the standard name*/
	normalise_datum(datum);
	return TR_OK;
}
/*the actual conversion of proj4 text to mlb */
/*TODO: streamline recognition of mlbs with implicit datum to work more generally.... */
int proj4_to_mlb(char *proj4_text, char *mlb){
	extern def_data *DEF_DATA;
	int is_transverse_mercator=0,is_mercator=0,found_exact_alias=0,rc;
	char proj[32],datum[32],vdatum[32],sep_char='_';
	char param_string[128]; /*string to print extra parameters to */
	
	proj4_entry *proj_entry=parse_proj4(proj4_text);
	proj[0]='\0';
	datum[0]='\0';
	vdatum[0]='\0';
	param_string[0]='\0'; /*no extra parameters as default*/
	if (proj_entry==NULL){
		lord_error(TR_LABEL_ERROR,"Unable to parse proj4 string: %s",proj4_text);
		return TR_LABEL_ERROR;
	}
	
	/*set projection*/
	
	if (proj_entry->proj[0]=='\0'){
		lord_error(TR_LABEL_ERROR,LORD("Proj4: projection not set!"));
		free(proj_entry);
		return TR_LABEL_ERROR;
	}
	if (!strcmp(proj_entry->proj,"utm")){
		if (proj_entry->zone<0 || proj_entry->zone>60){
			lord_error(TR_LABEL_ERROR,LORD("Proj4: utm-zone not properly defined"));
			free(proj_entry);
			return TR_LABEL_ERROR;
		}
		sprintf(proj,"utm%02d",proj_entry->zone);
		
	}
	else if (!strcmp(proj_entry->proj,"latlong") || !(strcmp(proj_entry->proj,"longlat"))){
		strcpy(proj,"geo");
		mlb+=3;
	}
	else if (!strcmp(proj_entry->proj,"tmerc") || !(strcmp(proj_entry->proj,"etmerc"))){
		is_transverse_mercator=1;
		strcpy(proj,"itm");
	}
	else if (!strcmp(proj_entry->proj,"merc")){
		is_mercator=1;
		strcpy(proj,"mrc");
	}
	else{
		strcpy(mlb,"err");
		lord_error(TR_LABEL_ERROR,LORD("Proj4-projection: %s, not supported."),proj_entry->proj);
		free(proj_entry);
		return TR_LABEL_ERROR;
	}
	/*set datum*/
	rc=guess_proj4_datum(proj_entry,datum);
	
	if (strlen(datum)==0){
		lord_error(TR_LABEL_ERROR,"Proj4: Unable to recognize datum...");
		free(proj_entry);
		return TR_LABEL_ERROR;
	}
	/*if given - set vdatum */
	if (strlen(proj_entry->vdatum)>0 && ALLOW_KMS_DATUMS){
		if (DEF_DATA==NULL)
			lord_warning(1,LORD("definition data not initialised - unable to search for vdatum minilabel."));
		else{
			int i;
			for(i=0;i<DEF_DATA->n_dtm; i++){
				if (!strcmp((DEF_DATA->datums+i)->mlb,proj_entry->vdatum)){
					strcpy(vdatum,(DEF_DATA->datums+i)->mlb);
					sep_char='H';
					break;
				}
			}
		}
	}/*end looking for vdatum*/
			
	/*put extra parameters*/
	if (is_transverse_mercator){
		/*search for definition*/
		def_projection *prj=NULL,*this_prj;
		int i,params_ok;
		if (DEF_DATA==NULL)
			lord_warning(1,LORD("definition data not initialised - unable to search for projection minilabel."));
		else{
			for(i=0;i<DEF_DATA->n_prj; i++){
				params_ok=0;
				this_prj=DEF_DATA->projections+i;
				if (this_prj->q_par==5 && (!strcmp(this_prj->native_proj,"itm") || !(strcmp(this_prj->native_proj,"itmi")))) {
					/*test parameters*/
					double scale=1;
					if (!strcmp(proj_entry->units,"km"))
						scale=1000;
					params_ok+=(proj_entry->k==this_prj->native_params[4]);
					/*allow some 'round off' flexibility here....*/
					params_ok+=(fabs(proj_entry->lat_0-this_prj->native_params[0]*DOUT)<1e-9);
					params_ok+=(fabs(proj_entry->y_0*scale-this_prj->native_params[1])<1e-9);
					params_ok+=(fabs(proj_entry->lon_0-this_prj->native_params[2]*DOUT)<1e-9);
					params_ok+=(fabs(proj_entry->x_0*scale-this_prj->native_params[3])<1e-9);
					if (params_ok==5){
						prj=this_prj;
						break;
					}
					
				}
			}
		} /*end else*/
		if (prj!=NULL && !strcmp(datum,prj->p_datum)){
			strcpy(mlb,prj->mlb);
			found_exact_alias=1;
		}
		else{
			strcpy(proj,"itm");
			sprintf(param_string,"%.2fdg  %.2f%s  %.2fdg  %.2f%s  %.8f",proj_entry->lat_0,proj_entry->y_0,proj_entry->units,
			proj_entry->lon_0,proj_entry->x_0,proj_entry->units,proj_entry->k);
			if (prj!=NULL)
				lord_info(1,"Proj4: %s found semi-matching mlb %s, however no datum match.",proj4_text,prj->mlb);
		}
	}
	else if (is_mercator){ /*TODO: the same as above....*/
		/*check mrc0 and webmrc */
		if (proj_entry->lat_ts==0 && proj_entry->lon_0==0 && proj_entry->x_0==0 && proj_entry->y_0==0 && proj_entry->k==1){
			if (proj_entry->a==RADIUS_FLAT && proj_entry->b==RADIUS_FLAT && strcmp(proj_entry->nadgrids,"@null")==0 && strcmp(datum,"wgs84")==0){
				strcpy(mlb,"webmrc");
				found_exact_alias=1;
			}
			else if (proj_entry->a==NO_DATA && proj_entry->b==NO_DATA)
				strcpy(proj,"mrc0");
		}
		else{
			sprintf(param_string,"%.2fdg  %.2f%s  %.2fdg  %.2f%s",proj_entry->lat_ts,proj_entry->y_0,proj_entry->units,
			proj_entry->lon_0,proj_entry->x_0,proj_entry->units);
		}
			
	}
	if (!found_exact_alias){
		mlb+=sprintf(mlb,"%s%c%s",proj,sep_char,datum);
		if (sep_char!='E' && sep_char!='_' && strlen(vdatum)>0)
			mlb+=sprintf(mlb,"_h_%s",vdatum);
		if (strlen(param_string)>0)
			sprintf(mlb,"  %s",param_string);
	}
	free(proj_entry);
	return TR_OK;
}


/*
int main(int argc, char **argv){
	char mlb[128];
	int rc;
	rc=proj4_to_mlb(argv[0],mlb);
	printf("Got: %s, rc: %d\n",mlb,rc);
	return 0;
}
*/		