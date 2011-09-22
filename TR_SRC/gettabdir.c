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

