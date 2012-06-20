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
/* Modified, KE JAN 2010                                       */
/* Does not use def_lab anymore - instead use preparsed data stored in extern def_data *DEF_DATA */
/* simlk, june 2012 */

#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <sys/types.h>
//#include    <sys/stat.h>
#include    <ctype.h>
#include    <fcntl.h>
#include    <math.h>
#include    "geo_lab.h"
#include    "parse_def_file.h"


int             set_dtm_1(
/*______________________*/
short               d_no,
char               *d_nm,
short              *p_no,
char               *p_nm,
char               *e_nm,
char               *rgn_nm,
short              *mask,
struct dsh_str     *trp
)

{
extern def_data *DEF_DATA;

#define  MAS  (RHO*1000.0)

  /* DATUM SHIFT TYPES, See geo_lab.h */
  /* DATUM NAMES */
  /* central block */
  /*     LOKAL    0     lokalt  */
#define  W84GS    (1)  /* wgs84   */
#define  E50DT    (2)  /* ed50    */
#define  N9WLD    (3)  /* nwl9d   */
#define  E89RF    (4)  /* euref89 = ITRF92 timetrf -> ERTF89 */
#define  GR96     (5)  /* GR96    = ITRF96 */

  /* igs and itrf datums */
#define  IGSyy    (6)  /* IGSyy   for GPS only */
#define  ITRFyy   (7)  /* ITRFyy  NOT for GPS */
#define  ETRFyy   (8)  /* ETRFyy              */


  /* adjacent datums */
#define  ETOPEX   (9)  /* TOPEX datum */
#define  E87DT   (10)  /* ed87    */
#define  W72GS   (11)  /* wgs72   */
#define  E79DT   (12)  /* ed79    */
#define  F54DT   (13)  /* fd54    */
#define  QORNK   (14)  /* qornoq  */
#define  N83AD   (15)  /* nad83   */
#define  N27AD   (16)  /* nad27   */
#define  N27AC   (17)  /* nad27c  */
#define  P42UL   (18)  /* pulkovo */
#define  ISLND   (20)  /* island  */
#define  DHDNT   (21)  /* dhdn    */
#define  SCOSD   (22)  /* scosd   */

#define  AMMLK   (23)  /* ammlk   */
#define  RT90G   (24)  /* rt90    */
#define  SRT90G  (25)  /* rt90    */
#define  INdia   (30)  /* india   */
#define  M50SK   (31)  /* mærsk_ed50    */
#define  EEU89   (51)  /* eesti_euref89 */
#define  N83GD   (62)  /* nad83g  */
#define  EXTRA   (63)  /* test    */

  /* implicit datums */
#define  MIN_IMPL_CDTM   (128)
#define  SJ34D  (129)  /* s34j    */
#define  SS34D  (130)  /* s34s    */
#define  SB45D  (131)  /* s45b    */
#define  GSDTM  (132)  /* gs      */
#define  GSBDT  (133)  /* gsb     */
#define  OSDTM  (134)  /* os      */
#define  KKDTM  (135)  /* kk      */
#define  SBDTM  (136)  /* sb      */
#define  RT90V  (137)  /* rt90v   */
#define  RT38V  (138)  /* rt38v   */
#define  RT38G  (139)  /* rt38g   */
#define  D90KS  (140)  /* dks     */
#define  E42ES  (141)  /* eesti42 */
#define  FD54G  (142)  /* fg54    */
#define  FD50G  (143)  /* fg50    */
#define  F54LM  (144)  /* fk54    */
#define  F77UT  (145)  /* fu50    topografisk ed50 forløbig */
#define  F89LM  (146)  /* fk89    */
#define  EOLES  (147)  /* eeold42 */
#define  E37ES  (148)  /* eeepv37 */
#define  A90SB  (149)  /* asb     */
#define  NAH67  (150)  /* Qatar Nahwan 1967 */
#define  QNG24  (151)  /* Qatar National Gridsystem 1924 */
#define  QBC01  (152)  /* Qatar Basic Coordinate System 2001 */
                       /* QBC01 COWI DEF */
#define  QND95  (153)  /* Qatar National Datum 1995 */

#define  MAX_IMPL_CDTM   (153)


/* set_dtm     ver 2010.1          # page 3   12 jan 2010 10 19 */


  /* implicit datums Height */
#define  HLOKD  (201)  /* Local Reference Level */
#define  HMSLD  (202)  /* Mean Sea Level of geoid dtm */
#define  GM91D  (203)  /* G.M. 1891 */
#define  GI44D  (204)  /* G.I. 1944 */
#define  KN44D  (205)  /* Kbh.Nul 1944 */
#define  GI50D  (206)  /* G.I. 1950 */
#define  KMSD   (207)  /* KMS  1990 MIDLERTIDIGT REGNENIVEAU */
#define  DVRD   (208)  /* DVR90  */
#define  DNND   (209)  /* DNN: gm91/gi44/h_msl(dnn50)  */
#define  EVRF00D (210) /* European Vertical Reference Frame 2000 */
  /* EESTONIA datums Height */
#define  EE40D  (291)  /* ESTONIA 1940 */
#define  EE48D  (292)  /* ESTONIA 1948 */
#define  EE77D  (293)  /* ESTONIA 1977 */

  /* implicit datums Gravity */
#define  IGSN   (301)  /* International Gravity System */
#define  GABS   (302)  /* Absolut Gravity System */
#define  GPOT   (303)  /* Potsdam Gravity System */

  /* DATUM SHIFT IS PERFORMED from datum to p_datum : +ST7PA = +7 */
  /* TO = scale * ROTZ * ROTY * ROTX * FROM + TRANSLA */
  /*      ( cz sz 0)       (cy 0 -sy)       (1   0  0) */
  /* ROTZ=(-sz cz 0), ROTY=(0  1   0), ROTX=(0  cx sx) */
  /*      (  0  0 1)       (sy 0  cy)       (0 -sx cx) */
  /* small rotations appr. ==                          */
  /*                      ( 1    rz  -ry)              */
  /* ROTZ * ROTY * ROTX = (-ry   1    rx)              */
  /*                      ( ry  -rx   1 )              */

  /* DATUM SHIFT IS PERFORMED from datum to p_datum : -ST7PA = -7 */
  /* TO = ROTX^T * ROTY^T * ROTZ^T * (FROM - TRANSLA) / scale */

  /* INVERSE DATUM SHIFT IS PERFORMED from p_datum to datum : -type */

  /* datum parameters are read from ~geoid/def_dtm.txt : */
  /* datum                                    */
  /* no        datum     p_datum    ellipsoid */
  /* type        tx         ty         tz     */
  /* dm          rx         ry         rz     */
  /* Datumtext                                */

    int                            res = -1, i, mode;
    char                       *w, w_name[24];
    double                      sin_rx, cos_rx, scale;
    double                      sin_ry, cos_ry;
    double                      sin_rz, cos_rz;



    def_datum *dtm=NULL;


    if (DEF_DATA==NULL)
	  return -2;

  /* find datum name from datum number */
 
    if (d_no > 0) {
	    for(i=0; i<DEF_DATA->n_dtm; i++){
		    if ((DEF_DATA->datums[i]).no==d_no){
			    strncpy(d_nm,DEF_DATA->datums[i].mlb,MLBLNG);
			    return d_no;
			    }
		}
    return -1;
	}
  
    /* change to lover case letters */
    (void) strcpy(w_name, d_nm);
    w     = w_name;
    while ((*w = (char) tolower(*w)) != '\0') w++;
	
    /* documentation 'mode' not implemented here anymore */
    if (!isalpha(*d_nm) || strlen(d_nm) < 2)
	    goto report;
    
    /* get datum info from datum name */
    
    
    
    for(mode=0; mode<2; mode++){
	    /* do the actual search */
	    for(i=0; i<DEF_DATA->n_dtm && dtm==NULL; i++){
		    if(!strcmp(DEF_DATA->datums[i].mlb,w_name))
			    dtm=((DEF_DATA->datums)+i);
	    
	}
	/* if not found and w_name looks right have another go with igsyy, itrfyy, etrfyy */
	if (dtm==NULL && (*w_name=='i' || *w_name=='e') && strlen(w_name)>4 && isdigit(w_name[strlen(w_name)-1])){
		w       = w_name + strlen(w_name) - 2; 
		*(w)    = 'y';
		*(w +1) = 'y';
		}
	else
		break;
	}
	
      if (dtm==NULL)
	      return -1;
      
      res = dtm->no;
      /* transfer preparsed data */
      
      *p_no=dtm->p_no;
     
      /* get names of parent datum and actual ellipsoid */
      
       strncpy(p_nm,dtm->p_datum,MLBLNG);
       strncpy(e_nm, dtm->ellipsoid,MLBLNG);
       strncpy(rgn_nm,dtm->rgn,3);
       *mask=dtm->imit;
       trp->tp=dtm->type;
      
      /* translations */
      
      trp->tx = dtm->translation[0];
      trp->ty = dtm->translation[1];
      trp->tz = dtm->translation[2];
      
     /* scale and rotations */
      scale   = dtm->scale;
      cos_rx  = dtm->rotation[0];
      sin_rx  = sin(cos_rx);
      cos_rx  = cos(cos_rx);
      cos_ry  = dtm->rotation[1];
      sin_ry  = sin(cos_ry);
      cos_ry  = cos(cos_ry);
      cos_rz  = dtm->rotation[2];
      sin_rz  = sin(cos_rz);
      cos_rz  = cos(cos_rz);

      /* DATUM SHIFT IS PERFORMED  STD : */
      /* TO = scale * ROTZ * ROTY * ROTX * FROM + TRANSLA */
      /*      ( cz sz 0)       (cy 0 -sy)       (1   0  0) */
      /* ROTZ=(-sz cz 0), ROTY=(0  1   0), ROTX=(0  cx sx) */
      /*      (  0  0 1)       (sy 0  cy)       (0 -sx cx) */




      trp->r11  =  cos_ry*cos_rz;
      trp->r12  =  cos_rx*sin_rz + sin_rx*sin_ry*cos_rz;
      trp->r13  =  sin_rx*sin_rz - cos_rx*sin_ry*cos_rz;

      trp->r21  = -cos_ry*sin_rz;
      trp->r22  =  cos_rx*cos_rz - sin_rx*sin_ry*sin_rz;
      trp->r23  =  sin_rx*cos_rz + cos_rx*sin_ry*sin_rz;

      trp->r31  =  sin_ry;
      trp->r32  = -sin_rx*cos_ry;
      trp->r33  =  cos_rx*cos_ry;

      trp->scale = 1.0 + scale;

            
      
      /* return res or 'no datum found' */
     
      return(res);
   



/* This is not implemented here - should perhaps be up to a separate documentation function??*/
    report:
         strcpy(d_nm,"Not implemented");
         return(-1);
}

