#!/usr/bin/python
###########################
## Complete python autotestsuite for TrLib 
## Wrapper of several individual tests
## Needs a job-definition file auto_tests.txt for file transformations
## simlk, oct. 2011
############################
import TrLib
import TestFiles
import RandomTests
import ThreadTest
import time,os,sys,platform
JOB_DEF=os.path.join(os.path.dirname(__file__),"auto_tests.txt")
GEOIDS=os.path.join(os.path.dirname(__file__),"Geoids")
LOG_FILE="auto_test"
PROG=os.path.basename(sys.argv[0])
LS="*"*80+"\n"
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

def Usage():
	print("To run:\n%s <shared_library> <geoid_dir>\nLast parameter is optional, will default to: %s" %(PROG,GEOIDS))
	print("Will read the job definition file: %s" %JOB_DEF)
	sys.exit()

def main(args): #progname, test_lib
	if len(args)<2:
		Usage()
	lib=args[1]
	lib_dir=os.path.dirname(lib)
	lib_obj=os.path.basename(lib)
	if len(args)>2:
		geoids=args[2]
	else:
		geoids=GEOIDS
	now=time.asctime()
	nows=now.replace(" ","_").replace(":","_")
	log_name=LOG_FILE+"_"+nows+".log"
	log_fp=open(log_name,"w")
	sys.stdout=RedirectStdout(log_fp)
	sys.stderr=sys.stdout
	nerr=0
	print("Running test suite %s for the KMS transformation library at %s" %(PROG,now))
	print("System is: %s, architecture: %s" %(sys.platform,platform.architecture()))
	print("Initialising library with geoid-dir: %s and shared-library: %s" %(geoids,lib))
	print("Files in geoid-dir: %s" %repr(os.listdir(geoids)))
	TrLib.InitLibrary(geoids,lib_obj,lib_dir)
	print LS
	try:
		nerr+=RandomTests.main(["RandomTests.py","-lib",lib,"-N",10000])
	except Exception,msg:
		print("Error:\n%s" %repr(msg))
		nerr+=1
	print LS
	try:
		TestFiles.main(["TestFiles.py",JOB_DEF,"-lib",lib])
	except Exception,msg:
		nerr+=1
		print("Error:\n%s" %repr(msg))
	print LS
	try:
		ThreadTest.main(["ThreadTest.py","-lib",lib])
	except Exception,msg:
		nerr+=1
		print("Error:\n%s" %repr(msg))
	print LS
	print("Tests finished at %s" %time.asctime())
	print("Errors: %i" %nerr)

if __name__=="__main__":
	sys.exit(main(sys.argv))
	
	