#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "KmsFncs.h"

/*the bulding blocks of a transformation*/
#define PTG    0    /*proj to geo*/
#define GTC    1    /*geo to cartesian*/
#define DGE    3      /*differential way to geoE*/
#define TCTC  5    /*simple way to crt_table_dtm*/
#define CTC    6   /*cartesian to cartesian (datum shift)*/
#define HTH    7   /* height transformation - or just STOP*/
#define CTG    8   /* cartesian to geo*/
#define GTP    9  /*  geo to proj*/
#define IDT    10  /*do nothing */

/* get to 3D - perform dshifts - go down*/
/* DGE and TCTC will produce table_lat and table_lon */
/* Entry point for crt_coords is always second slot of TO3D*/
/*roads which need to go over table datum  */
static int D3_TO_TABLE1[4]={DGE,GTC,CTC,CTG}; /*input not in 'E' - what if output is geoEsame_dtm??*/
static int D3_TO_TABLE2[4]={GTC,TCTC,CTC,CTG}; /*input in 'E'*/
/*simple road - not over table dtm - input in 'E'*/
static int D3_SIMPLE[4]={GTC,IDT,CTC,CTG};
/*another simple road
PTG->DGTC->...->CTG  covered by D3_TO_TABLE1*/
/* DGTC shall produce crt in input dtm...*/

static void convert2geo(char *mlb_in, char *mlb_out);
int apply_datum_shift(def_dtm_shift *shifts[], int *direction, int n_shifts, double tlat, double tlon, double *out,int channels);

	
/*Not used at the moment...*/
static void convert2geo(char *mlb_in, char *mlb_out){
	short sep;
	char dtm[MLBLNG];
	char prj[MLBLNG];
	char *p;
	get_mlb(mlb_in,prj,&sep,dtm,&p);
	sprintf(mlb_out,"geoE%s",dtm);
	
}


