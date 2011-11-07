#!/usr/bin/python
#######################################
##Skeleton of a test suite for trlib using python 'bindings'
## This script performs some randomized tests....
## simlk, may 2011
#######################################
GEOIDS="./Geoids/" #default pointer to geoid directory
import TrLib
try:
	import numpy as np
except:
	HAS_NUMPY=False
	import random
else:
	HAS_NUMPY=True
import os
import sys
import time #should really use timeit-module....
import threading
GEOIDS=os.path.dirname(__file__)+"/Geoids/" #pointer to geoid directory
D2M=9.3349049354951814e-06
TEST_SYSTEMS_2D=[["utm32_wgs84","utm33_wgs84",512200.0,6143200.0,1.0],
["dktm1","utm32_wgs84",112928.119,1117545.350,1.0],
["geo_wgs84","geo_ed50",12.0,54.0,D2M],
["utm32_ed50","utm32_etrs89",512200.0,6143200.0,1.0,],
["utm32_wgs84","utm33_wgs84",512200.0,6143200.0,1.0],
["dktm1","utm32_wgs84",112928.119,1117545.350,1.0],
["s34j","dktm1",284686.705,84120.484,1.0],
["geo_wgs84","s34s",11.18,54.65,D2M],
["kp2000j","utm32_ed50",181078.422,6117383.291,1.0]]

#["geo_wgs84","GR_utm22_gr96",-52.23,64.68,D2M],
#["geo_wgs84","FO_fotm",-6.8,62.0,D2M]]
TEST_SYSTEMS_3D=[["geoHwgs84_h_dvr90","geoHed50_h_dvr90",12.0,54.0,100.0,D2M],
["utm32Hetrs89_h_dvr90","utm33Netrs89",512200.0,6143200.0,100,1.0,],
["utm32Hwgs84_h_dnn","utm33Hwgs84_h_dvr90",512200.0,6143200.0,100,1.0],
["crt_etrs89","geoEwgs84",3436572.0354,562338.0079,5325761.9520,1.0],
["GR_geoEwgs84","GR_utm22Ngr96",-52.23,64.68,200.0,D2M],
["DK_geoHwgs84_h_dvr90","fcsH_h_fcsvr10",11.18,54.65,100,D2M], #pretty close to bridge project.
["geoNwgs84","crt_wgs84",14.75,54.10,100,D2M]]
#["utm32Nwgs84","geoEed50",512200.1,6143200.1,100.0,1.0]]
THREAD_SAFE_3D=[["geoHwgs84_h_dvr90","geoHed50_h_dvr90",12.0,54.0,100.0,D2M],
["utm32Hetrs89_h_dvr90","utm33Netrs89",512200.0,6143200.0,100,1.0,],
["utm32Hwgs84_h_dnn","utm33Hwgs84_h_dvr90",512200.0,6143200.0,100,1.0],
["crt_etrs89","geoEwgs84",3436572.0354,562338.0079,5325761.9520,1.0]] #,
#["GR_geoEwgs84","GR_utm22Ngr96",-52.23,64.68,200.0,D2M]] #since it is known that Fehmarn transf. are NOT thread safe! Well now they are!!
BASE_POINT=[512200.0,6143200.0] #Et sted i Jylland?
LINE_SPLIT="*"*65
THREAD_TEST=False #Flag to force a break on error when running thread test

def SetThreadMode(): #since  Fehmarn transformations are known NOT to be thread safe at the moment!
	global THREAD_TEST
	global TEST_SYSTEMS_3D
	TEST_SYSTEMS_3D=THREAD_SAFE_3D #NOT NEEDED ANYMORE!
	THREAD_TEST=True
			
def RandomPoints(N,dim,x,y,z=0,scale=1):
	mult=[scale*10000.0,scale*10000.0,50.0]
	if HAS_NUMPY:
		if dim==2:
			return np.random.rand(N,2)*(scale*10000.0)+[x,y]
		else:
			return np.random.rand(N,3)*mult+[x,y,z]
	xyz=[]
	for i in xrange(N):
		xy=[x+random.random()*scale*10000,y+random.random()*scale*10000]
		if dim==3:
			xy.append(random.random()*50)
		xyz.append(xy)
	return xyz
		
