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
 


/* sand_afst                       # page 1    2 Feb 2009 12 54 */


/* Copyright (c) 2009, National Space Institute, DTU, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of DTU, Denmark */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */

#if !defined     (H_SAND_AFST)
#define           H_SAND_AFST

extern double  sand_afst(
/*_____________________*/
char          *datum,
double         afst,
double         E_middel
);

/* sand_afst omregner afstanden givet ved koordinater til
   ========= afstanden på ellipsoiden, der kun afviger 5.5 ppm
             fra den i marken målte afstand korrigeret til nulniveau
   Ved illegalt *datum returneres: sand_afst = -1.0

  *datum    : char pointer til datum(projektion).
              Legale : "euref89", "dktm1", "dktm2", "dktm3", "dktm4",
                       "kp2000j", "kp2000s", "kp2000b"
              Ved illegalt *datum returneres: sand_afst = -1.0

   afst     : afstand fra pkt1(N1, E1) til pkt2(N2, E2) 
              afst = sqrt((N1-N2)*(N1-N2) + (E1-E2)*(E1-E2));

   E_middel : middel Easting fra pkt1(N1, E1) til pkt2(N2, E2) 
              E_middel = (E1 + E2)/2;

Programmed by: Karsten Engsager, 7. febr. 2005
*/

#endif
