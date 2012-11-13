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
 
/* Prog: KP OCT 1990                                           */
/* Modified, KP MAY 1993                                       */
/* Modified, KE JUL 2007                                       */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdarg.h>
#include "parse_def_file.h"

#ifndef    M_PI
#include   "kms_math.h"
#endif


int          set_grs(
/*_________________*/
int          ell_nmb,
char        *ellname,
double      *e,
...
)

{




  char                        s_name[24], *ell_nm = s_name;
 

  int                         empty = 1, mode = 0, mode_234;
  int                         e_no, i, res = 0;
  double                      fh, n2, toise_t_m = 0.0;
  double                      m, qd, qdp, d, omg2, n = 0, Q;
  double                      a, b, amg, J2 = 0.0, f = 0.0, e2p, geq;
  double                      GM, ksf, U0, sum;
 
  double   a_m;       /* Semi-major axis or mean degree    */
  double   J2f;       /* J2 or 1/f depending on definition */
  double   GMg;       /* GM or geq         -"-             */
  double   omg;       /* rotation velocity                 */
  /* use the global parsed data : simlk, june 2012 */
  extern def_data *DEF_DATA;
  def_grs *grs_def;
  int n_grs=0;




#define  GEQ      978049.e-5      /* Equator qravity    */
#define  KM       3986005.e8      /* Grav. const * Mass */
#define  OMEGA    7292115.0e-11   /* Rotation velocity  */

/* Defence Mapping Agency, Dept. of Defence World Geodetic System 1984,
   DMA Technical Report 8350.2, Sec. Ed., September 1, 1991 */
/* mcCarthy, D., IERS Conventions (1996), IERS Technical Note 21,
   Observatoire de Paris, 1996 */
#define  GEQ84    979764.5e-5     /* Equator qravity    */
#define  KMW84    3986004.418e8   /* Grav. const * Mass */

  /* Definition of kms numbers of ellipsoids  */
  /*           nmb  name                      */
  /*            0   lokal when datum ==  0 */
  /*    EUD     0   User defined ellipsoid */

  /*    E80     1   GRS80      */
  /*    EHA     2   Hayford    */
  /*    E67     3   GRS67      */

  /*    E84     4   WGS84      */
  /*    E72     5   WGS72      */
  /*    E9D     6   NWL9D      */

  /*    EGS     7   GS         */
  /*    EDA     8   Dansk      */
  /*    EBE     9   Bessel     */

  /*    EKR    10   Krassovsky */
  /*    ECL66  11   Clarke66   */
  /*    ECL80  12   Clarke80   */

  /*    EEV    13   Everest    */
  /*    EAI    14   Airy       */
  /*    ESB    15   Bessel, or.*/

  /*    ETP    16   TOPEX      */

  /*    SPH    90   SPHERIC    */
  /*    EMO    99   Moon       */
  /*    EMA   104   Mars       */

  /* Geodetic Reference Systems are read from ~geoid/def_grs.txt: */
  /* #name                                               */
  /* no  mode    a or m.dg  J2 or 1/f   km or geq  omega */
  /* Datum name                                          */
  /* Reference text"                                     */

  /* km may be functions: KM<op>factor. <op> = * / + -   */
  /* Reference text may continue over more lines         */
  /* Reference text is terminated by: "                  */


    if (DEF_DATA==NULL) 
	    return -1;

 


   
    

    if (*s_name=='#'){ /* userdefined ellipsoid */
      (void) strcpy(s_name, ellname);
      (void) sscanf(++ell_nm, "%d", &mode);
      if (*(e+3) <= 0.0)  *(e+3) = KM;
      if (*(e+6) <= 0.0)  *(e+6) = GEQ;
      if (*(e+4) <= 0.0)  *(e+4) = OMEGA;
      switch(mode) {
      case  1: /* define by a, J2, GM, and omg */
        J2f = *(e+2);    /* J2 */
        GMg = *(e+3);    /* GM */
        break;
      case  2: /* define by a, f, geq and omg */
        J2f = *(e+1);    /* f  */
        GMg = *(e+6);    /* geq */
        break;
      case  3: /* define by a, f, GM, omg */
        J2f = *(e+1);    /* f  */
        GMg = *(e+3);    /* GM */
        break;
      case  4: /* define by mean degr., f, GM and omg */
        J2f = *(e+1);    /* f  */
        GMg = *(e+3);    /* GM */
        break;
      default: /* illegal mode of init, handled later */
        break;
      }
      a_m  = *(e+0);
      omg  = *(e+4);
      if (!strlen(++ell_nm)) (void) sprintf(ell_nm, "%d",0); /*Why that?? Doesn't seem to be used anywhere? */
      res = empty = 0;
    } /*end user defined ellipsoid */
    else{
	    do {
    
	grs_def=DEF_DATA->ellipsoids+(n_grs++);
	if ((ell_nmb==-1 && !strcmp(grs_def->mlb, ellname)) || (grs_def->no==ell_nmb && ell_nmb>=0)){ /*might not need ell_nmb>=0*/
		e_no=grs_def->no;
		res   = e_no; /* ellip no */
          /* collect mode and definining params */
          /*____________________________________*/
		mode=grs_def->mode;
		a_m=grs_def->axis;
		J2f=grs_def->flattening;
		GMg=grs_def->km;
		omg=grs_def->omega;
		empty = 0;
		if (ell_nmb>=0)
			strcpy(ellname,grs_def->mlb); /*used strncpy before, but this fct. could be called with too short strings */
			
        }
      
    } while (res <= 0 &&  n_grs<DEF_DATA->n_ellip);
  } /*end not user defined */
    
  
 

if (empty) {
    ell_nmb = -2;
    *e      = 0.0;
    (void) strcpy(ellname, "undef ell");
    return(ell_nmb);
  }

  a = amg = a_m;
  if (omg < 0.0) omg = OMEGA;
  omg2     = omg*omg;
  mode_234 = (mode == 2) || (mode == 3) || (mode == 4);
  if (mode_234) {
    f = J2f;
    if (f >= 1.0) f = 1.0/f;
    n = f/(2. - f);
  }

  switch(mode) {
  case  1: /* define by a, J2, GM, and omg */
    J2 = J2f;
    GM = GMg;
    if (e_no == 4) /* WGS84 */
        J2 = -J2*sqrt(5.0);
    /* f */
    /* m  = (Geodesists Handbook) m * a/b */
    /* qd = (Geodesists Handbook) q0 * pow(e', 3) */
    m = omg2*pow(a, 3.)/GM;
    f = 1.5*J2 + .5*m;
    do {
      fh  = f;
      e2p = f*(2. - f)/(1. - f)/(1. - f);
      for (i = 2*8, qd = 0.0; i > 0; i -= 2)
          qd = i/(i + 1.)/(i + 3.) - qd*e2p;
      f = 1.5*J2 + .5*f*f + m*pow(1. -f, 3.)/qd/15.;
    /* e' * e2p / pow(e', 3) *b/a = pow(1. -f, 3.) */
    } while (fabs(f - fh) >= f*1.0e-14);
    m = m*(1.- f);
    b = a*(1.- f);
    /* geq */
    for (i = 2*8, qdp = 0.; i > 0; i -= 2)
         qdp = 1./(i+1.)/(i+3.) - qdp*e2p;
    geq = GM/a/b*(1. - m*(1. + qdp/qd));
    n   = f/(2. - f);
    break;




  case  2: /* define by a, f, geq and omg */
    geq = GMg;
    b   = a*(1.- f);
    /* GM */
    e2p = f*(2. - f)/(1. - f)/(1. - f);
    for (i = 2*8, qdp = qd = 0.0; i> 0; i -= 2) {
      d   = (i + 1.)*(i + 3.);
      qd  = i/d  - qd *e2p;
      qdp = 1./d - qdp*e2p;
    }
    GM = a*b*(geq + a*omg2*(1. + qdp/qd));
    m  = omg2*a*a*b/GM;
    break;

  case  3: /* define by a, f, GM, omg */
    GM  = GMg;
    e2p = f*(2. - f)/(1. - f)/(1. - f);
    b   = a*(1. - f);
    m   = omg2*a*a*b/GM;
    /* geq */
    for (i = 2*8, qd = qdp = 0.; i > 0; i -= 2) {
      d   = (i + 1.)*(i + 3.);
      qd  = i/d -  qd *e2p;
      qdp = 1./d - qdp*e2p;
    }
    geq = GM/a/b*(1. - m*(1. + qdp/qd));
    break;

  case  4: /* define by mean degr., f, geq and omg */
    switch (e_no) {
    case 15: /*  Special Bessel */
      /* Helmert, vol.1 p. 18. toise_t_m = 1.9490363098 */
      a *= (toise_t_m = 1.94903631030);
      /* Bessel original:    n = 0.0016741848   */
      /* Helmert, vol 1 p.38 n = 0.001674184767 */
      /* f  = 0.001674184805; */
      n  = f;
      f  = 2.0*n/(1.0 + n);
      /* a *= (toise_t_m = 1.9490363098);
      _  Official value, value above fitted to give usual value in m
      _  a *= (toise_t_m = 1.9490605000);
      _  f = 1.0/299.15281285;  */
      break;
    case 7: /* GS */
      a *= (toise_t_m = exp(0.289825730*M_LN10));
      break;
    case 8: /* Dansk */
      a *= (toise_t_m = exp(0.289819930*M_LN10));
      break;
    default:
      toise_t_m = 1.0;
      break;
    }




    n2 = n*n;
    a *= 90.0/M_PI_2*(1. + n)
         / (1. + n2*(1./4. + n2*(1./64. + n2/256.)));
    b  = a*(1. - f);
    GM = GMg;
    m  = omg2*a*a*b/GM;
    /* geq */
    e2p = f*(2. - f)/(1. - f)/(1. - f);
    for (i = 2*8, qd = qdp = 0.0; i > 0; i -= 2) {
      d   = (i + 1.)*(i + 3.);
      qd  = i/d -  qd *e2p;
      qdp = 1./d - qdp*e2p;
    }
    geq = GM/a/b*(1. - m*(1. + qdp/qd));
    break;
  default:
    (void) fprintf(stderr,
          "\n** set_grs: mode %d is illegal\n", mode);
    *e = 0.0;
    return(-1);
  }

  /* J2 */
  if (mode_234)
      J2 = f*(2. - f)/3. - 2./45.*omg2*pow(b, 3.)/GM/qd;
  /* k from Somigliana */
  d   = m*qdp/qd;
  ksf = (3.0*d + m - f*(2. - f)*(1. + 2.*d))/(1. - m - d);
  /* normal potential */
  for (i = 16, sum = 0.; i >= 0; i -= 2)
               sum = 1./(i + 1.) - e2p*sum;
  U0 = GM/b*(sum + m/3);

  /* meridian quadrant */
  /* Koenig und Weise p.50 (96), p.19 (38b), p.5 (2) */
  n2 = n*n;
  Q  = a*M_PI_2/(1. + n)*(1. + n2*(1./4. + n2*(1./64. + n2/256.)));

  /* output of results */
  
  *(e+0) = a;
  *(e+1) = f;
  *(e+2) = J2;
  *(e+3) = GM;
  *(e+4) = omg;
  *(e+5) = U0;
  *(e+6) = geq;
  *(e+7) = ksf;
  *(e+8) = Q;

 /*all documentation should be moved to another function */
  return res;

/* remove macro names */
#undef  GEQ
#undef  KM
#undef  OMEGA

#undef  GEQ84
#undef  KMW84

}

