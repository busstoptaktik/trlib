/* -- Module GRIM ------------------------------------------------------- */

#if !0xD0C

\summary{GRid Interpolation and Memory mapping}

\author{Thomas Knudsen}
\date{2012-11-13...23}

%-----------------------------------------------------
\SECTION{Synopsis}
%-----------------------------------------------------

    The GRIM module handles memory mapping of grid files, and provides
    a simplistic set of functions for obtaining bilinearly interpolated
    grid values.

    Hence, you may conveniently think of GRIM as an implementation of
    disk based (i.e. persistent) arrays with non-integral indexing.


%-----------------------------------------------------
\SECTION{Lineage, Licence and Copyright}
%-----------------------------------------------------

GRIM is based on the BooGIE grid implementation boogrid.c,
and counting its ancestors back to code written for my masters thesis
(around 1991 - ufgrid.c), and PhD-project (1998 - busgrid.c, grin.c).

Originally, these files were (C) by Thomas Knudsen, and distributed
under the GNU General Public Licence (GPL) v. 2.

Recently (fall 2012), I changed the licence to ISC, for compatibility with trlib.
Hence, this somewhat convoluted copyright notice:

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


%-----------------------------------------------------
\SECTION{Compilation examples}
%-----------------------------------------------------

%. thokn@SIT1130565 ~/2012/Playground

. gcc  -I ../../trlib/TR_INC/ -I .  -std=gnu99 -xc -W -Wall -W -pedantic -o grim grim.c
. gcc  -I ../../trlib/TR_INC/ -I .  -W -Wall -W -pedantic -DTEST_GRIM -DWITH_MMAP -o grim grim.c -lmman
.
. gcc  -I ../../trlib/TR_INC/ -I .  -W -Wall -W -pedantic -DTEST_GRIM -o grim grim.c -lmman

%.    g c c  -std=c99 -xc -W -Wall -W -pedantic -DESRITEST  -o geoideinterpolationstest geoideinterpolationstest.c
%---------------------------------------------------------------------------


%-----------------------------------------------------
\SECTION{Generic example}
%-----------------------------------------------------

\begin{lstlisting}[language=C,breaklines,identifierstyle=,morekeywords={size_t, GRIM, assert}]
#include <stdio.h>      /*  for printf and friends  */
#include <stdlib.h>     /*  for malloc/free         */
#include <ctype.h>      /*  for isspace()           */
#include <math.h>       /*  for HUGE_VAL            */
#include "grim.h"

