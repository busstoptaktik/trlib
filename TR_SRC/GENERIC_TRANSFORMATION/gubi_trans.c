#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "KmsFncs.h"

static void convert2geo(char *mlb_in, char *mlb_out);
int apply_datum_shift(def_dtm_shift *shifts[], int *direction, int n_shifts, double tlat, double tlon, double *out,int channels);

	
/*TODO - fix this up! */
static void convert2geo(char *mlb_in, char *mlb_out){
	short sep;
	char dtm[MLBLNG];
	char prj[MLBLNG];
	char *p;
	get_mlb(mlb_in,prj,&sep,dtm,&p);
	sprintf(mlb_out,"geoE%s",dtm);
	lord_debug(0,LORD("in: %s, out: %s"),mlb_in,mlb_out);
}

gd_state *gd_open(struct coord_lab *lab_in, struct coord_lab *lab_out, char *extra, tab_dir *tdir){
	gd_state         *self;
	static int init=0;
	static struct coord_lab tlab;
	int res;
	char buf[MLBLNG];
	if (!init){ /*TABLE system - TODO: put this somewhere else....*/
		conv_w_crd("geo_wgs84",&tlab,tdir->def_lab);
		init=1;
	}
	if (!lab_in || !lab_out || lab_in->lab_type!=CRD_LAB || lab_out->lab_type!=CRD_LAB){
		lord_error(TR_ALLOCATION_ERROR,LORD("Input crs structures not initialised."));
		return NULL;
	}
	lord_debug(0,LORD("mlb1: %s, hdtm1: %d, mlb2: %s, hdtm2: %d, dtm2: %d"),GET_MLB(lab_in),GET_HDTM(lab_in),GET_MLB(lab_out),GET_HDTM(lab_out),GET_DTM(lab_out));
	self=calloc(sizeof(gd_state),1);
	if (!self){
		lord_error(TR_ALLOCATION_ERROR,LORD("Failed to allocate memory."));
		return NULL;
	}
	/*set datum numbers*/
	self->dtm_in=GET_DTM(lab_in);
	if (GET_HDTM(lab_in)==0)
		self->hdtm_in=200; /*TODO - get from tdir */
	else
		self->hdtm_in=GET_HDTM(lab_in);
	self->dtm_out=GET_DTM(lab_out);
	if (GET_HDTM(lab_out)==0)
		self->hdtm_out=200; /*TODO - get from tdir */
	else
		self->hdtm_out=GET_HDTM(lab_out);
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
				res=conv_w_crd("DK_geo_ed50",  self->tcgeo_in, tdir->def_lab);
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
				res=conv_w_crd("DK_geo_ed50",  self->tcgeo_out, tdir->def_lab);
				self->trf_out = dk_trans;
				break;
			/*TODO: other cases*/
			default:
				convert2geo(lab_out->mlb,buf);
				conv_w_crd(buf,self->tcgeo_out,tdir->def_lab);
				break;
			}
	}
	if (self->hdtm_in!=self->hdtm_out && self->hdtm_out!=200){
		lord_debug(0,LORD("Finding dh-route..."));
		self->dh_route=calloc(sizeof(htr_route),1);
		if (!self->dh_route){
			lord_error(TR_ALLOCATION_ERROR,LORD("Failed to allocated memory."));
			goto cleanup;
		}
		if (find_htr_route(self->hdtm_in,self->hdtm_out,self->dh_route,tdir->def_lab->dtm_shifts,tdir->def_lab->n_dtm_shifts)!=0){
			lord_error(TRF_ILLEG_,LORD("No route from input height datum %d to output height datum %d"),self->hdtm_in,self->hdtm_out);
			goto cleanup;
		}
		self->dh_route->clab=&tlab; /*TODO.: fix this...*/
	}
		/*TODO: err check*/
	else
		self->dh_route=NULL;
	if (self->hdtm_in!=200){ 
		self->geoid_route=calloc(sizeof(htr_route),1);
		if (!self->geoid_route){
			lord_error(TR_ALLOCATION_ERROR,LORD("Failed to allocated memory."));
			goto cleanup;
		}
		if (find_htr_route(self->hdtm_in,200,self->geoid_route,tdir->def_lab->dtm_shifts,tdir->def_lab->n_dtm_shifts)!=0){
			lord_error(TRF_ILLEG_,LORD("No route from input height datum %d to ellipsoid."),self->hdtm_in);
			goto cleanup;
		}
		self->geoid_route->clab=&tlab;
	}
	else
		self->geoid_route=NULL;
	self->lab_in=lab_in;
	self->lab_out=lab_out;
	self->tdir=tdir;
	return self;
	cleanup:
		gd_close(self);
		return NULL;
}


