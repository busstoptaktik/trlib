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
 
#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>
/*#include <windows.h>* enable if we want to define our dllMain under windows */
//#include    "trlib_intern.h"
#include    "KmsFncs.h"
#include    "trthread.h"
#include    "epsg_to_mlb.h"
#include    "proj4_to_mlb.h"
#include    "sgetshpprj.h"
#include    "grim.h"

/*   
* Defines
*/
#ifndef  TRLIB_REVISION
#define TRLIB_REV_STRING "not specified"
#else
#define TRLIB_REV_STRING TOSTRING(TRLIB_REVISION)
#endif

#define TRLIB_VERSION ( "dev v1.1 (rev: " TRLIB_REV_STRING  ", " __DATE__ ", " __TIME__ ")" )

#define TR_TABDIR_ENV  "TR_TABDIR" /* some env var, that can be set by the user to point to relevant library. Should perhaps be in trlib_intern.h */
#define TR_DEF_FILE      "def_trlib.xml"
#define TR_EPSG_FILE    "def_epsg.txt"


/* R2D equals the geo_lab define DOUT = (180.0/M_PI) */

#define R2D 57.295779513082323 
#define D2R 0.017453292519943295

/*various global state variables */

static int ALLOW_UNSAFE=0; /*do we allow transformations which are not thread safe?*/
static int UNSAFE[]={-1}; /*list of imit-attrs of transformations deemed unsafe.*/
static int N_UNSAFE=0;
static int HAS_GEOIDS=0; /*global flag which signals if we have geoids available*/
static THREAD_SAFE int THREAD_ID=0; /*used to distinguish the thread that initialised the library from other threads.*/
static int *MAIN_THREAD_ID=0; /* same as above - also signals whether the library has been succesfully initialised.*/


FILE *ERR_LOG=0;


tab_dir    *TAB_DIR=0;
def_data *DEF_DATA=0;  /*really a member of TAB_DIR - but useful for error checking to include pointer here...*/

/* only used in InitLibrary at the moment- but could be useful */
int TR_IsMainThread(void){
	return ((MAIN_THREAD_ID==&THREAD_ID) || (!MAIN_THREAD_ID));
}

/* A function  which checks whether input is in list of coordinate systems deemed thread unsafe */

int TR_IsThreadSafe(PR *plab){
	int i;
	if (!plab)
		return 1;
	for (i=0;i<N_UNSAFE;i++){
		if (UNSAFE[i]==(plab->imit))
			return 0;
	}
	return 1;
}

/* set state to allow unsafe transformations */
void TR_AllowUnsafeTransformations(void){
	ALLOW_UNSAFE=1;
}

/*set state to forbid unsafe transformations */
void TR_ForbidUnsafeTransformations(void){
	ALLOW_UNSAFE=0;
}

