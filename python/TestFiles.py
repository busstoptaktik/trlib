#!/usr/bin/python
##########################################################
##Test suite for TrLib.
## Test definition file must follow the format:
##     input_file output_file minilabel_out precision_out 
##     .....
## File paths are relative to the test definition file! (Makes portability easier).
##
##Transform some standard files to compare output against prev. versions.
## Files must follow the format:
## #minilabel
## #addtional comments can be placed below as long as they are preceeded by a hash '#'
## x1 y1 (z1)
## x2 y2 (z2)
## ....
## ....
##simlk, may-oct. 2011.
##########################################################
import TrLib
import os
import sys
import time
#format for the test setup is [[filename_in,label_out,precision_out],...]
GEOIDS=os.path.join(os.path.dirname(__file__),"Geoids/") #default pointer to geoid directory
def main(args): #progname, test_def, -lib libname (optional)
	progname=os.path.basename(args[0])
	IS_INIT=TrLib.IS_INIT
	if not IS_INIT:
		if "-lib" in args: #In this case we assume that input is the library that we want to test....
			lib=args[args.index("-lib")+1]
			lib=os.path.basename(lib)
			dir=os.path.dirname(lib)
			IS_INIT=TrLib.InitLibrary(GEOIDS,lib,dir)
		else:
			print("You can specify the TrLib-library to use by %s -lib <lib_path>" %progname)
			IS_INIT=TrLib.InitLibrary(GEOIDS)
	if not IS_INIT:
		print("Could not initialize library...")
		print("Find a proper shared library and geoid dir and try again!")
		return -1
	print("Running %s at %s" %(progname,time.asctime()))
	print("Using TrLib v. %s" %TrLib.GetVersion())
	print("Shared library is: %s" %repr(TrLib.tr_lib))
	try:
		f=open(args[1])
	except:
		print("Test definition must be specified as first argument!")
		return -1
	job_dir=os.path.dirname(args[1])
	print("Using job definition in file %s" %args[1])
	for line in f:
		sline=line.split()
		if len(sline)==0:
			continue
		if sline[0][0]=="#":
			continue
		try:
			file_in,file_out,label_out,precision=line.split()
			precision=int(precision)
		except:
			print("Wrong definition:\n%s" %line)
			continue
		file_in=os.path.join(job_dir,file_in)
		file_out=os.path.join(job_dir,file_out)
		if os.path.exists(file_in):
			TrLib.TransformFile(file_in,label_out,file_out,precision)
		else:
			print("%s does not exits!" %file_in)
	f.close()
	return 0
if __name__=="__main__":
	sys.exit(main(sys.argv))
		