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
/*#include <windows.h>* enable if we want to define our dllMain under windows */
#include "conv_lab.h"
#include "gd_trans.h"
#include "itrf_trans.h"
#include "gettabdir.h"
#include "geoid_d.h"
#include "geoid_c.h"
#include "geoid_i.h"
#include "setDtrc.h"
#include "ptg_d.h"
#include "sputshpprj.h"
#include "c_tabdir_file.h"
#include "tab_doc_f.h"
#include "trlib_intern.h"
#include "trlib_api.h"
#include "parse_def_file.h"
#include "doc_def_data.h"
#include "trthread.h"
#include "lord.h"
#define TRLIB_VERSION "dev v1.0 2012-09-21"
#ifndef TRLIB_REVISION
#define TRLIB_REVISION "hg revison not specified"
#endif
#define CRT_SYS_CODE 1 /*really defined in def_lab.txt, so perhaps we should first parse this with a conv_lab call */
#define GEO_SYS_CODE 2
#define TR_TABDIR_ENV "TR_TABDIR" /* some env var, that can be set by the user to point to relevant library. Should perhaps be in trlib_intern.h */
#define TR_DEF_FILE "def_lab.txt"
/* R2D equals the geo_lab define DOUT = (180.0/M_PI) */
#define R2D 57.295779513082323 
#define D2R 0.017453292519943295
/*various global state variables */

static int ALLOW_UNSAFE=0; /*do we allow transformations which are not thread safe?*/
static int UNSAFE[]={FHMASK}; /*list of imit-attrs of transformations deemed unsafe.*/
static int HAS_GEOIDS=0; /*global flag which signals if we have geoids available*/
static THREAD_SAFE int THREAD_ID=0; /*used to distinguish the thread that initialised the library from other threads.*/
static int *MAIN_THREAD_ID=0; /* same as above - also signals whether the library has been succesfully initialised.*/

THREAD_SAFE int TR_LAST_ERROR=TR_OK; /*Last error msg (int) from gd_trans - also set in TR_Open and TR_GeoidTable on allocation errors.*/
FILE *ERR_LOG=0;

def_data *DEF_DATA=0;

/*return last buffered error code from gd_trans */
int TR_GetLastError(void){
    return TR_LAST_ERROR;
}

/* only used in InitLibrary at the moment- but could be useful */
int TR_IsMainThread(void){
	return ((MAIN_THREAD_ID==&THREAD_ID) || (!MAIN_THREAD_ID));
}

/* A function  which checks whether input is in list of coordinate systems deemed thread unsafe */

