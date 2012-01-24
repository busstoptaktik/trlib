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
 
//------------------------------------------------------------------------------
//                          K M S  F N C S . H
//------------------------------------------------------------------------------

// Connects the release or debug version of the LIB file 
// (for further connection to KmsFncs.dll).
// This DLL contains many functions for i/o of KMS textfiles, 
// fncs used in the KMS transformation system etc

// - the debug version of the LIB and the DLL file are :
// KmsFncsD.lib and KmsFncsD.dll



#if !defined(__KMSFNCS_H)
#define __KMSFNCS_H

#include "geo_lab.h"
#include "geoid_d.h"
#include "kms_math.h"
#include "kms_shape.h"
#include "typ_dec.h"



extern "C" {
#include "bh_lat.h"
#include "bh_lmb.h"
#include "bshlm1.h"
#include "bshlm2.h"
#include "bshlm2g.h"
#include "c_gps_c.h"
#include "c_tabdir_file.h"
#include "ch_tabdir_par.h"
#include "cic.h"
#include "clenshaw.h"
#include "conv_cnd.h"
#include "conv_crd.h"
#include "conv_hgt.h"
#include "conv_lab.h"
#include "conv_obs.h"
#include "conv_plm.h"
#include "conv_rgn.h"
#include "conv_spc.h"
#include "conv_tab.h"
#include "ctc.h"
#include "cwfJD.h"
#include "cwtJD.h"
#include "dk_trans.h"
#include "dks_tr.h"
#include "ee_trans.h"
#include "eetmtu_g.h"
#include "erad.h"
#include "etrs89_trans.h"
#include "f_nxt.h"
#include "fe_trans.h"
#include "fgetdt.h"
#include "fgetg.h"
#include "fgetlhtx.h"
#include "fgetln_kms.h"
#include "fgetshpprj.h"
#include "fh_trans.h"
#include "fo_g_tr.h"
#include "fputcw.h"
#include "fputg.h"
#include "fputshpprj.h"
#include "fputst.h"
#include "gatg.h"
#include "gd_t_val.h"
#include "gd_trans.h"
#include "gen_pol.h"
#include "geoid_c.h"
#include "geoid_i.h"
#include "geoid_t.h"
#include "get_areacf.h"
#include "get_areaf.h"
#include "get_ha_s.h"
#include "get_mlb.h"
#include "get_tpd.h"
#include "get_zone.h"
#include "getsinexlin.h"
#include "getsinexver.h"
#include "gettabdir.h"
#include "gmb1.h"
#include "gmb2.h"
#include "grid_val.h"
#include "gtc.h"
#include "h_trans.h"
#include "hlm_tr.h"
#include "htr_init.h"
#include "i_tabdir_file.h"
#include "inpoly.h"
#include "itrf_trans.h"
#include "kmsshp_tr.h"
#include "labchsum.h"
#include "ng_trans.h"
#include "pla_adm_f.h"
#include "prn_cho.h"
#include "ptg.h"
#include "ptg_d.h"
#include "putsinexval.h"
#include "putsinexver.h"
#include "s34jtos.h"
#include "s34stkk.h"
#include "s_status.h"
#include "sand_afst.h"
#include "sand_areal.h"
#include "scn_cho.h"
#include "set_dtm.h"
#include "set_dtm_1.h"
#include "set_grs.h"
#include "set_itrf_c.h"
#include "set_tpd.h"
#include "set_trc.h"
#include "setDtrc.h"
#include "sftr.h"
#include "sgetcw.h"
#include "sgetdt.h"
#include "sgetg.h"
#include "sgetst.h"
#include "sputcw.h"
#include "sputg.h"
#include "sputshpprj.h"
#include "sputst.h"
#include "srch_plate.h"
#include "srch_tab_typ.h"
#include "t_status.h"
#include "tab_adm_f.h"
#include "tab_doc_f.h"
#include "tab3d_c.h"
#include "tab3d_i.h"
#include "tab3d_val.h"
#include "tabdd_c.h"
#include "tabdd_i.h"
#include "tabdd_val.h"
#include "tcts_u.h"
#include "tpd_doc_f.h"
#include "tr_DSFL.h"
#include "u32_to_map.h"
#include "utgs.h"
#include "utmtmgr.h"
#include "utsb.h"
#include "v_red.h"
#include "v_std.h"
}

#ifdef _DEBUG
#pragma comment(lib, "C:/D_DREV/DotNetProjects/KmsFncs/Debug/KmsFncsD.lib")
#else
#pragma comment(lib, "C:/D_DREV/DotNetProjects/KmsFncs/Release/KmsFncs.lib")
#endif

#endif

