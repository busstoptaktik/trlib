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
 
/*------------------------------------------------------------------------------
                          K M S  F N C S . H
------------------------------------------------------------------------------ */

/* Connects the release or debug version of the LIB file 
 (for further connection to KmsFncs.dll).
 This DLL contains many functions for i/o of KMS textfiles, 
 fncs used in the KMS transformation system etc

 - the debug version of the LIB and the DLL file are :
 KmsFncsD.lib and KmsFncsD.dll
*/


#if !defined(__KMSFNCS_H)
#define __KMSFNCS_H

#include "geo_lab.h"
#include "kms_math.h"
#include "typ_dec.h"
#include "trlib_intern.h"
#include "trlib_api.h"


#if defined __cplusplus
extern "C" { 
#endif
#include "bh_lat.h"
#include "bh_lmb.h"
#include "bshlm1.h"
#include "bshlm2.h"
#include "bshlm2g.h"
#include "c_gps_c.h"
#include "cic.h"
#include "clenshaw.h"
#include "conv_crd.h"
#include "conv_hgt.h"
#include "conv_rgn.h"
#include "ctc.h"
#include "cwtJD.h"
#include "dks_tr.h"
#include "doc_def_data.h"
#include "eetmtu_g.h"
#include "erad.h"
#include "fgetdt.h"
#include "fgetg.h"
#include "fgetlhtx.h"
#include "fgetln_kms.h"
#include "fgetshpprj.h"
#include "fo_g_tr.h"
#include "fputg.h"
#include "fputshpprj.h"
#include "fputcw.h"
#include "gatg.h"
#include "gd_trans.h"
#include "gen_pol.h"
#include "get_areacf.h"
#include "get_areaf.h"
#include "get_ha_s.h"
#include "get_mlb.h"
#include "get_tpd.h"
#include "get_zone.h"
#include "getsinexlin.h"
#include "getsinexver.h"
#include "gmb1.h"
#include "gmb2.h"
#include "gtc.h"
#include "grim.h"
#include "hlm_tr.h"
#include "inpoly.h"
#include "labchsum.h"
#include "lord.h"
#include "metadata.h"
#include "pom.h"
#include "ptg.h"
#include "ptg_d.h"
#include "putsinexval.h"
#include "putsinexver.h"
#include "s34jtos.h"
#include "s34stkk.h"
#include "s_status.h"
#include "sand_afst.h"
#include "sand_areal.h"
#include "set_dtm.h"
#include "set_dtm_1.h"
#include "set_grs.h"
#include "set_tpd.h"
#include "set_trc.h"
#include "setDtrc.h"
#include "sftr.h"
#include "sgetcw.h"
#include "sgetdt.h"
#include "sgetg.h"
#include "sputg.h"
#include "sputshpprj.h"
#include "sputcw.h"
#include "t_status.h"
#include "tcts_u.h"
#include "transformations.h"
#include "utgs.h"
#include "utmtmgr.h"
#include "utsb.h"
#include "v_red.h"
#include "v_std.h"
#if defined __cplusplus
}
#endif

#endif

