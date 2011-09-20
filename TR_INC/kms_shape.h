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
 
#ifndef _ESRISHAPE_H_INCLUDED
#define _ESRISHAPE_H_INCLUDED

/******************************************************************************
 * $Id: esrishape.h,  v. 1.0 2006/02/02 12:12:08 $
 *
 * Project:  KmsFnc
 * Purpose:  Primary include ESRI SHAPE format handling in KmsTrans
 * Author:   Karsten Engsager, ke@spacecenter.dk
 *
 ******************************************************************************
 * Copyright (c) 2006, Karsten Engsager
 *
 * This software is available as open sofware
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 *
 * $Log: esrishape.h,v $
 * Revision 1.0 2006/02/02 12:12:08 ke
 *
 */

#include <stdio.h>

/* -------------------------------------------------------------------- */
/*      Shape types (nSHPType)                                          */
/* -------------------------------------------------------------------- */
#define SHPT_NULL         (0)
#define SHPT_POINT        (1)
#define SHPT_ARC          (3)
#define SHPT_POLYGON      (5)
#define SHPT_MULTIPOINT   (8)
#define SHPT_POINTZ       (11)
#define SHPT_ARCZ         (13)
#define SHPT_POLYGONZ     (15)
#define SHPT_MULTIPOINTZ  (18)
#define SHPT_POINTM       (21)
#define SHPT_ARCM         (23)
#define SHPT_POLYGONM     (25)
#define SHPT_MULTIPOINTM  (28)
#define SHPT_MULTIPATCH   (31)

/* -------------------------------------------------------------------- */
/*      Part types - everything but SHPT_MULTIPATCH just uses           */
/*      SHPP_RING.                                                      */
/* -------------------------------------------------------------------- */

#define SHPP_TRISTRIP   (0)
#define SHPP_TRIFAN     (1)
#define SHPP_OUTERRING  (2)
#define SHPP_INNERRING  (3)
#define SHPP_FIRSTRING  (4)
#define SHPP_RING       (5)


/************************************************************************/
/*                             SHP Basics.                              */
/************************************************************************/

/* May NOT be USED in a uprounding to multipla of 8-bytes 
struct SHPFileHeader
{
  int     FileCode;   / * 9994 * /
  int       Unused1;
  int       Unused2;
  int       Unused3;
  int       Unused4;
  int       Unused5;
  int     FileLength;
  int     Version;      / * 1000 * /
  int     ShapeType;
  double  BoundsXMin;
  double  BoundsYMin;
  double  BoundsXMax;
  double  BoundsYMax;
  double  BoundsZMin;   / * unused => 0.0 * /
  double  BoundsZMax;   / * unused => 0.0 * /
  double  BoundsMMin;   / * unused => 0.0 * /
  double  BoundsMMax;   / * unused => 0.0 * /
} KMS_ShpFileHeader, KMS_ShxFileHeader; */ 


struct SHPRecordHeader
{
  int     RecordNumber;
  int     ContentLength;
};

struct SHPRPoint
{   /*  SHPType = 1 */
    double      X;
    double      Y;
};

/* May NOT be USED in a uprounding to multipla of 8-bytes
struct SHPRBoxPoint
{   / *  SHPType = 8:MultiPoint, 28:MultiPointM, 18:MultiPointZ * /
    double      Box[4];  / * Xmin, Ymin, Xmax, Ymax * /
    int     NumPoints;
} KMS_ShpBoxPoint;  */

struct SHPRBoxLine
{   /*  SHPType = 3:PolyLine,  5:Polygon,  13:PolyLineZ */
    /*  SHPType = 15:PolygonZ, 31:MultiPatch */
    double      Box[4];  /* Xmin, Ymin, Xmax, Ymax */
    int     NumParts;
    int     NumPoints;
};

struct SHPRMinMax
{   /*  SHPType = 9:PointM,    28:MultipointM,  23:PolyLineM */
    /*  SHPType = 25:PolygonM, 18:MultiPointZ,  13:PolyLineZ */
    /*  SHPType = 15:PolygonZ, 31:MultiPatch */
    double      Min;
    double      Max;
};

#endif /* ndef _ESRISHAPE_H_INCLUDED */