def RandomTests_2D(N=10000,repeat=3,log_file=sys.stdout):
	nerr=0
	log_file.write("%s\n" %LINE_SPLIT)
	log_file.write("Transforming %i 2d-points...\n" %N)
	for label_in,label_out,x,y,scale in TEST_SYSTEMS_2D:
		log_file.write("%s\n" %LINE_SPLIT)
		log_file.write("Label_in: %s, Label_out: %s\n" %(label_in,label_out))
		xy=RandomPoints(N,2,x,y,scale=scale)
		if N<=10:
			log_file.write("in:\n%s\n" %repr(xy))
		elif HAS_NUMPY:
			log_file.write("Center of mass: %.2f %.2f\n" %tuple(xy.mean(axis=0).tolist()))
		for i in range(repeat):
			tstart=time.clock()
			try:
				xy_out=TrLib.Transform(label_in,label_out,xy)
			except Exception,msg:
				log_file.write(repr(msg)+"\n")
				log_file.write("Last error: %d\n" %TrLib.GetLastError())
				nerr+=1
				if THREAD_TEST:
					return nerr
			else:
				log_file.write("Forward running time: %.5f s\n" %(time.clock()-tstart))
		log_file.write("Running inverse...\n")
		for i in range(repeat):
			tstart=time.clock()
			try:
				xy_back=TrLib.Transform(label_out,label_in,xy_out)
			except Exception,msg:
				print(repr(msg))
				log_file.write("Last error: %d\n" %TrLib.GetLastError())
				nerr+=1
				if THREAD_TEST:
					return nerr
			else:
				log_file.write("Inverse running time: %.5f s\n" %(time.clock()-tstart))
		if N<=10:
			log_file.write("out:\n%s\n" %repr(xy_out))
		if HAS_NUMPY:
			log_file.write("Maximum tr-loop error: %.10f (m or deg)\n" %np.fabs(xy-xy_back).max())
	return nerr

def RandomTests_3D(N=10000,repeat=3,log_file=sys.stdout):
	nerr=0
	if THREAD_TEST:
		id=str(threading.current_thread().id)
	else:
		id="main"
	log_file.write("%s\n" %LINE_SPLIT)
	log_file.write("Thread: %s, Transforming %i 3d-points...\n" %(id,N))
	for label_in,label_out,x,y,z,scale in TEST_SYSTEMS_3D:
		log_file.write("%s\n" %LINE_SPLIT)
		log_file.write("Thread: %s, Label_in: %s, Label_out: %s\n" %(id,label_in,label_out))
		xyz=RandomPoints(N,3,x,y,z,scale)
		if N<=10:
			log_file.write("in:\n%s\n" %repr(xyz))
		elif HAS_NUMPY:
			log_file.write("Center of mass: %.2f %.2f %.2f\n" %tuple(xyz.mean(axis=0).tolist()))
		for i in range(repeat):
			tstart=time.clock()
			try:
				xyz_out=TrLib.Transform(label_in,label_out,xyz)
			except Exception,msg:
				log_file.write(repr(msg)+"\n")
				log_file.write("Last error: %d\n" %TrLib.GetLastError())
				nerr+=1
				if THREAD_TEST:
					return nerr
			else:
				log_file.write("Forward running time: %.5f s\n" %(time.clock()-tstart))
		log_file.write("Running inverse...\n")
		for i in range(repeat):
			tstart=time.clock()
			try:
				xyz_back=TrLib.Transform(label_out,label_in,xyz_out)
			except Exception,msg:
				log_file.write(repr(msg)+"\n")
				log_file.write("Last error: %d\n" %TrLib.GetLastError())
				nerr+=1
				if THREAD_TEST:
					return nerr
			else:
				log_file.write("Inverse running time: %.5f s\n" %(time.clock()-tstart))
				if HAS_NUMPY and N<=10:
					err_xy=np.fabs(xyz[:,0:2]-xyz_back[:,0:2]).max()
					err_z=np.fabs(xyz[:,2]-xyz_back[:,2]).max()
					if "geo" in label_in[0:6]:
						err_xy/=D2M
					if max(err_xy,err_z)>0.01:
						log_file.write("Maximum tr-loop error: xy: %.10f m z: %.10f m\n" %(err_xy,err_z))
						log_file.write("in: %s\n" %repr(xyz_out))
						log_file.write("back: %s\n" %repr(xyz_back))
						
			
		if N<=10:
			log_file.write("out:\n%s\n" %repr(xyz_out))
		if HAS_NUMPY:
			err_xy=np.fabs(xyz[:,0:2]-xyz_back[:,0:2]).max()
			err_z=np.fabs(xyz[:,2]-xyz_back[:,2]).max()
			if "geo" in label_in[0:6]:
				err_xy/=D2M
			log_file.write("Maximum tr-loop error: xy: %.10f m z: %.10f m\n" %(err_xy,err_z))
	return nerr
	
		
def main(args):
	progname=os.path.basename(args[0])
	IS_INIT=TrLib.IS_INIT
	if not IS_INIT:
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
		return -1
	print("Running %s at %s" %(progname,time.asctime()))
	print("Using TrLib v. '%s'" %TrLib.GetVersion())
	print("Shared library is: %s" %repr(TrLib.tr_lib))
	N=0
	if "-N" in args:
		try:
			N=int(args[args.index("-N")+1])
		except:
			pass
	if N==0:
		print("You can input a parameter for the number of points as:\n%s -N <#points>" %(progname))
		N=1000
		if HAS_NUMPY:
			N=10000
	nerr=0
	nerr+=RandomTests_2D(N,log_file=sys.stdout)
	nerr+=RandomTests_3D(N,log_file=sys.stdout)
	return nerr

if __name__=="__main__":
	sys.exit(main(sys.argv))

	