#include <assert.h>
int main (void) {
    GRIM g;
    size_t i, n;
    double v, r, c, N, E, *w;

    g = grim_open ("./8chtest.pom");
    assert (0!=g);


    /* 1: test reading of a single specific channel from a record */


    /* 1.1: channel 0 at northing 2, easting 0 */
    v = grim_value (g, 2, 0, 0, 0);
    printf ("v1 = %g\n", v);

    /* 1.2: channel 7 at northing 2, easting 0 */
    v = grim_value (g, 1.9, 1.3, 7, 0);
    printf ("v7 = %g\n", v);


    /* 2: test reading of a full multi channel record */


    /* 2.1: make room for 1 record. The number of channels is specified by the GRIM g */
    w = grim_allocate_record_buffer (g, 1);

    /* 2.2: read the interpolated record */
    grim_values (g, 0, 0, 1, w);

    /* 2.3: first now, we need to worry about how many channels are really in the record */
    n = grim_channels (g);
    for (i = 0; i < n; i++)
        printf ("w[%d] = %g\n", (int)i, w[i]);


    /* 3: Test affine functions */


    r = grim_row (g, 0, 0);
    assert (r==2);
    r = grim_row (g, 0, 1);
    assert (r==2);
    r = grim_row (g, 1, 1);
    assert (r==1);

    c = grim_column (g, 1, 0);
    assert (c==0);
    c = grim_column (g, 1, 0.5);
    assert (c==0.5);
    c = grim_column (g, 1, 1);
    assert (c==1);

    E = grim_easting (g, 2, 2);
    assert (E==2)
    E = grim_easting (g, 0, 0);
    assert (E==0);

    N = grim_northing (g, 0, 0);
    assert (N==2);
    N = grim_northing (g, 2,2);
    assert (N==0);


   /* 4: Clean up */


   grim_free_record_buffer (w);
   grim_close (g);
   return 0;
\end{lstlisting}



#endif




/* -- Datatype -----------------------------------------------------------------*/
typedef struct  GRIM  *   GRIM;
#if !0xD0C
%-----------------------------------------------------
\SECTION{Synopsis}
%-----------------------------------------------------
The control object for the GRIM module. Note that
it is designed to be an opaque object: In the header,
only the forward declaration of the struct GRIM is
given. The actual declaration of struct GRIM is
in the grim.c source code, out of sight from application
programs. Hence avoiding accidental tampering with the
delicate contents of the struct GRIM.

Note that the typedef is to a \emph{pointer} to struct GRIM.
This is contrary to the archetypal Posix stdio ``FILE'' typedef,
where a "pointer to FILE" is returned by fopen.
It is, however, in accordance with the zlib compressed i/o
library, where gzopen() returns a gzFile, not a \emph{gzFile *} directly.
(cf. \url{http://www.zlib.net/})


#endif


/* The basic open/close functions */


/* -- Function -----------------------------------------------------------------*/
GRIM grim_open (char *pomfilename);

#if !0xD0C
%-----------------------------------------------------
\SECTION{Synopsis}
%-----------------------------------------------------
Get ready to use grid data described in the pom file
\emph{pomfilename}
\begin{description}
\item [pomfilename] name of pom file. The pom file
      contains the description (\emph{metadata}) for
      the grid file we want to access (its row and
      column counts, its spatial extent, the number
      of channels, etc.)
\end{description}
%-----------------------------------------------------
#endif




/* -- Function -----------------------------------------------------------------*/
GRIM grim_close (GRIM g);
#if !0xD0C
%-----------------------------------------------------
\SECTION{Synopsis}
%-----------------------------------------------------
Clean up everything assoicated with \lstinline!GRIM g!
(i.e. release allocated menory and memory mapping, close open files, etc.)
\begin{description}
\item [g] a GRIM object originally returned by grim_open().
\end{description}
%-----------------------------------------------------
#endif

/* get either one or all channel values from an interpolated point in a multichannel grid */

/* -- Function -----------------------------------------------------------------*/
int grim_values (const GRIM g, double northing, double easting, size_t extrapolation_method, double *record);
#if !0xD0C
%-----------------------------------------------------
\SECTION{Synopsis}
%-----------------------------------------------------
Access the interpolated values for the grid referred to by g,
at a given point of interest,
\begin{description}
\item [g] a GRIM object originally returned by grim_open().
\item [northing] the northing coordinate for the point of interest.
\item [easting]  the easting coordinate for the point of interest.
\item [extrapolation_method] determine how points of interest
       outside the grid coverage should be handled.
       Currently only two rather simplistic methods are supported:
       \begin{description}
       \item [extrapolation_method = 0] return nodata
       \item [extrapolation_method > 0] clamp point of interest to nearest grid boundary value.
       \end{description}
\item [record] a pointer to an array of doubles at least as large as the number
      of channels of the grid represented by \textbf{g}.
      A suitably large array can be obtained from a call to grim_allocate_record_buffer().
\end{description}
%-----------------------------------------------------
#endif

/* -- Function -----------------------------------------------------------------*/
double grim_value (const GRIM g, double northing, double easting, size_t channel, size_t extrapolation_method);
#if !0xD0C
%-----------------------------------------------------
\SECTION{Synopsis}
%-----------------------------------------------------
Return a the value for a single channel of the grid
referred to by g, at a given point of interest.
\begin{description}
\item [g] a GRIM object originally returned by grim_open().
\item [northing] the northing coordinate for the point of interest.
\item [easting]  the easting coordinate for the point of interest.
\item [extrapolation_method] determine how points of interest
       outside the grid coverage should be handled.
       Currently only two rather simplistic methods are supported:
       \begin{description}
       \item [extrapolation_method = 0] return nodata
       \item [extrapolation_method > 0] clamp point of interest to nearest grid boundary value.
       \end{description}
\end{description}
%-----------------------------------------------------
#endif

/* companions to "grim_values()" above: allocate a properly sized buffer */

/* -- Function -----------------------------------------------------------------*/
double * grim_allocate_record_buffer (const GRIM g, size_t n);
#if !0xD0C
%-----------------------------------------------------
\SECTION{Synopsis}
%-----------------------------------------------------
Allocate a buffer large enough to hold \emph{n} instances of
interpolated values for all channels of the grid referred to by g.

Primarily intended for use in connection with \emph{grim_values()},
in which case \emph{n} should be 1.
\begin{description}
\item [g] name of pom file
\item [n] number of records to allocate room for
\end{description}
%-----------------------------------------------------
#endif

/* -- Function -----------------------------------------------------------------*/
void grim_free_record_buffer (double *p);
#if !0xD0C
%-----------------------------------------------------
\SECTION{Synopsis}
%-----------------------------------------------------
Companion to \emph{grim_allocate_record_buffer}: free
the memory allocated. Currently this is a simply a masked
call to the system supplied \emph{free()} function.
\begin{description}
\item [p] pointer to buffer allocated by \emph{grim_allocate_record_buffer}
\end{description}
%-----------------------------------------------------
#endif

/* return the number of channels in the grid */
/* -- Function -----------------------------------------------------------------*/
size_t grim_channels (const GRIM g);
#if !0xD0C
%-----------------------------------------------------
\SECTION{Synopsis}
%-----------------------------------------------------
Return the number of channels in the grid referred to by \emph{g}.
\emph{pomfilename}
\begin{description}
\item [g] a GRIM object returned by \emph{grim_open()}
\end{description}
%-----------------------------------------------------
#endif


/* affine transformations from grid coordinates to world coordinates */
/* -- Function -----------------------------------------------------------------*/
double grim_northing (const GRIM g, double row, double column);
#if !0xD0C
%-----------------------------------------------------
\SECTION{Synopsis}
%-----------------------------------------------------
Return the northing value of a given set of (possibly fractional)
row and column values
\begin{description}
\item [g] the GRIM object for the data set
\item [row] the (possibly fractional) row number
\item [column] the (possibly fractional) column number
\end{description}
%-----------------------------------------------------
#endif

/* -- Function -----------------------------------------------------------------*/
double grim_easting  (const GRIM g, double row, double column);
#if !0xD0C
%-----------------------------------------------------
\SECTION{Synopsis}
%-----------------------------------------------------
Return the easting value of a given set of (possibly fractional)
row and column values
\begin{description}
\item [g] the GRIM object for the data set
\item [row] the (possibly fractional) row number
\item [column] the (possibly fractional) column number
\end{description}
%-----------------------------------------------------
#endif

/* affine transformations from world coordinates to grid coordinates */
/* -- Function -----------------------------------------------------------------*/
double grim_row      (const GRIM g, double northing, double easting);
#if !0xD0C
%-----------------------------------------------------
\SECTION{Synopsis}
%-----------------------------------------------------
Return the (possibly fractional) row value of a given set
of northing and easting values.
\begin{description}
\item [g] the GRIM object for the data set
\item [northing] the northing
\item [column] the easting
\end{description}
%-----------------------------------------------------
#endif

/* -- Function -----------------------------------------------------------------*/
double grim_column   (const GRIM g, double northing, double easting);
#if !0xD0C
%-----------------------------------------------------
\SECTION{Synopsis}
%-----------------------------------------------------
Return the (possibly fractional) column value of a given set
of northing and easting values.
\begin{description}
\item [g] the GRIM object for the data set
\item [northing] the northing
\item [column] the easting
\end{description}
%-----------------------------------------------------
#endif

const char *grim_filename(const GRIM g);
const char *grim_crs(const GRIM g);
const char *grim_field_name(const GRIM g);
GRIM grim_polynomial(double lat_0, double lon_0, double *coefficients, int n_coeff, int channels, int degree, char *mlb);
int grim_polynomial_workhorse(const GRIM g, double northing, double easting, *double record, int channels);
double grim_rows(const GRIM g);
double grim_columns(const GRIM g);



#if !0xD0C
.FILE pre.tex \renewcommand{\reportnumber}{GRIM}

#endif
