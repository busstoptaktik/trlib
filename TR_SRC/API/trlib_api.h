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
 
/* API header til eksterne brugere og til internt 'sanity' check, simlk, thokn sep. 2011 */

/*
Vi kan overveje om returmeddelelser skal være som i LH's API.....
enum KMSTR_Error
{
	KMSTR_OK = 0,  // No Error
	KMSTR_LABELERROR = 1, // Invalid input or output label
	KMSTR_TRANSFORMATIONERROR =2,  // Transformation failed
	KMSTR_EXCEPTION = 3, // Caught exception 
	KMSTR_GEOIDINITFAILED = 4, // Failed to initialise geoid library
	KMSTR_GDTRANSLOADFAILED = 5, // Failed to load GDTRANS
	KMSTR_GEOIDSALREADYINITIALISED = 6, // Geoid library has already been initialized 
}; */
#define TR_OK 0
#define LABEL_ERROR 1
#define TR_ERROR 2
#define TR_ALLOCATION_EXCEPTION 3

#pragma once
#if defined __cplusplus
#define KMSTR_API extern "C" 
#else 
#define KMSTR_API
#endif 
KMSTR_API int Transform(char *label_in, char *label_out,double *X,double *Y,double *Z,int npoints);
KMSTR_API int InitLibrary(char *path);
KMSTR_API void GetVersion(char *buffer, int bufferlength);
KMSTR_API int IsGeoidTableInitialised(void);
KMSTR_API int GetEsriText(char *label_in, char *file_name);
typedef struct TR TR;
KMSTR_API TR *tropen (char *label_in, char *label_out);
KMSTR_API  int tr(TR *tr, double *X, double *Y, double *Z, int n);
KMSTR_API void trclose (TR *tr);
KMSTR_API  int trstream(TR *tr, FILE *f_in, FILE *f_out, int n);
KMSTR_API void TerminateLibrary(void);


