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
 
#if !defined(H_TRLIB_INTERN)
#define      H_TRLIB_INTERN

/* Intern API header som definerer TR-objektet, thokn, simlk, sep. 2011 */
#include <stdio.h>
#include "geo_lab.h"
#include "geoid_d.h"

/*Utility macros implemented in order the hide the detailed composition of a PR-object (which may change) */
#define IS_CARTESIC(pr)           ((pr->plab->u_c_lab).cstm==1)
#define IS_GEOGRAPHIC(pr)       ((pr->plab->u_c_lab).cstm==2)
#define IS_TM(pr)                    ((pr->plab->u_c_lab).cstm==3)
#define IS_MERCATOR(pr)          ((pr->plab->u_c_lab).cstm==4)
#define GET_MLB(pr)                 ((pr->plab->u_c_lab).mlb)
#define HAS_HEIGHTS(pr)          ((pr->plab->u_c_lab).h_dtm>200)
#define IS_3D(pr)                     (pr!=NULL && (pr->plab->u_c_lab).ncoord==3)
#define COORD_ORDER(pr)          ((pr->plab->u_c_lab).p_seq)
#define GET_LAT0(pr)                ((pr->plab->u_c_lab).B0)
#define GET_LON0(pr)                ((pr->plab->u_c_lab).L0)
#define GET_X0(pr)                    ((pr->plab->u_c_lab).E0)
#define GET_Y0(pr)                    ((pr->plab->u_c_lab).N0)
#define GET_SCALE(pr)               ((pr->plab->u_c_lab).scale)
/*#define GET_ZONE(pr)                ((pr->plab->u_c_lab).zone) - not stored here as expected (use function) */
#define GET_DTM(pr)                  ((pr->plab->u_c_lab).datum)
#define GET_PDTM(pr)                ((pr->plab->u_c_lab).p_dtm)

/*An internal PR-object (whose implementation might change). Reference counting can easily be implemented by adding a n_references field */
struct PR_kms_str{
   union geo_lab *plab;
   double dgo[4]; /*space to be used by ptg_d for TM-projections. For other projections, the space can be used for other purposes.*/
};

typedef struct PR_kms_str PR;


struct TR_kms_str{
    PR *proj_in, *proj_out;
    struct mgde_str *geoid_pt;
    char geoid_name[FILENAME_MAX];
    int close_table;
    int use_geoids;
    int err;
};

struct XYZ {
    double x, y, z;
    int err;
};



struct PLATE {
    char *plate_model;           /* Name of plate model to use */
    char *intra_plate_model;     /* Name of intra plate model to use */
    int *plm_trf;                /* Plate model used (0->plate model NOT used) */
    int *ipl_trf;                /* Plate model used using: 0->no intraplate, 4->intra plate (iJD=oJD=-10000000.0), 1->iJD, 2->oJD, 3->iJD and oJD */
    double *plm_dt;              /* JD2000 plate model epoch */
    double *ipl_idt;             /* JD2000 intra plate model epoch in */
    double *ipl_odt;             /* JD2000 intra plate model epoch out */
    char *plm_nam;               /* Name of used plate model */
    char *plt_nam;               /* Name of used plate */
    char *ipl_nam;               /* Name of used intra plate */
};

/* 'Internal' functions not meant to be exposed in API 
* Could consider implementing TR *TR_Compose(PR*,PR*) and  void TR_CloseContainer(TR*)  functions for maximal performance and internal use.
* This would not require any reference counting in constructors and desctructors, as long as TR_CloseProjection is not called before TR_CloseContainer.
*/
int TR_GeoidTable(struct TR_kms_str *tr);
int TR_SpecialGeoidTable(struct TR_kms_str *tr, char *geoid_name);
int TR_IsMainThread(void);
int TR_IsThreadSafe(union geo_lab *plab);
int TR_tr(PR* proj_in, PR* proj_out, double* X_in, double* Y_in, double* Z_in, double* X_out, double* Y_out,double* Z_out, int n, int use_geoids, struct mgde_str*); 
int TR_itrf(union geo_lab*, union geo_lab*, double*, double*, double*, double*, double*, double*, int, double*, double*, double*, double*, double*, double*, int, double*, int, struct PLATE* plate_info);
PR *TR_OpenProjection(char *mlb);
void TR_CloseProjection(PR *proj);
void TR_GetGeoidName(struct TR_kms_str *tr,char *name);

#endif

