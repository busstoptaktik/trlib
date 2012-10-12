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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "proj4_to_mlb.h"
#include "strong.h"
#include "trlib_api.h"
#include "lord.h"
/* A sketcy implementation of proj4 to mlb translation. 
* lots of hardcoded stuff, which should perhaps be made more visible....
*/


 
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
	 strcpy(proj_entry->units,"m");
	 proj_entry->x_0=0;
	 proj_entry->y_0=0;
	 proj_entry->k=1.0;
	 proj_entry->lon_0=-9999;
	 proj_entry->lat_0=-9999;
	 proj_entry->zone=-9999;
	 
	 for(i=0;i<item_count;i++){
		 /*printf("token: %s\n",tokens[i]);*/
		 char **key_val=get_items(tokens[i],"=",&pair);
		
		 
		 if (key_val==NULL || pair!=2){
			 /*printf("proj4 token: %s not valid\n",tokens[i]);*/
			 continue;
		 }
		 /*parse proj key value pairs*/
		 /*printf("key: %s, val: %s\n",key_val[0],key_val[1]);*/
		 if (!strcmp(key_val[0],"proj"))
			 strncpy(proj_entry->proj,key_val[1],64);
		else if (!strcmp(key_val[0],"datum"))
			strncpy(proj_entry->datum,key_val[1],64);
		else if (!strcmp(key_val[0],"ellps"))
			strncpy(proj_entry->datum,key_val[1],64);
		else if (!strcmp(key_val[0],"units"))
			strncpy(proj_entry->units,key_val[1],16);
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
		else if (!strcmp(key_val[0],"k_0") || !strcmp(key_val[0],"k"))
			proj_entry->k=atof(key_val[1]);
		free(key_val);
	}
	free(tokens);
	free(no_spaces);
	return proj_entry;
}

int proj4_to_mlb(char *proj4_text, char *mlb){
	int is_transverse_mercator=0;
	proj4_entry *proj_entry=parse_proj4(proj4_text);
	if (proj_entry==NULL){
		lord_error(TR_LABEL_ERROR,"Unable to parse proj4 string: %s",proj4_text);
		return TR_LABEL_ERROR;
	}
	
	/*set projection*/
	
	if (proj_entry->proj[0]=='\0'){
		lord_error(TR_LABEL_ERROR,"Proj4: projection not set!");
		free(proj_entry);
		return TR_LABEL_ERROR;
	}
	if (!strcmp(proj_entry->proj,"utm")){
		strcpy(mlb,"utm");
		if (proj_entry->zone<0)
			return 1;
		sprintf(mlb+3,"%2d",proj_entry->zone);
		mlb+=5;
	}
	else if (!strcmp(proj_entry->proj,"latlong") || !(strcmp(proj_entry->proj,"longlat"))){
		strcpy(mlb,"geo");
		mlb+=3;
	}
	else if (!strcmp(proj_entry->proj,"tmerc") || !(strcmp(proj_entry->proj,"etmerc"))){
		strcpy(mlb,"itm");
		is_transverse_mercator=1;
		mlb+=3;
	}
	else{
		strcpy(mlb,"err");
		lord_error(TR_LABEL_ERROR,"Proj4-projection: %s, not supported.",proj_entry->proj);
		free(proj_entry);
		return TR_LABEL_ERROR;
	}
	/*set datum*/
	/*if no datum - set default*/
	if (proj_entry->datum[0]=='\0'){
		lord_warning(1,"Proj4-string, no datum, setting datum to WGS84.");
		strcpy(proj_entry->datum,"WGS84");
	}
	if (!strcmp(proj_entry->datum,"WGS84"))
		mlb+=sprintf(mlb,"_wgs84");
	else if (!strcmp(proj_entry->datum,"GRS80"))
		mlb+=sprintf(mlb,"_etrs89");
	else if (!strcmp(proj_entry->datum,"ED50"))
		mlb+=sprintf(mlb,"_ed50");
	else{
		lord_error(TR_LABEL_ERROR,"Proj4-datum: %s, not supported.",proj_entry->datum);
		return TR_LABEL_ERROR;
	}
	/*put extra parameters*/
	if (is_transverse_mercator)
		sprintf(mlb,"  %.2fdg  %.2f%s  %.2fdg  %.2f%s  %.8f",proj_entry->lat_0,proj_entry->y_0,proj_entry->units,
	proj_entry->lon_0,proj_entry->x_0,proj_entry->units,proj_entry->k);
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