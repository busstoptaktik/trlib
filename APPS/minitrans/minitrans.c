/* gcc -I ../trlib/TR_INC/ -L. -o minitrans minitrans.c libtr.so */

#include <stdio.h>
#include <math.h>
#include "trlib_api.h"

/*
 * 
 * In MSVC to use the M_PI constant from math.h you need to:
 * #define _USE_MATH_DEFINES
 * before you include math.h
 * 
 */


int main (int argc, char *argv[]) {
    TR *tr;
    FILE *in, *out;
    enum {linesize = 16000};
    char line[linesize+1];
    double x, y, z, radegpre = 1, radegpost = 1;
    int ret;
    
    ret = TR_InitLibrary (argv[3]);
    if (TR_OK != ret) {
        printf ("minitrans: TR_InitLibrary returns %d - bye\n", ret);
        fflush (stdout);
        return 2;
    }
    
    tr  = TR_Open (argv[1], argv[2],"");
    if (0==tr) {
        printf("minitrans: tr nullified\n");
        fflush(stdout);
        return 4;
    }
    
    /* convert input data to radians if in degrees */
    if (0 == strncmp (argv[1], "geo", 3))
        radegpre = atan(1) / 45.;
    
    /* convert output data to degrees if geographic */
    if (0 == strncmp (argv[2], "geo", 3))
        radegpre = 45. / atan(1);

    in  = fopen (argv[4], "rt");
    out = fopen (argv[5], "wt");
    
    while (0 != fgets(line, linesize, in)) {
		sscanf (line, "%*f,%*f,%*f,%lf,%lf,%lf", &x, &y, &z);
		x *= radegpre;
		y *= radegpre;
		TR_Transform(tr, &x, &y, &z, 1);
		x *= radegpost;
		y *= radegpost;
		
		/* todo: handle geographic output format */
		fprintf (out, "%7.2f, %7.2f, %5.2f,    %s", x, y, z, line);
    }

    fclose (out);
    fclose (in);
    TR_Close (tr);
    TR_TerminateLibrary();
    return 0;
}
