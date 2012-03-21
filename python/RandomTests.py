#!/usr/bin/python
#######################################
##Skeleton of a test suite for trlib using python 'bindings'
## This script performs some randomized tests....
## simlk, may 2011
#######################################
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
D2M=9.3349049354951814e-06
TEST_SYSTEMS_2D=[["utm32_wgs84","utm33_wgs84",512200.0,6143200.0,0,1.0],
["geo_wgs84","geo_ed50",12.0,55.0,0,D2M],
["utm32_ed50","utm32_etrs89",512200.0,6143200.0,0,1.0,],
["utm32_wgs84","utm33_wgs84",512200.0,6143200.0,0,1.0],
["utm32_wgs84","s34j",512200.0,6143200.0,0,1.0],
["utm32_wgs84","dktm1",512200.0,6143200.0,0,1.0],
["utm32_wgs84","sb",625000.0,6133600.0,0,0.1],
["utm32_wgs84","gs",512200.0,6143200.0,0,1.0],
["utm33_wgs84","gsb",495000.0,6108500.0,0,1.0],
["geo_wgs84","s34s",11.18,54.65,0,D2M],
["kp2000j","utm32_ed50",181078.422,6117383.291,0,1.0],
["s34j","os",265000.0,50000.0,0,1.0],
["s34s","kk",71000.0,141000.0,0,0.2],
["geo_wgs84","GR_utm22_gr96",-52.23,64.68,0,D2M],
["geo_wgs84","FO_fotm",-6.8,62.0,0,D2M],
["geo_wgs84","FO_fk89",-6.8,62.0,0,D2M],
["geo_wgs84","FO_fu50",-6.8,62.0,0,D2M],
["utm32_wgs84","webmrc",512200.0,6143200.0,0,1.0]]
TEST_SYSTEMS_3D=[["geoHwgs84_h_dvr90","geoHed50_h_dvr90",12.0,54.0,100.0,D2M],
["utm32Ewgs84","utm33Eed50",512200.0,6143200.0,100.0,1.0],
["utm32Hetrs89_h_dvr90","utm33Netrs89",512200.0,6143200.0,100,1.0,],
["utm32Hwgs84_h_dnn","utm33Hwgs84_h_dvr90",512200.0,6143200.0,100,1.0],
["crt_etrs89","geoEwgs84",3436572.0354,562338.0079,5325761.9520,1.0],
["GR_geoEwgs84","GR_utm22Ngr96",-52.23,64.68,200.0,D2M],
#["DK_geoHwgs84_h_dvr90","fcsH_h_fcsvr10",11.18,54.65,100,D2M*0.4], #pretty close to bridge project.
["geoNwgs84","crt_wgs84",14.75,54.10,100,D2M]]
#["utm32Nwgs84","geoEed50",512200.1,6143200.1,100.0,1.0]]
THREAD_SAFE_3D=[["geoHwgs84_h_dvr90","geoHed50_h_dvr90",12.0,55.0,100.0,D2M],
["utm32Hetrs89_h_dvr90","utm33Netrs89",512200.0,6143200.0,100,1.0,],
["utm32Hwgs84_h_dnn","utm33Hwgs84_h_dvr90",512200.0,6143200.0,100,1.0],
["utm32Hwgs84_h_dnn","utm33Hwgs84_h_dvr90",512200.0,6143200.0,100,1.0],
["crt_etrs89","geoEwgs84",3436572.0354,562338.0079,5325761.9520,1.0]] #,

FH_TEST=[["DK_geoHwgs84_h_dvr90","fcsH_h_fcsvr10",11.132,54.47,10,D2M*0.2]] #pretty close to bridge project.
#["GR_geoEwgs84","GR_utm22Ngr96",-52.23,64.68,200.0,D2M]] #since it is known that Fehmarn transf. are NOT thread safe! Well now they are!!
BASE_POINT=[512200.0,6143200.0] #Et sted i Jylland?
LINE_SPLIT="*"*65
THREAD_TEST=False #Flag to force a break on error when running thread test

def SetThreadMode(): #since  Fehmarn transformations are known NOT to be thread safe at the moment!
	global THREAD_TEST
	global TEST_SYSTEMS_3D
	TrLib.SetThreadMode()
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
	nerr=RandomTests(TEST_SYSTEMS_2D,2,N,repeat,log_file)
	return nerr

def RandomTests_3D(N=10000,repeat=3,log_file=sys.stdout):
	nerr=RandomTests(TEST_SYSTEMS_3D,3,N,repeat,log_file)
	return nerr