/*open an transformation 'state' - set up h-datum-shift tables and the sequence of actions*/
gd_state *gd_open(struct coord_lab *lab_in, struct coord_lab *lab_out, char *extra, tab_dir *tdir){
	gd_state         *self;
	static int init=0;
	static struct coord_lab tlab;
	int res,next,to_table_dtm,go_3d;
	char buf[MLBLNG];
	if (!init){ /*TABLE system - TODO: put this somewhere else....*/
		conv_w_crd("geoEwgs84",&tlab,tdir->def_lab);
		init=1;
	}
	if (!lab_in || !lab_out || lab_in->lab_type!=CRD_LAB || lab_out->lab_type!=CRD_LAB){
		lord_error(TR_ALLOCATION_ERROR,LORD("Input crs structures not initialised."));
		return NULL;
	}
	self=calloc(sizeof(gd_state),1);
	if (!self){
		lord_error(TR_ALLOCATION_ERROR,LORD("Failed to allocate memory."));
		return NULL;
	}
	self->table_system=&tlab;
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
	
	self->free_tc_in=0;
	/* determine where to start the ETPL*/
	if (IS_PROJECTION(lab_in)){
		switch(lab_in->imit){
			case DKMASK: /* dk_trans */
			case EDMASK: /* dk_trans */
				self->tcgeo_in=calloc(sizeof(struct coord_lab),1);
				self->free_tc_in=1;
				res=conv_w_crd("DK_geo_ed50",  self->tcgeo_in, tdir->def_lab);
				self->trf_in = dk_trans;
				self->dtm_in=GET_DTM(self->tcgeo_in); /*reset dtm number*/
				break;
			/*TODO: other cases*/
			default:
				/*
				convert2geo(lab_in->mlb,buf);
				conv_w_crd(buf,self->tcgeo_in,tdir->def_lab);*/
				break;
		}
	}
	self->tcgeo_out=lab_out;
	self->free_tc_out=0;
	if (IS_PROJECTION(lab_out)){
		switch(lab_out->imit){
			case DKMASK: /* dk_trans */
			case EDMASK: /* dk_trans */
				/*TODO: error check*/
				self->free_tc_out=1;
				self->tcgeo_out=calloc(sizeof(struct coord_lab),1);
				res=conv_w_crd("DK_geo_ed50",  self->tcgeo_out, tdir->def_lab);
				self->trf_out = dk_trans;
				self->dtm_out=GET_DTM(self->tcgeo_out); /*reset dtm number*/
				break;
			/*TODO: other cases*/
			default:
				/*convert2geo(lab_out->mlb,buf);
				conv_w_crd(buf,self->tcgeo_out,tdir->def_lab);*/
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
	/*set actions*/
	/* to table dtm: hdtm1!=hdtm2 or not E in*/
	/* to3d:            to_table_dtm || crt_out || datum_shift || crt_in */
	next=0;
	to_table_dtm=((self->hdtm_in != self->hdtm_out) || self->hdtm_in!=200);
	go_3d=(to_table_dtm || IS_CARTESIC(lab_in) || IS_CARTESIC(lab_out) || (self->dtm_in != self->dtm_out));
	if (IS_PROJECTION(lab_in))
		self->actions[next++]=PTG;
	if (go_3d){
		int start=0,stop=4,i,*to3d;
		if (to_table_dtm){
			if (self->hdtm_in==200)
				to3d=D3_TO_TABLE2;
			else
				to3d=D3_TO_TABLE1;
		}
		else
			to3d=D3_SIMPLE;
		if (IS_CARTESIC(lab_in))
			start=1;
		if (IS_CARTESIC(lab_out))
			stop=3;
		/*copy actions*/
		for(i=start;i<stop; i++){
			if (to3d[i]!=IDT)
				self->actions[next++]=to3d[i];
		}
	}
	if (IS_PROJECTION(lab_out)){
		self->actions[next++]=GTP;
	}
	self->actions[next++]=HTH; /*termination signal*/
	/*end set actions*/
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
	if (self->free_tc_in)
		free(self->tcgeo_in);
	if (self->free_tc_out)
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
	#ifdef HARD_DEBUG
	lord_debug(0,LORD("Datum shift called with lat: %.7f lon: %.7f"),tlat*DOUT,tlon*DOUT);
	#endif
	for(c=0; c<channels; c++){
		res=0;
		for(i=0; i<n_shifts; i++){
			ok=0;
			#ifdef HARD_DEBUG
			lord_debug(0,LORD("n_tabs: %d"),shifts[i]->n_tabs);
			#endif
			for(j=0;j<shifts[i]->n_tabs; j++){
				v=grim_value(shifts[i]->g[j],tlat,tlon,0,0);
				#ifdef HARD_DEBUG
				lord_debug(0,LORD("Grid: %s, mlb: %s, adress: %p"),grim_filename(shifts[i]->g[j]),grim_crs(shifts[i]->g[j]),shifts[i]->g[j]);
				lord_debug(0,LORD("rows: %d, cols: %d"),grim_rows(shifts[i]->g[j]),grim_columns(shifts[i]->g[j]));
				lord_debug(0,LORD("row: %.2f, col: %.2f"),grim_row(shifts[i]->g[j],tlat,tlon),grim_column(shifts[i]->g[j],tlat,tlon));
				lord_debug(0,LORD("Direction is: %d"),direction[i]);
				lord_debug(0,LORD("v is %.4f"),v);
				#endif
				if (v!=grim_nodata(shifts[i]->g[j])){ /*slightly inefficient to use a getter here - but what the heck...*/
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
	#ifdef HARD_DEBUG
	lord_debug(0,"**** updown: input - lat: %.10f, lon: %.10f, H: %.5f",N*DOUT,E*DOUT,H);
	#endif
	/*geo to crt - up */
	res=gtc(lab1,1,N,E,H,out,out+1,out+2,"",NULL);
	#ifdef HARD_DEBUG
	lord_debug(0,"****** res: %d",res);
	lord_debug(0,"****** crt_i_dtm : %.10f, %.10f, %.10f",out[0],out[1],out[2]);
	#endif
	/*change dtm*/
	/*check this step - seems to give differences to old trlib! - debug set_dtm1 and conv_crd2*/
	res=ctc(lab1,lab2,out[0],out[1],out[2],out,out+1,out+2,tdir);
	#ifdef HARD_DEBUG
	lord_debug(0,"*******ctc: in: dtm: %d, p_dtm: %d, out:  dtm: %d, p_dtm: %d",GET_DTM(lab1),GET_PDTM(lab1),GET_DTM(lab2),GET_PDTM(lab2));
	lord_debug(0,"****** res: %d",res);
	lord_debug(0,"****** crt_o_dtm : %.10f, %.10f, %.10f",out[0],out[1],out[2]);
	#endif
	/*down to geoE in dtm2*/
	res=gtc(lab2,-1,out[0],out[1],out[2],out,out+1,out+2,"",NULL);
	#ifdef HARD_DEBUG
	lord_debug(0,"****** res: %d",res);
	lord_debug(0,"**** updown: output - lat: %.10f, lon: %.10f, H: %.5f",out[0]*DOUT,out[1]*DOUT,out[2]);
	#endif
	return res;
}

int gd_trans(gd_state *state, double N, double E, double H, double *No, double *Eo, double *Ho, double *gh){
	struct coord_lab *in_step=NULL;
	double tlat=0,tlon=0,tE=0,HH;
	int res,i,n_iter,action;
	double out[3],dh;
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
	in_step=state->lab_in;
	for(i=0; i<7; i++){
		action=state->actions[i];
		switch(action){
			case IDT:
				lord_debug(0,LORD("*** idt ***"));
				break;
			case PTG:
				lord_debug(0,LORD("*** ptg ***"));
				if (state->trf_in!=NULL){
					lord_debug(0,LORD("Special ptg...!"));
				state->trf_in(state->lab_in,state->tcgeo_in,N,E,0,&N,&E,&HH,tdir);
				in_step=state->tcgeo_in;
				}
				else
					ptg(state->lab_in,1,N,E,&N,&E,"",NULL);
				break;
			case GTC:
				lord_debug(0,LORD("*** gtc ***"));
				gtc(in_step,1,N,E,H,&E,&N,&H,"",NULL);
				break;
			case TCTC:
				lord_debug(0,LORD("*** tctc ***"));
				if (GET_DTM(in_step)!=GET_DTM(state->table_system))
					ctc(in_step,state->table_system,E,N,H,&E,&N,&H,tdir);
				gtc(state->table_system,-1,E,N,H,&tlat,&tlon,&tE,"",NULL);
				in_step=state->table_system;
				break;
				
			/*case TGTC: 
				lord_debug(0,LORD("*** tgtc ***"));
				tlat=N;
				tlon=E;
				tE=H;
				gtc(in_step,1,N,E,H,&E,&N,&H,"",NULL); 
				if (GET_DTM(state->table_system)!=GET_DTM(in_step)){
					ctc(in_step,state->table_system,E,N,H,&E,&N,&H,tdir);
					gtc(state->table_system,-1,E,N,H,&tlat,&tlon,&tE,"",NULL);
				}
				in_step=state->table_system;
				break;*/
			case DGE: /*the complex road to geoE*/
				lord_debug(0,LORD("***  dge ***"));
				#ifdef HARD_DEBUG
				lord_debug(0,LORD("*** Taking the scenic gtc route ***"));
				lord_debug(0,LORD("Input hdtm is: %d, output hdtm is: %d, H is: %.3f"),state->hdtm_in, state->hdtm_out,H);
				#endif
				/*TODO: perhaps simplify if idtm=tdtm*/
				if (!geoid_route){
					lord_error(TAB_C_NON_,LORD("Geoid table is NULL - unable to perform this transformation."));
					return TAB_C_NON_;
				}
				/*shortcut if we're already in table_system*/
				if (GET_DTM(in_step)==GET_DTM(state->table_system)){
					res=apply_datum_shift(geoid_route->shifts,geoid_route->direction,geoid_route->n_shifts,N,E,out+2,1); /*get the geoid height*/
					if (res!=0)
						return TAB_C_ARE_;
					H+=out[2];
					tlat=N;
					tlon=E;
					tE=H;
					break;
				}
				/*trick: assume H is ellipsoidal height - the output table cell should be more or less independent of the actual h
				as long as h is small compared to R and the axes of the table system and the input system are more or less aligned*/
				n_iter=0;
				dh=1;
				while (dh>0.02 && n_iter<3){
					/*This is converging fast - a kind of Newton-Rapson which works well if the ellipsoids are not 'very' misaligned and the geoid is not too 'bumpy'*/ 
					n_iter++;
					#ifdef HARD_DEBUG
					lord_debug(0,LORD(" +++++++++++++  Iteration %d +++++++++++++"),n_iter);
					lord_debug(0,LORD("geo-coords with 'fake' ellipsoidal heights: %.7f %.7f %.4f"),N*DOUT,E*DOUT,H);
					#endif
					res=updown(in_step,state->table_system,N,E,H,out,state->tdir);
					/*now out is N,E,E_h in table dtm*/
					#ifdef HARD_DEBUG
					lord_debug(0,LORD("'fake' geoE coords in table dtm: lat: %.7f lon:%.7f E:%.4f"),out[0]*DOUT,out[1]*DOUT,out[2]);
					#endif
					/*TODO: only get table values if we have moved 'significantly' (first iteration)*/
					if (dh>0.3){
						res=apply_datum_shift(geoid_route->shifts,geoid_route->direction,geoid_route->n_shifts,out[0],out[1],out+2,1); /*get the geoid height*/
						if (res!=0)
							return TAB_C_ARE_;
					}
					out[2]+=*Ho;
					#ifdef HARD_DEBUG
					lord_debug(0,LORD("'real' geoE coords in table dtm: lat: %.7f lon:%.7f E:%.4f"),out[0]*DOUT,out[1]*DOUT,out[2]);
					#endif
					tlat=out[0];
					tlon=out[1];
					tE=out[2];
					/*now reverse*/
					res=updown(state->table_system,in_step,out[0],out[1],out[2],out,state->tdir);
					
					/*convergence can also be checked by looking at out[0]-N and out[1]-E*/
					dh=fabs(H-out[2]);
					H=out[2];
					#ifdef HARD_DEBUG
					lord_debug(0,LORD("geoE in input dtm: lat: %.7f  lon:%.7f  E:%.4f"),out[0]*DOUT,out[1]*DOUT,out[2]);
					lord_debug(0,LORD("dh is: %.8f"),dh);
					#endif
					/*TODO: REALLY, really do this. Another escape early test - would speed things up a lot!
					if (fabs(out[0]-N)<1e-7 etc...*/
					
					
				}
				/*TODO: if bad convergence return TRF_TOLE_*/
				/*no need to set in_step here!   in_step=state->tcgeo_in;*/
				break;
			case CTC:
				lord_debug(0,LORD("*** ctc ***"));
				if (GET_DTM(in_step)!=state->dtm_out)
					ctc(in_step,state->tcgeo_out,E,N,H,&E,&N,&H,tdir);
				break;
			case CTG:
				lord_debug(0,LORD("*** ctg ***"));
				gtc(state->tcgeo_out,-1,E,N,H,&N,&E,&H,"",NULL);
				break;
			case GTP:
				lord_debug(0,LORD("*** gtp ***"));
				if (state->trf_out!=NULL){
					lord_debug(2,LORD("Special gtp!"));
					state->trf_out(state->tcgeo_out,state->lab_out,N,E,0,&N,&E,&HH,tdir);
				}
				else
					ptg(state->lab_out,-1,N,E,&N,&E,"",NULL);
				break;
				
			
			case HTH:
				lord_debug(0,LORD("*** hth ***"));
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
			default:
				lord_error(TR_ERROR,LORD("Programming error!"));
				return TR_ERROR;
			} /*end switch*/
		} /*end action loop*/
	
	lord_critical(TR_ERROR,LORD("Unexpected end of action loop "));
	return TR_ERROR;
}
