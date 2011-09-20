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
 


/* geo_lab   ver 2010.01          # page 1   5 Jan 2010 11 57 */


/* Copyright (c) 2010 GEK  Danish National Space Center  DTU   */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of Danish       */
/* National Space Center  DTU  Denmark.  This copyright claim  */
/* does not indicate an intention of publishing this code.     */

#if !defined(H_GEO_LAB)
#define      H_GEO_LAB

#include    <stdio.h>
#include    <sys/types.h>
#include    <math.h>

#ifndef    M_PI
#include   "kms_math.h"
#endif

#define   LAB_VERSION      (2010)

/* machine dependent functions */
#ifdef  _WIN32
#define C_DIGIT(c)  ((c)!='Æ' && (c)!='Ø' && (c)!='Å' && ( c)!='æ' && (c)!='ø' && (c)!='å' && isdigit(c))
#define C_LOWER(c)  ((c)!='Æ' && (c)!='Ø' && (c)!='Å' && ((c)=='æ' || (c)=='ø' || (c)=='å' || islower(c))) 
#define C_UPPER(c)  ((c)!='æ' && (c)!='ø' && (c)!='å' && ((c)=='Æ' || (c)=='Ø' || (c)=='Å' || isupper(c)))
#define C_ALPHA(c)  (C_LOWER(c) || C_UPPER(c))
#define C_SPACE(c)  ((c) != -89 && (c)!='Æ' && (c)!='Ø' && (c)!='Å' && (c)!='æ' && (c)!='ø' && (c)!='å' && ((c)>0 && isspace(c)))
#define C_PUNCT(c)  ((c) != -89 && (c)!='Æ' && (c)!='Ø' && (c)!='Å' && (c)!='æ' && (c)!='ø' && (c)!='å' && ispunct(c))

#else
/* machine rekfs1 */
#define C_DIGIT(c)  (isdigit(c))
#define C_LOWER(c)  ((c)=='æ' || (c)=='ø' || (c)=='å' || islower(c))
#define C_UPPER(c)  ((c)=='Æ' || (c)=='Ø' || (c)=='Å' || isupper(c))
#define C_ALPHA(c)  (C_LOWER(c) || C_UPPER(c))
#define C_SPACE(c)  ((c)!='Æ' && (c)!='Ø' && (c)!='Å' && isspace(c))
#define C_PUNCT(c)  ((c) != -89 && (c)!='Æ' && (c)!='Ø' && (c)!='Å' && (c)!='æ' && (c)!='ø' && (c)!='å' && ispunct(c))
#endif

/* Utility macros */
#define  DOUT            (180.0/M_PI)
#define  RHO             (180.0*60.0*60.0/M_PI)
#define  CRO             (2000000.0/      M_PI)
#define  RD_LAT(f, b)    (atan2((1.0-(f))*sin((b)),cos((b))))
#define  GD_LAT(f, b)    (atan2(sin((b)),(1.0-(f))*cos((b))))

/* Definitions of imittance bits */
#define  DKMASK           (1)
#define  BOMASK           (2)
#define  EDMASK           (3)
#define  NGMASK           (4)
#define  FEMASK           (5)
#define  EEMASK           (6)
#define  FHMASK           (7)

/* geo_lab   ver 2010.01          # page 2   5 Jan 2010 11 57 */


#define   MAX_ENT_PLM     (55)

/* ERROR NUMBERS FOR TRANSFORMATIONS */
#define  TRF_INACC_       (-1)
#define  TRF_TOLLE_       (-2)
#define  TRF_AREA_        (-3)
#define  TRF_GEOID_       (-4)
#define  TRF_ILLEG_       (-5)
#define  TRF_PROGR_       (-6)
#define  HTRF_ILLEG_      (-7)
#define  TAB_C_ARE_       (-100)
#define  TAB_C_PRJ_       (-200)
#define  TAB_C_NON_       (-300)
#define  TAB_G_MAX_       (-400)
#define  TAB_N_NAM_       (-500)
#define  TAB_R_MAN_       (-600)
#define  TAB_N_MAN_       (-700)
#define  TAB_N_SYS_       (-800)
#define  PLATE_NON_       (-10000)
#define  PLATE_OUT_       (-20000)
#define  ITRF_NON_        (-30000)
#define  ITRF_SEQ_        (-40000)
#define  ITRF_DTM_        (-50000)
#define  ITRF_MAX_        (-60000)
#define  ITRF_NAM_        (-70000)
#define  ITRF_SYS_        (-80000)
#define  PLATE_NO_VEL_    (-90000)
/* ERROR NUMBERS FOR to be condemned */

