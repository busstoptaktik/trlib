#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "KmsFncs.h"

static void convert2geo(char *mlb_in, char *mlb_out);
int apply_datum_shift(def_dtm_shift *shifts[], int n_shifts, double tlon, double tlat, double *out,int channels);

	
/*TODO - fix this up! */
static void convert2geo(char *mlb_in, char *mlb_out){
	short rgn,sep;
	char dtm[MLBLNG];
	char prj[MLBLNG];
	char *p;
	get_mlb(mlb_in,&rgn,prj,&sep,dtm,&p);
	sprintf(mlb_out,"geoE%s",dtm);
	lord_debug(0,LORD("in: %s, out: %s"),mlb_in,mlb_out);
}

gd_state *gd_open(struct coord_lab *lab_in, struct coord_lab *lab_out, char *extra, tab_dir *tdir){
	gd_state         *self;
	char buf[MLBLNG];
	self=calloc(sizeof(gd_state),1);
	/* determine where to start the ETPL*/
	if (IS_CARTESIC(lab_in))
		self->start=2;
	else if (IS_GEOGRAPHIC(lab_in))
		self->start=1;
	else{
		self->start=0;
		self->tcgeo_in=calloc(sizeof(struct coord_lab),1);
		/*TODO: err check*/
		/* setup special functions and gateways*/
		switch(lab_in->imit){
			case DKMASK: /* dk_trans */
			case EDMASK: /* dk_trans */
				conv_w_crd("DK_geo_ed50",  self->tcgeo_in, tdir->def_lab);
				self->trf_in = dk_trans;
				break;
			/*TODO: other cases*/
			default:
				convert2geo(lab_in->mlb,buf);
				conv_w_crd(buf,self->tcgeo_in,tdir->def_lab);
				break;
		}
	}
	if (IS_CARTESIC(lab_out))
		self->stop=3;
	else if (IS_GEOGRAPHIC(lab_out))
		self->stop=4;
	else{
		self->stop=5;
		/*TODO: error check*/
		self->tcgeo_out=calloc(sizeof(struct coord_lab),1);
		switch(lab_out->imit){
			case DKMASK: /* dk_trans */
			case EDMASK: /* dk_trans */
				conv_w_crd("DK_geo_ed50",  self->tcgeo_out, tdir->def_lab);
				self->trf_out = dk_trans;
				break;
			/*TODO: other cases*/
			default:
				convert2geo(lab_out->mlb,buf);
				conv_w_crd(buf,self->tcgeo_out,tdir->def_lab);
				break;
			}
	}
	if (GET_HDTM(lab_in)!=GET_HDTM(lab_out)){
		self->dh_route=calloc(sizeof(htr_route),1);
		find_htr_route(GET_HDTM(lab_in),GET_HDTM(lab_out),self->dh_route,tdir->def_lab->dtm_shifts,tdir->def_lab->n_dtm_shifts);
	}
		/*TODO: err check*/
	else
		self->dh_route=NULL;
	if (GET_HDTM(lab_in)!=200){ /*TODO: add extra conditions*/
		self->geoid_route=calloc(sizeof(htr_route),1);
		find_htr_route(200,GET_HDTM(lab_in),self->geoid_route,tdir->def_lab->dtm_shifts,tdir->def_lab->n_dtm_shifts);
	}
	else
		self->geoid_route=NULL;
	self->lab_in=lab_in;
	self->lab_out=lab_out;
	self->tdir=tdir;
	return self;
}


void gd_close(gd_state *self){
	free(self); /*TODO - free all*/
}

int apply_datum_shift(def_dtm_shift *shifts[], int n_shifts, double tlon, double tlat, double *out,int channels){
	/*only one channel for now*/
	int i,j,c,n_ok=0,ok=0;
	double res,v;
	for(c=0; c<channels; c++){
		for(i=0; i<n_shifts; i++){
			res=0;
			ok=0;
			for(j=0;j<shifts[i]->n_tabs; j++){
				/*todo: use invert*/
				v=grim_value(shifts[i]->g[j],tlat,tlon,0,0);
				if (v!=HUGE_VAL){
					res+=v;
					ok=1;
					break;
				}
					
			}
			if (!ok)
				return 1;
		}
	}
	if (n_ok!=channels)
		return 1;
	return 0;
	
}

