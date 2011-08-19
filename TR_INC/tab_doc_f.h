

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
