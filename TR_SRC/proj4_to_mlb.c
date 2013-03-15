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
#include <ctype.h>
#include "geo_lab.h"
#include "proj4_to_mlb.h"
#include "strong.h"
#include "trlib_intern.h"
#include "trlib_api.h"
#include "lord.h"
#include "parse_def_file.h"
#include "get_mlb.h"

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
/* not used right now
static struct towgs84_translation
COMMON_PROJ4_DEFINITIONS[]={
{"ed50","intl",3,{-87,-98,-121,0,0,0,0}},
{"etrs89","grs80",3,{0,0,0,0,0,0,0}},   and this could be gr96 as well! 
{NULL,NULL,0,{0,0,0,0,0,0,0}}
};
*/

/* Some KMS aliases for etrs89 - possibly others as well*/

static struct translation_entry KMS_DATUM_ALIAS[]={
{"etrf89","etrs89"},
{"euref89","etrs89"},
{NULL,NULL}
};

/*KMS to proj4  ellps translation
* These are the ones, where we accept a direct translation!
*Proj4 keys are converted to lower case, that's reflected in this table. Use the normalisation table below to normalise....
*/
static struct translation_entry ELLIPSOID_TRANSLATIONS[]={
{"GRS80","grs80"}, 
{"WGS84","wgs84"},
{"Hayford","intl"},
{"WGS72","wgs72"}, 
{"NWL9D","nwl9d"}, 
{"Bessel","bessel"},
{"Clarke66","clrk66"},
{"Clarke80","clrk80"},
{"Everest","evrst30"},
{NULL,NULL}
};

/*Table used to normalise proj4 ellipsoids*/
static struct translation_entry PROJ4_ELLP_ALIAS[]={
{"grs80","GRS80"},
{"wgs84","WGS84"},
{"wgs72","WGS72"},
{"nwl9d","NWL9D"},
{"NULL","NULL"}
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

static char *translate(struct translation_entry *table, char *look_for){
	while (table->key!=NULL){
		if (!strcmp(table->key,look_for)){
			return table->value;
		}
		table++;
	} /*end loop over translations */
	return NULL;
}

static char *inverse_translate(struct translation_entry *table, char *look_for){
	while (table->value!=NULL){
		if (!strcmp(table->value,look_for)){
			return table->key;
		}
		table++;
	} /*end loop over translations */
	return NULL;
}

/*setter function for allowing/disallowing KMS-datum extensions */
/*Not thread safe! */

void set_kms_datums_allowed(int is_allowed){
	(is_allowed) ? (ALLOW_KMS_DATUMS=1) : (ALLOW_KMS_DATUMS=0);
}



/*Function that parses proj4 tokens into a struct*/
 proj4_entry *parse_proj4(char *proj4_text){
	 char *tokens[64], cleaned[512], *tmp;
	 int item_count=0,i,pair;
	 proj4_entry *proj_entry;
	 if (strlen(proj4_text)>511){
		 lord_error(TR_ALLOCATION_ERROR,LORD("Too long input text!"));
		 return NULL;
	 }
	 /*scan the proj4 text*/
	 remove_whitespace(proj4_text,cleaned);
	 tmp=cleaned;
	 /*this works also with scientific notation and things like +x_0=+45000*/
	 while(*tmp && item_count<64){
		 if (*tmp=='+' && isalpha(*(tmp+1))){
			 tokens[item_count]=tmp+1;
			 item_count++;
			 *tmp='\0';
		}
		tmp++;
	 }
	 if (item_count<2){
		lord_error(TR_LABEL_ERROR,LORD("Not enough items in proj4 string."));
		return NULL;
	 }
	 if (item_count==64)
		 lord_warning(0,LORD("Reached limit %d of proj4 items."),item_count);
	 proj_entry=malloc(sizeof(proj4_entry));
	 /*no memory, we exit*/
	 if (proj_entry==NULL){
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
	 /*these are sensible defaults*/
	 proj_entry->lon_0=0.0; 
	 proj_entry->lat_0=0.0;
	 proj_entry->lat_ts=0.0;
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
			for(j=0; j<n_params; j++){
				double out=atof(params[j]);
				proj_entry->towgs84[j]=out; /* check this!*/
				/*lord_warning(0,"in: %s, out: %.8f",params[j],out);*/
			}
			free(params);
			proj_entry->n_params=n_params;
		}
		free(key_val);
	}
	return proj_entry;
}

