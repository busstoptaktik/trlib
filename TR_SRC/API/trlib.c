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
#include "gettabdir.h"
#include "geoid_d.h"
#include "geoid_c.h"
#include "geoid_i.h"
#include "ptg_d.h"
#include "sputshpprj.h"
#include "c_tabdir_file.h"
#include "tab_doc_f.h"
#include "trlib_intern.h"
#include "trlib_api.h"
#include "trthread.h"
#define TRLIB_VERSION "dev v1.0 2012-03-23"
#ifndef TRLIB_REVISION
#define TRLIB_REVISION "hg revison not specified"
#endif
#define CRT_SYS_CODE 1 /*really defined in def_lab.txt, so perhaps we should first parse this with a conv_lab call */
#define GEO_SYS_CODE 2
#define TR_TABDIR_ENV "TR_TABDIR" /* some env var, that can be set by the user to point to relevant library. Should perhaps be in trlib_intern.h */
#define TR_DEF_FILE "def_lab.txt"
#define R2D 57.295779513082323 
#define D2R 0.017453292519943295
/*various global state variables */

static int ALLOW_UNSAFE=0; //do we allow transformations which are not thread safe?
static int UNSAFE[]={FHMASK}; //list of imit-attrs of transformations deemed unsafe.
static int HAS_GEOIDS=0; //global flag which signals if we have geoids available 
static THREAD_SAFE int THREAD_ID=0; //used to distinguish the thread that initialised the library from other threads.
static int *MAIN_THREAD_ID=0; // same as above - also signals whether the library has been succesfully initialised.

THREAD_SAFE int TR_LAST_ERROR=TR_OK; /*Last error msg (int) from gd_trans - also set in TR_Open and TR_GeoidTable on allocation errors.*/
FILE *ERR_LOG=0;

/*return last buffered error code from gd_trans */
int TR_GetLastError(void){
    return TR_LAST_ERROR;
}

/* not used at the moment- but could be useful */
int TR_IsMainThread(void){
	return ((MAIN_THREAD_ID==&THREAD_ID) || (!MAIN_THREAD_ID));
}

/* A function  which checks whether input is in list of transformations deemed thread unsafe */

int TR_IsThreadSafe(union geo_lab *plab_in, union geo_lab *plab_out){
	int i;
	int n=sizeof(UNSAFE)/sizeof(int);
	for (i=0;i<n;i++){
		if (UNSAFE[i]==(plab_in->u_c_lab).imit || UNSAFE[i]==(plab_out->u_c_lab).imit)
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
    double x=512200.0,y=6143200.0,z=0.0;
    TR* trf;
    FILE *fp;
    char buf[FILENAME_MAX],fname[FILENAME_MAX],*init_path=0;
    if (!TR_IsMainThread()) //Only one thread can succesfully initialise the library
	    return TR_ERROR;
    if (strlen(path)>0) 
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
    fprintf(stdout,"%s %s\n",init_path,fname); /*just debugging */
    #endif
    fp=fopen(fname,"r");
    if (0==fp)
	    return TR_ERROR;
    fclose(fp);
    settabdir(init_path);
    #ifdef _ROUT
    ERR_LOG=fopen("TR_errlog.log","wt");
    #endif
    /* Perform some transformations in order to initialse global statics in transformation functions. TODO: add all relevant transformations below */
    trf=TR_Open("utm32_ed50","utm32_wgs84","");
    #ifdef _ROUT
    if (!trf){
	    fprintf(ERR_LOG,"Failed to open first transformation!\n");
    }
    #endif
    if (0!=trf){
	    ok=TR_Transform(trf,&x,&y,&z,1);
	    TR_Close(trf);
	    }
    trf=TR_Open("utm32_wgs84","fotm","");
    if (0!=trf){
	    rc=TR_Transform(trf,&x,&y,&z,1);
	    TR_Close(trf);}
    trf=TR_Open("utm32_wgs84","s34j","");
	    if (0!=trf){
	    rc=TR_Transform(trf,&x,&y,&z,1);
	    TR_Close(trf);}
    trf=TR_Open("FO_fotm","GR_utm22_gr96","");
    if (0!=trf){
	    rc=TR_Transform(trf,&x,&y,&z,1);
	    TR_Close(trf);}
    trf=TR_Open("utm32_wgs84","fcsH_h_fcsvr10","");
    if (0!=trf){
	    rc=TR_Transform(trf,&x,&y,&z,1);
	    TR_Close(trf);}
    if (0!=ERR_LOG)
	    fprintf(ERR_LOG,"Is init: %d\n************ end initialisation **************\n",(ok==TR_OK));
    /* Set the main thread id */
    if (ok==TR_OK)
	    MAIN_THREAD_ID=&THREAD_ID;
    return (ok==TR_OK)?TR_OK:TR_ERROR; 
}


/* Mock up - not fully implemented! */
void TR_GeoidInfo(TR *tr) {
    char req[512];
    char geoid_name[256];
    int res;
    strcpy(req,"*");
    TR_GetGeoidName(tr,geoid_name);
    //strcat(req,tr->geoid_name);
    strcat(req,geoid_name);
    res=tab_doc_f(req,stdout);
    #ifdef _ROUT
    printf("Request: %s, res: %d\n",req,res);
    #endif
    }

void TR_GetGeoidName(TR *tr,char *name) {
	struct gde_lab *g_lab;
	g_lab=&((tr->geoid_pt->table_u)[tr->geoid_pt->tab_nr]);
	strcpy(name,g_lab->mlb);
	return;
}
	


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
	tr->ngeoids=ngeoids;
	strcpy(tr->geoid_name,"STD");
	tr->geoid_pt=GeoidTable;
	tr->close_table=0;
	return TR_OK;
}
		
		
       
