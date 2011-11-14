#!/usr/bin/python
###############################
## Thread safety test using RandomTests.py
## simlk, oct. 2011
###############################
import threading
import RandomTests
import random
import TrLib
import time
import os
import sys
GEOIDS=os.path.join(os.path.dirname(__file__),"Geoids/") #default pointer to geoid directory
OUTPUT_DIR=os.path.join(os.path.dirname(__file__),"THREAD_OUTPUT")
NTHREADS_2D=5
NTHREADS_3D=5
NITERATIONS=3
NPOINTS=1000
class BadGuy(threading.Thread):
	def __init__(self,id,n,iterations=3,is3d=False,log_file=None):
		self.N=n
		self.id=id
		self.is3d=is3d
		self.iterations=iterations
		self.log_file=log_file
		threading.Thread.__init__(self)
	def run(self):
		n=self.N #+something random??
		if self.log_file is None:
			fp=sys.stdout
		else:
			fp=open(self.log_file,"w")
		while self.iterations>0:
			fp.write("This is thread %i\n" %(self.id+1))
			if self.is3d:
				nerr=RandomTests.RandomTests_3D(n,log_file=fp)
			else:
				nerr=RandomTests.RandomTests_2D(n,log_file=fp)
			if nerr>0:
				fp.write("Encountered %i errors!\n" %nerr)
			time.sleep(random.random()*0.1)
			self.iterations-=1
		fp.write("Thread finished\n")
		if self.log_file is not None:
			fp.close()
		

def main(args):
	progname=os.path.basename(args[0])
	IS_INIT=TrLib.IS_INIT
	if not IS_INIT:
		if "-lib" in args: #In this case we assume that input is the library that we want to test....
			libpath=args[args.index("-lib")+1]
			lib=os.path.basename(libpath)
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
	print("Using TrLib v. %s" %TrLib.GetVersion())
	print("Shared library is: %s" %repr(TrLib.tr_lib))
	if not os.path.exists(OUTPUT_DIR):
		os.mkdir(OUTPUT_DIR)
	threads=[]
	RandomTests.SetThreadMode()
	for i in range(NTHREADS_2D):
		threads.append(BadGuy(i,NPOINTS,NITERATIONS,False,os.path.join(OUTPUT_DIR,"2dthread_%i.txt"%i)))
	for i in range(NTHREADS_3D):
		threads.append(BadGuy(NTHREADS_2D+i,NPOINTS,NITERATIONS,True,os.path.join(OUTPUT_DIR,"3dthread_%i.txt"%i)))
	for thread in threads:
		thread.start()
	while threading.activeCount()>1:
		print("Active threads: %i" %threading.activeCount())
		time.sleep(3)
	return 0

if __name__=="__main__":
	sys.exit(main(sys.argv))
	
	
	