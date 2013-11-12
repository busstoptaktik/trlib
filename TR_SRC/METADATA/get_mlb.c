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

#include    <string.h>
#include    <stdio.h>
#include    <ctype.h>
#include    <stddef.h> /*included here to be able to build on linux and mac osx */
#include    "KmsFncs.h"

int                  get_mlb(
/*_________________________*/
char                *mlb_str,
short               *region,
char                *mlb1,
short               *sepch,
char                *mlb2,
char               **h_mlb
)
{

  char                tx[4*MLBLNG], *ptx = tx;
  int                 tx_lng, i, res;
  char               *p_sep, *p_mlb;
  union rgn_un        lab_rgn;
  char                rgn_name[32];


  *region = 0;
  *sepch  = 0;
  *mlb1   = (char) '\0';
  *mlb2   = (char) '\0';
  *h_mlb  = NULL;

  if (mlb_str != NULL && strlen(mlb_str) >= 2) {

    /* COPY FIRST ITEM OF MLB-STRING */
    (void) sscanf(mlb_str, "%s%n", tx, &tx_lng);
    res = tx_lng;


    /* REGION PREFIX */
    /* Collect and remove a contingent region prefix from the  */
    /* label textstring. Test first and second char for region */
    /* The third char must be an underscore                    */
    if (isupper(*(tx +0)) && isupper(*(tx +1))
        &&     (*(tx +2) == '_')) {
      (void) strncpy(lab_rgn.prfx, tx, 2);
      lab_rgn.r_nr[1] = 0;

      if ((conv_rgn(-1, lab_rgn.prfx, rgn_name)) > 0) { 
        ptx    += 3;
        tx_lng -= 3;
      } else lab_rgn.r_nr[0] = 0;

    } else lab_rgn.r_nr[0] = 0; /* no prefix letters found */

    *region = lab_rgn.r_nr[0];

    /* SYSTEM PART */
    /* Extract the system part and the contingent separator  */

    /* get the separator char */
    /* A stupid exception for ETRS */
    if (strncmp(ptx, "ETRS-", 5) == 0) {
      p_sep = ptx + 5;
      if (strncmp(p_sep, "TM",   2) == 0) p_sep += 2;
      else
      if (strncmp(p_sep, "LAEA", 4) == 0) p_sep += 4;
      else
      if (strncmp(p_sep, "LCC",  3) == 0) p_sep += 3;
    }
    else p_sep = ptx;
    /* try underscore or CAPITAL */
    for ( ; *p_sep != '\0'; p_sep++) {
      if (*p_sep == '_' || isupper(*p_sep)) {
        *sepch = *p_sep;
        *p_sep = '\0';
        break;
      }
    }

    tx_lng = tx_lng - (int) ((ptrdiff_t)p_sep - (ptrdiff_t)ptx);

    if (*sepch == 'H') { 
      /* search underscore */
      *h_mlb = strchr(p_sep +1, '_');
      if (*h_mlb != NULL) {
        **h_mlb = '\0';
        tx_lng  = (int) ((ptrdiff_t)*h_mlb - (ptrdiff_t)p_sep);
        *h_mlb  = mlb_str + ((ptrdiff_t)*h_mlb - (ptrdiff_t)tx) +1;
      }
    }
    else *h_mlb = NULL;

    (void) strcpy(mlb1, ptx);
    *mlb2  = '\0';
    for (p_mlb = mlb2, i = tx_lng, p_sep++; i; i --)
       *(p_mlb ++) = (isupper(*p_sep))
                   ? (char) tolower(*p_sep++) : *(p_sep++);
  }
  else res = 0;

  return(res);
}

