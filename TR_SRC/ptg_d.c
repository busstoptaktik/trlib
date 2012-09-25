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
 


/* ptg_d   ver 2010.01           # page 1   12 Jan  2010 10 41 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "geo_lab.h"
#include "trthread.h"

#ifndef    M_PI
#include   "kms_math.h"
#endif


int                    ptg_d(
/*_________________________*/
union geo_lab         *TC_u,
int                    direct,
double                 N,
double                 E,
double                *Nout,
double                *Eout,
char                  *usertxt,
FILE                  *tr_error,
double                *D,
double                *Dgtu
)

{
/*
D                 double                           return
The values m and c for transformation from regular coordinate
system to geographical.

Dgtu              double                           call
The constants for the trig series to calc 1/m and c.
*/

#include          "clenshaw.h"
#include          "gatg.h"
#include          "v_red.h"
#include          "erad.h"
#include          "sftr.h"
#include          "t_status.h"
#include          "setDtrc.h"
  static THREAD_SAFE  int        D_init = 0, s_ell_no = 0;


  struct coord_lab  *TC = &(TC_u->u_c_lab);
  int               res = 0, i, s, h, cstm, mode, ell_trf, auth = 0;
  int               mrc, lmb, ste, spl, non_p, neg;
  double            Cn, Ce, dCn = 0.0, dCe = 0.0, dCt, Z, ZZ;
  double            sin_Cn, cos_Cn = 0.0, sin_Ce, cos_Ce;
  double            *GP, *utg, *gtu;
  char              *ptx;

  /* test labtype of TC_u */
  if (TC->lab_type == CRD_LAB) {



/* ptg_d   ver 2010.01           # page 2   12 Jan  2010 10 41 */


    /* Transf. parameters */
    if (TC->init) {

      /* coord system params and addr */
      cstm   = TC->cstm;
      mode   = TC->mode;
      GP     = TC->tcgg;
      utg    = TC->utg;
      gtu    = TC->gtu;
      neg    = TC->W_crd == 1;
      *D     = 1.0;
      *(D+1) = 0.0;

      /*TM*/
      if (((!D_init) || s_ell_no != TC->ellipsoid) && TC->cstm == 3) {
        D_init   = setDtrc(TC, Dgtu);
        s_ell_no = TC->ellipsoid;
      }

      if (D_init || TC->cstm != 2) {

        /* transformation sequence */
        if (direct) {
          i       = (direct > 0 ? 1 : -1);
          ell_trf = (direct == i);
          s       = h = -i;
          if (-10 <= direct && direct <= 10) h += h;
          if (direct < 0) E = v_red(E);
  
          Cn = N;
          Ce = E;


/* ptg_d   ver 2010.01           # page 3   12 Jan  2010 10 41 */


          switch(cstm) {
          case 3: /* Transverse mercator */
            for ( ; (h - i); i += s) {
              switch(i) {
              case 1: /* UTM -> GEO */
                /* normalize N, E */
                Cn = (Cn - TC->Zb)/(TC->Qn);
                Ce = (Ce - TC->E0)/(TC->Qn);
                if (fabs(Ce) <= 2.623395162778) {
                  if (fabs(Ce) > 1.14691472225) 
                     res = (fabs(Ce) > 2.2938245694606)
                         ? TRF_AREA_ : TRF_INACC_;
                  if (neg) Ce = -Ce;
                  /* norm. N, E -> compl. sph. LAT, LNG */
                  Cn += clenshaw('S', utg, 5, 2.*Cn, 2.*Ce, &dCn, &dCe);
                  Ce += dCe;
                  Ce = 2.0*(atan(exp(Ce)) - M_PI_4);
                  /* Scale and meridian conv. */
                  dCn = clenshaw('C', Dgtu, 4, 2.*Cn, 2.*Ce, &dCn, &dCe);
                  /* compl. sph. LAT -> Gaussian LAT, LNG */
                  cos_Cn = cos(Cn);
                  sin_Ce = sin(Ce);
                  cos_Ce = cos(Ce);
                  Ce     = atan2(sin_Ce, cos_Ce*cos_Cn);
#ifdef  WIN32
                  Cn     = atan2(sin(Cn)*cos_Ce,
                           _hypot(sin_Ce, cos_Ce*cos_Cn));
#else
                  Cn     = atan2(sin(Cn)*cos_Ce,
                           hypot(sin_Ce, cos_Ce*cos_Cn));
#endif
                  /* Scale and meridan conv. KW p247 (52) */
                  dCn   -= clenshaw('c', Dgtu, 4, 2.*Cn, 0.0, &dCt, &dCt);
                  sin_Ce = sin(Ce);
                  cos_Cn = cos(Cn) * sin_Ce;
                  dCn    = exp(dCn) / (sqrt(1 - cos_Cn * cos_Cn));
                  dCe   -= atan2(sin_Ce * sin(Cn), cos(Ce));
                  *D     = TC->scale * dCn;
                  *(D+1) = -dCe;
                  /* Gaussian LAT, LNG -> ell. LAT, LNG */
                  if (ell_trf) Cn = gatg(GP,  1, Cn);
                  Ce = v_red(Ce + TC->L0);
                }
                else res = TRF_AREA_;
                break;
              case -1: /* GEO -> UTM */
                /* ell. LAT, LNG -> Gaussian LAT, LNG */
                if (ell_trf) Cn = gatg(GP, -1, Cn);
                Ce -= TC->L0;
                if (neg) Ce = -Ce;
                /* Gaussian LAT, LNG -> compl. sph. LAT */
                sin_Cn = sin(Cn);
                cos_Cn = cos(Cn);
                cos_Ce = cos(Ce);
                Cn     = atan2(sin_Cn, cos_Ce*cos_Cn);
#ifdef  WIN32
                Ce     = atan2(sin(Ce)*cos_Cn,
                         _hypot(sin_Cn, cos_Cn*cos_Ce));
#else
                Ce     = atan2(sin(Ce)*cos_Cn,
                         hypot(sin_Cn, cos_Cn*cos_Ce));
#endif
                /* compl. sph. N, E -> ell. norm. N, E */
                Ce  = log(tan(M_PI_4 + Ce*0.5));
                Cn += clenshaw('S', gtu, 5, 2.*Cn, 2.*Ce, &dCn, &dCe);
                Ce += dCe;
                if (fabs(Ce) <= 2.623395162778) {
                  if (fabs(Ce) > 1.14691472225)
                     res = (fabs(Ce) > 2.2938245694606)
                         ? TRF_AREA_ : TRF_INACC_;
                  /* ell. norm. N, E -> metric N, E */
                  Cn  = (TC->Qn) * Cn + TC->Zb;  /* Northing */
                  Ce  = (TC->Qn) * Ce + TC->E0;  /* Easting  */
                }
                else res = TRF_AREA_;
                break;


/* ptg_d   ver 2010.01           # page 4   12 Jan  2010 10 41 */


              case 0: /* transfer results */
                *Nout = Cn;
                *Eout = Ce;
                if (res <= TRF_AREA_) i += s; /* stop */
                break;
              }
            }
            break;
  
          case 4: /* Mercator */
          case 5: /* Lambert  */
          case 9: /* GS conf. con. */
          case 6: /* Stereogr.*/
            if (cstm == 4) {
              lmb = ste = spl = non_p = 0;
              mrc = 1;
              if (direct > 0) {
                Ce = M_PI * TC->Qn;
                if (E - TC->E0 >  Ce) E -= 2.0 * Ce;
                else
                if (E - TC->E0 < -Ce) E += 2.0 * Ce;
                Ce = E;
              }
              if (mode == 3) ell_trf = 0; /* webmrc */
            } else {
              mrc   = 0;
              lmb   = cstm == 5 || cstm == 9;
              ste   = cstm == 6;
              spl   = ste && (mode == -1 || mode == -2);
              non_p = ste && (mode == 3 || mode == 4);
            }
  
            for ( ; h - i; i += s) {
              switch(i) {
              case 1: /* PRJ -> GEO */
                /* N, E -> local normalized -N, E */
                Cn = (TC->Zb - Cn)/TC->Qn;
                Ce = (Ce - TC->E0)/TC->Qn;
                if (spl) Cn = -Cn;
                if (neg) Ce = -Ce;
                if (mrc) Z  = exp(Cn);
                else { /* Lambert and stereographic */
                  /* Polar coord */
#ifdef  WIN32
                  Z  = _hypot(Cn, Ce);
#else
                  Z  = hypot(Cn, Ce);
#endif
                  if (Z > 57200000.0) res = TRF_AREA_;
                  else {
                    dCn = Z;
                    Ce  = atan2(Ce, Cn);
                    if (lmb) {
                      Z   = pow(Z, 1.0/TC->cP);
                      Ce /= TC->cP;
                    }
                  }
                }
                if (res != TRF_AREA_) {
                  /*Gaussian LAT */
                  Cn = 2.0*(M_PI_4 - atan(Z));
                  if (non_p) { /* Soldner local geo -> Gaussian */
                    Ce = M_PI - Ce;
                    Cn = sftr(Ce, TC->B1, M_PI_2 - Cn, &Z, &Ce);
                    Cn = M_PI_2 - 2.0*atan(tan(Cn*0.5)/TC->cP);
                  }
                  if (ell_trf) Cn = gatg(GP,  1, Cn);
                  if (spl)     Cn = -Cn;
                  *D = TC->Qn / erad(TC->a, TC->f, Cn, 'r');
                  if (mrc) {
                    *(D+1) = 0.0;
                  } else
                  if (ste) {
                    *D    *= dCn;
                    *(D+1) = (spl) ? -Ce : Ce;
                  } else {
                    *D    *= TC->cP * dCn;
                    *(D+1) = TC->cP * Ce;
                  }
                  Ce     = v_red(Ce + TC->L0);
                }
                break;
  


/* ptg_d   ver 2010.01           # page 5   12 Jan  2010 10 41 */


              case -1: /* GEO -> PRJ */
                /* cos Lat for check */
                cos_Cn = cos(Cn);
                /* ellipsoidal LAT, LNG -> Gaussian LAT, LNG */
                if (ell_trf) Cn = gatg(GP, -1, Cn);
                if (spl)     Cn = -Cn;
                Ce -= TC->L0;
                Ce  = v_red(Ce);
                if (neg) Ce = -Ce;
                if (non_p) { /* Gaussian -> Soldner local geo */
                  Cn = 2.0*atan(tan(M_PI_4 - Cn*0.5)*TC->cP);
                  Cn = M_PI_2 - sftr(Ce, TC->B1, Cn, &Z, &Ce);
                  Ce = M_PI - Ce;
                }
                /* Lambertian */
                Z  = tan(M_PI_4 - Cn*0.5);
                if (mrc) {
                  Cn = TC->Zb - TC->Qn*log(Z);
                  Ce = TC->E0 + TC->Qn*Ce;
                }
                else { /* Lambert and stereographic */
                  if (lmb) {
                    Z   = pow(Z, TC->cP);
                    Ce *= TC->cP;
                  }
                  /* Polar coord -> rectangular N, E */
                  Z *= TC->Qn;
                  Cn = -Z*cos(Ce);
                  if (spl) Cn = -Cn;
                  Cn = TC->Zb + Cn;
                  Ce = Z*sin(Ce) + TC->E0;
                }
                break;
  
              case 0: /* transfer results */
                *Nout = Cn;
                *Eout = Ce;
                break;
              }
              if (res <= TRF_AREA_) i += s; /* stop */
            }
            break;
  
  

/* ptg_d   ver 2010.01           # page 6   12 Jan  2010 10 41 */


          case 7: /* equivalent systems */

            switch(mode) {

            case 1: /* Sansom-Flamsteed, ellipsoidal */
              for ( ; h - i; i += s)
                switch(i) {

                case  1: /* PRJ -> GEO */
                  Cn = erad(TC->a, TC->f, Cn, 'L');
                  Z  = cos(Cn);
                  Ce = (Z != 0.0)
                     ?  Ce/erad(TC->a, TC->f, Cn, 'N')/Z : 0.0;
                  Ce = v_red(Ce + TC->L0);
                  break;

                case -1: /* GEO -> PRJ */
                  /* cos Lat for check */
                  Ce     = v_red(Ce - TC->L0);
                  cos_Cn = cos(Cn);
                  Ce     = erad(TC->a, TC->f, Cn, 'r')*Ce;
                  Cn     = erad(TC->a, TC->f, Cn, 'Q');
                  break;

                case  0: /* transfer results */
                  *Nout = Cn;
                  *Eout = Ce;
                  break;
                }
              break; /* safle */
  

/* ptg_d   ver 2010.01           # page 7   12 Jan  2010 10 41 */

  
            case 2: /* Sansom-Flamsteed, authalic */
              for ( ; h - i; i += s)
                switch(i) {
  
                case  1: /* PRJ -> GEO */
                  Cn -= TC->N0;
                  Ce -= TC->E0;
                  Cn /= TC->Zb;
                  Ce /= TC->Zb;
                  if (fabs(Cn) < M_PI_2+1.0e-9) {
                    /* authalic latitude  */
                    Z   = cos(Cn);
                    Ce  = (Z >= 1.0e-9)
                        ?  Ce/Z : 0.0;  /* longitude */
                    Ce  = v_red(Ce + TC->L0);
                    /* authalic to latitude  */
                    Cn += clenshaw('s', TC->tcgg, 5, 2.*Cn, 0.0,
                                   &dCn, &dCe);
                  } else res = TRF_AREA_;
                  break;
  
                case -1: /* GEO -> PRJ */
                  Ce     = v_red(Ce - TC->L0);
                  /* cos Lat for check */
                  cos_Cn = cos(Cn);
                  /* latitude to authalic latitude  */
                  Cn += clenshaw('s', TC->tcgg+5, 5, 2.*Cn, 0.0,
                                 &dCn, &dCe);
                  Ce = (TC->Zb * cos(Cn)) * Ce + TC->E0; /* easting  */
                  Cn =  TC->Zb *            Cn + TC->N0; /* northing */
                  break;
  
                case  0: /* transfer results */
                  *Nout = Cn;
                  *Eout = Ce;
                  break;
                }
              break; /* safl */
  

/* ptg_d   ver 2010.01           # page 8   12 Jan  2010 10 41 */

  
            case 3: /* Lambert equiv. cyl. proj., ellipsoidal */
              for ( ; h - i; i += s)
                switch(i) {
  
                case  1: /* PRJ -> GEO */
                  Cn -= TC->N0;
                  Ce -= TC->E0;
                  Cn /= TC->Zb;
                  if (fabs(Cn) < 1.0+1.0e-9) {
                    /* authalic latitude  */
                    Cn = (fabs(Cn) < 1.0) ? asin(Cn)
                       : ((Cn > 0.0) ? M_PI_2 : -M_PI_2);
                    Ce = v_red(Ce/TC->Zb + TC->L0);
                    /* authalic to latitude  */
                    Cn += clenshaw('s', TC->tcgg, 5, 2.*Cn, 0.0,
                                   &dCn, &dCe);
                  } else res = TRF_AREA_;
                  break;
  
                case -1: /* GEO -> PRJ */
                  Ce     = v_red(Ce - TC->L0);
                  /* cos Lat for check */
                  cos_Cn = cos(Cn);
                  /* latitude to authalic latitude  */
                  Cn += clenshaw('s', TC->tcgg+5, 5, 2.*Cn, 0.0,
                                 &dCn, &dCe);
                  Cn = TC->Zb*sin(Cn) + TC->N0;
                  Ce = TC->Zb*Ce      + TC->E0;
                  break;
  
                case  0: /* transfer results */
                  *Nout = Cn;
                  *Eout = Ce;
                  break;
                }
              break; /* lmbace */
  

/* ptg_d   ver 2010.01           # page 9   12 Jan  2010 10 41 */


            case 4: /* Lambert equiv. pol. proj., (N) */
            case 5: /* Lambert equiv. pol. proj., (S) */
              for ( ; h - i; i += s)
                switch(i) {
  
                case  1: /* PRJ -> GEO */
                  Cn -= TC->N0;
                  Ce -= TC->E0;
#ifdef  WIN32
                  Z  = 0.5 * _hypot(Cn, Ce) / TC->Zb;
#else
                  Z  = 0.5 * hypot(Cn, Ce) / TC->Zb;
#endif
                  if (fabs(Z) < 1.0+1.0e-9) {
                    Ce = v_red(atan2(Ce, Cn));
                    if (fabs(Z) < 1.0) {
                      /* authalic latitude  */
                      Cn  = M_PI_2 - 2.0*asin(Z);
                      /* geodetic latitude  */
                      Cn += clenshaw('s', TC->tcgg, 5, 2.*Cn, 0.0,
                                     &dCn, &dCe);
                    } else 
                      Cn = ((Z > 0.0) ? M_PI_2 : -M_PI_2);
                    if (mode == 5) Cn = -Cn;
                    Ce = v_red(Ce + TC->L0);
                  }
                  else res = TRF_AREA_;
                  break;
  
                case -1: /* GEO -> PRJ */
                  Ce     = v_red(Ce - TC->L0);
                  /* cos Lat for check */
                  cos_Cn = cos(Cn);
                  if (mode == 5) Cn = -Cn;
                  /* latitude to authalic latitude  */
                  Cn += clenshaw('s', TC->tcgg+5, 5, 2.*Cn, 0.0,
                                 &dCn, &dCe);
                  Z  = 2.0 * TC->Zb * sin(M_PI_4 - Cn*0.5);
                  Cn = Z*cos(Ce) + TC->N0;
                  Ce = Z*sin(Ce) + TC->E0;
                  break;
  
                case  0: /* transfer results */
                  *Nout = Cn;
                  *Eout = Ce;
                  if (res <= TRF_AREA_) i += s;
                  break;
                }
              break; /* lmbap, lmbaps */
  

/* ptg_d   ver 2010.01           # page 10   12 Jan  2010 10 41 */


            case 6: /* authalic geogr. */
              auth = 1;
              for ( ; h - i; i += s)
                switch(i) {
  
                case  1: /* PRJ -> GEO */
                  /* authalic latitude  */
                  Cn -= TC->N0;
                  Ce -= TC->E0;
                  /* geodetic latitude  */
                  Cn += clenshaw('s', TC->tcgg, 5, 2.*Cn, 0.0,
                                 &dCn, &dCe);
                  if (mode == 5) Cn = -Cn;
                  Ce = v_red(Ce + TC->L0);
                  break;
  
                case -1: /* GEO -> PRJ */
                  Ce = v_red(Ce - TC->L0);
                  /* cos Lat for check */
                  cos_Cn = cos(Cn);
                  if (mode == 5) Cn = -Cn;
                  /* geo latitude to authalic latitude  */
                  Cn += clenshaw('s', TC->tcgg+5, 5, 2.*Cn, 0.0,
                                 &dCn, &dCe);
                  Cn += TC->N0;
                  Ce += TC->E0;
                  break;
  
                case  0: /* transfer results */
                  *Nout = Cn;
                  *Eout = Ce;
                  if (res <= TRF_AREA_) i += s;
                  break;
                }
              break; /* authalic geogr. */
  

/* ptg_d   ver 2010.01           # page 11   12 Jan  2010 10 41 */


            case 7: /* Lambert Azimuthal Equal Area. */
             for ( ; h - i; i += s)
                switch(i) {
                case  1: /* PRJ -> GEO */
                  dCn    = (Cn - TC->N0) * TC->cP;
                  dCe    = (Ce - TC->E0) / TC->cP;
                  Z      = sqrt(dCn * dCn + dCe * dCe);
                  if (Z <= 2.0 * TC->Zb) {
                    Cn     = 2.0 * asin(0.5 * Z / TC->Zb);
                    sin_Cn = sin(Cn);
                    cos_Cn = cos(Cn);
                    /* authalic latitude  */
                    ZZ     = (Z < 1.0e-10) ? (dCn * *(TC->gtu+1))
                           : (dCn * *(TC->gtu+1) * sin_Cn / Z);
                    Cn     = asin(*TC->gtu * cos_Cn + ZZ);
                    Ce     = atan2(dCe * sin_Cn,
                                   Z * *(TC->gtu+1) * cos_Cn
                                   - dCn * *TC->gtu * sin_Cn) + TC->L0;
                    /* geodetic latitude  */
                    Cn += clenshaw('s', TC->tcgg, 5, 2.*Cn, 0.0,
                                   &dCn, &dCe);
                  }
                  else res = TRF_AREA_;
                  break;
  
                case -1: /* GEO -> PRJ */
                  Ce -= TC->L0;
                  /* geo latitude to authalic latitude  */
                  Cn += clenshaw('s', TC->tcgg+5, 5, 2.*Cn, 0.0,
                                 &dCn, &dCe);
                  cos_Cn  = cos(Cn);
                  sin_Cn  = sin(Cn);
                  /* gtu[0] : sin(autalic B0) */
                  /* gtu[1] : cos(autalic B0) */
                  Z       = TC->Zb * sqrt(2 /
                            (1+TC->gtu[0]*sin_Cn
                              +TC->gtu[1]*cos_Cn*cos(Ce)));
                  Cn      = Z / TC->cP * (*(TC->gtu+1) * sin_Cn 
                          - *TC->gtu * cos_Cn * cos(Ce)) + TC->N0;
                  Ce      = Z * TC->cP * cos_Cn *sin(Ce) + TC->E0;
                  break;
  
                case  0: /* transfer results */
                  *Nout = Cn;
                  *Eout = Ce;
                  if (res <= TRF_AREA_) i += s;
                  break;
                }
              break;  /* Lambert Azimuthal Equal Area. */
  
            }

            break;
  
          } /* extern sw(cstm) */

          if(abs(direct) > 10) return(0);


/* ptg_d   ver 2010.01           # page 12   12 Jan  2010 10 41 */


          /* tolerance test */
          if (res >= TRF_INACC_) {
            dCn = Cn - N;
            dCe = Ce - E;
            if (direct < 0 || auth) {
              if   (dCe >  M_PI) dCe -= (2.0*M_PI);
              else
                if (dCe < -M_PI) dCe += (2.0*M_PI);
              dCn *=  TC->Qn;
              dCe *= (TC->Qn * cos_Cn);
            }
  
            if ( (fabs(dCn) < TC->tol)
                && (fabs(dCe) < TC->tol)) return (0);
            else
            if ((fabs(dCn) < 1.0e6 * TC->tol) &&  /* 30 m */
                (fabs(dCe) < 1.0e6 * TC->tol)) res = TRF_INACC_;
            else res = TRF_TOLLE_;
          }
          if (res <= TRF_AREA_) {
            *Nout = 0.0;
            *Eout = 0.0;
          }


/* ptg_d   ver 2010.01           # page 13   12 Jan  2010 10 41 */


          /* status report block */
          /*_____________________*/
  
          switch(cstm) {
          case 3: /* UTM */
            ptx = "ptg_d(utm)";
            break;
          case 4: /* MRC */
            ptx = "ptg_d(mrc)";
            break;
          case 5: /* LMB */
          case 9: /* GS LM*/
            ptx = "ptg_d(lmb)";
            break;
          case 6: /* STG */
            ptx = "ptg_d(stg)";
            break;
          case 7: /* EQU */
            ptx = "ptg_d(equ)";
            break;
          default: /* alarm exit */
            ptx = "ptg_d(undef)";
            res = TRF_ILLEG_;
            break;
          } /* end switch(cstm) */

        } else {
          ptx = "ptg_d -direct";
          res = TRF_ILLEG_;
        }
      } else {
        ptx = "ptg_d -Dinit";
        res = TRF_ILLEG_;
      }
    } else {
      ptx = "ptg_d -TC_init";
      res = TRF_PROGR_;
    }
  } else {
    ptx = "ptg_d -crd_lab";
    res = TRF_ILLEG_;
  }

  return((tr_error==NULL || res==0) ? res :
          t_status(tr_error, usertxt, ptx, res,
          (direct > 0) ? "m " : "sx", "", N, E, dCn, dCe);

}