union geo_lab *TR_OpenProjection(char *mlb){
	int label_check;
	union geo_lab *plab;
	plab= malloc(sizeof(union geo_lab));
	if (plab)
		label_check = conv_lab(mlb,plab,"");
	if ((0==plab) || (label_check==0) || (label_check==-1)) {
		free(plab);
		TR_LAST_ERROR=TR_LABEL_ERROR;
		return 0;
		}
	return plab;
}
	


TR *TR_Open (char *label_in, char *label_out, char *geoid_name) {
    int err;
    union geo_lab *plab_in, *plab_out;
    struct mgde_str *special_geoid_table=NULL;
    int has_geoids=0;
    TR *tr;
   /* make room for the TR object proper */
    tr = malloc(sizeof(TR));
    if (0==tr){
	TR_LAST_ERROR=TR_ALLOCATION_ERROR;
        return 0;
    }
    
    /* initialize the plab_in object member */
    plab_in= TR_OpenProjection(label_in);
    if (!plab_in){
	    free(tr);
	    return 0;
    }
    /* initialize the plab_out object member */
    plab_out=TR_OpenProjection(label_out);
    if (!plab_out){
	    free(plab_in);
	    free(tr);
	    return 0;
    }
    /* test for allowed transformation */
    if (!TR_IsThreadSafe(plab_in,plab_out) && !ALLOW_UNSAFE){
	#ifdef _ROUT
	fprintf(ERR_LOG,"%s-> %s not allowed in multithreaded mode!\n",(plab_in->u_c_lab).mlb,(plab_out->u_c_lab).mlb);
	#endif
	free(plab_in);
        free(plab_out);
	free(tr);
	TR_LAST_ERROR=TR_LABEL_ERROR;
	
        return 0;
    }
    /*Set geoid info */
    if (0!=geoid_name && strlen(geoid_name)>0){
	   special_geoid_table=malloc(sizeof(struct mgde_str));
	   special_geoid_table->init=0;
	   has_geoids= geoid_i(geoid_name, GDE_LAB, special_geoid_table,NULL);
	   #ifdef _ROUT
	   fprintf(ERR_LOG,"has geoids: %d, name: %s\n",has_geoids,geoid_name);
	   #endif
	   if (has_geoids<0){  /*on error return null */
		   free(plab_in);
		   free(plab_out);
		   free(tr);
		   geoid_c(special_geoid_table,0,NULL);
		   free(special_geoid_table);
		   TR_LAST_ERROR=TR_ALLOCATION_ERROR;
		   return 0;
	   }
	   tr->geoid_pt=special_geoid_table;
	   tr->close_table=1;
	   tr->ngeoids=has_geoids;
	   tr->use_geoids=(has_geoids>0)?0:-1;
	   strncpy(tr->geoid_name,geoid_name,FILENAME_MAX);
	    }
    else {  /*insert standard geoids */
	   err =TR_GeoidTable(tr);
	   tr->use_geoids=(HAS_GEOIDS)?0:-1;
	   /* TODO: check error! */
	    }
    
    /* transfer everything into the TR object */
    tr->plab_in   = plab_in;
    tr->plab_out  = plab_out;
   
    return tr;

}

void TR_Close (TR *tr) {
    if (0==tr)
        return;    
    free (tr->plab_in);
    free (tr->plab_out);
    /*if using special geoid, close it down */
    if (tr->close_table){
	    #ifdef _ROUT
	    fprintf(ERR_LOG,"Closing special geoid!\n");
	    #endif
	    geoid_c(tr->geoid_pt,0,NULL);
	    free(tr->geoid_pt);}
    //gd_trans(NULL,NULL,0,0,0,NULL,NULL,NULL,NULL,0,NULL,"",0);  might not be needed! WANT TO close hgrid static in gd_trans 
    free (tr);
    return;
}
/*--------------------------------------------------------------------------*/
/* Wrappers of TR_tr */