/* Need to let KMSTrans parse the def-files, before any transformation */
int TR_InitLibrary(char *path) {
    int ok=-1,rc,unsafe_allowed=ALLOW_UNSAFE;
    double x,y,z;
    TR* trf;
    if (!TR_IsMainThread()) /*Only one thread can succesfully initialise the library*/
	    return TR_ERROR;
    /*only initialise if not already done*/
    if (!is_lord_initialised())
	    init_lord();  /* initialise the lord (logging, report and debugging) module with default values */
    
    #ifdef _ROUT
    ERR_LOG=fopen("TR_errlog.log","wt");
    set_lord_outputs(ERR_LOG,ERR_LOG,ERR_LOG,ERR_LOG,ERR_LOG);
    set_lord_modes(1,1,1,1,1);
    set_lord_verbosity_levels(3,3,3,3,3);
    #endif
    ok=TR_SetGeoidDir(path);
    if (ok!=TR_OK){
	    lord_error(TR_ERROR,"Failed to parse def-file!");
	    return TR_ERROR;
    }
    #ifdef _ROUT
    if (DEF_DATA && ERR_LOG){
	    fprintf(ERR_LOG,"\nContents of %s:\n",TR_DEF_FILE);
	    present_data(ERR_LOG,DEF_DATA);
	    fflush(ERR_LOG);
    }
    #endif
    /* Perform some transformations in order to initialse global statics in transformation functions. TODO: add all relevant transformations below 
     * Allow unsafe projections ONLY in initialisation!
    */
    ALLOW_UNSAFE=1;
    lord_debug(0,LORD("Init of data successfull! - trying to transform, def_lab not null: %d"),(TAB_DIR->def_lab!=NULL));
    trf=TR_Open("utm32_ed50","utm32_wgs84","");
    if (!trf){
	    lord_error(TR_LABEL_ERROR,"TR_InitLibrary: Failed to open first transformation!");
	    return TR_LABEL_ERROR;
    }
    else{
	    #ifdef _ROUT
	    lord_debug(0,"Successfully opened first transformation.");
	    fflush(ERR_LOG);
	    #endif
	    ok=TR_TransformPoint(trf,512200.1,6143200.1,0,&x,&y,&z);
	    TR_Close(trf);
    }
    trf=TR_Open("utm32_wgs84","fotm","");
    
    if (0!=trf){
	    rc=TR_TransformPoint(trf,652142,6058737,0, &x, &y,&z);
	    TR_Close(trf);
    }
    
    trf=TR_Open("utm32_wgs84","s34j","");
     if (0!=trf){
	    rc=TR_TransformPoint(trf,512200.1,6143200.1,0,&x,&y,&z);
	    TR_Close(trf);
    }
    
    trf=TR_Open("FO_fotm","GR_utm22_gr96","");
    if (0!=trf){
	    rc=TR_TransformPoint(trf,200000.0,876910.0,0,&x,&y,&z);
	    TR_Close(trf);
    }
    
    trf=TR_Open("utm32_wgs84","fcsH_h_fcsvr10","");
    if (0!=trf){
	    rc=TR_TransformPoint(trf,652142.0,6058737.0,0, &x, &y,&z);
	    TR_Close(trf);
    }
    lord_debug(0,"Is initialised: %d",(ok==TR_OK));
    if (ERR_LOG){
	    fprintf(ERR_LOG,"\n*************** end initialisation *****************\n");
	    fflush(ERR_LOG);
    }
    /*set the ALLOW_UNSAFE flag to what it was*/
    ALLOW_UNSAFE=unsafe_allowed;
    /* Set the main thread id */
    if (ok==TR_OK)
	    MAIN_THREAD_ID=&THREAD_ID;
    return (ok==TR_OK)?TR_OK:TR_ERROR; 
}

/*
* A function to change the tab-dir has turned out to be useful - this is done here: set or change the current tab-dir
* UNSAFE TO CALL IF THERE ARE ANY OPENED TR OBJECTS! These will have references to data which is freed!
* We might consider having a geoid table pr. TR object to avoid this!
*/

int TR_SetGeoidDir(char *path){
	FILE *fp;
	char buf[TR_MAX_FILENAME],fname[TR_MAX_FILENAME],*init_path=NULL;
	if (path && *path) 
		init_path=path;
	else
		init_path=getenv(TR_TABDIR_ENV);
	if (NULL==init_path){ 
		sprintf(buf,"./");
		init_path=buf;
	}
	else if (init_path[strlen(init_path)-1]!='/' && init_path[strlen(init_path)-1]!='\\'){
		strcpy(buf,init_path);
		strcat(buf,"/");
		init_path=buf;
	}
	
	/* close previously parsed definitions */
	tab_dir_close(TAB_DIR);
	
	/* close previously opened tab-dir files */
	/*Should soon be unneeded!!!*/
	/*init epsg table*/
	strcpy(fname,init_path);
	strcat(fname,TR_EPSG_FILE);
	fp=fopen(fname,"r");
	if (0==fp)
		lord_warning(0,"Failed to open epsg_def_file: %s",fname);
	else{
		int n=setup_epsg_table(fp);
		lord_debug(0,"Parsed epsg_def_file with %d entries.",n);
		fclose(fp);
	}
	/*init tab_dir var*/
	strcpy(fname,init_path);
	strcat(fname,TR_DEF_FILE);
	#ifdef _ROUT
	lord_debug(0,"tabdir: %s  def_lab: %s",init_path,fname); /*just debugging */
	#endif
	DEF_DATA=NULL;
	TAB_DIR=tab_dir_open(fname);
	if (TAB_DIR)
		DEF_DATA=TAB_DIR->def_lab;  /*just a useful pointer which could be NULL */
	return (TAB_DIR && DEF_DATA)? TR_OK: TR_ERROR;
}
	
