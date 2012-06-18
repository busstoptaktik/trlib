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
 




#include    <string.h>
#include    <stdio.h>
#include    <sys/stat.h>
#include    "geo_lab.h"
#include    "parse_def_file.h"

short            conv_rgn(
/*______________________*/
short            rgn_nr,
char            *rgn_prfx,
char            *rgn_name
)
{


/* Use preparsed regions. simlk, june 2012 */
	union rgn_un     f_rgn, r_rgn;
	extern def_data *DEF_DATA;
	int n_rgn=0;
	def_rgn *this_rgn;
	(void) strcpy(rgn_name, "Undefined");
	if (DEF_DATA==NULL)
		return (-2);
	if (rgn_nr!=-1){
		/* search for rgn_nr */
		for(n_rgn=0; n_rgn<DEF_DATA->n_rgn; n_rgn++)
		{
			this_rgn=DEF_DATA->regions+(n_rgn);
			strncpy(r_rgn.prfx,this_rgn->rgn_new,3);
			if (rgn_nr==r_rgn.r_nr[0]){
				strncpy(f_rgn.prfx,this_rgn->rgn_old,3);
				strncpy(rgn_prfx,this_rgn->rgn_old,3);
				return f_rgn.r_nr[0];
			}
		}
	return -1;
	}
	/*else search for rgn_prfx */
	for(n_rgn=0; n_rgn<DEF_DATA->n_rgn; n_rgn++)
	{
		this_rgn=DEF_DATA->regions+(n_rgn);
		if (!strcmp(this_rgn->rgn_new,rgn_prfx)){ /*found */
			strncpy(r_rgn.prfx,this_rgn->rgn_new,3);
			strncpy(rgn_prfx,this_rgn->rgn_old,3);
			return r_rgn.r_nr[0];
		}
	}
	return(-1);
}
