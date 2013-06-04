

#if 0
  BUILD:
      awk -f 0xD0C.awk trlib_foundations.h
      pdflatex trlib
      pdflatex trlib
      start TYPES.pdf
#endif


/* -- PRODUCT TRLIB ------------------------------------------- */
#if !0xD0C
\summary{Foundations of the transformation library}
\author{The TRLIB team}
\date{2013-06-04}
\SECTION{Preface} Bla bla bla
#endif


/* -- MODULE TYPES ------------------------------------------- */

#if !0xD0C
%\summary{TRLIB Data Types}
%\author{The TRLIB team}
%\date{2013-04-17}
%\SECTION{Preface} Bla bla bla
#endif

/* -- Datatype 0 ------------------------------------------- */
struct  dsh_str {
  int        tp;             /* dsh type       */
  double     tx, ty, tz;     /* translations   */
  double     scale;          /* scale (= 1+dm) */
  double     r11, r12, r13;
  double     r21, r22, r23;  /* rot[3][3]      */
  double     r31, r32, r33;
};
/* structure of datum shift constants */

/* -- Datatype 0 ------------------------------------------- */
struct  gps_c_str {
  char       datum[16];      /* datum name            */
  double     sc;             /* scale                 */
  double     tx, ty, tz;     /* translations          */
  double     rx, ry, rz;     /* molodensky scale_rot  */
};
/* structure of gps shift constants */

/* -- Datatype 0 ------------------------------------------- */
struct  plm_c_str {
  char       datum[16];      /* datum name            */
  double     drx, dry, drz;  /* molodensky scale_rot  */
};
/* structure of itrf plate_motions constants */


/* -- Datatype 0 ------------------------------------------- */
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
/* structure of itrf plate_motions info blocks */


/* -- Datatype 0 ------------------------------------------- */
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
/* structure of gps entries */
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

/* -- Datatype 0 ------------------------------------------- */
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



/* -- Datatype 0 ------------------------------------------- */
struct typ_dec {
  unsigned char   gf;
  unsigned char   tf;
  unsigned char   ff;
  unsigned char   df;
};


/* -- Datatype 0 ------------------------------------------- */
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


/* dummy content */
/* -- Datatype 0 ------------------------------------------- */
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


/* -- Datatype 0 ------------------------------------------- */
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

/* -- Datatype 0 ------------------------------------------- */
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

/* -- Datatype 0 ------------------------------------------- */
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


/* -- Datatype 0 ------------------------------------------- */
struct  wor_lab {
  struct  wrh_lab    wh_lab;
  struct  coord_lab  wc_lab;
};



/* -- Datatype 0 ------------------------------------------- */
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


/* -- Datatype 0 ------------------------------------------- */
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

/* -- Datatype 0 ------------------------------------------- */
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
/* -- Datatype 0 ------------------------------------------- */
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

/* -- Datatype 0 ------------------------------------------- */
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

/* -- Datatype 0 ------------------------------------------- */
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

/* -- Datatype 0 ------------------------------------------- */
union  rgn_un {
    char      prfx[4];
    short     r_nr[2];
  };

/* -- Datatype 0 ------------------------------------------- */
struct  o_fp_str {
  FILE            *o_fp;
  int              o_lc;
  int              o_pg;
  long             o_file_pos;
  double           out_time;
  char            *o_hd;
  union geo_lab   *u_lab;
};


/* -- Datatype 0 ------------------------------------------- */
struct st_str {
  short    typ;
  short    rgn;
  short    hnr;
  short    snr;
  int      lbn;
  short    slt;
  short    suf;
};

/* -- Datatype 0 ------------------------------------------- */
union stn_pck {
  struct st_str    stn;
  int              pck[4];
};

/* -- Datatype 0 ------------------------------------------- */
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


































/* ### *** XXX tabelsnak herunder */


/* geoid_d  ver 2004.01           # page 1   25 Jun 2004 11 37 */

/* In this library should allso be found the file :           */
/*       manager.tab                                          */
/* which gives the names of tables and their important adm.   */
/* data and gives the strategy for electing the proper table  */
/* Private tables may be put in the current library           */
/* where a private copy of the manager.tab allso may be found */

#define  GEOIDPGSIZE      16   /* 16 float or 64 bytes        */
#define  TAB3DPGSIZE      48   /* (3d) 16 float               */
#define  TABDDPGSIZE      32   /* (2d) 16 double              */
#define  GEOIDTABHEAD     64   /* Size of head defining table */
#define  GEOIDHEADER       1   /* 1=>compatible with fortran  */
#define  GEOIDPAGES        8   /* 2 is feasible               */
#define  MAXGEOIDS        10   /* Geoids no 0 -(MAXGEOIDS-1)  */
#define  STANDARD_GEOID   "nkg96g.01"
#define  BACKUP_GEOID     "dma84g.b1"

/* -- Datatype 0 ------------------------------------------- */
union  tab_pages {
  float            fghtab[GEOIDPGSIZE];     /* g-height as fl. */
/*  unsigned char    bghtab[4*GEOIDPGSIZE]; */  /* g-height + 128  */
};

/* -- Datatype 0 ------------------------------------------- */
union  t3d_pages {
  float            fghtab[TAB3DPGSIZE];     /* 3d_val as fl. */
/*  unsigned char    bghtab[4*TAB3DPGSIZE]; */  /*               */
};

/* -- Datatype 0 ------------------------------------------- */
union  tdd_pages {
  double           fghtab[TABDDPGSIZE];     /* 2d_val as double */
/*  unsigned char    bghtab[4*TABDDPGSIZE]; */  /*               */
};

/*
geoid_c is IMPORTANT to call before releasing the struct mgde_str
        to close the file pointer used.
+++ IMPORTANT to release file pointers in system +++
gd_trans(NULL, .... will close file pointers to special tables used
+++ IMPORTANT to release file pointers in system +++
*/

/* -- Datatype 0 ------------------------------------------- */
struct mgde_str {
  union  tab_pages     g_pg[GEOIDPAGES];  /* pages loaded in RAM  */
  struct gde_lab       table_u[MAXGEOIDS];/* geoid_units no. 0-7  */

  int                  hit;               /* grid_val in page     */
  int                  agn;               /* grid_val to page     */
  int                  xmt;               /* hole in grid-area    */
  long                 pge[GEOIDPAGES];   /* act. stream for page */
  int                  sgd[GEOIDPAGES];   /* act. nmb. of grid    */
  int                  victim;            /* fifo choice of page  */
  int                  tab_max;           /* number of tables     */

  /* Params set by the progr. or funct. calling the trnsf. func.  */
  int                  init;              /* 0 => initialize      */
  int                  tab_nr;            /* actual table         */
};

/* -- Datatype 0 ------------------------------------------- */
struct mtab3d_str {
  union  t3d_pages     g_pg[GEOIDPAGES];  /* pages loaded in RAM  */
  struct gde_lab       table_u;           /* geoid_units no. 0-7  */

  int                  hit;               /* grid_val in page     */
  int                  agn;               /* grid_val to page     */
  int                  xmt;               /* hole in grid-area    */
  long                 pge[GEOIDPAGES];   /* act. stream for page */
  int                  sgd[GEOIDPAGES];   /* act. nmb. of grid    */
  int                  victim;            /* fifo choice of page  */
  int                  tab_max;           /* number of tables     */

  /* Params set by the progr. or funct. calling the trnsf. func.  */
  int                  init;              /* 0 => initialize      */
  int                  tab_nr;            /* actual table         */
};

/* -- Datatype 0 ------------------------------------------- */
struct mtabdd_str {
  union  tdd_pages     g_pg[GEOIDPAGES];  /* pages loaded in RAM  */
  struct gde_lab       table_u;           /* geoid_units no. 0-7  */

  int                  hit;               /* grid_val in page     */
  int                  agn;               /* grid_val to page     */
  int                  xmt;               /* hole in grid-area    */
  long                 pge[GEOIDPAGES];   /* act. stream for page */
  int                  sgd[GEOIDPAGES];   /* act. nmb. of grid    */
  int                  victim;            /* fifo choice of page  */
  int                  tab_max;           /* number of tables     */

  /* Params set by the progr. or funct. calling the trnsf. func.  */
  int                  init;              /* 0 => initialize      */
  int                  tab_nr;            /* actual table         */
};























/* parse_def_file.h  ver 2013.01      # page 1     19 Feb 2013 11 57 */


 #define MAX_TABLE_LEN (32)
 #define MAX_DSCR_LEN (128)

/* -- Datatype 0 ------------------------------------------- */
struct def_grs {
	char mlb[MLBLNG];
	short no;
	short mode;
	double axis;
	double flattening;
	double km;
	double omega;
};

/* -- Datatype 0 ------------------------------------------- */
struct def_rgn {
	char rgn_new[3];
	char rgn_old[3];
	char country[64];
};

/* -- Datatype 0 ------------------------------------------- */
struct def_datum {
	char mlb[MLBLNG];
	int no;
	char p_datum[MLBLNG];
	char ellipsoid[MLBLNG]; /*really a reference to a def_grs struct */
	int imit;
	int type;
	int p_no;
	char rgn[3];
	double translation[3];
	double rotation[3];
	double scale;
	char descr[MAX_DSCR_LEN];

};

/* -- Datatype 0 ------------------------------------------- */
struct def_projection{
	char mlb[MLBLNG];
	int cha_str, type, cstm, mode, mask;
	char seq[4];
	char rgn[3];
	char p_datum[16];
	int q_par;
	char param_tokens[32];
	char param_text[128];
	char native_proj[MLBLNG];
	double native_params[8];
	char descr[MAX_DSCR_LEN];

};

/* -- Datatype 0 ------------------------------------------- */
struct  def_hth_tr{
	char from_mlb[MLBLNG];
	char to_dtm[MLBLNG];
	int type;
	double B0;
	double L0;
	double constants[5];
	char table[MAX_TABLE_LEN];
	char descr[MAX_DSCR_LEN];
};


/* -- Datatype 0 ------------------------------------------- */
typedef struct def_hth_tr def_hth_tr;
/* -- Datatype 0 ------------------------------------------- */
typedef struct def_grs def_grs;
/* -- Datatype 0 ------------------------------------------- */
typedef struct def_datum def_datum;
/* -- Datatype 0 ------------------------------------------- */
typedef struct def_projection def_projection;
/* -- Datatype 0 ------------------------------------------- */
typedef struct def_rgn def_rgn;

/* -- Datatype 0 ------------------------------------------- */
struct def_data{
	def_projection *projections;
	def_datum      *datums;
	def_grs    *ellipsoids;
	def_hth_tr *hth_entries;
	def_rgn     *regions;
	int n_prj;
	int n_dtm;
	int n_ellip;
	int n_rgn;
	int n_hth;
};

/* -- Datatype 0 ------------------------------------------- */
typedef struct def_data def_data;


























/* proj4_to_mlb.h                  # page 1   DD Mmm 2012 hh mm */

/* -- Datatype 0 ------------------------------------------- */
struct proj4_entry{
	char proj[64];
	char datum[64];
	char vdatum[64];
	char ellps[64];
	char units[16];
	char nadgrids[64];
	double towgs84[7];
	int n_params;
	int zone;
	double x_0;
	double y_0;
	double lat_0;
	double lat_ts;
	double lon_0;
	double k;
	double a;
	double b;
 };




/* -- Datatype 0 ------------------------------------------- */
typedef struct proj4_entry proj4_entry;























/* typ_dec.h  version 1.1           # page 1   22 Feb 1994 13 13 */


/* -- Datatype 0 ------------------------------------------- */
struct na_gr_tp_fc {
  char        na[5];
  unsigned char     gr; /* group */
  unsigned char     tp; /* type  */
  short             dd; /* default digits bef. dec.-point */
  short             md; /* max no. digits bef. dec.-point */
  double            fc;  /* conv.  factor from int to ext */
};

#if !0xD0C
   Defined in geo_lab.h
struct typ_dec {
  unsigned char   gf;  // group field
  unsigned char   tf;  // type field
  unsigned char   ff;  // minimal chars before point
  unsigned char   df;  // decimals after point
};
// SPECIAL:: df == 255 will shift to highest possible unit having no decimals
// for 1) sx, nt, dg, 2) cc, gr,  3) mm, cm, dm, m, km
// zero decimal will be written in any case!
// e.g. 9 00 sx will be written: 9nt and 9 00 00 sx be written 9dg
// e.g. 9 00.1 sx will be written: 9nt and 9 00.5 sx be written 9 01sx




typ_dec.h contains the source code of declarations used by
the functions set_tpd and get_tpd.

The information is a structure array containing

1.  the name of the unit
2.  the group of the unit
3.  the type of the unit
4.  the conversion factor of the unit

The name is a letter string of 1-4 characters identifying
the unit.

The groups are:

1. sexagesimal angular values
type 1: degrees, minutes and (arc)seconds               (sx)
type 2: hours, minutes and (time)seconds                (tm)
type 3-6: as type 1, but N,S,E,W are available      (N) (S)
          for convenience, S and W means negative   (E) (W)
          values.
type 7: not used at present
type 8: degrees and minutes                             (nt)
type 9: mili(degrees, minutes and (arc)seconds)         (mas)

2. decimal angular values
type 1: nonagesimal angles (old-degrees with decimals)  (dg)
type 2: arcseconds                                      (asx)
type 3: radians                                         (rad)
type 4: microradians  (1.0e-6 rad)                      (yrad)
type 5: centesimal degrees, minutes and seconds         (cc)
type 6: gon                                             (gr)
type 7: gon                                             (gon)
type 8: mgon                                            (mgon)
type 9: ts (="thousands") 4000 ts is 2 pi               (ts)
type 10: picoradians  (1.0e-9 rad)                      (prad)

3. not used

4. lengths units
type 1: metres                                       (m)
type 2: microns                                      (my)
type 3: millimetres                                  (mm)
type 4: centimetres                                  (cm)
type 5: decimetres                                   (dm)
type 6: kilometres                                   (km)
type 7: nautical miles 1 nm = 1852.0 m               (nm)

5. astronomical units
type 1: astronomical unit in meters                  (aum)
              149 597 870 691 m
type 2: astronomical unit in seconds                 (aus)
                          499.004 783 806 1 sec
type 3: lysår                                        (aly)
        9 460 730 472 580 800 m

6. - 7. not used

8. potential units
type 1: gpu, geopotential units (appr. 0.98 m)       (gpu)
type 2: milli-gpu                                    (mgpu)
type 3: micro-gpu                                    (ygpu)

9. not used

10. gravity units
type 1: gal    (cm/sec2)                             (gal)
type 2: milligal                                     (mgal)
type 3: microgal                                     (ygal)
type 4: kilogal                                      (kgal)

11. not used

12. mean error units
type 1: ppm   (parts pr million)                     (ppm)
type 2: ppb   (parts pr billion)                     (ppb)
type 3: ne    (mm pr sqrt(km))                       (ne)

13. - 255. not used

Group no. 0 is used as a signal for undefined.

The conversion factor is used to give a unique
internal representation:

group 1 and 2: radians
group 4:       metres
group 8:       geopotential units
group 10:      gal
group 12:      units of 1.0

The conversion factor is a multiplicator when the
conversion is to the unique interenal representation,
and a divisor when internal units are converted to
the external representation.

It is thus quite feasible to take input in one unit
and give output in any other sensible unit.

#endif






























/* -- MODULE GEOG ------------------------------------------- */
#if !0xD0C
%\summary{TRLIB Geometric Geodesy}
%\author{Karsten Engsager and Knud Poder}
%\date{1977--2013}
%\SECTION{Preface} Bla bla bla
#endif
/* -- Function 1 -----------------------------------------------------------------*/
double           bh_lat (
double                  b,
double                  az,
double                  em2,
double                  *K1,
double                  *s1
);

#if !0xD0C
= bh_lat      The max positive reduced latitude of a geodesic
              starting from reduced latitude red_lat in azimuth az.
= b           The reduced latitude of the geodesic where the azimuth is az,
              in units of radians.
= az          The azimuth of the geodesic in reduced latitude red_lat.
              The azimuth is reckoned from north, but the orientation
              of the geodesic is immaterial.
              The unit is radians.
= em2         The second eccentricity of the ellipsoid,
              $f*(2-f)/(1-f)/(1-f)$.
= K1          The generating parameter for the coefficients of the series
              of the longitude difference and the lengths of the geodesic.
= s1          The length of the great circle on the auxiliary sphere from
              the starting point to the to the max reduced latitude point,
              counted in units of radians.
= END
Prog.: Knud Poder SEP 1991.

#endif

/* bh_lmb.h                           # page 1   22 Feb 1993 12 54 */

/* -- Function 1 -----------------------------------------------------------------*/
double                bh_lmb (
double           K1,
double           f,
double           bm,
double           s1,
double           s
);


#if !0xD0C
= bh_lmb determines the correction to the spherical longitude difference
       to get the geodetic longtude difference.

= K1     The generating parameter for the coefficients of the series of
       the longitude difference and the lengths of the geodesic.

= f      flattening of the ellipsoid.


= bm     the max positive reduced latitude of a geodesic
       starting from reduced latitude red_lat in azimuth az.

= s1     The length of the great circle on the auxiliary sphere from
       the starting point to the to the max reduced latitude point,
       counted in units of radians.

= s      the length of the great circle on the auxiliary sphere from
       the starting point to the object, counted in units of radians.
= END
#endif
/* bshlm1.h                           # page 1    1 Mar 1993 11:18 */

/* -- Function 1 -----------------------------------------------------------------*/
int               bshlm1 (
double                   a,
double                   f,
double                   B1,
double                   *B2,
double                   L1,
double                   *L2,
double                   A1,
double                   *A2,
double                   S
);


#if !0xD0C
The function bshlm1 solves the direct (or first) geodetic
main problem using the Besssel-Helmert formulae.

Reference: JORDAN-EGGERT-KNEISSL: Handbuch der Vermessungs-
kunde Vol. IV, MCMLIX, p. 968-969. Eq. (xx), (xx), and (xx).

= bshlm1         (call \& return)     int
The result of the call: 0 => ok, 1 => ok, but the line is
shorter than a/1000000.

= a            (call)              double
The semi major axis of the ellipsoid in units of metres.

= f            (call)              double
The flattening of the ellipsoid. The flattening or its inverse
may be used freely.

= B1           (call)              double
latitude of station 1 in radians.

= *B2          (return)            double
latitude of station 2 in radians.

= L1           (call)              double
longitude of station 1 in radians.

= *L2          (return)            double
longitude of station 2 in radians.

= A1          (call)              double
the azimuth at station 1 towards station 2 in radians.

= *A2         (return)            double
the azimuth at station 2 towards station 1 in radians.

= S            (call)              double
the length of the geodesic between the stations in units of
metres.
= END

Prog: Knud Poder NOV 1990

#endif



/* bshlm2.h                         # page 1   18 Feb 1994 13 50 */

/* -- Function 1 -----------------------------------------------------------------*/
int              bshlm2 (
double                  a,
double                  f,
double                  B1,
double                  B2,
double                  L1,
double                  L2,
double                *A1,
double                *A2,
double                *S,
int                    mode
);

#if !0xD0C
The function bshlm2 solves the reverse (or second) geodetic
main problem using the Besssel-Helmert formulae.

Reference: JORDAN-EGGERT-KNEISSL: Handbuch der Vermessungs-
kunde Vol. IV, MCMLIX, p. 968-969. Eq. (xx), (xx), and (xx).

= bshlm2  The result of the call:
        0 => ok
        1 => distance too short, but a result anyway
        2 => no convergence, disaster, probably antipode problem

= a       The semi major axis of the ellipsoid in units of metres.

= f       The flattening of the ellipsoid. The flattening or its
        inverse may be used freely.

= B1      latitude of station 1 in radians.

= B2      latitude of station 2 in radians.

= L1      longitude of station 1 in radians.

= L2      longitude of station 2 in radians.

= A1      the azimuth at station 1 towards station 2 in radians.

= A2      the azimuth at station 2 towards station 1 in radians.

= S       the length of the geodesic between the stations in units
        of metres.
= END

Prog: Knud Poder NOV 1990
#endif

/* bshlm2g.c                        # page 1   18 Feb 1994 13 55 */

/* -- Function 1 -----------------------------------------------------------------*/
int     bshlm2g (
double         a,
double         f,
double         B1,
double         B2,
double         L1,
double         L2,
double        *A1,
double        *A2,
double        *S
);

#if !0xD0C
The function bshlm2g solves the reverse (or second) geodetic
main problem using the GAUSS mid-latitude formulae for distances
less than appr. 65 km and at latitudes between 85 N and 85 S,
and the BESSEL-HELMERT formulae otherwise.

Reference: JORDAN-EGGERT-KNEISSL: Handbuch der Vermessungs-
kunde Vol. IV, MCMLIX, p. 968-969. Eq. (xx), (xx), and (xx).

= bshlm2g  The result of the call:
         0 => ok
         1 => distance too short, but a result anyway
         2 => no convergence, disaster, probably antipode problem

= a        The semi major axis of the ellipsoid in units of metres.

= f        The flattening of the ellipsoid. The flattening or its
         inverse may be used freely.

= B1       latitude of station 1 in radians.

= B2       latitude of station 2 in radians.

= L1       longitude of station 1 in radians.

= L2       longitude of station 2 in radians.

= A1       the azimuth at station 1 towards station 2 in radians.

= A2       the azimuth at station 2 towards station 1 in radians.

= S        the length of the geodesic between the stations in units
         of metres.
= END

Prog: Knud Poder APR 1993.

#endif


/* c_gps_c    ver 2003.01             # page 1    4 Jul 2003 12 06 */

/* -- Function 1 -----------------------------------------------------------------*/
int       c_gps_c (
struct gps_c_str    *shp,
double                Xi,
double                Yi,
double                Zi,
double                *Xo,
double                *Yo,
double                *Zo
);

#if !0xD0C
 transforms cartesian coordinates to cartesian coordinates

 Molodensky transformation SMALL DATUM SHIFTS only

 (*Xo) = (Xi) + (T1) + ( D  -R3  R2) (Xi)\\
 (*Yo) = (Yi) + (T2) + ( R3  D  -R1) (Yi)\\
 (*Zo) = (Zi) + (T3) + (-R2  R1  D ) (Zi)\\
#endif



/* c_tabdir_file  ver 2010.01          # page 1    15 Feb 2010 15 35 */


/* -- Function  1 -----------------------------------------------------------------*/
int            cic (
struct dsh_str        *shp,
int                   direct,
double                Xi,
double                Yi,
double                Zi,
double                *Xo,
double                *Yo,
double                *Zo,
char                  *usertxt,
FILE                  *tr_error
);

#if !0xD0C
 cic:      Cartesic_Transformation_to_Cartesic

= *shp:     datum shift parameters
=  direct>0: the direct formula is used
=  direct<0: the inverse formula is used
=  Xi,Yi,Zi: from coordinates
=  Xo,Yo,Zo: to   coordinates
=  usertxt:  to be printed on error
=  tr_error: file for usertxt on error
= END
#endif



/* clenshaw.h version 1.0             # page 1   11 Aug 1994 12 54 */

/* -- Function 1 -----------------------------------------------------------------*/
int            ctc (
union geo_lab         *i_lab_u,
union geo_lab         *o_lab_u,
double                Xi,
double                Yi,
double                Zi,
double                *Xo,
double                *Yo,
double                *Zo,
char                  *usertxt,
FILE                  *tr_error
);

#if !0xD0C
 ctc:      Cartesic_Transformation_to_Cartesic

=  i_lab_u  from label
=  o_lab_u  to   label
=  Xi,Yi,Zi from coordinates
=  Xo,Yo,Zo to   coordinates
=  tr_cnt   == 0 causes break on error, othervice continue
=  usertxt  to be printed on error
=  tr_error file for usertxt on error
= END
#endif


/* cw from JD or n                 # page 1    1 Feb 2004 11:18 */

/* -- Function 1 -----------------------------------------------------------------*/
double               erad (
double               a,
double               f,
double               lat,
int                  ft
);

#if !0xD0C
 Function
 erad computes, using the semi-major axis and the flattening
(or automatically the inverse flattening):

        (a) 4 radii of curvatures of the ellipsoid,
            as a function of the latitude,
           (a1) M: the radius of crv. in the meridian
           (a2) N: the radius of crv. in the prime vertical
           (a3) R: the mean radius of crv.
           (a4) r: the radius of a latitude parallel

        (b) G: The Gaussian curvature as a function of
               the latitude

        (c) K: Half the Gaussian curv. as a function of
               the northing

        (d) Q: The length of the meridian arc from eq. to
               the latitude

        (e) L: The latitude corresponding to the length
               of the meridian arc.
 Parameters:

 a            call               double
 The semi-major axis of the ellipsoid in units of metres.
 NB: use a*(central scale) for curvatures related to projections.

 f            call               double
 The flattening of the ellipsoid (the inverse flattening
 may be used instead with equal effect).

 lat          call               double
 The latitude in units of radians or the meridian arc in units
 of metres according to the desired conversion.

 ft           call               int
 The flag controlling the computation:

 'M' => the meridian curvature radius
 'N' => the prime vertical curvature radius
 'R' => the mean curvature radius = sqrt(M*N)
 'r' => the radius of a latitude parallel = N cos(lat)
 'G' => the Gaussian curvature as a fncti. of the latitude
 'K' => half the Gauss. curv. as a fnct. of the Northing
 'Q' => the meridian arc
 'L' => the latitude

 Any other value of ft gives an alarm exit.

 Prog. KP OCT 1989/JAN 1992.

