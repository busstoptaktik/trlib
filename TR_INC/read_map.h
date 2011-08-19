

/* read_map.h    version 1.0        # page 1   25 Nov 1997 10 36 */


/* Copyright (c) 1997, Kort & Matrikelstyrelsen, Denmark */
/* All rights reserved.                                  */

/* This is unpublished proprietary source code of ITG, Kort & */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not */
/* indicate an intention of publishing this code.             */

/* Prog.: F. Steffensen NOV. 1997 (MI/ITG) */

/* Function:    */

#if !defined     (H_READ_MAP)
#define           H_READ_MAP 

#include         "geo_lab.h"

extern int        read_map(
char                      *in,
int                       *used,
char                      *map_str,
union geo_lab             *map_lab,
double                    *c_poly
); 

/* read_map     (kald og retur)         int                    */
/*              Læser kortbladsnummer fra strengen in, og      */
/*              returnere kortbladstypen.                      */

/* in           (kald)                  char *                 */
/*              Streng der indeholder kortbladsnummer.         */

/* map_str      (retur)                 char *                 */
/*              Pjat, pænt formateret streng indeholdende      */
/*              kortbladsnummeret, op til 24 karakterer lang.  */

/* map_lab      (retur)                 union geo_lab *        */
/*              Label for returnerede koordinater.             */

/* c_poly       (retur)                 long *                 */
/*              Hjørnekoordinater for kortbladsnummer.         */
/*              c_poly skal erklæres med mindst 9 elementer    */

/* Denne funktion er baseret på gamle algol map-procedure */
/* Der returneres kun hjørnekoordinater for typerne:      */
/* CM_MAP, MV_MAP                                         */

#endif


