##########################################################
##Test suite for TrLib.
##Transform some standard files to compare output against prev. versions.
## Files must follow the format:
## #minilabel
## #addtional comments can be placed below as long as they are preceeded by a hash '#'
## x1 y1 (z1)
## x2 y2 (z2)
## ....
## ....
##simlk, may 2011.
##########################################################

import TrLib
import os
import sys
import time
#format for the test setup is [[filename_in,label_out,precision_out],...]
GEOIDS=os.path.dirname(__file__)+"/Geoids/" #pointer to geoid directory
TESTDIR_IN=os.path.dirname(__file__)  #modify as needed - directory should contain files mentioned below in TESTS.
TESTDIR_OUT=os.path.dirname(__file__)+"/test_output" 
TESTS=[["Jylland_2d.txt","utm33_ed50",5],
["Jylland_2d.txt","s34j",5],
["Fyn_2d.txt","dktm2",5],
["Sjaelland_2d.txt","geo_ed50",8],
["Jylland_3d.txt","s34jE",5],
["Sjaelland_3d.txt","geoHed50_h_dnn",8]]
#Todo: Add more....
def main(args):
	progname=os.path.basename(args[0])
	progname=os.path.basename(args[0])
	if "-lib" in args: #In this case we assume that input is the library that we want to test....
		lib=args[args.index("-lib")+1]
		lib=os.path.splitext(os.path.basename(args[2]))[0]
		dir=os.path.dirname(args[2])
		IS_INIT=TrLib.InitLibrary(GEOIDS,lib,dir)
	else:
		print("You can specify the TrLib-library to use by %s -lib <lib_path>" %progname)
		IS_INIT=TrLib.InitLibrary(GEOIDS)
	if not IS_INIT:
		print("Could not initialize library...")
		print("Find a proper shared library and geoid dir and try again!")
		sys.exit()
	print("Running %s at %s" %(progname,time.asctime()))
	print("Using TrLib v. %s" %TrLib.GetVersion())
	print("DLL is: %s" %repr(TrLib.tr_lib))
	if not os.path.exists(TESTDIR_OUT):
		os.mkdir(TESTDIR_OUT)
		print("Genereting output directory: %s" %os.path.realpath(TESTDIR_OUT))
	for file_in,label_out,precision in TESTS:
		file_in_path=os.path.join(TESTDIR_IN,file_in)
		if os.path.exists(file_in_path):
			file_out=os.path.splitext(file_in)[0]+"_"+label_out.replace(" ","")+".txt"
			file_out_path=os.path.join(TESTDIR_OUT,file_out)
			TrLib.TransformFile(file_in_path,label_out,file_out_path,precision)
		else:
			print("%s does not exits!" %file_in_path)
	sys.exit()
if __name__=="__main__":
	main(sys.argv)
		