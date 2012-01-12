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
 


/* fo_g_tr   ver 2011.02         # page 1   7 Oct 2011 12 58 */


/* Copyright (c) 2001 SPACE Danish Technical University (DTU)  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of DTU SPACE    */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */

#include <stdio.h>
#include <math.h>
#include "geo_lab.h"


int                      fo_g_tr(
/*______________________________*/
int                      mode,
int                      direct,
double                   N_in,
double                   E_in,
double                  *N_out,
double                  *E_out,
char                    *usertxt,
FILE                    *tr_error
)

{
/* Definition of the permissible area for the transformation */
#define COORDLIM     (2.0e5)        /* +/- 200 km */

#include         "t_status.h"

  double           N, E, n, e, u, v, tol = 0.080; /*m*/
  double          *cb = NULL, *cw = NULL;
  double          *tcy, *tcx;
  int              res = 0, i, g, r, c, sz;



/* fo_g_tr   ver 2011.02         # page 2   7 Oct 2011 12 58 */


  /* Y&X-generelt pol. resultater
  utm29_etrs89 -> fu50
  grad:  8 pass: 5
  my  =  2.74 cm    my_loss =    +5  y_enp =  +3.9
  mx  =  2.76 cm    mx_loss =    +5  x_enp =  +4.0
  OBS =    474      red     =     2  m_lim_gen:  0.276
  */

  static double U29_FU50[] = {
    /*deg*/  8.0,
    /*Poly NORTH :: e-degree =  0  : n-degree =  8 */
    /*  0*/   6.8795534465e+06,   1.0000314157e+00,   5.4509911754e-11,
    /*  3*/   1.3818465993e-14,  -1.7832970792e-20,  -1.5613792896e-23,
    /*  6*/  -6.5246894560e-29,   5.2980867705e-33,   5.2365865341e-38,
    /*Poly NORTH :: e-degree =  1  : n-degree =  7 */
    /*  9*/  -1.2025438123e-05,  -1.1289277739e-11,   2.0554741307e-14,
    /* 12*/   1.1281624492e-19,  -2.4019082582e-23,  -7.7217194316e-29,
    /* 15*/   6.1569347678e-33,   6.2555377335e-39,
    /*Poly NORTH :: e-degree =  2  : n-degree =  6 */
    /* 17*/   9.7114165683e-11,   1.5778123395e-14,   1.0880788680e-18,
    /* 20*/  -1.1411186027e-22,   7.1753862075e-29,   4.8943761688e-32,
    /* 23*/  -3.7141012376e-38,
    /*Poly NORTH :: e-degree =  3  : n-degree =  5 */
    /* 24*/  -1.1143340177e-14,   3.7147439468e-18,  -1.9534566836e-22,
    /* 27*/   9.1355095787e-28,   9.5335569939e-32,  -1.3024381534e-36,
    /*Poly NORTH :: e-degree =  4  : n-degree =  4 */
    /* 30*/  -2.4087328125e-18,   1.1427081830e-22,  -2.3773716352e-27,
    /* 33*/   2.0781971670e-31,  -4.3719703010e-36,
    /*Poly NORTH :: e-degree =  5  : n-degree =  3 */
    /* 35*/   4.3709718118e-24,  -1.2183762445e-26,   7.4846691033e-31,
    /* 38*/  -1.0061215075e-35,
    /*Poly NORTH :: e-degree =  6  : n-degree =  2 */
    /* 39*/   6.9927880720e-27,  -6.3349963020e-31,   1.1804552917e-35,
    /*Poly NORTH :: e-degree =  7  : n-degree =  1 */
    /* 42*/   2.0553531447e-31,  -8.7363827266e-36,
    /*Poly NORTH :: e-degree =  8  : n-degree =  0 */
    /* 44*/   1.5593488820e-36,
    /*tcy*/    6879316.895051,


/* fo_g_tr   ver 2011.02         # page 3   7 Oct 2011 12 58 */


    /*Poly EAST  :: n-degree =  0  : e-degree =  8 */
    /*  0*/   6.1235663916e+05,   1.0000380827e+00,  -1.2219939938e-10,
    /*  3*/  -6.0732788838e-14,   1.4036070189e-18,   1.4410034237e-22,
    /*  6*/  -6.8907237889e-28,  -6.5225647734e-32,  -5.1108244980e-37,
    /*Poly EAST  :: n-degree =  1  : e-degree =  7 */
    /*  9*/   1.8380596292e-05,  -1.7451907445e-10,  -3.0031248311e-14,
    /* 12*/   5.7207376831e-18,  -4.1410441777e-23,  -1.7327467019e-26,
    /* 15*/   5.3583054216e-32,   2.9510606633e-36,
    /*Poly EAST  :: n-degree =  2  : e-degree =  6 */
    /* 17*/  -1.4813187473e-10,   8.7321458598e-15,   1.5946235334e-18,
    /* 20*/  -1.0370155607e-22,  -5.8372376059e-27,   8.3724340659e-31,
    /* 23*/  -1.4709729919e-37,
    /*Poly EAST  :: n-degree =  3  : e-degree =  5 */
    /* 24*/  -1.9212455919e-14,   3.0852927896e-19,   6.2551928881e-23,
    /* 27*/  -4.9247122422e-27,   2.0839488794e-31,  -1.4789096428e-35,
    /*Poly EAST  :: n-degree =  4  : e-degree =  4 */
    /* 30*/   1.4918773739e-19,  -1.6923577599e-23,  -1.0220717408e-27,
    /* 33*/   7.7783302062e-32,  -1.4487102620e-36,
    /*Poly EAST  :: n-degree =  5  : e-degree =  3 */
    /* 35*/   1.2539137634e-23,  -2.7557474007e-28,  -3.9431216829e-32,
    /* 38*/   2.5014830659e-36,
    /*Poly EAST  :: n-degree =  6  : e-degree =  2 */
    /* 39*/   1.1238790782e-29,   5.3219644653e-33,  -1.1242559163e-37,
    /*Poly EAST  :: n-degree =  7  : e-degree =  1 */
    /* 42*/  -2.6932326271e-33,   7.9610385961e-38,
    /*Poly EAST  :: n-degree =  8  : e-degree =  0 */
    /* 44*/  -1.9133031632e-38,
    /*tcx*/     612248.029188
  };
  

/* fo_g_tr   ver 2011.02         # page 4   7 Oct 2011 12 58 */


  /* Y&X-generelt pol. resultater
  fu50 -> utm29_etrs89
  grad:  8 pass: 5
  my  =  2.74 cm    my_loss =    +5  y_enp =  +3.9
  mx  =  2.76 cm    mx_loss =    +5  x_enp =  +4.0
  OBS =    474          red =     2  m_lim_gen:  0.276
  */

  static double FU50_U29[] = {
    /*deg*/  8.0,
    /*Poly NORTH :: e-degree =  0  : n-degree =  8 */
    /*  0*/   6.8793169594e+06,   9.9996858523e-01,  -5.4512379517e-11,
    /*  3*/  -1.3817290361e-14,   1.7850812693e-20,   1.5611707958e-23,
    /*  6*/   6.5226185901e-29,  -5.2971409274e-33,  -5.2353977135e-38,
    /*Poly NORTH :: e-degree =  1  : n-degree =  7 */
    /*  9*/   1.2024539721e-05,   1.1286459847e-11,  -2.0551406736e-14,
    /* 12*/  -1.1273629714e-19,   2.4010074964e-23,   7.7135843899e-29,
    /* 15*/  -6.1527914631e-33,  -6.2177329137e-39,
    /*Poly NORTH :: e-degree =  2  : n-degree =  6 */
    /* 17*/  -9.7124314769e-11,  -1.5773355271e-14,  -1.0878652584e-18,
    /* 20*/   1.1408682432e-22,  -7.1855818519e-29,  -4.8930677908e-32,
    /* 23*/   3.7149290285e-38,
    /*Poly NORTH :: e-degree =  3  : n-degree =  5 */
    /* 24*/   1.1144802993e-14,  -3.7147750821e-18,   1.9534163264e-22,
    /* 27*/  -9.1335556688e-28,  -9.5319387763e-32,   1.3018733108e-36,
    /*Poly NORTH :: e-degree =  4  : n-degree =  4 */
    /* 30*/   2.4084355857e-18,  -1.1427517150e-22,   2.3775449437e-27,
    /* 33*/  -2.0774913141e-31,   4.3706402084e-36,
    /*Poly NORTH :: e-degree =  5  : n-degree =  3 */
    /* 35*/  -4.3849598647e-24,   1.2183566599e-26,  -7.4844806760e-31,
    /* 38*/   1.0061430138e-35,
    /*Poly NORTH :: e-degree =  6  : n-degree =  2 */
    /* 39*/  -6.9918599932e-27,   6.3342957255e-31,  -1.1804174038e-35,
    /*Poly NORTH :: e-degree =  7  : n-degree =  1 */
    /* 42*/  -2.0548903317e-31,   8.7346572817e-36,
    /*Poly NORTH :: e-degree =  8  : n-degree =  0 */
    /* 44*/  -1.5588768683e-36,
    /*tcy*/    6879553.510878,


/* fo_g_tr   ver 2011.02         # page 5   7 Oct 2011 12 58 */


    /*Poly EAST  :: n-degree =  0  : e-degree =  8 */
    /*  0*/   6.1224806116e+05,   9.9996191852e-01,   1.2223463034e-10,
    /*  3*/   6.0723153559e-14,  -1.4038360797e-18,  -1.4407464955e-22,
    /*  6*/   6.8977884426e-28,   6.5229688978e-32,   5.1108040455e-37,
    /*Poly EAST  :: n-degree =  1  : e-degree =  7 */
    /*  9*/  -1.8379302367e-05,   1.7450089856e-10,   3.0018255847e-14,
    /* 12*/  -5.7192913887e-18,   4.1450842930e-23,   1.7322274826e-26,
    /* 15*/  -5.3670884887e-32,  -2.9512218135e-36,
    /*Poly EAST  :: n-degree =  2  : e-degree =  6 */
    /* 17*/   1.4811920861e-10,  -8.7325077534e-15,  -1.5938418718e-18,
    /* 20*/   1.0365645122e-22,   5.8342751897e-27,  -8.3692525075e-31,
    /* 23*/   1.4957099263e-37,
    /*Poly EAST  :: n-degree =  3  : e-degree =  5 */
    /* 24*/   1.9210155120e-14,  -3.0839057408e-19,  -6.2544542591e-23,
    /* 27*/   4.9233460654e-27,  -2.0829642979e-31,   1.4783154043e-35,
    /*Poly EAST  :: n-degree =  4  : e-degree =  4 */
    /* 30*/  -1.4916426102e-19,   1.6923175959e-23,   1.0214146223e-27,
    /* 33*/  -7.7745400905e-32,   1.4472902522e-36,
    /*Poly EAST  :: n-degree =  5  : e-degree =  3 */
    /* 35*/  -1.2537131736e-23,   2.7543906938e-28,   3.9426159387e-32,
    /* 38*/  -2.5007864990e-36,
    /*Poly EAST  :: n-degree =  6  : e-degree =  2 */
    /* 39*/  -1.1239730441e-29,  -5.3222840743e-33,   1.1255603616e-37,
    /*Poly EAST  :: n-degree =  7  : e-degree =  1 */
    /* 42*/   2.6927294767e-33,  -7.9586466952e-38,
    /*Poly EAST  :: n-degree =  8  : e-degree =  0 */
    /* 44*/   1.9130042444e-38,
    /*tcx*/     612356.671139
  };


/* fo_g_tr   ver 2011.02         # page 6   7 Oct 2011 12 58 */


  /* Y&X-generelt pol. resultater
  utm29_etrs89 -> fk54
  grad:  8 pass: 5
  my  =  3.32 cm    my_loss =    +5  y_enp =  +3.5
  mx  =  3.25 cm    mx_loss =    +5  x_enp =  +3.6
  OBS =    471          red =     2  m_lim_gen:  0.332
  */

  static double U29_FK54[] = {
    /*deg*/  8.0,
    /*Poly NORTH :: e-degree =  0  : n-degree =  8 */
    /*  0*/   5.0509812320e+05,   1.0002812491e+00,   2.0680829644e-10,
    /*  3*/   1.5745776924e-14,  -1.9314549547e-19,  -1.6205630689e-23,
    /*  6*/   6.6060816951e-30,   6.0160022580e-33,   5.0217251795e-38,
    /*Poly NORTH :: e-degree =  1  : n-degree =  7 */
    /*  9*/  -2.1810087792e-05,  -2.6291206865e-09,   6.1552633504e-15,
    /* 12*/  -4.8942905614e-19,  -1.2040297728e-23,   5.1144054735e-28,
    /* 15*/   5.1327252489e-33,  -1.1322165709e-37,
    /*Poly NORTH :: e-degree =  2  : n-degree =  6 */
    /* 17*/   4.0197583606e-10,   1.2279845506e-14,  -1.7284292676e-18,
    /* 20*/  -7.1583553372e-23,   2.5348901675e-27,   2.4803880803e-32,
    /* 23*/  -8.6410922861e-37,
    /*Poly NORTH :: e-degree =  3  : n-degree =  5 */
    /* 24*/  -8.3072780722e-15,   6.5304939443e-19,   4.6349378369e-24,
    /* 27*/   2.7732391446e-27,  -2.4972579370e-32,  -2.0028234884e-36,
    /*Poly NORTH :: e-degree =  4  : n-degree =  4 */
    /* 30*/  -2.9619341716e-18,   9.8729597137e-23,  -2.6430721437e-28,
    /* 33*/   1.8019575376e-33,   1.2793300222e-36,
    /*Poly NORTH :: e-degree =  5  : n-degree =  3 */
    /* 35*/  -3.9189970881e-24,  -5.1642809658e-27,   1.9159958682e-31,
    /* 38*/  -1.9881456405e-36,
    /*Poly NORTH :: e-degree =  6  : n-degree =  2 */
    /* 39*/   6.8015243817e-27,  -5.0723075838e-31,   9.1120673851e-36,
    /*Poly NORTH :: e-degree =  7  : n-degree =  1 */
    /* 42*/   1.8175277560e-31,  -6.9651264551e-36,
    /*Poly NORTH :: e-degree =  8  : n-degree =  0 */
    /* 44*/   1.2833237740e-36,
    /*tcy*/    6879356.448104,


/* fo_g_tr   ver 2011.02         # page 7   7 Oct 2011 12 58 */


    /*Poly EAST  :: n-degree =  0  : e-degree =  8 */
    /*  0*/   3.8743208351e+05,  -1.0002865950e+00,   1.0735037982e-09,
    /*  3*/   9.0880943724e-14,   2.4656348700e-18,  -1.7192287957e-22,
    /*  6*/  -7.6976119024e-27,  -1.1440536903e-31,  -5.8978675593e-37,
    /*Poly EAST  :: n-degree =  1  : e-degree =  7 */
    /*  9*/  -2.9670790694e-05,  -3.2653233033e-10,   9.4337789362e-14,
    /* 12*/  -4.9803268371e-18,  -3.0276158206e-22,   1.8668829246e-26,
    /* 15*/   6.6877874521e-31,   5.9622614782e-36,
    /*Poly EAST  :: n-degree =  2  : e-degree =  6 */
    /* 17*/  -1.2583968275e-09,  -2.6271031796e-14,  -8.6906594141e-19,
    /* 20*/   5.7368731156e-23,   5.7899438961e-27,  -7.7084848768e-31,
    /* 23*/  -1.2555835680e-35,
    /*Poly EAST  :: n-degree =  3  : e-degree =  5 */
    /* 24*/   1.4681531011e-14,   4.2420833152e-19,  -1.6935436501e-22,
    /* 27*/   5.5060799873e-27,   1.1327043712e-31,   5.3795441483e-36,
    /*Poly EAST  :: n-degree =  4  : e-degree =  4 */
    /* 30*/  -7.2406572181e-20,   3.1826783983e-23,  -4.0460515186e-28,
    /* 33*/  -3.8895325727e-32,   3.7296708511e-37,
    /*Poly EAST  :: n-degree =  5  : e-degree =  3 */
    /* 35*/  -7.6868013409e-24,  -9.5490571720e-29,   7.3313682810e-32,
    /* 38*/  -2.4922986016e-36,
    /*Poly EAST  :: n-degree =  6  : e-degree =  2 */
    /* 39*/  -2.4924604561e-29,  -1.4658438719e-32,   7.3680825466e-37,
    /*Poly EAST  :: n-degree =  7  : e-degree =  1 */
    /* 42*/   1.1600978624e-33,  -1.0472131795e-37,
    /*Poly EAST  :: n-degree =  8  : e-degree =  0 */
    /* 44*/   9.2998686820e-39,
    /*tcx*/     612228.553863
  };


/* fo_g_tr   ver 2011.02         # page 8   7 Oct 2011 12 58 */


  /* Y&X-generelt pol. resultater
  fk54 -> utm29_etrs89
  grad:  8 pass: 5
  my  =  3.32 cm    my_loss =    +5  y_enp =  +3.5
  mx  =  3.26 cm    mx_loss =    +5  x_enp =  +3.6
  OBS =    471          red =     2  m_lim_gen:  0.332
  */

  static double FK54_U29[] = {
    /*deg*/  8.0,
    /*Poly NORTH :: e-degree =  0  : n-degree =  8 */
    /*  0*/   6.8793565376e+06,   9.9971883292e-01,  -2.0680464694e-10,
    /*  3*/  -1.5735660337e-14,   1.9307991969e-19,   1.6183218612e-23,
    /*  6*/  -6.6952366714e-30,  -6.0044471033e-33,  -5.0088445761e-38,
    /*Poly NORTH :: e-degree =  1  : n-degree =  7 */
    /*  9*/  -2.1797719965e-05,  -2.6267726066e-09,   6.1560386143e-15,
    /* 12*/  -4.8965298525e-19,  -1.2016322007e-23,   5.1145707021e-28,
    /* 15*/   5.1165986277e-33,  -1.1323659045e-37,
    /*Poly NORTH :: e-degree =  2  : n-degree =  6 */
    /* 17*/  -4.0148021908e-10,  -1.2277599484e-14,   1.7282291977e-18,
    /* 20*/   7.1465959119e-23,  -2.5325828611e-27,  -2.4748151202e-32,
    /* 23*/   8.6259865700e-37,
    /*Poly NORTH :: e-degree =  3  : n-degree =  5 */
    /* 24*/  -8.3101142716e-15,   6.5257720924e-19,   4.6295960396e-24,
    /* 27*/   2.7665356835e-27,  -2.4914818356e-32,  -1.9973290134e-36,
    /*Poly NORTH :: e-degree =  4  : n-degree =  4 */
    /* 30*/   2.9571243209e-18,  -9.8512969312e-23,   2.5936916225e-28,
    /* 33*/  -1.7815882770e-33,  -1.2720085106e-36,
    /*Poly NORTH :: e-degree =  5  : n-degree =  3 */
    /* 35*/  -3.8335586340e-24,  -5.1585589932e-27,   1.9118359030e-31,
    /* 38*/  -1.9793698276e-36,
    /*Poly NORTH :: e-degree =  6  : n-degree =  2 */
    /* 39*/  -6.7885992046e-27,   5.0614484771e-31,  -9.0902211884e-36,
    /*Poly NORTH :: e-degree =  7  : n-degree =  1 */
    /* 42*/   1.8128149152e-31,  -6.9456190222e-36,
    /*Poly NORTH :: e-degree =  8  : n-degree =  0 */
    /* 44*/  -1.2792612395e-36,
    /*tcy*/     505098.212696,


/* fo_g_tr   ver 2011.02         # page 9   7 Oct 2011 12 58 */


    /*Poly EAST  :: n-degree =  0  : e-degree =  8 */
    /*  0*/   6.1222953144e+05,  -9.9971348819e-01,   1.0731178218e-09,
    /*  3*/  -9.0797392717e-14,   2.4597106553e-18,   1.7175104412e-22,
    /*  6*/  -7.6826492882e-27,   1.1408056733e-31,  -5.8750006951e-37,
    /*Poly EAST  :: n-degree =  1  : e-degree =  7 */
    /*  9*/  -2.9653185197e-05,   3.2637690246e-10,   9.4170162710e-14,
    /* 12*/   4.9727301372e-18,  -3.0197073804e-22,  -1.8635205175e-26,
    /* 15*/   6.6716279802e-31,  -5.9438853553e-36,
    /*Poly EAST  :: n-degree =  2  : e-degree =  6 */
    /* 17*/  -1.2573402367e-09,   2.6252277314e-14,  -8.6720144606e-19,
    /* 20*/  -5.7171003138e-23,   5.7738931856e-27,   7.6874969297e-31,
    /* 23*/  -1.2521659779e-35,
    /*Poly EAST  :: n-degree =  3  : e-degree =  5 */
    /* 24*/   1.4661123753e-14,  -4.2394791959e-19,  -1.6898866566e-22,
    /* 27*/  -5.4938978827e-27,   1.1280312641e-31,  -5.3568939660e-36,
    /*Poly EAST  :: n-degree =  4  : e-degree =  4 */
    /* 30*/  -7.2228334946e-20,  -3.1768440809e-23,  -4.0469161782e-28,
    /* 33*/   3.8758285301e-32,   3.7707733906e-37,
    /*Poly EAST  :: n-degree =  5  : e-degree =  3 */
    /* 35*/  -7.6689457122e-24,   9.5713323117e-29,   7.3114131468e-32,
    /* 38*/   2.4843402845e-36,
    /*Poly EAST  :: n-degree =  6  : e-degree =  2 */
    /* 39*/  -2.4891256397e-29,   1.4621981954e-32,   7.3482132745e-37,
    /*Poly EAST  :: n-degree =  7  : e-degree =  1 */
    /* 42*/   1.1558349797e-33,   1.0432028724e-37,
    /*Poly EAST  :: n-degree =  8  : e-degree =  0 */
    /* 44*/   9.2675083872e-39,
    /*tcx*/     387431.105648
  };


/* fo_g_tr   ver 2011.02         # page 10   7 Oct 2011 12 58 */


  /* Y&X-generelt pol. resultater
  utm29_etrs89 -> fk89
  grad:  7 pass: 6
  my  =  1.65 cm    my_loss =    +4  y_enp =  +5.2
  mx  =  1.51 cm    mx_loss =    +5  x_enp =  +4.6
  OBS =    617          red =     2  m_lim_gen:  0.165
  */

  static double U29_FK89[] = {
    /*deg*/  7.0,
    /*Poly NORTH :: e-degree =  0  : n-degree =  7 */
    /*  0*/   7.0740274146e+05,   1.0002511852e+00,  -8.6118085163e-11,
    /*  3*/   5.0824946745e-15,   2.2146435015e-19,  -3.8507098163e-25,
    /*  6*/  -9.0626041730e-29,  -7.1489193237e-34,
    /*Poly NORTH :: e-degree =  1  : n-degree =  6 */
    /*  8*/  -1.7930659748e-05,  -3.0250229099e-09,   4.7108116794e-15,
    /* 11*/   3.0809033163e-19,  -4.0228753090e-24,  -9.8029596413e-29,
    /* 14*/   3.2603973442e-34,
    /*Poly NORTH :: e-degree =  2  : n-degree =  5 */
    /* 15*/  -3.0193077186e-10,  -8.6431484036e-16,   7.0550694050e-19,
    /* 18*/  -2.5241190640e-23,  -3.0372248127e-28,   6.3268476785e-33,
    /*Poly NORTH :: e-degree =  3  : n-degree =  4 */
    /* 21*/  -1.2759826132e-14,   1.4650170329e-18,  -4.5593873757e-23,
    /* 24*/  -3.9189433480e-28,   2.6677938761e-32,
    /*Poly NORTH :: e-degree =  4  : n-degree =  3 */
    /* 26*/   2.7192608394e-19,  -2.7514172973e-23,  -5.1475417685e-28,
    /* 29*/   3.2694182342e-32,
    /*Poly NORTH :: e-degree =  5  : n-degree =  2 */
    /* 30*/   3.4469572647e-23,  -2.3436402559e-27,   2.8778894388e-32,
    /*Poly NORTH :: e-degree =  6  : n-degree =  1 */
    /* 33*/   8.8346449735e-28,  -2.1964578359e-32,
    /*Poly NORTH :: e-degree =  7  : n-degree =  0 */
    /* 35*/   7.8750432003e-33,
    /*tcy*/    6881660.520318,

    /*Poly EAST  :: n-degree =  0  : e-degree =  7 */
    /*  0*/   5.8596747472e+05,  -1.0002529458e+00,   1.1228793160e-09,
    /*  3*/   1.3648462178e-14,   7.4753517499e-19,  -2.7088969233e-23,
    /*  6*/  -1.8264643804e-27,  -2.3216845486e-32,
    /*Poly EAST  :: n-degree =  1  : e-degree =  6 */
    /*  8*/  -2.1491405148e-05,   1.1357878209e-11,   1.1579809096e-14,
    /* 11*/  -7.6476019936e-19,   1.8080367850e-23,   2.8117270945e-27,
    /* 14*/   4.5293766762e-32,
    /*Poly EAST  :: n-degree =  2  : e-degree =  5 */
    /* 15*/  -1.4798344969e-09,  -1.7574552702e-14,   2.6934015888e-19,
    /* 18*/  -5.5817644252e-26,  -1.1139612166e-27,  -1.3949143046e-32,
    /*Poly EAST  :: n-degree =  3  : e-degree =  4 */
    /* 21*/   5.7668707920e-15,  -2.8656913558e-19,  -7.6308000698e-24,
    /* 24*/   4.2290239256e-31,  -8.7007992806e-33,
    /*Poly EAST  :: n-degree =  4  : e-degree =  3 */
    /* 26*/   1.1631138459e-19,   5.0287691474e-24,  -1.7305464179e-28,
    /* 29*/   3.6969507023e-33,
    /*Poly EAST  :: n-degree =  5  : e-degree =  2 */
    /* 30*/  -2.2991508210e-24,   2.0890947255e-28,  -1.0284367310e-33,
    /*Poly EAST  :: n-degree =  6  : e-degree =  1 */
    /* 33*/  -7.1093563967e-29,   1.2743667045e-33,
    /*Poly EAST  :: n-degree =  7  : e-degree =  0 */
    /* 35*/  -4.7115306340e-34,
    /*tcx*/     613692.685906
  };


/* fo_g_tr   ver 2011.02         # page 11   7 Oct 2011 12 58 */


  /* Y&X-generelt pol. resultater
  utm29_etrs89 -> fk89
  grad:  7 pass: 6
  my  =  1.65 cm    my_loss =    +4  y_enp =  +5.2
  mx  =  1.51 cm    mx_loss =    +5  x_enp =  +4.6
  OBS =    617          red =     2  m_lim_gen:  0.165
  */

  static double FK89_U29[] = {
    /*deg*/  7.0,
    /*Poly NORTH :: e-degree =  0  : n-degree =  7 */
    /*  0*/   6.8816604526e+06,   9.9974888036e-01,   8.5975920339e-11,
    /*  3*/  -5.0828296640e-15,  -2.2119425558e-19,   3.8550652099e-25,
    /*  6*/   9.0477699676e-29,   7.1342478224e-34,
    /*Poly NORTH :: e-degree =  1  : n-degree =  6 */
    /*  8*/  -1.7922261843e-05,  -3.0227118426e-09,   4.7115391037e-15,
    /* 11*/   3.0767762000e-19,  -4.0216385989e-24,  -9.7770988064e-29,
    /* 14*/   3.2755807920e-34,
    /*Poly NORTH :: e-degree =  2  : n-degree =  5 */
    /* 15*/   3.0184346042e-10,   8.6384596857e-16,  -7.0450065166e-19,
    /* 18*/   2.5219536318e-23,   3.0309195782e-28,  -6.3205485298e-33,
    /*Poly NORTH :: e-degree =  3  : n-degree =  4 */
    /* 21*/  -1.2748401446e-14,   1.4629946135e-18,  -4.5508365932e-23,
    /* 24*/  -3.9148046383e-28,   2.6617485828e-32,
    /*Poly NORTH :: e-degree =  4  : n-degree =  3 */
    /* 26*/  -2.7150845502e-19,   2.7477896056e-23,   5.1354807280e-28,
    /* 29*/  -3.2634191408e-32,
    /*Poly NORTH :: e-degree =  5  : n-degree =  2 */
    /* 30*/   3.4405782237e-23,  -2.3387908196e-27,   2.8707938484e-32,
    /*Poly NORTH :: e-degree =  6  : n-degree =  1 */
    /* 33*/  -8.8151944386e-28,   2.1907092598e-32,
    /*Poly NORTH :: e-degree =  7  : n-degree =  0 */
    /* 35*/   7.8552991362e-33,
    /*tcy*/     707402.673739,

    /*Poly EAST  :: n-degree =  0  : e-degree =  7 */
    /*  0*/   6.1369351702e+05,  -9.9974711954e-01,   1.1220495589e-09,
    /*  3*/  -1.3635837000e-14,   7.4631182584e-19,   2.7041911955e-23,
    /*  6*/  -1.8224761896e-27,   2.3157576119e-32,
    /*Poly EAST  :: n-degree =  1  : e-degree =  6 */
    /*  8*/  -2.1480465460e-05,  -1.1204510924e-11,   1.1564521374e-14,
    /* 11*/   7.6327098918e-19,   1.8053777839e-23,  -2.8052019807e-27,
    /* 14*/   4.5176378826e-32,
    /*Poly EAST  :: n-degree =  2  : e-degree =  5 */
    /* 15*/  -1.4787192733e-09,   1.7567144449e-14,   2.6884358205e-19,
    /* 18*/   6.3015947181e-26,  -1.1112934184e-27,   1.3886539701e-32,
    /*Poly EAST  :: n-degree =  3  : e-degree =  4 */
    /* 21*/   5.7614586973e-15,   2.8622945128e-19,  -7.6174874069e-24,
    /* 24*/  -1.4635605061e-31,  -8.6947336011e-33,
    /*Poly EAST  :: n-degree =  4  : e-degree =  3 */
    /* 26*/   1.1617945235e-19,  -5.0205645518e-24,  -1.7274955100e-28,
    /* 29*/  -3.6890494365e-33,
    /*Poly EAST  :: n-degree =  5  : e-degree =  2 */
    /* 30*/  -2.2954995659e-24,  -2.0854503481e-28,  -1.0254292783e-33,
    /*Poly EAST  :: n-degree =  6  : e-degree =  1 */
    /* 33*/  -7.0962525415e-29,  -1.2718771355e-33,
    /*Poly EAST  :: n-degree =  7  : e-degree =  0 */
    /* 35*/  -4.7019196813e-34,
    /*tcx*/     585966.643395
  };



/* fo_g_tr   ver 2011.02         # page 12   7 Oct 2011 12 58 */


  /* Collect start values */
  N = N_in;
  E = E_in;
  switch (mode + ((direct > 0) ? 0 : 1)) {
  case 3: /* utm29_etrs89 -> fu50 (direct) */
    cb = U29_FU50;  cw = FU50_U29;  break;
  case 4: /* fu50 -> utm29_etrs89 (inverse) */
    cb = FU50_U29;  cw = U29_FU50;  break;
  case 5: /* utm29_etrs89 -> fk54 (direct) */
    cb = U29_FK54;  cw = FK54_U29;  break;
  case 6: /* fk54 -> utm29_etrs89 (inverse) */
    cb = FK54_U29;  cw = U29_FK54;  break;
  case 7: /* utm29_etrs89 -> fk89 (direct) */
    cb = U29_FK89;  cw = FK89_U29;  break;
  case 8: /* fk89 -> utm29_etrs89 (inverse) */
    cb = FK89_U29;  cw = U29_FK89;  break;
  default: 
    return(t_status(tr_error, usertxt, "fo_g_tr", TRF_PROGR_, 
           "m ", "", N, E, N - N_in, E - E_in));
  }

  if (direct) {
    /* Transformation loop */
    for (i = -1; i <= 1 && res >= TRF_TOLLE_; ++i)
      if (i) {
        /* Prepare for double Horner */
        g   = (int) *cb;
        sz  = (g + 1)*(g + 2)/2 + 1;
        tcy = cb + sz;
        tcx = tcy  + sz;
        /* Double Horner's scheme */
        /* N = n*Cy*e -> yout, E = e*Cx*n -> xout */

        n   = N - *tcy;
        e   = E - *tcx;

        if (fabs(n) < COORDLIM && fabs(e) < COORDLIM) {
          for (  r = g, N = *--tcy, E = *--tcx; r >  0; r--) {
            for (c = g, u = *--tcy, v = *--tcx; c >= r; c--) {
              u = n*u + *--tcy;
              v = e*v + *--tcx;
            }
            N = e*N + u;
            E = n*E + v;
          }
        }
        else res = TRF_AREA_;
      }
      else { /* collect output coord,switch to reverse checking */
        *N_out = N;
        *E_out = E;
        cb     = cw; /* reverse direction */
      }


/* fo_g_tr   ver 2011.02         # page 13   7 Oct 2011 12 58 */


    if (res >= TRF_TOLLE_) {
      if (fabs(N - N_in) < tol && fabs(E - E_in) < tol) return (0);
      else
      res = TRF_TOLLE_;
    }
  }
  else res = TRF_PROGR_;  /* UNDEFINED DIRECTION */

  return(t_status(tr_error, usertxt, "fo_g_tr", res, 
         "m ", "", N, E, N - N_in, E - E_in));

#undef   COORDLIM
}

