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

#include <stdlib.h>
#include <string.h>

#include "parse_xml.h"
#include "friendly_reader.h"
#include "tab_dir_open.h"
#include "KmsFncs.h"

void tab_dir_close(char *self){
	free(self);
}


tab_dir *tab_dir_open(char *path){
	tab_dir *self=NULL;
	int i, j;
	int len = 128;
	struct tag root,tag_l1, tag_l2;
	char *xml=NULL;
	char name[128];
	GRIM g;
	FILE *fp;
	self=calloc(sizeof(tab_dir),1);
	if (!self){
		lord_error(TR_ALLOCATION_ERROR,LORD("Failed to allocate space."));
		return NULL;
	}
	fp=fopen(path,"r");
	if (!fp){
		lord_error(TR_ALLOCATION_ERROR,LORD("Failed to open manager file."));
		free(self);
		return NULL;
	}
	xml=mem_friendly_reader(fp);
	fclose(fp);

	root=get_root_tag(xml);
	if(!root.valid || !compare_tag(&root,"def_tab_manager")){
		lord_error(TAB_N_MAN_,LORD("Illegal manager name %s"),path);
		goto CLEANUP;
	}
	
	tag_l1=get_named_child(&root,"geoids");
	if (!tag_l1.valid){
		lord_error(TR_ALLOCATION_ERROR,LORD("Tag: 'geoids' not found."));
		goto CLEANUP;
	}
	tag_l2=get_next_child(&tag_l1,NULL);
	
	for (i=0; tag_l2.valid; i++){
		get_value_as_string(&tag_l2,name,len);
		g=grim_open(name);
		if (!g){
			lord_error(TR_ALLOCATION_ERROR,LORD("Failed to memory map %s."),name);
			goto CLEANUP;
		}
		self->geoids[i]=g;
		/*TODO: err check*/
		tag_l2=get_next_child(&tag_l1,&tag_l2);
	}
	self->n_geoids = i;
	tag_l1=get_named_child(&root,"dhtables");
	if (!tag_l1.valid){
		lord_error(TR_ALLOCATION_ERROR,LORD("Tag: 'dhtables' not found."));
		goto CLEANUP;
	}
	tag_l2=get_next_child(&tag_l1,NULL);
	for (i=0; tag_l2.valid; i++){
		get_value_as_string(&tag_l2,name,len);
		if (!g){
			lord_error(TR_ALLOCATION_ERROR,LORD("Failed to memory map %s."),name);
			goto CLEANUP;
		}
		self->dh_tabs[i]=g;
		g=grim_open(name);
		tag_l2=get_next_child(&tag_l1,&tag_l2);
	}
	self->n_dhtabs = i;
	tag_l1=get_named_child(&root,"t3dtab");
	if (!tag_l1.valid){
		lord_error(TR_ALLOCATION_ERROR,LORD("Tag: 't3dtab' not found."));
		goto CLEANUP;
	}
	tag_l2=get_next_child(&tag_l1,NULL);
	for (i=0; tag_l2.valid; i++){
		get_value_as_string(&tag_l2,name,len);
		g=grim_open(name);
		if (!g){
			lord_error(TR_ALLOCATION_ERROR,LORD("Failed to memory map %s."),name);
			goto CLEANUP;
		}
		self->t3d[i]=g;
		/*TODO: err check*/
		tag_l2=get_next_child(&tag_l1,&tag_l2);
	}
	self->n_3dtabs = i;
	tag_l1=get_named_child(&root,"geoid_sequence");
	if (!tag_l1.valid){
		lord_error(TR_ALLOCATION_ERROR,LORD("Tag: 'geoid_sequence' not found."));
		goto CLEANUP;
	}
	tag_l2=get_next_child(&tag_l1,NULL);
	for (j=0; tag_l2.valid && j<10; j++){
		get_value_as_string(&tag_l2,name,len);
		self->geoid_seq_std[j]=-1;
		for(i=0; i<self->n_geoids && self->geoid_seq_std[j]==-1; i++){
			if (!strcmp(name,grim_name(self->geoids[i])))
				self->geoid_seq_std[j]=i;
		}
		if (self->geoid_seq_std[j]==-1){
			lord_error(TR_LABEL_ERROR,LORD("Undefined table %s in std-sequence."),name);
			goto CLEANUP;
		}
			
		/*TODO: err check*/
		tag_l2=get_next_child(&tag_l1,&tag_l2);
	}
	self->n_geoid_seq_std = j;
	lord_debug(0,LORD("Geoids: %d, dh_tabs: %d, 3d_tabs: %d"),self->n_geoids,self->n_dhtabs,self->n_3dtabs);
	free(xml);
	return self;
	
	CLEANUP:
		free(self);
		free(xml);
		return NULL;
}