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
 
/* int get_ellipsoid_data(struct coord_lab *plab_in, double *data);*/

#if !defined(H_DOC_DEF_DATA)
#define      H_DOC_DEF_DATA
#include "metadata.h"
int doc_rgn(char *rgn_name, char *descr, int detail, def_data *data);
int doc_prj(char *prj_name, char *descr, char *impl_datum, int *type, int detail, def_data *data);
int doc_dtm(char *dtm_name, char *descr, int detail, def_data *data);
int get_def_numbers(int *n_prj, int *n_dtm, int *n_grs, int *n_rgn, def_data *data);
int doc_grs(char *ell_name, FILE *out, int detail, def_data *data);
void present_data(FILE *fp,def_data *data);

#endif