def RandomTests(TESTS,dim=3,N=10000,repeat=3,log_file=sys.stdout):
	nerr=0
	id=str(threading.current_thread().name)
	log_file.write("%s\n" %LINE_SPLIT)
	log_file.write("Thread: %s, Transforming %d %dd-points...\n" %(id,N,dim))
	for test in TESTS:
		if len(test)==6:
			label_in,label_out,x,y,z,scale=test
			geoid=""
		else:
			label_in,label_out,x,y,z,scale,geoid=test
			
		log_file.write("%s\n" %LINE_SPLIT)
		log_file.write("Label_in: %s, Label_out: %s\n" %(label_in,label_out))
		xyz=RandomPoints(N,dim,x,y,z,scale)
		if N<=10:
			log_file.write("in:\n%s\n" %repr(xyz))
		elif HAS_NUMPY:
			log_file.write("Center of mass: %s\n" %xyz.mean(axis=0).tolist())
		nok=0
		try:
			ct=TrLib.CoordinateTransformation(label_in,label_out,geoid)
		except Exception,msg:
			log_file.write(repr(msg)+"\n")
			sys.stderr.write(repr(msg)+"\n")
			msg="Failed to open %s->%s, geoid: %s, last error: %d\n" %(label_in,label_out,geoid,TrLib.GetLastError())
			log_file.write(msg)
			sys.stderr.write(msg)
			nerr+=1
			if THREAD_TEST:
				return nerr
			continue
		for i in range(repeat):
			tstart=time.clock()
			try:
				xyz_out=ct.Transform(xyz)
			except Exception,msg:
				log_file.write(repr(msg)+"\n")
				sys.stderr.write(repr(msg)+"\n")
				msg="From: %s To: %s, Last error: %d\n" %(label_in,label_out,TrLib.GetLastError())
				log_file.write(msg)
				sys.stderr.write(msg)
				if N<10:
					sys.stderr.write("Input: %s\n" %repr(xyz))
				nerr+=1
				if THREAD_TEST:
					return nerr
			else:
				nok+=1
				log_file.write("Forward running time: %.5f s\n" %(time.clock()-tstart))
		
		if nok==0:
			continue
		log_file.write("Running inverse...\n")
		for i in range(repeat):
			tstart=time.clock()
			try:
				xyz_back=ct.InverseTransform(xyz_out)
			except Exception,msg:
				log_file.write(repr(msg)+"\n")
				sys.stderr.write(repr(msg)+"\n")
				msg="From: %s To: %s, Last error: %d\n" %(label_out,label_in,TrLib.GetLastError())
				log_file.write(msg)
				sys.stderr.write(msg)
				if N<10:
					sys.stderr.write("Input: %s\n" %repr(xyz_out))
				nerr+=1
				if THREAD_TEST:
					return nerr
			else:
				log_file.write("Inverse running time: %.5f s\n" %(time.clock()-tstart))
				if HAS_NUMPY and N<=10:
					err_xy=np.fabs(xyz[:,0:2]-xyz_back[:,0:2]).max()
					if dim==3:
						err_z=np.fabs(xyz[:,2]-xyz_back[:,2]).max()
					else:
						err_z=0
					if "geo" in label_in[0:6]:
						err_xy/=D2M
					if max(err_xy,err_z)>0.01:
						log_file.write("Maximum tr-loop error: xy: %.10f m z: %.10f m\n" %(err_xy,err_z))
						log_file.write("in: %s\n" %repr(xyz_out))
						log_file.write("back: %s\n" %repr(xyz_back))
			
		if N<=10:
			log_file.write("out:\n%s\n" %repr(xyz_out))
		elif HAS_NUMPY:
			log_file.write("Center of mass: %s\n" %xyz_out.mean(axis=0).tolist())
		if HAS_NUMPY:
			err_xy=np.fabs(xyz[:,0:2]-xyz_back[:,0:2]).max()
			if dim==3:
				err_z=np.fabs(xyz[:,2]-xyz_back[:,2]).max()
			else:
				err_z=0
			if "geo" in label_in[0:6]:
				err_xy/=D2M
			log_file.write("Maximum tr-loop error: xy: %.10f m z: %.10f m\n" %(err_xy,err_z))
		ct.Close()
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
	nerr+=RandomTests(TEST_SYSTEMS_2D,2,N,log_file=sys.stdout)
	nerr+=RandomTests(TEST_SYSTEMS_3D,3,N,log_file=sys.stdout)
	for i in range(3):
		nerr+=RandomTests(FH_TEST,3,N,log_file=sys.stdout)
	print("Errors: %d" %nerr)
	return nerr

if __name__=="__main__":
	sys.exit(main(sys.argv))

	