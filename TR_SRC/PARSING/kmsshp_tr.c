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
 


/* kmsshp_tr  ver 2009.01          # page 1     7 Jan 2009 11 57 */


/* Copyright (c) 2009, National Space Institute, DTU, Denmark */
/* All rights reserved.                                       */

/* This is unpublished proprietary source code of DTU, Denmark */
/* This copyright claim does not indicate an intention of      */
/* publishing this code.                                       */

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "geo_lab.h"
#include "geoid_d.h"
#include "geoid_d.h"
#include "kms_shape.h"

int              kmsshp_tr(
/*______________________*/
char            *shpin_txt,
char            *shpout_txt,
int              SOE_no_h,
union geo_lab   *g_lab_in,
union geo_lab   *g_lab_out,
int             *q_trf_ok,
char            *err_txt)
{

/* kmsshp_tr returns::
   >0        Number of trf OK (== *q_trf_ok).
   0         No transformations no errors.
   _TRF_**** error numbers se geo_lab.h
   -95       .prj file not set: Unknown esri code
   -96       Uknown SHP Type
   -97       Output file not writeable
   -98       Input file not found
   -99       Output file not found

   coordinates unit is (_in/_out):deafault: "m",
                              for:geo: "dg" or "rad"
   dependening geo_lab->u_c_lab.g_tpd
      (.gf ==2 && .tf ==3) ? "rad" : "dg"
*/

#include   "fgetlhtx.h"
#include   "fgetln_kms.h"
#include   "fputshpprj.h"
#include   "gd_trans.h"
#include   "s_status.h"

#define          SHP_NOVAL   -1.0e+308

  

  int copy_parts(FILE *fh_in, FILE *fh_out, int NumParts, double *Z);
  int swop_int(int i);

  int                  KMS_ShpFileHeaderInt[9], KMS_ShxFileHeaderInt[9];
  double               KMS_ShpFileHeaderBox[8], KMS_ShxFileHeaderBox[8];
  struct SHPRecordHeader KMS_ShpRecordHeader;
  struct SHPRPoint       KMS_ShpPoint;
  double                 KMS_ShpBoxPoint[4];
  int                    KMS_ShpNumPoints;
  struct SHPRBoxLine     KMS_ShpBoxLine;
//struct SHPRMinMax      KMS_ShpMinMaxM;
  int                    KMS_ShapeType;

  int              curpos, Z_pos = 0, boxpos = 0, Zboxpos, recused, wpos;
  int              Rncount, Rlcount, Rucount, ZMcount;
  int              ZMType, Z_Type = 0, i, tr_res, qTrn = 0;
  int              FileLength, RecordLength;
  struct SHPRPoint XY[128], Zmin_max, *pXY;
  double           Z[128], M[128], Slave[3], gh, val_ff = 0.0;
  double           XminF = 0.0, YminF = 0.0, ZminF = 0.0, XmaxF = 0.0, YmaxF = 0.0, ZmaxF = 0.0;
  double           XminR = 0.0, YminR = 0.0, ZminR = 0.0, XmaxR = 0.0, YmaxR = 0.0, ZmaxR = 0.0;
  char            *pb;
  struct stat      buf;
  struct mgde_str  GeoidTable;
  int              conv_in = 0, conv_out = 0;
  int              bGeo, bMval, bZval, bBox, bHupd = 0;
  FILE            *fh_in, *fh_out;
  // Transform the .SHP file  very simple implementation
  // Copy      the .SHX file
  // Copy      the .DBF file

  if  (g_lab_in->u_c_lab.cstm == 2 &&
       (g_lab_in->u_c_lab.g_tpd.gf !=2 ||
        g_lab_in->u_c_lab.g_tpd.tf != 3)) conv_in = 1;
  bGeo = (g_lab_out->u_c_lab.cstm == 2) ? 1 : 0;
  if (bGeo &&
       (g_lab_out->u_c_lab.g_tpd.gf!=2 ||
        g_lab_out->u_c_lab.g_tpd.tf!= 3)) conv_out = 1;

  pb  = (char *) (&val_ff);
  *pb = *(pb +1) = *(pb +2) = *(pb +3) = -127;
  for (i = 0; i < 128; i++) M[i] = Z[i] = val_ff;

  // Transform the .SHP file
  fh_in = fopen(shpin_txt, "rb");
  if (fh_in == NULL) {
    (void) sprintf(err_txt, "Input file not found: %s\n  ", shpin_txt);
    return(-98);
  }
  
  fh_out = fopen(shpout_txt, "w+b");
  if (fh_out == NULL) {
    (void) sprintf(err_txt,
                   "Output file not found: %s\n  ", shpout_txt);
    (void) fclose(fh_in);
    return(-99);
  }

  XminF =  930000000.0;
  YminF =  930000000.0;
  ZminF =  930000000.0;
  XmaxF = -930000000.0;
  YmaxF = -930000000.0;
  ZmaxF = -930000000.0;

  // read the SHP File Header and store prelim
  (void) fread(KMS_ShpFileHeaderInt, sizeof(int), 9, fh_in);
  (void) fread(KMS_ShpFileHeaderBox, sizeof(double), 8, fh_in);
  // M-values are optional
  if (KMS_ShpFileHeaderBox[6] < SHP_NOVAL ||
      KMS_ShpFileHeaderBox[7] < SHP_NOVAL) bMval = 0;
  else
  bMval = (KMS_ShpFileHeaderBox[6] != 0.0 ||
           KMS_ShpFileHeaderBox[7] != 0.0) ? 1 : 0;
  // Z-values are optional
  bZval = (KMS_ShpFileHeaderBox[4] > SHP_NOVAL &&
           KMS_ShpFileHeaderBox[5] > SHP_NOVAL) ? 1 : 0;
  // Box types:
  bBox  = (KMS_ShpFileHeaderInt[8] !=  1 &&
           KMS_ShpFileHeaderInt[8] != 11 &&
           KMS_ShpFileHeaderInt[8] != 21) ? 1 : 0;
  // store prelim the SHP File Header TO BE UPDATED
  if (fwrite(KMS_ShpFileHeaderInt, sizeof(int), 9, fh_out) != 9 ||
      fwrite(KMS_ShpFileHeaderBox, sizeof(double), 8, fh_out) != 8) {
    (void) sprintf(err_txt,
                   "Output file not writeable: %s\n  ", shpout_txt);
    (void) fclose(fh_in);
    (void) fclose(fh_out);
    return(-97);
  }
  curpos = ftell(fh_in);
  // KMS_ShpFileHeader.FileLength:  16 bits words
  FileLength = swop_int(KMS_ShpFileHeaderInt[6]) * 2;

  // loop the records
  while (curpos < FileLength) {
    recused = 0; // bytes
    (void) fread(&KMS_ShpRecordHeader,
           sizeof(KMS_ShpRecordHeader), 1, fh_in);
    (void) fwrite(&KMS_ShpRecordHeader,
           sizeof(KMS_ShpRecordHeader), 1, fh_out);
    // loop the record contents
    // KMS_ShpRecordHeader.ContentLength:  16 bits words
    RecordLength = swop_int(KMS_ShpRecordHeader.ContentLength) * 2;
    while (recused < RecordLength) {
      // Transform XY
      recused += sizeof(KMS_ShapeType);
      (void) fread(&KMS_ShapeType, sizeof(KMS_ShapeType), 1, fh_in);
      (void) fwrite(&KMS_ShapeType, sizeof(KMS_ShapeType), 1, fh_out);
      ZMType = KMS_ShapeType / 10;
      Z_Type = ZMType % 2;
      bHupd  = (Z_Type && !SOE_no_h) ? 1 : 0;
      // Read record or part of one
      switch (KMS_ShapeType) {
      case SHPT_POINT:
      case SHPT_POINTZ:
      case SHPT_POINTM:
        Rncount = 1;
        ZMcount = 1;
        break;
      case SHPT_MULTIPOINT:
      case SHPT_MULTIPOINTZ:
      case SHPT_MULTIPOINTM:
        recused += sizeof(KMS_ShpBoxPoint);
        (void) fread(&KMS_ShpBoxPoint, sizeof(double), 4, fh_in);
        (void) fread(&KMS_ShpNumPoints, sizeof(int), 1, fh_in);
        (void) fwrite(&KMS_ShpBoxPoint, sizeof(double), 4, fh_out);
        (void) fwrite(&KMS_ShpNumPoints, sizeof(int), 1, fh_out);
        Rncount = KMS_ShpNumPoints;
        ZMcount = Rncount +2;
        XminR   =  930000000.0;
        YminR   =  930000000.0;
        XmaxR   = -930000000.0;
        YmaxR   = -930000000.0;
        break;
      case SHPT_ARC:
      case SHPT_ARCZ:
      case SHPT_ARCM:
      case SHPT_POLYGON:
      case SHPT_POLYGONZ:
      case SHPT_POLYGONM:
        recused += sizeof(KMS_ShpBoxLine);
        (void) fread(&KMS_ShpBoxLine,
               sizeof(KMS_ShpBoxLine), 1, fh_in);
        boxpos   = ftell(fh_out);
        (void) fwrite(&KMS_ShpBoxLine,
               sizeof(KMS_ShpBoxLine), 1, fh_out);
        recused += copy_parts(fh_in, fh_out,
                              KMS_ShpBoxLine.NumParts, Z);
        Rncount  = KMS_ShpBoxLine.NumPoints;
        ZMcount  = Rncount +2;
        XminR    =  930000000.0;
        YminR    =  930000000.0;
        XmaxR    = -930000000.0;
        YmaxR    = -930000000.0;
        break;
      case SHPT_MULTIPATCH:
        recused += sizeof(KMS_ShpBoxLine);
        (void) fread(&KMS_ShpBoxLine,
               sizeof(KMS_ShpBoxLine), 1, fh_in);
        boxpos   = ftell(fh_out);
        (void) fwrite(&KMS_ShpBoxLine,
                sizeof(KMS_ShpBoxLine), 1, fh_out);
        recused += copy_parts(fh_in, fh_out,
                              KMS_ShpBoxLine.NumParts, Z);
        recused += copy_parts(fh_in, fh_out,
                              KMS_ShpBoxLine.NumParts, Z);
        Rncount  = KMS_ShpBoxLine.NumPoints;
        ZMcount  = Rncount +2;
        XminR    =  930000000.0;
        YminR    =  930000000.0;
        XmaxR    = -930000000.0;
        YmaxR    = -930000000.0;
        break;
      case SHPT_NULL:
        Rncount = 0;
        curpos  = ftell(fh_in);
        break;
      default:
        (void) sprintf(err_txt, "\nUknown SHP Type %d", KMS_ShapeType);
        (void) fclose(fh_in);
        (void) fclose(fh_out);
        return(-96);
      }

      if (Rncount) {
        if (bHupd) {
          curpos  = ftell(fh_in);
          Zboxpos = curpos + sizeof(KMS_ShpPoint) * Rncount;
          // FILL to Z_pos
          Rucount = 0;
          do {
            Rlcount  = ((Rncount - Rucount) < 128)
                     ?  (Rncount - Rucount) : 128;
            (void) fwrite(XY, sizeof(KMS_ShpPoint), Rlcount, fh_out);
            Rucount += Rlcount;
          } while (Rucount < Rncount);
          if (bBox) {
            // READ Header, Write Header
            recused += sizeof(Zmin_max);
            (void) fseek(fh_in, Zboxpos, SEEK_SET);
            (void) fread(&Zmin_max, sizeof(Zmin_max), 1, fh_in);
            (void) fwrite(&Zmin_max, sizeof(Zmin_max), 1, fh_out);
          }
          Z_pos = ftell(fh_out);
          (void) fseek(fh_in, curpos, SEEK_SET);
          (void) fseek(fh_out, curpos, SEEK_SET);
          ZminR =  930000000.0;
          ZmaxR = -930000000.0;
        }

        Rucount = 0;
        do {
          Rlcount  = ((Rncount - Rucount) < 128)
                   ?  (Rncount - Rucount) : 128;
          recused += sizeof(KMS_ShpPoint) * Rlcount;
          (void) fread(XY, sizeof(KMS_ShpPoint), Rlcount, fh_in);
          // Possibly: Get Z INFO
          if (bHupd) {
            curpos   = ftell(fh_in);
            recused += sizeof(double) * Rlcount;
            (void) fseek(fh_in, Z_pos, SEEK_SET);
            (void) fread(Z, sizeof(double), Rlcount, fh_in);
            (void) fseek(fh_in, curpos, SEEK_SET);
          }
          // Transform record or part of one
          for (i = 0, pXY = XY; i < Rlcount; i++, pXY++) {
            if (conv_in) {
              pXY->X /= DOUT;
              pXY->Y /= DOUT;
            }
            tr_res = gd_trans(g_lab_in, g_lab_out, pXY->Y, pXY->X,
                              *(Z+i), Slave, Slave +1, Slave +2,
                              &gh, 0, &GeoidTable, "", NULL);
            // if geo out convert rad to dg OR SET "Radian", 1.0 in .prj
            if (conv_out) {
              *Slave      *= DOUT;
              *(Slave +1) *= DOUT;
            }

            if (tr_res >= TRF_TOLLE_) {
              pXY->Y = *Slave;
              pXY->X = *(Slave +1);
              if (bHupd) *(Z+i) = *(Slave +2);
              if (bBox) {
                if (pXY->X < XminR) XminR = pXY->X;
                if (pXY->Y < YminR) YminR = pXY->Y;
                if (pXY->X > XmaxR) XmaxR = pXY->X;
                if (pXY->Y > YmaxR) YmaxR = pXY->Y;
                if (bHupd) {
                  if (*(Z +i) < ZminR) ZminR = *(Z +i);
                  if (*(Z +i) > ZmaxR) ZmaxR = *(Z +i);
                }
              } else {
                if (pXY->X < XminF) XminF = pXY->X;
                if (pXY->Y < YminF) YminF = pXY->Y;
                if (pXY->X > XmaxF) XmaxF = pXY->X;
                if (pXY->Y > YmaxF) YmaxF = pXY->Y;
                if (bHupd) {
                  if (*(Z +i) < ZminF) ZminF = *(Z +i);
                  if (*(Z +i) > ZmaxF) ZmaxF = *(Z +i);
                }
              }
              ++ qTrn;
            } else {  // Transformation error
              (void) s_status(err_txt, "kmsshp_tr", tr_res, "non");
              if (tr_res <= TRF_TOLLE_) {
                (void) fclose(fh_in);
                (void) fclose(fh_out);
                return(tr_res);
              }
            }
          }
          Rucount += Rlcount;
          // Write record or part of one
          (void) fwrite(XY, sizeof(KMS_ShpPoint), Rlcount, fh_out);
          curpos = ftell(fh_in);
          if (bHupd) {
            (void) fseek(fh_out, Z_pos, SEEK_SET);
            (void) fwrite(Z, sizeof(double), Rlcount, fh_out);
            Z_pos += sizeof(double) * Rlcount;
            (void) fseek(fh_out, curpos, SEEK_SET);
          }
        } while (Rucount < Rncount);

        // Possibly: update min/max box
        switch (KMS_ShapeType) {
        case SHPT_MULTIPOINT:
        case SHPT_MULTIPOINTZ:
        case SHPT_MULTIPOINTM:
          KMS_ShpBoxPoint[0] = XminR;
          KMS_ShpBoxPoint[1] = YminR;
          KMS_ShpBoxPoint[2] = XmaxR;
          KMS_ShpBoxPoint[3] = YmaxR;
          (void) fseek(fh_out, boxpos, SEEK_SET);
          (void) fwrite(&KMS_ShpBoxPoint, sizeof(double), 4, fh_out);
          (void) fseek(fh_out, curpos, SEEK_SET);
          break;
        case SHPT_ARC:
        case SHPT_ARCZ:
        case SHPT_ARCM:
        case SHPT_POLYGON:
        case SHPT_POLYGONZ:
        case SHPT_POLYGONM:
        case SHPT_MULTIPATCH:
          KMS_ShpBoxLine.Box[0] = XminR;
          KMS_ShpBoxLine.Box[1] = YminR;
          KMS_ShpBoxLine.Box[2] = XmaxR;
          KMS_ShpBoxLine.Box[3] = YmaxR;
          (void) fseek(fh_out, boxpos, SEEK_SET);
          (void) fwrite(&KMS_ShpBoxLine,
                  sizeof(KMS_ShpBoxLine), 1, fh_out);
          (void) fseek(fh_out, curpos, SEEK_SET);
          break;
        default: break;
        }
        if (bBox) {
          if (XminR < XminF) XminF = XminR;
          if (YminR < YminF) YminF = YminR;
          if (XmaxR > XmaxF) XmaxF = XmaxR;
          if (YmaxR > YmaxF) YmaxF = YmaxR;
          if (bHupd) {
            if (ZminR < ZminF) ZminF = ZminR;
            if (ZmaxR > ZmaxF) ZmaxF = ZmaxR;
          }
        }

        if (bHupd) {
          if (bBox) {
            Zmin_max.X = ZminR;
            Zmin_max.Y = ZmaxR;
            (void) fseek(fh_out, Zboxpos, SEEK_SET);
            (void) fwrite(&Zmin_max, sizeof(Zmin_max), 1, fh_out);
          }
          curpos = Z_pos;
          (void) fseek(fh_in, curpos, SEEK_SET);
          (void) fseek(fh_out, curpos, SEEK_SET);
        } else
        if (Z_Type) {
          // Possibly: Move Z INFO: min, max, values[Rncount]
          Rucount = 0;
          if (bZval) {
            do {
              Rlcount  = ((ZMcount - Rucount) < 128)
                       ?  (ZMcount - Rucount) : 128;
              recused += sizeof(double) * Rlcount;
              (void) fread(Z, sizeof(double), Rlcount, fh_in);
              (void) fwrite(Z, sizeof(double), Rlcount, fh_out);
              Rucount += Rlcount;
            } while (Rucount < ZMcount);
          } else {
            wpos = ftell(fh_in) + ZMcount * sizeof(double);
            (void) fseek(fh_in, wpos, SEEK_SET);
            if (ZMcount > 1) {
              Rucount  = 2;
              recused += sizeof(double) * 2;
              Slave[0] = Slave[1] = 0.0;
              (void) fwrite(Slave, sizeof(double), Rucount, fh_out);
            }
            do {
              Rlcount  = ((ZMcount - Rucount) < 128)
                       ?  (ZMcount - Rucount) : 128;
              recused += sizeof(double) * Rlcount;
              (void) fwrite(Z, sizeof(double), Rlcount, fh_out);
              Rucount += Rlcount;
            } while (Rucount < ZMcount);
          }
          Rucount  = 0;
        }

        // Possibly: Move M INFO: min, max, values[Rncount]
        if (ZMType && recused < RecordLength) {
          Rucount = 0;
          if (bMval) {
            do {
              Rlcount  = ((ZMcount - Rucount) < 128)
                       ?  (ZMcount - Rucount) : 128;
              recused += sizeof(double) * Rlcount;
              (void) fread(M, sizeof(double), Rlcount, fh_in);
              (void) fwrite(M, sizeof(double), Rlcount, fh_out);
              Rucount += Rlcount;
            } while (Rucount < ZMcount);
          } else {
            wpos = ftell(fh_in) + ZMcount * sizeof(double);
            (void) fseek(fh_in, wpos, SEEK_SET);
            if (ZMcount > 1) {
              Rucount  = 2;
              recused += sizeof(double) * 2;
              Slave[0] = Slave[1] = val_ff;
              (void) fwrite(Slave, sizeof(double), Rucount, fh_out);
            }
            do {
              Rlcount  = ((ZMcount - Rucount) < 128)
                       ?  (ZMcount - Rucount) : 128;
              recused += sizeof(double) * Rlcount;
              (void) fwrite(M, sizeof(double), Rlcount, fh_out);
              Rucount += Rlcount;
            } while (Rucount < ZMcount);
          }
        }
        curpos = ftell(fh_in);
      }
    }
    curpos = ftell(fh_in); // needed when no records to Header
  }

  // set min/max in file header
  KMS_ShpFileHeaderBox[0] = XminF;
  KMS_ShpFileHeaderBox[1] = YminF;
  KMS_ShpFileHeaderBox[2] = XmaxF;
  KMS_ShpFileHeaderBox[3] = YmaxF;
  if (Z_Type && bHupd) {
    KMS_ShpFileHeaderBox[4] = ZminF;
    KMS_ShpFileHeaderBox[5] = ZmaxF;
  }

  (void) fseek(fh_out, 0, SEEK_SET);
  (void) fwrite(KMS_ShpFileHeaderInt, sizeof(int), 9, fh_out);
  (void) fwrite(KMS_ShpFileHeaderBox, sizeof(double), 8, fh_out);
  (void) fclose(fh_out);
  (void) fclose(fh_in);

  // Copy      the .SHX file
  //shpin_txt.Replace(_T(".shp"), _T(".shx"));
  pb = strstr(shpin_txt, ".shp");
  *(pb + 3) = 'x';
  fh_in = fopen(shpin_txt, "rb");
  if (fh_in == NULL) {
    (void) sprintf(err_txt, "Input .SHX not found: %s\n  ", shpin_txt);
    return(-98);
  }
  //shpout_txt.Replace(_T(".shp"), _T(".shx"));
  pb = strstr(shpout_txt, ".shp");
  *(pb + 3) = 'x';
  fh_out = fopen(shpout_txt, "wb");
  if (fh_out == NULL) {
    (void) sprintf(err_txt, "Output .SHX not found: %s\n  ", shpin_txt);
    (void) fclose(fh_in);
    return(-99);
  }
  
  pb   = (char *) XY;
  // read the SHX File Header and store
  (void) fread(KMS_ShxFileHeaderInt, sizeof(int), 9, fh_in);
  (void) fread(KMS_ShxFileHeaderBox, sizeof(double), 8, fh_in);
  // store the SHP File Header
  KMS_ShxFileHeaderBox[0] = KMS_ShpFileHeaderBox[0];
  KMS_ShxFileHeaderBox[1] = KMS_ShpFileHeaderBox[1];
  KMS_ShxFileHeaderBox[2] = KMS_ShpFileHeaderBox[2];
  KMS_ShxFileHeaderBox[3] = KMS_ShpFileHeaderBox[3];
  if (fwrite(KMS_ShxFileHeaderInt, sizeof(int), 9, fh_out) != 9 ||
      fwrite(KMS_ShxFileHeaderBox, sizeof(double), 8, fh_out) != 8) {
    (void) sprintf(err_txt,
                   "Output file not writeable: %s\n  ", shpout_txt);
    (void) fclose(fh_in);
    (void) fclose(fh_out);
    return(-97);
  }
  // KMS_ShpFileHeader.FileLength:  16 bits words
  FileLength = swop_int(KMS_ShxFileHeaderInt[6]) * 2;
  curpos     = ftell(fh_out);
  // loop the records
  while (curpos < FileLength) {
    Rlcount  = ((FileLength - curpos) < 2048)
             ?  (FileLength - curpos) : 2048;
    curpos  += Rlcount;
    (void) fread(pb, 1, Rlcount, fh_in);
    (void) fwrite(pb, 1, Rlcount, fh_out);
  } 
  (void) fclose(fh_out);
  (void) fclose(fh_in);

  // Copy      the .DBF file
  //shpin_txt.Replace(_T(".shx"), _T(".dbf"));
  pb  = strstr(shpin_txt, ".shx");
  *pb = '\0';
  (void) strcat(shpin_txt, ".dbf");
  if (stat(shpin_txt, &buf)) {
    (void) sprintf(err_txt, "Input .DBF not found: %s\n  ", shpin_txt);
    return(-98);
  }
  FileLength = buf.st_size;
  fh_in = fopen(shpin_txt, "rb");

  //shpout_txt.Replace(_T(".shx"), _T(".dbf"));
  pb  = strstr(shpout_txt, ".shx");
  *pb = '\0';
  (void) strcat(shpout_txt, ".dbf");
  fh_out = fopen(shpout_txt, "wb");
  if (fh_out == NULL) {
    (void) sprintf(err_txt, "Output .DBF not found: %s\n  ", shpin_txt);
    (void) fclose(fh_in);
    return(-99);
  }

  pb     = (char *) XY;
  curpos = 0;
  // loop the records
  while (curpos < FileLength) {
    Rlcount  = ((FileLength - curpos) < 2048)
             ?  (FileLength - curpos) : 2048;
    curpos  += Rlcount;
    (void) fread(pb, 1, Rlcount, fh_in);
    (void) fwrite(pb, 1, Rlcount, fh_out);
  } 
  (void) fclose(fh_in);
  (void) fclose(fh_out);
  
  // if not known .prj codes then return(-95);
  pb  = strstr(shpout_txt, ".dbf");
  *pb = '\0';
  (void) strcat(shpout_txt, ".prj");
  fh_out = fopen(shpout_txt, "w");
  i      = fputshpprj(fh_out, g_lab_out);
  if (i < 0) {
    (void) strcpy(err_txt,
      (i == -2) ? "def_shp.txt  def file not found in geoid directory" :
      (i == -3) ? "def_shp.txt  is NOT a def_shp file" :
      (i == -4) ? "geo_lab unknown shape.prj format" :
      (i == -5) ? "illegal geo_label" :
      (i == -6) ? "program error" : "fputshpprj: program error");
  }
  (void) fclose(fh_out);
  *q_trf_ok = qTrn;
  return((i < 0) ? -95 : qTrn);

#undef          SHP_NOVAL

}

int                 copy_parts(
/*_____________________________*/
FILE               *fh_in   ,
FILE               *fh_out  ,
int                 NumParts,
double             *Z            ){
  int i = 0, n, *pZ;
  pZ = (int *) Z;
  while (i < NumParts) {
    n = ((NumParts -i) < 256) ? (NumParts -i) : 256;
    (void) fread(pZ, sizeof(int), n, fh_in);
    (void) fwrite(pZ, sizeof(int), n, fh_out);
    i += n;
  }
  return(sizeof(int) * NumParts);
}

int swop_int(int i) {
  int   res;
  char *pr, *pi;
  pr = (char *) (&res);
  pi = (char *) (&i) + 3;
  *(pr ++) = *(pi --);
  *(pr ++) = *(pi --);
  *(pr ++) = *(pi --);
  *(pr ++) = *(pi --);
  return(res);
}
