#if !0xD0C
/*

    G R I M . C

    GRid Interpolation and Memory mapping

    Thomas Knudsen, 2012-11-13...23

    Based on geoideinterpolationstest.c, which was in turn based on
    the BooGIE grid implementation boogrid.c, counting its ancestors
    back to code written for my masters thesis (around 1991 - ufgrid.c),
    and PhD-project (1998 - busgrid.c, grin.c).

    Originally, these files were (C) by Thomas Knudsen, and distributed
    under the GNU General Public Licence (GPL) v. 2.

    Recently, I changed the licence to ISC, for compatibility with trlib.
    Hence, this somewhat convoluted copyright notice:

 *
 *
 * Copyright (C) 1991-2012 Thomas Knudsen [knudsen.thomas at gmail dot com]
 *
 * Copyright (c) 2012, 2013 National Survey and Cadastre, Denmark
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
 *

thokn@SIT1130565 ~/2012/Playground

gcc  -I ../../trlib/TR_INC/ -I .  -std=gnu99 -xc -W -Wall -W -pedantic -o grim grim.c
gcc  -I ../../trlib/TR_INC/ -I .  -W -Wall -W -pedantic -DTEST_GRIM -DWITH_MMAP -o grim grim.c -lmman
gcc  -I ../../trlib/TR_INC/ -I .  -W -Wall -W -pedantic -DTEST_GRIM -o grim grim.c

    $ g c c  -std=c99 -xc -W -Wall -W -pedantic -DESRITEST  -o geoideinterpolationstest geoideinterpolationstest.c

-------------------------------------------------------------------------*/
#endif

#undef USE_MMAP
#if !defined(_WIN32) || defined WITH_MMAP
#define USE_MMAP
#endif
/* for large file mmap, see
http://stackoverflow.com/questions/7222164/mmap-an-entire-large-file */

/* large file IO. Formerly used by grim, left for reference */

/* http://coding.derkeiler.com/Archive/C_CPP/comp.lang.c/2006-12/msg03560.html */
#define _FILE_OFFSET_BITS 64
#include <stdio.h>

/* http://thompsonng.blogspot.dk/2011/09/vs2010-fseeko.html */
#ifdef _WIN32
#    ifdef __MINGW32__
#        define fseeko fseeko64
#        define ftello ftello64
#    else
         typedef __int64 off_t;
#        define fseeko _fseeki64
#        define ftello _ftelli64
#    endif
#endif
/*
if other system not supporting LFS (http://en.wikipedia.org/wiki/Large_file_support)
define fseeko fseek
define ftello ftell
typedef long off_t;
endif
*/




#include <stdio.h>      /*  for printf and friends  */
#include <stdlib.h>     /*  for malloc/free         */
#include <ctype.h>      /*  for isspace()           */
#include <math.h>       /*  for HUGE_VAL            */
#include "pom.h"
#include "grim.h"
#include "lord.h"

#ifdef USE_MMAP
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#endif


/*#include "pom.c"*/        /*  plain ol' metadata */
#include "affine.h"     /*  affine transformations and ESRI worldfile */


#ifdef _WIN32
#include <windows.h>
#endif



/* Implementation starts here */






struct GRIM {
    long           /* size_t here will get us into trouble for signed/unsigned comparisons */
        ncols,
        nrows;
    size_t         /* file lay out */
        offset,
        stride,
        channels;
    double         /* grid georeference, etc. */
        xllcenter,
        yllcenter,
        xcellsize,
        ycellsize,
        nodata;
    double         /* parameters for affine transformations between
                      world coordinates (m) and grid coordinates (px) */
        affine[6];


    float         /* the memory map herself */
        *data;
    POM
        *pom;

    FILE
        *file;
    char
        *filename;
    size_t            /* TODO: is size_t large enough for 64 bit file systems? */
        data_size;
#ifdef USE_MMAP
    int
        fd;
#elif defined _WIN32
    HANDLE
        hfile,
        hfilemap;
#else
    /* TODO: Nothing here? */
#endif

    int
        status;
};




#ifdef USE_MMAP

/* inspired by an example from http://beej.us/guide/bgipc/output/html/multipage/mmap.html
 * and advice from http://stackoverflow.com/questions/7222164/mmap-an-entire-large-file */

