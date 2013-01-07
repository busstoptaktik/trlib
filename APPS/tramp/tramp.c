/*
 * Copyright (c) 2012, National Survey and Cadastre, Denmark
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
 

/****************************************************************************

    N A M E

    tramp - TRAnsverse Mercator Projections.
    


    S Y N O P S I S

	tramp [-u <zone>] [-d <zone>] [-k zone] [-ivrR] [-o <outfile>] [-f <output format>] [-F <input format>]  [<infile>...|<coord1> <coord2>]


    D E S C R I P T I O N

    READS  geographical coordinates from file, stdin or command line
    WRITES various flavours of transverse mercator coordinates to file or stdout
    OR     vice versa

    O P T I O N S

    -u <zone> Transform to or from UTM zone <zone>.
    -d <zone> Transform to or from DKTM zone <zone>.
    -k <zone> Transform to or from Kp2000 zone <zone>. (-k not implemented yet)
    
    For options -u,-d, and -k, zone may be set to 0 for automatic selection,
    although this is highly discouraged.

    -i Inverse transformation: transform from grid coordinates to geographic.
    -r Input coordinates are reversed (longitude first)
    -R Output coordinates requested in reverse order (easting first)

    Default is to transform geographic coordinates to UTM zone 32.


    R E T U R N
    
    0 on success, non-0 on failure


    B U G S

    Only does rudimentary checks.

     
    R A T I O N A L E
    
    Written as an "easy way into trlib" (only needs the five c-files
    clenshaw.c, gatg.c, set_utm.c, utm_t_g.c, v_red.c from trlib).
    
    Also intended as a demo of a very useful way of handling command line
    options and compile time defaults, especially wrt. input and output
    files and record formats.


    E X A M P L E S

    1. Back and forth with coordinates given directly on the command line
    
    $ tramp  -i 6173745 727952
      55.655959834  12.623722618
    $ tramp 55.655959834 12.623722618
      6173745.0000   727952.0000


    2. Coordinates from stdin, projection written to a file,
       then inverse projection from stdin back to stdout

    $ tramp -o pap
      55.655959834  12.623722618
    $ cat pap
      6173745.0000   727952.0000
    $ tramp -i <pap
      55.655959834  12.623722618


    C O M P I L A T I O N

    gcc -W -Wall -pedantic -o tramp tramp.c clenshaw.c gatg.c set_utm.c utm_t_g.c v_red.c


    H I S T O R Y

    Thomas Knudsen, thokn@kms.dk,  2012-10-08

****************************************************************************/





#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>


/* trlib headers */
#include "proj_c.h"
#include "utm_t_g.h"
#include "set_utm.h"





/* This is probably too cute for real men - but still useful */
#define  decode(optstr, optchar, argc, argv) \
    while ((optchar = getopt (argc, argv, optstr))!=-1) switch (optchar)

static char _dash[] = {"-"};
static char *dash = _dash;

/* input format is simplistic */
static char default_input_format[] = {"%lg %lg\n"};

/* output format differs between linear and angular units */
static char default_output_format[] = {"%13.4f %13.4f\n"};         /* [m] */
static char default_inverse_output_format[] = {"%13.9f %13.9f\n"}; /* [deg] */

/* TODO: make help message more helpful! */
static char help_message[] = {
    "Usage: tramp [OPTION]...  [FILE | COORDINATE PAIR]...\n"
    "Do various transverse mercator projections (UTM, DKTM, Kp2000) on\n"
    "coordinates given on the command line, FILE or on standard input.\n"
    "\n"
    "\n"
    "Projection selection options:\n"
    "\n"
    "  -u ZONE    Transform to or from UTM    zone ZONE\n"
    "  -d ZONE    Transform to or from DKTM   zone ZONE\n"
    "  -k ZONE    Transform to or from Kp2000 zone ZONE (not implemented)\n"
    "  -i         Inverse transformation (N,E to latitude, longitude)\n"
    "\n"
    "Format and coordinate order options:\n"
    "\n"
    "  -f FORMAT  Specify record format for output data\n"
    "  -F FORMAT  Specify record format for input data\n"
    "  -r         Input coordinates given in reverse order (longitude first)\n"
    "  -R         Output requested in reverse order (easting first)\n"
    "\n"
    "  Note: DKTM conventionally use reverse order (E,N). This is not yet\n"
    "  implemented in tramp.\n"
    "\n"
    "Other options:\n"
    "\n"
    "  -h         Show this help text  \n"
    "  -t         Run the simplistic internal test suite\n"
    "  -v         Set verbose mode. Repeat option for more verbosity\n"
    "\n"
    "\n"
    "The default operation is to transform from geographic to UTM zone 32.\n"
    "Exit status is 0 if OK, 1 if minor problems, 2 if serious trouble.\n"
    "\n"
    "Report bugs through https://bitbucket.org/KMS/trlib\n"
};