/*Guess a datum from proj4 data 
* This is a mess if the +datum token is not give explicitely, since there does not seem to universal agreement on the +towgs84 7-parameter transformation for many datums!
* The proj4 policy seems also to have been not to include the datum-shift in the above case 
* Thus there is no bullet proof method of setting a KMS datum from a proj4 string, unless the datum is explicitly given with the +datum token.
*  E.g. there is no way of distinguishing etrs89 and gr96 in proj4 currently. (towgs both zeroes...)
*/
static int guess_proj4_datum(proj4_entry *proj_entry, char *datum){
	extern def_data *DEF_DATA; /*pointer to the preparsed def_data structure*/
	/*if no datum - look for towgs84 params or guesssssss*/
	datum[0]='\0';
	if (DEF_DATA==NULL){
		lord_error(TR_ALLOCATION_ERROR,LORD("def-data not initialised!"));
		return TR_ALLOCATION_ERROR;
	}
	/*if proj4 datum token is not set, we will conduct a search for a KMS-match */
	if (proj_entry->datum[0]=='\0'){
		/*if no datum shift parameters are set, guess datum from ellipsoid name from a list of a few defaults - shaky! */
		if (proj_entry->n_params<3){
			lord_warning(1,"Proj4: no proper datum (or datum shift) definition. Guessing datum from ellipsoid.");
			/* if ellipsoid is not set, default to wgs84*/
			if (proj_entry->ellps[0]=='\0'){
				lord_warning(1,LORD("Proj4: datum and ellps keys both undefined, setting datum to WGS84"));
				strcpy(datum,"wgs84");
			}
			else{/*look for a default translation of ellipsoid name to datum */
				char *datum_guess=translate(ELLIPSOID_TO_DATUM,proj_entry->ellps);
				if (datum_guess!=NULL)
					strcpy(datum,datum_guess);
				else{
					lord_error(TR_LABEL_ERROR,LORD("Unable to guess datum from ellipsoid."));
					return TR_LABEL_ERROR;
				}
			}
			
		}
		/* else shift parameters are set. Then see if datum shift and ellipsoids look like something we know */
		else{ 
			def_datum *dtm_def=NULL;
			int i,found=0;
			/*struct towgs84_translation *current_try=COMMON_PROJ4_DEFINITIONS;*/
			char *kms_ellipsoid=inverse_translate(ELLIPSOID_TRANSLATIONS,proj_entry->ellps);
			if (kms_ellipsoid==NULL){
				lord_error(0,LORD("Unable to translate proj4 ellipsoid - datum not set."));
				return TR_LABEL_ERROR;
			}
			for(i=0;i<DEF_DATA->n_dtm && !found; i++){
				dtm_def=DEF_DATA->datums+i;
				if (!strcmp(dtm_def->ellipsoid,kms_ellipsoid) && (!strcmp(dtm_def->p_datum,"wgs84") || !(strcmp(dtm_def->p_datum,"etrs89")))){
					int t_ok=0,r_ok=-1,s_ok=-1,j;
					double *transl=dtm_def->translation;
					double *rot=dtm_def->rotation;
					double scale=dtm_def->scale*1e6; /*scale converted to a decimal number, proj4 uses ppm (like we do in def_lab). */
					for(j=0;j<3;j++)
						t_ok+=(fabs(transl[j]-proj_entry->towgs84[j])<1e-2);
					/*reverse a sign! - change to arc seconds*/	
					if (proj_entry->n_params>5){
						r_ok=0;
						for(j=0;j<3;j++)
							r_ok+=(fabs(rot[j]*DOUT*3600.0+proj_entry->towgs84[j+3])<1e-4);
							
					}
					
					if (proj_entry->n_params==7){
						s_ok=0;
						s_ok+=(fabs(proj_entry->towgs84[6]-scale)<1e-8);
					}
					
					switch(dtm_def->type){
						case 3:
							found=(t_ok==3) && (r_ok==-1 || r_ok==3) && (s_ok==-1 || s_ok==1);
							break;
						default:
							found=(t_ok==3) && (r_ok==3) && (s_ok==1);
						
					}
					/*lord_debug(0,"dtm: %s, t_ok: %d, r_ok: %d, s_ok: %d, found: %d",dtm_def->mlb,t_ok,r_ok,s_ok,found);*/					
				}
			}
			if (found){
				strcpy(datum,dtm_def->mlb);
			}
			else{
				lord_error(TR_LABEL_ERROR,LORD("Failed to recognize datum from parameters."));
				/*lord_debug(0,"%.8f %.8f %.8f",proj_entry->towgs84[3],proj_entry->towgs84[4],proj_entry->towgs84[5]);*/
				return TR_LABEL_ERROR;
			}
			/*
			while (current_try->kms_dtm!=NULL){
				if (!strcmp(proj_entry->ellps,current_try->proj4_ellps) && proj_entry->n_params>=current_try->min_params){
					int nok=0;
					for (i=0; i<proj_entry->n_params && i<7; i++)
						nok+=(fabs(proj_entry->towgs84[i]-current_try->towgs84[i])<1e-7);
					if (nok==proj_entry->n_params){
						strcpy(datum,current_try->kms_dtm);
						break;
					} 
				} 
				current_try++;
			} 
			*/
			
		} /*end shift parameters given*/
			
			
		
	} /*end datum token not given */
	else{ /*datum token is given and we look for a translation */
		/*First check the proj4->kms datum translations*/
		char *datum_guess=translate(PROJ4_TO_KMS_DATUM,proj_entry->datum);
		if (datum_guess!=NULL)
			strcpy(datum,datum_guess);
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
	
	if (proj_entry==NULL){
		lord_error(TR_LABEL_ERROR,"Unable to parse proj4 string: %s",proj4_text);
		return TR_LABEL_ERROR;
	}
	proj[0]='\0';
	datum[0]='\0';
	vdatum[0]='\0';
	param_string[0]='\0'; /*no extra parameters as default*/
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
		double lat_given=proj_entry->lat_ts; /* if not given - defaults to 0.0*/
		if (proj_entry->lat_0!=0.0 && proj_entry->lat_ts==0.0) /*then this is what we've got*/
			lat_given=proj_entry->lat_0;
		if ( lat_given==0 && proj_entry->lon_0==0 && proj_entry->x_0==0 && proj_entry->y_0==0 && proj_entry->k==1){
			if (proj_entry->a==RADIUS_FLAT && proj_entry->b==RADIUS_FLAT && strcmp(proj_entry->nadgrids,"@null")==0 && strcmp(datum,"wgs84")==0){
				strcpy(mlb,"webmrc");
				found_exact_alias=1;
			}
			else if (proj_entry->a==NO_DATA && proj_entry->b==NO_DATA)
				strcpy(proj,"mrc0");
		}
		else{
			sprintf(param_string,"%.2fdg  %.2f%s  %.2fdg  %.2f%s",lat_given,proj_entry->y_0,proj_entry->units,
			proj_entry->lon_0,proj_entry->x_0,proj_entry->units);
		}
			
	}
	if (!found_exact_alias){
		char *pt=mlb;
		pt+=sprintf(pt,"%s%c%s",proj,sep_char,datum);
		/*lord_debug(0,"Proj4: proj - %s %s",proj,mlb);*/
		if (sep_char!='E' && sep_char!='_' && strlen(vdatum)>0)
			pt+=sprintf(pt,"_h_%s",vdatum);
		if (strlen(param_string)>0)
			sprintf(pt,"  %s",param_string);
	}
	free(proj_entry);
	return TR_OK;
}

