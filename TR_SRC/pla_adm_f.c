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
 


/* pla_adm_f    ver 1998.02            # page 1   18 Feb 1998 14 13 */


/* Copyright (c) 1998, Kort-og Matrikelstyrelsen, Denmark      */
/* All rights reserved.                                        */

/* This is unpublished proprietary source code of GEK, Kort-og */
/* Matrikelstyrelsen, Denmark.  This copyright claim does not  */
/* indicate an intention of publishing this code.              */

#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <math.h>
#include    <ctype.h>
#include    <sys/types.h>
#include    <sys/stat.h>
#include    <fcntl.h>
#include    "KmsFncs.h"

/* pla_adm_f is a maintenance tool for ITRF plates boundaries */
/* the plate boundaries are used to decide which plate is     */
/* appropriate for a point in question                        */
/*                                                            */
/* CALL  ::                                                   */
/* (int) pla_adm_f(                                           */
/*   char  mode,      mode = 'b' / 't' / 'a' / 'l'            */
/*   FILE *i_fd       input file_pointer                      */
/*   FILE *d_fd       input file_pointer (loop directives)    */
/*                    only used in mode == 'l'                */
/*   FILE *o_fd       output file_pointer                     */
/*   char *errtx)     string for error                        */
/*                                                            */
/* mode == 'b'                                                */
/* produces a binary file o_name from the file i_name         */
/*   from the file i_name                                     */
/*                                                            */
/* mode == 't'                                                */
/* produces a text in the file o_name                         */
/*   from the binary file i_name                              */
/*                                                            */
/* mode == 'a'                                                */
/* converts an arc (txt) file i_name                          */
/*   to poly lines Geodisp format (txt) in o_name             */
/*                                                            */
/* mode == 'l'                                                */
/* from the txt file d_name is read the loops directives      */
/* for lines to be selected from the (txt) file i_name        */
/*   to poly loops (txt) in o_name                            */
/*                                                            */


/* pla_adm_f                           # page 2   18 Feb 1998 14 13 */


