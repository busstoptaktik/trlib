uni_trans        updates

KmsFnc has been compiled in version of MS Develloper:
  "win32"  "x86"  
  :   "Microsoft.VC80.CRT"  version="8.0.50727.4053"
  :   "Microsoft.VC80.MFC"  version="8.0.50727.4053" 
See KmsFncs.dll.intermediate.manifest

(Security updates:
Microsoft Visual C++ 2008 SP1
http://www.microsoft.com/downloads/details.aspx?familyid=A5C84275-3B97-4AB7-A40D-3802B2AF5FC2&displaylang=en

Microsoft Visual C++ 2005 Service Pack 1 ATL Security Update
http://www.microsoft.com/downloads/details.aspx?familyid=766a6af7-ec73-40ff-b072-9112bab119c2&displaylang=en
)

KmsFncs.def defines the export files.
IncKms.zip  contains all the header files
SrcKms.zip  contains all the source files

KmsFncs.h placed in IncKms.zip is a common headerfile which
includes all header files with an "extern C" command. It also
has a pragma definition to KmsFncs.lib and KmsFncsD.lib. This
PATH may be corrected or deleted.

- means: internal use / observation


---------------------------------------------------------------


uni_trans        updates                            2. aug 2011

The definition part of the label system has been redesigned to use
    def_lab.txt
Consequently many sources and some include files have been updated.

The DHHN92 vertical referenceframe of Germany has been linked to DE_ETRS89.
The transformations from FCSVR10 to DHHN92 is implemented in the 1 cm level.
Transformation from DHHN92 to DVR90 is NOT possible.
Transformations to DHHN92 from CRT_FEH10 may give smaller errors.
Transformations to DVR90 from CRT_FEH10 may give smaller errors.

The geoid fbeltgeoid.bin is an extract of the official DE geoid to DHHN92
    covering the Belt Project only!       ---------------------------------------------------------------


uni_trans        opdateringer  pr 9. marts 2009

- betyder: eget brug / observation

NYT: Geoidekataloget skal indeholde filerne:
     def_dtm.txt, def_grs.txt, def_shp.txt


conv_crd: mere fejlsikker version
     NYT: geoi  (lon, lat) (til forskel fra geo (lat, lon)
     NYT: utmi  (E, N)     (til forskel fra utm (N, E)
     NYT: dktm<i>  (E, N) <i> = {1,2,3,4}
     NYT: kfmr  København Frederiksberg Metro Ring
     NYT: tm<zz> betegner en Gauss-Krüger med zonenr centralmeridian <zz>
                 <zz> SKAL VÆRE multipla af 3.   
     qbc kaldes nu: Qatar Bahrain Causeway 2001


conv_lab: mere fejlsikker version
NB:NB:NB: NYT KALD:: ALTID 3 parametre!!!!::
          enten   conv_lab("utm32Hetrs89_h_dvr90", &g_lab, "");
          eller   conv_lab("itmHetrs89_h_dvr90", &g_lab, "0dg  0km  9dg 200km  0.99991");
          eller   conv_lab("input", &g_lab, file_pointer);

sputg:
fputg:    hvis typedec->df == 255 udskrives bedst mulig terminator
          {km, m, dm, cm} eller {dg, nt, sx} 

set_dtm   :   KRÆVER filen def_dtm.txt i geoidekataloget!!!
set_grs   :   KRÆVER filen def_grs.txt i geoidekataloget!!!


NYT:
etrs89_trans: transformationer mellem projektioner i ETRS89
fputshpprj:   skriver en SHAPE .prj fil fra en geo_lab
          :   KRÆVER filen def_shp.txt i geoidekataloget!!!
fgetshpprj:   danner en geo_lab fra en .prj fil
          :   KRÆVER filen def_shp.txt i geoidekataloget!!!
kmsshp_tr :   transformerer en SHAPE fil til SHAPE.
fgetdt    :   læser en dato, tid.

DELTAG TIL AT FORBEDRE KMSTRANS::
Specielt SHAPE trans delen er skrevet til ESRI.prj formatet
som det bruges i CARIS. Har man problemer med de transformerede
.prj filer bedes man beskrive problemet med filer og sende 
det til ke@space.dtu.dk

--------------------------------------------------

uni_trans        opdateringer  pr 1. maj 2007

- betyder: eget brug / observation


conv_crd: mere fejlsikker version
          nad83g ændret
          qatar National system inkluderet: qng
          stereografisk proj lidt forbedret
       -  ITRFyy kræver epoke

conv_lab: mere fejlsikker version
       -  ny: condition label
       -  ny: 2D double tabel label
       -  ny: time series utm
       -  ny: condition heights: ch
       -  ny: condition potentials:chpot

conv_tab: udvidet med T3D_LAB, T2D_LAB og TDD_LAB
       -  ny: grsoek<map>


dk_trans: udvidet fra 2D til 3D kald !!! H_out = H_in


fe_trans: udvidet fra 2D til 3D kald !!! H_out = H_in
          ALVORLIG FEJL RETTET


fgetg:    local variabel class omdøbt til clss

fgetln:   local variabel class omdøbt til klasse

gatg:     mere præcis beregning med tilføjelse af 5 grad

gd_trans: ny behandling af grønlands transformationer !!!

geoid_i:  mindre fejl rettet

geoid_t:  ny grsoek<map>

gettabdir: lidt forbedret

grid_val: udvidet med T3D_LAB, T2D_LAB og TDD_LAB

ng_trans: NY FUNKTION transformation af GR

ptg:      udvidet grænser for UTM
          flere projektioner respekterer B0

set_dtm:  ny: nahrwan: Qatar Nahwan 1967
          ny: qng: Qatar National Gridsystem
          ny: etrsyy og etrfyy accepteres som euref89
          ny: 7-par for qornoq -> gr96
          ny: 7-par for scosd -> gr96
          ny: 7-par for ammlk -> gr96
          benyttes ikke længere: 7-par for nad83g -> nwl9d

set_grs:  ny 'ellipsoide': spheric 

set_trc:  clenshaw rækkerne udvidet med 5. grad

tab_adm_f: udvidet med T3D_LAB og T2D_LAB
           forbedret fejlreaktion

tab3d_val: udvidet med T3D_LAB og T2D_LAB

tabdd_c:   ny funtion til TDD_LAB
tabdd_i:   ny funtion til TDD_LAB
tabdd_val: ny funtion til TDD_LAB

tr_DSFL:   2 mindre rettelser

