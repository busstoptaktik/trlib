

/* set_trc                        # page 1   22 Oct 2002 10 45 */


/* Copyright (c) 1992 GD, U/32, Kort-og Matrikelstyrelsen, Denmark */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of G/S, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include    <stdio.h>
#include    <math.h>
#include    "geo_lab.h"

#ifndef    M_PI
#include   "kms_math.h"
#endif

int                     set_trc(
/*____________________________*/
struct coord_lab         *c_lab
)

{

#include            "clenshaw.h"
#include            "erad.h"
#include            "gatg.h"

  double                 n, np;
  double                 *gb = c_lab->tcgg, *bg = gb + 5;
  double                 dCn, dCe, Z;
  double                 *utg = c_lab->utg, *gtu = c_lab->gtu;
  double                 rN, rS, rM, pN, pS, pM;
  double                 tanN, tanS, tanM, Bc, Cn;
  double                 e;
  int                    res = -1; /* not a regular coord_lab */

  if (c_lab->lab_type == 1) {
    /* Initialize the transformation constants */

    res = c_lab->cstm;

    /* reset initword */
    c_lab->init = 1;

    /* test flattening */
    if (c_lab->f >= 1.0)  c_lab->f = 1.0/c_lab->f;

    /* transformation tolerances */
    /* 0.000 001 sx = 30 my on the ellipsoid */
    c_lab->tol = 1.0e-6 * (c_lab->a) * M_PI/180./60./60.;

    /* third flattening */
    np = n = c_lab->f/(2.0 - c_lab->f);


/* set_trc                        # page 2   22 Oct 2002 10 45 */


    /* COEF. OF TRIG SERIES GEO <-> GAUSS */
    /* gb := Gaussian -> Geodetic, KW p190 - 191 (61) - (62) */
    /* bg := Geodetic -> Gaussian, KW p186 - 187 (51) - (52) */
    /* 5 degree : Engsager and Poder: ICC2007 */
    bg  = gb + 5;
    *gb = n*( 2.0 + n*(-2.0/3.0  + n*(-2.0      + n*(116.0/45.0 + n*(26.0/45.0)))));
    *bg = n*(-2.0 + n*( 2.0/3.0  + n*( 4.0/3.0  + n*(-82.0/45.0 + n*(32.0/45.0)))));
    np     *= n;
    *(gb+1) = np*(7.0/3.0     + n*( -8.0/5.0  + n*(-227.0/45.0 + n*(2704.0/315.0))));
    *(bg+1) = np*(5.0/3.0     + n*(-16.0/15.0 + n*( -13.0/ 9.0 + n*( 904.0/315.0))));
    np     *= n;
    *(gb+2) = np*( 56.0/15.0  + n*(-136.0/35.0 + n*(1262.0/105.0)));
    *(bg+2) = np*(-26.0/15.0  + n*(  34.0/21.0 + n*(   8.0/  5.0)));
    np     *= n;
    *(gb+3) = np*(4279.0/630.0 + n*(-322.0/35.0));
    *(bg+3) = np*(1237.0/630.0 + n*( -12.0/ 5.0));
    np     *= n;
    *(gb+4) = np*(4174.0/315.0);
    *(bg+4) = np*(-734.0/315.0);

    /* Constants of the projections */
    switch(c_lab->cstm) {
    case  3: /* Transverse Mercator (UTM, ITM, etc) */
    case 12: /* rt90v and rt38v, spec. for dks-fools*/
      np = n*n;
      /* Norm. mer. quad, K&W p.50 (96), p.19 (38b), p.5 (2) */
      c_lab->Qn = c_lab->a*c_lab->scale/(1 + n)
                * (1. + np*(1./4.0 + np*(1./64.0 + np/256.0)));
      /* coef of trig series */
      /* utg := ell. N, E -> sph. N, E,  KW p194 (65) */
      /* gtu := sph. N, E -> ell. N, E,  KW p196 (69) */
      *utg = n*(-0.5  + n*( 2.0/3.0 + n*(-37.0/96.0 + n*( 1.0/360.0 + n*(  81.0/512.0)))));
      *gtu = n*( 0.5  + n*(-2.0/3.0 + n*(  5.0/16.0 + n*(41.0/180.0 + n*(-127.0/288.0)))));
      *(utg+1) = np*(-1.0/48.0 + n*(-1.0/15.0 + n*(437.0/1440.0 + n*(-46.0/105.0))));
      *(gtu+1) = np*(13.0/48.0 + n*(-3.0/5.0  + n*(557.0/1440.0 + n*(281.0/630.0))));
      np      *= n;
      *(utg+2) = np*(-17.0/480.0 + n*(  37.0/840.0 + n*(  209.0/ 4480.0)));
      *(gtu+2) = np*( 61.0/240.0 + n*(-103.0/140.0 + n*(15061.0/26880.0)));
      np      *= n;
      *(utg+3) = np*(-4397.0/161280.0 + n*(  11.0/504.0));
      *(gtu+3) = np*(49561.0/161280.0 + n*(-179.0/168.0));
      np     *= n;
      *(utg+4) = np*(-4583.0/161280.0);
      *(gtu+4) = np*(34729.0/ 80640.0);
      /* Gaussian latitude value of the origin latitude */
      Z        = gatg(c_lab->tcgg, -1, c_lab->B0);
      /* Origin northing minus true northing at the origin latitude */
      /* i.e. true northing = N - c_lab->Zb                         */
      c_lab->Zb = c_lab->N0 - c_lab->Qn*(Z
                + clenshaw('s', gtu, 5, 2.0*Z, 0.0, &dCn, &dCe));
      break;

    case 4: /* Mercator */
      /* WEB_Mercator:: f = 0.0, n = 0.0, tcgg = 0.0 */
      c_lab->Qn = erad(c_lab->a, c_lab->f, c_lab->B0, 'r');
      /* False Northing - Northing from Equator */
      c_lab->Zb = c_lab->N0 + c_lab->Qn *log(tan(M_PI_4
                - gatg(c_lab->tcgg, -1, c_lab->B0)/2.0));
      break;


/* set_trc                        # page 3   22 Oct 2002 10 45 */


    case 5: /* Lambert */
    case 9: /* GS conf. con. */
      switch(c_lab->mode) {
      case 1: /* 1 std par */
        c_lab->cP = sin(c_lab->B1);
        Cn        = erad(c_lab->a, c_lab->f,
                         c_lab->B1, 'N')/tan(c_lab->B1);
        c_lab->Qn = Cn/pow(tan(M_PI_4
                  - gatg(c_lab->tcgg, -1, c_lab->B1)/2.0), c_lab->cP);
        break;
      case 2: /* 2 std par */
      case 3: /* 2 lim par */
        rN   = erad(c_lab->a, c_lab->f, c_lab->B2, 'r');
        rS   = erad(c_lab->a, c_lab->f, c_lab->B1, 'r');
        tanN = tan(M_PI_4
             - gatg(c_lab->tcgg, -1, c_lab->B2)/2.0);
        tanS = tan(M_PI_4
             - gatg(c_lab->tcgg, -1, c_lab->B1)/2.0);
        if (fabs(c_lab->B2 - c_lab->B1) >= 0.1/180.*M_PI) {
          /* lat diff >= 0.1 dg */
          c_lab->cP = log(rN/rS)/log(tanN/tanS);
          Bc        = asin(c_lab->cP);
        }
        else {
          Bc        = (c_lab->B2 + c_lab->B1)/2.0;
          c_lab->cP = sin(Bc);
        }
        rM           = erad(c_lab->a, c_lab->f, Bc, 'r');
        tanM         = tan(M_PI_4
                     - gatg(c_lab->tcgg, -1, Bc)/2.0);
        pN           = rN/pow(tanN, c_lab->cP);
        pS           = rS/pow(tanS, c_lab->cP);
        pM           = rM/pow(tanM, c_lab->cP);
        c_lab->Qn    = (c_lab->mode == 2)
                     ? (pN + pS)/(c_lab->cP)/2.0
                     : 4.0/c_lab->cP/(1.0/pN + 2.0/pM + 1.0/pS);
        c_lab->scale = c_lab->scale*c_lab->Qn/pM*(c_lab->cP);
        break;
      }
      c_lab->Zb = c_lab->Qn*pow(tan(M_PI_4
                  - gatg(c_lab->tcgg, -1, c_lab->B0)/2.0), c_lab->cP)
                + c_lab->N0;
      break;



/* set_trc                        # page 4   22 Oct 2002 10 45 */


    case 6: /* Stereographic coordinates */
      e = sqrt(c_lab->f*(2.0 - c_lab->f));
      e = pow((1.0 - e)/(1.0 + e), e/2.0);
      switch(c_lab->mode) {
      case  1: /* N-polar stereographic, no scale eqlz. */
      case -1: /* S-polar stereographic, no scale eqlz. */
      case  2: /* upsn: UPS, north */
      case -2: /* upss: UPS, south */
        c_lab->Qn = 2.0*c_lab->scale * c_lab->a/(1. -c_lab->f) *e;
        c_lab->Zb = c_lab->N0;
        break;
      case 3: /* stereographic, no scale eqlz. */
        c_lab->Qn = 2.0*c_lab->scale
                  * erad(c_lab->a, c_lab->f, c_lab->B0, 'N');
        c_lab->B1 = M_PI_2 - c_lab->B0;
        /* Aux const. for mapping in Soldner sph. */
        c_lab->cP = tan(c_lab->B1/2.0)
                  / tan(M_PI_4 - gatg(c_lab->tcgg, -1, c_lab->B0)/2.0);
        c_lab->Zb = c_lab->N0;
        break;
      case 4: /* stereographic, scale eqlz. */
        c_lab->Qn    = 2.0*erad(c_lab->a, c_lab->f, c_lab->B0, 'N');
        c_lab->scale = 1.0- pow(c_lab->N0/c_lab->Qn, 2.0)/2.0;
        c_lab->Qn    = c_lab->Qn*c_lab->scale;
        c_lab->B1    = M_PI_2 - c_lab->B0;
        /* Aux const. for mapping in Soldner sph. */
        c_lab->cP = tan(c_lab->B1/2.0)
                  / tan(M_PI_4 - gatg(c_lab->tcgg, -1, c_lab->B0)/2.0);
        c_lab->Zb = c_lab->N0;
        break;
      case  5: /* N-polar stereographic, unit scale at B1 */
      case -5: /* S-polar stereographic, unit scale at B1 */
        c_lab->Qn     = 2.0*c_lab->a*(1.0-c_lab->f)*e;
        c_lab->Zb     = c_lab->N0;
        pM            = c_lab->Qn * tan(M_PI_4 
                            - gatg(c_lab->tcgg, -1, c_lab->B1)/2.0)
                      / erad(c_lab->a, c_lab->f, c_lab->B1, 'N')
                      / cos(c_lab->B1);
        c_lab->scale /= pM;
        c_lab->Qn    /= pM;
        break;
      }
      break;


/* set_trc                        # page 5   22 Oct 2002 10 45 */

      case 7: /* Equivalent projections */

      switch(c_lab->mode) {
      case 1: /* Sansom-Flamsteed, ellipsoidal  */
      case 2: /* Sansom-Flamsteed, authalic     */
      case 3: /* Lambert cylindrical equival.   */
      case 4: /* Lambert polar equivalent.  (N) */
      case 5: /* Lambert polar equivalent.  (S) */
      case 6: /* Authalic geogr.                */
      case 7: /* Lambert azimuthal equivalent   */

        /* K&W p 62, eq for 1/8 Oberfläche */
        /* The radius of a sphere with same surface */
        c_lab->Zb = c_lab->a * sqrt((1.0-n)/(1.0+n) *
                    (1.0 + n*(2.0/3.0 + n*(14.0/15.0
                    + n*(34.0/35.0 + n*(62.0/63.0 + n*98.0/99.0))))));

        /* for tolerance test used in ptg */
        c_lab->Qn = c_lab->Zb;

        /* K.Engsager & K.Poder : Equivalent projections */
        /* COEF. OF TRIG SERIES GEO <-> AUTHALIC */
        /* gb := Authalic -> Geodetic, EP pxx - xx (yy) - (yy) */
        /* bg := Geodetic -> Authalic, EP pxx - xx (yy) - (yy) */
        bg  = gb + 5;
        np  = n;
        *gb = n*(+4.0/3.0 +n*(+4.0/45.0 +n*(-16.0/35.0
             +n*(-2582.0/14175.0 +n*(+60136.0/467775.0)))));
        *bg = n*(-4.0/3.0 +n*(-4.0/45.0 +n*(+88.0/315.0
             +n*( +538.0/4725.0  +n*(+20824.0/467775.0)))));
        np     *= n;
        *(gb+1) = np*(+46.0/45.0 +n*(+152.0/945.0
                 + n*(-11966.0/14175.0 +n*( -21016.0/51975.0))));
        *(bg+1) = np*(+34.0/45.0 +n*(  +8.0/105.0
                 + n*( -2482.0/14175.0 +n*(-37192.0/467775.0))));
        np     *= n;
        *(gb+2) = np*(+3044.0/2835.0 +n*(+3802.0/14175.0
                 + n*(-94388.0/66825.0)));
        *(bg+2) = np*(-1532.0/2835.0 +n*(-898.0/14175.0
                 + n*( +54968.0/467775.0)));
        np     *= n;
        *(gb+3) = np*(+6059.0/4725.0  +n*(+41072.0/93555.0));
        *(bg+3) = np*(+6007.0/14175.0 +n*(+24496.0/467775.0));
        np     *= n;
        *(gb+4) = np*(+768272.0/467775.0);
        *(bg+4) = np*(   -23356.0/66825.0);
        if (c_lab->mode == 7) {
          Bc        = c_lab->B0 + clenshaw('s', 
                             bg, 5, 2.0*c_lab->B0, 0.0, &dCn, &dCe);
          *gtu      = sin(Bc); 
          *(gtu+1)  = cos(Bc); 
          Z         = sin(c_lab->B0); 
          Z         = sqrt(1 - c_lab->f * (2-c_lab->f) * Z * Z); 
          c_lab->cP = c_lab->a / c_lab->Zb
                    * cos(c_lab->B0) / (*(gtu+1) * Z);
        }
        break;


/* set_trc                        # page 6   22 Oct 2002 10 45 */


      default:
        (void) fprintf(stderr, "\n*** set_trc: unintelligible ");
        (void) fprintf(stderr,
                      "mode %d for equiv. projections", c_lab->mode);
        return(-1);
      } /* end equiv. modes */

    break;  /* equiv. case */


    case 8: /* system 1934/45 */

      /* set half Gaussian curvature for s34 proj. corr. */
      c_lab->cP = c_lab->Zb = 0.5 *
                erad(c_lab->a*c_lab->scale, c_lab->f, c_lab->B0, 'G');
      break;

    default: /* Other projections, no action */
      break;
    } /* switch(c_lab->cstm) */

  } /* end of init transf-const.) */

  return(res);
}