/* Mock up - not fully implemented! */
/*void TR_GeoidInfo(TR *tr) {
    char req[512];
    char geoid_name[256];
    int res;
    strcpy(req,"*");
    TR_GetGeoidName(tr,geoid_name);
    /*strcat(req,tr->geoid_name); * /
    strcat(req,geoid_name);
    res=tab_doc_f(req,stdout);
    #ifdef _ROUT
    lord_debug(0,"Request: %s, res: %d\n",req,res);
    #endif
    }
*/

/*Function which copies the last used geoid name to input buffer.*/    

void TR_GetGeoidName(TR *tr,char *name) {
	/*strcpy(name,grim_filename(tr->state->grid_tab->table_sequence[tr->state->grid_tab->cur_table]));*/
	strcpy(name,"TODO");
	return;
}
	

/*Return the version info */
void TR_GetVersion(char *buffer,int BufSize) {
	strncpy(buffer,TRLIB_VERSION,BufSize);
}
	


/*Function which returns a PR pointer from a minilabel - allocates room which should be freed afterwards*/
PR *TR_OpenProjection(char *mlb){
	int label_check;
	struct coord_lab *plab=NULL;
	PR *proj;
	if (!TAB_DIR || !DEF_DATA){
		lord_error(TR_ALLOCATION_ERROR,LORD("Library not initialised!"));
		return NULL;
	}
	proj= malloc(sizeof(PR));
	if (!proj){
		lord_error(TR_ALLOCATION_ERROR,LORD("Failed to allocate space."));
		return 0;
	}
	label_check = conv_w_crd(mlb,proj,DEF_DATA);
	if (label_check!=CRD_LAB) {
		free(proj);
		lord_error(TR_LABEL_ERROR,"Failed to open projection %s.",mlb);
		return 0;
	}
	lord_debug(0,LORD("So far so good!"));
	if (!TR_IsThreadSafe(proj) && !ALLOW_UNSAFE){
		lord_error(TR_LABEL_ERROR,"%s not allowed in multithreaded mode!",mlb);
		free(proj);
		return 0;
	}
	
	if (IS_TM(proj))
		setDtrc(proj,proj->dgo);
	return proj;
}

/*Destructor of a PR-object. Decreases reference count and closes if zero.*/

void TR_CloseProjection(PR *proj){
	if (!proj)
		return;
	free(proj);
}


/*
* Open a TR-object from minilabel (s),
* This should eventually be freed using TR_Close!
* Geoid table info is appended to the TR-object.
*/

TR *TR_Open (char *label_in, char *label_out, char *geoid_name) {
    TR *tr=NULL;
    PR *proj_in=NULL, *proj_out=NULL;
    gd_state *T = NULL, *I=NULL;

    /* initialize the plab_in object member */
    if (label_in && strlen(label_in)>0){
	    proj_in= TR_OpenProjection(label_in);
	    if (!proj_in)
		return 0;
    }

    /* initialize the plab_out object member */
    if (label_out && strlen(label_out)>0){
	    proj_out=TR_OpenProjection(label_out);
	    if (!proj_out)
		goto CLEANUP;
    }
    
    if(proj_in && proj_out){
	    T = gd_open(proj_in, proj_out, geoid_name, TAB_DIR);
	    if(!T)
                goto CLEANUP;
	    I= gd_open(proj_out,proj_in, geoid_name, TAB_DIR);
	    if (!I)
		goto CLEANUP;
	    
    }
    
    /* make room for the TR object proper */
    tr = malloc(sizeof(TR));
    if (0==tr){
	lord_error(TR_ALLOCATION_ERROR,LORD("Failed to allocate space."));
	goto CLEANUP;
    }
    
    /* transfer everything into the TR object */
    tr->proj_in   = proj_in;
    tr->proj_out  = proj_out;
    tr->state = T;
    tr->etats=  I;
    return tr;
	    
    /*escape on error, common clean up code.*/
    CLEANUP:
	    TR_CloseProjection(proj_in);
	    TR_CloseProjection(proj_out);
	    gd_close(T);
            gd_close(I);
	    free(tr);
	    return 0;
}

