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

#if !defined(H_PROJ4_TO_MLB)
#define      H_PROJ4_TO_MLB
#include    "metadata.h"

 struct proj4_entry{
        char proj[64];
        char datum[64];
        char vdatum[64];
        char ellps[64];
        char units[16];
        char nadgrids[64];
        double towgs84[7];
        int n_params;
        int zone;
        double x_0;
        double y_0;
        double lat_0;
        double lat_ts;
        double lon_0;
        double k;
        double a;
        double b;
 };
 
 
         
         
 typedef struct proj4_entry proj4_entry;

 
 /*Remember to free the returned pointer*/
 proj4_entry *parse_proj4(char *proj4_text);
 
 int proj4_to_mlb(char *proj4_text, char *mlb, def_data *data);
 
 /* setter function for flag to allow kms-datum extensions */
 void set_kms_datums_allowed(int is_allowed);
 int mlb_to_proj4(char *mlb, char *out, int buf_len, def_data *data);
 
 /*normalise datum (use specific etrs89 alias)*/
 void normalise_datum(char *datum);
 
 #endif