#define  UNDEFSYS         (-1)

/* Datum shift types */
#define  ST7PA  (+7) /* 7-parameter transformation, small */
#define  TWODP  (+0) /* 2-dimensional polynomiae          */

/* structure of datum shift constants */
struct  dsh_str {
  int        tp;             /* dsh type       */
  double     tx, ty, tz;     /* translations   */
  double     scale;          /* scale (= 1+dm) */
  double     r11, r12, r13;
  double     r21, r22, r23;  /* rot[3][3]      */
  double     r31, r32, r33;
};

/* structure of gps shift constants */
struct  gps_c_str {
  char       datum[16];      /* datum name            */
  double     sc;             /* scale                 */
  double     tx, ty, tz;     /* translations          */
  double     rx, ry, rz;     /* molodensky scale_rot  */
};

/* structure of itrf plate_motions constants */
struct  plm_c_str {
  char       datum[16];      /* datum name            */
  double     drx, dry, drz;  /* molodensky scale_rot  */
};


/* geo_lab   ver 2010.01          # page 3   5 Jan 2010 11 57 */


/* structure of itrf plate_motions info blocks */
struct  plate_info {
  short     plate_nr;  /* plate_nr in nrr_tables */
  short     poly_tp;   /* defining the types of quick info */
  int       a_ant;     /* number of doubles in an a_polygon */
  int       b_ant;     /* number of doubles in a  b_polygon */
  size_t    poly_a;    /* pos    of doubles in an a_polygon */
  size_t    poly_b;    /* pos    of doubles in a  b_polygon */
  double    BS;        /* Special purpos latitude */
  double    B1;        /* Square min latitude */
  double    B2;        /* Square max latitude */
  double    L1;        /* Square min longitude */
  double    L2;        /* Square max longitude */
};


/* structure of gps entries */
struct  gps_str {
  short             seq[6];  /* sequence of transf: 0/1/2/3/4        */
  int               req_plm_tr; /* request for plate translation     */
  int               req_ipl_tr; /* req. for intra plate translation  */
  double            i_ipl_dt;/* std i_sys ipl_ref date (Julian)      */
  double            o_ipl_dt;/* std o_sys ipl_ref date (from 2000.0) */
  double            plm_dt;  /* std i/o_sys plm_ref date (-do-)      */
  double            plm_yy;  /* plate Julian year shift              */
  double            ipl_yy;  /* intra plate Julian year shift        */
  struct gps_c_str  itrf_tr; /* 1: entry of itrfyy->itrfxx constants */
  struct gps_c_str  igs__tr; /* 2: entry of igsyy->igsxx constants   */
  struct gps_c_str  igat_tr; /* 3: entry of igate_tr constants       */
  struct gps_c_str  ogat_tr; /* 4: entry of ogate_tr constants       */
  struct gps_c_str  itrf_dt; /* 5: entry of itrfyy->itrfxx Dt-constants */
  struct gps_c_str  igs__dt; /* 6: entry of igsyy->igsxx Dt-constants */
};
/* seq[0] : number of first  transf / 0 == IDT and STOP */
/* seq[1] : number of second transf / 0 == IDT and STOP */
/* seq[2] : number of third  transf / 0 == IDT and STOP */
/* seq[3] : number of fourth transf / 0 == IDT and STOP */