/* Insert 'projection' into TR-object. Allows reuse... */
int TR_Insert(TR *tr, char *mlb, int is_out){
	PR *proj=NULL, *to_be_replaced=NULL;
	/*PR *i_lab, *o_lab;*/
	char geoid_name[MLBLNG];
	/*strncpy(geoid_name,tr->state->geoid_name,MLBLNG);*/
	gd_close(tr->state);
	gd_close(tr->etats);
	/*only open if given a proper string - otherwise the slot is just NULL'ed which can be useful also*/
	if (mlb && *mlb){
		proj=TR_OpenProjection(mlb);
		if (!proj)
			return TR_LABEL_ERROR;
	}
	to_be_replaced=(is_out)  ?  (tr->proj_out)  : (tr->proj_in);
	TR_CloseProjection(to_be_replaced);
	(is_out) ? (tr->proj_out=proj) : (tr->proj_in=proj);
	if (tr->proj_in && tr->proj_out){
		tr->state=gd_open(tr->proj_in,tr->proj_out,geoid_name, TAB_DIR);
		if (!tr->state)
			return TR_LABEL_ERROR;
		tr->etats=gd_open(tr->proj_out,tr->proj_in,geoid_name, TAB_DIR);
		if (!tr->etats)
			return TR_LABEL_ERROR;
	}
	return TR_OK;
}


/*Free space associated with a TR-object and its contents */
void TR_Close (TR *tr) {
    if (0==tr)
        return;
    /*release projections*/
    TR_CloseProjection(tr->proj_in);
    TR_CloseProjection(tr->proj_out);
    gd_close(tr->state);
    gd_close(tr->etats);
    free (tr);
    return;
}
/*--------------------------------------------------------------------------*/
/* Wrappers of TR_tr */

int TR_Transform(TR *tr, double *X, double *Y, double *Z, int n) {
	int err;
	 if ((0==tr)||(0==X)||(0==Y))
		 return TR_LABEL_ERROR;
	 err=TR_tr(tr->state, X,Y,Z,X,Y,Z,NULL,n);
	 return err;
 }
 
 int TR_InverseTransform(TR *tr, double *X, double *Y, double *Z, int n) {
	 int err;
	 if ((0==tr)||(0==X)||(0==Y))
		 return TR_LABEL_ERROR;
	 err=TR_tr(tr->etats, X,Y,Z,X,Y,Z,NULL,n);
	 return err;
 }

