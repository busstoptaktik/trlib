

/* utmtmgr.c                        # page 1   22 Aug 1994 11 07 */


/* Copyright (c) 1994, GD, Kort-og Matrikelstyrelsen, Denmark  */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GD, Kort- og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include   <stdio.h>
#include   <math.h>
#include   <string.h>
#include   "geo_lab.h"

#ifndef    M_PI
#include   "kms_math.h"
#endif

int                 utmtmgr(
/*________________________*/
union geo_lab       *u_lab,
int                  direct,
double              *N,
double              *E,
double              *B,
double              *L,
char                *mgref,
int                  cif
)

{
#include          "ptg.h"

  /*                       00        10        20    */
  /*                       012345678901234567890123  */
  char               *A = "ABCDEFGHJKLMNPQRSTUVWXYZ";

  char               *p;
  int                 czone, izone, i;

  struct coord_lab   *c_lab = &(u_lab->u_c_lab);
  double              N1, E1;
  double              B1, L1, B_s, B_n;
  double              Nx, Ex, Bx, Lx;
  double              L6 = 6.0*M_PI/180.0, L0, unit_h;
  int                 eix, nix, bix, r = 0, utm;
  short               cmplt;
  char                ceix, cnix, cbix;
  int                 n, e, b, nmd, emd, n_base;
  int                 unit = 100000, unit_m, fct;

  /* Save central longitude, find the UTM zone and save compl-flag */
  L0    = c_lab->L0;
  czone = (int) (L0*180.0/M_PI - 2.9);
  czone = (czone + 186)/6;
  cmplt = c_lab->cmplt;



/* utmtmgr.c                        # page 2   22 Aug 1994 11 07 */


  switch (direct) {

  case 1:  /* UTM -> MGR */

    /* Check number of output digits */
    if      (cif > 5) cif = 5;
    else if (cif < 1) cif = 1;

    /* Rounding and Divisor for E and N digits */
    for (i = cif, unit_m = unit; i > 0; unit_m /= 10, i--);
    unit_h = unit_m/2.0;

    /* LAT and LNG */
    N1 = *N;
    E1 = *E;
    (void) ptg(u_lab, 1, N1, E1, &B1, &L1, "mgrtest", stderr);

    /* LAT-BELT letter */
    b   = (int) (B1*180.0/M_PI);
    bix = (b + 96)/8;

   /* Test for utm and spec n-limit */
    utm = (-80 < b && b < 84);
    if (bix == 22 && utm) (bix--);

    /* Test for station coord outside the actual utm zone */
    if (fabs(L1 - c_lab->L0) <= 3.0*M_PI/180.0)
      izone = czone;
    else {
      /* Transform  Northing and Easting to the correct zone */
      izone = (int) (L1*180.0/M_PI);
      izone = (izone + 186)/6;

      /* Modify central lng, transform, and restore central lng */
      c_lab->L0    = L0 + (izone - czone)*L6;
      c_lab->cmplt = 0;
      (void) ptg(u_lab, -1, B1, L1, &N1, &E1, "mgrtest", stderr);
      c_lab->L0    = L0;
      c_lab->cmplt = cmplt;
    }

    /* Round N and E to long integers */
    n   = (int) (N1 + unit_h);
    e   = (int) (E1 + unit_h);

    /* Block coordinates -> block letters */
    nix = (n/unit + 5*((izone - 1)%2))%20;
    eix =  e/unit + 8*((izone - 1)%3) - 1;

    /* Coordinates inside the block -> E and N digits */
    nmd = (n - (n/unit)*unit)/unit_m;
    emd = (e - (e/unit)*unit)/unit_m;



/* utmtmgr.c                        # page 3   22 Aug 1994 11 07 */


    /* output */
    *N = N1;
    *E = E1;
    *B = B1;
    *L = L1;

    (void) sprintf(mgref, "%02d%c%c%c%0*d%0*d",
        izone, *(A+bix), *(A+eix), *(A+nix), cif, emd, cif, nmd);

    break; /* case +1 */

  case -1: /* MGR -> UTM */
    i = (int) strlen(mgref);

    /* MGR string length */
    switch(i) {
    case 7: /* 1 digit */
      p   = "%2d%c%c%c%1d%1d";
      fct = 10000;
      break;
    case 9: /* 2 digits */
      p   = "%2d%c%c%c%2d%2d";
      fct = 1000;
      break;
    case 11: /* 3 digits */
      p   = "%2d%c%c%c%3d%3d";
      fct = 100;
      break;
    case 13: /* 4 digits */
      p   = "%2d%c%c%c%4d%4d";
      fct = 10;
      break;
    case 15: /* 5 digits */
      p   = "%2d%c%c%c%5d%5d";
      fct = 1;
      break;
    default: /* untelligible mgr */
      (void) printf("\n*** utmtmgr: INKONSISTENT MGR-KODE\n");
      return(-100);
    } /* end switch i */

    /* String components: UTM-zone, LAT-BELT, GRID-LTR, E, N */
    if (sscanf(mgref, p,
        &izone, &cbix, &ceix, &cnix, &emd, &nmd) != 6) {
      (void) printf("\n*** utmtmgr: INKONSISTENT MGR-KODE\n");
      return(-100);
    }

    /* Contingent modification of the coord_label */
    if (izone != czone)
      c_lab->L0 = L0 + 6.0*(izone - czone)*M_PI/180.0;



/* utmtmgr.c                        # page 4   22 Aug 1994 11 07 */


    /* LAT-BELT INDEX -> latitudes (S and N) */
    bix = cbix - 'A';
    if (cbix >= 'I') bix--;
    if (cbix >= 'O') bix--;
    b   = 8*bix - 96;
    B_s = b*M_PI/180.0;
    B_n = (b+8)*M_PI/180.0;

    /* Estimated value of Northing of the LAT-BELT */
    (void) ptg(u_lab, -1, B_s, c_lab->L0, &N1, &E1, "mgrtest", stderr);
    n_base = (int) (floor(N1/20.0/unit));

    /* E-block letter and digits -> Easting */
    eix = ceix - 'A';
    /* Compensate for excluded I and O */
    if (ceix >= 'I') eix--;
    if (ceix >= 'O') eix--;
    eix = (eix - 8*((izone - 1)%3) + 1)%24;
    /* Easting in metres (long integers) */
    e   = unit*eix + fct*emd;

    /* N-block letter and digits -> Northing, initial guess */
    nix = cnix - 'A';
    /* Compensate for excluded I and O */
    if (cnix >= 'I') nix--;
    if (cnix >= 'O') nix--;
    nix = (nix - 5*((izone - 1)%2))%20;
    /* Northing in metres (long integers) */
    n   = n_base*20*unit + unit*nix + fct*nmd;

    /* Test and modify the latitude with the LAT-BELT limits */
    Nx  = n;
    Ex  = e;
    (void) ptg(u_lab, +1, Nx, Ex, &Bx, &Lx, "mgrtest", stderr);

    /* South of the belt */
    if      (Bx < B_s) {
      n += (20*unit);
      r  = 1;
    }
    /* North of the belt */
    else if (Bx > B_n) {
      n -= (20*unit);
      r  = 1;
    }



/* utmtmgr.c                        # page 5   22 Aug 1994 11 07 */


    /* contingent updating of the LNG and LAT */
    if (r) {
      Nx = n;
      (void) ptg(u_lab, +1, Nx, Ex, &Bx, &Lx, "mgrtest", stderr);
    }

    /* Restore the central longitude of the label */
    c_lab->L0 = L0;

    /* output coord */
    *E = Ex;
    *N = Nx;
    *L = Lx;
    *B = Bx;
    break; /* case -1 */

  default: /* illegal direct-param */
    (void) fprintf(stdout,
        "\n*** utmtmgr: illegal direct-param: %d", direct);
    return(-100);
  } /* end direct-switch */

  return(izone - czone);

}


