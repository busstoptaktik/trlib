#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include "friendly_reader.h"
#include "parse_xml.h"
#include "lord.h"
#define SKIP_SPACE(a) while( isspace(*a) ) a++

static ptrdiff_t item_len(struct tag *t);
static ptrdiff_t tag_len(struct tag *t);
static xml_error next_tag(char *xml, struct tag *t_in);
static xml_error find_end_tag(struct tag*);
static struct tag s_read_tag(char *, char *);
static xml_error preparse_xml(char *in, char **end);
static char *next_word(char *start, char **stop);
static struct tag new_tag(int level);
static xml_error go_deeper(struct tag *t);
static int copy_string(char *in, void *out, size_t len, void *nothing);

/*get value */
int get_value(struct tag *t, void *out, size_t size, item_converter conv, void *conv_data){
	int res=0;
	char *pos=t->pos[1]+1,*tmp,c;
	ptrdiff_t len=item_len(t);
	if (len==0 || t->is_simple){
		t->err_no=XML_EMPTY_ITEM;
		return 0;
	}
	if (!t->valid || (get_next_child(t,NULL)).valid){
		t->err_no=XML_BAD_HIERARCHY;
		return 0;
	}
	SKIP_SPACE(pos);
	if (pos==t->pos[2]) /*empty content*/
		return 0;
	/* oh no - we also need to skip right space*/
	tmp=t->pos[2]-1;
	while(isspace(*tmp) && tmp>pos){
		tmp--;
	}
	tmp++;
	c=*tmp;
	*tmp='\0'; /*insert a break*/
	res=conv(pos,out,size,conv_data); /*number of items converted*/
	*tmp=c; /*restore*/
	return res;
}

char *get_tag(struct tag *t, char *out, size_t buf_len){
	size_t len=(size_t) tag_len(t);
	if (buf_len<len+1)
		return NULL;
	memcpy(out,t->name[0],len);
	out[len]='\0';
	return out;
}

static struct tag new_tag(int level){
	struct tag rtag;
	rtag.valid=0;
	rtag.level=level;
	rtag.is_simple=0;
	rtag.err_no=XML_OK;
	return rtag;
}

static char *next_word(char *start, char **stop){
	char *pos;
	while(isspace(*start) && (start<*stop)) start++;
	if (start==*stop) return NULL;
	pos=start;
	while (!isspace(*start) && (start<*stop)) start++;
	*stop=start;
	return pos;
}

static int copy_string(char *in, void *out, size_t len, void *nothing){
	char *pos=(char*) out;
	strncpy(pos,in,len);
	pos[len-1]='\0';
	return 1;
}

/* copy the content of a tag */
char *get_value_as_string( struct tag *t, char *out, size_t buf_len){
	if (get_value(t,(void*)out,buf_len,copy_string,NULL))
		return out;
	return NULL;
}

/*get the len of an item*/
static ptrdiff_t item_len(struct tag *t){
	return t->pos[2]-t->pos[1]-1;
}

static ptrdiff_t tag_len(struct tag *t){
	return t->name[1]-t->name[0]+1;
}


/*simply compare a tag 'name' with given name */
int compare_tag(struct tag *t, char *name){
	ptrdiff_t len=tag_len(t);
	return (t->valid && strlen(name)==len && !memcmp(t->name[0],name,len));
}

/*fetch a specific nested tag*/
struct tag get_named_child(struct tag *o_tag, char *name){
	struct tag rtag;
	rtag=get_next_child(o_tag,NULL);
	while (rtag.valid){
		if (compare_tag(&rtag, name))
			break;
		rtag=get_next_child(o_tag,&rtag);
	}
	return rtag;
}

/* iterate over nested tags - last_tag must be subtag of o_tag*/
struct tag get_next_child(struct tag *o_tag, struct tag *last_tag){
	struct tag rtag;
	char *start,*stop;
	rtag=new_tag(o_tag->level+1);
	if (o_tag->is_simple){/*tadd special signal that we have reached innermost level */
		rtag.err_no=XML_ROOT_NOT_FOUND;
		return rtag;
	}
	stop=o_tag->pos[2];
	if (last_tag!=NULL){
		if (!(last_tag->pos[0]>o_tag->pos[1]  && last_tag->pos[3]<o_tag->pos[2])){
			rtag.err_no=XML_BAD_HIERARCHY;
			return rtag;
		}
		start=last_tag->pos[3]+1;
	}
	else
		 (start=o_tag->pos[1]+1);
	rtag=s_read_tag(start,stop);
	rtag.level=(o_tag->level)+1;
	return rtag;
}

/*if tag found tag->pos[0],pos[1] will point to '<',  '>'*/ 
static xml_error next_tag(char *xml, struct tag *t_in){
	char *start,*end;
	if ((start=strchr(xml,'<')) && (end=strchr(start,'>'))){
		t_in->pos[0]=start;
		t_in->pos[1]=end;
		/*check for improper '/' */
		while(start<end && *start!='/') start++;
		if (*start=='/'){
			if ((end-start)!=1)
				return XML_MALFORMED;
			else{
				t_in->is_simple=1;
				end--;
			}
		}
		/*find tag as first word after '<'*/
	        start=next_word(t_in->pos[0]+1,&end);
		if (start==NULL) return XML_EMPTY_TAG;
		t_in->name[0]=start;
		t_in->name[1]=end-1; /*make end point to the last char in name*/
		return XML_OK;
	}
	return XML_ROOT_NOT_FOUND;
}	

