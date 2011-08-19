

/* sand_areal                       # page 1    2 Feb 2009 12 54 */


/* Copyright (c) 2009, National Space Institute, DTU, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of DTU, Denmark */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */


extern double   sand_areal(
/*_______________________*/
char           *datum,
double         *pol_base
);

/* sand_areal omregner areal givet ved koordinater til
   ========= n polygonpunkter til arealet på ellipsoiden,
             der kun afviger 11 ppm fra det i marken målte
             areal korrigeret til nulniveau
   Ved illegalt *datum returneres: sand_areal = -1.0

  *datum    : char pointer til datum(projektion).
              Legale : "euref89", "dktm1", "dktm2", "dktm3", "dktm4",
                       "kp2000j", "kp2000s", "kp2000b"
              Ved illegalt *datum returneres: sand_areal = -1.0

   *pol_base: beskriver n polygon punkter:
              pol_base[0] = n
              pol_base[1] = N til pkt 1
              pol_base[2] = E til pkt 1
              pol_base[3] = N til pkt 2
              . . .
              pol_base[2n] = E til pkt n

Programmed by: Karsten Engsager, 7. febr. 2005
*/

