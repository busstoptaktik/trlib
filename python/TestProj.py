#!/usr/bin/python
#######################################
##Skeleton of a test suite for trlib using python 'bindings'
## This script performs some translations of proj4 definitions and EPSG codes to kms-minilabels
## simlk, oct. 2012
#######################################
import sys,os,time
import TrLib
TESTS={"+proj=utm +zone=32 +units=m +ellps=GRS80 +nodefs": "utm32_etrs89",
"+proj=utm +zone=32 +units=m +datum=WGS84 +nodefs": "utm32_wgs84",
"+proj=latlong +ellps=GRS80 +nodefs": "geo_etrs89",
"+proj=longlat +datum=WGS84 +nodefs": "geo_wgs84",
"+proj=utm +zone=32 +units=m +datum=NAD83 +nodefs": "utm32_nad83",
"+proj=etmerc +lat_0=0 +y_0=0 +lon_0=9 +x_0=500000 +k=0.9996 +units=m +datum=WGS84": None, #as for now, won't be able to recognize an utm projection like that
"+proj=etmerc +lat_0=0 +y_0=-5000000 +lon_0=9 +x_0=200000 +k=0.999980 +units=m +ellps=GRS80" : "dktm1",
"+proj=etmerc +lat_0=0 +y_0=-5000000 +lon_0=9 +x_0=200000 +k=0.999980 +units=m +datum=etrs89": "dktm1",
"+proj=etmerc +lat_0=0 +y_0=-5000000 +lon_0=9 +x_0=200000 +k=0.999980 +units=m +datum=etrf89": "dktm1",
"+proj=latlong +datum=ed50 +nodefs": "geo_ed50",
"+proj=latlong +datum=etrs89 +vdatum=dvr90": "geoHetrs89_h_dvr90",
"+proj=utm +zone=32 +datum=wgs84 +vdatum=dnn +units=m": "utm32Hwgs84_h_dnn",
"+proj=merc +lat_ts=0 +y_0=0 +lon_0=0 +x_0=0 +k=1 +datum=WGS84 +units=m +nodefs": "mrc0_wgs84",
"+proj=merc +a=6378137 +b=6378137 +lat_ts=0 +y_0=0 +lon_0=0 +x_0=0 +k=1 +datum=WGS84 +units=m +nadgrids=@null": "webmrc",
"+proj=merc +lat_ts=10 +y_0=100000 +lon_0=0 +x_0=200000 +k=1 +datum=ed50 +units=m +nodefs": None,
"EPSG:25832": "utm32_etrs89",
"EPSG:4093": "dktm1",
"EPSG:7416": "utm32Hetrs89_h_dvr90"
}

def test():
	nerr=0
	for text in TESTS:
		mlb=TrLib.ImportLabel(text)
		if mlb is None:
			print("\nUnable to translate %s" %text)
			nerr+=1
		else:
			print("\n%s\n-> %s"%(text,mlb))
			base=TESTS[text]
			if base is not None and mlb!=base:
					nerr+=1
					print("Translation failed - should be: %s" %base)
			#Test inverse:
			out=TrLib.ExportLabel(mlb,"EPSG")
			if out is not None:
				print("To EPSG: %s->%s" %(mlb,out))
			else:
				print("No EPSG translation of %s" %mlb)
		
	return nerr

def main(args):
	progname=os.path.basename(args[0])
	IS_INIT=TrLib.IS_INIT
	if not IS_INIT:
		if "-lib" in args: #In this case we assume that input is the library that we want to test....
			libpath=args[args.index("-lib")+1]
			lib=os.path.basename(libpath)
			dir=os.path.dirname(libpath)
			IS_INIT=TrLib.InitLibrary("",lib,dir)
		else:
			print("You can specify the TrLib-library to use by %s -lib <lib_path>" %progname)
			IS_INIT=TrLib.InitLibrary("")
	if not IS_INIT:
		print("Could not initialize library...")
		print("Find a proper shared library and geoid dir and try again!")
		return -1
	print("Running %s at %s" %(progname,time.asctime()))
	print("Using TrLib v. '%s'" %TrLib.GetVersion())
	print("Shared library is: %s" %repr(TrLib.tr_lib))
	nerr=test()
	print("Errors: %d" %nerr)
	return nerr

if __name__=="__main__":
	sys.exit(main(sys.argv))
		
		