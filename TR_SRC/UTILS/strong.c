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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "geo_lab.h"
#include "sgetg.h"
#include "strong.h"
 
/* splits string into items */
/*remember to free stuff afterwards */
char **get_items(char *line, char *delimiter, int *item_count){
	char *item;
	int n_items=0,buf_length=40,buf_size=40;
	char **items=malloc(sizeof(char*)*buf_length);
	if (items==NULL)
		return NULL;
	item=strtok(line,delimiter);
	if (item==NULL){
		free(items);
		return NULL;
	}
	while (item && items){
		*(items+(n_items++))=item;
	
		if (n_items>buf_size)
			{
			buf_size+=buf_length;
			items=realloc(items,(buf_size)*sizeof(char*));
			}
		item=strtok(NULL,delimiter);
	}
	items=realloc(items,n_items*sizeof(char*));
	*item_count=n_items;
	return items;
}

/*append items from list2 to list1 */
/*remember to free stuff afterwards */
char **append_items(char **items1, char **items2, int n_items1, int n_items2){
	int i;
	char **new_items= (char **) realloc(items1,sizeof(char*)*(n_items1+n_items2));
	if (new_items==NULL)
		return NULL;
	
	for (i=0;i<n_items2;i++)
		new_items[n_items1+i]=items2[i];
	return new_items;
}

void remove_whitespace(char *text, char *out){
	while (*text){
		if (!isspace(*text))
			*(out++)=*text;
		text++;
	}
	*out='\0';
}

void string_tolower(char *text){
	for(;*text;text++) *text=tolower(*text);
}

/* gets a number - if it has a unit sgetg is used */
double get_number(char *item){
	int n_chars = (int) strlen(item);
	int used;
	struct typ_dec type;
	char term[2];
	if (isalpha(item[n_chars-1]))
		return sgetg(item,&type,&used,term);
	return atof(item);
}


/*extracts basename/filename from a full path*/
char *path_basename(char *full_path){
	char *pos=full_path;
	while (*full_path){
		if (*full_path=='/' || *full_path=='\\')
			pos=full_path+1;
		full_path++;
	}
	return pos;
}
