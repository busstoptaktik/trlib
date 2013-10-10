#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <math.h>  /* for HUGE_VAL */
#include <assert.h>
#include "pom.h"





enum {BUFFER_SIZE = 16384};


char *chomp(char *p) {
   char *q;

   /* find start of string */
   while (isspace(*p)) p++;  /* for (; isspace (*p);  p++); or while (isspace (*(p++))) */

   /* find end of string */
   q = p; while (*q) q++;  /* for (q = p;  *q;  q++); */

   /* eat trailing whitespace */
   while (isspace(*(--q))); *(++q) = 0;

   return p;
}





int ispom(char *filename, FILE *p) {
	int firstchar = 0, last;
	if (p) {
		firstchar = fgetc (p);
		ungetc (firstchar, p);
    }
    
    /* does it look like a PPM on stdin? */
    if ((0==filename)||(0==strlen (filename)))		
		return 'P'==firstchar ? 0: 1;
	
	last = strlen (filename) - 1;
	if (last < 4)
	    return 0;
	
	/* extension must be 'pom' */    
	if ((filename[last]!='m') && (filename[last]!='M'))
        return 0;
	if ((filename[last-1]!='o') && (filename[last-1]!='O'))
        return 0;
	if ((filename[last-2]!='p') && (filename[last-2]!='P'))
        return 0;
    return 1;
}



POM *pom_create(unsigned int size){
    POM *s;
    if (0==(s  =  malloc(sizeof(POM))))
        return 0;
    if (0==(s->d = malloc(size*sizeof(void *)))){
        free(s);
        return 0;
    }
    s->size   = size;
    s->top    = 0;
    return s;
}


POM *pom_free(POM *s) {
    unsigned int i;
    for (i = 0; i < s->top; i++)
        free(s->d[i]);
    free(s->d);
    free(s);
    return 0;
}




void pom_push(char *item, POM *s){
    if(s->top == s->size){
        s->d = realloc(s->d, 2*(s->size+1)*sizeof(void *));
        assert(s->d!=0);
        s->size = 2*(s->size+1)-1;
    }
    s->d[(s->top)++] = item;
}


void *pom_pop(POM *s){
    return (s->top)? s->d[(s->top)--]: 0;
}


/* read a pom-file into a POM */
POM *pomread(const char *name) {
    POM *s;
    FILE *f;
    char p[BUFFER_SIZE], *q;
    if (0==(s = pom_create(10)))
        return 0;
    if (0==(f = fopen(name, "rt")))
        return pom_free(s);
    while(fgets(p, BUFFER_SIZE, f)){
        char *qq;
        q = chomp(p);
        if (q[0]=='#' || q[0]==0)
            continue;
        if (0==(qq = strdup(q)))
            return fclose(f), pom_free(s);
        pom_push(qq, s);
    }
    fclose(f);
    return s;
}


/* safe return value for undefined attributes */
static char EMPTY_STRING[] = {""};

/* search for attribute - return its value */
char *pomval(POM *s, char *att) {
   unsigned int i, m;
   char *c;
   m = strlen(att);
   for (i = 0; i < s->top; i++) {
       if (0!=strncmp(s->d[i], att, m))
           continue;
       if (':'!=s->d[i][m])
           continue;

       c = s->d[i] + (m+1);
       while (isspace(*c))
           c++;
       return c;
   }
   return EMPTY_STRING;
}



/* pomtod() is somewhat like strtod() - a slightly fail safe atof() */
double pomtod(POM *p, char *a) {
    double v;

    /* if the attribute value cannot be interpreted as a double, format is wrong */
    if (1!= sscanf (pomval (p, a), "%lg", &v))
        return HUGE_VAL;
    return v;
}





/* 
 * Create a full path name for a file named relative to another file
 * (typically a data file name referred inside a metadatafile).
 * 
 * TODO: gold plating (esp. for windows file names). Currently no
 * problem as we control contents of pom and relative location of data
 * 
 * NOTE: the buffer holding the merged file name is dynamically allocated,
 * and must be freed by the caller.
 * 
 */
 
