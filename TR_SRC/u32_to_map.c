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
 


/* u32_to_map.c                     # page 1   04 Jul 1997 12 34 */


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


#include <string.h>
#include <stdlib.h>
#include "geo_lab.h"
#include "geoid_d.h"
#include "conv_lab.h"
#include "gd_trans.h"
#include "trthread.h"

#define FALSE 0
#define TRUE  1

/* Globale variable */
int hoejde4cm, bredde4cm;         /* 4cmblad størrelse */

int            u32_to_map(
/*______________________*/
double         n,
double         e,
char           kvartstg[20])
{
  int u33;                          /* flag til projektionssystem */
  int antal4y = 0, antal4x = 0;     /* Antal blade fra nulpunkt */
  int rk1cm, sj1cm;                 /* Række/søjle for 1cm */
  int rk4cm, sj4cm;                 /* 4cm række/søjle indenfor 1cm */
  int blad4;                        /* Fortløbende 4cm nummer på 1cm */
  int cm = 0;                       /* Sammensat numerisk 1cm beteg. */
  int retur = 0;                    /* returværdi fra dktrans */
  double x_ud, y_ud, n_ud, e_ud;    /* hhv. gs og u33 uddata */
  double delta_x, delta_y;          /* kortbladsforskydninger */
  double x_afstand, y_afstand;      /* relativ afstand fra nulpunkt */

  /* Følgende variable er tilføjet d. 16/02/96 for at kunne hånd- */
  /* tere ny version af unitrans til erstatning af dktrans.  civ. */
  /* NOGLE er fjernet igen af FRS 4. juli 1997                    */

  char        inlab[20] = "utm32_ed50";
  char        outlab[20];
  double      dummy;
  char       *p_k;
  int         ch;

  union     geo_lab    i_reg, o_reg;

  /* Følgende variable er tilføjet d. 24/07/00 for at kunne hånd- */
  /* tere ny version af gd_trans til erstatning af unitrans. frs. */
  static THREAD_SAFE  char              G_req = -1;
  double            Gh;
  static THREAD_SAFE  struct mgde_str   grid_tab;



/* u32_to_map.c                     # page 3   04 Jul 1997 12 34 */


  p_k  = kvartstg;
  *p_k = '\0';

  if ((n > 6800000.0 || n < 5800000.0) &&
      (e > 1200000.0 || e <  200000.0))
    return(-2);
  /* Koordinater langt fra kortbladssystem sigtes fra */
  if (e <  370000.0)
    return(1);
  if (e >  990000.0)
    return(4);
  if (n > 6480000.0)
    return(2);
  if (n < 5990000.0)
    return(3);

  /* Der findes et sort (u32->gs ej ok) nø-hjørne på 1619 */
  /* Derfor er tolerancen i utgs.c sat op fra 1 til 3 cm */

  u33  = FALSE;

/* ind_label er altid u32 */

  if (conv_lab(inlab, &i_reg, "") == ILL_LAB) {
      (void) fprintf(stderr,
          "\n***label ej kendt (conv_lab), %s\n", inlab);
     exit(0);
   }

  if (e < 790000.0) {
    (void) strcpy(outlab, "gs");

    if (conv_lab(outlab, &o_reg, "") == ILL_LAB) {
      (void) fprintf(stderr,
          "\n***label ej kendt (conv_lab), %s\n", outlab);
      exit(0);
    }
    retur = gd_trans(&i_reg, &o_reg, n, e, 0.0, &y_ud, &x_ud, &dummy, 
        &Gh, G_req, &grid_tab, "", stderr);

    if (retur) { /* transformation ikke OK */
      return(-1);
    }


/* u32_to_map.c                     # page 4   04 Jul 1997 12 34 */


    if (x_ud >= -197730.343860) /* ikke øst for 16-søjlen */
    {
      if (x_ud > 197730.343860) /* vest for 10-søjlen */
        return(1);
        else
        if (y_ud > 361564.05760) /* nord for 19-rækken */
          return(2);
          else
          if (y_ud < -90391.014400) /* syd for 10-rækken */
            return(3);
          else /* Indenfor 1010 til 1619, beregn ud fra gs */
          {
            bredde4cm = (int) 14123.595990;
            hoejde4cm = (int) 11298.876800;

            delta_x = delta_y = 0.0;

            /* 3 par af flytninger for hhv. bladet, der flyttes, og for
            flytning af de koordinatsæt, der ligger hvor systembladet 
            flyttes fra. */

            /* Flytning for 1318 OG de 2 søjler kvarter øst for */
            if (y_ud <= 316368.550400 &&
                y_ud >= 271173.043200 &&
                x_ud <=  37662.922640 &&
                x_ud >= -28247.191980)
              delta_x = bredde4cm * 2 / 3; /* 2 kvarter mod øst */
              else

              /* Flytning for 1416 ii SE OG 
              de 5 systemrigtige kvarter : 1416 ii SE 3, 6, 7, 8, 9
                   */
              if ((y_ud <= 195847.197866  && /* Selve bladet */
              y_ud >= 184548.321066  &&
                  x_ud <= -65910.114620  &&
                  x_ud >= -80033.710610) ||
                  (y_ud <= 192080.905600  && /* De 5 kvarter */
              y_ud >= 180782.028800  &&
                  x_ud <= -70617.979950  &&
                  x_ud >= -84741.575940)) {
                delta_x = bredde4cm / 3; /* 1 kvart mod sydøs
                    t */
                delta_y = -hoejde4cm / 3;
              }
            else

                /* Flytning for 1417 iv 
                OG de 2 rækker kvarter nord for */
                if (y_ud <= 271173.043100 &&
                    y_ud >= 241042.705068 &&
                    x_ud <= -28247.191980 &&
                    x_ud >= -56494.383960) {
                  /* 2 kvarter mod nord */
                  delta_y = hoejde4cm * 2 / 3;
                }


/* u32_to_map.c                     # page 5   04 Jul 1997 12 34 */


            /* Her placeres koden til at finde        */
            /*     1010 - 1619 kortbladet             */

            /* Nulpunkt for 4cmblad-matrice lægges i  */
            /*     sv-hjørnet af 1010                 */
            /* svY = -90391.014400 meter,             */
            /* svX = 197730.343860 meter              */

            y_afstand =  y_ud + delta_y +  90391.014400;
            x_afstand = -x_ud + delta_x + 197730.343860;

            /* Bestemmelse af relativ 4cmblad-position (heltal + 1) */
            antal4y  = (int)(y_afstand / hoejde4cm + 1);
            antal4x  = (int)(x_afstand / bredde4cm + 1);

            sj1cm    = (antal4y - 1) / 4;
            rk1cm    = (antal4x - 1) / 4;

            /* 1cm bladnummer */
            cm       = (10 + rk1cm) * 100 + 10 + sj1cm;
          }
    }
    else
      u33 = TRUE;
  }
  else
    u33 = TRUE;


/* u32_to_map.c                     # page 6   04 Jul 1997 12 34 */


  if (u33) {
    /* Hvis indenfor 1710 til 1919 beregn da kortblad ud fra u33 */

    /* (void) strcpy(inlab, "u33_ed50"); 
    * (void) strcpy(outlab, "gs");  
    * Alle koordinater kommer idag ind i u32.   Civ 3/96
    */

    (void) strcpy(inlab, "utm32_ed50");
    /*
    (void) strcpy(outlab, "gsb_gsb");
    */
    (void) strcpy(outlab, "utm33_ed50");

    if (conv_lab(outlab, &o_reg, "") == ILL_LAB) {
      (void) fprintf(stderr,
          "\n***label ej kendt (conv_lab), %s\n", outlab);
      return(-1);
    }

    retur = gd_trans(&i_reg, &o_reg, n, e, 0.0, &n_ud, &e_ud, &dummy, 
        &Gh, G_req, &grid_tab, "", stderr);

    if (retur) {  /* transformation ikke OK */
      (void) fprintf (stderr, "retur : %d %7.5f %7.5f\n", 
          retur, n_ud, e_ud);
      return(-1);
    }

    if (e_ud >  594200.000000)
      return(4);
    if (n_ud > 6448400.000000)
      return(2);
    if (n_ud < 5996400.000000)
      return(3);

    /* Hvis e_ud bliver mindre end 424400 meter, d.v.s. at koordina- */
    /* ten ligger i trekanten mellem 16xx(gs) og 17xx(u33), sættes   */
    /* den hårdt til netop at være inden for 17xx-søjlen. */

    if (e_ud <  424400.000000)
      e_ud =    424401.000000;

    hoejde4cm = (int)11300.000000;
    bredde4cm = (int)14150.000000;

    /* Nulpunkt for 4cmblad-matrice lægges i sydvesthjørnet af 1710 */
    /* svN = 5996400.000000 meter, svE = 424400.000000 meter (u33)  */
    y_afstand = n_ud - 5996400.000000;
    x_afstand = e_ud -  424400.000000;

    /* Bestemmelse af relativ 4cmblad-position (heltal + 1) */
    antal4y  = (int)(y_afstand / hoejde4cm + 1);
    antal4x  = (int)(x_afstand / bredde4cm + 1);

    sj1cm    = (antal4y - 1) / 4;
    rk1cm    = (antal4x - 1) / 4;

    /* 1cm bladnummer */
    cm       = (17 + rk1cm) * 100 + 10 + sj1cm;
  }


/* u32_to_map.c                     # page 7   04 Jul 1997 12 34 */


  /* Lægger cm ned i kvartstg som streng */

  ch   = sprintf(p_k, "%d", cm);
  p_k += ch;

  /* Fælles beregning 2cm, 4cm og kvart */
  sj4cm    = (antal4y - 1) % 4 + 1;
  rk4cm    = (antal4x - 1) % 4 + 1;

  /* Fortløbende 4cm , horisontalt fra nederste venstre hjørne */
  blad4    = (sj4cm - 1) * 4 + rk4cm;

  switch (blad4) {
  case  0:
    break;
  case  1:
    (void) strcat(p_k, " III SV");
    retur = cm * 100 + 33;
    break;
  case  2:
    (void) strcat(p_k, " III SØ");
    retur = cm * 100 + 32;
    break;
  case  3:
    (void) strcat(p_k, " II SV");
    retur = cm * 100 + 23;
    break;
  case  4:
    (void) strcat(p_k, " II SØ");
    retur = cm * 100 + 22;
    break;
  case  5:
    (void) strcat(p_k, " III NV");
    retur = cm * 100 + 34;
    break;
  case  6:
    (void) strcat(p_k, " III NØ");
    retur = cm * 100 + 31;
    break;
  case  7:
    (void) strcat(p_k, " II NV");
    retur = cm * 100 + 24;
    break;
  case  8:
    (void) strcat(p_k, " II NØ");
    retur = cm * 100 + 21;
    break;
  case  9:
    (void) strcat(p_k, " IV SV");
    retur = cm * 100 + 43;
    break;
  case 10:
    (void) strcat(p_k, " IV SØ");
    retur = cm * 100 + 42;
    break;


/* u32_to_map.c                     # page 7   04 Jul 1997 12 34 */


  case 11:
    (void) strcat(p_k, " I SV");
    retur = cm * 100 + 13;
    break;
  case 12:
    (void) strcat(p_k, " I SØ");
    retur = cm * 100 + 12;
    break;
  case 13:
    (void) strcat(p_k, " IV NV");
    retur = cm * 100 + 44;
    break;
  case 14:
    (void) strcat(p_k, " IV NØ");
    retur = cm * 100 + 41;
    break;
  case 15:
    (void) strcat(p_k, " I NV");
    retur = cm * 100 + 14;
    break;
  case 16:
    (void) strcat(p_k, " I NØ");
    retur = cm * 100 + 11;
    break;
  }

  /* FRS, alt om kvart fjernet juli 1997 */
  return(retur);
}

