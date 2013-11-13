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
#include "metadata.h"
#include "strong.h"
#include "lord.h"


static int parse_table_manager(struct tag *root, tab_dir *self);

char *attach_pom_extension(char *dir,char *in){
	char *pos, buf[256];
	strcpy(buf,dir);
	strcat(buf,in);
	if (!(((pos=strstr(buf,".pom")) && (strchr(buf,'\0')-pos)==4)))
		strcat(buf,".pom");
	return strcpy(in,buf);
	
}
		

void tab_dir_close(tab_dir *self){
	int i;
	if (!self)
		return;
	if (self->def_lab)
		close_def_data(self->def_lab);
	/*close all additional open tables*/
	for (i=0; i<self->n_additional; i++)
		grim_close(self->additional_tables[i]);
	free(self->additional_tables);
	free(self);
}


tab_dir *tab_dir_open(char *path){
	tab_dir *self=NULL;
	struct tag root,def_lab_tag, def_tab_tag;
	char *xml=NULL;
	char dir[128],*pos;
	FILE *fp;
	int n_err,ok;
	self=calloc(sizeof(tab_dir),1);
	if (!self){
		lord_error(TR_ALLOCATION_ERROR,LORD("Failed to allocate space."));
		return NULL;
	}
	fp=fopen(path,"r");
	if (!fp){
		lord_error(TR_ALLOCATION_ERROR,LORD("Failed to open. %s"),path);
		free(self);
		return NULL;
	}
	pos=path_basename(path);
	strncpy(dir,path,pos-path);
	dir[pos-path]='\0';
	lord_debug(0,LORD("dir is: %s"),dir);
	strncpy(self->dir,dir,128);
	xml=mem_friendly_reader(fp);
	fclose(fp);
	 root=get_root_tag(xml);
	 if(!root.valid){
		lord_error(TAB_N_MAN_,LORD("Invalid xml - root tag not found."));
		goto CLEANUP;
	}
	
	if(!compare_tag(&root,"def_trlib")){
		lord_error(TAB_N_MAN_,LORD("Illegal root tag in table manager file."));
		goto CLEANUP;
	}
	def_lab_tag=get_named_child(&root,"def_lab");
	if (!def_lab_tag.valid){
		lord_error(TAB_N_MAN_,LORD("def_lab tag not found - invalid trlib definition file."));
		goto CLEANUP;
	}
	
	self->def_lab=open_def_data(&def_lab_tag,self->dir,&n_err);
	if (!(self->def_lab)){
		lord_error(TAB_N_MAN_,LORD("Failed to parse def_lab - invalid trlib definition file."));
		goto CLEANUP;
	}
	lord_debug(0,LORD("Parsed def_lab, errors: %d"),n_err);
	def_tab_tag=get_named_child(&root,"def_tab_manager");
	if(!def_tab_tag.valid){
		lord_error(TAB_N_MAN_,LORD("def_tab_manager tag not found - illegal trlib definition file."));
		goto CLEANUP;
	}
	ok=parse_table_manager(&def_tab_tag,self);
	if (ok!=TR_OK)
		goto CLEANUP;
	free(xml);
	return self;
	CLEANUP:
		close_def_data(self->def_lab);
		free(self);
		free(xml);
		return NULL;
}
	
	
static int parse_table_manager(struct tag *root, tab_dir *self){
	struct tag tag_l1, tag_l2;
	int i, j, n_allocated;
	int len = 128;
	char name[512];
	GRIM g;
	GRIM *tables=NULL;
	tables=calloc(sizeof(GRIM),30);
	if (tables==NULL){
		lord_error(TR_ALLOCATION_ERROR,LORD("Failed to allocate memory for tables."));
		return TR_ALLOCATION_ERROR;
	}
	n_allocated=30;
	tag_l1=get_named_child(root,"tables");
	if (!tag_l1.valid){
		lord_error(TR_ALLOCATION_ERROR,LORD("Tag: 'tables' not found."));
		return TR_ERROR;
	}

	tag_l2=get_next_child(&tag_l1,NULL);
	for (i=0; tag_l2.valid; i++){
		get_value_as_string(&tag_l2,name,len);
		g=grim_open(attach_pom_extension(self->dir,name));
		if (!g){
			lord_error(TR_ALLOCATION_ERROR,LORD("Failed to create grim object from %s."),name);
			return TR_ERROR;
		}
		tables[i]=g;
		if (i>n_allocated-1){
			tables=realloc(tables,n_allocated+30);
			n_allocated+=30;
		}
		/*TODO: err check*/
		tag_l2=get_next_child(&tag_l1,&tag_l2);
	}
	self->n_additional = i;
	self->additional_tables=realloc(tables,i);

	/*
	tag_l1=get_named_child(root,"geoid_sequence");
	if (!tag_l1.valid){
		lord_error(TR_ALLOCATION_ERROR,LORD("Tag: 'geoid_sequence' not found."));
		return 0;
	}
	tag_l2=get_next_child(&tag_l1,NULL);
	for (j=0; tag_l2.valid && j<10; j++){
		get_value_as_string(&tag_l2,name,len);
		self->geoid_seq_std[j]=NULL;
		for(i=0; i<self->n_geoids && self->geoid_seq_std[j]==NULL; i++){
			if (!strcmp(name,grim_filename(self->geoids[i])))
				self->geoid_seq_std[j]=self->geoids[i];
		}
		if (self->geoid_seq_std[j]==NULL){
			lord_error(TR_LABEL_ERROR,LORD("Undefined table %s in std-sequence."),name);
			return 0;
		}
			
		
		tag_l2=get_next_child(&tag_l1,&tag_l2);
	}
	self->n_geoid_seq_std = j;*/
	lord_debug(0,LORD("Additional tables: %d"),self->n_additional);
	return TR_OK;
}
	
	
