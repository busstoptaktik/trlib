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
//#include <windows.h>
#include "conv_lab.h"
#include "gd_trans.h"
#include "gettabdir.h"
#include "geoid_d.h"
#include "geoid_c.h"
#include "geoid_i.h"
#include "sputshpprj.h"
#include "c_tabdir_file.h"
#include "tab_doc_f.h"
#include "trlib_intern.h"
#include "trlib_api.h"
#include "trthread.h"
#define TRLIB_VERSION "RC0 v1.0 2012-02-03"
#define CRT_SYS_CODE 1 /*really defined in def_lab.txt, so perhaps we should first parse this with a conv_lab call */
#define TR_TABDIR_ENV "TR_TABDIR" /* some env var, that can be set by the user to point to relevant library. Should perhaps be in trlib_intern.h */
#define TR_DEF_FILE "def_lab.txt"
static int HAS_GEOIDS=0; //global flag which signals if we have geoids available 
static THREAD_SAFE int THREAD_ID=0; //used to distinguish the thread that initialised the library from other threads.
static int *MAIN_THREAD_ID=0; // same as above - also signals whether the library has been succesfully initialised.
THREAD_SAFE int TR_LAST_ERROR=TR_OK; /*Last error msg (int) from gd_trans - also set in tropen and TR_GeoidTable on allocation errors.*/
FILE *ERR_LOG=0;
int TR_GetLastError(void){
    return TR_LAST_ERROR;
}
int TR_IsMainThread(void){
	return ((MAIN_THREAD_ID==&THREAD_ID) || (!MAIN_THREAD_ID));
}
/* Need to let KMSTrans parse the def-files, before any transformation */
int TR_InitLibrary(char *path) {
    int ok=0,rc;
    double x=512200.0,y=6143200.0,z=0.0;
    TR* trf;
    FILE *fp;
    char buf[64],fname[1024],*init_path=0;
    if (!TR_IsMainThread()) //Only one thread can succesfully initialise the library
	    return 0;
    if (strlen(path)>0) 
	    init_path=path;
    else
	    init_path=getenv(TR_TABDIR_ENV);
    if (0==init_path){ 
	    sprintf(buf,"./");
	    init_path=buf;}
    else if (init_path[strlen(init_path)-1]!='/' && init_path[strlen(init_path)-1]!='\\')
	    strcat(init_path,"/");
    strcpy(fname,init_path);
    strcat(fname,TR_DEF_FILE);
    #ifdef _ROUT
    fprintf(stdout,"%s %s\n",init_path,fname); /*just debugging */
    #endif
    fp=fopen(fname,"r");
    if (0==fp)
	    return 0;
    fclose(fp);
    settabdir(init_path);
    #ifdef _ROUT
    ERR_LOG=fopen("TR_errlog.log","wt");
    #endif
    /* Perform some transformations in order to initialse global statics in transformation functions. TODO: add all relevant transformations below */
    trf=tropen("utm32_ed50","utm32_wgs84","");
    if (0!=trf){
	    ok=tr(trf,&x,&y,&z,1);
	    trclose(trf);
	    }
    trf=tropen("utm32_wgs84","fotm","");
    if (0!=trf){
	    rc=tr(trf,&x,&y,&z,1);
	    trclose(trf);}
    trf=tropen("utm32_wgs84","s34j","");
	    if (0!=trf){
	    rc=tr(trf,&x,&y,&z,1);
	    trclose(trf);}
    trf=tropen("FO_fotm","GR_utm22_gr96","");
    if (0!=trf){
	    rc=tr(trf,&x,&y,&z,1);
	    trclose(trf);}
    trf=tropen("utm32_wgs84","fcsH_h_fcsvr10","");
    if (0!=trf){
	    rc=tr(trf,&x,&y,&z,1);
	    trclose(trf);}
    if (0!=ERR_LOG)
	    fprintf(ERR_LOG,"Is init: %d\n************ end initialisation **************\n",(ok==TR_OK));
    /* Set the main thread id */
    if (ok==TR_OK)
	    MAIN_THREAD_ID=&THREAD_ID;
    return (ok==TR_OK); 
}
/* Mock up. Look in kmstr5 for better ideas */
void TR_GeoidInfo(TR *tr) {
    tab_doc_f(tr->geoid_name,stdout);
    }