/*A sketchy implementation of this translation - todo: improve.... easier than the other way!*/
int mlb_to_proj4(char *mlb, char *out, int buf_len){
	int n_chars,is_geo=0,set_datum=1;
	char internal_buf[512], *tmp;
	char mlb1[2*MLBLNG], mlb2[2*MLBLNG], *h_datum;
	short sepch,region;
	PR *srs=NULL;
	extern def_data *DEF_DATA; /*pointer to the preparsed def_data structure*/
	if (DEF_DATA==NULL){
		lord_error(TR_ALLOCATION_ERROR,LORD("def-data not initialised!"));
		return TR_ALLOCATION_ERROR;
	}
	out[0]='\0';
	srs=TR_OpenProjection(mlb);
	if (srs==NULL){
		return TR_LABEL_ERROR;
	}
	n_chars=get_mlb(mlb,&region,mlb1,&sepch,mlb2,&h_datum);
	if (n_chars==0){
		lord_error(TR_LABEL_ERROR,LORD("Failed to parse mlb"));
		goto MLB_TO_PROJ4_CLEANUP;
	}
	tmp=internal_buf;
	/*case utm*/
	if (strstr(mlb1,"utm")==mlb1){
		short utm_zone=GET_ZONE(srs);
		tmp+=sprintf(tmp,"+proj=utm +zone=%d",utm_zone);
	}
	/*case geo*/
	else if (strstr(mlb1,"geo")==mlb1){
		tmp+=sprintf(tmp,"+proj=latlong");
		is_geo=1;
	}
	else{
		/*case TM*/
		if (IS_TM(srs)){
			tmp+=sprintf(tmp,"+proj=etmerc");
			tmp+=sprintf(tmp," +lat_0=%.8g +lon_0=%.8g",GET_LAT0(srs)*DOUT,GET_LON0(srs)*DOUT);
			tmp+=sprintf(tmp," +x_0=%.2g +y_0=%.2g +k_0=%.10g",GET_X0(srs),GET_Y0(srs),GET_SCALE(srs)); 
		}
		/*case MERCATOR*/
		else if (IS_MERCATOR(srs)){
			tmp+=sprintf(tmp,"+proj=merc");
			/*well - this is not it: TODO:*/
			tmp+=sprintf(tmp," +lat_ts=%.8g +lon_0=%.8g",GET_LAT0(srs)*DOUT,GET_LON0(srs)*DOUT);
			tmp+=sprintf(tmp," +x_0=%.2g +y_0=%.2g",GET_X0(srs),GET_Y0(srs));
			/*and special case webmrc*/
			if (!strcmp(mlb,"webmrc")){
				tmp+=sprintf(tmp," +a=6378137 +b=6378137 +nadgrids=@null");
				set_datum=0;
			}
		}
		else{
			lord_warning(0,LORD("Translation not implemented"));
			goto MLB_TO_PROJ4_CLEANUP;
		}
	}
	if (!is_geo)
		tmp+=sprintf(tmp," +units=m");
	/*datum part*/
	if (set_datum){
		if (!strcmp(mlb2,"wgs84")){
			tmp+=sprintf(tmp," +datum=WGS84");
		}
		else{ /*Another datum - look up parameters*/
			short dtm_no=GET_DTM(srs);
			int n_dtm=DEF_DATA->n_dtm,i;
			char *ellipsoid,*n_ellipsoid;
			double *transl,rot[3],scale;
			def_datum *dtm_def=NULL;
			for(i=0;i<n_dtm && dtm_def==NULL;i++){
				if (DEF_DATA->datums[i].no==dtm_no){
					dtm_def=DEF_DATA->datums+i;
				}
			}
			if (dtm_def==NULL){
				lord_error(TR_LABEL_ERROR,LORD("Failed to locate datum-no: %d"),dtm_no);
				goto MLB_TO_PROJ4_CLEANUP;
			}
			/* ok now we have a proper handle of the definition of the datum!*/
			/*should we use our definitions, or COMMON_PROJ4_DEFINITIONS - otherwise the mapping is not 1-1, probably never can be!
			* Well - use our definitions - bot ways! */
			transl=dtm_def->translation;
			/*It seems like Proj4 uses reverse sign for rotations compared to us=gst/kms and EPSG*/
			/* But units seem to be in arc seconds: http://trac.osgeo.org/proj/wiki/GenParms#towgs84-DatumtransformationtoWGS84 
			* our parsed units are in radians */
			for(i=0;i<3;i++)
				rot[i]=-(dtm_def->rotation[i])*DOUT*(3600.0);
			scale=dtm_def->scale*1e6;
			strncpy(mlb2,dtm_def->mlb,MLBLNG);
			/*Translate ellipsoid*/
			ellipsoid=translate(ELLIPSOID_TRANSLATIONS,dtm_def->ellipsoid);
			if (ellipsoid==NULL){
				lord_error(TR_LABEL_ERROR,"Failed to translate ellipsoid!");
				goto MLB_TO_PROJ4_CLEANUP;
			}
			n_ellipsoid=translate(PROJ4_ELLP_ALIAS,ellipsoid); /*normalise the format - really convert to upper case if needed...*/
			if (n_ellipsoid!=NULL)
				ellipsoid=n_ellipsoid;
			tmp+=sprintf(tmp," +ellps=%s",ellipsoid);
			if (strcmp(dtm_def->p_datum,"wgs84") && (strcmp(dtm_def->p_datum,"etrs89"))){ /*if parent not wgs84 - more complicated - skip*/
				/*TODO: implement datum shift combination - or test if p_datum->p_datum is simple compared to wgs84!*/
				lord_warning(0,LORD("Parent datum not wgs84 - unable to fetch datum shift parameters!"));
			}
			else
				tmp+=sprintf(tmp," +towgs84=%.6g,%.6g,%.6g,%.10g,%.10g,%.10g,%.10g",transl[0],transl[1],transl[2],rot[0],rot[1],rot[2],scale);
			
		}
	} /*end set datum*/
	if (h_datum!=NULL){
		lord_warning(0,"+vdatum is a custom tag not supported in regular proj4 syntax");
		tmp+=sprintf(tmp," +vdatum=%s",h_datum+2);
	}
	tmp+=sprintf(tmp," +no_defs");
	if ((tmp-internal_buf)>buf_len-1){
		lord_error(TR_ALLOCATION_ERROR,LORD("Supplied input buffer not long enough!"));
		goto MLB_TO_PROJ4_CLEANUP;
	}
	strcpy(out,internal_buf);
	TR_CloseProjection(srs);
	return TR_OK;
	/* clean up code on exit*/
	MLB_TO_PROJ4_CLEANUP:
		TR_CloseProjection(srs);
		return TR_LABEL_ERROR;
	
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