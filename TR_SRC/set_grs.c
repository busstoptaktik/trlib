

/* set_grs version 2009.1            # page 1   30 Sep 2009 13 16 */


/* Copyright (c) 2009, Danish National Space Center, DTU       */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of DNSC, DTU    */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */

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
#include <sys/stat.h>
#include "trthread.h"

#ifndef    M_PI
#include   "kms_math.h"
#endif
extern THREAD_SAFE FILE               *def_lab_file;
extern THREAD_SAFE size_t              init_prj_pos, init_grs_pos;

int          set_grs(
/*_________________*/
int          ell_nmb,
char        *ellname,
double      *e,
...
)

{
#include   "fgetln.h"
#include   "fgetlhtx.h"
#include   "i_tabdir_file.h"

  static THREAD_SAFE  int                  e_quest = 0;
  static THREAD_SAFE  size_t               pos = 0;
  char                        pth_mlb[512];
  char                        s_name[24], e_name[24], *ell_nm = s_name;
  char                       *p_tp, param_v[24];

  int                         empty = 1, doc = 0, mode = 0, mode_234;
  int                         e_no, i, p = 0, qr, res = 0, used;
  double                      fh, n2, toise_t_m = 0.0;
  double                      m, qd, qdp, d, omg2, n, Q;
  double                      a, b, amg, J2 = 0.0, f = 0.0, e2p, geq;
  double                      GM, ksf, U0, sum;
  va_list                     gpu;
  FILE                       *out = NULL;
  double   a_m;       /* Semi-major axis or mean degree    */
  double   J2f;       /* J2 or 1/f depending on definition */
  double   GMg;       /* GM or geq         -"-             */
  double   omg;       /* rotation velocity                 */


/* set_grs version 2009.1            # page 2   30 Sep 2009 13 16 */


#define  GEQ      978049.e-5      /* Equator qravity    */
#define  KM       3986005.e8      /* Grav. const * Mass */
#define  OMEGA    7292115.0e-11   /* Rotation velocity  */

/* Defemce Mapping Agency, Dept. of Defence World Geodetic System 1984,
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


/* set_grs version 2009.1            # page 3   30 Sep 2009 13 16 */



  if (def_lab_file == NULL || init_prj_pos == 0) {
    (void) i_tabdir_file(3, "", &res, pth_mlb);
    if (res) {
      (void) fprintf(stdout, "\n*** def_lab.txt: %s %s;\n", pth_mlb,
        (res == -1) ? "NOT FOUND" :
        (res == -2) ? "NOT lab definition file" :
                      "Content not accepted");
      return (-2);
    }
  }

  if (init_grs_pos) {
    (void) fseek(def_lab_file, (long) init_grs_pos, SEEK_SET);
  } else { // search "def_grs"
    qr = 1;
    do {
      if ((res = fgetlhtx(def_lab_file, e_name)) == 0) {
        if (!strcmp("def_grs", e_name)) qr = 0;
      }
    } while (qr && res != EOF);
    if (qr) {
      (void) fprintf(stdout, 
         "\n*** set_grs: lab_def.txt GRS not found ;\n");
      return (-2);
    }
    init_grs_pos = ftell(def_lab_file);
  }
  if (e_quest <= 0) pos = init_grs_pos;
  /* GRS definition file detected */


/* set_grs version 2009.1            # page 4   30 Sep 2009 13 16 */


  if (ell_nmb == -1) {
    (void) strcpy(s_name, ellname);
    switch(*s_name) {
    case '!': /* list of ell-names */
    case '?': /* documentation     */
    case '*': /* full digit docum. */
      if      (*s_name == '*') p = 12;
      else if (*s_name == '?') p =  6;
      doc = 1;
      if (!strlen(++ell_nm)) {
        if (e_quest > 0) (void) fseek(def_lab_file, (long) pos, SEEK_SET);
        else {
          pos     = ftell(def_lab_file);
          e_quest = -1;
        }
        empty = -1;
      } else {
        e_quest = 0;
      }

      break;

    case '#': /* userdefined ellipsoid */
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
      if (!strlen(++ell_nm)) (void) sprintf(ell_nm, "%d",0);
      res = empty = 0;
      break;
    default: /* terminate scan */
      break;
    }


/* set_grs version 2009.1            # page 5   30 Sep 2009 13 16 */


    /* search for ellname */
    if (empty)
    do {
      qr = fgetlhtx(def_lab_file, e_name);
      if (qr != EOF) {
        p_tp = fgets(pth_mlb, 512, def_lab_file);
        if (strlen(pth_mlb) == 1)
            p_tp = fgets(pth_mlb, 512, def_lab_file);
        if (!strcmp(e_name, ell_nm) || empty == -1) {
          (void) sscanf(pth_mlb, "%d%n", &e_no, &used);
          p_tp  = pth_mlb + used;
          res   = e_no; /* ellip no */
          /* collect mode and definining params */
          /*____________________________________*/
          (void) sscanf(p_tp, "%d%n", &mode, &used);
          p_tp += used;

          /* a_m, J2f, GMg, omg */
          (void) sscanf(p_tp, "%lf%n", &a_m, &used);
          p_tp += used;
          (void) sscanf(p_tp, "%le%n", &J2f, &used);
          p_tp += used;
          while(isspace(*p_tp)) ++ p_tp;
          if (isalpha(*p_tp)) {
            (void) sscanf(p_tp, "%s%n", param_v, &used);
            if (strcmp(param_v, "KMW84") == 0) GMg = KMW84; 
            else
            if (strcmp(param_v, "KM") == 0) GMg = KM; 
            else
            if (strcmp(param_v, "GEQ") == 0) GMg = GEQ; 
            else {
              (void) sscanf(p_tp +3, "%lf%n", &sum, &used);
              used += 3;
              switch (*(p_tp +2)) {
              case '*': GMg = KM * sum; break;
              case '+': GMg = KM + sum; break;
              case '-': GMg = KM - sum; break;
              case '/': GMg = KM / sum; break;
              }
            }
          }
          else (void) sscanf(p_tp, "%le%n", &GMg, &used);
          p_tp += used;
          while(isspace(*p_tp)) ++ p_tp;
          if (isalpha(*p_tp)) {
            (void) sscanf(p_tp, "%s%n", param_v, &used);
            if (strcmp(param_v, "OMEGA") == 0) omg = OMEGA; 
            else omg = 0.0; 
          }
          else (void) sscanf(p_tp, "%le%n", &omg, &used);
          empty = 0;
        }
      }
    } while (res <= 0 && strcmp(e_name, "stop"));
    if (e_quest) pos = ftell(def_lab_file);
  }


/* set_grs version 2009.1            # page 6   30 Sep 2009 13 16 */


  else {
    do {
      qr = fgetlhtx(def_lab_file, e_name);
      if (qr != EOF) {
        qr = fgetln(pth_mlb, &used, def_lab_file);
        (void) sscanf(pth_mlb, "%d%n", &e_no, &used);
        if (e_no == ell_nmb) {
          empty = 0;
          p_tp  = pth_mlb + used;
          /* collect datum no */
          res   = e_no;

          /* collect mode and definining params */
          /*____________________________________*/
          (void) sscanf(p_tp, "%d%n", &mode, &used);
          p_tp += used;

          /* a_m, J2f, GMg, omg */
          (void) sscanf(p_tp, "%lf%n", &a_m, &used);
          p_tp += used;
          (void) sscanf(p_tp, "%le%n", &J2f, &used);
          p_tp += used +2;

          while(isspace(*p_tp)) ++ p_tp;
          if (isalpha(*p_tp)) {
            (void) sscanf(p_tp, "%s%n", param_v, &used);
            if (strcmp(param_v, "KMW84") == 0) GMg = KMW84; 
            else
            if (strcmp(param_v, "KM") == 0) GMg = KM; 
            else
            if (strcmp(param_v, "GEQ") == 0) GMg = GEQ; 
            else {
              (void) sscanf(p_tp +3, "%lf%n", &sum, &used);
              used += 3;
              switch (*(p_tp +2)) {
              case '*': GMg = KM * sum; break;
              case '+': GMg = KM + sum; break;
              case '-': GMg = KM - sum; break;
              case '/': GMg = KM / sum; break;
              }
            }
          }
          else (void) sscanf(p_tp, "%le%n", &GMg, &used);
          p_tp += used;
          while(isspace(*p_tp)) ++ p_tp;
          if (isalpha(*p_tp)) {
            (void) sscanf(p_tp, "%s%n", param_v, &used);
            if (strcmp(param_v, "OMEGA") == 0) omg = OMEGA; 
            else omg = 0.0; 
          }
          else (void) sscanf(p_tp, "%le%n", &omg, &used);
          empty = 0;
        }
      }
    } while (res <= 0 && strcmp(e_name, "stop"));
  }


/* set_grs version 2009.1            # page 7   30 Sep 2009 13 16 */


  if (doc) {
    if (e_no == -1) {
      pos     = 0;
      e_quest = 0;
      return(-1);
    }
  }

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


/* set_grs version 2009.1            # page 8   30 Sep 2009 13 16 */


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


/* set_grs version 2009.1            # page 9   30 Sep 2009 13 16 */


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
    (void) fprintf(stdout,
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
  (void) strcpy(ellname, e_name);
  *(e+0) = a;
  *(e+1) = f;
  *(e+2) = J2;
  *(e+3) = GM;
  *(e+4) = omg;
  *(e+5) = U0;
  *(e+6) = geq;
  *(e+7) = ksf;
  *(e+8) = Q;


/* set_grs version 2009.1            # page 10   30 Sep 2009 13 16 */


  /* documentation of params */
  /*_________________________*/
  if (doc) {

    va_start(gpu, e);
    out   = va_arg(gpu, FILE *);
    va_end(gpu);
    if (e_quest == -1) {
      if (!p && e_quest == -1) {
        (void) fprintf(out, "\n\n   Ellipsoid    KMS-nr");
        (void) fprintf(out, "             a            1/f");
      }
      e_quest = 1;
    }

    (void) fprintf(out, ((p) ? "\n\nEllipsoid = %14s\nKMS_nmb: %17d"
                             : "\n   %-12s %4d"), e_name, e_no);


/* set_grs version 2009.1            # page 11   30 Sep 2009 13 16 */


    if (p) { /* output of constants */
      (void) fprintf(out, "\n\na   = %20.*g m       definition.",
                     p+6, a);
      if (mode == 4  && p == 12) { /* def by mean degree */
        (void) fprintf(out, "\nmg  = %20.*g", p+6, amg);
        if (toise_t_m != 1.0)
           (void) fprintf(out, " toises  m/toise = %14.12f", toise_t_m);
        else (void) fprintf(out, " m");
      }
      if (f != 0.0) {
        (void) fprintf(out, "\n1/f = %20.*g", p+6, 1.0/f);
        if (mode != 1) (void) fprintf(out, "         definition.");
      }
      if (p == 12 || f == 0.0) {
        (void) fprintf(out, "\nf   =   %17.*e", p+4, f);
        if (mode != 1) (void) fprintf(out, "     definition.");
      }
      (void) fprintf(out, "\nJ2  = %20.*e", p, J2);
      if (mode == 1) (void) fprintf(out, "         definition.");
      (void) fprintf(out, "\nGM  = %20.*e m3/s2", p, GM);
      if ((mode == 1) || (mode == 3) || (mode == 4))
          (void) fprintf(out, "   definition.");
      (void) fprintf(out, "\nomg = %20.*e rad/s   definition.",
                     p, omg);
      (void) fprintf(out, "\nU0  = %20.*e m2/s2", p, U0);
      (void) fprintf(out, "\ngeq = %20.*e m/s2", p, geq);
      if (mode == 2) (void) fprintf(out, "    definition.");
      (void) fprintf(out, "\nksf = %20.*e", p, ksf);
      (void) fprintf(out, "\nMkv = %20.*g m", p+6, Q);
      (void) fprintf(out, "\n");
      if (e_no) {
        do {
          qr = fgetc(def_lab_file);
          if (qr != EOF) qr = '"';
          else
          if (qr != '"') (void) fputc((qr != '¨') ? qr : '#', out);
        } while (qr != '"');
      } else {
        (void) fprintf(out,"\nUser-defined Geodetic Reference System.");
        (void) fprintf(out,"\nDefining parameters are indicated.");
      }
    } /* end output of constants */
    else /* output of a and f */ {
      (void) fprintf(out, "%6s%14.4f m", " ", a);
      if (f != 0.0) (void) fprintf(out, "%16.8f", 1.0/f);
      else          (void) fprintf(out, "  Sfærisk");
    }

  }

  return(e_no);

/* remove macro names */
#undef  GEQ
#undef  KM
#undef  OMEGA

#undef  GEQ84
#undef  KMW84

}