#endif


/* etrs89_trans  ver 2009.03          # page 1   04 Mar 2009 11 36 */
/* -- Function 1 -----------------------------------------------------------------*/
double        gatg (
double               *TC,
int                 direct,
double               B
);
#if !0xD0C
    Function and Parameters

gatg               double function           call and return
Dual transformation function from Gaussian latitude to
geodetic latitude and the reverse transformation. The
returned value is the transformed latitude. Note that
gatg has no internal reverse check computation, leaving
this to the user's programme. The gatg uses Clenshaw sum-
mation for a series expressing the difference of the two
kinds of latitude by diff = sum(a[j-1]*sin(2*j*B)), with
j = 4,3,2,1, a[] as a function of n (3. flattn.), and
B is the relevant latitude. The coefficients for both
directions are storeed in TC, which is initialized by
conv_lab with the parameters of the actual ellipsoid,
even in the cases where gatg is not needed. The computation
precision is better than 0.000 000 1 arcseconds.
The output is in units of radians.

TC                  double array              call and return
The constants needed for the transformation, size at
least given by TC[8], but mostly a pointer to a general
array may be preferable. The content is as follows:

        TC[0:3] = the coefficients for the transformation
                Gaussian to geodetic.
        TC[4:7] = the coefficients for the transformation
                geodetic to Gaussian.

direct               int                         call
Determines the direction of the transformation:
        direct > 0  => Gaussian to geogr.
        direct < 0  => geogr. to Gaussian
        direct = 0  => Return of a latitude of 1.0e+30

B                   double                       call
The latitude to be transformed, given in radians.

The formulae are taken from KW = R.Koenig and K.H. Weise,
Mathematische Grundlagen der hoeheren Geodaesie und
Kartographie, Springer-Verlag, Berlin/Goettingen/
Heidelberg, 1951.

gauss->geo: p. 190-91, eq. (61) \& (62).
Last B in (61 0) should be b.
geo->gauss: p. 186-87, eq. (51) \& (52).

prog: Knud Poder 10 APR 1990;
modf: Knud Poder  1 JUN 1992, automatic init now by conv_lab;:
modf: Knud Poder  3 MAR 1994, protyping.
#endif



/* gd_t_val  ver 2001.01          # page 1   21 Dec 2001 11 57 */


/* -- Function 1 -----------------------------------------------------------------*/
int       get_areacf (
union geo_lab   *ilb_p,
double          *B,
double          *L,
int              max,
double          *area,
double          *circum,
int             *used_max,
char            *wrk_path,
char            *usertxt,
FILE            *tr_error
);

#if !0xD0C
 get_areaf  calculates the ellipsoidal area of the coordinates
given by the label ilb_p and coordinates B, L in total max numbers

get_area =
=         0  result == ok
=         1  illegal input coordinate label (ilb_p)
=         2  illegal work label (system error)
=         3  work_area.geo not initiated
=         4  work_area.geo i/o error
=         5  Transformation error (system error)
= END

= *ilb_p     pointer to input label.
= *B         Northing/Latitude coordinates (0:max-1)
= *L         Easting/Longitude coordinates (0:max-1)
             (first and last need not be equal)
= max        number of coordinates.
= *area      the calculated area.
= *circum    The circumference of the area.
= *used_max  the number of sections points for the calculation
= *wrk_path  The PATH to a work directory for two temporary workfiles
             e.g.  "."  (NB: "" is illegal)

= *usertxt   text to be written at transformation error.
= *tr_error  filepointer for error report.
= END

progr.: Karsten engsager, 2. Feb 2004

#endif


/* get_areaf                      # page 1    2 Feb 2004 14 02 */
/* -- Function 1 -----------------------------------------------------------------*/
int       get_areaf (
union geo_lab   *ilb_p,
double          *B,
double          *L,
int              max,
double          *area,
int             *used_max,
char            *wrk_path,
char            *usertxt,
FILE            *tr_error
);

#if !0xD0C
get_areaf  calculates the ellipsoidal area of the coordinates
given by the label ilb_p and coordinates B, L in total max numbers

get_area =
= 0 result == ok
         = 1 illegal input coordinate label (ilb_p)
         = 2 illegal work label (system error)
         = 3 work_area.geo not initiated
         = 4 work_area.geo i/o error
         = 5 Transformation error (system error)
= END

= *ilb_p   : pointer to input label.
= *B       : Northing/Latitude coordinates (0:max-1)
= *L       : Easting/Longitude coordinates (0:max-1)
           (first and last need not be equal)
= max      : number of coordinates.
= *area    : the calculated area in km*km.
= *used_max: the number of sections points for the calculation

= *wrk_path: The PATH to a work directory for two temporary workfiles
         : e.g.  "."  (NB: "" is illegal)

= *usertxt : text to be written at transformation error.
= *tr_error: filepointer for error report.
= END

progr.: Karsten engsager, 2. Feb 2004

#endif



/* get_half.h                        # page 1   28 Apr 1993 11:00 */


/* -- Function 1 -----------------------------------------------------------------*/
double      get_ha_s (
FILE              *fw,
FILE              *fw1,
double             a,
double             f,
int                j,
int               *i,
double             s_max
);

#if !0xD0C
 service routine for get_areaf and get_areacf
 get_ha_s   returns the circumference of the points in fw1

 get_ha_s   copies the j geo_crd in fw to fw1
 when dist(P(i), P(i-1)) > s_max a new point is inserted
 midway between the two points

 a      is the major axis of the ellipsoid
 f      is the flattening of the ellipsoid
 j      is the number of points in fw
 *i     is the number of points in fw1
 s_max  id the max distance for insertion of new points
#endif


/* get_mlb.h     ver 2003.01          # page   4 Jul 2003 14.06 */

/* -- Function 1 -----------------------------------------------------------------*/
int                 gmb1 (
double                     a,
double                     f,
double                     lat1,
double                    *lat2,
double                     lng1,
double                    *lng2,
double                     az1,
double                    *az2,
double                     s
);

#if !0xD0C
The function gmb1 solves the direct (or first) geodetic
main problem using the gaussian mid-latitude formulae.
Reference: JORDAN-EGGERT-KNEISSL: Handbuch der Vermessungs-
kunde Vol. IV, MCMLIX, p. 968-969. Eq. (xx), (xx), and (xx).

gmb1   Result of computing:
       gmb1 == 0      => ok
       gmb1 == 1      => line > 0.01*a (64 km) or abs(lat) > 86 dg.

a      The semi major axis of the ellipsoid in units of metres.

f      The flattening of the ellipsoid. The flattening or its inverse
       may be used freely.

lat1   latitude of station 1 in radians.

lat2   latitude of station 2 in radians. (res)

lng1   longitude of station 1 in radians.

lng2   longitude of station 2 in radians. (res)

az1    the azimuth at station 1 towards station 2 in radians.

az2    the azimuth at station 2 towards station 1 in radians. (res)

s      the length of the geodesic between the stations in units
       of metres.

Prog: Knud Poder NOV 1990 after older algol version by
Frede Madsen 1981 , where 5th order terms were included.
5th order terms removed, as bshlm1 is used for longer distances
#endif


/* gmb2.c version 1.0                 # page 1    1 Mar 1993 11:18 */
/* -- Function 1 -----------------------------------------------------------------*/
int                 gmb2 (
double                     a,
double                     f,
double                     lat1,
double                     lat2,
double                     lng1,
double                     lng2,
double                    *az1,
double                    *az2,
double                    *s
);

#if !0xD0C
The function gmb2 solves the inverse (or second) geodetic
main problem using the gaussian mid-latitude formulae.
Reference: JORDAN-EGGERT-KNEISSL: Handbuch der Vermessungs-
kunde Vol. IV, MCMLIX, p. 968-969. Eq. (36), (37), and (38).

gmb2         (call \& return)     int
The result of the call:
    0 => ok
    1 => not ok, line > 0.01*a (64 km) or abs(lat) > 86 deg.

a            (call)              double
The semi major axis of the ellipsoid in units of metres.

f            (call)              double
The flattening of the ellipsoid. The flattening or its inverse
may be used freely.

lat1         (call)              double
The latitude of stn1 in radians.

lat2         (call)              double
The latitude of stn2 in radians.

lng1         (call)              double
The longitude of stn1 in radians.

lng2         (call)              double
The longitude of stn2 in radians.

*az1        (return)            double
The azimuth at station 1 towards station 2 in radians in the
interval -pi, +pi.

*az2        (return)            double
The azimuth at station 2 towards station 1 in radians in the
interval -pi, +pi.

*s          (return)            double
The length of the geodesic between the stations in units of
metres.

Prog: Knud Poder NOV 1990 after older algol version by
Frede Madsen 1981 , where 5th order terms were included.
5th order terms removed, as bshlm2 is used for long lines.

#endif



/* grid_val                      # page 1   26 Oct 2001 11 37 */

/* -- Function 1 -----------------------------------------------------------------*/
int                gtc (
union geo_lab             *g_lab,
int                       direct,
double                    B,
double                    L,
double                    H,
double                    *X,
double                    *Y,
double                    *Z,
char                      *usertxt,
FILE                      *tr_error
);

#if !0xD0C
 Function:
Dual autochecking transformation function for geographic
coordinates to cartesian coordinates or reverse. The cartesian
system is righthanded with z-axis towards the mean pole and
x-axis intersecting the Greenwich meridian. The geographic
coordinates are latitude (B), longitude (L) and ellipsoidal
height (H).

ctg       (return)         int, transf. fnc no. = 2.
Return value
        0    => ok
        1<<2 => tolerance exceeded.
        -1   => severe error, gtc cannot work.

g_lab       (call)           union geo_lab
The geo_lab union must contain a a coordinate label. Only the
type ( == 1), the init (== 1), the semi major-axis and the
flattening are used, but the imit-mask may be necessary.

direct      (call)           int
The direction of the transformation:

    direct > 0  => geo->crt
    direct < 0  => crt->geo
    direct == 0 => alarm: return(UNDEFSYS)

B, L, H,     (call)           double
Input lat, lng, hgt, or X, Y, Z. Lat and lng in radians,
all others in metres.

*X, *Y, *Z   (return)         double
Output X, Y, Z or lat, lng, hgt. Lat and lng in radians,
all others in metres.

tr_cnt       (call)           int
Control of error actions for tolerance exceeded. The tolerance
is 0.5 micro arcsec, i.e. 15 microns on the earth.
     tr_cnt = 0  => alarm: return(UNDEFSYS)
     tr_cnt = 1  => error mess on stderr, return of 1<<2.
     tr_cnt > 1  => no    mess          , return of 1<<2;

Algol version (ctg) by RF, august 1981.
C-version KP JAN 1991.
#endif



/* h_trans  ver 2004.01          # page 1    24 Jan 2004 11 57 */
/* -- Function 1 -----------------------------------------------------------------*/
int        hlm_tr (
double         *f_t,
int             direct,
double          N_in,
double          E_in,
double         *Nout,
double         *Eout,
char           *usertxt,
FILE           *tr_error
);

#if !0xD0C
hlmtr       simple Helmert transformation in 2 dim.
Dual autochecking transformation, tol 30 microns.

direct > 0  => direct transformation
direct < 0  => inverse transformation
direct == 0  => identical transformation

tr_contr == 1  => alarm output to tr_error
tr_contr == 0  => exit

Prog: Knud Poder, MAR 1995


Transformation items
____________________
N0_in =  f_t[0]
E0_in =  f_t[1]
N0out =  f_t[2]
E0out =  f_t[3]
a     =  f_t[4]
b     =  f_t[5]
D     =  a*a + b*b
A     =  a/D
B     = -b/D

forward:
Nout  =  N0out + a*(N_in - N0_in) - b*(E_in - E0_in)
Eout  =  E0out + b*(N_in - N0_in) + a*(E_in - E0_in)

backward:
Nout  =  N0_in + A*(N_in - N0out) - B*(E_in - E0out)
Eout  =  E0_in + B*(N_in - N0out) - A*(E_in - E0out)

forward check:
N  =  N0_in + A*(Nout - N0out) - B*(Eout - E0out)
E  =  E0_in + B*(Nout - N0out) - A*(Eout - E0out)

backward check:
N  =  N0out + a*(Nout - N0_in) - b*(Eout - E0_in)
E  =  E0out + b*(Nout - N0_in) + a*(Eout - E0_in)
#endif





/* htr_init  ver 2010            # page 1    24 Jul 2010 11 57 */

/* -- Function 1 -----------------------------------------------------------------*/
int             inpoly (
double                       *pol_base,
double                       *Nc,
double                       *Ec
);

#if !0xD0C
 C function : inpoly

 inpoly        (call \& return)         int
               inpoly > 0   (Nc, Ec inside polygon )
               inpoly = 0   (Nc, Ec on polygon line)
               inpoly < 0   (Nc, Ec outside polygon)

 *pol_base     (call)                  * double
               pol_base[0] = number of corners in polygon
               pol_base[1] = 1. crd to 1. point
               pol_base[2] = 2. crd to 1. point
               pol_base[3] = 1. crd to 2. point
               ................................
               (Northing Easting etc.)

 *Nc           (call)                  * double
               1. coordinate for testing point in polygon

 *Ec           (call)                  * double
               2. coordinate for testing point in polygon

Prog.: F. Steffensen MAR. 1997 (MI/ITG)
#endif




/* itrf_trans  ver 2007.02          # page 1    10 Oct 2008 11 57 */

/* -- Function 1 -----------------------------------------------------------------*/
double  sand_afst (
char          *datum,
double         afst,
double         E_middel
);

#if !0xD0C
 sand_afst omregner afstanden givet ved koordinater til
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
#endif



/* sand_areal                       # page 1    2 Feb 2009 12 54 */
/* -- Function 1 -----------------------------------------------------------------*/
double   sand_areal (
char           *datum,
double         *pol_base
);

#if !0xD0C
 sand_areal omregner areal givet ved koordinater til
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
#endif


/* setDtrc.c                        # page 1   10 Aug 1994 11 40 */
/* -- Function 1 -----------------------------------------------------------------*/
int      set_dtm (
short            d_no,
char            *d_nm,
short           *p_no,
char            *p_nm,
char            *e_nm,
struct dsh_str  *trp
);

#if !0xD0C
Function and Parameters:

set_dtm produces the constants and datum information needed for
a datum shift by means of a linear transformation with 3-7 para-
meters.

set_dtm            (call and return)          int
The internal datum number is returned as the result. The result is
> 0 in case of succes and -1 in case of failure. conv_lab accepts
an ellipsoid name as a datum name, but the datum number returned
in the label is then always -2. The handling of ellipsoid names as
datum names is not done by set_dtm

d_no               (call)                     short
The internal datum number or -1. If called with an internal (and
existing) number, the datum name is returned in d_nm, but no other
parameters are returned. The call value -1 is used as a signal for
calling by means of d_nm, the datum name.

*d_nm             (return or call)            char
The datum name returned after a call with an internal datum number,
or used for calling with d_no = -1.

*p_no             (return)                    short
The internal datum number of the parent datum. The parent datum is
the datum, which can be used for datum shifts to/from the actual
datum.

*p_nm             (return)                    char
The name of the parent datum.

*e_nm             (return)                    char
The name of the ellipsoid of the datum.

*trp              (return)                   struct dsh_con
The structure containing the datum shift parameters.
See geo_lab.h.

Prog: Knud Poder 1990, 1991, 1992, 1994
Modified, KE JUL 2007, KE JAN 2010
#endif


/* set_dtm     ver 2010.1          # page 1   12 jan 2010 10 19 */
/* -- Function 1 -----------------------------------------------------------------*/
int      set_dtm_1 (
short            d_no,
char            *d_nm,
short           *p_no,
char            *p_nm,
char            *e_nm,
char            *rgn_nm,
short           *mask,
struct dsh_str  *trp
);

#if !0xD0C
Function and Parameters:

set_dtm produces the constants and datum information needed for
a datum shift by means of a linear transformation with 3-7 para-
meters.

set_dtm            (call and return)          int
The internal datum number is returned as the result. The result is
> 0 in case of succes and -1 in case of failure. conv_lab accepts
an ellipsoid name as a datum name, but the datum number returned
in the label is then always -2. The handling of ellipsoid names as
datum names is not done by set_dtm

d_no               (call)                     short
The internal datum number or -1. If called with an internal (and
existing) number, the datum name is returned in d_nm, but no other
parameters are returned. The call value -1 is used as a signal for
calling by means of d_nm, the datum name.

*d_nm             (return or call)            char
The datum name returned after a call with an internal datum number,
or used for calling with d_no = -1.

*p_no             (return)                    short
The internal datum number of the parent datum. The parent datum is
the datum, which can be used for datum shifts to/from the actual
datum.

*p_nm             (return)                    char
The name of the parent datum.

*e_nm             (return)                    char
The name of the ellipsoid of the datum.

*rgn_nm           (return)                    char
The name of the region of the datum.

*mask             (return)                    short
The IMIT mask to special hgt transformations.

*trp              (return)                   struct dsh_con
The structure containing the datum shift parameters.
See geo_lab.h.

Prog: Knud Poder 1990, 1991, 1992, 1994
Modified, KE JUL 2007, KE JAN 2010, simlk june 2012.
#endif



/* set_grs.h version 2009.1         # page 1   39 sep 2009 18 25 */
/* -- Function 1 -----------------------------------------------------------------*/
double            sftr (
double            C,
double            a,
double            b,
double            *A,
double            *B
);

#if !0xD0C
 Function:
Determination of a side and two adjacent angles in a spherical
triangle, where an angle and two adjacent sides are known. All
angles and sides are in radians.

sftr            call and return            double
The side c opposing the known angle C.

C               call                       double
The known angle.
a, b            call                       double
The two known sides, adjacent to C.

*A, *B          return                     double
The angles A (opposing a) and B (opposing b)

      ............. c .............
        .A                    B .
          .                   .
            .               .
              b           a
                .       .
                  . C .
                    .

PROG: KP 1989

#endif



/* sgetcw.h version 1.0               # page 1   14 Jun 1994 13 18 */
/* -- MODULE PHYG ------------------------------------------- */
#if !0xD0C
%\summary{TRLIB Physical Geodesy and Heights}
%\author{Karsten Engsager and Knud Poder}
%\date{1977--2013}
#endif
/* -- Function 2 -----------------------------------------------------------------*/
int               h_trans (
union geo_lab           *i_lab,
short                    oh_dtm,
double                   B_in,
double                   L_in,
double                  *dh,
char                    *tr_str
);


#if !0xD0C
 h_trans  gives the increment dh for transformations of heights
p.t. only : from h_dvr90 to h_evrf00 ::

             h_evrf00 = h_dvr90 + dh

i_lab must be a label describing the coordinate and height system
      The coordinate must be the requested system for the dh calc.
      the height system must be the input height datuma and
      correspond to one of the requested height systems.

B_in  the northing coordinate
L_in  the easting coordinate

dh    the pointer for the result

tr_error a File pointer to error text.

SYSTEMS :
h_dvr90 -> h_evrf00: geo_euref89

Programmer: Karsten Engsager, KMS, 2004.
#endif



/* hlm_tr.h                         # page 1   03 Mar 1995 13 14 */


/* -- Function 2 -----------------------------------------------------------------*/
int               htr_init (
struct  coord_lab       *ih_dtm,
struct  coord_lab       *oh_dtm,
union  geo_lab          *htr_lab,
struct htr_c_str        *htr_const,
char                    *dh_table_name,
char                    *dh_tr_info
);

#if !0xD0C
Initiates the parameters for height transformation between
Vertical reference frames.

htr_init = 1   : transformation by DHTAB
htr_init = 2   : add a1 (inv : sub a1)
htr_init = 3   : linear transformation
  errors::
            0  : NO transf
           -1  : def_lab.txt NOT FOUND/SYYNTAX ERROR
           -2  : def_lab.txt H_TR definition NOT FOUND
   TRF_ILLEG_  : in_lab error
  HTRF_ILLEG_  : h_transformation illegal

ih_dtm         : from datum
oh_dtm         :   to datum
*htr_lab       : label defining the coordsys for transformation
*inv           : Direct formula or inverse
*htr_const     ::
               inv    : Indirect or direct i.e. subtract/add
               LAT0   : Ref Latitude
               LON0   : Ref Longitude
               M0     : Curvature in Lat
               N0     : Curvature in Lon
               a1     : add konstant
               a2     : factor in Lat
               a3     : factor in Lon
*dh_table_name :  DHTAB name
*dh_tr_info    :  Ref. or description

#endif



/* i_tabdir_file  ver 2010.01          # page 1    15 Feb 2010 15 35 */


/* -- Function 2 -----------------------------------------------------------------*/
int           pla_adm_f (
char                 mode,
FILE                *i_fd,
FILE                *d_fd,
FILE                *o_fd,
char                *err_txt
);

#if !0xD0C
 pla_adm_f is a maintenance tool for ITRF plates boundaries
 the plate boundaries are used to decide which plate is
 appropriate for a point in question

 CALL  ::
 (int) pla_adm_f(
   char  mode,      mode = 'b' / 't' / 'a' / 'l'
   FILE *i_file     input file_pointer
   FILE *d_file     input file_pointer (loop directives)
                    only used in mode == 'l'
   FILE *o_file     output file_pointer
   char *errtx)     string for error

 mode == 'b'
 produces a binary file o_name from the file i_name
   from the file i_name

 mode == 't'
 produces a text in the file o_name
   from the binary file i_name

 mode == 'a'
 converts an arc (txt) file i_name
   to poly lines Geodisp format (txt) in o_name

 mode == 'l'
 from the txt file d_name is read the loops directives
 for lines to be selected from the (txt) file i_name
   to poly loops (txt) in o_name


 progr.:  Karsten Engsager, feb. 2003
#endif



/* pom.c: based on code from the boogie-project, originally written by Thomas Knudsen */
/* XXX TODO: include pom.h instead */



/* prn_cho.h                        # page 1   14 Sep 1994 16 33 */
/* -- Function 2 -----------------------------------------------------------------*/
int          set_grs (
int          ell_nmb,
char         *ellname,
double       *e,
...
);

#if !0xD0C
Function:

    set_grs initializes the parameters of the reference ellipsoid
as a function of the string ellname. The names and KMS-numbers
of the ellipsoids are shown in the macro definitions above.

    The original definitions are used for each ellipsoid, but for
the older ellipsoids (e.g. Hayford) the equator gravity value and
the standard rotation velocity is used to enable a full definition.

Call and parameters:

set_grs   (integer, return value).
The actual number of the initialized ellipsoid. The numbers can
be found by running grs_doc. NBNBNB any call will initialize the
ellipsoid, even the ?, \#, and ! without name and number, which
gives the number -1 and puts e[0] = 0.0. This is also the result
from an unsuccesfull call, reported as "mode 0 is illegal" on stderr.

ellname  (string, call value).
The name of the ellipsoid.  The number of the ellipsoid in
character representation can also be used.

e        (double e[9],  (call and) return value).
e[0] = a;    semi-major axis of the reference ellipsoid
e[1] = f;    flattening (a-b)/a, a/(a-b) can be used in a contingent
             call, but (a-b)/a is returned also in this case.
e[2] = J2;   dynamical form factor
e[3] = GM;   gravitational constant * mass
e[4] = omg;  angular velocity [radians/sec]
e[5] = U0;   normal potential at the ellipsoid
e[6] = geq;  normal gravity at equator
e[7] = ksf;  k from Somigliana's formula
e[8] = Q;    The meridian quadrant.

Prog: Maja Boserup, Knud Poder May-Oct 1990
Updated: Knud Poder NOV 1992
Updated: Knud Poder Sep 2009

User-defined ellipsoid:

    The user may define an ellipsoid during the execution of a pro-
gramme. This definition is valid until the termination of the pro-
gramme, or until the same programme produces a new definition.

The definition is carried out by calling set_grs with the ellip-
soid name string

                   \#<mode><NAME>

where mode and the necessary parameters in the call are:
     mode    e[0]   e[1]   e[2]   e[3]   e[4]   e[5]   e[6]
      1      a             J2     GM    omega
      2      a      f                   omega          geq
      3      a      f             GM    omega
      4    mean dg  f             GM    omeg

The defining constants according to the wanted mode are assigned
to the elements of e according to the table. For GM, geq and
omega any negative number will signal that the standard values are
used, viz. GM from GRS80 or geq from Hayford 1924 as required for
the mode, and the standard value of omega for the earth, unless
the user assigns a new value. All values of e are available after
the call, except for the special info-calls, where e[0] = 0.0 and
set_grs returns -1. <NAME> may be empty, resulting in the ellipsoid
name  "Special".

