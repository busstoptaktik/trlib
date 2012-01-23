#!/usr/bin/python
###############################
## Thread safety test using RandomTests.py
## simlk, nov. 2011
###############################
import threading
import RandomTests
import random
import TrLib
import time
import os
import sys
import gc
import random
#This is the minimal test to pass!#
KEEP_ALIVE=8
NRUNS=500 
NITERATIONS=30
NPOINTS=30
LOG_FILE="server_mode"
def tracer(frame,event,arg):
	if event=="call":
		print "call",frame.f_lineno,frame.f_code
	if event=="line":
		print "line",frame.f_lineno,frame.f_code
	
class RedirectStdout(object):
	def __init__(self,fp=None):
		self.fp=fp
	def write(self,text):
		sys.__stdout__.write(text)
		if self.fp is not None:
			try:
				self.fp.write(text)
			except:
				pass
				
class NoOut(object):
	def write(self,*args):
		pass
		
class BadGuy(threading.Thread):
	def __init__(self,id,n,iterations=3,mode=0,log_file=None,flag=None):
		self.N=n
		self.id=id
		self.mode=mode
		self.iterations=iterations
		self.log_file=log_file
		self.flag=flag
		threading.Thread.__init__(self)
		self.name="Thread %d" %id
	def run(self):
		tstart=time.clock()
		if self.log_file is None:
			fp=sys.stdout
		else:
			fp=open(self.log_file,"w")
		if self.flag is not None:
			self.flag.clear()
		fp.write("Hello world! This is thread %d. I perform %dD-tests. Mode is: %d" %(self.id,int(self.mode>0)+2,self.mode))
		if self.mode!=3: #turn up the heat! Test having an open transformation in between lots of other stuff
			ct=TrLib.CoordinateTransformation("utm32Hwgs84_h_dvr90","geoHed50_h_dvr90")
			fp.write(" *** %d ***" %self.id)
			x,y,z=ct.Transform(512200.1,6143200.1,100.0)
			fp.write(" %d: 1st done ***\n" %self.id)
		n=self.N #+something random??
		while self.iterations>0:
			fp.write("This is thread %d, todo: %d. I perform %dD-tests. Mode is: %d\n" %(self.id,self.iterations,int(self.mode>0)+2,self.mode))
			if self.mode==0:
				nerr=RandomTests.RandomTests_3D(n,log_file=NoOut())
			elif self.mode==1:
				nerr=RandomTests.RandomTests_2D(n,log_file=NoOut())
			else:
				nerr=RandomTests.RandomTests(RandomTests.FH_TEST,3,n,log_file=NoOut())
			if nerr>0:
				fp.write("Thread %d, mode %d, encountered %d errors!\n" %(self.id,self.mode,nerr))
			time.sleep(0.01)
			self.iterations-=1
		if self.mode!=3:
			x,y,z=ct.Transform(512200.1,6143200.1,100.0)
			ct.Close()
		if self.mode==3:
			time.sleep(0.01)
		TrLib.TerminateThread()
		if self.mode==3:
			time.sleep(0.01)
		if self.flag is not None:
			self.flag.set()
		tend=time.clock()
		fp.write("Thread %i finished. Running time %.2f s\n" %(self.id,tend-tstart))
		if self.log_file is not None:
			fp.close()
		return 
		
		

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
			IS_INIT=TrLib.InitLibrary()
	if not IS_INIT:
		print("Could not initialize library...")
		print("Find a proper shared library and geoid dir and try again!")
		return -1
	if "-trace" in args:
		threading.settrace(tracer)
	RandomTests.SetThreadMode()
	now=time.asctime()
	nows=now.replace(" ","_").replace(":","_")
	log_name=LOG_FILE+"_"+nows+".log"
	log_fp=open(log_name,"w")
	sys.stdout=RedirectStdout(log_fp)
	sys.stderr=sys.stdout
	if "-nruns" in args:
		nruns=int(args[args.index("-nruns")+1])
	else:
		nruns=NRUNS
	if "-npoints" in args:
		npoints=int(args[args.index("-npoints")+1])
	else:
		npoints=NPOINTS
	print("Running %s at %s" %(progname,time.asctime()))
	print("Using TrLib v. %s" %TrLib.GetVersion())
	print("Shared library is: %s" %repr(TrLib.tr_lib))
	finished_threads=0
	nfemern=0
	FH_flag=threading.Event()
	FH_flag.set()
	while finished_threads<nruns:
		if threading.activeCount()<KEEP_ALIVE:
			print("Active threads: %i" %threading.activeCount())
			finished_threads+=1
			rn=random.random()
			flag=None
			if rn<0.4:
				mode=0
			elif "-fehmarn" in args:
				if rn<0.75:
					mode=1
				else: #then start Fehmarn thread - which is problematic!
					FH_flag.wait()
					mode=3
					flag=FH_flag
					nfemern+=1
			else:
				mode=1
			new_thread=BadGuy(finished_threads,npoints,NITERATIONS,mode,None,flag)
			new_thread.start()
			if nfemern>0 and nfemern%10==0:
				print("%d Fehmarn threads started." %nfemern)
	slept=0
	while threading.activeCount()>1:
		print("\nActive threads: %i\n" %threading.activeCount())
		if slept>4:
			for thread in threading.enumerate():
				print("%s might be a zombie!" %thread.name)
		time.sleep(2)
		slept+=2
	return 0


if __name__=="__main__":
	sys.exit(main(sys.argv))