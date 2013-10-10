#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "friendly_reader.h"

#define SKIP_SPACE(a) while( isspace(*a) ) a++
#define R_SKIP_SPACE(start,a) a=(strchr(start,'\0')-1); while (a>=start && isspace(*a)) *(a--)='\0'

/*memory friendly text reader which skips left and right space and all chars until first '<' */
char *mem_friendly_reader(FILE *fp){
	char *out,*more,*pos1,*pos2,buf[8192];
	int found=0;
	unsigned int buf_size=8192,n_written=0;
	out=malloc(buf_size*sizeof(char));
	if (!out)
		return NULL;
	*out='\0';
	while (fgets(buf,8192,fp)){
		pos1=buf;
		if (!found && !(found=(pos1=strchr(buf,'<'))!=NULL))
			continue;
		SKIP_SPACE(pos1);
		if (!(*pos1))
			continue;
		R_SKIP_SPACE(pos1,pos2);
		n_written+=(int) (pos2-pos1)+1;
		if (n_written>buf_size-1){
			more=realloc(out,(n_written+1024)*sizeof(char));
			if (!more){
				free(out);
				return NULL;
			}
			buf_size=n_written+1024;
			out=more;
		}
		strcat(out,pos1);
	}
	realloc(out,(n_written+1)*sizeof(char));
	return out;
}