Documentation may be obtained by calls with ellname

     !<NAME or NUMBER>   =>  Ellipsoid name, and KMS-number
     ?<NAME or NUMBER>   =>  Ell. name, nmb, and constants
     *<NAME or NUMBER>   =>  Ell. name, nmb, and const. for mainten.

Calling with ?, \#, or ! only produces information for all
ellipsoids. The programme grs_doc may be used for online docu-
mentation. Use <CR> after prog prompt "grs_doc" for instruction.

#endif

/* set_itrf_c  ver 2009.02      # page 1    12 aug 2009 17 23 */

/* -- Function 2 -----------------------------------------------------------------*/
int            set_itrf_c (
struct coord_lab        *from_lb,
struct coord_lab        *to_lb,
double                  *i_JD,
int                      stn_vel,
int                     *z_o_vel,
struct gps_str          *gps_table,  /* crt_trf parameters */
char                    *tab_t,      /* nnr_plate_vel name */
struct plm_lab          *plm_lb,
struct plate_info       *pl_inf,     /* for plate boundary adm */
char                    *tab_i,      /* intra_plate_vel name */
struct mtab3d_str       *ipl_table,
union geo_lab           *Hipl_lab,
char                    *s_used_plm_nam,
char                    *s_used_ipl_nam,
char                    *err_str
);

#if !0xD0C

   set_itrf_c initializes the transformations constants to i_date.
  returns: max_seq: number of trans sequences in gps_table
  *from_lb is the from_coord_mini_label and maybe i_JD
  *to_lb   is the   to_coord_mini_label and o_JD

  Return : set_itrf_c >= 1         : ok.
  ERROR :
  set_itrf_c == TRF_ILLEG_ : illegal National ETRF89 and EUREF89 combi
  set_itrf_c == PLATE_NON_ : itrf_plat.bin file not found.
  set_itrf_c == PLATE_OUT_ : NO entries found.
  set_itrf_c == ITRF_NON_  : NO entries found.
  set_itrf_c == ITRF_SEQ_  : entries are NOT consequtive.
  set_itrf_c == ITRF_DTM_  : Illegal datum in from_lb/to_lb.
  set_itrf_c == ITRF_NAM_  : manager.gps table_name (gpstab) not found.
  set_itrf_c == ITRF_SYS_  : manager.gps file not found.

  i_JD       input coord epoch
  stn_vel    stn_velocities given after coordinates
  *z_o_vel   shows that o_vel should be set zero (*z_o_vel > 0)

  gps_table->
          seq[0:3]: trf_nr[i] :
  idt_tr  :0:  identical
  itrf_tr :1:  itrfyy -> itrfxx  ( + maybe itrfgate->igs_gate)
  igs__tr :2:  igs_yy -> igs_xx  ( + maybe igs_gate->itrfgate)
  igatetr :3:  itrfyy -> etrfyy or itrfgate -> nkg_euref89
  ogatetr :4:  itrfyy -> etrfyy or itrfgate -> nkg_euref89
  i_ipl_dt  :  ref_date for intra plate velocity
  o_ipl_dt  :  ref_date for intra plate velocity

  *tab_t    :: nnr_plate_vel name
  *plm_lb   :: label for tab_t
  *tab_i    :: intra_plate_vel name
  *ipl_table:: table and label for tab_i
  *req_plm_tr :: 0: none, +1: requested
  *req_ipl_tr :: 0: none, >0: requested in seq[i]==*req_ipl_tr
  *plm_dd   :: transformation period
  *ipl_dd   :: transformation period
  *s_used_plm_nam :: plm name saved
  *s_used_plm_nam :: ipl name saved
  *err_str  :: error info is placed in this string

  region: (from_lb/to_lb)->rgn  and ETRF89 ::
          {DK/NO/SF/SV/GR} : National ETRF89
          EU : the EUREF recommandations are used
       else ITRF PLATE and INTRA PLATE MODELS are used

  Transformation sequence
  I_GATE_TR  ITRF_TR   IGS_TR   ITRF_TR  O_GATE_TR
          0        1        2         3          4

  I_GATE_TR may be ::
            gr96 gate  to ITRF_gate                     i_zz > -1
      ngk_etrf89 gate  to ITRF_gate                  inkg_ii  1
          etrfyy gate  to ITRFyy                     ieur_ii  1
           wgs84 gate  to ITRF_gate                     i_zz > -1

  O_GATE_TR may be ::
            gr96 gate  to ITRF_gate                     o_zz > -1
      ngk_etrf89 gate  to ITRF_gate                  onkg_ii  1
          etrfyy gate  to ITRFyy                     oeur_ii  1
           wgs84 gate  to ITRF_gate                     o_zz > -1

                        yy <= xx
  ITRF_TR ::
  itrfyy  itrfxx  : add itrf entries from yy to xx  : itrf_ii 1

  IGS_TR ::
  igsyy    igsxx  : add igs  entries from yy to xx  : igs_ii  1

  itrfgate igsgate :                                :   g_zz > -1
  and reverse  . . .

  igsyy/itrfyy -> wgs84 is done to TRANSIT DOPPLER definition
  gr96/etrf89  -> wgs84 is done to itrfmax (latest realisation)
  and reverse  . . .
#endif

/* set_tpd.h                        # page 1   12 Jan 2010 13 14 */
/* -- Function 2 -----------------------------------------------------------------*/
int                srch_plate (
struct plate_info        *pl_inf,
double                    B,
double                    L,
struct plm_lab           *plm_tab,
struct gps_c_str         *c_gps,
char                     *err_txt
);

#if !0xD0C

 search plate containing coordinates  B, L,
 return values ::
       >=  0   :: plate number
          -1   :: NO plate found
          -2   :: i/o error on binary model

 the platenumber is used to find the plate_information in the
 struct plate_info pl_inf

 progr. K. Engsager. Feb. 2003, 2008
#endif

/* srch_tab_typ     ver 2010.1        # page 1   18 Feb 2010 10 19 */

/* -- MODULE UTIL ------------------------------------------- */
#if !0xD0C
%\summary{TRLIB Internal Plumbing}
%\author{Karsten Engsager and Knud Poder}
%\date{1977--2013}
#endif
/* -- Function 3 -----------------------------------------------------------------*/
extern double     clenshaw (
int               M,
double           *a,
int               g,
double            arg_r,
double            arg_i,
double           *R,
double           *I);

#if !0xD0C
a:           coeffs of the series, decl. a[8] at least.
g:           The number of coefficients in the series.
argr, argi:  The real and imaginary part of the argument of cosine.
R, I:        The real and imaginary part of the sum.

clenshaw:
Computes the sum of a series A[j-1]*F(j*argr + i*j*argi)
(where i is the imaginary unit) by Clenshaw summation
from g down to zero. The coefficients are here real and
the argument of F is complex. The real part of the sum
is the value of the function.

The function finds in complex arithmetic an array h
so that h(j) := A(j) + 2*G(argr+i*argi)*h(j+1) - h(j+2)
with h(g+1) = h(g+2) = 0.

Given M the A[j], S and the functions F and G are :
-------------------------------------------------
      C :   a[j], 1         Complex_cos Complex_cos
      S :   a[j], 1         Complex_sin Complex_sin
      G : a[j+1], 0   Complex_chebyshev arg_r+j*argi
      c :   a[j], 1              cos    cos
      s :   a[j], 1              sin    sin
      g : a[j+1], 0           Chebyshev arg_r

Given M the sum of the series is :
--------------------------------
      C :      -h(1)+h(0)*cos(argr+i*argi)
      S :            h(0)*sin(argr+i*argi)
      G :      -h(1)*(argr+i*argi)+h(0)
      c :      -h(1)+h(0)*cos(argr)
      s :            h(0)*sin(argr)
      g :      -h(1)*argr+h(0)

Prog.: K. Engsager, MAY 1994
#endif


/* conv_cnd  ver 2010.01            # page 1   12 jan 2010 15 35 */

/* -- Function 3 -----------------------------------------------------------------*/
double    cwfJD (
char             mode,
double           JD,
int              zone,
int              region
);

#if !0xD0C

cwfJD        Julian Date (n) converted to local time in cw
             in timezone +1 is corrected for danish summertime
             G.I. date_time
             199204170945.0  == 1992 Apr 17,  9.45

mode == 'n': n = JD counted from year 2000.0 (=JD= 2451545.0)
mode == 'J': JD

JD           JD according to mode true/(from 2000.0)

zone         timezone  :
_               0      : UK
_              +1      : DK

region       :  0      : no correction for summertime
_            : else    : include correction for summertime
_                        Programmed for :
_            :  DK     : Denmark.

REF.: Fliegel, H.F. and van Flandern, T.C. (1968).
      Communications of the ACM, Vol. 11, No. 10 (October, 1968).

Progr.: K.Engsager, Feb. 2004.
#endif



/* cw to JD, n and GMT              # page 1    1 Mar 1993 11:18 */
/* -- Function 3 -----------------------------------------------------------------*/
double    cwtJD (
double           cw,
double          *n,
double          *GMT,
int              zone,
int              region
);

#if !0xD0C

cwtJD        Julian Date converted from local time in cw
             in timezone +1 is corrected for danish summertime

cw           G.I. date_time
             199204170945.0  == 1992 Apr 17,  9.45

*n           days from JD 2000.0

*GMT         fraction of day from 0 hour UT.

zone         timezone  :
_               0      : UK
_              +1      : DK

region       :  0      : no correction for summertime
_            : else    : include correction for summertime
_                        Programmed for :
_            :  DK     : Denmark.

#endif

/* dk_trans.h version 1.0            # page 1   08 Mar 1995 13 02 */
/* -- Function 3 -----------------------------------------------------------------*/
int               s_status (
char                    *err_str,
char                    *fnc_name,
int                      err_type,
...
);
#if !0xD0C

 ADDITIONAL PARAMETERS
char                    *c_unit;
char                    *h_unit;
double                    N,  E,  H;
double                   dN, dE, dH;

SPECIAL::  c_unit == "non"  ==>>  SKIPS add params writing


  s_status: alarm function for transformations and predict-
  dicates a programme error.

  tr_error: FILE for alarm texts. Opened by the user, std-
  out or stderr may be used.

  usertxt: User-def. additional alarm text (e.g. a st_nmb)

  fnc_name: The name of the transformation function

 err_type: the type of error (1.group) + (2.group) + (3.group)
 1.group   ::  TRANSFORMATION of CRD
 TRF_INACC_ =>  Boundary exceeded (inacurate result) 5 add params
               (may still be usefull for mapping)
 TRF_TOLLE_ =>  Tolerance exceeded                   8 add params
 TRF_AREA_  => Boundary exceeded (serious)           5 add params
                (result is undefined)
 TRF_GEOID_ => NO GEOID ILLEGAL TRANSFORMATION
 TRF_ILLEG_ =>  Illegal transformation
 TRF_PROGR_ =>  System/Prog fault
 2.group   ::  GET GRID_VALUE
 TAB_C_ARE_ => Table Boundary exceeded (serious)
 TAB_C_PRJ_ => TABLE called with wrong coord label
 TAB_C_NON_ => manager.tab table_type not found
 TAB_G_MAX_ => Table too many tables to be initilized
 TAB_N_NAM_ => manager.tab table_name not found
 TAB_R_MAN_ => manager.tab region not found
 TAB_N_MAN_ => manager.tab not found
 TAB_N_SYS_ => manager.tab not found
 3.group   ::  GNNS transformation errors
 PLATE_NON_ => itrf_plat.bin NOT FOUND
 PLATE_OUT_ => station out of PLATES
 ITRF_NON_  => ITRFyy NONE FOUND
 ITRF_SEQ_  => ITRF NOT IN SEQUENCE in manager
 ITRF_DTM_  => DATUM NOT FOUND
 ITRF_MAX_  => ITRF TOO MANY
 ITRF_NAM_  => manger.gps NOT FOUND
 ITRF_SYS_  => ITRF SYSTEM FAULT
 PLATE_NO_VEL_ => PLATE has no velocity (warning)

 Any other type number gives -1 by default

 ... : parameters required as concluded from label and type

  c_unit: "sx" or "m ". Used for units of N and E
          "non" skips the parameter wtiting
  h_unit: "m " or "". "" means no heights

 Types 1,2 :   N,  E, (and  H if appropriate)
 Type  1:     dN, dE, (and dH if appropriate) in units of
 metres, from the reverse check transformation

 Example 1: s_status(err_str, "ptg", 1, "sx", "", N, E, dN, dE);

  Prog: Knud Poder  30 SEP 1995
  Updated: Karsten Engsager jan 2009
#endif



/* sand_afst                       # page 1    2 Feb 2009 12 54 */
/* -- Function 3 -----------------------------------------------------------------*/
int               t_status (
FILE                    *tr_error,
char                    *usertxt,
char                    *fnc_name,
int                      err_type,
...
);

#if !0xD0C
 ADDITIONAL PARAMETERS
char                    *c_unit;
char                    *h_unit;
double                    N,  E,  H;
double                   dN, dE, dH;


  t_status: alarm function for transformations and predict-
  dicates a programme error.

  tr_error: FILE for alarm texts. Opened by the user, std-
  out or stderr may be used.

  usertxt: User-def. additional alarm text (e.g. a st_nmb)

  fnc_name: The name of the transformation function

 err_type: the type of error
 1 =>  Tolerance exceeded
 2 =>  Boundary exceeded (serious)   (result is undefined)
 3 =>  System/Prog fault
 4 =>  Illegal transformation
 5 =>  Boundary exceeded (inacurate result)  (for mapping)
 Any other type number gives -1 by default

 ... : parameters required as concluded from label and type

  c_unit: "sx" or "m ". Used for units of N and E
  h_unit: "m " or "". "" means no heights

 Types 1,2 :   N,  E, (and  H if appropriate)
 Type  1:     dN, dE, (and dH if appropriate) in units of
 metres, from the reverse check transformation

 Example 1: t_status(stdout, st_text, "ptg", 1,
                "sx", "", N, E, dN, dE);

  Prog: Knud Poder  30 SEP 1995
#endif


/* tab3d_c                         # page 1   25 Jun 2004 17 25 */
/* -- Function 3 -----------------------------------------------------------------*/
double      v_red (double v);

#if !0xD0C
 Function:
v_red modifies the angle to be numerically less or equal pi.

v_red  an angle in the interval -pi to pi in radians.

v      The angle in radians to be reduced.

Prog KP, 1989

XXX Note: we could reimplement using math.h::fmod() (thokn, 2013-04-17)
#endif



/* v_std.h                        # page 1   22 Feb 1993 12 54 */

/* -- Function 3 -----------------------------------------------------------------*/
double      v_std (double  v);

#if !0xD0C
 Function:
v_std modifies the angle to be zero or positive less than 2pi.

v_std  an angle in the interval 0.0 to 2pi in radians.

v      The angle in radians to be reduced.

Prog KP, 1989

XXX Note: we could reimplement using math.h::fmod() (thokn, 2013-04-17)
#endif
























/* -- MODULE GENE ------------------------------------------- */
#if !0xD0C
%\summary{TRLIB Generic Transformations}
%\author{Karsten Engsager and Knud Poder}
%\date{1977--2013}
#endif
/* -- Function 4 -----------------------------------------------------------------*/
int                etrs89_trans (
char                     *i_lab_str,
char                     *o_lab_str,
double                    N_in,
double                    E_in,
double                   *X,
double                   *Y,
FILE                     *tr_error
);
#if !0xD0C

   etrs89_trans is a tiny version of gd_trans having
   transformations between projections in etrs89 Datum ONLY
   crt and ellipsoidal heights are not included
   leagal i_lab_str and o_lab_str are
   (zz is two digits for zone):
     "geo", "utmzz", "dktm1", "dktm2", "dktm3", "dktm4",
     "kp2000j", "kp2000s", "kp2000b", "kfmr", "fke",
     "etrs-tmzz", "ETRS-TMzz", "etrs-lcc", "ETRS-LCC",
     "etrs-laea", "ETRS-LAEA"

   The return values are:
     0           => ok
     TRF_INACC_  => boundary exceeded (inaccurate)
     TRF_TOLLE_  => transformation tollerance exceeded
     TRF_AREA_   => boundary exceeded (serious)
     TRF_ILLEG_  => illegal transformation

     in case of error an error text is returned in err_txt,
     which should be declared with a length bigger than 250

     NB: the values TRF_*_ and TAB_*_ are defined in geo_lab.h in
     decreasing order.
     For mapping purposes it may then be admissible to accept
     the result bigger or equal to TRF_INACC_ (or even TRF_TOLLE_).

     Parameters:
     char *i_lab_str, *o_lab_str : string from the list above
           describing the input and output.
     double N, E: The input coordinates:
       Geographical     latitude, longitude (unit: radians).
       Projection       Northing, Easting (unit: meter).
     double *X, *Y: The input coordinates:
       Geographical     latitude, longitude (unit: radians).
       Projection       Northing, Easting (unit: meter).

  Prog: Karsten Engsager 2009.
#endif


/* f_next.h                                  * page   */

/* -- Function 4 -----------------------------------------------------------------*/
int                 gd_trans (
union geo_lab            *i_lab,
union geo_lab            *o_lab,
double                    N_in,
double                    E_in,
double                    H_in,
double                   *X,
double                   *Y,
double                   *Z,
double                   *gh,
char                      req_gh,
struct mgde_str          *grid_tab,
char                     *usertxt,
FILE                     *tr_error
);


#if !0xD0C
 Function:

gd_trans              (call \& return)              int

Transforms and/or predicts between (allmost) all systems described by
csysdoc or the instructions following an unsuccesfull call of kmstr.
Senseless transformations as e.g. a local Danish system to/from a
Greenland conical system are not tested for, but will be trapped by
the checks in the activated transformation functions.

The return values are:

                 0 => ok
or ER_GP1 + ER_GP2 < 0 => ERROR

ER_GP1 (-1 to -99):
TRF_INACC_  => boundary exceeded (inaccurate)
TRF_TOLLE_  => transformation tollerance exceeded
TRF_AREA_   => boundary exceeded (serious)
TRF_GEOID_  => no geoid cause illegal transformation
TRF_ILLEG_  => illegal transformation
TRF_ILLEGH_ => illegal height transformation
TRF_PROGR_  => programe/system fault

ER_GP2 (-100 to -9900):
TAB_C_ARE_  => table area out (boundary exceeded)
TAB_C_PRJ_  => table called with wrong label
TAB_C_NON_  => table none found
TAB_G_MAX_  => too many tables in list
TAB_N_NAM_  => table name not found in manager
TAB_R_MAN_  => manager region not found
TAB_N_MAN_  => manager not found
TAB_N_SYS_  => manager system fault

ER_GP3 (-10000 to -990000):
PLATE_NON_  => PLATE not found
PLATE_OUT_  => PLATE station out of area (boundary exceeded)
ITRF_NON_   => ITRFyy not found
ITRF_SEQ_   => ITRF not in sequence in manager
ITRF_DTM_   => DTM not in {ITRF, IGS, ETRF, EUREF89}
ITRF_MAX_   => ITRF too many
ITRF_NAM_   => ITRF manager.gps not found
ITRF_SYS_   => ITRF system fault

NB: the values TRF_*_ and TAB_*_ are defined in geo_lab.h in
decreasing order.
For mapping purposes it may then be admissible to accept
the result BIGGER OR EQUAL to TRF_INACC_ (or even TRF_TOLLE_).

NB: Sofisticated used when errors in geoid height is not important:
accept results as above or when result < TRF_PROGR_
(or use req_gh = -1; see below)

The function is not dual, but as each of the individual transformation
functions activated by gd_trans are dual auto checking functions, every
transformation in the chain is checked.

Parameters:

*i_lab_u, *o_lab_u         (call \& return)          union geo_lab
Geo_labels describing the input system and the output system generated
by conv_lab.

N, E, H                    (call)                   double
The input coordinates:

       1. Cartesian        X, Y, Z   or
       2. Geographical     latitude, longitude, height   or
       3. Rectangular      y, x, height.

The height is treated :
   ellipsoidal height : used in transformation
   height             : geoid height is added and used in trf.
   no height          : geoid height is used in trf (when req_gh >= 0).
Special danish systems: transformation of heights are performed::
   from/to old_danish sytems (h_gm91, h_gi50, h_msl (small islands))
   to/from h_dvr90


In some cases the geoid height is not calculated.

The coordinates are in units of metres, except for the latitude and
longitude, which are in radians.

*X, *Y, *Z           (return)               double
The output coordinates:

       1. Cartesian        X, Y, Z   or
       2. Geographical     latitude, longitude, height   or
       3. Rectangular      y, x, height.

See N, E, H for details of special precautions and units.

*gh                  (return)               double
The geoid heigh or table value in the output coordinate system
IS calculated when req_gh != 0 ONLY

req_gh                (call)                char
_      == 0 NO output BUT *gh is calculated when needed
_       > 0    output *gh is calculated ALLWAYS
_       < 0 NO        grid table  is connected !!

*grid_tab             (call and return)    mtab struct
holds tables and names and statistics of use.
the entry : tab_nr gives the index of the gde_lab
enabling a report of the name of the grid actually used.

*usertxt               (call)                   char
A textstring supplied by the user (may be empty) to be inserted in the
alarm output. E.g. a station number may be useful for identifying the
reported error.

tr_error               (call and return)        FILE
A stream created, opened and manipulated by the calling programme.
The errors are reported here in text form. The usertxt is inserted
in the stream, so that the usertxt should be created before the call
of unitrans. The error output may be collected after each error or
after all calls of unitrans. The strategy for reading, contingent
reset and closing of the stream is left to the calling programme.

Prog: Karsten Engsager 1999.
Former versions:   Knud Poder \& Karsten Engsager.

#endif



/* gen_pol.h                        # page 1   06 Mar 1995 12 13 */
/* -- Function 4 -----------------------------------------------------------------*/
int            gen_pol (
double               *C_f,
double               *C_r,
double                N_in,
double                E_in,
double               *Nout,
double               *Eout
);

#if !0xD0C
 gen_pol:  2 general polynimiae for transformation
 C_f:      the polynomiae for forward  transform.
 C_r:      the polynomiae for reverse check transf.
 N, E:     input/output coordinates
 tr_contr: 1 => report, 0=> exit
 usertxt:  text inserted by the user's programme
 tr_error: file opened and read by the user's prog.

 NB: The internal independant variables used in the
 evaluation is relative to "center values". Values
 here exceeding 500 km give a termination.

 PROG: Knud Poder, MAR 1995
#endif




/* geoid_c                         # page 1   30 Oct 2001 17 25 */
/* -- Function 4 -----------------------------------------------------------------*/
int               itrf_trans (
union geo_lab            *i_lab,
union geo_lab            *o_lab,
int                       stn_vel,      /* velocity for stn in i_vel */
double                   *i_crd,        /* input coords:: 0:2 */
double                   *i_vel,        /* velocity to coords:: 0:2 */
double                    i_JD,         /* Julian Day from 2000.0 */
double                   *o_crd,        /* output coords:: 0:2 */
double                   *o_vel,        /* NOT calculated:: 0:2 (==0.0) */
struct PLATE             *plate_info,   /* call and return info on plates */
double                   *tr_par,       /* 7-par describing the transformation */
char                     *usertxt,
char                     *err_str
);

