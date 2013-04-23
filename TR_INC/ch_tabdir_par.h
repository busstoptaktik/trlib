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
 


/* ch_tabdir_par  ver 2010.01          # page 1    15 Feb 2010 15 35 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

/* ch_tabdir_par(mode, **s_f, *s_p, *n_f, n_p) saves some      */
/*      PARAMS to (**s_f, *s_p)                                */
/*      and asigns new values from (*n_f, n_p)                 */
/*                                                             */
/* sevice routine for label and table system                   */

/*    result ==  0 : ok                                        */

/*    mode         : kind of file to be opened                 */
/*    mode   ==  1 : *manager_file, init_man_pos               */
/*    mode   ==  2 : *man_gps_file, init_gps_pos               */
/*    mode   ==  3 : *def_lab_file, init_lab_pos               */
/*                 :  init_prj_pos = n_p, init_rgn_pos = 0     */
/*                 :  init_dtm_pos = 0, init_grs_pos = 0       */

#if !defined(H_CH_TABDIR_PAR)
#define      H_CH_TABDIR_PAR

#include <stdio.h>

extern void       ch_tabdir_par(
/*____________________________*/
int               mode,
FILE            **s_f,
size_t           *s_p,
FILE             *n_f,
size_t            n_p
);

#endif

