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



/* pom.c: based on code from the boogie-project, originally written by Thomas Knudsen */

if !defined(H_POM)
define      H_POM

#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>


#define inline static inline
typedef struct POM_s { char **d; unsigned int top, size; } POM;
enum {BUFFER_SIZE = 16384};

/* read a pom-file into a POM */
inline POM *pomread(const char *name);

/* search for attribute - return its value */
inline char *pomval(POM *s, char *att);

inline char *chomp(char *p) {
   char *q;

   /* find start of string */
   while (isspace(*p)) p++;

   /* find end of string */
   q = p; while (*q) q++;   /* looks like for (q = p; *q; q++); to me */

   /* eat trailing whitespace */
   while (isspace(*(--q))); *(++q) = 0;

   return p;
}







inline POM *pom_create(unsigned int size){
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

/* pom_free used to have a Danish language counterpart named pom_frit. Hilarious...? */
inline POM *pom_free(POM *s) {
   unsigned int i;
   for (i = 0; i < s->top; i++)
       free(s->d[i]);
   free(s->d);
   free(s);
   return 0;
}



/* hack: using a stack as generic storage */

inline void pom_push(char *item, POM *s){
 if(s->top == s->size){
   s->d = realloc(s->d, 2*(s->size+1)*sizeof(void *));
       assert(s->d!=0);
   s->size = 2*(s->size+1)-1;
 }
 s->d[(s->top)++] = item;
}


inline void *pom_pop(POM *s){
 return (s->top)? s->d[(s->top)--]: 0;
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


/* read a pom-file into a POM */
inline POM *pomread(const char *name) {
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


static char EMPTY_STRING[] = {""};

/* search for attribute - return its value */
inline char *pomval(POM *s, char *att) {
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




/* Demo program. Compile with -DTEST */
#ifdef TEST
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

#endif

