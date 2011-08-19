

/* u32_to_map.h                     # page 1   04 Jul 1997 12 34 */


/* Copyright (c) 1997, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                   */

/* This is unpublished proprietary source code of ITG, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

/* F. Steffensen JUL. 1997 (MI/ITG) */
/* SWN sept. 1997 REK (enkelt rettelse vedr. indlab på Bornholm */

/* u32_to_map.c er en udgave af nedenstående */

/* u32_t_kvart.c
* 
* Funktion til at beregne hvilket kortbladskvartnummmer en 
* koordinat (n,e) i UTM zone 32 opgiver i meter ligger i.
*
* OBS : Særkortene 10, 11, 20, 21, 30, 31 og 40 ikke medtaget.
*
* retur = u32_t_kvart(n, e, kvartstg);
*
* double n;   northing i meter                                (call)
* double e;   easting  i meter                                (call)
* char kvartstg[20];  fuld kvartbeteg. f.eks. 1214 iii NE 8  (return)
*
* Returværdier :
* -2                : Koordinat ligger udenfor acceptable værdier.
* -1                : Transformationsfejl
* 1                 : Koordinat vest for 10xx-søjlen
* 2                 : Koordinat nord for xx19-rækken og imellem
*                     10xx og 19xx søjlerne.
* 3                 : Koordinat syd for xx10-søjlen og imellem
*                     10xx og 19xx søjlerne.
* 4                 : Koordinat øst for 19xx-søjlen.
* 1010111 - 1919449 : Fuld numerisk kvartbetegnelse.
*                     Første 4  cifre  : 1cmblad nummer
*                            5. ciffer : 2cmblad nummer
*                            6. ciffer : 4cmblad nummer
*                            7. ciffer : kvart   nummer
*
* Det eksisterende kortbladsystem er udviddet til at spænde fra
* 1010 til 1919 således at utm-koordinaten omregnes til gs, hvorefter
* det undersøges, om koordinaten ligger øst for 16xx-søjlen.
* Er dette tilfældet omregnes koordinaten istedet til u33.
* 
* For de kortblade, der er rykket fra de systemrigtige placeringer
* (1318, 1416 ii se, 1417 iv) er der taget nogle specielle forholds-
* regler, idet de (søkortnavne)koordinater, der ligger indenfor det
* systembladsområde, som det rigtige blad bliver rykket væk fra, rykkes
* ud til nærmeste kvart udenfor, således at disse ikke vil give samme
* bladreference som de 3 rykkede blade. (se senere i koden)
*
* På trods af at bladrammerne i u33 er større end i gs viser det sig
* at, når kortbladssystemet udviddes til hhv. 1010-1619 (gs) og 
* 1710-1919 (u33) vil projektionssystemerne konvergerer så meget, at
* der opstår en åben kile mellem 16xx i gs og 17xx i u33 spidsende mod
* nord. Koordinater, der ligger i denne trekant, placeres på nærmeste
* blad i 17xx-rækken.
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