int TR_Transform2(TR *tr, double *X_in, double *Y_in, double *Z_in, double *X_out, double *Y_out, double *Z_out, int n) {
	 int err;
	 if ((0==tr)||(0==X_in)||(0==Y_in)||(0==X_out)||(0==Y_out))
		 return TR_LABEL_ERROR;
	 err=TR_tr(tr->state, X_in,Y_in,Z_in,X_out,Y_out,Z_out,NULL,n);
	 return err;
 }
 
 int TR_InverseTransform2(TR *tr, double *X_in, double *Y_in, double *Z_in, double *X_out, double *Y_out, double *Z_out, int n) {
	 int err;
	 if ((0==tr)||(0==X_in)||(0==Y_in)||(0==X_out)||(0==Y_out))
		 return TR_LABEL_ERROR;
	 err=TR_tr(tr->etats, X_in,Y_in,Z_in,X_out,Y_out,Z_out,NULL,n);
	 return err;
 }

 int TR_TransformPoint(TR *tr, double X_in, double Y_in, double Z_in, double *X_out, double *Y_out, double *Z_out) {
	 int err;
	 err=TR_Transform2(tr, &X_in, &Y_in, &Z_in, X_out, Y_out, Z_out,1);
	 return err;
 }	 
 
 int TR_InverseTransformPoint(TR *tr, double X_in, double Y_in, double Z_in, double *X_out, double *Y_out, double *Z_out) {
	 int err;
	 err=TR_InverseTransform2(tr, &X_in, &Y_in, &Z_in, X_out, Y_out, Z_out,1);
	 return err;
 }
 
 int TR_TransformGH(TR *tr, double *X_in, double *Y_in, double *Z_in, double *GH_out, int n){
	int err;
        err=TR_tr(tr->state,X_in,Y_in,Z_in,X_in,Y_in,Z_in,GH_out,n);
	return err;
}
 
 
/* transform one or more xyz-tables */
int TR_tr(
   gd_state *T,
   double *X_in, 
   double *Y_in, 
   double *Z_in,
   double *X_out,
   double *Y_out,
   double *Z_out,
   double *GH_out,
   int n
   ) 
 {
    PR *proj_in;
    PR *proj_out; 
   
   
    double *x_in, *y_in,*x_out,*y_out,z, GH = 0, z1 = 0, z2 = 0;
    int err, ERR = 0, i; /*use_geoids; */

    /*escape if the underlying TR-object is not fully composed....*/
    if (!T){
	    lord_error(TR_LABEL_ERROR,"TR_tr: TR-object not fully composed.\n");
	    return TR_LABEL_ERROR;
    }
    
    proj_in = T->lab_in;
    proj_out = T->lab_out;
    
    if (IS_CARTESIC(proj_in)){
        x_in=Y_in;
        y_in=X_in;
    }
    else{
        x_in=X_in;
        y_in=Y_in;
    }
    if (IS_CARTESIC(proj_out)){
        x_out=Y_out;
        y_out=X_out;
   }
   else{
        x_out=X_out;
        y_out=Y_out;
   }
  
   for (i = 0;  i < n;  i++) {
      	z = Z_in? Z_in[i]: z1;
        err = gd_trans(T, y_in[i], x_in[i], z,  y_out+i, x_out+i, (Z_out? Z_out+i: &z2), (GH_out?GH_out+i: &GH));
	#ifdef _ROUT
	if (err)
		lord_debug(0,"\nProj: %s->%s, last err: %d, out: %.5f %.5f %.3f\n",GET_MLB(proj_in),GET_MLB(proj_out),err,x_in[i],y_in[i],z);
	#endif
        if (err)
           ERR = err;
   }
   if (ERR)
	   lord_error(ERR,"Error in transformation.");
   return ERR? TR_ERROR: TR_OK;
    
}






/* transform ITRF one or more xyz-tables */
int TR_itrf(
   struct coord_lab *plab_in,      /* In label */
   struct coord_lab *plab_out,     /* Out label */
   double *x_in,                /* X in array */
   double *y_in,                /* Y in array */
   double *z_in,                /* Z in array */
   double *x_out,               /* X out array */
   double *y_out,               /* Y out array */
   double *z_out,               /* Z out array */
   int n,                       /* XYZ in/out array size */
   double *velx_in,             /* Velocity X in array */
   double *vely_in,             /* Velocity Y in array */
   double *velz_in,             /* Velocity Z in array */
   double *velx_out,            /* Velocity X out array */
   double *vely_out,            /* Velocity Y out array */
   double *velz_out,            /* Velocity Z out array */
   int n_vel,                   /* Velocity XYZ in/out size (0->Use std plate velocity) */
   double *JD_in,               /* JD2000 epoch in array */
   int n_JD,                    /* JD2000 epoch in/size (1->Use same epoch for all coordinates) */
   struct PLATE *plate_info
   )
 {

    double i_crd[3], i_vel[3], i_JD=-36525.0;
    double o_crd[3], o_vel[3], tr_par[7];
    char *err_str=NULL;
    int err, ERR = 0, i, stn_vel=0;

    if(n_vel>0) stn_vel=1;
    i_JD=JD_in[0];

    for (i = 0;  i < n;  i++) {
        if(n_JD==n) i_JD=JD_in[i];
        i_crd[0]=x_in[i]; i_crd[1]=y_in[i]; i_crd[2]=z_in[i];
        if(n_vel==n){
            i_vel[0]=velx_in[i]; i_vel[1]=vely_in[i]; i_vel[2]=velz_in[i];
        }
        /*err = itrf_trans(plab_in, plab_out, stn_vel, i_crd, i_vel, i_JD, o_crd, o_vel, plate_info, tr_par, "", err_str);*/
	/*TODO: rewrite itrf_trans as for gd_trans*/
	err=TRF_ILLEG_;
        x_out[i]=o_crd[0]; y_out[i]=o_crd[1]; z_out[i]=o_crd[2];
        if(n_vel==n){
            velx_out[i]=o_vel[0]; vely_out[i]=o_vel[1]; velz_out[i]=o_vel[2];
        }
   
        if (err)
           ERR = err;
   }
   if (ERR)
	   lord_error(ERR,LORD("Error in itrf transformation."));

   return ERR? TR_ERROR: TR_OK;

}