char *slashback (int as_is, int to_be, char *s) {
    int i;
    /* translate backslashes to slashes, but take care of unicode high bits */
    for (i = 0; s[i]!=0; i++) {
        if ((unsigned char) (s[i]) > 127)
            continue;
        if (s[i] == as_is)
            s[i] = to_be;
    }
    return s;
}

 
char *pom_absolute_filename (char *pomfile, char *datafile) {
    size_t n, m, i;
    char *merged, *last_slash;
    int slash = '/', backslash = '\\';
    n = strlen (pomfile);
    m = strlen (datafile);
    
    merged = malloc (n+m+3);
    if (0==merged)
        return 0;
    /* No path propagation if the data file is on another windows drive */
    if (':'==datafile[1])
        return strcpy (merged, datafile); /* strcpy returns its first argument */
    strcpy (merged, pomfile);
    
    /* translate backslashes to slashes, but take care of unicode high bits */
    for (i = 0; i < n; i++) {
        if ((unsigned char) (merged[i]) > 127)
            continue;
        if (merged[i] == backslash)
            merged[i] = slash;
    }
    last_slash = strrchr (merged, slash);
    
    /* no path? erase pomfilename, except for microsoft drive letter part */
    if (0==last_slash) {
        if (':'==merged[1])
            merged[2] = '\0';
        else 
            merged[0] = '\0';
    }
    else
        *(++last_slash) = '\0';
        
    /* append data file name */
    strcat (merged, datafile);
    
    /* clean up any slash/backslash mess */
    /* translate backslashes to slashes, but take care of unicode high bits */
    for (i = 0; i < n+m; i++) {
        if ((unsigned char) (merged[i]) > 127)
            continue;
        if (merged[i] == backslash)
            merged[i] = slash;
    }
#ifdef TEST_POM
    printf ("MERGED %s\n", merged_filename);
#endif
    return merged;

}










#ifdef TEST
/*
gcc -pedantic -W -Wall -DTEST -o pom pom.c
*/
const char pomfilecontents[] = {
   "# comment\n\n#interleaved by blanks\n"
   "    image0:       gylle.tif\n"
   "    nrows:        2134\n"
   "    ncols:        4321\n"
   "    col0was:      0\n"
   "    row0was:      2134\n"
   "    orientation0: pladre.abs\n"
   "    orientation1: pludre.dsup\n"
   "    refframe:     utm32Heuref89_h_dvr90\n"
   "    rotations:    -0.6717176095 0.1267830397 0.0992606113\n"
   "    X0:           477379.082 6264034.973 3837.841"
};


int main(int argc, char *argv[]){
   FILE *f;
   POM *s;
   char *val;
   unsigned int i;
   char *w;
   
   w = pom_absolute_filename ("D:\\gyllespreder\\møghavn/saltagurk\\pop\\sort.hvid", "..\\kanonsovs.plonk");
   printf ("w=%s\n", w); free (w);
   w = pom_absolute_filename ("D:\\gyllespreder\\møghavn/saltagurk\\pop\\sort.hvid", "D:..\\kanonsovs.plonk");
   printf ("w=%s\n", w);; free (w);

   f = fopen(argc>1 ? argv[1]: "gylle.pom", "wt");
   fwrite(pomfilecontents, sizeof(pomfilecontents), 1, f);
   fclose(f);

   s = pomread("06215475.tiffinfo");
   for (i=0; i<s->top; i++)
       puts(s->d[i]);

   val = pomval(s, "rotations");
   printf("\nattribute: [rotations]\nvalue:     [%s]\n", val);

   /* read an empty (i.e. undefined) attribute */
   printf("\nattribute: [pilemannn]\nvalue:     [%s]\n", pomval(s,"pilemannn"));


   val = pomval(s, "Image Width");
   printf("\nattribute: [Image Width]\nvalue:     [%s]\n", val);
   val = pomval(s, "Rows/Strip");
   printf("\nattribute: [Rows/Strip]\nvalue:     [%s]\n", val);
   val = pomval(s, "0");
   printf("\nattribute: [0]\nvalue:     [%s]\n", val);
   val = pomval(s, "1 Strips");
   printf("\nattribute: [1 Strips]\nvalue:     [%s]\n", val);


   return 0;
}
#endif

/*SE OGSÅ tiffinfo tiffcp tiffdump*/

