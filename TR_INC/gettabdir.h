
 /* gettabdir  ver 2001.01           # page 1   12 Dec 2001 17 23 */


#ifndef  _CONV_GETTABDIR_FNC_
#define  _CONV_GETTABDIR_FNC_

extern char   *gettabdir();

/* gettabdir will return a pointer to a string (dir_grid) holding the 
   directory of the tables.
*/

extern char   *settabdir(
char          *new_dir
);

/* settabdir will return a pointer to a string (dir_grid) holding the 
   directory of the tables.

   if (new_dir == NULL || strlen(new_dir) == 0) a default
   string is copyed to the string (dir_grid).

   else is the content of new_dir copied to the string (dir_grid).

   DEFAULT STRINGS ::
      sparc || __sparc :  "/export/home/geoid/"
      K_LNX_P          :  "/bin/geoids/"
      else             :  "/geoids/"
*/


#endif