void gd_close(gd_state *self){
	if (!self)
		return;
	free(self->tcgeo_in);
	free(self->tcgeo_out);
	free(self->geoid_route);
	free(self->dh_route);
	free(self);
	return;
}

int apply_datum_shift(def_dtm_shift *shifts[], int *direction, int n_shifts, double tlat, double tlon, double *out,int channels){
	/*only one channel for now - todo...*/
	int i,j,c,ok=0;
	double v=0,res=0;
	lord_debug(0,LORD("Datum shift called with lat: %.7f lon: %.7f"),tlat*DOUT,tlon*DOUT);
	for(c=0; c<channels; c++){
		res=0;
		for(i=0; i<n_shifts; i++){
			ok=0;
			lord_debug(0,LORD("n_tabs: %d"),shifts[i]->n_tabs);
			for(j=0;j<shifts[i]->n_tabs; j++){
				lord_debug(0,LORD("Grid: %s, mlb: %s, adress: %p"),grim_filename(shifts[i]->g[j]),grim_crs(shifts[i]->g[j]),shifts[i]->g[j]);
				lord_debug(0,LORD("rows: %d, cols: %d"),grim_rows(shifts[i]->g[j]),grim_columns(shifts[i]->g[j]));
				lord_debug(0,LORD("row: %.2f, col: %.2f"),grim_row(shifts[i]->g[j],tlat,tlon),grim_column(shifts[i]->g[j],tlat,tlon));
				lord_debug(0,LORD("Direction is: %d"),direction[i]);
				v=grim_value(shifts[i]->g[j],tlat,tlon,0,0);
				lord_debug(0,LORD("v is %.4f"),v);
				if (v!=HUGE_VAL){
					res+=v*direction[i];
					ok=1;
					break;
				}
					
			}
			if (!ok)
				return 1;
		}
		out[c]=res;
	}
	return 0;
}

int updown(struct coord_lab *lab1, struct coord_lab *lab2, double N, double E, double H, double *out, tab_dir *tdir){
	int res;
	lord_debug(0,"**** updown: input - lat: %.10f, lon: %.10f, H: %.5f",N*DOUT,E*DOUT,H);
	/*geo to crt - up */
	res=gtc(lab1,1,N,E,H,out,out+1,out+2,"",NULL);
	lord_debug(0,"****** res: %d",res);
	lord_debug(0,"****** crt_i_dtm : %.10f, %.10f, %.10f",out[0],out[1],out[2]);
	/*change dtm*/
	/*check this step - seems to give differences to old trlib! - debug set_dtm1 and conv_crd2*/
	res=ctc(lab1,lab2,out[0],out[1],out[2],out,out+1,out+2,tdir);
	lord_debug(0,"*******ctc: in: dtm: %d, p_dtm: %d, out:  dtm: %d, p_dtm: %d",GET_DTM(lab1),GET_PDTM(lab1),GET_DTM(lab2),GET_PDTM(lab2));
	lord_debug(0,"********ctc: dsh1: %.10f %.3f %.3f %.3f",lab1->dsh_con.scale,lab1->dsh_con.tx, lab1->dsh_con.ty,lab1->dsh_con.tz);
	lord_debug(0,"********ctc: dsh1: %.8f %.8f %.8f %.8f %8f",lab1->dsh_con.r11,lab1->dsh_con.r12, lab1->dsh_con.r13,lab1->dsh_con.r22,lab1->dsh_con.r23);
	if (GET_DTM(lab2)!=1){
		lord_debug(0,"********ctc: dsh1: %.10f %.3f %.3f %.3f",lab2->dsh_con.scale,lab2->dsh_con.tx, lab2->dsh_con.ty,lab2->dsh_con.tz);
		lord_debug(0,"********ctc: dsh1: %.8f %.8f %.8f %.8f %8f",lab2->dsh_con.r11,lab2->dsh_con.r12, lab2->dsh_con.r13,lab2->dsh_con.r22,lab2->dsh_con.r23);
	}
	lord_debug(0,"****** res: %d",res);
	lord_debug(0,"****** crt_o_dtm : %.10f, %.10f, %.10f",out[0],out[1],out[2]);
	/*down to geoE in dtm2*/
	res=gtc(lab2,-1,out[0],out[1],out[2],out,out+1,out+2,"",NULL);
	lord_debug(0,"****** res: %d",res);
	lord_debug(0,"**** updown: output - lat: %.10f, lon: %.10f, H: %.5f",out[0]*DOUT,out[1]*DOUT,out[2]);
	return res;
}