/* self test funcs. Placed at the end of this file */
double expect (double *from, double *expected, struct proj_const utg);
int internal_test (void);









/*****************************************************************************/
int main(int argc, char **argv) {
/*****************************************************************************/
    char *fnin     =  dash;
    char *fnout    =  dash;
    char *outfmt   =  0;
    char *infmt    =  default_input_format;
    
    int res;
    
    int inverse = 0, reverse_input = 0, reverse_output = 0, zone = 0, oneshot = 0, verbosity = 0;
    double coord1 = 0, coord2 = 0, outcoord1 = 0, outcoord2 = 0;
    

    FILE *fout    = stdout;
    FILE *fin     = stdin;

    int optchar;
	struct proj_const utg;
	
    /* default to UTM zone 32 on the GRS80 ellipsoid */
	utg.a     = 6378137.0;
	utg.f     = 1.0/298.257222101;
    utg.B0    = 0;                /* Origin latitude */
    utg.L0    = 9*M_PI/180;       /* Origin longitude */
    utg.N0    = 0;                /* False northing */
    utg.E0    = 500000;           /* False easting */
    utg.scale = 0.9996;           /* Central scale of the projection */

    set_utm(&utg);
    


    decode ("f:F:o:u:d:k:ivth", optchar, argc, argv) {
        case 'f': /* output format */
            outfmt = optarg;
            break;

        case 'F': /* input format */
            infmt = optarg;
            break;

        case 'o': /* output file name */
            fnout = optarg;
            fout  = fopen(fnout, "wt");
            if (0==fout)
                exit(printf ("Cannot open output file %s - bye\n", fnout));
            break;
            
        case 'i': /* inverse transformation */
            inverse = 1;
            break;
        case 't':
            internal_test();
            exit (0);
            
        case 'u': /* utm */
            if (1!=sscanf (optarg, "%d", &zone))
                exit(printf ("Cannot parse [%s] as an utm zone id - bye\n", optarg));
            if ((zone < 1) || (zone > 60))
                exit(printf ("Illegal utm zone: [%s] - bye\n", optarg));
            utg.B0    = 0;                        /* Origin latitude */
            utg.N0    = 0;                        /* Origin northing */
            utg.L0    = (-183 + 6*zone)*M_PI/180; /* Origin longitude */
            utg.E0    = 500000;                   /* Origin easting */
            utg.scale = 0.9996;                   /* Central scale of the projection */
            set_utm(&utg);
            break;
            
        case 'd': /* dktm */
            /* TODO: handle dktm E,N convention */
            if (1!=sscanf (optarg, "%d", &zone))
                exit(printf ("Cannot parse [%s] as a dktm zone id - bye\n", optarg));
            if ((zone < 1) || (zone > 4))
                exit(printf ("Illegal dktm zone: [%s] - bye\n", optarg));
                
            /* These parameters are identical for all 4 DKTM zones */    
            utg.B0    = 0;         /* Origin latitude */
            utg.N0    = -5000000;  /* False  northing */
            /* the scale is identical for zones 1-3 */
            utg.scale = 0.99998;       /* Central scale of the projection */

            /* L0 and E0 depend on the zone */
            switch (zone) {
            case 1:
                utg.L0    = 9*M_PI/180;     /* Origin longitude */
                utg.E0    = 200000;         /* False  easting */
                break;
            case 2:
                utg.L0    = 10*M_PI/180;    /* Origin longitude */
                utg.E0    = 400000;         /* False  easting */
                break;
            case 3:
                utg.L0    = 11.75*M_PI/180; /* Origin longitude */
                utg.E0    = 600000;         /* False  easting */
                break;
            case 4:
                utg.L0    = 15*M_PI/180;    /* Origin longitude */
                utg.E0    = 800000;         /* False  easting */
                utg.scale = 1.0;            /* Central scale of the projection */
                break;
            }
            set_utm(&utg);
            break;
            
        case 'v': /* verbosity */
            verbosity++;
            break;

        case 'h': /* print help message to stdout */
            puts(help_message);
            exit(EXIT_SUCCESS);
    }

    
    /* Set output format now, since we know whether doing forward or inverse */
    if (0==outfmt)
        outfmt = inverse? default_inverse_output_format: default_output_format;
        
    /* should we transform just one pair of coordinates from the command line? */
    oneshot = 0; 
    while (optind + 2 == argc) {
		if (1 != sscanf (argv[optind], "%lg", &coord1))
		    break;
		if (1 != sscanf (argv[optind+1], "%lg", &coord2))
		    break;
		oneshot = 1;
		break;
	}
    

    /* prepare to read input data either from stdin or from specified files (TODO: support multiple input files)  */
    if (!oneshot) {
        if (optind >= argc)
            fin = stdin;
        else {
            fnin = argv[optind];
            fin  = fopen(fnin, "rt");
            if (0==fin)
                exit (printf ("Cannot open input file %s - bye\n", fnin));
        }
    }




    /* loop over all data points */
    while (1) {
		if ((!oneshot) && feof (fin))
		    break;
            
        /* silently ignore unreadable lines */            
        if ((!oneshot) && (2!=fscanf (fin, infmt, &coord1, &coord2)))
            continue;
        if (reverse_input) {
            double swap = coord1;
            coord1 = coord2;
            coord2 = swap;
        }
        if (!inverse) {
            coord1 *= (M_PI/180);
            coord2 *= (M_PI/180);
        }
        
        res = utm_t_g(&utg, inverse? 1: -1, coord1, coord2, &outcoord1, &outcoord2);
        if (reverse_output) {
            double swap = outcoord1;
            outcoord1 = outcoord2;
            outcoord2 = swap;
        }
        
        /* prepare angular units for output in degrees */
        if (inverse) {
            outcoord1 *= (180/M_PI);
            outcoord2 *= (180/M_PI);
        }
        
        fprintf(fout, outfmt , outcoord1, outcoord2);
        if (oneshot)
            break;
    }

    return res;
}
