#if !0xD0C
 itrf_trans initialize the tables needed or closes the tables
 o_lab = NULL or i_lab = NULL closes the tables

 itrf_trans transforms i_crd[3] coordinates from:
     i_lab   i_JD  to  o_lab   o_lab->u_c_lab.JD (= o_JD)
 using the official convertions between the ITRF's and IGS
     and ETRF89 and nationsl realisations DK_ETRF89,
     NO_ETRF89, SE_ETRF89, SF_ETRF89 and gr96
 and  1) the STD plate velocities
 or   2) the named plate velocities
 or   3) no velocities             : NON
 and  1) the STD intra plate velocities
 or   2) no intra plate velocities : NON
 region: (i_lab/o_lab)->u_c_lab.rgn
         {DK/NO/SF/SV/GR} : National ETRF89
         EU : the EUREF recommandations are used
      else ITRF PLATE and INTRA PLATE MODELS are used

 usertxt: at ERROR: put first in err_str

 itrf_trans may return the values:
       0               : OK
      -1 TRF_INACC_    : inaccurate result
      -5 TRF_ILLEG_    : illegal label combination
      -6 TRF_PROGR_    : programming error in function
    -100 TRF_C_ARE_    : out of table area
  -20000 PLATE_OUT_    : No plate for position found  (TRF_PROGR_)
  -30000 ITRF_NON_     : NO entries found             (TRF_PROGR_)
  -40000 ITRF_SEQ_     : entries are NOT consequtive  (TRF_PROGR_)
  -50000 ITRF_DTM_     : Illegal datum in from_lb/to_lb
  -60000               : tr_par != NULL \&\& stn_vel !=0 : ILLEGAL
  -70000 ITRF_NAM_     : manager.gps table_name (gpstab) not found
  -90000 ITRF_SYS_     : manager.gps file not found
 -100000 PLATE_NO_VEL_ : Plate has no velocity


 tab_name strategy ::
 tab_t = plate_info->plate_model         to be used
 tab_i = plate_info->intra_plate_model   to be used
 stn_vel == 1  :  pl_tr i_JD to    o_JD i_vel
 stn_vel == 0  ::
     tab_t == nnr_std : pl_tr i_JD to o_JD STD:newest nnr_itrfYY
     tab_t == std :  pl_tr    i_JD to o_JD STD:newest nnr_itrfYY
     tab_t != 0:     pl_tr    i_JD to o_JD table tab_t vel
     tab_t == 0:     NO pl_tr
     tab_i == std :  ipl_tr   i_JD to o_JD SDT:ipl model for area
     tab_i != 0:     ipl_tr   i_JD to o_JD table tab_i vel
     tab_i == 0:     NO ipl_tr

 Exceptions:
 1) to/from NATIONAL ETRF89 : DK, NO, SF, SV::
     tab_t == {"" or "STD"} used_plm_nam == nnr_itrf00
              to:  i_JD to nkg_dt;  fr: nkg_dt to o_JD
           else the given name is used as NON STANDARD
     tab_i == {"" or "std"} used_ipl_nam == nkgrf03vel.01
              to:  i_JD to ipl_dt;  fr: ipl_dt to o_JD
           else the given name is used as NON STANDARD
 2) EUREF recommandation ETRF89 : EU ::
     tab_t == {"" or "STD"} used_plm_nam == recommended
     tab_i == {"" or "STD"} used_ipl_nam == ""
           else the given name is used as NON STANDARD

 GR96 epoch       :: 1996 08 15 :: 1996.xxx :: ITRF94
 ETRF89 NKG area DK, NO, SE, SF :: ipl_dt   ::   GATE
 ETRF89 NKG epoch :: 2003 10 01 :: 2003.750 :: ITRF2000
 DK_ETRF89  epoch :: 1994 09 15 :: 1994.707 :: ITRF93
 NO_ETRF89  epoch :: 1994 08 31 :: 1994.665 :: ITRF93
 SE_ETRF89  epoch :: 1999 07 02 :: 1999.500 :: ITRF97
 SF_ETRF89  epoch :: 1997 01 01 :: 1997.000 :: ITRF96

 RETURN PARAMETERS:
 o_crd[3] : Transformed i_crd[3]
 o_vel[3] : Transformed i_vel[3]: NOT ASSIGNED: 0.0
 plate_info-> ::
   plm_trf = 1/0: nnr plate model used/(not used)
   ipl_trf : {1,2,3} : std gates using:
                      {ipl_iJD, ipl_oJD, ipl_iJD \& ipl_oJD}
           : {4, 0} : intra plate model {used, not used}
                      (ipl_iJD \& ipl_oJD = -10000000.0)
   plm_dt   : gate Julian date for Plate Model
   ipl_idt  : Julian date of input National  (INTRA Plate)
   ipl_odt  : Julian date of output National (INTRA Plate)
   plm_nam is the name of the actual nnr plate model
   plt_nam is the name of the actual plate
   ipl_nam is the name of the actual intra plate model

 err_str: at ERROR: user_txt cat ERROR description

 tr_par == NULL gives transformations only
 tr_par != NULL \&\& stn_vel !=0 gives -6000: ILLEGAL
 tr_par != NULL \&\& stn_vel ==0
           gives transformations and 7-par values
           in tr_par: T1, T2, T3, R1, R2, R3, D
           se formula below

         TRANSFORMATION PRODUCTION LINE
         <--UP-HILL--> <--DOWN-HILL-->
 cstm:     PRJ   CRT     CRT    PRJ
 action:     GDTR    CTR    GDTR
 action:       0      1       2

 basic transformation formula (Molodensky) ::
 (XS)   (X)   (T1)   ( D  -R3  R2) (X)
 (YS) = (Y) + (T2) + ( R3  D  -R1)*(Y)
 (ZS)   (Z)   (T3)   (-R2  R1  D ) (Z)
#endif




/* -- Function 4 -----------------------------------------------------------------*/
int             ptg (
union geo_lab          *TC,
int                    direct,
double                 N,
double                 E,
double                 *Nout,
double                 *Eout,
char                   *usertxt,
FILE                   *tr_error
);

#if !0xD0C
Function

ptg              int function, fnc. no = 3-6       call
Dual autochecking transformation function for transformation
between geographical coordinates and the regular coordinate
systems
:
    Projection name           fnc (no). = cstm no.
    Gauss-Krueger (UTM)          3
    Mercator                     4
    Lambert                      5
    Stereographic                6

An alarm is produced when the check by the (internal) back-
transformation exceeds the tolerance of 0.000 001 arcsec (or
the equivalent linear amount 30 my on the earth). The function
returns
            ptg =  0              => ok
            ptg =  1 << (fnc)     => tolerance exceeded
            ptg =  UNDEFSYS (=-1) => non-applicable coord sys
            ptr =  ARAEOUT  (=-2) => outside acceptable area

N, E              double                           call
The projection- or geographical coordinates input for trans-
formation in units of metres (projection) or radians (geogr.).

Nout, Eout        double                           return
The transformed projection- or geographical coordinates output
in units of metres (projection) or radians (geogr.).

TC                struct coord_lab TC              call
The structure of transformation constants defined in the
include-file "geo_lab.h", which also describes the elements
in detail, indicating the parts to be set by the user and the
parts initialized by ptg at the first call. If more than one
single system transformation is called in a programme (or func-
tion), each must have its own TC, declared by

             struct coord_lab  *TC,

as the constants are retained until the structure member
TC->init is set = 0 (=FALSE). TC must point to a declared
structure holding the elements, eg. by TC = \& DTC. TC may be
re-used whenever TC->init  (or (*TC).init) is set = 0.

direct               int                             call
direct =  1 => transform projection -> geogr.
direct >  1 => transformation projection -> Gaussian Lat + Lng
direct = -1 => transformation geogr. -> projection
direct < -1 => transformation Gaussian Lat + LNG -> projection
direct =  0 => no transformation, input is passed as output, the
ptg gives the result -1, i.e. alarm as undefined system.

tr_cntr              int                              call
Control of the action in case of malfunction.
tr_cntr = 0  => alarm printing and abortion with return(-1)
tr_cntr = 1  => alarm printing and return result = 1 << fnc no.
tr_cntr > 1  => no alarm printing, return result = 1 << fnc no.
The alarm is printed on stderr.

External functions:
math library:
atan2, cos, sin, sinh, log, fabs
kms functions:
clencsin, gatg, t_report

The formulae are taken from R. Koenig / K.H. Weise:
Mathematische
Grundlagen der hoehren Geodaesie und Kartographie, Erster Band,
Das Erdsphaeriod und seine konformen Abbildungen.
Springer-Verlag, Berlin/Goettingen/Heidelberg 1951.

Prog:
Knud Poder 4 NOV 1977 (algol version), OCT 1989 (C-version);
AUG 1990, generalizing  from utm to 4 regular projections.
#endif


/* ptg_d      version 2010.1           # page 1   12 Jan 2010 16 51 */
/* -- Function 4 -----------------------------------------------------------------*/
int             ptg_d (
union geo_lab         *TC,
int                    direct,
double                 N,
double                 E,
double                *Nout,
double                *Eout,
char                  *usertxt,
FILE                  *tr_error,
double                *D,
double                *Dgtu
);

#if !0xD0C
Function

ptg_d            int function, fnc. no = 3-6       call
Dual autochecking transformation function for transformation
between geographical coordinates and the regular coordinate
systems
:
    Projection name           fnc (no). = cstm no.
    Gauss-Krueger (UTM)          3
    Mercator                     4
    Lambert                      5
    Stereographic                6

An alarm is produced when the check by the (internal) back-
transformation exceeds the tolerance of 0.000 001 arcsec (or
the equivalent linear amount 30 my on the earth). The function
returns
            ptg_d =  0              => ok
            ptg_d =  1 << (fnc)     => tolerance exceeded
            ptg_d =  UNDEFSYS (=-1) => non-applicable coord sys
            ptg_d =  ARAEOUT  (=-2) => outside acceptable area

N, E              double                           call
The projection- or geographical coordinates input for trans-
formation in units of metres (projection) or radians (geogr.).

Nout, Eout        double                           return
The transformed projection- or geographical coordinates output
in units of metres (projection) or radians (geogr.).

TC                struct coord_lab TC              call
The structure of transformation constants defined in the
include-file "geo_lab.h", which also describes the elements
in detail, indicating the parts to be set by the user and the
parts initialized by ptg_d at the first call. If more than one
single system transformation is called in a programme (or func-
tion), each must have its own TC.

direct               int                             call
direct =  1 => transform projection -> geogr.
direct >  1 => transformation projection -> Gaussian Lat + Lng
direct = -1 => transformation geogr. -> projection
direct < -1 => transformation Gaussian Lat + LNG -> projection
direct =  0 => no transformation, input is passed as output, the
ptg_d gives the result -1, i.e. alarm as undefined system.

The alarm is printed on tr_error.

D                 double                           return
The values m and c for transformation from regular coordinate
system to geographical.

Dgtu              double                           call
The constants for the trig series to calc 1/m and -c. (setDtrc.c)

External functions:
math library:
atan2, cos, sin, sinh, log, fabs
kms functions:
clenshaw, gatg, t_status

The formulae are taken from R. Koenig / K.H. Weise:
Mathematische Grundlagen der hoehren Geodaesie und
Kartographie, Erster Band.
Das Erdsphaeriod und seine konformen Abbildungen.
Springer-Verlag, Berlin/Goettingen/Heidelberg 1951.

Prog:
Knud Poder 4 NOV 1977 (algol version), OCT 1989 (C-version);
AUG 1990, generalizing  from utm to 4 regular projections.
AUG 1994, calculating m and c in utm projection. KE.
Karsten Engsager  12 Jan 2010. updated
#endif



/* putsinexval  ver 2008.01        # page 1    1 Dec 2008 15 35 */
/* -- Function 4 -----------------------------------------------------------------*/
int              setDtrc (
struct coord_lab         *c_lab,
double                   *DD
);

#if !0xD0C
 Function:
setDtrc             (call and return)            int
Produces the constants needed for calculation of scale and
meridan convergence (m, c) in the UTM projection, where the
coordinate system is described by a coord_lab.
An erroneous c_lab gives the return value -1.

Parameter:
*c_lab             (return)                 coord_lab
giving the ellipsoid parameters.

*DD                 (return)                    double
constants for calculation of m and c.

Prog: K. Engsager  1994.
#endif


/* set_dtm     ver 2010.1          # page 1   12 jan 2010 10 19 */
/* -- MODULE SPEC ------------------------------------------- */
#if !0xD0C
%\summary{TRLIB Specific Transformations}
%\author{Karsten Engsager and Knud Poder}
%\date{1977--2013}
#endif
/* -- Function 5 -----------------------------------------------------------------*/
int          dk_trans (
union geo_lab      *in_lab,
union geo_lab      *outlab,
double              N,
double              E,
double              H,
double             *Nout,
double             *Eout,
double             *Hout,
char               *usertxt,
FILE               *tr_error
);

#if !0xD0C

 2. dim. transformation function for area DK


*in_lab      :  from label
*out_lab     :    to label
N, E, H      :  from coordinates
*Nout, *Eout :    to coordinates
*Hout        :    to height == H
*usertxt     :  to be written in case of errors
*tr_error    :  file for error reports
             :  tr_error == NULL: NO REPORT IS PRINTED

#endif



/* dks_tr.h version.1               # page 1   17 Feb 2000 10 41 */
/* -- Function 5 -----------------------------------------------------------------*/
int              dks_tr (
char           *in_lab,
char           *outlab,
double         Nin,
double         Ein,
double         *yout,
double         *xout,
char           *usertxt,
FILE           *tr_error
);

#if !0xD0C
Function and parameters

dks_tr          int function,   fnc no. 8       call and return
Dual autochecking prediction (non-regular transform.) function for

          DKS (in ED50)     <-> System 1934 Sjælland
          DKS (in ED50)     <-> RT90 5 GON W
          DKS (in ED50)     <-> UTM Zone 33 (ED 50)

The function gives the result of the transformation as

      dks_tr = 0        => ok,
      dks_tr = -1       => inaccurate (may be used in mapping)
      dks_tr = -2       => tolerance exceeded
      dks_tr = -3       => outside the accepted area
      dks_tr = -4       => programming error

The physical precision of the transformation is generally
1-2 cm, but the numerical precision is some few mm,
increasing to some cm far from the central parts.

The accepted area is limited to dks-coordinates satisfying

      250 km <= N <= 290 km
      100 km <= E <= 140 km

in_lab, outlab  char                             call
The minilabels for the systems. The names are:

dks:    The Øresund bridge system
s34s:   System 1934 Sjælland
u33:    UTM zone 33, ED50
rt90v:  The Swedish RT90 5 Gon W.

Nin, Ein        double                           call
UTM or s34 coordinates input for transformation in metres.

yout, xout      double                           return
s34 or UTM coordinates output after transformation in metres.

The tolerance is set by a define statement to 2 mm, and may
be redefined. The chosen value is comparable to the precision
of the forward/backward computation, but the precision of the
polynomial approximations is rather 4-5 cm (mean error appr.
1.5 cm).

Prog: Knud Poder, OCT 1992
Prog: K. Engsager, OCT 1999

#endif



/* doc_def_data.h version ?.?            # page 1   25 Aug 1995 14 03 */


/* -- Function 5 -----------------------------------------------------------------*/
int         ee_trans (
union geo_lab      *in_lab_a,
union geo_lab      *outlab_a,
double              N_in,
double              E_in,
double              H_in,
double             *Nout,
double             *Eout,
double             *Hout,
char               *usertxt,
FILE               *tr_error
);

#if !0xD0C

 ee_trans   : prediction function for Estonian coord-systems
 ee_trans   : returns 0 for succes, and -1 for error
 in_lab_a   : label describing the actual input system
 outlab_a   : label descrining the actual output system
 N_in, E_in, H_in : input coordinates in units of metres
 Nout, Eout, Hout : output coordinates in units of metres
                  : Hout == H_in
 usertxt    : info text from the user, output->tr_error
 tr_error   : txtfile openend, read and closer by the user

 PROG: Knud Poder, SEP 1995
#endif







/* eetmtu_g.h                       # page 1   29 Sep 1995 16 30 */


/* Definition of the permissible area for the transformation */

/* -- Function 5 -----------------------------------------------------------------*/
int               eetmtu_g (
int                      direct,
double                   N_in,
double                   E_in,
double                  *N_out,
double                  *E_out,
char                    *usertxt,
FILE                    *tr_error
);