#define      ILL_LAB        (0)
#define      CRD_LAB        (1)
#define      HGT_LAB        (2)
#define      GDE_LAB        (3)
#define      WOR_LAB        (4)
#define      POC_LAB        (5)
#define      DTM_LAB        (6)
#define      DHH_LAB        (7)
#define      UPL_LAB        (8)
#define      OBS_LAB        (9)
#define      DEM_LAB       (10)
#define      TAB_LAB       (11)
#define      PLM_LAB       (12)
#define      BAG_LAB       (13)
#define      T3D_LAB       (14)
#define      T2D_LAB       (15)
#define      CND_LAB       (16)
#define      TDD_LAB       (17)
#define      IDT_LAB       (33)
#define      JDF_LAB       (34)
#define      STP_LAB       (63)

#define      MLBLNG        (32)

/* lab_type : Type of label */
/*  0 = empty lab               */
/*  1 = coordinates             */
/*  2 = heights                 */
/*  3 = geoide heights table    */
/*  4 = result from adjustment  */
/*  9 = observations            */
/* 33 = ident st nmb            */
/* 63 = stop                    */

struct  idt_lab {
  short     lab_type;     /* Type of label */
  short     version;      /* Label version */
  char      mlb[MLBLNG];  /* Minilabel of the system */
  short     sepix;        /* index of Separator-char in mlb */
  short     idt_1;        /* not used (cmplt) */
  short     idt_2;        /* not used (cstm) */
  short     idt_3;        /* not used (mode) */
  short     region;       /* Region, see conv_lab.h */
};

/* cmplt : geo_lab definitions completed by : conv_lab */
/*  0 : => to be completed                             */
/*  1 : => complete by conv_lab                        */
/*  2 : => complete by conv_lab                          */


/* geo_lab   ver 2010.01          # page 4   5 Jan 2010 11 57 */


/* cstm : Coordinate and Height System */
/* Coordinate system */
/*  1 = cartesic                               */
/*  2 = geografical                            */
/*  3 = utm                                    */
/*  4 = mercator                               */
/*  5 = lambert                                */
/*  6 = stereographic                          */
/*  7 = Sansom-Flamsteed/Lambert, eq. area     */
/*  8 = System 1934                            */
/*  9 = GS conform con                         */
/* 10 = Københ. Komm., Ostenfeldts system      */
/* 11 = 2-d cartesian (to be completed)        */
/* 12 = Rikstriangulation 1990 5g v            */
/* 16 = geoidal height/deflections of vertical */
/* 17 = ellipsoidal height                     */

/* Height system */
/* 33 = Metric            */
/* 34 = Orthometric       */
/* 35 = Normal            */
/* 36 = Potential         */
/* 37 = Dynamic           */

/* S_crd : Boolean for rect. coordinates counted pos -> S */
/* 0 = pos -> N */
/* 1 = pos -> S */

/* W_crd : Boolean for rect. coordinates counted pos -> W */
/* 0 = pos -> E */
/* 1 = pos -> W */

/* ellipsoid : Ellipsoid : see set_dtm.c */
/* datum     : Datum     : see set_dtm.c */

#if !defined(H_TYP_DEC)
#define      H_TYP_DEC
struct typ_dec {
  unsigned char   gf;
  unsigned char   tf;
  unsigned char   ff;
  unsigned char   df;
};
#endif


/* geo_lab   ver 2010.01          # page 5   5 Jan 2010 11 57 */


