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
 
/* Simple KMSTrLib API, simlk 03.11
Compile into a single shared library linking to compiled trffuncs code, use def file to expose this API
e.g.
gcc -I../include -O2 -Wall -c ../trffuncs/*.c ./*.c  (compiling done here)
gcc -o TrLib.dll -shared *.o KMSTrLib_simple.def (linking done here....)
*/

/*
----------------------------------------------------------------------
REVIEWmarkeringer
----------------------------------------------------------------------

Indlagt meget mere luft:
    1. blanke linjer som mere tydeligt adskiller blokke,
    2. blanktegn der adskiller operatorer og operander.

Erstattet tab med 4 spaces (jvf. Guido Van Rossum)

Rettet Pythonformatering til C-formatering (vel vidende at Python
er mere læseligt, men vi må hellere holde os til den etablerede
idiomatik

Ændret headerinklusionen så systemheaders kommer først

Tilføjet parenteser i tilfælde hvor operatorpræcedens kan være mindre
kendt (specielt ifbm. == og ||)
 
Rettet %i til %d i printf. De betyder i princippet det samme, men %d
er meget mere udbredt (%i er muligvis bevaret for bagudkompatibilitet
med K&R-C)

Rettet
    error_msg=0;
    while (error_msg==0 && i<npoints){
        ...
til
    for (i = 0, error_msg=0; (error_msg==0) && (i<npoints); i++)
        error_msg=gd_trans(plab_in,plab_out,xy_in[2*i+1],xy_in[2*i],0,xy_out+2*i+1,xy_out+2*i,&Z,&GH,-1,&GeoidTable,NULL,NULL);
 
Rettet 
    if (0==fp) return TR_ERROR;
til
    if (0==fp)
        return TR_ERROR;

(den første form er vist lidt Fortranagtig)

Rettet
    fp==NULL
til
    0==fp

(dette emne kan være årsag til hellige krige).
NULL er (iflg. C-standarden) identisk med 0, og 0 kan pr def konverteres til
alle objektpointertyper - og er (bizart nok) ikke nødvendigvis repræsenteret ved et digitalt 0 i maskinlageret.
0 er med andre ord en undtagelsesform - derfor er vi nogen som bare skriver den som 0: vi VED at 0 er en undtagelse,
hvorimod det ikke altid er oplagt hvad en eller anden #define NULL nede i en fjern headerfil dækker over.




Sideordnede overvejelser... (designoprydning for systemet): 
Hvorfor kan vi ikke bare returnere error_msg og hvorfor returnerer conv_lab ikke bare 0 ved fejl
(samme idiomatiske design som fopen()) - det tyder på at designet kunne optimeres...

Kan vi få dette:
plab_out=malloc(sizeof(union geo_lab));
label_check = conv_lab(label_out,plab_out,"");

til at få en form mere i retning af:

union geo_lab *p = conv_lab(label_out,"");

Jeg ville foretrække tre arrays ind og tre ud. Eventuelt kunne de være de samme (in-place transformation)
(eller vi kunne levere et "in-place entry point)

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "conv_lab.h"
#include "gd_trans.h"
#include "gettabdir.h"
#include "geoid_d.h"
#include "geoid_c.h"
#include "geoid_i.h"
#include "fputshpprj.h"
#include "trlib_intern.h"
#include "trlib_api.h"
#define TRLIB_VERSION "beta 0.001 2011-09-23"
#define CRT_SYS_CODE 1 /*really defined in def_lab.txt, so perhaps we should first parse this with a conv_lab call */
/* We use a global geoid table. This could be made thread local if needed */
 struct mgde_str GeoidTable;
/* Need to let KMSTrans parse the def-files, before any transformation
   We need to figure out a way of setting the tabdir to the directory of the running shared library (for people who don't want geoids)
*/
int InitLibrary(char *path) {
   int R=0;
  GeoidTable.init = 0;
    /* Will only try to set tabdir to something if path has length >0 otherwise, we should really set tabdir to the path of the running library TODO */ 
    if (strlen(path)>0){
        settabdir(path);
        R = geoid_i("STD", GDE_LAB, &GeoidTable,NULL);
	printf("%s, %d, %d\n",path,R,GeoidTable.init); /*only for debugging */}
    return ((R>0) && GeoidTable.init);
}

int IsGeoidTableInitialised(void) {
    return (int) GeoidTable.init;
}


void GetVersion(char *buffer,int BufSize) {
    strncpy(buffer,TRLIB_VERSION,BufSize);
}
	