int gd_trans(gd_state *state, double N, double E, double H, double *No, double *Eo, double *Ho, double *gh){
	struct coord_lab *in_step=NULL;
	double tlat=0,tlon=0,tE=0,HH;
	tab_dir *tdir=state->tdir;
	*No=N;
	*Eo=E;
	*Ho=H;
	if (!state)
		return 1;
	if (state->start>=state->stop)
		return 0;
	/* prj -> geo */
	if (state->start==0){
		if (state->trf_in!=NULL)
			state->trf_in(state->lab_in,state->tcgeo_in,N,E,0,&N,&E,&HH,tdir); 
		else
			ptg(state->lab_in,1,N,E,&N,&E,"",NULL);
		
		in_step=state->tcgeo_in;
	}
	else 
		in_step=state->lab_in;
	
	/* GO UP ?? */
	if (state->start==2 || GET_DTM(in_step)!=GET_DTM(state->lab_out) || GET_HDTM(state->lab_in)!=GET_HDTM(state->lab_out) || state->stop==3){
		/*we need to go up - test which way to go... */
		if (GET_HDTM(state->lab_in)!=200 && state->start<2){ /* go to the complex way - via a geoid*/
			double X,Y,Z;
			/*trick: assume H is ellipsoidal height - the output table cell should be more or less independent of the actual h
			as long as h is small compared to R and the axes of the table system and the input system are more or less aligned*/
			gtc(in_step,1,N,E,H,&X,&Y,&Z,"",NULL); /*go up from fake 'geoE' to crt*/
			ctc(in_step,state->dh_route->clab,X,Y,Z,&X,&Y,&Z,tdir); /*change to table dtm*/
			gtc(state->dh_route->clab,-1,X,Y,Z,&tlat,&tlon,&tE,"",NULL); /*go down to geo_coords*/
			apply_datum_shift(state->geoid_route->shifts,state->geoid_route->n_shifts,tlat,tlon,&Z,1); /*get the geoid height*/
			/*add the geoid height to H and we're pretty much in Ellipsodal heights*/
			H+=Z;
			gtc(in_step,1,N,E,H,&E,&N,&H,"",NULL); /*now go to the true 3d coords*/
			ctc(in_step,state->tcgeo_out,E,N,H,&E,&N,&H,tdir);
		}
		/* the simple way - geoE->crt_  or start==2*/
		else{
			if (state->start<2) /*else we're already in cartesian coords*/
				gtc(in_step,1,N,E,H,&E,&N,&H,"",NULL);
			if (state->dh_route!=NULL){/*do we need table values??*/
				ctc(in_step,state->dh_route->clab,E,N,H,&E,&N,&H,tdir); /*TODO - make this a call to apply_datum_shift - check if ctc does nothing for equal datums*/
				gtc(in_step,-1,E,N,H,&tlat,&tlon,&tE,"",NULL);
				/* Check if we're alreay done?*/
				if (state->stop==4 && GET_DTM(state->lab_out)==GET_DTM(state->dh_route->clab)){
					N=tlat;
					E=tlon; /*hmm what is H now??*/
					H=tE;
					goto h_transform;
				}
				ctc(state->dh_route->clab,state->tcgeo_out,E,N,H,&E,&N,&H,tdir); 
			}
			else/*no table values needed*/
				ctc(in_step,state->tcgeo_out,E,N,H,&E,&N,&H,tdir);
		}
		/*now we're in crt_odtm - no h_transform needed*/
		if (state->stop==3){
			*No=N;
			*Eo=E;
			*Ho=H;
			return 0;
		}
		/*else go down to geoE*/
		gtc(state->tcgeo_out,-1,E,N,H,&N,&E,&H,"",NULL);
		if (state->stop==4)
			goto h_transform;
	}
	/*END 3D-SECTION*/
	/*last step*/
	if (state->trf_out!=NULL)
		state->trf_out(state->tcgeo_out,state->lab_out,N,E,0,&N,&E,&HH,tdir);
	else
		ptg(state->lab_out,-1,N,E,&N,&E,"",NULL);
	h_transform:
		if (state->dh_route!=NULL){
			apply_datum_shift(state->dh_route->shifts,state->dh_route->n_shifts,tlat,tlon,Ho,1);
			H+=*Ho;
		}
			
		
	*Eo=E;
	*No=N;
	*Ho=H;
	return 0;
}