/*internal utility to set the end position of a given tag name */
static xml_error find_end_tag(struct tag *t_in){
	char *end,*start=t_in->pos[1];
	ptrdiff_t len=tag_len(t_in);
	if (t_in->is_simple){
		t_in->pos[2]=t_in->pos[3]=t_in->pos[1];
		return XML_OK;
	}
	while ((start=strstr(start,"</")) && (end=strchr(start,'>'))){
		t_in->pos[2]=start;
		t_in->pos[3]=end;
		start=next_word(start+2,&end);
		if (start!=NULL && (end-start)==tag_len(t_in) && !memcmp(start,t_in->name[0],end-start))
			return XML_OK;
	}
	return XML_UNBALANCED_TAGS;
}

/* simplistic wrapper to get the root tag of a proper terminated string */
struct tag get_root_tag(char *xml){
	struct tag root;
	char *end;
	root=new_tag(0);
	if (XML_OK!=(root.err_no=preparse_xml(xml,&end)))
		return root;
	return s_read_tag(xml,end);
}

/*simplify and prevalidate xml string. Will most likely shrink the string length*/
static xml_error preparse_xml(char *in, char **end){
	char *pos_in,*pos_out;
	pos_in=strchr(in,'<'); /*find opening tag*/
	if (!pos_in)
		return XML_ROOT_NOT_FOUND;
	pos_out=in;
	while(*pos_in){
		if (!strncmp(pos_in,"<!--",4)){
			if ((pos_in=strstr(pos_in,"-->")))
				pos_in+=3;
			else
				return XML_MALFORMED_COMMENT;
			SKIP_SPACE(pos_in);
			continue;
		}
		if (!strncmp(pos_in,"<?",2)){
			if ((pos_in=strstr(pos_in,"?>")))
				pos_in+=2;
			else
				return XML_MALFORMED_COMMENT;
			SKIP_SPACE(pos_in);
			continue;
		}
		*(pos_out++)=*(pos_in++);
		
	}
	*pos_out='\0';
	if (end)
		*end=pos_out;
	return XML_OK;
}

/* will expand next tag if any - end should point at 1 char after valid xml input to scan*/
 static struct tag s_read_tag(char *start, char *end){
	struct tag rtag;
	xml_error res;
	char e_val=*end; /*store end*/
	*end='\0'; /*insert terminator*/
	rtag=new_tag(0);
	if (XML_OK==(res=next_tag(start, &rtag))){
		if (XML_OK==(res=find_end_tag(&rtag)))
			rtag.valid=1;
		else
			rtag.err_no=res;
	}
	else
		rtag.err_no=res; /*means next nested tag is not valid */
	/*restore end_val*/
	*end=e_val;
	return rtag;
}
/*HMM iterate over each level*/
static xml_error go_deeper(struct tag *t){
	struct tag i;
	xml_error res;
	printf("Current level: %d\n",t->level);
	i=get_next_child(t,NULL);
	while (i.valid){
		res=go_deeper(&i);
		if (res!=XML_OK)
			return res;
		i=get_next_child(t,&i);
	}
	return (i.err_no==XML_ROOT_NOT_FOUND)?XML_OK:i.err_no;
}

/*validate xml */
xml_error validate_xml(struct tag *root){
	xml_error res=go_deeper(root);
	return res;
}

/*just a test*/
#ifdef TEST
int main(int argc, char **argv){
	char *fname;
	FILE *fp;
	char  buf[1024], *xml;
	struct tag root,o_tag,i_tag;
	xml_error res;
	size_t size=64*1024,rsize;
	int n_o=0,n_i=0,err;
	if (argc<2)
		return 1;
	fname=argv[1];
	fp=fopen(fname,"rb");
	xml=mem_friendly_reader(fp);
	fclose(fp);
	printf("%s\n",xml);
	if (!xml){
		puts("Mem err...");
		return 1;
	}
	root=get_root_tag(xml);
	if (!root.valid){
		printf("Root tag is not valid... err: %d\n",root.err_no);
		return 1;
	}
	printf("Root tag is: %s\n",get_tag(&root,buf,1024));
	printf("TEXT: %s\n",root.pos[0]);
	res=validate_xml(&root);
	if (res!=XML_OK)
		printf("Invalid XML: %d\n",res);
	o_tag=get_next_child(&root,NULL);
	while (o_tag.valid && n_o<2){
		n_o+=1;
		printf("\n*****************\nOuter Tag #%d: %s\n",n_o,get_tag(&o_tag,buf,1024));
		if (compare_tag(&o_tag,"def_prj")){
			i_tag=get_next_child(&o_tag,NULL);
			printf("Inner tag valid: %d, tag: %s\n",i_tag.valid,get_tag(&i_tag,buf,1024));
			if (compare_tag(&i_tag,"prj")){
				struct tag i_tag_n;
				printf("Inner tag is projection:\n");
				i_tag_n=get_named_child(&i_tag,"mlb");
				if (i_tag_n.valid){
					
					printf("Minilabel: %s\n", get_value_as_string(&i_tag_n,buf,1024));
				}
				i_tag_n=get_named_child(&i_tag,"data");
				if (i_tag_n.valid){
					printf("Data: %s\n", get_value_as_string(&i_tag_n,buf,1024));
				}
				i_tag_n=get_named_child(&i_tag,"descr");
				if (i_tag_n.valid){
					printf("Description: %s\n", get_value_as_string(&i_tag_n,buf,1024));
				}
			}
		}
		else if (compare_tag(&o_tag,"def_grs")){
			printf("Yep same thing....\n");
		}
		else
			puts("TODO\n");
		o_tag=get_next_child(&root,&o_tag);
	}
		
	puts("\n************\nNo more outer tags");
	fclose(fp);
	return 0;
}
#endif	
			