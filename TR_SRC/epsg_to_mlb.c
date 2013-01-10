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
 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sputshpprj.h"
#include "geo_lab.h"
#include "conv_lab.h"
#include "get_mlb.h"
#include "trlib_intern.h"
#include "trlib_api.h"
#include "proj4_to_mlb.h"
#include "epsg_to_mlb.h"
#include "lord.h"

/* Mock up of EPSG code handling. 
TODO: improve, perhaps using some intelligent guess work in order to decrease sice of cached table. 
Much of this HAS been done regarding utm-projections. */

#define EPSG_TABLE_MAX 256   /* less than 25 kb memory footprint!! */

/*setup global read onlys */

struct EPSG_ENTRY{
	int code1,code2,start_zone;
	char mlb[2*MLBLNG];
};

typedef struct EPSG_ENTRY EPSG_ENTRY;


static EPSG_ENTRY *EPSG_TABLE=NULL; 
static int EPSG_ENTRIES=0;


/* Lookup functions */
    
static EPSG_ENTRY *search_epsg_table(char *mlb, int code, int mode){
	int i=0;
	while(i<EPSG_ENTRIES){
		/*mode 1: look for code given mlb */
		if (mode==1 && !strcmp(EPSG_TABLE[i].mlb,mlb))
			return EPSG_TABLE+i;
		/*mode 0: look for mlb given code */
		if (mode==0 && EPSG_TABLE[i].code1<=code && EPSG_TABLE[i].code2>=code)
			return EPSG_TABLE+i;
		
		i++;}
	return 0;
}

void release_epsg_table(void){
	if (EPSG_TABLE)
		free(EPSG_TABLE);
}

int setup_epsg_table(FILE *f_in){
	
	int done=0, code1, code2, zone, argc;
	char mlb[2*MLBLNG];
	EPSG_ENTRY entry;
	enum {BUFSIZE = 16384};
	char buf[BUFSIZE];
	EPSG_TABLE=malloc(sizeof(EPSG_ENTRY)*EPSG_TABLE_MAX);
	if (!EPSG_TABLE){
		lord_error(TR_ALLOCATION_ERROR,LORD("Failed to allocate space."));
		return TR_ALLOCATION_ERROR;
	}
	while (0 != fgets(buf, BUFSIZE, f_in) && done<EPSG_TABLE_MAX) {
            argc = sscanf(buf, "%d %s %d %d", &code1, mlb, &code2, &zone);
	    if (argc<2)
		    continue;
	    entry.code1=code1;
	    entry.code2=code1;
	    strcpy(entry.mlb,mlb);
	    entry.start_zone=0;
	    if (argc==4){
		    entry.code2=code2;
		    entry.start_zone=zone;
	    }
	    EPSG_TABLE[done++]=entry;
		}
	EPSG_ENTRIES=done;
	EPSG_TABLE=realloc(EPSG_TABLE,sizeof(EPSG_ENTRY)*EPSG_ENTRIES);
	return done;
}




/*Need to determine the logic of how to handle the translation: e.g. 2d+1d or whatever....*/	
int export_to_epsg(char *mlb, int *h_code, int *v_code){
	int utm_zone=0;
	char mlb1[2*MLBLNG], mlb2[2*MLBLNG],mlb_src[2*MLBLNG+1], *h_datum,*utm_scan=0;
	EPSG_ENTRY *found;
	short sepch,region;
	
	if (EPSG_TABLE==NULL){
		lord_error(TR_ALLOCATION_ERROR,"EPSG table not initialised.");
		return TR_ALLOCATION_ERROR;
	}
	/*search for a perfect match - possibly a compound system*/
	
	if ((found=search_epsg_table(mlb,0,1))){
		*h_code=found->code1;
		*v_code=1; /* signal that we have a perfect match */
		return TR_OK;
	}
		
	/*secondly search for 2d-match*/
		
	get_mlb(mlb,&region,mlb1,&sepch,mlb2,&h_datum);
	
	/*first see if this is a utm system */
  if ((utm_scan=strstr(mlb1,"utm"))){
		if ((utm_zone=atoi(mlb1+3)))
			strcpy(mlb1,"utm");
	}
	normalise_datum(mlb2);
	strcpy(mlb_src,mlb1);
	strcat(mlb_src,"_");
	strcat(mlb_src,mlb2);
	found=search_epsg_table(mlb_src,0,1);
	if (!found)
		return TR_LABEL_ERROR;
	if (utm_zone && found->code1<found->code2){
		if ((utm_zone<found->start_zone) || (utm_zone>(found->code2-found->code1+found->start_zone))){
			#ifdef _ROUT
			lord_debug(0,"EPSG: Wrong zone: %d, start_zone is: %d\n",utm_zone,found->start_zone);
		        #endif
			return TR_LABEL_ERROR;
		}
		*h_code=found->code1+(utm_zone-found->start_zone);
		}
	else
		*h_code=found->code1;
	switch (sepch){
		case '_':
		case 'L':
			break;
		case 'E':
			*v_code=10;
		         break;
		case 'N':
			*v_code=20;
		        break;
		case 'H':
			found=search_epsg_table(h_datum,0,1);
			if (!found)
				*v_code=-1;
			else /*TODO: test if this is a vertical datum! */
				*v_code=found->code1;
			break;
		default:
			return TR_LABEL_ERROR;
	}
	return TR_OK;
}


int import_from_epsg(int h_code, int v_code, char *mlb){
	EPSG_ENTRY *found;
	char mlb1[2*MLBLNG], mlb2[2*MLBLNG],buf[2*MLBLNG],*h_datum,*utm_scan;
	short sepch,region;
	if (EPSG_TABLE==NULL){
		lord_error(TR_ALLOCATION_ERROR,"EPSG table not initialised.");
		return TR_ALLOCATION_ERROR;
	}
	/*first look for horisontal part - or possibly a matching compound system*/
	found=search_epsg_table(0,h_code,0);
	if (!found){
		return TR_LABEL_ERROR;
	}
	/*If utm system - append zone number after 'utm'*/
	if (found->code1<found->code2 && (utm_scan=strstr(found->mlb,"utm"))){
		sprintf(buf,"utm%02d%s",(found->start_zone+(h_code-found->code1)),utm_scan+3);
		get_mlb(buf,&region,mlb1,&sepch,mlb2,&h_datum);
		strcpy(mlb,buf);
	}
	else{
		get_mlb(found->mlb,&region,mlb1,&sepch,mlb2,&h_datum);
		strcpy(mlb,found->mlb);
	}
	if (v_code>0 && (sepch!='_')){
		lord_error(TR_LABEL_ERROR,"EPSG: found matching 3d-system from horisontal part while vertical code is also specified.");
		return TR_LABEL_ERROR;
	}
	if (v_code>0){
		/*look for vertical datum */
		found=search_epsg_table(0,v_code,0);
		if (!found){
			/*can debate if we should really return an error here... */
			lord_error(TR_LABEL_ERROR,"EPSG: found horisontal part - no match for vertical code."); 
			return TR_LABEL_ERROR;
		}
		/*compose minilabel */
		strcpy(mlb,mlb1);
		strcat(mlb,"H");
		strcat(mlb,mlb2);
		strcat(mlb,"_");
		strcat(mlb,found->mlb);
	}
	return TR_OK;
}
	


    
    