struct  coord_lab {
  short     lab_type;    /* Type of label */
  short     version;     /* Label version */
  char      mlb[MLBLNG]; /* Minilabel of the system */
  short     sepix;       /* index of Separator-char in mlb */
  short     cmplt;       /* geo_lab definitions completed by conv_lab */
  short     cstm;        /* Coordinate system */
  short     mode;        /* Coordinate system mode */
  short     region;      /* Regiondefined by label used in st_nmbs */
  short     ncoord;      /* Number of coordinates */
  short     S_crd;       /* crd. sign pos N == 0, pos S == 1 */
  short     W_crd;       /* crd. sign pos E == 0, pos W == 1 */
  short     p_seq;       /* seq. of coord, 0 => (N,E),   1 => (E,N)  */
  short     ellipsoid;   /* Ellipsoid */
  short     datum;       /* Datum */
  short     p_dtm;       /* Parent datum */
  double    a;           /* Semi major axis of the ellipsoid */
  double    f;           /* Flattening of the ellipsoid */
  double    B0;          /* Origin latitude */
  double    N0;          /* Origin northing */
  double    L0;          /* Origin longitude */
  double    E0;          /* Origin easting */
  double    scale;       /* Central scale of the projection */
  double    B1;          /* Contact/int.sect lat (lmb), 90-B0 (stg) */
  double    B2;          /* Latitude of intersection for lmb */
  double    tol;         /* Tolerance of check transformation */
  short     zone;        /* utm zone no. (or optional param) */
  short     imit;        /* imittance mask for trf systems */
  short     p_rgn;       /* Region for proj and datum */
  short     init;        /* init tr. const. 0 => initialize */
  double    Qn;          /* Merid. quad., scaled to the projection */
  double    Zb;          /* Radius vector in polar coord. systems  */
  double    cP;          /* Lambert exponent = cos P0              */
  /*        cP    1/(2sqrt(MN)) = Gaussian curv./2 for s34 corr.   */
  double    tcgg[10];    /* Constants for Gauss <-> Geo lat. NB: hgt*/
  double    utg[5];      /* Constants for transv. merc. -> geo  _lab*/
  double    gtu[5];      /* Constants for geo -> transv. merc.  ----*/
  struct dsh_str dsh_con;/* Structure of datum shift constants */
  int       ch_sum;      /* Checksum for identification of label */
  short     h_ix;        /* index of Height system char in mlb */
  short     hstm;        /* Height system */
  short     hmode;       /* Height system mode */
  short     h_dtm;       /* Height system datum */
  int       ch_hsum;     /* Checksum for identification of H_label */
  int       ch_tsum;     /* Checksum for identification total_label */
  struct typ_dec g_tpd;  /* typdec for geogr. coord */
  struct typ_dec m_tpd;  /* typdec for metric coord */
  struct typ_dec p1tpd;  /* typdec for parameter 1 */
  struct typ_dec p2tpd;  /* typdec for parameter 2 */
  double    date;        /* of coords; -1.0 signals date at coord */
  double    JD;          /* Julian day(date) */
};


/* geo_lab   ver 2010.01          # page 6   5 Jan 2010 11 57 */


/* dummy content */
struct    hgt_lab {
  short     lab_type;    /* Type of label */
  short     version;     /* Label version */
  char      mlb[MLBLNG]; /* Minilabel of the system */
  short     sepix;       /* index of Separator-char in mlb */
  short     cmplt;       /* geo_lab definitions completed by conv_lab */
  short     cstm;        /* Coordinate system */
  short     mode;        /* Coordinate system mode */
  short     region;      /* Regiondefined by label used in st_nmbs */
  short     ncoord;      /* Number of coordinates */
  short     S_crd;       /* crd. sign pos N == 0, pos S == 1 */
  short     W_crd;       /* crd. sign pos E == 0, pos W == 1 */
  short     p_seq;       /* seq. of coord, 0 => (N,E),   1 => (E,N) */
  short     ellipsoid;   /* Ellipsoid */
  short     datum;       /* Datum */
  short     p_dtm;       /* Parent datum */
  double    a;           /* Semi major axis of the ellipsoid */
  double    f;           /* Flattening of the ellipsoid */
  double    B0;          /* Origin latitude */
  double    N0;          /* Origin northing */
  double    L0;          /* Origin longitude */
  double    E0;          /* Origin easting */
  double    scale;       /* Central scale of the projection */
  double    B1;          /* Contact/int.sect lat (lmb), 90-B0 (stg) */
  double    B2;          /* Latitude of intersection for lmb */
  double    tol;         /* Tolerance of check transformation */
  short     zone;        /* utm zone no. (or optional param) */
  short     imit;        /* imittance mask for dkf systems */
  short     p_rgn;       /* Region for proj and datum */
  short     init;        /* init tr. const. 0 => initialize */
  double    Qn;          /* Merid. quad., scaled to the projection */
  double    Zb;          /* Radius vector in polar coord. systems  */
  double    cP;          /* Lambert exponent = cos P0              */
  /*        cP    1/(2sqrt(MN)) = Gaussian curv./2 for s34 corr.   */
  double    tcgg[10];     /* Constants for Gauss <-> Geo lat.   */
  double    utg[5];      /* Constants for transv. merc. -> geo */
  double    gtu[5];      /* Constants for geo -> transv. merc. */
  struct dsh_str dsh_con;/* Structure of datum shift constants */
  int       ch_sum;      /* Checksum for identification of label */
  struct typ_dec g_tpd;  /* typdec for geogr. coord */
  struct typ_dec m_tpd;  /* typdec for metric coord */
  struct typ_dec p1tpd;  /* typdec for parameter 1 */
  struct typ_dec p2tpd;  /* typdec for parameter 2 */
};