void TR_GetVersion(char *buffer,int BufSize) {
    strncpy(buffer,TRLIB_VERSION,BufSize);
}
	
/* A simple wrapper for tr */
int TR_Transform(char *label_in, char *label_out, double *X, double *Y, double *Z, int npoints) {
    int err_msg;
    TR *trf=tropen(label_in,label_out,"");
    if (trf==0){
    return TR_LABEL_ERROR;} /* No need to close anything! */
    err_msg=tr(trf,X,Y,Z,npoints);
    trclose(trf);
    return err_msg;
}

/*--------------------------------------------------------------------------
fopen() / fclose() / FILE * style interface to the transformation system 
makes trlib resemble "proj"s calling convention:

EXAMPLE 1:

minitrans.c
    transform data from file argv[3], write to file argv[4]
    input  data reference system is described by minilabel in argv[1],
    output data reference system is described by minilabel in argv[2].

------------------------------------------------
#include <stdio.h>
#include <trlib.h>

int main (int argc, char *argv[]) {
    TR *tr;
    FILE *in, *out;
    int err;
     
    tr  = tropen (argv[1], argv[2]);

    in  = fopen (argv[3], "rt");
    out = fopen (argv[4], "wt");
    
    err = trstream (tr, in, out, 0);
     
    fclose (out);
    fclose (in);

    trclose (tr);
    return err;
}
------------------------------------------------

21 lines of code - including 5 blanks.

This interface is compact, and in my humble opinion much more in line
with common C-programming idioms, than either of the two existing
interfaces.

Note, however, that this interface is still a sketch - we need to
decide how to handle the setup part pertaining to settabdir.

It could be handled by a 3rd argument to tropen, which could be a
path name or a 0, the latter meaning "use system default".
 
 
------------------------------------------------------------------------ 
 
 
EXAMPLE 2:

nanotrans.c
    transform one data point from command line
    input  data reference system is described by minilabel in argv[1],
    output data reference system is described by minilabel in argv[2].
    x, y, (z) are in argv[3..5]

------------------------------------------------
#include <stdio.h>
#include <trlib.h>

int main (int argc, char *argv[]) {
    TR *tr;
    double x, y, z = 0;
    int err;
     
    tr  = tropen (argv[1], argv[2]);

    x = atof(argv[3]);
    y = atof(argv[4]);
    if (argc > 5)
        z = atof(argv[5])
    
    err = tr (tr, &x, &y, &z, 1);
    printf("%.10g %.10g %.10g\n", x, y, z);
     
    trclose (tr);
    return err;
}
------------------------------------------------
 
----------------------------------------------------------------------*/
/* Function which inserts a standard geoid pointer into a TR object */
int TR_GeoidTable(TR *tr){
	static THREAD_SAFE struct mgde_str *GeoidTable=NULL;
	static THREAD_SAFE int init=0;
	static THREAD_SAFE int ngeoids=0;
	/*if called with NULL close evrything down */
	if (0==tr){
			if (init)
			    geoid_c(GeoidTable,0,NULL);
			free(GeoidTable);
			GeoidTable=NULL;
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
		
		
       
int TR_IsFehmarn(union geo_lab *plab_in, union geo_lab *plab_out){
	 return (((plab_in->u_c_lab).imit == FHMASK) || ((plab_out->u_c_lab).imit == FHMASK));
}


TR *tropen (char *label_in, char *label_out, char *geoid_name) {
    int label_check,err;
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
    plab_in= malloc(sizeof(union geo_lab));
    if (plab_in)
        label_check = conv_lab(label_in,plab_in,"");
    if ((0==plab_in) || (label_check==0) || (label_check==-1)) {
        free(plab_in);
	TR_LAST_ERROR=TR_LABEL_ERROR;
        return 0;
    }
    
    /* initialize the plab_out object member */
    plab_out = malloc(sizeof(union geo_lab));
    if (plab_out)
        label_check = conv_lab(label_out,plab_out,"");
    if ((0==plab_out) || (label_check==0) || (label_check==-1)) {
        free(plab_in);
        free(plab_out);
	TR_LAST_ERROR=TR_LABEL_ERROR;
        return 0;
    }
    if (TR_IsFehmarn(plab_in,plab_out) && !TR_IsMainThread()){
	free(plab_in);
        free(plab_out);
	TR_LAST_ERROR=TR_LABEL_ERROR;
	#ifdef _ROUT
	fprintf(ERR_LOG,"Fehmarn transformations not allowed in multihreaded mode!\n");
	#endif
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

void trclose (TR *tr) {
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
    //gd_trans(NULL,NULL,0,0,0,NULL,NULL,NULL,NULL,0,NULL,"",0); /*might not be needed! WANT TO close hgrid static in gd_trans */
    free (tr);
    return;
}
/*--------------------------------------------------------------------------*/


/* transform one or more xyz-tuples in place */
int tr(TR *tr, double *X, double *Y, double *Z, int n) {
   
    double *x_in, *y_in,*x_out,*y_out,z, GH = 0, z1 = 0, z2 = 0;
    int err, ERR = 0, i; //use_geoids; 
    if ((0==tr)||(0==X)||(0==Y))
        return TR_LABEL_ERROR;
    if ((tr->plab_in->u_c_lab).cstm==CRT_SYS_CODE){
        x_in=Y;
        y_in=X;}
    else{
        x_in=X;
        y_in=Y;}
    if ((tr->plab_out->u_c_lab).cstm==CRT_SYS_CODE){
        x_out=Y;
        y_out=X;}
   else{
        x_out=X;
        y_out=Y;}
    /*geoid_pt=&GeoidTable;
    use_geoids=(has_geoids)?0:-1;*/
    for (i = 0;  i < n;  i++) {
	z = Z? Z[i]: z1;
        err = gd_trans(tr->plab_in, tr->plab_out,  y_in[i], x_in[i], z,  y_out+i, x_out+i, (Z? Z+i: &z2), &GH,tr->use_geoids,tr->geoid_pt, "", ERR_LOG);
	#ifdef _ROUT
	if (err)
		fprintf(ERR_LOG,"\nProj: %s->%s, last err: %d, out: %.5f %.5f %.3f\n",(tr->plab_in->u_c_lab).mlb,(tr->plab_out->u_c_lab).mlb,err,x_in[i],y_in[i],z);
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
int trstream(TR *trf, FILE *f_in, FILE *f_out, int n) {
   
    int ERR = 0,i=0;
    enum {BUFSIZE = 16384};
    char buf[BUFSIZE];
    if ((0==trf) || (0==f_in) || (0==f_out))
        return TR_ERROR;
  
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

        err = tr(trf,&x,&y,&z,1);
        i++;

        /* buffer last error */
        if (err)
            ERR = err;

        if ((n==-1) && err)
            break;
        
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






/* This will work for now until we get a version of fputshpprj which writes to a string */
int TR_GetEsriText(char *label_in, char *wkt_out){
    union geo_lab *plab_in;
    int   label_check;
    plab_in = malloc(sizeof(union geo_lab));
    label_check = conv_lab(label_in,plab_in,"");
    if ((label_check==0) || (label_check==-1)){
        free(plab_in);
        return TR_LABEL_ERROR;
    }
    label_check=sputshpprj(wkt_out,plab_in); /* See fputshpprj.h for details on return value */
    free(plab_in);
    return label_check;}