#if !0xD0C

  eetmtu_g:   Autochecking dual prediction function for pre-
  diction  between Gauss-Krueger coordinates  with central
  longitude 27 degrees on Pulkovo 1942 datum  and UTM zone
  32 coordinates on EUREF89.
          Result: 0 => ok
                < 0 => error

           (    > 0   => eetm27 -> UTM
  direct   {   == 0   => No trans, alarm
           (    < 0   => UTM -> eetm27

  N_in,  E_in : Input  coordinates in metres
  N_out, E_out: Output coordinates in metres

  usertxt: User-def. additional alarm text (e.g. a st_nmb)
  tr_error: FILE for alarm texts. Opened by the user

  Prog: Knud Poder  5 OCT 1995
#endif



/* epsg_to_mlb.h                   # page 1   DD Mmm 20YY hh mm */
/* -- Function 5 -----------------------------------------------------------------*/
int          fe_trans (
union geo_lab      *in_lab,
union geo_lab      *outlab,
double              N,
double              E,
double              H,
double             *Nout,
double             *Eout,
double             *Hout,
char               *usertxt,
FILE               *tr_error
);

#if !0xD0C

 2. dim. transformation function for area FE


*in_lab      :  from label
*out_lab     :    to label
N, E, H      :  from coordinates
*Nout, *Eout :    to coordinates
*Hout        :    to height == H
*usertxt     :  to be written in case of errors
*tr_error    :  file for error reports
             :  tr_error == NULL: NO REPORT IS PRINTED

#endif


/* fgetdt version 2010.1            # page 1   12 jan 2010 09 39 */
/* -- Function 5 -----------------------------------------------------------------*/
int          fh_trans (
union geo_lab       *in_lab_u,
union geo_lab       *outlab_u,
double              N,
double              E,
double              H,
double              *Nout,
double              *Eout,
double              *Hout,
char                *usertxt,
FILE                *tr_error
);

#if !0xD0C

 Fehmarn Bridge Projection test of AREA
 and transf to/from geo

 2. dim. transformation function for area DK


*in_lab      :  from label
*out_lab     :    to label
N, E, H      :  from coordinates
*Nout, *Eout :    to coordinates
*Hout        :    to height == H
*usertxt     :  to be written in case of errors
*tr_error    :  file for error reports
             :  tr_error == NULL: NO REPORT IS PRINTED

#endif


/* fo_g_tr.h                       # page 1   29 Sep 1995 16 30 */


/* -- Function 5 -----------------------------------------------------------------*/
int               fo_g_tr (
int                      mode,
int                      direct,
double                   N_in,
double                   E_in,
double                  *N_out,
double                  *E_out,
char                    *usertxt,
FILE                    *tr_error
);

#if !0xD0C
  fo_g_tr:   Autochecking dual prediction function for pre-
  diction  from UTM zone 29 ETRS89 to
      to_sys = {UTM zone 29 ED 50, FK54, FU50} or reverse

  Result: 0 => ok
         -1 => error

           (    > 0   => utm29_etrs89 to to_sys
  direct   {   == 0   => No trans, alarm
           (    < 0   => to_sys to utm29_etrs89

  N_in,  E_in : Input  coordinates in metres
  N_out, E_out: Output coordinates in metres

  usertxt: User-def. additional alarm text (e.g. a st_nmb)
  tr_error: FILE for alarm texts. Opened by the user

  Prog: Knud Poder  5 OCT 1995
  Prog: K. Engsager 4 MAY 2011
#endif



/* fputcw.h version 1.0              # page 1    1 Mar 1993 11:18 */
/* -- Function 5 -----------------------------------------------------------------*/
int            get_zone (
union geo_lab        *u_lab
);

#if !0xD0C
 get_zone(lab) : Service function for finding the zone
    result  >  0 : the zone of the projection (e.g. utm)
    result == -1 : the coord lab has no zone
    result == -2 : the zone and the parameters do NOT agree
    result == -3 : the zone was not found
    result == -4 : the label is not a coord label

  SEE also conv_lab.c and conv_lab.h for details



service function for finding zone of a projection
    and testing of appropriate parameter of the COORD labels

Prog: K. Engsager, SEP 2000

#endif




/* getsinexlin  ver 2008.01        # page 1    1 Dec 2008 15 35 */
/* -- Function 5 -----------------------------------------------------------------*/
int           ng_trans (
union geo_lab       *in_lab_u,
union geo_lab       *outlab_u,
double               N,
double               E,
double               H,
double              *Nout,
double              *Eout,
double              *Hout,
char                *usertxt,
FILE                *tr_error
);

#if !0xD0C
 3D transformation function for NAD83G to/from GR96
 NAD83G is GREENLAND and is  NOT  equal to NAD83


*in_lab             :  from label
*out_lab            :    to label
N, E, H             :  from coordinates
*Nout, *Eout, *Hout :    to coordinates
*usertxt            :  to be written in case of errors
*tr_error           :  file for error reports
                    :  tr_error == NULL: NO REPORT IS PRINTED

Progr.: K.Engsager, Danish National Space Center, 2006
#endif



/* parse_def_file.h  ver 2013.01      # page 1     19 Feb 2013 11 57 */
/* -- Function 5 -----------------------------------------------------------------*/
int                  s34jtos (
double                      y,
double                      x,
double                      *yout,
double                      *xout,
int                         direct,
char                        *usertxt,
FILE                        *tr_error
);

#if !0xD0C
WARNING: The function is based upon far too few coordinate pairs,
but after the direct demand of the SDJ-division of KMS no improvements
will be introduced.

THE GD OF KMS WILL STRONGLY WARN AGAINST ANY PROFESSIONAL USE OF THE
FUNCTION - EXCEPT FOR FINDING PRELIMINARY COORDINATES FOR AN ADJUSTMENT,
WHICH THUS IS THE ONLY REASON FOR NOT DISCARDING IT.

The x and y notation for the coordinates follow the Prussian cadastral
principles, so that the x-coordinate is north-going. This is the basis
for the unnecessary clumsy swopping of axes in the code, but it seems
to be waste of time to rewrite the code of a too poor function, which is
only kept for finding preliminary coordinates.

s34jtos: int function giving the transformations

          s34 Jylland <-> Ostenfeldts system

The transformation may have errors up to 0.5 m, but the local
consistency is generally much better. The computing precision is
better than 0.5 mm.
s34jtos returns the result of the transformation as follows

      0     =>   all ok
      1<<5  =>   error (tolerance exceed, or parameter error)

y, x input coordinates (s34j or os), double
in units of metres. y and x are called by value.

yout, xout output coordinates in (os or s34j), double
in units of metres. yout and xout are called by reference.

direct, call by value indicating

          direct > 0  => transformation s34j --> os

          direct < 0  => transformation os --> s34j

          direct = 0  => no transformation, exit with error.

tr_cntr      control of alarm actions
      tr_cntr == 1  => activation of an alarm function
      tr_cntr != 1  => no alarm is given, but 1<<5 returned.

example:
if (s34jtos(y, x, \&Y, \&X, 1, 1))
fprintf(stderr, "\\nerror in s34jtos");
produces os coordinates Y,X from the s34j coordinates y,x.
Note the operator \& on Y and X.
#endif


/*  s34stkk.h version 1.0             # page 1   22 Feb 1993 12 54 */
/* -- Function 5 -----------------------------------------------------------------*/
int                 s34stkk (
double                     y,
double                     x,
double                     *yout,
double                     *xout,
int                        direct,
char                       *usertxt,
FILE                       *tr_error
);

#if !0xD0C
s34stkk: int function giving the transformations

          s34 Sjlland <-> Kbh. kommunes system

The transformation may have errors up to 0.5 m, but the local
consistency is generally much better. The computing precision is
better than 0.5 mm.
s34stkk returns

       0 => ok
       != 0 in case of error.

y, x input coordinates (s34j or kk), double,
in units of metres. y and x are called by value.

yout, xout output coordinates in (kk or s34j), double
in units of metres. yout and xout are called by reference.

direct, call by value indicating

          direct > 0  => transformation s34j --> kk

          direct < 0  => transformation kk --> s34j

          direct = 0  => no transformation,
          i. e. yout = y and xout = x (no alarm occurs)

tr_cntr      (call)        int
Control of error actions:
      tr_cntr == 1   => alarm output to tr_error
      tr_cntr == 0   => alarm output and return of -1.

usertxt      (call)        *char
A text string made by the user and copied into the alaem output.

tr_error     (call)        *FILE
A stream opened by the calling programme. The error text from t_report
is copied intp the stream.

example:
(void) s34stkk(y, x, \&Y, \&X, 1, 1, usertxt, tr_error);
printf("\\n Y = %14.6f X = %14.6f", Y, X);
produces KK coordinates Y,X from the s34j coordinates y,x.
Note the operator \& on Y and X.

  prog: Olaf Andersson jan 1979
  modification (s34s->kk): WLW, Januar 1989
  C-version  KP 1989
#endif


/* s_status.h                       # page 1   7 jan 2009 13 26 */
/* -- Function 5 -----------------------------------------------------------------*/
int             tcts_u (
double       Nin,
double       Ein,
double       *yout,
double       *xout,
int          landsdel,
int          direct,
char         *usertxt,
FILE         *trferror,
...
);

#if !0xD0C
Function and parameters

tcts_u          int function,   fnc no. 8        call and return
Dual autochecking transformation (prediction) function for

  u       TR TC zone 32, ED50 <-> UTM zone 32, ED50
  j       TR TC zone 32, ED50 <-> System 1934 Jylland
  s       TR TC zone 32, ED50 <-> System 1934 Sjælland
  b       TR TC zone 33, ED50 <-> System 1934/45 Bornholm

When landsdel is CAPITAL LETTER then the partial derivatives are
included and the results are placed in the last double pointer
position (called *DD):
          *(DD+0) = d(*yout) / d(Nin)
          *(DD+1) = d(*yout) / d(Ein)
          *(DD+2) = d(*xout) / d(Nin)
          *(DD+3) = d(*xout) / d(Ein)
          *(DD+4) = d(Nin) / d(*yout)
          *(DD+5) = d(Nin) / d(*xout)
          *(DD+6) = d(Ein) / d(*yout)
          *(DD+7) = d(Ein) / d(*xout)

The function gives the result of the transformation as

      tcts_u = 0        => ok,
      tcts_u = -1       => parameter error (landsdel or direct)
      tcts_u = -2       => coordinates too far away.

The physical precision of the transformation is generally
1-2 cm, but the numerical precision is generally some few
mm, increasing to some cm far from the central parts.

Alarm printing is to an error file. The user may add
identification of the alarm.

Nin, Ein        double                           call
UTM or s34 coordinates input for transformation in metres.

*yout, *xout    double                           return
s34 or UTM coordinates output after transformation in metres.

landsdel        int                              call
Control of the used coordinate systems.
landsdel = 'j' (or 1) => TR TC 32 ED50 <-> S34 Jylland
landsdel = 's' (or 2) => TR TC 32 ED50 <-> S34 Sjælland
landsdel = 'b' (or 3) => TR TC 32 ED50 <-> S34 (= S45) Bornholm
landsdel = 'u' (or 4) => TR TC 32 ED50 <-> UTM 32 ED50
Any other character (or value) creates alarm output on stdout
and return(-1).

direct           int                             call
Control of the direction of transformation.
     direct > 0 =>  UTM -> s34
     direct < 0 =>  s34 -> UTM
     direct = 0 =>  alarm output to the errorfile.

*usertxt          char                           return
A string pointer to a char array. The string should be written
before the call of the function. The station number is a possi-
bility. The string is written to the alarm file.

trferror        FILE                            return
A pointer to a stream. The file must be produced, opened and
closed by the user. Alternatively stdout or stderr may be used.

optional when landsdel is a CAPITAL LETTER:
*DD             double                           return
A pointer to the partial derivatives result
          *(DD+0) = d(*yout) / d(Nin)
          *(DD+1) = d(*yout) / d(Ein)
          *(DD+2) = d(*xout) / d(Nin)
          *(DD+3) = d(*xout) / d(Ein)
          *(DD+4) = d(Nin) / d(*yout)
          *(DD+5) = d(Nin) / d(*xout)
          *(DD+6) = d(Ein) / d(*yout)
          *(DD+7) = d(Ein) / d(*xout)

C extern functions:
double fabs()
int    printf()
int    fprintf()

KMS special C extern functions:
none

The tolerance is set by a define statement to 6 cm, and may
be redefined. The chosen value is comparable to the precision
of the polynomial approximation.

Prog: Knud Poder, APR 1991

#endif


/* tpd_doc_f.h  version ????.?           # page 1   DD  Mmm YYYY hh mm */

/* -- Function 5 -----------------------------------------------------------------*/
int u32_to_map (
double                n,
double                e,
char                  kvartstg[20]
);

#if !0xD0C
 F. Steffensen JUL. 1997 (MI/ITG)
 SWN sept. 1997 REK (enkelt rettelse vedr. indlab på Bornholm

 u32_to_map.c er en udgave af nedenstående

 u32_t_kvart.c
*
* Funktion til at beregne hvilket kortbladskvartnummmer en
* koordinat (n,e) i UTM zone 32 opgiver i meter ligger i.
*
* OBS : Særkortene 10, 11, 20, 21, 30, 31 og 40 ikke medtaget.
*
* retur = u32_t_kvart(n, e, kvartstg);
*
* double n;   northing i meter                                (call)
* double e;   easting  i meter                                (call)
* char kvartstg[20];  fuld kvartbeteg. f.eks. 1214 iii NE 8  (return)
*
* Returværdier :
* -2                : Koordinat ligger udenfor acceptable værdier.
* -1                : Transformationsfejl
* 1                 : Koordinat vest for 10xx-søjlen
* 2                 : Koordinat nord for xx19-rækken og imellem
*                     10xx og 19xx søjlerne.
* 3                 : Koordinat syd for xx10-søjlen og imellem
*                     10xx og 19xx søjlerne.
* 4                 : Koordinat øst for 19xx-søjlen.
* 1010111 - 1919449 : Fuld numerisk kvartbetegnelse.
*                     Første 4  cifre  : 1cmblad nummer
*                            5. ciffer : 2cmblad nummer
*                            6. ciffer : 4cmblad nummer
*                            7. ciffer : kvart   nummer
*
* Det eksisterende kortbladsystem er udviddet til at spænde fra
* 1010 til 1919 således at utm-koordinaten omregnes til gs, hvorefter
* det undersøges, om koordinaten ligger øst for 16xx-søjlen.
* Er dette tilfældet omregnes koordinaten istedet til u33.
*
* For de kortblade, der er rykket fra de systemrigtige placeringer
* (1318, 1416 ii se, 1417 iv) er der taget nogle specielle forholds-
* regler, idet de (søkortnavne)koordinater, der ligger indenfor det
* systembladsområde, som det rigtige blad bliver rykket væk fra, rykkes
* ud til nærmeste kvart udenfor, således at disse ikke vil give samme
* bladreference som de 3 rykkede blade. (se senere i koden)
*
* På trods af at bladrammerne i u33 er større end i gs viser det sig
* at, når kortbladssystemet udviddes til hhv. 1010-1619 (gs) og
* 1710-1919 (u33) vil projektionssystemerne konvergerer så meget, at
* der opstår en åben kile mellem 16xx i gs og 17xx i u33 spidsende mod
* nord. Koordinater, der ligger i denne trekant, placeres på nærmeste
* blad i 17xx-rækken.
*
* MS 08/92
#endif





/*  utgs.h version 2.0                # page 1   22 Feb 1993 12 54 */
/* -- Function 5 -----------------------------------------------------------------*/
int               utgs (
double                    Nin,
double                    Ein,
double                    *yout,
double                    *xout,
int                       landsdel,
int                       tr_cntr,
char                      *usertxt,
FILE                      *tr_error
);


#if !0xD0C
utgs: int function giving the transformations

          utm zone 32 <-> gs system (Jylland and Sjlland)
                         and
          utm zone 33 <-> gsb system (Bornholm).

The transformation may have errors up to 1 m, but the local
consistency and computing precision is of the order of 1 cm.
utgs returns the same value as returned in yout.

Nin, Ein input coordinates (utm or gs(b)), double
in units of metres. Nin and Ein are called by value.

yout, xout output coordinates in (gs(b) or utm), double
units of metres. yout and xout are called by reference.

landsdel, call by value indicating

          1 (or 2) => gs and utm zone 32 i.e. Jylland + Sjlland,
          's', 'j', or 'd' gives the same effect.

          3 (or 'b')  => gsb and utm zone 33 i.e. Bornholm.

direct, call by value indicating

          direct > 0  => transformation utm --> gs

          direct < 0  => transformation gs --> utm

          direct = 0  => no transformation,
          i. e. yout = Nin and xout = Ein (no alarm occurs)


#endif


/* utmtmgr.h                        # page 1   22 Aug 1994 11 08 */
/* -- Function 5 -----------------------------------------------------------------*/
int          utmtmgr (
union geo_lab       *c_lab_u,
int                 direct,
double              *N,
double              *E,
double              *B,
double              *L,
char                *mgref,
int                 cif
);

#if !0xD0C

Function:
Transformation to/from MilGridRef (MGR) and UTM. The UTM
coordinates refer to the zone described in the label, but
may be outside the zone limits. The MGR-strings produced
are however placed inside the proper UTM zone.

utmtmgr          (call and return)        int
The value of the function is for UTM -> MGR the zone number
difference and 0 for MGR -> UTM in case of succes and -100
in case of inconsistency of the MGR-string.

Parameters:
*c_lab           (call)                   struct coord_lab
The label describing the UTM zone of the input coordinates.
In case of transformation MGR -> UTM any UTM coordinate label
of proper datum is accepted.

direct           (call)                   int
Control of the direction of the transformation:

      direct == +1   => UTM -> MGR
      direct == -1   => MGR -> UTM

any other value causes an error exit.

*N, *E          (call and return)       double
The UTM coordinates. The input values should be in the UTM
zone described by the label. The return values are in the
proper UTM zone.

*B, *L          (return)                double
The geographical coordinates on the datum described in the
UTM coordinate label.

*mgref         (return or call)         char
The MGR string, returned in case of UTM -> MGR and input in
case of MGR -> UTM.

cif            (call)                   int
The number of digits in the E and N values of the MGR-string.
The call value is not used in case of transformation MGR->UTM.

PROG:  Knud Poder, AUG 1994

#endif



/* utsb.h                         # page 1   22 Feb 1993 12 54 */
/* -- Function 5 -----------------------------------------------------------------*/
int               utsb (
double                   N_in,
double                   E_in,
double                   *N_out,
double                   *E_out,
int                      direct,
char                     *usertxt,
FILE                     *tr_error
);

#if !0xD0C
  function and parameters

  utsb               (return)                   int
  Dual autochecking transformation procedure for transformation
  between UTM Zone 32 and system sb. An alarm is produced, when
  the check by the inverse transformation exceeds the tolerance
  of 0.5 mm.

  The values of the function are:

          0       => ok
          1<<11   => check tolerance of 0.0005 m exceeded
          -1      => outside the sb area

  The permissible sb area is in sb-coord:

        6 125 000 m  <= Northing <=  6 142 000 m
                485 000 m  <= Easting  <=    518 000 m

  N_in, E_in             (call)                   double
  The s34- or sb-coordinates input in units of metres.

  N_out, E_out             (return)                 double
  The sb- or UTM32-coordinates output from the procedure
  as transformed and checked coordinates in units of metres.

  direct                 (call)                   int
  direct >= +1  => transformation utm32 -> sb
  direct <= -1  => transformation sb -> utm32
  direct ==  0  => no transformation (output == input)


        Prog: Knud Poder  DEC 1992
#endif


/* v_red.h                        # page 1   22 Feb 1993 12 54 */
/* -- MODULE META ------------------------------------------- */
#if !0xD0C
%\summary{TRLIB Metadata and Table Handling}
%\author{The TRLIB team}
%\date{1977--2013}
#endif
/* -- Function 6 -----------------------------------------------------------------*/
extern void       c_tabdir_file (
int               mode,
FILE             *f_in
);

#if !0xD0C
 c_tabdir_file(mode, f_in) closes the file pointed by f_in
      some GLOBAL PARAMS are initialized to zero

 sevice routine for label and table system

    result ==  0 : ok

    mode         : kind of file to be opened
    mode   ==  0 : one short call for mode = 1 con 2 con 3
    mode   ==  1 : manager.tab            GLOBAL PAR:
                 :  *manager_file = NULL, init_man_pos = 0
    mode   ==  2 : manager.gps            GLOBAL PAR:
                 :  *man_gps_file = NULL, init_gps_pos = 0
    mode   ==  3 : def_lab.txt            GLOBAL PAR:
                 :  *def_lab_file = NULL, init_lab_pos = 0
                 :  init_prj_pos = 0, init_rgn_pos = 0
                 :  init_dtm_pos = 0, init_grs_pos = 0
    mode   ==  4 : binary file
    mode   ==  5 : txt file
    mode   ==  6 : txt file

   *f_in         : NOT USED  in mode 0 | 1 | 2 | 3
                 : else the file pointer to be closed
#endif


/* -- Function  6 -----------------------------------------------------------------*/
extern void       ch_tabdir_par (
int               mode,
FILE            **s_f,
size_t           *s_p,
FILE             *n_f,
size_t            n_p
);

#if !0xD0C

 ch_tabdir_par(mode, **s_f, *s_p, *n_f, n_p) saves some
      PARAMS to (**s_f, *s_p)
      and asigns new values from (*n_f, n_p)

 sevice routine for label and table system

    result ==  0 : ok

    mode         : kind of file to be opened
    mode   ==  1 : *manager_file, init_man_pos
    mode   ==  2 : *man_gps_file, init_gps_pos
    mode   ==  3 : *def_lab_file, init_lab_pos
                 :  init_prj_pos = n_p, init_rgn_pos = 0
                 :  init_dtm_pos = 0, init_grs_pos = 0
#endif



/* cic.h                            # page 1   24 Aug 1994 17 49 */

/* -- Function 6 -----------------------------------------------------------------*/
int               conv_cnd(
char                    *lab_info,
struct lab_def_str      *p_lb,
struct obs_lab          *o_lab,
...
);


#if !0xD0C
 conv_cnd(name, p_lb, lab) Service function for conv_lab
 NB: should be called only from conv_lab
    result ==  0 : no lab
    result ==  9 : obs lab               in lab
    result == 16 : cond lab              in lab
    UNDEFSYS     : unrecognized name
 an unrecognized name is considered a filename containing a
 genuine label

 progr. Karsten Engsager, 2005.
 updt.  Karsten Engsager, 2010.
#endif





/* conv_crd    ver 2010.01          # page 1   12 jan 2010 10 19 */

/* -- Function 6 -----------------------------------------------------------------*/
int            conv_crd(
char                 *lab_name,
struct lab_def_str   *p_lb,
struct coord_lab     *c_lab,
...
);

#if !0xD0C
 conv_crd(name, p_lb, lab) service function for conv_lab
    result ==  0 : no lab
    result ==  1 : coord lab             in lab
 an unrecognized name is considered a filename containing a
 genuine label




  SEE also conv_lab.c and conv_lab.h for details

 LABEL TABLE

 The struct label_table is defined in geo_lab.h in order to make
 it available as a parameter for conv_crd, conv_hgt, etc.



service function for conversion of COORD labels

Prog: Knud Poder, FEB 1995
      K. Engsager, updated JAN 1999
      K. Engsager, updated JAN 2010

#endif



/* conv_hgt    ver 2010.1          # page 1   12 jan 2010 10 19 */

/* -- Function 6 -----------------------------------------------------------------*/
int               conv_hgt (
char                    *lab_info,
struct lab_def_str      *p_lb,
struct hgt_lab          *h_lab,
...
);

#if !0xD0C
 conv_hgt(name, p_lb_ex, lab) fills data in the label
    see conv_lab.h
 an unrecognized name is considered a filename containing a
 genuine label



 service function called from conv_lab

Prog: Knud Poder, FEB 1995
      K. Engsager, updated JAN 1999
      K. Engsager, updated JAN 2010
#endif

/* conv_lab  ver 2010.01            # page 1    12 jan 2010 17 25 */


/* -- Function 6 -----------------------------------------------------------------*/
int            conv_lab (
char                           *lab_name,
union geo_lab                  *g_lab,
  ...
);

#if !0xD0C
 conv_lab(name, lab) converts the name of a lab to the lab
    result ==  -2     : def_prj.txt file not correct content
    result ==  -1     : def_prj.txt file not found
    result ==   0     : no lab
    result == CRD_LAB : coord lab                    in lab
    result == HGT_LAB : height lab                   in lab
    result == GDE_LAB : geoid                        in lab
    result == WOR_LAB : wor lab                      in lab
    result == POC_LAB : Photo OC lab                 in lab
    result == DTM_LAB : Digital Terrain ModeL lab    in lab
    result == DHH_LAB : Diff Height Model lab        in lab
    result == UPL_LAB : landUPLift model lab         in lab
    result == OBS_LAB : obs lab                      in lab
    result == DEM_LAB : Digital Elevation ModeL lab  in lab
    result == TAB_LAB : unspecified TABle lab        in lab
    result == IDT_LAB : idt lab                      in lab
    result == JDF_LAB : jdf lab                      in lab
    UNDEFSYS     : unrecognized name
    result == STP_LAB : stop lab
 an unrecognized name is considered a filename containing a
 genuine label


 Parameters

lab_name              call               *char
The mini_label string of the system. See labels below
extra info may be read from 3rd parameter except :
special :
lab_name == "input"  : from file <3rd parameter>
lab_name == "in_u_\#" : from file <3rd parameter> (no \#)  1)
lab_name == "output" :   to file <3rd parameter>
lab_name == "OUTPUT" :   to file <3rd parameter>
lab_name == "?"      : ordinary info                     2)
lab_name == "*"      : detailed info                     2)
lab_name == "\&"      : separated info                    2)
special special :
lab_name may be a filename containing a label.

1) only used internally in recursive mode.
2) for documentary purposes only.

g_lab                 return             *union geo_lab
The full label after a succesfull conversion of the
lab_name. See geo_lab.h.

Prog: KP, KE, FS.




   ABOUT  LABELS

LABELS read from a <file> must begin with \#


All labels may be written in a file which name is different from
the first caracters of a label name. Special coordinate labels
having extra parametres may then be read from the file.
conv_lab reads from the file <filename> when this is named as a
label. Obscure errors may occure when an erroneous <label name> is
written and this <label name> allso exists as a file name.

              STOP
             ******
#stop will stop further reading from the file


              COORDINATES / HEIGHTS
             ***********************

Coordinate labels have usually a datum definition associated ::
   _ed50             European Datum 1950
   _euref89          Euref Datum 1989
  (_wgs84            == _euref89)
Special coordinate systems has an implicit datum definition
example : System 1934.  (_ed50)

A coordinate label may include a height by ::
-  substituting _ in the datum definition by : H or E
-  concatenating a special label by          : H or E

   E       means Heights above the ellipsoid
   H       means Heights above the zero reference surface
           the coordinate label is then concatenated by :
           _<height label>
   N       is at present converted to H and concatenation  _h_msl

Labels ::
   s34j                        System 34 Jylland
   s34jH_h_msl                  System 34 Jylland and Normal heights.
   s34jE                        System 34 Jylland and Ellips. heights.

   s34s                        System 34 Sjælland

   s34b                        System 34 Bornholm
   s45b                        System 34 Bornholm

   u32                          UTM zone 32 ED50
   utm33_ED50                   UTM zone 33 ED50
   utm32Eeuref89                UTM 32 euref89 and Ellips. heights.
   utm33Heuref89_h_msl          UTM 33 euref89 and Normal heights.
   .
   .

   geo_euref89                 Geographic EUREF89
   geoHeuref89_h_dvr90         Geographic EUREF89 and Normal heights.
   geoEeuref89                 Geographic EUREF89 and Ellips. heights.

   geo_ed50                    Geographic ED50

   crt_ed50                    Cartesic ED50
   crt_euref89                 Cartesic EUREF89

   h_dvr90                     Othometric heights DVR 1990
   h_gm91                      Othometric heights G.M. 1891 (Jylland)
   h_gi44                      Othometric heights G.I. 1944 (Fyn/Sjæll.)
   DK_h_msl                    Othometric heights in known local system
   DK_h_kn44                   Københavns Heights e.a.
   DK_ellh_euref89             Ellipsoid heights in euref89.

For KMS computers ::
special labels and observation labels may be seen by
hjælp labels

#endif




/* conv_obs  ver 2010.1            # page 1   12 jan 2010 15 35 */


/* -- Function 6 -----------------------------------------------------------------*/
int               conv_obs (
char                    *lab_info,
struct lab_def_str      *p_lb,
struct obs_lab          *o_lab,
...
);

#if !0xD0C
 conv_obs(name, p_lb, lab) Service function for conv_lab
 NB: Should be called d from conv_lab only
    result ==  0 : no lab
    result ==  9 : obs lab               in lab
    UNDEFSYS     : unrecognized name
 an unrecognized name is considered a filename containing a
 genuine label


  SEE conv_lab.h for details

service function called from conv_lab : OBS labels

Prog: Knud Poder, FEB 1995
      K. Engsager, updated jan 1999
      K. Engsager, updated jan 2010

#endif



/* conv_plm    ver 2010.1          # page 1   12 jan 2010 10 19 */

/* -- Function 6 -----------------------------------------------------------------*/
int               conv_plm (
char                    *lab_info,
struct lab_def_str      *p_lb,
struct plm_lab          *p_lab,
...
);

#if !0xD0C
 conv_plm(name, p_lb, lab, ...) Service function for conv_lab
 result == PLM_LAB : no_net_rotation PLate Motion lab
 ERROR :
 result == ITRF_NON_ : NO entries found.
 result == ITRF_MAX_ : too many entries found. (> MAX_ENT_PLM)
 result == ITRF_NAM_ : manager.gps table_name (gps_tab) not found.
 result == ITRF_SYS_ : manager.gps file not found.

 name == "input" | "" | "output" | "OUTPUT", "SURVEY"
 will load the plate velocities for the model specified i p_lb
 name == "output" | "OUTPUT" | "SURVEY"
 will write the label on the o_file: ...
      (with info: CAPITAL) (survey: SURVEY)

 The struct lab_def_str is defined in geo_lab.h in order to
 make it available as a parameter for conv_crd, conv_hgt, etc.

 p_lb->name == "nnnr_"
 p_lb->mlb2 == <model>
 the plate motions model nnr_<model> is searched and loaded
 the name: nnr_std gives the newest (first found) model

 conv_plm initializes the plm_lab : p_lab with the
 rotation rates for the  p_lab->plm_max plates p_lab->plm__tr.
 (MAX_ENT_PLM) plates

  SEE also conv_lab.c and conv_lab.h for details
#endif


/* conv_rgn.h                       # page 1   21 dec 2010 10 46 */

/* -- Function 6 -----------------------------------------------------------------*/
short     conv_rgn (
short            rgn_nr,
char            *rgn_prfx,
char            *rgn_name
);

#if !0xD0C
 conv_rgn : info of region-nr, - prefix, and the country og region
 rgn_nr   : the internally used number = the num. rep. the prefix
 rgn_prfx : region prefix (2-letter abrev. country name)
 rgn_name : Country

 NB: Some old rgn_prfx has been used. Old_prfx is converted to new

 RETURN::
 conv_rgn >  0: == rgn_nr
 conv_rgn = -1: rgn_nr or rgn_prfx NOT FOUND
 conv_rgn = -2: def_file (def_lab.txt and "def_rgn") NOT FOUND

 CALL ::
 rgn_nr = -1: rgn_prfx is searched to return: conv_rgn, rgn_name
 rgn_nr > 0 : rgn_nr is searched to return: rgn_prfx, rgn_name


 special call: (rgn_nr = -1)
             : and (first char in rgn_prfx is '?' | '!' | '*')
 RETURN::
 conv_rgn = old_rgn_nr
 rgn_prfx = NEW_rgn_prfx
 CALL ::
 rgn_prfx = "?<region>": rgn_prfx is updated
 rgn_prfx = "?" | "!" | "*"  : loops over all defined Names
            when called reiterately
            until conv_rgn == -1 signals the end.
   ONLY by "*" calls the old_prfx are included


 NB:: illegal call: conv_rgn(-1, "?", name);

 NB::   legal call:
   union rgn_un   rgn_new, rgn_old;

   (void) strcpy(rgn_new.prfx, "?");
   rgn_old.r_nr[1] = 0;
   rgn_old.r_nr[0] = conv_rgn(-1, rgn_new.prfx, name);

 Prog. Knud Poder, 16 NOV 1994, after ideas by K.E and F.S.
 Modified, KE SEP 2010
#endif



/* conv_spc    ver 2010.1          # page 1   12 jan 2010 10 19 */


/* -- Function 6 -----------------------------------------------------------------*/
int               conv_spc (
char                    *lab_info,
struct lab_def_str      *p_lb,
union  geo_lab          *u_lab,
...
);


#if !0xD0C
 conv_spc(name, p_lb, lab) service function for conv_lab
    result ==  0 : no lab
    result ==  1 : wor lab               in lab
 an unrecognized name is considered a filename containing a
 genuine label

  SEE conv_lab.h for details

service function called from conv_lab : WOR, POC, IDT, JDF labels

Prog: Knud Poder, FEB 1995
      K. Engsager, updated jan 1999
      K. Engsager, updated jan 2010

#endif


/* conv_tab  ver 2010.1            # page 1   12 jan 2010 15 35 */

/* -- Function 6 -----------------------------------------------------------------*/
int            conv_tab (
char                 *lab_info,
struct lab_def_str   *p_lb,
struct gde_lab       *t_lab,
...
);

#if !0xD0C
 conv_lab(name, lab) converts the name of a lab to the lab
 see. this
 an unrecognized name is considered a filename containing a
 genuine label


  SEE conv_lab.h for details

service function called from conv_lab : TABLE labels

Prog: K. Engsager, DEC 2001
Prog: K. Engsager, DEC 2010

#endif


/*  ctc.h version 1.0               # page 1   25 Aug 1994 17 13 */
/* -- Function 6 -----------------------------------------------------------------*/
int doc_rgn (char *rgn_name, char *descr, int detail);
/* -- Function 6 -----------------------------------------------------------------*/
int doc_prj (char *prj_name, char *descr, char *impl_datum, int *type, int detail);
/* -- Function 6 -----------------------------------------------------------------*/
int doc_dtm (char *dtm_name, char *descr, int detail);
/* -- Function 6 -----------------------------------------------------------------*/
int get_def_numbers (int *n_prj, int *n_dtm, int *n_grs, int *n_rgn);
/* -- Function 6 -----------------------------------------------------------------*/
int doc_grs (char *ell_name, FILE *out, int detail);
/* -- Function 6 -----------------------------------------------------------------*/
void present_data (FILE *fp, def_data *data);
#if !0xD0C
Introspection
#endif


/* ee_trans.h version 2.0            # page 1   25 Aug 1995 14 03 */
/* -- Function 6 -----------------------------------------------------------------*/
int setup_epsg_table (FILE *f_in);
#if !0xD0C
 will initialise the epsg definition table
#endif

void release_epsg_table (void);
#if !0xD0C
 release the table again
#endif

/* -- Function 6 -----------------------------------------------------------------*/
 int export_to_epsg (char *mlb, int *h_code, int *v_code);
/* -- Function 6 -----------------------------------------------------------------*/
 int import_from_epsg (int h_code, int v_code, char *mlb);
#if !0xD0C
 import and export functions
#endif



/* erad.h                           # page 1   29 sep 2009 16 08 */
/* -- Function 6 -----------------------------------------------------------------*/
char         *f_nxt (
char                *str,
int                 *items,
FILE                *infile,
int                 used
);

#if !0xD0C
*f_nxt : next item of a line of character strings from fgetln_kms
*str   : the array of characters used by fgetln_kms
*items : the number of items remaining after call by fgetln_kms
*infile: the stream connect to the file
used   : the number of characters used in the previous call

The function utilises the filtering facilities of fgetln_kms.

A typical call for sgetg would be

G = sgetg(f_nxt(in_a, \&q, infile, used), \&tpd, \&used, udt);

Before first call use:  q = 0; (the value of used is then
not used).

Use the %n facility in sscanf for finding  the value of used.

PROG: Knud Poder, FEB 1995

#endif




/* fe_trans.h version 2010.1            # page 1   12 jan 2010 13 02 */


/* -- Function 6 -----------------------------------------------------------------*/
double       fgetdt (
double             *JD2000,
int                *used,
int                *term_c,
FILE               *fp
);

#if !0xD0C
 function fgetdt : reading of comment word, G.I. date_time
and MD. long word.

fgetdt       (return and return)        double
_            Date stripped for special characters

fp           (call)                     FILE *
_            Pointer  to a file opened and ready for reading

JD2000       (return)                  *double
_            Julian Date from JD 2000.0

used         (return)                  *int
_            Number of characters read from the file

term_c       (return)                  *int
_            Last char read, terminating char

The function reads date, time in the following format
ISO (our)   1992 04 17,  9.45  or
ISO         1992 04 17

Upon error the function returns -2.0.

Prog. Karsten Engsager 1999.
Upddated: KE 2010

#endif


/* fgetg.h                            # page 1   22 Feb 1993 12 54 */

/* -- Function 6 -----------------------------------------------------------------*/
double               fgetg (
FILE                        *str,
struct typ_dec              *tpd,
int                         *used,
char                        *udt
);

#if !0xD0C
fgetg returns a geodetic value read from the FILE *str.
*tpd contains the type and decimal of the read value
*used is the number of characters read
*udt is a terminating type string, which is used when none is read

reading is stopped after two 'sp' or a terminating string

the values are :
for metric values     in units of metres
for angular values    in units of radians
for potential values  in units of gpu
for gravity values    in units of gal
for mean error values in units of 1

#endif



/* fgetlhtx.h                       # page 1   09 Feb 1995 14 49 */
/* -- Function 6 -----------------------------------------------------------------*/
int         fgetlhtx (
FILE             *iofile,
char             *in_str
);

#if !0xD0C
fgetlhtx: input of the forrunner (\#) of a labeltext and the labeltext
iofile  : the input file with the label-text
in_str  : the output string with the labeltext (the \# is skipped)
return values:

      0  => ok
     -1  => old forerunner (<) input
     EOF => end of file

PROG: Knud Poder, FEB 1995

#endif



/* fgetln_kms version 1.0               # page 1   06 Sep 1994 16 42 */
/* -- Function 6 -----------------------------------------------------------------*/
int                  fgetln_kms (
char                        *outstr,
int                         *n_item,
FILE                        *fp
);

#if !0xD0C

fgetln_kms       (return)          int function
_            Number of characters pointed to by *outstr or EOF

outstr       (return)          char *
_            Char pointer to char array

n_item       (return)          int *
_            Integer pointer holding number of item returned

fp           (call)            FILE *
_            File pointer to open file

The function reads one line from a file pointed to by fp, skips
all single spaces, replace two or more spaces, or tabs with
two spaces.

Comments * ; (old algol) and the ones found in c_prog ( )
are skipped and replaced with two spaces, comments
over more than one line, are skipped and returned in one line
with two spaces.

Strings in "xx" will be returned as they are found.

The \\n at the end is returned too.

The function itself return numbers of characters including \\n,
or EOF.

PROG:  Frands Steffensen
F. Steffensen SEP. 1998 (MI/ITK) return strings unformatted

#endif


/* fgetshpprj  ver 2008.01          # page 1     6 Jan 2009 11 57 */




/* -- Function 6 -----------------------------------------------------------------*/
int       fgetshpprj (
FILE            *fh_in,
union geo_lab   *g_lab);

#if !0xD0C
 fgetshpprj reads the .prj file to a shape file
   and sets the geo_lab accordingly.

   The reference names for label recognition is found
   in the file def_sjp.txt in the geoid catalouge.

   fgetshpprj returns::
   0    prj line ok. label_set ok.
   -1   syntax error in prj line.
   -2   def_shp.txt  help file not found in geoid directory
   -3   def_shp.txt  is NOT a def_shp file
   +1   label NOT set from prj. projection NOT found.
   +2   label NOT set from prj. datum NOT found.
   +3   label NOT set from prj. params error
   +4   label NOT set from prj.

   coordinates unit is:deafault: "m",
                   for:geo: "degree" or "radian"
   sets geo_lab->u_c_lab.g_tpd
      "radian" ? (.gf ==2 \&\& .tf ==3) : (.gf ==2 \&\& .tf ==1)

#endif



/* fh_trans  ver 2010.1        # page 1    12 Jan 2010 13 55 */
/* -- Function 6 -----------------------------------------------------------------*/
int           fputcw (
FILE                *fp,
double               cw,
int                  mode
);

#if !0xD0C

fputcw        Number of characters written

fp            File pointer to an open file

cw            G.I. date_time, MD comment word or jnsd or ?

mode          mode == 0: digital date. mode == 1: month names



ISO (our)   1992 04 17, 09.45   or 17. Apr. 1992, 09.45
simple            1992
MD            1 00045 89 02 3 012   max < 900 00000 00 00 0 000

PROG: Frands Steffensen    1994

#endif



/*  fputg.h version 1.0               # page 1   22 Feb 1993 12 54 */

/* -- Function 6 -----------------------------------------------------------------*/
int               fputg (
FILE                    *f,
double                  val,
struct typ_dec          *typ,
char                    *layout
);


#if !0xD0C
 converts val acording to *typ and writes on FILE *f

   layout may be given :
   '\\0'   : default
   " "    : include blanks in output
   "0"    : start with zero
   "+"    : start with + (before possible zero)
   in any combination

#endif


/* fputshpprj  now based on sputshpprj, which is based on prev. verison of fputshpprj, simlk, oct 2011 */



/* -- Function 6 -----------------------------------------------------------------*/
int       fputshpprj (
FILE            *fh_out,
union geo_lab   *g_lab);

#if !0xD0C
 fputshpprj writes the .prj file to a shape file
   from the geo_lab.

   The reference names for label recognition is found
   in the file def_shp.txt in the geoid catalouge.

   fputshpprj returns::
   0    prj line written ok.
   -2   def_shp.txt  def file not found in geoid directory
   -3   def_shp.txt  is NOT a def_shp file
   -4   geo_lab unknown shape.prj format.
   -5   illegal geo_label.
   -6   program error.

   coordinates unit is:deafault: "m",
                   for:geo: "dg" or "rad"
   dependening geo_lab->u_c_lab.g_tpd
      (.gf ==2 \&\& .tf ==3) ? "rad" : "dg"

#endif

/* gatg.h      version 1.0          # page 1   10 Aug 1994 17 23 */
/* -- Function 6 -----------------------------------------------------------------*/
int                gd_t_val (
union geo_lab            *i_lab,
double                    N_in,
double                    E_in,
double                   *t_val,
struct mgde_str          *grid_tab,
char                     *eer_txt
);

#if !0xD0C
 i_lab      crd_sys of input N_in, E_in
 t_val      return value of table in (N_in, E_in)
 grid_tab   label and buffer initiated to table
 err_txt    return error report
#endif



/* gd_trans  ver 2010.1          # page 1   12 jan 2010 11 57 */

/* -- Function 6 -----------------------------------------------------------------*/
int               geoid_c (
struct mgde_str         *tab_table,
int                      info,
FILE                    *out
);

#if !0xD0C

 see allso geoid_d.h  and geoid_i.h

 closes the streams to the FILES used for access to the tables

 tab_table: the structure describing the use of the tables

 if info != 0 will the names of the tables be listed on the
      file pointed by *out
 if info > 1 will the statistic be listed

 PROG: Karsten Engsager, OCT 2001
#endif





/* XXX Some documentation here duplicated from grid_val.h ??? XXX */

/* -- Function 6 -----------------------------------------------------------------*/
int                grid_val (
union  geo_lab          *c_lab_u,
struct mgde_str         *tab_table,
double                   B_in,
double                   L_in,
double                  *tab_val,
char                    *usertxt
);

#if !0xD0C
grid_val  is a function giving the grid value in the defined system

The structure "mgde_str" serves as the work space for
grid_val. The user should declare a structure as e.g.

     static struct mgde_str        tab_table;

The statement of initializing once before first call:

     tab_table.init = 0;

DEFAULT : grid_val will initialize the geoids
without any other programming action needed.

The call of grid_val, now using and giving only euref89 values
is described in the prototype file grid_val.h


The return value is
zero     tab_val found.
> 0      recall the function with other euref89 datum (e.g. EE_)
< 0      error


The grid index in the array of structure mgde_str *tab_table
is found in the mtab structure : entry tab_nr,
enabling a report of the name of the actually used grid.

grid_val opens the streams to the tables, and the function geoid_c
closes again and writes information about the processing.
#endif


/* -- Function 6 -----------------------------------------------------------------*/
int               geoid_c (
struct mgde_str         *tab_table,
int                      info,
FILE                    *out
);

#if !0xD0C
geoid_c is IMPORTANT to call before releasing the struct mgde_str
        to close the file pointer used.
+++ IMPORTANT to release file pointers in system +++

The value of info determines the information given about
the use of the tables, so that
info == 0 suppresses the information and
info != 0 produces it.

The return value is 0 for ok and -1 for failure.

The number of GEOIDPAGES - here actually defined to 8 -
gives a hit-rate of mostly over 90 %. The size of the pages,
GEOIDPGSIZE, is mandatory.

The user may define a selection of a gridtable different from
the series of standard tables automatically initialized by geoid_h.
#endif

extern int               geoid_i(
char                    *geoid_n,
int                      geoid_tp,
struct mgde_str         *tab_table,
char                    *err_str
);

#if !0xD0C
geoid_tp gives the type of table (p.t.):
    GDE_LAB, DTM_LAB, DHH_LAB, UPL_LAB, DEM_LAB, TAB_LAB,
    BAG_LAB, T3D_LAB, T2D_LAB, TDD_LAB

FOR GEOIDS ONLY:
The name of the geoidfile may be empty or "STD" or
any other region defined (ex "DK") else
The name of the geoidfile is set in the parameter geoid_n. Only one
geoid is permitted. This geoid must be found in the stdin library.
If the position is outside the definition area of geoid_n, then
the standard ("STD")geoids are searched.

FOR OTHER TABLES:
The name of the tablefile may be empty or "STD" or
any other region defined (ex "DK") else
The name of the tablefile is set in the parameter geoid_n. Only one
grid is permitted. This grid must be found in the stdin library.
If the position is outside the definition area of geoid_n, then
NO OTHER tables are searched.

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

tab3d_val  is a function giving the grid value in the defined system

The structure "mtab3d_str" serves as the work space for
tab3d_val. The user should declare a structure as e.g.

     static struct mtab3d_str        tab_table;

The statement of initializing once before first call:

     tab_table.init = 0;

DEFAULT : tab3d_val will initialize the tab3ds
without any other programming action needed.

The call of tab3d_val, now using and giving only euref89 values
is described in the prototype file tab3d_val.h
#endif





/* geoid_t     ver 2001.01          # page 1    8 nov 2001 10 19 */

/* -- Function 6 -----------------------------------------------------------------*/
int               geoid_t (
struct gde_lab          *g_lab,
char                    *datum,
char                    *h_dtm,
char                    *descr
);

#if !0xD0C
 geoid_t(name, lab, descr) find the name of a table
                           and reads some parameters
    result ==  0      : grid lab
    result == -1      : name is not found
    result == -4      : manager.tab file error


 see also geoid_d.h
#endif



/* get_areacf                      # page 1    2 Feb 2004 14 02 */
/* -- Function 6 -----------------------------------------------------------------*/
int           get_mlb (
char                *mlb_str,
short               *region,
char                *mlb1,
short               *sepch,
char                *mlb2,
char               **h_mlb
);

#if !0xD0C
 get_mlb :: returns the number af characters in the label.

The items of the label *mlb_str are put into the parameters:

*region    :: the twoo region characters.
*mlb1      :: the kind/projection part is copied to *mlb1.
*sepch     :: the seperatior char is copied to *sepch.
*mlb2      :: the type/datum part is copied to *mlb2.
**h_mlb    :: the pointer to the HGT_mlb is put in **h_mlb.
           :: NULL when sep_ch != H or no h_lab

#endif



/* get_tpd.h                     # page 1   12 Jan 2010 13 14 */

/* -- Function 6 -----------------------------------------------------------------*/
int             get_tpd (
struct typ_dec        *gtfd,
int                   *tp,
char                  *name,
int                   *cfp,
int                   *dec,
double                *cf
);

#if !0xD0C
 See typ_dec.h for parameter description

get_tpd returns the group number (gr) or zero
of the typ_dec *gtfd.

*tp is the type
*name is the name string
*cfp is the number of characters before the point
*dec is the number of digits after the point
*fc is the conversion factor.

PROG:  KP 1989

#endif



/* get_zone    ver 2000.01          # page 1   19 jan 1999 10 19 */

/* -- Function 6 -----------------------------------------------------------------*/
int        getsinexlin (
char             *ftx,
int              *state,
FILE             *fh_in);


#if !0xD0C
 getsinexline reads a line from the open and ready file fh_in
 to the string ftx. The state gives info on which linetype is
 input and the return value is the number of chars input.

 getsinexline :: number of chars input to ftx
 *ftx         :: char array for one input line
 *state       :: input line type
 *fh_in       :: File pointer to an open and ready file


 *state       :: -9999 error line type
              :: 0<*state<200 :: begin block and info lines
              :: -200<*state<0 :: end block
              :: 1000<*state<1200 :: comment line
       ::   1 :: SINEX HEADER
       :: 101 :: FILE/REFERENCE
       :: 102 :: FILE/COMMENT
       :: 103 :: INPUT/HISTORY
       :: 104 :: INPUT/FILES
       :: 105 :: INPUT/ACKNOWLEDGMENTS
       :: 106 :: NUTATION/DATA
       :: 107 :: PRECESSION/DATA
       :: 108 :: SOURCE/ID
       :: 109 :: SITE/ID
       :: 110 :: SITE/DATA
       :: 111 :: SITE/RECEIVER
       :: 112 :: SITE/ANTENNA
       :: 113 :: SITE/GPS_PHASE_CENTER
       :: 114 :: SITE/GAL_PHASE_CENTER
       :: 115 :: SITE/ECCENTRICITY
       :: 116 :: SATELLITE/ID
       :: 117 :: SATELLITE/PHASE_CENTER
       :: 118 :: BIAS/EPOCHS
       :: 119 :: SOLUTION/EPOCHS
       :: 120 :: SOLUTION/STATISTICS
       :: 121 :: SOLUTION/ESTIMATE
       :: 122 :: SOLUTION/APRIORI
       :: 123 :: SOLUTION/MATRIX_ESTIMATE
       :: 124 :: SOLUTION/MATRIX_APRIORI
       :: 125 :: SOLUTION/NORMAL_EQUATION_VECTOR
       :: 126 :: SOLUTION/NORMAL_EQUATION_MATRIX
#endif



/* getsinexver  ver 2008.01        # page 1    1 Dec 2008 15 35 */
/* -- Function 6 -----------------------------------------------------------------*/
void        getsinexver (
char              *ftx,
int               *state,
int               *version);

#if !0xD0C
 getsinexver sets the version number to the text ftx

 *ftx         :: 1 :: SINEX HEADER
 *state       :: 1 :: at input
 version      :: number ver*100+serial to be written on ftx
#endif



/* gettabdir  ver 2001.01           # page 1   12 Dec 2001 17 23 */
/* -- Function 6 -----------------------------------------------------------------*/
char   *gettabdir (void);

#if !0xD0C
 gettabdir will return a pointer to a string (dir_grid) holding the
   directory of the tables.

#endif

/* -- Function 6 -----------------------------------------------------------------*/
char   *settabdir (char *new_dir);

#if !0xD0C
 settabdir will return a pointer to a string (dir_grid) holding the
   directory of the tables.

   if (new_dir == NULL || strlen(new_dir) == 0) a default
   string is copyed to the string (dir_grid).

   else is the content of new_dir copied to the string (dir_grid).

   DEFAULT STRINGS ::
      sparc || __sparc :  "/export/home/geoid/"
      K_LNX_P          :  "/bin/geoids/"
      else             :  "/geoids/"

#endif




/*  gmb1.h                            # page 1    1 Mar 1993 11:18 */
/* -- Function 6 -----------------------------------------------------------------*/
int                grid_val (
union  geo_lab           *c_lab_u,
struct mgde_str          *tab_table,
double                    N,
double                    E,
double                   *g_v,
char                     *err_str
);

#if !0xD0C
 see also grid_d.h  and  grid_i.h

 grid_val produces the table value from a table of gridded data.

The label be initialized by call of           grid_i.

Up to MAXGEOIDS geoid tables are initialized automatically.
Only one table may be open at a time.

The grid_val return value is::
    == 0  tab_val found.
     > 0  (no of wanted datum)
          recall the function with other euref89 datum (e.g. EE_)
     < 0  error
g_v       is the interpolated table value to the point N, E

The coordinates system c_lab_u must be the same as the table.
The system may be checked by the call ::
    table_adm -d -\#<table_name>

The coordinates are checked to be inside the table limits.

Closing of the streams is done by grid_c.

PROG: Karsten Engsager, OCT 2001.
#endif



/*  gtc.h                        # page 1   22 Feb 1993 12 54 */
/* -- Function 6 -----------------------------------------------------------------*/
FILE      *i_tabdir_file (
int               mode,
char             *name,
int              *result,
char             *dir_name
);

#if !0xD0C
 FILE *i_tabdir_file(mode, *name, *result, *dir_path)
      opens the file for reading
      some files are TESTED
      some GLOBAL PARAMS are initialized
  returns the file pointer opened ready for reading
 sevice routine for label and table system

    result ==  0 : ok
    result == -1 : name neither found in local nor geoid dir
    result == -2 : name may NOT be opened
    result == -3 : FILE TEST FAILED

    mode         : kind of file to be opened
    mode   ==  1 : manager.tab     TEST,  GLOBAL PAR:
                 :  *man_tab_file, init_tab_pos
    mode   ==  2 : manager.gps     TEST,  GLOBAL PAR:
                 :  *man_gps_file, init_gps_pos
    mode   ==  3 : def_lab.txt     TEST,  GLOBAL PAR:
                 :  *def_lab_file, init_lab_pos
                 :  init_prj_pos, init_rgn_pos
                 :  init_dtm_pos, init_grs_pos
    mode   ==  4 : binary file NO TEST
    mode   ==  5 : txt file  TEST: <name> (excl. extension)
    mode   ==  6 : txt file  NO TEST

   *name         : NOT USED in mode 1 | 2 |  3
                 : else the name of the file to be opened
   *dir_name     : path to the file ( == "" when local)
#endif



/* inpoly.h      version 1.1        # page 1   10 May 1994 08 57 */
/* -- Function 6 -----------------------------------------------------------------*/
int            labchsum (
union geo_lab        *u_lab,
int                  *int_sum
);

#if !0xD0C
labchsum: The (weightd) sum of the eleemnts of a label
u_lab   : The label to be summed

The sum must be inserted by the programme, e.g.:

      c_lab->ch_sum =  labchsum(u_lab);

Note, that the lab_type must be used in the LV.

PROG: Knud Poder, FEB 1995
#endif



/* LORD  ver 2013.01          # page 1     19 Feb 2013 11 57 */
#include  <stdio.h>


/* Pepper the format string with file(line) information */
#define STRINGIFY(x)  #x
#define TOSTRING(x)   STRINGIFY(x)
#define LORD(x)        __FILE__ "(" TOSTRING(__LINE__) "): " x

/* class code to distinguish type in call_back */
typedef enum
{LORD_DEBUG=0,
LORD_INFO=1,
LORD_WARNING=2,
LORD_ERROR=3,
LORD_CRITICAL=4} LORD_CLASS;

/*call back definition*/
typedef void( *LORD_CALLBACK )(LORD_CLASS, int , const char *);

/* -- Function 6 -----------------------------------------------------------------*/
double get_number (char *item);
/* -- Function 6 -----------------------------------------------------------------*/
def_data *open_def_data (FILE *fp, int *n_err);
/* -- Function 6 -----------------------------------------------------------------*/
void close_def_data (def_data *data);



/* pla_adm_f    ver 2003.01       # page 1   18 Feb 2003 14 13 */


/* -- Function 6 -----------------------------------------------------------------*/
short             prn_cho (
FILE                    *out,
union geo_lab           *d_lab,
double                  *crd,
struct typ_dec          *tpd,
char                    *o_str,
double                  *cmt,
double                  *old_cmt
);

#if !0xD0C
prn_cho writes a coordinate set acording to the label *d_lab
on the file *out.

*crd holds the coordinates (1 to 3)
*tpd holds the type decimal controling the output format
*cmt holds the commentwords
*old_cmt holds the last used comment_words.
*old_cmt equals *cmt at return.

*o_str == "\\0"  : no 'sp' and no unit in output (f.ex.: sx)
*o_str == " \\0" :             no unit in output
*o_str == "c\\0" : no 'sp'
*o_str == " c"  : both 'sp' and unit in output


#endif


/* proj4_to_mlb.h                  # page 1   DD Mmm 2012 hh mm */

/*Remember to free the returned pointer*/
/* -- Function 6 -----------------------------------------------------------------*/
proj4_entry *parse_proj4 (char *proj4_text);

/* -- Function 6 -----------------------------------------------------------------*/
int proj4_to_mlb (char *proj4_text, char *mlb);

/* setter function for flag to allow kms-datum extensions */
/* -- Function 6 -----------------------------------------------------------------*/
void set_kms_datums_allowed (int is_allowed);

/*normalise datum (use specific etrs89 alias)*/
/* -- Function 6 -----------------------------------------------------------------*/
void normalise_datum (char *datum);


/* ptg.h      version 2010.1        # page 1   12 Jan 2010 16 51 */

/* -- Function 6 -----------------------------------------------------------------*/
void putsinexval (char *ftx,  double val);


#if !0xD0C
 putsinexval sets the val to the text ftx

 *ftx         :: 1 :: SINEX HEADER
 val          :: Value to be writtena FORTRAN 21.15E
#endif



/* putsinexver  ver 2008.01        # page 1    1 Dec 2008 15 35 */
/* -- Function 6 -----------------------------------------------------------------*/
void        putsinexver (
char              *ftx,
int               *state,
int                version);

#if !0xD0C
 putsinexver sets the version number to the text ftx

 *ftx         :: 1 :: SINEX HEADER
 *state       :: 1 :: at input
 *state       :: -9999 error line type
 version      :: number ver*100+serial to be written on ftx
#endif



/* s34jtos.h  version 1.0  #          # page 1   22 Feb 1993 12 54 */
/* -- Function 6 -----------------------------------------------------------------*/
struct typ_dec      *set_tpd (
char                       *name,
int                         fwd,
int                         dec
);

#if !0xD0C
 See typ_dec,h for paremeter descriptions

set_tpd returns a type decimal as described in the string *name
fwd gives the number of digits before the decimalpoint
(a negative fwd gives a standard output)
and dec gives the number of decimals
(a negative dec gives zero decimals)

PROG: KP 1989
Karsten Engsager: updated 2010.
#endif



/* set_trc.c                        # page 1   10 Aug 1994 11 40 */

/* -- Function 6 -----------------------------------------------------------------*/
int set_trc (struct coord_lab  *c_lab);

#if !0xD0C
 Function:
set_trc             (call and return)            int
Produces the constants needed for a transformation,
where the coordinate system is described by a coord_lab.
The return value is the number of the coordinate system.
An erroneous c_lab gives the return value -1.

Parameter:
*c_lab             (call and return)           coord_lab
The constants needed for the transformation are inserted
in c_lab. A flattening given as inverse flattening is
converted to the flattening. E.g. 297 is converted to
the value of 1/297. The constants for transformation
geographical coordinates to/from Gaussian coordinates
are always inserted - even if not needed by the actual
coordinate system.

Prog: Knud Poder  1992.

#endif



/* sftr.h                        # page 1   22 Feb 1993 12 54 */
/* -- Function 6 -----------------------------------------------------------------*/
double           sgetcw (
char                   *out,
int                    *used,
int                    *term_c
);

#if !0xD0C
 function sgetcw : reading of comment word, G.I. date_time
and MD. long word.

sgetcw       Date_time stripped for special characters
             and MD do

out          Pointer to char array holding the cw

used         Number of characters read from out

term_c       Last char read, terminating char

The function reads date, time in the following format
ISO (our)   1992 04 17,  9.45  or
old (our)     17 04 92,  9.45  or
simpel            1992         or
(very old)          do             (not used anymore)

MD cw        1 00045 85 02 3 023

In case of 'do' the function returns -1, the user of this function
must take action in this case. (try prev. cw)

Upon error the function returns -2.

Due to the use of 'sscanf', and the way the cw is read, the
char array must not hold 'spaces'.

#endif



/* sgetdt                         # page 1    4 Jul 2003 13 18 */
/* -- Function 6 -----------------------------------------------------------------*/
double         sgetdt (
char                 *in,
double               *JD2000,
int                  *used,
int                  *term_c
);

#if !0xD0C
 function sgetdt : reading of comment word, G.I. date_time
and MD. long word.

sgetdt       (return and return)        double
_            Date stripped for special characters

in           (call)                     char *
_            Pointer to char array holding the cw

JD2000       (return)                  *double
_            Julian Date from JD 2000.0

used         (return)                  *int
_            Number of characters read from in

term_c       (return)                  *int
_            Last char read, terminating char

The function reads date, time in the following format
ISO (our)   1992 04 17,  9.45  or
ISO         1992 04 17

Upon error the function returns -2.0.

Due to the use of 'sscanf', and the way the cw is read, the
char array must not hold 'spaces'. Use fgetln_kms to this end.

#endif



/*  sgetg.h version 1.0               # page 1   22 Feb 1993 12 54 */
/* -- Function 6 -----------------------------------------------------------------*/
double               sgetg (
char                        *str,
struct typ_dec              *tpd,
int                         *used,
char                        *udt
);

#if !0xD0C
sgetg returns a geodetic value read from the string *str.
*tpd contains the type and decimal of the read value
*used is the number of characters read
*udt is a terminating type string, which is used when none is read

reading is stopped after two 'sp' or a terminating string

the values are :
for metric values     in units of metres
for angular values    in units of radians
for potential values  in units of gpu
for gravity values    in units of gal
for mean error values in units of 1

#endif

/*  sgetshpprj.h version 1.0               # page 1   DD Mmm 2012 hh mm */

/* -- Function 6 -----------------------------------------------------------------*/
int              sgetshpprj (
char            *wkt_in,
union geo_lab   *g_lab,
char            *mlb_out
);



/* sputcw.h version 1.0              # page 1    1 Mar 1993 11:18 */
/* -- Function 6 -----------------------------------------------------------------*/
int         sputcw (
char              *str,
double             cw,
int                mode
);

#if !0xD0C

sputcw       Number of characters written

str          pointer to a char array

cw           G.I. date_time, MD comment word or jnsd

mode         ==1 writes month in text (day month year, clock)
             ==3 writes month in text    and no clock at ISO
             ==0 writes month in number
             ==2 writes month in number  and no clock at ISO

ISO (our)   1992 04 17,  9.45
simple            1992
MD            1 00045 89 02 3 012   max < 900 00000 00 00 0 000

#endif


/* sputg.h version 1.0                # page 1   22 Feb 1993 12 54 */
/* -- Function 6 -----------------------------------------------------------------*/
int                 sputg (
char                       *outstr,
double                     val,
struct typ_dec             *typ,
char                       *layout
);

#if !0xD0C
 converts val acording to *typ and writes on out_str

   layout may be given :
   "\\0"   : default
   " "    : include blanks in output
   "0"    : start with zero
   "+"    : start with + (before possible zero)
   in any combination

#endif

/*  sputshpprj.h version 1.0               # page 1   DD Mmm 2012 hh mm */

/* -- Function 6 -----------------------------------------------------------------*/
int sputshpprj (char *wkt_out, union geo_lab *g_lab);

#if !0xD0C
 sputshpprj writes the .prj Esri wkt
   from the geo_lab.

   The reference names for label recognition is found
   in the file def_shp.txt in the geoid catalouge.

   sputshpprj returns::
   0    prj line written ok.
   -2   def_shp.txt  def file not found in geoid directory
   -3   def_shp.txt  is NOT a def_shp file
   -4   geo_lab unknown shape.prj format.
   -5   illegal geo_label.
   -6   program error.

   coordinates unit is:default: "m",
                   for:geo: "dg" or "rad"
   dependening geo_lab->u_c_lab.g_tpd
      (.gf ==2 \&\& .tf ==3) ? "rad" : "dg"

   The caller must ensure that *wkt_out is long enough to hold the output!

#endif



/* srch_plate  ver 2008.02          # page 1   16 May 2008 11 57 */

/* -- Function 6 -----------------------------------------------------------------*/
int               srch_tab_typ (
int                     *lab_typ,
char                    *tab_typ_txt,
long                    *pos
);

#if !0xD0C
 srch_tab_typ(mode, f777, *tab_typ_txt) searches the entry
 to a table type
  result  >  0  : tab_typ f777
  result == -2  : STOP or EOF

 *lab_typ       : Kind of table (or STP_LAB) See geo_lab.h

 *tab_typ_txt may be :: (acording to *lab_typ)
        "geoid", "dtmtab", "dhtab", "upltab", "demtab",
        "tabtab", "bagtab", "t3dtab", "t2dtab", "tddtab"
 *pos           : after "REC_TABLE" for next recursive call
#endif



/* strong.h version 1.0               # page 1   DD Mmm 2012 hh mm */
/* -- Function 6 -----------------------------------------------------------------*/
int               tab3d_c (
struct mtab3d_str       *tab_table
);

#if !0xD0C
 see also tab3d_d.h  and tab3d_i.h

 closes the streams to the FILE used for access to the table

 tab_table: the structure describing the use of the table

 PROG: Karsten Engsager, JUN 2004
#endif


/* tab3d_i  ver 2001           # page 1   24 Jun 2004 11 37 */
/* -- Function 6 -----------------------------------------------------------------*/
int               tab3d_i (
char                    *tab3d_n,
int                      tab3d_tp,
struct mtab3d_str       *tab_table,
char                    *err_str
);
#if !0xD0C

 tab3d_i must be called for all kinds of 2d or 3d tables
 before any call of tab3d_val


 see also tab3d_d.h

 tab3d_i prepares the table given by the tab3d_n name for
 further calls of tab3d_val
 the stream to the table is opened and initialization of the
 table labels are performed automatically.
 Closing of the stream is done by tab3d_c.

 Hint to the user ::
 T3D_LAB (tab3d tables) will open the tab3d
 T2D_LAB (tab2d tables) will open the tab2d
 When more kinds of tables are used simultaineously it is
 recommended to used seperate mtab3d_structures for each

 tab3d_i return value :
   > 0          :: number of tables initialized
  == TRF_G_NON_ :: number of tables initialized
  == TRF_G_MAX_ :: too many tables to initialize

 tab3d_i: Initializes a structure containing one tab3d
 administration and array locations for pages of tables,
 and variables needed for computation of table values.

 for tab3ds optionally ::
 tab3d_n:  Pointer to the full path and name of a file con-
           taining a table.
 If tab3d_n is empty or is "STD" the standard is used.
 for non_tab3ds tables mandatory ::
 tab3d_n:  Pointer to the full path and name of a file con-
           taining a table.

 tab3d_tp:  must be one of the types (se geo_lab.h)::
                T3d_LAB, T2d_LAB

 tab_table: Pointer to a structure (defined in geo_lab.h)
           containing:
     TAB3DPAGES pages holding parts of the tables
           after a fifo-strategy. A page has room for
     TAB3DPAGESIZE floats.
     tab_max tab3d_labels. (MAX_GEOIDS)
           containing (1) the name and path of the file with
           the table, (2) a table-label, (3) a FILE variable
           for the stream of the file (the file is open by
           the call), (4) a counter for number of calls for
           data, and (5) a boolean for the existence
           Note, that the streams should be closed after the
           use by means of the function tab3d_c

 err_str:  will hold the error explanation when errors occur


 FILE USED IN THE MANAGING OF TABLES ::

 manager.tab is a file placed in the same catalog as the
           grids. I must contain the official information
           on grid names and search sequences

 example of manager.tab file ::
   #tab3d   manager   977         :: info for identification
   REC_TABLE                      :: start of tab3d_names
   aitken.04  2  0  4  3  geo_euref89  "DK98/1    1.5x3.0 mn"
   stop                           :: last of tab3d_names
   :: each line contains name  cmplt  global
   ::     (length of float)  decimals  projection   text

   STD  DK  FE                    :: list of preceedencies

   STD                            :: start of standard
   aitken.04
   stop                           :: last of standard

   DK                             :: start of Danmark
   aitken.04
   stop                           :: last of Danmark

   FE                             :: start of Faroes
   stop                           :: last of Faroes

   #stop    manager

 example of manager.tab file :: FINISHED

 A private manager.tab may allso exist.
 and the official one is not cunsulted
 The private manager.tab is used to name a new tab3d ::
 example of private manager.tab file ::
   #tab3d   manager   977         :: info for identification
   REC_TABLE                      :: start of tab3d_names
   aitken.04  2  0  4  3  geo_euref89  "DK98/1    1.5x3.0 mn"
   stop                           :: last of tab3d_names

   STD                            :: list of preceedencies

   STD                            :: start of standard
   stop                           :: last of standard

           The last part of mgde_str contains variables used
           for i/o, numbering etc. See geo_lab.h for the
           valid definitions.

  int                  hit;           tab3d-height in page
  int                  agn;           tab3d-height to page
  int                  xmt;           hole in tab3d-area
  long                 pge[GEOIDPAGES]; act. stream for page
  int                  sgd[GEOIDPAGES]; act. nmb. of tab3d
  int                  victim;          fifo choice of page
  int                  tab_max;         number of tab3ds

  Params set by the progr. or funct. calling the trnsf. func.
  int                  init;            0 => initialize
  int                  tab_nr;           actual tab3d


 Prog: Knud Poder, JULY 1997, updated OCT 1997, upd JUN 1998
 Prog: Karsten Engsager, OCT 2001.
#endif



/* tab3d_val  ver 2004.01           # page 1   25 Jun 2004 11 37 */
/* -- Function 6 -----------------------------------------------------------------*/
int                tab3d_val (
union  geo_lab           *c_lab_u,
struct mtab3d_str        *tab3d_table,
double                    B_in,
double                    L_in,
double                   *val,
char                     *usertxt
);

#if !0xD0C

 see also tab3d_d.h                         (for GEOIDPATH)

 tab3d_val produces the 3d_val from the table covering part
of the ellipsoid.

val      is the returned 3d_valus: (N,E,H) or (X,Y,Z)

The stream to the table must be opened by tab3d_i.
Closing of the streams is done by tab3d_c.

PROG: K. Engsager, JUN 2004.

#endif



/* tab_adm_f                       # page 1   17 Dec 2001 22 00 */

/* -- Function 6 -----------------------------------------------------------------*/
int           tab_adm_f (
  char *tab_name,    /*  name of table                                    */
  char  mode,        /* mode = 'b' / 't' / 'd' / 'z'                      */
  int   header,      /* header (in i/o) = 0/1  recommended == 1           */
  int   hd_limits,   /* hd_limits in (i/file) = 0/1  always == 1          */
  char *global_dir,  /* directory for tables                              */
  FILE *in_file,     /* table is read from in_file (or stdin)        */
  FILE *outfile,     /* control output is written on outfile (or stdout)  */
  char *errtx        /* hard errors                                       */
);

#if !0xD0C
 tab_adm_f is a maintenance tool for tables and geoids

 the label must be found in manager.tab
           holding the limits and steps
           which are checked against the file

 CALL  ::
 (int) tab_adm_f(
   char *tab_name,  name of table
   char  mode,      mode = 'b' / 't' / 'd'
   int   header,    = 0/1  recommended == 1
   int   hd_limits, = 0/1  recommended == 1
   char *global_dir, directory for tables
   FILE *in_file,   table is read from in_file (or stdin)
   FILE *outfile,   control output is written on outfile (or stdout)
   char *errtx)     string for error

 mode == 'b'
 produces a binary table in the file tab_name
   from the file fi, controloutput to fo
 format of input file : usual as given by Rene Forsberg

 mode == 'z'
 as -b but all values are set NILL (== 9999.99)
       the table in fi is not read (only the limits e.t.c.)

 mode == 't'
 produces a text table in the file out
   from the binary file tab_name, controloutput to fo
 format of output file : with header  (== 1)

 mode == 'd'
 produces a documentation of the table
   from the binary file tab_name, controloutput to fo

 progr. K.Engsager, Dec, 2001,
#endif



/* tab_doc_f     ver 2009.01          # page 1    26 Mar 2009 10 19 */

/* -- Function 6 -----------------------------------------------------------------*/
int               tab_doc_f (
char                    *tab_name,
FILE                    *fh_out
);

#if !0xD0C
 tab_doc_f(name, fh_out) find the name of a table
                        and writes some parameters on fh_out
    result  >  0      : grid lab ok (==lab_type, geo_lab.h)
    result ==  0      : STOP recursive call
    result == -1      : name is not found
    result == -2      : manager.tab NOT FOUND
    result == -3      : unknown first char in name

   ?<NAME>   =>  Table name, short description
   *<NAME>   =>  Table name, detail description

   ?<TYPE>   =>  Table name, short description  (recursive)
   *<TYPE>   =>  Table name, detail description (recursive)

   ?         =>  Table name, short description  (recursive)
   *         =>  Table name, detail description (recursive)

   !         =>  works as ?

 Recursive calls produces information for one table in each
 call until result == 1. Continuing after this will restart
 the loop.

 <TYPE> may be ::
        "geoid", "dtmtab", "dhtab", "upltab", "demtab",
        "tabtab", "bagtab", "t3dtab", "t2dtab", "tddtab"

 see also geoid_d.h
 progr. K.Engsager, Mar, 2009,
#endif


/* tabdd_c                         # page 1   25 Jul 2006 17 25 */
/* -- Function 6 -----------------------------------------------------------------*/
int               tabdd_c (
struct mtabdd_str       *tab_table
);

#if !0xD0C
 see allso geoid_d.h  and tabdd_i.h

 closes the streams to the FILE used for access to the table

 tab_table: the structure describing the use of the table

 PROG: Karsten Engsager, JUL 2006
#endif


/* tabdd_i  ver 2006           # page 1   24 Jul 2006 11 37 */
/* -- Function 6 -----------------------------------------------------------------*/
int               tabdd_i (
char                    *tabdd_n,
int                      tabdd_tp,
struct mtabdd_str       *tab_table,
char                    *err_str
);

#if !0xD0C

 tabdd_i must be called for double kinds of 2d tables
 before any call of tabdd_val


 see allso tabdd_d.h

 tabdd_i prepares the table given by the tabdd_n name for
 further calls of tabdd_val
 the stream to the table is opened and initialization of the
 table labels are performed automatically.
 Closing of the stream is done by tabdd_c.

 Hint to the user ::
 TDD_LAB (tabdd tables) will open the tabdd
 When more kinds of tables are used simultaineously it is
 recommended to used seperate mtabdd_structures for each

 tabdd_i return value :
   > 0          :: number of tables initialized
  == TRF_G_NON_ :: number of tables initialized
  == TRF_G_MAX_ :: too many tables to initialize

 tabdd_i: Initializes a structure containing one tabdd
 administration and array locations for pages of tables,
 and variables needed for computation of table values.

 for tabdds optionally ::
 tabdd_n:  Pointer to the full path and name of a file con-
           taining a table.
 If tabdd_n is empty or is "STD" the standard is used.
 for non_tabdds tables mandatory ::
 tabdd_n:  Pointer to the full path and name of a file con-
           taining a table.

 tabdd_tp: must be the type (se geo_lab.h):: TDD_LAB

 tab_table: Pointer to a structure (defined in geo_lab.h)
           containing:
     TABDDPAGES pages holding parts of the tables
           after a fifo-strategy. A page has room for
     TABDDPAGESIZE doubles.
     tab_max tabdd_labels. (MAX_GEOIDS)
           containing (1) the name and path of the file with
           the table, (2) a table-label, (3) a FILE variable
           for the stream of the file (the file is open by
           the call), (4) a counter for number of calls for
           data, and (5) a boolean for the existence
           Note, that the streams should be closed after the
           use by means of the function tabdd_c

 err_str:  will hold the error explanation when errors occur


 FILE USED IN THE MANAGING OF TABLES ::

 manager.tab is a file placed in the same catalog as the
           grids. I must contain the official information
           on grid names and search sequences

 example of manager.tab file ::
   #tabdd   manager   977         :: info for identification
   REC_TABLE                      :: start of tabdd_names
   aitken.04  2  0  4  3  geo_euref89  "DK98/1    1.5x3.0 mn"
   stop                           :: last of tabdd_names
   :: each line contains name  cmplt  global
   ::     (length of float)  decimals  projection   text

   STD  DK  FE                    :: list of preceedencies

   STD                            :: start of standard
   aitken.04
   stop                           :: last of standard

   DK                             :: start of Danmark
   aitken.04
   stop                           :: last of Danmark

   FE                             :: start of Faroes
   stop                           :: last of Faroes

   #stop    manager

 example of manager.tab file :: FINISHED

 A private manager.tab may allso exist.
 and the official one is not cunsulted
 The private manager.tab is used to name a new tabdd ::
 example of private manager.tab file ::
   #tabdd   manager   977         :: info for identification
   REC_TABLE                      :: start of tabdd_names
   aitken.04  2  0  4  3  geo_euref89  "DK98/1    1.5x3.0 mn"
   stop                           :: last of tabdd_names

   STD                            :: list of preceedencies

   STD                            :: start of standard
   stop                           :: last of standard


           The last part of mgde_str contains variables used
           for i/o, numbering etc. See geo_lab.h for the
           valid definitions.

  int                  hit;           tabdd-height in page
  int                  agn;           tabdd-height to page
  int                  xmt;           hole in tabdd-area
  long                 pge[GEOIDPAGES]; act. stream for page
  int                  sgd[GEOIDPAGES]; act. nmb. of tabdd
  int                  victim;          fifo choice of page
  int                  tab_max;         number of tabdds

  Params set by the progr. or funct. calling the trnsf. func.
  int                  init;            0 => initialize
  int                  tab_nr;           actual tabdd


 Prog: Karsten Engsager, JUL 2006.
#endif


/* tabdd_val  ver 2006.01           # page 1   25 Jun 2006 11 37 */
/* -- Function 6 -----------------------------------------------------------------*/
int                tabdd_val (
union  geo_lab           *c_lab_u,
struct mtabdd_str        *tabdd_table,
double                    B_in,
double                    L_in,
double                   *val,
char                     *usertxt
);

#if !0xD0C
 see allso geoid_d.h                         (for GEOIDPATH)

 tabdd_val produces the 2d_val (double) from the table covering part
of the ellipsoid.

val      is the returned 2d_valus: (N,E) or (X,Y)

The stream to the table must be opened by tabdd_i.
Closing of the streams is done by tabdd_c.

PROG: K. Engsager, JUN 2006.

#endif






/* geoid_i  ver 2001           # page 1   31 Oct 2001 11 37 */
/* -- Function 6 -----------------------------------------------------------------*/
int               geoid_i (
char                    *geoid_n,
int                      geoid_tp,
struct mgde_str         *tab_table,
char                    *err_str
);

#if !0xD0C
 geoid_i must be called for all kinds of tables except geoids
 before any call of grid_val
 goids are initialized automatically
 grid_val will call geoid_i("std", GDE_LAB, tab_table, err_str)
 when tab_table->init == 0  !!!


 see allso geoid_d.h                         (for dir_geoid)

 geoid_i prepares the table given by the geoid_n name for
 further calls of grid_val
 the stream to the table is opened and initialization of the
 table labels are performed automatically.
 Closing of the stream is done by geoid_c.

 Hint to the user ::
 GDE_LAB (geoid tables) will open tab_max geoids
 DTM_LAB (Digital Terrain Model tables) will open one table
 DEM_LAB (Digital Ellevation Model tables) will open one tab
 DHH_LAB (Diff Height model tables) will open one table ONLY
 UPL_LAB (UPLift model tables) will open one table ONLY
 TAB_LAB (unspecified TABles) will open one table ONLY
 When more kinds of tables are used simultaineously it is
 recommended to used seperate mgde_structures for each kind

 geoid_i return value :
   > 0          :: number of tables initialized
  == TRF_G_NON_ :: number of tables initialized
  == TRF_G_MAX_ :: too many tables to initialize

 geoid_i: Initializes a structure containing up to MAXGEOIDS
 (pt. 9, see geoid_d.h) table labels, administration and
 array locations for pages of tables, and variables needed
 for computation of table values.

 for geoids optionally ::
 geoid_n:  Pointer to the full path and name of a file con-
           taining a table. (outside the definition area
           the standard geoids are used).
 If geoid_n is empty or is "STD" the standard is used.
 for non_geoids tables mandatory ::
 geoid_n:  Pointer to the full path and name of a file con-
           taining a table.

 geoid_tp:  must be one of the types (se geo_lab.h)::
                GDE_LAB, HML_LAB, DHM_LAB, UPL_LAB, TAB_LAB

 tab_table: Pointer to a structure (defined in geo_lab.h)
           containing:
     GEOIDPAGES pages holding parts of the tables
           after a fifo-strategy. A page has room for
     GEOIDPAGESIZE floats.
     tab_max geoid_labels. (MAX_GEOIDS)
           containing (1) the name and path of the file with
           the table, (2) a table-label, (3) a FILE variable
           for the stream of the file (the file is open by
           the call), (4) a counter for number of calls for
           data, and (5) a boolean for the existence
           Note, that the streams should be closed after the
           use by means of the function geoid_c

 err_str:  will hold the error explanation when errors occur


 FILE USED IN THE MANAGING OF TABLES ::

 manager.tab is a file placed in the same catalog as the
           grids. I must contain the official information
           on grid names and search sequences

 example of manager.tab file ::
   #geoid   manager   777         :: info for identification
   REC_TABLE                      :: start of geoid_names
   dk98g.01   2  0  4  3  geo_euref89  "DK98/1    1.5x3.0 mn"
   fe95g.01   2  0  4  3  geo_euref89  "Faroes    1.5x3.0 mn"
   nkg96g.01  2  0  4  3  geo_euref89  "NKG96/1   1.5x3.0 mn"
   egm96g.01  2  1  4  1  geo_wgs84    "EGM96  0.5x0.5dg  1 m"
   stop                           :: last of geoid_names
   :: each line contains name  cmplt  global
   ::     (length of float)  decimals  projection   text

   STD  DK  FE                    :: list of preceedencies

   STD                            :: start of standard
   dk98g.01
   fe95g.01
   nkg96g.01
   egm96g.01
   stop                           :: last of standard

   DK                             :: start of Danmark
   dk98g.01
   nkg96g.01
   egm96g.01
   stop                           :: last of Danmark

   FE                             :: start of Faroes
   fe95g.01
   egm96g.01
   stop                           :: last of Faroes


   #dhtab   manager   775
   dhdnn50.01    2   0   4   3   s34j  "GM91 NY-GL 200x200m"
   stop

   *
   name        COM GLO size dec datum    "descr"
                PL BAL
   ;

   * list of tables of precedenses (STD is mandatory);

   STD

   * STD is mandatory precedense of grids

   STD
   dhdnn50.01
   stop

   #stop    manager

 example of manager.tab file :: FINISHED

 A private manager.tab may allso exist.
 and the official one is not cunsulted
 The private manager.tab is used to name a new geoid ::
 example of private manager.tab file ::
   #geoid   manager   777         :: info for identification
   REC_TABLE                      :: start of geoid_names
   dk20g.01   2  0  4  3   euref89  "DK2000    1.5x3.0 mn"
   stop                           :: last of geoid_names

   STD                            :: list of preceedencies

   STD                            :: start of standard
   stop                           :: last of standard



           The last part of mgde_str contains variables used
           for i/o, numbering etc. See geo_lab.h for the
           valid definitions.

  int                  hit;           geoid-height in page
  int                  agn;           geoid-height to page
  int                  xmt;           hole in geoid-area
  long                 pge[GEOIDPAGES]; act. stream for page
  int                  sgd[GEOIDPAGES]; act. nmb. of geoid
  int                  victim;          fifo choice of page
  int                  tab_max;         number of geoids

  Params set by the progr. or funct. calling the trnsf. func.
  int                  init;            0 => initialize
  int                  tab_nr;           actual geoid


 Prog: Knud Poder, JULY 1997, updated OCT 1997, upd JUN 1998
 Prog: Karsten Engsager, OCT 2001.
#endif













/* tcts_u.h  version 2.0             # page 1   01 Jul 1994 11 02 */
/* -- Function 6 -----------------------------------------------------------------*/
int                   tpd_doc_f (
char                        *head_txt,
char                        *tpd_txt
);

#if !0xD0C
 tpd_doc_f returns the tpd_group number (> 0)
 tpd_doc_f should be called until tpd_doc_f == 0

 head_txt descibes the tpd type
          (to be written when a new group number appear)
 tpd_txt  descibes the tpd

 head_txt must be declared at least 128 chars
 tpd_txt  must be declared at least 128 chars

 Progr. Karsten Engsager jan 2010
#endif

/* tr_DSFL.h  version ????.?           # page 1   DD Mmm YYYY hh mm */


/* -- Function 6 -----------------------------------------------------------------*/
int tr_DSFL(char *MainFilePath, char *SlaveFilePath,
                   union geo_lab *g_lab_slave, char* MsgStr);

#if !0xD0C
MainFilePath contain the name of the input DSFL file

SlaveFilePath contain the name of the output DSFL file

g_lab_slave the pointer to the output coordinate system (incl. Height)

MsgStr      contain error messages if any

tr_DSFL return value int:
          0:  transformation ok.
     TRF_XX:  an error from Transformation system see geo_lab.h
        -50:  inputfile not opened: STOP
        -51:  outputfile not opened: STOP
        -52:  FileFormat is probably NOT DSFL !
        -53:  DSFL command %H1 twice
        -54:  DSFL command %H2 twice
        -55:  DSFL command %H3 twice
        -56:  DSFL command %H4 twice
        -57:  DSFL command %H5 twice
        -58:  H1 command : unknown DSFLlabel
        -59:  H2 command : unknown DSFL_Hlabel
        -60:  NO CRD label in input file
        -61:  DSFL command %H3 before %H1
        -62:  %H3: S34 layout should be YX or XY
        -63:  %H3: UTM layout should be NE or EN
        -64:  %H3: layout error
        -65:  Main system NOT ok
        -66:  Slave system NOT ok
        -67:  Height label found in one system only
        -68:  Syntax error or Transfor,ation error
        -69:  Error in H1/H2/H3 in DSFL-Format

example of call:
  conv_lab("utm32Heuref89_h_dvr90", \&i_lab_a);
  res = tr_DSFL("ind", "ud", \&i_lab_a, err_txt);
  if (res) fprintf(stdout, err_txt);

#endif










/* u32_to_map.h                     # page 1   04 Jul 1997 12 34 */
#define FALSE 0
#define TRUE  1

/* -- MODULE GRIM ------------------------------------------- */
#if !0xD0C
%\summary{TRLIB Grid Interpolation and Memory Mapping}
%\author{Thomas Knudsen}
%\date{2012--2013}
%.FILE GRIMpre.tex \renewcommand{\reportnumber}{GRIM}
#endif

#if !0xD0C
%-----------------------------------------------------
\SECTION{Synopsis}
%-----------------------------------------------------

    The GRIM module handles memory mapping of grid files, and provides
    a simplistic set of functions for obtaining bilinearly interpolated
    grid values.

    Hence, you may conveniently think of GRIM as an implementation of
    disk based (i.e. persistent) arrays with non-integral indexing.


%-----------------------------------------------------
\SECTION{Lineage, Licence and Copyright}
%-----------------------------------------------------

GRIM is based on the BooGIE grid implementation boogrid.c,
and counts among its ancestors code written for my masters thesis
(around 1991 - ufgrid.c), and PhD-project (1998 - busgrid.c, grin.c).

Originally, these files were (C) by Thomas Knudsen, and distributed
under the GNU General Public Licence (GPL) v. 2.

Recently (fall 2012), I changed the licence to ISC, for compatibility with trlib.
Hence, this somewhat convoluted copyright notice:

* Copyright (C) 1991-2012 Thomas Knudsen [knudsen.thomas at gmail dot com]
*
* Copyright (C) 2012 National Survey and Cadastre, Denmark
* (Kort- og Matrikelstyrelsen), kms@kms.dk
*
* Copyright (C) 2013 Danish Geodata Agency
* (Geodatastyrelsen), gst@gst.dk
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


%-----------------------------------------------------
\SECTION{Compilation examples}
%-----------------------------------------------------

%. thokn@SIT1130565 ~/2012/Playground

. gcc  -I ../../trlib/TR_INC/ -I .  -std=gnu99 -xc -W -Wall -W -pedantic -o grim grim.c
. gcc  -I ../../trlib/TR_INC/ -I .  -W -Wall -W -pedantic -DTEST_GRIM -DWITH_MMAP -o grim grim.c -lmman
.
. gcc  -I ../../trlib/TR_INC/ -I .  -W -Wall -W -pedantic -DTEST_GRIM -o grim grim.c -lmman

%.    g c c  -std=c99 -xc -W -Wall -W -pedantic -DESRITEST  -o geoideinterpolationstest geoideinterpolationstest.c
%---------------------------------------------------------------------------


%-----------------------------------------------------
\SECTION{Generic example}
%-----------------------------------------------------
\lstset{style=datatype}%
\begin{lstlisting}[language=C,breaklines,morekeywords={size_t, GRIM, assert}]
#include <stdio.h>      /*  for printf and friends  */
#include <stdlib.h>     /*  for malloc/free         */
#include <ctype.h>      /*  for isspace()           */
#include <math.h>       /*  for HUGE_VAL            */
#include "grim.h"

#include <assert.h>
int main (void) {
    GRIM g;
    size_t i, n;
    double v, r, c, N, E, *w;

    g = grim_open ("./8chtest.pom");
    assert (0!=g);


    /* 1: test reading of a single specific channel from a record */


    /* 1.1: channel 0 at northing 2, easting 0 */
    v = grim_value (g, 2, 0, 0, 0);
    printf ("v1 = %g\n", v);

    /* 1.2: channel 7 at northing 2, easting 0 */
    v = grim_value (g, 1.9, 1.3, 7, 0);
    printf ("v7 = %g\n", v);


    /* 2: test reading of a full multi channel record */


    /* 2.1: make room for 1 record. The number of channels is specified by the GRIM g */
    w = grim_allocate_record_buffer (g, 1);

    /* 2.2: read the interpolated record */
    grim_values (g, 0, 0, 1, w);

    /* 2.3: first now, we need to worry about how many channels are really in the record */
    n = grim_channels (g);
    for (i = 0; i < n; i++)
        printf ("w[%d] = %g\n", (int)i, w[i]);


    /* 3: Test affine functions */


    r = grim_row (g, 0, 0);
    assert (r==2);
    r = grim_row (g, 0, 1);
    assert (r==2);
    r = grim_row (g, 1, 1);
    assert (r==1);

    c = grim_column (g, 1, 0);
    assert (c==0);
    c = grim_column (g, 1, 0.5);
    assert (c==0.5);
    c = grim_column (g, 1, 1);
    assert (c==1);

    E = grim_easting (g, 2, 2);
    assert (E==2)
    E = grim_easting (g, 0, 0);
    assert (E==0);

    N = grim_northing (g, 0, 0);
    assert (N==2);
    N = grim_northing (g, 2,2);
    assert (N==0);


   /* 4: Clean up */


   grim_free_record_buffer (w);
   grim_close (g);
   return 0;
\end{lstlisting}
#endif


/* -- GROUP Data types -------------------------------------------------------- */

/* -- Datatype -----------------------------------------------------------------*/
typedef struct  GRIM  *   GRIM;
#if !0xD0C
The control object for the GRIM module. Note that
it is designed to be an opaque object: In the header,
only the forward declaration of the struct GRIM is
given. The actual declaration of struct GRIM is
in the grim.c source code, out of sight from application
programs. Hence avoiding accidental tampering with the
delicate contents of the struct GRIM.

Note that the typedef is to a \emph{pointer} to struct GRIM.
This is contrary to the archetypal Posix stdio ``FILE'' typedef,
where a "pointer to FILE" is returned by fopen.
It is, however, in accordance with the zlib compressed i/o
library, where gzopen() returns a gzFile, not a \emph{gzFile *} directly.
(cf. \url{http://www.zlib.net/})
#endif


/* -- GROUP File handling ------------------------------------- */


/* -- Function ----------------------------------------------------------------- */
GRIM grim_open (char *pomfilename);

#if !0xD0C
Get ready to use grid data described in the pom file \emph{pomfilename}
= pomfilename name of pom file. The pom file contains the description (\emph{metadata}) for the grid file we want to access (its row and column counts, its spatial extent, the numberof channels, etc.)
= END
#endif




/* -- Function ----------------------------------------------------------------- */
GRIM grim_close (GRIM g);
#if !0xD0C
Clean up everything assoicated with \lstinline!GRIM g!
(i.e. release allocated menory and memory mapping, close open files, etc.)
= g a GRIM object originally returned by grim_open().
= END
#endif

/* get either one or all channel values from an interpolated point in a multichannel grid */


/* -- GROUP Data access -------------------------------------------------------- */ 


/* -- Function ----------------------------------------------------------------- */
int grim_values (const GRIM g, double northing, double easting, size_t extrapolation_method, double *record);
#if !0xD0C
Access the interpolated values for the grid referred to by g,
at a given point of interest,

= g a GRIM object originally returned by grim_open().
= northing the northing coordinate for the point of interest.
= easting  the easting coordinate for the point of interest.
= extrapolation_method determine how points of interest
       outside the grid coverage should be handled.
       Currently only two rather simplistic methods are supported:
       \begin{description}
       \item [extrapolation_method = 0] return nodata
       \item [extrapolation_method > 0] clamp point of interest to nearest grid boundary value.
       \end{description}
= record a pointer to an array of doubles at least as large as the number
      of channels of the grid represented by \textbf{g}.
      A suitably large array can be obtained from a call to grim_allocate_record_buffer().
= END
%-----------------------------------------------------
#endif

/* -- Function ----------------------------------------------------------------- */
double grim_value (const GRIM g, double northing, double easting, size_t channel, size_t extrapolation_method);
#if !0xD0C
Return a the value for a single channel of the grid
referred to by g, at a given point of interest.
= g a GRIM object originally returned by grim_open().
= northing the northing coordinate for the point of interest.
= easting  the easting coordinate for the point of interest.
= extrapolation_method determine how points of interest
       outside the grid coverage should be handled.
       Currently only two rather simplistic methods are supported:
       \begin{description}
       \item [extrapolation_method = 0] return nodata
       \item [extrapolation_method > 0] clamp point of interest to nearest grid boundary value.
       \end{description}
= END
%-----------------------------------------------------
#endif

/* -- GROUP Memory management -- */ 

/* companions to "grim_values()" above: allocate a properly sized buffer */

/* -- Function ----------------------------------------------------------------- */
double * grim_allocate_record_buffer (const GRIM g, size_t n);
#if !0xD0C
Allocate a buffer large enough to hold \emph{n} instances of
interpolated values for all channels of the grid referred to by g.

Primarily intended for use in connection with \emph{grim_values()},
in which case \emph{n} should be 1.
= g name of pom file
= n number of records to allocate room for
= END
#endif

/* -- Function ----------------------------------------------------------------- */
void grim_free_record_buffer (double *p);
#if !0xD0C
Companion to \emph{grim_allocate_record_buffer}: free
the memory allocated. Currently this is a simply a masked
call to the system supplied \emph{free()} function.
= p pointer to buffer allocated by \emph{grim_allocate_record_buffer}
= END
#endif

/* -- Function ----------------------------------------------------------------- */
size_t grim_channels (const GRIM g);
#if !0xD0C
Return the number of channels in the grid referred to by \emph{g}.
= g a GRIM object returned by \emph{grim_open()}
= END
#endif

/* -- GROUP Affine transformations between grid coordinates and world coordinates -- */ 

/* affine transformations from grid coordinates to world coordinates */

/* -- Function ----------------------------------------------------------------- */
double grim_northing (const GRIM g, double row, double column);
#if !0xD0C
Return the northing value of a given set of (possibly fractional)
row and column values
= g the GRIM object for the data set
= row the (possibly fractional) row number
= column the (possibly fractional) column number
= END
#endif

/* -- Function ----------------------------------------------------------------- */
double grim_easting  (const GRIM g, double row, double column);
#if !0xD0C
Return the easting value of a given set of (possibly fractional)
row and column values
= g the GRIM object for the data set
= row the (possibly fractional) row number
= column the (possibly fractional) column number
= END
#endif

/* affine transformations from world coordinates to grid coordinates */
/* -- Function -----------------------------------------------------------------*/
double grim_row      (const GRIM g, double northing, double easting);
#if !0xD0C
Return the (possibly fractional) row value of a given set
of northing and easting values.
= g            the GRIM object for the data set
= northing     the northing
= easting      the easting
= END
#endif

/* -- Function -----------------------------------------------------------------*/
double grim_column   (const GRIM g, double northing, double easting);
#if !0xD0C
Return the (possibly fractional) column value of a given set
of northing and easting values.
= g           the GRIM object for the data set
= northing    the northing
= easting     the easting
= END
#endif




/* -- MODULE STRONG ------------------------------------------- */
#if !0xD0C
.FILE STRONGpre.tex \renewcommand{\reportnumber}{STRONG}
%\summary{TRLIB Stronger String Functions}
%\author{Jakob Jakobsen and Simon Lyngby Kokkendorff}
%\date{2012--2013}
#endif
/* -- Function 8 -----------------------------------------------------------------*/
char ** get_items (char *line, char *delimiter, int *item_count);
/* -- Function 8 -----------------------------------------------------------------*/
void remove_whitespace (char *text, char *out);
/* -- Function 8 -----------------------------------------------------------------*/
double get_number (char *item);
/* -- Function 8 -----------------------------------------------------------------*/
char ** append_items (char **items1, char **items2, int n_items1, int n_items2);
/* -- Function 8 -----------------------------------------------------------------*/
void string_tolower (char *text);
/* -- Function 8 -----------------------------------------------------------------*/
char * path_basename (char *full_path);
#if !0xD0C
Stronger string functions
#endif


/* t_status.h                       # page 1   02 Oct 1995 13 26 */

/* -- MODULE LORD ------------------------------------------- */
#if !0xD0C
%\summary{TRLIB Logging, Reporting and Debugging}
%\author{Jakob Jakobsen and Simon Lyngby Kokkendorff}
%\date{2012--2013}
%.FILE LORDpre.tex \renewcommand{\reportnumber}{LORD}
#endif
/* -- Function 9 -----------------------------------------------------------------*/
void lord_debug (int lord_code, char *frmt, ...);
/* -- Function 9 -----------------------------------------------------------------*/
void lord_info (int lord_code, char *frmt, ...);
/* -- Function 9 -----------------------------------------------------------------*/
void lord_warning (int lord_code, char *frmt, ...);
/* -- Function 9 -----------------------------------------------------------------*/
void lord_error (int lord_code, char *frmt, ...);
/* -- Function 9 -----------------------------------------------------------------*/
void lord_critical (int lord_code, char *frmt, ...);
/* -- Function 9 -----------------------------------------------------------------*/
void set_lord_debug_mode (FILE * stream, char *verbosity);
/* -- Function 9 -----------------------------------------------------------------*/
void set_lord_info_mode (FILE * stream, char *verbosity);
/* -- Function 9 -----------------------------------------------------------------*/
void set_lord_warning_mode (FILE * stream, char *verbosity);
/* -- Function 9 -----------------------------------------------------------------*/
void set_lord_error_mode (FILE * stream, char *verbosity);
/* -- Function 9 -----------------------------------------------------------------*/
void set_lord_critical_mode (FILE * stream, char *verbosity);
/* -- Function 9 -----------------------------------------------------------------*/
void init_lord ();
/* -- Function 9 -----------------------------------------------------------------*/
void set_lord_modes (int debug, int info, int warning, int error, int critical);
/* -- Function 9 -----------------------------------------------------------------*/
void set_lord_outputs (FILE * stream_debug, FILE * stream_info, FILE * stream_warning, FILE * stream_error, FILE * stream_critical);
/* -- Function 9 -----------------------------------------------------------------*/
void set_lord_verbosity_levels (int verb_debug, int verb_info, int verb_warning, int verb_error, int verb_critical);
/* -- Function 9 -----------------------------------------------------------------*/
void set_lord_file (char *fullfilename);
/* -- Function 9 -----------------------------------------------------------------*/
int lord_get_last_error ();
/* -- Function 9 -----------------------------------------------------------------*/
void reset_lord ();
/* -- Function 9 -----------------------------------------------------------------*/
void set_lord_callback (LORD_CALLBACK fct);
/* -- Function 9 -----------------------------------------------------------------*/
int is_lord_initialised();
#if !0xD0C
LORD dokumentation her
#endif


/* ng_trans  ver 2006          # page 1    3 Oct 2006 13 55 */
/*
#define  DEBUGFETRANS
*/

/* -- MODULE PAPIR ------------------------------------------- */
#if !0xD0C
%\summary{TRLIB Plumbing for API Realization}
%\author{Simon Lyngby Kokkendorff and Thomas Knudsen}
%\date{2012--2013}
#endif
/* Intern API header som definerer TR-objektet, thokn, simlk, sep. 2011 */


/* -- GROUP Macros ------------------------------------------------- */

/* Utility macros implemented in order the hide the detailed composition of a PR-object (which may change) */

/* -- Macro -- */
#define IS_CARTESIC(pr)           ((pr->plab->u_c_lab).cstm==1)
/* -- Macro -- */
#define IS_GEOGRAPHIC(pr)       ((pr->plab->u_c_lab).cstm==2)
/* -- Macro -- */
#define IS_TM(pr)                    ((pr->plab->u_c_lab).cstm==3)
/* -- Macro -- */
#define GET_MLB(pr)                 ((pr->plab->u_c_lab).mlb)
/* -- Macro -- */
#define HAS_HEIGHTS(pr)          ((pr->plab->u_c_lab).h_dtm>200)
/* -- Macro -- */
#define IS_3D(pr)                     (pr!=NULL && (pr->plab->u_c_lab).ncoord==3)
/* -- Macro -- */
#define COORD_ORDER(pr)          ((pr->plab->u_c_lab).p_seq)


/* -- GROUP Data types ------------------------------------------------- */

/* -- Datatype --------------------------------------------------------- */
typedef struct PR PR;
#if !0xD0C
An internal PR-object (whose implementation might change).
Reference counting can easily be implemented by adding a n_references field
#endif

/* -- Datatype Continued --------------------------------------------------------- */
struct PR {
   union geo_lab *plab;
   double dgo[4]; /* space to be used by ptg_d for TM-projections. For other projections, the space can be used for other purposes. */
};
#if !0xD0C
The underlying struct of the PR type
#endif




/* -- Datatype --------------------------------------------------------- */
struct TR {
    PR *proj_in, *proj_out;
    struct mgde_str *geoid_pt;
    char geoid_name[FILENAME_MAX];
    int close_table;
    int use_geoids;
    int err;
};

/* -- Datatype --------------------------------------------------------- */
struct XYZ {
    double x, y, z;
    int err;
};



/* -- Datatype --------------------------------------------------------- */
struct PLATE {
    char *plate_model;           /* Name of plate model to use */
    char *intra_plate_model;     /* Name of intra plate model to use */
    int *plm_trf;                /* Plate model used (0->plate model NOT used) */
    int *ipl_trf;                /* Plate model used using: 0->no intraplate, 4->intra plate (iJD=oJD=-10000000.0), 1->iJD, 2->oJD, 3->iJD and oJD */
    double *plm_dt;              /* JD2000 plate model epoch */
    double *ipl_idt;             /* JD2000 intra plate model epoch in */
    double *ipl_odt;             /* JD2000 intra plate model epoch out */
    char *plm_nam;               /* Name of used plate model */
    char *plt_nam;               /* Name of used plate */
    char *ipl_nam;               /* Name of used intra plate */
};

/* 'Internal' functions not meant to be exposed in API
* Could consider implementing TR *TR_Compose(PR*,PR*) and  void TR_CloseContainer(TR*)  functions for maximal performance and internal use.
* This would not require any reference counting in constructors and desctructors, as long as TR_CloseProjection is not called before TR_CloseContainer.
*/

/* -- GROUP Functions ------------------------------------------------- */

/* -- Function 10 -----------------------------------------------------------------*/
int TR_GeoidTable (struct TR *);
/* -- Function 10 -----------------------------------------------------------------*/
int TR_SpecialGeoidTable (struct TR *tr, char *geoid_name);
/* -- Function 10 -----------------------------------------------------------------*/
int TR_IsMainThread (void);
/* -- Function 10 -----------------------------------------------------------------*/
int TR_IsThreadSafe (union geo_lab *plab);
/* -- Function 10 -----------------------------------------------------------------*/
int TR_tr (PR *, PR *, double *, double *, double *, double *, double *,double *, double *, int , int , struct mgde_str *);
/* -- Function 10 -----------------------------------------------------------------*/
int TR_itrf (union geo_lab *, union geo_lab *, double *, double *, double *, double *, double *, double *, int, double *, double *, double *, double *, double *, double *, int, double *, int, struct PLATE *);
/* -- Function 10 -----------------------------------------------------------------*/
PR * TR_OpenProjection (char *mlb);
/* -- Function 10 -----------------------------------------------------------------*/
void TR_CloseProjection (PR *proj);
/* -- Function 10 -----------------------------------------------------------------*/
void TR_GetGeoidName (struct TR *tr, char *name);
/* -- Function 10 -----------------------------------------------------------------*/
int TR_ImportLabel (char *text, char *mlb);
/* -- Function 10 -----------------------------------------------------------------*/
int TR_TransformGH (struct TR *tr, double *X_in, double *Y_in, double *Z_in, double *gh_out, int n);
#if !0xD0C
a simple TR_tr wrapper which also outputs geoid height
#endif



/* trthread.h  version ????.?           # page 1   DD Mmm 2011 hh mm */
/**
------------------------------------------------------------------------
TRTHREAD.H  --  support thread safe operations in trlib
------------------------------------------------------------------------

Thomas Knudsen, Simon Lyngby Kokkendorff, KMS, 2011-08-11


0. INTRODUCTION

This is a ridiculously simple piece of syntactic sugar, essentially
overloading the "__thread" storage qualifier when providing the
thread safe version of trlib, the KMS geodetic transformation library.


1. RATIONALE

The use of thread local storage in C and C++ is still new in terms of
standard adoption: Although it has been supported by most compilers for
approximately a decade, it was first handled in the standardization
processes more recently.

Hence, the "encouraged" (Drepper 2005) syntax, prepending the storage
qualifier keyword "__thread" to identifier declarations and definitions,
may not be what we end up with once the standardization processes have
come to and end, and the smoke has risen.

Also, Microsoft would not be Microsoft if they had not insisted on a more
verbose, less clear syntax for exactly the same semantics:

Encouraged:
    __thread int i = 1;


Microsoft:
    __declspec( thread ) int tls_i = 1;

For what we know at time of writing, we could do very well by

    1. using a sufficiently sane compiler
    2. assuming thread safe to be the default build mode
    3. using the __thread keyword where needed, and
    4. defining it away on the compiler command line when building
       a non-thread-safe library (i.e. gcc -D__thread ...)

We must, however, if not actually respect, then at least live and suffer
with Microsoft's insistence of imposing misery onto our lives.


2. IMPLEMENTATION

To do this, and to anticipate potential syntax changes, the ideal
solution would be a macro expanding differently for different input
types:

THREAD_SAFE(static int)  ->   static __thread int
THREAD_SAFE(int)         ->   __thread int
THREAD_SAFE(extern int)  ->   extern __thread int

in anticipation that the syntax could change to (e.g.) "thread static int"
or somesuch.

Realizing, however, that this may well be preparing for "shit that just
ain't gonna happen", we have opted for this more minimalist version, that
eliminates the need to define away a keyword (-D__thread), while also
adding a bit of semantic content.

Essentially we assume non-thead-safety by default, and define the macro
THREAD_SAFE as empty. But if THREAD_SAFE is defined already (by using
-DTHREAD_SAFE on the compiler command line), we redefine it as __thread

... unless of course we live in a Microsoft world, and have to define it
as __declspec( thread ).


3. INTENDED USE

This header is intended to be included via geo_lab.h - which is the
closest match we currently have to a universal library internal include
file.


4. REFERENCES

Ulrich Drepper (2005): ELF handling for thread-local storage
    http://www.akkadia.org/drepper/tls.pdf

**/





#ifdef THREAD_SAFE
#undef  THREAD_SAFE

#ifdef _MSC_VER
#define THREAD_SAFE __declspec( thread )
#else
#define THREAD_SAFE __thread

#endif

#else
#define THREAD_SAFE
#endif

/* End of TRTHREAD_H */








































/* bh_lat.h                           # page 1   22 Feb 1993 12 54 */