/* geo_lab   ver 2010.01          # page 7   5 Jan 2010 11 57 */


struct  plm_lab {
  short     lab_type;    /* type of label */
  short     version;     /* Label version */
  char      mlb[MLBLNG]; /* PLate Motion model name  */
  short     sepix;       /* index of Separator-char in mlb */
  short     cmplt;       /* 2: standard */
  short     cstm;        /* 1 : crt */
  short     mode;        /* 0 */
  short     region;      /* 0 */
  short     f777;        /* manager.gps type */
  short     local;       /* local tab or use tab_dir */
  short     ellipsoid;   /* grs80 */
  short     datum;       /* itrf */
  short     plm_max;     /* no of used entries PLateMov */
                         /* entries of plate motions constants */
  struct plm_c_str  plm__tr[MAX_ENT_PLM];
  int       ch_sum;      /* Checksum for identification of label  */
  FILE     *f_poly;      /* file descriptor for the polygons */
};

struct  gde_lab {
  short     lab_type;    /* type of label */
  short     version;     /* Label version */
  char      mlb[MLBLNG]; /* Geoid name  */
  short     sepix;       /* index of Separator-char in mlb */
  short     cmplt;       /* 0: external, 1: optional, 2: standard */
  short     cstm;        /* 0 : geo, > 0 : utm zone, < 0 : -cstm */
  short     mode;        /* 0 : geo or utm,          > 0 : mode  */
  short     region;      /* datum_no of geoid coord_sys */
  short     f777;        /* bin file type */
  short     local;       /* local tab or use tab_dir */
  short     ellipsoid;   /* Ellipsoid */
  short     datum;       /* Datum */
  short     p_dtm;       /* Parent datum */
  double    B_min;       /* LAT or N min              */
  double    B_max;       /* LAT or N max              */
  double    L_min;       /* LNG or E min              */
  double    deltaL;      /* diff(LNG or E max, L_min) */
  double    dB;          /* LAT or N intv             */
  double    dL;          /* LNG or E intv             */
  short     global;      /* 1: regional geoid,  2: global geoid   */
  int       n_max;       /* last number of LAT/N record_rows      */
  int       e_max;       /* last number of LNG/E cols of geoid_h  */
  int       nbase;       /* base of geoid table                   */
  int       estop;       /* last number of LNG or E columns       */
  size_t    row_size;    /* size of row of LNG or E recs (bytes)  */
  size_t    blk_size;    /* size of record blocks        (bytes)  */
  int       rec_size;    /* record size (fl = 4)         (bytes)  */
                         /* record size (long = -4)      (bytes)  */
  int       rec_p_bl;    /* records pr block                      */
  int       gd_dec;      /* decimals in grid_h                    */
  int       f_comp;      /* compatible with geocol e.al.          */
  char      clb[MLBLNG]; /* coordinate sys name                   */
  struct typ_dec g_tpd;  /* typedec of B and L                    */
  int       ch_sum;      /* Checksum for identification of label  */
  FILE     *fd;          /* file descriptor for the geoid table   */
  int       used;        /* # of uses of the table                */
};

struct  wrh_lab {
  short     lab_type;    /* type of label */
  short     version;     /* Label version */
  char      mlb[MLBLNG]; /* "wor"  */
  short     sepix;       /* index of Separator-char in mlb */
  short     cmplt;       /* geo_lab definitions completed by conv_lab */
  short     cstm;        /* not used */
  short     bnorm;       /* adjustment norm */
  short     region;      /* Region, see conv_lab.h */
  char      c_mlb[MLBLNG]; /* coord mlb */
  double    adj_date;    /* date of adjustment */
  int       prodnr;      /* Number of producent */
  int       ch_sum;      /* Checksum for identification of label  */
};


