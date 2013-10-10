/*TODO: thokn - add header stuff*/
typedef struct POM_s { char **d; unsigned int top, size; } POM;
char *chomp(char *p);
int ispom(char *filename, FILE *p);
POM *pom_create(unsigned int size);
POM *pom_free(POM *s);
void pom_push(char *item, POM *s);
POM *pomread(const char *name);
char *pomval(POM *s, char *att);
double pomtod(POM *p, char *a);
char *slashback (int as_is, int to_be, char *s);
char *pom_absolute_filename (char *pomfile, char *datafile);