/*---------------------------------------------------------------------
  Self test using test data from http://valdemar.kms.dk/trf/
  -----------------------------------------------------------
  Date/time       2012 10 08, 18.49
  Transformation fra;
    # utm32_etrs89    6 173 745.00 m        727 952.00 m    (N,E)
    # DK_dktm3          654 995.38 m      1 170 480.85 m    (E,N)
    # utm33_etrs89    6 170 350.26 m        350 498.48 m    (N,E)
    # geo_etrs89    55 39 21.45540 sx   12 37 25.40142 sx   (F,L)
---------------------------------------------------------------------*/


double  utm32_etrs89[] = {  6173745.00,   727952.00  };
double  DK_dktm3[]     = {  1170480.85,   654995.38  };
double  utm33_etrs89[] = {  6170350.26,   350498.48  };
double  geo_etrs89[]   = {  55+39.0/60+21.45540/3600,  12+37.0/60+25.40142/3600 };


double expect (double *from, double *expected, struct proj_const utg) {
    int res = 0;
    double lat, lon, n, e, delta_forward, delta_inverse;
    printf ("from        [m]: %12.4f, %12.4f\n", from[0], from[1]);
    printf ("expected  [deg]: %12.9f, %12.9f\n", expected[0], expected[1]);

    /* from projected to geographical coordinates */    
    res += utm_t_g(&utg, 1, from[0], from[1], &lat, &lon);
    printf ("computed  [deg]: %12.9f, %12.9f\n", lat*180/M_PI, lon*180/M_PI);
    
    /* approximate representation of delta in microns (1 deg = 111 km) */
    delta_inverse = hypot (lat*180/M_PI-expected[0], lon*180/M_PI-expected[1])*111000000000.0;
    
    /* from computed geographical coordinates back to the "from" data */    
    res += utm_t_g(&utg, -1, lat, lon, &n, &e);
    printf ("roundtrip   [m]: %13.4f, %13.4f\n", n, e);
    
    /* the roundtrip delta is born in metres */
    delta_forward = hypot (n-from[0], e-from[1])*1000000;
    
    printf ("delta      [um]: %3.1f, %3.1f, %d\n", delta_inverse, delta_forward, res);
    return delta_forward;
}

int internal_test (void) {
    
	struct proj_const utg;
    double delta = 0;
    char banner[] = {
        "-------------------------------------------------\n"
        "  %s\n"
        "-------------------------------------------------\n"
    };
	
    /* test from UTM zone 32 on the GRS80 ellipsoid */
    printf (banner, "UTM zone 32 self test");
	utg.a     = 6378137.0;
	utg.f     = 1.0/298.257222101;
    utg.B0    = 0;                /* Origin latitude */
    utg.N0    = 0;                /* False northing */
    utg.L0    = 9*M_PI/180;       /* Origin longitude */
    utg.E0    = 500000;           /* False easting */
    utg.scale = 0.9996;           /* Central scale of the projection */
    set_utm(&utg);
    delta += expect (utm32_etrs89, geo_etrs89, utg);

    /* test from UTM zone 33 on the GRS80 ellipsoid */
    printf (banner, "UTM zone 33 self test");
    utg.L0    = 15*M_PI/180;       /* Origin longitude */
    set_utm(&utg);
    delta += expect (utm33_etrs89, geo_etrs89, utg);

    /* test from DKTM zone 3 on the GRS80 ellipsoid */
    printf (banner, "DKTM zone 3 self test");
    utg.E0    =  600000;         /* False  easting */
    utg.N0    = -5000000;  /* False  northing */
    utg.scale = 0.99998;       /* Central scale of the projection */
    utg.L0    = 11.75*M_PI/180;       /* Origin longitude */
    set_utm(&utg);
    delta += expect (DK_dktm3, geo_etrs89, utg);
    printf (banner, "end of self test");
    
    return delta > 1000? 1: 0;
}    
