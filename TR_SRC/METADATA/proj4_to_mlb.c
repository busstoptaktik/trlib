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
#include "KmsFncs.h"
#include "strong.h"
#include "proj4_to_mlb.h"


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
/*TODO: use alias table...*/
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
static int guess_proj4_datum(proj4_entry *proj_entry, char *datum, def_data *DEF_DATA){
       
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
int proj4_to_mlb(char *proj4_text, char *mlb, def_data *DEF_DATA){
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
	strcpy(mlb,"TODO");
	free(proj_entry);
        return TR_OK;
}

/*A sketchy implementation of this translation - todo: improve.... easier than the other way!*/
int mlb_to_proj4(char *mlb, char *out, int buf_len, def_data *DEF_DATA){
        int n_chars,is_geo=0,set_datum=1;
        char internal_buf[512], *tmp;
        char mlb1[2*MLBLNG], mlb2[2*MLBLNG], *h_datum;
        short sepch;
        PR *srs=NULL;
	strncpy(out,"TODO",buf_len);
	return TR_OK;
        
}

