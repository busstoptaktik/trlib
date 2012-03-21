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
 
/* Intern API header som definerer TR-objektet, thokn, simlk, sep. 2011 */
#include <stdio.h>
#include "geo_lab.h"
#include "geoid_d.h"
struct TR {
    union geo_lab *plab_in, *plab_out;
    struct mgde_str *geoid_pt;
    char geoid_name[FILENAME_MAX];
    int ngeoids;
    int close_table;
    int use_geoids;
    int   err;
};

struct XYZ {
    double x, y, z;
    int err;
}; 
/* 'Internal' functions not meant to be exposed in API */
int TR_GeoidTable(struct TR*);
int TR_IsMainThread(void);
int TR_IsThreadSafe(union geo_lab*, union geo_lab*);
int TR_tr(union geo_lab* ,union geo_lab*, double*, double*, double*, double*, double*,double*, int , int , struct mgde_str*); 