/* geo_lab   ver 2010.01          # page 8   5 Jan 2010 11 57 */


struct  wor_lab {
  struct  wrh_lab    wh_lab;
  struct  coord_lab  wc_lab;
};

struct  poc_lab {
  short     lab_type;    /* type of label */
  short     version;     /* Label version */
  char      mlb[MLBLNG]; /* "phoc"  */
  short     sepix;       /* index of Separator-char in mlb */
  short     cmplt;       /* geo_lab definitions completed by conv_lab */
  short     cstm;        /* not used */
  short     bnorm;       /* not used */
  short     region;      /* Region, see conv_lab.h */
  char      c_mlb[MLBLNG]; /* coord mlb */
  int       ch_sum;      /* Checksum for identification of label  */
};

struct  obs_lab {
  short     lab_type;       /* Type of label */
  short     version;        /* Label version */
  char      mlb[MLBLNG];    /* Minilabel of the system */
  short     sepix;          /* index of Separator-char in mlb */
  short     cmplt;          /* Completed by fsclab */
  short     obs_kind;       /* Obs kind */
  short     obs_type;       /* Obs_type */
  short     region;         /* Region, see conv_lab.h */
  double    m2d;            /* 1. mean error component, m2d */
  double    m2c;            /* 2. mean error component, m2c */
  double    spp;            /* Special parameter */
  struct typ_dec  md_tpd;   /* 1. mean error typ_dec */
  struct typ_dec  mc_tpd;   /* 2. mean error typ_dec */
  struct typ_dec spp_tpd;   /* Special parameter typ_dec */

  /* Additional mean error components for special obs_types */
  double    m2dx;           /* 1. additional mean error, m2dx */
  double    m2cx;           /* 2. additional mean error, m2cx */
  struct typ_dec  mdx_tpd;
  struct typ_dec  mcx_tpd;
  short     datum;          /* Datum for reduced observations */
  int       ch_sum;         /* Checksum for identification of label */
};


/* geo_lab   ver 2010.01          # page 9   5 Jan 2010 11 57 */


/* Mean errors completed by conv_lab */
/* 2 => cmplt and ok    ??? else 1   */

/* Obs kind */
/* 0 = undefined data                             */
/* 1 = geom. geod.   (dir, dst, Lapl., vt3)       */
/* 2 = red. geom. g. (dir, dst, Lapl.,)           */
/* 3 = levelling     (prs, niv, zds, mtz)         */
/* 4 = coordinate block (cl, acl, w, hn, he, hs)  */
/* 5 = Photogrammetry mono observations (cl, acl) */
/* 6 = gravity observations                       */
/* 7 = reserved for expansion                     */

/* Obs_type */
/* 0 = undefined                                   */
/* 1 ... ? depending on obs_kind, TBD              */

union geo_lab {
  struct coord_lab        u_c_lab;
  struct hgt_lab          u_h_lab;
  struct obs_lab          u_o_lab;
  struct gde_lab          u_g_lab;
  struct wor_lab          u_w_lab;
  struct plm_lab          u_m_lab;
  struct poc_lab          u_p_lab;
  struct idt_lab          u_i_lab;
};

/* LABEL_TABLE */

