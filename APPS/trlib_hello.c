/*
 * Demo program for trlib.
 * 
 * Illustrates the basic init/open/tranform/close/terminate life cycle
 * of the TR object.
 * 
 * Additionally, shows how to update TR using the TR_Insert (label) function.
 * 
 * Compilation:
 * gcc -I ../../trlib/TR_INC/ -L<path/to/lib> -o trlib_hello trlib_hello.c -lTrLib.dll && trlib_hello
 * 
 * Thomas Knudsen, thokn@kms.dk, 2013-06-07
 * 
 */

#include <trlib_api.h>

#include <stdio.h>
#include <math.h>

#define TORAD(x) ((x) * (M_PI/180.0))
#define TODEG(x) ((x) * (180.0/M_PI))


int main (void) {
    int ret = 0;
    double x, y, z;
    TR *tr;
    
    /* Tell trlib where to find geoids etc. */
    ret = TR_InitLibrary ("c:\\Geodata\\Geoids2013"); /* <- "/Path/to/geoids" */
    printf ("TR_Init ret = %d\n", ret);
    
    /*
     * Create a TR object transforming from
     * 
     * geographical coordinates, datum ETRS89,
     * 
     * to
     * 
     * UTM zone 32 in the same datum
     * 
     * No height datum is specified, so heights ("z") are ignored.
     * 
     */
    tr = TR_Open ("geo_etrs89","utm32_etrs89", 0);
    printf ("TR_Open ret = %p\n", tr);
    
    
    x = TORAD(12.34); /* longitude */
    y = TORAD(55.5);  /* latitude */
    z = 30;           /* height (ignored - zero on output) */
    
    
    /*
     * x, y, z may be arrays, in which case the last parameter of
     * the call to TR_transform should indicate the length of the
     * arrays
     * 
     */

    ret = TR_Transform (tr, &x, &y, &z, 1);
    printf ("ret = %d,  x = %f,  y = %f,  z = %f\n", ret, x,y,z);
    
    /* insert utm33 at the "input projection"-slot of tr */
    ret =  TR_Insert (tr, "utm33_etrs89", 0);
    printf ("TR_Insert ret = %d\n", ret);
    /* now tr tranforms input in utm zone 33 to output in utm zone 32 */


    /*
     * do an *inverse* transform of xyz(utm32),
     * i.e. transform from utm32 to utm33
     * 
     */
    ret = TR_InverseTransform (tr, &x, &y, &z, 1);
    printf ("ret = %d,  x = %f,  y = %f,  z = %f\n", ret, x,y,z);

    /* insert geo at the "output projection"-slot */
    ret =  TR_Insert (tr, "geo_etrs89", 1);
    printf ("insert ret = %d\n", ret);
    /* now: input: utm33, output: geo */
    
    /* transform back to geographical coordinates */
    ret = TR_Transform (tr, &x, &y, &z, 1);
    printf ("ret = %d,  x = %f,  y = %f,  z = %f\n", ret, TODEG(x), TODEG(y), z);



    /*
     * Now add some handling of heights. First: input slot becomes
     * 
     * Projection:     None (geographical coordinates),
     * Datum:          ETRS89
     * Heights:        Ellipsoidal
     * Vertical Datum: DVR90 (Danish Vertical Reference, 1990)
     *
     */ 
    ret = TR_Insert (tr, "geoEetrs89", 0);
    printf ("TR_Insert ret = %d\n", ret);

    /*
     * Then switch the output system to
     * 
     * Projection:     UTM zone 33,
     * Datum:          ETRS89,
     * Heights:        Orthometric
     * Vertical Datum: DVR90 (Danish Vertical Reference, 1990)
     * 
     */
    ret = TR_Insert (tr, "utm33Hetrs89_h_dvr90", 1);
    printf ("TR_Insert ret = %d\n", ret);
    /* now: input: geoE, output: utm33Hdvr90 */

    /*
     * We change our minds, and switch the output system to
     * 
     * Projection:     None (geographical coordinates),
     * Datum:          ETRS89,
     * Heights:        Orthometric
     * Vertical Datum: DVR90 (Danish Vertical Reference, 1990)
     * 
     */
    ret = TR_Insert (tr, "geoHetrs89_h_dvr90", 1);
    printf ("TR_Insert ret = %d\n", ret);
    /* now: input: geoE, output: geoHdvr90 */

    
    z = 40;  /* <- set an ellipsoidal height of 40 m */
    

    /* Transform from geographical ellipsoidal to ditto orthometric */    
    ret = TR_Transform (tr, &x, &y, &z, 1);
    printf ("ret = %d,  x = %f,  y = %f,  z = %f\n", ret, TODEG(x),TODEG(y),z);

    /* Deallocate the TR object */
    TR_Close (tr);
    
    /* Deallocate global state */
    TR_TerminateLibrary (); 
    
    return 0;
}
