
 /* gettabdir  ver 2000.01           # page 1   12 Jan 1999 17 23 */


#include        <string.h>


static char dir_grid[512];


char    *gettabdir() {

  return(dir_grid);

}


char     *settabdir(
char     *new_dir
) {

#ifdef   VALDEMARTEST
#define  GEOIDPATH       "/home/geoid/"
#elif    K_LNX_P                                     /* LINUX */
#define  GEOIDPATH       "/bin/geoids/"
#elif defined(sparc) || defined(__sparc)             /* UNIX */
#define  GEOIDPATH       "/export/home/geoid/"
#else                                                /* DOS */
#define  GEOIDPATH       "/geoids/"
#endif

  int i = 0;
  if (new_dir != NULL) {
    if (strlen(new_dir) != 0) {
      (void) strcpy(dir_grid, new_dir);
      i = 1;
    }
  }

  if (i == 0 && *dir_grid == '\0') (void) strcpy(dir_grid, GEOIDPATH);

  return(dir_grid);


#undef   GEOIDPATH

}