int TR_IsThreadSafe(union geo_lab *plab){
	int i;
	int n=sizeof(UNSAFE)/sizeof(int);
	if (!plab)
		return 1;
	for (i=0;i<n;i++){
		if (UNSAFE[i]==(plab->u_c_lab).imit)
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
    int ok=-1,rc;
    double x,y,z;
    TR* trf;
    
    if (!TR_IsMainThread()) /*Only one thread can succesfully initialise the library*/
	    return TR_ERROR;
    init_lord();  /* initialise the lord (logging, report and debugging) module with default values */
    
    #ifdef _ROUT
    ERR_LOG=fopen("TR_errlog.log","wt");
    set_lord_debug_mode(ERR_LOG,"all");
    set_lord_info_mode(ERR_LOG,"all");
    set_lord_warning_mode(ERR_LOG,"all");
    set_lord_error_mode(ERR_LOG,"all");
    set_lord_critical_mode(ERR_LOG,"all");
    #endif
    
    ok=TR_SetGeoidDir(path);
    if (ok!=TR_OK){
	    lord_error(TR_ERROR,"Failed to parse def-file!\n");
	    return TR_ERROR;
    }
    #ifdef _ROUT
    if (DEF_DATA && ERR_LOG){
	    fprintf(ERR_LOG,"Contents of %s:\n",TR_DEF_FILE);
	    present_data(ERR_LOG,DEF_DATA);
    }
    #endif
    /* Perform some transformations in order to initialse global statics in transformation functions. TODO: add all relevant transformations below */
    trf=TR_Open("utm32_ed50","utm32_wgs84","");
    if (!trf){
	    lord_error(TR_LABEL_ERROR,"TR_InitLibrary: Failed to open first transformation!\n");
	    return TR_LABEL_ERROR;
    }
    else{
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

    if (ERR_LOG)
	    fprintf(ERR_LOG,"Is init: %d\n************ end initialisation **************\n",(ok==TR_OK));
    /* Set the main thread id */
    if (ok==TR_OK)
	    MAIN_THREAD_ID=&THREAD_ID;
    return (ok==TR_OK)?TR_OK:TR_ERROR; 
}

/*
* A function to change the tab-dir has turned out to be useful - this is done here: set or change the current tab-dir
*/

int TR_SetGeoidDir(char *path){
	FILE *fp;
	int rc;
	char buf[FILENAME_MAX],fname[FILENAME_MAX],*init_path=0;
	if (path && strlen(path)>0) 
		init_path=path;
	else
		init_path=getenv(TR_TABDIR_ENV);
	if (0==init_path){ 
		sprintf(buf,"./");
		init_path=buf;
	}
	else if (init_path[strlen(init_path)-1]!='/' && init_path[strlen(init_path)-1]!='\\'){
		strcpy(buf,init_path);
		strcat(buf,"/");
		init_path=buf;
	}
		    
	strcpy(fname,init_path);
	strcat(fname,TR_DEF_FILE);
	#ifdef _ROUT
	lord_debug(0,"%s %s\n",init_path,fname); /*just debugging */
	#endif
	fp=fopen(fname,"r");
	if (0==fp)
		return TR_ERROR;
	/* close previously parsed definitions */
	if (DEF_DATA) 
		close_def_data(DEF_DATA);
	/* close previously opened tab-dir files */
	c_tabdir_file(0,NULL);
	DEF_DATA=open_def_data(fp,&rc);
	fclose(fp);
	#ifdef _ROUT
	lord_debug(0,"Parsed def_lab.txt, errs: %d\n",rc);
	#endif
	settabdir(init_path);
	return (DEF_DATA ? TR_OK: TR_ERROR);
}
	


/* Mock up - not fully implemented! */
void TR_GeoidInfo(TR *tr) {
    char req[512];
    char geoid_name[256];
    int res;
    strcpy(req,"*");
    TR_GetGeoidName(tr,geoid_name);
    /*strcat(req,tr->geoid_name); */
    strcat(req,geoid_name);
    res=tab_doc_f(req,stdout);
    #ifdef _ROUT
    lord_debug(0,"Request: %s, res: %d\n",req,res);
    #endif
    }

/*Function which copies the last used geoid name to input buffer.*/    

void TR_GetGeoidName(TR *tr,char *name) {
	struct gde_lab *g_lab;
	g_lab=&((tr->geoid_pt->table_u)[tr->geoid_pt->tab_nr]);
	strcpy(name,g_lab->mlb);
	return;
}
	

/*Return the version info */

void TR_GetVersion(char *buffer,int BufSize) {
    char version[256];
    strcpy(version,TRLIB_VERSION);
    strcat(version," rev: ");
    strcat(version,TRLIB_REVISION);
    strncpy(buffer,version,BufSize);
}
	
/* A simple wrapper for tr - deprecated!*/
int TR_TransformSimple(char *label_in, char *label_out, double *X, double *Y, double *Z, int npoints) {
    int err;
    TR *trf=TR_Open(label_in,label_out,"");
    if (trf==0)
	    return TR_LABEL_ERROR; /* No need to close anything! */
    err=TR_Transform(trf,X,Y,Z,npoints);
    TR_Close(trf);
    return err;
}


/* Function which inserts a standard geoid pointer into a TR object */
int TR_GeoidTable(TR *tr){
	static THREAD_SAFE struct mgde_str *GeoidTable=NULL;
	static THREAD_SAFE int init=0;
	static THREAD_SAFE int ngeoids=0;
	/*if called with NULL close evrything down */
	if (0==tr && init){
		geoid_c(GeoidTable,0,NULL);
		if (GeoidTable)
			free(GeoidTable);
		GeoidTable=NULL;
		init=0;
		return TR_OK;
		}
	/*Initialise on first call, makes geoid table thread local */
	if (!init){
		GeoidTable=malloc(sizeof(struct mgde_str));
		if (0==GeoidTable){
			TR_LAST_ERROR=TR_ALLOCATION_ERROR;
			return TR_ALLOCATION_ERROR;
			}
		GeoidTable->init=0;
		ngeoids= geoid_i("STD", GDE_LAB, GeoidTable,NULL);
		HAS_GEOIDS=(ngeoids>0);
		init=1;
		}
	/*insert stuff into the TR object */
	tr->use_geoids=(HAS_GEOIDS)?0:-1;
	strcpy(tr->geoid_name,"STD");
	tr->geoid_pt=GeoidTable;
	tr->close_table=0;
	return TR_OK;
}

int TR_SpecialGeoidTable(TR *tr, char *geoid_name){
	int has_geoids;
	struct mgde_str *special_geoid_table;
	special_geoid_table=malloc(sizeof(struct mgde_str));
	special_geoid_table->init=0;
	has_geoids= geoid_i(geoid_name, GDE_LAB, special_geoid_table,NULL);
	#ifdef _ROUT
	lord_debug(0,LORD("has geoids: %d, name: %s\n"),has_geoids,geoid_name);
	#endif
	if (has_geoids<0){  /*on error return null */
		geoid_c(special_geoid_table,0,NULL);
		free(special_geoid_table);
		lord_error(TR_ALLOCATION_ERROR,LORD("SpecialGeoidTable: Failed to allocate space."));
		return TR_ALLOCATION_ERROR;
	}
	tr->geoid_pt=special_geoid_table;
	tr->close_table=1;
	tr->use_geoids=(has_geoids>0)?0:-1;
	strncpy(tr->geoid_name,geoid_name,FILENAME_MAX);
	return TR_OK;
}

/*Function which returns a PR pointer from a minilabel - allocates room which should be freed afterwards*/
       
PR *TR_OpenProjection(char *mlb){
	int label_check;
	union geo_lab *plab=NULL;
	PR *proj=NULL;
	plab= malloc(sizeof(union geo_lab));
	if (!plab){
		lord_error(TR_ALLOCATION_ERROR,LORD("Failed to allocate space\n"));
		return 0;
	}
	label_check = conv_lab(mlb,plab,"");
	if ((label_check==0) || (label_check==-1)) {
		free(plab);
		lord_error(TR_LABEL_ERROR,"Failed to open projection %s\n",mlb);
		return 0;
	}
	if (!TR_IsThreadSafe(plab) && !ALLOW_UNSAFE){
		lord_error(TR_LABEL_ERROR,"%s not allowed in multithreaded mode!",mlb);
		free(plab);
		return 0;
	}
	proj= malloc(sizeof(PR));
	if (!proj){
		lord_error(TR_ALLOCATION_ERROR,LORD("Failed to allocate space\n"));
		free(plab);
		return 0;
	}
	/*This only does something for TM-projections - the space could be used for something else for other types*/
	proj->plab=plab;
	/*If implementing reference counting: proj->n_references=1;*/
	/* test for allowed transformation - can only fail for non null input*/
       
		
	if (IS_TM(proj))
		setDtrc(&(plab->u_c_lab),proj->dgo);
	return proj;
}

/*Destructor of a PR-object. Decreases reference count and closes if zero.*/

void TR_CloseProjection(PR *proj){
	if (!proj)
		return;
	/* Implement reference counting like this: 
	proj->n_references--
	if (proj->n_references==0){*/
	free(proj->plab);
	free(proj);
}


/*
* Open a TR-object from minilabel (s),
* This should eventually be freed using TR_Close!
* Geoid table info is appended to the TR-object.
*/

TR *TR_Open (char *label_in, char *label_out, char *geoid_name) {
    int err;
    TR *tr=NULL;
    PR *proj_in=NULL, *proj_out=NULL;
    /* initialize the plab_in object member */
    if (label_in && strlen(label_in)>0){
	    proj_in= TR_OpenProjection(label_in);
	    if (!proj_in){
		return 0;
	    }
    }
    /* initialize the plab_out object member */
    if (label_out && strlen(label_out)>0){
	    proj_out=TR_OpenProjection(label_out);
	    if (!proj_out){
		goto TR_OPEN_CLEANUP;
	    }
    }
    
    /* make room for the TR object proper */
    tr = malloc(sizeof(TR));
    if (0==tr){
	lord_error(TR_ALLOCATION_ERROR,LORD("Failed to allocate space."));
	goto TR_OPEN_CLEANUP;
    }
    
    /*Set geoid info */
    if (0!=geoid_name && strlen(geoid_name)>0){
	err=TR_SpecialGeoidTable(tr,geoid_name);
	
	if (err!=TR_OK){
		lord_error(TR_ALLOCATION_ERROR,LORD("Failed open geoid table."));
		goto TR_OPEN_CLEANUP;
	}
    }
    else {  /*insert standard geoids */
	err =TR_GeoidTable(tr);
	if (err!=TR_OK){
		lord_error(TR_ALLOCATION_ERROR,LORD("Failed open geoid table."));
		goto TR_OPEN_CLEANUP;
	}
	
    }
    
    /* transfer everything into the TR object */
    tr->proj_in   = proj_in;
    tr->proj_out  = proj_out;
    return tr;
	    
    /*escape on error, common clean up code.*/
    TR_OPEN_CLEANUP:
	    if (proj_in)
		    TR_CloseProjection(proj_in);
	    if (proj_out)
		    TR_CloseProjection(proj_out);
	    if (tr)
		    free(tr);
	    return 0;
}

int TR_Insert(TR *tr, char *mlb, int is_out){
	PR *proj=NULL, *to_be_replaced=NULL;
	if (!mlb)
		return TR_LABEL_ERROR;
	proj=TR_OpenProjection(mlb);
	if (!proj){
		return TR_LABEL_ERROR;
	}
	to_be_replaced=(is_out)  ?  (tr->proj_out)  : (tr->proj_in);
	if (to_be_replaced)
		TR_CloseProjection(to_be_replaced);
	(is_out) ? (tr->proj_out=proj) : (tr->proj_in=proj);
	return TR_OK;
}


/*Free space associated with a TR-object and its contents */

void TR_Close (TR *tr) {
    if (0==tr)
        return;
    /*release projections*/
    TR_CloseProjection(tr->proj_in);
    TR_CloseProjection(tr->proj_out);
    /*if using special geoid, close it down */
    if (tr->close_table){
	    #ifdef _ROUT
	    lord_debug(0,LORD("Closing special geoid!"));
	    #endif
	    geoid_c(tr->geoid_pt,0,NULL);
	    free(tr->geoid_pt);}
    free (tr);
    return;
}
/*--------------------------------------------------------------------------*/
/* Wrappers of TR_tr */

int TR_Transform(TR *tr, double *X, double *Y, double *Z, int n) {
	int err;
	 if ((0==tr)||(0==X)||(0==Y))
		 return TR_LABEL_ERROR;
	 err=TR_tr(tr->proj_in,tr->proj_out, X,Y,Z,X,Y,Z,n,tr->use_geoids,tr->geoid_pt);
	 return err;
 }
 
 int TR_InverseTransform(TR *tr, double *X, double *Y, double *Z, int n) {
	 int err;
	 if ((0==tr)||(0==X)||(0==Y))
		 return TR_LABEL_ERROR;
	 err=TR_tr(tr->proj_out,tr->proj_in, X,Y,Z,X,Y,Z,n,tr->use_geoids,tr->geoid_pt);
	 return err;
 }

int TR_Transform2(TR *tr, double *X_in, double *Y_in, double *Z_in, double *X_out, double *Y_out, double *Z_out, int n) {
	 int err;
	 if ((0==tr)||(0==X_in)||(0==Y_in)||(0==X_out)||(0==Y_out))
		 return TR_LABEL_ERROR;
	 err=TR_tr(tr->proj_in,tr->proj_out, X_in,Y_in,Z_in,X_out,Y_out,Z_out,n,tr->use_geoids,tr->geoid_pt);
	 return err;
 }
 
 int TR_InverseTransform2(TR *tr, double *X_in, double *Y_in, double *Z_in, double *X_out, double *Y_out, double *Z_out, int n) {
	 int err;
	 if ((0==tr)||(0==X_in)||(0==Y_in)||(0==X_out)||(0==Y_out))
		 return TR_LABEL_ERROR;
	 err=TR_tr(tr->proj_out,tr->proj_in, X_in,Y_in,Z_in,X_out,Y_out,Z_out,n,tr->use_geoids,tr->geoid_pt);
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
 
/* transform one or more xyz-tables */
int TR_tr(
   PR *proj_in,
   PR *proj_out, 
   double *X_in, 
   double *Y_in, 
   double *Z_in,
   double *X_out,
   double *Y_out,
   double *Z_out,
   int n, 
   int use_geoids, 
   struct mgde_str *geoid_pt
   ) 
 {
	 
    union geo_lab *plab_in,*plab_out;
    double *x_in, *y_in,*x_out,*y_out,z, GH = 0, z1 = 0, z2 = 0;
    int err, ERR = 0, i; /*use_geoids; */
	 
    /*escape if the underlying TR-object is not fully composed....*/
    if (!proj_in || !proj_out){
	    lord_error(TR_LABEL_ERROR,"TR_tr: TR-object not fully composed.\n");
	    return TR_LABEL_ERROR;
    }
    plab_in=proj_in->plab;
    plab_out=proj_out->plab;
    if (IS_CARTESIC(proj_in)){
        x_in=Y_in;
        y_in=X_in;}
    else{
        x_in=X_in;
        y_in=Y_in;}
    if (IS_CARTESIC(proj_out)){
        x_out=Y_out;
        y_out=X_out;}
   else{
        x_out=X_out;
        y_out=Y_out;}
  
    for (i = 0;  i < n;  i++) {
	z = Z_in? Z_in[i]: z1;
        err = gd_trans(plab_in, plab_out,  y_in[i], x_in[i], z,  y_out+i, x_out+i, (Z_out? Z_out+i: &z2), &GH,use_geoids,geoid_pt, "", ERR_LOG);
	#ifdef _ROUT
	if (err)
		lord_debug(0,"\nProj: %s->%s, last err: %d, out: %.5f %.5f %.3f\n",(plab_in->u_c_lab).mlb,(plab_out->u_c_lab).mlb,err,x_in[i],y_in[i],z);
	#endif
        /*err = gd_trans(tr->plab_in, tr->plab_out,  x,y,z,  X+i,Y+i, (Z? Z+i: &z2), &GH, -1, &GeoidTable, 0, 0);
         *   KE siger at arg 0 for GeoidTable er bedre end -1. Ved -1 er det "forbudt" at bruge geoidetabeller */
        if (err)
           ERR = err;
   }
   TR_LAST_ERROR=ERR;
   
   return ERR? TR_ERROR: TR_OK;
    
}

/* transform ITRF one or more xyz-tables */
int TR_itrf(
   union geo_lab *plab_in,      /* In label */
   union geo_lab *plab_out,     /* Out label */
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
        err = itrf_trans(plab_in, plab_out, stn_vel, i_crd, i_vel, i_JD, o_crd, o_vel, plate_info, tr_par, "", err_str);
        x_out[i]=o_crd[0]; y_out[i]=o_crd[1]; z_out[i]=o_crd[2];
        if(n_vel==n){
            velx_out[i]=o_vel[0]; vely_out[i]=o_vel[1]; velz_out[i]=o_vel[2];
        }
    #ifdef _ROUT
    if (err)
        fprintf(ERR_LOG,"\nProj: %s->%s, last err: %d, error str: %s, out: %.3f %.3f %.3f\n",(plab_in->u_c_lab).mlb,(plab_out->u_c_lab).mlb,err,err_str,x_in[i],y_in[i],z_in[i]);
    #endif
        if (err)
           ERR = err;
   }
   TR_LAST_ERROR=ERR;

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
	TR_TerminateThread(); /*terminate the 'main' thread - assuming this haven't been done already (perhaps check for this??) */
	if (DEF_DATA)
		close_def_data(DEF_DATA);
     
}

void TR_TerminateThread(void){
	gd_trans(NULL,NULL,0,0,0,NULL,NULL,NULL,NULL,0,NULL,"",0);
	TR_GeoidTable(NULL);
	c_tabdir_file(0,NULL);
}

/*
BOOL WINAPI
DllMain (HANDLE hDll, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
        case DLL_PROCESS_ATTACH:
            // Code to run when the DLL is loaded
            break;

        case DLL_PROCESS_DETACH:
            // Code to run when the DLL is freed
            break;

        case DLL_THREAD_ATTACH:
            // Code to run when a thread is created during the DLL's lifetime
            break;

        case DLL_THREAD_DETACH:
		printf("Thread detaching!\n");
	        TerminateLibrary();
            // Code to run when a thread ends normally.
            break;
    }
    return TRUE;
}
*/






/* Translates mlb to ESRI wkt*/
int TR_GetEsriText(char *mlb, char *wkt_out){
    int err;
    PR *TC=TR_OpenProjection(mlb);
    if (!TC)
	return TR_LABEL_ERROR;
    err=sputshpprj(wkt_out,TC->plab); /* See fputshpprj.h for details on return value */
    free(TC);
    return err;
}
    
/* out must be an array of length 2*/
int TR_GetLocalGeometry(TR *trf, double x, double y, double *s, double *mc){
	double xo,yo,out[2];
	int ret;
	/*Use the input lab of the TR-object*/
	PR *proj=NULL;
	if (trf->proj_in)
		proj=trf->proj_in;
	else if (trf->proj_out)
		proj=trf->proj_out;
	else
		return TR_LABEL_ERROR;
	if (IS_CARTESIC(proj)){
		return TR_ERROR;
	}
	if (IS_GEOGRAPHIC(proj)){
		*s=1;
		*mc=0;
		return TR_OK;
	}
	/*
	#ifdef _ROUT
	lord_debug(0,LORD("TR_GetLocalGeometry: init: %d, %.2f %.2f\n"),(plab->u_c_lab).init,x,y);
	#endif
	*/
	ret=ptg_d(proj->plab,1,y,x,&yo,&xo,"",ERR_LOG,out,proj->dgo);
	*s=out[0];
	*mc=out[1];
	return (ret==0)?TR_OK:TR_ERROR;
}
        
