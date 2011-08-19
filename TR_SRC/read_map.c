

/* read_map.c    version 1.0        # page 1   18 Oct 2000 10 01 */


/* Copyright (c) 1997, Kort & Matrikelstyrelsen, Denmark */
/* All rights reserved.                                  */

/* This is unpublished proprietary source code of ITG, Kort & */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not */
/* indicate an intention of publishing this code.             */

/* Prog.: F. Steffensen NOV. 1997 (MI/ITG)  */
/*   Oversat fra algol til C                */
/*   Hjørnekoordinater for CM_MAP, MV_MAP   */
/* Prog.: F. Steffensen OKT. 1998 (MI/ITK)  */
/*   EE_MAP indsat                          */
/* Prog.: F. Steffensen MAR. 2000 (MI/ITK)  */
/*   Hjørnekoordinater for GR_MAP indsat,   */
/*   EE_MAP fjernet, se readEEmap.c         */
/* Prog.: F. Steffensen OKT. 2000 (MI/ITK)  */
/*   MV_MAP udvidet med Peberholm.          */
/*   kolonne z øst for 0                    */
/* Prog.: F. Steffensen APR. 2004 (ITO)     */
/*   Opdateret minilabels.                  */
/* Prog.: F. Steffensen MAJ. 2006 (ITO)     */
/*   CM_MAP 1613 IV SALTHOLM                */
/*   flyttet 5 km mod syd, aftale LAN       */

/* Function:    */

/* read_map     (kald og retur)         int                    */
/*              Læser kortbladsnummer fra strengen in, og      */
/*              returnere kortbladstypen.                      */

/* in           (kald)                  char *                 */
/*              Streng der indeholder kortbladsnummer.         */

/* map_str      (retur)                 char *                 */
/*              Pjat, pænt formateret streng indeholdende      */
/*              kortbladsnummeret, op til 24 karakterer lang.  */
/*              Kan anvendes ved udskrivning af kort-nummeret. */

/* map_lab      (retur)                 union geo_lab *        */
/*              Label for returnerede koordinater.             */

/* c_poly       (retur)                 double *               */
/*              Hjørnekoordinater for kortbladsnummer.         */
/*              c_poly skal erklæres med mindst 9 elementer    */

/* Denne funktion er baseret på gamle algol map-procedure */
/* Ved hver kort-type er kommentar vedrørende oprindelse, */
/* virkemåde samt hvilke personer der har givet bidrag    */
/* bibeholdt.                                             */
/* Konstanter og beregningsmetode er som de gamle algol-  */
/* procedurer, men ikke benyttede ting er fjernet.        */

#include  <stdio.h>
#include  <string.h>
#include  <ctype.h>
#include  <sys/types.h>
#include  <math.h>
#include  "geo_lab.h"
#include  "conv_lab.h"

#ifndef    M_PI
#include  "kms_math.h"
#endif


/* read_map.c    version 1.0        # page 2   18 Oct 2000 10 01 */