/* read xyz-tuples from f_in; stream transformed tuples to f_out */
int TR_Stream(TR *trf, FILE *f_in, FILE *f_out, int n) {
    double r2d=R2D;
    double d2r=D2R;
    int ERR = 0,i=0,is_geo_in,is_geo_out;
    enum {BUFSIZE = 16384};
    char buf[BUFSIZE];
    if ((0==trf) || (0==f_in) || (0==f_out))
        return TR_ERROR;
    is_geo_in=(IS_GEOGRAPHIC(trf->proj_in));
    is_geo_out=(IS_GEOGRAPHIC(trf->proj_out));
    while (0 != fgets(buf, BUFSIZE, f_in)) {
        int    argc, err;
        double x,y,z;
        
        argc = sscanf(buf, "%lf %lf %lf", &x, &y, &z);
       /* 2D transformation? */
        if (argc==2)
            z= 0;
            
        /* Uninterpretable line */
        if ((argc==1) || (argc==0)) {
            /* n==-1: break on garbage */
            if (n==-1)
                break;
            /* n==0: ignore garbage - break on end-of-file */
            if (n==0)
                continue;
        }
        if (is_geo_in){
		x*=d2r;
		y*=d2r;
	}
        err = TR_Transform(trf,&x,&y,&z,1);
        i++;

        /* buffer last error */
        if (err)
            ERR = err;

        if ((n==-1) && err)
            break;
        if (is_geo_out){
		x*=r2d;
		y*=r2d;
	}
        fprintf(f_out, "%.10g %.10g %.10g\n", x,y,z);

        if (i==n)
            break;
    }

    return ERR? TR_ERROR: TR_OK;
}

/* We need to close file pointers! Other ressources *should* be freed automatically! */
void TR_TerminateLibrary(void) {
	tab_dir_close(TAB_DIR);
	release_epsg_table();
}

/*not needed*/
void TR_TerminateThread(void){
	return;
}

/* Translates mlb to ESRI wkt*/
int TR_GetEsriText(char *mlb, char *wkt_out){
    int err;
    PR *TC=TR_OpenProjection(mlb);
    if (!TC)
	return TR_LABEL_ERROR;
    err=sputshpprj(wkt_out,TC,DEF_DATA); /* See fputshpprj.h for details on return value */
    TR_CloseProjection(TC);
    return (err==0)?(TR_OK):(TR_LABEL_ERROR);
}
    
/* out must be an array of length 2*/
int TR_GetLocalGeometry(TR *trf, double x, double y, double *s, double *mc, int is_proj_out){
	double xo,yo,out[2];
	int ret;
	/*Use the input lab of the TR-object*/
	PR *proj=NULL;
	/*if is_proj_out is <=0 default to taking proj_in */
	if (!is_proj_out)
		proj=trf->proj_in;
	else
		proj=trf->proj_out;
	if (proj==NULL){
		lord_error(TR_LABEL_ERROR,"GetLocalGeometry: specified projection not set!");
		return TR_LABEL_ERROR;
	}
	if (IS_CARTESIC(proj)){
		lord_warning(0,"Local scale and convergence not defined for cartesic systems.");
		return TR_ERROR;
	}
	if (IS_GEOGRAPHIC(proj)){
		*s=1;
		*mc=0;
		return TR_OK;
	}
	ret=ptg_d(proj,1,y,x,&yo,&xo,"",ERR_LOG,out,proj->dgo);
	*s=out[0];
	*mc=out[1];
	return (ret==0)?TR_OK:TR_ERROR;
}

