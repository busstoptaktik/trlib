
#if !defined     (H_PARSE_XML)
#define           H_PARSE_XML

#include <stdio.h>

typedef enum{ 
	XML_OK=0, 
	XML_MALFORMED=1,
	XML_TAG_NOT_CLOSED=2, 
	XML_UNBALANCED_TAGS=3 , 
	XML_ROOT_NOT_FOUND=4, 
	XML_OVERFLOW_ERROR=5, 
	XML_MALFORMED_COMMENT=6,
	XML_BAD_HIERARCHY=7,
	XML_EMPTY_ITEM=8,
	XML_EMPTY_TAG=9
} xml_error;

struct tag{
	char *pos[4];
	char *name[2];
	int level,valid,is_simple;
	xml_error err_no;
};

typedef int( *item_converter)(char *val , void *out, size_t buf_size, void *conv_data);
/* super simplistic implementation - all memory managemnet is up to the user. 
* Make sure the input text buffer doesn not go out of scope as tags store pointers into that buffer 
* Easy enough to make a more fancy interface with memory management and improved tag navigation though.
*TODO: add attr parser...
*/

struct tag get_root_tag(char *xml);
struct tag get_named_child(struct tag *parent, char *name);
struct tag get_next_child(struct tag *parent, struct tag *last_tag);
int get_value(struct tag *t, void *out, size_t size, item_converter conv, void *conv_data);
char *get_value_as_string( struct tag *t, char *out, size_t buf_len);
int compare_tag(struct tag *t, char *name);
xml_error validate_xml(struct tag *root);

/* demo 
* first read xml into buf...*
struct i_tag,ii_tag;
int ok=preparse_xml(buf);
struct tag root=get_root_tag(buf);
i_tag=get_next_child(root,NULL); NULL means that we start reading from the top...
while (i_tag.valid){
    ii_tag=get_named_child(i_tag,"data");
    if (ii_tag.valid)
          do_stuff(&ii_tag);
    i_tag=get_next_tag(root,i_tag);
}
*/
#endif