int TR_Transform(TR *tr, double *X, double *Y, double *Z, int n) {
	int err;
	 if ((0==tr)||(0==X)||(0==Y))
		 return TR_LABEL_ERROR;
	 err=TR_tr(tr->plab_in,tr->plab_out, X,Y,Z,X,Y,Z,n,tr->use_geoids,tr->geoid_pt);
	 return err;
 }
 
 int TR_InverseTransform(TR *tr, double *X, double *Y, double *Z, int n) {
	 int err;
	 if ((0==tr)||(0==X)||(0==Y))
		 return TR_LABEL_ERROR;
	 err=TR_tr(tr->plab_out,tr->plab_in, X,Y,Z,X,Y,Z,n,tr->use_geoids,tr->geoid_pt);
	 return err;
 }

int TR_Transform2(TR *tr, double *X_in, double *Y_in, double *Z_in, double *X_out, double *Y_out, double *Z_out, int n) {
	 int err;
	 if ((0==tr)||(0==X_in)||(0==Y_in)||(0==X_out)||(0==Y_out))
		 return TR_LABEL_ERROR;
	 err=TR_tr(tr->plab_in,tr->plab_out, X_in,Y_in,Z_in,X_out,Y_out,Z_out,n,tr->use_geoids,tr->geoid_pt);
	 return err;
 }
 
 int TR_InverseTransform2(TR *tr, double *X_in, double *Y_in, double *Z_in, double *X_out, double *Y_out, double *Z_out, int n) {
	 int err;
	 if ((0==tr)||(0==X_in)||(0==Y_in)||(0==X_out)||(0==Y_out))
		 return TR_LABEL_ERROR;
	 err=TR_tr(tr->plab_out,tr->plab_in, X_in,Y_in,Z_in,X_out,Y_out,Z_out,n,tr->use_geoids,tr->geoid_pt);
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
 
/* transform one or more xyz-tuples */
int TR_tr(
   union geo_lab *plab_in,
   union geo_lab *plab_out, 
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
	 
   
    double *x_in, *y_in,*x_out,*y_out,z, GH = 0, z1 = 0, z2 = 0;
    int err, ERR = 0, i; //use_geoids; 
   
    if ((plab_in->u_c_lab).cstm==CRT_SYS_CODE){
        x_in=Y_in;
        y_in=X_in;}
    else{
        x_in=X_in;
        y_in=Y_in;}
    if ((plab_out->u_c_lab).cstm==CRT_SYS_CODE){
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
		fprintf(ERR_LOG,"\nProj: %s->%s, last err: %d, out: %.5f %.5f %.3f\n",(plab_in->u_c_lab).mlb,(plab_out->u_c_lab).mlb,err,x_in[i],y_in[i],z);
	#endif
        /*err = gd_trans(tr->plab_in, tr->plab_out,  x,y,z,  X+i,Y+i, (Z? Z+i: &z2), &GH, -1, &GeoidTable, 0, 0);
         *   KE siger at arg 0 før GeoidTable er bedre end -1. Ved -1 er det "forbudt" at bruge geoidetabeller */
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
    is_geo_in=(((trf->plab_in->u_c_lab).cstm)==GEO_SYS_CODE);
    is_geo_out=(((trf->plab_out->u_c_lab).cstm)==GEO_SYS_CODE);
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
     gd_trans(NULL,NULL,0,0,0,NULL,NULL,NULL,NULL,0,NULL,"",0);
     TR_GeoidTable(NULL);
     c_tabdir_file(0,NULL);
}

void TR_TerminateThread(void){
    TR_TerminateLibrary();
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
    union geo_lab  *TC=TR_OpenProjection(mlb);
    if (!TC)
	return TR_LABEL_ERROR;
    err=sputshpprj(wkt_out,TC); /* See fputshpprj.h for details on return value */
    free(TC);
    return err;}
    
/* out must be an array of length 2*/
int TR_GetLocalGeometry(char *mlb, double x, double y, double *s, double *mc){
	double xo,yo,out[2],dgo[4];
	int direct=1,ret;
	union geo_lab  *TC=TR_OpenProjection(mlb);
	if (!TC)
		return TR_LABEL_ERROR;
	if (((TC->u_c_lab).cstm)==CRT_SYS_CODE){
		free(TC);
		return TR_ERROR;
		}
	direct=(((TC->u_c_lab).cstm)!=GEO_SYS_CODE)?1:-1;
	#ifdef _ROUT
	fprintf(ERR_LOG,"init: %d, %.2f %.2f\n",(TC->u_c_lab).init,x,y);
	#endif
	ret=ptg_d(TC,direct,y,x,&yo,&xo,"",ERR_LOG,out,dgo);
	*s=out[0];
	*mc=out[1];
	free(TC);
	return (ret==0)?TR_OK:TR_ERROR;
}
        