/* The struct label_table is defined in geo_lab.h in order to make */
/* it available as a parameter for conv_crd, conv_hgt, etc.        */
/* To be CONDEMNED */
  struct label_table {
    short     lab_type;
    short     d_kind;    /* system number      ( = cstm )           */
    short     d_type;    /* sub_nmb of system  ( = mode )           */
    short     prefix;    /* National prefix                         */
    char     *name;      /* minilab for search  -> mlb1             */
    short     sepch;     /* index of Separator-char in name (mlb)   */
    char     *mlb2;      /* Part 2 of minilabel                     */
    char      cstr;      /* 1 => change digits to *, 0 => no change */
    char      q_par;     /* # of additional params                  */
    char      pr_dt;     /* 1 => predefined datum  "P.D."           */
    char     *addl;      /* additional params by short name         */
    char     *text;      /* name of system                          */
  };

  struct lab_def_str {
    short     lab_type;
    short     d_kind;         /* system number      ( = cstm )      */
    short     d_type;         /* sub_nmb of system  ( = mode )      */
    short     cha_str;        /* pos of digits to be changed to '*' */
    short     region;         /* National prefix from label         */
    short     lab_rgn;        /* National prefix from prj           */
    char      name[MLBLNG];   /* minilab for search  -> mlb1        */
    short     sepch;          /* Separator-char found in name (mlb) */
    short     mask;           /* special transformation info        */
    char      mlb2[MLBLNG];   /* Part 2 of minilabel                */
    char      a_seq[4];       /* axis sequence and name             */
    char      q_par;          /* # of additional params             */
    char      add_p[32];      /* additional params by short name    */
    char      par_str[128];   /* some of BO N0 L0 E0 B1 B2 scale    */
    char      pr_dtm[MLBLNG]; /* name of datum                      */
    char      text[128];      /* name of system                     */
  };

struct  htr_c_str {
    int       inv;
    double    LAT0;
    double    LON0;
    double    M0;
    double    N0;
    double    a1;
    double    a2;
    double    a3;
  };

union  rgn_un {
    char      prfx[4];
    short     r_nr[2];
  };

struct  o_fp_str {
  FILE            *o_fp;
  int              o_lc;
  int              o_pg;
  long             o_file_pos;
  double           out_time;
  char            *o_hd;
  union geo_lab   *u_lab;
};


/* geo_lab   ver 2010.01          # page 10  5 Jan 2010 11 57 */


struct st_str {
  short    typ;
  short    rgn;
  short    hnr;
  short    snr;
  int      lbn;
  short    slt;
  short    suf;
};

union stn_pck {
  struct st_str    stn;
  int              pck[4];
};

union typ_uni {
  struct typ_dec  p_typ_dec;
  int             p_typ_isq;
};

#define STXT   0
#define LDNR   1
#define STNR   2
#define NVNR   3
#define PHNR   4
#define DOSF   5
#define LBNR   6
#define LINR   7
#define GPSR   8
#define TERM  18
#define RPAR  63


/* geo_lab   ver 2010.01          # page 11   5 Jan 2010 11 57 */


/* Enumeration of NVNR, incl. Gravity Points  */

#define  GINR     1
#define  GMNR     2
#define  KMSNR    3
#define  KKNR     4
#define  FKNR     5
#define  GPNR     6
#define  GGNR     7

#define  GINM     "G.I."  /* 1 */
#define  GMNM     "G.M."  /* 2 */
#define  KMSNM    "KMS."  /* 3 */
#define  KKNM     "K.K."  /* 4 */
#define  FKNM     "F.K."  /* 5 */
#define  GPNM     "G.P."  /* 6 */
#define  GGNM     "G.G."  /* 7 */

#define  NIVTAB   GINM,GMNM,KMSNM,KKNM,FKNM,GPNM,GGNM


/* geo_lab   ver 2010.01          # page 12   5 Jan 2010 11 57 */


/* Structure of stn record. type st_str (description) */

/* field size    2      2      2      2      4      2      2      */
/* name         typ    rgn    hnr    snr    lbn    slt    suf     */
/* type        short  short  short  short   int   short  short    */

/* STXT          0      R     <-    text max 11 char      ->\0    */

/* LDNR          1      R     ddm    ddm   dddm      0        0      */
/* LDNR.sub      1      R     ddm    ddm   dddm      0       ddm     */

/* STNR          2      R      0      0    dddm      0        0      */
/* STNR.sub      2      R      0      0    dddm      .       ddm     */
/* STNR C        2      R      0      0    dddm      C        0      */
/* STNR C kal    2      R      0      0    dddm      C       ddm     */

/* NVNR          3      R      N     ddm    0        0        0      */
/* NVNR/sub      3      R      N     ddm   ddm       0        0      */
/* NVNR.sub      3      R      N     ddm    0        .       ddm     */
/* NVNR/sub.sub  3      R      N     ddm   ddm       .       ddm     */
/* GGNR    /sub is illegal                                           */