int gd_trans(gd_state *state, double N, double E, double H, double *No, double *Eo, double *Ho, double *gh){
	struct coord_lab *in_step=NULL;
	double tlat=0,tlon=0,tE=0,HH;
	int res;
	htr_route *geoid_route, *dh_route;
	tab_dir *tdir=state->tdir;
	geoid_route=state->geoid_route;
	dh_route=state->dh_route;
	*No=N;
	*Eo=E;
	*Ho=H;
	if (!state){
		lord_error(TR_ALLOCATION_ERROR,LORD("gd_state is NULL"));
		return TR_ALLOCATION_ERROR;
	}
	/*TODO: check for NO transformation, or h only*/
	if (state->start>=state->stop) /*h transform only??*/
		return TR_OK;
	/* prj -> geo */
	if (state->start==0){
		lord_debug(0,LORD("*** ptg ***"));
		if (state->trf_in!=NULL){
			lord_debug(0,LORD("Special ptg...!"));
			state->trf_in(state->lab_in,state->tcgeo_in,N,E,0,&N,&E,&HH,tdir); 
		}
		else
			ptg(state->lab_in,1,N,E,&N,&E,"",NULL);
		
		in_step=state->tcgeo_in;
	}
	else 
		in_step=state->lab_in;
	
	/* GO UP ??  - check logic here...*/
	/*TODO: allow different ref-systems for geoid_route and dh_route??*/
	if (state->start==2 || GET_DTM(in_step)!=GET_DTM(state->lab_out) || state->hdtm_in!=state->hdtm_out || state->stop==3){
		/*we need to go up - test which way to go... */
		if (state->hdtm_in!=200 && state->start<2){ /* go to the complex way - via a geoid*/
			double out[3],dh;
			int n_iter=0;
			if (!geoid_route){
				lord_error(TAB_C_NON_,LORD("Geoid table is NULL - unable to perform this transformation."));
				return TAB_C_NON_;
			}
			/*trick: assume H is ellipsoidal height - the output table cell should be more or less independent of the actual h
			as long as h is small compared to R and the axes of the table system and the input system are more or less aligned*/
			lord_debug(0,LORD("*** Taking the scenic gtc route ***"));
			lord_debug(0,LORD("Input hdtm is: %d, output hdtm is: %d, H is: %.3f"),state->hdtm_in, state->hdtm_out,H);
			dh=1;
			while (dh>0.02 && n_iter<4){
				/*This is converging fast - a kind of Newton-Rapson which works well if the ellipsoids are not 'very' misaligned and the geoid is not too 'bumpy'*/ 
				n_iter++;
				lord_debug(0,LORD(" +++++++++++++  Iteration %d +++++++++++++"),n_iter);
				lord_debug(0,LORD("geo-coords with 'fake' ellipsoidal heights: %.4f %.4f %.4f"),N*DOUT,E*DOUT,H);
				res=updown(in_step,geoid_route->clab,N,E,H,out,state->tdir);
				/*now out is N,E,E_h in table dtm*/
				lord_debug(0,LORD("'fake' geoE coords in table dtm: lat: %.7f lon:%.7f E:%.4f"),out[0]*DOUT,out[1]*DOUT,out[2]);
				/*TODO: only get table values if we have moved 'significantly' (first iteration)*/
				res=apply_datum_shift(geoid_route->shifts,geoid_route->direction,geoid_route->n_shifts,out[0],out[1],out+2,1); /*get the geoid height*/
				if (res!=0)
					return TAB_C_ARE_;
				out[2]+=*Ho;
				lord_debug(0,LORD("'real' geoE coords in table dtm: lat: %.7f lon:%.7f E:%.4f"),out[0]*DOUT,out[1]*DOUT,out[2]);
				tlat=out[0];
				tlon=out[1];
				/*now reverse*/
				res=updown(geoid_route->clab,in_step,out[0],out[1],out[2],out,state->tdir);
				lord_debug(0,LORD("geoE in input dtm: lat: %.7f  lon:%.7f  E:%.4f"),out[0]*DOUT,out[1]*DOUT,out[2]);
				/*convergence can also be checked by looking at out[0]-N and out[1]-E*/
				dh=fabs(H-out[2]);
				lord_debug(0,LORD("dh is: %.8f"),dh);
				H=out[2];
			}
			/*TODO: if bad convergence return TRF_TOLE_*/
			gtc(in_step,1,N,E,H,&E,&N,&H,"",NULL);
			lord_debug(0,LORD("crt coords in input dtm: %.2f %.2f %.2f"),E,N,H); 
			ctc(in_step,state->tcgeo_out,E,N,H,&E,&N,&H,tdir);
			lord_debug(0,LORD("crt coords in output dtm: %.2f %.2f %.2f"),E,N,H); 
		}
		/* the simple way - geoE->crt_  or start==2*/
		else{
			lord_debug(0,LORD("*** Taking the simple gtc route ***"));
			if (state->start<2) /*else we're already in cartesian coords*/
				gtc(in_step,1,N,E,H,&E,&N,&H,"",NULL);
			if (dh_route!=NULL){/*do we need table values - then go via table_datum??*/
				lord_debug(0,LORD("We need table values - so go via table datum."));
				ctc(in_step,dh_route->clab,E,N,H,&E,&N,&H,tdir); /*TODO - make this a call to apply_datum_shift - check if ctc does nothing for equal datums*/
				gtc(in_step,-1,E,N,H,&tlat,&tlon,&tE,"",NULL);
				/* Check if we're alreay done?*/
				if (state->stop==4 && state->dtm_out==GET_DTM(dh_route->clab)){
					N=tlat;
					E=tlon; /*hmm what is H now??*/
					H=tE;
					goto h_transform;
				}
				ctc(dh_route->clab,state->tcgeo_out,E,N,H,&E,&N,&H,tdir); 
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
		lord_debug(0,LORD("*** gtc 'down' ***"));
		gtc(state->tcgeo_out,-1,E,N,H,&N,&E,&H,"",NULL);
		lord_debug(0,LORD("geo-coords in output datum with 'real' ellipsoidal heights: %.4f %.4f %.4f"),N*DOUT,E*DOUT,H); 
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
		if (state->hdtm_out!=200) /*restore h*/
			H=*Ho;
		if (dh_route!=NULL){
			lord_debug(0,LORD("Applying h-dtm shift from %d to %d"),state->hdtm_in,state->hdtm_out);
			res=apply_datum_shift(dh_route->shifts,dh_route->direction,dh_route->n_shifts,tlat,tlon,Ho,1);
			if (res!=0)
				return TAB_C_ARE_;
			H+=*Ho;
		}
		
			
		
	*Eo=E;
	*No=N;
	*Ho=H;
	return TR_OK;
}
