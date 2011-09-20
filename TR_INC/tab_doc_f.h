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
 


/* tab_doc_f     ver 2009.01          # page 1    26 Mar 2009 10 19 */


/* Copyright (c) 2009, National Space Institute, DTU, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of DTU, Denmark */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */


/* tab_doc_f(name, fh_out) find the name of a table            */
/*                        and writes some parameters on fh_out */
/*    result  >  0      : grid lab ok (==lab_type, geo_lab.h)  */
/*    result ==  0      : STOP recursive call                  */
/*    result == -1      : name is not found                    */
/*    result == -2      : manager.tab NOT FOUND                */
/*    result == -3      : unknown first char in name           */
/*                                                             */
/*   ?<NAME>   =>  Table name, short description               */
/*   *<NAME>   =>  Table name, detail description              */
/*                                                             */
/*   ?<TYPE>   =>  Table name, short description  (recursive)  */
/*   *<TYPE>   =>  Table name, detail description (recursive)  */
/*                                                             */
/*   ?         =>  Table name, short description  (recursive)  */
/*   *         =>  Table name, detail description (recursive)  */
/*                                                             */
/*   !         =>  works as ?                                  */
/*                                                             */
/* Recursive calls produces information for one table in each  */
/* call until result == 1. Continuing after this will restart  */
/* the loop.                                                   */
/*                                                             */
/* <TYPE> may be ::                                            */
/*        "geoid", "dtmtab", "dhtab", "upltab", "demtab",      */
/*        "tabtab", "bagtab", "t3dtab", "t2dtab", "tddtab"     */

/* see allso geoid_d.h                                         */


extern int               tab_doc_f(
/*_______________________________*/
char                    *tab_name,
FILE                    *fh_out
);

/* progr. K.Engsager, Mar, 2009, */
