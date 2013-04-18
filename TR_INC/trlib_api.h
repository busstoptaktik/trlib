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
#define KMSTR_API extern "C" 
#else 
#define KMSTR_API
#endif 

KMSTR_API int   TR_InitLibrary(char *path);
KMSTR_API int   TR_SetGeoidDir(char *path);
KMSTR_API void TR_GetVersion(char *buffer, int bufferlength);
KMSTR_API int   TR_GetEsriText(char *label_in, char *wkt_out); /* DEPRECATED - should be moved to trlib_intern.h*/
KMSTR_API int TR_ExportLabel(char *mlb, char *out, int foreign_format_code, int buf_len);
KMSTR_API int TR_ImportLabel(char *text, char *mlb_out, int buf_len);
KMSTR_API void TR_TerminateLibrary(void);
KMSTR_API void TR_TerminateThread(void);
KMSTR_API void TR_AllowUnsafeTransformations(void);
KMSTR_API void TR_ForbidUnsafeTransformations(void);
typedef struct  TR TR;
KMSTR_API void TR_GeoidInfo(TR *tr);
KMSTR_API int   TR_GetLocalGeometry(TR *tr, double x, double y, double *m, double *c, int is_proj_out);
KMSTR_API TR  *TR_Open (char *label_in, char *label_out, char *geoid_name);
KMSTR_API int   TR_Insert (TR *tr, char *mlb, int is_out);
KMSTR_API int   TR_Transform(TR *tr, double *X, double *Y, double *Z, int n);
KMSTR_API int   TR_InverseTransform(TR *tr, double *X, double *Y, double *Z, int n);
KMSTR_API int   TR_Transform2(TR *tr, double *X_in, double *Y_in, double *Z_in, double *X_out, double *Y_out, double *Z_out, int n);
KMSTR_API int   TR_InverseTransform2(TR *tr, double *X_in, double *Y_in, double *Z_in, double *X_out, double *Y_out, double *Z_out, int n);
KMSTR_API int   TR_TransformPoint(TR *tr, double X_in, double Y_in, double Z_in, double *X_out, double *Y_out, double *Z_out);
KMSTR_API int   TR_InverseTransformPoint(TR *tr, double X_in, double Y_in, double Z_in, double *X_out, double *Y_out, double *Z_out);
KMSTR_API void TR_Close (TR *tr);
KMSTR_API int   TR_Stream(TR *tr, FILE *f_in, FILE *f_out, int n);

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
KMSTR_API int TR_GetLastError(void);

/* LORD module setter functions below.
* These are NOT thread safe and should only be used from the main thread (which called TR_InitLibrary)
*/

/* Set the maximal number of messages that should be logged - use a negative number if you do not want logging to stop at all */
KMSTR_API void TR_SetLordMaxMessages (int max_messages);

/* Sets the lord call back function  - call with NULL to remove callback functionality*/
KMSTR_API void TR_SetLordCallBack(LORD_CALLBACK fct);

/* Turn the lord modes on or off */
KMSTR_API void TR_SetLordModes(int debug, int info, int warning, int error, int critical);

/* Set the output pointers for each lord_type 
* The user 'owns' the file pointers and is responsible for closing these, when they are not needed anymore.
* Should generally not be mixed with calls to TR_SetLordFile
*/
KMSTR_API void TR_SetLordOutputs(FILE * stream_debug_outside, FILE * stream_info_outside, FILE * stream_warning_outside, FILE * stream_error_outside, FILE * stream_critical_outside);

/* Set the verbosity level for the modes */
KMSTR_API void TR_SetLordVerbosityLevels(int verb_debug, int verb_info, int verb_warning);

/* Sets the lord output file for ALL reporting types. Will close previously associated streams - thus the LORD module will take 'ownership' over the file pointers.
*  Should generally not be mixed with calls to TR_SetLordOutputs
*/
KMSTR_API int TR_SetLordFile(char *fullfilename);


#endif