int                  pla_adm_f(
/*___________________________*/
char                 mode,
FILE                *i_fd,
FILE                *d_fd,
FILE                *o_fd,
char                *err_txt
)
{

/* progr.:  Karsten Engsager, feb. 2003                       */

  long                   i_pos, p_pos, i_sz;
  int                    lb_nr, used = 1, qr, first = 1, plus, i, j;
  int                    pol_ant = 0, pkt_ant = 0, plate;
  int                    i_pol[500], i_ant[500], BL_ix[500], pol_out=0;
  int                   *pi_pol = i_pol, *pi_ant = i_ant, *p_ix = BL_ix;
  double                 B, L, Bp[20000], *p_B;
  char                   txt[128], *p_tx;

  union geo_lab          lab_a;
  struct  plate_info     pl_inf;
  struct typ_dec         tpd;


/* pla_adm_f                           # page 2   18 Feb 1998 14 13 */


  switch (mode) {
  case 'b': /* to_bin */
            /* ______ */
    i_sz = sizeof(pl_inf);

    /* read plates info */
    if (conv_lab("input", &lab_a, i_fd) != CRD_LAB) {
      (void) sprintf(err_txt, "\n+++ mangler label\n");
      return(-1);
    }
    (void) fgetln_kms(txt, &qr, i_fd);
    do {
      (void) sscanf(txt, "%d%n", &plate, &used);
      if (plate > 0) {
        pl_inf.plate_nr = (short) plate;
        p_tx            = txt + used;
        (void) sscanf(p_tx, "%d%n", &lb_nr, &used);
        pl_inf.poly_tp  = (short) lb_nr;
        p_tx           += used;
        pl_inf.BS       = sgetg(p_tx, &tpd, &used, "dg");
        p_tx           += used;
        pl_inf.B1       = sgetg(p_tx, &tpd, &used, "dg");
        p_tx           += used;
        pl_inf.B2       = sgetg(p_tx, &tpd, &used, "dg");
        p_tx           += used;
        pl_inf.L1       = sgetg(p_tx, &tpd, &used, "dg");
        p_tx           += used;
        pl_inf.L2       = sgetg(p_tx, &tpd, &used, "dg");
        pl_inf.a_ant    = 0;
        pl_inf.b_ant    = 0;
        pl_inf.poly_a   = 0;
        pl_inf.poly_b   = 0;
        if ((i = (int) fwrite((void *) &pl_inf, i_sz, 1, o_fd)) - 1) {
          (void) sprintf(err_txt, "%s%s%8ld%s%4d%s%4ld ;",
              "\n*** pla_adm_f: ERROR at write of table",
              "\n plate_info_adr ", ftell(o_fd), " plate  = ",
              plate, " size = ", i_sz);
          return(-1);
        }
        ++ pol_ant;
        (void) fgetln_kms(txt, &qr, i_fd);
      }
    } while (plate > 0);

    /* save stop : plates info */
    pl_inf.plate_nr = 0;
    pl_inf.poly_tp  = 0;
    pl_inf.BS       = 0.0;
    pl_inf.B1       = 0.0;
    pl_inf.B2       = 0.0;
    pl_inf.L1       = 0.0;
    pl_inf.L2       = 0.0;
    pl_inf.a_ant    = 0;
    pl_inf.b_ant    = 0;
    pl_inf.poly_a   = 0;
    pl_inf.poly_b   = 0;
    if ((i = (int) fwrite((void *) &pl_inf, i_sz, 1, o_fd)) - 1) {
          (void) sprintf(err_txt, "%s%s%8ld%s%4ld ;",
              "\n*** pla_adm_f: ERROR at write of table",
              "\n plate_info_stop_adr ", ftell(o_fd),
               " plate  = 0 size = ", i_sz);
      return(-1);
    }

    /* read poly_a, update info */
    pol_ant = 0;
    p_pos   = ftell(o_fd);
    (void) fgetln_kms(txt, &qr, i_fd);
    do {
      (void) sscanf(txt, "%d", &plate);
      if (plate > 0) {

        /* read info and check */
        i_pos = pol_ant * i_sz;
        if (fseek(o_fd, i_pos, SEEK_SET) ||
           ((i = (int) fread((void *) &pl_inf, i_sz, 1, o_fd)) - 1)) {
          (void) sprintf(err_txt, "%s%s%8ld%s%4d%s%4ld ;",
              "\n*** pla_adm_f: ERROR at read of info poly_a",
              "\n plate_info_adr ", ftell(o_fd), " plate  = ",
              plate, " size =", i_sz);
          return(-1);
        }
        if (plate != (int) pl_inf.plate_nr) {
          (void) sprintf(err_txt, "%s%s%2d%s%2d ;",
              "\n*** pla_adm_f: sequence error of info and poly_a",
              "\n info_plate ", pl_inf.plate_nr,
              "  poly_a_plate  = ", plate);
          return(-1);
        }
        plus = pl_inf.poly_tp == 5;

        /* read poly_a */
        p_B     = Bp +1;
        pkt_ant = 0;
        (void) fgetln_kms(txt, &qr, i_fd);
        do {
          (void) sscanf(txt, "%d%n", &lb_nr, &used);
          if (lb_nr > 0) {
            p_tx = txt + used;
            *(p_B ++) = sgetg(p_tx, &tpd, &used, "dg");
            p_tx += used;
            *(p_B ++) = sgetg(p_tx, &tpd, &used, "dg");
            /* Pacific translation to around zero Longitude */
            if (plus) *(p_B-1) = v_red(*(p_B -1) + M_PI);
            ++ pkt_ant;
            (void) fgetln_kms(txt, &qr, i_fd);
          }
        } while (lb_nr > 0);
        j   = 2 * pkt_ant +1;
        *Bp = (double) pkt_ant;

        /* update info */
        pl_inf.a_ant  = j;
        pl_inf.poly_a = p_pos;
        if (fseek(o_fd, i_pos, SEEK_SET) ||
           ((i = (int) fwrite((void *) &pl_inf, i_sz, 1, o_fd)) - 1)) {
          (void) sprintf(err_txt, "%s%s%8ld%s%4d%s%4ld ;",
              "\n*** pla_adm_f: ERROR at write of info poly_a",
              "\n plate_info_adr ", ftell(o_fd),
               " plate  = ", plate, " size = ", i_sz);
          return(-1);
        }

        /* save poly_a */
        if (fseek(o_fd, p_pos, SEEK_SET) ||
           ((i = (int) fwrite((void *) Bp, sizeof(double),
                               j, o_fd)) - j)) {
          (void) sprintf(err_txt, "%s%s%8ld%s%4d%s%4ld ;",
              "\n*** pla_adm_f: ERROR at write of poly_a",
              "\n poly_a_addr ", ftell(o_fd),
               " plate  = ", i, " size = ", (long) sizeof(double));
          return(-1);
        }
        ++ pol_ant;
        p_pos = ftell(o_fd);
        (void) fgetln_kms(txt, &qr, i_fd);
      }
    } while (plate > 0);

    /* read poly_b, update info */
    pol_ant = 0;
    p_pos   = ftell(o_fd);
    (void) fgetln_kms(txt, &qr, i_fd);
    do {
      (void) sscanf(txt, "%d", &plate);
      if (plate > 0) {

        /* read info and check */
        i_pos = pol_ant * i_sz;
        if (fseek(o_fd, i_pos, SEEK_SET) ||
           ((i = (int) fread((void *) &pl_inf, i_sz, 1, o_fd)) - 1)) {
          (void) sprintf(err_txt, "%s%s%8ld%s%4d%s%4ld ;",
              "\n*** pla_adm_f: ERROR at read of info poly_b",
              "\n plate_info_addr ", ftell(o_fd),
              "  plate  = ", plate, " size =", i_sz);
          return(-1);
        }
        if (plate != (int) pl_inf.plate_nr) {
          (void) sprintf(err_txt, "%s%s%2d%s%2d ;",
              "\n*** pla_adm_f: sequence error of info and poly_b",
              "\n info_plate ", pl_inf.plate_nr,
              "  poly_a_plate  = ", plate);
          return(-1);
        }
        plus = pl_inf.poly_tp == 5;

        /* read poly_b */
        p_B     = Bp +1;
        pkt_ant = 0;
        (void) fgetln_kms(txt, &qr, i_fd);
        do {
          (void) sscanf(txt, "%d%n", &lb_nr, &used);
          if (lb_nr > 0) {
            p_tx = txt + used;
            *(p_B ++) = sgetg(p_tx, &tpd, &used, "dg");
            p_tx += used;
            *(p_B ++) = sgetg(p_tx, &tpd, &used, "dg");
            /* Pacific translation to around zero Longitude */
            if (plus) *(p_B-1) = v_red(*(p_B -1) + M_PI);
            ++ pkt_ant;
            (void) fgetln_kms(txt, &qr, i_fd);
          }
        } while (lb_nr > 0);
        j   = 2 * pkt_ant +1;
        *Bp = (double) pkt_ant;

        /* update info */
        pl_inf.b_ant  = j;
        pl_inf.poly_b = p_pos;
        if (fseek(o_fd, i_pos, SEEK_SET) ||
           ((i = (int) fwrite((void *) &pl_inf, i_sz, 1, o_fd)) - 1)) {
          (void) sprintf(err_txt, "%s%s%8ld%s%4d%s%4ld ;",
              "\n*** pla_adm_f: ERROR at write of info poly_b",
              "\n plate_info_addr ", ftell(o_fd),
              "  plate  = ", plate, " size =", i_sz);
          return(-1);
        }

        /* save poly_b */
        if (fseek(o_fd, p_pos, SEEK_SET) ||
           ((i = (int) fwrite((void *) Bp, sizeof(double),
                               j, o_fd)) - j)) {
          (void) sprintf(err_txt, "%s%s%8ld%s%4d%s%4ld ;",
              "\n*** pla_adm_f: ERROR at write of poly_b",
              "\n poly_b_addr ", ftell(o_fd),
              "  plate  = ", plate, " size =", (long) sizeof(double));
          return(-1);
        }
        ++ pol_ant;
        p_pos = ftell(o_fd);
        (void) fgetln_kms(txt, &qr, i_fd);
      }
    } while (plate > 0);

    break; /* to_bin */

  case 't': /* to_txt */
            /* ______ */

    (void) fprintf(o_fd,"\n\n# geo_euref89\n\n");

    i_sz = sizeof(pl_inf);
    tpd  = *set_tpd("dg", 4, 4);
    do {
      /* read plates info */
      if ((i = (int) fread((void *) &pl_inf, i_sz, 1, i_fd)) - 1) {
        (void) sprintf(err_txt, "%s%s%8ld%s%4d%s%4ld%s%d ;",
            "\n*** pla_adm_f: ERROR at read of info",
            "\n plate_info_adr ", ftell(i_fd), " plate  = ",
            pl_inf.plate_nr, " size =", i_sz, " i = ", i);
        return(-1);
      }

      plate = (int) pl_inf.plate_nr;
      (void) fprintf(o_fd, "\n%2d  %2d ", plate, pl_inf.poly_tp);
      (void) fputg(o_fd, pl_inf.BS, &tpd, "u");
      (void) fprintf(o_fd, "  ");
      (void) fputg(o_fd, pl_inf.B1, &tpd, "u");
      (void) fprintf(o_fd, "  ");
      (void) fputg(o_fd, pl_inf.B2, &tpd, "u");
      (void) fprintf(o_fd, "  ");
      (void) fputg(o_fd, pl_inf.L1, &tpd, "u");
      (void) fprintf(o_fd, "  ");
      (void) fputg(o_fd, pl_inf.L2, &tpd, "u");
    } while (plate > 0);
    (void) fprintf(o_fd, "\n");

    i_pos = 0;
    /* read poly_a */
    do {
      /* read plates info */
      if (fseek(i_fd, i_pos, SEEK_SET) ||
         ((i = (int) fread((void *) &pl_inf, i_sz, 1, i_fd)) - 1)) {
        (void) sprintf(err_txt, "%s%s%8ld%s%4d%s%4ld ;",
            "\n*** pla_adm_f: ERROR at read of info poly_a",
            "\n plate_info_adr ", ftell(o_fd), " plate  = ",
            plate, " size =", i_sz);
        return(-1);
      }
      i_pos = ftell(i_fd);

      plus  = pl_inf.poly_tp == 5;
      plate = (int) pl_inf.plate_nr;
      used  = plate * 2000 +1;
      if (plate > 0) {
        (void) fprintf(o_fd, "\n\n* poly_a ;");
        (void) fprintf(o_fd, "\n\n%2d  ", plate);
        p_pos   = (long) pl_inf.poly_a;
        j       = pl_inf.a_ant;
        p_B     = Bp +1;
        first   = 1;
        if (fseek(i_fd, p_pos, SEEK_SET) ||
           ((i = (int) fread((void *) Bp, sizeof(double),
                             j, i_fd)) - j)) {
          (void) sprintf(err_txt, "%s%s%8ld%s%4d%s%4ld ;",
              "\n*** pla_adm_f: ERROR at read of poly_a",
              "\n plate_info_adr ", ftell(o_fd),
               " plate  = ", plate, " size = ", (long) sizeof(double));
          return(-1);
        }
        pkt_ant = (int) *Bp;

        for (i = 1; i <= pkt_ant; i ++) {
          if (first) {
            first = 0;
            (void) fprintf(o_fd, "\n%5dp  ", used ++);
          } else (void) fprintf(o_fd, "\n%5d   ", used ++);
          (void) fputg(o_fd, *(p_B ++), &tpd, "u");
          (void) fprintf(o_fd, "  ");
          /* Pacific translation from around zero Longitude */
          if (plus) *p_B = v_red(*p_B + M_PI);
          (void) fputg(o_fd, *(p_B ++), &tpd, "u");
        }
        (void) fprintf(o_fd, "\n-1z");
      }
    } while (plate > 0);
    (void) fprintf(o_fd, "\n\n*END;\n 0\n");

    i_pos = 0;
    /* read poly_b */
    do {
      /* read plates info */
      if (fseek(i_fd, i_pos, SEEK_SET) ||
         ((i = (int) fread((void *) &pl_inf, i_sz, 1, i_fd)) - 1)) {
        (void) sprintf(err_txt, "%s%s%8ld%s%4d%s%4ld ;",
            "\n*** pla_adm_f: ERROR at read of info poly_b",
            "\n plate_info_adr ", ftell(o_fd), " plate  = ",
            plate, " size =", i_sz);
        return(-1);
      }
      i_pos = ftell(i_fd);

      plus  = pl_inf.poly_tp == 5;
      plate = (int) pl_inf.plate_nr;
      used  = plate * 1000 +1;
      if (plate > 0) {
        (void) fprintf(o_fd, "\n* poly_b ;");
        (void) fprintf(o_fd, "\n\n%2d  ", plate);
        p_pos   = (long) pl_inf.poly_b;
        j       = pl_inf.b_ant;
        p_B     = Bp +1;
        first   = 1;
        if (fseek(i_fd, p_pos, SEEK_SET) ||
           ((i = (int) fread((void *) Bp, sizeof(double),
                             j, i_fd)) - j)) {
          (void) sprintf(err_txt, "%s%s%8ld%s%4d%s%4ld ;",
              "\n*** pla_adm_f: ERROR at write of info poly_b",
              "\n plate_info_adr ", ftell(o_fd),
               " plate  = ", plate, " size = ", (long) sizeof(double));
          return(-1);
        }
        pkt_ant = (int) *Bp;

        for (i = 1; i <= pkt_ant; i ++) {
          if (first) {
            first = 0;
            (void) fprintf(o_fd, "\n%5dp  ", used ++);
          } else (void) fprintf(o_fd, "\n%5d   ", used ++);
          (void) fputg(o_fd, *(p_B ++), &tpd, "u");
          (void) fprintf(o_fd, "  ");
          /* Pacific translation from around zero Longitude */
          if (plus) *p_B = v_red(*p_B + M_PI);
          (void) fputg(o_fd, *(p_B ++), &tpd, "u");
        }
        (void) fprintf(o_fd, "\n-1z");
      }
    } while (plate > 0);
    (void) fprintf(o_fd, "\n\n*END;\n 0\n");

    break; /* to_txt */

  case 'a': /* arc_pol */
            /* _______ */

    (void) fgetln_kms(txt, &qr, i_fd);
    do {
      (void) sscanf(txt, "%d", &lb_nr);
      lb_nr *= 1000;
      (void) fgetln_kms(txt, &qr, i_fd);
      while (strncmp(txt, "end", 3)) {
        (void) sscanf(txt, "%lf,%lf", &L, &B);

        if (first) {
          ++ pol_ant;
          first = 0;
          (void) fprintf(o_fd,"\n\n# geo_euref89\n\n");
          (void) fprintf(o_fd, "\n%8dp  %9.5f dg   %9.5f dg",
                         ++lb_nr, B, L);
        } else (void) fprintf(o_fd, "\n%8d   %9.5f dg   %9.5f dg",
                              ++lb_nr, B, L);
        ++ pkt_ant;
        (void) fgetln_kms(txt, &qr, i_fd);
      }
      if (!first) (void) fprintf(o_fd,"\n-1z\n");
      first = 1;
      (void) fgetln_kms(txt, &qr, i_fd);
    } while (strncmp(txt, "end", 3));
    (void) fprintf(o_fd, "\n\nloops    : %6d", pol_ant);
    (void) fprintf(o_fd, "\n\npoints   : %6d", pkt_ant);
    break; /* arc_pol */

  case 'l': /* loops */
            /* _____ */

    /* read poly to i_pol, i_ant, B, L, *p_fi */
    p_B   = Bp;
    while (conv_lab("input", &lab_a, i_fd) == CRD_LAB) {
      first   = 1;
      *pi_ant = 0;
      (void) fgetln_kms(txt, &qr, i_fd);
      do {
        (void) sscanf(txt, "%d%n", &lb_nr, &used);
        if (lb_nr > 0) {
          if (first) {
            first        = 0;
            *(p_ix  ++)  = (int) (p_B - Bp);
            *(pi_pol ++) = lb_nr / 1000;
            ++ pol_ant;
          }
          p_tx = txt + used +1;
          (void) sscanf(p_tx, "%lf%n", p_B ++, &used);
          p_tx += used +2;
          (void) sscanf(p_tx, "%lf", p_B ++);
          ++ *pi_ant;
          ++ pkt_ant;
          (void) fgetln_kms(txt, &qr, i_fd);
        }
      } while (lb_nr > 0);
      ++ pi_ant;
    }
    (void) fprintf(o_fd, "\n\npolygons : %6d", pol_ant);
    (void) fprintf(o_fd, "\n\npoints   : %6d\n\n", pkt_ant);

    pol_ant = (int) (pi_ant - i_ant);
    *pi_ant = 0;
    pkt_ant = 0;

    /* read polygon number */
    (void) fgetln_kms(txt, &qr, d_fd);
    do {
      (void) sscanf(txt, "%d%n", &plus, &used);
      if (plus > 0) {
        (void) fprintf(o_fd, "\n\n#geo_euref89\n");
        (void) fprintf(o_fd, "\n*loop nr   %3d;\n", plus);
        ++ pol_out;

        /* read polygon selections */
        (void) fgetln_kms(txt, &qr, d_fd);
        do {
          (void) sscanf(txt, "%d%n", &lb_nr, &used);
          if (lb_nr > 0) {
            first = lb_nr / 1000;
            p_tx  = txt + used;
            (void) sscanf(p_tx, "%d", &used);

            /* find line_nr : first */
            qr     = -1;
            pi_pol = i_pol + first - 101;
            {
              if (*pi_pol == first) {
                qr     = (int) (pi_pol - i_pol);
                pi_ant = i_ant + qr;
                p_B    = Bp + *(BL_ix + qr);
                qr     = lb_nr % 1000;
                if (0 < qr   && qr   <= *pi_ant &&
                    0 < used && used <= *pi_ant) {
                  p_B   += qr + qr -2;
                  if (qr <= used) {
                    used  -= qr;
                    first  = +1;
                  } else {
                    used   = qr - used;
                    first  = -1;
                  }
                } else qr = -2;
              }
            }

            /* write line_elements of line_nr */
            if (qr > 0) {
              for (i = 0; i <= used; i ++) {
                (void) fprintf(o_fd, "\n%5d   %9.4f dg   %9.4f dg",
                        pkt_ant ++, *p_B, *(p_B + first));
                p_B += first + first;
              }
            } else {
              if (qr == -1)
                   (void) fprintf(o_fd, "\nLine NOT found :");
              else (void) fprintf(o_fd, "\nLine lim NOT found :");
              (void) fprintf(o_fd, "%d  %d +++", lb_nr, used);
            }
            (void) fprintf(o_fd, "\n");

            (void) fgetln_kms(txt, &qr, d_fd);
          }
        } while (lb_nr > 0);
        (void) fprintf(o_fd, "\n-1z\n");
        (void) fgetln_kms(txt, &qr, d_fd);
      }
    } while (plus > 0);

    (void) fprintf(o_fd, "\n\nloops    : %6d", pol_out);
    (void) fprintf(o_fd, "\n\npoints   : %6d", pkt_ant);
    break; /* loops */
  default:
    (void) sprintf(err_txt, "\npla_adm_f : illegal mode : %c\n", mode);
    return(-1);
  }

  (void) fflush(o_fd);
  return(0);

}