/* Import a mlb from other projection specifications */
int TR_ImportLabel(char *text, char *mlb_out, int buf_len){
	int ok=TR_LABEL_ERROR;
	char buf[2*MLBLNG]; /*TODO: improve translation functions so that buf_len is included in call.
	/*We go for autodetection of format - could also implement explicit identifiers like in EPSG:xxxxx*/
	/*case proj4*/
	if (strstr(text,"+proj"))
		ok=proj4_to_mlb(text,buf,DEF_DATA);
	/*case epsg*/
	else if (strstr(text,"EPSG")){
		char *pos=strchr(text,':');
		long code,v_code=0;
		if (pos){
			char *end_pt; /*test if the format is like EPSG:h_code:v_code*/
			code=strtol(pos+1,&end_pt,0);
			if (code>0){
				if (*end_pt) /*so we have height info also*/
					v_code=strtol(end_pt+1,NULL,0);
				ok=import_from_epsg(code,v_code,buf);
			}
		}
		else
			lord_warning(1,"EPSG: invalid format of EPSG code specification.");  
	}
	/*case wkt*/
	else if (strchr(text,'[') && strchr(text,']')){
		ok=sgetshpprj(text,NULL,buf,DEF_DATA);
		ok=(ok==0)?(TR_OK):(TR_LABEL_ERROR);
	}
	else
		lord_warning(TR_LABEL_ERROR,"Unrecognisable input format.");
	if (ok!=TR_OK)
		*mlb_out='\0';
	else /*TODO: report error on overflow*/
		strncpy(mlb_out,buf,buf_len);
	return ok;
}

/*Export a mlb to a foreign format */
int TR_ExportLabel(char *mlb, char *out, int foreign_format_code, int buf_len){
	int ok,h_code,v_code;
	char buf[4096];
	/*TODO: test for height in mlb and return a warning/special return code if only partially translated
	char mlb1[2*MLBLNG], mlb2[2*MLBLNG], *h_datum;
	short sepch;
	ok=get_mlb(mlb,mlb1,&sepch,mlb2,&h_datum);
	*/
	*out='\0';
	switch(foreign_format_code){
		case TR_FRMT_EPSG:
			ok=export_to_epsg(mlb, &h_code, &v_code);
			if (ok==TR_OK){
				char *tmp=buf;
				tmp+=sprintf(tmp,"EPSG:%d",h_code);
				if (v_code>100)
					sprintf(tmp,":%d",v_code);
			}
			break;
		case TR_FRMT_PROJ4: /*TODO*/
			ok=mlb_to_proj4(mlb,buf,4096,DEF_DATA);
			break;
		case TR_FRMT_ESRI_WKT:
			ok=TR_GetEsriText(mlb,buf);
			break;
		default:
			lord_error(TR_LABEL_ERROR,LORD("Translation to foreign format with code %d not supported."),foreign_format_code);
			return TR_ERROR;
	}
	if (ok==TR_OK){
		if ( ((int) strlen(buf))>(buf_len-1)){
			lord_error(TR_ALLOCATION_ERROR,LORD("To short input buffer to hold external srs-definition."));
			return TR_ALLOCATION_ERROR;
		}
		strcpy(out,buf);
		return TR_OK;
	}
	return TR_ERROR;
}

/********************************************************
********* LORD wrapper functions ************************
********************************************************/

/*return last buffered error code from the lord module */
int TR_GetLastError(void){
    return lord_get_last_error();
}

/* Sets the max messages pr. thread in the lord module. Negative number means no limit */
void TR_SetLordMaxMessages (int max_messages) {
	set_lord_max_messages(max_messages);
}

/* Sets the lord call back function */
void TR_SetLordCallBack(LORD_CALLBACK fct) {
	set_lord_callback(fct);
}

/* Turn the lord modes on or off */
void TR_SetLordModes(int debug, int info, int warning, int error, int critical) {
	set_lord_modes(debug, info, warning, error, critical);
}

/* Set the output pointers for each lord_type */
void TR_SetLordOutputs(FILE * stream_debug_outside, FILE * stream_info_outside, FILE * stream_warning_outside, FILE * stream_error_outside, FILE * stream_critical_outside) {
	set_lord_outputs(stream_debug_outside, stream_info_outside, stream_warning_outside, stream_error_outside, stream_critical_outside);
}

/* Set the verbosity level for the modes */
void TR_SetLordVerbosityLevels(int verb_debug, int verb_info, int verb_warning) {
	set_lord_verbosity_levels(verb_debug, verb_info, verb_warning, 3, 3);
}

/* Sets the lord output file for all modes */
int TR_SetLordFile(char *fullfilename) {
	return set_lord_file(fullfilename);
}