/* PHNR          4      R      C1    ddm    0        0        0      */
/* PHNR.sub      4      R      C1    ddm    0        0       ddm     */
/* PHNR          4      R      C1    ddm    C2      ddm       0      */
/* PHNR.sub      4      R      C1    ddm    C2      ddm      ddm     */
/* QHNR          4      R      Q    DDDD   DDDD     ddm       0      */
/* QHNR.sub      4      R      Q    DDDD   DDDD     ddm      ddm     */
/* FHNR          4      R      F      0    dddm      0        0      */
/* FHNR.sub      4      R      F      0    dddm      0       ddm     */

/* DOSF          5      R     ddm    ddm    0        C               */
/* DOSF.sub      5      R     ddm    ddm    0        C       ddm     */

/* LBNR          6      R      0      0    dddm      0        0      */
/* LBNR.sub      6      R      0      0    dddm      .       ddm     */

/* LINR          7      R     DDDD    0    dddm      0        0      */
/* LINR.sub      7      R     DDDD    0    dddm      .       ddm     */
/* LINR-sub      7      R     DDDD    0    dddm      -       ddm     */
/* LINR C        7      R     DDDD    0    dddm      C        0      */
/* LINR C kal    7      R     DDDD    0    dddm      C       ddm     */

/* (to be continued on next page) */

/* ddd   =>   max  3 cifre                                       */
/* ddn   =>   max  3 cifre and  0 <= number <=           255     */
/* ddm   =>   max  5 cifre and  0 <= number <=        32_767     */
/* dddm  =>   max 10 cifre and  0 <= number <= 2_147_483_647     */
/* D     =>   Digit                                              */
/* C     =>   Capital                                            */
/* C1    =>   Capital excl. F and Q                              */
/* C2    =>   Capital                                            */
/* F     =>   Capital == F                                       */
/* Q     =>   Capital == Q                                       */
/* N     =>   Enum. of G.I., G.M., etc, G.G.                     */
/* R     =>   Region number from conv_rgn function or 0          */
/* .     =>   Separator char                                     */


/* geo_lab   ver 2010.01          # page 13   5 Jan 2010 11 57 */


/* Structure of stn record. type st_str (continued) */

/* field size    2      2      2      2      4      2      2      */
/* name         typ    rgn    hnr    snr    lbn    slt    suf     */
/* type        short  short  short  short   int   short  short    */

/* GPSR          8      R      CC    CC     0        0        0      */
/* GPSR dome     8      R      CC    CC    gpsC     DDD       0      */
/* GPSR dome     8      R      CC    CC    gpsC     DDD       C      */
/* GPSR          8      R      CD    CC     0        0        0      */
/* GPSR dome     8      R      CD    CC    gpsC     DDD       0      */
/* GPSR dome     8      R      CD    CC    gpsC     DDD       C      */
/* GPSR          8      R      CC    CD     0        0        0      */
/* GPSR dome     8      R      CC    CD    gpsC     DDD       0      */
/* GPSR dome     8      R      CC    CD    gpsC     DDD       C      */

/* gpsC  =>   (5 cifre) * 1000 + Capital    <= 2_147_483_647     */
/* D     =>   Digit                                              */
/* C     =>   Capital                                            */
/* R     =>   Region number from conv_rgn function or 0          */


/* Definition 26 FEB 1991, Knud Poder */
/* Definition  9 FEB 1993, Frands     */
/* Modified   20 MAR 1993, KP         */
/* geo_lab and iogeo joined 10 NOV 93, KP */
/* Macros for max, min, and abs  4 MAR 1994, KP, removed 1 NOV 94 */
/* Station number type = 7 (LSP) 30 MAR 1994, FS */
/* Geoid label                    1 NOV 1994, KP */
/* Station number description updated 27 JAN 1995, FS */
/* Geoid label                    1 DEC 1998, KP */
/* Geoid label                    1 FEB 1999, KE */
/* GPS number with domeid         1 MAY 2001, KE */
/* Q-version of PHNR             27 AUG 2004, KE */

#endif
