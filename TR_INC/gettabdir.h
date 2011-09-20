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