int memory_map_file (GRIM g, char *pomfilename) {
    int fd;
    void *data;
    struct stat sb;
    char *merged_filename;
    int pagesize, mapsize, pages;

    /* Filename relative to pomfile path */
    merged_filename = pom_absolute_filename (pomfilename, g->filename);
    if (0==merged_filename)
        return 1;

    /* open file and find its size */
    fd = open(merged_filename, O_RDONLY);
    free (merged_filename);
    if (0==fd)
        return 2;
    fstat(fd, &sb);
    pagesize = sysconf(_SC_PAGE_SIZE);
    pages = 1 + sb.st_size / pagesize;
    mapsize = pages * pagesize;

    /* attempt to memory map entire file as read only, shared  */
    data = mmap(0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (MAP_FAILED==data) {
        close (fd);
        return 3;
        /* TODO: attempt to allocate real memory and read entire file instead */
    }

    /* it worked - fill in relevant parts of the GRIM object */
    g->fd = fd;
    g->data = data;
    g->data_size = mapsize;
    return 0;
}

#elif defined _WIN32

int memory_map_file (GRIM g, char *pomfilename) {
    char *merged_filename;

    /* Filename relative to pomfile path */
    merged_filename = pom_absolute_filename (pomfilename, g->filename);
    if (0==merged_filename)
        return 1;

    /* oh boy, WIN32 memory mapping is a PITA
     * (although nice we do not need to stat the file to get its size) */
    g->hfile = CreateFile (
            merged_filename,
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL                                       );

    free (merged_filename);
    if (INVALID_HANDLE_VALUE==g->hfile)
        return 2;

    g->hfilemap = CreateFileMapping (g->hfile, NULL, PAGE_READONLY, 0, 0, NULL);
    /* TODO: check validity of g->hfilemap */
    g->data = MapViewOfFile (g->hfilemap, FILE_MAP_READ, 0, 0, 0);
    if (0==g->data)
        return 3;

    /* everything went smooth! */
    return 0;
}
#else
    /* TODO: read entire file into g->data? */
#endif


/*
 * grim_northing, grim_easting, grim_row, grim_column all use affine transformations
 * to transform between world coordinates and grid coordinates.
 *
 * These API entries are inspired by the ESRI world file explanation given on
 * wikipedia: http://en.wikipedia.org/wiki/World_file, and are plain macro wrappers
 * for the affine projection stuff in affine.h
 *
 */


/* define as functions for calls external to grim.c */
double grim_northing (const GRIM g, double row, double column) {
    return affine_northing (g->affine, row, column);
}
double grim_easting (const GRIM g, double row, double column) {
    return affine_easting  (g->affine, row, column);
}
double grim_row (const GRIM g, double northing, double easting) {
    return affine_row      (g->affine, northing, easting);
}
double grim_column (const GRIM g, double northing, double easting) {
    return affine_column   (g->affine, northing, easting);
}

	

/* premature optimization: define as macros for slightly faster grim.c-internal use */
#define grim_northing(g, row, column)     affine_northing (g->affine, row, column)
#define grim_easting(g, row, column)      affine_easting  (g->affine, row, column)

#define grim_row(g, northing, easting)    affine_row      (g->affine, northing, easting)
#define grim_column(g, northing, easting) affine_column   (g->affine, northing, easting)




GRIM grim_close (GRIM g) {
    if (g->file)
        fclose (g->file);

#ifdef USE_MMAP
    assert (0==munmap (g->data, g->data_size));
#elif defined _WIN32
    if (g->data)     UnmapViewOfFile (g->data);
    if (g->hfilemap) CloseHandle (g->hfilemap);
    if (g->hfile)    CloseHandle (g->hfile);
#else
/* TODO
   if (g->data)
        free (g->data);  */
#endif

	free (g);
	return 0;
}










/*
#------------------------------------------------------------------
# grim_open
#------------------------------------------------------------------

grim_open is based on pomgridopen (for bobs described by poms)
from boogrid.c. It does a fair amount of set up of the GRIM object
elements related to the grid format.
*
* TODO: check that all needed settings really are set

NOTE:

The GRIM affine transformation follows the conventions of the

ESRI WORLD FILE format: origin is at the center of the
*upper* left pixel.

The ESRI ASCII GRID format, however, defines the origin as the
center of the *lower* left pixel

Feel free to find this either amusing or confusing.

#------------------------------------------------------------------
*/



GRIM grim_open (char *pomfilename) {
    GRIM g;
	POM *p;
    int res;
    if (0==pomfilename)
        return 0;

    p = pomread (pomfilename);
    if (0==p){
	lord_error(0,LORD("Failed to read pomfile: %s"),pomfilename);
        return 0;
    }

    g = calloc (1, sizeof(struct GRIM));
    if (0==g)
		return (GRIM) pom_free (p);
    g->pom = p;

    /* Extract the grid header information from the pom */
    g->ncols     = pomtod (p, "ncols");
    g->nrows     = pomtod (p, "nrows");
    g->xllcenter = pomtod (p, "xllcenter");
    g->yllcenter = pomtod (p, "yllcenter");
    g->xcellsize = pomtod (p, "xcellsize");
    g->ycellsize = pomtod (p, "ycellsize");
    g->nodata    = pomtod (p, "nodata");
    g->filename  = pomval (p, "filename");

    g->offset    = pomtod (p, "offset");
    g->stride    = pomtod (p, "stride");
    g->channels  = pomtod (p, "channels");

    g->affine[0] = g->xcellsize;
    g->affine[1] = 0;
    g->affine[2] = g->xllcenter;

    g->affine[3] = 0;
    g->affine[4] = - g->ycellsize;
    g->affine[5] = g->yllcenter + (g->nrows - 1) * g->ycellsize;

    if (0!=(res=memory_map_file (g, pomfilename))){
	lord_error(res,LORD("Memory mapping failed..."));
        return grim_close (g);
    }

    return g;
}




/* offset from start of file */
#define GRIM_POINTER_TO(g, row, column, channel) \
    ((float *)  ( ((unsigned char *)(g->data)) + \
                   g->offset +          \
                   (row)*g->stride +      \
                   ((column)*g->channels + (channel)) * sizeof(float)))









/* grim_values_workhorse: interpolate in a multichannel grid. */
static int grim_values_workhorse (const GRIM g, double northing, double easting, size_t extrapolation_method, double *record, size_t first_channel, size_t n_channels) {
    double r, c, rr, cc, dr, dc;
    float *ul, *ur, *ll, *lr;
    double A, B, C, D, AC, BD;
    size_t i, j, rowmax, colmax, n;
    int di = 1, dj = 1, ret = 0;

    r = grim_row (g, northing, easting);
    c = grim_column (g, northing, easting);

    /* integral and fractional part of row and column id */
    rr = floor (r);
    cc = floor (c);
    dr = r - rr;
    dc = c - cc;

    /* maximum allowed row, col numbers make decision logic below a bit more readable */
    rowmax = g->nrows - 1;
    colmax = g->ncols - 1;

    /* TODO: better care for the "=" case */

    /* prepare for cheapskate extrapolation by clamping to 0, rowmax, etc. */
    if (rr <  0)       {ret = 1;  dr = 0; rr = 0;      di = 0; goto column;}
    if (rr >= rowmax)  {ret = 1;  dr = 0; rr = rowmax; di = 0; goto column;}
    column:
    if (cc <  0)       {ret += 2; dc = 0; cc = 0;      dj = 0; goto done;}
    if (cc >= colmax)  {ret += 2; dc = 0; cc = colmax; dj = 0; goto done;}
    done:

    /* return a nodata record if out of bounds and extrapolation not wanted */
    if ((extrapolation_method == 0) && (ret != 0)) {
        for (i = 0;  i < g->channels;  i++)
            record[i] = g->nodata;
        return ret;
    }

    /* i,j becomes upper left corner of cell */
    i  =  rr;
    j  =  cc;

    /* obtain one array for each corner of the cell */
    ul = GRIM_POINTER_TO (g, i,    j,    0);
    ur = GRIM_POINTER_TO (g, i,    j+dj, 0);
    ll = GRIM_POINTER_TO (g, i+di, j,    0);
    lr = GRIM_POINTER_TO (g, i+di, j+dj, 0);

    /* interpolate each channel in turn */
    for (i = first_channel; i < first_channel + n_channels; i++) {
        A = ul[i];    C = ur[i];
        B = ll[i];    D = lr[i];

        /* fill in nodata cyclically from anticlockwise neighbour */
        for (n = 0;  n < 4;  n++) {
            if (A==g->nodata) A = B;
            if (B==g->nodata) B = D;
            if (D==g->nodata) D = C;
            if (C==g->nodata) C = A;
        }

        if (A==g->nodata) {
            record[i-first_channel] = g->nodata;
            continue;
        }


        AC = dc * C + (1-dc) * A;
        BD = dc * D + (1-dc) * B;

        record[i-first_channel] = dr * BD + (1-dr) * AC;
    }
    return ret;
}





/* grim_values: interpolate in a multichannel grid. */
int grim_values (const GRIM g, double northing, double easting, size_t extrapolation_method, double *record) {
    return grim_values_workhorse (g, northing, easting, extrapolation_method, record, 0, g->channels);
}
double grim_value (const GRIM g, double northing, double easting, size_t channel, size_t extrapolation_method) {
    double v;
    grim_values_workhorse (g, northing, easting, extrapolation_method, &v, channel, 1);
    return v;
}





/* allocate a buffer large enough to hold n records from the single- or multi channel GRIM g */
double *grim_allocate_record_buffer (const GRIM g, size_t n) {
    return calloc (g->channels * n, sizeof (double));
}
void grim_free_record_buffer (double *p) {
    free (p);
}


size_t grim_channels (const GRIM g) {
    return g->channels;
}

char *grim_filename(const GRIM g){
	return g->filename;
}

char *grim_proj(const GRIM g){
	return pomval(g->pom, "proj_mlb");
}

char *grim_h_mlb(const GRIM g){
	return pomval(g->pom, "h_mlb");
}

double grim_rows(const GRIM g){
	return g->nrows;
}

double grim_columns(const GRIM g){
	return g->ncols;
}

#ifdef TEST_GRIM
const char pomfilecontents[] = {
   "# comment\n\n#interleaved by blanks\n"
   "    filename:          dkgeoid12a.bin\n"
   "    nrows:                        401\n"
   "    ncols:                        601\n"
   "    xllcenter:                      7\n"
   "    yllcenter:                     54\n"
   "    xcellsize:       0.01666666666667\n"
   "    ycellsize:                   0.01\n"
   "    nodata:                      9999\n"
   "    refframe:   utm32Heuref89_h_dvr90\n"
   "    offset:                        64\n"
   "    stride:                      3208\n"
   "    channels:                       1\n"
};



/*
 * Three rows, three columns, 8 channels
 * +--+--+
 * |  |  |
 * +--+--+
 * |  |  |
 * +--+--+
 *
 */


float eight_channel_grid[] = {
    0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7,
    1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7,
    2.0, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7,

    3.0, 3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7,
    4.0, 4.1, 4.2, 4.3, 4.4, 4.5, 4.6, 4.7,
    5.0, 5.1, 5.2, 5.3, 5.4, 5.5, 5.6, 5.7,

    6.0, 6.1, 6.2, 6.3, 6.4, 6.5, 6.6, 6.7,
    7.0, 7.1, 7.2, 7.3, 7.4, 7.5, 7.6, 7.7,
    8.0, 8.1, 8.2, 8.3, 8.4, 8.5, 8.6, 8.7,
};



const char eight_channel_pom[] = {
   "    filename:           8chtest.grd\n"
   "    nrows:                        3\n"
   "    ncols:                        3\n"
   "    xllcenter:                    0\n"
   "    yllcenter:                    0\n"
   "    xcellsize:                    1\n"
   "    ycellsize:                    1\n"
   "    nodata:                    9999\n"
   "    refframe: utm32Heuref89_h_dvr90\n"
   "    offset:                      64\n"
   "    stride:                      96\n"
   "    channels:                     8\n"
};





void create_test_files (void) {
   FILE *f;
   POM *p;
   unsigned int i;


   f = fopen("8chtest.pom", "wt");
   fwrite(eight_channel_pom, sizeof(eight_channel_pom), 1, f);
   fclose(f);

   p = pomread("8chtest.pom");
   for (i=0; i<p->top; i++)
       puts(p->d[i]);
   pom_free (p);

   /* create 8chtest.grd */
   f = fopen("8chtest.grd", "wb");
   fwrite ("8CHANNELbinary\r\n1234567 1234567 1234567 1234567 1234567 1234567\0", 64, 1, f);
   fwrite (eight_channel_grid, sizeof(float), 9*8, f);
   fclose(f);
}

void remove_test_files (void) {
   remove("8chtest.pom");
   remove("8chtest.grd");
}



int test_affine_functions (GRIM g) {
    double r;
    /* Test affine functions */
    printf ("AFFINE %7.3f %7.3f %7.3f\n", g->affine[0], g->affine[1], g->affine[2]);
    printf ("AFFINE %7.3f %7.3f %7.3f\n", g->affine[3], g->affine[4], g->affine[5]);

    r = grim_row (g, 0, 0);          printf ("row(0,0) = %f\n", r);      if (r != 2) return 1;
    r = grim_row (g, 0, 1);          printf ("row(0,1) = %f\n", r);      if (r != 2) return 2;
    r = grim_row (g, 1, 1);          printf ("row(1,1) = %f\n", r);      if (r != 1) return 3;

    r = grim_column (g, 1, 0);       printf ("col(1,0)   = %f\n", r);    if (r != 0)   return 4;
    r = grim_column (g, 1, 0.5);     printf ("col(1,0.5) = %f\n", r);    if (r != 0.5) return 5;
    r = grim_column (g, 1, 1);       printf ("col(1,1)   = %f\n", r);    if (r != 1)   return 6;

    r = grim_easting (g, 2, 2);      printf ("easting(2,2) = %f\n", r);  if (r != 2) return 7;
    r = grim_easting (g, 0, 0);      printf ("easting(0,0) = %f\n", r);  if (r != 0) return 8;

    r = grim_northing (g, 0, 0);     printf ("northing(0,0) = %f\n", r); if (r != 2) return 9;
    r = grim_northing (g, 2,2);      printf ("northing(2,2) = %f\n", r); if (r != 0) return 10;
    return 0;
}

void test_data_buffer_functions (GRIM g) {
    printf ("test_data_buffer_functions\n");
    printf ("    g->data:           %p\n",       (void *) g->data);
    printf ("    g->data_size:      %d\n", (int) g->data_size);
    printf ("    g->offset:         %d\n", (int) g->offset);
    printf ("    g->stride:         %d\n", (int) g->stride);
    printf ("    g->channels:       %d\n", (int) g->channels);
    printf ("    computed offset:   %d\n", (int)((char *) GRIM_POINTER_TO(g, 0,0,0) - (char *) g->data));
    printf ("    data start:        %p\n", (void *) GRIM_POINTER_TO(g, 0,0,0));
    printf ("    data start, ch 7:  %p\n", (void *) GRIM_POINTER_TO(g, 0,0,7));
    printf ("    second row start:  %p\n", (void *) GRIM_POINTER_TO(g, 1,0,0));
    printf ("OK\n");
}



#include <assert.h>
int main(void){
   GRIM g;
   size_t i, n;
   double v, *w;

   create_test_files ();

   g = grim_open ("./8chtest.pom");
   if (0==g)
       exit (puts("aaargh"));

   assert (0==test_affine_functions (g));
   test_data_buffer_functions (g);


   /* test reading of a single specific channel from a record */
   v = grim_value (g, 2, 0, 0, 0);       printf ("v1 = %g\n", v);
   v = grim_value (g, 1.9, 1.3, 7, 0);   printf ("v7 = %g\n", v);

   /* test reading of a full multi channel record */

   /* make room for 1 record. The number of channels is specified by the GRIM g */
   w = grim_allocate_record_buffer (g, 1);
    puts ("0,0");
   /* read the interpolated record */
   grim_values (g, 0, 0, 1, w);

   /* first now, we need to worry about how many channels are really in the record */
   n = grim_channels (g);
   for (i = 0; i < n; i++)
       printf ("w[%d] = %g\n", (int)i, w[i]);

    puts ("-5,1");
   grim_values (g, -5, 1, 1, w);
   for (i = 0; i < n; i++)
       printf ("w[%d] = %g\n", (int)i, w[i]);

    puts ("0,0,2..4");
   grim_values_workhorse (g, 0, 0, 1, w, 2,3);
   for (i = 0; i < 3; i++)
       printf ("w[%d] = %g\n", (int)i, w[i]);

    puts ("-5,1");
   grim_values (g, -5, 1, 0, w);
   for (i = 0; i < n; i++)
       printf ("w[%d] = %g\n", (int)i, w[i]);

    puts ("1,1");
   grim_values (g, 1, 1, 0, w);
   for (i = 0; i < n; i++)
       printf ("w[%d] = %g\n", (int)i, w[i]);


   /* kleen op ann corregdt ze speeling */
   grim_close (g);
   free (w);
   remove_test_files ();
   return 0;
}
#endif
/* end of test code */
