#!/usr/bin/python
#######################################
##Skeleton of a test suite for trlib using python 'bindings'
## This script performs some randomized tests....
## simlk, may 2011
#######################################
GEOIDS="/Geoids/" #pointer to geoid directory
import TrLib
import numpy as np
import os
import sys
import time #should really use timeit-module....
GEOIDS=os.path.dirname(__file__)+"/Geoids/" #pointer to geoid directory
D2M=9.3349049354951814e-06
TEST_SYSTEMS_2D=[["geo_wgs84","geo_ed50",12.0,54.0,D2M],
["utm32_ed50","utm32_etrs89",512200.0,6143200.0,1.0,],
["utm32_wgs84","utm33_wgs84",512200.0,6143200.0,1.0],
["dktm1","utm32_wgs84",112928.119,1117545.350,1.0],
["s34j","dktm1",284686.705,84120.484,1.0],
["kp2000j","utm32_ed50",181078.422,6117383.291,1.0]]
TEST_SYSTEMS_3D=[["geoHwgs84_h_dvr90","geoHed50_h_dvr90",12.0,54.0,100.0,D2M],
["utm32Hetrs89_h_dvr90","utm33Hetrs89_h_dnn",512200.0,6143200.0,100,1.0,],
["utm32Hwgs84_h_dnn","utm33Hwgs84_h_dvr90",512200.0,6143200.0,100,1.0],
["crt_etrs89","geoEwgs84",3436572.0354,562338.0079,5325761.9520,1.0]] 
BASE_POINT=[512200.0,6143200.0] #Et sted i Jylland?
LINE_SPLIT="*"*65
def RandomTests_2D(N=10000,repeat=3,log_file=sys.stdout):
	log_file.write("%s\n" %LINE_SPLIT)
	log_file.write("Transforming %i 2d-points...\n" %N)
	for label_in,label_out,x,y,scale in TEST_SYSTEMS_2D:
		log_file.write("%s\n" %LINE_SPLIT)
		log_file.write("Label_in: %s, Label_out: %s\n" %(label_in,label_out))
		xy=np.random.rand(N,2)*(scale*10000.0)+[x,y]
		if N>10:
			log_file.write("Center of mass: %.2f %.2f\n" %tuple(xy.mean(axis=0).tolist()))
		else:
			log_file.write("in:\n%s\n" %repr(xy))
		for i in range(repeat):
			tstart=time.clock()
			xy_out=TrLib.Transform(label_in,label_out,xy)
			log_file.write("Forward running time: %.5f s\n" %(time.clock()-tstart))
		for i in range(repeat):
			tstart=time.clock()
			xy_back=TrLib.Transform(label_out,label_in,xy_out)
			log_file.write("Inverse running time: %.5f s\n" %(time.clock()-tstart))
		if N<=10:
			log_file.write("out:\n%s\n" %repr(xy_out))
		log_file.write("Maximum tr-loop error: %.10f (m or deg)\n" %np.fabs(xy-xy_back).max())

def RandomTests_3D(N=10000,repeat=3,log_file=sys.stdout):
	log_file.write("%s\n" %LINE_SPLIT)
	log_file.write("Transforming %i 3d-points...\n" %N)
	for label_in,label_out,x,y,z,scale in TEST_SYSTEMS_3D:
		log_file.write("%s\n" %LINE_SPLIT)
		log_file.write("Label_in: %s, Label_out: %s\n" %(label_in,label_out))
		mult=[scale*10000.0,scale*10000.0,50.0]
		xyz=np.random.rand(N,3)*mult+[x,y,z]
		if N>10:
			log_file.write("Center of mass: %.2f %.2f %.2f\n" %tuple(xyz.mean(axis=0).tolist()))
		else:
			log_file.write("in:\n%s\n" %repr(xyz))
		for i in range(repeat):
			tstart=time.clock()
			xyz_out=TrLib.Transform(label_in,label_out,xyz)
			log_file.write("Forward running time: %.5f s\n" %(time.clock()-tstart))
		for i in range(repeat):
			tstart=time.clock()
			xyz_back=TrLib.Transform(label_out,label_in,xyz_out)
			log_file.write("Inverse running time: %.5f s\n" %(time.clock()-tstart))
		if N<=10:
			log_file.write("out:\n%s\n" %repr(xyz_out))
		log_file.write("Maximum tr-loop error: %.10f (m or deg)\n" %np.fabs(xyz-xyz_back).max())
	
		
def main(args):
	progname=os.path.basename(args[0])
	if "-lib" in args: #In this case we assume that input is the library that we want to test....
		libpath=args[args.index("-lib")+1]
		lib=os.path.splitext(os.path.basename(libpath))[0]
		dir=os.path.dirname(libpath)
		IS_INIT=TrLib.InitLibrary(GEOIDS,lib,dir)
	else:
		print("You can specify the TrLib-library to use by %s -lib <lib_path>" %progname)
		IS_INIT=TrLib.InitLibrary(GEOIDS)
	if not IS_INIT:
		print("Could not initialize library...")
		print("Find a proper shared library and geoid dir and try again!")
		sys.exit()
	print("Running %s at %s" %(progname,time.asctime()))
	print("Using TrLib v. '%s'" %TrLib.GetVersion())
	print("DLL is: %s" %repr(TrLib.tr_lib))
	N=0
	if "-N" in args:
		try:
			N=int(args[args.index("-N")+1])
		except:
			pass
	if N==0:
		print("You can input a parameter for the number of points as:\n%s -N <#points>" %(progname))
		N=10000
	RandomTests_2D(N)
	#for i in range(0,3):
	RandomTests_3D(N)
	sys.exit()

if __name__=="__main__":
	main(sys.argv)

	