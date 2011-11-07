#!/usr/bin/python
###############################
## Thread safety test using RandomTests.py
## simlk, nov. 2011
###############################
import threading
import RandomTests
RandomTests.SetThreadMode()
import random
import TrLib
import time
import os
import sys
import gc
GEOIDS=os.path.join(os.path.dirname(__file__),"Geoids/") #default pointer to geoid directory
KEEP_ALIVE=4
NRUNS=2500 #crashes at some point. Something to do with tls (open file pointers!)
NITERATIONS=2
NPOINTS=1500
LOG_FILE="server_mode"
if "-win32" in sys.argv:
	import ctypes
	class MEMORYSTATUSEX(ctypes.Structure):
		fields_ = [("dwLength", ctypes.c_uint),
		("dwMemoryLoad", ctypes.c_uint)
		("ullTotalPhys", ctypes.c_ulonglong),
		("ullAvailPhys", ctypes.c_ulonglong),
		("ullTotalPageFile", ctypes.c_ulonglong),
		("ullAvailPageFile", ctypes.c_ulonglong),
		("ullTotalVirtual", ctypes.c_ulonglong),
		("ullAvailVirtual", ctypes.c_ulonglong),
		("sullAvailExtendedVirtual", ctypes.c_ulonglong),]
		def __init__(self):
			# have to initialize this to the size of MEMORYSTATUSE
			self.dwLength = 2*4 + 7*8     # size = 2 ints, 7 longs
			return super(MEMORYSTATUSEX, self).__init__()
	WIN32=True
else:
	WIN32=False





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
			fp.write("This is thread %i\n" %self.id)
			if self.is3d:
				nerr=RandomTests.RandomTests_3D(n,log_file=NoOut())
			else:
				nerr=RandomTests.RandomTests_2D(n,log_file=NoOut())
			if nerr>0:
				fp.write("Encountered %i errors!\n" %nerr)
				fp.write("Last error: %d\n" %TrLib.GetLastError())
			time.sleep(random.random()*0.1)
			self.iterations-=1
		fp.write("Thread %i finished\n" %self.id)
		if self.log_file is not None:
			fp.close()
		TrLib.tr_lib.TerminateLibrary()
		

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
	now=time.asctime()
	nows=now.replace(" ","_").replace(":","_")
	log_name=LOG_FILE+"_"+nows+".log"
	log_fp=open(log_name,"w")
	sys.stdout=RedirectStdout(log_fp)
	sys.stderr=sys.stdout
	print("Running %s at %s" %(progname,time.asctime()))
	print("Using TrLib v. %s" %TrLib.GetVersion())
	print("Shared library is: %s" %repr(TrLib.tr_lib))
	finished_threads=0
	while finished_threads<NRUNS:
		if threading.activeCount()<KEEP_ALIVE:
			print("Active threads: %i" %threading.activeCount())
			finished_threads+=1
			new_thread=BadGuy(finished_threads,NPOINTS,NITERATIONS,False,None)
			new_thread.start()
			new_thread.join()
			if WIN32:
				stat = MEMORYSTATUSEX()
				ctypes.windll.kernel32.GlobalMemoryStatusEx(ctypes.byref(stat))
				print "Avail. mem:",stat.ullAvailPhys
				del new_thread
	while threading.activeCount()>1:
		print("Active threads: %i" %threading.activeCount())
		time.sleep(2)
	return 0


if __name__=="__main__":
	sys.exit(main(sys.argv))