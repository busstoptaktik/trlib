

/* u32_to_map.h                     # page 1   04 Jul 1997 12 34 */


/* Copyright (c) 1997, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                   */

/* This is unpublished proprietary source code of ITG, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

/* F. Steffensen JUL. 1997 (MI/ITG) */
/* SWN sept. 1997 REK (enkelt rettelse vedr. indlab p� Bornholm */

/* u32_to_map.c er en udgave af nedenst�ende */

/* u32_t_kvart.c
* 
* Funktion til at beregne hvilket kortbladskvartnummmer en 
* koordinat (n,e) i UTM zone 32 opgiver i meter ligger i.
*
* OBS : S�rkortene 10, 11, 20, 21, 30, 31 og 40 ikke medtaget.
*
* retur = u32_t_kvart(n, e, kvartstg);
*
* double n;   northing i meter                                (call)
* double e;   easting  i meter                                (call)
* char kvartstg[20];  fuld kvartbeteg. f.eks. 1214 iii NE 8  (return)
*
* Returv�rdier :
* -2                : Koordinat ligger udenfor acceptable v�rdier.
* -1                : Transformationsfejl
* 1                 : Koordinat vest for 10xx-s�jlen
* 2                 : Koordinat nord for xx19-r�kken og imellem
*                     10xx og 19xx s�jlerne.
* 3                 : Koordinat syd for xx10-s�jlen og imellem
*                     10xx og 19xx s�jlerne.
* 4                 : Koordinat �st for 19xx-s�jlen.
* 1010111 - 1919449 : Fuld numerisk kvartbetegnelse.
*                     F�rste 4  cifre  : 1cmblad nummer
*                            5. ciffer : 2cmblad nummer
*                            6. ciffer : 4cmblad nummer
*                            7. ciffer : kvart   nummer
*
* Det eksisterende kortbladsystem er udviddet til at sp�nde fra
* 1010 til 1919 s�ledes at utm-koordinaten omregnes til gs, hvorefter
* det unders�ges, om koordinaten ligger �st for 16xx-s�jlen.
* Er dette tilf�ldet omregnes koordinaten istedet til u33.
* 
* For de kortblade, der er rykket fra de systemrigtige placeringer
* (1318, 1416 ii se, 1417 iv) er der taget nogle specielle forholds-
* regler, idet de (s�kortnavne)koordinater, der ligger indenfor det
* systembladsomr�de, som det rigtige blad bliver rykket v�k fra, rykkes
* ud til n�rmeste kvart udenfor, s�ledes at disse ikke vil give samme
* bladreference som de 3 rykkede blade. (se senere i koden)
*
* P� trods af at bladrammerne i u33 er st�rre end i gs viser det sig
* at, n�r kortbladssystemet udviddes til hhv. 1010-1619 (gs) og 
* 1710-1919 (u33) vil projektionssystemerne konvergerer s� meget, at
* der opst�r en �ben kile mellem 16xx i gs og 17xx i u33 spidsende mod
* nord. Koordinater, der ligger i denne trekant, placeres p� n�rmeste
* blad i 17xx-r�kken.
*
* MS 08/92
*/


/* u32_to_map.c                     # page 2   04 Jul 1997 12 34 */



#define FALSE 0
#define TRUE  1

extern int u32_to_map(
double                n,
double                e,
char                  kvartstg[20]
);

