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


#if !defined(H_TRLIB_API)
#define      H_TRLIB_API

#include <stdio.h>

/* API header til eksterne brugere og til internt 'sanity' check, simlk, thokn sep. 2011 */

#define TR_OK                  (0)
#define TR_LABEL_ERROR    (1)
#define TR_ERROR             (2)
#define TR_ALLOCATION_ERROR  (3)

/* Defines for export to foreign srs-metadata */

#define TR_FRMT_EPSG  (0)
#define TR_FRMT_PROJ4 (1)
#define TR_FRMT_ESRI_WKT (2)

#if defined __cplusplus
extern "C" {
#endif

/* Initialisation function which will initialise various global resources - call with path to geoid library or empty string if TR_TABDIR env variable is set to point to a geoid library
 * Not thread safe - should be called from 'main' thread before other threads are started
*/
int TR_InitLibrary(char *path);

/* Change the geoid library (and definition files like def_lab.txt) - not thread safe. Should be called from main thread. */
int TR_SetGeoidDir(char *path);

/* Fetch version info. A buffer string of length 128 should suffice. */
void TR_GetVersion(char *buffer, int bufferlength);

/* DEPRECATED - use TR_ExportLabel instead*/
int TR_GetEsriText(char *label_in, char *wkt_out); 

/* Export a mini label to a foreign format defined by a code. Use the predefined macros above. Be sure to supply a sufficiently large buffer depending on the output format*/	
int TR_ExportLabel(char *mlb, char *out, int foreign_format_code, int buf_len);

/* Import a mini label from a foreign format (the ones defined in the macros above). For EPSG definitions prepend the definition with "EPSG:" e.g. "EPSG:25832" */
int TR_ImportLabel(char *text, char *mlb_out, int buf_len);
 
 /* Terminate the library and free various resources.*/
 void TR_TerminateLibrary(void);
 
/*Call to free thread local resources, when a thread performing transformations is not used anymore / destroyed. */
void TR_TerminateThread(void);

/* Call this function to allow transformations which are not thread safe */
void TR_AllowUnsafeTransformations(void);

/* Call this function to disallow transformations which are not thread safe - this is the DEFAULT behaviour */
void TR_ForbidUnsafeTransformations(void);

/* Typedef for transformation objects - basically a container for pointers to input and output projections. Any of these can be NULL. 
*  The object is only valid for use in a transformation if both input and output projection are set.
*/
typedef struct  TR_kms_str TR;

/* Print info on last used geoid to stdout. WARNING: this implementation will change in the future or the function will be deprecated.*/
void TR_GeoidInfo(TR *tr);

/* Get the scale (c) and meridian convergence (m) of a projection. If the flag is_proj_out is 0 / False, the input projection in the TR object is used, else the output projection is used. */
int   TR_GetLocalGeometry(TR *tr, double x, double y, double *m, double *c, int is_proj_out);

/* Open a transformation object specified by minilabels label_in and label_out.
 *  If any of the labels are NULL or "" the corresponding 'slot' is left unused (NULL).
 *  The last parameter geoid_name should typically be left as NULL or "", but can be used to select a special geoid defined in the geoid library. 
 */
 TR  *TR_Open (char *label_in, char *label_out, char *geoid_name);
 
 /*TODO*/
 int   TR_Insert (TR *tr, char *mlb, int is_out);
  /*TODO*/
 int   TR_Transform(TR *tr, double *X, double *Y, double *Z, int n);
  /*TODO*/
 int   TR_InverseTransform(TR *tr, double *X, double *Y, double *Z, int n);
  /*TODO*/
 int   TR_Transform2(TR *tr, double *X_in, double *Y_in, double *Z_in, double *X_out, double *Y_out, double *Z_out, int n);
  /*TODO*/
 int   TR_InverseTransform2(TR *tr, double *X_in, double *Y_in, double *Z_in, double *X_out, double *Y_out, double *Z_out, int n);
  /*TODO*/
 int   TR_TransformPoint(TR *tr, double X_in, double Y_in, double Z_in, double *X_out, double *Y_out, double *Z_out);
  /*TODO*/
 int   TR_InverseTransformPoint(TR *tr, double X_in, double Y_in, double Z_in, double *X_out, double *Y_out, double *Z_out);
  /*TODO*/
 void TR_Close (TR *tr);
  /*TODO*/
 int   TR_Stream(TR *tr, FILE *f_in, FILE *f_out, int n);

/***************************************
**********LORD module functions ********
****************************************/
/* class code to distinguish 'report' type in call_back*/
typedef enum 
{LORD_DEBUG=0, 
LORD_INFO=1, 
LORD_WARNING=2,
LORD_ERROR=3,
LORD_CRITICAL=4} LORD_CLASS;

/*call back definition*/
typedef void( *LORD_CALLBACK )(LORD_CLASS, int , const char *);


/*return last buffered thread local error code from the lord module - this function is thread safe*/
 int TR_GetLastError(void);

/* LORD module setter functions below.
* These are NOT thread safe and should only be used from the main thread (which called TR_InitLibrary)
*/

/* Set the maximal number of messages that should be logged - use a negative number if you do not want logging to stop at all */
 void TR_SetLordMaxMessages (int max_messages);

/* Sets the lord call back function  - call with NULL to remove callback functionality*/
 void TR_SetLordCallBack(LORD_CALLBACK fct);

/* Turn the lord modes on or off */
 void TR_SetLordModes(int debug, int info, int warning, int error, int critical);

/* Set the output pointers for each lord_type 
* The user 'owns' the file pointers and is responsible for closing these, when they are not needed anymore.
* Should generally not be mixed with calls to TR_SetLordFile
*/
 void TR_SetLordOutputs(FILE * stream_debug_outside, FILE * stream_info_outside, FILE * stream_warning_outside, FILE * stream_error_outside, FILE * stream_critical_outside);

/* Set the verbosity level for the modes */
 void TR_SetLordVerbosityLevels(int verb_debug, int verb_info, int verb_warning);

/* Sets the lord output file for ALL reporting types. Will close previously associated streams - thus the LORD module will take 'ownership' over the file pointers.
*  Should generally not be mixed with calls to TR_SetLordOutputs
*/
 int TR_SetLordFile(char *fullfilename);

#if defined __cplusplus
}
#endif

#endif