/* A simple wrapper for tr */
int Transform(char *label_in, char *label_out, double *X, double *Y, double *Z, int npoints) {
    int err_msg;
    TR *trf=tropen(label_in,label_out);
    if (trf==0){
    return LABEL_ERROR;} /* No need to close anything! */
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





TR *tropen (char *label_in, char *label_out) {
    int label_check;
    union geo_lab *plab_in, *plab_out;
    TR *tr;
    
    /* make room for the TR object proper */
    tr = malloc(sizeof(TR));
    if (0==tr)
        return 0;
    
    /* initialize the plab_in object member */
    plab_in= malloc(sizeof(union geo_lab));
    if (plab_in)
        label_check = conv_lab(label_in,plab_in,"");
    if ((0==plab_in) || (label_check==0) || (label_check==-1)) {
        free(plab_in);
        return 0;
    }
    
    /* initialize the plab_out object member */
    plab_out = malloc(sizeof(union geo_lab));
    if (plab_out)
        label_check = conv_lab(label_out,plab_out,"");
    if ((0==plab_out) || (label_check==0) || (label_check==-1)) {
        free(plab_in);
        free(plab_out);
        return 0;
    }
    
    /* transfer everything into the TR object */
    tr->plab_in   = plab_in;
    tr->plab_out  = plab_out;
    tr->err = 0;
    return tr;

}

void trclose (TR *tr) {
    if (0==tr)
        return;    
    free (tr->plab_in);
    free (tr->plab_out);
    free (tr);
    return;
}
/*--------------------------------------------------------------------------*/


/* transform one or more xyz-tuples in place */
int tr(TR *tr, double *X, double *Y, double *Z, int n) {
    double *x_in, *y_in,*x_out,*y_out,z, GH = 0, z1 = 0, z2 = 0;
    int err, ERR = 0, i;
    int use_geoids=(IsGeoidTableInitialised()?0:-1);
    if ((0==tr)||(0==X)||(0==Y))
        return TR_ERROR;
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
    for (i = 0;  i < n;  i++) {
	z = Z? Z[i]: z1;
        err = gd_trans(tr->plab_in, tr->plab_out,  y_in[i], x_in[i], z,  y_out+i, x_out+i, (Z? Z+i: &z2), &GH, use_geoids, &GeoidTable, "", 0);
        /*err = gd_trans(tr->plab_in, tr->plab_out,  x,y,z,  X+i,Y+i, (Z? Z+i: &z2), &GH, -1, &GeoidTable, 0, 0);
         *   KE siger at arg 0 før GeoidTable er bedre end -1. Ved -1 er det "forbudt" at bruge geoidetabeller */
        if (err)
           ERR = err;
    }

    return ERR? TR_ERROR: TR_OK;
}

/* read xyz-tuples from f_in; stream transformed tuples to f_out */
int trstream(TR *tr, FILE *f_in, FILE *f_out, int n) {

    double XYZ[3];
    int ERR = 0;
    int use_geoids=(IsGeoidTableInitialised()?0:-1);
    int i = 0;
    int swap_xy_in=0, swap_xy_out=0;

    enum {BUFSIZE = 16384};
    char buf[BUFSIZE];

    double GH; /* ignored, but needed in gd_trans call */
    
    if ((0==tr) || (0==f_in) || (0==f_out))
        return TR_ERROR;
    
    if ((tr->plab_in->u_c_lab).cstm==CRT_SYS_CODE) swap_xy_in=1; /* If crt-coordinates we want to input x,y,z to gd_trans, otherwise y,x,z */
    
    if ((tr->plab_out->u_c_lab).cstm==CRT_SYS_CODE) swap_xy_out=1;
          
    while (0 != fgets(buf, BUFSIZE, f_in)) {
        int    argc, err;
        double xyz[3];
        
        argc = sscanf(buf, "%lf %lf %lf", &xyz[swap_xy_in], &xyz[1-swap_xy_in], &xyz[2]);
        
        /* 2D transformation? */
        if (argc==2)
            xyz[2] = 0;
            
        /* Uninterpretable line */
        if ((argc==1) || (argc==0)) {
            /* n==-1: break on garbage */
            if (n==-1)
                break;
            /* n==0: ignore garbage - break on end-of-file */
            if (n==0)
                continue;
        }

        err = gd_trans(tr->plab_in, tr->plab_out,  xyz[1], xyz[0], xyz[2],  &XYZ[1], &XYZ[0], &XYZ[2], &GH, use_geoids, &GeoidTable, "", 0);
        i++;

        /* buffer last error */
        if (err)
            ERR = err;

        if ((n==-1) && err)
            break;
        
        fprintf(f_out, "%.10g %.10g %.10g\n", XYZ[swap_xy_out], XYZ[1-swap_xy_out], XYZ[2]);

        if (i==n)
            break;
    }

    trclose(tr);
    return ERR? TR_ERROR: TR_OK;
}

void TerminateLibrary(void) {

    geoid_c(&GeoidTable,0,NULL);
}





/* This will work for now until we get a version of fputshpprj which writes to a string */
int GetEsriText(char *label_in, char *file_name){
    union geo_lab *plab_in;
    FILE *fp;
    int   label_check;

    fp = fopen(file_name, "w");
    if (0==fp)
        return TR_ERROR;
    plab_in = malloc(sizeof(union geo_lab));
    
    label_check = conv_lab(label_in,plab_in,"");
    if ((label_check==0) || (label_check==-1)){
        free(plab_in);
        return LABEL_ERROR;
    }
    
    label_check=fputshpprj(fp,plab_in); /* See fputshpprj.h for details on return value */
    
    free(plab_in);
    fclose(fp);
    return label_check;}