int               read_map(
char                      *in,
int                       *used,
char                      *map_str,
union geo_lab             *map_lab,
double                    *c_poly
)
{

  char         *p, *txt, *pi, i_str[24];
  size_t        len;
  int           t, u;
  int           ch = 0, j, c, ok;
  int           ch1, ch2, ch3, ch4;
  int           ar1, ar2, ar3, ar4, typ, state;
  int           art, n1, n2, n3, n4;

  int           bornholm;
  int           nr, pf;
  double        cm4_y, cm4_x, hy, hx;
  double        x0, y0, x, y, dx = 0.0, dy = 0.0;
  double        poly[9];

  /* heltalsdelen i funktionen modf() */
  double        dbli;

  /* diverse korrektioner i y og x */
  double        cy_t = 0.0, cy_f = 0.0, cy_k = 0.0;
  double        cx_e = 0.0, cx_t = 0.0, cx_f = 0.0, cx_k = 0.0, cx_b;

  struct coord_lab *c_lab =&(map_lab->u_c_lab);

#define       NO_MAP  -1  /*  Tom                 */
#define       TX_MAP   0  /*  Ej genkendt tekst   */
#define       CM_MAP   1  /*  CM kort             */
#define       MV_MAP   2  /*  MD's kortbladsindd. */
#define       GR_MAP   3  /*  Grønlandske kort    */
#define       FA_MAP   4  /*  Færøske kort        */
#define       DI_MAP   5  /*  Diverse syd-danmark */
#define       MA_MAP   6  /*  Målebord            */
#define       AT_MAP   7  /*  Atlasblade          */


/* read_map.c    version 1.0        # page 3   18 Oct 2000 10 01 */


  /* Tom streng:   type = NO_MAP */

  /* Ikke afkodet: type = TX_MAP */

  /* CMKORT:  type = CM_MAP                          */
  /* ar1: 1CM kortnummer                             */
  /* ar2: 2CM betegnelse (0 hvis 1CM kort)           */
  /* ar3: 4CM betegnelse (0 hvis 1- eller 2CM kort)  */
  /* _     1:NØ, 2:SØ, 3:SV 4:NV                     */
  /* ar4: kvartnummer (0 hvis 1-, 2- eller 4CM kort) */
  /* _     1, 2, 3, ..... 9                          */

  /* MV's kortbladsinddeling:  type = MV_MAP      */
  /* ar1: 'a' ... 't' (excl. 'i')                 */
  /* ar2: 0 .. 9, z   (blokinddeling,  1 : 40000) */
  /*              Øst for ar2 = 0 betegnes mv-    */
  /*              bladene med bogstaver startende */
  /*              med z, y osv                    */
  /* ar3: 100                                     */

  /* ar3: 00 .. 99    (blokinddeling,  1 : 4000)  */
  /* ar4: 0                                       */

  /* ar4: 1 .. 4      (blokinddeling,  1 : 2000)  */

  /* ar4: 11 .. 14,   (blokinddeling,  1 : 1000)  */
  /* ar4: 21 .. 24,   (blokinddeling,  1 : 1000)  */
  /* ar4: 31 .. 34,   (blokinddeling,  1 : 1000)  */
  /* ar4: 41 .. 44    (blokinddeling,  1 : 1000)  */

  /* GRØNLAND:  type = GR_MAP                */
  /* ar4: 'g'                                */

  /* for G/100:                              */
  /* ar1: gradbetegnelse                     */
  /* ar2: 1  : Ø1, 2  : Ø2, .......          */
  /* _    101: V1, 102: V2, .......          */
  /* ar3: 101: NØ, 102: SØ, 103: SV, 104: NV */

  /* for G/250:                              */
  /* ar1: gradbetegnelse                     */
  /* ar2: 1  : Ø1, 2  : Ø2, .......          */
  /* _    101: V1, 102: V2, .......          */
  /* ar3: 250                                */


/* read_map.c    version 1.0        # page 4   18 Oct 2000 10 01 */


  /* FÆRØERNE:  type = FA_MAP */
  /* ar4: 'f'                 */

  /* for F/20:                */
  /* ar1: 0     hele bladet   */

  /* ar1: 1,    NØ-kvart      */
  /* ar1: 2,    SØ-kvart      */
  /* ar1: 3,    SV-kvart      */
  /* ar1: 4,    NV-kvart      */
  /* ar2: 20                  */
  /* ar3: målebordsnummer     */

  /* for F/100:               */
  /* ar2: 100                 */
  /* ar3: 1     nordlige blad */
  /* ar3: 2     sydlige blad  */

  /* for F/200:               */
  /* ar2: 200                 */
  /* ar3: 1                   */

  /* DIVERSE (SYD-DANMARK):  type = DI_MAP                        */
  /* ar4: 'd'                                                     */

  /* for D/200:                                                   */
  /* ar2: 200                                                     */
  /* ar3: 10: blad 1, 20: blad 2, 21: Anholt, indfældet           */
  /* _     30: blad 3, 40: blad 4, 41: Bornholm, indfældet        */
  /* _     3201 -> 3204: blad 1 -> 4 i zone 32 til nyt D/200 kort */
  /* _     3205 Anholt indfældet i blad 3202   til nyt D/200 kort */
  /* _     3301 Bornholm indfældet i blad 3201 til nyt D/200 kort */
  /* _     101 -> 118: siderne 1 -> 18         til nyt D/200 kort */

  /* for D/250, 1501-serien:                                     */
  /* ar1: 1: no,  2: nn                                          */
  /* ar2: 250                                                    */
  /* ar3: 32-9: 3209,  32-11: 3211,  32-12: 3212,  32-2: 3202,   */
  /* _     32-3: 3203,  33-1:  3301,  32-5:  3205,  32-6: 3206,  */
  /* _     33-4: 3304                                            */

  /* for D/300:                                                  */
  /* ar2: 300                                                    */
  /* ar3: 10: hovedblad, 11: Bornholm, indfældet                 */

  /* for D/500:                                                  */
  /* ar2: 500                                                    */
  /* ar3: 10                                                     */


/* read_map.c    version 1.0        # page 5   18 Oct 2000 10 01 */


  /* MÅLEBORD:  type = MA_MAP   */
  /* ar4: 'm'                   */
  /* ar2: målebordsnummer       */

  /* ar3: 0     hele målebordet */

  /* ar3: 1,    NØ-kvarten      */
  /* ar3: 2,    SØ-kvarten      */
  /* ar3: 3,    SV-kvarten      */
  /* ar3: 4,    NV-kvarten      */

  /* ATLASBLAD:  type = AT_MAP */
  /* ar4: 'a'                  */
  /* ar2: atlasbladnummer      */
  /* ar3: 0                    */

  /* init */
  poly[0]      = 0.0;
  poly[1]      = 0.0;
  poly[2]      = 0.0;
  poly[3]      = 0.0;
  poly[4]      = 0.0;
  poly[5]      = 0.0;
  poly[6]      = 0.0;
  poly[7]      = 0.0;
  poly[8]      = 0.0;
  conv_lab("geo_euref89", map_lab, "");
  *(c_poly +0) = 0.0;
  *(c_poly +1) = 0.0;
  *(c_poly +2) = 0.0;
  *(c_poly +3) = 0.0;
  *(c_poly +4) = 0.0;
  *(c_poly +5) = 0.0;
  *(c_poly +6) = 0.0;
  *(c_poly +7) = 0.0;
  *(c_poly +8) = 0.0;

  *used  = 0;
  len    = strlen((const char *) in);
  j      = (int) len;
  txt    = in;

  ar1    = 0;
  ar2    = 0;
  ar3    = 0;
  ar4    = 0;
  ok     = 0;

  if (j <= 1)
    return(NO_MAP);


/* read_map.c    version 1.0        # page 6   18 Oct 2000 10 01 */


  /* convert to upper-case, and remove !(alpha or digits) */
  p           = in;
  pi          = i_str;
  *pi         = '\0';
  while (((c  = *(p++)) != '\0') && j--) {
    switch (c) {
    case 'ø' :
    case 'Ø' :
      *(pi++) = 'E';
      break;

    case '/' :
      *(pi++) = '/';
      break;

    case '.' :
      *(pi++) = '.';
      break;

    default :
      if (C_DIGIT(c) || C_ALPHA(c))
        *(pi++) = (char) toupper(c);
      break;
    }
  }
  *pi         = '\0';
  (void) sprintf(map_str, "%s ", i_str);

  /* Funktionen læser et kortblads_nummer fra tekst strengen in. */
  /* For at kunne se forskel på de forskellige kortblade, skal   */
  /* første eller de to første karakterer være:                  */

  /* Atlasblade og målebordsblade (AT_MAP og MA_MAP) er fjernet  */
  /* fra koden, og vil derfor ikke kunne genkendes.              */
  /* Matrikeldirektoratets kortblade kan pga. ovenstående gen-   */
  /* kendes uden de indledende 'mv' eller 'md'                   */

  /* d eller D     for Diverse DK                      */
  /* f eller F     for Færøske kortblade               */
  /* g eller G     for Grønlandske kortblade           */
  /* mv eller MV   for Matrikeldirektoratets kortblade */
  /* md eller MD              do                       */

  /* Alle små bogstaver er konverteret til upper.  */


/* read_map.c    version 1.0        # page 7   18 Oct 2000 10 01 */


  /* Map type kan bestemmes af de tre */
  /* eller fire første karakterer     */
  p   =   i_str;
  ch1 = *(p +0);
  ch2 = *(p +1);
  ch3 = *(p +2);
  ch4 = *(p +3);
  typ =  TX_MAP;

  /* CM_MAP : CM kort */
  if (C_DIGIT(ch1) && C_DIGIT(ch2) && C_DIGIT(ch3) && C_DIGIT(ch4)) {
    typ = CM_MAP;
    sscanf(p, "%4d%n", &ar1, &ch);
    p  += ch;
    ok  = 1;
  }

  /* MV_MAP : MD's kortbladsindd. */
  if ((C_ALPHA(ch1) && (C_DIGIT(ch2) || ch2 == 'Z')) ||
      (ch1 == 'M' && (ch2 == 'V' || ch2 == 'D') && C_ALPHA(ch3))) {
    typ    = MV_MAP;

    /*  fjern 'MD' eller 'MV' */
    if (ch1 == 'M' && (ch2 == 'V' || ch2 == 'D'))
      p     += 2;

    if (C_ALPHA(*p)) {
      ar1        = *p;
      if (!('A' <= ar1 && ar1 <= 'T') || ar1 == 'I')
        typ      = TX_MAP;
    }
    else 
      typ    = TX_MAP;

    if (typ != TX_MAP) {
      p++;

      /* ar2 == 42 for 'Z' */
      if (C_DIGIT(*p) || *p == 'Z')
        ar2  = *p - '0';
      else 
        typ  = TX_MAP;
    }
    if (typ != TX_MAP) {
      p++;

      if (*p == '\0') {
        ar3   = 100;
        ok    = 1;
      }
      else {
        if (C_DIGIT(*p) && C_DIGIT(*(p +1))) {
          sscanf(p, "%2d%n", &ar3, &ch);
          p  += ch;
        }
        else 
          typ = TX_MAP;
      }
    }


/* read_map.c    version 1.0        # page 8   18 Oct 2000 10 01 */


    if (!ok && typ == MV_MAP) {
      if (C_DIGIT(*p)) {
        sscanf(p, "%d", &ar4);
        p += ch;
      }
    }
  }

  /* GR_MAP : Grønlandske kort */
  if (ch1 == 'G' && ch2 == '/' && C_DIGIT(ch3)) {
    typ    =  GR_MAP;
    p     +=  2;
    ar4    = 'G';
    sscanf(p, "%3d%n", &ar3, &ch);
    p     += ch;
  }

  /* FA_MAP : Færøske kort */
  if (ch1 == 'F' && ch2 == '/' && C_DIGIT(ch3)) {
    typ    =  FA_MAP;
    p     +=  2;
    ar4    = 'F';
    sscanf(p, "%d%n", &ar2, &ch);
    p     += ch;
  }

  /* DI_MAP : Diverse DK */
  if (ch1 == 'D' && ch2 == '/' && C_DIGIT(ch3)) {
    typ    =  DI_MAP;
    p     +=  2;
    ar4    = 'D';
    sscanf(p, "%d%n", &ar2, &ch);
    p     += ch;
  }

  state  = typ;


/* read_map.c    version 1.0        # page 9   18 Oct 2000 10 01 */


  /* Kort_blads type er fastlagt, resten læses   */
  /* korrektion af type kan ske.                 */
  /* MV_MAP, MA_MAP og AT_MAP er færdigbehandlet */

  /* DI_MAP behandles ikke videre                */

  switch (state) {

  case GR_MAP :  /* Grønlandske kort */
    if (*p == '\n' || *p == '\0') {
      typ   = TX_MAP;
      break;
    }

    /* læs gradbetegnelse */
    if (C_DIGIT(*p) && C_DIGIT(*(p +1))) {
      sscanf(p, "%2d%n", &ar1, &ch);
      p += ch;
    }
    else {
      typ  = TX_MAP;
      break;
    }

    /* læs øst eller vest */
    if (C_DIGIT(*(p +1)) &&
        (*p  == 'E' || *p == 'V' || *p == 'W')) {
      if (*p == 'V' || *p == 'W')
        ar2   = 100;
      p++;
      sscanf(p, "%1d%n", &t, &ch);
      ar2    += t;
      p      += ch;
    }
    else {
      typ   = TX_MAP;
      break;
    }

    /*
    if ((ar1 < 59 && ar2 > 100) || (ar1 < 60 && ar2 < 100)) {
    typ  = TX_MAP;
    break;
    }
    */

    if (ar3 == 250) break;


/* read_map.c    version 1.0        # page 10   18 Oct 2000 10 01 */


    /* læs kvarten */
    if (C_ALPHA(*p)) {
      if (!strncmp(p, "NE", 2))
        ar3 = 101;
        else
        if (!strncmp(p, "SE", 2))
          ar3 = 102;
          else
          if (!strncmp(p, "SV", 2) || !strncmp(p, "SW", 2))
            ar3 = 103;
            else
            if (!strncmp(p, "NV", 2) || !strncmp(p, "NW", 2))
              ar3 = 104;
            else 
              typ = TX_MAP;
    }
    else 
      typ  = TX_MAP;

    break;

  case FA_MAP :  /* Færøske kort */
    if (*p == '\n' || *p == '\0') {
      typ   = TX_MAP;
      break;
    }

    if (C_DIGIT(*p)) {
      sscanf(p, "%d%n", &ar3, &ch);
      p += ch;
    }
    if (*p == '\n' || *p == '\0')
      break;

    /* læs kvarten */
    if (C_ALPHA(*p)) {
      if (!strncmp(p, "NE", 2))
        ar1 = 1;
        else
        if (!strncmp(p, "SE", 2))
          ar1 = 2;
          else
          if (!strncmp(p, "SV", 2) || !strncmp(p, "SW", 2))
            ar1 = 3;
            else
            if (!strncmp(p, "NV", 2) || !strncmp(p, "NW", 2))
              ar1 = 4;
    }
    break;


/* read_map.c    version 1.0        # page 11   18 Oct 2000 10 01 */


  case CM_MAP :  /* CM kort */

    if (!(1000 < ar1 && ar1 < 2000)) {
      typ      = TX_MAP;
      break;
    }

    /* len opdateres for læsning af resten  */
    len   = strlen((const char *) p);
    j     = (int) len;

    /* læs 2 cm kortdel, hvis den er der */
    while (j) {

      /* IV */
      if (*(p +0) == 'I' && *(p +1) == 'V') {
        ar2   = 4;
        p    += 2;
        j     = 0;
        continue;
      }
      if (*(p +0) == 'I' && *(p +1) == 'I'  && 
          *(p +2) == 'I' && *(p +3) == 'I') {
        ar2   = 4;
        p    += 4;
        j     = 0;
        continue;
      }
      if (*(p +0) == '4' ) {
        ar2   = 4;
        p    += 1;
        j     = 0;
        continue;
      }

      /* III */
      if (*(p +0) == 'I' && *(p +1) == 'I'  && *(p +2) == 'I') {
        ar2   = 3;
        p    += 3;
        j     = 0;
        continue;
      }
      if (*(p +0) == '3' ) {
        ar2   = 3;
        p    += 1;
        j     = 0;
        continue;
      }


/* read_map.c    version 1.0        # page 12   18 Oct 2000 10 01 */


      /* II */
      if (*(p +0) == 'I' && *(p +1) == 'I') {
        ar2   = 2;
        p    += 2;
        j     = 0;
        continue;
      }
      if (*(p +0) == '2' ) {
        ar2   = 2;
        p    += 1;
        j     = 0;
        continue;
      }

      /* I */
      if (*(p +0) == 'I') {
        ar2   = 1;
        p    += 1;
        j     = 0;
        continue;
      }
      if (*(p +0) == '1' ) {
        ar2   = 1;
        p    += 1;
        j     = 0;
        continue;
      }
    }   /* while (j) */

    /* len opdateres for læsning af resten  */
    len   = strlen((const char *) p);
    j     = (int) len;

    /* læs 4 cm kortdel, hvis den er der */
    while (j) {

      /* NE */
      if (*(p +0) == 'N' && *(p +1) == 'E') {
        ar3   = 1;
        p    += 2;
        j     = 0;
        continue;
      }
      if (*(p +0) == '1') {
        ar3   = 1;
        p    += 1;
        j     = 0;
        continue;
      }


/* read_map.c    version 1.0        # page 13   18 Oct 2000 10 01 */


      /* SE */
      if (*(p +0) == 'S' && *(p +1) == 'E') {
        ar3   = 2;
        p    += 2;
        j     = 0;
        continue;
      }
      if (*(p +0) == '2') {
        ar3   = 2;
        p    += 1;
        j     = 0;
        continue;
      }

      /* SV */
      if (*(p +0) == 'S' && (*(p +1) == 'V' || *(p +1) == 'W')) {
        ar3   = 3;
        p    += 2;
        j     = 0;
        continue;
      }
      if (*(p +0) == '3') {
        ar3   = 3;
        p    += 1;
        j     = 0;
        continue;
      }

      /* NV */
      if (*(p +0) == 'N' && (*(p +1) == 'V' || *(p +1) == 'W')) {
        ar3   = 4;
        p    += 2;
        j     = 0;
        continue;
      }
      if (*(p +0) == '4') {
        ar3   = 4;
        p    += 1;
        j     = 0;
        continue;
      }
    }
    /* læs kvarten */
    if (C_DIGIT(*p))
      sscanf(p, "%d%n", &ar4, &ch);

    break;


/* read_map.c    version 1.0        # page 14   18 Oct 2000 10 01 */


  case DI_MAP :  /* Diverse syd-danmark */

    break;

  case TX_MAP :  /* Text, ikke afkodet */

    /* type TX_MAP kan indeholde op til 20 karakterer og \0 */
    p          = i_str;
    u          = 20;
    while (((c = *(p++)) != '\0') && (u-- && !isspace(c)))
      *txt++   = (char) c;
    *txt       = '\0';
    p--;

    /* read to first terminating character */
    while (((c = *(p++)) != '\0') && (!isspace(c)))
      ;
    break;

  default :
    break;

  }  /* switch (state) */
  /* Kort_blads type er fastlagt, resten læses */
  state  = typ;

  /*
  (void) fprintf(stdout,
  "\ntype = %3d  ar1 = %4d  ar2 = %4d  ar3 = %4d  ar4 = %4d",
  typ, ar1, ar2, ar3, ar4);
  */


/* read_map.c    version 1.0        # page 15   18 Oct 2000 10 01 */


  switch (state) {

  case MV_MAP :  /* MV's kortblad */

    /* Proceduren beregner koordinaterne til det aktuelle             */
    /* kortblads hjørnepunkter (polygonpunkter).                      */

    /* Programmør Erik Hansen.                                        */
    /* Første version  APRIL 1976.                                    */
    /*      opdateret  DECEMBER 1977 til brug i ny proc map_ramme     */
    /*      opdateret 12-07-78. Transformationer flyttet fra denne    */
    /*                          procedure til map_ramme. Desuden er   */
    /*                          parameter <lab> ændret til en return  */
    /*                          parameter.                            */
    /*      opdateret 27-06-80. 2 CM særkort Bornholm implementeret   */
    /*      opdateret 02-01-84. Passer nu bedre sammen med u32_to_map */

    if (ar1 >= 'Q' && ar2 <= 2) {  /* Bornholm */
      x0     =  3000000000.0;
      y0     = 12000000000.0 -300000000000.0;
    }
    else {
      x0     = 60000000000.0;
      y0     = 0.0;
    }

    /* Hvilken type kort er det? */
    art = ar3 == 100 ? 1 : 
        ar4   ==   0 ? 2 : 
        ar4    <   5 ? 3 : 4;

    /* Beregn sidelængderne til kortet */

    switch (art) {
    case 1 :
      dx  = 30000000000.0;
      dy  = 20000000000.0;
      break;
    case 2 :
      dx  =  3000000000.0;
      dy  =  2000000000.0;
      break;
    case 3 :
      dx  =  1500000000.0;
      dy  =  1000000000.0;
      break;
    case 4 :
      dx  =   750000000.0;
      dy  =   500000000.0;
      break;
    }


/* read_map.c    version 1.0        # page 16   18 Oct 2000 10 01 */


    /* Udpak cifrene i ar3 og ar4 */

    n1  = (ar3 / 10) % 10;
    n2  =  ar3 % 10;

    n3  =  ar4 < 5 ? ar4 : (ar4 / 10) % 10;
    n4  =  ar4 < 5 ?   0 : (ar4 % 10);

    /* Find den rigtige 'minilabel' */

    if (ar1 >= 'Q' && ar2 <= 2)
      conv_lab("s45b", map_lab, "");
      else
      if ((ar1 <= 'K' && (ar2 <= 3 || ar2 == 42)) ||
          (ar1 == 'N' &&  ar2 == 2))
        conv_lab("s34s", map_lab, "");
        else
        conv_lab("s34j", map_lab, "");

    /* Lidt justering omkring Langeland */

    if (!strcmp(c_lab->mlb, "s34s") &&
        ((ar1 == 'C' && ar2 == 3 && n2 > 7) ||
        ( ar1 == 'D' && ar2 == 3 && n2 > 5) ||
        ( ar1 == 'E' && ar2 == 3 && n2 > 7) ||
        ( ar1 == 'E' && ar2 == 3 && n2 > 5 && n1 < 3)))
      conv_lab("s34j", map_lab, "");

    /* Beregn koordinaterne til kortets sø-hjørne */

    j       =  ar1;
    if (ar1 > 'I')
      j   = j  - 1;  /* Bogstav 'i' findes ikke */

    /* HER ER JEG I TVIVL */
    if (ar2 == 42)
      x      = x0 -30000000000.0;
      else
      x      = x0 +30000000000.0 * ar2;
    y        = y0 +20000000000.0 * (j - 'A');

    if (ar3 != 100) {   /* ikke 1:40_000 */
      x = x +3000000000.0 * n2;
      y = y +2000000000.0 * n1;


/* read_map.c    version 1.0        # page 17   18 Oct 2000 10 01 */


      if (n3 != 0) {  /* ikke 1:4_000 */
        switch (n3) {
        case 1 :
          x = x +0.0;
          y = y +0.0;
          break;
        case 2 :
          x = x +1500000000.0;
          y = y +0.0;
          break;
        case 3 :
          x = x +0.0;
          y = y +1000000000.0;
          break;
        case 4 :
          x = x +1500000000.0;
          y = y +1000000000.0;
          break;
        }
      }

      if (n4 != 0) {  /* 1:1_000 */
        switch (n4) {
        case 1 :
          x = x +0.0;
          y = y +0.0;
          break;
        case 2 :
          x = x +750000000.0;
          y = y +0.0;
          break;
        case 3 :
          x = x +0.0;
          y = y +500000000.0;
          break;
        case 4 :
          x = x +750000000.0;
          y = y +500000000.0;
          break;
        }
      }
    }

    /* Overfør koordinaterne til poly */
    *(c_poly +0) = 4.0;
    *(c_poly +1) = (y +dy) /1000000.0;
    *(c_poly +2) = (x +dx) /1000000.0;
    *(c_poly +3) = (y +dy) /1000000.0;
    *(c_poly +4) = (x    ) /1000000.0;
    *(c_poly +5) = (y    ) /1000000.0;
    *(c_poly +6) = (x    ) /1000000.0;
    *(c_poly +7) = (y    ) /1000000.0;
    *(c_poly +8) = (x +dx) /1000000.0;

    break;


/* read_map.c    version 1.0        # page 18   18 Oct 2000 10 01 */


  case GR_MAP :  /* Grønlandske kort */

    /* gammel algol procedure grønramme */

    /* Proceduren beregner koordinaterne til det aktuelle kortblads */
    /* hjørnepunkter (polygonpunkter).                              */
    /* NB!!! Kun Nordgrønland implementeret.                        */

    /* Programmør Erik Hansen, Topografisk Afdeling 10-07-79.       */
    /* Opdateret 25-05-81: datum ændret fra WGS til NAD.            */
    /* Opdateret 08-12-81: external nl fjernet.                     */
    /* Opdateret 30-12-81: 80 (Ø) -> 83 (Ø) nye koordinater         */
    /* Opdateret 05-02-82: 81 (V) -> 82 (V) nye koordinater         */
    /* Opdateret 03-02-83: 78 (V) -> 80 (V) nye koordinater         */
    /* Opdateret 03-01-84: 80 V1 og V2 nye koordinater              */
    /* Opdateret 02-09-87: 77 (Ø) -> 80 (Ø) nye koordinater   /haj  */
    /* Opdateret 09-02-88: 59 (V) -> 77 (V) implementeret           */
    /* _                   60 (Ø) -> 75 (Ø) implementeret           */
    /* Udregningen af længde- og breddegrad giver fejl på           */
    /* 6. decimal i forhold til sekunder.                     /haj  */
    /* Fejlretning 11-06-90 ang.: minilab ggeon i g/100 område      */
    /* ggeo i resten af Grønland      OP                            */

    /*
    if ((ar1 >= 76 && ar2 < 100) ||
    (ar1 >= 78 && ar2 > 100))
    conv_lab("ggeon", map_lab, "");
    else
    conv_lab("ggeo", map_lab, "");
    */
    conv_lab("geo_euref89", map_lab, "");


/* read_map.c    version 1.0        # page 19   18 Oct 2000 10 01 */


    /* beregning af hjørnekoordinater til 1/250_000 */
    poly[0] = 4.0;
    poly[3] = M_PI / 180.0 *(ar1 + 1);
    poly[7] = M_PI / 180.0 * ar1;
    poly[1] = poly[3];
    poly[5] = poly[7];

    switch (ar1) {  /* gradbetegnelse */
    case 59 :
      /* V1 */
      poly[2] = M_PI /180.0 * -45.5;
      poly[4] = poly[2] +(M_PI /180.0 *(2.0 +45.0/60.0));
      break;

    case 60 :
      switch (ar2) {
      case 1 :  /* Ø1 */
        poly[2] = M_PI /180.0 * -44.0;
        poly[4] = poly[2] +(M_PI /180.0 * 2.0);
        break;
      case 101 :  /* V1 */
        poly[2] = M_PI /180.0 * -(48.0 +35.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 *(2.0 +35.0/60.0));
        break;
      case 102 :  /* V2 */
        poly[2] = M_PI /180.0 * -46.0;
        poly[4] = poly[2] +(M_PI /180.0 * 2.0);
        break;
      }

      break;

    case 61 :
      switch (ar2) {
      case 1 :  /* Ø1 */
        poly[2] = M_PI /180.0 * -44.0;
        break;
      case 101 :  /* V1 */
        poly[2] = M_PI /180.0 * -50.0;
        break;
      case 102 :  /* V2 */
        poly[2] = M_PI /180.0 * -48.0;
        break;
      case 103 :  /* V3 */
        poly[2] = M_PI /180.0 * -46.0;
        break;
      }
      poly[4] = poly[2] +(M_PI /180.0 * 2.0);

      break;


/* read_map.c    version 1.0        # page 20   18 Oct 2000 10 01 */


    case 62 :
      switch (ar2) {
      case 1 :  /* Ø1 */
        poly[2] = M_PI /180.0 * -(43.0 +35.0/60.0);
        break;
      case 101 :  /* V1 */
        poly[2] = M_PI /180.0 * -50.75;
        break;
      }
      poly[4] = poly[2] +(M_PI /180.0 *(2.0 +10.0/60.0));

      break;

    case 63 :
      switch (ar2) {
      case 1 :  /* Ø1 */
        poly[2] = M_PI /180.0 * -42.0;
        poly[4] = poly[2] +(M_PI /180.0 * 2.0);
        break;
      case 2 :  /* Ø2 */
        poly[2] = M_PI /180.0 * -44.0;
        poly[4] = poly[2] +(M_PI /180.0 * 2.0);
        break;
      case 101 :  /* V1 */
        poly[2] = M_PI /180.0 * -(52.0 +55.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 *(2.0 +10.0/60.0));
        break;
      case 102 :  /* V2 */
        poly[2] = M_PI /180.0 * -50.75;
        poly[4] = poly[2] +(M_PI /180.0 *(2.0 +10.0/60.0));
        break;
      }

      break;

    case 64 :
      switch (ar2) {
      case 1 :  /* Ø1 */
        poly[2] = M_PI /180.0 * -(41.0 +55.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 * 2.25);
        break;
      case 101 :  /* V1 */
        poly[2] = M_PI /180.0 * -(52.0 +55.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 *(2.0 +10.0/60.0));
        break;
      case 102 :  /* V2 */
        poly[2] = M_PI /180.0 * -50.75;
        poly[4] = poly[2] +(M_PI /180.0 *(2.0 +10.0/60.0));
        break;
      }

      break;


/* read_map.c    version 1.0        # page 21   18 Oct 2000 10 01 */


    case 65 :
      switch (ar2) {
      case 1 :  /* Ø1 */
        poly[2] = M_PI /180.0 * -38.0;
        break;
      case 2 :  /* Ø2 */
        poly[2] = M_PI /180.0 * -(40.0 +20.0/60.0);
        break;
      case 3 :  /* Ø3 */
        poly[2] = M_PI /180.0 * -(42.0 +40.0/60.0);
        break;
      case 101 :  /* V1 */
        poly[2] = M_PI /180.0 * -(54.0 +5.0/60.0);
        break;
      case 102 :  /* V2 */
        poly[2] = M_PI /180.0 * -51.75;
        break;
      }
      poly[4] = poly[2] +(M_PI /180.0 *(2.0 +20.0/60.0));

      break;

    case 66 :
      switch (ar2) {
      case 1 :  /* Ø1 */
        poly[2] = M_PI /180.0 * -(35.0 +40.0/60.0);
        break;
      case 2 :  /* Ø2 */
        poly[2] = M_PI /180.0 * -38.0;
        break;
      case 3 :  /* Ø3 */
        poly[2] = M_PI /180.0 * -(40.0 +20.0/60.0);
        break;
      case 101 :  /* V1 */
        poly[2] = M_PI /180.0 * -(54.0 +5.0/60.0);
        break;
      case 102 :  /* V2 */
        poly[2] = M_PI /180.0 * -51.75;
        break;
      }
      poly[4] = poly[2] +(M_PI /180.0 *(2.0 +20.0/60.0));

      break;


/* read_map.c    version 1.0        # page 22   18 Oct 2000 10 01 */


    case 67 :
      switch (ar2) {
      case 1 :  /* Ø1 */
        poly[2] = M_PI /180.0 * -(34.0 +20.0/60.0);
        break;
      case 2 :  /* Ø2 */
        poly[2] = M_PI /180.0 * -(36.0 +50.0/60.0);
        break;
      case 101 :  /* V1 */
        poly[2] = M_PI /180.0 * -(54.0 +25.0/60.0);
        break;
      case 102 :  /* V2 */
        poly[2] = M_PI /180.0 * -(51.0 +55.0/60.0);
        break;
      }
      poly[4] = poly[2] +(M_PI /180.0 * 2.5);

      break;

    case 68 :
      switch (ar2) {
      case 1 :  /* Ø1 */
        poly[2] = M_PI /180.0 * -(27.0 +20.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 *(2.0 +35.0/60.0));
        break;
      case 2 :  /* Ø2 */
        poly[2] = M_PI /180.0 * -(29.0 +55.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 *(2.0 +35.0/60.0));
        break;
      case 3 :  /* Ø3 */
        poly[2] = M_PI /180.0 * -32.5;
        poly[4] = poly[2] +(M_PI /180.0 *(2.0 +35.0/60.0));
        break;
      case 4 :  /* Ø4 */
        poly[2] = M_PI /180.0 * -(35.0 +5.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 *(2.0 +35.0/60.0));
        break;
      case 101 :  /* V1 */
        poly[2] = M_PI /180.0 * -55.0;
        poly[4] = poly[2] +(M_PI /180.0 *(2.0 +40.0/60.0));
        break;
      case 102 :  /* V2 */
        poly[2] = M_PI /180.0 * -(52.0 +20.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 *(2.0 +40.0/60.0));
        break;
      }

      break;


/* read_map.c    version 1.0        # page 23   18 Oct 2000 10 01 */


    case 69 :
      switch (ar2) {
      case 1 :  /* Ø1 */
        poly[2] = M_PI /180.0 * -24.75;
        poly[4] = poly[2] +(M_PI /180.0 *(2.0 +35.0/60.0));
        break;
      case 2 :  /* Ø2 */
        poly[2] = M_PI /180.0 * -(27.0 +20.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 *(2.0 +35.0/60.0));
        break;
      case 3 :  /* Ø3 */
        poly[2] = M_PI /180.0 * -(29.0 +55.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 *(2.0 +35.0/60.0));
        break;
      case 4 :  /* Ø4 */
        poly[2] = M_PI /180.0 * -32.5;
        poly[4] = poly[2] +(M_PI /180.0 *(2.0 +35.0/60.0));
        break;
      case 5 :  /* Ø5 */
        poly[2] = M_PI /180.0 * -(35.0 +5.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 *(2.0 +35.0/60.0));
        break;
      case 101 :  /* V1 */
        poly[2] = M_PI /180.0 * -55.0;
        poly[4] = poly[2] +(M_PI /180.0 *(2.0 +40.0/60.0));
        break;
      case 102 :  /* V2 */
        poly[2] = M_PI /180.0 * -(52.0 +20.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 *(2.0 +40.0/60.0));
        break;
      }

      break;

    case 70 :
      switch (ar2) {
      case 1 :  /* Ø1 */
        poly[2] = M_PI /180.0 * -24.25;
        poly[4] = poly[2] +(M_PI /180.0 *(2.0 +55.0/60.0));
        break;
      case 2 :  /* Ø2 */
        poly[2] = M_PI /180.0 * -27.5;
        poly[4] = poly[2] +(M_PI /180.0 * 3.25);
        break;
      case 3 :  /* Ø3 */
        poly[2] = M_PI /180.0 * -30.75;
        poly[4] = poly[2] +(M_PI /180.0 * 3.25);
        break;
      case 101 :  /* V1 */
        poly[2] = M_PI /180.0 * -(55.0 +50.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 *(2.0 +55.0/60.0));
        break;
      case 102 :  /* V2 */
        poly[2] = M_PI /180.0 * -(52.0 +55.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 *(2.0 +55.0/60.0));
        break;
      }

      break;


/* read_map.c    version 1.0        # page 24   18 Oct 2000 10 01 */


    case 71 :
      switch (ar2) {
      case 1 :  /* Ø1 */
        poly[2] = M_PI /180.0 * -24.25;
        poly[4] = poly[2] +(M_PI /180.0 * 3.25);
        break;
      case 2 :  /* Ø2 */
        poly[2] = M_PI /180.0 * -27.5;
        poly[4] = poly[2] +(M_PI /180.0 * 3.25);
        break;
      case 3 :  /* Ø3 */
        poly[2] = M_PI /180.0 * -30.75;
        poly[4] = poly[2] +(M_PI /180.0 * 3.25);
        break;
      case 101 :  /* V1 */
        poly[2] = M_PI /180.0 * -(55.0 +50.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 *(2.0 +55.0/60.0));
        break;
      case 102 :  /* V2 */
        poly[2] = M_PI /180.0 * -(52.0 +55.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 *(2.0 +55.0/60.0));
        break;
      }

      break;

    case 72 :
      switch (ar2) {
      case 1 :  /* Ø1 */
        poly[2] = M_PI /180.0 * -(22.0 +55.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 *(3.0 +5.0/60.0));
        break;
      case 2 :  /* Ø2 */
        poly[2] = M_PI /180.0 * -26.0;
        poly[4] = poly[2] +(M_PI /180.0 *(3.0 +5.0/60.0));
        break;
      case 3 :  /* Ø3 */
        poly[2] = M_PI /180.0 * -(29.0 +5.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 *(3.0 +5.0/60.0));
        break;
      case 4 :  /* Ø4 */
        poly[2] = M_PI /180.0 * -(32.0 +10.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 *(3.0 +5.0/60.0));
        break;
      case 101 :  /* V1 */
        poly[2] = M_PI /180.0 * -56.5;
        poly[4] = poly[2] +(M_PI /180.0 * 5.0);
        break;
      }

      break;


/* read_map.c    version 1.0        # page 25   18 Oct 2000 10 01 */


    case 73 :
      switch (ar2) {
      case 1 :  /* Ø1 */
        poly[2] = M_PI /180.0 * -(22.0 +55.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 *(3.0 +5.0/60.0));
        break;
      case 2 :  /* Ø2 */
        poly[2] = M_PI /180.0 * -26.0;
        poly[4] = poly[2] +(M_PI /180.0 *(3.0 +5.0/60.0));
        break;
      case 3 :  /* Ø3 */
        poly[2] = M_PI /180.0 * -(29.0 +5.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 *(3.0 +5.0/60.0));
        break;
      case 4 :  /* Ø4 */
        poly[2] = M_PI /180.0 * -(32.0 +10.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 *(3.0 +5.0/60.0));
        break;
      case 101 :  /* V1 */
        poly[2] = M_PI /180.0 * -58.5;
        poly[4] = poly[2] +(M_PI /180.0 * 4.5);
        break;
      }

      break;

    case 74 :
      switch (ar2) {
      case 1 :  /* Ø1 */
        poly[2] = M_PI /180.0 * -(20.0 +20.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 * 3.25);
        break;
      case 2 :  /* Ø2 */
        poly[2] = M_PI /180.0 * -(23.0 +35.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 * 3.25);
        break;
      case 3 :  /* Ø3 */
        poly[2] = M_PI /180.0 * -(26.0 +50.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 * 3.25);
        break;
      case 4 :  /* Ø4 */
        poly[2] = M_PI /180.0 * -(30.0 +5.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 * 3.25);
        break;
      case 101 :  /* V1 */
        poly[2] = M_PI /180.0 * -58.5;
        poly[4] = poly[2] +(M_PI /180.0 * 4.5);
        break;
      }

      break;


/* read_map.c    version 1.0        # page 26   18 Oct 2000 10 01 */


    case 75 :
      switch (ar2) {
      case 1 :  /* Ø1 */
        poly[2] = M_PI /180.0 * -(20.0 +20.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 * 3.25);
        break;
      case 2 :  /* Ø2 */
        poly[2] = M_PI /180.0 * -(23.0 +35.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 * 3.25);
        break;
      case 3 :  /* Ø3 */
        poly[2] = M_PI /180.0 * -(26.0 +50.0/60.0);
        poly[4] = poly[2] +(M_PI /180.0 * 3.25);
        break;
      case 101 :  /* V1 */
        poly[2] = M_PI /180.0 * -62.0;
        poly[4] = poly[2] +(M_PI /180.0 * 5.5);
        break;
      }
      break;

    case 76 :
      switch (ar2) {
      case 1 :  /* Ø1 */
        poly[2] = M_PI /180.0 * -23.0;
        poly[4] = poly[2] +(M_PI /180.0 * 5.0);
        break;
      case 2 :  /* Ø2 */
        poly[2] = M_PI /180.0 * -28.0;
        poly[4] = poly[2] +(M_PI /180.0 * 5.0);
        break;
      case 101 :  /* V1 */
        poly[2] = M_PI /180.0 * -73.5;
        poly[4] = poly[2] +(M_PI /180.0 * 6.0);
        break;
      case 102 :  /* V2 (BEMÆRK, der skal laves expand) */
        poly[2] = M_PI /180.0 * -67.5;
        poly[4] = poly[2] +(M_PI /180.0 * 6.0);
        break;
      case 103 :  /* V3 */
        poly[2] = M_PI /180.0 * -61.5;
        poly[4] = poly[2] +(M_PI /180.0 * 6.0);
        break;
      }

      break;


/* read_map.c    version 1.0        # page 27   18 Oct 2000 10 01 */


    case 77 :
      switch (ar2) {
      case 1 :  /* Ø1 */
        poly[2] = M_PI /180.0 * -22.5;
        poly[4] = poly[2] +(M_PI /180.0 * 5.0);
        break;
      case 2 :  /* Ø2 */
        poly[2] = M_PI /180.0 * -28.0;
        poly[4] = poly[2] +(M_PI /180.0 * 5.5);
        break;
      case 101 :  /* V1 */
        poly[2] = M_PI /180.0 * -73.5;
        poly[4] = poly[2] +(M_PI /180.0 * 6.0);
        break;
      case 102 :  /* V2 */
        poly[2] = M_PI /180.0 * -67.5;
        poly[4] = poly[2] +(M_PI /180.0 * 6.0);
        break;
      }

      break;

    case 78 :
      switch (ar2) {
      case 1 :  /* Ø1 */
        poly[2] = M_PI /180.0 * -23.5;
        break;
      case 2 :  /* Ø2 */
        poly[2] = M_PI /180.0 * -29.5;
        break;
      case 101 :  /* V1 */
        poly[2] = M_PI /180.0 * -74.0;
        break;
      case 102 :  /* V2 */
        poly[2] = M_PI /180.0 * -68.0;
        break;
      }
      poly[4] = poly[2] +(M_PI /180.0 * 6.0);

      break;


/* read_map.c    version 1.0        # page 28   18 Oct 2000 10 01 */


    case 79 :
      switch (ar2) {
      case 1 :  /* Ø1 */
        poly[2] = M_PI /180.0 * -23.5;
        break;
      case 2 :  /* Ø2 */
        poly[2] = M_PI /180.0 * -30.5;
        break;
      case 101 :  /* V1 */
        poly[2] = M_PI /180.0 * -69.0;
        break;
      case 102 :  /* V2 */
        poly[2] = M_PI /180.0 * -62.0;
        break;
      }
      poly[4] = poly[2] +(M_PI /180.0 * 7.0);

      break;

    case 80 :
      switch (ar2) {
      case 1 :  /* Ø1 */
        poly[2] = M_PI /180.0 * -19.0;
        break;
      case 2 :  /* Ø2 */
        poly[2] = M_PI /180.0 * -26.0;
        break;
      case 3 :  /* Ø3 */
        poly[2] = M_PI /180.0 * -33.0;
        break;
      case 101 :  /* V1 */
        poly[2] = M_PI /180.0 * -68.0;
        break;
      case 102 :  /* V2 */
        poly[2] = M_PI /180.0 * -61.0;
        break;
      }
      poly[4] = poly[2] +(M_PI /180.0 * 7.0);

      break;


/* read_map.c    version 1.0        # page 29   18 Oct 2000 10 01 */


    case 81 :
      switch (ar2) {
      case 1 :  /* Ø1 */
        poly[2] = M_PI /180.0 * -19.0;
        break;
      case 2 :  /* Ø2 */
        poly[2] = M_PI /180.0 * -27.0;
        break;
      case 3 :  /* Ø3 */
        poly[2] = M_PI /180.0 * -35.0;
        break;
      case 4 :  /* Ø4 */
        poly[2] = M_PI /180.0 * -43.0;
        break;
      case 101 :  /* V1 */
        poly[2] = M_PI /180.0 * -67.0;
        break;
      case 102 :  /* V2 */
        poly[2] = M_PI /180.0 * -59.0;
        break;
      case 103 :  /* V3 */
        poly[2] = M_PI /180.0 * -51.0;
        break;
      }
      poly[4] = poly[2] +(M_PI /180.0 * 8.0);

      break;

    case 82 :
      switch (ar2) {
      case 1 :  /* Ø1 */
        poly[2] = M_PI /180.0 * -26.5;
        break;
      case 2 :  /* Ø2 */
        poly[2] = M_PI /180.0 * -35.5;
        break;
      case 3 :  /* Ø3 */
        poly[2] = M_PI /180.0 * -44.5;
        break;
      case 101 :  /* V1 */
        poly[2] = M_PI /180.0 * -62.5;
        break;
      case 102 :  /* V2 */
        poly[2] = M_PI /180.0 * -53.5;
        break;
      }
      poly[4] = poly[2] +(M_PI /180.0 * 9.0);

      break;


/* read_map.c    version 1.0        # page 30   18 Oct 2000 10 01 */


    case 83 :
      switch (ar2) {
      case 1 :  /* Ø1 */
        poly[2] = M_PI /180.0 * -25.5;
        break;
      case 2 :  /* Ø2 */
        poly[2] = M_PI /180.0 * -36.5;
        break;
      case 3 :  /* Ø3 */
        poly[2] = M_PI /180.0 * -47.5;
        break;
      }
      poly[4] = poly[2] +(M_PI /180.0 * 11.0);

      break;

    }  /* switch (ar1) */
    poly[8] = poly[2];
    poly[6] = poly[4];

    /* For G/100 reguleres de beregnede koordinater */
    if (ar3 < 250) {
      if (ar3 == 101 || ar3 == 104) {
        poly[7] = poly[5] +((poly[1] - poly[5]) / 2.0);
        poly[5] = poly[7];
      }
      else {
        poly[3] = poly[5] +((poly[1] - poly[5]) / 2.0);
        poly[1] = poly[3];
      }
      if (ar3 == 101 || ar3 == 102) {
        poly[8] = poly[2] +((poly[4] - poly[2]) / 2.0);
        poly[2] = poly[8];
      }
      else {
        poly[6] = poly[2] +((poly[4] - poly[2]) / 2.0);
        poly[4] = poly[6];
      }
    }

    *(c_poly +0) = 4.0;
    *(c_poly +1) = poly[1];
    *(c_poly +2) = poly[2];
    *(c_poly +3) = poly[3];
    *(c_poly +4) = poly[4];
    *(c_poly +5) = poly[5];
    *(c_poly +6) = poly[6];
    *(c_poly +7) = poly[7];
    *(c_poly +8) = poly[8];

    break;


/* read_map.c    version 1.0        # page 31   18 Oct 2000 10 01 */


  case FA_MAP :  /* Færøske kort */
    break;


/* read_map.c    version 1.0        # page 32   18 Oct 2000 10 01 */


  case MA_MAP :  /* Målebord */
  case AT_MAP :  /* Atlasblade */
    break;

  case CM_MAP :  /* CM kort */

    /*  -----------------------------------------------   */
    /* |           |           |                       |  */
    /* |  NV       |  NE       |                       |  */
    /* |           |           |                       |  */
    /* |           |           |                       |  */
    /* |           |           |                       |  */
    /* |----------IV-----------|           I           |  */
    /* | 1 | 2 | 3 |           |                       |  */
    /* |--SV---|---|  SE       |                       |  */
    /* | 4 | 5 | 6 |           |                       |  */
    /* |---|---|---|           |                       |  */
    /* | 7 | 8 | 9 |           |                       |  */
    /* |-----------------------------------------------|  */
    /* |                       |                       |  */
    /* |                       |                       |  */
    /* |                       |                       |  */
    /* |                       |                       |  */
    /* |                       |                       |  */
    /* |          III          |          II           |  */
    /* |                       |                       |  */
    /* |                       |                       |  */
    /* |                       |                       |  */
    /* |                       |                       |  */
    /* |                       |                       |  */
    /*  -----------------------------------------------   */

    bornholm = 0;
    nr       = ar1 /100;
    pf       = ar1 %100;

    if (ar1 < 1700)
      conv_lab("gs_gs", map_lab, "");
      else
      conv_lab("utm33_ed50", map_lab, "");

    /* beregn koordinaterne */

    /* Naturtro afstand for 4-CM kort: */
    /* (N-S):   11_298.876_800 m       */
    /* (E-V):   14_123.595_990 m       */

    cm4_y = 11298876800.0;
    cm4_x = 14123595990.0;


/* read_map.c    version 1.0        # page 33   18 Oct 2000 10 01 */


    switch (nr % 10) {
    case 0 :
      cx_e =  7.0;
      break;

    case 1 :
      cx_e =  5.0;
      break;

    case 2 :
      cx_e =  3.0;
      break;

    case 3 :
      cx_e =  1.0;
      break;

    case 4 :
      cx_e = -1.0;
      break;

    case 5 :
      cx_e = -3.0;
      break;

    case 6 :
      cx_e = -5.0;
      break;

    case 7 :
      cx_e =  0.0;
      break;

    case 8 :
      cx_e =  0.0;
      break;

    }
    /* NE-hjørnekoordinater */
    hy    = cm4_y *4.0 *(pf -11);
    hx    = cm4_x *2.0 *cx_e;

    if (ar1 == 1318) {  /* NORDJYLLAND */
      /* hx     = hx +2.0 *(long long) cm4_x /3.0; */
      modf(cm4_x/3.0, &dbli);
      hx     = hx +2.0 *dbli;
    }

    /* NABO TIL 1318 OG 1417 LÆSØ */
    if (ar1 == 1418 || ar1 == 1419 ||
        ar1 == 1518 || ar1 == 1519) {
      /* hx     = hx +2.0 *(long long) cm4_x /3.0; */
      /* hy     = hy -2.0 *(long long) cm4_y /3.0; */
      modf(cm4_x/3.0, &dbli);
      hx     = hx +2.0 *dbli;
      modf(cm4_y/3.0, &dbli);
      hy     = hy -2.0 *dbli;
    }


/* read_map.c    version 1.0        # page 34   18 Oct 2000 10 01 */


    /* BORNHOLM, VEST */
    if (ar1   == 1712) {
      bornholm = 1;
      hy       = 6132000000000.0;
      hx       =  424400000000.0;
      cm4_y    =   11300000000.0;
      cm4_x    =   14150000000.0;
    }

    /* BORNHOLM */
    if (ar1   == 1812) {
      bornholm = 1;
      hy       = 6132000000000.0;
      hx       = ar2 == 0 ? 466850000000.0 : 481000000000.0;
      cm4_y    =   11300000000.0;
      cm4_x    =   14150000000.0;
    }

    /* LÆSØ */
    if (ar1 == 1417 && ar2 != 0) {
      /* hy     = hy -2.0 *(long long) cm4_y /3.0; */
      modf(cm4_y/3.0, &dbli);
      hy     = hy -2.0 *dbli;
    }

    /* ANHOLT */
    if ((ar1 == 1416 && ar2 == 2) ||
        (ar1 == 1516 && ar2 == 3)) {
      /* hy      = hy +(long long) cm4_y /3.0; */
      /* hx      = hx +(long long) cm4_x /3.0; */
      modf(cm4_y/3.0, &dbli);
      hy      = hy +dbli;
      modf(cm4_x/3.0, &dbli);
      hx      = hx +dbli;
    }

    if (ar1   == 1812 && ar2 == 6) {
      /* 2 CM bornolm, særkort */
      poly[1]  = poly[3]  = 6129200000000.0;
      poly[5]  = poly[7]  = 6091500000000.0;
      poly[2]  = poly[8]  =  481000000000.0;
      poly[4]  = poly[6]  =  509300000000.0;
    }
    else {
      /* originale rammer     */
      /* diverse korrektioner */
      cx_b   = bornholm ? -1.0 : 1.0;


/* read_map.c    version 1.0        # page 35   18 Oct 2000 10 01 */


      switch (ar2) {
      case 0 :
        cy_t = 0.0;
        cx_t = 0.0;
        break;

      case 1 :
        cy_t = 0.0;
        cx_t = 2.0;
        break;

      case 2 :
        cy_t = 2.0;
        cx_t = 2.0;
        break;

      case 3 :
        cy_t = 2.0;
        cx_t = 0.0;
        break;

      case 4 :
        cy_t = 0.0;
        cx_t = 0.0;
        break;
      }

      switch (ar3) {
      case 0 :
        cy_f = 0.0;
        cx_f = 0.0;
        break;

      case 1 :
        cy_f = 0.0;
        cx_f = 1.0;
        break;

      case 2 :
        cy_f = 1.0;
        cx_f = 1.0;
        break;

      case 3 :
        cy_f = 1.0;
        cx_f = 0.0;
        break;

      case 4 :
        cy_f = 0.0;
        cx_f = 0.0;
        break;
      }


/* read_map.c    version 1.0        # page 36   18 Oct 2000 10 01 */


      switch (ar4) {
      case 0 :
        cy_k = 0.0;
        cx_k = 0.0;
        break;

      case 1 :
        cy_k = 0.0;
        cx_k = 0.0;
        break;

      case 2 :
        cy_k = 0.0;
        cx_k = 1.0;
        break;

      case 3 :
        cy_k = 0.0;
        cx_k = 2.0;
        break;

      case 4 :
        cy_k = 1.0;
        cx_k = 0.0;
        break;

      case 5 :
        cy_k = 1.0;
        cx_k = 1.0;
        break;

      case 6 :
        cy_k = 1.0;
        cx_k = 2.0;
        break;

      case 7 :
        cy_k = 2.0;
        cx_k = 0.0;
        break;

      case 8 :
        cy_k = 2.0;
        cx_k = 1.0;
        break;

      case 9 :
        cy_k = 2.0;
        cx_k = 2.0;
        break;
      }


/* read_map.c    version 1.0        # page 37   18 Oct 2000 10 01 */


      modf(cm4_y/3.0, &dbli);
      poly[1] = poly[3] = hy =
          hy -((cy_t +cy_f) *cm4_y +cy_k *dbli);

      /* SALTHOLM (15. maj 2006 flyttet 5 km mod syd) */
      if (ar1 == 1613 && ar2 == 4) { 
        poly[1] -= 5000000000.0;
        poly[3] -= 5000000000.0;
        hy      -= 5000000000.0;
      }

      modf(cm4_x/3.0, &dbli);
      poly[2] = poly[8] = hx =
          hx -1.0 *cx_b *((cx_t +cx_f) *cm4_x +cx_k *dbli);

      if (ar4 != 0) {
        modf(cm4_y/3.0, &dbli);
        hy  -= dbli;
      }
      else
        if (ar3  != 0)
          hy  -=  cm4_y;
          else
          if (ar2  != 0)
            hy  -= 2.0 *cm4_y;
            else
            hy  -=  4.0 *cm4_y;
      poly[5] = poly[7] = hy;

      if (ar4  > 6) {
        poly[5] -= 2.0;
        poly[7] -= 2.0;
      }

      if (ar4 != 0) {
        modf(cm4_x/3.0, &dbli);
        hx -= 1.0 *cx_b *dbli;
      }
      else
        if (ar3  != 0)
          hx -= 1.0 *cx_b *cm4_x;
          else
          if (ar2  != 0)
            hx -= 1.0 *cx_b *2.0 *cm4_x;
            else
            hx -= 1.0 *cx_b *4 *cm4_x;

      poly[4] = poly[6] = hx;

      if (bornholm && (ar4 == 3 || ar4 == 6
          || ar4 == 9)) {
        poly[4] += 2.0;
        poly[6] += 2.0;
      }
    }  /* originale rammer     */
    /* resultat fra polygon i meter */
    *(c_poly +0) = 4.0;
    *(c_poly +1) = poly[1] /1000000.0;
    *(c_poly +2) = poly[2] /1000000.0;
    *(c_poly +3) = poly[3] /1000000.0;
    *(c_poly +4) = poly[4] /1000000.0;
    *(c_poly +5) = poly[5] /1000000.0;
    *(c_poly +6) = poly[6] /1000000.0;
    *(c_poly +7) = poly[7] /1000000.0;
    *(c_poly +8) = poly[8] /1000000.0;

    break;


/* read_map.c    version 1.0        # page 38   18 Oct 2000 10 01 */


  case DI_MAP :  /* Diverse syd-danmark */
    break;

  case TX_MAP :  /* Text, ikke afkodet */
    break;

  default :
    break;

  }  /* switch (state) */
  /* Beregning af koordinaterne til det aktuelle */
  /* kortblads hjørnepunkter (polygonpunkter).   */


/* read_map.c    version 1.0        # page 39   18 Oct 2000 10 01 */


  /* Pjat, pænt formateret streng indeholdende      */
  /* kortbladsnummeret, op til 24 karakterer lang.  */

  /* Her behandles kun store bogstaver, dvs de små er  */
  /* konverteret i efterfølgende kode.                 */
  /* Koden skal rettes hvis små skal kunne behandles.  */

  p = map_str;

  switch (state) {

  case MV_MAP :  /* MV's kortblad */
    ch    = sprintf(p, "%c", ar1);
    p    += ch;
    /* HER ER JEG I TVIVL */
    if (ar2 == 42)
      p     += sprintf(p, "%c", ar2 +'0');
      else
      p     += sprintf(p, "%d", ar2);
    if (ar3 != 100)  /* <> 1:40_000 blad */
      p     += sprintf(p, "%2d", ar3);
    if (ar4  > 0) {
      if (ar4 <= 4)
        (void) sprintf(p, "%d", ar4);
        else
        (void) sprintf(p, "%2d", ar4);
    }

    break;

  case GR_MAP :  /* Grønlandske kort */
    ch    = sprintf(p, "%c/", ar4);
    p    += ch;
    p    += sprintf(p, "%3d %d ", (ar3 /25) *25, ar1);
    switch (ar2 /100) {
    case 0 :
      p    += sprintf(p, "%s", "E");
      break;
    case 1 :
      p    += sprintf(p, "%s", "V");
      break;
    }
    p    += sprintf(p, "%d", ar2 % 100);

    if (ar3 < 250) {
      switch (ar3 % 100) {
      case 1 :
        (void) sprintf(p, " %s", "NE");
        break;

      case 2 :
        (void) sprintf(p, " %s", "SE");
        break;

      case 3 :
        (void) sprintf(p, " %s", "SV");
        break;

      case 4 :
        (void) sprintf(p, " %s", "NV");
        break;
      }
    }

    break;


/* read_map.c    version 1.0        # page 40   18 Oct 2000 10 01 */


  case FA_MAP :  /* Færøske kort */
    *(c_poly +0) = 0.0;
    break;

  case CM_MAP :  /* CM kort */
    ch    = sprintf(p, "%4d", ar1);
    p    += ch;
    if (ar2 != 0) {
      switch (ar2) {
      case 1 :
        p    += sprintf(p, " %s", "I");
        break;

      case 2 :
        p    += sprintf(p, " %s", "II");
        break;

      case 3 :
        p    += sprintf(p, " %s", "III");
        break;

      case 4 :
        p    += sprintf(p, " %s", "IV");
        break;

      case 5 :
        p    += sprintf(p, " %s", "ALLE");
        break;

      case 6 :
        p    += sprintf(p, " %s", "S");
        break;

      }
    }


/* read_map.c    version 1.0        # page 41   18 Oct 2000 10 01 */


    if (ar3 != 0) {
      switch (ar3) {
      case 1 :
        p    += sprintf(p, " %s", "NE");
        break;

      case 2 :
        p    += sprintf(p, " %s", "SE");
        break;

      case 3 :
        p    += sprintf(p, " %s", "SV");
        break;

      case 4 :
        p    += sprintf(p, " %s", "NV");
        break;

      case 5 :
        p    += sprintf(p, " %s", "ALLE");
        break;

      }
    }

    if (ar4   != 0) {
      if (ar4 == 10)
        (void) sprintf(p, " %s", "ALLE");
        else
        (void) sprintf(p, "%2d", ar4);
    }

    break;


/* read_map.c    version 1.0        # page 42   18 Oct 2000 10 01 */


  case MA_MAP :  /* Målebord */
  case AT_MAP :  /* Atlasblade */
  case DI_MAP :  /* Diverse syd-danmark */
    *(c_poly +0) = 0.0;

    if (ar4 > 96 && ar4 != 103 && ar2 != 250) {
      /* MA_MAP, AT_MAP eller DI_MAP */
      ch    = sprintf(p, "%c", ar4);
      p    += ch;
      if (ar4 == 100 || ar4 == 68 || ar4 == 102 || ar4 == 70)
        p    += sprintf(p, "%s", "/");
        else
        p    += sprintf(p, "%s", " ");
      if (ar2 < 100)
        p    += sprintf(p, "%2d", ar2);
        else
        if (ar2 < 1000)
          p    += sprintf(p, "%3d", ar2);
          else
          p    += sprintf(p, "%4d", ar2);

      if (ar4 != 100 && ar4 != 68 && ar4 != 102 && ar4 != 70) {
        if (ar3 > 0) {
          switch (ar3) {
          case 1 :
            (void) sprintf(p, " %s", "NE");
            break;

          case 2 :
            (void) sprintf(p, " %s", "SE");
            break;

          case 3 :
            (void) sprintf(p, " %s", "SV");
            break;

          case 4 :
            (void) sprintf(p, " %s", "NV");
            break;
          }
        }
      }


/* read_map.c    version 1.0        # page 43   18 Oct 2000 10 01 */


      else {
        /* DI_MAP  */
        if (ar4 == 102 || ar4 == 70) {
          /* FA_MAP */
          if (ar3 < 10)
            p    += sprintf(p, "%2d", ar3);
          if (ar3 < 100)
            p    += sprintf(p, "%3d", ar3);
          else 
            p    += sprintf(p, "%4d", ar3);

          if (ar1 > 0 && ar1 <= 5) {
            switch (ar1) {
            case 1 :
              (void) sprintf(p,
                  " %s", "NE");
              break;

            case 2 :
              (void) sprintf(p,
                  " %s", "SE");
              break;

            case 3 :
              (void) sprintf(p,
                  " %s", "SV");
              break;

            case 4 :
              (void) sprintf(p,
                  " %s", "NV");
              break;

            case 5 :
              (void) sprintf(p,
                  " %s", "ALLE");
              break;

            }
          }
          else {
            if (ar3 < 100) {
              p    += sprintf(p,
                  " %2d", ar3 /
                  10);
              if (ar3 % 10
                  == 1)
                (void) sprintf(p,
                    " %s", "A");
            }
            else
              if (ar3 <= 118)
                (void) sprintf(p,
                    " S%d", ar3 %
                    100);
                else
                (void) sprintf(p,
                    "%5d",
                    ar3);
          }
        }
      }
    }


/* read_map.c    version 1.0        # page 44   18 Oct 2000 10 01 */


    else
      if ((ar1 == 1 || ar1 == 2) && ar2 == 250) {
        /* 1501 */
        ch    = sprintf(p, "%s ", "D/250");
        p    += ch;
        switch (ar1) {
        case 1 :
          p    += sprintf(p, "%s", "NO");
          break;

        case 2 :
          p    += sprintf(p, "%s", "NN");
          break;
        }
        (void) sprintf(p, "%3d-%d", ar3 /100, ar3 % 100);
      }
    *(c_poly +0) = 0.0;
    break;

  case TX_MAP :  /* Text, ikke afkodet */
    *(c_poly +0) = 0.0;
    break;

  default :
    break;

  }  /* switch (state) */
  /* Pjat, pænt formateret streng indeholdende      */
  /* kortbladsnummeret, op til 24 karakterer lang.  */

  return(typ);
}

#undef       NO_MAP 
#undef       TX_MAP 
#undef       CM_MAP 
#undef       MV_MAP 
#undef       GR_MAP 
#undef       FA_MAP 
#undef       DI_MAP 
#undef       MA_MAP 
#undef       AT_MAP 


