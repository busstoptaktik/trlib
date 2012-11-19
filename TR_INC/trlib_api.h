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

#define TR_OK 0
#define TR_LABEL_ERROR 1
#define TR_ERROR 2
#define TR_ALLOCATION_ERROR 3

#pragma once
#if defined __cplusplus
#define KMSTR_API extern "C" 
#else 
#define KMSTR_API
#endif 

KMSTR_API int   TR_InitLibrary(char *path);
KMSTR_API int   TR_SetGeoidDir(char *path);
KMSTR_API int   TR_GetLastError(void);
KMSTR_API void TR_GetVersion(char *buffer, int bufferlength);
KMSTR_API int   TR_GetEsriText(char *label_in, char *wkt_out);
KMSTR_API void TR_TerminateLibrary(void);
KMSTR_API void TR_TerminateThread(void);
KMSTR_API void TR_AllowUnsafeTransformations(void);
KMSTR_API void TR_ForbidUnsafeTransformations(void);
KMSTR_API int TR_GetLocalGeometry(char *mlb, double x, double y, double *m, double *c);
typedef struct  TR TR;
KMSTR_API void TR_GeoidInfo(TR *tr);
KMSTR_API TR  *TR_Open (char *label_in, char *label_out, char *geoid_name);
KMSTR_API  int  TR_Transform(TR *tr, double *X, double *Y, double *Z, int n);
KMSTR_API int   TR_InverseTransform(TR *tr, double *X, double *Y, double *Z, int n);
KMSTR_API int   TR_Transform2(TR *tr, double *X_in, double *Y_in, double *Z_in, double *X_out, double *Y_out, double *Z_out, int n);
KMSTR_API int   TR_InverseTransform2(TR *tr, double *X_in, double *Y_in, double *Z_in, double *X_out, double *Y_out, double *Z_out, int n);
KMSTR_API int   TR_TransformPoint(TR *tr, double X_in, double Y_in, double Z_in, double *X_out, double *Y_out, double *Z_out);
KMSTR_API int   TR_InverseTransformPoint(TR *tr, double X_in, double Y_in, double Z_in, double *X_out, double *Y_out, double *Z_out);
KMSTR_API void TR_Close (TR *tr);
KMSTR_API  int   TR_Stream(TR *tr, FILE *f_in, FILE *f_out, int n);


#endif
