/*
 *  ESRI World file and affine transformation package. 
 * 
 *  TODO: fix write_worldfile to use affine[6] format
 *  
 *  Might be a good idea to include support for the dsup file format
 *  used for specification of interior orientation in photogrammetry.
 * 
 *  Based on code (c) by Thomas Knudsen 2006-2012
 * 
 */

#include <string.h>

/*
 * These API entries are inspired by the ESRI world file explanation given on
 * wikipedia: http://en.wikipedia.org/wiki/World_file
 */

/* Primary term:  Ax, Dy (for unrotated grids, these equal the GSD) */
#define A (affine[0][0])
#define D (affine[1][0])

/* Mixed terms "rotation": By, Ex */
#define B (affine[0][1])
#define E (affine[1][1])

/* Origin - top left pixel center C: easting, F: northing */
#define C (affine[0][2])
#define F (affine[1][2])


#undef F
#undef C
#undef E
#undef B
#undef D
#undef A


#define A (affine[0])
#define D (affine[3])

/* Mixed terms "rotation": By, Ex */
#define B (affine[1])
#define E (affine[4])

/* Origin - top left pixel center C: easting, F: northing */
#define C (affine[2])
#define F (affine[5])

double affine_northing (const double affine[6], double row, double column) {
	return D*column + E*row + F;	
}

double affine_easting (const double affine[6], double row, double column) {
	return A*column + B*row + C;	
}

double affine_row (const double affine[6], double northing, double easting) {
	return (-D*easting + A*northing + D*C - A*F) / (A*E - D*B);
}

double affine_column (const double affine[6], double northing, double easting) {
	return (E*easting - B*northing + B*F - E*C) / (A*E - D*B);
}
#undef F
#undef C
#undef E
#undef B
#undef D
#undef A










int read_world_file (const char *fnin, double *affine) {
    FILE *f;
    char *fn;
    size_t n;


    /* 1: create mangled file name as per ESRI spec (.gif -> .gfw) */

    n = strlen (fnin);
    if (n < 3)
        return 1;

    fn = malloc (n + 2);
    if (0==fn)
        return 2;

    strcpy (fn, fnin);
    fn[n - 2] = fn[n - 1];
    fn[n - 1] = 'w';


    /* 2: try alternate naming scheme if not able to open world file */

    f = fopen (fn, "rt");
    if (0==f) {
        strcpy (fn, fnin);
        fn[n] = 'w';
        f = fopen (fn, "rt");
        if (0==f)
            return (free (fn), 3);
    }

    n = fscanf (f, "%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n", affine, affine+1, affine+2, affine+3, affine+4, affine+5);


    /* 3: clean up and return */

    fclose (f);
    free (fn);
    
    if (n!=6)
        return 4;
        
    return 0;
}








int write_world_file (char *fnout, double N, double E, double dN, double dE) {
    FILE *f;
    char *fn;
    size_t n;


    /* 1: create mangled file name as per ESRI spec (.gif -> .gfw) */

    n = strlen (fnout);
    if (n < 3)
        return 1;

    fn = malloc (n + 1);
    if (0==fn)
        return 2;

    strcpy (fn, fnout);
    fn[n - 2] = fn[n - 1];
    fn[n - 1] = 'w';


    /* 2: open and write world file */

    f = fopen (fn, "wt");
    if (0==f) {
        free (fn);
        return 3;
    }

    fprintf (f, "%.9f \n 0 \n 0 \n %.9f \n %.9f \n %.9f \n", dE, -dN, E, N);


    /* 3: clean up and return */

    fclose (f);
    free (fn);
    return 0;
}
